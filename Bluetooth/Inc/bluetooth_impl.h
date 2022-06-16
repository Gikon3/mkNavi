#ifndef INC_BLUETOOTH_IMPL_H_
#define INC_BLUETOOTH_IMPL_H_

#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"
#include "proteus3.h"
#include "bluetooth.h"

#define PROTEUS_POINT(x)  (x->proteus3)
#define DATA_QUEUE(x)     (x->software.sendQueue)

typedef enum
{
  blthConfigIdle,
  blthConfigSecurityGet,
  blthConfigSecuritySet,
  blthConfigScanningGet,
  blthConfigScanningSet,
  blthConfigBeaconGet,
  blthConfigBeaconSet,
  blthConfigModuleGet,
  blthConfigModuleSet,
  blthConfigOK,
  blthConfigRequestParams,
} BluetoothConfig;

struct _Bluetooth
{
  struct
  {
    UART_HandleTypeDef  *uart;
    struct
    {
      GPIO_TypeDef      *port;
      uint16_t          pin;
    } reset;
  } hardware;
  struct
  {
    SemaphoreHandle_t   dmaMutex;
    QueueHandle_t       sendQueue;
    TaskHandle_t        receiveTask;
    TaskHandle_t        sendTask;
  } software;
  Proteus3              *proteus3;
  BluetoothConfig       config;
  struct
  {
    char                passkey[6];
    BluetoothDevices    devices;
  } params;
};

#endif /* INC_BLUETOOTH_IMPL_H_ */
