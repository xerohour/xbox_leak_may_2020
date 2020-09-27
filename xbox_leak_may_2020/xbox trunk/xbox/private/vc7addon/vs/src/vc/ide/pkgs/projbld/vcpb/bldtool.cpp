#include "stdafx.h"
#include "BldTool.h"
#include "BuildEngine.h"
#include "msgboxes2.h"

CBldToolWrapper::CBldToolWrapper(IVCToolImpl* pTool)
{
	m_spTool = pTool;

	m_fCustomTool = FALSE;
	m_fCustomToolInit = FALSE;
	m_fToolNameInit = FALSE;
	m_fCustomBuildToolNameInit = FALSE;
	m_fExtensionsInit = FALSE;
	m_fExtensionsValid = FALSE;
	m_fOrderInit = FALSE;
	m_nOrder = 0;
	m_fPerformIfAbleInit = FALSE;
	m_fPerformIfAble = FALSE;
	m_fPrimaryOutputInit = FALSE;
	m_fPrimaryOutput = FALSE;
}

CBldToolWrapper::~CBldToolWrapper()
{
}

BOOL CBldToolWrapper::IsTargetTool(IVCBuildAction* pAction)
{
	VARIANT_BOOL bVal = VARIANT_FALSE;
	if (!m_spTool || FAILED(m_spTool->IsTargetTool(pAction, &bVal)))
		bVal = VARIANT_FALSE;

	return (bVal == VARIANT_TRUE);
}

BOOL CBldToolWrapper::HasPrimaryOutput()
{
	if (!m_fPrimaryOutputInit)
	{
		m_fPrimaryOutputInit = TRUE;
		VARIANT_BOOL bHasPrimaryOutput = VARIANT_FALSE;
		if (m_spTool != NULL && SUCCEEDED(m_spTool->HasPrimaryOutputFromTool(NULL, VARIANT_TRUE, &bHasPrimaryOutput)) 
			&& bHasPrimaryOutput == VARIANT_TRUE)
			m_fPrimaryOutput = TRUE;
	}
	return m_fPrimaryOutput;
}

void CBldToolWrapper::InitToolName()
{
	if (m_fToolNameInit)
		return;		// already initialized

	m_fToolNameInit = TRUE;
	GetToolName(m_spTool, m_strToolName);
}

void CBldToolWrapper::GetToolName(IVCToolImpl* pTool, CStringW& rstrToolName)
{
	rstrToolName.Empty();
	if (pTool == NULL)	// want current tool
	{
		InitToolName();
		rstrToolName = m_strToolName;
		return;
	}

	CComBSTR bstrToolName;
	pTool->get_ToolShortName(&bstrToolName);
	rstrToolName = bstrToolName;	
}

BOOL CBldToolWrapper::SameToolType(IVCToolImpl* pTool)
{
	InitToolName();
	CStringW strCheckName;
	GetToolName(pTool, strCheckName);
	return (m_strToolName == strCheckName);
}

BOOL CBldToolWrapper::SameToolType(CStringW& rstrCheckName)
{
	InitToolName();
	return (m_strToolName == rstrCheckName);
}

BOOL CBldToolWrapper::CustomToolType()
{
	if (!m_fCustomToolInit)
	{
		m_fCustomToolInit = TRUE;
		m_fCustomTool = FALSE;
		VARIANT_BOOL bVal = VARIANT_FALSE;
		if (m_spTool != NULL && SUCCEEDED(m_spTool->get_IsCustomBuildTool(&bVal)))
			m_fCustomTool = (bVal == VARIANT_TRUE);
	}
	return m_fCustomTool;
}

BOOL CBldToolWrapper::GetDefaultExtensions(CStringW& rstrExtensions)
{
	if (!m_fExtensionsInit)
	{
		m_fExtensionsInit = TRUE;
		CComBSTR bstrExtensions;
		HRESULT hr = VCPROJ_E_INTERNAL_ERR;
		if (m_spTool)
			hr = m_spTool->get_DefaultExtensions(&bstrExtensions);
		m_strExtensions = bstrExtensions;
		m_fExtensionsValid = SUCCEEDED(hr);
	}
	rstrExtensions = m_strExtensions;
	return m_fExtensionsValid;
}

