// VCFGrp.cpp: implementation of the CVCFileGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VCFGrp.h"
#include "BldPkg.h"
#include "..\resdll\gpcmd.h"
#include "PrjNode.h"
#include "utils.h"
#include "vcfile.h"
#include <vcguid.h>
#include "vcarchy.h"
#include "context.h"
#include "AutoFilter.h"
#include "AutoFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// static member initialization

const LPOLESTR CVCFileGroup::s_wszCATID = L"{EE8299CA-19B6-4f20-ABEA-E1FD9A33B683}";

CVCFileGroup::CVCFileGroup() :
#ifdef AARDVARK
	m_eMagic( magicNotSet ),
	m_bstrDirectory( L"" ),
#endif // AARDVARK
	m_fDeleted( FALSE )
{
}

CVCFileGroup::~CVCFileGroup()
{
}

CVCProjectNode* CVCFileGroup::GetVCProjectNode()
{
	return (CVCProjectNode*)GetRootNode();
}

UINT CVCFileGroup::GetIconIndex(ICON_TYPE iconType)
{
#ifdef AARDVARK
	UINT index = BMP_FOLDER;

	if( iconType == ICON_StateImage )
		return STATEICON_NONE;

	// if the magic enum wasn't set, set it
	if( m_eMagic == magicNotSet )
	{
		CComBSTR bstrName;
		GetName( &bstrName );
		// references?
		if( wcsncmp( bstrName, L"References", wcslen( L"References" ) ) == 0 )
			m_eMagic = magicReferenceFolder;
		// satellite dlls?
		else if( wcsncmp( bstrName, L"Satellite DLLs", wcslen( L"Satellite DLLs" ) ) == 0 )
			m_eMagic = magicSatelliteFolder;
		// normal folder
		else
			m_eMagic = magicNone;
	}
	switch( m_eMagic )
	{
	case magicReferenceFolder:
		if( iconType == ICON_Open )
			index = BMP_OPENREFERENCEFOLDER;
		else
			index = BMP_REFERENCEFOLDER;
		break;
		break;
	case magicSatelliteFolder:
		if( iconType == ICON_Open )
			index = BMP_OPENSATELLITEFOLDER;
		else
			index = BMP_SATELLITEFOLDER;
		break;
	default:
		if( iconType == ICON_Open )
			index = BMP_OPENFOLDER;
		else
			index = BMP_FOLDER;
	}
	return index;
#else
	switch( iconType )
	{
	case ICON_StateImage:
		return STATEICON_NONE;
	case ICON_Closed:
		return BMP_FOLDER;
	case ICON_Open:
		return BMP_OPENFOLDER;
	default:
		return BMP_FOLDER;
	}
#endif // AARDVARK
}

HRESULT CVCFileGroup::CreateInstance(CVCFileGroup **ppProjectNode, IDispatch* pItem)
{
	CComObject<CVCFileGroup> *pPrjObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CVCFileGroup>::CreateInstance(&pPrjObj);
	RETURN_ON_FAIL_OR_NULL2(hr, pPrjObj, E_OUTOFMEMORY);

	pPrjObj->AddRef();
	pPrjObj->Initialize(pItem);
	*ppProjectNode = pPrjObj;

	return hr;
}


//-----------------------------------------------------------------------------
// Called by CreateInstance to initialize us.
//-----------------------------------------------------------------------------
void CVCFileGroup::Initialize(IDispatch* pItem)
{	
	// Only once
	m_dispkeyItem = pItem;
}


//-----------------------------------------------------------------------------
// Reads the project file (if we havent' already done so), and loads the project 
// into memory.
//-----------------------------------------------------------------------------
BOOL CVCFileGroup::Open()
{
	BOOL bRetVal = TRUE;
// open the project file. For now, we pretend we have one.
	return bRetVal;
}

HRESULT CVCFileGroup::DisplayContextMenu( VARIANT *pvaIn )
{
	return ShowContextMenu(IDM_VS_CTXT_FOLDERNODE, pvaIn, guidSHLMainMenu, static_cast<IOleCommandTarget *>(this));
}

