
#include "dce.h"

#include <string.h>

#define DCE_MAX_RBO_OBJECTS 128

static DCE_RenderCluster DCE_RENDER_CLUSTER[DCE_MAX_RBO_OBJECTS];
static unsigned char RBO_OBJECTS = 0;

static matrix4f RBO_MATRIX ALIGN32;

void DCE_TriggerResetRBO(unsigned char index)
{
	unsigned char i;
	for(i = 0; i < RBO_OBJECTS; i++)
		DCE_RENDER_CLUSTER[i].active[index] = 0;
}

void DCE_TriggerInitRBO(char * segment, unsigned char index)
{
	unsigned char i;
	for(i = 0; i < RBO_OBJECTS; i++)
		if(DCE_StringEqualsIgnoreCase(segment, DCE_RENDER_CLUSTER[i].segment))
		    DCE_RENDER_CLUSTER[i].active[index] = 1;
}

void DCE_TriggerRBO(char * segment, unsigned char active, unsigned char index)
{
	unsigned char i;
	for(i = 0; i < RBO_OBJECTS; i++)
		if(DCE_StringEqualsIgnoreCase(segment, DCE_RENDER_CLUSTER[i].segment))
		    DCE_RENDER_CLUSTER[i].active[index] = active;
}

void DCE_RenderSetTransRBO(float x, float y, float z)
{
/*
	rboi[RBOI_OBJECTS - 1].trans.x = x;
	rboi[RBOI_OBJECTS - 1].trans.y = y;
	rboi[RBOI_OBJECTS - 1].trans.z = z;
*/
}

void DCE_RenderSetRotRBO(float r)
{
/*
	rboi[RBOI_OBJECTS - 1].rot = r;
*/
}

void DCE_LerpRBO(DCE_SimpleVertex * a, DCE_SimpleVertex * b, DCE_SimpleVertex * c, float l, unsigned short int count)
{
	while(count--)
	{
	    vec3f_lerp(&a->x, &b->x, &c->x, l);
	    ++a; ++b; ++c;
	}
}

extern void DCE_PrintLoadStatus(char * string);
static char str[96];

static unsigned int RBO_VERTEX_COLOR = 0xFFD0D0D0;

void DCE_RBOSetVertexColor(unsigned int argb)
{
	RBO_VERTEX_COLOR = argb;
}

inline void DCE_RenderRBO(DCE_RenderObject * rbo)
{
	DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, rbo->texID, PVR_TXRENV_REPLACE);
		
	DCE_RenderTransformAndClipTriangles(&rbo->vert->pos.x, sizeof(DCE_SimpleVertex),
                                        &rbo->vert->texcoord.u, sizeof(DCE_SimpleVertex),
                                        //&RBO_VERTEX_COLOR, 0,
                                        &rbo->vert->color, sizeof(DCE_SimpleVertex),
                                        rbo->verts);	
}

void DCE_RenderRBOShadow(DCE_RenderObject * rbo)
{
	DCE_RenderCompileAndSubmitHeader(PVR_LIST_TR_POLY);
	
	unsigned int c = 0x40101010;
		
	DCE_RenderTransformAndClipTriangles(&rbo->vert->pos.x, sizeof(DCE_SimpleVertex),
                                        &rbo->vert->texcoord.u, sizeof(DCE_SimpleVertex),
                                        &c, 0,
                                        rbo->verts);	
}

