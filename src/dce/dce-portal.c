
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "dce-collision.h"
#include "dce-portal.h"
#include "dce-vector.h"
#include "dce-matrix.h"
#include "dce-render.h"

#include "q3-bsp.h"

static vec3f_t up = { 0, 1, 0 };

void DCE_PortalRenderSRC(DCE_Portal * portal, unsigned short texID);
void DCE_PortalRenderDST(DCE_Portal * portal, unsigned short texID);
void DCE_PortalRenderPortalTexSRC(DCE_Portal * portal, unsigned short texID);
void DCE_PortalRenderPortalTexDST(DCE_Portal * portal, unsigned short texID);
void DCE_PortalRenderPlayer(DCE_Player * player, unsigned short texID);

void DCE_SetPortal(DCE_Portal * p, DCE_Player *player)
{
	vec3f_t d; 

	vec3f_copy(&player->lookAt, &d);

	vec3f_shiftp(&d, &player->position, 1000.0f);

	if(Q3BSP_CheckCollision(&player->position, &d, &p->position))
	{
	    vec3f_copy(Q3BSP_CollisionPlaneNormal(), &p->direction);
		vec3f_add(&p->direction, &p->position, &p->position);

		p->active = 1;
	}
	else
		p->active = 0;
}

void DCE_CheckPortalCollision(DCE_Portal * p, DCE_Player *player)
{
	if(p[DCE_PORTAL_SRC].active && p[DCE_PORTAL_DST].active)
		if(DCE_CheckCollisionSphere(&p[DCE_PORTAL_SRC].position, p->size, &player->position, player->height / 2))
	    {
			float nl, vl;
		    vec3f_length(p[DCE_PORTAL_DST].direction.x, p[DCE_PORTAL_DST].direction.y, p[DCE_PORTAL_DST].direction.z, nl);

		    vec3f_distance(player->position.x, player->position.y, player->position.z,
			           player->lookAt.x, player->lookAt.y, player->lookAt.z, vl);

			vec3f_copy(&p[DCE_PORTAL_DST].position, &player->direction);
			vec3f_copy(&p[DCE_PORTAL_DST].position, &player->position);
			vec3f_add(&p[DCE_PORTAL_DST].position, &p[DCE_PORTAL_DST].direction, &player->lookAt);
			vec3f_shift(&player->lookAt, &player->direction, vl / nl);
			vec3f_sub(&p[DCE_PORTAL_DST].direction, &player->lookAt, &player->lookAt);
		}
}

void DCE_PortalViewDST(DCE_Portal * p, DCE_Player *player, vec3f_t * v)
{
	vec3f_add(&p[DCE_PORTAL_DST].position, &p[DCE_PORTAL_DST].direction, v);
}

void DCE_PortalViewSRC(DCE_Portal * p, DCE_Player *player, vec3f_t * v)
{
	vec3f_add(&p[DCE_PORTAL_SRC].position, &p[DCE_PORTAL_SRC].direction, v);
}

void DCE_PortalRenderToTexture(DCE_Portal * portal, DCE_Player *player, void * portalTex,
							   unsigned short texID0, unsigned short texID1, unsigned short texID2, unsigned short texID3)
{
	if(portal[DCE_PORTAL_SRC].active || portal[DCE_PORTAL_DST].active)
	{
		vec3f_t up = { 0, 1, 0 };
		vec3f_t vp;
		float rd;
		int w = 1024, h = 512;

		DCE_RenderStartToTexture((pvr_ptr_t)portalTex, &w, &h);		

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0f, 16.0f / 9.0f, 0.1f, 10000.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glViewport(0, 0, 640, 480);

		vec3f_t pv;
		DCE_PortalViewDST(portal, player, &pv);

		if(vec3f_equal(&portal[DCE_PORTAL_DST].direction, &up))
		{
			up.y = 0;
			up.z = -1;
		}
		else if(vec3f_equal_inv(&portal[DCE_PORTAL_DST].direction, &up))
		{
			up.y = 0;
			up.z = 1;
		}

		vec3f_sub(&player->position, &player->lookAt, &vp);
		vec3f_dot(vp.x, vp.y, vp.z, 
		          portal[DCE_PORTAL_DST].direction.x,
				  portal[DCE_PORTAL_DST].direction.y,
				  portal[DCE_PORTAL_DST].direction.z,
				  rd);
		if(rd < 0)
		{
	        vec3f_reflect(&vp, &portal[DCE_PORTAL_DST].direction, &vp);
		    vec3f_add(&portal[DCE_PORTAL_DST].position, &vp, &vp);
		}
		else
		{
		    vec3f_add(&portal[DCE_PORTAL_DST].position, &portal[DCE_PORTAL_DST].direction, &vp);
		}

	    glhLookAtf2((float *)&portal[DCE_PORTAL_DST].position,
			        (float *)&vp,
					(float *)&up);

	    DCE_MatrixApplyRender();

	    Q3BSP_RenderArraysTexturedOP();

		DCE_PortalRenderPortalTexSRC(portal, texID0);

	    DCE_RenderSwitchTR();

		DCE_PortalRenderPlayer(player, texID2);
		
		DCE_RenderCheapShadow(texID3, &player->shadow_position, 30.0f);

	    //Q3BSP_RenderArraysTexturedTR();

		Q3BSP_RenderArraysLightMaps();

		DCE_PortalRenderSRC(portal, texID1);

		DCE_RenderFinish();
	}
}

