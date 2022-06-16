#include "flow_ctrl.h"
#include "usart.h"
#include "usbd_cdc_if.h"
#include "mem_find.h"
#include "sd_card.h"
#include "station.h"
#include "ppm.h"
#include "gnss.h"
#include "jet.h"
#include "mi.h"

extern osMessageQueueId_t qCDC_RxHandle;
extern osMessageQueueId_t qPPM_Tx0Handle;
extern osMessageQueueId_t qGNSS_Tx1Handle;
extern osMessageQueueId_t qMI_TxHandle;
extern osMessageQueueId_t qJetTxHandle;
extern osMessageQueueId_t qCDC_TxHandle;
extern osMessageQueueId_t qBlthTxHandle;

#define PPM_BUFF_SIZE       10480
#define GNSS_BUFF_SIZE      4096
#define MI_BUFF_SIZE        4096
#define JET_BUFF_SIZE       4096
#define BLUETOTH_BUFF_SIZE  4096

static char ppmBuff0[PPM_BUFF_SIZE];
//static char ppmBuff1[PPM_BUFF_SIZE];
//static char gnssBuff0[GNSS_BUFF_SIZE];
static char gnssBuff1[GNSS_BUFF_SIZE];
static char miBuff[MI_BUFF_SIZE];
static char jetBuff[JET_BUFF_SIZE];
static char bluetoothBuff[BLUETOTH_BUFF_SIZE];

FlowCtrl flowCtrl;

static void mi_cmd_analysis_wrapper(String const *message, void *argument)
{
  FlowCtrlProcessArgument *processArgument = argument;
  mi_cmd_analysis(message, processArgument->srcPort, processArgument->sta);
}

static void ppm_cmd_analysis_wrapper(String const *message, void *argument)
{
  FlowCtrlProcessArgument *processArgument = argument;
  ppm_cmd_analysis(message, processArgument->srcPort, processArgument->sta);
}

static void mi_response_analysis_wrapper(String const *message, void *argument)
{
  FlowCtrlProcessArgument *processArgument = argument;
  mi_response_analysis(message, processArgument->srcPort, processArgument->sta);
}

static void dummy_analysis(String const *message, void *argument)
{
  // Если сообщение не нужно никак обрабатывать и отправлять
}

static data_processing_t mi_arbiter(RingBufferItem const *item, uint8_t const *beginMessage, uint8_t const *endMessage)
{
  char const *endPattern = "END_LSN_DATA\r\n";
  const size_t endSize = sizeof("END_LSN_DATA\r\n") - 1;
  char const *pEnd = (char const*)endMessage - endSize;
  if(pEnd < (char const*)item->beginBuffer)
    pEnd = (char const*)item->endBuffer - ((char const*)item->beginBuffer - pEnd);

  if(ring_buffer_find(item->beginBuffer, item->endBuffer,
      (uint8_t const*)endPattern, (uint8_t const*)endPattern+endSize,
      (uint8_t const*)pEnd, endSize)) {
    if(beginMessage[4] == '$') {
      char const *pCommandNo = (char const*)beginMessage + sizeof("MI\r\n$LSNCMD,") - 1;
      if(pCommandNo >= (char const*)item->endBuffer)
        pCommandNo = (char const*)item->beginBuffer + (pCommandNo - (char const*)item->endBuffer);
      uint8_t commandNo = 0;
      sscanf(pCommandNo, "%hhu", &commandNo);
      switch(commandNo) {
        case ppmRequestSelfTest: {
          char const *pDeviceNo = pCommandNo + sizeof("12,00,") - 1;
          uint8_t deviceNo = 0;
          sscanf(pDeviceNo, "%hhu", &deviceNo);
          if(deviceNo == 1)
            return ppm_cmd_analysis_wrapper;
          else if(deviceNo == 4)
            return mi_cmd_analysis_wrapper;
          return dummy_analysis;
        }
        case ppmSetStPositionVal:
        case ppmSetCurrentTime:
        case ppmEthernet:
          return mi_cmd_analysis_wrapper;
        default:
          return ppm_cmd_analysis_wrapper;
      }
    }
    return mi_response_analysis_wrapper;
  }
  return NULL;
}

