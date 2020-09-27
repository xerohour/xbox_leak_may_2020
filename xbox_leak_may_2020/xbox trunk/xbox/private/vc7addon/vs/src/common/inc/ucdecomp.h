// ucDecomp.h
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
// June 15, 1998 [paulde]
//
//-----------------------------------------------------------------
#pragma once
#ifndef __UCDECOMP_H__
#define __UCDECOMP_H__

PCWSTR  WINAPI  Decomposition     (WCHAR ch);
int     WINAPI  DecomposeStr      (PCWSTR src, int cchSrc, PWSTR dst, int cchDst);
BOOL    WINAPI  CanonCombiningStr (PWSTR  str, int cch);
int     WINAPI  CanonDecomposeStr (PCWSTR src, int cchSrc, PWSTR dst, int cchDst);

//---------------------------------------------------------------------
//
// PCWSTR Decomposition (WCHAR ch)
//
// Return decomposition of ch, or NULL if none.
//

//---------------------------------------------------------------------
//
// int DecomposeStr (PCWSTR src, int cchSrc, PWSTR dst, int cchDst)
//
// Decompose src into dst.
//
// cchSrc can be -1 for null-terminated mode.
// cchDst can be 0 to query for the required size to hold the decomposed string.
//
// Returns count of chars written.
//
// The decomposition is a maximal decomposition, but combining chars 
// are not in canonical order. See CanonCombiningStr and CanonDecomposeStr.
//

//---------------------------------------------------------------------
//
// BOOL CanonCombiningStr (PWSTR str, int cch)
//
// Put string with combining characters into canonical order.
//
// Returns : nonzero if successful, 0 on failure. 
// Call GetLastError() for more info.
//

//---------------------------------------------------------------------
//
// int CanonDecomposeStr (PCWSTR src, int cchSrc, PWSTR dst, int cchDst)
//
// Create a maximally decomposed string in canonical order.
//
// cchSrc can be -1 for null-terminated mode.
// cchDst can be 0 to query for the required size to hold the decomposed string.
//
// Returns count of chars written if successful, 0 on failure
// Call GetLastError() for more info.
//

#endif // __UCDECOMP_H__
