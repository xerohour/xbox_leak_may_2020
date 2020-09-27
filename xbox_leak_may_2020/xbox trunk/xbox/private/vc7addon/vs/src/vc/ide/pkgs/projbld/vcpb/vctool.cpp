#include "stdafx.h"
#include "VCTool.h"
#include "CmdLines.h"
#include "BldActions.h"
#include "scanner.h"
#include "BuildEngine.h"
#include "ProjWriter.h"
#include "stylesheet.h"

BOOL CVCToolImpl::ConfigBuildProp(long idProp)
{
	switch(idProp)
	{
	case VCCFGID_IntermediateDirectory:
	case VCCFGID_OutputDirectory:
		return TRUE;	// the case statements above here need to match those for the override ID VCCFGID_AllOutputsDirty in GetOverrideID
	default:
		return FALSE;
	};
}

STDMETHODIMP CVCToolImpl::GetAdditionalIncludeDirectoriesInternal(IVCPropertyContainer* pPropContainer, BSTR* pbstrIncPaths)
{
	if (pbstrIncPaths != NULL)
		*pbstrIncPaths = NULL;
	return S_FALSE;
}

STDMETHODIMP CVCToolImpl::get_IncludePathID(long* pnIncludeID)
{
	CHECK_POINTER_NULL(pnIncludeID);
	*pnIncludeID = 0;
	return S_FALSE;
}

STDMETHODIMP CVCToolImpl::SaveObject(IStream *xml, IVCPropertyContainer *pPropContainer, long nIndent)
{
	VSASSERT(pPropContainer, "Property container required in order to save tool properties");
	CHECK_READ_POINTER_NULL(pPropContainer);
	CHECK_READ_POINTER_NULL(xml);

	COptionHandlerBase* pOptionHandler = GetOptionHandler();
	RETURN_ON_NULL2(pOptionHandler, E_UNEXPECTED);

	HRESULT hr = S_OK;

	CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = pPropContainer;
	bool bWriteProps = false;
 	if (spFileCfgImpl != NULL)	 // need to figure out whether we need to set what tool to use or can just set properties for it
 	{
 		VARIANT_BOOL bIgnoreDefaultTool = VARIANT_FALSE;
 		if (SUCCEEDED(spFileCfgImpl->get_IgnoreDefaultTool(&bIgnoreDefaultTool)) && bIgnoreDefaultTool == VARIANT_TRUE)
			bWriteProps = true;
 	}

 	if (!bWriteProps)
 	{
 		VARIANT_BOOL bHaveLocalStorage = VARIANT_FALSE;
		pPropContainer->HasLocalStorage(VARIANT_TRUE /*for save*/, &bHaveLocalStorage);	 // real cheap way to know we've got nothing going on...
		if ( bHaveLocalStorage == VARIANT_FALSE )
			return S_OK;
	}
	StartNodeHeader( xml, L"Tool", false );

	// Name
	CComBSTR bstrToolShortName;
	get_ToolShortName(&bstrToolShortName);
	if (bstrToolShortName.Length() > 0)
	{
		NodeAttribute( xml, L"Name", bstrToolShortName ); 					
	}

	// Now allow the tool to inject anything it needs that isn't in the prop table
	SaveSpecialProps(xml, pPropContainer);

	SOptionEntry * pOptTableEntry;
	
	// Additional Options
	if (pOptionHandler->SupportsAdditionalOptions())
	{
		CComBSTR bstrAdditionalOptions;
		if (GetAdditionalOptionsInternal(pPropContainer, VARIANT_FALSE, VARIANT_FALSE, &bstrAdditionalOptions) == S_OK)
		{
			NodeAttributeWithSpecialChars( xml, L"AdditionalOptions", bstrAdditionalOptions );
		}
	}

	// Additional Dependencies
	CComBSTR bstrAdditionalDependencies;
	if (GetAdditionalDependenciesInternal(pPropContainer, NULL, TRUE, &bstrAdditionalDependencies) == S_OK)
	{
		NodeAttributeWithSpecialChars( xml, L"AdditionalDependencies", bstrAdditionalDependencies );
	}

	// options from property container
	pOptTableEntry = pOptionHandler->GetOptionTable();
	if (pOptTableEntry)
	{
		CStringW strOption;
		HRESULT hr = S_OK;

		for (; pOptTableEntry->entryType != stdOptEnd && SUCCEEDED(hr); pOptTableEntry++)
		{
			bool bString = false;
			CComVariant var;
			hr = pPropContainer->GetLocalProp(pOptTableEntry->idOption, &var);
			if (hr == S_FALSE || FAILED(hr))
				continue;
			switch (pOptTableEntry->entryType)
			{
			case stdOptEnum:
				{
				// ChangeType() in the Invoke() call will handle this for us
// 				CComQIPtr<IDispatch> pDisp;
// 				pDisp = static_cast<IUnknown *>(this);
// 				GetEnumVal(pPropContainer, pOptTableEntry, var, pDisp);
 				}
				break;
			case stdOptBstr:			// option is a string (quote it)
			case specOptBstr:		// option is a string with special handling required
				bString = true;	// put quotes around it...
				break;

			case stdOptBool:
				{
				CComBSTR bstrBool = var.boolVal == VARIANT_TRUE ? L"TRUE" : L"FALSE";
				var = bstrBool;
				}
				break;

			case stdOptBstrNoQuote:	// option is a string (don't quote it)
				break;

			}
			var.ChangeType(VT_BSTR);
			// TODO: these names should be bstr to start with
			CComBSTR bstrPropName = pOptTableEntry->szOptionName; 
// 			if( bString )
// 			{
// 				// prepend "
// 				CComBSTR bstrPropVal = L"&quot;";
// 				bstrPropVal += var.bstrVal;
// 				// append "
// 				bstrPropVal += L"&quot;";
//  				NodeAttribute( xml, bstrPropName, bstrPropVal );
// 			} 				
// 			else
			{
				NodeAttributeWithSpecialChars( xml, bstrPropName, var.bstrVal );
			} 				
		}
		hr = S_OK;
	}
	// end of node header
	EndNodeHeader( xml, false );
	// end of node
	EndNode( xml, L"Tool", false );
	return hr;
}

HRESULT CVCToolImpl::GetCommandLineOptions(IUnknown* pItem, IVCBuildAction* pAction, VARIANT_BOOL bIncludeAdditional, 
	VARIANT_BOOL bForDisplay, commandLineOptionStyle fStyle, CStringW& rstrCmdLine)
{
	CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
	RETURN_ON_NULL2(spPropContainer, E_NOINTERFACE);

	COptionHandlerBase* pOptionHandler = GetOptionHandler();
	RETURN_ON_NULL2(pOptionHandler, E_NOTIMPL);

	CComBSTR bstrProp;
	CComBSTR bstrOut;
	CStringW strOption;
	HRESULT hr = S_OK;

	BOOL bCForDisplay = (bForDisplay == VARIANT_TRUE);
	hr = pOptionHandler->GenerateCommandLine(spPropContainer, bCForDisplay, fStyle, strOption);
	RETURN_ON_FAIL(hr);
	pOptionHandler->AdvanceCommandLine(rstrCmdLine, strOption);

	if (pOptionHandler->SupportsAdditionalOptions() && bIncludeAdditional)
	{
		CComBSTR bstrOptions;
		hr = GetAdditionalOptionsInternal(pItem, VARIANT_TRUE, VARIANT_FALSE, &bstrOptions);
		if (hr == S_OK)
		{
			spPropContainer->Evaluate(bstrOptions, &bstrOptions);
			strOption = bstrOptions;
			pOptionHandler->AdvanceCommandLine(rstrCmdLine, strOption);
		}
		if (hr == S_FALSE)	// don't override a general S_OK with a single S_FALSE at the end here...
			hr = S_OK;
	}

	CComBSTR bstrAdditionalDependencies;
	if (GetAdditionalDependenciesInternal(spPropContainer, pAction, FALSE, &bstrAdditionalDependencies) == S_OK)
	{
		CComBSTR bstrDependenciesOut;
		spPropContainer->Evaluate(bstrAdditionalDependencies, &bstrDependenciesOut);
		strOption = bstrDependenciesOut;
		pOptionHandler->AdvanceCommandLine(rstrCmdLine, strOption);
	}

	if (fStyle == cmdLineForDisplay)
	{
		CStringW strTrailing;
		pOptionHandler->GetTrailingCommandLinePart(strTrailing);
		if (!strTrailing.IsEmpty())
			strTrailing = L"\r\n\r\n" + strTrailing;
		pOptionHandler->AdvanceCommandLine(rstrCmdLine, strTrailing);
	}

	return hr;
}

