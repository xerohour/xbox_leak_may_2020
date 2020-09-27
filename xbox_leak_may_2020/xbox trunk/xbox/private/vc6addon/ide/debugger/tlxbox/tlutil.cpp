#include "precomp.h"
#include "tlutil.h"

#define TRANSPORT_NAME _T("TLXBOX")
#define HOST_KEY_NAME _T("HostName")
#define REG_KEY_PREFIX	_T("Build System\\Components\\Platforms\\")
#define REG_KEY_SUFFIX	_T("\\Transports\\" TRANSPORT_NAME)
#define POLL_PORT_KEY_NAME _T("PollPort")
#define MAIN_PORT_KEY_NAME _T("MainPort")

#if 0
typedef struct _PLATFORM_INFO
{
	LPCTSTR	szRegistryKey ;
	LPCTSTR	szDialogTitle ;

} PLATFORM_INFO;


PLATFORM_INFO	rgPlatformInfo [] =
{
	{ NULL, NULL },							// nothing

	{	// mptix86
		_T (REG_KEY_PREFIX "Win32 (x86)" REG_KEY_SUFFIX),
		_T ("Win32 Network (TCP/IP) Settings")
	},
	
	{ NULL, NULL },							// mptm68k -- no more 68k debugging
	{ NULL, NULL },							// mptdaxp
	{ NULL, NULL },							// mptmips

	{	// mptmppc
		_T (REG_KEY_PREFIX "Power Macintosh" REG_KEY_SUFFIX),
		_T ("Power Macintosh Network (TCP/IP) Settings")
	},
	
	{ NULL, NULL },							// mptntppc
	{ NULL, NULL }							// mptUnknown
};
#endif
	

LPCTSTR
GetRegistryKey(
	MPT		mpt
	)
{
    return REG_KEY_PREFIX "Xbox" REG_KEY_SUFFIX;
}


LPCTSTR
GetDialogTitle(
	MPT	mpt
	)
{
	return "Xbox Network (TCP/IP) Settings";
}


BOOL
GetHostName(
	MPT		mpt,
	LPTSTR	buffer,
	ULONG	bufferLen,
	LPGETSETPROFILEPROC	profileProc
	)
{
    //_asm int 3
	BOOL	fSuccess;
	LPCTSTR	key;

	key = GetRegistryKey (mpt);

	if (!key)
		return FALSE;
	
	fSuccess = GetRegistryValue (key,
								 HOST_KEY_NAME,
								 buffer,
								 bufferLen,
								 profileProc
								 );

	return fSuccess;
}

BOOL
SetHostName(
	MPT		mpt,
	LPCTSTR	buffer,
	ULONG	bufferLen,
	LPGETSETPROFILEPROC	profileProc
	)
{
	BOOL	fSuccess;
	LPCTSTR	key;

	key = GetRegistryKey (mpt);

	if (!key)
		return FALSE;
	
	fSuccess = SetRegistryValue (key,
								 HOST_KEY_NAME,
								 buffer,
								 profileProc
								 );

	return fSuccess;
}


BOOL
GetRegistryValue(
	LPCTSTR				szKey,
	LPCTSTR				szValue,
	LPTCH				buffer,
	ULONG				bufferLen,
	LPGETSETPROFILEPROC	GetSetProfile
	)
{
	ULONG	size;
	ULONG	dwType = REG_SZ;
	
	assert (GetSetProfile);

	size = GetSetProfile ((LPTSTR) szKey,
						  (LPTSTR) szValue,
						  &dwType,
						  (BYTE*) buffer,
						  bufferLen,
						  FALSE
						  );

	if (size)
		return TRUE;

	return FALSE;
}

BOOL
GetRegistryValue(
	LPCTSTR				szKey,
	LPCTSTR				szValue,
	DWORD*				lpDword,
	LPGETSETPROFILEPROC	GetSetProfile
	)
{

	ULONG	size;
	ULONG	dwType = REG_DWORD;

	assert (GetSetProfile);

	size = GetSetProfile ((LPTSTR) szKey,
						  (LPTSTR) szValue,
						  &dwType,
						  (BYTE*) lpDword,
						  sizeof (*lpDword),
						  FALSE
						  );

	if (size) {
		return TRUE;
	}

	return FALSE;
}					  
	
	

BOOL
SetRegistryValue(
	LPCSTR				szKey,
	LPCSTR				szValue,
	LPCSTR				szStr,
	LPGETSETPROFILEPROC	GetSetProfile
	)
{
	ULONG	size = _tcslen (szStr);
	ULONG	rSize;
	ULONG	type = REG_SZ;

	rSize = GetSetProfile ( (LPTSTR) szKey,
							(LPTSTR) szValue,
							&type,
							(LPBYTE) szStr,
							size,
							TRUE
							);

	if (rSize == size)
		return TRUE;

	return FALSE;
}


#if 0
void
_Assert(
    const char*     cond,
    const char*     file,
    int             line
    )
{
	DbgPrint ("Assert failed: \"%s\", %s %d\n", cond, file, line);

	if (!_transport.AssertFailed (cond, file, line))
	{
		DbgBreakPoint ();
	}
}


ULONG
WaitForMultiple(
	HANDLE	h1,
	HANDLE	h2,
	BOOL	fWaitAll,
	ULONG	timeout
	)
{
	HANDLE	rg [2];

	rg[0] = h1;
	rg[1] = h2;

	return WaitForMultipleObjects (2, rg, fWaitAll, timeout);
}


ULONG
WaitForMultiple(
	HANDLE	h1,
	HANDLE	h2,
	HANDLE	h3,
	BOOL	fWaitAll,
	ULONG	timeout
	)
{
	HANDLE	rg [3];

	rg[0] = h1;
	rg[1] = h2;
	rg[2] = h3;

	return WaitForMultipleObjects (3, rg, fWaitAll, timeout);
}

ULONG
WaitForMultiple(
	HANDLE	h1,
	HANDLE	h2,
	HANDLE	h3,
	HANDLE	h4,
	BOOL	fWaitAll,
	ULONG	timeout
	)
{
	HANDLE	rg [4];

	rg[0] = h1;
	rg[1] = h2;
	rg[2] = h3;
	rg[3] = h4;

	return WaitForMultipleObjects (4, rg, fWaitAll, timeout);
}	
#endif
