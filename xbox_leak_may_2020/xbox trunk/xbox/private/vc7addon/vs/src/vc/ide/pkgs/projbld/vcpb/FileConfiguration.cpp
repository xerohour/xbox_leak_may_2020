// FileConfiguration.cpp : Implementation of 

#include "stdafx.h"
#include "FileConfiguration.h"
#include "BldActions.h"
#include "vctool.h"
#include "projwriter.h"

/////////////////////////////////////////////////////////////////////////////
//
HRESULT CFileConfiguration::CreateInstance(VCFileConfiguration **ppDispInfo)
{
	HRESULT hr;
	CFileConfiguration *pVar;
	CComObject<CFileConfiguration> *pObj;
	hr = CComObject<CFileConfiguration>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = pObj;
		pVar->AddRef();
		*ppDispInfo = pVar;
		//hr = pVar->Initialize(NULL, NULL);
	}
	return hr;
}

HRESULT CFileConfiguration::SetDirty(VARIANT_BOOL bDirty)
{
	CHECK_ZOMBIE(m_pFile, IDS_ERR_CFG_ZOMBIE);
	CComPtr<IDispatch> pDispProj;
	m_pFile->get_Project(&pDispProj);
	CComQIPtr<IVCProjectImpl> pProjImpl = pDispProj;
	CHECK_ZOMBIE(pProjImpl, IDS_ERR_CFG_ZOMBIE);

	return pProjImpl->put_IsDirty(bDirty);
}

STDMETHODIMP CFileConfiguration::MatchName(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched)
{
	CHECK_POINTER_VALID(pbMatched);
	CComBSTR bstrName;
	*pbMatched = VARIANT_FALSE;
	VSASSERT(m_pConfig, "Hey, no config set!");
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);

	m_pConfig->get_Name(&bstrName);
	if (!bstrName && !bstrNameToMatch)
		*pbMatched = VARIANT_TRUE;

	else if (!bstrName || !bstrNameToMatch)
		*pbMatched = VARIANT_FALSE;

	else if ( _wcsicmp(bstrName,bstrNameToMatch) == 0)
		*pbMatched = VARIANT_TRUE;

	else if (!bFullOnly)	// try matching without the platform
	{
		CStringW strName = bstrName;
		int nPos = strName.Find(L"|");
		if (nPos > 0)
			strName = strName.Left(nPos);
		if (_wcsicmp(strName,bstrNameToMatch) == 0)
			*pbMatched = VARIANT_TRUE;
	}

	return S_OK;
}

STDMETHODIMP CFileConfiguration::get_Name(BSTR *pVal)
{
	VSASSERT(m_pConfig, "Hey, no config set!");
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);

	CComBSTR bstrName;
	m_pConfig->get_Name(&bstrName);
	bstrName.CopyTo(pVal);
	return S_OK;
}


STDMETHODIMP CFileConfiguration::SaveObject(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent)
{
	VSASSERT(m_pConfig, "Hey, no config!");
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);

	if (!m_bIgnoreDefaultTool)	// if we have a non-default tool, always write things out
	{
		// if there is no local storage, then there is no point in writing the
		// file config or its child tool...
		VARIANT_BOOL bHaveLocalStorage = VARIANT_FALSE;
		HasLocalStorage( VARIANT_TRUE /* for save*/, &bHaveLocalStorage );
		if( bHaveLocalStorage == VARIANT_FALSE )
			return S_OK;
	}

	StartNodeHeader( xml, L"FileConfiguration", true );
	// Name
	CComBSTR bstrName;
	m_pConfig->get_Name(&bstrName);
	NodeAttribute( xml, L"Name", bstrName );

	// Excluded from build
	VARIANT_BOOL bExcludedFromBuild = VARIANT_FALSE;
	if (GetBoolProperty(VCFCFGID_ExcludedFromBuild, &bExcludedFromBuild) == S_OK 
		&& bExcludedFromBuild == VARIANT_TRUE)
	{
		CComBSTR bstrExcl = (bExcludedFromBuild == VARIANT_TRUE ? L"TRUE" : L"FALSE");
		NodeAttribute( xml, L"ExcludedFromBuild", bstrExcl );
	}

	// end of attributes, start children
	EndNodeHeader( xml, true );

	if (m_pTool)
	{
		m_pTool->SaveObject( xml, static_cast<IVCPropertyContainer *>(this), GetIndent() );
	}

	EndNode( xml, L"FileConfiguration", true );

	return S_OK;
}

STDMETHODIMP CFileConfiguration::Close()
{
	if (PROJECT_IS_LOADED() && !g_bInFileChangeMode)
		UnAssignActions(VARIANT_TRUE);

	if (m_spStagedContainer)
		m_spStagedContainer->Close();
	m_spStagedContainer.Release();

	m_pConfig = NULL;
	m_pFile = NULL;
	m_spActionList = NULL;
	if (m_pTool)
	{
		CComQIPtr<IVCToolImpl> spToolImpl = m_pTool;
		if (spToolImpl)
			spToolImpl->Close();
	}
	m_pTool = NULL;
	return S_OK;
}

STDMETHODIMP CFileConfiguration::get_Parent(IDispatch **pVal)
{
	m_pFile.QueryInterface(pVal);
	return S_OK;
}

STDMETHODIMP CFileConfiguration::get_Tool(IDispatch **pVal)
{
	CHECK_POINTER_VALID(pVal);
	*pVal = NULL;

	if (m_pTool == NULL)
	{
		// no tool yet set. See if we can get a default one...
		CComQIPtr<IVCConfigurationImpl> pConfigImpl = m_pConfig;
		HRESULT hr = S_OK;
		if (pConfigImpl)
			hr = pConfigImpl->SetDefaultToolForFile(static_cast<VCFileConfiguration *>(this));
		RETURN_ON_FAIL(hr);
	}

	if (m_pTool)
		m_pTool.QueryInterface(pVal);

	if (*pVal == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_NO_TOOL, IDS_ERR_NO_TOOL);

	return S_OK;
}

