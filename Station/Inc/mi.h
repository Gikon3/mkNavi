#ifndef INC_MI_H_
#define INC_MI_H_

#include "stm32f4xx.h"
#include "util_string.h"
#include "flow_ctrl_base.h"
#include "sta_base.h"

void mi_init(Station *sta);
void mi_config(Station const *sta, FlowCtrl const *flow);
void mi_cmd_analysis(String const* str, FlowPort srcPort, Station* sta);
void mi_response_analysis(String const* str, FlowPort srcPort, Station* sta);

#endif /* INC_MI_H_ */
