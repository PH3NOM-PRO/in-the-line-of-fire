#ifndef DCE_CAMERA_H
#define DCE_CAMERA_H

typedef struct
{
	vec3f_t pos;
	vec3f_t lookAt;
	vec3f_t dir;
	vec3f_t vantage_pos;
	float rx, ry;
	float mx, my;
} DCE_Camera;



#endif
