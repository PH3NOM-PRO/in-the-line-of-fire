#ifndef DCE_RENDER_H
#define DCE_RENDER_H

#include <dc/pvr.h>
#include <dc/vec3f.h>

#include "dce-player.h"

#define CLIP_NONE 0
#define CLIP_1ST 1<<0
#define CLIP_2ND 1<<1
#define CLIP_3RD 1<<2
#define CLIP_1ST_2 (1<<0 | 1<<1)
#define CLIP_1ST_AND_LAST (1<<0 | 1<<2)
#define CLIP_LAST_2 (1<<1 | 1<<2)
#define CLIP_ALL (1<<0 | 1<<1 | 1<<2)

#define DCE_PVR_INVALID_LIST 0xFF

#define SCREEN_HEIGHT_TOP    (int)(((vid_mode->height / 2) / 32)) * 32
#define SCREEN_HEIGHT_BOTTOM (int)(((vid_mode->height / 2) / 32) + 1) * 32

#define DCE_RENDER_TPS_OFFSET 100.0f

//#define RENDER_USE_SCISSOR 1

#define TA_SQ_ADDR (unsigned int *)(void *) \
    (0xe0000000 | (((unsigned long)0x10000000) & 0x03ffffe0))

#define PVR_FOG_LINEAR 0x0
#define PVR_FOG_EXP    0x1
#define PVR_FOG_EXP2   0x2

#define DCE_SHADOWS_NONE    0x0
#define DCE_SHADOWS_FAST    0x1
#define DCE_SHADOWS_STENCIL 0x2

#define DCE_PVR_CULL_FRONT 0x0
#define DCE_PVR_CULL_BACK  0x1

#define DCE_PVR_CULL_CW    0x0
#define DCE_PVR_CULL_CCW   0x1

typedef unsigned char ubyte;

typedef struct {
    uint32  flags;              /**< \brief TA command (vertex flags) */
    float   x;                  /**< \brief X coordinate */
    float   y;                  /**< \brief Y coordinate */
    float   z;                  /**< \brief Z coordinate */
    float   v;                  /**< \brief Texture V coordinate */
    float   u;                  /**< \brief Texture U coordinate */
    uint32  argb;               /**< \brief Vertex color */
    uint32  oargb;               /**< \brief Vertex offset color */
} pvr_cmd_cull_vertex_t;

typedef struct {
    uint32  flags;              /**< \brief TA command (vertex flags) */
    float   x;                  /**< \brief X coordinate */
    float   y;                  /**< \brief Y coordinate */
    float   z;                  /**< \brief Z coordinate */
    float   u;                  /**< \brief Texture U coordinate */
    float   v;                  /**< \brief Texture V coordinate */
    uint32  argb;               /**< \brief Vertex color */
    float   w;                  /**< \brief Vertex W coordinate */
} pvr_cmd_clip_vertex_t;

typedef struct {
    unsigned int flags;      /* Constant PVR_CMD_USERCLIP */
    unsigned int d1, d2, d3; /* Ignored for this type */
    unsigned int sx,         /* Start x */
             sy,         /* Start y */
             ex,         /* End x */
             ey;         /* End y */
} pvr_cmd_tclip_t; /* Tile Clip command for the pvr */

typedef struct {
    unsigned char b, g, r, a;
} colorui;

inline void pvr_list_submit(void *src, int n);

void DCE_InitRender();

void DCE_RenderStart();

void DCE_RenderStartToTexture(void * data, void *w, void *h);

void DCE_RenderSwitchPT();

void DCE_RenderSwitchTR();

void DCE_RenderFinish();

void DCE_RenderFinish();

void DCE_RenderSwitchOPMOD();
void DCE_RenderSwitchTRMOD();

void DCE_RenderSetTexEnv(unsigned char func);

void DCE_RenderSetUVClamp(unsigned char func);

void DCE_RenderEnableSpecular(unsigned char enable);

