// OutsEnum.h: Definition of the COutsEnum class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "rsrc.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// COutsEnum
class CVCArchy;
class CGenCfg;

class COutsEnum : 
	public IVsEnumOutputs, 
	public ISupportErrorInfo,
	public CComObjectRoot
	//,public CComCoClass<COutsEnum,&CLSID_OutsEnum>
{
public:
	COutsEnum() { m_pConfig = NULL; m_iOut = 0; }
	~COutsEnum() {}
	void Initialize(VCConfiguration* pConfig, CVCPtrList *pList);
	static HRESULT CreateInstance(IVsEnumOutputs **ppOutsEnum, VCConfiguration* pConfig, CVCPtrList *pList);

BEGIN_COM_MAP(COutsEnum)
	COM_INTERFACE_ENTRY(IVsEnumOutputs)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
//DECLARE_NOT_AGGREGATABLE(COutsEnum) 
// Remove the comment from the line above if you don't want your object to 
// support aggregation. 

DECLARE_NO_REGISTRY()
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IVsEnumOutputs
public:
	STDMETHOD(Reset)();
	STDMETHOD(Next)(/*[in]*/ ULONG cElements, /*[in, out, size_is(cElements)]*/ IVsOutput *rgpIVsOutput[], /*[out]*/ ULONG *pcElementsFetched);
	STDMETHOD(Skip)(/*[in]*/ ULONG cElements);
	STDMETHOD(Clone)(/*[out]*/ IVsEnumOutputs **ppIVsEnumOutputs);

protected:
	CVCPtrList m_ListOuts;
	VCConfiguration* m_pConfig;
	UINT m_iOut;
};
