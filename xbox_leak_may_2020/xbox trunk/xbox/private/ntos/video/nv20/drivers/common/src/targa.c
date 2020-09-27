// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
//  Module: targa.c
//      routines for reading and writing targa files
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        12Jun00         filched from architects
//
// **************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include "targa.h"

static FILE *ft = NULL;

static char itype_names[12][16] = {"NULL", "MAP", "RGB", "MONO", "4", "5", "6", "7", "8", "RLE-MAP", "RLE-RGB", "RLE-MONO"};
static char mess[200];

static int  decode_rgb     (HANDLE hFile, int width, int height, DWORD dwBPP, unsigned char *dest);
static int  decode_rgb_rle (HANDLE hFile, int width, int height, DWORD dwBPP, unsigned char *dest);

// Read targa file "filename"
// Put pixels into **xbgr, and set *width and *height
// Pixels are xbgr words on the PC
// This routine copes with raw RGB and run-length encoded RGB targa files only

int read_targa_file (char *filename, unsigned char **xbgr, int *width, int *height)
{
    HANDLE     hFile;
    DWORD      dwBytesRead;
    int        err;
    TGA_Header header;
    char       id[2560];
    DWORD      dwBPP;

    *xbgr = NULL;

    hFile = CreateFile (filename, GENERIC_READ, 0,0, OPEN_EXISTING, 0,0);

    if (hFile == INVALID_HANDLE_VALUE) {
        return(-1);
    }

    ReadFile (hFile, &header, sizeof(TGA_Header), &dwBytesRead, NULL);
    if (!dwBytesRead) {
        return(-2);
    }

    if (header.ImageIDLength > 0) {
        ReadFile (hFile, id, header.ImageIDLength, &dwBytesRead, NULL);
        if (!dwBytesRead) {
            return(-3);
        }
    }

    if ((header.ImgType != TGA_RGB) && (header.ImgType != TGA_RLERGB)) {
        return(-4);
    }

    if ((header.PixelSize != 16) && (header.PixelSize != 24) && (header.PixelSize != 32)) {
        return(-5);
    }

    if (((header.Desc & TGA_DESC_ORG_MASK) != TGA_ORG_TOP_LEFT) &&
        ((header.Desc & TGA_DESC_ORG_MASK) != TGA_ORG_BOTTOM_LEFT)) {
        return(-6);
    }

    *width =  ((header.Width_hi)  << 8) | header.Width_lo;
    *height = ((header.Height_hi) << 8) | header.Height_lo;

    *xbgr = (unsigned char *) GlobalAlloc (GMEM_FIXED, *width * *height * sizeof(int));

    if (!(*xbgr)) {
        return (-100);
    }

    dwBPP = header.PixelSize >> 3;

    if ((header.ImgType == TGA_RGB) && ((header.Desc & TGA_DESC_ORG_MASK) == TGA_ORG_TOP_LEFT)) {
        err = decode_rgb (hFile, *width, *height, dwBPP, *xbgr);
    }
    else if ((header.ImgType == TGA_RLERGB) ) {
        err = decode_rgb_rle (hFile, *width, *height, dwBPP, *xbgr);
    }
    CloseHandle (hFile);

    return (err);
}

// Convert from 24 bits/pixel to 32 bits/pixel

static int decode_rgb
(
    HANDLE  hFile,
    int     width,
    int     height,
    DWORD   dwBPP,
    unsigned char *dest_array
)
{
    unsigned char *src, *dest, *scanline;
    int            y, x;
    DWORD          dwBytesRead;

    scanline = (unsigned char *) GlobalAlloc (GMEM_FIXED, width * dwBPP + 1);
    if (scanline == NULL) {
        return(-7);
    }

    for (y=0; y<height; y++) {

        dest = &dest_array[y * width * 4];

        ReadFile (hFile, scanline, dwBPP*width, &dwBytesRead, NULL);
        if (!dwBytesRead) {
            return(-8);
        }

        src = scanline;
        for (x=0; x<width; x++) {
#ifdef   WIN32
            dest[3] = 0;
            dest[2] = src[0];
            dest[1] = src[1];
            dest[0] = src[2];
#else
            dest[0] = 0;
            dest[1] = src[0];
            dest[2] = src[1];
            dest[3] = src[2];
#endif
            src += dwBPP;
            dest += 4;
        }
    }

    GlobalFree (scanline);

    return(0);
}

// Decode run-length encoded Targa into 32-bit pixels

static int decode_rgb_rle
(
    HANDLE  hFile,
    int     width,
    int     height,
    DWORD   dwBPP,
    unsigned char *dest_array)
{
    unsigned char *src, *dest, packet[520];
    int            i, j, count, raw;
    DWORD          dwBytesRead;
    int            dest_count = width*height;

    dest = dest_array;
    
    for (i=0; i<dest_count;) {
        
        
        ReadFile (hFile, packet, dwBPP+1, &dwBytesRead, NULL); // Read repetition count and first color value
        if (!dwBytesRead) {
            return(-9);
        }
        
        raw = (packet[0] & 0x80) == 0;     // Is this packet raw pixels or a repeating color
        count = (packet[0] & 0x7f) + 1;    // How many raw pixels or color repeats
        i += count;
        src = &packet[1];                  // src[0-(dwBPP-1)] are first color value
        
        if (raw && count > 1) {
            ReadFile (hFile, &packet[dwBPP+1], dwBPP*(count-1), &dwBytesRead, NULL); // Read remainder of raw packet into src[3+]
            if (!dwBytesRead) {
                return(-10);
            }
        }
        
        for (j=0; j<count; j++) {
            
            dest[3] = (dwBPP == 4) ? src[3] : 0;
            dest[2] = src[2];
            dest[1] = src[1];
            dest[0] = src[0];
            
            if (raw)                   // In raw mode, keep advancing "src" to subsequent values
                src += dwBPP;          // In RLE mode, just repeat the packet[1] RGB color
            dest += 4;
        }
        if (!raw) 
            src += dwBPP;             // In RLE mode, advance src beyond color
    }
    
    return(0);
}

