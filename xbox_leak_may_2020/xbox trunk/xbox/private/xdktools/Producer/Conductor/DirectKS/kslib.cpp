// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation, 1996 - 2000.  All rights reserved.
//
// File Name:
//
//  kslib.cpp
//
// Abstract:
//
//  Implementation for exported functions and helpers
//      Exports:
//          KslRegisterLog
//          KslEnumFilters
//          KslInitKsLib
//          KslCloseKsLib
//
// -------------------------------------------------------------------------------

#include "stdafx.h"
#include "kslibp.h"

//
// statics
//
static LogStub s_LogStub;

//
// globals
//
HMODULE g_hSetupDLL     = NULL;
HMODULE g_hKsUserDLL    = NULL;
ILog*   s_iLog          = &s_LogStub;

//
// fxn Pointers
//
// setupapi functions
GETCLASSDEVS                fxnSetupDiGetClassDevs;
ENUMDEVICEINTERFACES        fxnSetupDiEnumDeviceInterfaces;
ENUMDEVICEINFO              fxnSetupDiEnumDeviceInfo;
GETDEVICEINTERFACEDETAIL    fxnSetupDiGetDeviceInterfaceDetail;
GETDEVICEREGISTRYPROPERTY   fxnSetupDiGetDeviceRegistryProperty;
OPENDEVICEINTERFACEREGKEY   fxnSetupDiOpenDeviceInterfaceRegKey;
GETDEVICEINTERFACEALIAS     fxnSetupDiGetDeviceInterfaceAlias;
DESTROYDEVICEINFOLIST       fxnSetupDiDestroyDeviceInfoList;

// ksuser functions
KSCREATEPIN                 fxnKsCreatePin;

//
// kslib functions
//
inline BOOL
IsEventSignaled(HANDLE hEvent)
{
    return (WAIT_OBJECT_0 == WaitForSingleObject((hEvent), 0));
}

// ------------------------------------------------------------------------

void 
PrepKSPinConnect
(
    PKSPIN_CONNECT_WFX  pConnect,
    DWORD               dwSampleRate, 
    DWORD               nChannels, 
    DWORD               nBits,
    BOOL                fLooped
)
{
    // set up pin connection parameters
    pConnect->Connect.PinToHandle = NULL;

    pConnect->Connect.Interface.Set = KSINTERFACESETID_Standard;
    pConnect->Connect.Interface.Id = fLooped ? KSINTERFACE_STANDARD_LOOPED_STREAMING : KSINTERFACE_STANDARD_STREAMING;
    pConnect->Connect.Interface.Flags = 0;

    pConnect->Connect.Medium.Set = KSMEDIUMSETID_Standard;
    pConnect->Connect.Medium.Id = KSMEDIUM_STANDARD_DEVIO;
    pConnect->Connect.Medium.Flags = 0;

    pConnect->Connect.Priority.PriorityClass = KSPRIORITY_NORMAL;
    pConnect->Connect.Priority.PrioritySubClass = 1;

    pConnect->Format.DataFormat.FormatSize = sizeof( KSDATAFORMAT_WAVEFORMATEX );//sizeof(pConnect->Format);
    pConnect->Format.DataFormat.Flags = 0;
    pConnect->Format.DataFormat.Reserved = 0;
    pConnect->Format.DataFormat.SampleSize = (USHORT)(nChannels*nBits/8);
    pConnect->Format.DataFormat.MajorFormat = KSDATAFORMAT_TYPE_AUDIO;
    pConnect->Format.DataFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    pConnect->Format.DataFormat.Specifier = KSDATAFORMAT_SPECIFIER_WAVEFORMATEX;

    pConnect->Format.WaveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
    pConnect->Format.WaveFormatEx.nChannels = (USHORT)nChannels;
    pConnect->Format.WaveFormatEx.nSamplesPerSec = dwSampleRate;
    pConnect->Format.WaveFormatEx.nAvgBytesPerSec = dwSampleRate*nChannels*nBits/8;
    pConnect->Format.WaveFormatEx.wBitsPerSample = (USHORT)nBits;
    pConnect->Format.WaveFormatEx.nBlockAlign = (USHORT)(nChannels*nBits/8);
    pConnect->Format.WaveFormatEx.cbSize = 0;
}


// ------------------------------------------------------------------------
void
KslRegisterLog
(
    ILog* iLog
)
{
    if (iLog)
        s_iLog = iLog;
}

