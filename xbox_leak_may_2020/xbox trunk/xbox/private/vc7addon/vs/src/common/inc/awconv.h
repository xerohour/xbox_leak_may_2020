// awconv.h - ANSI/Unicode conversion
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
// June 1, 1998 [paulde]
//
//---------------------------------------------------------------
// cb  == count of bytes
// cch == count of characters

#pragma once
#ifndef __AWCONV_H__
#define __AWCONV_H__
#include "unistr.h"

// To avoid conflicts between malloc.h and vsmem.h, declare _alloca here
//#include "malloc.h"
extern "C"  void * __cdecl _alloca(size_t);
#pragma intrinsic(_alloca)
#pragma intrinsic(strlen)

// for docs, see comment section below

#define cbWideForAnsiSize(cbAnsi)    ((cbAnsi)*sizeof(WCHAR))
#define cchWideForAnsiSize(cbAnsi)   (cbAnsi)
#define cbAnsiForWideSize(cbWide)    (cbWide)

// Win64Fix (MikhailA): strlen returns size_t which is 64-bit long.
// In this particular case I think int is sufficient and I won't need to fix ann the calls to the StrLenA
inline int cbWideForAnsiStr  (LPCSTR  sz) { return ((int)strlen(sz) + 1) * sizeof(WCHAR); }
inline int cchWideForAnsiStr (LPCSTR  sz) { return ((int)strlen(sz) + 1); }
inline int cbAnsiForWideStr  (LPCWSTR sz) { return (StrLen(sz) * sizeof(WCHAR)) + 1; }

PWSTR   WINAPI CopyWFromA   (PWSTR dest, PCSTR  src);
PSTR    WINAPI CopyAFromW   (PSTR  dest, PCWSTR src);

HRESULT WINAPI StrDupW      (PCWSTR psz, PWSTR * ppszNew); // free with VSFree
HRESULT WINAPI StrDupA      (PCSTR  psz, PSTR  * ppszNew); // free with VSFree
HRESULT WINAPI StrDupAFromW (PCWSTR psz, PSTR  * ppszNew); // free with VSFree
HRESULT WINAPI StrDupWFromA (PCSTR  psz, PWSTR * ppszNew); // free with VSFree

// cchMax is the max number of chars to duplicate into the new string,
// not including a trailing 0
HRESULT WINAPI StrNDupW     (PCWSTR psz, int cchMax, PWSTR * ppszNew); // free with VSFree
BSTR    WINAPI SysNDup      (PCWSTR psz, int cchMax); // free with SysFreeString

// Same as StrDupW, but conflicts with shlwapi.h which defines 
// StrDup as an A/W macro and breaks us.
HRESULT WINAPI StrDup       (PCWSTR psz, PWSTR * ppszNew); // free with VSFree

BSTR    WINAPI SysAllocStringFromA (PCSTR psz);

//---------------------------------------------------------------
// WIDESTR/ANSISTR
//
// WARNING:
// These macros use _alloca, which causes stack growth at EACH invocation.
// DO NOT use WIDESTR and ANSISTR in a loop.
// DO NOT use WIDESTR and ANSISTR with long strings.
//
#ifndef WIDESTR
#define WIDESTR(x)  ((HIWORD((UINT_PTR)(x)) != 0) ? CopyWFromA((LPWSTR) _alloca((strlen(x)+1) * 2), (x)) : (LPWSTR)(x))
#endif

#ifndef ANSISTR
#define ANSISTR(x)  ((HIWORD((UINT_PTR)(x)) != 0) ? CopyAFromW((LPSTR) _alloca((StrLen(x)*2)+1), (x)) : (LPSTR)(x))  // *2 for worst case DBCS string
#endif

//---------------------------------------------------------------

inline HRESULT WINAPI StrDup (PCWSTR sz, PWSTR * ppszNew)
{
    return StrDupW (sz, ppszNew);
}

//---------------------------------------------------------------
// cbWideForAnsiSize
//
// Returns: Max size of a WCHAR buffer required to hold the result
//          of converting cbAnsi bytes of an ANSI string to Unicode.
//
// This number is suitable for direct use as an argument to an allocator.
//
// Max count of wide characters that corresponds to an ANSI buffer of size cbAnsi
//

//---------------------------------------------------------------
// cbWideForAnsiStr
// 
// Returns: Max size of a WCHAR buffer required to hold the result
//          of converting sz to Unicode.
//
// This number is suitable for direct use as an argument to allocators
//

//---------------------------------------------------------------
//
// PWSTR CopyWFromA (PWSTR dest, LPCSTR src);
//
// Copy an ANSI string to a Unicode (wide) string. The destination
// is assumed to be large enough for the result.
//
// Returns: dest
//

//---------------------------------------------------------------
// PSTR  CopyAFromW (PSTR dest, PCWSTR src);
//
// Copy a Unicode (wide) string to an ANSI string. The destination
// is assumed to be large enough for the result.
//
// Returns: dest
//
#endif // __AWCONV_H__
