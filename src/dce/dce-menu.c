/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#include "dce.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static DCE_MenuOption DCE_Options;

static float DCE_DisplayModes[] = { 4.0f, 3.0f, 16.0f, 9.0f, 16.0f, 10.0f };

static unsigned char DCE_MenuBgndColor        = 0xFF; // 8 Bit Color Component
static unsigned int  DCE_MenuColor            = 0xFFFFFFFF; // 32 Bit ARGB Color
static unsigned char DCE_MenuBgndColorMode    = 0; // Increment or Decrement
static unsigned char DCE_MenuLayre            = 0; // Outter Index
static unsigned char DCE_MenuIndex[8]         = { 0, 0, 0, 0, 0, 0, 0, 0 }; // Inner Index

static float DCE_INV_MAX_VOL = 1.0f / 255.0f; // Pre-Computed 1 / MAX_VOL to avoid run-time division

static float DCE_VolumeToPercent(unsigned char vol)
{
    return vol * 100 * DCE_INV_MAX_VOL;
}

unsigned char DCE_MusicVolume()
{
    return DCE_Options.audio.music_volume;
}

unsigned char DCE_SfxVolume()
{
    return DCE_Options.audio.sfx_volume;
}

unsigned char DCE_RenderUseLightMaps()
{
    return DCE_Options.video.enable_light_maps;
}

unsigned char DCE_RenderUseFog()
{
    return DCE_Options.video.enable_fog;
}

unsigned char DCE_RenderShadowMapQuality()
{
    return DCE_Options.video.shadow_quality;
}

float DCE_VideoDisplayAspectRatio()
{
    return (float)DCE_DisplayModes[(DCE_Options.video.aspect_ratio) * 2] /
                  DCE_DisplayModes[((DCE_Options.video.aspect_ratio) * 2) + 1];
}

void DCE_MenuInitOptions(char * config_file)
{
    DCE_Options.video.enable_light_maps = 0;
    DCE_Options.video.shadow_quality = 2;
    DCE_Options.video.aspect_ratio = 0;
    DCE_Options.video.enable_fog = 0;
        
    DCE_Options.audio.music_volume = DCE_MAX_VOLUME;
    DCE_Options.audio.sfx_volume = DCE_MAX_VOLUME;
    
    DCE_Options.controls.invert_y_axis = 0;     
}


// MENU SHIT ///

static void DCE_PrintStringS(char * str, float x, float y, float s)
{
	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, DCE_TexID(DCE_TEXID_FONT), PVR_TXRENV_REPLACE);
	
	FontPrintString(DCE_Font(), str, x, y, s, s);	
}

extern void DCE_PrintString(char * str, float x, float y);

static  unsigned short menu_texID[8];

#define MENU_LOGO 0
#define MENU_NG   1
#define MENU_SP   2
#define MENU_MP   3
#define MENU_CONT 4
#define MENU_EXIT 5

float DCE_StrobePulse(float v, float min, float max, float step, unsigned char * mode)
{
	if(*mode)
	{
		v += step;
		if(v >= max)
		{
		    *mode = 0;
		    v = max;
		}
	}
	else
	{
		v -= step;
		if(v < min)
		{
			v = min;
			*mode = 1;
		}
	}
	
	return v;
}

typedef void *(*MenuCallback)(unsigned char input, float *s, unsigned char *retval);

void * DCE_MenuRenderRoot(unsigned char input, float *s, unsigned char *retval);
void * DCE_MenuRenderNewGame(unsigned char input, float *s, unsigned char *retval);

static unsigned char pause_menu_index = 0;

void DCE_MenuLoadTextures()
{
	printf("Loading Menu Textures\n");
	menu_texID[MENU_LOGO] = DCE_TextureLoadPVR("/cd/textures/gui/LOGO.pvr");//DCE_TextureLoadPVR("/cd/textures/menu/itlof.pvr");
	menu_texID[MENU_NG] = DCE_TextureLoadPVR("/cd/textures/gui/MENU_NG.pvr");
	menu_texID[MENU_SP] = DCE_TextureLoadPVR("/cd/textures/gui/MENU_SP.pvr");
	menu_texID[MENU_MP] = DCE_TextureLoadPVR("/cd/textures/gui/MENU_MP.pvr");
	menu_texID[MENU_CONT] = DCE_TextureLoadPVR("/cd/textures/gui/MENU_CONT.pvr");
	menu_texID[MENU_EXIT] = DCE_TextureLoadPVR("/cd/textures/gui/MENU_EXIT.pvr");
	printf("\nDone Loading Menu Textures\n");
}

extern DCE_Player * DCE_PlayerPointer(unsigned char index);
static char string[48];

static float DCE_LOGO_XPOS = 320;
static unsigned char DCE_LOGO_MODE = 0;

