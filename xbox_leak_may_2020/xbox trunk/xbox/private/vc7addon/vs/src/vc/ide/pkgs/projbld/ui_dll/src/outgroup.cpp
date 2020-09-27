// OutGroup.cpp: implementation of the COutputGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vssolutn.h"
#include "OutGroup.h"
#include "gencfg.h"

#ifdef UNICODE
#undef _TCHAR
#define  _TCHAR char
#endif
#include <mrengine.h>
#include <utf.h>

LPCOLESTR s_SystemFiles[] =
{
	L"KERNEL32.DLL",
	L"GDI32.DLL",
	L"SHELL32.DLL",
	L"OLE32.DLL",
	L"USER32.DLL",
	L"ADVAPI32.DLL",
	L"WINMM.DLL",
	L"WINSPOOL.DRV",
	L"MSCOREE.DLL",
	L"IMAGEHLP.DLL",
	L"SHLWAPI.DLL",
	L"WS2_32.DLL",
	L"DBGHELP.DLL",
	L"USERENV.DLL",
	L"OLEAUT32.DLL",
	L"COMCTL32.DLL",
	NULL
};

HRESULT InitMRE();
HRESULT GetImportDependencies(BSTR bstrIdbFile, CVCStringWList *pList);
HRESULT GetLibDependencies( VCConfiguration *pConfig, BSTR bstrPrimaryOutput, CVCStringWList *pList );
BOOL MRECALL FEnumDepFileForDeployment(PMREUtil pmre, EnumFile& ef, EnumType et);
BOOL MRECALL FEnumSrcFileForDeployment(PMREUtil pMre, EnumFile& ef, EnumType et);

HRESULT COutputGroup::CreateInstance(IVsOutputGroup **ppOutGroup, IVsProjectCfg2* pParent, int groupType)
{
	CComObject<COutputGroup> *pOutGroup = NULL;
	HRESULT hr = CComObject<COutputGroup>::CreateInstance(&pOutGroup);
	if( SUCCEEDED(hr) )
	{
		hr = pOutGroup->QueryInterface(__uuidof(IVsOutputGroup),(void**)ppOutGroup);
		pOutGroup->Initialize(pParent, groupType);
	}
	return hr;
}


void COutputGroup::Initialize(IVsProjectCfg2* pParent, int groupType)
{
	m_pCfg = pParent;
	m_groupType = groupType;
}

STDMETHODIMP COutputGroup::get_CanonicalName(BSTR *pbstrCanonicalName)
{
	CHECK_POINTER_NULL(pbstrCanonicalName)

	CComBSTR bstrGroupName;
	switch( m_groupType )
	{
		case PRIMARY_OUTPUT:
			bstrGroupName = L"Built";
			break;
		case SECONDARY_OUTPUT:
			bstrGroupName = L"Symbols";
			break;
		case CONTENT_OUTPUT:
			bstrGroupName = L"ContentFiles";
			break;
		case SOURCE_OUTPUT:
			bstrGroupName = L"SourceFiles";
			break;
		default:
			return E_FAIL;
	}
	*pbstrCanonicalName = bstrGroupName.Detach();
	return S_OK;
}

STDMETHODIMP COutputGroup::get_DisplayName(BSTR *pbstrDisplayName)
{
	CHECK_POINTER_NULL(pbstrDisplayName)
	
	CComBSTR bstrGroupName;
	switch( m_groupType )
	{
		case PRIMARY_OUTPUT:
			bstrGroupName.LoadString(IDS_BUILTGROUP);
			break;
		case SECONDARY_OUTPUT:
			bstrGroupName.LoadString(IDS_DEBUGGROUP);
			break;
		case CONTENT_OUTPUT:
			bstrGroupName.LoadString(IDS_CONTENTGROUP);
			break;
		case SOURCE_OUTPUT:
			bstrGroupName.LoadString(IDS_SOURCEGROUP);
			break;
		default:
			return E_FAIL;
	}
	*pbstrDisplayName = bstrGroupName.Detach();
	return S_OK;
}

// MattGe 8/12/99:  New method
STDMETHODIMP COutputGroup::get_KeyOutput(BSTR *pbstrKeyOutput)
{
	CHECK_POINTER_NULL(pbstrKeyOutput);
	*pbstrKeyOutput = NULL;

	CComPtr<VCConfiguration> pConfig;
	HRESULT hr = GetVCConfiguration(&pConfig);
	RETURN_ON_FAIL_OR_NULL(hr, pConfig);

	CComBSTR bstrName;
	switch( m_groupType )
	{
		case PRIMARY_OUTPUT:
			hr = pConfig->get_PrimaryOutput(&bstrName);
			break;
		case SECONDARY_OUTPUT:
			break;
		case CONTENT_OUTPUT:
			break;
		case SOURCE_OUTPUT:
			break;
	}
	*pbstrKeyOutput = bstrName.Detach();
	return hr;
}


