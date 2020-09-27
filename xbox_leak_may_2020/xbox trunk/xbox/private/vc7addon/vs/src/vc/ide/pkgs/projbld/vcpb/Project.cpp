// Project.cpp : Implementation of 

#include "stdafx.h"
#include "Project.h"
#include "file.h"
#include "configuration.h"
#include "filter.h"
#include "VCProjectEngine.h"
#include "Collection.h"
#include "vccoll.h"
#include "vcmap.h"
#include "BuildEngine.h"
#include "vctool.h"
#include "xmlfile.h"
#include "ProjWriter.h"
#include "util2.h"

class CInProjectClose
{
public:
	CInProjectClose() { m_bOldInClose = g_bInProjClose; g_bInProjClose = TRUE; }
	~CInProjectClose() { g_bInProjClose = m_bOldInClose; }
	BOOL m_bOldInClose;
};

/////////////////////////////////////////////////////////////////////////////
//
HRESULT CProject::CreateInstance(VCProject **ppDispInfo)
{
	HRESULT hr;
	CProject *pVar;
	CComObject<CProject> *pObj;
	hr = CComObject<CProject>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = pObj;
		pVar->AddRef();
		*ppDispInfo = pVar;
		
	}
	return hr;
}

CProject::CProject() :
	m_frh( NULL ),
	m_bDirty( VARIANT_FALSE ),
	m_pExternalCookie( NULL ),
	m_bAssignedActions( false ),
	m_pGlobals( NULL ),
	m_eFileFormat( eANSI ),
	m_bItemsCollectionDirty(true),
	m_wCloseFlag(closeFlagOpenClean)
{
}

CProject::~CProject()
{
	IDispatch* pDisp = this;
	CBldProjectRecord* pProjRecord = g_StaticBuildEngine.GetProjectRecord(pDisp, FALSE);
	VSASSERT(pProjRecord == NULL, "project record should have been disposed of on project close, not object destruction");
	if (pProjRecord)
		g_StaticBuildEngine.RemoveProjectRecord(pDisp);
	VSASSERT(m_frh == NULL, "file reg handle should have been disposed of on project close, not object destruction");
	if (m_frh)
		m_frh->ReleaseFRHRef();
	m_ProjectRegistry.SetFileRegistry(NULL);
	if( m_pGlobals )
		m_pGlobals->Release();
}

STDMETHODIMP CProject::Close()
{
	m_wCloseFlag |= closeFlagClosed;
	long l, lcItems;
	CInProjectClose inClose;

	if (m_bAssignedActions)
		UnAssignActions(VARIANT_TRUE);

	m_rgTopLevelItems.RemoveAll();
	m_bItemsCollectionDirty = true;

	lcItems = m_rgFiles.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComPtr<VCFile> pFile;
		pFile = m_rgFiles.GetAt(l);
		CComQIPtr<IVCFileImpl> pFileImpl = pFile;
		if (pFileImpl)
			pFileImpl->Close(); // close down entirely; release all
	}
	m_rgFiles.RemoveAll();		// collection of files for this project

	lcItems = m_rgConfigs.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComPtr<VCConfiguration> pConfiguration;
		pConfiguration = m_rgConfigs.GetAt(l);
		CComQIPtr<IVCConfigurationImpl> pConfigurationImpl = pConfiguration;
		if (pConfigurationImpl)
			pConfigurationImpl->Close(); // close down entirely; release all
	}
	m_rgConfigs.RemoveAll();	// collection of configs for this project

	lcItems = m_rgFilters.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComPtr<VCFilter> pFilter;
		pFilter = m_rgFilters.GetAt(l);
		CComQIPtr<IVCFilterImpl> pFilterImpl = pFilter;
		if (pFilterImpl)
			pFilterImpl->Close(); // close down entirely; release all
	}
	m_rgFilters.RemoveAll();		// collection of Filters for this project

	if (m_frh)
	{
		m_frh->ReleaseFRHRef();
		m_frh = NULL;
	}

	m_rgPlatforms.RemoveAll();	// collection of platforms for this project

	g_StaticBuildEngine.RemoveProjectRecord(this);

	return S_OK;
}

STDMETHODIMP CProject::AddPlatform(BSTR bstrPlatform)
{
	if (g_bInProjLoad)	// solution already checking for platforms being added
		return AddPlatformInternal(bstrPlatform);

	if (CVCProjectEngine::s_pBuildPackage != NULL)	// let the build package handle letting the solution in on this
		return CVCProjectEngine::s_pBuildPackage->AddPlatformToProject(this, bstrPlatform);

	return AddPlatformInternal(bstrPlatform);
}

STDMETHODIMP CProject::AddPlatformInternal(LPCOLESTR szPlatformName)
{
	// if no configs
	//	Add debug and release
	// else
	//for each platform
	// pCfg = CConfiguration::CreateInstance(pPlatform);
	// Add to config list
	// for each pFile
	//		pFile->AddConfig(pCfg)


	HRESULT hrLoop, hrT, hr;
	int iMac;
	int iIndex;
	CComPtr<IDispatch> pDispPlatform;
	CComQIPtr<VCPlatform> pPlatform;
	
	hr = VCPROJ_E_NOT_FOUND;
	iMac = m_rgPlatforms.GetSize();
	hrLoop = VCPROJ_E_NOT_FOUND;
	for (iIndex = 0; iIndex < iMac && hrLoop != S_FALSE; iIndex++)
	{
		pPlatform = m_rgPlatforms.GetAt(iIndex);
		if (pPlatform == NULL)
			continue;

		CComBSTR bstrName;
		hrT = pPlatform->get_Name(&bstrName);
		if (_wcsicmp(bstrName, szPlatformName) == 0)
		{
			hrLoop = S_FALSE;
			pDispPlatform = pPlatform;
			hr = S_FALSE;
		}
	}

	if (FAILED(hrLoop))
	{ // see if we have a platform of the same name in the global collection
		CComPtr<IDispatch> pDispPlatformList;
		hr = g_pProjectEngine->get_Platforms(&pDispPlatformList);
		CComQIPtr<IVCCollection> pPlatformList = pDispPlatformList;
		if (SUCCEEDED(hr) && pPlatformList)
		{
			long lcItemsToFetch;
			long i;

			pPlatformList->get_Count(&lcItemsToFetch);
			i = 1;
			while (lcItemsToFetch && hrLoop == VCPROJ_E_NOT_FOUND)
			{
				CComVariant varItem = i;
				CComPtr<IDispatch> spDispPlatform2;
				pPlatformList->Item(varItem, &spDispPlatform2);
				pDispPlatform = spDispPlatform2;
				pPlatform = pDispPlatform;
				if (pPlatform == NULL)
					continue;
				CComBSTR bstrName;
				pPlatform->get_Name(&bstrName);
				if (_wcsicmp(bstrName, szPlatformName) == 0)
				{
					hrLoop = S_OK;
					hr = S_OK;
				}
				lcItemsToFetch--;
				i++;
			}
		}
#ifdef NEW_FEATURE_LATER
		if (FAILED(hr))
		{	// create a new platform
			hr = g_pProjectEngine->AddPlatform(bstrPlatform, &pDispPlatform);
		}
#endif
	}

	if (hr == S_OK && pPlatform) // now create new project-configs for the new platform; S_FALSE means it is there already
	{
		CHECK_OK_TO_DIRTY(0);
		m_rgPlatforms.Add(pPlatform);
		// fire an event so the ui becomes aware of the new platform
		CComQIPtr<IVCProjectEngineImpl> pEngine = g_pProjectEngine;
		if( pEngine )
		{
			CComQIPtr<IDispatch> pDispPlatform = pPlatform;
			if( pDispPlatform )
				pEngine->DoFireItemAdded( pDispPlatform, static_cast<IDispatch*>(this) );
		}
		iMac = m_rgConfigs.GetSize();
		if (iMac == 0) // no configs yet added to this project!
		{
// 			// set up the project with default configs and platforms
//  			AddConfiguration(L"Debug");
//  			AddConfiguration(L"Release");
		}
		else
		{
			for (iIndex = 0; iIndex <= iMac; iIndex++)
			{
				CComPtr<VCConfiguration> pConfiguration = m_rgConfigs.GetAt(iIndex);
				if (pConfiguration == NULL)
					continue;
				CComBSTR bstrName;
				hrT = pConfiguration->get_ConfigurationName(&bstrName);
				VSASSERT(SUCCEEDED(hrT), "Configurations without names are generally zombie.");
				hrT = AddPlatformConfig(pDispPlatform, bstrName, m_rgPlatforms.GetSize() > 2);
				VSASSERT(SUCCEEDED(hrT), "Out of memory, perhaps?");
				if (hrT == S_FALSE)	// may be trying to add a config that is already there; if so, override the S_FALSE
					hrT = S_OK;
			}
		}
	}

	if (hr == VCPROJ_E_NOT_FOUND)
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NOT_FOUND, IDS_ERR_PLATFORM_NOT_FOUND, szPlatformName);

	return hr;
}

STDMETHODIMP CProject::AddConfigurationInternal(LPCOLESTR szConfigName)
{
	// check for empty string
	CStringW strConfigName = szConfigName;
	strConfigName.TrimLeft();
	strConfigName.TrimRight();
	if (strConfigName.IsEmpty())
		RETURN_INVALID();

	CHECK_OK_TO_DIRTY(0);

	HRESULT hr = VCPROJ_E_NO_PLATFORMS;
	int iIndex;
	int iMac  = m_rgPlatforms.GetSize();
	for (iIndex = 0; iIndex < iMac; iIndex++) // we only add a config for the platforms this project knows about
	{
		CComQIPtr<IDispatch> pDispPlatform = m_rgPlatforms.GetAt(iIndex);
		if (pDispPlatform == NULL)
			continue;

		hr = AddPlatformConfig(pDispPlatform, strConfigName, m_rgPlatforms.GetSize() > 1);
		VSASSERT(SUCCEEDED(hr), "Out of memory, perhaps?");
		if (hr == S_FALSE)	// get around trying to add same config twice...
			hr = S_OK;
	}

	if (hr == VCPROJ_E_NO_PLATFORMS)
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NO_PLATFORMS, IDS_ERR_NO_PROJ_PLATFORMS, m_bstrName);
	return hr;
}

STDMETHODIMP CProject::AddConfiguration(BSTR bstrConfigName)
{
	if (g_bInProjLoad)	// solution already checking for configs being added
		return AddConfigurationInternal(bstrConfigName);

	if (CVCProjectEngine::s_pBuildPackage != NULL)	// let the build package handle letting the solution in on this
		return CVCProjectEngine::s_pBuildPackage->AddConfigurationToProject(this, bstrConfigName);

	return AddConfigurationInternal(bstrConfigName);
}

HRESULT CProject::FindFile(BSTR bstrPath, IDispatch** ppDisp, BOOL bSetErrorInfo)
{
	*ppDisp = NULL;		// this is an internal function, so this pointer BETTER be valid...

	// check for empty string
	if( !bstrPath || *bstrPath == L'\0' )
		return CVCProjectEngine::DoSetErrorInfo2(E_INVALIDARG, IDS_ERR_BAD_FILE_NAME, bstrPath, bSetErrorInfo);

	//	Check for duplicate
	int i, iMac;
	VARIANT_BOOL bMatch;

	if( g_bInProjLoad == FALSE )
	{
		// don't validate if loading a project
		iMac = m_rgFiles.GetSize();
		for (i = 0; i < iMac; i++)
		{
			CComPtr<VCFile> pFile;
			pFile = m_rgFiles.GetAt(i); 
			if (pFile)
			{
				HRESULT hr = pFile->MatchName(bstrPath, VARIANT_FALSE, &bMatch);
				if (SUCCEEDED(hr) && bMatch)
					return pFile->QueryInterface(ppDisp);
			}
		}
	}

	return S_OK;
}

