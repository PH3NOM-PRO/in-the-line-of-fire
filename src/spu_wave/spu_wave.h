/*
** SPU_WAVE.H (C) PH3NOM 2011
*/

#ifndef SPU_WAVE_H
#define SPU_WAVE_H

/* WAVE Streaming Status */
volatile int spu_status;
#define WAVE_STATUS_NULL         0x00
#define WAVE_STATUS_INITIALIZING 0x01
#define WAVE_STATUS_READY        0x02
#define WAVE_STATUS_STREAMING    0x03
#define WAVE_STATUS_PAUSING      0x04
#define WAVE_STATUS_PAUSED       0x05
#define WAVE_STATUS_DONE         0x06
#define WAVE_STATUS_ERROR        0x07

/* Function Declarations */
void spu_wave_stream_callback();
void spu_wave_stream_stop();
int spu_wave_stream_start(char * file_name);

#endif
