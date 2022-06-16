#include "gnss.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "rtc.h"
#include "station.h"
#include "flow_ctrl.h"

extern osSemaphoreId_t semSyncTimeHandle;

void gnss_init()
{
# ifdef DEVICE_TYPE_ATB
  station.gnss.hard.pwrGpioPort = GL_ON_OFF_GPIO_Port;
  station.gnss.hard.pwrGpioPin = GL_ON_OFF_Pin;
# endif
# ifdef DEVICE_TYPE_ATV
  station.gnss.hard.pwrGpioPort = NULL;
  station.gnss.hard.pwrGpioPin = 0;
# endif
  station.gnss.power = pOff;
  station.gnss.exchAllow = bFalse;
  station.gnss.syncDT.date.day = 1;
  station.gnss.syncDT.date.month = 1;
  station.gnss.syncDT.date.year = 2008;
  station.gnss.syncDT.time.second = 0;
  station.gnss.syncDT.time.minute = 0;
  station.gnss.syncDT.time.hour = 0;
  station.gnss.syncDT.pending = bFalse;
  station.gnss.navMode = gnssNavModeRTK_Fix;
  station.gnss.solution = gnssNavTypeNotValied;
  station.gnss.timegap = -1;
  station.gnss.satellitsNum = 0;
  station.gnss.navSystem = gnssNavSystemNone;
  station.gnss.speed = 0.0;
  station.gnss.course = 0.0;
  station.gnss.hdop = 0.0;
  station.gnss.altitude = 0.0;
  station.gnss.geoidOffset = 0.0;
  station.gnss.coord.lat = 0.0;
  station.gnss.coord.lng = 0.0;
}

void gnss_config(Station* sta, FlowCtrl const* flow)
{
  HAL_UART_Receive_DMA(flow->raw.gnss[1].uart, (uint8_t*)flow->raw.gnss[1].buff, flow->raw.gnss[1].size);
  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);    // Прерывание на 1PPS

# ifdef DEVICE_TYPE_ATB
  gnss_power(sta, pOn);
  while (sta->gnss.exchAllow != bTrue) osDelay(10/portTICK_PERIOD_MS);
  gnss_send_nmeacmd(sta, flowGNSS1, gnssNCmdGGA_On);
  gnss_send_nmeacmd(sta, flowGNSS1, gnssNCmdGLL_Off);
  gnss_send_nmeacmd(sta, flowGNSS1, gnssNCmdGSA_Off);
  gnss_send_nmeacmd(sta, flowGNSS1, gnssNCmdGSV_Off);
  gnss_send_nmeacmd(sta, flowGNSS1, gnssNCmdRMC_On);
  gnss_send_nmeacmd(sta, flowGNSS1, gnssNCmdVTG_Off);
  gnss_send_nmeacmd(sta, flowGNSS1, gnssNCmdZDA_Off);
  gnss_send_nmeacmd(sta, flowGNSS1, gnssNCmdDTM_On);
  gnss_send_nmeacmd(sta, flowGNSS1, gnssNCmdRLM_Off);
  gnss_send_nmeacmd(sta, flowGNSS1, gnssNCmdDatp90);
# endif
# ifdef DEVICE_TYPE_ATV
  sta->gnss.power = pOn;
  javad_send_nmeacmd(sta, flowGNSS1, javNCmdGGA_On);
  javad_send_nmeacmd(sta, flowGNSS1, javNCmdGLL_On);
  javad_send_nmeacmd(sta, flowGNSS1, javNCmdGNS_Off);
  javad_send_nmeacmd(sta, flowGNSS1, javNCmdGRS_Off);
  javad_send_nmeacmd(sta, flowGNSS1, javNCmdGSA_Off);
  javad_send_nmeacmd(sta, flowGNSS1, javNCmdGST_Off);
  javad_send_nmeacmd(sta, flowGNSS1, javNCmdGSV_Off);
  javad_send_nmeacmd(sta, flowGNSS1, javNCmdRMC_On);
  javad_send_nmeacmd(sta, flowGNSS1, javNCmdHDT_Off);
  javad_send_nmeacmd(sta, flowGNSS1, javNCmdVTG_Off);
  javad_send_nmeacmd(sta, flowGNSS1, javNCmdROT_Off);
  javad_send_nmeacmd(sta, flowGNSS1, javNCmdZDA_Off);
  javad_send_nmeacmd(sta, flowGNSS1, javNCmdGMP_Off);
  javad_set_imode(sta, flowGNSS1, 'b', javInModeRTCM3);
  javad_switch_solution(sta, flowGNSS1, gnssNavModeRTK_Fix);
  javad_en_code_diff_pos(sta, flowGNSS1, bTrue);
  javad_en_sbas_code_diff_pos(sta, flowGNSS1, bTrue);
  javad_en_sattelite_system(sta, flowGNSS1, gnssSatAll);
