/////////////////////////////////////////////////////////////////////////////
//	SRCAPI.H
//		Source package interface declarations.
#ifndef __SRCAPI_H__
#define __SRCAPI_H__

/////////////////////////////////////////////////////////////////////////////
//	IVCSourceParser interface

#include <parse.h>
class CVCPath;
#include <textmgr.h>
#ifndef STRICT
typedef DWORD HLEXER;
typedef DWORD HPARSER;
typedef DWORD HBSC;
typedef DWORD HNCB;
#else	// STRICT
DECLARE_HANDLE(HLEXER);
DECLARE_HANDLE(HPARSER);
DECLARE_HANDLE(HBSC);
DECLARE_HANDLE(HNCB);
#endif	// STRICT


enum EXEFROM	// where did GetExecutableFilename get name from?
{
	exefromNone,	// nowhere (no name found)
	exefromProj,	// from project
	exefromLastDoc,	// from last open src window
	exefromExe,		// from currently active exe window
	exefromPid,		// from PID on command line (just-in-time debugging)
};

interface VCProject;
interface VCConfiguration;

interface IVsHierarchy;
interface IVsProjectCfg;

/////////////////////////////////////////////////////////////////////////////
//	IVCSourceParser interface

__interface __declspec(uuid("{90D1AE81-29DC-11cf-B250-00AA0057AFF3}")) IVCSourceParser : public IUnknown
{
    STDMETHOD(GetLexerAndParser)(const TCHAR *szFileName, HLEXER *phLexer, HPARSER *phParser);
    STDMETHOD(GetBsc)(IVsProjectCfg * pIVsProjectCfg, BscEx ** ppBscEx);
    STDMETHOD(GetBscForActiveConfiguration)(IVsHierarchy * pIVsHierarchy, BscEx ** ppBscEx);
    STDMETHOD(GetBscForActiveConfigurationOfSelectedProject)(BscEx ** ppBscEx);
    STDMETHOD(GetBscForVCConfiguration)(VCConfiguration * pIVCConfiguration, BscEx ** ppBscEx);
    STDMETHOD(CanParseFile)(BSTR bstrFileName);
    STDMETHOD(IsTerminateThread)();
    STDMETHOD(GetDBase)(HNCB *phNcbParse);
    STDMETHOD(IsSuspendThread)();
    STDMETHOD(NotifyParseDone)(LPCSTR lpszFileName);
    STDMETHOD(WaitParserSuspended)();
    STDMETHOD(AddDirtyFilesToParseQueue)(BOOL fParse = TRUE);
    STDMETHOD(GetLine)(void *pTextDoc, int iLine, LPSTR &pszLine, UINT &uLength, DWORD &lxs) PURE;
    STDMETHOD(RegisterFileToWatch)(CString &strFileName) PURE;  // Non-standard COM.
    STDMETHOD(UnregisterFileToWatch)(CString &strFileName) PURE;    // Non-standard COM.
    STDMETHOD(AddToQueue)(const CVCPath *pPath, HTARGET hTarget, Action action, BOOL fAtTheEnd, BOOL fOverrideComPlus, BOOL fComPlus) PURE; // Non-standard COM
    STDMETHOD(SetEventAddQueue)() PURE; // to wake up the parser thread whenever there is some autocompletion needs to be done
    STDMETHOD(SetDynamicParsing) (BOOL fDyParse) PURE;
    STDMETHOD(GetCurrentParseFileBuffer)(LPCCH *ppFileContents, ULONG *pcbFile, LONG *pilineBegin, LONG *pilineEnd);
    STDMETHOD(NotifyOfNewView)(const WCHAR *wszFileName, IVsTextView *pVsTextView, IVsCodeWindow *pVsCodeWindow);
    STDMETHOD(NotifyOfWindowClose)(const WCHAR *wszFileName, IVsCodeWindow *pVsCodeWindow);
    STDMETHOD(OnUserPreferencesChanged)();    // used by IVsTextManagerEvents::OnUserPreferencesChanged (cpp.cpp)
    STDMETHOD(GetQueueSize)(int *piSize);     // Returns the number of items in the queue
    STDMETHOD_(BOOL, FComPlusFile)(const WCHAR *wszFileName);  // queries the proj system to see if COM+ compilation is enabled for this file
    STDMETHOD_(LPCRITICAL_SECTION, GetFeacpCS)(); // critical section, because FEACP is not reantrant
    STDMETHOD(SuspendParse)();
    STDMETHOD(ResumeParse)();
    STDMETHOD(GetAutoCompEvent)(HANDLE *hEvent);
};
typedef IVCSourceParser* LPVCSOURCEPARSER;

