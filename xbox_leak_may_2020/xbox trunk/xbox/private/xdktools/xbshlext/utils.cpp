/*++

Copyright (c) Microsoft Corporation

Module Name:

    utils.cpp

Abstract:

    Miscellaneous Utility Methods

Environment:

    Windows 2000 and Later 
    User Mode

Revision History:
    
    03-27-2001 : created

--*/

#include "stdafx.h"
#include <stdio.h>

using namespace Utils;

STDAPI InitVariantFromBuffer(VARIANT *pvar, const void *pv, UINT cb)
{
    HRESULT hres;
    SAFEARRAY *psa = SafeArrayCreateVector(VT_UI1, 0, cb);   // create a one-dimensional safe array
    if (psa) 
    {
        memcpy(psa->pvData, pv, cb);

        memset(pvar, 0, sizeof(*pvar));  // VariantInit()
        pvar->vt = VT_ARRAY | VT_UI1;
        pvar->parray = psa;
        hres = S_OK;
    }
    else
        hres = E_OUTOFMEMORY;
    return hres;
}

STDAPI InitVariantFromString(VARIANT *pvar, LPCSTR szString)
{
   OLECHAR  oleszString[MAX_PATH];
   LPCSTR   szTemp = szString;
   LPOLESTR oleszTemp = oleszString; 

   //Copy to OLECHAR
   while(szTemp) *oleszTemp++ = (OLECHAR)*szTemp++;
   
   //Copy variant
   pvar->vt = VT_BSTR;
   pvar->bstrVal = SysAllocString(oleszString);
   if(pvar->bstrVal)
   {
        return S_OK;
   }
   
   return E_OUTOFMEMORY;
}


void FormatUtils::FileSize(ULONGLONG ullFileSize, LPSTR pszOutputBuffer)
/*++
  Routine Description:
    Give the nFileSizeHigh and nFileSizeLow of file formats the size as a readable string in the
    caller's buffer.
  Arguments:
    ullFileSize        - size of the file in bytes
    pszOutputBuffer    - pointer to user buffer to receive formated size - 
                         assumed to be large enough (25 characters?)
--*/
{
    UINT   uFormatResource = IDS_PRELOAD_FILESIZE_FORMAT_BYTES;
    double dbFileSize      = (double)ullFileSize;

    //
    //  Depending on the size the result in bytes, KB, MB, or GB.
    //  
    while(dbFileSize >= 1024 && uFormatResource!=IDS_PRELOAD_FILESIZE_FORMAT_GB)
    {
        dbFileSize /= 1024;
        uFormatResource++;
    }
    // Cannot use wsprintf, as it does not format floating point.
    sprintf(pszOutputBuffer, WindowUtils::GetPreloadedString(uFormatResource), dbFileSize);
}

static void FormatByteStringRecurse(ULONGLONG ullBytes, LPSTR *pszNextChar)
{
    if(ullBytes > 1000)
    {
        FormatByteStringRecurse(ullBytes/1000, pszNextChar);
        *pszNextChar += wsprintfA(
                            *pszNextChar,
                            WindowUtils::GetPreloadedString(IDS_PRELOAD_FILEBYTESIZE_FORMAT1),
                            (ULONG)(ullBytes%1000));
        return;
    }
    *pszNextChar += wsprintfA(*pszNextChar, "%d", (ULONG)ullBytes);
}

void FormatUtils::FileSizeBytes(ULONGLONG ullBytes, LPSTR pszOutputBuffer)
/*++
  Routine Description:
    Take a long long and convert it to a string indicating the count of bytes.
    The key feature that wsprintf cannot do is create a comma separate number.
--*/
{
    FormatByteStringRecurse(ullBytes, &pszOutputBuffer);
    strcpy(pszOutputBuffer, WindowUtils::GetPreloadedString(IDS_PRELOAD_FILEBYTESIZE_FORMAT2));
}

