/* @(#)tiffio.h	1.5	(Pixar - RenderMan Division)	5/16/90 */
/*	tiffio.h	1.14	89/04/25	*/

/*
 * Copyright (c) 1988 by Sam Leffler.
 * All rights reserved.
 *
 * This file is provided for unrestricted use provided that this
 * legend is included on all tape media and as a part of the
 * software program in whole or part.  Users may copy, modify or
 * distribute this file at will.
 */

#ifndef _TIFFIO_
#define	_TIFFIO_


#include "compat.h"
#include "tiffdefs.h"

/*
 * TIFF I/O Library Definitions.
 */

/*
 * Internal format of a TIFF directory entry.
 */
typedef	struct {
	u_short td_subfiletype;
	u_short	td_imagewidth, td_imagelength;
	u_short	td_bitspersample;
	u_short	td_compression;
	u_short	td_photometric;
	u_short	td_threshholding;
	u_short	td_orientation;
	u_short	td_samplesperpixel;
	u_short	td_predictor;
	u_long	td_rowsperstrip;
	u_long	td_minsamplevalue, td_maxsamplevalue;	/* maybe float? */
	float	td_xresolution, td_yresolution;
	u_short	td_resolutionunit;
	u_short	td_planarconfig;
	float	td_xposition, td_yposition;
	u_long	td_group3options;
	u_long	td_group4options;
	u_short td_pagenumber;
	u_short	td_grayresponseunit;
	u_short	td_colorresponseunit;
	u_short	td_matteing;
	u_short	*td_grayresponsecurve;	/* u_short for now (maybe float?) */
	u_short	*td_redresponsecurve;	/* u_short for now (maybe float?) */
	u_short	*td_greenresponsecurve;	/* u_short for now (maybe float?) */
	u_short	*td_blueresponsecurve;	/* u_short for now (maybe float?) */
	u_short	*td_redcolormap;
	u_short	*td_greencolormap;
	u_short	*td_bluecolormap;
#ifdef notdef
/* not yet used/supported */
	float	td_whitepoint[2];
	float	td_primarychromaticities[6];
#endif
	char	*td_documentname;
	char	*td_artist;
	char	*td_datetime;
	char	*td_hostcomputer;
	char	*td_imagedescription;
	char	*td_make;
	char	*td_model;
	char	*td_software;
	char	*td_pagename;
	u_long	td_fieldsset[2];	/* bit vector of fields that are set */
	u_long	td_stripsperimage;
	u_long	td_nstrips;		/* size of offset & bytecount arrays */
	u_long	*td_stripoffset;
	u_long	*td_stripbytecount;
#ifdef DESIGN_VER
	double	*td_modpixelscale;
	double	*td_matrix;
	u_short td_modtiescount;
	double	*td_modties;
	double	*td_modtrans;
	u_short *td_geokeydir;
	double	*td_geodouble;
	char	*td_geoascii;
#endif
} TIFFDirectory;

/*
 * Field flags used to indicate fields that have
 * been set in a directory, and to reference fields
 * when manipulating a directory.
 */
/* multi-entry fields */
#define	FIELD_IMAGEDIMENSIONS		0
#define	FIELD_CELLDIMENSIONS		1		/* XXX */
#define	FIELD_RESOLUTION		2
#define	FIELD_POSITION			3
/* single-entry fields */
#define	FIELD_SUBFILETYPE		4
#define	FIELD_BITSPERSAMPLE		5
#define	FIELD_COMPRESSION		6
#define	FIELD_PHOTOMETRIC		7
#define	FIELD_THRESHHOLDING		8
#define	FIELD_FILLORDER			9		/* XXX */
#define	FIELD_DOCUMENTNAME		10
#define	FIELD_IMAGEDESCRIPTION		11
#define	FIELD_MAKE			12
#define	FIELD_MODEL			13
#define	FIELD_ORIENTATION		14
#define	FIELD_SAMPLESPERPIXEL		15
#define	FIELD_ROWSPERSTRIP		16
#define	FIELD_MINSAMPLEVALUE		17
#define	FIELD_MAXSAMPLEVALUE		18
#define	FIELD_PLANARCONFIG		19
#define	FIELD_PAGENAME			20
#define	FIELD_GRAYRESPONSEUNIT		21
#define	FIELD_GRAYRESPONSECURVE		22
#define	FIELD_GROUP3OPTIONS		23
#define	FIELD_GROUP4OPTIONS		24
#define	FIELD_RESOLUTIONUNIT		25
#define	FIELD_PAGENUMBER		26
#define	FIELD_COLORRESPONSEUNIT		27
#define	FIELD_COLORRESPONSECURVE	28
#define	FIELD_STRIPBYTECOUNTS		29
#define	FIELD_STRIPOFFSETS		31
#define	FIELD_WHITEPOINT			32
#define	FIELD_PRIMARYCHROMATICITIES			33
#define	FIELD_COLORMAP			34
#define FIELD_PREDICTOR			35
#define FIELD_ARTIST			36
#define FIELD_DATETIME			37
#define FIELD_HOSTCOMPUTER		38
#define FIELD_SOFTWARE			39
#define	FIELD_MATTEING			40
#ifndef DESIGN_VER
#define	FIELD_LAST			FIELD_MATTEING
#else
#define FIELD_MODELPIXELSCALE	41
#define FIELD_MATRIX			42
#define FIELD_MODELTIES			43
#define FIELD_MODELTRANS		44
#define FIELD_GEOKEYDIR			45
#define FIELD_GEODOUBLE			46
#define FIELD_GEOASCII			47
#define FIELD_LAST			FIELD_GEOASCII
#endif

