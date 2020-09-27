#include "tif_port.h"

#ifndef lint
static char     sccsid[] = "@(#)dir.c	1.12	(Pixar)	4/16/90";
/* static char sccsid[] = "@(#)tiff_dir.c	1.29 4/27/89"; */
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
 * Directory Support Routines.
 */

extern int rd_offset;


#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include "tiffio.h"

#ifdef applec
#include <FCntl.h>
#define lseek unix_lseek	/* Mac's Standard 'lseek' won't extend file */
#define L_SET 0
#define L_INCR 1
#define L_XTND 2
#define NO_FSTAT
/* applec's float parameters are really extended 80-bit floats. */
#define VA_FLOAT_PARAM_TYPE extended
#endif

// More commented-out stuff so the microsoft compiler won't generate bogus dependencies
//#ifdef i860
//#define NO_FSTAT
//#endif

//#ifndef NO_FSTAT
//#include "port/stat.h"
//#endif

/* If nobody's particular about this, assume doubles. */
#ifndef VA_FLOAT_PARAM_TYPE
#define VA_FLOAT_PARAM_TYPE double
#endif

// Forward reference
extern TIFFFetchStripThing(TIFF *tif,TIFFDirEntry *dir,u_long nstrips,u_long **lpp);
extern TIFFFetchPerSampleShorts(TIFF *tif, TIFFDirEntry *dir, long *pl);

#define	howmany(x, y)	(((x)+((y)-1))/(y))
#define	roundup(x, y)	((((x)+((y)-1))/(y))*(y))

#define	FieldSet(fields, f)		(fields[f/32] & (1L<<(f&0x1f)))
#define	ResetFieldBit(fields, f)	(fields[f/32] &= ~(1L<<(f&0x1f)))

#define	TIFFExtractData(tif, type, v) \
    ((tif)->tif_header.tiff_magic == TIFF_BIGENDIAN ? \
        ((v) >> (tif)->tif_typeshift[type]) & (tif)->tif_typemask[type] : \
	(v) & (tif)->tif_typemask[type])
#define	TIFFInsertData(tif, type, v) \
    ((tif)->tif_header.tiff_magic == TIFF_BIGENDIAN ? \
        ((v) & (tif)->tif_typemask[type]) << (tif)->tif_typeshift[type] : \
	(v) & (tif)->tif_typemask[type])

typedef	struct {
	u_short	field_tag;		/* field's tag */
	short	field_count;		/* expected item count */
	TIFFDataType field_type;	/* type of associated data */
	u_short	field_bit;		/* bit in fieldsset bit vector */
	char	*field_name;		/* TIF_ASCII name */
} TIFFFieldInfo;




/*
 * NB: This array is assumed to be sorted by tag.
 */
static
TIFFFieldInfo FieldInfo[] = {
    /*
     * SUBFILETYPE should be TIF_LONG according to the tiff 5.0 spec,
     * but previous versions of the library wrote it as TIF_SHORT,
     * so allow TIF_SHORT as a valid length for input.
     */
    { TIFFTAG_SUBFILETYPE,	 1, TIF_LONG,	FIELD_SUBFILETYPE,
      "SubfileType" },
    { TIFFTAG_SUBFILETYPE,	 1, TIF_SHORT,	FIELD_SUBFILETYPE,
      "SubfileType" },

    { TIFFTAG_OSUBFILETYPE,	 1, TIF_SHORT,	FIELD_SUBFILETYPE,
      "OldSubfileType" },
    { TIFFTAG_IMAGEWIDTH,	 1, TIF_LONG,	FIELD_IMAGEDIMENSIONS,
      "ImageWidth" },
    { TIFFTAG_IMAGEWIDTH,	 1, TIF_SHORT,	FIELD_IMAGEDIMENSIONS,
      "ImageWidth" },
    { TIFFTAG_IMAGELENGTH,	 1, TIF_LONG,	FIELD_IMAGEDIMENSIONS,
      "ImageLength" },
    { TIFFTAG_IMAGELENGTH,	 1, TIF_SHORT,	FIELD_IMAGEDIMENSIONS,
      "ImageLength" },
    { TIFFTAG_BITSPERSAMPLE,	-1, TIF_SHORT,	FIELD_BITSPERSAMPLE,
      "BitsPerSample" },
    { TIFFTAG_COMPRESSION,	 1, TIF_SHORT,	FIELD_COMPRESSION,
      "Compression" },
    { TIFFTAG_PHOTOMETRIC,	 1, TIF_SHORT,	FIELD_PHOTOMETRIC,
      "PhotometricInterpretation" },
    { TIFFTAG_THRESHHOLDING,	 1, TIF_SHORT,	FIELD_THRESHHOLDING,
      "Threshholding" },
    { TIFFTAG_CELLWIDTH,	 1, TIF_SHORT,	(u_short)-1,
      "CellWidth" },
    { TIFFTAG_CELLLENGTH,	 1, TIF_SHORT,	(u_short)-1,
      "CellLength" },
    { TIFFTAG_FILLORDER,	 1, TIF_SHORT,	(u_short)-1,
      "FillOrder" },
    { TIFFTAG_DOCUMENTNAME,	-1, TIF_ASCII,	FIELD_DOCUMENTNAME,
      "DocumentName" },
    { TIFFTAG_IMAGEDESCRIPTION,	-1, TIF_ASCII,	FIELD_IMAGEDESCRIPTION,
      "ImageDescription" },
    { TIFFTAG_MAKE,		-1, TIF_ASCII,	FIELD_MAKE,
      "Make" },
    { TIFFTAG_MODEL,		-1, TIF_ASCII,	FIELD_MODEL,
      "Model" },
    { TIFFTAG_STRIPOFFSETS,	-1, TIF_LONG,	FIELD_STRIPOFFSETS,
      "StripOffsets" },
    { TIFFTAG_STRIPOFFSETS,	-1, TIF_SHORT,	FIELD_STRIPOFFSETS,
      "StripOffsets" },
    { TIFFTAG_ORIENTATION,	 1, TIF_SHORT,	FIELD_ORIENTATION,
      "Orientation" },
    { TIFFTAG_SAMPLESPERPIXEL,	 1, TIF_SHORT,	FIELD_SAMPLESPERPIXEL,
      "SamplesPerPixel" },
    { TIFFTAG_ROWSPERSTRIP,	 1, TIF_LONG,	FIELD_ROWSPERSTRIP,
      "RowsPerStrip" },
    { TIFFTAG_ROWSPERSTRIP,	 1, TIF_SHORT,	FIELD_ROWSPERSTRIP,
      "RowsPerStrip" },
    { TIFFTAG_STRIPBYTECOUNTS,	-1, TIF_LONG,	FIELD_STRIPBYTECOUNTS,
      "StripByteCounts" },
    { TIFFTAG_STRIPBYTECOUNTS,	-1, TIF_SHORT,	FIELD_STRIPBYTECOUNTS,
      "StripByteCounts" },
    { TIFFTAG_MINSAMPLEVALUE,	-1, TIF_SHORT,	FIELD_MINSAMPLEVALUE,
      "MinSampleValue" },
    { TIFFTAG_MAXSAMPLEVALUE,	-1, TIF_SHORT,	FIELD_MAXSAMPLEVALUE,
      "MaxSampleValue" },
    { TIFFTAG_XRESOLUTION,	 1, TIF_RATIONAL,	FIELD_RESOLUTION,
      "XResolution" },
    { TIFFTAG_YRESOLUTION,	 1, TIF_RATIONAL,	FIELD_RESOLUTION,
      "YResolution" },
    { TIFFTAG_PLANARCONFIG,	 1, TIF_SHORT,	FIELD_PLANARCONFIG,
      "PlanarConfiguration" },
    { TIFFTAG_PAGENAME,		-1, TIF_ASCII,	FIELD_PAGENAME,
      "PageName" },
    { TIFFTAG_XPOSITION,	 1, TIF_RATIONAL,	FIELD_POSITION,
      "XPosition" },
    { TIFFTAG_YPOSITION,	 1, TIF_RATIONAL,	FIELD_POSITION,
      "YPosition" },
    { TIFFTAG_FREEOFFSETS,	-1, TIF_LONG,	(u_short)-1,
      "FreeOffsets" },
    { TIFFTAG_FREEBYTECOUNTS,	-1, TIF_LONG,	(u_short)-1,
      "FreeByteCounts" },
    { TIFFTAG_GRAYRESPONSEUNIT,	 1, TIF_SHORT,	FIELD_GRAYRESPONSEUNIT,
      "GrayResponseUnit" },
    { TIFFTAG_GRAYRESPONSECURVE,-1, TIF_SHORT,	FIELD_GRAYRESPONSECURVE,
      "GrayResponseCurve" },
    { TIFFTAG_GROUP3OPTIONS,	 1, TIF_LONG,	FIELD_GROUP3OPTIONS,
      "Group3Options" },
    { TIFFTAG_GROUP4OPTIONS,	 1, TIF_LONG,	FIELD_GROUP4OPTIONS,
      "Group4Options" },
    { TIFFTAG_RESOLUTIONUNIT,	 1, TIF_SHORT,	FIELD_RESOLUTIONUNIT,
      "ResolutionUnit" },
    { TIFFTAG_PAGENUMBER,	 2, TIF_SHORT,	FIELD_PAGENUMBER,
      "PageNumber" },
    { TIFFTAG_COLORRESPONSEUNIT, 1, TIF_SHORT,	FIELD_COLORRESPONSEUNIT,
      "ColorResponseUnit" },
    { TIFFTAG_COLORRESPONSECURVE,-1,TIF_SHORT,	FIELD_COLORRESPONSECURVE,
      "ColorResponseCurve" },
    { TIFFTAG_SOFTWARE,		-1, TIF_ASCII,	FIELD_SOFTWARE,
      "Software" },
    { TIFFTAG_DATETIME,		-1, TIF_ASCII,	FIELD_DATETIME,
      "DateTime" },
    { TIFFTAG_ARTIST,		-1, TIF_ASCII,	FIELD_ARTIST,
      "Artist" },
    { TIFFTAG_HOSTCOMPUTER,	-1, TIF_ASCII,	FIELD_HOSTCOMPUTER,
      "HostComputer" },
    { TIFFTAG_PREDICTOR,	 1, TIF_SHORT,	FIELD_PREDICTOR,
      "Predictor" },
    { TIFFTAG_WHITEPOINT,		-1, TIF_SHORT,	FIELD_WHITEPOINT,
      "WhitePoint" },
    { TIFFTAG_PRIMARYCHROMATICITIES,  	-1, TIF_BYTE,FIELD_PRIMARYCHROMATICITIES,
      "PrimaryChromaticities" },
    { TIFFTAG_COLORMAP,		-1, TIF_SHORT,	FIELD_COLORMAP,
      "ColorMap" },
/* begin nonstandard tags */
    { TIFFTAG_MATTEING,		 1, TIF_SHORT,	FIELD_MATTEING,
      "Matteing" },
#ifdef DESIGN_VER
	{ TIFFTAG_MODELPIXELSCALE, 3,	TIF_DOUBLE, FIELD_MODELPIXELSCALE,
	"ModelPixelScale" },
	{ TIFFTAG_MATRIX,		-1,	TIF_DOUBLE,	FIELD_MATRIX, "IntergraphMatrix" },
	{ TIFFTAG_MODELTIES,	-1,	TIF_DOUBLE, FIELD_MODELTIES, "ModelTiepoint" },
	{ TIFFTAG_MODELTRANS,	16,	TIF_DOUBLE,	FIELD_MODELTRANS, "ModelTransformation" },
	{ TIFFTAG_GEOKEYDIR,	-1, TIF_SHORT, FIELD_GEOKEYDIR, "GeoKeyDirectory" },
	{ TIFFTAG_GEODOUBLE,	-1, TIF_DOUBLE, FIELD_GEODOUBLE, "GeoDoubleParams" },
	{ TIFFTAG_GEOASCII,		-1, TIF_ASCII, FIELD_GEOASCII, "GeoAsciiParams" },
#endif
};
#define	NFIELDINFO	(sizeof (FieldInfo) / sizeof (FieldInfo[0]))

