/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#include <stdio.h>
#include <stdlib.h>

#include "dce.h"

// Determine weather or not a Point(P) is inside the FOV of a Origin(O) and Direction(D) Ray(R)
unsigned char DCE_PointIsInsideFOV3(vec3f_t * O, vec3f_t * D, vec3f_t * P)
{
    vec3f_t V;
    float d;

    vec3f_sub_normalize(P->x, P->y, P->z, O->x, O->y, O->z, V.x, V.y, V.z);
    
    vec3f_dot(D->x, 0, D->z, V.x, 0, V.z, d);
    
    return d > DCE_FOV_MIN_DOT2;
}

// Determine weather or not a Point(P) is inside the FOV of a Origin(O) and Direction(D) Ray(R)
ubyte DCE_PointInView(vec3f_t * O, vec3f_t * D, vec3f_t * P)
{
	vec3f_t R;
	float   d1, d2;
		
	if(DCE_PointIsInsideFOV2(O, D, P))
	{
		vec3f_distance(O->x, O->y, O->z, P->x, P->y, P->z, d1);
		
		vec3f_sub_normalize(P->x, P->y, P->z, O->x, O->y, O->z, R.x, R.y, R.z);
		
		DCE_CheckCollisionCBO(O, &R, &d2);
		
		if(d2 > d1)
		    return 1;
	}
	
	return 0;
}

float DCE_PointIsInsideFOV2(vec3f_t * O, vec3f_t * D, vec3f_t * P)
{
    vec3f_t V;
    float d;

    vec3f_sub_normalize(P->x, P->y, P->z, O->x, O->y, O->z, V.x, V.y, V.z);
    
    vec3f_dot(D->x, D->y, D->z, V.x, V.y, V.z, d);

    if(d < DCE_FOV_MIN_DOT2)
        return 0;
    
    vec3f_distance(O->x, O->y, O->z, P->x, P->y, P->z, d);
    
    return d;
}

/*
** Determine Weather or not A Point Can Be Seen Given a Camera Position and Look-At
** Returns the Distance between Camera Position and Point if point is visisble.
*/
float DCE_PointIsInsideFOV(vec3f_t * from, vec3f_t * to, vec3f_t * p)
{
    vec3f_t E, V;
    float d;
    
    vec3f_sub(from, to, &E);
    vec3f_sub(from, p, &V);
    
    vec3f_dot(E.x, E.y, E.z, V.x, V.y, V.z, d);

    if(d < DCE_FOV_MIN_DOT)
        return 0;
    
    vec3f_distance(from->x, from->y, from->z, p->x, p->y, p->z, d);
    
    return d;
}

unsigned char DCE_CheckCollisionSphere(vec3f_t * v1, float r1, vec3f_t * v2, float r2)
{
	float d;

	vec3f_distance(v1->x, v1->y, v1->z, v2->x, v2->y, v2->z, d);

	return d <= r1 + r2;
}

unsigned char DCE_CheckCollisionPoint(vec3f_t * p, vec3f_t * min, vec3f_t * max)
{
    unsigned char collision = 0;
    
    if((p->x >= min->x) && (p->x <= max->x))
        collision |= DCE_COLLISION_INSIDE_X;
        
    if((p->y >= min->y) && (p->y <= max->y))
        collision |= DCE_COLLISION_INSIDE_Y;
        
    if((p->z >= min->z) && (p->z <= max->z))
        collision |= DCE_COLLISION_INSIDE_Z;   
                     
    return collision;
}

unsigned char DCE_CheckCollisionRay(vec3f_t * start, vec3f_t * end,
                                    vec3f_t * min, vec3f_t * max)
{
    unsigned char sc = DCE_CheckCollisionPoint(start, min, max);
    unsigned char ec = DCE_CheckCollisionPoint(end, min, max);

    return !((sc != DCE_COLLISION_INSIDE) && (ec != DCE_COLLISION_INSIDE));
}

extern void DCE_RequestPause();

void DCE_PlayerCollisionCallback(DCE_Player * player)
{
}

