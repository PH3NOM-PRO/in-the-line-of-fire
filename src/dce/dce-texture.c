/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#include "dce.h"

static DCE_Texture DCE_TEXTURE[DCE_MAX_BOUND_TEXTURES];

static unsigned int DCE_VRAM_USED = 0;

void DCE_InitTextures()
{
	unsigned short i;
	for(i = 0; i < DCE_MAX_BOUND_TEXTURES; i++)
	{
		DCE_TEXTURE[i].data = NULL;
		DCE_TEXTURE[i].bound = 0;
	}
}

unsigned short DCE_TextureBindData(unsigned short width, unsigned short height, unsigned int bytes,
							       unsigned int color, void * data)
{
	unsigned short index = 1;
	while(DCE_TEXTURE[index].bound)
		++index;

	DCE_TEXTURE[index].bound = 1;
	DCE_TEXTURE[index].w = width;
	DCE_TEXTURE[index].h = height;
	DCE_TEXTURE[index].color = color;
	DCE_TEXTURE[index].size = bytes;
	DCE_TEXTURE[index].data = data;
    
	return index;
}

unsigned short DCE_TextureBind(unsigned short width, unsigned short height, unsigned int bytes,
							   unsigned int color, void * data)
{
	unsigned short index = 1;
	while(DCE_TEXTURE[index].bound)
		++index;

	DCE_TEXTURE[index].bound = 1;
	DCE_TEXTURE[index].w = width;
	DCE_TEXTURE[index].h = height;
	DCE_TEXTURE[index].color = color;
	DCE_TEXTURE[index].size = bytes;
	DCE_TEXTURE[index].data = pvr_mem_malloc(bytes);

	pvr_txr_load(data, DCE_TEXTURE[index].data, bytes);

    DCE_VRAM_USED += bytes;
    
    printf("DCE Bind Texture: %i || Used: %i\n", index, DCE_VRAM_USED);
    
	return index;
}

unsigned short DCE_TextureBindInvert(unsigned short width, unsigned short height, unsigned int bytes,
							   unsigned int color, void * data)
{
	unsigned short index = 1;
	while(DCE_TEXTURE[index].bound)
		++index;

	DCE_TEXTURE[index].bound = 1;
	DCE_TEXTURE[index].w = width;
	DCE_TEXTURE[index].h = height;
	DCE_TEXTURE[index].color = color;
	DCE_TEXTURE[index].size = bytes;
	DCE_TEXTURE[index].data = pvr_mem_malloc(bytes);
    
    pvr_txr_load_ex(data, DCE_TEXTURE[index].data, width, height, PVR_TXRLOAD_16BPP | PVR_TXRLOAD_INVERT_Y);

    DCE_VRAM_USED += bytes;
    
    printf("DCE Bind Texture: %i || Used: %i\n", index, DCE_VRAM_USED);
    
	return index;
}

unsigned short DCE_TextureWidth(unsigned short index)
{
	return DCE_TEXTURE[index].w;
}

unsigned short DCE_TextureHeight(unsigned short index)
{
	return DCE_TEXTURE[index].h;
}

unsigned int DCE_TextureSize(unsigned short index)
{
	return DCE_TEXTURE[index].size;
}

void DCE_TextureUnBind(unsigned short index)
{
	if(!DCE_TEXTURE[index].bound)
		return;

	if(DCE_TEXTURE[index].data)
	{
		printf("DCE: Free Texture Index: %i\n", index);
		pvr_mem_free(DCE_TEXTURE[index].data);
		DCE_TEXTURE[index].data = NULL;
	}

    DCE_VRAM_USED -= DCE_TEXTURE[index].size;
    
    printf("DCE UnBind Texture: %i || Used: %i\n", index, DCE_VRAM_USED);

	DCE_TEXTURE[index].bound = 0;
}

DCE_Texture * DCE_TextureObject(unsigned short index)
{
	return &DCE_TEXTURE[index];
}

