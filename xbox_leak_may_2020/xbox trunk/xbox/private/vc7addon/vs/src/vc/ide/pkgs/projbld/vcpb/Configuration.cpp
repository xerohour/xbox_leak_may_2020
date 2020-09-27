// Configuration.cpp : Implementation of CYupApp and DLL registration.

#include "stdafx.h"
#include "Configuration.h"
#include "buildengine.h"
#include "msgboxes2.h"
#include "collection.h"
#include "debugsettings.h"
#include "ProjWriter.h"
#include "vctool.h"
#include <atltime.h>
#include "register.h"
#include "linktool.h"
#include "cltool.h"

extern "C" const __declspec(selectany) GUID IID_ICategorizeProperties = { 0x4d07fc10, 0xf931, 0x11ce, { 0xb0, 0x1, 0x0, 0xaa, 0x0, 0x68, 0x84, 0xe5} };

CTestableSection CConfiguration::g_sectionBuildEngine;

/////////////////////////////////////////////////////////////////////////////
//
HRESULT CConfiguration::CreateInstance(VCConfiguration **ppVCConfiguration, LPCOLESTR szConfigName, VCPlatform *pPlatform, VCProject *pProject)
{
	HRESULT hr;
	CConfiguration *pVar;
	CComObject<CConfiguration> *pObj;
	hr = CComObject<CConfiguration>::CreateInstance(&pObj);
	if (SUCCEEDED(hr))
	{
		pVar = pObj;
		pVar->AddRef();
		*ppVCConfiguration = pVar;
		hr = pVar->Initialize(szConfigName, pPlatform, pProject);
	}
	return hr;
}


HRESULT CConfiguration::Initialize(LPCOLESTR szConfigName, VCPlatform *pPlatform, VCProject *pProject)
{
	m_bstrName = szConfigName;
	m_pPlatform = pPlatform;
	m_pProject = pProject;
	// form the full name (config name|platform name)
	CComBSTR bstrPlatformName;
	m_pPlatform->get_Name( &bstrPlatformName );
	m_bstrFullName = m_bstrName;
	m_bstrFullName.Append(L"|");
	m_bstrFullName.Append( bstrPlatformName );

	HRESULT hr = CDebugSettings::CreateInstance(&m_pDebug, this);
	RETURN_ON_FAIL(hr);

	DoSetATLCRTStyle(VARIANT_FALSE, false /* no dirty */);
	DoSetUseOfMFC(useMfcStdWin, false /* no dirty */);

	ConfigurationTypes configType;
	SGetConfigurationType(&configType);
	return DoSetConfigurationType(configType, TRUE);	// this will also initialize the tools
}

CConfiguration::CConfiguration()
{
	m_bIsValidConfiguration = VARIANT_TRUE;	// DIANEME_TODO: determine when this isn't true
	m_bIsBuildable = VARIANT_TRUE;	// DIANEME_TODO: determine when this isn't true
	m_bInitExtensionList = FALSE;
}

CConfiguration::~CConfiguration()
{
	ClearToolExtensionList();
}

HRESULT CConfiguration::SetDirty(VARIANT_BOOL bDirty)
{
	CComQIPtr<IVCProjectImpl> pProjImpl = m_pProject;
	if (pProjImpl)
		return pProjImpl->put_IsDirty(bDirty);

	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_CFG_ZOMBIE);
}

HRESULT CConfiguration::DoSetIntProperty(long idProp, BOOL bCheckSpecial, long nValue, long nOverride /* = -1 */)
{
	if (bCheckSpecial)
	{
		HRESULT hr = S_OK;
		if (idProp == VCCFGID_ConfigurationType)
		{
			ConfigurationTypes configType = typeUnknown;
			hr = GetLocalIntProperty(VCCFGID_ConfigurationType, (long *)&configType);
			if (hr == S_OK && configType == nValue)	// nothing to do
				return S_OK;
			return DoSetConfigurationType((ConfigurationTypes)nValue);
		}
	}

	return CStyleSheetBase::DoSetIntProperty(idProp, bCheckSpecial, nValue, nOverride);
}

HRESULT CConfiguration::DoSetBoolProperty(long idProp, BOOL bCheckSpecial, VARIANT_BOOL bValue, long nOverrideID /* = -1 */)
{
	if (bCheckSpecial)
	{
		if (idProp == VCCFGID_AnyToolDirty)
		{
			HRESULT hr = CStyleSheetBase::DoSetBoolProperty(VCCFGID_AnyToolDirty, FALSE, bValue, nOverrideID);
			RETURN_ON_FAIL(hr);
			if (m_spBuildEngine != NULL)
				return CStyleSheetBase::DoSetBoolProperty(VCCFGID_DirtyDuringBuild, FALSE, bValue, nOverrideID);
			else
				return hr;
		}
	}

	return CStyleSheetBase::DoSetBoolProperty(idProp, bCheckSpecial, bValue, nOverrideID);
}

STDMETHODIMP CConfiguration::GenerateToolWrapperList()
{
	HRESULT hr = DoGenerateToolWrapperList();
	if (hr == S_OK && PROJECT_IS_LOADED())
		AssignActions(VARIANT_FALSE);

	return hr;
}

STDMETHODIMP CConfiguration::get_ConfigurationName(BSTR * pVal)
{
	CHECK_POINTER_VALID(pVal);
	return m_bstrName.CopyTo(pVal);
}

STDMETHODIMP CConfiguration::put_ConfigurationName(BSTR bstrVal)
{
	CHECK_OK_TO_DIRTY(VCCFGID_ConfigurationName);
	m_bstrName = bstrVal;
	return S_OK;
}

STDMETHODIMP CConfiguration::get_Name(BSTR * pVal)
{
	CHECK_POINTER_VALID( pVal );
	return m_bstrFullName.CopyTo(pVal);
}

STDMETHODIMP CConfiguration::SaveObject(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent)
{
	HRESULT hr;

	// Name
	CComBSTR bstrName;
	get_Name( &bstrName );
	NodeAttribute( xml, L"Name", bstrName );

	// Output Dir
	CComVariant varOutputDirectory;
	if( GetLocalProp( VCCFGID_OutputDirectory, &varOutputDirectory ) == S_OK && varOutputDirectory.vt == VT_BSTR)
	{
		CComBSTR bstrOutputDirectory = varOutputDirectory.bstrVal;
		if (bstrOutputDirectory.Length() > 0 )
			NodeAttribute( xml, L"OutputDirectory", bstrOutputDirectory );
	}

	// Intermediate Dir
	CComVariant varIntermediateDirectory;
	if( GetLocalProp( VCCFGID_IntermediateDirectory, &varIntermediateDirectory ) == S_OK && varIntermediateDirectory.vt == VT_BSTR)
	{
		CComBSTR bstrIntermediateDirectory = varIntermediateDirectory.bstrVal;
		if (bstrIntermediateDirectory.Length() > 0 )
			NodeAttribute( xml, L"IntermediateDirectory", bstrIntermediateDirectory );
	}

	// Config type
	OLECHAR szEnumVal[10] = {0};
	CComVariant varConfigType;
	hr = GetLocalProp( VCCFGID_ConfigurationType, &varConfigType );
	VSASSERT(hr == S_OK && (varConfigType.vt == VT_I2 || varConfigType.vt == VT_I4), "ConfigurationType property should never be default!");
	if (varConfigType.vt != VT_I2 && varConfigType.vt != VT_I4)
	{
		varConfigType.lVal = 0;
		varConfigType.vt = VT_I4;
	}
	NodeAttribute( xml, L"ConfigurationType", CComBSTR( _itow( varConfigType.lVal, szEnumVal, 10 ) ) );

	// Applied styles
	CComVariant varStyleSheets;
	if( GetLocalProp( VCCFGID_AppliedStyleSheets, &varStyleSheets ) == S_OK && varStyleSheets.vt == VT_BSTR)
	{
		CComBSTR bstrStyleSheets = varStyleSheets.bstrVal;
		if (bstrStyleSheets.Length() > 0 )
			NodeAttribute( xml, L"AppliedStyleSheets", bstrStyleSheets );
	}

	// Use of MFC
	CComVariant varUseMfc;
	if (GetLocalProp(VCCFGID_UseOfMFC, &varUseMfc) == S_OK && (varUseMfc.vt == VT_I2 || varUseMfc.vt == VT_I4))
		NodeAttribute( xml, L"UseOfMFC", CComBSTR( _itow( varUseMfc.lVal, szEnumVal, 10 ) ) );

	// Use of ATL
	CComVariant varUseATL;
	if (GetLocalProp(VCCFGID_UseOfATL, &varUseATL) == S_OK && (varUseATL.vt == VT_I2 || varUseATL.vt == VT_I4))
		NodeAttribute( xml, L"UseOfATL", CComBSTR( _itow( varUseATL.lVal, szEnumVal, 10 ) ) );

	// How ATL uses CRT
	CComVariant varCRT;
	if (GetLocalProp(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, &varCRT) == S_OK && (varCRT.vt == VT_BOOL || varCRT.vt == VT_I2))
		NodeAttribute( xml, L"ATLMinimizesCRunTimeLibraryUsage", varCRT.boolVal == VARIANT_TRUE ? L"TRUE" : L"FALSE" );

	// character set
	CComVariant varSet;
	if (GetLocalProp(VCCFGID_CharacterSet, &varSet) == S_OK && (varSet.vt == VT_I2 || varSet.vt == VT_I4))
		NodeAttribute( xml, L"CharacterSet", CComBSTR( _itow( varSet.lVal, szEnumVal, 10 ) ) );

	// Managed Extensions
	CComVariant varManaged;
	if (GetLocalProp(VCCFGID_ManagedExtensions, &varManaged) == S_OK && (varManaged.vt == VT_BOOL || varManaged.vt == VT_I2))
		NodeAttribute( xml, L"ManagedExtensions", varManaged.boolVal == VARIANT_TRUE ? L"TRUE" : L"FALSE" );
		
	// Wildcard extension list of things to delete in the intermediate directory on rebuild/clean
	CComVariant varDelExt;
	if (GetLocalProp(VCCFGID_DeleteExtensionsOnClean, &varDelExt) == S_OK && varDelExt.vt == VT_BSTR)
		NodeAttribute(xml, L"DeleteExtensionsOnClean", varDelExt.bstrVal);

	// perform whole program optimization on build; more or less requires optimization turned on to be effective
	CComVariant varOptimize;
	if (GetLocalProp(VCCFGID_WholeProgramOptimization, &varOptimize) == S_OK && (varOptimize.vt == VT_BOOL || varOptimize.vt == VT_I2))
		NodeAttribute(xml, L"WholeProgramOptimization", varOptimize.boolVal == VARIANT_TRUE ? L"TRUE" : L"FALSE");

	// end node header, start on children
	EndNodeHeader( xml, true );

	// tools
	long lcItems;
	long index;
	lcItems = m_rgTools.GetSize();
	for( index = 0; index < lcItems; index++ )
	{
		CComPtr<IVCToolImpl> pToolImpl;
 		CComBSTR bstrToolName;
		
		pToolImpl = m_rgTools.GetAt( index );
		if( pToolImpl )
		{
			hr = pToolImpl->get_ToolShortName( &bstrToolName );
			VSASSERT(SUCCEEDED(hr), "Tool missing its short name");
			hr = pToolImpl->SaveObject( xml, static_cast<IVCPropertyContainer *>(this), GetIndent() );
		}
	}

	return S_OK;
}

STDMETHODIMP CConfiguration::ReadFromStream(IStream *pStream)
{
	CHECK_READ_POINTER_NULL(pStream);

	while (true)
	{
		//Read in the settings header
		CComBSTR bstrName;
		HRESULT hr = bstrName.ReadFromStream(pStream);
		RETURN_ON_FAIL(hr);

		if (bstrName == L"EndConfigProperties")
			break;

		//Check to see if this is the debug settings
		if (bstrName == L"DebugSettings")
			hr = HandleReadingDebugSettings(pStream);

		else if (bstrName == L"GeneralConfigSettings")
			hr = HandleReadingGeneralConfigSettings(pStream);

		//TODO: Read in additional objects here

		else	// maybe a tool
			hr = HandleReadingToolSettings(pStream, bstrName);

		RETURN_ON_FAIL(hr);
	}
	
	return S_OK;
}

HRESULT CConfiguration::HandleReadingDebugSettings(IStream* pStream)
{
	//Get the debug settings impl
	CComQIPtr<IVCDebugSettingsImpl> pDbgSettings = m_pDebug;
	CHECK_ZOMBIE(pDbgSettings, IDS_ERR_CFG_ZOMBIE);

	//Load settings
	HRESULT hr = pDbgSettings->ReadFromStream(pStream);
	RETURN_ON_FAIL(hr);

	return S_OK;
}

HRESULT CConfiguration::HandleReadingToolSettings(IStream* pStream, CComBSTR& bstrName)
{
	HRESULT hr = S_OK;
	long cItems = m_rgTools.GetSize();
	long lItem;
	for (lItem = 0; lItem < cItems; lItem++)
	{
		CComPtr<IVCToolImpl> spToolImpl = m_rgTools.GetAt(lItem);
		if (spToolImpl == NULL)
			continue;

		VARIANT_BOOL bMatch = VARIANT_FALSE;
		spToolImpl->MatchName(bstrName, VARIANT_FALSE /* any match*/, &bMatch);
		if (bMatch)
		{
			hr = spToolImpl->ReadFromStream(pStream, static_cast<IVCPropertyContainer *>(this));
			break;
		}
	}

	if (lItem == cItems)	// no match!
	{
		CStringW strName = bstrName;
		long nLen = strName.GetLength();
		if (nLen >= 4)
		{
			strName = strName.Right(4);
			strName.MakeLower();
			if (strName == L"tool")		// it's a tool.  we just don't know it, so skip over it
			{
				while (true)
				{
					//Read in type
					VARTYPE	type;
					ULONG iByteCount;
					HRESULT hr = pStream->Read(&type, sizeof(VARTYPE), &iByteCount);
					RETURN_ON_FAIL(hr);

					//End of list marker
					if (type == VT_EMPTY) 
						break;
				}
				return S_OK;
			}
		}
		return E_UNEXPECTED;
	}

	return S_OK;
}

HRESULT CConfiguration::HandleReadingGeneralConfigSettings(IStream* pStream)
{
	while (true)
	{
		//Read in type
		VARTYPE	type;
		ULONG iByteCount;
		HRESULT hr = pStream->Read(&type, sizeof(VARTYPE), &iByteCount);
		RETURN_ON_FAIL(hr);

		//End of list marker
		if (type == VT_EMPTY) 
			break;

		//Read in dispid
		DISPID dispid;
		hr = pStream->Read(&dispid, sizeof(DISPID), &iByteCount);
		RETURN_ON_FAIL(hr);

		if (type == VT_BOOL && dispid == VCCFGID_BuildBrowserInformation)
		{
			VARIANT_BOOL bBsc;
			hr = pStream->Read(&bBsc, sizeof(VARIANT_BOOL), &iByteCount);
			RETURN_ON_FAIL(hr);
			put_BuildBrowserInformation(bBsc);
		}
		else
			return E_UNEXPECTED;	// only expecting the single property
	}

	return S_OK;
}

STDMETHODIMP CConfiguration::WriteToStream(IStream *pStream)
{
	CHECK_READ_POINTER_NULL(pStream);

	//Get and write name as header
	CComBSTR bstrName;
	HRESULT hr = get_Name(&bstrName);
	RETURN_ON_FAIL(hr);
	hr = bstrName.WriteToStream(pStream);
	RETURN_ON_FAIL(hr);

	bstrName.Empty();

	//Write out the debug settings header
	bstrName = L"DebugSettings";
	hr = bstrName.WriteToStream(pStream);
	RETURN_ON_FAIL(hr);

	//Get debug settings impl interface
	CComQIPtr<IVCDebugSettingsImpl> pDbgSettings = m_pDebug;
	RETURN_ON_FAIL_OR_NULL2(hr, pDbgSettings, E_NOINTERFACE);

	//Save to stream
	hr = pDbgSettings->WriteToStream(pStream);
	RETURN_ON_FAIL(hr);

	// Write out the user-specific general config settings
	bstrName = L"GeneralConfigSettings";
	hr = bstrName.WriteToStream(pStream);
	RETURN_ON_FAIL(hr);

	// Build browser info
	CComVariant varBrowser;
	hr = GetLocalProp(VCCFGID_BuildBrowserInformation, &varBrowser);
	if (hr == S_OK && (varBrowser.vt == VT_I2 || varBrowser.vt == VT_BOOL))
	{
		VARIANT_BOOL bBrowser = varBrowser.boolVal;
		hr = WritePropertyToStream(pStream, bBrowser, VCCFGID_BuildBrowserInformation);
		RETURN_ON_FAIL(hr);
	}

	//Write "end of property list" marker
	VARTYPE type = VT_EMPTY;
	ULONG	iByteCount;
	hr = pStream->Write(&type, sizeof(VARTYPE), &iByteCount);
	RETURN_ON_FAIL(hr);

	// tools
	long lcItems = m_rgTools.GetSize();
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCToolImpl> pToolImpl = m_rgTools.GetAt(index);
		if (!pToolImpl)
			continue;

		hr = pToolImpl->WriteToStream(pStream, static_cast<IVCPropertyContainer *>(this));
		RETURN_ON_FAIL(hr);
	}

	//TODO: Write out additional objects here

	bstrName = L"EndConfigProperties";
	hr = bstrName.WriteToStream(pStream);
	RETURN_ON_FAIL(hr);

	return S_OK;
}

