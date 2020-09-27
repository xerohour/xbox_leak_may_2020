// AutoWindowDefault.cpp : implementation file
//

#include "stdafx.h"
#include "AutoWinD.h"
#include "autoguid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// Delegate to Dispatch implementation

// properties

DELEGATE_BSTR_GET(CAutoWindowDefault,GenericWindow, Caption, Caption)
DELEGATE_BSTR_GET(CAutoWindowDefault,GenericWindow, Type, Type)

DELEGATE_BOOL_PUT(CAutoWindowDefault,GenericWindow,Active)

DELEGATE_BOOL_GET(CAutoWindowDefault,GenericWindow,Active)

DELEGATE_GET(CAutoWindowDefault,GenericWindow,Left,long)
DELEGATE_GET(CAutoWindowDefault,GenericWindow,Top,long)
DELEGATE_GET(CAutoWindowDefault,GenericWindow,Height,long)
DELEGATE_GET(CAutoWindowDefault,GenericWindow,Width,long)
DELEGATE_GET(CAutoWindowDefault,GenericWindow,Index,long)
DELEGATE_GET(CAutoWindowDefault,GenericWindow,WindowState,DsWindowState)

DELEGATE_PUT(CAutoWindowDefault,GenericWindow,Left,long)
DELEGATE_PUT(CAutoWindowDefault,GenericWindow,Top,long)
DELEGATE_PUT(CAutoWindowDefault,GenericWindow,Height,long)
DELEGATE_PUT(CAutoWindowDefault,GenericWindow,Width,long)
DELEGATE_PUT(CAutoWindowDefault,GenericWindow,WindowState,DsWindowState)

DELEGATE_GET(CAutoWindowDefault,GenericWindow,Next,LPDISPATCH)
DELEGATE_GET(CAutoWindowDefault,GenericWindow,Previous,LPDISPATCH)
DELEGATE_GET(CAutoWindowDefault,GenericWindow,Application,LPDISPATCH)
DELEGATE_GET(CAutoWindowDefault,GenericWindow,Parent,LPDISPATCH)

DELEGATE_ONE_PARAM_WITH_RETVAL(CAutoWindowDefault,GenericWindow,Close,VARIANT, DsSaveStatus)

/////////////////////////////////////////////////////////////////////////////
// CAutoWindowDefault

IMPLEMENT_DYNAMIC(CAutoWindowDefault, CAutoWindowObj)

CAutoWindowDefault::CAutoWindowDefault(CPartFrame* pFrame)
{
	ASSERT( pFrame != NULL) ;
	m_pAssociatedPartFrame = pFrame ;

#ifdef _DUAL_
	EnableDualAutomation();
#else
	EnableAutomation() ;
#endif
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CAutoWindowDefault::~CAutoWindowDefault()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.

	if (m_pAssociatedPartFrame != NULL)
	{
		m_pAssociatedPartFrame->AutomationObjectReleased() ;
	}

	AfxOleUnlockApp();
}

//
// CPartFrame and CPartDoc call this function to inform us
// that they are being deleted.
//
void CAutoWindowDefault::AssociatedObjectReleased()
{
	m_pAssociatedPartFrame = NULL ;
}