void FormatUtils::FileTime(const FILETIME *cpftTime, LPSTR pszOutputBuffer, DWORD dwFlags)
/*++
  Routine Description:
    Uses the WIN32 international API to format file times.  Takes a file time and produces a string
    with the user's selected localized date format immediately followed by the user's selected
    localized time format.
  
  Arguments:
    cpftTime         - filetime (modified or created)
    pszOutputBuffer  - buffer to receive string.  Should be of length MAX_PATH.
    dwFlags          - DATE_SHORTDATE or DATE_LONGDATE (Actually, anything that GetDateFormat accepts)

--*/
{
    SYSTEMTIME sysTime;
    int iCharCount;
    if(FileTimeToSystemTime(cpftTime, &sysTime))
    {
        //
        // Convert System Time to Local Time
        //
        TIME_ZONE_INFORMATION tzi;
        SYSTEMTIME sysTimeLocal;
        if(GetTimeZoneInformation(&tzi) != TIME_ZONE_ID_UNKNOWN)
        {
	        if(SystemTimeToTzSpecificLocalTime(&tzi, &sysTime, &sysTimeLocal))
            {
		        sysTime = sysTimeLocal;
            }
        }

        //
        //  Get date, and time
        //
        pszOutputBuffer[0] = '\0';
        iCharCount = GetDateFormatA(LOCALE_USER_DEFAULT, dwFlags, &sysTime, NULL, pszOutputBuffer, MAX_PATH);
        if(iCharCount)
        {
            pszOutputBuffer[iCharCount-1] = ' ';
            pszOutputBuffer[iCharCount] = '\0';
            GetTimeFormatA(LOCALE_USER_DEFAULT, 0, &sysTime, NULL, pszOutputBuffer+iCharCount, MAX_PATH-iCharCount);
        }
    }
}

void FormatUtils::FileAttributes(DWORD dwAttributes, LPSTR pszOutputBuffer)
{
    UINT uIndex;
    uIndex = 0;
    if(dwAttributes&FILE_ATTRIBUTE_READONLY)
    {
        pszOutputBuffer[uIndex++] = 'R';
    }
    if(dwAttributes&FILE_ATTRIBUTE_HIDDEN)
    {
        pszOutputBuffer[uIndex++] = 'H';
    }
    pszOutputBuffer[uIndex] = '\0';
}

UINT FGetErrorStringResourceId(HRESULT hr)
{
	switch(hr) {
	case E_FAIL:
	case E_UNEXPECTED:
		return IDC_E_UNEXPECTED;
	case E_INVALIDARG:
		return IDC_E_INVALIDARG;
    default:
        if(FAILED(hr) && HRESULT_FACILITY(hr) == FACILITY_XBDM)
            return 0x8000 | (hr & 0xfff);
        break;
	}
    return 0;
}
HRESULT FormatUtils::XboxErrorString(HRESULT hr, LPSTR lpBuffer, int nBufferMax)
{
    UINT    resourceId;
    int     len=TRUE;
    HRESULT hrRes = XBDM_NOERR;

    //Parameter check.
    if( (!lpBuffer) || (nBufferMax <= 0))
    {
        return E_INVALIDARG;
    }
    
    //Make sure it is an XBDM facility, otherwise use FormatMessage.
    if(FACILITY_XBDM == HRESULT_FACILITY(hr))
    {

        //Get the resource ID
        resourceId = FGetErrorStringResourceId(hr);
        if(0==resourceId) 
        {
            hrRes = XBDM_NOERRORSTRING;
            resourceId = IDC_XBDM_NOERRORSTRING;
        }

        //Load the string
        len = LoadStringA(_Module.GetModuleInstance(), resourceId, lpBuffer, nBufferMax);
        
    } else
    {
       if(!FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, lpBuffer, nBufferMax, NULL))
       {
           len = LoadStringA(_Module.GetModuleInstance(), IDC_E_UNEXPECTED, lpBuffer, nBufferMax);
       }
    }
    if(0==len)
    {
       hrRes = HRESULT_FROM_WIN32(GetLastError());
    }
    return hrRes;
}

