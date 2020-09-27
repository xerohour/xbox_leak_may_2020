// VCProjConvert.cpp : Implementation of CVCProjConvert
#include "stdafx.h"
#include "Vcprojcnvt2.h"
#include "VCProjConvert.h"
#include "optnbsc.h"
#include "optncplr.h"
#include "optnlib.h"
#include "optnmtl.h"
#include "optnrc.h"
#include "targitem.h"
#include "x86optn.h"
#include "mcdoptn.h"
#include "axpoptn.h"
#include <prjids.h>
#include "project.h"
#include "projdep.h"
#include <initguid.h>
#include <vcguid.h>
#include <vccolls.h>

CString CVCProjConvert::s_strSourceFilesString;	// the one, the only, UI string this DLL needs...

// forward declaration
BOOL ConvertMacros(CString& rstrVal);

/////////////////////////////////////////////////////////////////////////////
// CVCProjConvert
BOOL CVCProjConvert::DoBldSysCompRegister(CProjComponentMgr * pcompmgr, DWORD blc_type, DWORD blc_id)
{
	static int nOffset = 0;
	VSASSERT(pcompmgr, "No component manager!");
	if (!pcompmgr)
		return E_POINTER;

	// what is the hook?
	switch (blc_type)
	{
		case BLC_Tool:
			// only for platform 0 (generic platform)
			if (blc_id != 0)	break;

			// our 'generic' tools
			pcompmgr->RegisterBldSysComp(new CRCCompilerTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_RcCompiler));
			pcompmgr->RegisterBldSysComp(new CRCCompilerNTTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_RcCompilerNT));
			pcompmgr->RegisterBldSysComp(new CMkTypLibTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_MkTypLib));
			pcompmgr->RegisterBldSysComp(new CLibTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_Lib));
			pcompmgr->RegisterBldSysComp(new CBscmakeTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_BscMake));
			pcompmgr->RegisterBldSysComp(new CCCompilerTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_Compiler));
			pcompmgr->RegisterBldSysComp(new CLinkerTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_Linker));
			pcompmgr->RegisterBldSysComp(new CLinkerNTTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_LinkerNT));
			pcompmgr->RegisterBldSysComp(new CCustomBuildTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_CustomBuild));
			pcompmgr->RegisterBldSysComp(new CSpecialBuildTool, GenerateComponentId(idAddOnGeneric, BCID_Tool_SpecialBuild));
			break;

		case BLC_OptionHdlr:
			// only for platform 0 (generic platform)
			if (blc_id != 0)	break;

			// our 'generic' option handlers
			pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(CompilerCommon));
			pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(LinkerCommon));
			pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(LinkerNT));
			pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(ResCompiler));
			pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(ResCompilerNT));
			pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(BscMake));
			pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(Lib));
			pcompmgr->RegisterBldSysComp(new OLD_OPTION_HANDLER(MkTypLib));
			break;

		case BLC_TargetType:
			// do this for all registered platforms
			if (blc_id == 0)	break;

			nOffset++;
 			break;
		default:
			break;
	}

	// nothing left to register
	return FALSE;
}

STDMETHODIMP CVCProjConvert::GetPackage(long **ppPackage)
{
	CHECK_POINTER_NULL(ppPackage);
	*ppPackage = reinterpret_cast<long *>(static_cast<CPackage *>(this)); // not an interface; don't want to change devtools.dll
	return S_OK;
}

STDMETHODIMP CVCProjConvert::Initialize(BSTR bstrSourceFilesString)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr = E_FAIL;
	if (g_pPrjoptengine && g_pPrjcompmgr)
		m_bInit = true; // assume have been init'd in a former "life"
	if (!m_bInit)
	{
		s_strSourceFilesString = bstrSourceFilesString;
		VSASSERT(g_pPrjcompmgr == NULL, "Component manager already initialized!");
		VSASSERT(g_pPrjoptengine == NULL, "Option engine already initialized!");
		g_pPrjoptengine = new COptionTable;
		g_pPrjcompmgr = new CProjComponentMgr;
		if (g_pPrjcompmgr && g_pPrjoptengine)
		{
			hr = g_pPrjcompmgr->FInit(static_cast<IVCProjConvert *>(this));
			if (SUCCEEDED(hr))
				m_bInit = true;
		}
	}
	return hr;
}

CVCProjConvert::~CVCProjConvert()
{
	g_BldSysIFace.CloseProject(ACTIVE_PROJECT, WorkspaceClose);
}

STDMETHODIMP CVCProjConvert::OpenProject(BSTR bstrProjectPath, IUnknown *pProjEngineUnk, IUnknown *pUnkSP )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CComQIPtr<VCProjectEngine> pProjEngine = pProjEngineUnk;
	RETURN_INVALID_ON_NULL(pProjEngineUnk);

	if (g_pPrjcompmgr == NULL)
		return E_NOINTERFACE; // not the best error return code, but it means roughly that we haven't been initialized well.

	if (g_pPrjoptengine == NULL)
		return E_NOINTERFACE; // see last comment

	CProject * pOldProject = g_pActiveProject;
	CProject * pProject;
	CDir dirOld; 
	dirOld.CreateFromCurrent();
	try 
	{
		// Create the project object
		pProject = new CProject;
		g_pActiveProject = pProject;
		CString pchProjPath = bstrProjectPath;

		CPath path;

		if (!pchProjPath.IsEmpty() && path.Create(pchProjPath))
		{
			// set the current directory to match the pathname 
			if( !_tcsicmp( path.GetExtension(), ".mdp") ) {
				path.ChangeExtension(".mak");
				pchProjPath = (const TCHAR *)path;
			}

			CDir currentDir; currentDir.CreateFromPath(path);
			currentDir.MakeCurrent();
		}

		// create new one or create and open from storage?
		// open an existing document 
		if (!pProject->InitFromFile (pchProjPath, FALSE))
 			goto CreationError;

		// if this was a 2.0 project, reject it
		if( pProject->m_bConvertedVC20 )
			return VCPROJ_E_UNSUPPORTED_PROJ_FILE;

		// return a pointer to this newly created project

		// NOT SO FAST... now we have to take this project and create a NEW one in the real place...
		// walk the list of projects, since 4.x project files could've caused more than one project
		// to be loaded
		int num_project = 1;
		int num_unsupported_projects = 0;
		POSITION proj_pos = CProject::m_lstProjects.GetHeadPosition();
		while( proj_pos )
		{
			// get next project
			pProject = static_cast<CProject*>(CProject::m_lstProjects.GetNext( proj_pos ));

			// skip if this is a project type we don't support anymore
			if( IsUnsupportedProject( pProject ) )
			{
				num_unsupported_projects++;
				continue;
			}

			CComQIPtr<VCProject> pNewProject;

			HRESULT hr = CreateEmptyProject(pProject, num_project, bstrProjectPath, pUnkSP, pProjEngine, 
				&pNewProject);
			RETURN_ON_FAIL(hr);

			if (pNewProject)
			{
				// set member project pointer (only for the first project)
				if( num_project == 1 )
					m_pProject = pNewProject;

				// Source Code Control strings
				pNewProject->put_SccProjectName( CComBSTR( pProject->m_strProjSccProjName ) );
				pNewProject->put_SccLocalPath( CComBSTR( pProject->m_strProjSccRelLocalPath ) );

				// create a list of old (no longer valid) platforms
				CStringList lststrPlatforms;
				CreatePlatformsList(lststrPlatforms);

				// add the platforms
				AddRelevantPlatforms(pProject, lststrPlatforms, pNewProject);

				// get configs
				bool bHasRelease = false;
				bool bHasReleaseMinSize = false;

				pProject->InitTargetEnum();
				CString strTargetName;
				CTargetItem *pTarget;
				while (pProject->NextTargetEnum(strTargetName, pTarget))
				{
					CString strTarg = strTargetName;
					strTarg.MakeLower();

					// iterate through the list of invalid platforms, comparing this target
					// name to them
					bool bInvalidTarget = false;
					POSITION pos = lststrPlatforms.GetHeadPosition();
					while( pos != NULL && !bInvalidTarget)
					{
						CString strPlatformName = lststrPlatforms.GetNext( pos );
						if( _tcsstr( (LPCTSTR)strTarg, (LPCTSTR)strPlatformName ) )
							bInvalidTarget = true;	// if we found it, ignore this config (target)
					}
					if( bInvalidTarget )
						continue;
					
					// we made it here, this must be a target for a valid 7.0 platform...

					CString strConfigName;
					if (!GetConfigName(strTargetName, strConfigName))
						return E_FAIL;

					// is this config "Release"?
					if( _tcsicmp( strConfigName, "Release" ) == 0 )
						bHasRelease = true;
					// is this config "Release MinSize"?
					else if( _tcsicmp( strConfigName, "Release MinSize" ) == 0 )
						bHasReleaseMinSize = true;

					CComQIPtr<VCConfiguration> pConfig;
					AddNewConfiguration(strTargetName, strConfigName, pNewProject, num_project != 1, &pConfig);

					if (pConfig)
					{
						CConfigurationRecord* pcr;
						pcr = pProject->ConfigRecordFromConfigName(strTargetName);
						pProject->ForceConfigActive(pcr);

						CProjType * pProjType;
						g_pPrjcompmgr->LookupProjTypeByName(pProject->GetActiveConfig()->GetOriginalTypeName(), pProjType);
						if( pProjType )
						{
							SetConfigurationType(pProjType, pConfig);

							DoGenericAndSpecialCaseProperties(pProject, pConfig);

							if (pProjType->GetUniqueTypeId() != CProjType::generic)
							{
								// Now do the rest of the tools
								const CVCPtrList * pol = pProjType->GetToolsList ();
								for (VCPOSITION pos = pol->GetHeadPosition(); pos != NULL;)
								{
									CBuildTool * pSrcTool = (CBuildTool *)pol->GetNext (pos);
									CString strToolName;
									strToolName = pSrcTool->GetToolName();
									if (!strToolName.IsEmpty())
										SetToolSettings(pProject, pProject, strToolName, NULL, pConfig);
								}
								DoStyleSheetFixups(pNewProject, pConfig);
								DoSharedFileDelete(pConfig);
							}
							ConfigurationTypes cfgType;
							pConfig->get_ConfigurationType( &cfgType );
							if( cfgType == typeUnknown || cfgType == typeGeneric )
							{
								DoMakefileProjectProperties( pProject, pNewProject, pConfig );
							}
						}
						pProject->ForceConfigActive();
					}
				}
				// get files
				HandleFileProperties(pProject, pNewProject);

				hr = DoATLProjectConversionHack(pNewProject, bHasRelease, bHasReleaseMinSize);
				RETURN_ON_FAIL(hr);

				// get all folders (just in case we missed some
				AddFolders(pNewProject, pProject);
			}
			if (num_project > 1)	// we have to do the assigns here; only the first project will get them handled properly
			{
				CComQIPtr<IVCBuildableItem> spBldableProject = pNewProject;
				if (spBldableProject)
					spBldableProject->AssignActions(VARIANT_TRUE);
				CComQIPtr<IVCProjectImpl> spProjectImpl = pNewProject;
				if (spProjectImpl)
					spProjectImpl->put_IsConverted(VARIANT_TRUE);
			}

			num_project++;
		}
		// if there were unsupported projects, put up an error message
		// if there are no valid projects, we have failed
		if( num_project == 0 )
		{
			throw "No valid projects!";
		}

		// if we had an unsupported project
		if( num_project == 1 && num_unsupported_projects > 0 )
		{
			CComQIPtr<IVCProjectEngineImpl> pProjEngineImpl = pProjEngine;
			if( pProjEngineImpl )
				pProjEngineImpl->DoFireReportError( CComBSTR( L"This project type is not supported" ), E_FAIL, L"VC.ProjectConversionProblems" );
			throw "unsupported project type";
		}
		// if we had one or more unsupported projects in a multiple project 4.x
		// project file
		else if( num_project > 1 && num_unsupported_projects > 0 )
		{
			CComQIPtr<IVCProjectEngineImpl> pProjEngineImpl = pProjEngine;
			if( pProjEngineImpl )
				pProjEngineImpl->DoFireReportError( CComBSTR( L"This project contains sub-projects whose type is not supported" ), E_FAIL, L"VC.ProjectConversionProblems" );
		}

		// if we have more than one project, check for project dependencies
		else if( num_project > 1 )
		{
			VCPOSITION pos = g_lstprojdeps.GetHeadPosition();
			while( pos != NULL )
			{
				// get the dependency info
				CProjDepInfo* pProjDep = (CProjDepInfo*)g_lstprojdeps.GetNext( pos );
				CString strProject = pProjDep->strProject;
				CString strTarget = pProjDep->strTarget;

				// add it to our dependency map
				m_Dependencies.SetAt( strProject, strTarget );
			}
		}

		// clean up!!!
		g_pActiveProject = pOldProject;

		// Delete the project object
		pProject->Destroy();
		delete pProject;
		CProject::m_lstProjects.RemoveAll();
		
		return S_OK;
	}
	catch (...)
	{
		goto CreationError;
	}

CreationError:

	g_pActiveProject = pOldProject;

	// Delete the project object
	pProject->Destroy();
	delete pProject;
	CProject::m_lstProjects.RemoveAll();

	g_pActiveProject = pOldProject;
 
	// set back the current directory since it fails to open
	dirOld.MakeCurrent();

	return VCPROJ_E_BAD_PROJ_FILE;
}

STDMETHODIMP CVCProjConvert::GetProject( IDispatch **ppDisp )
{
	CHECK_POINTER_NULL( ppDisp );
	CComQIPtr<IDispatch> pDisp = m_pProject;
	*ppDisp = pDisp.Detach();
	return S_OK;
}

STDMETHODIMP CVCProjConvert::get_HasProjectDependencies( VARIANT_BOOL *pbHasDeps )
{
	if( m_Dependencies.IsEmpty() )
		*pbHasDeps = VARIANT_FALSE;
	else
		*pbHasDeps = VARIANT_TRUE;

	return S_OK;
}

STDMETHODIMP CVCProjConvert::get_ProjectDependenciesCount( long *plNumDeps )
{
	*plNumDeps = (long)m_Dependencies.GetCount();
	return S_OK;
}

STDMETHODIMP CVCProjConvert::GetNextProjectDependency( BSTR* pbstrProject, BSTR* pbstrDependentProject )
{
	CVCString strProject, strDepProject;
	m_Dependencies.GetNextAssoc( m_DependencyEnumPos, strProject, strDepProject );
	*pbstrProject = strProject.AllocSysString();
	*pbstrDependentProject = strDepProject.AllocSysString();
	return S_OK;
}

STDMETHODIMP CVCProjConvert::ResetProjectDependencyEnum()
{
	m_DependencyEnumPos = m_Dependencies.GetStartPosition();
	return S_OK;
}


HRESULT CVCProjConvert::CreateEmptyProject(CProject* pProject, int num_project, BSTR bstrProjectPath,
	IUnknown* pUnkSP, VCProjectEngine* pProjEngine, VCProject** ppNewProject)
{
	CHECK_POINTER_NULL(ppNewProject);
	HRESULT hr = S_OK;
	CComPtr<IDispatch> pDisp;
	CProjType *pProjType = pProject->GetProjType(); 
	CComQIPtr<VCProject> spNewProj;

	// synthesize the full project path and name
	CStringW strName = bstrProjectPath;
	int pos = strName.ReverseFind( L'.' ) + 1;
	strName.Delete( pos, strName.GetLength() - pos );
	strName += L"vcproj";

	// if this is not the first project
	if( (num_project > 1)  && pUnkSP )
	{
		// get the project path & name
		CString strProjPath;
		CString strProjName;
		strProjPath= (const TCHAR*)(pProject->GetWorkspaceDir());
		pProject->GetName( strProjName );

		// get the dte object
		CComQIPtr<IServiceProvider> pServiceProvider = pUnkSP;
		if( !pServiceProvider )
			return E_NOINTERFACE;
		CComQIPtr<_DTE> pDTE;
		hr = pServiceProvider->QueryService(SID_SDTE, IID__DTE, (void **)&pDTE);
		RETURN_ON_FAIL_OR_NULL(hr, pDTE);
		// get the solution object
		CComPtr<IDispatch> pDispSolution;
		hr = pDTE->get_Solution( (Solution**)&pDispSolution );
		CComQIPtr<_Solution> pSolution = pDispSolution;
		RETURN_ON_FAIL_OR_NULL2(hr, pSolution, E_NOINTERFACE);
		// add the project from the wizards' template
		CComPtr<IDispatch> pDispProject;

		// get the path to the template file
		CComPtr<IVsShell> pVsShell;
		HRESULT hr = pServiceProvider->QueryService(SID_SVsShell, IID_IVsShell, (void**)&pVsShell);
		RETURN_ON_FAIL_OR_NULL(hr, pVsShell);
		CComVariant varVirtualRegistryRoot;
		hr = pVsShell->GetProperty( VSSPROPID_VirtualRegistryRoot, &varVirtualRegistryRoot );
		RETURN_ON_FAIL(hr);
		CString strVCKey( varVirtualRegistryRoot.bstrVal );
		strVCKey += "\\Setup\\VC";
		CRegKey key;
		if( key.Open( HKEY_LOCAL_MACHINE, strVCKey, KEY_READ ) != ERROR_SUCCESS )
			return E_FAIL;
		TCHAR szValue[2048];
		DWORD dwCount = 2048;
		// DEPRECATED LONG lRes = key.QueryValue( szValue, _T("ProductDir"), &dwCount );
		LONG lRes = key.QueryStringValue( _T("ProductDir"), szValue, &dwCount );
		key.Close();
		if( lRes != ERROR_SUCCESS )
			return E_FAIL;
		CComBSTR bstrVal = szValue;
		bstrVal += "\\VCWizards\\default.vcproj";

		// Create the project from template
		pSolution->AddFromTemplate( bstrVal, CComBSTR( strProjPath ), CComBSTR( strProjName ), VARIANT_FALSE, (Project**)&pDispProject );
		CComQIPtr<_Project> pProj = pDispProject;
		if( !pProj )
			return E_NOINTERFACE;

		// get the vc project
		pDispProject = NULL;
		pProj->get_Object( &pDispProject );
		spNewProj = pDispProject;

 		// give it the correct name
 		spNewProj->put_Name( CComBSTR( strProjName ) );
		CComQIPtr<IVCBuildableItem> spBldableProj = spNewProj;
		if (spBldableProj)	// actions got assigned during AddFromTemplate, but they don't make sense there
			spBldableProj->UnAssignActions(VARIANT_FALSE);
	}
	else
	{
		CString strProjName;
		pProject->GetName( strProjName );
		hr = pProjEngine->CreateProject( CComBSTR( strProjName ), &pDisp );
		spNewProj = pDisp;
  		spNewProj->put_ProjectFile( CComBSTR( strName ) );
	}

	*ppNewProject = spNewProj.Detach();
	return hr;
}

void CVCProjConvert::AddRelevantPlatforms(CProject* pProject, CStringList& lststrPlatforms, 
	VCProject* pNewProject)
{
	// get platforms
	CString strTargetName;
	CTargetItem *pTarget;
	pProject->InitTargetEnum();
	while (pProject->NextTargetEnum(strTargetName, pTarget))
	{
		CComBSTR bstrPlatform;
		// strTargetName is in form : "<projectname> - <platform-name> <configuration>"
		int i, j;
		i = strTargetName.Find(_T(" - "));
		if (i > 0)
		{
			j = strTargetName.Find(_T(" "), i+3);
			if (j > 0)
			{
				bstrPlatform = strTargetName.Mid(i + 3, j - i - 3);
				CString strTarg = bstrPlatform;
				strTarg.MakeLower();

				// iterate through the list of invalid platforms, comparing this target
				// name to them
				bool bInvalidTarget = false;
				if (strTarg != _T("win32"))	// most likely correct case is win32, so check it first
				{
					POSITION pos = lststrPlatforms.GetHeadPosition();
					while( pos != NULL && !bInvalidTarget)
					{
						CString strPlatformName = lststrPlatforms.GetNext( pos );
						if( _tcsstr( (LPCTSTR)strTarg, (LPCTSTR)strPlatformName ) )
						{
							// if we found it, ignore this config (target)
							bInvalidTarget = true;
							break;
						}
					}
				}
				if( bInvalidTarget )
					continue;

				HRESULT hr = pNewProject->AddPlatform(bstrPlatform);
				VSASSERT(SUCCEEDED(hr), "Failed to add a platform to the project!");
			}
		}
	}
}

bool CVCProjConvert::IsUnsupportedProject(CProject* pProject)
{
	CString strTargetName;
	CTargetItem *pTarget;

	// get the first target and check it's platform
	pProject->InitTargetEnum();
	pProject->NextTargetEnum(strTargetName, pTarget);
	CComBSTR bstrPlatform;
	// strTargetName is in form : "<projectname> - <platform-name> <configuration>"
	int i, j;
	i = strTargetName.Find(_T(" - "));
	if (i > 0)
	{
		j = strTargetName.Find(_T(" "), i+3);
		if (j > 0)
		{
			bstrPlatform = strTargetName.Mid(i + 3, j - i - 3);
			CString strTarg = bstrPlatform;
			strTarg.MakeLower();

			if (strTarg.GetLength() != 4)
				return false;

			int nCRC = (int)strTarg[0] + (int)strTarg[1] + (int)strTarg[2] + (int)strTarg[3];
			if (nCRC == 418 && strTarg[0] == _T('j'))
				return true;
		}
	}
	return false;
}
void CVCProjConvert::SetConfigurationType(CProjType* pProjType, VCConfiguration* pConfig)
{
	ConfigurationTypes configType = typeUnknown;
	switch (pProjType->GetUniqueTypeId())
	{
	case CProjType::application:
	case CProjType::consoleapp:
		configType = typeApplication;
		break;
	case CProjType::dynamiclib:
	case CProjType::sharedlib:
		configType = typeDynamicLibrary;
		break;
	case CProjType::staticlib:
		configType = typeStaticLibrary;
		break;
	case CProjType::generic:
		configType = typeGeneric;
		break;
	default:
	// case CProjType::exttarget:
	// case CProjType::quickwin:
	// case CProjType::unknown_projtype:
	// case CProjType::standardgraphics:
		configType = typeUnknown;
		break;
	}

	pConfig->put_ConfigurationType(configType);
}

void CVCProjConvert::GetConfig(CString& strTargetName, CString& strConfigName, IVCCollection* pConfigs, 
	VCConfiguration** ppNewConfig)
{
	*ppNewConfig = NULL;
	if (pConfigs)
	{
		GetConfigName(strTargetName, strConfigName, TRUE);
		CComBSTR bstrProjConfig = strConfigName;
		CComPtr<IDispatch> pDispConfig;
		pConfigs->Item(CComVariant(bstrProjConfig), &pDispConfig);
		CComQIPtr<VCConfiguration> pConfig = pDispConfig;
		*ppNewConfig = pConfig.Detach();
	}
}

void CVCProjConvert::AddNewConfiguration(CString& strTargetName, CString& strConfigName, 
	VCProject* pNewProject, BOOL bCheckForExisting, VCConfiguration** ppNewConfig)
{
	CComPtr<IDispatch> pDispConfigs;
	pNewProject->get_Configurations(&pDispConfigs);
	CComQIPtr<IVCCollection> pConfigs = pDispConfigs;
	if (bCheckForExisting)
	{
		GetConfig(strTargetName, strConfigName, pConfigs, ppNewConfig);
		if (*ppNewConfig)
			return;
	}
	
	CComBSTR bstrConfig = strConfigName;
	HRESULT hr = pNewProject->AddConfiguration(bstrConfig);
	VSASSERT(SUCCEEDED(hr), "Failed to add a configuration to the project!");
	GetConfig(strTargetName, strConfigName, pConfigs, ppNewConfig);
}

