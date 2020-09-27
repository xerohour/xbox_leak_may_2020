#ifndef _DMP_INC
#define _DMP_INC
#define WINSOCK_API_LINKAGE
#define _KERNEL32_
#define _USER32_

#define _XBDM_
#include "ntos.h"
#include "ldr.h"
#include "mi.h"
#include <xbeimage.h>
#include <xprofp.h>
#include "dm.h"
#include <ntrtl.h>
#define _XAPI_
#include <nturtl.h>
#undef _XAPI_
#include <windows.h>
#include <winsockx.h>
#include <stdio.h>
#include <xbdm.h>
#include <xbdmp.h>
#include "dmserv.h"
#include "dmnotify.h"
#include "dmcount.h"

#define MAX_OBJ_PATH 320

/* Route our pool calls into our own pool manager */
#undef ExAllocatePool
#undef ExAllocatePoolWithTag
#undef ExFreePool
#define ExAllocatePool(n, cb) DmAllocatePool(cb)
#define ExAllocatePoolWithTag(n, cb, tag) DmAllocatePoolWithTag(cb, tag)
#define ExFreePool DmFreePool

/* ini file */
typedef struct _INF {
    HANDLE h;
    BYTE *pbBuf;
    int cbBuf;
    int cbUsed;
} INF;

void WriteIniData(INF *, const BYTE *pb, int cb);
void WriteIniSz(INF *, LPCSTR);
void WriteIniNotifiers(INF *);
void WriteIniSecurity(INF *);
void WriteIniFile(void);
BOOL FStartDebugService(void);
BOOL FInitThreadDebugData(PETHREAD);
ULONG CallOnStack(PVOID, PVOID, PVOID);
void HandleCommand(int, LPCSTR);
void InitNotifications();
BOOL FNotifySock(SOCKET s);
BOOL FConvertToNotifySock(SOCKET);
BOOL PrintSockLine(SOCKET s, LPCSTR sz);
BOOL ReadSockLine(SOCKET s, char *sz, int cch);
void FillSzFromWz(char *sz, int cch, WCHAR *wz, int cwch);
PLDR_DATA_TABLE_ENTRY PldteGetModule(LPCSTR sz, BOOL fMatchExt);
void InitPool(void);
void DmExFreePool(PVOID);
void InitBreakpoints(void);
BOOL FAddBreakpoint(BYTE *);
BOOL FRemoveBreakpoint(BYTE *);
void DisableBreakpoint(PVOID);
void RemoveAllBreakpoints(void);
void EnableBreakpointsInRange(PBYTE, PBYTE, BOOL);
void DisableBreakpointsInRange(PBYTE, PBYTE);
BOOL FIsBreakpoint(BYTE *);
BOOL FGetNextBreakpoint(BYTE **);
BOOL FGetMemory(BYTE *pbAddr, BYTE *pbInstr);
BOOL FSetMemory(BYTE *pbAddr, BYTE bInstr);
DWORD NotifyComponents(DWORD, DWORD);
BOOL FGetNotifySz(DWORD dwNotification, DWORD dwParam, LPSTR sz);
void GetModLoadSz(LPSTR, PDMN_MODLOAD);
void GetSectLoadSz(LPSTR, PDMN_SECTIONLOAD);
void DmslFromXsh(PXBEIMAGE_SECTION, PDMN_SECTIONLOAD);
DWORD DwXmhFlags(PDMN_MODLOAD);
void DoStopGo(BOOL);
void DoContinue(PETHREAD, BOOL);
void PrepareToStop(void);
void PrepareToReboot(void);
void DisableAPIC(PHAL_SHUTDOWN_REGISTRATION);
void SetupHalt(DWORD);
BOOL FStopAtException(void);
void ResumeAfterBreakpoint(PCONTEXT);
void ExceptionsToKd(void);
void FixupBistroImage(void);
void SyncHwbps(void);
void SetDreg(int ireg, PVOID pv, BYTE bRw, BYTE bLen, BOOLEAN fEnable);
ULONG __fastcall DwExchangeDr6(ULONG);
void QueueDfn(PDFN);
void ProcessDfns(void);
BOOL FBreakTrace(PCONTEXT);
BOOL FMatchDataBreak(ULONG, PDMN_DATABREAK);
void DmpThreadStartup(PKSTART_ROUTINE, PVOID);
void SetupFuncCall(void);
BOOL FResolveImport(PVOID pvBase, PIMAGE_EXPORT_DIRECTORY ped, ULONG cbExports,
    LPCSTR szName, PVOID *ppvRet);
