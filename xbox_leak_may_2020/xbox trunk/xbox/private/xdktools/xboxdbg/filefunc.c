/*
 *
 * filefunc.c
 *
 * File functions
 *
 */

#ifdef _IXBCONN
#define XBAPI(n) CXboxConnection::Hr##n
#define XBIMP(n) (::n)
#define PSCI (&m_sci)
#else
#define XBAPI(n) Dm##n
#define XBIMP(n) (n)
#define PSCI (&g_sci)
#endif

HRESULT XBAPI(ResolveXboxName)(LPDWORD lpdwAddr)
{
	struct sockaddr_in sin;
	HRESULT hr;
    int cb;

    if (!lpdwAddr)
        return E_INVALIDARG;

#ifndef _IXBCONN
	hr = DmSetXboxName(NULL);
	if(FAILED(hr))
		return hr;
#endif
    /* If we already have an open shared connection to this box, we can just
     * pluck the IP address out of there */
    hr = E_FAIL;
    if(PSCI->pdconShared) {
        EnterCriticalSection(&PSCI->csSharedConn);
        cb = sizeof sin;
        if(PSCI->pdconShared && 0 == getpeername(PSCI->pdconShared->s,
                (struct sockaddr *)&sin, &cb))
            hr = XBDM_NOERR;
        LeaveCriticalSection(&PSCI->csSharedConn);
    }
    if(FAILED(hr))
	    hr = HrResolveNameIP(PSCI->szXboxName, &sin);
	if(FAILED(hr))
		return hr;
	*lpdwAddr = ntohl(sin.sin_addr.s_addr);
	return XBDM_NOERR;
}

HRESULT XBAPI(GetNameOfXbox)(LPSTR szName, LPDWORD lpdwSize,
    BOOL fResolvable)
{
    PDM_CONNECTION s;
    HRESULT hr;

    hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrGetNameOfXbox)(s, szName, lpdwSize, fResolvable);
        CloseSharedConnection(s);
    }
    return hr;
}

HRESULT XBAPI(SetConnectionTimeout)(DWORD dwConnectTimeout,
    DWORD dwConversationTimeout)
{
    PSCI->dwConnectionTimeout = dwConnectTimeout;
    PSCI->dwConversationTimeout = dwConversationTimeout;
    return XBDM_NOERR;
}

HRESULT XBAPI(SendFile)(LPCSTR szLocalName, LPCSTR szRemoteName)
{
    PDM_CONNECTION s;
    HRESULT hr;

	hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrSendFile)(s, szLocalName, szRemoteName);
        CloseSharedConnection(s);
    }

    return hr;
}

HRESULT XBAPI(ReceiveFile)(LPCSTR szLocalName, LPCSTR szRemoteName)
{
    PDM_CONNECTION s;
    HRESULT hr;

    hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrReceiveFile)(s, szLocalName, szRemoteName);
        CloseSharedConnection(s);
    }

    return hr;
}

HRESULT XBAPI(GetFileAttributes)(LPCSTR szFile, PDM_FILE_ATTRIBUTES pdm)
{
    return XBIMP(HrGetFileAttributes)(PSCI, szFile, pdm);
}

HRESULT XBAPI(SetFileAttributes)(LPCSTR szFile, PDM_FILE_ATTRIBUTES pdm)
{
    return XBIMP(HrSetFileAttributes)(PSCI, szFile, pdm);
}

HRESULT XBAPI(Mkdir)(LPCSTR szName)
{
    PDM_CONNECTION s;
    HRESULT hr;

	hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrMkdir)(s, szName);
        CloseSharedConnection(s);
    }

    return hr;
}

HRESULT XBAPI(RenameFile)(LPCSTR szOldName, LPCSTR szNewName)
{
    PDM_CONNECTION s;
    HRESULT hr;

    hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrRenameFile)(s, szOldName, szNewName);
        CloseSharedConnection(s);
    }

    return hr;
}

HRESULT XBAPI(DeleteFile)(LPCSTR szFileName, BOOL fIsDirectory)
{
    PDM_CONNECTION s;
    HRESULT hr;

	hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrDeleteFile)(s, szFileName, fIsDirectory);
        CloseSharedConnection(s);
    }

    return hr;
}