long CBldToolWrapper::GetOrder()
{
	if (m_fOrderInit == FALSE)
	{
		m_fOrderInit = TRUE;
		if (m_spTool != NULL)
		{
			HRESULT hr = m_spTool->get_Bucket(&m_nOrder);
			VSASSERT(SUCCEEDED(hr), "All tools must provide a bucket number!");
		}
	}
	return m_nOrder;
}

BOOL CBldToolWrapper::PerformIfAble()
{
	if (m_fPerformIfAbleInit == FALSE)
	{
		m_fPerformIfAbleInit = TRUE;
		if (m_spTool != NULL)
		{
			VARIANT_BOOL bPerformIfAble = VARIANT_FALSE;
			HRESULT hr = m_spTool->get_PerformIfAble(&bPerformIfAble);
			VSASSERT(SUCCEEDED(hr), "All tools must be able to say whether they can perform an action if able!");
			m_fPerformIfAble = (bPerformIfAble == VARIANT_TRUE);
		}
	}
	return m_fPerformIfAble;
}

actReturnStatus CBldToolWrapper::PerformBuildActions(bldActionTypes type, CBldActionList* plstActions, 
	bldAttributes attrib, IVCBuildErrorContext* pEC, IVCBuildEngine* pBuildEngine)
{
	if (m_spTool == NULL || plstActions == NULL || !plstActions->NotEmpty())
		return ACT_Complete;	// nothing more to do

	actReturnStatus act = ACT_Complete;
	IVCBuildActionList* pIActions = plstActions;
	HRESULT hr = m_spTool->PrePerformBuildActions(type, pIActions, attrib, pEC, &act);
	if (FAILED(hr))
		return ACT_Error;
	else if (act != ACT_Complete || hr == S_FALSE)
		return act;

	CComPtr<VCProject> spProject;
	hr = plstActions->get_Project(&spProject);
	VSASSERT(SUCCEEDED(hr), "Actions list must have a project!");

	// perform the pre-build?
//	if (stage == TOB_Pre)
	{
		BOOL bSchmooze = IsTargetTool(NULL);
		VCPOSITION pos = plstActions->GetStart(); 
		while (pos != (VCPOSITION)NULL && (act == ACT_Complete) /* failure */)
		{
			CBldAction* pAction = (CBldAction *)plstActions->Next(pos);
			CBldFileRegSet* psetOutput = pAction->GetOutput(pEC); // want full list of output files
			if(psetOutput)
			{
				psetOutput->InitFrhEnum();
				BldFileRegHandle frh;
				while ((frh = psetOutput->NextFrh()) != (BldFileRegHandle)NULL)
				{
					// need to create directory for output?
					if (type != TOB_Clean) 
					{
						CDirW dir;	// directory of output file
						if (dir.CreateFromPath(*pAction->m_pregistry->GetRegEntry(frh)->GetFilePath()))
						{
							if (pBuildEngine->AddToOutputDirectoryList((wchar_t *)(const wchar_t *)dir) == S_OK)	// not there before
							{
								if (!dir.ExistsOnDisk())
								{
									// directory doesn't exist on disk?
									// try to create
									if (!dir.CreateOnDisk())
									{
										// error, 'failed to create directory'
										CStringW str;
										::VCMsgTextW(str, IDS_COULD_NOT_CREATE_DIR_PRJ0007, (const wchar_t *) dir);
										CVCProjectEngine::AddProjectError(pEC, str, L"PRJ0007", plstActions);
			
										if (!(attrib & AOB_IgnoreErrors))
										{
											act = ACT_Error; 
											break;
										}
									}
								}
							}
						}
					}
					else // delete output files
					{		
						// Special handling is required for schmooze tool users, since we may need to
						// explicitly delete certain targets (such as .ILK files) in order to
						// guarantee a clean build, however we may not want to delete other targets
						// (such as the browser file) which are useful during the build.  Thus we keep
						// separate filters for targets which are "Products" (i.e. time-stamping) and
						// targets to "Delete on Rebuild" for a clean buld.
						if (bSchmooze)
						{
							// FUTURE: fix this stuff when CSchmoozeTool merges with CSourceTool
							// linker deletes all outputs, bscmake none
							if (type != TOB_Clean)
							{
								CStringW strPath = *(g_FileRegistry.GetRegEntry(frh)->GetFilePath());
								CComBSTR bstrPath = strPath;
								VARIANT_BOOL bDelOnRebuild = VARIANT_FALSE;
								if (SUCCEEDED(m_spTool->IsDeleteOnRebuildFile(bstrPath, &bDelOnRebuild)) &&
									(bDelOnRebuild == VARIANT_TRUE))
								{
									frh->ReleaseFRHRef();
									continue;
								}
							}
						}
	
						// if we can't delete this output and we're not forcing a build then error
						if (!g_StaticBuildEngine.DeleteFile(frh, pEC, NULL, pIActions) && !(attrib & AOB_IgnoreErrors))		  
						{
							act = ACT_Error; 
							break;
						}
					}
					frh->ReleaseFRHRef();
				}
			}
		}
	}

	// get the tool type
	bool bIsNMakeTool = false;
	CComQIPtr<VCNMakeTool> pNMakeTool = m_spTool;
	if( pNMakeTool )
		bIsNMakeTool = true;
	
	// actually perform the build?
//	else if (stage == TOB_Stage)
	// build, clean or rebuild for makefile tool
	if( type != TOB_Clean || ((type == TOB_Clean || type == TOB_ReBuild) && bIsNMakeTool) )
	{
		CComPtr<IVCCommandLineList> spCmds;

		if (pBuildEngine == NULL)
			act = ACT_Error;	// failure
		else if( bIsNMakeTool && type == TOB_Clean )
		{
			// handle the makefile tool clean case here
			// (build a clean command line and run it)
			if( FAILED( m_spTool->GetCommandLinesForClean( pIActions, attrib, pBuildEngine, pEC, &spCmds ) ) )
			{
				CStringW strError;
				strError.LoadString(IDS_GET_COMMAND_LINE_FAILED_PRJ0004);

				CStringW strTmp;
				GetToolName(NULL, strTmp);
				strError += strTmp;
				CComBSTR bstrError = strError + strTmp;

				CVCProjectEngine::AddProjectError(pEC, bstrError, L"PRJ0004", pIActions);
				if( !(attrib & AOB_IgnoreErrors ) )
					act = ACT_Error; // failure
			}
			// clean can't batch
			if( attrib & AOB_IgnoreErrors )
				attrib = AOB_Default;
		}
		else if( bIsNMakeTool && type == TOB_ReBuild )
		{
			// handle the makefile tool rebuild case here
			// (build a rebuild command line and run it)
			if( FAILED( m_spTool->GetCommandLinesForRebuild( pIActions, attrib, pBuildEngine, pEC, &spCmds ) ) )
			{
				CStringW strError;
				strError.LoadString(IDS_GET_COMMAND_LINE_FAILED_PRJ0004);

				CStringW strTmp;
				GetToolName(NULL, strTmp);
				strError += strTmp;
				CComBSTR bstrError = strError + strTmp;

				CVCProjectEngine::AddProjectError(pEC, bstrError, L"PRJ0004", pIActions);
				if( !(attrib & AOB_IgnoreErrors ) )
					act = ACT_Error; // failure
			}
// 			// clean can't batch
// 			if( attrib & AOB_IgnoreErrors )
// 				attrib = AOB_Default;
		}
 		else if (FAILED(m_spTool->GetCommandLinesForBuild(pIActions, attrib, pBuildEngine, pEC, &spCmds)))
		{
			CComBSTR bstrError;
			bstrError.LoadString(IDS_GET_COMMAND_LINE_FAILED_PRJ0004);
			CVCProjectEngine::AddProjectError(pEC, bstrError, L"PRJ0004", pIActions);
			if (!(attrib & AOB_IgnoreErrors))
				act = ACT_Error;	// failure
		}

		// ok to continue with what we have?
		if (act == ACT_Complete && spCmds != NULL)
		{
			// check for need to preprend command with comspec if custom build tool
			VARIANT_BOOL bCheckForComSpec = VARIANT_FALSE;
			hr = m_spTool->get_IsComspecTool(&bCheckForComSpec);
			VSASSERT(SUCCEEDED(hr), "All tools must be able to specify whether or not they're a comspec tool!");
			CMD cmd = CMD_Complete;

			CComBSTR bstrProjDir;
			if (spProject != NULL)
			{
				hr = spProject->get_ProjectDirectory(&bstrProjDir);
				VSASSERT(SUCCEEDED(hr), "Must always be able to the project directory!");
			}
			VARIANT_BOOL bIgnoreErrs = (attrib & AOB_IgnoreErrors) ? VARIANT_TRUE : VARIANT_FALSE;
			pBuildEngine->ExecuteCommandLines(spCmds, bstrProjDir, bCheckForComSpec, bIgnoreErrs, pEC, &cmd);
			
			switch (cmd)
			{
				case CMD_Complete:
					act = ACT_Complete;
					break;
				case CMD_Canceled:
					act = ACT_Canceled;
					break;
				case CMD_Error:
					act = ACT_Error;
					break;
				default:
					VSASSERT(FALSE, "Unhandled CMD type");
					break;
			}
		}
	// perform the post-build?
//	else if (stage == TOB_Post)
//			// all of our outputs of the action are dirty
		VCPOSITION pos = plstActions->GetStart();
		while (pos != (VCPOSITION)NULL)
		{
			CBldAction* pAction = (CBldAction *)plstActions->Next(pos);
			CBldFileRegSet* pregset = pAction->GetOutput(pEC);
			pregset->MakeContentsDirty(pAction->m_pregistry);
		}
	}

	if (m_spTool != NULL)
		m_spTool->PostPerformBuildActions(type, pIActions, attrib, pEC, &act);

	return act;
}

