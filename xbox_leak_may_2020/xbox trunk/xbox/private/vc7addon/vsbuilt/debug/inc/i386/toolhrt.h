
/*++

Copyright(c) !YEAR Microsoft Corporation

Module Name:

	ToolHrt.h

Abstract:

	Useful stuff for listing processes, getting info from a PE image, etc.

Author:

    Matthew D Hendel (math) 03-Oct-1997

Environment:

	Win95, Win98, NT 4.0, NT 5.0.

--*/


#ifndef _TOOLHRT_H_
#define _TOOLHRT_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifndef LOCAL
#define LOCAL static
#endif


enum IMAGE_TYPE {
	IMAGE_TYPE_UNKNOWN,
	IMAGE_TYPE_DOS_STUB,
	IMAGE_TYPE_WIN16_OR_OS2,
	IMAGE_TYPE_WIN32,
	IMAGE_TYPE_CRASHDUMP,
	IMAGE_TYPE_MINIDUMP,
	IMAGE_TYPE_WIN64
};

#include "pshpack4.h"

typedef struct _IMAGE_INFO {

	DWORD	Size;			// Must be initialized prior to the call
	DWORD	ImageType;

	union {
		struct {
			ULONG	Subsystem;
			WORD	Characteristics;
			WORD	Machine;
			BOOL	ManagedCode;
			BOOL	ManagedOnly;
		} Win32;

		struct {
			BOOL	fUserMode;
			WORD	Machine;
		} CrashDump;
	} u;

} IMAGE_INFO;

#include "poppack.h"


//
// Get information about a binary executable file.
//

BOOL
WINAPI
GetImageInfo(
	LPCSTR		ImageName,
	IMAGE_INFO*	ImageInfo
	);

//
// Is this image a valid Win32 subsystem image
//

#define IS_WIN32_SUBSYSTEM_IMAGE(ii)	\
	((ii).ImageType == IMAGE_TYPE_WIN32 &&	\
	((ii).u.Win32.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI ||	\
	 (ii).u.Win32.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI))


#define WINDOW_TITLE_SIZE		64
#define PROCESS_NAME_SIZE		_MAX_FNAME

#define TASK_SYSTEM_PROCESS		0x0001
#define TASK_BEING_DEBUGGED		0x0002
#define TASK_16_BIT				0x0004
#define TASK_64_BIT				0x0008

#include "pshpack4.h"

typedef struct _TASK_ENTRY {
	//
	// Basic Info
    DWORD	dwProcessId;
	DWORD	dwFlags;

	// 
	// Extended Info
	//
	struct {
		DWORD   dwSessionId;
        DWORD	dwParentPid;
    	DWORD	dwImageType;

	    union{
	        HWND	hWnd;
	        DWORDLONG AlignhWnd;
	    };
        CHAR	szProcessName [PROCESS_NAME_SIZE];
        CHAR	szWindowTitle [WINDOW_TITLE_SIZE];
    	CHAR	szImageName [_MAX_PATH];
	} exInfo;
} TASK_ENTRY, *PTASK_ENTRY;

#include "poppack.h"

//
// Get just the dwProcessId and dwFlags fields for all processes
//
#define TASKENTRY_PROCESSID		0x00000001
#define TASKENTRY_FLAGS			0x00000002
#define TASKENTRY_EXTENDEDINFO	0x00000004
//
// Get the complete _TASK_ENTRY fields for all proccess
//
#define TASKENTRY_ALL			0x00000007



//
// Get a task list.
//

BOOL
WINAPI
GetTaskList(
	OUT TASK_ENTRY rgTasks [],
	IN OUT DWORD* lpdwTaskCount,
	IN DWORD      dwFlags
    );

//
// Check whether a process is a system (server) process or not.
//

BOOL
WINAPI
IsSystemProcess(
	DWORD dwProcessId
	);

//
// On Windows NT, change the SE_DEBUG_PRIVILEGE value. On Win9x, NOP.
//

BOOL
WINAPI
SetDebugPrivilege(
	BOOL fEnable
	);

BOOL
WINAPI
IsDebuggerPresentEx(
	DWORD dwPid
	);



HANDLE
OpenProcessEx(
	DWORD   dwDesiredAccess,
	BOOL    bInheritHandle,
	DWORD   dwProcessId,
	LPBOOL  lpfResetDebugPriv
	);

//
//	For dynimcally loading psapi.dll -- WinNT only
//

typedef DWORD (WINAPI *GETMODULEFILENAMEEXA)(
	HANDLE hProcess,
	HMODULE hModule,
	LPSTR lpFilename,
	DWORD nSize
	);

BOOL
WINAPI
GetImageName(
	DWORD dwProcessId,
	LPTSTR szImageName,
	DWORD cbBufferSize
	);
    

BOOL
WINAPI
GetSessionIdForProcess(
	DWORD   processId,
	LPDWORD lpSessionId
	);

BOOL
WINAPI
GetImageNameUsingPsapiEx(
	HANDLE               hProcess,
	HMODULE              hDll,
	DWORD                dwProcessId,
    GETMODULEFILENAMEEXA pfnGetModuleFileNameExA,
	LPTSTR               szImageName,
	DWORD                cbBufferSize
	);

BOOL
IsWin9x(
  );

BOOL
IsNT4(
 );

BOOL
WINAPI
IsProcess64Bit(
	DWORD dwProcessId
	);

BOOL
InitDbgProxy(
		);
VOID
UnInitDbgProxy(
    );

BOOL
ReconcileWithProxy(
    DWORD       dwCount,
	PTASK_ENTRY rgTasks,
	DWORD       dwFlags
	);

BOOL
ProxyIsDetachedPid(
	DWORD   dwPid,
	LPDWORD lpfIsDetached
	);

HANDLE
LoadUsrProfile(
	HANDLE hToken
	);

BOOL
UnLoadUsrProfile(
	HANDLE hToken,
	HANDLE hProfile
	);

BOOL
GetEnvironmentBlock(
	HANDLE  hToken,
	LPVOID* ppEnvBlock
	);
BOOL
ReleaseEnvironmentBlock(
	LPVOID pEnvBlock
	);



DECLARE_HANDLE(HJIT);

HJIT
NotifyOleRpcStepInitiate(
	);

BOOL
NotifyOleRpcStepComplete(
	HJIT hJit
	);

BOOL
GetPidForOleRpcJit(
	LPDWORD lpdwPid
	);

#ifdef __cplusplus
};		// extern "C"
#endif

#endif // _TOOLHRT_H_

