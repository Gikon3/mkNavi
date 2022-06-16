#include "javad_cmd.h"
#include <string.h>
#include <stdio.h>
#include "flow_ctrl.h"

static const char commands[][25] = {
    "em,/dev/ser/d,nmea/GGA\r", "dm,/dev/ser/d,nmea/GGA\r",
    "em,/dev/ser/d,nmea/GLL\r", "dm,/dev/ser/d,nmea/GLL\r",
    "em,/dev/ser/d,nmea/GNS\r", "dm,/dev/ser/d,nmea/GNS\r",
    "em,/dev/ser/d,nmea/GRS\r", "dm,/dev/ser/d,nmea/GRS\r",
    "em,/dev/ser/d,nmea/GSA\r", "dm,/dev/ser/d,nmea/GSA\r",
    "em,/dev/ser/d,nmea/GST\r", "dm,/dev/ser/d,nmea/GST\r",
    "em,/dev/ser/d,nmea/GSV\r", "dm,/dev/ser/d,nmea/GSV\r",
    "em,/dev/ser/d,nmea/RMC\r", "dm,/dev/ser/d,nmea/RMC\r",
    "em,/dev/ser/d,nmea/HDT\r", "dm,/dev/ser/d,nmea/HDT\r",
    "em,/dev/ser/d,nmea/VTG\r", "dm,/dev/ser/d,nmea/VTG\r",
    "em,/dev/ser/d,nmea/ROT\r", "dm,/dev/ser/d,nmea/ROT\r",
    "em,/dev/ser/d,nmea/ZDA\r", "dm,/dev/ser/d,nmea/ZDA\r",
    "em,/dev/ser/d,nmea/GMP\r", "dm,/dev/ser/d,nmea/GMP\r"
};

void javad_send_nmeacmd(Station* sta, FlowPort port, JavadNMEA_Cmd cmd)
{
  if(port != flowGNSS0 && port != flowGNSS1)
    return;
  const int8_t numPort = port == flowGNSS0 ? 0 : 1;
  switch(cmd) {
    case javNCmdGGA_On : sta->gnss.issueRate[numPort].gga = gnssIss1Sec; break;
    case javNCmdGGA_Off: sta->gnss.issueRate[numPort].gga = gnssIssNone; break;
    case javNCmdGLL_On : sta->gnss.issueRate[numPort].gll = gnssIss1Sec; break;
    case javNCmdGLL_Off: sta->gnss.issueRate[numPort].gll = gnssIssNone; break;
    case javNCmdGSA_On : sta->gnss.issueRate[numPort].gsa = gnssIss1Sec; break;
    case javNCmdGSA_Off: sta->gnss.issueRate[numPort].gsa = gnssIssNone; break;
    case javNCmdGSV_On : sta->gnss.issueRate[numPort].gsv = gnssIss1Sec; break;
    case javNCmdGSV_Off: sta->gnss.issueRate[numPort].gsv = gnssIssNone; break;
    case javNCmdRMC_On : sta->gnss.issueRate[numPort].rmc = gnssIss1Sec; break;
    case javNCmdRMC_Off: sta->gnss.issueRate[numPort].rmc = gnssIssNone; break;
    case javNCmdVTG_On : sta->gnss.issueRate[numPort].vtg = gnssIss1Sec; break;
    case javNCmdVTG_Off: sta->gnss.issueRate[numPort].vtg = gnssIssNone; break;
    case javNCmdZDA_On : sta->gnss.issueRate[numPort].zda = gnssIss1Sec; break;
    case javNCmdZDA_Off: sta->gnss.issueRate[numPort].zda = gnssIssNone; break;
    default: break;
  }

  String msg = make_str(commands[cmd]);
  flow_screen_give(&flowCtrl, &msg, port);
  if(port == flowGNSS0)
    flow_gnss0_give_own(&flowCtrl, &msg);
  else
    flow_gnss1_give_own(&flowCtrl, &msg);
}

