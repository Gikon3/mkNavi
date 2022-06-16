#include <stdio.h>
#include <string.h>
#include "user_diskio_spi.h"
#include "rtc_if.h"
#include "util_heap.h"
#include "flow_ctrl.h"
#include "sd_card.h"
#include "station.h"
#include "sensor.h"

#define TIME_FORMAT   "%02d/%02d/%04d %02d:%02d:%02d.%03d"

static SD_Card sdCard;

SD_Card* sd_instance()
{
  return &sdCard;
}

void sd_config(SD_Card *sd, SPI_IF *spiIF)
{
  sd->hardware.spiIf = spiIF;
  sd->state = sdNotInitialized;
  for(uint8_t i = 0; i < sizeof(sd->stream)/sizeof(sd->stream[0]); ++i) {
    sd->stream[i].inWork = bFalse;
    sd->stream[i].isOpen = bFalse;
    sd->stream[i].categories = sdCategoryNone;
    sd->stream[i].repeatNo = 0;
    sd->stream[i].filename = NULL;
    sd->stream[i].threshold = 10485760;  //< 10 MiB
    sd->stream[i].queue = xQueueCreate(8, sizeof(String));
  }

  const FRESULT result = f_mount(&sd->fs, "", 1);
  sd->state = result == FR_OK ? sdReady : sdMountFail;
}

HAL_StatusTypeDef sd_is_ready()
{
  if(USER_SPI_status(0))
    return HAL_ERROR;
  return HAL_OK;
}

int8_t sd_start(SD_Card *sd, char const *filename, uint8_t categories)
{
  int8_t handle = -1;
  if(sd->state == sdReady) {
    for(uint8_t i = 0; i < sizeof(sd->stream)/sizeof(sd->stream[0]); ++i) {
      if(sd->stream[i].inWork == bFalse) {
        handle = i;
        sd->stream[i].inWork = bTrue;
        sd->stream[i].isOpen = bFalse;
        sd->stream[i].categories = categories;
        sd->stream[i].repeatNo = 0;
        sd->stream[i].filename = util_malloc(strlen(filename)+1);
        strcpy(sd->stream[i].filename, filename);
        break;
      }
    }
  }
  return handle;
}

void sd_stop(SD_Card *sd, int8_t handle)
{
  const uint8_t handleNumber = sizeof(sd->stream) / sizeof(sd->stream[0]);
  if(handle >= 0 && handle < handleNumber && sd->stream[handle].inWork == bTrue) {
    SD_CardStream *stream = &sd->stream[handle];
    stream->inWork = bFalse;
    if(stream->isOpen == bTrue) {
      f_close(&stream->file);
      stream->isOpen = bFalse;
    }
    util_free(stream->filename);
    stream->filename = NULL;
  }
}

void sd_set_categories(SD_Card *sd, int8_t handle, uint8_t categories)
{
  const uint8_t handleNumber = sizeof(sd->stream) / sizeof(sd->stream[0]);
  if(handle >= 0 && handle < handleNumber && sd->stream[handle].inWork == bTrue)
    sd->stream[handle].categories = categories;
}

void sd_push(SD_Card *sd, String const *str, SD_CardCategory category)
{
  if(sd->state != sdReady)
      return;

  const uint8_t handleNumber = sizeof(sd->stream) / sizeof(sd->stream[0]);
  for(uint8_t i = 0; i < handleNumber; ++i) {
    if((sd->stream[i].categories & category) == category) {
      String spyStr = str_copy(str);
      xQueueSendToBack(sd->stream[i].queue, &spyStr, portMAX_DELAY);
    }
  }
}

static char* choose_new_filename(char const *filename, uint8_t *no)
{
  const size_t len = strlen(filename);
  char const *dot = strrchr(filename, '.');
  char const *parenthesisLeft = NULL;
  char const *parenthesisRight = NULL;
  if(dot && *(dot-1) == ')')
    parenthesisRight = dot - 1;
  else if(filename[len-1] == ')')
    parenthesisRight = &filename[len-1];
  int32_t noInFilename = 0;
  if(parenthesisRight) {
    parenthesisLeft = strrchr(filename, '(');
    if(parenthesisLeft)
      sscanf(parenthesisLeft,"(%ld)", &noInFilename);
  }

  const size_t filenameLen = parenthesisLeft && noInFilename > 0 ? parenthesisLeft - filename :
      dot ? dot - filename : len;
  const size_t filenameExtensionLen = dot ? strlen(dot) : 0;
  noInFilename = noInFilename > 0 ? noInFilename + 1 : 1;
  const size_t needMem = filenameLen + snprintf(NULL, 0, "(%ld)", noInFilename) + filenameExtensionLen + sizeof("");
  char *newFilename = util_malloc(needMem);
  memcpy(newFilename, filename, filenameLen);
  snprintf(&newFilename[filenameLen], needMem-filenameLen, "(%ld)%s", noInFilename, dot);

  if(no)
    *no = noInFilename;

  return newFilename;
}

