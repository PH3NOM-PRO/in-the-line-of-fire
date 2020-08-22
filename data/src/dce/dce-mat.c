//
// DCE3D Dreamcast 3D Engine (c)2013-2016 Josh PH3NOM Pearson
//

#include "dce.h"

static matrix4f DCE_MAT_SCREENVIEW ALIGN32;
static matrix4f DCE_MAT_IDENTITY   ALIGN32;
static matrix4f DCE_MAT_LOOKAT     ALIGN32;
static matrix4f DCE_MAT_PROJECTION ALIGN32;

static float DCE_FOV = 60.0f;

static matrix4f DCE_MAT_FRUSTUM    ALIGN32 = {
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, -1.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f }
};

void DCE_InitMatrices()
{
	mat_identity();
	
	mat_store(&DCE_MAT_SCREENVIEW);
	mat_store(&DCE_MAT_IDENTITY);	
	mat_store(&DCE_MAT_LOOKAT);
	
	DCE_MatrixViewport(0, 0, vid_mode->width, vid_mode->height);
}

void DCE_MatrixViewport(float x, float y, float width, float height)
{
    DCE_MAT_SCREENVIEW[0][0] = width * 0.5f;
    DCE_MAT_SCREENVIEW[1][1] = -height * 0.5f;
    DCE_MAT_SCREENVIEW[2][2] = 1;
    DCE_MAT_SCREENVIEW[3][0] = DCE_MAT_SCREENVIEW[0][0] + x;
    DCE_MAT_SCREENVIEW[3][1] = vid_mode->height - (-DCE_MAT_SCREENVIEW[1][1] + y);
}

void DCE_MatrixLookAt(vec3f_t * origin, vec3f_t * direction, vec3f_t * upVec)
{
    //Side = forward x up
    vec3f_cross3f(direction, upVec, &DCE_MAT_LOOKAT[0][0], &DCE_MAT_LOOKAT[1][0], &DCE_MAT_LOOKAT[2][0]);
    vec3f_normalize(DCE_MAT_LOOKAT[0][0], DCE_MAT_LOOKAT[1][0], DCE_MAT_LOOKAT[2][0]);

    //Recompute up as: up = side x forward
    vec3f_cross6f(DCE_MAT_LOOKAT[0][0], DCE_MAT_LOOKAT[1][0], DCE_MAT_LOOKAT[2][0],
	              direction, &DCE_MAT_LOOKAT[0][1], &DCE_MAT_LOOKAT[1][1], &DCE_MAT_LOOKAT[2][1]);

    DCE_MAT_LOOKAT[0][2] = -direction->x;
    DCE_MAT_LOOKAT[1][2] = -direction->y;
    DCE_MAT_LOOKAT[2][2] = -direction->z;

    DCE_MAT_LOOKAT[3][0] = DCE_MAT_LOOKAT[3][1] = DCE_MAT_LOOKAT[3][2] = 0;
    DCE_MAT_LOOKAT[0][3] = DCE_MAT_LOOKAT[1][3] = DCE_MAT_LOOKAT[2][3] = 0;
    DCE_MAT_LOOKAT[3][3] = 1;

    mat_apply(&DCE_MAT_LOOKAT);
    mat_translate(-origin->x, -origin->y, -origin->z);
}

void DCE_MatrixFrustum(float left, float right,
                       float bottom, float top,
                       float znear, float zfar)
{
    DCE_MAT_FRUSTUM[0][0] = (2.0f * znear) / (right - left);
    DCE_MAT_FRUSTUM[2][0] = (right + left) / (right - left);
    DCE_MAT_FRUSTUM[1][1] = (2.0f * znear) / (top - bottom);
    DCE_MAT_FRUSTUM[2][1] = (top + bottom) / (top - bottom);
    DCE_MAT_FRUSTUM[2][2] = zfar / (zfar - znear);
    DCE_MAT_FRUSTUM[3][2] = -(zfar * znear) / (zfar - znear);
}

