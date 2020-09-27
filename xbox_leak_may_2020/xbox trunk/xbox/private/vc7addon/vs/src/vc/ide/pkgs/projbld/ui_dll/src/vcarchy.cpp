//---------------------------------------------------------------------------
// Microsoft Visual C++
//
// Microsoft Confidential
// Copyright 1994 - 1997 Microsoft Corporation. All Rights Reserved.
//
// VCArchy.cpp : Implementation of CVCArchy
//---------------------------------------------------------------------------

#include "stdafx.h"
#include "rsrc.h"
#include "bldpkg.h"
#include "VCArchy.h"
#include "vcfile.h"
#include <vcguid.h>
#include "vcnode.h"
#include "path2.h"
#include "gencfg.h"
#include "vcfgrp.h"
#include "vstrkdoc.h"
#include "..\resdll\gpcmd.h"
#include "vssolutn.h"
#include "VsCoCreate.h"
#include "DataObject.h"
#include "VCPropObject.h"
#include "..\..\vcpb\comlist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// using this variable to generate unique cookies (via incrementing)
VSCOOKIE CVCArchy::g_dwCookie = 0;

// did we start this drag or not
BOOL CVCArchy::s_bStartedDrag = FALSE;

CVCArchy::CVCArchy(void) :
	m_hImageList(NULL),
	m_pRootNode(NULL),
	m_pParentHierarchy(NULL),
	m_dwParentHierarchyItemid(VSITEMID_NIL),
	m_fRegisteredWithScc( false ),
	m_dropType(DT_NONE),
	m_bInternalDrop(FALSE),
	m_dwClipEvtCookie(0),
	m_pDragDropDataObject(NULL),
	m_pClipboardDataObject(NULL),
	m_bWasCut(FALSE),
	m_pCachedNode(NULL),
	m_nCachedIconType(0)
{
	if (GetBuildPkg())
		GetBuildPkg()->AddSolutionRef();	// want the solution around while any project is around

	// get a VsCfgProviderEventsHelper to send cfg events to the shell with
	CComPtr<ILocalRegistry> pLocalReg;
	HRESULT hr = ExternalQueryService( SID_SLocalRegistry, IID_ILocalRegistry, (void**)&pLocalReg );
	VSASSERT( SUCCEEDED( hr ) && pLocalReg, "Failed to QueryService for SLocalRegistry!  Could this be a threading issue or is it just a registry issue?" );
	if (pLocalReg)
	{
		hr = pLocalReg->CreateInstance( CLSID_VsCfgProviderEventsHelper, NULL, IID_IVsCfgProviderEventsHelper, CLSCTX_SERVER, (void**)&m_pCfgProviderEventsHelper );
		VSASSERT( SUCCEEDED( hr ) && m_pCfgProviderEventsHelper, "Failed to CreateInstance IVsCfgProviderEventsHelper!  Could this be a threading issue or is it just a registry issue?" );
	}
}

CVCArchy::~CVCArchy(void)
{
}

HRESULT CVCArchy::CanDirtyProject( void )
{
	// see if we are allowed to touch the project
	CComPtr<IDispatch> pProject;
	GetDispVCProject( NULL, &pProject );
	CComQIPtr<IVCProjectImpl> pProjImpl = pProject;
	RETURN_ON_NULL2(pProjImpl, E_NOINTERFACE);
	VARIANT_BOOL bCanDirty;
	if( pProjImpl->get_CanDirty( &bCanDirty ) != S_OK || bCanDirty == VARIANT_FALSE )
		return E_ACCESSDENIED;
	return S_OK;
}

UINT CVCArchy::GetIconIndex(CVCNode* pNode, ICON_TYPE iconType) 
{
	if (pNode == m_pCachedNode)	// caching node info just to make SCC perform faster
		return m_nCachedIconType;
	else
		return pNode->GetIconIndex( iconType ); // TODO: Make sure all pNodes have one of these.
}

//---------------------------------------------------------------------------
// interface: IVsProject
//---------------------------------------------------------------------------

STDMETHODIMP CVCArchy::IsDocumentInProject (
						LPCOLESTR pszMkDocument,
						BOOL *pfFound,
						VSDOCUMENTPRIORITY *pdwPriority,
						VSITEMID *pvsid)
{
	CComPtr<VCProject> pProj;
	GetVCProject( NULL, &pProj );
	CHECK_ZOMBIE(pProj, IDS_ERR_PROJ_ZOMBIE);
	CHECK_READ_POINTER_NULL(pszMkDocument);

	CComBSTR bstrProjectFile;
	pProj->get_ProjectFile( &bstrProjectFile );
	CPathW path;
	{
		CCurDirW dir(bstrProjectFile, TRUE);
		if( _wcsnicmp(pszMkDocument,L"file:///", 8) == 0 )
		{
			pszMkDocument = pszMkDocument + 8; // skip past the "file:///" prepend
		}

		BOOL b = path.Create(pszMkDocument);
		if( b == FALSE )
		{
			return E_FAIL;  // not a valid path
		}
	}

	path.GetActualCase(TRUE);

	CComQIPtr<IVCProjectImpl> pProjImp = pProj;
	RETURN_ON_NULL(pProjImp);

	if(pfFound)
		*pfFound = FALSE;

	VSITEMID vsid = VSITEMID_NIL;
	VARIANT_BOOL bIn = VARIANT_FALSE;
	pProjImp->IsFileProbablyInProject( (LPCOLESTR)path, &bIn );
	if( bIn == VARIANT_TRUE )
	{
    
	    // is it the project file??
	    CComBSTR bstrCannon;
	    bstrCannon = path;
	    VARIANT_BOOL bMatch;
	    if (SUCCEEDED(pProj->MatchName(bstrCannon, VARIANT_TRUE, &bMatch)) && bMatch == VARIANT_TRUE)
		    vsid = VSITEMID_ROOT;
	    else
		    vsid = DoItemIDFromName(NULL, VARIANT_TRUE, bstrCannon);
    
	    if (vsid != VSITEMID_NIL)
	    {
		    if(pfFound)
			    *pfFound = TRUE;
		    if(pdwPriority)
			    *pdwPriority = DP_Standard;
	    }
    
	}

	if(pvsid)
		*pvsid = vsid;

	return S_OK;
}

STDMETHODIMP CVCArchy::GetMkDocument (VSITEMID vsid, BSTR *pbstrMkDocument)
{
	CHECK_POINTER_NULL(pbstrMkDocument);

	CVCNode*	pNode = NULL;
	HRESULT hr = VSITEMID2Node(vsid, &pNode);
	CVCNode *pVcNode = static_cast<CVCNode*>(pNode);
	RETURN_ON_NULL(pVcNode);

	CComQIPtr<VCFile> pFile = pVcNode->m_dispkeyItem;
	if (pFile)
		return pFile->get_FullPath(pbstrMkDocument);

	CComQIPtr<VCProject> pProj = pVcNode->m_dispkeyItem;
	if (pProj)
		return pProj->get_ProjectFile(pbstrMkDocument);

	return E_FAIL;
}

STDMETHODIMP CVCArchy::OpenItem (VSITEMID vsid,
				   REFGUID guidDocViewType,
				   IUnknown *punkDocDataExisting,
				   IVsWindowFrame **ppWindowFrame)
{
	CVCNode		   *pNode = NULL;
	HRESULT			hr = VSITEMID2Node(vsid, &pNode);
	BOOL			fIsOpen = FALSE;
	CVCFileNode	   *pFile;
	VSITEMID		vsidDummy;

	CComPtr<IVsUIShellOpenDocument> pVsUIShellOpenDoc;	
	hr = ExternalQueryService(SID_SVsUIShellOpenDocument, IID_IVsUIShellOpenDocument, (void **)&pVsUIShellOpenDoc);
	VSASSERT(SUCCEEDED(hr), "Failed to obtain VSUIShellOpenDocument service.  Did the service ID change?  Are you in the wrong thread?");

	pFile = static_cast<CVCFileNode*>(pNode);
	RETURN_ON_NULL(pFile);

	CComBSTR bstrFile = pFile->GetFullPath();
	RETURN_ON_NULL(bstrFile);

	if (guidDocViewType != LOGVIEWID_UserChooseView)
	{
		CComPtr<IVsUIHierarchy> pUIHier;
		CComPtr<IVsUIHierarchy> spHier = VCQI_cast<IVsUIHierarchy>(this);
		hr = pVsUIShellOpenDoc->IsDocumentOpen(spHier, vsid, bstrFile, guidDocViewType,
			IDO_ActivateIfOpen, &pUIHier, &vsidDummy, ppWindowFrame, &fIsOpen);
		VSASSERT(SUCCEEDED(hr), "Unable to determine if a document is open.  Are you in the wrong thread?");
	}

	if (!fIsOpen || !(*ppWindowFrame))
	{
		if (guidDocViewType == LOGVIEWID_ProjectSpecificEditor)
			return pFile->DoDefaultAction(FALSE);
		// REVIEW(kperry): does the file exist on disk ? If not this call will fail
		//					should we create it empty like in V6
		CComPtr<IVsUIHierarchy> spHier = VCQI_cast<IVsUIHierarchy>(pFile->GetHierarchy());
		hr = pVsUIShellOpenDoc->OpenStandardEditor(OSE_ChooseBestStdEditor, bstrFile, guidDocViewType,
			L"%3", spHier, vsid, punkDocDataExisting, NULL, ppWindowFrame);
	}

	return hr;
}

//---------------------------------------------------------------------------
// interface: IVsProject2
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::RemoveItem( 
	/* [in] */ DWORD dwReserved,
	/* [in] */ VSITEMID itemid,
	/* [retval][out] */ BOOL __RPC_FAR *pfResult)
{
	return DoRemoveItem(itemid, pfResult, TRUE /*close if open*/);
}

HRESULT CVCArchy::DoRemoveItem(VSITEMID itemid, BOOL __RPC_FAR *pfResult, BOOL bCloseIfOpen /*= TRUE*/)
{
	CHECK_POINTER_NULL(pfResult);
	HRESULT hr = DoDeleteItem( DELITEMOP_RemoveFromProject, itemid, bCloseIfOpen );
	if( SUCCEEDED( hr ) )
		*pfResult = TRUE;
	else 
		*pfResult = FALSE;
	return hr;
}

STDMETHODIMP CVCArchy::ReopenItem( 
	/* [in] */ VSITEMID itemid,
	/* [in] */ REFGUID rguidEditorType,
	/* [in] */ LPCOLESTR pszPhysicalView,
	/* [in] */ REFGUID rguidLogicalView,
	/* [in] */ IUnknown __RPC_FAR *punkDocDataExisting,
	/* [retval][out] */ IVsWindowFrame __RPC_FAR *__RPC_FAR *ppWindowFrame)
{
	CHECK_POINTER_NULL(ppWindowFrame);

	CVCNode* pNode = NULL;
	HRESULT hr = VSITEMID2Node(itemid, &pNode);
	RETURN_ON_FAIL_OR_NULL(hr, pNode);

	CComPtr<IVsUIShellOpenDocument> pVsUIShellOpenDoc;	
	hr = ExternalQueryService(SID_SVsUIShellOpenDocument, IID_IVsUIShellOpenDocument, (void **)&pVsUIShellOpenDoc);
	VSASSERT(SUCCEEDED(hr), "Failed to obtain VSUIShellOpenDocument service.  Did the service ID change?  Are you in the wrong thread?");
	RETURN_ON_FAIL_OR_NULL2(hr, pVsUIShellOpenDoc, E_UNEXPECTED);
  	
	CVCFileNode* pFile = (CVCFileNode *)pNode;
	RETURN_ON_NULL(pFile);

	CComBSTR bstrPath;
	hr = pFile->get_FullPath(&bstrPath);
	RETURN_ON_FAIL(hr);

	CComPtr<IVsUIHierarchy> spHier = VCQI_cast<IVsUIHierarchy>(pFile->GetHierarchy());
	CComPtr<IVsWindowFrame> srpWindowFrame;
	hr = pVsUIShellOpenDoc->OpenSpecificEditor(0, bstrPath, rguidEditorType, pszPhysicalView, rguidLogicalView, L"%3", 
		spHier, pFile->GetVsItemID(), punkDocDataExisting, NULL, &srpWindowFrame);
	if (SUCCEEDED(hr) && srpWindowFrame)
	{
		if (ppWindowFrame)
			*ppWindowFrame = srpWindowFrame.Detach();
	}

	return hr;
}

//---------------------------------------------------------------------------
// interface: IVsProject3
// AddItemWithSpecific is used to add item(s) to the project and additionally ask the project to
// open the item using the specified editor information.  An extension of IVsProject::AddItem().
STDMETHODIMP CVCArchy::AddItemWithSpecific( VSITEMID itemidLoc, VSADDITEMOPERATION dwAddItemOperation, LPCOLESTR pszItemName, ULONG cFilesToOpen, LPCOLESTR rgpszFilesToOpen[], HWND hwndDlg, VSSPECIFICEDITORFLAGS grfEditorFlags, REFGUID rguidEditorType, LPCOLESTR pszPhysicalView, REFGUID rguidLogicalView, VSADDRESULT* pResult)
{
	HRESULT hr;

	// add the item(s)
	hr = DoAddItem( itemidLoc, dwAddItemOperation, pszItemName, cFilesToOpen, rgpszFilesToOpen, hwndDlg, pResult );
	RETURN_ON_FAIL(hr);
	if( *pResult != ADDRESULT_Success )
		return hr;

	for( unsigned int i = 0; i < cFilesToOpen; i++ )
	{
		// open the item(s)
		// get the itemid for the new item
		VSITEMID itemid;
		BOOL bFound = FALSE;
		IsDocumentInProject(dwAddItemOperation == VSADDITEMOP_CLONEFILE ? pszItemName : rgpszFilesToOpen[i], &bFound, NULL, &itemid );
		if( !bFound )
		{
			*pResult = ADDRESULT_Failure;
			return E_FAIL;
		}

		if( grfEditorFlags & VSSPECIFICEDITOR_DoOpen )
		{
			CComPtr<IVsWindowFrame> pWindowFrame;
			hr = OpenItemWithSpecific( itemid, grfEditorFlags, rguidEditorType, pszPhysicalView, rguidLogicalView, NULL, &pWindowFrame );
			RETURN_ON_FAIL(hr);
			if( pWindowFrame )
				pWindowFrame->Show();
		}
	}
		
	return S_OK;
}

// OpenItemWithSpecific is used to ask the project to open the item using the
// specified editor information.  An extension of IVsProject::OpenItem().
STDMETHODIMP CVCArchy::OpenItemWithSpecific( VSITEMID itemid, VSSPECIFICEDITORFLAGS grfEditorFlags, REFGUID rguidEditorType, LPCOLESTR pszPhysicalView, REFGUID rguidLogicalView, IUnknown* punkDocDataExisting, IVsWindowFrame** ppWindowFrame)
{
	HRESULT hr;

	CVCFileNode* pNode = NULL;
	hr = VSITEMID2Node( itemid, reinterpret_cast<CVCNode**>(&pNode) );
	if( SUCCEEDED( hr ) && pNode )
	{
		CComPtr<IVsUIShellOpenDocument> pVsUIShellOpenDoc;
		hr = ExternalQueryService(SID_SVsUIShellOpenDocument, IID_IVsUIShellOpenDocument, (void **)&pVsUIShellOpenDoc);
		VSASSERT(SUCCEEDED(hr), "Unable to obtain IVsUIShellOpenDocument service!  Did the IDs change?  Do you need to investigate threading issues?");

		CComBSTR bstrPath;
		bstrPath = pNode->GetFullPath();

		hr = pVsUIShellOpenDoc->OpenSpecificEditor( grfEditorFlags, bstrPath, rguidEditorType, pszPhysicalView, rguidLogicalView, L"%3", this, itemid, punkDocDataExisting, GetServiceProvider(), ppWindowFrame );
//		ppWindowFrame->Show();
	}
		
	return hr;
}

//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::TransferItem( LPCOLESTR pszMkDocumentOld,
				   LPCOLESTR pszMkDocumentNew,
				   IVsWindowFrame *punkWindowFrame)
{
	HRESULT hr;

	// get the VSITEMID for this document
	VSITEMID itemidNew = ItemIDFromName( m_pRootNode, pszMkDocumentNew );
	if( itemidNew == VSITEMID_NIL )
		RETURN_INVALID();

	// get an IVsRunningDocumentTable ptr
	CComPtr<IVsRunningDocumentTable> spDocTable;
	hr = ExternalQueryService(SID_SVsRunningDocumentTable, __uuidof(IVsRunningDocumentTable), (void **)&spDocTable);
	RETURN_ON_FAIL_OR_NULL(hr, spDocTable);

	// call IVsRunningDocumentTable::RenameDocument to transfer ownership
	// to our hierarchy, and give the document a new itemid within our project
	CComPtr<IVsHierarchy> spHier = VCQI_cast<IVsHierarchy>(this);
	hr = spDocTable->RenameDocument( pszMkDocumentOld, pszMkDocumentNew, spHier, itemidNew );

	// set the caption
	if( punkWindowFrame )
	{
		CComVariant varCaption = CComBSTR(L"%3");
		punkWindowFrame->SetProperty( VSFPROPID_OwnerCaption, varCaption );
	}

	return hr;
}

//---------------------------------------------------------------------------
// ISupportItemHandoff
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::HandoffItem( VSITEMID itemid,			 // itemid in project where document should be removed
				  IVsProject3 *pProjDest,	   // project to which document should be transferred
				  LPCOLESTR pszMkDocumentOld,  // passed as pszDocumentOld to IVsRunningDocumentTable::RenameDocument
				  LPCOLESTR pszMkDocumentNew,  // passed as pszDocumentNew to IVsRunningDocumentTable::RenameDocument
				  IVsWindowFrame *punkWindowFrame) // optional if document not open
{
	HRESULT hr;

	// call TransferItem() on the IVsProject3 that we're handed
	hr = pProjDest->TransferItem( pszMkDocumentOld, pszMkDocumentNew, punkWindowFrame );

	return hr;
}

STDMETHODIMP CVCArchy::GetItemContext (VSITEMID vsid, IServiceProvider **ppSP)
{
	CHECK_POINTER_NULL(ppSP);

	*ppSP = GetServiceProvider();
	VSASSERT(*ppSP != NULL, "This hierarchy doesn't have a service provider!");
	(*ppSP)->AddRef();
	return S_OK;
} 

STDMETHODIMP CVCArchy::GenerateUniqueItemName (
			  VSITEMID itemidLoc,
			  LPCOLESTR pszExt,
			  LPCOLESTR pszSuggestedRoot,
			  BSTR * pbstrItemName)
{
	CHECK_POINTER_NULL(pbstrItemName);

	CComPtr<VCProject> pProj;
	GetVCProject( NULL, &pProj );
	// If there is no extension then we won't have any way of identifying if it in 
	// our project or not. If we fail, the shell does a better job of doing FindFirstFile
	// This is VS7:146503, 150963, 150964
	RETURN_ON_NULL(pProj);

	if (pszExt && !*pszExt)
	{
        // This is a null extension. It is not a file. This means that we have to identify
        // if there is a codeelement with this pszSuggestedRoot name
		RETURN_ON_NULL(pszSuggestedRoot);
        CComVariant var;
        GetProperty( VSITEMID_ROOT, VSHPROPID_ExtObject, &var );
        CComPtr<Project> spShlPrj;
		if (FAILED(var.pdispVal->QueryInterface(__uuidof(Project), reinterpret_cast<void **> (&spShlPrj))))
			return E_FAIL;
        CComPtr<CodeModel> spCM;
        if (FAILED(spShlPrj->get_CodeModel(&spCM)))
            return E_FAIL;
        
        CComPtr<CodeElements> spCEs;
        if (FAILED(spCM->get_CodeElements(&spCEs)))
            return E_FAIL;
        
        VARIANT_BOOL vBool;
        CComBSTR bstrSuggestedRoot(pszSuggestedRoot);
        if (FAILED(spCEs->CreateUniqueID(bstrSuggestedRoot, pbstrItemName, &vBool)))
            return E_FAIL;

        return S_OK;
    }

	CComBSTR bstrProjPath;
	pProj->get_ProjectDirectory(&bstrProjPath);
	
	CComBSTR bstrFile;
	if( pszSuggestedRoot == NULL )
		bstrFile.LoadString(IDS_FILENAME);
	else
		bstrFile = pszSuggestedRoot;

	int i = 0;	
	while( i >= 0 )
	{
		i++;
		CComBSTR bstrName;
		CComBSTR bstrFullName;
		WCHAR pszCount[10];
		_itow(i, pszCount, 10);

		bstrName = bstrFile;
		bstrName.Append( pszCount );
		bstrName.Append( pszExt );
		
		bstrFullName = bstrProjPath;
		bstrFullName.Append( bstrName );

		if( ItemIDFromName(NULL, bstrFullName) != VSITEMID_NIL )
			continue;

		// Does the file exist on disk;
		/*
		REVIEW(kperry) enable this
		DWORD dwAttr = GetFileAttributesW(bstrFullName);
		if( dwAttr != 0xFFFFFFFF && (dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0)
			continue;
		*/

		*pbstrItemName = bstrName.Detach();
		return S_OK;
	}

	return E_FAIL; // how many files are in this project ? : )
} 