float DCE_TraceRayCollision(DCE_Player * player, DCE_Player * target, unsigned short int targets)
{
    return 0;
}

unsigned char DCE_CheckPlayerAmmoCollision(DCE_Player * player, DCE_Player * target)
{
    #define Y_DELTA 5.0f
    vec3f_t point = { player->position.x,
                      player->position.y - player->height + Y_DELTA,
                      player->position.z };
                      
    if(target->health <= 0)
    {
        if(DCE_CheckCollisionPoint(&point, &target->bbox_min, &target->bbox_max)
           == DCE_COLLISION_INSIDE)
        {
            if(target->ammo || target->clip)
                DCE_SfxPlay(WEAPON_RELOAD); 
            return 1;
        }
    }
    
    return 0;
}

#define REPSILON 0.00001

static inline void vec3_cross(vec3f_t * v1, vec3f_t * v2, vec3f_t * out)
{
    out->x = v1->y * v2->z - v1->z * v2->y;
    out->y = v1->z * v2->x - v1->x * v2->z;
    out->z = v1->x * v2->y - v1->y * v2->x;
}

int DCE_TraceRayTriCollision( const vec3f_t * V1,  // Triangle vertices
                              const vec3f_t * V2,
                              const vec3f_t * V3,
                              const vec3f_t * O,  // Ray origin
                              const vec3f_t * D,  // Ray direction
                              const vec3f_t * E,
                              float * out )
{
    vec3f_t P, Q, T;
    float inv_det, u, v;
    float t;
		
    // Begin calculating determinant - also used to calculate u parameter
    vec3_cross(D, &E[1], &P);

    // if determinant is near zero, ray lies in plane of triangle
    vec3f_dot(E[0].x, E[0].y, E[0].z, P.x, P.y, P.z, inv_det);

    // NOT CULLING
    if(inv_det > -REPSILON && inv_det < REPSILON)
        return 0;
    inv_det = 1.f / inv_det;
 
    // Calculate distance from V1 to ray origin
    vec3f_sub(V1, O, &T);

    //Calculate u parameter and test bound
    vec3f_dot(T.x, T.y, T.z, P.x, P.y, P.z, u);
	u *= inv_det;

    //The intersection lies outside of the triangle
    if(u < 0.0f || u > 1.0f)
        return 0;

    //Prepare to test v parameter
    vec3_cross(&T, &E[0], &Q);

    //Calculate V parameter and test bound
    vec3f_dot(D->x, D->y, D->z, Q.x, Q.y, Q.z, v);
	v *= inv_det;
    
    //The intersection lies outside of the triangle
    if(v < 0.0f || u * v  > 1.f)
        return 0;

    vec3f_dot(E[1].x, E[1].y, E[1].z, Q.x, Q.y, Q.z, t);
	t *= inv_det;

    if(t > REPSILON) { //ray intersection
        *out = t;
        return 1;
    }

    // No hit, no win
    return 0;
}

int DCE_TraceRayCollisionTri( const vec3f_t * V1,  // Triangle vertices
                              const vec3f_t * E1,  // Edge 1
                              const vec3f_t * E2,  // Edge 2
                              const vec3f_t * O,   // Ray origin
                              const vec3f_t * D,   // Ray direction
                              float * out )
{
    vec3f_t P, Q, T;
    float inv_det, u, v;
    float t;
		
    // Begin calculating determinant - also used to calculate u parameter
    vec3_cross(D, E2, &P);

    // if determinant is near zero, ray lies in plane of triangle
    vec3f_dot(E1->x, E1->y, E1->z, P.x, P.y, P.z, inv_det);

    if(inv_det > -REPSILON && inv_det < REPSILON)
        return 0;
    inv_det = 1.f / inv_det;

    vec3f_sub(V1, O, &T);

    //Calculate u parameter and test bound
    vec3f_dot(T.x, T.y, T.z, P.x, P.y, P.z, u);
	u *= inv_det;

    //outside of the triangle
    if(u < 0.f || u > 1.f)
        return 0;

    vec3_cross(&T, E1, &Q);

    //Calculate V parameter and test bound
    vec3f_dot(D->x, D->y, D->z, Q.x, Q.y, Q.z, v);
	v *= inv_det;

    //outside of the triangle
    if(v < 0.f || u + v  > 1.f)
        return 0;

    vec3f_dot(E2->x, E2->y, E2->z, Q.x, Q.y, Q.z, t);
	t *= inv_det;

    if(t > REPSILON) { 
        *out = t;
        return 1;
    }

    return 0;
}

