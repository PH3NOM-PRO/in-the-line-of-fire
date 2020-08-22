#include <stdio.h>
#include <malloc.h>

#include "md2.h"

#include "dce-obj.h"
#include "dce-render.h"
#include "dce-vector.h"

static vec4f_t DCE_MD2_VERTEX[1024 * 6];
static unsigned char DCE_MD2_VERTEX_CLIP[1024 * 6];

void DCE_PreComputeHQMD2ClipCodes(md2_mdl * mdl, unsigned char frame)
{
	unsigned int i;

    struct md2_triangle_t * tri  = &mdl->triangles[0];
    struct hqmd2_frame_t *pframe = &mdl->frames[frame];
    struct hqmd2_vertex_t *pvert = &pframe->verts[0];

    for(i = 0; i < mdl->header.num_vertices; i++)
    {
    	// The inline asm macro should perform this check and then division!!!
    	// mat_trans_single3_clip(x, y, z, c)
    	// or mat_trans_single3_nomod_clip(x, y, z, xo, yo, zo, c) lolz fuck yeah
    	mat_trans_single3_nodivw_nomod(pvert->v[0], pvert->v[1], pvert->v[2],
		                               DCE_MD2_VERTEX[i].x, DCE_MD2_VERTEX[i].y, DCE_MD2_VERTEX[i].z, DCE_MD2_VERTEX[i].w);                  
	    
		++pvert;
	}

        
    for(i = 0; i < mdl->header.num_tris; i++)
    {
	    mdl->clip_code[frame][i / 32] |= vec3f_face_in(&DCE_MD2_VERTEX[tri->vertex[0]], &DCE_MD2_VERTEX[tri->vertex[1]], &DCE_MD2_VERTEX[tri->vertex[2]]) << (i % 32);
        ++tri;
	}         

	
    for(i = 0; i < mdl->header.num_vertices; i++)
    {
		if(DCE_MD2_VERTEX[i].z > -1.0f) // Outside Z Near Plane -- Cull Vertex
		    DCE_MD2_VERTEX_CLIP[i] = 1;
		else                            // Inside Z Near Plane -- Perspective Divide Vertex
		{
			DCE_MD2_VERTEX_CLIP[i] = 0;
			
			DCE_MD2_VERTEX[i].z = 1.0f / DCE_MD2_VERTEX[i].w;
			DCE_MD2_VERTEX[i].x *= DCE_MD2_VERTEX[i].z;
			DCE_MD2_VERTEX[i].y *= DCE_MD2_VERTEX[i].z;
		}                       
	}
    
    tri  = &mdl->triangles[0];
    
    for(i = 0; i < mdl->header.num_tris; i++)
    {
        if((DCE_MD2_VERTEX_CLIP[tri->vertex[0]] + DCE_MD2_VERTEX_CLIP[tri->vertex[1]] + DCE_MD2_VERTEX_CLIP[tri->vertex[2]])) 
            mdl->clip_code[frame][i / 32] |= 0 << (i % 32); 
	  
        ++tri;
	}
}

void DCE_PreComputeMD2TexCoords(md2_mdl * mdl)
{
    int i;
    for(i = 0; i < mdl->header.num_st; i++)
    {
        mdl->uvcoords[i].u = (float)(mdl->texcoords[i].s / (float)mdl->header.skinwidth);
	    mdl->uvcoords[i].v = (float)(mdl->texcoords[i].t / (float)mdl->header.skinheight);
    }
    
    free(mdl->texcoords);
    mdl->texcoords = NULL;
}

/**
 * Load an HQMD2 model from file.
 *
 * Note: MD2 format stores model's data in little-endian ordering.  On
 * big-endian machines, you'll have to perform proper conversions.
 */
