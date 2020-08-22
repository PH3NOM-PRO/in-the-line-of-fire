/* 
** DCE - Dreamcast Engine (C) 2013-2015 Josh PH3NOM Pearson
*/

#include "dce.h"

#define SKYBOX_SIZE 5000.0f

#define DCE_RENDER_DEBUG_INFO 1

static void DCE_PlayerInit(DCE_Player * player)
{
    player->ammo = 60;
    player->clip = 30;
    player->clip_size = 30;
    player->bullet_damage = 20;
    
    player->health = 100;
	player->height = 50;
	
	player->fall_distance = 0.0f;
	
	player->state = PSTATE_IDLE;
	player->lstate = PSTATE_IDLE;
	player->state_frame = 0;
	player->last_moved = 0;
	player->light_on = 0;
    player->perspective = DCE_PERSPECTIVE_FIRST;	

    DCE_BoudingBoxSetPlayer(player);   
}

void DCE_InitPlayer(DCE_Player * player)
{    
                
    DCE_InitPlayerHudSprites(player);            
    
    DCE_PlayerInit(player);
}  

void DCE_ReInitPlayer(DCE_Player * player)
{
	DCE_PlayerInit(player);
}  

void DCE_SetPlayerLookAtMatrix(DCE_Player * player)
{

}

void DCE_PlayerComputeRenderMatrix(DCE_Player * player, unsigned char player_num)
{

}

void DCE_PlayerLoadRenderMatrix(unsigned char player_num)
{

}

void DCE_RenderPlayerOP(DCE_Player * player, unsigned char index)
{

}

void DCE_RenderPlayerPT(DCE_Player * player, unsigned char index)
{

}

void DCE_RenderHurtScreenEffect(unsigned char id, unsigned char frames)
{

}

void DCE_RenderPlayerTR(DCE_Player * player, unsigned char index)
{

}

static inline unsigned char DCE_PlayerSetModelFrame(DCE_Player * player, char * frame)
{

}

void DCE_PlayerUpdateModelFrame(DCE_Player * player)
{

}

void DCE_PlayerAnimateModelFrame(DCE_Player * player, char * frame)
{

}

void DCE_PlayerAnimateModel(DCE_Player * player)
{

}
