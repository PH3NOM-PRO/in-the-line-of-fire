
#include "dce.h"

#include <string.h>

typedef struct
{
	DCE_RenderObject * rbo_door;
	DCE_RenderObject * rbo_assets;
	
	DCE_CollisionQuadTreeNode * cbo_closed;
	DCE_CollisionQuadTreeNode * cbo_open;
	
	DCE_RenderObjectInstance instance;
	
	char             segment[32];
	unsigned char    active;
	unsigned char    locked;
	
} DCE_Door;

static DCE_Door                  DCE_DOOR[DCE_MAX_DOORS];
static unsigned char             DCE_DOORS = 0;
static DCE_RenderObject          DCE_DOOR_MODEL[6];
static DCE_CollisionQuadTreeNode DCE_DOOR_COLLISION[6];

void DCE_TriggerDOOR(char * segment, unsigned char active)
{
	unsigned char i;
	for(i = 0; i < DCE_DOORS; i++)
		if(DCE_StringEqualsIgnoreCase(segment, DCE_DOOR[i].segment))
		    DCE_DOOR[i].active = active;
}

void DCE_TriggerResetDOOR()
{
	unsigned char i;
	for(i = 0; i < DCE_DOORS; i++)
	{
		if(DCE_StringEqualsIgnoreCase("LOBBY", DCE_DOOR[i].segment))
		
		    DCE_DOOR[i].active = 1;
		else
		    DCE_DOOR[i].active = 0;
        
        DCE_DOOR[i].locked = 0;
        DCE_DOOR[i].instance.rot = 0;
	}
}

void DCE_TriggerOpenDOOR()
{
	unsigned char i;
	for(i = 0; i < DCE_DOORS; i++)
	{
		DCE_DOOR[i].active = 1;
		DCE_DOOR[i].locked = 0;
		DCE_DOOR[i].instance.rot = 90;
	}
}

void DCE_DoorUnlock(vec3f_t * O, vec3f_t * D)
{
	
}

void DCE_CheckCollisionDOOR(vec3f_t * O, vec3f_t * D, float * d)
{
	vec3f_t P, L, Z = { 0, 0, 0};
    unsigned char i, j;

    for(i = 0; i < DCE_DOORS; i++)
    {
        if(DCE_DOOR[i].active)
        {
        	//vec3f_copy(D, &L);

			//vec3f_rotr_xz(L.x, L.y, L.z, Z.x, Z.y, Z.z, -DEG2RAD * DCE_DOOR[i].instance.rot);			
            
			vec3f_sub(&DCE_DOOR[i].instance.trans, O, &P);  // Translate Ray Origin By Inverse of Instance Position	
			
		    if(DCE_DOOR[i].locked || DCE_DOOR[i].instance.rot < 90.0f)
    			DCE_CollisionModelTraceRay(DCE_DOOR[i].cbo_closed, &P, D, d);
		    else
    			DCE_CollisionModelTraceRay(DCE_DOOR[i].cbo_open, &P, D, d);
		}
	}
}

void DCE_CheckCollisionDOOROpen(vec3f_t * O, vec3f_t * D, float * d)
{
	vec3f_t P, L, Z = { 0, 0, 0};
    unsigned char i, j;

    for(i = 0; i < DCE_DOORS; i++)
    {
        if(DCE_DOOR[i].active && DCE_DOOR[i].instance.rot >= 90.0f)
        {
        	//vec3f_copy(D, &L);

			//vec3f_rotr_xz(L.x, L.y, L.z, Z.x, Z.y, Z.z, -DEG2RAD * DCE_DOOR[i].instance.rot);			
            
			vec3f_sub(&DCE_DOOR[i].instance.trans, O, &P);  // Translate Ray Origin By Inverse of Instance Position	
			
		    if(DCE_DOOR[i].locked || DCE_DOOR[i].instance.rot < 90.0f)
    			DCE_CollisionModelTraceRay(DCE_DOOR[i].cbo_closed, &P, D, d);
		    else
    			DCE_CollisionModelTraceRay(DCE_DOOR[i].cbo_open, &P, D, d);
		}
	}
}

