// VCNode.cpp: implementation of the CVCNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BldPkg.h"
#include "VCNode.h"   // coming from HierLib
#include "VCArchy.h"
#include "..\resdll\gpcmd.h"
#include "oleipc.h"
#include <vcguid.h>
#include "context.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVCNode::CVCNode( void ) :
	m_pNodeParent( NULL ),
	m_pNodeNext( NULL ),
	m_fDragDropMoved( FALSE )
{
}

HRESULT CVCNode::GetVCProject(VCProject** ppProject) 
{
	CHECK_POINTER_NULL(ppProject);
	*ppProject = NULL;

	VSASSERT(g_pBuildPackage->m_pProjectEngine, "Project engine not initialized!");
	if (g_pBuildPackage->m_pProjectEngine)
	{
		CHECK_ZOMBIE(GetRootNode(), IDS_ERR_PROJ_ZOMBIE);
		CComPtr<IDispatch> pDispPropertyContainer = ((CVCProjectNode*)GetRootNode())->m_dispkeyItem;
		if (pDispPropertyContainer)
		{
			CComQIPtr<VCProject> pProj = pDispPropertyContainer;
			*ppProject = pProj.Detach();
		}
	}
	return S_OK; 
}

HRESULT CVCNode::GetDispVCProject(IDispatch** ppProject) 
{
	CHECK_POINTER_NULL(ppProject);
	*ppProject = NULL;

	VSASSERT(g_pBuildPackage->m_pProjectEngine, "Project engine not initialized!");
	if (g_pBuildPackage->m_pProjectEngine)
	{
		CHECK_ZOMBIE(GetRootNode(), IDS_ERR_PROJ_ZOMBIE);
		CComPtr<IDispatch> pDispPropertyContainer = ((CVCProjectNode*)GetRootNode())->m_dispkeyItem;
		*ppProject = pDispPropertyContainer.Detach();
	}
	return S_OK; 
}

HRESULT CVCNode::GetVCItem(IDispatch** ppItem)
{
	CHECK_POINTER_NULL(ppItem);
	*ppItem = m_dispkeyItem;
	if (*ppItem)
		(*ppItem)->AddRef();
	return S_OK;
}

HRESULT CVCNode::CanDirtyProject( void )
{
	CHECK_ZOMBIE(GetRootNode(), IDS_ERR_PROJ_ZOMBIE);
	// see if we are allowed to touch the project
	CComPtr<IDispatch> pProj;
	((CVCProjectNode*)GetRootNode())->GetDispVCProject(&pProj);
	CComQIPtr<IVCProjectImpl> pProjImpl = pProj;
	RETURN_ON_NULL2(pProjImpl, E_NOINTERFACE);
	VARIANT_BOOL bCanDirty = VARIANT_TRUE;
	if( pProjImpl->get_CanDirty( &bCanDirty ) != S_OK || bCanDirty == VARIANT_FALSE)
		return E_ACCESSDENIED;
	return S_OK;
}

// automation extender helper functions
HRESULT CVCNode::GetExtender( LPOLESTR wszGuid, BSTR bstrExtenderName, IDispatch **ppExtender )
{
	CHECK_POINTER_NULL(ppExtender);
	*ppExtender = NULL;

	CComBSTR bstrCATID( wszGuid );
	if( !bstrCATID )
		return S_OK;

	// validate params
	if( !bstrExtenderName )
		return S_FALSE;

	// get internal ExtensionManager svc
	CComPtr<ObjectExtenders> spExtMgr;
	HRESULT hr = ExternalQueryService( SID_SExtensionManager, IID_ObjectExtenders, (void **)&spExtMgr );
	RETURN_ON_FAIL_OR_NULL(hr, spExtMgr);

	// call GetExtension on svc
	return spExtMgr->GetExtender( bstrCATID, bstrExtenderName, (LPDISPATCH)this, ppExtender );
}

