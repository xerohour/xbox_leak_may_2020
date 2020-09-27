#ifndef _ISDOUTILS_
	#define _ISDOUTILS_

#define UNKNOWN_CPU (DWORD) 0
#define INTEL (DWORD)1
#define AMD	  (DWORD)2
#define CYRIX (DWORD)3

BOOL DoEXR(ULARGE_INTEGER exr);
BOOL ResetCXR();
BOOL DoTilde();
BOOL ReloadWin32k();

BOOL IsWaitingOnLoaderLock();
BOOL IsWaitingOnCriticalSection();
BOOL IsRtlEnterCriticalSection();
BOOL IsAquireResourceExclusive();
BOOL IsVerifierAquireResource();
BOOL IsAquireResourceShared();
BOOL IsRtlAquireResourceExclusive();
BOOL IsContaingW32EnterCritical();
BOOL IsStuckInDuplicateObject();
BOOL IsStuckInChainedDispatch();
BOOL IsAMutant();
BOOL IsWaitingOnLPC();
BOOL IsUserFailure();
BOOL IsGreLockDisplay();
DWORD GetCPUType();
BOOL Is1541HostBridge();

#endif