STDMETHODIMP COutputGroup::get_ProjectCfg(IVsProjectCfg2 **ppIVsProjectCfg2)
{
	CHECK_POINTER_NULL(ppIVsProjectCfg2);
	return m_pCfg.CopyTo(ppIVsProjectCfg2);
}

STDMETHODIMP COutputGroup::get_Outputs(ULONG celt,IVsOutput2* rgpcfg[], ULONG *pcActual)
{
	HRESULT hr = S_OK;
	CComPtr<VCConfiguration> pConfig;
	GetVCConfiguration(&pConfig);

	if( celt == 0 )
	{
		CHECK_POINTER_NULL(pcActual);
		switch( m_groupType )
		{
			case PRIMARY_OUTPUT:
			{
				CComQIPtr<IVCConfigurationImpl> spConfigImpl = pConfig;
				long cOutputs = 0;
				if (spConfigImpl)
					spConfigImpl->get_DeployableOutputsCount(&cOutputs);
				else
				{
					CComBSTR bstrPrimaryOutput;
					if (SUCCEEDED(pConfig->get_PrimaryOutput(&bstrPrimaryOutput)) && bstrPrimaryOutput && 
						bstrPrimaryOutput[0] != L'\0')
						cOutputs = 1;
				}
				*pcActual = cOutputs;
				break;
			}
			case SECONDARY_OUTPUT:
				*pcActual = 1;
				break;
			case CONTENT_OUTPUT:
			{
				CComPtr<IEnumVARIANT> pEnum;
				hr = GetVCFileEnumerator(&pEnum);
				RETURN_ON_FAIL_OR_NULL(hr, pEnum);

				unsigned long i = 0;
				while( true )
				{
					CComVariant var;
					hr = pEnum->Next( 1, &var, NULL );
					if( hr != S_OK )
						break;
					CComQIPtr<VCFile> pFile = var.punkVal;
					if (pFile == NULL)
						continue;

					VARIANT_BOOL bContent = VARIANT_FALSE;
					hr = pFile->get_DeploymentContent( &bContent );
					if( bContent == VARIANT_TRUE )
						i++;
				}
				*pcActual = i;
				break;
			}
			case SOURCE_OUTPUT:
			{
				CComPtr<IEnumVARIANT> pEnum;
				hr = GetVCFileEnumerator(&pEnum);
				RETURN_ON_FAIL_OR_NULL(hr, pEnum);

				// REVIEW(kperry) get_Count would be more efficient.
				unsigned long i = 1; // start at 1 to account for the project file.
				while( true )
				{
					CComVariant var;
					hr = pEnum->Next( 1, &var, NULL );
					if( hr != S_OK )
						break;
					i++;
				}
				*pcActual = i;
				break;
			}
		}
		return S_OK;
	}

	CHECK_POINTER_NULL(rgpcfg);

	// calculate the outputs here
	CComBSTR bstrName;
	CComBSTR bstrPrimaryOutput;
	switch( m_groupType )
	{
		case PRIMARY_OUTPUT:
		{
			RETURN_ON_NULL(pConfig);
			long idxActual = 0;
			CComQIPtr<IVCConfigurationImpl> spConfigImpl = pConfig;
			if (spConfigImpl)
			{
				CComPtr<IVCBuildStringCollection> spDeployableOutputs;
				spConfigImpl->get_DeployableOutputs(&spDeployableOutputs);
				if (spDeployableOutputs)
				{
					spDeployableOutputs->Reset();
					while (TRUE)
					{
						CComBSTR bstrOutput;
						HRESULT hrT = spDeployableOutputs->Next(&bstrOutput);
						if (hrT != S_OK)
							break;
						else if (bstrOutput == NULL)
							continue;

						CPathW path;
						path.Create(bstrOutput);
						CComBSTR bstrRelativePath = path.GetFileName();
						COutput2::CreateInstance(&rgpcfg[idxActual], m_pCfg, bstrOutput, bstrRelativePath);
						idxActual++;
					}
				}
			}
			if (idxActual == 0)
			{
				hr = pConfig->get_PrimaryOutput(&bstrPrimaryOutput);
				if (SUCCEEDED(hr) && bstrPrimaryOutput && bstrPrimaryOutput[0] != L'\0')
				{
					CPathW path;
					path.Create(bstrPrimaryOutput);
					CComBSTR bstrRelativePath = path.GetFileName();
					COutput2::CreateInstance(&rgpcfg[0], m_pCfg, bstrPrimaryOutput, bstrRelativePath);
					idxActual = 1;
				}
			}
			if( pcActual )
				*pcActual = idxActual;
			break;
		}
		case SECONDARY_OUTPUT:
		{
			CComBSTR bstrPDB;
			hr = pConfig->get_ProgramDatabase(&bstrPDB);
			if (hr == S_OK && bstrPDB && bstrPDB[0] != L'\0')
			{
				CPathW path;
				path.Create(bstrPDB);
				CComBSTR bstrRelativePath = path.GetFileName();

				COutput2::CreateInstance(&rgpcfg[0], m_pCfg, bstrPDB, bstrRelativePath);
				if( pcActual )
					*pcActual = 1;
			}
			else
			{
				if (pcActual)
					*pcActual = 0;
			}
			break;
		}
		case CONTENT_OUTPUT:
		case SOURCE_OUTPUT:
		{
			CComPtr<IEnumVARIANT> pEnum;
			hr = GetVCFileEnumerator(&pEnum);
			RETURN_ON_FAIL_OR_NULL(hr, pEnum);

			CComBSTR bstrDir;
			CComPtr<IDispatch> pDispProject;
			pConfig->get_Project( &pDispProject );
			CComQIPtr<VCProject> pProject = pDispProject;
			pProject->get_ProjectDirectory(&bstrDir);

			unsigned long i = 0;
			while( true )
			{
				CComVariant var;
				hr = pEnum->Next( 1, &var, NULL );
				if( hr != S_OK )
					break;
				CComQIPtr<VCFile> pFile = var.punkVal;
				if (pFile == NULL)
					continue;

				if( m_groupType  == CONTENT_OUTPUT )
				{
				    VARIANT_BOOL bContent = VARIANT_FALSE;
				    hr = pFile->get_DeploymentContent( &bContent );
				    if( bContent == VARIANT_FALSE )
					continue;
				}

				// create an output instance for this file
				CComBSTR bstrFilename;
				hr = pFile->get_FullPath( &bstrFilename );
				if (FAILED(hr) || bstrFilename == NULL)
					continue;

				CComBSTR bstrRelativePath;
				int rDirLen = bstrDir.Length();
				if (!_wcsnicmp(bstrDir, bstrFilename, rDirLen))
				{
					bstrRelativePath = &bstrFilename[rDirLen];
				}
				else
				{
					CPathW path;
					path.Create(bstrFilename);
					bstrRelativePath = path.GetFileName();
				}

				COutput2::CreateInstance(&rgpcfg[i], m_pCfg, bstrFilename, bstrRelativePath );
				i++;
			}

			if( m_groupType  == SOURCE_OUTPUT )
			{
				CComBSTR bstrFilename;
				pProject->get_ProjectFile( &bstrFilename );
				CPathW path;
				path.Create(bstrFilename);
				CComBSTR bstrRelativePath;
				bstrRelativePath = path.GetFileName();
				COutput2::CreateInstance(&rgpcfg[i], m_pCfg, bstrFilename, bstrRelativePath );
				i++;
			}
			
			if( pcActual )
				*pcActual = i;
			break;

		}
		default:
			return E_FAIL;
	}
	return S_OK;
}