void DCE_PortalRenderToTextureSRC(DCE_Portal * portal, DCE_Player *player, void * portalTex, unsigned short texID0, unsigned short texID1)
{
	if(portal[DCE_PORTAL_SRC].active || portal[DCE_PORTAL_DST].active)
	{
	    int w = 1024, h = 512;
		DCE_RenderStartToTexture((pvr_ptr_t)portalTex, &w, &h);		

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0f, 16.0f / 9.0f, 0.1f, 10000.0f);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glViewport(0, 0, 640, 480);

		vec3f_t pv;
		DCE_PortalViewSRC(portal, player, &pv);

	    glhLookAtf2((float *)&portal[DCE_PORTAL_SRC].position,
			        (float *)&pv,
					(float *)&up);

	    DCE_MatrixApplyRender();

	    Q3BSP_RenderArraysTexturedOP();

		DCE_PortalRenderPortalTexDST(portal, texID0);

	    DCE_RenderSwitchTR();

	    //Q3BSP_RenderArraysTexturedTR();

		Q3BSP_RenderArraysLightMaps();

		DCE_PortalRenderDST(portal, texID1);

		DCE_RenderFinish();
	}
}

void DCE_PortalRenderPortalTex(DCE_Portal * portal, unsigned short texID)
{
	if(portal[DCE_PORTAL_SRC].active)
	{
		float s = 60;
        float s2 = s / 2;

		uint32 argb[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		                  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };

		vec3f_t p;
		vec3f_add(&portal[DCE_PORTAL_SRC].position, &portal[DCE_PORTAL_SRC].direction, &p);
		/*
		float v[] = { p.x - s2, p.y + s, p.z,
				          p.x + s2, p.y + s, p.z,
   					      p.x - s, p.y + s2, p.z,
						  p.x + s, p.y + s2, p.z,
						  p.x - s, p.y -  s2, p.z,
						  p.x + s, p.y -  s2, p.z,
						  p.x - s2, p.y - s, p.z,
						  p.x + s2, p.y - s, p.z };


		   float uv[] = { 240 / 1024.0f, 0,
				           400 / 1024.0f, 0,
						   80 / 1024.0f, 160 / 512.0f,
						   560 / 1024.0f, 160 / 512.0f,
			               80 / 1024.0f, 320 / 512.0f,
				           560 / 1024.0f, 320 / 512.0f,
						   240 / 1024.0f, 480 / 512.0f,
						   400 / 1024.0f, 480 / 512.0f };

		DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, texID, 0);

		DCE_RenderTransformAndClipTriangleStrip(v, 3 * sizeof(float),
                                            uv, 2 * sizeof(float),
                                            (unsigned int *)argb, sizeof(unsigned int), 8);
											*/
		DCE_RenderTesselateDecal(&p, s, &portal[DCE_PORTAL_SRC].direction, texID, 640/1024.0f, 480/512.0f, 0xFFFFFFFF);
    }
	if(portal[DCE_PORTAL_DST].active)
	{
		float s = 60;
		float s2 = s / 2;

		uint32 argb[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		                  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };

		vec3f_t p;
		vec3f_add(&portal[DCE_PORTAL_DST].position, &portal[DCE_PORTAL_DST].direction, &p);
		/*
		float v[] = { p.x - s2, p.y + s, p.z,
				          p.x + s2, p.y + s, p.z,
   					      p.x - s, p.y + s2, p.z,
						  p.x + s, p.y + s2, p.z,
						  p.x - s, p.y -  s2, p.z,
						  p.x + s, p.y -  s2, p.z,
						  p.x - s2, p.y - s, p.z,
						  p.x + s2, p.y - s, p.z };


		   float uv[] = { 240 / 1024.0f, 0,
				           400 / 1024.0f, 0,
						   80 / 1024.0f, 160 / 512.0f,
						   560 / 1024.0f, 160 / 512.0f,
			               80 / 1024.0f, 320 / 512.0f,
				           560 / 1024.0f, 320 / 512.0f,
						   240 / 1024.0f, 480 / 512.0f,
						   400 / 1024.0f, 480 / 512.0f };

		DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, texID, 0);

		DCE_RenderTransformAndClipTriangleStrip(v, 3 * sizeof(float),
                                            uv, 2 * sizeof(float),
                                            (unsigned int *)argb, sizeof(unsigned int), 8);
											*/
		DCE_RenderTesselateDecal(&p, s, &portal[DCE_PORTAL_DST].direction, texID, 640/1024.0f, 480/512.0f, 0xFFFFFFFF);
    }
}