HRESULT CFileConfiguration::DoSetTool(IVCToolImpl* pNewTool, BOOL bDoAssign)
{
	CHECK_OK_TO_DIRTY(VCFCFGID_Tool);

	HRESULT hr = S_OK;
	CComPtr<IVCToolImpl> spToolImpl;
	if (pNewTool != NULL)
	{
		if (m_pTool != NULL)
		{
			CComBSTR bstrOldToolShortName;
			hr = m_pTool->get_ToolShortName(&bstrOldToolShortName);
			VSASSERT(SUCCEEDED(hr), "All tools have short names!");
			RETURN_ON_FAIL(hr);
			CComBSTR bstrNewToolShortName;
			hr = pNewTool->get_ToolShortName(&bstrNewToolShortName);
			VSASSERT(SUCCEEDED(hr), "All tools have short names!");
			RETURN_ON_FAIL(hr);
			if (bstrOldToolShortName.Length() == bstrNewToolShortName.Length())
			{
				CStringW strOldToolShortName = bstrOldToolShortName;
				CStringW strNewToolShortName = bstrNewToolShortName;
				if (strOldToolShortName == strNewToolShortName)
					return S_OK;
			}
		}
		hr = pNewTool->CreateToolObject(this, &spToolImpl);
		RETURN_ON_FAIL(hr);
	}

	if (!g_bInProjLoad && bDoAssign && m_pTool != NULL)
		UnAssignActions(VARIANT_FALSE);

	CComQIPtr<IDispatch> spDispTool = spToolImpl;
	if (m_pTool != NULL)
	{
		m_pTool->Close();
		m_pTool.Release();
	}
	m_pTool = spDispTool;

	if (!g_bInProjLoad && bDoAssign)
		AssignActions(VARIANT_FALSE);

	if (bDoAssign)	// came in as an explicit set, so need to figure out whether this is default tool or not
	{
		CComQIPtr<VCFile> spFile = m_pFile;
		if (spFile != NULL)
		{
			CComBSTR bstrName;
			if (SUCCEEDED(spFile->get_Name(&bstrName)))
			{
				CStringW strName = bstrName;
				if (!strName.IsEmpty())
				{
					int i = strName.ReverseFind(L'.');
					if (i > 0)
					{
						strName = strName.Mid(i);
						if (m_pTool)
						{	
							VARIANT_BOOL bIsTargetTool = VARIANT_FALSE;
							hr = m_pTool->IsTargetTool(NULL, &bIsTargetTool);
							VSASSERT(SUCCEEDED(hr), "Should always be able to tell whether a tool is a target tool!");
							if (bIsTargetTool == VARIANT_FALSE)	// don't attach a target tool to a file!!
							{
								CComBSTR bstrExt;
								hr = m_pTool->get_DefaultExtensions(&bstrExt);
								if (SUCCEEDED(hr))
								{
									CStringW strExtensions = bstrExt;
									if (!strExtensions.IsEmpty() && strExtensions.Find(strName) >= 0)
										m_bIgnoreDefaultTool = VARIANT_FALSE;
								}
							}
						}
					}
				}
			}
		}
	}
	return S_OK;
}

STDMETHODIMP CFileConfiguration::SetDefaultTool(IVCToolImpl* pTool, BOOL bReallyIsDefault)
{
	m_bIgnoreDefaultTool = bReallyIsDefault ? VARIANT_FALSE : VARIANT_TRUE;
	return DoSetTool(pTool, FALSE);
}

STDMETHODIMP CFileConfiguration::put_Tool(IDispatch *newVal)
{
	CComQIPtr<IVCToolImpl> spTool = newVal;
	if (newVal != NULL && spTool == NULL)	// allowed to set the tool to NULL, but must be intentional...
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_NO_TOOL, IDS_ERR_INVALID_TOOL);

	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);

	CHECK_OK_TO_DIRTY(0);

	if (spTool != NULL)
	{
		CComPtr<IDispatch> spDispTools;
		HRESULT hr = m_pConfig->get_Tools(&spDispTools);
		VSASSERT(SUCCEEDED(hr), "All configurations have tool lists, even if they're empty!");
		CComQIPtr<IVCCollection> spToolCollection = spDispTools;
		CHECK_ZOMBIE(spToolCollection, IDS_ERR_CFG_ZOMBIE);

		CComPtr<IDispatch> spMatchTool;
		CComQIPtr<IDispatch> spDispTool2 = spTool;
		CComVariant var;
		var.vt = VT_DISPATCH;
		var.pdispVal = spDispTool2.Detach();
		hr = spToolCollection->Item(var, &spMatchTool);
		RETURN_ON_FAIL(hr);
		if (spMatchTool == NULL)
			return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_NO_TOOL, IDS_ERR_NO_MATCHING_TOOL);
	}

	m_bIgnoreDefaultTool = VARIANT_TRUE;
	HRESULT hr = DoSetTool(spTool, TRUE);
	
	// if we're not loading, notify UI
	if( !g_bInProjLoad )
	{
		CComQIPtr<IDispatch> pDisp = this;
		if( pDisp )
		{
			CComQIPtr<IVCProjectEngineImpl> pProjEngImpl = g_pProjectEngine;
			if( pProjEngImpl )
				pProjEngImpl->DoFireItemPropertyChange( pDisp, VCFCFGID_Tool );
		}
	}

	return hr;
}

STDMETHODIMP CFileConfiguration::VerifyToolValid()
{	// verify that the currently assigned tool works with the current config properties
	RETURN_ON_NULL2(m_pTool, S_OK);

	if (m_bIgnoreDefaultTool)
	{
		CComPtr<IDispatch> spDispTools;
		HRESULT hr = m_pConfig->get_Tools(&spDispTools);
		VSASSERT(SUCCEEDED(hr), "All configurations have tool lists, even if they're empty!");
		CComQIPtr<IVCCollection> spToolCollection = spDispTools;
		if (spToolCollection == NULL)
		{
			m_pTool = NULL;
			return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_CFG_ZOMBIE);
		}

		CComBSTR bstrToolName;
		hr = m_pTool->get_ToolShortName(&bstrToolName);
		CComPtr<IDispatch> spMatchTool;
		CComVariant var = bstrToolName;
		hr = spToolCollection->Item(var, &spMatchTool);
		if (FAILED(hr) || spMatchTool == NULL)	// not in the list
			m_pTool = NULL;		// must pick up a different tool instead

		return S_OK;
	}
	else
	{
		// pick up a new tool -- no data will have been lost since it belongs to this file config, anyway, so this is safe
		m_pTool = NULL;
		return S_OK;
	}
}

STDMETHODIMP CFileConfiguration::get_OutputFile(BSTR* pbstrFile)
{	// output file name if this file gets built
	CHECK_POINTER_VALID(pbstrFile);
	*pbstrFile = NULL;

	VARIANT_BOOL bOutput = VARIANT_FALSE;
	if (m_pTool && m_pTool->HasPrimaryOutputFromTool((IUnknown*)(VCFileConfiguration*)this, VARIANT_FALSE, &bOutput) == S_OK 
		&& bOutput == VARIANT_TRUE)
		return m_pTool->GetPrimaryOutputFromTool((IUnknown*)(VCFileConfiguration*)this, VARIANT_FALSE, pbstrFile);

	return S_FALSE;
}

STDMETHODIMP CFileConfiguration::put_OutputFile(BSTR bstrFile)
{
	VARIANT_BOOL bOutput = VARIANT_FALSE;
	if (m_pTool && m_pTool->HasPrimaryOutputFromTool((IUnknown*)(VCFileConfiguration*)this, VARIANT_FALSE, &bOutput) == S_OK 
		&& bOutput == VARIANT_TRUE)
		return m_pTool->SetPrimaryOutputForTool((IUnknown*)(VCFileConfiguration*)this, bstrFile);

	return S_FALSE;
}

STDMETHODIMP CFileConfiguration::get_DefaultOutputFileFormat(BSTR* pbstrFormat)
{	// macro to use for determining the more or less default output name
	CHECK_POINTER_NULL(pbstrFormat);
	*pbstrFormat = NULL;

	VARIANT_BOOL bOutput = VARIANT_FALSE;
	if (m_pTool && m_pTool->HasPrimaryOutputFromTool((IUnknown*)(VCFileConfiguration*)this, VARIANT_FALSE, &bOutput) == S_OK 
		&& bOutput == VARIANT_TRUE)
		return m_pTool->GetDefaultOutputFileFormat((IUnknown*)(VCFileConfiguration*)this, pbstrFormat);

	return S_FALSE;
}

STDMETHODIMP CFileConfiguration::get_ExcludedFromBuild(/*[out, retval]*/  VARIANT_BOOL* pbExcludedFromBuild)
{
	HRESULT hr = GetBoolProperty(VCFCFGID_ExcludedFromBuild, pbExcludedFromBuild);
	if (hr == S_FALSE)
		SGetExcludedFromBuild(pbExcludedFromBuild);

	return hr;
}