unsigned short int DCE_TextureLoadJPG(char *fname)
{
	FILE *infile = NULL;
	int		i;
	uint16		* ourbuffer;
	uint16		* temp_tex;
    int scale = 1;

	infile = fopen(fname, "rb");
	if(infile == NULL)
	{
		printf("DCE: JPEG TEXTURE FILE READ ERROR: %s\n", fname);
		return 0;
	}

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY	buffer;			/* Output row buffer */
	int		row_stride;		/* physical row width in output buffer */
    
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, (FILE*)infile);
	(void)jpeg_read_header(&cinfo, TRUE);
	jpeg_calc_output_dimensions(&cinfo);

	temp_tex = (uint16 *)malloc(cinfo.image_width * cinfo.image_height* 2);	
	ourbuffer = (uint16 *)malloc(cinfo.image_width * 2);

	cinfo.scale_denom = scale;
	(void)jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	while (cinfo.output_scanline < cinfo.output_height)
	{
		(void)jpeg_read_scanlines(&cinfo, buffer, 1);

		for(i=0; i<row_stride/3; i++)
			ourbuffer[i] = ((buffer[0][i*3]>>3)<<11) + ((buffer[0][i*3+1]>>2)<<5) + (buffer[0][i*3+2]>>3);

		memcpy(temp_tex + cinfo.image_width*(cinfo.output_scanline-1), ourbuffer, 2*row_stride/3); 
	}

	unsigned short int w = 0x8, h = 0x8, index;

    while(w < cinfo.image_width)
		w <<= 1;
    while(h < cinfo.image_height)
		h <<= 1;

	printf("DCE: JPEG IMAGE Resolution: %ix%i\n", cinfo.image_width,cinfo.image_height);
	printf("DCE: Adjusted PVR TEXTURE Resolution: %ix%i\n", w, h);

	if(w != cinfo.image_width)
	{
		printf("DCE: JPEG IMAGE Width Non Power Of 2: Padding TEXTURE for PVR\n");
		printf("DCE: WARNING: TEXTURE U/V Will Be offset by %f, %f\n",
			                  (double)w / cinfo.image_width, (double)h / cinfo.image_height);
		unsigned short * data = malloc(w * h * 2);
		memset(data, 0, w * h * 2);

		unsigned short y;
		for(y = 0; y < cinfo.image_height; y++)
			memcpy(data + y * w, temp_tex + y * cinfo.image_width, w * 2);

		index = DCE_TextureBind(w, h, w * h * 2,
							   PVR_TXRFMT_NONTWIDDLED | PVR_TXRFMT_RGB565, data);

		free(data);
	}
	else
		index = DCE_TextureBind(w, h, w * h * 2,
							   PVR_TXRFMT_NONTWIDDLED | PVR_TXRFMT_RGB565, temp_tex);

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	free(ourbuffer);
	free(temp_tex);
	fclose(infile);

    return index;
}

void convert_to_555(uint16 *src, uint16 *dst, int count, uint16 key)
{
    int i;
    for(i = 0; i < count; i++)
    {
        if(*src == key)
            *dst++ = 0x0000;
        else
            *dst++ = (*src & 0x001F) | ((*src & 0xFFC0) >> 1) | 0x8000;
        *src++;
    }
}

