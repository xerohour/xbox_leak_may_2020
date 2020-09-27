// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     compstr.c
// Contents: This module implements xCompareString functionality.  Note that xCompareString differs
//           from the Win32 CompareString in that it does not take a locale identifier (unnecessary
//           since Xbox only has one locale.
//
// Author:   Jeff Simon (jeffsim) 16-May-2001
//
// UNDONE:   * Verify CompareStringW vs CompareStringA functionality
//           * How do Kanji and the kanas fit into g_rgdwLoc? Or handled algorithmically?
//           * What is Kanji, hira, kata, ascii sort order?
//           * Move defines to private header file to share them with compstra.c
//
// Copyright Microsoft Corporation
// 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "basedll.h"
#include "winnls.h"
#include <assert.h>
#pragma hdrstop


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Externally defined variables
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Conversion table.  This will be filled in algorithmically by InitLocTable at startup.
// UNDONE-ERR: Currently only big enough to hold the 256 ASCII chars.  Not sure how to handle
//             Kanji and the kanas -- will I add them to the table, or handle them algorithmically?
extern DWORD g_rgdwLoc[256];

extern void InitLocTable();

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Internal Defines - specific to this file
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Sort order (lower letters are sorted before upper letters; symbols are sorted before numbers...)
// UNDONE-ERR: Just guessing on the sort order of the non-ascii chars (kanji and kanas).  Guessing
//             that the sort order (first->last) would be "ascii", "kata", "hira", "kanji".
#define COMP_INVALID    0x80000000
#define COMP_KANJI      0x00800000
#define COMP_HIRAGANA   0x00400000
#define COMP_KATAKANA   0x00200000
#define COMP_ULETTER    0x00100000
#define COMP_LLETTER    0x00080000
#define COMP_NUMBER     0x00040000
#define COMP_SYMBOL     0x00020000
#define COMP_WORDSORT   0x00010000

// NEXTCHAR -- Simple define for skipping to the next character in the main CompareString loop
#define NEXTCHAR(x) {  pstr##x++; cch##x--;  }

