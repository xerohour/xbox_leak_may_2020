#include "tif_port.h"

#ifndef lint
static char sccsid[] = "@(#)tiff_ccittfax.c	1.2 2/19/89";
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
 * CCITT Groups 3 & 4 Facsimile-compatible
 * Compression Scheme Support.
 */
#include "tiffio.h"

TIFFInitCCITTFax3(tif)
	TIFF *tif;
{

	TIFFError(tif->tif_name,
            "CCITT Group 3 encoding not supported");
	return (0);
}

TIFFInitCCITTFax4(tif)
	TIFF *tif;
{

	TIFFError(tif->tif_name,
	    "CCITT Group 4 encoding not supported");
	return (0);
}
