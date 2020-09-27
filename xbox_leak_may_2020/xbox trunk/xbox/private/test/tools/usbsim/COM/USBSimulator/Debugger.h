// Debugger.h : Declaration of the CDebugger

#ifndef __DEBUGGER_H_
#define __DEBUGGER_H_

#include "resource.h"       // main symbols

#define DEBUGGER_PORT 0x2db
#define DMBREAK_NONE 0
#define DMBREAK_WRITE 1
#define DMBREAK_READWRITE 2
#define DMBREAK_EXECUTE 3


/////////////////////////////////////////////////////////////////////////////
// CDebugger
class ATL_NO_VTABLE CDebugger : public CComObjectRootEx<CComSingleThreadModel>, public CComCoClass<CDebugger, &CLSID_Debugger>, public IDispatchImpl<IDebugger, &IID_IDebugger, &LIBID_USBSIMULATORLib>
    {
    private:
        SOCKET sock;

    public:
        CDebugger();
        ~CDebugger();

    private:
        int IsDataAvailable(void);

    public:
        DECLARE_REGISTRY_RESOURCEID(IDR_DEBUGGER)
        DECLARE_PROTECT_FINAL_CONSTRUCT()
        BEGIN_COM_MAP(CDebugger)
            COM_INTERFACE_ENTRY(IDebugger)
            COM_INTERFACE_ENTRY(IDispatch)
        END_COM_MAP()

    // IDebugger
    public:
	    STDMETHOD(Test2)(/*[defaultvalue(22)]*/ int val);
	    STDMETHOD(SearchForIP)(/*[in]*/ BSTR machineName, /*[out, retval]*/ BSTR *pVal);
	    STDMETHOD(Test)();
        STDMETHOD(SendCommand)(/*[in]*/ const BSTR cmd);
        STDMETHOD(GetResponse)(/*[out, retval]*/ BSTR *pVal);
        STDMETHOD(Disconnect)(void);
        STDMETHOD(Connect)(/*[in]*/ const BSTR ipAddress);
    };

#endif //__DEBUGGER_H_
