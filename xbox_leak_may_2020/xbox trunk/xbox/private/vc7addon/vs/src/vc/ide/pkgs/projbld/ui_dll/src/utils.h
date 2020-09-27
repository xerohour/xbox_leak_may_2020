// ---------------------------------------------------------------------------
// Microsoft Visual C++
//
// Microsoft Confidential
// Copyright 1994 - 2000 Microsoft Corporation. All Rights Reserved.
//
// utils.h : declarations for a suite of simple utility functions.
// 
// NOTE: All functions here are mbcs/unicode compliant.
//		 Do NOT add non mbcs/unicode compliant functions!
// ---------------------------------------------------------------------------
#ifndef __UTILS_H__
#define __UTILS_H__

class CISBaseFileNode;

// Creates a directory and all the subdirectories up to it.
BOOL UtilCreateDirectory(LPCTSTR name);
BOOL UtilCreateDirectoryW(const wchar_t* name);

int UtilMessageBox(LPCTSTR lpszText, UINT nType = MB_OK|MB_ICONEXCLAMATION, LPCTSTR pszCaption = NULL, HWND hParent = NULL);
int UtilMessageBox(UINT nIDPrompt, UINT nType = MB_OK|MB_ICONEXCLAMATION, LPCTSTR pszCaption = NULL,  HWND hParent = NULL);

void UtilGetProjUIFolder(CString& strProjItemDir);

LRESULT UtilSendMessageW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
LRESULT UtilSendMessageWString( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

HRESULT UtilSetErrorInfo(HRESULT hrError, LPCOLESTR pszDescription);

#endif // __UTILS_H__