void flow_init()
{
  flowCtrl.raw.ppm[0].uart = &huart3;
  flowCtrl.raw.ppm[0].buff = ppmBuff0;
  flowCtrl.raw.ppm[0].size = sizeof(ppmBuff0);
  flowCtrl.raw.ppm[0].readPoint = ppmBuff0;

  flowCtrl.raw.ppm[1].uart = NULL;
  flowCtrl.raw.ppm[1].buff = NULL;
  flowCtrl.raw.ppm[1].size = 0;
  flowCtrl.raw.ppm[1].readPoint = NULL;

  flowCtrl.raw.gnss[0].uart = NULL;
  flowCtrl.raw.gnss[0].buff = NULL;
  flowCtrl.raw.gnss[0].size = 0;
  flowCtrl.raw.gnss[0].readPoint = NULL;

  flowCtrl.raw.gnss[1].uart = &huart4;
  flowCtrl.raw.gnss[1].buff = gnssBuff1;
  flowCtrl.raw.gnss[1].size = sizeof(gnssBuff1);
  flowCtrl.raw.gnss[1].readPoint = gnssBuff1;

  flowCtrl.raw.mi.uart = &huart2;
  ring_buffer_item_init(&flowCtrl.raw.mi.searchItem, (uint8_t const*)miBuff, sizeof(miBuff),
      (uint8_t const*)"\r\n", sizeof("\r\n")-1,
      mi_arbiter, &flowCtrl.raw.mi.argument);
  flowCtrl.raw.mi.argument.srcPort = flowMI;
  flowCtrl.raw.mi.argument.sta = &station;

  flowCtrl.raw.com.rxQ = qCDC_RxHandle;
  flowCtrl.raw.com.readPoint = usbRxStorage;

  flowCtrl.raw.jet.uart = &huart7;
  flowCtrl.raw.jet.buff = jetBuff;
  flowCtrl.raw.jet.size = sizeof(jetBuff);
  flowCtrl.raw.jet.readPoint = jetBuff;

  flowCtrl.raw.bluetooth.buff = bluetoothBuff;
  flowCtrl.raw.bluetooth.size = sizeof(bluetoothBuff);
  flowCtrl.raw.bluetooth.readPoint = bluetoothBuff;
  flowCtrl.raw.bluetooth.writeIndex = 0;

  flowCtrl.ripe.ppm[0].en = bTrue;
  flowCtrl.ripe.ppm[0].queue = qPPM_Tx0Handle;

  flowCtrl.ripe.ppm[1].en = bFalse;
  flowCtrl.ripe.ppm[1].queue = NULL;

  flowCtrl.ripe.gnss[0].en = bFalse;
  flowCtrl.ripe.gnss[0].queue = NULL;
  flowCtrl.ripe.gnss[0].accum = make_str_null();
  flowCtrl.ripe.gnss[0].start = 0;
  flowCtrl.ripe.gnss[0].timeout = 100;

  flowCtrl.ripe.gnss[1].en = bTrue;
  flowCtrl.ripe.gnss[1].queue = qGNSS_Tx1Handle;
  flowCtrl.ripe.gnss[1].accum = make_str_null();
  flowCtrl.ripe.gnss[1].start = 0;
  flowCtrl.ripe.gnss[1].timeout = 100;

  flowCtrl.ripe.mi.en = bTrue;
  flowCtrl.ripe.mi.queue = qMI_TxHandle;

  flowCtrl.ripe.com.en = bTrue;
  flowCtrl.ripe.com.queue = qCDC_TxHandle;

  flowCtrl.ripe.jet.en = bTrue;
  flowCtrl.ripe.jet.queue = qJetTxHandle;

  flowCtrl.ripe.bluetooth.en = bFalse;
  flowCtrl.ripe.bluetooth.queue = qBlthTxHandle;

  flowCtrl.ripe.screen.ppm.en = bFalse;
  flowCtrl.ripe.screen.ppm.queue = NULL;
  flowCtrl.ripe.screen.gnss.en = bFalse;
  flowCtrl.ripe.screen.gnss.queue = NULL;
}

static inline void buff_process(char const *buffer, size_t buffSize, char const **readPoint, size_t rxLen,
                                char const *end, size_t endSize, Station *sta, FlowPort srcPort,
                                data_proc_fp_t data_proc_fp)
{
  char const *endRxBuff = buffer + rxLen;
  const size_t lenRxData = (endRxBuff >= *readPoint) ? endRxBuff - *readPoint:
      rxLen + buffer + buffSize - *readPoint;

  char const* startPos = *readPoint;
  char const* endPos = NULL;
  char const* startFind = *readPoint;
  size_t lenTail = lenRxData;

  while(bTrue) {
    endPos = mem_find_circ(buffer, buffSize, startFind, lenTail, end, endSize);
    if(endPos) {
      String str = make_str_circ(buffer, buffSize, startPos, endPos);
      lenTail -= str.len;

      data_proc_fp(&str, srcPort, sta);
      str_clear(&str);

      startPos = endPos;
      startFind = endPos;
      continue;
    }
    break;
  }
  *readPoint = startFind;
}