#define	IGNORE	0		/* tag placeholder used below */

/*short datawidth[] = { 1, 1, 1, 2, 4, 8, };*/

static TIFFFetchData(TIFF *tif,TIFFDirEntry *dir,char *cp);
static char *ncp = NULL;   /* Array allocated in TIFFReadDirectory */
static	double TIFFFetchRational(); 
static TIFFFieldInfo *FieldWithTag();

/*
 * If the system has it, just use fstat to get the file size.
 * Otherwise use lseek.
 */


static u_long TIFFGetFileSize(FILE *fd){
#ifdef MICROSOFT
	u_long curpos=0L;
	u_long filesize=(u_long)-1;
	curpos= ftell(fd);
	if (curpos != -1L) {
	    fseek(fd, 0L, SEEK_END);
		filesize = ftell(fd);
	    fseek(fd, curpos, SEEK_SET);
	}
	return(filesize);
#else
	struct stat sb;
	return (fstat(fd, &sb) == -1 ? 0 : sb.st_size);
#endif // MICROSOFT 
}


/*
 * Read the next TIFF directory from a file
 * and convert it to the internal format.
 * We read directories sequentially.
 */
int 
TIFFReadDirectory(TIFF *tif) {
	register TIFFDirEntry *dp;
	register int n;
	register TIFFDirectory *td = NULL;
	TIFFDirEntry *dir = NULL;
	long v;
	TIFFFieldInfo *fip;
	unsigned short dircount;
	u_short tag;
	int cc;

	tif->tif_diroff = tif->tif_nextdiroff;
	if (tif->tif_diroff == 0)		/* no more directories */
		return (0);
	if (!SeekOK(tif->tif_fd, tif->tif_diroff)) {

		TIFFError(tif->tif_name, "Seek error accessing TIFF directory");
		return (0);
	}
	if (!ReadOK(tif->tif_fd, &dircount, sizeof (short))) {
		TIFFError(tif->tif_name, "Can't read TIFF directory count");
		return (0);
	}
	if (tif->tif_flags & TIFF_SWAB)
		TIFFSwabShort(&dircount);
	dir = (TIFFDirEntry *)alloc(dircount * sizeof (TIFFDirEntry));
	if (dir == NULL) {
		TIFFError(tif->tif_name, "No space to read TIFF directory");
		return (0);
	}
	if (!ReadOK(tif->tif_fd, dir, dircount * sizeof (TIFFDirEntry))) {
		TIFFError(tif->tif_name, "Can't read TIFF directory");
		goto bad;
	}
	/*
	 * Read offset to next directory for sequential scans.
	 */
	if (!ReadOK(tif->tif_fd, &tif->tif_nextdiroff, sizeof (long)))
		tif->tif_nextdiroff = 0;
	if (tif->tif_flags & TIFF_SWAB)
		TIFFSwabLong(&tif->tif_nextdiroff);

	tif->tif_flags &= ~TIFF_BEENWRITING;	/* reset before new dir */
	/*
	 * Setup default value and then make a pass over
	 * the fields to check type and tag information,
	 * and to extract info required to size data
	 * structures.  A second pass is made afterwards
	 * to read in everthing not taken in the first pass.
	 */
	td = &tif->tif_dir;
	if (tif->tif_diroff != tif->tif_header.tiff_diroff) {
		/* free any old stuff and reinit */
		TIFFFreeDirectory(tif);
		bzero((char *)td, sizeof (*td));
	}
	TIFFDefaultDirectory(tif);

	tag = 0;
	for (fip = FieldInfo, dp = dir, n = dircount; n > 0; n--, dp++) {
		if (tif->tif_flags & TIFF_SWAB) {
			TIFFSwabArrayOfShort(&dp->tdir_tag, 2);
			TIFFSwabArrayOfLong(&dp->tdir_count, 2);
		}
		/*
		 * For non-standard files that have out of order tags,
		 * print a warning, then reset the pointer so we try
		 * to parse it anyway.
		 */
#if 0		
		 printf(" TAG = %d (0x%X)\n",dp->tdir_tag,dp->tdir_tag);		
#endif

		if (dp->tdir_tag < tag)
		{
		    TIFFWarning(tif->tif_name,
		        "tag out of order, previous tag: %d (0x%x), new tag %d (0x%x)",
				tag, tag,
				dp->tdir_tag, dp->tdir_tag);
		    fip = FieldInfo;
		}
		tag = dp->tdir_tag; 
		/*
		 * Find the field information entry for this tag.
		 */
		while (fip < &FieldInfo[NFIELDINFO] &&
		    fip->field_tag < tag)
			fip++;
		if (fip >= &FieldInfo[NFIELDINFO] ||
		    fip->field_tag != tag) {
			/*	 Unknown field, should this be fatal? */
#if 0
			TIFFWarning(tif->tif_name,
			    "ignoring unknown field with tag %d (0x%x)",
				dp->tdir_tag, dp->tdir_tag);
#endif
			dp->tdir_tag = IGNORE;
			continue;
		}
		/*
		 * Null out old tags that we ignore.
		 */
		if (fip->field_bit == (u_short)-1) {
			dp->tdir_tag = IGNORE;
			continue;
		}
		/*
		 * Check data type. (now a warning, not an error)
		 */
		while ((TIFFDataType)dp->tdir_type != fip->field_type) {
			fip++;
			if (fip >= &FieldInfo[NFIELDINFO] ||
			    fip->field_tag != dp->tdir_tag) {
				TIFFWarning(tif->tif_name,
				    "Wrong data type %d for field \"%s\"",
				    dp->tdir_type, (--fip)->field_name);
			        dp->tdir_tag = IGNORE;
				break;
			}
		}

		switch (dp->tdir_tag) {
		case IGNORE:
			continue;
		case TIFFTAG_STRIPOFFSETS:
		case TIFFTAG_STRIPBYTECOUNTS:
			TIFFSetFieldBit(tif, fip->field_bit);
			break;
		case TIFFTAG_IMAGELENGTH:
		case TIFFTAG_PLANARCONFIG:
		case TIFFTAG_ROWSPERSTRIP:
		case TIFFTAG_SAMPLESPERPIXEL:
			if (!TIFFSetField(tif, dp->tdir_tag,
			  TIFFExtractData(tif, dp->tdir_type, dp->tdir_offset)))
				goto bad;
			break;
		}
	}

	/*
	 * Allocate directory structure and setup defaults.
	 */
	if (!TIFFFieldSet(tif, FIELD_IMAGEDIMENSIONS)) {
		MissingRequired(tif, "ImageLength");
		goto bad;
	}
	/* some writters don't set PLANARCONFIG we will use default */
	if (!TIFFFieldSet(tif, FIELD_PLANARCONFIG)) {
	    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	    TIFFWarning(tif->tif_name,
	    "Required field PLANARCONFIG not present will use default");
	}
	if (!TIFFFieldSet(tif, FIELD_STRIPOFFSETS)) {
		MissingRequired(tif, "StripOffsets");
		goto bad;
	}
	/* some writters don't set ROWSPERSTRIP we will use default */
	if (!TIFFFieldSet(tif, FIELD_ROWSPERSTRIP)) {
	    TIFFWarning(tif->tif_name,
	    "Required field ROWSPERSTRIP not present will use default");
	}
	td->td_stripsperimage = (td->td_rowsperstrip == 0xffffffff ?
	    1 : howmany(td->td_imagelength, td->td_rowsperstrip));
	td->td_nstrips = td->td_stripsperimage;
	if (td->td_planarconfig == PLANARCONFIG_SEPARATE)
		td->td_nstrips *= td->td_samplesperpixel;

	/*
	 * Second pass: extract other information.
	 *
	 * Should do a better job of verifying values.
	 */
	for (dp = dir, n = dircount; n > 0; n--, dp++) {
		if (dp->tdir_tag == IGNORE)
			continue;
		if (dp->tdir_type == (short)TIF_ASCII) {
			if (dp->tdir_count > 0) {
				ncp = alloc(dp->tdir_count+1);
				if (ncp == NULL || !TIFFFetchData(tif, dp, ncp) ||
				    !TIFFSetField(tif, dp->tdir_tag, ncp))
					goto bad;
			}
			continue;
		}
		if (dp->tdir_type == (short)TIF_RATIONAL) {
			if (!TIFFSetField(tif, dp->tdir_tag,
			    TIFFFetchRational(tif, dp)))
				goto bad;
			continue;
		}
		switch (dp->tdir_tag) {
		case TIFFTAG_COMPRESSION:
			/*
			 * The 5.0 spec says the compression tag has
			 * one value, while earlier specs say it has
			 * one value per sample.  Because of this, we
			 * accept the tag if one value is supplied.
			 */
			if (dp->tdir_count == 1) {
				v = TIFFExtractData(tif,
				    dp->tdir_type, dp->tdir_offset);
				if (!TIFFSetField(tif, TIFFTAG_COMPRESSION, v))
					goto bad;
				break;
			}
			/* fall thru... */
		case TIFFTAG_MINSAMPLEVALUE:
		case TIFFTAG_MAXSAMPLEVALUE:
		case TIFFTAG_BITSPERSAMPLE:
			if (!TIFFFetchPerSampleShorts(tif, dp, &v) ||
			    !TIFFSetField(tif, dp->tdir_tag, v))
				goto bad;
			break;
		case TIFFTAG_STRIPOFFSETS:
		    /* some files assume wrong default ROWSPERSTRIP */
		    if ((td->td_nstrips != dp->tdir_count) &&
		        (!TIFFFieldSet(tif, FIELD_ROWSPERSTRIP))) {
			td->td_nstrips = dp->tdir_count;
			td->td_stripsperimage = td->td_nstrips;
			if (td->td_planarconfig == PLANARCONFIG_SEPARATE)
		            td->td_stripsperimage /= td->td_samplesperpixel;;
		        td->td_rowsperstrip = td->td_imagelength/td->td_stripsperimage;
		        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, td->td_rowsperstrip);
		        TIFFWarning(tif->tif_name,
		         "assuming ROWSPERSTRIP of %d",td->td_rowsperstrip);
		    }
		    if (!TIFFFetchStripThing(tif, dp,
		        td->td_nstrips, &td->td_stripoffset))
				goto bad;
		    TIFFSetFieldBit(tif, FIELD_STRIPOFFSETS);
		    break;
		case TIFFTAG_STRIPBYTECOUNTS:
		    /* some files assume wrong default ROWSPERSTRIP */
		    if ((td->td_nstrips != dp->tdir_count) &&
		        (!TIFFFieldSet(tif, FIELD_ROWSPERSTRIP))) {
			td->td_nstrips = dp->tdir_count;
			td->td_stripsperimage = td->td_nstrips;
			if (td->td_planarconfig == PLANARCONFIG_SEPARATE)
		            td->td_stripsperimage /= td->td_samplesperpixel;;
		        td->td_rowsperstrip = td->td_imagelength/td->td_stripsperimage;
		        TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, td->td_rowsperstrip);
		        TIFFWarning(tif->tif_name,
		         "assuming ROWSPERSTRIP of %d",td->td_rowsperstrip);
		    }
		    if (!TIFFFetchStripThing(tif, dp,
		        td->td_nstrips, &td->td_stripbytecount))
			    goto bad;
		    TIFFSetFieldBit(tif, FIELD_STRIPBYTECOUNTS);
		    break;
		case TIFFTAG_IMAGELENGTH:
		case TIFFTAG_ROWSPERSTRIP:
		case TIFFTAG_SAMPLESPERPIXEL:
		case TIFFTAG_PLANARCONFIG:
			/* handled in first pass above */
			break;
		case TIFFTAG_GRAYRESPONSECURVE:
		case TIFFTAG_COLORRESPONSECURVE:
		case TIFFTAG_COLORMAP:
			v = (1L<<td->td_bitspersample) * sizeof (u_short);
			ncp = alloc(dp->tdir_tag == TIFFTAG_GRAYRESPONSECURVE ? 
				v : 3*v);
			if (ncp == NULL)
				goto bad;
			if (!TIFFSetField(tif, dp->tdir_tag, ncp,ncp+v,ncp+2*v) ||
			    !TIFFFetchData(tif, dp, ncp))
				goto bad;
			break;
/* BEGIN REV 4.0 COMPATIBILITY */
		case TIFFTAG_OSUBFILETYPE:
			v = 0;
			switch (TIFFExtractData(tif, dp->tdir_type,
			    dp->tdir_offset)) {
			case OFILETYPE_REDUCEDIMAGE:
				v = FILETYPE_REDUCEDIMAGE;
				break;
			case OFILETYPE_PAGE:
				v = FILETYPE_PAGE;
				break;
			}
			if (!TIFFSetField(tif, dp->tdir_tag, v))
				goto bad;
			break;
/* END REV 4.0 COMPATIBILITY */
#ifdef DESIGN_VER
		case TIFFTAG_GEOKEYDIR:
			ncp = alloc(2*dp->tdir_count);
			if (!TIFFFetchData(tif, dp, ncp) ||
				!TIFFSetField(tif, dp->tdir_tag, ncp))
				goto bad;
			break;
		case TIFFTAG_MODELPIXELSCALE:
		case TIFFTAG_MODELTRANS:
		case TIFFTAG_MATRIX:
		case TIFFTAG_GEODOUBLE:
			ncp = alloc(8*dp->tdir_count);
			if (!TIFFFetchData(tif, dp, ncp) ||
				!TIFFSetField(tif, dp->tdir_tag, ncp))
				goto bad;
			break;
		case TIFFTAG_MODELTIES:
			ncp = alloc(8*dp->tdir_count);
			if (!TIFFFetchData(tif, dp, ncp) ||
				!TIFFSetField(tif, dp->tdir_tag, dp->tdir_count, ncp))
				goto bad;
			break;
#endif
		default:
			if (!TIFFSetField(tif, dp->tdir_tag,
			  TIFFExtractData(tif, dp->tdir_type, dp->tdir_offset)))
				goto bad;
			break;
		}
	}
	if (!TIFFFieldSet(tif, FIELD_STRIPBYTECOUNTS)) {
		u_long file_size;
		u_long space;
		/*
		 * Some manufacturers violate the spec by not giving
		 * the size of the strips.  In this case, assume there
		 * is one uncompressed strip of data.
		 */
		if (td->td_nstrips > 1) {
			MissingRequired(tif, "StripByteCounts");
			goto bad;
		}
		TIFFWarning(tif->tif_name,
"TIFF directory is missing required \"%s\" field, calculating from imagelength",
		    FieldWithTag(TIFFTAG_STRIPBYTECOUNTS)->field_name);

		file_size = TIFFGetFileSize(tif->tif_fd);
		td->td_stripbytecount = (u_long *)alloc(sizeof (u_long));
		space = sizeof (TIFFHeader)
		    + sizeof (short)
		    + (dircount * sizeof (TIFFDirEntry))
		    + sizeof (long);
		/* calculate amount of space used by indirect values */
		for (dp = dir, n = dircount; n > 0; n--, dp++) {
                        switch(dp->tdir_type) {
                          case 0:
                          case 1:
                          case 2: cc = dp->tdir_count * 1;
				  break;
                          case 3: cc = dp->tdir_count * 2;
				  break;
                          case 4: cc = dp->tdir_count * 4;
				  break;
                          case 5: cc = dp->tdir_count * 8;
				  break;
                        }
/*			int cc = dp->tdir_count * datawidth[dp->tdir_type];*/
			if (cc > sizeof (long))
				space += cc;
		}

		if (TIFFFieldSet(tif, FIELD_STRIPOFFSETS))
			td->td_stripbytecount[0] = file_size - td->td_stripoffset[0];
		else 
			td->td_stripbytecount[0] = file_size - space;

		TIFFSetFieldBit(tif, FIELD_STRIPBYTECOUNTS);
		if (!TIFFFieldSet(tif, FIELD_ROWSPERSTRIP))
			td->td_rowsperstrip = td->td_imagelength;
	}
	if (dir) 
	    free((char *)dir);
	if (!TIFFFieldSet(tif, FIELD_MAXSAMPLEVALUE))
		td->td_maxsamplevalue = (1L<<td->td_bitspersample)-1;
	/*
	 * Setup default compression scheme.
	 */
	return (!TIFFFieldSet(tif, FIELD_COMPRESSION) ?
	    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE) : 1);
