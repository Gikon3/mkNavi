#ifndef FRAMEWORK_INC_FT_DL_CMDS_IMPL_H_
#define FRAMEWORK_INC_FT_DL_CMDS_IMPL_H_

typedef enum {
  ftFuncNever = 0,
  ftFuncLess = 1,
  ftFuncLEqual = 2,
  ftFuncGreater = 3,
  ftFuncGEqual = 4,
  ftFuncEqual = 5,
  ftFuncNotEqual = 6,
  ftFuncAlways = 7
} FT_Function;

typedef enum {
  ftPrimBitmaps = 1,
  ftPrimPoints = 2,
  ftPrimLines = 3,
  ftPrimLineStrip = 4,
  ftPrimEdgeStripR = 5,
  ftPrimEdgeStripL = 6,
  ftPrimEdgeStripA = 7,
  ftPrimEdgeStripB = 8,
  ftPrimRect = 9
} FT_Primitive;

typedef enum {
  ftFormatARGB1555 = 0,
  ftFormatL1 = 1,
  ftFormatL4 = 2,
  ftFormatL8 = 3,
  ftFormatRGB332 = 4,
  ftFormatARGB2 = 5,
  ftFormatARGB4 = 6,
  ftFormatRGB565 = 7,
  ftFormatText8X8 = 9,
  ftFormatTextVGA = 10,
  ftFormatBarGraph = 11,
  ftFormatPaletted565 = 14,
  ftFormatPaletted4444 = 15,
  ftFormatPaletted8 = 16,
  ftFormatL2 = 17
} FT_Format;

typedef enum {
  ftFilterNearest = 0,
  ftFilterBilinear = 1
} FT_Filter;

typedef enum {
  ftWrapBorder = 0,
  ftWrapRepeat = 1
} FT_Wrap;

typedef enum {
  ftBlendZero = 0,
  ftBlendOne = 1,
  ftBlendSrcAlpha = 2,
  ftBlendDstAlpha = 3,
  ftBlendOneMinusSrcAlpha = 4,
  ftBlendOneMinusDstAlpha = 5
} FT_Blend;

typedef enum {
  ftMacroReg0 = 0,
  ftMacroReg1 = 1
} FT_MacroReg;

typedef enum {
  ftActionZero = 0,
  ftActionKeep = 1,
  ftActionReplace = 2,
  ftActionIncr = 3,
  ftActionDecr = 4,
  ftActionInvert = 5
} FT_Action;

#endif /* FRAMEWORK_INC_FT_DL_CMDS_IMPL_H_ */
