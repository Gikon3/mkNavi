#ifndef FRAMEWORK_INC_FT_HOST_IMPL_H_
#define FRAMEWORK_INC_FT_HOST_IMPL_H_

#include "stm32f4xx_hal.h"
#include "spi.h"

typedef struct
{
  uint16_t size;
  uint16_t cycle;
  uint16_t offset;
  uint16_t sync0;
  uint16_t sync1;
} FT_SyncParam;

typedef struct
{
  FT_SyncParam  h;
  FT_SyncParam  v;
  uint8_t       pclk;
  uint8_t       swizzle;
  uint8_t       pclkpol;
  uint8_t       cspread;
  uint8_t       dither;
} FT_Display;

typedef struct _FT_Host
{
  uint32_t        whoAmI;
  FT_Display      param;
  struct
  {
    GPIO_TypeDef  *port;
    uint16_t      pin;
  } power;
  SPI_IF          *spiIF;
} FT_Host;

#endif /* FRAMEWORK_INC_FT_HOST_IMPL_H_ */
