/* 
** DCE - Dreamcast Engine (C) 2013-2016 Josh PH3NOM Pearson
*/

#include "dce.h"

uint16 DCE_StringFindCharFirst(char * string, char c)
{
	uint16 i = 0;
	for(i = 0; i < strlen(string); i++)
	    if(string[i] == c)
	        return i;
    return 0;
}

void DCE_StringCopyNoExt(char * dst, char * src)
{
   	uint16 slen = DCE_StringFindCharFirst(src, '.');
	
	strncpy(dst, src, slen);	
	
	dst[slen] = '\0';
}

void DCE_StringCopyNewExt(char * dst, char * src, char * ext)
{
   	uint16 slen = DCE_StringFindCharFirst(src, '.');
	
	strncpy(dst, src, slen);	
	
	dst[slen] = '\0';
	
	strcat(dst, ext);
}

void DCE_StringHasExt(char * src, char * ext)
{
   	uint16 slen = DCE_StringFindCharFirst(src, '.');
	
	return src[slen + 1] == ext[0] && src[slen + 2] == ext[1] && src[slen + 3] == ext[2];
}

unsigned char DCE_StringEqualsIgnoreCase(char * src, char * dst)
{
	unsigned int oft = 'a' - 'A';
	unsigned char o1, o2;
	unsigned short int i;
		
	if(strlen(dst) != strlen(src))
	    return 0;
	
	for(i = 0; i < strlen(dst); i++)
	{
		o1 = src[i] >= 'a' ? src[i] - oft : src[i];
		o2 = dst[i] >= 'a' ? dst[i] - oft : dst[i];
		
		if(o1 != o2)
		    return 0;
	}   
	   
	return 1;
}
