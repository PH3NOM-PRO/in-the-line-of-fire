#ifndef DCE_TEXTURE_C
#define DCE_TEXTURE_C

#include <dc/pvr.h>

#include "dce-vector.h"

#include "jpeg/jpeglib.h"

typedef struct
{
	unsigned short w, h;
	unsigned int color;
	unsigned char filter;
	pvr_ptr_t data;
	unsigned int size;
	unsigned char bound;
} DCE_Texture;

#define DCE_MAX_BOUND_TEXTURES 512

void DCE_InitTextures();

unsigned short DCE_TextureBind(unsigned short width, unsigned short height, unsigned int bytes,
							   unsigned int color, void * data);

unsigned short DCE_TextureBindData(unsigned short width, unsigned short height, unsigned int bytes,
							       unsigned int color, void * data);

void DCE_TextureUnBind(unsigned short index);

DCE_Texture * DCE_TextureObject(unsigned short index);

unsigned short int DCE_TextureLoadJPG(char *fname);

unsigned short int DCE_TextureLoadJPG1555(char *fname);

unsigned short int DCE_TextureLoadPVR(char *fname);

unsigned short int DCE_TextureLoadPVRBump(char *fname);

unsigned short DCE_TextureWidth(unsigned short index);

unsigned short DCE_TextureHeight(unsigned short index);

unsigned int DCE_TextureSize(unsigned short index);

void DCE_TextureDownsampleHalfSize(uint16 *src, uint16 *dst, uint16 width, uint16 height);

void DCE_TextureCoordProjected(vec3f_t *eye, vec3f_t *point, vec3f_t *normal,
                        float *s, float *t);

void DCE_TextureCoordProjectedFast(vec3f_t *point, float *s, float *t);

unsigned int DCE_TextureSample2D565(unsigned short int * buf, unsigned short int w, unsigned short int h,
                                    float u, float v);
uint16 * DCE_TextureLoadJPGBuf(char *fname, unsigned short int * w, unsigned short int * h);

uint16 DCE_TextureLoadTGA1555(char * fname);

void DCE_LoadTextureBatch(char * dir);

unsigned short DCE_TextureCacheIndex(char * name); // Give the name of texture and recieve its bound texID

#endif
