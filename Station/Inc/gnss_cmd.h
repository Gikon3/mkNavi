#ifndef INC_GNSS_CMD_H_
#define INC_GNSS_CMD_H_

#include "stm32f4xx_hal.h"
#include "util_string.h"
#include "sta_base.h"
#include "flow_ctrl_base.h"

// Порядок важен; используется как id в массиве
typedef enum {
  gnssNCmdSwprot = 0,
  gnssNCmdSavefl,
  gnssNCmdCstart,
  gnssNCmdWstart,
  gnssNCmdHstart,
  gnssNCmdRquery,
  gnssNCmdNquery,
  gnssNCmdBdr004,
  gnssNCmdBdr009,
  gnssNCmdBdr019,
  gnssNCmdBdr038,
  gnssNCmdBdr057,
  gnssNCmdBdr115,
  gnssNCmdBdr230,
  gnssNCmdBdr460,
  gnssNCmdBdr921,
  gnssNCmdStop01,
  gnssNCmdStop02,
  gnssNCmdGGA_On,
  gnssNCmdGGA_Off,
  gnssNCmdGLL_On,
  gnssNCmdGLL_Off,
  gnssNCmdGSA_On,
  gnssNCmdGSA_Off,
  gnssNCmdGSV_On,
  gnssNCmdGSV_Off,
  gnssNCmdRMC_On,
  gnssNCmdRMC_Off,
  gnssNCmdVTG_On,
  gnssNCmdVTG_Off,
  gnssNCmdZDA_On,
  gnssNCmdZDA_Off,
  gnssNCmdDTM_On,
  gnssNCmdDTM_Off,
  gnssNCmdRLM_On,
  gnssNCmdRLM_Off,
  gnssNCmdNMEA_V2,
  gnssNCmdNMEA_V4,
  gnssNCmdRate01,
  gnssNCmdRate02,
  gnssNCmdRate05,
  gnssNCmdRate10,
  gnssNCmdElev05,
  gnssNCmdElev06,
  gnssNCmdElev07,
  gnssNCmdElev08,
  gnssNCmdElev09,
  gnssNCmdElev10,
  gnssNCmdElev11,
  gnssNCmdElev12,
  gnssNCmdElev13,
  gnssNCmdElev14,
  gnssNCmdElev15,
  gnssNCmdDatp90,
  gnssNCmdDatw84,
  gnssNCmdNvsGps,
  gnssNCmdNvsGln,
  gnssNCmdNvsMix
} GNSS_NMEA_Cmd;

void gnss_send_nmeacmd(Station* sta, FlowPort port, GNSS_NMEA_Cmd cmd);

#endif /* INC_GNSS_CMD_H_ */
