// unistr.h
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
// June 1, 1998 [paulde]
//
//-----------------------------------------------------------------
#pragma once
#ifndef __UNISTR_H__
#define __UNISTR_H__

#pragma intrinsic(strlen)

int     WINAPI   StrLen          (PCWSTR psz);
int     WINAPI   StrLenA         (PCSTR  psz);

//----------------------------------------------------------------
// Copy strings for catenation.  Returns the next position for appending.
PWSTR   WINAPI   CopyCat         (PWSTR dst, PCWSTR src);
PSTR    WINAPI   CopyCatA        (PSTR  dst, PCSTR  src);
PWSTR   WINAPI   CopyNCat        (PWSTR dst, PCWSTR src, int cchz);
PSTR    WINAPI   CopyNCatA       (PSTR  dst, PCSTR  src, int cchz);
PWSTR   WINAPI   CopyCatInt      (PWSTR dst, int n, int radix);
PSTR    WINAPI   CopyCatIntA     (PSTR  dst, int n, int radix);

// Copies into dst up to but not including stop. Guarantees zero termination if stop > dst.
PWSTR   WINAPI   CopyCatStop     (PWSTR dst, PCWSTR src, PCWSTR stop); 

//----------------------------------------------------------------
// NextChar, PrevChar - Walk text by logical characters
// These routines honor 'grapheme' boundaries. They are aware of things like 
// combining characters (e.g. diacritics), surrogate pairs, Hangul syllables 
// formed by Hangul combining Jamo.
//
// They return NULL after reaching the bound of the string or buffer.
//
PCWSTR  WINAPI   NextChar (PCWSTR pchPoint);                // NUL terminated
PCWSTR  WINAPI   NextChar (PCWSTR pchPoint, PCWSTR pchEnd); // length-bounded
PCWSTR  WINAPI   PrevChar (PCWSTR pchStart, PCWSTR pchPoint);

// VS7:32377 non-const variants, like CRT
inline PWSTR WINAPI NextChar (PWSTR pchPoint                ) {return const_cast<PWSTR>(NextChar (const_cast<PCWSTR>(pchPoint)));}
inline PWSTR WINAPI NextChar (PWSTR pchPoint, PWSTR pchEnd  ) {return const_cast<PWSTR>(NextChar (const_cast<PCWSTR>(pchPoint), const_cast<PCWSTR>(pchEnd)));}
inline PWSTR WINAPI PrevChar (PWSTR pchStart, PWSTR pchPoint) {return const_cast<PWSTR>(PrevChar (const_cast<PCWSTR>(pchStart), const_cast<PCWSTR>(pchPoint)));}

// Returns true if pchPoint is at a grapheme boundary
bool    WINAPI   IsGraphemeBreak (PCWSTR pchStart, PCWSTR pchPoint);

//----------------------------------------------------------------
// See docs below
//#define wcsnpbrk FindCharInSet
PCWSTR  WINAPI   FindCharInSet         (PCWSTR pchBuffer, int cchBuffer, PCWSTR set);
PCWSTR  WINAPI   FindCharInOrderedSet  (PCWSTR pchBuffer, int cchBuffer, PCWSTR set);
PCWSTR  WINAPI   FindChar              (PCWSTR psz, WCHAR ch);
PCWSTR  WINAPI   FindCharN             (PCWSTR pchBuffer, int cch, WCHAR ch);
PCWSTR  WINAPI   FindLastChar          (PCWSTR psz, WCHAR ch);
PCSTR   WINAPI   FindLastCharA         (PCSTR  psz, CHAR  ch);
PCWSTR  WINAPI   CharInOrderedSet      (WCHAR ch, PCWSTR set);

