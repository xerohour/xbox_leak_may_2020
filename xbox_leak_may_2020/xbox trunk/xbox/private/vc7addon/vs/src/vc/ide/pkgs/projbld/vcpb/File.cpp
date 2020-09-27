// File.cpp : Implementation of 

#include "stdafx.h"
#include "File.h"
#include "collection.h"
#include "buildengine.h"
#include "fileconfiguration.h"
#include "projwriter.h"

/////////////////////////////////////////////////////////////////////////////
//
HRESULT CPEFile::CreateInstance(VCFile **ppVCFile)
{
	HRESULT hr;
	CPEFile *pVar;
	CComObject<CPEFile> *pObj;
	hr = CComObject<CPEFile>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = pObj;
		pVar->AddRef();
		*ppVCFile = pVar;
	}
	return hr;
}

CPEFile::CPEFile() :
	m_bstrRelativePath( NULL ),
	m_wszFileName( NULL ),
	m_pParent( NULL ),
	m_pProject( NULL ),
	m_frh( NULL ),
	m_pExternalCookie( NULL ),
	m_bDeploymentContent( VARIANT_FALSE )
{
}

CPEFile::~CPEFile()
{
	if (m_frh)
	{
		m_frh->ReleaseFRHRef();
		m_frh = NULL;
	}
}

STDMETHODIMP CPEFile::get_Project(IDispatch **pVal)
{
	CHECK_ZOMBIE(m_pProject, IDS_ERR_FILE_ZOMBIE);
	CHECK_POINTER_VALID(pVal);
	*pVal = NULL;

	return m_pProject.QueryInterface(pVal);
}

STDMETHODIMP CPEFile::put_Project(IDispatch *pVal)
{
	HRESULT hr = E_NOINTERFACE;	
	CComQIPtr<VCProject> pProj;

	pProj = pVal;
	if (pProj)
	{
		m_pProject = pProj;
		hr = S_OK;
	}
	return hr;
}

STDMETHODIMP CPEFile::get_Parent(IDispatch **pVal)
{
	CHECK_ZOMBIE(m_pParent, IDS_ERR_FILE_ZOMBIE);
	CHECK_POINTER_VALID(pVal);
	*pVal = NULL;

	return m_pParent.QueryInterface(pVal);
}

STDMETHODIMP CPEFile::put_Parent(IDispatch *pVal)
{
	m_pParent = pVal;
	return S_OK;
}

HRESULT CPEFile::SetDirty(VARIANT_BOOL bDirty)
{
	CComQIPtr<IVCProjectImpl> pProjImpl = m_pProject;
	CHECK_ZOMBIE(pProjImpl, IDS_ERR_FILE_ZOMBIE);

	return pProjImpl->put_IsDirty(bDirty);
}

STDMETHODIMP CPEFile::MatchName(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched)
{
	*pbMatched = VARIANT_FALSE;
	CHECK_READ_POINTER_VALID(bstrNameToMatch);

	if (!bFullOnly)	// allowing match of just the file name itself
	{
		CComBSTR bstrFileName;
		get_Name( &bstrFileName );
		if (bstrFileName && _wcsicmp(bstrFileName, bstrNameToMatch) == 0)
		{
			*pbMatched = VARIANT_TRUE;
			return S_OK;
		}
	}

	if (m_bstrRelativePath && _wcsicmp(m_bstrRelativePath, bstrNameToMatch) == 0)
	{
		*pbMatched = VARIANT_TRUE;
	}
	else
	{
		if( m_frh == NULL )
		{
			CComBSTR bstrFullPath;
			get_FullPath( &bstrFullPath );
			RETURN_ON_NULL2(m_frh, E_UNEXPECTED);
		}
			
		CPathW pathToMatch;
		if (pathToMatch.Create(bstrNameToMatch) && _wcsicmp(m_frh->GetFilePath()->GetFullPath(), pathToMatch) == 0)
			*pbMatched = VARIANT_TRUE;
	}
	return S_OK;
}

