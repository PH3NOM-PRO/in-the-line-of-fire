/* 
** DCE - Dreamcast Engine (C) 2013-2015 Josh PH3NOM Pearson
*/

#ifndef DCE_PLAYER_H
#define DCE_PLAYER_H

#include "dce-vector.h"
#include "dce-obj.h"
#include "md2.h"
#include "dce-hud.h"

#define PSTATE_IDLE              1<<0
#define PSTATE_RUN               1<<1
#define PSTATE_JUMP              1<<2
#define PSTATE_FALL              1<<3
#define PSTATE_ATTACK            1<<4
#define PSTATE_HURT              1<<5
#define PSTATE_DUCK              1<<6
#define PSTATE_LIGHT             1<<7
#define PSTATE_FIRE1             1<<8
#define PSTATE_FIRE2             1<<9
#define PSTATE_MOVE              1<<10
#define PSTATE_RELOAD            1<<11
#define PSTATE_DEAD              1<<12
#define PSTATE_PAIN              1<<13
#define PSTATE_WALK_FORWARD      1<<14
#define PSTATE_WALK_BACKWARD     1<<15
#define PSTATE_WALK_STRAFE_LEFT  1<<16
#define PSTATE_WALK_STRAFE_RIGHT 1<<17 
#define PSTATE_TURNING           1<<18
#define PSTATE_MELEE             1<<19
#define PSTATE_WEAPON_SWITCH     1<<20
#define PSTATE_HOLDING           1<<21

#define BBOX_SCALE 1.47f
#define BBOX_H1 17 * BBOX_SCALE
#define BBOX_H2 24 * BBOX_SCALE
#define BBOX_W 11 * BBOX_SCALE
#define HBOX_H 8 * BBOX_SCALE
#define HBOX_W 5 * BBOX_SCALE

#define DCE_PERSPECTIVE_FIRST  0x0
#define DCE_PERSPECTIVE_THIRD  0x1
#define DCE_PERSPECTIVE_STATIC 0x2

enum
{
	IDLE = 0,
	EQUIP,
	HOLSTER,
	FIRE,
	MELEE1,
	MELEE2,
	RELOAD,
	DAMAGE,
	CROUCH_FORWARD,
	CROUCH_BACKWARD,
	WALK_FORWARD,
	WALK_BACKWARD,
	WALK_STRAFE_LEFT,
	WALK_STRAFE_RIGHT,
	RUN_FORWARD,
	RUN_BACKWARD,
	RUN_STRAFE_LEFT,
	RUN_STRAFE_RIGHT,
	DEATH1,
	DEATH2,
	CROUCH_IDLE,
	WALK_FIRE
} DCE_PlayerFrame;

enum
{
	HANDS_IDLE = 0,
	HANDS_EQUIP,
	HANDS_HOLSTER,
	HANDS_FIRE,
	HANDS_MELEE1,
	HANDS_MELEE2,
	HANDS_RELOAD,
	HANDS_WALK,
    HANDS_RUN
} DCE_HandsFrame;

#define DCE_PLAYER_FRAMES 22
#define DCE_WEAPON_FRAMES  9

typedef unsigned char ubyte;

typedef struct
{
	md2_mdl * body;                 // Third Person Player Model
	md2_mdl * body_lod1;            // Level Of Deatail 1
	md2_mdl * body_lod2;            // Level Of Detail 2
	uint16    body_texID;
	uint16    head_texID;
	uint16    legs_texID;
	
	md2_mdl * weapon;               // Third Person Player Weapon Model
	md2_mdl * weapon_lod1;
	md2_mdl * weapon_lod2;
	uint16    weapon_texID;

	md2_mdl * weapon_attachment;
	uint16    weapon_attachment_texID;          // Weapon Attachment Model
		
	md2_mdl * hands;                // First Person Player Hands Model
	uint16    hands_texID;
	
	md2_mdl * blood;
	uint16    blood_texID;          // Blood Model

	md2_mdl * muzzleflash;         // MuzzleFlash
	uint16    muzzleflash_texID;
		
	ubyte body_fps;
	ubyte hands_fps;
	
	DCE_SimpleVertex * body_buf;    // Vertex Buffer for 1 frame of Player Model
	DCE_SimpleVertex * weapon_buf;  // Vertex Buffer for 1 frame of Weapon Model
	DCE_SimpleVertex * hands_buf;   // Vertex Buffer for 1 frame of Player Hands Model
	
	ubyte * body_frames[DCE_PLAYER_FRAMES];     // Frame Indices for Player Model
	ubyte   body_frame;                         // Frame Index for Player Model ( Weapon is synchronized )
	ubyte   body_index;	
	ubyte   body_frame_count[DCE_PLAYER_FRAMES];
	
	ubyte * hands_frames[DCE_WEAPON_FRAMES];     // Frame Indices for Player Model
	ubyte   hands_frame;                         // Frame Index for Player Model ( Weapon is synchronized )
	ubyte   hands_index;	
	ubyte   hands_frame_count[DCE_WEAPON_FRAMES];
	
} DCE_PlayerModel;

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
    	
} DCE_Player;

DCE_Player * DCE_PlayerPointer(unsigned char index);

void DCE_InitPlayerHUD(DCE_Player * player);
int DCE_HudInsertPlayerSprite(DCE_Player * player, HUD_Sprite * sprite);
unsigned char DCE_HudRemovePlayerSprite(DCE_Player * player, unsigned short index);
unsigned char DCE_HudDeletePlayerSprite(DCE_Player * player, unsigned short index);
void DCE_HudPlayerRenderCallback(DCE_Player * player);
void DCE_HudSetPlayerHealth(DCE_Player * player);
void DCE_HudSetPlayerAmmo(DCE_Player * player);

void DCE_InitPlayer(DCE_Player * player);
void DCE_InitPlayerState(DCE_Player * player);
void DCE_ReInitPlayer(DCE_Player * player);

void DCE_SetPlayerLookAtMatrix(DCE_Player * player);

void DCE_PlayerComputeRenderMatrix(DCE_Player * p, unsigned char player_num);
void DCE_PlayerLoadRenderMatrix(unsigned char player_num);

void DCE_RenderPlayerOP(DCE_Player * player, unsigned char index);
void DCE_RenderPlayerTR(DCE_Player * player, unsigned char index);

void DCE_MatrixComputePlayerRender(unsigned char player_num);
void DCE_MatrixLoadPlayerRender(unsigned char player_num);

void DCE_RenderPlayerPT(DCE_Player * player, unsigned char index);

void DCE_LoadPlayerModels();

DCE_Player * DCE_PlayerPointer(unsigned char index);

#endif
