#ifndef DRIVERS_INC_FT_HAL_H_
#define DRIVERS_INC_FT_HAL_H_

#include "stm32f4xx_hal.h"
#include "spi.h"
#include "util_types.h"
#include "util_vector.h"
#include "ft_host_impl.h"
#include "ft_touch_impl.h"

typedef enum {
  ftHostCmdActive = 0x00,
  ftHostCmdStandby = 0x41,
  ftHostCmdSleep = 0x42,
  ftHostCmdPwrdown = 0x43,
  ftHostCmdPdRoms = 0x49,
  ftHostCmdClkExt = 0x44,
  ftHostCmdClkInt = 0x48,
  ftHostCmdClkSel = 0x61,
  ftHostCmdRstPulse = 0x68,
  ftHostCmdPinDrive = 0x70,
  ftHostCmdPinPdState = 0x71
} FT_HostCmd;

void ft_hal_init(FT_Host* dev, SPI_IF* spiIF, FT_Display const* param, GPIO_TypeDef* port, uint16_t pin);
void ft_hal_config(FT_Host* dev);
void ft_hal_power_mngmt(FT_Host const* dev, Power state);
void ft_hal_wr(FT_Host const* dev, uint32_t addr, uint8_t* data, size_t size);
void ft_hal_wr8(FT_Host const* dev, uint32_t addr, uint8_t data);
void ft_hal_wr16(FT_Host const* dev, uint32_t addr, uint16_t data);
void ft_hal_wr32(FT_Host const* dev, uint32_t addr, uint32_t data);
void ft_hal_rd(FT_Host const* dev, uint32_t addr, uint8_t* data, size_t size);
uint8_t ft_hal_rd8(FT_Host const* dev, uint32_t addr);
uint16_t ft_hal_rd16(FT_Host const* dev, uint32_t addr);
uint32_t ft_hal_rd32(FT_Host const* dev, uint32_t addr);
void ft_hal_host_cmd(FT_Host const* dev, FT_HostCmd cmd, uint8_t param);
void ft_hal_touches(FT_Host const* dev, uint8_t number, FT_Touch* touches);
void ft_hal_tracks(FT_Host const* dev, uint8_t number, FT_Track* tracks);
void ft_hal_wr_into_dl(FT_Host const* dev, Vector* cmds);
void ft_hal_wr_into_cmd(FT_Host const* dev, Vector* cmds);
Bool ft_hal_fifo_empty(FT_Host const* dev);
uint16_t preparing_to_cpy_from_dl(FT_Host const* dev, Vector* cmds);
void ft_hal_cpy_from_dl(FT_Host const* dev, uint32_t addr, size_t size);

#endif /* DRIVERS_INC_FT_HAL_H_ */
