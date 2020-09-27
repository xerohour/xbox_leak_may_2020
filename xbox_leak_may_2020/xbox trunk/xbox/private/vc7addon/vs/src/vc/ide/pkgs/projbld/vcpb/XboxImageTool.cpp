// XboxImageTool.cpp : Implementation of Xbox Image Tool

#include "stdafx.h"
#include "XboxImageTool.h"
#include "FileRegistry.h"
#include "BldHelpers.h"
#include "BldActions.h"
#include "stylesheet.h"
#include "ProjWriter.h"

// constants
const wchar_t* const wszXboxImageDefaultExtensions = L"*.exe;*.pdb";	// remember to update Xbox.rgs if you change this...
const wchar_t* wszXboxImageDelOnRebuildExtensions = L"xbe";

const long lnTypeLibraryResourceID = 1;
const subSystemOption lSubSystem = subSystemNotSet;
const long lnHeapReserveSize = 0;
const long lnHeapCommitSize = 0;
const long lnStackReserveSize = 0;
const long lnStackCommitSize = 0;
const machineTypeOption lTargetMachine = machineNotSet;

// static initializers
CXboxImageToolOptionHandler CXboxImageTool::s_optHandler;
CComBSTR CXboxImageTool::s_bstrBuildDescription = L"";
CComBSTR CXboxImageTool::s_bstrToolName = L"";
CComBSTR CXboxImageTool::s_bstrExtensions = L"";
GUID CXboxImageTool::s_pPages[XBOXIMAGETOOL_NUMPAGES];
BOOL CXboxImageTool::s_bPagesInit;


////////////////////////////////////////////////////////////////////////////////
// Option Table for the Xbox Image Tool switches

// NOTE: we're using macros for enum value ranges to help in keeping enum range checks as low maintenance as possible.
// If you add an enum to this table, make sure you use macros to define the upper and lower bounds and use CHECK_BOUNDS
// on the get/put methods associated with that property (both object model and property page).
// WARNING: if you change ANYTHING about a multi-prop here (including add/remove), be sure to update how the property
// is obtained on both the tool and page objects.
BEGIN_OPTION_TABLE(CXboxImageToolOptionHandler, L"XboxImageTool", IDS_XBOXIMAGETOOL, TRUE /* pre & post */, FALSE /* case sensitive */)

	// general
	OPT_BSTR(FileName,			L"OUT:%s",						L"FileName", single,				IDS_XboxImageGeneral, XBOXIMAGEID)
	OPT_INT(XBEFlags,			L"initflags:0x%x",				L"XBEFlags",						IDS_XboxImageGeneral, XBOXIMAGEID)
	OPT_INT(StackSize,			L"stack:0x%x",					L"StackSize",						IDS_XboxImageGeneral, XBOXIMAGEID)
	OPT_BOOL(IncludeDebugInfo,	L"|debug",						L"IncludeDebugInfo",				IDS_XboxImageGeneral, XBOXIMAGEID)
	OPT_BOOL(LimitAvailableMemoryTo64MB,	L"|limitmem",		L"LimitAvailableMemoryTo64MB",		IDS_XboxImageGeneral, XBOXIMAGEID)

	// certificate
	OPT_INT(TitleID,			L"testid:0x%x",					L"TitleID",							IDS_XboxImageCertificate, XBOXIMAGEID)
	OPT_INT(XboxGameRegion,		L"testregion:0x%x",				L"XboxGameRegion",					IDS_XboxImageCertificate, XBOXIMAGEID)
	OPT_ENUM(XboxGameRating,	L"|testratings:%d", XboxGameRatingMin, XboxGameRatingMax, L"XboxGameRating", IDS_XboxImageCertificate, XBOXIMAGEID)
	
	// Title info
	OPT_BSTR(TitleInfo, L"|titleinfo:%s",						L"TitleInfo",		single,			IDS_XboxImageTitleInfo, XBOXIMAGEID)
	OPT_BSTR(TitleName, L"|testname:%s",						L"TitleName",		single,			IDS_XboxImageTitleInfo, XBOXIMAGEID)
	OPT_BSTR(TitleImage, L"|titleimage:%s",						L"TitleImage",		single,			IDS_XboxImageTitleInfo, XBOXIMAGEID)
	OPT_BSTR(SaveGameImage, L"|defaultsaveimage:%s",			L"SaveGameImage",	single,			IDS_XboxImageTitleInfo, XBOXIMAGEID)

END_OPTION_TABLE()

// default value handlers
// string props
void CXboxImageToolOptionHandler::GetDefaultValue( long id, BSTR *pVal, IVCPropertyContainer *pPropContainer )
{
	switch( id )
	{
	case XBOXIMAGEID_FileName:
	{
		if (pPropContainer == NULL)
			GetDefaultString( pVal );
		else
		{
			*pVal = NULL;
			CComBSTR bstrFile = L"$(OutDir)/$(ProjectName).xbe";
			*pVal = bstrFile.Detach();
		}
		break;
	}

	default:
		GetDefaultString( pVal );
		break;
	}
}

// integer props
void CXboxImageToolOptionHandler::GetDefaultValue( long id, long *pVal, IVCPropertyContainer *pPropCnt )
{
//	switch( id )
//	{
//	default:
		*pVal = 0;
//		break;
//	}
}

// boolean props
void CXboxImageToolOptionHandler::GetDefaultValue( long id, VARIANT_BOOL *pVal, IVCPropertyContainer *pPropCnt )
{
// 	switch( id )
// 	{
// 	default:
		GetValueFalse( pVal );
// 		break;
// 	}
}