#define	TIFFFieldSet(tif, field) \
    ((tif)->tif_dir.td_fieldsset[field/32] & (1L<<(field&0x1f)))
#define	TIFFSetFieldBit(tif, field) \
    ((tif)->tif_dir.td_fieldsset[field/32] |= (1L<<(field&0x1f)))

typedef	struct {
	char	*tif_name;		/* name of open file */
	FILEHNDL	tif_fd;			/* open file descriptor */
	short	tif_mode;		/* open mode (O_*) */
	short	tif_flags;
#define	TIFF_DIRTYHEADER	0x1	/* header must be written on close */
#define	TIFF_DIRTYDIRECT	0x2	/* current directory must be written */
#define	TIFF_BUFFERSETUP	0x4	/* data buffers setup */
#define	TIFF_BEENWRITING	0x8	/* written 1+ scanlines to file */
#define	TIFF_SWAB		0x10	/* byte swap file information */
	u_long	tif_diroff;		/* file offset of current directory */
	long	tif_nextdiroff;		/* file offset of following directory */
	TIFFDirectory tif_dir;		/* internal rep of current directory */
	TIFFHeader tif_header;		/* file's header block */
	int	tif_typeshift[6];	/* data type shift counts */
	long	tif_typemask[6];	/* data type masks */
	long	tif_row;		/* current scanline */
	int	tif_curstrip;		/* current strip for read/write */
	long	tif_curoff;		/* current offset for read/write */
/* compression scheme hooks */
	int	(*tif_stripdecode)();	/* strip decoding routine (pre) */
	int	(*tif_decoderow)();	/* scanline decoding routine */
	int	(*tif_stripencode)();	/* strip encoding routine (pre) */
	int	(*tif_encoderow)();	/* scanline encoding routine */
	int	(*tif_encodestrip)();	/* strip encoding routine (post) */
	int	(*tif_seek)();		/* position within a strip routine */
	int	(*tif_cleanup)();	/* routine called to cleanup state */
	char	*tif_data;		/* compression scheme private data */
/* input/output buffer */
	long	tif_scanlinesize;	/* # of bytes in a scanline */
	char	*tif_rawdata;		/* raw data buffer */
	long	tif_rawdatasize;	/* # of bytes in raw data buffer */
	char	*tif_rawcp;		/* current spot in raw buffer */
	long	tif_rawcc;		/* bytes unread from raw buffer */
} TIFF;

#ifndef NULL
#define	NULL	0
#endif

	/* Declarations of libtiff routines. */
 
#if defined (__STDC__) || defined (KINETIX_TIFF_CHANGES)
 
extern TIFF *TIFFOpen(char *name, char *mode);
extern int TIFFClose(TIFF *tiff);
#ifdef __MACH__		/* really NeXT */
extern int TIFFSetField(TIFF *tif, int tag, ...);
extern int TIFFGetField(TIFF *tif, int tag, ...);
#else
extern int TIFFSetField(TIFF *tif, u_short tag, ...);
extern int TIFFGetField(TIFF *tif, u_short tag, ...);
#endif /* __MACH__ */
extern int TIFFReadDirectory(TIFF *tif);
extern int TIFFWriteDirectory(TIFF *tif);
extern int TIFFReadScanline(TIFF *tif, u_char *buf, u_int row, u_int sample);
extern int TIFFWriteScanline(TIFF *tif, u_char *buf, u_int row, u_int sample);
extern int TIFFFlush(TIFF *tif);
 
#else   /* __STDC__ */
 
extern  TIFF *TIFFOpen();
 
#endif  /* __STDC__ */

#include "tif_misc.h"

#endif /* _TIFFIO_ */
