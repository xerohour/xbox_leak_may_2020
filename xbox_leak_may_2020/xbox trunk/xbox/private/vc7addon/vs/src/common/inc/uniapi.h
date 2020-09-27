// UniApi.h - Unicode API wrappers
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
// November 8, 1998 [paulde]
//
//
// TODO: 64-bit: these build targets are Unicode only so
//       the wrappers can be a simple #define alias for the W API.
//
#pragma once

//$UNDONE(olgaark)  We should keep this file free of ATL. The CCOmBSTR helpers should be moved.
#ifndef UNIAPI_NOATL
#include "atlbase.h"
#endif

//---------------------------------------------------------------
// Initialization

// This function must be called once before anything else to detect system support
BOOL    WINAPI W_IsUnicodeSystem();

//---------------------------------------------------------------
// API Wrappers
//---------------------------------------------------------------
#if defined(_M_IA64) 

#define W_FindFirstFile FindFirstFileW
#define W_FindNextFile          FindNextFileW
#define W_GetFileAttributes     GetFileAttributesW
#define W_SetFileAttributes     SetFileAttributesW
#define W_GetFullPathName       GetFullPathNameW
#define W_CreateFile            CreateFileW
#define W_GetCurrentDirectory   GetCurrentDirectoryW
#define W_GetTempPath           GetTempPathW
#define W_GetTempFileName       GetTempFileNameW
#define W_DeleteFile            DeleteFileW
#define W_GetWindowTextLength   GetWindowTextLengthW
#define W_GetWindowText         GetWindowTextW
#define W_RegisterClass         RegisterClassW
#define W_CreateWindowEx        CreateWindowExW
#define W_CreateWindow(C,W,S,x,y,w,h,p,m,i,L) CreateWindowExW(0L,C,W,S,x,y,w,h,p,m,i,L)
#define W_CreateDialogParam     CreateDialogParamW
#define W_CreateDialog(i,t,p,f) CreateDialogParamW(i,t,p,f,NULL)
#define W_CreateDialogIndirect(i,t,p,f) CreateDialogIndirectParamW(i,t,p,f,NULL)
#define W_CopyFile(s,d,f)       CopyFileW(s,d,f)

#else

HANDLE  WINAPI W_FindFirstFile      (PCWSTR pFileName, WIN32_FIND_DATAW * pfd);
BOOL    WINAPI W_FindNextFile       (HANDLE h, WIN32_FIND_DATAW * pfd);
DWORD   WINAPI W_GetFileAttributes  (PCWSTR pFileName);
BOOL    WINAPI W_SetFileAttributes  (PCWSTR pFileName, DWORD dwAttr);
DWORD   WINAPI W_GetFullPathName    (PCWSTR pFileName, DWORD cch, PWSTR pBuffer, PWSTR * ppFilePart);
HANDLE  WINAPI W_CreateFile         (PCWSTR pFileName, DWORD dwAccess, DWORD dwShare, SECURITY_ATTRIBUTES * psa,
                                     DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplate );
DWORD   WINAPI W_GetCurrentDirectory(DWORD cch, PWSTR pBuffer);
DWORD   WINAPI W_GetTempPath        (DWORD cch, PWSTR pBuffer);
UINT    WINAPI W_GetTempFileName    (PCWSTR pPathName, PCWSTR pPrefix, UINT uUnique, PWSTR pTempFileName);
BOOL    WINAPI W_DeleteFile         (PCWSTR pPathName);
BOOL    WINAPI W_CopyFile           (PCWSTR pExistingPathName, PCWSTR pNewPathName, BOOL bFailIfExists);

int     WINAPI W_GetWindowTextLength (HWND hwnd); // per docs, always large enough but not always exact
int     WINAPI W_GetWindowText       (HWND hwnd, PWSTR psz, int cch);

ATOM    WINAPI W_RegisterClass  (CONST WNDCLASSW * pWndClass);

// To see if window is really Unicode after creation, use the IsWindowUnicode() Win32 API
HWND    WINAPI W_CreateWindowEx (DWORD dwExStyle, PCWSTR pClassName, PCWSTR pWindowName,
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam);
#define W_CreateWindow(C,W,S,x,y,w,h,p,m,i,L) W_CreateWindowEx(0L,C,W,S,x,y,w,h,p,m,i,L)

HWND    WINAPI W_CreateDialogParam (HINSTANCE hInstance, PCWSTR pTemplateName, HWND hWndParent, 
                                    DLGPROC pDialogFunc, LPARAM dwInitParam);
#define W_CreateDialog(i,t,p,f) W_CreateDialogParam(i,t,p,f,NULL)

HWND    WINAPI W_CreateDialogIndirectParam (HINSTANCE hInstance, const DLGTEMPLATE * pTemplate,
                                            HWND hWndParent, DLGPROC pDialogFunc, LPARAM dwInitParam);
