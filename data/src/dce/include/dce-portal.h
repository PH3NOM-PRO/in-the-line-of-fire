#ifndef DCE_PORTAL_H
#define DCE_PORTAL_H

#include <dc/vec3f.h>
#include "dce-player.h"

#define DCE_PORTAL_SRC 0
#define DCE_PORTAL_DST 1

typedef struct
{
	vec3f_t position;
	vec3f_t direction;
	float size;
	unsigned char active;
} DCE_Portal;

void DCE_SetPortal(DCE_Portal * p, DCE_Player *player);

void DCE_CheckPortalCollision(DCE_Portal * p, DCE_Player *player);

void DCE_PortalView(DCE_Portal * p, DCE_Player *player, vec3f_t * v);

void DCE_PortalRenderToTexture(DCE_Portal * portal, DCE_Player *player, void * portalTex,
							   unsigned short texID0, unsigned short texID1, unsigned short texID2, unsigned short texID3);

void DCE_PortalRender(DCE_Portal * portal, unsigned short texID);

void DCE_PortalRenderPortalTex(DCE_Portal * portal, unsigned short texID);

void DCE_PortalRenderToTextureSRC(DCE_Portal * portal, DCE_Player *player, void * portalTex,
								  unsigned short texID0, unsigned short texID1);

#endif