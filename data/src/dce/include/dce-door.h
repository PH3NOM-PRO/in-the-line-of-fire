/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#ifndef DCE_DOOR_H
#define DCE_DOOR_H

#define DCE_DOOR_COLLISION_DISTANCE 66.5f

#define DCE_DOOR_ROT_SPEED 4.5f

#define DCE_MAX_DOORS 32

void DCE_LoadDoors();

void DCE_LoadDoorCluster(char * fname);

void DCE_RenderDoors();

void DCE_TriggerRenderOpenDoor(vec3f_t * O);

void DCE_CheckCollisionDOOR(vec3f_t * O, vec3f_t * D, float * d);

void DCE_TriggerDOOR(char * segment, unsigned char active);
void DCE_TriggerResetDOOR();
void DCE_TriggerOpenDOOR();

#endif
