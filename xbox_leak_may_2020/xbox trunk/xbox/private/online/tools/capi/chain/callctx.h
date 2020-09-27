//+---------------------------------------------------------------------------
//
//  Microsoft Windows NT Security
//  Copyright (C) Microsoft Corporation, 1997 - 1998
//
//  File:       callctx.h
//
//  Contents:   Certificate Chaining Infrastructure Call Context
//
//  History:    02-Mar-98    kirtd    Created
//
//----------------------------------------------------------------------------
#if !defined(__CALLCTX_H__)
#define __CALLCTX_H__

#include <chain.h>

#define CYCLE_DETECTION_MODULUS        25
#define DEFAULT_CREATION_CACHE_BUCKETS 13

//
// The call context object provides a mechanism for packaging and passing
// around per-call data in the certificate chaining infrastructure.
//

class CChainCallContext
{
public:

    //
    // Construction
    //

    CChainCallContext (
          OUT BOOL& rfResult
          );

    ~CChainCallContext ();

    //
    // Cert Object Creation Cache
    //
    // This caches all certificate objects created in the context of this
    // call.
    //

    BOOL AddObjectToCreationCache (
            IN PCCERTOBJECT pCertObject
            );

    PCCERTOBJECT FindObjectInCreationCache (
                     IN PCRYPT_DATA_BLOB pIdentifier
                     );

    inline HLRUCACHE CreationCache ();

private:

    //
    // Cert Object Creation cache
    //
    // NOTE: LRU is turned off
    //

    HLRUCACHE m_hObjectCreationCache;
};

//
// Call Context Utility Functions
//

BOOL WINAPI
CallContextCreateCallObject (
    OUT PCCHAINCALLCONTEXT* ppCallContext
    );

VOID WINAPI
CallContextFreeCallObject (
    IN PCCHAINCALLCONTEXT pCallContext
    );

VOID WINAPI
CallContextOnCreationCacheObjectRemoval (
    IN LPVOID pv,
    IN LPVOID pvRemovalContext
    );

//
// Inline methods
//

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::CreationCache, public
//
//  Synopsis:   creation cache
//
//----------------------------------------------------------------------------
inline HLRUCACHE
CChainCallContext::CreationCache ()
{
    return( m_hObjectCreationCache );
}

#endif
