/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#include <kos.h>

#include "dce.h"


/** Static Variables **************************************************************/

static ubyte DCE_PVR_BLEND_FUNC_SRC  = PVR_BLEND_ONE;
static ubyte DCE_PVR_BLEND_FUNC_DST  = PVR_BLEND_ZERO;
static ubyte DCE_PVR_SHADE_FUNC      = PVR_SHADE_GOURAUD;
static ubyte DCE_PVR_SCISSOR_FUNC    = 0;
static ubyte DCE_PVR_DEPTH_FUNC      = PVR_DEPTHCMP_GEQUAL;
static ubyte DCE_PVR_DEPTH_WRITE     = PVR_DEPTHWRITE_ENABLE;
static ubyte DCE_PVR_ENABLE_SHADOWS  = 0;
static ubyte DCE_PVR_CULL_FUNC       = PVR_CULLING_NONE;
static ubyte DCE_PVR_FACE_FRONT      = 0;
static ubyte DCE_PVR_ENABLE_CULL     = 0;
static ubyte DCE_PVR_FOG_FUNC        = PVR_FOG_TABLE;
static ubyte DCE_PVR_ENABLE_FOG      = 0;
static ubyte DCE_PVR_ENABLE_SPECULAR = PVR_SPECULAR_DISABLE;
static ubyte DCE_PVR_UV_CLAMP_FUNC   = 0;
static ubyte DCE_PVR_TEX_ENV         = PVR_TXRENV_MODULATEALPHA;

static pvr_poly_cxt_t DCE_PVR_POLY_CXT;

static pvr_vertex_t DCE_PVR_QUAD[4];
static pvr_vertex_t DCE_SPRITE_VERT[4];
static pvr_cmd_clip_vertex_t DCE_PVR_CLIP_VERTEX[4] __attribute__((aligned(32)));

static unsigned char DCE_PVR_LIST = DCE_PVR_INVALID_LIST;

#define DCE_VERTEX_BUFFER_MAX 1024 * 16

static pvr_cmd_clip_vertex_t DCE_CLIP_BUFFER[DCE_VERTEX_BUFFER_MAX] __attribute__((aligned(32)));
static unsigned int DCE_CLIP_VERTICES = 0;

static unsigned long int DCE_RENDER_START;
static unsigned char DCE_PVR_MAX_FPS = 100;
static float DCE_PVR_MIN_FRAME_TIME = 1000.0f / 33.0f;

static pvr_stats_t DCE_PVR_STATS;

/** Framerate Management **********************************************************/

void DCE_RenderSetMaxFPS(ubyte fps)
{
	DCE_PVR_MAX_FPS = fps;
}

float DCE_RenderGetFPS()
{
	return DCE_PVR_STATS.frame_rate;
}

/** Set Texture Environment *****************************************************/

void DCE_RenderSetTexEnv(unsigned char func)
{
	DCE_PVR_TEX_ENV = func;
}

/** Enable Texture U/V Clamping *************************************************/

void DCE_RenderSetUVClamp(unsigned char func)
{
	DCE_PVR_UV_CLAMP_FUNC = func;
}

/** Enable Specular Highlights **************************************************/

void DCE_RenderEnableSpecular(unsigned char enable)
{
	DCE_PVR_ENABLE_SPECULAR = enable;
}

/** Culling Functions *************************************************************/

void DCE_RenderEnableCull(ubyte enable)
{
	DCE_PVR_ENABLE_CULL = enable;
}

void DCE_RenderSetCullFunc(ubyte func)
{
	DCE_PVR_CULL_FUNC = func;
}

void DCE_RenderSetCullFace(ubyte face)
{
	DCE_PVR_FACE_FRONT = face;
}

/** Fog Functions *****************************************************************/

void DCE_RenderFogSetColor(float a, float r, float g, float b)
{
    pvr_fog_table_color(a, r, g, b);
}

void DCE_RenderFogSetDensity(unsigned char mode, float density)
{
    switch(mode)
    {
        case PVR_FOG_EXP:
             DCE_PVR_FOG_FUNC = PVR_FOG_TABLE;
             pvr_fog_table_exp(density);
             break;
             
        case PVR_FOG_EXP2:
             DCE_PVR_FOG_FUNC = PVR_FOG_TABLE2;
             pvr_fog_table_exp2(density);
             break;             
    }
}

void DCE_RenderFogSetLinear(float start, float end)
{
    DCE_PVR_FOG_FUNC = PVR_FOG_TABLE;
    pvr_fog_table_linear(start, end);
}

void DCE_RenderEnableFog(unsigned char enable)
{
    DCE_PVR_ENABLE_FOG = enable;
}

void DCE_RenderEnableShadowVolumes(unsigned char enable)
{
    DCE_PVR_ENABLE_SHADOWS = enable;
}

/** PVR Interface *****************************************************************/

/* Custom version of sq_cpy from KOS for copying a buffer the PVR */
inline void DCE_RenderSubmitPVR(void *src, int n) {
    uint32 *d = (uint32 *)TA_SQ_ADDR;
    uint32 *s = src;

    /* fill/write queues as many times necessary */
    while(n--) {
        asm("pref @%0" : : "r"(s + 8));  /* prefetch 32 bytes for next loop */
        d[0] = *(s++);
        d[1] = *(s++);
        d[2] = *(s++);
        d[3] = *(s++);
        d[4] = *(s++);
        d[5] = *(s++);
        d[6] = *(s++);
        d[7] = *(s++);
        asm("pref @%0" : : "r"(d));
        d += 8;
    }

    /* Wait for both store queues to complete */
    d = (uint32 *)0xe0000000;
    d[0] = d[8] = 0;
}

/* Custom version of sq_cpy from KOS for copying a pvr header or vertex to the PVR */
inline void DCE_RenderSubmitHeader(uint32 *src)
{
    uint32 *d = (uint32 *)TA_SQ_ADDR;

    d[0] = *(src++);
    d[1] = *(src++);
    d[2] = *(src++);
    d[3] = *(src++);
    d[4] = *(src++);
    d[5] = *(src++);
    d[6] = *(src++);
    d[7] = *(src++);
    
    asm("pref @%0" : : "r"(d));
}

void DCE_InitRender()
{
    pvr_init_params_t params = {

        /* Enable opaque and translucent polygons with size 32 and 32 */
        { PVR_BINSIZE_32, PVR_BINSIZE_0, PVR_BINSIZE_32, PVR_BINSIZE_0, PVR_BINSIZE_8 },

        65536 * 32, /* Vertex buffer size */

        0, /* No DMA */


        0, /* No FSAA */
        
        0 /* Autosort Disabled */
    };
    
    pvr_init(&params);
    //while(1)
    printf("\nDCE: PVR Initialized.  VRAM Available: %i\n", pvr_mem_available());
    
	DCE_PVR_QUAD[0].flags = DCE_PVR_QUAD[1].flags = DCE_PVR_QUAD[2].flags = PVR_CMD_VERTEX;
	DCE_PVR_QUAD[3].flags = PVR_CMD_VERTEX_EOL;
	
	DCE_PVR_QUAD[0].z = DCE_PVR_QUAD[1].z = DCE_PVR_QUAD[2].z = DCE_PVR_QUAD[3].z = 1.0f;

    DCE_SPRITE_VERT[0].flags = DCE_SPRITE_VERT[1].flags = DCE_SPRITE_VERT[2].flags = PVR_CMD_VERTEX;
    DCE_SPRITE_VERT[3].flags = PVR_CMD_VERTEX_EOL;
        
    DCE_SPRITE_VERT[0].u = 0;
    DCE_SPRITE_VERT[0].v = 0;
    DCE_SPRITE_VERT[1].u = 1;
    DCE_SPRITE_VERT[1].v = 0;
    DCE_SPRITE_VERT[2].u = 0;
    DCE_SPRITE_VERT[2].v = 1; 
    DCE_SPRITE_VERT[3].u = 1;
    DCE_SPRITE_VERT[3].v = 1;	 
	
    DCE_TesselateCheapShadow(15.0f);
}

void DCE_RenderStart()
{
	DCE_RENDER_START = DCE_GetTime();
	
    pvr_wait_ready();
    pvr_scene_begin();
    pvr_list_begin(PVR_LIST_OP_POLY);     
        
    DCE_PVR_LIST = PVR_LIST_OP_POLY;  
}

void DCE_RenderStartToTexture(void * data, void *w, void *h)
{
	DCE_RENDER_START = DCE_GetTime();
	
    pvr_wait_ready();
    pvr_scene_begin_txr(data, w, h);
    pvr_list_begin(PVR_LIST_OP_POLY);   
        
    DCE_PVR_LIST = PVR_LIST_OP_POLY;   
}

void DCE_RenderSwitchOPMOD()
{
    pvr_list_finish();
    
    pvr_list_begin(PVR_LIST_OP_MOD);
}

void DCE_RenderSwitchPT()
{
	pvr_list_finish();
	pvr_list_begin(PVR_LIST_PT_POLY);
	
	DCE_PVR_LIST = PVR_LIST_PT_POLY;    
}

void DCE_RenderSwitchTR()
{
	pvr_list_finish();
	pvr_list_begin(PVR_LIST_TR_POLY);
	
	DCE_PVR_LIST = PVR_LIST_TR_POLY;    
}

void DCE_RenderSwitchTRMOD()
{
    pvr_list_finish();
    
    pvr_list_begin(PVR_LIST_TR_MOD);
}

void DCE_RenderFinish()
{
    pvr_list_finish();
    
    pvr_scene_finish();
    
    pvr_get_stats(&DCE_PVR_STATS);

    while(DCE_GetTime() - DCE_RENDER_START < DCE_PVR_MIN_FRAME_TIME)
        thd_pass();
      
    DCE_PVR_LIST = DCE_PVR_INVALID_LIST; 
}

/******************************** Capability Functions *******************************/

void DCE_RenderSetBlendSrc(unsigned char src)
{
	DCE_PVR_BLEND_FUNC_SRC = src;
}

void DCE_RenderSetBlendDst(unsigned char dst)
{
	DCE_PVR_BLEND_FUNC_DST = dst;
}

unsigned char DCE_RenderGetBlendSrc()
{
	return DCE_PVR_BLEND_FUNC_SRC;
}

unsigned char DCE_RenderGetBlendDst()
{
	return DCE_PVR_BLEND_FUNC_DST;
}

void DCE_RenderEnableScissor(unsigned char enable)
{
	DCE_PVR_SCISSOR_FUNC = enable;
}

void DCE_SetBlendFunc(unsigned char func)
{
    DCE_PVR_SHADE_FUNC = func;
}

void DCE_RenderSetDepthCompare(unsigned short mode)
{
    DCE_PVR_DEPTH_FUNC = mode;
}

void DCE_RenderSetDepthWrite(unsigned char enable)
{
    DCE_PVR_DEPTH_WRITE = enable;     
}     

