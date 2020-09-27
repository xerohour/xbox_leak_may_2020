/*****************************************************************************\
    FILE: xboxapi.h

    DESCRIPTION:
        This file contains functions to perform the following 2 things:

    1. WININET WRAPPERS: Wininet APIs have either wierd bugs or bugs that come thru the APIs
    from the server.  It's also important to keep track of the perf impact
    of each call.  These wrappers solve these problems.

    2. XBOX STRs to PIDLs: These wrappers will take Xbox filenames and file paths
    that come in from the server and turn them into pidls.  These pidls contain
    both a unicode display string and the filename/path in wire bytes for future
    server requests.
\*****************************************************************************/


///////////////////////////////////////////////////////////////////////////////////////////
// 1. WININET WRAPPERS: Wininet APIs have either wierd bugs or bugs that come thru the APIs
// from the server.  It's also important to keep track of the perf impact
// of each call.  These wrappers solve these problems.
///////////////////////////////////////////////////////////////////////////////////////////
HRESULT XboxSetCurrentDirectoryWrap(HINTERNET hConnect, BOOL fAssertOnFailure, LPCWIRESTR pwXboxPath);
HRESULT XboxGetCurrentDirectoryWrap(HINTERNET hConnect, BOOL fAssertOnFailure, LPWIRESTR pwXboxPath, DWORD cchSize);
HRESULT XboxGetFileExWrap(HINTERNET hConnect, BOOL fAssertOnFailure, LPCWIRESTR pwXboxPath/*Src*/, LPCWSTR pwzFilePath/*Dest*/, BOOL fFailIfExists,
                       DWORD dwFlagsAndAttributes, DWORD dwFlags, DWORD_PTR dwContext);
HRESULT XboxPutFileExWrap(HINTERNET hConnect, BOOL fAssertOnFailure, LPCWSTR pwzFilePath/*Src*/, LPCWIRESTR pwXboxPath/*Dest*/, DWORD dwFlags, DWORD_PTR dwContext);
HRESULT XboxDeleteFileWrap(HINTERNET hConnect, BOOL fAssertOnFailure, LPCWIRESTR pwXboxFileName);
HRESULT XboxRenameFileWrap(HINTERNET hConnect, BOOL fAssertOnFailure, LPCWIRESTR pwXboxFileNameExisting, LPCWIRESTR pwXboxFileNameNew);
HRESULT XboxOpenFileWrap(HINTERNET hConnect, BOOL fAssertOnFailure, LPCWIRESTR pwXboxFileName, DWORD dwAccess, DWORD dwFlags, DWORD_PTR dwContext, HINTERNET * phFileHandle);
HRESULT XboxCreateDirectoryWrap(HINTERNET hConnect, BOOL fAssertOnFailure, LPCWIRESTR pwXboxPath);
HRESULT XboxRemoveDirectoryWrap(HINTERNET hConnect, BOOL fAssertOnFailure, LPCWIRESTR pwXboxPath);
HRESULT XboxCommandWrap(HINTERNET hConnect, BOOL fAssertOnFailure, BOOL fExpectResponse, DWORD dwFlags, LPCWIRESTR pszCommand, DWORD_PTR dwContext, HINTERNET *phXboxCommand);

HRESULT XboxDoesFileExist(HINTERNET hConnect, BOOL fAssertOnFailure, LPCWIRESTR pwFilterStr, LPXBOX_FIND_DATA pwfd, DWORD dwINetFlags);

HRESULT InternetOpenWrap(BOOL fAssertOnFailure, LPCTSTR pszAgent, DWORD dwAccessType, LPCTSTR pszProxy, LPCTSTR pszProxyBypass, DWORD dwFlags, HINTERNET * phFileHandle);
HRESULT InternetCloseHandleWrap(HINTERNET hInternet, BOOL fAssertOnFailure);
HRESULT InternetConnectWrap(HINTERNET hInternet, BOOL fAssertOnFailure, LPCTSTR pszServerName, INTERNET_PORT nServerPort,
                            LPCTSTR pszUserName, LPCTSTR pszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext, HINTERNET * phFileHandle);
