#include <windows.h>
#include <winsock2.h>
#define _XBDM_
#include <xboxdbg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_XBOX_NAME 256
typedef struct _SCI {
    char szXboxName[MAX_XBOX_NAME];
    ULONG ulXboxIPAddr;
    DWORD dwConnectionTimeout;
    DWORD dwConversationTimeout;
    CRITICAL_SECTION csSharedConn;
    PDM_CONNECTION pdconShared;
    DWORD tidShared;
    union {
        struct {
            ULONG fAllowSharing:1,
                fBadSysTime:1,
                fGotTimeCorrection:1,
                fAddDiff:1,
                fCacheAddr:1,
                fSecureConnection:1,
                unused:26;
        };
        DWORD dwFlags;
    };
    ULARGE_INTEGER liTimeDiff;
} SCI;

extern SCI g_sci;
extern HINSTANCE hXboxdbg;

void EnsureWSAInited();
HRESULT HrOpenConnection(SCI *, PDM_CONNECTION *ppdcon, BOOL fRequireAccess);
HRESULT HrUseSharedConnection(SCI *, BOOL fShare);
HRESULT HrDoOpenSharedConnection(SCI *, PDM_CONNECTION *);
void DoCloseSharedConnection(SCI *, PDM_CONNECTION);
#define HrOpenSharedConnection(s) HrDoOpenSharedConnection(&g_sci, s)
#define CloseSharedConnection(s) DoCloseSharedConnection(&g_sci, s)
void TerminateConnection(PDM_CONNECTION);
HRESULT HrResolveNameIP(LPCSTR szName, struct sockaddr_in *psin);
void InitNotificationEvents(void);
void DestroyNotificationEvents(void);
void StopAllNotifications(void);
void CloseThreadConnection(void);
PDM_CONNECTION PdconWrapSocket(SOCKET);
HRESULT HrOneLineCmd(LPCSTR);
HRESULT HrDoOneLineCmd(PDM_CONNECTION, LPCSTR);
BOOL FUnpackDmml(LPCSTR, PDMN_MODLOAD, BOOL);
BOOL FUnpackDmsl(LPCSTR, PDMN_SECTIONLOAD, BOOL);
HRESULT HrOpenSecureConnection(SCI *, PDM_CONNECTION *, LPCSTR);
HRESULT HrAuthenticateUser(PDM_CONNECTION, PULARGE_INTEGER, PULARGE_INTEGER);
HRESULT HrSetupKeyExchange(PDM_CONNECTION, PULARGE_INTEGER);

/* Security */
void XBCCross(PULARGE_INTEGER pluKey, PULARGE_INTEGER pluData,
    PULARGE_INTEGER pluResult);
void XBCHashData(PULARGE_INTEGER pluHash, const BYTE *pb, ULONG cb);

/* File function implementations */
HRESULT HrSendFile(PDM_CONNECTION, LPCTSTR szLocalName, LPCSTR szRemoteName);
HRESULT HrReceiveFile(PDM_CONNECTION, LPCTSTR szLocalName, LPCSTR szRemoteName);
HRESULT HrGetFileAttributes(SCI *, LPCSTR szFileName, PDM_FILE_ATTRIBUTES);
HRESULT HrSetFileAttributes(SCI *, LPCSTR szFileName, PDM_FILE_ATTRIBUTES);
HRESULT HrMkdir(PDM_CONNECTION, LPCSTR szDirectoryName);
HRESULT HrRenameFile(PDM_CONNECTION, LPCSTR szOldName, LPCSTR szNewName);
HRESULT HrDeleteFile(PDM_CONNECTION, LPCSTR szFileName, BOOL fIsDirectory);
HRESULT HrOpenDir(SCI *, PDM_WALK_DIR *ppdmwd, LPCSTR szDirName,
    LPDWORD pdw);
HRESULT HrWalkDir(SCI *, PDM_WALK_DIR *ppdmwd, LPCSTR szDirName,
    PDM_FILE_ATTRIBUTES pdmfa);
HRESULT HrGetDriveList(PDM_CONNECTION pdcon, LPSTR rgchDrives, DWORD *pcDrives);
HRESULT HrGetDiskFreeSpace(SCI *psci, char *szDrive,
                           PULARGE_INTEGER pnFreeBytesAvailableToCaller,
                           PULARGE_INTEGER pnTotalNumberOfBytes,
                           PULARGE_INTEGER pnTotalNumberOfFreeBytes);
