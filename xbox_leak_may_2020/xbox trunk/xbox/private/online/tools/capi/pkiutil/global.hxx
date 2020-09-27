//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1996 - 1996
//
//  File:       global.hxx
//
//  Contents:   Top level internal header file for PKI util APIs. This file
//              includes all base header files and contains other global
//              stuff.
//
//  History:    27-Nov-96   kevinr   created
//
//--------------------------------------------------------------------------
#define CMS_PKCS7   1
#define LINK_REDIR 1

#include <windows.h>
typedef ULONG REGSAM;
#define assert(exp)     ((void)0)
//#include <assert.h>
#include <malloc.h>
//#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <time.h>
#include <stddef.h>

#include <dbgdef.h>
#include <crtem.h>
#include <wincrypt.h>
#include <asn1util.h>
#include <list.hxx>
#include <pkialloc.h>
#include <utf8.h>
#include <pkiasn1.h>
#include <unicode.h>

#pragma hdrstop