STDMETHODIMP CVCArchy::AddItem (
			  /* [in] */ VSITEMID itemidLoc,
			  /* [in] */ VSADDITEMOPERATION dwAddItemOperation,
			  /* [in] */ LPCOLESTR pszItemName,
			  /* [in] */ DWORD cFilesToOpen,
			  /* [in, size_is(cFilesToOpen)] */ LPCOLESTR rgpszFilesToOpen[],
			  /* [in] */ HWND hwndDlg,
			  /* [out, retval] */ VSADDRESULT * pResult)
{
	HRESULT hr = DoAddItem(itemidLoc,dwAddItemOperation,pszItemName,cFilesToOpen,rgpszFilesToOpen,hwndDlg,pResult);
	RETURN_ON_FAIL(hr);
	if( *pResult != ADDRESULT_Success )
		return hr;

	if( dwAddItemOperation == VSADDITEMOP_CLONEFILE )
	{
		// open the item(s)
		// get the itemid for the new item
		VSITEMID itemid;
		BOOL bFound = FALSE;
		IsDocumentInProject( pszItemName, &bFound, NULL, &itemid );
		if( !bFound )
		{
			*pResult = ADDRESULT_Failure;
			return E_FAIL;
		}

		CVCFileNode* pNode = NULL;
		hr = VSITEMID2Node( itemid, reinterpret_cast<CVCNode**>(&pNode) );
		if( SUCCEEDED( hr ) && pNode )
		{
			pNode->DoDefaultAction(TRUE);
		}
	}

	return hr;
}

HRESULT CVCArchy::DoAddItem (
			  /* [in] */ VSITEMID itemidLoc,
			  /* [in] */ VSADDITEMOPERATION dwAddItemOperation,
			  /* [in] */ LPCOLESTR pszItemName,
			  /* [in] */ DWORD cFilesToOpen,
			  /* [in, size_is(cFilesToOpen)] */ LPCOLESTR rgpszFilesToOpen[],
			  /* [in] */ HWND hwndDlg,
			  /* [out, retval] */ VSADDRESULT * pResult)
{
	CHECK_POINTER_NULL(pResult);
	*pResult = ADDRESULT_Failure;

	if (cFilesToOpen <= 0)
		return S_OK;
	
	// adding this check because TeamServer wants it.  (VS7 196912)
	if (dwAddItemOperation == VSADDITEMOP_LINKTOFILE)
		return E_FAIL;

	HRESULT hr = S_OK;

	CVCBaseFolderNode* pNode;

	// check the item id for special cases
	switch( itemidLoc )
	{
	case VSITEMID_NIL:
		return S_OK;

	case VSITEMID_ROOT:
		pNode = (CVCBaseFolderNode*)GetProjectNode();
		break;

	default:
		pNode = (CVCBaseFolderNode*)itemidLoc;
		break;
	}

	RETURN_ON_NULL(pNode);

	// do the open
	hr = pNode->HandleAddItemDlg(dwAddItemOperation, pszItemName, cFilesToOpen, rgpszFilesToOpen, hwndDlg, pResult);

	return hr;
}

//---------------------------------------------------------------------------
// interface: IExtractIconW
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetIconLocation(UINT uFlags, LPWSTR szIconFile, UINT cchMax, 
								   int* piIndex, UINT* pwFlags)
{
	VSASSERT(szIconFile != NULL	 && cchMax != 0, "Invalid input parameter");
	CHECK_POINTER_NULL(piIndex);
	CHECK_POINTER_NULL(pwFlags);
	CHECK_READ_POINTER_NULL(szIconFile);
	if(cchMax == 0)
		RETURN_INVALID();

	// Get our path
	CStringW strIconFile = szIconFile;
	if(!UtilGetModuleFileNameW(_Module.GetModuleInstance(), strIconFile))
		return E_FAIL;

	// Return project icon index
	CComBSTR bstr;
	*piIndex = IDI_PROJICON;
	*pwFlags = 0;
	bstr = strIconFile;
	wcsncpy(szIconFile, bstr, cchMax-1);
	szIconFile[cchMax-1] = L'\0';
	return (cchMax <= (UINT)(strIconFile.GetLength()) ? S_OK : S_FALSE);
}

//---------------------------------------------------------------------------
// interface: IExtractIconW
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::Extract(LPCWSTR pszFile, UINT nIconIndex, HICON *phiconLarge, 
						   HICON *phiconSmall, UINT nIconSize)
{
	VSASSERT(pszFile != NULL  && nIconIndex == IDI_PROJICON, "Invalid input parameter");
#ifdef _DEBUG
	CStringW strOurFile, strTheirFile;
	if (!UtilGetModuleFileNameW(_Module.GetModuleInstance(), strOurFile))
		VSASSERT(FALSE, "Failed to pick up module file name!");
	else
	{
		strTheirFile = pszFile;
		VSASSERT(strTheirFile == strOurFile, "File name mismatch!");
	}
#endif
	if(pszFile == NULL	|| nIconIndex != IDI_PROJICON)
		RETURN_INVALID();

	BOOL bSuccess = FALSE;
	if(phiconLarge)
	{	// Get size
		int size = LOWORD(nIconSize);
		VSASSERT(size != 0, "Icon size is zero!");
		if(size == 0)
			RETURN_INVALID();
		*phiconLarge = (HICON)LoadImage(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDI_PROJICON),
								 IMAGE_ICON, size, size, LR_DEFAULTCOLOR );
		if(*phiconLarge)
			bSuccess = TRUE;
	}

	if(phiconSmall)
	{
		int size = HIWORD(nIconSize);
		VSASSERT(size != 0, "Icon size is zero!");
		if(size == 0)
			RETURN_INVALID();
		bSuccess = FALSE;
		*phiconSmall = (HICON)LoadImage(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDI_PROJICON),
								 IMAGE_ICON, size, size, LR_DEFAULTCOLOR);
		if(*phiconSmall)
			bSuccess = TRUE;
	}
	if(bSuccess)
		return S_OK;
	return E_FAIL;
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Called to close the hierarchy.-- dangerous! Probably should do through solution! 
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::Close(void)
{
	VSASSERT( GetRootNode() != NULL, _T("close ignored: CVCArchy"));

	m_srpServiceProvider.Release();
	m_srpMonSel.Release();

	UnregisterSccProject();

	CVCNode *pNode = GetProjectNode();
	CVCProjectNode *pProjNode = static_cast<CVCProjectNode *>(pNode);
	if (pProjNode != NULL)
	{
		// Get rid of any existing dragdrop object
		OnAfterDropOrPaste(FALSE /* no cut */, FALSE /* no move */, TRUE /* drag */, TRUE /* cleanup only */);
		// Get rid of any existing cut/copy/paste object
		OnClear(m_bWasCut);

		pProjNode->CleanUpBeforeDelete();
		pProjNode->Refresh(this);

		pProjNode->SetArchy(NULL);
		SetRootNode(NULL);
	}

	// Unadvise clipboard helper events
	RegisterClipboardNotifications(FALSE);
	
	// Release the IVsCfgProviderEventsHelper
	m_pCfgProviderEventsHelper.Release();

	GetBuildPkg()->ReleaseSolutionRef();	// want the solution to disappear from our view when the last project goes away

	return S_OK; 
}

//---------------------------------------------------------------------------
// interface: IPersist
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetClassID(
			/* [out] */ CLSID *pClassID)
{
	CHECK_POINTER_NULL(pClassID);
	*pClassID = IID_IVCArchy;
	return S_OK;
}

//---------------------------------------------------------------------------
// interface: IPersistFile
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::IsDirty(BOOL __RPC_FAR *pfIsDirty)
{
	CHECK_POINTER_NULL(pfIsDirty);
	*pfIsDirty = FALSE;
	if(m_pRootNode)
	{
		if(GetProjectNode() && GetProjectNode()->IsProjectFileDirty())
		{
			*pfIsDirty = TRUE;
			return S_OK;
		}
	}
	return S_FALSE;
}

//---------------------------------------------------------------------------
// interface: IPersistFileFormat
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::InitNew( 
			/* [in] */ DWORD nFormatIndex)
{
	return E_NOTIMPL;	// CVCArchy::InitNew
}

//---------------------------------------------------------------------------
// interface: IPersistFileFormat
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::Load(
			/* [in] */ LPCOLESTR pszFilename,
			/* [in] */ DWORD grfMode,
			/* [in] */ BOOL fReadOnly)
{
	return E_NOTIMPL;	// CVCArchy::Load
}

//---------------------------------------------------------------------------
// interface: IPersistFileFormat
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::Save(
			/* [in] */ LPCOLESTR pszFilename,
			/* [in] */ BOOL fRemember,
			/* [in] */ DWORD nFormatIndex)
{
	DWORD startTime, endTime;

	// if we're doing performance logging
	VARIANT_BOOL bLog = VARIANT_FALSE;
	if (GetBuildPkg()->GetProjectEngine())
		GetBuildPkg()->GetProjectEngine()->get_PerformanceLogging( &bLog );
	if( bLog == VARIANT_TRUE )
	{
		// start time
		startTime = GetTickCount();
		fprintf( stdout, "Project Save start: %d\n", startTime );
	}

	// do the save:
	
	// get a pointer to the VCProject so we can determine the current file
	// format 
	CComPtr<VCProject> pProj;
	GetVCProject( NULL, &pProj );
	RETURN_ON_NULL2(pProj, E_NOINTERFACE);

	// if fRemember is set, we need to do a Save AS (e.g. rename the project
	// file)
	if( fRemember )
		pProj->put_ProjectFile( const_cast<BSTR>(pszFilename) );

	// get the current file format
	enumFileFormat eCurFormat;
	pProj->get_FileFormat( &eCurFormat );
	if( (DWORD)eCurFormat != nFormatIndex )
		pProj->put_FileFormat( (enumFileFormat)nFormatIndex );
	
	// do the save
	CHECK_ZOMBIE(GetProjectNode(), IDS_ERR_PROJ_ZOMBIE);
	HRESULT hr = GetProjectNode()->OnSaveProject( pszFilename );

	if( bLog == VARIANT_TRUE )
	{
		// end time
		endTime = GetTickCount();
		fprintf( stdout, "Project Save end: %d\n", endTime );
	}

	return hr;
}

//---------------------------------------------------------------------------
// interface: IPersistFileFormat
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::SaveCompleted(
			/* [unique][in] */ LPCOLESTR pszFileName)
{
	return S_OK;
}

//---------------------------------------------------------------------------
// interface: IPersistFileFormat
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetCurFile(
			/* [out] */ LPOLESTR __RPC_FAR *ppszFilename,
			/* [out] */ DWORD __RPC_FAR *pnFormatIndex)
{
	CHECK_POINTER_NULL(ppszFilename);

	// get a pointer to the VCProject so we can determine the current file
	// format
	CComPtr<VCProject> pProj;
	GetVCProject( NULL, &pProj );
	CHECK_ZOMBIE(pProj, IDS_ERR_PROJ_ZOMBIE);
	CHECK_ZOMBIE(GetProjectNode(), IDS_ERR_PROJ_ZOMBIE);

	HRESULT hr = S_OK;
	CComPtr<VCProject> pProject;
	GetProjectNode()->GetVCProject(&pProject);
	RETURN_ON_NULL(pProject);

	CComBSTR bstrProjFile;
	pProject->get_ProjectFile( &bstrProjFile );

	DWORD dwLen = (DWORD)(wcslen( bstrProjFile ) + sizeof(OLECHAR));

	*ppszFilename = (LPOLESTR)::CoTaskMemAlloc(dwLen * sizeof(OLECHAR));
	if (NULL != *ppszFilename)
	{
		ocscpy(*ppszFilename, bstrProjFile);
		hr = S_OK;
	}
	else
	{
		hr = E_OUTOFMEMORY;
	}
	
	// get the current file format
	enumFileFormat eCurFormat;
	pProject->get_FileFormat( &eCurFormat );
	*pnFormatIndex = (DWORD)eCurFormat;
	
	return hr;
}

STDMETHODIMP CVCArchy::GetFormatList( 
			/* [out] */ LPOLESTR __RPC_FAR *ppszFormatList)
{
	// load the string from resource
	CStringW strFormatList;
	strFormatList.LoadString( IDS_PROJECTSAVEFORMATS );
	// co alloc memory for the return string
	*ppszFormatList = (wchar_t*)CoTaskMemAlloc( (strFormatList.GetLength()+1) * sizeof(wchar_t) );
	if( !ppszFormatList )
		return E_OUTOFMEMORY;
	wcscpy( *ppszFormatList, strFormatList );
	return S_OK;
}

HRESULT CVCArchy::GetCfgOfName(LPCOLESTR szCfgName, VCConfiguration** ppVCCfg)
{
	*ppVCCfg = NULL;
	CComPtr<VCProject> pProj;
	GetVCProject( NULL, &pProj );
	RETURN_ON_NULL2(pProj, E_NOINTERFACE);

	CComPtr<IEnumVARIANT> pEnum;
	CComQIPtr<IVCCollection> pCollection;
	CComPtr<IDispatch> pDisp;
	HRESULT hr = pProj->get_Configurations( &pDisp );
	if( SUCCEEDED( hr ) && pDisp )
	{
		pCollection = pDisp;
		RETURN_ON_NULL2(pCollection, E_NOINTERFACE);
		hr = pCollection->_NewEnum( reinterpret_cast<IUnknown **>( &pEnum ) );
	}
	VSASSERT(SUCCEEDED(hr) && pEnum, "Failed to pick up configurations enumerator!");
	RETURN_ON_FAIL_OR_NULL(hr, pEnum);
	
	CComBSTR bstrCfgName = szCfgName;
	long actualCfgs = 0;
	pEnum->Reset();
	while( true )
	{
		CComVariant var;
		hr = pEnum->Next(1, &var, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE ) 
			break;
		if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
			continue;

		CComQIPtr<VCConfiguration> pConfig = var.punkVal;
		if (pConfig == NULL)
			continue;
		CComBSTR bstrName;
		hr = pConfig->get_Name(&bstrName);
		RETURN_ON_FAIL(hr);
		if (bstrName == bstrCfgName)
		{
			*ppVCCfg = pConfig.Detach();
			return S_OK;
		}
	}

	// Hmmmm.  Got here, so maybe the config name we started with didn't have a platform on it, so no match
	CStringW strCfgName = bstrCfgName;
	if (strCfgName.Find(L"|") > 0)
		return E_FAIL;	// got told a platform and didn't find it; bye-bye

	bstrCfgName += L"|";
	int nLen = bstrCfgName.Length();

	pEnum->Reset();
	while (true)
	{
		CComVariant var;
		hr = pEnum->Next(1, &var, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE ) 
			break;
		if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
			continue;

		CComQIPtr<VCConfiguration> pConfig = var.punkVal;
		CComBSTR bstrName;
		if (pConfig == NULL)
			continue;
		hr = pConfig->get_Name(&bstrName);
		RETURN_ON_FAIL(hr);
		if (_wcsnicmp(bstrName.m_str, bstrCfgName.m_str, nLen) == 0)
		{
			*ppVCCfg = pConfig.Detach();
			return S_OK;
		}
	}

	return E_FAIL;
}

//---------------------------------------------------------------------------
// interface: IVsProjectCfgProvider2
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::OpenProjectCfg(/*[in]*/ LPCOLESTR szProjectCfgCanonicalName, IVsProjectCfg **ppIVsProjectCfg)
{
	CHECK_POINTER_NULL(ppIVsProjectCfg);
	CComPtr<VCConfiguration> spConfig;
	HRESULT hr = GetCfgOfName(szProjectCfgCanonicalName, &spConfig);
	RETURN_ON_FAIL(hr);

	CComQIPtr<IVsCfg> pCfg;
	hr = CGenCfg::CreateInstance(&pCfg, this, spConfig);
	RETURN_ON_FAIL_OR_NULL(hr, pCfg);
	return pCfg->QueryInterface(__uuidof(IVsProjectCfg), (void**)ppIVsProjectCfg);
}

STDMETHODIMP CVCArchy::get_UsesIndependentConfigurations(/*[out]*/ BOOL *pfUsesIndependentConfigurations)
{
	CHECK_POINTER_NULL(pfUsesIndependentConfigurations);
	*pfUsesIndependentConfigurations = TRUE;
	return S_OK;
}

STDMETHODIMP CVCArchy::GetCfgNames( 
	/* [in] */ ULONG celt,
	/* [size_is][out][in] */ BSTR rgbstr[  ],
	/* [optional][out] */ ULONG*pcActual)
{
	CComPtr<VCProject> pProj;
	GetVCProject( NULL, &pProj );
	RETURN_ON_NULL2(pProj, E_NOINTERFACE);

	CComPtr<IEnumVARIANT> pEnum;
	CComQIPtr<IVCCollection> pCollection;
	CComPtr<IDispatch> pDisp;
	
	//Get config collection
	HRESULT hr = pProj->get_Configurations( &pDisp );
	pCollection = pDisp;
	RETURN_ON_FAIL_OR_NULL2(hr, pCollection, E_NOINTERFACE);

	hr = pCollection->_NewEnum( reinterpret_cast<IUnknown **>( &pEnum ) );
	RETURN_ON_FAIL_OR_NULL(hr, pEnum);

	long cPlatforms = 1;
	CComPtr<IDispatch> spDispPlatforms;
	hr = pProj->get_Platforms(&spDispPlatforms);
	CComQIPtr<IVCCollection> spPlatforms = spDispPlatforms;
	if (SUCCEEDED(hr) && spPlatforms)
		spPlatforms->get_Count(&cPlatforms);

	//If there's a pointer to the actual count
	if (pcActual && !IsBadWritePtr(pcActual, sizeof(*pcActual)))
	{
		//Get the count
		long cCount;
		hr = pCollection->get_Count( &cCount );
		RETURN_ON_FAIL(hr);

		if (cPlatforms > 1)
			*pcActual = cCount / cPlatforms;
		else
			*pcActual = cCount;

		//If asking for count, return
		if (celt == 0)
			return S_OK;
	}
	else if (celt == 0) 
		return E_POINTER;	//Asking for count, but null pointer supplied


	//Loop the config name array filling it out
	pEnum->Reset();
	CVCMapStringWToPtr mapNames;
	for (ULONG i = 0, j = 0; i < celt; j++)
	{
		CComVariant var;
		hr = pEnum->Next(1, &var, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE ) 
			break;
		if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
			continue;
	
		//Get config
		CComQIPtr<VCConfiguration> pConfig;
		pConfig = var.punkVal;
		RETURN_ON_NULL2(pConfig, E_NOINTERFACE);
	
		CComBSTR bstrName;
		hr = pConfig->get_ConfigurationName(&bstrName);
		RETURN_ON_FAIL(hr);

		if (cPlatforms > 1)
		{
			CStringW strName = bstrName;
			int nBar = strName.Find(L'|');
			if (nBar > 0)
				strName = strName.Left(nBar);

			void* pDummy = NULL;
			if (mapNames.Lookup(strName, pDummy))
				continue;
			mapNames.SetAt(strName, pDummy);
			bstrName = strName;
		}

		//Fill out array
		rgbstr[i] = bstrName.Detach();
		i++;
	}
	return S_OK;
}

STDMETHODIMP CVCArchy::GetPlatformNames( 
	/* [in] */ ULONG celt,
	/* [size_is][out][in] */ BSTR rgbstr[  ],
	/* [optional][out] */ ULONG*pcActual)
{
	VSASSERT(NULL != GetProjectNode(), "No project node!  Is this hierarchy zombie?");
	CComPtr<VCProject> spProject;
	if (GetProjectNode())
	{
		GetProjectNode()->GetVCProject(&spProject);
		VSASSERT(spProject != NULL, "No project!  Is this hierarchy zombie?");
	}

	CComQIPtr<IVCCollection> spPlatforms;
	if (spProject)
	{
		CComPtr<IDispatch> spDispPlatforms;
		spProject->get_Platforms(&spDispPlatforms);
		spPlatforms = spDispPlatforms;
	}

	return GetSupportedPlatformNames(spPlatforms, celt, rgbstr, pcActual);
}

STDMETHODIMP CVCArchy::GetCfgOfName( 
	/* [in] */ LPCOLESTR pszCfgName,
	/* [in] */ LPCOLESTR pszPlatformName,
	/* [out] */ IVsCfg* *ppCfg)
{
	CHECK_POINTER_NULL(ppCfg);

	CComBSTR bstrCfgName = pszCfgName;
	bstrCfgName += L"|";
	bstrCfgName += pszPlatformName;

	CComPtr<VCConfiguration> spConfig;
	HRESULT hr = GetCfgOfName(bstrCfgName, &spConfig);
	RETURN_ON_FAIL_OR_NULL(hr, spConfig);

	return CGenCfg::CreateInstance(ppCfg, this, spConfig);
}

