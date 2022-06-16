#include <string.h>
#include "util_heap.h"
#include "flash.h"
#include "mt25q.h"

#define PARAMS_ADR  0x000000
//#define WRITE_DEFAULT_IP

#if defined(WRITE_DEFAULT_IP)
static uint32_t convert_ip(uint8_t quarter0, uint8_t quarter1, uint8_t quarter2, uint8_t quarter3)
{
  return (uint32_t)quarter0 << 24 | (uint32_t)quarter1 << 16 | (uint32_t)quarter2 << 8 | (uint32_t)quarter3;
}
#endif

void flash_read_params(Station *sta)
{
  const FlashStaRevision currentRev = {
      .release = GLOBAL_VERSION,
      .function = FUNCTION_VERSION,
      .bug = BUG_VERSION
  };
  mt25q_read(mt25q_instance(), PARAMS_ADR, (uint8_t*)&sta->flash, sizeof(sta->flash));
  if(*(uint32_t*)&sta->flash.rev != *(uint32_t*)&currentRev) {
    sta->flash.rev = currentRev;
    flash_write_params(sta);
  }
#if defined(WRITE_DEFAULT_IP)
  else {
    sta->flash.ip[0].whole = convert_ip(192, 168, 9, 10);
    sta->flash.ip[0].port = 6401;
    sta->flash.ip[1].whole = convert_ip(194, 58, 79, 38);
    sta->flash.ip[1].port = 6401;
    sta->flash.ip[2].whole = convert_ip(194, 58, 79, 38);
    sta->flash.ip[2].port = 6401;
    sta->flash.ip[3].whole = convert_ip(194, 58, 79, 38);
    sta->flash.ip[3].port = 6401;
    flash_write_params(sta);
  }
#endif
}

void flash_write_params(Station *sta)
{
  mt25q_erase_subsector_4kb(mt25q_instance(), PARAMS_ADR);
  FlashMemory *flashMem = util_malloc(sizeof(sta->flash));
  memcpy(flashMem, &sta->flash, sizeof(sta->flash));
  mt25q_write(mt25q_instance(), PARAMS_ADR, (uint8_t*)flashMem, sizeof(sta->flash));
}
