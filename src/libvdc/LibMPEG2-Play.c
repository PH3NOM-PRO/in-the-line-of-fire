/*
**
** This File is a part of Dreamcast Media Center
** (C) Josh "PH3NOM" Pearson 2011-2013
**
*/
/*
** Based on  mpeg2dec.c
 *
 * Copyright (C) 2000-2003 Michel Lespinasse <walken@zoy.org>
 * Copyright (C) 1999-2000 Aaron Holtzman <aholtzma@ess.engr.uvic.ca>
 *
 * This file is part of mpeg2dec, a free MPEG-2 video stream decoder.
 * See http://libmpeg2.sourceforge.net/ for updates.
 *
 * mpeg2dec is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * mpeg2dec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <inttypes.h>

#include <mpeg2.h>
#include <mpeg2convert.h>

#include "LibMPEG.h"
#include "LibMPEG2-Play.h"
#include "LibMPG123.h"

static mpeg2dec_t * mpeg2dec;
static const int buffer_size = 1024;
static int demux_track = 0xe0;	
static int demux_pid = 0;
static int total_offset = 0;
static int apts, vpts, adts, vdts;

static FILE * in_file;

static unsigned int MPEG_EOF;

static unsigned short RenderActive = 0;

static MpegSequenceHeader * seqHdr = NULL;

static unsigned char * FrameBuffer;
static unsigned char * buffer;
static unsigned char * end;

static void * malloc_hook (unsigned size, mpeg2_alloc_t reason)
{
    void * buf;
    if ((int)reason < 0) {
        return NULL;
    }
    buf = mpeg2_malloc (size, (mpeg2_alloc_t)-1);
    if (buf && (reason == MPEG2_ALLOC_YUV || reason == MPEG2_ALLOC_CONVERTED))
        memset (buf, 0, size);
    return buf;
}

static void decode_mpeg_dc (uint8_t * current, uint8_t * end)
{
    const mpeg2_info_t * info;
    mpeg2_state_t state;

    mpeg2_buffer (mpeg2dec, current, end);
    total_offset += end - current;

    info = mpeg2_info (mpeg2dec);
    while (1)
    {
	    state = mpeg2_parse (mpeg2dec);
	    switch (state) {
	    case STATE_BUFFER:
	        return;
	    case STATE_SEQUENCE:
		    mpeg2_convert (mpeg2dec, mpeg2convert_uyvy, NULL);

	        break;
	    case STATE_PICTURE:
            break;
	    case STATE_SLICE:
	    case STATE_END:
	    case STATE_INVALID_END:
                memcpy( FrameBuffer, info->display_fbuf->buf[0],
                        seqHdr->Width * seqHdr->Height * 2 );
                RenderActive = 1;
                
	        break;
	    default:
	    break;
	}
    }

}

#define DEMUX_PAYLOAD_START 1
static int demux (uint8_t * buf, uint8_t * end, int flags)
{
    static int mpeg1_skip_table[16] = {
	0, 0, 4, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    /*
     * the demuxer keeps some state between calls:
     * if "state" = DEMUX_HEADER, then "head_buf" contains the first
     *     "bytes" bytes from some header.
     * if "state" == DEMUX_DATA, then we need to copy "bytes" bytes
     *     of ES data before the next header.
     * if "state" == DEMUX_SKIP, then we need to skip "bytes" bytes
     *     of data before the next header.
     *
     * NEEDBYTES makes sure we have the requested number of bytes for a
     * header. If we dont, it copies what we have into head_buf and returns,
     * so that when we come back with more data we finish decoding this header.
     *
     * DONEBYTES updates "buf" to point after the header we just parsed.
     */

#define DEMUX_HEADER 0
#define DEMUX_DATA 1
#define DEMUX_SKIP 2
#define DEMUX_MPA_DATA 3
    static int state = DEMUX_SKIP;
    static int state_bytes = 0;
    static uint8_t head_buf[264];

    uint8_t * header;
    int bytes;
    int len;

