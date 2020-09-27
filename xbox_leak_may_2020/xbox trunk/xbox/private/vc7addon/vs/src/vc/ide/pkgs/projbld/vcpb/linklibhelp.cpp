#include "stdafx.h"
#include "vctool.h"
#include "bldhelpers.h"
#include "scanner.h"
#include "bldactions.h"

#include "linklibhelp.h"
#include "linktool.h"

HRESULT CLinkerLibrarianHelper::DoGenerateOutput(long type, IVCBuildActionList* plstActions, IVCBuildErrorContext* pEC, 
	BOOL bIsLinker, IVCToolImpl* pTool)
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

			HRESULT hrT = DoGetPrimaryOutputFromTool(spItem, pathName, bIsLinker);
			RETURN_ON_FAIL(hrT);

			pathName.GetActualCase(TRUE);
			hrT = spAction->AddOutputFromPath((wchar_t *)(const wchar_t *)pathName, pEC, VARIANT_TRUE, VARIANT_TRUE,
				bIsLinker ? VCLINKID_OutputFile : VCLIBID_OutputFile, pTool);
			if (hrT != S_OK)
				return hrT; 	// failure
		}

		if (!bIsLinker)	// librarian doesn't care about other outputs
			continue;

		// do we want other outputs as well?
		if (type & AOGO_Other)
		{
			RETURN_INVALID_ON_NULL(spPropContainer);

//			// linker output paths
//			// warning: this logic depends of pathName from .exe step
//			linkIncrementalType linkType;
//			if (spPropContainer->GetIntProperty(VCLINKID_LinkIncremental, (long *)&linkType) == S_OK
//				&& linkType == linkIncrementalYes)
//			{
//				pathName.ChangeExtension(L"ILK"); // generate .ilk name from .EXE name & path
//				pathName.GetActualCase(FALSE);
//				spAction->AddOutputFromPath((wchar_t *)(const wchar_t *)pathName, pEC, VARIANT_FALSE, VARIANT_TRUE,
//					bIsLinker ? VCLINKID_OutputFile : VCLIBID_OutputFile, pTool);	// no action (2ndary file)
//				// return value from AddOutputFromPath doesn't matter for 2ndary file
//			}

			// warning: this logic depends of pathName from .exe/.dll step
			VARIANT_BOOL fDll;	// is this generating a dynamic link library?
			if (spPropContainer->GetBoolProperty(VCLINKID_LinkDLL, &fDll) == S_OK && fDll == VARIANT_TRUE)
			{
				BOOL bNeedDefault = TRUE;
				CComBSTR bstrImpLib;
				if (spPropContainer->GetEvaluatedStrProperty(VCLINKID_ImportLibrary, &bstrImpLib) == S_OK)
				{
					strName = bstrImpLib;
					strName.TrimLeft();
					if (!strName.IsEmpty())
					{
						bNeedDefault = FALSE;
						CDirW baseDir;
						CVCToolImpl::CreateDirFromBSTR(bstrProjectDirectory, baseDir);
						if (!pathName.CreateFromDirAndFilename(baseDir, strName))
						{
							CBldAction::InformUserAboutBadFileName(pEC, spPropContainer, pTool, 
								VCLINKID_ImportLibrary, strName);
							return CVCProjectEngine::DoSetErrorInfo3(VCPROJ_E_BAD_PATH, IDS_ERR_CANNOT_CREATE_PATH, 
								bstrProjectDirectory, strName);
						}
						pathName.GetActualCase(TRUE);
					}
				}
				if (bNeedDefault)
				{
					// default is just .dll name
					pathName.ChangeExtension(L"LIB"); // generate .LIB name from .DLL name
					pathName.GetActualCase(FALSE);
				}

				spAction->AddOutputFromPath((wchar_t *)(const wchar_t *)pathName, pEC, VARIANT_FALSE, VARIANT_TRUE,
					bNeedDefault ? VCLINKID_OutputFile : VCLINKID_ImportLibrary, pTool);	// no action (2ndary file)
					// return value from AddOutputFromPath doesn't matter for 2ndary file

				// also generate .exp name with same name as .lib
				pathName.ChangeExtension(L"EXP");
				pathName.GetActualCase(FALSE);

				spAction->AddOutputFromPath((wchar_t *)(const wchar_t*)pathName, pEC, VARIANT_FALSE, VARIANT_TRUE,
					bNeedDefault ? VCLINKID_OutputFile : VCLINKID_ImportLibrary, pTool);	// no action (2ndary file)
				// return value from AddOutputFromPath doesn't matter for 2ndary file
			}

			VARIANT_BOOL bIsDebug = VARIANT_FALSE;
			// name of .pdb file is only an output if /debug
			if (spPropContainer->GetBoolProperty(VCLINKID_GenerateDebugInformation, &bIsDebug) == S_OK && bIsDebug == VARIANT_TRUE)
			{
				CComBSTR bstrPDBName;
				hr = spPropContainer->GetEvaluatedStrProperty(VCLINKID_ProgramDatabaseFile, &bstrPDBName);
				if (hr == S_FALSE)
				{
					CVCLinkerTool::s_optHandler.GetDefaultValue( VCLINKID_ProgramDatabaseFile, &bstrPDBName, spPropContainer );
					spPropContainer->Evaluate(bstrPDBName, &bstrPDBName);
				}
				if (bstrPDBName.Length() != 0)
				{
					strName = bstrPDBName;
					CDirW baseDir;
					CVCToolImpl::CreateDirFromBSTR(bstrProjectDirectory, baseDir);
					if (!pathName.CreateFromDirAndFilename(baseDir, strName))
					{
						CBldAction::InformUserAboutBadFileName(pEC, spPropContainer, pTool, 
							VCLINKID_ProgramDatabaseFile, strName);
						return CVCProjectEngine::DoSetErrorInfo3(VCPROJ_E_BAD_PATH, IDS_ERR_CANNOT_CREATE_PATH, 
							bstrProjectDirectory, strName);
					}

					pathName.GetActualCase(TRUE);
					spAction->AddOutputFromPath((wchar_t *)(const wchar_t *)pathName, pEC, VARIANT_FALSE, VARIANT_TRUE,
						VCLINKID_ProgramDatabaseFile, pTool);	// no action (2ndary file)
					// return value from AddOutputFromPath doesn't matter for 2ndary file
				}
			}

			// name of .map file
			VARIANT_BOOL bPropSet = VARIANT_FALSE;
			if (spPropContainer->GetBoolProperty(VCLINKID_GenerateMapFile, &bPropSet) == S_OK && 
				bPropSet == VARIANT_TRUE)
			{
				BOOL bUseDefault = TRUE;
				CComBSTR bstrMap;
				if (spPropContainer->GetEvaluatedStrProperty(VCLINKID_MapFileName, &bstrMap) == S_OK)
				{
					strName = bstrMap;
					strName.TrimLeft();
					if (!strName.IsEmpty())
					{
						bUseDefault = FALSE;
						CDirW baseDir;
						CVCToolImpl::CreateDirFromBSTR(bstrProjectDirectory, baseDir);
						if (!pathName.CreateFromDirAndFilename(baseDir, strName))
						{
							CBldAction::InformUserAboutBadFileName(pEC, spPropContainer, pTool, 
								VCLINKID_MapFileName, strName);
							return CVCProjectEngine::DoSetErrorInfo3(VCPROJ_E_BAD_PATH, IDS_ERR_CANNOT_CREATE_PATH, 
								bstrProjectDirectory, strName);
						}
						pathName.GetActualCase(TRUE);
					}
				}
				if (bUseDefault)
				{
					pathName.ChangeExtension(L"MAP"); // default
					pathName.GetActualCase(FALSE);
					strName = L"";	// failed to get map name, use default
				}

				spAction->AddOutputFromPath((wchar_t *)(const wchar_t *)pathName, pEC, VARIANT_FALSE, VARIANT_TRUE,
					VCLINKID_MapFileName, pTool);	// no action (2ndary file)
				// return value from AddOutputFromPath doesn't matter for 2ndary file
			}
		}
	}

	return S_OK; // success
}