STDMETHODIMP CPEFile::SaveObject(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent)
{
	long lcItems;
	long index;
	HRESULT hr;

	// Relative path
	NodeAttributeWithSpecialChars( xml, L"RelativePath", m_bstrRelativePath, true );

	// Deployment content, only write if true
	if( m_bDeploymentContent == VARIANT_TRUE )
		NodeAttribute( xml, L"DeploymentContent", L"TRUE" );
	
	// end the node header, start on children
	EndNodeHeader( xml, true );

	// file configs
	lcItems = m_configfiles.GetSize();
	for( index = 0; index < lcItems; index++ )
	{
		CComPtr<VCFileConfiguration> pFileCfg;
		CComBSTR bstrConfigName;
		
		pFileCfg = m_configfiles.GetAt( index );
		if( pFileCfg )
		{
			CComQIPtr<IVCFileConfigurationImpl> pFileCfgImpl = pFileCfg;
			if( pFileCfgImpl )
			{
				hr = pFileCfgImpl->SaveObject( xml, NULL, GetIndent() );
			}
		}
	}
	return S_OK;
}

STDMETHODIMP CPEFile::get_Name(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );

	// if we have a filename, use it
	if( m_wszFileName )
	{
		*pVal = SysAllocString( m_wszFileName );
	}
	// if we don't have a name, generate one
	else
	{
		m_wszFileName = wcsrchr( m_bstrRelativePath, L'\\' );
		// if there was no separator then the name is the whole string
		if( !m_wszFileName )
			m_wszFileName = m_bstrRelativePath;
		// otherwise we need to add one to it (so it doesn't point *at* the separator)
		else
			m_wszFileName++;

		*pVal = SysAllocString( m_wszFileName );
	}
	return S_OK;
}

STDMETHODIMP CPEFile::get_FullPath(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );

	// if we have a FileRegHandle, use it to get the full path
	if( m_frh )
	{
		*pVal = SysAllocString( m_frh->GetFilePath()->GetFullPath() );
		return S_OK;
	}

	if (m_pProject == NULL)	// we're basically zombie
	{
		*pVal = NULL;
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_FILE_ZOMBIE);
	}

	CComBSTR bstrEvaluatedPath;
	HRESULT hr;
	// eval 
	// get 1st file config, use prop cnt eval
	CComPtr<VCFileConfiguration> pFileCfg;
	pFileCfg = m_configfiles.GetAt( 1 );
	CComQIPtr<IVCPropertyContainer> pPropCnt = pFileCfg;
	if( !pPropCnt )
		hr = static_cast<CVCProjectEngine*>(g_pProjectEngine)->Evaluate( m_bstrRelativePath, &bstrEvaluatedPath );
	else
		hr = pPropCnt->Evaluate( m_bstrRelativePath, &bstrEvaluatedPath );
	RETURN_ON_FAIL(hr);

	// create a full path from the project directory and relative path
	CPathW path;
	CDirW dir;
	CComBSTR bstrProjDir;
	m_pProject->get_ProjectDirectory( &bstrProjDir );
	dir.CreateFromString( bstrProjDir );
	path.CreateFromDirAndFilename( dir, bstrEvaluatedPath );
	*pVal = SysAllocString( path.GetFullPath() );

	m_frh = CBldFileRegFile::GetFileHandle(*pVal, TRUE);

	return S_OK;
}

STDMETHODIMP CPEFile::get_RelativePath(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	m_bstrRelativePath.CopyTo(pVal);
	return S_OK;
}