BOOL COutputGroup::IsManaged(VCConfiguration* pConfig)
{
	ConfigurationTypes configType = typeUnknown;
	if (FAILED(pConfig->get_ConfigurationType(&configType)))
		return FALSE;
	else if (configType != typeApplication && configType != typeDynamicLibrary)
		return FALSE;

	CComQIPtr<IVCPropertyContainer> spPropContainer = pConfig;
	RETURN_ON_NULL2(spPropContainer, FALSE);

	compileAsManagedOptions managedExt = managedNotSet;
	if (spPropContainer->GetIntProperty(VCCLID_CompileAsManaged, (long *)&managedExt) != S_OK)
		return FALSE;

	return (managedExt != managedNotSet);
}

HRESULT COutputGroup::GetVCProject(VCProject** ppProject)
{
	*ppProject = NULL;

	CComPtr<VCConfiguration> pConfig;
	HRESULT hr = GetVCConfiguration(&pConfig);
	RETURN_ON_NULL(pConfig);

	CComPtr<IDispatch> pDispProject;
	pConfig->get_Project( &pDispProject );
	CComQIPtr<VCProject> pProject = pDispProject;
	RETURN_ON_NULL(pProject);

	*ppProject = pProject.Detach();
	return S_OK;
}

HRESULT COutputGroup::GetVCFileEnumerator(IEnumVARIANT** ppEnum, VCProject* pProj /* = NULL */)
{
	*ppEnum = NULL;

	// Get a VCProject from an IVsCfg project
	CComPtr<VCProject> pProject;
	HRESULT hr = S_OK;
	
	if (pProj)
		pProject = pProj;
	else
	{
		GetVCProject(&pProject);
		RETURN_ON_FAIL_OR_NULL(hr, pProject);
	}

	// Get the file collection
	CComPtr<IDispatch> pDispColl;
	hr = pProject->get_Files( &pDispColl );
	CComQIPtr<IVCCollection> spColl = pDispColl;
	RETURN_ON_FAIL_OR_NULL(hr, spColl);

	CComPtr<IEnumVARIANT> pEnum;
	hr = spColl->_NewEnum(reinterpret_cast<IUnknown **>(&pEnum));
	RETURN_ON_FAIL_OR_NULL(hr, pEnum);

	pEnum->Reset();
	*ppEnum = pEnum.Detach();
	return S_OK;
}

