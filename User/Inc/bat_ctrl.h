#ifndef INC_BAT_CTRL_H_
#define INC_BAT_CTRL_H_

#include "stm32f4xx_hal.h"

#define BAT_REG00   0x00  /* Battery Voltage Limit */
#define BAT_REG01   0x01  /* Charge Current Limit */
#define BAT_REG02   0x02  /* Input Voltage Limit */
#define BAT_REG03   0x03  /* Input Current Limit */
#define BAT_REG04   0x04  /* Precharge and Termination Control */
#define BAT_REG05   0x05  /* Charger Control 1 */
#define BAT_REG06   0x06  /* Charger Control 2 */
#define BAT_REG07   0x07  /* Charger Control 3 */
#define BAT_REG08   0x08  /* Charger Control 4 */
#define BAT_REG09   0x09  /* OTG Control */
#define BAT_REG0A   0x0A  /* ICO Current Limit */
#define BAT_REG0B   0x0B  /* Charger Status 1 */
#define BAT_REG0C   0x0C  /* Charger Status 2 */
#define BAT_REG0D   0x0D  /* NTC Status */
#define BAT_REG0E   0x0E  /* FAULT Status */
#define BAT_REG0F   0x0F  /* Charger Flag 1 */
#define BAT_REG10   0x10  /* Charger Flag 2 */
#define BAT_REG11   0x11  /* Fault Flag */
#define BAT_REG12   0x12  /* Charger Mask 1 */
#define BAT_REG13   0x13  /* Charger Mask 2 */
#define BAT_REG14   0x14  /* Fault Mask */
#define BAT_REG15   0x15  /* ADC Control */
#define BAT_REG16   0x16  /* ADC Function Disable */
#define BAT_REG17   0x17  /* IBUS ADC1 */
#define BAT_REG18   0x18  /* IBUS ADC0 */
#define BAT_REG19   0x19  /* ICHG ADC1 */
#define BAT_REG1A   0x1A  /* ICHG ADC0 */
#define BAT_REG1B   0x1B  /* VBUS ADC1 */
#define BAT_REG1C   0x1C  /* VBUS ADC0 */
#define BAT_REG1D   0x1D  /* VBAT ADC1 */
#define BAT_REG1E   0x1E  /* VBAT ADC0 */
#define BAT_REG1F   0x1F  /* VSYS ADC1 */
#define BAT_REG20   0x20  /* VSYS ADC0 */
#define BAT_REG21   0x21  /* TS ADC1 */
#define BAT_REG22   0x22  /* TS ADC0 */
#define BAT_REG23   0x23  /* TDIE ADC1 */
#define BAT_REG24   0x24  /* TDIE ADC0 */
#define BAT_REG25   0x25  /* Part Information */

typedef struct
{
  uint8_t vrechg      : 2;
  uint8_t batlowv     : 1;
  uint8_t enChg       : 1;
  uint8_t treg        : 2;
  uint8_t autoIndetEn : 1;
  uint8_t enOtg       : 1;
} BatReg06;

typedef enum {
  chrgNotCharging = 0,
  chrgTrickleCharge,
  chrgPreCharge,
  chrgFastCharge,
  chrgTaperCharge,
  chrgTopOffTimerCharging,
  chrgChargeTerminationDone
} ChrgStat;

typedef enum {
  pgNotPowerGood = 0,
  pgPowerGood
} PgStat;

typedef enum {
  vbusNoInput = 0,
  vbusUsbHostSdp,
  vbusUsbCdp,
  vbusAdapter,
  vbusPoorSrc,
  vbusUnknownAdapter,
  vbusNonStandardAdapter,
  vbusOtg
} VbusStat;

typedef enum {
  icoDisabled,
  icoOptimization,
  icoMaximumInputCurrent
} IcoStat;

HAL_StatusTypeDef bat_is_ready();
void bat_config();
void bat_wr(uint8_t address, uint8_t* data, uint16_t size);
void bat_rd(uint8_t address, uint8_t* data, uint16_t size);
void bat_wr_word(uint8_t address, uint8_t data);
uint8_t bat_rd_word(uint8_t address);

void bat_refresh_chrg_stat();
ChrgStat bat_get_chrg_stat();
PgStat bat_get_pg_stat();
VbusStat bat_get_vbus_stat();
IcoStat bat_get_ico_stat();
void bat_refresh_ibus();
void bat_refresh_ichrg();
void bat_refresh_vbus();
void bat_refresh_vbat();
void bat_refresh_vsys();
void bat_refresh_ts();
void bat_refresh_tdie();
int16_t bat_get_ibus();
int16_t bat_get_ichrg();
int16_t bat_get_vbus();
int16_t bat_get_vbat();
int16_t bat_get_vsys();
float bat_get_ts();
float bat_get_tdie();
void bat_out_en();
void bat_out_dis();

#endif /* INC_BAT_CTRL_H_ */
