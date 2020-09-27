// Filter.cpp : Implementation of 

#include "stdafx.h"
#include "Filter.h"
#include "file.h"
#include "VCProjectEngine.h"
#include "BuildEngine.h"
#include "projwriter.h"
#include "project.h"

/////////////////////////////////////////////////////////////////////////////
//
HRESULT CFilter::CreateInstance(VCFilter **ppDispInfo)
{
	HRESULT hr;
	CFilter *pVar;
	CComObject<CFilter> *pObj;
	hr = CComObject<CFilter>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = pObj;
		pVar->AddRef();
		*ppDispInfo = pVar;
		//hr = pVar->Initialize(NULL, NULL);
	}
	return hr;
}


HRESULT CFilter::SetDirty(VARIANT_BOOL bDirty)
{
	CComPtr<IDispatch> pProjDisp;
	get_Project(&pProjDisp);
	CComQIPtr<IVCProjectImpl> pProjectImpl = pProjDisp;
	CHECK_ZOMBIE(pProjectImpl, IDS_ERR_FILTER_ZOMBIE);

	return pProjectImpl->put_IsDirty(bDirty);
}
STDMETHODIMP CFilter::get_Parent(IDispatch **ppVal)
{
	CHECK_POINTER_VALID( ppVal );
	m_pParent.CopyTo(ppVal);
	return S_OK;
}

STDMETHODIMP CFilter::put_Parent(IDispatch *ppVal)
{
	m_pParent =ppVal;
	return S_OK;
}

STDMETHODIMP CFilter::AddChild(IDispatch* pChild)
{
	m_bItemsCollectionDirty = true;

	CComQIPtr<VCFile> spFile = pChild;
	if (spFile != NULL)	
	{
		m_rgFiles.Add(spFile);
		CComQIPtr<IVCFileImpl> spFileImpl = spFile;
		if (spFileImpl)
			spFileImpl->put_Parent(this);
		return S_OK;
	}

	CComQIPtr<VCFilter> spFilter = pChild;
	RETURN_ON_NULL2(spFilter, E_UNEXPECTED);	// only know about files and filters for this

	m_rgFilters.AddTail(spFilter);
	CComQIPtr<IVCFilterImpl> spFilterImpl = spFilter;
	if (spFilterImpl)
		spFilterImpl->put_Parent(this);
	return S_OK;
}

STDMETHODIMP CFilter::RemoveChild(IDispatch* pChild)
{
	m_bItemsCollectionDirty = true;

	CComQIPtr<VCFile> spFile = pChild;
	if (spFile != NULL)	
	{
		m_rgFiles.Remove(spFile);
		CComQIPtr<IVCFileImpl> spFileImpl = spFile;
		if (spFileImpl)
			spFileImpl->put_Parent(NULL);
		return S_OK;
	}

	CComQIPtr<VCFilter> spFilter = pChild;
	RETURN_ON_NULL2(spFilter, E_UNEXPECTED);	// only know about files and filters for this

	m_rgFilters.Remove(spFilter);
	CComQIPtr<IVCFilterImpl> spFilterImpl = spFilter;
	if (spFilterImpl)
		spFilterImpl->put_Parent(NULL);
	return S_OK;
}

STDMETHODIMP CFilter::get_Filters(IDispatch **ppVal)
{
	CHECK_POINTER_VALID(ppVal);
		
	CComPtr<IVCCollection> pCollection;

	HRESULT hr = CCollection<VCFilter>::CreateInstance(&pCollection, &m_rgFilters);
	if (SUCCEEDED(hr))
	{
		*ppVal = pCollection.Detach();
	}
	return hr;
}

STDMETHODIMP CFilter::get_Files(IDispatch **ppVal)
{
	CHECK_POINTER_VALID(ppVal);
		
	CComPtr<IVCCollection> pCollection;

	HRESULT hr = CCollection<VCFile>::CreateInstance(&pCollection, &m_rgFiles);
	if (SUCCEEDED(hr))
	{
		*ppVal = pCollection.Detach();
	}
	return hr;
}

STDMETHODIMP CFilter::get_Filter(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	return m_bstrFilter.CopyTo(pVal);
}

STDMETHODIMP CFilter::put_Filter(BSTR newVal)
{
	CHECK_OK_TO_DIRTY(0);
	m_bstrFilter = newVal;
	// USER_MODEL: Should we recalculte Filtering Now ?
	return S_OK;
}

STDMETHODIMP CFilter::get_ParseFiles(VARIANT_BOOL* pbParse)
{
	CHECK_POINTER_VALID(pbParse);
	*pbParse = m_bParseFiles;
	return S_OK;
}

STDMETHODIMP CFilter::put_ParseFiles(VARIANT_BOOL bParse)
{
	CHECK_VARIANTBOOL(bParse);
	CHECK_OK_TO_DIRTY(0);

	m_bParseFiles = bParse;
	return S_OK;
}

STDMETHODIMP CFilter::get_SourceControlFiles(VARIANT_BOOL* pbSCC)
{
	CHECK_POINTER_VALID(pbSCC);
	*pbSCC = m_bSCCFiles;
	return S_OK;
}

STDMETHODIMP CFilter::put_SourceControlFiles(VARIANT_BOOL bSCC)
{
	CHECK_VARIANTBOOL(bSCC);
	CHECK_OK_TO_DIRTY(0);

	m_bSCCFiles = bSCC;
	return S_OK;
}