STDMETHODIMP COutputGroup::get_DeployDependencies(ULONG celt, IVsDeployDependency* rgpdpd[], ULONG *pcActual)
{
	if( pcActual )
		*pcActual = 0;

	if( celt > 0 )
	{
		CHECK_POINTER_NULL(rgpdpd);
		rgpdpd[0] = NULL;
	}

	if( m_groupType != PRIMARY_OUTPUT)
		return S_OK;


	CComBSTR bstrPrimaryOutput;
	CComPtr<VCConfiguration> pConfig;
	GetVCConfiguration(&pConfig);
	RETURN_ON_NULL(pConfig);
	HRESULT hr = pConfig->get_PrimaryOutput(&bstrPrimaryOutput);
	RETURN_ON_FAIL(hr);
	if (bstrPrimaryOutput == NULL || bstrPrimaryOutput[0] == L'\0')	// empty, so no deployment dependencies here...
		return S_OK;

	FILETIME newTime;
	WIN32_FIND_DATAW fd;
	HANDLE hFind = FindFirstFileW(bstrPrimaryOutput, &fd);
	::FindClose(hFind);
	newTime = fd.ftLastWriteTime;
   	if( (m_oldTime.dwHighDateTime != newTime.dwHighDateTime) || (m_oldTime.dwLowDateTime != newTime.dwLowDateTime) )
	{
		m_oldTime = newTime;

		// new to recalculate everything
	    m_strList.RemoveAll();
	    CComQIPtr<IVCConfigurationImpl> spConfigImpl = pConfig;
	    if (spConfigImpl)
	    {
			CComBSTR bstrIdbFile;
			spConfigImpl->get_MrePath(&bstrIdbFile);
			hr = m_IDB.GetImportDependencies( bstrIdbFile, &m_strList);
	    }
	    hr = m_IDB.GetLibDependencies(pConfig, bstrPrimaryOutput, &m_strList);
	}

	ULONG nCount = 0;
	if (celt > 0)
	{
	    // For each file in
		VCPOSITION pos = m_strList.GetHeadPosition();
		while( pos && ((ULONG)nCount < celt) )
		{
			CComBSTR bstrDep = m_strList.GetNext(pos);
			CDeployDependency::CreateInstance(&(rgpdpd[nCount]), bstrDep);
		    nCount++;
		}
	}
	else
	{
		nCount = (ULONG)m_strList.GetCount();
	}

	if( pcActual )
		*pcActual = nCount;

	return S_OK;
}

STDMETHODIMP COutputGroup::get_Description(BSTR *pbstrDescription)
{
	CComBSTR bstrDesc;

	switch( m_groupType )
	{
	case PRIMARY_OUTPUT:
		bstrDesc.LoadString( IDS_PRIMARY_OUTPUT );
		break;
	case SECONDARY_OUTPUT:
		bstrDesc.LoadString( IDS_SECONDARY_OUTPUT );
		break;
	case CONTENT_OUTPUT:
		bstrDesc.LoadString( IDS_CONTENT_OUTPUT );
		break;
	case SOURCE_OUTPUT:
		bstrDesc.LoadString( IDS_SOURCE_OUTPUT );
		break;
	default:
		RETURN_INVALID();
	}
	*pbstrDescription = bstrDesc.Detach();
	return S_OK;
}


HRESULT CDeployDependency::CreateInstance(IVsDeployDependency** ppDep, BSTR bstrURL)
{
	CComObject<CDeployDependency> *pDep = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CDeployDependency>::CreateInstance(&pDep);
	if (SUCCEEDED(hr))
	{
		pDep->QueryInterface(__uuidof(IVsDeployDependency), (void**)ppDep);
		pDep->Initialize(bstrURL);
	}

	return hr;
}


void CDeployDependency::Initialize(BSTR bstrURL)
{
	// this is a URL, so add the file:// to the front of it.
	m_depName += bstrURL;
}

STDMETHODIMP CDeployDependency::get_DeployDependencyURL(BSTR *pbstrURL)
{
	CHECK_POINTER_NULL(pbstrURL);
	return m_depName.CopyTo(pbstrURL);
}


// IVsOutput2 implementation

