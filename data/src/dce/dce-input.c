/*
** input.c (C) 2013 Josh PH3NOM Pearson
**
** An input callback to update the camera position and destination
** based on user input in a 3d environment. Only meant for testing.
*/

#include "dce.h"
#include <math.h>

static const float RSHIFT = RADIAN*90.0f;
static const float FRSHIFT = RADIAN*270.0f;

static unsigned short DCE_CONTROLLER_INVERT_YAXIS = 0;

static float DCE_FPS_SPEED_SCALE = 0;
static float DCE_YAXIS_MOVE_SPEED = 0;
static float DCE_XAXIS_MOVE_SPEED = 0;
static float DCE_MOVE_SPEED = 0;

static float DCE_STEADY_MOVE_FACTOR = 0.1f;

void DCE_ControllerRumble(unsigned char cont_num, unsigned char len)
{
	purupuru_effect_t rumble;
	
	rumble.duration = len;
	
	rumble.effect1 = PURUPURU_EFFECT1_PULSE;
	rumble.effect2 = PURUPURU_EFFECT2_PULSE;
    rumble.special = PURUPURU_SPECIAL_PULSE | PURUPURU_SPECIAL_MOTOR1;

    purupuru_rumble(maple_enum_type(cont_num, MAPLE_FUNC_CONTROLLER), &rumble);
}

void DCE_InitInput()
{
	DCE_FPS_SPEED_SCALE = 30.0f / DCE_RenderGetFPS(); // Movement Time Slice Based on Frame Time
	DCE_YAXIS_MOVE_SPEED = .75f * DCE_FPS_SPEED_SCALE;
	DCE_XAXIS_MOVE_SPEED = RADIAN * 10.0f * DCE_FPS_SPEED_SCALE;
	DCE_MOVE_SPEED = 0.8f * DCE_FPS_SPEED_SCALE;
}

float DCE_TimeSliceMoveSpeed()
{
    return DCE_MOVE_SPEED * .75f;
}

float DCE_LerpTriQuadratic(float f)
{
	if(f < 0)
	{
		f *= f;
		f *= f;
		f *= f;
		f = -f;
	}
	else
	{
		f *= f;
		f *= f;
		f *= f;
	}
	
	return f;
}

float DCE_LerpBiQuadratic(float f)
{
	if(f < 0)
	{
		f *= f;
		f *= f;
		f = -f;
	}
	else
	{
		f *= f;
		f *= f;
	}
	
	return f;
}

float DCE_LerpQuadratic(float f)
{
	if(f < 0)
	{
		f *= f;
		f = -f;
	}
	else
	{
		f *= f;
	}
	
	return f;
}

extern unsigned char DCE_HackEnabled();