STDMETHODIMP CFilter::get_Items(IDispatch **ppVal)
{
	CHECK_POINTER_VALID(ppVal);

	BuildItemsCollection();
		
	CComPtr<IVCCollection> pCollection;
	HRESULT hr = CCollection<VCProjectItem>::CreateInstance(&pCollection, &m_rgItems, this, 1);
	if (SUCCEEDED(hr))
		*ppVal = pCollection.Detach();
	return hr;
}

STDMETHODIMP CFilter::UpdateItemsCollection(DWORD dwCookie)
{
	if (dwCookie != 0)	// only care about updating one collection
		BuildItemsCollection();

	return S_OK;
}

void CFilter::BuildItemsCollection()
{
	if (!m_bItemsCollectionDirty)
		return;

	m_rgItems.RemoveAll();

	long l;
	long lcItems = m_rgFiles.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComQIPtr<VCProjectItem> spItem = m_rgFiles.GetAt(l);
		if (spItem == NULL)
			continue;

		m_rgItems.Add(spItem);
	}

	lcItems = m_rgFilters.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComQIPtr<VCProjectItem> spItem = m_rgFilters.GetAt(l);
		if (spItem == NULL)
			continue;

		m_rgItems.Add(spItem);
	}

	m_bItemsCollectionDirty = false;
}

STDMETHODIMP CFilter::MatchName(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched)
{
	CHECK_POINTER_VALID(pbMatched);
	CComQIPtr<VCFilter> spFilterParent = m_pParent;
	*pbMatched = VARIANT_FALSE;
	if ((!bFullOnly || !spFilterParent) && m_bstrName && bstrNameToMatch && _wcsicmp(m_bstrName, bstrNameToMatch) == 0)
	{
		*pbMatched = VARIANT_TRUE;
		return S_OK;
	}

	// time to check for unique name since plain name didn't match

	RETURN_ON_NULL2(spFilterParent, S_OK);	// top level filter's unique name is same as its plain name, so no further work makes sense here

	CComBSTR bstrNameIn = bstrNameToMatch;
	// if the length of the name to match isn't significantly longer than the name of the filter, there's no point
	// in building up the unique filter path/name
	if (bstrNameIn.Length() <= m_bstrName.Length() + 1)
		return S_OK;

	RETURN_ON_NULL2(bstrNameToMatch, S_OK);		// not going to match if this is NULL and what we have here isn't

	CComBSTR bstrCanonicalName;
	get_CanonicalName(&bstrCanonicalName);
	if (bstrCanonicalName.Length() > 0 && _wcsicmp(bstrCanonicalName, bstrNameToMatch) == 0)
	{
		*pbMatched = VARIANT_TRUE;
		return S_OK;
	}

	if (m_bstrUniqueIdentifier.Length() > 0 && _wcsicmp(m_bstrUniqueIdentifier, bstrNameToMatch) == 0)
		*pbMatched = VARIANT_TRUE;

	return S_OK;
}

STDMETHODIMP CFilter::get_Name(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	return m_bstrName.CopyTo(pVal);
}

STDMETHODIMP CFilter::put_Name(BSTR newVal)
{
	// don't update UI during project load!
	if( g_bInProjLoad )
	{
		m_bstrName = newVal;
		return S_OK;
	}

	// validate name
	// check for all whitespace or 0 length string
	CStringW strName = newVal;
	strName.TrimLeft();
	strName.TrimRight();
	if( strName.IsEmpty() )
		return S_OK;

	// check for duplicate names
	HRESULT hr = E_FAIL;
	CComQIPtr<VCProject> spProject = m_pParent;
	CComQIPtr<VCFilter> spParentFilter = m_pParent;
	CComQIPtr<IVCProjectImpl> spProjectImpl = spProject;
	CComQIPtr<IVCFilterImpl> spParentFilterImpl = spParentFilter;
	if (spProjectImpl)
		hr = spProjectImpl->CanAddFilter2(newVal, VARIANT_TRUE /* errorinfo */);
	else if (spParentFilterImpl)
		hr = spParentFilterImpl->CanAddFilter2(newVal, VARIANT_TRUE /* errorinfo */);
	RETURN_ON_FAIL(hr);

	CHECK_OK_TO_DIRTY(0);

	CComBSTR bstrOldName = m_bstrName;
	m_bstrName = newVal;

	// send an event notifying everyone in the UI business that we've changed
	// appearances
	CComQIPtr<IDispatch> spDisp = this;
	if( spDisp )
	{
		CComQIPtr<IVCProjectEngineImpl> spPrjEngImpl = g_pProjectEngine;
		if( spPrjEngImpl )
		{
			spPrjEngImpl->DoFireItemPropertyChange( spDisp, VCFLTID_Name );
			spPrjEngImpl->DoFireItemRenamed( spDisp, m_pParent, bstrOldName );
		}
	}

	return S_OK;
}