// ------------------------------------------------------------------------
BOOL
SyncIoctl
(
    IN      HANDLE  handle,
    IN      ULONG   ulIoctl,
    IN      PVOID   pvInBuffer,
    IN      ULONG   ulInSize,
    OUT     PVOID   pvOutBuffer,
    IN      ULONG   ulOutSize,
    OUT     PULONG  pulBytesReturned
)
{
    OVERLAPPED  overlapped;
    BOOL        fRes = IsValidHandle(handle);
    ULONG       ulBytesReturned;

    if (!pulBytesReturned)
    {
        pulBytesReturned = &ulBytesReturned;
    }

    if (fRes)
    {
        memset(&overlapped,0,sizeof(overlapped));
        overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (!overlapped.hEvent)
        {
            LOG(eWarn2, "SyncIoctl: failed to create event\n");
            return FALSE;
        }

        fRes = 
            DeviceIoControl
            (
                handle, 
                ulIoctl, 
                pvInBuffer, 
                ulInSize, 
                pvOutBuffer, 
                ulOutSize, 
                pulBytesReturned, 
                &overlapped
            );

        if (!fRes)
        {
            DWORD dwError = GetLastError();

            if (ERROR_IO_PENDING == dwError) 
            {
                // put the 'sync' in 'syncioctl'
                fRes = (WAIT_OBJECT_0 == ::WaitForSingleObject(overlapped.hEvent, INFINITE));
            }
            else    // if this was a property call and the buffer size was zero, then we were 
            if      //  really asking for the size of buffer needed.
            (
                ((ERROR_INSUFFICIENT_BUFFER == dwError) || (ERROR_MORE_DATA == dwError)) && 
                (ulIoctl == IOCTL_KS_PROPERTY) && 
                (ulOutSize == 0)
            )
            {
                fRes = TRUE;
            }
#ifdef SUPERBLAB
            else
            {
                LOG(eBlab1, "DeviceIoControl failed.  IOCTL = %d", ulIoctl);
                s_iLog->LogStdErrorString(dwError);
            }
#endif
        }

        if (!fRes)
            *pulBytesReturned = 0;

        SafeCloseHandle(overlapped.hEvent);
    }

    return fRes;
}

#ifdef UNICODE
#define lstrtok     wcstok
#else
#define lstrtok     strtok
#endif

// --------------------------------------------------------------------------------------------------
// BinaryFromService
//  Given szDriver returns the binary name. szDriver is relative path to the registry.
// --------------------------------------------------------------------------------------------------
BOOL
BinaryFromDriver
(
    LPTSTR  szDriver,
    LPTSTR  szBinary
)
{
    BOOL                    fRes;
    char                    szRegKey[MAX_PATH];
    DWORD                   dwType = 0;
    DWORD                   dwDataSize;
    HKEY                    hKeyDriver = NULL;
    LONG                    lResult = ERROR_SUCCESS;

    fRes = (szDriver && szBinary && szDriver[0] != 0);
    
    // Find the OS.
    if (fRes)
    {
        OSVERSIONINFO osVersionInfo;

        ZeroMemory(&osVersionInfo, sizeof(osVersionInfo));
        
        osVersionInfo.dwOSVersionInfoSize = sizeof(osVersionInfo);
        GetVersionEx(&osVersionInfo);

        if ( (VER_PLATFORM_WIN32_WINDOWS == osVersionInfo.dwPlatformId) &&
             (0 < osVersionInfo.dwMinorVersion) )
        {
            strcpy(szRegKey, "SYSTEM\\CurrentControlSet\\Services\\Class\\");
        }
        else
        {
            strcpy(szRegKey, "SYSTEM\\CurrentControlSet\\Control\\Class\\");
        }

        strcat(szRegKey, (LPSTR) szDriver);
    }
    
    // Get other driver properties.
    if (fRes)
    {
        dwDataSize = MAX_PATH; 
        lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                               szRegKey,
                               0,
                               KEY_READ,
                               &hKeyDriver);
        if (ERROR_SUCCESS != lResult)
        {
            LOG(eWarn2, "Cannot open registry key %s", szRegKey); 
            fRes = FALSE;
        }
    }

    if (fRes)
    {
        dwDataSize = MAX_PATH; 
        lResult = RegQueryValueEx(hKeyDriver,
                                  "Driver",
                                  0,
                                  &dwType,
                                  (PBYTE) szBinary,
                                  &dwDataSize);
        if (ERROR_SUCCESS != lResult && ERROR_FILE_NOT_FOUND != lResult)
        {
            LOG(eWarn2, "Cannot read registry key Driver"); 
            fRes = FALSE;
        }
    }

    if (hKeyDriver)
    {
        RegCloseKey(hKeyDriver);
    }
    
    if (fRes)
    {
        CHAR    szPath[MAX_PATH];
        
        GetWindowsDirectory(szPath, MAX_PATH);
        lstrcat(szPath, "\\system32\\drivers\\");
        lstrcat(szPath, szBinary);

        strcpy(szBinary, szPath);
    }

    return fRes;
} // BinaryFromDriver