HRESULT CVCNode::GetExtenderNames( LPOLESTR wszGuid, VARIANT *pvarExtenderNames )
{
	// validate params
	CHECK_POINTER_NULL( pvarExtenderNames );

	CComBSTR bstrCATID( wszGuid );
	if( !bstrCATID )
		return S_OK;

	// get internal ExtensionManager svc
	CComPtr<ObjectExtenders> spExtMgr;
	HRESULT hr = ExternalQueryService( SID_SExtensionManager, IID_ObjectExtenders, (void **)&spExtMgr );
	RETURN_ON_FAIL_OR_NULL(hr, spExtMgr);

	// call GetExtensionNames on svc
	return spExtMgr->GetExtenderNames( bstrCATID, (LPDISPATCH) this, pvarExtenderNames );
}

IDispatch * CVCNode::GetPropertyContainer(void)
{
	CComPtr<IDispatch> pDispPropertyContainer;

	VSASSERT(g_pBuildPackage->m_pProjectEngine, "Project engine not initialized!");
	if (g_pBuildPackage->m_pProjectEngine)
	{
		pDispPropertyContainer = m_dispkeyItem;
	}
	return pDispPropertyContainer; 
}

DWORD CVCNode::TranslateEnableState(enumEnableStates eState)
{
	switch (eState)
	{
	case eEnableNotSupported:
		return 0;
	case eEnableInvisible:
		return OLECMDF_INVISIBLE;
	case eEnableLatchedAndEnabled:
		return OLECMDF_ENABLED|OLECMDF_LATCHED; // OLECMDF_NINCHED
	case eEnableSupportedAndEnabled:
		return OLECMDF_SUPPORTED|OLECMDF_ENABLED;
	case eEnableEnabled:
		return OLECMDF_ENABLED;
	case eEnableLatched:
		return OLECMDF_LATCHED;
	case eEnableSupported:
		return OLECMDF_SUPPORTED;
	case eEnableSupportedAndInvisible:
		return OLECMDF_SUPPORTED | OLECMDF_INVISIBLE;
	default:
		VSASSERT(FALSE, "Unhandled state for TranslateEnableState");
		return OLECMDF_SUPPORTED;
	}
}

//-----------------------------------------------------------------------------
// Called to execute our commands
//-----------------------------------------------------------------------------
STDMETHODIMP CVCNode::Exec(
		const GUID *pguidCmdGroup,
		DWORD nCmdID,
		DWORD nCmdexecopt,
		VARIANT *pvaIn,
		VARIANT *pvaOut)
{
	RETURN_ON_NULL2(pguidCmdGroup, OLECMDERR_E_UNKNOWNGROUP);
	
	if(*pguidCmdGroup == guidVSStd2K)
	{
		switch(nCmdID)
		{
			// WARNING: When adding commands to this list, they must be implemented
			//          in either VCProjectNode, or VCBsFolderNode, or we well recurse
			//          infinitely and crash !
			case ECMD_ADDRESOURCE:
			case ECMD_ADDWEBREFERENCE:
			{
				CVCProjectNode* pProjNode = GetVCProjectNode();
				RETURN_ON_NULL2(pProjNode, OLECMDERR_E_NOTSUPPORTED);
				return pProjNode->Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut);
			}
			default:
				return OLECMDERR_E_NOTSUPPORTED;
		}
	}
	else if(*pguidCmdGroup == guidVSStd97)
	{	
		switch(nCmdID)
		{
			// WARNING: When adding commands to this list, they must be implemented
			//          in either VCProjectNode, or VCBsFolderNode, or we well recurse
			//          infinitely and crash !
			case cmdidAddNewItem:
			case cmdidAddExistingItem:
			case cmdidNewFolder:
			case cmdidAddClass:
			{
				CVCProjectNode* pProjNode = GetVCProjectNode();
				RETURN_ON_NULL2(pProjNode, OLECMDERR_E_NOTSUPPORTED);
				return pProjNode->Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut);
			}
			default:
					return OLECMDERR_E_NOTSUPPORTED;
		}
	}
 	return OLECMDERR_E_NOTSUPPORTED;
}