HRESULT CProject::CheckCanAddFile(BSTR bstrFile, BOOL bSetErrorInfo)
{
	CComPtr<IDispatch> spExistingFile;
	HRESULT hr = FindFile(bstrFile, &spExistingFile, bSetErrorInfo);
	RETURN_ON_FAIL(hr);
	if (spExistingFile)	// already exists
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_FILE_EXISTS, IDS_ERR_FILE_EXISTS, bstrFile, bSetErrorInfo);
		
	hr = FileNameValid(bstrFile, bSetErrorInfo ? VARIANT_TRUE : VARIANT_FALSE);
	RETURN_ON_FAIL(hr);

	// it doesn't already exist, and it's a valid name, 
	// need to pass the full path through
	CDirW dir;
	dir.CreateFromString( m_bstrFullDir );
	CPathW path;
	path.CreateFromDirAndFilename( dir, bstrFile );
	CComBSTR bstrFullPath = path.GetFullPath();

	CHECK_OK_TO_DIRTY2(0, bSetErrorInfo);

	return QueryCanAddFile(this, NULL, bstrFullPath, bSetErrorInfo);
}

STDMETHODIMP CProject::CanAddFile(BSTR bstrFile, VARIANT_BOOL* pbCanAdd)
{
	CHECK_POINTER_VALID(pbCanAdd);
	
	HRESULT hr = CheckCanAddFile(bstrFile, TRUE);
	*pbCanAdd = FAILED(hr) ? VARIANT_FALSE : VARIANT_TRUE;

	return SUCCEEDED(hr) ? S_OK : S_FALSE;
}

STDMETHODIMP CProject::CanAddFile2(BSTR bstrFile)
{
	return CheckCanAddFile(bstrFile, FALSE);
}

STDMETHODIMP CProject::AddFile(BSTR bstrPath, /*[out, retval]*/ IDispatch * *ppDisp)
{
	CHECK_POINTER_VALID(ppDisp);

	HRESULT hr = CheckCanAddFile(bstrPath, TRUE);
	RETURN_ON_FAIL(hr);

	//	Add To list of files
	CComPtr<VCFile> pFile;
	CComQIPtr<IDispatch> pDisp;
	CComQIPtr<IDispatch> pDispParent;

	hr = CPEFile::CreateInstance(&pFile); 
	VSASSERT(pFile, "Out of memory, perhaps?");
	RETURN_ON_FAIL_OR_NULL2(hr, pFile, E_OUTOFMEMORY);

	hr = AddNewFile(pFile, bstrPath, VARIANT_FALSE);
	RETURN_ON_FAIL(hr);

	pDisp = pFile;
	VSASSERT(pDisp, "VCFile derives from IDispatch (deep doodoo if it does not anymore).  Out of memory, perhaps?");
	hr = pDisp.CopyTo(ppDisp);
	
	// only look for filter if we're not loading
	// (e.g. during load put the file in exactly the filter the project file
	// says that it's in)
	if( !g_bInProjLoad )
	{
		// find out what Filter to put this in
		CComPtr<VCFilter> pVCFilter;
		CComPtr<IVCFilterImpl> pVCFilterImpl;
		hr = FindFilter(bstrPath, &pVCFilter);
		pVCFilterImpl = pVCFilter;
		if (SUCCEEDED(hr) && pVCFilterImpl)
		{
			hr = pVCFilterImpl->AddItem(pDisp);
			pDispParent = pVCFilter;
			VSASSERT(SUCCEEDED(hr), "Uh.  Why did VCFilter::AddItem fail after VCProject::AddFile succeeded?");
		}
	}

	// fire events
	CComQIPtr<IVCProjectEngineImpl> pEngine;

	pEngine = g_pProjectEngine;
	if (pEngine)
	{
		if(pDispParent == NULL)
			pEngine->DoFireItemAdded(pDisp, (IDispatch*)this);
		else
			pEngine->DoFireItemAdded(pDisp, pDispParent);
	}
	InformFileAdded(this, pFile, bstrPath);

	return hr;
}

STDMETHODIMP CProject::AddNewFile(IDispatch *pFile, LPCOLESTR szRelativePath, VARIANT_BOOL bFireEvents)
{
	CComQIPtr<VCFile> pVCFile;
	CComQIPtr<IVCFileImpl> pVCFileImpl;

	CHECK_OK_TO_DIRTY(0);

	pVCFile = pFile;
	pVCFileImpl = pFile;
	VSASSERT(pFile, "Was pFile NULL when we got here?");
	VSASSERT(pVCFile && pVCFileImpl, "File object must support both VCFile and IVCFileImpl interfaces.");
	RETURN_ON_NULL2(pVCFile, E_UNEXPECTED);

	{
		CComPtr<IDispatch> pProject;
		pVCFile->get_Project(&pProject);
		if (pProject)
			return S_OK;
	}

	HRESULT hr = pVCFileImpl->put_Parent(this);
	VSASSERT(SUCCEEDED(hr), "Out of memory, perhaps?");
	hr = pVCFileImpl->put_Project(this);
	VSASSERT(SUCCEEDED(hr), "Out of memory, perhaps?");

	CStringW strRelPathTmp = szRelativePath;
	strRelPathTmp.TrimLeft();
	strRelPathTmp.TrimRight();
	if (!strRelPathTmp.IsEmpty())
	{
		// if we're not in project load, 
		if( !g_bInProjLoad )
		{
			// make sure this path is relative
			CDirW dir;
			dir.CreateFromString( m_bstrFullDir );
			CPathW path;
			path.CreateFromDirAndFilename( dir, szRelativePath );
			path.SetAlwaysRelative( true );
			CStringW strRelPath; 
			path.GetRelativeName( dir, strRelPath );
			pVCFile->put_RelativePath( CComBSTR( strRelPath ) );
		}
		else
			hr = pVCFile->put_RelativePath(CComBSTR(strRelPathTmp));
	}


	// add all our configs to this file
	{	// scope loadProj below
		CInLoadProject loadProj;
		int i, iMac;
		iMac = m_rgConfigs.GetSize();
		for (i = 0; i <= iMac; i++)
		{
			CComPtr<VCConfiguration> pConfig;
			pConfig = m_rgConfigs.GetAt(i);
			if (pConfig)
				pVCFileImpl->AddConfiguration(pConfig);
		}
	}
	m_rgFiles.Add(pVCFile);
	m_bItemsCollectionDirty = true;
	if (!g_bInProjLoad)
	{
		CComQIPtr<IVCBuildableItem> spBuildableFile = pVCFile;
		if (spBuildableFile)
			spBuildableFile->AssignActions(VARIANT_TRUE);
	}

	// fire events
	if (bFireEvents == VARIANT_TRUE)
	{
		CComQIPtr<IVCProjectEngineImpl> pEngine = g_pProjectEngine;
		if (pEngine)
		{
			CComPtr<IDispatch> pDispParent;
			pVCFile->get_Parent(&pDispParent);
			if (pDispParent == NULL)
				pEngine->DoFireItemAdded(pFile, (IDispatch*)this);
			else
				pEngine->DoFireItemAdded(pFile, pDispParent);
		}
	}

	return hr;
}