void flow_ppm0_process(FlowCtrl *flow, Station *sta)
{
  if(flow->raw.ppm[0].uart->Instance->SR & USART_SR_IDLE) {
    const size_t rxLen = flow->raw.ppm[0].size - flow->raw.ppm[0].uart->hdmarx->Instance->NDTR;
    buff_process(flow->raw.ppm[0].buff, flow->raw.ppm[0].size, &flow->raw.ppm[0].readPoint, rxLen,
        "END_LSN_DATA\r\n", 14, sta, flowPPM0, ppm_resp_analysis);
  }
}

void flow_ppm1_process(FlowCtrl *flow, Station *sta)
{
  if(flow->raw.ppm[1].uart->Instance->SR & USART_SR_IDLE) {
    const size_t rxLen = flow->raw.ppm[1].size - flow->raw.ppm[1].uart->hdmarx->Instance->NDTR;
    buff_process(flow->raw.ppm[1].buff, flow->raw.ppm[1].size, &flow->raw.ppm[1].readPoint, rxLen,
        "END_LSN_DATA\r\n", 14, sta, flowPPM1, ppm_resp_analysis);
  }
}

void flow_gnss0_process(FlowCtrl *flow, Station *sta)
{
  if(sta->gnss.power == pOn && (flow->raw.gnss[0].uart->Instance->SR & USART_SR_IDLE)) {
    const size_t rxLen = flow->raw.gnss[0].size - flow->raw.gnss[0].uart->hdmarx->Instance->NDTR;
    buff_process(flow->raw.gnss[0].buff, flow->raw.gnss[0].size, &flow->raw.gnss[0].readPoint, rxLen,
        "\r\n", 2, sta, flowGNSS0, gnss_resp_data_proc);
  }
}

void flow_gnss1_process(FlowCtrl *flow, Station *sta)
{
  if(sta->gnss.power == pOn && (flow->raw.gnss[1].uart->Instance->SR & USART_SR_IDLE)) {
    const size_t rxLen = flow->raw.gnss[1].size - flow->raw.gnss[1].uart->hdmarx->Instance->NDTR;
    buff_process(flow->raw.gnss[1].buff, flow->raw.gnss[1].size, &flow->raw.gnss[1].readPoint, rxLen,
        "\r\n", 2, sta, flowGNSS1, gnss_resp_data_proc);
  }
}

void flow_mi_process(FlowCtrl *flow, Station *sta)
{
  if(flow->raw.mi.uart->Instance->SR & USART_SR_IDLE) {
    const size_t rxLen = flow->raw.mi.searchItem.sizeBuffer - flow->raw.mi.uart->hdmarx->Instance->NDTR;
    const size_t sizeWrite = flow->raw.mi.searchItem.pWrite - flow->raw.mi.searchItem.beginBuffer;
    const size_t deltaWrite = rxLen >= sizeWrite ? rxLen - sizeWrite :
        flow->raw.mi.searchItem.sizeBuffer - sizeWrite + rxLen;
    ring_buffer_increase_write_point(&flow->raw.mi.searchItem, deltaWrite);
    ring_buffer_process(&flow->raw.mi.searchItem);
  }
}

void flow_com_process(FlowCtrl *flow, Station *sta)
{
  CDCRxData rx;
  xQueueReceive(flow->raw.com.rxQ, &rx, portMAX_DELAY);
  buff_process(rx.buff, rx.size, &flow->raw.com.readPoint, rx.rxLen,"\r\n", 2, sta, flowCOM, ppm_cmd_analysis);
}

void flow_jet_process(FlowCtrl *flow, Station *sta)
{
  if(flow->raw.jet.uart->Instance->SR & USART_SR_IDLE) {
    const size_t rxLen = flow->raw.jet.size - flow->raw.jet.uart->hdmarx->Instance->NDTR;
    buff_process(flow->raw.jet.buff, flow->raw.jet.size, &flow->raw.jet.readPoint, rxLen,
        "END.", 4, sta, flowJet, jet_cmd_analysis);
  }
}

