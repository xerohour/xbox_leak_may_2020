/*++

Copyright (c) Microsoft Corporation

Module Name:

    utils.h

Abstract:
    
    All kinds of utility methods.  These are sorted by namespaces
    and implemented in various files.

    namespace Utils - 

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    03-27-2001 : created

--*/
#ifndef __UTILS_H__
#define __UTILS_H__

//-----------------------------------------------------------
//  Useful Macros
//-----------------------------------------------------------
#ifndef ARRAYSIZE
#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif

//-----------------------------------------------------------
//  Utilities for using standard Windows Stuff:
//      Resources, Windows, Icons, Files, etc.
//-----------------------------------------------------------
namespace WindowUtils
{
    BOOL   SubstituteWindowText(HWND hWnd,...);
    int    rsprintf(LPSTR pBuffer, UINT uFormatResource,...);
    void   ReplaceWindowIcon(HWND hWnd, HICON hIcon);
    int    MessageBoxResource(HWND hWnd, UINT uTextResource, UINT uCaptionResource, UINT uType, ...);
    HANDLE CreateTempFile(char *pszFileName, bool fOpen=true);
    LPCSTR GetPreloadedString(UINT uResourceId);
    HWND   CreateWorkerWindow(HWND hWndParent);
    BOOL   IsMainShellProcess();
}

//-----------------------------------------------------------
//  Wait cursor
//-----------------------------------------------------------
class CWaitCursor
{
  public:
    CWaitCursor() : m_hOldCursor(NULL), m_fWait(false) {Set();}
    ~CWaitCursor(){Reset();}
    inline void Set() {if(!m_fWait) m_hOldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT)); m_fWait=true;}
    inline void Reset() {if(m_fWait) SetCursor(m_hOldCursor); m_fWait = false;}
    bool    m_fWait;
    HCURSOR m_hOldCursor;
};

//-----------------------------------------------------------
//  Formatting of various properties for display.
//-----------------------------------------------------------
namespace FormatUtils
{
    void FileSize(ULONGLONG ullFileSize, LPSTR pszOutputBuffer);
    void FileSizeBytes(ULONGLONG ullBytes, LPSTR pszOutputBuffer);
    void FileTime(const FILETIME *cpftTime, LPSTR pszOutputBuffer, DWORD dwFlags = DATE_SHORTDATE);
    void FileAttributes(DWORD dwAttributes, LPSTR pszOutputBuffer);
    HRESULT XboxErrorString(HRESULT hr, LPSTR szString, int nBufferMax);
}

//-----------------------------------------------------------
//  Utilities for working with COM.
//-----------------------------------------------------------
namespace ComUtils
{
    HRESULT QueryService(IUnknown* pUnknown, REFGUID guidService, REFIID riid, void **ppv);
    void DumpIID(REFIID riid);
    HRESULT QueryWindow(HWND *phWnd, IUnknown *pUnk);
}
#if NEVER
#define DEBUG_DUMP_IID(_riid_) ComUtils::DumpIID(_riid_)
#else
#define DEBUG_DUMP_IID(_riid_)
#endif

//-----------------------------------------------------------
//  Truly Miscellaneous Utilities.
//-----------------------------------------------------------
namespace Utils
{
    inline void CopyAtoW(LPWSTR pwszDest, LPCSTR pszSource) {while(*pszSource) *pwszDest++ = (WCHAR)(*pszSource++); *pwszDest = (WCHAR)*pszSource;}
    UINT CopyWtoA(LPSTR pszDest, LPCWSTR pwszSource);
    HRESULT GetXboxConnection(LPCSTR szXboxName, IXboxConnection **ppXboxConnection, DWORD dwMillisecondTimeout = 5000);
    BOOL VerifyXboxAlive(LPCSTR szXboxName);
    HRESULT ScreenCapture(HWND hWnd, LPCSTR szXboxName);
    HRESULT XboxErrorToWindowsError(HRESULT hr);
}

//-----------------------------------------------------------
//  Utilities For Manipulating DataObjects
//-----------------------------------------------------------
namespace DataObjUtil
{
    HRESULT SetHGLOBAL(IDataObject *pdtobj, UINT cf, HGLOBAL hGlobal);
    HRESULT SetDWORD(IDataObject *pdtobj, UINT cf, DWORD dw);
    HRESULT GetDWORD(IDataObject *pDataObject, UINT cf, DWORD *pdw);
    inline HRESULT SetPreferredDropEffect(IDataObject *pdtobj, DWORD dwEffect)
    {
        return SetDWORD(pdtobj, CF_PREFERREDDROPEFFECT, dwEffect);
    }
    inline HRESULT SetPerformedDropEffect(IDataObject *pdtobj, DWORD dwEffect)
    {
        return SetDWORD(pdtobj, CF_PERFORMEDDDROPEFFECT, dwEffect);
    }
    inline HRESULT SetPasteSucceeded(IDataObject *pdtobj, DWORD dwEffect)
    {
        return SetDWORD(pdtobj, CF_PASTESUCCEEDED, dwEffect);
    }
    inline HRESULT GetPreferredDropEffect(IDataObject *pdtobj, DWORD *pdwEffect)
    {
        return GetDWORD(pdtobj, CF_PREFERREDDROPEFFECT, pdwEffect);
    }
    inline HRESULT GetPerformedDropEffect(IDataObject *pdtobj, DWORD *pdwEffect)
    {
        return GetDWORD(pdtobj, CF_PERFORMEDDDROPEFFECT, pdwEffect);
    }
    inline HRESULT GetPasteSucceeded(IDataObject *pdtobj, DWORD *pdwEffect)
    {
        return GetDWORD(pdtobj, CF_PASTESUCCEEDED, pdwEffect);
    }
}

//-----------------------------------------------------------
//  Utilities For Displaying Dialogs 
//-----------------------------------------------------------
namespace Dialog
{

UINT
ConfirmReadOnlyMove(
    HWND hwndParent,
    LPCSTR pszFileName,
    bool fFolder
    );

UINT
ConfirmFolderReplace(
    HWND hWndParent,
    LPCSTR pszFileName
    );

UINT
ConfirmFileReplace(
    HWND hwndParent,
    LPCSTR pszFileName,
    WIN32_FILE_ATTRIBUTE_DATA *pTargetFileAttributes,
    WIN32_FILE_ATTRIBUTE_DATA *pSourceFileAttributes
    );


UINT
ConfirmDelete(
    HWND hWndParent,
    LPCSTR pszFileName,
    bool fFolder,
    bool fReadOnly
    );

UINT
ConfirmDeleteMultiple(
    HWND hWndParent,
    UINT uCount
    );

UINT
ConfirmDeleteFolder(
    HWND hWndParent,
    LPCSTR pszFileName
    );

UINT 
ConfirmSetAttributes(
    HWND hWndParent,
    DWORD dwSetAttributes,
    DWORD dwClearAttributes,
    BOOL fMultiItem
    );

UINT
PromptUserName(
    HWND hWndParent,
    LPSTR pszUserName,
    int iMaxCount
    );

UINT
PromptNewPassword(
    HWND hWndParent,
    LPSTR pszPassword,
    int iMaxCount
    );

}

#endif __UTILS_H__