STDMETHODIMP CPEFile::put_RelativePath(BSTR newVal)
{
	CHECK_OK_TO_DIRTY(VCFILEID_RelativePath);

	HRESULT hr = S_OK;
	bool bAddFile = false;

	// if we're loading the project, or don't have a file reg handle (meaning
	// this is coming through the AddItem dlg)
	if( g_bInProjLoad )
	{
		m_bstrRelativePath = newVal;
		return S_OK;
	}
	else if( !m_frh )
		bAddFile = true;

	// else not loading
	// compare eval'd input to full path from file registry
	CComBSTR bstrEvaluatedPath;
	if (bAddFile)
		bstrEvaluatedPath = newVal;
	else
	{
		// eval the input
		// get 1st file config, use its property container
		CComPtr<VCFileConfiguration> pFileCfg;
		pFileCfg = m_configfiles.GetAt( 1 );
		CComQIPtr<IVCPropertyContainer> pPropCnt = pFileCfg;
		if( !pPropCnt )
			hr = static_cast<CVCProjectEngine*>(g_pProjectEngine)->Evaluate( newVal, &bstrEvaluatedPath );
		else
			hr = pPropCnt->Evaluate( newVal, &bstrEvaluatedPath );
		RETURN_ON_FAIL2(hr, S_OK);
	}

	// create a full path
 	CPathW path;
	CDirW dir;
	CComBSTR bstrProjDir;
	m_pProject->get_ProjectDirectory( &bstrProjDir );
	dir.CreateFromString( bstrProjDir );
	path.CreateFromDirAndFilename( dir, bstrEvaluatedPath );

	if (bAddFile)
	{
		path.GetActualCase(TRUE);
		path.SetAlwaysRelative();
		CStringW strPath;
		if (path.GetRelativeName(dir, strPath))
			m_bstrRelativePath = strPath;
		else
			m_bstrRelativePath = newVal;
	}
	else
	{
		const wchar_t *wszPath = m_frh->GetFilePath()->GetFullPath();

		// if they're the same full path, set to whatever they gave us
		if( wcsicmp( path.GetFullPath(), wszPath ) == 0 )
		{
			// first make sure all /s are converted to \s
			wchar_t *pos;
 			wchar_t *newPos = newVal;
			while( pos = wcschr( newPos, L'/' ) )
			{
				*pos = L'\\';
 				newPos = pos;
			}
			m_bstrRelativePath = newVal;
		}
	}

	// cause the name prop to be updated next time it is asked for
	m_wszFileName = NULL;

	if (bAddFile)	// no notifications needed if we're adding this for the first time...
		return S_OK;
	
	// we're not loading, notify UI
	CComQIPtr<IDispatch> pDisp = this;
	if( pDisp )
	{
		CComQIPtr<IVCProjectEngineImpl> pProjEngImpl = g_pProjectEngine;
		if( pProjEngImpl )
			pProjEngImpl->DoFireItemPropertyChange( pDisp, VCFILEID_RelativePath );
	}

	return S_OK;
}

STDMETHODIMP CPEFile::get_DeploymentContent( VARIANT_BOOL *pVal )
{
	CHECK_POINTER_VALID( pVal );
	*pVal = m_bDeploymentContent;
	return S_OK;
}

STDMETHODIMP CPEFile::put_DeploymentContent( VARIANT_BOOL newVal )
{
	CHECK_VARIANTBOOL( newVal );
	CHECK_OK_TO_DIRTY(0);
	m_bDeploymentContent = newVal;
	return S_OK;
}

STDMETHODIMP CPEFile::get_Extension(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	*pVal = NULL;

	// get the path object from the file reg handle
	if (m_frh == NULL)
	{
		CComBSTR bstrPath;
		HRESULT hr = get_FullPath(&bstrPath);
		VSASSERT(SUCCEEDED(hr) && m_frh, "All files have a full path on them!");
		RETURN_ON_NULL2(m_frh, E_UNEXPECTED);
	}
	const CPathW *pPath = m_frh->GetFilePath();
	RETURN_ON_NULL2(pPath, E_UNEXPECTED);

	// get the extension
	const wchar_t* wszExt = pPath->GetExtension();
	RETURN_ON_NULL(wszExt);
	CComBSTR bstrRet( wszExt );
	*pVal = bstrRet.Detach();
	return S_OK;
}

