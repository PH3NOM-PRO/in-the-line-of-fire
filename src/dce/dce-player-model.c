
#include "dce.h"

#include <stdlib.h>
#include <string.h>

static md2_mdl * swat_lod0;
static md2_mdl * swat_lod1;
static md2_mdl * swat_lod2;
static md2_mdl * swat_hands;
static md2_mdl * blood_mdl;

static md2_mdl * swat_glock_lod0;
static md2_mdl * swat_deagle_lod0;
static md2_mdl * swat_hkusp_lod0;

static md2_mdl * swat_glock;
static md2_mdl * swat_hkusp;
static md2_mdl * swat_deagle;
static md2_mdl * muzflash;
static md2_mdl * muzflash2;

static uint16 swat_tex;
static uint16 swat_hands_tex;
static uint16 blood_tex;

static uint16 backer_legs, backer_torso, backer_head;

static md2_mdl * clown_hands;

static uint16 clown_hands_tex;

md2_mdl * DCE_MuzzleflashModel()
{
    return muzflash2;
}

static const char DCE_WeaponFrameName[DCE_WEAPON_FRAMES][16] = { "IDLE",
                                                              "EQUIP",
                                                              "HOLSTER",
													          "FIRE",
													          "MELEE1",
													          "MELEE2",
													          "RELOAD",
													          "WALK",
													          "RUN" };

static const char DCE_PlayerFrameName[DCE_PLAYER_FRAMES][24] = { "IDLE",
                                                              "EQUIP",
                                                              "HOLSTER",
													          "FIRE",
													          "MELEE1",
													          "MELEE2",
													          "RELOAD",
													          "DAMAGE",
													          "CROUCH_FORWARD",
													          "CROUCH_BACKWARD",
													          "WALK_FORWARD",
													          "WALK_BACKWARD",
													          "WALK_STRAFE_LEFT",
													          "WALK_STRAFE_RIGHT",
													          "RUN_FORWARD",
													          "RUN_BACKWARD",
													          "RUN_STRAFE_LEFT",
													          "RUN_STRAFE_RIGHT",
													          "DEATH1",
													          "DEATH2",
															  "CROUCH_IDLE",
															  "WALK_FIRE" };
													   
unsigned char DCE_PlayerWeaponModelFrameIndex(char * buf)
{
	unsigned char i;
	for(i = 0; i < DCE_WEAPON_FRAMES; i++)
	    if(!strncmp(buf, DCE_WeaponFrameName[i], strlen(DCE_WeaponFrameName[i])))
		    return i;
	
	return 255; 
}													   

unsigned char DCE_PlayerModelFrameIndex(char * buf)
{
	unsigned char i;
	for(i = 0; i < DCE_PLAYER_FRAMES; i++)
	    if(!strncmp(buf, DCE_PlayerFrameName[i], strlen(DCE_PlayerFrameName[i])))
		    return i;
	
	return 255; 
}

extern void DCE_TracePlayerMelee(DCE_Player * player);

void DCE_PlayerModelWeaponSwitch(DCE_Player * player)
{
	DCE_PlayerModel * mdl = &player->model;
	
	if(mdl->weapon_texID == DCE_TexID(DCE_TEXID_DEAGLE))
	{
		mdl->weapon_attachment = NULL;
		
		mdl->weapon_lod1 = swat_hkusp_lod0;	
		mdl->weapon = swat_hkusp;
		
		mdl->weapon_texID = DCE_TexID(DCE_TEXID_HKUSP);
	
		player->bullet_damage = 15;
	}
	else
	{
		mdl->weapon_attachment = NULL;
		
		mdl->weapon_lod1 = swat_deagle_lod0;	
		mdl->weapon = swat_deagle;
		
		mdl->weapon_texID = DCE_TexID(DCE_TEXID_DEAGLE);
	
		player->bullet_damage = 25;
	}
}