// unique name is built up of '\' delimited filter names up to the top level
STDMETHODIMP CFilter::get_CanonicalName(BSTR* pbstrName)
{
	CHECK_POINTER_VALID(pbstrName);
	CHECK_ZOMBIE(m_pParent, IDS_ERR_FILTER_ZOMBIE);

	CStringW strName = m_bstrName;
	CComQIPtr<VCFilter> pParentFilter = m_pParent;
	while( pParentFilter )
	{
		CComBSTR bstrParentName;
		HRESULT hr = pParentFilter->get_Name(&bstrParentName);
		RETURN_ON_FAIL(hr);
		strName = bstrParentName + "\\" + strName;
		CComPtr<IDispatch> pDispParent;
		hr = pParentFilter->get_Parent(&pDispParent);
		pParentFilter = pDispParent;
	};

	*pbstrName = strName.AllocSysString();
	return S_OK;
}

// unique GUID identifier for the filter
STDMETHODIMP CFilter::get_UniqueIdentifier(BSTR* pbstrID)
{
	CHECK_POINTER_VALID(pbstrID);
	m_bstrUniqueIdentifier.CopyTo(pbstrID);
	return S_OK;
}

STDMETHODIMP CFilter::put_UniqueIdentifier(BSTR bstrID)
{
	CHECK_OK_TO_DIRTY(0);
	m_bstrUniqueIdentifier = bstrID;
	return S_OK;
}

STDMETHODIMP CFilter::RemoveExistingFile(IDispatch* pItem)
{
	return DoRemoveExistingFile(pItem, FALSE);
}

STDMETHODIMP CFilter::RemoveFile(/*[in]*/  IDispatch *pItem)
{
	return DoRemoveExistingFile(pItem, TRUE);
}

HRESULT CFilter::QueryCanRemoveFile(VCFile* pFile)
{
	VSASSERT( pFile, "Hey, how did we get here with a NULL pFile?" );
	if (pFile == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	
	// warning: if you remove this ForwardTrackEvents, be sure to check CVCProjectEngine::s_pBuildPackage for NULL...
	if (!CProject::ForwardTrackEvents(NULL))	// check it once for cheap and dirty check
		return S_OK;

	if( !CProject::ForwardTrackEvents( this ) )	// check it again with the parent
		return S_OK;

	CComBSTR bstrPath;
	HRESULT hr = pFile->get_FullPath(&bstrPath);
	RETURN_ON_FAIL(hr);

	CComPtr<IDispatch> pProjDisp;
	hr = get_Project(&pProjDisp);// get the parent object so we can add the file to the flat list
	if( FAILED( hr ) )
		return hr;
	CComQIPtr<VCProject> pProject = pProjDisp;
	if( !pProject )
		return E_UNEXPECTED;

 	return CVCProjectEngine::s_pBuildPackage->AllowedToRemoveFile(pProject, bstrPath);
}

HRESULT CFilter::DoRemoveExistingFile(IDispatch* pItem, BOOL bTellProject)
{
	HRESULT hr = VCPROJ_E_NOT_FOUND;
	CComQIPtr<VCFile> pFile;
	long lcItems;
	long index;
 	lcItems = m_rgFiles.GetSize();
	for (index = 0; index <= lcItems && hr == VCPROJ_E_NOT_FOUND; index++)
	{
		pFile.Release();
		pFile = m_rgFiles.GetAt(index);
		if (pFile == NULL)
			continue;

		CComQIPtr<IDispatch> pDispFile = pFile;
		if (pDispFile && pDispFile == pItem)
		{
#ifdef _DEBUG
			CComBSTR bstrName; pFile->get_Name(&bstrName); // DEBUG
#endif
			// are we okay to delete the file?
			CHECK_OK_TO_DIRTY(0);
			hr = QueryCanRemoveFile(pFile);
			RETURN_ON_FAIL(hr);

			m_rgFiles.RemoveAtIndex(index);
			// let the project close the file...
			hr = S_OK;
		}
	}

	m_bItemsCollectionDirty = true;
	if (bTellProject && SUCCEEDED(hr))
	{
		CComPtr<IDispatch> pProjDisp;
		get_Project(&pProjDisp);
		CComQIPtr<IVCProjectImpl> pProjectImpl = pProjDisp;
		if (pProjectImpl)
			hr = pProjectImpl->RemoveExistingFile(pItem);
		else
			hr = E_UNEXPECTED;
	}

	if (hr == VCPROJ_E_NOT_FOUND)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_NOT_FOUND, IDS_ERR_FILE_NOT_FOUND_FOR_REMOVE, bTellProject);

	return hr;
}

HRESULT CFilter::RemoveExistingFilter(IDispatch* pDispFilter, long index)
{
	m_rgFilters.RemoveAtIndex(index);

	// fire events
	CComQIPtr<IVCProjectEngineImpl> pEngine = g_pProjectEngine;
	RETURN_ON_NULL2(pEngine, E_UNEXPECTED);

	CComQIPtr<IDispatch> pDispParent = this;
	if( pDispParent )
		pEngine->DoFireItemRemoved(pDispFilter, pDispParent);

	m_bItemsCollectionDirty = true;
	CComQIPtr<IVCFilterImpl> pFilterImpl = pDispFilter;
	CComQIPtr<IVCFilterImpl> pFilter = pDispFilter;
	if (pFilter)
		pFilter->RemoveContents();
	if (pFilterImpl)
		pFilterImpl->Close();

	return S_OK;
}

