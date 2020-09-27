///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) Microsoft Corporation.  All Rights Reserved.
//
//  File:       d3dx8seg.h
//  Content:    sets the sections for code and data in d3dx8.lib
//
///////////////////////////////////////////////////////////////////////////

#ifndef __D3DX8SEG_H__
#define __D3DX8SEG_H__

#include "xboxverp.h"

#pragma code_seg("D3DX")
#pragma data_seg("D3DX_RW")
#pragma const_seg("D3DX_RD")

// Tell the linker to merge constant data and data sections into code section.
#pragma comment(linker, "/merge:D3DX_RD=D3DX")
#pragma comment(linker, "/merge:D3DX_RW=D3DX")

// Tell the linker that the code section contains read/write data.
#pragma comment(linker, "/section:D3DX,ERW")

#ifdef __cplusplus
extern "C"
{
#endif

#pragma data_seg(".XBLD$V")

#if DBG
#pragma comment(linker, "/include:_D3DX8BuildNumberD")
__declspec(selectany) unsigned short D3DX8BuildNumberD[8] =
        { 'D' | ('3' << 8), 'D' | ('X' << 8), '8' | ('D' << 8), 0,
          VER_PRODUCTVERSION | 0x8000 };
#else
#pragma comment(linker, "/include:_D3DX8BuildNumber")
__declspec(selectany) unsigned short D3DX8BuildNumber[8] =
        { 'D' | ('3' << 8), 'D' | ('X' << 8), '8', 0,
          VER_PRODUCTVERSION | 0x4000 };
#endif

#pragma data_seg()
#ifdef __cplusplus
}
#endif

#endif //__D3DX8SEG_H__