BOOL CBldToolWrapper::HasDependencies(IVCBuildAction* pAction)
{
	RETURN_ON_NULL2(m_spTool, FALSE);
	RETURN_ON_NULL2(pAction, FALSE);

	VARIANT_BOOL bHasDependencies = VARIANT_FALSE;
	if (FAILED(m_spTool->HasDependencies(pAction, &bHasDependencies)))
		return FALSE;

	return (bHasDependencies == VARIANT_TRUE);
}

BOOL CBldToolWrapper::CanScanForDependencies()
{
	RETURN_ON_NULL2(m_spTool, FALSE);
	
	VARIANT_BOOL bCanScan = VARIANT_FALSE;
	if (FAILED(m_spTool->CanScanForDependencies(&bCanScan)))
		return FALSE;

	return (bCanScan == VARIANT_TRUE);
}

BOOL CBldToolWrapper::ScanDependencies(IVCBuildAction* pBldAction, IVCBuildErrorContext* pEC, BOOL bWriteOutput)
{
	RETURN_ON_NULL2(m_spTool, FALSE);
	HRESULT hr = m_spTool->ScanDependencies(pBldAction, pEC, (bWriteOutput == TRUE) ? VARIANT_TRUE : VARIANT_FALSE);
	return (hr == S_OK);
}

