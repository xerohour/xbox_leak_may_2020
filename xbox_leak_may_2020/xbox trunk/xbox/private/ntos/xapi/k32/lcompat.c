/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    lcompat.c

Abstract:

    This module implements the _l and l compatability functions
    like _lread, lstrlen...

Author:

    Mark Lucovsky (markl) 13-Mar-1991

Revision History:

--*/

#include "basedll.h"
#ifdef _XBOX
#include "winnls.h"
#endif
#pragma hdrstop

#ifdef _XBOX
// Xbox-specific versions of CompareString (differ from Win32 version in that they don't need to
// have a locale specified, since the Xbox only has one locale supported).
extern int xCompareStringA(DWORD dwCmpFlags, LPCSTR psz1, int cch1, LPCSTR psz2, int cch2);
extern int xCompareStringW(DWORD dwCmpFlags, LPCTSTR psz1, int cch1, LPCTSTR psz2, int cch2);
#endif

int
APIENTRY
#ifdef LCOMPATA
lstrcmpA(
    LPCSTR lpString1,
    LPCSTR lpString2
    )
#else  // LCOMPATA
lstrcmpW(
    LPCWSTR lpString1,
    LPCWSTR lpString2
    )
#endif // LCOMPATA
{
    int retval;

#ifdef _XBOX 	// Only one locale on the xbox
#ifdef LCOMPATA
    retval = xCompareStringA(
#else
    retval = xCompareStringW(
#endif
#else
#ifdef LCOMPATA
    retval = CompareStringA( GetThreadLocale(),
#else
    retval = CompareStringW( GetThreadLocale(),
#endif
#endif
                             0,
                             lpString1,
                             -1,
                             lpString2,
                             -1 );
#ifndef _XBOX
    if (retval == 0)
    {
        //
        // The caller is not expecting failure.  Try the system
        // default locale id.
        //
        retval = CompareStringW( GetSystemDefaultLCID(),
                                 0,
                                 lpString1,
                                 -1,
                                 lpString2,
                                 -1 );
    }
#endif
    if (retval == 0)
    {
        if (lpString1 && lpString2)
        {
            //
            // The caller is not expecting failure.  We've never had a
            // failure indicator before.  We'll do a best guess by calling
            // the C runtimes to do a non-locale sensitive compare.
            //
#ifdef LCOMPATA
            return ( strcmp(lpString1, lpString2) );
#else  // LCOMPATA
            return ( wcscmp(lpString1, lpString2) );
#endif // LCOMPATA
        }
        else if (lpString1)
        {
            return (1);
        }
        else if (lpString2)
        {
            return (-1);
        }
        else
        {
            return (0);
        }
    }

    return (retval - 2);
}

int
APIENTRY
#ifdef LCOMPATA
lstrcmpiA(
    LPCSTR lpString1,
    LPCSTR lpString2
    )
#else  // LCOMPATA
lstrcmpiW(
    LPCWSTR lpString1,
    LPCWSTR lpString2
    )
#endif // LCOMPATA
{
    int retval;

#ifdef _XBOX	 // Only one locale on the xbox
#ifdef LCOMPATA
    retval = xCompareStringA(
#else
    retval = xCompareStringW(
#endif
#else
#ifdef LCOMPATA
    retval = CompareStringA( GetThreadLocale(),
#else
    retval = CompareStringW( GetThreadLocale(),
#endif
#endif
                             NORM_IGNORECASE,
                             lpString1,
                             -1,
                             lpString2,
                             -1 );
#ifndef _XBOX
    if (retval == 0)
    {
        //
        // The caller is not expecting failure.  Try the system
        // default locale id.
        //
        retval = CompareStringW( GetSystemDefaultLCID(),
                                 NORM_IGNORECASE,
                                 lpString1,
                                 -1,
                                 lpString2,
                                 -1 );
    }
#endif
    if (retval == 0)
    {
        if (lpString1 && lpString2)
        {
            //
            // The caller is not expecting failure.  We've never had a
            // failure indicator before.  We'll do a best guess by calling
            // the C runtimes to do a non-locale sensitive compare.
            //
#ifdef LCOMPATA
            return ( _stricmp(lpString1, lpString2) );
#else  // LCOMPATA
            return ( _wcsicmp(lpString1, lpString2) );
#endif // LCOMPATA
        }
        else if (lpString1)
        {
            return (1);
        }
        else if (lpString2)
        {
            return (-1);
        }
        else
        {
            return (0);
        }
    }

    return (retval - 2);
}


#ifdef LCOMPATA
LPSTR
APIENTRY
lstrcpyA(
    LPSTR lpString1,
    LPCSTR lpString2
    )
#else  // LCOMPATA
LPWSTR
APIENTRY
lstrcpyW(
    LPWSTR lpString1,
    LPCWSTR lpString2
    )
#endif // LCOMPATA
{
    __try {
#ifdef LCOMPATA
        return strcpy(lpString1, lpString2);
#else  // LCOMPATA
        return wcscpy(lpString1, lpString2);
#endif // LCOMPATA
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return NULL;
    }
}

#ifdef LCOMPATA
LPSTR
APIENTRY
lstrcpynA(
    LPSTR lpString1,
    LPCSTR lpString2,
    int iMaxLength
    )
#else  // LCOMPATA
LPWSTR
APIENTRY
lstrcpynW(
    LPWSTR lpString1,
    LPCWSTR lpString2,
    int iMaxLength
    )
#endif // LCOMPATA
{
#ifdef LCOMPATA
    LPSTR src,dst;
#else  // LCOMPATA
    LPWSTR src,dst;
#endif // LCOMPATA

    __try {
#ifdef LCOMPATA
        src = (LPSTR)lpString2;
#else  // LCOMPATA
        src = (LPWSTR)lpString2;
#endif // LCOMPATA
        dst = lpString1;

        if ( iMaxLength ) {
            while(iMaxLength && *src){
                *dst++ = *src++;
                iMaxLength--;
                }
            if ( iMaxLength ) {
#ifdef LCOMPATA
                *dst = '\0';
#else  // LCOMPATA
                *dst = L'\0';
#endif // LCOMPATA
                }
            else {
                dst--;
#ifdef LCOMPATA
                *dst = '\0';
#else  // LCOMPATA
                *dst = L'\0';
#endif // LCOMPATA
                }
            }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return NULL;
    }

    return lpString1;
}

#ifdef LCOMPATA
LPSTR
APIENTRY
lstrcatA(
    LPSTR lpString1,
    LPCSTR lpString2
    )
#else  // LCOMPATA
LPWSTR
APIENTRY
lstrcatW(
    LPWSTR lpString1,
    LPCWSTR lpString2
    )
#endif // LCOMPATA
{
    __try {
#ifdef LCOMPATA
        return strcat(lpString1, lpString2);
#else  // LCOMPATA
        return wcscat(lpString1, lpString2);
#endif // LCOMPATA
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return NULL;
    }
}

int
APIENTRY
#ifdef LCOMPATA
lstrlenA(
    LPCSTR lpString
    )
#else  // LCOMPATA
lstrlenW(
    LPCWSTR lpString
    )
#endif // LCOMPATA
{
    if (!lpString)
        return 0;
    __try {
#ifdef LCOMPATA
        return strlen(lpString);
#else  // LCOMPATA
        return wcslen(lpString);
#endif // LCOMPATA
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
}