void CVCProjConvert::CreatePlatformsList(CStringList& lststrPlatforms)
{
	CString strPlatformName;
	BOOL bOK = strPlatformName.LoadString(IDS_V4_POWERMAC_PLATFORM);
	VSASSERT(bOK, "Failed to load string from string table!  Are resources initialized properly?");
	strPlatformName.MakeLower();
	lststrPlatforms.AddTail(strPlatformName);
	bOK = strPlatformName.LoadString(IDS_V4_MIPS_PLATFORM);
	VSASSERT(bOK, "Failed to load string from string table!  Are resources initialized properly?");
	strPlatformName.MakeLower();
	lststrPlatforms.AddTail(strPlatformName);
	bOK = strPlatformName.LoadString(IDS_V2_MAC68K_PLATFORM);
	VSASSERT(bOK, "Failed to load string from string table!  Are resources initialized properly?");
	strPlatformName.MakeLower();
	lststrPlatforms.AddTail(strPlatformName);
	bOK = strPlatformName.LoadString(IDS_V2_MACPPC_PLATFORM);
	VSASSERT(bOK, "Failed to load string from string table!  Are resources initialized properly?");
	strPlatformName.MakeLower();
	lststrPlatforms.AddTail(strPlatformName);
	bOK = strPlatformName.LoadString(IDS_V4_PPC_PLATFORM);
	VSASSERT(bOK, "Failed to load string from string table!  Are resources initialized properly?");
	strPlatformName.MakeLower();
	lststrPlatforms.AddTail(strPlatformName);
}


void CVCProjConvert::DoMakefileProjectProperties( CProject* pProject, VCProject* pNewProject, VCConfiguration *pConfig )
{
	// get the makefile project props
	CString strCmdLine, strRebuildOpt, strOutput, strBsc;
	pProject->GetStrProp( P_Proj_CmdLine, strCmdLine );
	pProject->GetStrProp( P_Proj_RebuildOpt, strRebuildOpt);
	pProject->GetStrProp( P_Proj_Targ, strOutput );
	pProject->GetStrProp( P_Proj_BscName, strBsc );

	// synthesize the clean command line
	CString strRebuildCmdLine = strCmdLine + " " + strRebuildOpt;

	// set the props in the new config's NMake tool
	CComPtr<IDispatch> pDispTools;
	CComQIPtr<IVCCollection> pTools;
	pConfig->get_Tools( &pDispTools );
	pTools = pDispTools;
	// only one tool in a makefile config (and this is a one-based collection)
	CComPtr<IDispatch> pDispTool;
	pTools->Item( CComVariant( 1 ), &pDispTool );
	CComQIPtr<VCNMakeTool> pNMakeTool = pDispTool;
	if( pNMakeTool )
	{
		if (!strCmdLine.IsEmpty())
			pNMakeTool->put_BuildCommandLine( CComBSTR( strCmdLine ) );
		if (!strRebuildCmdLine.IsEmpty())
			pNMakeTool->put_ReBuildCommandLine( CComBSTR( strRebuildCmdLine ) );
		if (!strOutput.IsEmpty())
			pNMakeTool->put_Output( CComBSTR( strOutput ) );
	}

	pNewProject->put_Keyword(L"MakeFileProj");
	if (pNewProject && !strBsc.IsEmpty())
	{
		// Since we're in project load, we cannot use the standard methods for determining whether it is OK to
		// add the file.  Instead, we have to go index into the file collection ourselves.  Sigh.
		CComBSTR bstrBsc = strBsc;
		CComPtr<IDispatch> spDispFiles;
		if (FAILED(pNewProject->get_Files(&spDispFiles)))
			return;
		CComQIPtr<IVCCollection> spFiles = spDispFiles;
		if (spFiles == NULL)
			return;

		CComPtr<IDispatch> spDispFile;
		spFiles->Item(CComVariant(bstrBsc), &spDispFile);
		if (spDispFile != NULL)
			return;		// already there

		pNewProject->AddFile(bstrBsc, &spDispFile);	// wasn't there, so add it
	}
}

void CVCProjConvert::DoGenericAndSpecialCaseProperties(CProject* pProject, VCConfiguration* pConfig)
{
	// do generic config properties...
	CString strOutDirs;
	pProject->GetConvertedStrProp(P_OutDirs_Target, strOutDirs);
	pConfig->put_OutputDirectory(CComBSTR(strOutDirs));
	pProject->GetConvertedStrProp(P_OutDirs_Intermediate, strOutDirs);
	pConfig->put_IntermediateDirectory(CComBSTR(strOutDirs));

	// set the config's use of MFC
	int iUseMFC;
	if (pProject->GetIntProp(P_ProjUseMFC, iUseMFC) == valid)
	{
		useOfMfc useMfc = useMfcStdWin;
		switch (iUseMFC)
		{
		case NoUseMFC:
			useMfc = useMfcStdWin;
			break;
		case UseMFCInLibrary:
			useMfc = useMfcStatic;
			break;
		case UseMFCInDll:
			// fall thru
		default:	// same as UseMFCInDll
			useMfc = useMfcDynamic;
			break;
		}

		pConfig->put_UseOfMFC(useMfc);
	}

	// Special case 1: Init runtime lib settings for cl tool differently based on
	// P_UseDebugLibs. This must be done before any other cl settings are processed
	// since they override this setting.
	BOOL fDebugLib = FALSE;
	pProject->GetIntProp(P_UseDebugLibs, fDebugLib);
	CComPtr<IVCToolImpl> pTool;
	CString strTool = "cl.exe";
	SetToolObject(pConfig, strTool, &pTool);
	if (pTool)
	{
		CComQIPtr<VCCLCompilerTool> pClTool = pTool;
		if (pClTool != NULL)
		{
			if( fDebugLib )
				pClTool->put_RuntimeLibrary(rtSingleThreadedDebug);
			else
				pClTool->put_RuntimeLibrary(rtSingleThreaded);

			long lItem = 0;
			COptionHandler* pOptHandler;
			GetOptStrForTool(strTool, pOptHandler, lItem);
			if (pOptHandler)
			{
				CComQIPtr<IVCPropertyContainer> spPropContainer = pConfig;
				pOptHandler->SetSlob(pProject);
				int nVal = 0;
//				gptRet = pOptHandler->GetDefIntProp(pOptHandler->MapLogical(P_Optimize_Y), nVal);
//				if (gptRet == valid)
//					SetBoolProperty(spPropContainer, VCCLID_OmitFramePointers, nVal);
				GPT gptRet = pOptHandler->GetDefIntProp(pOptHandler->MapLogical(P_Optimize_P), nVal);
				if (gptRet == valid)
					SetBoolProperty(spPropContainer, VCCLID_ImproveFloatingPointConsistency, nVal);
				gptRet = pOptHandler->GetDefIntProp(pOptHandler->MapLogical(P_InlineControl), nVal);
				if (gptRet == valid)
					SetEnumProperty(spPropContainer, VCCLID_InlineFunctionExpansion, nVal);
				gptRet = pOptHandler->GetDefIntProp(pOptHandler->MapLogical(P_StringPool), nVal);
				if (gptRet == valid)
					SetStringPooling(pClTool, nVal);
				gptRet = pOptHandler->GetDefIntProp(pOptHandler->MapLogical(P_IncLinking), nVal);
				if (gptRet == valid)
					SetBoolProperty(spPropContainer, VCCLID_EnableFunctionLevelLinking, nVal);
				pOptHandler->ResetSlob();
			}
		}
	}
	// Special case 2: init incremental link settings for link tool differently
	// based on P_UseDebugLibs.  
	// Special case 3: init /pdb for link tool
	// Both must be done before any other link settings are processed since they override these settings.
	pTool.Release();
	strTool = _T("link.exe");
	SetToolObject(pConfig, strTool, &pTool);
	if (pTool)
	{
		CComQIPtr<VCLinkerTool> pLinkTool = pTool;
		if (pLinkTool != NULL)
		{
			SetLinkIncremental(pLinkTool, fDebugLib);
			long lItem = 0;
			COptionHandler* pOptHandler;
			GetOptStrForTool(strTool, pOptHandler, lItem);
			if (pOptHandler)
			{
				CComQIPtr<IVCPropertyContainer> spPropContainer = pConfig;
				pOptHandler->SetSlob(pProject); 
				BOOL bUsePDB = fDebugLib;
				if (pProject->GetIntProp(pOptHandler->MapLogical(P_UsePDB), bUsePDB) != valid)
					bUsePDB = fDebugLib;
				SetUsePDB(spPropContainer, pProject, pOptHandler, bUsePDB);
				
				BOOL bIgnoreLib = FALSE;
				pProject->GetIntProp(pOptHandler->MapLogical(P_IgnoreExportLib), bIgnoreLib);
				if(bIgnoreLib)
				{
					SetBoolProperty(spPropContainer, VCLINKID_IgnoreImportLibrary, bIgnoreLib);
				}
				pOptHandler->ResetSlob();
			}
		}
	}

	// Now do their custom build events
	CString strPreLink = "Before the Linker";
	CString strPostBuild = "Post Build";

	HandleEvent(pProject, pConfig, P_PreLink_Command, P_PreLink_Description, strPreLink);
	HandleEvent(pProject, pConfig, P_PostBuild_Command, P_PostBuild_Description, strPostBuild);
}

void CVCProjConvert::HandleEvent(CProject* pProject, VCConfiguration* pConfig, UINT idCmd, UINT idDesc, 
	CString& strEventName)
{
	CString strCommand;
	CString strDesc;
	pProject->GetConvertedStrProp(idCmd,strCommand);
	// replace tabs (used as separators in vc 5/6) with newlines
	strCommand.Replace( '\t', '\n' );

	if(!strCommand.IsEmpty())
	{
		pProject->GetConvertedStrProp(idDesc, strDesc);
		CComPtr<IVCToolImpl> pTool;
		SetToolObject(pConfig, strEventName, &pTool);
		if (pTool)
		{
			CComQIPtr<IVCBuildEventToolInternal> pBuildEventTool = pTool;
			if (pBuildEventTool != NULL)
			{
				if (!strCommand.IsEmpty())
					pBuildEventTool->put_CommandLineInternal(CComBSTR(strCommand));
				if (!strDesc.IsEmpty())
					pBuildEventTool->put_DescriptionInternal(CComBSTR(strDesc));
			}
		}
	}
}

void CVCProjConvert::RemoveMultiples(CStringW& strList, CStringW& strSeparatorList)
{
	if (strList.IsEmpty())
		return;

	CStringW strOut;
	CStringW strItem;
	bool bFirstElem = true;

	int nNextIdx = 0;
	strList.TrimLeft();
	strList.TrimRight();
	int nMaxIdx = strList.GetLength();
	int nFoundIdx = 0;
	while (nNextIdx >= 0)
	{
		nNextIdx = GetNextItem(strList, nNextIdx, nMaxIdx, strSeparatorList, strItem);
		nFoundIdx = FindItem(strOut, strItem, strSeparatorList, 0);
		if (nFoundIdx >= 0)		// already there
			continue;
		if (!bFirstElem)
		{
			if (!strSeparatorList.IsEmpty())
				strOut += strSeparatorList.GetAt(0);
		}
		else
			bFirstElem = false;
		strOut += strItem;
	}

	if (strOut.IsEmpty())
		strList.Empty();
	else
		strList = strOut;
}

// this function handles clearing properties that have been set on the config that should have been taken as defaults
// from an applied style sheet
void CVCProjConvert::DoStyleSheetFixups(VCProject* pNewProject, VCConfiguration* pConfig)
{
	CComQIPtr<IVCPropertyContainer> spPropContainer = pConfig;
	if (spPropContainer == NULL)
		return;		// not a whole lot we can do...

	// let's start with the MFC style sheets
	useOfMfc useMFC = useMfcStdWin;
	HRESULT hr = pConfig->get_UseOfMFC(&useMFC);
	if (hr == S_OK)
	{
		switch(useMFC)
		{
		case useMfcStdWin:
			RemoveSymbols(spPropContainer, VCLINKID_AdditionalDependencies, L" ", 
				L"kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib");
			break;
		case useMfcStatic:
			ClearBoolProperty(spPropContainer, VCCLID_ExceptionHandling, VARIANT_TRUE /* to match mfcstatic.vcstyle */);
			break;
		case useMfcDynamic:
			RemoveSymbols(spPropContainer, VCCLID_PreprocessorDefinitions, L";,", L"_AFXDLL");
			ClearBoolProperty(spPropContainer, VCCLID_ExceptionHandling, VARIANT_TRUE /* to match mfcdynamic.vcstyle */);
		}
		if (useMFC != useMfcStdWin)
			pNewProject->put_Keyword(L"MFCProj");
	}

	// do we see an aspect of the character set style sheet?
	if (RemoveSymbols(spPropContainer, VCCLID_PreprocessorDefinitions, L";,", L"_UNICODE"))
		pConfig->put_CharacterSet(charSetUnicode);
	else if (RemoveSymbols(spPropContainer, VCCLID_PreprocessorDefinitions, L";,", L"_MBCS"))
		pConfig->put_CharacterSet(charSetMBCS);

	// do we see an aspect of the use of ATL style sheet?
	bool bIsAtl = true;
	if (RemoveSymbols(spPropContainer, VCCLID_PreprocessorDefinitions, L";,", L"_ATL_STATIC_REGISTRY"))
		pConfig->put_UseOfATL(useATLStatic);
	else if (RemoveSymbols(spPropContainer, VCCLID_PreprocessorDefinitions, L";,", L"_ATL_DLL"))
		pConfig->put_UseOfATL(useATLDynamic);
	else
		bIsAtl = false;
	if (bIsAtl)
		pNewProject->put_Keyword(L"AtlProj");

	// do we see the ATL min CRT stuff?
	bool bRemove = RemoveSymbols(spPropContainer, VCCLID_PreprocessorDefinitions, L";,", L"_ATL_MIN_CRT");
	pConfig->put_ATLMinimizesCRunTimeLibraryUsage(bRemove ? VARIANT_TRUE : VARIANT_FALSE);
	ClearBoolProperty(spPropContainer, VCCLID_ExceptionHandling, bRemove ? VARIANT_FALSE : VARIANT_TRUE);
	ClearIntProperty(spPropContainer, VCCLID_Optimization, bRemove ? optimizeMinSpace : optimizeMaxSpeed);

	// now, let's get rid of DLL style sheet elements
	ConfigurationTypes configType = typeApplication;
	hr = pConfig->get_ConfigurationType(&configType);
	if (hr == S_OK)
	{
		if (configType == typeDynamicLibrary || configType == typeApplication)
		{
			CComVariant varLinkDLL;
			if (spPropContainer->GetLocalProp(VCLINKID_LinkDLL, &varLinkDLL) == S_OK)	// this was set one way or another
			{
				if (varLinkDLL.boolVal)
					configType = typeDynamicLibrary;
				else
					configType = typeApplication;
			}
			else	// might be there (or not) due to fake prop info.  Argh.
			{
				CComBSTR bstrFile;
				if (spPropContainer->GetStrProperty(VCLINKID_OutputFile, &bstrFile) == S_OK)
				{
					CString strFile = bstrFile;
					CPath pathFile;
					if (pathFile.Create(strFile))
					{
						CString strExt = pathFile.GetExtension();
						strExt.MakeLower();
						if (strExt.IsEmpty())
						{}	// do nothing
						else if (strExt == _T(".exe"))
							configType = typeApplication;
						else if (strExt == _T(".dll") || strExt == _T(".ocx"))
							configType = typeDynamicLibrary;
					}
				}
			}
			pConfig->put_ConfigurationType(configType);
			if (configType == typeDynamicLibrary)
				RemoveSymbols(spPropContainer, VCCLID_PreprocessorDefinitions, L";,", L"_WINDLL");
			spPropContainer->Clear(VCLINKID_LinkDLL);
		}

	}
}

// This function handles deleting the PCH file, if any, that the config might have created.  It also deletes
// the linker PDB file, if any.  This causes the project to be out of date, but isn't as harsh an edit as 
// doing a 'clean' on the config would have been.
void CVCProjConvert::DoSharedFileDelete(VCConfiguration* pConfig)
{
	CComQIPtr<IVCPropertyContainer> spPropContainer = pConfig;
	if (spPropContainer == NULL)
		return;

	CComPtr<IDispatch> spDisp;
	if (FAILED(pConfig->get_Tools(&spDisp)))
		return;

	CComQIPtr<IVCCollection> spTools = spDisp;
	if (spTools == NULL)
		return;

	spDisp.Release();
	spTools->Item(CComVariant(L"VCCLCompilerTool"), &spDisp);
	CComQIPtr<VCCLCompilerTool> spCL = spDisp;

	spDisp.Release();
	spTools->Item(CComVariant(L"VCLinkerTool"), &spDisp);
	CComQIPtr<VCLinkerTool> spLinker = spDisp;


	CComBSTR bstrPCH;
	CString strPCH;
	if (spCL && SUCCEEDED(spCL->get_PrecompiledHeaderFile(&bstrPCH)) && bstrPCH.Length() > 0)
	{
		if (SUCCEEDED(spPropContainer->Evaluate(bstrPCH, &bstrPCH)) && bstrPCH.Length() > 0)
			strPCH = bstrPCH;
	}

	CComBSTR bstrPDB;
	CString strPDB;
	if (spLinker && SUCCEEDED(spLinker->get_ProgramDatabaseFile(&bstrPDB)) && bstrPDB.Length() > 0)
	{
		if (SUCCEEDED(spPropContainer->Evaluate(bstrPDB, &bstrPDB)) && bstrPDB.Length() > 0)
			strPDB = bstrPDB;
	}

	if (strPCH.IsEmpty() && strPDB.IsEmpty())	// if they're both empty strings, then nothing more to do
		return;

	// we now know that there is supposed to be a PCH and/or PDB file there...
	CComBSTR bstrProjDir;
	if (FAILED(spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir)) || bstrProjDir.Length() == 0)
		return;

	CString strProjDirTmp = bstrProjDir;
	int nLen = strProjDirTmp.GetLength();
	if (strProjDirTmp[nLen-1] != _T('/') && strProjDirTmp[nLen-1] != _T('\\'))
		strProjDirTmp += _T("/");
	strProjDirTmp += _T("foo");	// need to have this be a file, not the directory.  sigh.
	CPath pathProjDirTmp;
	if (!pathProjDirTmp.Create(strProjDirTmp))
		return;

	CDir dirProj;
	if (!dirProj.CreateFromPath(pathProjDirTmp))
		return;

	if (!strPCH.IsEmpty())
	{
		CPath pathPCH;
		if (pathPCH.CreateFromDirAndFilename(dirProj, strPCH))
			::DeleteFile((const char *) pathPCH);	// don't care if it exists or not...
	}

	if (!strPDB.IsEmpty())
	{
		CPath pathPDB;
		if (pathPDB.CreateFromDirAndFilename(dirProj, strPDB))
			::DeleteFile((const char *) pathPDB);	// don't care if it exists or not...
	}
}

bool CVCProjConvert::RemoveSymbols(IVCPropertyContainer* pPropContainer, long idProp, BSTR bstrSeparatorList, BSTR bstrSymbols)
{
	CComBSTR bstrExistingProp;
	HRESULT hr = pPropContainer->GetStrProperty(idProp, &bstrExistingProp);
	if (hr != S_OK)
		return false;		// nothing to do

	CStringW strExistingList = bstrExistingProp;
	CStringW strRemoveList = bstrSymbols;
	CStringW strSeparatorList = bstrSeparatorList;
	CStringW strItem;
	int nMaxIdx = strRemoveList.GetLength();
	int nNextIdx = 0;
	bool bFoundIt = false;
	while (nNextIdx >= 0)
	{
		nNextIdx = GetNextItem(strRemoveList, nNextIdx, nMaxIdx, strSeparatorList, strItem);
		if (RemoveItem(strExistingList, strItem, strSeparatorList))
			bFoundIt = true;
	}

	strExistingList.TrimLeft();
	strExistingList.TrimRight();
	if (strExistingList.IsEmpty())
		pPropContainer->Clear(idProp);
	else
	{
		CComBSTR bstrNewList = strExistingList;
		pPropContainer->SetStrProperty(idProp, bstrNewList);
	}

	return bFoundIt;
}

void CVCProjConvert::ClearBoolProperty(IVCPropertyContainer* pPropContainer, long idProp, VARIANT_BOOL bDefault)
{
	VARIANT_BOOL bPropVal = VARIANT_FALSE;
	HRESULT hr = pPropContainer->GetBoolProperty(idProp, &bPropVal);
	if (hr != S_OK)
		return;		// nothing needs to be done

	if (bPropVal == bDefault)
		pPropContainer->Clear(idProp);
}

void CVCProjConvert::ClearIntProperty(IVCPropertyContainer* pPropContainer, long idProp, long nDefault)
{
	long nPropVal = 0;
	HRESULT hr = pPropContainer->GetIntProperty(idProp, &nPropVal);
	if (hr != S_OK)
		return;		// nothing needs to be done

	if (nPropVal == nDefault)
		pPropContainer->Clear(idProp);
}

int CVCProjConvert::FindFirstSeparator(CStringW& strList, CStringW& strSeparatorList, int nStartIdx)
{
	int cSeparators = strSeparatorList.GetLength();
	if (cSeparators == 0)	// no separators means give back first char of list
		return 0;
	else if (cSeparators == 1)
		return strList.Find(strSeparatorList, nStartIdx);

	// got here; means we have the potential for multiple separator possibilities
	int nMinSep = -1, nThisSep = -1;
	for (int idx = 0; idx < cSeparators; idx++)
	{
		nThisSep = strList.Find(strSeparatorList.GetAt(idx), nStartIdx);
		if (nThisSep >= 0 && ((nThisSep < nMinSep) || (nMinSep < 0)))
			nMinSep = nThisSep;
	}

	return nMinSep;
}

int CVCProjConvert::GetNextItem(CStringW& strList, int nStartIdx, int nMaxIdx, CStringW& strSeparator, CStringW& strItem)
{
	strItem.Empty();
	if (nStartIdx < 0)
		return nStartIdx;
	else if (nStartIdx >= nMaxIdx)
		return -1;

	int nSep = FindFirstSeparator(strList, strSeparator, nStartIdx);
	if (nSep >= 0)
	{
		int nQuote = strList.Find(L'"', nStartIdx);
		if (nQuote >= nStartIdx && nQuote < nSep)	// need to get outside the quoted string
			return GetNextQuotedItem(strList, nStartIdx, nMaxIdx, nSep, nQuote, TRUE, strSeparator, strItem);
		strItem = strList.Mid(nStartIdx, nSep-nStartIdx);
		nSep++;
		return nSep;
	}

	if (nStartIdx < nMaxIdx)
		strItem = strList.Right(nMaxIdx-nStartIdx);

	return -1;
}

