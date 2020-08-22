/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#ifndef DCE_H
#define DCE_H

#include <math.h>
#include <dc/fmath.h>
#include <dc/matrix.h>
#include <dc/matrix3d.h>
#include <dc/pvr.h>
#include <dc/vec3f.h>
#include <dc/video.h>
#include <dc/maple/controller.h>
#include <dc/maple/purupuru.h>

#include "dce-animation.h"
#include "dce-bbox.h"
#include "dce-camera.h"
#include "dce-collision.h"
#include "dce-decals.h"
#include "dce-door.h"
#include "dce-gfx.h"
#include "dce-hud.h"
#include "dce-init.h"
#include "dce-input.h"
#include "dce-matrix.h"
#include "dce-menu.h"
#include "dce-obj.h"
#include "dce-player.h"
#include "dce-player-model.h"
#include "dce-portal.h"
#include "dce-render.h"
#include "dce-rgb.h"
#include "dce-script.h"
#include "dce-shader.h"
#include "dce-sfx.h"
#include "dce-skybox.h"
#include "dce-texture.h"
#include "dce-timer.h"
#include "dce-trigger.h"
#include "dce-vector.h"
#include "dce-waypoint.h"
#include "dce-render-object.h"
#include "dce-enemy.h"

#include "font.h"
#include "md2.h"

#define DCE_TRIGGER_DISTANCE 30.0f

int DCE_RunLevel(char * map);
int DCE_RestartLevel(char * null);

void DCE_StringCopyNoExt(char * dst, char * src);
void DCE_StringCopyNewExt(char * dst, char * src, char * ext);
void DCE_StringHasExt(char * src, char * ext);
uint16 DCE_StringFindCharFirst(char * string, char c);
unsigned char DCE_StringEqualsIgnoreCase(char * src, char * dst);

void DCE_PreComputeMD2VertexFlags();
void DCE_PlayerCollisionCallback(DCE_Player * player);

void DCE_RenderHurtScreenEffect(unsigned char id, unsigned char frames);

#endif