void DCE_RenderEnableShadowVolumes(unsigned char enable);

void DCE_RenderSetBlendSrc(unsigned char src);

void DCE_RenderSetBlendDst(unsigned char dst);

unsigned char DCE_RenderGetBlendSrc();

unsigned char DCE_RenderGetBlendDst();

void DCE_RenderEnableScissor(unsigned char enable);

void DCE_RenderSetScissor(int x, int y, int width, int height);

void DCE_RenderEnableCull(ubyte enable);
void DCE_RenderSetCullFunc(ubyte func);
void DCE_RenderSetCullFace(ubyte face);

void DCE_RenderCompileAndSubmitHeader(unsigned char list);

void DCE_RenderCompileAndSubmitHeaderTextured(unsigned char list, unsigned short index,
											  unsigned short tex_env);

void DCE_RenderCompileAndSubmitTextureHeader(unsigned short index);

void DCE_RenderTransformTriangles(float *vert_pos,
                                  float vert_stride,
                                  float * uv_coord,
                                  float uv_stride,
                                  unsigned int * argb,
                                  float argb_stride,
                                  float count);

void DCE_RenderModelShadow(float *vert_pos,
                                  float vert_stride,
                                  float * uv_coord,
                                  float uv_stride,
                                  unsigned int * argb,
                                  float argb_stride,
                                  float count);

void DCE_RenderTransformAndClipTriangles(float *vert_pos, int vert_stride,
                                         float *uv_coord, int uv_stride,
				                         unsigned int *argb, int argb_stride,
										 unsigned int count);

void DCE_RenderTransformAndClipTriangleStrip(float *vert_pos, int vert_stride,
                                             float *uv_coord, int uv_stride,
				                             unsigned int *argb, int argb_stride,
										     unsigned int count);

void DCE_RenderTexturedQuad2D(float x1, float y1, float x2, float y2,
							  float u1, float v1, float u2, float v2, unsigned int argb);

void DCE_RenderSubmitTexturedQuad2D(float x, float y, float w, float h,
							        float u1, float v1, float u2, float v2,
									unsigned short texID, unsigned int argb);

/* Private (Internal) Function Definitons */
unsigned char DCE_ClipTransformTriangle(float * vert_pos, float vert_stride,
                                        pvr_cmd_clip_vertex_t *dst,
                                        float * uv_coord, float uv_stride,
										unsigned int * argb, float argb_stride);

void DCE_RenderTesselateDecal(vec3f_t * point, float size, vec3f_t * normal, unsigned short texID,
							  float s, float t, unsigned int color);

void DCE_TesselateDecalQuad(vec3f_t * point, float size, vec3f_t * normal, vec3f_t * v);

inline void DCE_RenderSubmitHeader(uint32 *src);

void DCE_SetBlendFunc(unsigned char func);

void DCE_RenderSetDepthCompare(unsigned short mode);

void DCE_RenderSetDepthWrite(unsigned char enable);

void DCE_RenderCheapShadow(uint16 texID, vec3f_t * v, float size);

void DCE_RenderCheapFlashLight(DCE_Player * player, unsigned short texID,
                               float x1, float x2, float y1, float y2);

void DCE_RenderBoundingBox(vec3f_t * min, vec3f_t * max, unsigned short int texID, unsigned int color);

void DCE_RenderPlayerEntity(DCE_Player * player);

void DCE_RenderPushMultiTex(unsigned short int texID);

void DCE_RenderTransformAndClipTrianglesMultiTex(float *vert_pos, int vert_stride,
                                         float *uv_coord, int uv_stride,
                                         float * uv_coord2, int uv_stride2,
				                         unsigned int *argb, int argb_stride,
										 unsigned int count);

void DCE_RenderMultiTex();

void DCE_RenderTransformTris(float *vert_pos, int vert_stride,
                             float *uv_coord, int uv_stride,
				             unsigned int *argb, int argb_stride,
							 unsigned int count);
							 