BOOL CBldToolWrapper::GetDependencies(IVCBuildAction* pAction, CVCStringWArray& rstrArray, BOOL* pbUpToDate /* = NULL */)
{
	if (m_spTool == NULL || pAction == NULL)
		return FALSE;

	CComPtr<IVCBuildStringCollection> spStrings;
	VARIANT_BOOL bUpToDate = VARIANT_FALSE;
	HRESULT hr = m_spTool->GetDependencies(pAction, &spStrings, &bUpToDate);
	if (pbUpToDate != NULL)
		*pbUpToDate = (bUpToDate == VARIANT_TRUE);
	RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, spStrings, FALSE);

	spStrings->Reset();
	while (TRUE)
	{
		CComBSTR bstrString;
		HRESULT hr = spStrings->Next(&bstrString);
		if (hr != S_OK)
			break;
		CStringW strString = bstrString;
		if (strString.IsEmpty())
			continue;
		rstrArray.Add(strString);
	}

	return TRUE;
}

BOOL CBldToolWrapper::GetDeploymentDependencies(IVCBuildAction* pBldAction, IVCBuildStringCollection** ppDeployDeps)
{
	if (m_spTool == NULL || pBldAction == NULL)
		return FALSE;

	return (SUCCEEDED(m_spTool->GetDeploymentDependencies(pBldAction, ppDeployDeps)));
}

