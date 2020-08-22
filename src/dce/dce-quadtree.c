/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#include "malloc.h"
#include "stdlib.h"
#include "string.h"

#include "dce.h"
#include "dce-render-object.h"

#define DCE_QUAD_TREE_MAX_NODES 128
#define DCE_QUAD_TREE_MAX_BUF   224

static DCE_CollisionQuadTreeNode * QTNODE[1024*32];
static unsigned int QTNODES = 0;

extern int DCE_TraceRayCollisionTri( const vec3f_t * V1,  // Triangle vertices
                              const vec3f_t * E1,  // Edge 1
                              const vec3f_t * E2,  // Edge 2
                              const vec3f_t * O,   // Ray origin
                              const vec3f_t * D,   // Ray direction
                              float * out );

void DCE_CopyCollisionTri(DCE_CollisionTriangle * dst, DCE_CollisionTriangle * src)
{
	*dst = *src;
}

unsigned char DCE_QuadTreeFindSide(DCE_CollisionQuadTreeNode * node, vec3f * mins, vec3f * maxs)
{
	float cx = node->bbmin.x + node->half_width;

	if(maxs->x <= cx)
		return 1;

	if(mins->x >= cx)
	    return 2;

    return 0;
}

unsigned char DCE_QuadTreeFindQuadrant(DCE_CollisionQuadTreeNode * node, vec3f * mins, vec3f * maxs)
{
	float cx = node->bbmin.x + node->half_width;
	float cz = node->bbmin.z + node->half_depth;

	if(maxs->x <= cx && maxs->z <= cz)
		return 1;

	if(mins->x >= cx && maxs->z <= cz)
	    return 2;

	if(mins->z >= cz && maxs->x <= cx)
		return 3;
	
	if(mins->z >= cz && mins->x >= cx)
		return 4;

    return 0;
}

unsigned char DCE_QuadTreeFindPointQuadrant(DCE_CollisionQuadTreeNode * node, vec3f * p)
{
	float cx = node->bbmin.x + node->half_width;
	float cz = node->bbmin.z + node->half_depth;

	if(p->x < cx && p->z < cz)
		return 1;

	if(p->x > cx && p->z < cz)
	    return 2;

	if(p->z > cz && p->x < cx)
		return 3;
	
	if(p->z > cz && p->x > cx)
		return 4;

    return 0;
}

unsigned char DCE_QuadTreeFindRayPointQuadrant(DCE_CollisionQuadTreeNode * node, vec3f * p)
{
	float cx = node->bbmin.x + node->half_width;
	float cz = node->bbmin.z + node->half_depth;

    if(p->x < node->bbmin.x)
        return 5; // Outside, Left
        
    if(p->z < node->bbmin.z)
        return 6; // Outside, Top
        
    if(p->x > node->bbmax.x)
        return 7; // Outside, Right    

    if(p->z > node->bbmax.z)
        return 8; // Outside, Bottom 

	if(p->x < cx && p->z < cz)
		return 1;

	if(p->x > cx && p->z < cz)
	    return 2;

	if(p->z > cz && p->x < cx)
		return 3;
	
	if(p->z > cz && p->x > cx)
		return 4;

    return 0;
}

DCE_CollisionQuadTreeNode * DCE_QuadTreeNewParentNode(float minx, float miny, float minz,
                                                float maxx, float maxy, float maxz, float w, float d)
{
    DCE_CollisionQuadTreeNode * node = (DCE_CollisionQuadTreeNode *)malloc(sizeof(DCE_CollisionQuadTreeNode));
    
    node->parent = NULL;
    
	node->half_width = w;
	node->half_depth = d;

	node->bbmin.x = minx;
	node->bbmin.y = miny;
	node->bbmin.z = minz;

	node->bbmax.x = maxx;
	node->bbmax.y = maxy;
	node->bbmax.z = maxz;

	node->nodes = 0;

	node->triangles = 0;

	node->tris = (DCE_CollisionTriangle *)malloc(sizeof(DCE_CollisionTriangle) * DCE_QUAD_TREE_MAX_BUF * 2);

	return node;
}


