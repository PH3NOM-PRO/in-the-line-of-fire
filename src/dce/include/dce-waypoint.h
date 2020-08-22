#ifndef DCE_WAYPOINT_H
#define DCE_WAYPOINT_H

#include "dce-player.h"

#define DCE_STEP_DISTANCE 2.0f

#define DCE_BOT_VISION_FOV 0.1f

typedef struct
{
    vec3f_t position;
    float   direction;
    char    animation[16];
    unsigned int frames;
    unsigned int next_node;
}DCE_Waypoint;

typedef struct
{
    char sig[4];
    char id[4];
    unsigned int nodes;
    unsigned int current_node;
    unsigned int frame;
	float lerp_factor;
    DCE_Waypoint * node;
}DCE_WaypointCluster;

DCE_WaypointCluster * DCE_LoadWaypointCluster(char * fname);

void DCE_WaypointCallbackBot2(DCE_WaypointCluster * cluster, DCE_Player * player, DCE_Player * target);

void DCE_BotPlayerVisionCallback(DCE_Waypoint * point, DCE_Player * bot, DCE_Player * player);

#endif
