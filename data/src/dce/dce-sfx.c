/* 
** DCE - Dreamcast Engine (C) 2016 Josh PH3NOM Pearson
*/

#include <math.h>
#include <kos.h>

#include "dce-sfx.h"
#include "dce-menu.h"
#include "dce-player.h"

static uint32 DCE_SFX[DCE_MAX_SOUND_EFFECTS];

static DCE_Sfx DCE_STREAMFX[DCE_MAX_SOUND_EFFECTS];

/* Compute Stereo Pan Value For 3D Sound Source */
unsigned char DCE_PositionalAudioPan(vec3f_t * O, vec3f_t * L, vec3f_t * P)
{
	vec3f_t D, V;
	float angle, dot, det;
 
    vec3f_sub(O, L, &D);       // Player Look At Direction
    vec3f_normalize_2d(&D);

    vec3f_sub(O, P, &V);      // Sound Source To Player Direction
    vec3f_normalize_2d(&V);

    dot = (D.x * V.x) + (D.z * V.z);  // Stereo Dot Product
    det = (D.x * V.z) + (D.z * V.x);  // Stereo Determinant
    angle = atan2(det, dot) * DEGREE; // Compute Stereo Angle Between Vectors D and V
    
    if(angle > 90)                    // Domain Range Conversion for Angle to Pan
        angle = 90 - (angle - 90);
    else if(angle < -90)
        angle = 90 - (angle + 90);
    
    // Return Pan In Range of 0 = Left to 255 = Right
    return (unsigned char)((((angle + 90) * 1.4166666666666666666666666666667) + 0.5));
}

void DCE_InitSfx()
{
    spu_init();         // Initialize DC Sound Processor Unit
    
    snd_stream_init();  // Initialize DC SPU Stream Functions
    
    DCE_SFX[WEAPON_FIRE]     = snd_sfx_load("/cd/sound/weapons/m4a1fire.wav"); 
    
    DCE_SFX[FIRE_GLOCK_SILENCER] = snd_sfx_load("/cd/sound/weapons/FP9mm1.wav"); 
    
    DCE_SFX[WEAPON_EMPTY]    = snd_sfx_load("/cd/sound/weapons/noammo.wav");
    
    DCE_SFX[WEAPON_RELOAD]   = snd_sfx_load("/cd/sound/weapons/reload.wav");   
    
	DCE_SFX[WEAPON_HIT]      = snd_sfx_load("/cd/sound/misc/hit.wav");  
    
	DCE_SFX[WEAPON_HIT_HEAD] = snd_sfx_load("/cd/sound/misc/headshot.wav");     
	
    DCE_SFX[CAMERA_01]       = snd_sfx_load("/cd/sound/camera/camera_03.wav"); 
	
    DCE_SFX[WEAPON_MELEE1]   = snd_sfx_load("/cd/sound/weapons/FPmeleehandgun1.wav"); 	     

    DCE_SFX[SFX_HKUSP1]      = snd_sfx_load("/cd/sound/weapons/HKUSP/HKUSP_1.wav");
    DCE_SFX[SFX_HKUSP2]      = snd_sfx_load("/cd/sound/weapons/HKUSP/HKUSP_2.wav");

    DCE_SFX[SFX_GLOCK1]      = snd_sfx_load("/cd/sound/weapons/GLOCK17/glock17_1.wav");
    DCE_SFX[SFX_GLOCK2]      = snd_sfx_load("/cd/sound/weapons/GLOCK17/glock17_2.wav");
    DCE_SFX[SFX_GLOCK3]      = snd_sfx_load("/cd/sound/weapons/GLOCK17/glock17_3.wav");
			
    DCE_SFX[SFX_DEAGLE1]      = snd_sfx_load("/cd/sound/weapons/DEAGLE/DesertEagle_1.wav");		
    DCE_SFX[SFX_DEAGLE2]      = snd_sfx_load("/cd/sound/weapons/DEAGLE/DesertEagle_2.wav");
		
	DCE_SFX[ENEMY_WALK1]     = snd_sfx_load("/cd/sound/walking/asphaltsneakerwalk1.wav"); 
	DCE_SFX[ENEMY_WALK2]     = snd_sfx_load("/cd/sound/walking/asphaltsneakerwalk2.wav"); 
	DCE_SFX[ENEMY_WALK3]     = snd_sfx_load("/cd/sound/walking/asphaltsneakerwalk3.wav"); 	
	DCE_SFX[ENEMY_WALK4]     = snd_sfx_load("/cd/sound/walking/asphaltsneakerwalk4.wav"); 	
	DCE_SFX[ENEMY_WALK5]     = snd_sfx_load("/cd/sound/walking/asphaltsneakerwalk5.wav"); 	

	DCE_SFX[PLAYER_WALK1]     = snd_sfx_load("/cd/sound/walking/asphaltbootwalk1.wav");	
	DCE_SFX[PLAYER_WALK2]     = snd_sfx_load("/cd/sound/walking/asphaltbootwalk2.wav");
	DCE_SFX[PLAYER_WALK3]     = snd_sfx_load("/cd/sound/walking/asphaltbootwalk3.wav");
	DCE_SFX[PLAYER_WALK4]     = snd_sfx_load("/cd/sound/walking/asphaltbootwalk4.wav");
    DCE_SFX[PLAYER_WALK5]     = snd_sfx_load("/cd/sound/walking/asphaltbootwalk5.wav");
	DCE_SFX[PLAYER_WALK6]     = snd_sfx_load("/cd/sound/walking/asphaltbootwalk6.wav");	
	
	DCE_SFX[IMPACT_FLESH1]    = snd_sfx_load("/cd/sound/impact/FPHGmeleeimpactflesh1.wav");
	DCE_SFX[IMPACT_FLESH2]    = snd_sfx_load("/cd/sound/impact/FPHGmeleeimpactflesh2.wav");
    DCE_SFX[IMPACT_WOOD]      = snd_sfx_load("/cd/sound/impact/FPHGmeleeimpactwood.wav");   
    DCE_SFX[IMPACT_METAL]     = snd_sfx_load("/cd/sound/impact/FPHGmeleeimpactmetal.wav"); 		

	DCE_SFX[ENEMY_DEATH1]     = snd_sfx_load("/cd/sound/enemy/death/NMEcollapse1.wav");
	DCE_SFX[ENEMY_DEATH2]     = snd_sfx_load("/cd/sound/enemy/death/NMEcollapse2.wav");
	DCE_SFX[ENEMY_DEATH3]     = snd_sfx_load("/cd/sound/enemy/death/NMEcollapse3.wav");
	
	DCE_SFX[ENEMY_MELEE]      = snd_sfx_load("/cd/sound/enemy/melee/NMEmeleehandgun.wav");
	
	DCE_SFX[SFX_BOMBTICK]     = snd_sfx_load("/cd/sound/bombtick/bombtick.wav");
		
//	DCE_SFX[PLAYER_LOW_HEALTH] = snd_sfx_load_ex("/cd/sound/player/FPlowhealthheavybreath.adp");   
}

