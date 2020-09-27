#include "stdafx.h"
#include "bldpkg.h"
#include "projbuildevents.h"
#include "vcarchy.h"
#include "vcfile.h"
#include "vcfgrp.h"
#include "vsqryed.h"

#include "prjids.h"

#ifdef AARDVARK
#include "StyleHierarchy.h"
#endif	// AARDVARK

//t-samhar: declared in gencfg.cpp, use this
//to determine if its safe to do UI work
extern BOOL FIsIDEInteractive();

CVCProjBuildEvents::CVCProjBuildEvents()
{

}

CVCProjBuildEvents::~CVCProjBuildEvents()
{

}


HRESULT CVCProjBuildEvents::CreateInstance(IVCProjectEngineEvents** ppOutput)
{
	HRESULT hr;
	CVCProjBuildEvents *pVar;
	CComObject<CVCProjBuildEvents> *pObj;
	hr = CComObject<CVCProjBuildEvents>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = pObj;
		pVar->AddRef();
		*ppOutput = reinterpret_cast<IVCProjectEngineEvents *>(pVar);
	}
	return(hr);
}

// recursive helper function to find a particular folder, returns NULL if it can't find
CVCFileGroup* FindFolderInHierarchy( IDispatch* pItem, CVCNode *pNode, CVCNode *pTailNode )
{
	while( pNode )
	{
		// check for folder
		if( pNode->GetKindOf() == Type_CVCFileGroup )
		{
			// if this was the node,
			if( static_cast<CVCFileGroup*>(pNode)->m_dispkeyItem == pItem )
			{
				return static_cast<CVCFileGroup*>(pNode);
			}
			// otherwise check the nodes inside of it
			// TODO: need recursion here
			else
			{
				// next
				CVCNode *pNewNode = reinterpret_cast<CVCNode*>(static_cast<CVCBaseFolderNode*>(pNode)->GetFirstChild());
				if( pNewNode && pNewNode != (CVCNode*)VSITEMID_NIL )
				{
					// new tail 
					CVCNode *pNewTailNode = static_cast<CVCFileGroup*>(pNewNode)->GetTail();
					CVCFileGroup* pFolder = FindFolderInHierarchy( pItem, pNewNode, pNewTailNode );
					if( pFolder )
						return pFolder;
				}
			}
		}
		// was that the last node?
		if( pNode == pTailNode )
			pNode = NULL;

		// otherwise go on
		else
			pNode = pNode->GetNext();
	}
	return NULL;
}

// find the file for a given file config
IDispatch* FindFileForFileConfig( IDispatch *pItem, IDispatch *pDispFiles )
{
	CComPtr<IDispatch> pDisp;
	CComQIPtr<IVCCollection> pFileColl = pDispFiles;
	if( !pFileColl )
		return NULL;
	long lNumFiles;
	pFileColl->get_Count( &lNumFiles );
	for( long i = 1; i <= lNumFiles; i++ )
	{
		CComPtr<IDispatch> pDispFile;
		pFileColl->Item( CComVariant( i ), &pDispFile );
		CComQIPtr<VCFile> pFile = pDispFile;
		if( pFile )
		{
			// get each file config for this file
			pDisp = NULL;
			pFile->get_FileConfigurations( &pDisp );
			CComQIPtr<IVCCollection> pCfgColl = pDisp;
			if( !pCfgColl )
				return NULL;
			long lNumCfgs;
			pCfgColl->get_Count( &lNumCfgs );
			for( long j = 1; j <= lNumCfgs; j++ )
			{
				pDisp = NULL;
				pCfgColl->Item( CComVariant( j ), &pDisp );
				CComQIPtr<VCFileConfiguration> pFileCfg = pDisp;
				// compare it to ours
				if( pFileCfg == pItem )
					return pDispFile;
			}
		}
	}
	return NULL;
}

