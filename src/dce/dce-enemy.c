/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#include "dce.h"

#include "dce-enemy.h"

#include <math.h>

#define DCE_ENEMY_MDL_CLOWN   0x00
#define DCE_ENEMY_MDL_PANTY   0x01
#define DCE_ENEMY_MDL_GORILLA 0x02
#define DCE_ENEMY_MDL_SKIMASK 0x03

#define DCE_ENEMY_MDL_BLOOD   0x0F

#define DCE_ENEMY_MDL_HKUSP   0x10

#define DCE_ENEMY_MDL_MARKED  0x20

static ubyte DCE_ENEMIES = 0;

static ubyte DCE_ENEMY_MODELS = 0;

static DCE_Enemy DCE_ENEMY[DCE_MAX_ENEMIES];

static md2_mdl * DCE_ENEMY_MODEL[DCE_MAX_ENEMIES * 3];

static char DCE_ENEMY_MODEL_NAME[DCE_MAX_ENEMIES * 3][512];

static unsigned short DCE_ENEMY_MODEL_TEXID[DCE_MAX_ENEMIES * 3];

static unsigned char DCE_ENEMIES_KILLED = 0;
static unsigned char DCE_HEADSHOTS      = 0;

md2_mdl * DCE_EnemyModelClown()
{
	return DCE_ENEMY_MODEL[DCE_ENEMY_MDL_CLOWN];
}

/* Noop */
ubyte DCE_EnemyCount()
{
	return DCE_ENEMIES;
}

ubyte DCE_EnemyDeathCount()
{
	return DCE_ENEMIES_KILLED;
}

DCE_Enemy * DCE_EnemyPointer(ubyte index)
{
	return &DCE_ENEMY[index];
}