md2_mdl * DCE_LoadHQMD2 (const char *filename)
{
    FILE *fp;
    fp = fopen (filename, "rb");
    if (!fp)
    {
        fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
        return NULL;
    }
    
    md2_mdl * mdl = malloc(sizeof(md2_mdl));

    int i;

    /* Read header */
    fread (&mdl->header, 1, sizeof (struct md2_header_t), fp);

    if ((mdl->header.ident != 844121161) ||
      (mdl->header.version != 8))
    {
        /* Error! */
        fprintf (stderr, "Error: bad version or identifier\n");
        fclose (fp);
        free(mdl);
        return 0;
    }

    /* Memory allocations */
    mdl->skins = (struct md2_skin_t *)
    malloc (sizeof (struct md2_skin_t) * mdl->header.num_skins);
    mdl->texcoords = (struct md2_texCoord_t *)
    malloc (sizeof (struct md2_texCoord_t) * mdl->header.num_st);
    mdl->uvcoords = (struct md2_texCoord_uv *)
    malloc (sizeof (struct md2_texCoord_uv) * mdl->header.num_st);
    mdl->triangles = (struct md2_triangle_t *)
    malloc (sizeof (struct md2_triangle_t) * mdl->header.num_tris);
    mdl->frames = (struct md2_frame_t *)
    malloc (sizeof (struct md2_frame_t) * mdl->header.num_frames);
//    mdl->glcmds = (int *)malloc (sizeof (int) * mdl->header.num_glcmds);
    
    /* Read model data */
    fseek (fp, mdl->header.offset_skins, SEEK_SET);
    fread (mdl->skins, sizeof (struct md2_skin_t),
	 mdl->header.num_skins, fp);

    fseek (fp, mdl->header.offset_st, SEEK_SET);
    fread (mdl->texcoords, sizeof (struct md2_texCoord_t),
	 mdl->header.num_st, fp);

    fseek (fp, mdl->header.offset_tris, SEEK_SET);
    fread (mdl->triangles, sizeof (struct md2_triangle_t),
	 mdl->header.num_tris, fp);
/*
    fseek (fp, mdl->header.offset_glcmds, SEEK_SET);
    fread (mdl->glcmds, sizeof (int), mdl->header.num_glcmds, fp);
*/
    /* Read frames */
    fseek (fp, mdl->header.offset_frames, SEEK_SET);
    for (i = 0; i < mdl->header.num_frames; ++i)
    {
        /* Memory allocation for vertices of this frame */
        mdl->frames[i].verts = (struct hqmd2_vertex_t *)
        malloc (sizeof (struct hqmd2_vertex_t) * mdl->header.num_vertices);

        /* Read frame data */
        fread (mdl->frames[i].scale, sizeof (vec3_t), 1, fp);
        fread (mdl->frames[i].translate, sizeof (vec3_t), 1, fp);
        fread (mdl->frames[i].name, sizeof (char), 16, fp);
       // printf("MD2 FRAME %i: %s\n", i, mdl->frames[i].name );
        fread (mdl->frames[i].verts, sizeof (struct hqmd2_vertex_t),
	     mdl->header.num_vertices, fp);
	     
	    mdl->frames[i].name[3] = 0;
    }

    printf("\nMD2 Stats: %i Tris || %i Verts\n", mdl->header.num_tris, mdl->header.num_vertices);
    
    fclose (fp);
    
    DCE_PreComputeMD2TexCoords(mdl);
    
    return mdl;
}

