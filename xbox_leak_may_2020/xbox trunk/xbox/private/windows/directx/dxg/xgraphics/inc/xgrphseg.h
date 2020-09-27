///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) Microsoft Corporation.  All Rights Reserved.
//
//  File:       xgrphseg.h
//  Content:    sets the sections for code and data in d3dx8.lib
//
///////////////////////////////////////////////////////////////////////////

#ifndef __XGRPHSEG_H__
#define __XGRPHSEG_H__

#include "xboxverp.h"

#pragma code_seg("XGRPH")
#pragma data_seg("XGRPH_RW")
#pragma bss_seg("XGRPH_RW")
#pragma const_seg("XGRPH_RD")

// Tell the linker to merge constant data and data sections into code section.
#pragma comment(linker, "/merge:XGRPH_RD=XGRPH")
#pragma comment(linker, "/merge:XGRPH_RW=XGRPH")

// Tell the linker that the code section contains read/write data.
#pragma comment(linker, "/section:XGRPH,ERW")

#ifdef __cplusplus
extern "C"
{
#endif

#pragma data_seg(".XBLD$V")

#if DBG
#pragma comment(linker, "/include:_XGraphicsBuildNumberD")
__declspec(selectany) unsigned short XGraphicsBuildNumberD[8] =
        { 'X' | ('G' << 8), 'R' | ('A' << 8), 'P' | ('H' << 8), 'C' | ('D' << 8),
          VER_PRODUCTVERSION | 0x8000 };
#else
#pragma comment(linker, "/include:_XGraphicsBuildNumber")
__declspec(selectany) unsigned short XGraphicsBuildNumber[8] =
        { 'X' | ('G' << 8), 'R' | ('A' << 8), 'P' | ('H' << 8), 'C',
          VER_PRODUCTVERSION | 0x4000 };
#endif

#pragma data_seg()
#ifdef __cplusplus
}
#endif

#endif //__XGRPHSEG_H__
