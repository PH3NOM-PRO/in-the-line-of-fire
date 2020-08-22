
#include "dce.h"

#include <string.h>

#define DCE_MAX_CBO_OBJECTS 128

typedef struct
{
	DCE_CollisionQuadTreeNode cbo;
	char segment[32];
	unsigned char active[3];
	unsigned char instances;
	DCE_RenderObjectInstance * instance;
} DCE_CollisionObject;

static DCE_CollisionObject DCE_CBO[DCE_MAX_CBO_OBJECTS];
static unsigned char DCE_CBOS = 0;

void DCE_TriggerResetCBO(unsigned char index)
{
	unsigned char i;
	for(i = 0; i < DCE_CBOS; i++)
		DCE_CBO[i].active[index] = 0;	
}

void DCE_TriggerInitCBO(char * segment, unsigned char index)
{
	unsigned char i;
	for(i = 0; i < DCE_CBOS; i++)
		if(DCE_StringEqualsIgnoreCase(segment, DCE_CBO[i].segment))
		    DCE_CBO[i].active[index] = 1;
}

void DCE_TriggerCBO(char * segment, unsigned char active, unsigned char index)
{
	unsigned char i;
	for(i = 0; i < DCE_CBOS; i++)
		if(DCE_StringEqualsIgnoreCase(segment, DCE_CBO[i].segment))
		    DCE_CBO[i].active[index] = active;
}

extern void DCE_CheckCollisionDOOR(vec3f_t * O, vec3f_t * D, float * d);
void DCE_CheckCollisionDOOROpen(vec3f_t * O, vec3f_t * D, float * d);

static unsigned char DCE_CBO_INDEX = 0;

void DCE_SetCBOIndex(unsigned char index)
{
	DCE_CBO_INDEX = index;
}

unsigned char DCE_CheckCollisionNoDoorsCBO(vec3f_t * O, vec3f_t * D, float * d)
{
	vec3f_t P;
    unsigned char i, j;
    
	*d = DCE_MAX_RAY_DISTANCE; // Max Collision Distance
	   
    for(i = 0; i < DCE_CBOS; i++)
    {
        if(DCE_CBO[i].active[DCE_CBO_INDEX])
        {
		    if(DCE_CBO[i].instances)
		    {
			    for(j = 0; j < DCE_CBO[i].instances; j++)
                {
        	        vec3f_sub(&DCE_CBO[i].instance[j].trans , O, &P); 
        	
                    DCE_CollisionModelTraceRay(&DCE_CBO[i].cbo, &P, D, d);
		        }
			}
		    else
		        DCE_CollisionModelTraceRay(&DCE_CBO[i].cbo, O, D, d);
		}
	}
	
	DCE_CheckCollisionDOOROpen(O, D, d);
			    
    if(*d != DCE_MAX_RAY_DISTANCE)
        return 1;
    
	return 0;
}

unsigned char DCE_CheckCollisionCBO(vec3f_t * O, vec3f_t * D, float * d)
{
	vec3f_t P;
    unsigned char i, j;
    
	*d = DCE_MAX_RAY_DISTANCE; // Max Collision Distance
	   
    for(i = 0; i < DCE_CBOS; i++)
    {
        if(DCE_CBO[i].active[DCE_CBO_INDEX])
        {
		    if(DCE_CBO[i].instances)
		    {
			    for(j = 0; j < DCE_CBO[i].instances; j++)
                {
        	        vec3f_sub(&DCE_CBO[i].instance[j].trans , O, &P); 
        	
                    DCE_CollisionModelTraceRay(&DCE_CBO[i].cbo, &P, D, d);
		        }
			}
		    else
		        DCE_CollisionModelTraceRay(&DCE_CBO[i].cbo, O, D, d);
		}
	}
	
	DCE_CheckCollisionDOOR(O, D, d);
			    
    if(*d != DCE_MAX_RAY_DISTANCE)
        return 1;
    
	return 0;
}