int DCE_InputCallback(DCE_Player * p, unsigned char index)
{
    DCE_InitInput();
    
    maple_device_t *cont = maple_enum_type(index, MAPLE_FUNC_CONTROLLER);    

    if (cont)
    {          
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);
         
        if(!state)
           return 0;
        
        float move_speed = p->state & PSTATE_DUCK ? DCE_MOVE_SPEED / 2.0f : DCE_MOVE_SPEED;
        
        p->lstate = p->state;
        
        p->state &= ~PSTATE_MOVE;
        p->state &= ~PSTATE_FIRE2;
        p->state &= ~PSTATE_WALK_FORWARD;
        p->state &= ~PSTATE_WALK_BACKWARD;
        p->state &= ~PSTATE_WALK_STRAFE_LEFT;
        p->state &= ~PSTATE_WALK_STRAFE_RIGHT;
		        
        if(state->ltrig)
        {
            //if(state->rtrig)
			if(!(p->lstate & PSTATE_FIRE2))
			    p->state |= PSTATE_FIRE2;
        
			if(state->buttons & CONT_DPAD_LEFT)
			{
			    p->light_on = 0;
			    if(!DCE_HackEnabled())
    			p->perspective = DCE_PERSPECTIVE_STATIC;
			}
/*		    
			if(state->buttons & CONT_DPAD_RIGHT)
			    p->light_on = p->perspective == DCE_PERSPECTIVE_FIRST ? 1 : 0;
			    
            if(state->buttons & CONT_DPAD_UP)
			    p->perspective = DCE_PERSPECTIVE_FIRST;

		    if(state->buttons & CONT_DPAD_DOWN)
		    {
			    //p->perspective = DCE_PERSPECTIVE_THIRD;
                p->light_on = 0;
            }			
*/
        }
        else
        {
 		    if(state->buttons & CONT_DPAD_UP)
		        p->state &= ~PSTATE_DUCK;

		    if(state->buttons & CONT_DPAD_DOWN)
			    p->state |= PSTATE_DUCK;
			    
			if(state->buttons & CONT_DPAD_LEFT)
			{
				if(p->clip < p->clip_size && p->ammo)
				{
                    //if(!p->state & PSTATE_RELOAD)
                    {
					
					    p->last_moved = 1;
                    }
				}
			}
			if(state->buttons & CONT_DPAD_RIGHT)
			{
				p->state |= PSTATE_WEAPON_SWITCH;
			}
        }

		if(state->rtrig > 0.0f)
		{
            if(!(state->ltrig))
			    p->state |= PSTATE_FIRE1;
            
            //DCE_ControllerRumble(0, 64);
        }

        
       
		if(state->buttons & CONT_Y && !(state->buttons & CONT_A))  
		{
            vec3f_shift_xz(&p->lookAt, &p->position, move_speed * .75);
           
            p->state |= PSTATE_MOVE;
           
		    p->state |= PSTATE_WALK_FORWARD;
		    
        }
                
        if(state->buttons & CONT_A && !(state->buttons & CONT_Y)) 
		{ 
            vec3f_shift_xz(&p->lookAt, &p->position, -move_speed * .5);
            
			p->state |= PSTATE_MOVE;
            
            p->state |= PSTATE_WALK_BACKWARD;
        }
        		
        if(state->buttons & CONT_X) 
	    {            
	        vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                 p->position.x, p->position.y, p->position.z, RSHIFT);
		                 
		    vec3f_shift_xz(&p->lookAt, &p->position, -move_speed * .55f);
		   
	        vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                 p->position.x, p->position.y, p->position.z, FRSHIFT);
		                 
            p->state |= PSTATE_MOVE;
            
            p->state |= PSTATE_WALK_STRAFE_LEFT;
        }
           
		
        if(state->buttons & CONT_B) 
	    {
	        vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                  p->position.x, p->position.y, p->position.z, RSHIFT);              
            vec3f_shift_xz(&p->lookAt, &p->position, move_speed * .55f);
	        vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                  p->position.x, p->position.y, p->position.z, FRSHIFT);
            
			p->state |= PSTATE_MOVE;
            
            if(p->state & PSTATE_WALK_STRAFE_LEFT)
                p->state &= ~PSTATE_WALK_STRAFE_LEFT;
            else
                p->state |= PSTATE_WALK_STRAFE_RIGHT;
        }
        
        if(p->state & PSTATE_MOVE && !(p->lstate & PSTATE_MOVE))
            DCE_SfxPlay(PLAYER_WALK1 + rand() % 6);
        
		if(state->joyx > 0 || state->joyx < 0) 
		{
			float a = (float)(state->joyx / 126.0f); // Acceleration Before Processing (0.0f->1.0f)
			if(state->ltrig)
			{
			    a = DCE_LerpTriQuadratic(a);
			    a *= (DCE_XAXIS_MOVE_SPEED * 1);
			}
			else
			{
			    a = DCE_LerpQuadratic(a);
			    a *= DCE_XAXIS_MOVE_SPEED;
			}
								    
	        vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                  p->position.x, p->position.y, p->position.z,
                          a);
            
            if(!(p->state & PSTATE_MOVE))
            {
            	if(a < 0)
            	    p->state |= PSTATE_WALK_STRAFE_LEFT;
            	else
            	    p->state |= PSTATE_WALK_STRAFE_RIGHT;
			}
        }
                
		if(state->joyy > 0 || state->joyy < 0) 
            if(p->lookAt.y <= p->position.y + 90.0f && p->lookAt.y >= p->position.y - 90.0f)
            {
			    float a = (float)(state->joyy / 126.0f);
			    if(state->ltrig)
			    {
                    a = DCE_LerpTriQuadratic(a);
			        a *= (DCE_YAXIS_MOVE_SPEED * DCE_STEADY_MOVE_FACTOR);
				}			        
			    else
			    {
				    a = DCE_LerpBiQuadratic(a);
				    a *= DCE_YAXIS_MOVE_SPEED;
			    }
			       
				if(DCE_CONTROLLER_INVERT_YAXIS)
                    p->lookAt.y += a;
                else
                    p->lookAt.y -= a;
                //p->state |= PSTATE_MOVE;
            }  
        /*
        if(p->state & PSTATE_MOVE)
           p->last_moved = 0;
        else
           p->last_moved++;    
        */
        
        return state->buttons & CONT_START;
	}
          
	return 0;
}