static inline void DCE_RenderApplyDepthFunc()
{
    DCE_PVR_POLY_CXT.depth.comparison = DCE_PVR_DEPTH_FUNC;

    DCE_PVR_POLY_CXT.depth.write = DCE_PVR_DEPTH_WRITE;
}

static inline void DCE_RenderApplyFogFunc()
{
    if(DCE_PVR_ENABLE_FOG)
        DCE_PVR_POLY_CXT.gen.fog_type = DCE_PVR_FOG_FUNC;
}

static inline void DCE_RenderApplySpecularFunc()
{
    if(DCE_PVR_ENABLE_FOG)
        DCE_PVR_POLY_CXT.gen.specular = DCE_PVR_ENABLE_SPECULAR;
}

static inline void DCE_RenderApplyCullingFunc()
{
    if(DCE_PVR_ENABLE_CULL) {
        if(DCE_PVR_FACE_FRONT == DCE_PVR_CULL_BACK) {
            if(DCE_PVR_CULL_FUNC == DCE_PVR_CULL_CW )
                DCE_PVR_POLY_CXT.gen.culling = PVR_CULLING_CCW;
            else
                DCE_PVR_POLY_CXT.gen.culling = PVR_CULLING_CW;
        }
        else if(DCE_PVR_FACE_FRONT == DCE_PVR_CULL_FRONT) {
            if(DCE_PVR_CULL_FUNC == DCE_PVR_CULL_CCW)
                DCE_PVR_POLY_CXT.gen.culling = PVR_CULLING_CCW;
            else
                DCE_PVR_POLY_CXT.gen.culling = PVR_CULLING_CW;
        }
    }
    else
        DCE_PVR_POLY_CXT.gen.culling = PVR_CULLING_NONE;
}

/******************************** Submission Functions *******************************/

void DCE_RenderTexturedQuad2D(float x1, float y1, float x2, float y2,
							  float u1, float v1, float u2, float v2, unsigned int argb)
{
	DCE_PVR_QUAD[0].argb = DCE_PVR_QUAD[1].argb = DCE_PVR_QUAD[2].argb = 
	DCE_PVR_QUAD[3].argb = argb;

	DCE_PVR_QUAD[0].u = DCE_PVR_QUAD[2].u = u1;
	DCE_PVR_QUAD[1].u = DCE_PVR_QUAD[3].u = u2;

	DCE_PVR_QUAD[0].v = DCE_PVR_QUAD[1].v = v1;
	DCE_PVR_QUAD[2].v = DCE_PVR_QUAD[3].v = v2;

	DCE_PVR_QUAD[0].x = DCE_PVR_QUAD[2].x = x1;
	DCE_PVR_QUAD[1].x = DCE_PVR_QUAD[3].x = x2;

	DCE_PVR_QUAD[0].y = DCE_PVR_QUAD[1].y = y1;
	DCE_PVR_QUAD[2].y = DCE_PVR_QUAD[3].y = y2;

	DCE_RenderSubmitHeader((uint32 *)&DCE_PVR_QUAD[0]);
	DCE_RenderSubmitHeader((uint32 *)&DCE_PVR_QUAD[1]);
	DCE_RenderSubmitHeader((uint32 *)&DCE_PVR_QUAD[2]);
	DCE_RenderSubmitHeader((uint32 *)&DCE_PVR_QUAD[3]);
}

void DCE_RenderSubmitTexturedQuad2D(float x, float y, float w, float h,
							        float u1, float v1, float u2, float v2,
									unsigned short texID, unsigned int argb)
{
	DCE_RenderCompileAndSubmitTextureHeader(texID);
	
	DCE_PVR_QUAD[0].argb = DCE_PVR_QUAD[1].argb = DCE_PVR_QUAD[2].argb = 
	DCE_PVR_QUAD[3].argb = argb;

	DCE_PVR_QUAD[0].u = DCE_PVR_QUAD[2].u = u1;
	DCE_PVR_QUAD[1].u = DCE_PVR_QUAD[3].u = u2;

	DCE_PVR_QUAD[0].v = DCE_PVR_QUAD[1].v = v1;
	DCE_PVR_QUAD[2].v = DCE_PVR_QUAD[3].v = v2;

	DCE_PVR_QUAD[0].x = DCE_PVR_QUAD[2].x = x - (w * .5);
	DCE_PVR_QUAD[1].x = DCE_PVR_QUAD[3].x = x + (w * .5);

	DCE_PVR_QUAD[0].y = DCE_PVR_QUAD[1].y = y - (h * .5);
	DCE_PVR_QUAD[2].y = DCE_PVR_QUAD[3].y = y + (h * .5);

    sq_cpy(0x10000000, &DCE_PVR_QUAD[0], 32 * 4);
}

void DCE_RenderCompileAndSubmitHeader(unsigned char list)
{
    pvr_poly_hdr_t hdr;
    
    pvr_poly_cxt_col(&DCE_PVR_POLY_CXT, DCE_PVR_LIST);

    DCE_PVR_POLY_CXT.gen.shading = DCE_PVR_SHADE_FUNC;

    DCE_RenderApplyDepthFunc();

    DCE_RenderApplyCullingFunc();
    
	if(DCE_PVR_SCISSOR_FUNC)
		DCE_PVR_POLY_CXT.gen.clip_mode = PVR_USERCLIP_INSIDE;
    
    DCE_RenderApplyFogFunc();
        
    pvr_poly_compile(&hdr, &DCE_PVR_POLY_CXT);

    DCE_RenderSubmitHeader((uint32 *)&hdr);
}

void DCE_RenderCompileAndSubmitTextureHeaderFilter(unsigned short index, unsigned char filter)
{
    DCE_Texture * tex = DCE_TextureObject(index);
	
	pvr_poly_hdr_t hdr;

    pvr_poly_cxt_txr(&DCE_PVR_POLY_CXT,
                     DCE_PVR_LIST,
                     tex->color,
                     tex->w,
                     tex->h,
                     tex->data,
                     tex->filter);

    if(DCE_PVR_ENABLE_SHADOWS)
    {
        DCE_PVR_POLY_CXT.gen.modifier_mode = PVR_MODIFIER_CHEAP_SHADOW;
        DCE_PVR_POLY_CXT.fmt.modifier = PVR_MODIFIER_ENABLE;
    }

    DCE_PVR_POLY_CXT.gen.shading = DCE_PVR_SHADE_FUNC;

	if(DCE_PVR_LIST == PVR_LIST_TR_POLY)
	{
        DCE_PVR_POLY_CXT.blend.src = DCE_PVR_BLEND_FUNC_SRC;
        DCE_PVR_POLY_CXT.blend.dst = DCE_PVR_BLEND_FUNC_DST;
	    DCE_PVR_POLY_CXT.txr.env = DCE_PVR_TEX_ENV;
	}

	DCE_PVR_POLY_CXT.txr.filter = filter;

    DCE_RenderApplyDepthFunc();

    DCE_RenderApplyCullingFunc();

	if(DCE_PVR_SCISSOR_FUNC)
		DCE_PVR_POLY_CXT.gen.clip_mode = PVR_USERCLIP_INSIDE;
    
    DCE_RenderApplyFogFunc();
    
    DCE_RenderApplySpecularFunc();
    
    DCE_PVR_POLY_CXT.txr.uv_clamp = DCE_PVR_UV_CLAMP_FUNC;
    
	pvr_poly_compile(&hdr, &DCE_PVR_POLY_CXT);

    DCE_RenderSubmitHeader((uint32 *)&hdr);
}

void DCE_RenderCompileAndSubmitTextureHeader(unsigned short index)
{
    DCE_Texture * tex = DCE_TextureObject(index);
	
	pvr_poly_hdr_t hdr;

    pvr_poly_cxt_txr(&DCE_PVR_POLY_CXT,
                     DCE_PVR_LIST,
                     tex->color,
                     tex->w,
                     tex->h,
                     tex->data,
                     tex->filter);

    if(DCE_PVR_ENABLE_SHADOWS)
    {
        DCE_PVR_POLY_CXT.gen.modifier_mode = PVR_MODIFIER_CHEAP_SHADOW;
        DCE_PVR_POLY_CXT.fmt.modifier = PVR_MODIFIER_ENABLE;
    }

    DCE_PVR_POLY_CXT.gen.shading = DCE_PVR_SHADE_FUNC;

	if(DCE_PVR_LIST == PVR_LIST_TR_POLY)
	{
        DCE_PVR_POLY_CXT.blend.src = DCE_PVR_BLEND_FUNC_SRC;
        DCE_PVR_POLY_CXT.blend.dst = DCE_PVR_BLEND_FUNC_DST;
	    DCE_PVR_POLY_CXT.txr.env = DCE_PVR_TEX_ENV;
	}

	DCE_PVR_POLY_CXT.txr.filter = PVR_FILTER_BILINEAR;

    DCE_RenderApplyDepthFunc();

    DCE_RenderApplyCullingFunc();

	if(DCE_PVR_SCISSOR_FUNC)
		DCE_PVR_POLY_CXT.gen.clip_mode = PVR_USERCLIP_INSIDE;
    
    DCE_RenderApplyFogFunc();
    
    DCE_RenderApplySpecularFunc();
    
    DCE_PVR_POLY_CXT.txr.uv_clamp = DCE_PVR_UV_CLAMP_FUNC;
    
	pvr_poly_compile(&hdr, &DCE_PVR_POLY_CXT);

    DCE_RenderSubmitHeader((uint32 *)&hdr);
}

void DCE_RenderCompileAndSubmitHeaderTextured(unsigned char list, unsigned short index,
											  unsigned short tex_env) {
    DCE_Texture * tex = DCE_TextureObject(index);
	
	pvr_poly_hdr_t hdr;

    pvr_poly_cxt_txr(&DCE_PVR_POLY_CXT,
                     DCE_PVR_LIST,
                     tex->color,
                     tex->w,
                     tex->h,
                     tex->data,
                     tex->filter);

    if(DCE_PVR_ENABLE_SHADOWS)
    {
        DCE_PVR_POLY_CXT.gen.modifier_mode = PVR_MODIFIER_CHEAP_SHADOW;
        DCE_PVR_POLY_CXT.fmt.modifier = PVR_MODIFIER_ENABLE;
    }

    DCE_PVR_POLY_CXT.gen.shading = DCE_PVR_SHADE_FUNC;

	if(DCE_PVR_LIST == PVR_LIST_TR_POLY)
	{
        DCE_PVR_POLY_CXT.blend.src = DCE_PVR_BLEND_FUNC_SRC;
        DCE_PVR_POLY_CXT.blend.dst = DCE_PVR_BLEND_FUNC_DST;
	    DCE_PVR_POLY_CXT.txr.env = DCE_PVR_TEX_ENV;
	}

	DCE_PVR_POLY_CXT.txr.filter = PVR_FILTER_BILINEAR;

    DCE_RenderApplyDepthFunc();

    DCE_RenderApplyCullingFunc();

	if(DCE_PVR_SCISSOR_FUNC)
		DCE_PVR_POLY_CXT.gen.clip_mode = PVR_USERCLIP_INSIDE;
    
    DCE_RenderApplyFogFunc();
    
    DCE_RenderApplySpecularFunc();
    
    DCE_PVR_POLY_CXT.txr.uv_clamp = DCE_PVR_UV_CLAMP_FUNC;
    
	pvr_poly_compile(&hdr, &DCE_PVR_POLY_CXT);

    DCE_RenderSubmitHeader((uint32 *)&hdr);
}

