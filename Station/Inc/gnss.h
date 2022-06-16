#ifndef INC_PRO4_H_
#define INC_PRO4_H_

#include "stm32f4xx_hal.h"
#include "util_string.h"
#include "sta_base.h"
#include "flow_ctrl_base.h"
#include "gnss_cmd.h"
#include "javad_cmd.h"

void gnss_init();
void gnss_config(Station* sta, FlowCtrl const* flow);
void gnss_power(Station* sta, Power state);
GNSS_SyncDateTime const* gnss_datetime(Station const* sta);
GNSS_IssueRate const* gnss_issue_rate0(Station const* sta);
GNSS_IssueRate const* gnss_issue_rate1(Station const* sta);
void gnss_sync_time(Station* sta);
HAL_StatusTypeDef gnss_uart0_tx(FlowCtrl const* flow, String const* str);
HAL_StatusTypeDef gnss_uart1_tx(FlowCtrl const* flow, String const* str);
void gnss_resp_data_proc(String const* str, FlowPort srcPort, Station* sta);

#endif /* INC_PRO4_H_ */
