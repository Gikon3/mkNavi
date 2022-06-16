#ifndef INC_PROTEUS3_COMMANDS_H_
#define INC_PROTEUS3_COMMANDS_H_

#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "queue.h"
#include "util_vector.h"
#include "proteus3_types.h"

void proteus3_cmd_scan_start(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_scan_stop(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_get_devices(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_connect(Proteus3 *proteus3, QueueHandle_t dataQueue, uint8_t const *btmac);
void proteus3_cmd_disconnect(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_phy_update(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3Phy value);
void proteus3_cmd_passkkey(Proteus3 *proteus3, QueueHandle_t dataQueue, uint8_t const *passkey);
void proteus3_cmd_numeric_comp(Proteus3 *proteus3, QueueHandle_t dataQueue, Bool status);
void proteus3_cmd_get_bonds(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_delete_bond(Proteus3 *proteus3, QueueHandle_t dataQueue, uint16_t id);
void proteus3_cmd_delete_all_bonds(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_allow_unbonded_connections(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_data(Proteus3 *proteus3, QueueHandle_t dataQueue, void const *data, size_t size);
void proteus3_cmd_set_beacon(Proteus3 *proteus3, QueueHandle_t dataQueue, uint8_t const *data, size_t size);
void proteus3_cmd_set(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3UserSetting userSetting, void const *value, uint16_t size);
void proteus3_cmd_get(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3UserSetting userSetting);
void proteus3_cmd_get_state(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_reset(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_sleep(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_factory_reset(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_uart_disable(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_bootloader(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_dtm_start(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_dtm(Proteus3 *proteus3, QueueHandle_t dataQueue, uint8_t code, uint8_t channel_option, uint8_t length_command, uint8_t payload);
void proteus3_cmd_gpio_local_write_config(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3GPIO_BlockConfig const *blocks, uint8_t number);
void proteus3_cmd_gpio_local_read_config(Proteus3 *proteus, QueueHandle_t dataQueue3);
void proteus3_cmd_gpio_remote_write_config(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3GPIO_BlockConfig const *blocks, uint8_t number);
void proteus3_cmd_gpio_remote_read_config(Proteus3 *proteus3, QueueHandle_t dataQueue);
void proteus3_cmd_gpio_remote_write(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3GPIO_Block const *blocks, uint8_t number);
void proteus3_cmd_gpio_remote_read(Proteus3 *proteus3, QueueHandle_t dataQueue, uint8_t const *ids, uint8_t number);
void proteus3_cmd_gpio_local_write(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3GPIO_Block const *blocks, uint8_t number);
void proteus3_cmd_gpio_local_read(Proteus3 *proteus3, QueueHandle_t dataQueue, uint8_t const *ids, uint8_t number);

#endif /* INC_PROTEUS3_COMMANDS_H_ */
