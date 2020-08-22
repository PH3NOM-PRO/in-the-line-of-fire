#ifndef DCE_MODEL_H
#define DCE_MODEL_H

#define DCE_RBO_OP 0
#define DCE_RBO_TR (1<<0)
#define DCE_RBO_PT (1<<1)

typedef struct
{
	float x, y, z;      /* 3 Float Vertex */
} vec3f;

typedef struct
{
	float u, v;         /* 2 Float Texture Coordinate */
} tex2f;

typedef struct
{
	unsigned char a, r, g, b;
} color32;              /* 32 bit ARGB color */

typedef struct
{
	vec3f pos;           /* Vertex Position */
	tex2f texcoord;      /* Vertex Texture Coordinates */
	color32 color;       /* Vertex ARGB Color */
} DCE_Vertex;

typedef struct
{
	char tex_name[256];  /* Material Texture Name */
	color32 Ke;          /* Material Emmisive Light Factor */             
	vec3f   Ka;          /* Material Ambient Light Factor */
	vec3f   Kd;          /* Material Diffuse Light Factor */
	vec3f   Ks;          /* Material Specular Light Factor */
	float   Ns;          /* Material Normal Specular Factor */
} DCE_Material;

typedef struct
{
    DCE_Material mtl;          /* Surface Material */
    unsigned short int texID;  /* Texture Index (set by engine) */
    unsigned int verts;        /* Number of vertices in object */
    DCE_Vertex * vert;         /* Vertex Array */
} DCE_RenderObject;

typedef struct
{
	vec3f v1;                 /* 1st Vertex of Triangle */
	vec3f edge1;              /* Pre-Computed Edge 1 */
	vec3f edge2;              /* Pre-Computed Edge 2 */
	vec3f normal;             /* Surface Normal */
	vec3f P;                  /* Pre-Computed Gravity P Vector */
	float inv_det;            /* Pre-Computed Gravity Inverse Determinant */
	int epsilon;    /* Pre-Computed Gravity Boolean Inside Epsilon Range */
} DCE_CollisionTriangle;

typedef struct
{
	vec3f bbmin;                   /* Bounding Box Mins */
	vec3f bbmax;                   /* Bounding Box Maxs */

	float half_width;              /* Bounding Box Width */
	float half_depth;              /* Bounding Box Depth */

	unsigned int triangles;        /* Triangle Count In this Bounding Box */

	unsigned short int nodes;

	DCE_CollisionTriangle * tris;  /* Triangle Array */

    void * parent;
	void * node[4];                /* DCE_CollisionMesh Children Nodes For Quad Tree */

} DCE_CollisionQuadTreeNode;

typedef struct
{
	vec3f_t       trans;
	float         rot;
} DCE_RenderObjectInstance;

typedef struct
{
	DCE_RenderObject rbo;
	char             segment[32];
	unsigned char    shadow;
	unsigned char    flags;
	unsigned char    instances;
	unsigned char    active[5];
	DCE_RenderObjectInstance * instance;
} DCE_RenderCluster;

void DCE_QuadTreeInsert(DCE_CollisionQuadTreeNode * qt, DCE_CollisionTriangle * tri);

DCE_CollisionQuadTreeNode * DCE_QuadTreeNewParentNode(float minx, float miny, float minz,
                                                float maxx, float maxy, float maxz, float w, float d);

DCE_CollisionQuadTreeNode * DCE_QuadTreeNewNode(DCE_CollisionQuadTreeNode * parent, float minx, float miny, float minz,
                                                float maxx, float maxy, float maxz, float w, float d);
                                                
unsigned int DCE_QuadTreeFindCollision(DCE_CollisionQuadTreeNode * qt, vec3f * p, vec3f * r, float * d);

unsigned int DCE_QuadTreeTraceRayCollision(DCE_CollisionQuadTreeNode * qt, vec3f * p, vec3f * r, float * d);

DCE_CollisionQuadTreeNode * DCE_QuadTreeLoad(char * fname);

void DCE_RenderCallbackRBO(unsigned char mode, unsigned char index);
void DCE_RenderCallbackRBOShadows(unsigned char index);

unsigned char DCE_CheckCollisionCBO(vec3f_t * O, vec3f_t * D, float * d);
unsigned char DCE_LoadCBOBatch(char * fname);

void DCE_RBOSetVertexColor(unsigned int argb);

void DCE_LoadRBO(char * fname, DCE_RenderObject * rbo, char * tex_name);

inline void DCE_RenderRBO(DCE_RenderObject * rbo);

unsigned char DCE_ReadCBO(char * fname, DCE_CollisionQuadTreeNode * cmesh, unsigned char debug);

void DCE_TriggerRBO(char * segment, unsigned char active, unsigned char index);
void DCE_TriggerCBO(char * segment, unsigned char active, unsigned char index);

#endif