STDMETHODIMP CProject::AddChild(IDispatch* pChild)
{
	CComQIPtr<VCFile> spFile = pChild;
	if (spFile != NULL)
	{
		CComQIPtr<IVCFileImpl> spFileImpl = spFile;
		if (spFileImpl)
			spFileImpl->put_Parent(this);
		// should already be in the files collection
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

STDMETHODIMP CProject::RemoveChild(IDispatch* pChild)
{
	CComQIPtr<VCFile> spFile = pChild;
	if (spFile != NULL)	
	{
		CComQIPtr<IVCFileImpl> spFileImpl = spFile;
		if (spFileImpl)
			spFileImpl->put_Parent(NULL);
		// do NOT remove from the files collection...
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

HRESULT CProject::FindFilter(BSTR bstrFileName, VCFilter **ppFilter)
{
	HRESULT hrLoop = VCPROJ_E_NOT_FOUND;
	HRESULT hr = S_FALSE;
	*ppFilter = NULL;

	// find out what Filter to put this in
	long lItem, lcItems;
	VARIANT_BOOL bIsMatch;

	lcItems = m_rgFilters.GetSize();
	bIsMatch = false;
	for (lItem = 0; lItem < lcItems && FAILED(hrLoop) && bIsMatch == false; lItem++)
	{
		CComPtr<VCFilter> pVCFilter;
		CComQIPtr<IVCFilterImpl> pVCFilterImpl;
		pVCFilter = m_rgFilters.GetAt(lItem);
		pVCFilterImpl = pVCFilter;
		if (pVCFilter && pVCFilterImpl)
		{
			hrLoop = FindSubFilter(bstrFileName, pVCFilter, ppFilter);
			if (SUCCEEDED(hrLoop))
			{
				hr = pVCFilter.CopyTo(ppFilter);
			}
		}
	}

	if (hr == VCPROJ_E_NOT_FOUND)
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NOT_FOUND, IDS_ERR_FILTER_NOT_FOUND, bstrFileName);

	return hr;
}

HRESULT CProject::FindSubFilter(BSTR bstrFileName, VCFilter *pFilterStart, VCFilter **ppFilter)
{
	CHECK_READ_POINTER_NULL(pFilterStart);
	CHECK_POINTER_NULL(ppFilter);

	CComPtr<IEnumVARIANT> pEnumFilters;
	CComPtr<IDispatch> pDispCollection;
	CComQIPtr<IVCCollection> pCollection;
	VARIANT_BOOL bMatch = VARIANT_FALSE;
	CComQIPtr<IVCFilterImpl> pFilterStartImpl;

	pFilterStartImpl = pFilterStart;
	CHECK_READ_POINTER_NULL(pFilterStartImpl);

	// check to see if start Filter matches
	HRESULT hr = pFilterStartImpl->IsMatch(bstrFileName, &bMatch);

	if (bMatch == VARIANT_FALSE)
	{
		hr = pFilterStart->get_Filters(&pDispCollection);
		pCollection = pDispCollection;
		if (SUCCEEDED(hr) && pCollection)
		{
			pCollection->_NewEnum(reinterpret_cast<IUnknown **>(&pEnumFilters));
		}
		if (SUCCEEDED(hr) && pEnumFilters)
		{
			unsigned long lcItems;
			CComVariant varFilter;
			HRESULT hrT = VCPROJ_E_NOT_FOUND;

			hr = pEnumFilters->Next(1, &varFilter, &lcItems);
			while (SUCCEEDED(hr) && lcItems == 1 && FAILED(hrT))
			{
				CComQIPtr<VCFilter> pVCFilter;
				CComQIPtr<IVCFilterImpl> pVCFilterImpl;

				if (varFilter.vt == VT_UNKNOWN)
				{
					pVCFilter = varFilter.punkVal;
					pVCFilterImpl = pVCFilter;
					VSASSERT(pVCFilterImpl, "NULL filter object in collection with .vt set correctly?!?");
					if (pVCFilter && pVCFilterImpl)
					{
						hr = pVCFilterImpl->IsMatch(bstrFileName, &bMatch);
						if (bMatch == VARIANT_FALSE)
						{
							hrT = FindSubFilter(bstrFileName, pVCFilter, ppFilter); // recurse!
						}
						else
						{
							hrT = S_OK;
							pVCFilter.CopyTo(ppFilter);
						}
					}
				}
				hr = pEnumFilters->Next(1, &varFilter, &lcItems);
			}
			hr = hrT;
			
		}
	}

	if (hr == VCPROJ_E_NOT_FOUND)
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_NOT_FOUND, IDS_ERR_FILTER_NOT_FOUND, bstrFileName);

	return hr;
}

HRESULT CProject::GetConfigFromVariant(VARIANT varConfiguration, IDispatch **ppDispVal)
{
	HRESULT hr;
	int iMac;
	int iIndex;
	CComPtr<VCConfiguration> pConfig;
	CComQIPtr<IDispatch> pDisp;
	CComBSTR bstrName;

	hr = E_NOINTERFACE;
	*ppDispVal = NULL;
	if (varConfiguration.vt == VT_EMPTY)
	{
		hr = S_FALSE; // ALL configs!
	}
	else
	{
		iMac = m_rgConfigs.GetSize();
		for (iIndex = 0; iIndex <= iMac && hr == S_FALSE; iIndex++)
		{
			pConfig = m_rgConfigs.GetAt(iIndex);
			pDisp = pConfig;
			if (pConfig && pDisp)
			{
				switch (varConfiguration.vt)
				{
				case VT_DISPATCH:
					if (pDisp == varConfiguration.pdispVal)
					{
						hr = S_OK;
					}
					break;
				case VT_BSTR:
					bstrName.Empty();
					pConfig->get_Name(&bstrName);
					if (bstrName == varConfiguration.bstrVal)
					{
						hr = S_OK;
					}
					break;
				}
			}
		}
		pDisp.CopyTo(ppDispVal);
	}
	return hr;
}

HRESULT CProject::AddPlatformConfig(IDispatch *pDispPlatform, LPCOLESTR bstrConfigName, BOOL bSearchExisting)
{ // WARNING: CONTAINS EARLY RETURNS
	HRESULT hr;
	CComBSTR bstrConfigFlavorName;
	CComBSTR bstrPlatformName;
	CComBSTR bstrName;
	CComQIPtr<VCPlatform> pPlatform;
	CComPtr<VCConfiguration> pConfig;
	HRESULT hrT;
	int iMac;
	int iIndex;

	pPlatform = pDispPlatform;
	hr = E_NOINTERFACE;
	if (pPlatform)
	{

		// see if this project, for some reason, already has this project-config
		if (bSearchExisting)
		{
			pPlatform->get_Name(&bstrPlatformName);
			bstrConfigFlavorName = bstrConfigName;
			bstrConfigFlavorName.Append(L"|");
			bstrConfigFlavorName.Append(bstrPlatformName);
			iMac = m_rgConfigs.GetSize();
			for (iIndex = 0; iIndex <= iMac; iIndex++)
			{
				pConfig = m_rgConfigs.GetAt(iIndex);
				if (pConfig)
				{
					bstrName.Empty();
					hrT = pConfig->get_Name(&bstrName);
					if (_wcsicmp(bstrName, bstrConfigFlavorName) == 0)
						return S_FALSE; // WARNING: EARLY RETURN
				}
			}
		}

		CHECK_OK_TO_DIRTY(0);

		{
			// fake the everything into thinking we're loading, so that they
			// don't do AssignActions (we'll handle it afterwards)
			CInLoadProject loadProj;

			// configuration didn't exist. create a new one and set it up.
			hr = CConfiguration::CreateInstance(&pConfig, bstrConfigName, pPlatform, this);
			// Add the config 
			m_rgConfigs.Add(pConfig);
			VSASSERT(pConfig, "Out of memory, perhaps?"); // should have one right now

			// add a file config for each file in the project
			CComPtr<IDispatch> pDispColl;
			hr = get_Files( &pDispColl );
			CComQIPtr<IVCCollection> pColl = pDispColl;
			RETURN_ON_FAIL_OR_NULL2(hr, pColl, E_NOINTERFACE);
			long num_files;
			hr = pColl->get_Count( &num_files );
			RETURN_ON_FAIL(hr);
			for( long i = 1; i <= num_files; i++ )
			{
				CComPtr<IDispatch> pDisp;
				hr = pColl->Item( CComVariant( i ), &pDisp );
				if( FAILED( hr ) )
					continue;
				CComQIPtr<IVCFileImpl> pFileImpl = pDisp;
				if( !pFileImpl )
					continue;
				pFileImpl->AddConfiguration( pConfig );
			}
			// fire an event so the ui becomes aware of the new config
			CComQIPtr<IVCProjectEngineImpl> pEngine = g_pProjectEngine;
			if( pEngine )
			{
				CComQIPtr<IDispatch> pDispConfig = pConfig;
				if( pDispConfig )
					pEngine->DoFireItemAdded( pDispConfig, static_cast<IDispatch*>(this) );
			}
		}	// reset whether we're loading or not

		if (!g_bInProjLoad)
		{
			CComQIPtr<IVCBuildableItem> spBuildableCfg = pConfig;
			VSASSERT(spBuildableCfg, "Configurations of any stripe must support IVCBuildableItem interface.");
			if (spBuildableCfg)
				spBuildableCfg->AssignActions(VARIANT_FALSE);
		}
	}
	return hr;
}

HRESULT CProject::CheckCanAddFilter(BSTR bstrFilter, CStringW& strCleanedUpName, BOOL bSetErrorInfo)
{
	CComPtr<IDispatch> spFilter;
	HRESULT hr = FindExistingFilter(bstrFilter, strCleanedUpName, &spFilter, bSetErrorInfo);
	RETURN_ON_FAIL(hr);
	if (spFilter)	// already there
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_FILTER_EXISTS, IDS_ERR_FILTER_EXISTS, bstrFilter, bSetErrorInfo);

	CHECK_OK_TO_DIRTY(0);

	return S_OK;
}

STDMETHODIMP CProject::CanAddFilter(BSTR bstrFilter, VARIANT_BOOL* pbCanAdd)
{
	CHECK_POINTER_VALID(pbCanAdd);

	CStringW strCleanedUpName;
	HRESULT hr = CheckCanAddFilter(bstrFilter, strCleanedUpName, TRUE);
	*pbCanAdd = FAILED(hr) ? VARIANT_FALSE : VARIANT_TRUE;

	return SUCCEEDED(hr) ? S_OK : S_FALSE;
}

STDMETHODIMP CProject::CanAddFilter2(BSTR bstrFilter, VARIANT_BOOL bSetErrorInfo)
{
	CStringW strCleanedUpName;
	return CheckCanAddFilter(bstrFilter, strCleanedUpName, (bSetErrorInfo == VARIANT_TRUE));
}

HRESULT CProject::FindExistingFilter(BSTR bstrFilterName, CStringW& strCleanedUpName, IDispatch** ppDisp, BOOL bSetErrorInfo)
{
	*ppDisp = NULL;		// this had BETTER be valid since this is an internal function...

	// Validate: Is this a valid name ?
	strCleanedUpName = bstrFilterName;
	strCleanedUpName.TrimLeft();
	strCleanedUpName.TrimRight();
	if( strCleanedUpName.GetLength() == 0 )
		return CVCProjectEngine::DoSetErrorInfo2(E_INVALIDARG, IDS_ERR_BAD_FILTER_NAME, bstrFilterName, bSetErrorInfo);

	// validate: is there a folder with this name already?
	int iMac = m_rgFilters.GetSize();
	for (int iIndex = 0; iIndex <= iMac; iIndex++)
	{
		CComPtr<VCFilter> pFilterT = m_rgFilters.GetAt(iIndex);
		if (pFilterT)
		{
			CComBSTR bstrExistingName;
			pFilterT->get_Name(&bstrExistingName);
			if( bstrExistingName && wcsicmp( bstrExistingName, strCleanedUpName ) == 0 )
			{
				pFilterT.QueryInterface(ppDisp);
				return S_FALSE;
			}
		}
	}

	return S_OK;
}

STDMETHODIMP CProject::AddFilter(BSTR bstrFilterName, /*[out, retval]*/ IDispatch * *ppDisp)
{
	CHECK_POINTER_VALID(ppDisp);

	CStringW strCleanedUpName;
	HRESULT hr = CheckCanAddFilter(bstrFilterName, strCleanedUpName, TRUE);
	RETURN_ON_FAIL(hr);
	CComBSTR bstrFilter = strCleanedUpName;

	CComQIPtr<VCFilter> pFilter;
	hr = CFilter::CreateInstance(&pFilter);
	if (SUCCEEDED(hr) && pFilter)
	{
		CHECK_OK_TO_DIRTY(0);

		// Add to filter list
		m_rgFilters.Add(pFilter);
		m_bItemsCollectionDirty = true;
		CComQIPtr<IVCFilterImpl> pFilterImpl = pFilter;
		if (pFilterImpl)
			pFilterImpl->put_Parent(this);
		pFilter->put_Name(bstrFilter);
		pFilter.QueryInterface(ppDisp);

		CComQIPtr<IVCProjectEngineImpl> pEngine;
		pEngine = g_pProjectEngine;
		CComPtr<IDispatch> pDispParent;
		pFilter->get_Parent(&pDispParent);
		if(pDispParent == NULL)
			pEngine->DoFireItemAdded(pFilter, static_cast<IDispatch*>(this));
		else
			pEngine->DoFireItemAdded(pFilter, pDispParent);
	}
	return hr;
}

STDMETHODIMP CProject::get_IsDirty(/*[out, retval]*/ VARIANT_BOOL *pBoolDirty)
{
	CHECK_POINTER_VALID( pBoolDirty );
	*pBoolDirty = m_bDirty;
	return S_OK;
}

HRESULT CProject::WrapCanDirty()	// just need to wrap the CHECK_OK_TO_DIRTY macro
{
	CHECK_OK_TO_DIRTY(0);
	return S_OK;
}

STDMETHODIMP CProject::get_CanDirty(VARIANT_BOOL* pbCanDirty)
{
	HRESULT hr = WrapCanDirty();
	if (pbCanDirty)	// optional pointer
	{
		CHECK_POINTER_VALID(pbCanDirty);
		if (SUCCEEDED(hr))
			*pbCanDirty = VARIANT_TRUE;
		else
			*pbCanDirty = VARIANT_FALSE;
	}
	return hr;
}

STDMETHODIMP CProject::put_IsDirty(/*[in]*/  VARIANT_BOOL boolDirty)
{
	if (g_bInProjLoad)	// can't dirty while in project load
		return S_OK;

	return put_IsConverted(boolDirty);
}

STDMETHODIMP CProject::put_IsConverted(VARIANT_BOOL boolDirty)
{
	// can force it dirty even during project load if we're converting a multi-project .mdp file...
	if( boolDirty && !m_bDirty )
	{
		// fire event to notify scc etc 
		CComQIPtr<IVCProjectEngineImpl> pEngine;
		pEngine = g_pProjectEngine;
		VSASSERT( pEngine, "Deep trouble if project engine not available here..." );
		VARIANT_BOOL bRetVal;
		m_wCloseFlag = closeFlagMaybeSCCClose;
		pEngine->DoFireSccEvent( static_cast<IDispatch*>(this), ePreDirtyNotification, &bRetVal );
		if (m_wCloseFlag != closeFlagBySCC)
			m_wCloseFlag = closeFlagOpenClean;
		if( bRetVal == VARIANT_FALSE )
			return E_ACCESSDENIED;
	}
	if(	m_bDirty != boolDirty )
		m_bDirty = boolDirty;

	if (m_wCloseFlag == closeFlagBySCC)
		return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_PROJ_RELOADED, IDS_ERR_PROJ_RELOADED, m_bstrName, TRUE /* report */);

	return S_OK;
}

STDMETHODIMP CProject::put_OwnerKey( BSTR bstrName )
{
	CHECK_OK_TO_DIRTY(0);
	m_bstrOwnerKey = bstrName;
	return S_OK;
}

STDMETHODIMP CProject::get_OwnerKey( BSTR *pbstrName )
{
	CHECK_POINTER_VALID( pbstrName );
	return m_bstrOwnerKey.CopyTo( pbstrName );
}

STDMETHODIMP CProject::put_ProjectGUID( BSTR bstrGuid )
{
	CHECK_OK_TO_DIRTY(0);
	m_bstrGuid = bstrGuid;
	return S_OK;
}

STDMETHODIMP CProject::get_ProjectGUID( BSTR *pbstrGuid )
{
	CHECK_POINTER_VALID( pbstrGuid );
	return m_bstrGuid.CopyTo( pbstrGuid );
}

STDMETHODIMP CProject::Save()
{
	if (CVCProjectEngine::s_pBuildPackage != NULL)
		return CVCProjectEngine::s_pBuildPackage->SaveProject(this, NULL);

	return DoSave(NULL);
}

// helper function to sort files
static int FileCompare( const void* fileA, const void* fileB )
{
	CComBSTR bstrNameA, bstrNameB;
	(*(VCFile**)(fileA))->get_Name( &bstrNameA );
	(*(VCFile**)(fileB))->get_Name( &bstrNameB );
	return wcscoll( bstrNameA, bstrNameB );
}

