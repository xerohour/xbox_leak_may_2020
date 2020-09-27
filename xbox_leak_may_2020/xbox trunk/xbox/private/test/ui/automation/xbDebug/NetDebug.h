// NetDebug.h : Declaration of the CNetDebug

#ifndef __NETDEBUG_H_
#define __NETDEBUG_H_

#include "resource.h"       // main symbols


#include <windows.h>
#include <stdio.h>
#include <xboxdbg.h>




/////////////////////////////////////////////////////////////////////////////
// CNetDebug
class ATL_NO_VTABLE CNetDebug : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNetDebug, &CLSID_NetDebug>,
	public IDispatchImpl<INetDebug, &IID_INetDebug, &LIBID_XBDEBUGLib>
{
public:
	CNetDebug()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_NETDEBUG)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CNetDebug)
	COM_INTERFACE_ENTRY(INetDebug)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// INetDebug
public:
	STDMETHOD(ClearLines)();
	STDMETHOD(Disconnect)();
	STDMETHOD(get_ParseTag)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ParseTag)(/*[in]*/ BSTR newVal);
	STDMETHOD(GetAllLines)(/*[in]*/ BOOL clear, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(GetLine)(/*[in]*/ unsigned lineNum, /*[in]*/ short operation, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(Stop)();
	STDMETHOD(Start)();
	STDMETHOD(Connect)(/*[in]*/ BSTR xboxName);
};



#endif //__NETDEBUG_H_
