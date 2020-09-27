#ifndef __UTIL2_H__
#define __UTIL2_H__

#pragma once

#ifndef __UTIL_H__
// MBS <-> WCS conversion with truncation
#define _wcstombsz(lpszDest, lpszSrc, nMaxBytes) \
	wcstombs(lpszDest, lpszSrc, nMaxBytes), lpszDest[nMaxBytes-1] = 0
#endif	// __UTIL_H__

//
// Given a EXE filename determine it's executable type if any.
//
typedef enum {
    EXE_FAIL,
    EXE_NONE,
    EXE_DOS,
    EXE_WIN,
    EXE_NT,
    EXE_OS2_NE,
    EXE_OS2_LE
} EXT; /* EXecutable Type */

extern EXT GetExeType(LPCOLESTR lpstrEXEFilename, WORD * pwSubsystem);
bool CanCreateProcess( LPCOLESTR szExe );

extern LRESULT UtilSendMessageW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
extern LRESULT UtilSendMessageWString( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
FILE *fopenW( const wchar_t *szFilename, const wchar_t* szAttributes);

LPCOLESTR wcsistr(LPCOLESTR szSearch, LPCOLESTR szReqd);
LPSTR 	  stristr(char *szSearch, char *szReqd);

#endif	// __UTIL2_H__

