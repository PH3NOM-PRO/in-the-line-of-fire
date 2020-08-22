//=======================================================================================//
// RGB.H (C) 2014 Josh Pearson
//=======================================================================================//

#ifndef RGB_H
#define RGB_H

#define RGB16_RED_MASK    0xF800
#define RGB16_GREEN_MASK  0x7E0
#define RGB16_BLUE_MASK   0x1F

#define RGB16_RED_SHIFT   0xB
#define RGB16_GREEN_SHIFT 0x5

#define ARGB32_ALPHA_MASK 0xFF000000
#define ARGB32_RGB_MASK   0xFFFFFF
#define ARGB32_RED_SHIFT  0x8

#define RGBA32_APLHA_MASK 0xFF
#define RGBA32_RGB_MASK   0xFFFFFF00

#define RGB5_MAX          0x1F
#define RGB6_MAX          0x3F
#define RGB8_MAX          0xFF

#define RGBA32_2_ARGB32(n) ((n & ARGB32_RGB_MASK) << ARGB32_RED_SHIFT | n & ARGB32_ALPHA_MASK)

unsigned short RGB24_2_RGB16(unsigned char *in);

unsigned int ARGB32_ADD( unsigned int in, unsigned char o);

#endif