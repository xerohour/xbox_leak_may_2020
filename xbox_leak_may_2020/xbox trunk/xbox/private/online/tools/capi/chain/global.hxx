//+---------------------------------------------------------------------------
//
//  Microsoft Windows NT Security
//  Copyright (C) Microsoft Corporation, 1997 - 1998
//
//  File:       global.hxx
//
//  Contents:   Global Pre-compiled Header
//
//  History:    12-26-1997    kirtd    Created
//
//----------------------------------------------------------------------------
#if !defined(__GLOBAL_HXX__)
#define __GLOBAL_HXX__

//#define CMS_PKCS7       1

#include <windows.h>
typedef ULONG REGSAM;
#define assert(exp)     ((void)0)
#include <stdio.h>
#include <stdlib.h>
//#include <assert.h>
#include <malloc.h>
#include <wincrypt.h>
#include <lru.h>
#include <chain.h>
#include <ssctl.h>
#include <callctx.h>
#include <defce.h>
//#include <winwlx.h>
#include <unicode.h>

#include <crtem.h>

#endif
