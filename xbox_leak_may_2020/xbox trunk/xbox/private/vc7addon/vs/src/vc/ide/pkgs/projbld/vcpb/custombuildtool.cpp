// CustomBuildTool.cpp : Implementation of CVCCustomBuildTool and CVCCustomBuildToolShadow

#include "stdafx.h"
#include "CustomBuildTool.h"
#include "FileRegistry.h"
#include "BldHelpers.h"
#include "BldActions.h"
#include "ProjWriter.h"

#include "DTE.h"
#include "ProjBld2.h"


// constants
const wchar_t* const wszCustomBldDefaultExtensions = L"*.bat";	// remember to update vcpb.rgs if you change this...

// init statics
GUID CVCCustomBuildTool::s_pPages[1];
BOOL CVCCustomBuildTool::s_bPagesInit = FALSE;
CComBSTR CVCCustomBuildTool::s_bstrDescription = L"";
CComBSTR CVCCustomBuildTool::s_bstrToolName = L"";
CComBSTR CVCCustomBuildTool::s_bstrExtensions = L"";

#define PLACEHOLDER_OUTPUT L"somethingthatwillneverexist.junk"

//////////////////////////////////////////////////////////////////////////////
// implementation of CVCCustomBuildTool; behavior specific to custom build rules with storage

LPCOLESTR CVCCustomBuildTool::GetToolFriendlyName()
{
	InitToolName();
	return s_bstrToolName;
}

void CVCCustomBuildTool::InitToolName()
{
	if (s_bstrToolName.Length() == 0)
	{
		if (!s_bstrToolName.LoadString(IDS_CUSTOM_TOOLNAME))
			s_bstrToolName = szCustomBuildToolType;
	}
}

HRESULT CVCCustomBuildTool::CreateInstance(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppTool)
{
	CHECK_POINTER_NULL(ppTool);
	*ppTool = NULL;

	CComObject<CVCCustomBuildTool> *pObj;
	HRESULT hr = CComObject<CVCCustomBuildTool>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		if( pPropContainer )
			pObj->Initialize(pPropContainer);
		CVCCustomBuildTool *pVar = pObj;
		pVar->AddRef();
		*ppTool = pVar;
	}
	return hr;
}

void CVCCustomBuildTool::Initialize(IVCPropertyContainer* pPropContainer)
{
	CVCToolImpl::Initialize(pPropContainer);
	CComQIPtr<VCFileConfiguration> spFileCfg = pPropContainer;
	if (spFileCfg == NULL)
		m_nOffset = 0;	// we're on the project configuration
	else
		m_nOffset = VCCUSTID_FileLevelOffset;	// we're on a file configuration

	m_nLowKey = VCCUSTID_CommandLine + m_nOffset;
	m_nHighKey = VCCUSTID_AdditionalDependencies + m_nOffset;
	m_nDirtyKey = VCCUSTID_CmdLineOptionsDirty + m_nOffset;
	m_nDirtyOutputsKey = VCCUSTID_OutputsDirty + m_nOffset;
}

STDMETHODIMP CVCCustomBuildTool::get_ToolName(BSTR* pbstrToolName)	// friendly name of tool, e.g., "C/C++ Compiler Tool"
{
	CHECK_POINTER_VALID(pbstrToolName);
	InitToolName();
	s_bstrToolName.CopyTo(pbstrToolName);
	return S_OK;
}

STDMETHODIMP CVCCustomBuildTool::get_CommandLine(BSTR *pbstrCommandLine)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POINTER_VALID( pbstrCommandLine );

	HRESULT hr = m_spPropertyContainer->GetStrProperty(VCCUSTID_CommandLine + m_nOffset, pbstrCommandLine);
	if (hr == S_FALSE)
		COptionHandlerBase::GetDefaultString( pbstrCommandLine );
	return hr;
}

STDMETHODIMP CVCCustomBuildTool::put_CommandLine(BSTR bstrCommandLine)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );

	// grab each line and strip leading/trailing whitespace from it
	CStringW strCommandLine = bstrCommandLine;
	CStringW strLine, strNewCommandLine;
	int length = strCommandLine.GetLength();
	int left = 0;
	int right;
	while( true )
	{
		// find the next newline
		right = strCommandLine.Find( L'\n', left );
		// if we couldn't find one
		if( right == -1 )
		{
			// if we're at the end of the cmd line, we're done	
			if( left >= length )
				break;
			// otherwise this is the last line
			else
				right = length;
		}

		// grab this portion (logical line) of the cmd line
		strLine = strCommandLine.Mid( left, right - left );
		// trim the leading/trailing whitespace
		strLine.TrimLeft();
		strLine.TrimRight();
		// append it to the new command line
		strNewCommandLine += strLine;
		strNewCommandLine += L"\r\n";
		// move along to the next logical line
		left = right + 1;
	}
	if( strNewCommandLine.IsEmpty() )
		strNewCommandLine = strCommandLine;
	
	return m_spPropertyContainer->SetStrProperty(VCCUSTID_CommandLine + m_nOffset, CComBSTR( strNewCommandLine ) );
}

