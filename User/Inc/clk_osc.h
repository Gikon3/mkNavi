#ifndef INC_CLK_OSC_H_
#define INC_CLK_OSC_H_

#include "stm32f4xx_hal.h"

#define OSC_REG0  0x00
#define OSC_REG1  0x01
#define OSC_REG2  0x02

typedef enum {
  oscEnable,
  oscDisable
} ClkOscEn;

HAL_StatusTypeDef clk_osc_is_ready();
void clk_osc_config();
void clk_osc_config_bp();
void osc_wr(uint8_t address, uint8_t* data, uint16_t size);
void osc_rd(uint8_t address, uint8_t* data, uint16_t size);
void osc_wr_word(uint8_t address, uint16_t data);
uint16_t osc_rd_word(uint8_t address);
void clk_osc_en(ClkOscEn en);
void osc_set_ppm(float ppm);

#endif /* INC_CLK_OSC_H_ */