// ------------------------------------------------------------------------
// EnumFilters
//    Enumerate via the Setup APIs all filters of that are of type (all elements of) argpguidCategories.
// ------------------------------------------------------------------------
BOOL    
KslEnumFilters
(   
    CList<CKsFilter>*   plistFilters,
    ETechnology         eFilterType,
    LPGUID*             argpguidCategories,
    ULONG               cguidCategories,
    BOOL                fNeedPins,          // = TRUE // Enumerates devices for sysaudio.
    BOOL                fNeedNodes,         // = TRUE
    BOOL                fInstantiate        // = TRUE // Should we instantiate.
)
{
    BOOL        fRes = FALSE;
    HDEVINFO    hDevInfo = NULL;

    if (argpguidCategories && argpguidCategories[0] && !IsEqualGUIDAligned(GUID_NULL, *argpguidCategories[0]))
    {
        // Get a handle to the device set specified by the guid
        hDevInfo = 
            fxnSetupDiGetClassDevs
            (
                argpguidCategories[0], 
                NULL, 
                NULL, 
                DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
            );

        fRes = IsValidHandle(hDevInfo);
        if (!fRes)
        {
            LOG(eWarn2, "No devices of class KSCATEGORY_AUDIO found");
        }
        else
        {
            // Loop through members of the set and get details for each
            SP_DEVICE_INTERFACE_DETAIL_DATA*    pDevInterfaceDetails;
            int iClassMember;
            for(iClassMember = 0;;iClassMember++)
            {
                DWORD                               cbInterfaceDetails;
                TCHAR                               szFriendlyName[MAX_PATH];
                TCHAR                               szCLSID[MAX_PATH];
                TCHAR                               szService[MAX_PATH];
                TCHAR                               szBinary[MAX_PATH];
                TCHAR                               szDriver[MAX_PATH];
                DWORD                               cbData = sizeof(szFriendlyName);
                DWORD                               dwType = REG_SZ;
                BOOL                                fRes;
                HKEY                                hKey;
                SP_DEVICE_INTERFACE_DATA            DID;
                SP_DEVICE_INTERFACE_DATA            DIDAlias;
                SP_DEVINFO_DATA                     DevInfoData;
                SP_DEVINFO_DATA                     DeviceInfo;

                szFriendlyName[0] = 0;
                szCLSID[0] = 0;
                szService[0] = 0;
                szBinary[0] = 0;
                szDriver[0] = 0;

                DID.cbSize = sizeof(DID);
                DID.Reserved = 0;
                DIDAlias.cbSize = sizeof(DIDAlias);
                DIDAlias.Reserved = 0;

                DevInfoData.cbSize = sizeof(DevInfoData);
                DevInfoData.Reserved = 0;

                DeviceInfo.cbSize = sizeof(DeviceInfo);
                DeviceInfo.Reserved = 0;

                fRes = 
                    fxnSetupDiEnumDeviceInterfaces
                    (
                        hDevInfo, 
                        NULL, 
                        argpguidCategories[0], 
                        iClassMember,
                        &DID
                    );

                if (!fRes)
                    break;

                fRes = 
                    fxnSetupDiEnumDeviceInfo
                    (
                        hDevInfo,
                        iClassMember,
                        &DeviceInfo
                    );

                // Get details for the device registered in this class
                cbInterfaceDetails = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + MAX_PATH * sizeof(WCHAR);
                pDevInterfaceDetails = 
                    (SP_DEVICE_INTERFACE_DETAIL_DATA*)LocalAlloc(LPTR, cbInterfaceDetails);
                if (!pDevInterfaceDetails)
                {
                    LOG(eWarn2, "Failed to allocate SP_DEVICE_INTERFACE_DETAIL_DATA structure");
                    break;
                }

                pDevInterfaceDetails->cbSize = sizeof(*pDevInterfaceDetails);

                fRes = 
                    fxnSetupDiGetDeviceInterfaceDetail
                    (
                        hDevInfo, 
                        &DID, 
                        pDevInterfaceDetails, 
                        cbInterfaceDetails,
                        NULL, 
                        &DevInfoData
                    );

                if (fRes)
                {
                    // Get more details about the device
                    hKey = 
                        fxnSetupDiOpenDeviceInterfaceRegKey
                        (
                            hDevInfo, 
                            &DID, 
                            0, 
                            KEY_READ
                        );

                    if (IsValidHandle(hKey)) 
                    {
                        // Get the friendly name of the device
                        RegQueryValueEx
                        (
                            hKey, 
                            TEXT("FriendlyName"), 
                            0, 
                            &dwType,
                            (PBYTE)szFriendlyName, 
                            &cbData
                        );

                        // Get the friendly name of the device
                        RegQueryValueEx
                        (
                            hKey, 
                            TEXT("CLSID"), 
                            0, 
                            &dwType,
                            (PBYTE)szCLSID, 
                            &cbData
                        );

                        // Get the name of the service
                        cbData = MAX_PATH;
                        fxnSetupDiGetDeviceRegistryProperty
                        (
                            hDevInfo, 
                            &DevInfoData, 
                            SPDRP_SERVICE, 
                            NULL, 
                            (PBYTE)szService, 
                            cbData, 
                            &cbData
                        );

                        // Get the name of the service
                        cbData = MAX_PATH;
                        fxnSetupDiGetDeviceRegistryProperty
                        (
                            hDevInfo, 
                            &DevInfoData, 
                            SPDRP_DRIVER, 
                            NULL, 
                            (PBYTE)szDriver, 
                            cbData, 
                            &cbData
                        );

                        BinaryFromDriver(szDriver, szBinary);

                        RegCloseKey(hKey);
                    }

                    // 
                    // check additional category guids which may (or may not) have been supplied
                    //
                    for(ULONG nguidCategory = 1; nguidCategory < cguidCategories && fRes; nguidCategory++)
                    {
                        fRes = 
                            fxnSetupDiGetDeviceInterfaceAlias
                            (
                                hDevInfo,
                                &DID,
                                argpguidCategories[nguidCategory],
                                &DIDAlias
                            );

                        if (!fRes && s_iLog)  LOG(eWarn3, "Failed to get requested DeviceInterfaceAlias");

                        //
                        // Check if the this interface alias is enabled.
                        //
                        if (fRes)
                        {
                            if (!DIDAlias.Flags || (DIDAlias.Flags & SPINT_REMOVED))
                            {
                                fRes = FALSE;
                                if (s_iLog)
                                {
                                    LOG(eWarn3, "DeviceInterfaceAlias disabled.");
                                }
                            }
                        }
                    }

                    if (fRes)
                    {
                        CKsFilter* pnewFilter = NULL;

                        switch(eFilterType)
                        {
                            case eUnknown:
                                pnewFilter = new CKsFilter(pDevInterfaceDetails->DevicePath, szFriendlyName, szCLSID, szService, szBinary);
                                break;

                            case eTopology:
                                pnewFilter = new CTopologyFilter(pDevInterfaceDetails->DevicePath, szFriendlyName, szCLSID, szService, szBinary);
                                break;

                            case ePCMAudio:
                                pnewFilter = new CPCMAudioFilter(pDevInterfaceDetails->DevicePath, szFriendlyName, szCLSID, szService, szBinary);
                                break;

                            case eSysAudioDevice:
                                pnewFilter = new CSysAudioDeviceFilter(pDevInterfaceDetails->DevicePath, szFriendlyName, szCLSID, szService, szBinary);
                                break;

                            case eSysAudio:
                                pnewFilter = new CSysAudio(pDevInterfaceDetails->DevicePath, szFriendlyName, szCLSID, szService, szBinary);
                                break;

/*
                            case eMidi:
                                pnewFilter = new CPCMAudioFilter(pDevInterfaceDetails->DevicePath, szFriendlyName, szCLSID);
                                break;

                            case eDMusic:
                                pnewFilter = new CDMusicFilter(pDevInterfaceDetails->DevicePath, szFriendlyName, szCLSID);
                                break;
*/
                            default:
                                pnewFilter = new CKsFilter(pDevInterfaceDetails->DevicePath, szFriendlyName, szCLSID, szService, szBinary);
                                break;
                        }
                        
                        if (pnewFilter)
                        {
                            BOOL fResParse = TRUE;

                            // Device Instance Handle (DEVNODE) might be very useful
                            // for several purposes.
                            pnewFilter->m_dwDevInst = DevInfoData.DevInst;

                            if (fNeedNodes || fNeedPins || fInstantiate)
                            {
                                fResParse = pnewFilter->Instantiate();

                                if (fNeedNodes && fResParse)
                                    fResParse = pnewFilter->EnumerateNodes();
                            
                                // This enumerates Devices for SysAudio devices
                                if (fNeedPins && fResParse)
                                    fResParse = pnewFilter->EnumeratePins();
                            }

                            if (fResParse)
                                plistFilters->AddTail(pnewFilter);
                            else
                                delete pnewFilter;
                        }
                        else
                        {
                            LOG(
                                eFatalError, 
                                "Failed to create CIrpTarget for %s", 
                                pDevInterfaceDetails->DevicePath);
                        }
                    }
                }

                SafeLocalFree(pDevInterfaceDetails);
            } // for
        }
    }

    if (IsValidHandle(hDevInfo))
        fxnSetupDiDestroyDeviceInfoList(hDevInfo);

    return fRes;
} // KslEnumFilters