md2_mdl * DCE_LoadHQMD2Clip(const char *filename)
{
    FILE *fp;
    fp = fopen (filename, "rb");
    if (!fp)
    {
        fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
        return NULL;
    }
    
    md2_mdl * mdl = malloc(sizeof(md2_mdl));

    int i;

    /* Read header */
    fread (&mdl->header, 1, sizeof (struct md2_header_t), fp);

    if ((mdl->header.ident != 844121161) ||
      (mdl->header.version != 8))
    {
        /* Error! */
        fprintf (stderr, "Error: bad version or identifier\n");
        fclose (fp);
        free(mdl);
        return 0;
    }

    /* Memory allocations */
    mdl->skins = (struct md2_skin_t *)
    malloc (sizeof (struct md2_skin_t) * mdl->header.num_skins);
    mdl->texcoords = (struct md2_texCoord_t *)
    malloc (sizeof (struct md2_texCoord_t) * mdl->header.num_st);
    mdl->uvcoords = (struct md2_texCoord_uv *)
    malloc (sizeof (struct md2_texCoord_uv) * mdl->header.num_st);
    mdl->triangles = (struct md2_triangle_t *)
    malloc (sizeof (struct md2_triangle_t) * mdl->header.num_tris);
    mdl->frames = (struct md2_frame_t *)
    malloc (sizeof (struct md2_frame_t) * mdl->header.num_frames);
//    mdl->glcmds = (int *)malloc (sizeof (int) * mdl->header.num_glcmds);
    
    /* Read model data */
    fseek (fp, mdl->header.offset_skins, SEEK_SET);
    fread (mdl->skins, sizeof (struct md2_skin_t),
	 mdl->header.num_skins, fp);

    fseek (fp, mdl->header.offset_st, SEEK_SET);
    fread (mdl->texcoords, sizeof (struct md2_texCoord_t),
	 mdl->header.num_st, fp);

    fseek (fp, mdl->header.offset_tris, SEEK_SET);
    fread (mdl->triangles, sizeof (struct md2_triangle_t),
	 mdl->header.num_tris, fp);
/*
    fseek (fp, mdl->header.offset_glcmds, SEEK_SET);
    fread (mdl->glcmds, sizeof (int), mdl->header.num_glcmds, fp);
*/
    /* Read frames */
    fseek (fp, mdl->header.offset_frames, SEEK_SET);
    for (i = 0; i < mdl->header.num_frames; ++i)
    {
        /* Memory allocation for vertices of this frame */
        mdl->frames[i].verts = (struct hqmd2_vertex_t *)
        malloc (sizeof (struct hqmd2_vertex_t) * mdl->header.num_vertices);

        /* Read frame data */
        fread (mdl->frames[i].scale, sizeof (vec3_t), 1, fp);
        fread (mdl->frames[i].translate, sizeof (vec3_t), 1, fp);
        fread (mdl->frames[i].name, sizeof (char), 16, fp);
       // printf("MD2 FRAME %i: %s\n", i, mdl->frames[i].name );
        fread (mdl->frames[i].verts, sizeof (struct hqmd2_vertex_t),
	     mdl->header.num_vertices, fp);
	     
	    mdl->frames[i].name[3] = 0;
    }

    printf("\nMD2 Stats: %i Tris || %i Verts\n", mdl->header.num_tris, mdl->header.num_vertices);
    
    fclose (fp);
    
    DCE_PreComputeMD2TexCoords(mdl);

    mdl->clip_code = malloc(mdl->header.num_frames * 4);

    for(i = 0; i < mdl->header.num_frames; i++)
    {
        mdl->clip_code[i] = malloc(mdl->header.num_tris / sizeof(unsigned int));
        
        DCE_RenderPlayerModelFPPSetMatrix(mdl, i);
        
		DCE_PreComputeHQMD2ClipCodes(mdl, i);
	}
    
    
    return mdl;
}

/**
 * Load an MD2 model from file.
 *
 * Note: MD2 format stores model's data in little-endian ordering.  On
 * big-endian machines, you'll have to perform proper conversions.
 */