bad:
	if (dir)
            free((char *)dir);

	return (0);
}

static void
MissingRequired(tif, tagname)
	TIFF *tif;
	char *tagname;
{

	TIFFError(tif->tif_name,
	    "TIFF directory  missing \"%s\" field", tagname);
}

/*
 * Fetch a contiguous directory item.
 */
static
TIFFFetchData(TIFF *tif,TIFFDirEntry *dir,char *cp) {
	int cc, w;

   switch(dir->tdir_type) {
		case 0:
		case 1:
		case 2: w = 1;
			  break;
		case 3: w = 2;
			break;
		case 4: w = 4;
			break;
#ifdef DESIGN_VER
		case 12:
#endif
		case 5: w = 8;
			break;
		}
	/*w = datawidth[dir->tdir_type];*/
	cc = dir->tdir_count * w;
	if (SeekOK(tif->tif_fd, dir->tdir_offset) &&
	    ReadOK(tif->tif_fd, cp, cc)) {
		if (tif->tif_flags & TIFF_SWAB) {
			switch (dir->tdir_type) {
			case TIF_SHORT:
				TIFFSwabArrayOfShort((short *)cp, dir->tdir_count);     // KINETIX:  added cast to short *
				break;
			case TIF_LONG:
				TIFFSwabArrayOfLong((long *)cp, dir->tdir_count);       // KINETIX:  added cast to long *
				break;
			case TIF_RATIONAL:
				TIFFSwabArrayOfLong((long *)cp, 2*dir->tdir_count);     // KINETIX:  added cast to long *
				break;
			}
			}
		return (cc);
		}
	TIFFError(tif->tif_name, "Error fetching data for field \"%s\"",
	    FieldWithTag(dir->tdir_tag)->field_name);
	return (0);
	}

