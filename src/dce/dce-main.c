/*
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#include "dce.h"

/******************************************************************************/

static DCE_Player DCE_PLAYER[2];

DCE_Player * DCE_PlayerPointer(unsigned char index)
{
	return &DCE_PLAYER[index];
}

static char str[64];

static uint8 HACK_ENABLED = 0;
static uint8 HACK_COMPLETE = 0;
static uint8 VAULT_CLEAR = 0;
static uint8 ROOF_CLEAR = 0;
static uint8 DCE_INSIDE_VEHICLE = 0;
static uint8 RUN_EXECUTE = 0;

static uint64 FPCBT = 0;
static uint64 DCE_RENDER_FRAME = 0;

static uint8 DCE_GAME_PAUSED = 0;
static uint8 DCE_GAME_MODE = 2;
static uint8 DCE_PLAYERS = 2;

static uint32 DCE_TIMER_START;
static uint32 DCE_TIMER_TIME;

static uint8 pc = 0;

/******************************************************************************/

void DCE_PrintString(char * str, float x, float y) {
	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, DCE_TexID(DCE_TEXID_FONT), PVR_TXRENV_REPLACE);

	FontPrintString(DCE_Font(), str, x, y, 16, 16);
}

/******************************************************************************/

unsigned char DCE_GameMode()
{
	return DCE_GAME_MODE;
}

void DCE_ResetLevelProgress()
{
    HACK_ENABLED = 0;
    HACK_COMPLETE = 0;
    VAULT_CLEAR = 0;
    ROOF_CLEAR = 0;
    DCE_INSIDE_VEHICLE = 0;
    RUN_EXECUTE = 0;
}

uint8 DCE_HackEnabled() {
	return HACK_ENABLED;
}

uint8 DCE_VaultIsClear() {
	return VAULT_CLEAR;
}

uint8 DCE_RoofIsClear() {
	return ROOF_CLEAR;
}


void DCE_VaultClear(DCE_Player * p) {
	float d;

	vec3f_distance(-178.8, -441, -672, p->position.x, p->position.y, p->position.z, d);

	if(d < 32.0f)
		VAULT_CLEAR = 1;
}

void DCE_RoofReached(DCE_Player * p) {
	float d;

	vec3f_distance(-459.2, -185.3, -1017.3, p->position.x, p->position.y, p->position.z, d);

	if(d < 32.0f)
		ROOF_CLEAR = 1;
}

/******************************************************************************/

void DCE_PrintLoadStatus(char * string) {
	DCE_RenderStart();

	DCE_PrintString(string, 16, 16);

	DCE_RenderFinish();

	DCE_RenderStart();

	DCE_PrintString(string, 16, 16);

	DCE_RenderFinish();

	thd_sleep(100);
}

void DCE_LoadingScreen(char * texname) {
	unsigned short int texID = DCE_TextureLoadPVR(texname);

	DCE_RenderStart();

	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, texID, PVR_TXRENV_REPLACE);

	DCE_RenderTexturedQuad2D(0, 40, 640, 480 - 40,
	                         0, 0, 1, 1, 0xFFFFFFFF);

	DCE_RenderSwitchTR(); //== TR Blend =================================//

	DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);

	DCE_RenderSetBlendDst(PVR_BLEND_DESTALPHA);

	DCE_PrintString("In The Line Of Fire", 16, 16);

	DCE_PrintString("Early Access Build Version A002", 368, 480 - 32);

	DCE_RenderFinish();

	DCE_RenderStart();

	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, texID, PVR_TXRENV_REPLACE);

	DCE_RenderTexturedQuad2D(0, 40, 640, 480 - 40,
	                         0, 0, 1, 1, 0xFFFFFFFF);

	DCE_RenderSwitchTR(); //== TR Blend =================================//

	DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);

	DCE_RenderSetBlendDst(PVR_BLEND_DESTALPHA);

	DCE_PrintString("In The Line Of Fire", 16, 16);

	DCE_PrintString("Early Access Build Version A002", 368, 480 - 32);

	DCE_RenderFinish();

	DCE_TextureUnBind(texID);
}

void DCE_RenderHurtScreen() {
	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, DCE_TexID(DCE_TEXID_SPLATTER), PVR_TXRENV_REPLACE);

	DCE_RenderTexturedQuad2D(0, 0, 640, 480, 0, 0, 1, 1, 0xFFB0B0B0);
}

//== MODELS =====================================================================//

