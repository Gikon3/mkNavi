#include <string.h>
#include "util_heap.h"
#include "util_search.h"
#include "util_list.h"
#include "proteus3_impl.h"
#include "proteus3.h"

static void initialize(Proteus3 *proteus3, uint8_t const *buffer, size_t size,
    Proteus3Callbacks const *callbacks)
{
  proteus3->deviceReady = bFalse;
  proteus3->state = proteus3StateNoAction;
  proteus3->parser.state = findStartSignal;
  proteus3->commandSetPull = make_list(sizeof(Proteus3UserSetting));
  proteus3->commandGetPull = make_list(sizeof(Proteus3UserSetting));
  proteus3->parser.item.buffer = buffer;
  proteus3->parser.item.bufferSize = size;
  proteus3->parser.item.pWrite = buffer;
  proteus3->parser.item.pRead = buffer;
  memcpy(&proteus3->callback, callbacks, sizeof(proteus3->callback));
}

static void deinitialize(Proteus3 *proteus3)
{
  proteus3->parser.item.pWrite = proteus3->parser.item.buffer;
  proteus3->parser.item.pRead = proteus3->parser.item.buffer;
  list_clear(&proteus3->commandGetPull);
  list_clear(&proteus3->commandSetPull);
  proteus3->parser.state = findStartSignal;
  proteus3->deviceReady = bFalse;
}

Proteus3* proteus3_create(uint8_t const *buffer, size_t size, Proteus3Callbacks const *callbacks)
{
  Proteus3 *proteus3 = util_malloc(sizeof(Proteus3));
  initialize(proteus3, buffer, size, callbacks);
  return proteus3;
}

void proteus3_destroy(Proteus3 *proteus3)
{
  deinitialize(proteus3);
  util_free(proteus3);
}

static void ring_buffer_copy(Proteus3 *proteus3, size_t index, size_t size, void *destination)
{
  uint8_t const *endBuffer = proteus3->parser.item.buffer + proteus3->parser.item.bufferSize;
  uint8_t const *begin = proteus3->parser.pPayload + index;
  begin = begin < endBuffer ? begin : proteus3->parser.item.buffer + (begin - endBuffer);
  util_ring_buffer_copy(proteus3->parser.item.buffer, endBuffer, begin, size, destination);
}

static void devices_clear(Proteus3Devices *devices)
{
  for(uint8_t i = 0; i < devices->number; ++i)
    util_free(devices->device[i].name);
  devices->number = 0;
}

static void devices_fill(Proteus3 *proteus3, Proteus3Devices *devices, uint8_t const *data, uint8_t numberDevices)
{
  const uint8_t maxNumber = sizeof(devices->device) / sizeof(devices->device[0]);
  const uint8_t number = numberDevices > maxNumber ? maxNumber : numberDevices;
  uint8_t index = 2;
  for(uint8_t i = 0; i < number; ++i) {
    uint8_t header[9];
    ring_buffer_copy(proteus3, index, sizeof(header), header);
    index += 9;
    memcpy(devices->device[i].btmac, header, sizeof(devices->device[0].btmac));
    devices->device[i].rssi = header[6];
    devices->device[i].txPower = header[7];
    if(header[8] > 0) {
      devices->device[i].name = util_malloc(header[8]);
      ring_buffer_copy(proteus3, index, header[8], devices->device[i].name);
      index += header[8];
    }
    else {
      devices->device[i].name = util_malloc(1);
      *devices->device[i].name = '\0';
    }
  }
  devices->number = number;
}