HRESULT CFileConfiguration::SGetExcludedFromBuild(VARIANT_BOOL* pbExcluded)
{
	return COptionHandlerBase::GetValueFalse(pbExcluded);
}

STDMETHODIMP CFileConfiguration::put_ExcludedFromBuild(/*[in]*/ VARIANT_BOOL bExcludedFromBuild)
{
	CHECK_VARIANTBOOL(bExcludedFromBuild);
	return SetBoolProperty(VCFCFGID_ExcludedFromBuild, bExcludedFromBuild);
}

STDMETHODIMP CFileConfiguration::get_File(/*[out, retval]*/ IDispatch* *pVal)
{
	m_pFile.QueryInterface(pVal);

	return S_OK;
}

STDMETHODIMP CFileConfiguration::put_File(/*[in]*/ IDispatch* newVal)
{
	CComQIPtr<VCFile> pFile = newVal;
	m_pFile = pFile;
	return S_OK;
}

STDMETHODIMP CFileConfiguration::get_Configuration(/*[out, retval]*/ IDispatch* *pVal)
{
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);
	CHECK_POINTER_VALID(pVal);

	m_pConfig.QueryInterface(pVal);
	return S_OK;
}

STDMETHODIMP CFileConfiguration::put_Configuration(/*[in]*/ IDispatch* newVal)
{
	CComQIPtr<VCConfiguration> pConfig = newVal;
	m_pConfig = pConfig;

	return S_OK;
}

STDMETHODIMP CFileConfiguration::get_IgnoreDefaultTool(/*[out, retval]*/ VARIANT_BOOL* pbIgnoreDefaultTool)
{
	CHECK_POINTER_NULL(pbIgnoreDefaultTool);
	*pbIgnoreDefaultTool = m_bIgnoreDefaultTool;
	return S_OK;
}	// shortcut way to figure out if the user explicitly set the tool

HRESULT CFileConfiguration::GetFileCfgPCR(CBldCfgRecord** ppCfgRecord)
{
	CHECK_POINTER_NULL(ppCfgRecord);
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);

	CComQIPtr<IDispatch> pConfig;
	pConfig = m_pConfig;
	CComPtr<IDispatch> pDispProject;
	HRESULT hr = m_pConfig->get_Project(&pDispProject);
	RETURN_ON_FAIL2(hr, E_UNEXPECTED);

	*ppCfgRecord = g_StaticBuildEngine.GetCfgRecord(pDispProject, pConfig);
	return S_OK;
}


STDMETHODIMP CFileConfiguration::AssignActions(VARIANT_BOOL bOnLoad)
{
	CBldCfgRecord* pCfgRecord = NULL;
	HRESULT hr = GetFileCfgPCR(&pCfgRecord);
	RETURN_ON_FAIL(hr);

	VCFileConfiguration* pFileCfg = this;
	CBldAction::AssignActions(pCfgRecord, pFileCfg);
	return S_OK;
}


STDMETHODIMP CFileConfiguration::UnAssignActions(VARIANT_BOOL bOnClose)
{
	CBldCfgRecord* pCfgRecord = NULL;
	HRESULT hr = GetFileCfgPCR(&pCfgRecord);
	RETURN_ON_FAIL(hr);

	VCFileConfiguration *pFileCfg = this;
	CBldAction::UnAssignActions(pCfgRecord, pFileCfg);
	return S_OK;
}

STDMETHODIMP CFileConfiguration::CreatePageObject(IUnknown **ppUnk, CLSID *pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
{
	CComQIPtr<IVCToolImpl> pTool;
	CComPtr<IDispatch> pDisp;
	
	if( *pCLSID ==  CLSID_GeneralFileConfigSettingsPage )
	{
		CPageObjectImpl<CVCGeneralFileConfigSettingsPage, VCFILECONFIGURATION_MIN_DISPID, VCFILECONFIGURATION_MAX_DISPID>::CreateInstance(ppUnk, pPropCntr, pPage);
		return S_OK;		// this is the makefile project case.
	}

	HRESULT hr = get_Tool( &pDisp );
	if( FAILED( hr ) ) 
		return S_OK;		// this is the makefile project case.

	// if this file has a tool
	pTool = pDisp;
	if( pTool != NULL )
	{
		hr = pTool->CreatePageObject(ppUnk, pCLSID, pPropCntr, pPage );
		return hr;		// this is the makefile project case.
	}
	return E_FAIL;		// this is the makefile project case.
}

STDMETHODIMP CFileConfiguration::GetPages(CAUUID *pPages)
{
	CHECK_POINTER_NULL(pPages);

	CComQIPtr<IVCToolImpl> pTool;
	CComPtr<IDispatch> pDisp;
	CAUUID pages;

	pPages->pElems = (GUID*)NULL;
	pPages->cElems = 0;

	HRESULT hr = get_Tool( &pDisp );
	if( FAILED( hr ) ) 
		return S_OK;		// this is the makefile project case.

	// if this file has a tool
	pTool = pDisp;
	if( pTool != NULL )
	{

		hr = pTool->GetPages( &pages );

		// if we have a tool, allocate for its pages plus the general page
		if( SUCCEEDED( hr ) && pages.cElems )
		{
			// number of tool pages plus number of 'extra' pages (1)
			pPages->cElems = pages.cElems + 1;

			pPages->pElems = (GUID*) CoTaskMemAlloc(sizeof(CLSID)*(pPages->cElems));
			RETURN_ON_NULL2(pPages->pElems, E_OUTOFMEMORY);
		}
	}
	// otherwise just allocate for the general page
	else
	{
		pPages->cElems = 1;
		pPages->pElems = (GUID*) CoTaskMemAlloc( sizeof(CLSID) );
		RETURN_ON_NULL2(pPages->pElems, E_OUTOFMEMORY);
	}

	// copy the static list so that the caller can delete it.
	unsigned long i;

	// add the general->configuration page
	pPages->pElems[0] = __uuidof(GeneralFileConfigSettingsPage);

	for( i = 1; i < pPages->cElems; i++ )
	{
		pPages->pElems[i] = pages.pElems[i-1];
	}

	return hr;
}

STDMETHODIMP CFileConfiguration::get_ItemFileName(BSTR *pVal)
{
	CComPtr<IDispatch> spDispFile;
	if (FAILED(get_File(&spDispFile)) || spDispFile == NULL)
	{
		VSASSERT(FALSE, "Can only get here if we are zombie.");
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_CFG_ZOMBIE);
	}
	CComQIPtr<VCFile> spFile = spDispFile;
	VSASSERT(spFile != NULL, "Parent is not a file?!?");

	return spFile->get_Name(pVal);
}

STDMETHODIMP CFileConfiguration::get_ItemFullPath(BSTR* pbstrFullPath)
{
	CComPtr<IDispatch> spDispFile;
	if (FAILED(get_File(&spDispFile)) || spDispFile == NULL)
	{
		VSASSERT(FALSE, "Can only get here if we are zombie.");
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_CFG_ZOMBIE);
	}
	CComQIPtr<VCFile> spFile = spDispFile;
	VSASSERT(spFile != NULL, "Parent is not a file?!?");

	return spFile->get_FullPath(pbstrFullPath);
}

STDMETHODIMP CFileConfiguration::get_ActionList(IVCBuildActionList** ppActionList)
{
	CHECK_POINTER_NULL(ppActionList);

	if (m_spActionList == NULL)
	{
		HRESULT hr = CBldActionList::CreateInstance(&m_spActionList);
		RETURN_ON_FAIL(hr);
	}

	m_spActionList.CopyTo(ppActionList);
	return S_OK;
}