DCE_CollisionQuadTreeNode * DCE_QuadTreeNewNode(DCE_CollisionQuadTreeNode * parent, float minx, float miny, float minz,
                                                float maxx, float maxy, float maxz, float w, float d)
{
    DCE_CollisionQuadTreeNode * node = (DCE_CollisionQuadTreeNode *)malloc(sizeof(DCE_CollisionQuadTreeNode));
    
    node->parent = parent;
    
	node->half_width = w;
	node->half_depth = d;

	node->bbmin.x = minx;
	node->bbmin.y = miny;
	node->bbmin.z = minz;

	node->bbmax.x = maxx;
	node->bbmax.y = maxy;
	node->bbmax.z = maxz;

	node->nodes = 0;

	node->triangles = 0;

	node->tris = (DCE_CollisionTriangle *)malloc(sizeof(DCE_CollisionTriangle) * DCE_QUAD_TREE_MAX_BUF);

	return node;
}

void DCE_QuadTreeRemove(DCE_CollisionQuadTreeNode * qt, unsigned int index)
{
	unsigned int i = 0;

	for(i = index; i < qt->triangles - 1; i++)
		DCE_CopyCollisionTri(&qt->tris[i], &qt->tris[i + 1]);

	--qt->triangles;
}

void DCE_QuadTreeSplit(DCE_CollisionQuadTreeNode * qt)
{
	if(!qt->nodes)
	{
	    qt->nodes = 4;

		float cx = qt->bbmin.x + qt->half_width;  // X Center
		float cz = qt->bbmin.z + qt->half_depth;  // Z Center

		float w2 = qt->half_width * 0.5f;         // Half of the Half Width
		float d2 = qt->half_depth * 0.5f;         // Half of the Half Depth

		QTNODE[QTNODES++] = qt->node[0] = DCE_QuadTreeNewNode(qt, qt->bbmin.x, qt->bbmin.y, qt->bbmin.z, cx, qt->bbmax.y, cz, w2, d2);

		QTNODE[QTNODES++] = qt->node[1] = DCE_QuadTreeNewNode(qt, cx, qt->bbmin.y, qt->bbmin.z, qt->bbmax.x, qt->bbmax.y, cz, w2, d2);

		QTNODE[QTNODES++] = qt->node[2] = DCE_QuadTreeNewNode(qt, qt->bbmin.x, qt->bbmin.y, cz, cx, qt->bbmax.y, qt->bbmax.z, w2, d2);

		QTNODE[QTNODES++] = qt->node[3] = DCE_QuadTreeNewNode(qt, cx, qt->bbmin.y, cz, qt->bbmax.x, qt->bbmax.y, qt->bbmax.z, w2, d2);
	}

	unsigned int i;
	for(i = 0; i < qt->triangles; )
	{
		unsigned char index = DCE_QuadTreeFindQuadrant(qt, &qt->tris[i].bbmin, &qt->tris[i].bbmax);
		if(index)
		{
			DCE_QuadTreeInsert((DCE_CollisionQuadTreeNode *)qt->node[index - 1], &qt->tris[i]);
			DCE_QuadTreeRemove(qt, i);
		}
		else
		    i++;
	}
}

void DCE_QuadTreeInsert(DCE_CollisionQuadTreeNode * qt, DCE_CollisionTriangle * tri)
{
	DCE_CopyCollisionTri(&qt->tris[qt->triangles++], tri);

	if(qt->triangles >= DCE_QUAD_TREE_MAX_NODES)
		DCE_QuadTreeSplit(qt);
}

void DCE_TraceQuadTreeNodes(DCE_CollisionQuadTreeNode * qt, vec3f * p, vec3f * r, float * d)
{
	unsigned short int i;
	float cd;// collision distance
	
	for(i = 0; i < qt->triangles; i++)
	{
        DCE_RenderBoundingBox(&qt->tris[i].bbmin, &qt->tris[i].bbmax, 2, 0xFFFF0000);
        
	    if(DCE_TraceRayCollisionTri(&qt->tris[i].v1,         // Triangle vertices
                                        &qt->tris[i].edge1,  // Edge 1
                                        &qt->tris[i].edge2,  // Edge 2
                                        p,                   // Ray origin
                                        r,                   // Ray direction
                                        &cd))                // Collision Distance
            if(cd < *d)                                      // Check to find Nearest Collision Distance
                *d = cd;
    }
    
	if(!qt->nodes)
	    return;
	
	unsigned char index = DCE_QuadTreeFindPointQuadrant(qt, p);
	
	if(index)
	    DCE_TraceQuadTreeNodes((DCE_CollisionQuadTreeNode *)qt->node[index - 1], p, r, d);
	else
	    return;
}