unsigned char DCE_CheckGravityCollisionCBO(vec3f_t * O, float * d)
{
	vec3f_t P;
    unsigned char i, j;
    
	*d = DCE_MAX_RAY_DISTANCE; // Max Collision Distance
	   
    for(i = 0; i < DCE_CBOS; i++)
    {
        if(DCE_CBO[i].active[DCE_CBO_INDEX])
        {
		    if(DCE_CBO[i].instances)
		    {
			    for(j = 0; j < DCE_CBO[i].instances; j++)
                {
        	        vec3f_sub(&DCE_CBO[i].instance[j].trans , O, &P); 
        	
                    DCE_CollisionModelTraceGravityRay(&DCE_CBO[i].cbo, &P, d);
		        }
			}
			else
			    DCE_CollisionModelTraceGravityRay(&DCE_CBO[i].cbo, O, d);
		}
	}
			    
    if(*d != DCE_MAX_RAY_DISTANCE)
        return 1;
    
	return 0;
}

unsigned char DCE_ReadCBO(char * fname, DCE_CollisionQuadTreeNode * cmesh, unsigned char debug)
{
	FILE * f = NULL;
	f = fopen(fname, "rb");
    if(f == NULL)
    {
    	while(debug)
		    printf("Error Opening CBO File %s\n", fname);

		return 0;
	}
    
	fread(cmesh, 1, sizeof(DCE_CollisionQuadTreeNode) - (4 * 6), f);

    printf("\nCollision Mesh Triangles: %i\n", cmesh->triangles);
    printf("Collision Mesh Width: %f\n", (double)cmesh->half_width * 2);    
    printf("Collision Mesh Depth: %f\n\n", (double)cmesh->half_depth * 2);  

	cmesh->tris = malloc(sizeof(DCE_CollisionTriangle) * cmesh->triangles);
    
	fseek(f, sizeof(DCE_CollisionQuadTreeNode) - (4 * 6), SEEK_SET);
		 
	fread(cmesh->tris, sizeof(DCE_CollisionTriangle), cmesh->triangles, f);
	
	fclose(f);
	
	return 1;
}

unsigned char DCE_LoadCBOInstance(char * fname, unsigned char active, char * segment,
                                  DCE_RenderObjectInstance * instance, unsigned char instances)
{
    char fn[256];
    
    DCE_StringCopyNewExt(fn, fname, ".cbo");     
    
    if(!DCE_ReadCBO(fn, &DCE_CBO[DCE_CBOS].cbo, 0))
        return 0;
	
	strcpy(DCE_CBO[DCE_CBOS].segment, segment);
	
	DCE_CBO[DCE_CBOS].active[0] = active;
	
    DCE_CBO[DCE_CBOS].instance = instance;
    
	DCE_CBO[DCE_CBOS].instances = instances;
		
	++DCE_CBOS;
	
	return 1;
}

unsigned char DCE_LoadCBOInternal(char * fname, unsigned char active, char * segment)
{
	DCE_ReadCBO(fname, &DCE_CBO[DCE_CBOS].cbo, 1);
	
	strcpy(DCE_CBO[DCE_CBOS].segment, segment);
	
	DCE_CBO[DCE_CBOS].active[0] = active;
	
	DCE_CBO[DCE_CBOS].instances = 0;
	
	DCE_CBO[DCE_CBOS].instance = NULL;
		
	printf("CBO: %s || %i\n", DCE_CBO[DCE_CBOS].segment, DCE_CBO[DCE_CBOS].active);
		
	++DCE_CBOS;
	
	return 1;
}

void DCE_LoadCBOCluster(char * fname)
{
	FILE * f = NULL;      // Input TXT File
	
	char segment[32];
	char active;
	int  a;
	
	char buf[96]; // Read Buf
	char id[64];  // Tmp Buf
	char fn[64];  // File Name

	printf("\n\n====================================\n\n");
	
	f = fopen(fname, "rb");  // Open Segement Descriptor File
	if(f == NULL)
	{
		printf("File Error %s\n", fname);
		return;
	}
	
	fgets(buf, 96, f);                        // Read First Line
	
	sscanf(buf, "%s %s %i", id, segment, &a); // Parse Segment Name and Active State
	
	active = a & 0xFF;
	
	while(fgets(buf, 64, f))                  // Scan to Collision Objects
		if(buf[0] == 'C')
            break;

    if(buf[0] != 'C') // NO COLLISION Found
    {
        fclose(f);    
		return;       
	}
    
    fgets(buf, 64, f);//Skip { brace start

	while(fgets(buf, 64, f)) // Read Next Line
	{
		if(buf[0] == '}')    // Check for end of segment
            break;
	    
		sscanf(buf + 4, "%s", fn);
        
        DCE_LoadCBOInternal(fn, active, segment);
    }
	
	fclose(f);
}