// VS7:32377  non-const variants, like CRT
inline PWSTR WINAPI FindCharInSet         (PWSTR pchBuffer, int cchBuffer, PCWSTR set) {return const_cast<PWSTR>(FindCharInSet (const_cast<PCWSTR>(pchBuffer), cchBuffer, set));}
inline PWSTR WINAPI FindCharInOrderedSet  (PWSTR pchBuffer, int cchBuffer, PCWSTR set) {return const_cast<PWSTR>(FindCharInOrderedSet (const_cast<PCWSTR>(pchBuffer), cchBuffer, set));}
inline PWSTR WINAPI FindChar              (PWSTR psz, WCHAR ch) {return const_cast<PWSTR>(FindChar (const_cast<PCWSTR>(psz), ch));}
inline PWSTR WINAPI FindCharN             (PWSTR pchBuffer, int cch, WCHAR ch) {return const_cast<PWSTR>(FindCharN (const_cast<PCWSTR>(pchBuffer), cch, ch));}
inline PWSTR WINAPI FindLastChar          (PWSTR psz, WCHAR ch) {return const_cast<PWSTR>(FindLastChar (const_cast<PCWSTR>(psz), ch));}
inline PSTR  WINAPI FindLastCharA         (PSTR  psz, CHAR  ch) {return const_cast<PSTR>(FindLastCharA (const_cast<PCSTR>(psz), ch));}

//----------------------------------------------------------------
// PathSplit  - Split a unc/path/filename into it's elements
//
// Forward/back slashes are normalized before splitting.
//
// Argument  Description    Required Size   Comments
// --------  ------------   -------------   -----------------------------------------
// pszFN     Source path    n/a
// pszV      Drive or UNC   MAX_PATH        e.g. "C:" or "\\server\share"
// pszD      Directory      MAX_PATH        Everything between Drive|UNC and name[.ext]
// pszN      Name           MAX_PATH
// pszE      .Ext           MAX_PATH
//
// Note: a path like "d:\some\silly\thing" returns "d:", "\some\silly\", "thing", ""
//
void WINAPI PathSplit (PCWSTR pszFN, PWSTR pszV, PWSTR pszD, PWSTR pszN, PWSTR pszE);

void WINAPI PathSplitInPlace
(
	PCWSTR pszPath,
	int iPathLength,
	int *piDrive,
	int *piDriveLength,
	int *piDirectory,
	int *piDirectoryLength,
	int *piFilename,
	int *piFilenameLength,
	int *piExtension,
	int *piExtensionLength
);

//----------------------------------------------------------------
// Calculate the line and character index of an offset into a text buffer
BOOL    WINAPI   LineAndCharIndexOfPos (PCWSTR pchText, int cch, int cchPos, int * piLine, int * piIndex);

//----------------------------------------------------------------
// StrList* operate on empty string-terminated lists of NUL-terminated strings (e.g. filter strings).
//
int     WINAPI   StrListSize   (PCWSTR psz); // count of chars to hold the list (includes terminator)
int     WINAPI   StrListSizeA  (PCSTR  psz); // count of bytes to hold the list (includes terminator)
int     WINAPI   StrListCount  (PCWSTR psz); // count of strings in the list (not including terminator)
int     WINAPI   StrListCountA (PCSTR  psz);
int     WINAPI   StrListCounts (PCWSTR psz, int * pcStr = NULL); // return count of chars to hold list, *pcStr=count of strings

PCWSTR * WINAPI  StrListCreateArray (PCWSTR pList, int * pcel);
PCWSTR  WINAPI   StrListNext        (PCWSTR pList);
HRESULT WINAPI   StrListSort        (PWSTR pList, bool fCase = true);
HRESULT WINAPI   StrListSortPairs   (PWSTR pList, bool fCase = true);
PCWSTR  WINAPI   StrListFind        (PCWSTR pList, PCWSTR pPattern, bool fCase = true);
PCWSTR  WINAPI   StrListFindSorted  (PCWSTR pList, PCWSTR pPattern, bool fCase = true);

// VS7:32377  non-const variants
inline PWSTR * WINAPI StrListCreateArray (PWSTR pList, int * pcel) {return const_cast<PWSTR*>(StrListCreateArray (const_cast<PCWSTR>(pList), pcel));}
inline PWSTR   WINAPI StrListNext        (PWSTR pList) {return const_cast<PWSTR>(StrListNext(const_cast<PCWSTR>(pList)));}
inline PWSTR   WINAPI StrListFind        (PWSTR pList, PCWSTR pPattern, bool fCase = true){return const_cast<PWSTR>(StrListFind        (const_cast<PCWSTR>(pList), pPattern, fCase));}
inline PWSTR   WINAPI StrListFindSorted  (PWSTR pList, PCWSTR pPattern, bool fCase = true){return const_cast<PWSTR>(StrListFindSorted  (const_cast<PCWSTR>(pList), pPattern, fCase));}

