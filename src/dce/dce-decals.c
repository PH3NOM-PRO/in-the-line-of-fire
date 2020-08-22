/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#include "dce.h"

static DCE_Decal DCE_DECAL[DCE_MAX_DECALS];
static unsigned short int DCE_DECALS = 0;

extern unsigned char DCE_CheckCollisionNoDoorsCBO(vec3f_t * O, vec3f_t * D, float * d);

void DCE_DecalsReset()
{
	DCE_DECALS = 0;
}

void DCE_DecalsRender()
{
    unsigned short int i = 0;
    
	float uv[] = { 0, 0, 1, 0, 0, 1, 1, 1 };

	unsigned int argb[] = { 0xE0B0B0B0, 0xE0B0B0B0, 0xE0B0B0B0, 0xE0B0B0B0 };
    
    for(i = 0; i < DCE_DECALS; i++)
    {
	    DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_TR_POLY, DCE_DECAL[i].texID, PVR_TXRENV_MODULATEALPHA);

        DCE_RenderTransformAndClipTriangleStrip((float *)&DCE_DECAL[i].vertex[0], sizeof(vec3f_t),
                                                uv, 2 * sizeof(float),
                                                argb, sizeof(unsigned int), 4);
    }
}

void DCE_DecalTesselate(vec3f_t * I, vec3f_t * E1, vec3f_t * N, float size, vec3f_t * v)
{
	vec3f_t U;  
	vec3f_t V; 
    
    vec3f_cross(E1, N, &U);

    vec3f_normalize(U.x, U.y, U.z);

    vec3f_cross(N, &U, &V);
    
    vec3f_normalize(V.x, V.y, V.z);
	    
    vec3f_copy(&U, &v[0]);
    vec3f_mul_scalar(&v[0], size, &v[0]);
    vec3f_add(I, &v[0], &v[0]);

    vec3f_copy(&V, &v[1]);
    vec3f_mul_scalar(&v[1], size, &v[1]);
    vec3f_add(I, &v[1], &v[1]);

    vec3f_copy(&V, &v[2]);
    vec3f_mul_scalar(&v[2], -size, &v[2]);
    vec3f_add(I, &v[2], &v[2]);

    vec3f_copy(&U, &v[3]);
    vec3f_mul_scalar(&v[3], -size, &v[3]);
    vec3f_add(I, &v[3], &v[3]);
}

void DCE_DecalAdd(vec3f_t * I, vec3f_t * N, vec3f_t * E1, float size, unsigned short int texID)
{
    if(DCE_DECALS >= DCE_MAX_DECALS)
        DCE_DECALS = 0;

    DCE_DECAL[DCE_DECALS].texID = texID;
    
	DCE_DecalTesselate(I, E1, N, size, &DCE_DECAL[DCE_DECALS].vertex[0]);
	 
    ++DCE_DECALS;
}

void DCE_RenderInsertBulletDecal(DCE_Player * player, unsigned short int texID)
{
	vec3f_t D, P; 
    float d;
    
    if(DCE_CheckCollisionNoDoorsCBO(&player->position, &player->direction, &d))
    {
    	vec3f_copy(&player->position, &P);
    	
    	d -= DCE_RAY_EPSILON * .25;
    	
	    vec3f_mul_scalar(&player->direction, d, &D);
	    
	    vec3f_add(&D, &P, &P);
	
	    DCE_DecalAdd(&P, DCE_CollisionModelNormal(), DCE_CollisionModelE1(), 1.5f, texID);
	}
}

void DCE_RenderInsertDecal(DCE_Player * player, unsigned short int texID, float size)
{
	vec3f_t D, P; 
    float d;
    
    if(DCE_CheckCollisionNoDoorsCBO(&player->position, &player->direction, &d))
    {
    	vec3f_copy(&player->position, &P);
    	
    	d -= DCE_RAY_EPSILON * .25;
    	
	    vec3f_mul_scalar(&player->direction, d, &D);
	    
	    vec3f_add(&D, &P, &P);
	
	    DCE_DecalAdd(&P, DCE_CollisionModelNormal(), DCE_CollisionModelE1(), size, texID);
	}
}

void DCE_RenderInsertBloodDecal(vec3f_t * P, unsigned short int texID, float size)
{
    vec3f_t N = { 0, 1, 0 };
    
    vec3f_t E = { 1, 0, 0 };
	
	DCE_DecalAdd(&P, &N, &E, size, texID);
}

void DCE_RenderBulletDecal(DCE_Player * player, unsigned short int texID)
{
	
}

void DCE_RenderBloodDecal(DCE_Player * player, unsigned short int texID)
{

}

void DCE_RenderBloodDecalPosition(vec3f_t * position, unsigned short int texID)
{

}
