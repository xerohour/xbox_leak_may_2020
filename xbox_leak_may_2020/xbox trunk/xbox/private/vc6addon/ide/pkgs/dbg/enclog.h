///////////////////////////////////////////////////////////////////////////////
// 
// enclog.h
//
// Support for logging edit-and-continue results to the net for QA
//
///////////////////////////////////////////////////////////////////////////////

#define LOG_INIT					"Init"
#define LOG_POPEDIT					"PopEdit"
#define LOG_SAVEOLDOBJS				"SaveOldObjs"
#define LOG_GETCOMPILEENV			"GetCompileEnv"
#define LOG_INITIALIZESPAWN			"InitializeSpawn"
#define LOG_COMPILEERROR			"CompileError"
#define LOG_PUSHNEWEDIT				"PushNewEditHResultFailed"
#define LOG_APPLYCURRENTEDIT		"ApplyCurrentEditHResultFailed"
#define LOG_ADDMODSTOEC				"AddModsToEC"
#define LOG_DLGYES					"DlgYes"
#define LOG_DLGNO					"DlgNo"
#define LOG_RELINKFAILED			"RelinkFailed"
#define LOG_HRESULT_MEMMAP			"HResult=MEMMAP"
#define LOG_HRESULT_INITMEMALLOC	"HResult=INITMEMALLOC"
#define LOG_HRESULT_EXPORTPUB		"HResult=EXPORTPUB"
#define LOG_HRESULT_FIXUP			"HResult=FIXUP"
#define LOG_HRESULT_MEMWRITE		"HResult=MEMWRITE"
#define LOG_HRESULT_THUNKWRITE		"HResult=THUNKWRITE"
#define LOG_HRESULT_NOPDBSUPPORT	"HResult=NOPDBSUPPORT"
#define LOG_HRESULT_UNKNOWN			"HResult=Unknown"
#define LOG_SUCCESS					"Success"

#define MAX_NET_ACCESS_ATTEMPTS		40

void ENCLogPerformance(clock_t &start_clock, char* szStatus);
UINT ENCLogToNet(LPVOID pParam);
UINT ENCLogToNetCString(LPVOID pParam);
void ENCLogForHResult(HRESULT hResult);
void ENCLogHmod(HMOD hmod);

#ifdef _ENCLOG

// logs a static string to the net
inline void NET_LOG(LPCTSTR lsz) {
	AfxBeginThread(ENCLogToNet, (LPVOID)lsz);
}

// logs an HRESULT to the net
inline void NET_LOG_HRESULT(HRESULT hResult)	{
	ENCLogForHResult(hResult);
}

// logs information about an obj file used during ENC
inline void NET_LOG_HMOD(HMOD hMod) {
	ENCLogHmod(hMod);
}

// logs a string to the net
inline void NET_LOG_EX(LPCTSTR lsz) {
	CString *pStr = new CString(lsz);
	// pStr will be deleted by the logging thread
	AfxBeginThread(ENCLogToNetCString, (LPVOID)pStr);
}

#else
#define NET_LOG(pParam)
#define NET_LOG_HRESULT(hResult)
#define NET_LOG_HMOD(hMod)
#define NET_LOG_EX(pStr)
#endif

#define PERF_LOG(start_clock, szStatus) ENCLogPerformance(start_clock, szStatus);
#define START_PERF_CLOCK()					\
	clock_t start_clock = clock();			\
	static char szFailure[] = "Failure";	\
	static char szSuccess[] = "Success";


