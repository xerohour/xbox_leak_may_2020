#pragma once

class CVCProjCfgProperties :
	public IDispatchImpl<VCProjectConfigurationProperties, &__uuidof(VCProjectConfigurationProperties), &LIBID_VCProjectLibrary, PublicProjBuildTypeLibNumber, 0, CVsTypeInfoHolder>,
	public CComObjectRoot
{
public:
	CVCProjCfgProperties() {}
	virtual ~CVCProjCfgProperties() {}
	static HRESULT CreateInstance(VCConfiguration* pConfig, IDispatch** ppProjCfgProperties);

BEGIN_COM_MAP(CVCProjCfgProperties)
	COM_INTERFACE_ENTRY(VCProjectConfigurationProperties)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// VCProjectConfigurationProperties
public:
	STDMETHOD(get_OutputDirectory)(BSTR *Val);
	STDMETHOD(put_OutputDirectory)(BSTR newVal);
	STDMETHOD(get_IntermediateDirectory)(BSTR *Val);
	STDMETHOD(put_IntermediateDirectory)(BSTR newVal);
	STDMETHOD(get_ConfigurationType)(ConfigurationTypes* ConfigType);		// configuration type: application, DLL, etc.
	STDMETHOD(put_ConfigurationType)(ConfigurationTypes ConfigType);
	STDMETHOD(get_BuildBrowserInformation)(VARIANT_BOOL* Bsc);	// build .bsc file
	STDMETHOD(put_BuildBrowserInformation)(VARIANT_BOOL Bsc);
	STDMETHOD(get_UseOfMFC)(useOfMfc* useMfc);	// how are we using MFC?
	STDMETHOD(put_UseOfMFC)(useOfMfc useMfc);
	STDMETHOD(get_UseOfATL)(useOfATL* useATL);	// how are we using ATL?
	STDMETHOD(put_UseOfATL)(useOfATL useATL);
	STDMETHOD(get_ATLMinimizesCRunTimeLibraryUsage)(VARIANT_BOOL* UseCRT);	// does the user want to link with the static or dynamic CRT?
	STDMETHOD(put_ATLMinimizesCRunTimeLibraryUsage)(VARIANT_BOOL UseCRT);
	STDMETHOD(get_CharacterSet)(charSet* optSetting);	// character set to use: Unicode, MBCS, default
	STDMETHOD(put_CharacterSet)(charSet optSetting);
	STDMETHOD(get_ManagedExtensions)(VARIANT_BOOL* Managed);	// is this a managed C++ project?
	STDMETHOD(put_ManagedExtensions)(VARIANT_BOOL Managed);
	STDMETHOD(get_DeleteExtensionsOnClean)(BSTR* Ext);	// wildcard extension list to delete on clean/rebuild from within the intermediate directory
	STDMETHOD(put_DeleteExtensionsOnClean)(BSTR Ext);
	STDMETHOD(get_WholeProgramOptimization)(VARIANT_BOOL* Optimize);	// perform whole program optimization on build; more or less requires optimization turned on to be effective
	STDMETHOD(put_WholeProgramOptimization)(VARIANT_BOOL Optimize);
	STDMETHOD(get_Command)(BSTR *Val);
	STDMETHOD(put_Command)(BSTR newVal);
	STDMETHOD(get_WorkingDirectory)(BSTR *Val);
	STDMETHOD(put_WorkingDirectory)(BSTR newVal);
	STDMETHOD(get_CommandArguments)(BSTR *Val);
	STDMETHOD(put_CommandArguments)(BSTR newVal);
	STDMETHOD(get_Attach)(VARIANT_BOOL *Val);
	STDMETHOD(put_Attach)(VARIANT_BOOL newVal);
	STDMETHOD(get_Remote)(RemoteDebuggerType *Val);
	STDMETHOD(put_Remote)(RemoteDebuggerType newVal);
	STDMETHOD(get_RemoteMachine)(BSTR *Val);
	STDMETHOD(put_RemoteMachine)(BSTR newVal);
	STDMETHOD(get_DebuggerType)(TypeOfDebugger *Val);
	STDMETHOD(put_DebuggerType)(TypeOfDebugger newVal);
	STDMETHOD(get_RemoteCommand)(BSTR *Val);
	STDMETHOD(put_RemoteCommand)(BSTR newVal);
	STDMETHOD(get_HttpUrl)(BSTR *Val);
	STDMETHOD(put_HttpUrl)(BSTR newVal);
	STDMETHOD(get_PDBPath)(BSTR *Val);
	STDMETHOD(put_PDBPath)(BSTR NewVal);
	STDMETHOD(get_SQLDebugging)(VARIANT_BOOL *Val);
	STDMETHOD(put_SQLDebugging)(VARIANT_BOOL newVal);

protected:
	void Initialize(VCConfiguration* pConfig);

// data
protected:
	CComPtr<VCConfiguration> m_spConfig;
	CComQIPtr<VCDebugSettings> m_spDbgSettings;
};

class CVCFileCfgProperties :
	public IDispatchImpl<VCFileConfigurationProperties, &__uuidof(VCFileConfigurationProperties), &LIBID_VCProjectLibrary, PublicProjBuildTypeLibNumber, 0, CVsTypeInfoHolder>,
	public CComObjectRoot
{
public:
	CVCFileCfgProperties() {}
	virtual ~CVCFileCfgProperties() {}
	static HRESULT CreateInstance(VCFileConfiguration* pConfig, IDispatch** ppFileCfgProperties);

BEGIN_COM_MAP(CVCFileCfgProperties)
	COM_INTERFACE_ENTRY(VCFileConfigurationProperties)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// VCFileConfigurationProperties
public:
	STDMETHOD(get_ExcludedFromBuild)(VARIANT_BOOL* ExcludedFromBuild);
	STDMETHOD(put_ExcludedFromBuild)(VARIANT_BOOL ExcludedFromBuild);

protected:
	void Initialize(VCFileConfiguration* pConfig);

// data
protected:
	CComPtr<VCFileConfiguration> m_spConfig;
};