//-----------------------------------------------------------------------------
//	Called to update the status for each command
//-----------------------------------------------------------------------------
STDMETHODIMP CVCNode::QueryStatus(
		const GUID *pguidCmdGroup,
		ULONG cCmds,
		OLECMD prgCmds[],
		OLECMDTEXT *pCmdText)
{
	RETURN_ON_NULL2(pguidCmdGroup, OLECMDERR_E_UNKNOWNGROUP);
	
	enumKnownGuidList eGuid = eGuidUninitialized;
	if (*pguidCmdGroup == guidVSStd2K)
		eGuid = eGuidVSStd2K;
	else if (*pguidCmdGroup == guidVSStd97)
		eGuid = eGuidVSStd97;	// Common VB commands
	else
		return OLECMDERR_E_UNKNOWNGROUP;

	HRESULT hr = NOERROR;
	for (ULONG i = 0; i < cCmds && SUCCEEDED(hr); i++)
		hr = QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, i);

	if (FAILED(hr))
	{
		VSASSERT(cCmds == 1, "We're not set up right to handle a failed command if more than one present");
	}

	return hr;
}

HRESULT CVCNode::QueryStatusAtIndex(enumKnownGuidList eGuid, const GUID* pguidCmdGroup, OLECMD prgCmds[],
	OLECMDTEXT *pCmdText, ULONG nIndex)
{
	if (eGuid == eGuidUninitialized)
	{
		if (*pguidCmdGroup == guidVSStd2K)
			eGuid = eGuidVSStd2K;
		else if (*pguidCmdGroup == guidVSStd97)
			eGuid = eGuidVSStd97;
		else
			return OLECMDERR_E_UNKNOWNGROUP;
	}

	enumEnableStates fEnable = eEnableSupported;
	HRESULT hr = NOERROR;
	switch (eGuid)
	{
		case eGuidVSStd2K:
		{
			switch(prgCmds[nIndex].cmdID)
			{
				case ECMD_ADDRESOURCE:
				case ECMD_PROJSETTINGS:
				case ECMD_ADDWEBREFERENCE:
#ifdef AARDVARK
				case ECMD_SHOWALLFILES:
				case ECMD_COPYPROJECT:
				case ECMD_SLNREFRESH:
#endif
					fEnable = eEnableSupportedAndEnabled; //GetProjectNode()->IsSourceControlled() && !IsInSourceControl();
					break;
				default:
					fEnable = eEnableNotSupported;
					break;
			}
			break;
		}
		case eGuidVSStd97:
		{
			switch(prgCmds[nIndex].cmdID)
			{
				case cmdidAddClass:
				case cmdidAddExistingItem:
				case cmdidAddNewItem:
				case cmdidNewFolder:
				case cmdidPropertyPages:
				case cmdidShowProperties:
					fEnable = eEnableSupportedAndEnabled;
					break;
				default:
					return OLECMDERR_E_NOTSUPPORTED;
			}
			break;
		}
		default:
			break;
	}

	prgCmds[nIndex].cmdf = TranslateEnableState(fEnable);
	if (prgCmds[nIndex].cmdf == 0)
		return OLECMDERR_E_NOTSUPPORTED;

	return hr;
}

//---------------------------------------------------------------------------
//	Shows context-sensitive menu
//---------------------------------------------------------------------------
HRESULT CVCNode::ShowContextMenu(UINT imxMenuID, VARIANT *pvaIn, GUID guidMenuGroup, IOleCommandTarget *pOleCommandTarget /*=NULL*/)
{
	CComPtr<IOleComponentUIManager> srpOleComponentUImanager;
	HRESULT hr = ExternalQueryService(SID_OleComponentUIManager, __uuidof(IOleComponentUIManager), (void**)&srpOleComponentUImanager);
	RETURN_ON_FAIL_OR_NULL(hr, srpOleComponentUImanager);

    CComQIPtr<IOleInPlaceComponentUIManager> srpComponentUImanager = srpOleComponentUImanager;
	if (srpComponentUImanager)
		srpComponentUImanager->UpdateUI(0, FALSE, 0);

	if (!pOleCommandTarget)
	{
		CVCArchy *pVsHierarchy = GetHierarchy();
		VSASSERT(pVsHierarchy, "Unable to get the hierarchy!");

		pOleCommandTarget = VCQI_cast<IOleCommandTarget>(pVsHierarchy);
	}

	// POINT  pnt;
	// GetCursorPos(&pnt);
	// POINTS pnts; = { (SHORT)pnt.x, (SHORT)pnt.y };
	POINTS pnts;
	ULONG ulPts = V_UI4(pvaIn);
    memcpy((void*)&pnts, &ulPts, sizeof(POINTS));
	
	return srpOleComponentUImanager->ShowContextMenu(0, guidMenuGroup, imxMenuID, pnts, pOleCommandTarget);
}