// SYM_*    -- Defines for handling wordsort special symbols
#define SYM_NONE   0                // No special symbol encountered
#define SYM_1      1                // special symbol already encountered in pstr1
#define SYM_2      2                // special symbol already encountered in pstr2
#define SYM_BOTH   SYM_1 | SYM_2    // special symbol already encountered in both pstr1 and pstr2
#define SYM_1FIRST 4                // pstr1 had the first special symbol encountered
#define SYM_2FIRST 8                // pstr2 had the first special symbol encountered


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Functions
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  xCompareStringA/xCompareStringW
// Purpose:   Xbox-specific version of the CompareString function.  Compares two strings using the
//            global Xbox locale.  Differs from the Win32 version of CompareString in that we don't
//            need to have a locale specified.
// Arguments: dwCmpFlags        -- Comparison-style options
//            pstr1             -- First string to compare
//            cch1              -- Number of characters in first string to examine (-1 for 'all')
//            pstr2             -- First string to compare
//            cch2              -- Number of characters in first string to examine (-1 for 'all')
// Return:    CSTR_LESS_THAN    if pstr1 sorts BEFORE pstr2 (ie  CompareString("a","b") )
//            CSTR_EQUAL        if pstr1 EQUALS pstr2       (ie  CompareString("a","a") )
//            CSTR_GREATER_THAN if pstr1 sorts AFTER pstr2  (ie  CompareString("b","a") )
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef COMPSTRA
int xCompareStringA(DWORD dwCmpFlags, LPCSTR pstr1, int cch1, LPCSTR pstr2, int cch2)
#else
int xCompareStringW(DWORD dwCmpFlags, LPCWSTR pstr1, int cch1, LPCWSTR pstr2, int cch2)
#endif
{
    // w1, w2, dw1, dw2 -- Temp variables
    WORD  w1, w2;
    DWORD dw1, dw2;
    
    // nSymFound        -- Track which string(s) had special symbols, and which came first.
    int nSymFound = SYM_NONE;

    // Avoid repetitive flag masking in the while loop by doing it once here...
    BOOL fWordSort      = !(dwCmpFlags & SORT_STRINGSORT);
    BOOL fIgnoreCase    = dwCmpFlags & NORM_IGNORECASE;
    BOOL fIgnoreSymbols = dwCmpFlags & NORM_IGNORESYMBOLS;

    // s_rgnSymLookup   -- Optimization to remove comparisons from the case where both strings are
    //                     the same length and have word-sort symbols in them
    static int s_rgnSymLookup[] = {CSTR_EQUAL, 0,0,0,0, CSTR_GREATER_THAN, 0, CSTR_LESS_THAN, 0,0,
                                   CSTR_LESS_THAN, CSTR_GREATER_THAN};
    
    // Verify parameters
    if (pstr1 == NULL || pstr2 == NULL || cch1 < -1 || cch2 < -1) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    // UNDONE-WARN: I'm not sure if we actually need to handle NORM_IGNORENONSPACE.  If I read MSDN
    //              correctly, we can ignore it, but I need to verify this.

    // UNDONE-ERR: Remove following asserts and handle appropriately
    // assert(!(dwCmpFlags & NORM_IGNOREKANATYPE));    // not handled
    // assert(!(dwCmpFlags & NORM_IGNOREWIDTH));       // not handled


    // Loop until one of the limits is reached (or we return from inside the while loop).
    while (cch1 != 0 && cch2 != 0) {
        // Check if we've reached the end of either string.
        if (*pstr1 == '\0' && *pstr2 == '\0') return s_rgnSymLookup[nSymFound];
        if (*pstr1 == '\0')                   return CSTR_LESS_THAN;
        if (*pstr2 == '\0')                   return CSTR_GREATER_THAN;

        // Both strings still have characters -- do locale-specific comparison of next character
        
        // w1, w2   -- Track the character codes
        // UNDONE-ERR: Need to figure out how I'm going to handle the table for >256 and then
        //             modify these lines appropriately.
#if 1   // def COMPSTRA
        w1 = (WORD)((BYTE)*pstr1);
        w2 = (WORD)((BYTE)*pstr2);
#else
        w1 = (WORD)(*pstr1);
        w2 = (WORD)(*pstr2);
#endif

        // dw1, dw2 -- Track character code or'ed in with bucket describing the 'type' of the char.
        dw1 = g_rgdwLoc[w1];
        dw2 = g_rgdwLoc[w2];

        // UNDONE-OPT: Could pull wordsort check outside of the loop to remove an if per iteration.
        //             Same with fIgnoreSymbols, etc.  However, that particular optimization
        //             (#including a file with particular defines) obfuscates the code quite a bit.

        // Word sort -- some chars are special cased; if we hit one, then skip the character,
        // but track which string(s) have the chars, and which had the first special char in case
        // the strings otherwise match. (ie, correct ordering: "ab-cd", "abce", "a-bce", "ab-ce")
        if (fWordSort) {
            if (dw1 & COMP_WORDSORT) {
                // If both are special symbols, then skip them.  Also need to track if they're
                // different symbols (ie "'" in one string and "-" in the other)
                if (dw2 & COMP_WORDSORT) {
                    if (dw1 < dw2)      nSymFound = SYM_BOTH | SYM_1FIRST;
                    else if (dw1 > dw2) nSymFound = SYM_BOTH | SYM_2FIRST;
                    NEXTCHAR(1); NEXTCHAR(2);
                    continue;
                } else {
                    if (nSymFound == SYM_NONE) nSymFound = SYM_1FIRST;
                    nSymFound |= SYM_1;
                    NEXTCHAR(1);
                    continue;
                }
            }
            if (dw2 & COMP_WORDSORT) {
                if (nSymFound == SYM_NONE) nSymFound = SYM_2FIRST;
                nSymFound |= SYM_2;
                NEXTCHAR(2);
                continue;
            }

            // If here, then neither of the current characters were special symbols.  Continue
            // processing them...
        }
        
        if (fIgnoreSymbols) {
            if (dw1 & COMP_SYMBOL) {
                NEXTCHAR(1);
                continue;
            }
            if (dw2 & COMP_SYMBOL) {
                NEXTCHAR(2);
                continue;
            }
        }

        if (fIgnoreCase) {
            // Handle case-insensitivity.  (+32 converts from upper to lower in our table)
            if (dw1 & COMP_ULETTER)  dw1 = COMP_LLETTER | w1 + 32;
            if (dw2 & COMP_ULETTER)  dw2 = COMP_LLETTER | w2 + 32;
        }

        if (dw1 < dw2) return CSTR_LESS_THAN;
        if (dw1 > dw2) return CSTR_GREATER_THAN;

        // Strings Matched.  Move on to the next character
        NEXTCHAR(1); NEXTCHAR(2);
    }
    
    // If here, then we passed the limit on one of the strings, and both strings are equal so far.
    
    // Check if we hit both limits
    if (cch1 == 0 && cch2 == 0) {
        // If we're in a wordsort search, then we need to take the special symbol state into account.
        // (If we're in a stringsort search, then nSymFound == 0, so we'll return CSTR_EQUAL).
        return s_rgnSymLookup[nSymFound];
    }
    
    // If here, then we hit just one limit, and that string comes second...
    return (cch1 == 0) ? CSTR_GREATER_THAN : CSTR_LESS_THAN;
}