BOOL CXboxImageToolOptionHandler::SynthesizeOptionIfNeeded(IVCPropertyContainer* pPropContainer, long idOption)
{
	switch (idOption)
	{
	case XBOXIMAGEID_FileName:
		return TRUE;
	default:
		return FALSE;
	}
}

BOOL CXboxImageToolOptionHandler::SynthesizeOption(IVCPropertyContainer* pPropContainer, long idOption, 
	CComVariant& rvar)
{
	CComBSTR bstrVal;
	switch (idOption)
	{
	case XBOXIMAGEID_FileName:
		CreateFileName(pPropContainer, &bstrVal, FALSE);
		break;
	default:
		VSASSERT(FALSE, "Case statement mismatch between SynthesizeOptionIfNeeded and SynthesizeOption");	// shouldn't be here!!
		return FALSE;
	}

	rvar = bstrVal;
	return TRUE;
}

static const wchar_t* szScriptItemCL = L"\r\n<a title=\"%s | %s\" onclick=\"onHelp('VC.Project.VCConfiguration.%s')\" href=\"settingspage.htm\">%s</a>";

BOOL CXboxImageToolOptionHandler::CreateFileName(IVCPropertyContainer* pPropContainer, BSTR *pbstrVal,
	BOOL bCheckForExistence /* = TRUE */)
{
	VSASSERT(pPropContainer != NULL, "Cannot create output name without a property container");
	RETURN_ON_NULL2(pPropContainer, FALSE);

	if (bCheckForExistence &&
		pPropContainer->GetEvaluatedStrProperty(XBOXIMAGEID_FileName, pbstrVal) == S_OK)	// found it directly set, so use it
		return TRUE;

	CXboxImageTool::s_optHandler.GetDefaultValue( XBOXIMAGEID_FileName, pbstrVal, pPropContainer );
	HRESULT hr = pPropContainer->Evaluate(*pbstrVal, pbstrVal);

	return (SUCCEEDED(hr));
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CXboxImageToolHelper::DoGenerateOutput(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC, 
	IVCToolImpl* pTool)
{
	RETURN_ON_NULL2(plstActions, S_OK);	// nothing to do

	CComPtr<VCProject> spProject;
	CComBSTR bstrProjectDirectory;
	BOOL bFirst = TRUE;
	plstActions->Reset(NULL);
	while (TRUE)
	{
		CComPtr<IVCBuildAction> spAction;
		HRESULT hr = plstActions->Next(&spAction, NULL);
		BREAK_ON_DONE(hr);

		CComPtr<IVCBuildableItem> spItem;
		spAction->get_Item(&spItem);
		CComQIPtr<IVCPropertyContainer> spPropContainer = spItem;
		VSASSERT(spPropContainer != NULL, "Cannot generate output without a property container");

		if (bFirst)
		{
			bFirst = TRUE;
			if (spPropContainer != NULL)
				spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjectDirectory);	// should never have macros in it
		}

		// Remember that for a newly created project, the file path is supposed to be NULL:
		CPathW pathName;
		CStringW strName;
		
		// do we want the primary output?
		if (type & AOGO_Primary)
		{
			VARIANT_BOOL bHasPrimaryOutput = VARIANT_FALSE;
			hr = DoHasPrimaryOutputFromTool(spItem, VARIANT_TRUE, &bHasPrimaryOutput);
			RETURN_ON_FAIL(hr);
			if (bHasPrimaryOutput == VARIANT_FALSE)
				continue;	// nothing more to do

			HRESULT hrT = DoGetPrimaryOutputFromTool(spItem, pathName);
			RETURN_ON_FAIL(hrT);

			pathName.GetActualCase(TRUE);
			hrT = spAction->AddOutputFromPath((wchar_t *)(const wchar_t *)pathName, pEC, VARIANT_TRUE, VARIANT_TRUE,
				XBOXIMAGEID_FileName, pTool);
			if (hrT != S_OK)
				return hrT; 	// failure
		}

	}

	return S_OK; // success
}

HRESULT CXboxImageToolHelper::DoGetPrimaryOutputFromTool(IUnknown* pItem, CPathW& rpath)
{
	CComPtr<VCConfiguration> spProjCfg;
	GetProjectCfgForItem(pItem, &spProjCfg);
	VSASSERT(spProjCfg != NULL, "Xbox Image only works on project config, not file config");
	RETURN_ON_NULL2(spProjCfg, E_UNEXPECTED);

	VARIANT_BOOL bHasPrimaryOutput = VARIANT_FALSE;
	if (FAILED(DoHasPrimaryOutputFromTool(spProjCfg, VARIANT_TRUE, &bHasPrimaryOutput)) || bHasPrimaryOutput == VARIANT_FALSE)
		return S_FALSE;

	CComQIPtr<IVCPropertyContainer> spPropContainer = spProjCfg;
	RETURN_INVALID_ON_NULL(spPropContainer);

	CComBSTR bstrProjName;
	HRESULT hrT = spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjName);	// should never have macros in it
	RETURN_ON_FAIL(hrT);
	CStringW strDirName = bstrProjName;
	CDirW dirProj;
	if (!dirProj.CreateFromKnown(strDirName))
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	CStringW strName;
	CComBSTR bstrName;
	if (spPropContainer->GetEvaluatedStrProperty(XBOXIMAGEID_FileName, &bstrName) == S_OK)
	{
		strName = bstrName;
		if (!strName.IsEmpty())
		{
			if( !rpath.CreateFromDirAndFilename(dirProj, strName))
				return CVCProjectEngine::DoSetErrorInfo3(VCPROJ_E_BAD_PATH, IDS_ERR_CANNOT_CREATE_PATH, strDirName, strName);
			rpath.GetActualCase(TRUE);

			return S_OK;
		}
	}

	// if we get here, either the non-default is blank or there isn't one
	CreateFileName(spPropContainer, &bstrName);
	strName = bstrName;

	if (!rpath.CreateFromDirAndFilename(dirProj, strName))
		return CVCProjectEngine::DoSetErrorInfo3(VCPROJ_E_BAD_PATH, IDS_ERR_CANNOT_CREATE_PATH, strDirName, strName);
	rpath.GetActualCase(TRUE);

	return S_OK;
}

