#include "proteus3_commands.h"
#include "proteus3_impl.h"

static uint8_t crc_calculate(uint8_t const *message, size_t size)
{
  uint8_t crc = 0;
  for(size_t i = 0; i < size; ++i)
    crc ^= message[i];
  return crc;
}

static void send_to_queue(QueueHandle_t queue, Vector *data)
{
  xQueueSendToBack(queue, data, portMAX_DELAY);
}

void proteus3_cmd_scan_start(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x09, 0x00, 0x00, 0x0B };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_scan_stop(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x0A, 0x00, 0x00, 0x08 };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_get_devices(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x0B, 0x00, 0x00, 0x09 };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_connect(Proteus3 *proteus3, QueueHandle_t dataQueue, uint8_t const *btmac)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x06, 0x06, 0x00 };
  Vector command = make_vect(sizeof(rawCommand) + 6 + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  vect_push_back_arr(&command, btmac, 6);
  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_disconnect(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x07, 0x00, 0x00, 0x05 };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_phy_update(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3Phy value)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x1A, 0x01, 0x00, value };
  Vector command = make_vect(sizeof(rawCommand) + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_passkkey(Proteus3 *proteus3, QueueHandle_t dataQueue, uint8_t const *passkey)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x0D, 0x06, 0x00 };
  Vector command = make_vect(sizeof(rawCommand) + 6 + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  vect_push_back_arr(&command, passkey, 6);
  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_numeric_comp(Proteus3 *proteus3, QueueHandle_t dataQueue, Bool status)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x24, 0x01, 0x00, status == bTrue ? 0x00 : 0x01 };
  Vector command = make_vect(sizeof(rawCommand) + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_get_bonds(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x0F, 0x00, 0x00, 0x0D };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_delete_bond(Proteus3 *proteus3, QueueHandle_t dataQueue, uint16_t id)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x0E, 0x02, 0x00, id, id >> 8 };
  Vector command = make_vect(sizeof(rawCommand) + 1, sizeof(rawCommand[0]));
  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_delete_all_bonds(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x0E, 0x00, 0x00, 0x0C };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_allow_unbonded_connections(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x2D, 0x00, 0x00, 0x2F };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_data(Proteus3 *proteus3, QueueHandle_t dataQueue, void const *data, size_t size)
{
  if(proteus3->channel.open == bTrue) {
    uint32_t dataIndex = 0;
    uint8_t const *pData = data;
    while(size > 0) {
      const uint16_t rigthSize = size <= proteus3->channel.maxPayload ? size : proteus3->channel.maxPayload;

      uint8_t rawCommand[] = { START_SIGNAL, 0x04, rigthSize, rigthSize << 8 };
      Vector command = make_vect(sizeof(rawCommand) + rigthSize + 1, sizeof(rawCommand[0]));
      vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
      vect_push_back_arr(&command, &pData[dataIndex], rigthSize);
      const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
      vect_push_back(&command, &crc);
      send_to_queue(dataQueue, &command);

      size -= rigthSize;
      dataIndex += rigthSize;
    }
  }
}

