/* 
** DCE - Dreamcast Engine (C) 2013-2015 Josh PH3NOM Pearson
*/

#include "dce.h"

static unsigned int CURRENT_FRAME = 0;

int DCE_GetWeaponModelFrameRangeMin(DCE_Player * player, char * frame)
{
    return 0;
}

int DCE_GetModelFrameRangeMin(DCE_Player * player, char * frame)
{
    return 0;
}

void DCE_SetWeaponModelFrameRange(DCE_Player * player, char * frame)
{

}

void DCE_SetModelFrameRange(DCE_Player * player, char * frame)
{
                                 
}

void DCE_SetWeaponModelFrame(DCE_Player * player)
{

}

void DCE_SetModelFrame(DCE_Player * player)
{

}

void DCE_AnimationFrameAdvance()
{

}

unsigned int DCE_AnimationFrame()
{
    return CURRENT_FRAME;
}

static unsigned short int MuzzleFlashIndex = 0;
static HUD_Sprite * sprite;
static uint16 muzTexID = 0;

void DCE_HudSetMuzzleFlash()
{
	sprite = malloc(sizeof(HUD_Sprite));
	//sprite->pos.x = 380; // v_dual
	sprite->pos.x = 360;
	sprite->pos.y = 320;
	sprite->pos.z = 1.0f;
	sprite->width = 128;
	sprite->height = 128;
	sprite->frame = 0;
	sprite->frames = 3;
	sprite->frame_rate = 30;
	
    sprite->uv = malloc(sizeof(HUD_TexCoord) * 3);
    sprite->uv[0].u1 = 0;
    sprite->uv[0].v1 = 0;
    sprite->uv[0].u2 = 1;
    sprite->uv[0].v2 = 1;	
    sprite->uv[1].u1 = 0;
    sprite->uv[1].v1 = 0;
    sprite->uv[1].u2 = 1;
    sprite->uv[1].v2 = 1;	
    sprite->uv[2].u1 = 0;
    sprite->uv[2].v1 = 0;
    sprite->uv[2].u2 = 1;
    sprite->uv[2].v2 = 1;	
        
	sprite->texID = malloc(sizeof(unsigned short) * 3);
	muzTexID = sprite->texID[0] = DCE_TextureLoadJPG("/cd/sprites/player/muzzleflash1_0.jpg");
	sprite->texID[1] = DCE_TextureLoadJPG("/cd/sprites/player/muzzleflash1_1.jpg");
	sprite->texID[2] = DCE_TextureLoadJPG("/cd/sprites/player/muzzleflash1_2.jpg");
}

void DCE_HudInsertPlayerMuzzleFlash(DCE_Player * player)
{
	MuzzleFlashIndex = DCE_HudInsertPlayerSprite(player, sprite);
	//printf("\nMuzFlashIndex: %i\n\n", MuzzleFlashIndex);
}

void DCE_HudRemovePlayerMuzzleFlash(DCE_Player * player)
{
	//printf("\nRemoveMuzFlashIndex: %i\n\n", MuzzleFlashIndex);     
	DCE_HudRemovePlayerSprite(player, MuzzleFlashIndex);	
}

void DCE_HudInsertMuzzleFlash()
{
	MuzzleFlashIndex = DCE_HudInsertSprite(sprite);
	//printf("\nMuzFlashIndex: %i\n\n", MuzzleFlashIndex);
}

void DCE_PlayerAnimationCallback(DCE_Player * player, unsigned short int decalID,
                                 DCE_Player * target, unsigned short int targets,
                                 unsigned short int bloodID)
{

}
