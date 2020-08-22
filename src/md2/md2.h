
#ifndef DCE_REGISTER_MD2
#define DCE_REGISTER_MD2

#include "dce-vector.h"
#include <dc/pvr.h>

/* Vector */
typedef float vec3_t[3];

/* MD2 header */
struct md2_header_t
{
  int ident;
  int version;

  int skinwidth;
  int skinheight;

  int framesize;

  int num_skins;
  int num_vertices;
  int num_st;
  int num_tris;
  int num_glcmds;
  int num_frames;

  int offset_skins;
  int offset_st;
  int offset_tris;
  int offset_frames;
  int offset_glcmds;
  int offset_end;
};

/* Texture name */
struct md2_skin_t
{
  char name[64];
};

/* Texture coords */
struct md2_texCoord_t
{
  short s;
  short t;
};

struct md2_texCoord_uv
{
    float u, v;       
};       

/* Triangle info */
struct md2_triangle_t
{
  unsigned short vertex[3];
  unsigned short st[3];
};

/* Compressed vertex */
struct md2_vertex_t
{
  unsigned char v[3];
  unsigned char normalIndex;
};

/* Model frame */
struct md2_frame_t
{
  vec3_t scale;
  vec3_t translate;
  char name[16];
  struct md2_vertex_t *verts;
};

/* HQ vertex */
struct hqmd2_vertex_t
{
  unsigned short v[3];
};

/* HQ Model frame */
struct hqmd2_frame_t
{
  vec3_t scale;
  vec3_t translate;
  unsigned int *name[4];
  struct hqmd2_vertex_t *verts;
};

/* GL command packet */
struct md2_glcmd_t
{
  float s;
  float t;
  int index;
};

/* MD2 model structure */
struct md2_model_t
{
  struct md2_header_t header;

  struct md2_skin_t *skins;
  struct md2_texCoord_t *texcoords;
  struct md2_texCoord_uv *uvcoords;
  struct md2_triangle_t *triangles;
  struct md2_frame_t *frames;
  int *glcmds;
  
  unsigned int **clip_code;
};

typedef struct
{
    float x, y, z;
    float u, v;
    unsigned int color;
} MD2_SimpleVertex;

typedef struct md2_model_t md2_mdl;

md2_mdl * DCE_LoadMD2 (const char *filename);

md2_mdl * DCE_LoadHQMD2 (const char *filename);	

md2_mdl * DCE_LoadHQMD2Clip(const char *filename);

void DCE_DeleteMD2(struct md2_model_t *mdl);

void DCE_RenderCacheMD2Buf(md2_mdl * mdl, unsigned char frame, pvr_vertex_t * vert);
								
void DCE_RenderCacheIndexHQMD2(md2_mdl * mdl, unsigned char frame);
				                          
#endif
