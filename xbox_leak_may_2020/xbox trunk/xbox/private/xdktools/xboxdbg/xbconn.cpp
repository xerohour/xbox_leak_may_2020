/*
 *
 * xbconn.cpp
 *
 * IXboxConnection implementation
 *
 */

#include "precomp.h"
#undef HrOpenSharedConnection
#undef CloseSharedConnection
#include <ixbconn.h>

#undef PURE
#undef THIS_
#undef THIS

class CXboxConnection : public IXboxConnection
{
public:
    STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR *ppvObj);
    STDMETHOD_(ULONG, AddRef) (void);
    STDMETHOD_(ULONG, Release) (void);

    // IXboxConnection
    STDMETHOD(HrSetConnectionTimeout) (DWORD dwConnectTimeout,
        DWORD dwConversationTimeout);
    STDMETHOD(HrUseSharedConnection) (BOOL fShare);
    STDMETHOD(HrUseSecureConnection) (LPCSTR szPasswd);
    STDMETHOD(HrSendCommand) (LPCSTR szCommand, LPSTR szResponse,
        LPDWORD lpdwResponseSize);
    STDMETHOD(HrResolveXboxName) (LPDWORD lpdwAddress);
    STDMETHOD(HrGetNameOfXbox) (LPSTR szName, LPDWORD lpdwSize,
        BOOL fResolvable);
    STDMETHOD(HrSendFile) (LPCSTR szLocalName, LPCSTR szRemoteName);
    STDMETHOD(HrReceiveFile) (LPCSTR szLocalName, LPCSTR szRemoteName);
    STDMETHOD(HrGetFileAttributes) (LPCSTR szFileName, PDM_FILE_ATTRIBUTES pfa);
    STDMETHOD(HrSetFileAttributes) (LPCSTR szFileName, PDM_FILE_ATTRIBUTES pfa);
    STDMETHOD(HrMkdir) (LPCSTR szDirectoryName);
    STDMETHOD(HrRenameFile) (LPCSTR szOldName, LPCSTR szNewName);
    STDMETHOD(HrDeleteFile) (LPCSTR szFileName, BOOL fIsDirectory);
    STDMETHOD(HrOpenDir) (PDM_WALK_DIR *ppwd, LPCSTR szDir, LPDWORD pdw);
    STDMETHOD(HrWalkDir) (PDM_WALK_DIR *ppwd, LPCSTR szDir, PDM_FILE_ATTRIBUTES pfa);
    STDMETHOD(HrCloseDir) (PDM_WALK_DIR pwd);
    STDMETHOD(HrGetDriveList) (LPSTR rgchDrives, LPDWORD pcDrives);
    STDMETHOD(HrGetDiskFreeSpace) (LPSTR szDrive,
        PULARGE_INTEGER pnFreeBytesAvailableToCaller,
        PULARGE_INTEGER pnTotalNumberOfBytes,
        PULARGE_INTEGER pnTotalNumberOfFreeBytes);
    STDMETHOD(HrReboot)(DWORD dwFlags, LPCSTR pszXbeName);
    STDMETHOD(HrGetXbeInfo)(LPCSTR szName, PDM_XBE pxbe);
    STDMETHOD(HrGetSystemTime)(LPSYSTEMTIME lpSysTime);
    STDMETHOD(HrGetAltAddress)(LPDWORD lpdw);
    STDMETHOD(HrScreenShot)(LPCSTR filename);
    STDMETHOD(HrEnableSecurity)(BOOL fEnable);
    STDMETHOD(HrIsSecurityEnabled)(LPBOOL pfEnabled);
    STDMETHOD(HrSetAdminPassword)(LPCSTR szPasswd);
    STDMETHOD(HrSetUserAccess)(LPCSTR szUserName, DWORD dwAccess);
    STDMETHOD(HrGetUserAccess)(LPCSTR szUserName, LPDWORD lpdwAccess);
    STDMETHOD(HrAddUser)(LPCSTR szUserName, DWORD dwAccess);
    STDMETHOD(HrRemoveUser)(LPCSTR szUserName);
    STDMETHOD(HrOpenUserList)(PDM_WALK_USERS *, LPDWORD);
    STDMETHOD(HrWalkUserList)(PDM_WALK_USERS *, PDM_USER);
    STDMETHOD(HrCloseUserList)(PDM_WALK_USERS);

    CXboxConnection(LPCSTR szName);
    ~CXboxConnection();

    HRESULT HrOpenSharedConnection(PDM_CONNECTION *ppdcon)
    {
        return HrDoOpenSharedConnection(&m_sci, ppdcon);
    }

    void CloseSharedConnection(PDM_CONNECTION pdcon)
    {
        DoCloseSharedConnection(&m_sci, pdcon);
    }

    long m_cRef;
    SCI m_sci;

    void *operator new(size_t cb)
    {
        return LocalAlloc(LMEM_FIXED, cb);
    }

    void operator delete(void *pv)
    {
        if(pv)
            LocalFree(pv);
    }
};