BOOL WindowUtils::SubstituteWindowText(HWND hWnd,...)
/*++
  Routine Description:
    Reads the window text of any Window and treats it as the format string.
    Then calls _vstprintf to format the string and puts it back in the
    Window.

  Arguments:
    nFileSizeHigh       - high DWORD of filesize
    nFileSizeLow        - low DWORD of filesize
    ptszOutputBuffer    - pointer to user buffer to receive formated size
    ccBufferSize        - count in characters of ptszOutputBuffer
--*/
{
    va_list vl;
    char szFormat[256];
    char szResult[1024];
    BOOL fResult;

    va_start(vl, hWnd); 
    fResult = GetWindowTextA(hWnd, szFormat, ARRAYSIZE(szFormat));
    if(fResult)
    {
        wvsprintfA(szResult, szFormat, vl);
        fResult = SetWindowTextA(hWnd, szResult);
    }
    va_end(vl);
    return fResult;
}

int  WindowUtils::rsprintf(LPSTR pBuffer, UINT uFormatResource,...)
{
    int i=0;
    va_list vl;
    char szFormat[512];
    if(LoadStringA(_Module.GetModuleInstance(), uFormatResource, szFormat, sizeof(szFormat)))
    {
        va_start(vl, uFormatResource); 
        i = wvsprintfA(pBuffer, szFormat, vl);
        va_end(vl);
    }
    return i;
}


void WindowUtils::ReplaceWindowIcon(HWND hWnd, HICON hIcon)
{
    if (hIcon)
    {
        hIcon = (HICON)SendMessage(hWnd, STM_SETICON, (WPARAM)hIcon, 0L);
        if (hIcon)
            DestroyIcon(hIcon);
    }
}

int WindowUtils::MessageBoxResource(HWND hWnd, UINT uTextResource, UINT uCaptionResource, UINT uType, ...)
{
    va_list vl;
    char MessageText[512];
    char MessageTextTemplate[255];
    char MessageCaption[255];
    if(LoadStringA(_Module.GetModuleInstance(), uTextResource,  MessageTextTemplate, ARRAYSIZE(MessageTextTemplate)))
    {
        va_start(vl, uType);
        wvsprintfA(MessageText, MessageTextTemplate, vl);
        va_end(vl);
    }
    LoadStringA(_Module.GetModuleInstance(), uCaptionResource,  MessageCaption, ARRAYSIZE(MessageCaption));
    return MessageBoxA(hWnd, MessageText, MessageCaption, uType);
}


HANDLE WindowUtils::CreateTempFile(char *pszFileName, bool fOpen)
/*++
  Routine Description:
   Creates a temporary file, opens it, and returns the handle and the
   name.

   This is useful, because usually when you generate a temporary filename
   you also want to open it.  Furthermore, the WIN32 API for generating
   temporary filenames are just too flexible.
--*/
{
    char szTempPath[MAX_PATH];
    HANDLE hFile = INVALID_HANDLE_VALUE;
    if(GetTempPathA(sizeof(szTempPath), szTempPath))
    {
        if(GetTempFileNameA(szTempPath, "xse", 0, pszFileName))
        {
            if(fOpen)
            {
                hFile = CreateFileA(
                    pszFileName,
                    GENERIC_READ|GENERIC_WRITE,
                    0,
                    NULL,
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL|
                    FILE_ATTRIBUTE_TEMPORARY,
                    NULL
                    );
            } else
            {
                hFile = 0;
            }
        }
    }
    return hFile;
}

HWND WindowUtils::CreateWorkerWindow(HWND hWndParent)
{
    WNDCLASSA wc = {0};
    if(!GetClassInfoA(_Module.GetModuleInstance(), "XboxWorker", &wc))
    {
        wc.lpfnWndProc = DefWindowProcA;
        wc.cbWndExtra = sizeof(void *);
        wc.hInstance = _Module.GetModuleInstance();
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.lpszClassName = "XboxWorker";
        RegisterClassA(&wc);
    }

    HWND hwnd = CreateWindowExA(0, "XboxWorker", NULL, 0, 0, 0, 0, 0, NULL, NULL, _Module.GetModuleInstance(), NULL);
    if(hwnd)
    {
        SetWindowLongPtr(hwnd, 0, NULL);
    }
    return hwnd;
}