md2_mdl * DCE_LoadMD2 (const char *filename)
{
    FILE *fp;
    fp = fopen (filename, "rb");
    if (!fp)
    {
        fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
        return NULL;
    }
    
    md2_mdl * mdl = malloc(sizeof(md2_mdl));

    int i;

    /* Read header */
    fread (&mdl->header, 1, sizeof (struct md2_header_t), fp);

    if ((mdl->header.ident != 844121161) ||
      (mdl->header.version != 8))
    {
        /* Error! */
        fprintf (stderr, "Error: bad version or identifier\n");
        fclose (fp);
        free(mdl);
        return 0;
    }

    /* Memory allocations */
    mdl->skins = (struct md2_skin_t *)
    malloc (sizeof (struct md2_skin_t) * mdl->header.num_skins);
    mdl->texcoords = (struct md2_texCoord_t *)
    malloc (sizeof (struct md2_texCoord_t) * mdl->header.num_st);
    mdl->uvcoords = (struct md2_texCoord_uv *)
    malloc (sizeof (struct md2_texCoord_uv) * mdl->header.num_st);
    mdl->triangles = (struct md2_triangle_t *)
    malloc (sizeof (struct md2_triangle_t) * mdl->header.num_tris);
    mdl->frames = (struct md2_frame_t *)
    malloc (sizeof (struct md2_frame_t) * mdl->header.num_frames);
//    mdl->glcmds = (int *)malloc (sizeof (int) * mdl->header.num_glcmds);
    
    /* Read model data */
    fseek (fp, mdl->header.offset_skins, SEEK_SET);
    fread (mdl->skins, sizeof (struct md2_skin_t),
	 mdl->header.num_skins, fp);

    fseek (fp, mdl->header.offset_st, SEEK_SET);
    fread (mdl->texcoords, sizeof (struct md2_texCoord_t),
	 mdl->header.num_st, fp);

    fseek (fp, mdl->header.offset_tris, SEEK_SET);
    fread (mdl->triangles, sizeof (struct md2_triangle_t),
	 mdl->header.num_tris, fp);
/*
    fseek (fp, mdl->header.offset_glcmds, SEEK_SET);
    fread (mdl->glcmds, sizeof (int), mdl->header.num_glcmds, fp);
*/
    /* Read frames */
    fseek (fp, mdl->header.offset_frames, SEEK_SET);
    for (i = 0; i < mdl->header.num_frames; ++i)
    {
        /* Memory allocation for vertices of this frame */
        mdl->frames[i].verts = (struct md2_vertex_t *)
        malloc (sizeof (struct md2_vertex_t) * mdl->header.num_vertices);

        /* Read frame data */
        fread (mdl->frames[i].scale, sizeof (vec3_t), 1, fp);
        fread (mdl->frames[i].translate, sizeof (vec3_t), 1, fp);
        fread (mdl->frames[i].name, sizeof (char), 16, fp);
        //printf("MD2 FRAME %i: %s\n", i, mdl->frames[i].name );
        fread (mdl->frames[i].verts, sizeof (struct md2_vertex_t),
	     mdl->header.num_vertices, fp);
    }
    
    printf("MD2 Stats: %i Tris || %i Verts\n", mdl->header.num_tris, mdl->header.num_vertices);
    
    fclose (fp);
    
    DCE_PreComputeMD2TexCoords(mdl);
    
    return mdl;
}

void DCE_DeleteMD2(md2_mdl *mdl)
{
    if(mdl == NULL)
        return;
        
	int i;
     
    for (i = 0; i < mdl->header.num_frames; ++i)
      free(mdl->frames[i].verts);

    free(mdl->skins);
    free(mdl->uvcoords);
    free(mdl->triangles);
    free(mdl->frames);
    
    mdl = NULL;
}

//----------------------------------------------------------------------------//


/*
** Unpack the indexed frame into a linear array of vertices
** Vertex buffer output is last parameter of type DCE_SimpleVertex
*/

void DCE_RenderCacheMD2Buf(md2_mdl * mdl, unsigned char frame, pvr_vertex_t * vert)
{
	unsigned int i;
	unsigned char j;
    struct md2_triangle_t * tri = &mdl->triangles[0];
    struct md2_frame_t *pframe = &mdl->frames[frame];
    struct md2_vertex_t *pvert;
    
    for(i = 0; i < mdl->header.num_tris; i++)
    {
        //For each vertex 
        for(j = 0; j < 3; j++)
        {
            pvert = &pframe->verts[tri->vertex[j]];

            // Compute texture coordinates
	        vert->u = mdl->uvcoords[tri->st[j]].u;
	        vert->v = mdl->uvcoords[tri->st[j]].v;

	        // Calculate vertex real position
	        vert->x = pvert->v[0];
	        vert->y = pvert->v[1];
	        vert->z = pvert->v[2];
            
			++vert;
        }
        
        ++tri;
	}
}



