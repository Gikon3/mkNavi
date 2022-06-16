#ifndef INC_PPM_CMD_H_
#define INC_PPM_CMD_H_

#include "stm32f4xx_hal.h"
#include "util_string.h"
#include "flow_ctrl_base.h"
#include "sta_base.h"

void ppm_cmd_mode(Station const* sta, FlowPort port);
void ppm_cmd_info_exchange(Station const* sta, FlowPort port);
void ppm_cmd_id(Station const* sta, FlowPort port);
void ppm_cmd_data_xfer(Station const* sta, FlowPort port);
void ppm_cmd_cont_radiation(Station const* sta, FlowPort port);
void ppm_cmd_setting_out_data(Station const* sta, FlowPort port);
void ppm_cmd_psp_number(Station const* sta, FlowPort port);
void ppm_cmd_rppu_reg(Station const* sta, FlowPort port);
void ppm_cmd_selftest(Station const* sta, FlowPort port);
void ppm_cmd_rec_ifa(Station const* sta, FlowPort port);
void ppm_cmd_rec_frq(Station const* sta, FlowPort port);
void ppm_cmd_position(Station const* sta, FlowPort port);
void ppm_cmd_rec_lna(Station const* sta, FlowPort port);
void ppm_cmd_tr_frq(Station const* sta, FlowPort port);
void ppm_cmd_tr_att(Station const* sta, FlowPort port);
void ppm_cmd_req_params(Station const* sta, FlowPort port);
void ppm_cmd_amp(Station const* sta, FlowPort port, PpmOutConnect outAnt);
void ppm_cmd_bank_pll(Station const* sta, FlowPort port);
void ppm_cmd_latitude_station(Station const* sta, FlowPort port);
void ppm_cmd_longitude_station(Station const* sta, FlowPort port);
void ppm_cmd_calc_correlation_peak(Station const* sta, FlowPort port);

#endif /* INC_PPM_CMD_H_ */
