// DCE 3D Dreamcast Engine 3D (C) Josh Pearson 2016
//
// Shader file for Surface Parameters 
//
// Each shader is specified within outter braces
//
// Within the outter braces, the parameters are given for the specified shader
//
//    tcMod indicates a Texture Coordinate Modification
//          scroll = Scroll (Add) the Texture Coords by given u/v offset
//          scale = Scale (Multiply) the Texture Coords by given u/v scale
//          clamp = Clamp Texture Coords into range of 0->1
//          repeat = Repeat (Tile) Texture Coords
//          
//    surfaceparam indicates the blending as one of three possible settings:
//          no_blend      = Opaque Surface
//          punch_through = Punch Through Surface where each pixel is either fully opaque or fully transparent
//          blend         = Transparent Surface
//
//    blendfunc specifys the blending source and dest factors if blend is set for surfaceparam
//        GL_ONE
//        GL_ZERO
//        GL_SRC_COLOR
//        GL_DST_COLOR
//        GL_SRC_ALPHA
//        GL_DST_ALPHA
//        GL_ONE_MINUS_SRC_ALPHA
//        GL_ONE_MINUS_DST_ALPHA
//        GL_ONE_MINUS_DST_COLOR
//
//    texEnv specifys the texture environment (texture combiner for transparency)
//        GL_REPLACE       (C = Ct, A = At)
//        GL_MODULATE      (C = Cs * Ct, A = At)
//        GL_DECAL         (C = (Cs * At) + (Cs * (1-At)), A = As)
//        GL_MODULATEALPHA (C = Cs * Ct, A = As * At)
//

SHADER_START

{
	surfaceparam blend
	blendFunc GL_DST_COLOR GL_DST_ALPHA
	scale_vert = { 0.999f, 0.999f, 0.999f };
}

SHADER_END