STDMETHODIMP CConfiguration::SetDefaultToolForFile(IDispatch *pDispFileConfiguration)
{
	CComPtr<IDispatch> pDisp;

	CComQIPtr<IVCFileConfigurationImpl> pFileCfgImpl = pDispFileConfiguration;
	if (pFileCfgImpl == NULL)
	{
		VSASSERT(pFileCfgImpl, "Hmmm. The file config is bad somehow...");
		return E_UNEXPECTED;
	}

	CComQIPtr<VCFileConfiguration> pFileCfg = pDispFileConfiguration;
	VSASSERT(pFileCfg, "Could not possibly be one of *our* file configurations!");
	if(pFileCfg)
		pFileCfg->get_File(&pDisp);
	CComQIPtr<VCFile> pFile = pDisp;
	pDisp = NULL;

	VSASSERT(pFile, "Hmmm.  The file is bad, how'd that happen?");
	RETURN_ON_NULL2(pFile, E_UNEXPECTED);

	CComBSTR bstrExt;
	pFile->get_Extension( &bstrExt );
	if( bstrExt.Length() > 0 )
	{
		bstrExt.ToLower();
		InitToolExtensionList();
		IVCToolImpl* pTool = NULL;		// yes, I want a non-smart pointer here...
		// the extension list stores only the letter extension, NOT the "."
		if( bstrExt.Length() != 0 && m_ExtensionMap.Lookup( &bstrExt[1], (void*&)pTool ) && pTool )
			return pFileCfgImpl->SetDefaultTool( pTool, TRUE );
	}
	
	IVCToolImpl* pTool = NULL;		// yes, I want a non-smart pointer here...
	// Hmmm.  No specific tool, so use custom
	if (m_ExtensionMap.Lookup(L"bat", (void *&)pTool) && pTool != NULL)	// this will get us the custom build tool
		return pFileCfgImpl->SetDefaultTool(pTool, TRUE);

	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_NO_TOOL, IDS_ERR_NO_DEF_TOOL);
}

void CConfiguration::ClearToolExtensionList()
{
	m_ExtensionMap.RemoveAll();
	m_bInitExtensionList = FALSE;
}

void CConfiguration::InitToolExtensionList(BOOL bReInit /* = FALSE */)
{
	if (bReInit && m_bInitExtensionList)
		ClearToolExtensionList();
	else if (m_bInitExtensionList)
		return;

	m_bInitExtensionList = TRUE;

	HRESULT hr = S_OK;
	long lcItems = m_rgTools.GetSize();
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCToolImpl> pToolImpl;
		pToolImpl = m_rgTools.GetAt(index);
		if (pToolImpl == NULL)
			continue;

		VARIANT_BOOL bIsTargetTool = VARIANT_FALSE;
		hr = pToolImpl->IsTargetTool(NULL, &bIsTargetTool);
		VSASSERT(SUCCEEDED(hr), "Should *always* be able to tell whether or not a tool is a target tool!");
		if (bIsTargetTool == VARIANT_TRUE)	// don't attach a target tool to a file!!
		{
			// hmmm.  Custom build tool masquerades by default as target tool, but shouldn't be excluded here
			CComQIPtr<VCCustomBuildTool> spCustomTool = pToolImpl;
			if (spCustomTool == NULL)
				continue;
		}

		InitToolExtensionListForOneTool(pToolImpl);
	}
}

void CConfiguration::InitToolExtensionListForOneTool(IVCToolImpl* pToolImpl)
{
	CComBSTR bstrExt;
	HRESULT hr = pToolImpl->get_DefaultExtensions(&bstrExt);
	if (FAILED(hr))
		return;

	CStringW strExtensions = bstrExt;
	CStringW strExt;
	int extStart = 0;
	int extEnd = 0;
	int nLen = strExtensions.GetLength();
	if (nLen == 0)
	{
		VSASSERT(FALSE, "All tools must have an extension list!");
		return;
	}
	strExtensions.MakeLower();

	// while we're not at the end of the extension list string
	while( extStart < nLen )
	{
		// get the next extension in the filter list
		extEnd = strExtensions.Find( L';', extStart );
		// if it wasn't found there is an error
		if( extEnd == -1 )
			extEnd = nLen;

		// set up an extension to tool map
		IVCToolImpl* pNoSmartToolImpl = pToolImpl;	// want the actual pointer, not the overload of a CComPtr
		strExt = strExtensions.Mid( extStart, extEnd - extStart );
		strExt.TrimLeft();
		strExt.TrimRight();
		if (wcsncmp(strExt, L"*.", 2) == 0)
			strExt = strExt.Right(strExt.GetLength()-2);
		m_ExtensionMap.SetAt( strExt, (void *)pNoSmartToolImpl );
		extStart = extEnd + 1;
	}
}

STDMETHODIMP CConfiguration::MatchName(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched)
{
	CHECK_POINTER_VALID(pbMatched);
	CComBSTR bstrName;

	get_Name(&bstrName);
	*pbMatched = VARIANT_FALSE;
	CHECK_READ_POINTER_VALID(bstrNameToMatch);
	RETURN_ON_NULL2(bstrName, E_UNEXPECTED);

	if ( _wcsicmp(bstrName,bstrNameToMatch) == 0)
	{
		*pbMatched = VARIANT_TRUE;
	}
	else if (!bFullOnly)	// try matching without the platform
	{
		CStringW strName = bstrName;
		int nPos = strName.Find(L"|");
		if (nPos > 0)
			strName = strName.Left(nPos);
		if (_wcsicmp(strName,bstrNameToMatch) == 0)
			*pbMatched = VARIANT_TRUE;
	}
	return S_OK;
}

STDMETHODIMP CConfiguration::put_Name(BSTR pVal)
{
	CHECK_OK_TO_DIRTY(VCCFGID_Name);

	//Find '|'
	CComBSTR bstrVal = pVal;

	const wchar_t *pStr = wcsrchr(pVal,L'|');
	int index = -1;
	if( pStr )
	{
		index = (int)(pStr - pVal);
	}

	if (index >= 0)
	{
		//Copy only the portion we need
		BSTR bstrTemp;
		bstrTemp = SysAllocStringLen(pVal, index);
		m_bstrName = bstrTemp;
		SysFreeString(bstrTemp);
		CStringW strName = m_bstrName;
		strName.TrimRight();
		if (m_bstrName.Length() != strName.GetLength())
			m_bstrName = strName;
		// set the full name
		m_bstrFullName = pVal;
	}
	else	//No '|'
	{
		m_bstrName = pVal;
		// set the full name
		m_bstrFullName = m_bstrName;
		m_bstrFullName.Append(L"|");
		CComBSTR bstrPlatformName;
		m_pPlatform->get_Name( &bstrPlatformName );
		m_bstrFullName.Append( bstrPlatformName );
	}
	return S_OK;
}

STDMETHODIMP CConfiguration::get_Platform(IDispatch * * ppVal)
{
	return DoGetPlatform(ppVal);
}

STDMETHODIMP CConfiguration::get_PrimaryOutput(BSTR * pVal)
{
	CHECK_POINTER_VALID(pVal);
	*pVal = NULL;

	long lcItems = m_rgTools.GetSize();
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCToolImpl> spToolImpl = m_rgTools.GetAt(index);
		if (spToolImpl == NULL)
			continue;

		VARIANT_BOOL bHasPrimaryOutput = VARIANT_FALSE;
		IVCPropertyContainer* pContainer = this;
		if (SUCCEEDED(spToolImpl->HasPrimaryOutputFromTool(pContainer, VARIANT_TRUE, &bHasPrimaryOutput)) && bHasPrimaryOutput == VARIANT_TRUE)
			return spToolImpl->GetPrimaryOutputFromTool(pContainer, VARIANT_TRUE, pVal);
	}
	return S_FALSE;
}

STDMETHODIMP CConfiguration::GetPrimaryOutputFileID(IVCToolImpl** ppTool, long* pnOutputID)
{
	if (ppTool)
		*ppTool = NULL;
	CHECK_POINTER_NULL(pnOutputID);
	*pnOutputID = -1;

	long lcItems = m_rgTools.GetSize();
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCToolImpl> spToolImpl = m_rgTools.GetAt(index);
		if (spToolImpl == NULL)
			continue;

		VARIANT_BOOL bHasPrimaryOutput = VARIANT_FALSE;
		IVCPropertyContainer* pContainer = this;
		if (SUCCEEDED(spToolImpl->HasPrimaryOutputFromTool(pContainer, VARIANT_TRUE, &bHasPrimaryOutput)) && bHasPrimaryOutput == VARIANT_TRUE)
		{
			HRESULT hr = spToolImpl->GetPrimaryOutputIDFromTool(pContainer, VARIANT_TRUE, pnOutputID);
			if (ppTool && SUCCEEDED(hr))
				*ppTool = spToolImpl.Detach();
			return hr;
		}
	}
	return S_FALSE;
}

STDMETHODIMP CConfiguration::get_ImportLibrary(BSTR * pVal)
{
	CHECK_POINTER_VALID(pVal);
	*pVal = NULL;

	long lcItems = m_rgTools.GetSize();
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCToolImpl> spToolImpl = m_rgTools.GetAt(index);
		if (spToolImpl == NULL)
			continue;

		IVCPropertyContainer* pContainer = this;
		if (spToolImpl->GetImportLibraryFromTool(pContainer, pVal) == S_OK)
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CConfiguration::get_ProgramDatabase(BSTR* pbstrPDB)
{
	CHECK_POINTER_VALID(pbstrPDB);
	*pbstrPDB = NULL;

	long lcItems = m_rgTools.GetSize();
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCToolImpl> spToolImpl = m_rgTools.GetAt(index);
		if (spToolImpl == NULL)
			continue;

		IVCPropertyContainer* pContainer = this;
		if (spToolImpl->GetProgramDatabaseFromTool(pContainer, VARIANT_TRUE, pbstrPDB) == S_OK)
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CConfiguration::get_ErrorContext(IVCBuildErrorContext** ppEC)
{
	CHECK_POINTER_NULL(ppEC);
	m_spBuildErrorContext.CopyTo(ppEC);
	return (m_spBuildErrorContext ? S_OK : S_FALSE);
}

STDMETHODIMP CConfiguration::get_OutputDirectory(BSTR * pVal)
{
	CComBSTR bstrOutputDirectory;
	DoGetStrProperty(VCCFGID_OutputDirectory, FALSE, &bstrOutputDirectory);
	if (bstrOutputDirectory.Length() == 0)
	{
		bstrOutputDirectory = m_bstrName;
		put_OutputDirectory(bstrOutputDirectory);
	}
	return bstrOutputDirectory.CopyTo(pVal);
}

STDMETHODIMP CConfiguration::put_OutputDirectory(BSTR newVal)
{
	return SetStrProperty(VCCFGID_OutputDirectory, newVal);
}

STDMETHODIMP CConfiguration::get_IntermediateDirectory(BSTR * pVal)
{
	CComBSTR bstrIntermediateDirectory;
	DoGetStrProperty(VCCFGID_IntermediateDirectory, FALSE, &bstrIntermediateDirectory);
	if (bstrIntermediateDirectory.Length() == 0)
	{
		bstrIntermediateDirectory = m_bstrName;
		put_IntermediateDirectory(bstrIntermediateDirectory);
	}
	return bstrIntermediateDirectory.CopyTo(pVal);
}

STDMETHODIMP CConfiguration::put_IntermediateDirectory(BSTR newVal)
{
	return SetStrProperty(VCCFGID_IntermediateDirectory, newVal);
}

STDMETHODIMP CConfiguration::Delete()
{
	CHECK_OK_TO_DIRTY(0);

	return E_NOTIMPL;
}

STDMETHODIMP CConfiguration::Build()
{
	return DoBuild(TOB_Build);
}

STDMETHODIMP CConfiguration::Rebuild()
{
	return DoBuild(TOB_ReBuild);
}

STDMETHODIMP CConfiguration::Clean()
{
	return DoBuild(TOB_Clean);
}

HRESULT CConfiguration::DoBuild(bldActionTypes bldType)
{
	CComPtr<IVCGenericConfiguration> spGenCfg;
	HRESULT hr = S_OK;
	if (CVCProjectEngine::s_pBuildPackage != NULL)
	{
		hr = CVCProjectEngine::s_pBuildPackage->GetUIConfigurationForVCConfiguration(this, &spGenCfg);
		RETURN_ON_FAIL(hr);
	}

	if (m_spBuildEngine != NULL)
		return S_FALSE;

	if (spGenCfg != NULL)
		return spGenCfg->StartAutomationBuild(bldType);

	// hmmmm.  must mean we don't have any access to the shell. gotta do everything ourselves, then.
	CBldDummyGenCfg* pDummyGenCfg = NULL;
	hr = CBldDummyGenCfg::CreateInstance(&pDummyGenCfg, this);
	VSASSERT(SUCCEEDED(hr) && pDummyGenCfg != NULL, "No dummy gencfg.  Probably out of memory condition");
	RETURN_ON_FAIL(hr);
	CHECK_ZOMBIE(pDummyGenCfg, IDS_ERR_CFG_ZOMBIE);

	// yes, there's a loose ref count here.  it will take care of itself when the build finishes
	return pDummyGenCfg->DoStartBuild(bldType, 0, NULL);
}

STDMETHODIMP CConfiguration::get_Tools(IDispatch **ppVal)
{
	return DoGetTools(ppVal);
}

STDMETHODIMP CConfiguration::get_FileTools(IVCCollection **ppVal)
{
	return DoGetFileTools(ppVal);
}

STDMETHODIMP CConfiguration::get_DebugSettings(IDispatch * * pVal)
{
	CHECK_POINTER_VALID(pVal);
	
	if (m_pDebug)	
		m_pDebug.QueryInterface(pVal);
	else 
		*pVal = NULL;
	return S_OK;
}

STDMETHODIMP CConfiguration::get_UpToDate(VARIANT_BOOL* pbUpToDate)
{
	CHECK_POINTER_VALID(pbUpToDate);
	*pbUpToDate = VARIANT_TRUE;

	if (m_spBuildEngine || m_spBuildErrorContext)	// already something going
		return E_FAIL;

	CComQIPtr<IVCProjectEngineImpl> spProjEngineImpl = g_pProjectEngine;
	RETURN_ON_NULL2(spProjEngineImpl, E_UNEXPECTED);

	CComPtr<IDispatch> spDispBuildEngine;
	HRESULT hr = spProjEngineImpl->get_BuildEngine(&spDispBuildEngine);
	RETURN_ON_FAIL(hr);
	CComQIPtr<IVCBuildEngine> spBuildEngine = spDispBuildEngine;
	CComQIPtr<IVCBuildEngineImpl> spBuildEngineImpl = spDispBuildEngine;
	if (spBuildEngine == NULL || spBuildEngineImpl == NULL)
		return E_UNEXPECTED;

	CComPtr<IVCBuildErrorContext> spEC;
	hr = CBldCommandLineEC::CreateInstance(&spEC, NULL, spBuildEngine);
	RETURN_ON_FAIL_OR_NULL(hr, spEC);
	spEC->put_AssociatedBuildEngine(spDispBuildEngine);

	{	// scope the crit section
		CritSectionT cs(g_sectionBuildEngine);
		if (m_spBuildEngine || m_spBuildErrorContext)	// already something going
		{
			spEC->put_AssociatedBuildEngine(NULL);
			return E_FAIL;
		}
		m_spBuildEngine = spBuildEngine;
		m_spBuildErrorContext = spEC;
	}

	CBldCfgRecord* pCfgRecord = NULL;
	DoInitializeDepGraph(&pCfgRecord);
	CHECK_ZOMBIE(pCfgRecord, IDS_ERR_CFG_ZOMBIE);

	RefreshCommandLineOptionsAndOutputs(NULL);	// just in case a dirty command line changes something...
	hr = spBuildEngineImpl->HasBuildState((void *)pCfgRecord, spEC, pbUpToDate);
	ClearDirtyCommandLineOptionsFlag();
	
	{	// scope the crit section
		CritSectionT cs(g_sectionBuildEngine);
		m_spBuildEngine.Release();
		m_spBuildErrorContext.Release();
	}

	return S_OK;
}

STDMETHODIMP CConfiguration::get_Dependencies(IVCBuildStringCollection** ppDependencies)
{
	CHECK_POINTER_NULL(ppDependencies);
	*ppDependencies = NULL;

	RETURN_ON_NULL2(m_spActionList, S_FALSE);

	DoInitializeDepGraph(NULL);

	return m_spActionList->get_ProjectConfigurationDependencies(ppDependencies);
}

STDMETHODIMP CConfiguration::get_BuildOutputs(IVCBuildStringCollection** ppBuildOutputs)
{
	CHECK_POINTER_NULL(ppBuildOutputs);
	*ppBuildOutputs = NULL;

	RETURN_ON_NULL2(m_spActionList, S_FALSE);

	DoInitializeDepGraph(NULL);

	return m_spActionList->get_ProjectConfigurationBuildOutputs(ppBuildOutputs);
}

STDMETHODIMP CConfiguration::get_ProjectDependencies(IVCBuildOutputItems** ppProjDependencies)
{
	CHECK_POINTER_NULL(ppProjDependencies);
	*ppProjDependencies = NULL;

	RETURN_ON_NULL2(CVCProjectEngine::s_pBuildPackage, S_OK);

	return CVCProjectEngine::s_pBuildPackage->GetConfigurationDependencies(this, ppProjDependencies);
}

STDMETHODIMP CConfiguration::get_ProjectDependenciesAsStrings(IVCBuildStringCollection** ppProjDepStrings)
{
	CHECK_POINTER_NULL(ppProjDepStrings);
	*ppProjDepStrings = NULL;

	CComPtr<IVCBuildOutputItems> spProjDeps;
	HRESULT hr = get_ProjectDependencies(&spProjDeps);
	RETURN_ON_FAIL(hr);
	if (hr == S_FALSE || spProjDeps == NULL)
		return S_FALSE;

	hr = CVCBuildStringCollection::CreateInstance(ppProjDepStrings);
	RETURN_ON_FAIL_OR_NULL2(hr, *ppProjDepStrings, E_OUTOFMEMORY);

	spProjDeps->Reset();
	while (TRUE)
	{
		CComPtr<IVCBuildOutputItem> spItem;
		hr = spProjDeps->Next(&spItem);
		if (hr != S_OK)
			break;
		if (spItem == NULL)
			continue;

		CComBSTR bstrDep;
		hr = spItem->get_FullPathName(&bstrDep);
		if (FAILED(hr))
			continue;

		CStringW strDep = bstrDep;
		int nDot = strDep.ReverseFind(L'.');
		if (nDot < 0)	// don't care about it if it doesn't have an extension at all
			continue;

		CStringW strExt = strDep.Right(strDep.GetLength()-nDot);
		if (strExt.GetLength() == 4 && _wcsicmp(strExt, L".lib") != 0)
			continue;	// only care about .lib deps

		(*ppProjDepStrings)->Add(bstrDep);
	}

	return S_OK;
}

STDMETHODIMP CConfiguration::get_HasProjectDependencies(VARIANT_BOOL* pbHasDeps)
{
	CHECK_POINTER_NULL(pbHasDeps);
	*pbHasDeps = VARIANT_FALSE;

	RETURN_ON_NULL2(CVCProjectEngine::s_pBuildPackage, S_OK);	// nothing more we can do

	return CVCProjectEngine::s_pBuildPackage->HasConfigurationDependencies(this, pbHasDeps);
}

STDMETHODIMP CConfiguration::get_KnownDeploymentDependencies(IVCBuildStringCollection** ppDeployDepStrings)
{
	CHECK_POINTER_NULL(ppDeployDepStrings);
	*ppDeployDepStrings = NULL;

	CBldCfgRecord* pCfgRecord = NULL;
	DoInitializeDepGraph(&pCfgRecord);		// make sure this gets initialized properly so we have actions set up...

	CComPtr<IVCBuildEngine> spBuildEngine;
	HRESULT hr = CDynamicBuildEngine::CreateInstance(&spBuildEngine);
	CComQIPtr<IVCBuildEngineImpl> spBuildEngineImpl = spBuildEngine;
	RETURN_ON_FAIL_OR_NULL(hr, spBuildEngineImpl);

	spBuildEngineImpl->GetDeploymentDependencies((void *)pCfgRecord, ppDeployDepStrings);
	if (*ppDeployDepStrings)
		(*ppDeployDepStrings)->RemoveDuplicates(VARIANT_TRUE /* paths */, VARIANT_FALSE /* not case sensitive */);

	return S_OK;
}

STDMETHODIMP CConfiguration::get_DeployableOutputsCount(long* pnOutputs)
{	// number of deployable outputs
	CHECK_POINTER_NULL(pnOutputs);
	*pnOutputs = 0;

	long lcItems = m_rgTools.GetSize();
	IVCPropertyContainer* pPropContainer = this;
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCToolImpl> pToolImpl;
		pToolImpl = m_rgTools.GetAt(index);
		if (pToolImpl == NULL)
			continue;

		long cToolOutputs;
		HRESULT hr = pToolImpl->GetDeployableOutputsCount(pPropContainer, &cToolOutputs);
		if (SUCCEEDED(hr) && cToolOutputs)
			*pnOutputs += cToolOutputs;
	}

	return S_OK;
}

STDMETHODIMP CConfiguration::get_DeployableOutputs(IVCBuildStringCollection** ppDeployableOutputs)
{	// the deployable outputs
	CHECK_POINTER_NULL(ppDeployableOutputs);
	*ppDeployableOutputs = NULL;

	IVCPropertyContainer* pPropContainer = this;
	long lcItems = m_rgTools.GetSize();
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCToolImpl> pToolImpl;
		pToolImpl = m_rgTools.GetAt(index);
		if (pToolImpl == NULL)
			continue;

		CComPtr<IVCBuildStringCollection> spOutputs;
		HRESULT hr = pToolImpl->GetDeployableOutputs(pPropContainer, &spOutputs);
		if (SUCCEEDED(hr) && spOutputs)
		{
			if (*ppDeployableOutputs == NULL)
				*ppDeployableOutputs = spOutputs.Detach();
			else
			{
				spOutputs->Reset();
				while (TRUE)
				{
					CComBSTR bstrOutput;
					hr = spOutputs->Next(&bstrOutput);
					if (hr != S_OK)
						break;
					if (bstrOutput.Length() > 0)
						(*ppDeployableOutputs)->Add(bstrOutput);
				}
			}
		}
	}

	return S_OK;
}