/* Clamp X to [MIN,MAX]: */
#define CLAMP( X, MIN, MAX )  ( (X)<(MIN) ? (MIN) : ((X)>(MAX) ? (MAX) : (X)) )

void DCE_RenderSetScissor(int x, int y, int width, int height) {
    pvr_cmd_tclip_t c;

    int pvr_scissor_width = CLAMP(width, 0, vid_mode->width);
    int pvr_scissor_height = CLAMP(height, 0, vid_mode->height);

    /* force the origin to the lower left-hand corner of the screen */
    int miny = (vid_mode->height - pvr_scissor_height) - y;
    int maxx = (pvr_scissor_width + x);
    int maxy = (pvr_scissor_height + miny);

    /* load command structure while mapping screen coords to TA tiles */
    c.flags = PVR_CMD_USERCLIP;
    c.d1 = c.d2 = c.d3 = 0;
    c.sx = CLAMP(x / 32, 0, vid_mode->width / 32);
    c.sy = CLAMP(miny / 32, 0, vid_mode->height / 32);
    c.ex = CLAMP((maxx / 32) - 1, 0, vid_mode->width / 32);
    c.ey = CLAMP((maxy / 32) - 1, 0, vid_mode->height / 32);

    DCE_RenderSubmitHeader((uint32 *)&c);
}

//////////////////////////////////////////////////////////////////////////////////////
// Clipping Render Code

#define fclamp0_1(n) n > 1.0f ? 1.0f : n < 0.0f ? 0.0f : n

static pvr_dr_state_t dr_state;

static inline void DCE_SetVertUV(pvr_cmd_clip_vertex_t * v, float * uv)
{
    v->u = uv[0];
    v->v = uv[1];
}       

static inline void DCE_PerspectiveDivideAndSubmit(pvr_cmd_clip_vertex_t * V, vec2f_t * U, unsigned int argb)
{
    pvr_vertex_t * v = pvr_dr_target(dr_state);       

    v->z = 1.0f / V->w;
    
    v->u = U->u;
    v->v = U->v;
    v->argb = argb;
    v->flags = PVR_CMD_VERTEX;
    
    v->x = V->x * v->z;
    v->y = V->y * v->z;
    
	pvr_dr_commit(v);     
}

static inline void DCE_PerspectiveDivideAndSubmitEOL(pvr_cmd_clip_vertex_t * V, vec2f_t * U, unsigned int argb)
{
    pvr_vertex_t * v = pvr_dr_target(dr_state);       

    v->z = 1.0f / V->w;
    
    v->u = U->u;
    v->v = U->v;
    v->argb = argb;
    v->flags = PVR_CMD_VERTEX_EOL;
    
    v->x = V->x * v->z;
    v->y = V->y * v->z;
    
	pvr_dr_commit(v);     
}

static inline void DCE_ClipPerspectiveDivideAndSubmitEOL(pvr_cmd_clip_vertex_t * V1, vec2f_t * U1, unsigned int argb1,
                                                         pvr_cmd_clip_vertex_t * V2, vec2f_t * U2, unsigned int argb2)
{
    float l = fclamp0_1(((-1.0f - V1->z) / (V2->z - V1->z))); // Displacement Magnitude = Interpolation Factor    

	pvr_vertex_t * v = pvr_dr_target(dr_state);    // Dest Vertex = Direct Render Address
    
	colorui *c1 = (colorui *)&argb1;               // Source Color Pointer 1 for a, r, g, b component manipulation
    
	colorui *c2 = (colorui *)&argb2;               // Source Color Pointer 2 for a, r, g, b component manipulation 
              
    v->u = V1->u + ((V2->u - V1->u) * l);          // Lerp U, V
    v->v = V1->v + ((V2->v - V1->v) * l);

    v->z = 1.0f / (V1->w + ((V2->w - V1->w) * l)); // Z = 1.0f / ( Lerp W )
  
    v->argb = PVR_PACK_COLOR(c1->a + ((c2->a - c1->a) * l),  // Lerp A, R, G, B
	                         c1->r + ((c2->r - c1->r) * l),
	                         c1->g + ((c2->g - c1->g) * l),
	                         c1->b + ((c2->b - c1->b)));
    
    v->x = (V1->x + ((V2->x - V1->x) * l)) * v->z; // Lerp And Perspective Divide X, Y
    v->y = (V1->y + ((V2->y - V1->y) * l)) * v->z;        
	
	v->flags = PVR_CMD_VERTEX_EOL;
	
	pvr_dr_commit(v);      
}

static inline void DCE_ClipPerspectiveDivideAndSubmit(pvr_cmd_clip_vertex_t * V1, vec2f_t * U1, unsigned int argb1,
                                                      pvr_cmd_clip_vertex_t * V2, vec2f_t * U2, unsigned int argb2)
{
    float l = fclamp0_1(((-1.0f - V1->z) / (V2->z - V1->z))); // Displacement Magnitude = Interpolation Factor    

	pvr_vertex_t * v = pvr_dr_target(dr_state);    // Dest Vertex = Direct Render Address
    
	colorui *c1 = (colorui *)&argb1;               // Source Color Pointer 1 for a, r, g, b component manipulation
    
	colorui *c2 = (colorui *)&argb2;               // Source Color Pointer 2 for a, r, g, b component manipulation 
    
    unsigned int c;             // Dest Color Pointer
              
    v->u = V1->u + ((V2->u - V1->u) * l);          // Lerp U, V
    v->v = V1->v + ((V2->v - V1->v) * l);

    v->z = 1.0f / (V1->w + ((V2->w - V1->w) * l)); // Z = 1.0f / ( Lerp W )

    v->argb = PVR_PACK_COLOR(c1->a + ((c2->a - c1->a) * l),
	                         c1->r + ((c2->r - c1->r) * l),
	                         c1->g + ((c2->g - c1->g) * l),
	                         c1->b + ((c2->b - c1->b)));
    
    v->x = (V1->x + ((V2->x - V1->x) * l)) * v->z; // Lerp And Perspective Divide X, Y
    v->y = (V1->y + ((V2->y - V1->y) * l)) * v->z;        
	
	v->flags = PVR_CMD_VERTEX;
	
	pvr_dr_commit(v);      
}                                                        

static inline void DCE_SetFlagsTri(pvr_cmd_clip_vertex_t * v)
{
    v[0].flags = v[1].flags = PVR_CMD_VERTEX;
    v[2].flags = PVR_CMD_VERTEX_EOL;
}

static inline ubyte DCE_RenderClipTri(float *vert_pos, int vert_stride,
                                              float *uv_coord, int uv_stride,
                                              unsigned int *argb, int argb_stride,
											  pvr_cmd_clip_vertex_t *dst) {
    uint16 clip = 0; /* Clip Code for current Triangle */

	mat_trans_single3_nodivw_nomod(vert_pos[0], vert_pos[1], vert_pos[2], dst[0].x, dst[0].y, dst[0].z, dst[0].w);
    vert_pos += vert_stride;
    
	mat_trans_single3_nodivw_nomod(vert_pos[0], vert_pos[1], vert_pos[2], dst[1].x, dst[1].y, dst[1].z, dst[1].w);
    vert_pos += vert_stride;

	mat_trans_single3_nodivw_nomod(vert_pos[0], vert_pos[1], vert_pos[2], dst[2].x, dst[2].y, dst[2].z, dst[2].w);
    vert_pos += vert_stride;
    
    if(dst[0].z > -1.0f)
        clip |= CLIP_1ST;
        
    if(dst[1].z > -1.0f)
        clip |= CLIP_2ND;
    
    if(dst[2].z > -1.0f)
        clip |= CLIP_3RD;                		
                        						 
    if(clip == CLIP_ALL) /* All Verts Out - Output 0 Vertices */
        return 0;

    if(clip == CLIP_NONE)
    {
    	DCE_PerspectiveDivideAndSubmit(&dst[0], uv_coord, argb[0]);
    	DCE_PerspectiveDivideAndSubmit(&dst[1], &uv_coord[uv_stride], argb[argb_stride]);
    	DCE_PerspectiveDivideAndSubmitEOL(&dst[2], &uv_coord[uv_stride * 2], argb[argb_stride * 2]);
    	return 0;
    }

    DCE_SetVertUV(&dst[0], uv_coord);
    DCE_SetVertUV(&dst[1], &uv_coord[uv_stride]);
    DCE_SetVertUV(&dst[2], &uv_coord[uv_stride * 2]);    
    
    //0 &dst[0], &uv_coord[0], argb[0]
    //1 &dst[1], &uv_coord[uv_stride], argb[argb_stride]
    //2 &dst[2], &uv_coord[uv_stride<<1], argb[argb_stride<<1]
    
    switch(clip) { /* Start by examining # of vertices inside clip plane */
        case CLIP_1ST: //0 1 0 2            
            DCE_ClipPerspectiveDivideAndSubmit(&dst[0], &uv_coord[0], argb[0], &dst[1],
			                                   &uv_coord[uv_stride], argb[argb_stride]);
			                                   
            DCE_PerspectiveDivideAndSubmit(&dst[1], &uv_coord[uv_stride], argb[argb_stride]);
            
            DCE_ClipPerspectiveDivideAndSubmit(&dst[0], &uv_coord[0], argb[0],
			                                   &dst[2], &uv_coord[uv_stride<<1], argb[argb_stride<<1]);
			                                   
            DCE_PerspectiveDivideAndSubmitEOL(&dst[2], &uv_coord[uv_stride<<1], argb[argb_stride<<1]);
            
            return 0;

        case CLIP_2ND: //1 2 1 0
            DCE_ClipPerspectiveDivideAndSubmit(&dst[1], &uv_coord[uv_stride], argb[argb_stride],
			                                   &dst[2], &uv_coord[uv_stride<<1], argb[argb_stride<<1]);
			                                   
            DCE_PerspectiveDivideAndSubmit(&dst[2], &uv_coord[uv_stride<<1], argb[argb_stride<<1]);
            
            DCE_ClipPerspectiveDivideAndSubmit(&dst[1], &uv_coord[uv_stride], argb[argb_stride],
			                                   &dst[0], &uv_coord[0], argb[0]);
            
            DCE_PerspectiveDivideAndSubmitEOL(&dst[0], &uv_coord[0], argb[0]);
            
            return 0;

        case CLIP_3RD: //2 0 2 1
            DCE_ClipPerspectiveDivideAndSubmit(&dst[2], &uv_coord[uv_stride<<1], argb[argb_stride<<1],
			                                   &dst[0], &uv_coord[0], argb[0]);
			                                   
            DCE_PerspectiveDivideAndSubmit(&dst[0], &uv_coord[0], argb[0]);
            
            DCE_ClipPerspectiveDivideAndSubmit(&dst[2], &uv_coord[uv_stride<<1], argb[argb_stride<<1],
			                                   &dst[1], &uv_coord[uv_stride], argb[argb_stride]);
            
            DCE_PerspectiveDivideAndSubmitEOL(&dst[1], &uv_coord[uv_stride], argb[argb_stride]);
            
            return 0;
            
        case CLIP_1ST_2: // 0->2, 1->2, 2        
            DCE_ClipPerspectiveDivideAndSubmit(&dst[0], &uv_coord[0], argb[0],
			                                   &dst[2], &uv_coord[uv_stride<<1], argb[argb_stride<<1]);

            DCE_ClipPerspectiveDivideAndSubmit(&dst[1], &uv_coord[uv_stride], argb[argb_stride],
			                                   &dst[2], &uv_coord[uv_stride<<1], argb[argb_stride<<1]);
			                                   
			DCE_PerspectiveDivideAndSubmitEOL(&dst[2], &uv_coord[uv_stride<<1], argb[argb_stride<<1]);
			                                   
            return 0;

        case CLIP_1ST_AND_LAST:  // 0->1, 1, 2->1   
            DCE_ClipPerspectiveDivideAndSubmit(&dst[0], &uv_coord[0], argb[0],
			                                   &dst[1], &uv_coord[uv_stride], argb[argb_stride]);

			DCE_PerspectiveDivideAndSubmit(&dst[1], &uv_coord[uv_stride], argb[argb_stride]);

            DCE_ClipPerspectiveDivideAndSubmitEOL(&dst[2], &uv_coord[uv_stride<<1], argb[argb_stride<<1],
			                                      &dst[1], &uv_coord[uv_stride], argb[argb_stride]);
			                                			                                   
            return 0;

        case CLIP_LAST_2: // 0, 1->0, 2->0           
			DCE_PerspectiveDivideAndSubmit(&dst[0], &uv_coord[0], argb[0]);
			
            DCE_ClipPerspectiveDivideAndSubmit(&dst[1], &uv_coord[uv_stride], argb[argb_stride],
			                                   &dst[0], &uv_coord[0], argb[0]);

            DCE_ClipPerspectiveDivideAndSubmitEOL(&dst[2], &uv_coord[uv_stride<<1], argb[argb_stride<<1],
			                                      &dst[0], &uv_coord[0], argb[0]);
			                                			                                   
            return 0;
    }

    return 0;
}