STDMETHODIMP CVCToolImpl::GetCommandLineOptions(IUnknown* pItem, IVCBuildAction* pAction, VARIANT_BOOL bIncludeAdditional, 
	commandLineOptionStyle fStyle, BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	*pVal = NULL;

	CStringW strVal;
	HRESULT hr = GetCommandLineOptions(pItem, pAction, bIncludeAdditional, VARIANT_FALSE /* !for display */, fStyle, strVal);
	if (SUCCEEDED(hr))
		*pVal = strVal.AllocSysString();

	return hr;
}

STDMETHODIMP CVCToolImpl::GetCommandLineOptionsForDisplay(IUnknown* pItem, VARIANT_BOOL bIncludeAdditional, BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	*pVal = NULL;

	CStringW strVal;
	// DIANEME_TODO: CVCToolImpl::GetCommandLineOptionsForDisplay - turn the for display parameter ON
	HRESULT hr = GetCommandLineOptions(pItem, NULL, bIncludeAdditional, VARIANT_FALSE /* !for display (temporary) */, cmdLineForDisplay, strVal);
	if (SUCCEEDED(hr))
		*pVal = strVal.AllocSysString();

	return hr;
}

STDMETHODIMP CVCToolImpl::GetCommandLineEx(IVCBuildAction*pAction,IVCBuildableItem* pBuildableItem, 
	IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, BSTR *pVal)
{
	CHECK_READ_POINTER_NULL(pAction);
	CHECK_READ_POINTER_NULL(pBuildableItem);

	CHECK_POINTER_NULL(pVal);
	*pVal = NULL;

	CComBSTR bstrMidLine;
	HRESULT hr = GetCommandLineOptions(pBuildableItem, pAction, VARIANT_TRUE /* include additional options */, cmdLineForBuild, &bstrMidLine);
	RETURN_ON_FAIL(hr);

	COptionHandlerBase* pOptionHandler = GetOptionHandler();
	RETURN_ON_NULL2(pOptionHandler, E_NOTIMPL);

	CComBSTR bstrProp;
	hr = get_ToolPathInternal(&bstrProp);	// safe since we don't change the tool path on a per file cfg basis...
	RETURN_ON_FAIL(hr);

	CComQIPtr<IVCPropertyContainer> spPropContainer = pBuildableItem;
	RETURN_INVALID_ON_NULL(spPropContainer);
	CComBSTR bstrToolExe;
	spPropContainer->Evaluate(bstrProp, &bstrToolExe);

	CStringW strOption, strCmdLine;
	CComQIPtr<IVCBuildEngine> spUseBuildEngine;
	if (UsesResponseFiles())
		spUseBuildEngine = pBuildEngine;
	if (spUseBuildEngine == NULL)
	{
		strOption = bstrToolExe;
		pOptionHandler->AdvanceCommandLine(strCmdLine, strOption);
	}
	strOption = bstrMidLine;
	pOptionHandler->AdvanceCommandLine(strCmdLine, strOption);
	CComBSTR bstrProp2;
	BOOL bUsesRSP = UsesResponseFiles();
	pAction->GetRelativePathsToInputs(TRUE, bUsesRSP, pEC, &bstrProp2);
	strOption = bstrProp2;
	pOptionHandler->AdvanceCommandLine(strCmdLine, strOption, bUsesRSP ? L"\n" : L" ");
	if (spUseBuildEngine == NULL)
	{
		if (pBuildEngine)
		{
			CStringW strCommand;
			strCommand.Format(IDS_CREATING_COMMANDLINE, strCmdLine);
			CComBSTR bstrCommand = strCommand;
			pBuildEngine->LogTrace(eLogCommand, bstrCommand);
		}
		*pVal = strCmdLine.AllocSysString();
		return S_OK;
	}

	CComBSTR bstrCmdLineIn = strCmdLine;
	return spUseBuildEngine->FormCommandLine(bstrToolExe, bstrCmdLineIn, pEC, bUsesRSP, TRUE, pVal);
}

// by default tools don't do this (makefile tool is the exception)
STDMETHODIMP CVCToolImpl::GetCleanCommandLineEx(IVCBuildAction*pAction,IVCBuildableItem* pBuildableItem, 
	IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	pVal = NULL;
	return E_NOTIMPL;
}

// by default tools don't do this (makefile tool is the exception)
STDMETHODIMP CVCToolImpl::GetRebuildCommandLineEx(IVCBuildAction*pAction,IVCBuildableItem* pBuildableItem, 
	IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	pVal = NULL;
	return E_NOTIMPL;
}

STDMETHODIMP CVCToolImpl::GetPages(CAUUID* pPages)
{
	CHECK_POINTER_NULL(pPages);
		
	// normally these would be CoTaskMemAlloced, but the caller of this is going to 
	// merge the set of pages and do an alloc later, so we don't have to.
	pPages->cElems = GetPageCount();
	pPages->pElems = GetPageIDs();
	return S_OK;
}

STDMETHODIMP CVCToolImpl::GetPropertyOptionInternal(IUnknown* pItem, BSTR bstrProp, long dispidProp, BSTR *pVal)
{
	CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
	RETURN_ON_NULL2(spPropContainer, E_NOINTERFACE);

	CHECK_POINTER_NULL(pVal);
	*pVal = NULL;

	HRESULT hr = S_OK;
	COptionHandlerBase* pOptionHandler = GetOptionHandler();
	RETURN_ON_NULL2(pOptionHandler, E_NOTIMPL);

	CStringW strSwitch;
	hr = pOptionHandler->FindSwitch(spPropContainer, bstrProp, dispidProp, strSwitch);
	RETURN_ON_FAIL(hr);
	*pVal = strSwitch.AllocSysString();
	return hr;
}

STDMETHODIMP CVCToolImpl::IsTargetTool(IVCBuildAction* pAction, VARIANT_BOOL* pbIsTargetTool)
{ 
	return COptionHandlerBase::GetValueFalse(pbIsTargetTool);	// by default, not a target tool
}

STDMETHODIMP CVCToolImpl::get_PerformIfAble(VARIANT_BOOL* pbPerformIfAble)
{ 
	return COptionHandlerBase::GetValueFalse(pbPerformIfAble);	// by default, stop on error
}

STDMETHODIMP CVCToolImpl::get_IsComspecTool(VARIANT_BOOL* pbIsComspecTool)
{ 
	return COptionHandlerBase::GetValueFalse(pbIsComspecTool);	// by default, doesn't require comspec
}

STDMETHODIMP CVCToolImpl::get_IsCustomBuildTool(VARIANT_BOOL* pbIsCustom)
{
	return get_IsComspecTool(pbIsCustom);	// so far, all custom tools are comspec tools...
}

HRESULT CVCToolImpl::GetAdditionalDependenciesInternal(IVCPropertyContainer* pItem, IVCBuildAction* pAction, BOOL bForSave, 
	BSTR* pbstrInputs)
{
	CHECK_ZOMBIE(pItem, IDS_ERR_TOOL_ZOMBIE);
	CHECK_POINTER_NULL(pbstrInputs);
	*pbstrInputs = NULL;	// by default, doesn't have additional inputs
	return S_FALSE;
}

STDMETHODIMP CVCToolImpl::PrePerformBuildActions(bldActionTypes type, IVCBuildActionList* pActions, 
	bldAttributes aob, IVCBuildErrorContext* pEC, actReturnStatus* pActReturn)
{
	CHECK_POINTER_NULL(pActReturn);
	*pActReturn = ACT_Complete;
	return S_OK;
}

STDMETHODIMP CVCToolImpl::PostPerformBuildActions(bldActionTypes type, IVCBuildActionList* pActions, 
	bldAttributes aob, IVCBuildErrorContext* pEC, actReturnStatus* pActReturn)
{
	return S_OK;
}

STDMETHODIMP CVCToolImpl::IsDeleteOnRebuildFile(LPCOLESTR szFile, VARIANT_BOOL* pbDelOnRebuild)
{
	return COptionHandlerBase::GetValueTrue(pbDelOnRebuild);
}

