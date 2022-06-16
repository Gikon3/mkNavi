#ifndef INC_JAVAD_CMD_H_
#define INC_JAVAD_CMD_H_

#include "stm32f4xx.h"
#include "util_string.h"
#include "sta_base.h"
#include "flow_ctrl_base.h"

typedef enum {
  javInModeCmd,
  javInModeEcho,
  javInModeJPS,
  javInModeRTCM,
  javInModeRTCM3,
  javInModeCMR,
  javInModeOMNI,
  javInModeDTP,
  javInModeTerm
} JavadInMode;

// Порядок важен; используется как id в массиве
typedef enum {
  javNCmdGGA_On = 0,
  javNCmdGGA_Off,
  javNCmdGLL_On,
  javNCmdGLL_Off,
  javNCmdGNS_On,
  javNCmdGNS_Off,
  javNCmdGRS_On,
  javNCmdGRS_Off,
  javNCmdGSA_On,
  javNCmdGSA_Off,
  javNCmdGST_On,
  javNCmdGST_Off,
  javNCmdGSV_On,
  javNCmdGSV_Off,
  javNCmdRMC_On,
  javNCmdRMC_Off,
  javNCmdHDT_On,
  javNCmdHDT_Off,
  javNCmdVTG_On,
  javNCmdVTG_Off,
  javNCmdROT_On,
  javNCmdROT_Off,
  javNCmdZDA_On,
  javNCmdZDA_Off,
  javNCmdGMP_On,
  javNCmdGMP_Off
} JavadNMEA_Cmd;

void javad_send_nmeacmd(Station* sta, FlowPort port, JavadNMEA_Cmd cmd);
void javad_send(FlowPort port, char const* str);
void javad_set_imode(Station const* sta, FlowPort port, char serPort, JavadInMode imode);
void javad_switch_solution(Station* sta, FlowPort port, GNSS_NavMode sol);
void javad_en_code_diff_pos(Station* sta, FlowPort port, Bool mode);
void javad_en_sbas_code_diff_pos(Station* sta, FlowPort port, Bool mode);
void javad_en_sattelite_system(Station* sta, FlowPort port, GNSS_SatSystem system);

#endif /* INC_JAVAD_CMD_H_ */