void DCE_PlayerSetBBox(DCE_Player * player) {
	player->bbox_min.x = player->position.x - 13;
	player->bbox_min.y = player->position.y - player->height;
	player->bbox_min.z = player->position.z - 13;
	player->bbox_max.x = player->position.x + 13;
	player->bbox_max.y = player->position.y + 7 ;
	player->bbox_max.z = player->position.z + 13;

	player->hbbox_min.x = player->position.x - 6;
	player->hbbox_min.y = player->position.y + 7;
	player->hbbox_min.z = player->position.z - 6;
	player->hbbox_max.x = player->position.x + 4;
	player->hbbox_max.y = player->position.y + 15;
	player->hbbox_max.z = player->position.z + 6;
}


void DCE_PlayerTracePlayerRay(DCE_Player * player)
{
	uint8 i, c;
		
	DCE_Player * target;
	
	if(player == &DCE_PLAYER[0])
	    target = &DCE_PLAYER[1];
	else
	    target = &DCE_PLAYER[0];
    
    if(!target->health)
        return;
    
	c = DCE_PlayerTraceRayBBox(player, &target->bbox_min, &target->hbbox_max);
           
	c |= DCE_PlayerTraceRayBBox(player, &target->hbbox_min, &target->hbbox_max) << 1;

	if(c)
    {
        DCE_RenderInsertDecal(player, DCE_TexID(DCE_TEXID_BLOOD), 5.0f);

        target->health -= player->bullet_damage;
        
		if(c & (1<<1))
		{
            ++player->hud_health[0]; // Headshot Count
            
		    target->health -= player->bullet_damage; // Headshot = 2x damage 
		}
		        
        if(target->health <= 0)
        {
	        target->health = 0;
	        
			target->state |= PSTATE_DEAD;
			
			target->state_frame = 0;
			
			target->perspective = DCE_PERSPECTIVE_THIRD;
			
		    ++target->hud_health[2]; // Target Death Count
			
			++player->hud_health[3]; // Kill Count
		}
	}
}

void DCE_TracePlayerMelee(DCE_Player * player)
{
    unsigned char c, index = player == &DCE_PLAYER[0] ? 0 : 1; 
    
    if(DCE_PLAYER[!index].health == 0)
        return;
    
	if(!DCE_PlayerMeleeCollision(player, &DCE_PLAYER[!index]))
        return;
        
	c = DCE_PlayerTraceRayBBox(&DCE_PLAYER[index], &DCE_PLAYER[!index].bbox_min, &DCE_PLAYER[!index].bbox_max);
            
	c |= DCE_PlayerTraceRayBBox(&DCE_PLAYER[index], &DCE_PLAYER[!index].hbbox_min, &DCE_PLAYER[!index].hbbox_max) << 1;
            
	if(c)
    {
        DCE_SfxPlay(IMPACT_FLESH1);
				    
	    DCE_PLAYER[!index].state |= PSTATE_HURT;
				
		DCE_PLAYER[!index].health -= 15;
		
		if(c & (1<<1))
		{
		    ++player->hud_health[0]; // Headshot Count
		    
		    DCE_PLAYER[!index].health -= 5; // Headshot = extra damage
        }
        
		if(DCE_PLAYER[!index].health <= 0)
		{
			DCE_PLAYER[!index].health = 0;
			
			DCE_PLAYER[!index].state |= PSTATE_DEAD;
		    
			++DCE_PLAYER[!index].hud_health[2]; // Target Death Count
			
			++player->hud_health[3]; // Kill Count
		}
	}
}

uint8 DCE_PlayerCollisionCheck(DCE_Player * player, vec3f_t * F, vec3f_t * T, uint8 index)
{
    if(DCE_PlayerCollision(player, &DCE_PLAYER[!index]))
	{
	    if(DCE_PLAYER[!index].health > 0)
	    {
		    vec3f_copy(F, &player->position);
            vec3f_copy(T, &player->lookAt);
		}                
        else if(DCE_PLAYER[!index].ammo)
        {
        	DCE_PLAYER[index].ammo += DCE_PLAYER[!index].ammo;
        	
        	DCE_PLAYER[!index].ammo = 0;
       	
        	DCE_PLAYER[!index].clip = 0;
        	
        	DCE_PLAYER[!index].model.weapon_lod1 = NULL;
        	
        	DCE_SfxPlay(WEAPON_RELOAD);
		}
		return 1;
	}
	
	return 0;
}

