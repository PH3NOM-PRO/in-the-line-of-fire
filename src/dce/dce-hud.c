/* 
** DCE - Dreamcast Engine (C) 2016 Josh PH3NOM Pearson
*/

#include "dce.h"

static unsigned short DCE_HUD_WEAPONS_TEXID = 0;

typedef struct
{
	DCE_HudIcon swat_icon;
	DCE_HudIcon * active_weapon;
	DCE_HudIcon * next_weapon;
	
	DCE_HudIcon deagle_active;
	DCE_HudIcon deagle_next;

	DCE_HudIcon glock_active;
	DCE_HudIcon glock_next;
	
} DCE_PlayerHUD;

static DCE_PlayerHUD DCE_PLAYER_HUD[3];

extern void DCE_RenderCompileAndSubmitTextureHeaderFilter(unsigned short index, unsigned char filter);

void DCE_HudRenderCallback(unsigned char index)
{
    DCE_RenderSetTexEnv(PVR_TXRENV_REPLACE);
    
	DCE_RenderCompileAndSubmitTextureHeader(DCE_PLAYER_HUD[index].swat_icon.texID);

	DCE_RenderTexturedQuad2D(DCE_PLAYER_HUD[index].swat_icon.x, DCE_PLAYER_HUD[index].swat_icon.y,
			                         DCE_PLAYER_HUD[index].swat_icon.x + DCE_PLAYER_HUD[index].swat_icon.w, DCE_PLAYER_HUD[index].swat_icon.y + DCE_PLAYER_HUD[index].swat_icon.h,
							         DCE_PLAYER_HUD[index].swat_icon.u1, DCE_PLAYER_HUD[index].swat_icon.v1, DCE_PLAYER_HUD[index].swat_icon.u2, DCE_PLAYER_HUD[index].swat_icon.v2,
									 DCE_PLAYER_HUD[index].swat_icon.argb);

    DCE_RenderCompileAndSubmitTextureHeaderFilter(DCE_PLAYER_HUD[index].active_weapon->texID, PVR_FILTER_NONE);

	DCE_RenderTexturedQuad2D(DCE_PLAYER_HUD[index].active_weapon->x, DCE_PLAYER_HUD[index].active_weapon->y,
			                 DCE_PLAYER_HUD[index].active_weapon->x + DCE_PLAYER_HUD[index].active_weapon->w, DCE_PLAYER_HUD[index].active_weapon->y + DCE_PLAYER_HUD[index].active_weapon->h,
							 DCE_PLAYER_HUD[index].active_weapon->u1, DCE_PLAYER_HUD[index].active_weapon->v1, DCE_PLAYER_HUD[index].active_weapon->u2, DCE_PLAYER_HUD[index].active_weapon->v2,
							 DCE_PLAYER_HUD[index].active_weapon->argb);

    DCE_RenderCompileAndSubmitTextureHeaderFilter(DCE_PLAYER_HUD[index].next_weapon->texID, PVR_FILTER_NONE);

	DCE_RenderTexturedQuad2D(DCE_PLAYER_HUD[index].next_weapon->x, DCE_PLAYER_HUD[index].next_weapon->y,
			                 DCE_PLAYER_HUD[index].next_weapon->x + DCE_PLAYER_HUD[index].next_weapon->w, DCE_PLAYER_HUD[index].next_weapon->y + DCE_PLAYER_HUD[index].next_weapon->h,
							 DCE_PLAYER_HUD[index].next_weapon->u1, DCE_PLAYER_HUD[index].next_weapon->v1, DCE_PLAYER_HUD[index].next_weapon->u2, DCE_PLAYER_HUD[index].next_weapon->v2,
							 DCE_PLAYER_HUD[index].next_weapon->argb);

    DCE_RenderSetTexEnv(PVR_TXRENV_MODULATEALPHA);
}

unsigned char DCE_HudLoadIcon(DCE_HudIcon * hud, float x, float y, float w, float h, float u1, float v1, float u2, float v2,
                     unsigned short texID, unsigned int argb, unsigned char active)
{
	hud->x = x;
	hud->y = y;
	hud->w = w;
	hud->h = h;
	hud->u1 = u1;
	hud->v1 = v1;
	hud->u2 = u2;
	hud->v2 = v2;
	hud->texID = texID;
	hud->active = active;
	hud->argb = argb;
	
	return 1;
}

