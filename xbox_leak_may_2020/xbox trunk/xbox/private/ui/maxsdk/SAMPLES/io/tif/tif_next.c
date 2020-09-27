#ifndef lint
static	char sccsid[] = "@(#)tiff_next.c	1.4 4/27/89";
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
 * NeXT 2-bit Grey Scale Compression Algorithm Support
 */
#include "tif_port.h"
#include "tiffio.h"

static	int NeXTEncode(), NeXTDecode();

TIFFInitNeXT(tif)
	TIFF *tif;
{

	tif->tif_decoderow = NeXTDecode;
	tif->tif_encoderow = NeXTEncode;
	return (1);
}

static
NeXTEncode(tif, pp, cc)
	TIFF *tif;
	u_char *pp;
{

	TIFFError(tif->tif_name, "NeXT encoding is not implemented");
	return (-1);
}

#define SETPIXEL(op, v) {			\
	switch (npixels++ & 3) {		\
	case 0:	op[0]  = (v) << 6; break;	\
	case 1:	op[0] |= (v) << 4; break;	\
	case 2:	op[0] |= (v) << 2; break;	\
	case 3:	*op++ |= (v);	   break;	\
	}					\
}

#define LITERALROW	0x00
#define LITERALSPAN	0x40
#define WHITE   	((1<<2)-1)

static
NeXTDecode(tif, buf, occ)
	TIFF *tif;
	char *buf;
	int occ;
{
	register u_char *bp, *op;
	register int cc, n;
	u_char *row;
	int scanline;

	/*
	 * Each scanline is assumed to start off as all
	 * white (we assume a PhotometricInterpretation
	 * of ``min-is-black'').
	 */
	for (op = (u_char *)buf, cc = occ; cc-- > 0;)
		*op++ = 0xff;

	bp = (u_char *)tif->tif_rawcp;
	cc = tif->tif_rawcc;
	scanline = tif->tif_scanlinesize;
	for (row = (u_char *)buf; occ > 0; occ -= scanline, row += scanline) {
		n = *bp++, cc--;
		switch (n) {
		case LITERALROW:
			/*
			 * The entire scanline is given as literal values.
			 */
			if (cc < scanline)
				goto bad;
			bcopy(bp, row, scanline);
			bp += scanline;
			cc -= scanline;
			break;
		case LITERALSPAN: {
			int off;
			/*
			 * The scanline has a literal span
			 * that begins at some offset.
			 */
			off = (bp[0] * 256) + bp[1];
			n = (bp[2] * 256) + bp[3];
			if (cc < 4+n)
				goto bad;
			bcopy(bp+4, row+off, n);
			bp += 4+n;
			cc -= 4+n;
			break;
		}
		default: {
			register int npixels = 0, grey;
			int imagewidth = tif->tif_dir.td_imagewidth;

			/*
			 * The scanline is composed of a sequence
			 * of constant color ``runs''.  We shift
			 * into ``run mode'' and interpret bytes
			 * as codes of the form <color><npixels>
			 * until we've filled the scanline.
			 */
			op = row;
			for (;;) {
				grey = (n>>6) & 0x3;
				n &= 0x3f;
				if (grey < WHITE) {
					while (n-- > 0)
						SETPIXEL(op, grey);
				} else {
					/*
					 * Run of white -- just update stuff.
					 */
					npixels += n;
					op += n>>2;
				}
				if (npixels >= imagewidth)
					break;
				if (cc == 0)
					goto bad;
				n = *bp++, cc--;
			}
			break;
		}
		}
	}
	tif->tif_rawcp = (char *)bp;
	tif->tif_rawcc = cc;
	return (1);
bad:
	TIFFError(tif->tif_name, "NeXTDecode: Not enough data for scanline %d",
	    tif->tif_row);
	return (0);
}
