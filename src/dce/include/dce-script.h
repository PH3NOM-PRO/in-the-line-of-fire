/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#ifndef DCE_SCRIPT_H
#define DCE_SCRIPT_H

#include "stdio.h"  // FILE
#include "dce-player.h"

#define DCE_SCRIPT_IDENT_VEC3  0x01
#define DCE_SCRIPT_IDENT_MD2   0x02
#define DCE_SCRIPT_IDENT_TEXID 0x03

#define DCE_SCRIPT_KEY_POS     0x11
#define DCE_SCRIPT_KEY_LOOKAT  0x12
#define DCE_SCRIPT_KEY_SRC     0x13
#define DCE_SCRIPT_KEY_DST     0x14
#define DCE_SCRIPT_KEY_BMDL    0x15 // Body Model
#define DCE_SCRIPT_KEY_WMDL    0x16 // Weapon Model
#define DCE_SCRIPT_KEY_BTEX    0x17 // Body Texture
#define DCE_SCRIPT_KEY_WTEX    0x18 // Weapon Texture

#define DCE_SCRIPT_LINE_LEN    1023

unsigned char DCE_ScriptVerifyStart(char * buf);
unsigned char DCE_ScriptVerifyEnd(char * buf);

unsigned char DCE_ScriptEndEntry(char * buf);
unsigned char DCE_ScriptSeekNextEntry(FILE * f, char * buf);

unsigned short int DCE_ScriptNextVec3Offset(char * buf);
void DCE_ScriptReadVec3f(char * buf, float * x, float * y, float * z);

unsigned short int DCE_ScriptNextStringOffset(char * buf);
void DCE_ScriptReadString(char * buf, char * string);

unsigned char DCE_ScriptEntryParse(char * buf);

void DCE_ParsePlayerScript(char * fname, DCE_Player * p);

#endif