HRESULT CLinkerLibrarianHelper::DoGetPrimaryOutputFromTool(IUnknown* pItem, CPathW& rpath, BOOL bIsLinker)
{
	CComPtr<VCConfiguration> spProjCfg;
	GetProjectCfgForItem(pItem, &spProjCfg);
	VSASSERT(spProjCfg != NULL, "Linker and librarian only work on project config, not file config");
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
	if (spPropContainer->GetEvaluatedStrProperty(bIsLinker ? VCLINKID_OutputFile : VCLIBID_OutputFile, &bstrName) == S_OK)
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
	CreateOutputName(spPropContainer, &bstrName);
	strName = bstrName;

	if (!rpath.CreateFromDirAndFilename(dirProj, strName))
		return CVCProjectEngine::DoSetErrorInfo3(VCPROJ_E_BAD_PATH, IDS_ERR_CANNOT_CREATE_PATH, strDirName, strName);
	rpath.GetActualCase(TRUE);

	return S_OK;
}

void CLinkerLibrarianHelper::GetProjectCfgForItem(IUnknown* pItem, VCConfiguration** pProjCfg)
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

HRESULT CLinkerLibrarianHelper::DoHasDependencies(IVCBuildAction* pAction, BOOL bIsLinker, VARIANT_BOOL* pbHasDependencies)
{
	CHECK_POINTER_NULL(pbHasDependencies);
	*pbHasDependencies = VARIANT_FALSE;
	RETURN_ON_NULL2(pAction, S_FALSE);

	CComPtr<IVCBuildableItem> spItem;
	pAction->get_Item(&spItem);
	CComQIPtr<IVCPropertyContainer> spPropContainer = spItem;
	if (spPropContainer != NULL)
	{
		UINT idUserDeps = bIsLinker ? VCLINKID_AdditionalDependencies : VCLIBID_AdditionalDependencies;
		CComBSTR bstrLibs;
		HRESULT hr = spPropContainer->GetStrProperty(idUserDeps, &bstrLibs);
		if (hr == S_OK)
		{
			*pbHasDependencies = VARIANT_TRUE;
			return S_OK;
		}
	}

	CComPtr<VCConfiguration> spProjCfg;
	GetProjectCfgForItem(spItem, &spProjCfg);
	CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = spProjCfg;
	RETURN_ON_NULL2(spProjCfgImpl, S_FALSE);

	return spProjCfgImpl->get_HasProjectDependencies(pbHasDependencies);
}

