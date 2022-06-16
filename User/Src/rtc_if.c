#include "rtc_if.h"
#include "cmsis_os.h"
#include "rtc.h"

RTC_IF rtcIF = {
    .ti.startTicks = 0,
    .ti.zeroYear = 1980,
    .date.day = 1,
    .date.month = 1,
    .date.year = 41,
    .time.msec = 0,
    .time.sec = 0,
    .time.min = 0,
    .time.hour = 0
};

void rtc_datetime(RTC_IF* rtc)
{
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;
  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
  rtc->date.day = date.Date;
  rtc->date.month = date.Month;
  rtc->date.year = date.Year + rtc->ti.zeroYear;
  const uint32_t currentTicks = xTaskGetTickCount();
  rtc->time.msec = ((currentTicks - rtc->ti.startTicks) % 1000) / portTICK_PERIOD_MS;
  rtc->time.sec = time.Seconds;
  rtc->time.min = time.Minutes;
  rtc->time.hour = time.Hours;
}

void rtc_sync_datetime(RTC_IF* rtc)
{
  RTC_TimeTypeDef time;
  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
  time.Hours = rtc->time.hour;
  time.Minutes = rtc->time.min;
  time.Seconds = rtc->time.sec;
  time.SubSeconds = 0;
  rtc->ti.startTicks = xTaskGetTickCount();
  HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);

  RTC_DateTypeDef date;
  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
  date.Year = rtc->date.year - rtc->ti.zeroYear;
  date.Month = rtc->date.month;
  date.Date = rtc->date.day;
  HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
}

uint32_t rtc_datetime32(RTC_IF const* rtc)
{
  const uint32_t correctYear = rtc->ti.zeroYear - 1980;
  RTC_DateTypeDef date;
  RTC_TimeTypeDef time;
  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
  uint32_t dateTime32 = 0;
  dateTime32 |= (date.Year + correctYear) << 25;
  dateTime32 |= (date.Month & 0xF) << 21;
  dateTime32 |= (date.Date & 0x1F) << 16;
  dateTime32 |= (time.Hours & 0xF) << 11;
  dateTime32 |= (time.Minutes & 0x1F) << 5;
  dateTime32 |= time.Seconds & 0x1F;
  return dateTime32;
}
