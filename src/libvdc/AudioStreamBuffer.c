/**
 * AudioStreamBuffer.c (C) Josh Pearson 2014
 * 
 * This is a thread-safe buffer solution uses a circular buffer for storing
 * audio samples for stream data.
 * This ring buffer avoids the need to shift the data back in the
 * array every time data is read from the buffer. 
 */

#include <kos.h>

#include "AudioStreamBuffer.h"

/**
 * Reposition a pointer in the buffer, basically increment the position,
 * then adjust by the modulus of the buffer size.
 * 
 * @param abuf - the audio stream buffer object to manipulate
 * @param val - the value to reposition
 * @param step - the step of displacement
 * @return the new position in the buffer
 */
static unsigned int AudioStreamBuffer_Reposition( AudioStreamBuffer * abuf,
                                                  unsigned int val,
                                                  unsigned int step )
{
    return ( val + step ) % abuf->size;
}

AudioStreamBuffer * AudioStreamBuffer_NewObject( unsigned int size )
{
    AudioStreamBuffer * abuf = malloc( sizeof( AudioStreamBuffer ) );
    
    abuf->data = NULL;
    abuf->data = malloc( size );
    if( abuf->data == NULL )
    {
        printf("Malloc() ERROR\n");
        while(1);
    }
    
    abuf->size = size;
    abuf->readPos = abuf->writePos = abuf->bytes = 0;
    abuf->locked = 0;
  
    return abuf;
} 

void AudioStreamBuffer_FreeObject( AudioStreamBuffer * abuf )
{
    if( abuf->data )
    {
        free( abuf->data );
        abuf->data = NULL;
    }
    
    if( abuf )
        free( abuf );
    
    abuf = NULL;
}

void AudioStreamBuffer_Push( AudioStreamBuffer * abuf,
                             unsigned char * src,
                             unsigned int bytes )
{
    while( abuf->bytes + bytes > abuf->size )
        thd_pass();
    
    while( abuf->locked )
        thd_pass();
    
    abuf->locked = 1;

    abuf->bytes += bytes;
    
    int remaining = abuf->size - abuf->writePos;
    
    if( remaining < bytes )
    {
        bytes -= remaining;
        
        memcpy( abuf->data + abuf->writePos, src, remaining );
        memcpy( abuf->data, src + remaining, bytes );
        
        abuf->writePos = bytes;
    }
    else
    {
        memcpy( abuf->data + abuf->writePos, src, bytes );
        
        abuf->writePos = AudioStreamBuffer_Reposition( abuf, abuf->writePos, bytes );
    }
    
    abuf->locked = 0;
}

void AudioStreamBuffer_Pop( AudioStreamBuffer * abuf,
                            unsigned char * dst,
                            unsigned int bytes )
{
    while( abuf->bytes < bytes )
        thd_pass();

    while( abuf->locked )
        thd_pass();
  
    abuf->locked = 1;
    
    abuf->bytes -= bytes;
    
    int remaining = abuf->size - abuf->readPos;
    
    if( remaining < bytes )
    {
        bytes -= remaining;
        
        memcpy( dst, abuf->data + abuf->readPos, remaining );
        memcpy( dst + remaining, abuf->data, bytes );
        
        abuf->readPos = bytes;
    }
    else
    {
        memcpy( dst, abuf->data + abuf->readPos, bytes );
        
        abuf->readPos = AudioStreamBuffer_Reposition( abuf, abuf->readPos, bytes );
    }
    
    abuf->locked = 0;
}