void CAutoWindowDefault::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CAutoWindowObj::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CAutoWindowDefault, CAutoWindowObj)
	//{{AFX_MSG_MAP(CAutoWindowDefault)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoWindowDefault, CAutoWindowObj)
	//{{AFX_DISPATCH_MAP(CAutoWindowDefault)
	DISP_PROPERTY_EX(CAutoWindowDefault, "Caption", GetCaption, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CAutoWindowDefault, "Type", GetType, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CAutoWindowDefault, "Active", GetActive, SetActive, VT_BOOL)
	DISP_PROPERTY_EX(CAutoWindowDefault, "Left", GetLeft, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoWindowDefault, "Top", GetTop, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoWindowDefault, "Height", GetHeight, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoWindowDefault, "Width", GetWidth, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoWindowDefault, "Index", GetIndex, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoWindowDefault, "Next", GetNext, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoWindowDefault, "Previous", GetPrevious, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoWindowDefault, "WindowState", GetWindowState, SetWindowState, VT_I4)
	DISP_PROPERTY_EX(CAutoWindowDefault, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoWindowDefault, "Parent", GetParent, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION(CAutoWindowDefault, "Close", Close, VT_I4, VTS_VARIANT)
	DISP_DEFVALUE(CAutoWindowDefault, "Caption")
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IAutoViewDefault to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

/*
// {E1870223-933A-11CF-9BF9-00A0C90A632C}
static const IID IID_IAutoViewDefault =
{ 0xe1870223, 0x933a, 0x11cf, { 0x9b, 0xf9, 0x0, 0xa0, 0xc9, 0xa, 0x63, 0x2c } };
*/

BEGIN_INTERFACE_MAP(CAutoWindowDefault, CAutoWindowObj)
	INTERFACE_PART(CAutoWindowDefault, IID_IDispGenericWindow, Dispatch)
	INTERFACE_PART(CAutoWindowDefault, IID_IGenericWindow, Dispatch)
#ifdef _DUAL_
	DUAL_ERRORINFO_PART(CAutoWindowDefault)
#endif
END_INTERFACE_MAP()

#ifdef _DUAL_
IMPLEMENT_DUAL_ERRORINFO(CAutoWindowDefault, IID_IGenericWindow)
DS_IMPLEMENT_ENABLE_DUAL(CAutoWindowDefault, GenericWindow)
DS_DELEGATE_DUAL_INTERFACE(CAutoWindowDefault, GenericWindow)
DS_IMPLEMENT_VTBL_PAD_10(CAutoWindowDefault, GenericWindow)
#endif

/* See DualImpl.doc
// {E1870224-933A-11CF-9BF9-00A0C90A632C}
IMPLEMENT_OLECREATE(CAutoWindowDefault, "VSHELL7.AUTOVIEWDEFAULT", 0xe1870224, 0x933a, 0x11cf, 0x9b, 0xf9, 0x0, 0xa0, 0xc9, 0xa, 0x63, 0x2c)
*/


/////////////////////////////////////////////////////////////////////////////
// This function checks to see if the associated frame is still alive.
// If it is not alive, it returns TRUE. If it is alive it throws an exception 
// and returns false.
void CAutoWindowDefault::NoFrame() 
{
	if (m_pAssociatedPartFrame == NULL) 
	{
		DsThrowShellOleDispatchException(DS_E_WINDOW_RELEASED); 
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAutoWindowDefault message handlers

BSTR CAutoWindowDefault::GetCaption() 
{
	NoFrame();
	return m_pAssociatedPartFrame->GetCaption() ;
}


LPDISPATCH CAutoWindowDefault::GetApplication()
{	
	return theApp.m_pAutoApp->GetIDispatch(TRUE);
}

LPDISPATCH CAutoWindowDefault::GetParent()
{
	NoFrame();

	// Get the the view for this frame....
	CView *pView = m_pAssociatedPartFrame->GetActiveView();
	ASSERT(pView);

	// Get the document for this view.
	CPartDoc* pDoc = static_cast<CPartDoc*>(pView->GetDocument());

	// Return the IDispatch pointer...
	return pDoc->GetAutomationObject(TRUE);
}

BSTR CAutoWindowDefault::GetType() 
{
	CString strResult;
	strResult = "Generic" ;
	return strResult.AllocSysString();
}


BOOL CAutoWindowDefault::GetActive() 
{
	NoFrame();
	return m_pAssociatedPartFrame->GetActive() ;
}

void CAutoWindowDefault::SetActive(BOOL bNewValue) 
{
	NoFrame();
	m_pAssociatedPartFrame->SetActive(bNewValue) ;
}

long CAutoWindowDefault::GetLeft() 
{
	NoFrame();
	return m_pAssociatedPartFrame->GetLeft() ;
}

void CAutoWindowDefault::SetLeft(long nNewValue) 
{
	NoFrame();
	m_pAssociatedPartFrame->SetLeft(nNewValue) ;
}

long CAutoWindowDefault::GetTop() 
{
	NoFrame();
	return m_pAssociatedPartFrame->GetTop() ;
}

void CAutoWindowDefault::SetTop(long nNewValue) 
{
	NoFrame();
	m_pAssociatedPartFrame->SetTop(nNewValue) ;
}

long CAutoWindowDefault::GetHeight() 
{
	NoFrame();
	return m_pAssociatedPartFrame->GetHeight() ;
}

void CAutoWindowDefault::SetHeight(long nNewValue) 
{
	NoFrame();
	m_pAssociatedPartFrame->SetHeight(nNewValue) ;
}

long CAutoWindowDefault::GetWidth() 
{
	NoFrame();
	return m_pAssociatedPartFrame->GetWidth() ;
}

void CAutoWindowDefault::SetWidth(long nNewValue) 
{
	NoFrame();
	m_pAssociatedPartFrame->SetWidth(nNewValue) ;
}

long CAutoWindowDefault::GetIndex() 
{
	NoFrame();
	return m_pAssociatedPartFrame->GetIndex();
}

LPDISPATCH CAutoWindowDefault::GetNext() 
{
	NoFrame();
	return m_pAssociatedPartFrame->GetNext() ;
}


LPDISPATCH CAutoWindowDefault::GetPrevious() 
{
	NoFrame();
	return m_pAssociatedPartFrame->GetPrevious() ;
}


DsWindowState CAutoWindowDefault::GetWindowState() 
{
	NoFrame();
	return m_pAssociatedPartFrame->GetWindowState() ;
}

void CAutoWindowDefault::SetWindowState(DsWindowState nNewValue) 
{
	NoFrame();
	m_pAssociatedPartFrame->SetWindowState(nNewValue) ;
}

DsSaveStatus CAutoWindowDefault::Close(const VARIANT FAR& vtSaveChanges) 
{
	// TODO: Add your dispatch handler code here
	NoFrame();
	return m_pAssociatedPartFrame->AutomationClose(vtSaveChanges) ;
}

