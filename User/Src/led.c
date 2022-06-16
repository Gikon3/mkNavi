#include "led.h"
#include "main.h"

typedef struct LedPinSelect_
{
  GPIO_TypeDef* port;
  uint32_t pin;
} LedPinSelect;

static inline LedPinSelect led_pin(const LedPin pin)
{
  LedPinSelect ledSelect;
  switch (pin) {
    case ledRed:
      ledSelect.port = LED_R_GPIO_Port;
      ledSelect.pin = LED_R_Pin;
      return ledSelect;
    case ledGreen:
    default:
      ledSelect.port = LED_G_GPIO_Port;
      ledSelect.pin = LED_G_Pin;
      return ledSelect;
  }
}

void led_set(const LedPin pin)
{
  LedPinSelect ledSelect = led_pin(pin);
  HAL_GPIO_WritePin(ledSelect.port, ledSelect.pin, SET);
}

void led_reset(const LedPin pin)
{
  LedPinSelect ledSelect = led_pin(pin);
  HAL_GPIO_WritePin(ledSelect.port, ledSelect.pin, RESET);
}

void led_toggle(const LedPin pin)
{
  LedPinSelect ledSelect = led_pin(pin);
  HAL_GPIO_TogglePin(ledSelect.port, ledSelect.pin);
}