STDMETHODIMP CFileConfiguration::get_FileRegHandle(void** pfrh)
{
	CHECK_POINTER_NULL(pfrh);

	CComPtr<IDispatch> spDispFile;
	if (FAILED(get_File(&spDispFile)) || spDispFile == NULL)
	{
		VSASSERT(FALSE, "Can only get here if we are zombie.");
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_CFG_ZOMBIE);
	}
	CComQIPtr<IVCBuildableItem> spBuildableItem = spDispFile;
	VSASSERT(spBuildableItem != NULL, "Parent must be a file.  Files must be buildable items.");

	return spBuildableItem->get_FileRegHandle(pfrh);
}

STDMETHODIMP CFileConfiguration::get_ContentList(IEnumVARIANT** ppContents)
{
	CHECK_POINTER_NULL(ppContents);
	*ppContents = NULL;
	
	return S_FALSE;		// don't have a content list the file registry is interested in
}

STDMETHODIMP CFileConfiguration::get_Registry(void** ppFileRegistry)
{
	CHECK_POINTER_NULL(ppFileRegistry);

	CComPtr<IDispatch> spDispFile;
	if (FAILED(get_File(&spDispFile)) || spDispFile == NULL)
	{
		VSASSERT(FALSE, "Can only get here if we are zombie.");
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_CFG_ZOMBIE);
	}

	CComQIPtr<IVCBuildableItem> spBuildableItem = spDispFile;
	VSASSERT(spBuildableItem != NULL, "Parent must be a file.  Files must be buildable items.");

	return spBuildableItem->get_Registry(ppFileRegistry);
}

STDMETHODIMP CFileConfiguration::get_ProjectConfiguration(VCConfiguration** ppProjCfg)
{
	return m_pConfig.CopyTo(ppProjCfg);
}

STDMETHODIMP CFileConfiguration::get_ProjectInternal(VCProject** ppProject)
{
	CHECK_POINTER_NULL(ppProject);
	*ppProject = NULL;
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);

	CComPtr<IDispatch> spDispProject;
	HRESULT hr = m_pConfig->get_Project(&spDispProject);
	RETURN_ON_FAIL(hr);
	CComQIPtr<VCProject> spProject = spDispProject;
	return spProject.CopyTo(ppProject);
}

STDMETHODIMP CFileConfiguration::get_ExistingBuildEngine(IVCBuildEngine** ppBldEngine)
{
	CHECK_POINTER_NULL(ppBldEngine);
	*ppBldEngine = NULL;

	CComQIPtr<IVCBuildableItem> spBldableProjCfg = m_pConfig;
	CHECK_ZOMBIE(spBldableProjCfg, IDS_ERR_CFG_ZOMBIE);

	return spBldableProjCfg->get_ExistingBuildEngine(ppBldEngine);
}

STDMETHODIMP CFileConfiguration::get_PersistPath(BSTR* pbstrPersistPath)
{
	CHECK_POINTER_NULL(pbstrPersistPath);

	CComPtr<IDispatch> spDispFile;
	HRESULT hr = get_File(&spDispFile);
	CComQIPtr<IVCBuildableItem> spBldableFile = spDispFile;
	RETURN_ON_FAIL(hr);
	CHECK_ZOMBIE(spBldableFile, IDS_ERR_CFG_ZOMBIE);

	return spBldableFile->get_PersistPath(pbstrPersistPath);
}

STDMETHODIMP CFileConfiguration::RefreshActionOutputs(long nPropID, IVCBuildErrorContext* pEC)
{
	if (m_spActionList != NULL)
		m_spActionList->RefreshOutputs(nPropID, pEC);

	return S_OK;
}

STDMETHODIMP CFileConfiguration::HandlePossibleCommandLineOptionsChange(IVCBuildErrorContext* pEC)
{
	HRESULT hr = S_FALSE;
	if (m_spActionList != NULL)
		hr = m_spActionList->HandlePossibleCommandLineOptionsChange(pEC);

	return hr;
}

STDMETHODIMP CFileConfiguration::ClearDirtyCommandLineOptionsFlag()
{
	VARIANT_BOOL bDirtyDuringBuild = VARIANT_FALSE;
	if (GetBoolProperty(VCCFGID_DirtyDuringBuild, &bDirtyDuringBuild) == S_OK)
		return S_FALSE;	// if we dirtied during the build, let the build options be refreshed again next time

	HRESULT hr = S_FALSE;
	if (m_pTool)
		hr = m_pTool->DirtyCommandLineOptions(-1, -1, VARIANT_FALSE);

	return hr;
}

void CFileConfiguration::ResetRegRef(VARIANT_BOOL bReleaseRef)
{
	if (m_spActionList == NULL)
		return;		// nothing to do

	CComQIPtr<IVCBuildableItem> spFileItem = m_pFile;
	if (spFileItem == NULL)
		return;		// nothing to do

	BldFileRegHandle frh = NULL;
	spFileItem->get_FileRegHandle((void **)&frh);
	if (frh == NULL)
		return;		// nothing to do

	m_spActionList->Reset(NULL);
	while (TRUE)
	{
		CComPtr<IVCBuildAction> spAction;
		HRESULT hr = m_spActionList->Next(&spAction, NULL);
		if (hr != S_OK)
			break;
		else if (spAction == NULL)
			continue;

		CBldFileRegistry* preg = NULL;
		spAction->get_Registry((void **)&preg);
		if (preg != NULL)
		{
			if (bReleaseRef)
				preg->ReleaseRegRef(frh, TRUE /* pull out filtering */);
			else
				preg->AddRegRef(frh);
		}
	}
}

void CFileConfiguration::SetToolsDirty()
{
	VSASSERT(m_pConfig, "Hey, no config set!");
	if (m_pConfig == NULL)
		return;		// nothing to do

	CComQIPtr<IVCPropertyContainer> spParent = m_pConfig;
	if (spParent == NULL)
		return;		// nothing to do

	spParent->SetBoolProperty(VCCFGID_PrimaryOutputDirty, VARIANT_TRUE);	// force relink
	ConfigurationTypes configType;
	if (FAILED(m_pConfig->get_ConfigurationType(&configType)))
		return;		// nothing to do

	long nToolDirtyID = -1;
	long nToolDirtyOutputsID = -1;
	switch (configType)
	{
	case typeApplication:
	case typeDynamicLibrary:
		nToolDirtyID = VCLINKID_CmdLineOptionsDirty;
		nToolDirtyOutputsID = VCLINKID_OutputsDirty;
		break;
	case typeStaticLibrary:
		nToolDirtyID = VCLIBID_CmdLineOptionsDirty;
		nToolDirtyOutputsID = VCLIBID_OutputsDirty;
		break;
	}

	if (nToolDirtyID > 0)
	{
		spParent->SetBoolProperty(nToolDirtyID, VARIANT_TRUE);
		spParent->SetBoolProperty(nToolDirtyOutputsID, VARIANT_TRUE);
		spParent->SetBoolProperty(VCCFGID_AnyToolDirty, VARIANT_TRUE);
	}
}

void CFileConfiguration::StartSetOrClearProp(long id, BOOL& bCareAboutChange, VARIANT_BOOL& bOldVal)
{
	bCareAboutChange = FALSE;
	bOldVal = VARIANT_FALSE;

	if (id == VCFCFGID_ExcludedFromBuild)
	{
		if (GetBoolProperty(VCFCFGID_ExcludedFromBuild, &bOldVal) != S_OK)
			bOldVal = VARIANT_FALSE;
		bCareAboutChange = TRUE;
	}
	else if( id == VCFCSTID_CommandLine || id == VCFCSTID_Outputs )
	{
		bCareAboutChange = TRUE;
		bOldVal = VARIANT_TRUE;
	}
}