/*
 * Fetch a rational item from the file
 * at offset off.  We return the value
 * as floating point number.
 */
static double
TIFFFetchRational(tif, dir)
	TIFF *tif;
	TIFFDirEntry *dir;
{
	long l[2];

	if (!TIFFFetchData(tif, dir, (char *)l))
		return (1.);
	if (l[1] == 0) {
		TIFFError(tif->tif_name, "%s: Rational with zero denominator",
		    FieldWithTag(dir->tdir_tag)->field_name);
		return (1.);
	}
	return ((double)l[0] / (double)l[1]);
}

static
TIFFFetchPerSampleShorts(tif, dir, pl)
	TIFF *tif;
	TIFFDirEntry *dir;
	long *pl;
{
	u_short v[4];
	unsigned int i;     // KINETIX: changed type to from merely int

	switch (dir->tdir_count) {
	case 1:
		*pl = TIFFExtractData(tif, dir->tdir_type, dir->tdir_offset);
		return (1);		/* XXX should check samplesperpixel */
	case 2:
		if (tif->tif_header.tiff_magic == TIFF_BIGENDIAN) {
			v[0] = (u_short)dir->tdir_offset >> 16;
			v[1] = (u_short)dir->tdir_offset & 0xffff;
		} else {
			v[0] = (u_short)dir->tdir_offset & 0xffff;
			v[1] = (u_short)dir->tdir_offset >> 16;
		}
		break;
	default:
		if (!TIFFFetchData(tif, dir, (char *)v))
			return (0);
		break;
	}
	if (tif->tif_dir.td_samplesperpixel != dir->tdir_count) {
		TIFFError(tif->tif_name, "Incorrect count %d for field \"%s\"",
		    dir->tdir_count, FieldWithTag(dir->tdir_tag)->field_name);
		return (0);
	}
	for (i = 1; i < dir->tdir_count; i++)
		if (v[i] != v[0]) {
			TIFFError(tif->tif_name,
	"Cannot handle different per-sample values for field \"%s\"",
			     FieldWithTag(dir->tdir_tag)->field_name);
			return (0);
		}
	*pl = v[0];
	return (1);
}

static
TIFFFetchStripThing(tif, dir, nstrips, lpp)
	TIFF *tif;
	TIFFDirEntry *dir;
	u_long nstrips;
	u_long **lpp;
{
        char *dp;
	register u_long *lp;
	int status;

	if (nstrips != dir->tdir_count) {
		TIFFWarning(tif->tif_name,
		    "Count mismatch for field \"%s\"; expecting %d, got %d",
		    FieldWithTag(dir->tdir_tag)->field_name, nstrips,
		    dir->tdir_count);
		nstrips = dir->tdir_count;
		}
	/*
	 * Allocate space for strip information.
	 */
	if (*lpp == NULL &&
	    (*lpp = (u_long *)alloc(nstrips * sizeof (u_long))) == NULL) {
		TIFFError(tif->tif_name, "No space for \"%s\" array",
		    FieldWithTag(dir->tdir_tag)->field_name);
		return (0);
		}
	lp = *lpp;
	status = 1;
	if (dir->tdir_type == (int)TIF_SHORT) {
		/*
		 * Handle short->long expansion.
		 */
		if (dir->tdir_count > 2) {
			switch((int)TIF_SHORT) {
				case 0:
				case 1:
				case 2: dp = (char *)alloc(dir->tdir_count * 1);
					break;
				case 3: dp = (char *)alloc(dir->tdir_count * 2);
					break;
				case 4: dp = (char *)alloc(dir->tdir_count * 4);
					break;
				case 5: dp = (char *)alloc(dir->tdir_count * 8);
					break;
				}
			/*char *dp =  alloc(dir->tdir_count * datawidth[(int)TIF_SHORT]);*/
			if (dp == NULL) {
				TIFFError(tif->tif_name,
				    "No memory to fetch field \"%s\"",
				    FieldWithTag(dir->tdir_tag)->field_name);
				return (0);
				}
			if (status = TIFFFetchData(tif, dir, dp)) {
				register u_short *wp = (u_short *)dp;
				while (nstrips-- > 0)
					*lp++ = *wp++;
			}
			free(dp);
		} else {
			/*
			 * Extract data from offset field.
			 */
			if (tif->tif_header.tiff_magic == TIFF_BIGENDIAN) {
				*lp++ = dir->tdir_offset >> 16;
				*lp = dir->tdir_offset & 0xffff;
			} else {
				*lp++ = dir->tdir_offset & 0xffff;
				*lp = dir->tdir_offset >> 16;
			}
		}
	} else {
		if (dir->tdir_count > 1)
			status = TIFFFetchData(tif, dir, (char *)lp);
		else
			*lp = dir->tdir_offset;
	}
	return (status);
}

