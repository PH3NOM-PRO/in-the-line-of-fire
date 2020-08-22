#ifndef INPUT_H
#define INPUT_H

#include "dce.h"


#define DCE_INPUT_EXIT    251
#define DCE_INPUT_FIRE    252
#define DCE_INPUT_EXECUTE 253


#define RANGLE 15.0f          // Rotation Angle
#define RSD 360.0f/RANGLE     // Rotated Side

int InputCallbackP2(DCE_Player * p);
int InputCallback2(DCE_Player * p);
int InputCallbackX();
int DCE_MenuInputCallback();

int DCE_InputCallback(DCE_Player * p, unsigned char index);

int DCE_InputCallbackCam(DCE_Player * p, DCE_Camera * c);

float DCE_TimeSliceMoveSpeed();

unsigned char DCE_InputCallbackMenu();

#endif
