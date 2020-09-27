// VCPrjNode.cpp: implementation of the CVCProjectNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BldPkg.h"
#include "PrjNode.h"
#include "..\resdll\gpcmd.h"
#include "vcfgrp.h"
#include "vcfile.h"
#include "vcarchy.h"
#include "vssccmgr.h"
#include "stdidcmd.h"
#include "vsshlids.h"
#include <docobj.h>
#include <vcguid.h>
#include "AutoProject.h"
#include <vsqryed.h>
#include "context.h"

#ifndef ECMD_WEBDEPLOY
#define ECMD_WEBDEPLOY 5000
#endif	// ECMD_WEBDEPLOY

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// static member initialization

const LPOLESTR CVCProjectNode::s_wszCATID = L"{EE8299CB-19B6-4f20-ABEA-E1FD9A33B683}";

//-----------------------------------------------------------------------------
// create an instance of a project node with refrence count 1
//-----------------------------------------------------------------------------
HRESULT CVCProjectNode::CreateInstance(CVCProjectNode **ppProjectNode)
{
	CComObject<CVCProjectNode> *pPrjObj = NULL;	 // created with 0 ref count
	HRESULT hr = CComObject<CVCProjectNode>::CreateInstance(&pPrjObj);

	if (SUCCEEDED(hr))
		pPrjObj->AddRef();
	*ppProjectNode = pPrjObj;

	return hr;
}

CVCProjectNode::CVCProjectNode(void)
{
	m_pArchy = NULL;
	m_bStartedClose = false;
}

CVCProjectNode::~CVCProjectNode()
{
}

CVCArchy* CVCProjectNode::GetArchy()
{ 
	return m_pArchy;
}

void CVCProjectNode::SetArchy(CVCArchy* pArchy)
{
	m_pArchy = pArchy;
	if (m_pArchy != NULL)
		m_marshalVsHierarchy.Init(static_cast<IVsHierarchy*>(pArchy));	// yes, I WANT the static cast here!
	else
		m_marshalVsHierarchy.Clear();
}

HRESULT CVCProjectNode::GetVsHierarchy(IVsHierarchy** ppHier, BOOL bInMainThread /* = FALSE */)
{
	CHECK_POINTER_NULL(ppHier);

	if (bInMainThread)
	{
		CComPtr<IVsHierarchy> spHier = VCQI_cast<IVsHierarchy>(m_pArchy);
		*ppHier = spHier.Detach();
		return S_OK;
	}

	VSASSERT(m_marshalVsHierarchy.IsInit(), "GetVsHierarchy being called from secondary thread before initialization in primary thread");

	// yes, I know, we're not dealing with the aggregated version of the pointer here, but that isn't 
	// really necessary since we'll only be here in secondary threads, like in a build
	return m_marshalVsHierarchy.UnmarshalPtr(ppHier);
}

//---------------------------------------------------------------------------
// Returns our Hierarchy object
//---------------------------------------------------------------------------
CVCArchy* CVCProjectNode::GetHierarchy(void) const
{
	VSASSERT(NULL != m_pArchy, "Our hierarchy isn't initialized!");
	return m_pArchy;
}

//-----------------------------------------------------------------------------
// Reads the project file (if we havent' already done so), and loads the project 
// into memory.
//-----------------------------------------------------------------------------
BOOL CVCProjectNode::Open()
{
	BOOL bRetVal = TRUE;
// open the project file. For now, we pretend we have one.
	return bRetVal;
}

//---------------------------------------------------------------------------
// return the VSITEMID value for this node
// a cast of the this pointer to the VSITEMID (dword) type
// exception will be for root nodes which should return VSITEMID_ROOT
//---------------------------------------------------------------------------
VSITEMID CVCProjectNode::GetVsItemID(void) const
{
	VSASSERT(GetParent() == NULL, "Shouldn't have parent for a project!");
	return VSITEMID_ROOT;
}

UINT CVCProjectNode::GetIconIndex(ICON_TYPE iconType)
{
#ifdef AARDVARK
	if(iconType == ICON_StateImage)
		return STATEICON_NOSTATEICON;

	CComBSTR bstrName;
	GetName( &bstrName );
	if( wcsncmp( bstrName, L"CSharp", wcslen( L"CSharp" ) ) == 0 )
		return BMP_CSPROJNODE;
	else if( wcsncmp( bstrName, L"VB", wcslen( L"VB" ) ) == 0 )
		return BMP_VBPROJNODE;
	else
		return BMP_PROJNODE;
#endif // AARDVARK

	UINT index = BMP_SOLUTION;
	if(iconType == ICON_StateImage)
	{
		CVCArchy *pArchy;

		pArchy = (CVCArchy *)GetHierarchy();
		if (pArchy)
		{
			CComBSTR bstrPath;
			LPOLESTR pszFullPaths;
			VSSCCSTATUS dwSccStatus;
			index = STATEICON_NOSTATEICON;
			
			CComPtr<VCProject> pProject;
			GetVCProject(&pProject);
			RETURN_ON_NULL2(pProject, index);

			pProject->get_ProjectFile( &bstrPath );
			pszFullPaths = bstrPath;


			CComPtr<IVsSccManager2> srpIVsSccManager2;
			if (GetBuildPkg() && GetBuildPkg()->GetSccManager(&srpIVsSccManager2))
			{
				VsStateIcon siGlyph = STATEICON_NOSTATEICON;
				srpIVsSccManager2->GetSccGlyph(1, &pszFullPaths, &siGlyph, &dwSccStatus);
				index = siGlyph;
			}
		}
	}
	else
	{
		index = BMP_PROJNODE;
	}
	return index;
}

HRESULT CVCProjectNode::DoDefaultAction(BOOL fNewFile)
{
	return S_OK;
}

HRESULT CVCProjectNode::GetCanonicalName(BSTR* pbstrName)
{
	CComPtr<VCProject> pProject;
	GetVCProject(&pProject);
	RETURN_ON_NULL2(pProject, E_NOINTERFACE);

	return pProject->get_ProjectFile(pbstrName);
}

HRESULT CVCProjectNode::GetName(BSTR* pbstrName)
{
	CComPtr<VCProject> pProject;
	GetVCProject(&pProject);
	if (pProject)
		return pProject->get_Name(pbstrName);

	CHECK_ZOMBIE(NULL, IDS_ERR_PROJ_ZOMBIE);
	return S_OK;
}

void CVCProjectNode::OnActiveProjectCfgChange()
{
	if (m_pArchy == NULL)
		return;		// nothing we can do

	// the shell isn't paying attention to the fact that we really want to have the
	// OnPropertyChanged call passed down to all members of the hierarchy, so we
	// have to walk through the list and do it ourselves.  Sigh.  This code assumes
	// that only files can have per config changes in the icon.

	// walk through the file list and tell all of them that their icon may have changed
	CComQIPtr<VCProject> spProject = m_dispkeyItem;
	if (spProject == NULL)
		return;		// nothing more we can do

	CComPtr<IDispatch> spDispFiles;
	spProject->get_Files(&spDispFiles);
	CComQIPtr<IVCCollection> spFiles = spDispFiles;
	if (spFiles == NULL)
		return;		// nothing more we can do

	CComPtr<IEnumVARIANT> spEnum;
	spFiles->_NewEnum(reinterpret_cast<IUnknown **>(&spEnum));
	if (spEnum == NULL)
		return;		// nothing more we can do

	spEnum->Reset();
	while (TRUE)
	{
		CComVariant var;
		HRESULT hr = spEnum->Next(1, &var, NULL);
		if (hr != S_OK)
			break;

		if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
			continue;

		CComQIPtr<IVCExternalCookie> spCookie = var.pdispVal;
		if (spCookie == NULL)
			continue;

		void* pNode = NULL;
		spCookie->get_ExternalCookie(&pNode);
		if (pNode == NULL)
			continue;

		CVCFileNode* pFileNode = (CVCFileNode*)pNode;
		m_pArchy->OnPropertyChanged(pFileNode, VSHPROPID_IconIndex, 0x0);
	}
}

//---------------------------------------------------------------------------
//	REVIEW: This is for debugging only
//---------------------------------------------------------------------------
HRESULT CVCProjectNode::DisplayContextMenu(VARIANT *pvaIn)
{
	CComPtr<IOleCommandTarget> spTarget = VCQI_cast<IOleCommandTarget>(this);
	return ShowContextMenu(IDM_VS_CTXT_PROJNODE, pvaIn, guidSHLMainMenu, spTarget);
}

//-----------------------------------------------------------------------------
// Called to update the status of our commands
//-----------------------------------------------------------------------------
STDMETHODIMP CVCProjectNode::QueryStatus(
	const GUID *pguidCmdGroup,
	ULONG cCmds,
	OLECMD prgCmds[],
	OLECMDTEXT *pCmdText)
{
	if(!pguidCmdGroup)
		return OLECMDERR_E_UNKNOWNGROUP;

	if(*pguidCmdGroup == guidVSStd2K)
	{
		for (ULONG i=0; i<cCmds; i++)
		{
			enumEnableStates fEnable = eEnableSupported;
#ifdef _DEBUG_MENU
			ATLTRACE(_T("ProjectNode guidVSStd2K: cmdid=%ld (%lx)\n"), prgCmds[i].cmdID, prgCmds[i].cmdID);
#endif
			switch(prgCmds[i].cmdID)
			{
				case ECMD_PROJSETTINGS:
				case ECMD_ADDRESOURCE:
					fEnable = eEnableSupportedAndEnabled;
					break;
				case ECMD_ADDWEBREFERENCE:
				case ECMD_ADDWEBREFERENCECTX:
					fEnable = CanAddWebRef() ? eEnableSupportedAndEnabled : eEnableSupported;
					break;
				case ECMD_LINKONLY:
					fEnable = SupportsBuildType(TOB_Link);
					break;
				case ECMD_WEBDEPLOY:
					fEnable = SupportsBuildType(TOB_Deploy);
					break;
				case ECMD_XBOXIMAGE:
					fEnable = SupportsBuildType(TOB_XboxImage);
					break;
				case ECMD_XBOXDEPLOY:
					fEnable = SupportsBuildType(TOB_XboxDeploy);
					break;
				default:
					return CVCBaseFolderNode::QueryStatus(pguidCmdGroup, cCmds, prgCmds, pCmdText);
			}
			prgCmds[i].cmdf = TranslateEnableState(fEnable);
		}

		return NOERROR;
	}
	else if(*pguidCmdGroup == guidVSStd97)
	{
		for (ULONG i=0; i<cCmds; i++)
		{
			enumEnableStates fEnable = eEnableSupported;
#ifdef _DEBUG_MENU
			ATLTRACE(_T("ProjectNode guidVSStd97: cmdid=%ld (%lx)\n"), prgCmds[i].cmdID, prgCmds[i].cmdID);
#endif
			switch(prgCmds[i].cmdID)
			{
				case cmdidAddClass:
				case cmdidSetStartupProject:
					fEnable = eEnableSupportedAndEnabled;
					break;
				case cmdidPaste:
					fEnable = CanPaste() ? eEnableSupportedAndEnabled : eEnableSupported;
					break;
				default:
				{	// REVIEW: This won't work if cCmds > 1)
					VSASSERT(cCmds == 1, "REVIEW: our QueryStatus doesn't work if cCmds > 1");
					return CVCBaseFolderNode::QueryStatus(pguidCmdGroup, cCmds, prgCmds, pCmdText);	 // OLECMDERR_E_NOTSUPPORTED;
				}
			}
			prgCmds[i].cmdf = TranslateEnableState(fEnable);
		}
		return NOERROR;
	}
	// Let base class deal with the rest
	return CVCBaseFolderNode::QueryStatus(pguidCmdGroup, cCmds, prgCmds, pCmdText);
}