STDMETHODIMP CFilter::RemoveFilter(/*[in]*/  IDispatch *pItem)
{
	HRESULT hr = VCPROJ_E_NOT_FOUND;
	long lcItems;
	long index;
	CComQIPtr<VCFilter> pFilter;

	lcItems = m_rgFilters.GetSize();
	for (index = 0; index <= lcItems && hr == VCPROJ_E_NOT_FOUND; index++)
	{
		pFilter = m_rgFilters.GetAt(index);
		if (pFilter == NULL)
			continue;

		CComQIPtr<IDispatch> pDispFilter = pFilter;
		if (pDispFilter && pDispFilter == pItem)
		{
			CHECK_OK_TO_DIRTY(0);
			return RemoveExistingFilter(pDispFilter, index);
		}
	}

	if (hr == VCPROJ_E_NOT_FOUND)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_NOT_FOUND, IDS_ERR_FILTER_NOT_FOUND_FOR_REMOVE);

	return hr;
}

// helper function to sort files
static int FileCompare( const void* fileA, const void* fileB )
{
	CComBSTR bstrNameA, bstrNameB;
	(*(VCFile**)(fileA))->get_Name( &bstrNameA );
	(*(VCFile**)(fileB))->get_Name( &bstrNameB );
	return wcscoll( bstrNameA, bstrNameB );
}

STDMETHODIMP CFilter::SaveObject(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent)
{
	// Name
	NodeAttributeWithSpecialChars( xml, CComBSTR( L"Name" ), m_bstrName );

	// Filter
	NodeAttributeWithSpecialChars( xml, CComBSTR( L"Filter" ), m_bstrFilter, true );

	// only save the unique identifier if there actually is one
	if (m_bstrUniqueIdentifier.Length() > 0)
		NodeAttributeWithSpecialChars( xml, CComBSTR( L"UniqueIdentifier" ), m_bstrUniqueIdentifier );

	// only save the info about parsing files if the user DOESN'T want the files parsed
	if (m_bParseFiles == VARIANT_FALSE)
		NodeAttribute( xml, CComBSTR( L"ParseFiles"), L"FALSE" );
	
	// only save the info about files if the user DOESN'T want the files under SCC
	if (m_bSCCFiles == VARIANT_FALSE)
		NodeAttribute( xml, CComBSTR( L"SourceControlFiles"), L"FALSE" );
	
	// end of attributes, start children
	EndNodeHeader( xml, true );

	// files
	// sort the files alphabetically before writing
	// allocate an array of ptrs
	VCFile **rgFiles = new VCFile*[m_rgFiles.GetCount()];
	// copy the ptrs (NON-REFCOUNTED!) into the array
	long lcItems = m_rgFiles.GetSize();
	int i = 0;
	long index;
	for( index = 0; index < lcItems; index++ )
	{
		VCFile* pFile = m_rgFiles.GetAt( index );
		if( pFile )
			rgFiles[i++] = pFile;
	}
	// sort the array
	qsort( (void*)rgFiles, i, sizeof(VCFile*), FileCompare );

	CComBSTR bstrFile( L"File" );
	lcItems = i;
	for( index = 0; index < lcItems; index++ )
	{
		CComQIPtr<VCFile> pFile;
		
		pFile = rgFiles[index];
		if( pFile )
		{
			CComQIPtr<IVCFileImpl> pFileImpl = pFile;
			if ( pFileImpl )
			{
				StartNodeHeader( xml, bstrFile, true );
				pFileImpl->SaveObject( xml, NULL, GetIndent() );
				EndNode( xml, bstrFile, true );
			}
		}
	}
	delete[] rgFiles;

	// filters
	CComBSTR bstrFilter( L"Filter" );
	lcItems = m_rgFilters.GetSize();
	for( index = 0; index <= lcItems; index++ )
	{
		CComPtr<VCFilter> pFilter;
		pFilter = m_rgFilters.GetAt( index );
		if( pFilter )
		{
			CComQIPtr<IVCFilterImpl> pFilterImpl = pFilter;
			if( pFilterImpl )
			{
				StartNodeHeader( xml, bstrFilter, true );
				pFilterImpl->SaveObject( xml, NULL, GetIndent() );
				EndNode( xml, bstrFilter, true );
			}
		}
	}

	return S_OK;
}


// Returns TRUE if the filter for this filter matches the file
STDMETHODIMP CFilter::IsMatch(LPCOLESTR szFileName, VARIANT_BOOL *pbIsMatch)
{
	CStringW strItem, strFilter, strExt;
	int i;
	int extStart, extEnd;

	*pbIsMatch = VARIANT_FALSE;
	if (m_bstrFilter.Length() == 0)
		return S_OK;

	strFilter = m_bstrFilter;
	strItem = szFileName;
	i = strItem.ReverseFind(L'.');
	if( i > 0 )
	{ 
		// assume that a dot at the beginning isn't an extension
		strItem = strItem.Mid(i+1);
		strItem.MakeUpper();
		strFilter.MakeUpper();

		extStart = 0;
		extEnd = 0;
		// while we're not at the end of the extension list string
		while( extStart < strFilter.GetLength() )
		{
			// get the next extension in the filter list
			extEnd = strFilter.Find( L';', extStart );
			// if it wasn't found there is an error
			if( extEnd == -1 )
				extEnd = strFilter.GetLength();
			// does it match our file's extension?
			strExt = strFilter.Mid( extStart, extEnd - extStart );
			if( strExt == strItem )
			{
				*pbIsMatch = VARIANT_TRUE;
				return S_OK;
			}
			extStart = extEnd + 1;
		}
	}
	return S_OK;
}