STDMETHODIMP CConfiguration::SupportsBuildType(bldActionTypes buildType, VARIANT_BOOL* pbSupports, VARIANT_BOOL* pbAvailable)
{
	CHECK_POINTER_NULL(pbSupports);
	CHECK_POINTER_NULL(pbAvailable);
	*pbSupports = VARIANT_TRUE;
	*pbAvailable = VARIANT_FALSE;

	toolSetType listStyle;
	DoGetToolSetType(listStyle);

	if (buildType == TOB_Link)
	{
		if (listStyle == toolSetLinker || listStyle == toolSetLibrarian)
			*pbAvailable = VARIANT_TRUE;
		else
			*pbSupports = VARIANT_FALSE;
		if (*pbSupports == VARIANT_FALSE)
		{
			*pbAvailable = VARIANT_FALSE;
			return S_OK;
		}
		return S_OK;
	}

	if (buildType == TOB_Deploy)
	{
		*pbSupports = (listStyle == toolSetLinker);
		VARIANT_BOOL bExcluded = VARIANT_TRUE;
		if (GetBoolProperty(VCDPLYID_ExcludedFromBuild, &bExcluded) != S_OK)
			bExcluded = VARIANT_TRUE;
		*pbAvailable = (*pbSupports && !bExcluded) ? VARIANT_TRUE : VARIANT_FALSE;
		return S_OK;
	}

	if (buildType == TOB_XboxImage)
	{
		*pbSupports = (listStyle == toolSetLinker);
		*pbAvailable = *pbSupports ? VARIANT_TRUE : VARIANT_FALSE;
		return S_OK;
	}

	if (buildType == TOB_XboxDeploy)
	{
		*pbSupports = (listStyle == toolSetLinker);
		VARIANT_BOOL bExcluded = VARIANT_TRUE;
		if (GetBoolProperty(XBOXDPLYID_ExcludedFromBuild, &bExcluded) != S_OK)
			bExcluded = VARIANT_FALSE; // Default is not excluded
		*pbAvailable = (*pbSupports && !bExcluded) ? VARIANT_TRUE : VARIANT_FALSE;
		return S_OK;
	}

	if (IsBuildEvent(buildType))
	{
		switch (buildType)
		{
		case TOB_PreBuildEvent:
		case TOB_PostBuildEvent:
			*pbSupports = (listStyle != toolSetMakefile);
			break;
		default:
			VSASSERT(FALSE, "Hey, unsupported build event type!");
			// fall through
		case TOB_PreLinkEvent:
			*pbSupports = (listStyle == toolSetLinker || listStyle == toolSetLibrarian);
			break;
		}
		if (*pbSupports == VARIANT_FALSE)
		{
			*pbAvailable = VARIANT_FALSE;
			return S_OK;
		}
		long nIDCmd = PREBLDID_CommandLine;
		long nExcludeID = PREBLDID_ExcludedFromBuild;
		if (buildType == TOB_PreLinkEvent)
		{
			nIDCmd = PRELINKID_CommandLine;
			nExcludeID = PRELINKID_ExcludedFromBuild;
		}
		else if (buildType == TOB_PostBuildEvent)
		{
			nIDCmd = POSTBLDID_CommandLine;
			nExcludeID = POSTBLDID_ExcludedFromBuild;
		}
		VARIANT_BOOL bExcluded = VARIANT_FALSE;
		if (GetBoolProperty(nExcludeID, &bExcluded) != S_OK)
			bExcluded = VARIANT_FALSE;
		if (bExcluded == VARIANT_FALSE)
		{
			CComBSTR bstrCmd;
			*pbAvailable = (GetStrProperty(nIDCmd, &bstrCmd) == S_OK && bstrCmd.Length() > 0) ? VARIANT_TRUE : VARIANT_FALSE;
		}
		return S_OK;
	}

	if (buildType == TOB_Compile)
		*pbSupports = VARIANT_FALSE;
	else
		*pbAvailable = VARIANT_TRUE;
		
	return S_OK;
}

STDMETHODIMP CConfiguration::DirtyOutputs(long nProjID)
{	// dirty outputs based on the passed-in project ID
	DirtyActionOutputsAndCommandLineOptions(nProjID, nProjID, GetOverrideID(nProjID));
	return S_OK;
}

STDMETHODIMP CConfiguration::get_Project(IDispatch * * pVal)
{
	CHECK_POINTER_VALID(pVal);
	*pVal = NULL;
	CHECK_ZOMBIE(m_pProject, IDS_ERR_PROJ_ZOMBIE);
	return m_pProject.QueryInterface(pVal);
}

STDMETHODIMP CConfiguration::put_Project(IDispatch * pVal)
{
	m_pProject = pVal;
	return S_OK;
}

STDMETHODIMP CConfiguration::get_IsValidConfiguration(VARIANT_BOOL* pbIsValidConfiguration)
{
	CHECK_POINTER_NULL(pbIsValidConfiguration);
	*pbIsValidConfiguration = m_bIsValidConfiguration;
	return S_OK;
}


STDMETHODIMP CConfiguration::get_IsBuildable(VARIANT_BOOL* pbIsBuildable)
{
	CHECK_POINTER_NULL(pbIsBuildable);
	*pbIsBuildable = m_bIsBuildable;
	return S_OK;
}

STDMETHODIMP CConfiguration::get_CanStartBuild(VARIANT_BOOL* pbCanBuild)
{
	CHECK_POINTER_NULL(pbCanBuild);
	*pbCanBuild = (m_spBuildEngine == NULL);	// we've got a build in progress if we have a build engine...
	return S_OK;
}

STDMETHODIMP CConfiguration::get_MrePath(BSTR* pbstrMrePath)
{
	CHECK_POINTER_NULL(pbstrMrePath);
	*pbstrMrePath = NULL;

	// Get filename of minimal rebuild file(vc70.idb).
	CComBSTR bstrPdb;
	CVCCLCompilerTool::GetResolvedPdbName(this, &bstrPdb);
	CStringW strIdb = bstrPdb;
	if (strIdb.GetLength() > 3 && wcsnicmp(strIdb.Right(4), L".pdb", 4) == 0)
	{
		int nLen = strIdb.GetLength();
		strIdb.SetAt(nLen-3, L'i');	// replace the 'p' with an 'i' in '.pdb'
		bstrPdb = strIdb;
	}

	*pbstrMrePath = bstrPdb.Detach();
	return S_OK;
}

STDMETHODIMP CConfiguration::get_BaseToolList(void** ppPtrList)
{
	CHECK_POINTER_NULL(ppPtrList);
	*ppPtrList = (void *)(&m_toolWrapperList);
	return S_OK;
}

STDMETHODIMP CConfiguration::AssignActions(VARIANT_BOOL bOnLoad)
{
	CBldCfgRecord* pcr = g_StaticBuildEngine.GetCfgRecord(m_pProject, reinterpret_cast<IDispatch *>(this));
	CBldAction::AssignActions(pcr, NULL);
	return S_OK;
}

STDMETHODIMP CConfiguration::UnAssignActions(VARIANT_BOOL bOnClose)
{
	CComQIPtr<VCProject> pVCProject = m_pProject;
	CBldCfgRecord* pcr = g_StaticBuildEngine.GetCfgRecord(pVCProject, reinterpret_cast<IDispatch *>(this));
	CBldAction::UnAssignActions(pcr, NULL);
	if (bOnClose == VARIANT_TRUE)
		g_StaticBuildEngine.RemoveCfgRecord(m_pProject, reinterpret_cast<IDispatch *>(this));

	return S_OK;
}

HRESULT CConfiguration::DoClearToolWrapperList()
{
	if (PROJECT_IS_LOADED() && !m_toolWrapperList.IsEmpty())
		UnAssignActions(VARIANT_FALSE);

	return CStyleSheetBase::DoClearToolWrapperList();
}

STDMETHODIMP CConfiguration::ClearToolWrapperList()
{
	return DoClearToolWrapperList();
}

STDMETHODIMP CConfiguration::Close()
{
	if (!g_bInProjClose)
		DoClearToolWrapperList();	// need this done BEFORE we release the actions...

	m_pProject.Release();
	CComQIPtr<IVCDebugSettingsImpl> spDebugImpl = m_pDebug;
	if (spDebugImpl)
		spDebugImpl->Close();
	m_pDebug.Release();
	m_spActionList.Release();
	m_rgFileCfgs.RemoveAll();	// NOTE: this list here is not what needs closing...

	return FinishClose();
}

void CConfiguration::DoGetToolSetType(toolSetType& listStyle)
{
	ConfigurationTypes configType;
	HRESULT hr = GetIntProperty(VCCFGID_ConfigurationType, (long *)&configType);
	VSASSERT(hr == S_OK, "ConfigurationType should never be default!");
	listStyle = DetermineBaseType(configType);
}

STDMETHODIMP CConfiguration::get_ConfigurationType(ConfigurationTypes* pconfigType)
{
	CHECK_POINTER_VALID(pconfigType);
	return GetIntProperty(VCCFGID_ConfigurationType, (long *)pconfigType);
}

STDMETHODIMP CConfiguration::put_ConfigurationType(ConfigurationTypes configType)
{
	return SetIntProperty(VCCFGID_ConfigurationType, configType);
}

HRESULT CConfiguration::DoSetConfigurationType(ConfigurationTypes configType, BOOL bForceUpdate /* = FALSE */)
{
	// check validity
	switch (configType)
	{
	case typeUnknown:
	case typeApplication:
	case typeDynamicLibrary:
	case typeStaticLibrary:
	case typeGeneric:
		break;	// everything OK
	default:
		RETURN_INVALID();
	}

	toolSetType nOldBaseType = toolSetAll;
	if (!bForceUpdate)
	{
		ConfigurationTypes oldConfigType;
		HRESULT hr = DoGetIntProperty(VCCFGID_ConfigurationType, TRUE, (long *)&oldConfigType);
		nOldBaseType = DetermineBaseType(oldConfigType);
		VSASSERT(hr == S_OK, "ConfigurationType property should always be non-default!");
	}
	toolSetType nNewBaseType = DetermineBaseType(configType);

	HRESULT hr = DoSetIntProperty(VCCFGID_ConfigurationType, FALSE, configType);
	RETURN_ON_FAIL(hr);

	BOOL bResetTools = FALSE;
	if (bForceUpdate || nOldBaseType != nNewBaseType)
	{
		InitializeTools();
		ClearToolExtensionList();
		bResetTools = PROJECT_IS_LOADED();
	}

	CComBSTR bstrIntrinsicStyles;
	DoGetStrProperty(VCCFGID_IntrinsicAppliedStyleSheets, FALSE, &bstrIntrinsicStyles);
	if (configType == typeDynamicLibrary)
		hr = AddStyle(this, bstrIntrinsicStyles, WINDLL_STYLE);
	else
		hr = RemoveStyle(this, bstrIntrinsicStyles, WINDLL_STYLE);
	if (hr == S_OK)
		DoSetStrProperty(VCCFGID_IntrinsicAppliedStyleSheets, FALSE, bstrIntrinsicStyles);

	if (!bResetTools)
		return S_OK;

	// un-set the tool on files for makefile projects
	CComPtr<IEnumVARIANT> spEnum;
	if ((nNewBaseType == toolSetMakefile) && SUCCEEDED(GetFileListEnumerator(&spEnum)))
	{
		while (TRUE)
		{
			CComPtr<VCFileConfiguration> spFileCfg;
			hr = GetNextFileConfig(spEnum, &spFileCfg);
			if (hr != S_OK)
				break;
			CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = spFileCfg;
			if (spFileCfgImpl == NULL)
				continue;

			spFileCfgImpl->SetDefaultTool( NULL, FALSE );	// don't want an unassign/assign action pair on put_Tool...
		}
	}

	if (bResetTools && PROJECT_IS_LOADED())
		AssignActions(VARIANT_FALSE);		// UnAssign was called (indirectly) by InitializeTools

	return S_OK;
}