STDMETHODIMP CVCCustomBuildTool::get_Description(BSTR *pbstrDescription)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POINTER_VALID( pbstrDescription );

	HRESULT hr = m_spPropertyContainer->GetStrProperty(VCCUSTID_Description + m_nOffset, pbstrDescription);
	if (hr == S_FALSE)
 		SGetDescription(pbstrDescription);
	return hr;
}

STDMETHODIMP CVCCustomBuildTool::put_Description(BSTR bstrDescription)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCUSTID_Description + m_nOffset, bstrDescription);
}

STDMETHODIMP CVCCustomBuildTool::get_Outputs(BSTR *pbstrOutputs)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POINTER_VALID( pbstrOutputs );

	HRESULT hr = m_spPropertyContainer->GetStrProperty(VCCUSTID_Outputs + m_nOffset, pbstrOutputs);
	if (hr == S_FALSE)
		COptionHandlerBase::GetDefaultString( pbstrOutputs );
	return hr;
}

STDMETHODIMP CVCCustomBuildTool::put_Outputs(BSTR bstrOutputs)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCUSTID_Outputs + m_nOffset, bstrOutputs);
}

STDMETHODIMP CVCCustomBuildTool::get_AdditionalDependencies(BSTR *pbstrAdditionalDependencies)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	CHECK_POINTER_VALID( pbstrAdditionalDependencies );

	HRESULT hr = m_spPropertyContainer->GetStrProperty(VCCUSTID_AdditionalDependencies + m_nOffset, pbstrAdditionalDependencies);
	if (hr == S_FALSE)
		COptionHandlerBase::GetDefaultString( pbstrAdditionalDependencies );
	return hr;
}

STDMETHODIMP CVCCustomBuildTool::put_AdditionalDependencies(BSTR bstrAdditionalDependencies)
{
	CHECK_ZOMBIE( m_spPropertyContainer, IDS_ERR_TOOL_ZOMBIE );
	return m_spPropertyContainer->SetStrProperty(VCCUSTID_AdditionalDependencies + m_nOffset, bstrAdditionalDependencies);
}

// Automation properties
STDMETHODIMP CVCCustomBuildTool::get_ToolPath(BSTR *pbstrToolPath)
{
	*pbstrToolPath = SysAllocString( szCustomBuildToolPath );
	return S_OK;
}


// IVCToolImpl
STDMETHODIMP CVCCustomBuildTool::get_DefaultExtensions(BSTR* pVal)
{
	return DoGetDefaultExtensions(s_bstrExtensions, wszCustomBldDefaultExtensions, pVal);
}

STDMETHODIMP CVCCustomBuildTool::put_DefaultExtensions(BSTR newVal)
{
	s_bstrExtensions = newVal;
	return S_OK;
}

STDMETHODIMP CVCCustomBuildTool::GetAdditionalOptionsInternal(IUnknown* pItem, VARIANT_BOOL bForBuild, VARIANT_BOOL bSkipLocal, 
	BSTR* pbstrOptions)
{ 
	return E_NOTIMPL;	// don't have any; but don't want assert
}

STDMETHODIMP CVCCustomBuildTool::GetBuildDescription(IVCBuildAction* pAction, BSTR* pbstrBuildDescription)
{
	CHECK_POINTER_NULL( pbstrBuildDescription );

	if (pAction == NULL)
		return SGetDescription( pbstrBuildDescription );

	CComPtr<IVCPropertyContainer> spPropContainer;
	pAction->get_PropertyContainer(&spPropContainer);
	if (spPropContainer == NULL)
		return SGetDescription( pbstrBuildDescription );

	CComQIPtr<VCFileConfiguration> spFileCfg = spPropContainer;
	int nOffset = 0;
	if (spFileCfg)
		nOffset = VCCUSTID_FileLevelOffset;	// we're on a file configuration

	CComBSTR bstrDesc;
	HRESULT hr = spPropContainer->GetStrProperty(VCCUSTID_Description + nOffset, &bstrDesc);
	if (hr == S_FALSE)
		SGetDescription( &bstrDesc );
	RETURN_ON_FAIL(hr);

	HRESULT hr2 = spPropContainer->Evaluate(bstrDesc, pbstrBuildDescription);
	RETURN_ON_FAIL(hr2);
	return hr;
}