STDMETHODIMP CFilter::Close()
{
	// NOTE: not closing down the file list here since all we have is a copy of what the project
	// actually owns.  If you need to close down files, do it there.

	long lcItems = m_rgFilters.GetSize();
	for (long l = 0; l < lcItems; l++)
	{
		CComPtr<VCFilter> pFilter;
		pFilter = m_rgFilters.GetAt(l);
		CComQIPtr<IVCFilterImpl> pFilterImpl = pFilter;
		if (pFilterImpl)
			pFilterImpl->Close(); // close down entirely; release all
	}

	m_pParent = NULL;
	m_rgFiles.RemoveAll();	// collection of files
	m_rgFilters.RemoveAll();	// collection of filters
	return S_OK;
}

STDMETHODIMP CFilter::AddItem(IDispatch * pDisp)
{
	HRESULT hr = E_UNEXPECTED;
	CComQIPtr<VCFile> pFile;
	CComQIPtr<IVCFileImpl> pFileImpl;

	CHECK_OK_TO_DIRTY(0);

	pFile = pDisp;
	if (pFile)
	{
		CComPtr<IDispatch> pDispParentLast;
		pFile->get_Parent(&pDispParentLast);
		pFileImpl = pFile;
		VSASSERT(pFileImpl, "*All* file objects must implement IVCFileImpl");
		if (pFileImpl)
		{
			pFileImpl->put_Parent(static_cast<IDispatch *>(this));
		}
		m_rgFiles.Add(pFile);
		m_bItemsCollectionDirty = true;
		
		if (!pDispParentLast)
		{ // this kind of implies that the DoFireItemAdded is really a DoFireNEWItemAdded
			CComQIPtr<IVCProjectEngineImpl> pEngine;
			pEngine = g_pProjectEngine;
			if (pEngine)
				pEngine->DoFireItemAdded(pDisp, (IDispatch*)this);
		}
		hr = S_OK;
	}
	else
	{
		CComQIPtr<VCFilter> pFilter;
		CComQIPtr<IVCFilterImpl> pFilterImpl;
		
		pFilter = pDisp;
		if (pFilter)
		{
			CComPtr<IDispatch> pDispParentLast;
			pFilter->get_Parent(&pDispParentLast);
			pFilterImpl = pFilter;
			VSASSERT(pFilterImpl, "*All* filter objects must implement IVCFilterImpl");
			if (pFilterImpl)
			{
				pFilterImpl->put_Parent(static_cast<IDispatch *>(this));
			}
			m_rgFilters.Add(pFilter);
			m_bItemsCollectionDirty = true;

			if (!pDispParentLast)
			{ // this kind of implies that the DoFireItemAdded is really a DoFireNEWItemAdded
				CComQIPtr<IVCProjectEngineImpl> pEngine;
				pEngine = g_pProjectEngine;
				if (pEngine)
					pEngine->DoFireItemAdded(pDisp, (IDispatch*)this);
			}
			hr = S_OK;
		}
	}
	return hr;
}

HRESULT CFilter::CheckCanAddFilter(BSTR bstrFilter, CStringW& strCleanedUpName, BOOL bSetErrorInfo)
{
	CComPtr<IDispatch> spExistingFilter;
	HRESULT hr = FindExistingFilter(bstrFilter, strCleanedUpName, &spExistingFilter, bSetErrorInfo);
	RETURN_ON_FAIL(hr);
	if (spExistingFilter)	// already exists
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_FILTER_EXISTS, IDS_ERR_FILTER_EXISTS, bstrFilter, bSetErrorInfo);

	CHECK_OK_TO_DIRTY(0);

	return S_OK;
}

STDMETHODIMP CFilter::CanAddFilter(BSTR bstrFilter, VARIANT_BOOL* pbCanAdd)
{
	CHECK_POINTER_VALID(pbCanAdd);

	CStringW strCleanedUpName;
	HRESULT hr = CheckCanAddFilter(bstrFilter, strCleanedUpName, TRUE);
	*pbCanAdd = FAILED(hr) ? VARIANT_FALSE : VARIANT_TRUE;

	return SUCCEEDED(hr) ? S_OK : S_FALSE;
}

STDMETHODIMP CFilter::CanAddFilter2(BSTR bstrFilter, VARIANT_BOOL bSetErrorInfo)
{
	CStringW strCleanedUpName;
	return CheckCanAddFilter(bstrFilter, strCleanedUpName, (bSetErrorInfo == VARIANT_TRUE));
}