HRESULT CConfiguration::VerifyFileTools()
{
	if (!(PROJECT_IS_LOADED()))
		return S_OK;

	CComPtr<IEnumVARIANT> spFiles;
	HRESULT hr = CBldAction::GetFileCollection(this, &spFiles);
	RETURN_SPECIFIC_ON_FAIL_OR_NULL(hr, spFiles, S_FALSE);

	while (TRUE)
	{
		CComVariant var;
		hr = spFiles->Next(1, &var, NULL);
		BREAK_ON_DONE(hr);
		CComPtr<VCFileConfiguration> spFileCfg;
		if (!CBldAction::GetFileCfgForProjCfg(var, this, &spFileCfg))
			continue;
		CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = spFileCfg;
		if (!spFileCfgImpl)
			continue;
		spFileCfgImpl->VerifyToolValid();
	}

	return S_OK;
}

STDMETHODIMP CConfiguration::get_AppliedStyleSheets(BSTR* pbstrStyles)
{
	HRESULT hr = GetStrProperty(VCCFGID_AppliedStyleSheets, pbstrStyles);
	if (hr == S_FALSE)
		GetDefaultValue(VCCFGID_AppliedStyleSheets, pbstrStyles, this);
	return hr;
}

STDMETHODIMP CConfiguration::put_AppliedStyleSheets(BSTR bstrStyles)
{
	return DoSetAppliedStyleSheets(bstrStyles, TRUE);
}

STDMETHODIMP CConfiguration::get_BuildBrowserInformation(VARIANT_BOOL* pbBsc)
{	// build .bsc file
	CHECK_POINTER_VALID(pbBsc);
	return GetBoolProperty(VCCFGID_BuildBrowserInformation, pbBsc);
}

STDMETHODIMP CConfiguration::put_BuildBrowserInformation(VARIANT_BOOL bBsc)
{
	return SetBoolProperty(VCCFGID_BuildBrowserInformation, bBsc);
}

STDMETHODIMP CConfiguration::get_UseOfMFC(useOfMfc* useMfc)
{	// how are we using MFC?
	CHECK_POINTER_VALID(useMfc);
	return GetIntProperty(VCCFGID_UseOfMFC, (long *)useMfc);
}

STDMETHODIMP CConfiguration::put_UseOfMFC(useOfMfc useMfc)
{
	return SetIntProperty(VCCFGID_UseOfMFC, useMfc);
}

STDMETHODIMP CConfiguration::get_UseOfATL(useOfATL* useATL)
{	// how are we using ATL?
	CHECK_POINTER_VALID(useATL);
	return GetIntProperty(VCCFGID_UseOfATL, (long *)useATL);
}

STDMETHODIMP CConfiguration::put_UseOfATL(useOfATL useATL)
{
	return SetIntProperty(VCCFGID_UseOfATL, useATL);
}

STDMETHODIMP CConfiguration::get_ATLMinimizesCRunTimeLibraryUsage(VARIANT_BOOL* pbCRT)
{	// is this an ATL min dependency?
	CHECK_POINTER_VALID(pbCRT);
	return GetBoolProperty(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, pbCRT);
}

STDMETHODIMP CConfiguration::put_ATLMinimizesCRunTimeLibraryUsage(VARIANT_BOOL bCRT)
{
	return SetBoolProperty(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, bCRT);
}

STDMETHODIMP CConfiguration::get_CharacterSet(charSet* pset)
{	// what type of character set are we using?
	CHECK_POINTER_VALID(pset);
	return GetIntProperty(VCCFGID_CharacterSet, (long *)pset);
}

STDMETHODIMP CConfiguration::put_CharacterSet(charSet set)
{
	return SetIntProperty(VCCFGID_CharacterSet, set);
}

STDMETHODIMP CConfiguration::get_ManagedExtensions(VARIANT_BOOL* bManaged)
{	// is this a managed extensions project?
	CHECK_POINTER_VALID(bManaged);
	return GetBoolProperty(VCCFGID_ManagedExtensions, bManaged);
}

STDMETHODIMP CConfiguration::put_ManagedExtensions(VARIANT_BOOL bManaged)
{
	return SetBoolProperty(VCCFGID_ManagedExtensions, bManaged);
}

STDMETHODIMP CConfiguration::get_DeleteExtensionsOnClean(BSTR* pbstrExt)
{	// wildcard extension list to delete on clean/rebuild from within the intermediate directory
	HRESULT hr = GetStrProperty(VCCFGID_DeleteExtensionsOnClean, pbstrExt);
	if (hr == S_FALSE)
		GetDefaultValue(VCCFGID_DeleteExtensionsOnClean, pbstrExt, this);
	return hr;
}

STDMETHODIMP CConfiguration::put_DeleteExtensionsOnClean(BSTR bstrExt)
{
	return SetStrProperty(VCCFGID_DeleteExtensionsOnClean, bstrExt);
}

STDMETHODIMP CConfiguration::get_WholeProgramOptimization(VARIANT_BOOL* pbOptimized)
{  // perform whole program optimization on build; more or less requires optimization turned on to be effective
	return GetBoolProperty(VCCFGID_WholeProgramOptimization, pbOptimized);
}

STDMETHODIMP CConfiguration::put_WholeProgramOptimization(VARIANT_BOOL bOptimize)
{
	return SetBoolProperty(VCCFGID_WholeProgramOptimization, bOptimize);
}

STDMETHODIMP CConfiguration::get_RegisterOutput(VARIANT_BOOL* pbRegister)
{	// register the primary output of the build
	CHECK_POINTER_VALID(pbRegister);
	*pbRegister = VARIANT_FALSE;

	ConfigurationTypes configType = typeApplication;
	get_ConfigurationType(&configType);
	if (configType != typeDynamicLibrary)
		return S_FALSE;

	HRESULT hr = GetBoolProperty(VCLINKID_RegisterOutput, pbRegister);
	if (hr == S_FALSE)
		CVCLinkerTool::s_optHandler.GetDefaultValue( VCLINKID_RegisterOutput, pbRegister );
	return hr;
}

STDMETHODIMP CConfiguration::CopyTo(IDispatch *pDispDestCfg)
{
	CHECK_ZOMBIE(m_pProject, IDS_ERR_CFG_ZOMBIE);
	CHECK_READ_POINTER_VALID(pDispDestCfg);

	HRESULT hr;

	CComQIPtr<VCConfiguration> pDestCfg = pDispDestCfg;
	CHECK_READ_POINTER_NULL(pDestCfg);

	// clone
	CComQIPtr<IVCPropertyContainer> pPropCnt = (VCConfiguration*)this;
	RETURN_ON_NULL2(pPropCnt, E_NOINTERFACE);
	CComQIPtr<IVCConfigurationImpl> pDestCfgImpl = pDestCfg;
	RETURN_ON_NULL2(pDestCfgImpl, E_NOINTERFACE);
	pDestCfgImpl->Clone( pPropCnt );
	// clone file configs
	// get the file collection
	CComPtr<IDispatch> pDisp;
	m_pProject->get_Files( &pDisp );
	CComQIPtr<IVCCollection> pColl = pDisp;
	RETURN_ON_NULL(pColl);
	long num_files;
	CComVariant varIdx;
	// for each file
	hr = pColl->get_Count( &num_files );
	for( long i = 1; i <= num_files; i++ )
	{
		varIdx = i;
		CComPtr<IDispatch> pDispItem;
		hr = pColl->Item( varIdx, &pDispItem );
		if( FAILED( hr ) )
			continue;
		// get the file impl
		CComQIPtr<IVCFileImpl> pFileImpl = pDispItem;
		if( !pFileImpl )
			continue;
		// get the file config
		CComPtr<VCFileConfiguration> pFileCfg;
		hr = pFileImpl->GetFileConfigurationForProjectConfiguration( (VCConfiguration*)this, &pFileCfg );
		if( !pFileCfg )
			continue;
		// get the file config impl
		CComQIPtr<IVCFileConfigurationImpl> pFileCfgImpl = pFileCfg;
		if( !pFileCfgImpl )
			continue;
		// get the prop container for the file cfg
		CComQIPtr<IVCPropertyContainer> pFilePropCnt = pFileCfg;
		// get the cfg that we added to the file
		CComPtr<VCFileConfiguration> pNewFileCfg;
		hr = pFileImpl->GetFileConfigurationForProjectConfiguration( pDestCfg, &pNewFileCfg );
		CComQIPtr<IVCFileConfigurationImpl> pNewFileCfgImpl = pNewFileCfg;
		if( !pNewFileCfgImpl )
			continue;
		// clone it
		hr = pNewFileCfgImpl->Clone( pFilePropCnt );
	}
	return hr;
}

STDMETHODIMP CConfiguration::TopLevelBuild(bldActionTypes buildType, ULONG cFiles, VCFile* pFiles[], 
	IVCBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, BOOL bRecurse, BOOL fDoIdleAndPump, BOOL fCleanupEngine,
	BOOL bProvideBanner, long* warnings, long* errors, BOOL* bCancelled)
{
#ifdef PROF_BUILD_OVERHEAD
	StartCAPAll();
#endif

	if (pEC == NULL || pBuildEngine == NULL)
	{
		VSASSERT(FALSE, "No error context passed to TopLevelBuild!");
		RETURN_INVALID();
	}

	{	// scope the crit section
		CritSectionT cs(g_sectionBuildEngine);
		if (m_spBuildEngine || m_spBuildErrorContext)	// already something going
			return E_FAIL;
		m_spBuildEngine = pBuildEngine;
		m_spBuildErrorContext = pEC;
	}
	
	CHECK_POINTER_NULL(warnings);
	CHECK_POINTER_NULL(errors);
	CHECK_POINTER_NULL(bCancelled);

	BuildResults br = BuildComplete;
	BOOL bQuery = FALSE;
	BuildType bt;
	CStringW str;
	m_spBuildEngine = pBuildEngine;
	CComQIPtr<IVCBuildEngineImpl> spBuildEngineImpl = m_spBuildEngine;
	if (spBuildEngineImpl)
		spBuildEngineImpl->put_DoIdleAndPump(fDoIdleAndPump);

	DWORD startTime;

	// if we're doing performance logging
	// NOTE: matching build end is in buildengine.cpp, CDynamicBuildEngine::DoBuild()
	VARIANT_BOOL bLog = VARIANT_FALSE;
	g_pProjectEngine->get_PerformanceLogging( &bLog );
	if( bLog == VARIANT_TRUE )
	{
		// start time
		startTime = GetTickCount();
		fprintf( stdout, "Project Build start: %d\n", startTime );
	}

	FILETIME cTime;
	::GetSystemTimeAsFileTime(&cTime);
	CFileTime fileStartTime(cTime);
	
	long errs = 0, actualErrs = 0, warns = 0;

	CComBSTR bstrName;
	HRESULT hr = get_Name(&bstrName);
	VSASSERT(SUCCEEDED(hr), "Should always be able to obtain configuration's name!");
	CStringW strCfgName = bstrName;
	VSASSERT(!strCfgName.IsEmpty(), "Configurations *must* have names.  Zombie condition?");
	if (strCfgName.IsEmpty())
	{
#ifdef PROF_BUILD_OVERHEAD
		StopCAPAll();
#endif
		VSASSERT(FALSE, "Config name is empty in TopLevelBuild!");
		CleanUpEngine(fCleanupEngine);
		RETURN_INVALID();
	}
	int nBar = strCfgName.Find(L"|");
	if (nBar >= 0)
	{
		CStringW strNameLeft = strCfgName.Left(nBar);
		int nLen = strCfgName.GetLength();
		CStringW strNameRight = strCfgName.Right(nLen-nBar-1);
		strCfgName = strNameLeft;
		if (!strNameLeft.IsEmpty() && !strNameRight.IsEmpty())
			strCfgName += L" ";
		strCfgName += strNameRight;
	}

	VARIANT_BOOL bCanSpawn = VARIANT_TRUE;
	pBuildEngine->get_CanSpawnBuild(&bCanSpawn);
	if (bCanSpawn != VARIANT_TRUE)
	{
#ifdef PROF_BUILD_OVERHEAD
		StopCAPAll();
#endif
		VSASSERT(FALSE, "Cannot spawn spawner in TopLevelBuild!");
		CleanUpEngine(fCleanupEngine);
		return E_UNEXPECTED;
	}

	VSASSERT(m_pProject != NULL, "m_pProject only NULL if bad config initialization or config is zombie.");

	CStringW strProjectName;
	if (m_pProject)
	{
		CComBSTR bstrProjName;
		hr = m_pProject->get_Name(&bstrProjName);
		VSASSERT(SUCCEEDED(hr), "Should always be able to figure out project's name!");
		strProjectName = bstrProjName;
	}

	// No properties can change from here on out.  We can do this before a save 
	// because we know we won't be doing a save as, so the project filename and
	// and directory won't be changing

	// What type of build are we attempting to do, compile, build, or batch build?	
	if (cFiles > 0)
	{
		bt = Compile;
	}
	else if (buildType == TOB_Link)
		bt = Link;
	else if (IsBuildEvent(buildType))
		bt = BuildEvent;
	else
		bt = BatchBuild;

	// Initialize the spawner and output window:
#ifdef _DEBUG
	VARIANT_BOOL bSpawnerActive = VARIANT_FALSE;
	m_spBuildEngine->get_SpawnerActive(&bSpawnerActive);
	VSASSERT(bSpawnerActive == VARIANT_FALSE, "How did we get a build started when another one was in progress?");
#endif	// _DEBUG

	hr = spBuildEngineImpl->InitializeSpawner(FALSE);
	if (FAILED(hr))
	{
#ifdef PROF_BUILD_OVERHEAD
		StopCAPAll();
#endif
		VSASSERT(FALSE, "Failed to init spawner in TopLevelBuild");
		CleanUpEngine(fCleanupEngine);
		return E_UNEXPECTED;
	}
	else if (hr == S_FALSE)
	{
#ifdef PROF_BUILD_OVERHEAD
		StopCAPAll();
#endif
		*bCancelled = TRUE;
		CleanUpEngine(TRUE);	// we're not going to be back if the user cancelled...
		return S_FALSE;
	}

	if (bProvideBanner && pEC)
	{
		CStringW strTmp;
		strTmp.Format(IDS_BUILD_TARGETNAME, strProjectName, strCfgName);
		CComBSTR bstrTmp = strTmp;
		pEC->AddLine(bstrTmp);
	}

	CBldLogEnabler *pLog = NULL;
	if (buildType != TOB_Clean)
	{
		pLog = new CBldLogEnabler(this, m_spBuildEngine);
		if (CVCProjectEngine::s_bBuildLogging == VARIANT_TRUE)
		{
			VARIANT_BOOL bHaveLogging = VARIANT_FALSE;
			m_spBuildEngine->get_HaveLogging(&bHaveLogging);
			if (bHaveLogging == VARIANT_FALSE)
			{
				CComBSTR bstrWarn;
				bstrWarn.LoadString(IDS_WARN_NOBUILDLOG_PRJ0009);
				CVCProjectEngine::AddProjectWarning(pEC, bstrWarn, L"PRJ0009", this);
			}
		}
	}
	
	do   // this loops once and uses continue like a goto end
	{
		BOOL bIsClean = (buildType == TOB_Clean);
		DoPreparedBuild(cFiles, pFiles, buildType, bIsClean, pEC, bt, bRecurse, &br);

		pEC->get_Errors(&errs);
		pEC->get_Warnings(&warns);

		// Reconsile any differences between if we got errors, and the return code from the build.
		// REVIEW: the whole counting of errors scheme should be reworked so we don't have to do this.
		if( errs )
		{
			actualErrs = errs;
			if( br != BuildError )
			{
				// We had errors, make sure we return an error code.
				br = BuildError;
			}
		}
		else
		{
			// We had no errors but we have an error code, so report 1.
			if( br == BuildError )
				actualErrs = 1;
		}

		// BLOCK: Buzz the user...
		if (buildType != TOB_Clean)
		{
			UINT nBeepType = 0;
			UINT idsBeep = IDS_SOUND_BUILD_COMPLETE;

			// only beep on compile file, which the shell doesn't handle
			// beeping for
			if (buildType == TOB_Compile)
			{
				switch (br)
				{
				case BuildCanceled:
					break;

				case BuildError:
				case BuildComplete:
					if (actualErrs != 0)
					{
						idsBeep = IDS_SOUND_BUILD_ERROR;
						nBeepType = MB_ICONEXCLAMATION;
					}
					else if (warns != 0)
					{
						idsBeep = IDS_SOUND_BUILD_WARNING;
						nBeepType = MB_ICONQUESTION;
					}
					else
					{
						idsBeep = IDS_SOUND_BUILD_COMPLETE;
						nBeepType = MB_ICONASTERISK;
					}
					break;

				case NoBuild:
					idsBeep = IDS_SOUND_BUILD_UPTODATE;
					nBeepType = MB_ICONASTERISK;
					break;
				}
				if (nBeepType != 0)
					MessageBeep(nBeepType);
			}

			m_spBuildEngine->ClearPendingEnvironmentVariableWarnings();

			if (br == NoBuild) 
			{
				::VCMsgTextW(str, IDS_BUILD_UPTODATE, (const wchar_t *) strProjectName);
				CComBSTR bstr = str;
				pEC->AddLine(bstr);
			}
			else
			{
				pEC->AddLine(L"");
				// we already put out a message if the build was cancelled...
				if (br != BuildCanceled)
				{
					pEC->AddLine(L"");

					m_spBuildEngine->put_CurrentLogSection(eLogResults);

					CStringW strPreHeader;
					strPreHeader.LoadString(IDS_HTML_PRE_HEADER);
					CStringW strHeader;
					strHeader.LoadString(IDS_RESULTS);
					CStringW strPostHeader;
					strPostHeader.LoadString(IDS_HTML_POST_HEADER);

					CStringW strResults = strPreHeader + strHeader + strPostHeader;

					CComBSTR bstrResults = strResults;
					m_spBuildEngine->LogTrace(eLogResults, bstrResults);
				}
			}
		}
	} while(0);


	::GetSystemTimeAsFileTime(&cTime);
	CFileTime fileEndTime(cTime);

	VARIANT_BOOL bTime = VARIANT_FALSE;
	g_pProjectEngine->get_BuildTiming(&bTime);

	if ( (bTime == VARIANT_TRUE) && (buildType != TOB_Clean))
	{
		CFileTimeSpan fileElapsedTime = fileEndTime - fileStartTime;
		LONGLONG llElapsedTime = fileElapsedTime.GetTimeSpan();
		int nMinutes = (int)(llElapsedTime / 600000000);
		int nSeconds = (int)((llElapsedTime % 600000000) / 10000000);

 		CStringW strElapsedTime;
 		strElapsedTime.Format(IDS_BUILD_TIME, nMinutes, nSeconds);
		CComBSTR bstrElapsedTime = strElapsedTime;
		pEC->AddLine(bstrElapsedTime);

		// if we're doing performance logging also, log to stdout also
		VARIANT_BOOL bLog = VARIANT_FALSE;
		g_pProjectEngine->get_PerformanceLogging( &bLog );
		if( bLog == VARIANT_TRUE )
		{
			CStringA strBuildTime;	// yes, ANSI -- being used to write to stdout
			strBuildTime.LoadString(IDS_LOG_BUILD_TIME);
			fprintf( stdout, strBuildTime, nMinutes, nSeconds);
		}
	}

	if ((buildType != TOB_Clean) && (br != NoBuild))
	{
		// dump "build log saved at..." message to error context
		VARIANT_BOOL bLogging = VARIANT_FALSE;
		if (SUCCEEDED(g_pProjectEngine->get_BuildLogging(&bLogging)) && bLogging)
		{
			CComQIPtr<IVCBuildEngineImpl> pBldEngineImpl = m_spBuildEngine;
			if( pBldEngineImpl )
			{
				CComBSTR bstrLogFile;
				pBldEngineImpl->get_LogFile( &bstrLogFile );
				CStringW strFile = bstrLogFile;
				CStringW strMsg;
				strMsg.Format(IDS_BLDLOG_SAVED, strFile);
				CComBSTR bstrLogMsg = strMsg;
				pEC->AddLine( bstrLogMsg );
			}
		}

		::VCMsgTextW(str, IDS_BUILD_COMPLETE, (const wchar_t *) strProjectName, actualErrs, warns);
		CComBSTR bstr = str;
		pEC->AddLine(bstr);

		if (bProvideBanner)
		{
			CStringW strTmp;
			if (actualErrs)
				strTmp.LoadString(IDS_BUILD_FAILED);
			else
				strTmp.LoadString(IDS_BUILD_SUCCEEDED);
			CStringW strTmp2;
			strTmp2.LoadString(IDS_DONE);
			strTmp2 += strTmp;
			bstr = strTmp2;
			pEC->AddLine(bstr);
		}
	}

#ifdef PROF_BUILD_OVERHEAD
	StopCAPAll();
#endif

	// set out params
	*errors = actualErrs;
	*warnings = warns;

	spBuildEngineImpl->TerminateSpawner();
	*bCancelled = (br == BuildCanceled ? TRUE : FALSE);

	CleanUpEngine(fCleanupEngine || *bCancelled);
	delete pLog;

	if (br == BuildComplete || br == NoBuild) 
		return S_OK;
	
	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BUILD_FAILED, IDS_ERR_BUILD_FAILED);
}

