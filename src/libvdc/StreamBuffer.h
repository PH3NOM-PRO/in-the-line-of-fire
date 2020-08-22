/**
 * StreamBuffer.h (C) Josh Pearson 2014
 * 
 * This is a thread-safe buffer solution intended for transporting video streams. 
 *
 * StreamBufferMaxFrames is used to control the number of Decoded Video Frames
 * stored in the buffer.  Lower values will work for lower CPU latency decoder
 * side, higher values recomened when CPU latency is high from the video decoder.
 */

#ifndef STREAM_BUFFER_H
#define STREAM_BUFFER_H

#define StreamBufferMaxFrames 10

typedef struct
{
    unsigned char *data[StreamBufferMaxFrames];
    int readPos;
    int writePos;
    int frames;
    int maxFrames;
    int imgWidth;
    int imgHeight;
    int texWidth;
    int texHeight;
    void *vramData;
    unsigned char locked;
} StreamBuffer;

typedef unsigned char *(*StreamBufferCallback)();

StreamBuffer * StreamBuffer_NewObject( int imgWidth, int imgHeight );

void StreamBuffer_FreeObject( StreamBuffer * sbuf );

void StreamBuffer_Push( StreamBuffer * sbuf, unsigned char *data );

unsigned char* StreamBuffer_Pop( StreamBuffer * sbuf );

int StreamBuffer_PreFill( StreamBuffer * sbuf, StreamBufferCallback callback );

unsigned char StreamBuffer_HasFrames( StreamBuffer * sbuf );

#endif
