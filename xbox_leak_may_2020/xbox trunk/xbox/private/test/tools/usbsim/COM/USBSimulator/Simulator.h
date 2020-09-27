/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    Simulator.h

Abstract:

    COM wrapper object for the USBSimulator class

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#ifndef __SIMULATOR_H_
#define __SIMULATOR_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSimulator
class ATL_NO_VTABLE CSimulator : public CComObjectRootEx<CComSingleThreadModel>, public CComCoClass<CSimulator, &CLSID_Simulator>, public IDispatchImpl<ISimulator, &IID_ISimulator, &LIBID_USBSIMULATORLib>
    {
    public:
        CSimulator();
        ~CSimulator();

    private:
        USBSimulator usbsim;

    public:
        DECLARE_REGISTRY_RESOURCEID(IDR_SIMULATOR)
        DECLARE_PROTECT_FINAL_CONSTRUCT()
        BEGIN_COM_MAP(CSimulator)
            COM_INTERFACE_ENTRY(ISimulator)
            COM_INTERFACE_ENTRY(IDispatch)
        END_COM_MAP()

    // ISimulator
    public:
	    STDMETHOD(GetLastError)(/*[out, retval]*/ unsigned long *pVal);
	    STDMETHOD(get_logFilename)(/*[out, retval]*/ BSTR *pVal);
	    STDMETHOD(put_logFilename)(/*[in]*/ const BSTR newVal);
	    STDMETHOD(GetErrorDescription)(/*[in]*/ unsigned long errCode, /*[out, retval]*/ BSTR *pDescription);
	    STDMETHOD(LogFormat)(/*[in]*/ short format);
	    STDMETHOD(Log)(/*[in]*/ const BSTR string);
	    STDMETHOD(GetUSBPorts)(/*[in]*/ const BSTR ipAddr, /*[out, retval]*/ short *connectedMask);
	    STDMETHOD(GetMACAddr)(/*[in]*/ const BSTR ipAddr, /*[out, retval]*/ BSTR *macAddr);
        STDMETHOD(GetVersion)(/*[in]*/ const BSTR ipAddr, /*[out, retval]*/ BSTR *version);
    };

#endif //__SIMULATOR_H_