static unsigned char DCE_HUD_INDEX = 1;

void DCE_HudSetIndex(unsigned char index)
{
	DCE_HUD_INDEX = index;
}

void DCE_HudReInit()
{
    DCE_PLAYER_HUD[DCE_HUD_INDEX].active_weapon = &DCE_PLAYER_HUD[DCE_HUD_INDEX].deagle_active;
    
    DCE_PLAYER_HUD[DCE_HUD_INDEX].next_weapon = &DCE_PLAYER_HUD[DCE_HUD_INDEX].glock_next;	
}

void DCE_HudSwitchWeapons()
{
    if(DCE_PLAYER_HUD[DCE_HUD_INDEX].active_weapon == &DCE_PLAYER_HUD[DCE_HUD_INDEX].deagle_active)
    {
        DCE_PLAYER_HUD[DCE_HUD_INDEX].active_weapon = &DCE_PLAYER_HUD[DCE_HUD_INDEX].glock_active;
    
        DCE_PLAYER_HUD[DCE_HUD_INDEX].next_weapon = &DCE_PLAYER_HUD[DCE_HUD_INDEX].deagle_next;
	}
	else
	{
        DCE_PLAYER_HUD[DCE_HUD_INDEX].active_weapon = &DCE_PLAYER_HUD[DCE_HUD_INDEX].deagle_active;
    
        DCE_PLAYER_HUD[DCE_HUD_INDEX].next_weapon = &DCE_PLAYER_HUD[DCE_HUD_INDEX].glock_next;		
	}
}