void DCE_RenderCallbackRBO(unsigned char mode, unsigned char index)
{
	unsigned char src = DCE_RenderGetBlendSrc();
	unsigned char dst = DCE_RenderGetBlendDst();
	
	DCE_MatrixLoadProjection();
        
    DCE_MatrixApplyPlayer(0);
    
	mat_store(&RBO_MATRIX);

	DCE_RenderEnableCull(1);
	
	DCE_RenderSetCullFace(DCE_PVR_CULL_BACK);
	
	DCE_RenderSetCullFunc(DCE_PVR_CULL_CCW);
	
	unsigned char i, j;
	for(i = 0; i < RBO_OBJECTS; i++)
	    if((DCE_RENDER_CLUSTER[i].flags & (1<<1) | DCE_RENDER_CLUSTER[i].flags & (1<<0)) == mode
		   && DCE_RENDER_CLUSTER[i].active[index])
	    {		    
			if(DCE_RENDER_CLUSTER[i].instances > 0)
		    {
		    	if(mode == DCE_RBO_TR)
		    	{
		    		DCE_RenderSetBlendSrc(DCE_RENDER_CLUSTER[i].flags >> 5);
	                DCE_RenderSetBlendDst(DCE_RENDER_CLUSTER[i].flags >> 2);
				}
				
				for(j = 0; j < DCE_RENDER_CLUSTER[i].instances; j++)
		    	{
				    mat_load(&RBO_MATRIX);
				    
					mat_translate(DCE_RENDER_CLUSTER[i].instance[j].trans.x,
					              DCE_RENDER_CLUSTER[i].instance[j].trans.y,
								  DCE_RENDER_CLUSTER[i].instance[j].trans.z);
					
					if(DCE_RENDER_CLUSTER[i].instance[j].rot != 0.0f)
					    mat_rotate_y(DCE_RENDER_CLUSTER[i].instance[j].rot);
				    
				    DCE_RenderRBO(&DCE_RENDER_CLUSTER[i].rbo);
				}
			}
			else
			{
                mat_load(&RBO_MATRIX);
 
 		    	if(mode == DCE_RBO_TR)
		    	{
		    		DCE_RenderSetBlendSrc(DCE_RENDER_CLUSTER[i].flags >> 5);
	                DCE_RenderSetBlendDst(DCE_RENDER_CLUSTER[i].flags >> 2);
				}
                
	            DCE_RenderRBO(&DCE_RENDER_CLUSTER[i].rbo);
			}
        }
	
	DCE_RenderEnableCull(0);
	
	DCE_RenderSetBlendSrc(src);
	DCE_RenderSetBlendDst(dst);
}

void DCE_RenderCallbackRBOShadows(unsigned char index)
{
	DCE_MatrixLoadProjection();
        
    DCE_MatrixApplyPlayer(0);
    
	mat_store(&RBO_MATRIX);

	DCE_RenderEnableCull(1);
	
	DCE_RenderSetCullFace(DCE_PVR_CULL_BACK);
	
	DCE_RenderSetCullFunc(DCE_PVR_CULL_CCW);
	
	unsigned char i, j;
	for(i = 0; i < RBO_OBJECTS; i++)
	    if(DCE_RENDER_CLUSTER[i].shadow && DCE_RENDER_CLUSTER[i].active[index])
	    {		    
			if(DCE_RENDER_CLUSTER[i].instances > 0)
		    {
		    	for(j = 0; j < DCE_RENDER_CLUSTER[i].instances; j++)
		    	{
				    mat_load(&RBO_MATRIX);
				    
					mat_translate(DCE_RENDER_CLUSTER[i].instance[j].trans.x,
					              DCE_RENDER_CLUSTER[i].instance[j].trans.y + 0.1f,
								  DCE_RENDER_CLUSTER[i].instance[j].trans.z);
										
					if(DCE_RENDER_CLUSTER[i].instance[j].rot != 0.0f)
					    mat_rotate_y(DCE_RENDER_CLUSTER[i].instance[j].rot);
				    
                    mat_scale(1.0f, 0.0f, 1.0f);
                    
				    DCE_RenderRBOShadow(&DCE_RENDER_CLUSTER[i].rbo);
				}
			}
			else
			{
                mat_load(&RBO_MATRIX);

	            DCE_RenderRBOShadow(&DCE_RENDER_CLUSTER[i].rbo);
			}
        }
	
	DCE_RenderEnableCull(0);
}

static char string[128];

