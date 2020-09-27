// AutoDbg.cpp : implementation file
//

#include "stdafx.h"

#ifdef DEBUGGER_AUTOMATION

#include <afxctl.h>
#include "utilauto.h"
#include <ObjModel\dbgguid.h>
#include "AutoDbg.h"
#include "privguid.h"
#include <ObjModel\dbgdefs.h>
#include <ObjModel\appauto.h>
#include "resource.h"
#include "autobps.h"
#include "autobp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

VOID SetJitState(BOOL fNewState);
BOOL GetJitState();


CAutoDebugger *pAutoDebugger;

/////////////////////////////////////////////////////////////////////////////
// CAutoDebugger

IMPLEMENT_DYNCREATE(CAutoDebugger, CAutoObj)

CAutoDebugger::CAutoDebugger()
{
	ASSERT(FALSE);
}


CAutoDebugger::CAutoDebugger(CDebugPackage *pDebugPackage)
{
	dbprintf(("CAutoDebugger"));
	EnableDualAutomation();
	EnableConnections();
	m_pDebugPackage = pDebugPackage;
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
	pAutoDebugger = this;
}

CAutoDebugger::~CAutoDebugger()
{
	dbprintf(("~CAutoDebugger"));
	// To terminate the application when all objects created with
	// 	OLE automation are destroyed, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();

	pAutoDebugger = 0;
}


void CAutoDebugger::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CAutoObj::OnFinalRelease();
}

BOOL CAutoDebugger::NoPkg(void) // TRUE if associated package released, otherwise throws error
{
	if (!m_pDebugPackage || theApp.m_pAutoApp->IsZombified())
	{
		DsThrowOleDispatchException(
		  DS_E_DBG_PKG_RELEASED, // HRESULT
			IDS_ERR_DBG_PKG_RELEASED); // message resource ID

    return TRUE;
	}

	return FALSE;
}

// TRUE if we shouldn't execute a Run-type cmd
// Also called by Evaluate(), which is invalid while running
BOOL CAutoDebugger::NoRunCmds(void) 
{
	if (NoPkg())
		return TRUE;
		
	// If the debuggee is already running, it's a bad idea to run again
	if (DebuggeeRunning())
	{
		DsThrowOleDispatchException(
		  DS_E_DBG_RUNNING, // HRESULT
		  IDS_ERR_DBG_RUNNING); // message resource ID
		return TRUE;
	}
	return FALSE;
} // NoRunCmds


BEGIN_MESSAGE_MAP(CAutoDebugger, CAutoObj)
	//{{AFX_MSG_MAP(CAutoDebugger)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoDebugger, CAutoObj)
	//{{AFX_DISPATCH_MAP(CAutoDebugger)
	DISP_PROPERTY_EX(CAutoDebugger, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoDebugger, "Parent", GetParent, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoDebugger, "Breakpoints", GetBreakpoints, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoDebugger, "DefaultRadix", GetDefaultRadix, SetDefaultRadix, VT_I4)
	DISP_PROPERTY_EX(CAutoDebugger, "ProcessState", GetState, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoDebugger, "JustInTimeDebugging", GetJustInTimeDebugging, SetJustInTimeDebugging, VT_BOOL)
	DISP_PROPERTY_EX(CAutoDebugger, "RemoteProcedureCallDebugging", GetRemoteProcedureCallDebugging, SetRemoteProcedureCallDebugging, VT_BOOL)
	DISP_FUNCTION(CAutoDebugger, "Go", Go, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CAutoDebugger, "StepInto", StepInto, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CAutoDebugger, "StepOver", StepOver, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CAutoDebugger, "StepOut", StepOut, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CAutoDebugger, "Restart", Restart, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CAutoDebugger, "Stop", Stop, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CAutoDebugger, "Break", Break, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CAutoDebugger, "Evaluate", Evaluate, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CAutoDebugger, "ShowNextStatement", ShowNextStatement, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CAutoDebugger, "RunToCursor", RunToCursor, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CAutoDebugger, "SetNextStatement", SetNextStatement, VT_EMPTY, VTS_VARIANT)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IDispDebugger to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

BEGIN_INTERFACE_MAP(CAutoDebugger, CAutoObj)
  INTERFACE_PART(CAutoDebugger, IID_IDispDebugger, Dispatch)
  INTERFACE_PART(CAutoDebugger, IID_IDebugger, Dispatch)
  DUAL_ERRORINFO_PART(CAutoDebugger)
	INTERFACE_PART(CAutoDebugger, IID_IConnectionPointContainer, ConnPtContainer)
END_INTERFACE_MAP()

IMPLEMENT_DUAL_ERRORINFO(CAutoDebugger, IID_IDebugger)
DS_IMPLEMENT_ENABLE_DUAL(CAutoDebugger, Debugger)
DS_DELEGATE_DUAL_INTERFACE(CAutoDebugger, Debugger)

BEGIN_CONNECTION_MAP(CAutoDebugger, CAutoObj)
  CONNECTION_PART(CAutoDebugger, IID_IDebuggerEvents, DualDebuggerCP)
  CONNECTION_PART(CAutoDebugger, IID_IDispDebuggerEvents, DispDebuggerCP)
END_CONNECTION_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAutoDebugger message handlers


LPDISPATCH CAutoDebugger::GetApplication() 
{
	return theApp.m_pAutoApp->GetIDispatch(TRUE); // give 'em the app object if no frame
}

LPDISPATCH CAutoDebugger::GetParent() 
{
	// since parent of 'debugger' is the app, return the same answer
	return GetApplication();
}