//-----------------------------------------------------------------------------
// Command status updater
//-----------------------------------------------------------------------------
STDMETHODIMP CVCFileGroup::QueryStatus(
		const GUID *pguidCmdGroup,
		ULONG cCmds,
		OLECMD prgCmds[],
		OLECMDTEXT *pCmdText)
{
	RETURN_ON_NULL2(pguidCmdGroup, OLECMDERR_E_UNKNOWNGROUP);

	CVCNode::enumKnownGuidList eGuid;
	if (*pguidCmdGroup == guidVSStd97)
		eGuid = CVCNode::eGuidVSStd97;
	else if (*pguidCmdGroup == guidVSStd2K)
		eGuid = CVCNode::eGuidVSStd2K;
	else	// let the base class deal with it
		return CVCBaseFolderNode::QueryStatus(pguidCmdGroup, cCmds, prgCmds, pCmdText);

	HRESULT hr = S_OK;
	for (ULONG i = 0; i < cCmds && SUCCEEDED(hr); i++)
		hr = QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, i);

	return hr;
}

HRESULT CVCFileGroup::QueryStatusAtIndex(CVCNode::enumKnownGuidList eGuid, const GUID* pguidCmdGroup, OLECMD prgCmds[],
	OLECMDTEXT *pCmdText, ULONG nIndex)
{
	if (eGuid == eGuidUninitialized)
	{
		if (*pguidCmdGroup == guidVSStd2K)
			eGuid = CVCNode::eGuidVSStd2K;
		else if (*pguidCmdGroup == guidVSStd97)
			eGuid = CVCNode::eGuidVSStd97;
		else
			return CVCBaseFolderNode::QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, nIndex);
	}

	enumEnableStates fEnable = eEnableSupported;
	if (eGuid == CVCNode::eGuidVSStd97)
	{
		switch( prgCmds[nIndex].cmdID )
		{
		case cmdidPropertyPages:
			fEnable = eEnableSupported;
			break;
		case cmdidPaste:
			fEnable = CanPaste() ? eEnableSupportedAndEnabled : eEnableSupported;
			break;
		case cmdidCut:
			fEnable = eEnableSupportedAndEnabled;
			break;
		case cmdidCopy:
			fEnable = eEnableSupportedAndEnabled;
			break;
		default:
			return CVCBaseFolderNode::QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, nIndex); 
		}
	}
	else
		return CVCBaseFolderNode::QueryStatusAtIndex(eGuid, pguidCmdGroup, prgCmds, pCmdText, nIndex);

	prgCmds[nIndex].cmdf = TranslateEnableState(fEnable);
	return NOERROR;
}

//-----------------------------------------------------------------------------
// Command Executor
//-----------------------------------------------------------------------------
STDMETHODIMP CVCFileGroup::Exec(
		const GUID *pguidCmdGroup,
		DWORD nCmdID,
		DWORD nCmdexecopt,
		VARIANT *pvaIn,
		VARIANT *pvaOut)
{
	RETURN_ON_NULL2(pguidCmdGroup, OLECMDERR_E_UNKNOWNGROUP);
	
	if(*pguidCmdGroup == guidVSStd2K)
	{
		switch( nCmdID )
		{
		case ECMD_PROJSETTINGS:
			return ShowSettings(CLSID_FolderNodeProps);
		default:
			return CVCBaseFolderNode::Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut); //OLECMDERR_E_NOTSUPPORTED;
		}
	}
	else if(*pguidCmdGroup == guidVSStd97)
	{
		return CVCBaseFolderNode::Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut); //OLECMDERR_E_NOTSUPPORTED;
	}

	// Unknown command set
	return OLECMDERR_E_NOTSUPPORTED;
}

HRESULT CVCFileGroup::CleanUpBeforeDelete()
{
	CComPtr<IDispatch> pParentDisp;
	CComPtr<IDispatch> pItemDisp;
	GetObjectsForDelete( this, &pParentDisp, &pItemDisp);
	return CleanUpObjectModel(pParentDisp, pItemDisp);
}

HRESULT CVCFileGroup::GetObjectsForDelete(CVCFileGroup *pFilter, IDispatch** ppParentDisp, IDispatch** ppItemDisp)
{
	CHECK_POINTER_NULL(ppParentDisp);
	CHECK_POINTER_NULL(ppItemDisp);

	CComPtr<IDispatch> spParentDisp = ((CVCNode*)pFilter->GetParent())->m_dispkeyItem;
	*ppParentDisp = spParentDisp.Detach();

	CComPtr<IDispatch> spItemDisp = pFilter->m_dispkeyItem;
	*ppItemDisp = spItemDisp.Detach();

	return S_OK;
}