#define DCE_QUADTREE_RAY_DELTA 0.1f

inline void DCE_QuadTreeStepRay(DCE_CollisionQuadTreeNode * qt, vec3f * o, vec3f * d)
{
	float tx = 0, tx0 = 0, tx1 = 0;
	float tz = 0, tz0 = 0, tz1 = 0;
	float t;
	
	if(d->x) // Beware of axis-alligned ray direction
	{
		tx0 = (qt->bbmin.x - o->x) / d->x;
		tx1 = (qt->bbmax.x - o->x) / d->x;
		tx = max(tx0, tx1);
	}
	if(d->z)
	{
		tz0 = (qt->bbmin.z - o->z) / d->z;
		tz1 = (qt->bbmax.z - o->z) / d->z;
		tz = max(tz0, tz1);
	}

	if(tx <= 0)
	    t = tz;
	else if(tz <= 0)
	    t = tx;
	else
	    t = min(tx, tz);
	
	t += DCE_QUADTREE_RAY_DELTA;
    
	o->x += d->x * t;
	o->y += d->y * t;
	o->z += d->z * t;	
}

/* Non-Recursive Method for Searching a Point in a QuadTree */
void DCE_QuadTreeTracePoint(DCE_CollisionQuadTreeNode * qt, vec3f * p, vec3f * r, float * d)
{
	unsigned short int i;
	float cd;// collision distance
	
    while(qt->nodes) /* Navigate QuadTree From Root Node Down to Leaf Node */
    {
		for(i = 0; i < qt->triangles; i++)
	        if(DCE_TraceRayCollisionTri(&qt->tris[i].v1,     // Triangle vertices
                                        &qt->tris[i].edge1,  // Edge 1
                                        &qt->tris[i].edge2,  // Edge 2
                                        p,                   // Ray origin
                                        r,                   // Ray direction
                                        &cd))                // Collision Distance
                if(cd < *d)                                  // Check to find Nearest Collision Distance
                    *d = cd;

	    unsigned char index = DCE_QuadTreeFindPointQuadrant(qt, p); // Get Quadrant in this node that point belongs to
	    
	    if(index)
	        qt = (DCE_CollisionQuadTreeNode *)qt->node[index - 1]; // Link Pointer to Quadrant Node
	    else
	        break;                                                 // We have reached the Leaf Node
    }

    for(i = 0; i < qt->triangles; i++) /* Now Check Leaf Node */
	    if(DCE_TraceRayCollisionTri(&qt->tris[i].v1,         // Triangle vertices
                                        &qt->tris[i].edge1,  // Edge 1
                                        &qt->tris[i].edge2,  // Edge 2
                                        p,                   // Ray origin
                                        r,                   // Ray direction
                                        &cd))                // Collision Distance
            if(cd < *d)                                      // Check to find Nearest Collision Distance
                *d = cd;
}

void DCE_QuadTreeCheckNodeCollision(DCE_CollisionQuadTreeNode * qt, vec3f * p, vec3f * r, float * d)
{
    unsigned short int i;
	float cd;

	for(i = 0; i < qt->triangles; i++) /* Now Check Leaf Node */
    {
	    /**/
	    //DCE_RenderBoundingBox(&qt->tris[i].bbmin, &qt->tris[i].bbmax, 2, 0xFFFF0000);
	    
		if(DCE_TraceRayCollisionTri(&qt->tris[i].v1,     // Triangle vertices
                                    &qt->tris[i].edge1,  // Edge 1
                                    &qt->tris[i].edge2,  // Edge 2
                                    p,                   // Ray origin
                                    r,                   // Ray direction
                                    &cd))                // Collision Distance
        {
            //DCE_RenderBoundingBox(&qt->tris[i].bbmin, &qt->tris[i].bbmax, 2, 0xFF00FF00);
            
			if(cd < *d)                                    // Check to find Nearest Collision Distance
                *d = cd;
        }
    }
}

