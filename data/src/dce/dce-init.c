/* 
** DCE - Dreamcast Engine (C) 2013-2015 Josh PH3NOM Pearson
*/

#include "dce.h"

void DCE_Init()
{
    DCE_InitRender();
    
    DCE_MatrixInitRender();
 
    DCE_InitSfx(); 
	
	DCE_InitGFX();     
    
    DCE_MenuInitOptions("/cd/dce/dce.config");   

#include "pvr_internal.h"
#define FARZ_CLIP 0.00000001f /* Far Z Clipping - On PVR Smaller Value = Further Clip Pane */
    pvr_state.zclip = FARZ_CLIP;
    PVR_SET(PVR_BGPLANE_Z, FARZ_CLIP);         
}
