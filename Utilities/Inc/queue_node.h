#ifndef INC_QUEUE_NODE_H_
#define INC_QUEUE_NODE_H_

#include "ff_gen_drv.h"

typedef enum {
  typeTx,
  typeRx,
  typeCmd,
  typeTxData,
  typeRxData,
  typeIsReady
} TranType;

typedef enum {
  sdInit,
  sdRead,
  sdWrite,
  sdIoctl
} SdType;

typedef struct
{
  SdType type;
  BYTE pdrv;
  BYTE *buff;
  DWORD sector;
  UINT count;
  BYTE cmd;
  void *buffCmd;
  DRESULT result;
} SdNode;

#endif /* INC_QUEUE_NODE_H_ */
