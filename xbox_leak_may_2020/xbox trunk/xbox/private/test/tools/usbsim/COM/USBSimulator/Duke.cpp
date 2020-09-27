/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    Duke.cpp

Abstract:

    COM object wrapper for the XID USB Device class

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/
#include "stdafx.h"
#include "COMUSBSimulator.h"
#include "Duke.h"

CDuke::CDuke()
    {
    buttonUpDelay = 125;
    buttonDownDelay = 350;
    }
CDuke::~CDuke()
    {
    xid.SoftBreak(0);
    }

STDMETHODIMP CDuke::get_deviceName(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    *pVal = CreateBstrFromAnsi(xid.GetName());
    return S_OK;
    }

STDMETHODIMP CDuke::get_IP(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    char buffer[32];
    sprintf(buffer, "%u.%u.%u.%u", ((unsigned char*)&xid.simIP)[0], ((unsigned char*)&xid.simIP)[1], ((unsigned char*)&xid.simIP)[2], ((unsigned char*)&xid.simIP)[3]);
    *pVal = CreateBstrFromAnsi(buffer);
    return S_OK;
    }

STDMETHODIMP CDuke::put_IP(const BSTR newVal)
    {
    USES_CONVERSION;
    if(!newVal) return E_INVALIDARG;
    xid.simIP = inet_addr(W2A(newVal));
    return S_OK;
    }

STDMETHODIMP CDuke::get_port(short *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    *pVal = xid.usbPort;
    return S_OK;
    }

STDMETHODIMP CDuke::put_port(short newVal)
    {
    xid.usbPort = (char)newVal;
    xid.destPort = htons(newVal+SIM_NETPORT_BASE);
    return S_OK;
    }

STDMETHODIMP CDuke::get_enumerated(BOOL *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    if(xid.IsEnumerated()) *pVal = TRUE;
    else *pVal = FALSE;
    return S_OK;
    }

STDMETHODIMP CDuke::get_record(BOOL *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    *pVal = xid.record;
    return S_OK;
    }

STDMETHODIMP CDuke::put_record(BOOL newVal)
    {
    xid.record = !!newVal;
    return S_OK;
    }

STDMETHODIMP CDuke::Plug(unsigned long *errCode)
    {
    *errCode = xid.Plug();
    return S_OK;
    }

STDMETHODIMP CDuke::Unplug(unsigned long *errCode)
    {
    *errCode = xid.Unplug();
    return S_OK;
    }

STDMETHODIMP CDuke::EndpointMode(short mode)
    {
    if(mode<0 || mode>3) return E_INVALIDARG;
    xid.EndpointConfig(mode);
    return S_OK;
    }

STDMETHODIMP CDuke::Button(const BSTR buttonName)
    {
    XIDInputReport xidPacket = defaultXIDReport;
    XIDInputReport defaultPacket = defaultXIDReport;

    if(_wcsicmp(buttonName, L"UP") == 0)
        xidPacket.thumbLY = 32000;
    else if(_wcsicmp(buttonName, L"DOWN") == 0)
        xidPacket.thumbLY = -32000;
    else if(_wcsicmp(buttonName, L"LEFT") == 0)
        xidPacket.thumbLX = -32000;
    else if(_wcsicmp(buttonName, L"RIGHT") == 0)
        xidPacket.thumbLX = 32000;

    else if(_wcsicmp(buttonName, L"A") == 0)
        xidPacket.analogButtons[0] = 0xFF;
    else if(_wcsicmp(buttonName, L"B") == 0)
        xidPacket.analogButtons[1] = 0xFF;
    else if(_wcsicmp(buttonName, L"X") == 0)
        xidPacket.analogButtons[2] = 0xFF;
    else if(_wcsicmp(buttonName, L"Y") == 0)
        xidPacket.analogButtons[3] = 0xFF;
    else if(_wcsicmp(buttonName, L"BLACK") == 0)
        xidPacket.analogButtons[4] = 0xFF;
    else if(_wcsicmp(buttonName, L"WHITE") == 0)
        xidPacket.analogButtons[5] = 0xFF;
    else if(_wcsicmp(buttonName, L"LEFTTRIGGER") == 0)
        xidPacket.analogButtons[6] = 0xFF;
    else if(_wcsicmp(buttonName, L"RIGHTTRIGGER") == 0)
        xidPacket.analogButtons[7] = 0xFF;

    else if(_wcsicmp(buttonName, L"LEFTTHUMB") == 0)
        xidPacket.buttons = 0x80;
    else if(_wcsicmp(buttonName, L"RIGHTTHUMB") == 0)
        xidPacket.buttons = 0x40;
    else if(_wcsicmp(buttonName, L"BACK") == 0)
        xidPacket.buttons = 0x20;
    else if(_wcsicmp(buttonName, L"START") == 0)
        xidPacket.buttons = 0x10;
    else if(_wcsicmp(buttonName, L"DPADN") == 0)
        xidPacket.buttons = 0x01;
    else if(_wcsicmp(buttonName, L"DPADNE") == 0)
        xidPacket.buttons = 0x09;
    else if(_wcsicmp(buttonName, L"DPADE") == 0)
        xidPacket.buttons = 0x08;
    else if(_wcsicmp(buttonName, L"DPADSE") == 0)
        xidPacket.buttons = 0x0A;
    else if(_wcsicmp(buttonName, L"DPADS") == 0)
        xidPacket.buttons = 0x02;
    else if(_wcsicmp(buttonName, L"DPADSW") == 0)
        xidPacket.buttons = 0x06;
    else if(_wcsicmp(buttonName, L"DPADW") == 0)
        xidPacket.buttons = 0x04;
    else if(_wcsicmp(buttonName, L"DPADNW") == 0)
        xidPacket.buttons = 0x05;
    else
        {
        return E_INVALIDARG;
        }

    xid.SetInputReport(&xidPacket);
    Sleep(buttonDownDelay);
    xid.SetInputReport(&defaultPacket);
    Sleep(buttonUpDelay);

    return S_OK;
    }


