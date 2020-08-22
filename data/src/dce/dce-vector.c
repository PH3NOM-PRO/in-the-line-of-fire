/*
** dce-vector.c (C) 2013-2015 Josh PH3NOM Pearson
**
** A small collection of functions for working with 3D Vectors
*/

#include "dce-vector.h"
#include <math.h>
#include <stdlib.h>

static float r2deg = 180.0f / 3.1415926535897932384626433832795;

float vec3f_dot_2d(vec3f_t * v)
{
    return (v->x * v->x) + (v->z * v->z);
}

void vec3f_mul_scalar_2d(vec3f_t * v1, float s, vec3f_t * v2)
{
    v2->x = s * v1->x;
    v2->z = s * v1->z;
}

void vec3f_normalize_2d(vec3f_t * v)
{
    float invsqrt = 1.0f / sqrtf(vec3f_dot_2d(v));
    
    vec3f_mul_scalar_2d(v, invsqrt, v);
}

float vec3_dot_c(vec3f_t * v, vec3f_t * v2)
{
    return (v->x * v2->x) + (v->y * v2->y) + (v->z * v2->z);
}

unsigned char vec3f_normal(const vec4f_t * V0, const vec4f_t * V1, const vec4f_t * V2)
{
	float d;
	
	vec3f_t E1, E2, N;
	
	E1.x = V1->x - V0->x;
	E1.y = V1->y - V0->y;
	E1.z = V1->w - V0->w;

	E2.x = V2->x - V0->x;
	E2.y = V2->y - V0->y;
	E2.z = V2->w - V0->w;
    
    vec3f_cross(&E1, &E2, &N); // E1 == Cross (E1, E2) aka N

    vec3f_normalize_c(&N);

    return N.z > 0;
}

unsigned char vec3f_face_in(const vec3f_t * V0, const vec3f_t * V1, const vec3f_t * V2)
{
	float d;
	
	vec3f_t E1, E2, N;
	
	vec3f_sub(V0, V1, &E1);

    vec3f_sub(V0, V2, &E2);
    
    vec3f_cross(&E1, &E2, &N); // E1 == Cross (E1, E2) aka N
    
    vec3f_invert2(V0, &E1);
    
    //vec3f_normalize_c(&E1);
    
    //printf("N: %f %f %f\n", N.x, N.y, N.z);
    
 //   vec3f_normalize_c(&N);
    
    //printf("Normal: %f %f %f\n", N.x, N.y, N.z);
    
    d = vec3_dot_c(&E1, &N);
    
    //vec3f_dot(-V0->x, -V0->y, -V0->z, N.x, N.y, N.z, d);
    
    //printf("D: %f\n", d);
    
    return d >= 0;
}

void vec3f_switch(vec3f_t * V1, vec3f_t * V2)
{
	vec3f_t V;
	
	V.x = V2->x;
	V.y = V2->y;
	V.z = V2->z;
	
	V2->x = V1->x;
	V2->y = V1->y;
	V2->z = V1->z;	
	
	V1->x = V.x;
	V1->y = V.y;
	V1->z = V.z;			
}

float vec3f_dot_c(vec3f_t * v)
{
    return (v->x * v->x) + (v->y * v->y) + (v->z * v->z);
}

void vec3f_normalize_c(vec3f_t * v)
{
    float invsqrt = 1.0f / sqrtf(vec3f_dot_c(v));
    
    vec3f_mul_scalar(v, invsqrt, v);
}

float vec3f_vectors_to_ray_deg(vec3f_t * O, vec3f_t * L, vec3f_t * P)
{
	vec3f_t R, V;

	vec3f_sub(O, L, &V);

    V.y = 0;

	vec3f_normalize(V.x, V.y, V.z);
	
	R.y = 0;
	
	vec3f_sub(O, P, &R);

	vec3f_normalize(R.x, R.y, R.z);	
	
	return (atan2(R.z, R.x) - atan2(V.z, V.x)) * DEGREE;
}

float vec3f_vec_to_angle_xz(vec3f_t * O, vec3f_t * L, vec3f_t * P)
{
	vec3f_t R, V;
	float d;

	vec3f_sub(O, L, &V);
	
	V.y = 0;

	vec3f_normalize(V.x, V.y, V.z);
	
	vec3f_sub(O, P, &R);
	
	R.y = 0;
	
	vec3f_normalize(R.x, R.y, R.z);
	
	vec3f_dot(R.x, R.y, R.z, V.x, V.y, V.z, d);

	return acos(d) * DEGREE;
}

float vec3f_vectors_to_angle_xz(vec3f_t * v1, vec3f_t * v2)
{
    vec3f_t v;
    
    vec3f_sub(v1, v2, &v);
    vec3f_normalize_c(&v);
    //vec3f_normalize(v.x, v.y, v.z);
    
    return r2deg * atan2(v.z, v.x);
}