void DCE_PlayerFrameCallback(DCE_Player * player, DCE_Camera * cam, uint8 index)
{
	DCE_SetCBOIndex(index);
	
    DCE_HudSetIndex(DCE_GAME_MODE == 1 ? 0 : index + 1);
    
	if(player->health < 100.0f) {
		if(player->health <= 0)
		    player->health = 0;
		else
		    player->health += 0.05f;
		if(player->health > 100)
			player->health = 100;
	}

	vec3f_t F, T, D, B;
	float distance;

	vec3f_copy(&player->position, &F);
	vec3f_copy(&player->lookAt, &T);

	DCE_PlayerSetBBox(player);

	if(DCE_GAME_MODE == 1) {		
		if(player->perspective != DCE_PERSPECTIVE_STATIC) {
			if(HACK_ENABLED)
				DCE_GAME_PAUSED = (player->health) ? DCE_InputCallback(player, index) : 0;
			else
				HACK_ENABLED = DCE_InputCallbackWaitHack(player);
		} else {
			switch(DCE_InputCallbackCam(player, cam)) {
				case DCE_INPUT_FIRE:
					DCE_EnemyTraceCameraRay(cam);
					break;

				case DCE_INPUT_EXECUTE:
					if(DCE_EnemyMarkedTargets()>1) {
						HACK_COMPLETE = RUN_EXECUTE = 1;
					}
					break;
			}
		}
	} else if(player->health > 0)
		DCE_GAME_PAUSED += DCE_InputCallback(player, index);

    if(DCE_GAME_MODE == 2)
    {
        if(player->health > 0)
            player->perspective = DCE_PERSPECTIVE_FIRST;
	    else
	        player->perspective = DCE_PERSPECTIVE_THIRD;
	}        
    
	DCE_PlayerAnimationStartFrame(player);
    
    if(DCE_GAME_MODE == 1)
    {		
	    if(DCE_EnemyPlayerCollisionCallback(player, &F, &T))
		{
			vec3f_copy(&F, &player->position);
			vec3f_copy(&T, &player->lookAt);
			goto done;
		}
	}
	else
	{
		if(DCE_PlayerCollisionCheck(player, &F, &T, index))
		{
			vec3f_copy(&F, &player->position);
			vec3f_copy(&T, &player->lookAt);
			goto done;
		}
	}

	float gd = DCE_INSIDE_VEHICLE ? 50.0f : 30.0f;

	/* Apply Gravity Collision For Player Against World Geometry */
	if(DCE_CheckGravityCollisionCBO((vec3f_t *)&player->position, &distance)) {
		if(distance < gd) {
			vec3f_copy(&F, &player->position);
			vec3f_copy(&T, &player->lookAt);
		} else {
			player->shadow_position.x = player->position.x;
			player->shadow_position.y = player->position.y - distance;
			player->shadow_position.z = player->position.z;

			player->position.y -= distance;
			player->lookAt.y -= distance;

			player->position.y += player->height;
			player->lookAt.y += player->height;
		}
	}

	vec3f_sub(&F, &player->position, &D);

	vec3f_normalize_c(&D);

	D.y = 0;

	F.y -= player->height * 0.5;

	float d;
	vec3f_distance(F.x, 0, F.z, player->position.x, 0, player->position.z, d);

	if(DCE_CheckCollisionCBO((vec3f_t *)&F, (vec3f_t *)&D, &distance)) {
		if(distance < d * 6) {
			vec3f_copy(&F, &player->position);
			vec3f_copy(&T, &player->lookAt);
		}
	}

	F.y += player->height * 0.5;

	if(fabs(F.y - player->position.y) > 15) {
		vec3f_copy(&F, &player->position);
		vec3f_copy(&T, &player->lookAt);
	}

done:
	DCE_SetCBOIndex(0);
	return;
}

void DCE_RenderMatrixComputeProjection(uint8 index) {
	if(DCE_GAME_MODE == 2) {
		unsigned short y = index == 0 ? 256 : 0;
		unsigned short h = index == 0 ? 224 : 256;

		DCE_RenderSetScissor(0, y, 640, h);

		DCE_MatrixViewport(0, y, 640, h);
		
		//DCE_MatrixPerspective(50.0f, 16 / 9.0f, 0.1f, 10000.0f);
	} else
	{
		DCE_MatrixViewport(0, 0, 640, 480);
		
		//DCE_MatrixPerspective(60.0f, 16 / 9.0f, 0.1f, 10000.0f);
    }
    
	DCE_MatrixComputeProjection();
}