int CVCProjConvert::GetNextQuotedItem(CStringW& strList, int nStartIdx, int nMaxIdx, int nSep, int nLastQuote,
	BOOL bOddQuote, CStringW& strSeparator, CStringW& strItem)
{
	if (bOddQuote)
	{
		int nQuote = strList.Find(L'"', nLastQuote+1);
		if (nQuote < nSep)	// doesn't matter that we found a quote
		{
			strItem = strList.Mid(nStartIdx, nSep-nStartIdx);
			nSep++;
			return nSep;
		}
		else
			return GetNextQuotedItem(strList, nStartIdx, nMaxIdx, nSep, nQuote, FALSE, strSeparator, strItem);
	}
	else	// even quote
	{
		nSep = FindFirstSeparator(strList, strSeparator, nLastQuote+1);
		if (nSep >= 0)
		{
			int nQuote = strList.Find(L'"', nLastQuote+1);
			if (nQuote > nLastQuote && nQuote < nSep)	// still need to get outside the quoted string
				return GetNextQuotedItem(strList, nStartIdx, nMaxIdx, nSep, nQuote, TRUE, strSeparator, strItem);
			strItem = strList.Mid(nStartIdx, nSep-nStartIdx);
			nSep++;
			return nSep;
		}

		if (nStartIdx < nMaxIdx)
			strItem = strList.Right(nMaxIdx-nStartIdx);
		
		return -1;
	}
}

bool CVCProjConvert::RemoveItem(CStringW& strList, CStringW& strItem, CStringW& strSeparatorList)
{
	int nFoundIdx = FindItem(strList, strItem, strSeparatorList, 0);
	if (nFoundIdx < 0)
		return false;		// not there

	int nLen = strList.GetLength();
	int nItemLen = strItem.GetLength();
	CStringW strLeft, strRight;
	if (nFoundIdx == 0)
	{
		if (nLen > nItemLen)
			strList = strList.Right(nLen-nFoundIdx-nItemLen-1);
		else
			strList.Empty();
	}
	else if (nFoundIdx + nItemLen == nLen)
		strList = strList.Left(nFoundIdx-1);
	else
	{
		strLeft = strList.Left(nFoundIdx-1);
		strRight = strList.Right(nLen-nFoundIdx-nItemLen);
		strList = strLeft + strRight;
	}

	return true;
}

int CVCProjConvert::FindItem(CStringW& rstrItems, CStringW& rstrFindItem, CStringW& strSeparator, int nStart)
{
	nStart = rstrItems.Find(rstrFindItem, nStart);
	if (nStart < 0)
		return nStart;

	int nItemLen = rstrItems.GetLength();
	int nLen = rstrFindItem.GetLength();

	if (nStart > 0 && nStart + nLen < nItemLen && strSeparator.Find(rstrItems[nStart-1]) < 0)
		return FindItem(rstrItems, rstrFindItem, strSeparator, nStart+nLen);

	else if (nItemLen > nStart +  nLen && strSeparator.Find(rstrItems[nStart+nLen]) < 0)
		return FindItem(rstrItems, rstrFindItem, strSeparator, nStart+nLen);

	return nStart;
}

void CVCProjConvert::SetLinkIncremental(VCLinkerTool* pLinkTool, BOOL bSetIt)
{
	linkIncrementalType oldLinkIncremental = linkIncrementalDefault;
	pLinkTool->get_LinkIncremental(&oldLinkIncremental);
	if (bSetIt && oldLinkIncremental != linkIncrementalYes)
		pLinkTool->put_LinkIncremental(linkIncrementalYes);
	else if (!bSetIt && oldLinkIncremental != linkIncrementalNo)
		pLinkTool->put_LinkIncremental(linkIncrementalNo);
}
								
void CVCProjConvert::HandleFileProperties(CProject* pProject, VCProject* pNewProject)
{
	g_pActiveProject = pProject;
	HBLDFILE hFile = NULL;

	g_BldSysIFace.InitFileEnum((HPROJECT)pProject);

	while (SUCCEEDED((hFile = g_BldSysIFace.GetNextFile())) && hFile != (HBLDFILE)NO_FILE )
	{
		CFileItem *pFileItem = (CFileItem *)hFile;
		if (pFileItem == NULL)
			continue;

		CSlob* pSlobParent = pFileItem->GetContainer();

		CComPtr<VCFilter> pFilter;
		if (pSlobParent && pSlobParent->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			AddFolderForFile(pNewProject, pSlobParent, NULL, &pFilter);
		}

		if (pFileItem->IsKindOf(RUNTIME_CLASS(CTimeCustomBuildItem)))
		{
			// Go through all configs and set properties per config per file
			g_BldSysIFace.InitConfigEnum((HPROJECT)pProject);
			HCONFIGURATION hConfig;
			CString strTarget;
			while (hConfig = g_BldSysIFace.GetNextConfig(strTarget, (HPROJECT)pProject))
			{
				CComPtr<VCConfiguration> pConfig;
				GetNamedProjectConfiguration(strTarget, pNewProject, &pConfig);

				CConfigurationRecord * pcr = g_BldSysIFace.GetConfigRecFromConfigI(hConfig, 
					(HPROJECT)pProject);
				pProject->ForceConfigActive(pcr);
				AddCustomBuildEventItem(pFileItem, pConfig);
  				pProject->ForceConfigActive();
			} // while next config
		}
		else
		{
			const CPath *path = pFileItem->GetFilePath();
			CDir dir = pProject->GetWorkspaceDir();
			CString strRelPath;
			path->GetRelativeName( dir, strRelPath );
			CComBSTR bstrPath( strRelPath );
			CComPtr<IDispatch> pDispFile;
			HRESULT hr = S_OK;
			if (pFilter)
			{
				hr = pFilter->AddFile(bstrPath, &pDispFile);
			}
			else
			{
				hr = pNewProject->AddFile(bstrPath, &pDispFile);
			}
			CComQIPtr<VCFile> pFile = pDispFile;
			VSASSERT(SUCCEEDED(hr) && pFile, "Returned 'file' isn't a file!");
			if (pFileItem && pFile)
			{
				//case P_ItemExcludedFromScan:
				//case P_ItemCustomBuildExt:
				// do configuration-unspecific properties...

// 				CString strRelPath;
// 				pFileItem->GetConvertedStrProp(P_ProjItemOrgPath, strRelPath);
// 				CComBSTR bstrRelPath = strRelPath;
// 				pFile->put_RelativePath(bstrRelPath);

				// Go through all configs and set properties per config per file
				g_BldSysIFace.InitConfigEnum((HPROJECT)pProject);
				HCONFIGURATION hConfig;
				CString strTarget;
				while (hConfig = g_BldSysIFace.GetNextConfig(strTarget, (HPROJECT)pProject))
				{
					CComPtr<VCFileConfiguration> pFileConfig;
					GetNamedFileConfiguration(strTarget, pFile, &pFileConfig);

					CString strTool;
					// get this target's config record
					CConfigurationRecord * pcr = 
						g_BldSysIFace.GetConfigRecFromConfigI(hConfig, (HPROJECT)pProject);
					pProject->ForceConfigActive(pcr);
					
					pFileItem->GetStrProp(P_ItemBuildTool, strTool);
					if (!strTool.IsEmpty())
					{
						SetToolSettings(pFileItem, pProject, strTool, pFileConfig, NULL);
					}

					const CPath* pPath = pFileItem->GetFilePath();
					VSASSERT(pPath != NULL, "File item has no path!  Was it added to the project correctly originally?");
					if (pPath)
					{
						CString strExt = pPath->GetExtension();
						strExt.MakeLower();
						if (strExt == _T(".def"))
							SetLinkerDefSettings(pFileItem, pFileConfig);
					}
  					pProject->ForceConfigActive();
				} // while next config
			}
		}
	}  // while next file
}

HRESULT CVCProjConvert::GetNamedProjectConfiguration(CString& strTarget, VCProject* pNewProject, 
	VCConfiguration** ppNewConfig)
{
	CHECK_POINTER_NULL(ppNewConfig);
	*ppNewConfig = NULL;

	CComPtr<IDispatch> pDispConfigs;
	HRESULT hr = pNewProject->get_Configurations(&pDispConfigs);
	CComQIPtr<IVCCollection> pConfigs = pDispConfigs;
	RETURN_ON_FAIL_OR_NULL(hr, pConfigs);

	CString strConfigName;
	if (!GetConfigName(strTarget, strConfigName, TRUE))
		return E_FAIL;
	CComBSTR bstrConfig = strConfigName;

	CComPtr<IDispatch> pDispConfig;
	hr = pConfigs->Item(CComVariant(bstrConfig), &pDispConfig);
	CComQIPtr<VCConfiguration> pConfig = pDispConfig;
	RETURN_ON_FAIL_OR_NULL(hr, pConfig);

	*ppNewConfig = pConfig.Detach();
	return hr;
}

HRESULT CVCProjConvert::GetNamedFileConfiguration(CString& strTarget, VCFile* pFile, 
	VCFileConfiguration** ppNewFileConfig)
{
	CHECK_POINTER_NULL(ppNewFileConfig);
	*ppNewFileConfig = NULL;

	CComPtr<IDispatch> pDispFileConfigs;
	HRESULT hr = pFile->get_FileConfigurations(&pDispFileConfigs);
	CComQIPtr<IVCCollection> pFileConfigs = pDispFileConfigs;
	RETURN_ON_FAIL_OR_NULL(hr, pFileConfigs);

	CString strConfigName;
	if (!GetConfigName(strTarget, strConfigName, TRUE))
		return E_FAIL;
	CComBSTR bstrConfig = strConfigName;

	CComPtr<IDispatch> pDispFileConfig;
	hr = pFileConfigs->Item(CComVariant(bstrConfig), &pDispFileConfig);
	CComQIPtr<VCFileConfiguration> pFileConfig = pDispFileConfig;
	RETURN_ON_FAIL_OR_NULL(hr, pFileConfig);

	*ppNewFileConfig = pFileConfig.Detach();
	return hr;
}

// HACK for ATL project conversion:
// ATL 5/6 projects don't have a "Release" config, so "Release
// MinSize" should be copied into "Release"...
// if there was no "Release" config, but there was a "Release MinSize"...
HRESULT CVCProjConvert::DoATLProjectConversionHack(VCProject* pNewProject, bool bHasRelease, 
	bool bHasReleaseMinSize)
{
	HRESULT hr = S_OK;

 	if( !bHasRelease && bHasReleaseMinSize )
	{
		// copy Release MinSize into Release
		// get the configs collection
		CComPtr<IDispatch> pDisp;
		HRESULT hr = pNewProject->get_Configurations( &pDisp );
		CComQIPtr<IVCCollection> pCollection = pDisp;
		RETURN_ON_FAIL_OR_NULL2(hr, pCollection, E_NOINTERFACE);
		
		// assume the platform is "Win32"
		CComBSTR bstrNewCfgName = L"Release|Win32";
		CComBSTR bstrOldCfgName = L"Release MinSize|Win32";
		// look up these names in the config collections
		CComPtr<IDispatch> pDispNewCfg;
		CComVariant varIdx = bstrNewCfgName;
		hr = pCollection->Item( varIdx, &pDispNewCfg );
		CComQIPtr<VCConfiguration> pNewCfg = pDispNewCfg;
		RETURN_ON_FAIL_OR_NULL2(hr, pNewCfg, S_OK);
		varIdx = bstrOldCfgName;
		CComPtr<IDispatch> pDispOldCfg;
		hr = pCollection->Item( varIdx, &pDispOldCfg );
		CComQIPtr<VCConfiguration> pOldCfg = pDispOldCfg;
		RETURN_ON_FAIL_OR_NULL(hr, pOldCfg);
		// copy the config
		hr = pOldCfg->CopyTo( pDispNewCfg );
	}

	return hr;
}

// TODO: Consider doing a get_parent kind of thing to add filters, and then do AddFolder at end to make sure we got them all

void CVCProjConvert::AddFolders(VCProject *pProject, CSlob * pSlob)
{
	CObList *obList = pSlob->GetContentList();
	POSITION pos, pos2;
	CSlob * pSlobT, *pSlobG;

	pos = obList->GetHeadPosition();
	while (pos)
	{
		pSlobT = (CSlob *)obList->GetNext(pos);
		if (pSlobT && pSlobT->IsKindOf(RUNTIME_CLASS(CTargetItem)))
		{
			CObList *obList2 = pSlobT->GetContentList();
			pos2 = obList2->GetHeadPosition();
			while (pos2)
			{
				pSlobG = (CSlob *)obList2->GetNext(pos2);
				if (pSlobG && pSlobG->IsKindOf(RUNTIME_CLASS(CProjGroup)))
				{
					CComPtr<IDispatch> pDispFilter;
					CComQIPtr<VCFilter> pFilter;
					CProjGroup *pGroup = (CProjGroup *)pSlobG;
					if (pGroup->m_strDefaultExtensions != _T("bev"))
					{
						CComBSTR bstrFilterName = pGroup->m_strGroupName;

						// check if this folder already exists 
						CComPtr<IDispatch> pDispColl;
						pProject->get_Filters( &pDispColl );
						VSASSERT( pDispColl, "Can't get Filters collection from Filter object!" );
						CComQIPtr<IVCCollection> pColl = pDispColl;
						VSASSERT( pColl, "Filters collection doesn't support IVCCollection. How in the world did this happen?" );
						CComPtr<IDispatch> pDispItem;
						pColl->Item( CComVariant( bstrFilterName ), &pDispItem );
						if( !pDispItem )
						{
							pProject->AddFilter(bstrFilterName, &pDispFilter);
							pFilter = pDispFilter;
						}
						if (pFilter)
						{
							CComBSTR bstrFilters = pGroup->m_strDefaultExtensions;
							pFilter->put_Filter(bstrFilters);
							AddSubFolders(pFilter, pSlobG);
						}
					}
				}
			}
		}
	}

}
void CVCProjConvert::AddSubFolders(VCFilter *pFilter, CSlob * pSlob)
{
	CObList *obList = pSlob->GetContentList();
	POSITION pos;
	CSlob * pSlobT;

	pos = obList->GetHeadPosition();
	while (pos)
	{
		pSlobT = (CSlob *)obList->GetNext(pos);
		if (pSlobT && pSlobT->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			CComPtr<IDispatch> pDispFilter;
			CComQIPtr<VCFilter> pFilterNew;
			CProjGroup *pGroup = (CProjGroup *)pSlobT;
			CComBSTR bstrFilterName = pGroup->m_strGroupName;

			// check if this folder already exists 
			CComPtr<IDispatch> pDispColl;
			pFilter->get_Filters( &pDispColl );
			VSASSERT( pDispColl, "Can't get Filters collection from Filter object!" );
			CComQIPtr<IVCCollection> pColl = pDispColl;
			VSASSERT( pColl, "Filters collection doesn't support IVCCollection. How in the world did this happen?" );
			CComPtr<IDispatch> pDispItem;
			pColl->Item( CComVariant( bstrFilterName ), &pDispItem );
			if( !pDispItem )
			{
				pFilter->AddFilter(bstrFilterName, &pDispFilter);
				pFilterNew = pDispFilter;
			}
			if (pFilterNew)
			{
				CComBSTR bstrFilters = pGroup->m_strDefaultExtensions;
				pFilterNew->put_Filter(bstrFilters);
				AddSubFolders(pFilterNew, pSlobT);
			}
		}
	}

}