static void DCE_MenuRenderLogo()
{
    DCE_RenderSubmitTexturedQuad2D(DCE_LOGO_XPOS, 56, 512, 96, 0, 0, 1, 1, menu_texID[MENU_LOGO], 0xFFFFFFFF);
}

static void DCE_MenuRenderLogoAnimated()
{
    DCE_LOGO_XPOS += DCE_LOGO_MODE == 0 ? 1 : -1;
    if(DCE_LOGO_XPOS < 320 - 80)
        DCE_LOGO_MODE = 0;
    else if(DCE_LOGO_XPOS > 320 + 80)
        DCE_LOGO_MODE = 1;
  
    DCE_RenderSubmitTexturedQuad2D(DCE_LOGO_XPOS, 56, 512, 96, 0, 0, 1, 1, menu_texID[MENU_LOGO], 0xFFFFFFFF);
}

static void DCE_MenuRenderSwat()
{
	
}

unsigned char DCE_MenuRenderStatsMenu(unsigned char input, float * s, unsigned char *retval)
{
	DCE_RenderStart();			
    
    DCE_RenderSwitchPT();
    
    DCE_MenuRenderLogo();
   
    DCE_RenderSwitchTR();
 
    DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);
 
    DCE_RenderSetBlendDst(PVR_BLEND_DESTALPHA);	     
    
    DCE_PrintStringS("Round Over", 320 - (6 * strlen("Round Over")), 192, 24);
    
    sprintf(string, "WINNER: %s", DCE_PlayerPointer(0)->hud_health[3] >  DCE_PlayerPointer(1)->hud_health[3] ? "Player 1" :
	                              DCE_PlayerPointer(0)->hud_health[3] == DCE_PlayerPointer(1)->hud_health[3] ? "DRAW" : "Player 2" );
    
    DCE_PrintStringS(string, 320 - (6 * strlen(string)), 228, 24);
    
    DCE_RenderFinish();

    switch(input)
    {
    	case CONT_START:
    	case CONT_A:
    		DCE_SfxPlay(WEAPON_FIRE);

    		*s = 1.0f;
    		*retval = 1;
    		break;
	}
    
    return NULL;
}

unsigned char DCE_MenuRenderStats()
{
    uint8 r = 0;
	float s = 1;
	
	while(!r)
	    DCE_MenuRenderStatsMenu(DCE_MenuInputCallback(), &s, &r);
	
	return 0;
}

unsigned char DCE_MenuRenderGameOver(unsigned char input, float * s, unsigned char *retval)
{
	DCE_RenderStart();			

    DCE_RenderSwitchPT();
    
    DCE_MenuRenderLogo();
   
    DCE_RenderSwitchTR();
 
    DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);
 
    DCE_RenderSetBlendDst(PVR_BLEND_DESTALPHA);	     
    
    DCE_PrintStringS("Game Over", 320 - (6 * strlen("Game Over")), 192, 24);
    
    DCE_PrintStringS("Sometimes Even Good Men Die In the Line Of Fire", 320 - (6 * strlen("Sometimes Even Good Men Die In the Line Of Fire")), 228, 24);

    DCE_RenderFinish();

    switch(input)
    {
    	case CONT_START:
    	case CONT_A:
    		DCE_SfxPlay(WEAPON_FIRE);

    		*s = 1.0f;
    		*retval = 1;
    		break;
	}
    
    return NULL;
}

unsigned char DCE_MenuGameOver()
{
    uint8 r = 0;
	float s = 1;
	
	while(!r)
	    DCE_MenuRenderGameOver(DCE_MenuInputCallback(), &s, &r);
	
	return 0;
}

unsigned char DCE_MenuRenderPauseMenu(unsigned char input, float * s, unsigned char *retval)
{
	DCE_RenderStart();			
    
    DCE_RenderSubmitTexturedQuad2D(320, 200,
	                               224 * (pause_menu_index == 0 ? *s : 1.0f), 60 * (pause_menu_index == 0 ? *s : 1.0f),
	                               0, 0, 1, 1, menu_texID[MENU_CONT], 0xFFFFFFFF);

    DCE_RenderSubmitTexturedQuad2D(320, 300,
	                               224 * (pause_menu_index == 1 ? *s : 1.0f), 60 * (pause_menu_index == 1 ? *s : 1.0f),
								   0, 0, 1, 1, menu_texID[MENU_EXIT], 0xFFFFFFFF);

    DCE_RenderSwitchPT();
     
    DCE_MenuRenderLogo();
		     
    DCE_RenderFinish();

    switch(input)
    {
    	case CONT_A:
    		DCE_SfxPlay(WEAPON_FIRE);

    		*s = 1.0f;
    		*retval = pause_menu_index + 1;
    		pause_menu_index = 0;
    		break;
    		
    	case CONT_DPAD_UP:
    		if(pause_menu_index)
    		{
			    *s = 1.0f;
    		    --pause_menu_index;
    		    
    		    DCE_SfxPlay(WEAPON_EMPTY);
    	    }
			break;

    	case CONT_DPAD_DOWN:
    		if(pause_menu_index < 1)
    		{
			    *s = 1.0f;
    		    ++pause_menu_index;
    		    
    		    DCE_SfxPlay(WEAPON_EMPTY);
    	    }
    		break;

    	case CONT_B:
    		*s = 1.0f;
    		pause_menu_index = 0;
    		
    		DCE_SfxPlay(WEAPON_EMPTY);
    		
    		return NULL;
	}
    
    return NULL;
}

