#ifndef DCE_ENEMY_H
#define DCE_ENEMY_H

#include "dce-player.h"

#define DCE_MAX_ENEMIES 32

#define DCE_ENEMY_EXECUTE_SPEED   0.20f
#define DCE_ENEMY_TURN_ANGLE       9.0f
#define DCE_ENEMY_NODE_PROXIMITY  30.0f
#define DCE_ENEMY_WAIT_MIN        30
#define DCE_ENEMY_WAIT_RANGE     150

typedef struct
{
	vec3f_t position;
	vec3f_t lookAt;
	vec3f_t direction;
	vec3f_t shadow_position;
	
	float height;
	float health;
	
	float fall_distance;
	
    unsigned int state;
	unsigned int lstate;
	unsigned int state_frame;
	unsigned int last_moved;
	unsigned char light_on;
	unsigned char perspective;
    
	DCE_PlayerModel model;
	    
    HUD_Sprite * flashlight_sprite;
    
    vec3f_t bbox_min, bbox_max;
    vec3f_t hbbox_min, hbbox_max;
    
    unsigned short int hud_health[4];
    unsigned short int hud_ammo[6];
    unsigned short int hud_crosshair;
    	
    unsigned short int ammo;	
    unsigned short int clip_size;
    unsigned short int clip;
    unsigned short int bullet_damage;
    	
    HUD_Sprite * hud_sprites[HUD_MAX_SPRITES];	
    
	// Enemy Specific Components
	vec3f_t src;        // Source Position
	vec3f_t dst;        // Destination Position
	float lerp_step;    // Pre-Computed Interpolation value based on distance between src and dst  
	float lerp_factor;  // Current Interpolation value
	
	float rotation;     // Current rotation for turning between nodes
	
	ubyte index;        // Index in Enemy Array
	
	ubyte marked;       // Boolean value of enemy being "marked" by player
	
	ubyte active;       // Weather or not the indexed enemy is active in the array
	
	ubyte attack;       // Boolean value of enemy being in an attack state
	
	ubyte alive;
	
	vec3f_t trigger;
		
} DCE_Enemy;

ubyte DCE_EnemyCount(); 
DCE_Enemy * DCE_EnemyPointer(ubyte index);

void DCE_EnemyRenderCallback(DCE_Player * player, float t);

void DCE_EnemyRenderCallbackTR(DCE_Player * player, DCE_Camera * cam);

void DCE_EnemyRenderCallbackShadow(DCE_Player * player);

void DCE_LoadEnemies(char * fname);

void DCE_EnemyRoutineCallback(DCE_Player * p, float t);

void DCE_EnemyTracePlayerRay(DCE_Player * player);

void DCE_EnemyTraceCameraRay(DCE_Camera * cam);

void DCE_PlayerRotateFacingEnemy(DCE_Player * p, DCE_Camera * c, DCE_Enemy * e);

float DCE_EnemyFOV();

unsigned char DCE_EnemyView();

unsigned char DCE_EnemyMarkedTargets();

unsigned char DCE_EnemyPlayerCollisionCallback(DCE_Player * player, vec3f_t * F, vec3f_t * T);

void DCE_EnemyTracePlayerMelee(DCE_Player * player);

void DCE_EnemyCheckPlayerPickup(DCE_Player * player);

md2_mdl * DCE_EnemyModelClown();

unsigned short DCE_ClownTexID();
md2_mdl * DCE_ModelClown();
md2_mdl * DCE_ModelHKUSP();

#endif