HRESULT CFilter::FindExistingFilter(BSTR bstrFilterName, CStringW& strCleanedUpName, IDispatch** ppDispFilter, BOOL bSetErrorInfo)
{
	*ppDispFilter = NULL;	// BETTER be safe thing to do since this is an internal function...

	// Validate: Is this a valid name ?
	strCleanedUpName = bstrFilterName;
	strCleanedUpName.TrimLeft();
	strCleanedUpName.TrimRight();
	if( strCleanedUpName.GetLength() == 0 )
		return CVCProjectEngine::DoSetErrorInfo2(E_INVALIDARG, IDS_ERR_BAD_FILTER_NAME, bstrFilterName, bSetErrorInfo);

	// Validate: Do we have one with this name already ?
	if( g_bInProjLoad == FALSE )
	{
		int cFilters = m_rgFilters.GetSize();
		int index;
		for( index = 0; index <= cFilters; index++ )
		{
			CComPtr<VCFilter> pFilterT = m_rgFilters.GetAt( index );
			if( pFilterT )
			{
				CComBSTR bstrExistingName;
				pFilterT->get_Name( &bstrExistingName );
				if( bstrExistingName && wcsicmp( bstrExistingName, strCleanedUpName ) == 0 )
				{
					pFilterT.QueryInterface( ppDispFilter );
					return S_FALSE;
				}
			}
		}
	}

	return S_OK;
}

STDMETHODIMP CFilter::AddFilter(BSTR bstrFilterName, /*[out, retval]*/ IDispatch **ppDispFilter)
{
	CHECK_POINTER_VALID(ppDispFilter)
	CStringW strName;
	HRESULT hr = CheckCanAddFilter(bstrFilterName, strName, TRUE);	// dirty happens here...
	RETURN_ON_FAIL(hr);
	CComBSTR bstrName = strName;
		
	CComPtr<VCFilter> pFilter;
	CComQIPtr<IVCFilterImpl> pFilterImpl;
	hr = CFilter::CreateInstance(&pFilter);
	pFilterImpl = pFilter;
	RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, pFilterImpl, E_NOINTERFACE);

	pFilter.QueryInterface(ppDispFilter);
	pFilterImpl->put_Parent(this);
	pFilter->put_Name(bstrName);
	m_rgFilters.Add(pFilter);
	m_bItemsCollectionDirty = true;

	CComQIPtr<IVCProjectEngineImpl> pEngine;
	pEngine = g_pProjectEngine;
	if (pEngine)
		pEngine->DoFireItemAdded(pFilter, (IDispatch*)this);

	return S_OK;
}

STDMETHODIMP CFilter::get_Project(/*[out, retval]*/ IDispatch **ppProject)
{
	CHECK_POINTER_VALID( ppProject );
	return FindParentProject(this, ppProject);
}

HRESULT CFilter::WrapCanDirty(UINT idCheck)	// just need to wrap the CHECK_OK_TO_DIRTY macro
{
	CHECK_OK_TO_DIRTY(idCheck);
	return S_OK;
}

HRESULT CFilter::CheckCanAddFile(BSTR bstrFile, IVCProjectImpl** ppProjImpl, BOOL bSetErrorInfo)
{
	CComPtr<IDispatch> spFile;

	HRESULT hr = FindExistingFile(bstrFile, ppProjImpl, &spFile, bSetErrorInfo);
	RETURN_ON_FAIL(hr);
	if (spFile)	// already there
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_FILE_EXISTS, IDS_ERR_FILE_EXISTS, bstrFile, bSetErrorInfo);

	CHECK_ZOMBIE(*ppProjImpl, IDS_ERR_FILTER_ZOMBIE);

	hr = (*ppProjImpl)->FileNameValid(bstrFile, bSetErrorInfo ? VARIANT_TRUE : VARIANT_FALSE);
	if (hr != S_OK)
		return hr;

	CHECK_OK_TO_DIRTY2(VCFLTID_AddFile, bSetErrorInfo);

	CComQIPtr<VCProject> spProj = *ppProjImpl;
	return CProject::QueryCanAddFile(spProj, this, bstrFile, bSetErrorInfo);
}

STDMETHODIMP CFilter::CanAddFile(BSTR bstrFile, VARIANT_BOOL* pbCanAdd)
{
	CHECK_POINTER_VALID(pbCanAdd);

	CComPtr<IVCProjectImpl> spProjImpl;
	HRESULT hr = CheckCanAddFile(bstrFile, &spProjImpl, TRUE);
	*pbCanAdd = FAILED(hr) ? VARIANT_FALSE : VARIANT_TRUE;

	return SUCCEEDED(hr) ? S_OK : S_FALSE;
}

STDMETHODIMP CFilter::CanAddFile2(BSTR bstrFile)
{
	CComPtr<IVCProjectImpl> spProjImpl;
	return CheckCanAddFile(bstrFile, &spProjImpl, FALSE);
}

HRESULT CFilter::FindExistingFile(BSTR bstrPath, IVCProjectImpl** ppProjImpl, IDispatch** ppFile, BOOL bSetErrorInfo)
{
	*ppProjImpl = NULL;	// BETTER be valid output param...
	*ppFile = NULL;		// BETTER be valid output param...

	// check for empty string
	if( !bstrPath || *bstrPath == L'\0' )
		return CVCProjectEngine::DoSetErrorInfo2(E_INVALIDARG, IDS_ERR_BAD_FILE_NAME, bstrPath, bSetErrorInfo);

	CComPtr<IDispatch> pProjDisp;
	HRESULT hr = get_Project(&pProjDisp);// get the parent object so we can add the file to the flat list
	CComQIPtr<IVCProjectImpl> pProjImpl = pProjDisp;
	pProjImpl.CopyTo(ppProjImpl);
	CComQIPtr<VCProject> pProject = pProjDisp;
	RETURN_ON_NULL2(pProjImpl, E_UNEXPECTED);

	if( g_bInProjLoad == FALSE )		// dont validate if loading a project
	{
		CComPtr<IDispatch> pDispColl;
		pProject->get_Files(&pDispColl);
		CComQIPtr<IVCCollection> pColl = pDispColl; VSASSERT(pColl, "Either did not get a files collection or it was not of the right COM type.");
		hr = pColl->Item(CComVariant(bstrPath), ppFile);
	}

	return hr;	
}

