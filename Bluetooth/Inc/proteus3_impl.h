#ifndef INC_PROTEUS3_IMPL_H_
#define INC_PROTEUS3_IMPL_H_

#include "stm32f4xx.h"
#include "util_types.h"
#include "util_list.h"
#include "proteus3_types.h"

#define START_SIGNAL        0x02
#define COMMAND_TYPE_MASK   (3 << 6)

#define BEACON_TEXT_MAX 19

typedef enum
{
  cmdReset = 0x00,
  cmdGetState = 0x01,
  cmdSleep = 0x02,
  cmdData = 0x04,
  cmdConnect = 0x06,
  cmdDisconnect = 0x07,
  cmdScanStart = 0x09,
  cmdScanStop = 0x0A,
  cmdGetDevices = 0x0B,
  cmdSetBeacon = 0x0C,
  cmdPasskey = 0x0D,
  cmdDeleteBonds = 0x0E,
  cmdGetBonds = 0x0F,
  cmdGet = 0x10,
  cmdSet = 0x11,
  cmdPhyUpdate = 0x1A,
  cmdUartDisable = 0x1B,
  cmdFactoryReset = 0x1C,
  cmdDTM_Start = 0x1D,
  cmdDTM = 0x1E,
  cmdBootloader = 0x1F,
  cmdNumericComp = 0x24,
  cmdGPIO_LocalWriteConfig = 0x25,
  cmdGPIO_LocalWrite = 0x26,
  cmdGPIO_LocalRead = 0x27,
  cmdGPIO_RemoteWriteConfig = 0x28,
  cmdGPIO_RemoteWrite = 0x29,
  cmdGPIO_RemoteRead = 0x2A,
  cmdGPIO_LocalReadConfig = 0x2B,
  cmdGPIO_RemoteReadConfig = 0x2C,
  cmdAllowUnboundedConnections = 0x2D,
  cnfReset = 0x40,
  cnfGetState = 0x41,
  cnfSleep = 0x42,
  cnfData = 0x44,
  cnfConnect = 0x46,
  cnfDisconnect = 0x47,
  cnfScanStart = 0x49,
  cnfScanStop = 0x4A,
  cnfGetDevices = 0x4B,
  cnfSetBeacon = 0x4C,
  cnfPasskey = 0x4D,
  cnfDeleteBonds = 0x4E,
  cnfGetBonds = 0x4F,
  cnfGet = 0x50,
  cnfSet = 0x51,
  cnfPhyUpdate = 0x5A,
  cnfUartDisable = 0x5B,
  cnfFactoryReset = 0x5C,
  cnfDTM_Start = 0x5D,
  cnfDTM = 0x5E,
  cnfBootloader = 0x5F,
  cnfNumericComp = 0x64,
  cnfGPIO_LocalWriteConfig = 0x65,
  cnfGPIO_LocalWrite = 0x66,
  cnfGPIO_LocalRead = 0x67,
  cnfGPIO_RemoteWriteConfig = 0x68,
  cnfGPIO_RemoteWrite = 0x69,
  cnfGPIO_RemoteRead = 0x6A,
  cnfGPIO_LocalReadConfig = 0x6B,
  cnfGPIO_RemoteReadConfig = 0x6C,
  cnfGPIO_AllowUnboundedConnections = 0x6D,
  indSleep = 0x82,
  indData = 0x84,
  indConnect = 0x86,
  indDisconnect = 0x87,
  indSecurity = 0x88,
  indRSSI = 0x8B,
  indBeacon = 0x8C,
  indPasskey = 0x8D,
  indPhyUpdate = 0x9A,
  indUartEnable = 0x9B,
  indError = 0xA2,
  indDisplayPasskey = 0xA4,
  indGPIO_LocalWrite = 0xA6,
  indGPIO_RemoteWriteConfig = 0xA8,
  indGPIO_RemoteWrite = 0xA9,
  rspTxComplete = 0xC4,
  rspChannelOpen = 0xC6,
  rspBeacon = 0xCC,
} Command;

typedef enum
{
  cmdTypeReq = 0,
  cmdTypeCnf = 1 << 6,
  cmdTypeInd = 2 << 6,
  cmdTypeRsp = 3 << 6,
} CommandType;

typedef enum
{
  findStartSignal,
  findCommand,
  findLength,
  findPayload,
  findCRC,
} FindState;

typedef struct
{
  uint8_t const *buffer;
  size_t        bufferSize;
  uint8_t const *pWrite;
  uint8_t const *pRead;
} FindItem;

typedef struct
{
  FindState     state;
  uint8_t       crc;
  Command       command;
  uint16_t      payloadLength;
  uint8_t const *pPayload;
  uint8_t const *pStartMessage;
  FindItem      item;
} Parser;

typedef struct
{
  Bool    state;
  uint8_t btmac[6];
} Connect;

typedef struct
{
  Bool    open;
  uint8_t btmac[6];
  uint8_t maxPayload;
} Channel;

struct _Proteus3
{
  Bool              deviceReady;
  Proteus3State     state;
  Parser            parser;
  List              commandSetPull;
  List              commandGetPull;
  Proteus3Callbacks callback;
  Connect           connect;
  Channel           channel;
  Proteus3Devices   devices;
};

#endif /* INC_PROTEUS3_IMPL_H_ */
