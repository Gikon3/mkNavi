#ifndef INC_APP_UI_MANAGER_H_
#define INC_APP_UI_MANAGER_H_

#include "app_ui_base.h"

void app_ui_manager_init(AppUI *ui, FT_Core *core);
void app_ui_manager(AppUI *ui);
void app_ui_manager_next_screen(AppUI *ui, app_ui_screen_id_t screen_id);

#endif /* INC_APP_UI_MANAGER_H_ */