void CFileConfiguration::FinishSetOrClearProp(long id, BOOL bCareAboutChange, VARIANT_BOOL bOldVal, VARIANT_BOOL bNewVal)
{
	if (PROJECT_IS_LOADED() && IsRealProp(id))
	{
		DirtyCommandLineOptions(id, id);
		if (bCareAboutChange && bOldVal != bNewVal)
		{
			// update UI
			CComQIPtr<IDispatch> pDisp = this;
			if( pDisp )
			{
				CComQIPtr<IVCProjectEngineImpl> pProjEngImpl = g_pProjectEngine;
				if( pProjEngImpl )
					pProjEngImpl->DoFireItemPropertyChange( pDisp, id );
			}
			if (id == VCFCFGID_ExcludedFromBuild)
				ResetRegRef(bNewVal);
			SetToolsDirty();
		}
	}
}

STDMETHODIMP CFileConfiguration::SetProp(long id, VARIANT varValue)
{
	CHECK_OK_TO_DIRTY(id);

	BOOL bCareAboutChange = FALSE;
	VARIANT_BOOL bOldExcludeFromBuild = VARIANT_FALSE;
	StartSetOrClearProp(id, bCareAboutChange, bOldExcludeFromBuild);

	if( !m_pPropMap )
		m_pPropMap = new CVCMap<long,long,CComVariant,VARIANT&>;
	m_pPropMap->SetAt(id, varValue);

	VARIANT_BOOL bNewVal = VARIANT_FALSE;
	if (bCareAboutChange)
		bNewVal = varValue.boolVal;
	FinishSetOrClearProp(id, bCareAboutChange, bOldExcludeFromBuild, bNewVal);

	return S_OK;
}

HRESULT CFileConfiguration::DoGetProp(long id, BOOL bCheckSpecialProps, VARIANT *pVarValue)
{
	// check if it is a special prop that really has local storage (or belongs to a non-property container parent 
	// who does) first
	if (bCheckSpecialProps)
	{
		CComBSTR bstrProp;
		if (DoGetStrProperty(id, TRUE, &bstrProp) == S_OK)
		{
			pVarValue->bstrVal = bstrProp.Detach();
			pVarValue->vt = VT_BSTR;
			return S_OK;
		}
	}

	CComVariant varVal;

	// Check if its here first
	if( m_pPropMap && m_pPropMap->Lookup(id, varVal) )
	{
		varVal.Detach(pVarValue);
		return S_OK;
	}

	// Its not here, so check your parents
	return GetParentProp(id, VARIANT_TRUE /* allow inherit */, pVarValue);
}

STDMETHODIMP CFileConfiguration::GetProp(long id, VARIANT *pVarValue)
{
	return DoGetProp(id, TRUE, pVarValue);
}

STDMETHODIMP CFileConfiguration::GetMultiProp(long id, LPCOLESTR szSeparator, VARIANT_BOOL bSkipLocal, BSTR *pbstrValue)
{
	CHECK_POINTER_NULL(pbstrValue);
	*pbstrValue = NULL;

	HRESULT hrReturn = S_FALSE;
	CComBSTR bstrLocal;
	
	// look in here.
	CComVariant var;
	if( !bSkipLocal && m_pPropMap )
	{
		BOOL b;
		b = m_pPropMap->Lookup(id, var);
		if( b )
		{
			if (NoInheritOnMultiProp(var.bstrVal, szSeparator, pbstrValue))
				return S_OK;	// means we're not doing inheritance
			bstrLocal = var.bstrVal;
			hrReturn = S_OK;
		}
	}
	
	// And check your parents
	CComQIPtr<IVCPropertyContainer> pParent = m_pConfig;
	CComBSTR bstrParent;
	if (pParent)
	{
		HRESULT hr = pParent->GetMultiProp(id, szSeparator, VARIANT_FALSE, &bstrParent);
		if (hr == S_OK)
			hrReturn = S_OK;
		if (hr != S_OK)
			bstrParent = L"";
	}

	if (hrReturn != S_OK)	// nothing further to do here
		return hrReturn;

	return MakeMultiPropString(bstrLocal, bstrParent, szSeparator, pbstrValue);
}

STDMETHODIMP CFileConfiguration::GetEvaluatedMultiProp(long idProp, LPCOLESTR szSeparator, VARIANT_BOOL bCollapseMultiples, 
	VARIANT_BOOL bCaseSensitive, BSTR* pbstrValue)
{
	CHECK_POINTER_NULL(pbstrValue);
	*pbstrValue = NULL;

	HRESULT hr1 = GetMultiProp(idProp, szSeparator, VARIANT_FALSE, pbstrValue);
	RETURN_ON_FAIL(hr1);

    CComBSTR bstrTemp;
	HRESULT hr2 = Evaluate(*pbstrValue, &bstrTemp);
	RETURN_ON_FAIL(hr2);
    SysFreeString(*pbstrValue);
    *pbstrValue = bstrTemp.Detach();


	if (bCollapseMultiples)
	{
		HRESULT hr3 = CollapseMultiples(*pbstrValue, szSeparator, bCaseSensitive, pbstrValue);
		RETURN_ON_FAIL(hr3);
	}

	return hr1;
}

HRESULT CFileConfiguration::DoGetStrProperty(long idProp, BOOL bSpecialPropsOnly, BSTR* pbstrValue)
{
	CHECK_POINTER_NULL(pbstrValue);
	*pbstrValue = NULL;

	HRESULT hr = S_FALSE;

	switch (idProp)
	{
	case VCFILEID_Name:
		RETURN_ON_NULL2(m_pFile, S_FALSE);
		return m_pFile->get_Name(pbstrValue);
		break;
	default:
		if (!bSpecialPropsOnly)
		{
			CComVariant var;
			hr = GetProp(idProp, &var);
			if (hr == S_OK)
			{
				CComBSTR bstrVal = var.bstrVal;
				*pbstrValue = bstrVal.Detach();
			}
		}
	}

	return hr;
}

STDMETHODIMP CFileConfiguration::GetStrProperty(long idProp, BSTR* pbstrValue)
{
	return DoGetStrProperty(idProp, FALSE, pbstrValue);
}

STDMETHODIMP CFileConfiguration::SetStrProperty(long idProp, BSTR bstrValue)
{
	CComVariant var(bstrValue);
	return SetProp(idProp, var);
}

STDMETHODIMP CFileConfiguration::GetEvaluatedStrProperty(long idProp, BSTR* pbstrValue)
{
	CHECK_POINTER_NULL(pbstrValue);
	*pbstrValue = NULL;

	CComBSTR bstrIn;
	HRESULT hr1 = GetStrProperty(idProp, &bstrIn);
	if (FAILED(hr1))
	{
		if (bstrIn)
			*pbstrValue = bstrIn.Detach();
		return hr1;
	}

	HRESULT hr2 = Evaluate(bstrIn, pbstrValue);
	RETURN_ON_FAIL(hr2);

	return hr1;
}

STDMETHODIMP CFileConfiguration::GetIntProperty(long idProp, long* pnValue)
{
	CHECK_POINTER_NULL(pnValue);
	*pnValue = 0;

	CComVariant var;
	HRESULT hr = GetProp(idProp, &var);
	if (hr == S_OK)
		*pnValue = var.intVal;
	return hr;
}

