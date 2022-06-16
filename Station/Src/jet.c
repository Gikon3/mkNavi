#include "jet.h"
#include <string.h>
#include "cmsis_os.h"
#include "station.h"
#include "mem_find.h"

typedef enum {
  msgsOk,
  msgsBeginErr
} CheckMsgStat;

void jet_init()
{
  station.jet.solution.updated = bFalse;
  station.jet.solution.time = 0;
  station.jet.solution.coord.lat = 0.0;
  station.jet.solution.coord.lng = 0.0;
  station.jet.solution.system.lsn = bFalse;
  station.jet.solution.system.gnss = bFalse;
  station.jet.solution.system.ins = bFalse;
  station.jet.solution.system.wifi = bFalse;
  station.jet.solution.system.blu = bFalse;
  station.jet.solution.system.gsm = bFalse;
  station.jet.blu.updated = bFalse;
  station.jet.blu.time = 0;
  station.jet.blu.major = 0;
  station.jet.blu.minor = 0;
  memset(station.jet.blu.uuid, 0, sizeof(station.jet.blu.uuid));
  station.jet.blu.rssi = 0;
  station.jet.wifi.updated = bFalse;
  station.jet.wifi.time = 0;
  memset(station.jet.wifi.mac, 0, sizeof(station.jet.wifi.mac));
  station.jet.wifi.distance = 0.0;
  station.jet.gps.updated = bFalse;
  station.jet.gps.time = 0;
  station.jet.gps.coord.lat = 0.0;
  station.jet.gps.coord.lng = 0.0;
}

void jet_config(Station* sta, FlowCtrl const* flow)
{
  HAL_UART_Receive_DMA(flow->raw.jet.uart, (uint8_t*)flow->raw.jet.buff, flow->raw.jet.size);
}

static inline CheckMsgStat check_cmd(String const* str)
{
  if (memcmp(str->data, BEGIN_JET_MSG, BEGIN_JET_MSG_LEN)) return msgsBeginErr;
  return msgsOk;
}
static inline int64_t get_up_to_8(uint8_t numb, char const* accum)
{
  char sign = '+';
  uint64_t ret = 0;
  for (uint8_t i = 0; i < numb; ++i) {
    if (i == 0 && (accum[i] & 0x80)) {
      sign = '-';
      ret |= (uint64_t)(accum[i] & 0x7F) << ((numb - 1 - i) * 8);
    }
    else ret |= (uint64_t)accum[i] << ((numb - 1 - i) * 8);
  }
  return sign == '+' ? ret : -ret;
}
static inline void pack_process(char const* pack, Station* sta)
{
  const JetPack id = pack[0];
  switch (id) {
    case jetPackSolution:
      sta->jet.solution.time = (uint64_t)pack[1] << 56 | (uint64_t)pack[2] << 48 | (uint64_t)pack[3] << 40 | (uint64_t)pack[4] << 32 |
          (uint64_t)pack[5] << 24 | (uint64_t)pack[6] << 16 | (uint64_t)pack[7] << 8 | (uint64_t)pack[8];
      sta->jet.solution.coord.lat = get_up_to_8(6, &pack[9]) / 1E10;
      sta->jet.solution.coord.lng = get_up_to_8(6, &pack[15]) / 1E10;
      sta->jet.solution.system = *(JetSystem*)&pack[21];
      sta->jet.solution.updated = bTrue;
      break;
    case jetPackBlu:
      sta->jet.blu.time = (uint64_t)pack[1] << 56 | (uint64_t)pack[2] << 48 | (uint64_t)pack[3] << 40 | (uint64_t)pack[4] << 32 |
          (uint64_t)pack[5] << 24 | (uint64_t)pack[6] << 16 | (uint64_t)pack[7] << 8 | (uint64_t)pack[8];
      sta->jet.blu.major = (uint32_t)pack[9] << 24 | (uint32_t)pack[10] << 16 | (uint32_t)pack[11] << 8 | (uint32_t)pack[12];
      sta->jet.blu.minor = (uint32_t)pack[13] << 24 | (uint32_t)pack[14] << 16 | (uint32_t)pack[15] << 8 | (uint32_t)pack[16];
      memcpy(sta->jet.blu.uuid, &pack[17], sizeof(sta->jet.blu.uuid));
      const uint16_t rssiUnsign = ((uint16_t)pack[33] << 8 | (uint16_t)pack[34]) & 0x7FFF;
      sta->jet.blu.rssi = pack[33] & 0x80 ? -rssiUnsign : rssiUnsign;
      sta->jet.blu.updated = bTrue;
      break;
    case jetPackWiFi:
      sta->jet.wifi.time = (uint64_t)pack[1] << 56 | (uint64_t)pack[2] << 48 | (uint64_t)pack[3] << 40 | (uint64_t)pack[4] << 32 |
          (uint64_t)pack[5] << 24 | (uint64_t)pack[6] << 16 | (uint64_t)pack[7] << 8 | (uint64_t)pack[8];
      memcpy(sta->jet.wifi.mac, &pack[9], sizeof(sta->jet.wifi.mac));
      sta->jet.wifi.distance = ((uint32_t)pack[15] << 24 | (uint32_t)pack[16] << 16 | (uint32_t)pack[17] << 8 | (uint32_t)pack[18]) / 1000.0;
      sta->jet.wifi.updated = bTrue;
      break;
    case jetPackGPS:
      sta->jet.gps.time = (uint64_t)pack[1] << 56 | (uint64_t)pack[2] << 48 | (uint64_t)pack[3] << 40 | (uint64_t)pack[4] << 32 |
          (uint64_t)pack[5] << 24 | (uint64_t)pack[6] << 16 | (uint64_t)pack[7] << 8 | (uint64_t)pack[8];
      sta->jet.gps.coord.lat = get_up_to_8(6, &pack[9]) / 1E10;
      sta->jet.gps.coord.lng = get_up_to_8(6, &pack[15]) / 1E10;
      sta->jet.gps.updated = bTrue;
      break;
    default:
      break;
  }
}
void jet_cmd_analysis(String const* str, FlowPort srcPort, Station* sta)
{
  if (check_cmd(str) != msgsOk) return;

  char const* start = str->data + BEGIN_JET_MSG_LEN;
  char const* end = NULL;
  while ((end = mem_find(start, str->len, END_JET_PACK, END_JET_PACK_LEN))) {
    pack_process(start, sta);
    start = end;
  }
}