static void confirmations_process(Proteus3 *proteus3)
{
  switch(proteus3->parser.command) {
    case cnfReset:
      break;
    case cnfGetState:
      if(proteus3->deviceReady == bFalse)
        proteus3->callback.start(proteus3->callback.argument);
      proteus3->deviceReady = bTrue;
      proteus3->state = *(proteus3->parser.pPayload + 1);
      break;
    case cnfSleep:
      if(*proteus3->parser.pPayload == 0x00)
        proteus3->state = proteus3StateSleep;
      break;
    case cnfData:
    case cnfConnect:
    case cnfDisconnect:
      break;
    case cnfScanStart:
      if(*proteus3->parser.pPayload == 0x00)
        proteus3->state = proteus3StateScanning;
      break;
    case cnfScanStop:
      if(*proteus3->parser.pPayload == 0x00)
        proteus3->state = proteus3StateIdle;
      break;
    case cnfGetDevices:
      if(*proteus3->parser.pPayload == 0x00) {
        devices_clear(&proteus3->devices);
        devices_fill(proteus3, &proteus3->devices, proteus3->parser.pPayload + 2, *(proteus3->parser.pPayload + 1));
      }
      break;
    case cnfSetBeacon:
    case cnfPasskey:
    case cnfDeleteBonds:
    case cnfGetBonds:
      break;
    case cnfGet: {
      Proteus3UserSetting userSetting = proteus3UserSettingFS_FW_Version;
      list_pop_front(&proteus3->commandGetPull, &userSetting);
      if(proteus3->callback.get) {
        const uint16_t size = proteus3->parser.payloadLength;
        uint8_t *payload = util_malloc(size);
        ring_buffer_copy(proteus3, 0, size, payload);
        proteus3->callback.get(userSetting, payload, size, proteus3->callback.argument);
        util_free(payload);
      }
      break;
    }
    case cnfSet:
    case cnfPhyUpdate:
    case cnfUartDisable:
    case cnfFactoryReset:
    case cnfDTM_Start:
    case cnfDTM:
    case cnfBootloader:
    case cnfNumericComp:
    case cnfGPIO_LocalWriteConfig:
    case cnfGPIO_LocalWrite:
    case cnfGPIO_LocalRead:
    case cnfGPIO_RemoteWriteConfig:
    case cnfGPIO_RemoteWrite:
    case cnfGPIO_RemoteRead:
    case cnfGPIO_LocalReadConfig:
    case cnfGPIO_RemoteReadConfig:
    default: ;
  }
}

