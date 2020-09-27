// NMakeTool.cpp : Implementation of CVCNMakeTool

#include "stdafx.h"
#include "NMakeTool.h"
#include "FileRegistry.h"
#include "BldHelpers.h"
#include "BldActions.h"
#include "ProjWriter.h"

// static initializers
CComBSTR CVCNMakeTool::s_bstrDescription = L"";
CComBSTR CVCNMakeTool::s_bstrToolName = L"";
CComBSTR CVCNMakeTool::s_bstrExtensions = L"";
GUID CVCNMakeTool::s_pPages[1];
BOOL CVCNMakeTool::s_bPagesInit;

//////////////////////////////////////////////////////////////////////////////
// implementation of CVCNMakeTool; behavior specific to NMake tool, with storage

HRESULT CVCNMakeTool::CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool)
{
	CHECK_POINTER_NULL(ppTool);
	*ppTool = NULL;

	CComObject<CVCNMakeTool> *pObj;
	HRESULT hr = CComObject<CVCNMakeTool>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		if( pPropContainer )
			pObj->Initialize(pPropContainer);
		CVCNMakeTool *pVar = pObj;
		pVar->AddRef();
		*ppTool = pVar;
	}
	return hr;
}

//////////////////////////////////////////////////////////////////////////////
// VCNMakeTool

STDMETHODIMP CVCNMakeTool::get_ToolName(BSTR* pbstrToolName)	// friendly name of tool, e.g., "C/C++ Compiler Tool"
{
	CHECK_POINTER_VALID( pbstrToolName );
	InitToolName();
	s_bstrToolName.CopyTo(pbstrToolName);
	return S_OK;
}

STDMETHODIMP CVCNMakeTool::get_BuildCommandLine(BSTR *pbstrCommandLine)
{
	return ToolGetStrProperty(VCNMAKEID_BuildCommandLine, pbstrCommandLine);
}

STDMETHODIMP CVCNMakeTool::put_BuildCommandLine(BSTR bstrCommandLine)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCNMAKEID_BuildCommandLine, bstrCommandLine);
}

STDMETHODIMP CVCNMakeTool::get_ReBuildCommandLine(BSTR *pbstrCommandLine)
{
	return ToolGetStrProperty(VCNMAKEID_ReBuildCommandLine, pbstrCommandLine);
}

STDMETHODIMP CVCNMakeTool::put_ReBuildCommandLine(BSTR bstrCommandLine)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCNMAKEID_ReBuildCommandLine, bstrCommandLine);
}

STDMETHODIMP CVCNMakeTool::get_CleanCommandLine(BSTR *pbstrCommandLine)
{
	return ToolGetStrProperty(VCNMAKEID_CleanCommandLine, pbstrCommandLine);
}

STDMETHODIMP CVCNMakeTool::put_CleanCommandLine(BSTR bstrCommandLine)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCNMAKEID_CleanCommandLine, bstrCommandLine);
}

STDMETHODIMP CVCNMakeTool::get_Output(BSTR *pbstrOutput)
{
	return ToolGetStrProperty(VCNMAKEID_Output, pbstrOutput);
}

STDMETHODIMP CVCNMakeTool::put_Output(BSTR bstrOutput)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCNMAKEID_Output, bstrOutput);
}

