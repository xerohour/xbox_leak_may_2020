/*-----------------------------------------------------------------------------
Microsoft VSEE

Microsoft Confidential
Copyright 1995-2000 Microsoft Corporation. All Rights Reserved.

@doc external
@module VseeGuidDataToXxx.h

Macros to convert "guid data", of the
form (3f32766f, 2d94, 444d, bf,32,2f,32,9c,71,d4,08), to all of the various
needed forms:
  3f32766f-2d94-444d-bf32-2f329c71d408                               VSEE_GUID_DATA_TO_DASHED
 '3f32766f-2d94-444d-bf32-2f329c71d408'                              VSEE_GUID_DATA_TO_DASHED_CHAR
 "3f32766f-2d94-444d-bf32-2f329c71d408"                              VSEE_GUID_DATA_TO_DASHED_STRING
 {3f32766f-2d94-444d-bf32-2f329c71d408}                              VSEE_GUID_DATA_TO_BRACED_DASHED
'{3f32766f-2d94-444d-bf32-2f329c71d408}'                             VSEE_GUID_DATA_TO_BRACED_DASHED_CHAR
"{3f32766f-2d94-444d-bf32-2f329c71d408}"                             VSEE_GUID_DATA_TO_BRACED_DASHED_STRING
{0x3f32766f,0x2d94,0x444d,{0xbf,0x32,0x2f,0x32,0x9c,0x71,0xd4,0x08}} VSEE_GUID_DATA_TO_STRUCT_INITIALIZER

@owner JayK
-----------------------------------------------------------------------------*/
#if !defined(VS_COMMON_INC_VSEE_GUID_DATA_TO_XXX_H_INCLUDED_)
#define VS_COMMON_INC_VSEE_GUID_DATA_TO_XXX_H_INCLUDED_
/*#pragma once ends up in .rgi, which is bad, so do not do it*/

#include "VseePreprocessorPaste.h"
#include "VseePreprocessorStringize.h"
#include "VseePreprocessorCharize.h"

/*-----------------------------------------------------------------------------
I need to investigate more, but present usage is:

VSEE_GUID_DATA_TO_DASHED_STRING : __declpec(uuid())
VSEE_GUID_DATA_TO_BRACED_DASHED_STRING : also works with__declpec(uuid())
VSEE_GUID_DATA_TO_BRACED_DASHED : in .rgs files (unquoted)
VSEE_GUID_DATA_TO_STRUCT_INITIALIZER : would be used in .ctc files
VSEE_GUID_DATA_TO_BRACED_DASHED_CHAR : used in .rgs files (quoted)

-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
Name: VSEE_GUID_DATA_TO_STRUCT_INITIALIZER
@macro
This macro does like:
VSEE_GUID_DATA_TO_STRUCT_INITIALIZER(80f3e6ba, d9b2, 4c41, ae,90,63,93,da,ce,ac,2a)
 -> { 0x80f3e6ba, 0xd9b2, 0x4c41, { 0xae, 0x90, 0x63, 0x93, 0xda, 0xce, 0xac, 0x2a } }

The parameters are hex constants without 0x on them.
They must be exactly 8, 4, and 2 digits wide.
They must include leading zeros.

@owner JayK
-----------------------------------------------------------------------------*/
#define \
VSEE_GUID_DATA_TO_STRUCT_INITIALIZER\
( \
	dwData1,  \
	 wData2,  \
	 wData3,  \
	bData4_0, \
	bData4_1, \
	bData4_2, \
	bData4_3, \
	bData4_4, \
	bData4_5, \
	bData4_6, \
	bData4_7  \
) \
{ \
	VseePreprocessorPaste2(0x, dwData1), \
	VseePreprocessorPaste2(0x,  wData2), \
	VseePreprocessorPaste2(0x,  wData3), \
	{ \
		VseePreprocessorPaste2(0x, bData4_0), \
		VseePreprocessorPaste2(0x, bData4_1), \
		VseePreprocessorPaste2(0x, bData4_2), \
		VseePreprocessorPaste2(0x, bData4_3), \
		VseePreprocessorPaste2(0x, bData4_4), \
		VseePreprocessorPaste2(0x, bData4_5), \
		VseePreprocessorPaste2(0x, bData4_6), \
		VseePreprocessorPaste2(0x, bData4_7)  \
	} \
}