STDMETHODIMP CVCArchy::AddCfgsOfCfgName( 
	/* [in] */ LPCOLESTR pszCfgName,
	/* [in] */ LPCOLESTR pszCloneCfgName,
	/* [in] */ BOOL fPrivate)
{
	CComPtr<VCProject> pProj;
	GetVCProject( NULL, &pProj );
	CComQIPtr<IVCProjectImpl> pProjImpl = pProj;
	RETURN_ON_NULL2(pProjImpl, E_NOINTERFACE);

	// add a config to the project with the given name
	HRESULT hr = pProjImpl->AddConfigurationInternal(pszCfgName);
	RETURN_ON_FAIL(hr);		// handles the dirty check as well

	// if we're given a name to clone from, then we need to clone from it
	if( pszCloneCfgName && *pszCloneCfgName != L'\0')
	{
		// get the configs collection
		CComPtr<IDispatch> pDisp;
		hr = pProj->get_Configurations( &pDisp );
		CComQIPtr<IVCCollection> pCollection = pDisp;
		RETURN_ON_FAIL_OR_NULL2(hr, pCollection, E_NOINTERFACE);
		
		// iterate through the platforms, copying the config for each
		CComPtr<IDispatch> pDispPlatColl;
		hr = pProj->get_Platforms( &pDispPlatColl );
		CComQIPtr<IVCCollection> pPlatColl = pDispPlatColl;
		RETURN_ON_FAIL_OR_NULL2(hr, pPlatColl, E_NOINTERFACE);
		long num_platforms;
		hr = pPlatColl->get_Count( &num_platforms );
		RETURN_ON_FAIL(hr);
		CComQIPtr<VCConfiguration> pNewCfg;
		for( long i = 1; i <= num_platforms; i++ )
		{
			CComVariant varIdx = i;
			CComPtr<IDispatch> pDispPlatform;
			hr = pPlatColl->Item( varIdx, &pDispPlatform );
			if( FAILED( hr ) )
				continue;
			CComQIPtr<VCPlatform> pPlatform = pDispPlatform;
			if( !pPlatform )
				continue;
			// get the name of the platform
			CComBSTR bstrPlatformName;
			pPlatform->get_Name( &bstrPlatformName );
			// concat it (and the '|' separator) onto the config names
			CComBSTR bstrNewCfgName = CComBSTR( pszCfgName );
			bstrNewCfgName += L"|";
			bstrNewCfgName += bstrPlatformName;
			CComBSTR bstrOldCfgName = CComBSTR( pszCloneCfgName );
			bstrOldCfgName += L"|";
			bstrOldCfgName += bstrPlatformName;
			// look up these names in the config collections
			CComPtr<IDispatch> pDispNewCfg;
			varIdx = bstrNewCfgName;
			hr = pCollection->Item( varIdx, &pDispNewCfg );
			if( FAILED( hr ) )
				continue;
			pNewCfg = pDispNewCfg;
			if( !pNewCfg )
				continue;
			varIdx = bstrOldCfgName;
			CComPtr<IDispatch> pDispOldCfg;
			hr = pCollection->Item( varIdx, &pDispOldCfg );
			if( FAILED( hr ) )
				continue;
			CComQIPtr<VCConfiguration> pOldCfg = pDispOldCfg;
			if( !pOldCfg )
				continue;
			// copy the config
			hr = pOldCfg->CopyTo( pDispNewCfg );
		}

		// set the intermediate and output directories
		// NOTE: by setting them to blank strings we will force the config to
		// regenerate defaults next time it is asked for them,
		// then we ask for them immediately to ensure they aren't left in a
		// bad state, "just in case"
		hr = pNewCfg->put_IntermediateDirectory( CComBSTR( L"" ) );
		RETURN_ON_FAIL(hr);
		hr = pNewCfg->put_OutputDirectory( CComBSTR( L"" ) );
		RETURN_ON_FAIL(hr);
		CComBSTR bstrTemp;
		hr = pNewCfg->get_IntermediateDirectory( &bstrTemp );
		RETURN_ON_FAIL(hr);
		hr = pNewCfg->get_OutputDirectory( &bstrTemp );
		RETURN_ON_FAIL(hr);
	}
	
	return S_OK;
}

STDMETHODIMP CVCArchy::DeleteCfgsOfCfgName( 
	/* [in] */ LPCOLESTR pszCfgName)
{
	CComPtr<VCProject> pProj;
	GetVCProject( NULL, &pProj );
	RETURN_ON_NULL2(pProj, E_NOINTERFACE);

	// find the config with this name
	CComPtr<IDispatch> pDispColl;
	HRESULT hr = pProj->get_Configurations( &pDispColl );
	CComQIPtr<IVCCollection> pColl = pDispColl;
	RETURN_ON_FAIL_OR_NULL2(hr, pColl, E_NOINTERFACE);
	CComPtr<IDispatch> pDispCfg;
	long lcItems;
	pColl->get_Count( &lcItems );
	for( long i = 1; i <= lcItems && SUCCEEDED(hr); i++ )
	{
		pDispCfg = NULL;
		CComVariant varIdx = i;
		hr = pColl->Item( varIdx, &pDispCfg );
		if( FAILED( hr ) )
			continue;
		CComQIPtr<VCConfiguration> pCfg = pDispCfg;
		if( !pCfg )
			continue;
		CComBSTR bstrCfgName;
		hr = pCfg->get_ConfigurationName( &bstrCfgName );
		if( FAILED( hr ) )
			continue;
		// if we found it, delete it
		if( bstrCfgName == pszCfgName )
		{
			// remove it
			hr = pProj->RemoveConfiguration( pDispCfg );
		}
	}

	return S_OK;
}

STDMETHODIMP CVCArchy::RenameCfgsOfCfgName( 
	/* [in] */ LPCOLESTR pszOldName,
	/* [in] */ LPCOLESTR pszNewName)
{
	CComPtr<VCProject> pProj;
	GetVCProject( NULL, &pProj );
	RETURN_ON_NULL2(pProj, E_NOINTERFACE);	// handles the dirty check as well

	// find the config with this name
	CComPtr<IDispatch> pDispColl;
	HRESULT hr = pProj->get_Configurations( &pDispColl );
	CComQIPtr<IVCCollection> pColl = pDispColl;
	RETURN_ON_FAIL_OR_NULL2(hr, pColl, E_NOINTERFACE);
	long lcItems;
	pColl->get_Count( &lcItems );
	CComPtr<IDispatch> pDispCfg;
	for( long i = 1; i <= lcItems; i++ )
	{
		pDispCfg = NULL;
		CComVariant varIdx = i;
		hr = pColl->Item( varIdx, &pDispCfg );
		if( FAILED( hr ) )
			continue;
		CComQIPtr<VCConfiguration> pCfg = pDispCfg;
		if( !pCfg )
			continue;
		CComBSTR bstrCfgName;
		hr = pCfg->get_Name( &bstrCfgName );
		if( FAILED( hr ) )
			continue;
		// remove the platform part of the name
		CStringW strCfgName = bstrCfgName;
		int pos = strCfgName.Find( '|' );
		strCfgName = strCfgName.Left( pos );

		// if we found it, rename it
		if( strCfgName == pszOldName )
		{
			// don't need to tack the platform part (e.g. '|win32') on before
			// setting it
			hr = pCfg->put_Name( CComBSTR( pszNewName ) );
			if( FAILED( hr ) )
				break;
		}
	}

	return S_OK;
}

STDMETHODIMP CVCArchy::AddCfgsOfPlatformName(/* [in] */ LPCOLESTR pszPlatformName, /* [in] */ LPCOLESTR pszClonePlatformName)
{
	RETURN_ON_NULL(GetProjectNode());

	CComPtr<VCProject> spProject;
	GetProjectNode()->GetVCProject(&spProject);
	CComQIPtr<IVCProjectImpl> spProjectImpl = spProject;
	RETURN_ON_NULL(spProjectImpl);

	CComBSTR bstrPlatformName = pszPlatformName;

	HRESULT hr = spProjectImpl->AddPlatformInternal(bstrPlatformName);
	RETURN_ON_FAIL(hr);		// handles dirty check as well

	if (!pszClonePlatformName || *pszClonePlatformName == L'\0')
		return S_OK;

	// if we're given a name to clone from, then we need to clone from it
	RETURN_ON_NULL(GetBuildPkg()->GetProjectEngine());

	CComPtr<IDispatch> spDisp;
	GetBuildPkg()->GetProjectEngine()->get_Platforms(&spDisp);
	CComQIPtr<IVCCollection> spColl = spDisp;
	RETURN_ON_NULL(spColl);

	CComPtr<IDispatch> spDispNewPlatform;
	hr = spColl->Item(CComVariant(CComBSTR(pszPlatformName)), &spDispNewPlatform);
	RETURN_ON_FAIL_OR_NULL2(hr, spDispNewPlatform, E_UNEXPECTED);

	CComPtr<IDispatch> spDispClonePlatform;
	hr = spColl->Item(CComVariant(CComBSTR(pszClonePlatformName)), &spDispClonePlatform);
	RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, spDispClonePlatform, S_FALSE);	// not a whole lot we can do if we can't find the platform to clone from

	// get the configs collection
	spDisp.Release();
	hr = spProject->get_Configurations( &spDisp );
	spColl = spDisp;
	RETURN_ON_FAIL_OR_NULL2(hr, spColl, E_NOINTERFACE);
	
	// iterate through the configs collection, building up the two collections we need to work with
	CComDynamicListTyped<VCConfiguration> rgClonePlatformConfigs;
	CComDynamicListTyped<VCConfiguration> rgNewPlatformConfigs;

	CComPtr<IEnumVARIANT> spEnumCfgs;
	hr = spColl->_NewEnum(reinterpret_cast<IUnknown **>(&spEnumCfgs));
	RETURN_ON_FAIL_OR_NULL2(hr, spEnumCfgs, E_UNEXPECTED);

	spEnumCfgs->Reset();
	while (TRUE)
	{
		CComVariant var;
		HRESULT hr2 = spEnumCfgs->Next(1, &var, NULL);
		if (hr2 != S_OK)
			break;
		if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
			continue;
		CComQIPtr<VCConfiguration> spConfig = var.pdispVal;
		if (spConfig == NULL)
			continue;

		spDisp.Release();
		hr2 = spConfig->get_Platform(&spDisp);
		if (FAILED(hr2))
			continue;
		else if (spDisp == spDispClonePlatform)
			rgClonePlatformConfigs.Add(spConfig);
		else if (spDisp == spDispNewPlatform)
			rgNewPlatformConfigs.Add(spConfig);
	}

	// now copy the configs that match
	long lcCloneConfigs = rgClonePlatformConfigs.GetSize();
	long lcNewConfigs = rgNewPlatformConfigs.GetSize();
	for (long idxClone = 0; idxClone < lcCloneConfigs; idxClone++)
	{
		CComPtr<VCConfiguration> spCloneConfig = rgClonePlatformConfigs.GetAt(idxClone);
		if (spCloneConfig == NULL)
			continue;

		CComBSTR bstrCloneName;
		spCloneConfig->get_Name(&bstrCloneName);
		if (!bstrCloneName)
			continue;

		CStringW strCloneName = bstrCloneName;
		int nBar = strCloneName.Find(L'|');
		if (nBar > 0)
			strCloneName = strCloneName.Left(nBar);
		if (strCloneName.IsEmpty())
			continue;

		BOOL bFoundIt = FALSE;
		for (long idxNew = 0; idxNew < lcNewConfigs && !bFoundIt; idxNew++)
		{
			CComPtr<VCConfiguration> spNewConfig = rgNewPlatformConfigs.GetAt(idxNew);
			if (spNewConfig == NULL)
				continue;

			CComBSTR bstrNewName;
			spNewConfig->get_Name(&bstrNewName);
			if (!bstrNewName)
				continue;

			CStringW strNewName = bstrNewName;
			nBar = strNewName.Find(L'|');
			if (nBar > 0)
				strNewName = strNewName.Left(nBar);

			if (strCloneName != strNewName)
				continue;

			bFoundIt = TRUE;
			// they match, so clone it
			hr = spCloneConfig->CopyTo(spNewConfig);

			// set the intermediate and output directories
			// NOTE: by setting them to blank strings we will force the config to
			// regenerate defaults next time it is asked for them,
			// then we ask for them immediately to ensure they aren't left in a
			// bad state, "just in case"
			hr = spNewConfig->put_IntermediateDirectory( CComBSTR( L"" ) );
			RETURN_ON_FAIL(hr);
			hr = spNewConfig->put_OutputDirectory( CComBSTR( L"" ) );
			RETURN_ON_FAIL(hr);
			CComBSTR bstrTemp;
			hr = spNewConfig->get_IntermediateDirectory( &bstrTemp );
			RETURN_ON_FAIL(hr);
			hr = spNewConfig->get_OutputDirectory( &bstrTemp );
			RETURN_ON_FAIL(hr);

			rgNewPlatformConfigs.Remove(spNewConfig);	// take it out so we don't try to match against it again
		}
	}

	return S_OK;
}

STDMETHODIMP CVCArchy::DeleteCfgsOfPlatformName(/* [in] */ LPCOLESTR pszPlatformName)
{
	RETURN_ON_NULL(GetProjectNode());

	if (!pszPlatformName || *pszPlatformName == L'\0')
		RETURN_INVALID();

	CComPtr<VCProject> spProject;
	GetProjectNode()->GetVCProject(&spProject);
	CComQIPtr<IVCProjectImpl> spProjectImpl = spProject;
	RETURN_ON_NULL(spProjectImpl);

	CComPtr<IDispatch> spDisp;
	spProject->get_Platforms(&spDisp);
	CComQIPtr<IVCCollection> spColl = spDisp;
	RETURN_ON_NULL(spColl);

	spDisp.Release();
	spColl->Item(CComVariant(CComBSTR(pszPlatformName)), &spDisp);
	RETURN_ON_NULL2(spDisp, S_FALSE);

	return spProjectImpl->RemovePlatformInternal(spDisp);
}

STDMETHODIMP CVCArchy::GetSupportedPlatformNames(/* [in] */ ULONG celt, /* [size_is][out][in] */ BSTR rgbstr[],
	/* [optional][out] */ ULONG*pcActual)
{
	CComQIPtr<IVCCollection> spPlatforms;
	VSASSERT(g_pBuildPackage->m_pProjectEngine, "Project engine not initialized!");
	if (g_pBuildPackage && g_pBuildPackage->m_pProjectEngine)
	{
		CComPtr<IDispatch> spDispPlatforms;
		g_pBuildPackage->m_pProjectEngine->get_Platforms(&spDispPlatforms);
		spPlatforms = spDispPlatforms;
	}

	return GetSupportedPlatformNames(spPlatforms, celt, rgbstr, pcActual);
}

HRESULT CVCArchy::GetSupportedPlatformNames(IVCCollection* pPlatforms, ULONG celt, BSTR rgbstr[], ULONG* pcActual)
{
	long cFound = 0;
	HRESULT hr = S_OK;
	if( celt == 0 )
	{
		CHECK_POINTER_NULL(pcActual);
		if (pPlatforms)
		{
			hr = pPlatforms->get_Count(&cFound);
			*pcActual = cFound;
			return hr;
		}
		else
		{
			*pcActual = 1;
			return S_OK;
		}
	}

	if (celt > 1 && pcActual != NULL && !IsBadWritePtr(pcActual, sizeof(*pcActual)))
	{
		if (pPlatforms)
		{
			pPlatforms->get_Count(&cFound);
			*pcActual = cFound;
		}
		else
			*pcActual = 1;
	}

	CHECK_POINTER_NULL(rgbstr);

	if (pPlatforms)
	{
		CComPtr<IEnumVARIANT> spEnum;
		if (SUCCEEDED(pPlatforms->_NewEnum(reinterpret_cast<IUnknown **>(&spEnum))) && spEnum)
		{
			spEnum->Reset();
			ULONG cFetched = 0;
			while (TRUE && cFetched <= celt)
			{
				CComVariant var;
				HRESULT hr2 = spEnum->Next(1, &var, NULL);
				if (hr2 != S_OK)
					break;
				if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
					continue;
				CComQIPtr<VCPlatform> spPlatform = var.pdispVal;
				if (spPlatform == NULL)
					continue;
				CComBSTR bstrName;
				if (FAILED(spPlatform->get_Name(&bstrName)))
					continue;
				rgbstr[cFetched] = bstrName.Detach();
				cFetched++;
			}
			return S_OK;
		}
	}

	CComBSTR bstrPlat = L"Win32";
	rgbstr[0] = bstrPlat.Detach();
	return S_OK;
}

VARIANT_BOOL CVCArchy::SupportsMultiplePlatforms()
{
	long cPlatforms = 1;
	if (GetBuildPkg() && GetBuildPkg()->GetProjectEngine())
	{
		CComPtr<IDispatch> spDispPlatforms;
		if (SUCCEEDED(GetBuildPkg()->GetProjectEngine()->get_Platforms(&spDispPlatforms)))
		{
			CComQIPtr<IVCCollection> spPlatforms = spDispPlatforms;
			if (spPlatforms)
			{
				if (FAILED(spPlatforms->get_Count(&cPlatforms)))
					cPlatforms = 1;
			}
		}
	}

	return (cPlatforms > 1) ? VARIANT_TRUE : VARIANT_FALSE;
}

STDMETHODIMP CVCArchy::GetCfgProviderProperty(/* [in] */ VSCFGPROPID propid, /* [out] */ VARIANT*pOut)
{
	CHECK_POINTER_NULL(pOut);

    // check for bad arg
    if( pOut == NULL || pOut->vt != VT_EMPTY ) 
        return E_POINTER;

	switch( propid )
	{
	case VSCFGPROPID_SupportsCfgEditing:
		pOut->vt = VT_BOOL;
		pOut->boolVal = VARIANT_TRUE;
		break;
	case VSCFGPROPID_SupportsPlatformEditing:
		pOut->vt = VT_BOOL;
		pOut->boolVal = SupportsMultiplePlatforms();
		break;
	case VSCFGPROPID_SupportsCfgAdd:
		pOut->vt = VT_BOOL;
		pOut->boolVal = VARIANT_TRUE;
		break;
	case VSCFGPROPID_SupportsCfgDelete:
		pOut->vt = VT_BOOL;
		pOut->boolVal = VARIANT_TRUE;
		break;
	case VSCFGPROPID_SupportsCfgRename:
		pOut->vt = VT_BOOL;
		pOut->boolVal = VARIANT_TRUE;
		break;
	case VSCFGPROPID_SupportsPlatformAdd:
		pOut->vt = VT_BOOL;
		pOut->boolVal = SupportsMultiplePlatforms();
		break;
	case VSCFGPROPID_SupportsPlatformDelete:
		pOut->vt = VT_BOOL;
		pOut->boolVal = SupportsMultiplePlatforms();
		break;
	case VSCFGPROPID_SupportsPrivateCfgs:
		pOut->vt = VT_BOOL;
		pOut->boolVal = VARIANT_FALSE;
		break;
	case VSCFGPROPID_IntrinsicExtenderCATID:
		pOut->vt = VT_BSTR;
		LPOLESTR pStr;
		StringFromCLSID(__uuidof(IVCCfg),&pStr);
		pOut->bstrVal = SysAllocString(pStr);
		CoTaskMemFree(pStr);
		break;
	default:
		RETURN_INVALID();
	}
	
	return S_OK;
}

STDMETHODIMP CVCArchy::AdviseCfgProviderEvents( 
	/* [in] */ IVsCfgProviderEvents*pCPE,
	/* [out] */ VSCOOKIE* pdwCookie)
{
	if (m_pCfgProviderEventsHelper)
 		m_pCfgProviderEventsHelper->AdviseCfgProviderEvents( pCPE, pdwCookie );
	return S_OK;
}

STDMETHODIMP CVCArchy::UnadviseCfgProviderEvents( 
	/* [in] */ VSCOOKIE dwCookie)
{
// This assert should be perfectly valid, but this method gets called by the Property Page Frame AFTER the solution is closed!
//	VSASSERT( m_pCfgProviderEventsHelper, "UnadviseCfgProviderEvents() is being called on a VC Hierarchy that has already been Closed!" );
	if( m_pCfgProviderEventsHelper )
		m_pCfgProviderEventsHelper->UnadviseCfgProviderEvents( dwCookie );
	return S_OK;
}

HRESULT CVCArchy::GetCfgs(/*[in]*/ ULONG celt,
				/*[in, out, size_is(celt)]*/ IVsCfg *rgpcfg[],
				/*[out, optional]*/ ULONG *pcActual,
				/*[out, optional]*/ VSCFGFLAGS *prgfFlags)
{
	CComPtr<VCProject> pProj;
	GetVCProject( NULL, &pProj );
	RETURN_ON_NULL2(pProj, E_NOINTERFACE);

	CComPtr<IEnumVARIANT> pEnum;
	CComQIPtr<IVCCollection> pCollection;
	CComPtr<IDispatch> pDisp;
	HRESULT hr = pProj->get_Configurations( &pDisp );
	pCollection = pDisp;
	RETURN_ON_FAIL_OR_NULL(hr, pCollection)

	long actualCfgs = 0;
	if( celt == 0 )
	{
		CHECK_POINTER_NULL(pcActual);

		pCollection->get_Count(&actualCfgs);
		*pcActual = actualCfgs;
		return S_OK;
	}

	CHECK_POINTER_NULL(rgpcfg);

	hr = pCollection->_NewEnum( reinterpret_cast<IUnknown **>( &pEnum ) );
	VSASSERT(SUCCEEDED(hr) && pEnum, "Failed to pick up configurations enumerator!");
	RETURN_ON_FAIL_OR_NULL(hr, pEnum);

	pEnum->Reset();
	ULONG i = 0;
	while( i < celt )
	{
		CComVariant var;
		hr = pEnum->Next(1, &var, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE ) 
			break;
		if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
			continue;

		CComQIPtr<VCConfiguration> pConfig;
		pConfig = var.punkVal;
		hr = CGenCfg::CreateInstance(&rgpcfg[i], this, pConfig );
		RETURN_ON_FAIL(hr);
		i++;
	}
	return S_OK;
}

// IVsUpdateSolutionEvents
STDMETHODIMP CVCArchy::OnActiveProjectCfgChange(IVsHierarchy* pIVsHierarchy)
{
	if (GetProjectNode())
		GetProjectNode()->OnActiveProjectCfgChange();
	return S_OK;
}

//------------------------------------------------------------------------
// CVCArchy::IVsHierarchyDeleteHandler
//------------------------------------------------------------------------
STDMETHODIMP CVCArchy::QueryDeleteItem(
		/* [in] */ VSDELETEITEMOPERATION dwDelItemOp,
		/* [in] */ VSITEMID itemid,
		/* [retval][out] */ BOOL __RPC_FAR *pfCanDelete)
{
	CHECK_POINTER_NULL(pfCanDelete);
	if (dwDelItemOp != DELITEMOP_RemoveFromProject)
	{
		*pfCanDelete = FALSE;
		return S_OK;
	}

	// only support remove from project
	*pfCanDelete = TRUE;

	CVCNode		*pNode = NULL;
	HRESULT			 hr = VSITEMID2Node(itemid, &pNode);
	CVCNode *pVcNode = static_cast<CVCNode*>(pNode);
	if( pVcNode == NULL )
	{
		*pfCanDelete = FALSE;
		return hr;
	}

	// cannot remove anything while we're building
	if (!(GetBuildPkg()->NoBuildsInProgress()))
	{
		*pfCanDelete = FALSE;
		return hr;
	}

	CVCProjectNode* pProjNode = pVcNode->GetVCProjectNode();
	if (pProjNode == NULL || pProjNode->InBuild())
		*pfCanDelete = FALSE;
	
	return hr;
}
	