void CAutoDebugger::Go() 
{
	// If we're inside an event handler, then defer this call to go
	//  until after the event handler returns
	if (lpprcCurr && lpprcCurr->m_bInsideEvent)
	{
		lpprcCurr->m_bGoDeferred = TRUE;
		return;
	}

	if (NoRunCmds())
		return;

	m_pDebugPackage->OnDebugGoPostMsg(IDM_RUN_GO);
}

void CAutoDebugger::StepInto() 
{
	if (NoRunCmds())
		return;

	m_pDebugPackage->OnDebugTraceInto();
}


void CAutoDebugger::StepOver() 
{
	if (NoRunCmds())
		return;

	m_pDebugPackage->OnDebugStepOver();
}

void CAutoDebugger::StepOut() 
{
	if (NoRunCmds())
		return;

	m_pDebugPackage->OnDebugStopAfterReturn();
}

void CAutoDebugger::Restart() 
{
	if (NoRunCmds())
		return;

	m_pDebugPackage->OnDebugRestart();
}

void CAutoDebugger::Stop() 
{
	if (NoPkg())
		return;

	if (lpprcCurr && lpprcCurr->m_bInsideEvent)
	{
		lpprcCurr->m_bStopDeferred = TRUE;
		return;
	}
	
	m_pDebugPackage->OnDebugStop();
}

void CAutoDebugger::Break() 
{
	if (NoPkg())
		return;

	m_pDebugPackage->OnDebugBreak(IDM_RUN_BREAK);
}

BSTR CAutoDebugger::Evaluate(LPCTSTR Expression) 
{
	char szRet[256];

	if (NoRunCmds())	// Won't actually return, throws exception
		return NULL;
		
	if (!FEvalExpr((char *)Expression, szRet, sizeof(szRet)))
	{
		DsThrowOleDispatchException(
		  DS_E_DBG_CANT_EVAL, // HRESULT
			IDS_ERR_DBG_CANT_EVAL); // message resource ID
	}

	CString strResult(szRet);
	
	return strResult.AllocSysString();
}

void CAutoDebugger::ShowNextStatement() 
{
	if (NoRunCmds())
		return;
		
	m_pDebugPackage->OnGotoCurrentInstruction();
}

void CAutoDebugger::RunToCursor() 
{
	if (NoRunCmds())
		return;

	m_pDebugPackage->OnDebugToCursor();
}

void CAutoDebugger::SetNextStatement(const VARIANT FAR& Selection) 
{
	if (NoRunCmds())
		return;

	if (!MoveIPToCursor(VarEditorLine(Selection)))
	{
		DsThrowOleDispatchException(
		  DS_E_DBG_SET_IP, // HRESULT
		  IDS_ERR_DBG_SET_IP); // message resource ID
	}
}

LPDISPATCH CAutoDebugger::GetBreakpoints() 
{
	
	if (NoPkg())
		return NULL;

	if (pAutoBPS) return pAutoBPS->GetIDispatch(TRUE);
	
	// Create the Windows automation object.
	CAutoBreakpoints* pAutoBreakpoints = new CAutoBreakpoints();

	// Get and return the IDispatch pointer for CAutoDocuments, but don't addref.
	return pAutoBreakpoints->GetIDispatch(FALSE);
}

long CAutoDebugger::GetDefaultRadix() 
{
	return radix;
}

void CAutoDebugger::SetDefaultRadix(long nNewValue) 
{
	ASSERT((nNewValue == 10) || (nNewValue == 16));
	
	if (nNewValue == 10) runDebugParams.decimalRadix = 1;
	else if (nNewValue == 16) runDebugParams.decimalRadix = 0;
	
	UpdateRadix(runDebugParams.decimalRadix);
}

DsExecutionState CAutoDebugger::GetState() 
{
	if (!DebuggeeAlive()) return dsNoDebugee;
	else if (!DebuggeeRunning()) return dsBreak;
	else return dsRunning;
}

BOOL CAutoDebugger::GetJustInTimeDebugging() 
{
	return GetJitState();
}

void CAutoDebugger::SetJustInTimeDebugging(BOOL bNewValue) 
{
	// UI has RPC changing whenever JIT does
	if (!bNewValue != !runDebugParams.fOleRpc) 
	{
		runDebugParams.fOleRpc = bNewValue;
		HammerOleRegKey();
	}
	SetJitState(bNewValue);
}

BOOL CAutoDebugger::GetRemoteProcedureCallDebugging() 
{
	return runDebugParams.fOleRpc;
}

void CAutoDebugger::SetRemoteProcedureCallDebugging(BOOL bNewValue) 
{
	// If we enable RPC, force JIT on - match UI
	if (bNewValue && !GetJitState()) 
	{
		SetJitState(TRUE);
	}
	runDebugParams.fOleRpc = bNewValue;
	HammerOleRegKey();
}

VOID CAutoDebugger::FireBreakpointHit(CAutoBreakpoint *bp)
{
	ASSERT (lpprcCurr != NULL);
	ASSERT (lpprcCurr->m_bInsideEvent == FALSE);
	lpprcCurr->m_bInsideEvent = TRUE;
	IDispatch *pDispatch = bp->GetIDispatch(TRUE);
	DS_FIRE_EVENT1(&m_xDispDebuggerCP,&m_xDualDebuggerCP,IDebuggerEvents,
			BreakpointHit,1,EVENT_PARAM(VTS_DISPATCH),pDispatch);
	pDispatch->Release();
	lpprcCurr->m_bInsideEvent = FALSE;
}
	


#endif DEBUGGER_AUTOMATION