void CXboxImageToolHelper::GetProjectCfgForItem(IUnknown* pItem, VCConfiguration** pProjCfg)
{
	if (pProjCfg == NULL)
		return;
	*pProjCfg = NULL;

	CComQIPtr<VCConfiguration> spProjCfg = pItem;
	if (spProjCfg != NULL)
	{
		*pProjCfg = spProjCfg.Detach();
		return;
	}

	CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = pItem;
	if (spFileCfgImpl == NULL)
		return;

	CComPtr<IDispatch> spDispProjCfg;
	if (FAILED(spFileCfgImpl->get_Configuration(&spDispProjCfg)) || spDispProjCfg == NULL)
		return;

	spProjCfg = spDispProjCfg;
	*pProjCfg = spProjCfg.Detach();
}

HRESULT CXboxImageToolHelper::DoHasDependencies(IVCBuildAction* pAction, VARIANT_BOOL* pbHasDependencies)
{
	CHECK_POINTER_NULL(pbHasDependencies);
	*pbHasDependencies = VARIANT_FALSE;
	RETURN_ON_NULL2(pAction, S_FALSE);

	CComPtr<IVCBuildableItem> spItem;
	pAction->get_Item(&spItem);

	CComPtr<VCConfiguration> spProjCfg;
	GetProjectCfgForItem(spItem, &spProjCfg);
	CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = spProjCfg;
	RETURN_ON_NULL2(spProjCfgImpl, S_FALSE);

	return spProjCfgImpl->get_HasProjectDependencies(pbHasDependencies);
}

HRESULT CXboxImageToolHelper::DoGetAdditionalDependenciesInternal(IVCPropertyContainer* pItem, IVCBuildAction* pAction,
	BOOL bForSave, BSTR* pbstrInputs, COptionHandlerBase* poptHandler, UINT idUserDeps, UINT idProjDeps, 
	LPCOLESTR szExtras /* = L"" */)
{
	CHECK_ZOMBIE( pItem, IDS_ERR_CFG_ZOMBIE );
	CHECK_POINTER_NULL( pbstrInputs );

	if (bForSave)
	{
		*pbstrInputs = NULL;
		CComVariant varDeps;
		HRESULT hr = pItem->GetLocalProp(idUserDeps, &varDeps);
		if (hr == S_OK && varDeps.vt == VT_BSTR)
		{
			CComBSTR bstrDeps = varDeps.bstrVal;
			*pbstrInputs = bstrDeps.Detach();
		}
		else if (hr == S_FALSE)
			poptHandler->GetDefaultValue( idUserDeps, pbstrInputs );
		return hr;
	}

	CComBSTR bstrExtras;
	if (szExtras && szExtras[0] != L'\0')
	{
		bstrExtras = L" ";
		bstrExtras += szExtras;
	}

	CComBSTR bstrUserExtraInputs;
	HRESULT hr1 = pItem->GetMultiProp(idUserDeps, L" ", VARIANT_FALSE, &bstrUserExtraInputs);
	if (hr1 == S_FALSE)
		poptHandler->GetDefaultValue( idUserDeps, &bstrUserExtraInputs );
	RETURN_ON_FAIL(hr1);
	HRESULT hr2 = pItem->Evaluate(bstrUserExtraInputs, &bstrUserExtraInputs);
	RETURN_ON_FAIL(hr2);

	// best way to figure out project dependencies, unfortunately, is to go out and get them
	CComPtr<IVCBuildStringCollection> spStrings;
	VARIANT_BOOL bUpToDate;
	HRESULT hr3 = DoGetDependencies(pAction, &spStrings, &bUpToDate, FALSE /* !dirty */);
	RETURN_ON_FAIL(hr3);
	CComBSTR bstrProjExtraInputs;
	hr3 = pItem->GetStrProperty(idProjDeps, &bstrProjExtraInputs);	// shared ID for dependent inputs
	if (hr3 == S_FALSE)
	{
		bstrUserExtraInputs += bstrExtras;
		*pbstrInputs = bstrUserExtraInputs.Detach();
		return hr1;
	}

	CStringW strUserInputs = bstrUserExtraInputs;
	strUserInputs.TrimLeft();
	strUserInputs.TrimRight();
	CStringW strProjInputs = bstrProjExtraInputs;
	strProjInputs.TrimLeft();
	strProjInputs.TrimRight();
	CComBSTR bstrFinalExtras = strUserInputs;
	if (!strUserInputs.IsEmpty() && !strProjInputs.IsEmpty())
		bstrFinalExtras += L" ";
	CComBSTR bstrProjInputs;
	if (!strProjInputs.IsEmpty())
		bstrProjInputs = strProjInputs;
	bstrFinalExtras += bstrProjInputs;
	bstrFinalExtras += bstrExtras;
	*pbstrInputs = bstrFinalExtras.Detach();

	return S_OK;
}

