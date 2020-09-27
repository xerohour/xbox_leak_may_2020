//+---------------------------------------------------------------------------
//
//  Microsoft Windows NT Security
//  Copyright (C) Microsoft Corporation, 1997 - 1998
//
//  File:       chain.h
//
//  Contents:   Certificate Chaining Infrastructure
//
//  History:    13-Jan-98    kirtd    Created
//
//----------------------------------------------------------------------------
#if !defined(__CHAIN_H__)
#define __CHAIN_H__

#include <windows.h>
#include <wincrypt.h>
#include <lrucache.h>
#include <msr_md5.h>

//
// Certificate Object Forward class declarations
//

class CCertObject;
class CCertIssuerList;
class CCertObjectCache;
class CCertChainEngine;
class CCertChainContext;

//
// Certificate Object Class pointer typedefs
//

typedef CCertObject*       PCCERTOBJECT;
typedef CCertIssuerList*   PCCERTISSUERLIST;
typedef CCertObjectCache*  PCCERTOBJECTCACHE;
typedef CCertChainEngine*  PCCERTCHAINENGINE;
typedef CCertChainContext* PCCERTCHAINCONTEXT;

//
// SSCTL Forward class declarations
//

class CSSCtlObject;
class CSSCtlObjectCache;

//
// SSCTL Class pointer typedefs
//

typedef class CSSCtlObject*      PCSSCTLOBJECT;
typedef class CSSCtlObjectCache* PCSSCTLOBJECTCACHE;

//
// Call Context Forward class declarations
//

class CChainCallContext;
class CChainPathObject;

//
// Call Context class pointer typedefs
//

typedef CChainCallContext* PCCHAINCALLCONTEXT;
typedef CChainPathObject*  PCCHAINPATHOBJECT;

//
// Certificate Object Identifier.  This is a unique identifier for a certificate
// object and is the MD5 hash of the issuer and serial no.
//

typedef BYTE CERT_OBJECT_IDENTIFIER[ MD5DIGESTLEN ];

//
// CCertObject.  This is the main object used for caching trust information
// about a certificate
//

class CCertObject
{
public:

    //
    // Construction
    //

    CCertObject (
         IN PCCERTCHAINENGINE pChainEngine,
         IN PCCHAINCALLCONTEXT pCallContext,
         IN PCCERT_CONTEXT pCertContext,
         IN PCRYPT_HASH_BLOB pCertHash,
         IN PCERT_TRUST_STATUS pKnownStatus,
         IN HCERTSTORE hAdditionalStore,
         OUT BOOL& rfResult
         );

    ~CCertObject ();

    //
    // Reference counting
    //

    inline VOID AddRef ();
    inline VOID Release ();

    //
    // Identifier access
    //

    inline VOID GetObjectIdentifier (
                   OUT CERT_OBJECT_IDENTIFIER ObjectIdentifier
                   );

    //
    // Trust information access
    //

    inline PCCERT_CONTEXT CertContext ();

    inline PCERT_ENHKEY_USAGE Usage ();

    inline DWORD ErrorStatus ();

    inline DWORD InfoStatus ();

    inline PCERT_TRUST_STATUS TrustStatus ();

    VOID CalculateAdditionalStatus (
                  IN LPFILETIME pTime,
                  IN PCERT_USAGE_MATCH pRequestedUsage,
                  IN DWORD dwFlags,
                  OUT PCERT_TRUST_STATUS pAdditionalStatus
                  );

    DWORD ChainErrorStatus ();

    //
    // Hash access
    //

    inline DWORD CertificateHashSize ();
    inline LPBYTE CertificateHash ();

    //
    // Key identifier access
    //

    inline DWORD KeyIdentifierSize ();
    inline LPBYTE KeyIdentifier ();

    //
    // Issuer access
    //

    inline PCERT_AUTHORITY_KEY_ID_INFO AuthorityKeyIdentifier ();

    inline PCCERTISSUERLIST IssuerList ();

    BOOL GetIssuer (
            IN PCCHAINCALLCONTEXT pCallContext,
            IN LPFILETIME pTime,
            IN LPFILETIME pCurrentTime,
            IN HCERTSTORE hStore,
            IN PCERT_USAGE_MATCH pRequestedUsage,
            IN DWORD dwFlags,
            OUT PCCERTOBJECT* ppCertObject,
            OUT PCERT_TRUST_STATUS pSubjectStatus,
            OUT PCERT_TRUST_STATUS pAdditionalStatus,
            OUT PCERT_TRUST_STATUS pCtlStatus,
            OUT PCERT_TRUST_LIST_INFO* ppTrustListInfo,
            OUT PCERT_USAGE_MATCH pTrustListRequestedUsage,
            OUT LPFILETIME pTrustListRequestedTime
            );

    //
    // The index entry handles for the Certificate Object Cache are maintained
    // on the object itself.  The primary index entry is the hash index entry
    //

    inline HLRUENTRY HashIndexEntry ();

    inline HLRUENTRY IdentifierIndexEntry ();

    inline HLRUENTRY SubjectNameIndexEntry ();

    inline HLRUENTRY IssuerNameIndexEntry ();

    inline HLRUENTRY KeyIdIndexEntry ();

    //
    // Chain engine access
    //

    inline PCCERTCHAINENGINE ChainEngine ();

private:

    //
    // Reference count
    //

    ULONG                       m_cRefs;

    //
    // Certificate Object Identifier
    //

    CERT_OBJECT_IDENTIFIER      m_ObjectIdentifier;

    //
    // Certificate context
    //

    PCCERT_CONTEXT              m_pCertContext;

    //
    // MD5 Hash of the certificate
    //

    BYTE                        m_CertificateHash[ MD5DIGESTLEN ];

    //
    // Key Identifier of the certificate
    //

    DWORD                       m_cbKeyIdentifier;
    LPBYTE                      m_pbKeyIdentifier;

    //
    // Trust Status.  This does not represent the full trust status
    // for the object.  Some of the bits are calculated on demand and placed
    // into the ending chain context.  The following are the trust status
    // bits which can appear here
    //
    // CERT_TRUST_IS_SELF_SIGNED
    // CERT_TRUST_HAS_EXACT_MATCH_ISSUER
    // CERT_TRUST_HAS_NAME_MATCH_ISSUER
    // CERT_TRUST_HAS_KEY_MATCH_ISSUER
    //
    // CERT_TRUST_IS_NOT_SIGNATURE_VALID (if the certificate is self-signed)
    // CERT_TRUST_IS_UNTRUSTED_ROOT (if the certificate is self-signed)
    //

    CERT_TRUST_STATUS           m_TrustStatus;

    //
    // Usage.  This usage has been merged and sorted based on extensions and
    // properties on the certificate context.
    //

    PCERT_ENHKEY_USAGE          m_pUsage;

    //
    // Authority Key Identifier.  This contains the issuer and serial number
    // of the issuing certificate for this certificate object if the trust
    // status includes CERT_TRUST_HAS_EXACT_MATCH_ISSUER
    //

    PCERT_AUTHORITY_KEY_ID_INFO m_pAuthKeyIdentifier;

    //
    // Issuer Certificate Objects.  The list of issuers of this
    // certificate object along with information about those issuers
    // relevant to this subject.
    //

    PCCERTISSUERLIST            m_pIssuerList;

