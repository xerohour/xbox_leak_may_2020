#ifndef _ILOG_INC
#define _ILOG_INC

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ------------------------------------------------------------------------------
// For consistancy sake, it is recommended that you use the following values for 
// the nLevel param in the various ILog methods
// ------------------------------------------------------------------------------
typedef enum 
{
    eFatalError,            // Memory allocation errors, internal test errors.
    eError,                 // Test case will fail because of this.
    eWarn1,                 // Important warning messages.
    eInfo1,                 // Important information, such as the device name, format.
    eWarn2,                 // 
    eInfo2,                 // Information about test flow.
    eWarn3,                 //
    eInfo3,                 // API logs (tdsound, wave, etc)
    eBlab1,                 // noise not important.
    eBlab2                  // noise not important.
} EErrorLevel;

// ------------------------------------------------------------------------------------------
// interface class for ILog
// Generic logging interface.
// ------------------------------------------------------------------------------------------
class ILog
{
public:
    virtual int      _cdecl Log( UINT nLevel, LPSTR pszFormat, ...) = 0;
    virtual int      _cdecl LogEx( DWORD dwType, UINT nLevel, LPSTR pszFormat, ...) = 0;
    virtual int      _cdecl LogSummary( UINT nLevel, LPSTR pszFormat, ...) = 0;
    virtual int      _cdecl LogStatusBar( LPSTR pszFormat, ...) = 0;
    virtual void     LogStdErrorString(DWORD dwErrorCode) = 0;
	virtual void     SetDeferredLogging (BOOL fOn) = 0;
	virtual void     CommitLog () = 0;
	virtual void	 DiscardDeferredLog () = 0;
};

// ------------------------------------------------------------------------------------------
// This is an implimentation of ILog that doesn't do anything
// If you are building a lib that can be used by both the testshell and by other programs, declare a 
// static LogStub in the lib and point a ILog* to it.  Also provide a registration method, so that 
// apps that use the test shell can enable logging.
//   example:  in the lib  
/*
static LogStub s_LogStub;
static ILog*   s_iLog = &s_LogStub;

BOOL xyzRegisterLog(ILog* iLog)
{
    if(!iLog)
        return FALSE;

    s_iLog = iLog;
    return TRUE;
}
*/
//   end of example
// ------------------------------------------------------------------------------------------
class LogStub : public ILog
{
public:
    virtual int      _cdecl Log( UINT nLevel, LPSTR pszFormat, ...) {UNREFERENCED_PARAMETER(nLevel);UNREFERENCED_PARAMETER(pszFormat);return 0;}
    virtual int      _cdecl LogEx( DWORD dwType, UINT nLevel, LPSTR pszFormat, ...) {UNREFERENCED_PARAMETER(dwType);UNREFERENCED_PARAMETER(nLevel);UNREFERENCED_PARAMETER(pszFormat);return 0;}
    virtual int      _cdecl LogSummary( UINT nLevel, LPSTR pszFormat, ...) {UNREFERENCED_PARAMETER(nLevel);UNREFERENCED_PARAMETER(pszFormat);return 0;}
    virtual int      _cdecl LogStatusBar( LPSTR pszFormat, ...) {UNREFERENCED_PARAMETER(pszFormat);return 0;}
    virtual void     LogStdErrorString(DWORD dwErrorCode) {UNREFERENCED_PARAMETER(dwErrorCode);return;}
	virtual void     SetDeferredLogging (BOOL fOn) {UNREFERENCED_PARAMETER(fOn);return;}
	virtual void     CommitLog () {return;}
	virtual void	 DiscardDeferredLog () {return;}
};

#endif //#ifndef _ILOG_INC
