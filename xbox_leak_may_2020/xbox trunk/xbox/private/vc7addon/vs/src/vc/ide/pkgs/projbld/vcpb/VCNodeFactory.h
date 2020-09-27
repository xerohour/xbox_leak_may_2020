// VCNodeFactory.h : Declaration of the CVCNodeFactory

#ifndef __VCNODEFACTORY_H_
#define __VCNODEFACTORY_H_

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CVCNodeFactory
class ATL_NO_VTABLE CVCNodeFactory: 
	public CComObjectRoot,
	public CComCoClass<CVCNodeFactory, &CLSID_VCNodeFactory>,
	IVCNodeFactory,
	IXMLNodeFactory
{
public:
	CVCNodeFactory();
	~CVCNodeFactory();

	static HRESULT CreateInstance( IVCNodeFactory **ppNodeFactory );

DECLARE_NO_REGISTRY()

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CVCNodeFactory)
	COM_INTERFACE_ENTRY(IVCNodeFactory)
	COM_INTERFACE_ENTRY(IXMLNodeFactory)
END_COM_MAP()

// IVCNodeFactory
public:
	STDMETHOD(put_Name)( BSTR bstrName );
	STDMETHOD(get_Root)( IDispatch **ppDisp );
	STDMETHOD(get_FileEncoding)( BSTR* pbstrEncoding );

// IXMLNodeFactory
public:
    STDMETHOD(NotifyEvent)( IXMLNodeSource* pSource, XML_NODEFACTORY_EVENT iEvt );
    STDMETHOD(BeginChildren)( IXMLNodeSource* pSource, XML_NODE_INFO* pNodeInfo );   
    STDMETHOD(EndChildren)( IXMLNodeSource* pSource, BOOL fEmpty, XML_NODE_INFO* pNodeInfo );
    STDMETHOD(Error)( IXMLNodeSource* pSource, HRESULT hrErrorCode, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR **aNodeInfo );
    STDMETHOD(CreateNode)( IXMLNodeSource __RPC_FAR *pSource, PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR **aNodeInfo );

// helper functions
protected:
	HRESULT HandleFileNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp);
	HRESULT HandleFilterNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp);
	HRESULT HandleToolNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp);
	HRESULT HandleProjConfigNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp);
	HRESULT HandleFileConfigNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp);
	HRESULT HandlePlatformNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp);
	HRESULT HandleProjectNode(USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp, USHORT& i);
	HRESULT HandleStyleSheetNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp);
	HRESULT HandleGlobalNode(PVOID pNodeParent, USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, CComPtr<IDispatch>& pDisp, USHORT& i);
	HRESULT ValidateTagAndGetName(USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, LPCOLESTR szAttribName, CComBSTR& bstrName);
	HRESULT HandleXMLHeaderNode(USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo);
	void CollectAttributeNameAndValue(USHORT cNumRecs, XML_NODE_INFO __RPC_FAR ** aNodeInfo, USHORT& i, CComBSTR& bstrAttrName, 
		CComBSTR& bstrAttrValue);

private:
	CComBSTR m_bstrName;
	CComBSTR m_bstrEncoding;
	CComPtr<IDispatch> m_pRoot;
};

#endif // __VCNODEFACTORY_H_
