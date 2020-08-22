/* 
** DCE - Dreamcast Engine (C) 2013-2015 Josh PH3NOM Pearson
*/

#include "dce-bbox.h"

void DCE_BoudingBoxSetPlayer(DCE_Player * player)
{
    if(!(player->state & PSTATE_DEAD)) // Compute Bounding Box For Living Player
    {
    	if(!(player->state & PSTATE_DUCK)) // Compute Standing Bounding Box
    	{
            player->bbox_min.x = player->position.x - BBOX_W;
            player->bbox_min.y = player->position.y - BBOX_H2 - player->height / 2;
            player->bbox_min.z = player->position.z - BBOX_W;
            player->bbox_max.x = player->position.x + BBOX_W;
            player->bbox_max.y = player->position.y + BBOX_H1 - player->height / 2;
            player->bbox_max.z = player->position.z + BBOX_W;
    
            player->hbbox_min.x = player->position.x - HBOX_W;
            player->hbbox_min.y = player->position.y + BBOX_H1 - player->height / 2;
            player->hbbox_min.z = player->position.z - HBOX_W;
            player->hbbox_max.x = player->position.x + HBOX_W;
            player->hbbox_max.y = player->position.y + BBOX_H2 - player->height / 2;
            player->hbbox_max.z = player->position.z + HBOX_W;
		}
    	else // Compute Crouching Bounding Box
    	{
            player->bbox_min.x = player->position.x - BBOX_W;
            player->bbox_min.y = player->position.y - BBOX_H2 - player->height / 4;
            player->bbox_min.z = player->position.z - BBOX_W;
            player->bbox_max.x = player->position.x + BBOX_W;
            player->bbox_max.y = player->position.y + (BBOX_H2 / 2) - HBOX_H - player->height / 4;
            player->bbox_max.z = player->position.z + BBOX_W;
    
            player->hbbox_min.x = player->position.x - HBOX_W;
            player->hbbox_min.y = player->position.y + (BBOX_H2 / 2) - HBOX_H - player->height / 4;
            player->hbbox_min.z = player->position.z - HBOX_W;
            player->hbbox_max.x = player->position.x + HBOX_W;
            player->hbbox_max.y = player->position.y + (BBOX_H2 / 2) - player->height / 4;
            player->hbbox_max.z = player->position.z + HBOX_W;
		}		
	}
	else // Compute Bounding Box For Dead Player - ToDo: Only need be done once
    {
        player->bbox_min.x = player->position.x - BBOX_W;
        player->bbox_min.y = player->position.y - BBOX_H2 - player->height / 2;
        player->bbox_min.z = player->position.z - BBOX_W;
        player->bbox_max.x = player->position.x + BBOX_W;
        player->bbox_max.y = player->position.y - (BBOX_H2 / 2) - player->height / 2;
        player->bbox_max.z = player->position.z + BBOX_W;
    
        player->hbbox_min.x = player->position.x - BBOX_W;
        player->hbbox_min.y = player->position.y - BBOX_H2 - player->height / 2;
        player->hbbox_min.z = player->position.z - BBOX_W;
        player->hbbox_max.x = player->position.x + BBOX_W;
        player->hbbox_max.y = player->position.y - (BBOX_H2 / 2) - player->height / 2;
        player->hbbox_max.z = player->position.z + BBOX_W;	
	}       
}