STDMETHODIMP CDuke::get_buttonDownDelay(unsigned short *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    *pVal = buttonDownDelay;
    return S_OK;
    }

STDMETHODIMP CDuke::put_buttonDownDelay(unsigned short newVal)
    {
    buttonDownDelay = newVal;
    return S_OK;
    }

STDMETHODIMP CDuke::get_buttonUpDelay(unsigned short *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    *pVal = buttonUpDelay;
    return S_OK;
    }

STDMETHODIMP CDuke::put_buttonUpDelay(unsigned short newVal)
    {
    buttonUpDelay = newVal;
    return S_OK;
    }

STDMETHODIMP CDuke::SetButtonState(long digital, short analog1, short analog2, short analog3, short analog4, short analog5, short analog6, short analog7, short analog8, short thumbLX, short thumbLY, short thumbRX, short thumbRY)
    {
    XIDInputReport xidPacket = defaultXIDReport;

    xidPacket.buttons = (unsigned __int16)digital;
    xidPacket.analogButtons[0] = (unsigned __int8)analog1;
    xidPacket.analogButtons[1] = (unsigned __int8)analog2;
    xidPacket.analogButtons[2] = (unsigned __int8)analog3;
    xidPacket.analogButtons[3] = (unsigned __int8)analog4;
    xidPacket.analogButtons[4] = (unsigned __int8)analog5;
    xidPacket.analogButtons[5] = (unsigned __int8)analog6;
    xidPacket.analogButtons[6] = (unsigned __int8)analog7;
    xidPacket.analogButtons[7] = (unsigned __int8)analog8;
    xidPacket.thumbLX = thumbLX;
    xidPacket.thumbLY = thumbLY;
    xidPacket.thumbRX = thumbRX;
    xidPacket.thumbRY = thumbRY;

    xid.SetInputReport(&xidPacket);
    return S_OK;
    }

STDMETHODIMP CDuke::Free(void)
    {
    return S_OK;
    }

STDMETHODIMP CDuke::get_deviceDescriptor(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    xid.Lock();
    *pVal = CreateBstrFromAnsi((char*)xid.deviceDescriptor, 8);
    xid.Unlock();
    return S_OK;
    }

STDMETHODIMP CDuke::put_deviceDescriptor(BSTR newVal)
    {
    if(!newVal) return E_INVALIDARG;
    xid.Lock();
    for(unsigned i=0; i<8; i++)
        xid.deviceDescriptor[i] = (unsigned __int8)newVal[i];
    xid.Unlock();
    return S_OK;
    }

STDMETHODIMP CDuke::get_configurationDescriptor(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    xid.Lock();
    *pVal = CreateBstrFromAnsi((char*)xid.configurationDescriptor, 32);
    xid.Unlock();
    return S_OK;
    }

STDMETHODIMP CDuke::put_configurationDescriptor(BSTR newVal)
    {
    if(!newVal) return E_INVALIDARG;
    xid.Lock();
    for(unsigned i=0; i<32; i++)
        xid.configurationDescriptor[i] = (unsigned __int8)newVal[i];
    xid.Unlock();
    return S_OK;
    }

STDMETHODIMP CDuke::get_xidDescriptor(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    xid.Lock();
    *pVal = CreateBstrFromAnsi((char*)xid.xidDescriptor, xid.xidDescriptorLen);
    xid.Unlock();
    return S_OK;
    }

STDMETHODIMP CDuke::put_xidDescriptor(BSTR newVal)
    {
    if(!newVal) return E_INVALIDARG;
    xid.Lock();
    for(unsigned i=0; i<xid.xidDescriptorLen; i++)
        xid.xidDescriptor[i] = (unsigned __int8)newVal[i];
    xid.Unlock();
    return S_OK;
    }