void vec3f_angle_to_vector_xy(float degrees, vec3f_t * v)
{
	fsincos(degrees, &v->x, &v->y);
	v->z = 0;
}

void vec3f_angle_to_vector_xz(float degrees, vec3f_t * v)
{
	fsincos(degrees, &v->z, &v->x);
	v->y = 0;
}

void vec3f_angle_to_vector_yz(float degrees, vec3f_t * v)
{
	fsincos(degrees, &v->y, &v->z);
	v->x = 0;
}

void vec3f_rot(vec3f_t * v, float r)
{
	mat3f_t m, m2;
	float s, c;

	fsincos(r, &s, &c);

	m.v[0].x = -c;
	m.v[0].y = s;
	m.v[0].z = 0;
	m.v[1].x = -s;
	m.v[1].y = c;
	m.v[1].z = 0;
	m.v[2].x = 0;
	m.v[2].y = 0;
	m.v[2].z = 1;

	m2.v[0].x = c;
	m2.v[0].y = 0;
	m2.v[0].z = s;
	m2.v[1].x = 0;
	m2.v[1].y = 1;
	m2.v[1].z = 0;
	m2.v[2].x = -s;
	m2.v[2].y = 0;
	m2.v[2].z = c;

	mat3f_mul(&m, &m2);

	m2.v[0].x = 1;
	m2.v[0].y = 0;
	m2.v[0].z = 0;
	m2.v[1].x = 0;
	m2.v[1].y = c;
	m2.v[1].z = s;
	m2.v[2].x = 0;
	m2.v[2].y = -s;
	m2.v[2].z = c;

	mat3f_mul(&m, &m2);

	vec3f_mul_matrix3f(v, &m);
}

void vec3f_mul_matrix3f(vec3f_t * v, mat3f_t * m)
{
	v->x = ( m->v[0].x * v->x ) + ( m->v[0].y * v->y ) + ( m->v[0].z * v->z );
	v->y = ( m->v[1].x * v->x ) + ( m->v[1].y * v->y ) + ( m->v[1].z * v->z );
	v->z = ( m->v[2].x * v->x ) + ( m->v[2].y * v->y ) + ( m->v[2].z * v->z );
}

void mat3f_mul(mat3f_t * m1, mat3f_t * m2)
{
	vec3f_mul_matrix3f(&m1->v[0], m2);
	vec3f_mul_matrix3f(&m1->v[1], m2);
	vec3f_mul_matrix3f(&m1->v[2], m2);
}

unsigned char vec3f_equal(vec3f_t * v1, vec3f_t * v2)
{
	return v1->x == v2->x && v1->y == v2->y && v1->z == v2->z;
}

unsigned char vec3f_equal_inv(vec3f_t * v1, vec3f_t * v2)
{
	return v1->x == -v2->x && v1->y == -v2->y && v1->z == -v2->z;
}

void vec3f_major_axis(vec3f_t * v, vec3f_t * m)
{
	if(( fabs(v->x) > fabs(v->y) ) && ( fabs(v->x) > fabs(v->z) ))
	{
		m->x = 1;
		m->y = 0; 
		m->z = 0;
	}
	else if(( fabs(v->y) > fabs(v->x) ) && ( fabs(v->y) > fabs(v->z) ))
	{
		m->x = 0;
		m->y = -1; 
		m->z = 0;
	}
	else
	{
		m->x = 0;
		m->y = 0; 
		m->z = 1;
	}
}

unsigned char vec3f_major_axis_component(vec3f_t * v)
{
	if(( fabs(v->x) > fabs(v->y) ) && ( fabs(v->x) > fabs(v->z) ))
		return 0;

	if(( fabs(v->y) > fabs(v->x) ) && ( fabs(v->y) > fabs(v->z) ))
	    return 1;
	
	return 2;
}

void vec3f_rotated_xy(vec3f_t * p, vec3f_t * o, float angle) 
{ 
    float r[2];
	float s, c;

	fsincos(angle, &s, &c);

	r[0] = (o->x + ( ( p->x - o->x ) * c ) - ( ( p->y - o->y ) * s )); 
    r[1] = (o->y + ( ( p->x - o->x ) * s ) + ( ( p->y - o->y ) * c )); 
 
	p->x = r[0]; 
    p->y = r[1]; 
}

void vec3f_rotated_xz(vec3f_t * p, vec3f_t * o, float angle) 
{ 
    float r[2];
	float s, c;

	fsincos(angle, &s, &c);

	r[0] = (o->x + ( ( p->x - o->x ) * c ) - ( ( p->z - o->z ) * s )); 
    r[1] = (o->z + ( ( p->x - o->x ) * s ) + ( ( p->z - o->z ) * c )); 
 
	p->x = r[0]; 
    p->z = r[1]; 
}

