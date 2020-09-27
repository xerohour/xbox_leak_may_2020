// autocfg.cpp : implementation file
//

#include "stdafx.h"
#include "autocfg.h"
#include <ObjModel\appauto.h>
#include <main.h>
#include "autoprj.h"
#include "autocfgs.h"
#include <ObjModel\blddefs.h>
#include "prjconfg.h"
#include "projitem.h"
#include "project.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoConfiguration dual implementation

STDMETHODIMP CAutoConfiguration::XConfiguration::get_Application(IDispatch * FAR* Application)
{
	METHOD_PROLOGUE(CAutoConfiguration, Dispatch)
	
	TRY_DUAL(IID_IConfiguration)
	{
		*Application = pThis->GetApplication();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CAutoConfiguration::XConfiguration::get__Name(BSTR FAR* _Name)
{
	METHOD_PROLOGUE(CAutoConfiguration, Dispatch)
	
	TRY_DUAL(IID_IConfiguration)
	{
		*_Name = pThis->GetName();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


STDMETHODIMP CAutoConfiguration::XConfiguration::get_Name(BSTR FAR* Name)
{
	METHOD_PROLOGUE(CAutoConfiguration, Dispatch)
	
	TRY_DUAL(IID_IConfiguration)
	{
		*Name = pThis->GetName();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoConfiguration::XConfiguration::get_Parent(IDispatch * FAR* Parent)
{
	METHOD_PROLOGUE(CAutoConfiguration, Dispatch)
	
	TRY_DUAL(IID_IConfiguration)
	{
		*Parent = pThis->GetParent();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoConfiguration::XConfiguration::AddToolSettings(BSTR szTool, BSTR szSettings, VARIANT Reserved)
{
	METHOD_PROLOGUE(CAutoConfiguration, Dispatch)
	
	TRY_DUAL(IID_IConfiguration)
	{
		CString strTool = szTool;
		CString strSettings = szSettings;
		pThis->AddToolSettings(strTool, strSettings, Reserved);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoConfiguration::XConfiguration::RemoveToolSettings(BSTR szTool, BSTR szSettings, VARIANT Reserved)
{
	METHOD_PROLOGUE(CAutoConfiguration, Dispatch)
	
	TRY_DUAL(IID_IConfiguration)
	{
		CString strTool = szTool;
		CString strSettings = szSettings;
		pThis->RemoveToolSettings(strTool, strSettings, Reserved);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoConfiguration::XConfiguration::AddCustomBuildStep(BSTR szCommand, BSTR szOutput, BSTR szDescription, VARIANT Reserved)
{
	METHOD_PROLOGUE(CAutoConfiguration, Dispatch)
	
	TRY_DUAL(IID_IConfiguration)
	{
		CString strCommand = szCommand;
		CString strOutput = szOutput;
		CString strDescription = szDescription;
		pThis->AddCustomBuildStep(strCommand, strOutput, strDescription, Reserved);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoConfiguration::XConfiguration::AddFileSettings(BSTR szFile, BSTR szSettings, VARIANT Reserved)
{
	METHOD_PROLOGUE(CAutoConfiguration, Dispatch)
	
	TRY_DUAL(IID_IConfiguration)
	{
		CString strFile = szFile;
		CString strSettings = szSettings;
		pThis->AddFileSettings(strFile, strSettings, Reserved);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoConfiguration::XConfiguration::RemoveFileSettings(BSTR szFile, BSTR szSettings, VARIANT Reserved)
{
	METHOD_PROLOGUE(CAutoConfiguration, Dispatch)
	
	TRY_DUAL(IID_IConfiguration)
	{
		CString strFile = szFile;
		CString strSettings = szSettings;
		pThis->RemoveFileSettings(strFile, strSettings, Reserved);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoConfiguration::XConfiguration::AddCustomBuildStepToFile(BSTR szFile, BSTR szCommand, BSTR szOutput, BSTR szDescription, VARIANT Reserved)
{
	METHOD_PROLOGUE(CAutoConfiguration, Dispatch)
	
	TRY_DUAL(IID_IConfiguration)
	{
		CString strFile = szFile;
		CString strCommand = szCommand;
		CString strOutput = szOutput;
		CString strDescription = szDescription;
		pThis->AddCustomBuildStepToFile(strFile, strCommand, strOutput, strDescription, Reserved);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}


// REVIEW(kperry): This is an artifact of a poorly thought out feature. The vtable entry can be resued.
STDMETHODIMP CAutoConfiguration::XConfiguration::Reserved11(void)
{
	METHOD_PROLOGUE(CAutoConfiguration, Dispatch)
	
	TRY_DUAL(IID_IConfiguration)
	{
		return E_NOTIMPL;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoConfiguration::XConfiguration::MakeCurrentSettingsDefault( VARIANT Reserved)
{
	METHOD_PROLOGUE(CAutoConfiguration, Dispatch)
	
	TRY_DUAL(IID_IConfiguration)
	{
		pThis->MakeCurrentSettingsDefault(Reserved);
		return NOERROR;
	}
	CATCH_ALL_DUAL
}

STDMETHODIMP CAutoConfiguration::XConfiguration::get_Configurations(IConfigurations FAR* FAR* Configurations)
{
	METHOD_PROLOGUE(CAutoConfiguration, Dispatch)
	
	TRY_DUAL(IID_IConfiguration)
	{
		
		*Configurations = (IConfigurations*) pThis->GetDependentConfigurations();
		return NOERROR;
	}
	CATCH_ALL_DUAL
}




/////////////////////////////////////////////////////////////////////////////
// CAutoConfiguration

IMPLEMENT_DYNCREATE(CAutoConfiguration, CAutoObj)

CAutoConfiguration::CAutoConfiguration()
{
	EnableDualAutomation();
	
	m_pConfigs = NULL;
	m_hBuilder = NULL;
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	
	AfxOleLockApp();
}

CAutoConfiguration::~CAutoConfiguration()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	
	AfxOleUnlockApp();

	g_LinkAutoObjToProject.OnAutoObjDestroyed(this);
}


void CAutoConfiguration::OnFinalRelease()
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

void CAutoConfiguration::AssociatedObjectReleased()
{
	ASSERT (m_hBuilder != NULL);
	m_hBuilder = NULL;
}


BEGIN_MESSAGE_MAP(CAutoConfiguration, CAutoObj)
	//{{AFX_MSG_MAP(CAutoConfiguration)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAutoConfiguration, CAutoObj)
	//{{AFX_DISPATCH_MAP(CAutoConfiguration)
	DISP_PROPERTY_EX(CAutoConfiguration, "Application", GetApplication, SetNotSupported, VT_DISPATCH)
	DISP_PROPERTY_EX(CAutoConfiguration, "Parent", GetParent, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION(CAutoConfiguration, "AddToolSettings", AddToolSettings, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CAutoConfiguration, "RemoveToolSettings", RemoveToolSettings, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CAutoConfiguration, "AddCustomBuildStep", AddCustomBuildStep, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_BSTR VTS_VARIANT)
	DISP_PROPERTY_EX(CAutoConfiguration, "Configurations", GetDependentConfigurations, SetNotSupported, VT_DISPATCH)
	DISP_FUNCTION(CAutoConfiguration, "Reserved11", Reserved11, VT_EMPTY, VTS_NONE )
	DISP_FUNCTION(CAutoConfiguration, "MakeCurrentSettingsDefault", MakeCurrentSettingsDefault, VT_EMPTY, VTS_VARIANT)
//	DISP_FUNCTION(CAutoConfiguration, "Build", Build, VT_EMPTY, VTS_VARIANT)
	DISP_FUNCTION(CAutoConfiguration, "AddFileSettings", AddFileSettings, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CAutoConfiguration, "RemoveFileSettings", RemoveFileSettings, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CAutoConfiguration, "AddCustomBuildStepToFile", AddCustomBuildStepToFile, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_VARIANT)
	//}}AFX_DISPATCH_MAP
	DISP_PROPERTY_EX_ID(CAutoConfiguration, "Name", DISPID_VALUE, GetName, SetNotSupported, VT_BSTR)
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CAutoConfiguration, CAutoObj)
	INTERFACE_PART(CAutoConfiguration, IID_IDispConfiguration, Dispatch)
	INTERFACE_PART(CAutoConfiguration, IID_IConfiguration, Dispatch)
	DUAL_ERRORINFO_PART(CAutoConfiguration)
END_INTERFACE_MAP()

// Implement ISupportErrorInfo to indicate we support the 
// OLE Automation error handler.
IMPLEMENT_DUAL_ERRORINFO(CAutoConfiguration, IID_IConfiguration)
DS_IMPLEMENT_ENABLE_DUAL(CAutoConfiguration, Configuration)
DS_DELEGATE_DUAL_INTERFACE(CAutoConfiguration, Configuration)


/////////////////////////////////////////////////////////////////////////////
// CAutoConfiguration message handlers

LPDISPATCH CAutoConfiguration::GetApplication() 
{
	ASSERT(theApp.m_pAutoApp != NULL) ;
    return theApp.m_pAutoApp->GetIDispatch(TRUE);
}

LPDISPATCH CAutoConfiguration::GetParent() 
{
	if (m_hBuilder == NULL)
		ThrowInvalidError();
	CProject* pProj = g_BldSysIFace.CnvHBuilder(m_hBuilder);
	const CPath* pPath = pProj->GetFilePath();
	if (pPath == NULL)
		// No path?  Bogus
		return NULL;

	CString strProjName;
	pPath->GetBaseNameString(strProjName);
	return CAutoProject::Create(m_hBuilder, strProjName)->GetIDispatch(FALSE);
}

BSTR CAutoConfiguration::GetName() 
{
	return m_strName.AllocSysString();
}

CAutoConfiguration* CAutoConfiguration::Create(HBUILDER hBuilder, LPCTSTR szName)
{
	CAutoConfiguration* pConfig = new CAutoConfiguration;
	pConfig->m_strName = szName;
	pConfig->m_hBuilder = hBuilder;
	pConfig->m_nDepth = 0;

	//REVIEW(kperry) name should be name of project I think.
	CAutoDepConfigurations* pConfigs = CAutoDepConfigurations::Create(hBuilder, szName);
	// Don't AddRef pConfigs, since we just created it, and that implicitly
	//  addrefs it.
	pConfig->m_pConfigs = pConfigs->GetIDispatch(FALSE);

	g_LinkAutoObjToProject.OnAutoObjCreated(pConfig, hBuilder);
	return pConfig;
}

void CAutoConfiguration::ThrowInvalidError()
{
	// This configuration is no longer valid
	CString strPrompt;
	AfxFormatString1(strPrompt, IDS_AUTO_CFG_NOTEXIST, m_strName);
	DsThrowOleDispatchException(DS_E_CONFIGURATION_NOT_VALID, (LPCTSTR) strPrompt);
}


void CAutoConfiguration::EnsureBuildable()
{
	if (m_hBuilder == NULL)
		ThrowInvalidError();

	CProject* pProject = g_BldSysIFace.CnvHBuilder(m_hBuilder);
	CProjTempConfigChange projTempConfigChange(pProject);
	projTempConfigChange.ChangeConfig(m_strName);
	CProjType* pProjType = pProject->GetProjType();
	if (pProjType == NULL)
		ThrowInvalidError();

	if (pProject->m_bProjIsExe || !pProjType->IsSupported() 
		|| pProjType->GetPlatform() == NULL 
		|| !pProjType->GetPlatform()->GetBuildable())
	{
		CString strPrompt;
		AfxFormatString1(strPrompt, IDS_AUTO_CFG_NOTSUPPORTED, m_strName);
		DsThrowOleDispatchException(DS_E_CONFIGURATION_NOT_SUPPORTED, (LPCTSTR) strPrompt);
	}
}

HBLDTARGET CAutoConfiguration::GetTarget()
{
	HBLDTARGET hTarget = 0;
	if (m_hBuilder == NULL ||
		(hTarget = g_BldSysIFace.GetTarget(m_strName, m_hBuilder)) == NULL)
	{
		ThrowInvalidError();
	}
	return hTarget;
}

void CAutoConfiguration::AddToolSettings(LPCTSTR szTool, LPCTSTR szSettings, const VARIANT FAR& Reserved) 
{
	CTempDisableUI tempNoUI;
	HBLDTARGET hTarget = GetTarget();	// Will throw error if invalid
	if (!g_BldSysIFace.SetToolSettings(hTarget, szSettings, szTool,
		TRUE, FALSE, m_hBuilder))
	{
		DsThrowOleDispatchException(DS_E_CANT_ADD_SETTINGS, IDS_AUTO_CANT_ADD_SETTINGS);
	}
}

void CAutoConfiguration::RemoveToolSettings(LPCTSTR szTool, LPCTSTR szSettings, const VARIANT FAR& Reserved) 
{
	CTempDisableUI tempNoUI;
	HBLDTARGET hTarget = GetTarget();	// Will throw error if invalid

	// [davbr] TEMPORARY (hopefully) FIX FOR DEVSTUDIO 96 BUG 11735.  The real
	//  fix should be made in bldiface.cpp: CBldSysIFace::SetToolSettingsI.
	//  SetToolSettingsI should no longer search for a verbatim occurrence
	//  of the user-supplied settings string in the canonical listing of
	//  all the settings on the tool.  Instead, it should call
	//  g_prjoptengine.ParseString with optbeh having OBAnti (not OBClear)
	//  set.  In other words, erase the entire "if (!fAdd) ... else ..."
	//  clause and always pass the original settings string to ParseString;
	//  simply adjust optbeh based on whether the user is adding or
	//  removing (OBAnti) options.  Unfortunately, I tried this and it
	//  didn't work.  Some settings would get removed correctly, but others
	//  appeared to be misinterpreted while parsing.  All function calls
	//  "succeeded" but the wrong things were happening.  THIS SHOULD BE
	//  FIXED IN THE FUTURE
	//
	// For now, the workaround [HACK] code is HERE, not in bldiface, so only
	//  automation controllers will benefit from it (and no one else
	//  will get screwed if there are problems).  We manually parse
	//  through the settings string finding options and passing them one
	//  by one to SetToolSettings.  We make a special case for /D which
	//  has two common formats (/D_SYMBOL and /D "_SYMBOL"), only one of
	//  which (the latter) will work with SetToolSettings.

	// Make R/W copy of the string on the stack to play with
	int nOrigLength = _tcslen(szSettings);
	LPTSTR szSettingsCopy = (LPTSTR) _alloca(nOrigLength+sizeof(TCHAR));
	_tcscpy(szSettingsCopy, szSettings);

	LPCTSTR szPrefixes = "/-";
	LPTSTR szNext = szSettingsCopy;
	LPTSTR szCurr;
	
	// Judiciously add/remove NULL terminators in the middle of the
	//  string to pick out each option individually, to pass to
	//  bldiface's AddToolSettings.
	while (szNext != NULL)
	{
		szCurr = szNext;

		// Is there a flag after the current one?  If so, temporarily NULL
		//  terminate before next flag.
		int nPos = _tcscspn(szCurr+1, szPrefixes);
		if (nPos + (szCurr+1-szSettingsCopy) >= nOrigLength)
			szNext = NULL;
		else
		{
			szNext = szCurr+1 + nPos;
			*szNext = '\0';
		}

		LPTSTR szStringToPass = szCurr;		// May get modified for special flags

		// SPECIAL CASE: /D flag (don't adjust if we're altering linker
		//  or bscmake settings)
		if (*(szCurr+1) == 'D' && *(szCurr+2) != ' ' && *(szCurr+2) != '"'
			&& _tcsicmp(szTool, "link.exe") && _tcsicmp(szTool, "bscmake.exe"))
		{
			// Convert: /D_SYMBOL -> /D "_SYMBOL".  Allocate new
			//  string on the stack, with extra room for space and two quotes
			szStringToPass = (LPTSTR) _alloca(_tcslen(szCurr) + 4*sizeof(TCHAR));
			_tcscpy(szStringToPass, "/D \"");

			// Length of symbol is distance to nearest space,
			//  minus /D.
			int nSymbolLength = _tcscspn(szCurr+2, " ");

			_tcsncat(szStringToPass, szCurr+2, nSymbolLength);	// append sym
			_tcscat(szStringToPass, "\"");						// append final "
		}

		if (!g_BldSysIFace.SetToolSettings(hTarget, szStringToPass, szTool,
			FALSE, FALSE, m_hBuilder))
		{
			DsThrowOleDispatchException(DS_E_CANT_REMOVE_SETTINGS, IDS_AUTO_CANT_REMOVE_SETTINGS);
		}

		// Undo NULL termination and continue
		if (szNext != NULL)
			*szNext = '/';
	}
}

void CAutoConfiguration::AddCustomBuildStep(LPCTSTR szCommand, LPCTSTR szOutput, LPCTSTR szDescription, const VARIANT FAR& Reserved) 
{
	CTempDisableUI tempNoUI;
	if( szCommand && _tcslen(szCommand) && szOutput && _tcslen(szOutput) )
	{
		HBLDTARGET hTarget = GetTarget();	// Will throw error if invalid
		if (g_BldSysIFace.AssignCustomBuildStep(szCommand, szOutput, szDescription, hTarget,
			NO_FILE, m_hBuilder))
		{
			return;
		}
	}
	DsThrowOleDispatchException(DS_E_CANT_ADD_BUILD_STEP, IDS_AUTO_CANT_ADD_BUILD_STEP);
}

void CAutoConfiguration::AddFileSettings(LPCTSTR szFile, LPCTSTR szSettings, const VARIANT FAR& Reserved) 
{
	CTempDisableUI tempNoUI;
	HBLDTARGET	hTarget = GetTarget();	
	CPath 		pathFile;
	HBLDFILE 	hFile;
	HFILESET	hFileSet = ACTIVE_FILESET;
	HBUILDER	hBld = GetHBuilder();	

	CProject *	pProject=(CProject *)hBld;	

	ASSERT( pProject != NULL );
	CDir dir = pProject->GetProjDir();
	if ( pathFile.CreateFromDirAndFilename( dir, szFile ) )
	{
		if( g_BldSysIFace.GetFile( &pathFile, hFile, ACTIVE_FILESET, hBld ) )
		{
			if (g_BldSysIFace.SetToolSettings(hTarget, hFile, szSettings, NULL,
				TRUE, FALSE))
			{
				// success
				return;
			}
		}
	}
	DsThrowOleDispatchException(DS_E_CANT_ADD_SETTINGS, IDS_AUTO_CANT_ADD_SETTINGS);
}

void CAutoConfiguration::RemoveFileSettings(LPCTSTR szFile, LPCTSTR szSettings, const VARIANT FAR& Reserved)
{
	CTempDisableUI tempNoUI;
	HBLDTARGET hTarget = GetTarget();	// Will throw error if invalid
	CPath 		pathFile;
	HBLDFILE 	hFile;
	HFILESET	hFileSet = ACTIVE_FILESET;
	HBUILDER	hBld = GetHBuilder();	

	CProject *	pProject=(CProject *)hBld;	

	ASSERT( pProject != NULL );
	CDir dir = pProject->GetProjDir();
	if ( pathFile.CreateFromDirAndFilename( dir, szFile ) )
	{
		if( g_BldSysIFace.GetFile( &pathFile, hFile, ACTIVE_FILESET, hBld ) )
		{

			// [davbr] TEMPORARY (hopefully) FIX FOR DEVSTUDIO 96 BUG 11735.  The real
			//  fix should be made in bldiface.cpp: CBldSysIFace::SetToolSettingsI.
			//  SetToolSettingsI should no longer search for a verbatim occurrence
			//  of the user-supplied settings string in the canonical listing of
			//  all the settings on the tool.  Instead, it should call
			//  g_prjoptengine.ParseString with optbeh having OBAnti (not OBClear)
			//  set.  In other words, erase the entire "if (!fAdd) ... else ..."
			//  clause and always pass the original settings string to ParseString;
			//  simply adjust optbeh based on whether the user is adding or
			//  removing (OBAnti) options.  Unfortunately, I tried this and it
			//  didn't work.  Some settings would get removed correctly, but others
			//  appeared to be misinterpreted while parsing.  All function calls
			//  "succeeded" but the wrong things were happening.  THIS SHOULD BE
			//  FIXED IN THE FUTURE
			//
			// For now, the workaround [HACK] code is HERE, not in bldiface, so only
			//  automation controllers will benefit from it (and no one else
			//  will get screwed if there are problems).  We manually parse
			//  through the settings string finding options and passing them one
			//  by one to SetToolSettings.  We make a special case for /D which
			//  has two common formats (/D_SYMBOL and /D "_SYMBOL"), only one of
			//  which (the latter) will work with SetToolSettings.
		
			// Make R/W copy of the string on the stack to play with
			int nOrigLength = _tcslen(szSettings);
			LPTSTR szSettingsCopy = (LPTSTR) _alloca(nOrigLength+sizeof(TCHAR));
			_tcscpy(szSettingsCopy, szSettings);
		
			LPCTSTR szPrefixes = "/-";
			LPTSTR szNext = szSettingsCopy;
			LPTSTR szCurr;
			
			
			// Judiciously add/remove NULL terminators in the middle of the
			//  string to pick out each option individually, to pass to
			//  bldiface's AddToolSettings.
			while (szNext != NULL)
			{
				szCurr = szNext;
		
				// Is there a flag after the current one?  If so, temporarily NULL
				//  terminate before next flag.
				int nPos = _tcscspn(szCurr+1, szPrefixes);
				if (nPos + (szCurr+1-szSettingsCopy) >= nOrigLength)
					szNext = NULL;
				else
				{
					szNext = szCurr+1 + nPos;
					*szNext = '\0';
				}
		
				LPTSTR szStringToPass = szCurr;		// May get modified for special flags
		
				// SPECIAL CASE: /D flag (don't adjust if we're altering linker
				//  or bscmake settings)
				if( *(szCurr+1) == 'D' && *(szCurr+2) != ' ' && *(szCurr+2) != '"' )
				{
					// Convert: /D_SYMBOL -> /D "_SYMBOL".  Allocate new
					//  string on the stack, with extra room for space and two quotes
					szStringToPass = (LPTSTR) _alloca(_tcslen(szCurr) + 4*sizeof(TCHAR));
					_tcscpy(szStringToPass, "/D \"");
		
					// Length of symbol is distance to nearest space,
					//  minus /D.
					int nSymbolLength = _tcscspn(szCurr+2, " ");
		
					_tcsncat(szStringToPass, szCurr+2, nSymbolLength);	// append sym
					_tcscat(szStringToPass, "\"");						// append final "
				}
		
				if (!g_BldSysIFace.SetToolSettings(hTarget, hFile, szStringToPass, NULL,
					FALSE, FALSE))
				{
					DsThrowOleDispatchException(DS_E_CANT_REMOVE_SETTINGS, IDS_AUTO_CANT_REMOVE_SETTINGS);
				}
		
				// Undo NULL termination and continue
				if (szNext != NULL)
					*szNext = '/';
			}
		}
	}
}

void CAutoConfiguration::AddCustomBuildStepToFile(LPCTSTR szFile, LPCTSTR szCommand, LPCTSTR szOutput, LPCTSTR szDescription, const VARIANT FAR& Reserved)
{
	CTempDisableUI tempNoUI;
	HBLDTARGET hTarget = GetTarget();	// Will throw error if invalid
	CPath 		pathFile;
	HBLDFILE 	hFile;
	HFILESET	hFileSet = ACTIVE_FILESET;
	HBUILDER	hBld = GetHBuilder();	
	CProject *	pProject=(CProject *)hBld;	

	ASSERT( pProject != NULL );
	CDir dir = pProject->GetProjDir();
	if ( pathFile.CreateFromDirAndFilename( dir, szFile ) )
	{
		if( g_BldSysIFace.GetFile( &pathFile, hFile, ACTIVE_FILESET, hBld ) )
		{
			if( szCommand && _tcslen(szCommand) && szOutput && _tcslen(szOutput) )
			{
				if (g_BldSysIFace.AssignCustomBuildStep(szCommand, szOutput, szDescription, hTarget,
					hFile, m_hBuilder))
				{
					g_BldSysIFace.SetUserDefinedDependencies(
						"",
						hTarget,
						hFile,
						m_hBuilder);
					// success
					return;
				}
			}
		}
	}
	DsThrowOleDispatchException(DS_E_CANT_ADD_BUILD_STEP, IDS_AUTO_CANT_ADD_BUILD_STEP);
}


void CAutoConfiguration::Reserved11(void)
{
}

void CAutoConfiguration::MakeCurrentSettingsDefault(const VARIANT FAR& Reserved)
{
	CTempDisableUI tempNoUI;
	HBLDTARGET hTarget = GetTarget();	
	g_BldSysIFace.MakeTargPropsDefault(hTarget, m_hBuilder);
}


LPDISPATCH CAutoConfiguration::GetDependentConfigurations() 
{
	ASSERT (m_pConfigs != NULL);
	m_pConfigs->AddRef();
	return m_pConfigs;
}