# endif

  gnss_sync_time(sta);
}

void gnss_power(Station* sta, Power state)
{
  switch (state) {
    case pOn: HAL_GPIO_WritePin(sta->gnss.hard.pwrGpioPort, sta->gnss.hard.pwrGpioPin, SET); break;
    case pOff: HAL_GPIO_WritePin(sta->gnss.hard.pwrGpioPort, sta->gnss.hard.pwrGpioPin, RESET); break;
    default: return;
  }
  sta->gnss.power = state;
  if (state == pOff) sta->gnss.exchAllow = bFalse;
}

GNSS_SyncDateTime const* gnss_datetime(Station const* sta) { return &sta->gnss.syncDT; }
GNSS_IssueRate const* gnss_issue_rate0(Station const* sta) { return &sta->gnss.issueRate[0]; }
GNSS_IssueRate const* gnss_issue_rate1(Station const* sta) { return &sta->gnss.issueRate[1]; }

void gnss_sync_time(Station* sta)
{
  sta->gnss.syncDT.pending = bTrue;
}

HAL_StatusTypeDef gnss_uart0_tx(FlowCtrl const* flow, String const* str)
{
  HAL_StatusTypeDef status;
  vPortEnterCritical();
  status = HAL_UART_Transmit(flow->raw.gnss[0].uart, (uint8_t*)str->data, str->len, str->len);
  vPortExitCritical();
  return status;
}
HAL_StatusTypeDef gnss_uart1_tx(FlowCtrl const* flow, String const* str)
{
  HAL_StatusTypeDef status;
  vPortEnterCritical();
  status = HAL_UART_Transmit(flow->raw.gnss[1].uart, (uint8_t*)str->data, str->len, str->len);
  vPortExitCritical();
  return status;
}

