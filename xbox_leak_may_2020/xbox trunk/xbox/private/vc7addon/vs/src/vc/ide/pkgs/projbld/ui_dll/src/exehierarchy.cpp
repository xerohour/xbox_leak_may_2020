//---------------------------------------------------------------------------
// Microsoft Visual InterDev
//
// Microsoft Confidential
// Copyright 1994 - 1997 Microsoft Corporation. All Rights Reserved.
//
// WebArchy.cpp : Implementation of CIswsApp and DLL registration.
//---------------------------------------------------------------------------

#include "stdafx.h"
#include "ExeHierarchy.h"
#include "..\resdll\gpcmd.h"
#include "VsCoCreate.h"
#include "context.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL g_bCommandLineDBG = FALSE;

HRESULT CExeHierarchy::Initialize( LPCOLESTR pszFilename, LPCOLESTR pszArgs )
{
	wchar_t drive[_MAX_PATH];  // Should these be _MAX_DRIVE, _MAX_DIR and _MAX_FNAME?
	wchar_t path[_MAX_PATH];
	wchar_t name[_MAX_PATH];
	wchar_t ext[_MAX_EXT];

	_wsplitpath( pszFilename, drive, path, name, ext );

	m_bstrFullPath = pszFilename;
	m_bstrName = name;
	m_bstrName.Append(ext);

	m_pParentHierarchy = NULL;
	m_dwParentHierarchyItemID = VSITEMID_NIL;

    // To emulate VC6 behavior, we want to see if this .EXE has an BSC file associated with it.  If it does we want it loaded.
    wchar_t bscpath[_MAX_PATH];
    _wmakepath(bscpath, drive, path, name, L"bsc");

    if (GetFileAttributesW(bscpath) != -1)
    {
        //Try to open the .bsc
        CComPtr<IVsUIShellOpenDocument> spOpenDoc;
        BOOL bIsOpen = FALSE;
        if (SUCCEEDED(ExternalQueryService(SID_SVsUIShellOpenDocument, IID_IVsUIShellOpenDocument,reinterpret_cast<void **>(&spOpenDoc))) &&
            SUCCEEDED(spOpenDoc->IsDocumentOpen(NULL, NULL, bscpath, GUID_NULL, 0, NULL, NULL, NULL, &bIsOpen)))
        {
            // We are not checking the return value because we are not going to do anything with it anyways.
            if (!bIsOpen)
                spOpenDoc->OpenDocumentViaProject(bscpath, LOGVIEWID_Primary, NULL, NULL, NULL, NULL);
        }
    }
	
	return CExeConfig::CreateInstance(&m_pConfig, pszFilename, pszArgs, this);
}

// _Project
STDMETHODIMP CExeHierarchy::get_Name(BSTR FAR* pbstrName)
{
	wchar_t drive[_MAX_PATH];
	wchar_t path[_MAX_PATH];
	wchar_t name[_MAX_PATH];
	wchar_t ext[_MAX_EXT];

	_wsplitpath( m_bstrFullPath, drive, path, name, ext );

	CComBSTR bstrName = name;
	*pbstrName = bstrName.Detach();
	return S_OK;
}

STDMETHODIMP CExeHierarchy::get_FileName( BSTR *pbstrPath )
{
	return get_FullName(pbstrPath);
}

