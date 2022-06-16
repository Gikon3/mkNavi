#ifndef INC_FLOW_CTRL_H_
#define INC_FLOW_CTRL_H_

#include "stm32f4xx_hal.h"
#include "util_string.h"
#include "sta_base.h"
#include "flow_ctrl_base.h"

typedef void (*data_proc_fp_t)(String const*, FlowPort, Station*);

extern FlowCtrl flowCtrl;

void flow_init();

void flow_ppm0_process(FlowCtrl *flow, Station *sta);
void flow_ppm1_process(FlowCtrl *flow, Station *sta);
void flow_gnss0_process(FlowCtrl *flow, Station *sta);
void flow_gnss1_process(FlowCtrl *flow, Station *sta);
void flow_mi_process(FlowCtrl *flow, Station *sta);
void flow_com_process(FlowCtrl *flow, Station *sta);
void flow_jet_process(FlowCtrl *flow, Station *sta);
void flow_bluetooth_append_data(FlowCtrl *flow, uint8_t const *data, size_t size);
void flow_bluetooth_process(FlowCtrl *flow, Station *sta);

void flow_ppm0_give_own(FlowCtrl const *flow, String *str);
void flow_ppm1_give_own(FlowCtrl const *flow, String *str);
void flow_gnss0_give_own(FlowCtrl const *flow, String *str);
void flow_gnss1_give_own(FlowCtrl const *flow, String *str);
void flow_mi_give_own(FlowCtrl const *flow, String *str);
void flow_com_give_own(FlowCtrl const *flow, String *str);
void flow_jet_give_own(FlowCtrl const *flow, String *str);
void flow_bluetooth_give_own(FlowCtrl const *flow, String *str);
void flow_screen_give_own(FlowCtrl const *flow, String *str, FlowPort srcPort);

void flow_ppm0_give(FlowCtrl const *flow, String const *str);
void flow_ppm1_give(FlowCtrl const *flow, String const *str);
void flow_gnss0_give(FlowCtrl const *flow, String const *str);
void flow_gnss1_give(FlowCtrl const *flow, String const *str);
void flow_mi_give(FlowCtrl const *flow, String const *str);
void flow_com_give(FlowCtrl const *flow, String const *str);
void flow_jet_give(FlowCtrl const *flow, String const *str);
void flow_bluetooth_give(FlowCtrl const *flow, String const *str);
void flow_sd_give(FlowCtrl const *flow, String const *str, FlowPort srcPort);
void flow_screen_give(FlowCtrl const *flow, String const *str, FlowPort srcPort);

void flow_gnss0_tx_data(FlowCtrl *flow, Station const *sta);
void flow_gnss1_tx_data(FlowCtrl *flow, Station const *sta);

#endif /* INC_FLOW_CTRL_H_ */