void flow_bluetooth_append_data(FlowCtrl *flow, uint8_t const *data, size_t size)
{
  const uint32_t oldWriteIndex = flow->raw.bluetooth.writeIndex;
  flow->raw.bluetooth.writeIndex += size;
  flow->raw.bluetooth.writeIndex %= flow->raw.bluetooth.size;
  if(flow->raw.bluetooth.writeIndex >= oldWriteIndex) {
    memcpy(&flow->raw.bluetooth.buff[oldWriteIndex], data, size);
  }
  else {
    const size_t size0 = flow->raw.bluetooth.size - oldWriteIndex;
    memcpy(&flow->raw.bluetooth.buff[oldWriteIndex], data, size0);
    const size_t size1 = size - size0;
    memcpy(flow->raw.bluetooth.buff, &data[size0], size1);
  }
}

void flow_bluetooth_process(FlowCtrl *flow, Station *sta)
{
  const size_t rxLen = flow->raw.bluetooth.writeIndex;
  buff_process(flow->raw.bluetooth.buff, flow->raw.bluetooth.size, &flow->raw.bluetooth.readPoint, rxLen,
      "\r\n", 2, sta, flowBluetooth, ppm_cmd_analysis);
}

void flow_ppm0_give_own(FlowCtrl const *flow, String *str)
{
  if(flow->ripe.ppm[0].en == bTrue)
    xQueueSendToBack(flow->ripe.ppm[0].queue, str, portMAX_DELAY);
  else
    str_clear(str);
}

void flow_ppm1_give_own(FlowCtrl const *flow, String *str)
{
  if(flow->ripe.ppm[1].en == bTrue)
    xQueueSendToBack(flow->ripe.ppm[1].queue, str, portMAX_DELAY);
  else
    str_clear(str);
}

void flow_gnss0_give_own(FlowCtrl const *flow, String *str)
{
  if(flow->ripe.gnss[0].en == bTrue)
    xQueueSendToBack(flow->ripe.gnss[0].queue, str, portMAX_DELAY);
  else
    str_clear(str);
}

void flow_gnss1_give_own(FlowCtrl const *flow, String *str)
{
  if(flow->ripe.gnss[1].en == bTrue)
    xQueueSendToBack(flow->ripe.gnss[1].queue, str, portMAX_DELAY);
  else
    str_clear(str);
}

void flow_mi_give_own(FlowCtrl const *flow, String *str)
{
  if(flow->ripe.mi.en == bTrue)
    xQueueSendToBack(flow->ripe.mi.queue, str, portMAX_DELAY);
  else
    str_clear(str);
}

void flow_com_give_own(FlowCtrl const *flow, String *str)
{
  if(flow->ripe.com.en == bTrue)
    xQueueSendToBack(flow->ripe.com.queue, str, portMAX_DELAY);
  else
    str_clear(str);
}

void flow_jet_give_own(FlowCtrl const *flow, String *str)
{
  if(flow->ripe.jet.en == bTrue)
    xQueueSendToBack(flow->ripe.jet.queue, str, portMAX_DELAY);
  else
    str_clear(str);
}

void flow_bluetooth_give_own(FlowCtrl const *flow, String *str)
{
  if(flow->ripe.bluetooth.en == bTrue)
    xQueueSendToBack(flow->ripe.bluetooth.queue, str, portMAX_DELAY);
  else
    str_clear(str);
}

void flow_screen_give_own(FlowCtrl const *flow, String *str, FlowPort srcPort)
{
  switch (srcPort) {
    case flowPPM0:
    case flowPPM1:
      if(flow->ripe.screen.ppm.en == bTrue)
        xQueueSendToBack(flow->ripe.screen.ppm.queue, str, portMAX_DELAY);
      else
        str_clear(str);
      break;
    case flowGNSS0:
    case flowGNSS1:
      if(flow->ripe.screen.gnss.en == bTrue)
        xQueueSendToBack(flow->ripe.screen.gnss.queue, str, portMAX_DELAY);
      else
        str_clear(str);
      break;
    case flowMI:
    case flowCOM:
    case flowJet:
    case flowBluetooth:
      str_clear(str);
      break;
  }
}

void flow_ppm0_give(FlowCtrl const *flow, String const *str)
{
  if(flow->ripe.ppm[0].en == bTrue) {
    String msg = str_copy(str);
    xQueueSendToBack(flow->ripe.ppm[0].queue, &msg, portMAX_DELAY);
  }
}

void flow_ppm1_give(FlowCtrl const *flow, String const *str)
{
  if(flow->ripe.ppm[1].en == bTrue) {
    String msg = str_copy(str);
    xQueueSendToBack(flow->ripe.ppm[1].queue, &msg, portMAX_DELAY);
  }
}

