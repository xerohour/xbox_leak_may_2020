#include "tif_port.h"

#ifndef lint
static char sccsid[] = "@(#)tiff_close.c	1.6 4/25/89";
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
#include "tif_misc.h"

TIFFClose(tif)
	TIFF *tif;
{
#ifdef NEEDED
	static char module[] = "TIFFClose";
	TIFFDirEntry *dir = NULL;
#endif /* NEEDED */

#ifdef DO_WRITE
	if (tif->tif_mode != O_RDONLY)
		/*
		 * Flush buffered data and directory (if dirty).
		 */
		TIFFFlush(tif);
#endif

	if (tif->tif_cleanup)
		(*tif->tif_cleanup)(tif);
	TIFFFreeDirectory(tif);
	if (tif->tif_rawdata)
		free(tif->tif_rawdata);
	(void) close(tif->tif_fd);
#ifdef applec
	if (tif->tif_mode & O_RDWR)
		mac_typefile(tif->tif_name, MAC_TIFF_TYPE);
#endif
	free((char *)tif);
	return (1);
}
