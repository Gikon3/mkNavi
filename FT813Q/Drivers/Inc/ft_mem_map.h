#ifndef DRIVERS_INC_FT_MEM_MAP_H_
#define DRIVERS_INC_FT_MEM_MAP_H_

#include "ft_config.h"

/* FTDI FT813Q map */

#define FT_RAM_G                    0x000000  /* to 0x0FFFFF */
#define FT_ROM_FONT                 0x1E0000  /* to 0x2FFFFB */
#define FT_ROM_FONT_ADDR            0x2FFFFC  /* to 0x2FFFFF */
#define FT_RAM_DL                   0x300000  /* to 0x301FFF */
#define FT_RAM_REG                  0x302000  /* to 0x302FFF */
#define FT_RAM_CMD                  0x308000  /* to 0x308FFF */

#define FT_RAM_CHIP_ID              0x0C0000

#define FT_REG_ID                   (FT_RAM_REG + 0x000)
#define FT_REG_FRAMES               (FT_RAM_REG + 0x004)
#define FT_REG_CLOCK                (FT_RAM_REG + 0x008)
#define FT_REG_FREQUENCY            (FT_RAM_REG + 0x00C)
#define FT_REG_RENDERMODE           (FT_RAM_REG + 0x010)
#define FT_REG_SNAPY                (FT_RAM_REG + 0x014)
#define FT_REG_SNAPSHOT             (FT_RAM_REG + 0x018)
#define FT_REG_SNAPFORMAT           (FT_RAM_REG + 0x01C)
#define FT_REG_CPURESET             (FT_RAM_REG + 0x020)
#define FT_REG_TAP_CRC              (FT_RAM_REG + 0x024)
#define FT_REG_TAP_MASK             (FT_RAM_REG + 0x028)
#define FT_REG_HCYCLE               (FT_RAM_REG + 0x02C)
#define FT_REG_HOFFSET              (FT_RAM_REG + 0x030)
#define FT_REG_HSIZE                (FT_RAM_REG + 0x034)
#define FT_REG_HSYNC0               (FT_RAM_REG + 0x038)
#define FT_REG_HSYNC1               (FT_RAM_REG + 0x03C)
#define FT_REG_VCYCLE               (FT_RAM_REG + 0x040)
#define FT_REG_VOFFSET              (FT_RAM_REG + 0x044)
#define FT_REG_VSIZE                (FT_RAM_REG + 0x048)
#define FT_REG_VSYNC0               (FT_RAM_REG + 0x04C)
#define FT_REG_VSYNC1               (FT_RAM_REG + 0x050)
#define FT_REG_DLSWAP               (FT_RAM_REG + 0x054)
#define FT_REG_ROTATE               (FT_RAM_REG + 0x058)
#define FT_REG_OUTBITS              (FT_RAM_REG + 0x05C)
#define FT_REG_DITHER               (FT_RAM_REG + 0x060)
#define FT_REG_SWIZZLE              (FT_RAM_REG + 0x064)
#define FT_REG_CSPREAD              (FT_RAM_REG + 0x068)
#define FT_REG_PCLK_POL             (FT_RAM_REG + 0x06C)
#define FT_REG_PCLK                 (FT_RAM_REG + 0x070)
#define FT_REG_TAG_X                (FT_RAM_REG + 0x074)
#define FT_REG_TAG_Y                (FT_RAM_REG + 0x078)
#define FT_REG_TAG                  (FT_RAM_REG + 0x07C)
#define FT_REG_VOL_PB               (FT_RAM_REG + 0x080)
#define FT_REG_VOL_SOUND            (FT_RAM_REG + 0x084)
#define FT_REG_SOUND                (FT_RAM_REG + 0x088)
#define FT_REG_PLAY                 (FT_RAM_REG + 0x08C)
#define FT_REG_GPIO_DIR             (FT_RAM_REG + 0x090)
#define FT_REG_GPIO                 (FT_RAM_REG + 0x094)
#define FT_REG_GPIOX_DIR            (FT_RAM_REG + 0x098)
#define FT_REG_GPIOX                (FT_RAM_REG + 0x09C)
#define FT_REG_INT_FLAGS            (FT_RAM_REG + 0x0A8)
#define FT_REG_INT_EN               (FT_RAM_REG + 0x0AC)
#define FT_REG_INT_MASK             (FT_RAM_REG + 0x0B0)
#define FT_REG_PLAYBACK_START       (FT_RAM_REG + 0x0B4)
#define FT_REG_PLAYBACK_LENGTH      (FT_RAM_REG + 0x0B8)
#define FT_REG_PLAYBACK_READPTR     (FT_RAM_REG + 0x0BC)
#define FT_REG_PLAYBACK_FREQ        (FT_RAM_REG + 0x0C0)
#define FT_REG_PLAYBACK_FORMAT      (FT_RAM_REG + 0x0C4)
#define FT_REG_PLAYBACK_LOOP        (FT_RAM_REG + 0x0C8)
#define FT_REG_PLAYBACK_PLAY        (FT_RAM_REG + 0x0CC)
#define FT_REG_PWM_HZ               (FT_RAM_REG + 0x0D0)
#define FT_REG_PWM_DUTY             (FT_RAM_REG + 0x0D4)
#define FT_REG_MACRO_0              (FT_RAM_REG + 0x0D8)
#define FT_REG_MACRO_1              (FT_RAM_REG + 0x0DC)
#define FT_REG_CMD_READ             (FT_RAM_REG + 0x0F8)
#define FT_REG_CMD_WRITE            (FT_RAM_REG + 0x0FC)
#define FT_REG_CMD_DL               (FT_RAM_REG + 0x100)
#if defined(FT810) || defined(FT812)
#define FT_REG_TOUCH_MODE           (FT_RAM_REG + 0x104)
#define FT_REG_TOUCH_ADC_MODE       (FT_RAM_REG + 0x108)
#define FT_REG_TOUCH_CHARGE         (FT_RAM_REG + 0x10C)
#define FT_REG_TOUCH_SETTLE         (FT_RAM_REG + 0x110)
#define FT_REG_TOUCH_OVERSAMPLE     (FT_RAM_REG + 0x114)
#define FT_REG_TOUCH_RZTHRESH       (FT_RAM_REG + 0x118)
#define FT_REG_TOUCH_RAW_XY         (FT_RAM_REG + 0x11C)
#define FT_REG_TOUCH_RZ             (FT_RAM_REG + 0x120)
#define FT_REG_TOUCH_SCREEN_XY      (FT_RAM_REG + 0x124)
#define FT_REG_TOUCH_TAG            (FT_RAM_REG + 0x128)
#endif
#if defined(FT811) || defined(FT813)
#define FT_REG_CTOUCH_MODE          (FT_RAM_REG + 0x104)
#define FT_REG_CTOUCH_EXTENDED      (FT_RAM_REG + 0x108)
#define FT_REG_CTOUCH_TOUCH1_XY     (FT_RAM_REG + 0x11C)
#define FT_REG_CTOUCH_TOUCH4_Y      (FT_RAM_REG + 0x120)
#define FT_REG_CTOUCH_TOUCH_XY      (FT_RAM_REG + 0x124)
#define FT_REG_CTOUCH_TAG_XY        (FT_RAM_REG + 0x128)
#define FT_REG_CTOUCH_TAG           (FT_RAM_REG + 0x12C)
#define FT_REG_CTOUCH_TAG1_XY       (FT_RAM_REG + 0x130)
#define FT_REG_CTOUCH_TAG1          (FT_RAM_REG + 0x134)
#define FT_REG_CTOUCH_TAG2_XY       (FT_RAM_REG + 0x138)
#define FT_REG_CTOUCH_TAG2          (FT_RAM_REG + 0x13C)
#define FT_REG_CTOUCH_TAG3_XY       (FT_RAM_REG + 0x140)
#define FT_REG_CTOUCH_TAG3          (FT_RAM_REG + 0x144)
#define FT_REG_CTOUCH_TAG4_XY       (FT_RAM_REG + 0x148)
#define FT_REG_CTOUCH_TAG4          (FT_RAM_REG + 0x14C)
#endif
#define FT_REG_TOUCH_TRANSFORM_A    (FT_RAM_REG + 0x150)
#define FT_REG_TOUCH_TRANSFORM_B    (FT_RAM_REG + 0x154)
#define FT_REG_TOUCH_TRANSFORM_C    (FT_RAM_REG + 0x158)
#define FT_REG_TOUCH_TRANSFORM_D    (FT_RAM_REG + 0x15C)
#define FT_REG_TOUCH_TRANSFORM_E    (FT_RAM_REG + 0x160)
#define FT_REG_TOUCH_TRANSFORM_F    (FT_RAM_REG + 0x164)
#define FT_REG_TOUCH_CONFIG         (FT_RAM_REG + 0x168)
#if defined(FT811) || defined(FT813)
#define FT_REG_CTOUCH_TOUCH4_X      (FT_RAM_REG + 0x16C)
#endif
#define FT_REG_BIST_EN              (FT_RAM_REG + 0x174)
#define FT_REG_TRIM                 (FT_RAM_REG + 0x180)
#define FT_REG_ANA_COMP             (FT_RAM_REG + 0x184)
#define FT_REG_SPI_WIDTH            (FT_RAM_REG + 0x188)
#if defined(FT811) || defined(FT813)
#define FT_REG_CTOUCH_TOUCH2_XY     (FT_RAM_REG + 0x18C)
#define FT_REG_CTOUCH_TOUCH3_XY     (FT_RAM_REG + 0x190)
#endif
#define FT_REG_DATESTAMP            (FT_RAM_REG + 0x564)
#define FT_REG_CMDB_SPACE           (FT_RAM_REG + 0x574)
#define FT_REG_CMDB_WRITE           (FT_RAM_REG + 0x578)

#define FT_SPREG_TRACKER            (FT_RAM_REG + 0x7000)
#define FT_SPREG_TRACKER_1          (FT_RAM_REG + 0x7004)
#define FT_SPREG_TRACKER_2          (FT_RAM_REG + 0x7008)
#define FT_SPREG_TRACKER_3          (FT_RAM_REG + 0x700C)
#define FT_SPREG_TRACKER_4          (FT_RAM_REG + 0x7010)
#define FT_SPREG_MEDIAFIFO_READ     (FT_RAM_REG + 0x7014)
#define FT_SPREG_MEDIAFIFO_WRITE    (FT_RAM_REG + 0x7018)

#endif /* DRIVERS_INC_FT_MEM_MAP_H_ */
