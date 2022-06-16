#include <string.h>
#include "util_heap.h"
#include "util_vector.h"
#include "bluetooth_impl.h"
#include "bluetooth_callbacks.h"

static uint8_t uartBuffer[6144];

static HAL_StatusTypeDef uart_send(Bluetooth *bluetooth, Vector const *data)
{
  const size_t sendSize = vect_size(data) * vect_item_size(data);
  const uint32_t timeout = sendSize > 2 ? sendSize : 2;
  vPortEnterCritical();
  const HAL_StatusTypeDef status = HAL_UART_Transmit(bluetooth->hardware.uart, vect_data(data), sendSize, timeout);
  vPortExitCritical();
  return status;
}

static HAL_StatusTypeDef uart_dma_send(Bluetooth *bluetooth, Vector const *data)
{
  const size_t sendSize = vect_size(data) * vect_item_size(data);
  xSemaphoreTake(bluetooth->software.dmaMutex, portMAX_DELAY);
  const HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(bluetooth->hardware.uart, vect_data(data), sendSize);
  return status;
}

static void start_receive(void *argument)
{
  Bluetooth *bluetooth = argument;
  TickType_t wakeTime = xTaskGetTickCount();
  const TickType_t period = pdMS_TO_TICKS(20);
  while(bTrue) {
    proteus3_pwrite_set(bluetooth->proteus3, sizeof(uartBuffer) - bluetooth->hardware.uart->hdmarx->Instance->NDTR);
    proteus3_message_parser(bluetooth->proteus3);
    vTaskDelayUntil(&wakeTime, period);
  }
}

static void start_send(void *argument)
{
  Bluetooth *bluetooth = argument;
  HAL_StatusTypeDef (*send)(Bluetooth*, Vector const*) = bluetooth->software.dmaMutex ? uart_dma_send : uart_send;
  while(proteus3_ready(bluetooth->proteus3) == bFalse)
    vTaskDelay(pdMS_TO_TICKS(5));
  while(bTrue) {
    Vector message;
    xQueueReceive(bluetooth->software.sendQueue, &message, portMAX_DELAY);
    send(bluetooth, &message);
    vect_clear(&message);
  }
}

static void initialize(Bluetooth *bluetooth, UART_HandleTypeDef *uart, GPIO_TypeDef *resetPort, uint16_t resetPin)
{
  bluetooth->hardware.uart = uart;
  bluetooth->hardware.reset.port = resetPort;
  bluetooth->hardware.reset.pin = resetPin;
  bluetooth->software.sendQueue = xQueueCreate(4, sizeof(Vector));
  if(uart->hdmatx) {
    bluetooth->software.dmaMutex = xSemaphoreCreateBinary();
    xSemaphoreGive(bluetooth->software.dmaMutex);
  }
  else {
    bluetooth->software.dmaMutex = NULL;
  }

  bluetooth->proteus3 = proteus3_create(uartBuffer, sizeof(uartBuffer), blth_callbacks(bluetooth));
  bluetooth->config = blthConfigIdle;
  memset(bluetooth->params.passkey, '\0', sizeof(bluetooth->params.passkey));
  bluetooth->params.devices.number = 0;
  for(uint8_t i = 0; i < (sizeof(bluetooth->params.devices.device) / sizeof(bluetooth->params.devices.device[0])); ++i) {
    BluetoothDevice *device = &bluetooth->params.devices.device[i];
    device->rssi = 0;
    device->txPower = 0;
    device->name = NULL;
  }

  xTaskCreate(start_receive, "bluReceive", 512, bluetooth, osPriorityNormal, &bluetooth->software.receiveTask);
  xTaskCreate(start_send, "bluSend", 256, bluetooth, osPriorityNormal, &bluetooth->software.sendTask);

  HAL_UART_Receive_DMA(bluetooth->hardware.uart, uartBuffer, sizeof(uartBuffer));
  HAL_GPIO_WritePin(resetPort, resetPin, SET);
}