STDMETHODIMP CExeHierarchy::get_IsDirty(VARIANT_BOOL FAR* lpfReturn)
{
	CHECK_POINTER_VALID(lpfReturn)
	*lpfReturn = VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CExeHierarchy::get_DTE(DTE FAR* FAR* lppaReturn)
{
	CHECK_POINTER_VALID(lppaReturn)
	return ExternalQueryService(SID_SDTE, IID__DTE, (void **)lppaReturn);
}

STDMETHODIMP CExeHierarchy::get_Kind(BSTR FAR* lpbstrFileName)
{
	CHECK_POINTER_VALID(lpbstrFileName);

	CComBSTR bstr = L"{F1C25864-3097-11D2-A5C5-00C04F7968B4}"; // should be guid ?
	*lpbstrFileName = bstr.Detach();
	return S_OK;
}

STDMETHODIMP CExeHierarchy::get_FullName(BSTR *pbstrPath)
{
	CHECK_POINTER_VALID(pbstrPath);
	return m_bstrFullPath.CopyTo( pbstrPath );
}

STDMETHODIMP CExeHierarchy::get_Saved(VARIANT_BOOL *lpfReturn)
{
	CHECK_POINTER_VALID(lpfReturn)
	*lpfReturn = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CExeHierarchy::get_ConfigurationManager(ConfigurationManager **ppConfigurationManager)
{
	CHECK_POINTER_VALID(ppConfigurationManager);
	HRESULT hr = S_OK;
	CComPtr<IVsExtensibility> pExtService;
	CComPtr<IVsCfgProvider> pCfgProvider;

	hr = ExternalQueryService(SID_SVsExtensibility, IID_IVsExtensibility, (LPVOID*)&pExtService);
	RETURN_ON_FAIL_OR_NULL(hr, pExtService);

	CComPtr<IVsHierarchy> spHier = VCQI_cast<IVsHierarchy>(this);
	return pExtService->GetConfigMgr(spHier, VSITEMID_ROOT, ppConfigurationManager);
}

STDMETHODIMP CExeHierarchy::get_ParentProjectItem(ProjectItem ** ppProjectItem)
{
	CHECK_POINTER_VALID(ppProjectItem);
	*ppProjectItem = NULL;
	return NOERROR;
}

//  IVsHierarchy
STDMETHODIMP CExeHierarchy::SetSite(IServiceProvider *pSP)
{
	m_pServiceProvider = pSP;
	return S_OK;
}

STDMETHODIMP CExeHierarchy::GetSite(IServiceProvider **ppSP)
{
	CHECK_POINTER_NULL(ppSP);
	return m_pServiceProvider.CopyTo(ppSP);
}

STDMETHODIMP CExeHierarchy::QueryClose(BOOL *pfCanClose)
{
	CHECK_POINTER_NULL(pfCanClose);
	*pfCanClose = TRUE;
	return S_OK;
}

STDMETHODIMP CExeHierarchy::GetGuidProperty(VSITEMID itemid, VSHPROPID propid, GUID *pguid)
{
	CHECK_POINTER_NULL(pguid);
	return E_FAIL;
}

STDMETHODIMP CExeHierarchy::SetGuidProperty( /* [in] */ VSITEMID itemid, /* [in] */ VSHPROPID propid, /* [in] */ REFGUID guid)
{
	return E_FAIL;
}

STDMETHODIMP CExeHierarchy::ParseCanonicalName( /* [in] */ LPCOLESTR pszName, /* [out] */ VSITEMID *pitemid)
{
	CHECK_POINTER_NULL(pitemid);
	return E_NOTIMPL;	// CExeHierarchy::ParseCanonicalName
}

STDMETHODIMP CExeHierarchy::GetCanonicalName( /* [in] */ VSITEMID itemid, /* [out] */ BSTR *ppszName)
{
	CHECK_POINTER_NULL(ppszName);
	if(itemid != VSITEMID_ROOT)
		return E_FAIL;

	return m_bstrFullPath.CopyTo( ppszName );
};

STDMETHODIMP CExeHierarchy::GetProperty(  VSITEMID itemid, /* [in] */ VSHPROPID propid, /* [out] */ VARIANT *pvar)
{
	CHECK_POINTER_NULL(pvar);
	switch(propid)
	{
		case VSHPROPID_Caption:
		case VSHPROPID_Name:
		{	
			V_VT(pvar) = VT_BSTR;
			return m_bstrName.CopyTo( &V_BSTR(pvar) );
		}
		case VSHPROPID_Expandable:		
		case VSHPROPID_ExpandByDefault:
		{
			V_VT(pvar) = VT_BOOL;
			V_BOOL(pvar) = VARIANT_FALSE;
			return S_OK;
		}
		case VSHPROPID_AltHierarchy:
		{
			V_VT(pvar) = VT_I4;
			V_UI4(pvar) = NULL;
			return E_FAIL;
		}
		case VSHPROPID_SortPriority:
		{
			V_VT(pvar) = VT_BOOL;
			V_UI4(pvar) = VARIANT_FALSE;
			return E_FAIL;
		}
		case VSHPROPID_ParentHierarchy:
		{
			pvar->vt = VT_UNKNOWN;
			if( !m_pParentHierarchy )
				return E_FAIL;
			pvar->punkVal = m_pParentHierarchy;
			if (pvar->punkVal)
				pvar->punkVal->AddRef();
			return S_OK;
		}
		case VSHPROPID_ParentHierarchyItemid:
		{
			pvar->vt = VT_INT_PTR;
			if( m_dwParentHierarchyItemID == VSITEMID_NIL )
			{
				V_INT_PTR(pvar) = VSITEMID_ROOT;
				return S_OK;
			}
			V_INT_PTR(pvar) = m_dwParentHierarchyItemID;
			return S_OK;
		}
		case VSHPROPID_Parent:			
		{
			V_VT(pvar) = VT_INT_PTR;
			V_INT_PTR(pvar) = VSITEMID_NIL;
			return S_OK;
		}
		case VSHPROPID_Root:			
		{
			V_VT(pvar) = VT_INT_PTR;			
			V_INT_PTR(pvar) = VSITEMID_ROOT;
			return S_OK;
		}

		case VSHPROPID_BrowseObject:
		case VSHPROPID_ExtObject:
		{
			VariantInit(pvar);
			if(SUCCEEDED(QueryInterface(IID_IDispatch, (void **)&V_DISPATCH(pvar))))
			{
				V_VT(pvar) = VT_DISPATCH;
				return S_OK;
			}
			return E_FAIL;
		}
		case VSHPROPID_ItemDocCookie:
		case VSHPROPID_ImplantHierarchy:
		case VSHPROPID_FirstVisibleChild:
		case VSHPROPID_NextVisibleSibling:
		case VSHPROPID_IsHiddenItem:
		case VSHPROPID_IsNonMemberItem:
		case VSHPROPID_OpenFolderIconIndex:
		case VSHPROPID_SelContainer:
		case VSHPROPID_EditLabel:
			return E_NOTIMPL;	// CExeHierarchy::GetProperty, several properties that don't need to be impl

		case VSHPROPID_UserContext:
			{
				V_VT(pvar) = VT_UNKNOWN;
				IVsUserContext *pUserCtx = NULL;
				CComPtr<IVsMonitorUserContext> pmuc;
				if (SUCCEEDED(ExternalQueryService(SID_SVsMonitorUserContext,
					IID_IVsMonitorUserContext, (void **)&pmuc)))
				{
					pmuc->CreateEmptyContext(&pUserCtx);
					if(pUserCtx)
					{
						pUserCtx->AddAttribute(VSUC_Usage_Filter, L"PRODUCT", L"VC");
						pUserCtx->AddAttribute(VSUC_Usage_Filter, L"ITEM", L"EXE");
						V_UNKNOWN(pvar) = pUserCtx;
						return S_OK;
					}
				}
				return E_FAIL;
			}
		case VSHPROPID_IconImgList:		
			V_VT(pvar) = VT_INT_PTR;
			V_INT_PTR(pvar) = reinterpret_cast<INT_PTR>(GetBuildPkg()->m_hImageList);
			return S_OK;
		
		case VSHPROPID_IconIndex:		
			V_VT(pvar) = VT_I4;
			V_I4(pvar) = BMP_PROJNODE;
			return S_OK;
		
		case VSHPROPID_StateIconIndex:
			V_VT(pvar) = VT_I4;
			V_I4(pvar) = STATEICON_NONE;
			return S_OK;
		
		case VSHPROPID_HandlesOwnReload:
		{
			V_VT(pvar) = VT_BOOL;
			V_BOOL(pvar) = VARIANT_TRUE;
			return S_OK;
		}
			
		default:
			break;
	}
	return E_UNEXPECTED;
}

STDMETHODIMP CExeHierarchy::SetProperty(VSITEMID itemid, VSHPROPID propid, /* [in] */ VARIANT var)
{
	switch( propid )
	{
	case VSHPROPID_ParentHierarchy:
		if( var.vt != VT_UNKNOWN )
			RETURN_INVALID();
		m_pParentHierarchy = var.punkVal;
		return S_OK;

	case VSHPROPID_ParentHierarchyItemid:
		if( var.vt != VT_I4 )
			RETURN_INVALID();
		m_dwParentHierarchyItemID = var.lVal;
		return S_OK;

	default:
		return S_OK;
	}
}


// IVsUIHierarchy
STDMETHODIMP CExeHierarchy::QueryStatusCommand( VSITEMID itemid, const GUID * pguidCmdGroup,
						ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText)
{
	return S_OK;
}

HRESULT CExeHierarchy::ExecCommand(
			VSITEMID itemid, const GUID * pguidCmdGroup, DWORD nCmdID,
			DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut)
{
	if(*pguidCmdGroup == GUID_VsUIHierarchyWindowCmds)
	{
		HRESULT hr;
		switch(nCmdID)
		{
			case UIHWCMDID_RightClick:
			{
				CComPtr<IOleComponentUIManager> pOleComponentUImanager;
				hr = ExternalQueryService(SID_OleComponentUIManager, __uuidof(IOleComponentUIManager), (void**)&pOleComponentUImanager);
				if (SUCCEEDED(hr) && pOleComponentUImanager)
				{
					CComQIPtr<IOleInPlaceComponentUIManager> pComponentUImanager = pOleComponentUImanager;
					if (pComponentUImanager)
						pComponentUImanager->UpdateUI(0, FALSE, 0);

					POINTS pnts;
					ULONG ulPts = V_UI4(pvaIn);
					memcpy((void*)&pnts, &ulPts, sizeof(POINTS));
					CComPtr<IOleCommandTarget> spTarget = VCQI_cast<IOleCommandTarget>(this);
					hr = pOleComponentUImanager->ShowContextMenu(
						0, 
						guidVCGrpId, 
						IDMX_VC_EXEPROJECT,
						pnts,
						spTarget);
				}

				break;
			}
//			case UIHWCMDID_DoubleClick:
//			case UIHWCMDID_EnterKey:
			default:
				hr = OLECMDERR_E_NOTSUPPORTED;
				break;
		}
		return hr;
	}

	return OLECMDERR_E_UNKNOWNGROUP;
}

STDMETHODIMP CExeHierarchy::AdviseHierarchyEvents( /* [in] */ IVsHierarchyEvents *pEventSink, /* [out] */ VSCOOKIE *pdwCookie)
{
	CHECK_POINTER_NULL(pdwCookie);
	*pdwCookie = 1;
	return S_OK;
};

// IVsHierarchyDeleteHandler
STDMETHODIMP CExeHierarchy::QueryDeleteItem( /* [in] */ VSDELETEITEMOPERATION dwDelItemOp, /* [in] */ VSITEMID itemid, /* [retval][out] */ BOOL __RPC_FAR *pfCanDelete)
{
	CHECK_POINTER_NULL(pfCanDelete);

	if (dwDelItemOp != DELITEMOP_RemoveFromProject)	// support remove only, not delete
	{
		*pfCanDelete = FALSE;
		return S_OK;
	}

	// yes, we can be deleted
	*pfCanDelete = TRUE;
	return S_OK;
}
	
STDMETHODIMP CExeHierarchy::DeleteItem( /* [in] */ VSDELETEITEMOPERATION dwDelItemOp, /* [in] */ VSITEMID itemid)
{
	if (dwDelItemOp != DELITEMOP_RemoveFromProject)	// support remove only, not delete
		return E_UNEXPECTED;

	// cause the solution to shut down the project
	CComPtr<IVsSolution> pSolution;

	HRESULT hr = GetBuildPkg()->GetIVsSolution( &pSolution );
	VSASSERT( SUCCEEDED( hr ), "QueryService for solution failed!  Note that you can't do a QueryService for that in a secondary thread..." );
	RETURN_ON_FAIL(hr);
	
	// remove project from solution
	CComPtr<IVsHierarchy> spHier = VCQI_cast<IVsHierarchy>(this);
	return pSolution->CloseSolutionElement( SLNSAVEOPT_PromptSave, spHier, 0 );
}

// IOleCommandTarget
STDMETHODIMP CExeHierarchy::QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText)
{
	return OLECMDERR_E_UNKNOWNGROUP;
}

STDMETHODIMP CExeHierarchy::Exec(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt,
            VARIANT *pvaIn, VARIANT *pvaOut)
{
	return OLECMDERR_E_UNKNOWNGROUP;
}
			

		   
// IVsCfgProvider
STDMETHODIMP CExeHierarchy::GetCfgs(ULONG celt, IVsCfg *rgpcfg[], ULONG *pcActual, VSCFGFLAGS *prgfFlags)
{
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_CFG_ZOMBIE);

	if (celt == 0)
	{
		CHECK_POINTER_NULL(pcActual);
		*pcActual = 1;
		return S_OK;
	}

	CHECK_POINTER_NULL(rgpcfg);

	if (pcActual)
		*pcActual = 1;

	return m_pConfig->QueryInterface(IID_IVsCfg, (void**)rgpcfg);
}

