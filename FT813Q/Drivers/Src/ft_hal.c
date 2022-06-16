#include "ft_hal.h"
#include "main.h"
#include "ft_mem_map.h"
#include "ft_utils.h"
#include "ft_dl_cmds.h"
#include "ft_co_pro_cmds.h"

void ft_hal_init(FT_Host* dev, SPI_IF* spiIF, FT_Display const* param, GPIO_TypeDef* port, uint16_t pin)
{
  dev->param = *param;
  dev->power.port = port;
  dev->power.pin = pin;
  dev->spiIF = spiIF;
}

void ft_hal_config(FT_Host* dev)
{
  ft_hal_power_mngmt(dev, pOn);
  osDelay(20/portTICK_RATE_MS);

  ft_hal_host_cmd(dev, ftHostCmdClkExt, 0x00);
  ft_hal_host_cmd(dev, ftHostCmdActive, 0x00);
  osDelay(300/portTICK_RATE_MS);

  ft_hal_wr16(dev, FT_REG_HSIZE, dev->param.h.size);
  ft_hal_wr16(dev, FT_REG_HCYCLE, dev->param.h.cycle);
  ft_hal_wr16(dev, FT_REG_HOFFSET, dev->param.h.offset);
  ft_hal_wr16(dev, FT_REG_HSYNC0, dev->param.h.sync0);
  ft_hal_wr16(dev, FT_REG_HSYNC1, dev->param.h.sync1);

  ft_hal_wr16(dev, FT_REG_VSIZE, dev->param.v.size);
  ft_hal_wr16(dev, FT_REG_VCYCLE, dev->param.v.cycle);
  ft_hal_wr16(dev, FT_REG_VOFFSET, dev->param.v.offset);
  ft_hal_wr16(dev, FT_REG_VSYNC0, dev->param.v.sync0);
  ft_hal_wr16(dev, FT_REG_VSYNC1, dev->param.v.sync1);

  ft_hal_wr8(dev, FT_REG_SWIZZLE, dev->param.swizzle);
  ft_hal_wr8(dev, FT_REG_PCLK_POL, dev->param.pclkpol);
  ft_hal_wr8(dev, FT_REG_CSPREAD, dev->param.cspread);
  ft_hal_wr8(dev, FT_REG_DITHER, dev->param.dither);

  Vector defaultScreen = make_vect(4, sizeof(uint32_t));
  ft_cmd_dlstart(&defaultScreen);
  ft_cmd_dl_set(&defaultScreen, ft_dl_clear(bTrue, bTrue, bTrue));
  ft_cmd_dl_set(&defaultScreen, ft_dl_display());
  ft_cmd_dlswap(&defaultScreen);
  ft_hal_wr_into_cmd(dev, &defaultScreen);
  osDelay(20/portTICK_RATE_MS);

  ft_hal_wr8(dev, FT_REG_GPIO_DIR, 0x80 | ft_hal_rd8(dev, FT_REG_GPIO_DIR));
  ft_hal_wr8(dev, FT_REG_GPIO, 0x080 | ft_hal_rd8(dev, FT_REG_GPIO));
  ft_hal_wr8(dev, FT_REG_PCLK, dev->param.pclk);

  ft_hal_wr8(dev, FT_REG_CPURESET, 0);
  ft_hal_wr8(dev, FT_REG_CTOUCH_EXTENDED, 0);
  ft_hal_rd8(dev, FT_REG_INT_FLAGS);
  ft_hal_wr8(dev, FT_REG_INT_MASK, 2);
  ft_hal_wr8(dev, FT_REG_INT_EN, 1);

  dev->whoAmI = ft_hal_rd32(dev, FT_RAM_CHIP_ID);
  dev->spiIF->prescaler = SPI_BAUDRATEPRESCALER_2;
}

void ft_hal_power_mngmt(FT_Host const* dev, Power state)
{
  switch(state) {
    case pOn:
      HAL_GPIO_WritePin(dev->power.port, dev->power.pin, SET);
      break;
    case pOff:
    default:
      HAL_GPIO_WritePin(dev->power.port, dev->power.pin, RESET);
  }
}

void ft_hal_wr(FT_Host const* dev, uint32_t addr, uint8_t* data, size_t size)
{
  SpiQNode node;
  node.spi = dev->spiIF;
  node.type = typeTx;
  node.address = addr;
  node.data = (uint8_t*)data;
  node.dataSize = size;
  node.ready = NULL;
  node.sdParam = NULL;
  xQueueSendToBack(dev->spiIF->queue, &node, portMAX_DELAY);
}

void ft_hal_wr8(FT_Host const* dev, uint32_t addr, uint8_t data)
{
  uint8_t* txData = pvPortMalloc(1);
  txData[0] = data;
  ft_hal_wr(dev, addr, txData, 1);
}

void ft_hal_wr16(FT_Host const* dev, uint32_t addr, uint16_t data)
{
  uint8_t* txData = pvPortMalloc(2);
  txData[1] = data >> 8;
  txData[0] = data >> 0;
  ft_hal_wr(dev, addr, txData, 2);
}