#include "dce.h"

void DCE_RenderHQMD2(md2_mdl * mdl, unsigned char frame)
{
	unsigned int i;

    struct md2_triangle_t * tri  = &mdl->triangles[0];
    struct hqmd2_frame_t *pframe = &mdl->frames[frame];
    struct hqmd2_vertex_t *pvert = &pframe->verts[0];

    pvr_dr_state_t dr_state;
    pvr_vertex_t * vert;
    
    pvr_dr_init(dr_state);

    for(i = 0; i < mdl->header.num_tris; i++)
    {
        //Vertex 0
        vert = pvr_dr_target(dr_state);       
        
	    vert->x = (float)pframe->verts[tri->vertex[0]].v[0];
	    vert->y = (float)pframe->verts[tri->vertex[0]].v[1];
	    vert->z = (float)pframe->verts[tri->vertex[0]].v[2];
            
        mat_trans_single(vert->x, vert->y, vert->z);    
            
	    vert->u = mdl->uvcoords[tri->st[0]].u;
	    vert->v = mdl->uvcoords[tri->st[0]].v;

        vert->argb = 0xFFFFFFFF;
         
		vert->flags = PVR_CMD_VERTEX;
        
        pvr_dr_commit(vert);

        //Vertex 1
        vert = pvr_dr_target(dr_state);       
        
	    vert->x = (float)pframe->verts[tri->vertex[1]].v[0];
	    vert->y = (float)pframe->verts[tri->vertex[1]].v[1];
	    vert->z = (float)pframe->verts[tri->vertex[1]].v[2];
            
        mat_trans_single(vert->x, vert->y, vert->z);    
            
	    vert->u = mdl->uvcoords[tri->st[1]].u;
	    vert->v = mdl->uvcoords[tri->st[1]].v;

        vert->argb = 0xFFFFFFFF;
         
		vert->flags = PVR_CMD_VERTEX;
        
        pvr_dr_commit(vert);

        //Vertex 2
        vert = pvr_dr_target(dr_state);       
        
	    vert->x = (float)pframe->verts[tri->vertex[2]].v[0];
	    vert->y = (float)pframe->verts[tri->vertex[2]].v[1];
	    vert->z = (float)pframe->verts[tri->vertex[2]].v[2];
            
        mat_trans_single(vert->x, vert->y, vert->z);    
            
	    vert->u = mdl->uvcoords[tri->st[2]].u;
	    vert->v = mdl->uvcoords[tri->st[2]].v;

        vert->argb = 0xFFFFFFFF;
         
		vert->flags = PVR_CMD_VERTEX_EOL;
        
        pvr_dr_commit(vert);

        ++tri;
	}
}

