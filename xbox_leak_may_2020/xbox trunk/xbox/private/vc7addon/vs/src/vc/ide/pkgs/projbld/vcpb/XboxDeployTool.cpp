// XboxDeployTool.cpp : Implementation of Xbox Deployment Tool

#include "stdafx.h"
#include "XboxDeployTool.h"
#include "BuildEngine.h"
#include "scanner.h"
#include "ProjWriter.h"
#include "XMLFile.h"
#include "stylesheet.h"
#include "linktool.h"

// constants
const wchar_t* const wszXboxDeployDefaultExtensions = L"";	// remember to update Xbox.rgs if you change this...

// static initializers
CXboxDeployToolOptionHandler CXboxDeploymentTool::s_optHandler;
CComBSTR CXboxDeploymentTool::s_bstrBuildDescription = L"";
CComBSTR CXboxDeploymentTool::s_bstrToolName = L"";
CComBSTR CXboxDeploymentTool::s_bstrExtensions = L"";
GUID CXboxDeploymentTool::s_pPages[1];
BOOL CXboxDeploymentTool::s_bPagesInit;

////////////////////////////////////////////////////////////////////////////////
// Option Table for Web Service Utility switches

// NOTE: we're using macros for enum value ranges to help in keeping enum range checks as low maintenance as possible.
// If you add an enum to this table, make sure you use macros to define the upper and lower bounds and use CHECK_BOUNDS
// on the get/put methods associated with that property (both object model and property page).
// NOTE 2: this table is being used for persistence ONLY.  We generate our command line elsewhere since it is an XML file.
// WARNING: if you change ANYTHING about a multi-prop here (including add/remove), be sure to update how the property
// is obtained on both the tool and page objects.
BEGIN_OPTION_TABLE(CXboxDeployToolOptionHandler, L"XboxDeploymentTool", IDS_XBOXDEPLOYTOOL, TRUE /* pre & post */, FALSE /* case sensitive */)
	// general
	OPT_BOOL(ExcludedFromBuild,		L"F1|T1",		L"ExcludedFromBuild",		IDS_XBOXDEPLOYTOOL,	XBOXDPLYID)
	OPT_BSTR(RemotePath,			L"B %s",		L"RemotePath",	single,	IDS_XBOXDEPLOYTOOL,	XBOXDPLYID)
	OPT_BSTR(AdditionalFiles,		L"A %s",		L"AdditionalFiles",	single,	IDS_XBOXDEPLOYTOOL,	XBOXDPLYID)
END_OPTION_TABLE()


// default value handlers
// string props
void CXboxDeployToolOptionHandler::GetDefaultValue( long id, BSTR *pVal, IVCPropertyContainer *pPropCnt )
{
	CComBSTR bstrDef;
	switch (id)
	{
	case XBOXDPLYID_RemotePath:
		bstrDef = L"xe:\\$(SolutionName)";
		*pVal = bstrDef.Detach();
		break;
//	case XBOXDPLYID_AdditionalFiles:
//	case XBOXDPLYID_ApplicationMappings:
	default:
		GetDefaultString(pVal);
		break;
	}
}

// integer props
void CXboxDeployToolOptionHandler::GetDefaultValue( long id, long *pVal, IVCPropertyContainer *pPropCnt )
{
	// Nothing
	*pVal = 0;
}

// boolean props
void CXboxDeployToolOptionHandler::GetDefaultValue( long id, VARIANT_BOOL *pVal, IVCPropertyContainer *pPropCnt )
{
	// XBOXDPLYID_ExcludedFromBuild
	GetValueFalse( pVal );
}


///////////////////////////////////////////////////////////////////////////////
// Xbox Deployment Tool
HRESULT CXboxDeploymentTool::CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppDeployTool)
{
	CHECK_POINTER_NULL(ppDeployTool);
	*ppDeployTool = NULL;

	CComObject<CXboxDeploymentTool> *pObj;
	HRESULT hr = CComObject<CXboxDeploymentTool>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		if( pPropContainer )
			pObj->Initialize(pPropContainer);
		CXboxDeploymentTool *pVar = pObj;
		pVar->AddRef();
		*ppDeployTool = pVar;
	}
	return hr;
}