int CXboxImageToolHelper::GetNextDep(int nStartDep, CStringW& strDepList, int nDepLen, CStringW& strDep)
{
	strDep.Empty();
	if (nStartDep < 0)
		return nStartDep;
	else if (nStartDep >= nDepLen)
		return -1;

	int nSpace = nStartDep;
	const wchar_t* pchDepList = (const wchar_t*)strDepList + nStartDep;
	while (*pchDepList == L' ' && nSpace < nDepLen)	// first, skip any leading spaces
	{
		pchDepList++;
		nSpace++;
	}

	if (nSpace == nDepLen)	// we were dealing with trailing spaces
		return -1;

	nStartDep = nSpace;
	wchar_t chTerm;
	if (*pchDepList == L'"')
	{
		pchDepList++;	// go to next character;
		nSpace++;
		chTerm = L'"';
	}
	else
		chTerm = L' ';

	while (*pchDepList != chTerm && nSpace < nDepLen)
	{
		pchDepList++;
		nSpace++;
	}
	if (chTerm == L'"' && nSpace < nDepLen)	// need to pick up the terminator in this case...
		nSpace++;

	if (nSpace < nDepLen)
	{
		strDep = strDepList.Mid(nStartDep, nSpace-nStartDep);
		nSpace++;
		return nSpace;
	}

	if (nStartDep < nDepLen)
		strDep = strDepList.Right(nDepLen-nStartDep);

	return -1;
}

HRESULT CXboxImageToolHelper::DoGetDependencies(IVCBuildAction* pAction, IVCBuildStringCollection** ppStrings, 
	VARIANT_BOOL* pbUpToDate, BOOL bAllowDirty /* = TRUE */)
{
	CHECK_POINTER_NULL(ppStrings);
	*ppStrings = NULL;
	RETURN_ON_NULL2(pAction, S_FALSE);

	BOOL bHasDepend = FALSE;

	HRESULT hr1 = S_FALSE;
	CComPtr<IVCBuildableItem> spItem;
	pAction->get_Item(&spItem);
	CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = spItem;
	if (spProjCfgImpl != NULL)
	{
		hr1 = spProjCfgImpl->get_ProjectDependenciesAsStrings(ppStrings);
		RETURN_ON_FAIL(hr1);
		if (hr1 == S_OK)
			bHasDepend = TRUE;
	}

	UINT idBuiltFlag = XBOXIMAGEID_ImageCommandLineGenerated;
	CComPtr<IVCPropertyContainer> spPropContainer;
	pAction->get_PropertyContainer(&spPropContainer);
	VSASSERT(spPropContainer, "Hey, how'd we get to GetDependencies with an action with no property container?!?");
	RETURN_ON_NULL(spPropContainer);

	// now that we've got the list of dependencies at the project level, we need to make sure we add them
	// to the list of inputs to the xbox image program
	BOOL bResetDirty = TRUE;
	if (!bAllowDirty)	// playing with this from somewhere we DON'T want to be setting the dirty flag...
	{
		VARIANT_BOOL bVal;
		bResetDirty = (spPropContainer->GetBoolProperty(idBuiltFlag, &bVal) == S_OK);
		spPropContainer->Clear(idBuiltFlag);
	}

	if (bHasDepend && *ppStrings)
	{
		CStringW strProjDeps;
		CComBSTR bstrProjDir;
		spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir);
		CDirW dirProj;
		bool bNeedSpace = false;
		if (dirProj.CreateFromKnown(bstrProjDir))
		{
			(*ppStrings)->Reset();
			while (TRUE)
			{
				CComBSTR bstrDep;
				if ((*ppStrings)->Next(&bstrDep) != S_OK)
					break;

				CPathW path;
				if (!path.Create(bstrDep))
					continue;

				if (_wcsicmp(path.GetExtension(), L".lib") != 0)	// only looking for .lib extensions
					continue;

				CStringW strPath;
				if (!path.GetRelativeName(dirProj, strPath, TRUE))
					continue;

				if (bNeedSpace)
					strProjDeps += L" ";

				strProjDeps += strPath;
				bNeedSpace = true;
			}
		}
	}

	if (bResetDirty)
		spPropContainer->SetBoolProperty(idBuiltFlag, VARIANT_TRUE);	// we've now been through here at least once

	return bHasDepend ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
CXboxImageTool::CXboxImageTool() 
{
	m_nLowKey = XBOXIMAGETOOL_MIN_DISPID;
	m_nHighKey = XBOXIMAGETOOL_MAX_DISPID;
	m_nDirtyKey = XBOXIMAGEID_CmdLineOptionsDirty;
	m_nDirtyOutputsKey = XBOXIMAGEID_OutputsDirty;
}

HRESULT CXboxImageTool::CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppXboxImageTool)
{
	CHECK_POINTER_NULL(ppXboxImageTool);
	*ppXboxImageTool = NULL;

	CComObject<CXboxImageTool> *pObj;
	HRESULT hr = CComObject<CXboxImageTool>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		if( pPropContainer )
			pObj->Initialize(pPropContainer);
		CXboxImageTool *pVar = pObj;
		pVar->AddRef();
		*ppXboxImageTool = pVar;
	}
	return hr;
}

