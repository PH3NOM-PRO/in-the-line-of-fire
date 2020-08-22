/*
** SPU_WAVE (C) PH3NOM 2011
** Based on modplug_test
** And the wave parser from snd_sfgmgr
*/

#include <kos.h>
#include "spu_wave.h"

static uint32 fd;
static uint16 sound_buffer[65536];
static snd_stream_hnd_t shnd;

int wave_restart() {
/*    
    if( spu_status == WAVE_STATUS_STREAMING ) {
       spu_status = WAVE_STATUS_PAUSING;
    
       while( spu_status != WAVE_STATUS_PAUSED )
          thd_pass();
            
       fs_seek(fd, 0x32, SEEK_SET);

       spu_status = WAVE_STATUS_STREAMING;
    }
    
    else
       printf("SPU WAVE: Cant Reset if not already streaming\n");
    
    return spu_status;    
*/
}

void *wave_callback(snd_stream_hnd_t hnd, int len, int * actual)
{
	int r = fs_read(fd, sound_buffer, len);
	if(r != len )
	{
		fs_seek(fd, 0x32, SEEK_SET);
		fs_read(fd, sound_buffer + r, len - r);
	}

	*actual = len;
	
	return sound_buffer;
}

void spu_wave_stream_callback()
{
	if(spu_status == WAVE_STATUS_STREAMING)
	    snd_stream_poll(shnd);
}

void spu_wave_stream_stop( )
{
	snd_stream_destroy(shnd);
	snd_stream_shutdown();
       
    fs_close(fd);
 
    sq_clr( sound_buffer, 65536 );
}

int spu_wave_stream_start( char * file_name )
{
	uint32	len, hz;
	uint16	chn, bitsize, fmt;

    snd_stream_init();
	   
	fd = fs_open(file_name, O_RDONLY);
	   
	if(!fd)
       return 0;
       
	hz = 0;
	fs_seek(fd, 8, SEEK_SET);
	fs_read(fd, &hz, 4);
	if (strncmp((char*)&hz, "WAVE", 4))
	    return 0;

    /* Read WAV header info */
    fs_seek(fd, 0x14, SEEK_SET);
	fs_read(fd, &fmt, 2);
	fs_read(fd, &chn, 2);
	fs_read(fd, &hz, 4);
	fs_seek(fd, 0x22, SEEK_SET);
	fs_read(fd, &bitsize, 2);
	
	/* Read WAV data */
	fs_seek(fd, 0x32, SEEK_SET);
	
    len = fs_total(fd) - 0x32;
       
	printf("WAVE File: %i chn / %i hz", chn, hz);
	      	    	
	fs_read(fd, sound_buffer, 65536);
	
	shnd = snd_stream_alloc(wave_callback, SND_STREAM_BUFFER_MAX);
	     
    snd_stream_start(shnd, hz, chn-1);
    
    spu_status = WAVE_STATUS_STREAMING;
    
    return 1;
}
