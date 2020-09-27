//-----------------------------------------------------------------------------
// Microsoft Visual C++
//
// Microsoft Confidential
// Copyright 1994 - 2000 Microsoft Corporation. All Rights Reserved.
//
// Utils.cpp : Implementation of useful functions
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "rsrc.h"
#include "utils.h"
#include "path2.h"
#include "locale.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static bool bPlatformInited = false;
static DWORD dwPlatform;

//-----------------------------------------------------------------------------
// Useful directory separator check
//-----------------------------------------------------------------------------
static inline BOOL IsDirSep(wchar_t ch)
{
	return (ch == L'\\' || ch == L'/');
}

static inline BOOL IsDirSep(char ch)
{
	return (ch == '\\' || ch == '/');
}

//-----------------------------------------------------------------------------
// Returns the registered folder holding our new Ui dlls
//-----------------------------------------------------------------------------
void UtilGetProjUIFolder(CString& strProjItemDir)
{
	strProjItemDir.Empty();
	if (!UtilGetModuleFileName(_Module.GetModuleInstance( ), strProjItemDir))
		return;

	int nLoc = strProjItemDir.ReverseFind(_T('\\'));
	VSASSERT(nLoc >= 0, "At least one slash required in devenv.exe's location!");
	strProjItemDir = strProjItemDir.Left(nLoc);
}

//-----------------------------------------------------------------------------
//	Creates the directory strDirName. Unlike CreateDirectory() this function will
//	create all the intermediate directories along the way. Note you MUST provide 
//	the full path!
//-----------------------------------------------------------------------------
BOOL UtilCreateDirectory(LPCTSTR strDirName)
{
	// Extract the root name of the volume
	TCHAR szRoot[_MAX_PATH+1];
	memset(szRoot, 0, _MAX_PATH+1);
	_tcsncpy(szRoot, strDirName, _MAX_PATH);

	for (LPTSTR lpsz = szRoot; *lpsz != _T('\0'); lpsz++)
	{	// Find first double slash and stop
		if (IsDirSep(lpsz[0]) && IsDirSep(lpsz[1]))
			break;
	}
	if (*lpsz != _T('\0'))
	{	// It is a UNC name, find second slash past '\\'
		lpsz += 2;
		while (*lpsz != _T('\0') && (!IsDirSep(*lpsz)))
			lpsz = _tcsinc(lpsz);
		if (*lpsz != _T('\0'))
			lpsz = _tcsinc(lpsz);
		while (*lpsz != _T('\0') && (!IsDirSep(*lpsz)))
			lpsz = _tcsinc(lpsz);
		// Terminate after the UNC root (ie. '\\server\share\')
		if (*lpsz != _T('\0'))
			lpsz[1] = _T('\0');
	}
	else
	{
		// Not a UNC, look for just the first slash
		lpsz = szRoot;
		while (*lpsz != _T('\0') && (!IsDirSep(*lpsz)))
			lpsz = _tcsinc(lpsz);
		// Terminate it just after root (ie. 'x:\')
		if (*lpsz != _T('\0'))
			lpsz[1] = _T('\0');
	}

	// Figure out offset past the volume info and set buffer ptr there.
	TCHAR buffer[_MAX_PATH];
	_tcsncpy(buffer, strDirName, _MAX_PATH);
	buffer[_MAX_PATH-1] = _T('0');
	TCHAR* pPath = &buffer[_tcsclen(szRoot)];

	SECURITY_ATTRIBUTES sa; 
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa. bInheritHandle = TRUE;

	TCHAR* pNextToken = _tcstok( pPath, _T("\\/"));
	while(pNextToken)
	{
		// Append token to path so far
		_tcscat(szRoot, pNextToken);
		
		// If this subdir hasn't been created, do so.
		if(::GetFileAttributes(szRoot) == -1)
		{
				if(!::CreateDirectory(szRoot, &sa))
				{	// Failed create of subdir
					return FALSE;
				}
		}

		// Add in separator and get next part of path
		_tcscat(szRoot, _T("\\"));
		pNextToken = _tcstok( NULL, _T("\\/"));
	}
	
	// Made it all the way through.
	return TRUE;
}