void DCE_RenderTransformAndClipTriangles(float *vert_pos, int vert_stride,
                                         float *uv_coord, int uv_stride,
				                         unsigned int *argb, int argb_stride,
										 unsigned int count)
{
    unsigned int i;  

	int vs = vert_stride / 4;
	int ts = uv_stride / 4;
	int cs = argb_stride / 4;

    pvr_dr_init(dr_state);

    for(i = 0; i < count; i += 3)
		DCE_RenderClipTri(&vert_pos[i * vs], vs, &uv_coord[i * ts], ts,
						 &argb[i * cs], cs, DCE_PVR_CLIP_VERTEX);
}

void DCE_RenderTransformAndClipTriangleStrip(float *vert_pos, int vert_stride,
                                             float *uv_coord, int uv_stride,
				                             unsigned int *argb, int argb_stride,
										     unsigned int count)
{
    unsigned int i;  

	int vs = vert_stride / 4;
	int ts = uv_stride / 4;
	int cs = argb_stride / 4;

    pvr_dr_init(dr_state);

    for(i = 0; i < count - 2; i++)
		DCE_RenderClipTri(&vert_pos[i * vs], vs, &uv_coord[i * ts], ts,
						 &argb[i * cs], cs, DCE_PVR_CLIP_VERTEX);
}

//============================================================================//

void DCE_RenderCheapShadow(uint16 texID, vec3f_t * v, float size)
{
	//DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);

	//DCE_RenderSetBlendDst(PVR_BLEND_INVSRCALPHA);

	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_TR_POLY, texID, PVR_TXRENV_MODULATEALPHA);

	float uv[] = { 0, 0, 1, 0, 0, 1, 1, 1 };

	unsigned int argb[] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };

#define YEPSILON 0.50f

    float vert[] = { v->x - size, v->y + YEPSILON, v->z - size,
		             v->x + size, v->y + YEPSILON, v->z - size,
					 v->x - size, v->y + YEPSILON, v->z + size,
					 v->x + size, v->y + YEPSILON, v->z + size };

    DCE_RenderTransformAndClipTriangleStrip(vert, 3 * sizeof(float),
                                            uv, 2 * sizeof(float),
                                            argb, sizeof(unsigned int), 4);
}

void DCE_RenderTesselateDecal(vec3f_t * point, float size, vec3f_t * normal, unsigned short texID,
							  float s, float t, unsigned int color)
{
	vec3f_t up = { 0, -1, 0 };             // Default Axis Aligned Up Vector
	vec3f_t right = { 0, 0, -1 };          // Default Axis Aligned Right Vector
	vec3f_t v[4];                          // Vertices of Axis Aligned Decal Quad

	if(vec3f_equal(normal, &up))           // Up Axis Equals Normal, Compute New Up Vector
		vec3f_cross(&right, normal, &up);  // Right Vector = Normal Cross up
	else if(vec3f_equal_inv(normal, &up))  // Up Axis Equals Invererse Normal, Compute New Up Vector And Right Vector
	{
		right.z = 1;
		vec3f_cross(&right, normal, &up);  // Up Vector = Normal cross Right
	}
	else
		vec3f_cross(&up, normal, &right);  // Right Vector = Normal cross Up

	vec3f_copy(&right, &v[0]);
	vec3f_invert(&v[0]);
	vec3f_sub(&up, &v[0], &v[0]);
	vec3f_mul_scalar(&v[0], size, &v[0]);
	vec3f_add(&v[0], point, &v[0]);

	vec3f_copy(&right, &v[1]);
	vec3f_sub(&up, &v[1], &v[1]);
	vec3f_mul_scalar(&v[1], size, &v[1]);
	vec3f_add(&v[1], point, &v[1]);

	vec3f_copy(&right, &v[2]);
	vec3f_invert(&v[2]);
	vec3f_add(&up, &v[2], &v[2]);
	vec3f_mul_scalar(&v[2], size, &v[2]);
	vec3f_add(&v[2], point, &v[2]);

	vec3f_copy(&right, &v[3]);
	vec3f_add(&up, &v[3], &v[3]);
	vec3f_mul_scalar(&v[3], size, &v[3]);
	vec3f_add(&v[3], point, &v[3]);

	float uv[] = { 0, 0, s, 0, 0, t, s, t };

	unsigned int argb[] = { color, color, color, color };

	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_TR_POLY, texID, PVR_TXRENV_MODULATEALPHA);

    DCE_RenderTransformAndClipTriangleStrip((float *)v, sizeof(vec3f_t),
                                            uv, 2 * sizeof(float),
                                            argb, sizeof(unsigned int), 4);
}

void DCE_RenderTesselateDecal2(vec3f_t * I, vec3f_t * E1, vec3f_t * N,
                               float size,  unsigned short texID,
							   float s, float t, unsigned int color)
{
	vec3f_t U;  
	vec3f_t V; 
	vec3f_t v[4];                          // Vertices of Decal Quad
    
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

	float uv[] = { 0, 0, s, 0, 0, t, s, t };

	unsigned int argb[] = { color, color, color, color };

	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_TR_POLY, texID, PVR_TXRENV_MODULATEALPHA);

    DCE_RenderTransformAndClipTriangleStrip((float *)v, sizeof(vec3f_t),
                                            uv, 2 * sizeof(float),
                                            argb, sizeof(unsigned int), 4);
}

void DCE_TesselateDecalQuad(vec3f_t * point, float size, vec3f_t * normal, vec3f_t * v)
{
	vec3f_t up = { 0, -1, 0 };             // Default Axis Aligned Up Vector
	vec3f_t right = { 0, 0, -1 };          // Default Axis Aligned Right Vector

	if(vec3f_equal(normal, &up))           // Up Axis Equals Normal, Compute New Up Vector
		vec3f_cross(&right, normal, &up);  // Right Vector = Normal Cross up
	else if(vec3f_equal_inv(normal, &up))  // Up Axis Equals Invererse Normal, Compute New Up Vector And Right Vector
	{
		right.z = 1;
		vec3f_cross(&right, normal, &up);  // Up Vector = Normal cross Right
	}
	else
		vec3f_cross(&up, normal, &right);  // Right Vector = Normal cross Up

	vec3f_copy(&right, &v[0]);
	vec3f_invert(&v[0]);
	vec3f_sub(&up, &v[0], &v[0]);
	vec3f_mul_scalar(&v[0], size, &v[0]);
	vec3f_add(&v[0], point, &v[0]);

	vec3f_copy(&right, &v[1]);
	vec3f_sub(&up, &v[1], &v[1]);
	vec3f_mul_scalar(&v[1], size, &v[1]);
	vec3f_add(&v[1], point, &v[1]);

	vec3f_copy(&right, &v[2]);
	vec3f_invert(&v[2]);
	vec3f_add(&up, &v[2], &v[2]);
	vec3f_mul_scalar(&v[2], size, &v[2]);
	vec3f_add(&v[2], point, &v[2]);

	vec3f_copy(&right, &v[3]);
	vec3f_add(&up, &v[3], &v[3]);
	vec3f_mul_scalar(&v[3], size, &v[3]);
	vec3f_add(&v[3], point, &v[3]);
}

void DCE_RenderCheapFlashLight(DCE_Player * player, unsigned short texID,
                               float x1, float x2, float y1, float y2)
{
    vec3f_t cam_dir, cam_from;
	float d, c;

	vec3f_copy(&player->lookAt, &cam_dir);
	vec3f_copy(&player->position, &cam_from);

	vec3f_shift(&cam_dir, &cam_from, 1200.0f);

	if(!Q3BSP_CheckCollision(&player->position, &cam_dir, &cam_dir))
	    return;

	vec3f_distance(player->position.x, player->position.y, player->position.z,
		           cam_dir.x, cam_dir.y, cam_dir.z, d);

	d /= 6;

	if(d > 320)
		d = 320;

    c = 1.0f - ((((255.0f / 320.0f * d) / 255.0f) * .88) + 0.25f);
     
    c -= 0.05f;
     
    if(c>1.0) c = 1.0;
    if(c<0) c = 0;
     
    float h = y2 - y1;
    float x = (x2 - x1) / 2;
    float y = y1 + (h / 2);

	DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);
 
    DCE_RenderSetBlendDst(PVR_BLEND_DESTALPHA);

	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_TR_POLY, texID, PVR_TXRENV_MODULATEALPHA);
     
    #define MIN_RAD 80
    DCE_RenderTexturedQuad2D(x - d - MIN_RAD, y - d - MIN_RAD,
                             x + d + MIN_RAD, y + d + MIN_RAD,
                             0, 0, 1, 1, PVR_PACK_COLOR( c * .75, c, c, c ));
}