STDMETHODIMP CVCCustomBuildTool::get_ToolPathInternal(BSTR* pbstrToolPath)
{ 
	return get_ToolPath(pbstrToolPath);
}

STDMETHODIMP CVCCustomBuildTool::get_Bucket(long *pVal)
{
	CHECK_POINTER_NULL(pVal);
	*pVal = BUCKET_CUSTOMBUILD;
	return S_OK;
}

STDMETHODIMP CVCCustomBuildTool::IsTargetTool(IVCBuildAction* pAction, VARIANT_BOOL* pbIsTargetTool)
{
	CHECK_POINTER_NULL(pbIsTargetTool);
	*pbIsTargetTool = VARIANT_TRUE;	// by default, this is a target tool

	if (pAction != NULL)
	{
		CComPtr<IVCBuildableItem> spItem;
		if (SUCCEEDED(pAction->get_Item(&spItem)) && spItem != NULL)
		{
			CComQIPtr<VCFileConfiguration> spFileCfg = spItem;
			if (spFileCfg!= NULL)	// operating on a file
				*pbIsTargetTool = VARIANT_FALSE;
		}
	}	

	return S_OK;
}

STDMETHODIMP CVCCustomBuildTool::CreateToolObject(IVCPropertyContainer* pPropContainer, IVCToolImpl** ppToolObject)
{ 
	return CreateInstance(pPropContainer, ppToolObject); 
}

STDMETHODIMP CVCCustomBuildTool::get_ToolShortName(BSTR* pbstrToolName)
{
	*pbstrToolName = SysAllocString( szCustomBuildToolShortName );
	return S_OK;
}

STDMETHODIMP CVCCustomBuildTool::MatchName(BSTR bstrName, VARIANT_BOOL bFullOnly, VARIANT_BOOL* pbMatches)
{
	return DoMatchName(bstrName, szCustomBuildToolType, szCustomBuildToolShortName, pbMatches);
}

STDMETHODIMP CVCCustomBuildTool::get_IsComspecTool(VARIANT_BOOL* pbIsComspecTool)
{
	return COptionHandlerBase::GetValueTrue(pbIsComspecTool);
}

STDMETHODIMP CVCCustomBuildTool::get_ToolDisplayIndex(long* pIndex)
{
	CHECK_POINTER_NULL(pIndex);
	*pIndex = TOOL_DISPLAY_INDEX_CUSTBLD;
	return S_OK;
}