void DCE_RenderCallbackOP(DCE_Player * player, DCE_Camera * cam, float t, uint8 index) {
	DCE_RenderMatrixComputeProjection(index);

	DCE_MatrixLoadProjection();

	DCE_MatrixComputePlayer(player, cam, 0);

	DCE_MatrixApplyPlayer(0);

	DCE_RenderCallbackRBO(DCE_RBO_OP, index);

	DCE_RenderDoors();

	DCE_RenderModelPlayer(player);

	if(DCE_GAME_MODE == 1)
		DCE_EnemyRenderCallback(player, t);
	else
		DCE_RenderPlayerLOD(player, &DCE_PLAYER[!index]);
}

void DCE_RenderCallbackPT(DCE_Player * player, DCE_Camera * cam, float t, uint8 index) {
	DCE_RenderMatrixComputeProjection(index);

	DCE_MatrixLoadProjection();

	DCE_MatrixComputePlayer(player, cam, 0);

	DCE_MatrixApplyPlayer(0);

	DCE_RenderCallbackRBO(DCE_RBO_PT, index);
	
	if(player->perspective != DCE_PERSPECTIVE_STATIC)
        DCE_HudRenderCallback(DCE_GAME_MODE == 1 ? 0 : index + 1);  
}

void DCE_RenderMissionState(DCE_Player * player)
{
	DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);

	DCE_RenderSetBlendDst(PVR_BLEND_DESTALPHA);
	
	if(!HACK_ENABLED) {
		DCE_RenderCompileAndSubmitHeader(NULL);
		DCE_RenderTexturedQuad2D(16, 16, 368, 32, 0, 0, 0, 0, 0xD0505050);
		DCE_RenderTexturedQuad2D(16, 40, 392, 56, 0, 0, 0, 0, 0xD0505050);

		DCE_PrintString("MISSION UPDATE: HACK CAMERA TO MARK TARGETS\n", 16, 16);
		DCE_PrintString("TIP: Hold Left Trigger And Press Left on D-Pad", 16, 40);
	} else if(!HACK_COMPLETE) {
		DCE_RenderCompileAndSubmitHeader(NULL);
		DCE_RenderTexturedQuad2D(16, 16, 24 + strlen("MISSION UPDATE: USE CAMERA TO MARK TARGETS") * 8,
		                         32, 0, 0, 0, 0, 0xD0505050);
		DCE_RenderTexturedQuad2D(16, 40, 24 + strlen("TIP: Use Joypad to Rotate Camera Towards an Enemy") * 8,
		                         56, 0, 0, 0, 0, 0xD0505050);
		DCE_RenderTexturedQuad2D(16, 64, 24 + strlen("TIP: Pull Right Trigger to Mark Enemy") * 8,
		                         80, 0, 0, 0, 0, 0xD0505050);
		DCE_RenderTexturedQuad2D(16, 88, 24 + strlen("TIP: Press Start to Execute Marked Enemy Targets") * 8,
		                         104, 0, 0, 0, 0, 0xD0505050);

		DCE_PrintString("MISSION UPDATE: USE CAMERA TO MARK TARGETS\n", 16, 16);
		DCE_PrintString("TIP: Use Joypad to Rotate Camera Towards an Enemy\n", 16, 40);
		DCE_PrintString("TIP: Pull Right Trigger to Mark Enemy\n", 16, 64);
		DCE_PrintString("TIP: Press Start to Execute Marked Enemy Targets\n", 16, 88);
	} else if(!VAULT_CLEAR) {
		DCE_RenderCompileAndSubmitHeader(NULL);
		DCE_RenderTexturedQuad2D(16, 16, 24 + strlen("MISSION UPDATE: GET TO VAULT TO STOP ROBBERY") * 8,
		                         32, 0, 0, 0, 0, 0xD0505050);

		DCE_PrintString("MISSION UPDATE: GET TO VAULT TO STOP ROBBERY\n", 16, 16);

		DCE_VaultClear(player);
	} else if(!ROOF_CLEAR) {
		DCE_RenderCompileAndSubmitHeader(NULL);
		DCE_RenderTexturedQuad2D(16, 16, 24 + strlen("MISSION UPDATE: BOMBS PLANTED IN VAULT! GET TO ROOF TO STOP ROBBERY") * 8,
		                         32, 0, 0, 0, 0, 0xD0505050);

		DCE_PrintString("MISSION UPDATE: BOMBS PLANTED IN VAULT! GET TO ROOF TO STOP ROBBERY\n", 16, 16);

		DCE_RoofReached(player);

		//DCE_VaultClear(player);
	}
}