unsigned short int DCE_TextureLoadJPG1555(char *fname)
{
	FILE *infile = NULL;
	int		i;
	uint16		* ourbuffer;
	uint16		* temp_tex;
    int scale = 1;

	infile = fopen(fname, "rb");
	if(infile == NULL)
	{
		printf("DCE: JPEG TEXTURE FILE READ ERROR: %s\n", fname);
		return 0;
	}

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY	buffer;			/* Output row buffer */
	int		row_stride;		/* physical row width in output buffer */
    
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, (FILE*)infile);
	(void)jpeg_read_header(&cinfo, TRUE);
	jpeg_calc_output_dimensions(&cinfo);

	temp_tex = (uint16 *)malloc(cinfo.image_width * cinfo.image_height* 2);	
	ourbuffer = (uint16 *)malloc(cinfo.image_width * 2);

	cinfo.scale_denom = scale;
	(void)jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	while (cinfo.output_scanline < cinfo.output_height)
	{
		(void)jpeg_read_scanlines(&cinfo, buffer, 1);

		for(i=0; i<row_stride/3; i++)
			ourbuffer[i] = ((buffer[0][i*3]>>3)<<11) + ((buffer[0][i*3+1]>>2)<<5) + (buffer[0][i*3+2]>>3);

		memcpy(temp_tex + cinfo.image_width*(cinfo.output_scanline-1), ourbuffer, 2*row_stride/3); 
	}

	unsigned short int w = 0x8, h = 0x8, index;

    while(w < cinfo.image_width)
		w <<= 1;
    while(h < cinfo.image_height)
		h <<= 1;

	printf("DCE: JPEG IMAGE Resolution: %ix%i\n", cinfo.image_width,cinfo.image_height);
	printf("DCE: Adjusted PVR TEXTURE Resolution: %ix%i\n", w, h);

	if(w != cinfo.image_width)
	{
		printf("DCE: JPEG IMAGE Width Non Power Of 2: Padding TEXTURE for PVR\n");
		printf("DCE: WARNING: TEXTURE U/V Will Be offset by %f, %f\n",
			                  (double)w / cinfo.image_width, (double)h / cinfo.image_height);
		unsigned short * data = malloc(w * h * 2);
		unsigned short * data1555 = malloc(w * h * 2);
		
        memset(data, 0, w * h * 2);

		unsigned short y;
		for(y = 0; y < cinfo.image_height; y++)
			memcpy(data + y * w, temp_tex + y * cinfo.image_width, w * 2);

		convert_to_555(data, data1555, w * h, 0x0000);
		
        index = DCE_TextureBind(w, h, w * h * 2,
							   PVR_TXRFMT_NONTWIDDLED | PVR_TXRFMT_ARGB1555, data1555);
        free(data1555);
		free(data);
	}
	else
	{
        unsigned short * data1555 = malloc(w * h * 2);
        
        convert_to_555(temp_tex, data1555, w * h, 0x0000);
        
		index = DCE_TextureBind(w, h, w * h * 2,
							   PVR_TXRFMT_NONTWIDDLED | PVR_TXRFMT_ARGB1555, data1555);
							   
        free(data1555);
     }

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	free(ourbuffer);
	free(temp_tex);
	fclose(infile);

    return index;
}

unsigned short int DCE_TextureLoadPVR(char *fname)
{
#define PVR_HDR_SIZE 0x20
    FILE *tex = NULL;
    unsigned char *texBuf;
    unsigned int texSize;

    tex = fopen(fname, "rb");

    if(tex == NULL) {
        printf("DCE: PVR TEXTURE FILE READ ERROR: %s\n", fname);
        
		fclose(tex);
        
		return 0;
    }

    fseek(tex, 0, SEEK_END);
    texSize = ftell(tex);

    texBuf = malloc(texSize);
    fseek(tex, 0, SEEK_SET);
    fread(texBuf, 1, texSize, tex);
    fclose(tex);

    int texW = texBuf[PVR_HDR_SIZE - 4] | texBuf[PVR_HDR_SIZE - 3] << 8;
    int texH = texBuf[PVR_HDR_SIZE - 2] | texBuf[PVR_HDR_SIZE - 1] << 8;
    int texFormat, texColor;

    printf("DCE: PVR TEXTURE Resolution: %ix%i\n", texW, texH);
    
    switch((unsigned int)texBuf[PVR_HDR_SIZE - 8]) {
        case 0x00:
            texColor = PVR_TXRFMT_ARGB1555;
            break; //(bilevel translucent alpha 0,255)

        case 0x01:
            texColor = PVR_TXRFMT_RGB565;
            break; //(non translucent RGB565 )

        case 0x02:
            texColor = PVR_TXRFMT_ARGB4444;
            break; //(translucent alpha 0-255)

        case 0x03:
            texColor = PVR_TXRFMT_YUV422;
            break; //(non translucent UYVY )

        case 0x04:
            texColor = PVR_TXRFMT_BUMP;
            break; //(special bump-mapping format)

        case 0x05:
            texColor = PVR_TXRFMT_PAL4BPP;
            break; //(4-bit palleted texture)

        case 0x06:
            texColor = PVR_TXRFMT_PAL8BPP;
            break; //(8-bit palleted texture)

        default:
            texColor = PVR_TXRFMT_RGB565;
            break;
    }

    switch((unsigned int)texBuf[PVR_HDR_SIZE - 7]) {
        case 0x01:
            texFormat = PVR_TXRFMT_TWIDDLED;
            break;//SQUARE TWIDDLED

        case 0x03:
            texFormat = PVR_TXRFMT_VQ_ENABLE;
            break;//VQ TWIDDLED

        case 0x09:
            texFormat = PVR_TXRFMT_NONTWIDDLED;
            break;//RECTANGLE

        case 0x0B:
            texFormat = PVR_TXRFMT_STRIDE | PVR_TXRFMT_NONTWIDDLED;
            break;//RECTANGULAR STRIDE

        case 0x0D:
            texFormat = PVR_TXRFMT_TWIDDLED;
            break;//RECTANGULAR TWIDDLED

        case 0x10:
            texFormat = PVR_TXRFMT_VQ_ENABLE | PVR_TXRFMT_NONTWIDDLED;
            break;//SMALL VQ

        default:
            texFormat = PVR_TXRFMT_NONE;
            break;
    }

    int index = DCE_TextureBind(texW, texH, texSize - PVR_HDR_SIZE,
							   texFormat | texColor, texBuf + PVR_HDR_SIZE);
    
    free(texBuf);
    
    return index;
}

