//+---------------------------------------------------------------------------
//
//  Microsoft Windows NT Security
//  Copyright (C) Microsoft Corporation, 1997 - 1998
//
//  File:       api.cpp
//
//  Contents:   Certificate Chaining Infrastructure
//
//  History:    28-Jan-98    kirtd    Created
//
//----------------------------------------------------------------------------
#include <global.hxx>

//
// Globals
//

HMODULE                g_hCryptnet = NULL;
CRITICAL_SECTION       g_CryptnetLock;
CDefaultChainEngineMgr DefaultChainEngineMgr;
//+---------------------------------------------------------------------------
//
//  Function:   ChainDllMain
//
//  Synopsis:   Chaining infrastructure initialization
//
//  Declare as export so that the master DllMain can do a late binding to it.
//
//----------------------------------------------------------------------------


extern "C"
BOOL WINAPI
ChainDllMain (
     IN HMODULE hModule,
     IN ULONG ulReason,
     IN LPVOID pvReserved
     )
{
    BOOL fResult = TRUE;

    switch ( ulReason )
    {
    case DLL_PROCESS_ATTACH:

        InitializeCriticalSection( &g_CryptnetLock );

        fResult = DefaultChainEngineMgr.Initialize();

        break;
    case DLL_PROCESS_DETACH:

        DefaultChainEngineMgr.Uninitialize();

        if ( g_hCryptnet != NULL )
        {
            //Xbox FreeLibrary( g_hCryptnet );
        }

        DeleteCriticalSection( &g_CryptnetLock );
        break;
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   InternalCertCreateCertificateChainEngine
//
//  Synopsis:   create a chain engine handle
//
//----------------------------------------------------------------------------
BOOL WINAPI
InternalCertCreateCertificateChainEngine (
    IN PCERT_CHAIN_ENGINE_CONFIG pConfig,
    IN BOOL fDefaultEngine,
    OUT HCERTCHAINENGINE* phChainEngine
    )
{
    BOOL                     fResult = TRUE;
    PCCERTCHAINENGINE        pChainEngine = NULL;
    CERT_CHAIN_ENGINE_CONFIG Config;

    if ( pConfig->cbSize != sizeof( CERT_CHAIN_ENGINE_CONFIG ) )
    {
        SetLastError( E_INVALIDARG );
        return( FALSE );
    }

    Config = *pConfig;

    if ( Config.MaximumCachedCertificates == 0 )
    {
        Config.MaximumCachedCertificates = DEFAULT_MAX_INDEX_ENTRIES;
    }

    if ( Config.CycleDetectionModulus == 0 )
    {
        Config.CycleDetectionModulus = CYCLE_DETECTION_MODULUS;
    }

    if ( Config.dwUrlRetrievalTimeout == 0 )
    {
        Config.dwUrlRetrievalTimeout = DEFAULT_ENGINE_URL_RETRIEVAL_TIMEOUT;
    }

    pChainEngine = new CCertChainEngine( &Config, fDefaultEngine, fResult );
    if ( pChainEngine == NULL )
    {
        SetLastError( E_OUTOFMEMORY );
        fResult = FALSE;
    }

    if ( fResult == TRUE )
    {
        *phChainEngine = (HCERTCHAINENGINE)pChainEngine;
    }
    else
    {
        delete pChainEngine;
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   CertCreateCertificateChainEngine
//
//  Synopsis:   create a certificate chain engine
//
//----------------------------------------------------------------------------
BOOL WINAPI
CertCreateCertificateChainEngine (
    IN PCERT_CHAIN_ENGINE_CONFIG pConfig,
    OUT HCERTCHAINENGINE* phChainEngine
    )
{
    return( InternalCertCreateCertificateChainEngine(
                    pConfig,
                    FALSE,
                    phChainEngine
                    ) );
}

//+---------------------------------------------------------------------------
//
//  Function:   CertFreeCertificateChainEngine
//
//  Synopsis:   free the chain engine handle
//
//----------------------------------------------------------------------------
VOID WINAPI
CertFreeCertificateChainEngine (
    IN HCERTCHAINENGINE hChainEngine
    )
{
    if ( ( hChainEngine == HCCE_CURRENT_USER ) ||
         ( hChainEngine == HCCE_LOCAL_MACHINE ) )
    {
        DefaultChainEngineMgr.FlushDefaultEngine( hChainEngine );
        return;
    }

    ( (PCCERTCHAINENGINE)hChainEngine )->Release();
}

#ifndef CE_BUILD
//+---------------------------------------------------------------------------
//
//  Function:   CertResyncCertificateChainEngine
//
//  Synopsis:   resync the chain engine
//
//----------------------------------------------------------------------------
BOOL WINAPI
CertResyncCertificateChainEngine (
    IN HCERTCHAINENGINE hChainEngine
    )
{
    BOOL              fResult;
    PCCERTCHAINENGINE pChainEngine = (PCCERTCHAINENGINE)hChainEngine;

    if ( ( hChainEngine == HCCE_LOCAL_MACHINE ) ||
         ( hChainEngine == HCCE_CURRENT_USER ) )
    {
        if ( DefaultChainEngineMgr.GetDefaultEngine(
                                      hChainEngine,
                                      (HCERTCHAINENGINE *)&pChainEngine
                                      ) == FALSE )
        {
            return( FALSE );
        }
    }
    else
    {
        pChainEngine->AddRef();
    }

    pChainEngine->LockEngine();

    fResult = pChainEngine->Resync( NULL, TRUE );

    pChainEngine->UnlockEngine();

    pChainEngine->Release();

    return( fResult );
}
#endif

//+---------------------------------------------------------------------------
//
//  Function:   CertGetCertificateChain
//
//  Synopsis:   get the certificate chain for the given end certificate
//
//----------------------------------------------------------------------------
BOOL WINAPI
CertGetCertificateChain (
    IN OPTIONAL HCERTCHAINENGINE hChainEngine,
    IN PCCERT_CONTEXT pCertContext,
    IN OPTIONAL LPFILETIME pTime,
    IN OPTIONAL HCERTSTORE hAdditionalStore,
    IN PCERT_CHAIN_PARA pChainPara,
    IN DWORD dwFlags,
    IN LPVOID pvReserved,
    OUT PCCERT_CHAIN_CONTEXT* ppChainContext
    )
{
    BOOL              fResult;
    PCCERTCHAINENGINE pChainEngine = (PCCERTCHAINENGINE)hChainEngine;
    FILETIME          CurrentTime;
    LPVOID            pvImpersonated = NULL;

    if ( ( pChainPara == NULL ) || ( pvReserved != NULL ) )
    {
        SetLastError( E_INVALIDARG );
        return( FALSE );
    }

#ifdef CE_BUILD
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);
	SystemTimeToFileTime(&sysTime,&CurrentTime);
#else
    GetSystemTimeAsFileTime( &CurrentTime );
#endif

    if ( pTime == NULL )
    {
        pTime = &CurrentTime;
    }

    if ( ( hChainEngine == HCCE_LOCAL_MACHINE ) ||
         ( hChainEngine == HCCE_CURRENT_USER ) )
    {
        if ( DefaultChainEngineMgr.GetDefaultEngine(
                                      hChainEngine,
                                      (HCERTCHAINENGINE *)&pChainEngine
                                      ) == FALSE )
        {
            return( FALSE );
        }
    }
    else
    {
        pChainEngine->AddRef();
    }

    fResult = pChainEngine->GetChainContext(
                               pCertContext,
                               pTime,
                               &CurrentTime,
                               hAdditionalStore,
                               &pChainPara->RequestedUsage,
                               dwFlags,
                               pvReserved,
                               ppChainContext
                               );

    pChainEngine->Release();

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   CertFreeCertificateChain
//
//  Synopsis:   free a certificate chain context
//
//----------------------------------------------------------------------------
VOID WINAPI
CertFreeCertificateChain (
    IN PCCERT_CHAIN_CONTEXT pChainContext
    )
{
    ChainReleaseInternalChainContext(
         (PINTERNAL_CERT_CHAIN_CONTEXT)pChainContext
         );
}

//+---------------------------------------------------------------------------
//
//  Function:   CertDuplicateCertificateChain
//
//  Synopsis:   duplicate (add a reference to) a certificate chain
//
//----------------------------------------------------------------------------
PCCERT_CHAIN_CONTEXT WINAPI
CertDuplicateCertificateChain (
    IN PCCERT_CHAIN_CONTEXT pChainContext
    )
{
    ChainAddRefInternalChainContext(
         (PINTERNAL_CERT_CHAIN_CONTEXT)pChainContext
         );

    return( pChainContext );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainGetCryptnetModule
//
//  Synopsis:   get the cryptnet.dll module handle
//
//----------------------------------------------------------------------------
HMODULE WINAPI
ChainGetCryptnetModule ()
{
    HMODULE hModule;

    EnterCriticalSection( &g_CryptnetLock );

    if ( g_hCryptnet == NULL )
    {
#ifdef UNDER_CE
        //Xbox g_hCryptnet = LoadLibrary( TEXT("cryptnet.dll") );
#else
        g_hCryptnet = LoadLibraryA( "cryptnet.dll" );
#endif        
    }

    hModule = g_hCryptnet;

    LeaveCriticalSection( &g_CryptnetLock );

    return( hModule );
}

#ifndef CE_BUILD
//+---------------------------------------------------------------------------
//
//  Function:   ChainWlxLogoffEvent
//
//  Synopsis:   logoff event processing
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainWlxLogoffEvent (PWLX_NOTIFICATION_INFO pNotificationInfo)
{
    CertFreeCertificateChainEngine( HCCE_CURRENT_USER );
    return( TRUE );
}
#endif
