// $$dlgautoproxy_ifile$$.cpp : implementation file
//

#include "stdafx.h"
#include "$$root$$.h"
#include "$$dlgautoproxy_hfile$$.h"
#include "$$dlg_hfile$$.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$DLGAUTOPROXY_CLASS$$

IMPLEMENT_DYNCREATE($$DLGAUTOPROXY_CLASS$$, $$DLGAUTOPROXY_BASE_CLASS$$)

$$DLGAUTOPROXY_CLASS$$::$$DLGAUTOPROXY_CLASS$$()
{
	EnableAutomation();
	
	// To keep the application running as long as an automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

$$IF(VERBOSE)
	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
$$ENDIF //VERBOSE
	ASSERT (AfxGetApp()->m_pMainWnd != NULL);
	ASSERT_VALID (AfxGetApp()->m_pMainWnd);
	ASSERT_KINDOF($$DLG_CLASS$$, AfxGetApp()->m_pMainWnd);
	m_pDialog = ($$DLG_CLASS$$*) AfxGetApp()->m_pMainWnd;
	m_pDialog->m_pAutoProxy = this;
}

$$DLGAUTOPROXY_CLASS$$::~$$DLGAUTOPROXY_CLASS$$()
{
$$IF(VERBOSE)
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
$$ENDIF //VERBOSE
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void $$DLGAUTOPROXY_CLASS$$::OnFinalRelease()
{
$$IF(VERBOSE)
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

$$ENDIF //VERBOSE
	$$DLGAUTOPROXY_BASE_CLASS$$::OnFinalRelease();
}

BEGIN_MESSAGE_MAP($$DLGAUTOPROXY_CLASS$$, $$DLGAUTOPROXY_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$DLGAUTOPROXY_CLASS$$)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP($$DLGAUTOPROXY_CLASS$$, $$DLGAUTOPROXY_BASE_CLASS$$)
	//{{AFX_DISPATCH_MAP($$DLGAUTOPROXY_CLASS$$)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_I$$Safe_root$$ to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {$$DISPIID_CLSID_ODL$$}
static const IID IID_I$$Safe_root$$ =
$$DISPIID_CLSID$$;

BEGIN_INTERFACE_MAP($$DLGAUTOPROXY_CLASS$$, $$DLGAUTOPROXY_BASE_CLASS$$)
	INTERFACE_PART($$DLGAUTOPROXY_CLASS$$, IID_I$$Safe_root$$, Dispatch)
END_INTERFACE_MAP()

$$IF(VERBOSE)
// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
$$ENDIF //VERBOSE
// {$$APP_CLSID_REG$$}
IMPLEMENT_OLECREATE2($$DLGAUTOPROXY_CLASS$$, "$$Safe_root$$.Application", $$APP_CLSID_MACRO$$)

/////////////////////////////////////////////////////////////////////////////
// $$DLGAUTOPROXY_CLASS$$ message handlers