HRESULT CVCProjectNode::GetGuidProperty( VSHPROPID propid, GUID *pguid)
{
	CHECK_POINTER_NULL(pguid)

	switch(propid)
	{
		case VSHPROPID_TypeGuid:
		{
			*pguid = IID_IVCArchy;
			return S_OK;
		}
		case VSHPROPID_CmdUIGuid:
		{
			*pguid = IID_IVCArchy;
			return S_OK;
		}
		case VSHPROPID_ProjectIDGuid:
		{
			HRESULT hr;
			CComBSTR bstrGuid;
			CComQIPtr<VCProject> pProj = m_dispkeyItem;
			hr = pProj->get_ProjectGUID( &bstrGuid );
			hr = CLSIDFromString( bstrGuid, pguid );
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CVCProjectNode::SetGuidProperty( VSHPROPID propid, GUID guid)
{
	switch(propid)
	{
		case VSHPROPID_ProjectIDGuid:
		{
			HRESULT hr;
			CComQIPtr<VCProject> pProj = m_dispkeyItem;

		    	CComBSTR bstrOldGuid;
			hr = pProj->get_ProjectGUID( &bstrOldGuid );
			
			LPOLESTR lpszBuf = NULL;
			hr = StringFromCLSID(guid, &lpszBuf);
			CComBSTR bstrGuid;
			bstrGuid = lpszBuf;
			::CoTaskMemFree(lpszBuf);

			if( bstrGuid == bstrOldGuid )  
			    return S_OK;		// Shell doing something really stupid here ! Don't write if they match.

			hr = pProj->put_ProjectGUID( bstrGuid );
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CVCProjectNode::SetProperty(VSHPROPID propid, const VARIANT& var)
{
	if( propid == VSHPROPID_EditLabel )
	{
		// don't try to set if it hasn't actually changed
		CComBSTR bstrOldName;
		get_Name( &bstrOldName );
		if( wcscmp( bstrOldName, var.bstrVal ) != 0 )
			put_Name( var.bstrVal );
		return S_OK;
	}
	else if( propid == VSHPROPID_OwnerKey )
	{
		CComPtr<VCProject> pProj;
		HRESULT hr = GetVCProject( &pProj );
		RETURN_ON_FAIL_OR_NULL(hr, pProj);
		return pProj->put_OwnerKey( var.bstrVal );
	}

	// handle project node properties here...
	else 
		return CVCBaseFolderNode::SetProperty(propid, var);
}

//---------------------------------------------------------------------------
//	Returns the requested property in the given variant.
//---------------------------------------------------------------------------
HRESULT CVCProjectNode::GetProperty(VSHPROPID propid, VARIANT *pvar)
{
	// handle project node properties here...
	HRESULT hr = E_FAIL;
	CComVariant varRes;
	switch( propid )
	{
	case VSHPROPID_Name:
	case VSHPROPID_SaveName:
	case VSHPROPID_Caption:
	case VSHPROPID_EditLabel:
	{	// return this node's displayed caption
		pvar->vt = VT_BSTR;
		CComBSTR bstrName;
		hr = GetName(&bstrName);
		if (SUCCEEDED(hr))
			pvar->bstrVal = bstrName.Detach();
		return hr;
	}
	case VSHPROPID_UserContext:
	{
		varRes.ChangeType(VT_UNKNOWN);
		varRes.punkVal = NULL;

		CComPtr<IVsUserContext> pUserCtx;
		CComPtr<IVsMonitorUserContext> pmuc;
		if (SUCCEEDED(ExternalQueryService(SID_SVsMonitorUserContext, IID_IVsMonitorUserContext, (void **)&pmuc)))
		{

			CComQIPtr<VCProject> pProj = m_dispkeyItem;
			
			CComBSTR bstrKeyword;
			pProj->get_Keyword( &bstrKeyword );
			
			if( m_bstrProjectType.Length() == 0 )
			{
				CComPtr<IVCGenericConfiguration> spGenCfg;
				hr = GetActiveConfig(&spGenCfg);
				CComQIPtr<IVCCfg> spCfg = spGenCfg;
				if( FAILED(hr) || spCfg == NULL )
				{
					hr = S_OK;
					break;
				}

				CComPtr<IDispatch> pDisp;
				hr = spCfg->get_Object(&pDisp);
				CComQIPtr<VCConfiguration> pConfig = pDisp;
				if( FAILED(hr) || pConfig == NULL )
				{
					hr = S_OK;
					break;
				}

				ConfigurationTypes cfgType;
				pConfig->get_ConfigurationType( &cfgType );
				switch( cfgType )
				{
					case typeApplication:
						m_bstrProjectType = L"exe";
						break;
					case typeDynamicLibrary:
						m_bstrProjectType = L"dll";
						break;
					case typeStaticLibrary:
						m_bstrProjectType = L"lib";
						break;
					case typeGeneric:
						m_bstrProjectType = L"util";
						break;
					case typeUnknown:
						m_bstrProjectType = L"mak";
						break;
					default:
						m_bstrProjectType = L"exe";
						break;
				}
			}
				
			CComBSTR bstrSccProjectName;
			pProj->get_SccProjectName( &bstrSccProjectName );
			LPOLESTR szSCC;
			if( bstrSccProjectName.Length() > 0 )
				 szSCC = L"true";
			else
				 szSCC = L"false";
				
			pmuc->CreateEmptyContext(&pUserCtx);
			pUserCtx->AddAttribute(VSUC_Usage_Filter, L"product",	L"vc");
			pUserCtx->AddAttribute(VSUC_Usage_Filter, L"item",		L"project");
			pUserCtx->AddAttribute(VSUC_Usage_Filter, L"target",	L"win32");
			pUserCtx->AddAttribute(VSUC_Usage_Filter, L"project",	m_bstrProjectType );
			pUserCtx->AddAttribute(VSUC_Usage_Filter, L"projtype", bstrKeyword );
			pUserCtx->AddAttribute(VSUC_Usage_Filter, L"sourcecontrol", szSCC);
			
			varRes = pUserCtx;
			hr = S_OK;
		}
		break;
	}
	case VSHPROPID_OwnerKey:
	{
		CComPtr<VCProject> pProj;
		hr = GetVCProject( &pProj );
		RETURN_ON_FAIL_OR_NULL(hr, pProj);
		CComBSTR bstrTemp;
		pProj->get_OwnerKey( &bstrTemp );
		pvar->vt = VT_BSTR;
		pvar->bstrVal = bstrTemp.Detach();
		return S_OK;
	}
	case VCPROPID_ProjBldBscFile:
	{
		CComBSTR bstrTemp;
		hr = GetBscFileName(&bstrTemp);
		pvar->vt = VT_BSTR;
		pvar->bstrVal = bstrTemp.Detach();
		return hr;
	}
	default:
		hr = CVCBaseFolderNode::GetProperty(propid, &varRes);
		break;
	}
	varRes.Detach(pvar);
	return hr;
}


HRESULT CVCProjectNode::GetBscFileName(BSTR* pbstrBSCName)
{
	*pbstrBSCName = NULL;

	CComPtr<IDispatch> spDispProjCfg;
	HRESULT hr = GetActiveVCProjectConfig(&spDispProjCfg);
	CComQIPtr<VCConfiguration> pVcCfg = spDispProjCfg;
	RETURN_ON_FAIL_OR_NULL(hr, pVcCfg);

	// now, let's try to pick up the per-tool stuff
	VARIANT_BOOL b;
	pVcCfg->get_BuildBrowserInformation( &b );

	if( b == VARIANT_TRUE )
	{
		CComPtr<IDispatch> pDispColl;
		pVcCfg->get_Tools(&pDispColl);
		CComQIPtr<IVCCollection> pToolsColl = pDispColl;
		CComBSTR bstrName( L"VCBscMakeTool");
		CComPtr<IDispatch> pDispTool;
		hr = pToolsColl->Item( CComVariant( bstrName ), &pDispTool );

		CComQIPtr<IVCToolImpl> spToolImpl = pDispTool;
		long nPathID = 0;
		if ( spToolImpl != NULL )
		{
			CComQIPtr<IVCPropertyContainer> spPropContainer = pVcCfg;
			RETURN_ON_NULL2(spPropContainer, E_UNEXPECTED);
			
			CComBSTR bstrOutName;
			hr = spPropContainer->GetEvaluatedStrProperty(VCBSCID_OutputFile, &bstrOutName);
			if (hr != S_OK)
				return hr;
			
			CStringW strName = bstrOutName;
			if (strName.IsEmpty())
				return E_FAIL;
			
			CComBSTR bstrProjName;
			hr = spPropContainer->GetStrProperty(VCPROJID_ProjectDirectory, &bstrProjName); 
			if (FAILED(hr))
				return hr;
	
			CStringW strDirName = bstrProjName;
			CDirW dirProj;
			if (!dirProj.CreateFromKnown(strDirName))
				return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
			
			CPathW path;
			if( !path.CreateFromDirAndFilename(dirProj, strName))
				return E_FAIL;
			path.GetActualCase(TRUE);
			*pbstrBSCName = SysAllocString((LPCOLESTR)path);
		
			return S_OK;
		}
	}
	return S_FALSE;
}


//-----------------------------------------------------------------------------
// IProvidePropertyBuilder methods
//-----------------------------------------------------------------------------
STDMETHODIMP CVCProjectNode::MapPropertyToBuilder( LONG dispid, LONG *pdwCtlBldType, BSTR *pbstrGuidBldr, VARIANT_BOOL *pfRetVal )
{
	CHECK_POINTER_NULL( pfRetVal );
	CHECK_POINTER_NULL( pbstrGuidBldr );
	if( dispid == VCPRJNODID_ProjectDependencies )
	{
		*pfRetVal = VARIANT_TRUE;
		*pdwCtlBldType = CTLBLDTYPE_FINTERNALBUILDER;
	}
	else
		*pfRetVal = VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CVCProjectNode::ExecuteBuilder( LONG dispid, BSTR bstrGuidBldr, IDispatch *pdispApp, LONG_PTR hwndBldrOwner, VARIANT *pvarValue, VARIANT_BOOL *pfRetVal )
{
	CComPtr<IOleCommandTarget> pCmdTgt;
	HRESULT hr = ExternalQueryService( SID_SUIHostCommandDispatcher, IID_IOleCommandTarget, (void**)&pCmdTgt );
	RETURN_ON_FAIL_OR_NULL(hr, pCmdTgt);
	return pCmdTgt->Exec( &CLSID_StandardCommandSet97, cmdidProjectDependencies, OLECMDEXECOPT_DODEFAULT, NULL, NULL );
}

STDMETHODIMP CVCProjectNode::IsPropertyReadOnly(DISPID dispid, BOOL *fReadOnly)
{ 
	CHECK_POINTER_NULL(fReadOnly);
	if( dispid == VCPRJNODID_Name )
		*fReadOnly = FALSE;
	else
		*fReadOnly = TRUE;
	return S_OK; 
}

STDMETHODIMP CVCProjectNode::GetLocalizedPropertyInfo(DISPID dispid, LCID localeID, BSTR *pbstrName, BSTR *pbstrDesc)
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

STDMETHODIMP CVCProjectNode::GetClassName(BSTR* pbstrClassName)
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

//-----------------------------------------------------------------------------
// IVsGetCfgProvider method
//-----------------------------------------------------------------------------
STDMETHODIMP CVCProjectNode::GetCfgProvider(IVsCfgProvider** ppCfgProvider)
{
	VSASSERT(m_pArchy != NULL, "Hey, somebody calling GetCfgProvider after the project has been closed!");
	CHECK_ZOMBIE(m_pArchy, IDS_ERR_PROJ_ZOMBIE);
	CHECK_POINTER_NULL(ppCfgProvider);
	return m_pArchy->QueryInterface(IID_IVsCfgProvider, (void**)ppCfgProvider);
}

//-----------------------------------------------------------------------------
// Called to execute our commands
//-----------------------------------------------------------------------------
STDMETHODIMP CVCProjectNode::Exec(
	const GUID *pguidCmdGroup,
	DWORD nCmdID,
	DWORD nCmdexecopt,
	VARIANT *pvaIn,
	VARIANT *pvaOut)
{
	if (!pguidCmdGroup)
		  return OLECMDERR_E_UNKNOWNGROUP;
	
	if(*pguidCmdGroup == guidVSStd2K)
	{
		switch(nCmdID)
		{
			case ECMD_PROJSETTINGS:
				return ShowSettings(CLSID_VCProject);
			case ECMD_LINKONLY:
				return DoBuildType(TOB_Link);
			case ECMD_WEBDEPLOY:
				return DoBuildType(TOB_Deploy);
			case ECMD_XBOXIMAGE:
				return DoBuildType(TOB_XboxImage);
			case ECMD_XBOXDEPLOY:
				return DoBuildType(TOB_XboxDeploy);
			default:
				return CVCBaseFolderNode::Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut);
		}
	}
	else if(*pguidCmdGroup == guidVSStd97)
		return CVCBaseFolderNode::Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut); // OLECMDERR_E_NOTSUPPORTED;

	return OLECMDERR_E_NOTSUPPORTED;
}

HRESULT CVCProjectNode::CleanUpUI( CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen /*= TRUE*/ )
{
	if (!m_bStartedClose)
		return OnDelete( pRoot, dwDelItemOp, bCloseIfOpen );
	return S_OK;
}

HRESULT CVCProjectNode::OnDelete( CVCNode *pRoot, DWORD dwDelItemOp, BOOL bCloseIfOpen /*= TRUE*/ )
{
	// cause the solution to shut down the project
	CComPtr<IVsSolution> pSolution;

	HRESULT hr = GetBuildPkg()->GetIVsSolution( &pSolution );
	VSASSERT( SUCCEEDED( hr ), "QueryService for solution failed!  Please note that the shell is not thread-safe..." );
	RETURN_ON_FAIL_OR_NULL(hr, pSolution);

	// remove project from solution
	if(dwDelItemOp == DELITEMOP_RemoveFromProject)
	{
		CComPtr<IVsHierarchy> spHier = VCQI_cast<IVsHierarchy>(GetHierarchy());
		hr = pSolution->CloseSolutionElement( SLNSAVEOPT_NoSave, spHier, 0 );
		// if closing the solution element failed, it's probably because the
		// project failed to load and hasn't actually been added to their list
		// yet, so try to close our hierarchy explicitly...
		// UNLESS (!!!) it's E_ABORT, in which case SCC may have sent that
		// back from QueryEditQuerySave(), indicating that we cannot dirty the
		// sln file!
		if( FAILED( hr ) && hr != E_ABORT )
			if( spHier )
				hr = spHier->Close();
		return hr;
	}
	// delete from disk
	else
	{
		CComPtr<VCProject> pProject;
		GetArchy()->GetVCProject( static_cast<CVCNode*>(this), &pProject );
		VSASSERT(pProject != NULL, "Hey, we have a NULL project!");
		RETURN_ON_NULL2(pProject, E_NOINTERFACE);
		CComPtr<IDispatch> pDisp;
		pProject->get_Files( &pDisp );
		CComQIPtr<IVCCollection> pColl = pDisp;
		RETURN_ON_NULL2(pColl, E_NOINTERFACE);
		long num_elems; 
		pColl->get_Count( &num_elems );
		// for each file in the project
		for( int i = 1; i <= num_elems; i++ )
		{
			// get the filename
			CComPtr<IDispatch> pDispFile;
			pColl->Item( CComVariant( i ), &pDispFile );
			CComQIPtr<VCFile> pFile = pDispFile;
			VSASSERT( pFile, "Non-file in file collection!" );
			CComBSTR bstrFullPath;
			pFile->get_FullPath( &bstrFullPath );

			// close the file window if it's open
			BOOL fIsOpen;
			CComPtr<IVsUIShellOpenDocument> pVsUIShellOpenDoc;	
			hr = ExternalQueryService( SID_SVsUIShellOpenDocument, 
									  IID_IVsUIShellOpenDocument, (void **)&pVsUIShellOpenDoc );
			VSASSERT( SUCCEEDED( hr ), "Failed to obtain VsUIShellOpenDoc service; either service ID has changed or you're in the wrong thread" );
					
			CComPtr<IVsUIHierarchy> spHier = VCQI_cast<IVsUIHierarchy>(GetHierarchy());
			CComPtr<IVsWindowFrame> pFrame;
			hr = pVsUIShellOpenDoc->IsDocumentOpen(
					spHier, 
					GetArchy()->ItemIDFromName( static_cast<CVCNode*>(this), bstrFullPath ),
					bstrFullPath,
					GUID_NULL,
					IDO_ActivateIfOpen,
					NULL,
					NULL,
					&pFrame,
					&fIsOpen );
			VSASSERT( SUCCEEDED( hr ), "Failed to determine if the file is open!  Are you in the right thread?" );

			if( fIsOpen )
			{
				// close the file
				hr = pFrame->CloseFrame( FRAMECLOSE_NoSave );
				RETURN_ON_FAIL(hr);
			}

			// do the actual delete
			CString strName;
			strName = bstrFullPath;

			SHFILEOPSTRUCT fop;
			fop.hwnd = NULL;
			fop.wFunc = FO_DELETE;
			fop.pFrom = strName;
			fop.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_NOCONFIRMATION;
			fop.fAnyOperationsAborted = NULL;
			fop.hNameMappings = NULL;;
			fop.lpszProgressTitle = NULL;
			fop.pTo = NULL;

			SHFileOperation( &fop );
		}
		// delete the project file
		CComBSTR bstrProjFile;
		pProject->get_ProjectFile( &bstrProjFile );

		CString strName;
		strName = bstrProjFile;

		SHFILEOPSTRUCT fop;
		fop.hwnd = NULL;
		fop.wFunc = FO_DELETE;
		fop.pFrom = strName;
		fop.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_NOCONFIRMATION;
		fop.fAnyOperationsAborted = FALSE;
		fop.hNameMappings = NULL;
		fop.lpszProgressTitle = NULL;
		fop.pTo = NULL;

		SHFileOperation( &fop );

		// remove project from solution
		CComPtr<IVsHierarchy> spHier = VCQI_cast<IVsHierarchy>(GetHierarchy());
		return pSolution->CloseSolutionElement( SLNSAVEOPT_NoSave, spHier, 0 );
	}
}

HRESULT CVCProjectNode::CleanUpBeforeDelete()
{
	// if we've already started closing, then we've already deleted the
	// objects and don't want to do it again
	if( !m_bStartedClose )
	{
		// we're deleting via the engine object model, not UI, so we set this flag
		// to keep us from trying to delete things twice
		m_bStartedClose = true;

		CComPtr<IDispatch> pProjectDisp;
		GetObjectsForDelete(NULL, &pProjectDisp);
		return CleanUpObjectModel(NULL, pProjectDisp);
	}
	else 
		m_bStartedClose = false;

	return S_OK;
}

HRESULT CVCProjectNode::GetObjectsForDelete(IDispatch** ppParent, IDispatch** ppItem)
{
	CHECK_POINTER_NULL(ppItem);

	if (ppParent != NULL)	// for project, don't need to worry about parent...
		*ppParent = NULL;

	if( m_dispkeyItem )
	{
		*ppItem = m_dispkeyItem;
		if (*ppItem)
			(*ppItem)->AddRef();
	}
	return S_OK;
}

HRESULT CVCProjectNode::CleanUpObjectModel(IDispatch* pParent, IDispatch* pProjectDisp)
{
	CHECK_READ_POINTER_NULL(pProjectDisp);

	CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = GetBuildPkg()->GetProjectEngine();
	RETURN_ON_NULL2(spProjEngineImpl, E_UNEXPECTED);
	return spProjEngineImpl->RemoveProjectUnderShell(pProjectDisp);
}

STDMETHODIMP CVCProjectNode::OnSaveProject(LPCOLESTR pszNewName)
{ 
	HRESULT hr;
	CComPtr<VCProject> pProject;
	GetVCProject(&pProject);
	CComQIPtr<IVCProjectImpl> pProjectImpl = pProject;
	RETURN_ON_NULL(pProjectImpl);

	// if we're under SCC
	CComBSTR bstrSccProjectName;
	CComQIPtr<VCProject> pProjectItem = m_dispkeyItem;
	RETURN_ON_NULL(pProjectItem);
	pProjectItem->get_SccProjectName( &bstrSccProjectName );
	if( bstrSccProjectName && bstrSccProjectName.Length() > 0 )
	{
		// ask SCC if we can save the file
		CComPtr<IVsQueryEditQuerySave> pQuerySave;
		hr = ExternalQueryService( SID_SVsQueryEditQuerySave, IID_IVsQueryEditQuerySave, (void**)&pQuerySave );
		if( SUCCEEDED( hr ) && pQuerySave )
		{
			VSQuerySaveResult fSaveResult;
			CComBSTR bstrFullPath;
			pProjectItem->get_ProjectFile( &bstrFullPath );
			hr = pQuerySave->QuerySaveFile( bstrFullPath, VSQEQS_AllowCheckout, NULL, &fSaveResult );
			if( fSaveResult != QSR_SaveOK )
				return E_FAIL;
		}
	}

	// check for NULL - cannot assign NULL to a CComBSTR (it will assert)
	if( !pszNewName )
		hr = pProjectImpl->DoSave( NULL );
	else
		hr = pProjectImpl->DoSave( pszNewName );
	return hr;
}

HRESULT CVCProjectNode::GetActiveConfig(IVCGenericConfiguration** ppGenCfg)
{
	CHECK_POINTER_NULL(ppGenCfg);

	CComPtr<IVsHierarchy> spHier;
	HRESULT hr = GetVsHierarchy(&spHier, TRUE /* main thread */);
	RETURN_ON_FAIL_OR_NULL(hr, spHier);

	CComPtr<IVsSolutionBuildManager> spSolnBldMgr;
	if( GetBuildPkg() == NULL )
		return E_FAIL;
	hr = GetBuildPkg()->GetSolutionBuildManager(&spSolnBldMgr, TRUE);
	RETURN_ON_FAIL_OR_NULL(hr, spSolnBldMgr);

	CComPtr<IVsProjectCfg> spVsProjCfg;
	hr = spSolnBldMgr->FindActiveProjectCfg(NULL, NULL, spHier, &spVsProjCfg);
	CComQIPtr<IVCGenericConfiguration> spGenCfg = spVsProjCfg;
	RETURN_ON_FAIL_OR_NULL(hr, spGenCfg);

	*ppGenCfg = spGenCfg.Detach();
	return S_OK;
}

HRESULT CVCProjectNode::CompileFileItems(CVCPtrList& rlistFiles)
{
	if (rlistFiles.IsEmpty())
		return S_OK;	// nothing to do

	HRESULT hr = S_OK;
	CComPtr<IVCGenericConfiguration> spGenCfg;
	hr = GetActiveConfig(&spGenCfg);
	RETURN_ON_FAIL_OR_NULL(hr, spGenCfg);

	ULONG celt = (ULONG) rlistFiles.GetCount();
	VCFile** rgpVCFile = (VCFile**) ::CoTaskMemAlloc(celt * sizeof(VCFile *));
	memset(rgpVCFile, 0, sizeof(VCFile*)*celt);
	RETURN_ON_NULL2(rgpVCFile, E_OUTOFMEMORY);

	int idx = 0;
	VCPOSITION pos = rlistFiles.GetHeadPosition();
	while (pos != NULL)
	{
		CVCFileNode* pFileNode = (CVCFileNode *)rlistFiles.GetNext(pos);
		CComPtr<VCFile> spFile;
		pFileNode->GetVCFile(&spFile);
		rgpVCFile[idx] = spFile;
		idx++;
	}

	hr = spGenCfg->StartFileBuild(celt, rgpVCFile);
	::CoTaskMemFree(rgpVCFile);

	return hr;
}

BOOL CVCProjectNode::IsProjectFileDirty()
{
	CComPtr<VCProject> pProject;
	GetVCProject(&pProject);
	RETURN_ON_NULL2(pProject, FALSE);

	HRESULT hr;
	VARIANT_BOOL bDirty;
	hr = pProject->get_IsDirty(&bDirty);

	return (bDirty == VARIANT_TRUE);
}

enumEnableStates CVCProjectNode::SupportsBuildType(bldActionTypes buildType)
{
	CComPtr<IVCGenericConfiguration> spGenCfg;
	HRESULT hr = GetActiveConfig(&spGenCfg);
	if (FAILED(hr))
		return eEnableInvisible;

	VARIANT_BOOL bSupported = VARIANT_FALSE;
	VARIANT_BOOL bAvailable = VARIANT_FALSE;

	hr = spGenCfg->SupportsBuildType(buildType, &bSupported, &bAvailable);
	if (FAILED(hr))
		return eEnableInvisible;

	if (bSupported == VARIANT_FALSE)
		return eEnableInvisible;
	else if (bAvailable == VARIANT_FALSE)	// supported, but not available
		return eEnableSupported;

	// now we need to figure out whether we've got a build in progress to determine whether it should be grayed out or active
	hr = spGenCfg->get_NoBuildIsInProgress(NULL);
	if (SUCCEEDED(hr))
		return eEnableSupportedAndEnabled;
	else	// build going, so gray this out
		return eEnableSupported;
}


BOOL CVCProjectNode::InBuild()
{
	CComPtr<IVCGenericConfiguration> spGenCfg;
	HRESULT hr = GetActiveConfig(&spGenCfg);
	RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, spGenCfg, TRUE);

	hr = spGenCfg->get_NoBuildIsInProgress(NULL);
	return (FAILED(hr));	// previous call succeeds if no build in progress
}

HRESULT CVCProjectNode::DoBuildType(bldActionTypes buildType)
{
	CComPtr<IVsOutputWindowPane> spOutputWindowPane;
	if (SUCCEEDED(CGenCfg::GetOutputWindowPane(&spOutputWindowPane)) && spOutputWindowPane != NULL)
	{
		if (GetBuildPkg()->NoBuildsInProgress())
			spOutputWindowPane->Clear();
		spOutputWindowPane->Activate();
	}

	CComPtr<IVCGenericConfiguration> spGenCfg;
	HRESULT hr = GetActiveConfig(&spGenCfg);
	RETURN_ON_FAIL_OR_NULL(hr, spGenCfg);

	return spGenCfg->StartAutomationBuild(buildType);
}

HRESULT CVCProjectNode::GetExtObject(CComVariant& varRes)
{
	varRes.vt = VT_EMPTY;
	IDispatch* pAutoProjects = g_pBuildPackage->GetAutoProjects();
	VSASSERT(pAutoProjects != NULL, "No auto projects list!");
	RETURN_ON_NULL(pAutoProjects);

	CComPtr<IDispatch> spDispProject;
	GetDispVCProject(&spDispProject);
	RETURN_ON_NULL(spDispProject);

	CComPtr<CAutoProject> pAutoProject;
	HRESULT hr = CAutoProject::CreateInstance(&pAutoProject, pAutoProjects, NULL, spDispProject);
	RETURN_ON_FAIL(hr);

	varRes.vt = VT_DISPATCH;
	varRes.pdispVal = (IDispatch *)pAutoProject.Detach();

	return S_OK;
}

STDMETHODIMP CVCProjectNode::get_ProjectFile( BSTR *pVal )
{
	CHECK_POINTER_NULL(pVal);
	CComQIPtr<VCProject> pProject = m_dispkeyItem;
	CHECK_ZOMBIE(pProject, IDS_ERR_PROJ_ZOMBIE);
	return pProject->get_ProjectFile( pVal );
}

STDMETHODIMP CVCProjectNode::get_Name( BSTR *pVal )
{
	CHECK_POINTER_NULL(pVal);
	CComQIPtr<VCProject> pProject = m_dispkeyItem;
	CHECK_ZOMBIE(pProject, IDS_ERR_PROJ_ZOMBIE);
	return pProject->get_Name( pVal );
}

STDMETHODIMP CVCProjectNode::put_Name( BSTR Val )
{
	CComQIPtr<VCProject> pProject = m_dispkeyItem;
	CHECK_ZOMBIE(pProject, IDS_ERR_PROJ_ZOMBIE);

	HRESULT hr = pProject->put_Name( Val );
	CFirePropNotifyEvent::FireOnChanged( GetUnknown(), VCPRJNODID_Name );
	return hr;
}

STDMETHODIMP CVCProjectNode::get_ProjectDependencies( BSTR *pVal )
{
	CHECK_POINTER_NULL( pVal );
	CComBSTR bstrDeps( L"" );
	*pVal = bstrDeps.Detach();
	return S_OK;
}

// automation extender methods
STDMETHODIMP CVCProjectNode::get_Extender(BSTR bstrName, IDispatch **ppDisp)
{
	return GetExtender( s_wszCATID, bstrName, ppDisp );
}

STDMETHODIMP CVCProjectNode::get_ExtenderNames(VARIANT *pvarNames)
{
	return GetExtenderNames( s_wszCATID, pvarNames );
}

STDMETHODIMP CVCProjectNode::get_ExtenderCATID(BSTR *pbstrGuid)
{
	CHECK_POINTER_NULL( pbstrGuid );
	CComBSTR bstrRetVal = s_wszCATID;
	*pbstrGuid = bstrRetVal.Detach();
	return S_OK;
}

//-----------------------------------------------------------------------------
// Called on a drop operation. rDataObject contains the dragdrop format from 
// the source. Also passed in is pOurDataSource. This is our datasource if we 
// sourced the drag or clipboard operation. If it is non-null, we can assume that
// the source is us and we can use the information in pOurDataSource to complete
// the move. This is important for web scenarios where we delay render the GetData() so
// that we can ensure local copies of files exist. We don't want to call GetData() in 
// that scenario, since there is no need to get local copies (dirprj handles 
// internal moves and copies).
//-----------------------------------------------------------------------------
class CDropMoveHelper
{
public:
	CDropMoveHelper() : m_rgMovedNodes(NULL) {}
	~CDropMoveHelper() { delete [] m_rgMovedNodes; }

	CVCNode** m_rgMovedNodes;
};

HRESULT CVCProjectNode::OnDropMove(CVCOleDataObject& rDataObject, CVCNode* pDropNode, CVCProjDataSource* pOurDataSource,
	BOOL bCrossVC, BOOL bDrag, BOOL& bReported)
{
	RETURN_ON_NULL2(pDropNode, S_FALSE);

	// check out the project file before adding/removing anything to the project
	if ((bCrossVC || pOurDataSource == NULL) && CanDirtyProject() != S_OK)
		return OLE_E_PROMPTSAVECANCELLED;

	HRESULT hr = S_OK;

	if (bCrossVC)
		return DoCrossVCMoveOrCopy(rDataObject, pDropNode, true /* move */, bDrag, bReported);
	else if (pOurDataSource == NULL)	// We didn't source this thing, do a copy instead
		return OnDropCopy(rDataObject, pDropNode, pOurDataSource, bCrossVC, bDrag, bReported); 

	const CVCNodeArray& rgNodes = pOurDataSource->GetNodeList();
	int numFiles = (int) rgNodes.GetSize();
	if (numFiles == 0)
		return S_OK;

	// first, let's make sure we don't have a single file/folder we're trying to drop back on the same spot
	if (numFiles == 1)
	{
		CVCNode* pNode = rgNodes[0];
		if (pNode->GetParent() == pDropNode)	// this is almost certainly a no-op, treat it like one
			return S_OK;
	}

	// check out the project file before adding/removing anything to the project
	if (CanDirtyProject() != S_OK)
		return OLE_E_PROMPTSAVECANCELLED;

	CDropMoveHelper moveHelper;
	moveHelper.m_rgMovedNodes = new CVCNode*[numFiles];
	RETURN_ON_NULL2(moveHelper.m_rgMovedNodes, E_OUTOFMEMORY);

	int numActualFiles = 0;
	for (int i = 0; i < numFiles; i++)
	{
		CVCNode* pNode = rgNodes[i];

		if(pNode == NULL || pNode->IsZombie())
		{	// Can't find source node.
			CStringW strErr;
			CComBSTR bstrName;
			if (pNode)
				pNode->GetName(&bstrName);
			else
				bstrName = L"";
			CString strTmp = bstrName;
			strErr.Format(IDS_E_MISSINGSOURCENODE, strTmp);
			CComBSTR bstrErr = strErr;
			ReportError(bstrErr, E_FAIL);
			bReported = TRUE;
			return E_FAIL;	// caller must honor bReported and return a success code to VS since VS report error requires 
							// that we not return an error code in this case...
		}

		// This one's OK
		VSASSERT(pNode->GetKindOf() == Type_CVCFile || pNode->GetKindOf() == Type_CVCFileGroup, "Wrong node type in CVCProjectNode::OnDropMove" );
		moveHelper.m_rgMovedNodes[numActualFiles]= pNode;
		// Adjust count of files
		numActualFiles++;
		hr = S_OK;
	}

	if (numActualFiles)
		hr = DoInternalMove(pDropNode, numActualFiles, moveHelper.m_rgMovedNodes, bDrag, bReported);

	return hr;
}

//-----------------------------------------------------------------------------
// Called on a drop operation. rDataObject contains the dragdrop format from 
// the source. Also passed in is pOurDataSource. This is our datasource if we 
// sourced the drag or clipboard operation. If it is non-null, we can assume that
// the source is us and we can use the information in pOurDataSource to complete
// the move. 
//-----------------------------------------------------------------------------
HRESULT CVCProjectNode::OnDropCopy(CVCOleDataObject& rDataObject, CVCNode* pDropNode, CVCProjDataSource* pOurDataSource,
	BOOL bCrossVC, BOOL bDrag, BOOL& bReported)
{
	// Is this internal? ie is pOurDataSource != NULL?
	if (pOurDataSource)
	{
		CComBSTR bstrErr;
		bstrErr.LoadString(IDS_E_COPY_IN_SAME_PROJ);		// don't allow for copy within the same project
		ReportError(bstrErr);
		bReported = TRUE;
		return E_FAIL;	// caller needs to honor bReported and return a success code since VS requires we return a 
						// non-error code if we handled the error
	}

	RETURN_ON_NULL2(pDropNode, S_FALSE);

	// check out the project file before adding/removing anything to the project
	if (CanDirtyProject() != S_OK)
		return OLE_E_PROMPTSAVECANCELLED;

	if (bCrossVC)
		return DoCrossVCMoveOrCopy(rDataObject, pDropNode, false /* not move */, bDrag, bReported);

	return DoNonVCCopy(rDataObject, pDropNode, bDrag, bReported);
}

//-----------------------------------------------------------------------------
// Returns a CString array populated with the files from a PROJREF drop. Note that 
// we can't use the system's DragQueryFile() functions because they will NOT work 
// on win9x with unicode strings. Returns the count of files. The format looks like 
// the following: DROPFILES structure with pFiles member containing the offset to 
// the list of files:
//	 ----------------------------------------------------------------------------
//	|{DROPFILES structure}|ProjRefItem1|0|ProjRefItem2|0|.......|ProjRefItemN|0|0|
//	 ----------------------------------------------------------------------------
//
// If index == -1 it returns everything otherwise it just returns the one file at index (0 based) and
// returns a count of 1. Count of 0 means it couldn't find the one at the index requested.
//-----------------------------------------------------------------------------
int UtilGetFilesFromPROJITEMDrop(HGLOBAL h, CVCStringWArray& rgFiles, int index)
{
	int numFiles = 0;
	DROPFILES* pszDropFiles = reinterpret_cast<DROPFILES*>(::GlobalLock(h));
	ASSERT(pszDropFiles);
	if(pszDropFiles)
	{	// It better be marked unicode
		ASSERT(pszDropFiles->fWide);
		// The first member of the structure contains the offset to the files
		WCHAR* wzBuffer = reinterpret_cast<WCHAR*>(((BYTE*)pszDropFiles) + pszDropFiles->pFiles);
		// We go until *wzBuffer is null since we don't allow empty strings.
		while(*wzBuffer)
		{
			if(index == -1)
			{
				rgFiles.Add(wzBuffer);
				ASSERT(!rgFiles[numFiles].IsEmpty());
				wzBuffer += rgFiles[numFiles].GetLength()+1;
				numFiles++;
			}
			else
			{
				if(index == numFiles)
				{	// Just return the one asked for and return a count of 1.
					rgFiles.Add(wzBuffer);
					return 1;
				}
				numFiles++;

				// Skip to the next one.
				while(*wzBuffer++);
			}
		}
		::GlobalUnlock(h);
	}
	// if we didn't find the one requested 
	if(index != -1)
		numFiles = 0;
	return numFiles;
}

HRESULT CVCProjectNode::BuildProjItemList(COnDropHelper& dropHelper, CVCOleDataObject& rDataObject, BOOL bCrossVC)
{
	CComPtr<IVsSolution> spSolution;
	GetBuildPkg()->GetIVsSolution( &spSolution );
	VSASSERT(spSolution, "Unable to get IVsSolution!" );

	dropHelper.m_hItemDropInfo = (HDROP)rDataObject.GetGlobalData(CVCProjDataSource::s_cfRefProjItems);
	if (dropHelper.m_hItemDropInfo == NULL)
		dropHelper.m_hItemDropInfo = (HDROP)rDataObject.GetGlobalData(CVCProjDataSource::s_cfStgProjItems);
	RETURN_ON_NULL2(dropHelper.m_hItemDropInfo, S_OK);

	// Walk the list of files/folders
	dropHelper.m_numActualFiles = 0;
	CVCStringWArray rgFiles;
	dropHelper.m_numFiles = UtilGetFilesFromPROJITEMDrop(dropHelper.m_hItemDropInfo, rgFiles, -1);
	if (dropHelper.m_numFiles == 0)
		return S_OK;

	// Get the first item to get the source hierarchy.
	CComPtr<IVsHierarchy> spSrcHierarchy;
	VSITEMID itemidLoc;
	CStringW strMoniker;
	strMoniker = rgFiles.GetAt(0);
	HRESULT hrT = spSolution->GetItemOfProjref(strMoniker, &spSrcHierarchy, &itemidLoc, NULL, NULL);
	RETURN_ON_FAIL(hrT);

	// It better NOT have come from us!!
	// and, yes, that GetIVsHierarchy() handles the VCQI_cast
	VSASSERT(spSrcHierarchy && spSrcHierarchy != GetHierarchy()->GetIVsHierarchy(), "Bad hierarchy in CVCProjectNode::BuildProjItemList" );

	dropHelper.m_rgSrcHierarchies = new IVsHierarchy*[dropHelper.m_numFiles];
	RETURN_ON_NULL2(dropHelper.m_rgSrcHierarchies, E_OUTOFMEMORY);
	memset(dropHelper.m_rgSrcHierarchies, 0, dropHelper.m_numFiles*sizeof(IVsHierarchy*));
	dropHelper.m_rgSrcItemIDs = new VSITEMID[dropHelper.m_numFiles];
	RETURN_ON_NULL2(dropHelper.m_rgSrcItemIDs, E_OUTOFMEMORY);
	memset(dropHelper.m_rgSrcItemIDs, 0, dropHelper.m_numFiles*sizeof(VSITEMID));

	for (int i = 0; i < dropHelper.m_numFiles; i++)
	{	// Add each item (file or folder)
		CComPtr<IVsHierarchy> spHier;
		strMoniker = rgFiles.GetAt(i);
		if (FAILED(spSolution->GetItemOfProjref(strMoniker, &spHier, &itemidLoc, NULL, NULL)))
			break;

		if ((spHier == NULL) || (itemidLoc == VSITEMID_NIL))
			continue;

		dropHelper.m_rgSrcItemIDs[dropHelper.m_numActualFiles] = itemidLoc;
		dropHelper.m_rgSrcHierarchies[dropHelper.m_numActualFiles] = spHier.Detach();
		dropHelper.m_numActualFiles++;
	}

	return S_OK;
}

HRESULT CVCProjectNode::DoInternalMove(CVCNode* pTgtNode, DWORD cItems, CVCNode* rgMovedNodes[], BOOL bDrag, BOOL& bReported)
{
	// already checked for ability to dirty project file and that pTgtNode is non-NULL
	CVCWaitCursor waitCur;
	CComQIPtr<IDispatch> spParent = pTgtNode->m_dispkeyItem;

	HRESULT hr = S_FALSE;
	VARIANT_BOOL bOK = VARIANT_TRUE;
	DWORD idx = 0;
	BOOL bAnyMove = FALSE;
	for (idx = 0; idx < cItems && SUCCEEDED(hr) && bOK; idx++)
	{
		CVCNode* pNode = rgMovedNodes[idx];
		CComQIPtr<IDispatch> spItem = pNode->m_dispkeyItem;
		if (spItem == NULL)
			continue;

		CComQIPtr<VCFile> spThisFile = spItem;
		CComQIPtr<VCFilter> spThisFilter = spItem;
		if (spThisFile != NULL)
			hr = spThisFile->CanMove(spParent, &bOK);
		else if (spThisFilter != NULL)
			hr = spThisFilter->CanMove(spParent, &bOK);

		if (hr == S_OK)
			bAnyMove = TRUE;
		else if (FAILED(hr))
			return ReportMoveError(hr, spItem, spParent, bDrag, bReported); // not allowed to move at least one
		else if (!bOK)
			return ReportMoveError(E_FAIL, spItem, spParent, bDrag, bReported); // not allowed to move at least one
	}

	if (!bAnyMove)	// all potential moves are no-ops
	{
		bReported = false;
		for (idx = 0; idx < cItems && !bReported; idx++)
		{
			CVCNode* pNode = rgMovedNodes[idx];
			CComQIPtr<IDispatch> spItem = pNode->m_dispkeyItem;
			if (spItem == NULL)
				continue;

			CComBSTR bstrName;
			CComQIPtr<VCFile> spFile = spItem;
			if (spFile != NULL)
				hr = spFile->get_Name(&bstrName);
			else
			{
				CComQIPtr<VCFilter> spFilter = spItem;
				if (spFilter != NULL)
					spFilter->get_Name(&bstrName);
				else
					continue;
			}

			bReported = true;
			CStringW strErr;
			strErr.Format(IDS_E_SAME_SOURCE_DEST, bstrName);
			CComBSTR bstrErr = strErr;
			ReportError(bstrErr);
		}
		if (bReported)
			return E_FAIL;	// caller needs to know we reported it, but failed
		return S_FALSE;
	}

	bAnyMove = FALSE;
	hr = S_FALSE;
	for (idx = 0; idx < cItems && SUCCEEDED(hr); idx++)
	{
		CVCNode* pNode = rgMovedNodes[idx];
		CComQIPtr<IDispatch> spItem = pNode->m_dispkeyItem;
		if (spItem == NULL)
			continue;

		CComQIPtr<VCFile> spThisFile = spItem;
		CComQIPtr<VCFilter> spThisFilter = spItem;
		if (spThisFile)
			hr = spThisFile->Move(spParent);
		else if (spThisFilter)
			hr = spThisFilter->Move(spParent);
		if (hr == S_OK)
			bAnyMove = TRUE;
		else if (FAILED(hr))
			return ReportMoveError(hr, spItem, spParent, bDrag, bReported);
	}

	if (bAnyMove)
		return S_OK;
	else
		return S_FALSE;
}

/* static */
BOOL CVCProjectNode::VsHandlesError(HRESULT hrReport)
{
	return (hrReport == E_OUTOFMEMORY);
}

HRESULT CVCProjectNode::ReportMoveError(HRESULT hrReport, IDispatch* pItem, IDispatch* pParent, BOOL bDrag, BOOL& bReported)
{
	if (VsHandlesError(hrReport))
		return hrReport;	// let VS handle the error message

	bReported = TRUE;		// anything left, we're reporting here

	long errID;
	int cParams = 1;
	CStringW strParam1;
	CStringW strParam2;
	CStringW strItemName;
	GetIdentifierString(pItem, strItemName);
	switch(hrReport)
	{
	case E_ACCESSDENIED:
		errID = IDS_E_READ_ONLY_PROJ_FILE;
		GetProjectNameString(pParent, strParam1);
		break;

	case VCPROJ_E_BAD_PATH:
		errID = IDS_BAD_PATH_PARENT;
		strParam1 = strItemName;
		break;

	case VCPROJ_E_BAD_PARENT:
		errID = bDrag ? IDS_BAD_DROP_PARENT : IDS_BAD_PASTE_PARENT;
		strParam1 = strItemName;
		GetIdentifierString(pParent, strParam2);
		cParams = 2;
		break;

	case VCPROJ_E_FILTER_EXISTS:
		errID = IDS_FILTER_EXISTS;
		strParam1 = strItemName;
		GetIdentifierString(pParent, strParam2);
		cParams = 2;
		break;

	// case VCPROJ_E_NULL_PATH:		// not likely case since we'd better not have a NULL ID parent in the hierarchy!
	// case E_UNEXPECTED:
	// case VCPROJ_E_DIFF_PROJ:		// not likely as we've already ascertained that we're dropping on the same project!
	default:
		errID = bDrag ? IDS_ERR_DROP : IDS_ERR_PASTE;
		strParam1 = strItemName;
		GetIdentifierString(pParent, strParam2);
		cParams = 2;
		break;
	}

	CStringW strErr;
	if (cParams == 1)
		strErr.Format(errID, strParam1);
	else
	{
		VSASSERT(cParams == 2, "Hey, wrong number of params to format!");
		strErr.Format(errID, strParam1, strParam2);
	}

	CComBSTR bstrErr = strErr;
	ReportError(bstrErr, hrReport);
	return hrReport;	// caller must honor bReported later and return a success code; VS requires that we return a 
						// non-error code if we handled the error
}

HRESULT CVCProjectNode::ReportVCCopyError(HRESULT hrReport, VCProject* pProjectParent, VCFilter* pFilterParent, 
	IDispatch* pItem, bool bIsMove, BOOL bDrag, BOOL& bReported)
{
	if (VsHandlesError(hrReport))
		return hrReport;	// let VS handle the error message

	CComPtr<IDispatch> spParent = pProjectParent;
	if (spParent == NULL)
		spParent = pFilterParent;
	if (bIsMove)
		return ReportMoveError(hrReport, pItem, spParent, bDrag, bReported);
	
	CStringW strItemName;
	GetIdentifierString(pItem, strItemName);
	CStringW strParentName;
	GetIdentifierString(spParent, strParentName);
	return ReportCopyError(hrReport, spParent, strItemName, strParentName, bDrag, bReported);
}

/* static */
HRESULT CVCProjectNode::ReportCopyError(HRESULT hrReport, IDispatch* pParent, CStringW& strItemName, CStringW& strParentName, 
	BOOL bDrag, BOOL& bReported)
{
	if (VsHandlesError(hrReport))
		return hrReport;	// let VS handle the error message

	bReported = TRUE;	// let caller know we handled the error reporting

	long nErrID;
	CStringW strParam1, strParam2;
	int cParams = 1;
	switch (hrReport)
	{
	case E_INVALIDARG:
		nErrID = IDS_E_INVALID_NAME;
		strParam1 = strItemName;
		break;

	case VCPROJ_E_ZOMBIE:
		nErrID = IDS_E_ZOMBIE;
		strParam1 = strParentName;
		break;

	case E_ACCESSDENIED:
		nErrID = IDS_E_READ_ONLY_PROJ_FILE;
		GetProjectNameString(pParent, strParam1);
		break;

	case VCPROJ_E_FILE_EXISTS:
		nErrID = IDS_E_FILE_ALREADY_EXISTS;
		strParam1 = strItemName;
		GetProjectNameString(pParent, strParam2);
		cParams = 2;
		break;

	case VCPROJ_E_FILTER_EXISTS:
		nErrID = IDS_FILTER_EXISTS;
		strParam1 = strItemName;
		strParam2 = strParentName;
		cParams = 2;
		break;

	default:
//	case E_UNEXPECTED:
//	case E_OUTOFMEMORY:
		nErrID = bDrag ? IDS_ERR_DROP : IDS_ERR_PASTE;
		strParam1 = strItemName;
		strParam2 = strParentName;
		cParams = 2;
		break;
	}

	CStringW strErr;
	if (cParams == 1)
		strErr.Format(nErrID, strParam1);
	else
	{
		VSASSERT(cParams == 2, "Hey, wrong number of params to format!");
		strErr.Format(nErrID, strParam1, strParam2);
	}

	CComBSTR bstrErr = strErr;
	ReportError(bstrErr, hrReport);
	return hrReport;	// caller must honor bReported later and return a success code; VS requires that we return a 
						// non-error code if we handled the error
}

HRESULT CVCProjectNode::ReportRemoveError(HRESULT hrReport, IDispatch* pItem, BOOL& bReported)
{
	if (VsHandlesError(hrReport))
		return hrReport;	// let VS handle the error message

	bReported = TRUE;	// we're handling the error message

	CComQIPtr<VCFile> spFile = pItem;
	CComQIPtr<VCFilter> spFilter = pItem;
	CComPtr<IDispatch> spParent;
	if (spFile)
		spFile->get_Parent(&spParent);
	else if (spFilter)
		spFilter->get_Parent(&spParent);

	CStringW strItemName, strParentName;
	GetIdentifierString(pItem, strItemName);
	GetIdentifierString(spParent, strParentName);

	long errID;
	int cParams = 2;
	CStringW strParam1, strParam2;

	switch (hrReport)
	{
	case VCPROJ_E_NOT_FOUND:
		errID = IDS_E_REMOVE_NOT_FOUND;
		strParam1 = strItemName;
		strParam2 = strParentName;
		break;

	case E_ACCESSDENIED:
		errID = IDS_E_READ_ONLY_PROJ_FILE;
		GetProjectNameString(spParent, strParam1);
		cParams = 1;
		break;

	default:
	//case E_UNEXPECTED:
	//case E_NOINTERFACE:
		errID = IDS_E_NO_REMOVE;
		strParam1 = strItemName;
		strParam2 = strParentName;
	}

	CStringW strErr;
	if (cParams == 1)
		strErr.Format(errID, strParam1);
	else
	{
		VSASSERT(cParams == 2, "Hey, wrong number of params to format!");
		strErr.Format(errID, strParam1, strParam2);
	}

	CComBSTR bstrErr = strErr;
	ReportError(bstrErr, hrReport);
	return hrReport;	// caller must honor bReported and return a success code later; VS requires that we return a 
						// non-error code if we handled the error
}

HRESULT CVCProjectNode::ReportVsCopyError(HRESULT hrReport, VCProject* pProjectParent, VCFilter* pFilterParent, 
	IVsHierarchy* pHier, VSITEMID itemID, BOOL bDrag, BOOL& bReported)
{
	if (VsHandlesError(hrReport))
		return hrReport;	// let VS handle the error message

	CComQIPtr<IDispatch> spParent = pProjectParent;
	if (spParent == NULL)
		spParent = pFilterParent;
	CStringW strParentName;
	GetIdentifierString(spParent, strParentName);

	CStringW strItemName;
	if (itemID != VSITEMID_NIL && pHier)
	{
		CComBSTR bstrItemName;
		pHier->GetCanonicalName(itemID, &bstrItemName);
		CStringW str1 = bstrItemName;
		strItemName.Format(IDS_ITEM, str1);
	}
	else
		strItemName.LoadString(IDS_UNKNOWN_ITEM);

	return ReportCopyError(hrReport, spParent, strItemName, strParentName, bDrag, bReported);
}

/* static */ 
HRESULT CVCProjectNode::ReportShellCopyError(HRESULT hrReport, VCProject* pProject, VCFilter* pFilter, LPWSTR wstrFile,
	BOOL& bReported)
{
	if (VsHandlesError(hrReport))
		return hrReport;	// let VS handle the error message

	CComQIPtr<IDispatch> spParent = pProject;
	if (spParent == NULL)
		spParent = pFilter;

	CStringW strParentName;
	GetIdentifierString(spParent, strParentName);

	CStringW strTmp = wstrFile;
	CStringW strItemName;
	strItemName.Format(IDS_FILE, strTmp);

	return ReportCopyError(hrReport, spParent, strItemName, strParentName, TRUE, bReported);
}

/* static */
void CVCProjectNode::GetIdentifierString(IDispatch* pItem, CStringW& strItemString)
{
	CStringW str;
	CComBSTR bstr;

	CComQIPtr<VCFile> spFile = pItem;
	if (spFile)
	{
		spFile->get_FullPath(&bstr);
		str = bstr;
		strItemString.Format(IDS_FILE, str);
		return;
	}

	CComQIPtr<VCFilter> spFilter = pItem;
	if (spFilter)
	{
		spFilter->get_Name(&bstr);
		str = bstr;
		strItemString.Format(IDS_FILTER, str);
		return;
	}

	CComQIPtr<VCProject> spProject = pItem;
	if (spProject)
	{
		spProject->get_Name(&bstr);
		str = bstr;
		strItemString.Format(IDS_PROJECT, str);
		return;
	}

	strItemString.LoadString(IDS_UNKNOWN_ITEM);
}

/* static */
void CVCProjectNode::GetProjectNameString(IDispatch* pItem, CStringW& strProj)
{
	CComQIPtr<VCProject> spProject = pItem;
	CComBSTR bstr;
	if (spProject)
	{
		spProject->get_Name(&bstr);
		strProj = bstr;
		return;
	}

	CComPtr<IDispatch> spDispProject;
	CComQIPtr<VCProjectItem> spProjItem = pItem;
	if (spProjItem)
		spProjItem->get_Project(&spDispProject);
	spProject = spDispProject;
	if (spProject)
		spProject->get_Name(&bstr);
	strProj = bstr;
}

BOOL CVCProjectNode::MatchesHierarchy(IVsHierarchy* pIVsHierarchy)
{
	CComPtr<IVsHierarchy> spHier;
	HRESULT hr = GetVsHierarchy(&spHier, TRUE);
	RETURN_ON_FAIL2(hr, FALSE);

	return (pIVsHierarchy == spHier);
}

void CVCProjectNode::GetVsItemNameAndType(IVsHierarchy* pHier, VSITEMID itemID, BSTR* pbstrName, BOOL& bIsFile, BOOL& bIsFilter)
{
	*pbstrName = NULL;	// BETTER be valid pointer since this is internal...
	bIsFile = FALSE;
	bIsFilter = FALSE;
	if (pHier == NULL || itemID == VSITEMID_NIL)
		return;

	if (FAILED(pHier->GetCanonicalName(itemID, pbstrName)))
		return;

	GUID guid;
	if (FAILED(pHier->GetGuidProperty(itemID, VSHPROPID_TypeGuid, &guid)))	// if this fails, probably Misc Files project
	{
		bIsFile = TRUE;
		bIsFilter = FALSE;
		return;
	}

	bIsFile = IsEqualGUID(guid, GUID_ItemType_PhysicalFile);
	bIsFilter = (IsEqualGUID(guid, GUID_ItemType_PhysicalFolder) || IsEqualGUID(guid, GUID_ItemType_VirtualFolder));
}

HRESULT CVCProjectNode::CanCopyVsElement(VCProject* pProjectParent, VCFilter* pFilterParent, IVsHierarchy* pHier, VSITEMID itemID, 
	bool bIsTopLevel, BOOL bDrag, BOOL& bReported)
{
	CComBSTR bstrName;
	BOOL bIsFile = FALSE;
	BOOL bIsFilter = FALSE;
	GetVsItemNameAndType(pHier, itemID, &bstrName, bIsFile, bIsFilter);
	if (!bIsFile && !bIsFilter)
		return S_FALSE; // neither filter nor file, so ignore it

	HRESULT hr = S_FALSE;

	CComQIPtr<IVCProjectImpl> spProjectParentImpl = pProjectParent;
	CComQIPtr<IVCFilterImpl> spFilterParentImpl = pFilterParent;
	if (bIsFile)
	{
		if (spProjectParentImpl)
			hr = spProjectParentImpl->CanAddFile2(bstrName);
		else if (spFilterParentImpl)
			hr = spFilterParentImpl->CanAddFile2(bstrName);
		else
			hr = E_FAIL;
		if (FAILED(hr))
			return ReportVsCopyError(hr, pProjectParent, pFilterParent, pHier, itemID, bDrag, bReported);
	}

	if (bIsTopLevel)	// only doing this for top level filter being copied
	{
		if (spProjectParentImpl)
			hr = spProjectParentImpl->CanAddFilter2(bstrName, VARIANT_FALSE /* no errorinfo */);
		else if (spFilterParentImpl)
			hr = spFilterParentImpl->CanAddFilter2(bstrName, VARIANT_FALSE /* no errorinfo */);
		else
			hr = E_FAIL;

		if (FAILED(hr))
			return ReportVsCopyError(hr, pProjectParent, pFilterParent, pHier, itemID, bDrag, bReported);
	}
	else
		hr = S_OK;

	// may have contents
	CComVariant varFirstChild;
	hr = pHier->GetProperty(itemID, VSHPROPID_FirstChild, &varFirstChild);
	VSASSERT(varFirstChild.vt == VT_I4 || varFirstChild.vt == VT_UI4, "Unexpected variant type");
	VSITEMID itemidChild = varFirstChild.lVal;
	while (itemidChild != 0 && itemidChild != VSITEMID_NIL && SUCCEEDED(hr))
	{
		hr = CanCopyVsElement(pProjectParent, pFilterParent, pHier, itemidChild, false, bDrag, bReported);
		if (SUCCEEDED(hr))
		{
			CComVariant varChild;
			hr = pHier->GetProperty(itemidChild, VSHPROPID_NextSibling, &varChild);
			VSASSERT(varChild.vt == VT_I4 || varChild.vt == VT_UI4, "Unexpected variant type");
			itemidChild = varChild.lVal;
		}
	}

	return hr;
}

HRESULT CVCProjectNode::AddVsElement(VCProject* pProjectParent, VCFilter* pFilterParent, IVsHierarchy* pHier, VSITEMID itemID,
	BOOL bDrag, BOOL& bReported)
{
	CComBSTR bstrName;
	BOOL bIsFile = FALSE;
	BOOL bIsFilter = FALSE;
	GetVsItemNameAndType(pHier, itemID, &bstrName, bIsFile, bIsFilter);
	if (!bIsFile && !bIsFilter)
		return S_FALSE; // neither filter nor file, so ignore it

	HRESULT hr = S_FALSE;

	CComPtr<IDispatch> spOut;

	if (bIsFile)
	{
		if (pProjectParent)
			hr = pProjectParent->AddFile(bstrName, &spOut);
		else
			hr = pFilterParent->AddFile(bstrName, &spOut);
		if (FAILED(hr))
			return ReportVsCopyError(hr, pProjectParent, pFilterParent, pHier, itemID, bDrag, bReported);
		return hr;
	}

	// pick off the last directory name if what we have is a full path
	CPathW pathFilter;
	CStringW strName = bstrName;
	int nLastFwdSlash = strName.ReverseFind(L'/');
	int nLastBackSlash = strName.ReverseFind(L'\\');
	int nLen = strName.GetLength();
	if (nLastFwdSlash == nLen-1 || nLastBackSlash == nLen-1)
	{
		strName = strName.Left(nLen-1);		// kill any trailing slash
		nLen--;
	}
	if (pathFilter.Create(strName))
	{
		pathFilter.GetActualCase(TRUE);	// need to make it proper case per file system
		strName = pathFilter;
		nLen = strName.GetLength(); // length can change due to beautification
	}
	nLastFwdSlash = strName.ReverseFind(L'/');
	nLastBackSlash = strName.ReverseFind(L'\\');
	int nLastSlash = -1;
	if (nLastFwdSlash > nLastBackSlash)
		nLastSlash = nLastFwdSlash;
	else if (nLastBackSlash > nLastFwdSlash)
		nLastSlash = nLastBackSlash;
	if (nLastSlash >= 0)
		strName = strName.Right(nLen-nLastSlash-1);
	bstrName = strName;

	if (pProjectParent)
		hr = pProjectParent->AddFilter(bstrName, &spOut);
	else
		hr = pFilterParent->AddFilter(bstrName, &spOut);
	if (FAILED(hr))
		return ReportVsCopyError(hr, pProjectParent, pFilterParent, pHier, itemID, bDrag, bReported);

	CComQIPtr<VCFilter> spNewFilterParent = spOut;
	RETURN_ON_NULL2(spNewFilterParent, E_UNEXPECTED);

		// may have contents
	CComVariant varFirstChild;
	hr = pHier->GetProperty(itemID, VSHPROPID_FirstChild, &varFirstChild);
	VSASSERT(varFirstChild.vt == VT_I4 || varFirstChild.vt == VT_UI4, "Unexpected variant type");
	VSITEMID itemidChild = varFirstChild.lVal;
	while (itemidChild != 0 && itemidChild != VSITEMID_NIL && SUCCEEDED(hr))
	{
		hr = AddVsElement(NULL, spNewFilterParent, pHier, itemidChild, bDrag, bReported);
		if (SUCCEEDED(hr))
		{
			CComVariant varChild;
			hr = pHier->GetProperty(itemidChild, VSHPROPID_NextSibling, &varChild);
			VSASSERT(varChild.vt == VT_I4 || varChild.vt == VT_UI4, "Unexpected variant type");
			itemidChild = varChild.lVal;
		}
	}

	return hr;
}

HRESULT CVCProjectNode::DoNonVCCopy(CVCOleDataObject& rDataObject, CVCNode* pTgtNode, BOOL bDrag, BOOL& bReported)
{
	// already checked for ability to dirty project file and that pTgtNode is non-NULL
	COnDropHelper dropHelper;
	HRESULT hr = BuildProjItemList(dropHelper, rDataObject, FALSE);
	RETURN_ON_FAIL_OR_NULL2(hr, dropHelper.m_numActualFiles, S_FALSE);

	CComQIPtr<IDispatch> spParent = pTgtNode->m_dispkeyItem;
	CComQIPtr<VCProject> spProjectParent = spParent;
	CComQIPtr<VCFilter> spFilterParent = spParent;
	if (spProjectParent == NULL && spFilterParent == NULL)
		return E_UNEXPECTED;

	int idx = 0;
	bool bAnyMove = false;
	for (idx = 0; idx < dropHelper.m_numActualFiles && SUCCEEDED(hr); idx++)
	{
		IVsHierarchy* pHier = dropHelper.m_rgSrcHierarchies[idx];
		VSITEMID itemID = dropHelper.m_rgSrcItemIDs[idx];
		hr = CanCopyVsElement(spProjectParent, spFilterParent, pHier, itemID, true, bDrag, bReported);
		if (hr == S_OK)
			bAnyMove = true;
	}

	RETURN_ON_FAIL(hr);	// error reported in CanCopyVsElement
	if (!bAnyMove)
		return S_OK;	// nothing to do

	bAnyMove = false;
	for (idx = 0; idx < dropHelper.m_numActualFiles && SUCCEEDED(hr); idx++)
	{
		IVsHierarchy* pHier = dropHelper.m_rgSrcHierarchies[idx];
		VSITEMID itemID = dropHelper.m_rgSrcItemIDs[idx];
		hr = AddVsElement(spProjectParent, spFilterParent, pHier, itemID, bDrag, bReported);
		if (hr == S_OK)
			bAnyMove = true;
	}

	RETURN_ON_FAIL(hr);	// error reported in AddVsElement

	return S_OK;	// override any S_FALSE we might otherwise have seen
}

void CVCProjectNode::GetVCItemFromVsItem(IVsHierarchy* pHier, VSITEMID itemID, IDispatch** ppVCItem)
{
	*ppVCItem = NULL;	// BETTER be valid pointer since this is internal...

	if (pHier == NULL || itemID == VSITEMID_NIL)
		return;

	CComVariant var;
	pHier->GetProperty(itemID, VSHPROPID_ExtObject, &var);
	if (var.vt == VT_EMPTY)
		return;

	CComQIPtr<_ProjectItem> spVsItem = var.pdispVal;
	if (spVsItem == NULL)
		return;

	spVsItem->get_Object(ppVCItem);
}

HRESULT CVCProjectNode::DoCrossVCMoveOrCopy(CVCOleDataObject& rDataObject, CVCNode* pTgtNode, bool bIsMove, BOOL bDrag, 
	BOOL& bReported)
{
	// already checked for ability to dirty project file and that pTgtNode is non-NULL
	CVCWaitCursor waitCur;

	COnDropHelper dropHelper;
	HRESULT hr = BuildProjItemList(dropHelper, rDataObject, TRUE);
	RETURN_ON_FAIL_OR_NULL2(hr, dropHelper.m_numActualFiles, S_FALSE);

	CComQIPtr<IDispatch> spParent = pTgtNode->m_dispkeyItem;
	CComQIPtr<VCProject> spProjectParent = spParent;
	CComQIPtr<VCFilter> spFilterParent = spParent;
	if (spProjectParent == NULL && spFilterParent == NULL)
		return E_UNEXPECTED;

	int idx = 0;
	// don't check to see if we are allowed to move/copy here, the AddElement will
	// do the check itself
	for (idx = 0; idx < dropHelper.m_numActualFiles && SUCCEEDED(hr); idx++)
	{
		IVsHierarchy* pHier = dropHelper.m_rgSrcHierarchies[idx];
		VSITEMID itemID = dropHelper.m_rgSrcItemIDs[idx];
		CComPtr<IDispatch> spItem;
		GetVCItemFromVsItem(pHier, itemID, &spItem);
		if (spItem == NULL) // ignore any empty elements
			continue;

		hr = AddElement(spProjectParent, spFilterParent, spItem, bIsMove, bDrag, bReported);
	}

	// any errors reported in AddElement and/or RemoveElement
	return hr;
}

HRESULT CVCProjectNode::CanMoveOrCopyElement(VCProject* pProjectParent, VCFilter* pFilterParent, IDispatch* pItem, bool bTopLevel, 
	bool bIsMove, BOOL bDrag, BOOL& bReported)
{
	HRESULT hr = E_UNEXPECTED;	// assume neither file or filter

	CComBSTR bstrName;
	CComQIPtr<VCFile> spFile = pItem;
	CComQIPtr<IVCProjectImpl> spProjectParentImpl = pProjectParent;
	CComQIPtr<IVCFilterImpl> spFilterParentImpl = pFilterParent;
	if (spFile)
	{
		spFile->get_FullPath(&bstrName);
		if (spProjectParentImpl)
			hr = spProjectParentImpl->CanAddFile2(bstrName);
		else if (spFilterParentImpl)
			hr = spFilterParentImpl->CanAddFile2(bstrName);
		else
			hr = E_FAIL;

		if (FAILED(hr))
			return ReportVCCopyError(hr, pProjectParent, pFilterParent, pItem, bIsMove, bDrag, bReported);
	}

	CComQIPtr<VCFilter> spFilter = pItem;
	if (spFilter)
	{
		if (bTopLevel)	// only doing this for top level filter being moved
		{
			spFilter->get_Name(&bstrName);
			if (spProjectParentImpl)
				hr = spProjectParentImpl->CanAddFilter2(bstrName, VARIANT_FALSE /* no errorinfo */);
			else if (spFilterParentImpl)
				hr = spFilterParentImpl->CanAddFilter2(bstrName, VARIANT_FALSE /* no errorinfo */);
			else
				hr = E_FAIL;
			if (FAILED(hr))
				return ReportVCCopyError(hr, pProjectParent, pFilterParent, pItem, bIsMove, bDrag, bReported);
		}
		else
			hr = S_OK;

		// may have contents
		CComPtr<IDispatch> spDispFiles;
		spFilter->get_Files(&spDispFiles);
		hr = CanAddElementCollection(pProjectParent, pFilterParent, spDispFiles, bDrag, bReported);
		RETURN_ON_FAIL_OR_NULL(hr, spFilter);
		CComPtr<IDispatch> spDispFilters;
		spFilter->get_Filters(&spDispFilters);
		hr = CanAddElementCollection(pProjectParent, pFilterParent, spDispFilters, bDrag, bReported);
		RETURN_ON_FAIL(hr);
	}

	if (spFilter == NULL && spFile == NULL)
		return ReportVCCopyError(E_UNEXPECTED, pProjectParent, pFilterParent, NULL, bIsMove, bDrag, bReported);

	if (bTopLevel && bIsMove)
	{
		// verify we can do a cut
		CComPtr<IDispatch> spDispProject;
		if (spFile)
			spFile->get_Project(&spDispProject);
		else	// spFilter
			spFilter->get_Project(&spDispProject);
		CComQIPtr<IVCProjectImpl> spOldProjectImpl = spDispProject;
		if (spOldProjectImpl == NULL)
			hr = E_UNEXPECTED;
		else
			hr = spOldProjectImpl->get_CanDirty(NULL);
		if (FAILED(hr))
			return ReportRemoveError(hr, pItem, bReported);
	}

	return hr;
}

HRESULT CVCProjectNode::CanAddElementCollection(VCProject* pProject, VCFilter* pFilter, IDispatch* pItemCollection, BOOL bDrag, 
	BOOL& bReported)
{
	HRESULT hr = S_OK;
	CComQIPtr<IVCCollection> spItems = pItemCollection;
	if (spItems)
	{
		CComPtr<IEnumVARIANT> spEnum;
		spItems->_NewEnum((IUnknown**)&spEnum);
		if (spEnum)
		{
			spEnum->Reset();
			while (SUCCEEDED(hr))
			{
				CComVariant var;
				hr = spEnum->Next(1, &var, NULL);
				if (hr != S_OK)
					break;
				if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
					continue;
				CComQIPtr<IDispatch> spDispItem = var.pdispVal;
				hr = CanMoveOrCopyElement(pProject, pFilter, spDispItem, false, false, bDrag, bReported);
			}
		}
	}

	if (hr == S_FALSE)
		hr = S_OK;
	return hr;
}

HRESULT CVCProjectNode::AddElement(VCProject* pProjectParent, VCFilter* pFilterParent, IDispatch* pItem, bool bIsMove, 
	BOOL bDrag, BOOL& bReported)
{
	HRESULT hr = S_OK;
	CComPtr<IDispatch> spOut;
	CComBSTR bstrName;
	CComQIPtr<VCFile> spFile = pItem;
	if (spFile)
	{
		spFile->get_FullPath(&bstrName);
		if (pProjectParent)
			hr = pProjectParent->AddFile(bstrName, &spOut);
		else
			hr = pFilterParent->AddFile(bstrName, &spOut);
		if (FAILED(hr))
			return ReportVCCopyError(hr, pProjectParent, pFilterParent, pItem, bIsMove, bDrag, bReported);
		return hr;
	}

	CComQIPtr<VCFilter> spFilter = pItem;
	if (spFilter)
	{
		spFilter->get_Name(&bstrName);
		if (pProjectParent)
			hr = pProjectParent->AddFilter(bstrName, &spOut);
		else
			hr = pFilterParent->AddFilter(bstrName, &spOut);
		if (FAILED(hr))
			return ReportVCCopyError(hr, pProjectParent, pFilterParent, pItem, bIsMove, bDrag, bReported);
		CComQIPtr<VCFilter> spNewFilterParent = spOut;
		RETURN_ON_NULL2(spNewFilterParent, E_UNEXPECTED);
		CComPtr<IDispatch> spFiles;
		spFilter->get_Files(&spFiles);
		hr = AddElementCollection(NULL, spNewFilterParent, spFiles, bDrag, bReported);
		RETURN_ON_FAIL(hr);
		CComPtr<IDispatch> spFilters;
		spFilter->get_Filters(&spFilters);
		return AddElementCollection(NULL, spNewFilterParent, spFilters, bDrag, bReported);
	}

	return ReportVCCopyError(E_UNEXPECTED, pProjectParent, pFilterParent, pItem, bIsMove, bDrag, bReported);	// neither file nor filter
}

HRESULT CVCProjectNode::AddElementCollection(VCProject* pProject, VCFilter* pFilter, IDispatch* pItemCollection, BOOL bDrag,
	BOOL& bReported)
{
	HRESULT hr = S_OK;
	CComQIPtr<IVCCollection> spItems = pItemCollection;
	if (spItems)
	{
		CComPtr<IEnumVARIANT> spEnum;
		spItems->_NewEnum((IUnknown**)&spEnum);
		if (spEnum)
		{
			spEnum->Reset();
			while (SUCCEEDED(hr))
			{
				CComVariant var;
				hr = spEnum->Next(1, &var, NULL);
				if (hr != S_OK)
					break;
				CComQIPtr<IDispatch> spDispItem = var.pdispVal;
				if (spDispItem == NULL)		// ignore any empty elements
					continue;
				hr = AddElement(pProject, pFilter, spDispItem, false, bDrag, bReported);
			}
		}
	}

	if (hr == S_FALSE)
		hr = S_OK;
	return hr;
}

//-----------------------------------------------------------------------------
// Execution context class. Allows program to set state about an execution through
// the use of key/value pairs. For example: the delete commaand could set the ctx
// IDYES for it's delete confirmation dialog. So when user checks "apply to all items"
// it won't prompt. The code can get back the ctx (in this case IDYES) and use
// that. The Start/EndSingleOp() method should be used at the start and end of an operation to
// flush out the context. MultiSelect handlers should use the start/end MultiOp 
// funcitons.
// The class provides a generic CANCELLED state since this applies across all items.
//-----------------------------------------------------------------------------
CVCExecutionCtx::CVCExecutionCtx()
  :
	m_bCancelled(FALSE),
	m_dwRef(0)
{
	m_exCtxArray.SetSize(0,5);
}

//----------------------------------------------------------------------------
// Cleans up object
//----------------------------------------------------------------------------
CVCExecutionCtx::~CVCExecutionCtx()
{
	Clear();
}

//----------------------------------------------------------------------------
// Clears all values from the ctx, including the cancelled status. 
//
//----------------------------------------------------------------------------
void CVCExecutionCtx::Clear()
{
	ClearData();
	m_dwRef = 0;
}

//----------------------------------------------------------------------------
// Clears all values from the ctx, including the cancelled status, but not ref count
//
//----------------------------------------------------------------------------
void CVCExecutionCtx::ClearData()
{
	for(int i=0; i< m_exCtxArray.GetSize(); i++)
	{
		CExCtx* pExCtx = m_exCtxArray[i];
		delete pExCtx;
	}
	m_exCtxArray.RemoveAll();
	m_bCancelled = FALSE;
}

//----------------------------------------------------------------------------
// Called to start an operation
//----------------------------------------------------------------------------
void CVCExecutionCtx::StartOp()
{
	if(m_dwRef == 0)
		Clear();
	++m_dwRef;
}
//----------------------------------------------------------------------------
// Called at end operation
//----------------------------------------------------------------------------
void CVCExecutionCtx::EndOp()
{
	--m_dwRef;
	VSASSERT(((int)m_dwRef) >= 0, "Bad refcount in CVCExecutionCtx::EndOp" );
	if(((int)m_dwRef) < 0)
		m_dwRef = 0;
	if(m_dwRef == 0)
		Clear();
}

//----------------------------------------------------------------------------
// Returns the value at key, or 0 if not found.
//----------------------------------------------------------------------------
DWORD CVCExecutionCtx::GetValue(DWORD key)
{
	CExCtx* pExCtx = GetExCtx(key);
	if(!pExCtx)
		return 0;
	return pExCtx->value;

}

//----------------------------------------------------------------------------
// Sets the value at key. If a value already exists, it is replaced.
//----------------------------------------------------------------------------
void CVCExecutionCtx::SetValue(DWORD key, DWORD value)
{
	CExCtx* pExCtx = GetExCtx(key);
	if(!pExCtx)
	{
		if(value)
		{
			pExCtx = new CExCtx;
			if( pExCtx )
			{
				pExCtx->key = key;
				m_exCtxArray.Add(pExCtx);
			}
		}
	}
	else
	{	// Setting a value to 0 effectively clears it.
		pExCtx->value = value;
	}

}

//----------------------------------------------------------------------------
// Searchs and returns the CExCtx structure for key.
//----------------------------------------------------------------------------
CVCExecutionCtx::CExCtx* CVCExecutionCtx::GetExCtx(DWORD key)
{
	for(int i=0; i< m_exCtxArray.GetSize(); i++)
	{
		CExCtx* pExCtx = m_exCtxArray[i];
		if(pExCtx->key == key)
			return pExCtx;
	}
	// Not found
	return NULL;
}

COnDropHelper::COnDropHelper() : m_hItemDropInfo(NULL), m_numFiles(0), m_rgSrcItemIDs(NULL), m_rgSrcHierarchies(NULL),
	m_numActualFiles(0) 
{}

COnDropHelper::~COnDropHelper()
{	// Free everything
	if (m_hItemDropInfo)
		::GlobalFree(m_hItemDropInfo);
	if (m_rgSrcHierarchies)
	{
		for (int idx = 0; idx < m_numFiles; idx++)
		{
			IVsHierarchy* pHier = m_rgSrcHierarchies[idx];
			if (pHier)
				pHier->Release();
		}
		delete [] m_rgSrcHierarchies;
	}
	if (m_rgSrcItemIDs)
		delete [] m_rgSrcItemIDs;
}