BOOL WindowUtils::IsMainShellProcess()
{
    DWORD dwProcessId;
    HWND hWndDesktop = GetDesktopWindow();
    GetWindowThreadProcessId(hWndDesktop, &dwProcessId);
    return dwProcessId == GetCurrentProcessId();
}

class CPreloadedStrings
{
  public:
    CPreloadedStrings();
    ~CPreloadedStrings();
    LPCSTR GetString(UINT uResourceId);
  private:
    LPSTR m_Strings[IDS_NEXT_PRELOADED_STRING-IDS_FIRST_PRELOADED_STRING];
};

CPreloadedStrings::CPreloadedStrings()
{
    memset(m_Strings, 0, sizeof(m_Strings));
}

CPreloadedStrings::~CPreloadedStrings()
{
    UINT uIndex;
    for(uIndex = 0; uIndex < (IDS_NEXT_PRELOADED_STRING-IDS_FIRST_PRELOADED_STRING); uIndex++)
    {
        delete m_Strings[uIndex];
    }
}

LPCSTR CPreloadedStrings::GetString(UINT uResourceId)
/*++
  Routine Description:
    Returns a constant pointer to a resource string.

    Implements a lazy load-once model.
--*/
{
    _ASSERTE(uResourceId >= IDS_FIRST_PRELOADED_STRING); 
    _ASSERTE(uResourceId < IDS_NEXT_PRELOADED_STRING); 

    UINT uIndex;
    uIndex = uResourceId-IDS_FIRST_PRELOADED_STRING;
    
    if(NULL == m_Strings[uIndex])
    {
        char szStringBuffer[512];
        if(LoadStringA(_Module.GetModuleInstance(), uResourceId, szStringBuffer, sizeof(szStringBuffer)))
        {
            m_Strings[uIndex] = new char[strlen(szStringBuffer)+1];
            if(m_Strings[uIndex])
            {
                strcpy(m_Strings[uIndex], szStringBuffer);
            }
        }
    }
    
    return m_Strings[uIndex];
}
CPreloadedStrings g_PreloadedStrings;


LPCSTR WindowUtils::GetPreloadedString(UINT uResourceId)
/*++
  Routine Description:

    Gets a preloaded string.
    Uses CPreloadedStrings to do the work.  The main point of CPreloadedStrings
    is so that the d'tor can cleanup automagically when the module is unloaded,
    so that we do not leak.

  Arguments:

    uResourceID - ID of string to preload. Must be in preloaded string range.
--*/
{
    return g_PreloadedStrings.GetString(uResourceId);
}


UINT Utils::CopyWtoA(LPSTR pszDest, LPCWSTR pwszSource)
/*++
  Routine Description:
    Copys a UNICODE string to ASCII (not MBCS).  It substitutes
    # for every unrecognized character.  It returns the count of
    such characters.
--*/
{
    UINT uSubstituteCount = 0;
    while(*pwszSource)
    {
        if(*pwszSource&0xff00)
        {
            uSubstituteCount++;
            pwszSource++;
            *pszDest++ = '#';
        } else
        {
            *pszDest++ = (char)(*pwszSource++);
        }
    }
    *pszDest = '\0';
    return uSubstituteCount;
}

HRESULT Utils::GetXboxConnection(LPCSTR szXboxName, IXboxConnection **ppXboxConnection, DWORD dwMillisecondTimeout)
{
    HRESULT hr;
    hr = DmGetXboxConnection(szXboxName,XBCONN_VERSION,ppXboxConnection);
    if(SUCCEEDED(hr))
    {
        hr = (*ppXboxConnection)->HrSetConnectionTimeout(dwMillisecondTimeout, 0);
        if(FAILED(hr))
        {
            (*ppXboxConnection)->Release();
            *ppXboxConnection = NULL;
        }
    }
    return hr;
}

