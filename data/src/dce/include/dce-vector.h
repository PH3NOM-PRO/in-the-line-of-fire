/*
** vector.c (C) 2013 Josh PH3NOM Pearson
**
** A set of functions for working with 3D Vectors optimized for the SH4
*/

#ifndef VECTOR_H
#define VECTOR_H

#include <dc/fmath.h>
#include <dc/vec3f.h>

#define RAD2DEG (180.0f / F_PI)
#define RADIAN 0.0174532925   // Convert Degrees to Radians
#define DEGREE 57.295779515   // Convert Radian To Degree
#define CIRCLE 6.2831853      // RADIAN * 360.0f

typedef struct
{
	unsigned short int x, y;
}vec2i_t;

typedef struct
{
	float x, y, z, w;
} vec4f_t;

typedef struct
{
	vec3f_t v[3];
} mat3f_t;

typedef struct
{
	float u, v;
} vec2f_t;

float vec3f_dot_2d(vec3f_t * v);
void vec3f_mul_scalar_2d(vec3f_t * v1, float s, vec3f_t * v2);
void vec3f_normalize_2d(vec3f_t * v);


float vec3f_vectors_to_ray_deg(vec3f_t * O, vec3f_t * L, vec3f_t * P);

float vec3f_vec_to_angle_xz(vec3f_t * O, vec3f_t * L, vec3f_t * P);

void vec3f_switch(vec3f_t * V1, vec3f_t * V2);

float vec3f_vectors_to_angle_xz(vec3f_t * v1, vec3f_t * v2);

void vec3f_angle_to_vector_xy(float degrees, vec3f_t * v);
void vec3f_angle_to_vector_xz(float degrees, vec3f_t * v);
void vec3f_angle_to_vector_yz(float degrees, vec3f_t * v);

unsigned char vec3f_equal(vec3f_t * v1, vec3f_t * v2);
unsigned char vec3f_equal_inv(vec3f_t * v1, vec3f_t * v2);
unsigned char vec3f_major_axis_component(vec3f_t * v);

void vec3f_rot(vec3f_t * v, float r);
void vec3f_mul_matrix3f(vec3f_t * v, mat3f_t * m);
void mat3f_mul(mat3f_t * m1, mat3f_t * m2);
void vec3f_major_axis(vec3f_t * v, vec3f_t * m);
void vec3f_rotated_xy(vec3f_t * p, vec3f_t * o, float angle) ;
void vec3f_cross(vec3f_t * v1, vec3f_t * v2, vec3f_t * out);
inline void vec3f_cross3f(vec3f_t * v1, vec3f_t * v2, float * x, float * y , float * z);
inline void vec3f_cross6f(float v1x, float v1y, float v1z, vec3f_t * v2, float * x, float * y , float * z);
void vec3f_reflect(vec3f_t * i, vec3f_t *  n, vec3f_t * out);
void vec3f_lerp(vec3f_t * v1, vec3f_t * v2, vec3f_t * out, float mag);
void vec3f_copy(vec3f_t * src, vec3f_t * dst);
void vec3f_shift_xz(vec3f_t * p, vec3f_t * c, float mag);
void vec3f_shift(vec3f_t * p, vec3f_t * c, float mag);
void vec3f_shiftp(vec3f_t * p, vec3f_t * c, float mag) ;
void vec3f_sub(vec3f_t * v1, vec3f_t * v2, vec3f_t * v3);
void vec3f_add(vec3f_t * v1, vec3f_t * v2, vec3f_t * v3);
void vec3f_mul(vec3f_t * v1, vec3f_t * v2, vec3f_t * v3);
void vec3f_mul_scalar(vec3f_t * v1, float s, vec3f_t * v2);
void vec3f_invert(vec3f_t * v);
void vec3f_invert2(vec3f_t * src, vec3f_t * dst);
void vec3f_div(vec3f_t * v1, vec3f_t * v2, vec3f_t * v3);
void vec3f_rotated_xz(vec3f_t * p, vec3f_t * o, float angle) ;
void vec3f_rotated_xz_c(vec3f_t * p, vec3f_t * o, float angle);

unsigned char vec3f_face_in(const vec3f_t * V0, const vec3f_t * V1, const vec3f_t * V2);

#ifndef max
#define max(a,b) a>b ? a : b
#endif
#ifndef min
#define min(a,b) a<b ? a : b
#endif

#define mat_trans_single3_nodivw_nomod(x, y, z, xo, yo, zo, wo) { \
        register float __x __asm__("fr12") = (x); \
        register float __y __asm__("fr13") = (y); \
        register float __z __asm__("fr14") = (z); \
        register float __w __asm__("fr15"); \
        __asm__ __volatile__( \
                              "fldi1 fr15\n" \
                              "ftrv  xmtrx, fv12\n" \
                              : "=f" (__x), "=f" (__y), "=f" (__z), "=f" (__w) \
                              : "0" (__x), "1" (__y), "2" (__z), "3" (__w) ); \
        xo = __x; yo = __y; zo = __z; wo = __w; \
    }

/* SH4 fmac - Returns a*b+c at the cost of a single floating-point operation */
#define fmac( a, b, c ) ({ \
     register float __FR0 __asm__("fr0") = a; \
     register float __FR1 __asm__("fr1") = b; \
     register float __FR2 __asm__("fr2") = c; \
     __asm__ __volatile__( \
        "fmac   fr0, fr1, fr2\n" \
        : "=f" (__FR0), "=f" (__FR1), "=f" (__FR2) \
        : "0" (__FR0), "1" (__FR1), "2" (__FR2) \
        ); \
     __FR2; })

#endif
