/*
** SPU_WAVE (C) PH3NOM 2011
*/

#include <kos.h>
#include "spu_wave.h"

#define CONT_PLAY    0x01
#define CONT_PAUSE   0x02
#define CONT_RESTART 0x03
#define CONT_STOP    0x04

int check_cont() {
		
		int ret;
    	
        maple_device_t *cont;
	    cont_state_t *state;		
        cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);

		if(cont)
		{
			state = (cont_state_t *)maple_dev_status(cont);
			if (!state)
				ret = CONT_STOP;
			if (state->buttons & CONT_START) {
                ret = CONT_STOP;               
			}
			if (state->buttons & CONT_X) {
                ret = CONT_RESTART;
            }

		}
		
		return ret;
		
}

int main() {

	int o;
	o = 20*640 + 20;
	bfont_set_encoding(BFONT_CODE_ISO8859_1);
    bfont_draw_str(vram_s + o, 640, 1, "SPU WAVE STREAM (C) PH3NOM 2011"); o += 640*48;
    bfont_draw_str(vram_s + o, 640, 1, "Press Start to stop, press X to restart"); o += 640*48;

    /* Start the wave stream */
    wave_stream( "/cd/test.wav" );
    
    /* Wait for the stream to start */
    while( spu_status == WAVE_STATUS_NULL )
        thd_pass(); 
          
    /* Check for user input and eof */
	while(spu_status != WAVE_STATUS_NULL) {
       
       int input = check_cont();
       int exit = 0;
       switch (input) {
           
            case CONT_RESTART:
                 wave_restart();
                 break;
            case CONT_STOP: 
                 wave_stop();
                 exit ++;
                 break;

       }
       
       thd_pass();
       
       if(exit)
          break;
    }

    printf( "Wave Stream Finished\n");
    return 0;
}