STDMETHODIMP CVCToolImpl::GetCommandLinesForBuild(IVCBuildActionList* pActions, bldAttributes attrib, 
	IVCBuildEngine* pBldEngine, IVCBuildErrorContext* pErrorContext, IVCCommandLineList** ppCmds)
{
	CHECK_POINTER_NULL(ppCmds);
	*ppCmds = NULL;

	RETURN_ON_NULL2(pActions, S_FALSE);

	// generate standard command-line
	HRESULT hr = S_OK;
	BOOL bFirstTime = TRUE;
	pActions->Reset(NULL);
	while (hr == S_OK)
	{
		CComPtr<IVCBuildAction> spAction;
		hr = pActions->Next(&spAction, NULL);
		BREAK_ON_DONE(hr);
		ASSERT_AND_CONTINUE_ON_NULL(spAction);

		CComPtr<IVCBuildableItem> spBuildableItem;
		HRESULT hr1 = spAction->get_Item(&spBuildableItem);
		ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr1, spBuildableItem);
		if (bFirstTime)
		{
			bFirstTime = FALSE;
			CComPtr<VCConfiguration> spProjCfg;
			hr1 = spAction->get_ProjectConfiguration(&spProjCfg);
			ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr1, spProjCfg);

			if (!HaveCommandLine(spAction))
				continue;	// nothing whatsoever to do if there's nothing there to build...
		}

		VARIANT_BOOL bIsTargetTool = VARIANT_FALSE;
		IsTargetTool(spAction, &bIsTargetTool);

		// this keeps the non-schmooze tools off the schmooze list...
		CComQIPtr<VCFileConfiguration> spFileCfg = spBuildableItem;
		if (bIsTargetTool == VARIANT_TRUE && spFileCfg != NULL)	// schmooze tool on file
			continue;
		else if (bIsTargetTool == VARIANT_FALSE && spFileCfg == NULL)	// non-schmooze tool on cfg
			continue;

		CComBSTR bstrCmd;
		hr1 = GetCommandLineEx(spAction, spBuildableItem, pBldEngine, pErrorContext, &bstrCmd);
		ASSERT_AND_CONTINUE_ON_FAIL(hr1);

		CComBSTR bstrDescription;
		hr1 = GetBuildDescription(spAction, &bstrDescription);
		ASSERT_AND_CONTINUE_ON_FAIL(hr1);

		if (*ppCmds == NULL)
		{
			hr1 = CVCCommandLineList::CreateInstance(ppCmds);
			ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr1, *ppCmds);
			VARIANT_BOOL bIsComspec = VARIANT_FALSE;
			if (SUCCEEDED(get_IsComspecTool(&bIsComspec)))
				(*ppCmds)->put_UseConsoleCodePageForSpawner(bIsComspec);
		}
		CComPtr<IVCCommandLine> spCmdLine;
		hr1 = CVCCommandLine::CreateInstance(&spCmdLine);
		ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr, spCmdLine);

		spCmdLine->put_CommandLineContents(bstrCmd);
		spCmdLine->put_Description(bstrDescription);
		spCmdLine->put_NumberOfProcessors(1);

		(*ppCmds)->Add(spCmdLine, FALSE);
	}

	return S_OK;
}

STDMETHODIMP CVCToolImpl::GetCommandLinesForClean(IVCBuildActionList* pActions, bldAttributes attrib, 
	IVCBuildEngine* pBldEngine, IVCBuildErrorContext* pErrorContext, IVCCommandLineList** ppCmds)
{
	CHECK_POINTER_NULL(ppCmds);
	*ppCmds = NULL;

	RETURN_ON_NULL2(pActions, S_FALSE);

	// generate standard command-line
	HRESULT hr = S_OK;
	BOOL bFirstTime = TRUE;
	pActions->Reset(NULL);
	while (hr == S_OK)
	{
		CComPtr<IVCBuildAction> spAction;
		hr = pActions->Next(&spAction, NULL);
		BREAK_ON_DONE(hr);
		ASSERT_AND_CONTINUE_ON_NULL(spAction);

		CComPtr<IVCBuildableItem> spBuildableItem;
		HRESULT hr1 = spAction->get_Item(&spBuildableItem);
		ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr1, spBuildableItem);
		if (bFirstTime)
		{
			bFirstTime = FALSE;
			CComPtr<VCConfiguration> spProjCfg;
			hr1 = spAction->get_ProjectConfiguration(&spProjCfg);
			ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr1, spProjCfg);

			if (!HaveCommandLine(spAction))
				continue;	// nothing whatsoever to do if there's nothing there to build...
		}

		VARIANT_BOOL bIsTargetTool = VARIANT_FALSE;
		IsTargetTool(spAction, &bIsTargetTool);

		// this keeps the non-schmooze tools off the schmooze list...
		CComQIPtr<VCFileConfiguration> spFileCfg = spBuildableItem;
		if (bIsTargetTool == VARIANT_TRUE && spFileCfg != NULL)	// schmooze tool on file
			continue;
		else if (bIsTargetTool == VARIANT_FALSE && spFileCfg == NULL)	// non-schmooze tool on cfg
			continue;

		CComBSTR bstrCmd;
		hr1 = GetCleanCommandLineEx(spAction, spBuildableItem, pBldEngine, pErrorContext, &bstrCmd);
		ASSERT_AND_CONTINUE_ON_FAIL(hr1);

		CComBSTR bstrDescription;
		hr1 = GetBuildDescription(spAction, &bstrDescription);
		ASSERT_AND_CONTINUE_ON_FAIL(hr1);

		if (*ppCmds == NULL)
		{
			hr1 = CVCCommandLineList::CreateInstance(ppCmds);
			ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr1, *ppCmds);
			VARIANT_BOOL bIsComspec = VARIANT_FALSE;
			if (SUCCEEDED(get_IsComspecTool(&bIsComspec)))
				(*ppCmds)->put_UseConsoleCodePageForSpawner(bIsComspec);
		}
		CComPtr<IVCCommandLine> spCmdLine;
		hr1 = CVCCommandLine::CreateInstance(&spCmdLine);
		ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr, spCmdLine);

		spCmdLine->put_CommandLineContents(bstrCmd);
		spCmdLine->put_Description(bstrDescription);
		spCmdLine->put_NumberOfProcessors(1);

		(*ppCmds)->Add(spCmdLine, FALSE);
	}

	return S_OK;
}

STDMETHODIMP CVCToolImpl::GetCommandLinesForRebuild(IVCBuildActionList* pActions, bldAttributes attrib, 
	IVCBuildEngine* pBldEngine, IVCBuildErrorContext* pErrorContext, IVCCommandLineList** ppCmds)
{
	CHECK_POINTER_NULL(ppCmds);
	*ppCmds = NULL;

	RETURN_ON_NULL2(pActions, S_FALSE);

	// generate standard command-line
	HRESULT hr = S_OK;
	BOOL bFirstTime = TRUE;
	pActions->Reset(NULL);
	while (hr == S_OK)
	{
		CComPtr<IVCBuildAction> spAction;
		hr = pActions->Next(&spAction, NULL);
		BREAK_ON_DONE(hr);
		ASSERT_AND_CONTINUE_ON_NULL(spAction);

		CComPtr<IVCBuildableItem> spBuildableItem;
		HRESULT hr1 = spAction->get_Item(&spBuildableItem);
		ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr1, spBuildableItem);
		if (bFirstTime)
		{
			bFirstTime = FALSE;
			CComPtr<VCConfiguration> spProjCfg;
			hr1 = spAction->get_ProjectConfiguration(&spProjCfg);
			ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr1, spProjCfg);

			if (!HaveCommandLine(spAction))
				continue;	// nothing whatsoever to do if there's nothing there to build...
		}

		VARIANT_BOOL bIsTargetTool = VARIANT_FALSE;
		IsTargetTool(spAction, &bIsTargetTool);

		// this keeps the non-schmooze tools off the schmooze list...
		CComQIPtr<VCFileConfiguration> spFileCfg = spBuildableItem;
		if (bIsTargetTool == VARIANT_TRUE && spFileCfg != NULL)	// schmooze tool on file
			continue;
		else if (bIsTargetTool == VARIANT_FALSE && spFileCfg == NULL)	// non-schmooze tool on cfg
			continue;

		CComBSTR bstrCmd;
		hr1 = GetRebuildCommandLineEx(spAction, spBuildableItem, pBldEngine, pErrorContext, &bstrCmd);
		ASSERT_AND_CONTINUE_ON_FAIL(hr1);

		CComBSTR bstrDescription;
		hr1 = GetBuildDescription(spAction, &bstrDescription);
		ASSERT_AND_CONTINUE_ON_FAIL(hr1);

		if (*ppCmds == NULL)
		{
			hr1 = CVCCommandLineList::CreateInstance(ppCmds);
			ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr1, *ppCmds);
			VARIANT_BOOL bIsComspec = VARIANT_FALSE;
			if (SUCCEEDED(get_IsComspecTool(&bIsComspec)))
				(*ppCmds)->put_UseConsoleCodePageForSpawner(bIsComspec);
		}
		CComPtr<IVCCommandLine> spCmdLine;
		hr1 = CVCCommandLine::CreateInstance(&spCmdLine);
		ASSERT_AND_CONTINUE_ON_FAIL_OR_NULL(hr, spCmdLine);

		spCmdLine->put_CommandLineContents(bstrCmd);
		spCmdLine->put_Description(bstrDescription);
		spCmdLine->put_NumberOfProcessors(1);

		(*ppCmds)->Add(spCmdLine, FALSE);
	}

	return S_OK;
}
STDMETHODIMP CVCToolImpl::IsSpecialConsumable(LPCOLESTR szPath, VARIANT_BOOL* pbSpecial)
{
	return COptionHandlerBase::GetValueFalse(pbSpecial);	// by default, nothing is a special consumable
}