void DCE_RenderCacheIndexHQMD2(md2_mdl * mdl, unsigned char frame)
{
	unsigned int i;

    struct md2_triangle_t * tri  = &mdl->triangles[0];
    struct hqmd2_frame_t *pframe = &mdl->frames[frame];
    struct hqmd2_vertex_t *pvert = &pframe->verts[0];

    for(i = 0; i < mdl->header.num_vertices; i++)
    {
    	// The inline asm macro should perform this check and then division!!!
    	// mat_trans_single3_clip(x, y, z, c)
    	// or mat_trans_single3_nomod_clip(x, y, z, xo, yo, zo, c) lolz fuck yeah
    	mat_trans_single3_nodivw_nomod(pvert->v[0], pvert->v[1], pvert->v[2],
		                               DCE_MD2_VERTEX[i].x, DCE_MD2_VERTEX[i].y, DCE_MD2_VERTEX[i].z, DCE_MD2_VERTEX[i].w);
		        
		if(DCE_MD2_VERTEX[i].z > -1.0f) // Outside Z Near Plane -- Cull Vertex
		    DCE_MD2_VERTEX_CLIP[i] = 1;
		else                            // Inside Z Near Plane -- Perspective Divide Vertex
		{
			DCE_MD2_VERTEX_CLIP[i] = 0;
			
			DCE_MD2_VERTEX[i].z = 1.0f / DCE_MD2_VERTEX[i].w;
			DCE_MD2_VERTEX[i].x *= DCE_MD2_VERTEX[i].z;
			DCE_MD2_VERTEX[i].y *= DCE_MD2_VERTEX[i].z;
		}                       
	    
		++pvert;
	}
	
    pvr_dr_state_t dr_state;
    pvr_vertex_t * vert;
    
    pvr_dr_init(dr_state);
	    
    for(i = 0; i < mdl->header.num_tris; i++)
    {
      if(!(DCE_MD2_VERTEX_CLIP[tri->vertex[0]] + DCE_MD2_VERTEX_CLIP[tri->vertex[1]] + DCE_MD2_VERTEX_CLIP[tri->vertex[2]]))
	  {
		//Vertex 0
        vert = pvr_dr_target(dr_state);       
                  
        vert->x = DCE_MD2_VERTEX[tri->vertex[0]].x;
		vert->y = DCE_MD2_VERTEX[tri->vertex[0]].y;
        vert->z = DCE_MD2_VERTEX[tri->vertex[0]].z;
            
	    vert->u = mdl->uvcoords[tri->st[0]].u;
	    vert->v = mdl->uvcoords[tri->st[0]].v;

        vert->argb = 0xFFFFFFFF;
         
		vert->flags = PVR_CMD_VERTEX;
        
        pvr_dr_commit(vert);

        //Vertex 1
        vert = pvr_dr_target(dr_state);       
        
        vert->x = DCE_MD2_VERTEX[tri->vertex[1]].x;
		vert->y = DCE_MD2_VERTEX[tri->vertex[1]].y;
        vert->z = DCE_MD2_VERTEX[tri->vertex[1]].z;
            
	    vert->u = mdl->uvcoords[tri->st[1]].u;
	    vert->v = mdl->uvcoords[tri->st[1]].v;

        vert->argb = 0xFFFFFFFF;
         
		vert->flags = PVR_CMD_VERTEX;
        
        pvr_dr_commit(vert);

        //Vertex 2
        vert = pvr_dr_target(dr_state);       
        
        vert->x = DCE_MD2_VERTEX[tri->vertex[2]].x;
		vert->y = DCE_MD2_VERTEX[tri->vertex[2]].y;
        vert->z = DCE_MD2_VERTEX[tri->vertex[2]].z;
            
	    vert->u = mdl->uvcoords[tri->st[2]].u;
	    vert->v = mdl->uvcoords[tri->st[2]].v;

        vert->argb = 0xFFFFFFFF;
         
		vert->flags = PVR_CMD_VERTEX_EOL;
        
        pvr_dr_commit(vert);
	  }
	  
      ++tri;
	}
}

