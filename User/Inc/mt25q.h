#ifndef INC_MT25Q_H_
#define INC_MT25Q_H_

#include "stm32f4xx.h"
#include "util_types.h"
#include "spi.h"

typedef enum {
  mt25qCmdResetEnable = 0x66,
  mt25qCmdResetMemory = 0x99,
  mt25qCmdReadID = 0x9E,
  mt25qCmdRead = 0x03,
  mt25qCmdWriteEnable = 0x06,
  mt25qCmdReadSR = 0x05,
  mt25qCmdReadFlagSR = 0x70,
  mt25qCmdPageProgram = 0x02,
  mt25qCmdEraseSub32KiB = 0x52,
  mt25qCmdEraseSub4KiB = 0x20,
  mt25qCmdEraseSector = 0xD8,
  mt25qCmdEraseBulk = 0xC7
} MT25Q_Cmd;

typedef enum {
  mt25qMem3V = 0xBA,
  mt25qMem18V = 0xBB
} MT25Q_MemoryType;

typedef enum {
  mt25qMem2Gb = 0x22,
  mt25qMem1Gb = 0x21,
  mt25qMem512Mb = 0x20,
  mt25qMem256Mb = 0x19,
  mt25qMem128Mb = 0x18,
  mt25qMem64Mb = 0x17
} MT25Q_MemoryCapacity;

typedef enum {
  mt25qConfStandart = 0x00
} MT25Q_ConfigInfo;

typedef struct
{
  uint8_t               manufacturerID;
  MT25Q_MemoryType      type;
  MT25Q_MemoryCapacity  capacity;
  uint8_t               remainingID_Bytes;
  uint8_t               extendedDeviceID;
  MT25Q_ConfigInfo      configInfo;
  uint8_t               uid[14];
} MT25Q_DeviceID_Data;

typedef struct
{
  struct
  {
    GPIO_TypeDef      *holdPort;
    uint16_t          holdPin;
    GPIO_TypeDef      *wpPort;
    uint16_t          wpPin;
    SPI_IF            *spiIF;
    osSemaphoreId_t   sem;        //< Ожидание отпраки сообщения SPI
  } hardware;
  osMutexId_t         mut;
  MT25Q_DeviceID_Data deviceID_Data;
} MT25Q;

typedef struct
{
  uint8_t writeInProgress : 1;
  uint8_t writeEnable     : 1;
  uint8_t bp20            : 3;
  uint8_t topBottom       : 1;
  uint8_t bp3             : 1;
  uint8_t srWriteEn       : 1;
} MT25Q_SR;

typedef struct
{
  uint8_t                 : 1;
  uint8_t protection      : 1;
  uint8_t programSuspend  : 1;
  uint8_t                 : 1;
  uint8_t program         : 1;
  uint8_t erase           : 1;
  uint8_t eraseSuspend    : 1;
  uint8_t prOrErControl   : 1;
} MT25Q_FlagSR;

void mt25q_spi_send(SPI_IF *spiIF, MT25Q_Cmd command, uint32_t address, size_t addressSize, uint8_t *data, size_t dataSize);
void mt25q_spi_receive(SPI_IF *spiIF, MT25Q_Cmd command, uint32_t address, size_t addressSize, uint8_t *data, size_t dataSize);

MT25Q* mt25q_instance();
void mt25q_init(MT25Q *dev);
void mt25q_config(MT25Q *dev);
HAL_StatusTypeDef mt25q_is_ready(MT25Q *dev);
void mt25q_read(MT25Q *dev, uint32_t address, uint8_t *data, size_t dataSize);
void mt25q_write(MT25Q *dev, uint32_t address, uint8_t *data, size_t dataSize);
void mt25q_erase_subsector_4kb(MT25Q *dev, uint32_t address);
MT25Q_SR mt25q_get_sr(MT25Q *dev);
MT25Q_FlagSR mt25q_get_flag_sr(MT25Q *dev);

#endif /* INC_MT25Q_H_ */
