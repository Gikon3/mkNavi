#include "m_hmc5883l.h"
#include "i2c.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"
#include "sensor.h"

HMC5883L hmc5883l;

void m_hmc5883l_wr(HMC5883L* hmc, uint8_t address, uint8_t* data, uint16_t size);
void m_hmc5883l_rd(HMC5883L* hmc, uint8_t address, uint8_t* data, uint16_t size);
void m_hmc5883l_wr_word(HMC5883L* hmc, uint8_t address, uint8_t data);
uint8_t m_hmc5883l_rd_word(HMC5883L* hmc, uint8_t address);

void m_hmc5883l_init()
{
  hmc5883l.comm.type = commI2C;
  hmc5883l.comm.i2cIF = &i2cIF_HMC5883L;
  hmc5883l.valid = bFalse;
  hmc5883l.whoami = HMC5883L_ID0;
  hmc5883l.fullScale = 0;
}

Bool m_hmc5883l_is_ready(HMC5883L* hmc)
{
  return comm_is_ready(&hmc->comm, HMC5883L_ID_REG_A, hmc->whoami);
}

void m_hmc5883l_config(HMC5883L* hmc)
{
  if (m_hmc5883l_is_ready(hmc) != bTrue) return;
  hmc->valid = bTrue;

  HMC5883L_ConfA regA = {
      .ma = 0,
      .dOut = 6,
      .ms = 0
  };
  HMC5883L_ConfB regB = {
      .gn = hmc5883l_1_3gauss
  };
  HMC5883L_Mode regMode = {
      .hs = 0,
      .md = 2
  };

  m_hmc5883l_wr_word(hmc, HMC5883L_CONF_A, *(uint8_t*)&regA);
  m_hmc5883l_wr_word(hmc, HMC5883L_CONF_B, *(uint8_t*)&regB);
  m_hmc5883l_wr_word(hmc, HMC5883L_MODE, *(uint8_t*)&regMode);

  m_hmc5883l_out_mngt(hmc, bTrue);
}

void m_hmc5883l_wr(HMC5883L* hmc, uint8_t address, uint8_t* data, uint16_t size)
{
  comm_send(&hmc->comm, address, data, size);
}

void m_hmc5883l_rd(HMC5883L* hmc, uint8_t address, uint8_t* data, uint16_t size)
{
  comm_recv(&hmc->comm, address, data, size);
}

void m_hmc5883l_wr_word(HMC5883L* hmc, uint8_t address, uint8_t data)
{
  uint8_t* txData = pvPortMalloc(1);
  *txData = data;
  m_hmc5883l_wr(hmc, address, txData, 1);
}

uint8_t m_hmc5883l_rd_word(HMC5883L* hmc, uint8_t address)
{
  uint8_t rxData;
  m_hmc5883l_rd(hmc, address, &rxData, 1);
  return rxData;
}

void m_hmc5883l_refresh(HMC5883L* hmc)
{
  if (hmc->valid != bTrue) return;
  double fullScaleVal = 0;
  switch (hmc->fullScale) {
    case hmc5883l_0_88gauss: fullScaleVal = 0.88; break;
    case hmc5883l_1_3gauss: fullScaleVal = 1.3; break;
    case hmc5883l_1_9gauss: fullScaleVal = 1.9; break;
    case hmc5883l_2_5gauss: fullScaleVal = 2.5; break;
    case hmc5883l_4_0gauss: fullScaleVal = 4.0; break;
    case hmc5883l_4_7gauss: fullScaleVal = 4.7; break;
    case hmc5883l_5_6gauss: fullScaleVal = 5.6; break;
    case hmc5883l_8_1gauss: fullScaleVal = 8.1; break;
  }
  uint8_t data[6];
  m_hmc5883l_rd(hmc, HMC5883L_OUT_X_MSB, data, sizeof(data));
  sens.mag.raw.x = -(int16_t)(data[4] << 8 | data[5]);
  sens.mag.raw.y = (int16_t)(data[0] << 8 | data[1]);
  sens.mag.raw.z = (int16_t)(data[2] << 8 | data[3]);
  sens.mag.ripe.x = sens.mag.raw.x * fullScaleVal / 0x7FFL;
  sens.mag.ripe.y = sens.mag.raw.y * fullScaleVal / 0x7FFL;
  sens.mag.ripe.z = sens.mag.raw.z * fullScaleVal / 0x7FFL;
}

void m_hmc5883l_out_mngt(HMC5883L* hmc, Bool en)
{
  if (hmc->valid != bTrue) return;
  uint8_t temp = m_hmc5883l_rd_word(hmc, HMC5883L_MODE);
  HMC5883L_Mode regMode = *(HMC5883L_Mode*)&temp;
  regMode.md = en == bTrue ? 0 : 2;
  m_hmc5883l_wr_word(hmc, HMC5883L_MODE, *(uint8_t*)&regMode);
}