#define NEEDBYTES(x)						\
    do {							\
	int missing;						\
								\
	missing = (x) - bytes;					\
	if (missing > 0) {					\
	    if (header == head_buf) {				\
		if (missing <= end - buf) {			\
		    memcpy (header + bytes, buf, missing);	\
		    buf += missing;				\
		    bytes = (x);				\
		} else {					\
		    memcpy (header + bytes, buf, end - buf);	\
		    state_bytes = bytes + end - buf;		\
		    return 0;					\
		}						\
	    } else {						\
		memcpy (head_buf, header, bytes);		\
		state = DEMUX_HEADER;				\
		state_bytes = bytes;				\
		return 0;					\
	    }							\
	}							\
    } while (0)

#define DONEBYTES(x)		\
    do {			\
	if (header != head_buf)	\
	    buf = header + (x);	\
    } while (0)

    if (flags & DEMUX_PAYLOAD_START)
	goto payload_start;
    switch (state) {
    case DEMUX_HEADER:
	if (state_bytes > 0) {
	    header = head_buf;
	    bytes = state_bytes;
	    goto continue_header;
	}
	break;
    case DEMUX_DATA:
	if (demux_pid || (state_bytes > end - buf)) {
	    decode_mpeg_dc (buf, end);
	    state_bytes -= end - buf;
	    return 0;
	}
	decode_mpeg_dc (buf, buf + state_bytes);
	buf += state_bytes;
	break;
    case DEMUX_SKIP:
	if (demux_pid || (state_bytes > end - buf)) {
	    state_bytes -= end - buf;
	    return 0;
	}
	buf += state_bytes;
	break; 
    case DEMUX_MPA_DATA:
	if (demux_pid || (state_bytes > end - buf))
    {
        int mpa_got = end - buf;
        
        LibMPG123_DecodeChunk( buf, mpa_got );
        
        state_bytes -= mpa_got;
	    return 0;	    
	}

    LibMPG123_DecodeChunk( buf, state_bytes );
    
    buf += state_bytes;
	break;
   }
   
    while (1) {
	if (demux_pid) {
	    state = DEMUX_SKIP;
	    return 0;
	}
    payload_start:
	header = buf;
	bytes = end - buf;
    continue_header:
	NEEDBYTES (4);
	if (header[0] || header[1] || (header[2] != 1)) {
	    if (demux_pid) {
		state = DEMUX_SKIP;
		return 0;
	    } else if (header != head_buf) {
		buf++;
		goto payload_start;
	    } else {
		header[0] = header[1];
		header[1] = header[2];
		header[2] = header[3];
		bytes = 3;
		goto continue_header;
	    }
	}  
	 if ((header[3] >=0xe0 && header[3]<= 0xef) && (demux_track == 0)){
	   demux_track = header[3];
	 }
	 switch (header[3]) {
	case 0xb9:	/* program end code */
	    /* DONEBYTES (4); */
	    /* break;         */
	    return 1;
	case 0xba:	/* pack header */
	    NEEDBYTES (5);
	    if ((header[4] & 0xc0) == 0x40) {	/* mpeg2 */
		NEEDBYTES (14);
		len = 14 + (header[13] & 7);
		NEEDBYTES (len);
		DONEBYTES (len);
		/* header points to the mpeg2 pack header */
	    } else if ((header[4] & 0xf0) == 0x20) {	/* mpeg1 */
		NEEDBYTES (12);
		DONEBYTES (12);
		/* header points to the mpeg1 pack header */
	    } else {
		fprintf (stderr, "weird pack header\n");
		DONEBYTES (5);
	    }
	    break;
	default:
	    if((header[3]>=0xc0) && (header[3]<=0xdf)){	// mpeg audio stream 
		NEEDBYTES (7);
		if ((header[6] & 0xc0) == 0x80) {	/* mpeg2 */
		    NEEDBYTES (9);
		    len = 9 + header[8];
		    NEEDBYTES (len);
		    /* header points to the mpeg2 pes header */
		    if (header[7] & 0x80) {
			uint32_t pts, dts;

			pts = (((header[9] >> 1) << 30) |
			       (header[10] << 22) | ((header[11] >> 1) << 15) |
			       (header[12] << 7) | (header[13] >> 1));
			dts = (!(header[7] & 0x40) ? pts :
			       (((header[14] >> 1) << 30) |
				(header[15] << 22) |
				((header[16] >> 1) << 15) |
				(header[17] << 7) | (header[18] >> 1)));
			apts = pts; adts=dts;
		    }
		} else {	/* mpeg1 */
		    int len_skip;
		    uint8_t * ptsbuf;

		    len = 7;
		    while (header[len - 1] == 0xff) {
			len++;
			NEEDBYTES (len);
			if (len >= 23) {
			    fprintf (stderr, "too much stuffing\n");
			    break;
			}	
		    }
		    if ((header[len - 1] & 0xc0) == 0x40) {
			len += 2;
			NEEDBYTES (len);
		    }
		    len_skip = len;
		    len += mpeg1_skip_table[header[len - 1] >> 4];
		    NEEDBYTES (len);
		    /* header points to the mpeg1 pes header */
		    ptsbuf = header + len_skip;
		    if ((ptsbuf[-1] & 0xe0) == 0x20) {
			uint32_t pts, dts;

			pts = (((ptsbuf[-1] >> 1) << 30) |
			       (ptsbuf[0] << 22) | ((ptsbuf[1] >> 1) << 15) |
			       (ptsbuf[2] << 7) | (ptsbuf[3] >> 1));
			dts = (((ptsbuf[-1] & 0xf0) != 0x30) ? pts :
			       (((ptsbuf[4] >> 1) << 30) |
				(ptsbuf[5] << 22) | ((ptsbuf[6] >> 1) << 15) |
				(ptsbuf[7] << 7) | (ptsbuf[18] >> 1)));

			apts = pts; adts=dts;
			
		    }
		}
		DONEBYTES (len);
		bytes = 6 + (header[4] << 8) + header[5] - len;
		if (demux_pid ||(bytes > end - buf))
        {
            int mpa_got = end - buf;

            LibMPG123_DecodeChunk( buf, mpa_got );

		    state = DEMUX_MPA_DATA;
		    state_bytes = bytes - mpa_got;
		    return 0;
		
        } else if (bytes > 0)
        {
            LibMPG123_DecodeChunk( buf, bytes );    
            buf += bytes;
		}
	    } //else if (header[3] >=0xe0 && header[3]<= 0xef){ /* video stream */
	    else if (header[3] == demux_track){ /* video stream */
//	    pes:
		NEEDBYTES (7);
		if ((header[6] & 0xc0) == 0x80) {	/* mpeg2 */
		    NEEDBYTES (9);
		    len = 9 + header[8];
		    NEEDBYTES (len);
		    /* header points to the mpeg2 pes header */
		    if (header[7] & 0x80) {
			uint32_t pts, dts;

			pts = (((header[9] >> 1) << 30) |
			       (header[10] << 22) | ((header[11] >> 1) << 15) |
			       (header[12] << 7) | (header[13] >> 1));
			dts = (!(header[7] & 0x40) ? pts :
			       (((header[14] >> 1) << 30) |
				(header[15] << 22) |
				((header[16] >> 1) << 15) |
				(header[17] << 7) | (header[18] >> 1)));
			mpeg2_tag_picture (mpeg2dec, pts, dts);

			vpts = pts; vdts=dts;
			
		    }
		} else {	/* mpeg1 */
		    int len_skip;
		    uint8_t * ptsbuf;

		    len = 7;
		    while (header[len - 1] == 0xff) {
			len++;
			NEEDBYTES (len);
			if (len > 23) {
			    fprintf (stderr, "too much stuffing\n");
			    break;
			}
		    }
		    if ((header[len - 1] & 0xc0) == 0x40) {
			len += 2;
			NEEDBYTES (len);
		    }
		    len_skip = len;
		    len += mpeg1_skip_table[header[len - 1] >> 4];
		    NEEDBYTES (len);
		    /* header points to the mpeg1 pes header */
		    ptsbuf = header + len_skip;
		    if ((ptsbuf[-1] & 0xe0) == 0x20) {
			uint32_t pts, dts;

			pts = (((ptsbuf[-1] >> 1) << 30) |
			       (ptsbuf[0] << 22) | ((ptsbuf[1] >> 1) << 15) |
			       (ptsbuf[2] << 7) | (ptsbuf[3] >> 1));
			dts = (((ptsbuf[-1] & 0xf0) != 0x30) ? pts :
			       (((ptsbuf[4] >> 1) << 30) |
				(ptsbuf[5] << 22) | ((ptsbuf[6] >> 1) << 15) |
				(ptsbuf[7] << 7) | (ptsbuf[18] >> 1)));
			mpeg2_tag_picture (mpeg2dec, pts, dts);

			vpts = pts; vdts=dts;
			
		    }
		}
		DONEBYTES (len);
		bytes = 6 + (header[4] << 8) + header[5] - len;
		if (demux_pid || (bytes > end - buf)) {
		    decode_mpeg_dc (buf, end);
		    state = DEMUX_DATA;
		    state_bytes = bytes - (end - buf);
		    return 0;
		} else if (bytes > 0) {
		    decode_mpeg_dc (buf, buf + bytes);
		    buf += bytes;
		}
	    } else if (header[3] < 0xb9) {
		fprintf (stderr,
			 "looks like a video stream, not system stream\n");
		DONEBYTES (4);
		return 2; /* Returning a positive value will allow us the chance
			to get other functions to quit trying to play an
			elementary (non-muxed) stream.  */
	    } else {
		NEEDBYTES (6);
		DONEBYTES (6);
		bytes = (header[4] << 8) + header[5];
//		skip:
		if (bytes > end - buf) {
		    state = DEMUX_SKIP;
		    state_bytes = bytes - (end - buf);
		    return 0;
		}
		buf += bytes;
	    }
	}
    }
}

