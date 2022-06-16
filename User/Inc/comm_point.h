#ifndef INC_COMM_POINT_H_
#define INC_COMM_POINT_H_

#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "spi.h"
#include "util_types.h"

typedef enum {
  commI2C,
  commSPI
} CommType;

typedef struct
{
  CommType  type;
  union
  {
    I2C_IF* i2cIF;
    SPI_IF* spiIF;
  };
} CommPoint;

Bool comm_send(CommPoint* comm, uint8_t address, uint8_t* data, uint16_t size);
Bool comm_recv(CommPoint* comm, uint8_t address, uint8_t* data, uint16_t size);
Bool comm_is_ready(CommPoint* comm, uint8_t whoamiAddr, uint8_t whoamiVal);

#endif /* INC_COMM_POINT_H_ */