// Automation properties
STDMETHODIMP CVCNMakeTool::get_ToolPath(BSTR *pbstrToolPath)
{
	CHECK_POINTER_VALID(pbstrToolPath);
	*pbstrToolPath = SysAllocString( szNMakeToolPath );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
// IVCToolImpl

STDMETHODIMP CVCNMakeTool::get_DefaultExtensions(BSTR* pVal)
{
	return DoGetDefaultExtensions(s_bstrExtensions, wszNMakeDefaultExtensions, pVal);
}

STDMETHODIMP CVCNMakeTool::put_DefaultExtensions(BSTR newVal)
{
	s_bstrExtensions = newVal;
	return S_OK;
}

STDMETHODIMP CVCNMakeTool::GetAdditionalOptionsInternal(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, BSTR* pbstrOptions)
{ 
	return E_NOTIMPL;	// don't have any; but don't want assert
}

STDMETHODIMP CVCNMakeTool::GetBuildDescription(IVCBuildAction* pAction, BSTR* pbstrBuildDescription)
{
	if( s_bstrDescription.Length() == 0 )
		s_bstrDescription.LoadString( IDS_NMAKE_DESC );
	return s_bstrDescription.CopyTo( pbstrBuildDescription );
}

STDMETHODIMP CVCNMakeTool::get_ToolPathInternal(BSTR* pbstrToolPath)
{ 
	return get_ToolPath(pbstrToolPath);
}

STDMETHODIMP CVCNMakeTool::get_Bucket(long *pVal)
{
	CHECK_POINTER_NULL( pVal );
	*pVal = BUCKET_NMAKE;
	return S_OK;
}

STDMETHODIMP CVCNMakeTool::GenerateOutput(long type, IVCBuildActionList* pActions, IVCBuildErrorContext* pEC)
{
	RETURN_ON_NULL2(pActions, S_FALSE);

	pActions->Reset(NULL);
 	while (TRUE)
 	{
		CComPtr<IVCBuildAction> spAction;
		HRESULT hr = pActions->Next(&spAction, NULL);
		BREAK_ON_DONE(hr);
		if (spAction == NULL)
			continue;
				
		// create a bogus, non-existant, output string
		// so that this tool will always run
		CStringW strOutputFile = _wtmpnam( NULL );
		if( !strOutputFile )
		{
			VSASSERT( FALSE, "_wtmpnam() failed" );
			CComBSTR bstrErr;
			bstrErr.LoadString(IDS_ERR_NO_CREATE_TEMP_FILE_PRJ0039);
			CVCProjectEngine::AddProjectError(pEC, bstrErr, L"PRJ0039", pActions);
			return S_FALSE;
		}

		// create our path given this output string
		CPathW pathOutput;		// path of our output based on this spec.
		if (pathOutput.Create(strOutputFile))
		{
			pathOutput.GetActualCase(TRUE);
			if (spAction->AddOutputFromPath((wchar_t *)(const wchar_t *)pathOutput, pEC, VARIANT_TRUE, VARIANT_FALSE, -1, 
				this) != S_OK)
			{
				// FUTURE: error goes here! 
				VSASSERT(FALSE, "Failed to add output for nmake command line");	
			}
		}
 	}

	return S_OK; // success
}

STDMETHODIMP CVCNMakeTool::GetCommandLineEx(IVCBuildAction* pAction, IVCBuildableItem* pBuildableItem, 
	IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	*pVal = NULL;

	CComQIPtr<IVCPropertyContainer> spPropContainer = pBuildableItem;
	RETURN_INVALID_ON_NULL(spPropContainer);

	CComBSTR bstrCommandLine;
	HRESULT hr = spPropContainer->GetEvaluatedStrProperty(VCNMAKEID_BuildCommandLine, &bstrCommandLine);
	RETURN_ON_FAIL(hr);

	// if it's blank, put a message out to the error context
	if( bstrCommandLine.Length() == 0 )
	{
		CComBSTR bstrMsg;
		bstrMsg.LoadString( IDS_NOBUILDCOMMANDLINEMSG );
		pEC->AddInfo( bstrMsg );
	}

	if (hr != S_OK)
	{
		bstrCommandLine.CopyTo(pVal);
		return S_FALSE;
	}

	if (pBuildEngine == NULL)
		return bstrCommandLine.CopyTo(pVal);

	CComBSTR bstrFullCommandLine;
	BuildCommandLineBatchFile(pAction, bstrCommandLine, bstrFullCommandLine);

	return pBuildEngine->FormBatchFile(bstrFullCommandLine, pEC, pVal);
}

STDMETHODIMP CVCNMakeTool::GetCleanCommandLineEx(IVCBuildAction* pAction, IVCBuildableItem* pBuildableItem, 
	IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	*pVal = NULL;

	CComQIPtr<IVCPropertyContainer> spPropContainer = pBuildableItem;
	RETURN_INVALID_ON_NULL(spPropContainer);

	CComBSTR bstrCommandLine;
	HRESULT hr = spPropContainer->GetEvaluatedStrProperty(VCNMAKEID_CleanCommandLine, &bstrCommandLine);
	RETURN_ON_FAIL(hr);

	// if it's blank, put a message out to the error context
	if( bstrCommandLine.Length() == 0 )
	{
		CComBSTR bstrMsg;
		bstrMsg.LoadString( IDS_NOCLEANCOMMANDLINEMSG );
		pEC->AddInfo( bstrMsg );
	}
	else
	{
		CComBSTR bstrRealCmd = bstrCommandLine;
		BuildCommandLineBatchFile(pAction, bstrRealCmd, bstrCommandLine);
	}

	if (hr != S_OK)
	{
		bstrCommandLine.CopyTo(pVal);
		return S_FALSE;
	}

	if (pBuildEngine == NULL)
		return bstrCommandLine.CopyTo(pVal);

	return pBuildEngine->FormBatchFile(bstrCommandLine, pEC, pVal);
}

STDMETHODIMP CVCNMakeTool::GetRebuildCommandLineEx(IVCBuildAction* pAction, IVCBuildableItem* pBuildableItem, 
	IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	*pVal = NULL;

	CComQIPtr<IVCPropertyContainer> spPropContainer = pBuildableItem;
	RETURN_INVALID_ON_NULL(spPropContainer);

	CComBSTR bstrCommandLine;
	HRESULT hr = spPropContainer->GetEvaluatedStrProperty(VCNMAKEID_ReBuildCommandLine, &bstrCommandLine);
	RETURN_ON_FAIL(hr);

	// if it's blank, put a message out to the error context
	if( bstrCommandLine.Length() == 0 )
	{
		CComBSTR bstrMsg;
		bstrMsg.LoadString( IDS_NOREBUILDCOMMANDLINEMSG );
		pEC->AddInfo( bstrMsg );
	}
	else
	{
		CComBSTR bstrRealCmd = bstrCommandLine;
		BuildCommandLineBatchFile(pAction, bstrRealCmd, bstrCommandLine);
	}

	if (hr != S_OK)
	{
		bstrCommandLine.CopyTo(pVal);
		return S_FALSE;
	}

	if (pBuildEngine == NULL)
		return bstrCommandLine.CopyTo(pVal);

	return pBuildEngine->FormBatchFile(bstrCommandLine, pEC, pVal);
}

STDMETHODIMP CVCNMakeTool::IsTargetTool(IVCBuildAction* pAction, VARIANT_BOOL* pbTargetTool)
{
	CHECK_POINTER_NULL( pbTargetTool );
	*pbTargetTool = VARIANT_TRUE;	// this is a target tool
	return S_OK;
}

STDMETHODIMP CVCNMakeTool::get_ToolShortName(BSTR* pbstrToolName)
{
	CHECK_POINTER_NULL(pbstrToolName);
	*pbstrToolName = SysAllocString( szNMakeToolShortName );
	return S_OK;
}

STDMETHODIMP CVCNMakeTool::MatchName(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches)
{
	return DoMatchName(bstrName, szNMakeToolType, szNMakeToolShortName, pbMatches);
}

STDMETHODIMP CVCNMakeTool::get_IsComspecTool(VARIANT_BOOL* pbIsComspecTool)
{
	CHECK_POINTER_NULL( pbIsComspecTool );
	*pbIsComspecTool = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CVCNMakeTool::SaveObject(IStream *xml, IVCPropertyContainer *pPropContainer, long nIndent)
{
	VSASSERT(pPropContainer, "Cannot save properties without a property container.");
	RETURN_INVALID_ON_NULL(pPropContainer);

	StartNodeHeader( xml, L"Tool", false );

	// Name
	CComBSTR bstrToolShortName;
	get_ToolShortName(&bstrToolShortName);
	if (bstrToolShortName.Length() > 0)
	{
		NodeAttribute( xml, L"Name", bstrToolShortName ); 					
	}
	AddSaveLine( xml, pPropContainer, L"BuildCommandLine", VCNMAKEID_BuildCommandLine );
	AddSaveLine( xml, pPropContainer, L"ReBuildCommandLine", VCNMAKEID_ReBuildCommandLine );
	AddSaveLine( xml, pPropContainer, L"CleanCommandLine", VCNMAKEID_CleanCommandLine );
	AddSaveLine( xml, pPropContainer, L"Output", VCNMAKEID_Output );

	// end of node header
	EndNodeHeader( xml, false );
	// end of node
	EndNode( xml, L"Tool", false );

	return S_OK;
}

void CVCNMakeTool::AddSaveLine(IStream *xml, IVCPropertyContainer* pPropContainer, LPOLESTR bszPropertyName, long nPropertyID)
{
	CComBSTR bstrProp;
	if (pPropContainer->GetStrProperty(nPropertyID, &bstrProp) != S_OK)
		return;

	NodeAttributeWithSpecialChars( xml, bszPropertyName, bstrProp );
}

STDMETHODIMP CVCNMakeTool::get_ToolDisplayIndex(long* pIndex)
{
	CHECK_POINTER_NULL(pIndex);
	*pIndex = TOOL_DISPLAY_INDEX_NMAKE;
	return S_OK;
}

GUID* CVCNMakeTool::GetPageIDs()
{ 
	if (s_bPagesInit == FALSE)
	{
		s_pPages[0] = __uuidof(VCNMakeGeneral);
		s_bPagesInit = TRUE;
	}
	return s_pPages; 
}

LPCOLESTR CVCNMakeTool::GetToolFriendlyName()
{
	InitToolName();
	return s_bstrToolName;
}

void CVCNMakeTool::InitToolName()
{
	if (s_bstrToolName.Length() == 0)
	{
		if (!s_bstrToolName.LoadString(IDS_NMAKE_TOOLNAME))
			s_bstrToolName = szNMakeToolType;
	}
}

STDMETHODIMP CVCNMakeTool::HasPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, VARIANT_BOOL* pbHasPrimaryOutput)
{ 
	return COptionHandlerBase::GetValueTrue(pbHasPrimaryOutput);
}

STDMETHODIMP CVCNMakeTool::GetPrimaryOutputFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, BSTR* pbstrPath)
{ 
	CHECK_POINTER_NULL(pbstrPath);
	*pbstrPath = NULL;

	CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
	RETURN_ON_NULL2(spPropContainer, S_FALSE);

	CComBSTR bstrTemp, bstrFile;
	HRESULT hr = spPropContainer->GetStrProperty(VCNMAKEID_Output, &bstrTemp);
	if (hr != S_OK)
		return hr;
	hr = spPropContainer->Evaluate(bstrTemp, &bstrFile);
	RETURN_ON_FAIL(hr);

	CComBSTR bstrProjDir;
	hr = spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjDir);
	RETURN_ON_FAIL(hr);

	CDirW dirProj;
	if (!dirProj.CreateFromKnown(bstrProjDir))
		return E_UNEXPECTED;

	CPathW pathOut;
	if (pathOut.CreateFromDirAndRelative(dirProj, bstrFile))
		bstrFile = (const wchar_t*)pathOut;
	
	*pbstrPath = bstrFile.Detach();
	return S_OK;
}