STDMETHODIMP CXboxImageTool::IsDeleteOnRebuildFile(LPCOLESTR szFile, VARIANT_BOOL* pbDelOnRebuild)
{
	CHECK_POINTER_NULL(pbDelOnRebuild);

	BOOL fDel = BldFileNameMatchesExtension(szFile, wszXboxImageDelOnRebuildExtensions);
	*pbDelOnRebuild = fDel ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CXboxImageTool::get_DefaultExtensions(BSTR* pVal)
{
	return DoGetDefaultExtensions(s_bstrExtensions, wszXboxImageDefaultExtensions, pVal);
}

STDMETHODIMP CXboxImageTool::put_DefaultExtensions(BSTR newVal)
{
	s_bstrExtensions = newVal;
	return S_OK;
}

STDMETHODIMP CXboxImageTool::GetAdditionalOptionsInternal(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, 
	BSTR* pbstrAdditionalOptions)
{ 
	return GetProcessedAdditionalOptions(pItem, XBOXIMAGEID_AdditionalOptions, bForBuild, bSkipLocal, pbstrAdditionalOptions);
}

STDMETHODIMP CXboxImageTool::GetBuildDescription(IVCBuildAction* pAction, BSTR* pbstrBuildDescription)
{
	if (s_bstrBuildDescription.Length() == 0)
		s_bstrBuildDescription.LoadString(IDS_DESC_XBOXIMAGEING);
	return s_bstrBuildDescription.CopyTo(pbstrBuildDescription);
}

STDMETHODIMP CXboxImageTool::get_ToolPathInternal(BSTR* pbstrToolPath)
{ 
	return get_ToolPath(pbstrToolPath);
}

STDMETHODIMP CXboxImageTool::get_Bucket(long *pVal)
{
	CHECK_POINTER_NULL( pVal );
	*pVal = BUCKET_XBOXIMAGE; 
	return S_OK;
}

STDMETHODIMP CXboxImageTool::GenerateOutput(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC)
{
	return DoGenerateOutput(type, plstActions, pEC, this);
}

///////////////////////////////////////////////////////////////////////////////
// Affects our output?
STDMETHODIMP CXboxImageTool::AffectsOutput(long nPropID, VARIANT_BOOL* pbAffectsOutput)
{
	CHECK_POINTER_NULL(pbAffectsOutput);
	*pbAffectsOutput = VARIANT_FALSE;

	// change our outputs?
	switch (nPropID)
	{
	case XBOXIMAGEID_FileName:			// *.xbe
		// anything we care about dirty
		*pbAffectsOutput = VARIANT_TRUE;
		break;
	default:
		*pbAffectsOutput = VARIANT_FALSE;
		break;
	}

	return S_OK;
}

long CXboxImageTool::GetPageCount()	{ return XBOXIMAGETOOL_NUMPAGES; }


GUID* CXboxImageTool::GetPageIDs()
{
	if (s_bPagesInit == FALSE)
	{
		s_pPages[0] = __uuidof(XboxImageGeneral);
		s_pPages[1] = __uuidof(XboxImageCertificate);
		s_pPages[2] = __uuidof(XboxImageTitleInfo);
		s_bPagesInit = TRUE;
	}
	return s_pPages; 
}

STDMETHODIMP CXboxImageTool::IsSpecialConsumable(LPCOLESTR szPath, VARIANT_BOOL* pbSpecial)
{
	CHECK_POINTER_NULL(pbSpecial);

	//if (BldFileNameMatchesExtension(szPath, L"def"))
	//	*pbSpecial = VARIANT_TRUE;
	//else
		*pbSpecial = VARIANT_FALSE;
	return S_OK;
}

HRESULT CXboxImageTool::GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rpath)
{
	return DoGetPrimaryOutputFromTool(pItem, rpath);
}

STDMETHODIMP CXboxImageTool::HasPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput)
{
	return COptionHandlerBase::GetValueTrue( pbHasPrimaryOutput );
}

STDMETHODIMP CXboxImageTool::GetPrimaryOutputIDFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID)
{
	CHECK_POINTER_NULL(pnOutputID);
	*pnOutputID = XBOXIMAGEID_FileName;
	return S_OK;
}

STDMETHODIMP CXboxImageTool::HasDependencies(IVCBuildAction* pAction, VARIANT_BOOL* pbHasDependencies)
{
	return DoHasDependencies(pAction, pbHasDependencies);
}

STDMETHODIMP CXboxImageTool::IsTargetTool(IVCBuildAction* pAction, VARIANT_BOOL* pbTargetTool)
{ // tool always operates on target, not on file
	return COptionHandlerBase::GetValueTrue(pbTargetTool);
}

STDMETHODIMP CXboxImageTool::get_ToolShortName(BSTR* pbstrToolName)
{
	*pbstrToolName = SysAllocString( szXboxImageToolShortName );
	return S_OK;
}

STDMETHODIMP CXboxImageTool::MatchName(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches)
{
	return DoMatchName(bstrName, szXboxImageToolType, szXboxImageToolShortName, pbMatches);
}