void CVCProjConvert::AddFolderForFile(VCProject *pProject, CSlob *pSlob, VCFile *pFile, VCFilter **ppFilter)
{
	if (pSlob && pSlob->IsKindOf(RUNTIME_CLASS(CProjGroup)))
	{
		CProjGroup *pGroup = (CProjGroup *)pSlob;
		CComBSTR bstrFilterName = pGroup->m_strGroupName;
		CString strDefaultExtensions;
		pGroup->GetStrProp(P_GroupDefaultFilter, strDefaultExtensions);
		CComBSTR bstrFilters = strDefaultExtensions;
		
		CSlob *pSlobT = pSlob->GetContainer();
		CComPtr<IDispatch> pDispFilter;
		CComQIPtr<VCFilter> pFilterNew;
		if (pSlobT && pSlobT->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		{
			CComPtr<VCFilter> pFilterParent;
			AddFolderForFile(pProject, pSlobT, NULL, &pFilterParent);
			if (pFilterParent)
			{
				// check if this folder already exists
				CComPtr<IDispatch> pDispColl;
				pFilterParent->get_Filters( &pDispColl );
				VSASSERT( pDispColl, "Can't get Filters collection from Filter object!" );
				CComQIPtr<IVCCollection> pColl = pDispColl;
				VSASSERT( pColl, "Filters collection doesn't support IVCCollection. How in the world did this happen?" );
				CComPtr<IDispatch> pDispItem;
				pColl->Item( CComVariant( bstrFilterName ), &pDispItem );
				if( !pDispItem )
					pFilterParent->AddFilter(bstrFilterName, &pDispFilter);
				else 
					pDispFilter = pDispItem;
			}
		}
		else
		{
			// check if this folder already exists
			CComPtr<IDispatch> pDispColl;
			pProject->get_Filters( &pDispColl );
			VSASSERT( pDispColl, "Can't get Filters collection from Project object!" );
			CComQIPtr<IVCCollection> pColl = pDispColl;
			VSASSERT( pColl, "Filters collection doesn't support IVCCollection. How in the world did this happen?" );
			CComPtr<IDispatch> pDispItem;
			pColl->Item( CComVariant( bstrFilterName ), &pDispItem );
			if( !pDispItem )
				pProject->AddFilter(bstrFilterName, &pDispFilter);
			else
				pDispFilter = pDispItem;
		}
		pFilterNew = pDispFilter;
		if (pFilterNew)
		{
			pFilterNew->put_Filter(bstrFilters);
			if (ppFilter)
				pFilterNew.CopyTo(ppFilter);
			if (pFile)
			{
				CComQIPtr<IVCFilterImpl> pFilterImpl = pFilterNew; // TODO: Need File::MoveTo()
				if (pFilterImpl)
				{
					pFilterImpl->AddItem(pFile);
				}
			}
		}
	}
}

COptStr * CVCProjConvert::GetOptStrForTool(LPCTSTR szTool, COptionHandler * &popthdlr, long & lItem)
{
	long lItemT = 0;
	g_pPrjcompmgr->InitOptHdlrEnum();
	while (g_pPrjcompmgr->NextOptHdlr(popthdlr) && popthdlr)
	{
		if (popthdlr->m_pAssociatedBuildTool && popthdlr->m_pAssociatedBuildTool->GetToolName() == szTool)
		{
			if (lItem == lItemT)
			{
				COptStr * poptStr;
				poptStr = popthdlr->GetOptionStringTable();
				lItem++;
				return (poptStr);
			}
		lItemT++;
		}
	}
	return NULL;
}

// we're here to set the /DEF switch for the configuration; the last non-excluded from build .def file wins
// NOTE: we're assuming that we got called because the tool associated with a file was the linker and that
// should only be true for .def files.  If some other such case shows up, then we need to do some extra
// checking here.  Otherwise, it isn't worth taking the time to determine that the extension is, indeed, .def.
void CVCProjConvert::SetLinkerDefSettings(CProjItem *pFileItem, VCFileConfiguration *pFileConfig)
{
	if (pFileItem == NULL || pFileConfig == NULL)
	{
		VSASSERT(FALSE, "Trying to set the def file on a non-file item");	// shouldn't be here, then...
		return;
	}

	BOOL bExcludedFromBuild = FALSE;
	pFileItem->GetIntProp(P_ItemExcludedFromBuild, bExcludedFromBuild);
	if (bExcludedFromBuild)
		return;		// nothing to do, then

	CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = pFileConfig;
	if (spFileCfgImpl == NULL)
	{
		VSASSERT(FALSE, "File config isn't what it's supposed to be!");
		return;
	}

	CComPtr<IDispatch> spDispProjCfg;
	spFileCfgImpl->get_Configuration(&spDispProjCfg);
	CComQIPtr<VCConfiguration> spProjCfg = spDispProjCfg;
	VSASSERT(spProjCfg != NULL, "Project config either doesn't exist or isn't what it's supposed to be!");
	if (spProjCfg == NULL)
		return;

	CComPtr<IDispatch> spDispTools;
	spProjCfg->get_Tools(&spDispTools);
	CComQIPtr<IVCCollection> spTools = spDispTools;
	VSASSERT(spTools != NULL, "Tool list not initialized in project engine!");
	if (spTools == NULL)
		return;

	CComPtr<IDispatch> spDispTool;
	spTools->Item(CComVariant(L"VCLinkerTool"), &spDispTool);
	CComQIPtr<VCLinkerTool> spTool = spDispTool;
	if (spTool == NULL)
		return;

	CPath path = *(pFileItem->GetFilePath());
	path.SetAlwaysRelative();
	CDir projDir = pFileItem->GetProject()->GetWorkspaceDir();
	CString strDefFile;
	BOOL bOK = path.GetRelativeName(projDir, strDefFile);
	VSASSERT(bOK, "Unable to get relative path name for def file!");

	CComBSTR bstrDefFile = strDefFile;
	spTool->put_ModuleDefinitionFile(bstrDefFile);
}

void CVCProjConvert::SetToolSettings(CProjItem *pFileItem, CProject* pProject, LPCTSTR szTool, 
	VCFileConfiguration *pFileConfig, VCConfiguration *pConfig)
{
	OptBehaviour optOld = pFileItem->SetOptBehaviour(OBShowMacro);
	CComPtr<IVCToolImpl> pTool;
	CComQIPtr<IVCPropertyContainer> spPropContainer;
	bool bIsBase = false;
	if (pFileConfig)
	{
		SetToolObject(pFileConfig, szTool, &pTool);
		BOOL bExcludedFromBuild = FALSE;
		pFileItem->GetIntProp(P_ItemExcludedFromBuild, bExcludedFromBuild);
		if (bExcludedFromBuild)
			pFileConfig->put_ExcludedFromBuild(VARIANT_TRUE);
		spPropContainer = pFileConfig;
	}
	else if (pConfig)
	{
		SetToolObject(pConfig, szTool, &pTool);
		spPropContainer = pConfig;
		bIsBase = true;
	}

	if (pTool)
	{
		CComQIPtr<VCCustomBuildTool> pCustomBuild = pTool;
		if (pCustomBuild)
		{
			AddCustomBuildItem(pFileItem, pCustomBuild, pFileConfig, pConfig);
		}
		else
		{
			COptionHandler *pOptHandler;
			long lItem = 0;
			COptStr * poptStr = GetOptStrForTool(szTool, pOptHandler, lItem);
			while (poptStr && pOptHandler)
			{
				pOptHandler->SetSlob(pFileItem); 
				while (poptStr->idOption != (UINT)-1)
				{
					if (poptStr->rgidArg[0] != -1)
					{
						UINT idOption = poptStr->rgidArg[0];
						CString strVal;
						int intVal;
						GPT gptRet;
						CComVariant var;
						PROP_TYPE pType = pOptHandler->GetDefOptionType(idOption);
						if (pType == null)
						{
							SUBTYPE subtype = pOptHandler->GetDefOptionSubType(idOption);
							switch (subtype)
							{
							case boolST:
								pType = integer;
								break;
							case  intST:
								pType = integer;
								break;
							case  hexST:
								pType = integer; 
								break;
							case  octST:
								pType = integer;
								break;
							case  strST:
								pType = string;
								break;
							case  pathST:
								pType = string;
								break;
							case  dirST:
								pType = string;
								break;
							default: // null
								pType = null;
								break;
							}
						}
						switch (pType)
						{
						case string:
							gptRet = pFileItem->GetConvertedStrProp(idOption, strVal);
							if (gptRet == invalid && bIsBase)
							{
								gptRet = pOptHandler->GetDefStrProp(idOption, strVal);
								if (gptRet == valid)
									::ConvertMacros(strVal);
							}
							var = strVal;
							break;
						default:
							gptRet = pFileItem->GetIntProp(idOption, intVal);
							var = intVal;
							break;
						}
						if (gptRet == valid)
						{
							SetToolSetting(pTool, spPropContainer, var, poptStr, pFileItem, pOptHandler, bIsBase);
						}
					}
					poptStr++;
				}
				BOOL bHandled = DoSpecialLinkerProps(pTool, spPropContainer, pOptHandler, pFileItem);
				if (!bHandled)
					bHandled = DoSpecialMidlProps(pTool, pOptHandler, pFileItem);
				if (!bHandled)
					DoSpecialUnknownStringProps(pTool, spPropContainer, pOptHandler, pFileItem);
				pOptHandler->ResetSlob();
				poptStr = GetOptStrForTool(szTool, pOptHandler, lItem); // next Opt table
			}

			CString strIntDir, strDummy;
			if (pFileConfig && pProject != pFileItem && pFileItem->GetOutDirString(strDummy, P_OutDirs_Intermediate, &strIntDir))
			{
				CString strProjIntDir;
				if (pProject->GetOutDirString(strDummy, P_OutDirs_Intermediate, &strProjIntDir)  && strIntDir != strProjIntDir)
				{
					// hmmm.  first cut says they're different.  verify
					CleanupDirSpec(strIntDir);
					CleanupDirSpec(strProjIntDir);
					if (strIntDir != strProjIntDir)		// definitely different
					{
						CComPtr<IDispatch> spDispTool;
						if (SUCCEEDED(pFileConfig->get_Tool(&spDispTool)))
						{
							CComBSTR bstrIntDir = strIntDir;
							CComQIPtr<VCCLCompilerTool> spCLTool = spDispTool;
							if (spCLTool)
								spCLTool->put_ObjectFile(bstrIntDir);
							CComQIPtr<VCResourceCompilerTool> spRCTool = spDispTool;
							if (spRCTool)
								spRCTool->put_ResourceOutputFileName(bstrIntDir);
							CComQIPtr<VCMidlTool> spMidlTool = spDispTool;
							if (spMidlTool)
								spMidlTool->put_OutputDirectory(bstrIntDir);
						}
					}
				}
			}
		}
	}

	pFileItem->SetOptBehaviour(optOld);
}

void CVCProjConvert::CleanupDirSpec(CString& strDir)
{
	strDir.TrimLeft();
	strDir.TrimRight();
	if (strDir.IsEmpty())
		return;	// not a whole lot we can do

	int nLen = strDir.GetLength();
	if (nLen == 2 && (strDir  == _T(".\\") || strDir == _T("./")))
	{
		strDir = _T("./");	// leave it looking like this
		return;
	}
	else if (nLen == 1 && (strDir == _T("\\") || strDir == _T("/")))
	{
		strDir = _T("/");	// more or less leave it alone
		return;
	}
	if (nLen == 2 && (strDir[0] == _T('\\') || strDir[0] == _T('/')))
		return;		// not much else we can do here...

	if (strDir[nLen-1] == _T('/'))
		return;		// looks good

	if (strDir[nLen-1] == _T('\\'))
	{
		nLen--;
		strDir = strDir.Left(nLen);		// want to trail a forward slash, not a back one
	}
	else if (strDir[nLen-1] == _T(')'))		// ah.  macro.
		return;		// leave it alone

	strDir += _T("/");
}

BOOL CVCProjConvert::DoSpecialLinkerProps(IVCToolImpl* pTool, IVCPropertyContainer* pPropContainer, COptionHandler* pOptHandler, 
	CProjItem* pFileItem)
{
	CComQIPtr<VCLinkerTool> spLinkTool = pTool;
	if (spLinkTool != NULL)
	{
		pOptHandler->SetSlob(pFileItem); 
		UINT nIDUnknownOption, nIDUnknownString;
		pOptHandler->GetSpecialOptProps(nIDUnknownOption, nIDUnknownString, OBInherit);
		if (nIDUnknownString != (UINT)-1)
		{
			CProject* pProj = pFileItem->GetProject();
			if (pProj != NULL)
			{
				CString strUnknown;
				GPT gptRet = pProj->GetConvertedStrProp(nIDUnknownString, strUnknown);
				if (gptRet == valid)
				{
					CStringW strUnknownW = strUnknown, strSeparator = L" ";
					RemoveMultiples(strUnknownW, strSeparator);
					CComBSTR bstrUnknown = strUnknownW;
					SetStrProperty(pPropContainer, VCLINKID_AdditionalDependencies, bstrUnknown);
				}
				gptRet = pProj->GetConvertedStrProp(nIDUnknownOption, strUnknown);
				if (gptRet == valid)
				{
					int nStringIdx = 0;
					while (nStringIdx >= 0)
					{
						CString strOpt;
						GetUnknownToken(strUnknown, strOpt, nStringIdx);
						if (strOpt.IsEmpty())
						{}	// nothing to do
						else if (_tcsnicmp(strOpt, _T("/opt:ref"), 8) == 0)
							spLinkTool->put_OptimizeReferences(optReferences);
						else if (_tcsnicmp(strOpt, _T("/opt:noref"), 10) == 0)
							spLinkTool->put_OptimizeReferences(optNoReferences);
						else if (_tcsnicmp(strOpt, _T("/opt:win98"), 10) == 0)
							spLinkTool->put_OptimizeForWindows98(optWin98Yes);
						else if (_tcsnicmp(strOpt, _T("/opt:nowin98"), 12) == 0)
							spLinkTool->put_OptimizeForWindows98(optWin98No);
						else if (_tcsnicmp(strOpt, _T("/opt:noicf"), 10) == 0)
							spLinkTool->put_EnableCOMDATFolding(optNoFolding);
						else if (_tcsnicmp(strOpt, _T("/opt:icf"), 8) == 0)
						{
							CString strOpt2 = strOpt.Right(strOpt.GetLength()-8);
							if (!strOpt2.IsEmpty())
							{
								if (strOpt2[0] == _T(','))
								{
									strOpt2 = strOpt.Right(strOpt.GetLength()-1);
									strOpt2.TrimRight();
									long lVal = atoi(strOpt2);
									if (lVal == 2)
										spLinkTool->put_EnableCOMDATFolding(optFolding);
									else if (lVal != 0)
									{
										CComBSTR bstrOpt = strOpt;
										AddAdditionalOptions(pPropContainer, VCLINKID_AdditionalOptions, bstrOpt);
									}
								}
								else
									spLinkTool->put_EnableCOMDATFolding(optFolding);
							}
							else
								spLinkTool->put_EnableCOMDATFolding(optFolding);
						}
						else if (_tcsnicmp(strOpt, _T("/base:"), 6) == 0)	// table lookups don't like this one for some reason...
						{
							strOpt = strOpt.Right(strOpt.GetLength()-6);
							strOpt.TrimRight();
							CComBSTR bstrOpt = strOpt;
							SetStrProperty(pPropContainer, VCLINKID_BaseAddress, bstrOpt);
						}
						// looking to see more of these unknown props show up...
					}
				}
			}
		}
		pOptHandler->ResetSlob();
		return TRUE;	// was linker tool
	}

	return FALSE;	// was not linker tool
}

void CVCProjConvert::GetUnknownToken(CString& rstrString, CString& rstrOpt, int& rnIdx)
{
	int nLen = rstrString.GetLength();
	if (rnIdx >= nLen-1)
	{
		rnIdx = -1;
		return;
	}

	int nEnd = rnIdx;
	BOOL bInSingleQuotes = FALSE, bInDoubleQuotes = FALSE;
	BOOL bDone = FALSE;
	for (nEnd = rnIdx; nEnd < nLen && !bDone; nEnd++)
	{
		if (rstrString[nEnd] == _T('"'))
			bInDoubleQuotes = !bInDoubleQuotes;
		else if (rstrString[nEnd] == _T('\''))
			bInSingleQuotes = !bInSingleQuotes;
		else if (bInSingleQuotes || bInDoubleQuotes)
			continue;

		bDone = _istspace(rstrString[nEnd]);
	}

	if (nEnd > rnIdx + 1)
		rstrOpt = rstrString.Mid(rnIdx, nEnd-1);	// don't want that trailing space if there (and don't want to index past end if not)
	else
		rstrOpt.Empty();

	rnIdx = nEnd;
	while (rnIdx < nLen && _istspace(rstrString[rnIdx]))	// skip over any white space between/after options
		rnIdx++;
	if (rnIdx == nLen)
		rnIdx = -1;
}

BOOL CVCProjConvert::DoSpecialMidlProps(IVCToolImpl* pTool, COptionHandler* pOptHandler, CProjItem* pFileItem)
{
	CComQIPtr<VCMidlTool> spMidlTool = pTool;
	if (spMidlTool != NULL)
	{
		pOptHandler->SetSlob(pFileItem); 
		UINT nIDUnknownOption, nIDUnknownString;
		pOptHandler->GetSpecialOptProps(nIDUnknownOption, nIDUnknownString, OBInherit);
		if (nIDUnknownString != (UINT)-1)
		{
			CProject* pProj = pFileItem->GetProject();
			if (pProj != NULL)
			{
				CString strUnknown;
				GPT gptRet = pProj->GetConvertedStrProp(nIDUnknownString, strUnknown);
				if (gptRet == valid)
				{
					if (strUnknown.Find(_T("/win64")) >= 0)
						spMidlTool->put_TargetEnvironment(midlTargetWin64);
					else	// 'old' default...
						spMidlTool->put_TargetEnvironment(midlTargetWin32);
				}
			}
		}
		CComBSTR bstrTlb;
		HRESULT hr2 = spMidlTool->get_TypeLibraryName(&bstrTlb);
		if (hr2 != S_OK)
		{
			CString strProj, strBase;
			GPT gpt = pFileItem->GetStrProp(P_ProjItemName, strProj);
			if (gpt == valid && !strProj.IsEmpty())
			{
				CPath pathProj;
				pathProj.Create(strProj);
				pathProj.GetBaseNameString(strBase);
			}

			if (!strBase.IsEmpty())
			{
				CProjItem* pItem = pFileItem;
				CString strVal;
				GPT gpt = pItem->GetConvertedStrProp(P_OutDirs_Target, strVal);
				while (gpt != valid)
				{
					// *chain* the proper. config.
					CProjItem * pItemOld = pItem;
					pItem = (CProjItem *)pItem->GetContainerInSameConfig();
					if (pItemOld != pOptHandler->GetSlob())
						pItemOld->ResetContainerConfig();

					VSASSERT(pItem != (CSlob *)NULL, "No valid container found with P_OutDirs_Target set!");
					gpt = pItem->GetConvertedStrProp(P_OutDirs_Target, strVal);
				}

				// reset the last container we found
				if (pItem != pOptHandler->GetSlob())
					pItem->ResetContainerConfig();

				if (!strVal.IsEmpty())
				{
					// If the output directory doesn't end in a forward slash
					// or a backslash, append one.
					const TCHAR * pchT = (const TCHAR *)strVal + strVal.GetLength();
					pchT = _tcsdec((const TCHAR *)strVal, (TCHAR *)pchT);

					if (*pchT != _T('/') && *pchT != _T('\\'))
						strVal += _T('/');
				}

				strVal += strBase + _TEXT(".tlb") ;
				bstrTlb = strVal;
				spMidlTool->put_TypeLibraryName(bstrTlb);
			}
		}
		// for ODL files, we need to make sure that /h is set or the first time the 
		// user builds, they may get their <projname>.h file overwritten.
		CComBSTR bstrHeader;
		hr2 = spMidlTool->get_HeaderFileName(&bstrHeader);
		if (hr2 != S_OK)
		{
			const CPath* pPath = pFileItem->GetFilePath();
			VSASSERT(pPath != NULL, "File item not initialized properly");
			if (pPath)
			{
				CString strExt = pPath->GetExtension();
				strExt.MakeLower();
				if (strExt == _T(".odl"))
				{
					CString strFile;
					pPath->GetBaseNameString(strFile);
					strFile += _T("_h.h");
					bstrHeader = strFile;
					spMidlTool->put_HeaderFileName(bstrHeader);
				}
			}
		}
		pOptHandler->ResetSlob();
		return TRUE;	// was MIDL tool
	}
	return FALSE;	// was not MIDL tool
}

BOOL CVCProjConvert::DoSpecialUnknownStringProps(IVCToolImpl* pTool, IVCPropertyContainer* pPropContainer,
	COptionHandler* pOptHandler, CProjItem* pFileItem)
{
	// NOTE: linker and MIDL handle the unknown string prop individually
	CComQIPtr<VCCLCompilerTool> spCLTool = pTool;
	if (spCLTool)
		return DoUnknownStringProp(pPropContainer, pOptHandler, pFileItem, VCCLID_AdditionalOptions);

	CComQIPtr<VCLibrarianTool> spLibTool = pTool;
	if (spLibTool)
		return DoUnknownStringProp(pPropContainer, pOptHandler, pFileItem, VCLIBID_AdditionalOptions);

	CComQIPtr<VCResourceCompilerTool> spRCTool = pTool;
	if (spRCTool)
		return DoUnknownStringProp(pPropContainer, pOptHandler, pFileItem, VCRCID_AdditionalOptions);

	CComQIPtr<VCBscMakeTool> spBscTool = pTool;
	if (spBscTool)
		return DoUnknownStringProp(pPropContainer, pOptHandler, pFileItem, VCBSCID_AdditionalOptions);

	return FALSE;
}

BOOL CVCProjConvert::DoUnknownStringProp(IVCPropertyContainer* pPropContainer, COptionHandler* pOptHandler, 
	CProjItem* pFileItem, long nOptionID)
{
	pOptHandler->SetSlob(pFileItem); 
	UINT nIDUnknownOption, nIDUnknownString;
	pOptHandler->GetSpecialOptProps(nIDUnknownOption, nIDUnknownString, OBInherit);
	if (nIDUnknownString != (UINT)-1)
	{
		CProject* pProj = pFileItem->GetProject();
		if (pProj != NULL)
		{
			DoUnknownStringPropHelper(pProj, nIDUnknownString, pPropContainer, nOptionID);
			DoUnknownStringPropHelper(pProj, nIDUnknownOption, pPropContainer, nOptionID);
		}
	}
	pOptHandler->ResetSlob();

	return TRUE;
}

void CVCProjConvert::DoUnknownStringPropHelper(CProjItem* pProjItem, UINT nIDUnknown, IVCPropertyContainer* pPropContainer,
	long nOptionID)
{
	CString strUnknown;
	GPT gptRet = pProjItem->GetConvertedStrProp(nIDUnknown, strUnknown);
	if (gptRet == valid && !strUnknown.IsEmpty())
	{
		CComBSTR bstrUnknown = strUnknown;
		AddAdditionalOptions(pPropContainer, nOptionID, bstrUnknown);
	}
}

typedef struct {
	LPOLESTR	wszToolShortName;
	LPCTSTR		szOldToolName;
} MPNEW2OLDTOOLNAME;

static MPNEW2OLDTOOLNAME srgmpNew2OldToolName[] = {
		L"VCCLCompilerTool", "cl.exe",
		L"VCResourceCompilerTool"	, "rc.exe",
		L"VCLinkerTool"	, "link.exe",
		L"VCMIDLTool"	, "midl.exe",
		L"VCBscMakeTool", "BSC",
		L"VCCustomBuildTool", "cmd.exe",
		L"VCLibrarianTool"	, "lib.exe",
		L"VCPreBuildEventTool", "Pre Build",
		L"VCPreLinkEventTool", "Before the Linker",
		L"VCPostBuildEventTool", "Post Build",
		L"VCLibrarianTool", "link.exe"
};

void CVCProjConvert::SetToolObject(VCConfiguration *pConfig, LPCTSTR szTool, IVCToolImpl **ppTool)
{
	// check to see if default case is ok
	if (pConfig)
	{
		CComPtr<IDispatch> pDispTools;
		CComQIPtr<IVCCollection> pTools;
		pConfig->get_Tools(&pDispTools);
		pTools = pDispTools;
		long lTools;
		pTools->get_Count(&lTools);
		while (lTools > 0)
		{
			CComPtr<IDispatch> pDispTool;
			pTools->Item(CComVariant(lTools), &pDispTool);
			CComQIPtr<IVCToolImpl> pTool = pDispTool;
			if (pTool && IsToolMatch(pTool, szTool, true))
			{
				pTool.CopyTo(ppTool);
				return;
			}
			lTools--;
		}
	}
}

void CVCProjConvert::SetToolObject(VCFileConfiguration *pFileConfig, LPCTSTR szTool, IVCToolImpl **ppTool)
{
	// check to see if default case is ok
	bool bFound = false;
	CComPtr<IDispatch> pDispTool;
	CComQIPtr<IVCToolImpl> pTool;
	*ppTool = NULL;
	if (pFileConfig)
	{
		pFileConfig->get_Tool(&pDispTool);
		pTool = pDispTool;
		if (IsToolMatch(pTool, szTool, false))
		{
			bFound = true;
			pFileConfig->put_Tool(pDispTool);
			pTool.CopyTo(ppTool);
		}
		else
		{
			CString strLinker = _T("link.exe");
			bFound = (strLinker == szTool);
			if (bFound)
				pTool.CopyTo(ppTool);
		}
	}
	if (!bFound) 
	{ // find tool in configs...
		CComQIPtr<IVCFileConfigurationImpl> pFileConfigImpl = pFileConfig;
		if (pFileConfigImpl)
		{
			CComPtr<IDispatch> pDispConfiguration;
			HRESULT hr = pFileConfigImpl->get_Configuration(&pDispConfiguration);
			CComQIPtr<VCConfiguration> pConfiguration = pDispConfiguration;
			if (pConfiguration)
			{
				CComPtr<IDispatch> pDispTools;
				hr = pConfiguration->get_Tools(&pDispTools);
				CComQIPtr<IVCCollection> pTools = pDispTools;
				if (pTools)
				{
					long lcTools;
					pTools->get_Count(&lcTools);
					while (lcTools > 0 && !bFound)
					{
						CComPtr<IDispatch> pDispTool;
						pTools->Item(CComVariant(lcTools), &pDispTool);
						CComQIPtr<IVCToolImpl> pTool = pDispTool;
						if (pTool)
						{
							if (IsToolMatch(pTool, szTool, false))
							{
								bFound = true;
								pFileConfig->put_Tool(pDispTool);
								pDispTool.Release();	// want to reuse
								pFileConfig->get_Tool(&pDispTool);
								CComQIPtr<IVCToolImpl> pToolImpl = pDispTool;
								pToolImpl.CopyTo(ppTool);
							}
						}
						lcTools--;
					}
				}

			}
			
		}
	}
	VSASSERT(bFound, "Failed to find correct tool for file!");
}

BOOL CVCProjConvert::IsToolMatch(IVCToolImpl *pTool, LPCTSTR szTool, bool bLinkerAllowed)
{
	if (!bLinkerAllowed)
	{
		CString strLinker = _T("link.exe");
		if (strLinker == szTool)
			return FALSE;
	}

	if (pTool)
	{
		CComBSTR bstrToolName;
		pTool->get_ToolShortName(&bstrToolName);
		for (int i = 0; i < sizeof(srgmpNew2OldToolName) / sizeof(MPNEW2OLDTOOLNAME); i++)
		{
			if (bstrToolName == srgmpNew2OldToolName[i].wszToolShortName &&  
				_tcscmp(srgmpNew2OldToolName[i].szOldToolName, szTool) == 0)
				return TRUE;
		}
	}
	return FALSE;
}

void CVCProjConvert::SetToolSetting(IVCToolImpl *pTool, IVCPropertyContainer* pPropContainer,
	CComVariant& varVal, COptStr * pOptStr, CProjItem *pProjItem, COptionHandler *pOptHandler, bool bIsBase)
{

	if (SetCLToolSetting(pTool, pPropContainer, varVal, pOptStr, pProjItem, pOptHandler, bIsBase))
		return;
	if (SetLNKToolSetting(pTool, pPropContainer, varVal, pOptStr, pProjItem, pOptHandler, bIsBase))
		return;
	if (SetLIBToolSetting(pTool, pPropContainer, varVal, pOptStr, pProjItem, pOptHandler))
		return;
	if (SetMIDLToolSetting(pTool, pPropContainer, varVal, pOptStr, pProjItem, pOptHandler, bIsBase))
		return;
	if (SetRCToolSetting(pTool, pPropContainer, varVal, pOptStr, pProjItem, pOptHandler, bIsBase))
		return;
	if (SetBSCToolSetting(pTool, pPropContainer, varVal, pOptStr, pProjItem, pOptHandler))
		return;
}

BOOL CVCProjConvert::SetCLToolSetting(IVCToolImpl *pTool, IVCPropertyContainer* pPropContainer,
	CComVariant& varVal, COptStr * pOptStr,  CProjItem *pProjItem, COptionHandler *pOptHandler, bool bIsBase)
{
	CComQIPtr<VCCLCompilerTool> pCompiler = pTool;
	if (pCompiler && pPropContainer)
	{
		switch (pOptStr->idOption)
		{
		case IDOPT_CPLRNOLOGO: // IDOPT_CPLRNOLOGO, "nologo%T1", OPTARGS1(P_CplrNoLogo), single,
			SetBoolProperty(pPropContainer, VCCLID_SuppressStartupBanner, varVal.lVal);
			break;
		case IDOPT_MSEXT: // IDOPT_MSEXT, "Z%{e|a}1", OPTARGS1(P_MSExtension), single, 
			SetBoolProperty(pPropContainer, VCCLID_DisableLanguageExtensions, varVal.lVal);	// apears to be stored as bool, not enum...
			break;
		case IDOPT_TURN_OFF_WARNINGS: // IDOPT_TURN_OFF_WARNINGS,	"w%<1>1", OPTARGS1(P_WarningLevel), single,
		case IDOPT_WARN: // IDOPT_WARN, "W%{0|1|2|3|4}1", OPTARGS1(P_WarningLevel), single,
			SetEnumProperty(pPropContainer, VCCLID_WarningLevel, varVal.lVal);
			break;
		case IDOPT_WARNASERROR: // IDOPT_WARNASERROR,	"WX%T1", OPTARGS1(P_WarnAsError), single,
			SetBoolProperty(pPropContainer, VCCLID_WarnAsError, varVal.lVal);
			break;
		case IDOPT_ENABLE_MR: // IDOPT_ENABLE_MR,	"Gm%{-|}1", OPTARGS1(P_Enable_MR), single,
			SetBoolProperty(pPropContainer, VCCLID_MinimalRebuild, EnumIsTrue(varVal.lVal));
			break;
		case IDOPT_ENABLE_INCR: // IDOPT_ENABLE_INCR,	"Gi%{-|}1", OPTARGS1(P_Enable_Incr), single,
			break;	// don't bother converting this one
		case IDOPT_PTRMBR1: // IDOPT_PTRMBR1, "vm%{b|g}1", OPTARGS1(P_PtrMbr1), single,
			SetPtrToMember1(pPropContainer, varVal.lVal);
			break;
		case IDOPT_PTRMBR2: // IDOPT_PTRMBR2, "vm%{v|s|m}1", OPTARGS1(P_PtrMbr2), single,
			SetPtrToMember2(pPropContainer, varVal.lVal);
			break;
		case IDOPT_VTORDISP: // IDOPT_VTORDISP, "vd%{1|0}1", OPTARGS1(P_VtorDisp), single,DisableConstructionDisplacements
			AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, varVal.lVal, L"/vd0");	// apears to be stored as bool, not enum...
			break;
		case IDOPT_ENABLE_RTTI: // IDOPT_ENABLE_RTTI,	"GR%{-|}1", OPTARGS1(IDOPT_ENABLE_RTTI), single,
			SetBoolProperty(pPropContainer, VCCLID_RuntimeTypeInfo, varVal.lVal);
			break;
		case IDOPT_DEBUGINFO: // IDOPT_DEBUGINFO,	"Z%{d|7|i|I}1", OPTARGS1(P_DebugInfo), single, 
			SetDebugInfo(pCompiler, varVal.lVal);
			break;
		case IDOPT_OPTIMIZE: // IDOPT_OPTIMIZE, "O%{d|2|1}1", OPTARGS1(P_Optimize), single,
			SetOptimization(pCompiler, varVal.lVal);
			break;
		case IDOPT_OPT_X: // IDOPT_OPT_X, "Ox%T1%<4>2", OPTARGS2(P_Optimize_X, P_Optimize), single,
			SetOptimizeX(pCompiler, varVal.lVal);
			break;
		case IDOPT_OPT_A: // IDOPT_OPT_A, "Oa%T1%<4>2", OPTARGS2(P_Optimize_A, P_Optimize), single, AliasingWithinFunctions
			AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, varVal.lVal, L"/Oa");
			break;
		case IDOPT_OPT_W: // IDOPT_OPT_W, "Ow%T1%<4>2", OPTARGS2(P_Optimize_W, P_Optimize), single, AliasingAcrossFunctions
			AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, varVal.lVal, L"/ow");
			break;
		case IDOPT_OPT_G: // IDOPT_OPT_G, "Og%T1%<4>2", OPTARGS2(P_Optimize_G, P_Optimize), single, GlobalOptimizations
			SetBoolProperty(pPropContainer, VCCLID_GlobalOptimizations, varVal.lVal);
			break;
		case IDOPT_OPT_I: // IDOPT_OPT_I, "Oi%T1%<4>2", OPTARGS2(P_Optimize_I, P_Optimize), single, EnableIntrinsicFunctions
			SetBoolProperty(pPropContainer, VCCLID_EnableIntrinsicFunctions, varVal.lVal);
			break;
		case IDOPT_OPT_T: // IDOPT_OPT_T, "Ot%T1%<4>2", OPTARGS2(P_Optimize_T, P_Optimize), single,
		case IDOPT_OPT_S: // IDOPT_OPT_S, "Os%T1%<4>2", OPTARGS2(P_Optimize_S, P_Optimize), single, FavorSizeOrSpeed // see favorSizeOrSpeedOption enum
			SetCodeGenFavor(pCompiler, pOptStr->idOption, varVal.lVal);
			break;
		case IDOPT_OPT_P: // IDOPT_OPT_P, "Op%{-|}1", OPTARGS1(P_Optimize_P), single, ImproveFloatingPointConsistency
			SetBoolProperty(pPropContainer, VCCLID_ImproveFloatingPointConsistency, varVal.lVal);
			break;
		case IDOPT_OPT_Y: // IDOPT_OPT_Y, "Oy%{-|}1", OPTARGS1(P_Optimize_Y), single, OmitFramePointers
			SetBoolProperty(pPropContainer, VCCLID_OmitFramePointers, varVal.lVal);
			break;
		case IDOPT_INLINECTRL: // IDOPT_INLINECTRL,	"Ob%{0|1|2}1", OPTARGS1(P_InlineControl), single, InlineFunctionExpansion
			SetEnumProperty(pPropContainer, VCCLID_InlineFunctionExpansion, varVal.lVal);
			break;
		case IDOPT_STRPOOL: // IDOPT_STRPOOL, "G%{f|F}1", OPTARGS1(P_StringPool), single, StringPooling 	// see stringPoolOption enum
			SetStringPooling(pCompiler, varVal.lVal);
			break; 
		case IDOPT_INCLINK: // IDOPT_INCLINK, "Gy%T1", OPTARGS1(P_IncLinking), single,
			SetBoolProperty(pPropContainer, VCCLID_EnableFunctionLevelLinking, varVal.lVal);
			break;
		case IDOPT_CPLRIGNINC: // IDOPT_CPLRIGNINC,	"X%T1", OPTARGS1(P_CplrIgnInCPath), single, IgnoreStandardIncludePath
			SetBoolProperty(pPropContainer, VCCLID_IgnoreStandardIncludePath, varVal.lVal);
			break;
		case IDOPT_CPLRINCLUDES: // IDOPT_CPLRINCLUDES,	"I[ ]%1", OPTARGS1(P_CplrIncludes), multiple,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMultiStrProperty(pPropContainer, VCCLID_AdditionalIncludeDirectories, varVal.bstrVal, bIsBase);
			}
			break;
		case IDOPT_FORCEINCLUDES: // IDOPT_FORCEINCLUDES,	"FI%1", OPTARGS1(P_ForceIncludes), multiple,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMultiStrProperty(pPropContainer, VCCLID_ForcedIncludeFiles, varVal.bstrVal, bIsBase);
			}
			break;
		case IDOPT_UNDEFINE: // IDOPT_UNDEFINE, "u%T1", OPTARGS1(P_Undefine), single,
			SetBoolProperty(pPropContainer, VCCLID_UndefineAllPreprocessorDefinitions, varVal.lVal);
			break;
		case IDOPT_MACROS: // IDOPT_MACROS, "D[ ]%1", OPTARGS1(P_MacroNames), multiple,
			{ // "WIN32,NDEBUG,_WINDOWS,_AFXDLL,_MBCS"
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMultiStrProperty(pPropContainer, VCCLID_PreprocessorDefinitions, varVal.bstrVal, bIsBase);
			}
			break;
		case IDOPT_UNDEFMACROS: // IDOPT_UNDEFMACROS,	"U[ ]%1", OPTARGS1(P_UndefMacros), multiple,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMultiStrProperty(pPropContainer, VCCLID_UndefinePreprocessorDefinitions, varVal.bstrVal, bIsBase);
			}
			break;
		case IDOPT_LISTASM: // IDOPT_LISTASM, "FA%{%f1%f2|cs%t1%t2|c%t1%f2|s%f1%t2}3",	OPTARGS3(P_AsmListHasMC, P_AsmListHasSrc, P_ListAsm), single,
			SetAsmOptions(pCompiler, pProjItem, pOptHandler, varVal.lVal);
			break;
		case IDOPT_OUTDIR_COD: // IDOPT_OUTDIR_COD,	"Fa%1", OPTARGS1(P_OutputDir_Cod), single, AssemblerListingLocation
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCCLID_AssemblerListingLocation, varVal.bstrVal);
			}
			break;
		case IDOPT_LISTBSCNOLCL: // IDOPT_LISTBSCNOLCL,	"Fr%T1%t2[%3]", OPTARGS3(P_GenBrowserInfo, P_GenBrowserInfoNoLcl, P_OutputDir_Sbr),	single,
		case IDOPT_LISTBSC: // IDOPT_LISTBSC, "FR%T1%f2[%3]", OPTARGS3(P_GenBrowserInfo, P_GenBrowserInfoNoLcl, P_OutputDir_Sbr),	single,
			SetListBsc(pCompiler, pPropContainer, pProjItem, pOptHandler, pOptStr->idOption, varVal.lVal);
			break;
		case IDOPT_PCHNAME: // IDOPT_PCHNAME, "Fp%1", OPTARGS1(P_OutputDir_Pch), single, PrecompiledHeaderFile
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCCLID_PrecompiledHeaderFile, varVal.bstrVal);
			}
			break;
		case IDOPT_AUTOPCH: // IDOPT_AUTOPCH, "YX%T1[%2]", OPTARGS2(P_AutoPch, P_AutoPchUpTo), single,
			SetAutoPCH(pCompiler, pPropContainer, pOptHandler, pProjItem, varVal.lVal);
			break;
		case IDOPT_PCHCREATE: // IDOPT_PCHCREATE,	"Yc%T1[%2]", OPTARGS2(P_PchCreate, P_PchCreateUptoHeader),	single,
			SetCreatePCH(pCompiler, pPropContainer, pOptHandler, pProjItem, varVal.lVal);
			break;
		case IDOPT_PCHUSE: // IDOPT_PCHUSE, "Yu%T1[%2]", OPTARGS2(P_PchUse, P_PchUseUptoHeader), single,
			SetUsePCH(pCompiler, pPropContainer, pOptHandler, pProjItem, varVal.lVal);
			break;
		case IDOPT_OUTDIR_OBJ: // IDOPT_OUTDIR_OBJ,	"Fo%1", OPTARGS1(P_OutputDir_Obj), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCCLID_ObjectFile, varVal.bstrVal);
			}
			break;
		case IDOPT_OUTDIR_PDB: // IDOPT_OUTDIR_PDB,	"Fd%1", OPTARGS1(P_OutputDir_Pdb), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCCLID_ProgramDataBaseFileName, varVal.bstrVal);
			}
			break;
		case IDOPT_STACK_PROBE: // IDOPT_STACK_PROBE,	"Ge%T1", OPTARGS1(P_StackProbe), single, ForceStackCheck
			AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, varVal.lVal, L"/Ge");
			break;
		case IDOPT_STACK_PROBE_THOLD: // IDOPT_STACK_PROBE_THOLD,	"Gs%1", OPTARGS1(P_StackProbeTHold), single,
			SetStackCheck(pPropContainer, varVal.lVal);
			break;
		case IDOPT_MAXLEN_EXTNAME: // IDOPT_MAXLEN_EXTNAME, "H%1", OPTARGS1(P_MaxLenExtNames), single,
			break;	// obsolete
		case IDOPT_USECHAR_AS_UNSIGNED: // IDOPT_USECHAR_AS_UNSIGNED,	"J%T1", OPTARGS1(P_UseChasAsUnsigned), single,
			SetBoolProperty(pPropContainer, VCCLID_DefaultCharIsUnsigned, varVal.lVal);
			break;
		case IDOPT_EMBED_STRING: // IDOPT_EMBED_STRING, "V%1", OPTARGS1(P_EmbedString), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				CComBSTR bstrVal = varVal.bstrVal;
				if (bstrVal.Length())
				{
					CComBSTR bstrVal2 = L"/V ";
					bstrVal2 += varVal.bstrVal;
					AddAdditionalOptions(pPropContainer, VCCLID_AdditionalOptions, bstrVal2);
				}
			}
			break;
		case IDOPT_NO_DEFLIB_NAME: // IDOPT_NO_DEFLIB_NAME, "Zl%T1", OPTARGS1(P_NoDefLibNameInObj), single,
			AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, varVal.lVal, L"/Z1");
			break;
		case IDOPT_PREP_COMMENTS: // IDOPT_PREP_COMMENTS, "C%T1", OPTARGS1(P_PrepPreserveComments), single,
			SetBoolProperty(pPropContainer, VCCLID_KeepComments, varVal.lVal);
			break;
		case IDOPT_PREPROCESS_WO_LINES: // IDOPT_PREPROCESS_WO_LINES,	"EP%T1", OPTARGS1(P_PreprocessWOLines), single,
		case IDOPT_PREPROCESS: // IDOPT_PREPROCESS, "E%T1", OPTARGS1(P_Preprocess), single,
			SetPreprocess1(pCompiler, pOptStr->idOption, varVal.lVal);
			break;
		case IDOPT_PREPROCESS_TO_FILE: // IDOPT_PREPROCESS_TO_FILE,	"P%T1", OPTARGS1(P_PrepprocessToFile), single,
			SetPreprocess2(pCompiler, pProjItem, varVal.lVal);
			break;
		case IDOPT_ENABLE_FD: // IDOPT_ENABLE_FD,	"FD%{-|}1", OPTARGS1(P_Enable_FD), single,
			break;	// user isn't allowed to unset this...
		case IDOPT_ENABLE_FC: // IDOPT_ENABLE_FC, "FC%T1", OPTARGS1(P_Enable_FC), single,
			// Don't want /FC to be in the property set, but carry it forward if the user had it
			AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, varVal.lVal, L"/FC");
			break;
		case IDOPT_COM_EmitMergedSource: // IDOPT_COM_EmitMergedSource,	"Fx%T1", OPTARGS1(P_COM_EmitMergedSource), single,
			SetBoolProperty(pPropContainer, VCCLID_ExpandAttributedSource, varVal.lVal);
			break;

		case IDOPT_STACKSIZE: // IDOPT_STACKSIZE, "F%1", OPTARGS1(P_StackSize), single,
			break;
		case IDOPT_ENABLE_DLP: // IDOPT_ENABLE_DLP, "dlp%{-|}1",	OPTARGS1(P_Enable_dlp), single,
			// Don't want this to be in the property set, but carry it forward if the user had it
			AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, varVal.lVal, L"/dlp");
			break;
		case IDOPT_GEN_FUNC_PROTO: // IDOPT_GEN_FUNC_PROTO, "Zg%T1", OPTARGS1(P_GenFuncProto), single,
			AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, varVal.lVal, L"/Zg");
			break;
		case IDOPT_CHECK_SYNTAX: // IDOPT_CHECK_SYNTAX, "Zs%T1", OPTARGS1(P_CheckSyntax), single,
			AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, varVal.lVal, L"/Zs");
			break;
		case IDOPT_UNKNOWN_OPTION: //  IDOPT_UNKNOWN_OPTION,	"", NO_OPTARGS, single, PostMiscellaneous
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				AddAdditionalOptions(pPropContainer, VCCLID_AdditionalOptions, varVal.bstrVal);
			}
			break;
		case IDOPT_COMPILE_ONLY: // IDOPT_COMPILE_ONLY,	"c%T1", OPTARGS1(P_CompileOnly), single,
			break;	// don't care WHAT the user sets this to, we're throwing it anyway...
		case IDOPT_TREATFILEAS_CPP1: // IDOPT_TREATFILEAS_CPP1,	"Tp%T1", OPTARGS1(P_TreatFileAsCPP1), single,	
		case IDOPT_TREATFILEAS_CPP2: // IDOPT_TREATFILEAS_CPP2,	"TP%T1", OPTARGS1(P_TreatFileAsCPP2), single,	
		case IDOPT_TREATFILEAS_C1: // IDOPT_TREATFILEAS_C1,	"Tc%T1", OPTARGS1(P_TreatFileAsC1), single,
		case IDOPT_TREATFILEAS_C2: // IDOPT_TREATFILEAS_C2,	"TC%T1", OPTARGS1(P_TreatFileAsC2), single,
			SetTreatFileAs(pCompiler, pOptStr->idOption, varVal.lVal);
			break;
		case IDOPT_COM_HresultKnowledge: // IDOPT_COM_HresultKnowledge,	"noHRESULT%{|-}1",	OPTARGS1(P_COM_HresultKnowledge),	single,
			// SetBoolProperty(pPropContainer, VCCLID_SuppressIntrinsicHRESULTSupport, EnumIsTrue(varVal.lVal), TRUE /* flip it*/);	 -- no longer supported
			break;
 		case IDOPT_CPUX86: // IDOPT_CPUX86, "G%{3|4|5|6|B}1", OPTARGS1(P_Cpux86), single, // OptimizeForProcessor see ProcessorOptimizeOption enum
			SetProcOptimize(pCompiler, varVal.lVal);
			break;
		case IDOPT_CALLCONV_ALPHA: // IDOPT_CALLCONV_ALPHA,	"G%{d}1", OPTARGS1(P_CallConvAlpha), single,
		case IDOPT_CALLCONV_MERCED: // IDOPT_CALLCONV_MERCED,	"G%{d}1", OPTARGS1(P_CallConvMerced), single,
		case IDOPT_CALLCONV: // IDOPT_CALLCONV, "G%{d|r|z}1", OPTARGS1(P_CallConv), single, CallingConvention // see callingConventionOption enum
			SetCallConvention(pCompiler, varVal.lVal);
			break;
		case IDOPT_GZ: // IDOPT_GZ, "GZ%T1", OPTARGS1(P_GZ), single, Enable runtime checks (/RTC1)
			SetRuntimeChecks(pCompiler, varVal.lVal, runtimeBasicCheckAll);
			break;
		case IDOPT_BYTE_MERCED: // IDOPT_BYTE_MERCED,	"Zp%{2|4|8|16|[1]}1",	OPTARGS1(P_ByteAlignMerced), single,
		case IDOPT_BYTE_ALPHA: // IDOPT_BYTE_ALPHA,	"Zp%{2|4|8|16|[1]}1",	OPTARGS1(P_ByteAlignAlpha), single,
		case IDOPT_BYTE: // IDOPT_BYTE, "Zp%{2|4|8|16|[1]}1", OPTARGS1(P_ByteAlign), single, StructMemberAlignment see structMemberAlignOption enum
			SetByteAlign(pCompiler, varVal.lVal);
			break;
		case IDOPT_THREAD_MERCED: // IDOPT_THREAD_MERCED,	"M%{L|T|D}1", OPTARGS1(P_ThreadMerced), single,
		case IDOPT_THREAD_ALPHA: // IDOPT_THREAD_ALPHA,	"M%{L|T|D}1", OPTARGS1(P_ThreadAlpha), single,
			if (varVal.lVal)
				varVal.lVal += 3;
			// fall through
		case IDOPT_THREAD: // IDOPT_THREAD, "M%{Ld|Td|Dd|L|T|D}1",	OPTARGS1(P_Thread), single, RuntimeLibrary runtimeLibraryOption
			SetRuntimeLib(pCompiler, varVal.lVal);
			break;

		case IDOPT_FLOATPOINT: // IDOPT_FLOATPOINT,	"FP%{i87|i|a|c87|c}1", OPTARGS1(P_FloatPoint), single,
			break;	// nuke obsolete compiler option now...