STDMETHODIMP CVCCustomBuildTool::GenerateOutput(long type, IVCBuildActionList* pActions, IVCBuildErrorContext* pEC)
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

		CComPtr<IVCPropertyContainer> spPropContainer;
		spAction->get_PropertyContainer(&spPropContainer);
		VSASSERT(spPropContainer != NULL, "No property container associated with action.  Bad action initialization.");
		if (spPropContainer == NULL)
			continue;

		if (IsExcludedFromBuild(spPropContainer))
			continue;	// nothing to do for things excluded from build...

		int nOffset = 0;
		CComQIPtr<VCFileConfiguration> spFileCfg = spPropContainer;
		if (spFileCfg != NULL)
			nOffset = VCCUSTID_FileLevelOffset;

		CComBSTR bstrCommand;
		if (spPropContainer->GetEvaluatedStrProperty(VCCUSTID_CommandLine+nOffset, &bstrCommand) != S_OK)
		{
			spAction->ClearSourceDependencies(pEC);
			continue;	// no command, then outputs mean nothing
		}
		CStringW strCommand = bstrCommand;
		strCommand.TrimLeft();
		strCommand.TrimRight();
		if (strCommand.IsEmpty())
		{
			spAction->ClearSourceDependencies(pEC);
			continue;	// no command, then outputs mean nothing
		}

		CComBSTR bstrOutput;
		spPropContainer->GetStrProperty(VCCUSTID_Outputs+nOffset, &bstrOutput);

		CStringW strOutput = bstrOutput;
		strOutput.TrimLeft();
		strOutput.TrimRight();
		if (strOutput.IsEmpty())
			strOutput = PLACEHOLDER_OUTPUT;

		CBldFileRegistry* pRegistry = NULL;
		spAction->get_Registry((void **)&pRegistry);

		// get the project directory
		CComBSTR bstrProjectDir;
		spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjectDir);	// should never have macros in it
		CStringW strProjectDir = bstrProjectDir;
		int nLen = strProjectDir.GetLength();
		if (nLen > 0 && (strProjectDir[nLen-1] == L'\\' || strProjectDir[nLen-1] == L'/'))
			strProjectDir = strProjectDir.Left(nLen-1);

		// generate our output in the context of the project directory
		CDirW dirProj;
		dirProj.CreateFromKnown(strProjectDir);

		// process each of our output specs.
		CBldOptionList lstOutSpec(L';', FALSE, TRUE);
		lstOutSpec.SetString((const wchar_t *)strOutput);
		VCPOSITION pos = lstOutSpec.GetHeadPosition();

		while (pos != (VCPOSITION)NULL)
		{
			const wchar_t* pchOutSpec = lstOutSpec.GetNext(pos);

			// form the output
			CComBSTR bstrOut1 = pchOutSpec;
			CComBSTR bstrOut2;
			spPropContainer->Evaluate(bstrOut1, &bstrOut2);
			CStringW strOutputFile = bstrOut2;

			// create our path given this output string
			CPathW pathOutput;		// path of our output based on this spec.
			if (pathOutput.CreateFromDirAndFilename(dirProj, strOutputFile))
			{
				pathOutput.GetActualCase(TRUE);
				// only filter if not target-level custom build tool
				VARIANT_BOOL bFilter = (nOffset == VCCUSTID_FileLevelOffset) ? VARIANT_TRUE : VARIANT_FALSE;
				if (spAction->AddOutputFromPath((wchar_t *)(const wchar_t *)pathOutput, pEC, VARIANT_TRUE, bFilter, 
					VCCUSTID_Outputs, this) != S_OK)
				{
					VSASSERT(FALSE, "Failed to generate output dependencies for custom build tool");
					AddCustomBuildError(pEC, IDS_ERR_CUST_OUTPUT_FILE_INVALID_PRJ0031, L"PRJ0031", 
						IDS_ERR_CUST_OUTPUT_PROJ_INVALID_PRJ0032, L"PRJ0032", pchOutSpec, strOutputFile, spPropContainer);
					return E_FAIL;
				}
			}
			else
			{
					AddCustomBuildError(pEC, IDS_ERR_CUST_OUTPUT_FILE_INVALID_PRJ0031, L"PRJ0031", 
						IDS_ERR_CUST_OUTPUT_PROJ_INVALID_PRJ0032, L"PRJ0032", pchOutSpec, strOutputFile, spPropContainer);
					return E_FAIL;
			}
		}

		//
		// Add source deps.
		//
		spAction->ClearSourceDependencies(pEC);
		CComBSTR bstrInputs;
		if (spPropContainer->GetStrProperty(VCCUSTID_AdditionalDependencies+nOffset, &bstrInputs) == S_OK)
		{
			CStringW strInputs = bstrInputs;
			CBldOptionList lstInSpec(L';', FALSE, TRUE);
			lstInSpec.SetString((const wchar_t *)strInputs);
			VCPOSITION pos = lstInSpec.GetHeadPosition();

			while (pos != (VCPOSITION)NULL)
			{
				const wchar_t* pchInSpec = lstInSpec.GetNext(pos);

				// form the input
				CComBSTR bstrIn1 = pchInSpec;
				CComBSTR bstrIn2;
				spPropContainer->Evaluate(bstrIn1, &bstrIn2);
				CStringW strInputFile = bstrIn2;

				BldFileRegHandle frh = (BldFileRegHandle)NULL;
				CPathW cpFile;
				if (cpFile.CreateFromDirAndFilename(dirProj, strInputFile))
					frh = CBldFileRegFile::GetFileHandle(cpFile, TRUE);

				if (frh != NULL)
				{
					if (wcsstr(pchInSpec, L"..\\"))
					{
						CBldFileRegFile *pFile = (CBldFileRegFile*)g_FileRegistry.GetRegEntry(frh);
						pFile->SetOrgName(pchInSpec);
						pFile->SetFileNameRelativeToProj();
					}
					BOOL bForceRelativePath = ((pchInSpec[0] == L'.') && (pchInSpec[1] == L'.'));
					if (bForceRelativePath && frh)
						g_FileRegistry.GetRegEntry(frh)->SetRelative(TRUE);

					spAction->AddSourceDependencyFromFrh((void *)frh, pEC);
					frh->ReleaseFRHRef();
				}
				else
					{
							AddCustomBuildError(pEC, IDS_ERR_CUST_INPUT_FILE_INVALID_PRJ0033, L"PRJ0033", 
								IDS_ERR_CUST_INPUT_PROJ_INVALID_PRJ0034, L"PRJ0034", pchInSpec, strInputFile, spPropContainer);
							return E_FAIL;
					}
			}
		}
		// lastly, make sure we get the source dep of the thing we're associated with...
		if (spFileCfg != NULL)
		{
			CComPtr<IDispatch> spDispFile;
			HRESULT hr2 = spFileCfg->get_File(&spDispFile);
			VSASSERT(SUCCEEDED(hr2) && spDispFile != NULL, "No file on file config?  File config probably zombie.");
			if (SUCCEEDED(hr) && spDispFile != NULL)
			{
				CComQIPtr<IVCBuildableItem> spBldableFile = spDispFile;
				VSASSERT(spBldableFile != NULL, "File objects must support IVCBuildableItem interface");
				if (spBldableFile != NULL)
				{
					BldFileRegHandle frh = NULL;
					hr2 = spBldableFile->get_FileRegHandle((void **)&frh);
					VSASSERT(SUCCEEDED(hr2) && frh != NULL, "All files have reg handles.  NULL reg handle means file not added to registry when it was added to the project.");
					if (frh != NULL)
						spAction->AddSourceDependencyFromFrh((void *)frh, pEC);
				}
			}
		}
		else	// project config level
		{
			CComQIPtr<VCConfiguration> spProjCfg = spPropContainer;
			VSASSERT(spProjCfg != NULL, "Custom build step can only be applied to file or project config.  Must be trying to apply it to something else.");
			if (spProjCfg != NULL)
			{
				CComBSTR bstrPrimaryOutput;
				HRESULT hr2 = spProjCfg->get_PrimaryOutput(&bstrPrimaryOutput);
				VSASSERT(SUCCEEDED(hr2) && bstrPrimaryOutput.Length() != 0, "Project config *must* have a primary output.");
				if (SUCCEEDED(hr2))
				{
					hr2 = spAction->AddSourceDependencyFromString(bstrPrimaryOutput, pEC);
					VSASSERT(SUCCEEDED(hr2), "Failed to add source dependency for project config custom build step");
				}
				else
				{
					long nToolID = -1, nErrPropID = -1;
					CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = spProjCfg;
					if (spProjCfgImpl)
					{
						CComPtr<IVCToolImpl> spTool;
						spProjCfgImpl->GetPrimaryOutputFileID(&spTool, &nErrPropID);
						if (nErrPropID > 0)
							InformUserAboutBadFileName(pEC, spPropContainer, nErrPropID, bstrPrimaryOutput);
					}
				}
			}
		}
	}

	return S_OK; // success
}

