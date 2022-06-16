#ifndef INC_PROTEUS3_TYPES_H_
#define INC_PROTEUS3_TYPES_H_

typedef enum
{
  proteus3StateNoAction = 0x00,
  proteus3StateIdle = 0x01,
  proteus3StateScanning = 0x02,
  proteus3StateConnected = 0x03,
  proteus3StateSleep = 0x04,
  proteus3StateDirectTestMode = 0x05,
} Proteus3State;

typedef enum
{
  proteus3UserSettingFS_FW_Version = 1,
  proteus3UserSettingRF_DeviceName = 2,
  proteus3UserSettingFS_MAC = 3,
  proteus3UserSettingFS_BTMAC = 4,
  proteus3UserSettingRF_AdvertisingTimeout = 7,
  proteus3UserSettingRF_ConnectionTiming = 8,
  proteus3UserSettingRF_ScanTiming = 9,
  proteus3UserSettingRF_ScanFactor = 10,
  proteus3UserSettingUART_ConfigIndex = 11,
  proteus3UserSettingRF_SecFlags = 12,
  proteus3UserSettingRF_ScanFlags = 13,
  proteus3UserSettingRF_BeaconFlags = 14,
  proteus3UserSettingFS_DeviceInfo = 15,
  proteus3UserSettingFS_SerialNumber = 16,
  proteus3UserSettingRF_TX_Power = 17,
  proteus3UserSettingRF_StaticPasskey = 18,
  proteus3UserSettingDIS_Flags = 19,
  proteus3UserSettingDIS_ManufacturerName = 20,
  proteus3UserSettingDIS_ModelNumber = 21,
  proteus3UserSettingDIS_SerialNumber = 22,
  proteus3UserSettingDIS_HW_Version = 23,
  proteus3UserSettingDIS_SW_Version = 24,
  proteus3UserSettingRF_Appearance = 25,
  proteus3UserSettingRF_SPP_BaseUUID = 26,
  proteus3UserSettingCFG_Flags = 28,
  proteus3UserSettingRF_AdvertisingFlags = 29,
  proteus3UserSettingRF_SPP_ServiceUUID = 32,
  proteus3UserSettingRF_SPP_RX_UUID = 33,
  proteus3UserSettingRF_SPP_TX_UUID = 34,
  proteus3UserSettingRF_SecFlagsPerOnly = 44,
} Proteus3UserSetting;

typedef enum
{
  proteus3DisconnectReasonUnknown,
  proteus3DisconnectReasonConnectionTimeout,
  proteus3DisconnectReasonUserTerminatedConnection,
  proteus3DisconnectReasonHostTerminatedConnection,
  proteus3DisconnectReasonConnectionIntervalUnacceptable,
  proteus3DisconnectReasonMicFailure,
  proteus3DisconnectReasonConnectionSetupFailed,
} Proteus3DisconnectReason;

typedef enum
{
  proteus3SecurityStateBLE_ReBonded = 0,
  proteus3SecurityStateBLE_Bonded = 1,
  proteus3SecurityStateBLE_Paired = 2,
} Proteus3SecurityState;

typedef enum
{
  proteus3Phy1MiB,
  proteus3Phy2MiB,
  proteus3Phy125KiB,
} Proteus3Phy;

typedef enum
{
  proteus3DisplayPasskeyActionNoAction,
  proteus3DisplayPasskeyActionPleaseConfirm
} Proteus3DisplayPasskeyAction;

typedef enum
{
  proteus3GPIO_Disconnect = 0x00,
  proteus3GPIO_Input = 0x01,
  proteus3GPIO_Output = 0x02,
  proteus3GPIO_PWM = 0x03,
} Proteus3GPIO_Function;

typedef enum
{
  proteus3GPIO_NoPull = 0x00,
  proteus3GPIO_PullDown = 0x01,
  proteus3GPIO_PullUp = 0x02,
} Proteus3GPIO_FunctionInput;

typedef enum
{
  proteus3GPIO_OutputLow = 0x00,
  proteus3GPIO_OutputHigh = 0x01,
} Proteus3GPIO_FunctionOutput;

typedef struct
{
  uint16_t  period;
  uint8_t   ratio;
} Proteus3GPIO_FunctionPWM;

typedef struct
{
  uint8_t                       id;
  Proteus3GPIO_Function         function;
  union
  {
    Proteus3GPIO_FunctionInput  input;
    Proteus3GPIO_FunctionOutput output;
    Proteus3GPIO_FunctionPWM    pwm;
  };
} Proteus3GPIO_BlockConfig;

typedef struct
{
  uint8_t id;
  uint8_t value;
} Proteus3GPIO_Block;

typedef struct
{
  uint8_t number;
  struct
  {
    uint8_t btmac[6];
    int8_t  rssi;
    int8_t  txPower;
    char    *name;
  } device[10];
} Proteus3Devices;

typedef struct
{
  void (*start)(void *argument);
  void (*set)(Proteus3UserSetting userSetting, uint8_t const *payload, uint16_t size, void *argument);
  void (*get)(Proteus3UserSetting userSetting, uint8_t const *payload, uint16_t size, void *argument);
  void (*receive)(uint8_t const *btMac, int8_t rssi, uint8_t const *data, uint16_t dataLength, void *argument);
  void (*connect)(Bool success, uint8_t const *btMac, void *argument);
  void (*disconnect)(Proteus3DisconnectReason reason, void *argument);
  void (*security)(Proteus3SecurityState securityState, uint8_t const *btMac, void *argument);
  void (*rssi)(uint8_t const *btMac, int8_t rssi, int8_t txPower, void *argument);
  void (*passkey)(uint8_t const *btMac, void *argument);
  void (*phy_update)(Bool success, Proteus3Phy phyRx, Proteus3Phy phyTx, uint8_t const *btMac, void *argument);
  void (*error)(uint8_t errorCode, void *argument);
  void (*display_passkey)(Proteus3DisplayPasskeyAction action, uint8_t const *btMac, uint8_t const *passkey,
      void *argument);
  void (*sleep)(void *argument);
  void (*channel_open)(uint8_t const *btMac, uint16_t maxPayload, void *argument);
  void (*beacon)(uint8_t const *btMac, int8_t rssi, uint8_t const *rawData, uint16_t rawDataSize, void *argument);
  void *argument;
} Proteus3Callbacks;

struct _Proteus3;
typedef struct _Proteus3 Proteus3;

#endif /* INC_PROTEUS3_TYPES_H_ */
