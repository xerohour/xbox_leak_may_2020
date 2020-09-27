// AutoCfgs.cpp : implementation file
//

#include "stdafx.h"
#include "autocfg.h"
#include "AutoCfgs.h"
#include "autoprj.h"
#include <utilauto.h>
#include <main.h>
#include <ObjModel\blddefs.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAutoConfigurations dual implementation

STDMETHODIMP CAutoConfigurations::XConfigurations::get_Application(IDispatch * FAR* Application)
{
	METHOD_PROLOGUE(CAutoConfigurations, Dispatch)
	
	TRY_DUAL(IID_IConfigurations)
	{
		*Application = pThis->GetApplication();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoConfigurations::XConfigurations::get_Count(long FAR* Count)
{
	METHOD_PROLOGUE(CAutoConfigurations, Dispatch)
	
	TRY_DUAL(IID_IConfigurations)
	{
		*Count = pThis->GetCount();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoConfigurations::XConfigurations::get_Parent(IBuildProject FAR* FAR* Parent)
{
	METHOD_PROLOGUE(CAutoConfigurations, Dispatch)
	
	TRY_DUAL(IID_IConfigurations)
	{
		*Parent = (IBuildProject*) pThis->GetParent();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoConfigurations::XConfigurations::get__NewEnum(IUnknown * FAR* _NewEnum)
{
	METHOD_PROLOGUE(CAutoConfigurations, Dispatch)
	
	TRY_DUAL(IID_IConfigurations)
	{
		*_NewEnum = pThis->GetNewEnum();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoConfigurations::XConfigurations::Item(VARIANT Index, IConfiguration FAR* FAR* Item)
{
	METHOD_PROLOGUE(CAutoConfigurations, Dispatch)
	
	TRY_DUAL(IID_IConfigurations)
	{
		*Item = (IConfiguration*) pThis->Item(Index);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


/////////////////////////////////////////////////////////////////////////////
// CAutoConfigurations

IMPLEMENT_DYNCREATE(CAutoConfigurations, CAutoObj)

CAutoConfigurations::CAutoConfigurations()
{
	EnableDualAutomation();
	m_pPtrlConfigs = NULL;
	m_hBld = NULL;
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CAutoConfigurations::~CAutoConfigurations()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
	g_LinkAutoObjToProject.OnAutoObjDestroyed(this);
}

void CAutoConfigurations::ReleaseConfigsInList()
{
	POSITION pos = m_pPtrlConfigs->GetHeadPosition();
	while (pos != NULL)
	{
		IDispatch* pDispatch = m_pPtrlConfigs->GetNext(pos);
		ASSERT (pDispatch != NULL);
		pDispatch->Release() ;
	}
	m_pPtrlConfigs->RemoveAll();
}
void CAutoConfigurations::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	// Release each individual configuration & the collection
	ReleaseConfigsInList();

	// The collection is in charge of cleaning this up.
	delete m_pPtrlConfigs ;
	m_pPtrlConfigs = NULL;


	CAutoObj::OnFinalRelease();
}

void CAutoConfigurations::AssociatedObjectReleased()
{
	ASSERT (m_hBld != NULL);
	m_hBld = NULL;
}


BEGIN_MESSAGE_MAP(CAutoConfigurations, CAutoObj)
	//{{AFX_MSG_MAP(CAutoConfigurations)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoConfigurations, CAutoObj)
	//{{AFX_DISPATCH_MAP(CAutoConfigurations)
	DISP_PROPERTY_EX(CAutoConfigurations, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoConfigurations, "Count", GetCount, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoConfigurations, "Parent", GetParent, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION(CAutoConfigurations, "Item", Item, VT_DISPATCH, VTS_VARIANT)
	DISP_DEFVALUE(CAutoConfigurations, "Item")
	//}}AFX_DISPATCH_MAP
	DISP_PROPERTY_EX_ID(CAutoConfigurations, "_NewEnum", DISPID_NEWENUM, GetNewEnum, SetNotSupported, VT_UNKNOWN)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CAutoConfigurations, CAutoObj)
 	INTERFACE_PART(CAutoConfigurations, IID_IDispConfigurations, Dispatch)
 	INTERFACE_PART(CAutoConfigurations, IID_IConfigurations, Dispatch)
    DUAL_ERRORINFO_PART(CAutoConfigurations)
END_INTERFACE_MAP()

// Implement ISupportErrorInfo to indicate we support the 
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CAutoConfigurations, IID_IConfigurations)
DS_IMPLEMENT_ENABLE_DUAL(CAutoConfigurations, Configurations)
DS_DELEGATE_DUAL_INTERFACE(CAutoConfigurations, Configurations)

/////////////////////////////////////////////////////////////////////////////
// CAutoConfigurations message handlers

LPDISPATCH CAutoConfigurations::GetApplication() 
{
	ASSERT(theApp.m_pAutoApp != NULL) ;
    return theApp.m_pAutoApp->GetIDispatch(TRUE);
}

long CAutoConfigurations::GetCount() 
{
	RefreshConfigsInList();
	return m_pPtrlConfigs->GetCount();
}

LPUNKNOWN CAutoConfigurations::GetNewEnum() 
{
	RefreshConfigsInList();
	CEnumVariantObjs* pEnumVariant = new CEnumVariantObjs(m_pPtrlConfigs);
	
	if (pEnumVariant == NULL)
		DsThrowCannedOleDispatchException(E_OUTOFMEMORY);

	// We don't QueryInterface, since we don't want the IEnumVARIANT
	//  AddRef'd.  The "new CEnumVariantObjs" above already AddRefs it for
	//  us.
	return &pEnumVariant->m_xEnumVariant;
}

LPDISPATCH CAutoConfigurations::GetParent() 
{
	if (m_hBld == NULL)
	{
		// This project no longer exists
		CString strPrompt;
		AfxFormatString1(strPrompt, IDS_AUTO_PRJ_NOTEXIST, m_strProject);
		DsThrowOleDispatchException(DS_E_PROJECT_NOT_VALID, (LPCTSTR) strPrompt);
	}

	return CAutoProject::Create(m_hBld, m_strProject)->GetIDispatch(FALSE);
}

LPDISPATCH CAutoConfigurations::Item(const VARIANT FAR& index) 
{
	// Check to see if there isn't a parameter.
	if (index.vt == VT_ERROR)
	{
		// Parameter is optional and is not supplied.
		// Return a pointer to this collection.
		DsThrowCannedOleDispatchException(E_INVALIDARG);
	}

	RefreshConfigsInList();

	// Check to see if the parameter is a string
	if (index.vt == VT_BSTR)
	{
		// Parameter is the configuration name
		CString strName(index.bstrVal);

		LPDISPATCH pDispRetConfiguration = NULL;
		POSITION pos = m_pPtrlConfigs->GetHeadPosition();
		while (pos != NULL)
		{
			LPDISPATCH pDispCurrConfiguration = m_pPtrlConfigs->GetNext(pos);
			CAutoConfiguration* pProj = CAutoConfiguration::FromIDispatch((IConfiguration*)pDispCurrConfiguration);
			ASSERT_KINDOF(CAutoConfiguration, pProj);
			if (strName == pProj->m_strName)
			{
				pDispRetConfiguration = pDispCurrConfiguration;
				pDispRetConfiguration->AddRef();
				break;
			}
		}
		return pDispRetConfiguration ;
	}

    // coerce index to VT_I4
	COleVariant coercedIndex(index);
	coercedIndex.ChangeType(VT_I4);
 	IDispatch* pObj = NULL ;
	// Subtract 1 from index, since we expect a 1-based index.
    POSITION pos = m_pPtrlConfigs->FindIndex(coercedIndex.lVal - 1);
    if (pos != NULL)
	{
        pObj = m_pPtrlConfigs->GetAt(pos);
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

CAutoConfigurations* CAutoConfigurations::Create(HBUILDER hBld, LPCTSTR szProject)
{
	// Make a new CAutoConfigurations
	CAutoConfigurations* pConfigs = new CAutoConfigurations;
	pConfigs->m_pPtrlConfigs = new CDispatchList;
	pConfigs->m_hBld = hBld;
	pConfigs->m_strProject = szProject;

	// Enumerate through targets, create a CAutoConfiguration for each,
	//  and put them all in m_pPtrlConfigs.
/*
	if (hBld != NULL)
		pConfigs->RefreshConfigsInList();
	else
		ASSERT(FALSE);
*/
	g_LinkAutoObjToProject.OnAutoObjCreated(pConfigs, hBld);
	return pConfigs;
}


void CAutoConfigurations::RefreshConfigsInList()
{
	ReleaseConfigsInList();

	if (m_hBld == NULL)
	{
		// This project no longer exists
		CString strPrompt;
		AfxFormatString1(strPrompt, IDS_AUTO_PRJ_NOTEXIST, m_strProject);
		DsThrowOleDispatchException(DS_E_PROJECT_NOT_VALID, (LPCTSTR) strPrompt);
	}

	HBLDTARGET hBldTarget;
	CString strTarget;
	g_BldSysIFace.InitTargetEnum(m_hBld);
	while ( (hBldTarget = g_BldSysIFace.GetNextTarget(strTarget, m_hBld)) != NULL )
	{
		CAutoConfiguration* pConfig = 
			CAutoConfiguration::Create(m_hBld, strTarget );

		// Add the new configuration's IDispatch on the end of this
		//  configuration collection's internal pointer list.
		// Don't AddRef pConfig, since it was just created (that implicitly
		//  addrefs it).
		m_pPtrlConfigs->AddTail(pConfig->GetIDispatch(FALSE));
	}
}


// ------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CAutoDepConfigurations dual implementation

STDMETHODIMP CAutoDepConfigurations::XConfigurations::get_Application(IDispatch * FAR* Application)
{
	METHOD_PROLOGUE(CAutoDepConfigurations, Dispatch)
	
	TRY_DUAL(IID_IConfigurations)
	{
		*Application = pThis->GetApplication();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoDepConfigurations::XConfigurations::get_Count(long FAR* Count)
{
	METHOD_PROLOGUE(CAutoDepConfigurations, Dispatch)
	
	TRY_DUAL(IID_IConfigurations)
	{
		*Count = pThis->GetCount();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoDepConfigurations::XConfigurations::get_Parent(IBuildProject FAR* FAR* Parent)
{
	METHOD_PROLOGUE(CAutoDepConfigurations, Dispatch)
	
	TRY_DUAL(IID_IConfigurations)
	{
		*Parent = (IBuildProject*) pThis->GetParent();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoDepConfigurations::XConfigurations::get__NewEnum(IUnknown * FAR* _NewEnum)
{
	METHOD_PROLOGUE(CAutoDepConfigurations, Dispatch)
	
	TRY_DUAL(IID_IConfigurations)
	{
		*_NewEnum = pThis->GetNewEnum();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoDepConfigurations::XConfigurations::Item(VARIANT Index, IConfiguration FAR* FAR* Item)
{
	METHOD_PROLOGUE(CAutoDepConfigurations, Dispatch)
	
	TRY_DUAL(IID_IConfigurations)
	{
		*Item = (IConfiguration*) pThis->Item(Index);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


/////////////////////////////////////////////////////////////////////////////
// CAutoDepConfigurations

IMPLEMENT_DYNCREATE(CAutoDepConfigurations, CAutoObj)

CAutoDepConfigurations::CAutoDepConfigurations()
{
	EnableDualAutomation();
	m_pPtrlConfigs = NULL;
	m_hBld = NULL;
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CAutoDepConfigurations::~CAutoDepConfigurations()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();
	g_LinkAutoObjToProject.OnAutoObjDestroyed(this);
}

void CAutoDepConfigurations::ReleaseConfigsInList()
{
	POSITION pos = m_pPtrlConfigs->GetHeadPosition();
	while (pos != NULL)
	{
		IDispatch* pDispatch = m_pPtrlConfigs->GetNext(pos);
		ASSERT (pDispatch != NULL);
		pDispatch->Release() ;
	}
	m_pPtrlConfigs->RemoveAll();
}
void CAutoDepConfigurations::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	// Release each individual configuration & the collection
	ReleaseConfigsInList();

	// The collection is in charge of cleaning this up.
	delete m_pPtrlConfigs ;
	m_pPtrlConfigs = NULL;


	CAutoObj::OnFinalRelease();
}

void CAutoDepConfigurations::AssociatedObjectReleased()
{
	ASSERT (m_hBld != NULL);
	m_hBld = NULL;
}


BEGIN_MESSAGE_MAP(CAutoDepConfigurations, CAutoObj)
	//{{AFX_MSG_MAP(CAutoDepConfigurations)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoDepConfigurations, CAutoObj)
	//{{AFX_DISPATCH_MAP(CAutoDepConfigurations)
	DISP_PROPERTY_EX(CAutoDepConfigurations, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoDepConfigurations, "Count", GetCount, SetNotSupported, VT_I4)
	DISP_PROPERTY_EX(CAutoDepConfigurations, "Parent", GetParent, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION(CAutoDepConfigurations, "Item", Item, VT_DISPATCH, VTS_VARIANT)
	DISP_DEFVALUE(CAutoDepConfigurations, "Item")
	//}}AFX_DISPATCH_MAP
	DISP_PROPERTY_EX_ID(CAutoDepConfigurations, "_NewEnum", DISPID_NEWENUM, GetNewEnum, SetNotSupported, VT_UNKNOWN)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CAutoDepConfigurations, CAutoObj)
 	INTERFACE_PART(CAutoDepConfigurations, IID_IDispConfigurations, Dispatch)
 	INTERFACE_PART(CAutoDepConfigurations, IID_IConfigurations, Dispatch)
    DUAL_ERRORINFO_PART(CAutoDepConfigurations)
END_INTERFACE_MAP()

// Implement ISupportErrorInfo to indicate we support the 
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CAutoDepConfigurations, IID_IConfigurations)
DS_IMPLEMENT_ENABLE_DUAL(CAutoDepConfigurations, Configurations)
DS_DELEGATE_DUAL_INTERFACE(CAutoDepConfigurations, Configurations)

/////////////////////////////////////////////////////////////////////////////
// CAutoDepConfigurations message handlers

LPDISPATCH CAutoDepConfigurations::GetApplication() 
{
	ASSERT(theApp.m_pAutoApp != NULL) ;
    return theApp.m_pAutoApp->GetIDispatch(TRUE);
}

long CAutoDepConfigurations::GetCount() 
{
	RefreshConfigsInList();
	return m_pPtrlConfigs->GetCount();
}

LPUNKNOWN CAutoDepConfigurations::GetNewEnum() 
{
	RefreshConfigsInList();
	CEnumVariantObjs* pEnumVariant = new CEnumVariantObjs(m_pPtrlConfigs);
	
	if (pEnumVariant == NULL)
		DsThrowCannedOleDispatchException(E_OUTOFMEMORY);

	// We don't QueryInterface, since we don't want the IEnumVARIANT
	//  AddRef'd.  The "new CEnumVariantObjs" above already AddRefs it for
	//  us.
	return &pEnumVariant->m_xEnumVariant;
}

LPDISPATCH CAutoDepConfigurations::GetParent() 
{
	if (m_hBld == NULL)
	{
		// This project no longer exists
		CString strPrompt;
		AfxFormatString1(strPrompt, IDS_AUTO_PRJ_NOTEXIST, m_strProject);
		DsThrowOleDispatchException(DS_E_PROJECT_NOT_VALID, (LPCTSTR) strPrompt);
	}

	return CAutoProject::Create(m_hBld, m_strProject)->GetIDispatch(FALSE);
}

LPDISPATCH CAutoDepConfigurations::Item(const VARIANT FAR& index) 
{
	// Check to see if there isn't a parameter.
	if (index.vt == VT_ERROR)
	{
		// Parameter is optional and is not supplied.
		// Return a pointer to this collection.
		DsThrowCannedOleDispatchException(E_INVALIDARG);
	}

	RefreshConfigsInList();

	// Check to see if the parameter is a string
	if (index.vt == VT_BSTR)
	{
		// Parameter is the configuration name
		CString strName(index.bstrVal);

		LPDISPATCH pDispRetConfiguration = NULL;
		POSITION pos = m_pPtrlConfigs->GetHeadPosition();
		while (pos != NULL)
		{
			LPDISPATCH pDispCurrConfiguration = m_pPtrlConfigs->GetNext(pos);
			CAutoConfiguration* pProj = CAutoConfiguration::FromIDispatch((IConfiguration*)pDispCurrConfiguration);
			ASSERT_KINDOF(CAutoConfiguration, pProj);
			if (strName == pProj->m_strName)
			{
				pDispRetConfiguration = pDispCurrConfiguration;
				pDispRetConfiguration->AddRef();
				break;
			}
		}
		return pDispRetConfiguration ;
	}

    // coerce index to VT_I4
	COleVariant coercedIndex(index);
	coercedIndex.ChangeType(VT_I4);
 	IDispatch* pObj = NULL ;
	// Subtract 1 from index, since we expect a 1-based index.
    POSITION pos = m_pPtrlConfigs->FindIndex(coercedIndex.lVal - 1);
    if (pos != NULL)
	{
        pObj = m_pPtrlConfigs->GetAt(pos);
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



void CAutoDepConfigurations::RefreshConfigsInList()
{
	ReleaseConfigsInList();

	if (m_hBld == NULL)
	{
		// This project no longer exists
		CString strPrompt;
		AfxFormatString1(strPrompt, IDS_AUTO_PRJ_NOTEXIST, m_strProject);
		DsThrowOleDispatchException(DS_E_PROJECT_NOT_VALID, (LPCTSTR) strPrompt);
	}

	HPROJDEP hProjDep;
	HBLDTARGET hBldTarget;
	CString strTarget;
	
	CProject* pProject = g_BldSysIFace.CnvHBuilder(m_hBld);
	CProjTempConfigChange projTempConfigChange(pProject);
	projTempConfigChange.ChangeConfig(m_strProject);

	// Find the HFileSet for the parent config.
	CObList lstDepSet;
	POSITION posHDepSet;
	g_BldSysIFace.InitProjectDepEnum(ACTIVE_FILESET,m_hBld,lstDepSet,posHDepSet);
	while ( (hProjDep = g_BldSysIFace.GetNextProjectDep(ACTIVE_FILESET, m_hBld,lstDepSet,posHDepSet)) != NULL )
	{

		// From the proj dep find the target.
		// from the target
		CProjectDependency *pProjDep =(CProjectDependency*)hProjDep;
		CTargetItem * pTarg = pProjDep->GetProjectDep();
		CProject *pDepProject = pTarg->GetProject();
		HBLDTARGET hTarget = g_BldSysIFace.GetActiveTarget ((HBUILDER) pDepProject);
		HFILESET hFileSet = g_BldSysIFace.GetFileSet ((HBUILDER) pDepProject, hTarget);
		strTarget.Empty();
		g_BldSysIFace.GetTargetNameFromFileSet (hFileSet, strTarget, (HBUILDER)pProject );
		if( !strTarget.IsEmpty() ){
			// From the HBLDTARGET find the string strTarget
			
			CAutoConfiguration* pConfig = 
				CAutoConfiguration::Create((HBUILDER) pDepProject, strTarget );
	
			// Add the new configuration's IDispatch on the end of this
			//  configuration collection's internal pointer list.
			// Don't AddRef pConfig, since it was just created (that implicitly
			//  addrefs it).
			m_pPtrlConfigs->AddTail(pConfig->GetIDispatch(FALSE));
		}
	}
}


CAutoDepConfigurations* CAutoDepConfigurations::Create(HBUILDER hBld, LPCTSTR szProject)
{
	// Make a new CAutoDepConfigurations
	CAutoDepConfigurations* pConfigs = new CAutoDepConfigurations;
	pConfigs->m_pPtrlConfigs = new CDispatchList;
	pConfigs->m_hBld = hBld;
	pConfigs->m_strProject = szProject;

	// Enumerate through targets, create a CAutoConfiguration for each,
	//  and put them all in m_pPtrlConfigs.
/*
	if (hBld != NULL)
		pConfigs->RefreshConfigsInList();
	else
		ASSERT(FALSE);
*/
	g_LinkAutoObjToProject.OnAutoObjCreated(pConfigs, hBld);
	return pConfigs;
}