HRESULT CVCNode::GetGuidProperty( VSHPROPID propid, GUID *pguid)
{
	return E_NOTIMPL;	// CVCNode::GetGuidProperty
}

HRESULT CVCNode::SetGuidProperty( VSHPROPID propid, GUID guid)
{
	return E_NOTIMPL;	// CVCNode::SetGuidProperty
}

HRESULT CVCNode::SetProperty(VSHPROPID propid, const VARIANT& var)
{
	return E_NOTIMPL;	// CVCNode::SetProperty
}

//---------------------------------------------------------------------------
//	Returns the requested property in the given variant.
//---------------------------------------------------------------------------
HRESULT CVCNode::GetProperty(VSHPROPID propid, VARIANT *pvar)
{
	HRESULT hr = E_FAIL;
	CComVariant varRes;
	switch( propid )
	{
		case VSHPROPID_ProjectDir:
		{
			CComBSTR bstrProjDir;
			CComPtr<VCProject> pProject;
			GetVCProject(&pProject);
			if (pProject)
			{
				hr = pProject->get_ProjectDirectory( &bstrProjDir );
				varRes = bstrProjDir;
			}
			break;
		}
		case VSHPROPID_ExtObject:
		{
			hr = GetExtObject(varRes);
			break;
		}
		case VSHPROPID_TypeName:
		{
			CString strTitle;
	
			strTitle.LoadString(g_hInstLocale, IDS_PROJECTS_TITLE);
			varRes = strTitle;
			hr = S_OK;
			break;
		}
		case VSHPROPID_ReloadableProjectFile:
		{
			varRes.ChangeType(VT_BOOL);
			varRes = VARIANT_FALSE;
			hr = S_OK;
			break;
		}
		case VSHPROPID_IsNonLocalStorage:
		{
			varRes.ChangeType(VT_BOOL);
			varRes = VARIANT_FALSE;
			hr = S_OK;
			break;
		}
		case VSHPROPID_ConfigurationProvider:
		{
			varRes.ChangeType(VT_UNKNOWN);
			varRes.punkVal = NULL;
			CVCArchy *pVsHierarchy = (CVCArchy *)GetHierarchy();
			IUnknown *pUnk = NULL;
			if (pVsHierarchy)
			{
				hr = pVsHierarchy->QueryInterface(IID_IUnknown, (void **)&pUnk);
				varRes = pUnk;
				pUnk->Release();
			}
			break;
		}
		case VSHPROPID_Expandable:
		{
			V_VT(pvar) = VT_I4;
			V_I4(pvar) = FALSE;
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
		case VSHPROPID_ParentHierarchy :
		{
			V_VT(pvar) = VT_UNKNOWN;
			V_UNKNOWN(pvar) = NULL;
			return S_OK;
		}
		case VSHPROPID_ParentHierarchyItemid:
		{
			V_VT(pvar) = VT_INT_PTR;
			V_INT_PTR(pvar) = VSITEMID_NIL;
			return S_OK;
		}
		case VSHPROPID_BrowseObject :
		{
			VariantInit(pvar);
			if(SUCCEEDED(QueryInterface(IID_IDispatch, (void **)&V_DISPATCH(pvar))))
			{
				V_VT(pvar) = VT_DISPATCH;
			}
			return S_OK;
		}
		case VSHPROPID_ItemDocCookie:
		case VSHPROPID_ImplantHierarchy:
		case VSHPROPID_FirstVisibleChild:
		case VSHPROPID_NextVisibleSibling:
		case VSHPROPID_IsHiddenItem:
		case VSHPROPID_IsNonMemberItem:
        case VSHPROPID_OverlayIconIndex:
			return E_NOTIMPL;	// CVCNode::GetProperty, properties we specifically don't support

		case VCPROPID_ProjBldActiveFileConfig:
		case VCPROPID_ProjBldIncludePath:
			return E_NOTIMPL;	// CVCNode::GetProperty, properties we specifically don't support
	
		case VCPROPID_ProjBldObject:
			if (SUCCEEDED(GetVCItem(&V_DISPATCH(pvar))))
			{
				V_VT(pvar) = VT_DISPATCH;
				return S_OK;
			}
			break;

		case VCPROPID_ProjBldProject:
			if (SUCCEEDED(GetDispVCProject(&V_DISPATCH(pvar))))
			{
				V_VT(pvar) = VT_DISPATCH;
				return S_OK;
			}
			break;

		case VCPROPID_ProjBldActiveProjConfig:
			hr = GetActiveVCProjectConfig(&V_DISPATCH(pvar));
			if (SUCCEEDED(hr))
			{
				V_VT(pvar) = VT_DISPATCH;
				return S_OK;
			}
			break;

		default:
			hr = E_FAIL;
			break;
	}
	varRes.Detach(pvar);
	return hr;
}

HRESULT CVCNode::GetActiveVCProjectConfig(IDispatch** ppProjCfg)
{
	CHECK_POINTER_NULL(ppProjCfg);

	CVCArchy* pArchy = GetHierarchy();
	RETURN_ON_NULL(pArchy);

	CVCProjectNode* pProjNode = pArchy->GetProjectNode();
	RETURN_ON_NULL(pProjNode);

	CComPtr<IVCGenericConfiguration> spGenCfg;
	HRESULT hr = pProjNode->GetActiveConfig(&spGenCfg);
	CComQIPtr<IVCCfg> spCfg = spGenCfg;
	RETURN_ON_FAIL_OR_NULL(hr, spCfg);

	return spCfg->get_Object(ppProjCfg);
}

void CVCNode::SetCaption(BSTR bstrNewCaption)
{	
	// inform the ui
	if (GetHierarchy())
		GetHierarchy()->OnPropertyChanged(this, /*VSHPROPID_NIL*/ VSHPROPID_Caption, 0);	// NOT an interface call...
}

HRESULT CVCNode::ShowSettings(REFGUID guidProps)
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

	hr = pPPFrame->ShowFrame(guidProps);
	if (FAILED(hr))
	{
		pPPFrame->ReportError(hr);
		pPPFrame->HideFrame();
	}

	return hr;
}