inline unsigned char DCE_TraceGravityRayCollisionTri( const DCE_CollisionTriangle * ct, // Collision Triangle
                                                      const vec3f_t * O,         // Ray origin
                                                      float * out )              // Collision Distance if return value is 1
{
    vec3f_t Q, T;
    float t, u, v;

    vec3f_sub(&ct->v1, O, &T);

    //Calculate u parameter and test bound
    vec3f_dot(T.x, T.y, T.z, ct->P.x, ct->P.y, ct->P.z, u);
	u *= ct->inv_det;

    //outside of the triangle
    if(u < 0.f || u > 1.f)
        return 0;

    vec3_cross(&T, &ct->edge1, &Q);

    //Calculate V parameter and test bound
    v = -Q.y * ct->inv_det;

    //outside of the triangle
    if(v < 0.f || u + v  > 1.f)
        return 0;

    vec3f_dot(ct->edge2.x, ct->edge2.y, ct->edge2.z, Q.x, Q.y, Q.z, t);
	t *= ct->inv_det;

    if(t > REPSILON) { 
        *out = t;
        return 1;
    }

    return 0;
}

/*
*  Trace a ray in the Direction the Player is looking at against target Bounding Box.
*/
unsigned char DCE_PlayerTraceRay(DCE_Player * player, DCE_Player * target)
{
	vec3f_t Ro; // Ray Origin
	float tx = 0, ty = 0, tz = 0;
	float tmin, tmax, mins, maxs;
	
	vec3f_copy(&player->position, &Ro);
	
	if(player->direction.x != 0)
	{
		tmin = (target->bbox_min.x - Ro.x) / player->direction.x;
		tmax = (target->bbox_max.x - Ro.x) / player->direction.x;
		mins = min(tmin, tmax);
		maxs = max(tmin, tmax);
		if(mins > 0 && maxs > 0)
		    tx = mins;
	    else
		    tx = 0;
	}
	if(tx)
	{
		Ro.x += player->direction.x * tx;
		Ro.y += player->direction.y * tx;
		Ro.z += player->direction.z * tx;
	}
	if(player->direction.y != 0)
	{
		tmin = (target->bbox_min.y - Ro.y) / player->direction.y;
		tmax = (target->bbox_max.y - Ro.y) / player->direction.y;
		mins = min(tmin, tmax);
		maxs = max(tmin, tmax);
		if(mins > 0 && maxs > 0)
		    ty = mins;
	    else
		    ty = 0;
	}
	if(ty)
	{
		Ro.x += player->direction.x * ty;
		Ro.y += player->direction.y * ty;
		Ro.z += player->direction.z * ty;
	}
	if(player->direction.z != 0)
	{
		tmin = (target->bbox_min.z - Ro.z) / player->direction.z;
		tmax = (target->bbox_max.z - Ro.z) / player->direction.z;
		mins = min(tmin, tmax);
		maxs = max(tmin, tmax);
		if(mins > 0 && maxs > 0)
		    tz = mins;
	    else
		    tz = 0;
	}
	if(tz)
	{
		Ro.x += player->direction.x * tz;
		Ro.y += player->direction.y * tz;
		Ro.z += player->direction.z * tz;
	}

	return DCE_CheckCollisionPoint(&Ro, &target->bbox_min, &target->bbox_max) == DCE_COLLISION_INSIDE;
}