void DCE_TriggerRenderOpenDoor(vec3f_t * O)
{
	float d;
	unsigned char i;
	
	for(i = 0; i < DCE_DOORS; i++)
	{
		if(DCE_DOOR[i].active)
		{
		    vec3f_distance(O->x, O->y, O->z,
		               DCE_DOOR[i].instance.trans.x, DCE_DOOR[i].instance.trans.y, DCE_DOOR[i].instance.trans.z, d);
		
		    if(d < DCE_DOOR_COLLISION_DISTANCE * 1.1)
		        if(!DCE_DOOR[i].locked)
			        if(DCE_DOOR[i].instance.rot < 90.0f)
			            DCE_DOOR[i].instance.rot += DCE_DOOR_ROT_SPEED;
	    }
    }
}


void DCE_RenderDoors()
{
	unsigned char i;
	for(i = 0; i < DCE_DOORS; i++)
	{
		if(DCE_DOOR[i].active)
		{
			DCE_MatrixLoadProjection();
        
            DCE_MatrixApplyPlayer(0);
            
            mat_translate(DCE_DOOR[i].instance.trans.x, DCE_DOOR[i].instance.trans.y, DCE_DOOR[i].instance.trans.z);
			
			mat_rotate_y(DCE_DOOR[i].instance.rot * -DEG2RAD);
			
			DCE_RenderRBO(DCE_DOOR[i].rbo_door);   
			
			DCE_RenderRBO(DCE_DOOR[i].rbo_assets);         
		}
	}
}

unsigned char DCE_DoorStringToIndex(char * str)
{
	switch(str[DCE_StringFindCharFirst(str, '.') - 1])
	{
		case 'a':
			return 0;
			
		case 'b':
			return 2;

		case 'c':
			return 4;			
	}
	
	return 0;
}

void DCE_LoadDoors()
{
	DCE_LoadRBO("/cd/models/doors/Door.rbo", &DCE_DOOR_MODEL[0], "wall_floor_window_door_d.pvr");
	DCE_LoadRBO("/cd/models/doors/Door_Lock_assets.rbo", &DCE_DOOR_MODEL[1], "assets.pvr");

	DCE_LoadRBO("/cd/models/doors/DoorB.rbo", &DCE_DOOR_MODEL[2], "wall_floor_window_door_d.pvr");
	DCE_LoadRBO("/cd/models/doors/Door_LockB_assets.rbo", &DCE_DOOR_MODEL[3], "assets.pvr");
	
	DCE_LoadRBO("/cd/models/doors/DoorC.rbo", &DCE_DOOR_MODEL[4], "wall_floor_window_door_d.pvr");
	DCE_LoadRBO("/cd/models/doors/Door_LockC_assets.rbo", &DCE_DOOR_MODEL[5], "assets.pvr");
	
	DCE_ReadCBO("/cd/models/doors/Door.cbo",  &DCE_DOOR_COLLISION[0], 1);
	DCE_ReadCBO("/cd/models/doors/DoorB.cbo", &DCE_DOOR_COLLISION[1], 1);
	DCE_ReadCBO("/cd/models/doors/DoorB.cbo", &DCE_DOOR_COLLISION[2], 1);
	DCE_ReadCBO("/cd/models/doors/DoorC.cbo", &DCE_DOOR_COLLISION[3], 1);		
	DCE_ReadCBO("/cd/models/doors/DoorC.cbo", &DCE_DOOR_COLLISION[4], 1);	
	DCE_ReadCBO("/cd/models/doors/Door.cbo",  &DCE_DOOR_COLLISION[5], 1);
}

