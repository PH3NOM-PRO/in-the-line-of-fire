/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#ifndef DCE_HUD_H
#define DCE_HUD_H

#define HUD_MAX_SPRITES 512

#define DCE_MAX_HUD_ICONS 64

typedef struct
{
    float x, y, w, h;
    float u1, v1, u2, v2;
    unsigned int argb;
    unsigned char blend;
    unsigned char active;
    unsigned char used;
    unsigned short int texID;
} DCE_HudIcon;

void DCE_HudLoad(char * fname);

unsigned char DCE_HudLoadIcon(DCE_HudIcon * hud, float x, float y, float w, float h, float u1, float v1, float u2, float v2,
                     unsigned short texID, unsigned int argb, unsigned char active);
                     
void DCE_HudRenderCallback(unsigned char index);
              
void DCE_HudSetIndex(unsigned char index);
			        
void DCE_HudSwitchWeapons();

void DCE_HudReInit();
			        
/////////////// OLD HUD API /////////////////////////////////////////////

typedef struct
{
    float u1;
    float v1;
    float u2;
    float v2;
} HUD_TexCoord;  // Texture Coordinate Per Frame of Sprite

typedef struct
{
    vec3f_t pos;                 // position in 3d space
    float width;                 // width of sprite in pixels
    float height;                // height of sprite
    HUD_TexCoord * uv;           // Start / End Tex Coords per animation frame
    unsigned short int * texID;  // dynamic array of texture id's
    unsigned short frames;       // count of frames of animation
    unsigned short frame;        // current frame of animation
    unsigned short active;       // active status of sprite
    float frame_rate;            // frames per second
} HUD_Sprite;

void DCE_InitHUD();

int DCE_HudInsertSprite(HUD_Sprite * sprite);

void DCE_HudNextSpriteFrame(HUD_Sprite * sprite);

unsigned char DCE_HudRemoveSprite(unsigned short index);

unsigned char DCE_HudDeleteSprite(unsigned short index);

#endif