STDMETHODIMP CVCArchy::DeleteItem( 
		/* [in] */ VSDELETEITEMOPERATION dwDelItemOp,
		/* [in] */ VSITEMID itemid)
{
	return DoDeleteItem(dwDelItemOp, itemid, TRUE /*close if open*/);
}

HRESULT CVCArchy::DoDeleteItem(VSDELETEITEMOPERATION dwDelItemOp, VSITEMID itemid, BOOL bCloseIfOpen /* = TRUE */)
{
	if (dwDelItemOp != DELITEMOP_RemoveFromProject)
		return E_UNEXPECTED;

	CVCNode* pNode = NULL;
	HRESULT hr = VSITEMID2Node(itemid, &pNode);
	RETURN_ON_FAIL_OR_NULL(hr, pNode);

	return pNode->OnDelete(NULL, dwDelItemOp, bCloseIfOpen);
}

// helper function for SccGlyphChanged
// recursively call OnPropertyChanged for a node and its children
void CVCArchy::RecurseChildrenAndRefreshGlyphs( CVCNode* pHead )
{
	// refresh this node
	OnPropertyChanged( pHead, VSHPROPID_StateIconIndex, 0 );

	// refresh its children
	if( pHead->GetKindOf() == Type_CVCFileGroup )
	{
		CVCNode *pNode = NULL;
		VSITEMID id = static_cast<CVCBaseFolderNode*>(pHead)->GetFirstChild();
		if( id == VSITEMID_NIL )
			return;
		VSITEMID2Node( id, &pNode );
		if( !pNode )
			return;
		while( pNode )
		{
			RecurseChildrenAndRefreshGlyphs( pNode );
			pNode = pNode->GetNext();
		}
	}
}

//------------------------------------------------------------------------
// CVCArchy::IVsSccProject2::SccGlyphChanged
//------------------------------------------------------------------------
STDMETHODIMP CVCArchy::SccGlyphChanged( 
	/* [in] */ int cAffectedNodes,
	/* [size_is][in] */ const VSITEMID rgitemidAffectedNodes[],
	/* [size_is][in] */ const VsStateIcon rgsiNewGlyphs[],
	/* [size_is][in] */ const DWORD rgdwNewSccStatus[])
{
	HRESULT hr;
	CVCNode* pNode;

	// RefreshAll case:
	if( cAffectedNodes == 0 )
	{
		pNode = GetRootNode();
		OnPropertyChanged( pNode, VSHPROPID_StateIconIndex, 0 );
		pNode = static_cast<CVCBaseFolderNode*>(pNode)->GetHead();
		while( pNode )
		{
			// refresh this node and its children
// 			OnPropertyChanged( pNode, VSHPROPID_StateIconIndex, 0 );
			RecurseChildrenAndRefreshGlyphs( pNode );
			// get the next node
			pNode = pNode->GetNext();
		}
	}
	// normal case
	else
	{
		for( int i = 0; i < cAffectedNodes; i++ )
		{
			pNode = NULL;
			hr = VSITEMID2Node( rgitemidAffectedNodes[i], &pNode );
			m_pCachedNode = pNode;
			m_nCachedIconType = rgsiNewGlyphs[i];

			OnPropertyChanged( pNode, VSHPROPID_StateIconIndex, 0 );
			m_pCachedNode = NULL;
			m_nCachedIconType = 0;
		}
	}

	return S_OK;
}

//---------------------------------------------------------------------------
// Interface: IVsSccProject2
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::SetSccLocation(
	/* [in] */ LPCOLESTR pszSccProjectName,
	/* [in] */ LPCOLESTR pszSccAuxPath,
	/* [in] */ LPCOLESTR pszSccLocalPath,
	/* [in] */ LPCOLESTR pszSccProvider)
{
	// get a project pointer
	CComPtr<CVCProjectNode> pVcNode = GetProjectNode();
	RETURN_ON_NULL(pVcNode);

	CComQIPtr<VCProject> pProject = pVcNode->m_dispkeyItem;
	// if this wasn't a project node, we shouldn't be here
	RETURN_ON_NULL2(pProject, E_NOINTERFACE);

	// store the props
	CComBSTR bstrSccProjectName = pszSccProjectName;
	CComBSTR bstrSccAuxPath = pszSccAuxPath;
	CComBSTR bstrSccLocalPath = pszSccLocalPath;
	CComBSTR bstrSccProvider = pszSccProvider;

	pProject->put_SccProjectName( bstrSccProjectName );
	pProject->put_SccAuxPath( bstrSccAuxPath );
	pProject->put_SccLocalPath( bstrSccLocalPath );
	pProject->put_SccProvider( bstrSccProvider );
	
	return S_OK;
}

//---------------------------------------------------------------------------
// Interface: IVsSccProject2
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetSccFiles(
	/* [in] */ VSITEMID vsitemid,
	/* [out] */ CALPOLESTR* pCaStringsOut,
	/* [out] */ CADWORD* pCaFlagsOut)
{
	// fill the arrays with full path names/flags for the item ids passed in

	CHECK_POINTER_NULL(pCaStringsOut);
	CHECK_POINTER_NULL(pCaFlagsOut);

	CVCNode*	pNode = NULL;
	CComBSTR bstrFullPath;
	HRESULT hr;

	hr = VSITEMID2Node( vsitemid, &pNode );
	CVCNode *pVcNode = static_cast<CVCNode*>(pNode);
	RETURN_ON_NULL2(pVcNode, E_UNEXPECTED);
	CComPtr<IDispatch> pDisp = pVcNode->m_dispkeyItem;

	pCaStringsOut->cElems = 0;
	pCaStringsOut->pElems = NULL;
	pCaFlagsOut->cElems = 0;
	pCaFlagsOut->pElems = NULL;

	// is the node a file node?
	CComQIPtr<VCFile> pFile = pDisp;
	if( pFile )
	{
		CComPtr<IDispatch> pDispParent;
		pFile->get_Parent(&pDispParent);
		CComQIPtr<VCFilter> pFilter = pDispParent;
		if (pFilter != NULL)
		{
		    VARIANT_BOOL bScc = VARIANT_TRUE;
		    pFilter->get_SourceControlFiles(&bScc);
		    if( bScc == VARIANT_FALSE )
				return S_OK;
		}

		hr = pFile->get_FullPath( &bstrFullPath );
		RETURN_ON_FAIL(hr);
		
		// CoTaskMemAlloc arrays
		pCaStringsOut->cElems = 1;
		pCaStringsOut->pElems = (LPOLESTR*)CoTaskMemAlloc( sizeof(LPOLESTR) );
		pCaFlagsOut->cElems = 1;
		pCaFlagsOut->pElems = (DWORD*)CoTaskMemAlloc( sizeof(DWORD) );
		// set arrays
		pCaStringsOut->pElems[0] = (LPOLESTR)CoTaskMemAlloc( (bstrFullPath.Length() + 1) * sizeof( OLECHAR ) );
		wcscpy( pCaStringsOut->pElems[0], bstrFullPath );
		pCaFlagsOut->pElems[0] = 0;

		return S_OK;
	}

	// is the node a project node?
	CComQIPtr<VCProject> pProj = pDisp;
	if( pProj )
	{
 		hr = pProj->get_ProjectFile( &bstrFullPath );
		RETURN_ON_FAIL(hr);

		// CoTaskMemAlloc arrays
		pCaStringsOut->cElems = 1;
		pCaStringsOut->pElems = (LPOLESTR*)CoTaskMemAlloc( pCaStringsOut->cElems * sizeof(LPOLESTR) );
		pCaFlagsOut->cElems = 1;
		pCaFlagsOut->pElems = (DWORD*)CoTaskMemAlloc( pCaFlagsOut->cElems * sizeof(DWORD) );
		// set arrays
		pCaStringsOut->pElems[0] = (LPOLESTR)CoTaskMemAlloc( (bstrFullPath.Length() + 1) * sizeof( OLECHAR ) );
		wcscpy( pCaStringsOut->pElems[0], bstrFullPath );
		pCaFlagsOut->pElems[0] = 0;

		return S_OK;
	}

	// other kinds of nodes don't have files in scc
	return S_OK;
}

//---------------------------------------------------------------------------
// Interface: IVsSccProject2
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetSccSpecialFiles( 
	/* [in] */ VSITEMID,
	/* [in] */ LPCOLESTR,
	/* [out] */ CALPOLESTR*,
	/* [out] */ CADWORD*)
{
	VSASSERT(false, "Zero flags above means this shouldn't be called, so GetSccSpecialFiles isn't needed.");
	return E_FAIL;
}

//---------------------------------------------------------------------------
// CVCArchy::RegisterSccProject
// Send our opaque source code control settings to the scc manager.
// This called just after opening a project.
//---------------------------------------------------------------------------
HRESULT CVCArchy::RegisterSccProject()
{
	// if we have an SccProjectName property, we're under scc, proceed
	// call the IVsSccManager2 impl to register the project
	
	HRESULT hr = S_OK;
	CComBSTR bstrSccProjectName, bstrSccAuxPath, bstrSccLocalPath, bstrSccProvider;

	CComPtr<CVCProjectNode> pVcNode = GetProjectNode();
	RETURN_ON_NULL2(pVcNode, E_UNEXPECTED);
	CComPtr<IDispatch> pDisp = pVcNode->m_dispkeyItem;
	CComQIPtr<VCProject> pProject = pDisp;
	RETURN_ON_NULL2(pProject, E_NOINTERFACE);

 	if( m_fRegisteredWithScc )
		return S_OK;	// already done

    CComPtr<IVsSccManager2> srpSccManager;
	if (GetBuildPkg())
		GetBuildPkg()->GetSccManager(&srpSccManager);
	RETURN_ON_NULL2(srpSccManager, S_FALSE);

	CComPtr<IVsSccProject2> spSccProj = VCQI_cast<IVsSccProject2>(this);
	pProject->get_SccProjectName( &bstrSccProjectName ),
	pProject->get_SccAuxPath( &bstrSccAuxPath ),
	pProject->get_SccLocalPath( &bstrSccLocalPath ),
	pProject->get_SccProvider( &bstrSccProvider ),
 	hr = srpSccManager->RegisterSccProject(spSccProj, bstrSccProjectName, bstrSccAuxPath, bstrSccLocalPath, bstrSccProvider);
	m_fRegisteredWithScc = true;

	return hr;
}

HRESULT CVCArchy::UnregisterSccProject()
{
    if (!m_fRegisteredWithScc)
		return S_OK;	// nothing to do

    CComPtr<IVsSccManager2> srpSccManager;
	if (GetBuildPkg())
		GetBuildPkg()->GetSccManager(&srpSccManager);
	RETURN_ON_NULL2(srpSccManager, S_FALSE);
	m_fRegisteredWithScc = false;

    return srpSccManager->UnregisterSccProject(this);
}

STDMETHODIMP CVCArchy::ReadUserOptions(IStream *pStream, LPCOLESTR pszKey)
{
	CHECK_READ_POINTER_NULL(pStream);
		
	CComPtr<VCProject> pProject;
	GetVCProject( NULL, &pProject );
	RETURN_ON_NULL2(pProject, E_NOINTERFACE);

	CComPtr<IUnknown> spStreamUnk;
	pStream->QueryInterface(IID_IUnknown, (void **) &spStreamUnk);
	pProject->LoadProjectOptions(spStreamUnk);
	return S_OK;
}

STDMETHODIMP CVCArchy::WriteUserOptions(IStream *pStream, LPCOLESTR pszKey)
{
	CHECK_READ_POINTER_NULL(pStream);

	CComPtr<VCProject> pProject;
	GetVCProject( NULL, &pProject );
	RETURN_ON_NULL2(pProject, E_NOINTERFACE);

	CComPtr<IUnknown> spStreamUnk;
	pStream->QueryInterface(IID_IUnknown, (void **) &spStreamUnk);
	pProject->SaveProjectOptions(spStreamUnk);

	return S_OK;
}

STDMETHODIMP CVCArchy::ParseCanonicalName(LPCOLESTR pszName, VSITEMID *pitemid)
{
	CHECK_POINTER_NULL(pitemid);

	*pitemid = ItemIDFromName(NULL, pszName);
	RETURN_ON_NULL(*pitemid);
	return S_OK;
}

HRESULT CVCArchy::InformConfigurationAdded( LPOLESTR pszCfgName )
{
	if (m_pCfgProviderEventsHelper)
 		return m_pCfgProviderEventsHelper->NotifyOnCfgNameAdded( pszCfgName );
	else
		return S_OK;
}

HRESULT CVCArchy::InformConfigurationRemoved( LPOLESTR pszCfgName )
{
	if (m_pCfgProviderEventsHelper)
 		return m_pCfgProviderEventsHelper->NotifyOnCfgNameDeleted( pszCfgName );
	else
		return S_OK;
}