CXboxConnection::CXboxConnection(LPCSTR szName)
{
    memset(&m_sci, 0, sizeof m_sci);
    strcpy(m_sci.szXboxName, szName);
    InitializeCriticalSection(&m_sci.csSharedConn);
    m_sci.fCacheAddr = TRUE;
    m_cRef = 1;
}

CXboxConnection::~CXboxConnection()
{
    HrUseSharedConnection(FALSE);
    DeleteCriticalSection(&m_sci.csSharedConn);
}

// import function definitions
#include "filefunc.c"

STDMETHODIMP CXboxConnection::QueryInterface(REFIID riid, LPVOID FAR *ppvObj)
{
    /* We don't support QI */
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CXboxConnection::AddRef(void)
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CXboxConnection::Release(void)
{
    int cRef = InterlockedDecrement(&m_cRef);
    if(0 == cRef)
        delete this;
    return cRef;
}

STDMETHODIMP CXboxConnection::HrUseSharedConnection(BOOL fShare)
{
    return ::HrUseSharedConnection(&m_sci, fShare);
}

STDMETHODIMP CXboxConnection::HrUseSecureConnection(LPCSTR szPasswd)
{
    return ::HrOpenSecureConnection(&m_sci, NULL, szPasswd);
}

STDMETHODIMP CXboxConnection::HrSendCommand(LPCSTR szCommand, LPSTR szResponse,
    LPDWORD lpdwResponseSize)
{
    HRESULT hr;
    PDM_CONNECTION s;

    hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = DmSendCommand(s, szCommand, szResponse, lpdwResponseSize);
        CloseSharedConnection(s);
    }
    return hr;
}

STDMETHODIMP CXboxConnection::HrOpenDir(PDM_WALK_DIR *ppwd, LPCSTR szDirName,
    LPDWORD pdw)
{
    return ::HrOpenDir(&m_sci, ppwd, szDirName, pdw);
}

STDMETHODIMP CXboxConnection::HrCloseDir(PDM_WALK_DIR pwd)
{
    return DmCloseDir(pwd);
}

STDMETHODIMP CXboxConnection::HrOpenUserList(PDM_WALK_USERS *ppwu, LPDWORD pdw)
{
    return ::HrOpenUserList(&m_sci, ppwu, pdw);
}

STDMETHODIMP CXboxConnection::HrCloseUserList(PDM_WALK_USERS pwu)
{
    return DmCloseUserList(pwu);
}

HRESULT CXboxConnection::HrReboot(DWORD dwFlags, LPCSTR pszXbeName)
{
    PDM_CONNECTION s;
    char *szEmpty = "";
	char *szWait;
	char *szWarm = dwFlags & DMBOOT_WARM ? " WARM" : szEmpty;
    
	char sz[512];

    if(dwFlags & DMBOOT_STOP)
        szWait = " STOP";
    else if(dwFlags & DMBOOT_WAIT)
        szWait = " WAIT";
    else
        szWait = szEmpty;

    if(!pszXbeName)
    {
        char *szNDebug = dwFlags & DMBOOT_NODEBUG ? " NODEBUG" : szEmpty;
	    sprintf(sz, "REBOOT%s%s%s", szWait, szWarm, szNDebug);    
    } else
    {
        char *szDebug = dwFlags & DMBOOT_NODEBUG ? szEmpty : " DEBUG";
        sprintf(sz, "magicboot title=%s%s", pszXbeName,szDebug);
    }
    
    HRESULT hr = HrOpenSharedConnection(&s);
    if(SUCCEEDED(hr)) {
        hr = HrDoOneLineCmd(s, sz);
        CloseSharedConnection(s);
    }
    return hr;
}

HRESULT DmGetXboxConnection(LPCSTR sz, DWORD dwVersion, IXboxConnection **ppvObj)
{
    CXboxConnection *pxc;
    PDM_CONNECTION pdcon;
    HRESULT hr;
    struct sockaddr_in sin;

    if(dwVersion != XBCONN_VERSION)
        return E_INVALIDARG;

    pxc = new CXboxConnection(sz);
    if(!pxc)
        return E_OUTOFMEMORY;
    *ppvObj = pxc;
    /* Make sure we can get an IP address */
    hr = HrResolveNameIP(pxc->m_sci.szXboxName, &sin);
    if(SUCCEEDED(hr))
        pxc->m_sci.ulXboxIPAddr = sin.sin_addr.s_addr;
    else
        delete pxc;
    return hr;
}

