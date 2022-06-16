#ifndef INC_GNSS_BASE_H_
#define INC_GNSS_BASE_H_

#include "stm32f4xx_hal.h"
#include "util_types.h"

typedef enum {
  gnssIss10Hz = '0',
  gnssIss5Hz  = '1',
  gnssIss2Hz  = '2',
  gnssIss1Hz  = '3'
} GNSS_IssueRateOutData;
typedef enum {
  gnssNMEA_Ver2 = '2',
  gnssNMEA_Ver4 = '4'
} GNSS_NMEA_Version;
typedef enum {
  gnssIssNone     = '0',
  gnssIss1Sec     = '1',
  gnssIss2Sec     = '2',
  gnssIss20Sec    = '3',
  gnssIssOutData  = 'R'
} GNSS_IssueRateVal;
typedef struct
{
  GNSS_IssueRateOutData outData;
  GNSS_NMEA_Version     nmeaVer;
  GNSS_IssueRateVal     gga;
  GNSS_IssueRateVal     gns;
  GNSS_IssueRateVal     gsa;
  GNSS_IssueRateVal     gsv;
  GNSS_IssueRateVal     rmc;
  GNSS_IssueRateVal     vtg;
  GNSS_IssueRateVal     gll;
  GNSS_IssueRateVal     zda;
  GNSS_IssueRateVal     dtm;
  GNSS_IssueRateVal     rlm;
} GNSS_IssueRate;
typedef struct
{
  struct
  {
    uint8_t   day;
    uint8_t   month;
    uint32_t  year;
  } date;
  struct
  {
    uint8_t   second;
    uint8_t   minute;
    uint8_t   hour;
  } time;
  Bool        pending;
} GNSS_SyncDateTime;
typedef enum {
  gnssNavModeRTK_Fix,
  gnssNavModeRTK_Float,
  gnssNavModeDGPS,
  gnssNavModeWDGPS,
  gnssNavModeSP,
  gnssNavModePPP
} GNSS_NavMode;
typedef enum {
  gnssSatNone = 0,
  gnssSatGPS = 1 << 0,
  gnssSatGLO = 1 << 1,
  gnssSatGAL = 1 << 2,
  gnssSatSBAS = 1 << 3,
  gnssSatQZSS = 1 << 4,
  gnssSatBDS = 1 << 5,
  gnssSatIRNSS = 1 << 6,
  gnssSatAll = 0x7F
} GNSS_SatSystem;
typedef enum {
  gnssNavTypeNotValied = 0,
  gnssNavTypeAutonomous,
  gnssNavTypeDifferencial,
  gnssNavTypePPS,
  gnssNavTypeRTK_Fix,
  gnssNavTypeRTK_Float,
  gnssNavTypeEstimated,
  gnssNavTypeManual,
  gnssNavTypeSimulator
} GNSS_NavType;
typedef enum {
  gnssNavSystemNone = 0,
  gnssNavSystemPZ90_11,
  gnssNavSystemWGS84,
  gnssNavSystemUser
} GNSS_NavSystem;

typedef struct
{
  struct
  {
    GPIO_TypeDef* pwrGpioPort;
    uint16_t      pwrGpioPin;
  } hard;
  Power             power;          // Питание модуля
  Bool              exchAllow;      // Индикатор, что модуль готов к обмену сообщениями
  GNSS_SyncDateTime syncDT;         // Синхронизация даты и времени
  GNSS_IssueRate    issueRate[2];   // Темпы выдачи сообщений для каждого порта
  GNSS_NavMode      navMode;        // Тип выбранного навигационного решения
  GNSS_NavType      solution;       // Тип полученного навигационного решения
  int32_t           timegap;        // Секунд с последнего RTCM сообщения
  uint8_t           satellitsNum;   // Количество спутников в решении
  GNSS_NavSystem    navSystem;      // Навигационная система, для которой представленны координаты
  float             speed;          // Скорость относительно земли, узлы
  float             course;         // Курс, град (истинный)
  float             hdop;
  float             altitude;       // Высота над средним уровнем моря, м
  float             geoidOffset;    // Отклонение геоида, м
  struct
  {
    double          lat;            // Широта, град ('-' при 'S')
    double          lng;            // Долгота, град ('-' при 'W')
  } coord;
} GNSS;

#endif /* INC_GNSS_BASE_H_ */
