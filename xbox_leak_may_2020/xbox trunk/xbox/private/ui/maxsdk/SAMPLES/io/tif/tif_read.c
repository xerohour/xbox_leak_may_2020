#include "tif_port.h"

#ifndef lint
static char sccsid[] = "@(#)read.c	1.3	(Pixar - RenderMan Division)	7/31/89";
/* static char sccsid[] = "@(#)tiff_read.c	1.13 4/25/89"; */
#endif

/*
 * Copyright (c) 1988 by Sam Leffler.
 * All rights reserved.
 *
 * This file is provided for unrestricted use provided that this
 * legend is included on all tape media and as a part of the
 * software program in whole or part.  Users may copy, modify or
 * distribute this file at will.
 */

#define xxxDBGTIF

/*
 * TIFF Library.
 * Scanline-oriented Read Support
 */
#include "tiffio.h"

#ifdef applec
#include <FCntl.h>
#define lseek unix_lseek	/* Mac's Standard 'lseek' won't extend file */
#define L_SET 0
#define L_INCR 1
#define L_XTND 2
#endif

#define	roundup(x, y)	((((x)+((y)-1))/(y))*(y))

extern int rd_offset;


/* Set state to appear as if a
 * strip has just been read in */
static TIFFStartStrip(register TIFF *tif,	u_int strip) {
	TIFFDirectory *td = &tif->tif_dir;

#ifdef DBGTIF
	printf("TIFFStartStrip, strip %d, \n",strip);
#endif
	tif->tif_curstrip = strip;
	tif->tif_row = (strip % td->td_stripsperimage) * td->td_rowsperstrip;
	tif->tif_rawcp = tif->tif_rawdata;
	tif->tif_rawcc = td->td_stripbytecount[strip];
	return (tif->tif_stripdecode == NULL ||     // KINETIX: removed (int) cast from NULL
                (*tif->tif_stripdecode)(tif));
	}


/* Read a strip of data from the file. */
TIFFReadStrip( register TIFF *tif,	u_int strip )	{
	
	static char module[] = "TIFFReadStrip";
	TIFFDirectory *td = &tif->tif_dir;
	long bytecount;     // KINETIX: made type signed

#ifdef DBGTIF
	printf("TIFFReadStrip, strip %d, \n",strip);
#endif
	if (!SeekOK(tif->tif_fd, td->td_stripoffset[strip])) {
		TIFFError(module, "%s: Seek error at scanline %d, strip %d",
		    tif->tif_name, tif->tif_row, strip);
		return (0);
		}
	/*
	 * Expand raw data buffer, if needed, to
	 * hold data strip coming from file
	 * (perhaps should set upper bound on
	 *  the size of a buffer we'll use?).
	 */
	bytecount = td->td_stripbytecount[strip];
	if (bytecount > tif->tif_rawdatasize) {
		tif->tif_curstrip = -1;		/* unknown state */
		if (tif->tif_rawdata) {
			free(tif->tif_rawdata);
			tif->tif_rawdata = NULL;
			}
		tif->tif_rawdatasize = roundup(bytecount, 1024);
		tif->tif_rawdata = alloc(tif->tif_rawdatasize);
		if (tif->tif_rawdata == NULL) {
			TIFFError(module,
			    "%s: No space for data buffer at scanline %d",
			    tif->tif_name, tif->tif_row);
			tif->tif_rawdatasize = 0;
			return (0);
			}
		}
	if (!ReadOK(tif->tif_fd, tif->tif_rawdata, bytecount)) {
		TIFFError(module, "%s: Read error at scanline %d, bytecount=%d",
		    tif->tif_name, tif->tif_row, bytecount);
		return (0);
		}
	return (TIFFStartStrip(tif, strip));
	}


/* Seek to a random row+sample in a file. */
static
/*VARARGS2*/
TIFFSeek(register TIFF *tif, u_int in_u_row, u_int sample)	{
    // KINETIX: added local signed "row" to match signed comparisons below
    int row = (int)in_u_row;
	register TIFFDirectory *td = &tif->tif_dir;
	int strip;  // KINETIX: changed from u_int

#ifdef DBGTIF
	printf("TIFFSeek, row %d,  sample %d \n",row,sample);
#endif
	if (row >= td->td_imagelength) {	
		/* out of range */
		TIFFError(tif->tif_name, "%d: Row out of range, max %d",
		    row, td->td_imagelength);
		return (0);
		}	
	if (td->td_planarconfig == PLANARCONFIG_SEPARATE) {
		if (sample >= td->td_samplesperpixel) {
			TIFFError(tif->tif_name,
			    "%d: Sample out of range, max %d",
			    sample, td->td_samplesperpixel);
			return (0);
			}
		strip = sample*td->td_stripsperimage + row/td->td_rowsperstrip;
		} 
	else
		strip = row / td->td_rowsperstrip;
	if (strip != tif->tif_curstrip) { 	/* different strip, refill */
		if (!TIFFReadStrip(tif, strip))
			return (0);
		} 
	else if (row < tif->tif_row) {
		/* Moving backwards within the same strip: backup
		 * to the start and then decode forward (below).
		 *
		 * NB: If you're planning on lots of random access within a
		 * strip, it's better to just read and decode the entire
		 * strip, and then access the decoded data in a random fashion. 
		 */
		if (!TIFFStartStrip(tif, strip))
			return (0);
		}
	if (row != tif->tif_row) {
		if (tif->tif_seek) {
			/* Seek forward to the desired row. */
			if (!(*tif->tif_seek)(tif, row - tif->tif_row))
				return (0);
			tif->tif_row = row;
			} 
		else {
			TIFFError(tif->tif_name,
		     "Compression algorithm does not support random access");
			return (0);
			}
		}
	return (1);
	}

/*VARARGS3*/
TIFFReadScanline(	register TIFF *tif,u_char *buf,u_int row, u_int sample) {
	int e;

#ifdef DBGTIF
	printf("TIFFReadScanline, row %d,  sample %d \n",row,sample);
#endif
	if (tif->tif_mode == O_WRONLY) {
		TIFFError(tif->tif_name, "File not open for reading");
		return (-1);
		}
	if (e = TIFFSeek(tif, row, sample)) {
		/*
		 * Decompress desired row into user buffer
		 */
		e = (*tif->tif_decoderow)(tif, buf, tif->tif_scanlinesize);
		tif->tif_row++;
		}
	return (e ? 1 : -1);
	}


