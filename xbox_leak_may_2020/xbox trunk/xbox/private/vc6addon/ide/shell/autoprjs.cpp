// AutoPrjs.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPrjs.h"
#include "autoguid.h"
#include <main.h>
#include <ObjModel\appauto.h>
#include "toolexpt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoProjects dual implementation

STDMETHODIMP CAutoProjects::XProjects::get_Application(IDispatch * FAR* Application)
{
	METHOD_PROLOGUE(CAutoProjects, Dispatch)
	
	TRY_DUAL(IID_IProjects)
	{
		*Application = pThis->GetApplication();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoProjects::XProjects::get_Count(long FAR* Count)
{
	METHOD_PROLOGUE(CAutoProjects, Dispatch)
	
	TRY_DUAL(IID_IProjects)
	{
		*Count = pThis->GetCount();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoProjects::XProjects::get_Parent(IDispatch * FAR* Parent)
{
	METHOD_PROLOGUE(CAutoProjects, Dispatch)
	
	TRY_DUAL(IID_IProjects)
	{
		*Parent = pThis->GetParent();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoProjects::XProjects::get__NewEnum(IUnknown * FAR* _NewEnum)
{
	METHOD_PROLOGUE(CAutoProjects, Dispatch)
	
	TRY_DUAL(IID_IProjects)
	{
		*_NewEnum = pThis->GetNewEnum();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoProjects::XProjects::Item(VARIANT Index, IGenericProject FAR* FAR* Item)
{
	METHOD_PROLOGUE(CAutoProjects, Dispatch)
	
	TRY_DUAL(IID_IProjects)
	{
		*Item = (IGenericProject*) pThis->Item(Index);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

/////////////////////////////////////////////////////////////////////////////
// CAutoProjects
// This class represents the collection of all projects in the currently
//  open workspace.  An instance of this class is created by 
//  CBldPackage::GetPackageExtension in vproj.cpp (via CAutoProjects::Create).

IMPLEMENT_DYNCREATE(CAutoProjects, CAutoObj)

CAutoProjects::CAutoProjects()
{
	EnableDualAutomation();

	m_pPtrlProjects = NULL;
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CAutoProjects::~CAutoProjects()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
}


void CAutoProjects::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	// Release each individual project & the collection
	POSITION pos = m_pPtrlProjects->GetHeadPosition();
	while (pos != NULL)
	{
		IDispatch* pDispatch = m_pPtrlProjects->GetNext(pos);
		ASSERT (pDispatch != NULL);
		pDispatch->Release() ;
	}

	// The collection is in charge of cleaning this up.
	delete m_pPtrlProjects ;
	m_pPtrlProjects = NULL;


	CAutoObj::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CAutoProjects, CAutoObj)
	//{{AFX_MSG_MAP(CAutoProjects)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoProjects, CAutoObj)
	//{{AFX_DISPATCH_MAP(CAutoProjects)
	DISP_PROPERTY_EX(CAutoProjects, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoProjects, "Count", GetCount, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoProjects, "Parent", GetParent, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION(CAutoProjects, "Item", Item, VT_DISPATCH, VTS_VARIANT)
	DISP_DEFVALUE(CAutoProjects, "Item")
	//}}AFX_DISPATCH_MAP
	DISP_PROPERTY_EX_ID(CAutoProjects, "_NewEnum", DISPID_NEWENUM, GetNewEnum, SetNotSupported, VT_UNKNOWN)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CAutoProjects, CAutoObj)
 	INTERFACE_PART(CAutoProjects, IID_IDispProjects, Dispatch)
 	INTERFACE_PART(CAutoProjects, IID_IProjects, Dispatch)
    DUAL_ERRORINFO_PART(CAutoProjects)
END_INTERFACE_MAP()

// Implement ISupportErrorInfo to indicate we support the 
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CAutoProjects, IID_IProjects)
DS_IMPLEMENT_ENABLE_DUAL(CAutoProjects, Projects)
DS_DELEGATE_DUAL_INTERFACE(CAutoProjects, Projects)

/////////////////////////////////////////////////////////////////////////////
// CAutoProjects message handlers

LPDISPATCH CAutoProjects::GetApplication() 
{
	ASSERT(theApp.m_pAutoApp != NULL) ;
    return theApp.m_pAutoApp->GetIDispatch(TRUE);
}

long CAutoProjects::GetCount() 
{
	IProjectWorkspace *pPW = g_IdeInterface.GetProjectWorkspace();
	CADWORD PkgProjects;
	VERIFY(SUCCEEDED(pPW->GetLoadedProjects(&PkgProjects)));

	// Release the CADWORD now
	for(int i=0; i<PkgProjects.cElems; i++)
	{
		((IPkgProject *)PkgProjects.pElems[i])->Release();
	}
	CoTaskMemFree((void *)PkgProjects.pElems);

	return PkgProjects.cElems;
}

LPUNKNOWN CAutoProjects::GetNewEnum() 
{
	// This is a live collection, so we need to update our
	// list of projects every time we look at them.
	BuildNewList();

	CEnumVariantObjs* pEnumVariant = new CEnumVariantObjs(m_pPtrlProjects);
	
	if (pEnumVariant == NULL)
		DsThrowCannedOleDispatchException(E_OUTOFMEMORY);

	// We don't QueryInterface, since we don't want the IEnumVARIANT
	//  AddRef'd.  The "new CEnumVariantObjs" above already AddRefs it for
	//  us.
	return &pEnumVariant->m_xEnumVariant;
}

LPDISPATCH CAutoProjects::GetParent() 
{
	// The parent of the Projects collection is the Application object
	ASSERT(theApp.m_pAutoApp != NULL) ;
    return theApp.m_pAutoApp->GetIDispatch(TRUE);
}

LPDISPATCH CAutoProjects::Item(const VARIANT FAR& index) 
{
	// Check to see if there isn't a parameter.
	if (index.vt == VT_ERROR)
	{
		// Parameter is optional and is not supplied.
		// Return a pointer to this collection.
		DsThrowCannedOleDispatchException(E_INVALIDARG);
	}
	// This is a live collection, so we need to update our
	// list of projects every time we look at them.
	BuildNewList();

	// Check to see if the parameter is a string
	if (index.vt == VT_BSTR)
	{
		LPDISPATCH pDispRetProject = NULL;
		POSITION pos = m_pPtrlProjects->GetHeadPosition();
		while (pos != NULL)
		{
			IGenericProject *pCurrProject = (IGenericProject *)m_pPtrlProjects->GetNext(pos);
			BSTR bstrName;
			pCurrProject->get_Name(&bstrName);
			if(wcscmp(bstrName, index.bstrVal) == 0)
			{
				pDispRetProject = pCurrProject;
				pDispRetProject->AddRef();
			}
			::SysFreeString(bstrName);
			if(pDispRetProject != NULL)
				break;
		}
		return pDispRetProject ;
	}

    // coerce index to VT_I4
	COleVariant coercedIndex(index);
	coercedIndex.ChangeType(VT_I4);
 	IDispatch* pObj = NULL ;
	POSITION pos = NULL;
	// Subtract 1 from index, since we expect a 1-based index.
	if(coercedIndex.lVal > 0)
	    pos = m_pPtrlProjects->FindIndex(coercedIndex.lVal-1);
    if (pos != NULL)
	{
        pObj = m_pPtrlProjects->GetAt(pos);
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

CAutoProjects* CAutoProjects::Create()
{
	// Make a new CAutoProjects
	CAutoProjects* pProjects = new CAutoProjects;
	pProjects->m_pPtrlProjects = new CDispatchList;
	return pProjects;
}

void CAutoProjects::BuildNewList()
{
	ASSERT(m_pPtrlProjects != NULL);
	POSITION pos = m_pPtrlProjects->GetHeadPosition();
	while (pos != NULL)
	{
		IGenericProject *pCurrProject = (IGenericProject *)m_pPtrlProjects->GetNext(pos);
		pCurrProject->Release();
	}

	// Now empty the list to prepare for the new elements.
	m_pPtrlProjects->RemoveAll();

	// Enumerate through Projects, create a CAutoProject for each,
	//  and put them all in m_pPtrlProjects.
	IProjectWorkspace *pPW = g_IdeInterface.GetProjectWorkspace();
	CADWORD PkgProjects;
	VERIFY(SUCCEEDED(pPW->GetLoadedProjects(&PkgProjects)));
	for(int i=0; i<PkgProjects.cElems; i++)
	{
		LPDISPATCH pDisp;
		VERIFY(SUCCEEDED(((IPkgProject *)PkgProjects.pElems[i])->GetAutomationObject(&pDisp)));
		((IPkgProject *)PkgProjects.pElems[i])->Release();
		m_pPtrlProjects->AddTail(pDisp);
	}
	// Free the CADWORD now
	CoTaskMemFree((void *)PkgProjects.pElems);
}