void DCE_PlayerAnimationStartFrame(DCE_Player * player)
{
 	if(player->health == 0 || player->model.body_index == DEATH2)
    {
		if(player->model.body_index != DEATH2)
    	{
    		player->model.body_index = DEATH2;
    		player->model.body_frame = 0;
		}
	    return;
	}   /*
	else if(player->state & PSTATE_HURT)
    {
    	if(player->model.body_index != DAMAGE)
    	{
    		player->model.body_index = DAMAGE;
    		player->model.body_frame = 0;
		}
		else if(player->model.body_index == player->model.body_frame_count[player->model.body_frame] - 1)
		{
			player->state &= ~PSTATE_HURT;
		}    	
	}*/
	else if(player->last_moved) // RELOAD
    {
    	/* Body
		if(player->model.body_index != RELOAD)
    	{
    		player->model.body_index = RELOAD;
    		player->model.body_frame = 0;
		}
		else if(player->model.body_index == player->model.body_frame_count[player->model.body_frame] - 1)
		{
			player->state &= ~PSTATE_HURT;
		} */   	
        // Hands
    	if(player->model.hands_index != RELOAD)
    	{
    		player->model.hands_index = RELOAD;
    		player->model.hands_frame = 0;
    		
    		DCE_SfxPlay(WEAPON_RELOAD);
		}
		else if(player->model.hands_frame == player->model.hands_frame_count[player->model.hands_index] - 1)
		{
			player->last_moved = 0;

        	player->model.hands_index = HANDS_IDLE;
    	    player->model.hands_frame = 0;

	        if(player->ammo)
	        {
	        	unsigned char d = player->clip_size - player->clip;
	        	
	        	if(player->ammo < d)
	        	{
	        	    player->clip = player->ammo;
	        	    player->ammo = 0;		        		
				}
				else
				{
				    player->clip += d;
	        	    player->ammo -= d;
				}
				
			}
		} 
	}
	else if(player->state & PSTATE_WEAPON_SWITCH)
	{
    	if((player->model.hands_index != HANDS_HOLSTER) && (player->model.hands_index != HANDS_EQUIP))
    	{
    		player->model.hands_index = HANDS_HOLSTER;
    		player->model.hands_frame = 0;
    		
    		DCE_SfxPlay(WEAPON_RELOAD);
		}
		else if(player->model.hands_frame == player->model.hands_frame_count[player->model.hands_index] - 1)
		{
        	if(player->model.hands_index != HANDS_EQUIP)
        	{			
			    // Set Model And HUD Here!!!
			    DCE_PlayerModelWeaponSwitch(player);
			    
			    DCE_HudSwitchWeapons();
			    
				player->model.hands_index = HANDS_EQUIP;
    	        player->model.hands_frame = 0;
            }
            else
			{
			
        	    player->model.hands_index = HANDS_IDLE;
    	        player->model.hands_frame = 0;

	            player->state &= ~PSTATE_WEAPON_SWITCH;
	        }
		} 
	}
	else if((player->state & PSTATE_FIRE1) && !(player->lstate & PSTATE_FIRE1))
	{
		if(player->state & PSTATE_MOVE)
		{
		    if(player->model.body_index != WALK_FIRE)
		    {
			    player->model.body_index = WALK_FIRE;
			    player->model.body_frame = 0;
		    }		
		}
		else
		{
		    if(player->model.body_index != FIRE)
		    {
			    player->model.body_index = FIRE;
			    player->model.body_frame = 0;
		    }
		}
	}
	else if(player->state & PSTATE_FIRE2)
	{
		if(player->model.body_index != MELEE1)
		{
			
			player->model.body_index = MELEE1;
			player->model.body_frame = 0;
		}
	}
    else if(player->state & PSTATE_WALK_FORWARD)
    {
    	if(player->model.body_index != WALK_FORWARD)
    	{
    		player->model.body_index = WALK_FORWARD;
    		player->model.body_frame = 0;
		}
	}
    else if(player->state & PSTATE_WALK_BACKWARD)
    {
    	if(player->model.body_index != WALK_BACKWARD)
    	{
    		player->model.body_index = WALK_BACKWARD;
    		player->model.body_frame = 0;
      
		}
	}
    else if(player->state & PSTATE_WALK_STRAFE_LEFT)
    {
    	if(player->model.body_index != WALK_STRAFE_LEFT)
    	{
    		player->model.body_index = WALK_STRAFE_LEFT;
    		player->model.body_frame = 0;
		}
	}
    else if(player->state & PSTATE_WALK_STRAFE_RIGHT)
    {
    	if(player->model.body_index != WALK_STRAFE_RIGHT)
    	{
    		player->model.body_index = WALK_STRAFE_RIGHT;
    		player->model.body_frame = 0;
		}
	}

	else
	{
    	if(player->model.body_index == MELEE1 || player->model.body_index == FIRE || player->model.body_index == WALK_FIRE)  // States that need to continue untill animation is finished
    	{
    		if(player->model.body_frame == player->model.body_frame_count[player->model.body_index] - 1)
            {
	    	    player->model.body_index = IDLE;
	            player->model.body_frame = 0;
		    }
		}
		else if(player->model.body_index != IDLE)
    	{
    		player->model.body_index = IDLE;
    		player->model.body_frame = 0;

		}		
	}
	
	/*	*/
	if(player->state & PSTATE_FIRE2 && !(player->last_moved))
	{
		if(player->model.hands_index != HANDS_MELEE1)
		{
			DCE_SfxPlay(WEAPON_MELEE1);
			player->model.hands_index = HANDS_MELEE1;
			player->model.hands_frame = 0;
		}
	}
	
	if(player->state & PSTATE_FIRE1 && (!(player->lstate & PSTATE_FIRE1))
	   && !(player->last_moved) && !(player->state & PSTATE_WEAPON_SWITCH))
	{
		if(player->model.hands_index != HANDS_FIRE)
		{
	        if(player->clip > 0)
	        {        	
			    switch(player->bullet_damage)
	            {
	        	    case 25:
	        		    DCE_SfxPlay(SFX_DEAGLE1 + rand() % 1);
	        		    break;
	        		
                    case 15:
                	    DCE_SfxPlay(SFX_GLOCK1 + rand() % 1);
                	    break;
			    }
			
			    DCE_RenderInsertBulletDecal(player, DCE_TexID(DCE_TEXID_BULLET));
			
			    DCE_EnemyTracePlayerRay(player);
			    
				DCE_PlayerTracePlayerRay(player);
			
			    player->model.hands_index = HANDS_FIRE;
			    player->model.hands_frame = 0;
		        
		        --player->clip;
		   }
		   else if(player->ammo)
		   {
			  player->last_moved = 1;
	       } 
		}
	}
		
	if(((player->model.hands_index == HANDS_MELEE1) || (player->model.hands_index == HANDS_FIRE)) && !(player->state & PSTATE_WEAPON_SWITCH))
	{
        if(player->model.hands_frame == player->model.hands_frame_count[player->model.hands_index] - 1)
        {
	    	player->model.hands_index = HANDS_IDLE;
	        player->model.hands_frame = 0;
		}
		
		if(player->model.hands_index == HANDS_MELEE1)
		   if(player->model.hands_frame == 6)
		   {
		   	   DCE_EnemyTracePlayerMelee(player);
		   	   DCE_TracePlayerMelee(player);
		   }
	}

	else if(player->state & PSTATE_MOVE && !(player->last_moved) && !(player->state & PSTATE_WEAPON_SWITCH))
	{
		if(player->model.hands_index != HANDS_WALK)
		{
			
			player->model.hands_index = HANDS_WALK;
			player->model.hands_frame = 0;
		
		}
	}
	else if(!player->last_moved && !(player->state & PSTATE_WEAPON_SWITCH))
	{    		
    	player->model.hands_index = HANDS_IDLE;
    	player->model.hands_frame = 0;
	}
}

