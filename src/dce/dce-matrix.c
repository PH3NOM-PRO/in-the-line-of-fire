/* KallistiGL for KallistiOS ##version##

   libgl/gl-matrix.c
   Copyright (C) 2013-2014 Josh Pearson
   Copyright (C) 2014 Lawrence Sebald

   Some functionality adapted from the original KOS libgl:
   Copyright (C) 2001 Dan Potter

   The GL matrix operations use the KOS SH4 matrix operations.
   Basically, we keep two seperate matrix stacks:
   1.) Internal GL API Matrix Stack ( screenview, modelview, etc. ) ( fixed stack size )
   2.) External Matrix Stack for client to push / pop ( size of each stack is determined by MAX_MATRICES )
*/

#include <string.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "dce-vector.h"
#include "dce-matrix.h"

typedef float vector3f[3];  /* 3 float Vector */

void glMatrixMode(GLenum mode) {

}

void glPushMatrix() {

}

void glPopMatrix() {

}

void glLoadIdentity() {

}

void glTranslatef(GLfloat x, GLfloat y, GLfloat z) {

}

void glScalef(GLfloat x, GLfloat y, GLfloat z) {

}

void glRotatef(GLfloat angle, GLfloat x, GLfloat  y, GLfloat z) {

}

/* Load an arbitrary matrix */
void glLoadMatrixf(const GLfloat *m) {

}

/* Load an arbitrary transposed matrix */
void glLoadTransposeMatrixf(const GLfloat *m) {

}

/* Multiply the current matrix by an arbitrary matrix */
void glMultMatrixf(const GLfloat *m) {

}


/* Set the depth range */
void glDepthRange(GLclampf n, GLclampf f) {

}

/* Set the GL viewport */
void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {

}

/* Set the GL frustum */
void glFrustum(GLfloat left, GLfloat right,
               GLfloat bottom, GLfloat top,
               GLfloat znear, GLfloat zfar) {

}

/* Ortho */
void glOrtho(GLfloat left, GLfloat right,
             GLfloat bottom, GLfloat top,
             GLfloat znear, GLfloat zfar) {

}

/* Set the Perspective */
void gluPerspective(GLfloat angle, GLfloat aspect,
                    GLfloat znear, GLfloat zfar) {

}

static vector3f forward, side, up;

/* glhLookAtf2 adapted from http://www.opengl.org/wiki/GluLookAt_code */
void glhLookAtf2(vector3f eyePosition3D,
                 vector3f center3D,
                 vector3f upVector3D) {

}

void DCE_MatrixGetUpVector(vec3f_t * v)
{
}

void DCE_MatrixGetSideVector(vec3f_t * v)
{

}

void DCE_MatrixGetForwardVector(vec3f_t * v)
{

}

void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx,
               GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy,
               GLfloat upz) {

}

void DCE_MatrixApplyRender() {

}

void DCE_MatrixApplyTexture() {

}

void _glKosMatrixLoadRender() {

}

void _glKosMatrixLoadModelView() {

}

void _glKosMatrixLoadModelRot() {

}

void _glKosMatrixApplyScreenSpace() {

}

void DCE_InitMatrix() {

}