STDMETHODIMP CProject::DoSave(LPCOLESTR szFileName)
{
	// determine the file name
	CComBSTR bstrFile = szFileName;
	if (bstrFile.Length() == 0)	// do a Save rather than a SaveAs
		bstrFile = m_bstrFileName;
	else
		put_ProjectFile(bstrFile);

	// create an XMLFile object
	CComPtr<IVCXMLFile> xmlfile;
	CXMLFile::AdjustFileEncodingForFileFormat(m_eFileFormat, m_eFileFormat, m_bstrFileEncoding, TRUE);
	HRESULT hr = CXMLFile::CreateInstance( &xmlfile, bstrFile, 1024 * 1024, m_bstrFileEncoding, m_eFileFormat );
	CComQIPtr<IStream> xml = xmlfile;
	VSASSERT( xml, "QI for IStream on an IVCXMLFile failed!" );
	RETURN_ON_FAIL_OR_NULL(hr, xml);

	
	// write header
	CStringW strEncoding = m_bstrFileEncoding;
	CStringW strHeaderFormat = L"<?xml version=\"1.0\" encoding = \"%s\"?>\r\n";
	CStringW strHeader;
	strHeader.Format(strHeaderFormat, strEncoding);
	CComBSTR bstrHeader = strHeader;

	unsigned long bytesWritten;
	xml->Write( (void*)bstrHeader, (int) wcslen( bstrHeader ) * sizeof(OLECHAR), &bytesWritten );

	// write project node
	StartNodeHeader( xml, L"VisualStudioProject", true );
	SaveObject( xml, NULL, GetIndent() );
	EndNodeHeader( xml, true );

	// Build node (for vb format compliance only)
	StartNodeHeader( xml, L"Platforms", true );
	EndNodeHeader( xml, true );


	// write platform information
	long lcItems, lItem;
	lcItems = m_rgPlatforms.GetSize();
	for( lItem = 0; lItem < lcItems; lItem++ )
	{
		CComQIPtr<VCPlatform> pPlatform;
		CComQIPtr<IVCPlatformImpl> pPlatformImpl;
		pPlatform = m_rgPlatforms.GetAt( lItem );
		pPlatformImpl = pPlatform;
		if( pPlatform && pPlatformImpl )
		{
			StartNodeHeader( xml, L"Platform", false );
			hr = pPlatformImpl->SaveObject( xml, NULL, GetIndent() );
			EndNode( xml, L"Platform", false );
		}
	}
	// write the end of the Settings node (node for vb format compliance)
	EndNode( xml, L"Platforms", true );

	// Settings node (for vb format compliance only)
	StartNodeHeader( xml, L"Configurations", true );
	EndNodeHeader( xml, true );
	
	// write config information
	lcItems = m_rgConfigs.GetSize();
	for( lItem = 0; lItem < lcItems; lItem++ )
	{
		CComQIPtr<VCConfiguration> pConfig;
		CComQIPtr<IVCConfigurationImpl> pConfigImpl;
		pConfig = m_rgConfigs.GetAt( lItem );
		pConfigImpl= pConfig;
		if( pConfig && pConfigImpl )
		{
			StartNodeHeader( xml, L"Configuration", true ); 			
			hr = pConfigImpl->SaveObject( xml, NULL, GetIndent() );
			EndNode( xml, L"Configuration", true );
		}
	}


	// write the end of the Build node (node for vb format compliance)
	EndNode( xml, L"Configurations", true );

	// Files node (for vb format compliance only)
	StartNodeHeader( xml, L"Files", true );
	EndNodeHeader( xml, true );

	// write filter information
	lcItems = m_rgFilters.GetSize();
	for( lItem = 0; lItem < lcItems; lItem++ )
	{
		CComQIPtr<VCFilter> pVCFilter;
		CComQIPtr<IVCFilterImpl> pVCFilterImpl;
		pVCFilter = m_rgFilters.GetAt( lItem );
		pVCFilterImpl= pVCFilter;
		if( pVCFilter && pVCFilterImpl )
		{
			StartNodeHeader( xml, L"Filter", true );
			hr = pVCFilterImpl->SaveObject( xml, NULL, GetIndent() );
			EndNode( xml, L"Filter", true );
		}
	}

	// sort the files alphabetically before writing
	// allocate an array of ptrs
	VCFile **rgFiles = new VCFile*[m_rgFiles.GetCount()];
	// copy the ptrs (NON-REFCOUNTED!) into the array
	lcItems = m_rgFiles.GetSize();
	int n = 0;
	for( lItem = 0; lItem < lcItems; lItem++ )
	{
		VCFile* pFile = m_rgFiles.GetAt( lItem );
		if (pFile == NULL)
			continue;

		CComPtr<IDispatch> pParent;
		hr = pFile->get_Parent( &pParent );
		VSASSERT(SUCCEEDED(hr), "File without parent generally means file is already zombie.");
		// add only files that are on the project node, not in folders!
		if( pParent == NULL || pParent == static_cast<IDispatch *>(this) ) 
			rgFiles[n++] = pFile;
	}
	// sort the array
	qsort( (void*)rgFiles, n, sizeof(VCFile*), FileCompare );
	
	// write file information for files not in filters
	lcItems = n;
	for( lItem = 0; lItem < lcItems; lItem++ )
	{
		CComQIPtr<VCFile> pFile;
		CComQIPtr<IVCFileImpl> pFileImpl;
		pFile = rgFiles[lItem];
		pFileImpl = pFile;
		if( pFile && pFileImpl )
		{
			StartNodeHeader( xml, L"File", true );
			hr = pFileImpl->SaveObject( xml, NULL, GetIndent() );
			EndNode( xml, L"File", true );
		}
	}
	delete[] rgFiles;

	// write the end of the Files node (node for vb format compliance)
	EndNode( xml, L"Files", true );

	// write globals
	if( m_pGlobals )
	{
		StartNodeHeader( xml, L"Globals", true );
		EndNodeHeader( xml, true );
		hr = m_pGlobals->SaveObject( xml, NULL, GetIndent() );
		EndNode( xml, L"Globals", true );
	}
	
	// write the end of the project node
	EndNode( xml, L"VisualStudioProject", true );

	hr = xml->Commit(0);
	RETURN_ON_FAIL(hr);

	// "un-dirty" the project
	put_IsDirty( VARIANT_FALSE );

	return hr;
}

STDMETHODIMP CProject::GetGlobals(IVCGlobals **ppGlobals)
{
	// do we have a globals object yet?
	if( !m_pGlobals )
	{
		CVCGlobals *pGlobals = NULL;
		HRESULT hr = CVCGlobals::CreateInstance( &pGlobals );
		VSASSERT( SUCCEEDED( hr ) && pGlobals, "Unable to initialize CAutoProject: Can't create Globals object!");
		m_pGlobals = (IVCGlobals*)pGlobals;
	}
	// add ref for return (this is a raw pointer)
	m_pGlobals->AddRef();
	*ppGlobals = m_pGlobals;
	return S_OK;
}

STDMETHODIMP CProject::SaveObject( IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent )
{
	// write ProjectType, Version and Name nodes
	NodeAttribute( xml, L"ProjectType", CComBSTR( L"Visual C++" ) );
	NodeAttribute( xml, L"Version", CComBSTR( L"7.00" ) );
	NodeAttributeWithSpecialChars( xml, L"Name", m_bstrName );

	// write the OwnerKey prop, if there is one
	NodeAttribute( xml, L"OwnerKey", m_bstrOwnerKey );
	NodeAttribute( xml, L"ProjectGUID", m_bstrGuid );

	// write SCC strings if the project is under SCC
	NodeAttributeWithSpecialChars( xml, L"SccProjectName", m_bstrSccProjectName );
	NodeAttributeWithSpecialChars( xml, L"SccAuxPath", m_bstrSccAuxPath );
	NodeAttributeWithSpecialChars( xml, L"SccLocalPath", m_bstrSccLocalPath );
	NodeAttributeWithSpecialChars( xml, L"SccProvider", m_bstrSccProvider );

	NodeAttributeWithSpecialChars( xml, L"Keyword", m_bstrKeyword );

	return S_OK;
}

STDMETHODIMP CProject::MatchName(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched)
{
	CHECK_POINTER_VALID(pbMatched);
	*pbMatched = VARIANT_FALSE;
	if (bstrNameToMatch == NULL)
	{
		if (m_bstrName == NULL)
			*pbMatched = VARIANT_TRUE;
		else
			*pbMatched = VARIANT_FALSE;
	}
	else if (m_bstrName == NULL)
	{
		*pbMatched = VARIANT_FALSE;
	}
	else if (!bFullOnly && _wcsicmp(m_bstrName,bstrNameToMatch) == 0)
	{
		*pbMatched = VARIANT_TRUE;
	}
	else if (_wcsicmp(m_bstrFileName, bstrNameToMatch) == 0)
	{
		*pbMatched = VARIANT_TRUE;
	}
	else
	{
		CComBSTR bstrName;
		bstrName = m_bstrFullDir;
		bstrName.Append(m_bstrFileName);
		if (bstrName == NULL)
		{
			*pbMatched = VARIANT_FALSE;
		}
		else if (_wcsicmp(bstrName, bstrNameToMatch) == 0)
		{
			*pbMatched = VARIANT_TRUE;
		}
		else
		{
			CPathW pathSrc, pathMatch; // to handle case where shell passes in something
			pathSrc.Create(bstrName);
			pathMatch.Create(bstrNameToMatch);
			if (pathSrc == pathMatch)
			{
				*pbMatched = VARIANT_TRUE;
			}
		}
	}
	return S_OK;
}

STDMETHODIMP CProject::get_Name(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	m_bstrName.CopyTo(pVal);
	return S_OK;
}

STDMETHODIMP CProject::put_Name(BSTR newVal)
{
	// validate name
	// check for all whitespace or 0 length string
	CStringW strName = newVal;
	strName.TrimLeft();
	strName.TrimRight();
	if( strName.IsEmpty() )
		return S_OK;
		
	CHECK_OK_TO_DIRTY(VCPROJID_Name);

	CComBSTR bstrOldName;
	if (!g_bInProjLoad)
		bstrOldName = m_bstrName;
	m_bstrName = newVal;

	// don't update UI during project load!
	if( !g_bInProjLoad)
	{
		if (bstrOldName != m_bstrName)
		{
			long lcItems = m_rgConfigs.GetCount();
			for (long idx = 0; idx < lcItems; idx++)
			{
				CComQIPtr<IVCConfigurationImpl> spConfigImpl = m_rgConfigs.GetAt(idx);
				if (spConfigImpl == NULL)
					continue;
				spConfigImpl->DirtyOutputs(VCPROJID_Name);
			}
		}

		// send an event notifying everyone in the UI business that we've changed
		// appearances
		CComQIPtr<IDispatch> spDisp = this;
		if( spDisp )
		{
			CComQIPtr<IVCProjectEngineImpl> spPrjEngImpl = g_pProjectEngine;
			if( spPrjEngImpl )
				spPrjEngImpl->DoFireItemPropertyChange( spDisp, VCPROJID_Name );
		}
	}
	
	if( !m_bstrFileName )	// do need to give the file name heads up in case of error during load
		put_ProjectFile( newVal );
		
	return S_OK;
}

STDMETHODIMP CProject::get_Keyword(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	m_bstrKeyword.CopyTo(pVal);
	return S_OK;
}

STDMETHODIMP CProject::put_Keyword(BSTR newVal)
{
	CHECK_OK_TO_DIRTY(0);
	m_bstrKeyword = newVal;
	return S_OK;
}

STDMETHODIMP CProject::get_ProjectFile(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	m_bstrFileName.CopyTo( pVal );
	return S_OK;
}

