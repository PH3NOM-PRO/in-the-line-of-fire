/*
**
** This file is a part of Dreamcast Media Center
** (C) Josh PH3NOM Pearson 2011-2013
**
*/
/*
**  LibMPG123 streaming decoder (C) Josh 'PH3NOM' Pearson.
**  This decoder demonstrates the use of mpg123_open_feed() decoding.
**  In this mode, we must manually feed the input buffer.
**  This algorithm is based around decoding "Chunks" of bitstream data.
**  I.E., reading chunks of bitstream from an interleaved AVI or MPEG file.
*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "stdio.h"

#include "mpg123.h"

#include "LibMPG123.h"

#include "snddrv.h"

#include "AudioStreamBuffer.h"

/* Global variables for the LibMPG123 decoder */
static mpg123_handle * mh = NULL;

static int enc, err;

static volatile int MPG123_WAIT_START = 0;

static AudioStreamBuffer * abuf = NULL;

static snddrv_hnd * sndhnd = NULL;

static unsigned char wave_buffer[ 1024 * 64 ];

#define PCM_SIZE 1024 * 256 * 2

void LibMPG123_WaitStart()
{
    MPG123_WAIT_START = 1;
}

void LibMPG123_Start()
{
    MPG123_WAIT_START = 0;
}

unsigned int LibMPG123_Rate()
{
    return sndhnd->rate;
}

unsigned int LibMPG123_Chan()
{
    return sndhnd->chan;
}

/* This callback will handle the AICA Driver */
static void *mpa_drv_callback( snd_stream_hnd_t hnd, int pcm_needed, int * pcm_done )
{   
    while( MPG123_WAIT_START )
        thd_sleep( 2 );
          
    AudioStreamBuffer_Pop( abuf, sndhnd->drv_buf, pcm_needed );
    
    *pcm_done = pcm_needed;    
 
    return sndhnd->drv_buf; 
}

static void * mpa_drv_thd()
{
    snddrv_hnd_start( sndhnd );   
    
    while( sndhnd->stat != SNDDRV_STATUS_NULL )
    {
        snddrv_hnd_cb( sndhnd );
        thd_sleep( 30 );    
    } 
    
    return NULL;
}

void LibMPG123_VolumeUp()
{
    snddrv_hnd_volume_up( sndhnd );
}

void LibMPG123_VolumeDown()
{
    snddrv_hnd_volume_down( sndhnd );
}

/* Decode a chunk of MPEG Audio, given a bitstream buffer and its size */
int LibMPG123_DecodeChunk( uint8 * mpabuf, int size )
{
    err = mpg123_decode(mh, mpabuf, size, wave_buffer, 163840, &sndhnd->samples_done);
    
    if( sndhnd->samples_done )
        AudioStreamBuffer_Push( abuf, wave_buffer, sndhnd->samples_done );

    switch( err )
    {
        case MPG123_NEED_MORE:   /* Status OK - No need to waste time */
             break;
                 
        case MPG123_ERR:         /* Check for MPG123 Error Code */
             printf("err = %s\n", mpg123_strerror(mh));
             return 0;
     
        case MPG123_NEW_FORMAT:  /* First frame initialize structures */             
             err = mpg123_getformat(mh, &sndhnd->rate, &sndhnd->chan, &enc);

             while(err!=MPG123_NEED_MORE && err!=MPG123_ERR )
             {                        
                err = mpg123_decode(mh, NULL, 0, wave_buffer, 163840, &sndhnd->samples_done);
                
                if( sndhnd->samples_done )
                    AudioStreamBuffer_Push( abuf, wave_buffer, sndhnd->samples_done );
             } 

             /* Start the AICA Driver */
             sndhnd->drv_cb = mpa_drv_callback;
             thd_create( 0, mpa_drv_thd, NULL );
    }
     
    return 1;    
}

/* Exit LibMPG123 */
int LibMPG123_Exit( )
{  
    snddrv_hnd_exit( sndhnd );              /* Exit the AICA Driver */
    free( sndhnd );
    sndhnd = NULL;
    
    AudioStreamBuffer_FreeObject( abuf );    /* Reset the resources */
    enc=err=0;
                  
    mpg123_close(mh);                   /* Release the MPG123 handle */    
    mpg123_exit();  

    return 1; 
}

/* Initialize LibMPG123 -  Do this before calling LibMPG123_decode_chunk() */
int LibMPG123_Init( )
{    
    sndhnd = malloc( sizeof( snddrv_hnd ) );  /* Allocate Sound Driver Handle */
    memset( sndhnd, 0, sizeof(snddrv_hnd) );
    
    abuf = AudioStreamBuffer_NewObject( PCM_SIZE );    /* Allocate PCM Buffer */
    if( abuf == NULL )
        return 0;
    
    mpg123_init();                            /* Initialize the MPG123 handle */
    mh = mpg123_new( NULL, &err );
    if( mh == NULL )
        return 0;
       
    err = mpg123_open_feed( mh );        /* Start LibMPG123 in open_feed mode */

    return err == MPG123_OK ? 1 : 0; /* Return 1 if status is OK, 0 otherwise */
}
