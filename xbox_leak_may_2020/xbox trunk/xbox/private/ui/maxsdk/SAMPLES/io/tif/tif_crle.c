
#include "tif_port.h"

#ifndef lint
static char sccsid[] = "@(#)tiff_ccittrle.c	1.2 2/19/89";
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
 * CCITT Group 3 1-D Modified Huffman
 * Run Length Encoding Compression Support
 */
#include "tiffio.h"

TIFFInitCCITTRLE(tif)
	TIFF *tif;
{
	TIFFError(tif->tif_name, "CCITT Group 3 1-D encoding not supported");
	return (0);
}

TIFFInitCCITTRLEW(tif)
	TIFF *tif;
{

	TIFFError(tif->tif_name, "CCITT Group 3 1-D encoding not supported");
	return (0);
}