STDMETHODIMP CFileConfiguration::SetIntProperty(long idProp, long nValue)
{
	CComVariant var(nValue);
	return SetProp(idProp, var);
}

STDMETHODIMP CFileConfiguration::GetBoolProperty(long idProp, VARIANT_BOOL* pbValue)
{
	CHECK_POINTER_NULL(pbValue);
	*pbValue = VARIANT_FALSE;

	CComVariant var;
	HRESULT hr = GetProp(idProp, &var);
	if (hr == S_OK)
		*pbValue = var.boolVal;
	return hr;
}

STDMETHODIMP CFileConfiguration::SetBoolProperty(long idProp, VARIANT_BOOL bValue)
{
	CComVariant var(bValue);
	return SetProp(idProp, var);
}

STDMETHODIMP CFileConfiguration::Clear(long id)
{
	if( m_pPropMap )
	{
		CHECK_OK_TO_DIRTY(id);
		BOOL bCareAboutChange = FALSE;
		VARIANT_BOOL bOldVal = VARIANT_FALSE;
		StartSetOrClearProp(id, bCareAboutChange, bOldVal);
		m_pPropMap->RemoveKey(id);
		FinishSetOrClearProp(id, bCareAboutChange, bOldVal, VARIANT_FALSE);
	}
	return S_OK;
}

STDMETHODIMP CFileConfiguration::GetLocalProp(long id, VARIANT *pvarValue)
{
	CComVariant varVal;

	// Check if its here first
	if( m_pPropMap && m_pPropMap->Lookup(id, varVal) )
	{
		if(pvarValue)
			varVal.Detach(pvarValue);
		return S_OK;
	}

	return S_FALSE;
}

STDMETHODIMP CFileConfiguration::GetParentProp(long id, VARIANT_BOOL bAllowInherit, VARIANT *pVarValue)
{
	CComQIPtr<IVCPropertyContainer> pParent = m_pConfig;
	RETURN_ON_NULL2(pParent, S_FALSE);

	CComPtr<IVCStagedPropertyContainer> spStagedParent;
	pParent->GetStagedPropertyContainer(VARIANT_FALSE, &spStagedParent);
	if (spStagedParent)
	{
		CComQIPtr<IVCPropertyContainer> spPropContainer = spStagedParent;
		if (spPropContainer)
			return spPropContainer->GetProp(id, pVarValue);
	}

	if (bAllowInherit)
	{
		if(pParent->GetProp( id, pVarValue ) == S_OK)
			return S_OK;
	}
	else
	{
		if(pParent->GetLocalProp( id, pVarValue ) == S_OK)
			return S_OK;
	}

	// We didn't find it
	return S_FALSE;  
}

STDMETHODIMP CFileConfiguration::IsMultiProp(long id, VARIANT_BOOL* pbIsMultiProp)
{
	CHECK_POINTER_NULL(pbIsMultiProp);
	*pbIsMultiProp = VARIANT_FALSE;

	if (id >= VCFILECONFIGURATION_MIN_DISPID && id <= VCFILECONFIGURATION_MAX_DISPID)
		return S_OK;	// file config's properties are not multi-props

	if (m_pTool)
		return m_pTool->IsMultiProp(id, pbIsMultiProp);

	return S_FALSE;
}

STDMETHODIMP CFileConfiguration::GetMultiPropSeparator(long id, BSTR* pbstrPreferred, BSTR* pbstrAll)
{
	CHECK_POINTER_NULL(pbstrPreferred);
	CHECK_POINTER_NULL(pbstrAll);
	*pbstrPreferred = NULL;
	*pbstrAll = NULL;

	if (id >= VCFILECONFIGURATION_MIN_DISPID && id <= VCFILECONFIGURATION_MAX_DISPID)
		return S_FALSE;	// file config's properties are not multi-props

	if (m_pTool)
		return m_pTool->GetMultiPropSeparator(id, pbstrPreferred, pbstrAll);

	return S_FALSE;
}

STDMETHODIMP CFileConfiguration::DirtyProp(long id)
{
	return S_FALSE;	// can't apply a style sheet here, so can't do a dirty like this, either
}

STDMETHODIMP CFileConfiguration::Evaluate(BSTR bstrIn, BSTR *pbstrOut)
{
	CHECK_POINTER_VALID(pbstrOut);
	return ExpandMacros(pbstrOut, bstrIn, this, FALSE);
}

STDMETHODIMP CFileConfiguration::EvaluateWithValidation(BSTR bstrSource, long idProp, BSTR* pbstrExpanded)
{
	return ExpandMacros(pbstrExpanded, bstrSource, this, FALSE, idProp);
}

STDMETHODIMP CFileConfiguration::HasLocalStorage(VARIANT_BOOL bForSave, VARIANT_BOOL* pbHasLocalStorage)
{
	CHECK_POINTER_NULL(pbHasLocalStorage);
	*pbHasLocalStorage = VARIANT_FALSE;

	if (m_pPropMap != NULL)
	{
		VCPOSITION pos = m_pPropMap->GetStartPosition();
		while (pos && *pbHasLocalStorage == VARIANT_FALSE)
		{
			long id;
			CComVariant var;
			m_pPropMap->GetNextAssoc(pos, id, var);
			if (!IsRealProp(id))
				continue;	// false hit for storage

			switch (var.vt)
			{
			case VT_I2:
			case VT_I4:
			case VT_BOOL:
				*pbHasLocalStorage = VARIANT_TRUE;
				break;
			case VT_BSTR:
				{
					CStringW strVal = var.bstrVal;
					if (!strVal.IsEmpty())
						*pbHasLocalStorage = VARIANT_TRUE;
					else
					{
						IsMultiProp(id, pbHasLocalStorage);
						*pbHasLocalStorage = !(*pbHasLocalStorage);		// don't treat blank multi-prop value as local storage
					}
					break;
				}
			}
		}

		return S_OK;
	}
	else if (bForSave == VARIANT_TRUE)
		return S_OK;

	if (m_pTool)
		return m_pTool->HasVirtualLocalStorage(this, pbHasLocalStorage);

	return S_OK;
}

HRESULT CFileConfiguration::DirtyCommandLineOptions(long nLowKey, long nHighKey)
{
	if (m_pTool)
	{
		HRESULT hr = m_pTool->DirtyCommandLineOptions(nLowKey, nHighKey, VARIANT_TRUE);
		if (hr == S_OK)
		{
			CComQIPtr<IVCPropertyContainer> pParent = m_pConfig;
			if (pParent)
			{
				pParent->SetBoolProperty(VCCFGID_AnyToolDirty, VARIANT_TRUE);
				VARIANT_BOOL bOutputDirty;
				if (m_pTool->OutputsAreDirty(this, &bOutputDirty) == S_OK)
					pParent->SetBoolProperty(VCCFGID_AnyOutputDirty, VARIANT_TRUE);
			}
		}
		return hr;
	}

	return S_FALSE;
}

