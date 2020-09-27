/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    Simulator.cpp

Abstract:

    COM wrapper object for the USBSimulator class

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/
#include "stdafx.h"
#include "COMUSBSimulator.h"
#include "Simulator.h"

CSimulator::CSimulator()
    {
    }
CSimulator::~CSimulator()
    {
    }

STDMETHODIMP CSimulator::GetVersion(const BSTR ipAddr, BSTR *version)
    {
    USES_CONVERSION;
    if(!version) return E_INVALIDARG;

    usbsim.ip[0] = inet_addr(W2A(ipAddr));

    SimPacketTyped<unsigned char [128]> simData;

    usbsim.SendCommand(0, SIM_CMD_IPQUERY, SIM_SUBCMD_IPQUERY, simData);

    if(usbsim.SendCommand(0, SIM_CMD_STATUS, SIM_SUBCMD_STATUS_VERSION, simData) != USBSIM_ERROR_OK)
        {
        *version = CreateBstrFromAnsi("");
        return S_OK;
        }
    simData.data[simData.header.dataSize] = '\0';
    *version = CreateBstrFromAnsi((char*)simData.data);

    return S_OK;
    }

STDMETHODIMP CSimulator::GetMACAddr(const BSTR ipAddr, BSTR *macAddr)
    {
    USES_CONVERSION;
    if(!macAddr) return E_INVALIDARG;

    char buffer[128];
    usbsim.ip[0] = inet_addr(W2A(ipAddr));

    SimPacketTyped<unsigned char [128]> simData;
    if(usbsim.SendCommand(0, SIM_CMD_STATUS, SIM_SUBCMD_STATUS_MACADDR, simData) != USBSIM_ERROR_OK)
        {
        *macAddr = CreateBstrFromAnsi("");
        return S_OK;
        }
    sprintf(buffer, "%02X %02X %02X %02X %02X %02X", simData.data[0], simData.data[1], simData.data[2], simData.data[3], simData.data[4], simData.data[5]);
    *macAddr = CreateBstrFromAnsi((char*)buffer);

    return S_OK;
    }

STDMETHODIMP CSimulator::GetUSBPorts(const BSTR ipAddr, short *connectedMask)
    {
    USES_CONVERSION;

    if(!connectedMask) return E_INVALIDARG;

    usbsim.ip[0] = inet_addr(W2A(ipAddr));

    SimPacketTyped<unsigned __int8> simData;
    if(usbsim.SendCommand(0, SIM_CMD_STATUS, SIM_SUBCMD_STATUS_CONNECTED, simData) != USBSIM_ERROR_OK)
        {
        *connectedMask = 0;
        return S_OK;
        }
    *connectedMask = simData.data;

    return S_OK;
    }

STDMETHODIMP CSimulator::Log(const BSTR string)
    {
    recorder.LogPrint("%ws", string);
    return S_OK;
    }

STDMETHODIMP CSimulator::LogFormat(short format)
    {
    recorder.SetFormat(format);
    return S_OK;
    }

STDMETHODIMP CSimulator::GetErrorDescription(unsigned long errCode, BSTR *pDescription)
    {
    if(!pDescription) return E_INVALIDARG;

    char *descriptions[] =
        {
        "Success",
        "Device is already connected",
        "Device is not connected",
        "Connection failed, use GetLastError to get more information",
        "Socket error, use GetLastError to get more information",
        "Parameter is out of range or denotes a port currently not in use",
        "Parameter is out of range or denotes a simulator currently not in use",
        "Pointer to a USBDevice is not valid",
        "Abort specifed by the user",
        "Timeout waiting for response from simulator",
        "Got data from the simulator but it is likely corrupted",
        "Send failure, use GetLastError to get more information"
        };

    if(errCode >= ARRAYSIZE(descriptions))
        {
        *pDescription = CreateBstrFromAnsi("Unknown Error Code");
        return S_OK;
        }

    *pDescription = CreateBstrFromAnsi(descriptions[errCode]);    

    return S_OK;
    }

STDMETHODIMP CSimulator::get_logFilename(BSTR *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = CreateBstrFromAnsi(recorder.filename);    

    return S_OK;
    }

STDMETHODIMP CSimulator::put_logFilename(const BSTR newVal)
    {
    USES_CONVERSION;
    if(!newVal) return E_INVALIDARG;

    recorder.SetFilename(W2A(newVal));

    return S_OK;
    }

STDMETHODIMP CSimulator::GetLastError(unsigned long *pVal)
    {
    if(!pVal) return E_INVALIDARG;

    *pVal = ::GetLastError();

    return S_OK;
    }