/*-----------------------------------------------------------------------------
Name: VSEE_GUID_DATA_TO_DASHED
@macro
This macro does like:
VSEE_GUID_DATA_TO_DASHED(80f3e6ba, d9b2, 4c41, ae,90,63,93,da,ce,ac,2a)
 -> 80f3e6ba-d9b2-4c41-ae90-6393daceac2a

The parameters are hex constants without 0x on them.
They must be exactly 8, 4, and 2 digits wide.
They must include leading zeros.

@owner JayK
-----------------------------------------------------------------------------*/
#define \
VSEE_GUID_DATA_TO_DASHED\
( \
	dwData1,  \
	 wData2,  \
	 wData3,  \
	bData4_0, \
	bData4_1, \
	bData4_2, \
	bData4_3, \
	bData4_4, \
	bData4_5, \
	bData4_6, \
	bData4_7  \
) \
VseePreprocessorPaste15(dwData1,-,wData2,-,wData3,-,bData4_0,bData4_1,-,bData4_2,bData4_3,bData4_4,bData4_5,bData4_6,bData4_7)

// without braces

/*-----------------------------------------------------------------------------
Name: VSEE_GUID_DATA_TO_DASHED_STRING
@macro
VSEE_GUID_DATA_TO_DASHED_STRING(80f3e6ba, d9b2, 4c41, ae,90,63,93,da,ce,ac,2a)
 -> "80f3e6ba-d9b2-4c41-ae90-6393daceac2a"

The parameters are hex constants without 0x on them.
They must be exactly 8, 4, and 2 digits wide.
They must include leading zeros.

@owner JayK
-----------------------------------------------------------------------------*/
#define \
VSEE_GUID_DATA_TO_DASHED_STRING\
( \
	dw1,  \
	 w2,  \
	 w3,  \
	 b0, \
	 b1, \
	 b2, \
	 b3, \
	 b4, \
	 b5, \
	 b6, \
	 b7  \
) \
	VseePreprocessorStringize(VSEE_GUID_DATA_TO_DASHED(dw1,w2,w3,b0,b1,b2,b3,b4,b5,b6,b7))

/*-----------------------------------------------------------------------------
Name: VSEE_GUID_DATA_TO_DASHED_CHAR
@macro
VSEE_GUID_DATA_TO_DASHED_CHAR(80f3e6ba, d9b2, 4c41, ae,90,63,93,da,ce,ac,2a)
 -> '80f3e6ba-d9b2-4c41-ae90-6393daceac2a'

The parameters are hex constants without 0x on them.
They must be exactly 8, 4, and 2 digits wide.
They must include leading zeros.

@owner JayK
-----------------------------------------------------------------------------*/
#define \
VSEE_GUID_DATA_TO_DASHED_CHAR\
( \
	dw1,  \
	 w2,  \
	 w3,  \
	 b0, \
	 b1, \
	 b2, \
	 b3, \
	 b4, \
	 b5, \
	 b6, \
	 b7  \
) \
	VseePreprocessorCharize(VSEE_GUID_DATA_TO_DASHED(dw1,w2,w3,b0,b1,b2,b3,b4,b5,b6,b7))

// with braces