STDMETHODIMP CFileConfiguration::ClearAll()
{
	if( m_pPropMap )
	{
		CHECK_OK_TO_DIRTY(0);
		if (PROJECT_IS_LOADED())	// make sure we get any appropriate command lines dirtied...
		{
			VCPOSITION pos = m_pPropMap->GetStartPosition();
			while (pos != NULL)
			{
				long id;
				CComVariant var;
				m_pPropMap->GetNextAssoc(pos, id, var);
				BOOL bCareAboutChange;
				VARIANT_BOOL bOldVal;
				StartSetOrClearProp(id, bCareAboutChange, bOldVal);
				if (bCareAboutChange)	// want change in effect more or less immediately...
					m_pPropMap->RemoveKey(id);
				FinishSetOrClearProp(id, bCareAboutChange, bOldVal, VARIANT_FALSE);
			}
		}
		m_pPropMap->RemoveAll();
		delete m_pPropMap;
		m_pPropMap = NULL;
	}
	return S_OK;
}

STDMETHODIMP CFileConfiguration::GetStagedPropertyContainer(VARIANT_BOOL bCreateIfNecessary, IVCStagedPropertyContainer** ppPropContainer)
{
	CHECK_POINTER_NULL(ppPropContainer);

	if (!m_spStagedContainer)
	{
		if (bCreateIfNecessary)
		{
			HRESULT hr = CVCStagedPropertyContainer::CreateInstance(this, &m_spStagedContainer);
			RETURN_ON_FAIL(hr);
		}
		else
		{
			*ppPropContainer = NULL;
			return S_OK;
		}
	}

	return m_spStagedContainer.CopyTo(ppPropContainer);
}

STDMETHODIMP CFileConfiguration::ReleaseStagedPropertyContainer()		// intended for use ONLY by the staged property container itself
{
	if (m_spStagedContainer)
	{
		m_spStagedContainer->Close();
		m_spStagedContainer.Release();
	}

	return S_OK;
}

// WARNING: Clone must only be called on a brand new config!!
STDMETHODIMP CFileConfiguration::Clone( IVCPropertyContainer *pSource )
{
	// check arg
	CHECK_READ_POINTER_NULL(pSource);

	CFileConfiguration* pIn = (CFileConfiguration*)pSource;

	VARIANT_BOOL bIgnoreDefaultTool = VARIANT_FALSE;
	pIn->get_IgnoreDefaultTool(&bIgnoreDefaultTool);
	if (bIgnoreDefaultTool)		// non-default tool
	{
		CComPtr<IDispatch> spTool;
		pIn->get_Tool(&spTool);
		put_Tool(spTool);
	}

	RETURN_ON_NULL2(pIn->m_pPropMap, S_OK);	// no true local storage

	// if this file cfg doesn't have local storage, create it
	if( !m_pPropMap )
		m_pPropMap = new CVCMap<long,long,CComVariant,VARIANT&>;
	
	// get the start of the map
	VCPOSITION iter = pIn->m_pPropMap->GetStartPosition();
	// iterate over each item in the map
	while( iter )
	{
		long key;
		CComVariant value;

		pIn->m_pPropMap->GetNextAssoc( iter, key, value );

		// copy this element
		m_pPropMap->SetAt( key, value );
	}

	return S_OK;
}

STDMETHODIMP CFileConfiguration::get_VCProjectEngine(IDispatch** ppProjEngine)
{
	CHECK_POINTER_VALID(ppProjEngine);
	RETURN_ON_NULL(g_pProjectEngine);
	return g_pProjectEngine->QueryInterface(IID_IDispatch, (void **)ppProjEngine);
}

STDMETHODIMP CVCGeneralFileConfigSettingsPage::get_ExcludedFromBuild(VARIANT_BOOL *pbExcluded)
{
	HRESULT hr = GetBoolProperty(VCFCFGID_ExcludedFromBuild, pbExcluded);
	if (hr == S_FALSE)
		CFileConfiguration::SGetExcludedFromBuild(pbExcluded);
	return hr;
}

STDMETHODIMP CVCGeneralFileConfigSettingsPage::put_ExcludedFromBuild(VARIANT_BOOL bExcluded)
{
	CHECK_VARIANTBOOL(bExcluded);
	return SetBoolProperty(VCFCFGID_ExcludedFromBuild, bExcluded);
}

STDMETHODIMP CVCGeneralFileConfigSettingsPage::get_Tool(int* pdwTool)
{
	CHECK_POINTER_NULL(pdwTool);
	*pdwTool = m_nCurrentTool;
	return S_OK;
}

STDMETHODIMP CVCGeneralFileConfigSettingsPage::put_Tool(int dwTool)
{
	if (dwTool < 0 || dwTool >= m_cTools)
		RETURN_INVALID();
	m_nCurrentTool = dwTool;
	if (m_pPage)
		m_pPage->Dirty();
	return S_OK;
}

STDMETHODIMP CVCGeneralFileConfigSettingsPage::Commit()
{
	if (m_nCurrentTool != m_nLastTool)
	{
		VSASSERT(!m_bNoFileTools, "Hey, how'd we get multiple tool choices when there's only one choice?");
		CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
		CComQIPtr<IVCPropertyContainer> spLiveContainer;
		if (spStagedContainer)
			spStagedContainer->get_Parent(&spLiveContainer);
		if (spLiveContainer == NULL)
			spLiveContainer = m_pContainer;
		CComQIPtr<VCFileConfiguration> spFileCfg = spLiveContainer;
		CHECK_ZOMBIE(spFileCfg, IDS_ERR_CFG_ZOMBIE);

		int nToolID = m_nCurrentTool;
		if (m_bAllowInherit && nToolID == m_cTools-1)	// last tool in this case is <inherit>
			nToolID = m_nDefaultTool;
		CComQIPtr<IVCToolImpl> spToolImpl = m_rgFileTools.GetAt(nToolID);
		CComQIPtr<IDispatch> spDispTool = spToolImpl;
		HRESULT hr = spFileCfg->put_Tool(spDispTool);
		RETURN_ON_FAIL(hr);
		m_nLastTool = m_nCurrentTool;
		CVCProjectEngine::DoUpdateAfterApply();
	}
	Clear(VCFCFGID_Tool);	// this is only a temporary member of the property container...

	return CBase::Commit();
}

STDMETHODIMP CVCGeneralFileConfigSettingsPage::GetDisplayString(DISPID dispID, BSTR *pBstr)
{
	if (dispID != VCFCFGID_Tool)
		return CBase::GetDisplayString(dispID, pBstr);

	CStringW strCurrentTool = m_strToolNames.GetAt(m_nCurrentTool);
	*pBstr = strCurrentTool.AllocSysString();
	return S_OK;
}

STDMETHODIMP CVCGeneralFileConfigSettingsPage::GetPredefinedStrings(DISPID dispID, CALPOLESTR *pCaStringsOut,
	CADWORD *pCaCookiesOut)
{
	if (dispID != VCFCFGID_Tool)
		return CBase::GetPredefinedStrings(dispID, pCaStringsOut, pCaCookiesOut);

	CHECK_POINTER_NULL(pCaStringsOut);
	CHECK_POINTER_NULL(pCaCookiesOut);

	pCaCookiesOut->cElems = m_cTools;
	pCaCookiesOut->pElems = (DWORD*)CoTaskMemAlloc(m_cTools * sizeof(DWORD));
	RETURN_ON_NULL2(pCaCookiesOut->pElems, E_OUTOFMEMORY);

	pCaStringsOut->cElems = m_cTools;
	pCaStringsOut->pElems = (LPOLESTR*)CoTaskMemAlloc(m_cTools * sizeof(LPOLESTR));
	RETURN_ON_NULL2(pCaStringsOut->pElems, E_OUTOFMEMORY);

	int idx;
	for (idx = 0; idx < m_cTools; idx++)
		pCaCookiesOut->pElems[idx] = idx;

	for (idx = 0; idx < m_cTools; idx++)
	{
		CStringW strToolName = m_strToolNames.GetAt(idx);
		CComBSTR bstrToolName = strToolName;
		pCaStringsOut->pElems[idx] = (LPOLESTR)CoTaskMemAlloc((bstrToolName.Length()+1) * sizeof(OLECHAR));
		wcscpy(pCaStringsOut->pElems[idx], bstrToolName);
	}

	return S_OK;
}

