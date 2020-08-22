/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#ifndef DCE_SHADER_H
#define DCE_SHADER_H

#define DCE_MAX_SHADERS 1024

#define DCE_SURFACE_OPAQUE        0x0
#define DCE_SURFACE_PUNCHTHROUGH  0x1
#define DCE_SURFACE_TRANSPARENT   0x2

typedef struct
{
	char tex_name[128];

	unsigned char enable_scroll;
	float au, av; // additive u,  v

	unsigned char enable_scale;
	float mu, mv; // multiplicative u/v
	
	unsigned char surface_env;	  // Texture Environment
	
	unsigned char surface_type;   // Blend Enable
	unsigned int  blend_src;     // Blend Src Factor
	unsigned int  blend_dst;     // Blend Dst Factor
	
	unsigned char uv_clamp;

} DCE_Shader;

void DCE_LoadShader(char * fname);
uint16 DCE_TextureShaderLookUp(char * tex_name);
void DCE_ApplyShader(uint16 index, DCE_SimpleVertex * vert, unsigned int count);
ubyte DCE_ShaderSurface(uint16 index);

unsigned char DCE_ShaderSeekNextEntry(FILE * f, char * buf);

#endif