void CVCCustomBuildTool::AddCustomBuildError(IVCBuildErrorContext* pEC, long idErrFile, BSTR bstrErrNumFile, long idErrProj, 
	BSTR bstrErrNumProj, const wchar_t* szWithMacros, const wchar_t* szNoMacros, IVCPropertyContainer* pPropContainer)
{
	if (pEC == NULL || pPropContainer == NULL)
		return;

	CComQIPtr<VCFileConfiguration> spFileCfg = pPropContainer;
	CStringW strErr;
	if (spFileCfg)
	{
		CComBSTR bstrFileName;
		pPropContainer->GetStrProperty(VCFILEID_Name, &bstrFileName);
		CStringW strFileName = bstrFileName;
		strErr.Format(idErrFile, strFileName, szWithMacros, szNoMacros);
		CVCProjectEngine::AddProjectError(pEC, strErr, bstrErrNumFile, pPropContainer);
	}
	else
	{
		strErr.Format(idErrProj, szWithMacros, szNoMacros);
		CVCProjectEngine::AddProjectError(pEC, strErr, bstrErrNumProj, pPropContainer);
	}
}

STDMETHODIMP CVCCustomBuildTool::GetCommandLineEx(IVCBuildAction* pAction, IVCBuildableItem* pBuildableItem, 
	IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, BSTR *pVal)
{
	CHECK_POINTER_NULL(pVal);
	*pVal = NULL;

	CComQIPtr<IVCPropertyContainer> spPropContainer = pBuildableItem;
	CHECK_ZOMBIE(spPropContainer, IDS_ERR_TOOL_ZOMBIE);

	CComQIPtr<VCFileConfiguration> spFileCfg = spPropContainer;
	int nOffset = 0;
	if (spFileCfg != NULL)
		nOffset = VCCUSTID_FileLevelOffset;

	HRESULT hr = S_OK;
	CComBSTR bstrCommandLineEnd, bstrOutput;
	if (spPropContainer->GetStrProperty(VCCUSTID_Outputs+nOffset, &bstrOutput) != S_OK || bstrOutput.Length() == 0)
	{
		bstrCommandLineEnd.LoadString(IDS_WARN_NO_CUST_OUTPUT_PRJ0029);
		if (nOffset)	// File level.  Groan.
		{
			CComBSTR bstrFile;
			spPropContainer->GetStrProperty(VCFILEID_Name, &bstrFile);
			bstrFile += L" : ";
			bstrFile = L"echo " + bstrFile;
			bstrFile += bstrCommandLineEnd;
			bstrCommandLineEnd = bstrFile;
		}
		else	// project level
			bstrCommandLineEnd = L"echo Project : " + bstrCommandLineEnd;
	}
	else
	{
		hr = spPropContainer->GetEvaluatedStrProperty(VCCUSTID_CommandLine+nOffset, &bstrCommandLineEnd);
		RETURN_ON_FAIL(hr);
	}

	CComBSTR bstrCmdLine;
	BuildCommandLineBatchFile(pAction, bstrCommandLineEnd, bstrCmdLine);
	
	if (hr != S_OK)
	{
		bstrCmdLine.CopyTo(pVal);
		return S_FALSE;
	}

	if (pBuildEngine == NULL)
		return bstrCmdLine.CopyTo(pVal);

	return pBuildEngine->FormBatchFile(bstrCmdLine, pEC, pVal);
}