STDMETHODIMP CVCGeneralFileConfigSettingsPage::GetPredefinedValue(DISPID dispID, DWORD dwCookie, VARIANT* pVarOut)
{
	if (dispID != VCFCFGID_Tool)
		return CBase::GetPredefinedValue(dispID, dwCookie, pVarOut);

	if (dwCookie < 0 || (int)dwCookie >= m_cTools)
		RETURN_INVALID();

	CHECK_POINTER_NULL(pVarOut);
	CComVariant var = dwCookie;
	var.Detach(pVarOut);
	return S_OK;
}

BOOL CVCGeneralFileConfigSettingsPage::IsDefaultTool(int nToolID)
{
	if (m_bAllowInherit && nToolID == m_cTools-1)	// last tool is inherit
		return FALSE;
	CComQIPtr<IVCToolImpl> spToolImpl = m_rgFileTools.GetAt(nToolID);
	if (spToolImpl)
	{
		CComBSTR bstrExt;
		HRESULT hr = spToolImpl->get_DefaultExtensions(&bstrExt);
		if (SUCCEEDED(hr))
		{
			CStringW strExtensions = bstrExt;
			if (!strExtensions.IsEmpty() && strExtensions.Find(m_strFileExt) >= 0)
				return TRUE;
		}
	}

	return FALSE;
}

STDMETHODIMP CVCGeneralFileConfigSettingsPage::HasDefaultValue(DISPID dispID, BOOL *pfDefault)
{
	if (dispID != VCFCFGID_Tool)
		return CBase::HasDefaultValue(dispID, pfDefault);

	CHECK_POINTER_NULL(pfDefault);
	*pfDefault = IsDefaultTool(m_nCurrentTool);
	return S_OK;
}

void CVCGeneralFileConfigSettingsPage::ResetParentIfNeeded()
{
	m_rgFileTools.RemoveAll();
	m_strToolNames.RemoveAll();
	m_nLastTool = 0;
	m_nCurrentTool = 0;
	m_cTools = 0;
	m_bNoFileTools = false;

	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	CComQIPtr<IVCPropertyContainer> spLiveContainer;
	if (spStagedContainer)
		spStagedContainer->get_Parent(&spLiveContainer);
	if (spLiveContainer == NULL)
		spLiveContainer = m_pContainer;
	CComQIPtr<VCFileConfiguration> spFileCfg = spLiveContainer;
	if (spFileCfg == NULL)
	{
		VSASSERT(FALSE, "Hey, bad parent for a file config page!");
		return;
	}

	CComPtr<IDispatch> spDispFile;
	spFileCfg->get_Parent(&spDispFile);
	CComQIPtr<VCFile> spFile = spDispFile;
	if (spFile)
	{
		CComBSTR bstrFile;
		spFile->get_Name(&bstrFile);
		CStringW strFile = bstrFile;
		int i = strFile.ReverseFind(L'.');
		if (i > 0)
			m_strFileExt = strFile.Mid(i);
		m_strFileExt.MakeLower();
	}

	CComPtr<IDispatch> spDispTool;
	spFileCfg->get_Tool(&spDispTool);
	CComQIPtr<IVCToolImpl> spTool = spDispTool;
	CStringW strToolName;
	if (spTool != NULL)
	{
		CComBSTR bstrToolName;
		spTool->get_ToolNameInternal(&bstrToolName);
		strToolName = bstrToolName;
	}

	CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = spFileCfg;
	if (spFileCfgImpl == NULL)
	{
		VSASSERT(FALSE, "Hey, bad parent for a file config page!");
		return;
	}

	VARIANT_BOOL bIgnoreDefaultTool = VARIANT_FALSE;
	spFileCfgImpl->get_IgnoreDefaultTool(&bIgnoreDefaultTool);
	m_bAllowInherit = (bIgnoreDefaultTool == VARIANT_TRUE);	

	CComPtr<IDispatch> spDispProjCfg;
	spFileCfgImpl->get_Configuration(&spDispProjCfg);
	CComQIPtr<VCConfiguration> spProjCfg = spDispProjCfg;
	if (spProjCfg == NULL)
	{
		VSASSERT(FALSE, "Hey, bad parent for file config page!");
		return;
	}

	CComQIPtr<IVCCollection> spFileTools;
	spProjCfg->get_FileTools(&spFileTools);
	if (spFileTools == NULL)
	{
		VSASSERT(FALSE, "Hey, no file tools!");
		return;
	}
	CComPtr<IEnumVARIANT> spEnum;
	spFileTools->_NewEnum(reinterpret_cast<IUnknown **>(&spEnum));
	if (spEnum == NULL)
	{
		VSASSERT(FALSE, "Hey, no file tools!");
		return;
	}
	spEnum->Reset();

	m_cTools = 0;
	BOOL bFoundLast = FALSE;
	while (TRUE)
	{
		CComVariant var;
		HRESULT hr = spEnum->Next(1, &var, NULL);
		if (hr != S_OK)
			break;
		if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
			continue;
		IDispatch* pDispTool = var.pdispVal;
		CComQIPtr<IVCToolImpl> spToolImpl = pDispTool;
		if (spToolImpl == NULL)
			continue;
		CComBSTR bstrFileTool;
		spToolImpl->get_ToolNameInternal(&bstrFileTool);
		CStringW strFileTool = bstrFileTool;
		if (strFileTool.IsEmpty())
			continue;
		// these two stay in sync only because we aren't going to be doing any removes from the tool list...
		m_rgFileTools.Add(spToolImpl);
		m_strToolNames.Add(strFileTool);
		if (!bFoundLast)
		{
			bFoundLast = (strFileTool == strToolName);
			if (bFoundLast)
				m_nLastTool = m_cTools;
		}
		m_cTools++;
	}
	m_nCurrentTool = m_nLastTool;

	m_bNoFileTools = (m_cTools == 0);
	if (m_bNoFileTools)	// no file tools, such as for makefile or utility projects
	{
		m_cTools = 1;
		CStringW strNoTool;
		strNoTool.LoadString(IDS_NOTOOL);
		m_strToolNames.Add(strNoTool);
	}

	m_nDefaultTool = -1;
	if (m_bAllowInherit)
	{
		for (int idx = 0; idx < m_cTools; idx++)
		{
			if (IsDefaultTool(idx))
			{
				m_nDefaultTool = idx;
				break;
			}
		}
		if (m_nDefaultTool < 0)	// must be custom tool as default
		{
			for (int idx = 0; idx < m_cTools; idx++)
			{
				CComQIPtr<IVCToolImpl> spToolImpl = m_rgFileTools.GetAt(idx);
				if (spToolImpl == NULL)
					continue;
				VARIANT_BOOL bIsCustom = VARIANT_FALSE;
				spToolImpl->get_IsCustomBuildTool(&bIsCustom);
				if (bIsCustom)	// only gonna be one custom tool for files...
				{
					m_nDefaultTool = idx;
					break;
				}
			}
		}
		CStringW strInherit;
		strInherit = L"<inherit setting>";	// REVIEW: does this need to be localized?
		m_strToolNames.Add(strInherit);
		m_cTools++;
	}
}