static void open_file_if_necessary(SD_CardStream *stream)
{
  if(stream->isOpen == bTrue)
    return;

  FRESULT result = f_stat(stream->filename, NULL);
  while(result != FR_NO_FILE) {
    char *newFilename = choose_new_filename(stream->filename, &stream->repeatNo);
    if(newFilename) {
      util_free(stream->filename);
      stream->filename = newFilename;
    }
    result = f_stat(stream->filename, NULL);
  }
  result = f_open(&stream->file, stream->filename, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
  if(result == FR_OK)
    stream->isOpen = bTrue;
}

static void close_file_if_necessary(SD_CardStream *stream)
{
  if(stream->isOpen == bFalse)
    return;

  if(f_size(&stream->file) > stream->threshold) {
    f_close(&stream->file);
    stream->isOpen = bFalse;
  }
}

static FRESULT write_to_file(SD_CardStream *stream, char const *str, size_t len)
{
  open_file_if_necessary(stream);
  const FRESULT result = f_write(&stream->file, str, len, NULL);
  close_file_if_necessary(stream);
  return result;
}

static void sensor_write(SD_CardStream *stream)
{
  rtc_datetime(&rtcIF);

  char accum[200];
  const size_t len = snprintf(accum, sizeof(accum), TIME_FORMAT"\r\n"
      "acc %8.4f g, %8.4f g, %8.4f g\r\n"
      "gyr %10.4f dps, %10.4f dps, %10.4f dps\r\n"
      "mag %8.4f G, %8.4f G, %8.4f G\r\n"
      "bar %6.0f Pa\r\n",
      rtcIF.date.day, rtcIF.date.month, rtcIF.date.year,
      rtcIF.time.hour, rtcIF.time.min, rtcIF.time.sec, rtcIF.time.msec,
      sens.acc.ripe.x, sens.acc.ripe.y, sens.acc.ripe.z,
      sens.gyr.ripe.x, sens.gyr.ripe.y, sens.gyr.ripe.z,
      sens.mag.ripe.x, sens.mag.ripe.y, sens.mag.ripe.z,
      sens.bar.ripe.val);

  write_to_file(stream, accum, len);
}

static void jetson_solution_write(SD_CardStream *stream, Jet const *jetson)
{
  rtc_datetime(&rtcIF);

  char accum[200];
  const size_t len = snprintf(accum, sizeof(accum), TIME_FORMAT"\r\n"
      "Jetson Solution\r\n"
      "  Tim %llu\r\n"
      "  Lat %17.8f\r\n"
      "  Lng %17.8f\r\n"
      "  Sys %02X\r\n",
      rtcIF.date.day, rtcIF.date.month, rtcIF.date.year,
      rtcIF.time.hour, rtcIF.time.min, rtcIF.time.sec, rtcIF.time.msec,
      jetson->solution.time,
      jetson->solution.coord.lat,
      jetson->solution.coord.lng,
      *(uint16_t*)&jetson->solution.system);

  write_to_file(stream, accum, len);
}

static void jetson_bluetooth_write(SD_CardStream *stream, Jet const *jetson)
{
  rtc_datetime(&rtcIF);

  char accum[200];
  const uint32_t uuid[4] = {
      (uint32_t)jetson->blu.uuid[0] << 24 | (uint32_t)jetson->blu.uuid[1] << 16 |
      (uint32_t)jetson->blu.uuid[2] << 8 | (uint32_t)jetson->blu.uuid[3],
      (uint32_t)jetson->blu.uuid[4] << 24 | (uint32_t)jetson->blu.uuid[5] << 16 |
      (uint32_t)jetson->blu.uuid[6] << 8 | (uint32_t)jetson->blu.uuid[7],
      (uint32_t)jetson->blu.uuid[8] << 24 | (uint32_t)jetson->blu.uuid[9] << 16 |
      (uint32_t)jetson->blu.uuid[10] << 8 | (uint32_t)jetson->blu.uuid[11],
      (uint32_t)jetson->blu.uuid[12] << 24 | (uint32_t)jetson->blu.uuid[13] << 16 |
      (uint32_t)jetson->blu.uuid[14] << 8 | (uint32_t)jetson->blu.uuid[15]
  };
  const size_t len = snprintf(accum, sizeof(accum), TIME_FORMAT"\r\n"
      "Jetson Bluetooth\r\n"
      "  Tim %llu\r\n"
      "  Major %lu\r\n"
      "  Minor %lu\r\n"
      "  UUID %08lX-%08lX-%08lX-%08lX\r\n"
      "  RSSI %d\r\n",
      rtcIF.date.day, rtcIF.date.month, rtcIF.date.year,
      rtcIF.time.hour, rtcIF.time.min, rtcIF.time.sec, rtcIF.time.msec,
      jetson->blu.time,
      jetson->blu.major,
      jetson->blu.minor,
      uuid[0], uuid[1], uuid[2], uuid[3],
      jetson->blu.rssi);

  write_to_file(stream, accum, len);
}

static void jetson_wifi_write(SD_CardStream *stream, Jet const *jetson)
{
  rtc_datetime(&rtcIF);

  char accum[200];
  uint8_t const* mac = jetson->wifi.mac;
  const size_t len = snprintf(accum, sizeof(accum), TIME_FORMAT"\r\n"
      "Jetson WiFi\r\n"
      "  Tim %llu\r\n"
      "  MAC %02X-%02X-%02X-%02X-%02X-%02X\r\n"
      "  Distance %f\r\n",
      rtcIF.date.day, rtcIF.date.month, rtcIF.date.year,
      rtcIF.time.hour, rtcIF.time.min, rtcIF.time.sec, rtcIF.time.msec,
      jetson->wifi.time,
      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
      jetson->wifi.distance);

  write_to_file(stream, accum, len);
}

static void jetson_gps_write(SD_CardStream *stream, Jet const *jetson)
{
  rtc_datetime(&rtcIF);

  char accum[200];
  const size_t len = snprintf(accum, sizeof(accum), TIME_FORMAT"\r\n"
      "Jetson GPS\r\n"
      "  Tim %llu\r\n"
      "  Lat %17.8f\r\n"
      "  Lng %17.8f\r\n",
      rtcIF.date.day, rtcIF.date.month, rtcIF.date.year,
      rtcIF.time.hour, rtcIF.time.min, rtcIF.time.sec, rtcIF.time.msec,
      jetson->gps.time,
      jetson->gps.coord.lat,
      jetson->gps.coord.lng);

  write_to_file(stream, accum, len);
}

static void jetson_write(SD_CardStream *stream, Jet *jetson)
{
  if(jetson->solution.updated == bTrue) {
    jetson->solution.updated = bFalse;
    jetson_solution_write(stream, jetson);
  }
  if(jetson->blu.updated == bTrue) {
    jetson->blu.updated = bFalse;
    jetson_bluetooth_write(stream, jetson);
  }
  if(jetson->wifi.updated == bTrue) {
    jetson->wifi.updated = bFalse;
    jetson_wifi_write(stream, jetson);
  }
  if(jetson->gps.updated == bTrue) {
    jetson->gps.updated = bFalse;
    jetson_gps_write(stream, jetson);
  }
}

static void other_write(SD_CardStream *stream)
{
  String str;
  while(xQueueReceive(stream->queue, &str, 0) == pdTRUE) {
    rtc_datetime(&rtcIF);

    open_file_if_necessary(stream);
    f_printf(&stream->file, TIME_FORMAT"\r\n%s",
        rtcIF.date.day, rtcIF.date.month, rtcIF.date.year,
        rtcIF.time.hour, rtcIF.time.min, rtcIF.time.sec, rtcIF.time.msec,
        str_cdata(&str));
    close_file_if_necessary(stream);

    str_clear(&str);
  }
}

void sd_write_loop(SD_Card *sd)
{
  if(sd->state != sdReady)
    return;

  for(uint8_t i = 0; i < sizeof(sd->stream)/sizeof(sd->stream[0]); ++i) {
    if(sd->stream[i].inWork == bTrue) {
      if((sd->stream[i].categories & sdCategorySensors) == sdCategorySensors)
        sensor_write(&sd->stream[i]);
      if((sd->stream[i].categories & sdCategoryJetson) == sdCategoryJetson)
        jetson_write(&sd->stream[i], &station.jet);
      other_write(&sd->stream[i]);
    }
  }
}

char const* sd_filename(SD_Card const *sd, int8_t handle)
{
  const uint8_t handleNumber = sizeof(sd->stream) / sizeof(sd->stream[0]);
  if(handle >= 0 && handle < handleNumber)
    return sd->stream[handle].filename;
  return NULL;
}

size_t sd_file_size(SD_Card const *sd, int8_t handle)
{
  const uint8_t handleNumber = sizeof(sd->stream) / sizeof(sd->stream[0]);
  if(handle >= 0 && handle < handleNumber)
    return f_size(&sd->stream[handle].file);
  return 0;
}

size_t sd_file_size_in_text(SD_Card const *sd, int8_t handle, char *text, size_t size)
{
  const size_t fileSize = sd_file_size(sd, handle);
  float sizeScience = fileSize;
  char const *prefix = "B";
  if(fileSize >= (1024*1024*1024)) {
    sizeScience /= (float)(1024 * 1024 * 1024);
    prefix = "GiB";
  }
  else if(fileSize >= (1024*1024)) {
    sizeScience /= (float)(1024 * 1024);
    prefix = "MiB";
  }
  else if(fileSize >= 1024) {
    sizeScience /= 1024.0;
    prefix = "KiB";
  }

  snprintf(text, size, "%.3f %s", sizeScience, prefix);

  return fileSize;
}