STDMETHODIMP CDuke::get_xidPacket(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    xid.Lock();
    *pVal = CreateBstrFromAnsi((char*)xid.xidPacket, xid.xidPacketLen);
    xid.Unlock();
    return S_OK;
    }

STDMETHODIMP CDuke::put_xidPacket(BSTR newVal)
    {
    if(!newVal) return E_INVALIDARG;
    xid.Lock();
    for(unsigned i=0; i<xid.xidPacketLen; i++)
        xid.xidPacket[i] = (unsigned __int8)newVal[i];
    xid.Unlock();
    return S_OK;
    }

STDMETHODIMP CDuke::get_deviceCapabilities(BSTR type, BSTR *pVal)
    {
    if(!type || !pVal) return E_INVALIDARG;
    if(_wcsicmp(type, L"IN") == 0) // input - buttons
        {
        xid.Lock();
        *pVal = CreateBstrFromAnsi((char*)xid.inCapabilities, xid.inCapabilitiesLen);
        xid.Unlock();
        }
    else if(_wcsicmp(type, L"OUT") == 0) // output - motors
        {
        xid.Lock();
        *pVal = CreateBstrFromAnsi((char*)xid.outCapabilities, xid.outCapabilitiesLen);
        xid.Unlock();
        }
    else
        {
        return E_INVALIDARG;
        }
    return S_OK;
    }

STDMETHODIMP CDuke::put_deviceCapabilities(BSTR type, BSTR newVal)
    {
    if(!type || !newVal) return E_INVALIDARG;
    if(_wcsicmp(type, L"IN") == 0) // input - buttons
        {
        xid.Lock();
        for(unsigned i=0; i<xid.inCapabilitiesLen; i++)
            xid.inCapabilities[i] = (unsigned __int8)newVal[i];
        xid.Unlock();
        }
    else if(_wcsicmp(type, L"OUT") == 0) // output - motors
        {
        xid.Lock();
        for(unsigned i=0; i<xid.outCapabilitiesLen; i++)
            xid.outCapabilities[i] = (unsigned __int8)newVal[i];
        xid.Unlock();
        }
    else
        {
        return E_INVALIDARG;
        }
    return S_OK;
    }

STDMETHODIMP CDuke::get_xidPacketLength(unsigned short *pVal)
    {
    xid.Lock();
    *pVal = xid.xidPacketLen;
    xid.Unlock();

    return S_OK;
    }

STDMETHODIMP CDuke::put_xidPacketLength(unsigned short newVal)
    {
    xid.Lock();
    xid.xidPacketLen = newVal;
    delete[] xid.xidPacket;
    xid.xidPacket = new unsigned __int8[xid.xidPacketLen];
    xid.Unlock();

    return S_OK;
    }

STDMETHODIMP CDuke::get_xidDescriptorLength(unsigned short *pVal)
    {
    xid.Lock();
    *pVal = xid.xidDescriptorLen;
    xid.Unlock();

    return S_OK;
    }

STDMETHODIMP CDuke::put_xidDescriptorLength(unsigned short newVal)
    {
    xid.Lock();
    xid.xidDescriptorLen = newVal;
    delete[] xid.xidDescriptor;
    xid.xidDescriptor = new unsigned __int8[xid.xidDescriptorLen];
    xid.Unlock();

    return S_OK;
    }

STDMETHODIMP CDuke::get_deviceCapabilitiesLength(BSTR type, unsigned short *pVal)
    {
    if(!type || !pVal) return E_INVALIDARG;
    if(_wcsicmp(type, L"IN") == 0) // input - buttons
        {
        xid.Lock();
        *pVal = xid.inCapabilitiesLen;
        xid.Unlock();
        }
    else if(_wcsicmp(type, L"OUT") == 0) // output - motors
        {
        xid.Lock();
        *pVal = xid.outCapabilitiesLen;
        xid.Unlock();
        }
    else
        {
        return E_INVALIDARG;
        }
    return S_OK;
    }

STDMETHODIMP CDuke::put_deviceCapabilitiesLength(BSTR type, unsigned short newVal)
    {
    if(!type || !newVal) return E_INVALIDARG;
    if(_wcsicmp(type, L"IN") == 0) // input - buttons
        {
        xid.Lock();
        xid.inCapabilitiesLen = newVal;
        delete[] xid.inCapabilities;
        xid.inCapabilities = new unsigned __int8[xid.inCapabilitiesLen];
        xid.Unlock();
        }
    else if(_wcsicmp(type, L"OUT") == 0) // output - motors
        {
        xid.Lock();
        xid.outCapabilitiesLen = newVal;
        delete[] xid.outCapabilities;
        xid.outCapabilities = new unsigned __int8[xid.outCapabilitiesLen];
        xid.Unlock();
        }
    else
        {
        return E_INVALIDARG;
        }
    return S_OK;
    }