void DCE_PlayerAnimationEndFrame(DCE_Player * player, unsigned int render_frame)
{
	if(player->health == 0)
		if(player->model.body_frame == player->model.body_frame_count[player->model.body_index] - 1)
		{
		    ++player->state_frame;
		    return;
        }
    if(player->state & PSTATE_MOVE)
    {
    	if(player->model.body_frame == 6 || player->model.body_frame == 12)
    	{
    		if(player->state_frame % 2 == 0)
			    DCE_SfxPlay(PLAYER_WALK1 + rand() % 6);
		}
	}
	
	++player->state_frame;
	
	if(!(render_frame % (30  / player->model.body_fps)))
	{
		++player->model.body_frame;
						
		if(player->model.body_frame >= player->model.body_frame_count[player->model.body_index])
		{
		    player->model.body_frame = 0;
		    player->state_frame = 0;
		}
	}
	
	if(!(render_frame % (30 / player->model.hands_fps)))
	{
		++player->model.hands_frame;
		
		if(player->model.hands_frame >= player->model.hands_frame_count[player->model.hands_index])
		    player->model.hands_frame = 0;
	}  	
}

void DCE_SetFramesAscend(unsigned char * buf, unsigned char start, unsigned char count)
{
	unsigned char i;
	for(i = 0; i < count; i++)
	    buf[i] = start + i;
}

void DCE_SetFramesDescend(unsigned char * buf, unsigned char start, unsigned char count)
{
	unsigned char i;
	for(i = 0; i < count; i++)
	    buf[i] = start - i;
}

static char buf[512];