    //
    // Certificate Chain Engine which owns this certificate object
    //

    PCCERTCHAINENGINE           m_pChainEngine;

    //
    // Certificate Object Cache Index entries
    //

    HLRUENTRY                   m_hHashEntry;
    HLRUENTRY                   m_hIdentifierEntry;
    HLRUENTRY                   m_hSubjectNameEntry;
    HLRUENTRY                   m_hIssuerNameEntry;
    HLRUENTRY                   m_hKeyIdEntry;
};

//
// CCertIssuerList.  List of issuer certificate objects along with related
// issuer information.  This is used by the certificate object to cache
// its possible set of issuers
//

// Currently in a self signed certificate object, the issuer elements will
// have CTL issuer data set and pIssuer may be NULL if we haven't yet gotten
// the CTL signer

typedef struct _CTL_ISSUER_DATA {

    PCSSCTLOBJECT         pSSCtlObject;
    PCERT_TRUST_LIST_INFO pTrustListInfo;
    BOOL                  fAdditionalStoreSigner;

} CTL_ISSUER_DATA, *PCTL_ISSUER_DATA;

typedef struct _CERT_ISSUER_ELEMENT {

    BOOL                         fCtlIssuer;
    PCCERTOBJECT                 pIssuer;
    CERT_TRUST_STATUS            SubjectStatus;
    CERT_TRUST_STATUS            ChainStatus;
    BOOL                         fAdditionalStore;
    PCTL_ISSUER_DATA             pCtlIssuerData;
    struct _CERT_ISSUER_ELEMENT* pPrevElement;
    struct _CERT_ISSUER_ELEMENT* pNextElement;

} CERT_ISSUER_ELEMENT, *PCERT_ISSUER_ELEMENT;

class CCertIssuerList
{
public:

    //
    // Construction
    //

    CCertIssuerList (
         IN PCCERTOBJECT pSubject
         );

    ~CCertIssuerList ();

    //
    // Issuer management
    //

    inline BOOL IsEmpty ();

    BOOL AddIssuer (
            IN PCCHAINCALLCONTEXT pCallContext,
            IN PCCERTOBJECT pIssuer,
            IN BOOL fAdditionalStore,
            IN BOOL fCheckForDuplicate
            );

    BOOL AddCtlIssuer (
            IN PCCHAINCALLCONTEXT pCallContext,
            IN PCSSCTLOBJECT pSSCtlObject,
            IN BOOL fAdditionalStore,
            IN HCERTSTORE hAdditionalStore,
            IN BOOL fCheckForDuplicate
            );

    //
    // List Flushing and Restoration
    //

    inline BOOL IsFlushed ();

    inline BOOL IsCtlIssuerFlushed ();

    VOID Flush ();

    VOID CtlIssuerFlush (IN PCCERTOBJECT pCtlIssuer);

    BOOL Restore (IN PCCHAINCALLCONTEXT pCallContext, IN BOOL fForce);

    //
    // Additional store element retrieval and purging
    //

    inline BOOL IsAdditionalStoreProcessed ();

    BOOL GetAdditionalStoreIssuers (
            IN PCCHAINCALLCONTEXT pCallContext,
            IN HCERTSTORE hAdditionalStore
            );

    VOID PurgeAdditionalStoreIssuers ();

    //
    // Element management
    //

    BOOL CreateElement (
               IN PCCHAINCALLCONTEXT pCallContext,
               IN BOOL fCtlIssuer,
               IN PCCERTOBJECT pIssuer,
               IN BOOL fAdditionalStore,
               IN HCERTSTORE hAdditionalStore,
               IN PCSSCTLOBJECT pSSCtlObject,
               IN PCERT_TRUST_LIST_INFO pTrustListInfo,
               OUT PCERT_ISSUER_ELEMENT* ppElement
               );

    VOID DeleteElement (
               IN PCERT_ISSUER_ELEMENT pElement
               );

    inline VOID AddElement (
                   IN PCERT_ISSUER_ELEMENT pElement
                   );

    inline VOID RemoveElement (
                      IN PCERT_ISSUER_ELEMENT pElement
                      );

    BOOL CheckForDuplicateElement (
              IN PCRYPT_HASH_BLOB pHashBlob,
              IN BOOL fCtlIssuer
              );

    //
    // Enumerate the issuers
    //

    inline PCERT_ISSUER_ELEMENT PrevElement (
                                    IN PCERT_ISSUER_ELEMENT pElement
                                    );

    inline PCERT_ISSUER_ELEMENT NextElement (
                                    IN PCERT_ISSUER_ELEMENT pElement
                                    );

private:

    //
    // Subject Certificate Object
    //
    // NOTE: This is NOT ref-counted
    //

    PCCERTOBJECT          m_pSubject;

    //
    // Signature Valid Issuer public key
    //

    PCERT_PUBLIC_KEY_INFO m_pIssuerPublicKey;

    //
    // Issuer List
    //

    PCERT_ISSUER_ELEMENT  m_pHead;

    //
    // Flush flag
    //

    BOOL                  m_fFlushed;
    BOOL                  m_fCtlIssuerFlushed;

    //
    // Processed additional store
    //

    BOOL                  m_fAdditionalStoreProcessed;
};

//
// The implementation of CCertObject::GetIssuer will choose the issuer with
// the highest quality from the issuer list.  Quality Values are as follows:
//
// BUGBUG: Consider minimum quality level configuration for chain engine
//

#define CERT_QUALITY_ISSUER_TIME_NESTED     0x00000001
#define CERT_QUALITY_CHAIN_TIME_NESTED      0x00000002
#define CERT_QUALITY_ISSUER_TIME_VALID      0x00000004
#define CERT_QUALITY_MEETS_USAGE_CRITERIA   0x00000008
#define CERT_QUALITY_HAS_PROTECTED_ROOT     0x00000010
#define CERT_QUALITY_ISSUER_SIGNATURE_VALID 0x00000020
#define CERT_QUALITY_CHAIN_SIGNATURE_VALID  0x00000040

// Large negative value for minimum quality
#define CERT_QUALITY_MINIMUM                0xFFFFFFFF

//
// CCertObjectCache.  Cache of certificate object references indexed by the
// following keys:
//
//      Certificate Hash
//      Certificate Object Identifier
//      Subject Name
//      Issuer Name
//
// The indexes are built upon hash tables of which the primary index (The
// Certificate Hash) is LRU maintained.  Touching a cache entry therefore
// means touching the LRU entry on the primary index which is actually
// maintained on the certificate object itself.
//

#define DEFAULT_CERT_OBJECT_CACHE_BUCKETS 127
#define DEFAULT_MAX_INDEX_ENTRIES         256

typedef BOOL (WINAPI *PFN_ENUM_CERTOBJECTS) (
                          IN LPVOID pvParameter,
                          IN PCCERTOBJECT pCertObject
                          );

class CCertObjectCache
{
public:

    //
    // Construction
    //

    CCertObjectCache (
         IN DWORD MaxIndexEntries,
         OUT BOOL& rfResult
         );

    ~CCertObjectCache ();

    //
    // Certificate Object Management
    //

    BOOL AddObject (
            IN PCCHAINCALLCONTEXT pCallContext,
            IN PCCERTOBJECT pCertObject,
            IN BOOL fCheckForDuplicate
            );