HRESULT CVCFileGroup::CleanUpObjectModel(IDispatch* pParentDisp, IDispatch* pItemDisp)
{
	// Project
	CComQIPtr<VCProject> pProject = pParentDisp;
	if(pProject)
		return pProject->RemoveFilter(pItemDisp);

	// Filter
	CComQIPtr<VCFilter> pFilter = pParentDisp;
	if( pFilter )
		return pFilter->RemoveFilter(pItemDisp);

	return E_FAIL;
}

HRESULT CVCFileGroup::CleanUpUI( CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen /*= TRUE*/ )
{
	if (pRoot == NULL)
		pRoot = this;

	CVCNode* pNext, *pNextT;
	CVCBaseFolderNode *pParent;

	pParent = static_cast<CVCBaseFolderNode*>(pRoot->GetParent());

	pNext = static_cast<CVCBaseFolderNode*>(pRoot)->GetHead();

	while (pNext)
	{
		if( pNext->GetKindOf() == Type_CVCFileGroup || pNext->GetKindOf() == Type_CVCProjectNode )
		{
			pNextT = pNext->GetNext();
			CleanUpUI(static_cast<CVCNode*>(pNext), dwDelItemOp, bCloseIfOpen);
			pNext = pNextT;
		}
		else
		{
			// delete files
			pNextT = pNext->GetNext();
			((CVCNode*)pNext)->CleanUpUI(static_cast<CVCNode*>(this), dwDelItemOp, bCloseIfOpen);
			pNext = pNextT;
		}
	}	
	((CVCFileGroup *)pRoot)->m_fDeleted = TRUE;
	HRESULT hr = pParent->Delete((CVCFileGroup *)pRoot, GetHierarchy());
	return S_OK;
}

HRESULT CVCFileGroup::OnDelete(CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen /* = TRUE */)
{
	if( pRoot == NULL )
		pRoot = this;

	// get the objects before we wipe out the links to them
	CComPtr<IDispatch> spParentDisp;
	CComPtr<IDispatch> spItemDisp;
	GetObjectsForDelete( (CVCFileGroup*)pRoot, &spParentDisp, &spItemDisp);

	CleanUpObjectModel(spParentDisp, spItemDisp);
	return S_OK;
}

HRESULT CVCFileGroup::GetGuidProperty( VSHPROPID propid, GUID *pguid)
{
	CHECK_POINTER_NULL(pguid);
	*pguid = GUID_ItemType_VirtualFolder;
	return S_OK;
}

//---------------------------------------------------------------------------
//	Sets the given property to the value specified in the variant
//---------------------------------------------------------------------------
HRESULT CVCFileGroup::SetProperty(VSHPROPID propid, const VARIANT& var)
{
	CComVariant vt = VARIANT_TRUE;

	if( propid == VSHPROPID_EditLabel )
	{
		// check and make sure the name is actually different before we try to
		// set it...
		CComBSTR bstrOldName;
		CComPtr<VCFilter> spFilter;
		GetVCFilter(&spFilter);
		RETURN_ON_NULL2(spFilter, E_NOINTERFACE);
		spFilter->get_Name( &bstrOldName );
		if( bstrOldName != var.bstrVal )
		{
			CComPtr<IDispatch> spParent;
			HRESULT hr = spFilter->get_Parent(&spParent);
			RETURN_ON_FAIL_OR_NULL(hr, spParent);
			CComQIPtr<VCProject> spProjectParent = spParent;
			CComQIPtr<VCFilter> spFilterParent = spParent;
			CComQIPtr<IVCProjectImpl> spProjectParentImpl = spProjectParent;
			CComQIPtr<IVCFilterImpl> spFilterParentImpl = spFilterParent;
			if (spProjectParentImpl)
				hr = spProjectParentImpl->CanAddFilter2(var.bstrVal, VARIANT_FALSE /* no errorinfo */);
			else if (spFilterParentImpl)
				spFilterParentImpl->CanAddFilter2(var.bstrVal, VARIANT_FALSE /* no errorinfo */);
			else
				hr = E_FAIL;
			if (FAILED(hr))
			{
				if (hr != E_FAIL)
				{
					if (CVCProjectNode::VsHandlesError(hr))
						return hr;	// let VS handle the error message

					long nErrID = 0;
					CStringW strParam, strItemName;
					strItemName.Format(IDS_FILTER, var.bstrVal);
					int cParams = 1;
					switch (hr)
					{
					case E_INVALIDARG:
						nErrID = IDS_E_INVALID_NAME;
						strParam = strItemName;
						break;

					case VCPROJ_E_ZOMBIE:
						nErrID = IDS_E_ZOMBIE;
						cParams = 0;
						break;

					case E_ACCESSDENIED:
						nErrID = IDS_E_READ_ONLY_PROJ_FILE;
						CVCProjectNode::GetProjectNameString(spParent, strParam);
						break;

					case VCPROJ_E_FILTER_EXISTS:
						nErrID = IDS_E_FILTER_ALREADY_EXISTS;
						strParam = var.bstrVal;
						break;
					default:
						return hr;
					}
					CStringW strErr;
					if (nErrID)
					{
						if (cParams == 0)
							strErr.LoadString(nErrID);
						else if (cParams == 1)
							strErr.Format(nErrID, strParam);
						else
						{
							VSFAIL("Wrong number of parameters for error formatting!");
							return E_FAIL;
						}
						CComBSTR bstrErr = strErr;
						ReportError(bstrErr);
						return S_FALSE;
					}
				}
				return E_FAIL;
			}
			put_Name( var.bstrVal );
		}
		return S_OK;
	}
	// handle project node properties here...
	else 
		return CVCBaseFolderNode::SetProperty(propid, var);

	return E_UNEXPECTED;
}

