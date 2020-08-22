/**
 * AudioStreamBuffer.h (C) Josh Pearson 2014
 * 
 * This is a thread-safe buffer solution uses a circular buffer for storing
 * audio samples for stream data.
 * This ring buffer avoids the need to shift the data back in the
 * array every time data is read from the buffer. 
 */

#ifndef AUDIO_STREAM_BUFFER_H
#define AUDIO_STREAM_BUFFER_H

typedef struct
{
    char * data;
    unsigned int size;
    unsigned int bytes;
    unsigned int readPos;
    unsigned int writePos;
    unsigned char locked;
} AudioStreamBuffer;

/**
 * Create a new AudioStreamBuffer object.
 * 
 * @param size - the size of the array to allocate for the object
 * @return - the new AudioStreamBuffer object ready for use.
 */
AudioStreamBuffer * AudioStreamBuffer_NewObject( unsigned int size );

/**
 * Free a previously created AudioStreamBuffer objet.
 * 
 * @param abuf - the object free the resources of.
 */
void AudioStreamBuffer_FreeObject( AudioStreamBuffer * abuf );

void AudioStreamBuffer_Push( AudioStreamBuffer * abuf,
                             unsigned char * src,
                             unsigned int bytes );

void AudioStreamBuffer_Pop( AudioStreamBuffer * abuf,
                            unsigned char * dst,
                            unsigned int bytes );

#endif