unsigned char * LibMPEG2_DecodeNextFrame()
{
    do {
        end = buffer + fread (buffer, 1, buffer_size, in_file);
        if (demux (buffer, end, 0))
	    break;	/* hit program_end_code */ 
    } while ( end == buffer + buffer_size && !RenderActive );
    
    RenderActive = 0;
    
    return ftell( in_file ) >= MPEG_EOF ? NULL : FrameBuffer;
}

unsigned char LibMPEG2_InitFileStream( char * fileName, int *width, int *height )
{
    /* Open the MPEG File and Initialize LibMPEG2 */   
    in_file = fopen ( fileName, "rb" );
    if( in_file == NULL )
    {
        printf("LibMPEG2: Error Reading File.\n");
        return 0;       
    }
    
    fseek( in_file, 0, SEEK_END );
    MPEG_EOF = ftell( in_file );
    fseek( in_file, 0 , SEEK_SET );
    
    seqHdr = LibMPEG_ParseSequenceHeader( in_file );
    if( seqHdr == NULL )
    {
        printf("LibMPEG2: Error Reading Sequence Header\n");
        return 0;
    }    
    else
        printf("LibMPEG2: Resolution: %ix%i\n", seqHdr->Width, seqHdr->Height );
    
    mpeg2dec = mpeg2_init ();
    if( mpeg2dec == NULL )
        return 0;
    
    mpeg2_malloc_hooks( malloc_hook, NULL );
    
    buffer = (uint8_t *) malloc (buffer_size);
    if( buffer == NULL )
	return 0;
    
    FrameBuffer = malloc( seqHdr->Width * seqHdr->Height * 2 );
    
    LibMPG123_WaitStart();
    LibMPG123_Init();    
    
    *width = seqHdr->Width;
    *height = seqHdr->Height;
    
    return 1;
}

void LibMPEG2_Exit()
{
    LibMPG123_Exit();   
     
    mpeg2_close (mpeg2dec);
    
    free(seqHdr); seqHdr=NULL;
    
    if( buffer )
    {
        free( buffer );
        buffer = NULL;
    }
    
    if( FrameBuffer )
    {
        free( FrameBuffer );
        FrameBuffer = NULL;
    }
    
    fclose (in_file);     
}