static inline int8_t responce_check(String const* str)
{
  if (str->data[0] != '$' || str->data[str->len-3-2] != '*') {
    return 1;
  }
  size_t cnt = 1;
  uint8_t checksum = 0;
  while (str->data[cnt] != '*') {
    checksum ^= str->data[cnt++];
  }
  uint8_t txChecksum = strtol(&str->data[cnt+1], NULL, 16);
  return checksum - txChecksum;
}
static inline double coord_transform(double val, char pol)
{
  const uint16_t degree = val / 100;
  const double ret = degree + (val - degree) / 60.0;
  return pol == 'S' || pol == 'W' ? -ret : ret;
}
void gnss_resp_data_proc(String const* str, FlowPort srcPort, Station* sta)
{
  if (responce_check(str)) return;
  if (sta->gnss.power == pOn) sta->gnss.exchAllow = bTrue;
  flow_screen_give(&flowCtrl, str, srcPort);
  switch(srcPort) {
    case flowGNSS0:
      flowCtrl.ripe.gnss[0].start = xTaskGetTickCount();
      if(!flowCtrl.ripe.gnss[0].accum.data)
        flowCtrl.ripe.gnss[0].accum = make_str("GNSS\r\n");
      str_append_str(&flowCtrl.ripe.gnss[0].accum, str);
      break;
    case flowGNSS1:
      flowCtrl.ripe.gnss[1].start = xTaskGetTickCount();
      if(!flowCtrl.ripe.gnss[1].accum.data)
        flowCtrl.ripe.gnss[1].accum = make_str("GNSS\r\n");
      str_append_str(&flowCtrl.ripe.gnss[1].accum, str);
      break;
    default:;
  }

  // Разбор строки $--GGA
  if (!memcmp(&str->data[3], "GGA", 3)) {
    char const* body = &str->data[7];
    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;
    uint8_t msecond = 0;
    double lat = 0.0;
    char latPol = '\0';
    double lng = 0.0;
    char lngPol = '\0';
    uint8_t recMode = 0;
    uint8_t satellitsNum = 0;
    float hdop = 0.0;
    float altitude = 0.0;
    float geoidOffset = 0.0;
    float diffAge = -1.0;
    uint8_t diffId = 0;
    sscanf(body, "%2hhd%2hhd%2hhd.%2hhd,%lf,%c,%lf,%c,%c,%2hhd,%f,%f,M,%f,M,%f,%c",
           &hour, &minute, &second, &msecond, &lat, &latPol, &lng, &lngPol, &recMode,
           &satellitsNum, &hdop, &altitude, &geoidOffset, &diffAge, &diffId);
    sta->gnss.timegap = diffAge;
    switch (recMode) {
      case '0': sta->gnss.solution = gnssNavTypeNotValied; break;
      case '1': sta->gnss.solution = gnssNavTypeAutonomous; break;
      case '2': sta->gnss.solution = gnssNavTypeDifferencial; break;
      case '3': sta->gnss.solution = gnssNavTypePPS; break;
      case '4': sta->gnss.solution = gnssNavTypeRTK_Fix; break;
      case '5': sta->gnss.solution = gnssNavTypeRTK_Float; break;
      case '6': sta->gnss.solution = gnssNavTypeEstimated; break;
      case '7': sta->gnss.solution = gnssNavTypeManual; break;
      case '8': sta->gnss.solution = gnssNavTypeSimulator; break;
    }
    if (sta->gnss.solution != gnssNavTypeNotValied && sta->gnss.solution != gnssNavTypeEstimated) {
      sta->gnss.coord.lat = coord_transform(lat, latPol);
      sta->gnss.coord.lng = coord_transform(lng, lngPol);
      sta->gnss.satellitsNum = satellitsNum;
      sta->gnss.hdop = hdop;
      sta->gnss.altitude = altitude;
      sta->gnss.geoidOffset = geoidOffset;
    }
  }
  else if (!memcmp(&str->data[3], "GLL", 3)) {
  }
  else if (!memcmp(&str->data[3], "GNS", 3)) {
  }
  else if (!memcmp(&str->data[3], "GSA", 3)) {
  }
  else if (!memcmp(&str->data[3], "GSV", 3)) {
  }
  // Разбор строки $--RMC
  else if (!memcmp(&str->data[3], "RMC", 3)) {
    char const* body = &str->data[7];
    uint16_t hour = 0;
    uint16_t minute = 0;
    uint16_t second = 0;
    uint16_t msecond = 0;
    char valid = '\0';
    double lat = 0.0;
    char latPol = '\0';
    double lng = 0.0;
    char lngPol = '\0';
    float speed = 0.0;
    float course = 0.0;
    uint16_t day = 1;
    uint16_t month = 1;
    uint16_t year = 0;
    float magnDecl = 0.0;
    char magnDeclPol = '\0';
    char recMode = '\0';
    sscanf(body, "%2hu%2hu%2hu.%2hu,%c,%lf,%c,%lf,%c,%f,%f,%2hu%2hu%2hu,%f,%c,%c",
           &hour, &minute, &second, &msecond, &valid, &lat, &latPol, &lng, &lngPol,
           &speed, &course, &day, &month, &year, &magnDecl, &magnDeclPol, &recMode);
//    switch (recMode) {
//      case 'A': sta->gnss.solution = gnssNavTypeAutonomous; break;
//      case 'D': sta->gnss.solution = gnssNavTypeDifferencial; break;
//      case 'E': sta->gnss.solution = gnssNavTypeEstimated; break;
//      case 'M': sta->gnss.solution = gnssNavTypeManual; break;
//      case 'S': sta->gnss.solution = gnssNavTypeSimulator; break;
//      case 'N': sta->gnss.solution = gnssNavTypeNotValied; break;
//    }
    if (valid == 'A') {
      sta->gnss.speed = speed;
      sta->gnss.course = course;
      GNSS_SyncDateTime* dt = &sta->gnss.syncDT;
      if (dt->pending == bTrue && second < 59) {
        dt->pending = bFalse;
        dt->time.hour = hour;
        dt->time.minute = minute;
        dt->time.second = second + 1;
        dt->date.day = day;
        dt->date.month = month;
        dt->date.year = 2000 + year;
#       ifdef DEVICE_TYPE_ATV
        xSemaphoreGive(semSyncTimeHandle);
#       else
        HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
#       endif
      }
    }
  }
  else if (!memcmp(&str->data[3], "VTG", 3)) {
  }
  else if (!memcmp(&str->data[3], "ZDA", 3)) {
    char const* body = &str->data[7];
    unsigned int hour = 0;
    unsigned int minute = 0;
    unsigned int second = 0;
    unsigned int day = 0;
    unsigned int month = 0;
    unsigned int year = 0;
    int shiftHour = 0;
    unsigned int shiftMinute = 0;
    sscanf(body, "%2u%2u%2u.%*2c,%2u,%2u,%4u,%2d,%2u",
           &hour, &minute, &second, &day, &month, &year, &shiftHour, &shiftMinute);
  }
  // Разбор строки $--DTM
  else if (!memcmp(&str->data[3], "DTM", 3)) {
    char const* body = &str->data[7];
    char localCode[4] = {'\0'};
    double deltaLat = 0.0;
    char latPol = '\0';
    double deltaLon = 0.0;
    char lonPol = '\0';
    float deltaHeight = 0.0;
    char referenceCode[4] = {'\0'};
    sscanf(body, "%3s,,%lf,%c,%lf,%c,%f,%3s",
           localCode, &deltaLat, &latPol, &deltaLon, &lonPol, &deltaHeight, referenceCode);
    if (!memcmp(localCode, "P90", 3)) sta->gnss.navSystem = gnssNavSystemPZ90_11;
    else if (!memcmp(localCode, "W84", 3)) sta->gnss.navSystem = gnssNavSystemWGS84;
    else if (!memcmp(localCode, "999", 3)) sta->gnss.navSystem = gnssNavSystemUser;
  }
  else if (!memcmp(&str->data[3], "RLM", 3)) {
  }
  else if (!memcmp(&str->data[3], "SGG", 3)) {
    if (!memcmp(&str->data[7], "RQUERY", 6)) {
    }
    else if (!memcmp(&str->data[7], "NQUERY", 6)) {
      int8_t numPort = 0;
      switch (str->data[14]) {
        case '0': numPort = 0; break;
        case '1': numPort = 1; break;
        default: return;  // Невалидный номер порта
      }
      sta->gnss.issueRate[numPort].outData = str->data[16];
      sta->gnss.issueRate[numPort].nmeaVer = str->data[18];
      sta->gnss.issueRate[numPort].gga = str->data[20];
      sta->gnss.issueRate[numPort].gns = str->data[22];
      sta->gnss.issueRate[numPort].gsa = str->data[24];
      sta->gnss.issueRate[numPort].gsv = str->data[26];
      sta->gnss.issueRate[numPort].rmc = str->data[28];
      sta->gnss.issueRate[numPort].vtg = str->data[30];
      sta->gnss.issueRate[numPort].gll = str->data[32];
      sta->gnss.issueRate[numPort].zda = str->data[34];
      sta->gnss.issueRate[numPort].dtm = str->data[36];
      sta->gnss.issueRate[numPort].rlm = str->data[38];
    }
  }
}