// IVsProjectCfgProvider
STDMETHODIMP CExeHierarchy::get_UsesIndependentConfigurations(BOOL *pfUsesIndependentConfigurations )
{
	CHECK_POINTER_NULL(pfUsesIndependentConfigurations);
	*pfUsesIndependentConfigurations = FALSE;
	return S_FALSE;
};



HRESULT CExeConfig::CreateInstance(IVsDebuggableProjectCfg ** ppCfg, LPCOLESTR pszFilename, LPCOLESTR pszArgs, 
	CExeHierarchy* pArchy)
{
	CComObject<CExeConfig> *pCfg = NULL;  
	HRESULT hr = CComObject<CExeConfig>::CreateInstance(&pCfg);
	if (SUCCEEDED(hr))
	{
		pCfg->Initialize( pszFilename, pszArgs, pArchy );
		hr = pCfg->QueryInterface(__uuidof(IVsDebuggableProjectCfg), (void**)ppCfg );
	}
	return hr;
}

HRESULT CExeConfig::Initialize( LPCOLESTR pszFilename, LPCOLESTR pszArgs, CExeHierarchy* pArchy )
{
	CComPtr<VCDebugSettings> pDebug;
	HRESULT hr = VsLoaderCoCreateInstance<VCDebugSettings>(CLSID__VCDebugSettings, &pDebug);
//	HRESULT hr = pDebug.CoCreateInstance(CLSID_VCDebugSettings);
	if(SUCCEEDED(hr) && pDebug)
	{
		m_pDbgSettings = pDebug;

		wchar_t drive[_MAX_PATH];
		wchar_t path[_MAX_PATH];
		wchar_t name[_MAX_PATH];
		wchar_t ext[_MAX_EXT];

		_wsplitpath( pszFilename, drive, path, name, ext );

		CComBSTR bstrFullPath = pszFilename;
		CComBSTR bstrName = name;
		bstrName.Append(ext);
		
		CStringW strDir;
		if( g_bCommandLineDBG == TRUE )
		{
			GetCurrentDirectoryW(MAX_PATH, strDir.GetBuffer(MAX_PATH));
			strDir.ReleaseBuffer();
			g_bCommandLineDBG = FALSE;
		}
		else
		{
			strDir = drive;
			strDir += path;
		}

		CComBSTR bstrDir = strDir;
		CComBSTR bstrArgs = pszArgs;
		pDebug->put_Command(bstrFullPath);
		pDebug->put_WorkingDirectory(bstrDir);
		pDebug->put_CommandArguments(bstrArgs);

		m_pArchy = pArchy;
	}
	return hr;
}

	
// IVsCfg
STDMETHODIMP CExeConfig::get_DisplayName(BSTR *pbstrDisplayName)
{
	CHECK_POINTER_NULL(pbstrDisplayName);

	CComBSTR bstrCfg = L"Debug";
	return bstrCfg.CopyTo(pbstrDisplayName);
}
	
