// OutGroup.h: interface for the COutputGroup class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <path2.h>
class CGenCfg;
class COutput2 : 
	public IVsOutput2,
	public IVCBuildOutputItem,
	public CComObjectRoot
{
public:
	COutput2(){}
	~COutput2(){}

	void Initialize(IVsProjectCfg2* pCfg, BSTR bstrPath, BSTR bstrRelativePath);
	static HRESULT CreateInstance(IVsOutput2 **ppOutput, IVsProjectCfg2* pCfg, BSTR bstrPath, BSTR bstrRelativePath);

BEGIN_COM_MAP(COutput2)
	COM_INTERFACE_ENTRY(IVsOutput)
	COM_INTERFACE_ENTRY(IVsOutput2)
	COM_INTERFACE_ENTRY(IVCBuildOutputItem)
END_COM_MAP()

public:
	// IVsOutput
    STDMETHOD(get_DisplayName)( /* [out] */ BSTR *pbstrDisplayName);
    STDMETHOD(get_CanonicalName)( /* [out] */ BSTR *pbstrCanonicalName);
    STDMETHOD(get_DeploySourceURL)( /* [out] */ BSTR *pbstrDeploySourceURL);
    STDMETHOD(get_Type)( /* [out] */ GUID *pguidType);
	// IVSOutput2
    STDMETHOD(get_RootRelativeURL)( /* [out] */ BSTR *pbstrRelativePath); 
    STDMETHOD(get_Property)( /* [in] */ LPCOLESTR szProperty, VARIANT *pvar);

public:
	// IVCBuildOutputItem
	STDMETHOD(get_FullPathName)(BSTR* pbstrFullPathName) 
		{ return get_CanonicalName(pbstrFullPathName); }

protected:	// helpers
	HRESULT GetVCConfiguration(VCConfiguration** ppConfig);

// Member variables
protected:
	CComBSTR	m_bstrFullPath;
	CComBSTR	m_bstrRelativePath;
	CComPtr<IVsCfg> m_pCfg;
};


class CIDBDeps
{
public:
	static BOOL IsComPlus(LPCOLESTR strFile);
	HRESULT InitMRE();
	HRESULT GetImportDependencies(BSTR bstrIdbFile, CVCStringWList *pList);
	HRESULT GetLibDependencies( VCConfiguration *pConfig, BSTR bstrPrimaryOutput, CVCStringWList *pList );

private:
	static HINSTANCE s_hInstPdbDll;
	static wchar_t s_szComplusDir[MAX_PATH+1];
	static int s_nComPlusLen;
};

class COutputGroup : 
	public IVsOutputGroup,
	public CComObjectRoot
{
public:
	COutputGroup() {m_oldTime.dwLowDateTime = 0;m_oldTime.dwHighDateTime = 0;}
	~COutputGroup() {}
	static HRESULT CreateInstance(IVsOutputGroup **ppOutGroup, IVsProjectCfg2* pParent, int groupType);
	void Initialize(IVsProjectCfg2* pParent, int groupType);

BEGIN_COM_MAP(COutputGroup)
	COM_INTERFACE_ENTRY(IVsOutputGroup)
END_COM_MAP()

public:
	// IVsOutputGroup
//    STDMETHOD(get_CfgType)( REFIID iidCfg, void **ppCfg)
//		{ return E_NOTIMPL; /* COutputGroup::get_CfgType */ }
	STDMETHOD(get_CanonicalName)( BSTR *pbstrCanonicalName);
	STDMETHOD(get_DisplayName)( BSTR *pbstrDisplayName);
	STDMETHOD(get_KeyOutput)( BSTR *pbstrKeyOutput);
    STDMETHOD(get_ProjectCfg)( IVsProjectCfg2 **ppIVsProjectCfg2);
    STDMETHOD(get_Outputs)( ULONG celt, IVsOutput2 * rgpcfg[], ULONG *pcActual);
    STDMETHOD(get_DeployDependencies)( ULONG celt,IVsDeployDependency * rgpdpd[], ULONG *pcActual);
	STDMETHOD(get_Description)(BSTR *pbstrDescription);

// helper functions
	HRESULT GetVCConfiguration(VCConfiguration** ppConfig);
	HRESULT GetVCFileEnumerator(IEnumVARIANT** ppEnum, VCProject* pProj = NULL);
	HRESULT GetVCProject(VCProject** ppProject);
	BOOL IsManaged(VCConfiguration* pConfig);

// member variables
protected:
	int 					m_groupType;
	CComPtr<IVsProjectCfg2> m_pCfg;
	CVCStringWList 			m_strList;
	FILETIME			m_oldTime;
	CIDBDeps			m_IDB;
};

