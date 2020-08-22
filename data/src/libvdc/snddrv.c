/*
**
** SNDDRV.c (C) Josh 'PH3NOM' Pearson 2011-2013
**
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kos/thread.h>

#include "snddrv.h"

/* This is probably the final version of the driver, based on a handle */

/* Set the Sound Driver volume */
int snddrv_hnd_volume_set( snddrv_hnd * drvhnd, unsigned short vol )
{           
    drvhnd->vol = vol;
    
    snd_stream_volume( drvhnd->shnd, drvhnd->vol );
    
    return drvhnd->vol;
}

/* Increase the Sound Driver volume */
int snddrv_hnd_volume_up( snddrv_hnd * drvhnd )
{    
    if( drvhnd->vol <= 245 )
    {
        drvhnd->vol += 10;
  	    snd_stream_volume( drvhnd->shnd, drvhnd->vol );
    }
    return drvhnd->vol;
}

/* Decrease the Sound Driver volume */
int snddrv_hnd_volume_down( snddrv_hnd * drvhnd )
{    
    if( drvhnd->vol >= 10 )
    {
        drvhnd->vol -= 10;
        snd_stream_volume( drvhnd->shnd, drvhnd->vol );
    }
    return drvhnd->vol;
}

/* Exit the Sound Driver */
void snddrv_hnd_exit( snddrv_hnd * drvhnd )
{
    if(drvhnd->stat == SNDDRV_STATUS_NULL)
       return;
       
    drvhnd->stat = SNDDRV_STATUS_NULL;
    thd_sleep( 50 );
    
    snd_stream_destroy( drvhnd->shnd );
    snd_stream_shutdown();
}

/* Poll the Sound Driver */
void snddrv_hnd_cb( snddrv_hnd * drvhnd  )
{
    snd_stream_poll( drvhnd->shnd );
}

/* Start the Sound Driver */
int snddrv_hnd_start( snddrv_hnd * drvhnd )
{
    if(drvhnd->stat != SNDDRV_STATUS_NULL)
       return 0;
    
    if( drvhnd->chan < 1 || drvhnd->chan > 2 )
    {
        printf("SNDDRV: ERROR - Invalid Number of channels\n");
        return 0;
    }
#ifdef DEBUG    
    printf("SNDDRV: %iHz - %i channel\n", drvhnd->rate, drvhnd->chan );
#endif
    drvhnd->vol = 0xFF;  
      
    snd_stream_init();
        
    drvhnd->shnd = snd_stream_alloc( drvhnd->drv_cb, SND_STREAM_BUFFER_MAX / 4 );
	
    drvhnd->stat = SNDDRV_STATUS_STREAMING;
	
    snd_stream_start(drvhnd->shnd, drvhnd->rate, drvhnd->chan-1);
    
    return 1;
}
