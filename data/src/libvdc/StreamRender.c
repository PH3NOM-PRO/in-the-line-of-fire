#include <kos.h>
#include <dc/pvr.h>

#include "GetTime.h"

#include "StreamRender.h"

#include "LibMPEG2-Play.h"

/* Generic PVR Commands */
typedef struct {
    unsigned int cmd[8];
} pvr_cmd_t; /* Generic 32byte command for the pvr */

#define VERTEX_COUNT 5

static pvr_cmd_t VERTEX_BUFFER[VERTEX_COUNT];
static pvr_poly_cxt_t PVR_POLY_CXT;

static StreamBuffer * sbuf;
static unsigned char decoder_active = 0;

void StreamTexturePVR( StreamBuffer * sbuf )
{
    int i, n;
    unsigned int *s = (unsigned int *)StreamBuffer_Pop(sbuf);
    unsigned int *d = (unsigned int *)(void *)
		(0xe0000000 | (((unsigned long)sbuf->vramData) & 0x03ffffe0));
    uint32 count = sbuf->imgWidth * 2; 
   
    /* Set store queue memory area as desired */
    QACR0 = ((((unsigned int)sbuf->vramData)>>26)<<2)&0x1c;
    QACR1 = ((((unsigned int)sbuf->vramData)>>26)<<2)&0x1c;

    for( i = 0; i < sbuf->imgHeight; i++ )
    {                        
	  d = (unsigned int *)(void *)
		(0xe0000000 | (((unsigned long)sbuf->vramData+(i*(sbuf->texWidth * 2))) & 0x03ffffe0));
	  
	  n = count>>5;
	  
	  while(n--) {
		asm("pref @%0" : : "r" (s + 8)); // prefetch 32 bytes for next loop 
		d[0] = *(s++);
		d[1] = *(s++);
		d[2] = *(s++);
		d[3] = *(s++);
		d[4] = *(s++);
		d[5] = *(s++);
		d[6] = *(s++);
		d[7] = *(s++);
		asm("pref @%0" : : "r" (d));
		d += 8;
	  }
	}
	/* Wait for both store queues to complete */
	d = (unsigned int *)0xe0000000;
	d[0] = d[8] = 0;    
}

void StreamRender_DisplayFrame( StreamBuffer * sbuf )
{
    while(!sbuf->frames)
        thd_pass();
    
    printf("RenderFrame: Buffer Contains %i frames\n", sbuf->frames );
    
    while(sbuf->locked)
        thd_pass();
    sbuf->locked = 1;
    
    StreamTexturePVR( sbuf );
    
    sbuf->locked = 0;
    
    pvr_wait_ready();
    
    pvr_scene_begin();
    
    pvr_list_begin( PVR_LIST_OP_POLY );

    sq_cpy( (void*)0x10000000, VERTEX_BUFFER, VERTEX_COUNT * 32 );
    
    pvr_list_finish();

    pvr_scene_finish();
}

#define screenWidth 640
#define screenHeight 480
#define UV_EPSILON 0.001f

void StreamRender_Initialize( StreamBuffer * sbuf )
{      
    int top = 0;
    int bottom = screenHeight;
    int left = 0;
    int right = screenWidth;
    int height = 480;//( screenWidth * sbuf->imgHeight ) / sbuf->imgWidth;
/*
    if( sbuf->imgWidth > sbuf->imgHeight )
    {
        top = ( screenHeight - height ) / 2.0f;
        bottom = top + height;
    }
    else
    {
        
    }
 */   
    pvr_vertex_t * vptr = (pvr_vertex_t *)&VERTEX_BUFFER[1];
    
    vptr->x = left;
    vptr->y = top;
    vptr->z = 1;
    vptr->u = UV_EPSILON;
    vptr->v = UV_EPSILON;
    vptr->argb = 0xFFFFFFFF;
    vptr->flags = PVR_CMD_VERTEX;
    ++vptr;
    
    vptr->x = right;
    vptr->y = top;
    vptr->z = 1;
    vptr->u = ((float)sbuf->imgWidth / sbuf->texWidth) - UV_EPSILON;
    vptr->v = UV_EPSILON;    
    vptr->argb = 0xFFFFFFFF;
    vptr->flags = PVR_CMD_VERTEX;    
    ++vptr;
    
    vptr->x = left;
    vptr->y = bottom;
    vptr->z = 1;
    vptr->u = UV_EPSILON;
    vptr->v = ((float)sbuf->imgHeight / sbuf->texHeight) - UV_EPSILON;     
    vptr->argb = 0xFFFFFFFF;
    vptr->flags = PVR_CMD_VERTEX;   
    ++vptr;
    
    vptr->x = right;
    vptr->y = bottom;
    vptr->z = 1;
    vptr->u = ((float)sbuf->imgWidth / sbuf->texWidth) - UV_EPSILON;
    vptr->v = ((float)sbuf->imgHeight / sbuf->texHeight) - UV_EPSILON;     
    vptr->argb = 0xFFFFFFFF;
    vptr->flags = PVR_CMD_VERTEX_EOL; 
    
    pvr_poly_hdr_t *hdr = (pvr_poly_hdr_t *)&VERTEX_BUFFER[0];

    pvr_poly_cxt_txr( &PVR_POLY_CXT,
                      0,
                      PVR_TXRFMT_YUV422 | PVR_TXRFMT_NONTWIDDLED,
                      sbuf->texWidth,
                      sbuf->texHeight,
                      sbuf->vramData,
                      PVR_FILTER_BILINEAR );  
    
    pvr_poly_compile( hdr, &PVR_POLY_CXT );   
}

/*****************************************************************************/

void * RenderThread()
{
    float frameTime = 1000.0f / 29.97;//23.969;
    
    float frameEnd = (float)GetTime() + frameTime;
    
    while(decoder_active)
    {
        StreamRender_DisplayFrame( sbuf );

        while( GetTime() < frameEnd )
           thd_pass();
        
        //if( GetTime() < frameEnd )
        //    thd_sleep( frameEnd - GetTime() );       
        
        frameEnd += frameTime;
    }
    
    return NULL;
}

extern int InputCallbackX();

void StreamRender_Execute( StreamBuffer * inSbuf,
                           StreamBufferCallback callback,
                           AudioStreamCallback audCallback )
{
    sbuf = inSbuf;
    
    StreamBuffer_PreFill( sbuf, callback );
    
    unsigned char * buffer = callback();
    
    decoder_active = 1;
    
    thd_create( 0, RenderThread, NULL );
    
    audCallback();
    
    while( buffer != NULL )
    {
        StreamBuffer_Push( sbuf, buffer );
        
        buffer = callback();
        
        if(InputCallbackX())
            goto done;
    }
    
    while( StreamBuffer_HasFrames( sbuf ) )
        thd_sleep(5);
    
    done:
    
    LibMPEG2_Exit();
    
    decoder_active = 0;    
}