#define RGB565_RED_MASK    0xF800
#define RGB565_GREEN_MASK  0x7E0
#define RGB565_BLUE_MASK   0x1F

#define RGB565_RED_SHIFT   0xB
#define RGB565_GREEN_SHIFT 0x5
#define RGB565_BLUE_SHIFT  0x0

#define RGBA32_APLHA_MASK 0xFF
#define RGBA32_RGB_MASK   0xFFFFFF00

static unsigned char RGB565_RED(unsigned short c) {
    return (c & RGB565_RED_MASK) >> RGB565_RED_SHIFT;
}

static unsigned char RGB565_GREEN(unsigned short c) {
    return (c & RGB565_GREEN_MASK) >> RGB565_GREEN_SHIFT;
}

static unsigned char RGB565_BLUE(unsigned short c) {
    return c & RGB565_BLUE_MASK;
}

uint16 DCE_AverageQuadPixelRGB565(uint16 p1, uint16 p2, uint16 p3, uint16 p4) {
    uint8 R = (RGB565_RED(p1) + RGB565_RED(p2) + RGB565_RED(p3) + RGB565_RED(p4)) / 4;
    uint8 G = (RGB565_GREEN(p1) + RGB565_GREEN(p2) + RGB565_GREEN(p3) + RGB565_GREEN(p4)) / 4;
    uint8 B = (RGB565_BLUE(p1) + RGB565_BLUE(p2) + RGB565_BLUE(p3) + RGB565_BLUE(p4)) / 4;

    return R << RGB565_RED_SHIFT | G << RGB565_GREEN_SHIFT | B;
}

void DCE_TextureDownsampleHalfSize(uint16 *src, uint16 *dst, uint16 width, uint16 height)
{
    uint32 i = 0;
    uint16 x, y;

    for(y = 0; y < height; y += 2)
    {
        for(x = 0; x < width; x += 2)
        {
            dst[i++] = DCE_AverageQuadPixelRGB565(*src, *(src + 1), *(src + width), *(src + width + 1));

            src += 2;
        }
        src += width;
    }
}

void DCE_TextureCoord3D(vec3f_t *eye, vec3f_t *point, vec3f_t *normal,
                        float *s, float *t, unsigned char *face)
{
	float sc, tc, ma;
	
    vec3f_t P;
    vec3f_t N;
    vec3f_t I;
    vec3f_t R;

 //   mat_trans_single3_nodiv_nomod(point->x, point->y, point->z, P.x, P.y, P.z);
 //   mat_trans_normal3_nomod(normal->x, normal->y, normal->z, N.x, N.y, N.z);
    
    vec3f_sub(eye, point, &I);
    vec3f_reflect(&I, normal, &R);
        
    switch(vec3f_major_axis_component(&R))
    {
        case 0:
             *face = R.x > 0.0f ? 0 : 1;
             sc = R.x > 0.0f ? -R.z : R.z;
             tc = -R.y;
             ma = fabs(R.x);
             
             break;
             
        case 1:
             *face = R.y > 0.0f ? 2 : 3;
             sc = R.x;
             tc = R.y > 0.0f ? -R.z : R.z;
             ma = fabs(R.y);

             break;
        
        case 2:
             *face = R.z > 0.0f ? 4 : 5;
             sc = R.z > 0.0f ? R.x : -R.x;
             tc = -R.y;
             ma = fabs(R.z);
             
             break;
    }

	if (ma == 0.0f)
    {
		*s = 0.0f;
		*t = 0.0f;
	}
    else
    {
		*s = ((sc / ma) + 1.0f) * 0.5f;
		*t = ((tc / ma) + 1.0f) * 0.5f;
	}
}

#define SCALE_AND_BIAS(n)((n * .5f) + .5f)
#define CLAMP_0(n)(n < 0.0f ? 0.0f : n > 1.0f ? 0.0f : n)
#define MAX(a,b)(a < b ? b : a)

