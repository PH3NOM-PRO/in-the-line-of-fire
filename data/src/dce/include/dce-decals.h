/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#ifndef DCE_DECALS_H
#define DCE_DECALS_H

#include <dc/vec3f.h>

#include "dce-player.h"

#define DCE_MAX_DECALS 64

typedef struct
{
    vec3f_t vertex[4];

    unsigned short int texID;
} DCE_Decal;

void DCE_RenderInsertBulletDecal(DCE_Player * player, unsigned short int texID);

void DCE_DecalsRender();

void DCE_DecalsReset();

void DCE_RenderBulletDecal(DCE_Player * player, unsigned short int texID);

void DCE_RenderBloodDecal(DCE_Player * player, unsigned short int texID);

void DCE_RenderBloodDecalPosition(vec3f_t * position, unsigned short int texID);

void DCE_RenderInsertDecal(DCE_Player * player, unsigned short int texID, float size);

void DCE_RenderInsertBloodDecal(vec3f_t * P, unsigned short int texID, float size);

#endif
