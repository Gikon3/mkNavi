#ifndef INC_M_HMC5883L_H_
#define INC_M_HMC5883L_H_

#include "stm32f4xx_hal.h"
#include "util_types.h"
#include "comm_point.h"

#define HMC5883L_CONF_A     0x00
#define HMC5883L_CONF_B     0x01
#define HMC5883L_MODE       0x02
#define HMC5883L_OUT_X_MSB  0x03
#define HMC5883L_OUT_X_LSB  0x04
#define HMC5883L_OUT_Z_MSB  0x05
#define HMC5883L_OUT_Z_LSB  0x06
#define HMC5883L_OUT_Y_MSB  0x07
#define HMC5883L_OUT_Y_LSB  0x08
#define HMC5883L_STAT       0x09
#define HMC5883L_ID_REG_A   0x0A
#define HMC5883L_ID_REG_B   0x0B
#define HMC5883L_ID_REG_C   0x0C

#define HMC5883L_ID0        'H'
#define HMC5883L_ID1        '4'
#define HMC5883L_ID2        '3'

typedef enum {
  hmc5883l_0_88gauss,
  hmc5883l_1_3gauss,
  hmc5883l_1_9gauss,
  hmc5883l_2_5gauss,
  hmc5883l_4_0gauss,
  hmc5883l_4_7gauss,
  hmc5883l_5_6gauss,
  hmc5883l_8_1gauss
} HMC5883L_FullScale;

typedef struct
{
  CommPoint           comm;
  Bool                valid;
  uint8_t             whoami;
  HMC5883L_FullScale  fullScale;
} HMC5883L;

typedef struct
{
  uint8_t ms    : 2;
  uint8_t dOut  : 3;
  uint8_t ma    : 2;
  uint8_t       : 1;
} HMC5883L_ConfA;
typedef struct
{
  uint8_t     : 5;
  uint8_t gn  : 3;
} HMC5883L_ConfB;
typedef struct
{
  uint8_t md  : 2;
  uint8_t     : 5;
  uint8_t hs  : 1;
} HMC5883L_Mode;
typedef struct
{
  uint8_t rdy   : 1;
  uint8_t lock  : 1;
  uint8_t       : 6;
} HMC5883L_Stat;

extern HMC5883L hmc5883l;

void m_hmc5883l_init();
Bool m_hmc5883l_is_ready(HMC5883L* hmc);
void m_hmc5883l_config(HMC5883L* hmc);
void m_hmc5883l_refresh(HMC5883L* hmc);
void m_hmc5883l_out_mngt(HMC5883L* hmc, Bool en);

#endif /* INC_M_HMC5883L_H_ */