void DCE_ParsePlayerHandsModelFrameList(DCE_PlayerModel * model, char * fname)
{
	FILE * f = fopen(fname, "rb");
	if(f == NULL)
	    return;

	char id[32];
	int start;
	int count;
	
	unsigned int index, i;
	
	fgets(buf, 1023, f);
	
	sscanf(buf, "%s %i", id, &count);
	
	model->hands_fps = count & 0xFF;
    
	printf("PlayerHandsInit\n");
		
	while(fgets(buf, 1023, f))
	{
		sscanf(buf, "%s %i %i", id, &start, &count);
		
		index = DCE_PlayerWeaponModelFrameIndex(id);

//		printf("%s = Index %i || st %i / ct %i\n", id, index, start, count);
		
		model->hands_frames[index] = malloc(abs(count));
		model->hands_frame_count[index] = (abs(count)) & 0xFF;
	
	    if(count < 0)
	        DCE_SetFramesDescend(&model->hands_frames[index][0], start, abs(count));
	    else
	        DCE_SetFramesAscend(&model->hands_frames[index][0], start, count);	    
	        
//	    for(i = 0; i < abs(count); i++)
//	        printf("Index: %i\n", model->hands_frames[index][i]);
	}
	
	fclose(f);
	
	model->hands_index = HANDS_WALK;
	model->hands_frame = 0;

//	for(i = 0; i < DCE_WEAPON_FRAMES; i++)
//	    printf("%s: %i frames\n", DCE_WeaponFrameName[i], model->hands_frame_count[i]);

}

void DCE_ParsePlayerBodyModelFrameList(DCE_PlayerModel * model, char * fname)
{
	FILE * f = fopen(fname, "rb");
	if(f == NULL)
	    return;

	char id[32];
	int start;
	int count;
	
	unsigned int index, i;
	
	fgets(buf, 1023, f);
	
	sscanf(buf, "%s %i", id, &count);
	
	model->body_fps = count & 0xFF;
    
	printf("PlayerBodyInit\n");
		
	while(fgets(buf, 1023, f))
	{
		sscanf(buf, "%s %i %i", id, &start, &count);
		
		index = DCE_PlayerModelFrameIndex(id);

		printf("%s = Index %i || st %i / ct %i\n", id, index, start, count);
		
		model->body_frames[index] = malloc(abs(count));
		model->body_frame_count[index] = (abs(count)) & 0xFF;
	
	    if(count < 0)
	        DCE_SetFramesDescend(&model->body_frames[index][0], start, abs(count));
	    else
	        DCE_SetFramesAscend(&model->body_frames[index][0], start, count);	    
	        
	    for(i = 0; i < abs(count); i++)
	        printf("Index: %i\n", model->body_frames[index][i]);
	}
	
	fclose(f);
	
	model->body_index = IDLE;
	model->body_frame = 0;
}

void DCE_PlayerModelLoad(DCE_PlayerModel * mdl, char * body_txt, char * hands_txt,
                                md2_mdl * body_mdl, md2_mdl * body_lod1, md2_mdl * body_lod2,
								md2_mdl * hands_mdl, md2_mdl * weapon_mdl,
								unsigned short body_texID, unsigned short hands_texID, unsigned short weapon_texID)
{
	if(hands_txt != NULL)
	    DCE_ParsePlayerHandsModelFrameList(mdl, hands_txt);
    
	//if(mdl->body != NULL)
	DCE_ParsePlayerBodyModelFrameList(mdl, body_txt);
	
	mdl->body = body_mdl;
	mdl->body_lod1 = body_lod1;
	mdl->body_lod2 = body_lod2;	
	mdl->hands = hands_mdl;	
	mdl->weapon_lod1 = weapon_mdl;	
	mdl->body_texID	= body_texID;
	mdl->hands_texID = hands_texID;									
	mdl->weapon_texID = weapon_texID;
	
	mdl->muzzleflash = muzflash2;
}