void CConfiguration::CleanUpEngine(BOOL fCleanupEngine)
{
	if (!fCleanupEngine)
		return;

	CritSectionT cs(g_sectionBuildEngine);
	m_spBuildEngine.Release();
	m_spBuildErrorContext.Release();
}

HRESULT CConfiguration::DoPreparedBuild(ULONG cFiles, VCFile* pFiles[], bldActionTypes buildType, 
	BOOL bContinueAfterErrors, IVCBuildErrorContext* pEC, BuildType bt, BOOL bRecurse, BuildResults* pbrResults)
{
	CHECK_POINTER_NULL(pbrResults);
	*pbrResults = NoBuild;

	BOOL bDoOnce = (cFiles == 0);

	if (bRecurse && m_spBuildEngine)
		m_spBuildEngine->put_ReuseLogFile(VARIANT_TRUE);

	CComBSTR bstrProjCfgName;
	HRESULT hr = get_Name(&bstrProjCfgName);
	VSASSERT(SUCCEEDED(hr), "Should always be able to figure out configuration's name!");
	CStringW strProjCfgName = bstrProjCfgName;
	VSASSERT(!strProjCfgName.IsEmpty(), "valid configs *all* have names.  Config must be zombie.");

	CStringW strProjectName;
	if (m_pProject != NULL)
	{
		CComBSTR bstrProjName;
		hr = m_pProject->get_Name(&bstrProjName);
		VSASSERT(SUCCEEDED(hr), "Should always be able to figure out project's name!");
		strProjectName = bstrProjName;
	}

	BldFileRegHandle frhStart;
	bldAttributes aob;

	// Spawner should be active:
#ifdef _DEBUG
	VARIANT_BOOL bActive = VARIANT_FALSE;
	m_spBuildEngine->get_SpawnerActive(&bActive);
	VSASSERT(bActive == VARIANT_TRUE, "How did we get a build started when one was already in progress?");
#endif	// _DEBUG

	if (buildType != TOB_Clean)
	{
		CStringW str;
		m_spBuildEngine->LogTrace(eLogHeader, L"<h3>");
		::VCMsgTextW(str, IDS_BUILD_TARGETNAME, (const wchar_t *) strProjectName, (const wchar_t *) strProjCfgName);
		CComBSTR bstr = str;
		m_spBuildEngine->LogTrace(eLogHeader, bstr);
		m_spBuildEngine->LogTrace(eLogHeader, L"</h3>\n");
	}
	else
	{
		CStringW str;
		::VCMsgTextW(str, IDS_DESC_CLEANING, (const wchar_t *) strProjectName, (const wchar_t *) strProjCfgName);
		str += L"\n";
		CComBSTR bstr = str;
		pEC->AddLine(bstr);
	}

	aob = (bContinueAfterErrors) ? (bldAttributes)(AOB_Default | AOB_IgnoreErrors) : AOB_Default;

	if ((bt == Compile) || (bt == BuildEvent) || (bt == Link))
	{
		UINT aobTmp = (UINT)aob;
		aobTmp |= AOB_Forced;
		aob = (bldAttributes)aobTmp;
	}

	CComPtr<IDispatch> spDispProject;
	hr = get_Project(&spDispProject);
	VSASSERT(SUCCEEDED(hr), "If no project, we must be zombie and we shouldn't be doing a build if we're zombie...");

	CBldCfgRecord* pCfgRecord = NULL;
	CComQIPtr<IVCBuildEngineImpl> spBuildEngineImpl = m_spBuildEngine;
	if (spBuildEngineImpl == NULL)
	{
		VSASSERT(FALSE, "Bad build engine pointer in CConfiguration::DoPreparedBuild!");
		*pbrResults = BuildError;
		goto EndBuild;
	}

	DoInitializeDepGraph(&pCfgRecord);
	if (pCfgRecord == NULL)
	{
		// Assert was fired in DoInitializeDepGraph
		*pbrResults = BuildError;
		goto EndBuild;
	}

	if (cFiles != 0 && pFiles == NULL)
	{
		VSASSERT(FALSE, "Cannot specify build files without providing the files to build");
		*pbrResults = BuildError;
		goto EndBuild;
	}

	for (ULONG idx = 0; (idx < cFiles || bDoOnce) && (*pbrResults == BuildComplete || *pbrResults == NoBuild); idx++)
	{
		bDoOnce = FALSE;	// done it now
		CComPtr<VCFileConfiguration> spFileCfg;
		if (cFiles > 0 && pFiles != NULL)
		{
			CComQIPtr<IVCFileImpl> spFileImpl = pFiles[idx];
			if (spFileImpl != NULL)
				spFileImpl->GetFileConfigurationForProjectConfiguration(this, &spFileCfg);
			VSASSERT(spFileCfg != NULL, "File was not initialized properly with all the project configs");
			if (spFileCfg == NULL)
				continue;
		}

		CBldConfigurationHolder cfgHolder(pCfgRecord, spFileCfg, this);
		BOOL bFileRecurse = TRUE;
		frhStart = (BldFileRegHandle)NULL;
		CComPtr<IVCBuildAction> spAction;
		if (!cfgHolder.IsProject() || (bt == Link))
		{
			// FUTURE: short term fix .. build an output
			CComPtr<IVCBuildActionList> spActions;
			cfgHolder.GetActionList(&spActions);
			long cActions = 0;
			BOOL bOK = (spActions != NULL) && SUCCEEDED(spActions->get_Count(&cActions));
			VSASSERT(bOK, "Should always be able to pick up the actions list during a build!");
			if (cActions == 0)
			{
				*pbrResults = BuildError;
				goto EndBuild;
			}

			if (bt == Link)
			{
				toolSetType listStyle;
				DoGetToolSetType(listStyle);
				if (listStyle == toolSetLibrarian)
					spActions->FindByName(szLibrarianToolShortName, &spAction);
				else
					spActions->FindByName(szLinkerToolShortName, &spAction);
			}
			else
				spActions->get_LastAction(&spAction);

			if (spAction == NULL)
			{
				*pbrResults = NoBuild;
				goto EndBuild;
			}

			if (idx == 0)	// only need to do this once per build...
				RefreshCommandLineOptionsAndOutputs(pEC);

			CBldFileRegSet* pfrsOutput = NULL;
			hr = spAction->GetOutput(pEC, (void **)&pfrsOutput);
			VSASSERT(SUCCEEDED(hr), "Should always be able to get the outputs, even if they're empty");
			if (bt != BuildEvent && pfrsOutput->IsEmpty())
			{
				*pbrResults = NoBuild;
				goto EndBuild;
			}
			else if (!pfrsOutput->IsEmpty())
			{
				pfrsOutput->InitFrhEnum();
				frhStart = pfrsOutput->NextFrh();
				// Not sure where this should be released.  Here to be safe.
				if (NULL != frhStart)
					frhStart->ReleaseFRHRef();
			}

			bFileRecurse = FALSE;
		}
		else if (bt == BuildEvent)
		{
			CComBSTR bstrEventTool;
			switch (buildType)
			{
			case TOB_PreBuildEvent:
				bstrEventTool = szPreBuildEventToolShortName;
				break;
			case TOB_PreLinkEvent:
				bstrEventTool = szPreLinkEventToolShortName;
				break;
			case TOB_PostBuildEvent:
				bstrEventTool = szPostBuildEventToolShortName;
				break;
			case TOB_Deploy:
				bstrEventTool = szDeployToolShortName;
				break;
			case TOB_XboxImage:
				bstrEventTool = szXboxImageToolShortName;
				break;
			case TOB_XboxDeploy:
				bstrEventTool = szXboxDeployToolShortName;
				break;
			default:
				VSASSERT(FALSE, "Unknown event type in DoPreparedBuild!)");
				*pbrResults = BuildError;
				goto EndBuild;
			}

			CComPtr<IVCBuildActionList> spActions;
			cfgHolder.GetActionList(&spActions);
			long cActions = 0;
			BOOL bOK = (spActions != NULL) && SUCCEEDED(spActions->get_Count(&cActions));
			VSASSERT(bOK, "Should always be able to pick up the actions list during a build!");
			if (cActions == 0)
			{
				*pbrResults = BuildError;
				goto EndBuild;
			}

			spActions->FindByName(bstrEventTool, &spAction);
		}

		if (idx == 0 && frhStart == NULL)	// only need to do this once per build...
			RefreshCommandLineOptionsAndOutputs(pEC);

		spBuildEngineImpl->DoBuild((void *)pCfgRecord, cfgHolder.IsProject(), pEC, (void *)frhStart, aob, buildType, 
			bFileRecurse, spAction, pbrResults);

		if (idx == 0)	// only need to do this once per build...
			ClearDirtyCommandLineOptionsFlag();
	}

EndBuild:
	if (bRecurse && m_spBuildEngine)
		m_spBuildEngine->put_ReuseLogFile(VARIANT_FALSE);

	if (pEC && cFiles > 0)	// error context should know whether there were errors or not
		pEC->ActivateTaskList(VARIANT_FALSE);

	if (*pbrResults == BuildError)
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_BUILD_FAILED, IDS_ERR_BUILD_FAILED);
	
	return S_OK;
}

void CConfiguration::DoInitializeDepGraph(CBldCfgRecord** ppCfgRecord)
{
	CComPtr<IDispatch> spDispProject;
	HRESULT hr = get_Project(&spDispProject);
	VSASSERT(SUCCEEDED(hr), "Should always be able to get the project");

	CBldCfgRecord* pCfgRecord = g_StaticBuildEngine.GetCfgRecord(spDispProject, this);
	VSASSERT(pCfgRecord != NULL, "*All* configs have CfgRecords.  Probably have an out of memory situation.");
	if (pCfgRecord == NULL)
	{
		if (ppCfgRecord != NULL)
			*ppCfgRecord = NULL;
		return;	// can't do anything more
	}

	InitializeDepGraph(pCfgRecord);
	if (ppCfgRecord != NULL)
		*ppCfgRecord = pCfgRecord;
}

void CConfiguration::InitializeDepGraph(CBldCfgRecord* pCfgRecord)
{
	VSASSERT(pCfgRecord != NULL, "*All* configs have CfgRecords.  Probably have an out of memory situation.");
	if (pCfgRecord == NULL)
		return;

	CBldFileDepGraph *pDepGraph = g_StaticBuildEngine.GetDepGraph(pCfgRecord);
	if (pDepGraph == NULL)
		return;

	if (pDepGraph->IsInitialized())
		return;

	VCProject* pProject = pCfgRecord->GetVCProject();	// not ref-counted
	VSASSERT(pProject != NULL, "CfgRecord not initialized properly");
	if (pProject == NULL)
		return;

	VCConfiguration* pProjCfg = pCfgRecord->GetVCConfiguration();	// not ref-counted
	CComQIPtr<IVCBuildableItem> spBuildableItem = pProjCfg;
	VSASSERT(pProjCfg != NULL, "CfgRecord not initialized properly");
	VSASSERT(spBuildableItem != NULL, "How did we get a project cfg that was not a buildable item?");
	if (spBuildableItem == NULL)
		return;

	CComPtr<IVCBuildActionList> spCfgActions;
	HRESULT hr = spBuildableItem->get_ActionList(&spCfgActions);
	VSASSERT(SUCCEEDED(hr), "Should always be able to get the action list");
	if (spCfgActions == NULL)
	{
		VSASSERT(FALSE, "*All* buildable items have action lists.  Out of memory situation?");
		return;
	}
	InitActions(spCfgActions);

	// get the list of buildable files in the configuration
	CComPtr<IEnumVARIANT> spFiles;
	hr = CBldAction::GetFileCollection(pProjCfg, &spFiles);
	VOID_RETURN_ON_FAIL_OR_NULL(spFiles, hr);
	CVCPtrList lstFileCfgs;
	VCFileConfiguration* pFileCfg;
	while (TRUE)
	{
		CComVariant var;
		hr = spFiles->Next(1, &var, NULL);
		BREAK_ON_DONE(hr);
		if (!CBldAction::GetFileCfgForProjCfg(var, pProjCfg, &pFileCfg))
			continue;
		lstFileCfgs.AddTail(pFileCfg);
	}

	// Initialize everything...
	VCPOSITION pos = NULL;
	for (pos = lstFileCfgs.GetHeadPosition(); pos != NULL;)
	{
		VCFileConfiguration* pFileCfg = (VCFileConfiguration *)lstFileCfgs.GetNext(pos);
		CComPtr<IVCBuildActionList> spFileCfgActions;
		if (!CBldAction::GetFileCfgActions(pFileCfg, &spFileCfgActions))
			continue;

		InitActions(spFileCfgActions);
	}

	// Late bind everything...
	for (pos = lstFileCfgs.GetHeadPosition(); pos != NULL;)
	{
		VCFileConfiguration* pFileCfg = (VCFileConfiguration *)lstFileCfgs.GetNext(pos);
		CComPtr<IVCBuildActionList> spFileCfgActions;
		if (!CBldAction::GetFileCfgActions(pFileCfg, &spFileCfgActions))
			continue;

		LateBindActions(spFileCfgActions);
	}

	// Release everything...
	for (pos = lstFileCfgs.GetHeadPosition(); pos != NULL;)
	{
		VCFileConfiguration* pFileCfg = (VCFileConfiguration *)lstFileCfgs.GetNext(pos);
		pFileCfg->Release();
	}
	lstFileCfgs.RemoveAll();
	
	LateBindActions(spCfgActions);

	pDepGraph->SetGraphMode(Graph_Stable);
	pDepGraph->SetGraphInitialized();
}

