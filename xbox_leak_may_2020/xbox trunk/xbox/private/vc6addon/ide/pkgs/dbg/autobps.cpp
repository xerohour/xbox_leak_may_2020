// AutoBps.cpp : implementation file
//

#include "stdafx.h"

#ifdef DEBUGGER_AUTOMATION

#include <utilauto.h>
#include "AutoBps.h"
#include "autodbg.h"
#include "autobp.h"
#include "resource.h"
#include "privguid.h"
#include <ObjModel\dbgguid.h>
#include "autoguid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
void DBprintf(const char *format, ...) {
	char buf[500];
	va_list ap;
	
	va_start(ap, format);
	wvsprintf(buf, format, ap);
	OutputDebugString(buf);
	va_end(ap);
} // dbprintf

#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoBreakpoints

//IMPLEMENT_DYNCREATE(CAutoBreakpoints, CAutoObj)
IMPLEMENT_DYNAMIC(CAutoBreakpoints, CAutoObj)

#ifdef _DEBUG
int nAutoBreakpoints;
#endif

class CAutoBreakpoints *pAutoBPS;

CAutoBreakpoints::CAutoBreakpoints()
{

	dbprintf(("  +BPS %x (#%d)\n", this, ++nAutoBreakpoints));

	pAutoBPS = this;
	
	EnableDualAutomation();
//	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Create a list to hold snapshot of breakpoints
	m_pPtrlData = new CDispatchList(20);

	// For each breakpoint in the system
    PBREAKPOINTNODE pbpn = BHFirstBPNode();
    while( pbpn )
    {
    	// Find out if it already has an automation object
    	CAutoBreakpoint *pbp = pbpn->pAutoBP;

		// if breakpoint exists, add to collection, and inc refcount
    	if (pbp) 
    	{
    		m_pPtrlData->AddTail(pbp->GetIDispatch(TRUE));
    	}
    	else // If not, create a new one
    	{
    		// This automatically adds it to this collection, and incs refcount
	    	pbp = new CAutoBreakpoint(pbpn);
		}
        pbpn = pbpnNext( pbpn );
    }

    // Hi, Dad (or Mom)
	pAutoDebugger->GetIDispatch(TRUE);
}


CAutoBreakpoints::~CAutoBreakpoints()
{
	dbprintf(("  -BPS %x (#%d)\n", this, nAutoBreakpoints--));

	POSITION pos = m_pPtrlData ? m_pPtrlData->GetHeadPosition() : NULL;

	// Since we are destroying the collection, we need to remove
	// all AutoBPs held in the collection.  This will dec their
	// refcount, and cause them to go away unless they are
	// reff'ed by an external agent.

	// Only do this for breakpoints left in the collection
	while (pos != NULL)
	{
		IDispatch* pDispatch = m_pPtrlData->GetNext(pos);
		ASSERT (pDispatch != NULL);
		CAutoBreakpoint *pbp = 0;
		HRESULT hr = pDispatch->QueryInterface(IID_IBreakpoint, (void**)&pbp) ; 

		// QueryInterface generates a reference we need to release
		if (pbp) {
			pDispatch->Release();
		}

		// I'm not getting the right value for the interface here!
		if (pbp && ((void *)pbp == (void *)pDispatch)) { // BUGBUG
			pbp = (CAutoBreakpoint *)((char *)pbp - 0x10);
		}
		ASSERT((void *)pbp != (void *)pDispatch);

		dbprintf(( "  ?BPS Release BP %x\n", pDispatch));
		
		pDispatch->Release();
	}

	// Nuke the global collection pointer
	pAutoBPS = 0;
	
	// The collection is in charge of cleaning this up.
	delete m_pPtrlData ;

	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();

	// Bye, Dad (or Mom)
	IDispatch *pDispatch = pAutoDebugger->GetIDispatch(FALSE);
	pDispatch->Release();
}