// recursive helper function to find a particular file, returns NULL if it can't find
CVCFileNode* FindFileInHierarchy( IDispatch* pItem, CVCNode *pNode, CVCNode *pTailNode )
{
	while( pNode )
	{
		// check for file node
		if( pNode->GetKindOf() == Type_CVCFile )
		{
			// if this was the node,
			if( static_cast<CVCFileNode*>(pNode)->m_dispkeyItem == pItem )
			{
				return static_cast<CVCFileNode*>(pNode);
			}
		}
		// otherwise, if it's a folder, check the nodes inside of it
		else if( pNode->GetKindOf() == Type_CVCFileGroup )
		{
			// next
			CVCNode *pNewNode = reinterpret_cast<CVCNode*>(static_cast<CVCBaseFolderNode*>(pNode)->GetFirstChild());
			if( pNewNode && pNewNode != (CVCNode*)VSITEMID_NIL )
			{
				// new tail 
				CVCNode *pNewTailNode = static_cast<CVCFileGroup*>(pNewNode)->GetTail();
				CVCFileNode* pFile = FindFileInHierarchy( pItem, pNewNode, pNewTailNode );
				if( pFile )
					return pFile;
			}
		}
		// was that the last node?
		if( pNode == pTailNode )
			pNode = NULL;

		// otherwise go on
		else
			pNode = pNode->GetNext();
	}
	return NULL;
}

// find where to insert a filter or file
void CVCProjBuildEvents::FindNodeForItem(IDispatch* pItem, CVCNode** ppNode)
{
	*ppNode = NULL;
	CComQIPtr<IVCExternalCookie> pExCookie = pItem;
	if (pExCookie == NULL)	// not a whole lot we can do here...
		return;

	void* pCookie;
	pExCookie->get_ExternalCookie(&pCookie);
	*ppNode = (CVCNode*)pCookie;
}