void CallDxtEntry(PVOID, BOOL *);
BOOL FFixupXbdm(PVOID pvXbdmBase, PDMINIT);
void StopServ(void);
void InitServ(void);
void InitLoader(void);
void InitCounters(void);
void InitSecurity(void);
void EnsurePersistentSockets(void);
BOOL FNotifyAt(ULONG, USHORT, DWORD, BOOL);
BOOL FNotifyAtCmd(SOCKET, LPCSTR, BOOL);
ULONG UlAddrFromSz(LPCSTR);
HRESULT HrAddUserCommand(LPCSTR, LPSTR, DWORD, PDM_CMDCONT);
DWORD DmplFromSz(LPCSTR);
void FillAccessPrivSz(LPSTR, DWORD);
void FillUserInfoSz(LPSTR, LPCSTR szName, DWORD dwPrivileges,
    PULARGE_INTEGER pluPasswd);
DWORD DmplAuthenticateUser(LPCSTR szUserName, PULARGE_INTEGER pluNonce,
    PULARGE_INTEGER pluResponse, BOOL *pfKeyXchg);
void RemoveAllUsers(void);
BOOL FFileNameToObName(LPCSTR sz, OCHAR *osz, int cchMax);
BOOL FTitleExists(LPCSTR szDir, LPCSTR szTitle, LPCSTR szCmdLine, BOOL fMapIt);
VOID FObNameToFileName(IN POBJECT_STRING objectName, OUT LPSTR pszFileName, int cchMax);
VOID MapDebugDrive(OCHAR, PCOSTR);
HRESULT HrFromStatus(NTSTATUS st, HRESULT hrDefault);
NTSTATUS
FCreateFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN LPCSTR FileName,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG CreateDisposition,
    IN ULONG CreateOptions
    );

// useful line handling stuff
const char *PchGetParam(LPCSTR szLine, LPCSTR szKey, BOOL fNeedValue);
BOOL FGetSzParam(LPCSTR szLine, LPCSTR szKey, LPSTR szBuf, int cchBuf);
BOOL FGetDwParam(LPCSTR szLine, LPCSTR szKey, DWORD *pdw);
BOOL FGetQwordParam(LPCSTR szLine, LPCSTR szKey, PULARGE_INTEGER plu);
BOOL FGetNamedDwParam(LPCSTR szLine, LPCSTR szKey, DWORD *pdw, LPSTR szResp);
int CchOfWord(LPCSTR sz);
BOOL FEqualRgch(LPCSTR, LPCSTR, int);

// connection security
ULONG DmplOfConnection(PDM_CMDCONT pdmcc);
#define FConnectionPermission(pdmcc, dmpl) \
    ((DmplOfConnection(pdmcc) & (dmpl)) != 0)

// crypt
void XBCCross(PULARGE_INTEGER pluKey, PULARGE_INTEGER pluData,
    PULARGE_INTEGER pluResult);
void XBCHashData(PULARGE_INTEGER pluHash, const BYTE *pb, ULONG cb);

void RewindDmws(PDM_WALK_MODSECT);
unsigned long Crc32(unsigned long InitialCrc, const void *Buffer, unsigned long Bytes);
HRESULT FlashKernelImage(PVOID ImageBuffer, SIZE_T ImageSize, LPSTR szResp,
    DWORD cchResp, BOOL IgnoreVersionChecking);

NTSTATUS DmCapControl(ULONG Action, ULONG Param);
HRESULT HrUpdateSystemFile(LPCSTR sz, LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc);

extern PKPROCESS pprocSystem;
extern BOOL fAllowKd;
extern DWORD g_grbitStopOn;
extern DWORD dwExecState;
extern DMINIT g_dmi;
extern DMGD g_dmgd;
extern DMXAPI g_dmxapi;
extern char rgchTitleDir[MAX_OBJ_PATH+1];
extern char rgchTitleName[64];
extern char rgchDbgName[256];
extern BOOL fBootWait;
extern char *pszCmdLine;
extern KEVENT kevtServ;
extern PULONG g_pulFrameCount;
extern BOOL g_fDebugging;
extern ULONG g_ulDbgIP;
extern BOOL g_fLockLevel;
extern ULARGE_INTEGER g_luBoxId;
extern KEVENT kevtNull;
extern ULARGE_INTEGER g_luAdminPasswd;
extern BOOL g_fAdminPasswd;
extern XProfpGlobals* DmProfileData;

#endif // _DMP_INC
