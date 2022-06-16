#ifndef INC_USB_PORT_CTRL_H_
#define INC_USB_PORT_CTRL_H_

#include "stm32f4xx_hal.h"

#define USBP_REG00  0x00
#define USBP_REG01  0x01
#define USBP_REG02  0x02
#define USBP_REG03  0x03
#define USBP_REG04  0x04
#define USBP_REG05  0x05
#define USBP_REG06  0x06
#define USBP_REG07  0x07
#define USBP_REG08  0x08
#define USBP_REG09  0x09
#define USBP_REG0A  0x0A
#define USBP_REG45  0x45
#define USBP_REGA0  0xA0

HAL_StatusTypeDef usbp_ctrl_is_ready();
void usbp_ctrl_config();
void usbp_ctrl_wr(uint8_t address, uint8_t* data, uint16_t size);
void usbp_ctrl_rd(uint8_t address, uint8_t* data, uint16_t size);
void usbp_ctrl_wr_word(uint8_t address, uint8_t data);
uint8_t usbp_ctrl_rd_word(uint8_t address);

void usbp_ctrl_rd_id(char* str, uint16_t size);
char* usbp_ctrl_get_id();
void usbp_ctrl_clear_int();

#endif /* INC_USB_PORT_CTRL_H_ */