STDMETHODIMP CXboxImageTool::CreateToolObject(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
{ 
	return CreateInstance(pPropContainer, ppToolObject); 
}

STDMETHODIMP CXboxImageTool::CanScanForDependencies(VARIANT_BOOL* pbIsScannable)
{
	return COptionHandlerBase::GetValueTrue(pbIsScannable);
}

STDMETHODIMP CXboxImageTool::GetDependencies(IVCBuildAction* pAction, IVCBuildStringCollection** ppStrings, 
	VARIANT_BOOL* pbUpToDate)
{
	CHECK_READ_POINTER_NULL(pAction);
	return DoGetDependencies(pAction, ppStrings, pbUpToDate);
}

BOOL CXboxImageTool::CreateFileName(IVCPropertyContainer* pPropContainer, BSTR* pbstrVal)
{
	return s_optHandler.CreateFileName(pPropContainer, pbstrVal, FALSE);
}

STDMETHODIMP CXboxImageTool::CreatePageObject(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
{
	// Get the list of pages
	if( CLSID_XboxImageGeneral == *pCLSID )
	{
		CPageObjectImpl< CXboxImageGeneralPage, XBOXIMAGETOOL_MIN_DISPID, XBOXIMAGETOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_XboxImageCertificate == *pCLSID )
	{
		CPageObjectImpl< CXboxImageCertificatePage, XBOXIMAGETOOL_MIN_DISPID, XBOXIMAGETOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else if( CLSID_XboxImageTitleInfo == *pCLSID )
	{
		CPageObjectImpl< CXboxImageTitleInfoPage, XBOXIMAGETOOL_MIN_DISPID, XBOXIMAGETOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else
	{
		return S_FALSE;
	}
		
	return S_OK;
}

LPCOLESTR CXboxImageTool::GetToolFriendlyName()
{
	InitToolName();
	return s_bstrToolName;
}

void CXboxImageTool::InitToolName()
{
	if (s_bstrToolName.Length() == 0)
	{
		if (!s_bstrToolName.LoadString(IDS_XBOXIMAGE_TOOLNAME))
			s_bstrToolName = szXboxImageToolType;
	}
}

// general
STDMETHODIMP CXboxImageTool::get_ToolName(BSTR* pbstrToolName)
{	// friendly name of tool, e.g., "C/C++ Compiler Tool"
	CHECK_POINTER_VALID( pbstrToolName );
	InitToolName();
	s_bstrToolName.CopyTo(pbstrToolName);
	return S_OK;
}

STDMETHODIMP CXboxImageTool::get_AdditionalOptions(BSTR* pbstrAdditionalOptions)
{	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	return ToolGetStrProperty(XBOXIMAGEID_AdditionalOptions, pbstrAdditionalOptions);
}

STDMETHODIMP CXboxImageTool::put_AdditionalOptions(BSTR bstrAdditionalOptions)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(XBOXIMAGEID_AdditionalOptions, bstrAdditionalOptions);
}

STDMETHODIMP CXboxImageTool::get_FileName(BSTR* pbstrFileName)
{	// (/OUT:[file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	return ToolGetStrProperty(XBOXIMAGEID_FileName, pbstrFileName);
}

STDMETHODIMP CXboxImageTool::put_FileName(BSTR bstrFileName)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(XBOXIMAGEID_FileName, bstrFileName);
}

STDMETHODIMP CXboxImageTool::get_XBEFlags(long* pnFlags)
{
	return ToolGetIntProperty(XBOXIMAGEID_XBEFlags, pnFlags);
}

STDMETHODIMP CXboxImageTool::put_XBEFlags(long nFlags)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetIntProperty(XBOXIMAGEID_XBEFlags, nFlags);
}

STDMETHODIMP CXboxImageTool::get_StackSize(long* pnStackSize)
{
	return ToolGetIntProperty(XBOXIMAGEID_StackSize, pnStackSize);
}

STDMETHODIMP CXboxImageTool::put_StackSize(long nStackSize)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetIntProperty(XBOXIMAGEID_StackSize, nStackSize);
}

STDMETHODIMP CXboxImageTool::get_IncludeDebugInfo(VARIANT_BOOL* pbIncludeDebugInfo)
{	// (/NOLOGO) enable suppression of copyright message (no explicit off)
	return ToolGetBoolProperty(XBOXIMAGEID_IncludeDebugInfo, pbIncludeDebugInfo);
}

STDMETHODIMP CXboxImageTool::put_IncludeDebugInfo(VARIANT_BOOL bIncludeDebugInfo)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bIncludeDebugInfo );
	return m_spPropertyContainer->SetBoolProperty(XBOXIMAGEID_IncludeDebugInfo, bIncludeDebugInfo);
}

STDMETHODIMP CXboxImageTool::get_LimitAvailableMemoryTo64MB(VARIANT_BOOL* pbLimitAvailableMemoryTo64MB)
{	// (/NOLOGO) enable suppression of copyright message (no explicit off)
	return ToolGetBoolProperty(XBOXIMAGEID_LimitAvailableMemoryTo64MB, pbLimitAvailableMemoryTo64MB);
}

STDMETHODIMP CXboxImageTool::put_LimitAvailableMemoryTo64MB(VARIANT_BOOL bLimitAvailableMemoryTo64MB)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bLimitAvailableMemoryTo64MB );
	return m_spPropertyContainer->SetBoolProperty(XBOXIMAGEID_LimitAvailableMemoryTo64MB, bLimitAvailableMemoryTo64MB);
}

STDMETHODIMP CXboxImageTool::get_TitleID(long* pnTitleID)
{
	return ToolGetIntProperty(XBOXIMAGEID_TitleID, pnTitleID);
}

STDMETHODIMP CXboxImageTool::put_TitleID(long nTitleID)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetIntProperty(XBOXIMAGEID_TitleID, nTitleID);
}

STDMETHODIMP CXboxImageTool::get_XboxGameRegion(long* pnXboxGameRegion)
{
	return ToolGetIntProperty(XBOXIMAGEID_XboxGameRegion, pnXboxGameRegion);
}

STDMETHODIMP CXboxImageTool::put_XboxGameRegion(long nXboxGameRegion)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetIntProperty(XBOXIMAGEID_XboxGameRegion, nXboxGameRegion);
}