STDMETHODIMP CVCProjBuildEvents::ItemAdded(IDispatch *pItem, IDispatch *pItemParent)
{
	HRESULT hr = S_OK;

	CComQIPtr<IVCExternalCookie> pExCookie = pItem;
 	if (pExCookie)
	{
		// Project
		CComQIPtr<VCProject> pProject = pItem;
		if( pProject )
		{
			CComBSTR bszFilename;
			CComObject<CVCArchy> *pVCArchy;	// created with Ref count 0
			hr = pProject->get_ProjectFile(&bszFilename);
			// if we haven't done so in the "PreCreate", create the archy now.
			if(CVCProjectFactory::s_EFPolicyCookie == NULL)
			{
				hr = CComObject<CVCArchy>::CreateInstance(&pVCArchy);
				if (SUCCEEDED(hr))
				{
					pVCArchy->AddRef();
				}
				else
				{
					VSASSERT(SUCCEEDED(hr), "Failed to create a hierarchy for a new project!");
					return E_FAIL; 
				}
			}
			else
			{
				// TODO: ASSERT g_EFPolicyCookieStr == bszFilename
				pVCArchy = (CComObject<CVCArchy> *)CVCProjectFactory::s_EFPolicyCookie;
				CVCProjectFactory::s_EFPolicyCookie= NULL;
			}

			pVCArchy->SetSite(GetServiceProvider());
			CVCProjectNode *pProj;
			hr = CVCProjectNode::CreateInstance(&pProj);
			VSASSERT(SUCCEEDED(hr), "Failed to create a project node for a new project");
			hr = pExCookie->put_ExternalCookie((void *)pProj);
			VSASSERT(SUCCEEDED(hr), "Failed in giving the project engine a hint about our hierarchy address");
			pProj->m_dispkeyItem = pItem;
			GetBuildPkg()->InitializeArchy(bszFilename, pProj, pVCArchy);
			
			return S_OK;
		}

#ifdef AARDVARK
		// Style Sheet
		CComQIPtr<VCStyleSheet> pStyleSheet = pItem;
		if (pStyleSheet)
		{
			CComBSTR bstrFilename;
			CStyleSheetHierarchy* pStyleSheetHierarchy;
			hr = pStyleSheet->get_StyleSheetFile(&bstrFilename);
			RETURN_ON_FAIL(hr);
			hr = CStyleSheetHierarchy::CreateInstance(&pStyleSheetHierarchy, bstrFilename);
			RETURN_ON_FAIL_OR_NULL(hr, pStyleSheetHierarchy);

			pStyleSheetHierarchy->SetSite(GetServiceProvider());
			hr = pExCookie->put_ExternalCookie((void *)pStyleSheetHierarchy);
			VSASSERT(SUCCEEDED(hr), "Failed in giving the style sheet engine a hint about our hierarchy address");
			pStyleSheetHierarchy->m_dispkeyItem = pItem;
			return S_OK;
		}
#endif	// AARDVARK

		// File
		CComQIPtr<VCFile> pFile = pItem;
		if( pFile )
		{
			CVCNode* pParentNode = NULL;
			FindNodeForItem(pItemParent, &pParentNode);
			RETURN_ON_NULL(pParentNode);

			CVCFileNode *pVCFile;
			hr = CVCFileNode::CreateInstance(&pVCFile, pItem);
 			hr = pExCookie->put_ExternalCookie((void *)pVCFile);
			RETURN_ON_FAIL2(hr, S_FALSE);
			CComQIPtr<VCProject> pProjParent = pItemParent;
			if( pProjParent )
			{
				CVCProjectNode* pProjNode = (CVCProjectNode*) pParentNode;
		   		pProjNode->AddTail(pVCFile);
			}
			else
			{
				CComQIPtr<VCFilter> pFiltParent = pItemParent;
				if( pFiltParent )
				{
					CVCFileGroup *pVCFileGroup = (CVCFileGroup*) pParentNode;
					pVCFileGroup->AddTail(pVCFile);
					// if this file shouldn't be under SCC, then we need to exclude it
					VARIANT_BOOL bScc;
					pFiltParent->get_SourceControlFiles( &bScc );
					if( !bScc )
					{
						CComQIPtr<VCProjectItem> pProjItem = pParentNode;
						if( !pProjItem )
							return E_FAIL;
						CComPtr<IDispatch> pDisp;
						hr = pProjItem->get_Project( &pDisp );
						if( FAILED( hr ) )
							return hr;
						CComQIPtr<VCProject> pProj = pDisp;
						if( !pProj )
							return E_FAIL;
						CComBSTR bstrProjFile;
						pProj->get_ProjectFile( &bstrProjFile );
						CComPtr<_DTE> pDTE;
						hr = ExternalQueryService( SID_SDTE, __uuidof(_DTE), (void **)&pDTE );
						if( FAILED( hr ) )
							return hr;
						if( !pDTE )
							return E_FAIL;
						CComPtr<SourceControl> pScc;
						hr = pDTE->get_SourceControl( &pScc );
						if( FAILED( hr ) )
							return hr;
						if( !pScc )
							return E_FAIL;
						CComBSTR bstrFilename;
						pVCFile->get_FullPath( &bstrFilename );
						pScc->ExcludeItem( bstrProjFile, bstrFilename );
					}
				}
			}
			
			// update SCC etc				
			// do NOT TrackProjectDocuments during project load or from
			// command line builds!!
			if( !g_bInProjectLoad )
			{
				CComBSTR bstrFilePath;
				hr = pFile->get_FullPath( &bstrFilePath );

				// Fix up Deployment content ...
				CPathW path;
				path.Create( bstrFilePath );
				CStringW strExt = path.GetExtension();
				if( _wcsicmp( strExt, L".srf" ) == 0 || _wcsicmp( strExt, L".htm" ) == 0 )
				{
					pFile->put_DeploymentContent( VARIANT_TRUE );
				}

				// change the output file if needed to make it unique
				MakeOutputFileUnique(pFile);
			}
			return S_OK;
		}
		// Filter
		CComQIPtr<VCFilter> pFilter = pItem;
		if( pFilter )
		{
			CVCNode* pParentNode = NULL;
			FindNodeForItem(pItemParent, &pParentNode);
			RETURN_ON_NULL(pParentNode);
	
			CVCFileGroup *pVCGroup;
			hr = CVCFileGroup::CreateInstance(&pVCGroup, pItem);
			hr = pExCookie->put_ExternalCookie((void *)pVCGroup);
			if (SUCCEEDED(hr))
			{
				// project
				CComQIPtr<VCProject> pProjParent = pItemParent;
				if( pProjParent )
				{
 					CVCProjectNode* pProjNode = (CVCProjectNode*) pParentNode;
		   		    pProjNode->AddTail(pVCGroup);
					pVCGroup->GetHeadEx();	// don't really care about return since it is known NULL, but need side effects of this call
					return S_OK;
				}
				// filter
				CComQIPtr<VCFilter> pFiltParent = pItemParent;
				if( pFiltParent )
				{
					CVCFileGroup *pVCFileGroup = (CVCFileGroup*) pParentNode;
		   		    pVCFileGroup->AddTail(pVCGroup);
					pVCGroup->GetHeadEx();	// don't really care about return since it is known NULL, but need side effects of this call
					return S_OK;
				}
			}
		}
	}
	// configs don't support externalcookie
	else
	{
		// Config
		CComQIPtr<VCConfiguration> pConfig = pItem;
		if( pConfig )
		{
			CComQIPtr<VCProject> spProject = pItemParent;
			CVCArchy* pArchy = NULL;
			CVCProjectNode* pNode = GetBuildPkg()->GetProjectNode(spProject);
			if (pNode)
				pArchy = pNode->GetArchy();
			if (pArchy == NULL)
			{
				VSASSERT(false, "Failed to find the correct archy to inform!");
				return E_FAIL;
			}

			// get the cfg name
			CComBSTR bstrCfgName;
			hr = pConfig->get_Name( &bstrCfgName );
			RETURN_ON_FAIL(hr);
			// inform about the new cfg
			pArchy->InformConfigurationAdded( bstrCfgName );
			return S_OK;
		}
	}
 	return E_FAIL;
}