void DCE_RenderLoadingScreen(char *texname);							 
void DCE_RenderLoadingScreenPVR(char *texname);

void DCE_RenderPlayerModel(DCE_Player * player);

void DCE_RenderPlayerModelShadow(DCE_Player * player);
void DCE_RenderPlayerModelShadowRayTrace(DCE_Player * player, vec3f_t * light_pos);

void DCE_RenderPlayerModelCull(vec3f_t * from, vec3f_t * to, DCE_Player * player);

void DCE_RenderPlayerModelShadowCull(vec3f_t * from, vec3f_t * to, DCE_Player * player);

void DCE_TesselateCheapShadow(float r);
void DCE_RenderCheapShadowCull(DCE_Player * player, vec3f_t * p, float h, float s);

void DCE_RenderEnableFog(unsigned char enable);
void DCE_RenderFogSetColor(float a, float r, float g, float b);
void DCE_RenderFogSetDensity(unsigned char mode, float density);
void DCE_RenderFogSetLinear(float start, float end);

void DCE_RenderTransformAndSubmitShadow(float *vert_pos, int vert_stride, unsigned int argb, unsigned int count);
void DCE_RenderTransformAndSubmitMDL(float *vert_pos, int vert_stride, 
                                     float *uv, int uv_stride,
									 unsigned int argb, unsigned int count);

void DCE_RenderCompileAndSubmitHeaderBumpMapped(unsigned short index);
	
void DCE_RenderSetMaxFPS(ubyte fps);

float DCE_RenderGetFPS();	
									 
void DCE_RenderIndexedBuffer(vec3f_t * buf, unsigned int *index, unsigned int count);

void DCE_RenderTesselatePointSprite(vec3f_t * p, vec3f_t * d, unsigned short texID, unsigned int argb);
				
void DCE_RenderTransformAndClipTrianglesNC(float *vert_pos, int vert_stride,
                                           float *uv_coord, int uv_stride,
				                           unsigned int count);
void DCE_RenderInitClipBuffer();				
									 
void DCE_RenderTransformAndCullTriangles(float *vert_pos, int vert_stride,
                                         float *uv_coord, int uv_stride,
				                         unsigned int argb, unsigned int count);									 
									
void DCE_RenderModel(vec3f_t *p, float scale, vec3f_t * mtrans, vec3f_t * mscale,
                               unsigned short texID, int tris, float r, pvr_vertex_t * vert);
void DCE_RenderPlayerModelTPP(DCE_Player * player);
void DCE_RenderPlayerLOD(DCE_Player * player, DCE_Player * player2);
void DCE_PlayerHandsModelComputeMatrix(float x, float y, float z, float rx, float ry, float rz, float scale);
void DCE_RenderModelPlayer(DCE_Player * player);									
						
int DCE_RenderCullTriangles( float * vert_src, float * uv_src, unsigned int argb, unsigned int count );						
								 
void DCE_RenderTransformCullTriangles(float *vert_pos,
                            float vert_stride,
                            float * uv_coord,
                            float uv_stride,
                            unsigned int * argb,
                            float argb_stride,
                            float count);								 
		
void DCE_RenderPlayerLODShadow(DCE_Player * player, DCE_Player * player2);
void DCE_RenderPlayerModelShadowTPP(DCE_Player * player);
							
void DCE_RenderPlayerHQMD2(DCE_PlayerModel * mdl, vec3f_t * pos, float r, unsigned char frame);							

int DCE_RenderCullTris( float * vert_src, float * uv_src, int count, int stride);
								 
void DCE_RenderTesselateDecal2(vec3f_t * I, vec3f_t * E1, vec3f_t * N, 
                               float size, unsigned short texID,
							   float s, float t, unsigned int color);
			
//void DCE_SelectScreen(DCE_Player * player, DCE_Camera * cam);			

void DCE_RenderEnemyLODTR(DCE_Player * player, DCE_Player * player2);							  							
							  							
#endif
