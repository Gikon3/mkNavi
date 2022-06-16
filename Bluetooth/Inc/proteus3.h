#ifndef INC_PROTEUS3_H_
#define INC_PROTEUS3_H_

#include "proteus3_types.h"
#include "proteus3_commands.h"

Proteus3* proteus3_create(uint8_t const *buffer, size_t size, Proteus3Callbacks const *callbacks);
void proteus3_destroy(Proteus3 *proteus3);
void proteus3_message_parser(Proteus3 *proteus3);
void proteus3_pwrite_set(Proteus3 *proteus3, size_t txSize);
Bool proteus3_ready(Proteus3 *proteus3);
Bool proteus3_connect_state(Proteus3 const *proteus3, uint8_t *btmac);
Proteus3State proteus3_state(Proteus3 const *proteus3);
Proteus3Devices const* proteus3_devices(Proteus3 const *proteus3);

#endif /* INC_PROTEUS3_H_ */