int InputCallbackX()
{
     maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);    

     if (cont)
     {          
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);
         
        if(!state)
           return 0;

        if (state->buttons & CONT_START) 
	    {            
            if(state->buttons & CONT_DPAD_LEFT)
                return 1;         
            
            if(state->buttons & CONT_DPAD_UP)
                return 2;
            
            if(state->buttons & CONT_DPAD_RIGHT)
                return 3; 
				
			return 4;   
        }   
	 }
	 
	 return 0;
}

static cont_state_t lstate;

unsigned int DCE_CopyContState(cont_state_t * src, cont_state_t * dst, unsigned int param)
{
    *dst = *src;
    
    return param;
}

int DCE_MenuInputCallback()
{
     maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);    

     if (cont)
     {          
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);
         
        if(!state)
           return 0;

        if(lstate.buttons & CONT_DPAD_UP || lstate.joyy < 0) 
        {   
            if(!(state->buttons & CONT_DPAD_UP|| state->joyy < 0))
                return DCE_CopyContState(state, &lstate, CONT_DPAD_UP);
        }   
        if(lstate.buttons & CONT_DPAD_DOWN || lstate.joyy > 0) 
        {   
            if(!(state->buttons & CONT_DPAD_DOWN|| state->joyy > 0))
                return DCE_CopyContState(state, &lstate, CONT_DPAD_DOWN);
        }  
        if(lstate.buttons & CONT_DPAD_LEFT) 
        {   
            if(!(state->buttons & CONT_DPAD_LEFT))
                return DCE_CopyContState(state, &lstate, CONT_DPAD_LEFT);
        }   
        if(lstate.buttons & CONT_DPAD_RIGHT)    
        {
            if(!(state->buttons & CONT_DPAD_RIGHT))
                return DCE_CopyContState(state, &lstate, CONT_DPAD_RIGHT);
        }
        if(lstate.buttons & CONT_START)    
        {
            if(!(state->buttons & CONT_START))
                return DCE_CopyContState(state, &lstate, CONT_START);
        }
        if(lstate.buttons & CONT_A)    
        {
            if(!(state->buttons & CONT_A))
                return DCE_CopyContState(state, &lstate, CONT_A);
        }
        if(lstate.buttons & CONT_B)    
        {
            if(!(state->buttons & CONT_B))
                return DCE_CopyContState(state, &lstate, CONT_B);
        }        
        return DCE_CopyContState(state, &lstate, 0);
	 }
	 
	 return 0;
}

#define RANGLE 15.0f          // Rotation Angle
#define RSD 360.0f/RANGLE     // Rotated Side
static const float ANGLE = 9.0f;
#define ROTATION RADIAN*ANGLE

#define MOVE_SPEED 1.0f

int InputCallback2(DCE_Player * p)
{
     maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);    

     if (cont)
     {          
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);
         
        if(!state)
           return 0;
        
        p->state &= ~PSTATE_MOVE;
        
		if(state->buttons & CONT_DPAD_UP || state->buttons & CONT_DPAD2_UP) 
            if(p->lookAt.y <= p->position.y + 90.0f)
            {
                p->lookAt.y += 0.5f;
                p->state |= PSTATE_MOVE;
            }
		
        if(state->buttons & CONT_DPAD_DOWN || state->buttons & CONT_DPAD2_DOWN) 
            if(p->lookAt.y >= p->position.y - 90.0f)
            {
                p->lookAt.y -= 0.5f;
                p->state |= PSTATE_MOVE;
            }
            
		if(state->buttons & CONT_DPAD_LEFT || state->buttons & CONT_DPAD2_LEFT) 
		{
	        vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                 p->position.x, p->position.y, p->position.z, -ROTATION/2.0);
            p->state |= PSTATE_MOVE;
        }
        
		if(state->buttons & CONT_DPAD_RIGHT || state->buttons & CONT_DPAD2_RIGHT)  
		{
	       vec3f_rotr_xz( p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                  p->position.x, p->position.y, p->position.z, ROTATION/2.0);
            p->state |= PSTATE_MOVE;
        }
        
		if(state->buttons & CONT_Y)  
		{
           vec3f_shift(&p->lookAt, &p->position, MOVE_SPEED);
            p->state |= PSTATE_MOVE;
        }
                
        if(state->buttons & CONT_A) 
		{ 
           vec3f_shift_xz(&p->lookAt, &p->position, -MOVE_SPEED);
            p->state |= PSTATE_MOVE;
        }
        		
        if(state->buttons & CONT_X) 
	    {            
	       vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                 p->position.x, p->position.y, p->position.z, RSHIFT);
		   vec3f_shift_xz(&p->lookAt, &p->position, -MOVE_SPEED);
	       vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                 p->position.x, p->position.y, p->position.z, FRSHIFT);
            p->state |= PSTATE_MOVE;
        }
           
		
        if(state->buttons & CONT_B) 
	    {
	       vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                 p->position.x, p->position.y, p->position.z, RSHIFT);              
           vec3f_shift_xz(&p->lookAt, &p->position, MOVE_SPEED);
	       vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                 p->position.x, p->position.y, p->position.z, FRSHIFT);
            p->state |= PSTATE_MOVE;
        }
        
        if(p->state & PSTATE_MOVE)
           p->last_moved = 0;
        else
           p->last_moved++;    

		if(state->ltrig > 0.0f)
		{
			if(state->buttons & CONT_DPAD_UP)
				p->state &= ~PSTATE_DUCK;

			if(state->buttons & CONT_DPAD_DOWN)
				p->state |= PSTATE_DUCK;

			if(state->buttons & CONT_DPAD_LEFT)
				p->state &= ~PSTATE_LIGHT;

			if(state->buttons & CONT_DPAD_RIGHT)
				p->state |= PSTATE_LIGHT;
			
			p->state |= PSTATE_FIRE1;
		}
		if(state->rtrig > 0.0f)
			p->state |= PSTATE_FIRE2;
	 }
	 return 0;
}


