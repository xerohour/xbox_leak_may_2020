//---------------------------------------------------------------------------
// Microsoft Visual C++
//
// Microsoft Confidential
// Copyright 1994 - 2001 Microsoft Corporation. All Rights Reserved.
//
// StyleHierarchy.cpp : Implementation of CIswsApp and DLL registration.
//---------------------------------------------------------------------------

//#include "stdafx.h"
#include "StyleHierarchy.h"
//#include "..\resdll\gpcmd.h"
//#include "VsCoCreate.h"
//#include "context.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

// static
HRESULT CStyleSheetHierarchy::CreateInstance(CStyleSheetHierarchy** ppStyle, LPCOLESTR szFileName)
{
	CHECK_POINTER_NULL(ppStyle);
	*ppStyle = NULL;

	CComObject<CStyleSheetHierarchy> *pStyleObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CStyleSheetHierarchy>::CreateInstance(&pStyleObj);
	RETURN_ON_FAIL_OR_NULL(hr, pStyleObj);
	hr = pStyleObj->Initialize();
	RETURN_ON_FAIL(hr);
	((IVsHierarchy *)pStyleObj)->AddRef();
	
	*ppStyle = pStyleObj;
	return hr;
}

HRESULT CStyleSheetHierarchy::Initialize()
{
	return CStyleSheetConfig::CreateInstance(&m_pConfig, this);
}

//  IVsHierarchy
STDMETHODIMP CStyleSheetHierarchy::SetSite(IServiceProvider *pSP)
{
	m_pServiceProvider = pSP;
	return S_OK;
}

STDMETHODIMP CStyleSheetHierarchy::GetSite(IServiceProvider **ppSP)
{
	CHECK_POINTER_NULL(ppSP);
	return m_pServiceProvider.CopyTo(ppSP);
}

STDMETHODIMP CStyleSheetHierarchy::QueryClose(BOOL *pfCanClose)
{
	CHECK_POINTER_NULL(pfCanClose);
	*pfCanClose = TRUE;
	return S_OK;
}

STDMETHODIMP CStyleSheetHierarchy::GetGuidProperty(VSITEMID itemid, VSHPROPID propid, GUID *pguid)
{
	CHECK_POINTER_NULL(pguid);
	return E_FAIL;
}

STDMETHODIMP CStyleSheetHierarchy::SetGuidProperty( /* [in] */ VSITEMID itemid, /* [in] */ VSHPROPID propid, /* [in] */ REFGUID guid)
{
	return E_FAIL;
}

STDMETHODIMP CStyleSheetHierarchy::ParseCanonicalName( /* [in] */ LPCOLESTR pszName, /* [out] */ VSITEMID *pitemid)
{
	CHECK_POINTER_NULL(pitemid);
	return E_NOTIMPL;	// CStyleSheetHierarchy::ParseCanonicalName
}

STDMETHODIMP CStyleSheetHierarchy::GetCanonicalName( /* [in] */ VSITEMID itemid, /* [out] */ BSTR *ppszName)
{
	CHECK_POINTER_NULL(ppszName);
	if(itemid != VSITEMID_ROOT)
		return E_FAIL;

	CComPtr<VCStyleSheet> spStyleSheet;
	GetVCStyleSheet(&spStyleSheet);
	CHECK_ZOMBIE(spStyleSheet, IDS_ERR_STYLESHEET_ZOMBIE);

	return spStyleSheet->get_StyleSheetFile(ppszName);
};

