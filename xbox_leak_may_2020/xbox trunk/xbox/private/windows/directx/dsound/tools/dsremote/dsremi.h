/***************************************************************************
 *
 *  Copyright (C) 01/08/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsremi.h
 *  Content:    DirectSound remote API internal header.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  01/08/2002  dereks  Created.
 *
 ****************************************************************************/

#ifndef __DSREMI_H__
#define __DSREMI_H__

#ifndef _XBOX
#define USEDPF
#endif // _XBOX

#include "dscommon.h"
#ifndef _XBOX
#include <winsock2.h>
#endif // _XBOX
#include "xboxdbg.h"
#include "xboxverp.h"
#include "dsremote.h"
#include "ifileio.h"
#include "iremote.h"

#ifdef __cplusplus

static void GetLibraryVersion(LPSTR pszString)
{
    static const DWORD      dwMajor = (VER_PRODUCTVERSION_DW >> 24) & 0xFF;
    static const DWORD      dwMinor = (VER_PRODUCTVERSION_DW >> 16) & 0xFF;
    static const DWORD      dwBuild = VER_PRODUCTVERSION_DW & 0xFFFF;
    
    sprintf(pszString, "%lu.%lu.%lu", dwMajor, dwMinor, dwBuild);
}

#endif // __cplusplus

#endif // __DSREMI_H__