void DCE_HudLoad(char * fname)
{
    DCE_HUD_WEAPONS_TEXID = DCE_TextureLoadPVR("/cd/textures/gui/guns_icons_texture.pvr");
    
    uint16 face_icon = DCE_TextureLoadPVR("/cd/textures/gui/face_icon.pvr");

    uint16 face_icon2 = DCE_TextureLoadPVR("/cd/textures/gui/clown_icon.pvr");

	DCE_HudLoadIcon(&DCE_PLAYER_HUD[0].swat_icon, 16, 480 - 128, 96, 112, 0, 0, 1, 1,
	                face_icon, 0xFFFFFFFF, 1);
	                                                             
	DCE_HudLoadIcon(&DCE_PLAYER_HUD[0].deagle_active, 640 - 96 - 26, 480 - 128 - 8, 96, 118,
	                1.9/3.0f, 1.0/3.0f, 1, 2.0/3.0f,
	                DCE_HUD_WEAPONS_TEXID, 0xFFFFFFFF, 1);
    
	DCE_HudLoadIcon(&DCE_PLAYER_HUD[0].glock_active, 640 - 96 - 26 - 8, 480 - 128 - 8, 104, 127,
	                2.0/3.0f, 2.0/3.0f, 3.0/3.0f, 1.0f,
	                DCE_HUD_WEAPONS_TEXID, 0xFFFFFFFF, 0);

	DCE_HudLoadIcon(&DCE_PLAYER_HUD[0].deagle_next, 640 - 96 - 4 + 36, 480 - 128 + 48, 55, 68,
	                1.9/3.0f, 1.0/3.0f, 1, 2.0/3.0f,
	                DCE_HUD_WEAPONS_TEXID, 0xFFFFFFFF, 0);

	DCE_HudLoadIcon(&DCE_PLAYER_HUD[0].glock_next, 640 - 96 - 4 + 36, 480 - 128 + 48, 54, 70,
	               2.0/3.0f, 2.0/3.0f, 3.0/3.0f, 1.0f,
	                DCE_HUD_WEAPONS_TEXID, 0xFFFFFFFF, 1);

    DCE_PLAYER_HUD[0].active_weapon = &DCE_PLAYER_HUD[0].deagle_active;
    DCE_PLAYER_HUD[0].next_weapon = &DCE_PLAYER_HUD[0].glock_next;
	    
	float y = 224;
	float s = 0.5; 
        
    DCE_HudLoadIcon(&DCE_PLAYER_HUD[1].swat_icon, 16, y - (128 - 16) * s, 104 * s, 112 * s, 0, 0, 1, 1,
	                                    face_icon, 0xFFFFFFFF, 1);

	DCE_HudLoadIcon(&DCE_PLAYER_HUD[1].deagle_active, 640 - 96 - 36, y - (128 - 4) * s,
	                126 * s, 132 * s, 1.9/3.0f, 1.0/3.0f, 1, 2.0/3.0f, DCE_HUD_WEAPONS_TEXID, 0xFFFFFFFF, 1);

	DCE_HudLoadIcon(&DCE_PLAYER_HUD[1].glock_active, 640 - 96 - 30, y - (128 - 4) * s, 
	                128 * s, 140 * s, 2.0/3.0f, 2.0/3.0f, 3.0/3.0f, 1.0f, DCE_HUD_WEAPONS_TEXID, 0xFFFFFFFF, 0);

	DCE_HudLoadIcon(&DCE_PLAYER_HUD[1].deagle_next, 640 - 96 - 4 + 30, y - (128 - 0 - 52) * s,
	                48 * s, 64 * s, 1.9/3.0f, 1.0/3.0f, 1, 2.0/3.0f,  DCE_HUD_WEAPONS_TEXID, 0xFFFFFFFF, 0);

	DCE_HudLoadIcon(&DCE_PLAYER_HUD[1].glock_next, 640 - 96 - 8 + 32, y - (128 - 0 - 52) * s,
	                60 * s, 66 * s, 2.0/3.0f, 2.0/3.0f, 3.0/3.0f, 1.0f, DCE_HUD_WEAPONS_TEXID, 0xFFFFFFFF, 1);

    DCE_PLAYER_HUD[1].active_weapon = &DCE_PLAYER_HUD[1].deagle_active;
    DCE_PLAYER_HUD[1].next_weapon = &DCE_PLAYER_HUD[1].glock_next;

    y = 480;
	DCE_HudLoadIcon(&DCE_PLAYER_HUD[2].swat_icon, 16, y - (128 - 16) * s, 104 * s, 112 * s, 0, 0, 1, 1,
	                                    face_icon2, 0xFFFFFFFF, 1);
	                                                           
	DCE_HudLoadIcon(&DCE_PLAYER_HUD[2].deagle_active, 640 - 96 - 36, y - (128 - 4) * s,
	                126 * s, 132 * s, 1.9/3.0f, 1.0/3.0f, 1, 2.0/3.0f, DCE_HUD_WEAPONS_TEXID, 0xFFFFFFFF, 1);

	DCE_HudLoadIcon(&DCE_PLAYER_HUD[2].glock_active, 640 - 96 - 30, y - (128 - 4) * s, 
	                128 * s, 140 * s, 2.0/3.0f, 2.0/3.0f, 3.0/3.0f, 1.0f, DCE_HUD_WEAPONS_TEXID, 0xFFFFFFFF, 0);

	DCE_HudLoadIcon(&DCE_PLAYER_HUD[2].deagle_next, 640 - 96 - 4 + 30, y - (128 - 0 - 52) * s,
	                48 * s, 64 * s, 1.9/3.0f, 1.0/3.0f, 1, 2.0/3.0f,  DCE_HUD_WEAPONS_TEXID, 0xFFFFFFFF, 0);

	DCE_HudLoadIcon(&DCE_PLAYER_HUD[2].glock_next, 640 - 96 - 8 + 32, y - (128 - 0 - 52) * s,
	                60 * s, 66 * s, 2.0/3.0f, 2.0/3.0f, 3.0/3.0f, 1.0f, DCE_HUD_WEAPONS_TEXID, 0xFFFFFFFF, 1);

    DCE_PLAYER_HUD[2].active_weapon = &DCE_PLAYER_HUD[2].deagle_active;
    DCE_PLAYER_HUD[2].next_weapon = &DCE_PLAYER_HUD[2].glock_next;
}

