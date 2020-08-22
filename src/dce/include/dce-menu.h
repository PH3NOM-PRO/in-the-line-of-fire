/* 
** DCE - Dreamcast Engine (C) 2013-2015 Josh PH3NOM Pearson
*/

#ifndef DCE_MENU_H
#define DCE_MENU_H

#define DCE_MAX_VOLUME 255

#define DCE_RENDER_NO_SHADOWS      0
#define DCE_RENDER_FAST_SHADOWS    1
#define DCE_RENDER_STENCIL_SHADOWS 2

typedef struct
{
    unsigned char enable_light_maps;
    unsigned char shadow_quality;
    unsigned char aspect_ratio;  /* Index into DCE_DisplayModes */ 
    unsigned char enable_fog;      
} DCE_VideoOption;

typedef struct
{
    unsigned char music_volume;
    unsigned char sfx_volume;
} DCE_AudioOption;

typedef struct
{
    unsigned char invert_y_axis;
} DCE_ControllerOption;

typedef struct
{
    DCE_VideoOption video;
    DCE_AudioOption audio;
    DCE_ControllerOption controls;
} DCE_MenuOption;

void DCE_MenuInitOptions(char * config_file);

unsigned char DCE_MenuRun();

unsigned char DCE_MusicVolume();
unsigned char DCE_SfxVolume();

unsigned char DCE_RenderUseLightMaps();
unsigned char DCE_RenderShadowMapQuality();
unsigned char DCE_RenderUseFog();

float DCE_VideoDisplayAspectRatio();

void DCE_RequestPause();
void DCE_HandlePauseRequest();

unsigned char DCE_MenuRenderStats();
unsigned char DCE_MenuGameOver();

///////////////////////////////////////////////

typedef int (*ElementOnClick)(char *);

typedef struct
{
	char title[128];
	vec2i_t title_pos;
	char description[256];
	vec2i_t desc_pos;
	unsigned short int texID;
	vec2i_t tex_pos;
	vec2i_t tex_size;
	
	ElementOnClick func;
	char *         arg;
	
	unsigned short int nodes; // Scrolling Layres
	void * next_node;
	void * last_node;
	
} DCE_MenuElement;

typedef struct
{
	unsigned short int texID;    // BGND TexID
	float font_size;
	unsigned short int elements; // Element Count
	DCE_MenuElement * element; // Pointer Array of Elements
} DCE_Menu;

float DCE_StrobePulse(float v, float min, float max, float step, unsigned char * mode);

void DCE_InitMenu();

int DCE_MenuMain(char * menu);

void DCE_MenuLoadTextures();
unsigned char DCE_MenuRenderPauseMenu(unsigned char input, float * s, unsigned char *retval);
void DCE_WaitStartScreen();
unsigned char DCE_MenuRoot();

unsigned char DCE_MenuRenderPauseMenu(unsigned char input, float * s, unsigned char *retval);

#endif
