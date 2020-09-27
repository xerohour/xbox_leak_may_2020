#include "tif_port.h"

#ifndef lint
static char sccsid[] = "@(#)tiff_compress.c	1.12 4/27/89";
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
 * TIFF Library
 *
 * Compression Scheme Configuration Support.
 */
#include "tiffio.h"

extern	int TIFFInitDumpMode();
extern	int TIFFInitCCITTRLE(), TIFFInitCCITTRLEW();
extern	int TIFFInitPackBits();
#ifdef FAX_SUPPORT
extern	int TIFFInitCCITTFax3(), TIFFInitCCITTFax4();
#endif
#ifdef THUNDER_SUPPORT
extern	int TIFFInitThunderScan();
#endif
#ifdef PICIO_SUPPORT
extern	int TIFFInitPicio();
#endif
#ifdef NEXT_SUPPORT
extern	int TIFFInitNeXT();
#endif
#ifdef SGI_SUPPORT
extern	int TIFFInitSGI();
#endif
extern	int TIFFInitLZW();

static	struct cscheme {
	int	scheme;
	int	(*init)();
} CompressionSchemes[] = {
    { COMPRESSION_NONE,		TIFFInitDumpMode },
    { COMPRESSION_LZW,		TIFFInitLZW },
    { COMPRESSION_PACKBITS,	TIFFInitPackBits },
#ifdef PICIO_SUPPORT
    { COMPRESSION_PICIO,	TIFFInitPicio },
#endif
#ifdef THUNDER_SUPPORT
    { COMPRESSION_THUNDERSCAN,	TIFFInitThunderScan },
#endif
#ifdef NEXT_SUPPORT
    { COMPRESSION_NEXT,		TIFFInitNeXT },
#endif
#ifdef SGI_SUPPORT
    { COMPRESSION_SGIRLE,	TIFFInitSGI },
#endif
    { COMPRESSION_CCITTRLE,	TIFFInitCCITTRLE },
    { COMPRESSION_CCITTRLEW,	TIFFInitCCITTRLEW },
#ifdef FAX_SUPPORT
    { COMPRESSION_CCITTFAX3,	TIFFInitCCITTFax3 },
    { COMPRESSION_CCITTFAX4,	TIFFInitCCITTFax4 },
#endif
};
#define	NSCHEMES (sizeof (CompressionSchemes) / sizeof (CompressionSchemes[0]))

TIFFSetCompressionScheme(tif, scheme)
	TIFF *tif;
	int scheme;
{
	register struct cscheme *c;

	for (c = CompressionSchemes; c < &CompressionSchemes[NSCHEMES]; c++)
		if (c->scheme == scheme) {
			tif->tif_stripdecode = NULL;
			tif->tif_stripencode = NULL;
			tif->tif_encodestrip = NULL;
			tif->tif_seek = NULL;
			tif->tif_cleanup = NULL;
			return ((*c->init)(tif));
		}
	TIFFError(tif->tif_name, "Unknown data compression algorithm %u (0x%x)",
	    scheme, scheme);
	return (0);
}