void CAutoBreakpoints::OnFinalRelease()
{
	dbprintf(( "  <BPS %x (OnFinalRelease)\n", this));
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CAutoObj::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CAutoBreakpoints, CAutoObj)
	//{{AFX_MSG_MAP(CAutoBreakpoints)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoBreakpoints, CAutoObj)
	//{{AFX_DISPATCH_MAP(CAutoBreakpoints)
	DISP_PROPERTY_EX(CAutoBreakpoints, "Count", GetCount, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoBreakpoints, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoBreakpoints, "Parent", GetParent, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION(CAutoBreakpoints, "Item", Item, VT_DISPATCH, VTS_VARIANT)
	DISP_FUNCTION(CAutoBreakpoints, "RemoveAllBreakpoints", RemoveAllBreakpoints, VT_EMPTY, VTS_NONE)
	// link: private virtual xBreakpoints
	DISP_FUNCTION(CAutoBreakpoints, "RemoveBreakpointAtLine", RemoveBreakpointAtLine, VT_BOOL, VTS_VARIANT)
	DISP_FUNCTION(CAutoBreakpoints, "AddBreakpointAtLine", AddBreakpointAtLine, VT_DISPATCH, VTS_VARIANT)
	DISP_DEFVALUE(CAutoBreakpoints, "Item")
	//}}AFX_DISPATCH_MAP
	DISP_PROPERTY_EX_ID(CAutoBreakpoints, "Get_NewEnum", DISPID_NEWENUM, Get_NewEnum, SetNotSupported, VT_UNKNOWN)
END_DISPATCH_MAP()


BEGIN_INTERFACE_MAP(CAutoBreakpoints, CAutoObj)
	INTERFACE_PART(CAutoBreakpoints, IID_IDispBreakpoints, Dispatch)
	INTERFACE_PART(CAutoBreakpoints, IID_IBreakpoints, Dispatch)
   DUAL_ERRORINFO_PART(CAutoBreakpoints)
END_INTERFACE_MAP()

// Implement ISupportErrorInfo to indicate we support the 
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CAutoBreakpoints, IID_IBreakpoints)
DS_IMPLEMENT_ENABLE_DUAL(CAutoBreakpoints, Breakpoints)
DS_DELEGATE_DUAL_INTERFACE(CAutoBreakpoints, Breakpoints)


/////////////////////////////////////////////////////////////////////////////
// CAutoBreakpoints message handlers

long CAutoBreakpoints::GetCount() 
{
	return m_pPtrlData->GetCount();
}

LPUNKNOWN CAutoBreakpoints::Get_NewEnum() 
{
	CEnumVariantObjs* pEnumVariant = new CEnumVariantObjs(m_pPtrlData);
	
	if (pEnumVariant == NULL)
		DsThrowCannedOleDispatchException(E_OUTOFMEMORY);

	// We don't QueryInterface, since we don't want the IEnumVARIANT
	//  AddRef'd.  The "new CEnumVariantObjs" above already AddRefs it for
	//  us.
	return &pEnumVariant->m_xEnumVariant;
}

LPDISPATCH CAutoBreakpoints::Item(const VARIANT FAR& index) 
{
	// Check to see if there isn't a parameter.
	if (index.vt == VT_ERROR)
	{
		// Parameter is optional and is not supplied.
		// Return a pointer to this collection.
		return GetIDispatch(TRUE) ;
	}

	// Check to see if the parameter is a string
	if (index.vt == VT_BSTR)
	{
		return NULL ;
	}

    // coerce index to VT_I4
	COleVariant coercedIndex(index);
	coercedIndex.ChangeType(VT_I4);
 	IDispatch* pObj = NULL ;
    POSITION pos = m_pPtrlData->FindIndex(coercedIndex.lVal);
    if (pos != NULL)
	{
        pObj = m_pPtrlData->GetAt(pos);
		ASSERT (pObj != NULL);
	}
	else
	{
		DsThrowCannedOleDispatchException(E_INVALIDARG);
	}
    
	ASSERT (pObj != NULL);
	pObj->AddRef();
	return pObj ;
}

LPDISPATCH CAutoBreakpoints::GetApplication() 
{

	ASSERT(theApp.m_pAutoApp != NULL) ;
    return theApp.m_pAutoApp->GetIDispatch(TRUE);
}

LPDISPATCH CAutoBreakpoints::GetParent() 
{
	ASSERT(pAutoDebugger);
	return pAutoDebugger->GetIDispatch(TRUE);
}