HRESULT InternetOpenUrlWrap(HINTERNET hInternet, BOOL fAssertOnFailure, LPCTSTR pszUrl, LPCTSTR pszHeaders, DWORD dwHeadersLength, DWORD dwFlags, DWORD_PTR dwContext, HINTERNET * phFileHandle);
HRESULT InternetReadFileWrap(HINTERNET hFile, BOOL fAssertOnFailure, LPVOID pvBuffer, DWORD dwNumberOfBytesToRead, LPDWORD pdwNumberOfBytesRead);
HRESULT InternetWriteFileWrap(HINTERNET hFile, BOOL fAssertOnFailure, LPCVOID pvBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD pdwNumberOfBytesWritten);
HRESULT InternetFindNextFileWrap(HINTERNET hFind, BOOL fAssertOnFailure, LPVOID pvFindData);
HRESULT InternetGetLastResponseInfoWrap(BOOL fAssertOnFailure, LPDWORD pdwError, LPWIRESTR pwBuffer, LPDWORD pdwBufferLength);
HRESULT InternetGetLastResponseInfoDisplayWrap(BOOL fAssertOnFailure, LPDWORD pdwError, LPWSTR pwzBuffer, DWORD cchBufferSize);
INTERNET_STATUS_CALLBACK InternetSetStatusCallbackWrap(HINTERNET hInternet, BOOL fAssertOnFailure, INTERNET_STATUS_CALLBACK pfnInternetCallback);
HRESULT InternetCheckConnectionWrap(BOOL fAssertOnFailure, LPCTSTR pszUrl, DWORD dwFlags, DWORD dwReserved);
HRESULT InternetAttemptConnectWrap(BOOL fAssertOnFailure, DWORD dwReserved);
HRESULT InternetFindNextFileWrap(HINTERNET hConnect, BOOL fAssertOnFailure, LPXBOX_FIND_DATA pwfd);


///////////////////////////////////////////////////////////////////////////////////////////
// 2. XBOX STRs to PIDLs: These wrappers will take Xbox filenames and file paths
// that come in from the server and turn them into pidls.  These pidls contain
// both a unicode display string and the filename/path in wire bytes for future
// server requests.
///////////////////////////////////////////////////////////////////////////////////////////

HRESULT XboxSetCurrentDirectoryPidlWrap(HINTERNET hConnect, BOOL fAssertOnFailure, LPCITEMIDLIST pidlXboxPath, BOOL fAbsolute, BOOL fOnlyDirs);
HRESULT XboxGetCurrentDirectoryPidlWrap(HINTERNET hConnect, BOOL fAssertOnFailure, CWireEncoding * pwe, LPITEMIDLIST * ppidlXboxPath);
HRESULT XboxFindFirstFilePidlWrap(HINTERNET hConnect, BOOL fAssertOnFailure, CMultiLanguageCache * pmlc, CWireEncoding * pwe,
        LPCWIRESTR pwFilterStr, LPITEMIDLIST * ppidlXboxItem, DWORD dwINetFlags, DWORD_PTR dwContext, HINTERNET * phFindHandle);
HRESULT InternetFindNextFilePidlWrap(HINTERNET hConnect, BOOL fAssertOnFailure, CMultiLanguageCache * pmlc, CWireEncoding * pwe, LPITEMIDLIST * ppidlXboxItem);
HRESULT XboxRenameFilePidlWrap(HINTERNET hConnect, BOOL fAssertOnFailure, LPCITEMIDLIST pidlExisting, LPCITEMIDLIST pidlNew);
HRESULT XboxGetFileExPidlWrap(HINTERNET hConnect, BOOL fAssertOnFailure, LPCITEMIDLIST pidlXboxPath/*Src*/, LPCWSTR pwzFilePath/*Dest*/, BOOL fFailIfExists,
                       DWORD dwFlagsAndAttributes, DWORD dwFlags, DWORD_PTR dwContext);



