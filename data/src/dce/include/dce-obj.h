#ifndef DCE_OBJ_LOADER_H
#define DCE_OBJ_LOADER_H

#define DCE_MAX_MODEL_FRAMES 200
#define DCE_MAX_MODEL_FRAMELEN 16

#define DCE_FRAME_MODE_ASCEND  0
#define DCE_FRAME_MODE_DESCEND 1

typedef struct
{
    float x, y, z;
    float u, v;
    unsigned int color;
} DCE_SimpleVertex;

typedef struct
{
	DCE_SimpleVertex * vertex;
	unsigned int triangles;
	unsigned short texID0;
	unsigned short texID1;
} DCE_SimpleModel;

DCE_SimpleModel * DCE_LoadOBJ(char * fname);

void DCE_RenderMDL(DCE_SimpleModel * dmdl, unsigned int frame, vec3f_t *position, const int rotation, float scale);

#endif
