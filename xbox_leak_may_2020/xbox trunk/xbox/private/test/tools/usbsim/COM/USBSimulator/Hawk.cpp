/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    Hawk.cpp

Abstract:

    COM object wrapper for the Isoc USB Device class

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/
#include "stdafx.h"
#include "COMUSBSimulator.h"
#include "Hawk.h"


CHawk::CHawk()
    {
    }
CHawk::~CHawk()
    {
    hawk.SoftBreak(0);
    }

STDMETHODIMP CHawk::get_deviceName(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    *pVal = CreateBstrFromAnsi(hawk.GetName());
    return S_OK;
    }

STDMETHODIMP CHawk::get_IP(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    char buffer[32];
    sprintf(buffer, "%u.%u.%u.%u", ((unsigned char*)&hawk.simIP)[0], ((unsigned char*)&hawk.simIP)[1], ((unsigned char*)&hawk.simIP)[2], ((unsigned char*)&hawk.simIP)[3]);
    *pVal = CreateBstrFromAnsi(buffer);
    return S_OK;
    }

STDMETHODIMP CHawk::put_IP(const BSTR newVal)
    {
    USES_CONVERSION;
    if(!newVal) return E_INVALIDARG;
    hawk.simIP = inet_addr(W2A(newVal));
    return S_OK;
    }

STDMETHODIMP CHawk::get_port(short *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    *pVal = hawk.usbPort;
    return S_OK;
    }

STDMETHODIMP CHawk::put_port(short newVal)
    {
    hawk.usbPort = (char)newVal;
    hawk.destPort = htons(newVal+SIM_NETPORT_BASE);
    return S_OK;
    }

STDMETHODIMP CHawk::get_enumerated(BOOL *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    if(hawk.IsEnumerated()) *pVal = TRUE;
    else *pVal = FALSE;
    return S_OK;
    }

STDMETHODIMP CHawk::get_record(BOOL *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    *pVal = hawk.record;
    return S_OK;
    }

STDMETHODIMP CHawk::put_record(BOOL newVal)
    {
    hawk.record = !!newVal;
    return S_OK;
    }

STDMETHODIMP CHawk::Plug(void)
    {
    hawk.Plug();
    return S_OK;
    }

STDMETHODIMP CHawk::Unplug(void)
    {
    hawk.Unplug();
    return S_OK;
    }

STDMETHODIMP CHawk::EndpointMode(short mode)
    {
    if(mode<0 || mode>3) return E_INVALIDARG;
    hawk.EndpointConfig(mode);
    return S_OK;
    }

STDMETHODIMP CHawk::Free(void)
    {
    return S_OK;
    }
