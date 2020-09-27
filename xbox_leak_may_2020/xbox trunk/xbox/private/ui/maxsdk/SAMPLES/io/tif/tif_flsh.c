#include "tif_port.h"

#ifndef lint
static char sccsid[] = "@(#)tiff_flush.c	1.5 4/25/89";
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

#ifdef KINETIX_TIFF_CHANGES
    extern int TIFFFlushData (TIFF *tif);
#endif

TIFFFlush(tif)
	TIFF *tif;
{

	if (tif->tif_mode != O_RDONLY) {
		if (tif->tif_rawcc > 0 && !TIFFFlushData(tif))
			return (0);
		if ((tif->tif_flags & TIFF_DIRTYDIRECT) &&
		    !TIFFWriteDirectory(tif))
			return (0);
	}
	return (1);
}