void DCE_TextureCoordProjected(vec3f_t *eye, vec3f_t *point, vec3f_t *normal,
                        float *s, float *t)
{   
    float d;
    vec3f_t L;
    vec3f_t P = { point->x, point->y, point->z };

    mat_trans_single(P.x, P.y, P.z);

    vec3f_sub(point, eye, &L);
    vec3f_normalize(L.x, L.y, L.z);
    vec3f_dot(normal->x, normal->y, normal->z, L.x, L.y, L.z, d);
    
    d = d < 0 ? 0 : d;
    
    if(d == 0 || P.z < 0.0f)
    {
         *s = 0.0f;
         *t = 0.0f;
    }
    else
    {
        *s = P.x / 640.0f;
        *t = P.y / 480.0f;
    }
}

void DCE_TextureCoordProjectedFast(vec3f_t *point, float *s, float *t)
{   
    float d;
    vec3f_t P = { point->x, point->y, point->z };

    mat_trans_single(P.x, P.y, P.z);

    if(P.z < 0.0f || (P.x < 0.0f || P.y < 0.0f) ||(P.x > 640.0f || P.y > 480.0f))
    {
         *s = 0.0f;
         *t = 0.0f;
    }
    else
    {
        *s = P.x / 640.0f;
        *t = P.y / 480.0f;
    }
}

unsigned int DCE_TextureSample2D565(unsigned short int * buf, unsigned short int w, unsigned short int h,
                                    float u, float v)
{
	unsigned short int x = u * w;
	unsigned short int y = v * h;
	
	unsigned short int pixel = buf[x * y];
	
	unsigned char r = (((pixel & RGB565_RED_MASK) >> RGB565_RED_SHIFT) * RGB8_MAX ) / RGB5_MAX;
	unsigned char g = (((pixel & RGB565_GREEN_MASK) >> RGB565_GREEN_SHIFT) * RGB8_MAX ) / RGB6_MAX;
	unsigned char b = (((pixel & RGB565_BLUE_MASK) >> RGB565_BLUE_SHIFT) * RGB8_MAX ) / RGB5_MAX;	
	
	return (0xFF000000 | (r << 16) | (g << 8) | b);
}

uint16 * DCE_TextureLoadJPGBuf(char *fname, unsigned short int * w, unsigned short int * h)
{
	FILE *infile = NULL;
	int		i;
	uint16		* ourbuffer;
	uint16		* temp_tex;
    int scale = 1;

	infile = fopen(fname, "rb");
	if(infile == NULL)
	{
		printf("DCE: JPEG TEXTURE FILE READ ERROR: %s\n", fname);
		return 0;
	}

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY	buffer;			/* Output row buffer */
	int		row_stride;		/* physical row width in output buffer */
    
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, (FILE*)infile);
	(void)jpeg_read_header(&cinfo, TRUE);
	jpeg_calc_output_dimensions(&cinfo);

	temp_tex = (uint16 *)malloc(cinfo.image_width * cinfo.image_height* 2);	
	ourbuffer = (uint16 *)malloc(cinfo.image_width * 2);

	cinfo.scale_denom = scale;
	(void)jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	while (cinfo.output_scanline < cinfo.output_height)
	{
		(void)jpeg_read_scanlines(&cinfo, buffer, 1);

		for(i=0; i<row_stride/3; i++)
			ourbuffer[i] = ((buffer[0][i*3]>>3)<<11) + ((buffer[0][i*3+1]>>2)<<5) + (buffer[0][i*3+2]>>3);

		memcpy(temp_tex + cinfo.image_width*(cinfo.output_scanline-1), ourbuffer, 2*row_stride/3); 
	}

   	fclose(infile);

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	free(ourbuffer);
	free(temp_tex);
    
    *w = cinfo.image_width;
    *h = cinfo.image_height;
    
    return temp_tex;
}

unsigned short int DCE_TextureLoadPVRBump(char *fname)
{
    FILE *tex = NULL;
    unsigned char *texBuf;
    unsigned int texSize;

    tex = fopen(fname, "rb");

    if(tex == NULL) {
        printf("DCE: PVR TEXTURE FILE READ ERROR: %s\n", fname);
        return 0;
    }

    fseek(tex, 0, SEEK_END);
    texSize = ftell(tex);
    fseek(tex, 0, SEEK_SET);
    
    texBuf = malloc(texSize);
    fread(texBuf, 1, texSize, tex);
    fclose(tex);

    int index = DCE_TextureBind(256, 256, texSize, PVR_TXRFMT_BUMP | PVR_TXRFMT_TWIDDLED, texBuf);
    
    free(texBuf);
    
    return index;
}