void DCE_WaitScreen(unsigned short logoID)
{
	DCE_RenderStart();			

    DCE_RenderSwitchPT();
 
	DCE_MenuRenderLogoAnimated();
	
	DCE_RenderSwitchTR(); //== TR Blend =================================//
                
    DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);
 
    DCE_RenderSetBlendDst(PVR_BLEND_DESTALPHA);	     
            
    DCE_PrintStringS("Press Start", 320 - (6 * strlen("Press Start")), 256, 24);
    
    DCE_PrintString("Early Access Build Version A002", 368, 480 - 32);
    
    DCE_RenderFinish(); 	  
}

void DCE_WaitStartScreen()
{
	pvr_set_bg_color(0.2f, 0.2f, 0.2f);
	
	while(1)
	{
	    DCE_WaitScreen(menu_texID[MENU_LOGO]);
	    
	    switch(DCE_MenuInputCallback())
	    {
    	    case CONT_A:
    	    case CONT_START:
    		    DCE_SfxPlay(WEAPON_FIRE);
				return;	    	
		}
	} 
}




void * DCE_MenuRenderRoot(unsigned char input, float * s, unsigned char *retval)
{
	DCE_RenderStart();			
			             
    DCE_RenderSubmitTexturedQuad2D(320, 240, 224 * (*s), 60 * (*s), 0, 0, 1, 1, menu_texID[MENU_NG], 0xFFFFFFFF);

    DCE_RenderSwitchPT();

    DCE_MenuRenderLogo();
    
    DCE_RenderFinish();
    
    switch(input)
    {
    	case CONT_A:
    	case CONT_START:
    		DCE_SfxPlay(WEAPON_FIRE);
    		*s = 1.0f;
    		return DCE_MenuRenderNewGame;
	}
    
    return DCE_MenuRenderRoot;
}

static unsigned char new_game_index = 0;

void * DCE_MenuRenderNewGame(unsigned char input, float * s, unsigned char *retval)
{
	DCE_RenderStart();			

    DCE_RenderSubmitTexturedQuad2D(320, 200,
	                               224 * (new_game_index == 0 ? *s : 1.0f), 60 * (new_game_index == 0 ? *s : 1.0f),
	                               0, 0, 1, 1, menu_texID[MENU_SP], 0xFFFFFFFF);

    DCE_RenderSubmitTexturedQuad2D(320, 300,
	                               224 * (new_game_index == 1 ? *s : 1.0f), 60 * (new_game_index == 1 ? *s : 1.0f),
								   0, 0, 1, 1, menu_texID[MENU_MP], 0xFFFFFFFF);

    DCE_RenderSwitchPT();
  
    DCE_MenuRenderLogo();
    
    DCE_RenderFinish();

    switch(input)
    {
    	case CONT_A:
    	case CONT_START:

    		DCE_SfxPlay(WEAPON_FIRE);

    		*s = 1.0f;
    		*retval = new_game_index + 1;
    		new_game_index = 0;
    		break;
    		
    	case CONT_DPAD_UP:
    		if(new_game_index)
    		{
			    *s = 1.0f;
    		    --new_game_index;
    		    
    		    DCE_SfxPlay(WEAPON_EMPTY);
    	    }
			break;

    	case CONT_DPAD_DOWN:
    		if(new_game_index < 1)
    		{
			    *s = 1.0f;
    		    ++new_game_index;
    		    
    		    DCE_SfxPlay(WEAPON_EMPTY);
    	    }
    		break;

    	case CONT_B:
    		*s = 1.0f;
    		new_game_index = 0;
    		
    		DCE_SfxPlay(WEAPON_EMPTY);
    		
    		return DCE_MenuRenderRoot;
	}
    
    return DCE_MenuRenderNewGame;
}

unsigned char DCE_MenuRoot()
{
	float s = 1.0f;
	unsigned char mode = 1;
	
	unsigned char v = 0;
	
	MenuCallback menu_func = DCE_MenuRenderRoot;
	
    while(1)
	{
		menu_func = menu_func(DCE_MenuInputCallback(), &s, &v);
        
        s = DCE_StrobePulse(s, 0.87f, 1.13f, 0.0075f, &mode);
        
        if(v)
            return v;
	}	
}

/******************************************************************************/