void DCE_RenderCallbackTR(DCE_Player * player, DCE_Camera * cam, float t, uint8 index) {
	DCE_RenderMatrixComputeProjection(index);

	DCE_MatrixLoadProjection();

	DCE_MatrixComputePlayer(player, cam, 0);

	DCE_MatrixApplyPlayer(0);

	DCE_RenderSetBlendSrc(PVR_BLEND_INVSRCALPHA);

	DCE_RenderSetBlendDst(PVR_BLEND_DESTCOLOR);

//    if(1/*player->perspective == DCE_PERSPECTIVE_STATIC*/)
//    {
//	    DCE_RBOSetVertexColor(0xFFD0D0D0);
	/*
			DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, DCE_TexID(DCE_TEXID_LIGHT1), PVR_TXRENV_REPLACE);

		    DCE_RenderTexturedQuad2D(0, 0, 640, 480, 0.1, 0.1, 0.9, 0.9, 0xFF00FF00);
	*/
//	}
//	else
//	{
//		DCE_RBOSetVertexColor(0xFF151515);
//	}

	DCE_RenderCallbackRBO(DCE_RBO_TR, index);

	if(DCE_GAME_MODE == 1)
		DCE_RenderCallbackRBOShadows(0);

	DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);

	DCE_RenderSetBlendDst(PVR_BLEND_DESTALPHA);

	DCE_RenderModelPlayerTR(player);

	if(pc)
		DCE_RenderHurtScreen();

	if(player->perspective != DCE_PERSPECTIVE_THIRD)
	{
		if(DCE_GAME_MODE == 1)
		{
            sprintf(str, "O\n");
		    DCE_PrintString(str, 312, 232);
		    
		    sprintf(str, "%i / %i\n", player->clip, player->ammo);					
				    
		    DCE_RenderCompileAndSubmitHeader(NULL);
		    DCE_RenderTexturedQuad2D(500, 480-32, 500 + strlen(str) * 8, 480-16, 0, 0, 0, 0, 0xD0505050);
		       
		    DCE_PrintString(str, 500, 480 - 32);
		}
		else
		{
			float x = 572,  y;
			
			if(index == 0)
			{
				y = 224 - 56;
				
                sprintf(str, "O\n");		
				DCE_PrintString(str, 312, 104);				
			}
			else
			{
				y = 480 - 56;
				
                sprintf(str, "O\n");		
				DCE_PrintString(str, 312, 344);				
			}

	        sprintf(str, "%i / %i\n", player->clip, player->ammo);

	        DCE_RenderCompileAndSubmitHeader(NULL);
	        DCE_RenderTexturedQuad2D(x, y, x + strlen(str) * 8, y + 16, 0, 0, 0, 0, 0xD0505050);
		    	    
	        DCE_PrintString(str, x, y);		
		}
	}

	if(player->health < 100)
	{
		uint8 alpha = 255 - ((player->health * 255) / 100);

		DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, DCE_TexID(DCE_TEXID_SPLATTER), PVR_TXRENV_REPLACE);

		DCE_RenderTexturedQuad2D(0, 0, 640, 480, 0.1, 0.1, 0.9, 0.9, alpha << 24 | 0xFFFFFF);
	}

	DCE_MatrixLoadProjection();

	DCE_MatrixApplyPlayer(index);

	DCE_DecalsRender();

	DCE_RenderSetBlendSrc(PVR_BLEND_INVSRCALPHA);

	DCE_RenderSetBlendDst(PVR_BLEND_DESTCOLOR);

	if(player->perspective != DCE_PERSPECTIVE_FIRST)
		DCE_RenderCheapShadow(DCE_TexID(DCE_TEXID_SHADOW1), &player->shadow_position, 20.0f);
    
	if(DCE_GAME_MODE == 2)
    {
        uint32 r = (DCE_TIMER_START + DCE_TIMER_TIME) - DCE_GetTime();
        uint8 min = r / 60000;
        float y;
    
        r -= min * 60000;
    
        uint8 sec = r / 1000;

        if(player == &DCE_PLAYER[0])
		    y = 16;
  	    else
  	        y = 240;
    

		if(sec > 9)
		    sprintf(str, "%i:%i\n", min, sec); 
        else
            sprintf(str, "%i:0%i\n", min, sec); 

    	DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);

    	DCE_RenderSetBlendDst(PVR_BLEND_DESTALPHA);
        
        DCE_RenderCompileAndSubmitHeader(NULL);
        DCE_RenderTexturedQuad2D(320 - (strlen(str) * 4), y, 320 + strlen(str) * 4, y + 16, 0, 0, 0, 0, 0xD0505050);
		DCE_PrintString(str, 320 - (strlen(str) * 4), y);        
		
		sprintf(str, "%.0f\n", player->health);
		 
		DCE_RenderCompileAndSubmitHeader(NULL);
		DCE_RenderTexturedQuad2D(32, y, 32 + strlen(str) * 8, y + 16, 0, 0, 0, 0, 0xD0505050);
        DCE_PrintString(str, 32, y);

		sprintf(str, "KILLS: %i\n", DCE_PLAYER[index].hud_health[3]);
		 
		DCE_RenderCompileAndSubmitHeader(NULL);
		DCE_RenderTexturedQuad2D(512, y, 512 + strlen(str) * 8, y + 16, 0, 0, 0, 0, 0xD0505050);
        DCE_PrintString(str, 512, y);
  	}
    
    //== Enemy TR Render Callback ==//
	DCE_MatrixLoadProjection();

	DCE_MatrixApplyPlayer(index);

    if(DCE_GAME_MODE == 1)
	    DCE_EnemyRenderCallbackShadow(player);

	DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);

	DCE_RenderSetBlendDst(PVR_BLEND_DESTALPHA);
	
    if(DCE_GAME_MODE == 1)
	    DCE_EnemyRenderCallbackTR(player, cam);
