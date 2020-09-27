/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    Hawk.h

Abstract:

    COM object wrapper for the Isoc USB Device class

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/
#ifndef __HAWK_H_
#define __HAWK_H_

#include "resource.h"       // main symbols

class ATL_NO_VTABLE CHawk : public CComObjectRootEx<CComSingleThreadModel>, public CComCoClass<CHawk, &CLSID_Hawk>, public IDispatchImpl<IHawk, &IID_IHawk, &LIBID_USBSIMULATORLib>
    {
    public:
        CHawk();
        ~CHawk();

    private:
        IsocDevice hawk;

    public:
        DECLARE_REGISTRY_RESOURCEID(IDR_HAWK)
        DECLARE_PROTECT_FINAL_CONSTRUCT()
        BEGIN_COM_MAP(CHawk)
            COM_INTERFACE_ENTRY(IHawk)
            COM_INTERFACE_ENTRY(IDispatch)
        END_COM_MAP()

    // IHawk
    public:
	    STDMETHOD(Free)(void);
	    STDMETHOD(EndpointMode)(/*[in]*/ short mode);
	    STDMETHOD(Unplug)(void);
	    STDMETHOD(Plug)(void);
	    STDMETHOD(get_enumerated)(/*[out, retval]*/ BOOL *pVal);
	    STDMETHOD(get_record)(/*[out, retval]*/ BOOL *pVal);
	    STDMETHOD(put_record)(/*[in]*/ BOOL newVal);
        STDMETHOD(get_port)(/*[out, retval]*/ short *pVal);
        STDMETHOD(put_port)(/*[in]*/ short newVal);
        STDMETHOD(get_IP)(/*[out, retval]*/ BSTR *pVal);
        STDMETHOD(put_IP)(/*[in]*/ const BSTR newVal);
	    STDMETHOD(get_deviceName)(/*[out, retval]*/ BSTR *pVal);
    };

#endif //__HAWK_H_