//---------------------------------------------------------------------------
//	Returns the requested property in the given variant.
//---------------------------------------------------------------------------
HRESULT CVCFileGroup::GetProperty(VSHPROPID propid, VARIANT *pvar)
{
	CHECK_POINTER_NULL(pvar);

	// handle file node properties here...
	HRESULT hr = E_FAIL;
	CComVariant varRes;
	switch( propid )
	{
	case VSHPROPID_Name:
	case VSHPROPID_SaveName:
	case VSHPROPID_Caption:
	case VSHPROPID_EditLabel:
	{	// return this node's displayed caption
		V_VT(pvar) = VT_BSTR;
		CComBSTR bstrName;
		CComPtr<VCFilter> spFilter;
		GetVCFilter(&spFilter);
		RETURN_ON_NULL2(spFilter, E_NOINTERFACE);
		spFilter->get_Name( &bstrName );
		pvar->bstrVal = bstrName.Detach();
		return S_OK;
	}
	case VSHPROPID_UserContext:
	{
		varRes.ChangeType(VT_UNKNOWN);
		varRes.punkVal = NULL;

		CComPtr<IVsUserContext> pUserCtx;
		CComPtr<IVsMonitorUserContext> pmuc;
		if (SUCCEEDED(ExternalQueryService(SID_SVsMonitorUserContext, IID_IVsMonitorUserContext, (void **)&pmuc)) && pmuc)
		{
			pmuc->CreateEmptyContext(&pUserCtx);
			if (pUserCtx)
			{
				pUserCtx->AddAttribute(VSUC_Usage_Filter, L"item", L"folder");
				varRes = pUserCtx;
				hr = S_OK;
			}
		}
		break;
	}
	default:
		hr = CVCBaseFolderNode::GetProperty(propid, &varRes);
		break;
	}
	if (SUCCEEDED(hr))
		varRes.Detach(pvar);
	return hr;
}

HRESULT CVCFileGroup::GetCanonicalName(BSTR* pbstrName)
{
	CComPtr<VCFilter> spFilter;
	GetVCFilter(&spFilter);
	CHECK_ZOMBIE(spFilter, IDS_ERR_FILTER_ZOMBIE);
	return spFilter->get_CanonicalName(pbstrName);
}

HRESULT CVCFileGroup::GetName(BSTR* pbstrName)
{
	CComPtr<VCFilter> spFilter;
	GetVCFilter(&spFilter);
	CHECK_ZOMBIE(spFilter, IDS_ERR_FILTER_ZOMBIE);
	return spFilter->get_Name(pbstrName);
}

/////////////////////////////////////////////////////////////////////////////
// IVCFolderNodeProps
// 

STDMETHODIMP CVCFileGroup::get_Name( BSTR *pVal )
{
	CComQIPtr<VCFilter> pFilter = m_dispkeyItem;
	RETURN_ON_NULL2(pFilter, E_NOINTERFACE);
	return pFilter->get_Name( pVal );
}

