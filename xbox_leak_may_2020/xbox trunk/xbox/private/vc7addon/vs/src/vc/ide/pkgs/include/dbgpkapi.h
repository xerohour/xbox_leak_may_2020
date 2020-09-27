#ifndef __DBGPKAPI_H__
#define __DBGPKAPI_H__

interface IDbgPkg;
interface IDLLInfo;

typedef IDbgPkg* PIDBGPKG;
typedef IDbgPkg* LPDEBUGPACKAGE;
typedef IDLLInfo* LPDLLINFO;

typedef enum _DocState {
	DOCSTATE_CLEAR,
	DOCSTATE_EDITED,
	DOCSTATE_SAVED,
} DocState;

/////////////////////////////////////////////////////////////////////////////
//	IDbgPkg interface

#undef INTERFACE
#define INTERFACE IDbgPkg

DECLARE_INTERFACE_(IDbgPkg, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	/*
	** IDbg
	**
	*/

    STDMETHOD_(BOOL, DebuggeeAlive)(void) PURE;
    STDMETHOD_(BOOL, DebuggeeRunning)(void) PURE;
    STDMETHOD_(void *, DbgDAMHsfFromFile)(LPCSTR) PURE;
    STDMETHOD_(BOOL, GetExecutablePath)(LPSTR) PURE;
	STDMETHOD_(BOOL, CanShowDataTip)(void) PURE;
	STDMETHOD(EvaluateExpression)(LPSTR szText, BOOL bFromSelection, LPSTR szResult, ULONG cb) PURE;

	STDMETHOD(SetDebugLines)(CDocument *pDoc, BOOL ResetTraceInfo) PURE;
	STDMETHOD(AdjustDebugLines)(CDocument * pDoc, int StartLine, int NumberLines, BOOL fAdded, BOOL fLineBreak, BOOL fWhiteSpaceOnly) PURE;
	STDMETHOD(DeleteBreakpointLinesInDoc)(LPCSTR sz) PURE;
    STDMETHOD_(BOOL, IsENCProjectFile)(CPath *pPath) PURE;
	STDMETHOD(VTestDebugActive)(BOOL *bActive) PURE;

	STDMETHOD(UpdateDocState)(LPCTSTR szDocName, DocState docState) PURE;
	STDMETHOD(StopDebugging)(void) PURE;

};

/////////////////////////////////////////////////////////////////////////////
//	IDLLInfo interface

typedef struct
{
	BOOL fPreload;
	CString strLocalName;
	CString strRemoteName;
	BOOL fDelete;
} DLLREC;	// DLL record


#undef  INTERFACE
#define INTERFACE IDLLInfo

DECLARE_INTERFACE_(IDLLInfo, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IDLLInfo methods
	STDMETHOD(DiffDLLs)(ULONG hTarget, CPtrList &listDLLs, INT *piRow, UINT *pidMsg) PURE;
	STDMETHOD(GetDLLCount)(ULONG hTarget, int *piCount) PURE;
	STDMETHOD(InitDLLList)(ULONG hTarget, CPtrList &listDLLs) PURE;
	STDMETHOD(SaveDLLList)(ULONG hTarget, CPtrList &listDLLs) PURE;
};

#endif // __DBGPKAPI_H__
