#include "tif_port.h"

#ifndef lint
static char sccsid[] = "@(#)dumpmode.c	1.2	(Pixar - RenderMan Division)	5/5/89";
/* static char sccsid[] = "@(#)tiff_dumpmode.c	1.7 4/25/89"; */
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
 *
 * "Null" Compression Algorithm Support.
 */
#include "tiffio.h"

static	int DumpModeEncode(), DumpModeDecode(), DumpModeSeek();

/*
 * Initialize dump mode.
 */
TIFFInitDumpMode(tif)
	register TIFF *tif;
{
	tif->tif_decoderow = DumpModeDecode;
	tif->tif_encoderow = DumpModeEncode;
	tif->tif_seek = DumpModeSeek;
	return (1);
}

/*
 * Encode a scanline of pixels.
 */
static
DumpModeEncode(tif, pp, cc)
	register TIFF *tif;
	u_char *pp;
{

#ifdef DO_WRITE
	if (tif->tif_rawcc + cc > tif->tif_rawdatasize)
		if (!TIFFFlushData(tif))
			return (-1);
	bcopy(pp, tif->tif_rawcp, cc);
	tif->tif_rawcp += cc;
	tif->tif_rawcc += cc;
#endif
	return (1);
}

/*
 * Decode a scanline of pixels.
 */
static
DumpModeDecode(tif, buf, cc)
	TIFF *tif;
	u_char *buf;
{
	TIFFDirectory	*td = &tif->tif_dir;

	if (tif->tif_rawcc < cc) {
		TIFFError(tif->tif_name,
		    "DumpModeDecode: Not enough data for scanline %d",
		    tif->tif_row);
		return (0);
	}
	bcopy(tif->tif_rawcp, buf, cc);

	if (td->td_bitspersample > 8 && td->td_bitspersample <= 16 &&
                        (tif->tif_flags & TIFF_SWAB))
		TIFFSwabArrayOfShort((u_short *)buf, cc/2);

	return (DumpModeSeek(tif, 1));
}

/*
 * Seek forwards nrows in the current strip.
 */
static
DumpModeSeek(tif, nrows)
	register TIFF *tif;
	int nrows;
{

	tif->tif_rawcp += nrows * tif->tif_scanlinesize;
	tif->tif_rawcc -= nrows * tif->tif_scanlinesize;
	return (1);
}
