/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
    Temporary module implementing some logon and service stuff

Module Name:

    service.cpp

--*/

#include "xonp.h"
#include "xonver.h"

#define XONLINE_SERVICE_OVERRIDE_FILE "\\Device\\harddisk0\\Partition1\\devkit\\xonline.ini"

#define MAX_CONFIG_SIZE        4096

HRESULT CXo::_XOnlineOverrideInfo(LPSTR pszXOnlineIniFile)
{
#ifdef XONLINE_FEATURE_INSECURE
    return InitOverrideInfo( pszXOnlineIniFile );
#else
    return E_NOTIMPL;
#endif
}

#ifdef XONLINE_FEATURE_INSECURE

HRESULT CXo::InitOverrideInfo(LPSTR pszXOnlineIniFile)
{
    #define NOT_KDC 0
    #define IS_PKDC 1
    #define IS_XKDC 2
    #define IS_MKDC 3
    
    HRESULT          hr;
    NTSTATUS         Status;
    OBJECT_ATTRIBUTES Obja;
    OBJECT_STRING    FileName;
    IO_STATUS_BLOCK IoStatusBlock;
    HANDLE           hFile = INVALID_HANDLE_VALUE;
    PBYTE            pbAlloc = NULL;
    PCHAR            pcBuffer;
    PCHAR            pcBufferEnd;
    DWORD            dwSize;
    DWORD            dwWhichKDC;
    PCHAR            pcFirstString;
    PCHAR            pcIP;
    PCHAR            pcPort;
    PCHAR            pcCR;
    DWORD            dwServiceID;
    DWORD            len;
    DWORD            i;

    m_ctServiceInfoOverride = 0;
    
    //
    // Open the configuration ini file
    //
    RtlInitObjectString( &FileName, (pszXOnlineIniFile == NULL ? XONLINE_SERVICE_OVERRIDE_FILE : pszXOnlineIniFile));

    InitializeObjectAttributes(
        &Obja,
        &FileName,
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
        );

    Status = NtCreateFile(
                &hFile,
                GENERIC_READ | SYNCHRONIZE,
                &Obja,
                &IoStatusBlock,
                NULL,
                0,
                0,
                FILE_OPEN,
                FILE_SYNCHRONOUS_IO_NONALERT
                );

    if (!NT_SUCCESS(Status)) 
    {
        hr = HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status));
        goto Cleanup;
    }
    
    if (hFile == INVALID_HANDLE_VALUE)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

    pbAlloc = (PBYTE)_alloca(MAX_CONFIG_SIZE);
    if (pbAlloc == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Cleanup;
    }
    
    pcBuffer = (PCHAR)pbAlloc;

    if (!ReadFile(hFile, pbAlloc, MAX_CONFIG_SIZE, &dwSize, NULL))
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto Cleanup;
    }

    Assert( dwSize <= MAX_CONFIG_SIZE - 1 );
    // Parse the configuration file
    pcBufferEnd = pcBuffer + dwSize;
    *pcBufferEnd++ = '\r';
    pcFirstString = pcBuffer;
    pcIP = NULL;
    pcPort = NULL;
    pcCR = NULL;
    
    while (pcBuffer < pcBufferEnd)
    {
        if (*pcBuffer == ':')
        {
            if (pcIP)
            {
                pcPort = pcBuffer+1;
                *pcBuffer = '\0';
            }
            else
            {
                pcIP = pcBuffer+1;
                *pcBuffer = '\0';
            }
        }
        else if (*pcBuffer == '\r')
        {
            pcCR = pcBuffer;
            *pcBuffer = '\0';
            pcBuffer++;
        }
        
        pcBuffer++;

        if (pcCR)
        {
            dwWhichKDC = NOT_KDC;
            
            if (*pcFirstString == 'S' && pcIP && pcPort)
            {
                dwServiceID = atol(pcFirstString+1);

                Assert( m_ctServiceInfoOverride < XONLINE_MAX_SERVICE_REQUEST );
                m_rgServiceInfoOverride[m_ctServiceInfoOverride].dwServiceID = dwServiceID;
                m_rgServiceInfoOverride[m_ctServiceInfoOverride].serviceIP.s_addr = inet_addr(pcIP);
                m_rgServiceInfoOverride[m_ctServiceInfoOverride].wServicePort = (WORD)atoi(pcPort);
                m_ctServiceInfoOverride++;
                TraceSz4(Verbose, "Overriding slot %d Service %d to IP %s Port %s", m_ctServiceInfoOverride, dwServiceID, pcIP, pcPort);
            }
            else if (_stricmp(pcFirstString,"FORCEMKDC") == 0)
            {
                m_fForceNewMachineAccount = TRUE;
            }
            else if (_stricmp(pcFirstString,"NUM_KERB_RETRY") == 0)
            {
                g_dwNumKerbRetry = atol(pcIP);
            }
            else if (_stricmp(pcFirstString,"NUM_MACS_RETRY") == 0)
            {
                g_dwNumMACSRetry = atol(pcIP);
            }
            else if (_stricmp(pcFirstString,"KERB_RETRY1_MS") == 0)
            {
                g_dwKerbRetryDelayInMilliSeconds[0] = atol(pcIP);
            }
            else if (_stricmp(pcFirstString,"KERB_RETRY2_MS") == 0)
            {
                g_dwKerbRetryDelayInMilliSeconds[1] = atol(pcIP);
            }
            else if (_stricmp(pcFirstString,"KERB_RETRY3_MS") == 0)
            {
                g_dwKerbRetryDelayInMilliSeconds[2] = atol(pcIP);
            }
            else if (_stricmp(pcFirstString,"KERB_RETRY4_MS") == 0)
            {
                g_dwKerbRetryDelayInMilliSeconds[3] = atol(pcIP);
            }
            else if (_stricmp(pcFirstString,"MACS_RETRY1_MS") == 0)
            {
                g_dwMACSRetryDelayInMilliSeconds[0] = atol(pcIP);
            }
            else if (_stricmp(pcFirstString,"MACS_RETRY2_MS") == 0)
            {
                g_dwMACSRetryDelayInMilliSeconds[1] = atol(pcIP);
            }
            else if (_stricmp(pcFirstString,"MACS_RETRY3_MS") == 0)
            {
                g_dwMACSRetryDelayInMilliSeconds[2] = atol(pcIP);
            }
            else if (_stricmp(pcFirstString,"MACS_RETRY4_MS") == 0)
            {
                g_dwMACSRetryDelayInMilliSeconds[3] = atol(pcIP);
            }
            else if (_stricmp(pcFirstString,"MKDC") == 0)
            {
                dwWhichKDC = IS_MKDC;
            }
            else if (_stricmp(pcFirstString,"XKDC") == 0)
            {
                dwWhichKDC = IS_XKDC;
            }
            else if (_stricmp(pcFirstString,"SG") == 0)
            {
                len = 0;
                if (pcIP)
                {
                    len = strlen(pcIP);
                }
                if (len != 0)
                {
                    m_sgAddr.s_addr = inet_addr( pcIP );
                }
                else
                {
                    m_fBypassSG = TRUE;
                }
            }

            if (dwWhichKDC != NOT_KDC)
            {
                len = 0;
                if (pcIP)
                {
                    len = strlen(pcIP);
                }
                if (len == 0)
                {
                       TraceSz1(Verbose, "Disabling %s to No Operation", pcFirstString);
                    m_fBypassAuth = TRUE;
                }
                else
                {
                       TraceSz2(Verbose, "Overriding %s to %s", pcFirstString, pcIP);
                       if (dwWhichKDC == IS_MKDC)
                       {
                           m_DNSCache[4].dwState = DNS_LOOKUP_COMPLETE;
                           m_DNSCache[4].address.cina = 1;
                           m_DNSCache[4].address.aina[0].s_addr = inet_addr( pcIP );
                       }
                       else if (dwWhichKDC == IS_XKDC)
                       {
                           m_DNSCache[0].dwState = DNS_LOOKUP_COMPLETE;
                           m_DNSCache[0].address.cina = 1;
                           m_DNSCache[0].address.aina[0].s_addr = inet_addr( pcIP );
                           m_DNSCache[1].dwState = DNS_LOOKUP_COMPLETE;
                           m_DNSCache[1].address.cina = 1;
                           m_DNSCache[1].address.aina[0].s_addr = inet_addr( pcIP );
                           m_DNSCache[2].dwState = DNS_LOOKUP_COMPLETE;
                           m_DNSCache[2].address.cina = 1;
                           m_DNSCache[2].address.aina[0].s_addr = inet_addr( pcIP );
                           m_DNSCache[3].dwState = DNS_LOOKUP_COMPLETE;
                           m_DNSCache[3].address.cina = 1;
                           m_DNSCache[3].address.aina[0].s_addr = inet_addr( pcIP );
                       }
                       else
                       {
                           AssertSz( FALSE, "dwWhichKDC has an invalid value" );
                       }
                }
            }
            
            pcFirstString = pcBuffer;
            pcIP = NULL;
            pcPort = NULL;
            pcCR = NULL;
        }            
    }

Cleanup:
        
    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
    }
    
    return hr;
}


#endif