void DCE_EnemySetBBox(DCE_Enemy * player)
{
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

void DCE_LoadEnemyModels()
{
	DCE_ENEMY_MODEL[DCE_ENEMY_MDL_HKUSP] = DCE_LoadHQMD2("/cd/models/weapons/HK_usp.md2");
	DCE_ENEMY_MODEL_TEXID[DCE_ENEMY_MDL_HKUSP] = DCE_TexID(DCE_TEXID_HKUSP);
	
	DCE_ENEMY_MODEL[DCE_ENEMY_MDL_SKIMASK] = DCE_LoadHQMD2("/cd/models/skimask/skimask_lod0.md2");
	DCE_ENEMY_MODEL_TEXID[DCE_ENEMY_MDL_SKIMASK] = DCE_TextureLoadPVR("/cd/models/skimask/skimask_d.pvr");

	DCE_ENEMY_MODEL[DCE_ENEMY_MDL_PANTY] = DCE_LoadHQMD2("/cd/models/stocking/stocking_lod0.md2");
	DCE_ENEMY_MODEL_TEXID[DCE_ENEMY_MDL_PANTY] = DCE_TextureLoadPVR("/cd/models/stocking/stocking_d.pvr");

	DCE_ENEMY_MODEL[DCE_ENEMY_MDL_CLOWN] = DCE_LoadHQMD2("/cd/models/clown/clown_lod0.md2");
	DCE_ENEMY_MODEL_TEXID[DCE_ENEMY_MDL_CLOWN] = DCE_TextureLoadPVR("/cd/models/clown/clown_D2.pvr");
	
	DCE_ENEMY_MODEL_TEXID[DCE_ENEMY_MDL_MARKED] = DCE_TextureLoadPVR("/cd/textures/overlays/Flare__.pvr");
	
	DCE_ENEMY_MODEL[DCE_ENEMY_MDL_BLOOD] = DCE_LoadHQMD2("/cd/models/blood/blood_anim_01.md2");
	DCE_ENEMY_MODEL_TEXID[DCE_ENEMY_MDL_BLOOD] = DCE_TexID(DCE_BLOOD_MODEL);
}

unsigned short DCE_ClownTexID()
{
	return DCE_ENEMY_MODEL_TEXID[DCE_ENEMY_MDL_CLOWN];
}

md2_mdl * DCE_ModelClown()
{
	return DCE_ENEMY_MODEL[DCE_ENEMY_MDL_CLOWN];
}

md2_mdl * DCE_ModelHKUSP()
{
	return DCE_ENEMY_MODEL[DCE_ENEMY_MDL_HKUSP];
}

void DCE_SetClown(DCE_Enemy * e, unsigned char model_index, unsigned char weapon_index, unsigned char active)
{    
	e->height = 50;
	
	e->health = 100000.0f;
    
    e->bullet_damage = 50.0f;

    e->state = PSTATE_IDLE;		

    e->clip_size = 15;
    e->ammo = 15;
    e->clip = 15;

    e->model.blood = DCE_ENEMY_MODEL[DCE_ENEMY_MDL_BLOOD];
    e->model.blood_texID = DCE_ENEMY_MODEL_TEXID[DCE_ENEMY_MDL_BLOOD];
    
    DCE_PlayerModelLoad(&e->model,
	                    "/cd/models/clown/clown.txt",
	                    NULL,
                        DCE_ENEMY_MODEL[model_index], NULL, NULL,
						NULL,
						DCE_ENEMY_MODEL[weapon_index],
						DCE_ENEMY_MODEL_TEXID[model_index],
						0,
						DCE_ENEMY_MODEL_TEXID[weapon_index]);   
			
	float d;
	
	vec3f_distance(e->src.x, e->src.y, e->src.z, e->dst.x, e->dst.y, e->dst.z, d);
	
	e->lerp_step = 4.15f / d;
	e->lerp_factor = 0;
	e->rotation = 0;
						
	e->active = active;
	
	e->alive = 1;

	e->attack = 0;
	
	e->marked = 0;
}

void DCE_SetEnemy(DCE_Enemy * e, unsigned char model_index, unsigned char weapon_index, unsigned char active)
{    
	e->height = 50;
	
	e->health = 100.0f;
    
    e->bullet_damage = 10.0f;

    e->state = PSTATE_IDLE;		

    e->clip_size = 15;
    e->ammo = 15;
    e->clip = 15;

    e->model.blood = DCE_ENEMY_MODEL[DCE_ENEMY_MDL_BLOOD];
    e->model.blood_texID = DCE_ENEMY_MODEL_TEXID[DCE_ENEMY_MDL_BLOOD];
    
    DCE_PlayerModelLoad(&e->model,
	                    "/cd/models/stocking/stocking.txt",
	                    NULL,
                        DCE_ENEMY_MODEL[model_index], NULL, NULL,
						NULL,
						DCE_ENEMY_MODEL[weapon_index],
						DCE_ENEMY_MODEL_TEXID[model_index],
						0,
						DCE_ENEMY_MODEL_TEXID[weapon_index]);   
			
	float d;
	
	vec3f_distance(e->src.x, e->src.y, e->src.z, e->dst.x, e->dst.y, e->dst.z, d);
	
	e->lerp_step = 4.15f / d;
	e->lerp_factor = 0;
	e->rotation = 0;
						
	e->active = active;
	
	e->alive = 1;

	e->attack = 0;
	
	e->marked = 0;
}

unsigned short int DCE_BufNextVec3Offset(char * buf, unsigned int len)
{
	int i;
	for(i = 0; i < len; i++)
		if(buf[i] == '{')
			return i;
	return 0;
}

void DCE_BufReadVec3f(char * buf, unsigned int len,  vec3f_t * P)
{
    unsigned short int pos = DCE_BufNextVec3Offset(buf, len); 
	char c[1];

	sscanf(&buf[pos + 1], "%f%c %f%c %f", &P->x, c, &P->y, c, &P->z);
}

void DCE_LoadEnemies(char * fname)
{
    DCE_ENEMIES = 0;
    
    FILE * f = fopen(fname, "rb");
    if(f == NULL)
    {
    	printf("ERROR: %s\n", fname);
    	return;
	}
	
	char buf[128];
	char id[32];
	
    while(fgets(buf, 128, f))
	    if(buf[0] == '{')
			break;
	
	unsigned char model_index = 0, weapon_index = 0;
	unsigned int active = 0;
	
    while(fgets(buf, 128, f))
    {
	    if(buf[0] == '}') // end of entry
	    {
		    if(model_index == DCE_ENEMY_MDL_CLOWN)
			    DCE_SetClown(DCE_EnemyPointer(DCE_ENEMIES), model_index, weapon_index, active & 0xFF);
			else
			    DCE_SetEnemy(DCE_EnemyPointer(DCE_ENEMIES), model_index, weapon_index, active & 0xFF);
							
			DCE_EnemyPointer(DCE_ENEMIES)->index = DCE_ENEMIES;
			
			DCE_ENEMIES++;
		}
		
		switch(buf[4])
		{
			case 'm': // model
			    switch(buf[12])
			    {
			    	case 'r': // robber
			    	    switch(buf[19])
			    	    {
			    	    	case 's': // skimask
			    	    	    model_index = DCE_ENEMY_MDL_SKIMASK;
								break;
							case 'c':
								model_index = DCE_ENEMY_MDL_CLOWN;
								break;
							case 'p':
								model_index = DCE_ENEMY_MDL_PANTY;
								break;
						}
						break;
				}
				break;
			
			case 'w': // weapon
			    switch(buf[13])
			    {
			    	case 'H': 
			        case 'h': // HK_USP
			            weapon_index = DCE_ENEMY_MDL_HKUSP;
			            break;
				}
				break;
				
			case 'a': // active state
			    sscanf(buf, "%s %s %i", id, id, &active);
				break;
				
			case 's': // src
			    DCE_BufReadVec3f(buf, 128,  &DCE_EnemyPointer(DCE_ENEMIES)->src);
			    vec3f_copy(&DCE_EnemyPointer(DCE_ENEMIES)->src, &DCE_EnemyPointer(DCE_ENEMIES)->position);
			    break;

			case 'd': // dst
			    DCE_BufReadVec3f(buf, 128,  &DCE_EnemyPointer(DCE_ENEMIES)->dst);
			    vec3f_copy(&DCE_EnemyPointer(DCE_ENEMIES)->dst, &DCE_EnemyPointer(DCE_ENEMIES)->lookAt);
			    break;

			case 't': // trigger
			    DCE_BufReadVec3f(buf, 128,  &DCE_EnemyPointer(DCE_ENEMIES)->trigger);
			    break;			    
		}
    }
    
    fclose(f);
}

void DCE_EnemyEndFrame(DCE_Enemy * player, unsigned int render_frame, float t)
{
	if(player->state & PSTATE_DEAD)
	{
		if(player->state_frame++ > 300)
		    player->active = 0;
		    
		if(player->model.body_frame == player->model.body_frame_count[player->model.body_index] - 1)
		    return;
	}
	
	float tf = t * render_frame;
	int rf = (int)tf;
	
	if(!(rf % (30 / player->model.body_fps)))
	{
		++player->model.body_frame;
						
		if(player->model.body_frame >= player->model.body_frame_count[player->model.body_index])
		    player->model.body_frame = 0;
	}
	
	++player->state_frame;
}

unsigned char DCE_AnimationFinished(DCE_Player * p)
{
	return p->model.body_index = 0;
}

unsigned char DCE_AnimationSetFrameAI(DCE_Player * p, unsigned char frame)
{
	if(p->model.body_index != frame)
	{
		p->model.body_index = frame;
		p->model.body_frame = 0;
		
		return 1;
	}
	
	return 0;
}

void DCE_EnemyStartFrame(DCE_Enemy * e)
{
//	if(!DCE_AnimationFinished(e))
//	    return;
	    
	if(e->state & PSTATE_DEAD)
	{
		DCE_AnimationSetFrameAI(e, DEATH1);
		
		return;
	}

	if(e->state & PSTATE_HURT)
	{
		if(!DCE_AnimationSetFrameAI(e, DAMAGE))
		{
		    if(e->model.body_frame == e->model.body_frame_count[e->model.body_index] - 1)
		    {
		    	e->state &= ~PSTATE_HURT;
		    	
				if(!e->attack)
                {

				    e->state |= PSTATE_ATTACK;
    	            e->attack = 1;
   	
    	            vec3f_sub_normalize(DCE_PlayerPointer(0)->position.x, DCE_PlayerPointer(0)->position.y, DCE_PlayerPointer(0)->position.z,
    	                    e->position.x, e->position.y, e->position.z,
		                    e->direction.x, e->direction.y, e->direction.z);
		
		            vec3f_add(&e->direction, &e->position, &e->lookAt);
		
		           DCE_SfxPlayAttenuation(WEAPON_FIRE, &e->position, &DCE_PlayerPointer(0)->position);
		        }
		    }  
		}  		
		return;
	}
	
    if(e->state & PSTATE_MELEE)
    {
		if(!DCE_AnimationSetFrameAI(e, MELEE1))
		{
		    if(e->model.body_frame == 14)
		    {
		    	e->state &= ~PSTATE_MELEE;
		    }  
		} 
		
		return;	
	}

    if((e->state & PSTATE_WALK_FORWARD) || (e->state & PSTATE_MOVE))
    {
    	if(e->attack)
    	{
		    if(DCE_AnimationSetFrameAI(e, WALK_FIRE))
		        DCE_SfxPlayAttenuation(WEAPON_FIRE, &e->position, &DCE_PlayerPointer(0)->position);
		}
		else
		
		    DCE_AnimationSetFrameAI(e, WALK_FORWARD);
	}
    else if((e->state & PSTATE_TURNING) && e->state_frame > e->last_moved)
    {
    	DCE_AnimationSetFrameAI(e, WALK_STRAFE_RIGHT);
	}	
	else
	{
    	DCE_AnimationSetFrameAI(e, IDLE);		
	}
	
/*
 	if(player->state & PSTATE_DEAD)
    {
    	if(player->model.body_index != DEATH1 && player->model.body_index != DEATH2)
    	{
    		player->model.body_index = DEATH1 + (rand() % 1);
    		player->model.body_frame = 0;
		}
		
	    return;
	}   
    
    if(player->state & PSTATE_MELEE)
    {
    	if(player->model.body_index != MELEE1)
    	{
    		player->model.body_index = MELEE1;
    		player->model.body_frame = 0;
		}    	
	}
    else if(player->attack)
    {
		    if(player->model.body_index != WALK_FIRE)
		    {
			    player->model.body_index = WALK_FIRE;
			    player->model.body_frame = 0;
			    
			    //player->state_frame = 0;
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
    else if(player->state & PSTATE_TURNING && player->state_frame > player->last_moved)
    {
    	if(player->model.body_index != WALK_STRAFE_RIGHT)
    	{
    		player->model.body_index = WALK_STRAFE_RIGHT;
    		player->model.body_frame = 0;
		}
	}	
	else if(player->model.body_index != IDLE)
    {
    	player->model.body_index = IDLE;
    	player->model.body_frame = 0;
   }
*/	
/*
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
	}
    else if(player->state & PSTATE_RELOAD)
    {
    	if(player->model.body_index != RELOAD)
    	{
    		player->model.body_index = RELOAD;
    		player->model.body_frame = 0;
      
		}
	}
	else if((player->state & PSTATE_FIRE1))
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
			    
			    player->state_frame = 0;
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
*/
}

void DCE_EnemyTraceCameraRay(DCE_Camera * cam)
{
	unsigned char i, c = 0;

	for(i = 0; i < DCE_ENEMIES; i++)
	{
		if(DCE_EnemyPointer(i)->active)
		{
			c = DCE_TraceRayBBox(&cam->pos, &cam->dir, &DCE_EnemyPointer(i)->bbox_min, &DCE_EnemyPointer(i)->bbox_max);
            
			c |= DCE_TraceRayBBox(&cam->pos, &cam->dir, &DCE_EnemyPointer(i)->hbbox_min, &DCE_EnemyPointer(i)->hbbox_max) << 1;
            
			if(c)
			{
                if(!DCE_EnemyPointer(i)->marked )
                    DCE_SfxPlay(WEAPON_EMPTY);
                    
			    DCE_EnemyPointer(i)->marked = 1;
            }
		}
	}
}

extern unsigned char DCE_VaultIsClear();

void DCE_EnemyTracePlayerMelee(DCE_Player * player)
{
	unsigned char i, c;

	for(i = 0; i < DCE_ENEMIES; i++)
	{
		if(DCE_EnemyPointer(i)->active && !(DCE_EnemyPointer(i)->state & PSTATE_DEAD))
		{
			c = DCE_PlayerTraceRayBBox(player, &DCE_EnemyPointer(i)->bbox_min, &DCE_EnemyPointer(i)->bbox_max);
            
			c |= DCE_PlayerTraceRayBBox(player, &DCE_EnemyPointer(i)->hbbox_min, &DCE_EnemyPointer(i)->hbbox_max) << 1;
            
			if(c && DCE_PlayerMeleeCollision(player, (DCE_Player *)DCE_EnemyPointer(i)))
            {
            	//DCE_RenderInsertDecal(player, DCE_TexID(DCE_TEXID_BLOOD), 5.0f);
				
				DCE_SfxPlay(IMPACT_FLESH1);
				    
				DCE_EnemyPointer(i)->state |= PSTATE_HURT;
				
				if(!DCE_VaultIsClear() && i > 5)
				{
				    DCE_EnemyPointer(i)->health -= 1;
				    if(c & (1<<1))
				       DCE_EnemyPointer(i)->health -= 1; // Headshot = extra damage
				}  
				else
				{
				    DCE_EnemyPointer(i)->health -= player->bullet_damage;
				    if(c & (1<<1))
				       DCE_EnemyPointer(i)->health -= player->bullet_damage * .5; // Headshot = extra damage
				}    
				
				if(DCE_EnemyPointer(i)->health <= 0)
				{
				    DCE_EnemyPointer(i)->state |= PSTATE_DEAD;
					    
					DCE_EnemyPointer(i)->state_frame = 0;
				}
			}
		}
	}
}

void DCE_EnemyTracePlayerRay(DCE_Player * player)
{
	unsigned char i, c;
	
	for(i = 0; i < DCE_ENEMIES; i++)
	{
		if(DCE_EnemyPointer(i)->active && !(DCE_EnemyPointer(i)->state & PSTATE_DEAD))
		{
			c = DCE_PlayerTraceRayBBox(player, &DCE_EnemyPointer(i)->bbox_min, &DCE_EnemyPointer(i)->bbox_max);
            
			c |= DCE_PlayerTraceRayBBox(player, &DCE_EnemyPointer(i)->hbbox_min, &DCE_EnemyPointer(i)->hbbox_max) << 1;
            
			if(c)
            {
            	DCE_RenderInsertDecal(player, DCE_TexID(DCE_TEXID_BLOOD), 5.0f);
				    
				DCE_EnemyPointer(i)->state |= PSTATE_HURT;
				
				if(!DCE_VaultIsClear() && i > 5)
				{
				    DCE_EnemyPointer(i)->health -= 1;
				    if(c & (1<<1))
				       DCE_EnemyPointer(i)->health -= 1; // Headshot = extra damage
				}    
				else
				{
				    DCE_EnemyPointer(i)->health -= player->bullet_damage;
				    if(c & (1<<1))
				       DCE_EnemyPointer(i)->health -= player->bullet_damage * .5; // Headshot = extra damage
				}  
				    
				if(DCE_EnemyPointer(i)->health <= 0)
				{
				    DCE_EnemyPointer(i)->state |= PSTATE_DEAD;
					
					++DCE_ENEMIES_KILLED;
					    
					DCE_EnemyPointer(i)->state_frame = 0;
				}
			}
		}
	}
}

void DCE_EnemyCheckPlayerPickup(DCE_Player * player)
{
	unsigned char i, c;

	for(i = 0; i < DCE_ENEMIES; i++)
	{
		if(DCE_EnemyPointer(i)->active && (DCE_EnemyPointer(i)->state & PSTATE_DEAD))
		{
			if((DCE_EnemyPointer(i)->model.weapon_lod1 != NULL) && DCE_PlayerMeleeCollision(player, (DCE_Player *)DCE_EnemyPointer(i)))
            {
				DCE_SfxPlay(WEAPON_RELOAD);
				    
				player->ammo += 10;
				
				if(player->ammo > 120)
				   player->ammo = 120;
				
				DCE_EnemyPointer(i)->model.weapon_lod1 = NULL;
				DCE_EnemyPointer(i)->marked = 0;
			}
		}
	}
}



void DCE_EnemyGravityCallback(DCE_Enemy * e)
{
	vec3f_t F, T;
    float gd;    
    
	vec3f_copy(&e->position, &F);
	vec3f_copy(&e->lookAt, &T);
	
    /* Apply Gravity Collision For Enemy Against World Geometry */
    if(DCE_CheckGravityCollisionCBO(&e->position, &gd))
    {
        if(gd < 30)
        {
            vec3f_copy(&F, &e->position);
            vec3f_copy(&T, &e->lookAt);                   	
	    }
		else
		{
		    e->shadow_position.x = e->position.x;
		    e->shadow_position.y = e->position.y - gd + 1.0f;
		   	e->shadow_position.z = e->position.z;
			
		   	e->position.y = e->position.y - gd + e->height;
		
		    e->lookAt.y   = e->lookAt.y - gd + e->height;
	   	}
	} 	
}


static float fov = 0;

static unsigned char view = 0;

float DCE_EnemyFOV()
{
	return fov;
}

unsigned char DCE_EnemyView()
{
	return view;
}

unsigned char DCE_EnemyViewsPlayer(DCE_Enemy * e, DCE_Player * p)
{
	vec3f_t D, V; // Direction Vectors
	float cd, d;  // Collision Distance and Player Distance

    // Normalize and Compute Enemy Looking Direction as D
	vec3f_sub(&e->position, &e->lookAt, &D);
	vec3f_normalize_c(&D);
	/*
	vec3f_sub_normalize(e->lookAt.x, e->lookAt.y, e->lookAt.z,
	                    e->position.x, e->position.y, e->position.z,
						D.x, D.y, D.z);
	*/				
	/* Normalize and Compute Enemy to Player Direction as V
	vec3f_sub_normalize(p->position.x, p->position.y, p->position.z,
	                    e->position.x, e->position.y, e->position.z,
						V.x, V.y, V.z);	
	*/
	vec3f_sub(&e->position, &p->position, &V);
	vec3f_normalize_c(&V);
	
	// Dot product of D and V gives us the cosine of the angle to determine FOV inlucsion					
	//vec3f_dot(D.x, 0, D.z, V.x, 0, V.z, d);
    
    d = ((D.x * V.x) + (D.z * V.z));
    //d = sqrt(d);
    
    fov = d;
    
    // Return false if the Player is outside of the Enemy FOV
	if(d < 0.01)
	{
        view  = 0;
	    return 0;
	}
	// Get distance from enemy to player    
	vec3f_distance(e->position.x, e->position.y, e->position.z, p->position.x, p->position.y, p->position.z, d);
	
	// Get collision distance from enemy to world collision
	DCE_CheckCollisionCBO(&e->position, &V, &cd);
	
	// Return True if distance from enemy to player is less than collision distance ( world blocks view )
	view = 1;
	return d < cd;
}

#define DCE_ENEMY_MOVE_SPEED 10.0f

void DCE_EnemyAttackCallback(DCE_Enemy * e, DCE_Player * p, float t)
{
    if(e->state & PSTATE_DEAD) // Exit callback if enemy is dead
        return;
        
	vec3f_t D, V;
	float d;
	
	DCE_EnemyGravityCallback(e);
	
	DCE_EnemySetBBox(e);
	
	// Get distance from enemy to player    
	vec3f_distance(e->position.x, e->position.y, e->position.z, p->position.x, p->position.y, p->position.z, d);
	
	if((e->state & PSTATE_HURT))
	    return;
	    
	if(d < DCE_PLAYER_COLLISION_DISTANCE)
	{
	    e->state |= PSTATE_MELEE;
	    e->state &= ~PSTATE_MOVE;    
	    
	    if(e->model.body_frame == 0 && e->state_frame %2 == 0)
	    {
	    	DCE_SfxPlayAttenuation(ENEMY_MELEE, &e->position, &p->position);
		}
		
	    if(e->model.body_frame == 4 && e->state_frame %2 == 0)
	    {
	    	p->health -= e->bullet_damage * 1.5f;
	    	    	
	    	DCE_SfxPlay(IMPACT_FLESH1);
	    	
	    	if(p->health <= 0)
	    	{
	    	    p->health = 0;
			    return;
			}
		}
	}
    else // distance is too far to melee attack
	{
		if(e->state & PSTATE_MELEE)
		    return;
		//    goto no_move;
		    
		e->state |= PSTATE_MOVE;
        
        if(e->model.body_frame % 5 == 0 && e->state_frame % 2 == 0)
           DCE_SfxPlayAttenuation(ENEMY_WALK1 + (rand() % 5), &e->position, &p->position);

	    if(e->model.body_frame == 0 && e->state_frame % 2 == 0)
	    {
	    	DCE_SfxPlayAttenuation(WEAPON_FIRE, &e->position, &p->position);
		}

	    if(e->model.body_frame == 4 && e->state_frame % 2  == 0)
	    {
	    	p->health -= e->bullet_damage;
	    	    	
	    	DCE_SfxPlay(WEAPON_HIT);
	    	
	    	if(p->health <= 0)
	    	{
	    	    p->health = 0;
			    return;
			}
		}

		vec3f_copy(&p->position, &e->lookAt);
		
		// Compute and Normalize Enemy Looking Direction
		vec3f_sub_normalize(p->position.x, p->position.y, p->position.z,
	                        e->position.x, e->position.y, e->position.z,
						    e->direction.x, e->direction.y, e->direction.z);
						    
		// Get World Collision Distance in Direction enemy is looking
		DCE_CheckCollisionCBO(&e->shadow_position, &e->direction, &d);				    
		
		// If World Collision Distance is less than move speed, "slide" along collided surface
		if(d < DCE_ENEMY_MOVE_SPEED * t)
		{
			vec3f_mul_scalar(&e->direction, d, &V);
			
			vec3f_add(&e->position, &V, &e->position);
			
			vec3f_copy(&e->direction, &V);
			
			vec3f_add(DCE_CollisionModelNormal(), &V, &V);
			
			vec3f_mul_scalar(&V, (DCE_ENEMY_MOVE_SPEED * t) - d, &V);
			
			vec3f_add(&V, &e->position, &V);
		}
        else
        {
		    vec3f_mul_scalar(&e->direction, DCE_ENEMY_MOVE_SPEED * t, &V);
		
		    vec3f_add(&e->position, &V, &e->position);
	    } 
	    
	    no_move:
	    	;
	}
}

unsigned char DCE_EnemyViewCallback(DCE_Enemy * e, DCE_Player * p)
{
    if(DCE_EnemyViewsPlayer(e, p))
    {
    	/**/
		e->state |= PSTATE_ATTACK;
    	
    	e->attack = 1;
   	
    	vec3f_sub_normalize(p->position.x, p->position.y, p->position.z,
    	                    e->position.x, e->position.y, e->position.z,
		                    e->direction.x, e->direction.y, e->direction.z);
		
		vec3f_add(&e->direction, &e->position, &e->lookAt);
		
		DCE_SfxPlayAttenuation(WEAPON_FIRE, &e->position, &p->position);
		
		return 1;
	}
	
	return 0;
}

void DCE_EnemyPatrolCallback(DCE_Enemy * e, DCE_Player * p, float t)
{
    if(e->state & PSTATE_DEAD) // Exit callback if enemy is dead
        return;
        
    if(e->state & PSTATE_HURT)
        return;
        
    if(DCE_EnemyViewCallback(e, p))
        return;
       
	vec3f_t F, T;
    float d;
    
	vec3f_copy(&e->position, &F);
	vec3f_copy(&e->lookAt, &T);
        
	if(e->state & PSTATE_TURNING)
	{
		if(e->rotation >= 180)
		{
			e->rotation = 0;
			
			e->state &= ~PSTATE_TURNING;
			
			vec3f_copy(&e->dst, &e->lookAt);
		}
		else
		{
			if(e->state_frame++ >= e->last_moved)
			{
		        if(e->model.body_frame % 2 == 0 && e->state_frame % 2 == 0)
		           DCE_SfxPlayAttenuation(ENEMY_WALK1 + (rand() % 5), &e->position, &p->position);
		    
			    e->rotation += DCE_ENEMY_TURN_ANGLE * t;
			
			    vec3f_rotd_xz(e->lookAt.x, e->lookAt.y, e->lookAt.z,
			                  e->position.x, e->position.y, e->position.z,
			                  DCE_ENEMY_TURN_ANGLE * t);
			             
			    vec3f_sub_normalize(e->lookAt.x, e->lookAt.y, e->lookAt.z,
			                        e->position.x, e->position.y, e->position.z,
								    e->direction.x, e->direction.y, e->direction.z);
			}
		}
	}
	else
	{
		if(e->model.body_frame % 5 == 0 && e->state_frame % 2 == 0)
		    DCE_SfxPlayAttenuation(ENEMY_WALK1 + (rand() % 5), &e->position, &p->position);
		    
		e->state |= PSTATE_WALK_FORWARD;
		
		e->lerp_factor += e->lerp_step * t;
		
		vec3f_lerp(&e->src, &e->dst, &e->position, e->lerp_factor);
		
		vec3f_distance(e->position.x, e->position.y, e->position.z, 
		               e->dst.x, e->dst.y, e->dst.z, d);
		
		if(d < DCE_ENEMY_NODE_PROXIMITY)
		{
			e->state &= ~PSTATE_WALK_FORWARD;
			
			vec3f_switch(&e->src, &e->dst);
			
			e->lerp_factor = 1.0f - e->lerp_factor;
			
			e->state |= PSTATE_TURNING;
			
			e->state_frame = 0;
			
			e->last_moved = DCE_ENEMY_WAIT_MIN + (rand() % DCE_ENEMY_WAIT_RANGE);
		}
	}

    DCE_EnemyGravityCallback(e);
	
	DCE_EnemySetBBox(e);
}


void DCE_EnemyRoutineCallback(DCE_Player * p, float t)
{
	unsigned char i, attack = 0;
	
	for(i = 0; i < DCE_ENEMIES; i++)
	{
		if(DCE_EnemyPointer(i)->active)
		{
		    if(!DCE_EnemyPointer(i)->attack)
		    {
			    DCE_EnemyPatrolCallback(DCE_EnemyPointer(i), p, t);
			}
			
			if(DCE_EnemyPointer(i)->attack)
			{  
		        DCE_EnemyAttackCallback(DCE_EnemyPointer(i), p, t);		    
	
	            ++attack;
	        } 
		}
		else
		{
			float d;
			vec3f_distance(p->position.x, p->position.y, p->position.z,
			               DCE_EnemyPointer(i)->trigger.x, DCE_EnemyPointer(i)->trigger.y, DCE_EnemyPointer(i)->trigger.z, d);
			
			if(d < 50.0f)
			    if(!(DCE_EnemyPointer(i)->state & PSTATE_DEAD))
			        DCE_EnemyPointer(i)->active = 1;
		}
    }
	if(attack)
	{
		//DCE_PlayCDDA(ASHES_HIGH_OCTANE);
	}
	else
	{
		//DCE_PlayCDDA(DYSTOPIA);
	}
}

static unsigned long render_frame = 0;

void DCE_EnemyRenderCallback(DCE_Player * player, float t)
{
	unsigned char i;
	
	for(i = 0; i < DCE_ENEMIES; i++)
	{
		if(DCE_EnemyPointer(i)->active)
		{
		    DCE_EnemyStartFrame(DCE_EnemyPointer(i));
		        
			DCE_RenderEnemyLOD(player, (DCE_Player *)DCE_EnemyPointer(i));
			
			DCE_EnemyEndFrame(DCE_EnemyPointer(i), render_frame, t);
			
			DCE_EnemyCheckPlayerPickup(player);
		}
	}
	
	++render_frame;
}

unsigned char DCE_EnemyMarkedTargets()
{
	unsigned char i, m = 0;
	
	for(i = 0; i < DCE_ENEMIES; i++)
		if(DCE_EnemyPointer(i)->active && DCE_EnemyPointer(i)->marked)
		    ++m;
		    
    return m;
}

unsigned char DCE_EnemyPlayerCollisionCallback(DCE_Player * player, vec3f_t * F, vec3f_t * T)
{
	unsigned char i;
	
	for(i = 0; i < DCE_ENEMIES; i++)
		if(DCE_EnemyPointer(i)->active && !(DCE_EnemyPointer(i)->state & PSTATE_DEAD))
		    if(DCE_PlayerCollision(player, DCE_EnemyPointer(i)))
		    {
		        vec3f_copy(F, &player->position);
                vec3f_copy(T, &player->lookAt);
                
                return 1;
			}
	
	return 0;
}

void DCE_EnemyRenderCallbackShadow(DCE_Player * player)
{
	unsigned char i;
	
	for(i = 0; i < DCE_ENEMIES; i++)
		if(DCE_EnemyPointer(i)->active)
		    if(!(DCE_EnemyPointer(i)->state & PSTATE_DEAD))
			    DCE_RenderCheapShadow(DCE_TexID(DCE_TEXID_SHADOW1), &DCE_EnemyPointer(i)->shadow_position, 20.0f);
}

void DCE_EnemyRenderCallbackTR(DCE_Player * player, DCE_Camera * cam)
{
	unsigned char i;
	
	for(i = 0; i < DCE_ENEMIES; i++)
	{
		if(DCE_EnemyPointer(i)->active)
		{
		    DCE_RenderEnemyLODTR(player, DCE_EnemyPointer(i));
		    
			if(DCE_EnemyPointer(i)->marked)
		    {
		    	vec3f_t R = { player->direction.x, 0, player->direction.z };
		        
		        if(player->perspective == DCE_PERSPECTIVE_STATIC)
		        {
		    	    R.x = cam->dir.x;
		    	    R.z = cam->dir.z;
			    }
		    
		        vec3f_rotr_xz(R.x, R.y, R.z, 0, 0, 0, DEG2RAD * 90.0f);
		    
                DCE_RenderTesselatePointSprite(&DCE_EnemyPointer(i)->shadow_position, &R, 
				                        DCE_ENEMY_MODEL_TEXID[DCE_ENEMY_MDL_MARKED], 0xFFFF1F1F);   
			}
		}
	}
}

static unsigned RF = 0;

void DCE_PlayerRotateFacingEnemy(DCE_Player * p, DCE_Camera * c, DCE_Enemy * e)
{
	float r = 0;

	vec3f_sub(&p->position, &p->lookAt, &p->direction);
	
    vec3f_normalize(p->direction.x, p->direction.y, p->direction.z);

	float a = vec3f_vectors_to_ray_deg(&p->position, &p->lookAt, &e->position);
	
	float ts = a / (11.25f);
	
	if(a < 0) 
	    p->state |= PSTATE_WALK_STRAFE_LEFT;
	else
	    p->state |= PSTATE_WALK_STRAFE_RIGHT;	   
	   
	while(1)
	{
		vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		              p->position.x, p->position.y, p->position.z,
		              ts * RADIAN);
		            
		vec3f_sub(&p->position, &p->lookAt, &p->direction);
	
	    vec3f_normalize(p->direction.x, p->direction.y, p->direction.z);
					 
    	DCE_EnemyRoutineCallback(p, DCE_ENEMY_EXECUTE_SPEED);
    	
    	DCE_PlayerAnimationStartFrame(p);   
    	
    	DCE_RenderCallback(p, c, DCE_ENEMY_EXECUTE_SPEED);
    	
    	DCE_PlayerAnimationEndFrame(p, RF++); 	
		
		r += ts;	
		
		if(fabs(r) >= fabs(a))
		    break;
	}
	
	p->state &= ~PSTATE_WALK_STRAFE_LEFT;
	p->state &= ~PSTATE_WALK_STRAFE_RIGHT;
}
