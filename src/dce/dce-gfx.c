/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#include <kos.h>


#include "dce-gfx.h"
#include "dce-render.h"
#include "dce-texture.h"

/******************************************************************************/

static uint16 texture[32];
static Font * font;

unsigned short int DCE_TexID(unsigned short GFX)
{
	return texture[GFX];
}

Font * DCE_Font()
{
	return font;
}

Font * DCE_InitGFX()
{
    font = FontInit(512, 512, 10, 10, 0xFF0000FF);   
    
	texture[DCE_TEXID_FONT] = DCE_TextureLoadPVR("/cd/font/FONT0.pvr");

	texture[DCE_TEXID_BULLET] = DCE_TextureLoadJPG("/cd/textures/decals/bullet_decal_01.jpg");	

	texture[DCE_TEXID_BLOOD] = DCE_TextureLoadPVR("/cd/textures/decals/blood_01.pvr");

	texture[DCE_TEXID_SPLATTER] = DCE_TextureLoadPVR("/cd/textures/decals/splatter_01.pvr");
	
	texture[DCE_TEXID_LIGHT1] = DCE_TextureLoadPVR("/cd/textures/decals/flashlight_01.pvr");
	
	texture[DCE_TEXID_SHADOW1] = DCE_TextureLoadPVR("/cd/textures/decals/shadow_01.pvr");

	texture[DCE_TEXID_WEAPONHUD] = DCE_TextureLoadPVR("/cd/textures/overlays/HUD_GUNS.pvr");	
		
	texture[DCE_TEXID_MUZFLASH1] = DCE_TextureLoadPVR("/cd/models/muzzleflash/muzzleflash_01_lq.pvr");

	texture[DCE_TEXID_MUZFLASH2] = DCE_TextureLoadPVR("/cd/models/muzzleflash/muzzleflash_02_lq.pvr");
	
	texture[DCE_TEXID_MUZFLASH3] = DCE_TextureLoadPVR("/cd/models/muzzleflash/muzzleflash_03_lq.pvr");
	
	texture[DCE_TEXID_MUZFLASH4] = DCE_TextureLoadPVR("/cd/models/muzzleflash/muzzleflash_04_lq.pvr");
	
	texture[DCE_BLOOD_MODEL] = DCE_TextureLoadPVR("/cd/models/blood/blood.pvr");
	
    texture[DCE_TEXID_DEAGLE] = DCE_TextureLoadPVR("/cd/models/weapons/DE_50_D.pvr");
    texture[DCE_TEXID_HKUSP] = DCE_TextureLoadPVR("/cd/models/weapons/HK_usp_D.pvr");
	
/*
	muzTexID = sprite->texID[0] = DCE_TextureLoadJPG("/cd/sprites/player/muzzleflash1_0.jpg");
	sprite->texID[1] = DCE_TextureLoadJPG("/cd/sprites/player/muzzleflash1_1.jpg");
	sprite->texID[2] = DCE_TextureLoadJPG("/cd/sprites/player/muzzleflash1_2.jpg");
*/
	
	return font;
}