//	else
//	    DCE_RenderEnemyLODTR(DCE_PlayerPointer(index), DCE_PlayerPointer(!index));
	/*
	    sprintf(str, "%.1f %.1f %.1f\n", player->position.x, player->position.y, player->position.z);
	    DCE_PrintString(str, 16, 48);

	    sprintf(str, "FPS:%.2f\n", (double)DCE_RenderGetFPS());
	    DCE_PrintString(str, 552, 16);
	 */

    //== HUD ==//
    DCE_RenderSetBlendSrc(PVR_BLEND_INVSRCALPHA);
 
    DCE_RenderSetBlendDst(PVR_BLEND_DESTCOLOR);	
/*	    
	if(player->perspective != DCE_PERSPECTIVE_STATIC)
        DCE_HudRenderCallback(DCE_GAME_MODE == 1 ? 0 : index + 1);  
*/
    //== FPS Mission State ==//
	if(DCE_GAME_MODE == 1)
		DCE_RenderMissionState(player);

	DCE_PlayerAnimationEndFrame(player, DCE_RENDER_FRAME);
 
    
    /* Collision ********************************************************/

	DCE_TriggerRenderOpenDoor(&player->position);

	DCE_TriggerCallback(&player->position, index);

}

/* Render Callback For CAMERA */
void DCE_RenderCallback(DCE_Player * player, DCE_Camera * cam, float t) {
	DCE_RenderStart();

	DCE_RenderCallbackOP(player, cam, t, 0);

	DCE_RenderSwitchPT(); //== PT Blend =================================//

	DCE_RenderCallbackPT(player, cam, t, 0);

	DCE_RenderSwitchTR(); //== TR Blend =================================//

	DCE_RenderCallbackTR(player, cam, t, 0);

	DCE_RenderFinish();
}

