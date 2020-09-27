// AutoDocD.cpp : implementation file
//

#include "stdafx.h"
#include "vshell.h"
#include "autoguid.h"
#include "AutoDocD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dual Interface Delegation

// methods
#ifdef _DUAL_

DELEGATE_BOOL_PUT(CAutoDocumentDefault,GenericDocument,Active)
DELEGATE_BOOL_GET(CAutoDocumentDefault,GenericDocument,Active)

DELEGATE_NO_PARAMS_WITH_BOOL_RETVAL(CAutoDocumentDefault, GenericDocument,PrintOut)
DELEGATE_NO_PARAMS_WITH_BOOL_RETVAL(CAutoDocumentDefault, GenericDocument,Undo)
DELEGATE_NO_PARAMS_WITH_BOOL_RETVAL(CAutoDocumentDefault, GenericDocument,Redo)

DELEGATE_NO_PARAMS_WITH_RETVAL(CAutoDocumentDefault, GenericDocument,NewWindow, LPDISPATCH)

DELEGATE_TWO_PARAMS_WITH_RETVAL(CAutoDocumentDefault, GenericDocument, Save, VARIANT, VARIANT, DsSaveStatus)
DELEGATE_ONE_PARAM_WITH_RETVAL(CAutoDocumentDefault, GenericDocument, Close, VARIANT, DsSaveStatus)

// properties

DELEGATE_BSTR_GET(CAutoDocumentDefault, GenericDocument, Name, Name)
DELEGATE_BSTR_GET(CAutoDocumentDefault, GenericDocument, FullName, FullName)

DELEGATE_BSTR_GET(CAutoDocumentDefault, GenericDocument, Path, Path)
DELEGATE_BSTR_GET(CAutoDocumentDefault, GenericDocument, Type, Type)

DELEGATE_GET(CAutoDocumentDefault, GenericDocument,Parent,LPDISPATCH)
DELEGATE_GET(CAutoDocumentDefault, GenericDocument,Application,LPDISPATCH)
DELEGATE_GET(CAutoDocumentDefault, GenericDocument,ActiveWindow,LPDISPATCH)
DELEGATE_GET(CAutoDocumentDefault, GenericDocument,Windows,LPDISPATCH)

DELEGATE_BOOL_GET(CAutoDocumentDefault, GenericDocument,Saved)
DELEGATE_BOOL_GET(CAutoDocumentDefault, GenericDocument,ReadOnly)
DELEGATE_BOOL_PUT(CAutoDocumentDefault, GenericDocument,ReadOnly)

#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoDocumentDefault

IMPLEMENT_DYNAMIC(CAutoDocumentDefault, CAutoObj)

CAutoDocumentDefault::CAutoDocumentDefault(CPartDoc* pDoc)
{
#ifdef _DUAL_
	EnableDualAutomation();
#else
	EnableAutomation() ;
#endif
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();

	ASSERT( pDoc != NULL) ;
	m_pDoc = pDoc ;
}

CAutoDocumentDefault::~CAutoDocumentDefault()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.

	AfxOleUnlockApp();

	if (m_pDoc != NULL)
	{
		m_pDoc->AutomationObjectReleased() ;
	}
}


//
// CPartFrame and CPartDoc call this function to inform us
// that they are being deleted.
//
void CAutoDocumentDefault::AssociatedObjectReleased()
{
	m_pDoc = NULL ;
}


