// AutoBp.cpp : implementation file
//

#include "stdafx.h"

#ifdef DEBUGGER_AUTOMATION

#include "utilauto.h"
#include <ObjModel\dbgguid.h>
#include "autoguid.h"
#include "AutoBp.h"
#include "autobps.h"
#include "autodbg.h"
#include "privguid.h"
#include <ObjModel\dbgdefs.h>
#include <ObjModel\appauto.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoBreakpoint

IMPLEMENT_DYNCREATE(CAutoBreakpoint, CAutoObj)

#ifdef _DEBUG
int nAutoBreakpoint;	// BUGBUG this is temp
#endif

CAutoBreakpoint::CAutoBreakpoint()
{
	ASSERT(FALSE); // don't want to use this ctor

	EnableDualAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
	m_pbpn = 0;

	// Hi, Dad (or Mom)
	pAutoDebugger->GetIDispatch(TRUE);
}

CAutoBreakpoint::CAutoBreakpoint(PBREAKPOINTNODE pbpNode)
{
	EnableDualAutomation();

#ifdef _DEBUG
	char foo[200];	// BUGBUG this is temp
	wsprintf(foo, "    +BP %x/%x/%x (#%d)\n", 
		this, pbpNode, (char *)this + 0x10, ++nAutoBreakpoint);
	OutputDebugString(foo);
#endif
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();

	// Store away the breakpoint node
	m_pbpn = pbpNode;

	// Let the breakpoint node point back to this breakpoint object
	ASSERT(pbpNode->pAutoBP == NULL);
	pbpNode->pAutoBP = this;

	if (pAutoBPS) pAutoBPS->AddBP(this);

	// Hi, Dad (or Mom)
	pAutoDebugger->GetIDispatch(TRUE);
}

CAutoBreakpoint::~CAutoBreakpoint()
{
#ifdef _DEBUG
	char foo[200];	// BUGBUG this is temp
	wsprintf(foo, "    -BP %x/%x (#%d)\n", this, m_pbpn, nAutoBreakpoint--);
	OutputDebugString(foo);
#endif
	
	// To terminate the application when all objects created with
	// 	OLE automation are destroyed, the destructor calls AfxOleUnlockApp.
	AfxOleUnlockApp();

	// We DON'T want to free the breakpoint node associated with this object,
	// but we do want to make sure it knows this object no longer exists.
	if (m_pbpn) 
	{
		m_pbpn->pAutoBP = NULL;
	}

	// Bye, Dad (or Mom)
	IDispatch *pDispatch = pAutoDebugger->GetIDispatch(FALSE);
	pDispatch->Release();
}


