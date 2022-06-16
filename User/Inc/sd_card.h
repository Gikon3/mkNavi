#ifndef INC_SD_CARD_H_
#define INC_SD_CARD_H_

#include "stm32f4xx.h"
#include "spi.h"
#include "fatfs.h"
#include "FreeRTOS.h"
#include "queue.h"

typedef enum
{
  sdNotInitialized,
  sdReady,
  sdMountFail
} SD_CardState;

typedef enum
{
  sdCategoryNone = 0,
  sdCategorySensors = 1,
  sdCategoryJetson = 2,
  sdCategoryPPM = 4,
  sdCategoryGNSS = 8
} SD_CardCategory;

typedef struct
{
  Bool          inWork;
  Bool          isOpen;
  uint8_t       categories;
  uint8_t       repeatNo;
  FIL           file;
  char          *filename;
  uint32_t      threshold;
  QueueHandle_t queue;
} SD_CardStream;

typedef struct
{
  struct
  {
    SPI_IF      *spiIf;
  } hardware;
  SD_CardState  state;
  FATFS         fs;
  SD_CardStream stream[3];
} SD_Card;

SD_Card* sd_instance();
void sd_config(SD_Card *sd, SPI_IF *spiIF);
HAL_StatusTypeDef sd_is_ready();
int8_t sd_start(SD_Card *sd, char const *filename, uint8_t categories);
void sd_stop(SD_Card *sd, int8_t handle);
void sd_set_categories(SD_Card *sd, int8_t handle, uint8_t categories);
void sd_push(SD_Card *sd, String const *str, SD_CardCategory category);
void sd_write_loop(SD_Card *sd);
char const* sd_filename(SD_Card const *sd, int8_t handle);
size_t sd_file_size(SD_Card const *sd, int8_t handle);
size_t sd_file_size_in_text(SD_Card const *sd, int8_t handle, char *text, size_t size);

#endif /* INC_SD_CARD_H_ */