void javad_send(FlowPort port, char const* str)
{
  String msg = make_str(str);
  flow_screen_give(&flowCtrl, &msg, port);
  if(port == flowGNSS0)
    flow_gnss0_give_own(&flowCtrl, &msg);
  else
    flow_gnss1_give_own(&flowCtrl, &msg);
}

void javad_set_imode(Station const* sta, FlowPort port, char serPort, JavadInMode imode)
{
  char const *imodeChar = NULL;
  switch(imode) {
    case javInModeCmd: imodeChar = "cmd"; break;
    case javInModeEcho: imodeChar = "echo"; break;
    case javInModeJPS: imodeChar = "jps"; break;
    case javInModeRTCM: imodeChar = "rtcm"; break;
    case javInModeRTCM3: imodeChar = "rtcm3"; break;
    case javInModeCMR: imodeChar = "cmr"; break;
    case javInModeOMNI: imodeChar = "omni"; break;
    case javInModeDTP: imodeChar = "dtp"; break;
    case javInModeTerm: imodeChar = "term"; break;
  }

  char accum[32];
  snprintf(accum, sizeof(accum), "set,/par/dev/ser/%c/imode,%s\r", serPort, imodeChar);
  javad_send(port, accum);
}

void javad_switch_solution(Station* sta, FlowPort port, GNSS_NavMode mode)
{
  char const *solution = NULL;
  switch(mode) {
    case gnssNavModeRTK_Fix: solution = "pd"; break;
    case gnssNavModeRTK_Float: solution = "pf"; break;
    case gnssNavModeDGPS: solution = "cd"; break;
    case gnssNavModeWDGPS:solution = "wd"; break;
    case gnssNavModeSP: solution = "sp"; break;
    case gnssNavModePPP: solution = "pp"; break;
  }

  sta->gnss.navMode = mode;
  char accum[26];
  snprintf(accum, sizeof(accum), "set,/par/pos/mode/cur,%s\r", solution);
  javad_send(port, accum);
}

void javad_en_code_diff_pos(Station* sta, FlowPort port, Bool mode)
{
  char accum[26];
  snprintf(accum, sizeof(accum), "set,/par/pos/mode/cd,%s\r", mode == bFalse ? "off" : "on");
  javad_send(port, accum);
}

void javad_en_sbas_code_diff_pos(Station* sta, FlowPort port, Bool mode)
{
  char accum[26];
  snprintf(accum, sizeof(accum), "set,/par/pos/mode/wd,%s\r", mode == bFalse ? "off" : "on");
  javad_send(port, accum);
}

void javad_en_sattelite_system(Station* sta, FlowPort port, GNSS_SatSystem system)
{
  char accum[26];
  snprintf(accum, sizeof(accum), "set,/par/pos/sys/gps,%c\r", system & gnssSatGPS ? 'y' : 'n');
  javad_send(port, accum);
  snprintf(accum, sizeof(accum), "set,/par/pos/sys/glo,%c\r", system & gnssSatGLO ? 'y' : 'n');
  javad_send(port, accum);
  snprintf(accum, sizeof(accum), "set,/par/pos/sys/gal,%c\r", system & gnssSatGAL ? 'y' : 'n');
  javad_send(port, accum);
  snprintf(accum, sizeof(accum), "set,/par/pos/sys/sbas,%c\r", system & gnssSatSBAS ? 'y' : 'n');
  javad_send(port, accum);
  snprintf(accum, sizeof(accum), "set,/par/pos/sys/qzss,%c\r", system & gnssSatQZSS ? 'y' : 'n');
  javad_send(port, accum);
  snprintf(accum, sizeof(accum), "set,/par/pos/sys/comp,%c\r", system & gnssSatBDS ? 'y' : 'n');
  javad_send(port, accum);
  snprintf(accum, sizeof(accum), "set,/par/pos/sys/irnss,%c\r", system & gnssSatIRNSS ? 'y' : 'n');
  javad_send(port, accum);
}
