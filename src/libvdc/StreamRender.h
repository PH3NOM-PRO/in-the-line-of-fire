#ifndef STREAM_RENDER_H
#define STREAM_RENDER_H

#include "StreamBuffer.h"

typedef void (*AudioStreamCallback)();

void StreamRender_DisplayFrame( StreamBuffer * sbuf );
void StreamRender_Initialize( StreamBuffer * sbuf );

void StreamRender_Execute( StreamBuffer * inSbuf,
                           StreamBufferCallback callback,
                           AudioStreamCallback audCallback );

#endif