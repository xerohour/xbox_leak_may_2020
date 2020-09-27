#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4103)
#include <initguid.h>
#include <olectlid.h>
#include <dbgpguid.h>
#include <prjguid.h>
#include <srcguid.h>
#include <bldguid.h>
#include <utilguid.h>
#include <clvwguid.h>
#pragma warning(default:4103)

BEGIN_INTERFACE_MAP(CDebugPackage, CPackage)
	INTERFACE_PART(CDebugPackage, IID_IDbgPkg, DbgPkg)
	INTERFACE_PART(CDebugPackage, IID_IDLLInfo, DLLInfo)
END_INTERFACE_MAP()

STDMETHODIMP CDebugPackage::XDbgPkg::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP_(ULONG) CDebugPackage::XDbgPkg::AddRef()
{
	// Delegate to our exported AddRef.

	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CDebugPackage::XDbgPkg::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP_(BOOL) CDebugPackage::XDbgPkg::DebuggeeAlive(void)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
    return( ::DebuggeeAlive() );
}

STDMETHODIMP_(BOOL) CDebugPackage::XDbgPkg::DebuggeeRunning(void)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
    return( ::DebuggeeRunning() );
}

STDMETHODIMP_(BOOL) CDebugPackage::XDbgPkg::CanShowDataTip(void)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
	return runDebugParams.fDataTips && 
			DebuggeeAlive() && 
			!DebuggeeRunning();
}

STDMETHODIMP CDebugPackage::XDbgPkg::EvaluateExpression(LPTSTR szText, BOOL bFromSelection, LPTSTR szResult, ULONG cb)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
	if (bFromSelection)
	{
		RemoveSideEffectsExpr(szText);
	}
	if (FEvalExpr(szText, szResult, (int)cb))
		return ResultFromScode(NOERROR);
	else
		return ResultFromScode(E_FAIL);
}

STDMETHODIMP CDebugPackage::XDbgPkg::SetDebugLines(CDocument *pDoc, BOOL ResetTraceInfo)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
	::SetDebugLines(pDoc, ResetTraceInfo);
	return ResultFromScode(NOERROR);
}

STDMETHODIMP CDebugPackage::XDbgPkg::AdjustDebugLines(CDocument * pDoc, int StartLine, int NumberLines, BOOL fAdded, BOOL fLineBreak, BOOL fWhiteSpaceOnly)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
	::AdjustDebugLines(pDoc, StartLine, NumberLines, fAdded, fLineBreak, fWhiteSpaceOnly);
	return ResultFromScode(NOERROR);
}

STDMETHODIMP CDebugPackage::XDbgPkg::DeleteBreakpointLinesInDoc(LPCTSTR sz)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
	CString str(sz);
	::DeleteBreakpointLinesInDoc(str);
	return ResultFromScode(NOERROR);
}

STDMETHODIMP_(void *) CDebugPackage::XDbgPkg::DbgDAMHsfFromFile(LPCTSTR psz)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
    extern HSF PASCAL DAMHsfFromFile( const char FAR * );

    return( DAMHsfFromFile(psz) );
}

STDMETHODIMP_(BOOL) CDebugPackage::XDbgPkg::GetExecutablePath(PSTR pszOut)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
    lstrcpy(pszOut, ExecutablePath);

    return( TRUE );
}

STDMETHODIMP_(BOOL) CDebugPackage::XDbgPkg::IsENCProjectFile(CPath *ppath)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
    extern BOOL ENCProjectFile(CPath *ppath);

    return ENCProjectFile(ppath);
}

STDMETHODIMP CDebugPackage::XDbgPkg::VTestDebugActive(BOOL *bActive)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DbgPkg)
	BOOL bReturn = fVisTestDebugActive;
	fVisTestDebugActive = *bActive;
	*bActive = bReturn;
	return NOERROR;
}

///////////////////////////////////////////////////////////////////
// IUnknown methods (IDLLInfo)

STDMETHODIMP_(ULONG) CDebugPackage::XDLLInfo::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CDebugPackage, DLLInfo)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CDebugPackage::XDLLInfo::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CDebugPackage, DLLInfo)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CDebugPackage::XDLLInfo::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CDebugPackage, DLLInfo)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

///////////////////////////////////////////////////////////////////
// IDLLInfo methods (IDLLInfo)

extern BOOL DiffDlls(HTARGET, CPtrList &, INT *, UINT *);
IDE_EXPORT int GetDLLCount(HTARGET);
extern void InitDLLList(ULONG, CPtrList &);
extern void SaveDLLList(ULONG, CPtrList &);

STDMETHODIMP CDebugPackage::XDLLInfo::DiffDLLs(ULONG hTarget, CPtrList &listDLLs, INT *piRow, UINT *pidMsg)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DLLInfo);
	return(::DiffDlls(hTarget, listDLLs, piRow, pidMsg) ? S_OK : S_FALSE);
}

STDMETHODIMP CDebugPackage::XDLLInfo::GetDLLCount(ULONG hTarget, int *piCount)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DLLInfo);
	*piCount = ::GetDLLCount((HTARGET)hTarget);
	return NOERROR;
}

STDMETHODIMP CDebugPackage::XDLLInfo::InitDLLList(ULONG hTarget, CPtrList &listDLLs)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DLLInfo);
	::InitDLLList(hTarget, listDLLs);
	return NOERROR;
}

STDMETHODIMP CDebugPackage::XDLLInfo::SaveDLLList(ULONG hTarget, CPtrList &listDLLs)
{
	METHOD_PROLOGUE_EX(CDebugPackage, DLLInfo);
	::SaveDLLList(hTarget, listDLLs);
	return NOERROR;
}