unsigned char DCE_PlayerTraceRayBBox(DCE_Player * player, vec3f_t * bbmin, vec3f_t * bbmax)
{
	float tx = 0, ty = 0, tz = 0;
	float tmin, tmax, mins, maxs;
	vec3f_t Ro; // Ray Origin
		
	vec3f_copy(&player->position, &Ro);
	
	if(player->direction.x != 0)
	{
		tmin = (bbmin->x - Ro.x) / player->direction.x;
		tmax = (bbmax->x - Ro.x) / player->direction.x;
		
		mins = min(tmin, tmax);
		maxs = max(tmin, tmax);
		
		if(mins > 0 && maxs > 0)
		    tx = mins;
	    else
		    tx = 0;
	}
	if(tx)
	{
		Ro.x += player->direction.x * tx;
		Ro.y += player->direction.y * tx;
		Ro.z += player->direction.z * tx;
	}
	
	if(player->direction.y != 0)
	{
		tmin = (bbmin->y - Ro.y) / player->direction.y;
		tmax = (bbmax->y - Ro.y) / player->direction.y;
		
		mins = min(tmin, tmax);
		maxs = max(tmin, tmax);
		
		if(mins > 0 && maxs > 0)
		    ty = mins;
	    else
		    ty = 0;
	}
	if(ty)
	{
		Ro.x += player->direction.x * ty;
		Ro.y += player->direction.y * ty;
		Ro.z += player->direction.z * ty;
	}
	
	if(player->direction.z != 0)
	{
		tmin = (bbmin->z - Ro.z) / player->direction.z;
		tmax = (bbmax->z - Ro.z) / player->direction.z;
		
		mins = min(tmin, tmax);
		maxs = max(tmin, tmax);
		
		if(mins > 0 && maxs > 0)
		    tz = mins;
	    else
		    tz = 0;
	}
	if(tz)
	{
		Ro.x += player->direction.x * tz;
		Ro.y += player->direction.y * tz;
		Ro.z += player->direction.z * tz;
	}

	return DCE_CheckCollisionPoint(&Ro, bbmin, bbmax) == DCE_COLLISION_INSIDE;
}

unsigned char DCE_TraceRayBBox(vec3f_t * O, vec3f_t * D, vec3f_t * bbmin, vec3f_t * bbmax)
{
	float tx = 0, ty = 0, tz = 0;
	float tmin, tmax, mins, maxs;
	vec3f_t Ro; // Ray Origin
		
	vec3f_copy(O, &Ro);
	
	if(D->x != 0)
	{
		tmin = (bbmin->x - Ro.x) / D->x;
		tmax = (bbmax->x - Ro.x) / D->x;
		
		mins = min(tmin, tmax);
		maxs = max(tmin, tmax);
		
		if(mins > 0 && maxs > 0)
		    tx = mins;
	    else
		    tx = 0;
	}
	if(tx)
	{
		Ro.x += D->x * tx;
		Ro.y += D->y * tx;
		Ro.z += D->z * tx;
	}
	
	if(D->y != 0)
	{
		tmin = (bbmin->y - Ro.y) / D->y;
		tmax = (bbmax->y - Ro.y) / D->y;
		
		mins = min(tmin, tmax);
		maxs = max(tmin, tmax);
		
		if(mins > 0 && maxs > 0)
		    ty = mins;
	    else
		    ty = 0;
	}
	if(ty)
	{
		Ro.x += D->x * ty;
		Ro.y += D->y * ty;
		Ro.z += D->z * ty;
	}
	
	if(D->z != 0)
	{
		tmin = (bbmin->z - Ro.z) / D->z;
		tmax = (bbmax->z - Ro.z) / D->z;
		
		mins = min(tmin, tmax);
		maxs = max(tmin, tmax);
		
		if(mins > 0 && maxs > 0)
		    tz = mins;
	    else
		    tz = 0;
	}
	if(tz)
	{
		Ro.x += D->x * tz;
		Ro.y += D->y * tz;
		Ro.z += D->z * tz;
	}

	return DCE_CheckCollisionPoint(&Ro, bbmin, bbmax) == DCE_COLLISION_INSIDE;
}

#define DCE_PLAYER_COLLISION_DISTANCE 40.0f