HRESULT XBAPI(WalkDir)(PDM_WALK_DIR *ppdmwd, LPCSTR szDirName,
    PDM_FILE_ATTRIBUTES pdmfa)
{
    return XBIMP(HrWalkDir)(PSCI, ppdmwd, szDirName, pdmfa);
}

HRESULT XBAPI(GetDriveList)(LPSTR rgchDrives, DWORD *pcDrives)
{
    PDM_CONNECTION s;
    HRESULT hr;

	hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrGetDriveList)(s, rgchDrives, pcDrives);
        CloseSharedConnection(s);
    }

    return hr;
}

HRESULT XBAPI(GetDiskFreeSpace)(char *szDrive,
                           PULARGE_INTEGER pnFreeBytesAvailableToCaller,
                           PULARGE_INTEGER pnTotalNumberOfBytes,
                           PULARGE_INTEGER pnTotalNumberOfFreeBytes)
{
    return XBIMP(HrGetDiskFreeSpace)(PSCI, szDrive,
        pnFreeBytesAvailableToCaller, pnTotalNumberOfBytes,
        pnTotalNumberOfFreeBytes);
}

HRESULT XBAPI(GetXbeInfo)(LPCSTR szName, PDM_XBE pxbe)
{
    return XBIMP(HrGetXbeInfo)(PSCI, szName, pxbe);
}

HRESULT XBAPI(GetSystemTime)(LPSYSTEMTIME lpSysTime)
{
    if (!lpSysTime)
        return E_INVALIDARG;

    return XBIMP(HrGetSystemTime)(PSCI, lpSysTime);
}

HRESULT XBAPI(GetAltAddress)(LPDWORD lpdw)
{
    return XBIMP(HrGetAltAddress)(PSCI, lpdw);
}

HRESULT XBAPI(ScreenShot)(LPCSTR filename)
{
    return XBIMP(HrScreenShot)(PSCI, filename);
}

HRESULT XBAPI(IsSecurityEnabled)(LPBOOL pfEnabled)
{
    HRESULT hr;
    PDM_CONNECTION s;

    hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrIsSecurityEnabled)(s, pfEnabled);
        CloseSharedConnection(s);
    }
    return hr;
}

HRESULT XBAPI(EnableSecurity)(BOOL fEnable)
{
    HRESULT hr;
    PDM_CONNECTION s;

    hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrEnableSecurity)(s, fEnable);
        CloseSharedConnection(s);
    }
    return hr;
}

HRESULT XBAPI(SetAdminPassword)(LPCSTR szPasswd)
{
    HRESULT hr;
    PDM_CONNECTION s;

    hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrSetAdminPassword)(s, szPasswd);
        CloseSharedConnection(s);
    }
    return hr;
}

HRESULT XBAPI(AddUser)(LPCSTR szUserName, DWORD dwAccess)
{
    HRESULT hr;
    PDM_CONNECTION s;

    hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrAddUser)(s, szUserName, dwAccess);
        CloseSharedConnection(s);
    }
    return hr;
}

HRESULT XBAPI(RemoveUser)(LPCSTR szUserName)
{
    HRESULT hr;
    PDM_CONNECTION s;

    hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrRemoveUser)(s, szUserName);
        CloseSharedConnection(s);
    }
    return hr;
}

HRESULT XBAPI(GetUserAccess)(LPCSTR szUserName, LPDWORD lpdwAccess)
{
    HRESULT hr;
    PDM_CONNECTION s;

    hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrGetUserAccess)(s, szUserName, lpdwAccess);
        CloseSharedConnection(s);
    }
    return hr;
}

HRESULT XBAPI(SetUserAccess)(LPCSTR szUserName, DWORD dwAccess)
{
    HRESULT hr;
    PDM_CONNECTION s;

    hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = XBIMP(HrSetUserAccess)(s, szUserName, dwAccess);
        CloseSharedConnection(s);
    }
    return hr;
}

HRESULT XBAPI(WalkUserList)(PDM_WALK_USERS *ppdmwu, PDM_USER pdu)
{
    return XBIMP(HrWalkUserList)(PSCI, ppdmwu, pdu);
}