STDMETHODIMP CPEFile::Move(IDispatch *pParent)
{
	HRESULT hr = CheckCanMove(pParent);
	RETURN_ON_FAIL(hr);
	if (hr == S_FALSE)	// signal for no-op
		return S_OK;

	CComQIPtr<IDispatch> spOldParent = m_pParent;
	CComQIPtr<IVCProjectImpl> spOldProjectImpl = m_pParent;
	CComQIPtr<IVCFilterImpl> spOldFilterImpl = m_pParent;
	CComQIPtr<IVCProjectImpl> spNewProjectImpl = pParent;
	CComQIPtr<IVCFilterImpl> spNewFilterImpl = pParent;
	VSASSERT((spOldProjectImpl != NULL || spOldFilterImpl != NULL) && (spNewProjectImpl != NULL || spNewFilterImpl != NULL), "Hey, bad parenting on move!");
	if ((spOldProjectImpl == NULL && spOldFilterImpl == NULL) || (spNewProjectImpl == NULL && spNewFilterImpl == NULL))
		return E_UNEXPECTED;

	IDispatch* pThis = static_cast<IDispatch*>(this);
	if (spOldProjectImpl)
		spOldProjectImpl->RemoveChild(pThis);
	else
		spOldFilterImpl->RemoveChild(pThis);

	// parent will be reset during AddChild
	if (spNewProjectImpl)
		spNewProjectImpl->AddChild(pThis);
	else
		spNewFilterImpl->AddChild(pThis);

	CComQIPtr<IVCProjectEngineImpl> spEngine = g_pProjectEngine;
	if (spEngine)
		spEngine->DoFireItemMoved(pThis, pParent, spOldParent);
	SetDirty(VARIANT_TRUE);

	return hr;
}

STDMETHODIMP CPEFile::CanMove(IDispatch* pParent, VARIANT_BOOL* pbCanMove)
{
	HRESULT hr = CheckCanMove(pParent);
	if (pbCanMove)	// NULL pointer is allowed here since the out param is optional
	{
		CHECK_POINTER_VALID(pbCanMove);
		*pbCanMove = SUCCEEDED(hr) ? VARIANT_TRUE : VARIANT_FALSE;
	}

	return (SUCCEEDED(hr) ? S_OK : S_FALSE);
}

HRESULT CPEFile::CheckCanMove(IDispatch* pParent)
{
	if (pParent == NULL)
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NULL_PATH, IDS_ERR_SET_NULL_PARENT, m_bstrRelativePath);

	CComQIPtr<IDispatch> spDispParent = m_pParent;	// current parent check
	if (pParent == m_pParent)
		return S_FALSE;	// no-op

	CHECK_OK_TO_DIRTY(0);

	// Verify that pParent is the project or is in the project
	CComQIPtr<VCProject> pProj = pParent;
	if (pProj)
	{
		if (pProj == m_pProject)
			return S_OK;
	}

	CComQIPtr<VCFilter> pFilter = pParent;
	if (pFilter == NULL)
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_BAD_PARENT, IDS_ERR_BAD_FILE_PARENT, m_bstrRelativePath);

	CComQIPtr<VCFilter> pFilterT;
	CComPtr<VCFilter> pFilterLast;
	
	pFilterT = pFilter;
	do
	{ // loop ends with pFilterT == NULL 
		CComPtr<IDispatch> pDisp;
		pFilterT->get_Parent(&pDisp);
		pFilterLast = pFilterT;
		pFilterT = pDisp;
	} while (pFilterT);
	
	if (pFilterLast != NULL)
	{
		CComPtr<IDispatch> pDisp;
		pFilterLast->get_Parent(&pDisp);
		pProj = pDisp;
		if (pProj == m_pProject)
			return S_OK;
	}

	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_DIFF_PROJ, IDS_ERR_MOVE_CROSS_PROJ);	// can only move within same project
}

STDMETHODIMP CPEFile::Remove()
{
	return m_pProject->RemoveFile(static_cast<IDispatch *>(this));
}