HRESULT CVCArchy::InformConfigurationRenamed( LPOLESTR pszOldCfgName, LPOLESTR pszNewCfgName )
{
	if (m_pCfgProviderEventsHelper)
 		return m_pCfgProviderEventsHelper->NotifyOnCfgNameRenamed( pszOldCfgName, pszNewCfgName );
	else
		return S_OK;
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Called to set the IServerProvider site. 
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::SetSite(
			/* [in] */ IServiceProvider *pSP)
{
	VSASSERT(NULL != pSP, "Trying to SetSite without a service provider!");
	CHECK_READ_POINTER_NULL(pSP);
	m_srpServiceProvider = pSP;

	HRESULT hr;
	hr = pSP->QueryService(SID_SVsShellMonitorSelection,
							IID_IVsMonitorSelection,
							(void **)&m_srpMonSel);
	VSASSERT(SUCCEEDED(hr), "Failed QueryService for SID_SVsShellMonitorSelection");
	VSASSERT(m_srpMonSel != NULL, "Failed to pick up IVsMonitorSelection pointer");
	return hr;
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Called to get the IServerProvider site.
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetSite(
			/* [out] */ IServiceProvider **ppSP)
{
	CHECK_POINTER_NULL(ppSP);
	return m_srpServiceProvider.CopyTo(ppSP);
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// 
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::QueryClose(BOOL *pfCanClose)
{
	CHECK_POINTER_NULL(pfCanClose);
	*pfCanClose = TRUE;

	CVCProjectNode* pProjNode = GetProjectNode();
	RETURN_ON_NULL2(pProjNode, S_OK);

	CComPtr<IVCGenericConfiguration> spGenCfg;
	HRESULT hr = pProjNode->GetActiveConfig(&spGenCfg);
	RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, spGenCfg, S_OK);

	BOOL bNoBuild = TRUE;
	spGenCfg->get_NoBuildIsInProgress(&bNoBuild);
	if (bNoBuild)
		return S_OK;

	// if we got here, then we're doing a build and we cannot shut down in that case
	*pfCanClose = FALSE;
	return E_FAIL;
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Returns the guidType for the itemid.
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetGuidProperty(
			/* [in] */ VSITEMID itemid,
			/* [in] */ VSHPROPID propid,
			/* [out] */ GUID *pguid)
{
	CHECK_POINTER_NULL(pguid);
	*pguid = IID_NULL;

	CVCNode *pNode = NULL;
	HRESULT hr = VSITEMID2Node(itemid, &pNode);
	RETURN_ON_NULL2(pNode, E_UNEXPECTED);

	return pNode->GetGuidProperty(propid, pguid);
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Returns the guidType for the itemid.
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::SetGuidProperty(/* [in] */ VSITEMID itemid, /* [in] */ VSHPROPID propid, /* [in] */ REFGUID guid)
{
	CVCNode *pNode = NULL;
	HRESULT hr = VSITEMID2Node(itemid, &pNode);
	RETURN_ON_NULL2(pNode, E_UNEXPECTED);

	return pNode->SetGuidProperty(propid, guid);
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Returns the nested hierarchy for item id. 
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetNestedHierarchy(/* [in] */ VSITEMID itemid, /* [in] */ REFIID iidHierarchyNested,
	/* [iid_is][out] */ void  **ppHierarchyNested, /* [out] */ VSITEMID *pitemidNested)
{
	return E_NOTIMPL;	// CVCArchy::GetNestedHierarchy
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Returns the canonical name for itemid.
//
// For web hierarchies this is its absolute url?
// For db hierarchies this is??
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetCanonicalName(
			/* [in] */ VSITEMID itemid,
			/* [out] */ BSTR *pbstrName)
{
	CHECK_POINTER_NULL(pbstrName);

	CVCNode*	pNode = NULL;
	HRESULT		hr = VSITEMID2Node(itemid, &pNode);
	VSASSERT(pNode != NULL, "Failed to convert itemid to hier node!");
	RETURN_ON_FAIL_OR_NULL(hr, pNode);

	return pNode->GetCanonicalName(pbstrName);
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Returns our toplevel project automation object
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetAutomationObject(/* [out] */ IDispatch  **ppdisp)
{
	CHECK_POINTER_VALID(ppdisp);
	*ppdisp = NULL;

	return E_NOTIMPL;	// CVCArchy::GetAutomationObject
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::AdviseHierarchyEvents(
			/* [in] */ IVsHierarchyEvents *pEventSink,
			/* [out] */ VSCOOKIE *pdwCookie)
{
	CHECK_READ_POINTER_NULL(pEventSink);
	CHECK_POINTER_NULL(pdwCookie);

	EventSink *pSink = new EventSink;
	RETURN_ON_NULL2(pSink, E_OUTOFMEMORY);

	pSink->m_srpEventSink = pEventSink;
	 
	 ++g_dwCookie;
	pSink->m_dwCookie = (DWORD) g_dwCookie;
	*pdwCookie = g_dwCookie;

	VSASSERT(*pdwCookie, "VsShell doesn't like cookies of 0");

	m_rgEventSinks.Add(pSink);
	return S_OK;
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::UnadviseHierarchyEvents(
			/* [in] */ VSCOOKIE dwCookie)
{
	int n = m_rgEventSinks.GetSize();
	for (int i = 0; i < n; ++i)
	{
		if (m_rgEventSinks[i]->m_dwCookie == dwCookie)
		{
			delete m_rgEventSinks[i];
			m_rgEventSinks.RemoveAt(i);
			return S_OK;
		}
	}
	VSASSERT(0 != dwCookie && dwCookie <= g_dwCookie, "unadvising an unknown cookie!"); 
	VSASSERT(0 == dwCookie || g_dwCookie < dwCookie, "unadvising a cookie twice!");
	return E_FAIL;
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Allows for a binary-comapatible change.	Needed since IVsUIHierarchy 
// derrives from IVsHierarchy
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::Unused0(void)
{
	return E_NOTIMPL;	// CVCArchy::Unused0
}


//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Allows for a binary-comapatible change.	Needed since IVsUIHierarchy 
// derrives from IVsHierarchy
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::Unused1(void)
{
	return E_NOTIMPL;	// CVCArchy::Unused1
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Allows for a binary-comapatible change.	Needed since IVsUIHierarchy 
// derrives from IVsHierarchy
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::Unused2(void)
{
	return E_NOTIMPL;	// CVCArchy::Unused2
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Allows for a binary-comapatible change.	Needed since IVsUIHierarchy 
// derrives from IVsHierarchy
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::Unused3(void)
{
	return E_NOTIMPL;	// CVCArchy::Unused3
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Allows for a binary-comapatible change.	Needed since IVsUIHierarchy 
// derrives from IVsHierarchy
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::Unused4(void)
{
	return E_NOTIMPL;	// CVCArchy::Unused4
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy helper function
// Returns the requested property for the CVCNode.
// There are several of properties handled on the VsHierarchy level
//---------------------------------------------------------------------------
HRESULT CVCArchy::GetProperty(
			/* [in] */ CVCNode *pNode,
			/* [in] */ VSHPROPID propid,
			/* [out] */ VARIANT *pvar)
{
	VSASSERT(!IsZombie(), "Calling GetProperty when zombied!");
	// CHECK_READ_POINTER_NULL(pNode);
	// CHECK_POINTER_NULL(pvar);

	HRESULT hr = S_OK;
	switch (propid)
	{
	case VSHPROPID_Parent:
		V_VT(pvar) = VT_INT_PTR;
		V_INT_PTR(pvar) = (pNode->GetParent() != NULL) ? pNode->GetParent()->GetVsItemID() : VSITEMID_NIL;
		break;

	case VSHPROPID_NextSibling:
		V_VT(pvar) = VT_INT_PTR;
		V_INT_PTR(pvar) = (pNode->GetNext() != NULL) ? pNode->GetNext()->GetVsItemID() : VSITEMID_NIL;
		break;

	case VSHPROPID_Root:			// VsHierarchy property
		V_VT(pvar) = VT_INT_PTR;	// this is useless according to spec
		V_INT_PTR(pvar) = VSITEMID_ROOT;
		break;

	case VSHPROPID_IconImgList:		// VsHierarchy property
		V_VT(pvar) = VT_INT_PTR;
		V_INT_PTR(pvar) = (INT_PTR)(GetImageList());
		break;

	case VSHPROPID_IconIndex:		// VsHierarchy derived property
		V_VT(pvar) = VT_I4;
		V_I4(pvar) = static_cast<INT>(GetIconIndex(pNode, ICON_Closed));
		break;

	case VSHPROPID_OpenFolderIconIndex:
		V_VT(pvar) = VT_I4;
		V_I4(pvar) = static_cast<INT>(GetIconIndex(pNode, ICON_Open));
		break;

	case VSHPROPID_SelContainer:
		// REVIEW: should we return E_NOINTERFACE
		hr = E_NOINTERFACE;
		break;

	case VSHPROPID_StateIconIndex:
		V_VT(pvar) = VT_I4;
		V_I4(pvar) = static_cast<INT>(GetIconIndex(pNode, ICON_StateImage));
		break;
		
	case VSHPROPID_ShowProjInSolutionPage:
		V_VT(pvar) = VT_BOOL;
		V_BOOL(pvar) = VARIANT_TRUE;
		break;


	case VSHPROPID_ParentHierarchy:
		hr = S_OK;
//		VSASSERT(pNode->GetVsItemID() == VSITEMID_ROOT, "Parent hierarchy only available for root node!");
		V_VT(pvar) = VT_UNKNOWN;
		V_UNKNOWN(pvar) = m_pParentHierarchy;
		if (NULL != V_UNKNOWN(pvar))
			V_UNKNOWN(pvar)->AddRef();
		break;

	case VSHPROPID_ParentHierarchyItemid:
		hr = S_OK;
//		VSASSERT(pNode->GetVsItemID() == VSITEMID_ROOT, "Parent hierarchy itemid only available for root node!");
		V_VT(pvar) = VT_INT_PTR;
		V_INT_PTR(pvar) = m_dwParentHierarchyItemid;
		break;

	default:
		return pNode->GetProperty(propid, pvar);
	}
	return hr;
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Returns the requested property for the itemid.
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetProperty( 
			/* [in] */ VSITEMID itemid,
			/* [in] */ VSHPROPID propid,
			/* [out] */ VARIANT *pvar)
{
	CHECK_POINTER_NULL(pvar);
	VariantInit(pvar);

	CVCNode *pNode = NULL;
	HRESULT hr = VSITEMID2Node(itemid, &pNode);
	VSASSERT(NULL != pNode, "Failed to convert itemid to hierarchy node!");
	RETURN_ON_FAIL_OR_NULL(hr, pNode);
	
	return GetProperty(pNode, propid, pvar);
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Set's the given property on the item specified by itemid. 
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::SetProperty(
			/* [in] */ VSITEMID itemid,
			/* [in] */ VSHPROPID propid,
			/* [in] */ VARIANT var)
{
	CVCNode *pNode = NULL;
	HRESULT hr = VSITEMID2Node(itemid, &pNode);
	VSASSERT(NULL != pNode, "Failed to convert itemid to hier node!");
	RETURN_ON_FAIL_OR_NULL(hr, pNode);

	return SetProperty(pNode, propid, var);
}

//---------------------------------------------------------------------------
// interface: IVsHierarchy
// Set's the given property on the item specified by itemid. 
//---------------------------------------------------------------------------
HRESULT CVCArchy::SetProperty(
			/* [in] */ CVCNode *pNode,
			/* [in] */ VSHPROPID propid,
			/* [in] */ const VARIANT& var)
{
	CHECK_READ_POINTER_NULL(pNode);
	VSASSERT(!IsZombie(), "SetProperty called on zombied object!");

	HRESULT hr = E_INVALIDARG;

	switch (propid)
	{
	case VSHPROPID_ExpandByDefault:		// CVCBaseFolderNode property
	case VSHPROPID_Expanded:
		pNode->SetProperty(propid, var);
		break;

	case VSHPROPID_Caption:			// CVCNode property
		VSASSERT(VT_BSTR == V_VT(&var) && ::SysStringLen(V_BSTR(&var)), "Invalid parameter type passed!");
		if (VT_BSTR == V_VT(&var) && ::SysStringLen(V_BSTR(&var)))
		{	// don't allow empty string setting
			pNode->SetCaption(V_BSTR(&var));
			OnPropertyChanged(pNode, propid, 0);
		}
		break;

	case VSHPROPID_ParentHierarchy:
		VSASSERT(VT_UNKNOWN == V_VT(&var), "Invalid parameter type passed!");
		if (VT_UNKNOWN == V_VT(&var))
		{
			m_pParentHierarchy = V_UNKNOWN(&var);	// NOTE: hold as UNADDREF'ed pointer
			hr = S_OK;
		}
		break;

	case VSHPROPID_ParentHierarchyItemid:
		VSASSERT(VT_INT_PTR == V_VT(&var), "Invalid parameter type passed!");
		if (VT_INT_PTR == V_VT(&var))
		{
			m_dwParentHierarchyItemid = V_INT_PTR(&var);
			hr = S_OK;
		}
		break;

	default:
		hr = pNode->SetProperty(propid, var);
	}
	return	hr;
}

//---------------------------------------------------------------------------
// interface: IVsUIHierarchy
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::QueryStatusCommand( 
			/* [in] */		  VSITEMID	   itemid, 
			/* [in, unique] */ const GUID * pguidCmdGroup,
			/* [in] */		  ULONG cCmds,
			/* [in, out, size_is(cCmds)] */ OLECMD prgCmds[],
			/* [in, out, unique] */ OLECMDTEXT *pCmdText)
{
	HierNodeList rgNodeList;
	HRESULT hr = VSITEMID2Node(itemid, rgNodeList);
	RETURN_ON_FAIL(hr);

	return QueryStatusSelection(pguidCmdGroup, cCmds, prgCmds, pCmdText, rgNodeList, TRUE);
}

//---------------------------------------------------------------------------
// interface: IVsUIHierarchy
// Called to execute the cmdid from the guidCmdSet on the itemid. If itemid = 
// VSITEMID_SELECTED (the normal case), the command is applied to the selected
// items.
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::ExecCommand( 
			/* [in] */		  VSITEMID	   itemid, 
			/* [in, unique] */ const GUID * pguidCmdGroup,
			/* [in] */		  DWORD		   nCmdID,
			/* [in] */		  DWORD		   nCmdexecopt,
			/* [in, unique] */ VARIANT *	pvaIn,
			/* [in, out, unique] */ VARIANT* pvaOut)
{
	CVCNode *pNode = NULL;
	HierNodeList rgNodeList;
	HRESULT hr = VSITEMID2Node(itemid, rgNodeList);
	RETURN_ON_FAIL2(hr, OLECMDERR_E_NOTSUPPORTED);
	
	// Handle std hierarchy commands here
	if(*pguidCmdGroup == GUID_VsUIHierarchyWindowCmds)
	{
		switch(nCmdID)
		{
			case UIHWCMDID_RightClick:
				// NOTE: Normally the IOleCommandTarget for the IVsHierarchy is not allowed to 
				// handle commands like Cut, Copy, Paste, Delete, Renmame during the current
				// hierarchy point in the command routing. These type of commands should only
				// apply to the object with focus. For example, it is very undesierable if 
				// executing the Edit.Delete command while focus is in a document window was
				// handled by the project to delete the select project item (i.e. the open 
				// document). For a hierarchy these commands are normally processed via
				// IVsUIHierarchy::ExecCommand. But in the case of bringing up a context menu
				// we know that the hierarchy has focus, thus these commands are valid. In order
				// to communicate this fact to our IOleCommandTarget implementation we set
				// the "m_fInContextMenu" flag.
				hr = DisplayContextMenu(rgNodeList,pvaIn);
				break;
			case UIHWCMDID_DoubleClick:
			case UIHWCMDID_EnterKey:
				do
				{
					pNode = rgNodeList.RemoveHead();
					hr = pNode->DoDefaultAction(FALSE);
				} while(SUCCEEDED(hr) && !rgNodeList.IsEmpty());
				break;
			default:
				hr = OLECMDERR_E_NOTSUPPORTED;
				break;
		}
	}
	else
	{	// Pass command to nodes Exec command
		hr = ExecSelection(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut, rgNodeList, TRUE);

	}

	return hr;
}

//---------------------------------------------------------------------------
// interface: IVsPersistHierarchyItem
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::IsItemDirty(
		/* [in] */ VSITEMID,	//vsid,
		/* [in] */ IUnknown *punkDocData,
		/* [out] */ BOOL *pfDirty)
{
	CHECK_READ_POINTER_NULL(punkDocData);
	CHECK_POINTER_NULL(pfDirty);
	*pfDirty = FALSE;

	CComQIPtr<IVsPersistDocData> srpPersistDocData;
	HRESULT hr = punkDocData->QueryInterface(IID_IVsPersistDocData, (void**)&srpPersistDocData);
	RETURN_ON_FAIL_OR_NULL(hr, srpPersistDocData);

	return srpPersistDocData->IsDocDataDirty(pfDirty);
}

//---------------------------------------------------------------------------
// interface: IVsPersistHierarchyItem
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::SaveItem(
		/* [in] */ VSSAVEFLAGS dwSave,
		/* [in] */ LPCOLESTR lpstrMkDocument,
		/* [in] */ VSITEMID itemid,
		/* [in] */ IUnknown *punkDocData,
		/* [out] */ BOOL *pfCanceled)
{
	CHECK_READ_POINTER_NULL(punkDocData);
	CHECK_POINTER_NULL(pfCanceled);

	HRESULT hr = S_OK;

	CComQIPtr<IVsPersistDocData> srpPersistDocData;
	srpPersistDocData = punkDocData;
	RETURN_ON_NULL(srpPersistDocData);

	CComBSTR bstrMkDocumentNew;
	hr = srpPersistDocData->SaveDocData(dwSave, &bstrMkDocumentNew, pfCanceled);

	if (FAILED(hr) || dwSave != VSSAVE_SaveAs)
		return hr;

	// get the document window frame
	CComPtr<IVsUIShellOpenDocument> pOpenDoc;
	hr = ExternalQueryService( SID_SVsUIShellOpenDocument, IID_IVsUIShellOpenDocument, (void **)&pOpenDoc );
	RETURN_ON_FAIL_OR_NULL(hr, pOpenDoc);

	BOOL bIsOpen;
	CComPtr<IVsUIHierarchy> spHier = VCQI_cast<IVsUIHierarchy>(this);
	CComPtr<IVsWindowFrame> pWindowFrame;
	hr = pOpenDoc->IsDocumentOpen( spHier, itemid, lpstrMkDocument, GUID_NULL, IDO_IgnoreLogicalView, NULL, NULL, &pWindowFrame, &bIsOpen );
	RETURN_ON_FAIL_OR_NULL(hr, pWindowFrame);

	BOOL bFound;
	VSITEMID itemidFound = VSITEMID_NIL;
	CComPtr<IVsUIHierarchy> pHierarchy;
	CComQIPtr<IVsProject3> pProject3;

	// is this file in a project?
	hr = pOpenDoc->IsDocumentInAProject( bstrMkDocumentNew, &pHierarchy, &itemidFound, NULL, &bFound );
	if( bFound )
	{
		pProject3 = pHierarchy;
		if( !pProject3 )
			return E_FAIL;
		// transfer the document to that project
		hr = pProject3->TransferItem( lpstrMkDocument, bstrMkDocumentNew, pWindowFrame );
	}
	else
	{
		// obtain external files manager and transfer the document
		CComPtr<IVsExternalFilesManager> pExtFilesMgr;
		hr = ExternalQueryService( SID_SVsExternalFilesManager, IID_IVsExternalFilesManager, (void **)&pExtFilesMgr);
		RETURN_ON_FAIL_OR_NULL(hr, pExtFilesMgr);
		// transfer the document to the external files project
		hr = pExtFilesMgr->TransferDocument( lpstrMkDocument, bstrMkDocumentNew, pWindowFrame );
	}

	return hr;
}

//---------------------------------------------------------------------------
// interface: IOleCommandTarget
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::QueryStatus( 
			/* [unique][in] */ const GUID *pguidCmdGroup,
			/* [in] */ ULONG cCmds,
			/* [out][in][size_is] */ OLECMD prgCmds[  ],
			/* [unique][out][in] */ OLECMDTEXT *pCmdText)
{
	VSASSERT(GetRootNode() != NULL, "No root node!");
	RETURN_ON_NULL2(m_srpMonSel, E_UNEXPECTED);

	// Because QueryStatus() is called OFTEN(!) and the selection is almost
	// always a single item, we check for that case FIRST in the following inline code

	// find out if we have a single selection or a multi-selection
	HRESULT hr;
	VSITEMID itemid;						// if VSITEMID_SELECTION then multiselection
	CVCNode *pNode = NULL;
	CComPtr<IVsHierarchy> srpIVsHierarchy;	// if NULL then selection spans VsHierarchies
	CComPtr<IVsMultiItemSelect> srpIVsMultiItemSelect;
	CComPtr<ISelectionContainer> srpISelectionContainer;		// unused?
	CComPtr<IVsHierarchy> spThisHier = VCQI_cast<IVsHierarchy>(this);

	hr = m_srpMonSel->GetCurrentSelection(&srpIVsHierarchy, &itemid, &srpIVsMultiItemSelect, &srpISelectionContainer);
	RETURN_ON_FAIL(hr);

	// if we're not selected, but the shell still calls us (this does happen)
	if (srpIVsHierarchy != NULL && srpIVsHierarchy != spThisHier)
		return E_UNEXPECTED;
	// nothing selected
	if (VSITEMID_NIL == itemid)
	{
		VSASSERT(SUCCEEDED(hr), "Nothing selected!");
		return hr;
	}
	// single selection
	if (VSITEMID_SELECTION != itemid)
	{
		VSASSERT(srpIVsHierarchy == spThisHier, "Hierarchy isn't what we expected!");	// shouldn't be NULL

		hr = VSITEMID2Node(itemid, &pNode);
		if( FAILED( hr ) )
			return hr;
		// QueryStatus on this node only
		hr = pNode->QueryStatus( pguidCmdGroup, cCmds, prgCmds, pCmdText );
	}
	// multi-selection
	else
	{
		HierNodeList rgNodes;

		if (NULL == pguidCmdGroup)
		{
			hr = OLECMDERR_E_UNKNOWNGROUP;
		}
		else 
		{
			hr = GetSelectedNodes(rgNodes);
			if (SUCCEEDED(hr))
			{
				hr = QueryStatusSelection(pguidCmdGroup, cCmds, prgCmds, pCmdText, rgNodes, FALSE);
			}
			else
			{
				// Its safer to return OLECMDERR_E_NOTSUPPORTED to shell rather than return failure
				// indicating that we actually handle the command.
				hr = OLECMDERR_E_NOTSUPPORTED;
			}
		}
	}

	return hr;
}

//---------------------------------------------------------------------------
// interface: IOleCommandTarget
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::Exec( 
			/* [unique][in] */ const GUID *pguidCmdGroup,
			/* [in] */ DWORD nCmdID,
			/* [in] */ DWORD nCmdexecopt,
			/* [unique][in] */ VARIANT *pvaIn,
			/* [unique][out][in] */ VARIANT *pvaOut)
{
	VSASSERT(GetRootNode() != NULL, "No root node!");

	RETURN_ON_NULL2(pguidCmdGroup, OLECMDERR_E_UNKNOWNGROUP);

	HierNodeList rgNodes;
	HRESULT hr = GetSelectedNodes(rgNodes);
	RETURN_ON_FAIL2(hr, OLECMDERR_E_NOTSUPPORTED);	// Its safer to return OLECMDERR_E_NOTSUPPORTED to shell rather than 
													// return failure indicating that we actually handle the command.

	return ExecSelection(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut, rgNodes, FALSE);
}

//---------------------------------------------------------------------------
// call OnItemAdded for each listed IVsHierarchyEvent sink
//---------------------------------------------------------------------------
HRESULT CVCArchy::OnItemAdded(CVCNode *pNodeParent, CVCNode *pNodePrev, CVCNode *pNodeAdded) const
{
	CHECK_READ_POINTER_NULL(pNodeParent);
	CHECK_READ_POINTER_NULL(pNodeAdded);
	VSITEMID itemidParent = pNodeParent->GetVsItemID();
	VSITEMID itemidSiblingPrev = (NULL != pNodePrev) ? pNodePrev->GetVsItemID(): VSITEMID_NIL;
	VSITEMID itemidAdded = pNodeAdded->GetVsItemID();

	int n = m_rgEventSinks.GetSize();
	for (int i = 0; i < n; ++i)
	{
		HRESULT hr = E_FAIL;
		if (m_rgEventSinks[i]->m_srpEventSink)
			hr = m_rgEventSinks[i]->m_srpEventSink->OnItemAdded(itemidParent, itemidSiblingPrev, itemidAdded);
		VSASSERT(SUCCEEDED(hr), "Failed during OnItemAdded!");
	}
	return S_OK;
}

//---------------------------------------------------------------------------
// call OnItemDeleted for each listed IVsHierarchyEvent sink
//
// ASSUMPTION: pNode may be in the middle of the destructor
//			   but CVCNode::GetVsItemID okay to call
//---------------------------------------------------------------------------
HRESULT CVCArchy::OnItemDeleted(CVCNode *pNode) const
{
	RETURN_ON_NULL(pNode);

	VSITEMID itemid = pNode->GetVsItemID();
	int n = m_rgEventSinks.GetSize();
	for (int i = 0; i < n; ++i)
	{
		HRESULT hr = E_FAIL;
		if (m_rgEventSinks[i]->m_srpEventSink)
			hr = m_rgEventSinks[i]->m_srpEventSink->OnItemDeleted(itemid);
		VSASSERT(SUCCEEDED(hr), "Failed during OnItemDeleted!");
	}
	return S_OK;
}

//---------------------------------------------------------------------------
// call OnPropertyChanged for each listed IVsHierarchyEvent sink
//---------------------------------------------------------------------------
HRESULT CVCArchy::OnPropertyChanged(CVCNode *pNode, VSHPROPID propid, DWORD flags) const
{
	VSITEMID itemid = pNode->GetVsItemID();
	int n = m_rgEventSinks.GetSize();
	for (int i = 0; i < n; ++i)
	{
		HRESULT hr = E_FAIL;
		if (m_rgEventSinks[i]->m_srpEventSink)
			hr = m_rgEventSinks[i]->m_srpEventSink->OnPropertyChanged(itemid, propid, flags);
		VSASSERT(SUCCEEDED(hr), "Failed during OnPropertyChanged!");
	}
	return S_OK;
}

//---------------------------------------------------------------------------
// call OnInvalidateItems for each listed IVsHierarchyEvent sink
//
// ASSUMPTION: pNode may be in the middle of the destructor
//			   but CVCNode::GetVsItemID okay to call
//---------------------------------------------------------------------------
HRESULT CVCArchy::OnInvalidateItems(CVCNode *pNode) const
{
	VSITEMID itemid = pNode->GetVsItemID();
	int n = m_rgEventSinks.GetSize();
	for (int i = 0; i < n; ++i)
	{
		HRESULT hr = E_FAIL;
		if (m_rgEventSinks[i]->m_srpEventSink)
			hr = m_rgEventSinks[i]->m_srpEventSink->OnInvalidateItems(itemid);
		VSASSERT(SUCCEEDED(hr), "Failed during OnInvalidateItems!");
	}
	return S_OK;
}

//---------------------------------------------------------------------------
// call OnInvalidateIcon for each listed IVsHierarchyEvent sink
//---------------------------------------------------------------------------
HRESULT CVCArchy::OnInvalidateIcon(HICON hIcon) const
{
	int n = m_rgEventSinks.GetSize();
	for (int i = 0; i < n; ++i)
	{
		HRESULT hr = E_FAIL;
		if (m_rgEventSinks[i]->m_srpEventSink)
			hr = m_rgEventSinks[i]->m_srpEventSink->OnInvalidateIcon(hIcon);
		VSASSERT(SUCCEEDED(hr), "Failed during OnInvalidateIcon!");	// annoying for other uses, but still needed to find why
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// CVCArchy::DisplayContextMenu
//-----------------------------------------------------------------------------
HRESULT CVCArchy::DisplayContextMenu(HierNodeList &rgSelection, VARIANT *pvaIn)
{
	// if the delete command is called, we could be destroyed
	// when 'm_bInContextMenu = FALSE;' is called
	CComPtr<IVsHierarchy> srpSafety = VCQI_cast<IVsHierarchy>(this);

	CVCNode *pNode = NULL;

	if (rgSelection.GetCount() == 1)
	{
		pNode = rgSelection.GetHead();
		VSASSERT(pNode, "Selection contains a NULL head even though there's an element in the list!");
		if (pNode)
			pNode->DisplayContextMenu(pvaIn);
    }
	// handle multiple selected nodes
	else if (rgSelection.GetCount() > 1)
	{
		VCPOSITION pos;
		unsigned int firstKind;
		bool bNodeTypesDiffer = false;

		// determine if the selection in homogenous
		pos = rgSelection.GetHeadPosition();
		VSASSERT( pos, "Selection count differs from actual contents" );
		pNode = rgSelection.GetNext( pos );
		firstKind = pNode->GetKindOf();
		while( pos != NULL )
		{
			pNode = rgSelection.GetNext( pos );
			if( pNode->GetKindOf() != firstKind )
				bNodeTypesDiffer = true;
			VSASSERT(pNode, "Selection contains a NULL item in the list!");
		}
		VSASSERT(pNode, "Selection contains a NULL item in the list!");

		// display the appropriate menu
		if( bNodeTypesDiffer )
		{
			CComPtr<IOleCommandTarget> spTarget = VCQI_cast<IOleCommandTarget>(this);
			static_cast<CVCNode*>(pNode)->ShowContextMenu( IDM_VS_CTXT_XPROJ_MULTIITEM, pvaIn, guidSHLMainMenu,	 spTarget);
		}
		else
			pNode->DisplayContextMenu(pvaIn);
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// CVCArchy::QueryStatusSelection
//-----------------------------------------------------------------------------
HRESULT CVCArchy::QueryStatusSelection(
	const GUID *pguidCmdGroup,
	ULONG cCmds,
	OLECMD prgCmds[],
	OLECMDTEXT *pCmdText,
	HierNodeList &rgSelection,
	BOOL		 bIsHierCmd)		// TRUE if cmd originated via CVSUiHierarchy::ExecCommand
{
	HRESULT hr = S_OK;
	OLECMD cmdTmp;
	bool bSupported = false;
	bool bEnabled = true;
	bool bInvisible = false;
	bool bLatched = true;
	
	VSASSERT( pguidCmdGroup, "NULL command group GUID!" );
	VSASSERT( cCmds == 1, "We don't know how to handle situation where more than one command is being asked after" );

	for( VCPOSITION pos = rgSelection.GetHeadPosition(); pos != NULL && SUCCEEDED( hr ); )
	{
		cmdTmp.cmdID = prgCmds[0].cmdID;
		cmdTmp.cmdf = 0;
		CVCNode *pNode = rgSelection.GetNext( pos );
		hr = pNode->QueryStatus( pguidCmdGroup, cCmds, &cmdTmp, pCmdText );
		if( SUCCEEDED( hr ) || hr == OLECMDERR_E_NOTSUPPORTED )
		{			
			// cmd is supported if any node supports cmd
			// cmd is enabled if all nodes enable cmd
			// cmd is invisible if any node sets invisibility
			// cmd is latched only if all are latched
			bSupported = bSupported || (cmdTmp.cmdf & OLECMDF_SUPPORTED);
			bEnabled = bEnabled && (cmdTmp.cmdf & OLECMDF_ENABLED);
			bInvisible = bInvisible || (cmdTmp.cmdf & OLECMDF_INVISIBLE);
			bLatched = bLatched && (cmdTmp.cmdf & OLECMDF_LATCHED);
			// optimization
			if( !bSupported || bInvisible )
				break;
		}
		else
		{
			bSupported = false;
			bEnabled = false;
			bInvisible = false;
			bLatched = false;
		}
	}
	prgCmds[0].cmdf = 0;

	if( bSupported )
		prgCmds[0].cmdf |= OLECMDF_SUPPORTED;
	if( bEnabled )
		prgCmds[0].cmdf |= OLECMDF_ENABLED;
	if( bInvisible )
		prgCmds[0].cmdf |= OLECMDF_INVISIBLE;
	if( bLatched )
		prgCmds[0].cmdf |= OLECMDF_LATCHED;

	return hr;
}

//-----------------------------------------------------------------------------
// CVCArchy::ExecSelection
//-----------------------------------------------------------------------------
HRESULT CVCArchy::ExecSelection(
	const GUID *pguidCmdGroup,
	DWORD nCmdID,
	DWORD nCmdexecopt,
	VARIANT *pvaIn,
	VARIANT *pvaOut,
	HierNodeList &rgSelection,
	BOOL		 bIsHierCmd)	// TRUE if cmd originated via CVSUiHierarchy::ExecCommand
{
	VSASSERT(NULL != GetProjectNode(), "No project node!  Is this hierarchy zombie?");
	CHECK_ZOMBIE(GetProjectNode(), IDS_ERR_PROJ_ZOMBIE);

	if (*pguidCmdGroup == guidVSStd2K)
	{
		if (nCmdID == ECMD_COMPILE)
			return CompileFileItems(rgSelection);
	}
	else if (*pguidCmdGroup == guidVSStd97)
	{
		if (nCmdID == cmdidCut)
			return OnCommandCut();
		else if (nCmdID == cmdidCopy)
			return OnCommandCopy();
	}

	HRESULT hr = S_OK;
	VCPOSITION pos = rgSelection.GetHeadPosition();
	if (pos == NULL)
		hr = OLECMDERR_E_NOTSUPPORTED;
	if( SUCCEEDED(hr) )
	{
		HRESULT hr2 = S_OK;
		while( pos != NULL )
		{
			CVCNode *pNode = rgSelection.GetNext(pos);
			//$UNDONE: This enables a superset of commands - not the intersection
			//$CONSIDER: What is correct behavior if command fails or is canceled?
			if (FAILED(hr2 = pNode->Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut)))
			{
				// record the failure code, but continue with the exec.
				hr = hr2;
			}
		}
	}
	return hr;
}

CVCNode* CVCArchy::ResolveProjNode(CVCNode* pNode)
{
	CVCNode* pTmpNode = NULL;
	if (pNode)
		pTmpNode = pNode;
	else
	{
		HRESULT hr = VSITEMID2Node(VSITEMID_ROOT, &pTmpNode);
		RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, pTmpNode, NULL);
	}
	return static_cast<CVCNode*>(pTmpNode);
}

HRESULT CVCArchy::GetVCProject(CVCNode* pNode, VCProject** ppProject)
{
	CHECK_POINTER_NULL(ppProject);
	*ppProject = NULL;

	CVCNode* pProj = ResolveProjNode(pNode);
	RETURN_ON_NULL(pProj);

	pProj->GetVCProject(ppProject);
	return S_OK;
}

HRESULT CVCArchy::GetDispVCProject(CVCNode* pNode, IDispatch** ppDispProject)
{
	CHECK_POINTER_NULL(ppDispProject);
	*ppDispProject = NULL;

	CVCNode* pProj = ResolveProjNode(pNode);
	RETURN_ON_NULL(pProj);

	pProj->GetDispVCProject(ppDispProject);
	return S_OK;
}

HRESULT CVCArchy::CreateGenCfg(VCConfiguration* pProjCfg, IVCGenericConfiguration** ppGenCfg)
{
	CHECK_READ_POINTER_NULL(pProjCfg);
	CHECK_POINTER_NULL(ppGenCfg);
	*ppGenCfg = NULL;

	CComPtr<IVsCfg> spVsCfg;
	HRESULT hr = CGenCfg::CreateInstance(&spVsCfg, this, pProjCfg);
	CComQIPtr<IVCGenericConfiguration> spVCGenCfg = spVsCfg;

	RETURN_ON_FAIL(hr);
	*ppGenCfg = spVCGenCfg.Detach();
	RETURN_ON_NULL(*ppGenCfg);

	return hr;
}

HRESULT CVCArchy::CompileFileItems(HierNodeList& rgSelection)
{
	VCPOSITION pos = rgSelection.GetHeadPosition();
	RETURN_ON_NULL2(pos, S_OK);	// nothing to do

	CVCPtrList listFiles;
	VCPOSITION oldPos = pos;
	BOOL bFirst = TRUE;
	CVCProjectNode* pProjNode = NULL;
	HRESULT hr = S_OK;
	while (pos != NULL && SUCCEEDED(hr))
	{
		while (pos != NULL)
		{
			CVCNode* pNode = rgSelection.GetNext(pos);
			CVCFileNode* pFileNode = (CVCFileNode *)pNode;
			CVCProjectNode* pThisProjNode = pFileNode->GetVCProjectNode();
			if (bFirst)
			{
				bFirst = FALSE;
				pProjNode = pThisProjNode;
			}
			else if (pThisProjNode != pProjNode)
			{
				pos = oldPos;
				break;
			}
			listFiles.AddTail(pFileNode);
		}
		if (!listFiles.IsEmpty())
		{
			CHECK_ZOMBIE(pProjNode, IDS_ERR_PROJ_ZOMBIE);
			hr = pProjNode->CompileFileItems(listFiles);
			listFiles.RemoveAll();
		}
		bFirst = TRUE;
	}

	if (SUCCEEDED(hr) && !listFiles.IsEmpty())
	{
		hr = pProjNode->CompileFileItems(listFiles);
		listFiles.RemoveAll();
	}

	return hr;
}

VSITEMID CVCArchy::ItemIDFromName(CVCNode *pRoot, LPCOLESTR pszMkDocument) 
{
	VSITEMID vsid = DoItemIDFromName(pRoot, VARIANT_TRUE, pszMkDocument);
	if (vsid != VSITEMID_NIL)
		return vsid;

	return DoItemIDFromName(pRoot, VARIANT_FALSE, pszMkDocument);
}

VSITEMID CVCArchy::DoItemIDFromName(CVCNode *pRoot, VARIANT_BOOL bFullOnly, LPCOLESTR pszMkDocument) 
{
	CVCNode* pNext;
	VSITEMID vsid = VSITEMID_NIL;

	if (pRoot == NULL)
	{
		pRoot = GetRootNode();
		RETURN_ON_NULL2(pRoot, VSITEMID_NIL);
	}
	
	VSASSERT(pRoot, "Unable to obtain root container!");
	pNext = static_cast<CVCBaseFolderNode*>(pRoot)->GetHead();

	while (pNext != NULL)
	{
		CVCNode *pVcNode = static_cast<CVCNode*>(pNext);
		CComPtr<IDispatch> pDisp;
		if (pVcNode)
			pDisp = pVcNode->m_dispkeyItem;
		CComBSTR bstrName;
		VARIANT_BOOL bMatched = VARIANT_FALSE;

		CComQIPtr<VCFile> pFile = pDisp;
		if(pFile)
		{
			bstrName = pszMkDocument;
			pFile->MatchName(bstrName, bFullOnly, &bMatched);
			if (bMatched == VARIANT_TRUE)  
				return pNext->GetVsItemID();
			pNext = pNext->GetNext();
			continue;
		}

		CComQIPtr<VCFilter> pFilter = pDisp;
		if(pFilter)
		{
			vsid = DoItemIDFromName(static_cast<CVCNode*>(pNext), bFullOnly, pszMkDocument);
			if ( vsid != VSITEMID_NIL)
				return vsid;
			else	// see if we're actually looking for the filter itself
			{
				bstrName = pszMkDocument;
				pFilter->MatchName(bstrName, bFullOnly, &bMatched);
				if (bMatched == VARIANT_TRUE)
					return pNext->GetVsItemID();
			}

			pNext = pNext->GetNext();
		}
	}
	return vsid;
}

//---------------------------------------------------------------------------
// converts an VSITEMID to a CVCNode *
// returns:
//		S_OK
//		E_FAIL				(NIL, SELECTED, FOCUSED)
//		E_INVALIDARG		(itemid == 0)
//		E_UNEXPECTED		(zombie state)
//---------------------------------------------------------------------------
HRESULT CVCArchy::VSITEMID2Node(VSITEMID itemid, CVCNode **ppNode)
{
	CHECK_POINTER_VALID(ppNode);

	if( itemid == VSITEMID_NIL || itemid == VSITEMID_SELECTION )
		return E_FAIL;

	if( itemid == VSITEMID_ROOT )
	{
		*ppNode = GetProjectNode();
		RETURN_ON_NULL2(*ppNode, E_UNEXPECTED);
		return S_OK;
	}

	CVCNode *pNode = (CVCNode *)itemid;
	RETURN_ON_NULL(pNode);

	try
	{
		// if the item ID is bad, one of these 2 calls is likely to throw.
		if (NULL == pNode->GetParent()) 	// Make sure this node is not a zombie or dead.
		{
			ATLTRACE(_T("ERROR: invalid VSITEMID -> CVCNode\n"));
			return E_UNEXPECTED;
		}
	
		if(pNode->GetVsItemID() != itemid) // Make sure this node actually is an itemID.
			return E_FAIL;

		*ppNode = pNode;
		return S_OK;
	}
	catch( ... )
	{
		VSASSERT(0, "Someone just handed us a bad itemid");
		return E_FAIL;
	}
}

HRESULT CVCArchy::VSITEMID2Node(VSITEMID itemid, HierNodeList& rgNodeList)
{
	HRESULT hr;
	if (VSITEMID_SELECTION != itemid)
	{
		CVCNode *pNode = NULL;
		hr = VSITEMID2Node(itemid, &pNode);
		if (SUCCEEDED(hr))
		{
			rgNodeList.AddHead(pNode);
		}
	}
	else
	{
		hr = GetSelectedNodes(rgNodeList);
	}
	return hr;
}

//---------------------------------------------------------------------------
// fill out an array of selected nodes
//---------------------------------------------------------------------------
HRESULT CVCArchy::GetSelectedNodes(HierNodeList& rgNodes)
{ 
	VSASSERT(m_srpMonSel != NULL, "SetSite must not have been called yet!");
	RETURN_ON_NULL2(m_srpMonSel, E_UNEXPECTED);
	
	HRESULT hr;
	VSITEMID itemid;						// if VSITEMID_SELECTION then multiselection
	CVCNode *pNode = NULL;
	CComPtr<IVsHierarchy> srpIVsHierarchy;	// if NULL then selection spans VsHierarchies
	CComPtr<IVsMultiItemSelect> srpIVsMultiItemSelect;
	CComPtr<ISelectionContainer> srpISelectionContainer;		// unused?
	CComPtr<IVsHierarchy> spThisHier = VCQI_cast<IVsHierarchy>(this);

	hr = m_srpMonSel->GetCurrentSelection(&srpIVsHierarchy, &itemid, &srpIVsMultiItemSelect, &srpISelectionContainer);
	RETURN_ON_FAIL(hr);

	// if we're not selected, but the shell still calls us (this does happen)
	if (srpIVsHierarchy != NULL && srpIVsHierarchy != spThisHier)
		return E_UNEXPECTED;

	if (VSITEMID_NIL == itemid) // nothing selected
	{
		VSASSERT(SUCCEEDED(hr), "Nothing selected!");
		return hr;
	}

	if (VSITEMID_SELECTION != itemid)
	{	// single selection
		VSASSERT(srpIVsHierarchy == spThisHier, "Hierarchy isn't what we expected!");	// shouldn't be NULL

		hr = VSITEMID2Node(itemid, &pNode);
		if (SUCCEEDED(hr))
			rgNodes.AddHead(pNode);
		else
			ATLTRACE(_T("ERROR: invalid VSITEMID in selection\n"));
		return hr;
	}

	ULONG cItems = 0;
	BOOL  fSingleHierarchy = TRUE;
	hr = srpIVsMultiItemSelect->GetSelectionInfo(&cItems, &fSingleHierarchy);
	RETURN_ON_FAIL(hr);
	if (cItems == 0)	// something could be selected, but it isn't for us
		return S_FALSE;
	VSASSERT(0 < cItems, "nothing selected should already be filtered out");

	VSITEMSELECTION *pItemSel = new VSITEMSELECTION[cItems];
	RETURN_ON_NULL2(pItemSel, E_OUTOFMEMORY);

	VSGSIFLAGS fFlags = (fSingleHierarchy) ? GSI_fOmitHierPtrs : static_cast<VSGSIFLAGS>(0);
	hr = srpIVsMultiItemSelect->GetSelectedItems(fFlags, cItems, pItemSel);
	if (SUCCEEDED(hr))
	{
		ULONG i;
		for (i = 0; i < cItems; ++i)
		{
			if (fSingleHierarchy || pItemSel[i].pHier == spThisHier)
			{
				pNode = NULL;
				hr = VSITEMID2Node(pItemSel[i].itemid, &pNode);
				if (SUCCEEDED(hr))
				{
					rgNodes.AddTail(pNode);
					// Needs to be reset for multi-sel
				}
				else
				{	// why is there an invalid itemid?
					VSASSERT(SUCCEEDED(hr), "why is there an invalid itemid?");
					break;
				}
			}
		}
		if (!fSingleHierarchy)
		{	// release all the hierarchies
			for (i = 0; i < cItems; ++i)
			{
				if (NULL != pItemSel[i].pHier)
				{
					pItemSel[i].pHier->Release();
				}
			}
		}
	}
	delete[] pItemSel;

	return hr;
}

//---------------------------------------------------------------------------
// interface: IVsHierarchyDropDataSource
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetDropInfo(DWORD* pdwOKEffects, IDataObject** ppDataObject, IDropSource** ppDropSource)
{
	CHECK_POINTER_NULL(pdwOKEffects);
	CHECK_POINTER_NULL(ppDataObject);
	CHECK_POINTER_NULL(ppDropSource);

	*pdwOKEffects = DROPEFFECT_NONE;
	*ppDataObject = NULL;
	*ppDropSource = NULL;

	// Get rid of any existing dragdrop object
	OnAfterDropOrPaste(FALSE /* no cut */, FALSE /* no move */, TRUE /* drag */, TRUE /* cleanup only */);

    CVCProjDataSource* pDataSource = GetDataSource(pdwOKEffects);
	RETURN_ON_NULL(pDataSource);

	*ppDataObject = pDataSource;	// Return it
	// Hold onto it.
	m_pDragDropDataObject = pDataSource;
	pDataSource->AddRef();

	// Set a bunch of flags.
	ASSERT(s_bStartedDrag == FALSE);
	s_bStartedDrag = TRUE;
	m_bInternalDrop = FALSE;

	return S_OK;
}

//---------------------------------------------------------------------------
// interface: IVsHierarchyDropDataSource
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::OnDropNotify(BOOL fDropped, DWORD dwEffects)
{
	ASSERT(s_bStartedDrag); // ASSERT that a drag is in progress

	if (s_bStartedDrag)
	{
		if(fDropped && dwEffects != DROPEFFECT_NONE)
		{	// Process after the drop
			OnAfterDropOrPaste(FALSE /* no cut */, (dwEffects & DROPEFFECT_MOVE) == DROPEFFECT_MOVE, TRUE /* drag */, FALSE /* not cleanup only */);
		}
		else
		{	// Just cleanup the drag
			OnAfterDropOrPaste(FALSE /* no cut */, FALSE /* no move */, TRUE /* drag */, TRUE /* cleanup only */);
		}
	}

	// Clear our internal drop flag.
	m_bInternalDrop = FALSE;
	return S_OK;
}

//---------------------------------------------------------------------------
// interface: IPkgDropDataTarget
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, VSITEMID itemid, DWORD* pdwEffect)
{
	CHECK_POINTER_NULL(pdwEffect);
	*pdwEffect = DROPEFFECT_NONE;
	CHECK_READ_POINTER_NULL(pDataObj);

	CVCNode *pNode = NULL;
	HRESULT hr = VSITEMID2Node(itemid, &pNode);
	RETURN_ON_FAIL_OR_NULL(hr, pNode);
	hr = DragEnter(pDataObj, pNode);
	RETURN_ON_FAIL(hr);

	return DragOver(grfKeyState, pNode, pdwEffect);
}

//---------------------------------------------------------------------------
// interface: IVsHierarchyDropDataSource, helper routine
//---------------------------------------------------------------------------
HRESULT CVCArchy::DragEnter(IDataObject* pDataObj, CVCNode* pDropNode)
{
	CHECK_READ_POINTER_NULL(pDataObj);

	// Assume we don't know what it is.
	m_dropType = DT_NONE;

	// Attach an MFC data object - no auto release since we aren't addref'n it
	CVCOleDataObject dataObject;
	dataObject.Attach(pDataObj, FALSE);

	if (dataObject.IsDataAvailable(CVCProjDataSource::s_cfRefProjItems))
	{	// It's a project item drop
		if (dataObject.IsDataAvailable(CVCProjDataSource::s_cfProjectDescriptor))
			m_dropType = DT_PROJITEM;	// VC project
		else
			m_dropType = DT_VSREFITEM;	// any other reference-based project
	}
	else if (dataObject.IsDataAvailable(CVCProjDataSource::s_cfStgProjItems))
	{	// It's a drop from a VS project other than VC
		m_dropType = DT_VSITEM;
	}
	else if (dataObject.IsDataAvailable(CF_HDROP))
	{	// It's a shell drop, but we'll only accept data from it if there's a non-folder in it
		m_dropType = DT_NONE;
		// Attach to the data object.
		CVCOleDataObject dataObj;
		dataObj.Attach(pDataObj, FALSE);
		HDROP hDropInfo = (HDROP)dataObj.GetGlobalData(CF_HDROP);
		if(hDropInfo)
		{
			CShellDropHelper dropHelper;
			int iNumActual = 0;
			bool bContainsFolders = false;
			HRESULT hr = PrepareShellDrop(pDropNode, dropHelper, hDropInfo, iNumActual, bContainsFolders);
			if (SUCCEEDED(hr) && iNumActual)
				m_dropType = DT_SHELL;
			::GlobalFree(hDropInfo);
		}
	}

	return S_OK;	
}

BOOL CVCArchy::IsDropTargetCandidate(CVCNode* pNode)
{
	return (pNode->GetKindOf() == Type_CVCFile || pNode->GetKindOf() == Type_CVCFileGroup || 
		pNode->GetKindOf() == Type_CVCProjectNode);
}

//---------------------------------------------------------------------------
// interface: IVsHierarchyDropDataTarget, helper routine
// If the target is not connected to the web server, return DROPEFFECT_NONE
//---------------------------------------------------------------------------
HRESULT CVCArchy::DragOver(DWORD grfKeyState, CVCNode *pNode, DWORD *pdwEffect)
{
	CHECK_READ_POINTER_NULL(pNode);
	CHECK_POINTER_NULL(pdwEffect);
	ASSERT(DROPEFFECT_NONE == *pdwEffect);

	if(m_dropType == DT_SHELL && IsDropTargetCandidate(pNode))	// from Windows Explorer
	{
		*pdwEffect = (((grfKeyState & MK_CONTROL) || (grfKeyState & MK_SHIFT)) ? DROPEFFECT_NONE : DROPEFFECT_LINK);
	}
	else if((m_dropType == DT_PROJITEM || m_dropType == DT_VSREFITEM) && IsDropTargetCandidate(pNode))	// reference-based
	{
		*pdwEffect = ((grfKeyState & MK_CONTROL) ? DROPEFFECT_COPY : DROPEFFECT_MOVE);
	}
	else if (m_dropType == DT_VSITEM && IsDropTargetCandidate(pNode))	// directory-based
	{
		*pdwEffect = (((grfKeyState & MK_CONTROL) || (grfKeyState & MK_SHIFT)) ? DROPEFFECT_NONE : DROPEFFECT_LINK);
	}
	else
	{	// We don't understand the format
		*pdwEffect = DROPEFFECT_NONE;
	}
	return S_OK;
}

//---------------------------------------------------------------------------
// interface: IVsHierarchyDropDataTarget
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::DragOver(DWORD grfKeyState, VSITEMID itemid, DWORD *pdwEffect)
{
	CHECK_POINTER_NULL(pdwEffect);
	*pdwEffect = DROPEFFECT_NONE;

	CVCNode *pNode = NULL;
	HRESULT hr = VSITEMID2Node(itemid, &pNode);
	RETURN_ON_FAIL_OR_NULL(hr, pNode);

	return DragOver(grfKeyState, pNode, pdwEffect);
}

//---------------------------------------------------------------------------
// interface: IVsHierarchyDropDataSource
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::DragLeave(void)
{
	// Reset our drop type
	m_dropType = DT_NONE;
	return S_OK;
}

//---------------------------------------------------------------------------
// interface: IVsHierarchyDropDataSource
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::Drop(IDataObject *pDataObj, DWORD grfKeyState, VSITEMID itemid, DWORD *pdwEffect)
{
	CHECK_POINTER_NULL(pdwEffect);
	*pdwEffect = DROPEFFECT_NONE;
	CHECK_READ_POINTER_NULL(pDataObj);

	CVCNode *pNode = NULL;
	HRESULT hr = VSITEMID2Node(itemid, &pNode);
	RETURN_ON_FAIL_OR_NULL(hr, pNode);

	hr = E_UNEXPECTED;
	if(m_dropType == DT_NONE)
		return hr;

	// Assume it is NOT an internal Drop
	m_bInternalDrop = FALSE;

	CVCNode* pDropNode = NULL;

	if (pNode->GetKindOf() == Type_CVCProjectNode)
		pDropNode = pNode;
	else if (pNode->GetKindOf() == Type_CVCFileGroup)
		pDropNode = pNode;
	else if (pNode->GetKindOf() == Type_CVCFile)
		pDropNode = pNode->GetParent();
	else
		ASSERT(FALSE);

	BOOL bReported = FALSE;
	if (pDropNode)
	{
		CVCExecution multiEx(&GetExecutionCtx());

		// Attach to the data object.
		CVCOleDataObject dataObj;
		dataObj.Attach(pDataObj, FALSE);
		if(m_dropType == DT_SHELL)
		{	// Do shell drop
			ASSERT(dataObj.IsDataAvailable(CF_HDROP));
			HDROP hDropInfo = (HDROP)dataObj.GetGlobalData(CF_HDROP);
			if(hDropInfo)
			{
				// Make sure we are the foreground window
				CComPtr<IVsUIShell> spShell;
				hr = GetBuildPkg()->GetVsUIShell(&spShell, TRUE /* in main thread */);
				ASSERT(spShell);
				if(spShell != NULL)
					spShell->SetForegroundWindow();

				hr = DoShellDrop(pDropNode, hDropInfo, bReported);
				::GlobalFree(hDropInfo);
				*pdwEffect = DROPEFFECT_COPY;
			}
			else
			{	// Just silently skip this one.
				hr = S_OK;
				*pdwEffect = DROPEFFECT_NONE;
			}
		}
		else if (m_dropType == DT_PROJITEM || m_dropType == DT_VSITEM || m_dropType == DT_VSREFITEM)
		{	// Do project item drop
			ASSERT(((m_dropType == DT_PROJITEM || m_dropType == DT_VSREFITEM) && 
				dataObj.IsDataAvailable(CVCProjDataSource::s_cfRefProjItems)) ||
				(m_dropType == DT_VSITEM && dataObj.IsDataAvailable(CVCProjDataSource::s_cfStgProjItems)));

			// Make sure we are the foreground window
			CComPtr<IVsUIShell> spShell;
			hr = GetBuildPkg()->GetVsUIShell(&spShell, TRUE /* in main thread */);
			ASSERT(spShell);
			if(spShell != NULL)
				spShell->SetForegroundWindow();

			if (grfKeyState & MK_CONTROL)
				*pdwEffect = DROPEFFECT_COPY;
			else if (m_dropType == DT_VSITEM)
				*pdwEffect = DROPEFFECT_LINK;
			else
				*pdwEffect =  DROPEFFECT_MOVE;
			hr = DoProjItemDrop(pDropNode, dataObj, TRUE, *pdwEffect, bReported);
		}
		else
		{	// Shouldn't get here.
			ASSERT(FALSE);
		}
	}
	else
	{
		ASSERT(FALSE);
	}

	// We need to report our own errors.
	if(FAILED(hr) && hr != E_UNEXPECTED && hr != OLE_E_PROMPTSAVECANCELLED)
	{
		hr = S_FALSE;
		*pdwEffect = DROPEFFECT_NONE;
	}

	if (FAILED(hr) && m_dropType == DT_SHELL)
	{
		hr = S_FALSE;
		*pdwEffect = DROPEFFECT_NONE;
	}

	return hr;
}

//-----------------------------------------------------------------------------
// Cut command handler
//-----------------------------------------------------------------------------
HRESULT CVCArchy::OnCommandCut()
{
	// Get rid of any existing dragdrop object
	OnAfterDropOrPaste(m_bWasCut, FALSE /* no move */, FALSE /* no drag */, TRUE /* cleanup only */);

	HRESULT hr = E_OUTOFMEMORY;
	DROPEFFECT de;
	CVCProjDataSource* pDataSource = GetDataSource(&de);
	RETURN_ON_NULL2(pDataSource, E_OUTOFMEMORY);

	// Returned with refcount of 1. Keep a hold onto it.
	ASSERT(m_pClipboardDataObject == NULL);
	pDataSource->SetClipboard();

	// Must set this AFTER calling SetClipboard. The shell's clipboard helper
	// will call Clear() and we will release our data source!
	m_pClipboardDataObject = pDataSource;
	m_bWasCut = TRUE;

	// Register clipboard helper events (OK to call multiple times)
	RegisterClipboardNotifications(TRUE);

	// Now set cut highlight on the nodes that were actually cut
	const CVCNodeArray& rgActualNodesAdded = pDataSource->GetNodeList();
	CComPtr<IVsUIHierarchyWindow> spIVsUIHierarchyWindow;
	GetUIHierarchyWindow(&spIVsUIHierarchyWindow);
	RETURN_ON_NULL(spIVsUIHierarchyWindow);
	int num = 1;
	for(int i = 0; i < rgActualNodesAdded.GetSize(); i++)
	{	
		CVCNode* pCutNode = rgActualNodesAdded[i];
		if (pCutNode)
		{
			if(num == 1)
				spIVsUIHierarchyWindow->ExpandItem(GetIVsUIHierarchy(), pCutNode->GetVsItemID(), EXPF_CutHighlightItem);
			else
				spIVsUIHierarchyWindow->ExpandItem(GetIVsUIHierarchy(), pCutNode->GetVsItemID(), EXPF_AddCutHighlightItem);
		}
		num++;
	}

	// Inform the shell of the cut
	CComPtr<IVsUIHierWinClipboardHelper> spClipboardHelper;
	ExternalQueryService(SID_SVsUIHierWinClipboardHelper, IID_IVsUIHierWinClipboardHelper, (void **)&spClipboardHelper);
	if (spClipboardHelper)
		spClipboardHelper->Cut(static_cast<IDataObject*>(pDataSource));

	return S_OK;
}

//-----------------------------------------------------------------------------
// Copy command handler
//-----------------------------------------------------------------------------
HRESULT CVCArchy::OnCommandCopy()
{
	// Get rid of any existing dragdrop object
	OnAfterDropOrPaste(m_bWasCut, FALSE /* no move */, FALSE /* no drag */, TRUE /* cleanup only */);

	DROPEFFECT de;
	CVCProjDataSource* pDataSource = GetDataSource(&de);
	RETURN_ON_NULL2(pDataSource, E_OUTOFMEMORY);

	// Returned with refcount of 1. Save it away
	ASSERT(m_pClipboardDataObject == NULL);
	pDataSource->SetClipboard();
	m_bWasCut = FALSE;

	// Must set this AFTER calling SetClipboard. The shell's clipboard helper
	// will call Clear() and we will release our data source!
	m_pClipboardDataObject = pDataSource;

	// Register clipboard helper events (OK to call multiple times)
	RegisterClipboardNotifications(TRUE);

	// Inform the shell of the copy
	CComPtr<IVsUIHierWinClipboardHelper> spClipboardHelper;
	ExternalQueryService(SID_SVsUIHierWinClipboardHelper, IID_IVsUIHierWinClipboardHelper, (void **)&spClipboardHelper);
	if (spClipboardHelper)
		spClipboardHelper->Copy(static_cast<IDataObject*>(pDataSource));

	return S_OK;
}

//-----------------------------------------------------------------------------
// Handles a paste
//-----------------------------------------------------------------------------
BOOL CVCArchy::CanPasteFromClipboard()
{
	CVCOleDataObject dataObject;
	dataObject.AttachClipboard();
	return(dataObject.IsDataAvailable(CF_HDROP) || dataObject.IsDataAvailable(CVCProjDataSource::s_cfRefProjItems)
		|| dataObject.IsDataAvailable(CVCProjDataSource::s_cfStgProjItems));
}

//-----------------------------------------------------------------------------
// Handles a paste
//-----------------------------------------------------------------------------
HRESULT CVCArchy::PasteFromClipboard(CVCNode *pDropNode)
{
	HRESULT hr = S_OK;
	CVCOleDataObject dataObject;
	dataObject.AttachClipboard();
	BOOL bReported = FALSE;
	DWORD dwDropEffect = DROPEFFECT_LINK;
	if (dataObject.IsDataAvailable(CF_HDROP))
	{
		HDROP hDropInfo = (HDROP)dataObject.GetGlobalData(CF_HDROP);
		if(hDropInfo)
		{
			hr = DoShellDrop(pDropNode, hDropInfo, bReported);
			::GlobalFree(hDropInfo);
		}
	}
	else if (dataObject.IsDataAvailable(CVCProjDataSource::s_cfRefProjItems) || 
		dataObject.IsDataAvailable(CVCProjDataSource::s_cfStgProjItems))
	{
		if (dataObject.IsDataAvailable(CVCProjDataSource::s_cfRefProjItems))
			dwDropEffect = m_bWasCut ? DROPEFFECT_MOVE : DROPEFFECT_COPY;
		hr = DoProjItemDrop(pDropNode, dataObject, FALSE, dwDropEffect, bReported);
	}
	else
	{
		hr = E_UNEXPECTED;
	}

	// Inform the shell that the paste happened. 
	CComPtr<IVsUIHierWinClipboardHelper> spClipboardHelper;
	ExternalQueryService(SID_SVsUIHierWinClipboardHelper, IID_IVsUIHierWinClipboardHelper, (void **)&spClipboardHelper);
	ASSERT(spClipboardHelper);
	if (spClipboardHelper)
		spClipboardHelper->Paste(dataObject.GetIDataObject(FALSE), SUCCEEDED(hr)? dwDropEffect : DROPEFFECT_NONE);

	if (bReported && FAILED(hr))
		hr = E_ABORT;	// don't give VS a chance at this error return code...

	return hr;
}

CShellDropHelper::~CShellDropHelper()
{
	if (m_rgFiles || m_rgActualFiles)		// Free everything
	{
		for (int i = 0; i < m_numFiles; i++)
		{	
			if (m_rgFiles[i])
				delete [] m_rgFiles[i];
			if (m_rgActualFiles[i])
				delete [] m_rgActualFiles[i];
		}
		delete [] m_rgFiles;
		delete [] m_rgActualFiles;
	}
}

HRESULT CVCArchy::PrepareShellDrop(CVCNode* pDropNode, CShellDropHelper& dropHelper, HDROP hDropInfo, int& iNumActual,
	bool& bContainsFolders)
{
	CHECK_READ_POINTER_NULL(pDropNode);

	// Walk the list of files/folders
	dropHelper.m_numFiles = ::DragQueryFileW(hDropInfo, 0xFFFFFFFF, (LPWSTR) NULL, 0);
	if (dropHelper.m_numFiles == 0)
		return S_OK;

	dropHelper.m_rgFiles = new LPWSTR [dropHelper.m_numFiles];
	memset(dropHelper.m_rgFiles, 0,dropHelper.m_numFiles*sizeof(LPWSTR));
	if (dropHelper.m_rgFiles == NULL)
	{
		dropHelper.m_hr = E_OUTOFMEMORY;
		return dropHelper.m_hr;
	}

	for (int i = 0; i < dropHelper.m_numFiles; i++)
	{	// Add each item (file or folder)
		dropHelper.m_rgFiles[i] = new WCHAR[_MAX_PATH];
		if (dropHelper.m_rgFiles[i] == NULL)
		{
			dropHelper.m_hr = E_OUTOFMEMORY;
			return dropHelper.m_hr;
		}
		::DragQueryFileW(hDropInfo, i, dropHelper.m_rgFiles[i], _MAX_PATH);
	}

	// add the stuff (DIANEME_TODO: currently - 3/20/2000 - ignoring shell folders altogether)
	dropHelper.m_rgActualFiles = new LPWSTR[dropHelper.m_numFiles];
	memset(dropHelper.m_rgActualFiles, 0, dropHelper.m_numFiles*sizeof(LPWSTR));
	if (dropHelper.m_rgActualFiles == NULL)
	{
		dropHelper.m_hr = E_OUTOFMEMORY;
		return dropHelper.m_hr;
	}

	bContainsFolders = false;
	iNumActual = 0;
	for (i = 0; i < dropHelper.m_numFiles; i++)
	{
		DWORD dwAttribs = GetFileAttributesW(dropHelper.m_rgFiles[i]);
		if (dwAttribs & FILE_ATTRIBUTE_DIRECTORY)	// DIANEME_TODO: handle shell folders
		{
			bContainsFolders = true;
			continue;
		}
		dropHelper.m_rgActualFiles[iNumActual] = new WCHAR[_MAX_PATH];
		if (dropHelper.m_rgActualFiles[iNumActual] == NULL)
		{
			dropHelper.m_hr = E_OUTOFMEMORY;
			return dropHelper.m_hr;
		}
		wcscpy(dropHelper.m_rgActualFiles[iNumActual], dropHelper.m_rgFiles[i]);
		iNumActual++;
	}

	return S_OK;
}

//---------------------------------------------------------------------------
//	Handles drop files from the shell. Note that this function does NOT free the
//	hdrop. The caller is responsible for this.
//---------------------------------------------------------------------------
HRESULT CVCArchy::DoShellDrop(CVCNode* pDropNode, HDROP hDropInfo, BOOL& bReported)
{
	CShellDropHelper dropHelper;
	int iNumActual = 0;
	bool bContainsFolders = false;
	HRESULT hr = PrepareShellDrop(pDropNode, dropHelper, hDropInfo, iNumActual, bContainsFolders);
	RETURN_ON_FAIL(hr);

	if (iNumActual == 0)
	{
		if (bContainsFolders)
		{
			CComBSTR bstrErr;
			bstrErr.LoadString(IDS_W_DROP_SHELL_FOLDERS);
			ReportError(bstrErr);
		}
		return S_OK;	// nothing to do
	}

	// Track documents batcher to batch up all subsequent calls to IVsTrackProjectDocuments
	CVCTrackProjectDocumentsBatcher track(TRUE);

	// we can check whether files can be added at the project level regardless of what the drop node
	// actually is.	 This is because files have to be unique at the project level.	Once we can start
	// adding folders this way, folder checks will need to be done at the drop node level.
	CComPtr<VCProject> spProject;
	pDropNode->GetVCProject(&spProject);
	CComQIPtr<IVCProjectImpl> spProjectImpl = spProject;
	if (spProjectImpl == NULL)
	{
		dropHelper.m_hr = VCPROJ_E_ZOMBIE;
		return dropHelper.m_hr;
	}

	int i;
	for (i = 0; i < iNumActual && SUCCEEDED(dropHelper.m_hr); i++)
	{
		dropHelper.m_hr = spProjectImpl->CanAddFile2(dropHelper.m_rgActualFiles[i]);
		if (FAILED(dropHelper.m_hr))
			return CVCProjectNode::ReportShellCopyError(dropHelper.m_hr, spProject, NULL, dropHelper.m_rgActualFiles[i], 
				bReported);
	}

	CVCNode* pNode = pDropNode;
	if (pDropNode->GetKindOf() == Type_CVCFile)
		pNode = pDropNode->GetParent();

	CComPtr<VCFilter> spFilter;
	if (pDropNode->GetKindOf() == Type_CVCFileGroup)
		((CVCFileGroup*)pDropNode)->GetVCFilter(&spFilter);

	for (i = 0; i < iNumActual && SUCCEEDED(dropHelper.m_hr); i++)
	{
		CComPtr<IDispatch> spFile;
		if (spFilter)
			dropHelper.m_hr = spFilter->AddFile(dropHelper.m_rgActualFiles[i], &spFile);
		else
			dropHelper.m_hr = spProject->AddFile(dropHelper.m_rgActualFiles[i], &spFile);
		if (SUCCEEDED(dropHelper.m_hr) && spFile == NULL)
			dropHelper.m_hr = E_UNEXPECTED;
		if (FAILED(dropHelper.m_hr))
			return CVCProjectNode::ReportShellCopyError(dropHelper.m_hr, spProject, spFilter, dropHelper.m_rgActualFiles[i], 
				bReported);
	}

	if (SUCCEEDED(dropHelper.m_hr) && bContainsFolders)
	{
		CComBSTR bstrErr;
		bstrErr.LoadString(IDS_W_DROP_SHELL_FOLDERS);
		ReportError(bstrErr);
	}

	return dropHelper.m_hr;
}

//---------------------------------------------------------------------------
//	Handles drop files from other projects. Note that this function does NOT free the
//	hdrop. The caller is responsible for this.
//---------------------------------------------------------------------------
HRESULT CVCArchy::DoProjItemDrop(CVCNode* pDropNode, CVCOleDataObject& rDataObject, BOOL bDrag, DWORD dwDropEffect, 
	BOOL& bReported)
{
	CHECK_ZOMBIE(GetProjectNode(), IDS_ERR_PROJ_ZOMBIE);

	HRESULT hr = S_OK;

	// Track documents batcher to batch up all subsequent calls to IVsTrackProjectDocuments
	CVCTrackProjectDocumentsBatcher track(TRUE);

	// We look for our project descriptor format to extract the project file. From this we
	// determine whether we pasted the data or not. We can't just compare IDataObject pointers
	// as the delayrender technique that we use causes us to get a "proxy" IDataObject created by
	// the clipboard.
	m_bInternalDrop = FALSE;
	BOOL bCrossVC = FALSE;
	CVCProjDataSource* pDataSource = bDrag ? m_pDragDropDataObject : m_pClipboardDataObject;
	if (rDataObject.IsDataAvailable(CVCProjDataSource::s_cfProjectDescriptor))
	{
		HGLOBAL hg = rDataObject.GetGlobalData(CVCProjDataSource::s_cfProjectDescriptor);
		if(hg)
		{
			WCHAR* pBuffer = (WCHAR*)::GlobalLock(hg);
			CComBSTR bstrProjFile;
			GetProjectNode()->get_ProjectFile(&bstrProjFile);
			if(pBuffer && bstrProjFile && !_wcsicmp(pBuffer, bstrProjFile))
				m_bInternalDrop = TRUE;
			else
			{
				CPathW pathProj;
				CStringW strSourceProj = pBuffer;
				if (pathProj.Create(strSourceProj))
				{
					CStringW strExt = pathProj.GetExtension();
					if (_wcsicmp(strExt, L".vcproj") == 0)
						bCrossVC = TRUE;		// we're getting the data from another VC project
				}
			}
			::GlobalFree(hg);
		}
	}


	if((dwDropEffect & DROPEFFECT_MOVE) == DROPEFFECT_MOVE || (dwDropEffect & DROPEFFECT_LINK) == DROPEFFECT_LINK)
		hr = GetProjectNode()->OnDropMove(rDataObject, pDropNode, pDataSource, bCrossVC, bDrag, bReported);

	else if((dwDropEffect & DROPEFFECT_COPY) == DROPEFFECT_COPY)
		hr = GetProjectNode()->OnDropCopy(rDataObject, pDropNode, pDataSource, bCrossVC, bDrag, bReported);

	else
		ASSERT(FALSE);

	return hr;
}

class CCleanupAfterDropOrPaste
{
public:
	CCleanupAfterDropOrPaste(CVCArchy* pArchy) : m_pArchy(pArchy), m_hr(S_OK) {}
	~CCleanupAfterDropOrPaste()
	{
		m_pArchy->m_bInternalDrop = FALSE;
		m_pArchy->m_bWasCut = FALSE;
		m_pArchy->s_bStartedDrag = FALSE;
	}
	CVCArchy* m_pArchy;
	HRESULT m_hr;
};

//---------------------------------------------------------------------------
// After a drop or paste, will use the dwEffects and whether it was handled 
// internally to determine whether we need to clean up the source nodes or not. If
// bCleanupOnly is set, it only does the cleanup work.
//---------------------------------------------------------------------------
HRESULT CVCArchy::OnAfterDropOrPaste(BOOL bWasCut, BOOL bWasMove, BOOL bDrag, BOOL bCleanupOnly)
{
	CCleanupAfterDropOrPaste cleanup(this); // let's us do early returns and still fix up the appropriate members for CVCArchy

	// Do we have a clipboard or drag object?
	CVCProjDataSource* pDataSource = bDrag? m_pDragDropDataObject : m_pClipboardDataObject;
	RETURN_ON_NULL2(pDataSource, S_OK);

	if (!bCleanupOnly && !m_bInternalDrop && ((bDrag && bWasMove) || bWasCut))
	{	// Handle deleting the items here: drag-move/cut-paste across projects
		const CVCNodeArray& rgDragNodes = pDataSource->GetNodeList();
		if(rgDragNodes.GetSize())
		{   // Need to check out project file first
			if(CanDirtyProject() == S_OK)
			{
				HierNodeList rgHierList;
				for (int i = 0; i < rgDragNodes.GetSize(); i++)
					rgHierList.AddTail(rgDragNodes[i]);

				CComPtr<IVsUIShellOpenDocument> pOpenDoc;
				CComPtr<IVsSolution> pSolution;
				CComPtr<IVsProject3> pTransferProject;
				ExternalQueryService( SID_SVsUIShellOpenDocument, IID_IVsUIShellOpenDocument, (void **)&pOpenDoc );
				VCPOSITION pos = rgHierList.GetHeadPosition();
				while (pos != NULL)
				{
					CVCNode* pNode = rgHierList.GetNext(pos);
					BOOL bWasTransferred = FALSE;
					BOOL fResult = FALSE;
					if (pOpenDoc)
					{
						if (!bWasCut)
						{
							CComBSTR bstrMoniker;
							GetMkDocument(pNode->GetVsItemID(), &bstrMoniker);
							CComPtr<IVsWindowFrame> pFrame;
							BOOL fOpen = FALSE;
							if (SUCCEEDED(pOpenDoc->IsDocumentOpen(NULL, NULL, bstrMoniker, GUID_NULL, IDO_IgnoreLogicalView, 
								NULL, NULL, &pFrame, &fOpen)) && fOpen)
							{	// no point in bothering with this extra work if the document isn't even open...
								// fair assumption (being made here): all files will be dropped on the same project
								if (pSolution == NULL)
								{
									GetBuildPkg()->GetIVsSolution(&pSolution);
									if (pSolution)
									{
										CComPtr<IEnumHierarchies> pHierarchies;
										pSolution->GetProjectEnum(EPF_ALLPROJECTS, GUID_NULL, &pHierarchies);
										if (pHierarchies)
										{
											CComPtr<IVsProject> spThisProj = VCQI_cast<IVsProject>(this);
											pHierarchies->Reset();
											while (TRUE)
											{
												CComPtr<IVsHierarchy> pTmpHierarchy;
												ULONG cFetched = 0;
												HRESULT hrT = pHierarchies->Next(1, &pTmpHierarchy, &cFetched);
												if (cFetched == 0)
													break;
												CComQIPtr<IVsProject> pProj = pTmpHierarchy;
												if (pProj == NULL)
													continue;
												if (pProj == spThisProj)	// skip ourself
													continue;
												BOOL fFound = FALSE;
												VSITEMID vsid = VSITEMID_NIL;
												VSDOCUMENTPRIORITY dp = DP_Unsupported;
												if (FAILED(pProj->IsDocumentInProject(bstrMoniker, &fFound, &dp, &vsid)) || 
													!fFound)
													continue;
												pTransferProject = pProj;
												if (pTransferProject)
													break;
											}
										}
									}
								}

								if (pTransferProject)
								{
									pTransferProject->TransferItem(bstrMoniker, bstrMoniker, pFrame);
									bWasTransferred = TRUE;
								}
							}
						}
					}
					cleanup.m_hr = DoRemoveItem(pNode->GetVsItemID(), &fResult, !bWasTransferred);
					// Display any failures
//					if(FAILED(hr) && hr != DIRPRJ_E_SKIPPED && hr != OLE_E_PROMPTSAVECANCELLED)
//						LUtilReportErrorInfo(hr);
				}
			}
			else
			{
				cleanup.m_hr = OLE_E_PROMPTSAVECANCELLED;
			}
		}
	}
	else if (bWasCut)
	{	// Just make sure the cut highlight is reset
		const CVCNodeArray& rgNodes = pDataSource->GetNodeList();
		CComPtr<IVsUIHierarchyWindow> spIVsUIHierarchyWindow;
		GetUIHierarchyWindow(&spIVsUIHierarchyWindow);
		for(int i=0; i< rgNodes.GetSize(); i++)
		{	
			CVCNode* pCutNode = rgNodes[i];
			if(pCutNode && !pCutNode->IsZombie())
				spIVsUIHierarchyWindow->ExpandItem(GetIVsUIHierarchy(), pCutNode->GetVsItemID(), EXPF_UnCutHighlightItem);
		}
	}

	// If we just did a cut, or we are told to cleanup, then we need to free the data object. Otherwise, we leave it
	// alone so that you can continue to paste the data in new locations.
	if (bWasCut || bDrag || bCleanupOnly)
	{
		if(bDrag)
		{
			ASSERT(pDataSource == m_pDragDropDataObject);
			if (m_pDragDropDataObject)
				m_pDragDropDataObject->Release();
			m_pDragDropDataObject = NULL;
		}
		else 
		{
			FlushClipboard();
		}
	}

	return cleanup.m_hr;
}

//-----------------------------------------------------------------------------
// Flushes any clipboard data we may have stuffed on the clipboard
//-----------------------------------------------------------------------------
HRESULT CVCArchy::FlushClipboard()
{
	if(m_pClipboardDataObject)
	{
		m_pClipboardDataObject->Release();
		m_pClipboardDataObject = NULL;
	}
	CVCOleDataObject dataObject;
	dataObject.AttachClipboard();
	if (dataObject.IsDataAvailable(CVCProjDataSource::s_cfProjectDescriptor))
	{
		HGLOBAL hg = dataObject.GetGlobalData(CVCProjDataSource::s_cfProjectDescriptor);
		if (hg)
		{
			WCHAR* pBuffer = (WCHAR*)::GlobalLock(hg);
			CComBSTR bstrProjFile;
			CHECK_ZOMBIE(GetProjectNode(), IDS_ERR_PROJ_ZOMBIE);
			GetProjectNode()->get_ProjectFile(&bstrProjFile);
			if (pBuffer && bstrProjFile && _wcsicmp(pBuffer, bstrProjFile) == 0)
			{
				::OleFlushClipboard();
				::OpenClipboard(NULL);
				::EmptyClipboard();
				::CloseClipboard();
			}
			::GlobalFree(hg);
		}
	}

	return S_OK;
}

//---------------------------------------------------------------------------
// Registers for clipboard events. Can be safely called many times
//---------------------------------------------------------------------------
HRESULT CVCArchy::RegisterClipboardNotifications(BOOL bRegister)
{
	if (bRegister && m_dwClipEvtCookie == 0)
	{
		// register advise sink for clipboard helper events
		CComPtr<IVsUIHierWinClipboardHelper> spClipboardHelper;
		HRESULT hr = ExternalQueryService(SID_SVsUIHierWinClipboardHelper, IID_IVsUIHierWinClipboardHelper, 
			(void **)&spClipboardHelper);
		RETURN_ON_FAIL_OR_NULL(hr, spClipboardHelper);

		CComQIPtr<IVsUIHierWinClipboardHelperEvents, &IID_IVsUIHierWinClipboardHelperEvents> pSink 
			= VCQI_cast<IVsUIHierWinClipboardHelperEvents>(this);
		spClipboardHelper->AdviseClipboardHelperEvents(pSink, &m_dwClipEvtCookie);
	}
	else if (!bRegister && m_dwClipEvtCookie != 0)
	{
		// unregister advise sink for clipboard helper events
		CComPtr<IVsUIHierWinClipboardHelper> spClipboardHelper;
		HRESULT hr = ExternalQueryService(SID_SVsUIHierWinClipboardHelper, IID_IVsUIHierWinClipboardHelper, 
			(void **)&spClipboardHelper);
		RETURN_ON_FAIL_OR_NULL(hr, spClipboardHelper);

		spClipboardHelper->UnadviseClipboardHelperEvents(m_dwClipEvtCookie);
		m_dwClipEvtCookie = 0;
	}

	return S_OK;
}

//---------------------------------------------------------------------------
// IVsUIHierWinClipboardHelperEvents
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::OnPaste(BOOL fDataWasCut, DWORD dwEffects)
{	
	if (dwEffects != DROPEFFECT_NONE)
	{
		OnAfterDropOrPaste(fDataWasCut, (dwEffects & DROPEFFECT_MOVE) == DROPEFFECT_MOVE, FALSE /* no drag */, FALSE /* no cleanup only */);
	}
	else
	{	// Just cleanup the drag
		OnAfterDropOrPaste(fDataWasCut, FALSE /* no move */, FALSE /* no drag */, TRUE /* cleanup only */);
	}

	return S_OK;
}

//---------------------------------------------------------------------------
// IVsUIHierWinClipboardHelperEvents
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::OnClear(BOOL fDataWasCut)
{	
	OnAfterDropOrPaste(fDataWasCut, FALSE /* no move */, FALSE /* no drag */, TRUE /* cleanup only */);
	return S_OK;
}

//-----------------------------------------------------------------------------
// Returns a CVCProjDataSource for the selected items. Or NULL if error. Returns the 
// dropeffect in de. Also returns the list of nodes actually added
//-----------------------------------------------------------------------------
CVCProjDataSource* CVCArchy::GetDataSource(DROPEFFECT* pde)
{
	// Get selected nodes
	CComObject<CVCProjDataSource> *pDataSource = NULL;	// has ref count of 0
	HierNodeList rgNodes;
	CVCNode *pNode = NULL;

	*pde = DROPEFFECT_NONE;
	if (FAILED(GetSelectedNodes(rgNodes)))
		return NULL;

	BOOL bHaveCandidates = FALSE;
	VCPOSITION pos = rgNodes.GetHeadPosition();
	while (NULL != pos)
	{	// At least one node must be a CLangBaseNode . Count files and folders
		CVCNode *pTmpNode = rgNodes.GetNext(pos);
		if (pNode == NULL && (pTmpNode->GetKindOf() == Type_CVCFile || pTmpNode->GetKindOf() == Type_CVCFileGroup))
		{	
			bHaveCandidates = TRUE;
			if (pTmpNode->GetKindOf() == Type_CVCFile)
				break;
		}
	}
	
	if (!bHaveCandidates)
		return NULL;

	HRESULT hr = CComObject<CVCProjDataSource>::CreateInstance(&pDataSource);
	RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, pDataSource, NULL);

	pDataSource->AddRef();

	// Write the project descriptor (allows us to determine which project sourced the data). We just
	// write the project file path.
	CComBSTR bstrProjFile;
	RETURN_ON_NULL2(GetProjectNode(), NULL);
	GetProjectNode()->get_ProjectFile(&bstrProjFile);
	int bufsz = (bstrProjFile.Length() + 1) * sizeof(WCHAR);
	ASSERT(bufsz > 0);
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, bufsz);
	if(hg)
	{	
		WCHAR* pBuffer = static_cast<WCHAR *>(GlobalLock(hg));
		wcscpy(pBuffer, bstrProjFile);
		::GlobalUnlock(hg);
		pDataSource->CacheGlobalData(CVCProjDataSource::s_cfProjectDescriptor, hg);
	}

	// Now go get the actual list of nodes to drag/drop
	CVCNodeArray rgDragDropNodes;
	GetDragDropNodesFromSelection(rgNodes, rgDragDropNodes);

	// Offer CVCProjDataSource::s_cfRefProjItems format but only if there are files and folders
	BOOL bAddedProjItemFormat = FALSE;

	// Delay render our CVCProjDataSource::s_cfRefProjItems. This permits other VC projects to go get the files
	// when the data is requested
	if(rgDragDropNodes.GetSize())
	{
		FORMATETC fmt;
		fmt.cfFormat = CVCProjDataSource::s_cfRefProjItems;
		fmt.ptd		 = NULL;
		fmt.dwAspect = DVASPECT_CONTENT;
		fmt.lindex	 = -1;					// Must be -1!
		fmt.tymed	 = TYMED_HGLOBAL;
		pDataSource->SetNodeList(this, rgDragDropNodes);
		pDataSource->DelayRenderData(CVCProjDataSource::s_cfRefProjItems, &fmt);
		bAddedProjItemFormat = TRUE;
	}

	// Don't support move unless ProjItemFormat offered
	*pde = bAddedProjItemFormat ? DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK : DROPEFFECT_COPY;

	return pDataSource;
}

//-----------------------------------------------------------------------------
// Given the selection list rgSelection, returns all the nodes that need to
// participate in dragdrop.
//-----------------------------------------------------------------------------
HRESULT CVCArchy::GetDragDropNodesFromSelection(HierNodeList& rgSelection, CVCNodeArray& rgDragDropNodes)
{
	// Now we want to go through the nodes and make sure we include the related files.
	rgDragDropNodes.SetSize(0, 20);
	VCPOSITION pos = rgSelection.GetHeadPosition();

	for( int i = 0; pos != NULL; i++)
	{
		CVCNode* pNode = rgSelection.GetNext(pos);
		if (pNode->GetKindOf() == Type_CVCFile || pNode->GetKindOf() == Type_CVCFileGroup)
			rgDragDropNodes.Add(pNode);
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Returns our ui hierarchy.
//-----------------------------------------------------------------------------
HRESULT CVCArchy::GetUIHierarchyWindow(IVsUIHierarchyWindow** ppIVsUIHierarchyWindow)
{
	CHECK_POINTER_NULL(ppIVsUIHierarchyWindow);
	*ppIVsUIHierarchyWindow = NULL;

	// Go get the ui hierarchy window
	CComPtr<IVsUIShell> spIVsUIShell;
	HRESULT hr = GetBuildPkg()->GetVsUIShell(&spIVsUIShell, TRUE /* in main thread */);
	RETURN_ON_FAIL_OR_NULL(hr, spIVsUIShell);

	CComPtr<IVsWindowFrame> spIVsWindowFrame;
	hr = spIVsUIShell->FindToolWindow(NULL, GUID_SolutionExplorer, &spIVsWindowFrame);
	RETURN_ON_FAIL_OR_NULL(hr, spIVsWindowFrame);

	CComVariant srpvt;
	hr = spIVsWindowFrame->GetProperty(VSFPROPID_DocView, &srpvt);
	ASSERT(VT_UNKNOWN == V_VT(&srpvt));
	RETURN_ON_FAIL(hr);
	if (VT_UNKNOWN != V_VT(&srpvt))
		return E_FAIL;

	CComPtr<IVsWindowPane> spIVsWindowPane;
	hr = V_UNKNOWN(&srpvt)->QueryInterface(IID_IVsWindowPane, (void **)&spIVsWindowPane);
	ASSERT(spIVsWindowPane != NULL);
	RETURN_ON_FAIL_OR_NULL(hr, spIVsWindowPane);

	hr = spIVsWindowPane->QueryInterface(IID_IVsUIHierarchyWindow, (void **)ppIVsUIHierarchyWindow);
	ASSERT(ppIVsUIHierarchyWindow != NULL);
	RETURN_ON_FAIL_OR_NULL(hr, ppIVsUIHierarchyWindow);

	return S_OK;
}


// Pass TRUE for bBatch if you want the constructor to immedidately call BeginBatch.
//---------------------------------------------------------------------------
CVCTrackProjectDocumentsBatcher::CVCTrackProjectDocumentsBatcher(BOOL bBatch) : m_cBatches(0)
{
	if (bBatch)
		BeginBatch();
}

//---------------------------------------------------------------------------
// Calls EndBatch() for any outstanding BeginBatches().
//---------------------------------------------------------------------------
CVCTrackProjectDocumentsBatcher::~CVCTrackProjectDocumentsBatcher()
{
	ASSERT(m_cBatches >= 0);
	while (m_cBatches > 0)
		EndBatch();
}

//---------------------------------------------------------------------------
// Calls IVsTrackProjectDocumets::BeginBatch(). Increments count of outstanding
// BeginBatch calls.
//---------------------------------------------------------------------------
HRESULT CVCTrackProjectDocumentsBatcher::BeginBatch()
{
	HRESULT hr = E_NOINTERFACE;
	ASSERT(m_cBatches >= 0);
	if (m_spTracker == NULL)
		ExternalQueryService(SID_SVsTrackProjectDocuments, IID_IVsTrackProjectDocuments, (void **)&m_spTracker);
	if (m_spTracker)
	{
		hr = m_spTracker->BeginBatch();
		ASSERT(SUCCEEDED(hr));
		m_cBatches++;
	}
	return hr;
}

//---------------------------------------------------------------------------
// Calls EndBatch() for any outstanding BeginBatches().
//---------------------------------------------------------------------------
HRESULT CVCTrackProjectDocumentsBatcher::EndBatch()
{
	HRESULT hr = E_NOINTERFACE;
	ASSERT(m_cBatches > 0);
	if(m_spTracker)
	{
		hr = m_spTracker->EndBatch();
		ASSERT(SUCCEEDED(hr));
		m_cBatches--;
	}
	return hr;
}

//---------------------------------------------------------------------------
// IVsExtensibleObject
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetAutomationObject(/*[in]*/ LPCOLESTR  pszPropName, /*[out]*/ IDispatch **ppDisp)
{
	CHECK_POINTER_VALID(ppDisp);
	*ppDisp = NULL;

	CComBSTR bstrCfgName = pszPropName;
	CComPtr<VCConfiguration> spConfig;
	HRESULT hr = GetCfgOfName(bstrCfgName, &spConfig);
	RETURN_ON_FAIL_OR_NULL(hr, spConfig);

	return CVCProjCfgProperties::CreateInstance(spConfig, ppDisp);
}

//---------------------------------------------------------------------------
// interface: IVsProjectResources
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::GetResourceItem(/* [in] */ VSITEMID itemidDocument, /* [in] */ LPCOLESTR pszCulture, 
	/* [in] */ VSPROJRESFLAGS grfPRF, /* [out, retval] */ VSITEMID* pitemidResource)
{

	CHECK_POINTER_NULL(pitemidResource);

	HRESULT hr = E_NOTIMPL; // CVCArchy::GetResourceItem
#if 0
	// Called by the WFC designer to add a resource of specified culture to the project,
	// or to find a culture-specific resource associated with a particular itemid.

	CStringW strResourceFilename;

	// Initialize outparams
	*pitemidResource = VSITEMID_NIL;

	CVCNode *pNode = NULL;
	hr = VSITEMID2Node(itemid, &pNode);

	// Form a filename for the resources
	// REVIEW (kperry) how to mimplement this file name is TBD ...
	GetResourceFilename(pNode, pszCulture, strResourceFilename);
	CPathW path;
	path.CreateFromString(strResourceFilename);

	if(!path.ExistsOnDisk())
	{
		if (!(grfPRF & PRF_CreateIfNotExist))
		{
			// If we didn't find it and we're not told to create it then bail.
			hr = E_FAIL;
			return hr;
		}

		// Create an empty resource file
		if (CreateFile(strResourceFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) == 0)
		{
			hr = HRESULT_FROM_WIN32(::GetLastError());
			return hr;
		}

		// Create the node
		CComPtr<VCProject> pProj;
		GetVCProject( NULL, &pProj );
		RETURN_ON_NULL2(pProj, E_NOINTERFACE);

		CComPtr<IDispatch> pFileAdded;
		pProject->AddFile(bstrPath, &pFileAdded);
	}
	else
	{
	}

	// Return what the caller wants.
	*pitemidResource = pNode->GetVsItemID();
#endif
	return hr;
}

//---------------------------------------------------------------------------
// interface: IVsProjectResources
//---------------------------------------------------------------------------
STDMETHODIMP CVCArchy::CreateResourceDocData(/* [in] */ VSITEMID itemidResource, /* [out, retval] */ IUnknown** punkDocData)
{
	// Creates the resource doc data for a particular VSITEMID.	 VBA
	// needs to override whatever we do here because we're file-based.
	HRESULT hr = E_NOTIMPL; // CVCArchy::CreateResourceDocData
#if 0
	CHECK_POINTER_NULL(punkDocData);

	CVCNode* pNode = NULL;
	hr = VSITEMID2Node(itemidResource, &pNode);

	CComPtr<ILocalRegistry> srpLocalRegistry;
	CComPtr<IVsPersistDocData> srpPersistDocData;

	if (pNode == NULL )
	{
		// Get a local registry
		ExternalQueryService(SID_SLocalRegistry, IID_ILocalRegistry, (LPVOID*) &srpLocalRegistry) );

		// Make a text buffer instance
		srpLocalRegistry->CreateInstance(CLSID_VsTextBuffer,
												  NULL,
												  IID_IVsPersistDocData,
												  CLSCTX_INPROC_SERVER,
												  (LPVOID*) &srpPersistDocData));

		CComBSTR bstrPath = ((CVCFileNode*)pNode)->GetFullPath( void );
		// Load the doc data
		srpPersistDocData->LoadDocData(bstrPath);

		// Get an IUnknown to return
		hr = srpPersistDocData->QueryInterface(IID_IUnknown, (LPVOID*) punkDocData);
	}
	else if (SUCCEEDED(hr))
	{
		// We could have gotten a node, but it might not have been a file.
		// We need to fail in this case.
		hr = E_FAIL;
	}

#endif
	return hr;
}

//---------------------------------------------------------------------------
// IVsProjectSpecificEditorMap
//---------------------------------------------------------------------------

STDMETHODIMP CVCArchy::GetSpecificEditorType(LPCOLESTR pszMkDocument, GUID *pguidEditorType)
{ 
	if (!IsResourceFile(pszMkDocument))
		return E_NOTIMPL; 

	CHECK_POINTER_NULL(pguidEditorType);
	*pguidEditorType = LOGVIEWID_ProjectSpecificEditor;
	return S_OK;
}

//---------------------------------------------------------------------------
// IVsProjectSpecificEditorMap2
//---------------------------------------------------------------------------

STDMETHODIMP CVCArchy::GetSpecificEditorProperty(LPCOLESTR pszMkDocument, VSPSEPROPID propid, VARIANT *pvar)
{
	CHECK_POINTER_NULL(pvar);

	if (!IsResourceFile(pszMkDocument))
		return E_NOTIMPL;

	switch( propid )
	{
	case VSPSEPROPID_UseGlobalEditorByDefault:
		pvar->vt = VT_BOOL;
		pvar->boolVal = GetBuildPkg() ? GetBuildPkg()->UseGlobalEditorForResourceFiles() : VARIANT_FALSE;
		break;

	case VSPSEPROPID_ProjectDefaultEditorName:
		{
		CComBSTR bstrEditorName;
		bstrEditorName.LoadString(IDS_RESOURCEVIEW);
		pvar->vt = VT_BSTR;
		pvar->bstrVal = bstrEditorName.Detach();
		}
		break;

	default:
		return E_NOTIMPL;
	}
	return S_OK;
}

STDMETHODIMP CVCArchy::SetSpecificEditorProperty(LPCOLESTR pszMkDocument, VSPSEPROPID propid, VARIANT var)
{
	if (!IsResourceFile(pszMkDocument))
		return E_NOTIMPL;

	switch( propid )
	{
	case VSPSEPROPID_UseGlobalEditorByDefault:
		if (GetBuildPkg())
			GetBuildPkg()->SetUseGlobalEditorForResourceFiles(var.boolVal);
		break;

	default:
		return E_NOTIMPL;
	}

	return S_OK;
}

BOOL CVCArchy::IsResourceFile(LPCOLESTR pszMkDocument)
{
	CPathW pathDoc;
	if (!pathDoc.Create(pszMkDocument))
		return FALSE;

	CStringW strExt = pathDoc.GetExtension();
	strExt.MakeLower();
	if (strExt != L".rc")
		return FALSE;

	return TRUE;
}