void flow_gnss0_give(FlowCtrl const *flow, String const *str)
{
  if(flow->ripe.gnss[0].en == bTrue) {
    String msg = str_copy(str);
    xQueueSendToBack(flow->ripe.gnss[0].queue, &msg, portMAX_DELAY);
  }
}

void flow_gnss1_give(FlowCtrl const *flow, String const *str)
{
  if(flow->ripe.gnss[1].en == bTrue) {
    String msg = str_copy(str);
    xQueueSendToBack(flow->ripe.gnss[1].queue, &msg, portMAX_DELAY);
  }
}

void flow_mi_give(FlowCtrl const *flow, String const *str)
{
  if(flow->ripe.mi.en == bTrue) {
    String msg = str_copy(str);
    xQueueSendToBack(flow->ripe.mi.queue, &msg, portMAX_DELAY);
  }
}

void flow_com_give(FlowCtrl const *flow, String const *str)
{
  if(flow->ripe.com.en == bTrue) {
    String msg = str_copy(str);
    xQueueSendToBack(flow->ripe.com.queue, &msg, portMAX_DELAY);
  }
}

void flow_jet_give(FlowCtrl const *flow, String const *str)
{
  if(flow->ripe.jet.en == bTrue) {
    String msg = str_copy(str);
    xQueueSendToBack(flow->ripe.jet.queue, &msg, portMAX_DELAY);
  }
}

void flow_bluetooth_give(FlowCtrl const *flow, String const *str)
{
  if(flow->ripe.bluetooth.en == bTrue) {
    String msg = str_copy(str);
    xQueueSendToBack(flow->ripe.bluetooth.queue, &msg, portMAX_DELAY);
  }
}

void flow_sd_give(FlowCtrl const *flow, String const *str, FlowPort srcPort)
{
  switch(srcPort) {
    case flowPPM0:
    case flowPPM1:
      sd_push(sd_instance(), str, sdCategoryPPM);
      break;
    case flowGNSS0:
    case flowGNSS1:
      sd_push(sd_instance(), str, sdCategoryGNSS);
      break;
    case flowMI:
    case flowCOM:
    case flowJet:
    case flowBluetooth:
      break;
  }
}

void flow_screen_give(FlowCtrl const *flow, String const *str, FlowPort srcPort)
{
  switch(srcPort) {
    case flowPPM0:
    case flowPPM1:
      if(flow->ripe.screen.ppm.en == bTrue) {
        String msg = str_copy(str);
        xQueueSendToBack(flow->ripe.screen.ppm.queue, &msg, 0);
      }
      break;
    case flowGNSS0:
    case flowGNSS1:
      if(flow->ripe.screen.gnss.en == bTrue) {
        String msg = str_copy(str);
        xQueueSendToBack(flow->ripe.screen.gnss.queue, &msg, 0);
      }
      break;
    case flowMI:
    case flowCOM:
    case flowJet:
    case flowBluetooth:
      break;
  }
}

void flow_gnss0_tx_data(FlowCtrl *flow, Station const *sta)
{
  const uint32_t time = flow->ripe.gnss[0].start + flow->ripe.gnss[0].timeout;
  if(xTaskGetTickCount() >= time && flow->ripe.gnss[0].accum.len > 0) {
    String *str = &flow->ripe.gnss[0].accum;
    str_append(str, "END_LSN_DATA\r\n");
    if(sta->outToMI.gnss == bTrue)
      flow_mi_give(&flowCtrl, str);
    if(sta->outToCOM.gnss == bTrue)
      flow_com_give(&flowCtrl, str);
    if(sta->outToBlth.gnss == bTrue)
      flow_bluetooth_give(&flowCtrl, str);
    flow_sd_give(&flowCtrl, str, flowGNSS0);
    str_clear(&flow->ripe.gnss[0].accum);
  }
}

void flow_gnss1_tx_data(FlowCtrl *flow, Station const *sta)
{
  const uint32_t time = flow->ripe.gnss[1].start + flow->ripe.gnss[1].timeout;
  if(xTaskGetTickCount() >= time && flow->ripe.gnss[1].accum.len > 0) {
    String *str = &flow->ripe.gnss[1].accum;
    str_append(str, "END_LSN_DATA\r\n");
    if(sta->outToMI.gnss == bTrue)
      flow_mi_give(&flowCtrl, str);
    if(sta->outToCOM.gnss == bTrue)
      flow_com_give(&flowCtrl, str);
    if(sta->outToBlth.gnss == bTrue)
      flow_bluetooth_give(&flowCtrl, str);
    flow_sd_give(&flowCtrl, str, flowGNSS1);
    str_clear(&flow->ripe.gnss[1].accum);
  }
}
