// ProjOuts.h: Definition of the CProjectOutput class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "rsrc.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CProjectOutput
class CVCArchy;

class CProjectOutput : 
	public IVsOutput,
	public CComObjectRoot
{
public:
	CProjectOutput() : m_pConfig(NULL) {}
	~CProjectOutput();
	void Initialize(VCConfiguration* pConfig, CPathW& path);
	static HRESULT CreateInstance(IVsOutput** ppProjOut, VCConfiguration* pConfig, CPathW& path);

BEGIN_COM_MAP(CProjectOutput)
	COM_INTERFACE_ENTRY(IVsOutput)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(CProjectOutput) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_NO_REGISTRY()

// IVsOutput
public:
	STDMETHOD(get_DisplayName)(/*[out]*/ BSTR *pbstrDisplayName);
	STDMETHOD(get_CanonicalName)(/*[out]*/ BSTR *pbstrCanonicalName);
	STDMETHOD(get_DeploySourceURL)(/*[out]*/ BSTR *pbstrDeploySourceURL);
	STDMETHOD(get_Type)(/*[out]*/ GUID *pguidType);

protected:
	CPathW m_path;
	VCConfiguration* m_pConfig;
};
