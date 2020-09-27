/***************************************************************************
 *
 *  Copyright (C) 11/2/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       wavbndli.h
 *  Content:    Wave Bundler common library definitions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/2/2001   dereks  Created.
 *
 ****************************************************************************/

#ifndef __WAVBNDLI_H__
#define __WAVBNDLI_H__

#ifndef _XBOX
#define USEDPF
#define DPFLVL_DEFAULT DPFLVL_INFO
#endif // _XBOX

#include "..\..\..\common\dscommon.h"
#include "xboxverp.h"
#include "wavbndlr.h"

#ifndef _XBOX

#ifndef _CONSOLE

EXTERN_C HWND g_hwndMain;
EXTERN_C LPCSTR g_pszAppTitle;

#endif // _CONSOLE

#include "imaadpcm.h"
#include "filter.h"
#include "writer.h"

#endif // _XBOX

static void GetWaveBundlerVersion(LPSTR pszString)
{
    static const DWORD      dwMajor = (VER_PRODUCTVERSION_DW >> 24) & 0xFF;
    static const DWORD      dwMinor = (VER_PRODUCTVERSION_DW >> 16) & 0xFF;
    static const DWORD      dwBuild = VER_PRODUCTVERSION_DW & 0xFFFF;
    
    sprintf(pszString, "%lu.%lu.%lu", dwMajor, dwMinor, dwBuild);
}

#endif // __WAVBNDLI_H__
