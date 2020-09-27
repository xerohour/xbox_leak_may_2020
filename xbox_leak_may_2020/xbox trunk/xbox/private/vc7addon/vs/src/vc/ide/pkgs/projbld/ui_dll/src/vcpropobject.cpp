#include "stdafx.h"

#include "VCPropObject.h"

HRESULT CVCProjCfgProperties::CreateInstance(VCConfiguration* pConfig, IDispatch** ppProjCfgProperties)
{
	CComObject<CVCProjCfgProperties> *pPrjObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CVCProjCfgProperties>::CreateInstance(&pPrjObj);
	if (SUCCEEDED(hr))
	{
		pPrjObj->AddRef();
		pPrjObj->Initialize(pConfig);
	}
	*ppProjCfgProperties = pPrjObj;
	return hr;
}

void CVCProjCfgProperties::Initialize(VCConfiguration* pConfig)
{
	m_spConfig = pConfig;

	CComPtr<IDispatch> spDispDbgSettings;
	if (m_spConfig)
		m_spConfig->get_DebugSettings(&spDispDbgSettings);
	m_spDbgSettings = spDispDbgSettings;
}

STDMETHODIMP CVCProjCfgProperties::get_OutputDirectory(BSTR *Val)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->get_OutputDirectory(Val);
}

STDMETHODIMP CVCProjCfgProperties::put_OutputDirectory(BSTR newVal)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->put_OutputDirectory(newVal);
}

STDMETHODIMP CVCProjCfgProperties::get_IntermediateDirectory(BSTR *Val)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->get_IntermediateDirectory(Val);
}

STDMETHODIMP CVCProjCfgProperties::put_IntermediateDirectory(BSTR newVal)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->put_IntermediateDirectory(newVal);
}

STDMETHODIMP CVCProjCfgProperties::get_ConfigurationType(ConfigurationTypes* ConfigType)		// configuration type: application, DLL, etc.
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->get_ConfigurationType(ConfigType);
}

STDMETHODIMP CVCProjCfgProperties::put_ConfigurationType(ConfigurationTypes ConfigType)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->put_ConfigurationType(ConfigType);
}

STDMETHODIMP CVCProjCfgProperties::get_BuildBrowserInformation(VARIANT_BOOL* Bsc)	// build .bsc file
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->get_BuildBrowserInformation(Bsc);
}

STDMETHODIMP CVCProjCfgProperties::put_BuildBrowserInformation(VARIANT_BOOL Bsc)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->put_BuildBrowserInformation(Bsc);
}

STDMETHODIMP CVCProjCfgProperties::get_UseOfMFC(useOfMfc* useMfc)	// how are we using MFC?
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->get_UseOfMFC(useMfc);
}

STDMETHODIMP CVCProjCfgProperties::put_UseOfMFC(useOfMfc useMfc)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->put_UseOfMFC(useMfc);
}

STDMETHODIMP CVCProjCfgProperties::get_UseOfATL(useOfATL* useATL)	// how are we using ATL?
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->get_UseOfATL(useATL);
}

STDMETHODIMP CVCProjCfgProperties::put_UseOfATL(useOfATL useATL)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->put_UseOfATL(useATL);
}

STDMETHODIMP CVCProjCfgProperties::get_ATLMinimizesCRunTimeLibraryUsage(VARIANT_BOOL* UseCRT)	// does the user want to link with the static or dynamic CRT?
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->get_ATLMinimizesCRunTimeLibraryUsage(UseCRT);
}

STDMETHODIMP CVCProjCfgProperties::put_ATLMinimizesCRunTimeLibraryUsage(VARIANT_BOOL UseCRT)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->put_ATLMinimizesCRunTimeLibraryUsage(UseCRT);
}

STDMETHODIMP CVCProjCfgProperties::get_CharacterSet(charSet* optSetting)	// character set to use: Unicode, MBCS, default
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->get_CharacterSet(optSetting);
}

STDMETHODIMP CVCProjCfgProperties::put_CharacterSet(charSet optSetting)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->put_CharacterSet(optSetting);
}

STDMETHODIMP CVCProjCfgProperties::get_ManagedExtensions(VARIANT_BOOL* Managed)	// is this a managed C++ project?
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->get_ManagedExtensions(Managed);
}

STDMETHODIMP CVCProjCfgProperties::put_ManagedExtensions(VARIANT_BOOL Managed)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->put_ManagedExtensions(Managed);
}

STDMETHODIMP CVCProjCfgProperties::get_DeleteExtensionsOnClean(BSTR* Ext)	// wildcard extension list to delete on clean/rebuild from within the intermediate directory
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->get_DeleteExtensionsOnClean(Ext);
}

STDMETHODIMP CVCProjCfgProperties::put_DeleteExtensionsOnClean(BSTR Ext)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->put_DeleteExtensionsOnClean(Ext);
}

