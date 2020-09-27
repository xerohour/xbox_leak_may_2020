// ---------------------------------------------------------------------------------------
// xonline.cpp
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#include "xonp.h"
#include "xboxp.h"
#include "xonver.h"

// ---------------------------------------------------------------------------------------
// Main entrypoints for XOnline API
// ---------------------------------------------------------------------------------------

#ifdef XONLINE_FEATURE_XBOX
CXo * g_pXo = NULL;
#endif

#undef  XONAPI
#define XONAPI(ret, fname, arglist, paramlist) ret _XONAPI_ fname arglist { return(GetXo()->fname paramlist); }
#undef  XONAPI_
#define XONAPI_(ret, fname, arglist, paramlist)

XONLINEAPILIST()

// ---------------------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------------------

LONG g_lXoLock = 0;

//@@@ drm: Not used anymore.  Switch to flag in XOnlineStartup.
BOOL g_ParamCheckReturnsError = FALSE;

// ---------------------------------------------------------------------------------------
// Memory Allocation
// ---------------------------------------------------------------------------------------

const char * XoLeakTagToString(ULONG tag)
{
    switch (tag)
    {
        #undef  XONPTAG
        #define XONPTAG(_name, _tag) case PTAG_##_name: return(#_name); break;
        XONPTAGLIST()
    }

    return("?");
}

// ---------------------------------------------------------------------------------------
// XOnlineStartup & XOnlineCleanup
// ---------------------------------------------------------------------------------------

HRESULT _XONAPI_ XOnlineStartup(PXONLINE_STARTUP_PARAMS pxosp)
{
    while (InterlockedCompareExchange(&g_lXoLock, 1, 0))
    {
        Sleep(0);
    }

    HRESULT hr = S_OK;

    WSADATA WSAData;
    INT err = WSAStartup(0x0200, &WSAData);

    if (err != 0)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());

        if (!FAILED(hr))
        {
            hr = E_FAIL;
        }
    }

    if (hr == S_OK)
    {
        CXo ** ppXo = GetXoRef();
        CXo *  pXo  = *ppXo;

        if (pXo == NULL)
        {
            pXo = (CXo *)XoSysAlloc(sizeof(CXo), PTAG_CXo);

            if (pXo == NULL)
            {
                hr = E_OUTOFMEMORY;
            }
            else
            {
                memset(pXo, 0, sizeof(CXo));

                #ifdef XONLINE_FEATURE_WINDOWS
                pXo->SetXn(GetXn());
                pXo->SetAchXbox(GetAchXbox());
                #endif

                hr = pXo->Init(pxosp);

                if (FAILED(hr))
                {
                    pXo->Term();
                    XoSysFree(pXo);
                    pXo = NULL;
                }
                else
                {
                    *ppXo = pXo;
                }
            }
        }
        else
        {
            pXo->AddRef();
        }

        if (FAILED(hr))
        {
            WSACleanup();
        }
    }

    g_lXoLock = 0;

    if (FAILED(hr))
    {
        TraceSz1(Warning, "XOnlineStartup failed: %08lX", hr);
    }

    return(hr);
}

HRESULT _XONAPI_ XOnlineCleanup()
{
    HRESULT hr = S_OK;

    while (InterlockedCompareExchange(&g_lXoLock, 1, 0))
    {
        Sleep(0);
    }

    CXo ** ppXo = GetXoRef();
    CXo *  pXo  = *ppXo;

    if (pXo == NULL)
    {
        hr = XONLINE_E_NOT_INITIALIZED;
    }
    else
    {
        if (pXo->Release() == 0)
        {
            pXo->Term();
            XoSysFree(pXo);
            *ppXo = NULL;
        }

        WSACleanup();
    }

    g_lXoLock = 0;

    if (FAILED(hr))
    {
        TraceSz1(Warning, "XOnlineCleanup failed: %08lX", hr);
    }

    return(hr);
}

// ---------------------------------------------------------------------------------------
// CXo::Init
// ---------------------------------------------------------------------------------------

HRESULT CXo::Init(PXONLINE_STARTUP_PARAMS pxosp)
{
    HRESULT hr;
    
#ifdef XNET_FEATURE_ASSERT
    m_LeakInfo._pfnLeakTag = XoLeakTagToString;
#endif

#ifdef XONLINE_FEATURE_XBOX
    PXBEIMAGE_CERTIFICATE pCert = XeImageHeader()->Certificate;
    m_dwTitleId         = pCert->TitleID;
    m_dwTitleVersion    = pCert->Version;
    m_dwTitleGameRegion = pCert->GameRegion;
    AssertSz( m_dwTitleId != 0, "The TitleID cannot be zero because the presence system will fail" );
#else
    m_dwTitleId         = 1234;
    m_dwTitleVersion    = 1;
    m_dwTitleGameRegion = 0xFFFFFFFF;
#endif

    XONLINE_DNS_CACHE_ENTRY * pdce = m_DNSCache;
    (pdce++)->name = PASSPORT_AS_DNS;
    (pdce++)->name = PASSPORT_WEB_DNS;
    (pdce++)->name = PASSPORT_TGS_DNS;
    (pdce++)->name = XBOX_KDC_DNS;
    (pdce++)->name = MACS_KDC_DNS;

    m_hConfigVolume = XNetOpenConfigVolume();

    if (m_hConfigVolume == INVALID_HANDLE_VALUE)
    {
        hr = E_FAIL; // TODO something better
        goto Cleanup;
    }

	//
	// Open the recent logon time cache
	//
    hr = CacheOpen(XONLC_TYPE_RECENT_LOGON_TIME, NULL, NULL, &m_hLogonTimeCache, &m_hLogonTimeCacheTask);
    if (FAILED(hr))
    {
        m_hLogonTimeCache = NULL;
        m_hLogonTimeCacheTask = NULL;
        m_dwLogonTimeCacheState = XON_CACHE_STATE_ERROR;
    }
    else
    {
        m_dwLogonTimeCacheState = XON_CACHE_STATE_OPENING;
    }
    
    m_qwMachineId = -1;
    
    KRB5_Module_Startup(&m_KRB5_Module);

#ifdef XONLINE_FEATURE_INSECURE
    InitOverrideInfo();
#endif

    XnSetXoBase(this);

    m_cRefs = 1;

    hr = S_OK;
    
Cleanup:
    return(hr);
}