    //
    // Access the indexes
    //

    inline HLRUCACHE HashIndex ();

    inline HLRUCACHE IdentifierIndex ();

    inline HLRUCACHE SubjectNameIndex ();

    inline HLRUCACHE IssuerNameIndex ();

    inline HLRUCACHE KeyIdIndex ();

    //
    // Certificate Object Searching
    //

    PCCERTOBJECT FindObjectByHash (
                     IN PCRYPT_HASH_BLOB pHash
                     );

    PCCERTOBJECT FindObjectByIdentifier (
                     IN CERT_OBJECT_IDENTIFIER ObjectIdentifier
                     );

    PCCERTOBJECT FindObjectBySubjectName (
                     IN PCERT_NAME_BLOB pSubjectName
                     );

    PCCERTOBJECT FindObjectByIssuerName (
                     IN PCERT_NAME_BLOB pIssuerName
                     );

    PCCERTOBJECT FindObjectByKeyId (
                     IN PCRYPT_HASH_BLOB pKeyId
                     );

    //
    // Certificate Object Enumeration
    //

    PCCERTOBJECT NextMatchingObjectByIdentifier (
                     IN PCCERTOBJECT pCertObject
                     );

    PCCERTOBJECT NextMatchingObjectBySubjectName (
                     IN PCCERTOBJECT pCertObject
                     );

    PCCERTOBJECT NextMatchingObjectByIssuerName (
                     IN PCCERTOBJECT pCertObject
                     );

    PCCERTOBJECT NextMatchingObjectByKeyId (
                     IN PCCERTOBJECT pCertObject
                     );

    VOID EnumObjects (
             IN PFN_ENUM_CERTOBJECTS pfnEnum,
             IN LPVOID pvParameter
             );

    //
    // LRU control
    //

    inline VOID EnableLruOfObjects (IN PCCHAINCALLCONTEXT pCallContext);

    inline VOID DisableLruOfObjects ();

    //
    // Cache flushing
    //

    inline VOID FlushObjects (IN PCCHAINCALLCONTEXT pCallContext);

private:

    //
    // Certificate Hash Index
    //

    HLRUCACHE m_hHashIndex;

    //
    // Certificate Object Identifier Index
    //

    HLRUCACHE m_hIdentifierIndex;

    //
    // Subject Name Index
    //

    HLRUCACHE m_hSubjectNameIndex;

    //
    // Issuer Name Index
    //

    HLRUCACHE m_hIssuerNameIndex;

    //
    // Key Identifier Index
    //

    HLRUCACHE m_hKeyIdIndex;

    //
    // Private methods
    //

    PCCERTOBJECT FindObject (
                     IN HLRUCACHE hIndex,
                     IN PCRYPT_DATA_BLOB pIdentifier
                     );

    PCCERTOBJECT NextMatchingObject (
                     IN HLRUENTRY hObjectEntry,
                     IN PCCERTOBJECT pCertObject
                     );
};

//
// Certificate Object Cache Primary Index Entry Removal Notification
//
// Free entry data from certificate object cache when LRU'd from the primary
// index ( the certificate hash ).  This should remove the relevant entries
// from the other indexes and release the reference on the certificate object
// maintained by the primary index.
//

VOID WINAPI
CertObjectCacheOnRemovalFromPrimaryIndex (
    IN LPVOID pv,
    IN LPVOID pvRemovalContext
    );

//
// Certificate Object Cache Identifier Hashing Functions
//

DWORD WINAPI
CertObjectCacheHashMd5Identifier (
    IN PCRYPT_DATA_BLOB pIdentifier
    );

DWORD WINAPI
CertObjectCacheHashNameIdentifier (
    IN PCRYPT_DATA_BLOB pIdentifier
    );

//
// Certificate Object Cache Enumerator Function
//

typedef struct _CHAIN_ENUM_OBJECTS_DATA {

    PFN_ENUM_CERTOBJECTS pfnEnumObjects;
    LPVOID               pvEnumParameter;

} CHAIN_ENUM_OBJECTS_DATA, *PCHAIN_ENUM_OBJECTS_DATA;

BOOL WINAPI
ChainEnumObjectsWalkFn (
     IN LPVOID pvParameter,
     IN HLRUENTRY hEntry
     );

//
// CCertChainEngine.  The chaining engine satisfies requests for chain contexts
// given some set of parameters.  In order to make the building of these
// contexts efficient, the chain engine caches trust and chain information
// for certificates
//

class CCertChainEngine
{
public:

    //
    // Construction
    //

    CCertChainEngine (
         IN PCERT_CHAIN_ENGINE_CONFIG pConfig,
         IN BOOL fDefaultEngine,
         OUT BOOL& rfResult
         );

    ~CCertChainEngine ();

    //
    // Chain Engine Locking
    //

    inline VOID LockEngine ();
    inline VOID UnlockEngine ();

    //
    // Chain Engine reference counting
    //

    inline VOID AddRef ();
    inline VOID Release ();

    //
    // Cache access
    //

    inline PCCERTOBJECTCACHE CertObjectCache ();
#ifdef CAPI_INCLUDE_CTL    
    inline PCSSCTLOBJECTCACHE SSCtlObjectCache ();
#endif    

    //
    // Store access
    //

    inline HCERTSTORE RootStore ();
    inline HCERTSTORE RealRootStore ();
    inline HCERTSTORE TrustStore ();
    inline HCERTSTORE OtherStore ();

    //
    // Cycle detection modulus access
    //

    inline DWORD CycleDetectionModulus ();

    //
    // Chain Context Retrieval
    //

    BOOL GetChainContext (
            IN PCCERT_CONTEXT pCertContext,
            IN LPFILETIME pTime,
            IN LPFILETIME pCurrentTime,
            IN HCERTSTORE hAdditionalStore,
            IN PCERT_USAGE_MATCH pRequestedUsage,
            IN DWORD dwFlags,
            IN LPVOID pvReserved,
            OUT PCCERT_CHAIN_CONTEXT* ppChainContext
            );

    //
    // Simple Chain Retrieval
    //

    BOOL AddSimpleChain (
            IN PCCHAINCALLCONTEXT pCallContext,
            IN PCCERTCHAINCONTEXT pCertChainContext,
            IN DWORD NewSimpleChainIndex,
            IN PCCERTOBJECT pEndCertObject,
            IN PCERT_TRUST_STATUS pEndAdditionalStatus,
            IN LPFILETIME pTime,
            IN LPFILETIME pCurrentTime,
            IN HCERTSTORE hAdditionalStore,
            IN PCERT_USAGE_MATCH pRequestedUsage,
            IN DWORD dwFlags,
            IN LPVOID pvReserved,
            OUT PCCERTOBJECT* ppNextEndCertObject,
            OUT PCERT_TRUST_STATUS pNextEndAdditionalStatus,
            OUT PCERT_USAGE_MATCH pNextEndRequestedUsage,
            OUT LPFILETIME pNextEndRequestedTime
            );

    //
    // Find and store issuers for a particular certificate either exact match
    // or name match algorithms
    //

    BOOL FindAndStoreMatchingIssuers (
             IN PCCERTOBJECT pCertObject,
             IN PCCHAINCALLCONTEXT pCallContext
             );

    BOOL FindAndStoreExactMatchIssuers (
             IN PCCERTOBJECT pCertObject,
             IN PCCHAINCALLCONTEXT pCallContext
             );