int     WINAPI   StrSubstituteChar  (PWSTR psz, WCHAR chOld, WCHAR chNew); 
int     WINAPI   StrSubstituteCharA (PSTR  psz, CHAR  chOld, CHAR  chNew); // skips double byte chars

//----------------------------------------------------------------
// !! WARNING!! If you call FindURL or IsProtocol, you must call FreeCachedURLResources()
#define NO_HITTEST -1
BOOL    WINAPI   FindURL (
    PCWSTR sz,                // IN buffer
    int iLen,                 // IN length of buffer
    int iAt,                  // IN index of point to intersect, or NO_HITTEST
    INT_PTR * piStart,        // IN/OUT starting index to begin scan (IN), start of URL (OUT)
    INT_PTR * piEndProtocol,  // OUT index of end of protocol
    INT_PTR * piEnd           // OUT index of end of URL
    );
BOOL    WINAPI   IsProtocol (PCWSTR sz);
void    WINAPI   FreeCachedURLResources (void);

//----------------------------------------------------------------
void    WINAPI   SwapSegments    (PWSTR x, PWSTR y, PWSTR z);
void    WINAPI   PivotSegments   (PWSTR pA, PWSTR pB, PWSTR pC, PWSTR pD);

#define TRUNC_BEGIN    0 // remove text at the very beginning
#define TRUNC_LEFT     1 // remove text towards the beginning
#define TRUNC_CENTER   2 // remove text from the center
#define TRUNC_RIGHT    3 // remove text towards the end
#define TRUNC_END      4 // remove text from the very end
void    WINAPI   FitText(PCWSTR pszText, int cchText, PWSTR pszDst, int cchDst, PCWSTR pszFill, DWORD flags);

inline  BOOL WINAPI AbbreviateText  (PCWSTR szText, int cchGoal, PCWSTR szFiller, PCWSTR szDelim, PWSTR szBuf, int cchBuf)
{
    //VSASSERT(cchGoal <= cchBuf);
    (void)szDelim;
    (void)cchBuf;
    FitText(szText, -1, szBuf, cchGoal, szFiller, TRUNC_CENTER);
    return TRUE;
}
// anyone using this version?
BOOL    WINAPI   AbbreviateTextA (PCSTR  szText, int cchGoal, PCSTR  szFiller, PCSTR  szDelim, PSTR  szBuf, int cchBuf);

// NormalizeFileSlashes
// Convert forward slashes to backslashes.
// Reduce multiple slashes to a single slash (leading double slash allowed).
//
// To normalize to forward slashes use StrSubstituteChar(psz, '\\', '/'); after NormalizeFileSlashes
//
// Returns a pointer to the 0 terminator of the transformed string
//
PWSTR   WINAPI   NormalizeFileSlashes  (PWSTR szFile);
PSTR    WINAPI   NormalizeFileSlashesA (PSTR  szFile);

void    WINAPI   TrimWhitespace  (PWSTR psz); // removes leading and trailing whitespace from psz

// Remove blanks adjacent to line ends within a buffer.
#define TLB_START      1
#define TLB_END        2
int     WINAPI   TrimLineBlanks  (PWSTR pchBuf, int cch, DWORD dwFlags);

enum StripBlanksFlags{
//  --------------------    ------     -----------------------   -------------------------
//  Flag:                   Value:     On input:                 Return value:
//  --------------------    ------     -----------------------   -------------------------
    STB_NONE              = 0x0000, // Remove all blanks.        No blanks found.
    STB_SINGLE            = 0x0001, // Reduce to single blanks.  Only single blanks found.
    STB_MULTIPLE          = 0x0002, // (N/A)                     Multiple blanks found.
    STB_STRIP_LINEBREAKS  = 0x0010  // Include line breaks.      (N/A)
};
StripBlanksFlags  WINAPI  StripBlanks (WCHAR * pchBuf, INT_PTR * plen, int flags = STB_NONE);