/* Non-Recursive Method for Tracing a Ray in a QuadTree */
void DCE_QuadTreeTraceRay(DCE_CollisionQuadTreeNode * qt, vec3f * p, vec3f * r, float * d)
{
	unsigned char index;  // QuadTree Node Index
	unsigned short int i; // Loop Counter
	float cd;             // Collision Distance
	
	LOOP_START:
		
    while(qt->nodes) /* Navigate QuadTree Down to Leaf Node */
    {
	    index = DCE_QuadTreeFindPointQuadrant(qt, p); // Get Quadrant in this node that point belongs to
	    
	    if(index)
	        qt = (DCE_CollisionQuadTreeNode *)qt->node[index - 1]; // Link Pointer to Quadrant Node
	    else
	        break;                                                 // We have reached the Leaf Node
    }

    DCE_QuadTreeCheckNodeCollision(qt, p, r, d); /* Check this Leaf Node for Collisions */
	    
    DCE_QuadTreeStepRay(qt, p, r); /* Step the Ray in its Direction just far enough to exit this Leaf Node Bounding Box */

    while(qt->parent) /* Navigate From Leaf Node Up the QuadTree untill the Ray is inside Node Bounding Box */
    {
    	qt = qt->parent;
    	
    	DCE_QuadTreeCheckNodeCollision(qt, p, r, d);
    	
    	index = DCE_QuadTreeFindRayPointQuadrant(qt, p);
    	
    	if(!index)
    	    break;

        if(index < 5)
            break;
	}
	
	if(qt->parent == NULL && index > 4)
	    return;
	
	goto LOOP_START;
}

unsigned int DCE_QuadTreeFindCollision(DCE_CollisionQuadTreeNode * qt, vec3f * p, vec3f * r, float * d)
{  
    float cd = 10000; // Max Collision Distance
    
    DCE_QuadTreeTracePoint((DCE_CollisionQuadTreeNode *)qt, p, r, &cd);
	    
    if(cd != 10000)
    {
        *d = cd;
        return 1;
	}
    
	return 0;
}

unsigned int DCE_QuadTreeTraceRayCollision(DCE_CollisionQuadTreeNode * qt, vec3f * p, vec3f * r, float * d)
{  
    float cd = 10000; // Max Collision Distance
    
    DCE_QuadTreeTraceRay((DCE_CollisionQuadTreeNode *)qt, p, r, &cd);
	    
    if(cd != 10000)
    {
        *d = cd;
        return 1;
	}
    
	return 0;
}

DCE_CollisionQuadTreeNode * DCE_QuadTreeLoad(char * fname)
{
    FILE * F = NULL;
	F = fopen(fname, "rb");
    if(F == NULL)
    {
    	printf("Error Opening File %s\n", fname);
    	return NULL;
	}
    
    DCE_CollisionQuadTreeNode * cmesh = malloc(sizeof(DCE_CollisionQuadTreeNode));
    DCE_CollisionQuadTreeNode * qt;
    
	fread(cmesh, 1, sizeof(DCE_CollisionQuadTreeNode) - (4 * 6), F);

    printf("Collision Mesh Triangles: %i\n", cmesh->triangles);
    printf("Collision Mesh Width: %f\n", cmesh->half_width * 2);    
    printf("Collision Mesh Depth: %f\n", cmesh->half_depth * 2);  

	cmesh->tris = malloc(sizeof(DCE_CollisionTriangle) * cmesh->triangles);
    
	fseek(F, sizeof(DCE_CollisionQuadTreeNode) - (4 * 6), SEEK_SET);
		 
	fread(cmesh->tris, sizeof(DCE_CollisionTriangle), cmesh->triangles, F);
	
	fclose(F);
		
	printf("\n\nEDGEs2: %f %f %f\n\n", cmesh->tris[0].edge1.x, cmesh->tris[0].edge1.y, cmesh->tris[0].edge1.z);	
	printf("\n\nV2: %f %f %f\n\n", cmesh->tris[0].edge2.x, cmesh->tris[0].edge2.y, cmesh->tris[0].edge2.z);	
	
	qt = DCE_QuadTreeNewParentNode(cmesh->bbmin.x, cmesh->bbmin.y, cmesh->bbmin.z,
	                         cmesh->bbmax.x, cmesh->bbmax.y, cmesh->bbmax.z, cmesh->half_width, cmesh->half_depth);
    
    int n;
	for(n = 0; n < cmesh->triangles; n++)
        DCE_QuadTreeInsert(qt, &cmesh->tris[n]);
    
    free(cmesh->tris);
    free(cmesh);
    
    return qt;
}