STDMETHODIMP CVCCustomBuildTool::SaveObject(IStream *xml, IVCPropertyContainer *pPropContainer, long nIndent)
{
	VSASSERT(pPropContainer, "Property container required when saving tool properties");
	RETURN_INVALID_ON_NULL(pPropContainer);
	
	int nOffset = 0;
	CComQIPtr<VCFileConfiguration> spFileCfg = pPropContainer;
	if (spFileCfg != NULL)
	{
		nOffset = VCCUSTID_FileLevelOffset;
	}

	StartNodeHeader( xml, L"Tool", false );

	// Name
	CComBSTR bstrToolShortName;
	get_ToolShortName(&bstrToolShortName);
	if (bstrToolShortName.Length() > 0)
	{
		NodeAttribute( xml, L"Name", bstrToolShortName ); 					
	}
	AddSaveLine( xml, pPropContainer, L"Description", VCCUSTID_Description+nOffset );
	AddSaveLine( xml, pPropContainer, L"CommandLine", VCCUSTID_CommandLine+nOffset );
	AddSaveLine( xml, pPropContainer, L"AdditionalDependencies", VCCUSTID_AdditionalDependencies+nOffset );
	AddSaveLine( xml, pPropContainer, L"Outputs", VCCUSTID_Outputs+nOffset );

	// end of node header
	EndNodeHeader( xml, false );
	// end of node
	EndNode( xml, L"Tool", false );

	return S_OK;
}

void CVCCustomBuildTool::AddSaveLine(IStream *xml, IVCPropertyContainer* pPropContainer, LPOLESTR bszPropertyName, long nPropertyID )
{
	CComBSTR bstrProp;
	if (pPropContainer->GetStrProperty(nPropertyID, &bstrProp) != S_OK)
		return;

	// TODO: special multiline handling??
	NodeAttributeWithSpecialChars( xml, bszPropertyName, bstrProp );
}

///////////////////////////////////////////////////////////////////////////////
// Affects our output?
STDMETHODIMP CVCCustomBuildTool::AffectsOutput(long nPropID, VARIANT_BOOL* pbAffectsOutput)
{
	CHECK_POINTER_NULL(pbAffectsOutput);
	*pbAffectsOutput = VARIANT_FALSE;

	// change our outputs?
	if (m_nOffset)	// file cfg
	{
		switch(nPropID)
		{
		case VCFCSTID_Outputs:
		case VCFCSTID_OutputsDirty:
		case VCFCSTID_CommandLine:
		case VCFCFGID_ExcludedFromBuild:
			*pbAffectsOutput = VARIANT_TRUE;
		}
	}
	else
	{
		switch (nPropID)
		{
		case VCCUSTID_Outputs:
		case VCCUSTID_OutputsDirty:
		case VCCUSTID_CommandLine:
			*pbAffectsOutput = VARIANT_TRUE;
		}
	}

	return S_OK;
}