void DCE_RenderBoundingBox(vec3f_t * min, vec3f_t * max, unsigned short int texID, unsigned int color)
{
    vec3f_t v[4];
	 
    float uv[] = { 0, 0, 1, 0, 0, 1, 1, 1 };

	unsigned int argb[] = { color, color, color, color };
    
    vec3f_copy(min, &v[0]);
    vec3f_copy(min, &v[1]); v[1].x = max->x;
    vec3f_copy(min, &v[2]); v[2].y = max->y;
    vec3f_copy(min, &v[3]); v[3].x = max->x; v[3].y = max->y;
    
	DCE_RenderSetBlendSrc(PVR_BLEND_SRCALPHA);
 
    DCE_RenderSetBlendDst(PVR_BLEND_DESTALPHA);

	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_TR_POLY, texID, PVR_TXRENV_MODULATEALPHA);    

    DCE_RenderTransformAndClipTriangleStrip((float *)v, sizeof(vec3f_t),
                                            uv, 2 * sizeof(float),
                                            argb, sizeof(unsigned int), 4);	 
                                            
    vec3f_copy(min, &v[0]); v[0].z = max->z;
    vec3f_copy(min, &v[1]); v[1].x = max->x; v[1].z = max->z;
    vec3f_copy(min, &v[2]); v[2].y = max->y; v[2].z = max->z;
    vec3f_copy(min, &v[3]); v[3].x = max->x; v[3].y = max->y; v[3].z = max->z;         

    DCE_RenderTransformAndClipTriangleStrip((float *)v, sizeof(vec3f_t),
                                            uv, 2 * sizeof(float),
                                            argb, sizeof(unsigned int), 4);	   
                                            
    vec3f_copy(min, &v[0]); 
    vec3f_copy(min, &v[1]); v[1].z = max->z;
    vec3f_copy(min, &v[2]); v[2].y = max->y; 
    vec3f_copy(min, &v[3]); v[3].y = max->y; v[3].z = max->z;       

    DCE_RenderTransformAndClipTriangleStrip((float *)v, sizeof(vec3f_t),
                                            uv, 2 * sizeof(float),
                                            argb, sizeof(unsigned int), 4);	   
    vec3f_copy(min, &v[0]); v[0].x = max->x;
    vec3f_copy(min, &v[1]); v[1].x = max->x; v[1].z = max->z;
    vec3f_copy(min, &v[2]); v[2].x = max->x; v[2].y = max->y; 
    vec3f_copy(max, &v[3]);

    DCE_RenderTransformAndClipTriangleStrip((float *)v, sizeof(vec3f_t),
                                            uv, 2 * sizeof(float),
                                            argb, sizeof(unsigned int), 4);	                                                                                                                                   
}

void DCE_RenderLoadingScreen(char *texname)
{
	unsigned short texID = DCE_TextureLoadJPG(texname);
	
	DCE_RenderStart();
	
	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, texID, PVR_TXRENV_REPLACE);
	DCE_RenderTexturedQuad2D(0, 64, 640, 480 - 64,
							  0, 0, 1, 1, 0xFFFFFFFF); 
							  
    DCE_RenderFinish();

	DCE_RenderStart();
	
	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, texID, PVR_TXRENV_REPLACE);
	DCE_RenderTexturedQuad2D(0, 64, 640, 480 - 64,
							  0, 0, 1, 1, 0xFFFFFFFF); 
							  
    DCE_RenderFinish();

    DCE_TextureUnBind(texID);
}

void DCE_RenderLoadingScreenPVR(char *texname)
{
	unsigned short texID = DCE_TextureLoadPVR(texname);
	
	DCE_RenderStart();
	
	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, texID, PVR_TXRENV_REPLACE);
	DCE_RenderTexturedQuad2D(0, 64, 640, 480 - 64,
							  0, 0, 1, 1, 0xFFFFFFFF); 
							  
    DCE_RenderFinish();

	DCE_RenderStart();
	
	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, texID, PVR_TXRENV_REPLACE);
	DCE_RenderTexturedQuad2D(0, 64, 640, 480 - 64,
							  0, 0, 1, 1, 0xFFFFFFFF); 
							  
    DCE_RenderFinish();

    DCE_TextureUnBind(texID);
}

//////////////////////////////////////////////////////////////////////////////////

void DCE_RenderTransformTris(float *vert_pos, int vert_stride,
                             float *uv_coord, int uv_stride,
				             unsigned int *argb, int argb_stride,
							 unsigned int count)
{
    unsigned int i;  
/*
	int vs = vert_stride / 4;
	int ts = uv_stride / 4;
	int cs = argb_stride / 4;
*/   
//    pvr_dr_state_t dr_state;
    
//    pvr_dr_init(dr_state);
    
    pvr_cmd_clip_vertex_t * v = &DCE_PVR_CLIP_VERTEX[0];//pvr_dr_target(dr_state);
    
    for(i = 0; i < count; i+= 3)
    {
        //v = pvr_dr_target(dr_state);      
        
        mat_trans_single3_nomod(vert_pos[(i * vert_stride) + 0], vert_pos[(i * vert_stride) + 1],
                                vert_pos[(i * vert_stride) + 2], v->x, v->y, v->z);
          
        v->u = uv_coord[(i * uv_stride)];  
        v->v = uv_coord[(i * uv_stride) + 1];  
        
        v->flags = PVR_CMD_VERTEX;
        v->argb = 0xFFFFFFFF;
                  
        //pvr_dr_commit(v);  
        DCE_RenderSubmitHeader((uint32 *)v);
        
        //v = pvr_dr_target(dr_state);
               
        mat_trans_single3_nomod(vert_pos[((i + 1) * vert_stride) + 0], vert_pos[((i + 1) * vert_stride) + 1],
                                vert_pos[((i + 1) * vert_stride) + 2], v->x, v->y, v->z);
          
        v->u = uv_coord[((i + 1) * uv_stride)];  
        v->v = uv_coord[((i + 1) * uv_stride) + 1];  
        
        //v->flags = PVR_CMD_VERTEX;
        //v->argb = 0xFFFFFFFF;
                  
        //pvr_dr_commit(v);      
        DCE_RenderSubmitHeader((uint32 *)v);
        
        //v = pvr_dr_target(dr_state);
               
        mat_trans_single3_nomod(vert_pos[((i + 2) * vert_stride) + 0], vert_pos[((i + 2) * vert_stride) + 1],
                                vert_pos[((i + 2) * vert_stride) + 2], v->x, v->y, v->z);
          
        v->u = uv_coord[((i + 2) * uv_stride)];  
        v->v = uv_coord[((i + 2) * uv_stride) + 1];  
        
        v->flags = PVR_CMD_VERTEX_EOL;
        //v->argb = 0xFFFFFFFF;
                  
        //pvr_dr_commit(v);
        DCE_RenderSubmitHeader((uint32 *)v);                 
    }
}

#include "dce-collision.h"
void DCE_RenderPlayerModelShadowCull(vec3f_t * from, vec3f_t * to, DCE_Player * player)
{
}

void DCE_RenderPlayerModelCull(vec3f_t * from, vec3f_t * to, DCE_Player * player)
{   
}

void DCE_RenderPlayerModel(DCE_Player * player)
{
}

void DCE_RenderPlayerModelShadow(DCE_Player * player)
{
}

static pvr_vertex_t DCE_CHEAP_SHADOW[256];
static unsigned short int DCE_CHEAP_SHADOW_VERTICES = 0;

void DCE_TesselateCheapShadow(float r)
{
     vec3f_t p = { 10, 0, 0 };
     vec3f_t c = { 0, 0, 0 };
     
     DCE_CHEAP_SHADOW_VERTICES = (3 * (360.0f / r));

     int i;
     
     for(i = 0; i < DCE_CHEAP_SHADOW_VERTICES; i += 3)
     {
         DCE_CHEAP_SHADOW[i + 0].flags = PVR_CMD_VERTEX;
         DCE_CHEAP_SHADOW[i + 0].argb = 0x80000000;
         DCE_CHEAP_SHADOW[i + 0].x = p.x;
         DCE_CHEAP_SHADOW[i + 0].y = 0.0f;
         DCE_CHEAP_SHADOW[i + 0].z = p.z;
         
         DCE_CHEAP_SHADOW[i + 1].flags = PVR_CMD_VERTEX;
         DCE_CHEAP_SHADOW[i + 1].argb = 0x80000000;
         DCE_CHEAP_SHADOW[i + 1].x = c.x;
         DCE_CHEAP_SHADOW[i + 1].y = 0.0f;
         DCE_CHEAP_SHADOW[i + 1].z = c.z;         
         
         vec3f_rotated_xz_c(&p, &c, r);
         
         DCE_CHEAP_SHADOW[i + 2].flags = PVR_CMD_VERTEX_EOL;
         DCE_CHEAP_SHADOW[i + 2].argb = 0x80000000;
         DCE_CHEAP_SHADOW[i + 2].x = p.x;
         DCE_CHEAP_SHADOW[i + 2].y = 0.0f;
         DCE_CHEAP_SHADOW[i + 2].z = p.z;         
     }  
}

void DCE_RenderCheapShadowCull(DCE_Player * player, vec3f_t * p, float h, float s)
{
    glPushMatrix();
              
    glTranslatef(p->x, p->y - h, p->z);

	glScalef(s, s, s);
	
	DCE_MatrixApplyRender();
    
    DCE_RenderCompileAndSubmitHeader(PVR_LIST_TR_POLY);

	DCE_RenderTransformAndClipTriangles(&DCE_CHEAP_SHADOW[0].x, sizeof(pvr_vertex_t),
                                        &DCE_CHEAP_SHADOW[0].u, sizeof(pvr_vertex_t),
                        (unsigned int *)&DCE_CHEAP_SHADOW[0].argb, sizeof(pvr_vertex_t),
                                        DCE_CHEAP_SHADOW_VERTICES);
                                 
    glPopMatrix();  
    
    DCE_MatrixApplyRender();
}

/** \brief  Calculate the offset color value for a set of bumpmap parameters.

    This function calculates the value to be placed into the oargb value for the
    use of bumpmapping on a polygon. The angles specified should be expressed in
    radians and within the limits specified for the individual parameter.

    \param  h               Weighting value in the range [0, 1] for how defined
                            the bumpiness of the surface should be.
    \param  t               Spherical elevation angle in the range [0, pi/2]
                            between the surface and the lighting source. A value
                            of pi/2 implies that the light is directly overhead.
    \param  q               Spherical rotation angle in the range [0, 2*pi]
                            between the surface and the lighting source.
    \return                 32-bit packed value to be used as an offset color on
                            the surface to be bump mapped.

    \note   For more information about how bumpmapping on the PVR works, refer
            to <a href="https://google.com/patents/US6819319">US Patent
            6,819,319</a>, which describes the algorithm implemented in the
            hardware (specifically look at Figures 2 and 3, along with the
            description in the Detailed Description section).
    \note   Thanks to Fredrik Ehnbom for figuring this stuff out and posting it
            to the mailing list back in 2005!
*/