STDMETHODIMP CXboxImageTool::get_XboxGameRating(enumXboxGameRating* peXboxGameRating)
{
	return ToolGetIntProperty(XBOXIMAGEID_XboxGameRating, (long *)peXboxGameRating);
}

STDMETHODIMP CXboxImageTool::put_XboxGameRating(enumXboxGameRating eXboxGameRating)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_BOUNDS(XboxGameRatingMin, XboxGameRatingMax, eXboxGameRating);
	return m_spPropertyContainer->SetIntProperty(XBOXIMAGEID_XboxGameRating, eXboxGameRating);
}

STDMETHODIMP CXboxImageTool::get_TitleInfo(BSTR* pbstrTitleInfo)
{
	return ToolGetStrProperty(XBOXIMAGEID_TitleInfo, pbstrTitleInfo);
}

STDMETHODIMP CXboxImageTool::put_TitleInfo(BSTR bstrTitleInfo)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(XBOXIMAGEID_TitleInfo, bstrTitleInfo);
}

STDMETHODIMP CXboxImageTool::get_TitleName(BSTR* pbstrTitleName)
{
	return ToolGetStrProperty(XBOXIMAGEID_TitleName, pbstrTitleName);
}

STDMETHODIMP CXboxImageTool::put_TitleName(BSTR bstrTitleName)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(XBOXIMAGEID_TitleName, bstrTitleName);
}

STDMETHODIMP CXboxImageTool::get_TitleImage(BSTR* pbstrTitleImage)
{
	return ToolGetStrProperty(XBOXIMAGEID_TitleImage, pbstrTitleImage);
}

STDMETHODIMP CXboxImageTool::put_TitleImage(BSTR bstrTitleImage)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(XBOXIMAGEID_TitleImage, bstrTitleImage);
}

STDMETHODIMP CXboxImageTool::get_SaveGameImage(BSTR* pbstrSaveGameImage)
{
	return ToolGetStrProperty(XBOXIMAGEID_SaveGameImage, pbstrSaveGameImage);
}

STDMETHODIMP CXboxImageTool::put_SaveGameImage(BSTR bstrSaveGameImage)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(XBOXIMAGEID_SaveGameImage, bstrSaveGameImage);
}

#if 0

STDMETHODIMP CXboxImageTool::get_SuppressStartupBanner(VARIANT_BOOL* pbNoLogo)
{	// (/NOLOGO) enable suppression of copyright message (no explicit off)
	return ToolGetBoolProperty(XBOXIMAGEID_SuppressStartupBanner, pbNoLogo);
}

STDMETHODIMP CXboxImageTool::put_SuppressStartupBanner(VARIANT_BOOL bNoLogo)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_VARIANTBOOL( bNoLogo );
	return m_spPropertyContainer->SetBoolProperty(XBOXIMAGEID_SuppressStartupBanner, bNoLogo);
}

#endif

// Automation properties
STDMETHODIMP CXboxImageTool::get_ToolPath(BSTR *pVal)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POINTER_VALID(pVal);
	*pVal = SysAllocString( szXboxImageToolPath );
	return S_OK;
}

/*********************************************************************/

STDMETHODIMP CXboxImageGeneralPage::get_AdditionalOptions(BSTR* pbstrAdditionalOptions)
{	// anything we don't know what to do with, given verbatim, in same order as entered, parent contributions first, goes after all other options
	return ToolGetStrProperty(XBOXIMAGEID_AdditionalOptions, &(CXboxImageTool::s_optHandler), pbstrAdditionalOptions);
}

STDMETHODIMP CXboxImageGeneralPage::put_AdditionalOptions(BSTR bstrAdditionalOptions)
{
	return SetStrProperty(XBOXIMAGEID_AdditionalOptions, bstrAdditionalOptions);
}

STDMETHODIMP CXboxImageGeneralPage::get_FileName(BSTR* pbstrFileName)
{	// (/OUT:[file]) change the output file name (default is based on 1st lib or obj name on cmd line)
	return ToolGetStrProperty(XBOXIMAGEID_FileName, &(CXboxImageTool::s_optHandler), pbstrFileName);
}

STDMETHODIMP CXboxImageGeneralPage::put_FileName(BSTR bstrFileName)
{
	return SetStrProperty(XBOXIMAGEID_FileName, bstrFileName);
}

STDMETHODIMP CXboxImageGeneralPage::get_XBEFlags(long* pnFlags)
{
	return ToolGetIntProperty(XBOXIMAGEID_XBEFlags, &(CXboxImageTool::s_optHandler), pnFlags);
}

STDMETHODIMP CXboxImageGeneralPage::put_XBEFlags(long nFlags)
{
	return SetIntProperty(XBOXIMAGEID_XBEFlags, nFlags);
}

STDMETHODIMP CXboxImageGeneralPage::get_StackSize(long* pnStackSize)
{
	return ToolGetIntProperty(XBOXIMAGEID_StackSize, &(CXboxImageTool::s_optHandler), pnStackSize);
}

STDMETHODIMP CXboxImageGeneralPage::put_StackSize(long nStackSize)
{
	return SetIntProperty(XBOXIMAGEID_StackSize, nStackSize);
}

STDMETHODIMP CXboxImageGeneralPage::get_IncludeDebugInfo(enumBOOL* pbIncludeDebugInfo)
{	// ignore export lib
	return GetEnumBoolProperty(XBOXIMAGEID_IncludeDebugInfo, pbIncludeDebugInfo);
}

