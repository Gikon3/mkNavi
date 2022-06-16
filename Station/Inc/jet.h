#ifndef INC_JET_H_
#define INC_JET_H_

#include "stm32f4xx_hal.h"
#include "util_string.h"
#include "sta_base.h"
#include "flow_ctrl_base.h"

#define BEGIN_JET_MSG       "$MSG"
#define BEGIN_JET_MSG_LEN   (sizeof("$MSG") - 1)
#define END_JET_MSG         "END."
#define END_JET_MSG_LEN     (sizeof("END.") - 1)
#define END_JET_PACK        "\r\n"
#define END_JET_PACK_LEN    (sizeof("\r\n") - 1)

typedef enum {
  jetPackSens     = 0x01,
  jetPackGNSS     = 0x02,
  jetPackLSN      = 0x03,
  jetPackSolution = 0x80,
  jetPackBlu      = 0x81,
  jetPackWiFi     = 0x82,
  jetPackGPS      = 0x83
} JetPack;

void jet_init();
void jet_config(Station* sta, FlowCtrl const* flow);
void jet_cmd_analysis(String const* str, FlowPort srcPort, Station* sta);

#endif /* INC_JET_H_ */
