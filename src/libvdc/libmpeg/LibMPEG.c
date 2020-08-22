/*
** LibMPEG.c (C) Josh PH3NOM Pearson 2013
**
** I only wrote this small lib to parse data from an MPEG stream,
** specifically the Sequence Header.
** Thanks to some info on the MPEG format by Wilson Woo <wilson00@HK.Super.NET>
** and various collective source around the net that indicated Big Endian order
** and details of aspect ratio encoding not known by Wilson Woo.
*/

#include <stdlib.h>
#include <stdio.h>

#include "LibMPEG.h"

unsigned int ByteSwap( unsigned int b )
{
    return ( (b&0x000000FF)<<24 |
             (b&0x0000FF00)<<8 |
             (b&0x00FF0000)>>8 |
             (b&0xFF000000)>>24 );
}

int LibMPEG_SeekSequenceHeader( FILE * ins )
{
    if(ins==NULL) return -1;
     
    unsigned int size;
    unsigned char SEQHDR[4] = { 0x00, 0x00, 0x01, 0xB3 }; // Sequence Header
    unsigned char buf[2];
    
    fseek( ins, 0, SEEK_SET );
    fseek( ins, 0, SEEK_END );    
    size = ftell(ins);
    fseek( ins, 0 , SEEK_SET );
    
    while(ftell(ins)<size) // Not pretty, but it does work :-0
    {
        fread( buf, 1, sizeof(unsigned char), ins );
        if(buf[0]==SEQHDR[0])
        {
            fread( buf, 1, sizeof(unsigned char), ins );
            if(buf[0]==SEQHDR[1])
            {
                fread( buf, 1, sizeof(unsigned char), ins );
                if(buf[0]==SEQHDR[2])
                {
                    fread( buf, 1, sizeof(unsigned char), ins );
                    if(buf[0]==SEQHDR[3])
                        return ftell(ins);
                }                  
            }
        }    
    }
    return -1;
}

float LibMPEG_ParseDAR( unsigned char d )
{
    switch(d)
    {
        case 1:
             return 1.f;
             break;
        case 2:
             return 4.f/3.f;
             break;
        case 3:
             return 16.f/9.f;
             break;
        case 4:
             return 2.21f/1.f;
             break; 
        default:
             return 1.f;
             break;            
    }
}

float LibMPEG_ParseFrameRate( unsigned char d )
{
    switch(d)
    {
        case 1:
             return 24000.f/1001.f;
             break;
        case 2:
             return 24.f;
             break;
        case 3:
             return 25.f;
             break;
        case 4:
             return 30000.f/1001.f;
             break;             
        case 5:
             return 30.f;
             break;
        case 6:
             return 50.f;
             break;
        case 7:
             return 60000.f/1001.f;
             break;      
        case 8:
             return 60.f;
             break;
        default:
             return 24000.f/1001.f;
             break;
    }
}

MpegSequenceHeader * LibMPEG_ParseSequenceHeader( FILE * ins )
{
    if(LibMPEG_SeekSequenceHeader(ins)<0)
        return NULL;
    
    MpegSequenceHeader * seqHdr = malloc(sizeof(MpegSequenceHeader));
    MpgSeqHdr hdr[1];
    
    fread( hdr, 1, sizeof(MpgSeqHdr), ins );
    fseek( ins, 0, SEEK_SET );
    
    hdr[0] = ByteSwap(hdr[0]);
       
    seqHdr->Width  = (hdr[0]&0xFFF00000)>>20;
    seqHdr->Height = (hdr[0]&0x000FFF00)>>8;
    seqHdr->DAR    = (hdr[0]&0x000000F0)>>4;//LibMPEG_ParseDAR( (hdr[0]&0x000000F0)>>4 );
    seqHdr->FrameRate = LibMPEG_ParseFrameRate( (hdr[0]&0x0000000F) );
    
    return seqHdr;
}

