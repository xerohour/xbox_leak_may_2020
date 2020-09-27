/+---------------------------------------------------------------------------
//
//  Microsoft Windows NT Security
//  Copyright (C) Microsoft Corporation, 1997 - 1998
//
//  File:       ssctl.cpp
//
//  Contents:   Self Signed Certificate Trust List Subsystem used by the
//              Certificate Chaining Infrastructure for building complex
//              chains
//
//  History:    11-Feb-98    kirtd    Created
//
//----------------------------------------------------------------------------
#include <global.hxx>
//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObject::CSSCtlObject, public
//
//  Synopsis:   Constructor
//
//----------------------------------------------------------------------------
CSSCtlObject::CSSCtlObject (
                    IN PCCERTCHAINENGINE pChainEngine,
                    IN PCCTL_CONTEXT pCtlContext,
                    OUT BOOL& rfResult
                    )
{
    DWORD           cbData = MD5DIGESTLEN;
    CRYPT_DATA_BLOB DataBlob;

    rfResult = TRUE;

    m_cRefs = 1;
    m_pCtlContext = CertDuplicateCTLContext( pCtlContext );
    m_fHasSignatureBeenVerified = FALSE;
    m_fSignatureValid = FALSE;
    m_hMessageStore = NULL;
    m_hHashEntry = NULL;
    m_pChainEngine = pChainEngine;

    memset( &m_SignerInfo, 0, sizeof( m_SignerInfo ) );

    rfResult = CertGetCTLContextProperty(
                   pCtlContext,
                   CERT_MD5_HASH_PROP_ID,
                   m_CtlHash,
                   &cbData
                   );

    if ( rfResult == TRUE )
    {
        DataBlob.cbData = CtlHashSize();
        DataBlob.pbData = CtlHash();

        rfResult = I_CryptCreateLruEntry(
                          pChainEngine->SSCtlObjectCache()->HashIndex(),
                          &DataBlob,
                          this,
                          &m_hHashEntry
                          );
    }

    if ( rfResult == TRUE )
    {
        m_hMessageStore = CertOpenStore(
                              CERT_STORE_PROV_MSG,
                              X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                              NULL,
                              0,
                              pCtlContext->hCryptMsg
                              );

        if ( m_hMessageStore != NULL )
        {
            rfResult = CertAddStoreToCollection(
                           pChainEngine->OtherStore(),
                           m_hMessageStore,
                           0,
                           0
                           );
        }
        else
        {
            rfResult = FALSE;
        }
    }

    if ( rfResult == TRUE )
    {
        rfResult = SSCtlGetSignerInfo( pCtlContext, &m_SignerInfo );
    }

    assert( m_pChainEngine != NULL );
    assert( m_pCtlContext != NULL );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObject::~CSSCtlObject, public
//
//  Synopsis:   Destructor
//
//----------------------------------------------------------------------------
CSSCtlObject::~CSSCtlObject ()
{
    SSCtlFreeSignerInfo( &m_SignerInfo );

    if ( m_hMessageStore != NULL )
    {
        CertRemoveStoreFromCollection(
            m_pChainEngine->OtherStore(),
            m_hMessageStore
            );

        CertCloseStore( m_hMessageStore, 0 );
    }

    if ( m_hHashEntry != NULL )
    {
        I_CryptReleaseLruEntry( m_hHashEntry );
    }

    CertFreeCTLContext( m_pCtlContext );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObject::GetSigner, public
//
//  Synopsis:   get the certificate object of the signer
//
//----------------------------------------------------------------------------
BOOL
CSSCtlObject::GetSigner (
                 IN PCCERTOBJECT pSubject,
                 IN PCCHAINCALLCONTEXT pCallContext,
                 IN HCERTSTORE hAdditionalStore,
                 OUT PCCERTOBJECT* ppSigner,
                 OUT BOOL* pfAdditionalStoreSigner,
                 OUT BOOL* pfCtlSignatureValid
                 )
{
    BOOL         fResult;
    PCCERTOBJECT pSigner = NULL;

    assert( m_pChainEngine == pSubject->ChainEngine() );

    fResult = SSCtlGetSignerCertificateObject(
                   m_pChainEngine,
                   pSubject,
                   pCallContext,
                   &m_SignerInfo,
                   hAdditionalStore,
                   &pSigner,
                   pfAdditionalStoreSigner
                   );

    if ( fResult == TRUE )
    {
        if ( m_fHasSignatureBeenVerified == FALSE )
        {
#ifdef CMS_PKCS7
            CMSG_CTRL_VERIFY_SIGNATURE_EX_PARA CtrlPara;

            memset(&CtrlPara, 0, sizeof(CtrlPara));
            CtrlPara.cbSize = sizeof(CtrlPara);
            // CtrlPara.hCryptProv =

            // BUGBUG this needs to be updated when chain building
            // supports CTLs with more than one signer.
            CtrlPara.dwSignerIndex = 0;
            CtrlPara.dwSignerType = CMSG_VERIFY_SIGNER_CERT;
            CtrlPara.pvSigner = (void *) pSigner->CertContext();

            m_fSignatureValid = CryptMsgControl(
                                     m_pCtlContext->hCryptMsg,
                                     0,
                                     CMSG_CTRL_VERIFY_SIGNATURE_EX,
                                     &CtrlPara
                                     );
#else
            m_fSignatureValid = CryptMsgControl(
                                     m_pCtlContext->hCryptMsg,
                                     0,
                                     CMSG_CTRL_VERIFY_SIGNATURE,
                                     pSigner->CertContext()->pCertInfo
                                     );
#endif  // CMS_PKCS7

            m_fHasSignatureBeenVerified = TRUE;
        }

        *ppSigner = pSigner;
        *pfCtlSignatureValid = m_fSignatureValid;
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObject::GetTrustListInfo, public
//
//  Synopsis:   get the trust list information relative to a particular cert
//              object
//
//----------------------------------------------------------------------------
BOOL
CSSCtlObject::GetTrustListInfo (
                 IN PCCERTOBJECT pCertObject,
                 OUT PCERT_TRUST_LIST_INFO* ppTrustListInfo
                 )
{
    PCTL_ENTRY            pCtlEntry;
    PCERT_TRUST_LIST_INFO pTrustListInfo;

    pCtlEntry = CertFindSubjectInCTL(
                    X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                    CTL_CERT_SUBJECT_TYPE,
                    (LPVOID)pCertObject->CertContext(),
                    m_pCtlContext,
                    0
                    );

    if ( pCtlEntry == NULL )
    {
        SetLastError( CRYPT_E_NOT_FOUND );
        return( FALSE );
    }

    pTrustListInfo = new CERT_TRUST_LIST_INFO;
    if ( pTrustListInfo == NULL )
    {
        SetLastError( E_OUTOFMEMORY );
        return( FALSE );
    }

    pTrustListInfo->cbSize = sizeof( CERT_TRUST_LIST_INFO );
    pTrustListInfo->pCtlEntry = pCtlEntry;
    pTrustListInfo->pCtlContext = CertDuplicateCTLContext( m_pCtlContext );

    *ppTrustListInfo = pTrustListInfo;

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObject::CalculateStatus, public
//
//  Synopsis:   calculate the status
//
//----------------------------------------------------------------------------
VOID
CSSCtlObject::CalculateStatus (
                       IN LPFILETIME pTime,
                       IN PCERT_USAGE_MATCH pRequestedUsage,
                       IN DWORD dwFlags,
                       OUT PCERT_TRUST_STATUS pStatus
                       )
{
    assert( m_fHasSignatureBeenVerified == TRUE );

    SSCtlGetCtlTrustStatus(
         m_pCtlContext,
         m_fSignatureValid,
         pTime,
         pRequestedUsage,
         dwFlags,
         pStatus
         );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObject::GetSignerRequestedUsage, public
//
//  Synopsis:   get the signer requested usage and time
//
//----------------------------------------------------------------------------
BOOL
CSSCtlObject::GetSignerRequestedUsageAndTime (
                 IN LPFILETIME pCurrentTime,
                 OUT PCERT_USAGE_MATCH pSignerRequestedUsage,
                 OUT LPFILETIME pSignerRequestedTime
                 )
{
    LPSTR            pszUsage = szOID_KP_CTL_USAGE_SIGNING;
    CERT_USAGE_MATCH SignerRequestedUsage;

    *pSignerRequestedTime = *pCurrentTime;

    memset( &SignerRequestedUsage, 0, sizeof( SignerRequestedUsage ) );

    SignerRequestedUsage.Usage.cUsageIdentifier = 1;
    SignerRequestedUsage.Usage.rgpszUsageIdentifier = &pszUsage;

    return( ChainCopyRequestedUsage(
                 &SignerRequestedUsage,
                 pSignerRequestedUsage
                 ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObjectCache::CSSCtlObjectCache, public
//
//  Synopsis:   Constructor
//
//----------------------------------------------------------------------------
CSSCtlObjectCache::CSSCtlObjectCache (
                         OUT BOOL& rfResult
                         )
{
    LRU_CACHE_CONFIG Config;

    memset( &Config, 0, sizeof( Config ) );

    Config.dwFlags = LRU_CACHE_NO_SERIALIZE | LRU_CACHE_NO_COPY_IDENTIFIER;
    Config.pfnHash = CertObjectCacheHashMd5Identifier;
    Config.cBuckets = DEFAULT_CERT_OBJECT_CACHE_BUCKETS;
    Config.pfnOnRemoval = SSCtlOnRemovalFromCache;

    m_hHashIndex = NULL;

    rfResult = I_CryptCreateLruCache( &Config, &m_hHashIndex );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObjectCache::~CSSCtlObjectCache, public
//
//  Synopsis:   Destructor
//
//----------------------------------------------------------------------------
CSSCtlObjectCache::~CSSCtlObjectCache ()
{
    I_CryptFreeLruCache( m_hHashIndex, 0, NULL );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObjectCache::PopulateCache, public
//
//  Synopsis:   populate the cache
//
//----------------------------------------------------------------------------
BOOL
CSSCtlObjectCache::PopulateCache (
                           IN PCCERTCHAINENGINE pChainEngine
                           )
{
    assert( pChainEngine->SSCtlObjectCache() == this );

    return( SSCtlPopulateCacheFromCertStore( pChainEngine, NULL ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObjectCache::AddObject, public
//
//  Synopsis:   add an object to the cache
//
//----------------------------------------------------------------------------
BOOL
CSSCtlObjectCache::AddObject (
                      IN PCSSCTLOBJECT pSSCtlObject,
                      IN BOOL fCheckForDuplicate
                      )
{
    if ( fCheckForDuplicate == TRUE )
    {
        CRYPT_HASH_BLOB HashBlob;
        PCSSCTLOBJECT   pDuplicate;

        HashBlob.cbData = pSSCtlObject->CtlHashSize();
        HashBlob.pbData = pSSCtlObject->CtlHash();

        pDuplicate = FindObjectByHash( &HashBlob );
        if ( pDuplicate != NULL )
        {
            pDuplicate->Release();
            SetLastError( CRYPT_E_EXISTS );
            return( FALSE );
        }
    }

    pSSCtlObject->AddRef();

    I_CryptInsertLruEntry( pSSCtlObject->HashIndexEntry(), NULL );

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObjectCache::RemoveObject, public
//
//  Synopsis:   remove object from cache
//
//----------------------------------------------------------------------------
VOID
CSSCtlObjectCache::RemoveObject (
                         IN PCSSCTLOBJECT pSSCtlObject
                         )
{
    I_CryptRemoveLruEntry( pSSCtlObject->HashIndexEntry(), 0, NULL );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObjectCache::FindObjectByHash, public
//
//  Synopsis:   find object with given hash
//
//----------------------------------------------------------------------------
PCSSCTLOBJECT
CSSCtlObjectCache::FindObjectByHash (
                       IN PCRYPT_HASH_BLOB pHashBlob
                       )
{
    HLRUENTRY     hFound;
    PCSSCTLOBJECT pFound = NULL;

    hFound = I_CryptFindLruEntry( m_hHashIndex, (PCRYPT_DATA_BLOB)pHashBlob );
    if ( hFound != NULL )
    {
        pFound = (PCSSCTLOBJECT)I_CryptGetLruEntryData( hFound );
        pFound->AddRef();

        I_CryptReleaseLruEntry( hFound );
    }

    return( pFound );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObjectCache::EnumObjects, public
//
//  Synopsis:   enumerate objects
//
//----------------------------------------------------------------------------
VOID
CSSCtlObjectCache::EnumObjects (
                       IN PFN_ENUM_SSCTLOBJECTS pfnEnum,
                       IN LPVOID pvParameter
                       )
{
    SSCTL_ENUM_OBJECTS_DATA EnumData;

    EnumData.pfnEnumObjects = pfnEnum;
    EnumData.pvEnumParameter = pvParameter;

    I_CryptWalkAllLruCacheEntries(
           m_hHashIndex,
           SSCtlEnumObjectsWalkFn,
           &EnumData
           );
}

//+---------------------------------------------------------------------------
//
//  Member:     CSSCtlObjectCache::Resync, public
//
//  Synopsis:   resync the cache
//
//----------------------------------------------------------------------------
BOOL
CSSCtlObjectCache::Resync (IN PCCERTCHAINENGINE pChainEngine)
{
    I_CryptFlushLruCache( m_hHashIndex, 0, NULL );

    return( PopulateCache( pChainEngine ) );
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlOnRemovalFromCache
//
//  Synopsis:   SS CTL removal notification used when the cache is destroyed
//              or an object is explicitly removed.  Note that this cache
//              does not LRU remove objects
//
//----------------------------------------------------------------------------
VOID WINAPI
SSCtlOnRemovalFromCache (
     IN LPVOID pv,
     IN OPTIONAL LPVOID pvRemovalContext
     )
{
    assert( pvRemovalContext == NULL );

    ( (PCSSCTLOBJECT)pv )->Release();
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlGetSignerInfo
//
//  Synopsis:   get the signer info
//
//----------------------------------------------------------------------------
BOOL WINAPI
SSCtlGetSignerInfo (
     IN PCCTL_CONTEXT pCtlContext,
     OUT PSSCTL_SIGNER_INFO pSignerInfo
     )
{
    BOOL              fResult;
    PCMSG_SIGNER_INFO pMessageSignerInfo = NULL;
    DWORD             cbData = 0;

    fResult = CryptMsgGetParam(
                   pCtlContext->hCryptMsg,
                   CMSG_SIGNER_INFO_PARAM,
                   0,
                   NULL,
                   &cbData
                   );

    if ( fResult == TRUE )
    {
        pMessageSignerInfo = (PCMSG_SIGNER_INFO)new BYTE [ cbData ];
        if ( pMessageSignerInfo != NULL )
        {
            fResult = CryptMsgGetParam(
                           pCtlContext->hCryptMsg,
                           CMSG_SIGNER_INFO_PARAM,
                           0,
                           pMessageSignerInfo,
                           &cbData
                           );
        }
        else
        {
            SetLastError( E_OUTOFMEMORY );
            fResult = FALSE;
        }
    }

    if ( fResult == TRUE )
    {
        pSignerInfo->pMessageSignerInfo = pMessageSignerInfo;
        pSignerInfo->fSignerHashAvailable = FALSE;
        pSignerInfo->SignerCertificateHashSize = MD5DIGESTLEN;

        ChainCreateCertificateObjectIdentifier(
             &pMessageSignerInfo->Issuer,
             &pMessageSignerInfo->SerialNumber,
             pSignerInfo->SignerCertObjectIdentifier
             );
    }
    else
    {
        delete pMessageSignerInfo;
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlFreeSignerInfo
//
//  Synopsis:   free the data in the signer info
//
//----------------------------------------------------------------------------
VOID WINAPI
SSCtlFreeSignerInfo (
     IN PSSCTL_SIGNER_INFO pSignerInfo
     )
{
    delete (LPBYTE)pSignerInfo->pMessageSignerInfo;
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlGetSignerCertificateObject
//
//  Synopsis:   get the signer certificate object
//
//----------------------------------------------------------------------------
BOOL WINAPI
SSCtlGetSignerCertificateObject (
     IN PCCERTCHAINENGINE pChainEngine,
     IN PCCERTOBJECT pSubject,
     IN PCCHAINCALLCONTEXT pCallContext,
     IN PSSCTL_SIGNER_INFO pSignerInfo,
     IN HCERTSTORE hAdditionalStore,
     OUT PCCERTOBJECT* ppCertObject,
     OUT BOOL* pfAdditionalStoreSigner
     )
{
    BOOL              fResult = TRUE;
    PCCERTOBJECT      pCertObject;
    PCCERTOBJECTCACHE pCertObjectCache;
    PCCERT_CONTEXT    pCertContext = NULL;
    CRYPT_HASH_BLOB   HashBlob;
    BOOL              fAdditionalStoreUsed = FALSE;
    DWORD             cbData = pSignerInfo->SignerCertificateHashSize;
    BYTE              CertificateHash[ MD5DIGESTLEN ];
    BOOL              fFromCallCreationCache = FALSE;

    assert( pSignerInfo->SignerCertificateHashSize == MD5DIGESTLEN );

    pCertObjectCache = pChainEngine->CertObjectCache();

    if ( pSignerInfo->fSignerHashAvailable == TRUE )
    {
        HashBlob.cbData = MD5DIGESTLEN;
        HashBlob.pbData = pSignerInfo->SignerCertificateHash;

        pCertObject = pCertObjectCache->FindObjectByHash( &HashBlob );
    }
    else
    {
        pCertObject = pCertObjectCache->FindObjectByIdentifier(
                                            pSignerInfo->SignerCertObjectIdentifier
                                            );
    }

    if ( pCertObject == NULL )
    {
        if ( pSignerInfo->fSignerHashAvailable == TRUE )
        {
            pCertContext = SSCtlFindCertificateInStoreByHash(
                                pChainEngine->OtherStore(),
                                &HashBlob
                                );

            if ( ( pCertContext == NULL ) && ( hAdditionalStore != NULL ) )
            {
                fAdditionalStoreUsed = TRUE;

                pCertContext = SSCtlFindCertificateInStoreByHash(
                                    hAdditionalStore,
                                    &HashBlob
                                    );
            }
        }

        if ( pCertContext == NULL )
        {
            fAdditionalStoreUsed = FALSE;

            pCertContext = SSCtlFindCertificateInStoreByIssuerAndSerialNumber(
                                pChainEngine->OtherStore(),
                                NULL,
                                &pSignerInfo->pMessageSignerInfo->Issuer,
                                &pSignerInfo->pMessageSignerInfo->SerialNumber
                                );
        }

        if ( ( pCertContext == NULL ) && ( hAdditionalStore != NULL ) )
        {
            fAdditionalStoreUsed = TRUE;

            pCertContext = SSCtlFindCertificateInStoreByIssuerAndSerialNumber(
                                hAdditionalStore,
                                NULL,
                                &pSignerInfo->pMessageSignerInfo->Issuer,
                                &pSignerInfo->pMessageSignerInfo->SerialNumber
                                );
        }

        if ( pCertContext != NULL )
        {
            fResult = CertGetCertificateContextProperty(
                          pCertContext,
                          CERT_MD5_HASH_PROP_ID,
                          CertificateHash,
                          &cbData
                          );

            if ( fResult == TRUE )
            {
                HashBlob.cbData = cbData;
                HashBlob.pbData = CertificateHash;

                fResult = ChainCreateCertificateObject(
                               pChainEngine,
                               pCallContext,
                               pCertContext,
                               &HashBlob,
                               NULL,
                               hAdditionalStore,
                               &pCertObject,
                               &fFromCallCreationCache
                               );
            }

            CertFreeCertificateContext( pCertContext );
        }
        else
        {
            fResult = FALSE;
        }

        if ( ( fResult == TRUE ) && ( fAdditionalStoreUsed == FALSE ) &&
             ( pCertObject != NULL ) )
        {
            fResult = ChainCacheEndCertificate( pCallContext, pCertObject );
        }
    }

    if ( fResult == TRUE )
    {
        if ( pSignerInfo->fSignerHashAvailable == FALSE )
        {
            memcpy(
               pSignerInfo->SignerCertificateHash,
               CertificateHash,
               pSignerInfo->SignerCertificateHashSize
               );

            pSignerInfo->fSignerHashAvailable = TRUE;
        }

        *ppCertObject = pCertObject;
        *pfAdditionalStoreSigner = fAdditionalStoreUsed;
    }
    else if ( pCertObject != NULL )
    {
        pCertObject->Release();
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlFindCertificateInStoreByIssuerAndSerialNumber
//
//  Synopsis:   find certificate in store given the issuer and serial number
//
//----------------------------------------------------------------------------
PCCERT_CONTEXT WINAPI
SSCtlFindCertificateInStoreByIssuerAndSerialNumber (
     IN HCERTSTORE hStore,
     IN PCCERT_CONTEXT pCertContext,
     IN PCERT_NAME_BLOB pIssuer,
     IN PCRYPT_INTEGER_BLOB pSerialNumber
     )
{
    while ( ( pCertContext = CertFindCertificateInStore(
                                 hStore,
                                 X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                                 0,
                                 CERT_FIND_ISSUER_NAME,
                                 pIssuer,
                                 pCertContext
                                 ) ) != NULL )
    {
        if ( ( pCertContext->pCertInfo->SerialNumber.cbData ==
               pSerialNumber->cbData ) &&
             ( memcmp(
                  pCertContext->pCertInfo->SerialNumber.pbData,
                  pSerialNumber->pbData,
                  pCertContext->pCertInfo->SerialNumber.cbData
                  ) == 0 ) )
        {
            break;
        }
    }

    return( pCertContext );
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlFindCertificateInStoreByHash
//
//  Synopsis:   find certificate in store by hash
//
//----------------------------------------------------------------------------
PCCERT_CONTEXT WINAPI
SSCtlFindCertificateInStoreByHash (
     IN HCERTSTORE hStore,
     IN PCRYPT_HASH_BLOB pHashBlob
     )
{
    return( CertFindCertificateInStore(
                hStore,
                X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                0,
                CERT_FIND_MD5_HASH,
                pHashBlob,
                NULL
                ) );
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlGetCtlTrustStatus
//
//  Synopsis:   get the trust status for the CTL
//
//----------------------------------------------------------------------------
VOID WINAPI
SSCtlGetCtlTrustStatus (
     IN PCCTL_CONTEXT pCtlContext,
     IN BOOL fSignatureValid,
     IN LPFILETIME pTime,
     IN PCERT_USAGE_MATCH pRequestedUsage,
     IN DWORD dwFlags,
     OUT PCERT_TRUST_STATUS pStatus
     )
{
    FILETIME          NoTime;
    CERT_TRUST_STATUS UsageStatus;

    memset( pStatus, 0, sizeof( CERT_TRUST_STATUS ) );
    memset( &NoTime, 0, sizeof( NoTime ) );

    if ( fSignatureValid == FALSE )
    {
        pStatus->dwErrorStatus |= CERT_TRUST_CTL_IS_NOT_SIGNATURE_VALID;
    }

    if ( ( CompareFileTime(
                  pTime,
                  &pCtlContext->pCtlInfo->ThisUpdate
                  ) < 0 ) ||
         ( ( ( CompareFileTime(
                      &NoTime,
                      &pCtlContext->pCtlInfo->NextUpdate
                      ) != 0 ) &&
             ( CompareFileTime(
                      pTime,
                      &pCtlContext->pCtlInfo->NextUpdate
                      ) > 0 ) ) ) )
    {
        pStatus->dwErrorStatus |= CERT_TRUST_CTL_IS_NOT_TIME_VALID;
    }

    ChainGetUsageStatus(
         (PCERT_ENHKEY_USAGE)&pRequestedUsage->Usage,
         (PCERT_ENHKEY_USAGE)&pCtlContext->pCtlInfo->SubjectUsage,
         pRequestedUsage->dwType,
         &UsageStatus
         );

    if ( UsageStatus.dwErrorStatus & CERT_TRUST_IS_NOT_VALID_FOR_USAGE )
    {
        pStatus->dwErrorStatus |= CERT_TRUST_CTL_IS_NOT_VALID_FOR_USAGE;
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlPopulateCacheFromCertStore
//
//  Synopsis:   populate the SS CTL object cache from certificate store CTLs
//
//----------------------------------------------------------------------------
BOOL WINAPI
SSCtlPopulateCacheFromCertStore (
     IN PCCERTCHAINENGINE pChainEngine,
     IN OPTIONAL HCERTSTORE hStore
     )
{
    BOOL               fResult;
    BOOL               fAdditionalStore = TRUE;
    PCCTL_CONTEXT      pCtlContext = NULL;
    DWORD              cbData = MD5DIGESTLEN;
    BYTE               CtlHash[ MD5DIGESTLEN ];
    CRYPT_HASH_BLOB    HashBlob;
    PCSSCTLOBJECT      pSSCtlObject;
    PCSSCTLOBJECTCACHE pSSCtlObjectCache;

    pSSCtlObjectCache = pChainEngine->SSCtlObjectCache();

    if ( hStore == NULL )
    {
        hStore = pChainEngine->TrustStore();
        fAdditionalStore = FALSE;
    }

    while ( ( pCtlContext = CertEnumCTLsInStore(
                                hStore,
                                pCtlContext
                                ) ) != NULL )
    {
        fResult = CertGetCTLContextProperty(
                      pCtlContext,
                      CERT_MD5_HASH_PROP_ID,
                      CtlHash,
                      &cbData
                      );

        if ( fResult == TRUE )
        {
            HashBlob.cbData = cbData;
            HashBlob.pbData = CtlHash;

            pSSCtlObject = pSSCtlObjectCache->FindObjectByHash( &HashBlob );
            if ( pSSCtlObject == NULL )
            {
                fResult = SSCtlCreateCtlObject(
                               pChainEngine,
                               pCtlContext,
                               &pSSCtlObject
                               );
            }
            else
            {
                pSSCtlObject->Release();
                fResult = FALSE;
            }

            if ( fResult == TRUE )
            {
                fResult = pSSCtlObjectCache->AddObject( pSSCtlObject, FALSE );

                // NOTE: Since fDuplicate == FALSE this should never fail
                assert( fResult == TRUE );

                pSSCtlObject->Release();
            }
        }
    }

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlCreateCtlObject
//
//  Synopsis:   create an SS CTL Object
//
//----------------------------------------------------------------------------
BOOL WINAPI
SSCtlCreateCtlObject (
     IN PCCERTCHAINENGINE pChainEngine,
     IN PCCTL_CONTEXT pCtlContext,
     OUT PCSSCTLOBJECT* ppSSCtlObject
     )
{
    BOOL          fResult = TRUE;
    PCSSCTLOBJECT pSSCtlObject;

    pSSCtlObject = new CSSCtlObject( pChainEngine, pCtlContext, fResult );
    if ( pSSCtlObject == NULL )
    {
        SetLastError( E_OUTOFMEMORY );
        fResult = FALSE;
    }
    else if ( fResult == TRUE )
    {
        *ppSSCtlObject = pSSCtlObject;
    }
    else
    {
        delete pSSCtlObject;
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlEnumObjectsWalkFn
//
//  Synopsis:   object enumerator walk function
//
//----------------------------------------------------------------------------
BOOL WINAPI
SSCtlEnumObjectsWalkFn (
     IN LPVOID pvParameter,
     IN HLRUENTRY hEntry
     )
{
    PSSCTL_ENUM_OBJECTS_DATA pEnumData = (PSSCTL_ENUM_OBJECTS_DATA)pvParameter;

    return( ( *pEnumData->pfnEnumObjects )(
                             pEnumData->pvEnumParameter,
                             (PCSSCTLOBJECT)I_CryptGetLruEntryData( hEntry )
                             ) );
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlCreateObjectCache
//
//  Synopsis:   create the SS CTL object cache
//
//----------------------------------------------------------------------------
BOOL WINAPI
SSCtlCreateObjectCache (
     OUT PCSSCTLOBJECTCACHE* ppSSCtlObjectCache
     )
{
    BOOL               fResult = TRUE;
    PCSSCTLOBJECTCACHE pSSCtlObjectCache;

    pSSCtlObjectCache = new CSSCtlObjectCache( fResult );

    if ( pSSCtlObjectCache == NULL )
    {
        SetLastError( E_OUTOFMEMORY );
        fResult = FALSE;
    }
    else if ( fResult == TRUE )
    {
        *ppSSCtlObjectCache = pSSCtlObjectCache;
    }
    else
    {
        delete pSSCtlObjectCache;
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlFreeObjectCache
//
//  Synopsis:   free the object cache
//
//----------------------------------------------------------------------------
VOID WINAPI
SSCtlFreeObjectCache (
     IN PCSSCTLOBJECTCACHE pSSCtlObjectCache
     )
{
    delete pSSCtlObjectCache;
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlFreeTrustListInfo
//
//  Synopsis:   free the trust list info
//
//----------------------------------------------------------------------------
VOID WINAPI
SSCtlFreeTrustListInfo (
     IN PCERT_TRUST_LIST_INFO pTrustListInfo
     )
{
    CertFreeCTLContext( pTrustListInfo->pCtlContext );

    delete pTrustListInfo;
}

//+---------------------------------------------------------------------------
//
//  Function:   SSCtlAllocAndCopyTrustListInfo
//
//  Synopsis:   allocate and copy the trust list info
//
//----------------------------------------------------------------------------
BOOL WINAPI
SSCtlAllocAndCopyTrustListInfo (
     IN PCERT_TRUST_LIST_INFO pTrustListInfo,
     OUT PCERT_TRUST_LIST_INFO* ppTrustListInfo
     )
{
    PCERT_TRUST_LIST_INFO pCopyTrustListInfo;

    pCopyTrustListInfo = new CERT_TRUST_LIST_INFO;
    if ( pCopyTrustListInfo == NULL )
    {
        SetLastError( E_OUTOFMEMORY );
        return( FALSE );
    }

    pCopyTrustListInfo->cbSize = sizeof( CERT_TRUST_LIST_INFO );

    pCopyTrustListInfo->pCtlContext = CertDuplicateCTLContext(
                                          pTrustListInfo->pCtlContext
                                          );

    pCopyTrustListInfo->pCtlEntry = pTrustListInfo->pCtlEntry;

    *ppTrustListInfo = pCopyTrustListInfo;

    return( TRUE );
}