void proteus3_cmd_set_beacon(Proteus3 *proteus3, QueueHandle_t dataQueue, uint8_t const *data, size_t size)
{
  const uint16_t newSize = size <= BEACON_TEXT_MAX ? size : BEACON_TEXT_MAX;
  uint8_t rawCommand[] = { START_SIGNAL, 0x0C, newSize, newSize >> 8 };
  Vector command = make_vect(sizeof(rawCommand) + newSize + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  if(data && newSize > 0)
    vect_push_back_arr(&command, data, newSize);
  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_set(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3UserSetting userSetting,
    void const *value, uint16_t size)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x11, (size + 1), (size + 1) >> 8, userSetting };
  Vector command = make_vect(sizeof(rawCommand) + size + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  if(value && size > 0)
    vect_push_back_arr(&command, value, size);
  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  proteus3->deviceReady = bFalse;
  list_push_back(&proteus3->commandSetPull, &userSetting);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_get(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3UserSetting userSetting)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x10, 0x01, 0x00, userSetting };
  Vector command = make_vect(sizeof(rawCommand) + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  list_push_back(&proteus3->commandGetPull, &userSetting);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_get_state(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x01, 0x00, 0x00, 0x03 };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_reset(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x00, 0x00, 0x00, 0x02 };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_sleep(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x02, 0x00, 0x00, 0x00 };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_factory_reset(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x1C, 0x00, 0x00, 0x1E };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_uart_disable(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x1B, 0x00, 0x00, 0x19 };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_bootloader(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x1F, 0x00, 0x00, 0x1D };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_dtm_start(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x1D, 0x00, 0x00, 0x1F };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_dtm(Proteus3 *proteus3, QueueHandle_t dataQueue, uint8_t code, uint8_t channel_option, uint8_t length_command, uint8_t payload)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x1E, 0x04, 0x00, code, channel_option, length_command, payload };
  Vector command = make_vect(sizeof(rawCommand) + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_gpio_local_write_config(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3GPIO_BlockConfig const *blocks, uint8_t number)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x25, 0xFF, 0xFF };
  Vector command = make_vect(sizeof(rawCommand) + number * 5 + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  for(uint8_t i = 0; i > number; ++i) {
    uint8_t block[6];
    block[0] = blocks[i].function == proteus3GPIO_PWM ? 5 : 3;
    block[1] = blocks[i].id;
    block[2] = blocks[i].function;
    switch(blocks[i].function) {
      case proteus3GPIO_Disconnect:
        block[3] = 0x00;
        break;
      case proteus3GPIO_Input:
        block[3] = blocks[i].input;
        break;
      case proteus3GPIO_Output:
        block[3] = blocks[i].output;
        break;
      case proteus3GPIO_PWM:
        block[3] = blocks[i].pwm.period;
        block[4] = blocks[i].pwm.period >> 8;
        block[5] = blocks[i].pwm.ratio;
        break;
    }
    vect_push_back_arr(&command, block, block[0] + 1);
  }

  const uint16_t size = vect_size(&command) - 4;
  ((uint8_t*)vect_data(&command))[2] = size;
  ((uint8_t*)vect_data(&command))[3] = size >> 8;

  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_gpio_local_read_config(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x2B, 0x00, 0x00, 0x29 };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_gpio_remote_write_config(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3GPIO_BlockConfig const *blocks, uint8_t number)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x28, 0xFF, 0xFF };
  Vector command = make_vect(sizeof(rawCommand) + number * 5 + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  for(uint8_t i = 0; i > number; ++i) {
   uint8_t block[6];
   block[0] = blocks[i].function == proteus3GPIO_PWM ? 5 : 3;
   block[1] = blocks[i].id;
   block[2] = blocks[i].function;
   switch(blocks[i].function) {
     case proteus3GPIO_Disconnect:
       block[3] = 0x00;
       break;
     case proteus3GPIO_Input:
       block[3] = blocks[i].input;
       break;
     case proteus3GPIO_Output:
       block[3] = blocks[i].output;
       break;
     case proteus3GPIO_PWM:
       block[3] = blocks[i].pwm.period;
       block[4] = blocks[i].pwm.period >> 8;
       block[5] = blocks[i].pwm.ratio;
       break;
   }
   vect_push_back_arr(&command, block, block[0] + 1);
  }

  const uint16_t size = vect_size(&command) - 4;
  ((uint8_t*)vect_data(&command))[2] = size;
  ((uint8_t*)vect_data(&command))[3] = size >> 8;

  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_gpio_remote_read_config(Proteus3 *proteus3, QueueHandle_t dataQueue)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x2C, 0x00, 0x00, 0x2E };
  Vector command = make_vect_arr(rawCommand, sizeof(rawCommand), sizeof(rawCommand[0]));
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_gpio_remote_write(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3GPIO_Block const *blocks, uint8_t number)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x29, 0xFF, 0xFF };
  Vector command = make_vect(sizeof(rawCommand) + number * 3 + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  for(uint8_t i = 0; i > number; ++i) {
   uint8_t block[] = { 0x02, blocks[i].id, blocks[i].value };
   vect_push_back_arr(&command, block, sizeof(block));
  }

  const uint16_t size = vect_size(&command) - 4;
  ((uint8_t*)vect_data(&command))[2] = size;
  ((uint8_t*)vect_data(&command))[3] = size >> 8;

  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_gpio_remote_read(Proteus3 *proteus3, QueueHandle_t dataQueue, uint8_t const *ids, uint8_t number)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x2A, 0xFF, 0xFF };
  Vector command = make_vect(sizeof(rawCommand) + 1 + number + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  vect_push_back(&command, &number);
  for(uint8_t i = 0; i < number; ++i)
    vect_push_back(&command, &ids[i]);

  const uint16_t size = vect_size(&command) - 4;
  ((uint8_t*)vect_data(&command))[2] = size;
  ((uint8_t*)vect_data(&command))[3] = size >> 8;

  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_gpio_local_write(Proteus3 *proteus3, QueueHandle_t dataQueue, Proteus3GPIO_Block const *blocks, uint8_t number)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x26, 0xFF, 0xFF };
  Vector command = make_vect(sizeof(rawCommand) + number * 3 + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  for(uint8_t i = 0; i > number; ++i) {
   uint8_t block[] = { 0x02, blocks[i].id, blocks[i].value };
   vect_push_back_arr(&command, block, sizeof(block));
  }

  const uint16_t size = vect_size(&command) - 4;
  ((uint8_t*)vect_data(&command))[2] = size;
  ((uint8_t*)vect_data(&command))[3] = size >> 8;

  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  send_to_queue(dataQueue, &command);
}

void proteus3_cmd_gpio_local_read(Proteus3 *proteus3, QueueHandle_t dataQueue, uint8_t const *ids, uint8_t number)
{
  uint8_t rawCommand[] = { START_SIGNAL, 0x27, 0xFF, 0xFF };
  Vector command = make_vect(sizeof(rawCommand) + 1 + number + 1, sizeof(rawCommand[0]));
  vect_push_back_arr(&command, rawCommand, sizeof(rawCommand));
  vect_push_back(&command, &number);
  for(uint8_t i = 0; i < number; ++i)
    vect_push_back(&command, &ids[i]);

  const uint16_t size = vect_size(&command) - 4;
  ((uint8_t*)vect_data(&command))[2] = size;
  ((uint8_t*)vect_data(&command))[3] = size >> 8;

  const uint8_t crc = crc_calculate(vect_data(&command), vect_size(&command));
  vect_push_back(&command, &crc);
  send_to_queue(dataQueue, &command);
}