static void indications_process(Proteus3 *proteus3)
{
  switch(proteus3->parser.command) {
    case indSleep:
      break;
    case indData:
      if(proteus3->callback.receive) {
        uint8_t btmac[6];
        ring_buffer_copy(proteus3, 0, sizeof(btmac), btmac);
        int8_t rssi;
        ring_buffer_copy(proteus3, 6, sizeof(rssi), &rssi);
        const uint16_t dataLength = proteus3->parser.payloadLength - 7;
        uint8_t *data = util_malloc(dataLength);
        ring_buffer_copy(proteus3, 7, dataLength, data);
        proteus3->callback.receive(btmac, rssi, data, dataLength, proteus3->callback.argument);
        util_free(data);
      }
      break;
    case indConnect:
      proteus3->connect.state = *proteus3->parser.pPayload == 0 ? bTrue : bFalse;
      proteus3->state = proteus3->connect.state == bTrue ? proteus3StateConnected : proteus3->state;
      ring_buffer_copy(proteus3, 1, sizeof(proteus3->connect.btmac), proteus3->connect.btmac);
      if(proteus3->callback.connect)
        proteus3->callback.connect(proteus3->connect.state, proteus3->connect.btmac, proteus3->callback.argument);
      break;
    case indDisconnect:
      proteus3->connect.state = bFalse;
      proteus3->state = proteus3StateIdle;
      if(proteus3->callback.disconnect) {
        uint8_t payload = *proteus3->parser.pPayload;
        Proteus3DisconnectReason reason = proteus3DisconnectReasonUnknown;
        if(payload == 0x08)
          reason = proteus3DisconnectReasonConnectionTimeout;
        else if(payload == 0x13)
          reason = proteus3DisconnectReasonUserTerminatedConnection;
        else if(payload == 0x16)
          reason = proteus3DisconnectReasonHostTerminatedConnection;
        else if(payload == 0x3B)
          reason = proteus3DisconnectReasonConnectionIntervalUnacceptable;
        else if(payload == 0x3D)
          reason = proteus3DisconnectReasonMicFailure;
        else if(payload == 0x3E)
          reason = proteus3DisconnectReasonConnectionSetupFailed;
        proteus3->callback.disconnect(reason, proteus3->callback.argument);
      }
      break;
    case indSecurity:
      if(proteus3->callback.security) {
        const Proteus3SecurityState status = *proteus3->parser.pPayload;
        uint8_t btmac[6];
        ring_buffer_copy(proteus3, 1, sizeof(btmac), btmac);
        proteus3->callback.security(status, btmac, proteus3->callback.argument);
      }
      break;
    case indRSSI:
      if(proteus3->callback.rssi) {
        uint8_t btmac[6];
        ring_buffer_copy(proteus3, 0, sizeof(btmac), btmac);
        int8_t rssi;
        ring_buffer_copy(proteus3, 6, sizeof(rssi), &rssi);
        int8_t txPower;
        ring_buffer_copy(proteus3, 7, sizeof(txPower), &txPower);
        proteus3->callback.rssi(btmac, rssi, txPower, proteus3->callback.argument);
      }
      break;
    case indBeacon:
      break;
    case indPasskey:
      if(proteus3->callback.passkey) {
        uint8_t btmac[6];
        ring_buffer_copy(proteus3, 1, sizeof(btmac), btmac);
        proteus3->callback.passkey(btmac, proteus3->callback.argument);
      }
      break;
    case indPhyUpdate:
      if(proteus3->callback.phy_update) {
        const Bool status = *proteus3->parser.pPayload == 0 ? bTrue : bFalse;
        Proteus3Phy phyRx = 0;
        Proteus3Phy phyTx = 0;
        uint8_t btmac[6];
        if(status == bTrue) {
          ring_buffer_copy(proteus3, 1, sizeof(phyRx), &phyRx);
          ring_buffer_copy(proteus3, 2, sizeof(phyTx), &phyTx);
          ring_buffer_copy(proteus3, 3, sizeof(btmac), btmac);
        }
        else {
          memset(btmac, 0, sizeof(btmac));
        }
        proteus3->callback.phy_update(status, phyRx, phyTx, btmac, proteus3->callback.argument);
      }
      break;
    case indUartEnable:
      break;
    case indError:
      if(proteus3->callback.error)
        proteus3->callback.error(*proteus3->parser.pPayload, proteus3->callback.argument);
      break;
    case indDisplayPasskey:
      if(proteus3->callback.display_passkey) {
        const Proteus3DisplayPasskeyAction action = *proteus3->parser.pPayload;
        uint8_t btmac[6];
        ring_buffer_copy(proteus3, 1, sizeof(btmac), btmac);
        uint8_t passkey[6];
        ring_buffer_copy(proteus3, 7, sizeof(passkey), passkey);
        proteus3->callback.display_passkey(action, btmac, passkey, proteus3->callback.argument);
      }
      break;
    case indGPIO_LocalWrite:
    case indGPIO_RemoteWriteConfig:
    case indGPIO_RemoteWrite:
    default: ;
  }
}

static void responses_process(Proteus3 *proteus3)
{
  switch(proteus3->parser.command) {
    case rspTxComplete:
      break;
    case rspChannelOpen:
      proteus3->channel.open = bTrue;
      ring_buffer_copy(proteus3, 1, sizeof(proteus3->channel.btmac), proteus3->channel.btmac);
      ring_buffer_copy(proteus3, 7, sizeof(proteus3->channel.maxPayload), &proteus3->channel.maxPayload);
      if(proteus3->callback.channel_open)
        proteus3->callback.channel_open(proteus3->channel.btmac, proteus3->channel.maxPayload,
            proteus3->callback.argument);
      break;
    case rspBeacon:
      if(proteus3->callback.beacon) {
        uint8_t btmac[6];
        ring_buffer_copy(proteus3, 0, sizeof(btmac), btmac);
        int8_t rssi;
        ring_buffer_copy(proteus3, 6, sizeof(rssi), &rssi);
        const uint16_t rawDataSize = proteus3->parser.payloadLength - 7;
        uint8_t *rawData = util_malloc(rawDataSize);
        ring_buffer_copy(proteus3, 7, rawDataSize, rawData);
        proteus3->callback.beacon(btmac, rssi, rawData, rawDataSize, proteus3->callback.argument);
        util_free(rawData);
      }
      break;
    default: ;
  }
}

