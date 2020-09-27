//+---------------------------------------------------------------------------
//
//  Microsoft Windows NT Security
//  Copyright (C) Microsoft Corporation, 1997 - 1998
//
//  File:       callctx.cpp
//
//  Contents:   Certificate Chaining Infrastructure Call Context
//
//  History:    02-Mar-98    kirtd    Created
//
//----------------------------------------------------------------------------
#include <global.hxx>
//+---------------------------------------------------------------------------
//
//  Member:     CChainCallContext::CChainCallContext, public
//
//  Synopsis:   Constructor
//
//----------------------------------------------------------------------------
CChainCallContext::CChainCallContext (
                         OUT BOOL& rfResult
                         )
{
    LRU_CACHE_CONFIG Config;

    m_hObjectCreationCache = NULL;

    memset( &Config, 0, sizeof( Config ) );

    Config.dwFlags = LRU_CACHE_NO_SERIALIZE | LRU_CACHE_NO_COPY_IDENTIFIER;
    Config.pfnHash = CertObjectCacheHashMd5Identifier;
    Config.pfnOnRemoval = CallContextOnCreationCacheObjectRemoval;
    Config.cBuckets = DEFAULT_CREATION_CACHE_BUCKETS;

    rfResult = I_CryptCreateLruCache( &Config, &m_hObjectCreationCache );
}

//+---------------------------------------------------------------------------
//
//  Member:     CChainCallContext::~CChainCallContext, public
//
//  Synopsis:   Destructor
//
//----------------------------------------------------------------------------
CChainCallContext::~CChainCallContext ()
{
    if ( m_hObjectCreationCache != NULL )
    {
        I_CryptFreeLruCache( m_hObjectCreationCache, 0, NULL );
    }
}

//+---------------------------------------------------------------------------
//
//  Member:     CChainCallContext::AddObjectToCreationCache, public
//
//  Synopsis:   add a certificate object to the creation cache
//
//----------------------------------------------------------------------------
BOOL
CChainCallContext::AddObjectToCreationCache (
                      IN PCCERTOBJECT pCertObject
                      )
{
    BOOL            fResult;
    CRYPT_DATA_BLOB DataBlob;
    HLRUENTRY       hEntry;

    DataBlob.cbData = pCertObject->CertificateHashSize();
    DataBlob.pbData = pCertObject->CertificateHash();

    fResult = I_CryptCreateLruEntry(
                     m_hObjectCreationCache,
                     &DataBlob,
                     pCertObject,
                     &hEntry
                     );

    if ( fResult == TRUE )
    {
        pCertObject->AddRef();
        I_CryptInsertLruEntry( hEntry, pCertObject );
        I_CryptReleaseLruEntry( hEntry );
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CChainCallContext::FindObjectInCreationCache, public
//
//  Synopsis:   find a certificate object in the creation cache
//
//----------------------------------------------------------------------------
PCCERTOBJECT
CChainCallContext::FindObjectInCreationCache (
                       IN PCRYPT_DATA_BLOB pIdentifier
                       )
{
    HLRUENTRY    hFound;
    PCCERTOBJECT pFound = NULL;

    hFound = I_CryptFindLruEntry( m_hObjectCreationCache, pIdentifier );
    if ( hFound != NULL )
    {
        pFound = (PCCERTOBJECT)I_CryptGetLruEntryData( hFound );
        pFound->AddRef();

        I_CryptReleaseLruEntry( hFound );
    }

    return( pFound );
}

//+---------------------------------------------------------------------------
//
//  Function:   CallContextCreateCallObject
//
//  Synopsis:   create a chain call context object
//
//----------------------------------------------------------------------------
BOOL WINAPI
CallContextCreateCallObject (
    OUT PCCHAINCALLCONTEXT* ppCallContext
    )
{
    BOOL               fResult = FALSE;
    PCCHAINCALLCONTEXT pCallContext;

    pCallContext = new CChainCallContext( fResult );
    if ( pCallContext == NULL )
    {
        SetLastError( E_OUTOFMEMORY );
        return( FALSE );
    }

    if ( fResult == TRUE )
    {
        *ppCallContext = pCallContext;
    }
    else
    {
        CallContextFreeCallObject( pCallContext );
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   CallContextFreeCallObject
//
//  Synopsis:   free the chain call context object
//
//----------------------------------------------------------------------------
VOID WINAPI
CallContextFreeCallObject (
    IN PCCHAINCALLCONTEXT pCallContext
    )
{
    delete pCallContext;
}

//+---------------------------------------------------------------------------
//
//  Function:   CallContextOnCreationCacheObjectRemoval
//
//  Synopsis:   removal notification callback
//
//----------------------------------------------------------------------------
VOID WINAPI
CallContextOnCreationCacheObjectRemoval (
    IN LPVOID pv,
    IN LPVOID pvRemovalContext
    )
{
    ( (PCCERTOBJECT)pv )->Release();
}