STDMETHODIMP CVCToolImpl::GenerateOutput(long type, IVCBuildActionList* pActions, IVCBuildErrorContext* pEC)
{
	// meant to be overridden
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// Affects our output?
STDMETHODIMP CVCToolImpl::AffectsOutput(long nPropID, VARIANT_BOOL* pbAffectsOutput)
{
	// meant to be overridden
	CHECK_POINTER_NULL(pbAffectsOutput);
	*pbAffectsOutput = VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP CVCToolImpl::HasPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput)
{ 
	return COptionHandlerBase::GetValueFalse(pbHasPrimaryOutput);	// by default, not primary output generator
}

STDMETHODIMP CVCToolImpl::GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, BSTR* pbstrPath)
{ 
	CHECK_POINTER_NULL(pbstrPath);
	*pbstrPath = NULL;

	CPathW path;
	HRESULT hr = GetPrimaryOutputFromTool(pItem, bSchmoozeOnly, path);
	if (hr != S_OK)
		return hr;
	path.GetActualCase(TRUE);

	CStringW strPath = path;
	if (!strPath.IsEmpty())
		*pbstrPath = strPath.AllocSysString();
	return S_OK;
}

STDMETHODIMP CVCToolImpl::GetPrimaryOutputIDFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID)
{
	// meant to be overridden
	CHECK_POINTER_NULL(pnOutputID);
	*pnOutputID = -1;

	return S_FALSE;
}

STDMETHODIMP CVCToolImpl::GetImportLibraryFromTool(IUnknown* pItem, BSTR* pbstrPath)
{
	CHECK_POINTER_NULL(pbstrPath);
	*pbstrPath = NULL;
	return S_FALSE;
}

STDMETHODIMP CVCToolImpl::GetDefaultOutputFileFormat(IUnknown* pItem, BSTR* pbstrOutMacro)
{	// macro to use for generating default output file if the original default isn't unique
	// meant to be overridden
	CHECK_POINTER_NULL(pbstrOutMacro);
	*pbstrOutMacro = NULL;

	return S_FALSE;
}

STDMETHODIMP CVCToolImpl::GetProgramDatabaseFromTool(IVCPropertyContainer* pContainer, VARIANT_BOOL bSchmoozeOnly, BSTR* pbstrPDB)
{
	CHECK_POINTER_NULL(pbstrPDB);
	*pbstrPDB = NULL;
	return S_FALSE;
}

STDMETHODIMP CVCToolImpl::SetPrimaryOutputForTool(IUnknown* pItem, BSTR bstrFile)
{	// sets the primary output for a tool
	return S_FALSE;		// by default, you can't do this; override for tools that want to
}

STDMETHODIMP CVCToolImpl::get_IsFileTool(VARIANT_BOOL* pbIsFileTool)
{
	CHECK_POINTER_NULL(pbIsFileTool);

	// in general, anything that isn't a primary output tool is a file tool (includes custom tool)
	VARIANT_BOOL bIsTargetTool = VARIANT_FALSE;
	HasPrimaryOutputFromTool(NULL, VARIANT_TRUE, &bIsTargetTool);
	if (bIsTargetTool == VARIANT_TRUE)
		*pbIsFileTool = VARIANT_FALSE;
	else
		*pbIsFileTool = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CVCToolImpl::HasDependencies(IVCBuildAction* pAction, VARIANT_BOOL* pbHasDependencies)
{
	return COptionHandlerBase::GetValueFalse(pbHasDependencies);	// by default, has no dependencies
}

STDMETHODIMP CVCToolImpl::GetDependencies(IVCBuildAction* pAction, IVCBuildStringCollection** ppStrings, 
	VARIANT_BOOL* pbUpToDate)
{
	CHECK_POINTER_NULL(ppStrings);
	*ppStrings = NULL;
	return S_FALSE;
}

STDMETHODIMP CVCToolImpl::CanScanForDependencies(VARIANT_BOOL* pbIsScannable)
{
	return COptionHandlerBase::GetValueFalse(pbIsScannable);	// by default, the tool can't scan for dependencies
}

STDMETHODIMP CVCToolImpl::ScanDependencies(IVCBuildAction* pAction, IVCBuildErrorContext* pEC, VARIANT_BOOL bWriteOutput)
{
	RETURN_INVALID_ON_NULL(pAction);
	return pAction->ScanToolDependencies(this, pEC, bWriteOutput);
}

STDMETHODIMP CVCToolImpl::GetDeploymentDependencies(IVCBuildAction* pAction, IVCBuildStringCollection** ppDeployDepStrings)
{
	return S_FALSE;	// by default, tools don't have deployment dependencies
}

STDMETHODIMP CVCToolImpl::GetDeployableOutputsCount(IVCPropertyContainer* pPropContainer, long* pnOutputs)
{	// number of deployable outputs
	CHECK_POINTER_NULL(pnOutputs);
	*pnOutputs = 0;

	VARIANT_BOOL bHasPrimaryOutput = VARIANT_FALSE;
	if (SUCCEEDED(HasPrimaryOutputFromTool(pPropContainer, VARIANT_TRUE, &bHasPrimaryOutput)) && bHasPrimaryOutput == VARIANT_TRUE)
	{
		CComBSTR bstrPrimaryOutput;
		if (SUCCEEDED(GetPrimaryOutputFromTool(pPropContainer, VARIANT_TRUE, &bstrPrimaryOutput)) 
			&& bstrPrimaryOutput != NULL && bstrPrimaryOutput[0] != L'\0')
			*pnOutputs = 1;
	}

	return S_OK;
}

STDMETHODIMP CVCToolImpl::GetDeployableOutputs(IVCPropertyContainer* pPropContainer, IVCBuildStringCollection** ppDeployableOutputs)
{	// the deployable outputs
	CHECK_POINTER_NULL(ppDeployableOutputs);
	*ppDeployableOutputs = NULL;

	VARIANT_BOOL bHasPrimaryOutput = VARIANT_FALSE;
	if (FAILED(HasPrimaryOutputFromTool(pPropContainer, VARIANT_TRUE, &bHasPrimaryOutput)) || bHasPrimaryOutput != VARIANT_TRUE)
		return S_OK;	// nothing to do

	CComBSTR bstrPrimaryOutput;
	HRESULT hr = GetPrimaryOutputFromTool(pPropContainer, VARIANT_TRUE, &bstrPrimaryOutput);
	if (FAILED(hr) || bstrPrimaryOutput.Length() == 0)
		return hr;

	hr = CVCBuildStringCollection::CreateInstance(ppDeployableOutputs);
	RETURN_ON_FAIL_OR_NULL2(hr, *ppDeployableOutputs, E_OUTOFMEMORY);
	(*ppDeployableOutputs)->Add(bstrPrimaryOutput);

	return S_OK;
}

STDMETHODIMP CVCToolImpl::DirtyCommandLineOptions(long nLowPropID, long nHighPropID, VARIANT_BOOL bDirty)
{
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);
	VSASSERT(m_nDirtyKey >= 0, "Dirty key must be initialized");
	if (!bDirty || AffectsTool(nLowPropID, nHighPropID, NULL) == S_OK)
	{
		BOOL bCTypeDirty = (bDirty == VARIANT_TRUE);
		VARIANT_BOOL bAlreadyDirty;
		BOOL bExists = (m_spPropertyContainer->GetBoolProperty(m_nDirtyKey, &bAlreadyDirty) == S_OK);
		HRESULT hr = S_FALSE;
		if (bCTypeDirty != bExists)	// no-op when they match
		{
			if (bDirty)
				m_spPropertyContainer->SetBoolProperty(m_nDirtyKey, VARIANT_TRUE);
			else 
				m_spPropertyContainer->Clear(m_nDirtyKey);
			hr = S_OK;
		}
		if (m_nDirtyOutputsKey >= 0)
		{
			bExists = (m_spPropertyContainer->GetBoolProperty(m_nDirtyOutputsKey, &bAlreadyDirty) == S_OK);
			if (!bDirty  && bExists)
				m_spPropertyContainer->Clear(m_nDirtyOutputsKey);
			else if (bDirty && !bExists)
			{
				VARIANT_BOOL bAffectsOutput = VARIANT_FALSE;
				for (long idx = nLowPropID; idx <= nHighPropID && !bAffectsOutput; idx++)
					AffectsOutput(idx, &bAffectsOutput);
				if (bAffectsOutput)
				{
					m_spPropertyContainer->SetBoolProperty(m_nDirtyOutputsKey, VARIANT_TRUE);
					hr = S_OK;
				}
			}
		}
		return hr;
	}

	return S_FALSE;
}