int DCE_RunRBO()
{
	uint8 i;
	DCE_Camera cam;

	cam.pos.x = -10;
	cam.pos.y = -124.58;
	cam.pos.z = -113.88;

	cam.lookAt.x   = -491.5;
	cam.lookAt.y   = -315.76;
	cam.lookAt.z   = -158.2;

	cam.vantage_pos.x = -381.5;
	cam.vantage_pos.y = -315.76;
	cam.vantage_pos.z = -158.2;

	cam.rx = 0;
	cam.ry = 0;
	cam.mx = 1.3f;
	cam.my = 200.0f;

	//DCE_RBOSetVertexColor(0xFF151515);

	DCE_SetVolumeCDDA(11);

	DCE_PlayCDDA(2);

	DCE_RenderEnableScissor(DCE_GAME_MODE - 1);

    if(DCE_GAME_MODE == 2)
    {
    	DCE_TriggerOpenDOOR();
    	
		DCE_ReSpawn(&DCE_PLAYER[0], 0);
        DCE_ReSpawn(&DCE_PLAYER[1], 1);
		
		DCE_HudSetIndex(1);
		DCE_HudReInit();
		DCE_HudSetIndex(2);
		DCE_HudReInit();		
		
		vec3f_t * a = &DCE_PLAYER[0].position;
		vec3f_t * b = &DCE_PLAYER[1].position;
		float d;
		
		vec3f_distance(a->x, a->y, a->z, b->x, b->y, b->z, d);
		
		while(d < 500)
		{
		   DCE_ReSpawn(&DCE_PLAYER[1], 1);
		   vec3f_distance(a->x, a->y, a->z, b->x, b->y, b->z, d);
		}
	}
	else
	{
		DCE_LoadEnemies("/cd/level_01/level_01_enemy.txt");
		
		DCE_HudSetIndex(0);
		DCE_HudReInit();

		DCE_TriggerResetDOOR();
		DCE_InitPlayerState(&DCE_PLAYER[0]);
		DCE_TriggerLobbyA(&DCE_PLAYER[0], 0);
	}

	while(1)
	{
		/* Render Pause Menu At Players Request */
		if(DCE_GAME_PAUSED)
		{
            unsigned char r = 0, mode = 1;
			float s = 1.0f;
            
			DCE_RenderEnableScissor(0);
			 	
			while(!r)
			{
				DCE_MenuRenderPauseMenu(DCE_MenuInputCallback(), &s, &r);

			    s = DCE_StrobePulse(s, 0.85f, 1.15f, 0.01f, &mode);
            }
            
            DCE_GAME_PAUSED = 0;
            
            DCE_RenderEnableScissor(DCE_GAME_MODE - 1);
            
            if(r == 2)
			    return;				
		}
		
		if(DCE_GAME_MODE == 1) //== Enemy Routine Callback ==================//
			DCE_EnemyRoutineCallback(&DCE_PLAYER[0], 1.0f);

		for(i = 0; i < DCE_PLAYERS; i++) //== Player Collision Callback =====//
			DCE_PlayerFrameCallback(&DCE_PLAYER[i], &cam, i);

		DCE_RenderStart(); //== Begin Render ================================//

		for(i = 0; i < DCE_PLAYERS; i++)
			DCE_RenderCallbackOP(&DCE_PLAYER[i], &cam, 1.0f, i);
        
        if(DCE_GAME_MODE == 2)
        {
        	DCE_RenderEnableScissor(0);
        	
        	DCE_RenderCompileAndSubmitHeader(NULL);

	        DCE_RenderTexturedQuad2D(0, 224, 640, 228,
	                         0, 0, 1, 1, 0x0000000F);
        	
        	DCE_RenderEnableScissor(1);
		}
        
		DCE_RenderSwitchPT(); //== PT Blend =================================//

		for(i = 0; i < DCE_PLAYERS; i++)
			DCE_RenderCallbackPT(&DCE_PLAYER[i], &cam, 1.0f, i);

		DCE_RenderSwitchTR(); //== TR Blend =================================//

		for(i = 0; i < DCE_PLAYERS; i++)
			DCE_RenderCallbackTR(&DCE_PLAYER[i], &cam, 1.0f, i);

		DCE_RenderFinish(); //== Finsih Render ==============================//

		++DCE_RENDER_FRAME;

		if(DCE_GAME_MODE == 1 && RUN_EXECUTE)
		{
			DCE_RunExecutionSequence(&DCE_PLAYER[0], &cam);

			DCE_PLAYER[0].perspective = DCE_PERSPECTIVE_FIRST;

			RUN_EXECUTE = 0;
		}
		else if(DCE_GAME_MODE == 1)
		{
			if(DCE_PLAYER[0].health <= 0)
			{
				if(DCE_PLAYER[0].state_frame >= 240)
				    return - 1;
				else			
			        DCE_PLAYER[0].perspective = DCE_PERSPECTIVE_THIRD;
			}
		}
		
		if(DCE_GAME_MODE == 2)
		{
			uint8 i;
			for(i = 0; i < DCE_PLAYERS; i++)
			if(DCE_PLAYER[i].health == 0 && DCE_PLAYER[i].state_frame >= 150)
			{
			    vec3f_t * a = &DCE_PLAYER[i].position;
			    vec3f_t * b = &DCE_PLAYER[!i].position;
			    float d;
			    vec3f_distance(a->x, a->y, a->z, b->x, b->y, b->z, d);
				while(d < 500)
				{
				    DCE_ReSpawn(&DCE_PLAYER[i], i);
				    vec3f_distance(a->x, a->y, a->z, b->x, b->y, b->z, d);
				}
				
				DCE_HudSetIndex(i + 1);
		        DCE_HudReInit();
		    }
			
			if(DCE_GetTime() >= DCE_TIMER_START + DCE_TIMER_TIME)
			    return 0;
		}
	}
}