static void message_process(Proteus3 *proteus3)
{
  switch(proteus3->parser.command & COMMAND_TYPE_MASK) {
    case cmdTypeReq:
      break;
    case cmdTypeCnf:
      confirmations_process(proteus3);
      break;
    case cmdTypeInd:
      indications_process(proteus3);
      break;
    case cmdTypeRsp:
      responses_process(proteus3);
      break;
  }
}

static uint8_t const* pread_increment(uint8_t const *buffer, size_t bufferSize, uint8_t const *pRead, size_t step)
{
  pRead += step;
  uint8_t const *endBuffer = buffer + bufferSize;
  return pRead < endBuffer ? pRead : buffer + (endBuffer - pRead);
}

void proteus3_message_parser(Proteus3 *proteus3)
{
  Parser *parser = &proteus3->parser;
  FindItem *item = &parser->item;
  uint16_t writeSize = item->pWrite >= item->pRead ? item->pWrite - item->pRead :
      item->bufferSize - (item->pRead - item->pWrite);

  while(writeSize > 0) {
    switch(parser->state) {
      case findStartSignal:
        if(*item->pRead == START_SIGNAL) {
          parser->state = findCommand;
          parser->crc = START_SIGNAL;
          parser->pStartMessage = item->pRead;
          item->pRead = pread_increment(item->buffer, item->bufferSize, item->pRead, 1);
        }
        --writeSize;
        break;
      case findCommand:
        parser->state = findLength;
        parser->crc ^= *item->pRead;
        parser->command = *item->pRead;
        item->pRead = pread_increment(item->buffer, item->bufferSize, item->pRead, 1);
        --writeSize;
        break;
      case findLength:
        if(writeSize >= 2) {
          util_ring_buffer_copy(item->buffer, item->buffer + item->bufferSize, item->pRead,
              sizeof(parser->payloadLength), &parser->payloadLength);
          parser->state = findPayload;
          parser->crc ^= (parser->payloadLength >> 8) ^ parser->payloadLength;
          item->pRead = pread_increment(item->buffer, item->bufferSize, item->pRead, 2);
          writeSize -= 2;
        }
        else {
          writeSize = 0;
        }
        break;
      case findPayload:
        if(writeSize >= parser->payloadLength) {
          parser->state = findCRC;
          parser->pPayload = item->pRead;
          for(size_t i = 0; i < parser->payloadLength; ++i) {
            parser->crc ^= *item->pRead;
            item->pRead = pread_increment(item->buffer, item->bufferSize, item->pRead, 1);
          }
          writeSize -= parser->payloadLength;
        }
        else {
          writeSize = 0;
        }
        break;
      case findCRC:
        if(*item->pRead == parser->crc) {
          message_process(proteus3);
          item->pRead = pread_increment(item->buffer, item->bufferSize, item->pRead, 1);
        }
        else {
          item->pRead = pread_increment(item->buffer, item->bufferSize, parser->pStartMessage, 1);
        }
        parser->state = findStartSignal;
        --writeSize;
        break;
    }
  }
}

void proteus3_pwrite_set(Proteus3 *proteus3, size_t txSize)
{
  proteus3->parser.item.pWrite = proteus3->parser.item.buffer + txSize;
}

Bool proteus3_ready(Proteus3 *proteus3)
{
  return proteus3->deviceReady;
}

Bool proteus3_connect_state(Proteus3 const *proteus3, uint8_t *btmac)
{
  const Bool connectState = proteus3->connect.state;
  if(connectState == bTrue && btmac)
    memcpy(btmac, proteus3->connect.btmac, sizeof(proteus3->connect.btmac));
  return connectState;
}

Proteus3State proteus3_state(Proteus3 const *proteus3)
{
  return proteus3->state;
}

Proteus3Devices const* proteus3_devices(Proteus3 const *proteus3)
{
  return &proteus3->devices;
}
