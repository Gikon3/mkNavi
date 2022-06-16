#include "clk_key.h"
#include "main.h"

void clk_key_sel_src(ClkKey selClk)
{
  switch (selClk) {
    case clkInternal:
      HAL_GPIO_WritePin(CLK_SWITCH_V1_GPIO_Port, CLK_SWITCH_V1_Pin, RESET);
      HAL_GPIO_WritePin(CLK_SWITCH_V2_GPIO_Port, CLK_SWITCH_V2_Pin, SET);
      break;
    case clkExternal:
      HAL_GPIO_WritePin(CLK_SWITCH_V1_GPIO_Port, CLK_SWITCH_V1_Pin, SET);
      HAL_GPIO_WritePin(CLK_SWITCH_V2_GPIO_Port, CLK_SWITCH_V2_Pin, RESET);
      break;
    case clkDisable:
    default:
      HAL_GPIO_WritePin(CLK_SWITCH_V1_GPIO_Port, CLK_SWITCH_V1_Pin, RESET);
      HAL_GPIO_WritePin(CLK_SWITCH_V2_GPIO_Port, CLK_SWITCH_V2_Pin, RESET);
      break;
  }
}