void ft_hal_wr32(FT_Host const* dev, uint32_t addr, uint32_t data)
{
  uint8_t* txData = pvPortMalloc(4);
  txData[3] = data >> 24;
  txData[2] = data >> 16;
  txData[1] = data >> 8;
  txData[0] = data >> 0;
  ft_hal_wr(dev, addr, txData, 4);
}

void ft_hal_rd(FT_Host const* dev, uint32_t addr, uint8_t* data, size_t size)
{
  SpiQNode node;
  node.spi = dev->spiIF;
  node.type = typeRx;
  node.address = addr;
  node.data = data;
  node.dataSize = size;
  node.ready = xSemaphoreCreateBinary();
  node.sdParam = NULL;
  xQueueSendToBack(dev->spiIF->queue, &node, portMAX_DELAY);
  xSemaphoreTake(node.ready, portMAX_DELAY);
  vSemaphoreDelete(node.ready);
}

uint8_t ft_hal_rd8(FT_Host const* dev, uint32_t addr)
{
  uint8_t data;
  ft_hal_rd(dev, addr, &data, 1);
  return data;
}

uint16_t ft_hal_rd16(FT_Host const* dev, uint32_t addr)
{
  uint16_t data;
  ft_hal_rd(dev, addr, (uint8_t*)&data, 2);
  return data;
}

uint32_t ft_hal_rd32(FT_Host const* dev, uint32_t addr)
{
  uint32_t data;
  ft_hal_rd(dev, addr, (uint8_t*)&data, 4);
  return data;
}

void ft_hal_host_cmd(FT_Host const* dev, FT_HostCmd cmd, uint8_t param)
{
  uint8_t* command = pvPortMalloc(3);
  command[0] = cmd;
  command[1] = param;
  command[2] = 0x00;

  SpiQNode node;
  node.spi = dev->spiIF;
  node.type = typeTxData;
  node.address = 0;
  node.data = command;
  node.dataSize = 3;
  node.ready = NULL;
  node.sdParam = NULL;
  xQueueSendToBack(dev->spiIF->queue, &node, portMAX_DELAY);
}

void ft_hal_touches(FT_Host const* dev, uint8_t number, FT_Touch* touches)
{
  uint32_t regs[FT_NUMBER_OF_TOUCHES*2];
  ft_hal_rd(dev, FT_REG_CTOUCH_TAG_XY, (uint8_t*)regs, sizeof(regs[0])*2*number);
  for(uint32_t i = 0; i < number; ++i) {
    touches[i].tag = regs[2*i+1];
    touches[i].x = regs[2*i] >> 16;
    touches[i].y = regs[2*i];
  }
}

void ft_hal_tracks(FT_Host const* dev, uint8_t number, FT_Track* tracks)
{
  uint32_t regs[FT_NUMBER_OF_TOUCHES];
  ft_hal_rd(dev, FT_SPREG_TRACKER, (uint8_t*)regs, sizeof(regs[0])*number);
  for(uint32_t i = 0; i < number; ++i) {
    tracks[i].tag = regs[i];
    tracks[i].val = regs[i] >> 16;
  }
}

void ft_hal_wr_into_dl(FT_Host const* dev, Vector* cmds)
{
  const size_t txSize = cmds->size * cmds->itemSize;
  ft_hal_wr(dev, FT_RAM_DL, vect_release(cmds), txSize);
}

void ft_hal_wr_into_cmd(FT_Host const* dev, Vector* cmds)
{
  const size_t txSize = cmds->size * cmds->itemSize;
  ft_hal_wr(dev, FT_REG_CMDB_WRITE, vect_release(cmds), txSize);
}

Bool ft_hal_fifo_empty(FT_Host const* dev)
{
  uint32_t fifo[2];
  ft_hal_rd(dev, FT_REG_CMD_READ, (uint8_t *)fifo, sizeof(fifo));
  return fifo[0] == fifo[1] ? bTrue : bFalse;
}

uint16_t preparing_to_cpy_from_dl(FT_Host const* dev, Vector* cmds)
{
  Vector startFrame = make_vect(1, sizeof(uint32_t));
  ft_cmd_dlstart(&startFrame);
  ft_hal_wr_into_cmd(dev, &startFrame);
  while(ft_hal_fifo_empty(dev) != bTrue);

  ft_hal_wr_into_cmd(dev, cmds);
  while(ft_hal_fifo_empty(dev) != bTrue);

  return ft_hal_rd16(dev, FT_REG_CMD_DL);
}

void ft_hal_cpy_from_dl(FT_Host const* dev, uint32_t addr, size_t size)
{
  Vector memWrite = make_vect(4, sizeof(uint32_t));
  ft_cmd_memcpy(&memWrite, addr, FT_RAM_DL, size);
  ft_hal_wr_into_cmd(dev, &memWrite);
}