STDMETHODIMP CExeConfig::get_IsDebugOnly(BOOL *pfIsDebugOnly )
{
	CHECK_POINTER_NULL(pfIsDebugOnly);

	*pfIsDebugOnly = TRUE;
	return S_OK;
};

STDMETHODIMP CExeConfig::get_IsReleaseOnly(BOOL *pfIsRetailOnly)
{
	CHECK_POINTER_NULL(pfIsRetailOnly);

	*pfIsRetailOnly = FALSE;
	return S_FALSE;
};

// IVsProjectCfg
STDMETHODIMP CExeConfig::get_ProjectCfgProvider(/* [out] */ IVsProjectCfgProvider **ppIVsProjectCfgProvider)
{
	CHECK_POINTER_NULL(ppIVsProjectCfgProvider);
	*ppIVsProjectCfgProvider = NULL;

	CHECK_POINTER_NULL(m_pArchy);

	return m_pArchy->QueryInterface(IID_IVsProjectCfgProvider, (void**)ppIVsProjectCfgProvider );
}

STDMETHODIMP CExeConfig::get_CanonicalName(/* [out] */ BSTR *pbstrCanonicalName)
{
	CHECK_POINTER_NULL(pbstrCanonicalName);

	CComBSTR bstrCfg = L"Debug";
	return bstrCfg.CopyTo(pbstrCanonicalName);
}
	
