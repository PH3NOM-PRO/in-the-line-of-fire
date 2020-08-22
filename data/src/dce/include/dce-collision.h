/* 
** DCE - Dreamcast Engine (C) 2013-2015 Josh PH3NOM Pearson
*/

#ifndef DCE_COLLISION_H
#define DCE_COLLISION_H

#include <dc/vec3f.h>

#include "dce-player.h"
#include "dce-portal.h"

#include "dce-render-object.h"

#define DCE_COLLISION_OUTSIDE  0
#define DCE_COLLISION_INSIDE_X 1<<0
#define DCE_COLLISION_INSIDE_Y 1<<1
#define DCE_COLLISION_INSIDE_Z 1<<2
#define DCE_COLLISION_INSIDE   7

#define DCE_RAY_EPSILON 0.1f

#define DCE_MAX_RAY_DISTANCE 100000.0f

#define DCE_MAX_PROJECTILE_DISTANCE 2000.0f
#define DCE_MAX_PROJECTILE_RANGE    10.0f

#define DCE_GRAVITY_FACTOR 14.0f

#define DCE_FOV_MIN_DOT 296.0f
#define DCE_FOV_MIN_DOT2 0.2f

#define DCE_PLAYER_COLLISION_DISTANCE 70.0f

unsigned char DCE_CheckCollisionSphere(vec3f_t * v1, float r1, vec3f_t * v2, float r2);

unsigned char DCE_CheckCollisionRay(vec3f_t * start, vec3f_t * end,
                                    vec3f_t * min, vec3f_t * max);

void DCE_PlayerCollisionCallback(DCE_Player * player);
void DCE_PlayerCollisionCallback2(DCE_Player * player);

float DCE_TraceRayCollision(DCE_Player * player, DCE_Player * target, unsigned short int targets);

int DCE_GetRayCollisions();

unsigned char DCE_CheckPlayerAmmoCollision(DCE_Player * player, DCE_Player * target);

float DCE_PointIsInsideFOV(vec3f_t * from, vec3f_t * to, vec3f_t * p);

int DCE_TraceRayTriangleCollision( const vec3f_t * V1,  // Triangle vertices
                                  const vec3f_t * V2,
                                  const vec3f_t * V3,
                                  const vec3f_t * O,  // Ray origin
                                  const vec3f_t * D,  // Ray direction
                                  float * out );


int DCE_TraceRayTriangleGravityCollision( const vec3f_t * V1,  // Triangle vertices
                                  const vec3f_t * V2,
                                  const vec3f_t * V3,
                                  const vec3f_t * O,  // Ray origin
                                  const vec3f_t * D,  // Ray direction
								  const vec3f_t * E1,
								  const vec3f_t * E2,
								  float inv_det,
                                  float * out );

void DCE_CollisionRayTrace(float * V1, float * V2, float * V3, float * D, float * E1, float * E2);

void DCE_Vec3fSub(float * V1, float * V2, float * V3, float * E);

int DCE_TraceRayTriCollision( const vec3f_t * V1,  // Triangle vertices
                              const vec3f_t * V2,
                              const vec3f_t * V3,
                              const vec3f_t * O,  // Ray origin
                              const vec3f_t * D,  // Ray direction
                              const vec3f_t * E,
                              float * out );

int DCE_TraceRayTriCollision2( const vec3f_t * V1,  // Triangle vertices
                              const vec3f_t * V2,
                              const vec3f_t * V3,
                              const vec3f_t * O,  // Ray origin
                              const vec3f_t * D,  // Ray direction
                              const vec3f_t * E,
                              const vec3f_t * P,
                              float det,
                              float * out );

float DCE_PointIsInsideFOV2(vec3f_t * O, vec3f_t * D, vec3f_t * P);

float DCE_PlayerTraceRayCollision(DCE_Player * player, DCE_Player * target, unsigned short int targets);

unsigned char DCE_PlayerCollision(DCE_Player * p, DCE_Player * p2);

DCE_CollisionQuadTreeNode * DCE_LoadCBO(char * fname);

unsigned char DCE_CollisionModelTraceRay(DCE_CollisionQuadTreeNode * cm, vec3f_t * p, vec3f_t * r, float * d);
unsigned char DCE_CollisionModelTraceGravityRay(DCE_CollisionQuadTreeNode * cm, vec3f_t * O, float * d);

vec3f_t * DCE_CollisionModelNormal();
vec3f_t * DCE_CollisionModelE1();

unsigned char DCE_PlayerTraceRayBBox(DCE_Player * player, vec3f_t * bbmin, vec3f_t * bbmax);

// Determine weather or not a Point(P) is inside the FOV of a Origin(O) and Direction(D) Ray(R)
ubyte DCE_PointInView(vec3f_t * O, vec3f_t * D, vec3f_t * P);

unsigned char DCE_TraceRayBBox(vec3f_t * O, vec3f_t * D, vec3f_t * bbmin, vec3f_t * bbmax);

unsigned char DCE_CheckGravityCollisionCBO(vec3f_t * O, float * d);

unsigned char DCE_PointIsInsideFOV3(vec3f_t * O, vec3f_t * D, vec3f_t * P);

unsigned char DCE_PlayerMeleeCollision(DCE_Player * p, DCE_Player * p2);

#endif
