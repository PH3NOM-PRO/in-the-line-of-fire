/* 
** DCE - Dreamcast Engine (C) 2013-2015 Josh PH3NOM Pearson
*/

#ifndef DCE_SFX_H
#define DCE_SFX_H

#include "dce-vector.h"

#define DCE_MAX_SOUND_EFFECTS 255

#define DCE_SFX_CHAN_LEFT   0x0
#define DCE_SFX_CHAN_STEREO 0x80
#define DCE_SFX_CHAN_RIGHT  0xFF

#define DCE_MIN_VOL 0x0
#define DCE_MAX_VOL 0xFF

#define DCE_MAX_VOL_ATTENUATION 1750.0f
#define DCE_MAX_VOL_DISTANCE    2000.0f

enum DCE_CDDA_TRACK
{
    DYSTOPIA = 1,
	FINAL_ASSAULT,
	TRIUMPH,
	ASHES_HIGH_OCTANE
};

enum DCE_SOUND_EFFECT
{
    WEAPON_FIRE = 0,
    WEAPON_RELOAD,
    WEAPON_EMPTY,
    WEAPON_HIT,
    WEAPON_HIT_HEAD,
	WEAPON_MELEE1,
	WEAPON_MELEE2,
	WEAPON_MELEE3,
	CAMERA_01,
	PLAYER_LOW_HEALTH,
	ENEMY_WALK1,
	ENEMY_WALK2,
	ENEMY_WALK3,
	ENEMY_WALK4,
	ENEMY_WALK5,
	FIRE_GLOCK_SILENCER,
	FIRE_GLOCK,
	FIRE_HKUSP,
	PLAYER_WALK1,
	PLAYER_WALK2,
	PLAYER_WALK3,
	PLAYER_WALK4,
	PLAYER_WALK5,    
	PLAYER_WALK6,
	IMPACT_FLESH1,
	IMPACT_FLESH2,
	IMPACT_METAL,
	IMPACT_WOOD,
	ENEMY_DEATH1,
	ENEMY_DEATH2,
	ENEMY_DEATH3,
	ENEMY_MELEE, // 32 Effect Mark
	SFX_HKUSP1,
	SFX_HKUSP2,
	SFX_GLOCK1,
	SFX_GLOCK2,
	SFX_GLOCK3, 
	SFX_DEAGLE1,
	SFX_DEAGLE2,
	SFX_BOMBTICK 
};

typedef struct
{
	vec3f_t pos;
	float   distance;
	unsigned char index;
	unsigned char loop;
	unsigned short len;
	unsigned long start;
	char segment[32];
	unsigned char active[4];
	unsigned char used;
} DCE_Sfx;

void DCE_InitSfx();

void DCE_SfxPlay(uint32 sfx);

void DCE_SfxPlayHalfVolume(uint32 sfx);

void DCE_SfxStartStreamThread();

void DCE_SfxStreamLoad(char * segment, unsigned char active,
                       char * sound, vec3f_t * pos, float d, unsigned char loop, unsigned short len);

/* Play A Sound Effect with Attenuating Volume based on Position */
/* S = Sound Source Position D = Sound Destination Position */
/* The closer the Distance between S and D, the louder the volme will be */
void DCE_SfxPlayAttenuation(uint32 sfx, vec3f_t *s, vec3f_t * d);

void DCE_SfxPlayAttenuationPan(uint32 sfx, vec3f_t *s, vec3f_t * d, float di);


void DCE_PlayCDDA(unsigned char track);

#endif
