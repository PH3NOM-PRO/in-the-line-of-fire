#ifndef DCE_GFX_H
#define DCE_GFX_H

#include "font.h"

#define DCE_TEXID_FONT       0
#define DCE_TEXID_LIGHT1     2
#define DCE_TEXID_LIGHT2     3
#define DCE_TEXID_BULLET     7
#define DCE_TEXID_BLOOD      9

#define DCE_TEXID_SHADOW1    10

#define DCE_TEXID_SPLATTER  12
 
#define DCE_TEXID_MUZFLASH1 13
#define DCE_TEXID_MUZFLASH2 14
#define DCE_TEXID_MUZFLASH3 15
#define DCE_TEXID_MUZFLASH4 16

#define DCE_TEXID_WEAPONHUD 17

#define DCE_BLOOD_MODEL     18

#define DCE_TEXID_HKUSP     20
#define DCE_TEXID_GLOCK     21
#define DCE_TEXID_DEAGLE    22

unsigned short int DCE_TexID(unsigned short GFX);

Font * DCE_InitGFX();

Font * DCE_Font();

#endif