// run-time checks
		case IDOPT_ENABLE_EH: // IDOPT_ENABLE_EH,	"GX%{-|}1", OPTARGS1(P_Enable_EH), single,  enables synchronous exception 
			SetOldEH(pCompiler, varVal.lVal);
			break;
		case IDOPT_CPP_RTC: // IDOPT_CPP_RTC, "RTC%{1|2}1", OPTARGS1(P_RTC), single,	
			SetRuntimeChecks(pCompiler, varVal.lVal, runtimeBasicCheckAll);
			break;
		case IDOPT_CPP_RTC_StackFrame: // IDOPT_CPP_RTC_StackFrame,	"RTCs%T1%<3>2", OPTARGS2(P_RTC_StackFrame, P_RTC),	single, 
			SetRuntimeChecks(pCompiler, varVal.lVal, runtimeCheckStackFrame);
			break;
		case IDOPT_CPP_RTC_MemoryAccess: // IDOPT_CPP_RTC_MemoryAccess,	"RTCm%T1%<3>2", OPTARGS2(P_RTC_MemoryAccess, P_RTC),	single,
			break;	// option does not exist in VC7 compiler
		case IDOPT_CPP_RTC_MemoryLeaks: // IDOPT_CPP_RTC_MemoryLeaks,	"RTCl%T1%<3>2", OPTARGS2(P_RTC_MemoryLeaks, P_RTC),	single,
			break;	// option does not exist in VC7 compiler
		case IDOPT_CPP_RTC_SmallerType: // IDOPT_CPP_RTC_SmallerType,	"RTCc%T1%<3>2", OPTARGS2(P_RTC_SmallerType, P_RTC),	single,	
			SetBoolProperty(pPropContainer, VCCLID_SmallerTypeCheck, varVal.lVal);
			break;
		case IDOPT_CPP_RTC_VectorNew: // IDOPT_CPP_RTC_VectorNew,	"RTCv%T1%<3>2", OPTARGS2(P_RTC_VectorNew, P_RTC),	single,	
			break;	// option does not exist in VC7 compiler
		case IDOPT_GTVALUE_MERCED: // IDOPT_GTVALUE_MERCED, "Gt%1", OPTARGS1(P_GtvalueMerced), single,
		case IDOPT_GTVALUE_ALPHA: // IDOPT_GTVALUE_ALPHA, "Gt%1", OPTARGS1(P_GtvalueAlpha), single,
			break;	// option does not exist in VC7 compiler (Intel only)
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CVCProjConvert::SetLNKToolSetting(IVCToolImpl *pTool, IVCPropertyContainer* pPropContainer,
	CComVariant& varVal, COptStr * pOptStr, CProjItem *pProjItem, COptionHandler *pOptHandler, bool bIsBase)
{
	CComQIPtr<VCLinkerTool> pLinker = pTool;
	if (pLinker)
	{
		if (pProjItem->IsKindOf(RUNTIME_CLASS(CFileItem)))	// no switches to translate for files associated with the linker...
			return TRUE;

		switch (pOptStr->idOption)
		{
		case IDOPT_UNKNOWN_STRING: //  IDOPT_UNKNOWN_STRING,	"", NO_OPTARGS, single,
			{
				CStringW strUnknownW = varVal.bstrVal, strSeparator = L" ";
				RemoveMultiples(strUnknownW, strSeparator);
				CComBSTR bstrUnknown = strUnknownW;
				SetStrProperty(pPropContainer, VCLINKID_AdditionalDependencies, bstrUnknown);
			}
			break;
		case IDOPT_LINKNOLOGO: //  IDOPT_LINKNOLOGO,	"nologo%T1", OPTARGS1(P_LinkNoLogo), single,
			SetBoolProperty(pPropContainer, VCLINKID_SuppressStartupBanner, varVal.lVal);
			break;
		case IDOPT_DERIVED_OPTHDLR: //  IDOPT_DERIVED_OPTHDLR,	"", NO_OPTARGS, single,
			break;
		case IDOPT_LINKVERBOSE: //  IDOPT_LINKVERBOSE,	"verbose%T1", OPTARGS1(P_LinkVerbose), single, // ShowProgress  see linkProgressOption enum
			SetLinkShowProgress(pLinker, varVal.lVal);
			break;
		case IDOPT_DLLGEN: //  IDOPT_DLLGEN, "dll%T1", OPTARGS1(P_GenDll), single,
			SetBoolProperty(pPropContainer, VCLINKID_LinkDLL, varVal.lVal);
			break;
		case IDOPT_PROFILE: //  IDOPT_PROFILE, "profile%T1", OPTARGS1(P_Profile), single,
			break;	// option has been deprecated
		case IDOPT_USEPDBNONE: //  IDOPT_USEPDBNONE,	"pdb: // none%F1", OPTARGS1(P_UsePDB), single,
			break;	// PDB:NONE no longer supported
		case IDOPT_INCREMENTALLINK: //  IDOPT_INCREMENTALLINK, "incremental: // %{no|yes}1",	OPTARGS1(P_IncrementalLink), single,
			SetLinkIncremental(pLinker, EnumIsTrue(varVal.lVal));
			break;
		case IDOPT_USEPDB: //  IDOPT_USEPDB, "pdb: // %T1%2", OPTARGS2(P_UsePDB, P_UsePDBName), single,
			SetUsePDB(pPropContainer, pProjItem, pOptHandler, varVal.lVal);
			break;
		case IDOPT_MAPGEN: //  IDOPT_MAPGEN, "m[ap]%T1[: // %2]", OPTARGS2(P_GenMap, P_MapName), single,
			SetMapGen(pPropContainer, pProjItem, pOptHandler, varVal.lVal);
			break;
		case IDOPT_DEBUG: //  IDOPT_DEBUG, "debug%T1", OPTARGS1(P_GenDebug), single,
			SetBoolProperty(pPropContainer, VCLINKID_GenerateDebugInformation, varVal.lVal);
			break;
		case IDOPT_DEBUGTYPE: //  IDOPT_DEBUGTYPE,	"debugtype: // %{cv|coff|both}1",	OPTARGS1(P_DebugType), single,
			break;	// obsolete property

		case IDOPT_SUBSYSTEM: // IDOPT_SUBSYSTEM,	"subsystem:%{windows|console}1",OPTARGS1(P_SubSystem), single, SubSystem see subSystemOption enum
			SetLinkerSubSystem(pPropContainer, varVal.lVal);
			break;
		case IDOPT_MACHINETYPE: //  IDOPT_MACHINETYPE, "machine: // %{I386|IX86|M68K|ALPHA|MPPC}1",	OPTARGS1(P_MachineType),	single,
			AddExtraOption(pPropContainer, VCLINKID_AdditionalOptions, varVal.lVal == 1 ? 2 : 1, L"/MACHINE:I386");
			break;
		case IDOPT_LARGEADDRESS:	// IDOPT_LARGEADDRESS,	"largeaddressaware%{:no|}1", OPTARGS(P_LargeAddress), single,
			SetLinkerLargeAddressAwareness(pPropContainer, varVal.lVal);
			break;
		case IDOPT_NODEFAULTLIB: //  IDOPT_NODEFAULTLIB,	"nod[efaultlib]: // %1", OPTARGS1(P_NoDefaultLibs), multiple,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMultiStrProperty(pPropContainer, VCLINKID_IgnoreDefaultLibraryNames, varVal.bstrVal, bIsBase);
			}
			break;
		case IDOPT_EXALLLIB: //  IDOPT_EXALLLIB, "nod[efaultlib]%T1", OPTARGS1(P_ExAllLibs), single,
			SetBoolProperty(pPropContainer, VCLINKID_IgnoreAllDefaultLibraries, varVal.lVal);
			break;
		case IDOPT_INCLUDESYM: //  IDOPT_INCLUDESYM,	"include: // %1", OPTARGS1(P_IncludeSym), multiple,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMultiStrProperty(pPropContainer, VCLINKID_ForceSymbolReferences, varVal.bstrVal, bIsBase);
			}
			break;
		case IDOPT_DEFNAME: //  IDOPT_DEFNAME, "def: // %1", OPTARGS1(P_DefName), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCLINKID_ModuleDefinitionFile, varVal.bstrVal);
			}
			break;
		case IDOPT_FORCE: //  IDOPT_FORCE, "force%T1", OPTARGS1(P_Force), single,
			AddExtraOption(pPropContainer, VCLINKID_AdditionalOptions, varVal.lVal, L"/FORCE");
			break;
		case IDOPT_OUT: //  IDOPT_OUT, "out: // %1", OPTARGS1(P_OutName), single,	
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCLINKID_OutputFile, varVal.bstrVal);
			}
			break;
		case IDOPT_IMPLIB: //  IDOPT_IMPLIB, "implib: // %1", OPTARGS1(P_ImpLibName), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCLINKID_ImportLibrary, varVal.bstrVal);
			}
			break;
		case IDOPT_LAZYPDB: //  IDOPT_LAZYPDB, "pdbtype: // %{sept|con}1", OPTARGS1(P_LazyPdb), single,  UseSeparateTypes
			break;	// this switch is going away in VC7
		case IDOPT_LIBPATH: //  IDOPT_LIBPATH, "libpath: // %1", OPTARGS1(P_LibPath), multiple,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMultiStrProperty(pPropContainer, VCLINKID_AdditionalLibraryDirectories, varVal.bstrVal, bIsBase);
			}
			break;
		case IDOPT_DELAY_NOBIND: //  IDOPT_DELAY_NOBIND,	"delay: // nobind%T1", OPTARGS1(P_LinkDelayNoBind), single,	
			AddExtraOption(pPropContainer, VCLINKID_AdditionalOptions, varVal.lVal, L"/DELAY:NOBIND");
			break;
		case IDOPT_DELAY_UNLOAD: //  IDOPT_DELAY_UNLOAD,	"delay: // unload%T1", OPTARGS1(P_LinkDelayUnLoad), single,	
			SetBoolProperty(pPropContainer, VCLINKID_SupportUnloadOfDelayLoadedDLL, varVal.lVal);
			break;
		case IDOPT_RESONLY_DLL:	//	IDOPT_RESONLY_DLL,	"noentry%T1", OPTARGS1(P_ResOnlyDLL), single,
			SetBoolProperty(pPropContainer, VCLINKID_ResourceOnlyDLL, varVal.lVal);
			break;
		case IDOPT_DELAY_LOAD_DLL: //  IDOPT_DELAY_LOAD_DLL,"delayload: // %1", OPTARGS1(P_LinkDelayLoadDLL), multiple,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMultiStrProperty(pPropContainer, VCLINKID_DelayLoadDLLs, varVal.bstrVal, bIsBase);
			}
			break; 
		case IDOPT_VIRTUAL_DEVICE_DRIVER:	//	IDOPT_VIRTUAL_DEVICE_DRIVER, "vxd%T1", OPTARGS1(P_VirtualDeviceDriver), single,
			AddExtraOption(pPropContainer, VCLINKID_AdditionalOptions, varVal.lVal, L"/VXD");
			break;
		case IDOPT_EXE_DYNAMIC:	// IDOPT_EXE_DYNAMIC, "exetype:dynamic%T1",	OPTARGS1(P_ExeDynamic),	single,
			AddExtraOption(pPropContainer, VCLINKID_AdditionalOptions, varVal.lVal, L"/EXETYPE:DYNAMIC");
			break;
		case IDOPT_DRIVER:	//	IDOPT_DRIVER, "driver%{:junk||:uponly|:wdm}%1", OPTARGS1(P_Driver), single,
			SetLinkerDriver(pPropContainer, varVal.lVal);
			break;
		case IDOPT_50COMPAT:	// 	IDOPT_50COMPAT, "link50compat%T1", OPTARGS(P_Link50Compat),	single,
			break;	// don't bother to convert
		case IDOPT_ALIGN:	// IDOPT_ALIGN,	"align:%1",	OPTARGS(P_Align), single,
			SetLinkerAlign(pPropContainer, varVal.lVal);
			break;
		case IDOPT_SET_CHECKSUM:	// IDOPT_SET_CHECKSUM, "release%T1", OPTARGS(P_SetCheckSum), single,
			SetBoolProperty(pPropContainer, VCLINKID_SetChecksum, varVal.lVal);
			break;
		case IDOPT_MERGE_SECTIONS:	//	IDOPT_MERGE_SECTIONS, "merge:%1", OPTARGS(P_MergeSections), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCLINKID_MergeSections, varVal.bstrVal);
			}
			break;
		case IDOPT_ORDER_FUNCS:	//	IDOPT_ORDER_FUNCS, "order:%1", OPTARGS(P_OrderFuncs), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCLINKID_FunctionOrder, varVal.bstrVal);
			}
			break;
		case IDOPT_TRIM_PROC_MEM:	// IDOPT_TRIM_PROC_MEM, "ws:aggressive%T1", OPTARGS(P_TrimProcMem), single,
			AddExtraOption(pPropContainer, VCLINKID_AdditionalOptions, varVal.lVal, L"/WS:AGGRESSIVE");
			break;
		case IDOPT_FIXED:	// IDOPT_FIXED,	"fixed%{:no|}1", OPTARGS1(P_Fixed), single,
			SetLinkerFixed(pPropContainer, varVal.lVal);
			break;
		case IDOPT_UNKNOWN_OPTION: //  IDOPT_UNKNOWN_OPTION,	"", NO_OPTARGS, single, PostMiscellaneous
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				AddAdditionalOptions(pPropContainer, VCLINKID_AdditionalOptions, varVal.bstrVal);
			}
			break;
		case IDOPT_EXE_BASE: // IDOPT_EXE_BASE, "base:%1", OPTARGS1(P_BaseAddr), single, BaseAddress
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCLINKID_BaseAddress, varVal.bstrVal);
			}
			break; 
		case IDOPT_PREVENT_BIND:	// IDOPT_PREVENT_BIND, "allowbind%{|:no}1", OPTARGS1(P_PreventBind), single,
			AddExtraOption(pPropContainer, VCLINKID_AdditionalOptions, varVal.lVal, L"/ALLOWBIND:NO");
			break;
		case IDOPT_VERSION: // IDOPT_VERSION, "version:%1[.%2]", OPTARGS2(P_VersionMaj, P_VersionMin),	single, MajorVersion.MinorVersion
			SetLinkerVersion(pPropContainer, pProjItem, pOptHandler, varVal.lVal);
			break;
		case IDOPT_STACK: // IDOPT_STACK, "st[ack]:%1[,%2]", OPTARGS2(P_StackReserve, P_StackCommit),single, StackReserveSize
			{
				varVal.ChangeType(VT_I4);
				SetIntProperty(pPropContainer, VCLINKID_StackReserveSize, varVal.lVal);
				int i;
				if (pProjItem->GetIntProp(pOptHandler->MapLogical(P_StackCommit), i) == valid)
					SetIntProperty(pPropContainer, VCLINKID_StackCommitSize, i);
			}
			break;
		case IDOPT_HEAP: // IDOPT_HEAP, "st[ack]:%1[,%2]", OPTARGS2(P_HeapReserve, P_HeapCommit),single, HeapReserveSize
			{
				varVal.ChangeType(VT_I4);
				SetIntProperty(pPropContainer, VCLINKID_HeapReserveSize, varVal.lVal);
				int i;
				if (pProjItem->GetIntProp(pOptHandler->MapLogical(P_HeapCommit), i) == valid)
					SetIntProperty(pPropContainer, VCLINKID_HeapCommitSize, i);
			}
			break;
		case IDOPT_ENTRYPOINT: // IDOPT_ENTRYPOINT,	"entry:%1", OPTARGS1(P_EntryName), single, EntryPointSymbol
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCLINKID_EntryPointSymbol, varVal.bstrVal);
			}
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CVCProjConvert::SetLIBToolSetting(IVCToolImpl *pTool, IVCPropertyContainer* pPropContainer,
	CComVariant& varVal, COptStr* pOptStr,  CProjItem* pProjItem, COptionHandler* pOptHandler)
{
	CComQIPtr<VCLibrarianTool> pLibrarian = pTool;
	if (pLibrarian)
	{
		switch (pOptStr->idOption)
		{
		case IDOPT_UNKNOWN_STRING: // IDOPT_UNKNOWN_STRING,	"", NO_OPTARGS, single,
			AddAdditionalOptions(pPropContainer, VCLIBID_AdditionalOptions, varVal.bstrVal);
			break;
		case IDOPT_LIBNOLOGO: // IDOPT_LIBNOLOGO, "nologo%T1", OPTARGS1(P_NoLogoLib), single,
			SetBoolProperty(pPropContainer, VCLIBID_SuppressStartupBanner, varVal.lVal);
			break;
		case IDOPT_DEFLIB: // IDOPT_DEFLIB, "def:%1", OPTARGS1(P_DefNameLib), single, ModuleDefinitionFile
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCLIBID_ModuleDefinitionFile, varVal.bstrVal);
			}
			break;
		case IDOPT_OUTLIB: // IDOPT_OUTLIB, "out:%1", OPTARGS1(P_OutNameLib), single,	
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCLIBID_OutputFile, varVal.bstrVal);
			}
			break;
		case IDOPT_UNKNOWN_OPTION: //  IDOPT_UNKNOWN_OPTION,	"", NO_OPTARGS, single, PostMiscellaneous
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				AddAdditionalOptions(pPropContainer, VCLIBID_AdditionalOptions, varVal.bstrVal);
			}
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CVCProjConvert::SetMIDLToolSetting(IVCToolImpl *pTool, IVCPropertyContainer* pPropContainer,
	CComVariant& varVal, COptStr* pOptStr, CProjItem* pProjItem, COptionHandler* pOptHandler, bool bIsBase)
{
	CComQIPtr<VCMidlTool> pMidl = pTool;
	if (pMidl)
	{
		switch (pOptStr->idOption)
		{
		case IDOPT_MTLNOLOGO: // IDOPT_MTLNOLOGO, "nologo%T1", OPTARGS1(P_MTLNologo), single,
			SetBoolProperty(pPropContainer, VCMIDLID_SuppressStartupBanner, varVal.lVal);
			break;
		case IDOPT_MTLINCLUDES: // IDOPT_MTLINCLUDES, "I[ ]%1", OPTARGS1(P_MTLIncludes), multiple,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMultiStrProperty(pPropContainer, VCMIDLID_AdditionalIncludeDirectories, varVal.bstrVal, bIsBase);
			}
			break;
		case IDOPT_MTLDLLDATA: // IDOPT_MTLDLLDATA, "dlldata[ ]%1", OPTARGS1(P_MTLDllData), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCMIDLID_DLLDataFileName, varVal.bstrVal);
			}
			break;
		case IDOPT_MTLMACROS: // IDOPT_MTLMACROS, "D[ ]%1", OPTARGS1(P_MTLMacros), multiple,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMultiStrProperty(pPropContainer, VCMIDLID_PreprocessorDefinitions, varVal.bstrVal, bIsBase);
			}
			break;
		case IDOPT_MTLOUTPUTTLB: // IDOPT_MTLOUTPUTTLB, "tlb[ ]%1", OPTARGS1(P_MTLOutputTlb), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCMIDLID_TypeLibraryName, varVal.bstrVal, (bIsBase == false));
			}
			break;
		case IDOPT_MTLOUTPUTINC: // IDOPT_MTLOUTPUTINC, "h[ ]%1", OPTARGS1(P_MTLOutputInc), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCMIDLID_HeaderFileName, varVal.bstrVal);
			}
			break;
		case IDOPT_MTLOUTPUTUUID: // IDOPT_MTLOUTPUTUUID,	"iid[ ]%1", OPTARGS1(P_MTLOutputUuid), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCMIDLID_InterfaceIdentifierFileName, varVal.bstrVal);
			}
			break;
		case IDOPT_MTLOUTPUTDIR: // IDOPT_MTLOUTPUTDIR, "out[ ]%1", OPTARGS1(P_MTLOutputDir), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMidlOutput(pPropContainer, VCMIDLID_OutputDirectory, varVal.bstrVal);
			}
			break;
		case IDOPT_MTLMTLCOMPATIBLE: // IDOPT_MTLMTLCOMPATIBLE,	"mktyplib203%T1",	OPTARGS1(P_MTLMtlCompatible), single,
			SetBoolProperty(pPropContainer, VCMIDLID_MkTypLibCompatible, varVal.lVal);
			break;
		case IDOPT_MTL_STUBLESS_PROXY: // IDOPT_MTL_STUBLESS_PROXY,	"Oicf%T1", OPTARGS1(P_MTLStublessProxy),	single,
			SetMidlStublessProxy(pPropContainer, varVal.lVal);
			break;
		case IDOPT_MTLNOCLUTTER: // IDOPT_MTLNOCLUTTER, "o[ ]%1", OPTARGS1(P_MTLNoClutter), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMidlOutput(pPropContainer, VCMIDLID_RedirectOutputAndErrors, varVal.bstrVal);
			}
			break;
		case IDOPT_UNKNOWN_OPTION: //  IDOPT_UNKNOWN_OPTION,	"", NO_OPTARGS, single, PostMiscellaneous
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				AddAdditionalOptions(pPropContainer, VCMIDLID_AdditionalOptions, varVal.bstrVal);
			}
			break;
		case IDOPT_MTL_CHAR: // IDOPT_MTL_CHAR, "char %{signed|ascii7|unsigned}1",	OPTARGS1(P_MTL_Char),	single, DefaultCharType	// see midlCharOption enum
			SetMidlChar(pPropContainer, varVal.lVal);
			break;
		case IDOPT_MTL_CLIENT: // IDOPT_MTL_CLIENT, "client %{stub|none}1",	OPTARGS1(P_MTL_Client), single, Client
			SetMidlClient(pPropContainer, varVal.lVal);
			break;
		case IDOPT_MTL_SERVER: // IDOPT_MTL_SERVER, "server %{stub|none}1",	OPTARGS1(P_MTL_Server), single, Server
			SetMidlServer(pPropContainer, varVal.lVal);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CVCProjConvert::SetRCToolSetting(IVCToolImpl *pTool, IVCPropertyContainer* pPropContainer,
	CComVariant& varVal, COptStr* pOptStr, CProjItem* pProjItem, COptionHandler* pOptHandler, bool bIsBase)
{
	CComQIPtr<VCResourceCompilerTool> pResourceCompiler = pTool;
	if (pResourceCompiler)
	{
		switch (pOptStr->idOption)
		{
		case IDOPT_RESLANGID: // IDOPT_RESLANGID, "l[ ]%1",	OPTARGS1(P_ResLangID), single,
			SetIntProperty(pPropContainer, VCRCID_Culture, varVal.lVal);
			break;
		case IDOPT_RESVERBOSE: // IDOPT_RESVERBOSE, "v%T1", OPTARGS1(P_ResVerbose), single,
			SetBoolProperty(pPropContainer, VCRCID_ShowProgress, varVal.lVal);
			break;
		case IDOPT_RESIGNINC: // IDOPT_RESIGNINC, "x%T1", OPTARGS1(P_ResIgnInCPath), single,
			SetBoolProperty(pPropContainer, VCRCID_IgnoreStandardIncludePath, varVal.lVal);
			break;
		case IDOPT_OUTDIR_RES: // IDOPT_OUTDIR_RES, "fo%1", OPTARGS1(P_OutNameRes), single, ResFileName
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCRCID_ResourceOutputFileName, varVal.bstrVal);
			}
			break;
		case IDOPT_RESINCLUDES: // IDOPT_RESINCLUDES, "i[ ]%1",	OPTARGS1(P_ResIncludes),	multiple,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMultiStrProperty(pPropContainer, VCRCID_AdditionalIncludeDirectories, varVal.bstrVal, bIsBase);
			}
			break;
		case IDOPT_RESMACROS: // IDOPT_RESMACROS, "d[ ]%1",	OPTARGS1(P_ResMacroNames),	multiple,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetMultiStrProperty(pPropContainer, VCRCID_PreprocessorDefinitions, varVal.bstrVal, bIsBase);
			}
			break;
		case IDOPT_UNKNOWN_OPTION: //  IDOPT_UNKNOWN_OPTION,	"", NO_OPTARGS, single, PostMiscellaneous
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				AddAdditionalOptions(pPropContainer, VCRCID_AdditionalOptions, varVal.bstrVal);
			}
			break;
		case IDOPT_UNKNOWN_STRING: // IDOPT_UNKNOWN_STRING,	"", NO_OPTARGS, single,
			AddAdditionalOptions(pPropContainer, VCRCID_AdditionalOptions, varVal.bstrVal);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CVCProjConvert::SetBSCToolSetting(IVCToolImpl* pTool, IVCPropertyContainer* pPropContainer,
	CComVariant& varVal, COptStr* pOptStr,  CProjItem* pProjItem, COptionHandler* pOptHandler)
{
	CComQIPtr<VCBscMakeTool> pBscMake = pTool;
	if (pBscMake && pPropContainer)
	{
		switch (pOptStr->idOption)
		{
		case IDOPT_BSCMAKE_NOLOGO: // IDOPT_BSCMAKE_NOLOGO,	"nologo%T1", OPTARGS1(P_BscMakeNoLogo), single,
			SetBoolProperty(pPropContainer, VCBSCID_SuppressStartupBanner, varVal.lVal);
			break;
		case IDOPT_BSCMAKE_INCUNREF: // IDOPT_BSCMAKE_INCUNREF,	"Iu%T1", OPTARGS1(P_InclUnref), single,
			AddExtraOption(pPropContainer, VCBSCID_AdditionalOptions, varVal.lVal, L"/Iu");
			break;
		case IDOPT_BSCMAKE_OUTNAME: // IDOPT_BSCMAKE_OUTNAME,	"o%1", OPTARGS1(P_BscMakeOutputFilename), single,
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				SetStrProperty(pPropContainer, VCBSCID_OutputFile, varVal.bstrVal);
			}
			break;
		case IDOPT_UNKNOWN_OPTION: //  IDOPT_UNKNOWN_OPTION,	"", NO_OPTARGS, single, PostMiscellaneous
			{
				VSASSERT(varVal.vt == VT_BSTR, "String expected");
				AddAdditionalOptions(pPropContainer, VCBSCID_AdditionalOptions, varVal.bstrVal);
			}
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CVCProjConvert::AddCustomBuildEventItem(CProjItem *pItem, VCConfiguration *pConfiguration)
{
	if (pConfiguration == NULL)
		return FALSE;

	CProjItem *pProjItem = (CProjItem *)pItem;
	OptBehaviour optbehOld = pProjItem->GetOptBehaviour();
	pProjItem->SetOptBehaviour((OptBehaviour)(optbehOld & ~OBInherit));	// do NOT inherit custom step values from config!

	CString strCommand;
	BOOL bExcludedFromBuild;
	CString strDescription;
	CString strBuildTime;

	pItem->GetConvertedStrProp(P_ItemCustomBuildTime, strBuildTime);
	pItem->GetConvertedStrProp(P_CustomDescription, strDescription);
	pItem->GetConvertedStrProp(P_CustomCommand, strCommand);
	pItem->GetIntProp(P_ItemExcludedFromBuild, bExcludedFromBuild);

	CComPtr<IVCToolImpl> pTool;
	SetToolObject(pConfiguration, strBuildTime, &pTool);
	CComQIPtr<IVCBuildEventToolInternal> pBuildEventTool = pTool;
	VSASSERT(pBuildEventTool, "Failed to set desired build event tool on config!");
	if (pBuildEventTool)
	{
		if (!strCommand.IsEmpty())
			pBuildEventTool->put_CommandLineInternal(CComBSTR(strCommand));
		if (!strDescription.IsEmpty())
			pBuildEventTool->put_DescriptionInternal(CComBSTR(strDescription));
		pBuildEventTool->put_ExcludedFromBuildInternal(bExcludedFromBuild ? VARIANT_TRUE : VARIANT_FALSE);
	}

	pProjItem->SetOptBehaviour(optbehOld);
	return TRUE;
}

BOOL CVCProjConvert::AddCustomBuildItem(CProjItem *pItem, VCCustomBuildTool* pCustomBuildTool, VCFileConfiguration *pFileConfig, VCConfiguration *pConfig)
{// P_CustomDescription P_CustomCommand
	if ((pFileConfig || pConfig) && pItem && pCustomBuildTool)
	{
		CString strDescription;
		CString strCustomCommand;
		CString strOutputs;
		CString strInputs;

		pItem->GetConvertedStrProp(P_CustomDescription, strDescription);
		pItem->GetConvertedStrProp(P_CustomCommand, strCustomCommand);
		CCustomBuildTool::ConvertCustomBuildStrProp(pItem, P_CustomOutputSpec, strOutputs);
		CCustomBuildTool::ConvertCustomBuildStrProp(pItem, P_UserDefinedDeps, strInputs);

		if (!strCustomCommand.IsEmpty())
			pCustomBuildTool->put_CommandLine(CComBSTR(strCustomCommand));
		if (!strDescription.IsEmpty())
			pCustomBuildTool->put_Description(CComBSTR(strDescription));
		if (!strOutputs.IsEmpty())
			pCustomBuildTool->put_Outputs(CComBSTR(strOutputs));
		if (!strInputs.IsEmpty())
			pCustomBuildTool->put_AdditionalDependencies(CComBSTR(strInputs));
	}
	return FALSE;
}


bool CVCProjConvert::GetConfigName(CString& strTargetName, CString& strConfigName, BOOL bFileConfig /* = FALSE */)
{
	CString strPlatformName;
	strConfigName.Empty();
	// strTargetName is in form : "<projectname> - <platform-name> <configuration>"
	int i, j;
	i = strTargetName.Find(_T(" - "));
	if (i > 0)
	{
		j = strTargetName.Find(_T(" "), i+3);
		if (j > 0)
		{
			strPlatformName = strTargetName.Mid(i + 3, j - i - 3);
			strConfigName = strTargetName.Mid(j+1);
			// pull out any (80x86) we may find since this is the default, anyway
			j = strConfigName.Find(_T("(80x86)"));
			if (j >= 0)
			{
				if (j == 0)	// found it on the left
				{
					strConfigName = strConfigName.Right(strConfigName.GetLength()-7);
					strConfigName.TrimLeft();
				}
				else if (j == (strConfigName.GetLength()-7))	// found it on the right
				{
					strConfigName = strConfigName.Left(strConfigName.GetLength()-7);
					strConfigName.TrimRight();
				}
				else	// found it in the middle
				{
					CString strLeft, strRight;
					strLeft = strConfigName.Left(j-1);
					strLeft.TrimRight();
					strRight = strConfigName.Right(strConfigName.GetLength()-j-7);
					strRight.TrimLeft();
					strConfigName = strLeft;
					strConfigName += _T(" ");
					strConfigName += strRight;
				}
			}
			if (bFileConfig)
			{
				CString strT;

				strT = strConfigName;
				strT += _T("|");
				strT += strPlatformName;
				strConfigName = strT;
			}
		}
		return true;
	}
	else	// must be early version of product, say, 2.0, where format is <platform> <config>
	{
		if (!SplitOutV2ConfigName(IDS_V2_WIN32X86_PLATFORM, strTargetName, strConfigName, bFileConfig) ||
			!SplitOutV2ConfigName(IDS_V2_WIN3280X86_PLATFORM, strTargetName, strConfigName, bFileConfig) ||
			!SplitOutV2ConfigName(IDS_V2_WIN32_PLATFORM, strTargetName, strConfigName, bFileConfig))
			return false;
		return true;
	}
}

bool CVCProjConvert::SplitOutV2ConfigName(UINT idPlatform, CString& strTargetName, CString& strConfigName, 
	BOOL bFileConfig /* = FALSE */)
{
	CString strPlatform;
	if (!strPlatform.LoadString(idPlatform))
		return false;

	int nLen = strPlatform.GetLength();
	if (_tcsncmp(strPlatform, strTargetName, nLen))
		return false;	// not for us

	strConfigName = strTargetName.Right(strTargetName.GetLength()-nLen);
	strConfigName.TrimLeft();
	strConfigName.TrimRight();

	if (bFileConfig)
	{
		// pull out any (80x86) we may find since this is the default, anyway
		int j = strPlatform.Find(_T("(80x86)"));
		if (j >= 0)
		{
			CString strLeft, strRight;
			strLeft = strPlatform.Left(j);
			if (j < nLen-7)
				strRight = strPlatform.Right(j+7);
			strLeft.TrimRight();
			strRight.TrimLeft();
			if (!strLeft.IsEmpty() && !strRight.IsEmpty())
				strLeft += _T(" ");
			strPlatform = strLeft + strRight;
		}
		strPlatform.TrimLeft();
		strPlatform.TrimRight();
		strConfigName += _T("|");
		strConfigName += strPlatform;
	}

	return true;
}

BOOL CVCProjConvert::PropertyIsDifferent(CComBSTR& bstrOld, CComBSTR& bstrNew, BOOL bBlankOK /* = FALSE */)
{
	if (bstrNew.Length() == 0)
	{
		if (!bBlankOK || bstrOld.Length() == 0)
			return FALSE;
	}

	if (bstrOld.Length() == 0)
		return TRUE;

	return (_wcsicmp(bstrOld, bstrNew) != 0);
}


BOOL CVCProjConvert::PropertyIsDifferent(CComBSTR& bstrOld, BSTR bstrNew, BOOL bBlankOK /* = FALSE */)
{
	CComBSTR bstrNew2 = bstrNew;
	return PropertyIsDifferent(bstrOld, bstrNew2, bBlankOK);
}

VARIANT_BOOL CVCProjConvert::ConvertToBoolValue(long lVal)
{
	if (lVal)
		return VARIANT_TRUE;

	return VARIANT_FALSE;
}

BOOL CVCProjConvert::EnumIsTrue(long lVal)
{
	return (lVal == 2);
}

void CVCProjConvert::AddExtraOption(IVCPropertyContainer* pPropContainer, long propID, long lVal, BSTR bstrOption)
{
	if (EnumIsTrue(lVal))
		AddAdditionalOptions(pPropContainer, propID, bstrOption);
}

void CVCProjConvert::AddAdditionalOptions(IVCPropertyContainer* pPropContainer, long propID, BSTR bstrOption)
{
	CComBSTR bstrNewValue = bstrOption;
	if (bstrNewValue.Length() == 0)
		return;

	CComBSTR bstrCurrentValue;
	pPropContainer->GetStrProperty(propID, &bstrCurrentValue);
	if (bstrCurrentValue.Length() > 0)
	{
		if (bstrCurrentValue == bstrNewValue)
			return;	// they're the same
		bstrCurrentValue += L" ";
	}
	bstrCurrentValue += bstrNewValue;
	pPropContainer->SetStrProperty(propID, bstrCurrentValue);
}

void CVCProjConvert::SetBoolProperty(IVCPropertyContainer* pPropContainer, long propID, long lVal, 
	BOOL bFlipIt /* = FALSE */)
{
	VARIANT_BOOL bNewValue = ConvertToBoolValue(lVal);
	if (bFlipIt)
		bNewValue = (bNewValue == VARIANT_TRUE) ? VARIANT_FALSE : VARIANT_TRUE;
	VARIANT_BOOL bOldValue;
	pPropContainer->GetBoolProperty(propID, &bOldValue);
	if (bOldValue != bNewValue)
		pPropContainer->SetBoolProperty(propID, bNewValue);
}

void CVCProjConvert::SetStrProperty(IVCPropertyContainer* pPropContainer, long propID, BSTR bstrNewVal, BOOL bBlankOK /* = FALSE */)
{
	CComBSTR bstrOldVal, bstrNewEval;
	// compare _evaluated_ string props
	pPropContainer->GetEvaluatedStrProperty(propID, &bstrOldVal);
	pPropContainer->Evaluate( bstrNewVal, &bstrNewEval );
	if (PropertyIsDifferent(bstrOldVal, bstrNewEval, bBlankOK))
		pPropContainer->SetStrProperty(propID, bstrNewVal);
}

void CVCProjConvert::SetMultiStrProperty(IVCPropertyContainer* pPropContainer, long propID, BSTR bstrNewVal, bool bIsBase)
{
	CComBSTR bstrNewVal2 = bstrNewVal;
	if (bstrNewVal2.Length() > 0)
	{
		CComBSTR bstrCurrentVal;
		pPropContainer->GetStrProperty(propID, &bstrCurrentVal);
		if (bstrCurrentVal.Length() == 0 || wcscmp(bstrCurrentVal, bstrNewVal) != 0)
		{
			if (bstrCurrentVal.Length() == 0)
				bstrCurrentVal.Append(bstrNewVal);
			else
			{
				CStringW strCurrentVal = bstrCurrentVal;
				CStringW strNewVal = bstrNewVal;
				CStringW strSeparatorList = L";,";
				CStringW strItem;
				int nMaxIdx = strNewVal.GetLength();
				int nNextIdx = 0;
				while (nNextIdx >= 0)
				{
					nNextIdx = GetNextItem(strNewVal, nNextIdx, nMaxIdx, strSeparatorList, strItem);
					int nFoundIdx = FindItem(strCurrentVal, strItem, strSeparatorList, 0);
					if (nFoundIdx < 0)		// not there
					{
						bstrCurrentVal.Append(L";");
						CComBSTR bstrItem = strItem;
						bstrCurrentVal.Append(bstrItem);
					}
				}


			}
			if (!bIsBase)
				bstrCurrentVal.Append(L";$(NoInherit)");
			pPropContainer->SetStrProperty(propID, bstrCurrentVal);
		}
	}
}

void CVCProjConvert::SetEnumProperty(IVCPropertyContainer* pPropContainer, long propID, long nNewValue)
{
	VSASSERT(nNewValue > 0, "Old enums are one-based!");

	if (nNewValue <= 0)
		return;

	nNewValue--;	// old stuff is one-based, new stuff is zero-based
	long nOldValue = 0;
	pPropContainer->GetIntProperty(propID, &nOldValue);
	if (nNewValue != nOldValue)
		pPropContainer->SetIntProperty(propID, nNewValue);
}

void CVCProjConvert::SetIntProperty(IVCPropertyContainer* pPropContainer, long propID, long nNewValue,
	BOOL bZeroOK /* = FALSE */)
{
	if (nNewValue == 0 && !bZeroOK)
		return;

	long nOldValue = 0;
	pPropContainer->GetIntProperty(propID, &nOldValue);
	if (nNewValue != nOldValue)
		pPropContainer->SetIntProperty(propID, nNewValue);
}

void CVCProjConvert::SetPtrToMember1(IVCPropertyContainer* pPropContainer, long lVal)
{  // IDOPT_PTRMBR1, "vm%{b|g}1", OPTARGS1(P_PtrMbr1), single,
	CComBSTR bstrSwitch;
	switch (lVal)
	{
	default:
	case 1:	// /vmb
		bstrSwitch = L"/vmb";
		break;
	case 2:	// /vmg
		bstrSwitch = L"/vmg";
		break;
	}

	if (bstrSwitch.Length())
		AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, lVal, bstrSwitch);
}

void CVCProjConvert::SetPtrToMember2(IVCPropertyContainer* pPropContainer, long lVal)
{ // IDOPT_PTRMBR2, "vm%{v|s|m}1", OPTARGS1(P_PtrMbr2), single,
	CComBSTR bstrSwitch;
	switch (lVal)
	{
	default:
	case 1:	// /vmv
		bstrSwitch = L"/vmv";
		break;
	case 2:	// /vms
		bstrSwitch = L"/vms";
		break;
	case 3:	// /vmm
		bstrSwitch = L"/vmm";
		break;
	}

	if (bstrSwitch.Length())
		AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, lVal, bstrSwitch);
}