#define W_CreateDialogIndirect(i,t,p,f) W_CreateDialogIndirectParam(i,t,p,f,NULL)

#endif

//---------------------------------------------------------------
// Additional utility functions
//---------------------------------------------------------------

// These resource string load funcs never convert the string, even on Win9x
// -- you get resource data verbatim.
int     WINAPI W_LoadString   (HINSTANCE hinst, UINT id, PWSTR psz, int cch);
BSTR    WINAPI W_LoadBSTR     (HINSTANCE hInst, UINT id); // Free with SysFreeString
PWSTR   WINAPI W_LoadVSString (HINSTANCE hInst, UINT id); // Free with VSFree


#if !defined(NUMBER_OF)
#define NUMBER_OF(x) (sizeof(x) / sizeof((x)[0]))
#endif // !defined(NUMBER_OF)

// Returns length-prefixed non-null-terminated resource image for string
PCWSTR  WINAPI W_GetStringResourceImage (HINSTANCE hinst, UINT id);

// file/directory functions
int WINAPI W_Access(PCWSTR pPathName, int mode);                // _wchdir/_chdir
int WINAPI W_Rename(PCWSTR pOldPathName, PCWSTR pNewPathName);  // _wrename/_rename
int WINAPI W_Remove(PCWSTR pPathName);                          // _wremove/_remove
int WINAPI W_ChDir(PCWSTR pPathName);                           // _wchdir/_chdir
int WINAPI W_MkDir(PCWSTR pPathName);                           // _wmkdir/_mkdir
int WINAPI W_RmDir(PCWSTR pPathName);                           // _wrmdir/_rmdir

// Walk a file's path and get it's actual letter case
void    WINAPI W_GetActualFileCase      (PCWSTR pszName, PWSTR psz);

//---------------------------------------------------------
void inline W_ReplaceChar(PWSTR psz, WCHAR chOld, WCHAR chNew)
{
    if (psz)
    {
        WCHAR ch;
        while (0 != (ch = *psz))
        {
            if (ch == chOld)
                *psz = chNew;
            psz++;
        }
    }
}

//--------------

#ifndef UNIAPI_NOATL
void inline W_AddTrailingChar(ATL::CComBSTR &bstr, WCHAR wch)
{
  int len = bstr.Length();
  if(len > 0)
  {
    if(bstr[len - 1] != wch)
    {
      WCHAR wchBuf[2] = {wch, 0};
      bstr += wchBuf;
    }
  }
}

void inline W_AddTrailingSlash(ATL::CComBSTR &bstrFolder)
{
  W_AddTrailingChar(bstrFolder, L'\\');
}

void inline W_RemoveTrailingSlash(ATL::CComBSTR &bstrFolder)
{
  int len = bstrFolder.Length();
  if(len > 0)
  {
    if(bstrFolder[len - 1] == L'\\')
    {
	  ATL::CComBSTR bstrTmp(bstrFolder);
      bstrTmp[len - 1] = 0;
      bstrFolder = bstrTmp;
    }
  }
}

//---------------------------------------------------------

int inline BSTRcmp(BSTR bstr1, BSTR bstr2)
{
  if ((bstr1 == NULL) && (bstr2 == NULL))
    return 0;
  else if ((bstr1 == NULL) && (bstr2 != NULL))
    return -1;
  else if ((bstr1 != NULL) && (bstr2 == NULL))
    return 1;
  else 
    return wcscmp(bstr1, bstr2);
}

int inline BSTRicmp(BSTR bstr1, BSTR bstr2)
{
  if ((bstr1 == NULL) && (bstr2 == NULL))
    return 0;
  else if ((bstr1 == NULL) && (bstr2 != NULL))
    return -1;
  else if ((bstr1 != NULL) && (bstr2 == NULL))
    return 1;
  else 
    return _wcsicmp(bstr1, bstr2);
}

//------------------------------------------

// GetWindowText
BOOL W_GetWindowText(HWND hwnd, ATL::CComBSTR &bstrText);
BOOL WINAPI W_GetWindowTextTrimSpaces(HWND hwnd, ATL::CComBSTR &bstrText);

BOOL inline W_GetDlgItemText(HWND hwnd, UINT uiID, ATL::CComBSTR &bstrText)
{
  HWND hwndItem = GetDlgItem(hwnd, uiID);
  //_ASSERTE(hwndItem); // commented because we can't rely on _ASSERTE being defined

  return W_GetWindowText(hwndItem, bstrText);
}

BOOL inline W_GetDlgItemTextTrimSpaces(HWND hwnd, UINT uiID, ATL::CComBSTR &bstrText)
{
  HWND hwndItem = GetDlgItem(hwnd, uiID);
  //_ASSERTE(hwndItem); // commented because we can't rely on _ASSERTE being defined

  return W_GetWindowTextTrimSpaces(hwndItem, bstrText);
}

#endif // UNIAPI_NOATL

//----------------------------------