unsigned char DCE_LoadRBOIHeader(char * fname, char * tex_name, unsigned char * blend, unsigned char * instances)
{
	FILE * f = fopen(fname, "rb");
	
	char buf[256];
	unsigned int index;
	char id[32];
	char val[32];
	
	fgets(buf, 256, f);
    sscanf(buf, "%s %i", id, &index);

    *instances = index & 0xFF;

	fgets(buf, 256, f);
    sscanf(buf, "%s %s", id, val);

    *blend = val[0] == 'b' ? 1 : 0;

	fgets(buf, 256, f);
    sscanf(buf, "%s %s", id, tex_name);    
	
	fclose(f);
}
/*
color32 DCE_VectorLight(vec3f * p, vec3f * n, vec3f * lp, vec3f * lc)
{
    color32 C;
	C.a = 0xFF;

	vec3f L, Kd;
	vec3f_sub(lp, p, &L);
	vec3f_normalize(&L);

	float diffuseLight = vec3f_dot(n, &L);

	float distance = vec3f_distance(p, lp) / 2000.0f;

	if(distance > 1.0f)
		distance = 1.0f;

	if(diffuseLight > 0)
	{
		Kd.x = min(min(lc->x * diffuseLight, 1.0f) + .05, 1.0f) * distance;
		Kd.y = min(min(lc->y * diffuseLight, 1.0f) + .05, 1.0f) * distance;
		Kd.z = min(min(lc->z * diffuseLight, 1.0f) + .05, 1.0f) * distance;

		C.r = Kd.x * 0xFF;
	    C.g = Kd.y * 0xFF;
	    C.b = Kd.z * 0xFF;
	}

	return C;
}*/

#define clamp0_255(n) n > 255 ? 255 : n < 0 ? 0 : n

void DCE_VertexLightRBO(DCE_Vertex * v, unsigned int count)
{
	float r, g, b, s = 0.55f;
	
	while(count--)
	{
	    r = ((v->color.g / 255.0f) * s);
		g = ((v->color.r / 255.0f) * s);
		b = ((v->color.a / 255.0f) * s);
		
		v->color.b = 0xFF;
		v->color.r = clamp0_255(r * 255);
		v->color.g = clamp0_255(g * 255);
		v->color.a = clamp0_255(b * 255);
		
		++v;
	}
}

void DCE_VertexScaleTexCoordRBO(DCE_Vertex * vert, unsigned int count, float u, float v)
{
	while(count--)
	{
		vert->texcoord.u *= u;
		vert->texcoord.v *= v;
		
		++vert;
	}
}

void DCE_VertexScaleRBO(DCE_Vertex * vert, unsigned int count, float x, float y, float z)
{
	while(count--)
	{
		vert->pos.x *= x;
		vert->pos.y *= y;
		vert->pos.z *= z;
		
		++vert;
	}
}

void DCE_VertexAddRBO(DCE_Vertex * vert, unsigned int count, float x, float y, float z)
{
	while(count--)
	{
		vert->pos.x += x;
		vert->pos.y += y;
		vert->pos.z += z;
		
		++vert;
	}
}

void DCE_VertexSetColorRBO(DCE_Vertex * vert, unsigned int count, float a, float r, float g, float b)
{
	while(count--)
	{
		vert->color.a = a * 255;
		vert->color.r = r * 255;
		vert->color.g = g * 255;
		vert->color.b = b * 255;
		
		++vert;
	}
}

void DCE_LoadRBO(char * fname, DCE_RenderObject * rbo, char * tex_name)
{
	FILE * f = fopen(fname, "rb");                                            // Open Input RBO File
	if(f == NULL)
		while(1)
			printf("RBO Error %s\n", fname);                                  // Infinite Debug Loop
	
	fread(rbo, 1, sizeof(DCE_RenderObject) - sizeof(DCE_Vertex *), f); // Read RBO Header  
    
    rbo->texID = DCE_TextureCacheIndex(tex_name);                        // Find Texture Index  
    if(!rbo->texID)
    {
		//DCE_StringCopyNewExt(tn, buf + 4, ".tga");
		
		printf("Attempt To Read RBO TexName\n");
		printf("FILE: %s, TEX: %s\n", fname, rbo->mtl.tex_name);
		rbo->texID = DCE_TextureCacheIndex(rbo->mtl.tex_name);
	}   
    
	rbo->vert = malloc(sizeof(DCE_SimpleVertex) * rbo->verts);      // Malloc RBO Vertex Array
    
	fseek(f, sizeof(DCE_RenderObject) - sizeof(DCE_Vertex *), SEEK_SET);      // Seek To RBO Vertex Offset

	fread(rbo->vert, sizeof(DCE_Vertex), rbo->verts, f);            // Read RBO Vertex Array
	
	fclose(f);                                                                // Close RBO file
	
	DCE_VertexLightRBO(rbo->vert, rbo->verts);
}