STDMETHODIMP CVCToolImpl::HasVirtualLocalStorage(IVCPropertyContainer* pPropContainer, VARIANT_BOOL* pbHasVirtualLocalStorage)
{
	CHECK_POINTER_NULL(pbHasVirtualLocalStorage);
	CHECK_READ_POINTER_NULL(pPropContainer);
	*pbHasVirtualLocalStorage = VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CVCToolImpl::AffectsTool(long nLowPropID, long nHighPropID, VARIANT_BOOL* pbCanDirty)
{
	VSASSERT(m_nLowKey >= 0 && m_nHighKey >= 0 && m_nHighKey >= m_nLowKey, "Key range to watch must be initialized and in ascending order");
	if (pbCanDirty)
		*pbCanDirty = FALSE;
	BOOL bAffectsUs = FALSE;

	if (nLowPropID == nHighPropID)
	{	// single prop check
		if ((nLowPropID == -1) ||  	// override range checking
			ConfigBuildProp(nLowPropID))	// affects all outputs
			bAffectsUs = TRUE;
		if (!bAffectsUs && nLowPropID == VCFCFGID_ExcludedFromBuild)
		{
			CComQIPtr<VCFileConfiguration> spFileCfg = m_spPropertyContainer;
			bAffectsUs = (spFileCfg != NULL);
		}
	}
	if (!bAffectsUs &&						// may need to check single key against range
		((nLowPropID >= m_nLowKey && nLowPropID <= m_nHighKey) ||	// low key in range
		(nHighPropID >= m_nLowKey && nHighPropID <= m_nHighKey) ||	// high key in range
		(nLowPropID < m_nLowKey && nHighPropID > m_nHighKey)))	// desired range completely overlaps our range
	{
		bAffectsUs = TRUE;
	}

	if (bAffectsUs && pbCanDirty)
		*pbCanDirty = VARIANT_TRUE;

	return (bAffectsUs ? S_OK : S_FALSE);
}

STDMETHODIMP CVCToolImpl::IsMultiProp(long idProp, VARIANT_BOOL* pbIsMulti)
{
	CHECK_POINTER_NULL(pbIsMulti);

	*pbIsMulti = VARIANT_FALSE;
	if (idProp < m_nLowKey || idProp > m_nHighKey)	// doesn't belong to this tool, anyway
		return S_FALSE;

	if (GetOptionHandler() == NULL)
		return S_OK;	// by default, tools without option tables don't have multi-props

	if (IsExtraMultiProp(idProp))	// override this method for multi-props not found in option tables, ex: VCLINKID_AdditionalDependencies
	{
		*pbIsMulti = VARIANT_TRUE;
		return S_OK;
	}

	CLookupByDispidMap* pOptionMap = GetOptionHandler()->GetOptionLookupByDispid();
	RETURN_ON_NULL2(pOptionMap, S_FALSE);

	SOptionEntry* pEntry = pOptionMap->FindOptionEntry(idProp);
	RETURN_ON_NULL2(pEntry, S_FALSE);

	if (pEntry->type == multiple)
		*pbIsMulti = VARIANT_TRUE;
	
	return S_OK;
}

STDMETHODIMP CVCToolImpl::GetMultiPropSeparator(long idProp, BSTR* pbstrPreferred, BSTR* pbstrAll)
{
	CHECK_POINTER_NULL(pbstrPreferred);
	CHECK_POINTER_NULL(pbstrAll);
	*pbstrPreferred = NULL;
	*pbstrAll = NULL;

	if (idProp < m_nLowKey || idProp > m_nHighKey)	// doesn't belong to this tool, anyway
		return S_FALSE;

	if (GetOptionHandler() == NULL)
		return S_OK;	// by default, tools without option tables don't have multi-props

	if (IsExtraMultiProp(idProp))	// override this method for multi-props not found in option tables, ex: VCLINKID_AdditionalDependencies
	{
		CComBSTR bstrSep = L" ";
		bstrSep.CopyTo(pbstrPreferred);
		bstrSep.CopyTo(pbstrAll);
		return S_OK;
	}

	GetOptionHandler()->GetMultiPropSeparator(idProp, pbstrPreferred, pbstrAll);
	
	return S_OK;
}

STDMETHODIMP CVCToolImpl::CommandLineOptionsAreDirty(IVCPropertyContainer* pPropContainer, VARIANT_BOOL* pbIsDirty)
{
	VSASSERT(m_nDirtyKey >= 0, "Dirty key must be initialized");
	CHECK_POINTER_NULL(pbIsDirty);
	CHECK_ZOMBIE(pPropContainer, IDS_ERR_TOOL_ZOMBIE);

	VARIANT_BOOL bDirty;
	HRESULT hr = pPropContainer->GetBoolProperty(m_nDirtyKey, &bDirty);
	if (hr == S_OK)	// value doesn't really matter since we clear the property when we undirty it...
		*pbIsDirty = VARIANT_TRUE;
	else
		*pbIsDirty = VARIANT_FALSE;

	return hr;
}

STDMETHODIMP CVCToolImpl::OutputsAreDirty(IVCPropertyContainer* pPropContainer, VARIANT_BOOL* pbIsDirty)
{
	CHECK_POINTER_NULL(pbIsDirty);
	CHECK_ZOMBIE(pPropContainer, IDS_ERR_TOOL_ZOMBIE);
	if (m_nDirtyOutputsKey < 0)
	{
		*pbIsDirty = VARIANT_FALSE;
		return S_FALSE;
	}

	VARIANT_BOOL bDirty;
	HRESULT hr = pPropContainer->GetBoolProperty(m_nDirtyOutputsKey, &bDirty);
	if (hr == S_OK)	// value doesn't really matter since we clear the property when we undirty it...
		*pbIsDirty = VARIANT_TRUE;
	else
		*pbIsDirty = VARIANT_FALSE;

	return hr;
}

STDMETHODIMP CVCToolImpl::ClearDirtyOutputs(IVCPropertyContainer* pPropContainer)
{
	CHECK_ZOMBIE(pPropContainer, IDS_ERR_TOOL_ZOMBIE);
	if (m_nDirtyOutputsKey < 0)
		return S_OK;	// nothing to clear

	pPropContainer->Clear(m_nDirtyOutputsKey);
	return S_OK;
}

HRESULT CVCToolImpl::GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, CPathW& rPath)
{ 
	return S_FALSE;	// by default, not primary output generator
}

void CVCToolImpl::ConvertPathToBSTR(CPathW& rpath, CComBSTR& rbstrPath)
{
	CStringW strPath = rpath;
	rbstrPath = strPath;
}

void CVCToolImpl::CreateDirFromBSTR(BSTR bstrDir, CDirW& rDir)
{
	CStringW strDir = bstrDir;
	rDir.CreateFromKnown(strDir);
}

STDMETHODIMP CVCToolImpl::GenerateDependencies(IVCBuildAction* pAction, void* frhFile, IVCBuildErrorContext* pEC)
{
	return S_OK;	// ok
}

#define HAVE_FILENAME	0x001
#define HAVE_PROPNAME	0x010
#define HAVE_TOOLNAME	0x100

