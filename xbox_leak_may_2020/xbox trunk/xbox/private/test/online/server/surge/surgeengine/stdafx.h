// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__12310FD2_2279_11D3_944E_00C04F7986A3__INCLUDED_)
#define AFX_STDAFX_H__12310FD2_2279_11D3_944E_00C04F7986A3__INCLUDED_

// disable unicode
#undef UNICODE
#undef _UNICODE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <time.h>

#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>

#ifdef _DEBUG
    #include <crtdbg.h>
#endif

#define CLIENTNAME_LEN (MAX_COMPUTERNAME_LENGTH + 1)
extern void _cdecl Trace(LPCTSTR lpszFormat, ...);
extern void AddWindowText(HWND window, char *text);
extern char* GetToken(char *str, char **tokenStart, size_t *tokenLen, char **nextToken);
extern char* MD5Hash(char *inSource, char *outStr);


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__12310FD2_2279_11D3_944E_00C04F7986A3__INCLUDED)