// Determine whether a file path is local, UNC, or absolute 
//
// These examine the string only, and only the first part at that.  They don't
// attempt to decide whether the path is a valid filename or not.

BOOL WINAPI IsLocalAbsPath(PCWSTR sz);   // Path starts with <letter>:
BOOL WINAPI IsUNC(PCWSTR sz);            // Path starts with two backslashes
BOOL WINAPI IsAbsPath(PCWSTR sz);        // Either of the above

// Remove Bidi formatting characters from a string if the underlying system does not support them.
// They would be displayed as ? on Win9x systems that don't have appropriate codepage support.

void WINAPI StripUnsupportedBidiFormatChars(PWSTR sz);

//-----------------------------------------------------------------
/*
  BOOL FindURL (
    PCWSTR sz,                // IN buffer
    int iLen,                 // IN length of buffer
    int iAt,                  // IN index of point to intersect, or NO_HITTEST
    INT_PTR * piStart,        // IN/OUT starting index to begin scan (IN), start of URL (OUT)
    INT_PTR * piEndProtocol,  // OUT index of end of protocol
    INT_PTR * piEnd           // OUT index of end of URL
    );

  Find an URL in text, starting at *piStart index into wsz.
  iAt is NO_HITTEST to find the first URL in the text.
  To find an URL that intersects a point in the text, iAt is index from wsz of the point.
*/

//-----------------------------------------------------------------
//
// int StrLen (PCWSTR psz);
// Returns: count of chars in string 
//

//-----------------------------------------------------------------
//
// PSTR FindLastCharA (PCSTR psz, CHAR ch);
//
// MBCS-aware version of strrchr.
// Returns: pointer to right-most instance of ch in psz.
//

//-----------------------------------------------------------------
//
// PWSTR FindCharInSet (PCWSTR pchBuffer, int cchBuffer, PCWSTR set);
//
// Length-limited wide-char version of strpbrk.
// Returns: pointer to first char from set in buffer.
//

//-----------------------------------------------------------------
//
// PWSTR CopyCat (PWSTR dst, PCWSTR src)
//
// Wide char string copy for concatenation. Copy Src to Dst.
// Return: position of NUL in dst for further catenation.
//

//-----------------------------------------------------------------
//
// PWSTR CopyNCat (PWSTR dst, PCWSTR src, int cchz);
//
// NUL-limited char copy up to n chars for catentation
//
// Return: Position after char n in destination or position of 
//         copied NUL for further catenation.
//

//-----------------------------------------------------------------
//
// int StrListSize   (PCWSTR psz); 
//
// Return: count of chars to hold the list (includes terminator)

//-----------------------------------------------------------------
//
// int StrListSizeA  (PCSTR  psz); 
//
// Return: count of bytes to hold the list (includes terminator)

//-----------------------------------------------------------------
//
// int StrListCount  (PCWSTR psz); 
//
// Return: count of strings in the list (not including terminator)

//-----------------------------------------------------------------
//
// int StrListCountA (PCSTR  psz);
//
// Return: count of strings in the list (not including terminator)

//-----------------------------------------------------------------
//
// int StrListCounts (PCWSTR psz, int * pcStr = NULL); 
//
// pcStr    Receives count of strings in the list (not including terminator)
//
// Return: return count of chars to hold list
//

//-----------------------------------------------------------------
//
// PCWSTR * StrListCreateArray (PCWSTR pList, int * pcel);
//
// pcel     Receives a count of elements in the returned array,
//          not including the terminating NULL entry.
//
// Return: NULL-terminated array of PWSTRs pointing to the strings 
//         in pList, or NULL if out of memory.
//
// You must free the returned array using VSFree
//

//-----------------------------------------------------------------
//
// PCWSTR StrListNext (PCWSTR pList);
//
// Return: Pointer to the next non-empty string in the list, or 
//         NULL if no more strings
//

//-----------------------------------------------------------------
//
// HRESULT StrListSort (PWSTR pList, bool fCase = true);
//
// Sorts the string list
//
// fCase    true for case-sensitive, false for case-insensitive
//
// Return: success
//