STDMETHODIMP CVCProjCfgProperties::get_WholeProgramOptimization(VARIANT_BOOL* Optimize)	// perform whole program optimization on build more or less requires optimization turned on to be effective
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->get_WholeProgramOptimization(Optimize);
}

STDMETHODIMP CVCProjCfgProperties::put_WholeProgramOptimization(VARIANT_BOOL Optimize)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->put_WholeProgramOptimization(Optimize);
}

STDMETHODIMP CVCProjCfgProperties::get_Command(BSTR *Val)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->get_Command(Val);
}

STDMETHODIMP CVCProjCfgProperties::put_Command(BSTR newVal)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->put_Command(newVal);
}

STDMETHODIMP CVCProjCfgProperties::get_WorkingDirectory(BSTR *Val)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->get_WorkingDirectory(Val);
}

STDMETHODIMP CVCProjCfgProperties::put_WorkingDirectory(BSTR newVal)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->put_WorkingDirectory(newVal);
}

STDMETHODIMP CVCProjCfgProperties::get_CommandArguments(BSTR *Val)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->get_CommandArguments(Val);
}

STDMETHODIMP CVCProjCfgProperties::put_CommandArguments(BSTR newVal)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->put_CommandArguments(newVal);
}

STDMETHODIMP CVCProjCfgProperties::get_Attach(VARIANT_BOOL *Val)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->get_Attach(Val);
}

STDMETHODIMP CVCProjCfgProperties::put_Attach(VARIANT_BOOL newVal)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->put_Attach(newVal);
}

STDMETHODIMP CVCProjCfgProperties::get_SQLDebugging(VARIANT_BOOL *Val)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->get_SQLDebugging(Val);
}

STDMETHODIMP CVCProjCfgProperties::put_SQLDebugging(VARIANT_BOOL newVal)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->put_SQLDebugging(newVal);
}

STDMETHODIMP CVCProjCfgProperties::get_Remote(RemoteDebuggerType *Val)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->get_Remote(Val);
}

STDMETHODIMP CVCProjCfgProperties::put_Remote(RemoteDebuggerType newVal)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->put_Remote(newVal);
}

STDMETHODIMP CVCProjCfgProperties::get_RemoteMachine(BSTR *Val)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->get_RemoteMachine(Val);
}

STDMETHODIMP CVCProjCfgProperties::put_RemoteMachine(BSTR newVal)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->put_RemoteMachine(newVal);
}

STDMETHODIMP CVCProjCfgProperties::get_DebuggerType(TypeOfDebugger *Val)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->get_DebuggerType(Val);
}

STDMETHODIMP CVCProjCfgProperties::put_DebuggerType(TypeOfDebugger newVal)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->put_DebuggerType(newVal);
}

STDMETHODIMP CVCProjCfgProperties::get_RemoteCommand(BSTR *Val)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->get_RemoteCommand(Val);
}

STDMETHODIMP CVCProjCfgProperties::put_RemoteCommand(BSTR newVal)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->put_RemoteCommand(newVal);
}

STDMETHODIMP CVCProjCfgProperties::get_HttpUrl(BSTR *Val)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->get_HttpUrl(Val);
}

STDMETHODIMP CVCProjCfgProperties::put_HttpUrl(BSTR newVal)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->put_HttpUrl(newVal);
}

STDMETHODIMP CVCProjCfgProperties::get_PDBPath(BSTR *Val)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->get_PDBPath(Val);
}

STDMETHODIMP CVCProjCfgProperties::put_PDBPath(BSTR NewVal)
{
	CHECK_ZOMBIE(m_spDbgSettings, IDS_ERR_CFG_ZOMBIE);
	return m_spDbgSettings->put_PDBPath(NewVal);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT CVCFileCfgProperties::CreateInstance(VCFileConfiguration* pConfig, IDispatch** ppFileCfgProperties)
{
	CComObject<CVCFileCfgProperties> *pPrjObj = NULL;  // created with 0 ref count
	HRESULT hr = CComObject<CVCFileCfgProperties>::CreateInstance(&pPrjObj);
	if (SUCCEEDED(hr))
	{
		pPrjObj->AddRef();
		pPrjObj->Initialize(pConfig);
	}
	*ppFileCfgProperties = pPrjObj;
	return hr;
}

void CVCFileCfgProperties::Initialize(VCFileConfiguration* pConfig)
{
	m_spConfig = pConfig;
}

STDMETHODIMP CVCFileCfgProperties::get_ExcludedFromBuild(VARIANT_BOOL* ExcludedFromBuild)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->get_ExcludedFromBuild(ExcludedFromBuild);
}

STDMETHODIMP CVCFileCfgProperties::put_ExcludedFromBuild(VARIANT_BOOL ExcludedFromBuild)
{
	CHECK_ZOMBIE(m_spConfig, IDS_ERR_CFG_ZOMBIE);
	return m_spConfig->put_ExcludedFromBuild(ExcludedFromBuild);
}