//-----------------------------------------------------------------------------
//	Creates the directory strDirName. Unlike CreateDirectory() this function will
//	create all the intermediate directories along the way. Note you MUST provide 
//	the full path!
//-----------------------------------------------------------------------------
BOOL UtilCreateDirectoryW(const wchar_t* strDirName)
{
	// Extract the root name of the volume
	wchar_t szRoot[_MAX_PATH+1];
	memset(szRoot, 0, sizeof(wchar_t)*(_MAX_PATH+1) );
	wcsncpy(szRoot, strDirName, _MAX_PATH);
	szRoot[_MAX_PATH] = 0; // wcsncpy does not terminate for you.

	for (wchar_t* lpsz = szRoot; *lpsz != L'\0'; lpsz++)
	{	// Find first double slash and stop
		if (IsDirSep(lpsz[0]) && IsDirSep(lpsz[1]))
			break;
	}
	if (*lpsz != L'\0')
	{	// It is a UNC name, find second slash past '\\'
		lpsz += 2;
		while (*lpsz != L'\0' && (!IsDirSep(*lpsz)))
			lpsz++;
		if (*lpsz != L'\0')
			lpsz++;
		while (*lpsz != L'\0' && (!IsDirSep(*lpsz)))
			lpsz++;
		// Terminate after the UNC root (ie. '\\server\share\')
		if (*lpsz != L'\0')
			lpsz[1] = L'\0';
	}
	else
	{
		// Not a UNC, look for just the first slash
		lpsz = szRoot;
		while (*lpsz != L'\0' && (!IsDirSep(*lpsz)))
			lpsz++;
		// Terminate it just after root (ie. 'x:\')
		if (*lpsz != L'\0')
			lpsz[1] = L'\0';
	}

	// Figure out offset past the volume info and set buffer ptr there.
	wchar_t buffer[_MAX_PATH];
	wcsncpy(buffer, strDirName, _MAX_PATH);
	buffer[_MAX_PATH-1] = 0;
	wchar_t* pPath = &buffer[wcslen(szRoot)];

	SECURITY_ATTRIBUTES sa; 
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	wchar_t* pNextToken = wcstok( pPath, L"\\/");
	while(pNextToken)
	{
		// Append token to path so far
		wcscat(szRoot, pNextToken);
		
		// If this subdir hasn't been created, do so.
		if(::GetFileAttributesW(szRoot) == -1)
		{
				if(!::CreateDirectoryW(szRoot, &sa))
				{	// Failed create of subdir
					return FALSE;
				}
		}

		// Add in separator and get next part of path
		wcscat(szRoot, L"\\");
		pNextToken = wcstok( NULL, L"\\/");
	}
	
	// Made it all the way through.
	return TRUE;
}


//-----------------------------------------------------------------------------
// Handle SendMessage as SendMessageW on NT/Win2k
//-----------------------------------------------------------------------------
LRESULT UtilSendMessageW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	if( !bPlatformInited )
	{
		bPlatformInited = true;
		// what OS are we running on?
		OSVERSIONINFO osver;
		memset( &osver, 0, sizeof(osver) );
		osver.dwOSVersionInfoSize = sizeof(osver);
		GetVersionEx( &osver );
		dwPlatform = osver.dwPlatformId;
	}
	// if we're running on NT, use Unicode 
	if( dwPlatform == VER_PLATFORM_WIN32_NT )
		return SendMessageW( hWnd, Msg, wParam, lParam );
	// otherwise, Win9x, we can't use Unicode
	else
		return SendMessage( hWnd, Msg, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Handle SendMessage as SendMessageW on NT/Win2k where the LPARAM is a string
// (hence ANSI on Win9x and Unicode on NT/Win2k...)
//-----------------------------------------------------------------------------
LRESULT UtilSendMessageWString( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	if( !bPlatformInited )
	{
		bPlatformInited = true;
		// what OS are we running on?
		OSVERSIONINFO osver;
		memset( &osver, 0, sizeof(osver) );
		osver.dwOSVersionInfoSize = sizeof(osver);
		GetVersionEx( &osver );
		dwPlatform = osver.dwPlatformId;
	}
	// if we're running on NT, use Unicode 
	if( dwPlatform == VER_PLATFORM_WIN32_NT )
	{
		return SendMessageW( hWnd, Msg, wParam, lParam );
	}
	// otherwise, Win9x, we can't use Unicode
	else
	{
		// convert the string to ANSI
		USES_CONVERSION;
		char* szString = W2A( (wchar_t*)lParam );
		return SendMessageA( hWnd, Msg, wParam, (LPARAM)szString );
	}
}


//-----------------------------------------------------------------------------
// Sets the error information in the shell. 
//-----------------------------------------------------------------------------
HRESULT UtilSetErrorInfo(HRESULT hrError, LPCOLESTR pszDescription)
{
    HRESULT     hr = S_OK;

    if (hrError == S_OK)
    {
        hr = ::SetErrorInfo(0, NULL);
        return hr;
    }

    CComPtr<ICreateErrorInfo>   srpCreateErrInfo;
    CComQIPtr<IErrorInfo>       srpErrInfo;

    hr = ::CreateErrorInfo(&srpCreateErrInfo);
    hr = srpCreateErrInfo->SetDescription((LPOLESTR)pszDescription);
    hr = srpCreateErrInfo->SetGUID(GUID_NULL);
    hr = srpCreateErrInfo->SetHelpContext(0);
    hr = srpCreateErrInfo->SetHelpFile(NULL);
    hr = srpCreateErrInfo->SetSource(L"");
	srpErrInfo = srpCreateErrInfo;
    hr = ::SetErrorInfo(0, srpErrInfo);

	return hr;
}

// End of utils.cpp