int DCE_InputCallbackCam(DCE_Player * p, DCE_Camera * c)
{
    DCE_InitInput();
    
    maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);    
    
    unsigned char cam_moved = 0;
    
    if (cont)
    {          
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);
		 
        if(!state)
           return 0;
        
        float move_speed = p->state & PSTATE_DUCK ? DCE_MOVE_SPEED / 2.0f : DCE_MOVE_SPEED;
        
        p->lstate = p->state;
        
        p->state &= ~PSTATE_MOVE;
        p->state &= ~PSTATE_FIRE2;
        p->state &= ~PSTATE_WALK_FORWARD;
        p->state &= ~PSTATE_WALK_BACKWARD;
        p->state &= ~PSTATE_WALK_STRAFE_LEFT;
        p->state &= ~PSTATE_WALK_STRAFE_RIGHT;
        
        if(state->buttons & CONT_START)
            return DCE_INPUT_EXECUTE;
            
		if(state->rtrig > 0.0f)
		{
            return DCE_INPUT_FIRE;
        }
        
		if(state->joyx > 0 || state->joyx < 0) 
		{
			cam_moved = 1;
			
			float a = (float)(state->joyx / 126.0f); // Acceleration Before Processing (0.0f->1.0f)
			
			a = DCE_LerpTriQuadratic(a);
			a *= (DCE_XAXIS_MOVE_SPEED * DCE_STEADY_MOVE_FACTOR);
			
			if(fabs(c->rx + a) < c->mx)								    
	        {
			    c->rx += a;
			    
			    printf("Rx: %f / %f\n", c->rx, c->mx);
			    
				vec3f_rotr_xz(c->lookAt.x, c->lookAt.y, c->lookAt.z,
		                  c->pos.x, c->pos.y, c->pos.z, a); 
			}
        }
     
		if(state->joyy > 0 || state->joyy < 0) 
        {
            cam_moved = 1;
		        
			float a = -(float)(state->joyy / 126.0f) * 8.0f;

			if(fabs(c->ry + a) < c->my)								    
	        {
				c->lookAt.y += a;
				c->ry += a;		
			}
        }  
        
		if(cam_moved)
		    DCE_SfxPlay(CAMERA_01);

        return 0;
	}
          
	return 0;
}

unsigned char DCE_InputCallbackMenu()
{
     maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);    

     if (cont)
     {          
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);
         
        if(!state)
           return 0;
           
        if(state->rtrig > 0.0f)
           return 1;
           
        if(state->buttons & CONT_DPAD_LEFT)
            return 2;
            
        if(state->buttons & CONT_DPAD_RIGHT)
            return 3;
  
    }
    
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////


static float speed = 0;

#define DCE_MAX_VEHICLE_SPEED 320.0f
#define DCE_MIN_VEHICLE_SPEED -60.0f