STDMETHODIMP CProject::put_ProjectFile(BSTR newVal)
{
	// validate name
	// check for all whitespace or 0 length string
	CStringW strName = newVal;
	strName.TrimLeft();
	strName.TrimRight();
	if( strName.IsEmpty() )
		return S_OK;

	// Much as we'd like to, we cannot dirty the file here.  Must be done MUCH lower down.
	// Please note that we DO have to SET the new values BEFORE the dirty check since we've
	// got our SCC pre-dirty event set up so that it does not take the project file name as
	// a parameter (this is 'cause the 99.95% case is that the project file name isn't changing).
	// If, for some reason, our dirty check fails later, we saved off the old values so we could
	// revert back to them.

	CComBSTR bstrOldFileName = m_bstrFileName;
	CComBSTR bstrOldFullDir = m_bstrFullDir;
	CPathW pathProj;
	if (pathProj.Create(strName))
	{
		CStringW strExt = pathProj.GetExtension();
		if (strExt.IsEmpty())	// had a bare name for this...
			pathProj.ChangeExtension(L".vcproj");
		m_bstrFileName = pathProj.GetFullPath();
	}
	else
		m_bstrFileName = strName;

	// MUST ALWAYS PASS IN FULL PATH
	CDirW dir;
	dir.CreateFromPath( newVal, FALSE, TRUE );
	const wchar_t* wszDir = (const wchar_t*)dir;
	m_bstrFullDir = wszDir;
	int len = m_bstrFullDir.Length();
	VSASSERT( len > 0, "Invalid project directory name: length of name is <= 0!" );
	if( m_bstrFullDir[len-1] != L'\\' )
		// append the trailing backslash, which the build process wants
		m_bstrFullDir += L"\\";

	HRESULT hr = WrapCanDirty();	// wrap the CHECK_OK_TO_DIRTY call so we can revert changes if needed
	if (hr != S_OK)
	{
		m_bstrFileName = bstrOldFileName;
		m_bstrFullDir = bstrOldFullDir;
		return hr;
	}

	return S_OK;
}

STDMETHODIMP CProject::get_FileFormat( enumFileFormat *pType )
{
	CHECK_POINTER_VALID(pType);
	*pType = m_eFileFormat;
	return S_OK;
}

STDMETHODIMP CProject::put_FileFormat( enumFileFormat type )
{
	CHECK_OK_TO_DIRTY(0);
	CXMLFile::AdjustFileEncodingForFileFormat(m_eFileFormat, type, m_bstrFileEncoding);
	m_eFileFormat = type;
	return S_OK;
}

STDMETHODIMP CProject::get_FileEncoding( BSTR *pEncoding )
{
	CHECK_POINTER_VALID( pEncoding );
	m_bstrFileEncoding.CopyTo(pEncoding);
	return S_OK;
}

STDMETHODIMP CProject::put_FileEncoding( BSTR bstrEncoding )
{
	CHECK_OK_TO_DIRTY(0);
	CXMLFile::AdjustFileFormatForFileEncoding(m_bstrFileEncoding, bstrEncoding, m_eFileFormat);
	m_bstrFileEncoding = bstrEncoding;
	return S_OK;
}

STDMETHODIMP CProject::SaveProjectOptions(IUnknown *pStreamUnk)
{
	//Get the IStream interface
	CComQIPtr<IStream> spStream = pStreamUnk;
	RETURN_ON_NULL2(spStream, E_NOINTERFACE);

	//TODO: Write out additional project properties here

	//Write out count of configs
	int iCount = m_rgConfigs.GetCount();
	ULONG iByteCount;
	HRESULT hr = spStream->Write(&iCount, sizeof(int), &iByteCount);
	RETURN_ON_FAIL(hr);
	
	// get the number of configs to iterate over
	iCount = m_rgConfigs.GetSize();

	for (int i = 0; i < iCount; i++)
	{
		//Get config
		CComQIPtr<IVCConfigurationImpl> spConfigImpl = m_rgConfigs.GetAt(i);
		if (spConfigImpl == NULL) continue;
		
		hr = spConfigImpl->WriteToStream(spStream);
		RETURN_ON_FAIL(hr);
	}

	return S_OK;
}

STDMETHODIMP CProject::LoadProjectOptions(IUnknown *pStreamUnk)
{
	//TODO: Load additional project properties here

	//Get the config count
	int iCfgCount = m_rgConfigs.GetSize();

	//Get the IStream interface
	CComQIPtr<IStream> spStream = pStreamUnk;
	RETURN_ON_NULL2(spStream, E_NOINTERFACE);

	//Read the config count
	int iCount;
	ULONG byteCount;
	HRESULT hr = spStream->Read(&iCount, sizeof(int), &byteCount);
	RETURN_ON_FAIL(hr);

	//For each config stored
	for (int i = 0; i < iCount; i++)
	{
		CComPtr<VCConfiguration>		spConfig;
		CComPtr<IDispatch>				pDispDbgSettings;
		CComQIPtr<IVCDebugSettingsImpl> pDbgSettings;
		CComBSTR						bstrName;
		
		//Read in config name
		hr = bstrName.ReadFromStream(spStream);
		RETURN_ON_FAIL(hr);

		//Loop the configs looking for the one we're loading
		CComQIPtr<IVCConfigurationImpl> spConfigImpl;
		for (int j = 0; j < iCfgCount; j++)
		{
			CComBSTR	bstrCfgName;

			CComPtr<VCConfiguration> spConfig = m_rgConfigs.GetAt(j);
			spConfigImpl = spConfig;
			if (spConfig == NULL || spConfigImpl == NULL)
				continue;
			spConfig->get_Name(&bstrCfgName);
			if (bstrCfgName == bstrName) 
				break;
		}
		if (j == iCfgCount) 
			continue;

		hr = spConfigImpl->ReadFromStream(spStream);
		RETURN_ON_FAIL(hr);
	}

	//TODO: handle reading any additional non-config objects here

	return S_OK;
}

STDMETHODIMP CProject::get_SccProjectName( BSTR* bstrName )
{
	CHECK_POINTER_VALID( bstrName );
	return m_bstrSccProjectName.CopyTo( bstrName );
}

STDMETHODIMP CProject::put_SccProjectName( BSTR bstrName )
{
	// Dirty the project ONLY if we're given new information and we're not in
	// project load
	CHECK_READ_POINTER_VALID( bstrName );
	if( PROJECT_IS_LOADED() && (!m_bstrSccProjectName || _wcsicmp( bstrName, m_bstrSccProjectName ) != 0) )
	{
		CHECK_OK_TO_DIRTY(0)
	}
	m_bstrSccProjectName = bstrName;
	return S_OK;
}

STDMETHODIMP CProject::get_SccAuxPath( BSTR* bstrName )
{
	CHECK_POINTER_VALID( bstrName );
	return m_bstrSccAuxPath.CopyTo( bstrName );
}

STDMETHODIMP CProject::put_SccAuxPath( BSTR bstrName )
{
	if (PROJECT_IS_LOADED() && (!m_bstrSccAuxPath || !bstrName || _wcsicmp(bstrName, m_bstrSccAuxPath) != 0))
	{
		CHECK_OK_TO_DIRTY(0);
	}

	m_bstrSccAuxPath = bstrName;
	return S_OK;
}

STDMETHODIMP CProject::get_SccLocalPath( BSTR* bstrName )
{
	CHECK_POINTER_VALID( bstrName );
	return m_bstrSccLocalPath.CopyTo( bstrName );
}

STDMETHODIMP CProject::put_SccLocalPath( BSTR bstrName )
{
	if (PROJECT_IS_LOADED() && (!m_bstrSccLocalPath || !bstrName || _wcsicmp(bstrName, m_bstrSccLocalPath) != 0))
	{
		CHECK_OK_TO_DIRTY(0);
	}

	m_bstrSccLocalPath = bstrName;
	return S_OK;
}

STDMETHODIMP CProject::get_SccProvider( BSTR* bstrName )
{
	CHECK_POINTER_VALID( bstrName );
	return m_bstrSccProvider.CopyTo( bstrName );
}

STDMETHODIMP CProject::put_SccProvider( BSTR bstrName )
{
	if (PROJECT_IS_LOADED() && (!m_bstrSccProvider || !bstrName || _wcsicmp(bstrName, m_bstrSccProvider) != 0))
	{
		CHECK_OK_TO_DIRTY(0);
	}

	m_bstrSccProvider = bstrName;
	return S_OK;
}

STDMETHODIMP CProject::get_ProjectDirectory(BSTR *pVal)
{
	CHECK_POINTER_VALID( pVal );
	m_bstrFullDir.CopyTo(pVal);

	return S_OK;
}

STDMETHODIMP CProject::get_Configurations(IDispatch **ppVal)
{
	CHECK_POINTER_VALID( ppVal );

	CComPtr<IVCCollection> pCollection;

	HRESULT hr = CCollection<VCConfiguration>::CreateInstance(&pCollection, &m_rgConfigs);
	if (SUCCEEDED(hr))
	{
		hr = pCollection.QueryInterface(ppVal);
	}
	return hr;
}

STDMETHODIMP CProject::get_Platforms(IDispatch **ppVal)
{
	CHECK_POINTER_VALID( ppVal );
		
	CComPtr<IVCCollection> pCollection;

	HRESULT hr = CCollection<VCPlatform>::CreateInstance(&pCollection, &m_rgPlatforms);
	if (SUCCEEDED(hr))
	{
		hr = pCollection.QueryInterface(ppVal);
	}
	return hr;
}

STDMETHODIMP CProject::get_Files(IDispatch **ppVal)
{
	CHECK_POINTER_VALID( ppVal );
		
	CComPtr<IVCCollection> pCollection;
	HRESULT hr = CCollection<VCFile>::CreateInstance(&pCollection, &m_rgFiles);
	if (SUCCEEDED(hr))
	{
		hr = pCollection.QueryInterface(ppVal);
	}
	return hr;
}

STDMETHODIMP CProject::get_Filters(IDispatch **ppVal)
{
	CHECK_POINTER_VALID( ppVal );

	CComPtr<IVCCollection> pCollection;

	HRESULT hr = CCollection<VCFilter>::CreateInstance(&pCollection, &m_rgFilters);
	if (SUCCEEDED(hr))
	{
		hr = pCollection.QueryInterface(ppVal);
	}
	return hr;
}

STDMETHODIMP CProject::get_Items(IDispatch** ppVal)
{
	CHECK_POINTER_VALID(ppVal);

	BuildTopLevelItemsCollection();

	CComPtr<IVCCollection> pCollection;
	HRESULT hr = CCollection<VCProjectItem>::CreateInstance(&pCollection, &m_rgTopLevelItems, this, 1);
	if (SUCCEEDED(hr))
		hr = pCollection.QueryInterface(ppVal);
	return hr;
}

STDMETHODIMP CProject::UpdateItemsCollection(DWORD dwCookie)
{
	if (dwCookie != 0)	// only care about updating a single collection
		BuildTopLevelItemsCollection();

	return S_OK;
}

void CProject::BuildTopLevelItemsCollection()
{
	if (!m_bItemsCollectionDirty)
		return;

	m_rgTopLevelItems.RemoveAll();

	long l;
	long lcItems = m_rgFiles.GetSize();
	// all files get added, regardless of where they are in the hierarchy
	for (l = 0; l < lcItems; l++)
	{
		CComQIPtr<VCProjectItem> spItem = m_rgFiles.GetAt(l);
		if (spItem == NULL)
			continue;

		m_rgTopLevelItems.Add(spItem);
	}

	lcItems = m_rgFilters.GetSize();
	for (l = 0; l < lcItems; l++)
	{
		CComQIPtr<VCProjectItem> spFilter = m_rgFilters.GetAt(l);
		if (spFilter == NULL)
			continue;
	
		m_rgTopLevelItems.Add(spFilter);
	}

	m_bItemsCollectionDirty = false;
}


STDMETHODIMP CProject::AssignActions(VARIANT_BOOL bOnLoad)
{
	m_bAssignedActions = true;

	if (bOnLoad == VARIANT_TRUE)
	{
		IVCBuildableItem *pBuildable = this;
		m_ProjectRegistry.RestoreToProject(pBuildable, TRUE);
	}

	int iMac = m_rgConfigs.GetSize();
	for (int idx = 0; idx <= iMac; idx++)
	{
		CComQIPtr<IVCBuildableItem> spBuildableItem = m_rgConfigs.GetAt(idx);
		if (spBuildableItem == NULL)
			continue;

		spBuildableItem->AssignActions(bOnLoad);
	}

	return S_OK;
}

