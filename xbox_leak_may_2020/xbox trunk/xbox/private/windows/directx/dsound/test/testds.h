/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       testds.h
 *  Content:    Main header.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  06/07/01    dereks  Created.
 *
 ****************************************************************************/

#ifndef __TESTDS_H__
#define __TESTDS_H__

// #define STALL_ON_START
#define BREAK_ON_START
// #define ALLOC_ALIGNED
#define ALLOC_CONTIGUOUS
// #define ALLOC_NOCACHE
// #define ALLOC_GUARD_PAGE

#define BEGIN_TEST(name) \
    if(SUCCEEDED(hr)) DPF_TEST("*** Starting " #name " ***"), hr = name

#include <dsoundi.h>
#include <dsstdfx.h>
#include <wavbndli.h>
#include "util.h"

static void __cdecl DPF_TEST(LPCSTR pszFormat, ...)
{
    CHAR                    szString[0x100];
    va_list                 va;

    va_start(va, pszFormat);
    vsprintf(szString, pszFormat, va);
    va_end(va);

    OutputDebugStringA("TESTDS: ");
    OutputDebugStringA(szString);
    OutputDebugStringA("\n");
}

//
// Global variables
//

EXTERN_C BOOL fTestMe;
EXTERN_C BOOL fBreakTest;

//
// Main DirectSound test function
//

EXTERN_C HRESULT TestDirectSound(void);

//
// DirectSoundCreate stress test
//

EXTERN_C HRESULT TestDirectSoundCreate(void);

//
// AC97 XMO tests
//

EXTERN_C HRESULT TestAc97Xmo(DWORD dwChannel, DWORD dwMode);

//
// Memory usage tests
//

EXTERN_C HRESULT TestMemoryUsage(void);

#endif // __TESTDS_H__