class CDeployDependency : 
	public IVsDeployDependency,
	public CComObjectRoot
{
public:
	CDeployDependency() : m_depName("") { }
	void Initialize(BSTR bstrURL);
	static HRESULT CreateInstance( IVsDeployDependency **ppDep, BSTR bstrURL);

BEGIN_COM_MAP(CDeployDependency)
	COM_INTERFACE_ENTRY(IVsDeployDependency)
END_COM_MAP()


// IVsDeployDependency
public:
    STDMETHOD(get_DeployDependencyURL)( BSTR *pbstrURL );

// Member variables
protected:
	CComBSTR m_depName;

};

class CVCBuildOutputItems :
	public IVCBuildOutputItems, 
	public CComObjectRoot
{
public:
	static HRESULT CreateInstance(CVCBuildOutputItems** ppItems);
	CVCBuildOutputItems() : m_pos(NULL) {}
	~CVCBuildOutputItems();

BEGIN_COM_MAP(CVCBuildOutputItems)
	COM_INTERFACE_ENTRY(IVCBuildOutputItems)
END_COM_MAP()

// IVCBuildOutputItems
public:
	STDMETHOD(get_Count)(long* pcOutputs);
	STDMETHOD(Reset)() 
	{ 
		m_pos = m_outputs.GetHeadPosition();
		return S_OK;
	}
	STDMETHOD(Next)(IVCBuildOutputItem** ppOutput);

public:
	void Add(IVsOutput* pItem);

protected:
	CVCPtrList m_outputs;
	VCPOSITION m_pos;
};

class COutputWrapper : 
	public IVsOutput,
	public IVCBuildOutputItem, 
	public CComObjectRoot
{
public:
	COutputWrapper() {}
	~COutputWrapper() {}
	static HRESULT CreateInstance(IVsOutput **ppOutputWrapper, IVsOutput* pOutput)
	{
		CHECK_POINTER_NULL(ppOutputWrapper);
		*ppOutputWrapper = NULL;
		CComObject<COutputWrapper> *pOutputWrapperObj = NULL;  // created with 0 ref count
		HRESULT hr = CComObject<COutputWrapper>::CreateInstance(&pOutputWrapperObj);
		RETURN_ON_FAIL_OR_NULL(hr, pOutputWrapperObj);
		IVsOutput* pOutputVar = pOutputWrapperObj;
		pOutputVar->AddRef();
		pOutputWrapperObj->Initialize(pOutput);
		*ppOutputWrapper = pOutputVar;
		return hr;
	}

BEGIN_COM_MAP(COutputWrapper)
	COM_INTERFACE_ENTRY(IVsOutput)
	COM_INTERFACE_ENTRY(IVCBuildOutputItem)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CDeployDependency) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

protected:
	void Initialize(IVsOutput* pOutput) { m_spOutput = pOutput; }

public:
	// IVsOutput
    STDMETHOD(get_DisplayName)(BSTR *pbstrDisplayName) 
	{
		CHECK_ZOMBIE(m_spOutput, IDS_ERR_OUTPUT_ZOMBIE);
		return m_spOutput->get_DisplayName(pbstrDisplayName);
	}
    STDMETHOD(get_CanonicalName)(BSTR *pbstrCanonicalName)
	{
		CHECK_ZOMBIE(m_spOutput, IDS_ERR_OUTPUT_ZOMBIE);
		return m_spOutput->get_CanonicalName(pbstrCanonicalName);
	}
    STDMETHOD(get_DeploySourceURL)(BSTR *pbstrDeploySourceURL)
	{
		CHECK_ZOMBIE(m_spOutput, IDS_ERR_OUTPUT_ZOMBIE);
		return m_spOutput->get_DeploySourceURL(pbstrDeploySourceURL);
	}
    STDMETHOD(get_Type)(GUID *pguidType)
	{
		CHECK_ZOMBIE(m_spOutput, IDS_ERR_OUTPUT_ZOMBIE);
		return m_spOutput->get_Type(pguidType);
	}

// IVCBuildOutputItem
public:
	STDMETHOD(get_FullPathName)(BSTR* pbstrFullPathName) 
		{ return get_CanonicalName(pbstrFullPathName); }

// Member variables
protected:
	CComPtr<IVsOutput> m_spOutput;
};