static TIFFFieldInfo *
FieldWithTag(tag)
	u_short tag;
{
	register TIFFFieldInfo *fip;

	for (fip = FieldInfo; fip < &FieldInfo[NFIELDINFO]; fip++)
		if (fip->field_tag == tag)
			return (fip);
	TIFFError("FieldWithTag", "Internal error, unknown tag 0x%x", tag);
	return NULL;
}

void
TIFFFreeDirectory(tif)
	TIFF *tif;
{
	register TIFFDirectory *td = &tif->tif_dir;

	if (TIFFFieldSet(tif, FIELD_GRAYRESPONSECURVE))
		free((char *)td->td_grayresponsecurve);
	if (TIFFFieldSet(tif, FIELD_COLORRESPONSECURVE)) {
		free((char *)td->td_redresponsecurve);
		/* free((char *)td->td_blueresponsecurve); */
		/* free((char *)td->td_greenresponsecurve); */
	}
	if (TIFFFieldSet(tif, FIELD_COLORMAP)) {
		if ((unsigned short)tif->tif_mode == (O_RDONLY | O_BINARY))
		free((char *)td->td_redcolormap);
		/* free((char *)td->td_bluecolormap); */
		/* free((char *)td->td_greencolormap); */
	}
#ifdef notdef
	/* can't free -- user supplied strings may be static */
	if (TIFFFieldSet(tif, FIELD_DOCUMENTNAME))
		free(td->td_documentname);
	if (TIFFFieldSet(tif, FIELD_ARTIST))
		free(td->td_artist);
	if (TIFFFieldSet(tif, FIELD_DATETIME))
		free(td->td_datetime);
	if (TIFFFieldSet(tif, FIELD_HOSTCOMPUTER))
		free(td->td_hostcomputer);
	if (TIFFFieldSet(tif, FIELD_IMAGEDESCRIPTION))
		free(td->td_imagedescription);
	if (TIFFFieldSet(tif, FIELD_MAKE))
		free(td->td_make);
	if (TIFFFieldSet(tif, FIELD_MODEL))
		free(td->td_model);
	if (TIFFFieldSet(tif, FIELD_SOFTWARE))
		free(td->td_software);
	if (TIFFFieldSet(tif, FIELD_PAGENAME))
		free(td->td_pagename);
#endif
	if  (td->td_stripoffset)
		free((char *)td->td_stripoffset);
	if  (td->td_stripbytecount)
		free((char *)td->td_stripbytecount);
#if 0
   if (ncp != NULL) {
     free(ncp);
     ncp = NULL;
     }
#endif
}

/*
 * Setup a default directory structure.
 */
TIFFDefaultDirectory(tif)
	TIFF *tif;
{
	register TIFFDirectory *td = &tif->tif_dir;

	td->td_bitspersample = 1;
	td->td_threshholding = THRESHHOLD_BILEVEL;
	td->td_orientation = ORIENTATION_TOPLEFT;
	td->td_samplesperpixel = 1;
	td->td_predictor = 1;
	td->td_rowsperstrip = 0xfffffff;
	td->td_grayresponseunit = GRAYRESPONSEUNIT_100S;
	td->td_resolutionunit = RESUNIT_INCH;
	td->td_colorresponseunit = COLORRESPONSEUNIT_100S;
	return (TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE));
}


#ifdef KINETIX_TIFF_CHANGES
    extern TIFFSetCompressionScheme (TIFF *tif, int scheme);
#endif
/*
 * Record the value of a field in the
 * internal directory structure.  The
 * field will be written to the file
 * when/if the directory structure is
 * updated.
 */
/*VARARGS2*/
#ifdef __STDC__
#ifdef __MACH__		/* NeXT really */
TIFFSetField(TIFF *tif, int tag, ...)
#else
TIFFSetField(TIFF *tif, u_short tag, ...)
#endif /* __MACH__ */
#else
TIFFSetField(tif, tag, va_alist)
	TIFF *tif;
	u_short tag;
	va_dcl