BOOL Utils::VerifyXboxAlive(LPCSTR szXboxName)
{
    BOOL fResult = FALSE;
    HRESULT hr;
    IXboxConnection *pConnection;
    hr = DmGetXboxConnection(szXboxName,XBCONN_VERSION, &pConnection);
    if(SUCCEEDED(hr))
    {
        char szConsoleName[20];
        DWORD dwLen = sizeof(szConsoleName);
        hr = pConnection->HrGetNameOfXbox(szConsoleName, &dwLen, FALSE);
        pConnection->Release();
        if((XBDM_CANNOTCONNECT != hr) && (XBDM_CONNECTIONLOST != hr))
        {
            fResult = TRUE;
        }
    }
    return fResult;
}

HRESULT Utils::ScreenCapture(HWND hWnd, LPCSTR szXboxName)
{
  CWaitCursor waitCursor;
  char szImagePath[MAX_PATH];
  char szImageFile[MAX_PATH];
  UINT uImageNumber = 1;
  UINT uPathLen;
  DWORD dwError;
  HRESULT hr;

  //Get the mypictures folder
  SHGetFolderPathA(hWnd, CSIDL_MYPICTURES|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT,szImagePath);
  uPathLen = strlen(szImagePath);
           
  //Terminate the path with '\\'
  if(szImagePath[uPathLen-1] != '\\')
  {
    szImagePath[uPathLen++] = '\\';
    szImagePath[uPathLen]='\0';
  }
           
  do
  {
    HANDLE hFile;
    wsprintfA(szImageFile, "%s%s-image%d.bmp", szImagePath, szXboxName, uImageNumber++);
    hFile = CreateFileA(szImageFile, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

    if(INVALID_HANDLE_VALUE != hFile)
    {
        dwError = ERROR_SUCCESS;
        CloseHandle(hFile);
        IXboxConnection *pConnection;
        hr = Utils::GetXboxConnection(szXboxName, &pConnection);
        if(SUCCEEDED(hr))
        {
            hr = pConnection->HrScreenShot(szImageFile);
            if(SUCCEEDED(hr))
            {
                SHELLEXECUTEINFOA shellExecuteInfo;
                memset(&shellExecuteInfo, 0, sizeof(SHELLEXECUTEINFOA));
                shellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
                shellExecuteInfo.hwnd = hWnd;
                shellExecuteInfo.lpVerb = "open";
                shellExecuteInfo.lpFile = szImageFile;
                shellExecuteInfo.nShow = SW_SHOWNORMAL;
                ShellExecuteExA(&shellExecuteInfo);
             }
        }
        if(FAILED(hr))
        {
            char szError[60];
            FormatUtils::XboxErrorString(hr, szError, sizeof(szError));
            WindowUtils::MessageBoxResource(hWnd, IDS_COULDNT_GET_SCREEN_SHOT, IDS_GENERIC_CAPTION, MB_OK|MB_ICONSTOP, szXboxName, szError);
            DeleteFileA(szImageFile);
        }
        break;
    }
    dwError = GetLastError();
  } while(ERROR_FILE_EXISTS == dwError  || ERROR_ALREADY_EXISTS == dwError);
  
  if(ERROR_SUCCESS != dwError)
  { 
    char szError[60];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, 0, szError, sizeof(szError), NULL);
    WindowUtils::MessageBoxResource(hWnd, IDS_COULDNT_GET_SCREEN_SHOT, IDS_GENERIC_CAPTION, MB_OK|MB_ICONSTOP, szXboxName, szError);
    hr = HRESULT_FROM_WIN32(dwError);
   }
   return hr;
}