void CXo::Term()
{
    DWORD i;
    Assert(m_cRefs == 0);

    for (i=0; i<m_ctAllocatedServiceContexts; ++i)
    {
        SysFree( m_pAllocatedServiceContexts[i] );
    }

    if (m_hConfigVolume != INVALID_HANDLE_VALUE)
    {
        XNetCloseConfigVolume(m_hConfigVolume);
    }

    if (m_hLogonTimeCacheTask != NULL)
    {
        XOnlineTaskClose(m_hLogonTimeCacheTask);
    }
    
    if (m_hLogonTimeCache != NULL)
    {
        CacheClose(m_hLogonTimeCache);
    }

#ifdef XNET_FEATURE_ASSERT
    if (m_LeakInfo._cLeak > 0)
    {
        XnLeakTerm(&m_LeakInfo);
    }
#endif

    XnSetXoBase(NULL);
}


DWORD CXo::_XOnlineSetTitleId(DWORD dwTitleId)
{
    AssertSz( dwTitleId != 0, "The TitleID cannot be set to zero because the presence system will fail" );
#ifdef XONLINE_FEATURE_INSECURE
    return m_dwTitleId = dwTitleId;
#else
    return m_dwTitleId;
#endif
}


DWORD CXo::_XOnlineSetTitleVersion(DWORD dwTitleVersion)
{
#ifdef XONLINE_FEATURE_INSECURE
    return m_dwTitleVersion = dwTitleVersion;
#else
    return m_dwTitleVersion;
#endif
}


DWORD CXo::_XOnlineSetTitleGameRegion(DWORD dwTitleGameRegion)
{
#ifdef XONLINE_FEATURE_INSECURE
    return m_dwTitleGameRegion = dwTitleGameRegion;
#else
    return m_dwTitleGameRegion;
#endif
}


HRESULT CXo::_XOnlineGetMachineID(ULONGLONG* pqwMachineID)
{
    XoEnter("_XOnlineGetMachineID");
    XoCheck(pqwMachineID != NULL);

    BYTE abConfigData[CONFIG_DATA_LENGTH];

    if (m_qwMachineId == -1)
    {
        m_qwMachineId = 0;
        if (XNetLoadConfigSector(m_hConfigVolume, MACHINE_ACCOUNT_CONFIG_SECTOR, abConfigData, CONFIG_DATA_LENGTH))
        {
            m_qwMachineId = ((XC_ONLINE_MACHINE_ACCOUNT_STRUCT*)abConfigData)->xuid.qwUserID;
        }
    }
    *pqwMachineID = m_qwMachineId;

    return(XoLeave(S_OK));
}

HRESULT CXo::_XOnlineGetSerialNumber(CHAR abSerialNumber[12])
{
    XoEnter("_XOnlineGetSerialNumber");

    HRESULT hr = S_OK;
    NTSTATUS Status;
    DWORD dwType;
    DWORD cbResultLength;

    //
    // Read Serial Number from the EEPROM
    //
    Status = ExQueryNonVolatileSetting(XC_FACTORY_SERIAL_NUMBER, &dwType, abSerialNumber, 12, &cbResultLength);
    if (Status != ERROR_SUCCESS)
    {
        hr = Status;
    }
    else
    {
        Assert( dwType == REG_BINARY );
        Assert( cbResultLength == 12 );
    }

    return(XoLeave(hr));
}

// ---------------------------------------------------------------------------------------
// Windows support
// ---------------------------------------------------------------------------------------
    
#ifdef XONLINE_FEATURE_WINDOWS

#include <xboxp.h>

DWORD CXo::XGetParentalControlSetting()
{
    ULONG ulType;
    DWORD dwValue;
    return(NT_SUCCESS(ExQueryNonVolatileSetting(XC_PARENTAL_CONTROL_GAMES, &ulType, &dwValue, sizeof(dwValue), NULL)) ? dwValue : 0);
}

DWORD CXo::XGetLanguage()
{
    ULONG ulType;
    DWORD dwValue;
    return NT_SUCCESS(ExQueryNonVolatileSetting(XC_LANGUAGE, &ulType, &dwValue, sizeof(dwValue), NULL)) ? dwValue : XC_LANGUAGE_UNKNOWN;
}

#endif
