#include "tif_port.h"

#ifndef lint
static char sccsid[] = "@(#)print.c	1.2	(Pixar - RenderMan Division)	5/1/89";
/* static char sccsid[] = "@(#)tiff_print.c	1.9 4/25/89"; */
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
 * Directory Printing Support
 */
#include "tiffio.h"

static char *ResponseUnitNames[] = {
	"#0",
	"10ths",
	"100ths",
	"1,000ths",
	"10,000ths",
	"100,000ths",
};
static	float ResponseUnit[] = { 1.0f, 0.1f, 0.01f, 0.001f, 0.0001f, 0.00001f };
#define	MAXRESPONSEUNIT \
    (sizeof (ResponseUnitNames) / sizeof (ResponseUnitNames[0]))

/*
 * Print the contents of the current directory
 * to the specified stdio file stream.
 */
void
TIFFPrintDirectory(tif, fd, showstrips, showresponsecurve, showcolormap)
	TIFF *tif;
	FILE *fd;
{
	register TIFFDirectory *td;
	int i;
	long n;
	float unit;

	fprintf(fd, "TIFF Directory at offset 0x%x\n", tif->tif_diroff);
	td = &tif->tif_dir;
	if (tif->tif_flags &TIFF_SWAB) 
		fprintf(fd, "  Byte order: Big Endian (Motorola) \n");
	else 
		fprintf(fd, "  Byte order: Little Endian (Intel) \n");
	if (TIFFFieldSet(tif,FIELD_SUBFILETYPE)) {
		fprintf(fd, "  Subfile Type: ");
		if (td->td_subfiletype & FILETYPE_REDUCEDIMAGE)
			fprintf(fd, "reduced-resolution image");
		if (td->td_subfiletype & FILETYPE_PAGE)
			fprintf(fd, "multi-page document");
		if (td->td_subfiletype & FILETYPE_MASK)
			fprintf(fd, "transparency mask");
		fprintf(fd, " (%d = 0x%x)\n",
		    td->td_subfiletype, td->td_subfiletype);
	}
	if (TIFFFieldSet(tif,FIELD_IMAGEDIMENSIONS))
		fprintf(fd, "  Image Width: %d Image Length: %d\n",
		    td->td_imagewidth, td->td_imagelength);
	if (TIFFFieldSet(tif,FIELD_RESOLUTION))
		fprintf(fd, "  Resolution: %g, %g\n",
		    td->td_xresolution, td->td_yresolution);
	if (TIFFFieldSet(tif,FIELD_POSITION))
		fprintf(fd, "  Position: %g, %g\n",
		    td->td_xposition, td->td_yposition);
	if (TIFFFieldSet(tif,FIELD_BITSPERSAMPLE))
		fprintf(fd, "  Bits/Sample: %d\n", td->td_bitspersample);
	if (TIFFFieldSet(tif,FIELD_COMPRESSION)) {
		fprintf(fd, "  Compression Scheme: ");
		switch (td->td_compression) {
		case COMPRESSION_NONE:
			fprintf(fd, "none\n");
			break;
		case COMPRESSION_CCITTRLE:
			fprintf(fd, "CCITT modified Huffman encoding\n");
			break;
		case COMPRESSION_CCITTFAX3:
			fprintf(fd, "CCITT Group 3 facsimile encoding\n");
			break;
		case COMPRESSION_CCITTFAX4:
			fprintf(fd, "CCITT Group 4 facsimile encoding\n");
			break;
		case COMPRESSION_CCITTRLEW:
			fprintf(fd, "CCITT modified Huffman encoding %s\n",
			    "w/ word alignment");
			break;
		case COMPRESSION_PACKBITS:
			fprintf(fd, "Macintosh PackBits encoding\n");
			break;
		case COMPRESSION_THUNDERSCAN:
			fprintf(fd, "ThunderScan 4-bit encoding\n");
			break;
		case COMPRESSION_LZW:
			fprintf(fd, "Lempel-Ziv & Welch encoding\n");
			break;
		case COMPRESSION_PICIO:
			fprintf(fd, "Pixar picio encoding\n");
			break;
		case COMPRESSION_NEXT:
			fprintf(fd, "NeXT 2-bit encoding\n");
			break;
		default:
			fprintf(fd, "%d (0x%x)\n",
			    td->td_compression, td->td_compression);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_PHOTOMETRIC)) {
		fprintf(fd, "  Photometric Interpretation: ");
		switch (td->td_photometric) {
		case PHOTOMETRIC_MINISWHITE:
			fprintf(fd, "\"min-is-white\"\n");
			break;
		case PHOTOMETRIC_MINISBLACK:
			fprintf(fd, "\"min-is-black\"\n");
			break;
		case PHOTOMETRIC_RGB:
			fprintf(fd, "RGB color\n");
			break;
		case PHOTOMETRIC_PALETTE:
			fprintf(fd, "palette color (RGB from colormap)\n");
			break;
		case PHOTOMETRIC_MASK:
			fprintf(fd, "transparency mask\n");
			break;
		default:
			fprintf(fd, "%d (0x%x)\n",
			    td->td_photometric, td->td_photometric);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_MATTEING))
		fprintf(fd, "  Matteing: %s\n",
		    td->td_matteing ? "alpha channel present" : "none");
	if (TIFFFieldSet(tif,FIELD_THRESHHOLDING)) {
		fprintf(fd, "  Thresholding: ");
		switch (td->td_threshholding) {
		case THRESHHOLD_BILEVEL:
			fprintf(fd, "bilevel art scan\n");
			break;
		case THRESHHOLD_HALFTONE:
			fprintf(fd, "halftone or dithered scan\n");
			break;
		case THRESHHOLD_ERRORDIFFUSE:
			fprintf(fd, "error diffused\n");
			break;
		default:
			fprintf(fd, "%d (0x%x)\n",
			    td->td_threshholding, td->td_threshholding);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_PREDICTOR))
		fprintf(fd, "  Predictor: %d\n", td->td_predictor);
	if (TIFFFieldSet(tif,FIELD_ARTIST))
		fprintf(fd, "  Artist: %s\n", td->td_artist);
	if (TIFFFieldSet(tif,FIELD_DATETIME))
		fprintf(fd, "  Date & Time: %s\n", td->td_datetime);
	if (TIFFFieldSet(tif,FIELD_HOSTCOMPUTER))
		fprintf(fd, "  Host Computer: %s\n", td->td_hostcomputer);
	if (TIFFFieldSet(tif,FIELD_SOFTWARE))
		fprintf(fd, "  Software: %s\n", td->td_software);
	if (TIFFFieldSet(tif,FIELD_DOCUMENTNAME))
		fprintf(fd, "  Document Name: %s\n", td->td_documentname);
	if (TIFFFieldSet(tif,FIELD_IMAGEDESCRIPTION))
		fprintf(fd, "  Image Description: %s\n",
		    td->td_imagedescription);
	if (TIFFFieldSet(tif,FIELD_MAKE))
		fprintf(fd, "  Make: %s\n", td->td_make);
	if (TIFFFieldSet(tif,FIELD_MODEL))
		fprintf(fd, "  Model: %s\n", td->td_model);
	if (TIFFFieldSet(tif,FIELD_ORIENTATION)) {
		fprintf(fd, "  Orientation: ");
		switch (td->td_orientation) {
		case ORIENTATION_TOPLEFT:
			fprintf(fd, "row 0 top, col 0 lhs\n");
			break;
		case ORIENTATION_TOPRIGHT:
			fprintf(fd, "row 0 top, col 0 rhs\n");
			break;
		case ORIENTATION_BOTRIGHT:
			fprintf(fd, "row 0 bottom, col 0 rhs\n");
			break;
		case ORIENTATION_BOTLEFT:
			fprintf(fd, "row 0 bottom, col 0 lhs\n");
			break;
		case ORIENTATION_LEFTTOP:
			fprintf(fd, "row 0 lhs, col 0 top\n");
			break;
		case ORIENTATION_RIGHTTOP:
			fprintf(fd, "row 0 rhs, col 0 top\n");
			break;
		case ORIENTATION_RIGHTBOT:
			fprintf(fd, "row 0 rhs, col 0 bottom\n");
			break;
		case ORIENTATION_LEFTBOT:
			fprintf(fd, "row 0 lhs, col 0 bottom\n");
			break;
		default:
			fprintf(fd, "%d (0x%x)\n",
			    td->td_orientation, td->td_orientation);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_SAMPLESPERPIXEL))
		fprintf(fd, "  Samples/Pixel: %d\n", td->td_samplesperpixel);
	if (TIFFFieldSet(tif,FIELD_ROWSPERSTRIP))
		fprintf(fd, "  Rows/Strip: %d\n", td->td_rowsperstrip);
	if (TIFFFieldSet(tif,FIELD_MINSAMPLEVALUE))
		fprintf(fd, "  Min Sample Value: %d\n", td->td_minsamplevalue);
	if (TIFFFieldSet(tif,FIELD_MAXSAMPLEVALUE))
		fprintf(fd, "  Max Sample Value: %d\n", td->td_maxsamplevalue);
	if (TIFFFieldSet(tif,FIELD_PLANARCONFIG)) {
		fprintf(fd, "  Planar Configuration: ");
		switch (td->td_planarconfig) {
		case PLANARCONFIG_CONTIG:
			fprintf(fd, "single image plane\n");
			break;
		case PLANARCONFIG_SEPARATE:
			fprintf(fd, "separate image planes\n");
			break;
		default:
			fprintf(fd, "%d (0x%x)\n",
			    td->td_planarconfig, td->td_planarconfig);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_PAGENAME))
		fprintf(fd, "  Page Name: %s\n", td->td_pagename);
	if (TIFFFieldSet(tif,FIELD_GRAYRESPONSEUNIT)) {
		fprintf(fd, "  Gray Response Unit: ");
		if (td->td_grayresponseunit < MAXRESPONSEUNIT)
			fprintf(fd, "%s\n",
			    ResponseUnitNames[td->td_grayresponseunit]);
		else
			fprintf(fd, "%d (0x%x)\n",
			    td->td_grayresponseunit, td->td_grayresponseunit);
	}
	if (TIFFFieldSet(tif,FIELD_GRAYRESPONSECURVE)) {
		fprintf(fd, "  Gray Response Curve: ");
		if (showresponsecurve) {
			fprintf(fd, "\n");
			unit = ResponseUnit[td->td_grayresponseunit];
			n = 1L<<td->td_bitspersample;
			for (i = 0; i < n; i++)
				fprintf(fd, "    %2d: %g (%d)\n",
				    i,
				    td->td_grayresponsecurve[i] * unit,
				    td->td_grayresponsecurve[i]);
		} else
			fprintf(fd, "(present)\n");
	}
	if (TIFFFieldSet(tif,FIELD_GROUP3OPTIONS))
		fprintf(fd, "  Group 3 Options: 0x%x\n", td->td_group3options);
	if (TIFFFieldSet(tif,FIELD_GROUP4OPTIONS))
		fprintf(fd, "  Group 4 Options: 0x%x\n", td->td_group4options);
	if (TIFFFieldSet(tif,FIELD_RESOLUTIONUNIT)) {
		fprintf(fd, "  Resolution Unit: ");
		switch (td->td_resolutionunit) {
		case RESUNIT_NONE:
			fprintf(fd, "no meaningful units\n");
			break;
		case RESUNIT_INCH:
			fprintf(fd, "inches\n");
			break;
		case RESUNIT_CENTIMETER:
			fprintf(fd, "centimeters\n");
			break;
		default:
			fprintf(fd, "%d (0x%x)\n",
			    td->td_resolutionunit, td->td_resolutionunit);
			break;
		}
	}
	if (TIFFFieldSet(tif,FIELD_PAGENUMBER))
		fprintf(fd, "  Page Number: %d\n", td->td_pagenumber);
	if (TIFFFieldSet(tif,FIELD_COLORRESPONSEUNIT)) {
		fprintf(fd, "  Color Response Unit: ");
		if (td->td_colorresponseunit < MAXRESPONSEUNIT)
			fprintf(fd, "%s\n",
			    ResponseUnitNames[td->td_colorresponseunit]);
		else
			fprintf(fd, "%d (0x%x)\n",
			    td->td_colorresponseunit, td->td_colorresponseunit);
	}
	if (TIFFFieldSet(tif,FIELD_COLORMAP)) {
		fprintf(fd, "  Color Map: ");
		if (showcolormap) {
			fprintf(fd, "\n");
			n = 1L<<td->td_bitspersample;
			for (i = 0; i < n; i++)
				fprintf(fd, "    %2d: %4d %4d %4d\n",
				    i,
				    td->td_redcolormap[i],
				    td->td_greencolormap[i],
				    td->td_bluecolormap[i]);
		} else
			fprintf(fd, "(present)\n");
	}
	if (TIFFFieldSet(tif,FIELD_COLORRESPONSECURVE)) {
		fprintf(fd, "  Color Response Curve: ");
		if (showresponsecurve) {
			fprintf(fd, "\n");
			unit = ResponseUnit[td->td_colorresponseunit];
			n = 1L<<td->td_bitspersample;
			for (i = 0; i < n; i++)
				fprintf(fd, "    %2d: %6.4f %6.4f %6.4f\n",
				    i,
				    td->td_redresponsecurve[i] * unit,
				    td->td_greenresponsecurve[i] * unit,
				    td->td_blueresponsecurve[i] * unit);
		} else
			fprintf(fd, "(present)\n");
	}
	if (showstrips && TIFFFieldSet(tif,FIELD_STRIPOFFSETS)) {
    	unsigned int i;    // KINETIX: added shadow, so we can have an unsigned counter
		fprintf(fd, "  %d Strips:\n", td->td_nstrips);
		for (i = 0; i < td->td_nstrips; i++)
			fprintf(fd, "    %3d: [%8d, %8d]\n",
			    i, td->td_stripoffset[i], td->td_stripbytecount[i]);
	}
}