void DCE_RenderCacheIndexHQMD2_V2(md2_mdl * mdl, unsigned char frame)
{
	unsigned short int i;

    struct md2_triangle_t * tri  = &mdl->triangles[0];
    struct hqmd2_frame_t *pframe = &mdl->frames[frame];
    struct hqmd2_vertex_t *pvert = &pframe->verts[0];

 //   DCE_PreComputeHQMD2ClipCodes(mdl, frame);
    
    for(i = 0; i < mdl->header.num_vertices; i++)
    {
    	// The inline asm macro should perform this check and then division!!!
    	// mat_trans_single3_clip(x, y, z, c)
    	// or mat_trans_single3_nomod_clip(x, y, z, xo, yo, zo, c) lolz fuck yeah
    	mat_trans_single3_nodivw_nomod(pvert->v[0], pvert->v[1], pvert->v[2],
		                               DCE_MD2_VERTEX[i].x, DCE_MD2_VERTEX[i].y, DCE_MD2_VERTEX[i].z, DCE_MD2_VERTEX[i].w);
		        
		if(!(DCE_MD2_VERTEX[i].z > -1.0f)) // Inside Z Near Plane -- Perspective Divide Vertex                          
		{
			DCE_MD2_VERTEX[i].z = 1.0f / DCE_MD2_VERTEX[i].w;
			DCE_MD2_VERTEX[i].x *= DCE_MD2_VERTEX[i].z;
			DCE_MD2_VERTEX[i].y *= DCE_MD2_VERTEX[i].z;
		}                       
	    
		++pvert;
	}	

    pvr_dr_state_t dr_state;
    pvr_vertex_t * vert;
    
    pvr_dr_init(dr_state);

    for(i = 0; i < mdl->header.num_tris; i++)
    {
      if((mdl->clip_code[frame][(i / 32)] & (1 << (i % 32)))) 
	  {
		//Vertex 0
        vert = pvr_dr_target(dr_state);       
                  
        vert->x = DCE_MD2_VERTEX[tri->vertex[0]].x;
		vert->y = DCE_MD2_VERTEX[tri->vertex[0]].y;
        vert->z = DCE_MD2_VERTEX[tri->vertex[0]].z;
            
	    vert->u = mdl->uvcoords[tri->st[0]].u;
	    vert->v = mdl->uvcoords[tri->st[0]].v;

        vert->argb = 0xFFFFFFFF;
         
		vert->flags = PVR_CMD_VERTEX;
        
        pvr_dr_commit(vert);

        //Vertex 1
        vert = pvr_dr_target(dr_state);       
        
        vert->x = DCE_MD2_VERTEX[tri->vertex[1]].x;
		vert->y = DCE_MD2_VERTEX[tri->vertex[1]].y;
        vert->z = DCE_MD2_VERTEX[tri->vertex[1]].z;
            
	    vert->u = mdl->uvcoords[tri->st[1]].u;
	    vert->v = mdl->uvcoords[tri->st[1]].v;

        vert->argb = 0xFFFFFFFF;
         
		vert->flags = PVR_CMD_VERTEX;
        
        pvr_dr_commit(vert);

        //Vertex 2
        vert = pvr_dr_target(dr_state);       
        
        vert->x = DCE_MD2_VERTEX[tri->vertex[2]].x;
		vert->y = DCE_MD2_VERTEX[tri->vertex[2]].y;
        vert->z = DCE_MD2_VERTEX[tri->vertex[2]].z;
            
	    vert->u = mdl->uvcoords[tri->st[2]].u;
	    vert->v = mdl->uvcoords[tri->st[2]].v;

        vert->argb = 0xFFFFFFFF;
         
		vert->flags = PVR_CMD_VERTEX_EOL;
        
        pvr_dr_commit(vert);
	  }
	  
      ++tri;
	}
	//while(1)
	//printf("TRIS: %i / %i\n", tris, mdl->header.num_tris);
}