STDMETHODIMP CXboxImageGeneralPage::put_IncludeDebugInfo(enumBOOL bIncludeDebugInfo)
{
	return SetBoolProperty(XBOXIMAGEID_IncludeDebugInfo, bIncludeDebugInfo);
}

STDMETHODIMP CXboxImageGeneralPage::get_LimitAvailableMemoryTo64MB(enumBOOL* pbLimitAvailableMemoryTo64MB)
{	// ignore export lib
	return GetEnumBoolProperty(XBOXIMAGEID_LimitAvailableMemoryTo64MB, pbLimitAvailableMemoryTo64MB);
}

STDMETHODIMP CXboxImageGeneralPage::put_LimitAvailableMemoryTo64MB(enumBOOL bLimitAvailableMemoryTo64MB)
{
	return SetBoolProperty(XBOXIMAGEID_LimitAvailableMemoryTo64MB, bLimitAvailableMemoryTo64MB);
}

void CXboxImageGeneralPage::GetBaseDefault(long id, CComVariant& varValue)
{
	MessageBox(NULL,L"CXboxImageGeneralPage::GetBaseDefault",0,0);
	switch (id)
	{
	case XBOXIMAGEID_FileName:
		{
			CComBSTR bstrVal;
			CXboxImageTool::s_optHandler.GetDefaultValue( id, &bstrVal, m_pContainer );
			if (!bstrVal)
				bstrVal = L"";
			varValue = bstrVal;
		}
		break;
#if 0
	case XBOXIMAGEID_SuppressStartupBanner:
		{
			VARIANT_BOOL bVal;
			CXboxImageTool::s_optHandler.GetDefaultValue( id, &bVal );
			varValue = bVal;
		}
		break;
#endif
	default:
		CBase::GetBaseDefault(id, varValue);
	}
}

// XBox Image Certifacte Page

STDMETHODIMP CXboxImageCertificatePage::get_TitleID(long* pnTitleID)
{
	return ToolGetIntProperty(XBOXIMAGEID_TitleID, &(CXboxImageTool::s_optHandler), pnTitleID);
}

STDMETHODIMP CXboxImageCertificatePage::put_TitleID(long nTitleID)
{
	return SetIntProperty(XBOXIMAGEID_TitleID, nTitleID);
}

STDMETHODIMP CXboxImageCertificatePage::get_XboxGameRegion(long* pnXboxGameRegion)
{
	return ToolGetIntProperty(XBOXIMAGEID_XboxGameRegion, &(CXboxImageTool::s_optHandler), pnXboxGameRegion);
}

STDMETHODIMP CXboxImageCertificatePage::put_XboxGameRegion(long nXboxGameRegion)
{
	return SetIntProperty(XBOXIMAGEID_XboxGameRegion, nXboxGameRegion);
}

STDMETHODIMP CXboxImageCertificatePage::get_XboxGameRating(enumXboxGameRating* pnXboxGameRating)
{
	return ToolGetIntProperty(XBOXIMAGEID_XboxGameRating, &(CXboxImageTool::s_optHandler), (long*) pnXboxGameRating);
}

STDMETHODIMP CXboxImageCertificatePage::put_XboxGameRating(enumXboxGameRating nXboxGameRating)
{
	CHECK_BOUNDS(XboxGameRatingMin, XboxGameRatingMax, nXboxGameRating);
	return SetIntProperty(XBOXIMAGEID_XboxGameRating, nXboxGameRating);
}

// XBox Image Title Info Page

STDMETHODIMP CXboxImageTitleInfoPage::get_TitleInfo(BSTR* pbstrTitleInfo)
{
	return ToolGetStrProperty(XBOXIMAGEID_TitleInfo, &(CXboxImageTool::s_optHandler), pbstrTitleInfo);
}

STDMETHODIMP CXboxImageTitleInfoPage::put_TitleInfo(BSTR bstrTitleInfo)
{
	return SetStrProperty(XBOXIMAGEID_TitleInfo, bstrTitleInfo);
}

STDMETHODIMP CXboxImageTitleInfoPage::get_TitleName(BSTR* pbstrTitleName)
{
	return ToolGetStrProperty(XBOXIMAGEID_TitleName, &(CXboxImageTool::s_optHandler), pbstrTitleName);
}

STDMETHODIMP CXboxImageTitleInfoPage::put_TitleName(BSTR bstrTitleName)
{
	return SetStrProperty(XBOXIMAGEID_TitleName, bstrTitleName);
}

STDMETHODIMP CXboxImageTitleInfoPage::get_TitleImage(BSTR* pbstrTitleImage)
{
	return ToolGetStrProperty(XBOXIMAGEID_TitleImage, &(CXboxImageTool::s_optHandler), pbstrTitleImage);
}

STDMETHODIMP CXboxImageTitleInfoPage::put_TitleImage(BSTR bstrTitleImage)
{
	return SetStrProperty(XBOXIMAGEID_TitleImage, bstrTitleImage);
}

STDMETHODIMP CXboxImageTitleInfoPage::get_SaveGameImage(BSTR* pbstrSaveGameImage)
{
	return ToolGetStrProperty(XBOXIMAGEID_SaveGameImage, &(CXboxImageTool::s_optHandler), pbstrSaveGameImage);
}

STDMETHODIMP CXboxImageTitleInfoPage::put_SaveGameImage(BSTR bstrSaveGameImage)
{
	return SetStrProperty(XBOXIMAGEID_SaveGameImage, bstrSaveGameImage);
}