STDMETHODIMP CPEFile::AddConfiguration(VCConfiguration * pCfg)
{
	HRESULT hr = E_POINTER;
	CComQIPtr<IVCFileConfigurationImpl> pFileConfigImpl;
	CComPtr<VCFileConfiguration> pFileConfiguration;

	if (pCfg)
	{ 
		hr = CFileConfiguration::CreateInstance(&pFileConfiguration);
		pFileConfigImpl = pFileConfiguration;
		VSASSERT(SUCCEEDED(hr) && pFileConfigImpl, "must be out of memory");
		if (SUCCEEDED(hr) && pFileConfigImpl)
		{
			pFileConfigImpl->put_File(static_cast<VCFile *>(this));
			pFileConfigImpl->put_Configuration(pCfg);
			m_configfiles.Add(pFileConfiguration);
			if (PROJECT_IS_LOADED())
			{
				CComQIPtr<IVCBuildableItem> spBuildableItem = pFileConfigImpl;
				if (spBuildableItem)
					spBuildableItem->AssignActions(VARIANT_TRUE);
			}
		}
	}

	return hr;
}

STDMETHODIMP CPEFile::RemoveConfiguration( IDispatch *pItem )
{
	// find the config we're looking for
	long l, lcItems;
	lcItems = m_configfiles.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComPtr<VCFileConfiguration> pFileCfg;
		pFileCfg = m_configfiles.GetAt(l);
		CComQIPtr<IDispatch> pDisp = pFileCfg;
		if( pDisp )
		{
			if( pDisp == pItem )
			{
				// remove it from the collection
				m_configfiles.Remove( pFileCfg );
				// close it down
				CComQIPtr<IVCFileConfigurationImpl> pFileCfgImpl = pFileCfg;
				if (pFileCfgImpl)
					pFileCfgImpl->Close(); // close down entirely; release all
			}
		}
	}
	return S_OK;
}

STDMETHODIMP CPEFile::Initialize(IDispatch * pParent)
{
	return E_NOTIMPL;
}

STDMETHODIMP CPEFile::get_FileConfigurations(IDispatch **ppVal)
{
	CHECK_POINTER_VALID( ppVal );

	CComPtr<IVCCollection> pCollection;
	HRESULT hr = CCollection<VCFileConfiguration>::CreateInstance(&pCollection, &m_configfiles);
	if (SUCCEEDED(hr))
	{
		hr = pCollection.QueryInterface(ppVal);
	}
	return hr;
}

STDMETHODIMP CPEFile::Close()
{
	if (PROJECT_IS_LOADED())
		UnAssignActions(VARIANT_TRUE);

	long l, lcItems;

	BOOL bOldInFileChangeMode = g_bInFileChangeMode;
	g_bInFileChangeMode = TRUE;
	lcItems = m_configfiles.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComPtr<VCFileConfiguration> pFileCfg;
		pFileCfg = m_configfiles.GetAt(l);
		CComQIPtr<IVCFileConfigurationImpl> pFileCfgImpl = pFileCfg;
		if (pFileCfgImpl)
			pFileCfgImpl->Close(); // close down entirely; release all
	}
	g_bInFileChangeMode = bOldInFileChangeMode;

	m_pParent = NULL;
	m_pProject = NULL;
	m_configfiles.RemoveAll();	// collection of configs for this file

	// Let the full path to the file still be available even after we're closed.
	// This means don't release the FRH until destruction time.
	// Reason: need to be able to get the full path for shell transfer purposes.

	return S_OK;
}

