/*************************************************************************

Copyright (C) 1996 -- 1998  Microsoft Corporation

Module Name:

	repeatpad_wmv.h

Abstract:


Author:


Revision History:

*************************************************************************/

/////////// main external API's /////////////////

#define RepeatRef0YArgs 	PixelC* ppxlcRef0Y,     \
                        	CoordI  iStart,         \
                            CoordI  iEnd,           \
                        	I32_WMV     iOldLeftOffet,  \
                        	Bool_WMV    fTop,           \
                            Bool_WMV    fBottom,        \
                            I32_WMV     iWidthY,        \
                            I32_WMV     iWidthYPlusExp, \
                            I32_WMV     iWidthPrevY


#define RepeatRef0UVArgs	PixelC* ppxlcRef0U,     \
                        	PixelC* ppxlcRef0V,     \
   	                        CoordI  iStart,         \
                            CoordI  iEnd,           \
	                        I32_WMV     iOldLeftOffet,  \
	                        Bool_WMV    fTop,           \
                            Bool_WMV    fBottom,        \
                            I32_WMV     iWidthUV,       \
                            I32_WMV     iWidthUVPlusExp,\
                            I32_WMV     iWidthPrevUV


extern Void_WMV (*g_pRepeatRef0UV) (RepeatRef0UVArgs);
extern Void_WMV (*g_pRepeatRef0Y) (RepeatRef0YArgs);

///////////////////////////////////////////////////////

#ifndef OPT_REPEATPAD_ARM
Void_WMV g_RepeatRef0Y     (RepeatRef0YArgs);
Void_WMV g_RepeatRef0UV     (RepeatRef0UVArgs);
#else
extern "C" Void_WMV g_RepeatRef0Y     (RepeatRef0YArgs);
extern "C" Void_WMV g_RepeatRef0UV     (RepeatRef0UVArgs);
#endif
Void_WMV g_RepeatRef0Y_MMX (RepeatRef0YArgs);
Void_WMV g_RepeatRef0UV_MMX (RepeatRef0UVArgs);
