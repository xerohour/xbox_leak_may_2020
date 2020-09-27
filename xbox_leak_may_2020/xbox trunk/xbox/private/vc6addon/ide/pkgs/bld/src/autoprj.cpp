// AutoPrj.cpp : implementation file
//

#include "stdafx.h"
#include "autocfgs.h"
#include "AutoPrj.h"
#include <main.h>
#include <ObjModel\appauto.h>
#include <ObjModel\blddefs.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void RefreshTargetCombos(BOOL bEmpty = FALSE);

/////////////////////////////////////////////////////////////////////////////
// CAutoProject dual implementation

STDMETHODIMP CAutoProject::XBuildProject::get_Application(IDispatch * FAR* Application)
{
	METHOD_PROLOGUE(CAutoProject, Dispatch)
	
	TRY_DUAL(IID_IBuildProject)
	{
		*Application = pThis->GetApplication();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CAutoProject::XBuildProject::get_Name(BSTR FAR* Name)
{
	METHOD_PROLOGUE(CAutoProject, Dispatch)
	
	TRY_DUAL(IID_IBuildProject)
	{
		*Name = pThis->GetName();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoProject::XBuildProject::get_FullName(BSTR FAR* FullName)
{
	METHOD_PROLOGUE(CAutoProject, Dispatch)
	
	TRY_DUAL(IID_IBuildProject)
	{
		*FullName = pThis->GetFullName();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoProject::XBuildProject::get_Type(BSTR FAR* Type)
{
	METHOD_PROLOGUE(CAutoProject, Dispatch)
	
	TRY_DUAL(IID_IBuildProject)
	{
		*Type= pThis->GetType();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoProject::XBuildProject::get_Parent(IDispatch * FAR* Parent)
{
	METHOD_PROLOGUE(CAutoProject, Dispatch)
	
	TRY_DUAL(IID_IBuildProject)
	{
		*Parent = pThis->GetParent();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoProject::XBuildProject::get_Configurations(IConfigurations FAR* FAR* Configurations)
{
	METHOD_PROLOGUE(CAutoProject, Dispatch)
	
	TRY_DUAL(IID_IBuildProject)
	{
		*Configurations = (IConfigurations*) pThis->GetConfigurations();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoProject::XBuildProject::AddFile(BSTR szFile, VARIANT Reserved)
{
	METHOD_PROLOGUE(CAutoProject, Dispatch)
	
	TRY_DUAL(IID_IBuildProject)
	{
		CString strFile = szFile;
		pThis->AddFile(strFile, Reserved);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoProject::XBuildProject::AddConfiguration(BSTR szConfiguration, VARIANT Reserved)
{
	METHOD_PROLOGUE(CAutoProject, Dispatch)
	
	TRY_DUAL(IID_IBuildProject)
	{
		CString strConfig = szConfiguration;
		pThis->AddConfiguration(strConfig, Reserved);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


/////////////////////////////////////////////////////////////////////////////
// CAutoProject

IMPLEMENT_DYNCREATE(CAutoProject, CAutoObj)

CAutoProject::CAutoProject()
{
	EnableDualAutomation();

	m_pConfigs = NULL;
	m_hBld = NULL;

	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CAutoProject::~CAutoProject()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();

	g_LinkAutoObjToProject.OnAutoObjDestroyed(this);
}


void CAutoProject::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	ASSERT (m_pConfigs != NULL);

	m_pConfigs->Release();
	m_pConfigs = NULL;

	CAutoObj::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CAutoProject, CAutoObj)
	//{{AFX_MSG_MAP(CAutoProject)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoProject, CAutoObj)
	//{{AFX_DISPATCH_MAP(CAutoProject)
	DISP_PROPERTY_EX_ID(CAutoProject, "Application", 2, GetApplication, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CAutoProject, "Parent", 3,GetParent, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX_ID(CAutoProject, "Type", 4,GetType, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX_ID(CAutoProject, "Configurations", 100, GetConfigurations, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION_ID(CAutoProject, "AddFile", 101, AddFile, VT_EMPTY, VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION_ID(CAutoProject, "AddConfiguration", 102,AddConfiguration, VT_EMPTY, VTS_BSTR VTS_VARIANT)
	//}}AFX_DISPATCH_MAP
	DISP_PROPERTY_EX_ID(CAutoProject, "Name", 0, GetName, SetNotSupported, VT_BSTR)
	DISP_PROPERTY_EX_ID(CAutoProject, "FullName", 1, GetFullName, SetNotSupported, VT_BSTR)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CAutoProject, CAutoObj)
	INTERFACE_PART(CAutoProject, IID_IDispProject, Dispatch)
	INTERFACE_PART(CAutoProject, IID_IGenericProject, Dispatch)
	INTERFACE_PART(CAutoProject, IID_IBuildProject, Dispatch)
	DUAL_ERRORINFO_PART(CAutoProject)
END_INTERFACE_MAP()

// Implement ISupportErrorInfo to indicate we support the 
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CAutoProject, IID_IBuildProject)
DS_IMPLEMENT_ENABLE_DUAL(CAutoProject, BuildProject)
DS_DELEGATE_DUAL_INTERFACE(CAutoProject, BuildProject)
DS_IMPLEMENT_VTBL_PAD_10(CAutoProject, BuildProject)

/////////////////////////////////////////////////////////////////////////////
// CAutoProject message handlers

LPDISPATCH CAutoProject::GetApplication() 
{
	ASSERT(theApp.m_pAutoApp != NULL) ;
    return theApp.m_pAutoApp->GetIDispatch(TRUE);
}

LPDISPATCH CAutoProject::GetParent() 
{
	// Application object is a Project's parent
	ASSERT(theApp.m_pAutoApp != NULL) ;
    return theApp.m_pAutoApp->GetIDispatch(TRUE);
}

BSTR CAutoProject::GetName() 
{
	return m_strName.AllocSysString();
}

BSTR CAutoProject::GetFullName()
{
	return m_strFullName.AllocSysString();
}

BSTR CAutoProject::GetType()
{
	CString strResult(DS_BUILD_PROJECT);

	return strResult.AllocSysString();
}

LPDISPATCH CAutoProject::GetConfigurations() 
{
	ASSERT (m_pConfigs != NULL);
	m_pConfigs->AddRef();
	return m_pConfigs;
}

void CAutoProject::AddFile(LPCTSTR szFile, const VARIANT FAR& Reserved) 
{
	CTempDisableUI tempNoUI;
	CPath 		pathFile;
	HBLDFILE 	hFile;
	HFILESET	hFileSet = ACTIVE_FILESET;

	CProject *	pProject=(CProject *)m_hBld;

	ASSERT( pProject != NULL );
	CDir dir = pProject->GetProjDir();
	if ( pathFile.CreateFromDirAndFilename( dir, szFile ) )
	{
		if( g_BldSysIFace.AddFile( ACTIVE_FILESET, &pathFile, 0, m_hBld ) )
		{
			return;
		}
	}
	DsThrowOleDispatchException(DS_E_CANT_ADD_FILE, IDS_AUTO_CANT_ADD_FILE);
}

void CAutoProject::AddConfiguration(LPCTSTR szName, const VARIANT FAR& Reserved)
{
	CTempDisableUI tempNoUI;
	CPath 		pathFile;
	HBLDFILE 	hFile;
	HFILESET	hFileSet = ACTIVE_FILESET;

	CProject *	pProject=(CProject *)m_hBld;

	ASSERT( pProject != NULL );
	CDir dir = pProject->GetProjDir();

	// FIX this replace with GetDefaultPlatform
	const TCHAR *szPlatform = _T("Win32");
	// strPlatDesc = *(g_prjcompmgr.GetPrimaryPlatform()->GetUIDescription());

	const TCHAR *szProjType = NULL;

	// FIX this replace with GetName
	CProjType * pProjType = pProject->GetProjType();
	const CString *strTypeName = pProjType->GetTypeUIDescription();
	szProjType = *strTypeName;
	CString strProjName;
	pProject->GetName(strProjName);
	CString strName =  strProjName + " - " + szPlatform + " " + szName;

	// Make sure this name doen't already exist
	CString strTarget;
	// Enumerate all the targets
	g_BldSysIFace.InitTargetEnum(m_hBld);
	HBLDTARGET hTarget = g_BldSysIFace.GetNextTarget(strTarget, m_hBld);
	while (hTarget != NO_TARGET)
	{
		if (strTarget.CompareNoCase(strName) == 0)
		{
			// target already exist
			DsThrowOleDispatchException(DS_E_CANT_ADD_CONFIGURATION, IDS_AUTO_CANT_ADD_CONFIGURATION);
		}
		hTarget = g_BldSysIFace.GetNextTarget(strTarget, m_hBld);
	}

	HBLDTARGET hSettingsTarg = NO_TARGET;
	hSettingsTarg = g_BldSysIFace.GetActiveTarget(m_hBld);
	if (g_BldSysIFace.AddTarget( strName,
								szPlatform, 
								szProjType,
								TRUE,
								TRUE,
								TrgMirror,
								hSettingsTarg,
								SettingsDefault,
								hSettingsTarg,
								TRUE,
								TRUE,
								TRUE,
								m_hBld
								))
	{
		RefreshTargetCombos();
		return;
	}
	DsThrowOleDispatchException(DS_E_CANT_ADD_CONFIGURATION, IDS_AUTO_CANT_ADD_CONFIGURATION);
}

CAutoProject* CAutoProject::Create(HBUILDER hBld, LPCTSTR szName)
{
	CAutoProject* pProject = new CAutoProject;
	pProject->m_strName = szName;
	ASSERT(hBld != NO_BUILDER);
	if(hBld != NO_BUILDER)
		pProject->m_strFullName = (const char *)*g_BldSysIFace.CnvHBuilder(hBld)->GetFilePath();

	pProject->m_hBld = hBld;

	CAutoConfigurations* pConfigs = CAutoConfigurations::Create(hBld, szName);

	// Don't AddRef pConfigs, since we just created it, and that implicitly
	//  addrefs it.
	pProject->m_pConfigs = pConfigs->GetIDispatch(FALSE);
	g_LinkAutoObjToProject.OnAutoObjCreated(pProject, hBld);
	return pProject;
}

/////////////////////////////////////////////////////////////////////////////
// CLinkAutoObjToProject

CLinkAutoObjToProject g_LinkAutoObjToProject;

void CLinkAutoObjToProject::OnProjectDestroyed(CProject* pProject)
{
	// Notify each autoobj that's mapped to this CProject
	POSITION pos = m_PtrMap.GetStartPosition();
	while (pos != NULL)
	{
		CAutoObj* pAutoObj;
		CProject* pProjectIter;
		m_PtrMap.GetNextAssoc(pos, pAutoObj, pProjectIter);
		if (pProject != pProjectIter)
			continue;
		ASSERT_KINDOF(CAutoObj, pAutoObj);
		pAutoObj->AssociatedObjectReleased();
		VERIFY(m_PtrMap.RemoveKey(pAutoObj));
	}
}

void CLinkAutoObjToProject::OnAutoObjDestroyed(CAutoObj* pAutoObj)
{
	CProject* pDummy;
	VERIFY(!m_PtrMap.Lookup(pAutoObj, pDummy) || m_PtrMap.RemoveKey(pAutoObj));
}

void CLinkAutoObjToProject::OnAutoObjCreated(CAutoObj* pAutoObj, HBUILDER hBld)
{
	m_PtrMap[pAutoObj] = g_BldSysIFace.CnvHBuilder(hBld);
}