HRESULT HrGetXbeInfo(SCI *psci, LPCSTR szName, PDM_XBE pxbe);
HRESULT HrGetSystemTime(SCI *psci, LPSYSTEMTIME lpSysTime);
HRESULT HrGetNameOfXbox(PDM_CONNECTION, LPSTR szName, LPDWORD lpdwSize,
    BOOL fResolvable);
HRESULT HrGetAltAddress(SCI *psci, LPDWORD lpdw);
HRESULT HrScreenShot(SCI *psci, LPCSTR filename);
HRESULT HrIsSecurityEnabled(PDM_CONNECTION, LPBOOL pfEnabled);
HRESULT HrEnableSecurity(PDM_CONNECTION, BOOL fEnable);
HRESULT HrSetAdminPassword(PDM_CONNECTION, LPCSTR szPasswd);
HRESULT HrSetUserAccess(PDM_CONNECTION, LPCSTR szUserName, DWORD dwAccess);
HRESULT HrGetUserAccess(PDM_CONNECTION, LPCSTR szUserName, LPDWORD lpdwAccess);
HRESULT HrAddUser(PDM_CONNECTION, LPCSTR szUserName, DWORD dwAccess);
HRESULT HrRemoveUser(PDM_CONNECTION, LPCSTR szUserName);
HRESULT HrOpenUserList(SCI *, PDM_WALK_USERS *, LPDWORD);
HRESULT HrWalkUserList(SCI *, PDM_WALK_USERS *, PDM_USER);

// directory walking
typedef struct _DM_WALK_DIR *PDM_WALK_DIR;
DMHRAPI DmWalkDir(PDM_WALK_DIR *, LPCSTR szDir, PDM_FILE_ATTRIBUTES);

BOOL FGetLineData(LPCSTR szBuf, LPCSTR szKey, const char **ppchData);
const char *PchGetParam(LPCSTR szBuf, LPCSTR szKey, BOOL fNeedValue,
	BOOL fNoCommand);
BOOL FGetDwParamFromSz(LPCSTR sz, DWORD *pdwValue);
__inline BOOL FGetDwParam(LPCSTR szBuf, LPCSTR szKey, DWORD *pdwValue)
{
	return FGetDwParamFromSz(PchGetParam(szBuf, szKey, TRUE, TRUE), pdwValue);
}
BOOL FGetQwordParam(LPCSTR szBuf, LPCSTR szKey, ULARGE_INTEGER *plu);
BOOL FGetSzParam(LPCSTR szBuf, LPCSTR szKey, LPSTR szValue);
BOOL FGetNotificationData(LPCSTR sz, DWORD *pdwNotification, PVOID pv);
UINT FGetErrorStringResourceId(HRESULT hr);

struct _DM_CONNECTION {
	BYTE rgbBuf[1024];
	SOCKET s;
    union {
        struct {
            BOOL fAuthenticated:1,
                fAuthenticationAttempted:1,
                unused:30;
        };
        BOOL grbitFlags;
    };
	DWORD ibBuf;
	DWORD cbBuf;
    SCI *psci;
};

struct _DM_WALKDIR_FILE {
    struct _DM_WALKDIR_FILE *pwfNext;
    DM_FILE_ATTRIBUTES dmfa;
};

struct _DM_WALK_DIR {
    struct _DM_WALKDIR_FILE *pwfFirst;
};

struct _DM_WALKMOD_MODULE {
	struct _DM_WALKMOD_MODULE *pwmNext;
    DMN_MODLOAD dmml;
};

struct _DM_WALK_MODULES {
    struct _DM_WALKMOD_MODULE *pwmFirst;
};

struct _DM_WALKMOD_SECTION {
	struct _DM_WALKMOD_SECTION *pwsNext;
    DMN_SECTIONLOAD dmsl;
};

struct _DM_WALK_MODSECT {
    struct _DM_WALKMOD_SECTION *pwsFirst;
};

struct _DM_WALKPC {
    struct _DM_WALKPC *pwpcNext;
    DM_COUNTINFO dmci;
};

struct _DM_WALK_COUNTERS {
    struct _DM_WALKPC *pwpcFirst;
};

struct _DM_WALKUSER {
    struct _DM_WALKUSER *pwuNext;
    DM_USER dmu;
};

struct _DM_WALK_USERS {
    struct _DM_WALKUSER *pwuFirst;
};

#ifdef __cplusplus
}
#endif