STDMETHODIMP CVCFileGroup::put_Name( BSTR newVal )
{
	CComQIPtr<VCFilter> pFilter = m_dispkeyItem;
	RETURN_ON_NULL2(pFilter, E_NOINTERFACE);

	return pFilter->put_Name( newVal );
}

STDMETHODIMP CVCFileGroup::get_Filter( BSTR *pVal )
{
	CComQIPtr<VCFilter> pFilter = m_dispkeyItem;
	RETURN_ON_NULL2(pFilter, E_NOINTERFACE);

	return pFilter->get_Filter( pVal );
}

STDMETHODIMP CVCFileGroup::put_Filter( BSTR newVal )
{
	CComQIPtr<VCFilter> pFilter = m_dispkeyItem;
	RETURN_ON_NULL2(pFilter, E_NOINTERFACE);

	return pFilter->put_Filter( newVal );
}

STDMETHODIMP CVCFileGroup::get_ParseFiles(VARIANT_BOOL* pbParse)
{
	CComQIPtr<VCFilter> pFilter = m_dispkeyItem;
	RETURN_ON_NULL2(pFilter, E_NOINTERFACE);

	return pFilter->get_ParseFiles( pbParse );
}

STDMETHODIMP CVCFileGroup::put_ParseFiles(VARIANT_BOOL bParse)
{
	CComQIPtr<VCFilter> pFilter = m_dispkeyItem;
	RETURN_ON_NULL2(pFilter, E_NOINTERFACE);

	return pFilter->put_ParseFiles( bParse );
}

STDMETHODIMP CVCFileGroup::get_SourceControlFiles(VARIANT_BOOL* pbSCC)
{
	CComQIPtr<VCFilter> pFilter = m_dispkeyItem;
	RETURN_ON_NULL2(pFilter, E_NOINTERFACE);

	return pFilter->get_SourceControlFiles( pbSCC );
}

STDMETHODIMP CVCFileGroup::put_SourceControlFiles(VARIANT_BOOL bSCC)
{
	CComQIPtr<VCFilter> pFilter = m_dispkeyItem;
	RETURN_ON_NULL2(pFilter, E_NOINTERFACE);

	return pFilter->put_SourceControlFiles( bSCC );
}

//-----------------------------------------------------------------------------
// Used to implement RTTI without the overhead. Derived classes should check for
// their type and if not return their parents type.
//-----------------------------------------------------------------------------
UINT CVCFileGroup::GetKindOf(void) const
{
	return Type_CVCFileGroup;
}

HRESULT CVCFileGroup::GetExtObject(CComVariant& varRes)
{
	CComPtr<IDispatch> spDispFilter;
	GetVCItem(&spDispFilter);
	CComQIPtr<VCFilter> spFilter = spDispFilter;
	RETURN_ON_NULL(spFilter);

	HRESULT hr = S_OK;

	CComPtr<IDispatch> spDispParent;
	spFilter->get_Parent(&spDispParent);
	CComQIPtr<VCProject> spProjectParent = spDispParent;
	CComQIPtr<VCFilter> spFilterParent = spDispParent;
	if (spProjectParent == NULL && spFilterParent == NULL)
		return E_UNEXPECTED;

	// need the project regardless
	CVCProjectNode* pProjNode = GetVCProjectNode();
	RETURN_ON_NULL(pProjNode);
	CComVariant var;
	pProjNode->GetExtObject(var);
	if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
		return E_FAIL;
	CComPtr<IDispatch> spDispProj = var.pdispVal;

	CComPtr<IDispatch> spDispItems;
	CComQIPtr<IDispatch> spAutoParent;
	if (spProjectParent)	// project is parent
	{
		spAutoParent = spDispProj;
		hr = spProjectParent->get_Items(&spDispItems);
	}
	else	// filter is parent
	{
		CComQIPtr<IVCExternalCookie> spCookie = spFilterParent;
		RETURN_ON_NULL(spCookie);
		void* pCookie = NULL;
		spCookie->get_ExternalCookie(&pCookie);
		RETURN_ON_NULL(pCookie);
		CVCFileGroup* pParentGroup = (CVCFileGroup*)pCookie;
		CComVariant var2;
		pParentGroup->GetExtObject(var2);
		if (var2.vt != VT_DISPATCH && var2.vt != VT_UNKNOWN)
			return E_FAIL;
		spAutoParent = var2.pdispVal;
		hr = spFilterParent->get_Items(&spDispItems);
	}
	RETURN_ON_FAIL_OR_NULL(hr, spDispItems);

	CComPtr<CAutoItems> pAutoItems;
	hr = CAutoItems::CreateInstance(&pAutoItems, spAutoParent, spDispProj, spDispItems);
	RETURN_ON_FAIL(hr);

	CComPtr<CAutoFilter> pAutoFilter;
	hr = CAutoFilter::CreateInstance(&pAutoFilter, pAutoItems, spDispProj, spDispFilter);	
	RETURN_ON_FAIL(hr);

	varRes.vt = VT_DISPATCH;
	varRes.pdispVal = (IDispatch *)pAutoFilter.Detach();

	return S_OK;
}

