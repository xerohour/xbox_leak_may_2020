#include "tif_port.h"


#ifndef lint
static char sccsid[] = "@(#)open.c	1.4	(Pixar - RenderMan Division)	11/2/89";
/* static char sccsid[] = "@(#)tiff_open.c	1.10 4/27/89"; */
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

/*
 * TIFF Library.
 */
#include "tiffio.h"

// TH 2/20/96 Commented this out to keep the microsoft compiler happy...
//#ifdef HIGHC
//#include <stdefs.h>
//extern int errno;
//static int saverr;
//#endif

long TIFFScanlineSize();


#ifndef MSDOS
#ifdef O_BINARY
#undef O_BINARY
#endif
#define O_BINARY 0
#endif

/*
 * Return an open file descriptor or -1.
 */
#if defined(applec) || defined(THINK_C)
#define	TIFFOpenFile(name, mode, prot)	open(name, mode)
#else
#ifdef HIGHC
#define TIFFOpenFile(name, mode, prot)  c_open(name, mode)
#define TIFFCreateFile(name, mode, prot)  c_create(name, mode)
#else
#define	TIFFOpenFile(name, mode, prot)	open(name, mode, prot)
#endif /* HIGHC */
#endif

#define	ord(e)	((int)e)

/*
 * Initialize shift & mask tables and byte
 * swapping state according to the file
 * byte order.
 */
static
TIFFInitByteOrder(register TIFF *tif,int magic, int bigendian)
	{
	tif->tif_typemask[0] = 0;
	tif->tif_typemask[ord(TIF_BYTE)] = 0xff;
	tif->tif_typemask[ord(TIF_SHORT)] = 0xffff;
	tif->tif_typemask[ord(TIF_LONG)] = 0xffffffff;
	tif->tif_typemask[ord(TIF_RATIONAL)] = 0xffffffff;
	tif->tif_typeshift[0] = 0;
	tif->tif_typeshift[ord(TIF_LONG)] = 0;
	tif->tif_typeshift[ord(TIF_RATIONAL)] = 0;
	if (magic == TIFF_BIGENDIAN) {
		tif->tif_typeshift[ord(TIF_BYTE)] = 24;
		tif->tif_typeshift[ord(TIF_SHORT)] = 16;
		if (!bigendian)
			tif->tif_flags |= TIFF_SWAB;
		} 
	else {
		tif->tif_typeshift[ord(TIF_BYTE)] = 0;
		tif->tif_typeshift[ord(TIF_SHORT)] = 0;
		if (bigendian)
			tif->tif_flags |= TIFF_SWAB;
		}
#ifdef KINETIX_TIFF_CHANGES
    return 1;   // arbitrary, but int function must return a value
#endif
	}


int rd_offset;
TIFF *TIFFReadHdr(FILEHNDL fd) {
	static char module[] = "TIFFReadHdr";
	TIFF *tif;
	int m, bigendian;
	rd_offset=0;

	m = O_RDONLY|O_BINARY;

	errno = 0;
	tif = (TIFF *)alloc(sizeof (TIFF));
	if (tif == NULL) {
		TIFFError(module, "Out of memory (TIFF structure)");
		return ((TIFF *)0);
		}
	bzero((char *)tif, sizeof (*tif));
	tif->tif_fd = fd;
	tif->tif_mode = m &~ (O_CREAT|O_TRUNC|O_BINARY);
	tif->tif_curoff = 0;
	tif->tif_curstrip = -1;		/* invalid strip */
	tif->tif_row = -1;		/* read/write pre-increment */
	{ int one = 1; bigendian = (*(char *)&one == 0); }
	/*
	 * Read in TIFF header.
	 */
	try_again:
	if (!ReadOK(fd, &tif->tif_header, sizeof (TIFFHeader))) {
		TIFFError("TIFFReadHdr", "Can't read TIFF header");
			goto bad;
		}
	/*
	 * Setup the byte order handling.
	 */
	if (tif->tif_header.tiff_magic != TIFF_BIGENDIAN &&
		tif->tif_header.tiff_magic != TIFF_LITTLEENDIAN) {
		if (rd_offset>0) {
			TIFFError("TIFFReadHdr",  "Not a TIFF file ");
			goto bad;
			}
		else {
			if (!SeekOK(fd,128)) goto bad;
			rd_offset = 128;
			goto try_again;
			}
		}
	TIFFInitByteOrder(tif, tif->tif_header.tiff_magic, bigendian);
	/*
	 * Swap header if required.
	 */
	if (tif->tif_flags & TIFF_SWAB) {
		TIFFSwabShort(&tif->tif_header.tiff_version);
		TIFFSwabLong(&tif->tif_header.tiff_diroff);
		}
	/*
	 * Now check version (if needed, it's been byte-swapped).
	 * Note that this isn't actually a version number, it's a
	 * magic number that doesn't change (stupid).
	 */
	if (tif->tif_header.tiff_version != TIFF_VERSION) {
		TIFFError("TIFFReadHdr",
			"Not a TIFF file, bad vers # %d (0x%x)",
			tif->tif_header.tiff_version,
			tif->tif_header.tiff_version); 
		goto bad;
		}
	/*
	 * Setup initial directory.
	 */

	tif->tif_nextdiroff = tif->tif_header.tiff_diroff;
	if (TIFFReadDirectory(tif)) {
		tif->tif_scanlinesize = TIFFScanlineSize(tif);
		tif->tif_rawcc = -1;
		tif->tif_flags |= TIFF_BUFFERSETUP;
		return (tif);
		}
bad:
	tif->tif_mode = O_RDONLY;	/* XXX avoid flush */
	return ((TIFF *)0);
   }

#define	howmany(x, y)	(((x)+((y)-1))/(y))
long
TIFFScanlineSize(	TIFF *tif) {
	TIFFDirectory *td = &tif->tif_dir;
	long scanline;
	
	scanline = td->td_bitspersample * td->td_imagewidth;
	if (td->td_planarconfig == PLANARCONFIG_CONTIG)
		scanline *= td->td_samplesperpixel;
	return (howmany(scanline, 8));
	}