void CAutoDocumentDefault::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CAutoObj::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CAutoDocumentDefault, CAutoObj)
	//{{AFX_MSG_MAP(CAutoDocumentDefault)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoDocumentDefault, CAutoObj)
	//{{AFX_DISPATCH_MAP(CAutoDocumentDefault)
	DISP_PROPERTY_EX(CAutoDocumentDefault, "Name", GetName, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CAutoDocumentDefault, "FullName", GetFullName, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CAutoDocumentDefault, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoDocumentDefault, "Parent", GetParent, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoDocumentDefault, "Path", GetPath, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CAutoDocumentDefault, "Saved", GetSaved, SetSaved, VT_BOOL)
	DISP_PROPERTY_EX(CAutoDocumentDefault, "Type", GetType, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX(CAutoDocumentDefault, "ReadOnly", GetReadOnly, SetReadOnly, VT_BOOL)
	DISP_PROPERTY_EX(CAutoDocumentDefault, "ActiveWindow", GetActiveWindow, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoDocumentDefault, "Windows", GetWindows, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoDocumentDefault, "Active", GetActive, SetActive, VT_BOOL)
	DISP_FUNCTION(CAutoDocumentDefault, "Close", Close, VT_I4, VTS_VARIANT)
	DISP_FUNCTION(CAutoDocumentDefault, "Save", Save, VT_I4, VTS_VARIANT)
	DISP_FUNCTION(CAutoDocumentDefault, "Redo", Redo, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAutoDocumentDefault, "Undo", Undo, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAutoDocumentDefault, "NewWindow", NewWindow, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CAutoDocumentDefault, "PrintOut", PrintOut, VT_BOOL, VTS_NONE)
	DISP_DEFVALUE(CAutoDocumentDefault, "Name")
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IAutoDocumentDefault to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

/* removed per dualimple.doc 
// {E1870221-933A-11CF-9BF9-00A0C90A632C}
static const IID IID_IAutoDocumentDefault =
{ 0xe1870221, 0x933a, 0x11cf, { 0x9b, 0xf9, 0x0, 0xa0, 0xc9, 0xa, 0x63, 0x2c } };
*/

BEGIN_INTERFACE_MAP(CAutoDocumentDefault, CAutoObj)
	INTERFACE_PART(CAutoDocumentDefault, IID_IDispGenericDocument, Dispatch)
	INTERFACE_PART(CAutoDocumentDefault, IID_IGenericDocument, Dispatch) 
#ifdef _DUAL_
	DUAL_ERRORINFO_PART(CAutoDocumentDefault)
#endif
END_INTERFACE_MAP()

#ifdef _DUAL_
IMPLEMENT_DUAL_ERRORINFO(CAutoDocumentDefault, IID_IGenericDocument)
DS_IMPLEMENT_ENABLE_DUAL(CAutoDocumentDefault, GenericDocument)
DS_DELEGATE_DUAL_INTERFACE(CAutoDocumentDefault, GenericDocument)
DS_IMPLEMENT_VTBL_PAD_10(CAutoDocumentDefault, GenericDocument)
#endif

/* Removed see DualImpl.doc
// {E1870222-933A-11CF-9BF9-00A0C90A632C}
IMPLEMENT_OLECREATE(CAutoDocumentDefault, "VSHELL7.AUTODOCUMENTDEFAULT", 0xe1870222, 0x933a, 0x11cf, 0x9b, 0xf9, 0x0, 0xa0, 0xc9, 0xa, 0x63, 0x2c)
*/

/////////////////////////////////////////////////////////////////////////////
// This function checks to see if the associated frame is still alive.
// If it is not alive, it returns TRUE. If it is alive it throws an exception 
// and returns false.
inline void CAutoDocumentDefault::NoDoc() 
{	
	if (m_pDoc == NULL) 
	{
		DsThrowShellOleDispatchException(DS_E_DOC_RELEASED); 
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAutoDocumentDefault message handlers

BSTR CAutoDocumentDefault::GetName() 
{
	NoDoc();
	return m_pDoc->GetName() ;
}


BSTR CAutoDocumentDefault::GetFullName() 
{	
	NoDoc();	
	return m_pDoc->GetFullName() ;
}


LPDISPATCH CAutoDocumentDefault::GetApplication() 
{
	NoDoc();
	return m_pDoc->GetApplication();
}


LPDISPATCH CAutoDocumentDefault::GetParent() 
{
	NoDoc();
	return m_pDoc->GetParent() ;
}


BSTR CAutoDocumentDefault::GetPath() 
{
	NoDoc();
	return m_pDoc->GetPath();
}


BOOL CAutoDocumentDefault::GetSaved() 
{
	NoDoc();
	return m_pDoc->GetSaved();
}

void CAutoDocumentDefault::SetSaved(BOOL bNewValue) 
{
	NoDoc();
	m_pDoc->SetSaved(bNewValue);
}

BOOL CAutoDocumentDefault::GetActive() 
{
	NoDoc();
	return m_pDoc->GetActive() ;
}

void CAutoDocumentDefault::SetActive(BOOL bNewValue) 
{
	NoDoc();
	m_pDoc->SetActive(bNewValue) ;
}


DsSaveStatus CAutoDocumentDefault::Close(const VARIANT FAR& longSaveChanges)
{
	NoDoc();
	return m_pDoc->Close(longSaveChanges); 
}


DsSaveStatus CAutoDocumentDefault::Save(const VARIANT FAR& vtFilename, const VARIANT FAR& longSaveChanges) 
{
	NoDoc();
	return m_pDoc->Save(vtFilename, longSaveChanges) ;
}

BSTR CAutoDocumentDefault::GetType() 
{
	CString strResult("Generic");
	return strResult.AllocSysString();
}


BOOL CAutoDocumentDefault::GetReadOnly() 
{
	NoDoc();
	return m_pDoc->ReadOnly();
}

void CAutoDocumentDefault::SetReadOnly(BOOL bReadOnly) 
{
	NoDoc();
	m_pDoc->SetReadOnly(bReadOnly);
}


LPDISPATCH CAutoDocumentDefault::GetActiveWindow() 
{
	NoDoc();
	return m_pDoc->ActiveWindow();
}

LPDISPATCH CAutoDocumentDefault::GetWindows() 
{
	NoDoc() ;
	return m_pDoc->GetWindows() ;
}


BOOL CAutoDocumentDefault::Redo() 
{
	// This is not and will not be implemented...return error!
	DsThrowCannedOleDispatchException(E_NOTIMPL);
	return FALSE;
}

BOOL CAutoDocumentDefault::Undo() 
{
	// This is not and will not be implemented...return error!
	DsThrowCannedOleDispatchException(E_NOTIMPL);
	return FALSE;
}

LPDISPATCH CAutoDocumentDefault::NewWindow() 
{
	// This is not and will not be implemented...return error!
	DsThrowCannedOleDispatchException(E_NOTIMPL);

	return NULL;
}

BOOL CAutoDocumentDefault::PrintOut() 
{
	// This is not and will not be implemented...return error!
	DsThrowCannedOleDispatchException(E_NOTIMPL);
	
	return FALSE;
}