static int rotf = 0.0f;

static void DCE_PortalR2(vec3f_t * p, float s, unsigned int *argb, unsigned short texID, float r)
{
	vec3f_t v[] = { { p->x - s, p->y + s, p->z },
		                { p->x + s, p->y + s, p->z },
		                { p->x - s, p->y - s, p->z },
		                { p->x + s, p->y - s, p->z} };  

	vec3f_t v2[] = { { p->x - s, p->y + s, p->z },
		                { p->x + s, p->y + s, p->z },
		                { p->x - s, p->y - s, p->z },
		                { p->x + s, p->y - s, p->z} };  

	vec3f_rotated_xy(&v[0], p, r);
	vec3f_rotated_xy(&v[1], p, r);
	vec3f_rotated_xy(&v[2], p, r);
	vec3f_rotated_xy(&v[3], p, r);
	vec3f_rotated_xy(&v2[0], p, -r);
	vec3f_rotated_xy(&v2[1], p, -r);
	vec3f_rotated_xy(&v2[2], p, -r);
	vec3f_rotated_xy(&v2[3], p, -r);

	float uv[] = { 0, 0, 1, 0, 0, 1, 1, 1 };

	DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);

	DCE_RenderSetBlendDst(PVR_BLEND_DESTALPHA);

	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_TR_POLY, texID, PVR_TXRENV_MODULATEALPHA);

    DCE_RenderTransformAndClipTriangleStrip((float *)v, 3 * sizeof(float),
                                            uv, 2 * sizeof(float),
                                            (unsigned int *)argb, sizeof(unsigned int), 4);

	DCE_RenderTransformAndClipTriangleStrip((float *)v2, 3 * sizeof(float),
                                            uv, 2 * sizeof(float),
                                            (unsigned int *)argb, sizeof(unsigned int), 4);
}

static void DCE_PortalR3(vec3f_t * p, float s, vec3f_t * n, unsigned int *argb, unsigned short texID, float r)
{
	vec3f_t v[8];

	DCE_TesselateDecalQuad(p, s, n, &v);

	float uv[] = { 0, 0, 1, 0, 0, 1, 1, 1 };

	DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);

	DCE_RenderSetBlendDst(PVR_BLEND_DESTALPHA);

	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_TR_POLY, texID, PVR_TXRENV_MODULATEALPHA);

    DCE_RenderTransformAndClipTriangleStrip((float *)v, 3 * sizeof(float),
                                            uv, 2 * sizeof(float),
                                            (unsigned int *)argb, sizeof(unsigned int), 4);

	vec3f_rotated_xy(&v[0], p, r);
	vec3f_rotated_xy(&v[1], p, r);
	vec3f_rotated_xy(&v[2], p, r);
	vec3f_rotated_xy(&v[3], p, r);
	vec3f_rotated_xy(&v[4], p, r);
	vec3f_rotated_xy(&v[5], p, r);
	vec3f_rotated_xy(&v[6], p, r);
	vec3f_rotated_xy(&v[7], p, r);

    DCE_RenderTransformAndClipTriangleStrip((float *)v, 3 * sizeof(float),
                                            uv, 2 * sizeof(float),
                                            (unsigned int *)argb, sizeof(unsigned int), 4);

	DCE_RenderTransformAndClipTriangleStrip((float *)&v[4], 3 * sizeof(float),
                                            uv, 2 * sizeof(float),
                                            (unsigned int *)argb, sizeof(unsigned int), 4);
											
}

