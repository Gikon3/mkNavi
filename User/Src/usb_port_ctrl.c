#include "usb_port_ctrl.h"
#include "i2c.h"

#define ID_SIZE   8

static I2C_IF* i2cIf;
static char id[ID_SIZE];

static void usbp_ctrl_init()
{
  i2cIf = &i2cIF_USB;
}

HAL_StatusTypeDef usbp_ctrl_is_ready()
{
  uint8_t isReady;
  I2cQNode node;
  node.i2c = i2cIf;
  node.type = typeIsReady;
  node.address = 0;
  node.data = &isReady;
  node.dataSize = sizeof(isReady);
  node.ready = xSemaphoreCreateBinary();
  xQueueSendToBack(i2cIf->queue, &node, portMAX_DELAY);
  xSemaphoreTake(node.ready, portMAX_DELAY);
  vSemaphoreDelete(node.ready);
  return isReady;
}

void usbp_ctrl_config()
{
  usbp_ctrl_init();
  if (usbp_ctrl_is_ready() != HAL_OK) {
    return;
  }

  usbp_ctrl_rd_id(id, sizeof(id));
}

void usbp_ctrl_wr(uint8_t address, uint8_t* data, uint16_t size)
{
  I2cQNode node;
  node.i2c = i2cIf;
  node.type = typeTx;
  node.address = address;
  node.data = data;
  node.dataSize = size;
  node.ready = NULL;
  xQueueSendToBack(i2cIf->queue, &node, portMAX_DELAY);
}

void usbp_ctrl_rd(uint8_t address, uint8_t* data, uint16_t size)
{
  I2cQNode node;
  node.i2c = i2cIf;
  node.type = typeRx;
  node.address = address;
  node.data = data;
  node.dataSize = size;
  node.ready = xSemaphoreCreateBinary();
  xQueueSendToBack(i2cIf->queue, &node, portMAX_DELAY);
  xSemaphoreTake(node.ready, portMAX_DELAY);
  vSemaphoreDelete(node.ready);
}

void usbp_ctrl_wr_word(uint8_t address, uint8_t data)
{
  uint8_t* txData = pvPortMalloc(1);
  *txData = data;
  usbp_ctrl_wr(address, txData, 1);
}

uint8_t usbp_ctrl_rd_word(uint8_t address)
{
  uint8_t rxData;
  usbp_ctrl_rd(address, &rxData, 1);
  return rxData;
}

void usbp_ctrl_rd_id(char* str, uint16_t size)
{
  static const uint16_t idSize = 8;
  uint16_t localSize = size < idSize ? size - 1 : idSize;
  usbp_ctrl_rd(USBP_REG00, (uint8_t*)str, localSize);
  str[size - 1] = '\0';
}

char* usbp_ctrl_get_id() { return id; }

void usbp_ctrl_clear_int()
{
  uint8_t reg08;
  usbp_ctrl_rd(USBP_REG08, &reg08, sizeof(reg08));
  reg08 |= 0x10;
  usbp_ctrl_wr_word(USBP_REG08, reg08);
}
