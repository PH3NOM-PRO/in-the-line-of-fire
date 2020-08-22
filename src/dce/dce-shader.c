/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#include "malloc.h"
#include "stdlib.h"
#include "string.h"

#include "dce.h"

static DCE_Shader * shader[DCE_MAX_SHADERS];
static unsigned int shaders = 0;

unsigned char DCE_ShaderVerifyStart(char * buf)
{
	return strncmp(buf, "DCE_SHADER_START", strlen("DCE_SHADER_START")) == 0;
}

unsigned char DCE_ShaderVerifyEnd(char * buf)
{
	return strncmp(buf, "DCE_SHADER_END", strlen("DCE_SHADER_END")) == 0;
}

unsigned char DCE_ShaderEndEntry(char * buf)
{
	return buf[0] == '}';
}

unsigned char DCE_ShaderSeekNextEntry(FILE * f, char * buf)
{
    while(fgets(buf, DCE_SCRIPT_LINE_LEN, f))
	    if(buf[0] == '{')
			return 1;

	return 0;
}

unsigned char DCE_ShaderParseToken(char * buf)
{
    if(!strncmp(buf, "GL_REPLACE", strlen(buf)))
        return PVR_TXRENV_REPLACE;
        
    if(!strncmp(buf, "GL_MODULATE", strlen(buf)))
        return PVR_TXRENV_MODULATE;
        
    if(!strncmp(buf, "GL_DECAL", strlen(buf)))
        return PVR_TXRENV_DECAL;
        
    if(!strncmp(buf, "GL_MODULATEALPHA", strlen(buf)))
        return PVR_TXRENV_MODULATEALPHA;
        
    if(!strncmp(buf, "GL_ONE", strlen(buf)))
        return PVR_BLEND_ONE;

    if(!strncmp(buf, "GL_ZERO", strlen(buf)))
        return PVR_BLEND_ZERO;

    if(!strncmp(buf, "GL_SRC_COLOR", strlen(buf)))
        return PVR_BLEND_SRCALPHA;

    if(!strncmp(buf, "GL_DST_COLOR", strlen(buf)))
        return PVR_BLEND_DESTCOLOR;

    if(!strncmp(buf, "GL_SRC_ALPHA", strlen(buf)))
        return PVR_BLEND_SRCALPHA;

    if(!strncmp(buf, "GL_DST_ALPHA", strlen(buf)))
        return PVR_BLEND_DESTALPHA;

    if(!strncmp(buf, "GL_ONE_MINUS_SRC_ALPHA", strlen(buf)))
        return PVR_BLEND_INVSRCALPHA;

    if(!strncmp(buf, "GL_ONE_MINUS_DST_ALPHA", strlen(buf)))
        return PVR_BLEND_INVDESTALPHA;

    if(!strncmp(buf, "GL_ONE_MINUS_DST_COLOR", strlen(buf)))
        return PVR_BLEND_INVDESTCOLOR;
            
	return 0;
}