#endif
{
	static char module[] = "TIFFSetField";
	TIFFDirectory *td = &tif->tif_dir;
	va_list ap;
	long v;
	int field = -1, status = 1;

	if (tag != TIFFTAG_IMAGELENGTH && (tif->tif_flags & TIFF_BEENWRITING)) {
		TIFFError(module,
		    "%s: Cannot change TIFF directory while writing",
		    tif->tif_name);
		return (0);
	}
#ifdef __STDC__
	va_start(ap, tag);
#else
	va_start(ap);
#endif
	switch (tag) {
	case TIFFTAG_SUBFILETYPE:
		td->td_subfiletype = (u_short)va_arg(ap, long);
		field = FIELD_SUBFILETYPE;
		break;
	case TIFFTAG_IMAGEWIDTH:
		td->td_imagewidth = va_arg(ap, int);
		field = FIELD_IMAGEDIMENSIONS;
		break;
	case TIFFTAG_IMAGELENGTH:
		td->td_imagelength = va_arg(ap, int);
		field = FIELD_IMAGEDIMENSIONS;
		break;
	case TIFFTAG_BITSPERSAMPLE:
		td->td_bitspersample = va_arg(ap, int);
		field = FIELD_BITSPERSAMPLE;
		break;
	case TIFFTAG_COMPRESSION:
		v = va_arg(ap, int) & 0xffff;
		/*
		 * If we're changing the compression scheme,
		 * the notify the previous module so that it
		 * can cleanup any state it's setup.
		 */
		if (TIFFFieldSet(tif, FIELD_COMPRESSION)) {
			if (td->td_compression == v)
				break;
			if (tif->tif_cleanup)
				(*tif->tif_cleanup)(tif);
		}
		/*
		 * Setup new compression routine state.
		 */
		if (status = TIFFSetCompressionScheme(tif, v)) {
			td->td_compression = (u_short)v;
			field = FIELD_COMPRESSION;
		}
		break;
	case TIFFTAG_PHOTOMETRIC:
		td->td_photometric = va_arg(ap, int);
		field = FIELD_PHOTOMETRIC;
		break;
	case TIFFTAG_THRESHHOLDING:
		td->td_threshholding = va_arg(ap, int);
		field = FIELD_THRESHHOLDING;
		break;
	case TIFFTAG_DOCUMENTNAME:
		td->td_documentname = va_arg(ap, char *);
		field = FIELD_DOCUMENTNAME;
		break;
	case TIFFTAG_ARTIST:
		td->td_artist = va_arg(ap, char *);
		field = FIELD_ARTIST;
		break;
	case TIFFTAG_DATETIME:
		td->td_datetime = va_arg(ap, char *);
		field = FIELD_DATETIME;
		break;
	case TIFFTAG_HOSTCOMPUTER:
		td->td_hostcomputer = va_arg(ap, char *);
		field = FIELD_HOSTCOMPUTER;
		break;
	case TIFFTAG_IMAGEDESCRIPTION:
		td->td_imagedescription = va_arg(ap, char *);
		field = FIELD_IMAGEDESCRIPTION;
		break;
	case TIFFTAG_MAKE:
		td->td_make = va_arg(ap, char *);
		field = FIELD_MAKE;
		break;
	case TIFFTAG_MODEL:
		td->td_model = va_arg(ap, char *);
		field = FIELD_MODEL;
		break;
	case TIFFTAG_SOFTWARE:
		td->td_software = va_arg(ap, char *);
		field = FIELD_SOFTWARE;
		break;
	case TIFFTAG_ORIENTATION:
		td->td_orientation = va_arg(ap, int);
		field = FIELD_ORIENTATION;
		break;
	case TIFFTAG_SAMPLESPERPIXEL:
		v = va_arg(ap, int);
		if (v == 0)
			goto badvalue;
		if (v > 4) {
			TIFFError(tif->tif_name,
			    "Cannot handle %d-channel data", v); 
			goto bad;
		}
		td->td_samplesperpixel = (u_short)v;
		field = FIELD_SAMPLESPERPIXEL;
		break;
	case TIFFTAG_ROWSPERSTRIP:
		v = va_arg(ap, long);
		if (v == 0)
			goto badvalue;
		td->td_rowsperstrip = v;
		field = FIELD_ROWSPERSTRIP;
		break;
	case TIFFTAG_MINSAMPLEVALUE:
		td->td_minsamplevalue = va_arg(ap, int) & 0xffff;
		field = FIELD_MINSAMPLEVALUE;
		break;
	case TIFFTAG_MAXSAMPLEVALUE:
		td->td_maxsamplevalue = va_arg(ap, int) & 0xffff;
		field = FIELD_MAXSAMPLEVALUE;
		break;
	case TIFFTAG_XRESOLUTION:
		td->td_xresolution = (float)va_arg(ap, VA_FLOAT_PARAM_TYPE);
		field = FIELD_RESOLUTION;
		break;
	case TIFFTAG_YRESOLUTION:
		td->td_yresolution = (float)va_arg(ap, VA_FLOAT_PARAM_TYPE);
		field = FIELD_RESOLUTION;
		break;
	case TIFFTAG_PLANARCONFIG:
		v = va_arg(ap, int);
		td->td_planarconfig = (u_short)v;
		field = FIELD_PLANARCONFIG;
		break;
	case TIFFTAG_PAGENAME:
		td->td_pagename = va_arg(ap, char *);
		field = FIELD_PAGENAME;
		break;
	case TIFFTAG_XPOSITION:
		td->td_xposition = (float)va_arg(ap, VA_FLOAT_PARAM_TYPE);
		field = FIELD_POSITION;
		break;
	case TIFFTAG_YPOSITION:
		td->td_yposition = (float)va_arg(ap, VA_FLOAT_PARAM_TYPE);
		field = FIELD_POSITION;
		break;
	case TIFFTAG_GRAYRESPONSEUNIT:
		td->td_grayresponseunit = va_arg(ap, int);
		field = FIELD_GRAYRESPONSEUNIT;
		break;
	case TIFFTAG_GRAYRESPONSECURVE:
		td->td_grayresponsecurve = va_arg(ap, u_short *);
		field = FIELD_GRAYRESPONSECURVE;
		break;
	case TIFFTAG_GROUP3OPTIONS:
		td->td_group3options = va_arg(ap, long);
		field = FIELD_GROUP3OPTIONS;
		break;
	case TIFFTAG_GROUP4OPTIONS:
		td->td_group4options = va_arg(ap, long);
		field = FIELD_GROUP4OPTIONS;
		break;
	case TIFFTAG_RESOLUTIONUNIT:
		td->td_resolutionunit = va_arg(ap, int);
		field = FIELD_RESOLUTIONUNIT;
		break;
	case TIFFTAG_PAGENUMBER:
		td->td_pagenumber = va_arg(ap, int);
		field = FIELD_PAGENUMBER;
		break;
	case TIFFTAG_COLORRESPONSEUNIT:
		td->td_colorresponseunit = va_arg(ap, int);
		field = FIELD_COLORRESPONSEUNIT;
		break;
	case TIFFTAG_COLORRESPONSECURVE:
		td->td_redresponsecurve = va_arg(ap, u_short *);
		td->td_greenresponsecurve = va_arg(ap, u_short *);
		td->td_blueresponsecurve = va_arg(ap, u_short *);
		field = FIELD_COLORRESPONSECURVE;
		break;
	case TIFFTAG_COLORMAP:
		td->td_redcolormap = va_arg(ap, u_short *);
		td->td_greencolormap = va_arg(ap, u_short *);
		td->td_bluecolormap = va_arg(ap, u_short *);
		field = FIELD_COLORMAP;
		break;
	case TIFFTAG_PREDICTOR:
		td->td_predictor = va_arg(ap, int);
		field = FIELD_PREDICTOR;
		break;
	case TIFFTAG_MATTEING:
		td->td_matteing = va_arg(ap, int);
		field = FIELD_MATTEING;
		break;
#ifdef DESIGN_VER
	case TIFFTAG_MODELPIXELSCALE:
		td->td_modpixelscale = va_arg(ap, double *);
		field = FIELD_MODELPIXELSCALE;
		break;
	case TIFFTAG_MODELTRANS:
		td->td_modtrans = va_arg(ap, double *);
		field = FIELD_MODELTRANS;
		break;
	case TIFFTAG_MATRIX:
		td->td_matrix = va_arg(ap, double *);
		field = FIELD_MATRIX;
		break;
	case TIFFTAG_MODELTIES:
		td->td_modtiescount = va_arg(ap, u_short);
		td->td_modties = va_arg(ap, double *);
		field = FIELD_MODELTIES;
		break;
	case TIFFTAG_GEOKEYDIR:
		td->td_geokeydir = va_arg(ap, u_short *);
		field = FIELD_GEOKEYDIR;
		break;
	case TIFFTAG_GEODOUBLE:
		td->td_geodouble = va_arg(ap, double *);
		field = FIELD_GEODOUBLE;
		break;
	case TIFFTAG_GEOASCII:
		td->td_geoascii = va_arg(ap, char *);
		field = FIELD_GEOASCII;
		break;
#endif
	}
	if (field >= 0) {
		TIFFSetFieldBit(tif, field);
		tif->tif_flags |= TIFF_DIRTYDIRECT;
	}
	va_end(ap);
	return (status);
badvalue:
	TIFFError(tif->tif_name, "%d: Bad value for \"%s\"", v,
	    FieldWithTag(tag)->field_name);
bad:
	va_end(ap);
	return (0);
}

/*
 * Return the value of a field in the
 * internal directory structure.
 */
#ifdef __STDC__
#ifdef __MACH__		/* NeXT really */
TIFFGetField(TIFF *tif, int tag, ...)
#else
TIFFGetField(TIFF *tif, u_short tag, ...)
#endif /* __MACH__ */
#else
TIFFGetField(tif, tag, va_alist)
	TIFF *tif;
	u_short tag;
	va_dcl
#endif
{
	register TIFFFieldInfo *fip;
	TIFFDirectory *td = &tif->tif_dir;
	va_list ap;

	for (fip = FieldInfo; fip < &FieldInfo[NFIELDINFO]; fip++)
		if (fip->field_tag == tag)
			break;
	if (fip >= &FieldInfo[NFIELDINFO]) {
		TIFFError("TIFFGetField", "Unknown field, tag 0x%x", tag);
		return (0);
	}
	if (TIFFFieldSet(tif, fip->field_bit)) {
#ifdef __STDC__
		va_start(ap, tag);
#else
		va_start(ap);
#endif
		(void) TIFFGetField1(td, tag, ap);
		va_end(ap);
		return (1);
	}
	return (0);
}