STDMETHODIMP CPEFile::GetFileConfigurationForProjectConfiguration(VCConfiguration* pProjCfg, 
	VCFileConfiguration** ppFileCfg)
{
	CHECK_READ_POINTER_NULL(pProjCfg);
	CHECK_POINTER_NULL(ppFileCfg);

	long cConfigFiles = m_configfiles.GetSize();
	for (long idx = 0; idx < cConfigFiles; idx++)
	{
		CComPtr<VCFileConfiguration> pFileCfg = m_configfiles.GetAt(idx);
		CComQIPtr<IVCFileConfigurationImpl> pFileCfgImpl = pFileCfg;
		if (!pFileCfgImpl)
			continue;
		CComPtr<IDispatch> pThisDispProjCfg;
		HRESULT hr = pFileCfgImpl->get_Configuration(&pThisDispProjCfg);
		CComQIPtr<VCConfiguration> pThisProjCfg = pThisDispProjCfg;
		if (pThisProjCfg && pThisProjCfg == pProjCfg)
		{
			pFileCfg.CopyTo(ppFileCfg);
			return S_OK;
		}
	}
	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_NOT_FOUND, IDS_ERR_FILE_CFG_NOT_FOUND);	// didn't find one
}

STDMETHODIMP CPEFile::AssignActions(VARIANT_BOOL bOnLoad)
{
	if (m_pProject != NULL)
	{
		CComQIPtr<IVCBuildableItem> spBuildableProject = m_pProject;
		if (spBuildableProject != NULL)
		{
			CBldFileRegistry* pReg = NULL;
			if (SUCCEEDED(spBuildableProject->get_Registry((void **)&pReg)) && pReg != NULL)
			{
				IVCBuildableItem *pBuildable = this;
				pReg->RestoreToProject(pBuildable, FALSE /* no contents */);
			}
		}
	}

	int iMac = m_configfiles.GetSize();
	for (int idx = 0; idx <= iMac; idx++)
	{
		CComPtr<VCFileConfiguration> pConfig = m_configfiles.GetAt(idx);
		CComQIPtr<IVCBuildableItem> spBuildableItem = pConfig;
		if (pConfig == NULL || spBuildableItem == NULL)
			continue;
		spBuildableItem->AssignActions(bOnLoad);
	}
	return S_OK;
}

STDMETHODIMP CPEFile::UnAssignActions(VARIANT_BOOL bOnClose)
{
	if (bOnClose == VARIANT_TRUE)
	{
		if (m_pProject != NULL)
		{
			CComQIPtr<IVCBuildableItem> spBuildableProject = m_pProject;
			if (spBuildableProject != NULL)
			{
				CBldFileRegistry* pReg = NULL;
				if (SUCCEEDED(spBuildableProject->get_Registry((void **)&pReg)) && pReg != NULL)
				{
					IVCBuildableItem *pBuildable = this;
					pReg->RemoveFromProject(pBuildable, FALSE /* no contents */);
				}
			}
		}
	}
	
	int iMac = m_configfiles.GetSize();
	for (int idx = 0; idx <= iMac; idx++)
	{
		CComPtr<VCFileConfiguration> pConfig = m_configfiles.GetAt(idx);
		CComQIPtr<IVCBuildableItem> spBuildableItem = pConfig;
		if (pConfig == NULL || spBuildableItem == NULL)
			continue;
		spBuildableItem->UnAssignActions(bOnClose);
	}
	return S_OK;
}

#ifdef LATER
STDMETHODIMP CPEFile::GetCfgs(
			/*[in] */ ULONG celt, IVsCfg **rgpcfg,
			/*[out]*/ ULONG *pcActual, VSCFGFLAGS *prgfFlags )
{
	// only want the count ?
	if( celt == 0 )
	{
		CHECK_POINTER_VALID(pcActual);
		*pcActual = m_configfiles.GetCount();
		return S_OK;
	}

	// want some elements ?
	CHECK_POINTER_VALID(rgpcfg);

	// don't return more than there are
	ULONG actualCfgs = m_configfiles.GetSize();
	if( celt > actualCfgs )
		celt = actualCfgs;

	ULONG i;
    for( i=0; i<(unsigned)celt; i++ )
	{
		CComQIPtr<IVsCfg> pConfig;
		pConfig = m_configfiles.GetAt((long)i);
		rgpcfg[i] = pConfig.Detach();
	}
	*pcActual = i;
	return S_OK;
}
#endif 

STDMETHODIMP CPEFile::get_ItemFileName(BSTR *pVal)
{
	CHECK_POINTER_NULL( pVal );
	return get_Name(pVal);
}

