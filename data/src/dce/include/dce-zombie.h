#ifndef DCE_ZOMBIE_H
#define DCE_ZOMBIE_H

#define DCE_ZOMBIE_STATE_IDLE    0
#define DCE_ZOMBIE_STATE_MOVE    1
#define DCE_ZOMBIE_STATE_ATTACK  2
#define DCE_ZOMBIE_STATE_DEATH_A 3
#define DCE_ZOMBIE_STATE_DEATH_B 4

#define DCE_ZOMBIE_MIN_FRAME_STAND 0
#define DCE_ZOMBIE_MAX_FRAME_STAND 12

#define DCE_ZOMBIE_MIN_FRAME_WALK  32 
#define DCE_ZOMBIE_MAX_FRAME_WALK  61

#define DCE_ZOMBIE_MIN_FRAME_WALK_REACH  62 
#define DCE_ZOMBIE_MAX_FRAME_WALK_REACH  86

#define DCE_ZOMBIE_MIN_FRAME_ATTACK  115 
#define DCE_ZOMBIE_MAX_FRAME_ATTACK  125

#define DCE_ZOMBIE_MIN_FRAME_DEATH_A 137
#define DCE_ZOMBIE_MAX_FRAME_DEATH_A 144

#define DCE_ZOMBIE_MIN_FRAME_DEATH_B 153
#define DCE_ZOMBIE_MAX_FRAME_DEATH_B 160

#define DCE_ZOMBIE_HIT_FRAME_1 116
#define DCE_ZOMBIE_HIT_FRAME_2 121

#define DCE_ZOMBIE_SCALE 1.3f

#define DCE_ZOMBIE_ATTACK_RADIUS 50.0f

#define DCE_ZOMBIE_COLLISION_RADIUS 5.0f

#define DCE_ZOMBIE_SPEED 1.5f

typedef struct
{
	vec3f_t position;
	vec3f_t lookAt;
	vec3f_t direction;
	vec3f_t shadow_position;
	
	float height;
	float health;
	
    unsigned int state;
	unsigned int lstate;
	unsigned int state_frame;
	unsigned int last_moved;
	unsigned char perspective;
	
	q1_mdl * model_head;
	q1_mdl * model_body;
	
	unsigned char min_frame;
	unsigned char max_frame;
	unsigned char cur_frame;
	unsigned char frame_mode;
	float         lerp_factor;

    vec3f_t bbox_min, bbox_max;
    vec3f_t hbbox_min, hbbox_max;
    	
} DCE_Zombie;

void DCE_ZombieInit(DCE_Zombie * zombie, vec3f_t * pos, vec3f_t * look_at);
void DCE_ZombieRender(DCE_Zombie * z);

void DCE_ZombieRenderCacheTorso(DCE_Zombie * z);
void DCE_ZombieRenderCacheHead(DCE_Zombie * z);
void DCE_ZombieRenderTorso(DCE_Zombie * z);
void DCE_ZombieRenderHead(DCE_Zombie * z);

void DCE_ZombieCollisionCallback(DCE_Zombie  * z);

void DCE_ZombieSetState(DCE_Zombie * zombie, unsigned char state);

#endif