static unsigned int DCE_PVR_BUMP_PARAM = 0;
static vec3f_t      DCE_PVR_BUMP_LIGHT_SOURCE = { 0, 5000, 0 };
static float        DCE_PVR_BUMP_QSCALE = 255.0f / 360.0f;

unsigned int DCE_RenderComputeBumpParam(vec3f_t * p)
{	
	if(!DCE_PVR_BUMP_PARAM)
	{
		float s, c;
		
		fsincos(F_PI / 2, &s, &c);
		
		uint8 hp = 255;
        uint8 k1 = ~hp;
        uint8 k2 = (uint8)(hp * s);
        uint8 k3 = (uint8)(hp * c);
        
        DCE_PVR_BUMP_PARAM = (k1 << 24) | (k2 << 16) | (k3 << 8) | 0;
	}

    uint8 qp = (uint8)(vec3f_vectors_to_angle_xz(&DCE_PVR_BUMP_LIGHT_SOURCE, p) * DCE_PVR_BUMP_QSCALE);

    DCE_PVR_BUMP_PARAM |= qp;

    return DCE_PVR_BUMP_PARAM;
}

void DCE_RenderCompileAndSubmitHeaderBumpMapped(unsigned short index)
{
    DCE_Texture * tex = DCE_TextureObject(index);
	
	pvr_poly_hdr_t hdr;

    pvr_poly_cxt_txr(&DCE_PVR_POLY_CXT,
                     DCE_PVR_LIST,
                     tex->color,
                     tex->w,
                     tex->h,
                     tex->data,
                     tex->filter);

    if(DCE_PVR_ENABLE_SHADOWS)
    {
        DCE_PVR_POLY_CXT.gen.modifier_mode = PVR_MODIFIER_CHEAP_SHADOW;
        DCE_PVR_POLY_CXT.fmt.modifier = PVR_MODIFIER_ENABLE;
    }

    DCE_PVR_POLY_CXT.gen.shading = DCE_PVR_SHADE_FUNC;

	if(DCE_PVR_LIST)
	{
        DCE_PVR_POLY_CXT.blend.src = DCE_PVR_BLEND_FUNC_SRC;
        DCE_PVR_POLY_CXT.blend.dst = DCE_PVR_BLEND_FUNC_DST;
	}

	DCE_PVR_POLY_CXT.txr.filter = PVR_FILTER_NONE;

    DCE_RenderApplyDepthFunc();

    DCE_RenderApplyCullingFunc();

	if(DCE_PVR_SCISSOR_FUNC)
		DCE_PVR_POLY_CXT.gen.clip_mode = PVR_USERCLIP_INSIDE;
    
    DCE_RenderApplyFogFunc();
    
    DCE_PVR_POLY_CXT.gen.specular = PVR_SPECULAR_ENABLE;
    DCE_PVR_POLY_CXT.txr.env = PVR_TXRENV_DECAL;
    
	pvr_poly_compile(&hdr, &DCE_PVR_POLY_CXT);

    DCE_RenderSubmitHeader((uint32 *)&hdr);
}

void DCE_RenderIndexedBuffer(vec3f_t * buf, unsigned int *index, unsigned int count)
{
	pvr_vertex_t * dst = DCE_CLIP_BUFFER;
	unsigned int i;
	
	for(i = 0; i < count; i ++)
	{
        dst[i].x = buf[index[i]].x;
        dst[i].y = buf[index[i]].y;
        dst[i].z = buf[index[i]].z;
        
        dst[i].argb = 0xFFFF0000;

/*        
		dst->x = buf[index[i] + 1].x;
        dst->y = buf[index[i] + 1].y;
        dst->z = buf[index[i] + 1].z;
        
        dst->argb = 0xFFA0A0A0;
        
		++dst;
		
		dst->x = buf[index[i] + 2].x;
        dst->y = buf[index[i] + 2].y;
        dst->z = buf[index[i] + 2].z;
        
        dst->argb = 0xFFA0A0A0;
        
		++dst;	
*/	
    }
    
	DCE_RenderCompileAndSubmitHeader(PVR_LIST_OP_POLY);    
    
    DCE_RenderTransformTriangles(&DCE_CLIP_BUFFER[0].x, sizeof(pvr_vertex_t),
                                 &DCE_CLIP_BUFFER[0].u, sizeof(pvr_vertex_t),
                                 &DCE_CLIP_BUFFER[0].argb, sizeof(pvr_vertex_t), count);	     
    
/*	
	for(i = 0; i < count / 3; i += 3)
	{
        dst->x = buf[index[i]].x;
        dst->y = buf[index[i]].y;
        dst->z = buf[index[i]].z;
        
        mat_trans_single(dst->x, dst->y, dst->z);
        
        dst->argb = 0xFFFFFFFF;
        
        dst->flags = PVR_CMD_VERTEX;

        ++dst;

        dst->x = buf[index[i + 1]].x;
        dst->y = buf[index[i + 1]].y;
        dst->z = buf[index[i + 1]].z;
        
        mat_trans_single(dst->x, dst->y, dst->z);
        
        dst->argb = 0xFFFFFFFF;
        
        dst->flags = PVR_CMD_VERTEX;
        
        dst->x = buf[index[i + 2]].x;
        dst->y = buf[index[i + 2]].y;
        dst->z = buf[index[i + 2]].z;
        
        mat_trans_single(dst->x, dst->y, dst->z);
        
        dst->argb = 0xFFFFFFFF;
        
        dst->flags = PVR_CMD_VERTEX_EOL;
        
        ++dst;
	}
	
    DCE_RenderSubmitPVR(DCE_CLIP_BUFFER, count);
*/
}

void DCE_RenderTesselatePointSprite(vec3f_t * p, vec3f_t * d, unsigned short texID, unsigned int argb)
{   
    vec3f_t D;
    
    vec3f_mul_scalar(d, 30, &D);
    
    vec3f_add(&D, p, &DCE_SPRITE_VERT[0].x);
    
    vec3f_sub(&D, p, &DCE_SPRITE_VERT[1].x);
    
    vec3f_copy(&DCE_SPRITE_VERT[0].x, &DCE_SPRITE_VERT[2].x);
    
    vec3f_copy(&DCE_SPRITE_VERT[1].x, &DCE_SPRITE_VERT[3].x);   
	
	DCE_SPRITE_VERT[0].y = p->y + 5;
	DCE_SPRITE_VERT[1].y = p->y + 5; 
	DCE_SPRITE_VERT[2].y = p->y + 70;
	DCE_SPRITE_VERT[3].y = p->y + 70;	   
    
    DCE_SPRITE_VERT[0].argb = DCE_SPRITE_VERT[1].argb = DCE_SPRITE_VERT[2].argb = DCE_SPRITE_VERT[3].argb = argb;
    
    DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_TR_POLY, texID, PVR_TXRENV_REPLACE);
    
    mat_transform_sq(&DCE_SPRITE_VERT, TA_SQ_ADDR, 4);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

      
void DCE_RenderInitClipBuffer()
{
	unsigned int i;
	for(i = 0; i < DCE_VERTEX_BUFFER_MAX; i++)
	    DCE_CLIP_BUFFER[i].argb = 0xFFFFFFFF;
}

//======================================================================================//

static matrix4f DCE_MAT_PLAYER_HANDS ALIGN32;

void DCE_PlayerHandsModelComputeMatrix(float x, float y, float z, float rx, float ry, float rz, float scale)
{
    DCE_MatrixLoadIdentity();

    mat_translate(x, y, z);

    mat_rotate(DEG2RAD * rx, 0,0);

    mat_rotate(0, DEG2RAD * ry, 0);

    mat_rotate(0, 0, DEG2RAD * rz);
    
	mat_scale(scale, scale, scale);
    
    mat_store(&DCE_MAT_PLAYER_HANDS);
}

void DCE_RenderModel(vec3f_t *p, float scale, vec3f_t * mtrans, vec3f_t * mscale,
                               unsigned short texID, int tris, float r, pvr_vertex_t * vert)
{
    DCE_MatrixLoadProjection();
        
    DCE_MatrixApplyPlayer(0);
              
    mat_translate(p->x, p->y, p->z);

	mat_rotate(DEG2RAD * 90.0f, 0.0f, 0.0f);
	
	mat_rotate(0.0f, 0.0f, DEG2RAD * r);
	
	mat_scale(scale, scale, scale);

    mat_translate(mtrans->x, mtrans->y, mtrans->z);

	mat_scale(mscale->x, mscale->y, mscale->z);

    DCE_RenderEnableCull(1);
    
    DCE_RenderSetCullFace(DCE_PVR_CULL_BACK);    
    
    DCE_RenderSetCullFunc(DCE_PVR_CULL_CW);   
    
 	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, texID, NULL);
    
    DCE_RenderTransformAndCullTriangles(&vert->x, sizeof(pvr_vertex_t),
                                 &vert->u, sizeof(pvr_vertex_t),
                                 0xFFFFFFFF, tris * 3);
    
	DCE_RenderEnableCull(0);    
}

void DCE_RenderModelPlayerShadow(vec3f_t *p, float scale, vec3f_t * mtrans, vec3f_t * mscale,
                               unsigned short texID, int tris, float r, pvr_vertex_t * vert)
{
    unsigned int c = 0xFF202020;
    
	DCE_MatrixLoadProjection();
        
    DCE_MatrixApplyPlayer(0);
            
  if(DCE_RenderShadowMapQuality() == 1)
  {
    mat_translate(p->x, p->y + 0.4f, p->z);

	mat_rotate(DEG2RAD * 90.0f, 0.0f, 0.0f);
	
	mat_rotate(0.0f, 0.0f, DEG2RAD * r);
	
	mat_scale(scale + .03f, scale + .03f, 0.0f);

    mat_translate(mtrans->x, mtrans->y, mtrans->z);

	mat_scale(mscale->x, mscale->y, 0.0f);

    DCE_RenderEnableCull(1);
    
    DCE_RenderSetCullFace(DCE_PVR_CULL_BACK);    
    
    DCE_RenderSetCullFunc(DCE_PVR_CULL_CW);   
   
 	DCE_RenderCompileAndSubmitHeader(PVR_LIST_TR_POLY);
    
    DCE_RenderTransformAndCullTriangles(&vert->x, sizeof(pvr_vertex_t),
                                        &vert->u, sizeof(pvr_vertex_t),
                                        c, tris * 3);
    
	DCE_RenderEnableCull(0); 
 }
 else if(DCE_RenderShadowMapQuality() == 2)
 { 
    mat_translate(p->x, p->y + 0.4f, p->z);
    
    mat_scale(2.0f, 1.0f, 2.0f);
    
    DCE_RenderCompileAndSubmitHeader(PVR_LIST_TR_POLY);

	DCE_RenderTransformAndCullTriangles(&DCE_CHEAP_SHADOW[0].x, sizeof(pvr_vertex_t),
                                        &DCE_CHEAP_SHADOW[0].u, sizeof(pvr_vertex_t),
                                        0x40101010, DCE_CHEAP_SHADOW_VERTICES);
 }
 
}