STDMETHODIMP CVCToolImpl::InformUserAboutBadFileName(IVCBuildErrorContext* pEC, IVCPropertyContainer* pPropContainer,
	long nErrPropID, LPCOLESTR szFile)
{
	CStringW strFile = szFile;
	CStringW strPropName;
	if (nErrPropID > 0)
	{
		CStringW strProp;
		strProp.LoadString(nErrPropID);
		int nLoc = strProp.Find(L" :");
		if (nLoc > 0)
			strPropName = strProp.Left(nLoc);
		else
			strPropName = strProp;
		strPropName.TrimLeft();
		strPropName.TrimRight();
	}
	CStringW strTool;
	long nToolID = GetToolID();
	if (nToolID > 0)
		strTool.LoadString(nToolID);

	int nCombo = 0;
	if (!strPropName.IsEmpty())
		nCombo |= HAVE_PROPNAME;
	if (!strFile.IsEmpty())
		nCombo |= HAVE_FILENAME;
	if (!strTool.IsEmpty())
		nCombo |= HAVE_TOOLNAME;

	CStringW strMsg;
	CComBSTR bstrErr;
	if (nCombo == (HAVE_PROPNAME | HAVE_FILENAME | HAVE_TOOLNAME))
	{
		strMsg.Format(IDS_ERR_BADFILE_PRJ0020, strTool, strPropName, strFile);
		bstrErr = L"PRJ0020";
	}
	else if (nCombo == (HAVE_PROPNAME | HAVE_TOOLNAME))
	{
		strMsg.Format(IDS_ERR_BADFILE_PRJ0021, strTool, strPropName);
		bstrErr = L"PRJ0021";
	}
	else if (nCombo == (HAVE_PROPNAME | HAVE_FILENAME))
	{
		strMsg.Format(IDS_ERR_BADFILE_PRJ0022, strPropName, strFile);
		bstrErr = L"PRJ0022";
	}
	else if (nCombo == (HAVE_TOOLNAME | HAVE_FILENAME))
	{
		strMsg.Format(IDS_ERR_BADFILE_PRJ0023, strTool, strFile);
		bstrErr = L"PRJ0023";
	}

	if (!strMsg.IsEmpty())
	{
		CComBSTR bstrMsg = strMsg;
		CVCProjectEngine::AddProjectError(pEC, bstrMsg, bstrErr, pPropContainer);
	}

	return S_OK;
}

void CVCToolImpl::GetEnumVal(IUnknown *pUnk, SOptionEntry *pEntry, CComVariant &var, IDispatch *pDispTool)
{// use type lib to get an enumval
	CComPtr<IDispatch> pDisp = pDispTool;
	if (!pDisp)
	{
		CComQIPtr<VCFileConfiguration> pFileConfig;
		pFileConfig = pUnk;
		if (pFileConfig) // play games to get the tool
		{
			pFileConfig->get_Tool(&pDisp);
		}
	}
	VSASSERT(pDisp, "No tool associated with this file config.  Need either file config or tool to be passed in to GetEnumValue");
	if (!pDisp)
		return;// have to have a dispatch to do this...
	CComPtr<ITypeInfo> pTypeInfo;
	HRESULT  hr = pDisp->GetTypeInfo(0, LOCALE_SYSTEM_DEFAULT, &pTypeInfo);
	if (pTypeInfo && SUCCEEDED(hr))
	{
		TYPEATTR* pta;
		bool bBreak = false;
		hr = pTypeInfo->GetTypeAttr(&pta);
		if (pta)
		{
			for (int iIndex=0; iIndex<pta->cFuncs && !bBreak; iIndex++)
			{
				FUNCDESC* pfd;
				if (SUCCEEDED(pTypeInfo->GetFuncDesc(iIndex, &pfd)) && pfd)
				{
					#ifdef _DEBUG
								CComBSTR bstrPropName;
								pTypeInfo->GetDocumentation(pfd->memid, &bstrPropName, NULL, NULL, NULL);
					#endif
					if (pfd->memid == pEntry->idOption)
					{
						if (pfd->elemdescFunc.tdesc.vt == VT_USERDEFINED)
						{
							CComPtr<ITypeInfo> pTypeInfoRef;
							hr = pTypeInfo->GetRefTypeInfo(pfd->elemdescFunc.tdesc.hreftype, &pTypeInfoRef);
							if (SUCCEEDED(hr) && pTypeInfoRef)
							{
								TYPEATTR* ptaRef;
								hr = pTypeInfoRef->GetTypeAttr(&ptaRef);
								if (ptaRef)
								{
									for (int iIndexRef = 0; ptaRef && SUCCEEDED(hr) && iIndexRef < ptaRef->cVars && !bBreak; iIndexRef++)
									{
										VARDESC* pvdRef;
										if (SUCCEEDED(pTypeInfoRef->GetVarDesc(iIndexRef, &pvdRef)))
										{
											CComBSTR bstrNameItem;
											if (SUCCEEDED(pTypeInfoRef->GetDocumentation(pvdRef->memid, &bstrNameItem, NULL, NULL, NULL)))
											{
												VSASSERT(pvdRef->varkind == VAR_CONST, "Bad varkind");
												if (var == *pvdRef->lpvarValue)
												{
													bBreak = true;
													var = bstrNameItem;
												}
											}
											pTypeInfoRef->ReleaseVarDesc(pvdRef);
										}
									}
									pTypeInfoRef->ReleaseTypeAttr(ptaRef);
								}
							}
						}
					}
					pTypeInfo->ReleaseFuncDesc(pfd);
				}
			}
			pTypeInfo->ReleaseTypeAttr(pta);
		}
	}
}

// Given a path to a source file, 'pathSrc', and a list of include directives, 'strlstIncs',
// this function will return a list of paths, 'lstpath', that represent the 'resolved' (or
// absolute) location of the file represented by each include directive.
// Any standard includes, eg. #include <stdio.h>, will be ignored
// If per-file include directives, eg. compiler /I, exist for the 'pathSrc' then these will be used.
//
// Returns FALSE if the operation cannot be performed.
BOOL CVCToolImpl::ResolveIncludeDirectivesToPath(const CPathW& pathSrc, IVCBuildAction* pAction, 
	const CVCStringWList& strlstIncs, CVCStringWList& lstPath, BOOL fIsInclude /* = TRUE */)
{
	CStringW strIncDirs;	// our include directories
	CStringW strPlatDirs;	// the platform include path
	
	if (!GetIncludePathI(pAction, strIncDirs, &pathSrc, strPlatDirs, fIsInclude))
		return FALSE;
		
	// our base directory
	CDirW dirBase;
	if (!dirBase.CreateFromPath(pathSrc))
		return FALSE;	// failed to get the base directory for the source

	return ResolveIncludeDirectivesI(pAction, strIncDirs, (const CDirW &)dirBase, strlstIncs, lstPath, &pathSrc, strPlatDirs, 
		fIsInclude);
}

// Return the full search path for includes for a file.
// FUTURE: Use per-file include paths, eg. as specified by /i for the linker
BOOL CVCToolImpl::GetIncludePathI(IVCBuildAction* pAction, CStringW& strIncDirs, const CPathW* ppathSrc, CStringW& strPlatDirs, 
	BOOL fIsInclude)
{
	VSASSERT(pAction, "pAction required for GetIncludePathI!");
	RETURN_ON_NULL2(pAction, FALSE);

	BOOL fGotPerFile = FALSE;

	// make all of these relative to the project directory
				
	CComBSTR bstrIncDirs;
	CComPtr<IVCPropertyContainer> spPropContainer;
	pAction->get_PropertyContainer(&spPropContainer);
	VSASSERT(spPropContainer != NULL, "Property container required in order to get IncludeDirectories");
	if (spPropContainer != NULL)
	{
		if (ppathSrc != (const CPathW *)NULL)
		{
			// get the include directories for this tool
			CComPtr<IVCToolImpl> spTool;
			HRESULT hr = pAction->get_VCToolImpl(&spTool);
			VSASSERT(SUCCEEDED(hr), "Action must have a tool!");
			if (spTool != NULL)
			{
				spTool->GetAdditionalIncludeDirectoriesInternal(spPropContainer, &bstrIncDirs);
				fGotPerFile = TRUE;
			}
		}

		// set up the platform dirs
		CComBSTR bstrPlatDirs;
		spPropContainer->GetEvaluatedStrProperty(fIsInclude ? VCPLATID_IncludeDirectories : VCPLATID_LibraryDirectories, 
			&bstrPlatDirs);
		strPlatDirs = bstrPlatDirs;
		if (fGotPerFile)
		{
			spPropContainer->Evaluate(bstrIncDirs, &bstrIncDirs);
			strIncDirs = bstrIncDirs;
		}
	}

	// got per-file includes?
	if (fGotPerFile)
	{
		if (strIncDirs.IsEmpty())	// probably didn't get to evaluate for some reason...
			strIncDirs = bstrIncDirs;
		else if (!strPlatDirs.IsEmpty())
		{
			strIncDirs += L";";
			strIncDirs += strPlatDirs;
		}
	}
	else
		strIncDirs = strPlatDirs;

	return TRUE;	// success 
}