// ----------------------------------------------------------------------------------
// KslLogEnumResults
//  logs results of EnumFilters
// ----------------------------------------------------------------------------------
void
KslLogEnumResults
(
    CList<CKsFilter>*   plistFilters,
    CKsFilter*          pFilterDefault     // optional
)
{
    // log enum results
    LOG(eInfo2, "  Found %d viable filters:", plistFilters->GetCount());
    if (!plistFilters->IsEmpty())
    {
        CNode<CKsFilter>* pNodeFilter;
        CNode<CKsPin>*    pNodePin;            
        
        pNodeFilter = plistFilters->GetHead();
        while(pNodeFilter)
        {
            CKsFilter* pFilter = pNodeFilter->pData;
            pNodeFilter = plistFilters->GetNext(pNodeFilter);

            ASSERT(pFilter);
            if (pFilter == pFilterDefault)
                LOG(eInfo2, "*     %s", pFilter->m_szFilterName);
            else
                LOG(eInfo2, "      %s", pFilter->m_szFilterName);

            LOG(eInfo2, "    Viable Render Sink Pins:");
            pNodePin = pFilter->m_listRenderSinkPins.GetHead();
            while(pNodePin)
            {
                CKsPin* pPin = pNodePin->pData;
                pNodePin = pFilter->m_listRenderSinkPins.GetNext(pNodePin);

                if (pFilterDefault && pPin == pFilterDefault->m_pRenderPin)
                    LOG(eInfo2, "*     #%d", pPin->m_nId);
                else
                    LOG(eInfo2, "      #%d", pPin->m_nId);
            }

            LOG(eInfo2, "    Viable Render Source Pins:");
            pNodePin = pFilter->m_listRenderSourcePins.GetHead();
            while(pNodePin)
            {
                CKsPin* pPin = pNodePin->pData;
                pNodePin = pFilter->m_listRenderSourcePins.GetNext(pNodePin);

                if (pFilterDefault && pPin == pFilterDefault->m_pRenderPin)
                    LOG(eInfo2, "*     #%d", pPin->m_nId);
                else
                    LOG(eInfo2, "      #%d", pPin->m_nId);
            }
            
            LOG(eInfo2, "    Viable Capture Pins:");
            pNodePin = pFilter->m_listCaptureSinkPins.GetHead();
            while(pNodePin)
            {
                CKsPin* pPin = pNodePin->pData;
                pNodePin = pFilter->m_listCaptureSinkPins.GetNext(pNodePin);

                if (pFilterDefault && pPin == pFilterDefault->m_pCapturePin)
                    LOG(eInfo2, "*     #%d", pPin->m_nId);
                else
                    LOG(eInfo2, "      #%d", pPin->m_nId);
            }

            LOG(eInfo2, "    Viable Capture Pins:");
            pNodePin = pFilter->m_listCaptureSourcePins.GetHead();
            while(pNodePin)
            {
                CKsPin* pPin = pNodePin->pData;
                pNodePin = pFilter->m_listCaptureSourcePins.GetNext(pNodePin);

                if (pFilterDefault && pPin == pFilterDefault->m_pCapturePin)
                    LOG(eInfo2, "*     #%d", pPin->m_nId);
                else
                    LOG(eInfo2, "      #%d", pPin->m_nId);
            }
        }
    }

    LOG(eInfo2, "\n\n");
}