void CVCProjBuildEvents::MakeOutputFileUnique(VCFile* pFile)
{
	if (!(GetBuildPkg()->m_fAddNewItem))
		return;		// only do this during Add Item

	CComQIPtr<IVCFileImpl> spFileImpl = pFile;
	if (spFileImpl == NULL)
		return;

	CComPtr<IDispatch> spDispProject;
	pFile->get_Project(&spDispProject);
	CComQIPtr<IVCProjectImpl> spProjectImpl = spDispProject;
	if (spProjectImpl == NULL)
		return;

	spProjectImpl->MakeOutputFileUnique(pFile);
}

HRESULT CVCProjBuildEvents::FindProjectParent(IDispatch* pItem, IDispatch* pItemParent, VCProject** ppProject)
{
	CHECK_POINTER_NULL(ppProject);
	*ppProject = NULL;

	CComQIPtr<VCProject> spProject = pItemParent;
	if (spProject)
	{
		*ppProject = spProject.Detach();
		return S_OK;
	}

	CComPtr<IDispatch> spDispProject;
	CComQIPtr<VCFile> spFile = pItem;
	if (spFile)
	{
		spFile->get_Project(&spDispProject);
		spProject = spDispProject;
		*ppProject = spProject.Detach();
		return S_OK;
	}

	CComQIPtr<VCFilter> spFilter = pItem;
	if (spFilter)
	{
		spFilter->get_Project(&spDispProject);
		spProject = spDispProject;
		*ppProject = spProject.Detach();
		return S_OK;
	}

	CComQIPtr<VCFileConfiguration> pFileCfg = pItem;
	if( pFileCfg )
	{
		CComPtr<IDispatch> pFileDisp;
		pFileCfg->get_File( &pFileDisp );
		CComQIPtr<VCFile> pFile = pFileDisp;
		pFile->get_Project( &spDispProject );
		spProject = spDispProject;
		*ppProject = spProject.Detach();
		return S_OK;
	}

	// OK, no luck with either of those, so try to get the project of the parent
	spFilter = pItemParent;
	if (spFilter)
	{
		spFilter->get_Project(&spDispProject);
		spProject = spDispProject;
		*ppProject = spProject.Detach();
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CVCProjBuildEvents::FindItemInHierarchy(IDispatch* pItem, IDispatch* pItemParent, CVCProjectNode** ppProjNode,
	CVCFileNode** ppFileNode, CVCFileGroup** ppFileGroup)
{
	if (ppProjNode == NULL && ppFileNode == NULL && ppFileGroup == NULL)
		return E_UNEXPECTED;
	if (ppProjNode)
		*ppProjNode = NULL;
	if (ppFileNode)
		*ppFileNode = NULL;
	if (ppFileGroup)
		*ppFileGroup = NULL;

	CComQIPtr<VCFile> spFile = pItem;
	CComQIPtr<VCFilter> spFilter = pItem;
	CComQIPtr<VCProject> spProject = pItem;
	if (spFile == NULL && spFilter == NULL && spProject == NULL)
		return S_FALSE;		// not a whole lot we can do...

	if (spProject)
	{
		CHECK_POINTER_NULL(ppProjNode);	// this had better be available in this case...
		*ppProjNode = GetBuildPkg()->GetProjectNode(spProject);
		return ((*ppProjNode != NULL) ? S_OK : S_FALSE);
	}

	CVCNode *pTailNode, *pHeadNode;
	FindProjectParent(pItem, pItemParent, &spProject);
	CVCProjectNode* pNode = GetBuildPkg()->GetProjectNode(spProject);
	if (pNode)
	{
		pHeadNode = pNode->GetHead();
		pTailNode = pNode->GetTail();
	}
	else
	{
		pHeadNode = pTailNode = NULL;
	}

	if (spFile)
	{
		CHECK_POINTER_NULL(ppFileNode);	// this had better be available in this case...
		*ppFileNode = FindFileInHierarchy(pItem, pHeadNode, pTailNode);
	}
	else
	{
		CHECK_POINTER_NULL(ppFileGroup);
		*ppFileGroup = FindFolderInHierarchy(pItem, pHeadNode, pTailNode);
	}

	return S_OK;
}

STDMETHODIMP CVCProjBuildEvents::ItemRemoved(IDispatch *pItem, IDispatch *pItemParent)
{
	CComQIPtr<IVCExternalCookie> pExCookie;
	HRESULT hr;

 	pExCookie = pItem;
 	if (pExCookie)
	{
		CVCProjectNode* pProjNode = NULL;
		CVCFileNode* pFileNode = NULL;
		CVCFileGroup* pFileGroup = NULL;
		FindItemInHierarchy(pItem, pItemParent, &pProjNode, &pFileNode, &pFileGroup);
		// file
		if( pFileNode )
		{
			// call CleanUpUI() on the ui file object
			// The second arg is the delete op, and we NEVER delete from disk via the engine
			return pFileNode->CleanUpUI( pFileNode, DELITEMOP_RemoveFromProject );
		}
		// filter
		else if( pFileGroup )
		{
			// call CleanUpUI() on the ui filter object
			// The second arg is the delete op, and we NEVER delete from disk via the engine
 			return pFileGroup->CleanUpUI( pFileGroup, DELITEMOP_RemoveFromProject );
		}
		// project
		if( pProjNode )
		{
			// call CleanUpUI() on the ui project object
			// The second arg is the delete op, and we NEVER delete from disk via the engine
			return pProjNode->CleanUpUI( pProjNode, DELITEMOP_RemoveFromProject );
		}
	}
	// configs don't support externalcookie
	else
	{
		// Config
		CComQIPtr<VCConfiguration> pConfig = pItem;
		if( pConfig )
		{
			// get the cfg name
			CComBSTR bstrCfgName;
			hr = pConfig->get_Name( &bstrCfgName );
			RETURN_ON_FAIL(hr);
			CComQIPtr<VCProject> spProject = pItemParent;
			CVCProjectNode *pProjNode = GetBuildPkg()->GetProjectNode(spProject);
			if( pProjNode && pProjNode->GetArchy())	// inform about the new cfg
				pProjNode->GetArchy()->InformConfigurationRemoved( bstrCfgName );
			return S_OK;
		}
	}
	return S_OK;
}

STDMETHODIMP CVCProjBuildEvents::ItemRenamed(IDispatch *pItem, IDispatch *pItemParent, BSTR bstrOldName)
{
	// only configs come through this path currently
	// (folders go through PropertyChange, below)
	HRESULT hr;
	CComQIPtr<VCConfiguration> pConfig = pItem;
	if( pConfig )
	{
		// get the cfg name
		CComBSTR bstrCfgName;
		hr = pConfig->get_Name( &bstrCfgName );
		RETURN_ON_FAIL(hr);
		// inform about the new cfg
		CComQIPtr<VCProject> spProject = pItemParent;
		CVCProjectNode* pProjNode = GetBuildPkg()->GetProjectNode(spProject);
		if (pProjNode && pProjNode->GetArchy())
			pProjNode->GetArchy()->InformConfigurationRenamed( bstrCfgName, bstrOldName );
	}
	return S_OK;
}

STDMETHODIMP CVCProjBuildEvents::ItemMoved(IDispatch *pItem, IDispatch *pNewParent, IDispatch* pOldParent)
{
	CVCProjectNode* pProjNode = NULL;
	CVCFileNode* pFileNode = NULL;
	CVCFileGroup* pFileGroup = NULL;
	CVCNode* pNode = NULL;
	FindItemInHierarchy(pItem, pOldParent, &pProjNode, &pFileNode, &pFileGroup);
	if (pFileNode == NULL && pFileGroup == NULL)	// we're not moving a project, so don't care about pProjNode above
	{
		VSASSERT(FALSE, "Hey, trying to do a move across projects here?!?");
		return ItemAdded(pItem, pNewParent);	// couldn't find spot to remove from...
	}

	pNode = ((pFileNode == NULL) ? static_cast<CVCNode*>(pFileGroup) : static_cast<CVCNode*>(pFileNode));

	CVCNode* pNewParentNode = NULL;
	FindNodeForItem(pNewParent, &pNewParentNode);

	CComQIPtr<VCProject> pProjParent = pNewParent;
	CComQIPtr<VCFilter> pFiltParent = pNewParent;
	if (pProjParent == NULL && pFiltParent == NULL)
		return E_UNEXPECTED;

	pNode->GetHierarchy()->OnItemDeleted(pNode);	// just tell the hierarchy about the remove AND NOBODY ELSE (NOT an interface call)
	static_cast<CVCBaseFolderNode*>(pNode->GetParent())->Remove(pNode);		// pull it out of old location

	if( pProjParent )
	{
		CVCProjectNode* pVCProjNode = (CVCProjectNode*) pNewParentNode;
		pVCProjNode->AddTail(pNode);
	}
	else	// pFiltParent
	{
		CVCFileGroup *pVCFileGroup = (CVCFileGroup*) pNewParentNode;
		pVCFileGroup->AddTail(pNode);
	}
			
	return S_OK;
}

STDMETHODIMP CVCProjBuildEvents::ItemPropertyChange(IDispatch *pItem, long dispid)
{
	// TODO: handle the properties we care about here
	// which are:
	switch( dispid )
	{
	// project Name
	case VCPROJID_Name:
	{
		// make sure we got a project
		CComQIPtr<VCProject> pProject = pItem;
		RETURN_ON_NULL2(pProject, E_NOINTERFACE);

		// get the project node
		CVCProjectNode* pNode = GetBuildPkg()->GetProjectNode(pProject);
		CHECK_ZOMBIE(pNode, IDS_ERR_PROJ_ZOMBIE);

		// set the caption
		CComBSTR bstrName;
		pProject->get_Name( &bstrName );
		pNode->SetCaption( bstrName );
		return S_OK;
	}
	break;
	// filter Name
	case VCFLTID_Name:
	{
		// make sure we got a filter
		CComQIPtr<VCFilter> pFilter = pItem;
		if( !pFilter )
			return E_NOINTERFACE;

		// find the folder associated with it
		CVCFileGroup* pFolder = NULL;
		FindItemInHierarchy(pItem, NULL, NULL, NULL, &pFolder);
		if( pFolder )
		{
			// set the caption
			CComBSTR bstrName;
			pFilter->get_Name( &bstrName );
			pFolder->SetCaption( bstrName );
			return S_OK;
		}
	}
	break;
	// file RelativePath
	case VCFILEID_RelativePath:
	{
		// make sure we got a file
		CComQIPtr<VCFile> pFile = pItem;
		if( !pFile )
			return E_NOINTERFACE;

		// find the file node associated with it
		CVCFileNode* pFileNode = NULL;
		FindItemInHierarchy(pItem, NULL, NULL, &pFileNode, NULL);
		if( pFileNode )
		{
			// set the caption
			CComBSTR bstrName;
			pFile->get_Name( &bstrName );
			pFileNode->SetCaption( bstrName );
			return S_OK;
		}
	}
	break;
	// file level custom build stuff
	case VCFCSTID_CommandLine:
	case VCFCSTID_Outputs:
	// file fileconfig Tool
	case VCFCFGID_Tool:
	// file ExcludeFromBuild
	case VCFCFGID_ExcludedFromBuild:
	{
		// find the file that has this fileconfig
		CComQIPtr<VCProject> spProject;
		FindProjectParent(pItem, NULL, &spProject);
		RETURN_ON_NULL2(spProject, E_NOINTERFACE);

		CComPtr<IDispatch> pDispFiles;
		spProject->get_Files( &pDispFiles );
		IDispatch *pDispFile = FindFileForFileConfig( pItem, pDispFiles );
		// Find this file in the hierarchy
		CVCNode *pTailNode, *pHeadNode;
		CVCProjectNode* pNode = GetBuildPkg()->GetProjectNode(spProject);
		if (pNode)
		{
			pHeadNode = pNode->GetHead();
			pTailNode = pNode->GetTail();
		}
		else
		{
			pHeadNode = pTailNode = NULL;
		}
		CVCFileNode *pFileNode = FindFileInHierarchy( pDispFile, pHeadNode, pTailNode );
		if( pFileNode && pFileNode->GetHierarchy())
		{
			// update the glyph
			pFileNode->GetHierarchy()->OnPropertyChanged( pFileNode, VSHPROPID_IconIndex, 0 );	// not an interface call...
		}
	}
	break;

	default:
		break;
	}

	return S_OK ;
}

STDMETHODIMP CVCProjBuildEvents::SccEvent( IDispatch* pItem, enumSccEvent event, VARIANT_BOOL *pRet )
{
	CHECK_POINTER_NULL(pRet);
	*pRet = VARIANT_TRUE;	// whatever it is, assume it will be OK to do unless told otherwise...

	// we can bail right off the bat if we're doing a command line build...
	if( !FIsIDEInteractive() )
		return S_OK;

	HRESULT hr;
	CComQIPtr<VCProject> pProject;
	CComPtr<IVsQueryEditQuerySave> pQueryEdit;
	CVCProjectNode* pProjNode;

	switch( event )
	{
	case eProjectInScc:
		pProject = pItem;
		RETURN_INVALID_ON_NULL(pProject);
		pProjNode = GetBuildPkg()->GetProjectNode(pProject);
		if (pProjNode && pProjNode->GetArchy())
			pProjNode->GetArchy()->RegisterSccProject();
		return S_OK;

	case ePreDirtyNotification:
		hr = ExternalQueryService(SID_SVsQueryEditQuerySave, IID_IVsQueryEditQuerySave,(void**)&pQueryEdit);
		if( SUCCEEDED(hr) && pQueryEdit )
		{
			VSQueryEditResult fEditResult;
			pProject = pItem;
			RETURN_INVALID_ON_NULL(pProject);
			CComBSTR bstrFullPath;
			pProject->get_ProjectFile( &bstrFullPath );
			hr = pQueryEdit->QueryEditFiles(QEF_AllowInMemoryEdits, 1,
						&bstrFullPath, NULL, NULL, &fEditResult, NULL);
			if( fEditResult != QER_EditOK )
				*pRet = VARIANT_FALSE;
			else 
				*pRet = VARIANT_TRUE;
		}
		return hr;

	default:
		RETURN_INVALID();
	}

	return S_OK;
}

STDMETHODIMP CVCProjBuildEvents::ReportError( BSTR bstrErrMsg, HRESULT hrReport, BSTR bstrHelpKeyword )
{
	CComPtr<IVsUIShell> pUIShell;
	HRESULT hr = GetBuildPkg()->GetVsUIShell(&pUIShell, TRUE /* in main thread */);
	if( SUCCEEDED( hr ) && pUIShell )
		pUIShell->SetErrorInfo( hrReport, bstrErrMsg, 0, bstrHelpKeyword, NULL );
		pUIShell->ReportErrorInfo(hrReport);
	return S_OK;
}

STDMETHODIMP CVCProjBuildEvents::ProjectBuildStarted( IDispatch *pCfg )
{
	return S_OK;
}

STDMETHODIMP CVCProjBuildEvents::ProjectBuildFinished( IDispatch *pCfg, long warnings, long errors, VARIANT_BOOL bCancelled )
{
	return S_OK;
}