void DCE_LoadRBOCluster(char * fname)
{
	FILE * f = NULL;      // Input TXT File
	FILE * rbo = NULL;    // Parsed RBO File
	FILE * shader = NULL; // Parsed SHADER File
	
	char buf[96]; // Read Buf
	char id[64];  // Tmp Buf
	char sn[64];  // Shader Name
	char fn[64];  // File Name
	char tn[64];  // Texture Name
	char pm[32];
	int active;
	int shadow;
	float u, v, x, y, z, a, r, g, b;
	
	DCE_RenderCluster * robj;  // Render Object Cluster Pointer
	DCE_RenderCluster * rptr = &DCE_RENDER_CLUSTER[RBO_OBJECTS]; // Pointer to First Node In Cluster
	
	f = fopen(fname, "rb");  // Open Segement Descriptor File
	if(f == NULL)
	{
		printf("File Error %s\n", fname);
		return 0;
	}

	fgets(buf, 96, f);                                // Read First Line
	
	sscanf(buf, "%s %s %i", id, rptr->segment, &active);  // Parse Segment Name and Active State
	
	rptr->active[0] = active & 0xFF;
	
	while(fgets(buf, 64, f)) // Scan to Render Objects
		if(buf[0] == 'R')
            break;

    if(buf[0] != 'R') // NO RENDER Found
    {
        fclose(f);    
		return;       
	}
    
    fgets(buf, 64, f);//Skip { brace start

	while(fgets(buf, 64, f)) // Read Next Line
	{
		if(buf[0] == '}')    // Check for end of segment
            break;
	    
	    robj = &DCE_RENDER_CLUSTER[RBO_OBJECTS];
	    
	    robj->active[0] = rptr->active[0]; // Set active state
	    
	    strncpy(robj->segment, rptr->segment, strlen(rptr->segment)); // Set Segment Name
	    
	    robj->instances = 0;
	    
        robj->instance = NULL;
	    	    	    
	    uint16 len = DCE_StringFindCharFirst(buf, '.');
	
        sscanf(buf + 4, "%s %i", id, &shadow);
	    
	    robj->shadow = shadow & 0xFF;

	    buf[len + 4] = '\0';         // Clear trailing newline character read in
	    
		sprintf(fn, "%s", buf + 4);  // File Name
        
        sprintf(sn, "%s", buf + 4);  // Possible Shader Name
        
        DCE_StringCopyNewExt(sn, buf + 4, ".shader");

        sprintf(tn, "%s", buf + 4);  // Possible Shader Name
        
        DCE_StringCopyNewExt(tn, buf + 4, ".pvr");
            
        DCE_LoadRBO(fn, &robj->rbo, tn); // Read RBO File
	    
	    shader = fopen(sn, "rb"); // Parse Shader ///////////////////////////////////
	    if(shader != NULL)
	    {
            while(fgets(buf, 64, shader)) // Scan to SHADER Start Objects
		        if(buf[0] == '{')
                    break;
			
            while(fgets(buf, 64, shader)) // Exit SHADER End
            {
		        if(buf[0] == '}')
                    break;
                    
                sscanf (buf, "%s %s %s", id, sn, pm);
                
                printf("ID: %s || SN: %s || PM: %s\n", id, sn, pm);
                
                switch(id[0])
                {
                	case 'a': // argb
						if(id[1] == 'r')
						{
						    sscanf(buf, "%s %s %s %f %s %f %s %f %s %f", id, sn, pm, &a, id, &r, id, &g, id, &b);
							    
                	        DCE_VertexSetColorRBO(robj->rbo.vert, robj->rbo.verts, a, r, g, b);
                	    }
                	    else
                	    {
                	        sscanf(buf, "%s %s %s %f %s %f %s %f", id, sn, pm, &x, id, &y, id, &z);
                	    
                	        DCE_VertexAddRBO(robj->rbo.vert, robj->rbo.verts, x, y, z);
						}                	    
                	    break;
                	    
                	case 's':  
                	    
                	    if(id[2] == 'a') // Scale
                	    {
                	    	if(id[6] == 't') // scale_tex
                	    	{
							    sscanf(buf, "%s %s %s %f %s %f", id, sn, pm, &u, id, &v);
                	    	
							    DCE_VertexScaleTexCoordRBO(robj->rbo.vert, robj->rbo.verts, u, v);
							}
                	    	else if(id[6] == 'v') // scale_vert
                	    	{
							    sscanf(buf, "%s %s %s %f %s %f %s %f", id, sn, pm, &x, id, &y, id, &z);
                	    	    /*
                	    	    robj->rbo.mtl.Ns = 1.0f;
                	    	    robj->rbo.mtl.Ks.x = x;
                	    	    robj->rbo.mtl.Ks.y = y;
                	    	    robj->rbo.mtl.Ks.z = z;
                	    	    */
							    DCE_VertexScaleRBO(robj->rbo.vert, robj->rbo.verts, x, y, z);
							}
						}
						else if(id[2] = 'r') // Surfaceparam
						{
						    if(sn[0] == 'b')
                	            robj->flags |= 1;        // Enable Blend
                	        else if(sn[0] == 'p')
                	            robj->flags |= (1 << 1); // Punch Through
						}                	        
                		break;
                	
                	case 'b': //blend modes
                	    
						robj->flags |= (DCE_ShaderParseToken(sn) & 0x7) << 5; // Compute and Compress Blend SRC Factor
                	    robj->flags |= (DCE_ShaderParseToken(pm) & 0x7) << 2; // Compute and Compress Blend DST Factor
                	    
                	    break;
                	    
                	case 't': // tex
                	    break;
				}
			}
	    }	   
			    
	    fclose(shader);
	    
	    ++RBO_OBJECTS;
    }
	
	fclose(f);

}


