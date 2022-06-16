#ifndef INC_BLUETOOTH_H_
#define INC_BLUETOOTH_H_

#include "stm32f4xx.h"
#include "util_types.h"

struct _Bluetooth;
typedef struct _Bluetooth Bluetooth;

typedef enum
{
  blthStateNoAction,
  blthStateIdle,
  blthStateScanning,
  blthStateConnected,
  blthStateSleep,
  blthStateTestMode,
} BluetoothState;

typedef struct
{
  int8_t  rssi;
  int8_t  txPower;
  char    *name;
} BluetoothDevice;

typedef struct
{
  uint8_t         number;
  BluetoothDevice device[10];
} BluetoothDevices;

Bluetooth* blth_create(UART_HandleTypeDef *uart, GPIO_TypeDef *resetPort, uint16_t resetPin);
void blth_destroy(Bluetooth *bluetooth);
void blth_dma_isr(Bluetooth *bluetooth);
BluetoothState blth_state(Bluetooth const *bluetooth);
Bool blth_connect_state(Bluetooth const *bluetooth, uint8_t *btmac);
void blth_disconnect(Bluetooth *bluetooth);
char const* blth_passkey(Bluetooth const* bluetooth);
BluetoothDevices const* blth_get_devices(Bluetooth *bluetooth);
void blth_scanning_start(Bluetooth *bluetooth);
void blth_scanning_stop(Bluetooth *bluetooth);
void blth_send(Bluetooth *bluetooth, uint8_t const *data, size_t size);

#endif /* INC_BLUETOOTH_H_ */