void CAutoBreakpoint::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// delete the object.  Add additional cleanup required for your
	// object before calling the base class.

	// Don't need to clean up anything.  Can't be found in any collections,
	// or our refcount would be >0.  Don't need to clean up the BPNode,
	// cause those last longer than we do.

	CAutoObj::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CAutoBreakpoint, CAutoObj)
	//{{AFX_MSG_MAP(CAutoBreakpoint)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoBreakpoint, CAutoObj)
	//{{AFX_DISPATCH_MAP(CAutoBreakpoint)
	DISP_PROPERTY_EX(CAutoBreakpoint, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoBreakpoint, "Parent", GetParent, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoBreakpoint, "Enabled", GetEnabled, SetEnabled, VT_BOOL)
	DISP_PROPERTY_EX(CAutoBreakpoint, "Location", GetLocation, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CAutoBreakpoint, "File", GetFile, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CAutoBreakpoint, "Function", GetFunction, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CAutoBreakpoint, "Executable", GetExecutable, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CAutoBreakpoint, "Condition", GetCondition, SetCondition, VT_BSTR)
	DISP_PROPERTY_EX(CAutoBreakpoint, "Elements", GetElements, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoBreakpoint, "PassCount", GetPassCount, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoBreakpoint, "Message", GetMessage, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoBreakpoint, "WindowProcedure", GetWindowProcedure, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CAutoBreakpoint, "Type", GetType, SetNotSupported, VT_I4)
	DISP_FUNCTION(CAutoBreakpoint, "Remove", Remove, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

//	DISP_PROPERTY_EX(CAutoBreakpoint, "IsValid", GetValid, SetNotSupported, VT_BOOL)

// Note: we add support for IID_IDispBreakpoint to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

BEGIN_INTERFACE_MAP(CAutoBreakpoint, CAutoObj)
  INTERFACE_PART(CAutoBreakpoint, IID_IDispBreakpoint, Dispatch)
  INTERFACE_PART(CAutoBreakpoint, IID_IBreakpoint, Dispatch)
  DUAL_ERRORINFO_PART(CAutoBreakpoint)
END_INTERFACE_MAP()

IMPLEMENT_DUAL_ERRORINFO(CAutoBreakpoint, IID_IBreakpoint)
DS_IMPLEMENT_ENABLE_DUAL(CAutoBreakpoint, Breakpoint)
DS_DELEGATE_DUAL_INTERFACE(CAutoBreakpoint, Breakpoint)


BOOL CAutoBreakpoint::NoBP(void) // if associated BP released throws OLE error
{
	if (!m_pbpn) 
	{
		DsThrowOleDispatchException(
		  DS_E_BP_REMOVED, // HRESULT
			IDS_ERR_BP_REMOVED); // message resource ID
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CAutoBreakpoint message handlers


LPDISPATCH CAutoBreakpoint::GetApplication() 
{
	return theApp.m_pAutoApp->GetIDispatch(TRUE); // give 'em the app object if no frame
}

LPDISPATCH CAutoBreakpoint::GetParent() 
{
	ASSERT(pAutoDebugger);
	return pAutoDebugger->GetIDispatch(TRUE);
}

#if 0
// We have a valid breakpoint object if it is associated with a BP Node, 
// whether or not it is enabled, etc.
BOOL CAutoBreakpoint::GetIsValid() 
{
	return m_pbpn != 0;
}
#endif

BOOL CAutoBreakpoint::GetEnabled() 
{
	if (NoBP()) 
		return FALSE;
	return pbpnEnabled(m_pbpn);
}

void CAutoBreakpoint::SetEnabled(BOOL bNewValue) 
{
	if (NoBP())
		return;
		
	pbpnEnabled(m_pbpn) = bNewValue;
}

BSTR CAutoBreakpoint::GetLocation() 
{
	if (NoBP())
		return NULL;
		
	char sz[250];
	BPCONTEXT *pBPC = &m_pbpn->bpl.Cxt;

	if (m_pbpn->bpl.LocType == BPLOCFILELINE)
	{
		wsprintf(sz, ".%d", m_pbpn->bpl.Loc.l.InitLine);
	}
	else 
	{
		lstrcpy(sz, m_pbpn->bpl.Loc.ae.AddrExpr);
	}

	CString strResult(sz);
	return strResult.AllocSysString();
}

BSTR CAutoBreakpoint::GetFile() 
{
	if (NoBP())
		return NULL;
		
	CString strResult(m_pbpn->bpl.Cxt.Source);
	return strResult.AllocSysString();
}

BSTR CAutoBreakpoint::GetFunction() 
{
	if (NoBP())
		return NULL;

	CString strResult(m_pbpn->bpl.Cxt.Function);
	return strResult.AllocSysString();
}

#if 0
void CAutoBreakpoint::SetFunction(LPCTSTR lpszNewValue) 
{
	if (NoBP())
		return;;

	lstrcpy(m_pbpn->bpl.Cxt.Function, lpszNewValue);
}
#endif

BSTR CAutoBreakpoint::GetExecutable() 
{
	if (NoBP())
		return NULL;

	CString strResult(m_pbpn->bpl.Cxt.Exe);
	return strResult.AllocSysString();
}

BSTR CAutoBreakpoint::GetCondition() 
{
	if (NoBP())
		return NULL;
		
	CString strResult(m_pbpn->bpe.ne.Expr);
	return strResult.AllocSysString();
}

void CAutoBreakpoint::SetCondition(LPCTSTR lpszNewValue) 
{
	if (NoBP())
		return;;

	lstrcpy(m_pbpn->bpe.ne.Expr, lpszNewValue);
}

long CAutoBreakpoint::GetElements() 
{
	if (NoBP())
		return 0;

	return m_pbpn->bpe.ne.Len;
}

long CAutoBreakpoint::GetPassCount() 
{
	if (NoBP())
		return 0;

	return m_pbpn->cPass;
}

long CAutoBreakpoint::GetMessage() 
{
	if (NoBP())
		return 0;
		
	if (m_pbpn->Type != dsMessage)
		return 0;

	return m_pbpn->bpe.we.MessageNum;
}

BSTR CAutoBreakpoint::GetWindowProcedure() 
{
	if (NoBP())
		return NULL;

	if (m_pbpn->Type != dsMessage)
		return NULL;
		
	CString strResult(m_pbpn->bpl.Loc.wpe.WndProcName);
	return strResult.AllocSysString();
}

long CAutoBreakpoint::GetType() 
{
	if (NoBP())
		return 0;

	return m_pbpn->Type;
}

// Remove this breakpoint node, and associated object
void CAutoBreakpoint::Remove() 
{
	if (NoBP())
		return;

	// If we still have a breakpoint node, free it
	if (m_pbpn) {
		// DeleteBreakpointNode will call back and remove the automation object
		DeleteBreakpointNode(m_pbpn);
		m_pbpn = 0;
	}
}

// This breakpoint has been removed, and should be deleted from the collection
void CAutoBreakpoint::BPBeGone()
{
	// associated BP node is gone
	m_pbpn = 0;

	// now remove me from the collection.  This may drop refcount to 0,
	// in which case 'this' will soon get deleted
	if (pAutoBPS) pAutoBPS->RemoveBP(this);
}

#endif DEBUGGER_AUTOMATION