void CVCProjConvert::SetDebugInfo(VCCLCompilerTool* pCompiler, long lVal)
{ // IDOPT_DEBUGINFO,	"Z%{d|7|i|I}1", OPTARGS1(P_DebugInfo), single, 
	debugOption lNewDebugInformation = debugDisabled;
	switch (lVal)
	{
	case 1:	// /Zd
		lNewDebugInformation = debugLineInfoOnly;
		break;
	case 2:	// /Z7
		lNewDebugInformation = debugOldStyleInfo;
		break;
	case 3:	// /Zi
		lNewDebugInformation = debugEnabled;
		break;
	case 4:	// /ZI
		lNewDebugInformation = debugEditAndContinue;
		break;
	}
	debugOption lOldDebugInformation;
	pCompiler->get_DebugInformationFormat(&lOldDebugInformation);
	if (lOldDebugInformation != lNewDebugInformation)
		pCompiler->put_DebugInformationFormat(lNewDebugInformation); 	// see debugOption enum 
}

void CVCProjConvert::SetOptimization(VCCLCompilerTool* pCompiler, long lVal)
{ // IDOPT_OPTIMIZE, "O%{d|2|1}1", OPTARGS1(P_Optimize), single,
	optimizeOption lOptimization = optimizeCustom, oo = optimizeCustom;
	pCompiler->get_Optimization(&lOptimization);
	switch(lVal)
	{
	case 1: // Od
		oo = optimizeDisabled;
		break;
	case 2: // O2
		oo = optimizeMaxSpeed;
		break;
	case 3: // O1
		oo = optimizeMinSpace;
		break;
	}
	if (lOptimization != oo)
		pCompiler->put_Optimization(oo); 	// see optimizeOption enum 
}