unsigned char DCE_ShaderEntryParse(DCE_Shader * shader, char * buf)
{
    char type[16], id[128];

	sscanf (buf, "%s %s", type, id);
    
    /* Texture Map */
    if(!strncmp(type, "texMap", strlen("texMap")))
    {
    	strncpy(shader->tex_name, id, strlen(id));
    	
    	return 1;
	}

    /* Texture Coordinate Modification */
	if(!strncmp(type, "tcMod", strlen("tcMod"))) 
	{
		if(!strncmp(id, "clamp", strlen("clamp"))) 
		{
			shader->uv_clamp = 1;
			return 2;
		}
		if(!strncmp(id, "repeat", strlen("repeat"))) 
		{
			shader->uv_clamp = 0;
			return 2;
		}
		
		// Assume a u/v modification and read next 2 floats
		float u, v;
		sscanf (buf, "%s %s %f %f", type, id, &u, &v);
		
		if(!strncmp(id, "scale", strlen("scale"))) 
		{
			shader->enable_scale = 1;
			shader->mu = u;
			shader->mv = v;
			
			return 2;
		}	    

		if(!strncmp(id, "scroll", strlen("scroll"))) 
		{
			shader->enable_scroll = 1;
			shader->au = u;
			shader->av = v;
			
			return 2;
		}	    
	}

    /* Texture Surface Descriptoin */
	if(!strncmp(type, "surfaceparam", strlen("surfaceparam"))) 
	{
        if(!strncmp(id, "blend", strlen("blend")))
		{
		    shader->surface_type = DCE_SURFACE_TRANSPARENT;
			return 3;	
		} 
        if(!strncmp(id, "no_blend", strlen("no_blend")))
		{
		    shader->surface_type = DCE_SURFACE_OPAQUE;
			return 3;	
		} 
        if(!strncmp(id, "punch_through", strlen("punch_through")))
		{
		    shader->surface_type = DCE_SURFACE_PUNCHTHROUGH;
			return 3;	
		} 		
	}
	
	if(!strncmp(type, "blendFunc", strlen("blendFunc"))) 
	{	
        char dest[32];
        sscanf (buf, "%s %s %s", type, id, dest);
        shader->blend_src = DCE_ShaderParseToken(id);
        shader->blend_dst = DCE_ShaderParseToken(dest);
	}

	if(!strncmp(type, "texEnv", strlen("texEnv"))) 
        shader->surface_env = DCE_ShaderParseToken(id);
        
	return 0;
}

uint16 DCE_TextureShaderLookUp(char * tex_name)
{
	int i;
	
    for(i = 0; i < shaders; i++)
		if(!memcmp(shader[i]->tex_name, tex_name, strlen(tex_name)))
            return i + 1;
        
    return 0;
}

ubyte DCE_ShaderSurface(uint16 index)
{
	return shader[index - 1]->surface_type;
}

void DCE_ApplyShader(uint16 index, DCE_SimpleVertex * vert, unsigned int count)
{
	DCE_Shader * s = shader[index - 1];
	
	DCE_RenderSetTexEnv(s->surface_env);
	
	if(s->surface_type == DCE_SURFACE_TRANSPARENT)
	{
	    DCE_RenderSetBlendSrc(s->blend_src);

	    DCE_RenderSetBlendDst(s->blend_dst);
	}
	
	if(s->uv_clamp)
		DCE_RenderSetUVClamp(PVR_UVCLAMP_UV);
	else
	    DCE_RenderSetUVClamp(PVR_UVCLAMP_NONE);	
	
	if(s->enable_scroll)
	{
	    int i;
	    for(i = 0; i < count; i++)
	    {
		    vert->u += s->au;
		    vert->v += s->av;
		    ++vert;
	    }
	}

	if(s->enable_scale)
	{
	    int i;
	    for(i = 0; i < count; i++)
	    {
		    vert->u *= s->mu;
		    vert->v *= s->mv;
		    ++vert;
	    }
	}	
}

void DCE_LoadShader(char * fname)
{
	FILE * f = fopen(fname, "rb");
	char buf[DCE_SCRIPT_LINE_LEN + 1];
		
	if(shaders)  // Free any existing shaders
	{
		unsigned int i;
		for(i = 0; i< shaders; i++)
			free(shader[i]);
		
		shaders = 0;
	}

	fgets(buf, DCE_SCRIPT_LINE_LEN, f); // Read first line

	if(!DCE_ShaderVerifyStart(buf)) // Scan past comments until we reach Start of Script
	{
	    int found = 0;
		while(fgets(buf, DCE_SCRIPT_LINE_LEN, f))
		{
			if(DCE_ShaderVerifyStart(buf))
			{
				found = 1;
				break;
			}
		}
		if(!found)
		{
		    printf("Error Loading Script: Invalid File Signature\n");
			return;
		}
	}

	while(DCE_ShaderSeekNextEntry(f, buf)) // Parse Each Shader
	{
		shader[shaders] = malloc(sizeof(DCE_Shader));    // Memory for new Shader
		memset(shader[shaders], 0 , sizeof(DCE_Shader));

		while(1)  // Loop untill End of Shader is Reached
		{
		    fgets(buf, 1023, f);

	        if(DCE_ShaderEndEntry(buf))
				break;

		    DCE_ShaderEntryParse(shader[shaders], buf); // Parse an entry into the Shader
		}
		
		++shaders;
	}
}