void CConfiguration::InitActions(IVCBuildActionList* pBldActions)
{
	pBldActions->Reset(NULL);
	while (TRUE)
	{
		CComPtr<IVCBuildAction> spAction;
		HRESULT hr = pBldActions->Next(&spAction, NULL);
		BREAK_ON_DONE(hr);
		spAction->InitSets();
	}
}

void CConfiguration::LateBindActions(IVCBuildActionList* pBldActions)
{
	pBldActions->Reset(NULL);
	while (TRUE)
	{
		CComPtr<IVCBuildAction> spAction;
		HRESULT hr = pBldActions->Next(&spAction, NULL);
		BREAK_ON_DONE(hr);
		spAction->LateBind(m_spBuildErrorContext);
	}
}

STDMETHODIMP CConfiguration::get_ItemFileName(BSTR *pVal)
{
	if (m_pProject == NULL)
	{
		VSASSERT(FALSE, "Configuration either not initialized properly or is zombie.");
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_CFG_ZOMBIE);
	}

	return m_pProject->get_Name(pVal);
}

STDMETHODIMP CConfiguration::get_ItemFullPath(BSTR* pbstrFullPath)
{
	if (m_pProject == NULL)
	{
		VSASSERT(FALSE, "Configuration either not initialized properly or is zombie.");
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_CFG_ZOMBIE);
	}

	return m_pProject->get_ProjectFile(pbstrFullPath);
}

STDMETHODIMP CConfiguration::get_ActionList(IVCBuildActionList** ppActionList)
{
	CHECK_POINTER_NULL(ppActionList);

	if (m_spActionList == NULL)
	{
		HRESULT hr = CBldActionList::CreateInstance(&m_spActionList);
		RETURN_ON_FAIL(hr);
	}

	m_spActionList.CopyTo(ppActionList);
	return S_OK;
}

STDMETHODIMP CConfiguration::get_FileRegHandle(void** pfrh)
{
	CComQIPtr<IVCBuildableItem> spBuildableItem = m_pProject;
	if (spBuildableItem == NULL)
	{
		VSASSERT(FALSE, "Configuration either not initialized properly or is zombie.");
		return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_CFG_ZOMBIE);
	}

	return spBuildableItem->get_FileRegHandle(pfrh);
}

STDMETHODIMP CConfiguration::get_ContentList(IEnumVARIANT** ppContents)
{
	CComQIPtr<IVCBuildableItem> spBuildableProj = m_pProject;
	CHECK_ZOMBIE(spBuildableProj, IDS_ERR_CFG_ZOMBIE);

	CComPtr<IEnumVARIANT> spFileList;
	HRESULT hr = spBuildableProj->get_ContentList(&spFileList);
	RETURN_ON_FAIL_OR_NULL2(hr, spFileList, E_OUTOFMEMORY);

	m_rgFileCfgs.Clear();	// empty out anything we might have had before
	spFileList->Reset();
	while (TRUE)
	{
		CComVariant var;
		hr = spFileList->Next(1, &var, NULL);
		if (hr != S_OK)
			break;
		else if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
			continue;
		IDispatch* pDispFile = var.pdispVal;
		CComQIPtr<IVCFileImpl> spFileImpl = pDispFile;
		if (spFileImpl == NULL)
			continue;
		CComPtr<VCFileConfiguration> spFileCfg;
		hr = spFileImpl->GetFileConfigurationForProjectConfiguration(reinterpret_cast<VCConfiguration *>(this), &spFileCfg);
		if (FAILED(hr) || spFileCfg == NULL)
			continue;

		m_rgFileCfgs.Add(spFileCfg);
	}

	CComPtr<IVCCollection> spCollection;
	hr = CCollection<VCFileConfiguration>::CreateInstance(&spCollection, &m_rgFileCfgs);
	RETURN_ON_FAIL_OR_NULL2(hr, spCollection, E_OUTOFMEMORY);

	return spCollection->_NewEnum(reinterpret_cast<IUnknown **>(ppContents));
}

STDMETHODIMP CConfiguration::get_Registry(void** ppFileRegistry)
{
	CComQIPtr<IVCBuildableItem> spBuildableProject = m_pProject;
	RETURN_ON_NULL2(spBuildableProject, E_NOINTERFACE);

	return spBuildableProject->get_Registry(ppFileRegistry);
}

STDMETHODIMP CConfiguration::get_ProjectConfiguration(VCConfiguration** ppProjCfg)
{
	CHECK_POINTER_NULL(ppProjCfg);

	*ppProjCfg = this;
	(*ppProjCfg)->AddRef();
	return S_OK;
}

STDMETHODIMP CConfiguration::get_ProjectInternal(VCProject** ppProject)
{
	CHECK_POINTER_NULL(ppProject);
	*ppProject = NULL;

	CComPtr<IDispatch> spDispProject;
	HRESULT hr = get_Project(&spDispProject);
	RETURN_ON_FAIL(hr);
	CComQIPtr<VCProject> spProject = spDispProject;
	return spProject.CopyTo(ppProject);
}

STDMETHODIMP CConfiguration::get_ExistingBuildEngine(IVCBuildEngine** ppEngine)
{
	CHECK_POINTER_NULL(ppEngine);
	return m_spBuildEngine.CopyTo(ppEngine);
}

STDMETHODIMP CConfiguration::get_PersistPath(BSTR* pbstrPersistPath)
{
	CHECK_POINTER_NULL(pbstrPersistPath);

	CComPtr<IDispatch> spDispProject;
	HRESULT hr = get_Project(&spDispProject);
	CComQIPtr<IVCBuildableItem> spBldableProj = spDispProject;
	RETURN_ON_FAIL(hr);
	CHECK_ZOMBIE(spBldableProj, IDS_ERR_CFG_ZOMBIE);

	return spBldableProj->get_PersistPath(pbstrPersistPath);
}

STDMETHODIMP CConfiguration::RefreshActionOutputs(long nPropID, IVCBuildErrorContext* pEC)
{
	HRESULT hr = S_OK;
	if (m_spActionList != NULL)
	{
		hr = m_spActionList->RefreshOutputs(nPropID, pEC);
		RETURN_ON_FAIL(hr);
		if (hr == S_FALSE)
			return S_OK;
	}

	CComPtr<IEnumVARIANT> spEnum;
	if (SUCCEEDED(GetFileListEnumerator(&spEnum)))
	{
		while (TRUE)
		{
			CComPtr<IVCBuildableItem> spBldableFileCfg;
			hr = GetNextFileConfig(spEnum, &spBldableFileCfg);
			if (hr != S_OK)
				break;
			else if (spBldableFileCfg == NULL)
				continue;

			spBldableFileCfg->RefreshActionOutputs(nPropID, pEC);
		}
	}

	return S_OK;
}

HRESULT CConfiguration::StartCommandLineOptionChange(IVCBuildErrorContext* pEC)
{
	HRESULT hr = S_OK;
	if (m_spActionList)
		hr = m_spActionList->HandlePossibleCommandLineOptionsChange(pEC);

	return hr;
}

STDMETHODIMP CConfiguration::HandlePossibleCommandLineOptionsChange(IVCBuildErrorContext* pEC)
{
	HRESULT hr = StartCommandLineOptionChange(pEC);
	RETURN_ON_FAIL(hr);

	CComPtr<IEnumVARIANT> spEnum;
	if (SUCCEEDED(GetFileListEnumerator(&spEnum)))
	{
		while (TRUE)
		{
			CComPtr<IVCBuildableItem> spBldableFileCfg;
			hr = GetNextFileConfig(spEnum, &spBldableFileCfg);
			if (hr != S_OK)
				break;
			else if (spBldableFileCfg == NULL)
				continue;

			spBldableFileCfg->HandlePossibleCommandLineOptionsChange(pEC);
		}
	}

	return S_OK;
}

HRESULT CConfiguration::DirtyActionOutputsAndCommandLineOptions(long nLowPropID, long nHighPropID, long nOverride)
{
	if (nOverride >= 0)		// overrides are there only for things that affect the outputs...
	{
		SetBoolProperty(nOverride, VARIANT_TRUE);
		SetBoolProperty(VCCFGID_AnyOutputDirty, VARIANT_TRUE);
	}

	VSASSERT(nLowPropID >= 0 && nHighPropID >= nLowPropID, "Invalid PropID range!");
	if (nLowPropID < 0 || nHighPropID < nLowPropID)
		return E_UNEXPECTED;

	HRESULT hrFinal = S_FALSE;
	long lcItems = m_rgTools.GetSize();
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCToolImpl> pToolImpl;
		pToolImpl = m_rgTools.GetAt(index);
		if (pToolImpl)
		{
			HRESULT hr = pToolImpl->DirtyCommandLineOptions(nLowPropID, nHighPropID, VARIANT_TRUE);
			if (hr == S_OK)
			{
				SetBoolProperty(VCCFGID_AnyToolDirty, VARIANT_TRUE);
				if (nOverride < 0)	// we didn't already set the AnyOutputDirty flag above
				{
					VARIANT_BOOL bOutputDirty;
					if (GetBoolProperty(VCCFGID_AnyOutputDirty, &bOutputDirty) != S_OK)
					{
						if (pToolImpl->OutputsAreDirty(this, &bOutputDirty) == S_OK)
							SetBoolProperty(VCCFGID_AnyOutputDirty, VARIANT_TRUE);
					}
				}
				hrFinal = S_OK;
			}
		}
	}

	return hrFinal;
}

STDMETHODIMP CConfiguration::DirtyProp(long id)
{
	return FinishSetPropOrClear(id, GetOverrideID(id));
}

HRESULT CConfiguration::FinishSetPropOrClear(long id, long nOverride)
{
	if (PROJECT_IS_LOADED() && 
		(IsRealProp(id) || id == VCLINKID_DependentInputs || id == VCLIBID_DependentInputs || id == VCBSCID_RunBSCMakeTool))
	{
		bool bDirty = true;
		VARIANT_BOOL bFlag;
		if (id == VCLINKID_DependentInputs)
			bDirty = (DoGetBoolProperty(VCLINKID_LinkerCommandLineGenerated, FALSE, &bFlag) == S_OK);
		else if (id == VCLIBID_DependentInputs)
			bDirty = (DoGetBoolProperty(VCLIBID_LibCommandLineGenerated, FALSE, &bFlag) == S_OK);
		if (bDirty)
			DirtyActionOutputsAndCommandLineOptions(id, id, nOverride);
	}

	return S_OK;
}

HRESULT CConfiguration::DoGetStrProperty(long idProp, BOOL bSpecialPropsOnly, BSTR* pbstrValue)
{
	CHECK_POINTER_NULL(pbstrValue);
	*pbstrValue = NULL;

	HRESULT hr = S_FALSE;

	if (bSpecialPropsOnly)
	{
		if (idProp == VCCFGID_IntermediateDirectory)
			return get_IntermediateDirectory(pbstrValue);
		else if (idProp == VCCFGID_OutputDirectory)
			return get_OutputDirectory(pbstrValue);
	}

	switch (idProp)
	{
	case VCCFGID_ConfigurationName:
		return get_ConfigurationName(pbstrValue);
		break;
	case VCCFGID_PrimaryOutput:
		return get_PrimaryOutput(pbstrValue);
		break;
	case VCCFGID_ImportLibrary:
		return get_ImportLibrary(pbstrValue);
		break;
	case VCPROJID_ProjectDirectory:
		CHECK_ZOMBIE(m_pProject, IDS_ERR_CFG_ZOMBIE);
		return m_pProject->get_ProjectDirectory(pbstrValue);
		break;
	case VCPROJID_ProjectFile:
		CHECK_ZOMBIE(m_pProject, IDS_ERR_CFG_ZOMBIE);
		return m_pProject->get_ProjectFile(pbstrValue);
		break;
	case VCPROJID_Name:
		CHECK_ZOMBIE(m_pProject, IDS_ERR_CFG_ZOMBIE);
		return m_pProject->get_Name(pbstrValue);
		break;
	case VCPLATID_IncludeDirectories:
		CHECK_ZOMBIE(m_pPlatform, IDS_ERR_CFG_ZOMBIE);
		return m_pPlatform->get_IncludeDirectories(pbstrValue);
		break;
	case VCPLATID_ReferenceDirectories:
		CHECK_ZOMBIE(m_pPlatform, IDS_ERR_CFG_ZOMBIE);
		return m_pPlatform->get_ReferenceDirectories(pbstrValue);
		break;
	case VCPLATID_LibraryDirectories:
		CHECK_ZOMBIE(m_pPlatform, IDS_ERR_CFG_ZOMBIE);
		return m_pPlatform->get_LibraryDirectories(pbstrValue);
		break;
	case VCPLATID_Name:
		CHECK_ZOMBIE(m_pPlatform, IDS_ERR_CFG_ZOMBIE);
		return m_pPlatform->get_Name(pbstrValue);
		break;
	default:
		if (!bSpecialPropsOnly)
			return CStyleSheetBase::DoGetStrProperty(idProp, FALSE, pbstrValue);
	}

	return hr;
}

HRESULT CConfiguration::DoGetBoolProperty(long idProp, BOOL bCheckSpecial, VARIANT_BOOL* pbValue)
{
	if (idProp == VCCFGID_RegisterOutput)
	{
		ConfigurationTypes configType = typeApplication;
		get_ConfigurationType(&configType);
		if (configType == typeDynamicLibrary)
			return DoGetBoolProperty(VCLINKID_RegisterOutput, bCheckSpecial, pbValue);
		else
			return COptionHandlerBase::GetValueFalse(pbValue);
	}

	return CPropertyContainerImpl::DoGetBoolProperty(idProp, bCheckSpecial, pbValue);
}

STDMETHODIMP CConfiguration::ClearDirtyCommandLineOptionsFlag()
{
	VARIANT_BOOL bDirtyDuringBuild = VARIANT_FALSE;
	if (GetBoolProperty(VCCFGID_DirtyDuringBuild, &bDirtyDuringBuild) == S_OK)
	{	// if we dirtied during the build, let the build options be refreshed again next time
		Clear(VCCFGID_DirtyDuringBuild);

		// clear out the C/C++ compiler PDB file name now that we're finished with it
		CVCCLCompilerTool::ClearResolvedPdbName(this);

		return S_OK;
	}
	Clear(VCCFGID_AllOutputsDirty);
	Clear(VCCFGID_PrimaryOutputDirty);
	Clear(VCCFGID_AnyOutputDirty);
	Clear(VCCFGID_AnyToolDirty);

	HRESULT hrFinal = S_FALSE;
	long lcItems = m_rgTools.GetSize();
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCToolImpl> pToolImpl;
		pToolImpl = m_rgTools.GetAt(index);
		if (pToolImpl)
		{
			HRESULT hr = pToolImpl->DirtyCommandLineOptions(-1, -1, VARIANT_FALSE);
			if (hr == S_OK)
				hrFinal = S_OK;
		}
	}

	// clear out the C/C++ compiler PDB file name now that we're finished with it
	CVCCLCompilerTool::ClearResolvedPdbName(this);

	return hrFinal;
}