void CAutoBreakpoints::RemoveAllBreakpoints() 
{
	dbprintf(( "  =BPS %x Remove All\n", this));
	
	POSITION pos = m_pPtrlData->GetHeadPosition();
	CAutoBreakpoint *pbp = 0;
	
	while (pos != NULL)
	{
		IDispatch* pDispatch = m_pPtrlData->GetNext(pos);
		ASSERT (pDispatch != NULL);
		pbp = 0;
		HRESULT hr = pDispatch->QueryInterface(IID_IBreakpoint, (void**)&pbp) ; 

		// QueryInterface generates a reference we need to release
		if (pbp) {
			pDispatch->Release();
		}

		// I'm not getting the right value for the interface here!
		if (pbp && ((void *)pbp == (void *)pDispatch)) { // BUGBUG
			pbp = (CAutoBreakpoint *)((char *)pbp - 0x10);
		}
		ASSERT((void *)pbp != (void *)pDispatch);
		if (pbp)
		{
			pbp->Remove();
		}
		else
		{
		// TODO Throw exception
		}
	}
}

// Convert variant to integer line number
// Return -1 if no line parameter specified
int VarEditorLine(const VARIANT FAR &Selection) 
{
	// Check to see if there isn't a parameter.
	if (Selection.vt == VT_ERROR)
	{
		// Parameter is optional and is not supplied.
		// Return a pointer to this collection.
		return -1;
	}

	if (Selection.vt == VT_I4)
	{
		return Selection.lVal;
	}
	
	// Check to see if the parameter is a string
	if (Selection.vt == VT_BSTR)
	{
		DsThrowCannedOleDispatchException(E_INVALIDARG);
	}

    // coerce index to VT_I4
	COleVariant coercedIndex(Selection);
	
	// BUGBUG what happens if we can't coerce the type?
	coercedIndex.ChangeType(VT_I4);
	
	return coercedIndex.lVal;
}

// Line is one based
BOOL CAutoBreakpoints::RemoveBreakpointAtLine(const VARIANT FAR& Selection) 
{
	PBREAKPOINTNODE pbpn = 0;
	CAutoBreakpoint *pbp;
	
	SetEditorLocBP(bplfind, &pbpn, VarEditorLine(Selection));

	if (!pbpn) 
		return FALSE;

	dbprintf(( "  =PBS %x/%x RemoveAtLine\n", this, pbpn->pAutoBP));
	
	// If we have a breakpoint object, tell it to remove itself
	if (pbpn->pAutoBP) 
	{
		pbpn->pAutoBP->Remove();
		return TRUE;	// Assume Remove() worked
	}

	// remove the low level breakpoint object manually
	return SetEditorLocBP(bplclear);
}

LPDISPATCH CAutoBreakpoints::AddBreakpointAtLine(const VARIANT FAR& Selection) 
{
    PBREAKPOINTNODE  pbpn;

	// Check if it is valid to create a breakpoint here
    if (!BHFIsValidWindowForBP())
    {
    	return NULL;
    }
	
    if (SetEditorLocBP(bplset, &pbpn, VarEditorLine(Selection)))
    {
    	CAutoBreakpoint *pbp = pbpn->pAutoBP;
    	ASSERT(pbp);

		return pbp->GetIDispatch(TRUE);
    }

	return NULL;
}

// Add this BP to current collection of BPs (because new BP node has been created)
void CAutoBreakpoints::AddBP(CAutoBreakpoint *pBP)
{
	m_pPtrlData->AddTail(pBP->GetIDispatch(FALSE));
}

// Remove this BP from collection of BPs (because BP node has been removed)
void CAutoBreakpoints::RemoveBP(CAutoBreakpoint *pBP)
{
	dbprintf(( "  ?BPS RemoveBP(%x) refcount was %d\n", pBP, pBP->GetRef()));

	// Remove BP from linked list of BPs in this collection
	IDispatch *pDispatch = pBP->GetIDispatch(FALSE);
	ASSERT(pDispatch);
	POSITION pos = m_pPtrlData->Find(pDispatch);
	ASSERT(pos);
	m_pPtrlData->RemoveAt(pos);

	// and dec refcount of BP object
	pDispatch->Release();
	dbprintf(("  ?BPS RemoveBP(%x) refcount now %d\n", pBP, pBP->GetRef()));
}

#endif // DEBUGGER_AUTOMATION