STDMETHODIMP CFilter::AddFile(BSTR bstrPath, /*[out, retval]*/ IDispatch * *ppDispFile)
{
	CHECK_POINTER_VALID(ppDispFile);

	HRESULT hr;
	CComQIPtr<IVCProjectImpl> spProjImpl;

	if( g_bInProjLoad == FALSE )
	{
		hr = CheckCanAddFile(bstrPath, &spProjImpl, TRUE);
		RETURN_ON_FAIL(hr);
		CHECK_ZOMBIE(spProjImpl, IDS_ERR_PROJ_ZOMBIE);
	}
	// in project load, need to get an IVCProjectImpl ptr
	else
	{
		CComPtr<IDispatch> pProjDisp;
		hr = get_Project(&pProjDisp);// get the parent object so we can add the file to the flat list
		spProjImpl = pProjDisp;
		RETURN_ON_FAIL(hr);
		CHECK_ZOMBIE(spProjImpl, IDS_ERR_PROJ_ZOMBIE);
	}

	CComQIPtr<VCFile> spFile;
	hr = CPEFile::CreateInstance(&spFile);
	CComQIPtr<IDispatch> spDisp = spFile;
	VSASSERT(spFile && spDisp, "Must be out of memory.");
	RETURN_ON_FAIL(hr);
	if (spFile == NULL || spDisp == NULL)
		return E_OUTOFMEMORY;

	hr = spProjImpl->AddNewFile(spDisp, bstrPath, VARIANT_FALSE);
	VSASSERT(SUCCEEDED(hr), "Must not have done all the proper checks to make sure the file we want to add was not there already.");
	RETURN_ON_FAIL(hr);

	spDisp.CopyTo(ppDispFile);

	CComQIPtr<IVCFileImpl> spFileImpl = spFile;
	if (spFileImpl)
		spFileImpl->put_Parent(this);
	m_rgFiles.Add(spFile);
	m_bItemsCollectionDirty = true;

	// fire events
	CComQIPtr<IVCProjectEngineImpl> pEngine = g_pProjectEngine;
	if (pEngine)
	{
		pEngine->DoFireItemAdded(spDisp, static_cast<IDispatch*>(this));
		if( g_bInProjLoad == FALSE )
		{
			CComQIPtr<VCProject> spProject = spProjImpl;
			if (spProject)
				CProject::InformFileAdded(spProject, spFile, bstrPath);
		}
		return S_OK;
	}

	return DoSetErrorInfo(E_UNEXPECTED, IDS_ERR_INTERNAL_ERROR);
}

STDMETHODIMP CFilter::Move(IDispatch *pParent)
{
	HRESULT hr = CheckCanMove(pParent);
	RETURN_ON_FAIL(hr);
	if (hr == S_FALSE)	// signal for no-op
		return S_OK;

	CComPtr<IDispatch> spOldParent = m_pParent;
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
		spEngine->DoFireItemMoved(pThis, m_pParent, spOldParent);

	SetDirty(VARIANT_TRUE);
	return hr;
}

bool CFilter::FindAsParent(IDispatch* pItem, IDispatch* pParent)
{
	CComQIPtr<VCFilter> spFilter = pParent;
	RETURN_ON_NULL2(spFilter, false);	// only care about filters on filters

	CComQIPtr<VCFilter> spFilterLast;

	do
	{ // loop ends with spFilter == NULL 
		CComPtr<IDispatch> spDisp;
		spFilter->get_Parent(&spDisp);
		if (spDisp == pItem)
			return true;
		spFilterLast = spFilter;
		spFilter = spDisp;
	} while (spFilter);

	return false;	// obviously didn't find it
}

STDMETHODIMP CFilter::CanMove(IDispatch* pParent, VARIANT_BOOL* pbCanMove)
{
	HRESULT hr = CheckCanMove(pParent);
	if (pbCanMove)		// NULL pointer is allowed here since the out param is optional
	{
		CHECK_POINTER_VALID(pbCanMove);
		*pbCanMove = SUCCEEDED(hr) ? VARIANT_TRUE : VARIANT_FALSE;
	}

	return (SUCCEEDED(hr) ? S_OK : S_FALSE);
}