//-----------------------------------------------------------------
//
// HRESULT StrListSortPairs (PWSTR pList, bool fCase = true);
//
// Sorts the paired-string (key/value) list. There must be an even 
// number of strings in the list. Keys and values must not contain L'\1'.
//
// fCase    true for case-sensitive, false for case-insensitive
//
// Return: success
//

//-----------------------------------------------------------------
//
// PCWSTR StrListFind (PCWSTR pList, PCWSTR pPattern, bool fCase = true);
//
// pList     Sorted list of strings to search
// pPattern  String to find in list
// fCase     true to match exact case, false to ignore case 
//
// Return: Matching string in list or NULL if not found
//

//-----------------------------------------------------------------
//
// PCWSTR StrListFindSorted (PCWSTR pList, PCWSTR pPattern, bool fCase = true);
//
// pList     Sorted list of strings to search
// pPattern  String to find in list
// fCase     true to match exact case, false to ignore case 
//           The list must be sorted in ascending order with the same fCase
//
// Return: Matching string in list or NULL if not found
//

//-----------------------------------------------------------------
//
// int StrSubstituteChar  (PWSTR psz, WCHAR chOld, WCHAR chNew); 
// int StrSubstituteCharA (PSTR  psz, CHAR  chOld, CHAR  chNew); // skips double byte chars
//
// Return: Count of chars replaced
//

//-----------------------------------------------------------------
//
// void TrimWhitespace (PWSTR psz);
//
// Remove whitespace from the start and end of the string
//

//-----------------------------------------------------------------
//
// int TrimLineBlanks (PWSTR pchBuf, int cch, DWORD dwFlags);
//
// Remove blanks adjacent to line ends within a buffer.
//
// cch == -1 for zero-terminated string.
// Set TLB_START in dwFlags to also remove blanks at the start of the buffer.
// Set TLB_END in dwFlags to also remove blanks at the end of the buffer.
// 
// Returns trimmed length (if cch == -1, length includes terminator)
// 

//-----------------------------------------------------------------
//
// BOOL AbbreviateText (PCWSTR szText, int cchGoal, PCWSTR szFiller, PCWSTR szDelim, PWSTR szBuf, int cchBuf);
//
//  szText      String to abbreviate
//  cchGoal     Size to reduce it too, including filler
//  szFiller    String to replace deleted text with (e.g. "...")
//  szDelim     Set of characters that delimit chunks of text
//  szBuf       Buffer for result
//  cchBuf      Size of szBuf
//
// Returns FALSE if szText couldn't be shortened to cchGoal.
//

//-----------------------------------------------------------------
//
// void WINAPI SwapSegments(PWSTR x, PWSTR y, PWSTR z);
//
// Swap two segments of a string.
//
//  IN: xxxxxYYYz
// OUT: YYYxxxxxz
//
// Z can point to the zero terminator or past the end of the buffer.
// The swap is performed in-place.
//

//-----------------------------------------------------------------
// void PivotSegments (PWSTR pA, PWSTR pB, PWSTR pC, PWSTR pD);
//
// Pivot two segments of a string around a middle segment.
//
//  IN: aaaaaaaBBcccccD
// OUT: cccccBBaaaaaaaD
//
// D can point to the zero terminator or past the end of the buffer.
// The pivot is performed in-place.
//

//=================================================================
//===== Implementation ============================================
//=================================================================

inline int WINAPI StrLen (PCWSTR psz)
{
    if (!psz) return 0;
    PCWSTR pch = psz;
    while (*pch)
        pch++;

    // ASSUME: length never > 4GB
    return (LONG32) (pch - psz);
}

inline int WINAPI StrLenA (PCSTR psz) 
{ 
    if (!psz) return 0;
	// Win64Fix (MikhailA): strlen returns size_t which is 64-bit long.
	// In this particular case I think int is sufficient and I won't need to fix ann the calls to the StrLenA
    return (int) strlen(psz);
}

inline PWSTR WINAPI CopyCat (PWSTR dst, PCWSTR src)
{
    while (*dst++ = *src++)
        ;
    return --dst; 
}

inline PWSTR WINAPI CopyCatStop (PWSTR dst, PCWSTR src, PCWSTR stop)
{
    if (dst >= stop) return dst;
    while ((dst < stop) && (*dst++ = *src++))
        ;
    --dst;
    if (!*dst)
      *dst = 0;
    return dst;
}

