#include "comm_point.h"
#include "cmsis_os.h"
#include "queue.h"
#include "semphr.h"

Bool comm_send(CommPoint* comm, uint8_t address, uint8_t* data, uint16_t size)
{
  switch (comm->type) {
    case commI2C: {
      I2cQNode node;
      node.i2c = comm->i2cIF;
      node.type = typeTx;
      node.address = address;
      node.data = data;
      node.dataSize = size;
      node.ready = NULL;
      xQueueSendToBack(comm->i2cIF->queue, &node, portMAX_DELAY);
      break;
    }
    case commSPI: {
      SpiQNode node;
      node.spi = comm->spiIF;
      node.type = typeTx;
      node.address = address;
      node.data = data;
      node.dataSize = size;
      node.ready = NULL;
      node.sdParam = NULL;
      xQueueSendToBack(comm->spiIF->queue, &node, portMAX_DELAY);
      break;
    }
    default:
      return bFalse;
  }
  return bTrue;
}

Bool comm_recv(CommPoint* comm, uint8_t address, uint8_t* data, uint16_t size)
{
  switch (comm->type) {
    case commI2C: {
      I2cQNode node;
      node.i2c = comm->i2cIF;
      node.type = typeRx;
      node.address = address;
      node.data = data;
      node.dataSize = size;
      node.ready = xSemaphoreCreateBinary();
      xQueueSendToBack(comm->i2cIF->queue, &node, portMAX_DELAY);
      xSemaphoreTake(node.ready, portMAX_DELAY);
      vSemaphoreDelete(node.ready);
      break;
    }
    case commSPI: {
      SpiQNode node;
      node.spi = comm->spiIF;
      node.type = typeRx;
      node.address = address;
      node.data = data;
      node.dataSize = size;
      node.ready = xSemaphoreCreateBinary();
      node.sdParam = NULL;
      xQueueSendToBack(comm->spiIF->queue, &node, portMAX_DELAY);
      xSemaphoreTake(node.ready, portMAX_DELAY);
      vSemaphoreDelete(node.ready);
      break;
    }
    default:
      return bFalse;
  }
  return bTrue;
}

Bool comm_is_ready(CommPoint* comm, uint8_t whoamiAddr, uint8_t whoamiVal)
{
  switch (comm->type) {
    case commI2C: {
      uint8_t isReady;
      I2cQNode node;
      node.i2c = comm->i2cIF;
      node.type = typeIsReady;
      node.address = 0;
      node.data = &isReady;
      node.dataSize = sizeof(isReady);
      node.ready = xSemaphoreCreateBinary();
      xQueueSendToBack(comm->i2cIF->queue, &node, portMAX_DELAY);
      xSemaphoreTake(node.ready, portMAX_DELAY);
      vSemaphoreDelete(node.ready);
      if (isReady != HAL_OK) return bFalse;
      break;
    }
    case commSPI:
      break;
    default:
      return bFalse;
  }
  uint8_t whoami = 0;
  comm_recv(comm, whoamiAddr, &whoami, sizeof(whoami));
  return whoami == whoamiVal ? bTrue : bFalse;
}