void COutput2::Initialize(IVsProjectCfg2* pCfg, BSTR bstrPath, BSTR bstrRelativePath )
{
	m_pCfg = pCfg;
	m_bstrFullPath = bstrPath;
	m_bstrRelativePath = bstrRelativePath;
}

HRESULT COutput2::CreateInstance(IVsOutput2 **ppOutput, IVsProjectCfg2* pCfg, BSTR bstrPath, BSTR bstrRelativePath)
{
	CComObject<COutput2> *pOutput = NULL;
	HRESULT hr = CComObject<COutput2>::CreateInstance(&pOutput);
	if (SUCCEEDED(hr))
	{
		pOutput->Initialize(pCfg, bstrPath, bstrRelativePath);
		hr = pOutput->QueryInterface( __uuidof(IVsOutput2), (void**)ppOutput );
	}
	return hr;
}

HRESULT COutput2::get_DisplayName(BSTR *pbstrDisplayName)
{
	CHECK_POINTER_NULL(pbstrDisplayName)

	// well, here we just send the file name
	return m_bstrRelativePath.CopyTo(pbstrDisplayName);
}

HRESULT COutput2::get_CanonicalName(BSTR *pbstrCanonicalName)
{
	CHECK_POINTER_NULL(pbstrCanonicalName)
	return m_bstrFullPath.CopyTo(pbstrCanonicalName);
}

// The DeploySourceURL is the web location of the item.  For items in the
// local filesystem, the URL should begin with the eight characters:
// "file:///".  Consumers of outputs may not be able to process URLs of
// other forms, so it's very important for projects that are generating URLs
// for local items to try to use this form of URL as much as possible.
HRESULT COutput2::get_DeploySourceURL(BSTR *pbstrDeploySourceURL)
{
	CHECK_POINTER_NULL(pbstrDeploySourceURL)

	CStringW out = L"file:///";
	out += m_bstrFullPath;
	*pbstrDeploySourceURL = SysAllocString((LPCOLESTR)out);
	return S_OK;
}

HRESULT COutput2::get_Type(GUID *pguidType)
{
	CHECK_POINTER_NULL(pguidType);
	return E_NOTIMPL;	// COutput2::get_Type
}

STDMETHODIMP COutput2::get_RootRelativeURL(BSTR *pbstrRelativePath)
{
	CHECK_POINTER_NULL(pbstrRelativePath)
	
	return m_bstrRelativePath.CopyTo(pbstrRelativePath);
}

HRESULT COutput2::GetVCConfiguration(VCConfiguration** ppConfig)
{
	CComQIPtr<IVCCfg> pCfg = m_pCfg;
	CHECK_ZOMBIE(pCfg, IDS_ERR_OUTPUT_ZOMBIE);
	CComPtr<IDispatch> spDispCfg;
	pCfg->get_Object(&spDispCfg);
	CComQIPtr<VCConfiguration> spConfig = spDispCfg;
	CHECK_ZOMBIE(spConfig, IDS_ERR_OUTPUT_ZOMBIE);

	*ppConfig = spConfig.Detach();
	return S_OK;
}

HRESULT COutputGroup::GetVCConfiguration(VCConfiguration** ppConfig)
{
	CComQIPtr<IVCCfg> pCfg = m_pCfg;
	CHECK_ZOMBIE(pCfg, IDS_ERR_OUTPUT_ZOMBIE);
	CComPtr<IDispatch> spDispCfg;
	pCfg->get_Object(&spDispCfg);
	CComQIPtr<VCConfiguration> spConfig = spDispCfg;
	CHECK_ZOMBIE(spConfig, IDS_ERR_OUTPUT_ZOMBIE);

	*ppConfig = spConfig.Detach();
	return S_OK;
}

STDMETHODIMP COutput2::get_Property(LPCOLESTR szProperty, VARIANT *pvar)
{
	CHECK_POINTER_NULL(pvar);

	HRESULT hr = S_FALSE;
	CStringW strProp = szProperty;
	if (strProp == L"PrimaryOutput")
	{
		CComPtr<VCConfiguration> spConfig;
		hr = GetVCConfiguration(&spConfig);
		RETURN_ON_FAIL_OR_NULL(hr, spConfig);

		CComBSTR bstrName;
		hr = spConfig->get_PrimaryOutput(&bstrName);
		CComVariant var = bstrName;
		var.Detach(pvar);
		return hr;
	}
	else if (strProp == L"COM2REG")
	{

		CComPtr<VCConfiguration> spConfig;
		hr = GetVCConfiguration(&spConfig);
		RETURN_ON_FAIL_OR_NULL(hr, spConfig);

		VARIANT_BOOL bRegister = VARIANT_FALSE;
		hr = spConfig->get_RegisterOutput(&bRegister);
		if( hr == S_FALSE )
			hr = S_OK;
		CComVariant var = bRegister;
		var.Detach(pvar);
		return hr;
	}

	RETURN_INVALID();
}