STDMETHODIMP CVCNMakeTool::GetPrimaryOutputIDFromTool(IUnknown* pItem, VARIANT_BOOL bSchmoozeOnly, long* pnOutputID)
{
	CHECK_POINTER_NULL(pnOutputID);
	*pnOutputID = VCNMAKEID_Output;

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// Affects our output?
STDMETHODIMP CVCNMakeTool::AffectsOutput(long nPropID, VARIANT_BOOL* pbAffectsOutput)
{
	CHECK_POINTER_NULL(pbAffectsOutput);
	*pbAffectsOutput = VARIANT_FALSE;

	// change our outputs?
	if (nPropID == VCNMAKEID_Output || nPropID == VCNMAKEID_OutputsDirty)
		*pbAffectsOutput = VARIANT_TRUE;

	return S_OK;
}

STDMETHODIMP CVCNMakeTool::CreatePageObject(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
{
	// Get the list of pages
	if( CLSID_VCNMakeGeneral == *pCLSID )
	{
		CPageObjectImpl<CVCNMakePage, VCNMAKETOOL_MIN_DISPID, VCNMAKETOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else
	{
		return S_FALSE;
	}
		
	return S_OK;
}


STDMETHODIMP CVCNMakePage::get_BuildCommandLine(BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	CStringW str;
	if(GetTruncatedStringForProperty(VCNMAKEID_BuildCommandLine, str))
	{
		*pVal = str.AllocSysString();
		return S_OK;
	}
	return GetStrProperty(VCNMAKEID_BuildCommandLine, pVal);
}

STDMETHODIMP CVCNMakePage::put_BuildCommandLine(BSTR newVal)
{
	HRESULT hr = SetStrProperty(VCNMAKEID_BuildCommandLine, newVal);
	RefreshPropGrid();
	return hr;
}

STDMETHODIMP CVCNMakePage::get_ReBuildCommandLine(BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	CStringW str;
	if(GetTruncatedStringForProperty(VCNMAKEID_ReBuildCommandLine, str))
	{
		*pVal = str.AllocSysString();
		return S_OK;
	}
	return GetStrProperty(VCNMAKEID_ReBuildCommandLine, pVal);
}

STDMETHODIMP CVCNMakePage::put_ReBuildCommandLine(BSTR newVal)
{
	HRESULT hr = SetStrProperty(VCNMAKEID_ReBuildCommandLine, newVal);
	RefreshPropGrid();
	return hr;
}

STDMETHODIMP CVCNMakePage::get_CleanCommandLine(BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	CStringW str;
	if(GetTruncatedStringForProperty(VCNMAKEID_CleanCommandLine, str))
	{
		*pVal = str.AllocSysString();
		return S_OK;
	}
	return GetStrProperty(VCNMAKEID_CleanCommandLine, pVal);
}

STDMETHODIMP CVCNMakePage::put_CleanCommandLine(BSTR newVal)
{
	HRESULT hr = SetStrProperty(VCNMAKEID_CleanCommandLine, newVal);
	RefreshPropGrid();
	return hr;
}

STDMETHODIMP CVCNMakePage::get_Output(BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	return GetStrProperty(VCNMAKEID_Output, pVal);
}

STDMETHODIMP CVCNMakePage::put_Output(BSTR newVal)
{
	return SetStrProperty(VCNMAKEID_Output, newVal);
}

