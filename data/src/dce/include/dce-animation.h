/* 
** DCE - Dreamcast Engine (C) 2013-2015 Josh PH3NOM Pearson
*/

#ifndef DCE_ANIMATION_H
#define DCE_ANIMATION_H

#include "dce-player.h"

void DCE_SetWeaponModelFrameRange(DCE_Player * player, char * frame);

void DCE_SetModelFrameRange(DCE_Player * player, char * frame);

void DCE_SetWeaponModelFrame(DCE_Player * player);

void DCE_SetModelFrame(DCE_Player * player);

void DCE_UpdatePlayerEntity(DCE_Player * player);

void DCE_AnimationFrameAdvance();

unsigned int DCE_AnimationFrame();

void DCE_PlayerAnimationCallback(DCE_Player * player, unsigned short int decalID,
                                 DCE_Player * target, unsigned short int targets,
                                 unsigned short int bloodID);

void DCE_HudSetMuzzleFlash();

int DCE_GetModelFrameRangeMin(DCE_Player * player, char * frame);

int DCE_GetWeaponModelFrameRangeMin(DCE_Player * player, char * frame);

void DCE_PlayerAnimateModel(DCE_Player * player);

#endif