void DCE_SfxStop(uint32 sfx)
{
	snd_sfx_stop(DCE_SFX[sfx]);
}

void DCE_SfxPlay(uint32 sfx)
{
    snd_sfx_play(DCE_SFX[sfx], DCE_SfxVolume(), DCE_SFX_CHAN_STEREO);
}

void DCE_SfxPlayHalfVolume(uint32 sfx)
{
    snd_sfx_play(DCE_SFX[sfx], DCE_SfxVolume() / 2, DCE_SFX_CHAN_STEREO);
}

void DCE_SfxPlayAttenuation(uint32 sfx, vec3f_t * O, vec3f_t * P)
{
    float d;
    
    vec3f_distance(O->x, O->y, O->z, P->x, P->y, P->z, d); 
    
    if(d > DCE_MAX_VOL_DISTANCE)
        return;  // Too Quiet To be Heard - Dont Even Bother Playing SFX at 0 volume
    
    unsigned char v = 255 - ((d * 255) / DCE_MAX_VOL_DISTANCE); // Invert Bias and Scale in range 0->255
 
	snd_sfx_play(DCE_SFX[sfx], v, DCE_SFX_CHAN_STEREO);
}

void DCE_SfxPlayAttenuationPan(uint32 sfx, vec3f_t *s, vec3f_t * d, float di)
{
    if(di > DCE_MAX_VOL_ATTENUATION)
        return;  // Too Quiet To be Heard - Dont Even Bother Playing SFX at 0 volume
    
    di = 255 - ((di / DCE_MAX_VOL_ATTENUATION) * 255); // Invert Bias and Scale in range 0->255

	snd_sfx_play(DCE_SFX[sfx], (unsigned char)di, DCE_SFX_CHAN_STEREO);
}


int DCE_BombtickThread()
{
	vec3f_t V;
	float d, dot;
	
	vec3f_t P = { -178.8, -441, -672 };
	
	while(1)
	{
		vec3f_distance(P.x, P.y, P.z,
		               DCE_PlayerPointer(0)->position.x, DCE_PlayerPointer(0)->position.y, DCE_PlayerPointer(0)->position.z, d);	           
		if(d <= 750 && (fabs(P.y - DCE_PlayerPointer(0)->position.y) < 60))
		    snd_sfx_play(DCE_SFX[SFX_BOMBTICK],
			            (unsigned char)255 - ((d / 750) * 255), 0x80);
			           // DCE_PositionalAudioPan(&DCE_PlayerPointer(0)->position, &DCE_PlayerPointer(0)->lookAt, &P));
		
		thd_sleep(900);
	}
	
	return 0;
}

void DCE_StreamSFX()
{
	thd_create(NULL, DCE_BombtickThread, NULL);
}


//== Streaming SFX Manager ====================================================================//

static unsigned char DCE_STREAM_THREAD = 0;

