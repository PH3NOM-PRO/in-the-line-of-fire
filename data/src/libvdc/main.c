/* 
   KallistiOS 2.0.0

   (c)2014 Josh Pearson.
*/

#include <kos.h>
#include <dc/pvr.h>

#include "LibXVID-Play.h"
#include "LibMPEG2-Play.h"
#include "GetTime.h"

#include "StreamBuffer.h"
#include "StreamRender.h"

void LibVDC_PlayMPEG( char * fileName )
{
    int width, height;

    if( LibMPEG2_InitFileStream( fileName, &width, &height ) )
        printf( "MPEG File Loaded OK...\nLibXVID Initialized...\n" );
    else
    {
        printf( "LibMPEG: Error loading file\n" );
        return;
    }
    
    StreamBuffer * sbuf = StreamBuffer_NewObject( width, height );
    
    StreamRender_Initialize( sbuf );

    StreamRender_Execute( sbuf, LibMPEG2_DecodeNextFrame, LibMPG123_Start );
    
    StreamBuffer_FreeObject( sbuf );    
}

void LibVDC_PlayXVID( char * fileName )
{
    int width, height;

    if( LibXVID_InitFileStream( fileName, &width, &height ) )
    {
        printf( "AVI File Loaded OK...\nLibXVID Initialized...\n" );
        printf( "AVI Video Resolution: %ix%i\n", width, height);
	}
    else
    {
        printf( "LibXVID: Error loading file\n" );
        return;
    }
    
    StreamBuffer * sbuf = StreamBuffer_NewObject( width, height );
    
    StreamRender_Initialize( sbuf );

    StreamRender_Execute( sbuf, LibXVID_DecodeNextFrame, LibMPG123_Start );
    
    StreamBuffer_FreeObject( sbuf );    
}

int main(int argc, char **argv) {

    pvr_init_defaults();
    
    //LibVDC_PlayXVID( "/cd/sample.avi" );
    LibVDC_PlayMPEG( "/cd/sample.mpg" );
    
    return 0;
}


