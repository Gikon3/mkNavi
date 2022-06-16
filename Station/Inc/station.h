#ifndef INC_BASE_H_
#define INC_BASE_H_

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"
#include "util_string.h"
#include "flow_ctrl_base.h"
#include "sta_base.h"

#define MAX_CNT_OF_MESSAGE        3

extern Station station;

void sta_init();
void sta_calculate(Station* sta);
void sta_send_navi(Station const* sta);
void sta_send_sys_info(Station* sta);
void sta_send_telemetry(Station* sta);
void sta_send_sensor(Station* sta);
void sta_send_jet(FlowCtrl const* flow, Station* sta);

#endif /* INC_BASE_H_ */
