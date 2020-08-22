/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#ifndef DCE_SKYBOX_H
#define DCE_SKYBOX_H

void DCE_LoadSkyBoxTexturesPVR(char * base_name);
void DCE_LoadSkyBoxTexturesJPG(char * base_name);

void DCE_SubmitSkyboxTextureHeader(unsigned char index);

void DCE_RenderSkyBox(float size);

void DCE_LoadSkyBoxTexturesPVR(char * base_name);

unsigned char DCE_SkyboxTextureID(unsigned char index);

void DCE_FreeSkBoxTextures();

#endif
