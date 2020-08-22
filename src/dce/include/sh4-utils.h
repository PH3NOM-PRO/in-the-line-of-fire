#ifndef SH4_UTILS_H
#define SH4_UTILS_H

typedef struct {
    uint32  flags;              /**< \brief TA command (vertex flags) */
    float   x;                  /**< \brief X coordinate */
    float   y;                  /**< \brief Y coordinate */
    float   z;                  /**< \brief Z coordinate */
    float   u;                  /**< \brief Texture U coordinate */
    float   v;                  /**< \brief Texture V coordinate */
    uint32  argb;               /**< \brief Vertex color */
    float   w;                  /**< \brief Vertex W coordinate */
} gl_pvr_vertex_t;

int _glKosCopyDoublePrecision( void *src, void *dst, int stride, int count );

void _glKosCopyVertexPVRUV( void *src, void *dst, void *uvsrc, void *uvdst );

void _glKosCopyVertexPVR( void *src, void *dst );

void _glKosCopyVertexUV( void *src, void *dst );

//void _glKosFSCHG();

GLubyte TransformAndClipTri(float *vert_pos, int vert_stride, pvr_vertex_t *in, pvr_vertex_t * out);

GLubyte TransformAndClipTriangles(float *vert_pos, int vert_stride, pvr_vertex_t *in, pvr_vertex_t * out, float count);

GLubyte TransformAndClipTri2(float *vert_pos, float vert_stride,              
							   float *tex_coord, float uv_stride,             
							   pvr_vertex_t *in, pvr_vertex_t * out);  

GLubyte _glKosTransformTri2(float *vert_pos, float vert_stride, gl_pvr_vertex_t * out, float * tex_coord, float uv_stride, unsigned int * argb);
/*
GLuint _glKosTransformTriSQ(float *vert_pos,
                            float vert_stride,
                            gl_pvr_vertex_t * out,
                            float * tex_coord,
                            float uv_stride,
                            unsigned int * argb,
                            float count,
                            float argb_stride );
*/
void _glKosTransformTriSQ(float *vert_pos,
                            float vert_stride,
                            float * uv_coord,
                            float uv_stride,
                            unsigned int * argb,
                            float argb_stride,
                            pvr_vertex_t * dst,
                            float count);

void _glKosVertexCopy(void * src, const void * dst);

#endif