HRESULT Utils::XboxErrorToWindowsError(HRESULT hr)
{
    switch(hr)
    {
        case XBDM_MAXCONNECT:
            return HRESULT_FROM_WIN32(ERROR_REQ_NOT_ACCEP);
        case XBDM_BADFILENAME:
            return HRESULT_FROM_WIN32(ERROR_INVALID_NAME);
        case XBDM_NOSUCHFILE:
            return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        case XBDM_CANNOTACCESS:
            return E_ACCESSDENIED;
        case XBDM_CANNOTCREATE:
            return HRESULT_FROM_WIN32(ERROR_CANNOT_MAKE);
        case XBDM_DEVICEFULL:
            return HRESULT_FROM_WIN32(ERROR_DISK_FULL);
        case XBDM_ALREADYEXISTS:
            return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
        case XBDM_DIRNOTEMPTY:
            return HRESULT_FROM_WIN32(ERROR_DIR_NOT_EMPTY);
        case XBDM_CANNOTCONNECT:
        case XBDM_CONNECTIONLOST:
            return HRESULT_FROM_WIN32(ERROR_CONNECTION_UNAVAIL);
        // The rest of these are either not applicable to the shell extension,
        // Or should never be returned, or are simply too generic to worry about.
        case XBDM_NOMODULE:
        case XBDM_MEMUNMAPPED:
        case XBDM_NOTHREAD:
        case XBDM_CLOCKNOTSET:
        case XBDM_INVALIDCMD:
        case XBDM_NOTSTOPPED:
        case XBDM_MUSTCOPY:
        case XBDM_NOTDEBUGGABLE:
        case XBDM_BADCOUNTTYPE:
        case XBDM_COUNTUNAVAILABLE:
        case XBDM_NOTLOCKED:
        case XBDM_KEYXCHG:
        case XBDM_FILEERROR:
        case XBDM_ENDOFLIST:
        case XBDM_BUFFER_TOO_SMALL:
        case XBDM_NOTXBEFILE:
        case XBDM_MEMSETINCOMPLETE:
        case XBDM_NOXBOXNAME:
        case XBDM_NOERRORSTRING:
        case XBDM_CONNECTED:
        case XBDM_MULTIRESPONSE:
        case XBDM_BINRESPONSE:
        case XBDM_READYFORBIN:
        case XBDM_DEDICATED:
        default:
            return E_FAIL;

    }
    return E_FAIL;
}

HRESULT ComUtils::QueryService(IUnknown* pUnknown, REFGUID guidService, REFIID riid, void **ppv)
/*++
 Routine Description:
   Little Utility to get a COM service.
--*/
{
    HRESULT hres;
    IServiceProvider *pServiceProvider = NULL;

    *ppv = NULL;
    if(!pUnknown)
        return E_FAIL;

    hres = pUnknown->QueryInterface(IID_IServiceProvider, (void **)&pServiceProvider);

    if(SUCCEEDED(hres))
    {
        hres = pServiceProvider->QueryService(guidService, riid, ppv);
        pServiceProvider->Release();
    }
    return hres;
}


void ComUtils::DumpIID(REFIID riid)
{
    if(riid == IID_IShellFolder)
    {
        OutputDebugStringA("IID_IShellFolder");
    } else if (riid == IID_IShellFolder2)
    {
        OutputDebugStringA("IID_IShellFolder2");
    } else if (riid == IID_IPersistFolder)
    {
        OutputDebugStringA("IID_IPersistFolder");
    } else if (riid == IID_IPersistFolder2)
    {
        OutputDebugStringA("IID_IPersistFolder2");
    } else if (riid == IID_IDataObject)
    {
        OutputDebugStringA("IID_IDataObject");
    } else if (riid == IID_IContextMenu)
    {
        OutputDebugStringA("IID_IContextMenu");
    } else if (riid == IID_IDropTarget)
    {
        OutputDebugStringA("IID_IDropTarget");
    } else if (riid == IID_IExtractIcon)
    {
        OutputDebugStringA("IID_IExtractIcon");
    } else if (riid == IID_IShellView)
    {
        OutputDebugStringA("IID_IShellView");
    } else if (riid == IID_IShellView2)
    {
        OutputDebugStringA("IID_IShellView2");
    } else if (riid == IID_IShellFolderView)
    {
        OutputDebugStringA("IID_IShellFolderView");
    } else if (riid == IID_IUnknown)
    {
        OutputDebugStringA("IID_IUnknown");
    } else if (riid == IID_IDropSource)
    {
        OutputDebugStringA("IID_IDropSource");
    } else if (riid == IID_IObjectWithSite)
    {
        OutputDebugStringA("IID_IObjectWithSite");
    } else if (riid == IID_IShellLink)
    {
        OutputDebugStringA("IID_IShellLink");
    } else if (riid == IID_IDelegateFolder)
    {
        OutputDebugStringA("IID_IDelegateFolder");
    } else
    {
        WCHAR wszBuffer[60];
        StringFromGUID2(riid, wszBuffer, ARRAYSIZE(wszBuffer));
        OutputDebugStringW(wszBuffer);
    }
}  