// ------------------------------------------------------------------------------
ULONG
CalculateFrameSize
(
    ULONG   nFrame,
    ULONG   cFrames,
    ULONG   cbBuffer
)
{
    if (cbBuffer % cFrames)
    {
        // if this is the last buffer, then pick up the remainder
        if (nFrame == (cFrames - 1))
        {
            return (cbBuffer % (cFrames - 1));
        }
        // otherwise, round down
        else
        {
            return (cbBuffer / (cFrames - 1));
        }

    }
    else
    {
        return (cbBuffer / cFrames);
    }
}
    
// ----------------------------------------------------------------------------------
// KslInitKsLib
//  Loads functions from SETUPAPI.dll
// ----------------------------------------------------------------------------------
BOOL
KslInitKsLib()
{
    BOOL fRes;

    g_hSetupDLL = LoadLibrary("setupapi.dll");
    g_hKsUserDLL = LoadLibrary("ksuser.dll");

    fRes = IsValidHandle(g_hSetupDLL) && IsValidHandle(g_hKsUserDLL);

    if (fRes)
    {
#ifdef UNICODE
        fxnSetupDiGetClassDevs = (GETCLASSDEVS)
            GetProcAddress(g_hSetupDLL, "SetupDiGetClassDevsW");

        fxnSetupDiGetDeviceInterfaceDetail = (GETDEVICEINTERFACEDETAIL)
            GetProcAddress(g_hSetupDLL, "SetupDiGetDeviceInterfaceDetailW");

        fxnSetupDiGetDeviceRegistryProperty = (GETDEVICEREGISTRYPROPERTY)
            GetProcAddress(g_hSetupDLL, "SetupDiGetDeviceRegistryPropertyW");
#else
        fxnSetupDiGetClassDevs = (GETCLASSDEVS)
            GetProcAddress(g_hSetupDLL, "SetupDiGetClassDevsA");

        fxnSetupDiGetDeviceInterfaceDetail = (GETDEVICEINTERFACEDETAIL)
            GetProcAddress(g_hSetupDLL, "SetupDiGetDeviceInterfaceDetailA");

        fxnSetupDiGetDeviceRegistryProperty = (GETDEVICEREGISTRYPROPERTY)
            GetProcAddress(g_hSetupDLL, "SetupDiGetDeviceRegistryPropertyA");
#endif

        fxnSetupDiEnumDeviceInterfaces = (ENUMDEVICEINTERFACES)
            GetProcAddress(g_hSetupDLL, "SetupDiEnumDeviceInterfaces");

        fxnSetupDiEnumDeviceInfo = (ENUMDEVICEINFO)
            GetProcAddress(g_hSetupDLL, "SetupDiEnumDeviceInfo");

        fxnSetupDiOpenDeviceInterfaceRegKey = (OPENDEVICEINTERFACEREGKEY)
            GetProcAddress(g_hSetupDLL, "SetupDiOpenDeviceInterfaceRegKey");

        fxnSetupDiGetDeviceInterfaceAlias = (GETDEVICEINTERFACEALIAS)
            GetProcAddress(g_hSetupDLL, "SetupDiGetDeviceInterfaceAlias");

        fxnSetupDiDestroyDeviceInfoList = (DESTROYDEVICEINFOLIST)
            GetProcAddress(g_hSetupDLL, "SetupDiDestroyDeviceInfoList");

        fxnKsCreatePin = (KSCREATEPIN)
            GetProcAddress(g_hKsUserDLL, "KsCreatePin");

        fRes = 
            fxnSetupDiEnumDeviceInfo &&
            fxnSetupDiGetClassDevs &&
            fxnSetupDiGetDeviceInterfaceDetail &&
            fxnSetupDiGetDeviceRegistryProperty &&
            fxnSetupDiGetClassDevs &&
            fxnSetupDiGetDeviceInterfaceDetail &&
            fxnSetupDiGetDeviceRegistryProperty &&
            fxnSetupDiEnumDeviceInterfaces &&
            fxnSetupDiOpenDeviceInterfaceRegKey &&
            fxnSetupDiGetDeviceInterfaceAlias &&
            fxnSetupDiDestroyDeviceInfoList &&
            fxnKsCreatePin;
    }

    return fRes;
} // KslInitKsLib

// ----------------------------------------------------------------------------------
// KslCloseKsLib
//  Unloads
// ----------------------------------------------------------------------------------
BOOL
KslCloseKsLib()
{
    if (g_hSetupDLL)
        FreeLibrary(g_hSetupDLL);

    if (g_hKsUserDLL)
        FreeLibrary(g_hKsUserDLL);

    return TRUE;
} // KslCloseKsLib