inline PSTR WINAPI CopyCatStopA (PSTR dst, PCSTR src, PCSTR stop)
{
    if (dst >= stop) return dst;
    while ((dst < stop) && (*dst++ = *src++))
        ;
    --dst;
    if (!*dst)
      *dst = 0;
    return dst;
}

// Ansi version
inline PSTR WINAPI CopyCatA (PSTR dst, PCSTR src)
{
    while (*dst++ = *src++)
        ;
    return --dst; 
}

// Guarantees zero termination. Can write one more than cchz
inline PWSTR WINAPI CopyNCat (PWSTR dst, PCWSTR src, int cchz)
{
    WCHAR ch = 0xFFFF; // UCH_NONCHAR
    while (cchz-- && (ch = *dst++ = *src++))
        ;
    if (ch)
    {
        *dst = 0;
        return dst;
    }
    else
        return --dst;
}

// does not guarantee zero termination, never writes more than cch
inline PWSTR WINAPI MemCopyNCat (PWSTR dst, PCWSTR src, int cch)
{
    while (cch-- && (*dst++ = *src++))
        ;
    if (cch >= 0)
        --dst;
    return dst;
}

// Ansi version
inline PSTR WINAPI CopyNCatA (PSTR dst, PCSTR src, int cchz)
{
    CHAR ch = 1;
    while (cchz-- && (ch = *dst++ = *src++))
        ;
    if (ch)
    {
        *dst = 0;
        return dst;
    }
    else
        return --dst;
}

inline PWSTR WINAPI CopyCatInt (PWSTR dst, int n, int radix)
{
	_itow(n, dst, radix);
	while (*dst)
		dst++;
	return dst;
}

inline PSTR WINAPI CopyCatIntA (PSTR dst, int n, int radix)
{
	_itoa(n, dst, radix);
	while (*dst)
		dst++;
	return dst;
}

inline PCWSTR WINAPI FindChar (PCWSTR psz, WCHAR ch)
{
    if (!psz) 
        return NULL;
    WCHAR T;
    do
	{
        T = *psz;
        if (T == ch)
            return psz;
        psz++;
    } while (T);
    return NULL;
}

inline PCWSTR WINAPI FindSlash (PCWSTR psz)
{
    if (!psz) 
        return NULL;
    WCHAR T;
    do
	{
        T = *psz;
        if ((T == L'\\') || (T == L'/'))
            return psz;
        psz++;
    } while (T);
    return NULL;
}

inline PCWSTR WINAPI FindCharN (PCWSTR pchBuffer, int cch, WCHAR ch)
{
    if (!pchBuffer) 
        return NULL;
    while (cch--)
    {
        if (ch == *pchBuffer)
            return pchBuffer;
        ++pchBuffer;
    }
    return NULL;
}

inline PCWSTR WINAPI FindLastChar (PCWSTR  psz, WCHAR ch)
{
    if (!psz) 
        return NULL;
    PCWSTR pch = NULL;
    WCHAR T;
    for (; T = *psz; psz++)
    {
        if (T == ch)
            pch = psz;
    }
    return pch;
}

inline PCWSTR WINAPI FindLastSlash (PCWSTR  psz, int iLength)
{
    if (!psz || !iLength) 
        return NULL;

    for (PCWSTR pch = psz + (iLength-1); pch >= psz; pch--)
    {
        if ((*pch == L'\\') || (*pch == L'/'))
            return pch;
    }
    return NULL;
}

inline BOOL WINAPI IsLocalAbsPath (PCWSTR sz)
{
    return ((sz[0] >= L'A' && sz[0] <= L'Z') || (sz[0] >= L'a' && sz[0] <= L'z'))
        && sz[1] == L':';
}

inline BOOL WINAPI IsUNC (PCWSTR sz)
{
    return sz[0] == L'\\' && sz[1] == L'\\';
}

inline BOOL WINAPI IsAbsPath (PCWSTR sz)
{
    return IsLocalAbsPath(sz) || IsUNC(sz);
}

#endif __UNISTR_H__