CVCArchy* CVCNode::GetHierarchy(void) const
{
	// if parent is NULL, then this should be in CHierRoot
	// or this is a serious programming mistake
	VSASSERT(NULL != GetParent(), "No parent?  Deep trouble somewhere.");

	CVCNode *pNode = GetParent();
	RETURN_ON_NULL2(pNode, NULL);

	while (NULL != pNode->GetParent())
		pNode = pNode->GetParent();
	return pNode->GetHierarchy();
}

//---------------------------------------------------------------------------
//	Iteratively get the root node of the tree
//---------------------------------------------------------------------------
CVCNode* CVCNode::GetRootNode(void) const
{
	CVCNode *pNode = const_cast<CVCNode *>(this);	// safe
	for (; pNode->GetParent() != NULL; pNode = pNode->GetParent());
	return pNode;
}

//---------------------------------------------------------------------------
// return the VSITEMID value for this node
// a cast of the this pointer to the VSITEMID (dword) type
// exception will be for root nodes which should return VSITEMID_ROOT
//---------------------------------------------------------------------------
VSITEMID CVCNode::GetVsItemID(void) const
{
	return reinterpret_cast<VSITEMID>(this);
}

//-----------------------------------------------------------------------------
// class CVCWaitCursor
//-----------------------------------------------------------------------------
HCURSOR CVCWaitCursor::m_hWaitCur = ::LoadCursor(NULL, IDC_WAIT);
