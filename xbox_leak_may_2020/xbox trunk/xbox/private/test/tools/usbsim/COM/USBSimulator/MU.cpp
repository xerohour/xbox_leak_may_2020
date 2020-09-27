/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    MU.cpp

Abstract:

    COM object wrapper for the Bulk USB Device class

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/
#include "stdafx.h"
#include "COMUSBSimulator.h"
#include "MU.h"

CMU::CMU()
    {
    }
CMU::~CMU()
    {
    bulk.SoftBreak(0);
    }

STDMETHODIMP CMU::get_deviceName(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    *pVal = CreateBstrFromAnsi(bulk.GetName());
    return S_OK;
    }

STDMETHODIMP CMU::get_IP(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    char buffer[32];
    sprintf(buffer, "%u.%u.%u.%u", ((unsigned char*)&bulk.simIP)[0], ((unsigned char*)&bulk.simIP)[1], ((unsigned char*)&bulk.simIP)[2], ((unsigned char*)&bulk.simIP)[3]);
    *pVal = CreateBstrFromAnsi(buffer);
    return S_OK;
    }

STDMETHODIMP CMU::put_IP(const BSTR newVal)
    {
    USES_CONVERSION;
    if(!newVal) return E_INVALIDARG;
    bulk.simIP = inet_addr(W2A(newVal));
    return S_OK;
    }

STDMETHODIMP CMU::get_port(short *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    *pVal = bulk.usbPort;
    return S_OK;
    }

STDMETHODIMP CMU::put_port(short newVal)
    {
    bulk.usbPort = (char)newVal;
    bulk.destPort = htons(newVal+SIM_NETPORT_BASE);
    return S_OK;
    }

STDMETHODIMP CMU::get_enumerated(BOOL *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    if(bulk.IsEnumerated()) *pVal = TRUE;
    else *pVal = FALSE;
    return S_OK;
    }

STDMETHODIMP CMU::get_record(BOOL *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    *pVal = bulk.record;
    return S_OK;
    }

STDMETHODIMP CMU::put_record(BOOL newVal)
    {
    bulk.record = !!newVal;
    return S_OK;
    }

STDMETHODIMP CMU::Plug(void)
    {
    bulk.Plug();
    return S_OK;
    }

STDMETHODIMP CMU::Unplug(void)
    {
    bulk.Unplug();
    return S_OK;
    }

STDMETHODIMP CMU::EndpointMode(short mode)
    {
    if(mode<0 || mode>3) return E_INVALIDARG;
    bulk.EndpointConfig(mode);
    return S_OK;
    }

STDMETHODIMP CMU::SetCapacity(unsigned long numBlocks, unsigned short blockSize, unsigned short logicalSize /*=0*/)
    {
    bulk.SetCapacity(numBlocks, blockSize, logicalSize);
    return S_OK;
    }

STDMETHODIMP CMU::FakeCapacity(unsigned long numBlocks, unsigned short blockSize, unsigned short logicalSize)
    {
    bulk.SetCapacity(0x3EFF, 0x0200, 0); // 8megs
    bulk.capacity.numBlocks = numBlocks;
    bulk.capacity.blockSize = blockSize;
    bulk.capacity.logicalSize = logicalSize;

    return S_OK;
    }

STDMETHODIMP CMU::LoadMU(const BSTR filename, unsigned long *pErr)
    {
    USES_CONVERSION;
    if(!filename) return E_INVALIDARG;
    if(!pErr) return E_INVALIDARG;

    *pErr = bulk.LoadMU(W2A(filename));

    return S_OK;
    }

STDMETHODIMP CMU::StoreMU(const BSTR filename, unsigned long *pErr)
    {
    USES_CONVERSION;
    if(!filename) return E_INVALIDARG;
    if(!pErr) return E_INVALIDARG;

    *pErr = bulk.StoreMU(W2A(filename));

    return S_OK;
    }

STDMETHODIMP CMU::Free(void)
    {
    Lock();
    delete[] bulk.storage;
    bulk.storage = NULL;
    Unlock();

    return S_OK;
    }

STDMETHODIMP CMU::Unformat(void)
    {
    Lock();
    memset(bulk.storage, 0xFF, (DWORD)bulk.capacity.numBlocks * (DWORD)bulk.capacity.blockSize);
    Unlock();

    return S_OK;
    }

STDMETHODIMP CMU::Format()
    {
    if(((DWORD)bulk.capacity.numBlocks * (DWORD)bulk.capacity.blockSize) < 8192) return S_OK;

    bulk.Lock();
    memset(bulk.storage, 0xFF, 4096);
    memset(bulk.storage+4096, 0x00, 4096);

    // FATX header
    memcpy(bulk.storage, "\x46\x41\x54\x58\x10\xB9\xD7\xBF\x04\x00\x00\x00\x01\x00\x00\x00\x00\x00", 18);

    // serial number
    bulk.storage[0x0004] = rand()%256;
    bulk.storage[0x0005] = rand()%256;
    bulk.storage[0x0006] = rand()%256;
    bulk.storage[0x0007] = rand()%256;

    bulk.storage[0x1000] = 0xF8;
    bulk.storage[0x1001] = 0xFF;
    bulk.storage[0x1002] = 0xFF;
    bulk.storage[0x1003] = 0xFF;
    bulk.Unlock();

    return S_OK;
    }


STDMETHODIMP CMU::get_data(unsigned long offset, unsigned long length, BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    if(offset + length >= (DWORD)bulk.capacity.numBlocks * (DWORD)bulk.capacity.blockSize) return E_INVALIDARG;
    bulk.Lock();
    *pVal = CreateBstrFromAnsi((char*)bulk.storage+offset, length);
    bulk.Unlock();
    return S_OK;
    }

STDMETHODIMP CMU::put_data(unsigned long offset, unsigned long length, BSTR newVal)
    {
    if(!newVal) return E_INVALIDARG;
    if(offset + length >= (DWORD)bulk.capacity.numBlocks * (DWORD)bulk.capacity.blockSize) return E_INVALIDARG;
    bulk.Lock();
    for(unsigned long i=0; i<length; i++)
        bulk.storage[offset+i] = (unsigned __int8)newVal[i];
    bulk.Unlock();
    return S_OK;
    }

STDMETHODIMP CMU::get_deviceDescriptor(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    bulk.Lock();
    *pVal = CreateBstrFromAnsi((char*)bulk.deviceDescriptor, 8);
    bulk.Unlock();
    return S_OK;
    }

STDMETHODIMP CMU::put_deviceDescriptor(BSTR newVal)
    {
    if(!newVal) return E_INVALIDARG;
    bulk.Lock();
    for(unsigned i=0; i<8; i++)
        bulk.deviceDescriptor[i] = (unsigned __int8)newVal[i];
    bulk.Unlock();
    return S_OK;
    }

STDMETHODIMP CMU::get_configurationDescriptor(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;
    bulk.Lock();
    *pVal = CreateBstrFromAnsi((char*)bulk.configurationDescriptor, 32);
    bulk.Unlock();
    return S_OK;
    }

STDMETHODIMP CMU::put_configurationDescriptor(BSTR newVal)
    {
    if(!newVal) return E_INVALIDARG;
    bulk.Lock();
    for(unsigned i=0; i<32; i++)
        bulk.configurationDescriptor[i] = (unsigned __int8)newVal[i];
    bulk.Unlock();
    return S_OK;
    }