HRESULT CFilter::CheckCanMove(IDispatch* pParent)
{
	if (pParent == NULL)
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NULL_PATH, IDS_ERR_SET_NULL_PARENT, m_bstrName);

	CComQIPtr<IDispatch> spOldDispParent = m_pParent;
	if (spOldDispParent == pParent)
		return S_FALSE;	// no-op since same parent
	
	CComQIPtr<IDispatch> spDispThis = this;
	if (pParent == spDispThis)
		return S_FALSE;	// should be no-op to drop on itself

	CHECK_OK_TO_DIRTY(0);

	CComPtr<IDispatch> spMyProjParent;
	CComPtr<IDispatch> spNewProjParent;
	FindParentProject((IDispatch *)this, &spMyProjParent);
	FindParentProject(pParent, &spNewProjParent);
	if (spMyProjParent != spNewProjParent)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_DIFF_PROJ, IDS_ERR_MOVE_CROSS_PROJ);	// can only move within same project

	if (FindAsParent(this, pParent))	// trying to make this filter a child of one of its children!
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_BAD_PATH, IDS_ERR_PARENT_AS_CHILD, m_bstrName);

	CComQIPtr<VCProject> spProject = pParent;
	CComPtr<IDispatch> spDispFilters;
	if (spProject)
		spProject->get_Filters(&spDispFilters);
	else
	{
		CComQIPtr<VCFilter> spFilter = pParent;
		if (spFilter == NULL)
			return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_BAD_PARENT, IDS_ERR_BAD_FILTER_PARENT, m_bstrName);
		spFilter->get_Filters(&spDispFilters);
	}

	CComQIPtr<IVCCollection> spFilterCollection = spDispFilters;
	RETURN_ON_NULL2(spFilterCollection, E_UNEXPECTED);

	CComVariant varName = m_bstrName;
	CComPtr<IDispatch> spExists;
	spFilterCollection->Item(varName, &spExists);
	if (spExists != NULL)	// cannot have two folders there with the same name
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_FILTER_EXISTS, IDS_ERR_FILTER_EXISTS, m_bstrName, TRUE);

	return S_OK;
}

HRESULT CFilter::FindParentProject(IDispatch* pObject, IDispatch** ppParentProject)
{
	*ppParentProject = NULL;

	CComQIPtr<VCProject> spProject = pObject;
	if (spProject)	// top level filter
	{
		CComQIPtr<IDispatch> spDispProject = spProject;
		*ppParentProject = spDispProject.Detach();
		return S_OK;
	}

	CComPtr<VCFilter> pFilterLast;
	CComQIPtr<VCFilter> pFilterT = pObject;
	RETURN_ON_NULL2(pFilterT, E_UNEXPECTED);	// nothing we can do

	do
	{ // loop ends with pFilterT == NULL 
		CComPtr<IDispatch> pDisp;
		pFilterT->get_Parent(&pDisp);
		pFilterLast = pFilterT;
		pFilterT = pDisp;
	} while (pFilterT);
	
	if (pFilterLast != NULL)
		return pFilterLast->get_Parent(ppParentProject);

	return E_UNEXPECTED;
}

STDMETHODIMP CFilter::Remove()
{
	CHECK_OK_TO_DIRTY(0);
	return DoRemoveContents(TRUE);
}

STDMETHODIMP CFilter::RemoveContents()
{
	// CHECK_OK_TO_DIRTY should have been called long before we got here...
	return DoRemoveContents(FALSE);
}

HRESULT CFilter::DoRemoveContents(BOOL bTellParent)
{
	CComPtr<IDispatch> pProjDisp;
	HRESULT hr = get_Project(&pProjDisp);
	CComQIPtr<VCProject> pProject = pProjDisp;
	CComQIPtr<VCFilter> spFilterParent = m_pParent;
	CComQIPtr<VCProject> spProjectParent = m_pParent;

	long l;
	long lcItems = m_rgFiles.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComPtr<VCFile> pFile;
		pFile = m_rgFiles.GetAt(l);
		if (pFile)
		{
			CComQIPtr<IDispatch> pDispFile = pFile;
			pProject->RemoveFile(pDispFile);
		}
	}

	lcItems = m_rgFilters.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComQIPtr<IDispatch> pFilter = m_rgFilters.GetAt(l);
		if (pFilter == NULL)
			continue;
		RemoveExistingFilter(pFilter, l);
	}

	m_pParent = NULL;
	m_rgFiles.RemoveAll();	// collection of files
	m_rgFilters.RemoveAll();	// collection of filters

	if (bTellParent)
	{
		if (spFilterParent)
			spFilterParent->RemoveFilter(static_cast<IDispatch *>(this));
		else if (spProjectParent)		
			spProjectParent->RemoveFilter(static_cast<IDispatch *>(this));
		else
			VSASSERT(FALSE, "Hey, filter's parent must be a project or another filter!");
	}

	return S_OK;
}

STDMETHODIMP CFilter::AddWebReference(BSTR bstrUrl, IDispatch** ppFile)
{
	CComPtr<IDispatch> spDispProject;
	HRESULT hr = get_Project(&spDispProject);
	CComQIPtr<IVCProjectImpl> spProject = spDispProject;
	RETURN_ON_FAIL_OR_NULL(hr, spProject);

	hr = spProject->AddWSDL(bstrUrl, (VCFilter*)this, ppFile);
	return hr;
}

STDMETHODIMP CFilter::get_Kind(BSTR* kind)
{
	CHECK_POINTER_VALID(kind);
	CComBSTR bstrKind = L"VCFilter";
	*kind = bstrKind.Detach();
	return S_OK;
}

STDMETHODIMP CFilter::get_VCProjectEngine(IDispatch** ppProjEngine)
{
	CHECK_POINTER_VALID(ppProjEngine);
	RETURN_ON_NULL(g_pProjectEngine);
	return g_pProjectEngine->QueryInterface(IID_IDispatch, (void **)ppProjEngine);
}