    BOOL FindAndStoreNameMatchIssuers (
             IN PCCERTOBJECT pCertObject,
             IN PCCHAINCALLCONTEXT pCallContext
             );

    BOOL FindAndStoreKeyMatchIssuers (
             IN PCCERTOBJECT pCertObject,
             IN PCCHAINCALLCONTEXT pCallContext
             );

    BOOL RetrieveAndStoreMatchingIssuersByUrl (
                 IN PCCERTOBJECT pCertObject,
                 IN PCCHAINCALLCONTEXT pCallContext,
                 IN DWORD dwFlags
                 );

    BOOL FindAndStoreCtlIssuers (
             IN PCCERTOBJECT pCertObject,
             IN PCCHAINCALLCONTEXT pCallContext
             );

    //
    // Resync the engine
    //

    BOOL Resync (IN PCCHAINCALLCONTEXT pCallContext, BOOL fForce);

private:

    //
    // Reference count
    //

    ULONG                    m_cRefs;

    //
    // Engine Lock
    //

    CRITICAL_SECTION         m_Lock;

    //
    // Root store ( Certs )
    //

    HCERTSTORE               m_hRealRootStore;
    HCERTSTORE               m_hRootStore;

    //
    // Trust Store Collection ( CTLs )
    //

    HCERTSTORE               m_hTrustStore;

    //
    // Other store collection ( Certs and CRLs )
    //

    HCERTSTORE               m_hOtherStore;

    //
    // Engine Store ( Collection of Root, Trust and Other )
    //

    HCERTSTORE               m_hEngineStore;

    //
    // Engine Store Change Notification Event
    //

    HANDLE                   m_hEngineStoreChangeEvent;

    //
    // Engine flags
    //

    DWORD                    m_dwFlags;

    //
    // Retrieval timeout
    //

    DWORD                    m_dwUrlRetrievalTimeout;

    //
    // Cycle detection modulus
    //

    DWORD                    m_CycleDetectionModulus;

    //
    // Certificate Object Cache
    //

    PCCERTOBJECTCACHE        m_pCertObjectCache;

#ifdef CAPI_INCLUDE_CTL
    //
    // Self Signed Certificate Trust List Object Cache
    //

    PCSSCTLOBJECTCACHE       m_pSSCtlObjectCache;
#endif    
};

//
// CCertChainContext.  The chain context object is used to manage the creation
// of the chain context data structure which is exported to clients of this
// infrastructure.
//

#define INITIAL_CHAIN_ALLOC                1
#define GROW_CHAIN_ALLOC                   2
#define GROW_ELEMENT_ALLOC                 5

typedef struct _INTERNAL_CERT_CHAIN_CONTEXT {

    CERT_CHAIN_CONTEXT ChainContext;
    ULONG              cRefs;
    DWORD              cSimpleChain;
    DWORD*             rgElementCount;
    DWORD              TotalElements;

} INTERNAL_CERT_CHAIN_CONTEXT, *PINTERNAL_CERT_CHAIN_CONTEXT;

class CCertChainContext
{
public:

    //
    // Construction
    //

    CCertChainContext (OUT BOOL& rfResult);

    ~CCertChainContext ();

    //
    // Context management
    //

    BOOL AddChainElement (
            IN DWORD SimpleChain,
            IN PCCERTOBJECT pCertObject,
            IN PCERT_TRUST_STATUS pSubjectStatus,
            IN PCERT_TRUST_STATUS pAdditionalStatus,
            IN PCERT_TRUST_STATUS pCtlStatus,
            IN PCERT_TRUST_LIST_INFO pTrustListInfo
            );

    inline DWORD ElementCount (
                        IN DWORD SimpleChain
                        );

    VOID UpdateSummaryStatus ();

    VOID UpdateEndEntityCertContext (PCCERT_CONTEXT pEndCertContext);

    //
    // Access the context
    //

    inline PINTERNAL_CERT_CHAIN_CONTEXT InternalContext ();
    inline PCERT_CHAIN_CONTEXT ExposedContext ();

private:

    //
    // Chain Context
    //

    PINTERNAL_CERT_CHAIN_CONTEXT m_pContext;

    //
    // Private methods
    //

    BOOL GrowSimpleChains (IN DWORD cGrowChain);

    BOOL GrowChainElements (IN DWORD SimpleChain, IN DWORD cGrowElement);
};

//
// Chain Infrastructure Utility Function Prototypes
//

VOID WINAPI
ChainCreateCertificateObjectIdentifier (
     IN PCERT_NAME_BLOB pIssuer,
     IN PCRYPT_INTEGER_BLOB pSerialNumber,
     OUT CERT_OBJECT_IDENTIFIER ObjectIdentifier
     );

BOOL WINAPI
ChainGetCertificateUsage (
     IN PCCERT_CONTEXT pCertContext,
     OUT PCERT_ENHKEY_USAGE* ppUsage
     );

VOID WINAPI
ChainFreeCertificateUsage (
     IN PCERT_ENHKEY_USAGE pUsage
     );

BOOL WINAPI
ChainCreateIssuerList (
     IN PCCERTOBJECT pSubject,
     OUT PCCERTISSUERLIST* ppIssuerList
     );

VOID WINAPI
ChainFreeIssuerList (
     IN PCCERTISSUERLIST pIssuerList
     );

VOID WINAPI
ChainGetSelfSignedStatus (
     IN PCCERTOBJECT pCertObject,
     OUT PCERT_TRUST_STATUS pStatus
     );

VOID WINAPI
ChainGetRootStoreStatus (
     IN HCERTSTORE hRoot,
     IN HCERTSTORE hRealRoot,
     IN PCRYPT_HASH_BLOB pMd5Hash,
     OUT PCERT_TRUST_STATUS pStatus
     );

VOID WINAPI
ChainGetIssuerMatchStatus (
     IN PCCERT_CONTEXT pCertContext,
     OUT PCERT_AUTHORITY_KEY_ID_INFO* ppAuthKeyIdInfo,
     OUT PCERT_TRUST_STATUS pStatus
     );

VOID WINAPI
ChainFreeAuthorityKeyIdentifier (
     IN PCERT_AUTHORITY_KEY_ID_INFO pAuthKeyIdInfo
     );

VOID WINAPI
ChainGetUsageStatus (
     IN PCERT_ENHKEY_USAGE pRequestedUsage,
     IN PCERT_ENHKEY_USAGE pAvailableUsage,
     IN DWORD dwMatchType,
     OUT PCERT_TRUST_STATUS pStatus
     );

BOOL WINAPI
ChainFindAndStoreMatchingIssuersFromCertStore (
     IN PCCERTOBJECT pCertObject,
     IN PCCHAINCALLCONTEXT pCallContext,
     IN HCERTSTORE hStore,
     IN BOOL fAdditionalStore,
     IN BOOL fCacheIssuers
     );

BOOL WINAPI
ChainCreateIssuerMatchStores (
     IN PCCERTOBJECT pCertObject,
     IN HCERTSTORE hStore,
     IN BOOL fCacheIssuers,
     OUT HCERTSTORE* phCacheIssuersStore,
     OUT HCERTSTORE* phIssuerListStore
     );