/*-----------------------------------------------------------------------------
Name: VSEE_GUID_DATA_TO_BRACED_DASHED
@macro
VSEE_GUID_DATA_TO_BRACED_DASHED(80f3e6ba, d9b2, 4c41, ae,90,63,93,da,ce,ac,2a)
 -> {80f3e6ba-d9b2-4c41-ae90-6393daceac2a}

The parameters are hex constants without 0x on them.
They must be exactly 8, 4, and 2 digits wide.
They must include leading zeros.

@owner JayK
-----------------------------------------------------------------------------*/
#define \
VSEE_GUID_DATA_TO_BRACED_DASHED\
( \
	dw1, \
	 w2, \
	 w3, \
	 b0, \
	 b1, \
	 b2, \
	 b3, \
	 b4, \
	 b5, \
	 b6, \
	 b7  \
) \
	VseePreprocessorPaste3({,VSEE_GUID_DATA_TO_DASHED(dw1,w2,w3,b0,b1,b2,b3,b4,b5,b6,b7),})

/*-----------------------------------------------------------------------------
Name: VSEE_GUID_DATA_TO_BRACED_DASHED_STRING
@macro
VSEE_GUID_DATA_TO_BRACED_DASHED_STRING(80f3e6ba, d9b2, 4c41, ae,90,63,93,da,ce,ac,2a)
 -> "{80f3e6ba-d9b2-4c41-ae90-6393daceac2a}"

The parameters are hex constants without 0x on them.
They must be exactly 8, 4, and 2 digits wide.
They must include leading zeros.

@owner JayK
-----------------------------------------------------------------------------*/
#define \
VSEE_GUID_DATA_TO_BRACED_DASHED_STRING\
( \
	dw1, \
	 w2, \
	 w3, \
	 b0, \
	 b1, \
	 b2, \
	 b3, \
	 b4, \
	 b5, \
	 b6, \
	 b7  \
) \
	VseePreprocessorStringize(VSEE_GUID_DATA_TO_BRACED_DASHED(dw1,w2,w3,b0,b1,b2,b3,b4,b5,b6,b7))

/*-----------------------------------------------------------------------------
Name: VSEE_GUID_DATA_TO_BRACED_DASHED_STRING_W
@macro
VSEE_GUID_DATA_TO_BRACED_DASHED_STRING_W(80f3e6ba, d9b2, 4c41, ae,90,63,93,da,ce,ac,2a)
 -> L"{80f3e6ba-d9b2-4c41-ae90-6393daceac2a}"

The parameters are hex constants without 0x on them.
They must be exactly 8, 4, and 2 digits wide.
They must include leading zeros.

@owner JayK
-----------------------------------------------------------------------------*/
#define VSEE_GUID_DATA_TO_BRACED_DASHED_STRING_W(dw1, w2, w3, b0, b1, b2, b3, b4, b5, b6, b7) \
	VseePreprocessorPaste(L, VSEE_GUID_DATA_TO_BRACED_DASHED_STRING(dw1, w2, w3, b0, b1, b2, b3, b4, b5, b6, b7))

/*-----------------------------------------------------------------------------
Name: VSEE_GUID_DATA_TO_BRACED_DASHED_CHAR
@macro
VSEE_GUID_DATA_TO_BRACED_DASHED_CHAR(80f3e6ba, d9b2, 4c41, ae,90,63,93,da,ce,ac,2a)
 -> '{80f3e6ba-d9b2-4c41-ae90-6393daceac2a}'

The parameters are hex constants without 0x on them.
They must be exactly 8, 4, and 2 digits wide.
They must include leading zeros.

@owner JayK
-----------------------------------------------------------------------------*/
#define \
VSEE_GUID_DATA_TO_BRACED_DASHED_CHAR\
( \
	dw1, \
	 w2, \
	 w3, \
	 b0, \
	 b1, \
	 b2, \
	 b3, \
	 b4, \
	 b5, \
	 b6, \
	 b7  \
) \
	VseePreprocessorCharize(VSEE_GUID_DATA_TO_BRACED_DASHED(dw1,w2,w3,b0,b1,b2,b3,b4,b5,b6,b7))

#endif