void DCE_LoadDoor(char * fname, char * segment, char active)
{
	char buf[64];
	char id[32];
	
	FILE * f = fopen(fname, "rb");
	if(f == NULL)
	    while(1) printf("Door File ERROR: %s\n", fname);
	
	//printf("DOOR: %s || SEG: %s || Index: %i\n", fname, segment, DCE_DoorStringToIndex(fname));
	
	while(fgets(buf, 64, f)) // Read TXT File Line
	{
		if(buf[0] == '}')
            ++DCE_DOORS;
        
        if(buf[0] == '{')
        {
        	DCE_DOOR[DCE_DOORS].active = active;
        	
        	strcpy(DCE_DOOR[DCE_DOORS].segment, segment);
        	
        	DCE_DOOR[DCE_DOORS].rbo_door =   &DCE_DOOR_MODEL[DCE_DoorStringToIndex(fname)];
        	DCE_DOOR[DCE_DOORS].rbo_assets = &DCE_DOOR_MODEL[DCE_DoorStringToIndex(fname) + 1];

        	DCE_DOOR[DCE_DOORS].cbo_closed = &DCE_DOOR_COLLISION[DCE_DoorStringToIndex(fname)];
        	DCE_DOOR[DCE_DOORS].cbo_open =   &DCE_DOOR_COLLISION[DCE_DoorStringToIndex(fname) + 1];
        	
        	DCE_DOOR[DCE_DOORS].locked = 0;
		}
        
        if(buf[4] == 't' && buf[5] == 'r' && buf[6] == 'a')
        {
        	sscanf(buf, "%s %c %c %f %c %f %c %f", id, id, id, &DCE_DOOR[DCE_DOORS].instance.trans.x, id,
		            &DCE_DOOR[DCE_DOORS].instance.trans.z, id, &DCE_DOOR[DCE_DOORS].instance.trans.y); 
    
		    DCE_DOOR[DCE_DOORS].instance.trans.x *= -10; // Scale and Swizzle for Up Vector Transform
		    DCE_DOOR[DCE_DOORS].instance.trans.y *= 10;
		    DCE_DOOR[DCE_DOORS].instance.trans.z *= 10;
		}    

        /* Parse Rotation Float */
        else if(buf[4] == 'r' && buf[5] == 'o' && buf[6] == 't')
        {
        	//sscanf(buf, "%s %c %f", id, id, &DCE_DOOR[DCE_DOORS].instance.rot; 
        	
        	//DCE_DOOR[DCE_DOORS].instance.rot *= -DEG2RAD; // Convert Degrees to Radians and Invert for UP Vector Transform
        	
        	//DCE_DOOR[DCE_DOORS].instance.rot = 90.0f;
		} 
		
		memset(buf, 0, 64); // Clear the buffer for next line		
	}
	
	fclose(f);
}

void DCE_LoadDoorCluster(char * fname)
{
	char buf[64];
	char id[32];
	char segment[32];
	int active;
	
	FILE * f = fopen(fname, "rb");
	if(f == NULL)
	    while(1) printf("Door File ERROR: %s\n", fname);
	
	fgets(buf, 64, f);       // Read First Line
	
	sscanf(buf, "%s %s %i", id, segment, &active);  // Parse Segment Name and Active State
	
	while(fgets(buf, 64, f)) // Scan to Door Objects
		if(buf[0] == 'D')
            break;	

    if(buf[0] != 'D') // NO DOOR Found
    {
        fclose(f);    
		return;       
	}

	while(fgets(buf, 64, f)) // Scan past open brace
		if(buf[0] == '{')
            break;
    
	while(fgets(buf, 64, f)) // Scan past open brace
	{
		if(buf[0] == '}')
            break;  
		
		uint16 len = DCE_StringFindCharFirst(buf, '.');
	
	    buf[len + 4] = '\0'; // Clear trailing newline character read in
		
		DCE_LoadDoor(buf + 4, segment, active & 0xFF);  
	}
	
	fclose(f);
}