STDMETHODIMP CVCCustomBuildTool::CreatePageObject(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
{
	// Get the list of pages
	if( CLSID_VCCustomGeneral == *pCLSID )
	{
		CPageObjectImpl< CVCCustomBuildPage, VCCUSTOMBUILDTOOL_MIN_DISPID, VCCUSTOMBUILDTOOL_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
	}
	else
	{
		return S_FALSE;
	}
		
	return S_OK;
}

GUID* CVCCustomBuildTool::GetPageIDs()
{ 
	if (s_bPagesInit == FALSE)
	{
		s_pPages[0] = __uuidof(VCCustomGeneral);
		s_bPagesInit = TRUE;
	}
	return s_pPages; 
}

HRESULT CVCCustomBuildTool::SGetDescription( BSTR * pVal )
{
	if (s_bstrDescription.Length() == 0)
		s_bstrDescription.LoadString(IDS_CUSTOMBUILD_DESC);
	return s_bstrDescription.CopyTo(pVal);
}

HRESULT CVCCustomBuildTool::GetCommandLineOptions(IUnknown* pItem, IVCBuildAction* pAction, VARIANT_BOOL bIncludeAdditional, 
	VARIANT_BOOL bForDisplay, commandLineOptionStyle fStyle, CStringW& rstrCmdLine)
{
	CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
	RETURN_ON_NULL2(spPropContainer, E_NOINTERFACE);

	if (bForDisplay)	// don't do display
		return S_FALSE;

	CComQIPtr<VCFileConfiguration> spFileCfg = spPropContainer;
	CComBSTR bstrCommandLine;
	long nOffset = spFileCfg ? VCCUSTID_FileLevelOffset : 0;
	HRESULT hr = spPropContainer->GetStrProperty(VCCUSTID_CommandLine + nOffset, &bstrCommandLine);
	rstrCmdLine = bstrCommandLine;
	return hr;
}

BOOL CVCCustomBuildTool::GetProcessedOutputString(IVCPropertyContainer* pPropContainer, CStringW& strOutputs)
{
	CComQIPtr<VCConfiguration> spProjCfg = pPropContainer;
	RETURN_ON_NULL2(spProjCfg, FALSE);

	CComBSTR bstrOutput;
	HRESULT hr = pPropContainer->GetEvaluatedStrProperty(VCCUSTID_Outputs, &bstrOutput);
	if (hr != S_OK || bstrOutput.Length() == 0)
		return FALSE;

	CComBSTR bstrCmd;
	hr = pPropContainer->GetStrProperty(VCCUSTID_CommandLine, &bstrCmd);
	if (hr != S_OK || bstrCmd.Length() == 0)
		return FALSE;

	// so, we've got both outputs and a command line, so count how many elements we've got on the list
	strOutputs = bstrOutput;
	strOutputs.TrimLeft();
	strOutputs.TrimRight();
	return(!strOutputs.IsEmpty());
}

STDMETHODIMP CVCCustomBuildTool::GetDeployableOutputsCount(IVCPropertyContainer* pPropContainer, long* pnOutputs)
{	// number of deployable outputs
	CHECK_POINTER_NULL(pnOutputs);
	*pnOutputs = 0;

	CStringW strOutputs;
	if (!GetProcessedOutputString(pPropContainer, strOutputs))
		return S_OK;

	CBldOptionList lstOutSpec(L';', FALSE, TRUE);
	lstOutSpec.SetString((const wchar_t *)strOutputs);
	VCPOSITION pos = lstOutSpec.GetHeadPosition();

	// get the project directory
	CComBSTR bstrProjectDir;
	pPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjectDir);	// should never have macros in it
	CStringW strProjectDir = bstrProjectDir;

	// generate our output in the context of the project directory
	CCurDirW dirCur(strProjectDir);

	while (pos != (VCPOSITION)NULL)
	{
		const wchar_t* pchOutSpec = lstOutSpec.GetNext(pos);
		CPathW pathOutput;		// path of our output based on this spec.
		if (pathOutput.Create(pchOutSpec))
			*pnOutputs += 1;
	}

	return S_OK;
}

