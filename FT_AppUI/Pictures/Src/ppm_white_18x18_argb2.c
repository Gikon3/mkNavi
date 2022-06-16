#include "ppm_white_18x18_argb2.h"

static const uint8_t bin[] = {
    120, 156, 101, 144, 201, 13, 0, 33, 12, 3, 75, 75, 105, 46, 205, 165,
    121, 115, 160, 44, 9, 126, 32, 196, 140, 136, 193, 172, 66, 218, 138, 180,
    14, 32, 97, 43, 75, 162, 128, 41, 133, 48, 164, 228, 188, 165, 194, 151,
    4, 177, 205, 217, 165, 165, 102, 189, 9, 4, 226, 151, 146, 136, 241, 176,
    51, 206, 251, 19, 58, 64, 89, 151, 8, 193, 75, 147, 42, 223, 144, 127,
    17, 32, 164, 34, 202, 213, 85, 191, 73, 79, 62, 188, 55, 159, 61
};

const FT_Picture ppmWhite18x18ARGB2 = {
    .bin = bin,
    .size = sizeof(bin),
    .format = ftFormatARGB2,
    .stride = 18,
    .resolution.w = 18,
    .resolution.h = 18
};