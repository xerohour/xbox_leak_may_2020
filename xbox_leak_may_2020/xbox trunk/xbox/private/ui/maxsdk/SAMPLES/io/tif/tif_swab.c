#include "tif_port.h"

#ifndef lint
static char sccsid[] = "@(#)tiff_swab.c	1.2 2/19/89";
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
 * TIFF Library Byte Swapping Routines.
 *
 * XXX We assume short = 16-bits and long = 32-bits XXX
 */
#include "machdep.h"

#ifndef TIFFSwabShort
void TIFFSwabShort(wp)
	unsigned short *wp;
{
	register unsigned char *cp = (unsigned char *)wp;
	int t;

	t = cp[1]; cp[1] = cp[0]; cp[0] = t;
}
#endif

#ifndef TIFFSwabLong
void TIFFSwabLong(lp)
	unsigned long *lp;
{
	register unsigned char *cp = (unsigned char *)lp;
	int t;

	t = cp[3]; cp[3] = cp[0]; cp[0] = t;
	t = cp[2]; cp[2] = cp[1]; cp[1] = t;
}
#endif

#ifndef TIFFSwabArrayOfShort
void TIFFSwabArrayOfShort(wp, n)
	unsigned short *wp;
	register int n;
{
	register unsigned char *cp;
	register int t;

	/* XXX unroll loop some */
	while (n-- > 0) {
		cp = (unsigned char *)wp;
		t = cp[1]; cp[1] = cp[0]; cp[0] = t;
		wp++;
	}
}
#endif

#ifndef TIFFSwabArrayOfLong
void TIFFSwabArrayOfLong(lp, n)
	register unsigned long *lp;
	register int n;
{
	register unsigned char *cp;
	register int t;

	/* XXX unroll loop some */
	while (n-- > 0) {
		cp = (unsigned char *)lp;
		t = cp[3]; cp[3] = cp[0]; cp[0] = t;
		t = cp[2]; cp[2] = cp[1]; cp[1] = t;
		lp++;
	}
}
#endif
