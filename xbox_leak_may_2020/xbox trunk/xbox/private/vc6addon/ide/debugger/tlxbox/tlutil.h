
BOOL
GetHostName(
	MPT		mpt,
	LPTSTR	buffer,
	ULONG	bufferLen,
	LPGETSETPROFILEPROC	profileProc
	);

LPCTSTR
GetDialogTitle(
	MPT	mpt
	);

BOOL
GetRegistryValue(
	LPCTSTR				szKey,
	LPCTSTR				szValue,
	LPSTR				buffer,
	ULONG				bufferLen,
	LPGETSETPROFILEPROC	GetSetProfile
	);

BOOL
SetRegistryValue(
	LPCTSTR				szKey,
	LPCTSTR				szValue,
	LPCTSTR				szStr,
	LPGETSETPROFILEPROC	GetSetProfile
	);

BOOL
SetHostName(
	MPT		mpt,
	LPCTSTR	buffer,
	ULONG	bufferLen,
	LPGETSETPROFILEPROC	profileProc
	);


ULONG
WaitForMultiple(
	HANDLE	h1,
	HANDLE	h2,
	BOOL	fWaitAll,
	ULONG	timeout
	);

ULONG
WaitForMultiple(
	HANDLE	h1,
	HANDLE	h2,
	HANDLE	h3,
	BOOL	fWaitAll,
	ULONG	timeout
	);

ULONG
WaitForMultiple(
	HANDLE	h1,
	HANDLE	h2,
	HANDLE	h3,
	HANDLE	h4,
	BOOL	fWaitAll,
	ULONG	timeout
	);



USHORT
GetPollPort(
	MPT					mpt,
	LPGETSETPROFILEPROC	profileProc
	);


USHORT
GetMainPort(
	MPT					mpt,
	LPGETSETPROFILEPROC	profileProc
	);

BOOL
GetRegistryValue(
	LPCTSTR				szKey,
	LPCTSTR				szValue,
	DWORD*				lpDword,
	LPGETSETPROFILEPROC	GetSetProfile
	);
