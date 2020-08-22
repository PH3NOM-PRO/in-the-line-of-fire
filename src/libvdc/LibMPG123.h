/*
**
** This file is a part of Dreamcast Media Center
** (C) Josh PH3NOM Pearson 2011-2013
**
*/
/*
**  LibMPG123 streaming decoder (C) Josh 'PH3NOM' Pearson
*/

#ifndef LibMPG123_H
#define LibMPG123_H

#include <kos/mutex.h>

mutex_t * mpa_mut;
#define MPA_create_mutex()  { mpa_mut = mutex_create(); }
#define MPA_lock_mutex()    { mutex_lock( mpa_mut );    }
#define MPA_unlock_mutex()  { mutex_unlock( mpa_mut );  }
#define MPA_destroy_mutex() { mutex_destroy( mpa_mut ); }

volatile unsigned char MP3_NEED_SAMPLES;

int LibMPG123_Init();

int LibMPG123_DecodeChunk( uint8 * mpabuf, int size );

int LibMPG123_Exit();

void LibMPG123_VolumeUp();

void LibMPG123_VolumeDown();

void LibMPG123_WaitStart();

void LibMPG123_Start();

#endif
