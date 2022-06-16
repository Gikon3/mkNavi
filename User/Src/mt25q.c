#include "spi.h"
#include "main.h"
#include "util_heap.h"
#include "mt25q.h"

#define MT25Q_MANUFACTURER_ID   0x20
#define MT25Q_SECTOR_SIZE       (64*1024)
#define MT25Q_SUBSECTOR32_SIZE  (32*1024)
#define MT25Q_SUNSECTOR4_SIZE   (4*1024)
#define MT25Q_PAGE_SIZE         256

static MT25Q mt25q;

static void wait_ready(MT25Q *dev)
{
  MT25Q_SR sr = mt25q_get_sr(dev);
  while(sr.writeInProgress == 1)
    sr = mt25q_get_sr(dev);
}

static void send(MT25Q *dev, MT25Q_Cmd command, uint32_t address, size_t addressSize, uint8_t *data, size_t dataSize)
{
  xSemaphoreTake(dev->mut, portMAX_DELAY);
  SpiQNode node;
  node.spi = dev->hardware.spiIF;
  node.type = typeTx;
  node.cmd = command;
  node.address = address;
  node.addressSize = addressSize;
  node.data = data;
  node.dataSize = dataSize;
  node.ready = dev->hardware.sem;
  xQueueSendToBack(dev->hardware.spiIF->queue, &node, portMAX_DELAY);
  xSemaphoreTake(node.ready, portMAX_DELAY);
  xSemaphoreGive(dev->mut);
}

static void receive(MT25Q *dev, MT25Q_Cmd command, uint32_t address, size_t addressSize, uint8_t *data, size_t dataSize)
{
  xSemaphoreTake(dev->mut, portMAX_DELAY);
  SpiQNode node;
  node.spi = dev->hardware.spiIF;
  node.type = typeRx;
  node.cmd = command;
  node.address = address;
  node.addressSize = addressSize;
  node.data = data;
  node.dataSize = dataSize;
  node.ready = dev->hardware.sem;
  xQueueSendToBack(dev->hardware.spiIF->queue, &node, portMAX_DELAY);
  xSemaphoreTake(node.ready, portMAX_DELAY);
  xSemaphoreGive(dev->mut);
}

void mt25q_spi_send(SPI_IF *spiIF, MT25Q_Cmd command, uint32_t address, size_t addressSize, uint8_t *data, size_t dataSize)
{
  spi_cs(spiIF);

  spi_tx(spiIF, (uint8_t*)&command, 1);
  if(addressSize > 0)
    spi_tx(spiIF, (uint8_t*)&address, addressSize);
  if(dataSize > 0) {
    spi_tx_dma(spiIF, data, dataSize);
    spi_wait_clear(spiIF);
    util_free(data);
  }

  spi_decs(spiIF);
}

void mt25q_spi_receive(SPI_IF *spiIF, MT25Q_Cmd command, uint32_t address, size_t addressSize, uint8_t *data, size_t dataSize)
{
  spi_cs(spiIF);

  spi_tx(spiIF, (uint8_t*)&command, 1);
  if(addressSize > 0)
    spi_tx(spiIF, (uint8_t*)&address, addressSize);
  if(dataSize > 0)
    spi_rx(spiIF, data, dataSize);

  spi_decs(spiIF);
}

MT25Q* mt25q_instance()
{
  return &mt25q;
}

void mt25q_init(MT25Q *dev)
{
  dev->hardware.holdPort = FLASH_HOLD_GPIO_Port;
  dev->hardware.holdPin = FLASH_HOLD_Pin;
  dev->hardware.wpPort = FLASH_WP_GPIO_Port;
  dev->hardware.wpPin = FLASH_WP_Pin;
  dev->hardware.spiIF = &spiIF_MT25Q;
}

void mt25q_config(MT25Q *dev)
{
  HAL_GPIO_WritePin(dev->hardware.wpPort, dev->hardware.wpPin, SET);
  HAL_GPIO_WritePin(dev->hardware.holdPort, dev->hardware.holdPin, SET);

  dev->hardware.sem = xSemaphoreCreateBinary();
  xSemaphoreGive(dev->hardware.sem);

  dev->mut = xSemaphoreCreateMutex();

  receive(dev, mt25qCmdReadID, 0, 0, (uint8_t*)&dev->deviceID_Data, sizeof(dev->deviceID_Data));
}

HAL_StatusTypeDef mt25q_is_ready(MT25Q *dev)
{
  MT25Q_DeviceID_Data idData;
  receive(dev, mt25qCmdReadID, 0, 0, (uint8_t*)&idData, sizeof(idData));
  if(idData.manufacturerID == MT25Q_MANUFACTURER_ID)
    return HAL_OK;
  return HAL_ERROR;
}

void mt25q_read(MT25Q *dev, uint32_t address, uint8_t *data, size_t dataSize)
{
  wait_ready(dev);
  receive(dev, mt25qCmdRead, address, 3, data, dataSize);
}

void mt25q_write(MT25Q *dev, uint32_t address, uint8_t *data, size_t dataSize)
{
  wait_ready(dev);

  while(dataSize > 0) {
    const uint32_t alignAddress = address + (MT25Q_PAGE_SIZE - (address + MT25Q_PAGE_SIZE) % MT25Q_PAGE_SIZE);
    const uint32_t deltaAddress = alignAddress - address;
    const size_t sendDataSize = dataSize > deltaAddress ? deltaAddress : dataSize;

    send(dev, mt25qCmdWriteEnable, 0, 0, NULL, 0);
    send(dev, mt25qCmdPageProgram, address, 3, data, sendDataSize);

    address += sendDataSize;
    dataSize -= sendDataSize;
    data += sendDataSize;
  }
}

void mt25q_erase_subsector_4kb(MT25Q *dev, uint32_t address)
{
  wait_ready(dev);
  send(dev, mt25qCmdWriteEnable, 0, 0, NULL, 0);
  send(dev, mt25qCmdEraseSub4KiB, address, 3, NULL, 0);
}

MT25Q_SR mt25q_get_sr(MT25Q *dev)
{
  MT25Q_SR reg;
  receive(dev, mt25qCmdReadSR, 0, 0, (uint8_t*)&reg, sizeof(reg));
  return reg;
}

MT25Q_FlagSR mt25q_get_flag_sr(MT25Q *dev)
{
  MT25Q_FlagSR reg;
  receive(dev, mt25qCmdReadFlagSR, 0, 0, (uint8_t*)&reg, sizeof(reg));
  return reg;
}