BOOL WINAPI
ChainIsMatchingIssuerCertificate (
     IN PCCERTOBJECT pSubject,
     IN PCCERT_CONTEXT pIssuer
     );

BOOL WINAPI
ChainIsInSameIssuerGrouping (
     IN PCCERTOBJECT pGroupMember,
     IN PCCERT_CONTEXT pPossibleGroupMember
     );

BOOL WINAPI
ChainCreateCertificateObject (
     IN PCCERTCHAINENGINE pChainEngine,
     IN PCCHAINCALLCONTEXT pCallContext,
     IN PCCERT_CONTEXT pCertContext,
     IN PCRYPT_HASH_BLOB pHashBlob,
     IN OPTIONAL PCERT_TRUST_STATUS pKnownStatus,
     IN OPTIONAL HCERTSTORE hAdditionalStore,
     OUT PCCERTOBJECT* ppCertObject,
     OUT BOOL* pfFromCallCreationCache
     );

LONG WINAPI
ChainGetIssuerQuality (
     IN PCERT_ISSUER_ELEMENT pElement,
     IN LPFILETIME pTime,
     IN LPFILETIME pCurrentTime,
     IN PCERT_USAGE_MATCH pRequestedUsage,
     IN DWORD dwFlags,
     OUT PCERT_TRUST_STATUS pAdditionalStatus,
     OUT PCERT_TRUST_STATUS pCtlStatus,
     OUT PCERT_USAGE_MATCH pCtlSignerRequestedUsage,
     OUT LPFILETIME pCtlSignerRequestedTime
     );

VOID WINAPI
ChainGetSubjectStatus (
     IN PCCERTOBJECT pIssuer,
     IN OPTIONAL PCERT_PUBLIC_KEY_INFO pIssuerPublicKey,
     IN PCCERTOBJECT pSubject,
     OUT PCERT_TRUST_STATUS pStatus
     );

VOID WINAPI
ChainTouchAllChosenIssuerRelatedObjects (
     IN PCCERTOBJECT pChosenIssuer
     );

VOID WINAPI
ChainTouchAllMatchingSubjectNames (
     IN PCCERTOBJECTCACHE pCertObjectCache,
     IN PCERT_NAME_BLOB pSubjectName,
     IN DWORD dwTouchFlags
     );

VOID WINAPI
ChainTouchAllMatchingObjectIds (
     IN PCCERTOBJECTCACHE pCertObjectCache,
     IN CERT_OBJECT_IDENTIFIER ObjectIdentifier,
     IN DWORD dwTouchFlags
     );

VOID WINAPI
ChainTouchAllMatchingKeyIds (
     IN PCCERTOBJECTCACHE pCertObjectCache,
     IN PCRYPT_HASH_BLOB pKeyId,
     IN DWORD dwTouchFlags
     );

VOID WINAPI
ChainTouchAllMatchingEntries (
     IN HLRUCACHE hIndex,
     IN PCRYPT_DATA_BLOB pIdentifier,
     IN DWORD dwTouchFlags
     );

VOID WINAPI
ChainRemoveCertificateObjectFromSecondaryCacheIndexes (
     IN PCCERTOBJECT pCertObject
     );

VOID WINAPI
ChainRemoveObjectsRelatedToThisIssuer (
     IN PCCERTOBJECT pRemovedIssuer
     );

VOID WINAPI
ChainRemoveAllMatchingSubjectNames (
     IN PCCERTOBJECTCACHE pCertObjectCache,
     IN PCERT_NAME_BLOB pSubjectName,
     IN DWORD dwRemovalFlags
     );

VOID WINAPI
ChainRemoveAllMatchingObjectIds (
     IN PCCERTOBJECTCACHE pCertObjectCache,
     IN CERT_OBJECT_IDENTIFIER ObjectIdentifier,
     IN DWORD dwRemovalFlags
     );

VOID WINAPI
ChainRemoveAllMatchingKeyIds (
     IN PCCERTOBJECTCACHE pCertObjectCache,
     IN PCRYPT_HASH_BLOB pKeyId,
     IN DWORD dwRemovalFlags
     );

VOID WINAPI
ChainRemoveAllMatchingEntries (
     IN HLRUCACHE hIndex,
     IN PCRYPT_DATA_BLOB pIdentifier,
     IN DWORD dwRemovalFlags
     );

VOID WINAPI
ChainFlushMatchingIssuerLists (
     IN PCCERTOBJECTCACHE pCertObjectCache,
     IN PCERT_NAME_BLOB pIssuerName
     );

VOID WINAPI
ChainFlushIssuerListsWithThisIssuer (
     IN PCCERTOBJECT pIssuer
     );

VOID WINAPI
ChainFlushIssuerListsWithThisIssuerByName (
     IN PCCERTOBJECT pIssuer
     );

VOID WINAPI
ChainFlushIssuerListsWithThisIssuerByObjectOrKeyId (
     IN PCCERTOBJECT pIssuer
     );

BOOL WINAPI
ChainFlushIssuerListsWithThisIssuerByObjectOrKeyIdEnumFn (
     IN LPVOID pvParameter,
     IN PCCERTOBJECT pCertObject
     );

VOID WINAPI
ChainFlushMatchingCtlIssuerLists (
     IN PCCERTOBJECT pCertObject
     );

BOOL WINAPI
ChainIsCertificateObjectCtlSigner (
     IN PCCERTOBJECT pCertObject
     );

typedef struct _FLUSH_CTL_ISSUERS_ENUM_DATA {

    PCCERTOBJECT pCtlIssuer;

} FLUSH_CTL_ISSUERS_ENUM_DATA, *PFLUSH_CTL_ISSUERS_ENUM_DATA;

BOOL WINAPI
ChainFlushMatchingCtlIssuerListsEnumFn (
     IN LPVOID pvParameter,
     IN PCCERTOBJECT pCertObject
     );

VOID WINAPI
ChainProcessRemovalNotification (
     IN PCCHAINCALLCONTEXT pCallContext,
     IN PCCERTOBJECT pCertObject,
     IN BOOL fProcessMatchingObjects
     );

BOOL WINAPI
ChainCreateCertificateObjectCache (
     IN DWORD MaxIndexEntries,
     OUT PCCERTOBJECTCACHE* ppCertObjectCache
     );

VOID WINAPI
ChainFreeCertificateObjectCache (
     IN PCCERTOBJECTCACHE pCertObjectCache
     );

BOOL WINAPI
ChainCreateCertificateChainContext (
     OUT PCCERTCHAINCONTEXT* ppCertChainContext
     );

VOID WINAPI
ChainFreeCertificateChainContext (
     IN PCCERTCHAINCONTEXT pCertChainContext
     );

VOID WINAPI
ChainAddRefInternalChainContext (
     IN PINTERNAL_CERT_CHAIN_CONTEXT pChainContext
     );

VOID WINAPI
ChainReleaseInternalChainContext (
     IN PINTERNAL_CERT_CHAIN_CONTEXT pChainContext
     );

VOID WINAPI
ChainFreeInternalChainContext (
     IN PINTERNAL_CERT_CHAIN_CONTEXT pChainContext
     );

#define CERT_TRUST_CERTIFICATE_ONLY_INFO_STATUS ( CERT_TRUST_IS_SELF_SIGNED |\
                                                  CERT_TRUST_HAS_EXACT_MATCH_ISSUER |\
                                                  CERT_TRUST_HAS_NAME_MATCH_ISSUER |\
                                                  CERT_TRUST_HAS_KEY_MATCH_ISSUER )

