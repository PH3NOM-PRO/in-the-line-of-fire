
#include "dce.h"

static unsigned RF = 0;

void DCE_PlayerFireSequence(DCE_Player * p, DCE_Camera * c, DCE_Enemy * e)
{
	unsigned char i;
	
	p->state |= PSTATE_FIRE1;
	
	for(i = 0; i < p->model.body_frame_count[FIRE]; i++)
	{
    	DCE_EnemyRoutineCallback(p, DCE_ENEMY_EXECUTE_SPEED);
    	
    	DCE_PlayerAnimationStartFrame(p);   
    	
    	DCE_RenderCallback(p, c, DCE_ENEMY_EXECUTE_SPEED);
    	
    	DCE_PlayerAnimationEndFrame(p, RF++);   
		
		if(i == 5)
		{
			//DCE_SfxPlay(WEAPON_FIRE);
			
			e->state |= PSTATE_DEAD;
			e->state_frame = 0;
		}		
	}	
	
	p->state &= ~PSTATE_FIRE1;
}

void DCE_RunExecutionSequence(DCE_Player * p, DCE_Camera * c)
{
	vec3f_t L, O, S;       // Last position, Offset, and Start vectors
	float d, l = 0, lf;
	
	vec3f_distance(p->position.x, p->position.y, p->position.z, c->vantage_pos.x, c->vantage_pos.y, c->vantage_pos.z, d);
	
	lf = 4.1 / d;

    vec3f_copy(&p->position, &S);
    
    p->state |= PSTATE_WALK_FORWARD;
    
    while(l < 1.0f)
    {
    	l += lf;
    	
    	vec3f_copy(&p->position, &L);
    	
    	vec3f_lerp(&S, &c->vantage_pos, &p->position, l);
    	
    	vec3f_sub(&L, &p->position, &O);
    	
    	vec3f_add(&O, &p->lookAt, &p->lookAt);
    	
    	vec3f_copy(&p->position, &p->shadow_position);
    	
    	p->shadow_position.y -= p->height - 2.0f;
    	
    	DCE_EnemyRoutineCallback(p, DCE_ENEMY_EXECUTE_SPEED);
    	
    	DCE_PlayerAnimationStartFrame(p);   
    	
    	DCE_RenderCallback(p, c, DCE_ENEMY_EXECUTE_SPEED);
    	
    	DCE_PlayerAnimationEndFrame(p, RF++);   
	}
	
	p->state &= ~PSTATE_WALK_FORWARD;
	
	unsigned char i;
	for(i = 0; i < DCE_MAX_ENEMIES; i++)
	{
		if(DCE_EnemyPointer(i)->active)
		    if(DCE_EnemyPointer(i)->marked)
		    {
		    	DCE_PlayerRotateFacingEnemy(p, c, DCE_EnemyPointer(i));

		    	DCE_PlayerFireSequence(p, c, DCE_EnemyPointer(i));
			}
	}
}