STDMETHODIMP CExeConfig::get_IsRetailOnly(BOOL *pfIsRetailOnly)
{
	CHECK_POINTER_NULL(pfIsRetailOnly);

	*pfIsRetailOnly = FALSE;
	return S_FALSE;
};

STDMETHODIMP CExeConfig::get_IsPackaged(BOOL *pfIsPackaged)
{
	CHECK_POINTER_NULL(pfIsPackaged);

	*pfIsPackaged = FALSE;
	return S_FALSE;
};

STDMETHODIMP CExeConfig::get_IsSpecifyingOutputSupported(BOOL *pfIsSpecifyingOutputSupported)
{
	CHECK_POINTER_NULL(pfIsSpecifyingOutputSupported);

	*pfIsSpecifyingOutputSupported = FALSE;
	return S_FALSE;
};

STDMETHODIMP CExeConfig::get_TargetCodePage( /* [out] */ UINT *puiTargetCodePage)
{
	CHECK_POINTER_NULL(puiTargetCodePage);

	*puiTargetCodePage = 1200;
	return S_OK;
}

// IVsDebuggableProjectCfg
STDMETHODIMP CExeConfig::DebugLaunch(/* [in] */ VSDBGLAUNCHFLAGS grfLaunch)
{
	CComPtr<IVsDebugger> pVsDebugger;
	HRESULT hr = ExternalQueryService(IID_IVsDebugger, IID_IVsDebugger, (void **)&pVsDebugger);
	if (SUCCEEDED(hr) && pVsDebugger)
	{
		VSASSERT(hr==S_OK, "Must have gotten some form of warning/success!");

		VsDebugTargetInfo  dbgi[2];
		ZeroMemory( dbgi, 2 * sizeof(VsDebugTargetInfo) );
		DWORD dwNumTargets = 2;

		CHECK_ZOMBIE(m_pDbgSettings, IDS_ERR_CFG_ZOMBIE);
		hr = m_pDbgSettings->CanGetDebugTargetInfo(NULL);
		if (hr != S_OK)
		{
			CComQIPtr<VCDebugSettings> pDbgSettingsPublic = m_pDbgSettings;
			hr = GetBuildPkg()->GetDebugCommandLines(pDbgSettingsPublic, NULL);
		}
		if (hr == S_OK)
			hr = m_pDbgSettings->GetDebugTargetInfo( grfLaunch, dbgi, &dwNumTargets );
		if (hr==S_OK)
		{
			hr = pVsDebugger->LaunchDebugTargets(dwNumTargets, dbgi);

			// free up the structures
			for (DWORD i = 0; i < dwNumTargets; i++)
			{
				SysFreeString(dbgi[i].bstrRemoteMachine);
				SysFreeString(dbgi[i].bstrMdmRegisteredName);
				SysFreeString(dbgi[i].bstrExe);
				SysFreeString(dbgi[i].bstrArg);
				SysFreeString(dbgi[i].bstrCurDir);
				SysFreeString(dbgi[i].bstrEnv);
				CoTaskMemFree(dbgi[i].pClsidList);
			}
		}

		if (FAILED(hr))
		{
			// do something reasonable here.
		}
	}

	return hr;
}