void DCE_RenderPlayerModelTPP(DCE_Player * player)
{
	DCE_PlayerModel * mdl = &player->model;
	
	unsigned char frame = mdl->body_frames[mdl->body_index][mdl->body_frame];	
	
	struct md2_frame_t * pframe = &mdl->body->frames[frame];

    float r = vec3f_vectors_to_angle_xz(&player->position, &player->lookAt) - 90.0f;

    DCE_MatrixLoadProjection();
        
    DCE_MatrixApplyPlayer(0);
              
    mat_translate(player->position.x, player->position.y - player->height, player->position.z);

    mat_scale(25.417f, 25.417f, 25.417f);

	mat_rotate_x(DEG2RAD * 90.0f);
	
	mat_rotate_z(DEG2RAD * r);
	
	//mat_scale(scale, scale, scale);
    
    mat_translate(pframe->translate[0], pframe->translate[1], pframe->translate[2]);

	mat_scale(pframe->scale[0], pframe->scale[1], pframe->scale[2]);

    DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, mdl->body_texID, NULL);

    DCE_RenderCacheIndexHQMD2(mdl->body, frame);

/* // Warning! Player model weapon not alligned / sycnchronized */
    if(mdl->weapon_lod1 != NULL)
    {
	    DCE_RenderMD2ComputeMatrixS(player, &mdl->weapon_lod1->frames[frame], r);
	    
	    DCE_RenderCompileAndSubmitTextureHeader(mdl->weapon_texID);

        DCE_RenderCacheIndexHQMD2(mdl->weapon_lod1, frame);
    }
/*
    DCE_RenderModelPlayerShadow(&pos, 1.0f, (vec3f_t *)pframe->translate, (vec3f_t *)pframe->scale,
	                mdl->weapon_texID, mdl->weapon->header.num_tris, r, DCE_CLIP_BUFFER);
*/
}

void DCE_RenderPlayerModelShadowTPP(DCE_Player * player)
{
	DCE_PlayerModel * mdl = &player->model;
	
	unsigned char frame = mdl->body_frames[mdl->body_index][mdl->body_frame];	
	
	struct md2_frame_t * pframe = &mdl->body->frames[frame];

    float r = vec3f_vectors_to_angle_xz(&player->position, &player->lookAt) - 90.0f;
    
    vec3f_t pos = { player->position.x, player->position.y - player->height, player->position.z };

    DCE_RenderModelPlayerShadow(&pos, 1.0f, (vec3f_t *)pframe->translate, (vec3f_t *)pframe->scale,
	                mdl->body_texID, mdl->body->header.num_tris, r, DCE_CLIP_BUFFER);

}

void DCE_RenderPlayerModelFPPSetMatrix(md2_mdl * md2, unsigned char frame)
{
	struct md2_frame_t *pframe = &md2->frames[frame];
	
	vec3f_t * t = pframe->translate;
	vec3f_t * s = pframe->scale;

    DCE_MatrixLoadProjection();

    mat_apply(&DCE_MAT_PLAYER_HANDS);

    mat_translate(t->x, t->y, t->z);

	mat_scale(s->x, s->y, s->z);
}

extern void DCE_RenderCacheIndexHQMD2_V2(md2_mdl * mdl, unsigned char frame);

void DCE_RenderPlayerModelFPP(md2_mdl * md2, unsigned char frame, unsigned short texID)
{
	struct md2_frame_t *pframe = &md2->frames[frame];
	
	vec3f_t * t = pframe->translate;
	vec3f_t * s = pframe->scale;

    DCE_MatrixLoadProjection();

    mat_apply(&DCE_MAT_PLAYER_HANDS);

    mat_translate(t->x, t->y, t->z);

	mat_scale(s->x, s->y, s->z);

	DCE_RenderEnableCull(0);
	
	DCE_RenderSetCullFace(DCE_PVR_CULL_BACK);
	
	DCE_RenderSetCullFunc(DCE_PVR_CULL_CW);

	DCE_RenderCompileAndSubmitTextureHeader(texID);

    DCE_RenderCacheIndexHQMD2_V2(md2, frame);
					                
   	DCE_RenderEnableCull(0);
}

void DCE_RenderPlayerModelFPP2(md2_mdl * md2, unsigned char frame, unsigned short texID)
{
	struct md2_frame_t *pframe = &md2->frames[frame];
	
	vec3f_t * t = pframe->translate;
	vec3f_t * s = pframe->scale;

    DCE_MatrixLoadProjection();

    mat_apply(&DCE_MAT_PLAYER_HANDS);

    mat_translate(5, -3, 5);

    mat_translate(t->x, t->y, t->z);

	mat_scale(s->x, s->y, s->z);

	DCE_RenderEnableCull(1);
	
	DCE_RenderSetCullFace(DCE_PVR_CULL_BACK);
	
	DCE_RenderSetCullFunc(DCE_PVR_CULL_CW);

	DCE_RenderCompileAndSubmitTextureHeader(texID);

    DCE_RenderCacheIndexHQMD2(md2, frame);
					                
   	DCE_RenderEnableCull(0);
}


void DCE_RenderModelPlayer(DCE_Player * player)
{
	if(player->perspective == DCE_PERSPECTIVE_FIRST)
	{
        unsigned char frame = player->model.hands_frames[player->model.hands_index][player->model.hands_frame];
        
        DCE_RenderPlayerModelFPP(player->model.hands, frame, player->model.hands_texID);
        
        DCE_RenderPlayerModelFPP(player->model.weapon, frame, player->model.weapon_texID);
        
        //if(player->model.weapon_attachment != NULL)
        //    DCE_RenderPlayerModelFPP2(player->model.weapon_attachment, frame, player->model.weapon_attachment_texID);
    }
    else
    	DCE_RenderPlayerModelTPP(player);
}

static unsigned short int texID = 0;

void DCE_RenderModelPlayerTR(DCE_Player * player)
{
	if(player->perspective == DCE_PERSPECTIVE_FIRST)
	{
        unsigned char frame = player->model.hands_frames[player->model.hands_index][player->model.hands_frame];
               
        if(frame == 0)
            texID = DCE_TexID(DCE_TEXID_MUZFLASH1 + rand() % 4);
		               
        if(player->model.muzzleflash != NULL)
            if(frame >= 10 && frame <= 10 + 5)
                DCE_RenderPlayerModelFPP2(player->model.muzzleflash, frame - 10, texID);
    }
}

void DCE_RenderMD2ComputeMatrix(DCE_Player * player, struct hqmd2_frame_t *pframe, float r)
{
	DCE_RenderEnableCull(0);
	
    DCE_MatrixLoadProjection();
        
    DCE_MatrixApplyPlayer(0);
              
    mat_translate(player->position.x, player->position.y - player->height, player->position.z);

	mat_rotate_x(DEG2RAD * 90.0f);
	
	mat_rotate_z(DEG2RAD * r);
	
	mat_translate(pframe->translate[0], pframe->translate[1], pframe->translate[2]);

	mat_scale(pframe->scale[0], pframe->scale[1], pframe->scale[2]);
}

void DCE_RenderMD2ComputeMatrixS(DCE_Player * player, struct hqmd2_frame_t *pframe, float r)
{
	DCE_RenderEnableCull(0);
	
    DCE_MatrixLoadProjection();
        
    DCE_MatrixApplyPlayer(0);
                            
    mat_translate(player->position.x, player->position.y - player->height, player->position.z);

    mat_scale(25.417f, 25.417f, 25.417f);

	mat_rotate_x(DEG2RAD * 90.0f);
	
	mat_rotate_z(DEG2RAD * r);
	
	mat_translate(pframe->translate[0], pframe->translate[1], pframe->translate[2]);

	mat_scale(pframe->scale[0], pframe->scale[1], pframe->scale[2]);
}

void DCE_RenderEnemyLOD(DCE_Player * player, DCE_Player * player2)
{   
	DCE_PlayerModel * mdl = &player2->model;
    float fov, d, r = 0;    
	vec3f_t V;
    vec3f_t pos = { player2->position.x, player2->position.y, player2->position.z };   

	unsigned char frame = mdl->body_frames[mdl->body_index][mdl->body_frame];
    
    vec3f_sub_normalize(pos.x, pos.y, pos.z,
	                    player->position.x, player->position.y, player->position.z,
	                    V.x, V.y, V.z);
 /*    */ 
    vec3f_dot(player->direction.x, player->direction.y, player->direction.z, V.x, V.y, V.z, fov);

    vec3f_distance(player->position.x, player->position.y, player->position.z,
	               pos.x, pos.y, pos.z, d);

    if((fov < 0.25f) && (player->perspective == DCE_PERSPECTIVE_FIRST) && (d > 120))
        return;
	   
    pos.y -= player2->height;
    
    r = vec3f_vectors_to_angle_xz(&player2->position, &player2->lookAt) - 90.0f;

	if(player2->state & PSTATE_DEAD)
	{
		if(player2->state_frame == 0)
		    DCE_SfxPlayAttenuation(ENEMY_DEATH1 + (rand() % 3), &player2->position, &player->position);
		    
	    if(player2->model.body_frame == player2->model.body_frame_count[player2->model.body_index] - 8)
	       	if(!(player2->state_frame % 2))
			   DCE_SfxPlayAttenuation(IMPACT_FLESH2, &player2->position, &player->position);
			
		if(player2->model.body_frame >= player2->model.body_frame_count[player2->model.body_index] - 6)
		{
			unsigned char frame;
			frame = (player2->state_frame - (player2->model.body_frame_count[player2->model.body_index] - 6)) / 5;
        	if(frame > 24)
        	    frame = 24;
	        	        
	    	DCE_RenderMD2ComputeMatrix(player2, &mdl->blood->frames[frame], r);
	    
	    	DCE_RenderCompileAndSubmitTextureHeader(mdl->blood_texID);

            mat_translate(0.0f, 1.0f, 0.0f);

            DCE_RenderCacheIndexHQMD2(mdl->blood, frame);	                              
		}
	}
	    
	if(d < 100 || mdl->body_lod1 == NULL)
	{
		DCE_RenderMD2ComputeMatrixS(player2, &mdl->body->frames[frame], r);
	    
	    DCE_RenderCompileAndSubmitTextureHeader(mdl->body_texID);

        DCE_RenderCacheIndexHQMD2(mdl->body, frame);
	
		if(mdl->weapon_lod1 != NULL)
		{
		    DCE_RenderMD2ComputeMatrixS(player2, &mdl->weapon_lod1->frames[frame], r);
	    
	        DCE_RenderCompileAndSubmitTextureHeader(mdl->weapon_texID);

            DCE_RenderCacheIndexHQMD2(mdl->weapon_lod1, frame);
		}
	}

	else if(d < 200 || mdl->body_lod2 == NULL)
	{
		DCE_RenderMD2ComputeMatrixS(player2, &mdl->body_lod1->frames[frame], r);
	    
	    DCE_RenderCompileAndSubmitTextureHeader(mdl->body_texID);

        DCE_RenderCacheIndexHQMD2(mdl->body_lod1, frame);
	
		if(mdl->weapon_lod1 != NULL)
		{
		    DCE_RenderMD2ComputeMatrixS(player2, &mdl->weapon_lod1->frames[frame], r);
	    
	        DCE_RenderCompileAndSubmitTextureHeader(mdl->weapon_texID);

            DCE_RenderCacheIndexHQMD2(mdl->weapon_lod1, frame);
		}

	}
/*	else
	{
		struct md2_frame_t *pframe = &mdl->body_lod2->frames[frame];
		
        DCE_RenderCacheMD2Buf(mdl->body_lod2, frame,  DCE_CLIP_BUFFER);
        
        DCE_RenderModel(&pos, 1.0f, (vec3f_t *)pframe->translate, (vec3f_t *)pframe->scale,
	                              mdl->body_texID, mdl->body_lod2->header.num_tris, r, DCE_CLIP_BUFFER);	


		pframe = &mdl->weapon->frames[frame];
		               
        DCE_RenderCacheHQMD2Buf(mdl->weapon, frame,  DCE_CLIP_BUFFER);
        
        DCE_RenderModel(&pos, 1.0f, (vec3f_t *)pframe->translate, (vec3f_t *)pframe->scale,
	                              mdl->weapon_texID, mdl->weapon->header.num_tris, r, DCE_CLIP_BUFFER);
                            
	}
*/
}