STDMETHODIMP CProject::UnAssignActions(VARIANT_BOOL bOnClose)
{
	m_bAssignedActions = false;

	if (bOnClose == VARIANT_TRUE)
	{
		IVCBuildableItem *pBuildable = this;
		m_ProjectRegistry.RemoveFromProject(pBuildable, TRUE);
	}

	int iMac = m_rgConfigs.GetSize();
	for (int idx = 0; idx <= iMac; idx++)
	{
		CComQIPtr<IVCBuildableItem> spBuildableItem = m_rgConfigs.GetAt(idx);
		if (spBuildableItem == NULL)
			continue;

		spBuildableItem->UnAssignActions(bOnClose);
	}

	return S_OK;
}

STDMETHODIMP CProject::FileNameValid(/*[in]*/  LPCOLESTR szFileName, VARIANT_BOOL bSetErrorInfo)
{
	CPathW path;
	BOOL b = path.Create( szFileName );
	if( b )
	{
		return S_OK;
	}
	else
	{
		if( IsURLW( szFileName ) )
		{
			return CVCProjectEngine::DoSetErrorInfo2(VCPROJ_E_URL_INVALID, IDS_ERR_FILE_AS_URL, szFileName, (bSetErrorInfo == VARIANT_TRUE));
		}
		return CVCProjectEngine::DoSetErrorInfo2(E_INVALIDARG, IDS_ERR_INVALID_FILE_NAME, szFileName, (bSetErrorInfo == VARIANT_TRUE));
	}
}

STDMETHODIMP CProject::RemoveExistingFile(IDispatch* pItem)
{
	return DoRemoveExistingFile(pItem, FALSE);
}

STDMETHODIMP CProject::RemoveFile(/*[in]*/  IDispatch *pItem)
{
	return DoRemoveExistingFile(pItem, TRUE);
}

HRESULT CProject::DoRemoveExistingFile(IDispatch* pItem, BOOL bRemoveFromFilter)
{
	HRESULT hr = VCPROJ_E_NOT_FOUND;
	long l, lcItems;

	lcItems = m_rgFiles.GetSize();
	for (l = 0; l < lcItems && hr == VCPROJ_E_NOT_FOUND; l++)
	{
		CComPtr<VCFile> pFile;
		pFile = m_rgFiles.GetAt(l);
		if (pFile == NULL)
			continue;

		CComQIPtr<IDispatch> pDispFile = pFile;
		if (pItem != pDispFile)
			continue;

		CHECK_OK_TO_DIRTY(0);
		hr = QueryCanRemoveFile(pFile);
		RETURN_ON_FAIL(hr);

		m_rgFiles.RemoveAtIndex(l); // first remove the file from the project so we can't do this recursively...

		m_bItemsCollectionDirty = true;
		CComPtr<IDispatch> pDispParent;
		HRESULT	hrT = pFile->get_Parent(&pDispParent);

		if (pDispParent)
		{
			// fire events
			CComQIPtr<IVCProjectEngineImpl> pEngine = g_pProjectEngine;
			hr = E_UNEXPECTED;
			if (pEngine)
			{
				pEngine->DoFireItemRemoved(pDispFile, pDispParent);
				hr = S_OK;
			}
		}

		if (bRemoveFromFilter && pDispParent && pDispParent != static_cast<IDispatch *>(this))
		{
			CComQIPtr<VCFilter> pFilter = pDispParent;
			if (pFilter)
			{
#ifdef _DEBUG
				CComBSTR bstrName; pFilter->get_Name(&bstrName); // DEBUG
#endif
				pFilter->RemoveFile(pDispFile);
			}
		}
		// do close here, regardless of owner, so that ItemRemove event got fired properly above
		CComQIPtr<IVCFileImpl> pFileImpl = pFile;
		CComBSTR bstrFilePath;
		pFile->get_FullPath(&bstrFilePath);
		InformFileRemoved(pFile, bstrFilePath);		// needs to be before close 'cause we need parent info
		if( pFileImpl )
			pFileImpl->Close();
		hr = S_OK;
	}

	if (hr == VCPROJ_E_NOT_FOUND)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_NOT_FOUND, IDS_ERR_FILE_NOT_FOUND_FOR_REMOVE, FALSE, NULL, TRUE);
	return hr;
}

STDMETHODIMP CProject::RemoveFilter(/*[in]*/  IDispatch *pItem)
{
	HRESULT hr = VCPROJ_E_NOT_FOUND;
	long l, lcItems;

	lcItems = m_rgFilters.GetSize();
	for (l = 0; l < lcItems && hr == VCPROJ_E_NOT_FOUND; l++)
	{
		CComPtr<VCFilter> pFilter = m_rgFilters.GetAt(l);
		if (pFilter == NULL)
			continue;

		CComQIPtr<IDispatch> pDispFilter = pFilter;
		if (pItem != pDispFilter)
			continue;
		CComQIPtr<IVCFilterImpl> pFilterImpl = pFilter;
		RETURN_ON_NULL2(pFilterImpl, E_UNEXPECTED);

		CHECK_OK_TO_DIRTY(0);

		// fire events
		CComQIPtr<IVCProjectEngineImpl> pEngine;

		pEngine = g_pProjectEngine;
		hr = E_UNEXPECTED;
		if (pEngine)
		{
			CComQIPtr<IDispatch> pDispParent = this;
			if( pDispParent )
				pEngine->DoFireItemRemoved( pDispFilter, pDispParent);
		}

		pFilter->Remove();
		m_rgFilters.RemoveAtIndex(l);
		m_bItemsCollectionDirty = true;
		pFilterImpl->Close();
		hr = S_OK;
	}

	if (hr == VCPROJ_E_NOT_FOUND)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_NOT_FOUND, IDS_ERR_FILTER_NOT_FOUND_FOR_REMOVE);
	return hr;
}

STDMETHODIMP CProject::RemoveConfiguration(IDispatch *pDispConfig)
{
	// get the config interface
	CComQIPtr<VCConfiguration> pConfig = pDispConfig;
	RETURN_INVALID_ON_NULL(pConfig);

	return RemoveConfigurationInternal(pConfig);
}

HRESULT CProject::RemoveConfigurationInternal(VCConfiguration *pConfig)
{
	CHECK_OK_TO_DIRTY(0);

	// remove file configs
	long lcItems = m_rgFiles.GetSize();
	for( long l = 0; l < lcItems; l++ )
	{
		CComPtr<VCFile> pFile;
		pFile = m_rgFiles.GetAt(l);
		CComQIPtr<IVCFileImpl> pFileImpl = pFile;
		if (pFileImpl)
		{
			CComPtr<VCFileConfiguration> pFileConfig;
			pFileImpl->GetFileConfigurationForProjectConfiguration( pConfig, &pFileConfig );
			if( pFileConfig )
			{
				CComQIPtr<IDispatch> pDispFileConfig = pFileConfig;
				if( pDispFileConfig )
					pFileImpl->RemoveConfiguration( pDispFileConfig );
			}
		}
	}

	// remove the config from the collection
	m_rgConfigs.Remove( pConfig );

	// close the config object
	CComQIPtr<IVCConfigurationImpl> pConfigurationImpl = pConfig;
	if (pConfigurationImpl)
		pConfigurationImpl->Close(); // close down entirely; release all

	// fire an event so the ui becomes aware of the missing config
	CComQIPtr<IVCProjectEngineImpl> pEngine = g_pProjectEngine;
	if( pEngine )
	{
		CComQIPtr<IDispatch> pDispConfig = pConfig;
		if( pDispConfig )
			pEngine->DoFireItemRemoved( pDispConfig, static_cast<IDispatch*>(this) );
	}
	return S_OK;
}

STDMETHODIMP CProject::RemovePlatformInternal(IDispatch* pDispPlatform)
{
	CComQIPtr<VCPlatform> spPlatform = pDispPlatform;
	RETURN_ON_NULL2(spPlatform, E_UNEXPECTED);

	CHECK_OK_TO_DIRTY(0);

	CComBSTR bstrPlatformName;
	HRESULT hr = spPlatform->get_Name(&bstrPlatformName);
	VSASSERT(SUCCEEDED(hr) && bstrPlatformName.Length() > 0, "Hey, platforms should always have valid names!");
	if (SUCCEEDED(hr))
	{
		CStringW strPlatformName = bstrPlatformName;
		long lcItems = m_rgConfigs.GetSize();
		for (long l = 0; l < lcItems; l++)
		{
			CComPtr<VCConfiguration> pConfiguration = m_rgConfigs.GetAt(l);
			if (pConfiguration == NULL)
				continue;
			CComBSTR bstrConfigName;
			if (FAILED(pConfiguration->get_Name(&bstrConfigName)))
				continue;
			CStringW strConfigName = bstrConfigName;
			CStringW strConfigPlatName;
			int nBar = strConfigName.Find(L'|');
			if (nBar)
				strConfigPlatName = strConfigName.Right(strConfigName.GetLength()-nBar-1);
			if (strConfigPlatName.IsEmpty())
				continue;
			RemoveConfigurationInternal(pConfiguration);
		}
	}

	// remove the platform from the collection
	m_rgPlatforms.Remove( spPlatform );

	// fire an event so the ui becomes aware of the missing config
	CComQIPtr<IVCProjectEngineImpl> pEngine = g_pProjectEngine;
	if( pEngine )
	{
		CComQIPtr<IDispatch> spDispPlatform = spPlatform;
		if( spDispPlatform )
			pEngine->DoFireItemRemoved( spDispPlatform, static_cast<IDispatch*>(this) );
	}

	return S_OK;
}

STDMETHODIMP CProject::RemovePlatform(IDispatch* pDispPlatform)
{
	if (g_bInProjClose)
		return RemovePlatformInternal(pDispPlatform);

	if (CVCProjectEngine::s_pBuildPackage != NULL)	// let the build package handle letting the solution in on this
		return CVCProjectEngine::s_pBuildPackage->RemovePlatformFromProject(this, pDispPlatform);

	return RemovePlatformInternal(pDispPlatform);
}

STDMETHODIMP CProject::IsFileProbablyInProject( LPCOLESTR szFile, VARIANT_BOOL *pb)
{
	CHECK_POINTER_NULL(pb);
	*pb = VARIANT_FALSE;
	CStringW strPath = szFile;
	if (strPath.IsEmpty())
		return S_OK;

	BldFileRegHandle frh;
	frh = m_ProjectRegistry.LookupFile(strPath, FALSE /* do NOT verify case */);	// requires caller to have obtained actual case
	if( frh )
	{
		frh->ReleaseFRHRef();
		*pb = VARIANT_TRUE;
		return S_OK;
	}

	// Hmmm.  Might not exist.  If it doesn't, force it through a slow check by saying 
	// that it is probably in the project.
	CPathW pathFile;
	if (pathFile.Create(strPath) && !pathFile.ExistsOnDisk())
	{
		*pb = VARIANT_TRUE;
		return S_OK;
	}

	// OK, it exists, but we're not sure if it is ours or not.
	if (_wcsnicmp(strPath, m_bstrFullDir, m_bstrFullDir.Length()) == 0)
		*pb = VARIANT_TRUE;		// if it is in our directory or a subdirectory, it is likely ours

	return S_OK;
}

STDMETHODIMP CProject::get_ItemFileName(BSTR *pVal)
{
	CHECK_POINTER_NULL( pVal );
	return get_Name(pVal);
}

STDMETHODIMP CProject::get_ItemFullPath(BSTR* pbstrFullPath)
{
	CHECK_POINTER_NULL( pbstrFullPath );
	return get_ProjectFile(pbstrFullPath);
}

STDMETHODIMP CProject::get_ActionList(IVCBuildActionList** ppActionList)
{
	return E_UNEXPECTED;	// project doesn't own one of these
}

