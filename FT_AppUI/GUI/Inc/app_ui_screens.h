#ifndef INC_APP_UI_SCREENS_H_
#define INC_APP_UI_SCREENS_H_

#include "app_ui_base.h"

void app_ui_statusbar_create(FT_Core *core, FT_Screen *screen, AppUI_StatusBar *widgets);
Bool app_ui_statusbar_refresh(AppUI_StatusBar *widgets);

FT_Screen* app_ui_screen_mainmenu(FT_Core *core, AppUI *ui);
Bool app_ui_screen_mainmenu_refresh(AppUI *ui);

FT_Screen *app_ui_screen_lsn(FT_Core *core, AppUI *ui);
Bool app_ui_screen_lsn_refresh(AppUI *ui);

FT_Screen *app_ui_screen_gnss(FT_Core *core, AppUI *ui);
Bool app_ui_screen_gnss_refresh(AppUI *ui);

FT_Screen* app_ui_screen_terminal(FT_Core *core, AppUI *ui);
Bool app_ui_screen_terminal_refresh(AppUI *ui);
void app_ui_screen_terminal_clear(AppUI *ui);

FT_Screen *app_ui_screen_bluetooth(FT_Core *core, AppUI *ui);
Bool app_ui_screen_bluetooth_refresh(AppUI *ui);

FT_Screen* app_ui_screen_log(FT_Core *core, AppUI *ui);
Bool app_ui_screen_log_refresh(AppUI *ui);

FT_Screen* app_ui_screen_debug(FT_Core *core, AppUI *ui);
Bool app_ui_screen_debug_refresh(AppUI *ui);

FT_Screen* app_ui_screen_debug_touch(FT_Core *core, AppUI *ui);
Bool app_ui_screen_debug_touch_refresh(AppUI *ui);

FT_Screen* app_ui_screen_debug_gesture(FT_Core *core, AppUI *ui);
Bool app_ui_screen_debug_gesture_refresh(AppUI *ui);

FT_Screen* app_ui_screen_sensors(FT_Core *core, AppUI *ui);
Bool app_ui_screen_sensors_refresh(AppUI *ui);

FT_Screen* app_ui_screen_jetson(FT_Core *core, AppUI *ui);
Bool app_ui_screen_jetson_refresh(AppUI *ui);

FT_Screen* app_ui_screen_devices(FT_Core *core, AppUI *ui);
Bool app_ui_screen_devices_refresh(AppUI *ui);

#endif /* INC_APP_UI_SCREENS_H_ */