CVCBuildOutputItems::~CVCBuildOutputItems()
{
	while (!m_outputs.IsEmpty())
	{
		IVsOutput* pOutput = (IVsOutput *)m_outputs.RemoveHead();
		if (pOutput)
			pOutput->Release();
	}
}

HRESULT CVCBuildOutputItems::CreateInstance(CVCBuildOutputItems** ppOutputs)
{
	CComObject<CVCBuildOutputItems> *pOutputsObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CVCBuildOutputItems>::CreateInstance(&pOutputsObj);
	if (SUCCEEDED(hr))
		((IVCBuildOutputItems *)pOutputsObj)->AddRef();
	*ppOutputs = pOutputsObj;
	return hr;
}

STDMETHODIMP CVCBuildOutputItems::get_Count(long* pcOutputs)
{
	CHECK_POINTER_NULL(pcOutputs)

	*pcOutputs = (long) m_outputs.GetCount();
	return S_OK;
}

STDMETHODIMP CVCBuildOutputItems::Next(IVCBuildOutputItem** ppItem)
{
	CHECK_POINTER_NULL(ppItem)
	*ppItem = NULL;
	
	RETURN_ON_NULL2(m_pos, S_FALSE);

	IVsOutput* pOutput = (IVsOutput* )m_outputs.GetNext(m_pos);
	CComQIPtr<IVCBuildOutputItem> spBldOutput = pOutput;
	*ppItem = spBldOutput.Detach();
	return S_OK;
}

void CVCBuildOutputItems::Add(IVsOutput* pOutput)
{
	if (pOutput == NULL)
		return;	// not adding NULL elements to the list...

	CComQIPtr<IVCBuildOutputItem> spBldOutput = pOutput;
	if (spBldOutput == NULL)	// dependency is not one of ours
	{
		CComPtr<IVsOutput> spItem;
		if (FAILED(COutputWrapper::CreateInstance(&spItem, pOutput)) || spItem == NULL)
		{
			VSASSERT(FALSE, "Out of memory");
			return;
		}
		m_outputs.AddTail(spItem.Detach());
	}
	else
	{
		pOutput->AddRef();
		m_outputs.AddTail(pOutput);
	}
}


static LPCOLESTR s_szMspdbDll = L"mspdb70.dll";;

typedef BOOL (MRECALL *FOpen_fnptr)(OUT PMREngine *ppmre, SZC szPdb, EC& ec, char szErr[], BOOL fReproSig, BOOL fWrite);
static FOpen_fnptr s_FOpen = NULL;

HINSTANCE	CIDBDeps::s_hInstPdbDll = NULL;
wchar_t		CIDBDeps::s_szComplusDir[MAX_PATH+1] = {0};
int			CIDBDeps::s_nComPlusLen = 0;

BOOL MRECALL FEnumSrcFileForDeployment(PMREUtil pMre, EnumFile& ef, EnumType et)
{
	pMre->EnumDepFiles(ef, FEnumDepFileForDeployment);
	return TRUE;
}

BOOL MRECALL FEnumDepFileForDeployment(PMREUtil pmre, EnumFile& ef, EnumType et)
{
	CVCStringWList *pList = (CVCStringWList *)ef.pvContext;
	if ((IncludeType)(ef.fiSrc.ftInclude) == itText)
		return TRUE;

	// Check for the slight possibility that ef.szFileSrc was not updated.
	if (NULL != ef.szFileSrc && ef.szFileSrc[0] != 0)
	{
		// Filter out system-provided stuff and write into list.
		CStringW str;
		PWSTR pStr = str.GetBuffer(_MAX_PATH);
		UTF8ToUnicode(ef.szFileSrc, NULL_TERMINATED_MODE, pStr, _MAX_PATH);
		str.ReleaseBuffer();
		if (str.IsEmpty())
			return TRUE;
//		if( !CIDBDeps::IsComPlus(str) )
		pList->AddTail(str);
	}
	return TRUE;
}