// automation extender methods
STDMETHODIMP CVCFileGroup::get_Extender(BSTR bstrName, IDispatch **ppDisp)
{
	return GetExtender( s_wszCATID, bstrName, ppDisp );
}

STDMETHODIMP CVCFileGroup::get_ExtenderNames(VARIANT *pvarNames)
{
	return GetExtenderNames( s_wszCATID, pvarNames );
}

STDMETHODIMP CVCFileGroup::get_ExtenderCATID(BSTR *pbstrGuid)
{
	CHECK_POINTER_NULL( pbstrGuid );
	CComBSTR bstrRetVal = s_wszCATID;
	*pbstrGuid = bstrRetVal.Detach();
	return S_OK;
}

STDMETHODIMP CVCFileGroup::MapPropertyToCategory( DISPID dispid, PROPCAT* ppropcat)
{
	switch( dispid )
	{
		case VCFOLDNODID_ParseFiles:
		case VCFOLDNODID_SourceControlFiles:
			*ppropcat = (PROPCAT)IDS_ADVANCEDCAT;
			break;
		case VCFOLDNODID_Name:
		case VCFOLDNODID_Filter:
		default:
			*ppropcat = (PROPCAT)IDS_GENERALCAT;
			break;
	}
	return S_OK;
}

STDMETHODIMP CVCFileGroup::GetCategoryName( PROPCAT propcat, LCID lcid, BSTR* pbstrName)
{
	CComBSTR bstrCat;
	if( propcat )
	    bstrCat.LoadString((UINT)propcat);
	return bstrCat.CopyTo(pbstrName);
}

STDMETHODIMP CVCFileGroup::GetLocalizedPropertyInfo( DISPID dispid, LCID localeID, BSTR *pbstrName, BSTR *pbstrDesc)
{
	CComQIPtr<IDispatch> pdisp;
	QueryInterface(__uuidof(IDispatch), (void**)&pdisp);
	RETURN_ON_NULL(pdisp);

	CComPtr<ITypeInfo> pTypeInfo;
	HRESULT hr = pdisp->GetTypeInfo(0, LANG_NEUTRAL, &pTypeInfo);
	CComQIPtr<ITypeInfo2> pTypeInfo2 = pTypeInfo;
	RETURN_ON_FAIL_OR_NULL(hr, pTypeInfo2);

	CComBSTR bstrDoc;
	hr = pTypeInfo2->GetDocumentation2(dispid, localeID, &bstrDoc, NULL, NULL);
	RETURN_ON_FAIL_OR_NULL(hr, bstrDoc.m_str);
	
	LPOLESTR pDesc = wcsstr( bstrDoc, L": " );
	RETURN_ON_NULL(pDesc);

	int nSize = (int)(pDesc - bstrDoc);
	pDesc+=2;

	CComBSTR bstrName( nSize-1, bstrDoc );
	if (pbstrName != NULL)
		*pbstrName = bstrName.Detach();

	CComBSTR bstrDesc( pDesc );
	if (pbstrDesc != NULL)
		*pbstrDesc = bstrDesc.Detach();
	
	return S_OK;
}

STDMETHODIMP CVCFileGroup::GetClassName(BSTR* pbstrClassName)
{
	CHECK_POINTER_NULL(pbstrClassName);
	CComBSTR bstrClassName;
	bstrClassName.LoadString(IDS_FILTER_PROPERTIES);
	*pbstrClassName = bstrClassName.Detach();
	if (*pbstrClassName)
		return S_OK;
	else
		return E_OUTOFMEMORY;
}

