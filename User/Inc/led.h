#ifndef INC_LED_H_
#define INC_LED_H_

typedef enum {
  ledRed,
  ledGreen
} LedPin;

void led_set(const LedPin pin);
void led_reset(const LedPin pin);
void led_toggle(const LedPin pin);

#endif /* INC_LED_H_ */
