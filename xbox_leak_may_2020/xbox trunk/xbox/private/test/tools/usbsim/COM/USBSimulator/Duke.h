/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    Duke.h

Abstract:

    COM object wrapper for the XID USB Device class

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/
#ifndef __DUKE_H_
#define __DUKE_H_

#include "resource.h"       // main symbols

class ATL_NO_VTABLE CDuke : public CComObjectRootEx<CComSingleThreadModel>, public CComCoClass<CDuke, &CLSID_Duke>, public IDispatchImpl<IDuke, &IID_IDuke, &LIBID_USBSIMULATORLib>
    {
    public:
	    CDuke();
	    ~CDuke();

    private:
        XIDDevice xid;
        unsigned short buttonUpDelay;
        unsigned short buttonDownDelay;

    public:
        DECLARE_REGISTRY_RESOURCEID(IDR_DUKE)
        DECLARE_PROTECT_FINAL_CONSTRUCT()
        BEGIN_COM_MAP(CDuke)
            COM_INTERFACE_ENTRY(IDuke)
            COM_INTERFACE_ENTRY(IDispatch)
        END_COM_MAP()

    // IDuke
    public:
	    STDMETHOD(get_deviceCapabilitiesLength)(BSTR type, /*[out, retval]*/ unsigned short *pVal);
	    STDMETHOD(put_deviceCapabilitiesLength)(BSTR type, /*[in]*/ unsigned short newVal);
	    STDMETHOD(get_xidDescriptorLength)(/*[out, retval]*/ unsigned short *pVal);
	    STDMETHOD(put_xidDescriptorLength)(/*[in]*/ unsigned short newVal);
	    STDMETHOD(get_xidPacketLength)(/*[out, retval]*/ unsigned short *pVal);
	    STDMETHOD(put_xidPacketLength)(/*[in]*/ unsigned short newVal);
	    STDMETHOD(get_deviceCapabilities)(/*[in]*/ BSTR type, /*[out, retval]*/ BSTR *pVal);
	    STDMETHOD(put_deviceCapabilities)(/*[in]*/ BSTR type, /*[in]*/ BSTR newVal);
	    STDMETHOD(get_xidPacket)(/*[out, retval]*/ BSTR *pVal);
	    STDMETHOD(put_xidPacket)(/*[in]*/ BSTR newVal);
	    STDMETHOD(get_xidDescriptor)(/*[out, retval]*/ BSTR *pVal);
	    STDMETHOD(put_xidDescriptor)(/*[in]*/ BSTR newVal);
	    STDMETHOD(get_configurationDescriptor)(/*[out, retval]*/ BSTR *pVal);
	    STDMETHOD(put_configurationDescriptor)(/*[in]*/ BSTR newVal);
	    STDMETHOD(get_deviceDescriptor)(/*[out, retval]*/ BSTR *pVal);
	    STDMETHOD(put_deviceDescriptor)(/*[in]*/ BSTR newVal);
	    STDMETHOD(Free)(void);
	    STDMETHOD(SetButtonState)(/*[in]*/ long digital, /*[in]*/ short analog1, /*[in]*/ short analog2, /*[in]*/ short analog3, /*[in]*/ short analog4, /*[in]*/ short analog5, /*[in]*/ short analog6, /*[in]*/ short analog7, /*[in]*/ short analog8, /*[in]*/ short thumbLX, /*[in]*/ short thumbLY, /*[in]*/ short thumbRX, /*[in]*/ short thumbRY);
	    STDMETHOD(get_buttonUpDelay)(/*[out, retval]*/ unsigned short *pVal);
	    STDMETHOD(put_buttonUpDelay)(/*[in]*/ unsigned short newVal);
	    STDMETHOD(get_buttonDownDelay)(/*[out, retval]*/ unsigned short *pVal);
	    STDMETHOD(put_buttonDownDelay)(/*[in]*/ unsigned short newVal);
	    STDMETHOD(Button)(/*[in]*/ const BSTR buttonName);
	    STDMETHOD(EndpointMode)(/*[in]*/ short mode);
	    STDMETHOD(Unplug)(/*[out, retval]*/ unsigned long *errCode);
	    STDMETHOD(Plug)(/*[out, retval]*/ unsigned long *errCode);
	    STDMETHOD(get_enumerated)(/*[out, retval]*/ BOOL *pVal);
	    STDMETHOD(get_record)(/*[out, retval]*/ BOOL *pVal);
	    STDMETHOD(put_record)(/*[in]*/ BOOL newVal);
        STDMETHOD(get_port)(/*[out, retval]*/ short *pVal);
        STDMETHOD(put_port)(/*[in]*/ short newVal);
        STDMETHOD(get_IP)(/*[out, retval]*/ BSTR *pVal);
        STDMETHOD(put_IP)(/*[in]*/ const BSTR newVal);
	    STDMETHOD(get_deviceName)(/*[out, retval]*/ BSTR *pVal);
    };

#endif //__DUKE_H_
