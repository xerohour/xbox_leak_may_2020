//+---------------------------------------------------------------------------
//
//  Microsoft Windows NT Security
//  Copyright (C) Microsoft Corporation, 1997 - 1998
//
//  File:       ssctl.h
//
//  Contents:   Self Signed Certificate Trust List Subsystem used by the
//              Certificate Chaining Infrastructure for building complex
//              chains
//
//  History:    02-Feb-98    kirtd    Created
//
//----------------------------------------------------------------------------
#if !defined(__SSCTL_H__)
#define __SSCTL_H__

#include <chain.h>

//
// CSSCtlObject.  This is the main object for caching trust information about
// a self signed certificate trust list
//

typedef struct _SSCTL_SIGNER_INFO {

    PCMSG_SIGNER_INFO      pMessageSignerInfo;
    CERT_OBJECT_IDENTIFIER SignerCertObjectIdentifier;
    BOOL                   fSignerHashAvailable;
    DWORD                  SignerCertificateHashSize;
    BYTE                   SignerCertificateHash[ MD5DIGESTLEN ];

} SSCTL_SIGNER_INFO, *PSSCTL_SIGNER_INFO;

class CSSCtlObject
{
public:

    //
    // Construction
    //

    CSSCtlObject (
          IN PCCERTCHAINENGINE pChainEngine,
          IN PCCTL_CONTEXT pCtlContext,
          OUT BOOL& rfResult
          );

    ~CSSCtlObject ();

    //
    // Reference counting
    //

    inline VOID AddRef ();
    inline VOID Release ();

    //
    // Trust information access
    //

    inline PCCTL_CONTEXT CtlContext ();

    BOOL GetSigner (
            IN PCCERTOBJECT pSubject,
            IN PCCHAINCALLCONTEXT pCallContext,
            IN HCERTSTORE hAdditionalStore,
            OUT PCCERTOBJECT* ppSigner,
            OUT BOOL* pfAdditionalStoreSigner,
            OUT BOOL* pfCtlSignatureValid
            );

    BOOL GetTrustListInfo (
            IN PCCERTOBJECT pCertObject,
            OUT PCERT_TRUST_LIST_INFO* ppTrustListInfo
            );

    VOID CalculateStatus (
                  IN LPFILETIME pTime,
                  IN PCERT_USAGE_MATCH pRequestedUsage,
                  IN DWORD dwFlags,
                  OUT PCERT_TRUST_STATUS pStatus
                  );

    BOOL GetSignerRequestedUsageAndTime (
            IN LPFILETIME pCurrentTime,
            OUT PCERT_USAGE_MATCH pSignerRequestedUsage,
            OUT LPFILETIME pSignerRequestedTime
            );

    //
    // Hash access
    //

    inline DWORD CtlHashSize ();
    inline LPBYTE CtlHash ();

    //
    // Index entry handles
    //

    inline HLRUENTRY HashIndexEntry ();

private:

    //
    // Reference count
    //

    ULONG                  m_cRefs;

    //
    // Self Signed Certificate Trust List Context
    //

    PCCTL_CONTEXT          m_pCtlContext;

    //
    // MD5 Hash of CTL
    //

    BYTE                   m_CtlHash[ MD5DIGESTLEN ];

    //
    // Signer information
    //

    SSCTL_SIGNER_INFO      m_SignerInfo;
    BOOL                   m_fHasSignatureBeenVerified;
    BOOL                   m_fSignatureValid;

    //
    // Message Store
    //

    HCERTSTORE             m_hMessageStore;

    //
    // Hash Index Entry
    //

    HLRUENTRY              m_hHashEntry;

    //
    // Chain engine
    //

    PCCERTCHAINENGINE      m_pChainEngine;
};

//
// CSSCtlObjectCache.  Cache of self signed certificate trust list objects
// indexed by hash. Note that this cache is NOT LRU maintained.  We expect
// the number of these objects to be small
//

typedef BOOL (WINAPI *PFN_ENUM_SSCTLOBJECTS) (
                          IN LPVOID pvParameter,
                          IN PCSSCTLOBJECT pSSCtlObject
                          );

class CSSCtlObjectCache
{
public:

    //
    // Construction
    //

    CSSCtlObjectCache (
          OUT BOOL& rfResult
          );

    ~CSSCtlObjectCache ();

    //
    // Object Management
    //

    BOOL PopulateCache (
                 IN PCCERTCHAINENGINE pChainEngine
                 );

    BOOL AddObject (
            IN PCSSCTLOBJECT pSSCtlObject,
            IN BOOL fCheckForDuplicate
            );

    VOID RemoveObject (
               IN PCSSCTLOBJECT pSSCtlObject
               );

    //
    // Access the indexes
    //

    inline HLRUCACHE HashIndex ();

    //
    // Searching and Enumeration
    //

    PCSSCTLOBJECT FindObjectByHash (
                      IN PCRYPT_HASH_BLOB pHash
                      );

    VOID EnumObjects (
             IN PFN_ENUM_SSCTLOBJECTS pfnEnum,
             IN LPVOID pvParameter
             );

    //
    // Resync
    //

    BOOL Resync (IN PCCERTCHAINENGINE pChainEngine);

private:

    //
    // Hash Index
    //

    HLRUCACHE m_hHashIndex;
};

//
// Object removal notification function
//