STDMETHODIMP CProject::get_FileRegHandle(void** pfrh)
{
	CHECK_POINTER_NULL(pfrh);

	if (m_frh == NULL)
	{
		CComBSTR bstrPath;
		get_ProjectFile(&bstrPath);
		CStringW strPath = bstrPath;
		if (!strPath.IsEmpty())
 			m_frh = CBldFileRegFile::GetFileHandle(strPath, TRUE);
	}
	*pfrh = (void *)m_frh;
	return S_OK;
}

STDMETHODIMP CProject::get_ContentList(IEnumVARIANT** ppContents)
{
	CHECK_POINTER_NULL(ppContents);
	*ppContents = NULL;
	
	CComPtr<IDispatch> spDispFiles;
	HRESULT hr = get_Files(&spDispFiles);
	CComQIPtr<IVCCollection> spCollection = spDispFiles;
	RETURN_ON_FAIL_OR_NULL2(hr, spCollection, E_OUTOFMEMORY);

	return spCollection->_NewEnum(reinterpret_cast<IUnknown **>(ppContents));	
}

STDMETHODIMP CProject::get_Registry(void** ppFileRegistry)
{
	CHECK_POINTER_NULL(ppFileRegistry);
	*ppFileRegistry = (void *)&m_ProjectRegistry;
	return S_OK;
}

STDMETHODIMP CProject::get_ProjectConfiguration(VCConfiguration** ppProjCfg)
{
	CHECK_POINTER_NULL(ppProjCfg);
	*ppProjCfg = NULL;
	return E_UNEXPECTED;
}

STDMETHODIMP CProject::get_ProjectInternal(VCProject** ppProject)
{
	CHECK_POINTER_NULL(ppProject);

	*ppProject = this;
	(*ppProject)->AddRef();
	return S_OK;
}

STDMETHODIMP CProject::get_ExistingBuildEngine(IVCBuildEngine** ppBldEngine)
{
	CHECK_POINTER_NULL(ppBldEngine);
	*ppBldEngine = NULL;
	return E_UNEXPECTED;
}

STDMETHODIMP CProject::get_PersistPath(BSTR* pbstrPersistPath)
{
	CHECK_POINTER_NULL( pbstrPersistPath );
	return get_ProjectFile(pbstrPersistPath);
}

STDMETHODIMP CProject::RefreshActionOutputs(long nPropID, IVCBuildErrorContext* pEC)
{
	return S_OK;	// meaningless to a non-config object
}

STDMETHODIMP CProject::HandlePossibleCommandLineOptionsChange(IVCBuildErrorContext* pEC)
{
	return S_FALSE;		// meaningless to a non-config object
}

STDMETHODIMP CProject::ClearDirtyCommandLineOptionsFlag()
{
	return S_FALSE;		// meaningless to a non-config object
}

STDMETHODIMP CProject::AddWebReference(BSTR bstrUrl, IDispatch** ppFile)
{
	return AddWSDL(bstrUrl, NULL, ppFile);
}

class CDownload : 
	public IBindStatusCallback,
	public CComObjectRoot
{
public:
	CDownload() {}
	~CDownload() {}
	static HRESULT CreateInstance(IBindStatusCallback** ppCallback)
	{
		CDownload *pVar;
		CComObject<CDownload> *pObj;
		HRESULT hr = CComObject<CDownload>::CreateInstance(&pObj);
		RETURN_ON_FAIL_OR_NULL(hr, pObj);
		pVar = pObj;
		pVar->AddRef();
		*ppCallback = pVar;
		return hr;
	}

BEGIN_COM_MAP(CDownload)
	COM_INTERFACE_ENTRY(IBindStatusCallback)
END_COM_MAP()

	// IBindStatusCallback
public:
	STDMETHOD(OnStartBinding)(DWORD, IBinding *) { return E_NOTIMPL; }
	STDMETHOD(GetPriority)(LONG *) { return E_NOTIMPL; }
	STDMETHOD(OnLowResource)(DWORD) { return E_NOTIMPL; }
	STDMETHOD(OnProgress)(ULONG, ULONG, ULONG, LPCWSTR) { return E_NOTIMPL; }
	STDMETHOD(OnStopBinding)(HRESULT, LPCWSTR) { return E_NOTIMPL; }
	STDMETHOD(GetBindInfo)(DWORD *grfBINDF, BINDINFO *pbindinfo)
	{
		CHECK_POINTER_NULL(pbindinfo);
	   pbindinfo->dwOptions |= BINDINFO_OPTIONS_USE_IE_ENCODING;
	   return S_OK;
	}
	STDMETHOD(OnDataAvailable)(DWORD, DWORD, FORMATETC *, STGMEDIUM *) { return E_NOTIMPL; }
	STDMETHOD(OnObjectAvailable)(REFIID, IUnknown *) { return E_NOTIMPL; }
};

STDMETHODIMP CProject::UpdateWSDL( BSTR bstrUrlIn, VARIANT_BOOL bExists, BSTR *pbstrSdlPath )
{
	CComBSTR bstrUrl = bstrUrlIn;

	if (bstrUrl.Length() == 0)
		return CVCProjectEngine::DoSetErrorInfo(E_INVALIDARG, IDS_ERR_EMPTY_URL, TRUE /* report */, NULL /* no help */, 
			FALSE /* !only errinfo */);
	CComVariant varUrl = bstrUrl;

	CComPtr<IXMLDOMDocument> pDoc;
	HRESULT hr = CoCreateInstance( CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&pDoc );
	RETURN_ON_FAIL(hr);

	pDoc->put_async( VARIANT_FALSE );
	VARIANT_BOOL bSuccess;
	pDoc->load( varUrl, &bSuccess );
	if (bSuccess == VARIANT_FALSE)
	{
		// hmmm.  Wonder if it is a WSDL file someone forgot to append WSDL to for us...
		bstrUrl += L"?WSDL";
		CComVariant varUrl2 = bstrUrl;
		pDoc->load(varUrl2, &bSuccess);
		if (bSuccess == VARIANT_FALSE)	// nope, that wasn't it.  I give up.
			return CVCProjectEngine::DoSetErrorInfo2(E_INVALIDARG, IDS_ERR_UNSUPPORTED_URL, bstrUrlIn, 
			TRUE, L"VC.AddWebReferenceProblems", FALSE /* !only errinfo */);
	}

	CComBSTR bstr = L"contractRef";
	CComPtr<IXMLDOMNodeList> pResultList;
	hr = pDoc->getElementsByTagName( bstr, &pResultList );
	RETURN_ON_FAIL_OR_NULL(hr, pResultList);

	CStringW strSDLName, strSDLBase;
	CComBSTR bstrSDLURL;
	CComPtr<IXMLDOMNode> pNode;
	hr = pResultList->get_item( 0, &pNode );
	if( hr == S_OK )
	{
		// We have been given a vsdisco, transform into a wsdl
		CComPtr<IXMLDOMNamedNodeMap> pNodeMap;
		hr = pNode->get_attributes(&pNodeMap);
		RETURN_ON_FAIL_OR_NULL(hr, pNodeMap);

		CComPtr<IXMLDOMNode> pRefNode;
		CComBSTR bstrRef = L"ref";
		hr = pNodeMap->getNamedItem(bstrRef, &pRefNode);
		RETURN_ON_FAIL_OR_NULL(hr, pRefNode);

		CComVariant varRef;
		hr = pRefNode->get_nodeValue( &varRef );
		RETURN_ON_FAIL(hr);

		wchar_t szBuffer[4096];
		DWORD dwLen = 4095;
		InternetCombineUrlW( bstrUrl, varRef.bstrVal, szBuffer, &dwLen, NULL );
		bstrSDLURL = szBuffer;

		CStringW strVal;
		strVal = varRef.bstrVal;
		if (_wcsnicmp(strVal, L"http:", 5) == 0)
			strVal = strVal.Right(strVal.GetLength()-5);
		else if (_wcsnicmp(strVal, L"https:", 6) == 0)
			strVal = strVal.Right(strVal.GetLength()-6);
		// else leave it.

		CPathW pathTmp;
		if (!pathTmp.Create(strVal))
			return E_FAIL;

		pathTmp.GetBaseNameString(strSDLBase);
	}
	else
	{
		// We have been given a wsdl leave it alone.
		bstrSDLURL = bstrUrl;

		CComBSTR bstr = L"service";
		CComPtr<IXMLDOMNodeList> pResultList;
		hr = pDoc->getElementsByTagName( bstr, &pResultList );
		RETURN_ON_FAIL_OR_NULL(hr, pResultList);

		CComPtr<IXMLDOMNode> pNode;
		hr = pResultList->get_item( 0, &pNode );
		RETURN_ON_FAIL_OR_NULL(hr, pNode);

		// We have been given a vsdisco, transform into a wsdl
		CComPtr<IXMLDOMNamedNodeMap> pNodeMap;
		hr = pNode->get_attributes(&pNodeMap);
		RETURN_ON_FAIL_OR_NULL(hr, pNodeMap);

		CComPtr<IXMLDOMNode> pRefNode;
		CComBSTR bstrRef = L"name";
		hr = pNodeMap->getNamedItem(bstrRef, &pRefNode);
		RETURN_ON_FAIL_OR_NULL(hr, pRefNode);

		CComVariant varRef;
		hr = pRefNode->get_nodeValue( &varRef );
		RETURN_ON_FAIL(hr);

		strSDLBase = varRef.bstrVal;
	}
	if (bExists)
	{
		strSDLName = strSDLBase;
		strSDLName += L".wsdl";
	}
	else
	{
		CComPtr<IDispatch> pDisp;
		hr = get_Files( &pDisp );
		CComQIPtr<IVCCollection> pColl = pDisp;

		RETURN_ON_FAIL_OR_NULL2(hr, pColl, E_NOINTERFACE);
		CStringW strNewName;
		strNewName = strSDLBase;
		strNewName += L".wsdl";
		CComBSTR bstrName = strNewName;
		CComPtr<IDispatch> spDispFile;
		hr = pColl->Item( CComVariant( bstrName ), &spDispFile );
		if (spDispFile)	// exists, so we need to make a name that doesn't
		{
			int nIdx = 1;
			VARIANT_BOOL bFoundMatch = VARIANT_TRUE;
			while (bFoundMatch)
			{
				strNewName.Format(L"%s%d.wsdl", strSDLBase, nIdx);
				bstrName = strNewName;
				CComPtr<IDispatch> spDispFile2;
				hr = pColl->Item( CComVariant( bstrName ), &spDispFile2 );
				bFoundMatch = (spDispFile2 != NULL);
				nIdx++;
			}
		}
		strSDLName = strNewName;
	}

	CStringW strURL = bstrSDLURL;

	// Now write the file to disk
	CComBSTR bstrProjDir;
	hr = get_ProjectDirectory(&bstrProjDir);
	RETURN_ON_FAIL(hr);

	CDirW dirProj;
	if (!dirProj.CreateFromKnown(bstrProjDir))
		return E_FAIL;

	CPathW pathFile;
	if (!pathFile.CreateFromDirAndFilename(dirProj, strSDLName))
		return E_FAIL;
	CStringW strFullSdlPath;
	pathFile.GetFullPath(strFullSdlPath);
	
	CComPtr<IBindStatusCallback> spCallback;
	hr = CDownload::CreateInstance(&spCallback);
	RETURN_ON_FAIL(hr);
	hr = URLDownloadToFileW(NULL, strURL, strFullSdlPath, 0, spCallback);
	RETURN_ON_FAIL(hr);
	*pbstrSdlPath = SysAllocString(strFullSdlPath);
	return S_OK;
}