void CVCProjConvert::SetOptimizeX(VCCLCompilerTool* pCompiler, long lVal)
{	// IDOPT_OPT_X, "Ox%T1%<4>2", OPTARGS2(P_Optimize_X, P_Optimize), single,
	optimizeOption lNewOptimization = lVal ? optimizeFull : optimizeDisabled;
	optimizeOption lOldOptimization;
	pCompiler->get_Optimization(&lOldOptimization);
	if (lNewOptimization != lOldOptimization)
		pCompiler->put_Optimization(lNewOptimization);
}

void CVCProjConvert::SetCodeGenFavor(VCCLCompilerTool* pCompiler, UINT idOption, long lVal)
{	// IDOPT_OPT_T, "Ot%T1%<4>2", OPTARGS2(P_Optimize_T, P_Optimize), single,
	// IDOPT_OPT_S, "Os%T1%<4>2", OPTARGS2(P_Optimize_S, P_Optimize), single, FavorSizeOrSpeed // see favorSizeOrSpeedOption enum
	favorSizeOrSpeedOption lFavorSizeOrSpeed, fcgo;
	if (idOption == IDOPT_OPT_T && lVal != 0)
		fcgo = favorSpeed;
	else if (lVal != 0)
		fcgo = favorSize;
	else
		fcgo = favorNone;

	pCompiler->get_FavorSizeOrSpeed(&lFavorSizeOrSpeed);
	if (lFavorSizeOrSpeed != fcgo)
		pCompiler->put_FavorSizeOrSpeed(fcgo); 	// see favorSizeOrSpeedOption enum 
}

void CVCProjConvert::SetStringPooling(VCCLCompilerTool* pCompiler, long lVal)
{	// converting /Gf to /GF on purpose
	VARIANT_BOOL bPoolNew = (lVal != 0) ? VARIANT_TRUE : VARIANT_FALSE;
	VARIANT_BOOL bPoolOld;
	pCompiler->get_StringPooling(&bPoolOld);
	if (bPoolNew != bPoolOld)
		pCompiler->put_StringPooling(bPoolNew);
}

void CVCProjConvert::SetAsmOptions(VCCLCompilerTool* pCompiler, CProjItem* pProjItem, COptionHandler* pOptHandler,
	long lVal)
{	// IDOPT_LISTASM, "FA%{%f1%f2|cs%t1%t2|c%t1%f2|s%f1%t2}3",	OPTARGS3(P_AsmListHasMC, P_AsmListHasSrc, P_ListAsm), single,
	// Looks like it doesn't matter WHAT P_ListAsm refers to as long as it is a non-default property...
	int iAsmListHasMC = 0, iAsmListHasSrc = 0;
	asmListingOption lNewAssemblerOutput = asmListingAssemblyOnly;
	if (pProjItem->GetIntProp(pOptHandler->MapLogical(P_AsmListHasMC), iAsmListHasMC) != valid)
		iAsmListHasMC = 0;
	if (pProjItem->GetIntProp(pOptHandler->MapLogical(P_AsmListHasSrc), iAsmListHasSrc) != valid)
		iAsmListHasSrc = 0;
	if (iAsmListHasMC)
	{
		if (iAsmListHasSrc)
			lNewAssemblerOutput = asmListingAsmMachineSrc;
		else
			lNewAssemblerOutput = asmListingAsmMachine;
	}
	else if (iAsmListHasSrc)
		lNewAssemblerOutput = asmListingAsmSrc;
	asmListingOption lOldAssemblerOutput;
	pCompiler->get_AssemblerOutput(&lOldAssemblerOutput);
	if (lOldAssemblerOutput != lNewAssemblerOutput)
		pCompiler->put_AssemblerOutput(lNewAssemblerOutput); 	// see asmListingOption enum 
}

void CVCProjConvert::SetListBsc(VCCLCompilerTool* pCompiler, IVCPropertyContainer* pPropContainer, 
	CProjItem* pProjItem, COptionHandler* pOptHandler, 
	UINT idOption, long lVal)
{	// IDOPT_LISTBSCNOLCL,	"Fr%T1%t2[%3]", OPTARGS3(P_GenBrowserInfo, P_GenBrowserInfoNoLcl, P_OutputDir_Sbr),	single,
	// IDOPT_LISTBSC, "FR%T1%f2[%3]", OPTARGS3(P_GenBrowserInfo, P_GenBrowserInfoNoLcl, P_OutputDir_Sbr),	single,
	browseInfoOption newListBsc;
	if (idOption == IDOPT_LISTBSCNOLCL)
		newListBsc = lVal ? brNoLocalSymbols : brInfoNone;
	else
		newListBsc = lVal ? brAllInfo : brInfoNone;
	CString strBscFile;
	if (newListBsc != brInfoNone)
	{
		int iGenInfo = FALSE;
		if (pProjItem->GetIntProp(pOptHandler->MapLogical(P_GenBrowserInfoNoLcl), iGenInfo) == valid
			&& iGenInfo)
			pProjItem->GetConvertedStrProp(pOptHandler->MapLogical(P_OutputDir_Sbr), strBscFile);
	}
	browseInfoOption oldListBsc = brInfoNone;
	pCompiler->get_BrowseInformation(&oldListBsc);
	if (oldListBsc != newListBsc)
		pCompiler->put_BrowseInformation(newListBsc);
	if (newListBsc != brInfoNone && !strBscFile.IsEmpty())
	{
		CComBSTR bstrBscFile = strBscFile;
		SetStrProperty(pPropContainer, VCCLID_BrowseInformationFile, bstrBscFile);
	}
}

void CVCProjConvert::SetAutoPCH(VCCLCompilerTool* pCompiler, IVCPropertyContainer* pPropContainer,
	COptionHandler* pOptHandler, CProjItem* pProjItem, long lVal)
{	// IDOPT_AUTOPCH, "YX%T1[%2]", OPTARGS2(P_AutoPch, P_AutoPchUpTo), single,
// /nologo /MDd /RTC1 /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" 
// /D "_MBCS" /Fp"Debug/mfcone.pch" /Yc"stdafx.h" /Fo"Debug/" /Fd"Debug/" /FD /c "x:\tests\mfcone\StdAfx.cpp"

	pchOption pchNewVal = (lVal ? pchGenerateAuto : pchNone);
	pchOption pchOldVal;
	pCompiler->get_UsePrecompiledHeader(&pchOldVal);
	if (pchNewVal != pchOldVal)
		pCompiler->put_UsePrecompiledHeader(pchNewVal);

	CString strHeader;
	if (pchNewVal != pchNone && pProjItem->GetConvertedStrProp(pOptHandler->MapLogical(P_AutoPchUpTo), 
		strHeader) == valid && !strHeader.IsEmpty())
	{
		CComBSTR bstrHeader = strHeader;
		SetStrProperty(pPropContainer, VCCLID_PrecompiledHeaderThrough, bstrHeader);
	}
}

void CVCProjConvert::SetCreatePCH(VCCLCompilerTool* pCompiler, IVCPropertyContainer* pPropContainer,
	COptionHandler* pOptHandler, CProjItem* pProjItem, long lVal)
{	// IDOPT_PCHCREATE,	"Yc%T1[%2]", OPTARGS2(P_PchCreate, P_PchCreateUptoHeader),	single,
// /nologo /MDd /RTC1 /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" 
// /D "_MBCS" /Fp"Debug/mfcone.pch" /Yc"stdafx.h" /Fo"Debug/" /Fd"Debug/" /FD /c "x:\tests\mfcone\StdAfx.cpp"

	pchOption pchNewVal = lVal ? pchCreateUsingSpecific : pchNone;
	pchOption pchOldVal;
	pCompiler->get_UsePrecompiledHeader(&pchOldVal);
	if (pchNewVal != pchOldVal)
		pCompiler->put_UsePrecompiledHeader(pchNewVal);

	CString strHeader;
	if (pchNewVal != pchNone)
	{
		GPT gptRet = pProjItem->GetConvertedStrProp(pOptHandler->MapLogical(P_PchCreateUptoHeader), strHeader);
		if (gptRet != valid || strHeader.IsEmpty())
		{
			// HACK: didn't always save the info about what file to use...
			CProject* pProject = pProjItem->GetProject();
			pOptHandler->SetSlob(pProject);
			gptRet = pProject->GetConvertedStrProp(pOptHandler->MapLogical(P_PchUseUptoHeader), strHeader);
			pOptHandler->ResetSlob();
		}
		if (gptRet == valid && !strHeader.IsEmpty())
		{
			CComBSTR bstrHeader = strHeader;
			SetStrProperty(pPropContainer, VCCLID_PrecompiledHeaderThrough, bstrHeader);
		}
	}
}

void CVCProjConvert::SetUsePCH(VCCLCompilerTool* pCompiler, IVCPropertyContainer* pPropContainer, 
	COptionHandler* pOptHandler, CProjItem* pProjItem, long lVal)
{	// IDOPT_PCHUSE, "Yu%T1[%2]", OPTARGS2(P_PchUse, P_PchUseUptoHeader), single,
	SetIntProperty(pPropContainer, VCCLID_UsePrecompiledHeader, lVal ? pchUseUsingSpecific : pchNone, TRUE);
	if (lVal)
	{
		CString strHeader;
		if (pProjItem->GetConvertedStrProp(pOptHandler->MapLogical(P_PchUseUptoHeader), 
			strHeader) == valid)
		{
			CComBSTR bstrHeader = strHeader;
			SetStrProperty(pPropContainer, VCCLID_PrecompiledHeaderThrough, bstrHeader);
		}
	}
}

void CVCProjConvert::SetStackCheck(IVCPropertyContainer* pPropContainer, long lVal)
{	// IDOPT_STACK_PROBE_THOLD,	"Gs%1", OPTARGS1(P_StackProbeTHold), single,
	if (lVal == 0)
		return;

	CStringW strOption;
	strOption.Format(L"/Gs%d", lVal);
	CComBSTR bstrOption = strOption;
	AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, lVal, bstrOption);
}

void CVCProjConvert::SetPreprocess1(VCCLCompilerTool* pCompiler, UINT idOption, long lVal)
{	// IDOPT_PREPROCESS_WO_LINES,	"EP%T1", OPTARGS1(P_PreprocessWOLines), single,
	// IDOPT_PREPROCESS, "E%T1", OPTARGS1(P_Preprocess), single,
	preprocessOption lPreprocess, ppo;
	if (idOption == IDOPT_PREPROCESS_WO_LINES && lVal != 0)
		ppo = preprocessNoLineNumbers;
	else if (lVal != 0)
		return;		// silently don't do this one
	else
		ppo = preprocessNo;

	pCompiler->get_GeneratePreprocessedFile(&lPreprocess);
	if (lPreprocess != ppo)
		pCompiler->put_GeneratePreprocessedFile(ppo); 	// see preprocessOption enum 
}

void CVCProjConvert::SetPreprocess2(VCCLCompilerTool* pCompiler, CProjItem* pProjItem, long lVal)
{	// IDOPT_PREPROCESS_TO_FILE,	"P%T1", OPTARGS1(P_PrepprocessToFile), single,
	preprocessOption oldPreProcessFile = preprocessNo;
	pCompiler->get_GeneratePreprocessedFile(&oldPreProcessFile);

	if (oldPreProcessFile == preprocessNo && !lVal)
		return;	// they're the same

	if (lVal && (oldPreProcessFile == preprocessYes || oldPreProcessFile == preprocessNoLineNumbers))
		return;	// again, they're the same

	if (!lVal)
		pCompiler->put_GeneratePreprocessedFile(preprocessNo);
	else
		pCompiler->put_GeneratePreprocessedFile(preprocessYes);
}