void DCE_PortalRender(DCE_Portal * portal, unsigned short texID)
{
	DCE_MatrixApplyRender();

	if(portal[DCE_PORTAL_SRC].active)
	{
		vec3f_t p;
		vec3f_add(&portal[DCE_PORTAL_SRC].position, &portal[DCE_PORTAL_SRC].direction, &p);
		vec3f_add(&p, &portal[DCE_PORTAL_SRC].direction, &p);

		uint32 argb[] = { 0xFF00FFFF, 0xFF00FFFF, 0xFF00FFFF, 0xFF00FFFF };
		/*
		DCE_RenderTesselateDecal(&p, 80.0f, &portal[DCE_PORTAL_SRC].direction, texID, 1, 1, 0xFF00FFFF);
		*/
		DCE_PortalR3(&p, 90.0f, &portal[DCE_PORTAL_SRC].direction, (unsigned int *)argb, texID, rotf);
		DCE_PortalR3(&p, 90.0f, &portal[DCE_PORTAL_SRC].direction, (unsigned int *)argb, texID, rotf / 2);
		DCE_PortalR3(&p, 90.0f, &portal[DCE_PORTAL_SRC].direction, (unsigned int *)argb, texID, rotf * 2);
		DCE_PortalR3(&p, 90.0f, &portal[DCE_PORTAL_SRC].direction, (unsigned int *)argb, texID, rotf *3);
		
	}
	if(portal[DCE_PORTAL_DST].active)
	{
		vec3f_t p;
		vec3f_add(&portal[DCE_PORTAL_DST].position, &portal[DCE_PORTAL_DST].direction, &p);
		vec3f_add(&p, &portal[DCE_PORTAL_DST].direction, &p);

		uint32 argb[] = { 0xFFFF0909, 0xFFFF0909, 0xFFFF0909, 0xFFFF0909 };
		/*
		DCE_RenderTesselateDecal(&p, 80.0f, &portal[DCE_PORTAL_DST].direction, texID, 1, 1, 0xFFFF0909);
		*/
	    DCE_PortalR3(&p, 90.0f, &portal[DCE_PORTAL_DST].direction, (unsigned int *)argb, texID, rotf);
		DCE_PortalR3(&p, 90.0f, &portal[DCE_PORTAL_DST].direction, (unsigned int *)argb, texID, rotf / 2.0f);
		DCE_PortalR3(&p, 90.0f, &portal[DCE_PORTAL_DST].direction, (unsigned int *)argb, texID, rotf / 4.0f);
	    DCE_PortalR3(&p, 90.0f, &portal[DCE_PORTAL_DST].direction, (unsigned int *)argb, texID, rotf / 6.0f);
		
	}

	++rotf;
}

void DCE_PortalRenderSRC(DCE_Portal * portal, unsigned short texID)
{
	DCE_MatrixApplyRender();

	if(portal[DCE_PORTAL_SRC].active)
	{
		vec3f_t p;
		vec3f_add(&portal[DCE_PORTAL_SRC].position, &portal[DCE_PORTAL_SRC].direction, &p);
		vec3f_add(&p, &portal[DCE_PORTAL_SRC].direction, &p);

		uint32 argb[] = { 0xFF00FFFF, 0xFF00FFFF, 0xFF00FFFF, 0xFF00FFFF };
		
		DCE_PortalR2(&p, 90.0f, (unsigned int *)argb, texID, rotf);
		DCE_PortalR2(&p, 90.0f, (unsigned int *)argb, texID, rotf / 2);
	}
}