HRESULT ComUtils::QueryWindow(HWND *phWnd, IUnknown *pUnk)
/*++
  Given an IUnknown interface to an object it just may support another interface that allows you to retrieve its
  Window.  For example, a progress dialog.  It is worth a try.  Callers should have a fall back plan
  though since this isn't officially documented.  The shell code tried IOleWindow, IShellView and IInternetSecurityMgr.
  This routine doesn't bother trying IInternetSecurityMgr.  It probably doesn't need to try IShellView either.

  Unlike the shell's IUnknown_GetWindow, this routine preserves the value of *phWnd on failure.  Why?  This way
  the caller may set it to the default choice, and not bother checking the error code.  If this routine fails,
  then they will just get their own default.
--*/
{
    HRESULT hr = E_INVALIDARG;
    if (pUnk) 
    {
        HWND hOrgWnd = *phWnd;
        *phWnd = NULL;

        IOleWindow* pOleWindow;
        IShellView* pShellView;

        hr = pUnk->QueryInterface(IID_PPV_ARG(IOleWindow, &pOleWindow));
        if (SUCCEEDED(hr))
        {
            hr = pOleWindow->GetWindow(phWnd);
            pOleWindow->Release();
        }
        else if (SUCCEEDED(hr = pUnk->QueryInterface(IID_PPV_ARG(IShellView, &pShellView))))
        {
            hr = pShellView->GetWindow(phWnd);
            pShellView->Release();
        }

        // If this failed, restore the original.
        if(FAILED(hr))
        {
            *phWnd = hOrgWnd;
        }
    }
    return hr;
}

HRESULT DataObjUtil::SetHGLOBAL(IDataObject *pdtobj, UINT cf, HGLOBAL hGlobal)
{
    FORMATETC fmte = {(CLIPFORMAT) cf, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    STGMEDIUM medium;

    medium.tymed = TYMED_HGLOBAL;
    medium.hGlobal = hGlobal;
    medium.pUnkForRelease = NULL;

    // give the data object ownership of ths
    return pdtobj->SetData(&fmte, &medium, TRUE);
}

HRESULT DataObjUtil::SetDWORD(IDataObject *pdtobj, UINT cf, DWORD dw)
{
    HRESULT hr = E_OUTOFMEMORY;
    DWORD *pdw = (DWORD *)GlobalAlloc(GPTR, sizeof(DWORD));
    if (pdw)
    {
        *pdw = dw;
        hr = SetHGLOBAL(pdtobj, cf, pdw);

        if (FAILED(hr))
            GlobalFree((HGLOBAL)pdw);
    }
    return hr;
}


HRESULT DataObjUtil::GetDWORD(IDataObject *pDataObject, UINT cf, DWORD *pdw)
{
    FORMATETC formatEtc = {(CLIPFORMAT) cf, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    STGMEDIUM stgMedium;
    *pdw = 0;    
    HRESULT hr = pDataObject->GetData(&formatEtc, &stgMedium);
    if(SUCCEEDED(hr))
    {
        *pdw = *((DWORD *)GlobalLock(stgMedium.hGlobal));
        ReleaseStgMedium(&stgMedium);
    }
    return hr;
}


