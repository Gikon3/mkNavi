#include "gnss_cmd.h"
#include "flow_ctrl.h"

static const char commands[][19] = {
    "$GPSGG,SWPROT*75\r\n", "$GPSGG,SAVEFL*63\r\n", "$GPSGG,CSTART*6B\r\n", "$GPSGG,WSTART*7F\r\n",
    "$GPSGG,HSTART*60\r\n", "$GPSGG,RQUERY*70\r\n", "$GPSGG,NQUERY*6C\r\n", "$GPSGG,BDR004*08\r\n",
    "$GPSGG,BDR009*05\r\n", "$GPSGG,BDR019*04\r\n", "$GPSGG,BDR038*07\r\n", "$GPSGG,BDR057*0E\r\n",
    "$GPSGG,BDR115*09\r\n", "$GPSGG,BDR230*0D\r\n", "$GPSGG,BDR460*0E\r\n", "$GPSGG,BDR921*06\r\n",
    "$GPSGG,STOP01*71\r\n", "$GPSGG,STOP02*72\r\n", "$GPSGG,GGA ON*08\r\n", "$GPSGG,GGAOFF*66\r\n",
    "$GPSGG,GLL ON*0E\r\n", "$GPSGG,GLLOFF*60\r\n", "$GPSGG,GSA ON*1C\r\n", "$GPSGG,GSAOFF*72\r\n",
    "$GPSGG,GSV ON*0B\r\n", "$GPSGG,GSVOFF*65\r\n", "$GPSGG,RMC ON*15\r\n", "$GPSGG,RMCOFF*7B\r\n",
    "$GPSGG,VTG ON*0C\r\n", "$GPSGG,VTGOFF*62\r\n", "$GPSGG,ZDA ON*16\r\n", "$GPSGG,ZDAOFF*78\r\n",
    "$GPSGG,DTM ON*14\r\n", "$GPSGG,DTMOFF*7A\r\n", "$GPSGG,RLM ON*1A\r\n", "$GPSGG,RLMOFF*74\r\n",
    "$GPSGG,NMEAV2*0B\r\n", "$GPSGG,NMEAV4*0D\r\n", "$GPSGG,RATE01*6B\r\n", "$GPSGG,RATE02*68\r\n",
    "$GPSGG,RATE05*6F\r\n", "$GPSGG,RATE10*6B\r\n", "$GPSGG,ELEV05*77\r\n", "$GPSGG,ELEV06*74\r\n",
    "$GPSGG,ELEV07*75\r\n", "$GPSGG,ELEV08*7A\r\n", "$GPSGG,ELEV09*7B\r\n", "$GPSGG,ELEV10*73\r\n",
    "$GPSGG,ELEV11*72\r\n", "$GPSGG,ELEV12*71\r\n", "$GPSGG,ELEV13*70\r\n", "$GPSGG,ELEV14*77\r\n",
    "$GPSGG,ELEV15*76\r\n", "$GPSGG,DATP90*60\r\n", "$GPSGG,DATW84*62\r\n", "$GPSGG,NVSGPS*67\r\n",
    "$GPSGG,NVSGLN*66\r\n", "$GPSGG,NVSMIX*7F\r\n"
};

void gnss_send_nmeacmd(Station* sta, FlowPort port, GNSS_NMEA_Cmd cmd)
{
  if (port != flowGNSS0 && port != flowGNSS1) return;
  const int8_t numPort = port == flowGNSS0 ? 0 : 1;
  switch (cmd) {
    case gnssNCmdGGA_On : sta->gnss.issueRate[numPort].gga = gnssIss1Sec; break;
    case gnssNCmdGGA_Off: sta->gnss.issueRate[numPort].gga = gnssIssNone; break;
    case gnssNCmdGLL_On : sta->gnss.issueRate[numPort].gll = gnssIss1Sec; break;
    case gnssNCmdGLL_Off: sta->gnss.issueRate[numPort].gll = gnssIssNone; break;
    case gnssNCmdGSA_On : sta->gnss.issueRate[numPort].gsa = gnssIss1Sec; break;
    case gnssNCmdGSA_Off: sta->gnss.issueRate[numPort].gsa = gnssIssNone; break;
    case gnssNCmdGSV_On : sta->gnss.issueRate[numPort].gsv = gnssIss1Sec; break;
    case gnssNCmdGSV_Off: sta->gnss.issueRate[numPort].gsv = gnssIssNone; break;
    case gnssNCmdRMC_On : sta->gnss.issueRate[numPort].rmc = gnssIss1Sec; break;
    case gnssNCmdRMC_Off: sta->gnss.issueRate[numPort].rmc = gnssIssNone; break;
    case gnssNCmdVTG_On : sta->gnss.issueRate[numPort].vtg = gnssIss1Sec; break;
    case gnssNCmdVTG_Off: sta->gnss.issueRate[numPort].vtg = gnssIssNone; break;
    case gnssNCmdZDA_On : sta->gnss.issueRate[numPort].zda = gnssIss1Sec; break;
    case gnssNCmdZDA_Off: sta->gnss.issueRate[numPort].zda = gnssIssNone; break;
    case gnssNCmdDTM_On : sta->gnss.issueRate[numPort].dtm = gnssIss1Sec; break;
    case gnssNCmdDTM_Off: sta->gnss.issueRate[numPort].dtm = gnssIssNone; break;
    case gnssNCmdRLM_On : sta->gnss.issueRate[numPort].rlm = gnssIss1Sec; break;
    case gnssNCmdRLM_Off: sta->gnss.issueRate[numPort].rlm = gnssIssNone; break;
    case gnssNCmdNMEA_V2: sta->gnss.issueRate[numPort].nmeaVer = gnssNMEA_Ver2; break;
    case gnssNCmdNMEA_V4: sta->gnss.issueRate[numPort].nmeaVer = gnssNMEA_Ver4; break;
    case gnssNCmdRate01: sta->gnss.issueRate[numPort].outData = gnssIss1Hz; break;
    case gnssNCmdRate02: sta->gnss.issueRate[numPort].outData = gnssIss2Hz; break;
    case gnssNCmdRate05: sta->gnss.issueRate[numPort].outData = gnssIss5Hz; break;
    case gnssNCmdRate10: sta->gnss.issueRate[numPort].outData = gnssIss10Hz; break;
    default: break;
  }

  String msg = make_str(commands[cmd]);
  flow_screen_give(&flowCtrl, &msg, port);
  if (port == flowGNSS0) flow_gnss0_give_own(&flowCtrl, &msg);
  else flow_gnss1_give_own(&flowCtrl, &msg);
}