void DCE_LoadARBO(char * fname, unsigned char active, char * segment, unsigned char shadow)
{
	FILE * f = NULL;      // Input TXT File
	FILE * shader = NULL;
	
	char buf[96]; // Read Buf
	char id[64];  // Tmp Buf
	char fn[64];  // File Name
	char tn[64];  // Texture Name
	char sn[32];
	char pm[32];
	
	unsigned char blend;
	
	DCE_RenderCluster * robj = &DCE_RENDER_CLUSTER[RBO_OBJECTS]; // Pointer to First Node In Cluster
	
	printf("\n\n====================================\n\n");
	
	printf("RBOI: %s\n", fname);
	
	DCE_LoadRBOIHeader(fname, tn, &blend, &robj->instances);
	
	printf("\n\n==TExName: %s==\n\n", tn);
	printf("instances: %i\n", robj->instances);
	    
    robj->active[0] = active; // Set active state
	    
    robj->instance = malloc(sizeof(DCE_RenderObjectInstance) * robj->instances);    
    
    robj->shadow = shadow;
    
    strncpy(robj->segment, segment, strlen(segment)); // Set Segment Name	
	
	DCE_StringCopyNewExt(fn, fname, ".rbo");

    DCE_StringCopyNewExt(sn, fn, ".shader");
	
	shader = fopen(sn, "rb"); // Parse Shader ///////////////////////////////////
	if(shader != NULL)
	{
            while(fgets(buf, 64, shader)) // Scan to SHADER Start Objects
		        if(buf[0] == '{')
                    break;
			
            while(fgets(buf, 64, shader)) // Exit SHADER End
            {
		        if(buf[0] == '}')
                    break;
                    
                sscanf (buf, "%s %s %s", id, sn, pm);
                
                printf("ID: %s || SN: %s || PM: %s\n", id, sn, pm);
                
                switch(id[0])
                {
                	case 's':  //surfaceparam
                	    
						if(sn[0] == 'b')
                	        robj->flags |= 1;        // Enable Blend
                	    else if(sn[0] == 'p')
                	        robj->flags |= (1 << 1); // Punch Through
                	        
                		break;
                	
                	case 'b': //blend modes
                	    
						robj->flags |= (DCE_ShaderParseToken(sn) & 0x7) << 5; // Compute and Compress Blend SRC Factor
                	    robj->flags |= (DCE_ShaderParseToken(pm) & 0x7) << 2; // Compute and Compress Blend DST Factor
                	    
                	    break;
                	    
                	case 't': // tex
                	    break;
				}
			}
	    }	   
			    
	    fclose(shader);	
	    
	DCE_LoadRBO(fn, &robj->rbo, tn); // Read RBO File

	f = fopen(fname, "rb");  // Open Segement Descriptor File
	if(f == NULL)
	{
		printf("File Error %s\n", fname);
		return;
	}    
	
	while(fgets(buf, 64, f)) // Scan to Next Object
		if(buf[0] == '{')
            break;
    
    DCE_RenderObjectInstance * instance = robj->instance;
    
	while(fgets(buf, 64, f)) // Read Next Line
	{
		if(buf[0] == '}')    // Check for end of segment
		{	    
			++instance;
	    }
	    
        // Parse Translation Vector
        if(buf[4] == 't' && buf[5] == 'r' && buf[6] == 'a')
        {
        	sscanf(buf, "%s %c %c %f %c %f %c %f", id, id, id, &instance->trans.x, id,
		            &instance->trans.z, id, &instance->trans.y); 
    
		    instance->trans.x *= -10; // Scale and Swizzle for Up Vector Transform
		    instance->trans.y *= 10;
		    instance->trans.z *= 10;
		}    

        // Parse Rotation Float
        else if(buf[4] == 'r' && buf[5] == 'o' && buf[6] == 't')
        {
        	sscanf(buf, "%s %c %f", id, id, &instance->rot); 
        	
        	instance->rot *= -DEG2RAD; // Convert Degrees to Radians and Invert for UP Vector Transform
		} 
		
		memset(buf, 0, 64); // Clear the buffer for next line
    }
    
    DCE_LoadCBOInstance(fname, robj->active, segment, robj->instance, robj->instances);
    
	++RBO_OBJECTS;
	
	fclose(f);

}


