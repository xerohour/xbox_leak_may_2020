#if !defined(DSPCONST_H_INCLUDED_)
#define DSPCONST_H_INCLUDED_

/*============================================================================
Microsoft Access

Microsoft Confidential.
Copyright 1992 Microsoft Corporation. All Rights Reserved.

Component: misc

File: dspconst.h

Display constants shared by multiple components
(e.g. FEDT, DispSz, forms, grid)
============================================================================*/
#ifndef _DSPCONST_H
#define _DSPCONST_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************

		Alignment codes

*****************************************************************/

#define alnGeneral			0
#define alnLeft				1
#define alnCenter			2
#define alnRight			3
#define alnCenterIfFit		4	/* center if text fits, else left align */
#define alnMax				5
#define alnNil				((BYTE)-1)	/* NEVER stored!!! used only as a return
										value */
#define alnDef			alnGeneral

/*****************************************************************

		Color id codes

*****************************************************************/

#define crBlack 		RGB(0x00,0x00,0x00)
#define crGrey			RGB(0x80,0X80,0x80)
#define crWhite 		RGB(0xff,0xff,0xff)
#define crRed 			RGB(0xff,0x00,0x00)
#define crGreen 		RGB(0x00,0xff,0x00)
#define crBlue  		RGB(0x00,0x00,0xff)
#define crYellow 		RGB(0xff,0xff,0x00)
#define crMagenta 		RGB(0xff,0x00,0xff)
#define crCyan  		RGB(0x00,0xff,0xff)
#define crNil 			((COLORREF)-1)

#define crLtGray		RGB(192, 192, 192)
#define crDkGray		RGB(64, 64, 64)

#define crLtGrey		crLtGray
#define crDkGrey		crDkGray
#define crGray			crGrey

// Win95 colors
// tearoff palette colors.  Comments are from Excel, meaningless to us.
#define crPal01		0x000000L
#define crPal02		0xFFFFFFL
#define crPal03		0x0000FFL
#define crPal04		0x00FF00L
#define crPal05		0xFF0000L
#define crPal06		0x00FFFFL
#define crPal07		0xFF00FFL
#define crPal08		0xFFFF00L
#define crPal09		0x000080L
#define crPal10		0x008000L
#define crPal11		0x800000L
#define crPal12		0x008080L
#define crPal13		0x800080L
#define crPal14		0x808000L
#define crPal15		0xC0C0C0L
#define crPal16		0x808080L
#define crPal17		RGB(0x99,0x99,0xff) // 24 Fill 1
#define crPal18		RGB(0x99,0x33,0x66) // 25 Fill 2
#define crPal19		RGB(0xff,0xff,0xcc) // 26 Fill 3  // warning! change this or it's 
#define crPal20		RGB(0xa6,0xca,0xf0) // 27 Fill 4 // and you must change 
#define crPal21		RGB(0x66,0x00,0x99) // 28 Fill 5 // (colorful 2)
#define crPal22		RGB(0xff,0x7c,0x80) // 29 Fill 6
#define crPal23		RGB(0x00,0x99,0xcc) // 30 Fill 7
#define crPal24		RGB(0xcc,0xcc,0xff) // 31 Fill 8
#define crPal25		RGB(0x00,0x00,0x80) // 32 Line 1
#define crPal26		RGB(0xff,0x00,0xff) // 33 Line 2
#define crPal27		RGB(0xff,0xff,0x00) // 34 Line 3
#define crPal28		RGB(0x00,0xff,0xff) // 35 Line 4
#define crPal29		RGB(0x80,0x00,0x80) // 36 Line 5
#define crPal30		RGB(0x80,0x00,0x00) // 37 Line 6
#define crPal31		RGB(0x00,0x80,0x80) // 38 Line 7
#define crPal32		RGB(0x00,0x00,0xff) // 39 Line 8
#define crPal33		RGB(0x00,0xcc,0xff) // 40
#define crPal34		RGB(0x66,0xff,0xff) // 41
#define crPal35		RGB(0xcc,0xff,0xcc) // 42
#define crPal36		RGB(0xff,0xff,0x99) // 43
#define crPal37		RGB(0xa6,0xca,0xf0) // 44  //(Sys9)
#define crPal38		RGB(0xcc,0x99,0xcc) // 45
#define crPal39		RGB(0xcc,0x99,0xff) // 46
#define crPal40		RGB(0xe3,0xe3,0xe3) // 47
#define crPal41		RGB(0x33,0x66,0xff) // 48
#define crPal42		RGB(0x33,0xcc,0xcc) // 49
#define crPal43		RGB(0x33,0x99,0x33) // 40
#define crPal44		RGB(0x99,0x99,0x33) // 51
#define crPal45		RGB(0x99,0x66,0x33) // 52
#define crPal46		RGB(0x99,0x66,0x66) // 53
#define crPal47		RGB(0x66,0x66,0x99) // 54
#define crPal48		RGB(0x96,0x96,0x96) // 55
#define crPal49		RGB(0x33,0x33,0xcc) // 56
#define crPal50		RGB(0x33,0x66,0x66) // 57
#define crPal51		RGB(0x00,0x33,0x00) // 58
#define crPal52		RGB(0x33,0x33,0x00) // 59
#define crPal53		RGB(0x66,0x33,0x00) // 60
#define crPal54		RGB(0x99,0x33,0x66) // 61
#define crPal55		RGB(0x33,0x33,0x99) // 62
#define crPal56		RGB(0x42,0x42,0x42) // 63
	
/*****************************************************************

		Pattern id codes

*****************************************************************/

#define patNil		0
#define patSolid	1
#define patBitMin	1
#define patBitMax	32

/*****************************************************************

		Structures

*****************************************************************/

typedef struct TXTINFO
	{
	COLORREF crTxt;		/* text color */
	UINT ifm;			/* index to font of this item */
			/* (really type IFM, but don't want to include fontmast.h et al.) */
	BYTE aln;			/* alignment.  If general, always left-align. */
	} TXTINFO;

typedef struct _brshinfo
	{
	HBR hbr;			/* brush */
	COLORREF crBkg;		/* background color - to optimize for solid brushes */
	BOOLB fPatterned;	/* is brush patterned? */
	BOOLB fTransparent; /* is background supposed to be transparent? */
	} BRSHINFO;
	
typedef struct _bdrinfo
	{
	union		// CONSIDER: making these one once grid uses KT borders
		{
		DPOINT dpt;		/* border width; 0 for transparent */
		RECT	rcpMarg;	/* amount to deflate cell RECT before Paint */
		};
	BYTE appear;		/* 3D appearance (0 = normal) */
	BOOL fTransparent;	/* is border clear? */
	COLORREF crBkg;		/* background color */
	INT iLnStyle;	/* Line pattern - 0=solid, 1=dashed, etc */
	} BDRINFO;
	

/* Constants used to simulate double-clicks for the shared fedt; from _sdm.h */
#define	dxDoubleClick	4
#define	dyDoubleClick	2
#ifdef __cplusplus
}
#endif
#endif // _DSPCONST_H

#endif // !defined(DSPCONST_H_INCLUDED)