unsigned char DCE_PlayerCollision(DCE_Player * p, DCE_Player * p2)
{
	float d;

	vec3f_distance(p->position.x, p->position.y, p->position.z,
	               p2->position.x, p2->position.y, p2->position.z, d);
	
	return d < DCE_PLAYER_COLLISION_DISTANCE;
}

unsigned char DCE_PlayerMeleeCollision(DCE_Player * p, DCE_Player * p2)
{
	float d;

	vec3f_distance(p->position.x, p->position.y, p->position.z,
	               p2->position.x, p2->position.y, p2->position.z, d);
	
	return d < DCE_PLAYER_COLLISION_DISTANCE * 1.5;
}

static vec3f_t *DCE_COLLISION_NORMAL;
static vec3f_t *DCE_COLLISION_E1;

vec3f_t * DCE_CollisionModelNormal()
{
    return DCE_COLLISION_NORMAL;
}

vec3f_t * DCE_CollisionModelE1()
{
    return DCE_COLLISION_E1;
}

unsigned char DCE_CollisionModelTraceRay(DCE_CollisionQuadTreeNode * cm, vec3f_t * p, vec3f_t * r, float * d)
{  
    float cd;
    
    unsigned short int i;

	for(i = 0; i < cm->triangles; i++)
		if(DCE_TraceRayCollisionTri(&cm->tris[i].v1,     // Triangle vertices
                                    &cm->tris[i].edge1,  // Edge 1
                                    &cm->tris[i].edge2,  // Edge 2
                                    p,                   // Ray origin
                                    r,                   // Ray direction
                                    &cd))                // Collision Distance     
			if(cd < *d)                                  // Check to find Nearest Collision Distance
			{
                *d = cd;
                
	            DCE_COLLISION_NORMAL = &cm->tris[i].normal;

	            DCE_COLLISION_E1 = &cm->tris[i].edge1;
		    }
}

unsigned char DCE_CollisionModelTraceGravityRay(DCE_CollisionQuadTreeNode * cm, vec3f_t * O, float * d)
{  
    float cd;
    
    unsigned short int i;

	for(i = 0; i < cm->triangles; i++)
		if(DCE_TraceGravityRayCollisionTri(&cm->tris[i], O, &cd))                // Collision Distance     
			if(cd < *d)                                  // Check to find Nearest Collision Distance
			{
                *d = cd;
                
	            DCE_COLLISION_NORMAL = &cm->tris[i].normal;

	            DCE_COLLISION_E1 = &cm->tris[i].edge1;
		    }
}

unsigned char DCE_PlayerViewsPlayer(DCE_Player * e, DCE_Player * p)
{
	vec3f_t D, V; // Direction Vectors
	float cd, d;  // Collision Distance and Player Distance
    
    // Normalize and Compute Enemy Looking Direction as D
	vec3f_sub_normalize(e->lookAt.x, e->lookAt.y, e->lookAt.z,
	                    e->position.x, e->position.y, e->position.z,
						D.x, D.y, D.z);
					
	// Normalize and Compute Enemy to Player Direction as V
	vec3f_sub_normalize(p->position.x, p->position.y, p->position.z,
	                    e->position.x, e->position.y, e->position.z,
						V.x, V.y, V.z);	
	
	if(e->state & PSTATE_TURNING)
	    vec3f_invert(&D);
	
	// Dot product of D and V gives us the cosine of the angle to determine FOV inlucsion					
	vec3f_dot(D.x, D.y, D.z, V.x, V.y, V.z, d);

    // Return false if the Player is outside of the Enemy FOV
	if(d < DCE_FOV_MIN_DOT2)
	    return 0;
	
	// Get distance from enemy to player    
	vec3f_distance(e->position.x, e->position.y, e->position.z, p->position.x, p->position.y, p->position.z, d);
	
	// Get collision distance from enemy to world collision
	DCE_CheckCollisionCBO(&e->position, &V, &cd);
	
	// Return True if distance from enemy to player is less than collision distance ( world blocks view )
	return d < cd;
}
