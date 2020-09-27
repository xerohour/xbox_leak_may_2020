/*
 *
 * ixbconn.h
 *
 * C++ Interface for host-Xbox connections
 *
 */

#ifndef _IXBCONN
#define _IXBCONN

#include <objbase.h>

#define XBCONN_VERSION 4

#ifdef __cplusplus
extern "C++" {
#endif

#undef INTERFACE
#define INTERFACE IXboxConnection

DECLARE_INTERFACE_(IXboxConnection, IUnknown)
{
    // IUnknown (we don't support QI)
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR *ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef) (THIS) PURE;
    STDMETHOD_(ULONG, Release) (THIS) PURE;

    // IXboxConnection
    STDMETHOD(HrSetConnectionTimeout) (THIS_ DWORD dwConnectTimeout,
        DWORD dwConversationTimeout) PURE;
    STDMETHOD(HrUseSharedConnection) (THIS_ BOOL fShare) PURE;
    STDMETHOD(HrUseSecureConnection) (THIS_ LPCSTR szPasswd) PURE;
    STDMETHOD(HrSendCommand) (THIS_ LPCSTR szCommand, LPSTR szResponse,
        LPDWORD lpdwResponseSize) PURE;
    STDMETHOD(HrResolveXboxName) (THIS_ LPDWORD lpdwAddress) PURE;
    STDMETHOD(HrGetNameOfXbox) (THIS_ LPSTR szName, LPDWORD lpdwSize,
        BOOL fResolvable) PURE;
    STDMETHOD(HrSendFile) (THIS_ LPCSTR szLocalName, LPCSTR szRemoteName) PURE;
    STDMETHOD(HrReceiveFile) (THIS_ LPCSTR szLocalName, LPCSTR szRemoteName) PURE;
    STDMETHOD(HrGetFileAttributes) (THIS_ LPCSTR szFileName, PDM_FILE_ATTRIBUTES pfa) PURE;
    STDMETHOD(HrSetFileAttributes) (THIS_ LPCSTR szFileName, PDM_FILE_ATTRIBUTES pfa) PURE;
    STDMETHOD(HrMkdir) (THIS_ LPCSTR szDirectoryName) PURE;
    STDMETHOD(HrRenameFile) (THIS_ LPCSTR szOldName, LPCSTR szNewName) PURE;
    STDMETHOD(HrDeleteFile) (THIS_ LPCSTR szFileName, BOOL fIsDirectory) PURE;
    STDMETHOD(HrOpenDir) (THIS_ PDM_WALK_DIR *ppwd, LPCSTR szDir, LPDWORD pdw) PURE;
    STDMETHOD(HrWalkDir) (THIS_ PDM_WALK_DIR *ppwd, LPCSTR szDir, PDM_FILE_ATTRIBUTES pfa) PURE;
    STDMETHOD(HrCloseDir) (THIS_ PDM_WALK_DIR pwd) PURE;
    STDMETHOD(HrGetDriveList) (THIS_ LPSTR rgchDrives, LPDWORD pcDrives) PURE;
    STDMETHOD(HrGetDiskFreeSpace) (THIS_ LPSTR szDrive,
        PULARGE_INTEGER pnFreeBytesAvailableToCaller,
        PULARGE_INTEGER pnTotalNumberOfBytes,
        PULARGE_INTEGER pnTotalNumberOfFreeBytes) PURE;
    STDMETHOD(HrReboot)(THIS_ DWORD dwFlags, LPCSTR pszXbeName) PURE;
    STDMETHOD(HrGetXbeInfo)(THIS_ LPCSTR szName, PDM_XBE pxbe) PURE;
    STDMETHOD(HrGetSystemTime)(THIS_ LPSYSTEMTIME lpSysTime) PURE;
    STDMETHOD(HrGetAltAddress)(THIS_ LPDWORD lpdw) PURE;
    STDMETHOD(HrScreenShot)(THIS_ LPCSTR filename) PURE;
    STDMETHOD(HrEnableSecurity)(THIS_ BOOL fEnable) PURE;
    STDMETHOD(HrIsSecurityEnabled)(THIS_ LPBOOL pfEnabled) PURE;
    STDMETHOD(HrSetAdminPassword)(THIS_ LPCSTR szPasswd) PURE;
    STDMETHOD(HrSetUserAccess)(THIS_ LPCSTR szUserName, DWORD dwAccess) PURE;
    STDMETHOD(HrGetUserAccess)(THIS_ LPCSTR szUserName, LPDWORD lpdwAccess) PURE;
    STDMETHOD(HrAddUser)(THIS_ LPCSTR szUserName, DWORD dwAccess) PURE;
    STDMETHOD(HrRemoveUser)(THIS_ LPCSTR szUserName) PURE;
    STDMETHOD(HrOpenUserList)(THIS_ PDM_WALK_USERS *, LPDWORD) PURE;
    STDMETHOD(HrWalkUserList)(THIS_ PDM_WALK_USERS *, PDM_USER) PURE;
    STDMETHOD(HrCloseUserList)(THIS_ PDM_WALK_USERS) PURE;
};

#ifdef __cplusplus
}
extern "C" {
#endif

DMHRAPI DmGetXboxConnection(LPCSTR szXboxName, DWORD dwVersion,
    IXboxConnection **ppvObj);

#ifdef __cplusplus
}
#endif

#endif _IXBOXCONN
