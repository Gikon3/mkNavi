#include <string.h>
#include "util_types.h"
#include "flow_ctrl.h"
#include "station.h"
#include "bluetooth_impl.h"
#include "bluetooth.h"
#include "bluetooth_callbacks.h"
#include "proteus3_commands.h"

#define CFG_SECURITY  0x09
#define CFG_SCANNING  0x00
#define CFG_BEACON    0x02
#define CFG_MODULE    0x00

typedef struct
{
  Bluetooth *bluetooth;
  FlowCtrl  *flowCtrl;
  Station   *station;
} CallbackArgument;

static void request_params(Bluetooth *bluetooth)
{
  proteus3_cmd_get(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth), proteus3UserSettingRF_StaticPasskey);
}

static void step_by_step_config(Bluetooth *bluetooth, void const *argument)
{
  switch(bluetooth->config) {
    case blthConfigIdle:
      bluetooth->config = blthConfigSecurityGet;
      proteus3_cmd_get(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth), proteus3UserSettingRF_SecFlags);
      break;

    case blthConfigSecurityGet:
      if(*(uint8_t const*)argument == CFG_SECURITY) {
        bluetooth->config = blthConfigScanningGet;
        proteus3_cmd_get(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth), proteus3UserSettingRF_ScanFlags);
      }
      else {
        bluetooth->config = blthConfigSecuritySet;
        const uint8_t configParameter = CFG_SECURITY;
        proteus3_cmd_set(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth),
            proteus3UserSettingRF_SecFlags, &configParameter, sizeof(configParameter));
      }
      break;
    case blthConfigSecuritySet:
      if(*(uint8_t const*)argument == 0x00) {
        bluetooth->config = blthConfigScanningGet;
        proteus3_cmd_get(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth), proteus3UserSettingRF_ScanFlags);
      }
      break;

    case blthConfigScanningGet:
      if(*(uint8_t const*)argument == CFG_SCANNING) {
        bluetooth->config = blthConfigBeaconGet;
        proteus3_cmd_get(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth), proteus3UserSettingRF_BeaconFlags);
      }
      else {
        bluetooth->config = blthConfigScanningSet;
        const uint8_t configParameter = CFG_SCANNING;
        proteus3_cmd_set(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth),
            proteus3UserSettingRF_ScanFlags, &configParameter, sizeof(configParameter));
      }
      break;
    case blthConfigScanningSet:
      if(*(uint8_t const*)argument == 0x00) {
        bluetooth->config = blthConfigBeaconGet;
        proteus3_cmd_get(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth), proteus3UserSettingRF_BeaconFlags);
      }
      break;

    case blthConfigBeaconGet:
      if(*(uint8_t const*)argument == CFG_BEACON) {
        bluetooth->config = blthConfigModuleGet;
        proteus3_cmd_get(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth), proteus3UserSettingCFG_Flags);
      }
      else {
        bluetooth->config = blthConfigBeaconSet;
        const uint8_t configParameter = CFG_BEACON;
        proteus3_cmd_set(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth),
            proteus3UserSettingRF_BeaconFlags, &configParameter, sizeof(configParameter));
      }
      break;
    case blthConfigBeaconSet:
      if(*(uint8_t const*)argument == 0x00) {
        bluetooth->config = blthConfigModuleGet;
        proteus3_cmd_get(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth), proteus3UserSettingCFG_Flags);
      }
      break;

    case blthConfigModuleGet:
      if(*(uint8_t const*)argument == CFG_MODULE) {
        bluetooth->config = blthConfigOK;
      }
      else {
        bluetooth->config = blthConfigModuleSet;
        const uint8_t configParameter[] = { CFG_MODULE, 0x00 };
        proteus3_cmd_set(PROTEUS_POINT(bluetooth), DATA_QUEUE(bluetooth),
            proteus3UserSettingCFG_Flags, &configParameter, sizeof(configParameter));
      }
      break;
    case blthConfigModuleSet:
      if(*(uint8_t const*)argument == 0x00) {
        bluetooth->config = blthConfigOK;
      }
      break;

    case blthConfigOK:
    case blthConfigRequestParams:
      break;
  }

  // После успешной конфигурации запросить параметры из модуля
  if(bluetooth->config == blthConfigOK) {
    request_params(bluetooth);
    bluetooth->config = blthConfigRequestParams;
  }
}

static void start_callback(void *argument)
{
  CallbackArgument *pack = argument;
  step_by_step_config(pack->bluetooth, NULL);
}

static void set_callback(Proteus3UserSetting userSetting, uint8_t const *payload, uint16_t size, void *argument)
{
  CallbackArgument *pack = argument;
  step_by_step_config(pack->bluetooth, payload);
  switch(userSetting) {
    case proteus3UserSettingFS_FW_Version:
    case proteus3UserSettingRF_DeviceName:
    case proteus3UserSettingFS_MAC:
    case proteus3UserSettingFS_BTMAC:
    case proteus3UserSettingRF_AdvertisingTimeout:
    case proteus3UserSettingRF_ConnectionTiming:
    case proteus3UserSettingRF_ScanTiming:
    case proteus3UserSettingRF_ScanFactor:
    case proteus3UserSettingUART_ConfigIndex:
    case proteus3UserSettingRF_SecFlags:
    case proteus3UserSettingRF_ScanFlags:
    case proteus3UserSettingRF_BeaconFlags:
    case proteus3UserSettingFS_DeviceInfo:
    case proteus3UserSettingFS_SerialNumber:
    case proteus3UserSettingRF_TX_Power:
    case proteus3UserSettingRF_StaticPasskey:
    case proteus3UserSettingDIS_Flags:
    case proteus3UserSettingDIS_ManufacturerName:
    case proteus3UserSettingDIS_ModelNumber:
    case proteus3UserSettingDIS_SerialNumber:
    case proteus3UserSettingDIS_HW_Version:
    case proteus3UserSettingDIS_SW_Version:
    case proteus3UserSettingRF_Appearance:
    case proteus3UserSettingRF_SPP_BaseUUID:
    case proteus3UserSettingCFG_Flags:
    case proteus3UserSettingRF_AdvertisingFlags:
    case proteus3UserSettingRF_SPP_ServiceUUID:
    case proteus3UserSettingRF_SPP_RX_UUID:
    case proteus3UserSettingRF_SPP_TX_UUID:
    case proteus3UserSettingRF_SecFlagsPerOnly:
      break;
  }
}