// Internal core for CVCToolImpl::ResolveIncludeDirectives*() methods.
BOOL CVCToolImpl::ResolveIncludeDirectivesI(IVCBuildAction* pAction, CStringW& strlstIncDirs, const CDirW& dirBase,
	const CVCStringWList& strlstIncs, CVCStringWList& lstPath, const CPathW *pathSrc, CStringW& strPlatDirs, BOOL fIsInclude)
{
	// FUTURE: use this information to update our own cache

	// include directive information
	CBldIncludeEntry entry;
	entry.m_OriginalDir = dirBase;
	entry.m_nLineNumber = 0;			// N/A 
	entry.m_bShouldBeScanned = FALSE;	// N/A (not to be scanned)

	CStringW strInclude, strFile;

	VCPOSITION posInc = strlstIncs.GetHeadPosition();
	CBldScannerCache* pScannerCache = NULL;
	if (posInc != (VCPOSITION)NULL && pAction)
	{
		CComPtr<IVCBuildableItem> spItem;
		pAction->get_Item(&spItem);
		if (spItem)
		{
			CComPtr<IVCBuildEngine> spBuildEngine;
			spItem->get_ExistingBuildEngine(&spBuildEngine);
			CComQIPtr<IVCBuildEngineImpl> spBuildEngineImpl = spBuildEngine;
			if (spBuildEngineImpl)
				spBuildEngineImpl->get_ScannerCache((void **)&pScannerCache);
		}
	}

	while (posInc != (VCPOSITION)NULL)
	{
		// get the include directive,
		// this is of the forms "file" or <file>
		strInclude = strlstIncs.GetNext(posInc);

		int cchInclude = strInclude.GetLength();
		const wchar_t * pchInclude = strInclude;

		if (*pchInclude != L'<' && *pchInclude != L'\"' && *pchInclude != L'\'')
		{
            // [paulde] change from assert to TRACE because some callers have a 
            // legitimate need to pass names that don't begin with these chars,
            // and for those that care, you at least get a trace message.
			ATLTRACE("CVCToolImpl::ResolveIncludeDirectivesI: file does not begin with [<\"']\n");
			return FALSE;
		}

		// standard include?
		if (fIsInclude)
			entry.m_EntryType = (*pchInclude == L'<') ?	IncTypeCheckIncludePath : IncTypeCheckIncludePath | IncTypeCheckOriginalDir;
		else
			entry.m_EntryType = IncTypeCheckLibPath | IncTypeCheckIntDir | IncTypeCheckOutDir | IncTypeCheckOriginalDir;

		// strip the leading and trailing characters from 'strInclude'
		wchar_t* pchFile = strFile.GetBuffer(cchInclude - 1);
		wcsncpy(pchFile, pchInclude + 1, cchInclude - 2);
		pchFile[cchInclude - 2] = L'\0';
		strFile.ReleaseBuffer();

		BOOL fIgnore = TRUE;	// by default ignore

		// is this a system include? ignore?
		CComPtr<VCConfiguration> spProjCfg;
		if (pAction != NULL)
		{
			pAction->get_ProjectConfiguration(&spProjCfg);
		}

		entry.m_FileReference = strFile;	// the file
		entry.CheckAbsolute();				// check the absoluteness of scanned dep.

		BldFileRegHandle frhFile = NULL;	// the 'resolved' file

		// build directory to make relative to?
		CStringW strProjDir;
		CComPtr<IVCBuildableItem> spItem;
		if (pAction != NULL)
		{
			pAction->get_Item(&spItem);
			CComQIPtr<IVCPropertyContainer> spPropContainer = spItem;
			VSASSERT(spPropContainer != NULL, "Property container required in order to get ProjectDirectory");
			CComBSTR bstrProjDir;
			if (spPropContainer != NULL)
				spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir);	// should never have macros in it
			strProjDir = bstrProjDir;
		}

		if (entry.FindFile((const wchar_t *)strlstIncDirs, dirBase, (const wchar_t *)strProjDir, frhFile, spItem, 
			pScannerCache, strPlatDirs))
		{
			CStringW strFile2;
			frhFile->GetFilePath()->GetFullPath(strFile2);
			if (g_SysInclReg.IsSysInclude(strFile2))
				frhFile->ReleaseFRHRef();	// going to be ignoring it
			else
			{
				CBldFileRegEntry * pfilereg = g_FileRegistry.GetRegEntry(frhFile);
				VSASSERT(pfilereg != (CBldFileRegEntry *)NULL, "frhFile not in global registry.  Either released too early or not added to begin with.");

				// add path to our list
				lstPath.AddTail((CStringW)*pfilereg->GetFilePath());
				frhFile->ReleaseFRHRef();
				fIgnore = FALSE;	// don't ignore 
			}
		}
		else if (NULL != frhFile)
		{
			frhFile->ReleaseFRHRef();
#ifdef DIANEME_DEP_CHECK_DIAGNOSTICS
			CStringA str = strFile;
			ATLTRACE("\tFailed to find input dependency file '%s'\n", str);
#endif	// DIANEME_DEP_CHECK_DIAGNOSTICS
			return FALSE;	// didn't find it
		}
	}

	return TRUE;	// success
}

BOOL CVCToolImpl::FileNameMatchesExtension(const CPathW* pPath, const  wchar_t* pExtensions)
{
	if (pPath == (const CPathW *)NULL)	
		return FALSE;

	const wchar_t* pExtNext = pExtensions;
	const wchar_t* pFileExt = pPath->GetExtension();
	if (*pFileExt)
		pFileExt++;
		// GetExtension() returns pointer to '.'

	INT_PTR n = wcslen(pFileExt);

	if (n)
	{
		while (*pExtNext != L'\0')
		{
			pExtNext = pExtensions;
			while (*pExtNext != L';' && *pExtNext != L',' && *pExtNext != L'\0')
				pExtNext++;

			// skip over the optional '*.'
			if (pExtensions[0] == L'*' && pExtensions[1] == L'.')
				pExtensions += 2;
			else if (pExtensions[0] == L'.')
				pExtensions += 1;

			// compare extensions in case-insensitive fashion
			if ((n == (pExtNext - pExtensions)) &&			// check for ext. length equivalence
				_wcsnicmp (pFileExt, pExtensions, n) == 0)	// check for ext. string equivalence
				return TRUE;

			pExtensions = pExtNext + 1;	// skip over the sep.
		}
	}

	return FALSE;
}

BOOL CVCToolImpl::IsExcludedFromBuild(IVCPropertyContainer* pPropContainer)
{
	RETURN_ON_NULL2(pPropContainer, TRUE);	// nothing there to build...

	VARIANT_BOOL bExcluded = VARIANT_FALSE;
	if (pPropContainer->GetBoolProperty(VCFCFGID_ExcludedFromBuild, &bExcluded) != S_OK)
		return FALSE;

	return (bExcluded == VARIANT_TRUE);
}

HRESULT CVCToolImpl::GetProcessedAdditionalOptions(IUnknown* pItem, long idProp, VARIANT_BOOL bForBuild, 
	VARIANT_BOOL bSkipLocal, BSTR* pbstrAdditionalOptions)
{
	CComQIPtr<IVCPropertyContainer> spContainer = pItem;
	RETURN_ON_NULL2(spContainer, E_NOINTERFACE);
	CHECK_POINTER_NULL(pbstrAdditionalOptions);
	*pbstrAdditionalOptions = NULL;
	
	HRESULT hr;
	if (bSkipLocal)
	{
		CComPtr<IVCPropertyContainer> spActualPropContainer;
		CVCProjectEngine::ResolvePropertyContainer(spContainer, &spActualPropContainer);
		hr = spActualPropContainer->GetMultiProp(idProp, L" ", VARIANT_TRUE, pbstrAdditionalOptions);
	}
	else if (bForBuild)
		hr = spContainer->GetMultiProp(idProp, L" ", VARIANT_FALSE, pbstrAdditionalOptions);
	else
	{
		CComVariant var;
		hr = spContainer->GetLocalProp(idProp, &var);
		if (hr == S_OK && var.vt == VT_BSTR)
		{
			CComBSTR bstrOptions = var.bstrVal;
			*pbstrAdditionalOptions = bstrOptions.Detach();
		}
		else if (SUCCEEDED(hr))
		{
			CComPtr<IVCPropertyContainer> spActualPropContainer;
			CVCProjectEngine::ResolvePropertyContainer(spContainer, &spActualPropContainer);
			if (spActualPropContainer)
			{
				hr = spActualPropContainer->GetLocalProp(idProp, &var);
				if (hr == S_OK && var.vt == VT_BSTR)
				{
					CComBSTR bstrOptions = var.bstrVal;
					*pbstrAdditionalOptions = bstrOptions.Detach();
				}
			}
		}
	}
	if (hr == S_FALSE)
		COptionHandlerBase::GetDefaultString(pbstrAdditionalOptions);
	return hr;
}

BOOL CVCToolImpl::CreatePathRelativeToProject(IVCPropertyContainer* pPropContainer, CStringW& strName, CPathW& pathName)
{
	CComBSTR bstrProjectDirectory;
	pPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjectDirectory);
	CDirW baseDir;
	CreateDirFromBSTR(bstrProjectDirectory, baseDir);
	return pathName.CreateFromDirAndFilename(baseDir, strName);
}