void vec3f_rotated_xz_c(vec3f_t * p, vec3f_t * o, float angle) 
{ 
    float r[2];
	float s, c;
    
    s = sin(angle * RADIAN);
    c = cos(angle * RADIAN);

	r[0] = (o->x + ( ( p->x - o->x ) * c ) - ( ( p->z - o->z ) * s )); 
    r[1] = (o->z + ( ( p->x - o->x ) * s ) + ( ( p->z - o->z ) * c )); 
 
	p->x = r[0]; 
    p->z = r[1]; 
}


void vec3f_cross(vec3f_t * v1, vec3f_t * v2, vec3f_t * out)
{
    out->x = v1->y * v2->z - v1->z * v2->y;
    out->y = v1->z * v2->x - v1->x * v2->z;
    out->z = v1->x * v2->y - v1->y * v2->x;
}

inline void vec3f_cross3f(vec3f_t * v1, vec3f_t * v2, float * x, float * y , float * z)
{
    *x = v1->y * v2->z - v1->z * v2->y;
    *y = v1->z * v2->x - v1->x * v2->z;
    *z = v1->x * v2->y - v1->y * v2->x;
}

inline void vec3f_cross6f(float v1x, float v1y, float v1z, vec3f_t * v2, float * x, float * y , float * z)
{
    *x = v1y * v2->z - v1z * v2->y;
    *y = v1z * v2->x - v1x * v2->z;
    *z = v1x * v2->y - v1y * v2->x;
}

void vec3f_reflect(vec3f_t * i, vec3f_t *  n, vec3f_t * out)
{
    float d;

	vec3f_dot(n->x, n->y, n->z, i->x, i->y, i->z, d);

	out->x = i->x - (2.0 * n->x * d);
	out->y = i->y - (2.0 * n->y * d);
	out->z = i->z - (2.0 * n->z * d);
}

void vec3f_lerp(vec3f_t * v1, vec3f_t * v2, vec3f_t * out, float mag)
{
	out->x = v1->x + (( v2->x - v1->x ) * mag);
	out->y = v1->y + (( v2->y - v1->y ) * mag);
	out->z = v1->z + (( v2->z - v1->z ) * mag);
}

void vec3f_copy(vec3f_t * src, vec3f_t * dst)
{
	*dst = *src;
}

void vec3f_shift_xz(vec3f_t * p, vec3f_t * c, float mag) 
{ 
    float d[2];
	d[0] = (p->x - c->x) * mag;   
    d[1] = (p->z - c->z) * mag;   
    p->x += d[0];
	c->x += d[0]; 
    p->z += d[1];
	c->z += d[1]; 
}

void vec3f_shift(vec3f_t * p, vec3f_t * c, float mag) 
{ 
    vec3f_t d;
    d.x = (p->x - c->x) * mag;   
    d.y = (p->y - c->y) * mag;  
    d.z = (p->z - c->z) * mag;   
    p->x += d.x;
	c->x += d.x; 
    p->y += d.y;
	c->y += d.y; 
    p->z += d.z;
	c->z += d.z; 
}

void vec3f_shiftp(vec3f_t * p, vec3f_t * c, float mag) 
{ 
    vec3f_t d;
    d.x = (p->x - c->x) * mag;   
    d.y = (p->y - c->y) * mag;  
    d.z = (p->z - c->z) * mag;   
    p->x += d.x;
    p->y += d.y;
    p->z += d.z;
}

void vec3f_sub(vec3f_t * v1, vec3f_t * v2, vec3f_t * v3)
{
    v3->x = v2->x - v1->x;
    v3->y = v2->y - v1->y;
    v3->z = v2->z - v1->z;
}

void vec3f_add(vec3f_t * v1, vec3f_t * v2, vec3f_t * v3)
{
    v3->x = v2->x + v1->x;
    v3->y = v2->y + v1->y;
    v3->z = v2->z + v1->z;
}

void vec3f_mul(vec3f_t * v1, vec3f_t * v2, vec3f_t * v3)
{
    v3->x = v2->x * v1->x;
    v3->y = v2->y * v1->y;
    v3->z = v2->z * v1->z;
}

void vec3f_mul_scalar(vec3f_t * v1, float s, vec3f_t * v2)
{
    v2->x = s * v1->x;
    v2->y = s * v1->y;
    v2->z = s * v1->z;
}

void vec3f_invert(vec3f_t * v)
{
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
}

void vec3f_invert2(vec3f_t * s, vec3f_t * d)
{
	d->x = -s->x;
	d->y = -s->y;
	d->z = -s->z;
}

void vec3f_div(vec3f_t * v1, vec3f_t * v2, vec3f_t * v3)
{
    v3->x = v2->x / v1->x;
    v3->y = v2->y / v1->y;
    v3->z = v2->z / v1->z;	
}

#define invert(f) f == 0 ? 0 : 1.0f / f

void vector_inverse(vec3f_t * src, vec3f_t * dst)
{
	dst->x = invert(src->x);
	dst->y = invert(src->y);
	dst->z = invert(src->z);
}
