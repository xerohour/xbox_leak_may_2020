/***************************************************************************
 *
 *  Copyright (C) 2/4/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       strhlp.h
 *  Content:    String helpers.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/4/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __STRHLP_H__
#define __STRHLP_H__

EXTERN_C void FormatStringResource(LPTSTR pszText, UINT nLength, UINT nResourceId, ...);
EXTERN_C LPCTSTR FormatStringResourceStatic(UINT nResourceId, ...);
EXTERN_C BOOL ParseCommandLine(LPCTSTR pszCmdLine, LPTSTR *ppszParsed, LPUINT pnArgCount);
EXTERN_C void GetXDKVersion(LPTSTR pszString);
EXTERN_C void FormatNumber(int nValue, BOOL fSigned, BOOL fThousands, LPTSTR pszString);
EXTERN_C UINT MsgBoxV(HWND hWnd, DWORD dwFlags, LPCTSTR pszText, va_list va);
EXTERN_C UINT MsgBox(HWND hWnd, DWORD dwFlags, LPCTSTR pszText, ...);
EXTERN_C UINT MsgBoxResource(HWND hWnd, DWORD dwFlags, UINT nStringResourceId, ...);

#endif // __STRHLP_H__