void DCE_RenderCacheIndexLerpHQMD2(md2_mdl * mdl, unsigned char frame, unsigned char min, unsigned char max, float l)
{
	unsigned int i, j;

    struct md2_triangle_t * tri  = &mdl->triangles[0];
    
	struct hqmd2_frame_t *pframe = &mdl->frames[frame];
    
    struct hqmd2_frame_t *pframe2 = &mdl->frames[frame + 1 > max ? max : frame + 1];
    
	struct hqmd2_vertex_t *pvert = &pframe->verts[0];

    pvr_dr_state_t dr_state;
    pvr_vertex_t * vert;
    
    pvr_dr_init(dr_state);

    for(i = 0; i < mdl->header.num_vertices; i++)
    {
    	// The inline asm macro should perform this check and then division!!!
    	// or mat_trans_single3_nomod_clip(x, y, z, xo, yo, zo, c) lolz fuck yeah
    	mat_trans_single3_nodivw_nomod(pvert->v[0], pvert->v[1], pvert->v[2],
		                               DCE_MD2_VERTEX[i].x, DCE_MD2_VERTEX[i].y, DCE_MD2_VERTEX[i].z, DCE_MD2_VERTEX[i].w);
		        
		if(DCE_MD2_VERTEX[i].z > -1.0f) // Outside Z Near Plane -- Clip Vertex
		    DCE_MD2_VERTEX_CLIP[i] = 1;
		else                            // Inside Z Near Plane -- Perspective Divide Vertex
		{
			DCE_MD2_VERTEX_CLIP[i] = 0;
			
			DCE_MD2_VERTEX[i].z = 1.0f / DCE_MD2_VERTEX[i].w;
			DCE_MD2_VERTEX[i].x *= DCE_MD2_VERTEX[i].z;
			DCE_MD2_VERTEX[i].y *= DCE_MD2_VERTEX[i].z;
		}                       
	    
		++pvert;
	}

    pvert = &pframe2->verts[0];

    for(j = 0; j < mdl->header.num_vertices; j++)
    {
    	i = j + 2048;
    	// The inline asm macro should perform this check and then division!!!
    	// or mat_trans_single3_nomod_clip(x, y, z, xo, yo, zo, c) lolz fuck yeah
    	mat_trans_single3_nodivw_nomod(pvert->v[0], pvert->v[1], pvert->v[2],
		                               DCE_MD2_VERTEX[i].x, DCE_MD2_VERTEX[i].y, DCE_MD2_VERTEX[i].z, DCE_MD2_VERTEX[i].w);
		        
		if(DCE_MD2_VERTEX[i].z > -1.0f) // Outside Z Near Plane -- Clip Vertex
		    DCE_MD2_VERTEX_CLIP[i] = 1;
		else                            // Inside Z Near Plane -- Perspective Divide Vertex
		{
			DCE_MD2_VERTEX_CLIP[i] = 0;
			
			DCE_MD2_VERTEX[i].z = 1.0f / DCE_MD2_VERTEX[i].w;
			DCE_MD2_VERTEX[i].x *= DCE_MD2_VERTEX[i].z;
			DCE_MD2_VERTEX[i].y *= DCE_MD2_VERTEX[i].z;
		}                       
	    
		++pvert;
	}
		    
    for(i = 0; i < mdl->header.num_tris; i++)
    {
      if(!(DCE_MD2_VERTEX_CLIP[tri->vertex[0]] + DCE_MD2_VERTEX_CLIP[tri->vertex[1]] + DCE_MD2_VERTEX_CLIP[tri->vertex[2]]))
	  {
		//Vertex 0
        vert = pvr_dr_target(dr_state);       
        
		vec3f_lerp(&DCE_MD2_VERTEX[tri->vertex[0]], &DCE_MD2_VERTEX[tri->vertex[0] + 2048], &vert->x, l);          

	    vert->u = mdl->uvcoords[tri->st[0]].u;
	    vert->v = mdl->uvcoords[tri->st[0]].v;

        vert->argb = 0xFFFFFFFF;
         
		vert->flags = PVR_CMD_VERTEX;
        
        pvr_dr_commit(vert);

        //Vertex 1
        vert = pvr_dr_target(dr_state);       
        
		vec3f_lerp(&DCE_MD2_VERTEX[tri->vertex[1]], &DCE_MD2_VERTEX[tri->vertex[1] + 2048], &vert->x, l); 
            
	    vert->u = mdl->uvcoords[tri->st[1]].u;
	    vert->v = mdl->uvcoords[tri->st[1]].v;

        vert->argb = 0xFFFFFFFF;
         
		vert->flags = PVR_CMD_VERTEX;
        
        pvr_dr_commit(vert);

        //Vertex 2
        vert = pvr_dr_target(dr_state);       
        
		vec3f_lerp(&DCE_MD2_VERTEX[tri->vertex[2]], &DCE_MD2_VERTEX[tri->vertex[2] + 2048], &vert->x, l); 
            
	    vert->u = mdl->uvcoords[tri->st[2]].u;
	    vert->v = mdl->uvcoords[tri->st[2]].v;

        vert->argb = 0xFFFFFFFF;
         
		vert->flags = PVR_CMD_VERTEX_EOL;
        
        pvr_dr_commit(vert);
	  }
	  
      ++tri;
	}
}