static void get_callback(Proteus3UserSetting userSetting, uint8_t const *payload, uint16_t size, void *argument)
{
  CallbackArgument *pack = argument;
  step_by_step_config(pack->bluetooth, payload + 1);
  switch(userSetting) {
    case proteus3UserSettingFS_FW_Version:
    case proteus3UserSettingRF_DeviceName:
    case proteus3UserSettingFS_MAC:
    case proteus3UserSettingFS_BTMAC:
    case proteus3UserSettingRF_AdvertisingTimeout:
    case proteus3UserSettingRF_ConnectionTiming:
    case proteus3UserSettingRF_ScanTiming:
    case proteus3UserSettingRF_ScanFactor:
    case proteus3UserSettingUART_ConfigIndex:
    case proteus3UserSettingRF_SecFlags:
    case proteus3UserSettingRF_ScanFlags:
    case proteus3UserSettingRF_BeaconFlags:
    case proteus3UserSettingFS_DeviceInfo:
    case proteus3UserSettingFS_SerialNumber:
    case proteus3UserSettingRF_TX_Power:
      break;
    case proteus3UserSettingRF_StaticPasskey:
      memcpy(pack->bluetooth->params.passkey, payload + 1, sizeof(pack->bluetooth->params.passkey));
      break;
    case proteus3UserSettingDIS_Flags:
    case proteus3UserSettingDIS_ManufacturerName:
    case proteus3UserSettingDIS_ModelNumber:
    case proteus3UserSettingDIS_SerialNumber:
    case proteus3UserSettingDIS_HW_Version:
    case proteus3UserSettingDIS_SW_Version:
    case proteus3UserSettingRF_Appearance:
    case proteus3UserSettingRF_SPP_BaseUUID:
    case proteus3UserSettingCFG_Flags:
    case proteus3UserSettingRF_AdvertisingFlags:
    case proteus3UserSettingRF_SPP_ServiceUUID:
    case proteus3UserSettingRF_SPP_RX_UUID:
    case proteus3UserSettingRF_SPP_TX_UUID:
    case proteus3UserSettingRF_SecFlagsPerOnly:
      break;
  }
}

static void receive_callback(uint8_t const *btMac, int8_t rssi, uint8_t const *data,
    uint16_t dataLength, void *argument)
{
  CallbackArgument *pack = argument;
  flow_bluetooth_append_data(pack->flowCtrl, data, dataLength);
  flow_bluetooth_process(pack->flowCtrl, pack->station);
}

static void connect_callback(Bool success, uint8_t const *btMac, void *argument)
{
  CallbackArgument *pack = argument;
  pack->flowCtrl->ripe.bluetooth.en = bTrue;
}

static void disconnect_callback(Proteus3DisconnectReason reason, void *argument)
{
  CallbackArgument *pack = argument;
  pack->flowCtrl->ripe.bluetooth.en = bFalse;
}

static void channel_open_callback(uint8_t const *btMac, uint16_t maxPayload, void *argument)
{
  CallbackArgument *pack = argument;
  proteus3_cmd_phy_update(PROTEUS_POINT(pack->bluetooth), DATA_QUEUE(pack->bluetooth), proteus3Phy2MiB);
}

static void rssi_callback(uint8_t const *btMac, int8_t rssi, int8_t txPower, void *argument)
{
}

static void beacon_callback(uint8_t const *btMac, int8_t rssi, uint8_t const *rawData, uint16_t rawDataSize, void *argument)
{
}

Proteus3Callbacks const* blth_callbacks(Bluetooth *bluetooth)
{
  static Bool initFl = bFalse;
  static CallbackArgument callbackArgument = {
      .bluetooth = NULL,
      .flowCtrl = NULL,
      .station = NULL,
  };
  static Proteus3Callbacks callbacks = {
      .start = start_callback,
      .set = set_callback,
      .get = get_callback,
      .receive = receive_callback,
      .connect = connect_callback,
      .disconnect = disconnect_callback,
      .security = NULL,
      .rssi = rssi_callback,
      .passkey = NULL,
      .phy_update = NULL,
      .error = NULL,
      .display_passkey = NULL,
      .sleep = NULL,
      .channel_open = channel_open_callback,
      .beacon = beacon_callback,
      .argument = NULL,
  };
  if(initFl == bFalse) {
    callbackArgument.bluetooth = bluetooth;
    callbackArgument.flowCtrl = &flowCtrl;
    callbackArgument.station = &station;
    callbacks.argument = &callbackArgument;
    initFl = bTrue;
  }
  return &callbacks;
}
