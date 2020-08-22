/* 
** DCE - Dreamcast Engine (C) 2013-2015 Josh PH3NOM Pearson
*/

#include <time.h>
#include <arch/timer.h>

/* Get current hardware timing using arch/timer.h */
static uint32 s, ms;
static uint64 msec;

unsigned int DCE_GetTime()
{
   	timer_ms_gettime(&s, &ms);
	msec = (((uint64)s) * ((uint64)1000)) + ((uint64)ms);
	
	return (unsigned int)msec;
}
