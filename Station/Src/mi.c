#include <string.h>
#include "flow_ctrl.h"
#include "mi.h"
#include "mi_cmd.h"

#define BEGIN_MI_MSG      "MI\r\n"
#define BEGIN_MI_MSG_LEN  (sizeof("MI\r\n") - 1)
#define END_MI_MSG        "END_LSN_DATA\r\n"
#define END_MI_MSG_LEN    (sizeof("END_LSN_DATA\r\n") - 1)
#define END_LINE          "\r\n"
#define END_LINE_LEN      (sizeof("\r\n") - 1)

typedef enum {
  msgsOk,
  msgsBeginErr,
  msgsEndErr
} MessageStat;

void mi_init(Station *sta)
{
  sta->mi.subscribe.rtk = bFalse;
  sta->mi.subscribe.ppp = bFalse;
  sta->mi.networkPoint = miPointProgress;
}

void mi_config(Station const *sta, FlowCtrl const *flow)
{
  mi_cmd_selftest(sta, flow);
  mi_cmd_set_time(sta, flow);
  mi_cmd_set_id(sta, flow);
  mi_cmd_set_ethernet_address(sta, flow);
  mi_cmd_set_gsm_address(sta, flow);
  mi_cmd_set_network_address(sta, flow);
  mi_cmd_select_network_point(sta, flow, miPointProgress);
}

void mi_cmd_analysis(String const* str, FlowPort srcPort, Station* sta)
{
  flow_mi_give(&flowCtrl, str);
}

static MessageStat check_response(String const* str)
{
  if(memcmp(str->data, BEGIN_MI_MSG, BEGIN_MI_MSG_LEN))
    return msgsBeginErr;
  if(memcmp(&str->data[str->len-END_MI_MSG_LEN], END_MI_MSG, END_MI_MSG_LEN))
    return msgsEndErr;
  return msgsOk;
}

static void parser_station_id(char const *begin, size_t len, Station *sta)
{
}

static void parser_mi_ok(char const *begin, size_t len, Station *sta)
{
}

static void parser_current_position(char const *begin, size_t len, Station *sta)
{
}

static void parser_current_time(char const *begin, size_t len, Station *sta)
{
}

static void parser_network_point(char const *begin, size_t len, Station *sta)
{
}

static void parser_ip(char const *begin, size_t len, Station *sta)
{
}

static void parser_rtk(char const *begin, size_t len, Station *sta)
{
  char const *patternOn = "RtkOn";
  char const *patternOff = "RtkOff";
  if(buffer_find((uint8_t const*)begin, len, (uint8_t const*)patternOn, sizeof("RtkOn")-1))
    sta->mi.subscribe.rtk = bTrue;
  else if(buffer_find((uint8_t const*)begin, len, (uint8_t const*)patternOff, sizeof("RtkOff")-1))
    sta->mi.subscribe.rtk = bFalse;
}

static void parser_ppp(char const *begin, size_t len, Station *sta)
{
  char const *patternOn = "PppOn";
  char const *patternOff = "PppOff";
  if(buffer_find((uint8_t const*)begin, len, (uint8_t const*)patternOn, sizeof("PppOn")-1))
    sta->mi.subscribe.ppp = bTrue;
  else if(buffer_find((uint8_t const*)begin, len, (uint8_t const*)patternOff, sizeof("PppOff")-1))
    sta->mi.subscribe.ppp = bFalse;
}

void mi_response_analysis(String const* str, FlowPort srcPort, Station* sta)
{
  if(check_response(str) != msgsOk)
    return;

  char const *beginLine = str_cdata(str);
  char const *endLine = strstr(beginLine, END_LINE) + END_LINE_LEN;

  // Пропуск преамбулы сообщения
  beginLine = endLine;
  endLine = strstr(beginLine, END_LINE);
  size_t lenInformation = endLine - beginLine;
  endLine += END_LINE_LEN;

  if(memcmp(beginLine, "MsgFail", lenInformation) == 0)
    return;

  while(*endLine != '\0') {
    beginLine = endLine;
    endLine = strstr(beginLine, END_LINE);
    lenInformation = endLine - beginLine;
    endLine += END_LINE_LEN;

    if(memcmp(beginLine, "StationId", 9) == 0)
      parser_station_id(beginLine, lenInformation, sta);
    else if(memcmp(beginLine, "MiOk", 4) == 0)
      parser_mi_ok(beginLine, lenInformation, sta);
    else if(memcmp(beginLine, "CurrentPosition", 15) == 0)
      parser_current_position(beginLine, lenInformation, sta);
    else if(memcmp(beginLine, "CurrentTime", 11) == 0)
      parser_current_time(beginLine, lenInformation, sta);
    else if(memcmp(beginLine, "Ip", 2) == 0)
      parser_ip(beginLine, lenInformation, sta);
    else if(memcmp(beginLine, "ServType", 8) == 0)
      parser_network_point(beginLine, lenInformation, sta);
    else if(memcmp(beginLine, "Rtk", 3) == 0)
      parser_rtk(beginLine, lenInformation, sta);
    else if(memcmp(beginLine, "Ppp", 3) == 0)
      parser_ppp(beginLine, lenInformation, sta);
  }
}
