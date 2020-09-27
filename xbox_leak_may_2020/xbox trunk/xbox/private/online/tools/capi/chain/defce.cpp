//+---------------------------------------------------------------------------
//
//  Microsoft Windows NT Security
//  Copyright (C) Microsoft Corporation, 1997 - 1998
//
//  File:       defce.cpp
//
//  Contents:   Default Chain Engine Manager
//
//  History:    21-Apr-98    kirtd    Created
//
//----------------------------------------------------------------------------
#include <global.hxx>
//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::CDefaultChainEngineMgr, public
//
//  Synopsis:   Constructor
//
//----------------------------------------------------------------------------
CDefaultChainEngineMgr::CDefaultChainEngineMgr ()
{
}

//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::~CDefaultChainEngineMgr, public
//
//  Synopsis:   Destructor
//
//----------------------------------------------------------------------------
CDefaultChainEngineMgr::~CDefaultChainEngineMgr ()
{
}

//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::Initialize, public
//
//  Synopsis:   initialization routine
//
//----------------------------------------------------------------------------
BOOL
CDefaultChainEngineMgr::Initialize ()
{
    LRU_CACHE_CONFIG Config;

    InitializeCriticalSection( &m_Lock );

    m_hLocalMachineEngine = NULL;
    m_hProcessUserEngine = NULL;
    m_hImpersonationCache = NULL;

    memset( &Config, 0, sizeof( Config ) );

    Config.dwFlags = LRU_CACHE_NO_SERIALIZE;
    Config.cBuckets = DEFAULT_IMPERSONATION_CACHE_BUCKETS;
    Config.MaxEntries = MAX_IMPERSONATION_CACHE_ENTRIES;
    Config.pfnHash = DefaultChainEngineMgrHashTokenIdentifier;
#ifdef CE_BUILD 
    Config.pfnOnRemoval = NULL;
#else
    Config.pfnOnRemoval = DefaultChainEngineMgrOnImpersonationEngineRemoval;
#endif    

    return( I_CryptCreateLruCache( &Config, &m_hImpersonationCache ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::Uninitialize, public
//
//  Synopsis:   uninitialization routine
//
//----------------------------------------------------------------------------
VOID
CDefaultChainEngineMgr::Uninitialize ()
{
    if ( m_hLocalMachineEngine != NULL )
    {
        CertFreeCertificateChainEngine( m_hLocalMachineEngine );
    }

    if ( m_hProcessUserEngine != NULL )
    {
        CertFreeCertificateChainEngine( m_hProcessUserEngine );
    }

    if ( m_hImpersonationCache != NULL )
    {
        I_CryptFreeLruCache( m_hImpersonationCache, 0, NULL );
    }

    DeleteCriticalSection( &m_Lock );
}

//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::GetDefaultEngine, public
//
//  Synopsis:   get the default engine
//
//----------------------------------------------------------------------------
BOOL
CDefaultChainEngineMgr::GetDefaultEngine (
                           IN HCERTCHAINENGINE hDefaultHandle,
                           OUT HCERTCHAINENGINE* phDefaultEngine
                           )
{
    assert( ( hDefaultHandle == HCCE_LOCAL_MACHINE ) ||
            ( hDefaultHandle == HCCE_CURRENT_USER ) );

    if ( hDefaultHandle == HCCE_LOCAL_MACHINE )
    {
        return( GetDefaultLocalMachineEngine( phDefaultEngine ) );
    }
    else if ( hDefaultHandle == HCCE_CURRENT_USER )
    {
        return( GetDefaultCurrentUserEngine( phDefaultEngine ) );
    }

    SetLastError( E_INVALIDARG );
    return( FALSE );
}

//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::GetDefaultLocalMachineEngine, public
//
//  Synopsis:   get the default local machine chain engine
//
//----------------------------------------------------------------------------
BOOL
CDefaultChainEngineMgr::GetDefaultLocalMachineEngine (
                           OUT HCERTCHAINENGINE* phDefaultEngine
                           )
{
    BOOL fResult = TRUE;

    EnterCriticalSection( &m_Lock );

    if ( m_hLocalMachineEngine == NULL )
    {
        HCERTCHAINENGINE         hEngine = NULL;
        CERT_CHAIN_ENGINE_CONFIG Config;

        LeaveCriticalSection( &m_Lock );

        memset( &Config, 0, sizeof( Config ) );

        Config.cbSize = sizeof( Config );
        Config.dwFlags = CERT_CHAIN_USE_LOCAL_MACHINE_STORE;
        Config.dwFlags |= CERT_CHAIN_ENABLE_CACHE_AUTO_UPDATE;
        Config.dwUrlRetrievalTimeout = DEFAULT_ENGINE_URL_RETRIEVAL_TIMEOUT;

        fResult = CertCreateCertificateChainEngine(
                      &Config,
                      &hEngine
                      );

        EnterCriticalSection( &m_Lock );

        if ( ( fResult == TRUE ) && ( m_hLocalMachineEngine == NULL ) )
        {
            m_hLocalMachineEngine = hEngine;
            hEngine = NULL;
        }

        if ( hEngine != NULL )
        {
            ( (PCCERTCHAINENGINE)hEngine )->Release();
        }
    }

    if ( fResult == TRUE )
    {
        ( (PCCERTCHAINENGINE)m_hLocalMachineEngine )->AddRef();
        *phDefaultEngine = m_hLocalMachineEngine;
    }

    LeaveCriticalSection( &m_Lock );

    return( fResult );
}

#ifdef CE_BUILD
// dont support impersonations
BOOL
CDefaultChainEngineMgr::IsImpersonatingUser (
                          OUT HANDLE* phUserToken
                          )
{
    SetLastError( ERROR_NO_TOKEN );
	return FALSE;
}
#endif

//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::GetDefaultCurrentUserEngine, public
//
//  Synopsis:   get the default current user chain engine
//
//----------------------------------------------------------------------------
BOOL
CDefaultChainEngineMgr::GetDefaultCurrentUserEngine (
                           OUT HCERTCHAINENGINE* phDefaultEngine
                           )
{
    BOOL   fResult = TRUE;
    HANDLE hUserToken;

    EnterCriticalSection( &m_Lock );

    if ( IsImpersonatingUser( &hUserToken ) == FALSE )
    {
        if ( GetLastError() != ERROR_NO_TOKEN )
        {
            LeaveCriticalSection( &m_Lock );
            return( FALSE );
        }

        if ( m_hProcessUserEngine == NULL )
        {
            HCERTCHAINENGINE         hEngine = NULL;
            CERT_CHAIN_ENGINE_CONFIG Config;

            LeaveCriticalSection( &m_Lock );

            memset( &Config, 0, sizeof( Config ) );

            Config.cbSize = sizeof( Config );
            Config.dwFlags = CERT_CHAIN_ENABLE_CACHE_AUTO_UPDATE;
            Config.dwUrlRetrievalTimeout = DEFAULT_ENGINE_URL_RETRIEVAL_TIMEOUT;

            fResult = CertCreateCertificateChainEngine(
                          &Config,
                          &hEngine
                          );

            EnterCriticalSection( &m_Lock );

            if ( ( fResult == TRUE ) && ( m_hProcessUserEngine == NULL ) )
            {
                m_hProcessUserEngine = hEngine;
                hEngine = NULL;
            }

            if ( hEngine != NULL )
            {
                ( (PCCERTCHAINENGINE)hEngine )->Release();
            }
        }

        if ( fResult == TRUE )
        {
            ( (PCCERTCHAINENGINE)m_hProcessUserEngine )->AddRef();
            *phDefaultEngine = m_hProcessUserEngine;
        }
    }
#ifndef CE_BUILD    
    else
    {
        fResult = GetDefaultCurrentImpersonatedUserEngine(
                     hUserToken,
                     phDefaultEngine
                     );

        CloseHandle( hUserToken );
    }
#endif

    LeaveCriticalSection( &m_Lock );

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::FlushDefaultEngine, public
//
//  Synopsis:   flush default engine
//
//----------------------------------------------------------------------------
VOID
CDefaultChainEngineMgr::FlushDefaultEngine (IN HCERTCHAINENGINE hDefaultHandle)
{
    EnterCriticalSection( &m_Lock );

    if ( hDefaultHandle == HCCE_CURRENT_USER )
    {
        if ( m_hProcessUserEngine != NULL )
        {
            CertFreeCertificateChainEngine( m_hProcessUserEngine );
            m_hProcessUserEngine = NULL;
        }

        assert( m_hImpersonationCache != NULL );

        I_CryptFlushLruCache( m_hImpersonationCache, 0, NULL );
    }
    else if ( hDefaultHandle == HCCE_LOCAL_MACHINE )
    {
        if ( m_hLocalMachineEngine != NULL )
        {
            CertFreeCertificateChainEngine( m_hLocalMachineEngine );
            m_hLocalMachineEngine = NULL;
        }
    }

    LeaveCriticalSection( &m_Lock );
}

#ifndef CE_BUILD
//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::GetDefaultCurrentImpersonatedUserEngine
//
//  Synopsis:   get current impersonated user chain engine
//
//----------------------------------------------------------------------------
BOOL
CDefaultChainEngineMgr::GetDefaultCurrentImpersonatedUserEngine (
                           IN HANDLE hUserToken,
                           OUT HCERTCHAINENGINE* phDefaultEngine
                           )
{
    BOOL                  fResult;
    CRYPT_DATA_BLOB       TokenId;
    PCIMPERSONATIONENGINE pEngine = NULL;

    fResult = GetTokenId( hUserToken, &TokenId );

    if ( fResult == TRUE )
    {
        if ( FindImpersonationEngine( &TokenId, &pEngine ) == FALSE )
        {
            PCIMPERSONATIONENGINE pCreatedEngine = NULL;

            LeaveCriticalSection( &m_Lock );

            fResult = CreateImpersonationEngine( &TokenId, &pCreatedEngine );

            EnterCriticalSection( &m_Lock );

            if ( fResult == TRUE )
            {
                fResult = FindImpersonationEngine( &TokenId, &pEngine );

                if ( fResult == FALSE )
                {
                    pEngine = pCreatedEngine;
                    pCreatedEngine = NULL;

                    AddToImpersonationCache( pEngine );

                    fResult = TRUE;
                }

                if ( pCreatedEngine != NULL )
                {
                    pCreatedEngine->Release();
                }
            }
        }

        FreeTokenId( &TokenId );
    }

    if ( fResult == TRUE )
    {
        *phDefaultEngine = pEngine->ChainEngine();
        ( (PCCERTCHAINENGINE)*phDefaultEngine )->AddRef();
    }

    if ( pEngine != NULL )
    {
        pEngine->Release();
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::IsImpersonatingUser, public
//
//  Synopsis:   is impersonating user?
//
//----------------------------------------------------------------------------
BOOL
CDefaultChainEngineMgr::IsImpersonatingUser (
                          OUT HANDLE* phUserToken
                          )
{
    if ( FIsWinNT() == FALSE )
    {
        SetLastError( ERROR_NO_TOKEN );
        return( FALSE );
    }

    return( OpenThreadToken(
                GetCurrentThread(),
                TOKEN_QUERY,
                TRUE,
                phUserToken
                ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::GetTokenId, public
//
//  Synopsis:   get the token id which is the ModifiedId LUID inside of
//              the TOKEN_STATISTICS information
//
//----------------------------------------------------------------------------
BOOL
CDefaultChainEngineMgr::GetTokenId (
                           IN HANDLE hUserToken,
                           OUT PCRYPT_DATA_BLOB pTokenId
                           )
{
    BOOL             fResult;
    TOKEN_STATISTICS ts;
    DWORD            Length = 0;

    fResult = GetTokenInformation(
                 hUserToken,
                 TokenStatistics,
                 &ts,
                 sizeof( ts ),
                 &Length
                 );

    if ( fResult == TRUE )
    {
        pTokenId->cbData = sizeof( ts.ModifiedId );
        pTokenId->pbData = new BYTE [ sizeof( ts.ModifiedId ) ];
        if ( pTokenId->pbData != NULL )
        {
            memcpy(
               pTokenId->pbData,
               &ts.ModifiedId,
               sizeof( ts.ModifiedId )
               );
        }
        else
        {
            SetLastError( E_OUTOFMEMORY );
            fResult = FALSE;
        }
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::FreeTokenId, public
//
//  Synopsis:   free token id
//
//----------------------------------------------------------------------------
VOID
CDefaultChainEngineMgr::FreeTokenId (
                            IN PCRYPT_DATA_BLOB pTokenId
                            )
{
    delete pTokenId->pbData;
}

//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::FindImpersonationEngine, public
//
//  Synopsis:   find the impersonation engine
//
//----------------------------------------------------------------------------
BOOL
CDefaultChainEngineMgr::FindImpersonationEngine (
                            IN PCRYPT_DATA_BLOB pTokenId,
                            OUT PCIMPERSONATIONENGINE* ppEngine
                            )
{
    BOOL                  fResult = FALSE;
    HLRUENTRY             hFound;
    PCIMPERSONATIONENGINE pEngine = NULL;

    hFound = I_CryptFindLruEntry( m_hImpersonationCache, pTokenId );

    if ( hFound != NULL )
    {
        pEngine = (PCIMPERSONATIONENGINE)I_CryptGetLruEntryData( hFound );
        pEngine->AddRef();

        *ppEngine = pEngine;

        I_CryptReleaseLruEntry( hFound );

        return( TRUE );
    }

    return( FALSE );
}

//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::CreateImpersonationEngine, public
//
//  Synopsis:   create an impersonation engine
//
//----------------------------------------------------------------------------
BOOL
CDefaultChainEngineMgr::CreateImpersonationEngine (
                              IN PCRYPT_DATA_BLOB pTokenId,
                              OUT PCIMPERSONATIONENGINE* ppEngine
                              )
{
    BOOL                  fResult = FALSE;
    PCIMPERSONATIONENGINE pEngine;

    pEngine = new CImpersonationEngine(
                                m_hImpersonationCache,
                                pTokenId,
                                fResult
                                );

    if ( pEngine == NULL )
    {
        SetLastError( E_OUTOFMEMORY );
        return( FALSE );
    }
    else if ( fResult == FALSE )
    {
        delete pEngine;
        return( FALSE );
    }

    *ppEngine = pEngine;
    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Member:     CDefaultChainEngineMgr::AddToImpersonationCache, public
//
//  Synopsis:   add to the cache
//
//----------------------------------------------------------------------------
VOID
CDefaultChainEngineMgr::AddToImpersonationCache(
                           IN PCIMPERSONATIONENGINE pEngine
                           )
{
    pEngine->AddRef();
    I_CryptInsertLruEntry( pEngine->LruEntry(), NULL );
}

//+---------------------------------------------------------------------------
//
//  Function:   DefaultChainEngineMgrOnImpersonationEngineRemoval
//
//  Synopsis:   removal notification
//
//----------------------------------------------------------------------------
VOID WINAPI
DefaultChainEngineMgrOnImpersonationEngineRemoval (
       IN LPVOID pv,
       IN LPVOID pvRemovalContext
       )
{
    ( (PCIMPERSONATIONENGINE)pv )->Release();
}

#endif	//CE_BUILD

//+---------------------------------------------------------------------------
//
//  Function:   DefaultChainEngineMgrHashTokenIdentifier
//
//  Synopsis:   hash the token identifier
//
//----------------------------------------------------------------------------
DWORD WINAPI
DefaultChainEngineMgrHashTokenIdentifier (
       IN PCRYPT_DATA_BLOB pIdentifier
       )
{
    DWORD  dwHash = 0;
    DWORD  cb = pIdentifier->cbData;
    LPBYTE pb = pIdentifier->pbData;

    while ( cb-- )
    {
        if ( dwHash & 0x80000000 )
        {
            dwHash = ( dwHash << 1 ) | 1;
        }
        else
        {
            dwHash = dwHash << 1;
        }

        dwHash += *pb++;
    }

    return( dwHash );
}

#ifndef CE_BUILD
//+---------------------------------------------------------------------------
//
//  Member:     CImpersonationEngine::CImpersonationEngine, public
//
//  Synopsis:   Constructor
//
//----------------------------------------------------------------------------
CImpersonationEngine::CImpersonationEngine (
                                    IN HLRUCACHE hCache,
                                    IN PCRYPT_DATA_BLOB pTokenId,
                                    OUT BOOL& rfResult
                                    )
{
    CERT_CHAIN_ENGINE_CONFIG Config;

    m_cRefs = 1;
    m_hChainEngine = NULL;
    m_hLruEntry = NULL;

    memset( &Config, 0, sizeof( Config ) );

    Config.cbSize = sizeof( Config );
    Config.dwFlags = CERT_CHAIN_ENABLE_CACHE_AUTO_UPDATE;
    Config.dwUrlRetrievalTimeout = DEFAULT_ENGINE_URL_RETRIEVAL_TIMEOUT;

    rfResult = CertCreateCertificateChainEngine( &Config, &m_hChainEngine );

    if ( rfResult == TRUE )
    {
        rfResult = I_CryptCreateLruEntry(
                          hCache,
                          pTokenId,
                          this,
                          &m_hLruEntry
                          );
    }
}

//+---------------------------------------------------------------------------
//
//  Member:     CImpersonationEngine::~CImpersonationEngine, public
//
//  Synopsis:   Destructor
//
//----------------------------------------------------------------------------
CImpersonationEngine::~CImpersonationEngine ()
{
    if ( m_hLruEntry != NULL )
    {
        I_CryptReleaseLruEntry( m_hLruEntry );
    }

    if ( m_hChainEngine != NULL )
    {
        CertFreeCertificateChainEngine( m_hChainEngine );
    }
}
#endif