static void 
TIFFGetField1(td, tag, ap)
	TIFFDirectory *td;
	u_short tag;
	va_list ap;
{

	switch (tag) {
	case TIFFTAG_SUBFILETYPE:
		*va_arg(ap, u_long *) = td->td_subfiletype;
		break;
	case TIFFTAG_IMAGEWIDTH:
		*va_arg(ap, u_short *) = td->td_imagewidth;
		break;
	case TIFFTAG_IMAGELENGTH:
		*va_arg(ap, u_short *) = td->td_imagelength;
		break;
	case TIFFTAG_BITSPERSAMPLE:
		*va_arg(ap, u_short *) = td->td_bitspersample;
		break;
	case TIFFTAG_COMPRESSION:
		*va_arg(ap, u_short *) = td->td_compression;
		break;
	case TIFFTAG_PHOTOMETRIC:
		*va_arg(ap, u_short *) = td->td_photometric;
		break;
	case TIFFTAG_THRESHHOLDING:
		*va_arg(ap, u_short *) = td->td_threshholding;
		break;
	case TIFFTAG_DOCUMENTNAME:
		*va_arg(ap, char **) = td->td_documentname;
		break;
	case TIFFTAG_ARTIST:
		*va_arg(ap, char **) = td->td_artist;
		break;
	case TIFFTAG_DATETIME:
		*va_arg(ap, char **) = td->td_datetime;
		break;
	case TIFFTAG_HOSTCOMPUTER:
		*va_arg(ap, char **) = td->td_hostcomputer;
		break;
	case TIFFTAG_IMAGEDESCRIPTION:
		*va_arg(ap, char **) = td->td_imagedescription;
		break;
	case TIFFTAG_MAKE:
		*va_arg(ap, char **) = td->td_make;
		break;
	case TIFFTAG_MODEL:
		*va_arg(ap, char **) = td->td_model;
		break;
	case TIFFTAG_SOFTWARE:
		*va_arg(ap, char **) = td->td_software;
		break;
	case TIFFTAG_ORIENTATION:
		*va_arg(ap, u_short *) = td->td_orientation;
		break;
	case TIFFTAG_SAMPLESPERPIXEL:
		*va_arg(ap, u_short *) = td->td_samplesperpixel;
		break;
	case TIFFTAG_ROWSPERSTRIP:
		*va_arg(ap, u_long *) = td->td_rowsperstrip;
		break;
	case TIFFTAG_MINSAMPLEVALUE:
		*va_arg(ap, u_short *) = (u_short)td->td_minsamplevalue;    // KINETIX: supplied cast (what should it really be?)
		break;
	case TIFFTAG_MAXSAMPLEVALUE:
		*va_arg(ap, u_short *) = (u_short)td->td_maxsamplevalue;    // KINETIX: supplied cast (what should it really be?)
		break;
	case TIFFTAG_XRESOLUTION:
		*va_arg(ap, float *) = td->td_xresolution;
		break;
	case TIFFTAG_YRESOLUTION:
		*va_arg(ap, float *) = td->td_yresolution;
		break;
	case TIFFTAG_PLANARCONFIG:
		*va_arg(ap, u_short *) = td->td_planarconfig;
		break;
	case TIFFTAG_XPOSITION:
		*va_arg(ap, float *) = td->td_xposition;
		break;
	case TIFFTAG_YPOSITION:
		*va_arg(ap, float *) = td->td_yposition;
		break;
	case TIFFTAG_PAGENAME:
		*va_arg(ap, char **) = td->td_pagename;
		break;
	case TIFFTAG_GRAYRESPONSEUNIT:
		*va_arg(ap, u_short *) = td->td_grayresponseunit;
		break;
	case TIFFTAG_GRAYRESPONSECURVE:
		*va_arg(ap, u_short **) = td->td_grayresponsecurve;
		break;
	case TIFFTAG_GROUP3OPTIONS:
		*va_arg(ap, u_long *) = td->td_group3options;
		break;
	case TIFFTAG_GROUP4OPTIONS:
		*va_arg(ap, u_long *) = td->td_group4options;
		break;
	case TIFFTAG_RESOLUTIONUNIT:
		*va_arg(ap, u_short *) = td->td_resolutionunit;
		break;
	case TIFFTAG_PAGENUMBER:
		*va_arg(ap, u_long *) = td->td_pagenumber;
		break;
	case TIFFTAG_COLORRESPONSEUNIT:
		*va_arg(ap, u_short *) = td->td_colorresponseunit;
		break;
	case TIFFTAG_COLORRESPONSECURVE:
		*va_arg(ap, u_short **) = td->td_redresponsecurve;
		*va_arg(ap, u_short **) = td->td_greenresponsecurve;
		*va_arg(ap, u_short **) = td->td_blueresponsecurve;
		break;
	case TIFFTAG_COLORMAP:
		*va_arg(ap, u_short **) = td->td_redcolormap;
		*va_arg(ap, u_short **) = td->td_greencolormap;
		*va_arg(ap, u_short **) = td->td_bluecolormap;
		break;
	case TIFFTAG_PREDICTOR:
		*va_arg(ap, u_short *) = td->td_predictor;
		break;
	case TIFFTAG_STRIPOFFSETS:
		*va_arg(ap, u_long **) = td->td_stripoffset;
		break;
	case TIFFTAG_STRIPBYTECOUNTS:
		*va_arg(ap, u_long **) = td->td_stripbytecount;
		break;
	case TIFFTAG_MATTEING:
		*va_arg(ap, u_short *) = td->td_matteing;
		break;
	}
	va_end(ap);
}

/*
 * Internal interface to TIFFGetField...
 */
static
#ifdef __STDC__
TIFFgetfield(TIFFDirectory *td, u_short tag, ...)
#else
TIFFgetfield(td, tag, va_alist)
	TIFFDirectory *td;
	u_short tag;
	va_dcl
#endif
{
	va_list ap;

#ifdef __STDC__
	va_start(ap, tag);
#else
	va_start(ap);
#endif
	(void) TIFFGetField1(td, tag, ap);
	va_end(ap);
#ifdef KINETIX_TIFF_CHANGES
    return 1;   // arbitrary, but int function must return a value
#endif
}

/* shorthands for setting up and writing directory... */
#define	MakeShortDirent(tag, v) \
	dir->tdir_tag = tag; \
	dir->tdir_type = (short)TIF_SHORT; \
	dir->tdir_count = 1; \
	dir->tdir_offset = TIFFInsertData(tif, (int)TIF_SHORT, v); \
	dir++
#define	WriteRationalPair(tag1, v1, tag2, v2) \
	(TIFFWriteRational(tif, tag1, dir++, v1) && \
	 TIFFWriteRational(tif, tag2, dir++, v2))

static	long dataoff;

#ifdef DOWRITE
/*
 * Write the contents of the current directory
 * to the specified file.  This routine doesn't
 * handle overwriting a directory with auxiliary
 * storage that's been changed.
 */
TIFFWriteDirectory(TIFF *tif) {
	short dircount, v;
	int nfields, dirsize;
	char *data, *cp;
	TIFFFieldInfo *fip;
	TIFFDirEntry *dir;
	TIFFDirectory *td;
	u_long b, off, fields[sizeof (td->td_fieldsset) / sizeof (u_long)];

	if (tif->tif_mode == O_RDONLY)
		return (1);
	td = &tif->tif_dir;
	/*
	 * Size the directory so that we can calculate
	 * offsets for the data items that aren't kept
	 * in-place in each field.
	 */
	nfields = 0;
	for (b = 0; b <= FIELD_LAST; b++)
		if (TIFFFieldSet(tif, b))
			nfields += (b < FIELD_SUBFILETYPE ? 2 : 1);
	dirsize = nfields * sizeof (TIFFDirEntry);
	data = alloc(dirsize);
	if (data == NULL) {
		TIFFError(tif->tif_name,
		    "Cannot write directory, out of space");
		return (0);
	}
	/*
	 * Directory hasn't been placed yet, put
	 * it at the end of the file and link it
	 * into the existing directory structure.
	 */
	if (tif->tif_diroff == 0 && !TIFFLinkDirectory(tif))
		return (0);
	dataoff = tif->tif_diroff + sizeof (short) + dirsize + sizeof (long);
	if (dataoff & 1)
		dataoff++;
	(void) myseek(tif->tif_fd, dataoff, L_SET);
	dir = (TIFFDirEntry *)data;
	/*
	 * Setup external form of directory
	 * entries and write data items.
	 */
	bcopy(td->td_fieldsset, fields, sizeof (fields));
	for (fip = FieldInfo; fip < &FieldInfo[NFIELDINFO]; fip++) {
		if (fip->field_bit == (u_short)-1 ||
		    !FieldSet(fields, fip->field_bit))
			continue;
		if (fip->field_type == TIF_ASCII) {
			TIFFgetfield(td, fip->field_tag, &cp);
			dir->tdir_tag = fip->field_tag;
			dir->tdir_type = (short)TIF_ASCII;
			dir->tdir_count = strlen(cp) + 1;
			if (!TIFFWriteData(tif, dir++, cp))
				goto bad;
			ResetFieldBit(fields, fip->field_bit);
			continue;
		}
		switch (fip->field_bit) {
		case FIELD_STRIPOFFSETS:
		case FIELD_STRIPBYTECOUNTS:
			if (!TIFFWriteStripThing(tif,
			    fip->field_tag, dir++,
			    fip->field_bit == FIELD_STRIPOFFSETS ?
			      td->td_stripoffset : td->td_stripbytecount))
				goto bad;
			break;
		case FIELD_GRAYRESPONSECURVE:
			dir->tdir_tag = fip->field_tag;
			dir->tdir_type = (short)TIF_SHORT;
			dir->tdir_count = 1L<<td->td_bitspersample;
			if (!TIFFWriteData(tif, dir++,td->td_grayresponsecurve))
				goto bad;
			break;
		case FIELD_COLORRESPONSECURVE:
		case FIELD_COLORMAP:
			dir->tdir_tag = fip->field_tag;
			dir->tdir_type = (short)TIF_SHORT;
			/* XXX -- yech, fool TIFFWriteData */
			dir->tdir_count = 1L<<td->td_bitspersample;
			off = dataoff;
#define	WRITE(x)	TIFFWriteData(tif, dir, x)
			if (fip->field_tag == TIFFTAG_COLORMAP) {
				if (!WRITE(td->td_redcolormap) ||
				    !WRITE(td->td_greencolormap) ||
				    !WRITE(td->td_bluecolormap))
					goto bad;
			} else {
				if (!WRITE(td->td_redresponsecurve) ||
				    !WRITE(td->td_greenresponsecurve) ||
				    !WRITE(td->td_blueresponsecurve))
					goto bad;
			}
#undef WRITE
			dir->tdir_count *= 3;
			dir->tdir_offset = off;
			break;
		case FIELD_IMAGEDIMENSIONS:
			MakeShortDirent(TIFFTAG_IMAGEWIDTH, td->td_imagewidth);
			MakeShortDirent(TIFFTAG_IMAGELENGTH,td->td_imagelength);
			break;
		case FIELD_POSITION:
			if (!WriteRationalPair(
			    TIFFTAG_XPOSITION, td->td_xposition,
			    TIFFTAG_YPOSITION, td->td_yposition))
				goto bad;
			break;
		case FIELD_RESOLUTION:
			if (!WriteRationalPair(
			    TIFFTAG_XRESOLUTION, td->td_xresolution,
			    TIFFTAG_YRESOLUTION, td->td_yresolution))
				goto bad;
			break;
		case FIELD_BITSPERSAMPLE:
		case FIELD_MINSAMPLEVALUE:
		case FIELD_MAXSAMPLEVALUE:
			TIFFgetfield(td, fip->field_tag, &v);
			if (!TIFFWritePerSampleShorts(tif, fip->field_tag,
			    dir++, v))
				goto bad;
			break;
		default:
			dir->tdir_tag = fip->field_tag;
			dir->tdir_type = (short)fip->field_type;
			dir->tdir_count = fip->field_count;
			if (fip->field_type == TIF_SHORT) {
				TIFFgetfield(td, fip->field_tag, &v);
				dir->tdir_offset =
				    TIFFInsertData(tif, dir->tdir_type, v);
			} else
				TIFFgetfield(td, fip->field_tag,
				    &dir->tdir_offset);
			dir++;
			break;
		}
		ResetFieldBit(fields, fip->field_bit);
	}
	/*
	 * Write directory.
	 */
	(void) myseek(tif->tif_fd, tif->tif_diroff, L_SET);
	dircount = nfields;
	if (!WriteOK(tif->tif_fd, &dircount, sizeof (short))) {
		TIFFError(tif->tif_name, "Error writing directory count");
		goto bad;
	}
	if (!WriteOK(tif->tif_fd, data, dirsize)) {
		TIFFError(tif->tif_name, "Error writing directory contents");
		goto bad;
	}
	if (!WriteOK(tif->tif_fd, &tif->tif_nextdiroff, sizeof (long))) {
		TIFFError(tif->tif_name, "Error writing directory link");
		goto bad;
	}
	free(data);
	return (1);
bad:
	free(data);
	return (0);
}
#undef MakeShortDirent
#undef WriteRationalPair