STDMETHODIMP CProject::AddWSDL(BSTR bstrUrlIn, VCFilter* pFilter, IDispatch** ppFile)
{

	HRESULT hr = S_OK;
	if (ppFile)
	{
		CHECK_POINTER_VALID(ppFile);
		*ppFile = NULL;
	}
	CComBSTR bstrUrl = bstrUrlIn;
	CComBSTR bstrSdlPath;
	hr = UpdateWSDL( bstrUrlIn, VARIANT_FALSE /* !exists */, &bstrSdlPath);
	RETURN_ON_FAIL(hr);

	// is this file in the project already??
	CComPtr<IDispatch> pDisp;
	hr = get_Files( &pDisp );
	CComQIPtr<IVCCollection> pColl = pDisp;

	RETURN_ON_FAIL_OR_NULL2(hr, pColl, E_NOINTERFACE);
	CComBSTR bstrPath;
	CComPtr<IDispatch> spDispFile;
	hr = pColl->Item( CComVariant( bstrSdlPath ), &spDispFile );

	// if the file isn't in the project, we need to add it
	// which means we need to check with SCC first
	if( hr != S_OK || !spDispFile )
	{
		spDispFile = NULL;
		if (pFilter)
			hr = pFilter->AddFile(bstrSdlPath, &spDispFile);

		else
			hr = AddFile(bstrSdlPath, &spDispFile);

		if (hr == VCPROJ_E_FILE_EXISTS)		// don't care if the file is already there
			hr = S_OK;
	}

	RETURN_ON_FAIL(hr);

	// now, we need to persist that URL.  Since we store that on the tool, we need to store it for all configs just to be safe.
	CComQIPtr<VCFile> spFile = spDispFile;
	if (spFile)
	{
		CComPtr<IDispatch> spDispCfgs;
		spFile->get_FileConfigurations(&spDispCfgs);
		CComQIPtr<IVCCollection> spColl = spDispCfgs;
		if (spColl)
		{
			CComPtr<IEnumVARIANT> spCfgs;
			if (SUCCEEDED(spColl->_NewEnum(reinterpret_cast<IUnknown **>(&spCfgs))) && spCfgs)
			{
				spCfgs->Reset();
				while (TRUE)
				{
					CComVariant varRes;
					HRESULT hrT = spCfgs->Next(1, &varRes, NULL);
					if (hrT == S_OK && (varRes.vt == VT_DISPATCH || varRes.vt == VT_UNKNOWN))
					{
						CComQIPtr<VCFileConfiguration> spFileCfg = varRes.pdispVal;
						if (spFileCfg)
						{
							CComPtr<IDispatch> spDispTool;
							hrT = spFileCfg->get_Tool(&spDispTool);
							if (SUCCEEDED(hrT))
							{
								CComQIPtr<VCWebServiceProxyGeneratorTool> spTool = spDispTool;
								if (spTool)
									spTool->put_URL(bstrUrl);
							}
						}
					}
					else
						break;
				}
			}
		}
	}

	// now, make sure we've got a unique output name for the file
	MakeOutputFileUnique(spFile);

	if (ppFile)
		*ppFile = spDispFile.Detach();
	return S_OK;
}

STDMETHODIMP CProject::MakeOutputFileUnique(VCFile* pFile)
{
	CComQIPtr<IVCFileImpl> spFileImpl = pFile;
	RETURN_ON_NULL2(spFileImpl, S_OK);

	long lcItems = m_rgConfigs.GetSize();
	for (long idx = 0; idx < lcItems; idx++)
	{
		CComQIPtr<IVCConfigurationImpl> spCfgImpl = m_rgConfigs.GetAt(idx);
		CComQIPtr<VCConfiguration> spCfg = spCfgImpl;
		if (spCfg == NULL || spCfgImpl == NULL)
			continue;

		VARIANT_BOOL bUnique = VARIANT_TRUE;
		long nSuffix = 1;
		CComPtr<VCFileConfiguration> spFileCfg;
		HRESULT hr = spFileImpl->GetFileConfigurationForProjectConfiguration(spCfg, &spFileCfg);
		if (FAILED(hr))
			continue;
		CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = spFileCfg;
		if (spFileCfgImpl == NULL)
			continue;
		if ((spCfgImpl->CreatesUniqueOutputName(spFileCfgImpl, &bUnique) != S_OK || bUnique != VARIANT_TRUE))
		{
			CComBSTR bstrDefFormat;
			spFileCfgImpl->get_DefaultOutputFileFormat(&bstrDefFormat);
			CStringW strDefFormat = bstrDefFormat;
			bUnique = VARIANT_FALSE;
			while (!bUnique)
			{
				CStringW strNewOutName;
				strNewOutName.Format(strDefFormat, nSuffix);
				CComBSTR bstrNewOutName = strNewOutName;
				spFileCfgImpl->put_OutputFile(bstrNewOutName);

				nSuffix++;
				if ((spCfgImpl->CreatesUniqueOutputName(spFileCfgImpl, &bUnique) != S_OK || bUnique != VARIANT_TRUE))
					bUnique = VARIANT_FALSE;
			}
		}
	}

	return S_OK;
}

STDMETHODIMP CProject::get_Project(IDispatch * *Val)
{
	CHECK_POINTER_VALID(Val);
	CComQIPtr<IDispatch> spDisp = this;
	*Val = spDisp.Detach();
	return S_OK;
}

STDMETHODIMP CProject::get_Parent(IDispatch * *Val)
{
	CHECK_POINTER_VALID(Val);
	Val = NULL;
	return S_FALSE;
}

STDMETHODIMP CProject::get_Kind(BSTR* kind)
{
	CHECK_POINTER_VALID(kind);
	CComBSTR bstrKind = L"VCProject";
	*kind = bstrKind.Detach();
	return S_OK;
}

/* static */
BOOL CProject::ForwardTrackEvents(VCFilter* pFilterParent)
{
	if (!PROJECT_IS_LOADED())
		return FALSE;

	RETURN_ON_NULL2(CVCProjectEngine::s_pBuildPackage, FALSE);
	RETURN_ON_NULL2(pFilterParent, TRUE);

	VARIANT_BOOL bScc = VARIANT_TRUE;
	if (SUCCEEDED(pFilterParent->get_SourceControlFiles(&bScc)) && bScc == VARIANT_FALSE)
		return FALSE;

	return TRUE;
}

/* static */
HRESULT CProject::QueryCanAddFile(VCProject* pProj, VCFilter* pFilter, LPCOLESTR szFile, BOOL bSetErrorInfo)
{
	if (!ForwardTrackEvents(NULL))	// call ForwardTrackEvents here ONLY for quick & dirty checks
		return S_OK;
	// once the project is loaded, all files must make this check. files in folders marked 'not for scc'
	// will be excluded from SCC in the ItemAdded event handler
	return CVCProjectEngine::s_pBuildPackage->AllowedToAddFile(pProj, szFile, bSetErrorInfo ? VARIANT_TRUE : VARIANT_FALSE);
}

BOOL CProject::DetermineFileParentFilter(VCFile* pFile, CComQIPtr<VCFilter>& rspFilterParent)
{
	RETURN_ON_NULL2(pFile, FALSE);

	CComPtr<IDispatch> spDispParent;
	pFile->get_Parent(&spDispParent);
	rspFilterParent = spDispParent;

	return (rspFilterParent != NULL);
}

HRESULT CProject::QueryCanRemoveFile(VCFile* pFile)
{
	// warning: if you remove this ForwardTrackEvents, be sure to check CVCProjectEngine::s_pBuildPackage for NULL...
	if (!ForwardTrackEvents(NULL))	// check it once for cheap and dirty check
		return S_OK;

	CComQIPtr<VCFilter> spFilterParent;
	DetermineFileParentFilter(pFile, spFilterParent);
	// warning: if you remove this ForwardTrackEvents, be sure to check CVCProjectEngine::s_pBuildPackage for NULL...
	if (spFilterParent && !ForwardTrackEvents(spFilterParent))	// check it again with the parent resolved
		return S_OK;

	VSASSERT(pFile, "Hey, how did we get here with a NULL pFile?");
	if (pFile == NULL)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);
	
	CComBSTR bstrPath;
	HRESULT hr = pFile->get_FullPath(&bstrPath);
	RETURN_ON_FAIL(hr);

	return CVCProjectEngine::s_pBuildPackage->AllowedToRemoveFile(this, bstrPath);
}

HRESULT CProject::InformFileAdded(VCProject* pProject, VCFile* pFile, BSTR bstrPath)
{
	// warning: if you remove this ForwardTrackEvents, be sure to check CVCProjectEngine::s_pBuildPackage for NULL...
	if (!ForwardTrackEvents(NULL))	// check it once for cheap and dirty check
		return S_OK;

	CComQIPtr<VCFilter> spFilterParent;
	DetermineFileParentFilter(pFile, spFilterParent);
	// warning: if you remove this ForwardTrackEvents, be sure to check CVCProjectEngine::s_pBuildPackage for NULL...
	if (spFilterParent && !ForwardTrackEvents(spFilterParent))	// check it again with the parent resolved
		return S_OK;

	// need to pass the full path through
	CDirW dir;
	CComBSTR bstrProjDir;
	pProject->get_ProjectDirectory( &bstrProjDir );
	dir.CreateFromString( bstrProjDir );
	CPathW path;
	path.CreateFromDirAndFilename( dir, bstrPath );
	CComBSTR bstrFullPath = path.GetFullPath();

	return CVCProjectEngine::s_pBuildPackage->InformFileAdded(pProject, bstrFullPath);
}

HRESULT CProject::InformFileRemoved(VCFile* pFile, BSTR bstrPath)
{
	// warning: if you remove this ForwardTrackEvents, be sure to check CVCProjectEngine::s_pBuildPackage for NULL...
	if (!ForwardTrackEvents(NULL))	// check it once for cheap and dirty check
		return S_OK;

	CComQIPtr<VCFilter> spFilterParent;
	DetermineFileParentFilter(pFile, spFilterParent);
	// warning: if you remove this ForwardTrackEvents, be sure to check CVCProjectEngine::s_pBuildPackage for NULL...
	if (spFilterParent && !ForwardTrackEvents(spFilterParent))	// check it again with the parent resolved
		return S_OK;

	return CVCProjectEngine::s_pBuildPackage->InformFileRemoved(this, bstrPath);
}

STDMETHODIMP CProject::AddWebReferenceEx(LPCOLESTR szUrl)
{
    CComPtr<IDispatch> pDisp;
    CComBSTR bstrUrl = szUrl;
    return AddWebReference(bstrUrl, &pDisp);
}


STDMETHODIMP CProject::IsPropertyReadOnly(DISPID dispid, BOOL *fReadOnly)
{ 
	*fReadOnly = TRUE;
	return S_OK; 
}

STDMETHODIMP CProject::GetLocalizedPropertyInfo(DISPID dispid, LCID localeID, BSTR *pbstrName, BSTR *pbstrDesc)
{
	CComQIPtr<IDispatch> pdisp;
	QueryInterface(__uuidof(IDispatch), (void**)&pdisp);
	if (pdisp == NULL)
		return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	CComPtr<ITypeInfo> pTypeInfo;
	HRESULT hr = pdisp->GetTypeInfo(0, LANG_NEUTRAL, &pTypeInfo);
	CComQIPtr<ITypeInfo2> pTypeInfo2 = pTypeInfo;
	RETURN_ON_FAIL(hr);
	if (pTypeInfo2 == NULL)
		return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	CComBSTR bstrDoc;
	hr = pTypeInfo2->GetDocumentation2(dispid, localeID, &bstrDoc, NULL, NULL);
	RETURN_ON_FAIL(hr);
	
	if (bstrDoc.m_str == NULL)
		return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	LPOLESTR pDesc = wcsstr( bstrDoc, L": " );
	if (pDesc == NULL)
		return DoSetErrorInfo(VCPROJ_E_INTERNAL_ERR, IDS_ERR_INTERNAL_ERROR);

	int nSize = (int)(pDesc - bstrDoc);
	pDesc+=2;

	CComBSTR bstrName( nSize, bstrDoc );
	if (pbstrName != NULL)
		*pbstrName = bstrName.Detach();

	CComBSTR bstrDesc( pDesc );
	if (pbstrDesc != NULL)
		*pbstrDesc = bstrDesc.Detach();
	
	return S_OK;
}

STDMETHODIMP CProject::get_VCProjectEngine(IDispatch** ppProjEngine)
{
	CHECK_POINTER_VALID(ppProjEngine);
	RETURN_ON_NULL(g_pProjectEngine);
	return g_pProjectEngine->QueryInterface(IID_IDispatch, (void **)ppProjEngine);
}