HRESULT CConfiguration::RefreshCommandLineOptionsAndOutputs(IVCBuildErrorContext* pEC)
{
	// set up the C/C++ compiler PDB file name up front
	CVCCLCompilerTool::RefreshResolvedPdbName(this, TRUE);

	VARIANT_BOOL bRefreshOutputs = VARIANT_FALSE;
	long nOutputID = -1;
	BOOL bDoRefresh = TRUE;
	BOOL bDoFileRefresh = FALSE;
	if (GetBoolProperty(VCCFGID_AllOutputsDirty, &bRefreshOutputs) == S_OK)
		nOutputID = 0;
	else if (GetBoolProperty(VCCFGID_PrimaryOutputDirty, &bRefreshOutputs) == S_OK)
		nOutputID = VCCFGID_PrimaryOutputDirty;
	else if (GetBoolProperty(VCBSCID_OutputsDirty, &bRefreshOutputs) == S_OK)
		nOutputID = VCBSCID_OutputsDirty;
	else if (GetBoolProperty(VCCUSTID_OutputsDirty, &bRefreshOutputs) == S_OK)
		nOutputID = VCCUSTID_OutputsDirty;
	else
		bDoRefresh = FALSE;

	bDoFileRefresh = bDoRefresh;
	if (GetBoolProperty(VCCFGID_AnyOutputDirty, &bRefreshOutputs) == S_OK)
		bDoFileRefresh = TRUE;

	VARIANT_BOOL bToolDirty = VARIANT_FALSE;
	BOOL bDoToolStuff = (GetBoolProperty(VCCFGID_AnyToolDirty, &bToolDirty) == S_OK);

	if (!bDoRefresh && !bDoFileRefresh && !bDoToolStuff)
		return S_FALSE;

	HRESULT hr = S_FALSE;
	if (m_spActionList != NULL)
	{
		if (bDoRefresh && m_spActionList != NULL)
		{
			hr = m_spActionList->RefreshOutputs(nOutputID, m_spBuildErrorContext);
			RETURN_ON_FAIL(hr);
		}
		if (bDoToolStuff)
		{
			hr = StartCommandLineOptionChange(pEC);
			RETURN_ON_FAIL(hr);
		}
	}

	CComPtr<IEnumVARIANT> spEnum;
	if (FAILED(GetFileListEnumerator(&spEnum)))
		return S_FALSE;

	while (TRUE)
	{
		CComPtr<IVCBuildableItem> spBldableFileCfg;
		hr = GetNextFileConfig(spEnum, &spBldableFileCfg);
		if (hr != S_OK)
			break;
		else if (spBldableFileCfg == NULL)
			continue;

		if (bDoRefresh)
			spBldableFileCfg->RefreshActionOutputs(nOutputID, m_spBuildErrorContext);

		if (bDoFileRefresh && nOutputID != 0)	// only some files need something refreshed...
		{
			CComQIPtr<VCFileConfiguration> spFileCfg = spBldableFileCfg;
			CComQIPtr<IVCPropertyContainer> spPropContainer = spFileCfg;
			if (spFileCfg && spPropContainer)
			{
				CComPtr<IDispatch> spTool;
				spFileCfg->get_Tool(&spTool);
				CComQIPtr<IVCToolImpl> spToolImpl = spTool;
				if (spToolImpl)
				{
					VARIANT_BOOL bOutputDirty;
					if (spToolImpl->OutputsAreDirty(spPropContainer, &bOutputDirty) == S_OK)
						spBldableFileCfg->RefreshActionOutputs(0, m_spBuildErrorContext);
				}
			}
		}
		if (bDoToolStuff)
			spBldableFileCfg->HandlePossibleCommandLineOptionsChange(pEC);
	}

	return S_OK;
}

// WARNING: Clone must only be called on a brand new config!!
STDMETHODIMP CConfiguration::Clone( IVCPropertyContainer *pSource )
{
	// check arg
	CHECK_READ_POINTER_NULL(pSource);

	ConfigurationTypes newConfigType = typeApplication;
	pSource->GetIntProperty(VCCFGID_ConfigurationType, (long *)&newConfigType);
	DoSetConfigurationType(newConfigType);

	{	// pretend we're in project load for the moment
		// this keeps us from firing all kinds of prop changed until we're ready
		CInLoadProject loadProj;
		CConfiguration* pIn = (CConfiguration*)pSource;

		// get the start of the map
		VCPOSITION iter = pIn->m_PropMap.GetStartPosition();
		// iterate over each item in the map
		while( iter )
		{
			long key;
			CComVariant value;

			pIn->m_PropMap.GetNextAssoc( iter, key, value );

			switch (key)
			{
			case VCCFGID_IntrinsicAppliedStyleSheets:	// let these get set properly elsewhere
			case VCCFGID_ConfigurationType:				// already set
				break;
		
			default:
				// copy this element
				DoSetProp(key, TRUE, value);	// allow side effects to happen...
			}
		}
	}

	return S_OK;
}

HRESULT CConfiguration::GetFileListEnumerator(IEnumVARIANT** ppFileList)
{
	CHECK_POINTER_NULL(ppFileList);
	CHECK_ZOMBIE(m_pProject, IDS_ERR_CFG_ZOMBIE);

	CComPtr<IDispatch> spDispFiles;
	HRESULT hr = m_pProject->get_Files(&spDispFiles);
	RETURN_ON_FAIL(hr);

	CComQIPtr<IVCCollection> spFiles = spDispFiles;
	if (spFiles != NULL)
	{
		CComPtr<IEnumVARIANT> spEnum;
		if (SUCCEEDED(spFiles->_NewEnum(reinterpret_cast<IUnknown **>(ppFileList))) && *ppFileList != NULL)
		{
			(*ppFileList)->Reset();
			return S_OK;
		}
	}

	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_CFG_ZOMBIE);
}

HRESULT CConfiguration::GetNextFileConfig(IEnumVARIANT* pEnum, VCFileConfiguration** ppFileCfg)
{
	VSASSERT(pEnum != NULL, "Bad input parameter!");
	CHECK_POINTER_NULL(ppFileCfg);
	CHECK_READ_POINTER_NULL(pEnum);
	*ppFileCfg = NULL;

	CComVariant var;
	HRESULT hr = pEnum->Next(1, &var, NULL);
	if (hr != S_OK)
		return hr;
	if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
		return hr;
	IDispatch* pDispFile = var.pdispVal;
	CComQIPtr<IVCFileImpl> spFileImpl = pDispFile;
	RETURN_ON_NULL2(spFileImpl, hr);
	return spFileImpl->GetFileConfigurationForProjectConfiguration(this, ppFileCfg);
}

HRESULT CConfiguration::GetNextFileConfig(IEnumVARIANT* pEnum, IVCBuildableItem** ppBldableFileCfg)
{
	CHECK_POINTER_NULL(ppBldableFileCfg);
	*ppBldableFileCfg = NULL;

	CComPtr<VCFileConfiguration> spFileCfg;
	HRESULT hr = GetNextFileConfig(pEnum, &spFileCfg);
	CComQIPtr<IVCBuildableItem> spBldableFileCfg = spFileCfg;
	spBldableFileCfg.CopyTo(ppBldableFileCfg);
	return hr;
}

STDMETHODIMP CConfiguration::AddTool(LPCOLESTR szName, IDispatch** ppTool)
{
	// DIANEME_TODO: need to handle add of tool AFTER project is loaded...
	HRESULT hr = DoAddTool(szName, ppTool);
	CComQIPtr<IVCToolImpl> spVCTool = *ppTool;
	RETURN_ON_FAIL_OR_NULL(hr, spVCTool);
	InitToolExtensionListForOneTool(spVCTool);
	return hr;
}

/* static */ 
HRESULT CConfiguration::SGetConfigurationType(ConfigurationTypes* pconfigType)
{
	CHECK_POINTER_NULL(pconfigType);
	*pconfigType = typeApplication;	// call it application unless told otherwise...
	return S_FALSE;		// all defaults should return S_FALSE
}

HRESULT CBldDummyGenCfg::CreateInstance(CBldDummyGenCfg** ppGenCfg, VCConfiguration *pCfg)
{
	CComObject<CBldDummyGenCfg> *pGenCfgObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CBldDummyGenCfg>::CreateInstance(&pGenCfgObj);
	if (SUCCEEDED(hr))
	{
		pGenCfgObj->Initialize(pCfg);
		IVCGenericConfiguration* pVCCfg = pGenCfgObj;
		VSASSERT(pVCCfg != NULL, "Must be out of memory situation.");
		if (pVCCfg)
			pVCCfg->AddRef();
		*ppGenCfg = pGenCfgObj;
	}
	return hr;

}

void CBldDummyGenCfg::Initialize(VCConfiguration *pCfg)
{
	m_spConfig = pCfg;
}

STDMETHODIMP CBldDummyGenCfg::InformBuildEnd(BOOL fOK)
{
	m_spBuildThread->ExitThread();
	m_spBuildThread.Release();
	m_spBuildEngine.Release();
	Release();	// release us, too

	return S_OK;	// return code not checked
}

STDMETHODIMP CBldDummyGenCfg::InformBuildBegin()
{
	// DIANEME_TODO: CBldDummyGenCfg::InformBuildBegin -- do something
	return S_OK;	// return code not checked
}

STDMETHODIMP CBldDummyGenCfg::StartAutomationBuild(bldActionTypes bldType)
{
	VSASSERT(FALSE, "Automation builds are for the UI version of GenCfg, not command line version.");	// shouldn't be here!!
	return E_NOTIMPL;
}

STDMETHODIMP CBldDummyGenCfg::StartFileBuild(ULONG celtFiles, VCFile* pFiles[])
{
	VSASSERT(FALSE, "Automation builds are for the UI version of GenCfg, not command line version.");	// shouldn't be here!!
	return E_NOTIMPL;
}

STDMETHODIMP CBldDummyGenCfg::SupportsBuildType(bldActionTypes buildType, VARIANT_BOOL* pbSupports, VARIANT_BOOL* pbAvailable)
{
	CHECK_POINTER_NULL(pbSupports);
	CHECK_POINTER_NULL(pbAvailable);
	*pbSupports = VARIANT_FALSE;
	*pbAvailable = VARIANT_FALSE;

	CComQIPtr<IVCConfigurationImpl> spCfgImpl = m_spConfig;
	CHECK_ZOMBIE(spCfgImpl, IDS_ERR_CFG_ZOMBIE);

	return spCfgImpl->SupportsBuildType(buildType, pbSupports, pbAvailable);
}

STDMETHODIMP CBldDummyGenCfg::get_NoBuildIsInProgress(BOOL* pbNoneInProgress)
{
	HRESULT hr = (m_spBuildEngine == NULL) ? S_OK : E_FAIL;
	if (pbNoneInProgress && !IsBadWritePtr(pbNoneInProgress, sizeof(pbNoneInProgress)))
		*pbNoneInProgress = SUCCEEDED(hr);

	return hr;
}

HRESULT CBldDummyGenCfg::DoStartBuild(bldActionTypes bldType, VCFile* pFiles[] /* = NULL */, ULONG celtFiles /* = 0 */)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);

	VARIANT_BOOL bBuildable = VARIANT_FALSE;
	VARIANT_BOOL bValid = VARIANT_FALSE;
	CComQIPtr <IVCConfigurationImpl> pConfigImpl;
	pConfigImpl = m_spConfig;

	if (FAILED(pConfigImpl->get_IsBuildable(&bBuildable)) || (bBuildable == VARIANT_FALSE))
		 return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_NOT_BUILDABLE, IDS_ERR_PROJ_NOT_BUILDABLE);
	else if (FAILED(pConfigImpl->get_IsValidConfiguration(&bValid)) || (bValid == VARIANT_FALSE))
		 return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_NOT_BUILDABLE, IDS_ERR_PROJ_NOT_BUILDABLE);

	if (FAILED(CDynamicBuildEngine::CreateInstance(&m_spBuildEngine)) || m_spBuildEngine == NULL)
	{
		VSASSERT(FALSE, "No build engine == out of memory");
		return E_OUTOFMEMORY;
	}
	CComQIPtr<IVCBuildEngineImpl> spBuildEngineImpl = m_spBuildEngine;
	VSASSERT(spBuildEngineImpl != NULL, "How did we create a build engine without the IVCBuildEngineImpl interface?!?");
	RETURN_ON_NULL2(spBuildEngineImpl, E_UNEXPECTED);

	HRESULT hr = S_OK;
	if (m_spBuildThread == NULL)
	{
		hr = spBuildEngineImpl->get_BuildThread(&m_spBuildThread);
		RETURN_ON_FAIL_OR_NULL2(hr, m_spBuildThread, E_OUTOFMEMORY);
	}

	CComPtr<IVCBuildErrorContext> spErrorContext;
	hr = CBldCommandLineEC::CreateInstance(&spErrorContext, NULL, m_spBuildEngine);
	VSASSERT(SUCCEEDED(hr), "out of memory?");

	hr = m_spBuildThread->InitializeForBuild(this, m_spConfig, celtFiles, pFiles, bldType, (pFiles == NULL), 
		TRUE /* provide banner */, m_spBuildEngine, spErrorContext);

	if (SUCCEEDED(hr))
		hr = m_spBuildThread->StartThread();
	
	return hr;
}
        

//---------------------------------------------------------------------------
// interface: ISpecifyPropertyPages
//---------------------------------------------------------------------------
STDMETHODIMP CConfiguration::GetPages(/* [out] */ CAUUID *pPages)
{
	CHECK_POINTER_NULL(pPages);

#ifdef AARDVARK
	bool bCPP = false;
	bool bRC = false;
	bool bIDL = false;
	bool bCSharp = false;
	bool bVB = false;
	bool bSDL = false;
	// get the list of files in the config
	CComPtr<IDispatch> pDispColl;
	m_pProject->get_Files( &pDispColl );
	CComQIPtr<IVCCollection> pColl = pDispColl;
	long numFiles = 0;
	pColl->get_Count( &numFiles );
	for( long i = 1; i <= numFiles; i++ )
	{
		CComPtr<IDispatch> pDisp;
		pColl->Item( CComVariant( i ), &pDisp );
		CComQIPtr<VCFile> pFile = pDisp;
		CComBSTR bstrExt;
		pFile->get_Extension( &bstrExt );
		// cpp,cxx,c
		if( !bCPP )
			if( _wcsicmp( bstrExt, L".cpp" ) == 0 || _wcsicmp( bstrExt, L".cxx" ) == 0 || _wcsicmp( bstrExt, L".c" ) == 0 )
				bCPP = true;
		// rc
		if( !bRC )
			if( _wcsicmp( bstrExt, L".rc" ) == 0 )
				bRC = true;
		// idl
		if( !bIDL )
			if( _wcsicmp( bstrExt, L".idl" ) == 0 )
				bIDL = true;
		// cs
		if( !bCSharp )
			if( _wcsicmp( bstrExt, L".cs" ) == 0 )
				bCSharp = true;
		// vb
		if( !bVB )
			if( _wcsicmp( bstrExt, L".vb" ) == 0 )
				bVB = true;
		// sdl,wsdl
		if( !bSDL )
			if( _wcsicmp( bstrExt, L".sdl" ) == 0 || _wcsicmp( bstrExt, L".wsdl" ) == 0 )
				bSDL = true;
	}
	//
#endif // AARDVARK
	
	// Get the list of pages
	long lPageCnt = 0;
	CAUUID aPageSet[1024] = {0};
	unsigned long lTools = 0;

	BOOL bUtility = FALSE;

	ConfigurationTypes configType;
	HRESULT hr = get_ConfigurationType(&configType);
	if( (hr == S_OK) && configType == typeGeneric )
		bUtility = TRUE;

	long lcItems = m_rgTools.GetSize();
	for (long index = 0; index < lcItems; index++)
	{
		CComPtr<IVCToolImpl> spToolImpl = m_rgTools.GetAt(index);
		if (spToolImpl == NULL)
			continue;

		long tool_index;

		spToolImpl->get_ToolDisplayIndex( &tool_index );
		if( bUtility  && tool_index == TOOL_DISPLAY_INDEX_CUSTBLD )
			continue;

#ifdef AARDVARK
		if( tool_index == TOOL_DISPLAY_INDEX_CPP && !bCPP )
			continue;
		if( tool_index == TOOL_DISPLAY_INDEX_BSC && !bCPP )
			continue;
		if( tool_index == TOOL_DISPLAY_INDEX_LINK && !bCPP )
			continue;
		if( tool_index == TOOL_DISPLAY_INDEX_LIB && !bCPP )
			continue;
		if( tool_index == TOOL_DISPLAY_INDEX_RC && !bRC )
			continue;
		if( tool_index == TOOL_DISPLAY_INDEX_MIDL && !bIDL )
			continue;
		if( tool_index == TOOL_DISPLAY_INDEX_WEB && !bSDL )
			continue;
		if( tool_index == TOOL_DISPLAY_INDEX_CSHARP && !bCSharp )
			continue;
		if( tool_index == TOOL_DISPLAY_INDEX_VB && !bVB )
			continue;
#endif // AARDVARKk

		HRESULT hr = spToolImpl->GetPages(&(aPageSet[tool_index]));

		if( SUCCEEDED(hr) && aPageSet[tool_index].cElems )
		{
			lPageCnt += aPageSet[tool_index].cElems;
			lTools++;
			if( lTools > 1024 )
				break;
		}
	}

	// number of tool pages plus number of 'extra' pages
	pPages->cElems = lPageCnt + 2;
	if( lPageCnt == 0 )
	{
		pPages->pElems = (GUID*) NULL;
		return S_OK;
	}

	// number of tool pages plus number of 'extra' pages
	pPages->pElems = (GUID*) CoTaskMemAlloc(sizeof(CLSID)*(lPageCnt+2));
	RETURN_ON_NULL2(pPages->pElems, E_OUTOFMEMORY);
		
	// add the general->configuration page
	pPages->pElems[0] = __uuidof(GeneralConfigSettingsPage);

	// add the debugger
	pPages->pElems[1] = __uuidof(DebugSettingsPage);

	// Merge the page lists
	unsigned long lPages = 2;
	// for each possible tool
	for( unsigned long i = 0; i < 1024; i++ )
	{
		// for each page this tool has
		for( unsigned long j = 0; j < aPageSet[i].cElems; j++ )
		{
			pPages->pElems[lPages] = aPageSet[i].pElems[j];
			lPages++;
		}
	}

	return S_OK;
}

STDMETHODIMP CConfiguration::CreatePageObject(IUnknown **ppUnk, CLSID*pCLSID, IVCPropertyContainer *pPropCntr, IVCSettingsPage *pPage )
{
	if( __uuidof(GeneralConfigSettingsPage) == *pCLSID )
	{
		// TODO: Choose nmake page or normal page here.
		return S_OK;
	}
	else if( CLSID_DebugSettingsPage == *pCLSID )
	{
		CPageObjectImpl< CDebugSettingsPage, VCDEBUG_MIN_DISPID, VCDEBUG_MAX_DISPID >::CreateInstance(ppUnk, pPropCntr, pPage);
		return S_OK;
	}
	else
	{
		// not a local one, try the tools.
		long lcItems = m_rgTools.GetSize();
		for (long index = 0; index < lcItems; index++)
		{
			CComPtr<IVCToolImpl> spToolImpl = m_rgTools.GetAt(index);
			if (spToolImpl == NULL)
				continue;
	
			HRESULT hr = spToolImpl->CreatePageObject(ppUnk, pCLSID, pPropCntr, pPage );
			if( hr == S_OK )
				return S_OK;
		}
		return E_FAIL;
	}
}