int DCE_StreamThread()
{
	vec3f_t V;
	float d, dot;
	unsigned short i;
		
	while(1) // Endless Loop Checking And Updating Stream SFX States
	{
		unsigned long time = DCE_GetTime(); // 1 Call to GetTime() Per Frame!
			
		for(i = 0; i < DCE_MAX_SOUND_EFFECTS; i++)
		{
			// If Stream SFX Is Active, Handle Stream State
			if(DCE_STREAMFX[i].active[0])
			{
				printf("Active Stream: %i, len %i, index %i\n", i, DCE_STREAMFX[i].len, DCE_STREAMFX[i].index);
				
				if(time >= (DCE_STREAMFX[i].start + DCE_STREAMFX[i].len)) // restart sfx
				{
					// Reset Start Time
					DCE_STREAMFX[i].start = time;
					
					// Compute Distance From Sound Source To Player
					vec3f_distance(DCE_STREAMFX[i].pos.x, DCE_STREAMFX[i].pos.y, DCE_STREAMFX[i].pos.z,
		                DCE_PlayerPointer(0)->position.x, DCE_PlayerPointer(0)->position.y, DCE_PlayerPointer(0)->position.z, d);
					
					printf("Restart Stream[%i], %f / %f @ %i\n", DCE_STREAMFX[i].index, d, DCE_STREAMFX[i].distance, time);
					
					// If Distance Is Within Range, Play SFX With Distance Attenuation
					if(d <= DCE_STREAMFX[i].distance)
		                snd_sfx_play(DCE_STREAMFX[i].index,
			                        (unsigned char)255 - ((d / DCE_STREAMFX[i].distance) * 255), 0x80);
			            // DCE_PositionalAudioPan(&DCE_PlayerPointer(0)->position, &DCE_PlayerPointer(0)->lookAt, &P));
			            
				}
			}
/*
			// If Stream SFX Is Active, Handle Stream State 2
			if(DCE_STREAMFX[i].active[1])
			{
				if((DCE_STREAMFX[i].start + DCE_STREAMFX[i].len) >= time) // restart sfx
				{
					// Reset Start Time
					DCE_STREAMFX[i].start = time;
					
					// Compute Distance From Sound Source To Player
					vec3f_distance(DCE_STREAMFX[i].pos.x, DCE_STREAMFX[i].pos.y, DCE_STREAMFX[i].pos.z,
		                DCE_PlayerPointer(1)->position.x, DCE_PlayerPointer(1)->position.y, DCE_PlayerPointer(1)->position.z, d);
					
					// If Distance Is Within Range, Play SFX With Distance Attenuation
					if(d <= DCE_STREAMFX[i].distance)
		                snd_sfx_play(DCE_STREAMFX[i].index,
			                        (unsigned char)255 - ((d / DCE_STREAMFX[i].distance) * 255), 0x80);
			            // DCE_PositionalAudioPan(&DCE_PlayerPointer(1)->position, &DCE_PlayerPointer(1)->lookAt, &P));
			            
				}
			}
*/
		}
		thd_sleep(10);
	}
	
	return 0;
}

void DCE_SfxStartStreamThread()
{
    thd_create(NULL, DCE_StreamThread, NULL);
}

void DCE_SfxStreamLoad(char * segment, unsigned char active,
                       char * sound, vec3f_t * pos, float d, unsigned char loop, unsigned short len)
{
	unsigned char i = 0;
	while(i++ < 255)
	    if(!DCE_STREAMFX[i].used)
	        break;
	
	DCE_STREAMFX[i].used = 1;
	
	strcpy(DCE_STREAMFX[i].segment, segment);
	
	DCE_STREAMFX[i].active[0] = DCE_STREAMFX[i].active[1] = DCE_STREAMFX[i].active[2] = DCE_STREAMFX[i].active[3] = active;
	
	DCE_STREAMFX[i].index = snd_sfx_load(sound); 
	
	vec3f_copy(pos, &DCE_STREAMFX[i].pos);
	
	DCE_STREAMFX[i].distance = d;
	
	DCE_STREAMFX[i].loop = loop;
	
	DCE_STREAMFX[i].len = len;
	
	DCE_STREAMFX[i].start = DCE_GetTime();
}

//== CDDA Wrapper =============================================================================//

unsigned char DCE_CURRENT_CDDA_TRACK = 1;

void DCE_SetVolumeCDDA(unsigned char vol)
{
	spu_cdda_volume(vol, vol);
}

void DCE_PlayCDDA(unsigned char track)
{
	if(track != DCE_CURRENT_CDDA_TRACK)
	    cdrom_cdda_play(track, track, 10, CDDA_TRACKS);
	    
	DCE_CURRENT_CDDA_TRACK = track;
}

void DCE_PauseCDDA()
{
	cdrom_cdda_pause();
}

void DCE_ResumeCDDA()
{
	cdrom_cdda_resume();
}

void DCE_StopCDDA()
{
	cdrom_spin_down();
}
