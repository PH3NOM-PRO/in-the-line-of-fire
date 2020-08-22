/*
**
** This File is a part of Dreamcast Media Center
** (C) Josh "PH3NOM" Pearson 2011-2012
**
*/

#ifndef SNDDRV_H
#define SNDDRV_H

#include <kos/mutex.h>
#include <dc/sound/stream.h>

/* Keep track of things from the Driver side */
volatile int snddrv_status;
#define SNDDRV_STATUS_NULL         0x00
#define SNDDRV_STATUS_INITIALIZING 0x01
#define SNDDRV_STATUS_READY        0x02
#define SNDDRV_STATUS_STREAMING    0x03
#define SNDDRV_STATUS_DONE         0x04
#define SNDDRV_STATUS_ERROR        0x05

typedef void * (*snddrv_cb)(snd_stream_hnd_t, int, int*); 

typedef struct
{
     long rate;
     int chan;
     unsigned int samples_done;
     short stat;
     unsigned short vol;
     snd_stream_hnd_t shnd;
     snddrv_cb drv_cb;
     unsigned char drv_buf[65536 + 16384];
} snddrv_hnd;


/* SNDDRV HND OBJECT FUNCTIONS */

int snddrv_hnd_start( snddrv_hnd * drvhnd );

void snddrv_hnd_cb( snddrv_hnd * drvhnd  );

void snddrv_hnd_exit( snddrv_hnd * drvhnd );

int snddrv_hnd_volume_set( snddrv_hnd * drvhnd, unsigned short vol );

int snddrv_hnd_volume_down( snddrv_hnd * drvhnd );

int snddrv_hnd_volume_up( snddrv_hnd * drvhnd );

#endif
