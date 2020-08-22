#ifndef MPEG_DC_H
#define MPEG_DC_H

/* Keep Track of MPEG Audio/Video Status */
volatile int MPA_STATUS;
#define MPA_STAT_NULL     0x00
#define MPA_STAT_READY    0x01
#define MPA_STAT_NEEDBUF  0x02
#define MPA_STAT_HAVEBUF  0x03
#define MPA_STAT_LOCKED   0x04
#define MPA_STAT_UNLOCKED 0x05
#define MPA_STAT_DONE     0x06


#include <kos/mutex.h>
mutex_t * mpeg_mut;
#define MPEG_create_mutex()  { mpeg_mut = mutex_create(); }
#define MPEG_lock_mutex()    { mutex_lock( mpeg_mut );    }
#define MPEG_unlock_mutex()  { mutex_unlock( mpeg_mut );  }
#define MPEG_destroy_mutex() { mutex_destroy( mpeg_mut ); }

mutex_t * mpa_mut;
#define MPA_create_mutex()  { mpa_mut = mutex_create(); }
#define MPA_lock_mutex()    { mutex_lock( mpa_mut );    }
#define MPA_unlock_mutex()  { mutex_unlock( mpa_mut );  }
#define MPA_destroy_mutex() { mutex_destroy( mpa_mut ); }

#define PCM_SIZE 65536+16384
#define BS_SIZE  512*3
uint8 buffer[BS_SIZE*2];
uint8 outbuf[1024*512]; 
uint8 mpa_buffer[1024*128];
volatile int mpa_bytes;
volatile int bs_done;
    
/* Function Definitions */
void lmpg123_exit();
void lmpg123_pause();
void lmpg123_play();
void * lmpg123_dec();

int mpa_stream_init();
int mpa_stream_dec( unsigned char * buf, int size );
int mpa_stream_exit();


#endif
