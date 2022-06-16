#ifndef INC_MI_CMD_H_
#define INC_MI_CMD_H_

#include "stm32f4xx.h"
#include "flow_ctrl_base.h"
#include "sta_base.h"

void mi_cmd_set_id(Station const *sta, FlowCtrl const *flow);
void mi_cmd_selftest(Station const *sta, FlowCtrl const *flow);
void mi_cmd_set_coord(Station const *sta, FlowCtrl const *flow);
void mi_cmd_set_time(Station const *sta, FlowCtrl const *flow);
void mi_cmd_set_ethernet_address(Station const *sta, FlowCtrl const *flow);
void mi_cmd_set_gsm_address(Station const *sta, FlowCtrl const *flow);
void mi_cmd_set_network_address(Station const *sta, FlowCtrl const *flow);
void mi_cmd_select_network_point(Station const *sta, FlowCtrl const *flow, MI_NetworkPoint point);
void mi_cmd_rtk_subscription(Station const *sta, FlowCtrl const *flow, Tumbler tumbler);
void mi_cmd_ppp_subscription(Station const *sta, FlowCtrl const *flow, Tumbler tumbler);

#endif /* INC_MI_CMD_H_ */