void DCE_LoadPlayerModels()
{
//    swat_lod0  = DCE_LoadHQMD2("/cd/models/swat/swat_lod0.md2");
/*
    swat_lod0 = DCE_LoadHQMD2("/cd/models/backer1/jacket.md2");

    swat_lod1        = DCE_LoadHQMD2("/cd/models/backer1/head.md2");//DCE_LoadHQMD2("/cd/models/swat/swat_lod1.md2");
    
	swat_lod2        = DCE_LoadHQMD2("/cd/models/backer1/pants.md2");//DCE_LoadHQMD2("/cd/models/swat/swat_lod2.md2");
*/
    swat_lod0 = DCE_LoadHQMD2("/cd/models/swat/swat_lod1.md2");

//    swat_lod1 = DCE_EnemyModelClown();
//    swat_lod2 = NULL;
    
    swat_deagle_lod0 = DCE_LoadHQMD2("/cd/models/weapons/DesertEagle.md2");
    
    swat_hkusp_lod0  = DCE_ModelHKUSP();
    
    swat_hands       = DCE_LoadHQMD2Clip("/cd/models/swat/swat_hands.md2");
    
    swat_deagle      = DCE_LoadHQMD2Clip("/cd/models/swat/desert_eagle.md2");
    
	swat_hkusp       = DCE_LoadHQMD2Clip("/cd/models/swat/hk_usp.md2");
    
    muzflash         = DCE_LoadHQMD2("/cd/models/muzzleflash/muzzleflash_anim.md2");
    
    muzflash2        = DCE_LoadHQMD2("/cd/models/muzzleflash/muzzleflash_anim_02.md2");
    
	blood_mdl        = DCE_LoadHQMD2("/cd/models/blood/blood_anim_02.md2");
	
    blood_tex        = DCE_TexID(DCE_BLOOD_MODEL);

    swat_tex         = DCE_TextureLoadPVR("/cd/models/swat/swat_D.pvr");
    
    swat_hands_tex   = DCE_TextureLoadPVR("/cd/models/swat/gloves_D.pvr");

    clown_hands      = DCE_LoadHQMD2Clip("/cd/models/clown/ClownHands.md2");
    
    clown_hands_tex  = DCE_TextureLoadPVR("/cd/models/clown/ClownHands_D.pvr");
/*    
    backer_legs  = DCE_TextureLoadPVR("/cd/models/backer1/pants_D.pvr");
    backer_torso = DCE_TextureLoadPVR("/cd/models/backer1/jacket_D.pvr");
    backer_head  = DCE_TextureLoadPVR("/cd/models/backer1/head_D.pvr");    
 */   
}

void DCE_PlayerModelLoadSwat(DCE_PlayerModel * mdl, char * body_txt, char * hands_txt)
{
	DCE_ParsePlayerHandsModelFrameList(mdl, hands_txt);
    DCE_ParsePlayerBodyModelFrameList(mdl, body_txt);
	
	mdl->body = swat_lod0;
	mdl->body_lod1 = swat_lod1;
	mdl->body_lod2 = swat_lod2;
	
	mdl->weapon = swat_deagle;	
	
	mdl->weapon_lod1 = swat_deagle_lod0;
	
    mdl->muzzleflash = muzflash;
				
	mdl->body_texID	= swat_tex;	
/*	
	mdl->body_texID	= backer_torso;//swat_tex;		
	mdl->head_texID = backer_head;	
	mdl->legs_texID = backer_legs;
*/			
	mdl->weapon_texID = DCE_TexID(DCE_TEXID_DEAGLE);

	mdl->hands = swat_hands;	
	mdl->hands_texID = swat_hands_tex;	
	
//	mdl->hands = clown_hands;	
//	mdl->hands_texID = clown_hands_tex;	
	
	mdl->blood = blood_mdl;
	mdl->blood_texID = blood_tex;
}

void DCE_PlayerModelLoadClown(DCE_PlayerModel * mdl, char * body_txt, char * hands_txt)
{
	DCE_ParsePlayerHandsModelFrameList(mdl, hands_txt);
    DCE_ParsePlayerBodyModelFrameList(mdl, body_txt);
	
	mdl->body = DCE_ModelClown();
	mdl->body_lod1 = NULL;
	mdl->body_lod2 = NULL;
		
	mdl->weapon = swat_deagle;	
	
	mdl->weapon_lod1 = swat_deagle_lod0;
	
    mdl->muzzleflash = muzflash;
	
	mdl->body_texID	= DCE_ClownTexID();	
							
	mdl->weapon_texID = DCE_TexID(DCE_TEXID_DEAGLE);

//	mdl->hands = swat_hands;	
//	mdl->hands_texID = swat_hands_tex;	
	
	mdl->hands = clown_hands;	
	mdl->hands_texID = clown_hands_tex;	
	
	mdl->blood = blood_mdl;
	mdl->blood_texID = blood_tex;
}

void DCE_InitPlayerState(DCE_Player * player)
{
    player->height = 60;
  
    player->perspective = DCE_PERSPECTIVE_FIRST;
    
    player->bullet_damage = 25.0f;
    
	player->health = 100.0f;
    
	player->state = PSTATE_IDLE;
	player->lstate = PSTATE_IDLE;
	
	player->state_frame = 0;
	player->last_moved = 0;
	player->light_on = 0;
	
	player->ammo = 30;
	player->clip_size = 15;
	player->clip = 15;
	
   	player->model.hands_index = HANDS_IDLE;
   	player->model.hands_frame = 0;
   	player->model.body_index = IDLE;
   	player->model.body_frame = 0;	
   	
   	player->model.weapon_texID = DCE_TexID(DCE_TEXID_DEAGLE);
   	player->model.weapon = swat_deagle;
   	player->model.weapon_lod1 = swat_deagle_lod0;
}
