#ifndef LibMPEG2_PLAY
#define LibMPEG2_PLAY

unsigned char LibMPEG2_InitFileStream( char * fileName, int *width, int *height );

unsigned char * LibMPEG2_DecodeNextFrame();

void LibMPEG2_Exit();

#endif