#define CERT_TRUST_CHAIN_FULL_ERROR_STATUS ( CERT_TRUST_IS_UNTRUSTED_ROOT |\
                                             CERT_TRUST_IS_NOT_SIGNATURE_VALID |\
                                             CERT_TRUST_IS_NOT_TIME_NESTED )

VOID WINAPI
ChainUpdateSummaryStatusByTrustStatus (
     IN PCERT_TRUST_STATUS pSummaryStatus,
     IN PCERT_TRUST_STATUS pTrustStatus
     );

VOID WINAPI
ChainOrInStatusBits (
     IN PCERT_TRUST_STATUS pDestStatus,
     IN PCERT_TRUST_STATUS pSourceStatus
     );

LONG WINAPI
ChainGetChainErrorStatusQuality (
     IN DWORD dwChainErrorStatus
     );

BOOL WINAPI
ChainCacheEndCertificate (
     IN PCCHAINCALLCONTEXT pCallContext,
     IN PCCERTOBJECT pEndCertObject
     );

BOOL WINAPI
ChainIsIssuerOfCertificates (
     IN PCCERTOBJECT pCertObject
     );

BOOL WINAPI
ChainConvertAuthKeyIdentifierFromV2ToV1 (
     IN PCERT_AUTHORITY_KEY_ID2_INFO pAuthKeyIdentifier2,
     OUT PCERT_AUTHORITY_KEY_ID_INFO* ppAuthKeyIdentifier
     );

typedef struct _CTL_ISSUERS_ENUM_DATA {

    PCCERTOBJECT       pCertObject;
    PCCHAINCALLCONTEXT pCallContext;

} CTL_ISSUERS_ENUM_DATA, *PCTL_ISSUERS_ENUM_DATA;

BOOL WINAPI
ChainFindAndStoreCtlIssuersEnumFn (
     IN LPVOID pvParameter,
     IN PCSSCTLOBJECT pSSCtlObject
     );

VOID WINAPI
ChainFreeCtlIssuerData (
     IN PCTL_ISSUER_DATA pCtlIssuerData
     );

VOID WINAPI
ChainFixupCtlIssuers (
     IN PCCERTOBJECT pSubject,
     IN PCCHAINCALLCONTEXT pCallContext,
     IN HCERTSTORE hAdditionalStore
     );

BOOL WINAPI
ChainFindAndStoreCtlIssuersFromAdditionalStore (
     IN PCCERTOBJECT pSubject,
     IN PCCHAINCALLCONTEXT pCallContext,
     IN HCERTSTORE hAdditionalStore
     );

VOID WINAPI
ChainFixupUntrustedRootElementStatus (
     IN PCERT_CHAIN_CONTEXT pContext
     );

BOOL WINAPI
ChainCreateWorldStore (
     IN HCERTSTORE hRoot,
     IN DWORD cAdditionalStore,
     IN HCERTSTORE* rghAdditionalStore,
     IN DWORD dwStoreFlags,
     OUT HCERTSTORE* phWorld
     );

BOOL WINAPI
ChainCreateEngineStore (
     IN HCERTSTORE hRootStore,
     IN HCERTSTORE hTrustStore,
     IN HCERTSTORE hOtherStore,
     IN BOOL fDefaultEngine,
     IN DWORD dwFlags,
     OUT HCERTSTORE* phEngineStore,
     OUT HANDLE* phEngineStoreChangeEvent
     );

BOOL WINAPI
ChainIsProperRestrictedRoot (
     IN HCERTSTORE hRealRoot,
     IN HCERTSTORE hRestrictedRoot
     );

BOOL WINAPI
ChainCopyRequestedUsage (
     IN PCERT_USAGE_MATCH pSource,
     OUT PCERT_USAGE_MATCH pDest
     );

VOID WINAPI
ChainFreeRequestedUsage (
     IN PCERT_USAGE_MATCH pRequestedUsage
     );

BOOL WINAPI
ChainIsCertificateObjectRepeatedInContext (
     IN PCCERTOBJECT pCertObject,
     IN PCERT_CHAIN_CONTEXT pContext
     );

VOID WINAPI
ChainFindAndAdjustChainContextToCycle (
     IN PCERT_CHAIN_CONTEXT pContext
     );

BOOL WINAPI
ChainIsElementRepeatedInContext (
     IN PCERT_CHAIN_CONTEXT pContext,
     IN DWORD iChain,
     IN DWORD iElement,
     OUT DWORD* piFoundChain,
     OUT DWORD* piFoundElement
     );

BOOL WINAPI
ChainCreateCollectionIncludingCtlCertificates (
     IN HCERTSTORE hStore,
     OUT HCERTSTORE* phCollection
     );

//
// Post processing revocation check
//
// BUGBUG: This WILL change!
//

#define CERT_CHAIN_REVOCATION_CHECK_ALL ( CERT_CHAIN_REVOCATION_CHECK_END_CERT | \
                                          CERT_CHAIN_REVOCATION_CHECK_CHAIN | \
                                          CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT | \
                                          CERT_CHAIN_REVOCATION_CHECK_CACHE_ONLY )

VOID WINAPI
ChainPostProcessCheckRevocation (
     IN HCERTSTORE hCrlStore,
     IN LPFILETIME pftTimeToUse,
     IN LPFILETIME pftCurrentTime,
     IN PCERT_CHAIN_CONTEXT pContext,
     IN DWORD dwFlags
     );

VOID WINAPI
ChainSetRevocationInfo (
     IN PCERT_CHAIN_CONTEXT pContext,
     IN PCERT_REVOCATION_STATUS pRevStatus,
     IN DWORD iChain,
     IN DWORD iElement
     );

VOID WINAPI
ChainFreeRevocationInfo (
     IN PCERT_REVOCATION_INFO pRevocationInfo
     );

//
// Cryptnet Thunk Helper API
//

typedef BOOL (WINAPI *PFN_GETOBJECTURL) (
                          IN LPCSTR pszUrlOid,
                          IN LPVOID pvPara,
                          IN DWORD dwFlags,
                          OUT OPTIONAL PCRYPT_URL_ARRAY pUrlArray,
                          IN OUT DWORD* pcbUrlArray,
                          OUT OPTIONAL PCRYPT_URL_INFO pUrlInfo,
                          IN OUT OPTIONAL DWORD* pcbUrlInfo,
                          IN OPTIONAL LPVOID pvReserved
                          );

BOOL WINAPI
ChainGetObjectUrl (
     IN LPCSTR pszUrlOid,
     IN LPVOID pvPara,
     IN DWORD dwFlags,
     OUT OPTIONAL PCRYPT_URL_ARRAY pUrlArray,
     IN OUT DWORD* pcbUrlArray,
     OUT OPTIONAL PCRYPT_URL_INFO pUrlInfo,
     IN OUT OPTIONAL DWORD* pcbUrlInfo,
     IN OPTIONAL LPVOID pvReserved
     );