HRESULT CVCToolImpl::DoMatchName(BSTR bstrName, const wchar_t* szToolType, const wchar_t* szShortName, VARIANT_BOOL* pbMatches)
{
	CHECK_POINTER_NULL(pbMatches);	// this MatchName is on a private interface and so doesn't need the strong protection
	*pbMatches = VARIANT_FALSE;
	RETURN_INVALID_ON_NULL(bstrName);

	*pbMatches = _wcsicmp(szShortName, bstrName) == 0 ? VARIANT_TRUE : VARIANT_FALSE;
	if (*pbMatches == VARIANT_TRUE)
		return S_OK;

	*pbMatches = _wcsicmp(szToolType, bstrName) == 0 ? VARIANT_TRUE : VARIANT_FALSE;
	if (*pbMatches == VARIANT_TRUE)
		return S_OK;
	
	*pbMatches = _wcsicmp(GetToolFriendlyName(), bstrName) == 0 ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

HRESULT CVCToolImpl::DoGetDefaultExtensions(CComBSTR& bstrSetExtensions, const wchar_t* const wszBaseDefaultExtensions, BSTR* pbstrVal)
{
	CHECK_POINTER_NULL(pbstrVal);
	if (bstrSetExtensions.Length() == 0)
		bstrSetExtensions = wszBaseDefaultExtensions;
	bstrSetExtensions.CopyTo(pbstrVal);
	return S_OK;
}

STDMETHODIMP CVCToolImpl::get_FullIncludePathInternal(BSTR* pbstrIncPath)
{	// include path, including all inherited values, plus platform includes
	CHECK_POINTER_NULL(pbstrIncPath);
	*pbstrIncPath = NULL;
	CHECK_ZOMBIE(m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE);

	CComBSTR bstrPlatformIncs, bstrTemp;
	HRESULT hr = m_spPropertyContainer->GetStrProperty(VCPLATID_IncludeDirectories, &bstrTemp);
	RETURN_ON_FAIL(hr);
	hr = m_spPropertyContainer->Evaluate(bstrTemp, &bstrPlatformIncs);
	RETURN_ON_FAIL(hr);

	long nPathID = 0;
	if (FAILED(get_IncludePathID(&nPathID)) || nPathID <= 0)
	{
		if (bstrPlatformIncs)
			*pbstrIncPath = bstrPlatformIncs.Detach();
		return S_OK;
	}

	CComBSTR bstrOverridePath;
	hr = m_spPropertyContainer->GetEvaluatedMultiProp(nPathID, L";,", VARIANT_FALSE, VARIANT_FALSE, &bstrOverridePath);
	if (FAILED(hr))
	{
		*pbstrIncPath = bstrPlatformIncs.Detach();
		return S_OK;
	}

	int nPlatLen = bstrPlatformIncs.Length();
	int nOverrideLen = bstrOverridePath.Length();

	if( nOverrideLen > 0 )
	{
		// fix up embeded spaces that shouldn't be there.
		CStringW str;
		wchar_t* szNoSpaces = str.GetBuffer(nOverrideLen+1);
		BOOL skipping = TRUE;
		int i,j;
		wchar_t c;
		for( i=0,j=0; bstrOverridePath[i] != NULL; i++ )
		{
		    	c=bstrOverridePath[i];
		    	if( c == L',' )
				c = L';';

			if( skipping )
			{
				if( c == L' ' )
					continue;
				else
					skipping = FALSE;
			}
			if( c == L';' )
				skipping = TRUE;

			szNoSpaces[j] = c;
			j++;
		}
		szNoSpaces[j] = NULL;
		str.ReleaseBuffer();
		bstrOverridePath = str;
		nOverrideLen = bstrOverridePath.Length();
	}

	if (nPlatLen && nOverrideLen)
	{ 
		bstrOverridePath += L";";
		bstrOverridePath += bstrPlatformIncs;
		*pbstrIncPath = bstrOverridePath.Detach();
	}
	else if (nPlatLen)
		*pbstrIncPath = bstrPlatformIncs.Detach();
	else if (nOverrideLen)
		*pbstrIncPath = bstrOverridePath.Detach();

	return S_OK;
}

static const wchar_t* const wszEnd =
	L"\nif errorlevel 1 goto VCReportError\n"
	L"goto VCEnd\n"
	L":VCReportError\n"
	L"echo Project : error PRJ0019: ";

void CVCToolImpl::BuildCommandLineBatchFile(IVCBuildAction* pAction, CComBSTR& bstrCommandLineEnd, CComBSTR& bstrCmdLine)
{
	bstrCmdLine = L"@echo off\n";
	bstrCmdLine += bstrCommandLineEnd;
	bstrCmdLine.Append(wszEnd);
	CComBSTR bstrError;
	bstrError.LoadString( IDS_ERR_CUSTOM_PRJ0019 );
	bstrCmdLine.Append( bstrError );

	CComBSTR bstrDesc;
	GetBuildDescription( pAction, &bstrDesc );

	int nLen = bstrDesc.Length();
	if (nLen > 0)
	{
		bstrCmdLine += L"\"";
		bstrCmdLine += bstrDesc;
		bstrCmdLine += L"\"";
	}
	bstrCmdLine += L"\nexit 1\n:VCEnd";
}

#ifndef _SHIP
HRESULT CVCToolImpl::ForceDirty()
{
	CComPtr<IVCPropertyContainer> spActualContainer;
	CVCProjectEngine::ResolvePropertyContainer(m_spPropertyContainer, &spActualContainer);
	CHECK_ZOMBIE(spActualContainer, IDS_ERR_TOOL_ZOMBIE);

	HRESULT hr = VCPROJ_E_ZOMBIE;
	CComPtr<IDispatch> spDispProject;
	CComQIPtr<VCConfiguration> spProjConfig;
	CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = spActualContainer;
	if (spFileCfgImpl)
	{
		CComPtr<IDispatch> spDispCfg;
		hr = spFileCfgImpl->get_Configuration(&spDispCfg);
		RETURN_ON_FAIL(hr);
		spProjConfig = spDispCfg;
	}
	else
		spProjConfig = spActualContainer;

	if (spProjConfig)
		hr = spProjConfig->get_Project(&spDispProject);
	else
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_TOOL_ZOMBIE);

	CComQIPtr<IVCProjectImpl> spProjectImpl = spDispProject;
	if (spProjectImpl)
		return spProjectImpl->put_IsConverted(VARIANT_TRUE);

	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_TOOL_ZOMBIE);
}
#endif	// _SHIP

HRESULT CVCToolImpl::DoGetVCProjectEngine(IDispatch** ppProjEngine)
{
	CHECK_POINTER_VALID(ppProjEngine);
	RETURN_ON_NULL(g_pProjectEngine);
	return g_pProjectEngine->QueryInterface(IID_IDispatch, (void **)ppProjEngine);
}

HRESULT CVCToolImpl::ToolGetStrProperty(long idProp, BSTR* pVal, bool bLocalOnly /* = false */)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POINTER_VALID( pVal );
	HRESULT hr = S_OK;
	
	if (bLocalOnly)
	{
		*pVal = NULL;
		CComVariant var;
		hr = m_spPropertyContainer->GetLocalProp(idProp, &var);
		if (hr == S_OK && var.vt == VT_BSTR)
		{
			CComBSTR bstrVal = var.bstrVal;
			*pVal = bstrVal.Detach();
		}
	}
	else
	{
		hr = m_spPropertyContainer->GetStrProperty(idProp, pVal);
		if (hr == S_FALSE && GetOptionHandler())
			GetOptionHandler()->GetDefaultValue( idProp, pVal, m_spPropertyContainer );
	}
	return hr;
}

HRESULT CVCToolImpl::ToolGetIntProperty(long idProp, long* pVal)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POINTER_VALID( pVal );
	HRESULT hr = m_spPropertyContainer->GetIntProperty(idProp, pVal);
	if (hr == S_FALSE && GetOptionHandler())
		GetOptionHandler()->GetDefaultValue( idProp, pVal, m_spPropertyContainer );
	return hr;
}

HRESULT CVCToolImpl::ToolGetBoolProperty(long idProp, VARIANT_BOOL* pVal)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POINTER_VALID( pVal );
	HRESULT hr = m_spPropertyContainer->GetBoolProperty(idProp, pVal);
	if (hr == S_FALSE && GetOptionHandler())
		GetOptionHandler()->GetDefaultValue( idProp, pVal, m_spPropertyContainer );
	return hr;
}