void DCE_PortalRenderDST(DCE_Portal * portal, unsigned short texID)
{
	DCE_MatrixApplyRender();

	if(portal[DCE_PORTAL_DST].active)
	{
		vec3f_t p;
		vec3f_add(&portal[DCE_PORTAL_DST].position, &portal[DCE_PORTAL_DST].direction, &p);
		vec3f_add(&p, &portal[DCE_PORTAL_DST].direction, &p);

		uint32 argb[] = { 0xFF00FFFF, 0xFF00FFFF, 0xFF00FFFF, 0xFF00FFFF };
		
		DCE_PortalR2(&p, 90.0f, (unsigned int *)argb, texID, rotf);
		DCE_PortalR2(&p, 90.0f, (unsigned int *)argb, texID, rotf / 2);
	}
}

void DCE_PortalRenderPortalTexSRC(DCE_Portal * portal, unsigned short texID)
{
	if(portal[DCE_PORTAL_SRC].active)
	{
		float s = 50;
        float s2 = s / 2;

		uint32 argb[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		                  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };

		vec3f_t p;
		vec3f_add(&portal[DCE_PORTAL_SRC].position, &portal[DCE_PORTAL_SRC].direction, &p);

		float v[] = { p.x - s2, p.y + s, p.z,
				          p.x + s2, p.y + s, p.z,
   					      p.x - s, p.y + s2, p.z,
						  p.x + s, p.y + s2, p.z,
						  p.x - s, p.y -  s2, p.z,
						  p.x + s, p.y -  s2, p.z,
						  p.x - s2, p.y - s, p.z,
						  p.x + s2, p.y - s, p.z };


		   float uv[] = { 240 / 1024.0f, 0,
				           400 / 1024.0f, 0,
						   80 / 1024.0f, 160 / 512.0f,
						   560 / 1024.0f, 160 / 512.0f,
			               80 / 1024.0f, 320 / 512.0f,
				           560 / 1024.0f, 320 / 512.0f,
						   240 / 1024.0f, 480 / 512.0f,
						   400 / 1024.0f, 480 / 512.0f };

		DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, texID, 0);

		DCE_RenderTransformAndClipTriangleStrip(v, 3 * sizeof(float),
                                            uv, 2 * sizeof(float),
                                            (unsigned int *)argb, sizeof(unsigned int), 8);
    }
}

void DCE_PortalRenderPortalTexDST(DCE_Portal * portal, unsigned short texID)
{
	if(portal[DCE_PORTAL_DST].active)
	{
		float s = 50;
		float s2 = s / 2;

		uint32 argb[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		                  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };

		vec3f_t p;
		vec3f_add(&portal[DCE_PORTAL_DST].position, &portal[DCE_PORTAL_DST].direction, &p);

		float v[] = { p.x - s2, p.y + s, p.z,
				          p.x + s2, p.y + s, p.z,
   					      p.x - s, p.y + s2, p.z,
						  p.x + s, p.y + s2, p.z,
						  p.x - s, p.y -  s2, p.z,
						  p.x + s, p.y -  s2, p.z,
						  p.x - s2, p.y - s, p.z,
						  p.x + s2, p.y - s, p.z };


		   float uv[] = { 240 / 1024.0f, 0,
				           400 / 1024.0f, 0,
						   80 / 1024.0f, 160 / 512.0f,
						   560 / 1024.0f, 160 / 512.0f,
			               80 / 1024.0f, 320 / 512.0f,
				           560 / 1024.0f, 320 / 512.0f,
						   240 / 1024.0f, 480 / 512.0f,
						   400 / 1024.0f, 480 / 512.0f };

		DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, texID, 0);

		DCE_RenderTransformAndClipTriangleStrip(v, 3 * sizeof(float),
                                            uv, 2 * sizeof(float),
                                            (unsigned int *)argb, sizeof(unsigned int), 8);
    }
}

void DCE_PortalRenderPlayer(DCE_Player * player, unsigned short texID)
{
	float s = 20;
	float s2 = s / 2;
	
	vec3f_t *p = &player->position;

	vec3f_t v[] = { { p->x - s, p->y + s, p->z },
		                { p->x + s, p->y + s, p->z },
		                { p->x - s, p->y - s, p->z },
		                { p->x + s, p->y - s, p->z} };  

	uint32 argb[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };

    float uv[] = { 0, 0, 1, 0, 0, 1, 1, 1 };

		DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_TR_POLY, texID, PVR_TXRENV_MODULATEALPHA);

		DCE_RenderTransformAndClipTriangleStrip(v, 3 * sizeof(float),
                                            uv, 2 * sizeof(float),
                                            (unsigned int *)argb, sizeof(unsigned int), 4);
											

}