STDMETHODIMP CStyleSheetHierarchy::GetProperty(  VSITEMID itemid, /* [in] */ VSHPROPID propid, /* [out] */ VARIANT *pvar)
{
	CHECK_POINTER_NULL(pvar);
	switch(propid)
	{
		case VSHPROPID_Caption:
		case VSHPROPID_Name:
		{	
			V_VT(pvar) = VT_BSTR;
			CComBSTR bstrName;
			HRESULT hr = get_Name(&bstrName);
			RETURN_ON_FAIL(hr);
			return bstrName.CopyTo( &V_BSTR(pvar) );
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
		case VSHPROPID_ShowProjInSolutionPage:
		{
			V_VT(pvar) = VT_BOOL;
			V_BOOL(pvar) = VARIANT_TRUE;
			return S_OK;
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
			return E_NOTIMPL;	// CStyleSheetHierarchy::GetProperty, several properties that don't need to be impl

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
						pUserCtx->AddAttribute(VSUC_Usage_Filter, L"ITEM", L"STYLESHEET");
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

STDMETHODIMP CStyleSheetHierarchy::SetProperty(VSITEMID itemid, VSHPROPID propid, /* [in] */ VARIANT var)
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
STDMETHODIMP CStyleSheetHierarchy::QueryStatusCommand( VSITEMID itemid, const GUID * pguidCmdGroup,
						ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText)
{
	return S_OK;
}

HRESULT CStyleSheetHierarchy::ExecCommand(
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
			case UIHWCMDID_DoubleClick:
				return ShowSettings();

//			case UIHWCMDID_EnterKey:
			default:
				hr = OLECMDERR_E_NOTSUPPORTED;
				break;
		}
		return hr;
	}

	return OLECMDERR_E_UNKNOWNGROUP;
}


HRESULT CStyleSheetHierarchy::ShowSettings()
{
	// for this one command, set the focus to Project Explorer before letting shell actually do what
	// it wants to do
	CComPtr<IOleCommandTarget> srpCmdTgt;
	if (SUCCEEDED(ExternalQueryService( SID_SUIHostCommandDispatcher, IID_IOleCommandTarget, 
		(void**)&srpCmdTgt)) && srpCmdTgt != NULL)
	{
		srpCmdTgt->Exec(&CLSID_StandardCommandSet97, cmdidProjectExplorer, OLECMDEXECOPT_DODEFAULT, 
			NULL, NULL);
	}

	CComPtr<IVsPropertyPageFrame> pPPFrame;
	HRESULT hr = ExternalQueryService(SID_SVsPropertyPageFrame, IID_IVsPropertyPageFrame, (void**)&pPPFrame);
	RETURN_ON_FAIL_OR_NULL(hr, pPPFrame);

	hr = pPPFrame->ShowFrame(CLSID_VCProject);
	if (FAILED(hr))
	{
		pPPFrame->ReportError(hr);
		pPPFrame->HideFrame();
	}

	return hr;
}


STDMETHODIMP CStyleSheetHierarchy::AdviseHierarchyEvents( /* [in] */ IVsHierarchyEvents *pEventSink, /* [out] */ VSCOOKIE *pdwCookie)
{
	CHECK_POINTER_NULL(pdwCookie);
	*pdwCookie = 1;
	return S_OK;
};

// IOleCommandTarget
STDMETHODIMP CStyleSheetHierarchy::QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText)
{
	return OLECMDERR_E_UNKNOWNGROUP;
}

STDMETHODIMP CStyleSheetHierarchy::Exec(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt,
            VARIANT *pvaIn, VARIANT *pvaOut)
{
	return OLECMDERR_E_UNKNOWNGROUP;
}
			
// IPersist
STDMETHODIMP CStyleSheetHierarchy::GetClassID( CLSID *pClassID)
{
	CHECK_POINTER_NULL(pClassID);
	*pClassID = IID_IStyleSheetHierarchy;
	return S_OK;
}

// IPersistFileFormat
STDMETHODIMP CStyleSheetHierarchy::IsDirty(BOOL *pfIsDirty)
{
	CHECK_POINTER_NULL(pfIsDirty);

	*pfIsDirty = FALSE;
	return S_FALSE;
};


STDMETHODIMP CStyleSheetHierarchy::ReadUserOptions(IStream *pStream, LPCOLESTR pszKey)
{
	return S_OK;
}

STDMETHODIMP CStyleSheetHierarchy::WriteUserOptions(IStream *pStream, LPCOLESTR pszKey)
{
	return S_OK;
}

STDMETHODIMP CStyleSheetHierarchy::GetCurFile( /* [out] */ LPOLESTR __RPC_FAR *ppszFilename, /* [out] */ DWORD __RPC_FAR *pnFormatIndex)
{
	CHECK_POINTER_NULL(ppszFilename);

	CComBSTR bstrName;
	HRESULT hr = GetCanonicalName(VSITEMID_ROOT, &bstrName);
	RETURN_ON_FAIL_OR_NULL(hr, bstrName);
	DWORD dwLen = (DWORD)(ocslen(bstrName) + sizeof(OLECHAR));

	*ppszFilename = (LPOLESTR)::CoTaskMemAlloc(dwLen * sizeof(OLECHAR));
	if (NULL == *ppszFilename)
		return E_OUTOFMEMORY;

	ocscpy(*ppszFilename, bstrName);
	return S_OK;
};

STDMETHODIMP CStyleSheetHierarchy::get_Name(BSTR FAR* pbstrName)
{
	CComPtr<VCStyleSheet> spStyleSheet;
	GetVCStyleSheet(&spStyleSheet);
	CHECK_ZOMBIE(spStyleSheet, IDS_ERR_STYLESHEET_ZOMBIE);

	return spStyleSheet->get_Name(pbstrName);
}

STDMETHODIMP CStyleSheetHierarchy::put_Name(BSTR bstrName)
{
	CComPtr<VCStyleSheet> spStyleSheet;
	GetVCStyleSheet(&spStyleSheet);
	CHECK_ZOMBIE(spStyleSheet, IDS_ERR_STYLESHEET_ZOMBIE);

	return spStyleSheet->put_Name(bstrName);
}

STDMETHODIMP CStyleSheetHierarchy::Close()
{
	m_pServiceProvider.Release();
	if (m_dispkeyItem)
		m_dispkeyItem.Release();

	return S_OK;
}

// IVsCfgProvider
STDMETHODIMP CStyleSheetHierarchy::GetCfgs(ULONG celt, IVsCfg *rgpcfg[], ULONG *pcActual, VSCFGFLAGS *prgfFlags)
{
	CHECK_ZOMBIE(m_pConfig, IDS_ERR_STYLESHEET_ZOMBIE);

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
STDMETHODIMP CStyleSheetHierarchy::get_UsesIndependentConfigurations(BOOL *pfUsesIndependentConfigurations )
{
	CHECK_POINTER_NULL(pfUsesIndependentConfigurations);
	*pfUsesIndependentConfigurations = FALSE;
	return S_FALSE;
};

HRESULT CStyleSheetHierarchy::GetVCStyleSheet(VCStyleSheet** ppStyle)
{
	CHECK_POINTER_NULL(ppStyle);
	CComQIPtr<VCStyleSheet> spStyleSheet = m_dispkeyItem;
	*ppStyle = spStyleSheet.Detach();
	CHECK_ZOMBIE(*ppStyle, IDS_ERR_STYLESHEET_ZOMBIE);
	return S_OK;
}

HRESULT CStyleSheetHierarchy::GetVCDispStyleSheet(IDispatch** ppStyle)
{
	RETURN_ON_NULL(ppStyle);
	CComQIPtr<IDispatch> spDispStyleSheet = m_dispkeyItem;
	*ppStyle = spDispStyleSheet.Detach();
	CHECK_ZOMBIE(*ppStyle, IDS_ERR_STYLESHEET_ZOMBIE);
	return S_OK;
}

HRESULT CStyleSheetConfig::CreateInstance(IVsProjectCfg ** ppCfg, CStyleSheetHierarchy* pArchy)
{
	CComObject<CStyleSheetConfig> *pCfg = NULL;  
	HRESULT hr = CComObject<CStyleSheetConfig>::CreateInstance(&pCfg);
	if (SUCCEEDED(hr))
	{
		pCfg->Initialize( pArchy );
		hr = pCfg->QueryInterface(__uuidof(IVsProjectCfg), (void**)ppCfg );
	}
	return hr;
}

HRESULT CStyleSheetConfig::Initialize( CStyleSheetHierarchy* pArchy )
{
	m_pArchy = pArchy;
	return S_OK;
}

	
// IVsCfg
STDMETHODIMP CStyleSheetConfig::get_DisplayName(BSTR *pbstrDisplayName)
{
	CHECK_ZOMBIE(m_pArchy, IDS_ERR_STYLESHEET_ZOMBIE);
	return m_pArchy->get_Name(pbstrDisplayName);
}
	
STDMETHODIMP CStyleSheetConfig::get_IsDebugOnly(BOOL *pfIsDebugOnly )
{
	CHECK_POINTER_NULL(pfIsDebugOnly);

	*pfIsDebugOnly = FALSE;
	return S_OK;
};

STDMETHODIMP CStyleSheetConfig::get_IsReleaseOnly(BOOL *pfIsRetailOnly)
{
	CHECK_POINTER_NULL(pfIsRetailOnly);

	*pfIsRetailOnly = FALSE;
	return S_FALSE;
};

// IVsProjectCfg
STDMETHODIMP CStyleSheetConfig::get_ProjectCfgProvider(/* [out] */ IVsProjectCfgProvider **ppIVsProjectCfgProvider)
{
	CHECK_POINTER_NULL(ppIVsProjectCfgProvider);
	*ppIVsProjectCfgProvider = NULL;

	RETURN_ON_NULL(m_pArchy);

	return m_pArchy->QueryInterface(IID_IVsProjectCfgProvider, (void**)ppIVsProjectCfgProvider );
}

STDMETHODIMP CStyleSheetConfig::get_CanonicalName(/* [out] */ BSTR *pbstrCanonicalName)
{
	CHECK_ZOMBIE(m_pArchy, IDS_ERR_STYLESHEET_ZOMBIE);
	return m_pArchy->GetCanonicalName(VSITEMID_ROOT, pbstrCanonicalName);
}
	
STDMETHODIMP CStyleSheetConfig::get_IsRetailOnly(BOOL *pfIsRetailOnly)
{
	CHECK_POINTER_NULL(pfIsRetailOnly);

	*pfIsRetailOnly = FALSE;
	return S_FALSE;
};

STDMETHODIMP CStyleSheetConfig::get_IsPackaged(BOOL *pfIsPackaged)
{
	CHECK_POINTER_NULL(pfIsPackaged);

	*pfIsPackaged = FALSE;
	return S_FALSE;
};

STDMETHODIMP CStyleSheetConfig::get_IsSpecifyingOutputSupported(BOOL *pfIsSpecifyingOutputSupported)
{
	CHECK_POINTER_NULL(pfIsSpecifyingOutputSupported);

	*pfIsSpecifyingOutputSupported = FALSE;
	return S_FALSE;
};

STDMETHODIMP CStyleSheetConfig::get_TargetCodePage( /* [out] */ UINT *puiTargetCodePage)
{
	CHECK_POINTER_NULL(puiTargetCodePage);

	*puiTargetCodePage = 1200;
	return S_OK;
}

// ISpecifyPropertyPages
STDMETHODIMP CStyleSheetConfig::GetPages(/* [out] */ CAUUID *pPages)
{
	CHECK_POINTER_NULL(pPages);
	CHECK_ZOMBIE(m_pArchy, IDS_ERR_STYLESHEET_ZOMBIE);

	CComPtr<IDispatch> spDispStyleSheet;
	m_pArchy->GetVCDispStyleSheet(&spDispStyleSheet);
	CComQIPtr<ISpecifyPropertyPages> pSPP = spDispStyleSheet;
	CHECK_ZOMBIE(pSPP, IDS_ERR_STYLESHEET_ZOMBIE);
	return pSPP->GetPages( pPages );
}

//---------------------------------------------------------------------------
// interface: IVCCfg
//---------------------------------------------------------------------------
STDMETHODIMP CStyleSheetConfig::get_Tool(/* [in] */ VARIANT toolIndex, /* [out] */ IUnknown **ppTool)
{
	CHECK_POINTER_NULL(ppTool);
	CHECK_ZOMBIE(m_pArchy, IDS_ERR_STYLESHEET_ZOMBIE);

	CComPtr<VCStyleSheet> spStyleSheet;
	m_pArchy->GetVCStyleSheet(&spStyleSheet);
	CHECK_ZOMBIE(spStyleSheet, IDS_ERR_STYLESHEET_ZOMBIE);
	
	CComQIPtr<IVCCollection> spCollection;	//Collection SP
	CComPtr<IDispatch> pDisp;

	HRESULT hr = spStyleSheet->get_Tools(&pDisp);	//Get the tools collection
	spCollection = pDisp;
	
	if (SUCCEEDED(hr) && spCollection)
	{
		CComPtr<IDispatch> spDisp;
		hr = spCollection->Item(toolIndex, &spDisp);	//Get the tool item
		RETURN_ON_FAIL_OR_NULL(hr, spDisp);
		hr = spDisp->QueryInterface(IID_IUnknown, (void **) ppTool);	//Get the tool unk
	}
	
	return hr;
}

STDMETHODIMP CStyleSheetConfig::get_Object( IDispatch **ppStyle)
{
	CHECK_POINTER_NULL(ppStyle);
	CHECK_ZOMBIE(m_pArchy, IDS_ERR_STYLESHEET_ZOMBIE);

	CComPtr<IDispatch> spDispStyle;
	m_pArchy->GetVCDispStyleSheet(&spDispStyle);
	CHECK_ZOMBIE(spDispStyle, IDS_ERR_STYLESHEET_ZOMBIE);
	*ppStyle = spDispStyle.Detach();
	return S_OK;
}

//

STDMETHODIMP CStyleSheetConfig::ReadUserOptions(IStream *pStream, LPCOLESTR pszKey)
{
	return S_OK;
}

STDMETHODIMP CStyleSheetConfig::WriteUserOptions(IStream *pStream, LPCOLESTR pszKey)
{
	return S_OK;
}