STDMETHODIMP CExeConfig::QueryDebugLaunch(/* [in] */ VSDBGLAUNCHFLAGS grfLaunch, /* [out] */ BOOL *pfCanLaunch)
{
	CHECK_POINTER_NULL(pfCanLaunch);

	*pfCanLaunch = TRUE;
	return S_OK;
}

// ISpecifyPropertyPages
STDMETHODIMP CExeConfig::GetPages(/* [out] */ CAUUID *pPages)
{
	CHECK_POINTER_NULL(pPages);

	// number of tool pages plus number of 'extra' pages
	pPages->pElems = (GUID*) CoTaskMemAlloc(sizeof(CLSID));
	RETURN_ON_NULL2(pPages->pElems, E_OUTOFMEMORY);

	pPages->pElems[0] = __uuidof(DebugSettingsPage);
	pPages->cElems = 1;
	return S_OK;
}

// IVcCfg

STDMETHODIMP CExeConfig::get_Object( IDispatch **ppConfig)
{
	CHECK_POINTER_NULL(ppConfig);
	return m_pDbgSettings->QueryInterface(IID_IDispatch, (void **) ppConfig);
}

//

STDMETHODIMP CExeConfig::ReadUserOptions(IStream *pStream, LPCOLESTR pszKey)
{
	CHECK_READ_POINTER_NULL(pStream);

	CComBSTR	bstrName;
	HRESULT 	hr;
	
	//Read in the settings header
	hr = bstrName.ReadFromStream(pStream);
	RETURN_ON_FAIL(hr);

	//Check to see if this is the debug settings
	if (bstrName == L"DebugSettings")
	{
		//Load settings
		CHECK_ZOMBIE(m_pDbgSettings, IDS_ERR_CFG_ZOMBIE);
		hr = m_pDbgSettings->ReadFromStream(pStream);
		RETURN_ON_FAIL(hr);
	}

	return hr;
}