STDMETHODIMP CVCCustomBuildTool::GetDeployableOutputs(IVCPropertyContainer* pPropContainer, IVCBuildStringCollection** ppDeployableOutputs)
{	// the deployable outputs
	CHECK_POINTER_NULL(ppDeployableOutputs);
	*ppDeployableOutputs = NULL;

	CStringW strOutputs;
	if (!GetProcessedOutputString(pPropContainer, strOutputs))
		return S_OK;

	CBldOptionList lstOutSpec(L';', FALSE, TRUE);
	lstOutSpec.SetString((const wchar_t *)strOutputs);
	VCPOSITION pos = lstOutSpec.GetHeadPosition();

	// get the project directory
	CComBSTR bstrProjectDir;
	pPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjectDir);	// should never have macros in it
	CStringW strProjectDir = bstrProjectDir;

	// generate our output in the context of the project directory
	CCurDirW dirCur(strProjectDir);

	while (pos != (VCPOSITION)NULL)
	{
		const wchar_t* pchOutSpec = lstOutSpec.GetNext(pos);
		CPathW pathOutput;		// path of our output based on this spec.
		if (pathOutput.Create(pchOutSpec))
		{
			if (*ppDeployableOutputs == NULL)
			{
				HRESULT hr = CVCBuildStringCollection::CreateInstance(ppDeployableOutputs);
				RETURN_ON_FAIL_OR_NULL2(hr, *ppDeployableOutputs, E_OUTOFMEMORY);
			}
			CComBSTR bstrOutputFile = (const wchar_t*)pathOutput;
			(*ppDeployableOutputs)->Add(bstrOutputFile);
		}
	}

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Settings UI page
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CVCCustomBuildPage::ResetParentIfNeeded()
{
	CComQIPtr<IVCStagedPropertyContainer> spStagedContainer = m_pContainer;
	CComQIPtr<IVCPropertyContainer> spLiveContainer;
	if (spStagedContainer)
		spStagedContainer->get_Parent(&spLiveContainer);
	if (spLiveContainer == NULL)
		spLiveContainer = m_pContainer;
	CComQIPtr<VCFileConfiguration> spFileCfg = spLiveContainer;
	if (spFileCfg == NULL)
		m_nOffset = 0;	// we're on the project configuration
	else
		m_nOffset = VCCUSTID_FileLevelOffset;	// we're on a file configuration
}

STDMETHODIMP CVCCustomBuildPage::get_CommandLine(BSTR *pVal)
{
	HRESULT hr = S_OK;
	CStringW str;
	if(GetTruncatedStringForProperty(VCCUSTID_CommandLine+m_nOffset, str))
	{
		*pVal = str.AllocSysString();
		return S_OK;
	}
	else
	{
		hr = GetStrProperty(VCCUSTID_CommandLine+m_nOffset, pVal);
		if (hr == S_FALSE)
			COptionHandlerBase::GetDefaultString( pVal );
	}
	return hr;
}

STDMETHODIMP CVCCustomBuildPage::put_CommandLine(BSTR newVal)
{
	HRESULT hr = SetStrProperty(VCCUSTID_CommandLine+m_nOffset, newVal);
	RefreshPropGrid();
	return hr;
}

STDMETHODIMP CVCCustomBuildPage::get_Description(BSTR *pVal)
{
	HRESULT hr = GetStrProperty(VCCUSTID_Description+m_nOffset, pVal);
	if (hr == S_FALSE)
		CVCCustomBuildTool::SGetDescription( pVal );
	return hr;
}

STDMETHODIMP CVCCustomBuildPage::put_Description(BSTR newVal)
{
	return SetStrProperty(VCCUSTID_Description+m_nOffset, newVal);
}

STDMETHODIMP CVCCustomBuildPage::get_AdditionalDependencies(BSTR *pVal)
{
	HRESULT hr = GetStrProperty(VCCUSTID_AdditionalDependencies+m_nOffset, pVal);
	if (hr == S_FALSE)
		COptionHandlerBase::GetDefaultString( pVal );
	return hr;
}

STDMETHODIMP CVCCustomBuildPage::put_AdditionalDependencies(BSTR newVal)
{
	return SetStrProperty(VCCUSTID_AdditionalDependencies+m_nOffset, newVal);
}

STDMETHODIMP CVCCustomBuildPage::get_Outputs(BSTR *pVal)
{
	HRESULT hr = GetStrProperty(VCCUSTID_Outputs+m_nOffset, pVal);
	if (hr == S_FALSE)
		COptionHandlerBase::GetDefaultString( pVal );
	return hr;
}

STDMETHODIMP CVCCustomBuildPage::put_Outputs(BSTR newVal)
{
	return SetStrProperty(VCCUSTID_Outputs+m_nOffset, newVal);
}


void CVCCustomBuildPage::GetBaseDefault(long id, CComVariant& varValue)
{
	if (id == VCCUSTID_Description || id == (VCCUSTID_Description + m_nOffset))
	{
		CComBSTR bstrVal;
		CVCCustomBuildTool::SGetDescription( &bstrVal );
		varValue = bstrVal;
	}
	else
		CBase::GetBaseDefault(id, varValue);
}

long CVCCustomBuildPage::KludgeLocID(long id)
{
	// kludge: if this property is a file level custom build prop, we need to get to the "real" custom build prop 
	// that is in the typelib
	if( id >= VCFILECUSTOMBUILDTOOL_MIN_DISPID && id <= VCFILECUSTOMBUILDTOOL_MAX_DISPID )
		return id - VCCUSTID_FileLevelOffset;
	else
		return id;
}