STDMETHODIMP CPEFile::get_ItemFullPath(BSTR* pbstrFullPath)
{
	CHECK_POINTER_NULL( pbstrFullPath );
	return get_FullPath(pbstrFullPath);
}

STDMETHODIMP CPEFile::get_ActionList(IVCBuildActionList** ppActionList)
{
	return E_UNEXPECTED;	// file doesn't own one of these
}

STDMETHODIMP CPEFile::get_FileRegHandle(void** pfrh)
{
	CHECK_POINTER_NULL(pfrh);

	if (m_frh == NULL)
	{
		CComBSTR bstrPath;
		HRESULT hr = get_FullPath(&bstrPath);
		VSASSERT(SUCCEEDED(hr), "All files have a full path on them!");

		// CStringW strPath = bstrPath;
		// if (!strPath.IsEmpty())
		// 	 m_frh = CBldFileRegFile::GetFileHandle(strPath, TRUE);
	}
	*pfrh = (void *)m_frh;
	return S_OK;
}

STDMETHODIMP CPEFile::get_ContentList(IEnumVARIANT** ppContents)
{
	CHECK_POINTER_NULL(ppContents);
	*ppContents = NULL;
	
	return S_FALSE;		// don't have a content list the file registry is interested in
}

STDMETHODIMP CPEFile::get_Registry(void** ppFileRegistry)
{
	CComQIPtr<IVCBuildableItem> spBuildableProject = m_pProject;
	CHECK_ZOMBIE(spBuildableProject, IDS_ERR_FILE_ZOMBIE);

	return spBuildableProject->get_Registry(ppFileRegistry);
}

STDMETHODIMP CPEFile::get_ProjectConfiguration(VCConfiguration** ppProjCfg)
{
	CHECK_POINTER_NULL(ppProjCfg);
	*ppProjCfg = NULL;
	return E_UNEXPECTED;
}

STDMETHODIMP CPEFile::get_ProjectInternal(VCProject** ppProject)
{
	CHECK_POINTER_NULL(ppProject);
	*ppProject = NULL;

	CComPtr<IDispatch> spDispProject;
	HRESULT hr = get_Project(&spDispProject);
	RETURN_ON_FAIL(hr);
	CComQIPtr<VCProject> spProject = spDispProject;
	return spProject.CopyTo(ppProject);
}

STDMETHODIMP CPEFile::get_ExistingBuildEngine(IVCBuildEngine** ppBldEngine)
{
	CHECK_POINTER_NULL(ppBldEngine);
	*ppBldEngine = NULL;
	return E_UNEXPECTED;
}

STDMETHODIMP CPEFile::get_PersistPath(BSTR* pbstrPersistPath)
{
	CHECK_POINTER_NULL( pbstrPersistPath );
	return m_bstrRelativePath.CopyTo(pbstrPersistPath);
}

STDMETHODIMP CPEFile::RefreshActionOutputs(long nPropID, IVCBuildErrorContext* pEC)
{
	return S_OK;	// meaningless to a non-config object
}

STDMETHODIMP CPEFile::HandlePossibleCommandLineOptionsChange(IVCBuildErrorContext* pEC)
{
	return S_FALSE;		// meaningless to a non-config object
}

STDMETHODIMP CPEFile::ClearDirtyCommandLineOptionsFlag()
{
	return S_FALSE;		// meaningless to a non-config object
}

STDMETHODIMP CPEFile::get_Kind(BSTR* kind)
{
	CHECK_POINTER_VALID(kind);
	CComBSTR bstrKind = L"VCFile";
	*kind = bstrKind.Detach();
	return S_OK;
}

STDMETHODIMP CPEFile::get_VCProjectEngine(IDispatch** ppProjEngine)
{
	CHECK_POINTER_VALID(ppProjEngine);
	RETURN_ON_NULL(g_pProjectEngine);
	return g_pProjectEngine->QueryInterface(IID_IDispatch, (void **)ppProjEngine);
}