extern md2_mdl * DCE_MuzzleflashModel();

void DCE_RenderEnemyLODTR(DCE_Player * player, DCE_Player * player2)
{   
	DCE_PlayerModel * mdl = &player2->model;
    float fov, d, r = 0;    
	vec3f_t V;
    vec3f_t pos = { player2->position.x, player2->position.y, player2->position.z };   

	unsigned char frame = mdl->body_frames[mdl->body_index][mdl->body_frame];
    
    vec3f_sub_normalize(pos.x, pos.y, pos.z,
	                    player->position.x, player->position.y, player->position.z,
	                    V.x, V.y, V.z);
    
 //   vec3f_dot(player->direction.x, player->direction.y, player->direction.z, V.x, V.y, V.z, fov);

//    if(fov < 0.25f && player->perspective == DCE_PERSPECTIVE_FIRST)
//        return;
    
    vec3f_distance(player->position.x, player->position.y, player->position.z,
	               pos.x, pos.y, pos.z, d);
	   
    pos.y -= player2->height;
    
    r = vec3f_vectors_to_angle_xz(&player2->position, &player2->lookAt) - 90.0f;
	    
	if(((mdl->body_index == WALK_FIRE) || (mdl->body_index == WALK_FIRE)) && mdl->body_frame > 2 && mdl->body_frame < 6)
	{
		DCE_RenderMD2ComputeMatrixS(player2, &DCE_MuzzleflashModel()->frames[0], r);
	    
	    DCE_RenderCompileAndSubmitTextureHeader(DCE_TexID(DCE_TEXID_MUZFLASH1 + rand() % 4));

        DCE_RenderCacheIndexHQMD2(DCE_MuzzleflashModel(), 0);
	}
}

void DCE_RenderPlayerLOD(DCE_Player * player, DCE_Player * player2)
{   
	DCE_PlayerModel * mdl = &player2->model;
    float fov, d, r = 0;    
	vec3f_t V;
    vec3f_t pos = { player2->position.x, player2->position.y, player2->position.z };   

	unsigned char frame = mdl->body_frames[mdl->body_index][mdl->body_frame];
    
    vec3f_sub_normalize(pos.x, pos.y, pos.z,
	                    player->position.x, player->position.y, player->position.z,
	                    V.x, V.y, V.z);
    
    vec3f_dot(player->direction.x, player->direction.y, player->direction.z, V.x, V.y, V.z, fov);

    //if(fov < 0.25f)
    //    return;
    
    vec3f_distance(player->position.x, player->position.y, player->position.z,
	               pos.x, pos.y, pos.z, d);
	   
    pos.y -= player2->height;
    
    r = vec3f_vectors_to_angle_xz(&player2->position, &player2->lookAt) - 90.0f;

	if(player2->state & PSTATE_DEAD)
	{
		if(player2->state_frame == 0)
		    DCE_SfxPlayAttenuation(ENEMY_DEATH1 + (rand() % 3), &player2->position, &player->position);
		    
	    if(player2->model.body_frame == player2->model.body_frame_count[player2->model.body_index] - 8)
	       	if(!(player2->state_frame % 2))
			   DCE_SfxPlayAttenuation(IMPACT_FLESH2, &player2->position, &player->position);
			
		if(player2->model.body_frame >= player2->model.body_frame_count[player2->model.body_index] - 6)
		{
			unsigned char frame;
			frame = (player2->state_frame - (player2->model.body_frame_count[player2->model.body_index] - 6)) / 5;
        	if(frame > 24)
        	    frame = 24;
	        	        
	    	DCE_RenderMD2ComputeMatrix(player2, &mdl->blood->frames[frame], r);
	    
	    	DCE_RenderCompileAndSubmitTextureHeader(mdl->blood_texID);

            mat_translate(0.0f, 1.0f, 0.0f);

            DCE_RenderCacheIndexHQMD2(mdl->blood, frame);	                              
		}
	}
		    
//	if(d < 100 || mdl->body_lod1 == NULL) // LOD 0
	{
		DCE_RenderMD2ComputeMatrixS(player2, &mdl->body->frames[frame], r);
	    
	    DCE_RenderCompileAndSubmitTextureHeader(mdl->body_texID);

        DCE_RenderCacheIndexHQMD2(mdl->body, frame);

////
        if(mdl->body_lod1 != NULL)
        {
        DCE_RenderMD2ComputeMatrixS(player2, &mdl->body_lod1->frames[frame], r);
	    
	    DCE_RenderCompileAndSubmitTextureHeader(mdl->head_texID);

        DCE_RenderCacheIndexHQMD2(mdl->body_lod1, frame);
		}
		
        if(mdl->body_lod2 != NULL)
        {		
		DCE_RenderMD2ComputeMatrixS(player2, &mdl->body_lod2->frames[frame], r);
	    
	    DCE_RenderCompileAndSubmitTextureHeader(mdl->legs_texID);

        DCE_RenderCacheIndexHQMD2(mdl->body_lod2, frame);
        }
////
	
		if(mdl->weapon_lod1 != NULL)
		{
		    DCE_RenderMD2ComputeMatrixS(player2, &mdl->weapon_lod1->frames[frame], r);
	    
	        DCE_RenderCompileAndSubmitTextureHeader(mdl->weapon_texID);

            DCE_RenderCacheIndexHQMD2(mdl->weapon_lod1, frame);
		}
	}
/*
	else if(d < 200 || mdl->body_lod2 == NULL) // LOD1
	{
		DCE_RenderMD2ComputeMatrixS(player2, &mdl->body_lod1->frames[frame], r);
	    
	    DCE_RenderCompileAndSubmitTextureHeader(mdl->body_texID);

        DCE_RenderCacheIndexHQMD2(mdl->body_lod1, frame);
	
		if(mdl->weapon_lod1 != NULL)
		{
		    DCE_RenderMD2ComputeMatrixS(player2, &mdl->weapon_lod1->frames[frame], r);
	    
	        DCE_RenderCompileAndSubmitTextureHeader(mdl->weapon_texID);

            DCE_RenderCacheIndexHQMD2(mdl->weapon_lod1, frame);
		}

	}

*/
/*	else
	{
		struct md2_frame_t *pframe = &mdl->body_lod2->frames[frame];
		
        DCE_RenderCacheMD2Buf(mdl->body_lod2, frame,  DCE_CLIP_BUFFER);
        
        DCE_RenderModel(&pos, 1.0f, (vec3f_t *)pframe->translate, (vec3f_t *)pframe->scale,
	                              mdl->body_texID, mdl->body_lod2->header.num_tris, r, DCE_CLIP_BUFFER);	


		pframe = &mdl->weapon->frames[frame];
		               
        DCE_RenderCacheHQMD2Buf(mdl->weapon, frame,  DCE_CLIP_BUFFER);
        
        DCE_RenderModel(&pos, 1.0f, (vec3f_t *)pframe->translate, (vec3f_t *)pframe->scale,
	                              mdl->weapon_texID, mdl->weapon->header.num_tris, r, DCE_CLIP_BUFFER);
                            
	}
*/
}

void DCE_RenderModelASM(vec3f_t *p, float scale, vec3f_t * mtrans, vec3f_t * mscale,
                               unsigned short texID, int tris, float r, pvr_vertex_t * vert)
{
    unsigned char c = 0xFFFFFFFF;
    
	DCE_MatrixLoadProjection();
        
    DCE_MatrixApplyPlayer(0);
              
    mat_translate(p->x, p->y, p->z);

	mat_rotate(DEG2RAD * 90.0f, 0.0f, 0.0f);
	
	mat_rotate(0.0f, 0.0f, DEG2RAD * r);
	
	mat_scale(scale, scale, scale);

    mat_translate(mtrans->x, mtrans->y, mtrans->z);

	mat_scale(mscale->x, mscale->y, mscale->z);

    DCE_RenderEnableCull(1);
    
    DCE_RenderSetCullFace(DCE_PVR_CULL_BACK);    
    
    DCE_RenderSetCullFunc(DCE_PVR_CULL_CW);   
    
 	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, texID, NULL);
    
    DCE_RenderCullTris(&vert->x, &vert->u, tris, sizeof(pvr_vertex_t));	
    
	DCE_RenderEnableCull(0);    
}


void DCE_RenderPlayerHQMD2(DCE_PlayerModel * mdl, vec3f_t * pos, float r, unsigned char frame)
{
        struct md2_frame_t *pframe = &mdl->body->frames[frame];
        
		DCE_RenderCacheHQMD2Buf(mdl->body, frame,  DCE_CLIP_BUFFER);
        
        DCE_RenderModelASM(pos, 1.0f, (vec3f_t *)pframe->translate, (vec3f_t *)pframe->scale,
	                              mdl->body_texID, mdl->body->header.num_tris, r, DCE_CLIP_BUFFER);
}


void DCE_RenderPlayerWeaponHQMD2(DCE_PlayerModel * mdl, vec3f_t * pos, float r, unsigned char frame)
{
        struct md2_frame_t *pframe = &mdl->body->frames[frame];
        
		DCE_RenderCacheHQMD2Buf(mdl->weapon, frame,  DCE_CLIP_BUFFER);
        
        DCE_RenderModelASM(pos, 1.0f, (vec3f_t *)pframe->translate, (vec3f_t *)pframe->scale,
	                              mdl->weapon_texID, mdl->weapon->header.num_tris, r, DCE_CLIP_BUFFER);
}


