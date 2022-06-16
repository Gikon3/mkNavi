#include "shutdown.h"
#include "main.h"

void shutdown()
{
  HAL_GPIO_WritePin(BTN_KILL_GPIO_Port, BTN_KILL_Pin, GPIO_PIN_RESET);
}