void CVCProjConvert::SetTreatFileAs(VCCLCompilerTool* pCompiler, UINT idOption, long lVal)
{	// IDOPT_TREATFILEAS_CPP1,	"Tp%T1", OPTARGS1(P_TreatFileAsCPP1), single,	
	// IDOPT_TREATFILEAS_CPP2,	"TP%T1", OPTARGS1(P_TreatFileAsCPP2), single,	
	// IDOPT_TREATFILEAS_C1,	"Tc%T1", OPTARGS1(P_TreatFileAsC1), single,
	// IDOPT_TREATFILEAS_C2,	"TC%T1", OPTARGS1(P_TreatFileAsC2), single,
	CompileAsOptions lCompileAs, cao;
	if ((idOption == IDOPT_TREATFILEAS_CPP1 || idOption == IDOPT_TREATFILEAS_CPP2) && lVal != 0)
		cao = compileAsCPlusPlus;
	else if (lVal != 0)
		cao = compileAsC;
	else
		cao = compileAsDefault;

	pCompiler->get_CompileAs(&lCompileAs);
	if (lCompileAs != cao)
		pCompiler->put_CompileAs(cao); 	// see CompileAsOptions enum 
}

void CVCProjConvert::SetProcOptimize(VCCLCompilerTool* pCompiler, long lVal)
{
	ProcessorOptimizeOption OptimizeForProcessor, poo;
	BOOL bIgnoreSwitch = FALSE;
	switch(lVal)
	{
	case 1: // G3
	case 2: // G4
		bIgnoreSwitch = TRUE;
		break;
	case 3: // G5
		poo = procOptimizePentium;
		break;
	case 4: // G6
		poo = procOptimizePentiumProAndAbove;
		break;
	default:
	case 5: // GB
		poo = procOptimizeBlended;
		break;
	}
	if (bIgnoreSwitch)
		return;		// nothing to do

	pCompiler->get_OptimizeForProcessor(&OptimizeForProcessor);
	if (!bIgnoreSwitch && OptimizeForProcessor != poo)
		pCompiler->put_OptimizeForProcessor(poo);
}

void CVCProjConvert::SetCallConvention(VCCLCompilerTool* pCompiler, long lVal)
{	// IDOPT_CALLCONV_ALPHA,	"G%{d}1", OPTARGS1(P_CallConvAlpha), single,
	// IDOPT_CALLCONV_MERCED,	"G%{d}1", OPTARGS1(P_CallConvMerced), single,
	// IDOPT_CALLCONV_MIPS,	"G%{d|r}1", OPTARGS1(P_CallConvMips), single,
	// IDOPT_CALLCONV, "G%{d|r|z}1", OPTARGS1(P_CallConv), single, CallingConvention // see callingConventionOption enum
	callingConventionOption newCallingConvention = callConventionCDecl;
	switch (lVal)
	{
	default:
	case 1:	// /Gd
		newCallingConvention = callConventionCDecl;
		break;
	case 2:	// /Gr
		newCallingConvention = callConventionFastCall;
		break;
	case 3:
		newCallingConvention = callConventionStdCall;
		break;
	}
	callingConventionOption oldCallingConvention;
	pCompiler->get_CallingConvention(&oldCallingConvention);
	if (oldCallingConvention != newCallingConvention)
		pCompiler->put_CallingConvention(newCallingConvention);
}

void CVCProjConvert::SetByteAlign(VCCLCompilerTool* pCompiler, long lVal)
{	// IDOPT_BYTE_MERCED,	"Zp%{2|4|8|16|[1]}1",	OPTARGS1(P_ByteAlignMerced), single,
	// IDOPT_BYTE_MIPS,	"Zp%{2|4|8|16|[1]}1",	OPTARGS1(P_ByteAlignMips), single,
	// IDOPT_BYTE_ALPHA,	"Zp%{2|4|8|16|[1]}1",	OPTARGS1(P_ByteAlignAlpha), single,
	// IDOPT_BYTE, "Zp%{2|4|8|16|[1]}1", OPTARGS1(P_ByteAlign), single, StructMemberAlignment see structMemberAlignOption enum
	structMemberAlignOption StructMemberAlignment, smao;
	pCompiler->get_StructMemberAlignment(&StructMemberAlignment);
	switch(lVal)
	{
	case 1: // Zp2
		smao = alignTwoBytes;
		break;
	case 2: // Zp4
		smao = alignFourBytes;
		break;
	case 3: // Zp8
		smao = alignEightBytes;
		break;
	case 4: // Zp16
		smao = alignSixteenBytes;
		break;
	default:
	case 5: // Zp1
		smao = alignSingleByte;
		break;
	}
	if (StructMemberAlignment != smao)
		pCompiler->put_StructMemberAlignment(smao);
}

void CVCProjConvert::SetRuntimeLib(VCCLCompilerTool* pCompiler, long lVal)
{	// IDOPT_THREAD_MIPS,	"M%{Ld|Td|Dd|L|T|D}1", OPTARGS1(P_ThreadMips), single,
	// IDOPT_THREAD, "M%{Ld|Td|Dd|L|T|D}1",	OPTARGS1(P_Thread), single, RuntimeLibrary runtimeLibraryOption
	// IDOPT_THREAD_MERCED (+3), "M%{L|T|D}1", OPTARGS1(P_ThreadMerced), single,
	// IDOPT_THREAD_ALPHA (+3), "M%{L|T|D}1", OPTARGS1(P_ThreadAlpha), single,
	runtimeLibraryOption RuntimeLibrary, rlo;
	pCompiler->get_RuntimeLibrary(&RuntimeLibrary);

	switch(lVal)
	{
	default:
	case 1: // Ld
		rlo = rtSingleThreadedDebug;
		break;
	case 2: // Td
		rlo = rtMultiThreadedDebug;
		break;
	case 3: // Dd
		rlo = rtMultiThreadedDebugDLL;
		break;
	case 4: // L
		rlo = rtSingleThreaded;
		break;
	case 5: // T
		rlo = rtMultiThreaded;
		break;
	case 6: // D
		rlo = rtMultiThreadedDLL;
		break;
	}

	if (RuntimeLibrary != rlo)
		pCompiler->put_RuntimeLibrary(rlo);
}

void CVCProjConvert::SetOldEH(VCCLCompilerTool* pCompiler, long lVal)
{	// IDOPT_ENABLE_EH,	"GX%{-|}1", OPTARGS1(P_Enable_EH), single,  enables synchronous exception 
	VARIANT_BOOL neho = lVal ? VARIANT_TRUE : VARIANT_FALSE;
	VARIANT_BOOL eho = VARIANT_FALSE;
	pCompiler->get_ExceptionHandling(&eho);
	if (eho != neho)
		pCompiler->put_ExceptionHandling(neho);
}

void CVCProjConvert::SetRuntimeChecks(VCCLCompilerTool* pCompiler, long lVal, basicRuntimeCheckOption newOpt)
{
	basicRuntimeCheckOption oldOpt = runtimeBasicCheckNone;
	HRESULT hr = pCompiler->get_BasicRuntimeChecks(&oldOpt);
	if (FAILED(hr))
		return;
	else if (hr == S_FALSE)		// nothing previously set
		pCompiler->put_BasicRuntimeChecks(lVal ? newOpt : runtimeBasicCheckNone);
	else if (oldOpt == runtimeBasicCheckAll)
	{
		if (newOpt == runtimeBasicCheckAll)	// already included if true, not allowing explicit removal of anything but all if false
			pCompiler->put_BasicRuntimeChecks(lVal ? newOpt : runtimeBasicCheckNone);
	}
	else	// need to check for inclusion
	{
		if (newOpt == oldOpt)
			pCompiler->put_BasicRuntimeChecks(lVal ? newOpt : runtimeBasicCheckNone);
		else if (lVal)
			pCompiler->put_BasicRuntimeChecks(runtimeBasicCheckAll);
	}
}

void CVCProjConvert::SetLinkShowProgress(VCLinkerTool* pLinker, long lVal)
{
	linkProgressOption newShowProgress = lVal ? linkProgressAll : linkProgressNotSet;
	linkProgressOption oldShowProgress;
	pLinker->get_ShowProgress(&oldShowProgress);
	if (oldShowProgress != newShowProgress)
		pLinker->put_ShowProgress(newShowProgress);
}

void CVCProjConvert::SetUsePDB(IVCPropertyContainer* pPropContainer, CProjItem* pProjItem, 
	COptionHandler* pOptHandler, long lVal)
{
	CComBSTR bstrPDBFile;
	if (lVal)
	{
		CString strPDBFile;
		GPT gptRet = pProjItem->GetConvertedStrProp(pOptHandler->MapLogical(P_UsePDBName), strPDBFile);
		if (gptRet == invalid)
		{
			gptRet = pOptHandler->GetDefStrProp(pOptHandler->MapLogical(P_UsePDBName), strPDBFile);
			if (gptRet == valid)
				::ConvertMacros(strPDBFile);
		}
		bstrPDBFile = strPDBFile;
	}
//	else
//		blank PDB file name

	SetStrProperty(pPropContainer, VCLINKID_ProgramDatabaseFile, bstrPDBFile, lVal /* blank OK if not supposed to set... */);
}

void CVCProjConvert::SetMapGen(IVCPropertyContainer* pPropContainer, CProjItem* pProjItem, COptionHandler* pOptHandler, long lVal)
{	//  IDOPT_MAPGEN, "m[ap]%T1[: // %2]", OPTARGS2(P_GenMap, P_MapName), single,
	SetBoolProperty(pPropContainer, VCLINKID_GenerateMapFile, lVal);
	if (lVal)
	{
		CString strMapFile;
		GPT gpt = pProjItem->GetConvertedStrProp(pOptHandler->MapLogical(P_MapName), strMapFile);
		if (gpt != valid)
		{
			gpt = pOptHandler->GetDefStrProp(pOptHandler->MapLogical(P_MapName), strMapFile);
			if (gpt == valid)
				::ConvertMacros(strMapFile);
		}
		if (gpt == valid)
		{
			CComBSTR bstrMapFile = strMapFile;
			SetStrProperty(pPropContainer, VCLINKID_MapFileName, bstrMapFile);
		}
	}
}

void CVCProjConvert::SetLinkerDriver(IVCPropertyContainer* pPropContainer, long lVal)
{	//	IDOPT_DRIVER, "driver%{:junk||:uponly|:wdm}%1", OPTARGS1(P_Driver), single,
	CComBSTR bstrOpt;
	switch (lVal)
	{
	case 2:
		bstrOpt = L"/DRIVER";
		break;
	case 3:
		bstrOpt = L"/DRIVER:UPONLY";
		break;
	case 4:
		bstrOpt = L"/DRIVER:WDM";
		break;
	}

	if (bstrOpt && bstrOpt != L"")
		AddAdditionalOptions(pPropContainer, VCLINKID_AdditionalOptions, bstrOpt);
}

void CVCProjConvert::SetLinkerAlign(IVCPropertyContainer* pPropContainer, long lVal)
{
	if (lVal == 0)
		return;

	CStringW strOpt;
	strOpt.Format(L"/ALIGN:%d", lVal);
	CComBSTR bstrOpt = strOpt;
	AddAdditionalOptions(pPropContainer, VCLINKID_AdditionalOptions, bstrOpt);
}

void CVCProjConvert::SetLinkerFixed(IVCPropertyContainer* pPropContainer, long lVal)
{
	CComBSTR bstrOpt;
	switch (lVal)
	{
	case 1:		// no
		bstrOpt = L"/FIXED:NO";
		break;
	case 2:		// blank
		bstrOpt = L"/FIXED";
		break;
	}

	if (bstrOpt && bstrOpt != L"")
		AddAdditionalOptions(pPropContainer, VCLINKID_AdditionalOptions, bstrOpt);
}

void CVCProjConvert::SetLinkerSubSystem(IVCPropertyContainer* pPropContainer, long lVal)
{
	subSystemOption lSubSystem, sso;
	pPropContainer->GetIntProperty(VCLINKID_SubSystem, (long *)&lSubSystem);
	switch (lVal)
	{
	default:
		sso = subSystemNotSet;
		break;
	case 1:
		sso = subSystemWindows;
		break;
	case 2:
		sso = subSystemConsole;
		break;
	}
	if (lSubSystem != sso)
		pPropContainer->SetIntProperty(VCLINKID_SubSystem, sso); 	// see subSystemOption enum 
}

void CVCProjConvert::SetLinkerLargeAddressAwareness(IVCPropertyContainer* pPropContainer, long lVal)
{	// IDOPT_LARGEADDRESS,	"largeaddressaware%{|:no}1", OPTARGS(P_LargeAddress), single,
	addressAwarenessType lAwareness = addrAwareDefault, mto = addrAwareDefault;
	pPropContainer->GetIntProperty(VCLINKID_LargeAddressAware, (long *)&lAwareness);
	switch (lVal)
	{
	case 1:	// no
		mto = addrAwareNoLarge;
		break;
	case 2:	// blank
		mto = addrAwareLarge;
		break;
	}

	if (lAwareness != mto)
		pPropContainer->SetIntProperty(VCLINKID_LargeAddressAware, mto);
}

void CVCProjConvert::SetLinkerVersion(IVCPropertyContainer* pPropContainer, CProjItem* pProjItem, 
	COptionHandler* pOptHandler, long lVal)
{
	CVCString strVersion;
	int i;
	if (pProjItem->GetIntProp(pOptHandler->MapLogical(P_VersionMin), i) == valid)
		strVersion.Format(_T("%d.%d"), lVal, i);
	else
		strVersion.Format(_T("%d"), lVal);
	CComBSTR bstrVersion = strVersion;
	SetStrProperty(pPropContainer, VCLINKID_Version, bstrVersion);
}

void CVCProjConvert::SetMidlOutput(IVCPropertyContainer* pPropContainer, long lPropID, BSTR bstrVal)
{
	CComBSTR bstrOut = bstrVal;
	if (bstrOut.Length() == 3)
	{
		bstrOut.ToLower();
		if (wcscmp(bstrOut, L"nul") == 0)
			return;	// don't want to set this...
	}
	SetStrProperty(pPropContainer, lPropID, bstrVal);
}

void CVCProjConvert::SetMidlStublessProxy(IVCPropertyContainer* pPropContainer, long lVal)
{  // IDOPT_MTL_STUBLESS_PROXY,	"Oicf%T1", OPTARGS1(P_MTLStublessProxy),	single,
	//GenerateStublessProxies 
	VARIANT_BOOL bGenerateStublessProxies = VARIANT_FALSE;
	pPropContainer->GetBoolProperty(VCMIDLID_GenerateStublessProxies, &bGenerateStublessProxies);
	if (lVal && bGenerateStublessProxies == VARIANT_FALSE)
		pPropContainer->SetBoolProperty(VCMIDLID_GenerateStublessProxies, VARIANT_TRUE);
	else if (!lVal && bGenerateStublessProxies == VARIANT_TRUE)
		pPropContainer->SetBoolProperty(VCMIDLID_GenerateStublessProxies, VARIANT_FALSE);
}

void CVCProjConvert::SetMidlChar(IVCPropertyContainer* pPropContainer, long lVal)
{
	midlCharOption newDefaultCharType;
	switch (lVal)
	{
	default:
	case 1:	// /char signed
		newDefaultCharType = midlCharSigned;
		break;
	case 2:	// /char ascii77
		newDefaultCharType = midlCharAscii7;
		break;
	case 3:	// /char unsigned
		newDefaultCharType = midlCharUnsigned;
		break;
	}
	midlCharOption DefaultCharType;
	pPropContainer->GetIntProperty(VCMIDLID_DefaultCharType, (long *)&DefaultCharType);
	if (DefaultCharType != newDefaultCharType)
		pPropContainer->SetIntProperty(VCMIDLID_DefaultCharType, newDefaultCharType);
}

void CVCProjConvert::SetMidlClient(IVCPropertyContainer* pPropContainer, long lVal)
{	// case IDOPT_MTL_CLIENT: // IDOPT_MTL_CLIENT, "client %{stub|none}1",	OPTARGS1(P_MTL_Client), single, Client
	if (lVal == 0)
		AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, 1, L"/client none");
	else if (lVal == 1)
		AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, lVal, L"/client stub");
}

void CVCProjConvert::SetMidlServer(IVCPropertyContainer* pPropContainer, long lVal)
{	// case IDOPT_MTL_SERVER: // IDOPT_MTL_SERVER, "server %{stub|none}1",	OPTARGS1(P_MTL_Server), single, Server
	if (lVal == 0)
		AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, 1, L"/server none");
	else if (lVal == 1)
		AddExtraOption(pPropContainer, VCCLID_AdditionalOptions, lVal, L"/server stub");
}

//
// primitive custom build 'tool macros'
// Only the ones that are changing their names for VC7 are here as all we're doing is converting those few names
// and not doing a full expansion of any kind on any of them.
//

typedef struct 
{
	UINT idMacro;
	const TCHAR * pchName;
} S_ToolMacroMapEl;

#define IDMACRO_SOLNDIR		100
#define IDMACRO_ENVDIR		101
#define IDMACRO_SOLNBASE	102
#define IDMACRO_PROJDIR		103
#define IDMACRO_CFGNAME		104
#define IDMACRO_TARGDIR		105
#define IDMACRO_REMOTEDIR	106
#define IDMACRO_INPTDIR		107

// yes, the last three in this list don't actually change name.  HOWEVER, having them here
// means that we get the conversion bit about handling the trailing slash correct.
static const S_ToolMacroMapEl mapToolMacros[] =
{
	{IDMACRO_SOLNDIR,	"WkspDir"},			// workspace directory
	{IDMACRO_ENVDIR,	"MSDEVDIR"},		// IDE directory
	{IDMACRO_SOLNBASE,	"WkspName"},		// workspace file basename
	{IDMACRO_CFGNAME,	"ConfigName"},		// configuration name
	{IDMACRO_PROJDIR,	"ProjDir"},			// path to project directory
	{IDMACRO_TARGDIR,	"TargetDir"},		// target directory
	{IDMACRO_REMOTEDIR, "RemoteDir"},		// remote target directory
	{IDMACRO_INPTDIR,	"InputDir"},		// input directory
};

static BOOL rgMacroUsage[sizeof(mapToolMacros) / sizeof(S_ToolMacroMapEl)];

// map a macro name to a macro id (IDM_)
BOOL MapMacroNameToId(const TCHAR * pchName, int & cchName, UINT & idMacro)
{
	cchName = 0;

	for (int i = sizeof(mapToolMacros) / sizeof(S_ToolMacroMapEl); i > 0; i--)
	{
		int cch = lstrlen(mapToolMacros[i-1].pchName);
		if ((cch > cchName) &&
			(_tcsnicmp(mapToolMacros[i-1].pchName, pchName, cch) == 0))
		{
			idMacro = mapToolMacros[i-1].idMacro;
			cchName = cch;	// found one match, try for bigger match....
		}
	}

	return cchName != 0;	// not mapped
}

BOOL GetMacroConversion(UINT nID, CVCString& rstrNewName)
{
	switch (nID)
	{
	case IDMACRO_SOLNDIR:		// workspace directory
		rstrNewName = _T("$(SolutionDir)");
		break;
	case IDMACRO_ENVDIR:		// IDE directory
		rstrNewName = _T("$(DevEnvDir)");
		break;
	case IDMACRO_SOLNBASE:	// workspace file basename
		rstrNewName = _T("$(SolutionName)");
		break;
	case IDMACRO_PROJDIR:
		rstrNewName = _T("$(ProjectDir)");
		break;
	case IDMACRO_CFGNAME:
		rstrNewName = _T("$(ConfigurationName)");
		break;
	case IDMACRO_TARGDIR:		// target directory
		rstrNewName = _T("$(TargetDir)");
		break;
	case IDMACRO_REMOTEDIR:	// remote target directory
		rstrNewName = _T("$(RemoteDir)");
		break;
	case IDMACRO_INPTDIR:	// input directory
		rstrNewName = _T("$(InputDir)");
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

#define CCH_ALLOC_EXPAND	512

BOOL ConvertMacros(CString& rstrVal)
{
	CVCString strIn = rstrVal;
	if (strIn.IsEmpty())
		return TRUE;
	rstrVal.Empty();

	TCHAR * pchSpec = strIn.GetBuffer(strIn.GetLength());

	int cchOut = 0;
	int cchAlloc = 0;
	TCHAR * pchOut = NULL;

	cchAlloc = cchOut = CCH_ALLOC_EXPAND;
	pchOut = (TCHAR *)malloc(cchAlloc * sizeof(TCHAR));
	*pchOut = _T('\0'); cchOut -= sizeof(TCHAR);

	CVCString strMacroName;

	// copy everything verbatim unless we come across a macro
	while (*pchSpec != _T('\0'))
	{
		// macro?
		TCHAR * pch = pchSpec;
		while (!(*pch == _T('$') && *(pch + sizeof(TCHAR)) == _T('(')) && *pch != _T('\0'))
			pch = _tcsinc(pch);

		int cchReq = 0; 
		const TCHAR * pchBlk = (const TCHAR *)NULL;

		// do block append?
		if (pch != pchSpec)
		{
			// yes
			cchReq = (int)(pch - pchSpec);

			// get block start
			pchBlk = pchSpec;

			// advance
			pchSpec = pch;
		}
		else
		{
			UINT idMacro; int cchMacro;

			// macro-name exact match?
			// skip '$('
			pch += sizeof(TCHAR) * 2;

			if (MapMacroNameToId(pch , cchMacro, idMacro) &&
				*(pch + cchMacro) == _T(')'))
			{
				// advance and skip macro
				pchSpec = pch + cchMacro;

				// skip trailing ')'
				pchSpec++;

				// check to see if this is a *Dir type macro with a trailing slash, if so, we'll want to skip it
				// note that this does NOT apply to $(IntDir) or $(OutDir)
				if (cchMacro > 4 && _tcsnicmp(pch+cchMacro-3, _T("dir)"), 4) == 0 && 
					(cchMacro < 7 || (_tcsnicmp(pch+cchMacro-6, _T("intdir)"), 7) != 0 && 
					_tcsnicmp(pch+cchMacro-6, _T("outdir)"), 7) != 0)) &&
					(*(pch + cchMacro + 1) == _T('/') || *(pch + cchMacro + 1) == _T('\\')))
					pchSpec++;

				// get the macro's new name
				if (GetMacroConversion(idMacro, strMacroName))
				{
					// get the char. block
					cchReq = strMacroName.GetLength();
					pchBlk = (const TCHAR *)strMacroName;
				}
			}
			else	// we're not converting this macro, so bring it on over as is
			{
				// yes
				cchReq = (int)(pch - pchSpec);

				// get block start
				pchBlk = pchSpec;

				// advance
				pchSpec = pch;
			}
		}

		// not just testing usage and chars. req'd and char blk to copy?
		if (cchReq && pchBlk)
		{
			if (cchReq > cchOut)
			{
				int cchIncAlloc = max(CCH_ALLOC_EXPAND, cchReq);

				cchAlloc += cchIncAlloc; cchOut += cchIncAlloc;
				pchOut = (TCHAR *)realloc(pchOut, cchAlloc * sizeof(TCHAR));
			}
						
			// copy block
			_tcsncat(pchOut, pchBlk, cchReq); cchOut -= cchReq;
		}
	}

	rstrVal = pchOut;

	// free up our local buffer
	free(pchOut);

	return TRUE;
}

