/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#ifndef DCE_TRIGGER_H
#define DCE_TRIGGER_H

#include <dc/vec3f.h>

#define DCE_MAX_TRIGGERS  128

#define DCE_TRIGGER_DELTA 28.0f

typedef struct
{
    vec3f_t P;	
    unsigned char trigger;  // ON or OFF
    unsigned char active;
	char    segment[32];
} DCE_Trigger;

void DCE_TriggerCallback(vec3f_t * O, unsigned char index);

void DCE_LoadTriggerCluster(char * fname);

void DCE_TriggerResetCBO(unsigned char index);
void DCE_TriggerInitCBO(char * segment, unsigned char index);

void DCE_TriggerResetRBO(unsigned char index);
void DCE_TriggerInitRBO(char * segment, unsigned char index);

void DCE_TriggerReset(unsigned char index);
void DCE_TriggerInit(char * trigger, unsigned char index);

void DCE_TriggerLobbyA(DCE_Player * player, unsigned char index);
void DCE_TriggerLobbyB(DCE_Player * player, unsigned char index);
void DCE_TriggerRoof(DCE_Player * player, unsigned char index);
void DCE_TriggerVaultA(DCE_Player * player, unsigned char index);
void DCE_TriggerVaultB(DCE_Player * player, unsigned char index);
void DCE_TriggerFloor2A(DCE_Player * player, unsigned char index);
void DCE_TriggerFloor2B(DCE_Player * player, unsigned char index);

void DCE_ReSpawn(DCE_Player * player, unsigned char index);
void DCE_ReSpawn2(DCE_Player * player, unsigned char index);

void DCE_InitPlayerState(DCE_Player * player);

#endif
