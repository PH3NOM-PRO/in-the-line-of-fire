/**
 * StreamBuffer.h (C) Josh Pearson 2014
 * 
 * This is a thread-safe buffer solution intended for transporting video streams. 
 */

#include <kos.h>

#include <malloc.h>

#include "StreamBuffer.h"

static int StreamBuffer_NextFrame( StreamBuffer * sbuf, int frame )
{
   return frame == sbuf->maxFrames - 1 ? 0 : ++frame; 
}

void StreamBuffer_FreeObject( StreamBuffer * sbuf )
{
    if(sbuf == NULL)
        return;
    
    unsigned char f;
    for( f = 0; f < sbuf->maxFrames; f++)
        if( sbuf->data[f] != NULL)
            free(sbuf->data[f]);
    
    if(sbuf->vramData != NULL)
        pvr_mem_free(sbuf->vramData);
    
    free(sbuf);
    sbuf = NULL;
    
    return;
}

StreamBuffer * StreamBuffer_NewObject( int imgWidth, int imgHeight )
{
    StreamBuffer * sbuf = malloc(sizeof(StreamBuffer));

    sbuf->frames = sbuf->maxFrames = sbuf->readPos = sbuf->writePos = 0;
    sbuf->locked = 0;

    sbuf->imgWidth = imgWidth;
    sbuf->imgHeight = imgHeight;
    
    sbuf->texWidth = sbuf->texHeight = 0x8;

    while( sbuf->texWidth < sbuf->imgWidth )
        sbuf->texWidth <<= 1;
    while( sbuf->texHeight < sbuf->imgHeight )
        sbuf->texHeight <<= 1;

    sbuf->vramData = pvr_mem_malloc(sbuf->texWidth * sbuf->texHeight * 2 );
    if(sbuf->vramData == NULL)
    {
        printf("PVR MEMORY ALLOCATION FAILURE\n");
        while(1);
    }
    
    unsigned char f;
    for( f = 0; f < StreamBufferMaxFrames; f++)
    {
        sbuf->data[f] = NULL;
        sbuf->data[f] = malloc( imgWidth * imgHeight * 2 );
        
        if(sbuf->data[f] == NULL)
        {
            printf("StreamBuffer: Null allocation at frame %i\n", sbuf->maxFrames + 1 );
            break;
        }
        ++sbuf->maxFrames;
    }
    
    printf("StreamBuffer: Allocated %i frames\n", sbuf->maxFrames );
    
    return sbuf;
}

void StreamBuffer_Push( StreamBuffer * sbuf, unsigned char *data )
{
    while(sbuf->frames == sbuf->maxFrames)
        thd_sleep(2);

#ifdef VERBOSE    
    printf("PushFrame, Have %i, Write %i\n", sbuf->frames, sbuf->writePos );
#endif
    while(sbuf->locked)
        thd_pass();
    sbuf->locked = 1;

    memcpy( sbuf->data[sbuf->writePos], data, sbuf->imgWidth * sbuf->imgHeight * 2 );
    
    sbuf->writePos = StreamBuffer_NextFrame(sbuf, sbuf->writePos);
    ++sbuf->frames;
    
    sbuf->locked = 0;
}

unsigned char* StreamBuffer_Pop( StreamBuffer * sbuf )
{
    while( !sbuf->frames )
        thd_pass();    
#ifdef VERBOSE      
    printf("Pop Frame %i\n", sbuf->readPos );
#endif    
    unsigned char * ptr = sbuf->data[ sbuf->readPos ];
    
    sbuf->readPos = StreamBuffer_NextFrame( sbuf, sbuf->readPos );
    --sbuf->frames;
    
    return ptr;
}

int StreamBuffer_PreFill( StreamBuffer * sbuf, StreamBufferCallback callback )
{
    unsigned char * buffer;
    
    while(sbuf->frames < sbuf->maxFrames)
    {
        buffer = callback();
        
        if(buffer == NULL)
        {
            printf("Unable to fill buffer: NULL pointer\n");
            return 0;
        }
        
        StreamBuffer_Push( sbuf, buffer );
    }    
    
    printf("StreamBufferPrefill: %i frames\n", sbuf->frames );
    
    return 1;
}

unsigned char StreamBuffer_HasFrames( StreamBuffer * sbuf )
{
    return sbuf->frames > 0;
}
