// regexp.h
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
// June 1, 1998 [paulde] revised for Unicode lib
//

#pragma once
#ifndef __REGEXP_H__
#define __REGEXP_H__

#define MAXPATARG   10          // 0 is entire 1-9 are valid

typedef unsigned char RE_OPCODE;

// compiled pattern
typedef struct {
    BOOL        fCase;              // TRUE => case is significant
    int         RESize;             // Estimated size
    RE_OPCODE * REip;               // Instruction pointer
    UINT        cArg;               // Total groups matched
    int         REArg;              // Current group
    WCHAR     * pArgBeg[MAXPATARG]; // Beginning of tagged strings
    WCHAR     * pArgEnd[MAXPATARG]; // End of tagged strings
    RE_OPCODE   code[1];            // Pseudo-code instructions
} REPattern;

// *ppat should be null when called
// free after use with REFree
HRESULT WINAPI RECompile     (WCHAR * p, BOOL fCase, REPattern ** ppat);
HRESULT WINAPI SimpleCompile (WCHAR * p, BOOL fCase, REPattern ** ppat);

BOOL WINAPI RESearch(
    PCWSTR        buf,
    ULONG_PTR     * ichStart,         // WCHAR index into buf where to start
    BOOL          fForward,         // TRUE if forward search, FALSE if backward
    BOOL          fLineStart,       // TRUE if buf starts at the start of a line
    BOOL          fLineEnd,         // TRUE if buf ends at the end of a line
    REPattern   * pat,              // compiled pattern
    PCWSTR        szString,         // pointer to regular expression
    RE_OPCODE *** pREStack,         // stack  -- should point to NULL first call, REFree when done with all calls
    ULONG       * pcREStackEntries, // stack size
    ULONG_PTR   * xEnd,             // out
    BOOL        * pfMatchLineStart, // out
    BOOL        * pfMatchLineEnd    // out
    );

HRESULT WINAPI RETranslate(REPattern *, WCHAR *, BSTR *);

// get length required to hold resulting translation
int WINAPI RETranslateLength (REPattern * pat, PWSTR src);

void WINAPI REFree (void * pv);

#endif  // __REGEXP_H__
