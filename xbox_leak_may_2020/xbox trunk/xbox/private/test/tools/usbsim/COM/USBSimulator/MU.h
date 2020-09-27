/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    MU.h

Abstract:

    COM object wrapper for the Bulk USB Device class

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/
#ifndef __MU_H_
#define __MU_H_

#include "resource.h"       // main symbols

class ATL_NO_VTABLE CMU : public CComObjectRootEx<CComSingleThreadModel>, public CComCoClass<CMU, &CLSID_MU>, public IDispatchImpl<IMU, &IID_IMU, &LIBID_USBSIMULATORLib>
    {
    public:
        CMU();
        ~CMU();

    private:
        BulkDevice bulk;

    public:
        DECLARE_REGISTRY_RESOURCEID(IDR_MU)
        DECLARE_PROTECT_FINAL_CONSTRUCT()
        BEGIN_COM_MAP(CMU)
            COM_INTERFACE_ENTRY(IMU)
            COM_INTERFACE_ENTRY(IDispatch)
        END_COM_MAP()

    // IMU
    public:
	    STDMETHOD(FakeCapacity)(/*[in]*/ unsigned long numBlocks, /*[in]*/ unsigned short blockSize, /*[in, defaultvalue(0)]*/ unsigned short logicalSize);
	    STDMETHOD(get_configurationDescriptor)(/*[out, retval]*/ BSTR *pVal);
	    STDMETHOD(put_configurationDescriptor)(/*[in]*/ BSTR newVal);
	    STDMETHOD(get_deviceDescriptor)(/*[out, retval]*/ BSTR *pVal);
	    STDMETHOD(put_deviceDescriptor)(/*[in]*/ BSTR newVal);
	    STDMETHOD(get_data)(/*[in]*/ unsigned long offset, /*[in]*/ unsigned long length, /*[out, retval]*/ BSTR *pVal);
	    STDMETHOD(put_data)(/*[in]*/ unsigned long offset, /*[in]*/ unsigned long length, /*[in]*/ BSTR newVal);
	    STDMETHOD(Format)(void);
	    STDMETHOD(Free)(void);
	    STDMETHOD(Unformat)(void);
	    STDMETHOD(StoreMU)(/*[in]*/ const BSTR filename, /*[out, retval]*/ unsigned long *pErr);
	    STDMETHOD(LoadMU)(/*[in]*/ const BSTR filename, /*[out, retval]*/ unsigned long *pErr);
	    STDMETHOD(SetCapacity)(/*[in]*/ unsigned long numBlocks, /*[in]*/ unsigned short blockSize, /*[in, defaultvalue(0)]*/ unsigned short logicalSize);
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

#endif //__MU_H_
