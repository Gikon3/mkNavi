#ifndef INC_RTC_IF_H_
#define INC_RTC_IF_H_

#include "stm32f4xx_hal.h"

typedef struct
{
  struct
  {
    uint32_t startTicks;
    uint32_t zeroYear;
  } ti;
  struct
  {
    uint8_t   day;
    uint8_t   month;
    uint16_t  year;
  } date;
  struct
  {
    uint16_t  msec;
    uint8_t   sec;
    uint8_t   min;
    uint8_t   hour;
  } time;
} RTC_IF;

extern RTC_IF rtcIF;

void rtc_datetime(RTC_IF* rtc);
void rtc_sync_datetime(RTC_IF* rtc);
uint32_t rtc_datetime32(RTC_IF const* rtc);

#endif /* INC_RTC_IF_H_ */