HRESULT CLinkerLibrarianHelper::DoGetAdditionalDependenciesInternal(IVCPropertyContainer* pItem, IVCBuildAction* pAction,
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
	HRESULT hr3 = DoGetDependencies(pAction, &spStrings, &bUpToDate, idProjDeps == VCLINKID_DependentInputs, FALSE /* !dirty */);
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

int CLinkerLibrarianHelper::GetNextDep(int nStartDep, CStringW& strDepList, int nDepLen, CStringW& strDep)
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

HRESULT CLinkerLibrarianHelper::DoGetDependencies(IVCBuildAction* pAction, IVCBuildStringCollection** ppStrings, 
	VARIANT_BOOL* pbUpToDate, BOOL bIsLinker, BOOL bAllowDirty /* = TRUE */)
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
	

	UINT idUserDeps = bIsLinker ? VCLINKID_AdditionalDependencies : VCLIBID_AdditionalDependencies;
	UINT idProjDeps = bIsLinker ? VCLINKID_DependentInputs : VCLIBID_DependentInputs;
	UINT idDefFile = bIsLinker ? VCLINKID_ModuleDefinitionFile : VCLIBID_ModuleDefinitionFile;
	UINT idBuiltFlag = bIsLinker ? VCLINKID_LinkerCommandLineGenerated : VCLIBID_LibCommandLineGenerated;
	CComPtr<IVCPropertyContainer> spPropContainer;
	pAction->get_PropertyContainer(&spPropContainer);
	VSASSERT(spPropContainer, "Hey, how'd we get to GetDependencies with an action with no property container?!?");
	RETURN_ON_NULL(spPropContainer);
	CComBSTR bstrDef;
	if (spPropContainer->GetEvaluatedStrProperty(idDefFile, &bstrDef) == S_OK)
	{
		CStringW strName = bstrDef;
		strName.TrimLeft();
		if (!strName.IsEmpty())
		{
			CPathW pathName;
			if (!CVCToolImpl::CreatePathRelativeToProject(spPropContainer, strName, pathName))
				return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_BAD_PATH, IDS_ERR_CANNOT_CREATE_RELATIVE_PATH, strName);
			if (*ppStrings == NULL)	// not created yet
			{
				HRESULT hr = CVCBuildStringCollection::CreateInstance(ppStrings);
				RETURN_ON_FAIL_OR_NULL2(hr, *ppStrings, E_OUTOFMEMORY);
			}
			(*ppStrings)->Add((wchar_t *)(const wchar_t *)pathName);
			bHasDepend = TRUE;
		}
	}

	// generate dependencies based on linker or librarian additional inputs
	CComBSTR bstrLocalDeps;
	spPropContainer->GetMultiProp(idUserDeps, L" ", VARIANT_FALSE, &bstrLocalDeps);

	CStringW strLocalDeps = bstrLocalDeps;
	strLocalDeps.TrimLeft();
	strLocalDeps.TrimRight();
	if (!strLocalDeps.IsEmpty())
	{
		if (*ppStrings == NULL)	// not created yet
		{
			HRESULT hr = CVCBuildStringCollection::CreateInstance(ppStrings);
			RETURN_ON_FAIL_OR_NULL2(hr, *ppStrings, E_OUTOFMEMORY);
		}

		BldFileRegHandle frh = NULL;
		if (spItem != NULL)
			spItem->get_FileRegHandle((void **)&frh);
		const CPathW* pPath = NULL;
		if (frh)
			pPath = g_FileRegistry.GetRegEntry(frh)->GetFilePath();

		int nIdx = 0;
		int nMax = (int) strLocalDeps.GetLength();
		int	iEntryType = IncTypeCheckLibPath | IncTypeCheckOriginalDir | IncTypeCheckIntDir | IncTypeCheckOutDir;

		CVCStringWList strlstIncs;
		while (nIdx >= 0)
		{
			CStringW strDep;
			nIdx = GetNextDep(nIdx, strLocalDeps, nMax, strDep);
			strDep.TrimLeft();
			if (strDep.IsEmpty())
				continue;

			strDep.TrimRight();
			if (strDep.GetAt(0) == L'"')
				strlstIncs.AddTail(strDep);
			else
				strlstIncs.AddTail( L"\"" + strDep + L"\"" );
		}
		if  (strlstIncs.GetCount() > 0)
		{
			CVCStringWList strlstPaths;
			BOOL bFoundDeps = CVCToolImpl::ResolveIncludeDirectivesToPath(*pPath, pAction, strlstIncs, strlstPaths, FALSE);
			VCPOSITION pos = strlstPaths.GetHeadPosition();
			if (!bFoundDeps)
			{
				pAction->MarkMissingDependencies(TRUE);
				bHasDepend = TRUE;
			}
			else if (pos)
				bHasDepend = TRUE;
			while (pos)
			{
				CStringW strInc = strlstPaths.GetNext(pos);
				CComBSTR bstrInc = strInc;
				(*ppStrings)->Add(bstrInc);
			}
		}

		if (NULL != pbUpToDate)
		{
			*pbUpToDate = FALSE;
		}
	}

	// now that we've got the list of dependencies at the project level, we need to make sure we add them
	// to the list of inputs to the linker or librarian
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

		if (!strProjDeps.IsEmpty())
		{
			CComBSTR bstrProjDeps = strProjDeps;
			CComBSTR bstrOldProjDeps;
			if (spPropContainer->GetStrProperty(idProjDeps, &bstrOldProjDeps) != S_OK || 
				_wcsicmp(bstrProjDeps, bstrOldProjDeps) != 0)
				spPropContainer->SetStrProperty(idProjDeps, bstrProjDeps);
		}
		else
		{
			CComVariant var;
			if (spPropContainer->GetProp(idProjDeps, &var) == S_OK)
				spPropContainer->Clear(idProjDeps);
		}
	}
	else
	{
		CComVariant var;
		if (spPropContainer->GetProp(idProjDeps, &var) == S_OK)
			spPropContainer->Clear(idProjDeps);
	}

	if (bResetDirty)
		spPropContainer->SetBoolProperty(idBuiltFlag, VARIANT_TRUE);	// we've now been through here at least once

	return bHasDepend ? S_OK : S_FALSE;
}