BOOL CIDBDeps::IsComPlus(LPCOLESTR strFile)
{
	if( strFile == NULL )
		return FALSE;

	if( CIDBDeps::s_nComPlusLen == 0 )
	{
		LONG lRet;
		DWORD nType = REG_SZ;
		DWORD nSize = MAX_PATH;
		HKEY hSectionKey = NULL;
		lRet = RegOpenKeyExW( HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\.NETFramework", 0, KEY_READ, &hSectionKey );
		if( hSectionKey )
		{
			lRet = RegQueryValueExW( hSectionKey, L"InstallRoot", NULL, &nType, (LPBYTE)CIDBDeps::s_szComplusDir, &nSize );
			RegCloseKey( hSectionKey );
		}
		CIDBDeps::s_nComPlusLen = (int)wcslen(CIDBDeps::s_szComplusDir);
		if( CIDBDeps::s_nComPlusLen == 0 )
			CIDBDeps::s_nComPlusLen = -1;	// failed, don't bother again.
	}
	if( CIDBDeps::s_nComPlusLen > 0 )
	{
		if( !_wcsnicmp(strFile,CIDBDeps::s_szComplusDir, CIDBDeps::s_nComPlusLen) )
			return TRUE;
	}
	return FALSE;
}

HRESULT CIDBDeps::InitMRE()
{
	if (s_hInstPdbDll == NULL)
	{
		s_hInstPdbDll = LoadLibraryW(s_szMspdbDll);
		if (NULL == s_hInstPdbDll)
			return E_FAIL;
	}
#ifdef _WIN64
	s_FOpen = (FOpen_fnptr)GetProcAddress(s_hInstPdbDll,"?FOpen@MREngine@@SAHPEAPEAU1@PEBDAEAJQEADHH@Z");  // NOTE: There is no GetProcAddressW in Win32
#else
	s_FOpen = (FOpen_fnptr)GetProcAddress(s_hInstPdbDll,"?FOpen@MREngine@@SGHPAPAU1@PBDAAJQADHH@Z");  // NOTE: There is no GetProcAddressW in Win32
#endif
	if (NULL == s_FOpen)
	{
		BOOL bOK = FreeLibrary(s_hInstPdbDll);
		s_hInstPdbDll = NULL;
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CIDBDeps::GetImportDependencies(BSTR bstrIdbFile, CVCStringWList *pList)
{
	HRESULT hr = S_OK;
	if( pList == NULL )
			return E_FAIL;

	if( !s_FOpen )
	{
		hr = InitMRE();
		if( !SUCCEEDED(hr) )
			return hr;
	}

	PMREngine pMre;
	EC ec;
	CStringA strIdbFileA = bstrIdbFile;	// yes, ANSI -- 'cause CBldMreDependencies needs it this way

	if( _access(strIdbFileA, 0) == -1 )
		return E_FAIL;
	if( !(*s_FOpen)(&pMre, strIdbFileA, ec, NULL, FALSE, FALSE) ) 
		return E_FAIL;
	if( pMre == NULL )
		return E_FAIL;

	PMREUtil pMreUtil;
	pMre->QueryMreUtil(pMreUtil);
	if( pMreUtil )
	{
		pMreUtil->EnumSrcFiles(FEnumSrcFileForDeployment, NULL, (void*)pList);
		pMreUtil->FRelease();
	}
	pMre->FClose(FALSE);

	return S_OK;
}


HRESULT CIDBDeps::GetLibDependencies( VCConfiguration *pConfig, BSTR bstrPrimaryOutput, CVCStringWList *pList )
{
	LPVOID					lpvFile;
	PIMAGE_FILE_HEADER		pIFH;
	PIMAGE_OPTIONAL_HEADER	pIOH;
	PIMAGE_SECTION_HEADER	pISH;
	CString					strPath;

	{
	    CHandle hMap;
	    {
			// Open the file for read.
			CHandle hFile;
			hFile.Attach( CreateFileW(bstrPrimaryOutput, GENERIC_READ, FILE_SHARE_READ, 
								NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL) );
						
			// Exit now if the file failed to open.
			if (hFile == INVALID_HANDLE_VALUE) 
				return S_OK;
			  
			// Create a file mapping object for the open module.
			hMap.Attach( CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL) );
	    }
	    
	    // Exit now if the file failed to map.
		RETURN_ON_NULL2(hMap, S_OK);
	    
	    // Create a file mapping view for the open module.
	    lpvFile = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	}

	PIMAGE_DOS_HEADER pIDH = (PIMAGE_DOS_HEADER)lpvFile;

	// Check for the DOS signature ("MZ").
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE) {
		UnmapViewOfFile(lpvFile);
		return S_OK;
	}

	// Map an IMAGE_NT_HEADERS structure onto our module file mapping.
	PIMAGE_NT_HEADERS pINTH = (PIMAGE_NT_HEADERS)((DWORD_PTR)lpvFile + pIDH->e_lfanew);

	// Check for NT/PE signature ("PE\0\0").
	if (pINTH->Signature != IMAGE_NT_SIGNATURE) {
		UnmapViewOfFile(lpvFile);
		return S_OK;
	}

	// Map our IMAGE_FILE_HEADER structure onto our module file mapping.
	pIFH = &pINTH->FileHeader;

	// Map our IMAGE_OPTIONAL_HEADER structure onto our module file mapping.
	pIOH = &pINTH->OptionalHeader;
	// If this module has no imports (like NTDLL.DLL), then just return success.
	if (pIOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0) {
		UnmapViewOfFile(lpvFile);
		return S_OK;
	}

	// Map our IMAGE_SECTION_HEADER structure array onto our module file mapping
	pISH = IMAGE_FIRST_SECTION(pINTH);

	// Locate our Import Image Directory's relative virtual address
	DWORD VAImageDir = pIOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	// Locate the section that contains this Image Directory. We do this by
	// walking through all of our sections until we find the one that specifies
	// an address range that our Image Directory fits in.
	PIMAGE_SECTION_HEADER pISHFound = NULL;
	int i;
	for (i = 0; i < pIFH->NumberOfSections; i++) {
		if ((VAImageDir >= pISH[i].VirtualAddress) &&
			 (VAImageDir < (pISH[i].VirtualAddress + pISH[i].SizeOfRawData)))
		{
			pISHFound = &pISH[i];
			break;
		}
	}

	// Bail out if we could not find a section that owns our Image Directory.
	if (pISHFound == NULL) {
		UnmapViewOfFile(lpvFile);
		return S_OK;
	}

	// Now lets setup the search path for DLLs
	// According to spec this is ...
	
	// Exe dir,
	// WorkingDir
	// Window\\system32
	// Windows
	// Rest of Path

	CStringW strFullPath;
	// The directory that the output file itself is in
	CDirW dirOutput;
	if (dirOutput.CreateFromPath(bstrPrimaryOutput))
	{
		strFullPath = (const wchar_t *)dirOutput;
		strFullPath += L";";
	}

	// now append the working directory for the debugger, if any
	CComPtr<IDispatch> spDispDebugSettings;
	if (SUCCEEDED(pConfig->get_DebugSettings(&spDispDebugSettings)))
	{
		CComQIPtr<VCDebugSettings> spDebugSettings = spDispDebugSettings;
		if (spDebugSettings)
		{
			CComBSTR bstrPath2;
			if (SUCCEEDED(spDebugSettings->get_WorkingDirectory(&bstrPath2)))
			{
				if (bstrPath2.Length() )
				{
					strFullPath += bstrPath2;
					strFullPath += L";";
				}
			}
		}
	}

	// now append the Windows System Directory
	CStringW strSys;
	::GetSystemDirectoryW(strSys.GetBuffer(MAX_PATH+1),MAX_PATH);
	strSys.ReleaseBuffer();
	strFullPath += strSys;
	strFullPath += L";";

	// now append the Windows Directory
	CStringA strWin;
	// Review - Note this funtion is not wrapped in Win9x Lib.
	::GetWindowsDirectoryA(strWin.GetBuffer(MAX_PATH+1),MAX_PATH);
	strWin.ReleaseBuffer();
	strFullPath += strWin;
	strFullPath += L";";

	// calculate path using this configs path variable
	CComPtr< IDispatch > spDispPlatform;
	if ( pConfig->get_Platform(&spDispPlatform)==S_OK)
	{
		CComQIPtr<VCPlatform> spPlatform = spDispPlatform;
		if (spPlatform)
		{
			CComBSTR bstrExePath1, bstrExePath2;
			spPlatform->get_ExecutableDirectories(&bstrExePath1);
			spPlatform->Evaluate(bstrExePath1, &bstrExePath2);
			strFullPath += bstrExePath2;
			strFullPath += L";";
		}
	}

	// Compute our base that everything else is an offset from. We do this by
	// taking our base file pointer and adding our section's PointerToRawData,
	// which is an absolute offset value into our file.  We then subtract off our
	// Virtual Address since the offsets we are going to be adding later will be
	// relative to the this Virtual Address
	DWORD_PTR dwBase = (DWORD_PTR)lpvFile + pISHFound->PointerToRawData - pISHFound->VirtualAddress;

	// To locate the beginning of our Image Import Descriptor array, we add our
	// Image Directory offset to our base.
	PIMAGE_IMPORT_DESCRIPTOR pIID = (PIMAGE_IMPORT_DESCRIPTOR)(dwBase + VAImageDir);

	// Loop through all the Image Import Descriptors in the array.
	while (pIID->OriginalFirstThunk || pIID->FirstThunk)
	{
		CComBSTR bstrName =(LPCSTR)(dwBase + pIID->Name);
		int j = 0;
		while( s_SystemFiles[j] && _wcsicmp( bstrName, s_SystemFiles[j] ) )
		{
			j++;
		}
		if( s_SystemFiles[j] == NULL )
		{ 
			wchar_t *szEnd = NULL;
			CStringW strBuf;
			DWORD dwOK = SearchPathW(strFullPath, bstrName, NULL, 4095, strBuf.GetBuffer(4096), &szEnd);
			strBuf.ReleaseBuffer();
			if( dwOK )
				pList->AddTail(strBuf);
		}
		pIID++;
	}
	UnmapViewOfFile(lpvFile);
	return S_OK;
}
