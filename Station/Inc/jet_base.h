#ifndef INC_JET_BASE_H_
#define INC_JET_BASE_H_

#include "stm32f4xx_hal.h"
#include "util_types.h"

typedef struct
{
  uint8_t lsn   : 1;
  uint8_t gnss  : 1;
  uint8_t ins   : 1;
  uint8_t wifi  : 1;
  uint8_t blu   : 1;
  uint8_t gsm   : 1;
  uint8_t       : 2;
} JetSystem;

typedef struct
{
  struct
  {
    Bool      updated;
    uint64_t  time;
    struct
    {
      double  lat;
      double  lng;
    } coord;
    JetSystem system;   // Системы участвующие в комплексировании
  } solution;
  struct
  {
    Bool      updated;
    uint64_t  time;
    uint32_t  major;
    uint32_t  minor;
    uint8_t   uuid[16];
    int16_t   rssi;
  } blu;
  struct
  {
    Bool      updated;
    uint64_t  time;
    uint8_t   mac[6];
    float     distance;
  } wifi;
  struct
  {
    Bool      updated;
    uint64_t  time;
    struct
    {
      double  lat;
      double  lng;
    } coord;
  } gps;
} Jet;

#endif /* INC_JET_BASE_H_ */