void DCE_LoadARBOCluster(char * fname)
{
	FILE * f = NULL;      // Input TXT File
	
	char buf[96]; // Read Buf
	char id[64];  // Tmp Buf
	char fn[64];  // File Name
    char segment[32];
    int active;
    int shadow;
    
	printf("\n\n==ARBO Cluster==========================\n\n");
	
	f = fopen(fname, "rb");  // Open Segement Descriptor File
	if(f == NULL)
	{
		printf("File Error %s\n", fname);
		return;
	}
	
	fgets(buf, 96, f);       // Read First Line
	
	sscanf(buf, "%s %s %i", id, segment, &active);  // Parse Segment Name and Active State
	
	while(fgets(buf, 64, f)) // Scan to Render Objects
		if(buf[0] == 'A')
            break;

    if(buf[0] != 'A') // NO ASSETS Found
    {
        fclose(f);    
		return;       
	}
    
    fgets(buf, 64, f);       // Skip { brace start

	while(fgets(buf, 64, f)) // Read Next Line
	{
		if(buf[0] == '}')    // Check for end of segment
            break;

	    uint16 len = DCE_StringFindCharFirst(buf, '.');
	
        sscanf(buf + 4, "%s %i", id, &shadow);
	    
	    buf[len + 4] = '\0'; // Clear trailing newline character read in
        
		sprintf(fn, "%s", buf + 4);  // File Name

        DCE_LoadARBO(fn, active & 0xFF, segment, shadow & 0xFF);
    }
	
	fclose(f);
}

void DCE_LoadLevelBatch(char * fname)
{
	char dir[32];
	
	DCE_StringCopyNewExt(dir, fname, ".f");
	
	uint16 pos = DCE_StringFindCharFirst(dir, '/');
	
	dir[pos] = '\0';
	
	printf("FILE: %s\n", fname);
	printf("DIR: %s\n", dir);
	
	chdir(dir);

    DCE_LoadDoorCluster(fname + pos + 1);   
	    
    DCE_LoadRBOCluster(fname + pos + 1);
     
    DCE_LoadCBOCluster(fname + pos + 1);
    
    DCE_LoadARBOCluster(fname + pos + 1);

    DCE_LoadTriggerCluster(fname + pos + 1);	
}

void DCE_LoadRBOBatch(char * fname)
{ 
    char buf[128];
    
    chdir("/cd/level_01");
    
	DCE_LoadTextureBatch("textures");
    
    chdir("/cd/level_01");
    
    DCE_LoadDoors();
    
    chdir("/cd/level_01");
    
    FILE * f = fopen(fname, "rb");
    if(f == NULL)
    {
    	printf("FILE ERROR: %s\n", fname);
    	return;
	}
    
    while(fgets(buf, 128, f))
    {
    	buf[DCE_StringFindCharFirst(buf, '.') + 4] = '\0';
    	
    	chdir("/cd/level_01");
    	
    	DCE_LoadLevelBatch(buf);
	}    
	
	fclose(f);
}