void DCE_MatrixPerspective(float angle, float aspect, float znear, float zfar)
{
    float xmin, xmax, ymin, ymax;

    ymax = znear * ftan(angle * F_PI / 360.0f);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    DCE_MatrixFrustum(xmin, xmax, ymin, ymax, znear, zfar);
}

void DCE_MatrixComputeProjection()
{
    mat_load(&DCE_MAT_SCREENVIEW);
    mat_apply(&DCE_MAT_FRUSTUM);
    mat_store(&DCE_MAT_PROJECTION);
}

inline void DCE_MatrixLoadProjection()
{
    mat_load(&DCE_MAT_PROJECTION);
}

inline void DCE_MatrixLoadIdentity()
{
	mat_load(&DCE_MAT_IDENTITY);
}

void DCE_MatrixInitRender()
{
	DCE_InitMatrices();
	
	DCE_MatrixPerspective(DCE_FOV, 16.0f / 9.0f, 0.1f, 10000.0f);
	
	DCE_MatrixComputeProjection();
	
	DCE_MatrixLoadIdentity();
}

#include "dce-render.h"

static vec3f_t up = { 0, 1, 0 };

static matrix4f DCE_MAT_PLAYER[4] ALIGN32;

#define DCE_CAMERA_EPSILON 10.0f

// Compute Player View Matrix //
void DCE_MatrixComputePlayer(DCE_Player * player, DCE_Camera * cam, unsigned char index)
{
	DCE_MatrixLoadIdentity();

    vec3f_sub_normalize(player->lookAt.x, player->lookAt.y, player->lookAt.z,
                        player->position.x, player->position.y, player->position.z,
						player->direction.x, player->direction.y, player->direction.z);
    
    
    if(player->perspective == DCE_PERSPECTIVE_FIRST)
    {
	    DCE_MatrixLookAt(&player->position, &player->direction, &up);
    }
    else if(player->perspective == DCE_PERSPECTIVE_THIRD) // Third Person Perspective 
    {
        float d, d2;
		vec3f_t from, dir, p, D;
        
        vec3f_invert2(&player->direction, &from);
        
        vec3f_add(&from, &player->position, &from);
        
        vec3f_shiftp(&from, &player->position, DCE_RENDER_TPS_OFFSET);
        
        from.y += 50.0f;
        
        vec3f_sub_normalize(from.x, from.y, from.z,
		                    player->position.x, player->position.y, player->position.z,
							dir.x, dir.y, dir.z);
       
        vec3f_copy(&player->position, &p);
       
        DCE_CheckCollisionCBO(&p, &dir, &d);
        
        if(d != DCE_MAX_RAY_DISTANCE)
		{
			vec3f_distance(from.x, from.y, from.z, player->position.x, player->position.y, player->position.z, d2);
            
            if(d < d2)
            {
				if(d > DCE_CAMERA_EPSILON)
				    d -= DCE_CAMERA_EPSILON;
			    if(d < DCE_CAMERA_EPSILON)
			        d = DCE_CAMERA_EPSILON;
			    
				vec3f_mul_scalar(&dir, d, &D);
        
                vec3f_add(&D, &player->position, &from);
			}
        }    
        
		vec3f_invert(&dir); // flip it back for rendering
        
        DCE_MatrixLookAt(&from, &dir, &up);
    }
    else  // Static Camera
    {
        vec3f_sub_normalize(cam->lookAt.x, cam->lookAt.y, cam->lookAt.z,
						    cam->pos.x, cam->pos.y, cam->pos.z, cam->dir.x, cam->dir.y, cam->dir.z);       
    
        DCE_MatrixLookAt(&cam->pos, &cam->dir, &up);
	}

	mat_store(&DCE_MAT_PLAYER[index]);
}

inline void DCE_MatrixApplyPlayer(unsigned char index)
{
	mat_apply(&DCE_MAT_PLAYER[index]);
}