#include "LibMPEG2-Play.h"
#include "LibMPG123.h"
#include "StreamBuffer.h"
#include "StreamRender.h"

void LibVDC_PlayMPEG( char * fileName )
{
    int width, height;

    if( LibMPEG2_InitFileStream( fileName, &width, &height ) )
        printf( "MPEG File Loaded OK...\nLibXVID Initialized...\n" );
    else
    {
        printf( "LibMPEG: Error loading file\n" );
        return;
    }
    
    StreamBuffer * sbuf = StreamBuffer_NewObject( width, height );
    
    StreamRender_Initialize( sbuf );

    StreamRender_Execute( sbuf, LibMPEG2_DecodeNextFrame, LibMPG123_Start );
    
    StreamBuffer_FreeObject( sbuf );    
}

extern void DCE_StreamSFX();

int main(int argc, char **argv)
{
	DCE_Init();
		
	LibVDC_PlayMPEG("/cd/video/intro.mpg");

	DCE_LoadingScreen("/cd/textures/bgnd_01.pvr");

	DCE_PlayerHandsModelComputeMatrix(0, -62, 4, 90.0f, 0, 180.0f, 1.0f);

	DCE_LoadRBOBatch("level_01.txt");

	DCE_LoadEnemyModels();

    DCE_LoadPlayerModels();
    
	DCE_Player * player1 = &DCE_PLAYER[0];
	DCE_Player * player2 = &DCE_PLAYER[1];

	DCE_PlayerModelLoadSwat(&player1->model,
	                        "/cd/models/swat/swat.txt",
	                        "/cd/models/weapons/swat_hands.txt");

	DCE_PlayerModelLoadClown(&player2->model,
	                        "/cd/models/swat/swat.txt",
	                        "/cd/models/weapons/swat_hands.txt");

    DCE_MenuLoadTextures();
    
    DCE_HudLoad("/cd/hud/hud.txt");
    
    DCE_StreamSFX();
    
    DCE_PlayCDDA(3);
    DCE_SetVolumeCDDA(15);  
    
    DCE_WaitStartScreen();
    
/*    
    vec3f_t P = { -178.8, -441, -672 };
    
    DCE_SfxStreamLoad("NULL", 1, "/cd/sound/bombtick/bombtick.wav", &P, 500, 1, 900);
                       
    DCE_SfxStartStreamThread();
 */   
    while(1)
    {
        DCE_GAME_MODE = DCE_PLAYERS = DCE_MenuRoot();
        
        if(DCE_GAME_MODE == 1)
            DCE_ResetLevelProgress();        
        
	    DCE_DecalsReset();  // Reset Decals At Start Of Each Round
	    
	    DCE_TIMER_TIME = 60 * 1000 * 5; // 5 Minutes
	    DCE_TIMER_START = DCE_GetTime();
	    
	    DCE_PLAYER[0].hud_health[0] = DCE_PLAYER[0].hud_health[1] = DCE_PLAYER[0].hud_health[2] = DCE_PLAYER[0].hud_health[3] = 0;
        DCE_PLAYER[1].hud_health[0] = DCE_PLAYER[1].hud_health[1] = DCE_PLAYER[1].hud_health[2] = DCE_PLAYER[1].hud_health[3] = 0;

    	DCE_RunRBO();
    	
    	DCE_PlayCDDA(3);
    	DCE_SetVolumeCDDA(15);
    	
    	DCE_RenderEnableScissor(0);

	    if(DCE_GAME_MODE == 2)
	    	DCE_MenuRenderStats();
	    else if(DCE_PLAYER[0].health <= 0)
	    {
	    	//Mission Failed
	    	DCE_MenuGameOver();
		}
	}

	return 0;
}