int DCE_InputCallbackVehicle(DCE_Player * p)
{
    DCE_InitInput();
    
    maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);    

    if (cont)
    {          
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);
         
        if(!state)
           return 0;

        if(state->buttons & CONT_B)
        {
            if(speed > 0)
			    speed -= 6;
			    
			if(speed < 0)
			    speed = 0;		
        }
 		else if(state->ltrig > 0.0f)
		{
            if(speed > DCE_MIN_VEHICLE_SPEED)
			    speed -= state->ltrig * .01;
        }       
		else if(state->rtrig > 0.0f)
		{
            if(speed < DCE_MAX_VEHICLE_SPEED)
			    speed += state->rtrig * .02;
        }
        else
        {
           if(speed > 0)
		       speed -= 4;
		   if(speed < 0)
		        speed = 0;
		}

        if(speed)
            vec3f_shift_xz(&p->lookAt, &p->position, speed / 100);

        if(state->buttons & CONT_DPAD_LEFT)
        {
        	vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                  p->position.x, p->position.y, p->position.z, max(-0.00035f * speed, -0.7f)); 
		}

        if(state->buttons & CONT_DPAD_RIGHT)
        {
        	vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                  p->position.x, p->position.y, p->position.z, min(0.00035f * speed, 0.7f)); 
		}
		
		if(state->joyx > 0 || state->joyx < 0) 
		{

        }
        
        if(state->buttons & CONT_START)
            return 1;
	}
          
	return 0;
}


int DCE_InputCallbackWaitHack(DCE_Player * p)
{
    DCE_InitInput();
    
    maple_device_t *cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);    

    if (cont)
    {          
        cont_state_t *state = (cont_state_t *)maple_dev_status(cont);
         
        if(!state)
           return 0;
        
        float move_speed = p->state & PSTATE_DUCK ? DCE_MOVE_SPEED / 2.0f : DCE_MOVE_SPEED;
        
        p->lstate = p->state;
        
        p->state &= ~PSTATE_MOVE;
        p->state &= ~PSTATE_FIRE2;
        p->state &= ~PSTATE_WALK_FORWARD;
        p->state &= ~PSTATE_WALK_BACKWARD;
        p->state &= ~PSTATE_WALK_STRAFE_LEFT;
        p->state &= ~PSTATE_WALK_STRAFE_RIGHT;
		        
        if(state->ltrig)
        {
			if(state->buttons & CONT_DPAD_LEFT)
			{
			    p->light_on = 0;
    			p->perspective = DCE_PERSPECTIVE_STATIC;
    			
    			return 1;
			}
		
        }
        
        if(p->state & PSTATE_MOVE && !(p->lstate & PSTATE_MOVE))
            DCE_SfxPlay(PLAYER_WALK1 + rand() % 6);
        
		if(state->joyx > 0 || state->joyx < 0) 
		{
			float a = (float)(state->joyx / 126.0f); // Acceleration Before Processing (0.0f->1.0f)
			if(state->ltrig)
			{
			    a = DCE_LerpTriQuadratic(a);
			    a *= (DCE_XAXIS_MOVE_SPEED * DCE_STEADY_MOVE_FACTOR);
			}
			else
			{
			    a = DCE_LerpQuadratic(a);
			    a *= DCE_XAXIS_MOVE_SPEED;
			}
								    
	        vec3f_rotr_xz(p->lookAt.x, p->lookAt.y, p->lookAt.z,
		                  p->position.x, p->position.y, p->position.z,
                          a);
            
            if(!(p->state & PSTATE_MOVE))
            {
            	if(a < 0)
            	    p->state |= PSTATE_WALK_STRAFE_LEFT;
            	else
            	    p->state |= PSTATE_WALK_STRAFE_RIGHT;
			}
        }
                
		if(state->joyy > 0 || state->joyy < 0) 
            if(p->lookAt.y <= p->position.y + 90.0f && p->lookAt.y >= p->position.y - 90.0f)
            {
			    float a = (float)(state->joyy / 126.0f);
			    if(state->ltrig)
			    {
                    a = DCE_LerpTriQuadratic(a);
			        a *= (DCE_YAXIS_MOVE_SPEED * DCE_STEADY_MOVE_FACTOR);
				}			        
			    else
			    {
				    a = DCE_LerpBiQuadratic(a);
				    a *= DCE_YAXIS_MOVE_SPEED;
			    }
			       
				if(DCE_CONTROLLER_INVERT_YAXIS)
                    p->lookAt.y += a;
                else
                    p->lookAt.y -= a;
                //p->state |= PSTATE_MOVE;
            }  
 
	}
          
	return 0;
}
