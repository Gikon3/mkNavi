#include "clk_osc.h"
#include "i2c.h"
#include "main.h"

static I2C_IF* i2cIF;

static float ppmList[] = {
    6.25, 10.0, 12.5, 25.0, 50.0, 80.0, 100.0, 125.0,
    150.0, 200.0, 400.0, 600.0, 800.0, 1200.0, 1600.0, 3200.0
};

static void clk_osc_init()
{
  i2cIF = &i2cIF_Osc;
}

HAL_StatusTypeDef clk_osc_is_ready()
{
  uint8_t isReady;
  I2cQNode node;
  node.i2c = i2cIF;
  node.type = typeIsReady;
  node.address = 0;
  node.data = &isReady;
  node.dataSize = sizeof(isReady);
  node.ready = xSemaphoreCreateBinary();
  xQueueSendToBack(i2cIF->queue, &node, portMAX_DELAY);
  xSemaphoreTake(node.ready, portMAX_DELAY);
  vSemaphoreDelete(node.ready);
  return isReady;
}

void clk_osc_config()
{
  clk_osc_init();
  if(clk_osc_is_ready() != HAL_OK)
    return;

  osc_wr_word(OSC_REG0, 0);
  osc_wr_word(OSC_REG1, 0);
  osc_wr_word(OSC_REG2, 0);
}

void clk_osc_config_bp()
{
  clk_osc_init();

  uint8_t reg0[] = {OSC_REG0, 0, 0};
  uint8_t reg1[] = {OSC_REG1, 0, 0};
  uint8_t reg2[] = {OSC_REG2, 0, 0};
  i2c_tx(i2cIF, reg0, sizeof(reg0));
  i2c_tx(i2cIF, reg1, sizeof(reg1));
  i2c_tx(i2cIF, reg2, sizeof(reg2));
}

void osc_wr(uint8_t address, uint8_t* data, uint16_t size)
{
  I2cQNode node;
  node.i2c = i2cIF;
  node.type = typeTx;
  node.address = address;
  node.data = data;
  node.dataSize = size;
  node.ready = NULL;
  xQueueSendToBack(i2cIF->queue, &node, portMAX_DELAY);
}

void osc_rd(uint8_t address, uint8_t* data, uint16_t size)
{
  I2cQNode node;
  node.i2c = i2cIF;
  node.type = typeRx;
  node.address = address;
  node.data = data;
  node.dataSize = size;
  node.ready = xSemaphoreCreateBinary();
  xQueueSendToBack(i2cIF->queue, &node, portMAX_DELAY);
  xSemaphoreTake(node.ready, portMAX_DELAY);
  vSemaphoreDelete(node.ready);
}

void osc_wr_word(uint8_t address, uint16_t data)
{
  uint8_t* txData = pvPortMalloc(2);
  txData[0] = data >> 8;
  txData[1] = data;
  osc_wr(address, txData, 2);
}

uint16_t osc_rd_word(uint8_t address)
{
  uint8_t rxData[2];
  osc_rd(address, rxData, sizeof(rxData));
  return (uint16_t)rxData[0] << 8 | (uint16_t)rxData[1];
}

void clk_osc_en(ClkOscEn en)
{
  switch (en) {
    case oscEnable:
      HAL_GPIO_WritePin(OSC_EN_GPIO_Port, OSC_EN_Pin, SET);
      break;
    case oscDisable:
    default:
      HAL_GPIO_WritePin(OSC_EN_GPIO_Port, OSC_EN_Pin, RESET);
      break;
  }
}

void osc_set_ppm(float ppm)
{
  const uint16_t power = osc_rd_word(OSC_REG1) & 0x400;
  uint8_t range;
  if(ppm >= 0.0)
    for(range = 0; range < (sizeof(ppmList)/sizeof(ppmList[0])) && ppmList[range] < ppm; ++range) {}
  else
    for(range = 0; range < (sizeof(ppmList)/sizeof(ppmList[0])) && -ppmList[range] > ppm; ++range) {}
  const int32_t freqCtrl = ppm * ((float)(0x1FFFFFF - 1) /  ppmList[range]);
  osc_wr_word(OSC_REG2, range);
  uint16_t txData = ((freqCtrl >> 8) & 0xFF) << 8 | (freqCtrl & 0xFF);
  osc_wr_word(OSC_REG0, txData);
  txData = power | ((freqCtrl >> 24) & 0x03) << 8 | ((freqCtrl >> 16) & 0xFF);
  osc_wr_word(OSC_REG1, txData);
}