VOID WINAPI
SSCtlOnRemovalFromCache (
     IN LPVOID pv,
     IN OPTIONAL LPVOID pvRemovalContext
     );

//
// SSCtl Subsystem Utility Function Prototypes
//

BOOL WINAPI
SSCtlGetSignerInfo (
     IN PCCTL_CONTEXT pCtlContext,
     OUT PSSCTL_SIGNER_INFO pSignerInfo
     );

VOID WINAPI
SSCtlFreeSignerInfo (
     IN PSSCTL_SIGNER_INFO pSignerInfo
     );

BOOL WINAPI
SSCtlGetSignerCertificateObject (
     IN PCCERTCHAINENGINE pChainEngine,
     IN PCCERTOBJECT pSubject,
     IN PCCHAINCALLCONTEXT pCallContext,
     IN PSSCTL_SIGNER_INFO pSignerInfo,
     IN HCERTSTORE hAdditionalStore,
     OUT PCCERTOBJECT* ppCertObject,
     OUT BOOL* pfAdditionalStoreSigner
     );

PCCERT_CONTEXT WINAPI
SSCtlFindCertificateInStoreByIssuerAndSerialNumber (
     IN HCERTSTORE hStore,
     IN PCCERT_CONTEXT pCertContext,
     IN PCERT_NAME_BLOB pIssuer,
     IN PCRYPT_INTEGER_BLOB pSerialNumber
     );

PCCERT_CONTEXT WINAPI
SSCtlFindCertificateInStoreByHash (
     IN HCERTSTORE hStore,
     IN PCRYPT_HASH_BLOB pHashBlob
     );

VOID WINAPI
SSCtlGetCtlTrustStatus (
     IN PCCTL_CONTEXT pCtlContext,
     IN BOOL fSignatureValid,
     IN LPFILETIME pTime,
     IN PCERT_USAGE_MATCH pRequestedUsage,
     IN DWORD dwFlags,
     OUT PCERT_TRUST_STATUS pStatus
     );

BOOL WINAPI
SSCtlPopulateCacheFromCertStore (
     IN PCCERTCHAINENGINE pChainEngine,
     IN OPTIONAL HCERTSTORE hStore
     );

BOOL WINAPI
SSCtlCreateCtlObject (
     IN PCCERTCHAINENGINE pChainEngine,
     IN PCCTL_CONTEXT pCtlContext,
     OUT PCSSCTLOBJECT* ppSSCtlObject
     );

typedef struct _SSCTL_ENUM_OBJECTS_DATA {

    PFN_ENUM_SSCTLOBJECTS pfnEnumObjects;
    LPVOID                pvEnumParameter;

} SSCTL_ENUM_OBJECTS_DATA, *PSSCTL_ENUM_OBJECTS_DATA;

BOOL WINAPI
SSCtlEnumObjectsWalkFn (
     IN LPVOID pvParameter,
     IN HLRUENTRY hEntry
     );

BOOL WINAPI
SSCtlCreateObjectCache (
     OUT PCSSCTLOBJECTCACHE* ppSSCtlObjectCache
     );

VOID WINAPI
SSCtlFreeObjectCache (
     IN PCSSCTLOBJECTCACHE pSSCtlObjectCache
     );

VOID WINAPI
SSCtlFreeTrustListInfo (
     IN PCERT_TRUST_LIST_INFO pTrustListInfo
     );

BOOL WINAPI
SSCtlAllocAndCopyTrustListInfo (
     IN PCERT_TRUST_LIST_INFO pTrustListInfo,
     OUT PCERT_TRUST_LIST_INFO* ppTrustListInfo
     );

//
// Inline methods
//

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObject::AddRef, public
//
//  Synopsis:   add a reference
//
//----------------------------------------------------------------------------
inline VOID
CSSCtlObject::AddRef ()
{
    InterlockedIncrement( (LONG *)&m_cRefs );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObject::Release, public
//
//  Synopsis:   release a reference
//
//----------------------------------------------------------------------------
inline VOID
CSSCtlObject::Release ()
{
    if ( InterlockedDecrement( (LONG *)&m_cRefs ) == 0 )
    {
        delete this;
    }
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObject::CtlContext, public
//
//  Synopsis:   return the CTL context
//
//----------------------------------------------------------------------------
inline PCCTL_CONTEXT
CSSCtlObject::CtlContext ()
{
    return( m_pCtlContext );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObject::CtlHashSize, public
//
//  Synopsis:   return the hash size
//
//----------------------------------------------------------------------------
inline DWORD
CSSCtlObject::CtlHashSize ()
{
    return( MD5DIGESTLEN );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObject::CtlHash, public
//
//  Synopsis:   return the hash
//
//----------------------------------------------------------------------------
inline LPBYTE
CSSCtlObject::CtlHash ()
{
    return( m_CtlHash );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObject::HashIndexEntry, public
//
//  Synopsis:   return the hash index entry
//
//----------------------------------------------------------------------------
inline HLRUENTRY
CSSCtlObject::HashIndexEntry ()
{
    return( m_hHashEntry );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObjectCache::HashIndex, public
//
//  Synopsis:   return the hash index
//
//----------------------------------------------------------------------------
inline HLRUCACHE
CSSCtlObjectCache::HashIndex ()
{
    return( m_hHashIndex );
}

#endif
