//==============================================================================
// Copyright (c) 2000 Microsoft Corporation.  All rights reserved.
//
// Module Name:
//	sysaud.cpp
//
// Abstract:
//  Class implementation of CSysAudio.
//
//==============================================================================

#include "stdafx.h"
#include "kslibp.h"

// ----------------------------------------------------------------------------------
// CSysAudio::CSysAudio
// ----------------------------------------------------------------------------------
CSysAudio::CSysAudio
(
    LPCTSTR  pszName,
    LPCTSTR  pszFriendlyName,
    LPCTSTR  pszCLSID,
    LPCTSTR  pszService, 
    LPCTSTR  pszBinary
) : CKsFilter(pszName, pszFriendlyName, pszCLSID, pszService, pszBinary),
    m_ulDeviceCount(0)
{
    m_eType = eSysAudio;
}

// ----------------------------------------------------------------------------------
// CSysAudio::~CSysAudio
// ----------------------------------------------------------------------------------
CSysAudio::~CSysAudio()
{
    DestroyLists();
} // ~CSysAudio

// ----------------------------------------------------------------------------------
// CSysAudio::DestroyLists
// ----------------------------------------------------------------------------------
void CSysAudio::DestroyLists()
{
    CKsFilter::DestroyLists();

    CNode <CSysAudioDeviceFilter> *pNode;

    pNode = m_listDevices.GetHead();
    while (pNode)
    {
        delete pNode->pData;
        
        pNode = m_listDevices.GetNext(pNode);
    }

    m_listDevices.Empty();
} // DestroyLists

// ----------------------------------------------------------------------------------
// CSysAudio::EnumeratePins
// This will actually create a list of sysaudio devices.
// ----------------------------------------------------------------------------------
BOOL CSysAudio::EnumeratePins()
{
    BOOL fResult = TRUE;

    //
    // Get number of devices for this filter.
    //
    if (TRUE == fResult)
    {
        fResult = 
            GetPropertySimple
            (
                KSPROPSETID_Sysaudio,
                KSPROPERTY_SYSAUDIO_DEVICE_COUNT,
                &m_ulDeviceCount,
                sizeof(m_ulDeviceCount)
            );
        if (TRUE != fResult)
        {
            LOG(eWarn1, "Cannot get device count from SysAudio");
        }
    }

    //
    // Create a list of devices.
    //
    if (TRUE == fResult)
    {
        WCHAR szDeviceInterfaceName[MAX_PATH];
        WCHAR szDeviceFriendlyName[MAX_PATH];
        CHAR  szInterfaceName[MAX_PATH];
        CHAR  szFriendlyName[MAX_PATH];


        for (ULONG ulDeviceId = 0; ulDeviceId < m_ulDeviceCount; ulDeviceId++)
        {
            //
            // Set current device instance.
            //
            fResult = 
                SetDevicePropertySimple
                (
                    ulDeviceId,
                    KSPROPSETID_Sysaudio,
                    KSPROPERTY_SYSAUDIO_DEVICE_INSTANCE,
                    NULL,
                    0
                );
            
            //
            // Get sysaudio device properties.
            //
            fResult =
                GetDevicePropertySimple
                (
                    ulDeviceId,
                    KSPROPSETID_Sysaudio,
                    KSPROPERTY_SYSAUDIO_DEVICE_INTERFACE_NAME,
                    szDeviceInterfaceName,
                    MAX_PATH * sizeof(WCHAR)
                );
            if (fResult)
            {
                fResult = 
                    GetDevicePropertySimple
                    (
                        ulDeviceId,
                        KSPROPSETID_Sysaudio,
                        KSPROPERTY_SYSAUDIO_DEVICE_FRIENDLY_NAME,
                        szDeviceFriendlyName,
                        MAX_PATH * sizeof(WCHAR)
                    );
            }

            //
            // Create the device if all is good.
            //
            PCSysAudioDeviceFilter pDevice = NULL;
            if (fResult)
            {
                WideCharToMultiByte
                    (
                        CP_ACP, 
                        0, 
                        szDeviceInterfaceName,
                        MAX_PATH,
                        szInterfaceName,
                        MAX_PATH,
                        NULL,
                        NULL
                    );

                WideCharToMultiByte
                    (
                        CP_ACP, 
                        0, 
                        szDeviceFriendlyName,
                        MAX_PATH,
                        szFriendlyName,
                        MAX_PATH,
                        NULL,
                        NULL
                    );

                pDevice =     
                    new CSysAudioDeviceFilter
                        (
                            szInterfaceName,
                            szFriendlyName
                        );
                if (!pDevice)
                {
                    fResult = FALSE;
                }
            }

            // 
            // Set device properties and add it to device list.
            //
            if (fResult)
            {
                pDevice->m_DeviceID = ulDeviceId;

                m_listDevices.AddTail(pDevice);
            }
        }
    }

    return fResult;
} // EnumeratePins

// ----------------------------------------------------------------------------------
// CSysAudio::GetDevicePropertySimple
// ----------------------------------------------------------------------------------
BOOL
CSysAudio::GetDevicePropertySimple
(
    IN  ULONG               ulDeviceId,
    IN  REFGUID             guidPropertySet,
    IN  ULONG               nProperty,
    OUT PVOID               pvDest,
    OUT ULONG               cbDest
)
{
    ULONG           ulReturned = 0;
    KSP_SYSAUDIO    KsSysAudioProp;

    KsSysAudioProp.Property.Set = guidPropertySet;
    KsSysAudioProp.Property.Id = nProperty;
    KsSysAudioProp.Property.Flags = KSPROPERTY_TYPE_GET;
    KsSysAudioProp.DeviceId = ulDeviceId;

    return 
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &KsSysAudioProp,
            sizeof(KSP_PIN),
            pvDest,
            cbDest,
            &ulReturned
        );
} // GetDevicePropertySimple

BOOL
CSysAudio::SetDevicePropertySimple
(
    IN  ULONG               ulDeviceId,
    IN  REFGUID             guidPropertySet,
    IN  ULONG               nProperty,
    OUT PVOID               pvDest,
    OUT ULONG               cbDest
)
{
    ULONG           ulReturned = 0;
    KSP_SYSAUDIO    KsSysAudioProp;

    KsSysAudioProp.Property.Set = guidPropertySet;
    KsSysAudioProp.Property.Id = nProperty;
    KsSysAudioProp.Property.Flags = KSPROPERTY_TYPE_SET;
    KsSysAudioProp.DeviceId = ulDeviceId;

    return 
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &KsSysAudioProp,
            sizeof(KSP_PIN),
            pvDest,
            cbDest,
            &ulReturned
        );
} // SetDevicePropertySimple