#define TGA_HDR_SIZE 18
#define TGA_HDR_BPP  16
#define TGA_HDR_WIDTH_LO 12
#define TGA_HDR_WIDTH_HI 13
#define TGA_HDR_HEIGHT_LO 14
#define TGA_HDR_HEIGHT_HI 15

#define ARGB1555_ALPHA_SHIFT 0xF
#define ARGB1555_RED_SHIFT   0xA
#define ARGB1555_GREEN_SHIFT 0x5
#define RGB5_MAX             0x1F
#define RGB8_MAX             0xFF


uint16 DCE_TextureLoadTGA1555(char * fname)
{
    uint16 w,h;
    unsigned char * hdr = (unsigned char*)malloc(TGA_HDR_SIZE);
	
	FILE * f = fopen(fname, "rb");

	if(f == NULL)
	{
	    while(1)
		printf("Error opening %s\n", fname);
		return 0;
    }
	
	fread(hdr, TGA_HDR_SIZE, 1, f);
    
	w =  hdr[TGA_HDR_WIDTH_HI] << 8 | hdr[TGA_HDR_WIDTH_LO];
	h =  hdr[TGA_HDR_HEIGHT_HI] << 8 | hdr[TGA_HDR_HEIGHT_LO];
	
	printf("TGA Dimensions : %ix%i\nTGA BPP: %i\n", w, h, hdr[16]);

	if(hdr[TGA_HDR_BPP] != 16)
		return 0;

	uint16 * tex = (uint16 * )malloc(w * h * sizeof(uint16));

	fseek(f, TGA_HDR_SIZE, SEEK_SET);

	fread(tex, sizeof(uint16), w * h, f);

	fclose(f);

    int index = DCE_TextureBind(w, h, w * h * 2, PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED, tex);

    free(hdr);

    free(tex);
    
	return index;
}

typedef struct
{
	char name[34];
	unsigned short texID;
} DCE_TextureIndex;

#define DCE_MAX_TEXTURES 64

static DCE_TextureIndex DCE_TEXTURE_CACHE[DCE_MAX_TEXTURES];
static unsigned char    DCE_TEXTURES = 0;

#include <sys/types.h>
#include <dirent.h>

typedef struct dirent direntry;

unsigned short DCE_TextureCacheIndex(char * name)
{
	unsigned char i;
	
	for(i = 0; i < DCE_TEXTURES; i++)
		if(DCE_StringEqualsIgnoreCase(name, DCE_TEXTURE_CACHE[i].name))
			return DCE_TEXTURE_CACHE[i].texID;
	
	printf("ERROR: %s does not exist\n", name);
	
    return 0;
}

void DCE_LoadTextureBatch(char * dir)
{
	DIR * d = opendir(dir);
    direntry * e;
    
	chdir(dir);
	   	 
    while((e = readdir (d)))
	{
	    uint16 pos = DCE_StringFindCharFirst(e->d_name, '.');
	    
	    switch(e->d_name[pos + 1])
	    {
	        case 'p': // PVR
				DCE_TEXTURE_CACHE[DCE_TEXTURES].texID = DCE_TextureLoadPVR(e->d_name);
				break;
			
			case 't': // TGA
				DCE_TEXTURE_CACHE[DCE_TEXTURES].texID = DCE_TextureLoadTGA1555(e->d_name);
				break;
			
			case 'j': // JPG			
				DCE_TEXTURE_CACHE[DCE_TEXTURES].texID = DCE_TextureLoadJPG(e->d_name);
				break;
		}	    
	    
		strcpy(DCE_TEXTURE_CACHE[DCE_TEXTURES].name, e->d_name, strlen(e->d_name));

	    printf("TEXTURE CACHE: %s\n", e->d_name); 
		
		++DCE_TEXTURES;
		
		if(DCE_TEXTURES == DCE_MAX_TEXTURES - 1)
	    {
	    	printf("ERROR: TEXTURE CACHE FULL\n");
	    	return;
		}
	} 
	
	chdir("/cd/");    
	
	closedir(d);
}