STDMETHODIMP CExeConfig::WriteUserOptions(IStream *pStream, LPCOLESTR pszKey)
{
	CHECK_READ_POINTER_NULL(pStream);
	CHECK_ZOMBIE(m_pDbgSettings, IDS_ERR_CFG_ZOMBIE);

	//Write out the debug settings header
	CComBSTR 	bstrName = L"DebugSettings";
	HRESULT 	hr;

	hr = bstrName.WriteToStream(pStream);
	RETURN_ON_FAIL(hr);

	//Save to stream
	hr = m_pDbgSettings->WriteToStream(pStream);
	RETURN_ON_FAIL(hr);

	return hr;
}


// IPersist
STDMETHODIMP CExeHierarchy::GetClassID( CLSID *pClassID)
{
	CHECK_POINTER_NULL(pClassID);
	*pClassID = IID_IExeHierarchy;
	return S_OK;
}

// IPersistFileFormat
STDMETHODIMP CExeHierarchy::IsDirty(BOOL *pfIsDirty)
{
	CHECK_POINTER_NULL(pfIsDirty);

	*pfIsDirty = FALSE;
	return S_FALSE;
};


STDMETHODIMP CExeHierarchy::ReadUserOptions(IStream *pStream, LPCOLESTR pszKey)
{
	CHECK_READ_POINTER_NULL(pStream);
	CComQIPtr<IVsPersistSolutionOpts> pOpts = m_pConfig;
	CHECK_ZOMBIE(pOpts, IDS_ERR_CFG_ZOMBIE);
	return pOpts->ReadUserOptions(pStream, pszKey);
}