/////////////////////////////////////////////////////////////////////////////
//	IOutputWindow interface

#undef  INTERFACE
#define INTERFACE IOutputWindow

DECLARE_INTERFACE_(IOutputWindow, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IOutputWindow methods
	STDMETHOD(ShowOutputWindow)() PURE;
	STDMETHOD(ClearOutputWindow)() PURE;
	STDMETHOD(WriteStringToOutputWindow)(LPCSTR lpszError, BOOL fToolError, BOOL fAppendCrLf) PURE;
	STDMETHOD(OutputWindowQueueHit)(UINT nCommandID, LPCSTR lpszError,
		BOOL fToolError, BOOL fAppendCrLf, BOOL fForceFlush) PURE;
	STDMETHOD(OutputWindowDeleteVwin)(UINT nCommandID) PURE;
	STDMETHOD(OutputWindowShowVwin)(UINT nCommandID) PURE;
	STDMETHOD(OutputWindowSelectVwin)(UINT nCommandID, BOOL fOnTop) PURE;
	STDMETHOD(OutputWindowAddVwin)(LPSTR szToolName, BOOL fCurrentVwin, UINT *puRet) PURE;
	STDMETHOD(OutputWindowModifyVwin)(UINT nCommandID, LPSTR szToolName, UINT *puRet) PURE;
	STDMETHOD(OutputWindowVwinClear)(UINT nCommandID) PURE;
	STDMETHOD(OutputWindowClearWindow)(UINT nCommandID) PURE;
	STDMETHOD(OutputWindowClearContents)(UINT nCommandID) PURE;
	STDMETHOD(OutputWindowGetLine)(ULONG iline, ULONG *pcbLine, LPCSTR *ppLine) PURE;
	STDMETHOD(OwinGetBuildCommandID)(UINT *puRet) PURE;
	STDMETHOD(OwinGetDebugCommandID)(UINT *puRet) PURE;
	STDMETHOD(OwinIsBufferFull)(BOOL* fFull) PURE;	
};

/////////////////////////////////////////////////////////////////////////////
//	IFileAccess interface

#undef  INTERFACE
#define INTERFACE IFileAccess

DECLARE_INTERFACE_(IFileAccess, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IFileAccess methods
	STDMETHOD(CreateFile)(LPCTSTR lpszName, DWORD fdwAccess, DWORD fdwShareMode, LPSECURITY_ATTRIBUTES lpsa, DWORD fdwCreate, DWORD fdwAttrsAndFlags, HANDLE hTemplateFile, HANDLE *hRet) PURE;
	STDMETHOD(OpenFile)(const char *pszFileName, const char *pszMode, FILE **ppRet) PURE;
	STDMETHOD(CreateFileForParse)(LPCTSTR lpszName, DWORD fdwAccess, DWORD fdwShareMode, LPSECURITY_ATTRIBUTES lpsa, DWORD fdwCreate, DWORD fdwAttrsAndFlags, HANDLE hTemplateFile, HANDLE *hRet) PURE;
	STDMETHOD(CloseFileForParse)(HANDLE handle) PURE;
	STDMETHOD(Replace)(const char *pszOldName, const char *pszNewName, int *iRet) PURE;
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//	ICustomContextMenu interface
//		Access to custom context menus on a per-language / lexer basis

#undef  INTERFACE
#define INTERFACE ICustomContextMenu

/////////////////////////////////////////////////////////////////////////////
//	forward declaration
struct POPDESC;
typedef POPDESC* LPPOPDESC;


DECLARE_INTERFACE_(ICustomContextMenu, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// ICustomContextMenu methods
	STDMETHOD(GetContextMenu)(THIS_ LPCTSTR, LPPOPDESC*) PURE;
};

#ifdef _SQLDBG
#include "sqlpkapi.h"
#endif // _SQLDBG

#endif	// __SRCAPI_H__