STDMETHODIMP CXboxDeploymentTool::CreatePageObject(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
{
	// Get the list of pages
	if( CLSID_XboxDeploymentGeneral == *pCLSID )
	{
		CPageObjectImpl< CXboxDeploymentGeneralPage, XBOXVCDPLYTOOL_MIN_DISPID, XBOXVCDPLYTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else
	{
		return S_FALSE;
	}
		
	return S_OK;
}


GUID* CXboxDeploymentTool::GetPageIDs()
{ 
	if (s_bPagesInit == FALSE)
	{
		s_pPages[0] = __uuidof(XboxDeploymentGeneral);
		s_bPagesInit = TRUE;
	}
	return s_pPages; 
}


///////////////////////////////////////////////////////////////////////////////////////
// CXboxDeploymentTool::CXboxDeploymentTool
///////////////////////////////////////////////////////////////////////////////////////

// general 
LPCOLESTR CXboxDeploymentTool::GetToolFriendlyName()
{
	InitToolName();
	return s_bstrToolName;
}

void CXboxDeploymentTool::InitToolName()
{
	if (s_bstrToolName.Length() == 0)
	{
		if (!s_bstrToolName.LoadString(IDS_XBOXDEPLOY_TOOLNAME))
			s_bstrToolName = szXboxDeployToolType;
	}
}

STDMETHODIMP CXboxDeploymentTool::get_ToolName(BSTR* pbstrToolName)
{	// friendly name of tool, e.g., "C/C++ Compiler Tool"
	CHECK_POINTER_VALID( pbstrToolName );
	InitToolName();
	s_bstrToolName.CopyTo(pbstrToolName);
	return S_OK;
}

STDMETHODIMP CXboxDeploymentTool::get_ExcludedFromBuild(VARIANT_BOOL* disableDeploy)
{	// exclude from build? default: yes
	return ToolGetBoolProperty(XBOXDPLYID_ExcludedFromBuild, disableDeploy);
}

STDMETHODIMP CXboxDeploymentTool::put_ExcludedFromBuild(VARIANT_BOOL disableDeploy)
{
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	CHECK_VARIANTBOOL(disableDeploy);
	return m_spPropertyContainer->SetBoolProperty(XBOXDPLYID_ExcludedFromBuild, disableDeploy);
}

STDMETHODIMP CXboxDeploymentTool::get_RemotePath(BSTR* dir)
{	// relative path to deploy to
	return ToolGetStrProperty(XBOXDPLYID_RemotePath, dir);
}

STDMETHODIMP CXboxDeploymentTool::put_RemotePath(BSTR dir)
{
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	return m_spPropertyContainer->SetStrProperty(XBOXDPLYID_RemotePath, dir);
}

STDMETHODIMP CXboxDeploymentTool::get_AdditionalFiles(BSTR* files)
{	// additional files to deploy
	return ToolGetStrProperty(XBOXDPLYID_AdditionalFiles, files);
}

STDMETHODIMP CXboxDeploymentTool::put_AdditionalFiles(BSTR files)
{
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	return m_spPropertyContainer->SetStrProperty(XBOXDPLYID_AdditionalFiles, files);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// CXboxDeploymentTool::IVCToolImpl
/////////////////////////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CXboxDeploymentTool::get_DefaultExtensions(BSTR* pVal)
{
	return DoGetDefaultExtensions(s_bstrExtensions, wszXboxDeployDefaultExtensions, pVal);
}

STDMETHODIMP CXboxDeploymentTool::put_DefaultExtensions(BSTR newVal)
{
	s_bstrExtensions = newVal;
	return S_OK;
}

STDMETHODIMP CXboxDeploymentTool::GetAdditionalOptionsInternal(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, 
	BSTR* pbstrAdditionalOptions)
{
	return S_FALSE;
}

STDMETHODIMP CXboxDeploymentTool::GetBuildDescription(IVCBuildAction* pAction, BSTR* pbstrBuildDescription)
{
	if (s_bstrBuildDescription.Length() == 0)
		s_bstrBuildDescription.LoadString(IDS_DESC_XBOXDEPLOYING);
	return s_bstrBuildDescription.CopyTo(pbstrBuildDescription);
}

STDMETHODIMP CXboxDeploymentTool::get_ToolPathInternal(BSTR* pbstrToolPath)
{ 
	return get_ToolPath(pbstrToolPath);
}

STDMETHODIMP CXboxDeploymentTool::get_Bucket(long *pVal)
{
	CHECK_POINTER_NULL( pVal );
	*pVal = BUCKET_XBOXDEPLOY;
	return S_OK;
}

STDMETHODIMP CXboxDeploymentTool::CreateToolObject(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
{ 
	return CreateInstance(pPropContainer, ppToolObject); 
}

STDMETHODIMP CXboxDeploymentTool::GetAdditionalIncludeDirectoriesInternal(IVCPropertyContainer* pPropContainer, BSTR* pbstrIncDirs)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXboxDeploymentTool::get_ToolShortName(BSTR* pbstrToolName)
{
	CHECK_POINTER_NULL(pbstrToolName);
	*pbstrToolName = SysAllocString( szXboxDeployToolShortName );
	return S_OK;
}

STDMETHODIMP CXboxDeploymentTool::MatchName(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches)
{
	return DoMatchName(bstrName, szXboxDeployToolType, szXboxDeployToolShortName, pbMatches);
}

HRESULT CXboxDeploymentTool::DoGetToolPath(BSTR* pbstrToolPath, IVCPropertyContainer* pPropContainer)
{
	CHECK_POINTER_NULL(pbstrToolPath);
	*pbstrToolPath = SysAllocString( szXboxDeployToolPath );
	return S_OK;
}

STDMETHODIMP CXboxDeploymentTool::get_ToolPath(BSTR* pbstrToolPath)
{
	return DoGetToolPath(pbstrToolPath, m_spPropertyContainer);
}

STDMETHODIMP CXboxDeploymentTool::get_PropertyOption(BSTR bstrProp, long dispidProp, BSTR *pVal)
{ 
	return E_NOTIMPL;
}

STDMETHODIMP CXboxDeploymentTool::IsTargetTool(IVCBuildAction* pAction, VARIANT_BOOL* pbTargetTool)
{ // tool always operates on target, not on file
	return COptionHandlerBase::GetValueTrue(pbTargetTool);
}

STDMETHODIMP CXboxDeploymentTool::get_IsFileTool(VARIANT_BOOL* pbIsFileTool)
{
	return COptionHandlerBase::GetValueFalse(pbIsFileTool);
}

static HRESULT ConvertExeToXbe(CComBSTR& b)
{
	// Replace .exe with .xbe
	int nLength = b.Length();
	if(nLength >= 4){
		LPWSTR p =  b.m_str + nLength - 4;
		if(p[0] == '.'
			&& (p[1] == 'e' || p[1] == 'E')
			&& (p[2] == 'x' || p[2] == 'X')
			&& (p[3] == 'e' || p[3] == 'E'))
		{
			p[1] = 'x';
			p[2] = 'b';
			p[3] = 'e';
			return S_OK;
		}
	}
	return E_FAIL;
}

STDMETHODIMP CXboxDeploymentTool::GetCommandLineEx(IVCBuildAction* pAction, IVCBuildableItem* pBuildableItem, 
	IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	*pVal = NULL;
	CHECK_READ_POINTER_NULL(pBuildableItem);
	CHECK_READ_POINTER_NULL(pBuildEngine);

	CComQIPtr<IVCPropertyContainer> spPropContainer = pBuildableItem;
	CHECK_ZOMBIE(spPropContainer, IDS_ERR_TOOL_ZOMBIE);

#ifdef USE_XML

	// NONE of this stuff is localizable which is why it is inline here...
	CComBSTR bstrCmdLine = L"<?xml version=\"1.0\" encoding=\"";
	CComBSTR bstrTmp, bstrTmp2;
	CXMLFile::DetermineDefaultFileEncoding(bstrTmp);
	bstrCmdLine += bstrTmp;

#if 0
	bstrCmdLine += L"\"?>\n<ATLSINSTSETTINGS>\n\t<WEBHOSTNAME>localhost</WEBHOSTNAME>\n";
#endif

#if 1 // No Virtual Directory name
	HRESULT hr = S_OK;
	int nLen;
#else

	// figure out the virtual root and the IIS root directory
	bstrTmp.Empty();
	CComBSTR bstrVirtDir;
	HRESULT hr = spPropContainer->GetStrProperty(XBOXDPLYID_VirtualDirectoryName, &bstrTmp);
	if (hr == S_FALSE)
		s_optHandler.GetDefaultValue(XBOXDPLYID_VirtualDirectoryName, &bstrTmp);
	hr = spPropContainer->Evaluate(bstrTmp, &bstrVirtDir);
	RETURN_ON_FAIL(hr);
	if (bstrVirtDir.Length() == 0)	// cannot have a blank virtual root...
	{
		bstrTmp.Empty();
		bstrVirtDir.Empty();
		s_optHandler.GetDefaultValue(XBOXDPLYID_VirtualDirectoryName, &bstrTmp);
		hr = spPropContainer->Evaluate(bstrTmp, &bstrVirtDir);
		RETURN_ON_FAIL(hr);
	}

	bstrCmdLine += L"\t<VIRTDIRNAME>";
	if (bstrVirtDir.Length())
		bstrCmdLine += bstrVirtDir;
	bstrCmdLine += L"</VIRTDIRNAME>\n";

	bstrTmp.Empty();
	hr = CVCProjectEngine::GetPathWWWRoot(bstrTmp);
//	RETURN_ON_FAIL(hr);		// Don't return on fail here.  Let VCDeploy have the chance to tell the user there 
							// is a problem with their install.
	BOOL bNotAdmin = bstrTmp.Length() && wcscmp(bstrTmp, L"!admin") == 0;
	int nLen = bstrTmp.Length();
	if (bNotAdmin)
	{}	// don't do anything
	else if (nLen <= 0)
		bstrTmp = L"";
	else if (bstrTmp[nLen-1] != L'/' && bstrTmp[nLen-1] != L'\\')
		bstrTmp += L"\\";

	bstrCmdLine += L"\t<VIRTDIRFSPATH>";
	bstrCmdLine += bstrTmp;
	if (!bNotAdmin)
		bstrCmdLine += bstrVirtDir;
	bstrCmdLine += L"</VIRTDIRFSPATH>\n";

	VARIANT_BOOL bReg = VARIANT_FALSE;
	if (spPropContainer->GetBoolProperty(XBOXDPLYID_RegisterOutput, &bReg) == S_OK && bReg)
		bstrCmdLine += L"\t<REGISTERISAPI>true</REGISTERISAPI>\n";

	VARIANT_BOOL bUnload = VARIANT_FALSE;
	if (spPropContainer->GetBoolProperty(XBOXDPLYID_UnloadBeforeCopy, &bUnload) == S_OK && bUnload)
		bstrCmdLine += L"\t<UNLOADBEFORECOPY>true</UNLOADBEFORECOPY>\n";

	long nProtect = 0;
	hr = spPropContainer->GetIntProperty(XBOXDPLYID_ApplicationProtection, &nProtect);
	if (hr == S_FALSE)
		s_optHandler.GetDefaultValue(XBOXDPLYID_ApplicationProtection, &nProtect, spPropContainer);
	if (SUCCEEDED(hr))
	{
		CStringW strTmp;
		strTmp.Format(L"\t<APPISOLATION>%d</APPISOLATION>\n", nProtect);
		bstrTmp = strTmp;
		bstrCmdLine += bstrTmp;
	}

	bstrTmp.Empty();
	bstrTmp2.Empty();
	BOOL bHaveMappings = FALSE;
	if (spPropContainer->GetStrProperty(XBOXDPLYID_ApplicationMappings, &bstrTmp) == S_OK && bstrTmp.Length() > 0)
	{
		hr = spPropContainer->Evaluate(bstrTmp, &bstrTmp2);
		RETURN_ON_FAIL(hr);
		CStringW strMapList = bstrTmp2;
		int nMaxIdx = strMapList.GetLength();
		int nStartMap = 0;
		while (nStartMap >= 0)
		{
			CComBSTR bstrMapping;
			nStartMap = CStyleSheetBase::GetNextStyle(strMapList, nStartMap, nMaxIdx, bstrMapping);
			if (bstrMapping.Length() > 0)
			{
				bstrCmdLine += L"\n\t<APPMAPPING fileext=\"";
				bstrCmdLine += bstrMapping;
				bstrCmdLine += L"\">\n\t\t<VERB>GET</VERB>\n\t\t<VERB>HEAD</VERB>\n\t\t<VERB>POST</VERB>\n\t\t<VERB>DEBUG</VERB>\n\t</APPMAPPING>\n";
				bHaveMappings = TRUE;
			}
		}
	}
#endif

	
	CComBSTR bstrProjDir;
	spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir);
	CStringW strProjDir = bstrProjDir;
	int nProjDirLen = strProjDir.GetLength();
	if (strProjDir.IsEmpty())
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	CDirW dirProj;
	dirProj.CreateFromKnown(strProjDir);

	// now, let's get the files we're going to be transferring
	bstrCmdLine += L"\n\t<APPFILEGROUP>\n";

	CVCMapStringWToPtr mapFiles;	// this will cover any files we already know about so we don't add them twice...

	// first, let's pass through any additional files we may have been given
	bstrTmp.Empty();
	if (spPropContainer->GetStrProperty(XBOXDPLYID_AdditionalFiles, &bstrTmp) == S_OK && bstrTmp.Length() > 0)
	{
		bstrTmp2.Empty();
		hr = spPropContainer->Evaluate(bstrTmp, &bstrTmp2);
		RETURN_ON_FAIL(hr);

		CStringW strFileList = bstrTmp2;
		int nMaxIdx = strFileList.GetLength();
		int nStartFiles = 0;
		while (nStartFiles >= 0)
		{
			CComBSTR bstrFile;
			nStartFiles = CStyleSheetBase::GetNextStyle(strFileList, nStartFiles, nMaxIdx, bstrFile);
			CStringW strFile = bstrFile;
			if (!strFile.IsEmpty())
			{
				CPathW pathFile;
				if (!pathFile.CreateFromDirAndFilename(dirProj, strFile))
				{
					CComBSTR bstrErr;
					bstrErr.LoadString(IDS_ERR_DEPLOY_ADDL_FILE_INVALID_PRJ0036);
					CVCProjectEngine::AddProjectError(pEC, bstrErr, L"PRJ0036", spPropContainer);
					return E_FAIL;
				}
				CStringW strFileLower = pathFile.GetFullPath();
				strFileLower.MakeLower();
				void* pDummy;
				if (!mapFiles.Lookup(strFileLower, pDummy))
				{
					bstrCmdLine += L"\t\t<APPFILENAME>\n\t\t\t<SRC>";
					strFile = pathFile.GetFullPath();
					bstrTmp = strFile;
					bstrCmdLine += bstrTmp;
					bstrCmdLine += L"</SRC>\n\t\t\t<DEST>";
					if (_wcsnicmp(strProjDir, strFile, nProjDirLen) == 0)
						pathFile.GetRelativeName(dirProj, strFile);
					else
						strFile = pathFile.GetFileName();
					bstrFile = strFile;
					bstrCmdLine += bstrFile;
					bstrCmdLine += L"</DEST>\n\t\t</APPFILENAME>\n";
					mapFiles.SetAt(strFileLower, pDummy);
				}
			}
		}
	}

	// now, add any deployment content
	CComQIPtr<VCConfiguration> spProjCfg = spPropContainer;
	if (spProjCfg == NULL)
	{
		CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = spPropContainer;
		RETURN_ON_NULL(spFileCfgImpl);
		CComPtr<IDispatch> spDispProjCfg;
		spFileCfgImpl->get_Configuration(&spDispProjCfg);
		spProjCfg = spDispProjCfg;
		RETURN_ON_NULL(spProjCfg);
	}
	CComPtr<IDispatch> spDispProj;
	spProjCfg->get_Project(&spDispProj);
	CComQIPtr<VCProject> spProj = spDispProj;
	RETURN_ON_NULL(spProj);
	CComPtr<IDispatch> spDispFileColl;
	spProj->get_Files(&spDispFileColl);
	CComQIPtr<IVCCollection> spFileColl = spDispFileColl;
	RETURN_ON_NULL(spFileColl);
	CComPtr<IEnumVARIANT> spFiles;
	spFileColl->_NewEnum(reinterpret_cast<IUnknown **>(&spFiles));
	RETURN_ON_NULL(spFiles);
	spFiles->Reset();
	while (TRUE)
	{
		CComVariant var;
		hr = spFiles->Next(1, &var, NULL);
		if (hr != S_OK)
			break;
		if (var.vt != VT_UNKNOWN && var.vt != VT_DISPATCH)
			continue;
		CComQIPtr<VCFile> spFile = var.punkVal;
		if (spFile == NULL)
			continue;
		VARIANT_BOOL bDeploy;
		if (spFile->get_DeploymentContent(&bDeploy) == S_OK && bDeploy)
		{
			CComBSTR bstrFullPath, bstrRemPath;
			spFile->get_FullPath(&bstrFullPath);
			CStringW strFullPathLower = bstrFullPath;
			strFullPathLower.MakeLower();
			void* pDummy;
			if (!mapFiles.Lookup(strFullPathLower, pDummy))
			{
				bstrCmdLine += L"\t\t<APPFILENAME>\n\t\t\t<SRC>";
				bstrCmdLine += bstrFullPath;
				bstrCmdLine += L"</SRC>\n\t\t\t<DEST>";
				if (_wcsnicmp(strProjDir, bstrFullPath, nProjDirLen) == 0)
				{
					spFile->get_RemotePath(&bstrRemPath);
					bstrTmp2.Empty();
					spPropContainer->Evaluate(bstrRemPath, &bstrTmp2);
				}
				else
					spFile->get_Name(&bstrTmp2);
				bstrCmdLine += bstrTmp2;
				bstrCmdLine += L"</DEST>\n\t\t</APPFILENAME>\n";
				mapFiles.SetAt(strFullPathLower, pDummy);
			}
		}
	}
	RETURN_ON_FAIL(hr);

	CComBSTR bstrSubDir;
	hr = spPropContainer->GetStrProperty(XBOXDPLYID_RemotePath, &bstrSubDir);
	RETURN_ON_FAIL(hr);
	if (hr == S_FALSE)
		s_optHandler.GetDefaultValue(XBOXDPLYID_RemotePath, &bstrSubDir, spPropContainer);
	CStringW strSubDir = bstrSubDir;
	nLen = strSubDir.GetLength();
	if (nLen > 0)
	{
		if (nLen == 1 && strSubDir[0] == L'.')
		{	// this is a no-op directory...
			strSubDir.Empty();
			nLen = 0;
		}
		if (nLen > 0 && strSubDir[nLen-1] != L'/' && strSubDir[nLen-1] != L'\\')
			strSubDir += L"\\";		// need a trailing slash...
	}
	if (strSubDir.IsEmpty())
		bstrSubDir.Empty();
	else
		bstrSubDir = strSubDir;

	//  add the primary output
	bstrCmdLine += L"\t\t<APPFILENAME";
#if 0
	if (bHaveMappings)
		bstrCmdLine += L" type=\"extension\"";
#endif
	bstrCmdLine += L">\n\t\t\t<SRC>";
	bstrTmp = L"$(TargetPath)";
	bstrTmp2.Empty();
	hr = spPropContainer->Evaluate(bstrTmp, &bstrTmp2);
	RETURN_ON_FAIL(hr);
	bstrCmdLine += bstrTmp2;
	bstrCmdLine += L"</SRC>\n\t\t\t<DEST>";
	if (bstrSubDir)
		bstrCmdLine += bstrSubDir;
	bstrTmp = L"$(TargetFileName)";
	bstrTmp2.Empty();
	hr = spPropContainer->Evaluate(bstrTmp, &bstrTmp2);
	RETURN_ON_FAIL(hr);
	bstrCmdLine += bstrTmp2;
	bstrCmdLine += L"</DEST>\n\t\t</APPFILENAME>\n";
	// if this is a managed C++ project, then we have to copy the PDB file as well
	VARIANT_BOOL bGenDebug = VARIANT_FALSE;
	if (spPropContainer->GetBoolProperty(VCLINKID_GenerateDebugInformation, &bGenDebug) == S_OK &&
		bGenDebug)	// yep, have debug info
	{
		VARIANT_BOOL bManaged = VARIANT_FALSE;
		compileAsManagedOptions managedOption = managedNotSet;
		HRESULT hr1 = spPropContainer->GetBoolProperty(VCCFGID_ManagedExtensions, &bManaged);
		HRESULT hr2 = spPropContainer->GetIntProperty(VCCLID_CompileAsManaged, (long *)&managedOption);
		if ((hr2 == S_OK && managedOption == managedAssembly) || (hr1 == S_OK && bManaged))
		{	// yep, managed
			CComBSTR bstrPDBName;
			if (spPropContainer->GetStrProperty(VCLINKID_ProgramDatabaseFile, &bstrPDBName) == S_FALSE)
				CVCLinkerTool::s_optHandler.GetDefaultValue(VCLINKID_ProgramDatabaseFile, &bstrPDBName, spPropContainer);
			if (bstrPDBName.Length() > 0)
			{
				bstrTmp = bstrPDBName.Detach();
				hr = spPropContainer->Evaluate(bstrTmp, &bstrPDBName);
				RETURN_ON_FAIL(hr);
				CPathW pathPDB;
				if (pathPDB.CreateFromDirAndFilename(dirProj, bstrPDBName))
				{
					bstrPDBName = pathPDB.GetFullPath();
					bstrCmdLine += L"\t\t<APPFILENAME>\n\t\t\t<SRC>";
					bstrCmdLine += bstrPDBName;
					bstrCmdLine += L"</SRC>\n\t\t\t<DEST>";
					if (bstrSubDir)
						bstrCmdLine += bstrSubDir;
					bstrPDBName = pathPDB.GetFileName();
					bstrCmdLine += bstrPDBName;
					bstrCmdLine += L"</DEST>\n\t\t</APPFILENAME>\n";
				}
			}
		}
	}
 
/*	Take out this whole bit on deployment outputs.  This code can be removed later (long after 6/15/01)
	if we decide to keep it this way.
	// add any deployment outputs
	CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = spProjCfg;
	if (spProjCfgImpl)
	{
		CComPtr<IVCBuildStringCollection> spDeployDeps;
		if (SUCCEEDED(spProjCfgImpl->get_KnownDeploymentDependencies(&spDeployDeps)) && spDeployDeps)
		{
			CStringW strKey = L"SOFTWARE\\Microsoft\\.NETFramework";
			CStringW strComDir;
			CStringW strComSDKDir;
			DWORD nType = REG_SZ;
			DWORD nSize = MAX_PATH;
			HKEY hSectionKey = NULL;
			LONG lRet = RegOpenKeyExW( HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hSectionKey );
			if( hSectionKey )
			{
				wchar_t szComplusDir[MAX_PATH+1] = {0};
				wchar_t szComplusSDKDir[MAX_PATH+1] = {0};
				lRet = RegQueryValueExW( hSectionKey, L"InstallRoot", NULL, &nType, (LPBYTE)szComplusDir, &nSize );
				lRet = RegQueryValueExW( hSectionKey, L"sdkInstallRoot", NULL, &nType, (LPBYTE)szComplusSDKDir, &nSize );
				strComDir = szComplusDir;
				strComSDKDir = szComplusSDKDir;
				int nLen1 = strComDir.GetLength();
				int nLen2 = strComSDKDir.GetLength();
				int nLenMin = nLen1 > nLen2 ? nLen2 : nLen1;
				if (nLenMin > 0 && _wcsnicmp(strComDir, strComSDKDir, nLenMin) == 0)
				{	// same root, so need to have to check twice lower down.
					if (nLen1 == nLenMin)
						strComSDKDir.Empty();	// this is the longer one, so toss it
					else
						strComDir.Empty();	// longer one, so toss it
				}
				RegCloseKey( hSectionKey );
			}
			while (TRUE)
			{
				CComBSTR bstrDep;
				hr = spDeployDeps->Next(&bstrDep);
				if (hr != S_OK)
					break;
				if (!bstrDep || bstrDep[0] == '\0')
					continue;
				CPathW pathDep;
				if (!pathDep.Create(bstrDep))
					continue;
				if (!strComDir.IsEmpty() && _wcsnicmp(strComDir, bstrDep, strComDir.GetLength()) == 0)
					continue;	// don't want to deal with COM+ stuff here
				if (!strComSDKDir.IsEmpty() && _wcsnicmp(strComSDKDir, bstrDep, strComSDKDir.GetLength()) == 0)
					continue;	// don't want to deal with COM+ SDK stuff here, either
				bstrCmdLine += L"\t\t<APPFILENAME>\n\t\t\t<SRC>";
				bstrCmdLine += bstrDep;
				bstrDep = pathDep.GetFileName();
				bstrCmdLine += L"</SRC>\n\t\t\t<DEST>";
				if (bstrSubDir)
					bstrCmdLine += bstrSubDir;
				bstrCmdLine += bstrDep;
				bstrCmdLine += L"</DEST>\n\t\t</APPFILENAME>\n";
			}
		}
	}
*/

	// finally, close it all off
	bstrCmdLine += L"\t</APPFILEGROUP>\n</ATLSINSTSETTINGS>\n";

	return pBuildEngine->FormXMLCommand(bstrCmdLine, szXboxDeployToolPath, L"/nologo", pVal);

#else // USE_XML
	HRESULT hr = S_OK;

	CComBSTR bstrCmdLine;
	bstrCmdLine.Append("xbecopy");
	bstrCmdLine.Append(" /NOLOGO");
	bstrCmdLine.Append(" \"");

	{
		CComBSTR bstrSubDir;
		hr = spPropContainer->GetStrProperty(XBOXDPLYID_RemotePath, &bstrSubDir);
		RETURN_ON_FAIL(hr);
		if (hr == S_FALSE)
			s_optHandler.GetDefaultValue(XBOXDPLYID_RemotePath, &bstrSubDir, spPropContainer);
		CComBSTR bstrSubDir2;
		hr = spPropContainer->Evaluate(bstrSubDir, &bstrSubDir2);
		RETURN_ON_FAIL(hr);
		CStringW strSubDir = bstrSubDir2;
		int nLen = strSubDir.GetLength();
		if (nLen > 0)
		{
			if (nLen == 1 && strSubDir[0] == L'.')
			{	// this is a no-op directory...
				strSubDir.Empty();
				nLen = 0;
			}
			if (nLen > 0 && strSubDir[nLen-1] != L'/' && strSubDir[nLen-1] != L'\\')
				strSubDir += L"\\";		// need a trailing slash...
		}
		if (strSubDir.IsEmpty())
			bstrSubDir.Empty();
		else
			bstrSubDir = strSubDir;

		//  add the primary output
		CComBSTR bstrTmp;
		CComBSTR bstrTmp2;
		bstrTmp = L"$(TargetPath)";
		bstrTmp2.Empty();
		hr = spPropContainer->Evaluate(bstrTmp, &bstrTmp2);
		RETURN_ON_FAIL(hr);
		hr = ConvertExeToXbe(bstrTmp2);
		RETURN_ON_FAIL(hr);
		bstrCmdLine += bstrTmp2;
		bstrCmdLine += L"\"";
		
		bstrCmdLine += L" ";
		bstrCmdLine += L"\"";
		if (bstrSubDir)
			bstrCmdLine += bstrSubDir;
		bstrTmp = L"$(TargetFileName)";
		bstrTmp2.Empty();
		hr = spPropContainer->Evaluate(bstrTmp, &bstrTmp2);
		RETURN_ON_FAIL(hr);

		hr = ConvertExeToXbe(bstrTmp2);
		RETURN_ON_FAIL(hr);

		bstrCmdLine += bstrTmp2;
		bstrCmdLine += L"\"";
	}

	CComBSTR bstrFullCommandLine;
	BuildCommandLineBatchFile(pAction, bstrCmdLine, bstrFullCommandLine);

	return pBuildEngine->FormBatchFile(bstrFullCommandLine, pEC, pVal);

#endif // USE_XML

}

// IVCBuildEventToolInternal
///////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CXboxDeploymentTool::get_CommandLineInternal(BSTR *pVal)
{
	HRESULT hr = GetCommandLineOptions(m_spPropertyContainer, NULL, VARIANT_FALSE /* !additional */, cmdLineForRefresh, pVal);
	if(SUCCEEDED(hr))
	{
		if(pVal && SysStringLen(*pVal) == 0)
		{
			// We have only the default options list, so the build system thinks we have nothing to do.
			SysFreeString(*pVal);
			CComBSTR dummy;
			dummy.Append(" ");
			*pVal = dummy.Detach();
		}
	}
	return hr;
}

STDMETHODIMP CXboxDeploymentTool::put_CommandLineInternal(BSTR newVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXboxDeploymentTool::get_DescriptionInternal(BSTR *pVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXboxDeploymentTool::put_DescriptionInternal(BSTR newVal)
{
	return E_NOTIMPL;
}

STDMETHODIMP CXboxDeploymentTool::get_ExcludedFromBuildInternal(VARIANT_BOOL* pbExcludedFromBuild)
{
	return get_ExcludedFromBuild(pbExcludedFromBuild);
}

STDMETHODIMP CXboxDeploymentTool::put_ExcludedFromBuildInternal(VARIANT_BOOL bExcludedFromBuild)
{
	return E_NOTIMPL;
}

// CXboxDeploymentGeneralPage
///////////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CXboxDeploymentGeneralPage::MapPropertyToCategory(DISPID dispid, PROPCAT* ppropcat)
{
	CHECK_POINTER_NULL(ppropcat);

	switch (dispid)
	{
	case XBOXDPLYID_ExcludedFromBuild:
	case XBOXDPLYID_AdditionalFiles:
	case XBOXDPLYID_RemotePath:
		*ppropcat = IDS_GENERALCAT;
		break;
	default:
		*ppropcat = 0;
		break;
	}

	return S_OK;
}

STDMETHODIMP CXboxDeploymentGeneralPage::GetCategoryName(PROPCAT propcat, LCID lcid, BSTR* pbstrName)
{
	CComBSTR bstrCat;
	bstrCat.LoadString(propcat);
	*pbstrName = bstrCat.Detach();
	return S_OK;
}

// file copy
STDMETHODIMP CXboxDeploymentGeneralPage::get_AdditionalFiles(BSTR* files)
{	// additional files to deploy
	return GetStrProperty(XBOXDPLYID_AdditionalFiles, files);
}

STDMETHODIMP CXboxDeploymentGeneralPage::put_AdditionalFiles(BSTR files)
{
	return SetStrProperty(XBOXDPLYID_AdditionalFiles, files);
}

STDMETHODIMP CXboxDeploymentGeneralPage::get_RemotePath(BSTR* dir)
{	// relative path to deploy to
	return ToolGetStrProperty(XBOXDPLYID_RemotePath, &(CXboxDeploymentTool::s_optHandler), dir);
}

STDMETHODIMP CXboxDeploymentGeneralPage::put_RemotePath(BSTR dir)
{
	return SetStrProperty(XBOXDPLYID_RemotePath, dir);
}

// general 
STDMETHODIMP CXboxDeploymentGeneralPage::get_ExcludedFromBuild(VARIANT_BOOL* disableDeploy)
{	// exclude from build? default: no
	return ToolGetBoolProperty(XBOXDPLYID_ExcludedFromBuild, &(CXboxDeploymentTool::s_optHandler), disableDeploy);
}

STDMETHODIMP CXboxDeploymentGeneralPage::put_ExcludedFromBuild(VARIANT_BOOL disableDeploy)
{
	return SetBoolProperty(XBOXDPLYID_ExcludedFromBuild, disableDeploy);
}