static void deinitialize(Bluetooth *bluetooth)
{
  HAL_GPIO_WritePin(bluetooth->hardware.reset.port, bluetooth->hardware.reset.pin, RESET);
  HAL_UART_DMAStop(bluetooth->hardware.uart);

  vTaskDelete(bluetooth->software.sendTask);
  vTaskDelete(bluetooth->software.receiveTask);

  for(uint8_t i = 0; i < bluetooth->params.devices.number; ++i) {
    BluetoothDevice *device = &bluetooth->params.devices.device[i];
    util_free(device->name);
    device->name = NULL;
  }
  bluetooth->params.devices.number = 0;
  bluetooth->config = blthConfigIdle;

  proteus3_destroy(bluetooth->proteus3);

  if(bluetooth->software.dmaMutex)
    vSemaphoreDelete(bluetooth->software.dmaMutex);
  Vector message;
  while(xQueueReceive(bluetooth->software.sendQueue, &message, 0) == pdTRUE)
    vect_clear(&message);
  vQueueDelete(bluetooth->software.sendQueue);
}

Bluetooth* blth_create(UART_HandleTypeDef *uart, GPIO_TypeDef *resetPort, uint16_t resetPin)
{
  Bluetooth *bluetooth = util_malloc(sizeof(Bluetooth));
  initialize(bluetooth, uart, resetPort, resetPin);
  return bluetooth;
}

void blth_destroy(Bluetooth *bluetooth)
{
  deinitialize(bluetooth);
  util_free(bluetooth);
}

void blth_dma_isr(Bluetooth *bluetooth)
{
  xSemaphoreGiveFromISR(bluetooth->software.dmaMutex, NULL);
}

BluetoothState blth_state(Bluetooth const *bluetooth)
{
  switch(proteus3_state(PROTEUS_POINT(bluetooth))) {
    case proteus3StateNoAction:
      return blthStateNoAction;
    case proteus3StateIdle:
      return blthStateIdle;
    case proteus3StateScanning:
      return blthStateScanning;
    case proteus3StateConnected:
      return blthStateConnected;
    case proteus3StateSleep:
      return blthStateSleep;
    case proteus3StateDirectTestMode:
      return blthStateTestMode;
    default:
      return blthStateNoAction;
  }
}

Bool blth_connect_state(Bluetooth const *bluetooth, uint8_t *btmac)
{
  return proteus3_connect_state(bluetooth->proteus3, btmac);
}

void blth_disconnect(Bluetooth *bluetooth)
{
  proteus3_cmd_disconnect(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth));
}

char const* blth_passkey(Bluetooth const* bluetooth)
{
  return bluetooth->params.passkey;
}

static void devices_fill(BluetoothDevices *blthDevices, Proteus3Devices const *devices)
{
  for(uint8_t i = 0; i < blthDevices->number; ++i) {
    util_free(blthDevices->device[i].name);
    blthDevices->device[i].name = NULL;
  }
  const uint8_t number = devices->number <= (sizeof(blthDevices->device) / sizeof(blthDevices->device[0])) ?
      devices->number : sizeof(blthDevices->device) / sizeof(blthDevices->device[0]);
  for(uint8_t i = 0; i < number; ++i) {
    blthDevices->device[i].rssi = devices->device[i].rssi;
    blthDevices->device[i].txPower = devices->device[i].txPower;
    blthDevices->device[i].name = util_malloc(strlen(devices->device[i].name));
    strcpy(blthDevices->device[i].name, devices->device[i].name);
  }
  blthDevices->number = number;
}

BluetoothDevices const* blth_get_devices(Bluetooth *bluetooth)
{
  devices_fill(&bluetooth->params.devices, proteus3_devices(PROTEUS_POINT(bluetooth)));
  proteus3_cmd_get_devices(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth));
  return &bluetooth->params.devices;
}

void blth_scanning_start(Bluetooth *bluetooth)
{
  proteus3_cmd_scan_start(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth));
}

void blth_scanning_stop(Bluetooth *bluetooth)
{
  proteus3_cmd_scan_stop(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth));
}

void blth_send(Bluetooth *bluetooth, uint8_t const *data, size_t size)
{
  proteus3_cmd_data(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth), data, size);
}