static
TIFFWriteRational(tif, tag, dir, v)
	TIFF *tif;
	u_short tag;
	TIFFDirEntry *dir;
	float v;
{
	long t[2];

	dir->tdir_tag = tag;
	dir->tdir_type = (short)TIF_RATIONAL;
	dir->tdir_count = 1;
	/* need algorithm to convert ... XXX */
	t[0] = v * 10000.;
	t[1] = 10000;
	return (TIFFWriteData(tif, dir, t));
}

static
TIFFWritePerSampleShorts(tif, tag, dir, v)
	TIFF *tif;
	u_short tag;
	register TIFFDirEntry *dir;
	short v;
{
	short w[4];
	int i, samplesperpixel = tif->tif_dir.td_samplesperpixel;

	dir->tdir_tag = tag;
	dir->tdir_type = (short)TIF_SHORT;
	dir->tdir_count = samplesperpixel;
	if (samplesperpixel <= 2) {
		if (tif->tif_header.tiff_magic == TIFF_BIGENDIAN) {
			dir->tdir_offset = (long)v << 16;
			if (samplesperpixel == 2)
				dir->tdir_offset |= v & 0xffff;
		} else {
			dir->tdir_offset = v & 0xffff;
			if (samplesperpixel == 2)
				dir->tdir_offset |= (long)v << 16;
		}
		return (1);
	}
	for (i = 0; i < samplesperpixel; i++)
		w[i] = v;
	return (TIFFWriteData(tif, dir, w));
}

static
TIFFWriteStripThing(tif, tag, dir, lp)
	TIFF *tif;
	u_short tag;
	TIFFDirEntry *dir;
	u_long *lp;
{

	dir->tdir_tag = tag;
	dir->tdir_type = (short)TIF_LONG;		/* XXX */
	dir->tdir_count = tif->tif_dir.td_nstrips;
	if (dir->tdir_count > 1)
		return (TIFFWriteData(tif, dir, lp));
	dir->tdir_offset = *lp;
	return (1);
}

/*
 * Write a contiguous directory item.
 */
static
TIFFWriteData(tif, dir, cp)
	TIFF *tif;
	TIFFDirEntry *dir;
	char *cp;
{
	int cc;

	dir->tdir_offset = dataoff;
                        switch(dir->tdir_type) {
                          case 0:
                          case 1:
                          case 2: cc = dir->tdir_count * 1;
				  break;
                          case 3: cc = dir->tdir_count * 2;
				  break;
                          case 4: cc = dir->tdir_count * 4;
				  break;
                          case 5: cc = dir->tdir_count * 8;
				  break;
                        }
	/*cc = dir->tdir_count * datawidth[dir->tdir_type];*/
	if (SeekOK(tif->tif_fd, dir->tdir_offset) &&
	    WriteOK(tif->tif_fd, cp, cc)) {
		dataoff += (cc + 1) & ~1;
		return (1);
	}
	TIFFError(tif->tif_name, "Error writing data for field \"%s\"",
	    FieldWithTag(dir->tdir_tag)->field_name);
	return (0);
}

/*
 * Link the current directory into the
 * directory chain for the file.
 */
static
TIFFLinkDirectory(tif)
	register TIFF *tif;
{
	static char module[] = "TIFFLinkDirectory";
	short dircount;
	long nextdir;

	tif->tif_diroff = (myseek(tif->tif_fd, 0L, L_XTND)+1) &~ 1L;
	if (tif->tif_header.tiff_diroff == 0) {
		/*
		 * First directory, overwrite header.
		 */
		tif->tif_header.tiff_diroff = tif->tif_diroff;
		(void) myseek(tif->tif_fd, 0L, L_SET);
		if (!WriteOK(tif->tif_fd, &tif->tif_header,
		    sizeof (tif->tif_header))) {
			TIFFError(tif->tif_name, "Error writing TIFF header");
			return (0);
		}
		return (1);
	}
	/*
	 * Not the first directory, search to the last and append.
	 */
	nextdir = tif->tif_header.tiff_diroff;
	do {
		if (!SeekOK(tif->tif_fd, nextdir) ||
		    !ReadOK(tif->tif_fd, &dircount, sizeof (dircount))) {
			TIFFError(module, "Error fetching directory count");
			return (0);
		}
		myseek(tif->tif_fd, dircount * sizeof (TIFFDirEntry), L_INCR);
		if (!ReadOK(tif->tif_fd, &nextdir, sizeof (nextdir))) {
			TIFFError(module, "Error fetching directory link");
			return (0);
		}
	} while (nextdir != 0);
	(void) myseek(tif->tif_fd, -sizeof (nextdir), L_INCR);
	if (!WriteOK(tif->tif_fd, &tif->tif_diroff, sizeof (tif->tif_diroff))) {
		TIFFError(module, "Error writing directory link");
		return (0);
	}
	return (1);
}

/*
 * Set the n-th directory as the current directory.
 * NB: Directories are numbered starting at 0.
 */
TIFFSetDirectory(tif, n)
	register TIFF *tif;
	int n;
{
	static char module[] = "TIFFSetDirectory";
	short dircount;
	long nextdir;

	nextdir = tif->tif_header.tiff_diroff;
	while (n-- > 0 && nextdir != 0) {
		if (!SeekOK(tif->tif_fd, nextdir) ||
		    !ReadOK(tif->tif_fd, &dircount, sizeof (dircount))) {
			TIFFError(module, "%s: Error fetching directory count",
			    tif->tif_name);
			return (0);
		}
		myseek(tif->tif_fd, dircount*sizeof (TIFFDirEntry), L_INCR);
		if (!ReadOK(tif->tif_fd, &nextdir, sizeof (nextdir))) {
			TIFFError(module, "%s: Error fetching directory link",
			    tif->tif_name);
			return (0);
		}
	}
	tif->tif_nextdiroff = nextdir;
	return (TIFFReadDirectory(tif));
}

#endif
