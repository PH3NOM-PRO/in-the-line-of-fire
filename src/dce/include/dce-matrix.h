#ifndef DCE_MATRIX_H
#define DCE_MATRIX_H

#include "dce.h"

typedef float matrix4f[4][4];  /* 4x4 float matrix */

/* DEG2RAD - convert Degrees to Radians =  DEG * ( PI / 180.0f ) */
#define DEG2RAD (0.01745329251994329576923690768489)

#define ALIGN32 __attribute__((aligned(32)))

/* Initialze the Matrix Stack */
void DCE_InitMatrices();

/* Set and Store the Render Projection Matrix */
void DCE_MatrixInitRender();

/* Set the Screenview Matrix based on the viewport */
void DCE_MatrixViewport(float x, float y, float width, float height);

/* Set the Look At Matrix  - This Is Applied to the currently Loaded Matrix */
void DCE_MatrixLookAt(vec3f_t * origin, vec3f_t * direction, vec3f_t * upVec);

/* Set the Frustum Matrix */
void DCE_MatrixFrustum(float left, float right,
                       float bottom, float top,
                       float znear, float zfar);

/* Set the Perspective Matrix ( Modifies Frustum Matrix ) */
void DCE_MatrixPerspective(float angle, float aspect, float znear, float zfar);

/* Load the Projection As Screenview x Frustum Matrix */
void DCE_MatrixComputeProjection();

/* Compute Player Camera Matrix */
void DCE_MatrixComputePlayer(DCE_Player * player, DCE_Camera * cam, unsigned char index);

/* Load the Identity Matrix */
inline void DCE_MatrixLoadIdentity();

/* Load the Projection Matrix */
inline void DCE_MatrixLoadProjection();

/* Load the Player Camera Matrix */
inline void DCE_MatrixApplyPlayer(unsigned char index);

#endif
