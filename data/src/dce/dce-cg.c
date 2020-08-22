void DCE_TexCoord3D(vec3f_t *d, float *s, float *t, unsigned char *face)
{
	vec3f_t absd;
    
	float sc, tc, ma;

	absd.x = fabs(d->x);
	absd.y = fabs(d->y);
	absd.z = fabs(d->z);

	*face = 0;

	if((absd.x >= absd.y) && (absd.x >= absd.z))
    {
		if(d->x > 0.0f)
        {
			*face = 0;
			sc = -d->z; tc = -d->y; ma = absd.x;
		}
        else
        {
			*face = 1;
			sc = d->z; tc = -d->y; ma = absd.x;
		}
	}

	if((absd.y >= absd.x) && (absd.y >= absd.z))
    {
		if(d->y > 0.0f)
        {
			*face = 2;
			sc = d->x; tc = d->z; ma = absd.y;
		}
        else
        {
			*face = 3;
			sc = d->x; tc = -d->z; ma = absd.y;
		}
	}

	if ((absd.z >= absd.x) && (absd.z >= absd.y)) {
		if (d->z > 0.0f) {
			*face = 4;
			sc = d->x; tc = -d->y; ma = absd.z;
		}
        else
        {
			*face = 5;
			sc = -d->x; tc = -d->y; ma = absd.z;
		}
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


extern void DCE_MatrixApplyTexture();

#define PHASE_UP 0
#define PHASE_DN 1

int PHASE = PHASE_UP;
int frame = 0;

void DCE_AnimateMesh()
{
    if(!(DCE_AnimationFrame() % 8))
    {
    float oft = 0.5f; 
    
    if(PHASE == PHASE_DN)
        oft = -oft;
        
    if(++frame == 3)
    {
         PHASE = !PHASE;
         frame = 0;
    }
         
    int vert = 0;    

    for(vert = 0; vert < ReflectionVertices; vert++)
        ReflectionMesh[vert].y += oft;
    }
}

void DCE_RenderProjectiveTexture(unsigned short * texID, unsigned int color)
{
    float uv[8];
  
    unsigned int argb[] = { color, color, color, color };
    
    vec3f_t N = { 0, 1, 0 };
    
    int vert = 0;        
     
    DCE_RenderCompileAndSubmitHeaderTextured(PVR_LIST_OP_POLY, texID, PVR_TXRENV_DECAL);
    
    for(vert = 0; vert < ReflectionVertices; vert += 4)
    {
        DCE_TextureCoordProjected(&player1.position, &ReflectionMesh[vert + 0], &N, &uv[0], &uv[1]);
        DCE_TextureCoordProjected(&player1.position, &ReflectionMesh[vert + 1], &N, &uv[2], &uv[3]);
        DCE_TextureCoordProjected(&player1.position, &ReflectionMesh[vert + 2], &N, &uv[4], &uv[5]);
        DCE_TextureCoordProjected(&player1.position, &ReflectionMesh[vert + 3], &N, &uv[6], &uv[7]);
         
        DCE_RenderTransformAndClipTriangleStrip(&ReflectionMesh[vert + 0], 3 * sizeof(float),
                                                uv, 2 * sizeof(float),
                                                argb, sizeof(unsigned int), 4);
    }    
}

void DCE_RenderCheapReflectMesh(DCE_Player * player, unsigned int color)
{
	float uv[8];
	               
    unsigned char face = 2, lface = 0;
    
    vec3f_t N = { 0, 1, 0 };
                     
    unsigned int argb[] = { color, color, color, color };

    int vert = 0;    

    for(vert = 0; vert < ReflectionVertices; vert += 4)
    {
        DCE_TextureCoord3D(&player->position, &ReflectionMesh[vert + 0], &N, &uv[0], &uv[1], &face);
        DCE_TextureCoord3D(&player->position, &ReflectionMesh[vert + 1], &N, &uv[2], &uv[3], &face);
        DCE_TextureCoord3D(&player->position, &ReflectionMesh[vert + 2], &N, &uv[4], &uv[5], &face);
        DCE_TextureCoord3D(&player->position, &ReflectionMesh[vert + 3], &N, &uv[6], &uv[7], &face);
        
        if(face != lface)
            DCE_SubmitSkyboxTextureHeader(face);
        
        DCE_RenderTransformAndClipTriangleStrip(&ReflectionMesh[vert + 0], 3 * sizeof(float),
                                                uv, 2 * sizeof(float),
                                                argb, sizeof(unsigned int), 4);
                                                
        lface = face;
    }
}

static vec3f_t * ReflectionMesh;
static int ReflectionVertices = 0;

void DCE_TesselateReflectionMesh(float xst, float zst, float xend, float zend, float size)
{
    int vertices = ((fabs(xst) + fabs(xend)) / size) * ((fabs(zst) + fabs(zend)) / size) * 4;
    ReflectionMesh = malloc(sizeof(vec3f_t) * vertices);
    vec3f_t * v = ReflectionMesh;
    
    int x, z;
    float y = 8.5f;
    
    for(x = xst; x < xend; x += size * 2)
    {
        for(z = zst; z < zend; z += size * 2)
        {
              v->x = x - size; v->y = y; v->z = z + size; v++;
              v->x = x + size; v->y = y; v->z = z + size; v++;
              v->x = x - size; v->y = y; v->z = z - size; v++;
              v->x = x + size; v->y = y; v->z = z - size; v++;
              
              ReflectionVertices += 4;
        }
    }
}