// Generate the corresponding outputs for each of the input file sets in each of the actions
// (errors should be pushed into the error context pEC)
// The 'type' is an AOGO_.
BOOL CBldToolWrapper::GenerateOutput(long type, IVCBuildActionList* pActions, IVCBuildErrorContext* pEC)
{
	RETURN_ON_NULL2(m_spTool, FALSE);
	RETURN_ON_NULL2(pActions, FALSE);

	HRESULT hr = m_spTool->GenerateOutput(type, pActions, pEC);
	return (hr == S_OK);
}

BOOL CBldToolWrapper::AffectsOutput(long nPropID)
{
	RETURN_ON_NULL2(m_spTool, FALSE);

	VARIANT_BOOL bAffectsOutput = VARIANT_FALSE;
	if (FAILED(m_spTool->AffectsOutput(nPropID, &bAffectsOutput)))
		return FALSE;

	return (bAffectsOutput == VARIANT_TRUE);
}

BOOL CBldToolWrapper::GetCommandLineOptions(IUnknown* pItem, IVCBuildAction* pAction, CStringW& rstrOptions)
{
	RETURN_ON_NULL2(m_spTool, FALSE);
	CComBSTR bstrOptions;
	HRESULT hr = m_spTool->GetCommandLineOptions(pItem, pAction, VARIANT_TRUE, cmdLineForRefresh, &bstrOptions);
	rstrOptions = bstrOptions;
	return (SUCCEEDED(hr));
}

BOOL CBldToolWrapper::IsSpecialConsumable(LPCOLESTR szPath)
{
	RETURN_ON_NULL2(m_spTool, FALSE);
	VARIANT_BOOL bSpecial = VARIANT_FALSE;
	m_spTool->IsSpecialConsumable(szPath, &bSpecial);
	return (bSpecial == VARIANT_TRUE);
}

BOOL CBldToolWrapper::AffectsTool(long nLowPropID, long nHighPropID)
{
	RETURN_ON_NULL2(m_spTool, FALSE);
	return (m_spTool->AffectsTool(nLowPropID, nHighPropID, NULL) == S_OK);
}

BOOL CBldToolWrapper::CommandLineOptionsAreDirty(IVCBuildableItem* pItem)
{
	RETURN_ON_NULL2(m_spTool, FALSE);

	CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
	RETURN_ON_NULL2(spPropContainer, FALSE);

	VARIANT_BOOL bDirty = VARIANT_FALSE;
	if (m_spTool->CommandLineOptionsAreDirty(spPropContainer, &bDirty) != S_OK)
		return FALSE;

	return (bDirty == VARIANT_TRUE);
}

BOOL CBldToolWrapper::OutputsAreDirty(IVCBuildableItem* pItem)
{
	RETURN_ON_NULL2(m_spTool, FALSE);

	CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
	RETURN_ON_NULL2(spPropContainer, FALSE);

	VARIANT_BOOL bDirty = VARIANT_FALSE;
	if (m_spTool->OutputsAreDirty(spPropContainer, &bDirty) != S_OK)
		return FALSE;

	return (bDirty == VARIANT_TRUE);
}

BOOL CBldToolWrapper::ClearDirtyOutputs(IVCBuildableItem* pItem)
{
	RETURN_ON_NULL2(m_spTool, FALSE);

	CComQIPtr<IVCPropertyContainer> spPropContainer = pItem;
	RETURN_ON_NULL2(spPropContainer, FALSE);

	return (m_spTool->ClearDirtyOutputs(spPropContainer) == S_OK);
}