STDMETHODIMP CExeHierarchy::WriteUserOptions(IStream *pStream, LPCOLESTR pszKey)
{
	CHECK_READ_POINTER_NULL(pStream);
	CComQIPtr<IVsPersistSolutionOpts> pOpts = m_pConfig;
	CHECK_ZOMBIE(pOpts, IDS_ERR_CFG_ZOMBIE);
	return pOpts->WriteUserOptions(pStream, pszKey);
}

STDMETHODIMP CExeHierarchy::GetCurFile( /* [out] */ LPOLESTR __RPC_FAR *ppszFilename, /* [out] */ DWORD __RPC_FAR *pnFormatIndex)
{
	CHECK_POINTER_NULL(ppszFilename);

	DWORD dwLen = (DWORD)(ocslen(m_bstrFullPath) + sizeof(OLECHAR));

	*ppszFilename = (LPOLESTR)::CoTaskMemAlloc(dwLen * sizeof(OLECHAR));
	if (NULL == *ppszFilename)
		return E_OUTOFMEMORY;

	ocscpy(*ppszFilename, m_bstrFullPath);
	return S_OK;
};

// IVsPerPropertyBrowsing
STDMETHODIMP CExeHierarchy::HideProperty(DISPID dispid, BOOL *pfHide)
{
	*pfHide = FALSE;
	switch( dispid )
	{
	case DISPID_VALUE:
	case 209:
		*pfHide = FALSE;
		break;
	default:
		*pfHide = TRUE;
		break;
	}
	return S_OK;
}

// IVsPerPropertyBrowsing
STDMETHODIMP CExeHierarchy::IsPropertyReadOnly( DISPID dispid, BOOL *fReadOnly)
{ 
	*fReadOnly = TRUE;
	return S_OK; 
}

STDMETHODIMP CExeHierarchy::GetLocalizedPropertyInfo(DISPID dispid, LCID localeID, BSTR *pbstrName, BSTR *pbstrDesc)
{
	CHECK_POINTER_NULL(pbstrName);
	*pbstrName = NULL;
	CHECK_POINTER_NULL(pbstrDesc);
	*pbstrDesc = NULL;

	CComBSTR bstrName;
	CComBSTR bstrDesc;

	switch( dispid )
	{
	case DISPID_VALUE:
	{
		if (!bstrName.LoadString(IDS_EXE_PROJ_NAME_LBL))
			bstrName = ( L"Name" );
		*pbstrName = bstrName.Detach();

		if (!bstrDesc.LoadString(IDS_EXE_PROJ_NAME_DESC))
			bstrDesc = ( L"The Name of the exe to debug" );

		break;
	}
	case 209:
	{
		if (!bstrName.LoadString(IDS_EXE_PROJ_PATH_LBL))
			bstrName = ( L"Full Path" );

		if (!bstrDesc.LoadString(IDS_EXE_PROJ_PATH_DESC))
			bstrDesc = ( L"The Full Path to the exe to debug" );

		break;
	}
	default:
		break;
	}

	if (bstrName)
		*pbstrName = bstrName.Detach();
	if (bstrDesc)
		*pbstrDesc = bstrDesc.Detach();
	
	return S_OK;
}

STDMETHODIMP CExeHierarchy::GetClassName(BSTR* pbstrClassName)
{
	CHECK_POINTER_NULL(pbstrClassName);
	CComBSTR bstrClassName;
	bstrClassName.LoadString(IDS_PROJ_PROPERTIES);
	*pbstrClassName = bstrClassName.Detach();
	if (*pbstrClassName)
		return S_OK;
	else
		return E_OUTOFMEMORY;
}

// IPersist
STDMETHODIMP CCrashDumpHierarchy::GetClassID( CLSID *pClassID)
{
	CHECK_POINTER_NULL(pClassID);
	*pClassID = IID_ICrashDumpHierarchy;
	return S_OK;
}