STDMETHODIMP CConfiguration::CreatesUniqueOutputName(IUnknown* pItem, VARIANT_BOOL* pbUnique)	// will this item have a unique output name?
{
	CHECK_READ_POINTER_NULL(pItem);
	CHECK_POINTER_NULL(pbUnique);
	*pbUnique = VARIANT_TRUE;

	RETURN_ON_NULL2(m_pProject, S_OK);		// cannot do any checking if no project...

	CComQIPtr<IVCFileConfigurationImpl> spNewFileCfgImpl = pItem;
	RETURN_ON_NULL2(spNewFileCfgImpl, S_OK);

	CComBSTR bstrOutputFile;
	if (spNewFileCfgImpl->get_OutputFile(&bstrOutputFile) != S_OK || bstrOutputFile.Length() == 0)
		return S_OK;
	CStringW strOutputFile = bstrOutputFile;

	// actually GOT an output file, so have to check through all the other file configs to see if it matches anything else
	CComPtr<IDispatch> spDispFiles;
	if (m_pProject->get_Files(&spDispFiles) != S_OK)
		return S_OK;
	CComQIPtr<IVCCollection> spFiles = spDispFiles;

	CComPtr<IEnumVARIANT> spEnum;
	spFiles->_NewEnum(reinterpret_cast<IUnknown **>(&spEnum));
	RETURN_ON_NULL2(spEnum, S_OK);

	spEnum->Reset();
	while (TRUE)
	{
		CComVariant var;
		HRESULT hr = spEnum->Next(1, &var, NULL);
		if (hr != S_OK)
			break;
		if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
			continue;

		// get the file impl
		CComQIPtr<IVCFileImpl> spFileImpl = var.pdispVal;
		if (spFileImpl == NULL)
			continue;

		// get the file config
		CComPtr<VCFileConfiguration> spFileCfg;
		hr = spFileImpl->GetFileConfigurationForProjectConfiguration((VCConfiguration*)this, &spFileCfg);
		if (spFileCfg == NULL)
			continue;
		// get the file config impl
		CComQIPtr<IVCFileConfigurationImpl> spFileCfgImpl = spFileCfg;
		if (spFileCfgImpl == NULL)
			continue;

		if (spFileCfgImpl == spNewFileCfgImpl)
			continue;	// skip the current element

		CComBSTR bstrOut;
		hr = spFileCfgImpl->get_OutputFile(&bstrOut);
		if (hr != S_OK)
			continue;

		CStringW strOut = bstrOut;
		if (strOutputFile == strOut)
		{
			*pbUnique = VARIANT_FALSE;
			return S_FALSE;
		}
	}

	return S_OK;	// no match
}


#ifndef _SHIP
HRESULT CConfiguration::ForceDirty()
{
	CComQIPtr<IVCProjectImpl> spProjectImpl = m_pProject;
	if (spProjectImpl)
		return spProjectImpl->put_IsConverted(VARIANT_TRUE);

	return CVCProjectEngine::DoSetErrorInfo(VCPROJ_E_ZOMBIE, IDS_ERR_CFG_ZOMBIE);
}
#endif	// _SHIP

STDMETHODIMP CVCProjConfigSettingsPage::Commit()
{
	// we need to change the CRT switch based on the combination of Use of MFC and ATL Minimize CRT Usage
	CComVariant varNewUseMFC;
	BOOL bChangedUseOfMFC = (GetLocalProp(VCCFGID_UseOfMFC, &varNewUseMFC) == S_OK);
	CComVariant varNewATLNoCRT;
	BOOL bChangedATLNoCRT = (GetLocalProp(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, &varNewATLNoCRT) == S_OK);
	if (!bChangedUseOfMFC && !bChangedATLNoCRT)	// neither of these changed
		return CVCConfigSettingsPageBase<CVCProjConfigSettingsPage, IVCProjectConfigurationSettings>::Commit();

	// figure out Use of MFC (old, new, current)
	CComVariant varOldUseMFC;
	useOfMfc oldUseOfMFC = useMfcStdWin;
	useOfMfc currentUseOfMFC = useMfcStdWin;
	if (GetParentProp(VCCFGID_UseOfMFC, VARIANT_TRUE /* allow inherit */, &varOldUseMFC) == S_OK)
		oldUseOfMFC = (useOfMfc)varOldUseMFC.lVal;
	if (bChangedUseOfMFC)
		currentUseOfMFC = (useOfMfc)varNewUseMFC.lVal;
	else
		currentUseOfMFC = oldUseOfMFC;

	// figure out ATL Minimize CRT Usage (old, new, current)
	CComVariant varOldATLNoCRT;
	VARIANT_BOOL bOldATLNoCRT = VARIANT_FALSE;
	if (GetParentProp(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, VARIANT_TRUE /* allow inherit */, &varOldATLNoCRT) == S_OK)
		bOldATLNoCRT = varOldATLNoCRT.boolVal;
	VARIANT_BOOL bCurrentATLNoCRT;
	if (bChangedATLNoCRT)
		bCurrentATLNoCRT = varNewATLNoCRT.boolVal;
	else
		bCurrentATLNoCRT = bOldATLNoCRT;

	HRESULT hr = S_OK;
	// Use of MFC takes precedence on how the RuntimeLibrary switch is set
	if (currentUseOfMFC == (long)useMfcStatic || currentUseOfMFC == (long)useMfcDynamic)
	{
		if (bChangedUseOfMFC && (useOfMfc)varNewUseMFC.lVal != oldUseOfMFC)
		{
			CComVariant varRunTimeLib;
			runtimeLibraryOption rtOldOption = rtSingleThreaded;
			if (GetProp(VCCLID_RuntimeLibrary, &varRunTimeLib) == S_OK)
				rtOldOption = (runtimeLibraryOption)varRunTimeLib.lVal;
	
			BOOL bIsDebug = (rtOldOption == rtMultiThreadedDebug || rtOldOption == rtMultiThreadedDebugDLL 
				|| rtOldOption == rtSingleThreadedDebug);

			runtimeLibraryOption rtNewOption;
			if ((useOfMfc)varNewUseMFC.lVal == useMfcStatic)
				rtNewOption = bIsDebug ? rtMultiThreadedDebug : rtMultiThreaded;

			else	// useMfcDynamic
				rtNewOption = bIsDebug ? rtMultiThreadedDebugDLL : rtMultiThreadedDLL;

			if (rtOldOption != rtNewOption)
			{
				CComVariant var = rtNewOption;
				SetProp(VCCLID_RuntimeLibrary, var);
				hr = CommitSingleProp(VCCLID_RuntimeLibrary);
				RETURN_ON_FAIL(hr);
			}
		}
	}

	else if (bCurrentATLNoCRT == VARIANT_TRUE)
	{
		if (bChangedATLNoCRT && varNewATLNoCRT.boolVal == VARIANT_TRUE && bOldATLNoCRT != VARIANT_TRUE)
		{
			CComVariant varRunTimeLib;
			runtimeLibraryOption rtOldOption = rtSingleThreaded;
			if (GetProp(VCCLID_RuntimeLibrary, &varRunTimeLib) == S_OK)
				rtOldOption = (runtimeLibraryOption)varRunTimeLib.lVal;
	
			BOOL bIsDebug = (rtOldOption == rtMultiThreadedDebug || rtOldOption == rtMultiThreadedDebugDLL 
				|| rtOldOption == rtSingleThreadedDebug);

			runtimeLibraryOption rtNewOption = bIsDebug ? rtMultiThreadedDebug : rtMultiThreaded;
			if (rtOldOption != rtNewOption)
			{
				CComVariant var = rtNewOption;
				SetProp(VCCLID_RuntimeLibrary, var);
				hr = CommitSingleProp(VCCLID_RuntimeLibrary);
				RETURN_ON_FAIL(hr);
			}
		}
	}

	return CVCConfigSettingsPageBase<CVCProjConfigSettingsPage, IVCProjectConfigurationSettings>::Commit();
}

void CVCProjConfigSettingsPage::GetBaseDefault(long id, CComVariant& varValue)
{
	if (id == VCCFGID_DeleteExtensionsOnClean)
	{
		CComBSTR bstrVal;
		CConfiguration::GetDefaultValue(VCCFGID_DeleteExtensionsOnClean, &bstrVal, this);
		varValue = bstrVal; 
	}
	else
		CBase::GetBaseDefault(id, varValue);
}

STDMETHODIMP CVCProjConfigSettingsPage::get_IntermediateDirectory(BSTR *pVal)
{
	HRESULT hr = GetStrProperty(VCCFGID_IntermediateDirectory, pVal);
	if (hr == S_FALSE)
		COptionHandlerBase::GetDefaultString(pVal);
	return hr;
}

STDMETHODIMP CVCProjConfigSettingsPage::put_IntermediateDirectory(BSTR newVal)
{
	return SetStrProperty(VCCFGID_IntermediateDirectory, newVal);
}

STDMETHODIMP CVCProjConfigSettingsPage::get_OutputDirectory(BSTR *pVal)
{
	HRESULT hr = GetStrProperty(VCCFGID_OutputDirectory, pVal);
	if (hr == S_FALSE)
		COptionHandlerBase::GetDefaultString(pVal);
	return hr;
}

STDMETHODIMP CVCProjConfigSettingsPage::put_OutputDirectory(BSTR newVal)
{
	return SetStrProperty(VCCFGID_OutputDirectory, newVal);
}

STDMETHODIMP CVCProjConfigSettingsPage::get_ConfigurationType(ConfigurationTypes* pconfigType)
{
	HRESULT hr = GetIntProperty(VCCFGID_ConfigurationType, (long *)pconfigType);
	if (hr == S_FALSE)
		CConfiguration::SGetConfigurationType(pconfigType);
	return hr;
}

STDMETHODIMP CVCProjConfigSettingsPage::put_ConfigurationType(ConfigurationTypes configType)
{
	return SetIntProperty(VCCFGID_ConfigurationType, configType);
}

STDMETHODIMP CVCProjConfigSettingsPage::get_BuildBrowserInformation(VARIANT_BOOL* pbBsc)	// build .bsc file
{
	HRESULT hr = GetBoolProperty(VCCFGID_BuildBrowserInformation, pbBsc);
	if (hr == S_FALSE)
		CConfiguration::GetDefaultValue(VCCFGID_BuildBrowserInformation, pbBsc, this);
	return hr;
}

STDMETHODIMP CVCProjConfigSettingsPage::put_BuildBrowserInformation(VARIANT_BOOL bBsc)
{
	return SetBoolProperty(VCCFGID_BuildBrowserInformation, bBsc);
}

STDMETHODIMP CVCProjConfigSettingsPage::get_UseOfMFC(useOfMfc* puseMfc)	// use of MFC
{
	HRESULT hr = GetIntProperty(VCCFGID_UseOfMFC, (long *)puseMfc);
	if (hr == S_FALSE)
		CConfiguration::GetDefaultValue(VCCFGID_UseOfMFC, (long *)puseMfc, this);
	return hr;
}

STDMETHODIMP CVCProjConfigSettingsPage::put_UseOfMFC(useOfMfc useMfc)
{
	return SetIntProperty(VCCFGID_UseOfMFC, useMfc);
}

STDMETHODIMP CVCProjConfigSettingsPage::get_UseOfATL(useOfATL* puseATL)
{	// how are we using ATL?
	CHECK_POINTER_VALID(puseATL);
	HRESULT hr = GetIntProperty(VCCFGID_UseOfATL, (long *)puseATL);
	if (hr == S_FALSE)
		CConfiguration::GetDefaultValue(VCCFGID_UseOfATL, (long *)puseATL, this);
	return hr;
}

STDMETHODIMP CVCProjConfigSettingsPage::put_UseOfATL(useOfATL useATL)
{
	return SetIntProperty(VCCFGID_UseOfATL, useATL);
}

STDMETHODIMP CVCProjConfigSettingsPage::get_ATLMinimizesCRunTimeLibraryUsage(VARIANT_BOOL* pbCRT)
{	// is this an ATL min dependency?
	CHECK_POINTER_VALID(pbCRT);
	HRESULT hr = GetBoolProperty(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, pbCRT);
	if (hr == S_FALSE)
		CConfiguration::GetDefaultValue(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, pbCRT, this);
	return hr;
}

STDMETHODIMP CVCProjConfigSettingsPage::put_ATLMinimizesCRunTimeLibraryUsage(VARIANT_BOOL bCRT)
{
	return SetBoolProperty(VCCFGID_ATLMinimizesCRunTimeLibraryUsage, bCRT);
}

STDMETHODIMP CVCProjConfigSettingsPage::get_CharacterSet(charSet* pset)
{	// what type of character set are we using?
	CHECK_POINTER_VALID(pset);
	HRESULT hr = GetIntProperty(VCCFGID_CharacterSet, (long *)pset);
	if (hr == S_FALSE)
		CConfiguration::GetDefaultValue(VCCFGID_CharacterSet, (long *)pset, this);
	return hr;
}

STDMETHODIMP CVCProjConfigSettingsPage::put_CharacterSet(charSet set)
{
	return SetIntProperty(VCCFGID_CharacterSet, set);
}

STDMETHODIMP CVCProjConfigSettingsPage::get_ManagedExtensions(VARIANT_BOOL* pbManaged)	// is this a managed extensions project?
{
	HRESULT hr = GetBoolProperty(VCCFGID_ManagedExtensions, pbManaged);
	if (hr == S_FALSE)
		CConfiguration::GetDefaultValue(VCCFGID_ManagedExtensions, pbManaged, this);
	return hr;
}

STDMETHODIMP CVCProjConfigSettingsPage::put_ManagedExtensions(VARIANT_BOOL bManaged)
{
	return SetBoolProperty(VCCFGID_ManagedExtensions, bManaged);
}

STDMETHODIMP CVCProjConfigSettingsPage::get_DeleteExtensionsOnClean(BSTR* pbstrExt)
{	// wildcard extension list to delete on clean/rebuild from within the intermediate directory
	HRESULT hr = GetStrProperty(VCCFGID_DeleteExtensionsOnClean, pbstrExt);
	if (hr == S_FALSE)
		CConfiguration::GetDefaultValue(VCCFGID_DeleteExtensionsOnClean, pbstrExt, this);
	return hr;
}

STDMETHODIMP CVCProjConfigSettingsPage::put_DeleteExtensionsOnClean(BSTR bstrExt)
{
	return SetStrProperty(VCCFGID_DeleteExtensionsOnClean, bstrExt);
}

STDMETHODIMP CVCProjConfigSettingsPage::get_WholeProgramOptimization(VARIANT_BOOL* pbOptimized)
{	// perform whole program optimization on build more or less requires optimization turned on to be effective
	return GetBoolProperty(VCCFGID_WholeProgramOptimization, pbOptimized);
}

STDMETHODIMP CVCProjConfigSettingsPage::put_WholeProgramOptimization(VARIANT_BOOL bOptimize)
{
	return SetBoolProperty(VCCFGID_WholeProgramOptimization, bOptimize);
}

STDMETHODIMP CVCProjConfigSettingsPage::MapPropertyToCategory( DISPID dispid, PROPCAT* ppropcat)
{
	switch( dispid )
	{
		case VCCFGID_OutputDirectory:
		case VCCFGID_IntermediateDirectory:
		case VCCFGID_DeleteExtensionsOnClean:
			*ppropcat = IDS_GENERALCAT;
			break;
		case VCCFGID_ConfigurationType:
		case VCCFGID_BuildBrowserInformation:
		case VCCFGID_UseOfMFC:
		case VCCFGID_UseOfATL:
		case VCCFGID_ATLMinimizesCRunTimeLibraryUsage:
		case VCCFGID_CharacterSet:
		case VCCFGID_ManagedExtensions:
		case VCCFGID_WholeProgramOptimization:
			*ppropcat = IDS_PROJDEFAULTSCAT;
			break;
		default:
			*ppropcat = 0;
			break;
	}
	return S_OK;
}

STDMETHODIMP CVCProjConfigSettingsPage::GetCategoryName( PROPCAT propcat, LCID lcid, BSTR* pbstrName)
{
	CComBSTR bstrCat;
	bstrCat.LoadString(propcat);
	return bstrCat.CopyTo(pbstrName);
}

STDMETHODIMP CVCGeneralMakefileSettingsPage::get_ConfigurationType(ConfigurationTypes* pconfigType)
{
	HRESULT hr = GetIntProperty(VCCFGID_ConfigurationType, (long *)pconfigType);
	if (hr == S_FALSE)
		CConfiguration::SGetConfigurationType(pconfigType);
	return hr;
}

STDMETHODIMP CVCGeneralMakefileSettingsPage::put_ConfigurationType(ConfigurationTypes configType)
{
	return SetIntProperty(VCCFGID_ConfigurationType, configType);
}
