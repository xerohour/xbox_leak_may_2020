//
// xosdNone must be 0.  The rest do not matter
//

//
// NOTE: If you modify this header file then you must rebuild the
// xosd.rc (in this directory) file as well.
//

DECL_XOSD(xosdNone,                 0, "No Error")

DECL_XOSD(xosdContinue,             1, "Continue processing EMF")
DECL_XOSD(xosdPunt,                 2, "Pass to next EM")

DECL_XOSD(xosdGeneral,              3, "API failed")
DECL_XOSD(xosdUnknown,				4, "General Debugger error")
DECL_XOSD(xosdUnsupported,          5, "Feature not available")
DECL_XOSD(xosdInvalidHandle,        6, "Invalid handle passed to API")
DECL_XOSD(xosdInvalidParameter,     7, "Invalid parameter")
DECL_XOSD(xosdDuplicate,            8, "Duplicate EM or TL")
DECL_XOSD(xosdInUse,                9, "EM or TL is in use")
DECL_XOSD(xosdOutOfMemory,         10, "Insufficient memory available")
DECL_XOSD(xosdFileNotFound,        11, "File not found")
DECL_XOSD(xosdAccessDenied,        12, "Access denied")
DECL_XOSD(xosdBadProcess,          13, "Os Failed to Init Debuggee Properly")
DECL_XOSD(xosdBadThread,           14, "Inappropriate or nonexistent thread")
DECL_XOSD(xosdBadAddress,          15, "Invalid address")
DECL_XOSD(xosdInvalidBreakPoint,   16, "nonexistent breakpoint")
DECL_XOSD(xosdBadVersion,          17, "Debugger component versions mismatched")
DECL_XOSD(xosdQueueEmpty,          18, "Queue Empty - no error")
DECL_XOSD(xosdProcRunning,         19, "Operation invalid when process is running")
DECL_XOSD(xosdRead,                20, "Read Failure")
DECL_XOSD(xosdWrite,               21, "Write Failure")
DECL_XOSD(xosdIORedirSyntax,       22, "Syntax error in IO redirection")
DECL_XOSD(xosdIORedirBadFile,      23, "Cannot redirect with this file")
DECL_XOSD(xosdAllThreadsSuspended, 24, "All debugee threads are suspended")
DECL_XOSD(xosdSoftBroken,          25, "Debugeee is softbroken")
DECL_XOSD(xosdEndOfStack,          26, "end of stack")

DECL_XOSD(xosdLoadMdError,         30, "Failed to load minidump dll")

DECL_XOSD(xosdDllNotFound,         31, "Loader failed to find a dll during load")
DECL_XOSD(xosdNonContinuableException, 32, "A non-continuable Exception was encounterred")


DECL_XOSD(xosdCannotSetStepBp,     33, "Failed to set Stepping bp")
DECL_XOSD(xosdInstrCannotStep,     34, "Failed to set Stepping bp")

DECL_XOSD(xosdAlreadyStopped,      35, "Stopped at a stopping event")
DECL_XOSD(xosdUnSafeToLock,        36, "Its not safe to Lock")
DECL_XOSD(xosdNotYetLoaded,        37, "Debuggee hasn't finished loading(interoP)")


DECL_XOSD(xosdNubInit,             40, "Failed to init dbgproxy")
DECL_XOSD(xosdNubImpersonate,      41, "dbgproxy could not impersonate current user")
DECL_XOSD(xosdNubTsUnsupported,    42, "cannot detach when running in a TS session")
DECL_XOSD(xosdWin32Detach,         44, "Detach Deffered to OS")

DECL_XOSD(xosdInterOpTransitionStub,  50, "cannot step because of transition stub")
DECL_XOSD(xosdInterOpORpcInit,  		  51, "Ole/RPC debugging unsupported in interop mode")

DECL_XOSD(xosdLineNotConnected,    70, "Not connected")
DECL_XOSD(xosdCannotConnect,       71, "cannot connect")
DECL_XOSD(xosdLineDisconnect,      72, "Connection has ben cut")


DECL_XOSD(xosdNotRemote,           76, "not remote")


DECL_XOSD(xosdBadIPAddress,	       80, "The IP Address is bad")
DECL_XOSD(xosdNoMsvcmon,	       81, "MsVCMON Not running on remote machine")
DECL_XOSD(xosdNoMsvcmonForUser,	   82, "No msvmcon on the remote machine in running under current user context")
DECL_XOSD(xosdMachineDown,	       83, "The Remote machine is down")
DECL_XOSD(xosdMachineUnspecified,  84, "The Remote machine is Unspecified")

DECL_XOSD(xosdCannotStep,          90, "cannot step")
DECL_XOSD(xosdTlsAddr,			   91, "the address requires a thread")
DECL_XOSD(xosdIgnoreException,	   92, "An exception has been ignored")
DECL_XOSD(xosdLoadDmError,	       93, "Debugger cannot load the DM")

DECL_XOSD(xosdMiniDumpReadOnWin9x, 94, "Reading of minidumps not supported on win'9x")
DECL_XOSD(xosdCrashDumpActive,     98, "Cannot Multiproc debug with an active crashdump")
DECL_XOSD(xosdCrashDump,           99, "Operation Not support on Crashdump")

DECL_XOSD(xosdPartialRead,         100,"Could only read some of the bytes requested")
DECL_XOSD(xosdUnSupportedBinary,   103,"This binary format is not supported")
DECL_XOSD(xosdBadDirectoryForWrite,104,"Cannot write to directory")
DECL_XOSD(xosdProcessFreed,        105,"The process structure was freed by this call")
DECL_XOSD(xosdMax,	               200, "The maxium XOSD value")

