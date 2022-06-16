#ifndef FRAMEWORK_INC_FT_CORE_H_
#define FRAMEWORK_INC_FT_CORE_H_

#include "stm32f4xx.h"
#include "util_string.h"
#include "ft_host_impl.h"
#include "ft_core_impl.h"
#include "ft_screen_impl.h"
#include "ft_widgets.h"
#include "ft_touch.h"

void ft_core_init(FT_Core *core, SPI_IF *spiIF, FT_Display const *param, GPIO_TypeDef *port, uint16_t pin, uint32_t delay);
void ft_core_screen_connect(FT_Core *core, FT_Screen *screen);
void ft_core_config(FT_Core *core, uint32_t stackSize, uint32_t queueSize, FT_Theme const *theme);

#endif /* FRAMEWORK_INC_FT_CORE_H_ */
