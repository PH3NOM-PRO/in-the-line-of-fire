//=======================================================================================//
// RGB.C (C) 2014 Josh PH3NOM Pearson
//=======================================================================================//

#include "dce-rgb.h"

unsigned char RGB8_TO_RGB5( unsigned char c )
{
    return (c * RGB5_MAX) / RGB8_MAX;
}

unsigned char RGB8_TO_RGB6( unsigned char c )
{
    return (c * RGB6_MAX) / RGB8_MAX;
}

unsigned char RGB16_RED( unsigned short c )
{
	return (c & RGB16_RED_MASK) >> RGB16_RED_SHIFT;
}

unsigned char RGB16_GREEN( unsigned short c )
{
	return (c & RGB16_GREEN_MASK) >> RGB16_GREEN_SHIFT;
}

unsigned char RGB16_BLUE( unsigned short c )
{
	return c & RGB16_BLUE_MASK;
}

unsigned short RGB24_2_RGB16( unsigned char *in )
{
	return RGB8_TO_RGB5(in[0]) << RGB16_RED_SHIFT | RGB8_TO_RGB6(in[1]) << RGB16_GREEN_SHIFT | RGB8_TO_RGB5(in[2]); 
}

unsigned int ARGB32_ADD( unsigned int in, unsigned char o)
{
	unsigned char *c = (unsigned char *)&in;
	unsigned int i;

	for(i = 0; i < 4; i++)
	{
		if((c[i] + o) > 0xFF)
			c[i] = 0xFF;
		else 
		    c[i] += o;
	}
	
	return in;
}