typedef BOOL (WINAPI *PFN_RETRIEVEOBJECTBYURLW) (
                          IN LPCWSTR pszUrl,
                          IN LPCSTR pszObjectOid,
                          IN DWORD dwRetrievalFlags,
                          IN DWORD dwTimeout,
                          OUT LPVOID* ppvObject,
                          IN HCRYPTASYNC hAsyncRetrieve,
                          IN PCRYPT_CREDENTIALS pCredentials,
                          IN LPVOID pvVerify,
                          IN LPVOID pvReserved
                          );

BOOL WINAPI
ChainRetrieveObjectByUrlW (
     IN LPCWSTR pszUrl,
     IN LPCSTR pszObjectOid,
     IN DWORD dwRetrievalFlags,
     IN DWORD dwTimeout,
     OUT LPVOID* ppvObject,
     IN HCRYPTASYNC hAsyncRetrieve,
     IN PCRYPT_CREDENTIALS pCredentials,
     IN LPVOID pvVerify,
     IN LPVOID pvReserved
     );

HMODULE WINAPI
ChainGetCryptnetModule ();

//
// URL helper
//

BOOL WINAPI
ChainIsFileOrLdapUrl (
     IN LPCWSTR pwszUrl
     );

//
// Inline methods
//

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::AddRef, public
//
//  Synopsis:   add a reference to the certificate object
//
//----------------------------------------------------------------------------
inline VOID
CCertObject::AddRef ()
{
    InterlockedIncrement( (LONG *)&m_cRefs );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::Release, public
//
//  Synopsis:   remove a reference from the certificate object
//
//----------------------------------------------------------------------------
inline VOID
CCertObject::Release ()
{
    if ( InterlockedDecrement( (LONG *)&m_cRefs ) == 0 )
    {
        delete this;
    }
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::ObjectIdentifier, public
//
//  Synopsis:   returns the CERT_OBJECT_IDENTIFIER
//
//----------------------------------------------------------------------------
inline VOID
CCertObject::GetObjectIdentifier (CERT_OBJECT_IDENTIFIER ObjectIdentifier)
{
    memcpy(
       ObjectIdentifier,
       m_ObjectIdentifier,
       sizeof( CERT_OBJECT_IDENTIFIER )
       );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::CertContext, public
//
//  Synopsis:   return the certificate context
//
//----------------------------------------------------------------------------
inline PCCERT_CONTEXT
CCertObject::CertContext ()
{
    return( m_pCertContext );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::Usage, public
//
//  Synopsis:   return the usage
//
//----------------------------------------------------------------------------
inline PCERT_ENHKEY_USAGE
CCertObject::Usage ()
{
    return( m_pUsage );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::ErrorStatus, public
//
//  Synopsis:   return the error status bits
//
//----------------------------------------------------------------------------
inline DWORD
CCertObject::ErrorStatus ()
{
    return( m_TrustStatus.dwErrorStatus );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::InfoStatus, public
//
//  Synopsis:   return the info status bits
//
//----------------------------------------------------------------------------
inline DWORD
CCertObject::InfoStatus ()
{
    return( m_TrustStatus.dwInfoStatus );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::TrustStatus, public
//
//  Synopsis:   return a pointer to the trust status structure
//
//----------------------------------------------------------------------------
inline PCERT_TRUST_STATUS
CCertObject::TrustStatus ()
{
    return( &m_TrustStatus );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::CertificateHashSize, public
//
//  Synopsis:   return the certificate hash size
//
//----------------------------------------------------------------------------
inline DWORD
CCertObject::CertificateHashSize ()
{
    return( sizeof( m_CertificateHash ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::CertificateHash, public
//
//  Synopsis:   return the certificate hash
//
//----------------------------------------------------------------------------
inline LPBYTE
CCertObject::CertificateHash ()
{
    return( m_CertificateHash );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::KeyIdentifierSize, public
//
//  Synopsis:   return the key identifier blob size
//
//----------------------------------------------------------------------------
inline DWORD
CCertObject::KeyIdentifierSize ()
{
    return( m_cbKeyIdentifier );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::KeyIdentifier, public
//
//  Synopsis:   return the key identifier
//
//----------------------------------------------------------------------------
inline LPBYTE
CCertObject::KeyIdentifier ()
{
    return( m_pbKeyIdentifier );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::AuthorityKeyIdentifier, public
//
//  Synopsis:   return the issuer authority key identifier information
//
//----------------------------------------------------------------------------
inline PCERT_AUTHORITY_KEY_ID_INFO
CCertObject::AuthorityKeyIdentifier ()
{
    return( m_pAuthKeyIdentifier );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::IssuerList, public
//
//  Synopsis:   return the issuer list object
//
//----------------------------------------------------------------------------
inline PCCERTISSUERLIST
CCertObject::IssuerList ()
{
    return( m_pIssuerList );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::HashIndexEntry, public
//
//  Synopsis:   return the hash index entry
//
//----------------------------------------------------------------------------
inline HLRUENTRY
CCertObject::HashIndexEntry ()
{
    return( m_hHashEntry );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::IdentifierIndexEntry, public
//
//  Synopsis:   return the identifier index entry
//
//----------------------------------------------------------------------------
inline HLRUENTRY
CCertObject::IdentifierIndexEntry ()
{
    return( m_hIdentifierEntry );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::SubjectNameIndexEntry, public
//
//  Synopsis:   return the subject name index entry
//
//----------------------------------------------------------------------------
inline HLRUENTRY
CCertObject::SubjectNameIndexEntry ()
{
    return( m_hSubjectNameEntry );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::IssuerNameIndexEntry, public
//
//  Synopsis:   return the issuer name index entry
//
//----------------------------------------------------------------------------
inline HLRUENTRY
CCertObject::IssuerNameIndexEntry ()
{
    return( m_hIssuerNameEntry );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::KeyIdIndexEntry, public
//
//  Synopsis:   return the key identifier index entry
//
//----------------------------------------------------------------------------
inline HLRUENTRY
CCertObject::KeyIdIndexEntry ()
{
    return( m_hKeyIdEntry );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::ChainEngine, public
//
//  Synopsis:   return the chain engine object
//
//----------------------------------------------------------------------------
inline PCCERTCHAINENGINE
CCertObject::ChainEngine ()
{
    return( m_pChainEngine );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::IsEmpty, public
//
//  Synopsis:   is the issuer list empty
//
//----------------------------------------------------------------------------
inline BOOL
CCertIssuerList::IsEmpty ()
{
    return( m_pHead == NULL );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::IsFlushed, public
//
//  Synopsis:   is the issuer list flushed
//
//----------------------------------------------------------------------------
inline BOOL
CCertIssuerList::IsFlushed ()
{
    return( m_fFlushed );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::IsCtlIssuerFlushed, public
//
//  Synopsis:   are the CTL issuers flushed
//
//----------------------------------------------------------------------------
inline BOOL
CCertIssuerList::IsCtlIssuerFlushed ()
{
    return( m_fCtlIssuerFlushed );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::IsAdditionalStoreProcessed, public
//
//  Synopsis:   has an additional store been processed
//
//----------------------------------------------------------------------------
inline BOOL
CCertIssuerList::IsAdditionalStoreProcessed ()
{
    return( m_fAdditionalStoreProcessed );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::AddElement, public
//
//  Synopsis:   add an element to the list
//
//----------------------------------------------------------------------------
inline VOID
CCertIssuerList::AddElement (IN PCERT_ISSUER_ELEMENT pElement)
{
    pElement->pNextElement = m_pHead;
    pElement->pPrevElement = NULL;

    if ( m_pHead != NULL )
    {
        m_pHead->pPrevElement = pElement;
    }

    m_pHead = pElement;
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::RemoveElement, public
//
//  Synopsis:   remove an element from the list
//
//----------------------------------------------------------------------------
inline VOID
CCertIssuerList::RemoveElement (IN PCERT_ISSUER_ELEMENT pElement)
{
    if ( pElement->pPrevElement != NULL )
    {
        pElement->pPrevElement->pNextElement = pElement->pNextElement;
    }

    if ( pElement->pNextElement != NULL )
    {
        pElement->pNextElement->pPrevElement = pElement->pPrevElement;
    }

    if ( pElement == m_pHead )
    {
        m_pHead = pElement->pNextElement;
    }

#if DBG
    pElement->pPrevElement = NULL;
    pElement->pNextElement = NULL;
#endif
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::PrevElement, public
//
//  Synopsis:   return the previous element
//
//----------------------------------------------------------------------------
inline PCERT_ISSUER_ELEMENT
CCertIssuerList::PrevElement (IN PCERT_ISSUER_ELEMENT pElement)
{
    return( pElement->pPrevElement );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::NextElement, public
//
//  Synopsis:   return the next element, if pElement == NULL the first element
//              is returned
//
//----------------------------------------------------------------------------
inline PCERT_ISSUER_ELEMENT
CCertIssuerList::NextElement (IN PCERT_ISSUER_ELEMENT pElement)
{
    if ( pElement == NULL )
    {
        return( m_pHead );
    }

    return( pElement->pNextElement );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::HashIndex, public
//
//  Synopsis:   return the hash index
//
//----------------------------------------------------------------------------
inline HLRUCACHE
CCertObjectCache::HashIndex ()
{
    return( m_hHashIndex );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::IdentifierIndex, public
//
//  Synopsis:   return the identifier index
//
//----------------------------------------------------------------------------
inline HLRUCACHE
CCertObjectCache::IdentifierIndex ()
{
    return( m_hIdentifierIndex );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::SubjectNameIndex, public
//
//  Synopsis:   return the subject name index
//
//----------------------------------------------------------------------------
inline HLRUCACHE
CCertObjectCache::SubjectNameIndex ()
{
    return( m_hSubjectNameIndex );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::IssuerNameIndex, public
//
//  Synopsis:   return the issuer name index
//
//----------------------------------------------------------------------------
inline HLRUCACHE
CCertObjectCache::IssuerNameIndex ()
{
    return( m_hIssuerNameIndex );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::KeyIdIndex, public
//
//  Synopsis:   return the key identifier index
//
//----------------------------------------------------------------------------
inline HLRUCACHE
CCertObjectCache::KeyIdIndex ()
{
    return( m_hKeyIdIndex );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::EnableLruOfObjects, public
//
//  Synopsis:   enable LRU of entries on the primary index
//
//----------------------------------------------------------------------------
inline VOID
CCertObjectCache::EnableLruOfObjects (IN PCCHAINCALLCONTEXT pCallContext)
{
    I_CryptEnableLruOfEntries( m_hHashIndex, pCallContext );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::DisableLruOfObjects, public
//
//  Synopsis:   disable LRU of entries on the primary index
//
//----------------------------------------------------------------------------
inline VOID
CCertObjectCache::DisableLruOfObjects ()
{
    I_CryptDisableLruOfEntries( m_hHashIndex );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::FlushObjects, public
//
//  Synopsis:   flush the cache of objects
//
//----------------------------------------------------------------------------
inline VOID
CCertObjectCache::FlushObjects (IN PCCHAINCALLCONTEXT pCallContext)
{
    I_CryptFlushLruCache( m_hHashIndex, 0, pCallContext );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::LockEngine, public
//
//  Synopsis:   acquire the engine lock
//
//----------------------------------------------------------------------------
inline VOID
CCertChainEngine::LockEngine ()
{
    EnterCriticalSection( &m_Lock );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::UnlockEngine, public
//
//  Synopsis:   release the engine lock
//
//----------------------------------------------------------------------------
inline VOID
CCertChainEngine::UnlockEngine ()
{
    LeaveCriticalSection( &m_Lock );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::AddRef, public
//
//  Synopsis:   increment the reference count
//
//----------------------------------------------------------------------------
inline VOID
CCertChainEngine::AddRef ()
{
    InterlockedIncrement( (LONG *)&m_cRefs );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::Release, public
//
//  Synopsis:   decrement the reference count
//
//----------------------------------------------------------------------------
inline VOID
CCertChainEngine::Release ()
{
    if ( InterlockedDecrement( (LONG *)&m_cRefs ) == 0 )
    {
        delete this;
    }
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::CertObjectCache, public
//
//  Synopsis:   return the certificate object cache
//
//----------------------------------------------------------------------------
inline PCCERTOBJECTCACHE
CCertChainEngine::CertObjectCache ()
{
    return( m_pCertObjectCache );
}

#ifdef CAPI_INCLUDE_CTL
//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::SSCtlObjectCache, public
//
//  Synopsis:   return the self signed certificate trust list object cache
//
//----------------------------------------------------------------------------
inline PCSSCTLOBJECTCACHE
CCertChainEngine::SSCtlObjectCache ()
{
    return( m_pSSCtlObjectCache );
}
#endif

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::RootStore, public
//
//  Synopsis:   return the configured root store
//
//----------------------------------------------------------------------------
inline HCERTSTORE
CCertChainEngine::RootStore ()
{
    return( m_hRootStore );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::RealRootStore, public
//
//  Synopsis:   return the real root store
//
//----------------------------------------------------------------------------
inline HCERTSTORE
CCertChainEngine::RealRootStore ()
{
    return( m_hRealRootStore );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::TrustStore, public
//
//  Synopsis:   return the configured trust store
//
//----------------------------------------------------------------------------
inline HCERTSTORE
CCertChainEngine::TrustStore ()
{
    return( m_hTrustStore );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::OtherStore, public
//
//  Synopsis:   return the configured other store
//
//----------------------------------------------------------------------------
inline HCERTSTORE
CCertChainEngine::OtherStore ()
{
    return( m_hOtherStore );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::CycleDetectionModulus, public
//
//  Synopsis:   return the cycle detection modulus
//
//----------------------------------------------------------------------------
inline DWORD
CCertChainEngine::CycleDetectionModulus ()
{
    return( m_CycleDetectionModulus );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainContext::ElementCount, public
//
//  Synopsis:   return the element count for a given simple chain
//
//----------------------------------------------------------------------------
inline DWORD
CCertChainContext::ElementCount (IN DWORD SimpleChain)
{
    return( m_pContext->ChainContext.rgpChain[ SimpleChain ]->cElement );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainContext::InternalContext, public
//
//  Synopsis:   return the internal chain context
//
//----------------------------------------------------------------------------
inline PINTERNAL_CERT_CHAIN_CONTEXT
CCertChainContext::InternalContext ()
{
    return( m_pContext );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainContext::ExposedContext, public
//
//  Synopsis:   return the exposed chain context
//
//----------------------------------------------------------------------------
inline PCERT_CHAIN_CONTEXT
CCertChainContext::ExposedContext ()
{
    return( &m_pContext->ChainContext );
}

#endif
