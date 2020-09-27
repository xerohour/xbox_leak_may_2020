//+---------------------------------------------------------------------------
//
//  Microsoft Windows NT Security
//  Copyright (C) Microsoft Corporation, 1997 - 1998
//
//  File:       chain.cpp
//
//  Contents:   Certificate Chaining Infrastructure
//
//  History:    15-Jan-98    kirtd    Created
//
//----------------------------------------------------------------------------
#include <global.hxx>
#include <xdbg.h>

#ifdef UNDER_CE
#define CEUNICODE(x)	TEXT(x)
#else
#define CEUNICODE(x)	(x)
#endif
//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::CCertObject, public
//
//  Synopsis:   Constructor
//
//----------------------------------------------------------------------------
CCertObject::CCertObject (
                  IN PCCERTCHAINENGINE pChainEngine,
                  IN PCCHAINCALLCONTEXT pCallContext,
                  IN PCCERT_CONTEXT pCertContext,
                  IN PCRYPT_HASH_BLOB pHashBlob,
                  IN PCERT_TRUST_STATUS pKnownStatus,
                  IN HCERTSTORE hAdditionalStore,
                  OUT BOOL& rfResult
                  )
{
    CRYPT_DATA_BLOB   DataBlob;
    CERT_TRUST_STATUS TrustStatus;

    rfResult = TRUE;

    m_cRefs = 1;
    m_pCertContext = CertDuplicateCertificateContext( pCertContext );
    m_cbKeyIdentifier = 0;
    m_pbKeyIdentifier = NULL;
    m_pUsage = NULL;
    m_pAuthKeyIdentifier = NULL;
    m_pIssuerList = NULL;

    // NOTE: The chain engine is NOT addref'd
    m_pChainEngine = pChainEngine;

    m_hHashEntry = NULL;
    m_hIdentifierEntry = NULL;
    m_hSubjectNameEntry = NULL;
    m_hIssuerNameEntry = NULL;

    memset( &m_TrustStatus, 0, sizeof( m_TrustStatus ) );

    if ( pKnownStatus != NULL )
    {
        ChainOrInStatusBits( &m_TrustStatus, pKnownStatus );
    }

    ChainCreateCertificateObjectIdentifier(
         &pCertContext->pCertInfo->Issuer,
         &pCertContext->pCertInfo->SerialNumber,
         m_ObjectIdentifier
         );

    memcpy( m_CertificateHash, pHashBlob->pbData, pHashBlob->cbData );

    rfResult = CertGetCertificateContextProperty(
                   pCertContext,
                   CERT_KEY_IDENTIFIER_PROP_ID,
                   NULL,
                   &m_cbKeyIdentifier
                   );

    if ( rfResult == TRUE )
    {
        m_pbKeyIdentifier = new BYTE [ m_cbKeyIdentifier ];
        if ( m_pbKeyIdentifier != NULL )
        {
            rfResult = CertGetCertificateContextProperty(
                           pCertContext,
                           CERT_KEY_IDENTIFIER_PROP_ID,
                           m_pbKeyIdentifier,
                           &m_cbKeyIdentifier
                           );
        }
        else
        {
            SetLastError( E_OUTOFMEMORY );
            rfResult = FALSE;
        }
    }

    if ( rfResult == TRUE )
    {
        rfResult = ChainGetCertificateUsage( pCertContext, &m_pUsage );
    }

    if ( rfResult == TRUE )
    {
        rfResult = ChainCreateIssuerList( this, &m_pIssuerList );
    }

    if ( rfResult == TRUE )
    {
        DataBlob.cbData = CertificateHashSize();
        DataBlob.pbData = CertificateHash();

        rfResult = I_CryptCreateLruEntry(
                          pChainEngine->CertObjectCache()->HashIndex(),
                          &DataBlob,
                          this,
                          &m_hHashEntry
                          );
    }

    if ( rfResult == TRUE )
    {
        DataBlob.cbData = sizeof( CERT_OBJECT_IDENTIFIER );
        DataBlob.pbData = m_ObjectIdentifier;

        rfResult = I_CryptCreateLruEntry(
                          pChainEngine->CertObjectCache()->IdentifierIndex(),
                          &DataBlob,
                          this,
                          &m_hIdentifierEntry
                          );
    }

    if ( rfResult == TRUE )
    {
        DataBlob.cbData = pCertContext->pCertInfo->Subject.cbData;
        DataBlob.pbData = pCertContext->pCertInfo->Subject.pbData;

        rfResult = I_CryptCreateLruEntry(
                          pChainEngine->CertObjectCache()->SubjectNameIndex(),
                          &DataBlob,
                          this,
                          &m_hSubjectNameEntry
                          );
    }

    if ( rfResult == TRUE )
    {
        DataBlob.cbData = pCertContext->pCertInfo->Issuer.cbData;
        DataBlob.pbData = pCertContext->pCertInfo->Issuer.pbData;

        rfResult = I_CryptCreateLruEntry(
                          pChainEngine->CertObjectCache()->IssuerNameIndex(),
                          &DataBlob,
                          this,
                          &m_hIssuerNameEntry
                          );
    }

    if ( rfResult == TRUE )
    {
        DataBlob.cbData = m_cbKeyIdentifier;
        DataBlob.pbData = m_pbKeyIdentifier;

        rfResult = I_CryptCreateLruEntry(
                          pChainEngine->CertObjectCache()->KeyIdIndex(),
                          &DataBlob,
                          this,
                          &m_hKeyIdEntry
                          );
    }

    if ( rfResult == FALSE )
    {
        return;
    }

    ChainGetIssuerMatchStatus(
         pCertContext,
         &m_pAuthKeyIdentifier,
         &TrustStatus
         );

    ChainOrInStatusBits( &m_TrustStatus, &TrustStatus );

    ChainGetSelfSignedStatus( this, &TrustStatus );
    ChainOrInStatusBits( &m_TrustStatus, &TrustStatus );

    if ( InfoStatus() & CERT_TRUST_IS_SELF_SIGNED )
    {
        //
        // NOTE: This means that only self-signed roots are supported
        //

        ChainGetRootStoreStatus(
             pChainEngine->RootStore(),
             pChainEngine->RealRootStore(),
             pHashBlob,
             &TrustStatus
             );

        ChainOrInStatusBits( &m_TrustStatus, &TrustStatus );
    }

    rfResult = pCallContext->AddObjectToCreationCache( this );

    if ( rfResult == TRUE )
    {
        rfResult = m_pIssuerList->Restore( pCallContext, TRUE );

        if ( ( rfResult == TRUE ) && ( hAdditionalStore != NULL ) )
        {
            rfResult = m_pIssuerList->GetAdditionalStoreIssuers(
                                         pCallContext,
                                         hAdditionalStore
                                         );
        }
    }

    assert( m_pCertContext != NULL );
    assert( m_pIssuerList != NULL );
    assert( m_pChainEngine == pChainEngine );
    assert( m_hHashEntry != NULL );
    assert( m_hIdentifierEntry != NULL );
    assert( m_hSubjectNameEntry != NULL );
    assert( m_hIssuerNameEntry != NULL );
    assert( m_hKeyIdEntry != NULL );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::~CCertObject, public
//
//  Synopsis:   Destructor
//
//----------------------------------------------------------------------------
CCertObject::~CCertObject ()
{
    if ( m_hKeyIdEntry != NULL )
    {
        I_CryptReleaseLruEntry( m_hKeyIdEntry );
    }

    if ( m_hIssuerNameEntry != NULL )
    {
        I_CryptReleaseLruEntry( m_hIssuerNameEntry );
    }

    if ( m_hSubjectNameEntry != NULL )
    {
        I_CryptReleaseLruEntry( m_hSubjectNameEntry );
    }

    if ( m_hIdentifierEntry != NULL )
    {
        I_CryptReleaseLruEntry( m_hIdentifierEntry );
    }

    if ( m_hHashEntry != NULL )
    {
        I_CryptReleaseLruEntry( m_hHashEntry );
    }

    delete m_pbKeyIdentifier;
    ChainFreeIssuerList( m_pIssuerList );
    ChainFreeAuthorityKeyIdentifier( m_pAuthKeyIdentifier );
    ChainFreeCertificateUsage( m_pUsage );
    CertFreeCertificateContext( m_pCertContext );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::CalculateAdditionalStatus, public
//
//  Synopsis:   calculate additional status bits based on time, usage etc
//
//----------------------------------------------------------------------------
VOID
CCertObject::CalculateAdditionalStatus (
                      IN LPFILETIME pTime,
                      IN PCERT_USAGE_MATCH pRequestedUsage,
                      IN DWORD dwFlags,
                      OUT PCERT_TRUST_STATUS pAdditionalStatus
                      )
{
    CERT_TRUST_STATUS TrustStatus;

    memset( &TrustStatus, 0, sizeof( TrustStatus ) );

    ChainGetUsageStatus(
         &pRequestedUsage->Usage,
         m_pUsage,
         pRequestedUsage->dwType,
         &TrustStatus
         );

    if ( ( CompareFileTime(
                  pTime,
                  &m_pCertContext->pCertInfo->NotBefore
                  ) < 0 ) ||
         ( CompareFileTime(
                  pTime,
                  &m_pCertContext->pCertInfo->NotAfter
                  ) > 0 ) )
    {
        TrustStatus.dwErrorStatus |= CERT_TRUST_IS_NOT_TIME_VALID;
    }

    *pAdditionalStatus = TrustStatus;
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::ChainErrorStatus, public
//
//  Synopsis:   return the chain error status
//
//----------------------------------------------------------------------------
DWORD
CCertObject::ChainErrorStatus ()
{
    DWORD                dwQualityErrorStatus;
    PCERT_ISSUER_ELEMENT pElement = NULL;
    LONG                 lQualityValue = 0;
    LONG                 lQualityElement = 0;

    if ( InfoStatus() & CERT_TRUST_IS_SELF_SIGNED )
    {
        if ( !( ErrorStatus() & CERT_TRUST_IS_UNTRUSTED_ROOT ) ||
              ( m_pIssuerList->IsEmpty() == TRUE ) )
        {
            return( ErrorStatus() & CERT_TRUST_CHAIN_FULL_ERROR_STATUS );
        }
    }

    dwQualityErrorStatus = CERT_TRUST_CHAIN_FULL_ERROR_STATUS;

    while ( ( pElement = m_pIssuerList->NextElement( pElement ) ) != NULL )
    {
        lQualityValue = ChainGetChainErrorStatusQuality(
                             pElement->ChainStatus.dwErrorStatus
                             );

        if ( lQualityValue > lQualityElement )
        {
            lQualityElement = lQualityValue;
            dwQualityErrorStatus = pElement->ChainStatus.dwErrorStatus;
        }
    }

    return( dwQualityErrorStatus );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObject::GetIssuer, public
//
//  Synopsis:   get the best issuer certificate object given the input
//              parameters
//
//----------------------------------------------------------------------------
BOOL
CCertObject::GetIssuer (
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
                )
{
    BOOL                 fResult = TRUE;
    PCERT_ISSUER_ELEMENT pElement = NULL;
    PCCERTOBJECT         pIssuer = NULL;
    CERT_TRUST_STATUS    AdditionalStatus;
    CERT_TRUST_STATUS    CtlStatus;
    CERT_USAGE_MATCH     CtlSignerRequestedUsage;
    FILETIME             CtlSignerRequestedTime;
    LONG                 lQualityValue = 0;
    LONG                 lQualityElement = CERT_QUALITY_MINIMUM;
    CERT_TRUST_STATUS    QualityAdditionalStatus;
    CERT_TRUST_STATUS    QualityCtlStatus;
    CERT_USAGE_MATCH     QualityCtlSignerRequestedUsage;
    FILETIME             QualityCtlSignerRequestedTime;
    PCERT_ISSUER_ELEMENT pQualityElement = NULL;

    ChainEngine()->CertObjectCache()->DisableLruOfObjects();

    fResult = m_pIssuerList->Restore( pCallContext, FALSE );

    if ( ( fResult == TRUE ) &&
         ( hStore != NULL ) &&
         ( m_pIssuerList->IsAdditionalStoreProcessed() == FALSE ) )
    {
        fResult = m_pIssuerList->GetAdditionalStoreIssuers(
                                    pCallContext,
                                    hStore
                                    );
    }

    if (  ( fResult == TRUE ) &&
          ( m_pIssuerList->IsEmpty() == TRUE ) &&
         !( InfoStatus() & CERT_TRUST_IS_SELF_SIGNED ) )
    {
        // BUGBUG: Inherited chain status is not available
        fResult = ChainEngine()->RetrieveAndStoreMatchingIssuersByUrl(
                                         this,
                                         pCallContext,
                                         dwFlags
                                         );
    }

    if ( fResult == FALSE )
    {
        ChainEngine()->CertObjectCache()->EnableLruOfObjects( pCallContext );
        return( FALSE );
    }

    memset(
       &QualityCtlSignerRequestedUsage,
       0,
       sizeof( QualityCtlSignerRequestedUsage )
       );

    while ( ( pElement = m_pIssuerList->NextElement( pElement ) ) != NULL )
    {
        lQualityValue = ChainGetIssuerQuality(
                             pElement,
                             pTime,
                             pCurrentTime,
                             pRequestedUsage,
                             dwFlags,
                             &AdditionalStatus,
                             &CtlStatus,
                             &CtlSignerRequestedUsage,
                             &CtlSignerRequestedTime
                             );

        if ( lQualityValue > lQualityElement )
        {
            lQualityElement = lQualityValue;
            pQualityElement = pElement;
            QualityAdditionalStatus = AdditionalStatus;
            QualityCtlStatus = CtlStatus;

            ChainFreeRequestedUsage( &QualityCtlSignerRequestedUsage );
            QualityCtlSignerRequestedUsage = CtlSignerRequestedUsage;

            QualityCtlSignerRequestedTime = CtlSignerRequestedTime;
        }
        else
        {
            ChainFreeRequestedUsage( &CtlSignerRequestedUsage );
        }
    }

    if ( ( pQualityElement != NULL ) && ( pQualityElement->pIssuer != NULL ) )
    {
        if ( pQualityElement->fCtlIssuer == FALSE )
        {
            *ppTrustListInfo = NULL;
        }
        else
        {
#ifdef CAPI_INCLUDE_CTL        
            fResult = SSCtlAllocAndCopyTrustListInfo(
                           pQualityElement->pCtlIssuerData->pTrustListInfo,
                           ppTrustListInfo
                           );
#else
			fResult = FALSE;
#endif			
        }

        ChainTouchAllChosenIssuerRelatedObjects( pQualityElement->pIssuer );

        if ( fResult == TRUE )
        {
            pQualityElement->pIssuer->AddRef();
            *ppCertObject = pQualityElement->pIssuer;
            *pSubjectStatus = pQualityElement->SubjectStatus;
            *pAdditionalStatus = QualityAdditionalStatus;
            *pCtlStatus = QualityCtlStatus;
            *pTrustListRequestedUsage = QualityCtlSignerRequestedUsage;
            *pTrustListRequestedTime = QualityCtlSignerRequestedTime;
        }
    }
    else
    {
        SetLastError( CRYPT_E_NOT_FOUND );
        fResult = FALSE;
    }

    if ( hStore != NULL )
    {
        m_pIssuerList->PurgeAdditionalStoreIssuers();
    }

    ChainEngine()->CertObjectCache()->EnableLruOfObjects( pCallContext );

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::CCertIssuerList, public
//
//  Synopsis:   Constructor
//
//----------------------------------------------------------------------------
CCertIssuerList::CCertIssuerList (IN PCCERTOBJECT pSubject)
{
    m_pSubject = pSubject;
    m_pIssuerPublicKey = NULL;
    m_pHead = NULL;
    m_fFlushed = FALSE;
    m_fCtlIssuerFlushed = FALSE;
    m_fAdditionalStoreProcessed = FALSE;
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::~CCertIssuerList, public
//
//  Synopsis:   Destructor
//
//----------------------------------------------------------------------------
CCertIssuerList::~CCertIssuerList ()
{
    Flush();
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::AddIssuer, public
//
//  Synopsis:   add an issuer to the list
//
//----------------------------------------------------------------------------
BOOL
CCertIssuerList::AddIssuer (
                    IN PCCHAINCALLCONTEXT pCallContext,
                    IN PCCERTOBJECT pIssuer,
                    IN BOOL fAdditionalStore,
                    IN BOOL fCheckForDuplicate
                    )
{
    PCERT_ISSUER_ELEMENT pElement = NULL;
    CRYPT_HASH_BLOB      HashBlob;

    if ( fCheckForDuplicate == TRUE )
    {
        HashBlob.cbData = pIssuer->CertificateHashSize();
        HashBlob.pbData = pIssuer->CertificateHash();

        if ( CheckForDuplicateElement( &HashBlob, FALSE ) == TRUE )
        {
            SetLastError( CRYPT_E_EXISTS );
            return( FALSE );
        }
    }

    if ( CreateElement(
               pCallContext,
               FALSE,
               pIssuer,
               fAdditionalStore,
               NULL,
               NULL,
               NULL,
               &pElement
               ) == FALSE )
    {
        return( FALSE );
    }

    AddElement( pElement );

    return( TRUE );
}

#ifdef CAPI_INCLUDE_CTL
//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::AddCtlIssuer, public
//
//  Synopsis:   add a CTL based issuer
//
//----------------------------------------------------------------------------
BOOL
CCertIssuerList::AddCtlIssuer (
                    IN PCCHAINCALLCONTEXT pCallContext,
                    IN PCSSCTLOBJECT pSSCtlObject,
                    IN BOOL fAdditionalStore,
                    IN HCERTSTORE hAdditionalStore,
                    IN BOOL fCheckForDuplicate
                    )
{
    PCERT_ISSUER_ELEMENT  pElement = NULL;
    CRYPT_HASH_BLOB       HashBlob;
    PCERT_TRUST_LIST_INFO pTrustListInfo = NULL;

    assert( m_pSubject->InfoStatus() & CERT_TRUST_IS_SELF_SIGNED );

    if ( fCheckForDuplicate == TRUE )
    {
        HashBlob.cbData = pSSCtlObject->CtlHashSize();
        HashBlob.pbData = pSSCtlObject->CtlHash();

        if ( CheckForDuplicateElement( &HashBlob, TRUE ) == TRUE )
        {
            SetLastError( CRYPT_E_EXISTS );
            return( FALSE );
        }
    }

    if ( pSSCtlObject->GetTrustListInfo(
                          m_pSubject,
                          &pTrustListInfo
                          ) == FALSE )
    {
        return( FALSE );
    }

    if ( CreateElement(
               pCallContext,
               TRUE,
               NULL,
               fAdditionalStore,
               hAdditionalStore,
               pSSCtlObject,
               pTrustListInfo,
               &pElement
               ) == FALSE )
    {
        SSCtlFreeTrustListInfo( pTrustListInfo );
        return( FALSE );
    }

    AddElement( pElement );

    return( TRUE );
}
#endif

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::Flush, public
//
//  Synopsis:   flush the issuer list
//
//----------------------------------------------------------------------------
VOID
CCertIssuerList::Flush ()
{
    PCERT_ISSUER_ELEMENT pElement;

    while ( ( pElement = NextElement( NULL ) ) != NULL  )
    {
        RemoveElement( pElement );
        DeleteElement( pElement );
    }

    m_pIssuerPublicKey = NULL;
    m_fFlushed = TRUE;
    m_fCtlIssuerFlushed = TRUE;
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::CtlIssuerFlush, public
//
//  Synopsis:   flush CTL issuers
//
//----------------------------------------------------------------------------
VOID
CCertIssuerList::CtlIssuerFlush (IN PCCERTOBJECT pCtlIssuer)
{
    PCERT_ISSUER_ELEMENT   pElement = NULL;
    CERT_OBJECT_IDENTIFIER CtlIssuerObjectIdentifier;
    CERT_OBJECT_IDENTIFIER IssuerObjectIdentifier;

    assert( m_pSubject->InfoStatus() & CERT_TRUST_IS_SELF_SIGNED );

    pCtlIssuer->GetObjectIdentifier( CtlIssuerObjectIdentifier );

    while ( ( pElement = NextElement( pElement ) ) != NULL )
    {
        if ( ( pElement->pIssuer == NULL ) ||
             ( pElement->fCtlIssuer == FALSE ) )
        {
            continue;
        }

        pElement->pIssuer->GetObjectIdentifier( IssuerObjectIdentifier );

        if ( ( pElement->pIssuer == pCtlIssuer ) ||
             ( memcmp(
                  CtlIssuerObjectIdentifier,
                  IssuerObjectIdentifier,
                  MD5DIGESTLEN
                  ) == 0 ) )
        {
            assert( pElement->pCtlIssuerData->fAdditionalStoreSigner == FALSE );

            pElement->pIssuer->Release();
            pElement->pIssuer = NULL;
            pElement->ChainStatus.dwErrorStatus = CERT_TRUST_CHAIN_FULL_ERROR_STATUS;
        }
    }

    m_fCtlIssuerFlushed = TRUE;
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::Restore, public
//
//  Synopsis:   restore issuer elements
//
//----------------------------------------------------------------------------
BOOL
CCertIssuerList::Restore (IN PCCHAINCALLCONTEXT pCallContext, IN BOOL fForce)
{
    BOOL fResult = TRUE;

    if ( ( IsFlushed() == TRUE ) || ( fForce == TRUE ) )
    {
        assert( IsEmpty() == TRUE );

        fResult = m_pSubject->ChainEngine()->FindAndStoreMatchingIssuers(
                                                 m_pSubject,
                                                 pCallContext
                                                 );

        if ( fResult == TRUE )
        {
            m_fFlushed = FALSE;
            m_fCtlIssuerFlushed = FALSE;
        }
    }
    else if ( IsCtlIssuerFlushed() == TRUE )
    {
        assert( m_pSubject->InfoStatus() & CERT_TRUST_IS_SELF_SIGNED );
#ifdef CAPI_INCLUDE_CTL
        ChainFixupCtlIssuers( m_pSubject, pCallContext, NULL );
#endif

        m_fCtlIssuerFlushed = FALSE;
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::GetAdditionalStoreIssuers, public
//
//  Synopsis:   find issuers in the additional store and put them into the
//              issuer list
//
//----------------------------------------------------------------------------
BOOL
CCertIssuerList::GetAdditionalStoreIssuers (
                    IN PCCHAINCALLCONTEXT pCallContext,
                    IN HCERTSTORE hAdditionalStore
                    )
{
    BOOL fExact = FALSE;

    m_fAdditionalStoreProcessed = TRUE;

    if ( m_pSubject->InfoStatus() & CERT_TRUST_IS_SELF_SIGNED )
    {
#ifdef CAPI_INCLUDE_CTL    
        ChainFixupCtlIssuers( m_pSubject, pCallContext, hAdditionalStore );

        return( ChainFindAndStoreCtlIssuersFromAdditionalStore(
                     m_pSubject,
                     pCallContext,
                     hAdditionalStore
                     ) );
#else
		return TRUE;
#endif		
    }

    // BUGBUG: I think I need to do a "fixupissuers" here

    return( ChainFindAndStoreMatchingIssuersFromCertStore(
                 m_pSubject,
                 pCallContext,
                 hAdditionalStore,
                 TRUE,
                 FALSE
                 ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::PurgeAdditionalStoreIssuers, public
//
//  Synopsis:   removes issuers that have the additional store tag
//
//----------------------------------------------------------------------------
VOID
CCertIssuerList::PurgeAdditionalStoreIssuers ()
{
    PCERT_ISSUER_ELEMENT pElement = NULL;
    PCERT_ISSUER_ELEMENT pTempElement = NULL;

    while ( ( pElement = NextElement( pElement ) ) != NULL  )
    {
        if ( pElement->fAdditionalStore == TRUE )
        {
            pTempElement = pElement;
            pElement = NextElement( pElement );

            RemoveElement( pTempElement );

            if ( m_pIssuerPublicKey == &pTempElement->pIssuer->CertContext()->pCertInfo->SubjectPublicKeyInfo )
            {
                m_pIssuerPublicKey = NULL;
            }

            DeleteElement( pTempElement );
        }
        else if ( ( pElement->fCtlIssuer == TRUE ) &&
                  ( pElement->pCtlIssuerData->fAdditionalStoreSigner == TRUE ) )
        {
            pElement->ChainStatus.dwErrorStatus = CERT_TRUST_CHAIN_FULL_ERROR_STATUS;
            pElement->pIssuer->Release();
            pElement->pIssuer = NULL;
            pElement->pCtlIssuerData->fAdditionalStoreSigner = FALSE;
        }
    }

    m_fAdditionalStoreProcessed = FALSE;
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::CreateElement, public
//
//  Synopsis:   create an element
//
//----------------------------------------------------------------------------
BOOL
CCertIssuerList::CreateElement (
                       IN PCCHAINCALLCONTEXT pCallContext,
                       IN BOOL fCtlIssuer,
                       IN PCCERTOBJECT pIssuer,
                       IN BOOL fAdditionalStore,
                       IN HCERTSTORE hAdditionalStore,
                       IN PCSSCTLOBJECT pSSCtlObject,
                       IN PCERT_TRUST_LIST_INFO pTrustListInfo,
                       OUT PCERT_ISSUER_ELEMENT* ppElement
                       )
{
    BOOL                 fResult = TRUE;
    BOOL                 fCtlSignatureValid = FALSE;
    PCERT_ISSUER_ELEMENT pElement;

    pElement = new CERT_ISSUER_ELEMENT;
    if ( pElement == NULL )
    {
        SetLastError( E_OUTOFMEMORY );
        return( FALSE );
    }

    memset( pElement, 0, sizeof( CERT_ISSUER_ELEMENT ) );

    pElement->fCtlIssuer = fCtlIssuer;
    pElement->fAdditionalStore = fAdditionalStore;

    if ( fCtlIssuer == FALSE )
    {
        pElement->pIssuer = pIssuer;
        pElement->pIssuer->AddRef();

        ChainGetSubjectStatus(
             pIssuer,
             m_pIssuerPublicKey,
             m_pSubject,
             &pElement->SubjectStatus
             );

        if (  ( m_pIssuerPublicKey == NULL ) &&
             !( pElement->SubjectStatus.dwErrorStatus &
                CERT_TRUST_IS_NOT_SIGNATURE_VALID ) )
        {
            m_pIssuerPublicKey = &pIssuer->CertContext()->pCertInfo->SubjectPublicKeyInfo;
        }
    }
    else
    {
#ifdef CAPI_INCLUDE_CTL    
        pElement->pCtlIssuerData = new CTL_ISSUER_DATA;
        if ( pElement->pCtlIssuerData != NULL )
        {
            memset( pElement->pCtlIssuerData, 0, sizeof( CTL_ISSUER_DATA ) );

            pSSCtlObject->AddRef();
            pElement->pCtlIssuerData->pSSCtlObject = pSSCtlObject;
            pElement->pCtlIssuerData->pTrustListInfo = pTrustListInfo;

            pSSCtlObject->GetSigner(
                             m_pSubject,
                             pCallContext,
                             hAdditionalStore,
                             &pElement->pIssuer,
                             &pElement->pCtlIssuerData->fAdditionalStoreSigner,
                             &fCtlSignatureValid
                             );
        }
        else
        {
            SetLastError( E_OUTOFMEMORY );
            fResult = FALSE;
        }
#else
		fResult = FALSE;
#endif //CAPI_INCLUDE_CTL		
    }

    if ( fResult == TRUE )
    {
        if ( pElement->pIssuer != NULL )
        {
            pElement->ChainStatus.dwErrorStatus = pElement->pIssuer->ChainErrorStatus();

            if ( fCtlIssuer == FALSE )
            {
                pElement->ChainStatus.dwErrorStatus |= pElement->SubjectStatus.dwErrorStatus;
            }
            else if ( fCtlSignatureValid == FALSE )
            {
                pElement->ChainStatus.dwErrorStatus |= CERT_TRUST_IS_NOT_SIGNATURE_VALID;
            }
        }
        else
        {
            assert( fCtlIssuer == TRUE );

            pElement->ChainStatus.dwErrorStatus |= CERT_TRUST_CHAIN_FULL_ERROR_STATUS;
        }

        *ppElement = pElement;
    }
    else
    {
        DeleteElement( pElement );
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::DeleteElement, public
//
//  Synopsis:   delete an element
//
//----------------------------------------------------------------------------
VOID
CCertIssuerList::DeleteElement (IN PCERT_ISSUER_ELEMENT pElement)
{
#ifdef CAPI_INCLUDE_CTL
    if ( pElement->fCtlIssuer == TRUE )
    {
        ChainFreeCtlIssuerData( pElement->pCtlIssuerData );
    }
#endif    

    if ( pElement->pIssuer != NULL )
    {
        pElement->pIssuer->Release();
    }

    delete pElement;
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertIssuerList::CheckForDuplicateElement, public
//
//  Synopsis:   check for a duplicate element
//
//----------------------------------------------------------------------------
BOOL
CCertIssuerList::CheckForDuplicateElement (
                      IN PCRYPT_HASH_BLOB pHashBlob,
                      IN BOOL fCtlIssuer
                      )
{
    PCERT_ISSUER_ELEMENT pElement = NULL;

    while ( ( pElement = NextElement( pElement ) ) != NULL )
    {
        if ( pElement->fCtlIssuer == fCtlIssuer )
        {
            if ( fCtlIssuer == FALSE )
            {
                if ( memcmp(
                        pHashBlob->pbData,
                        pElement->pIssuer->CertificateHash(),
                        pHashBlob->cbData
                        ) == 0 )
                {
                    return( TRUE );
                }
            }
            else
            {
#ifdef CAPI_INCLUDE_CTL            
                if ( memcmp(
                        pHashBlob->pbData,
                        pElement->pCtlIssuerData->pSSCtlObject->CtlHash(),
                        pHashBlob->cbData
                        ) == 0 )
                {
                    return( TRUE );
                }
#endif                
            }
        }
    }

    return( FALSE );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::CCertObjectCache, public
//
//  Synopsis:   Constructor
//
//----------------------------------------------------------------------------
CCertObjectCache::CCertObjectCache (
                       IN DWORD MaxIndexEntries,
                       OUT BOOL& rfResult
                       )
{
    LRU_CACHE_CONFIG Config;

    memset( &Config, 0, sizeof( Config ) );

    Config.dwFlags = LRU_CACHE_NO_SERIALIZE | LRU_CACHE_NO_COPY_IDENTIFIER;
    Config.cBuckets = DEFAULT_CERT_OBJECT_CACHE_BUCKETS;

    m_hHashIndex = NULL;
    m_hIdentifierIndex = NULL;
    m_hKeyIdIndex = NULL;
    m_hSubjectNameIndex = NULL;
    m_hIssuerNameIndex = NULL;

    Config.pfnHash = CertObjectCacheHashNameIdentifier;

    rfResult = I_CryptCreateLruCache( &Config, &m_hSubjectNameIndex );

    if ( rfResult == TRUE )
    {
        rfResult = I_CryptCreateLruCache( &Config, &m_hIssuerNameIndex );
    }

    Config.pfnHash = CertObjectCacheHashMd5Identifier;

    if ( rfResult == TRUE )
    {
        rfResult = I_CryptCreateLruCache( &Config, &m_hIdentifierIndex );
    }

    if ( rfResult == TRUE )
    {
        rfResult = I_CryptCreateLruCache( &Config, &m_hKeyIdIndex );
    }

    Config.MaxEntries = MaxIndexEntries;
    Config.pfnOnRemoval = CertObjectCacheOnRemovalFromPrimaryIndex;

    if ( rfResult == TRUE )
    {
        rfResult = I_CryptCreateLruCache( &Config, &m_hHashIndex );
    }
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::~CCertObjectCache, public
//
//  Synopsis:   Destructor
//
//----------------------------------------------------------------------------
CCertObjectCache::~CCertObjectCache ()
{
    I_CryptFreeLruCache(
           m_hHashIndex,
           0,
           NULL
           );

    I_CryptFreeLruCache(
           m_hIssuerNameIndex,
           LRU_SUPPRESS_REMOVAL_NOTIFICATION,
           NULL
           );

    I_CryptFreeLruCache(
           m_hSubjectNameIndex,
           LRU_SUPPRESS_REMOVAL_NOTIFICATION,
           NULL
           );

    I_CryptFreeLruCache(
           m_hIdentifierIndex,
           LRU_SUPPRESS_REMOVAL_NOTIFICATION,
           NULL
           );

    I_CryptFreeLruCache(
           m_hKeyIdIndex,
           LRU_SUPPRESS_REMOVAL_NOTIFICATION,
           NULL
           );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::AddObject, public
//
//  Synopsis:   add an object to the cache
//
//----------------------------------------------------------------------------
BOOL
CCertObjectCache::AddObject (
                     IN PCCHAINCALLCONTEXT pCallContext,
                     IN PCCERTOBJECT pCertObject,
                     IN BOOL fCheckForDuplicate
                     )
{
    if ( fCheckForDuplicate == TRUE )
    {
        CRYPT_HASH_BLOB HashBlob;
        PCCERTOBJECT    pDuplicate;

        HashBlob.cbData = pCertObject->CertificateHashSize();
        HashBlob.pbData = pCertObject->CertificateHash();

        pDuplicate = FindObjectByHash( &HashBlob );
        if ( pDuplicate != NULL )
        {
            pDuplicate->Release();
            SetLastError( CRYPT_E_EXISTS );
            return( FALSE );
        }
    }

    pCertObject->AddRef();

    I_CryptInsertLruEntry( pCertObject->HashIndexEntry(), pCallContext );
    I_CryptInsertLruEntry( pCertObject->IdentifierIndexEntry(), pCallContext );
    I_CryptInsertLruEntry( pCertObject->SubjectNameIndexEntry(), pCallContext );
    I_CryptInsertLruEntry( pCertObject->IssuerNameIndexEntry(), pCallContext );
    I_CryptInsertLruEntry( pCertObject->KeyIdIndexEntry(), pCallContext );

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::FindObjectByHash, public
//
//  Synopsis:   find object by hash
//
//----------------------------------------------------------------------------
PCCERTOBJECT
CCertObjectCache::FindObjectByHash (
                      IN PCRYPT_HASH_BLOB pHash
                      )
{
    return( FindObject( m_hHashIndex, (PCRYPT_DATA_BLOB)pHash ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::FindObjectByIdentifier, public
//
//  Synopsis:   find object by certificate object identifier
//
//----------------------------------------------------------------------------
PCCERTOBJECT
CCertObjectCache::FindObjectByIdentifier (
                      IN CERT_OBJECT_IDENTIFIER ObjectIdentifier
                      )
{
    CRYPT_DATA_BLOB DataBlob;

    DataBlob.cbData = sizeof( CERT_OBJECT_IDENTIFIER );
    DataBlob.pbData = ObjectIdentifier;

    return( FindObject( m_hIdentifierIndex, &DataBlob ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::FindObjectBySubjectName, public
//
//  Synopsis:   find object by subject name
//
//----------------------------------------------------------------------------
PCCERTOBJECT
CCertObjectCache::FindObjectBySubjectName (
                      IN PCERT_NAME_BLOB pSubjectName
                      )
{
    return( FindObject( m_hSubjectNameIndex, (PCRYPT_DATA_BLOB)pSubjectName ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::FindObjectByIssuerName, public
//
//  Synopsis:   find object by issuer name
//
//----------------------------------------------------------------------------
PCCERTOBJECT
CCertObjectCache::FindObjectByIssuerName (
                      IN PCERT_NAME_BLOB pIssuerName
                      )
{
    return( FindObject( m_hIssuerNameIndex, (PCRYPT_DATA_BLOB)pIssuerName ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::FindObjectByKeyId, public
//
//  Synopsis:   find object by key identifier
//
//----------------------------------------------------------------------------
PCCERTOBJECT
CCertObjectCache::FindObjectByKeyId (
                      IN PCRYPT_HASH_BLOB pKeyId
                      )
{
    return( FindObject( m_hKeyIdIndex, (PCRYPT_DATA_BLOB)pKeyId ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::NextMatchingObjectByIdentifier, public
//
//  Synopsis:   next matching object
//
//----------------------------------------------------------------------------
PCCERTOBJECT
CCertObjectCache::NextMatchingObjectByIdentifier (
                      IN PCCERTOBJECT pCertObject
                      )
{
    return( NextMatchingObject(
                pCertObject->IdentifierIndexEntry(),
                pCertObject
                ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::NextMatchingObjectBySubjectName, public
//
//  Synopsis:   next matching object
//
//----------------------------------------------------------------------------
PCCERTOBJECT
CCertObjectCache::NextMatchingObjectBySubjectName (
                      IN PCCERTOBJECT pCertObject
                      )
{
    return( NextMatchingObject(
                pCertObject->SubjectNameIndexEntry(),
                pCertObject
                ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::NextMatchingObjectByIssuerName, public
//
//  Synopsis:   next matching object
//
//----------------------------------------------------------------------------
PCCERTOBJECT
CCertObjectCache::NextMatchingObjectByIssuerName (
                      IN PCCERTOBJECT pCertObject
                      )
{
    return( NextMatchingObject(
                pCertObject->IssuerNameIndexEntry(),
                pCertObject
                ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::NextMatchingObjectByKeyId, public
//
//  Synopsis:   next matching object
//
//----------------------------------------------------------------------------
PCCERTOBJECT
CCertObjectCache::NextMatchingObjectByKeyId (
                      IN PCCERTOBJECT pCertObject
                      )
{
    return( NextMatchingObject(
                pCertObject->KeyIdIndexEntry(),
                pCertObject
                ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::EnumObjects, public
//
//  Synopsis:   enumerate all certificate objects in the cache
//
//----------------------------------------------------------------------------
VOID
CCertObjectCache::EnumObjects (
                      IN PFN_ENUM_CERTOBJECTS pfnEnum,
                      IN LPVOID pvParameter
                      )
{
    CHAIN_ENUM_OBJECTS_DATA EnumData;

    EnumData.pfnEnumObjects = pfnEnum;
    EnumData.pvEnumParameter = pvParameter;

    I_CryptWalkAllLruCacheEntries(
           m_hHashIndex,
           ChainEnumObjectsWalkFn,
           &EnumData
           );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertObjectCache::FindObject, public
//
//  Synopsis:   find object
//
//----------------------------------------------------------------------------
PCCERTOBJECT
CCertObjectCache::FindObject (
                      IN HLRUCACHE hIndex,
                      IN PCRYPT_DATA_BLOB pIdentifier
                      )
{
    HLRUENTRY    hFound;
    PCCERTOBJECT pFound = NULL;

    hFound = I_CryptFindLruEntry( hIndex, pIdentifier );
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
//  Member:     CCertObjectCache::NextMatchingObject, public
//
//  Synopsis:   next matching object
//
//----------------------------------------------------------------------------
PCCERTOBJECT
CCertObjectCache::NextMatchingObject (
                      IN HLRUENTRY hObjectEntry,
                      IN PCCERTOBJECT pCertObject
                      )
{
    HLRUENTRY    hFound;
    PCCERTOBJECT pFound = NULL;

    I_CryptAddRefLruEntry( hObjectEntry );

    hFound = I_CryptEnumMatchingLruEntries( hObjectEntry );
    if ( hFound != NULL )
    {
        pFound = (PCCERTOBJECT)I_CryptGetLruEntryData( hFound );
        pFound->AddRef();

        I_CryptReleaseLruEntry( hFound );
    }

    pCertObject->Release();

    return( pFound );
}

//+---------------------------------------------------------------------------
//
//  Function:   CertObjectCacheOnRemovalFromPrimaryIndex
//
//  Synopsis:   removes the cert object from all other indexes and also
//              removes the reference on the cert object.  It also gets rid
//              of all matching subjects and flushes the issuer lists of
//              all cert objects that have it has an issuer
//
//----------------------------------------------------------------------------
VOID WINAPI
CertObjectCacheOnRemovalFromPrimaryIndex (
    IN LPVOID pv,
    IN OPTIONAL LPVOID pvRemovalContext
    )
{
    ChainProcessRemovalNotification(
         (PCCHAINCALLCONTEXT)pvRemovalContext,
         (PCCERTOBJECT)pv,
         TRUE
         );
}

//+---------------------------------------------------------------------------
//
//  Function:   CertObjectCacheHashMd5Identifier
//
//  Synopsis:   DWORD hash an MD5 identifier.  This is done by taking the
//              first four bytes of the MD5 hash since there is enough
//              randomness already
//
//----------------------------------------------------------------------------
DWORD WINAPI
CertObjectCacheHashMd5Identifier (
    IN PCRYPT_DATA_BLOB pIdentifier
    )
{
    return( *( (LPDWORD)pIdentifier->pbData ) );
}

//+---------------------------------------------------------------------------
//
//  Function:   CertObjectCacheHashNameIdentifier
//
//  Synopsis:   DWORD hash a subject or issuer name.
//
//----------------------------------------------------------------------------
DWORD WINAPI
CertObjectCacheHashNameIdentifier (
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

//+---------------------------------------------------------------------------
//
//  Function:   ChainEnumObjectsWalkFn
//
//  Synopsis:   enumerate objects walk function
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainEnumObjectsWalkFn (
     IN LPVOID pvParameter,
     IN HLRUENTRY hEntry
     )
{
    PCHAIN_ENUM_OBJECTS_DATA pEnumData = (PCHAIN_ENUM_OBJECTS_DATA)pvParameter;

    return( ( *pEnumData->pfnEnumObjects )(
                             pEnumData->pvEnumParameter,
                             (PCCERTOBJECT)I_CryptGetLruEntryData( hEntry )
                             ) );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::CCertChainEngine, public
//
//  Synopsis:   Constructor
//
//----------------------------------------------------------------------------
CCertChainEngine::CCertChainEngine (
                       IN PCERT_CHAIN_ENGINE_CONFIG pConfig,
                       IN BOOL fDefaultEngine,
                       OUT BOOL& rfResult
                       )
{
    HCERTSTORE hWorld = NULL;
    DWORD      dwStoreFlags = CERT_SYSTEM_STORE_CURRENT_USER;

    assert( pConfig->cbSize == sizeof( CERT_CHAIN_ENGINE_CONFIG ) );

    rfResult = TRUE;

    m_cRefs = 1;
    m_hRootStore = NULL;
    m_hRealRootStore = NULL;
    m_hTrustStore = NULL;
    m_hOtherStore = NULL;
    m_hEngineStore = NULL;
    m_hEngineStoreChangeEvent = NULL;
    m_pCertObjectCache = NULL;
#ifdef CAPI_INCLUDE_CTL    
    m_pSSCtlObjectCache = NULL;
#endif    
    m_dwFlags = pConfig->dwFlags;
    m_dwUrlRetrievalTimeout = pConfig->dwUrlRetrievalTimeout;
    m_CycleDetectionModulus = pConfig->CycleDetectionModulus;

    InitializeCriticalSection( &m_Lock );

    if ( pConfig->dwFlags & CERT_CHAIN_USE_LOCAL_MACHINE_STORE )
    {
        dwStoreFlags = CERT_SYSTEM_STORE_LOCAL_MACHINE;
    }

    m_hRealRootStore = CertOpenStore(
                           CERT_STORE_PROV_SYSTEM_W,
                           X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                           NULL,
                           dwStoreFlags | CERT_STORE_MAXIMUM_ALLOWED_FLAG,
                           L"root"
                           );

    if ( m_hRealRootStore == NULL )
    {
        rfResult = FALSE;
        return;
    }

    if ( pConfig->hRestrictedRoot != NULL )
    {
        if ( ChainIsProperRestrictedRoot(
                  m_hRealRootStore,
                  pConfig->hRestrictedRoot
                  ) == TRUE )
        {
            m_hRootStore = CertDuplicateStore( pConfig->hRestrictedRoot );
        }
    }
    else
    {
        m_hRootStore = CertDuplicateStore( m_hRealRootStore );
    }

    if ( m_hRootStore == NULL )
    {
        rfResult = FALSE;
        return;
    }

    if ( ( pConfig->hRestrictedTrust == NULL ) ||
         ( pConfig->hRestrictedOther == NULL ) )
    {
        rfResult = ChainCreateWorldStore(
                        m_hRootStore,
                        pConfig->cAdditionalStore,
                        pConfig->rghAdditionalStore,
                        dwStoreFlags,
                        &hWorld
                        );

        if ( rfResult == FALSE )
        {
            return;
        }
    }

    if ( pConfig->hRestrictedTrust != NULL )
    {
        m_hTrustStore = CertDuplicateStore( pConfig->hRestrictedTrust );
    }
    else
    {
        m_hTrustStore = CertDuplicateStore( hWorld );
    }

    m_hOtherStore = CertOpenStore(
                        CERT_STORE_PROV_COLLECTION,
                        X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                        NULL,
                        CERT_STORE_DEFER_CLOSE_UNTIL_LAST_FREE_FLAG,
                        NULL
                        );

    if ( m_hOtherStore != NULL )
    {
        if ( pConfig->hRestrictedOther != NULL )
        {
            rfResult = CertAddStoreToCollection(
                           m_hOtherStore,
                           pConfig->hRestrictedOther,
                           0,
                           0
                           );

            if ( rfResult == TRUE )
            {
                rfResult = CertAddStoreToCollection(
                               m_hOtherStore,
                               m_hRootStore,
                               0,
                               0
                               );
            }
        }
        else
        {
            rfResult = CertAddStoreToCollection(
                           m_hOtherStore,
                           hWorld,
                           0,
                           0
                           );

            if ( ( rfResult == TRUE ) && ( pConfig->hRestrictedTrust != NULL ) )
            {
                rfResult = CertAddStoreToCollection(
                               m_hOtherStore,
                               pConfig->hRestrictedTrust,
                               0,
                               0
                               );
            }
        }
    }
    else
    {
        rfResult = FALSE;
    }

    if ( hWorld != NULL )
    {
        CertCloseStore( hWorld, 0 );
    }

    if ( rfResult == TRUE )
    {
        rfResult = ChainCreateEngineStore(
                        m_hRootStore,
                        m_hTrustStore,
                        m_hOtherStore,
                        fDefaultEngine,
                        pConfig->dwFlags,
                        &m_hEngineStore,
                        &m_hEngineStoreChangeEvent
                        );
    }

    if ( rfResult == TRUE )
    {
        rfResult = ChainCreateCertificateObjectCache(
                        pConfig->MaximumCachedCertificates,
                        &m_pCertObjectCache
                        );
    }
#ifdef CAPI_INCLUDE_CTL
    if ( rfResult == TRUE )
    {
        rfResult = SSCtlCreateObjectCache( &m_pSSCtlObjectCache );
    }

    if ( rfResult == TRUE )
    {
        rfResult = m_pSSCtlObjectCache->PopulateCache( this );
    }
#endif
    assert( m_hRootStore != NULL );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::~CCertChainEngine, public
//
//  Synopsis:   Destructor
//
//----------------------------------------------------------------------------
CCertChainEngine::~CCertChainEngine ()
{
    ChainFreeCertificateObjectCache( m_pCertObjectCache );
#ifdef CAPI_INCLUDE_CTL    
    SSCtlFreeObjectCache( m_pSSCtlObjectCache );
#endif
    if ( m_hRootStore != NULL )
    {
        CertCloseStore( m_hRootStore, 0 );
    }

    if ( m_hRealRootStore != NULL )
    {
        CertCloseStore( m_hRealRootStore, 0 );
    }

    if ( m_hTrustStore != NULL )
    {
        CertCloseStore( m_hTrustStore, 0 );
    }

    if ( m_hOtherStore != NULL )
    {
        CertCloseStore( m_hOtherStore, 0 );
    }

    if ( m_hEngineStore != NULL )
    {
        CertCloseStore( m_hEngineStore, 0 );
    }

    if ( m_hEngineStoreChangeEvent != NULL )
    {
        CloseHandle( m_hEngineStoreChangeEvent );
    }

    DeleteCriticalSection( &m_Lock );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::GetChainContext, public
//
//  Synopsis:   get a certificate chain context
//
//              NOTE: This method acquires the engine lock
//
//----------------------------------------------------------------------------
BOOL
CCertChainEngine::GetChainContext (
                     IN PCCERT_CONTEXT pCertContext,
                     IN LPFILETIME pTime,
                     IN LPFILETIME pCurrentTime,
                     IN HCERTSTORE hAdditionalStore,
                     IN PCERT_USAGE_MATCH pRequestedUsage,
                     IN DWORD dwFlags,
                     IN LPVOID pvReserved,
                     OUT PCCERT_CHAIN_CONTEXT* ppChainContext
                     )
{
    BOOL               fResult;
    BYTE               CertificateHash[ MD5DIGESTLEN ];
    CRYPT_HASH_BLOB    HashBlob;
    PCCERTOBJECT       pEndCertObject;
    PCCERTOBJECT       pNextEndCertObject;
    CERT_USAGE_MATCH   RequestedUsage;
    CERT_USAGE_MATCH   NextRequestedUsage;
    FILETIME           RequestedTime;
    FILETIME           NextRequestedTime;
    PCCERTCHAINCONTEXT pCertChainContext = NULL;
    DWORD              SimpleChainIndex = 0;
    BOOL               fEndCertInCache = TRUE;
    CERT_TRUST_STATUS  AdditionalStatus;
    PCCHAINCALLCONTEXT pCallContext;
    BOOL               fFromCallCreationCache = FALSE;

    HashBlob.cbData = MD5DIGESTLEN;
    HashBlob.pbData = CertificateHash;

    RequestedTime = *pTime;

    fResult = CertGetCertificateContextProperty(
                  pCertContext,
                  CERT_MD5_HASH_PROP_ID,
                  HashBlob.pbData,
                  &HashBlob.cbData
                  );

    if ( fResult == TRUE )
    {
        fResult = ChainCreateCertificateChainContext( &pCertChainContext );
    }

    if ( fResult == TRUE )
    {
        fResult = CallContextCreateCallObject( &pCallContext );
    }

    if ( fResult == FALSE )
    {
        ChainFreeCertificateChainContext( pCertChainContext );
        return( fResult );
    }

    LockEngine();

    if ( Resync( pCallContext, FALSE ) == FALSE )
    {
        UnlockEngine();
        return( FALSE );
    }

    CertObjectCache()->DisableLruOfObjects();

    pEndCertObject = m_pCertObjectCache->FindObjectByHash( &HashBlob );
    if ( pEndCertObject == NULL )
    {
        fEndCertInCache = FALSE;

        // NOTE: Currently defer passing the additional store until ::GetIssuer
        //       time.  Might want to revisit this decision.
        fResult = ChainCreateCertificateObject(
                       this,
                       pCallContext,
                       pCertContext,
                       &HashBlob,
                       NULL,
                       NULL,
                       &pEndCertObject,
                       &fFromCallCreationCache
                       );

        assert( fFromCallCreationCache == FALSE );
    }

    if ( fResult == TRUE )
    {
        fResult = ChainCopyRequestedUsage( pRequestedUsage, &RequestedUsage );
    }

    if ( fResult == TRUE )
    {
        assert( pEndCertObject != NULL );

        pEndCertObject->CalculateAdditionalStatus(
                                 &RequestedTime,
                                 &RequestedUsage,
                                 dwFlags,
                                 &AdditionalStatus
                                 );

        do
        {
            fResult = AddSimpleChain(
                         pCallContext,
                         pCertChainContext,
                         SimpleChainIndex,
                         pEndCertObject,
                         &AdditionalStatus,
                         &RequestedTime,
                         pCurrentTime,
                         hAdditionalStore,
                         &RequestedUsage,
                         dwFlags,
                         pvReserved,
                         &pNextEndCertObject,
                         &AdditionalStatus,
                         &NextRequestedUsage,
                         &NextRequestedTime
                         );

            if ( ( fResult == TRUE ) &&
                 ( ( m_dwFlags & CERT_CHAIN_CACHE_END_CERT ) ||
                   ( dwFlags & CERT_CHAIN_CACHE_END_CERT ) ) &&
                 ( fEndCertInCache == FALSE ) )
            {
                fEndCertInCache = TRUE;

                fResult = ChainCacheEndCertificate(
                               pCallContext,
                               pEndCertObject
                               );
            }

            SimpleChainIndex += 1;

            ChainFreeRequestedUsage( &RequestedUsage );
            RequestedUsage = NextRequestedUsage;
            RequestedTime = NextRequestedTime;

            pEndCertObject->Release();
            pEndCertObject = pNextEndCertObject;
        }
        while ( ( pEndCertObject != NULL ) && ( fResult == TRUE ) );
    }
    else if ( pEndCertObject != NULL )
    {
        pEndCertObject->Release();
    }

    CertObjectCache()->EnableLruOfObjects( pCallContext );

    UnlockEngine();

    if ( fResult == TRUE )
    {
#ifdef CAPI_INCLUDE_CRL    
        if ( dwFlags & CERT_CHAIN_REVOCATION_CHECK_ALL )
        {
            ChainPostProcessCheckRevocation(
                 OtherStore(),
                 pTime,
                 pCurrentTime,
                 pCertChainContext->ExposedContext(),
                 dwFlags
                 );
        }
#endif

        pCertChainContext->UpdateSummaryStatus();
        pCertChainContext->UpdateEndEntityCertContext( pCertContext );

        *ppChainContext = CertDuplicateCertificateChain(
                              pCertChainContext->ExposedContext()
                              );
    }

    CallContextFreeCallObject( pCallContext );
    ChainFreeCertificateChainContext( pCertChainContext );

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::AddSimpleChain, public
//
//  Synopsis:   add a simple chain to the cert chain context
//
//----------------------------------------------------------------------------
BOOL
CCertChainEngine::AddSimpleChain (
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
                     )
{
    BOOL                  fResult = TRUE;
    PCCERTOBJECT          pCertObject;
    PCCERTOBJECT          pIssuerCertObject = NULL;
    CERT_TRUST_STATUS     SubjectStatus;
    CERT_TRUST_STATUS     AdditionalStatus;
    CERT_TRUST_STATUS     IssuerAdditionalStatus;
    CERT_TRUST_STATUS     CtlStatus;
    PCERT_TRUST_LIST_INFO pTrustListInfo = NULL;

    pCertObject = pEndCertObject;
    pCertObject->AddRef();

    AdditionalStatus = *pEndAdditionalStatus;

    while ( ( pCertObject != NULL ) && ( fResult == TRUE ) )
    {
        memset( &SubjectStatus, 0, sizeof( SubjectStatus ) );
        memset( &CtlStatus, 0, sizeof( CtlStatus ) );

        if ( !( pCertObject->InfoStatus() & CERT_TRUST_IS_SELF_SIGNED ) ||
              ( pCertObject->ErrorStatus() & CERT_TRUST_IS_UNTRUSTED_ROOT ) )
        {
            fResult = pCertObject->GetIssuer(
                                      pCallContext,
                                      pTime,
                                      pCurrentTime,
                                      hAdditionalStore,
                                      pRequestedUsage,
                                      dwFlags,
                                      &pIssuerCertObject,
                                      &SubjectStatus,
                                      &IssuerAdditionalStatus,
                                      &CtlStatus,
                                      &pTrustListInfo,
                                      pNextEndRequestedUsage,
                                      pNextEndRequestedTime
                                      );

            if ( fResult == FALSE )
            {
                fResult = TRUE;
                pIssuerCertObject = NULL;
                pTrustListInfo = NULL;

                memset( pNextEndRequestedUsage, 0, sizeof( CERT_USAGE_MATCH ) );
                memset( pNextEndRequestedTime, 0, sizeof( FILETIME ) );
            }
        }
        else
        {
            pIssuerCertObject = NULL;
        }

        if ( fResult == TRUE )
        {
            fResult = pCertChainContext->AddChainElement(
                                            NewSimpleChainIndex,
                                            pCertObject,
                                            &SubjectStatus,
                                            &AdditionalStatus,
                                            &CtlStatus,
                                            pTrustListInfo
                                            );
        }

        if ( fResult == TRUE )
        {
            if ( pCertObject->InfoStatus() & CERT_TRUST_IS_SELF_SIGNED )
            {
                *ppNextEndCertObject = pIssuerCertObject;
                *pNextEndAdditionalStatus = IssuerAdditionalStatus;
                pIssuerCertObject = NULL;
            }
            else if ( pIssuerCertObject == NULL )
            {
                *ppNextEndCertObject = NULL;
            }
            else
            {
                AdditionalStatus = IssuerAdditionalStatus;
            }
        }
        else
        {
            if ( GetLastError() != CERT_E_CHAINING )
            {
#ifdef CAPI_INCLUDE_CTL            
                if ( pTrustListInfo != NULL )
                {
                    SSCtlFreeTrustListInfo( pTrustListInfo );
                }
#else
				assert(!pTrustListInfo);
#endif				
            }

            if ( pIssuerCertObject != NULL )
            {
                pIssuerCertObject->Release();
            }

            ChainFreeRequestedUsage( pNextEndRequestedUsage );
            memset( pNextEndRequestedUsage, 0, sizeof( CERT_USAGE_MATCH ) );
            memset( pNextEndRequestedTime, 0, sizeof( FILETIME ) );
        }

        pCertObject->Release();
        pCertObject = pIssuerCertObject;
    }

    if ( ( fResult == FALSE ) && ( GetLastError() == CERT_E_CHAINING ) )
    {
        *ppNextEndCertObject = NULL;
        fResult = TRUE;
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::FindAndStoreMatchingIssuers, public
//
//  Synopsis:   find and store matching issuers
//
//----------------------------------------------------------------------------
BOOL
CCertChainEngine::FindAndStoreMatchingIssuers (
                      IN PCCERTOBJECT pCertObject,
                      IN PCCHAINCALLCONTEXT pCallContext
                      )
{
    DWORD dwInfoStatus = pCertObject->InfoStatus();

    assert( pCertObject->ChainEngine() == this );

    if ( dwInfoStatus & CERT_TRUST_IS_SELF_SIGNED )
    {
        return( FindAndStoreCtlIssuers( pCertObject, pCallContext ) );
    }

    if ( dwInfoStatus & CERT_TRUST_HAS_EXACT_MATCH_ISSUER )
    {
        return( FindAndStoreExactMatchIssuers( pCertObject, pCallContext ) );
    }
    else if ( dwInfoStatus & CERT_TRUST_HAS_KEY_MATCH_ISSUER )
    {
        return( FindAndStoreKeyMatchIssuers( pCertObject, pCallContext ) );
    }
    else if ( dwInfoStatus & CERT_TRUST_HAS_NAME_MATCH_ISSUER )
    {
        return( FindAndStoreNameMatchIssuers( pCertObject, pCallContext ) );
    }
    else
    {
        return( TRUE );
    }
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::FindAndStoreExactMatchIssuers, public
//
//  Synopsis:   find and store exact match issuers
//
//----------------------------------------------------------------------------
BOOL
CCertChainEngine::FindAndStoreExactMatchIssuers (
                      IN PCCERTOBJECT pCertObject,
                      IN PCCHAINCALLCONTEXT pCallContext
                      )
{
    BOOL                        fResult = TRUE;
    PCCERTOBJECT                pIssuer;
    BOOL                        fCheckForDuplicate;
    CERT_OBJECT_IDENTIFIER      ObjectIdentifier;
    PCERT_AUTHORITY_KEY_ID_INFO pAuthKeyIdentifier;

    assert( pCertObject->ChainEngine() == this );

    fCheckForDuplicate = !pCertObject->IssuerList()->IsEmpty();
    pAuthKeyIdentifier = pCertObject->AuthorityKeyIdentifier();

    ChainCreateCertificateObjectIdentifier(
         &pAuthKeyIdentifier->CertIssuer,
         &pAuthKeyIdentifier->CertSerialNumber,
         ObjectIdentifier
         );

    pIssuer = CertObjectCache()->FindObjectByIdentifier( ObjectIdentifier );

    while ( pIssuer != NULL )
    {
        pCertObject->IssuerList()->AddIssuer(
                                      pCallContext,
                                      pIssuer,
                                      FALSE,
                                      fCheckForDuplicate
                                      );

        pIssuer = CertObjectCache()->NextMatchingObjectByIdentifier( pIssuer );
    }

    if ( pCertObject->IssuerList()->IsEmpty() == TRUE )
    {
        fResult = ChainFindAndStoreMatchingIssuersFromCertStore(
                       pCertObject,
                       pCallContext,
                       OtherStore(),
                       FALSE,
                       TRUE
                       );
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::FindAndStoreNameMatchIssuers, public
//
//  Synopsis:   find and store name match issuers
//
//----------------------------------------------------------------------------
BOOL
CCertChainEngine::FindAndStoreNameMatchIssuers (
                      IN PCCERTOBJECT pCertObject,
                      IN PCCHAINCALLCONTEXT pCallContext
                      )
{
    BOOL         fResult = TRUE;
    PCCERTOBJECT pIssuer;
    BOOL         fCheckForDuplicate;

    assert( pCertObject->ChainEngine() == this );

    fCheckForDuplicate = !pCertObject->IssuerList()->IsEmpty();

    pIssuer = CertObjectCache()->FindObjectBySubjectName(
                                     &pCertObject->CertContext()->pCertInfo->Issuer
                                     );

    while ( pIssuer != NULL )
    {
        pCertObject->IssuerList()->AddIssuer(
                                      pCallContext,
                                      pIssuer,
                                      FALSE,
                                      fCheckForDuplicate
                                      );

        pIssuer = CertObjectCache()->NextMatchingObjectBySubjectName( pIssuer );
    }

    if ( pCertObject->IssuerList()->IsEmpty() == TRUE )
    {
        fResult = ChainFindAndStoreMatchingIssuersFromCertStore(
                       pCertObject,
                       pCallContext,
                       OtherStore(),
                       FALSE,
                       TRUE
                       );
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::FindAndStoreKeyMatchIssuers, public
//
//  Synopsis:   find and store key match based issuers
//
//----------------------------------------------------------------------------
BOOL
CCertChainEngine::FindAndStoreKeyMatchIssuers (
                      IN PCCERTOBJECT pCertObject,
                      IN PCCHAINCALLCONTEXT pCallContext
                      )
{
    BOOL                        fResult = TRUE;
    PCCERTOBJECT                pIssuer;
    BOOL                        fCheckForDuplicate;
    PCERT_AUTHORITY_KEY_ID_INFO pAuthKeyIdentifier;

    assert( pCertObject->ChainEngine() == this );

    fCheckForDuplicate = !pCertObject->IssuerList()->IsEmpty();
    pAuthKeyIdentifier = pCertObject->AuthorityKeyIdentifier();

    pIssuer = CertObjectCache()->FindObjectByKeyId(
                                     &pAuthKeyIdentifier->KeyId
                                     );

    while ( pIssuer != NULL )
    {
        pCertObject->IssuerList()->AddIssuer(
                                      pCallContext,
                                      pIssuer,
                                      FALSE,
                                      fCheckForDuplicate
                                      );

        pIssuer = CertObjectCache()->NextMatchingObjectByKeyId( pIssuer );
    }

    if ( pCertObject->IssuerList()->IsEmpty() == TRUE )
    {
        fResult = ChainFindAndStoreMatchingIssuersFromCertStore(
                       pCertObject,
                       pCallContext,
                       OtherStore(),
                       FALSE,
                       TRUE
                       );
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::RetrieveAndStoreMatchingIssuersByUrl, public
//
//  Synopsis:   retrieve the issuers using the exact match issuer URL
//
//----------------------------------------------------------------------------
BOOL
CCertChainEngine::RetrieveAndStoreMatchingIssuersByUrl (
                          IN PCCERTOBJECT pCertObject,
                          IN PCCHAINCALLCONTEXT pCallContext,
                          IN DWORD dwFlags
                          )
{
    BOOL             fResult;
    DWORD            cbUrlArray;
    PCRYPT_URL_ARRAY pUrlArray = NULL;
    DWORD            cCount;
    HCERTSTORE       hStore;
    DWORD            dwRetrievalFlags = CRYPT_RETRIEVE_MULTIPLE_OBJECTS;
    DWORD            dwCacheResultFlag;

    if ( ( m_dwFlags & CERT_CHAIN_CACHE_ONLY_URL_RETRIEVAL ) ||
         ( dwFlags & CERT_CHAIN_CACHE_ONLY_URL_RETRIEVAL ) )
    {
        dwRetrievalFlags |= CRYPT_CACHE_ONLY_RETRIEVAL;
    }

    fResult = ChainGetObjectUrl(
                   URL_OID_CERTIFICATE_ISSUER,
                   (LPVOID)pCertObject->CertContext(),
                   CRYPT_GET_URL_FROM_EXTENSION,
                   NULL,
                   &cbUrlArray,
                   NULL,
                   NULL,
                   NULL
                   );

    if ( fResult == TRUE )
    {
        pUrlArray = (PCRYPT_URL_ARRAY)new BYTE [ cbUrlArray ];
        if ( pUrlArray == NULL )
        {
            SetLastError( E_OUTOFMEMORY );
            return( FALSE );
        }

        fResult = ChainGetObjectUrl(
                       URL_OID_CERTIFICATE_ISSUER,
                       (LPVOID)pCertObject->CertContext(),
                       CRYPT_GET_URL_FROM_EXTENSION,
                       pUrlArray,
                       &cbUrlArray,
                       NULL,
                       NULL,
                       NULL
                       );
    }

    if ( fResult == TRUE )
    {
        for ( cCount = 0; cCount < pUrlArray->cUrl; cCount++ )
        {
            if ( !( dwRetrievalFlags & CRYPT_CACHE_ONLY_RETRIEVAL ) &&
                  ( ChainIsFileOrLdapUrl( pUrlArray->rgwszUrl[ cCount ] ) == TRUE ) )
            {
                dwCacheResultFlag = CRYPT_DONT_CACHE_RESULT;
            }
            else
            {
                dwCacheResultFlag = 0;
            }

            fResult = ChainRetrieveObjectByUrlW(
                           pUrlArray->rgwszUrl[ cCount ],
                           CONTEXT_OID_CERTIFICATE,
                           dwRetrievalFlags | dwCacheResultFlag,
                           m_dwUrlRetrievalTimeout,
                           (LPVOID *)&hStore,
                           NULL,
                           NULL,
                           NULL,
                           NULL
                           );

            if ( fResult == TRUE )
            {
                fResult = ChainFindAndStoreMatchingIssuersFromCertStore(
                               pCertObject,
                               pCallContext,
                               hStore,
                               FALSE,
                               TRUE
                               );

                CertCloseStore( hStore, 0 );
                break;
            }
        }
    }

    delete (LPBYTE)pUrlArray;

    // NOTE: Need to somehow log that we tried to retrieve the issuer but
    //       it was inaccessible

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::FindAndStoreCtlIssuers, public
//
//  Synopsis:   find and store CTL based issuers from the engine
//
//----------------------------------------------------------------------------
BOOL
CCertChainEngine::FindAndStoreCtlIssuers (
                      IN PCCERTOBJECT pCertObject,
                      IN PCCHAINCALLCONTEXT pCallContext
                      )
{

    assert( pCertObject->InfoStatus() & CERT_TRUST_IS_SELF_SIGNED );

    if ( !( pCertObject->ErrorStatus() & CERT_TRUST_IS_UNTRUSTED_ROOT ) )
    {
        return( TRUE );
    }
#ifdef CAPI_INCLUDE_CTL
    CTL_ISSUERS_ENUM_DATA EnumData;
    EnumData.pCertObject = pCertObject;
    EnumData.pCallContext = pCallContext;

    m_pSSCtlObjectCache->EnumObjects(
                             ChainFindAndStoreCtlIssuersEnumFn,
                             &EnumData
                             );
#endif                             

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainEngine::Resync, public
//
//  Synopsis:   resync the store if necessary
//
//----------------------------------------------------------------------------
BOOL
CCertChainEngine::Resync (IN PCCHAINCALLCONTEXT pCallContext, BOOL fForce)
{
    BOOL fResult;

    if ( fForce == FALSE )
    {
        if ( WaitForSingleObject(
                 m_hEngineStoreChangeEvent,
                 0
                 ) != WAIT_OBJECT_0 )
        {
            return( TRUE );
        }
    }

    // BUGBUG: Should I trap errors from this or just continue with the current
    //         state of the engine store without resync??
    CertControlStore(
        m_hEngineStore,
        0,
        CERT_STORE_CTRL_RESYNC,
        &m_hEngineStoreChangeEvent
        );

    m_pCertObjectCache->FlushObjects( pCallContext );

#ifdef CAPI_INCLUDE_CTL
    fResult = m_pSSCtlObjectCache->Resync( this );
#else
	fResult = TRUE;
#endif	

    assert( fResult == TRUE );

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainContext::CCertChainContext, public
//
//  Synopsis:   Constructor
//
//----------------------------------------------------------------------------
CCertChainContext::CCertChainContext (OUT BOOL& rfResult)
{
    rfResult = TRUE;

    m_pContext = new INTERNAL_CERT_CHAIN_CONTEXT;
    if ( m_pContext != NULL )
    {
        memset( m_pContext, 0, sizeof( INTERNAL_CERT_CHAIN_CONTEXT ) );

        m_pContext->cRefs = 1;
        m_pContext->ChainContext.cbSize = sizeof( CERT_CHAIN_CONTEXT );

        rfResult = GrowSimpleChains( INITIAL_CHAIN_ALLOC );
    }
    else
    {
        SetLastError( E_OUTOFMEMORY );
        rfResult = FALSE;
    }
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainContext::~CCertChainContext, public
//
//  Synopsis:   Destructor
//
//----------------------------------------------------------------------------
CCertChainContext::~CCertChainContext ()
{
    ChainReleaseInternalChainContext( m_pContext );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainContext::AddChainElement, public
//
//  Synopsis:   add a chain element to a specified simple chain in the context
//
//----------------------------------------------------------------------------
BOOL
CCertChainContext::AddChainElement (
                      IN DWORD SimpleChain,
                      IN PCCERTOBJECT pCertObject,
                      IN PCERT_TRUST_STATUS pSubjectStatus,
                      IN PCERT_TRUST_STATUS pAdditionalStatus,
                      IN PCERT_TRUST_STATUS pCtlStatus,
                      IN PCERT_TRUST_LIST_INFO pTrustListInfo
                      )
{
    BOOL                fResult = TRUE;
    PCERT_CHAIN_ELEMENT pCurrentElement;
    PCERT_SIMPLE_CHAIN  pChain;

    if ( SimpleChain >= m_pContext->cSimpleChain )
    {
        fResult = GrowSimpleChains(
                      max(
                       GROW_CHAIN_ALLOC,
                       SimpleChain - m_pContext->cSimpleChain + 1
                       )
                      );
    }

    if ( ( fResult == TRUE ) &&
         ( m_pContext->ChainContext.rgpChain[ SimpleChain ]->cElement ==
           m_pContext->rgElementCount[ SimpleChain ] ) )
    {
        fResult = GrowChainElements( SimpleChain, GROW_ELEMENT_ALLOC );
    }

    if ( fResult == FALSE )
    {
        return( fResult );
    }

    pChain = m_pContext->ChainContext.rgpChain[ SimpleChain ];
    pCurrentElement = pChain->rgpElement[ pChain->cElement ];

    pCurrentElement->pCertContext = CertDuplicateCertificateContext(
                                        pCertObject->CertContext()
                                        );

    ChainOrInStatusBits(
         &pCurrentElement->TrustStatus,
         pCertObject->TrustStatus()
         );

    ChainOrInStatusBits( &pCurrentElement->TrustStatus, pAdditionalStatus );
    ChainOrInStatusBits( &pCurrentElement->TrustStatus, pSubjectStatus );

    if ( pCertObject->InfoStatus() & CERT_TRUST_IS_SELF_SIGNED )
    {
        pChain->pTrustListInfo = pTrustListInfo;

        ChainOrInStatusBits( &pChain->TrustStatus, pCtlStatus );
    }

    pChain->cElement += 1;
    m_pContext->TotalElements += 1;

    m_pContext->ChainContext.cChain = max(
                                       m_pContext->ChainContext.cChain,
                                       SimpleChain + 1
                                       );

    if ( ( m_pContext->TotalElements % pCertObject->ChainEngine()->CycleDetectionModulus() ) == 0 )
    {
        if ( ChainIsCertificateObjectRepeatedInContext(
                  pCertObject,
                  &m_pContext->ChainContext
                  ) == TRUE )
        {
            ChainFindAndAdjustChainContextToCycle(
                 &m_pContext->ChainContext
                 );

            SetLastError( CERT_E_CHAINING );
            return( FALSE );
        }
    }

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainContext::UpdateEndEntityCertContext, public
//
//  Synopsis:   update the end entity cert context in the chain context
//
//----------------------------------------------------------------------------
VOID
CCertChainContext::UpdateEndEntityCertContext (PCCERT_CONTEXT pEndCertContext)
{
    PCCERT_CONTEXT pCertContext;

    if ( m_pContext->ChainContext.rgpChain[0]->rgpElement[0]->pCertContext ==
         pEndCertContext )
    {
        return;
    }

    pCertContext = m_pContext->ChainContext.rgpChain[0]->rgpElement[0]->pCertContext;
    m_pContext->ChainContext.rgpChain[0]->rgpElement[0]->pCertContext = pEndCertContext;

#ifdef CMS_PKCS7
    {
        DWORD cbData;
        DWORD cbEndData;

        // If the chain context's end context has the public key parameter
        // property and the end context passed in to CertGetCertificateChain
        // doesn't, then copy the public key parameter property.
        if (CertGetCertificateContextProperty(
                pCertContext,
                CERT_PUBKEY_ALG_PARA_PROP_ID,
                NULL,                       // pvData
                &cbData) && 0 < cbData &&
            !CertGetCertificateContextProperty(
                pEndCertContext,
                CERT_PUBKEY_ALG_PARA_PROP_ID,
                NULL,                       // pvData
                &cbEndData))
        {
            BYTE *pbData;

            __try {
                pbData = (BYTE *) _alloca(cbData);
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                pbData = NULL;
            }
            if (pbData)
            {
                if (CertGetCertificateContextProperty(
                        pCertContext,
                        CERT_PUBKEY_ALG_PARA_PROP_ID,
                        pbData,
                        &cbData))
                {
                    CRYPT_DATA_BLOB Para;
                    Para.pbData = pbData;
                    Para.cbData = cbData;
                    CertSetCertificateContextProperty(
                        pEndCertContext,
                        CERT_PUBKEY_ALG_PARA_PROP_ID,
                        CERT_SET_PROPERTY_IGNORE_PERSIST_ERROR_FLAG,
                        &Para
                        );
                }
            }
        }
    }
#endif

    CertDuplicateCertificateContext( pEndCertContext );
    CertFreeCertificateContext( pCertContext );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainContext::UpdateSummaryStatus, public
//
//  Synopsis:   update the summary status for simple chains and the context
//
//----------------------------------------------------------------------------
VOID
CCertChainContext::UpdateSummaryStatus ()
{
    PCERT_SIMPLE_CHAIN pChain;
    DWORD              cChain;
    DWORD              cElement;

    ChainFixupUntrustedRootElementStatus( &m_pContext->ChainContext );

    for ( cChain = 0; cChain < m_pContext->ChainContext.cChain; cChain++ )
    {
        pChain = m_pContext->ChainContext.rgpChain[ cChain ];

        assert( pChain->cElement > 0 );

        for ( cElement = 0;
              cElement < pChain->cElement;
              cElement++ )
        {
            ChainUpdateSummaryStatusByTrustStatus(
                 &pChain->TrustStatus,
                 &pChain->rgpElement[ cElement ]->TrustStatus
                 );
        }

        if ( !( pChain->rgpElement[ pChain->cElement - 1 ]->TrustStatus.dwInfoStatus &
                CERT_TRUST_IS_SELF_SIGNED ) )
        {
            pChain->TrustStatus.dwErrorStatus |= CERT_TRUST_IS_PARTIAL_CHAIN;
        }

        ChainUpdateSummaryStatusByTrustStatus(
             &m_pContext->ChainContext.TrustStatus,
             &pChain->TrustStatus
             );
    }

    if ( m_pContext->ChainContext.cChain > 1 )
    {
        m_pContext->ChainContext.TrustStatus.dwInfoStatus |= CERT_TRUST_IS_COMPLEX_CHAIN;
    }
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainContext::GrowSimpleChains, private
//
//  Synopsis:   grow the number of simple chains allocated
//
//----------------------------------------------------------------------------
BOOL
CCertChainContext::GrowSimpleChains (IN DWORD cGrowChain)
{
    BOOL                fResult = TRUE;
    DWORD               cSimpleChain;
    PCERT_SIMPLE_CHAIN* rgpChain;
    DWORD*              rgElementCount;
    DWORD               cCount;

    cSimpleChain = m_pContext->cSimpleChain + cGrowChain;
    rgpChain = new PCERT_SIMPLE_CHAIN [ cSimpleChain ];
    rgElementCount = new DWORD [ cSimpleChain ];

    if ( ( rgpChain != NULL ) && ( rgElementCount != NULL ) )
    {
        memset( rgpChain, 0, cSimpleChain * sizeof( PCERT_SIMPLE_CHAIN ) );
        memset( rgElementCount, 0, cSimpleChain * sizeof( DWORD ) );

        for ( cCount = m_pContext->cSimpleChain;
              ( cCount < cSimpleChain ) && ( fResult == TRUE );
              cCount++ )
        {
            rgpChain[ cCount ] = new CERT_SIMPLE_CHAIN;
            if ( rgpChain[ cCount ] != NULL )
            {
                memset( rgpChain[ cCount ], 0, sizeof( CERT_SIMPLE_CHAIN ) );
                rgpChain[ cCount ]->cbSize = sizeof( CERT_SIMPLE_CHAIN );
            }
            else
            {
                fResult = FALSE;
            }
        }
    }
    else
    {
        fResult = FALSE;
    }

    if ( fResult == TRUE )
    {
        if ( m_pContext->ChainContext.rgpChain != NULL )
        {
            memcpy(
               rgpChain,
               m_pContext->ChainContext.rgpChain,
               m_pContext->ChainContext.cChain * sizeof( PCERT_SIMPLE_CHAIN )
               );

            memcpy(
               rgElementCount,
               m_pContext->rgElementCount,
               m_pContext->ChainContext.cChain * sizeof( DWORD )
               );

            delete m_pContext->ChainContext.rgpChain;
            delete m_pContext->rgElementCount;
        }

        m_pContext->cSimpleChain = cSimpleChain;
        m_pContext->ChainContext.rgpChain = rgpChain;
        m_pContext->rgElementCount = rgElementCount;
    }
    else
    {
        for ( cCount = m_pContext->cSimpleChain;
              cCount < cSimpleChain;
              cCount++ )
        {
            delete rgpChain[ cCount ];
        }

        delete rgpChain;
        delete rgElementCount;

        SetLastError( E_OUTOFMEMORY );
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Member:     CCertChainContext::GrowChainElements, public
//
//  Synopsis:   grow the number of chain elements in a particular simple chain
//
//----------------------------------------------------------------------------
BOOL
CCertChainContext::GrowChainElements (
                       IN DWORD SimpleChain,
                       IN DWORD cGrowElement
                       )
{
    BOOL                 fResult = TRUE;
    DWORD                cElement;
    PCERT_CHAIN_ELEMENT* rgpElement;
    DWORD                cCount;

    assert( m_pContext->ChainContext.rgpChain[ SimpleChain ]->cElement ==
            m_pContext->rgElementCount[ SimpleChain ] );

    cElement = m_pContext->rgElementCount[ SimpleChain ] + cGrowElement;
    rgpElement = new PCERT_CHAIN_ELEMENT [ cElement ];
    if ( rgpElement != NULL )
    {
        memset( rgpElement, 0, cElement * sizeof( PCERT_CHAIN_ELEMENT ) );

        for ( cCount = m_pContext->rgElementCount[ SimpleChain ];
              ( cCount < cElement ) && ( fResult == TRUE );
              cCount++ )
        {
            rgpElement[ cCount ] = new CERT_CHAIN_ELEMENT;
            if ( rgpElement[ cCount ] != NULL )
            {
                memset( rgpElement[ cCount ], 0, sizeof( CERT_CHAIN_ELEMENT ) );
                rgpElement[ cCount ]->cbSize = sizeof( CERT_CHAIN_ELEMENT );
            }
            else
            {
                fResult = FALSE;
            }
        }
    }
    else
    {
        cElement = m_pContext->rgElementCount[ SimpleChain ];
        fResult = FALSE;
    }

    if ( fResult == TRUE )
    {
        if ( m_pContext->ChainContext.rgpChain[ SimpleChain ]->rgpElement != NULL )
        {
            memcpy(
               rgpElement,
               m_pContext->ChainContext.rgpChain[ SimpleChain ]->rgpElement,
               m_pContext->ChainContext.rgpChain[ SimpleChain ]->cElement *
               sizeof( PCERT_CHAIN_ELEMENT )
               );

            delete m_pContext->ChainContext.rgpChain[ SimpleChain ]->rgpElement;
        }

        m_pContext->ChainContext.rgpChain[ SimpleChain ]->rgpElement = rgpElement;
        m_pContext->rgElementCount[ SimpleChain ] = cElement;
    }
    else
    {
        for ( cCount = m_pContext->rgElementCount[ SimpleChain ];
              cCount < cElement;
              cCount++ )
        {
            delete rgpElement[ cCount ];
        }

        delete rgpElement;

        SetLastError( E_OUTOFMEMORY );
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainCreateCertificateObjectIdentifier
//
//  Synopsis:   create an object identifier given the issuer name and serial
//              number.  This is done using an MD5 hash over the content
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainCreateCertificateObjectIdentifier (
     IN PCERT_NAME_BLOB pIssuer,
     IN PCRYPT_INTEGER_BLOB pSerialNumber,
     OUT CERT_OBJECT_IDENTIFIER ObjectIdentifier
     )
{
    MD5_CTX md5ctx;

    MD5Init( &md5ctx );

    MD5Update( &md5ctx, pIssuer->pbData, pIssuer->cbData );
    MD5Update( &md5ctx, pSerialNumber->pbData, pSerialNumber->cbData );

    MD5Final( &md5ctx );

    memcpy( ObjectIdentifier, md5ctx.digest, MD5DIGESTLEN );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainGetCertificateUsage
//
//  Synopsis:   get the merged enhanced key usage from the certificate context
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainGetCertificateUsage (
     IN PCCERT_CONTEXT pCertContext,
     OUT PCERT_ENHKEY_USAGE* ppUsage
     )
{
    BOOL               fResult;
    int                cUsage;
    DWORD              cbUsage = 0;
    DWORD              cbStruct;
    PCERT_ENHKEY_USAGE pUsage = NULL;

    fResult = CertGetValidUsages(
                  1,
                  &pCertContext,
                  &cUsage,
                  NULL,
                  &cbUsage
                  );

    if ( fResult == TRUE )
    {
        if ( cUsage == -1 )
        {
            *ppUsage = NULL;
            return( TRUE );
        }

        cbStruct = sizeof( CERT_ENHKEY_USAGE );
        pUsage = (PCERT_ENHKEY_USAGE)new BYTE [ cbUsage + cbStruct ];
        if ( pUsage != NULL )
        {
            pUsage->rgpszUsageIdentifier = (LPSTR*)( (LPBYTE)pUsage + cbStruct );

            fResult =  CertGetValidUsages(
                           1,
                           &pCertContext,
                           &cUsage,
                           pUsage->rgpszUsageIdentifier,
                           &cbUsage
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
        pUsage->cUsageIdentifier = (DWORD)cUsage;
        *ppUsage = pUsage;
    }
    else
    {
        delete (LPBYTE)pUsage;
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFreeCertificateUsage
//
//  Synopsis:   free the usage retrieved by ChainGetCertificateUsage
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFreeCertificateUsage (
     IN PCERT_ENHKEY_USAGE pUsage
     )
{
    delete (LPBYTE)pUsage;
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainCreateIssuerList
//
//  Synopsis:   create the issuer list object for the given subject
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainCreateIssuerList (
     IN PCCERTOBJECT pSubject,
     OUT PCCERTISSUERLIST* ppIssuerList
     )
{
    PCCERTISSUERLIST pIssuerList;

    pIssuerList = new CCertIssuerList( pSubject );
    if ( pIssuerList == NULL )
    {
        SetLastError( E_OUTOFMEMORY );
        return( FALSE );
    }

    *ppIssuerList = pIssuerList;
    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFreeIssuerList
//
//  Synopsis:   free the issuer list object
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFreeIssuerList (
     IN PCCERTISSUERLIST pIssuerList
     )
{
    delete pIssuerList;
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainGetSelfSignedStatus
//
//  Synopsis:   return status bits specifying if the certificate is self signed
//              and if so, if it is signature valid
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainGetSelfSignedStatus (
     IN PCCERTOBJECT pCertObject,
     OUT PCERT_TRUST_STATUS pStatus
     )
{
    assert( ( pCertObject->InfoStatus() & CERT_TRUST_HAS_NAME_MATCH_ISSUER ) ||
            ( pCertObject->InfoStatus() & CERT_TRUST_HAS_EXACT_MATCH_ISSUER ) ||
            ( pCertObject->InfoStatus() & CERT_TRUST_HAS_KEY_MATCH_ISSUER ) );

    memset( pStatus, 0, sizeof( CERT_TRUST_STATUS ) );

    if ( ChainIsMatchingIssuerCertificate(
              pCertObject,
              pCertObject->CertContext()
              ) == TRUE )
    {
        pStatus->dwInfoStatus |= CERT_TRUST_IS_SELF_SIGNED;
    }

    if ( pStatus->dwInfoStatus & CERT_TRUST_IS_SELF_SIGNED )
    {
#ifdef CMS_PKCS7
        if (!CryptVerifyCertificateSignatureEx(
                NULL,                   // hCryptProv
                X509_ASN_ENCODING,
                CRYPT_VERIFY_CERT_SIGN_SUBJECT_CERT,
                (void *) pCertObject->CertContext(),
                CRYPT_VERIFY_CERT_SIGN_ISSUER_CERT,
                (void *) pCertObject->CertContext(),
                0,                      // dwFlags
                NULL                    // pvReserved
                ))
#else
        if ( CryptVerifyCertificateSignature(
                  NULL,
                  X509_ASN_ENCODING,
                  pCertObject->CertContext()->pbCertEncoded,
                  pCertObject->CertContext()->cbCertEncoded,
                  &pCertObject->CertContext()->pCertInfo->SubjectPublicKeyInfo
                  ) == FALSE )
#endif
        {
            pStatus->dwErrorStatus |= CERT_TRUST_IS_NOT_SIGNATURE_VALID;
        }
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainGetRootStoreStatus
//
//  Synopsis:   determine if the certificate with the given hash is in the
//              root store
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainGetRootStoreStatus (
     IN HCERTSTORE hRoot,
     IN HCERTSTORE hRealRoot,
     IN PCRYPT_HASH_BLOB pMd5Hash,
     OUT PCERT_TRUST_STATUS pStatus
     )
{
    PCCERT_CONTEXT pCertContext;

    memset( pStatus, 0, sizeof( CERT_TRUST_STATUS ) );

    // BUGBUG: PhilH needs to optimize this.
    pCertContext = CertFindCertificateInStore(
                       hRoot,
                       X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                       0,
                       CERT_FIND_MD5_HASH,
                       (LPVOID)pMd5Hash,
                       NULL
                       );

    if ( pCertContext == NULL )
    {
        pStatus->dwErrorStatus = CERT_TRUST_IS_UNTRUSTED_ROOT;
    }
    else
    {
        CertFreeCertificateContext( pCertContext );

        if ( hRoot == hRealRoot )
        {
            return;
        }

        pCertContext = CertFindCertificateInStore(
                           hRealRoot,
                           X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                           0,
                           CERT_FIND_MD5_HASH,
                           (LPVOID)pMd5Hash,
                           NULL
                           );

        if ( pCertContext == NULL )
        {
            pStatus->dwErrorStatus = CERT_TRUST_IS_UNTRUSTED_ROOT;
        }
        else
        {
            CertFreeCertificateContext( pCertContext );
        }
    }
}


//+---------------------------------------------------------------------------
//
//  Function:   ChainGetIssuerMatchStatus
//
//  Synopsis:   return status bits specifying what type of issuer matching
//              will be done for this certificate and if available return the
//              decoded authority key identifier extension
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainGetIssuerMatchStatus (
     IN PCCERT_CONTEXT pCertContext,
     OUT PCERT_AUTHORITY_KEY_ID_INFO* ppAuthKeyIdentifier,
     OUT PCERT_TRUST_STATUS pStatus
     )
{
    BOOL                         fResult = FALSE;
    PCERT_EXTENSION              pExt;
    PCERT_AUTHORITY_KEY_ID_INFO  pAuthKeyIdInfo = NULL;
    PCERT_AUTHORITY_KEY_ID2_INFO pAuthKeyIdInfo2 = NULL;
    DWORD                        cbData = 0;
    LPVOID                       pv = NULL;
    BOOL                         fV1AuthKeyIdInfo = TRUE;

    memset( pStatus, 0, sizeof( CERT_TRUST_STATUS ) );

    pExt = CertFindExtension(
               szOID_AUTHORITY_KEY_IDENTIFIER,
               pCertContext->pCertInfo->cExtension,
               pCertContext->pCertInfo->rgExtension
               );

    if ( pExt == NULL )
    {
        fV1AuthKeyIdInfo = FALSE;

        pExt = CertFindExtension(
                   szOID_AUTHORITY_KEY_IDENTIFIER2,
                   pCertContext->pCertInfo->cExtension,
                   pCertContext->pCertInfo->rgExtension
                   );
    }

    if ( pExt != NULL )
    {
        fResult = CryptDecodeObject(
                       X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                       pExt->pszObjId,
                       pExt->Value.pbData,
                       pExt->Value.cbData,
                       0,
                       NULL,
                       &cbData
                       );

        if ( fResult == TRUE )
        {
            pv = (LPVOID)new BYTE [ cbData ];
            if ( pv != NULL )
            {
                fResult = CryptDecodeObject(
                               X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                               pExt->pszObjId,
                               pExt->Value.pbData,
                               pExt->Value.cbData,
                               0,
                               pv,
                               &cbData
                               );
            }
            else
            {
                SetLastError( E_OUTOFMEMORY );
                fResult = FALSE;
            }
        }
    }

    if ( fResult == TRUE )
    {
        PCERT_AUTHORITY_KEY_ID_INFO pAuthKeyIdentifier = NULL;

        if ( fV1AuthKeyIdInfo == FALSE )
        {
            // NOTENOTE: Yes, this is a bit backwards but, right now but the
            //           V1 structure is a bit easier to deal with and we
            //           only support the V1 version of the V2 structure
            //           anyway
            fResult = ChainConvertAuthKeyIdentifierFromV2ToV1(
                           (PCERT_AUTHORITY_KEY_ID2_INFO)pv,
                           &pAuthKeyIdentifier
                           );

            delete (LPBYTE)pv;
        }
        else
        {
            pAuthKeyIdentifier = (PCERT_AUTHORITY_KEY_ID_INFO)pv;
        }

        if ( pAuthKeyIdentifier != NULL )
        {
            if ( ( pAuthKeyIdentifier->CertIssuer.cbData != 0 ) &&
                 ( pAuthKeyIdentifier->CertSerialNumber.cbData != 0 ) )
            {
                pStatus->dwInfoStatus |= CERT_TRUST_HAS_EXACT_MATCH_ISSUER;
                *ppAuthKeyIdentifier = pAuthKeyIdentifier;
            }
            else if ( pAuthKeyIdentifier->KeyId.cbData != 0 )
            {
                pStatus->dwInfoStatus |= CERT_TRUST_HAS_KEY_MATCH_ISSUER;
                *ppAuthKeyIdentifier = pAuthKeyIdentifier;
            }
            else
            {
                pStatus->dwInfoStatus |= CERT_TRUST_HAS_NAME_MATCH_ISSUER;

                delete (LPBYTE)pAuthKeyIdentifier;
                *ppAuthKeyIdentifier = NULL;
            }
        }
    }

    if ( fResult == FALSE )
    {
        pStatus->dwInfoStatus |= CERT_TRUST_HAS_NAME_MATCH_ISSUER;
        *ppAuthKeyIdentifier = NULL;
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFreeAuthorityKeyIdentifier
//
//  Synopsis:   free the authority key identifier
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFreeAuthorityKeyIdentifier (
     IN PCERT_AUTHORITY_KEY_ID_INFO pAuthKeyIdInfo
     )
{
    delete (LPBYTE)pAuthKeyIdInfo;
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainGetUsageStatus
//
//  Synopsis:   get the usage status
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainGetUsageStatus (
     IN PCERT_ENHKEY_USAGE pRequestedUsage,
     IN PCERT_ENHKEY_USAGE pAvailableUsage,
     IN DWORD dwMatchType,
     OUT PCERT_TRUST_STATUS pStatus
     )
{
    DWORD cRequested;
    DWORD cAvailable;
    DWORD cFound;
    BOOL  fFound;

    memset( pStatus, 0, sizeof( CERT_TRUST_STATUS ) );

    if ( pAvailableUsage == NULL )
    {
        return;
    }

    if ( ( pRequestedUsage->cUsageIdentifier >
           pAvailableUsage->cUsageIdentifier ) &&
         ( dwMatchType == USAGE_MATCH_TYPE_AND ) )
    {
        pStatus->dwErrorStatus |= CERT_TRUST_IS_NOT_VALID_FOR_USAGE;
        return;
    }

    for ( cRequested = 0, cFound = 0;
          cRequested < pRequestedUsage->cUsageIdentifier;
          cRequested++ )
    {
        for ( cAvailable = 0, fFound = FALSE;
              ( cAvailable < pAvailableUsage->cUsageIdentifier ) &&
              ( fFound == FALSE );
              cAvailable++ )
        {
            // NOTE: Optimize compares of OIDs.  Perhaps with a different
            //       encoding
            if ( strcmp(
                    pRequestedUsage->rgpszUsageIdentifier[ cRequested ],
                    pAvailableUsage->rgpszUsageIdentifier[ cAvailable ]
                    ) == 0 )
            {
                fFound = TRUE;
            }
        }

        if ( fFound == TRUE )
        {
            cFound += 1;
        }
    }

    if ( ( dwMatchType == USAGE_MATCH_TYPE_AND ) &&
         ( cFound != pRequestedUsage->cUsageIdentifier ) )
    {
        pStatus->dwErrorStatus |= CERT_TRUST_IS_NOT_VALID_FOR_USAGE;
    }
    else if ( ( dwMatchType == USAGE_MATCH_TYPE_OR ) &&
              ( cFound == 0 ) )
    {
        pStatus->dwErrorStatus |= CERT_TRUST_IS_NOT_VALID_FOR_USAGE;
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFindAndStoreMatchingIssuersFromCertStore
//
//  Synopsis:   search the given certificate store for issuers of the
//              certificate object.  Store the issuers in the certificate
//              object's issuer list and the certificate object cache as
//              necessary
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainFindAndStoreMatchingIssuersFromCertStore (
     IN PCCERTOBJECT pCertObject,
     IN PCCHAINCALLCONTEXT pCallContext,
     IN HCERTSTORE hStore,
     IN BOOL fAdditionalStore,
     IN BOOL fCacheIssuers
     )
{
    PCCERT_CONTEXT    pCertContext = NULL;
    PCCERTOBJECT      pIssuer = NULL;
    BYTE              CertificateHash[ MD5DIGESTLEN ];
    CRYPT_HASH_BLOB   HashBlob;
    PCCERTOBJECTCACHE pCertObjectCache;
    BOOL              fObjectInCache;
    BOOL              fMarkAsAdditionalStore;
    HCERTSTORE        hStoreToUse = NULL;
    HCERTSTORE        hCacheIssuersStore = NULL;
    HCERTSTORE        hIssuerListStore = NULL;
    BOOL              fFromCallCreationCache = FALSE;

    HashBlob.cbData = MD5DIGESTLEN;
    HashBlob.pbData = CertificateHash;

    pCertObjectCache = pCertObject->ChainEngine()->CertObjectCache();

    if ( fAdditionalStore == TRUE )
    {
        if ( ChainCreateCollectionIncludingCtlCertificates(
                  hStore,
                  &hStoreToUse
                  ) == FALSE )
        {
            hStoreToUse = CertDuplicateStore( hStore );
        }
    }
    else
    {
        hStoreToUse = CertDuplicateStore( hStore );
    }

    if ( ChainCreateIssuerMatchStores(
              pCertObject,
              hStoreToUse,
              fCacheIssuers,
              &hCacheIssuersStore,
              &hIssuerListStore
              ) == FALSE )
    {
        CertCloseStore( hStoreToUse, 0 );
        return( TRUE );
    }

    pCertObjectCache->DisableLruOfObjects();

    while ( ( pCertContext = CertEnumCertificatesInStore(
                                 hCacheIssuersStore,
                                 pCertContext
                                 ) ) != NULL )
    {
        if ( CertGetCertificateContextProperty(
                 pCertContext,
                 CERT_MD5_HASH_PROP_ID,
                 CertificateHash,
                 &HashBlob.cbData
                 ) == TRUE )
        {
            pIssuer = pCertObjectCache->FindObjectByHash( &HashBlob );
        }

        if ( pIssuer == NULL )
        {
            if ( ChainCreateCertificateObject(
                      pCertObject->ChainEngine(),
                      pCallContext,
                      pCertContext,
                      &HashBlob,
                      NULL,
                      ( fAdditionalStore == TRUE ) ? hStoreToUse : NULL,
                      &pIssuer,
                      &fFromCallCreationCache
                      ) == TRUE )
            {
                if ( fFromCallCreationCache == FALSE )
                {
                    pCertObjectCache->AddObject(
                                         pCallContext,
                                         pIssuer,
                                         FALSE
                                         );
                }

                pIssuer->Release();
            }
        }
        else
        {
            pIssuer->Release();
        }

        pIssuer = NULL;
    }

    assert( pIssuer == NULL );

    while ( ( pCertContext = CertEnumCertificatesInStore(
                                 hIssuerListStore,
                                 pCertContext
                                 ) ) != NULL )
    {
        if ( CertGetCertificateContextProperty(
                 pCertContext,
                 CERT_MD5_HASH_PROP_ID,
                 CertificateHash,
                 &HashBlob.cbData
                 ) == TRUE )
        {
            pIssuer = pCertObjectCache->FindObjectByHash( &HashBlob );
        }

        fObjectInCache = TRUE;
        fFromCallCreationCache = FALSE;

        if ( pIssuer == NULL )
        {
            fObjectInCache = FALSE;

            ChainCreateCertificateObject(
                 pCertObject->ChainEngine(),
                 pCallContext,
                 pCertContext,
                 &HashBlob,
                 NULL,
                 ( fAdditionalStore == TRUE ) ? hStoreToUse : NULL,
                 &pIssuer,
                 &fFromCallCreationCache
                 );
        }

        if ( pIssuer != NULL )
        {
            fMarkAsAdditionalStore = FALSE;

            if ( ( fAdditionalStore == TRUE ) && ( fObjectInCache == FALSE ) )
            {
                fMarkAsAdditionalStore = TRUE;
            }

            pCertObject->IssuerList()->AddIssuer(
                                          pCallContext,
                                          pIssuer,
                                          fMarkAsAdditionalStore,
                                          fFromCallCreationCache
                                          );

            pIssuer->Release();
            pIssuer = NULL;
        }
    }

    pCertObjectCache->EnableLruOfObjects( pCallContext );

    CertCloseStore( hStoreToUse, 0 );
    CertCloseStore( hCacheIssuersStore, 0 );
    CertCloseStore( hIssuerListStore, 0 );

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainCreateIssuerMatchStores
//
//  Synopsis:   create stores that contain all related issuers as well as the
//              matching issuers for the cert object's issuer list
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainCreateIssuerMatchStores (
     IN PCCERTOBJECT pCertObject,
     IN HCERTSTORE hStore,
     IN BOOL fCacheIssuers,
     OUT HCERTSTORE* phCacheIssuersStore,
     OUT HCERTSTORE* phIssuerListStore
     )
{
    HCERTSTORE      hCacheIssuersStore = NULL;
    HCERTSTORE      hIssuerListStore = NULL;
    DWORD           dwInfoStatus;
    PCCERT_CONTEXT  pCertContext = NULL;
    PCCERT_CONTEXT  pSampleIssuer = NULL;
    DWORD           cbKeyIdentifier = 0;
    LPBYTE          pbKeyIdentifier = NULL;
    CRYPT_HASH_BLOB HashBlob;

    dwInfoStatus = pCertObject->InfoStatus();

    hCacheIssuersStore = CertOpenStore(
                             CERT_STORE_PROV_MEMORY,
                             X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                             NULL,
                             CERT_STORE_DEFER_CLOSE_UNTIL_LAST_FREE_FLAG,
                             NULL
                             );

    if ( hCacheIssuersStore == NULL )
    {
        return( FALSE );
    }

    hIssuerListStore = CertOpenStore(
                           CERT_STORE_PROV_MEMORY,
                           X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                           NULL,
                           CERT_STORE_DEFER_CLOSE_UNTIL_LAST_FREE_FLAG,
                           NULL
                           );

    if ( hIssuerListStore == NULL )
    {
        CertCloseStore( hCacheIssuersStore, 0 );
        return( FALSE );
    }

    while ( ( pCertContext = CertEnumCertificatesInStore(
                                 hStore,
                                 pCertContext
                                 ) ) != NULL )
    {
        if ( ChainIsMatchingIssuerCertificate(
                  pCertObject,
                  pCertContext
                  ) == TRUE )
        {
            CertAddCertificateLinkToStore(
                hIssuerListStore,
                pCertContext,
                CERT_STORE_ADD_NEW,
                NULL
                );

            if ( fCacheIssuers == TRUE )
            {
                CertAddCertificateLinkToStore(
                    hCacheIssuersStore,
                    pCertContext,
                    CERT_STORE_ADD_NEW,
                    NULL
                    );
            }
        }
    }

    assert( pCertContext == NULL );
    assert( pSampleIssuer == NULL );

    if ( fCacheIssuers == TRUE )
    {
        pSampleIssuer = CertEnumCertificatesInStore( hIssuerListStore, NULL );
    }

    if ( pSampleIssuer != NULL )
    {
        if ( !( dwInfoStatus & CERT_TRUST_HAS_NAME_MATCH_ISSUER ) )
        {
            while ( ( pCertContext = CertFindCertificateInStore(
                                         hStore,
                                         X509_ASN_ENCODING |
                                         PKCS_7_ASN_ENCODING,
                                         0,
                                         CERT_FIND_SUBJECT_NAME,
                                         &pSampleIssuer->pCertInfo->Subject,
                                         pCertContext
                                         ) ) != NULL )
            {
                CertAddCertificateLinkToStore(
                    hCacheIssuersStore,
                    pCertContext,
                    CERT_STORE_ADD_NEW,
                    NULL
                    );
            }
        }

        if ( !( dwInfoStatus & CERT_TRUST_HAS_KEY_MATCH_ISSUER ) )
        {
            if ( CertGetCertificateContextProperty(
                     pSampleIssuer,
                     CERT_KEY_IDENTIFIER_PROP_ID,
                     NULL,
                     &cbKeyIdentifier
                     ) == TRUE )
            {
                pbKeyIdentifier = new BYTE [ cbKeyIdentifier ];
                if ( pbKeyIdentifier != NULL )
                {
                    if ( CertGetCertificateContextProperty(
                             pSampleIssuer,
                             CERT_KEY_IDENTIFIER_PROP_ID,
                             NULL,
                             &cbKeyIdentifier
                             ) == TRUE )
                    {
                        HashBlob.cbData = cbKeyIdentifier;
                        HashBlob.pbData = pbKeyIdentifier;

                        while ( ( pCertContext = CertFindCertificateInStore(
                                                     hStore,
                                                     X509_ASN_ENCODING |
                                                     PKCS_7_ASN_ENCODING,
                                                     0,
                                                     CERT_FIND_KEY_IDENTIFIER,
                                                     &HashBlob,
                                                     pCertContext
                                                     ) ) != NULL )
                        {
                            CertAddCertificateLinkToStore(
                                hCacheIssuersStore,
                                pCertContext,
                                CERT_STORE_ADD_NEW,
                                NULL
                                );
                        }
                    }

                    delete pbKeyIdentifier;
                }
            }
        }

        if ( !( dwInfoStatus & CERT_TRUST_HAS_EXACT_MATCH_ISSUER ) )
        {
            while ( ( pCertContext = CertFindCertificateInStore(
                                         hStore,
                                         X509_ASN_ENCODING |
                                         PKCS_7_ASN_ENCODING,
                                         0,
                                         CERT_FIND_ISSUER_NAME,
                                         &pSampleIssuer->pCertInfo->Issuer,
                                         pCertContext
                                         ) ) != NULL )
            {
                if ( ( pSampleIssuer->pCertInfo->SerialNumber.cbData ==
                       pCertContext->pCertInfo->SerialNumber.cbData ) &&
                     ( memcmp(
                          pSampleIssuer->pCertInfo->SerialNumber.pbData,
                          pCertContext->pCertInfo->SerialNumber.pbData,
                          pCertContext->pCertInfo->SerialNumber.cbData
                          ) == 0 ) )
                {
                    CertAddCertificateLinkToStore(
                        hCacheIssuersStore,
                        pCertContext,
                        CERT_STORE_ADD_NEW,
                        NULL
                        );
                }
            }
        }

        CertFreeCertificateContext( pSampleIssuer );
    }

    *phCacheIssuersStore = hCacheIssuersStore;
    *phIssuerListStore = hIssuerListStore;

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainIsMatchingIssuerCertificate
//
//  Synopsis:   is this a matching issuer certificate for the given subject
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainIsMatchingIssuerCertificate (
     IN PCCERTOBJECT pSubject,
     IN PCCERT_CONTEXT pIssuer
     )
{
    BOOL       fResult;
    PCERT_INFO pSubjectInfo = pSubject->CertContext()->pCertInfo;
    PCERT_INFO pIssuerInfo = pIssuer->pCertInfo;
    DWORD      cbSubjectAuthKeyIdentifier = 0;
    LPBYTE     pbSubjectAuthKeyIdentifier = NULL;
    DWORD      cbIssuerKeyIdentifier = 0;
    LPBYTE     pbIssuerKeyIdentifier = NULL;
    DWORD      cbSubjectAuthIssuerName = 0;
    LPBYTE     pbSubjectAuthIssuerName = NULL;
    DWORD      cbSubjectAuthSerialNumber = 0;
    LPBYTE     pbSubjectAuthSerialNumber = NULL;
    DWORD      dwMatchScheme;

    dwMatchScheme = pSubject->InfoStatus();

    if ( dwMatchScheme & CERT_TRUST_HAS_NAME_MATCH_ISSUER )
    {
        if ( ( pSubjectInfo->Issuer.cbData == pIssuerInfo->Subject.cbData ) &&
             ( memcmp(
                  pSubjectInfo->Issuer.pbData,
                  pIssuerInfo->Subject.pbData,
                  pIssuerInfo->Subject.cbData
                  ) == 0 ) )
        {
            return( TRUE );
        }
        else
        {
            return( FALSE );
        }
    }

    if ( dwMatchScheme & CERT_TRUST_HAS_KEY_MATCH_ISSUER )
    {
        cbSubjectAuthKeyIdentifier = pSubject->AuthorityKeyIdentifier()->KeyId.cbData;
        pbSubjectAuthKeyIdentifier = pSubject->AuthorityKeyIdentifier()->KeyId.pbData;

        if ( CertGetCertificateContextProperty(
                 pIssuer,
                 CERT_KEY_IDENTIFIER_PROP_ID,
                 NULL,
                 &cbIssuerKeyIdentifier
                 ) == FALSE )
        {
            return( FALSE );
        }

        if ( cbSubjectAuthKeyIdentifier != cbIssuerKeyIdentifier )
        {
            return( FALSE );
        }

        pbIssuerKeyIdentifier = new BYTE [ cbIssuerKeyIdentifier ];
        if ( pbIssuerKeyIdentifier != NULL )
        {
            fResult = CertGetCertificateContextProperty(
                          pIssuer,
                          CERT_KEY_IDENTIFIER_PROP_ID,
                          pbIssuerKeyIdentifier,
                          &cbIssuerKeyIdentifier
                          );

            if ( ( fResult == TRUE ) &&
                 ( memcmp(
                      pbIssuerKeyIdentifier,
                      pbSubjectAuthKeyIdentifier,
                      cbSubjectAuthKeyIdentifier
                      ) != 0 ) )
            {
                fResult = FALSE;
            }

            delete pbIssuerKeyIdentifier;

            return( fResult );
        }
        else
        {
            return( FALSE );
        }
    }

    if ( dwMatchScheme & CERT_TRUST_HAS_EXACT_MATCH_ISSUER )
    {
        cbSubjectAuthIssuerName = pSubject->AuthorityKeyIdentifier()->CertIssuer.cbData;
        pbSubjectAuthIssuerName = pSubject->AuthorityKeyIdentifier()->CertIssuer.pbData;
        cbSubjectAuthSerialNumber = pSubject->AuthorityKeyIdentifier()->CertSerialNumber.cbData;
        pbSubjectAuthSerialNumber = pSubject->AuthorityKeyIdentifier()->CertSerialNumber.pbData;

        if ( ( cbSubjectAuthIssuerName == pIssuerInfo->Issuer.cbData ) &&
             ( memcmp(
                  pbSubjectAuthIssuerName,
                  pIssuerInfo->Issuer.pbData,
                  pIssuerInfo->Issuer.cbData
                  ) == 0 ) &&
             ( cbSubjectAuthSerialNumber == pIssuerInfo->SerialNumber.cbData ) &&
             ( memcmp(
                  pbSubjectAuthSerialNumber,
                  pIssuerInfo->SerialNumber.pbData,
                  pIssuerInfo->SerialNumber.cbData
                  ) == 0 ) )
        {
            return( TRUE );
        }
        else
        {
            return( FALSE );
        }
    }

    assert( FALSE && "Should not get here!" );

    return( FALSE );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainIsInSameIssuerGrouping
//
//  Synopsis:   an issuer group comprises all issuing certificates that would
//              match some subject certificate by name, exact or key match.
//              This means that given a group member, a possible group member
//              is a match if it either has the same subject name, same issuer
//              name and serial number or same key identifier
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainIsInSameIssuerGrouping (
     IN PCCERTOBJECT pGroupMember,
     IN PCCERT_CONTEXT pPossibleGroupMember
     )
{
    DWORD          cbKeyIdentifier;
    LPBYTE         pbKeyIdentifier;
    PCCERT_CONTEXT pGroupContext;

    pGroupContext = pGroupMember->CertContext();

    if ( ( pGroupContext->pCertInfo->Subject.cbData ==
           pPossibleGroupMember->pCertInfo->Subject.cbData ) &&
         ( memcmp(
              pGroupContext->pCertInfo->Subject.pbData,
              pPossibleGroupMember->pCertInfo->Subject.pbData,
              pPossibleGroupMember->pCertInfo->Subject.cbData
              ) == 0 ) )
    {
        return( TRUE );
    }

    if ( ( pGroupContext->pCertInfo->Issuer.cbData ==
           pPossibleGroupMember->pCertInfo->Issuer.cbData ) &&
         ( memcmp(
              pGroupContext->pCertInfo->Issuer.pbData,
              pPossibleGroupMember->pCertInfo->Issuer.pbData,
              pPossibleGroupMember->pCertInfo->Issuer.cbData
              ) == 0 ) &&
         ( pGroupContext->pCertInfo->SerialNumber.cbData ==
           pPossibleGroupMember->pCertInfo->SerialNumber.cbData ) &&
         ( memcmp(
              pGroupContext->pCertInfo->SerialNumber.pbData,
              pPossibleGroupMember->pCertInfo->SerialNumber.pbData,
              pPossibleGroupMember->pCertInfo->SerialNumber.cbData
              ) == 0 ) )
    {
        return( TRUE );
    }

    if ( CertGetCertificateContextProperty(
             pPossibleGroupMember,
             CERT_KEY_IDENTIFIER_PROP_ID,
             NULL,
             &cbKeyIdentifier
             ) == TRUE )
    {
        if ( cbKeyIdentifier != pGroupMember->KeyIdentifierSize() )
        {
            return( FALSE );
        }

        pbKeyIdentifier = new BYTE [ cbKeyIdentifier ];
        if ( pbKeyIdentifier != NULL )
        {
            if ( CertGetCertificateContextProperty(
                     pPossibleGroupMember,
                     CERT_KEY_IDENTIFIER_PROP_ID,
                     pbKeyIdentifier,
                     &cbKeyIdentifier
                     ) == TRUE )
            {
                if ( memcmp(
                        pGroupMember->KeyIdentifier(),
                        pbKeyIdentifier,
                        cbKeyIdentifier
                        ) == 0 )
                {
                    delete pbKeyIdentifier;
                    return( TRUE );
                }
            }

            delete pbKeyIdentifier;
        }
    }

    return( FALSE );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainCreateCertificateObject
//
//  Synopsis:   create a certificate object, note since it is a ref-counted
//              object, freeing occurs by doing a pCertObject->Release
//
//----------------------------------------------------------------------------
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
     )
{
    BOOL         fResult = TRUE;
    PCCERTOBJECT pCertObject;

    pCertObject = pCallContext->FindObjectInCreationCache( pHashBlob );
    if ( pCertObject != NULL )
    {
        *ppCertObject = pCertObject;
        *pfFromCallCreationCache = TRUE;
        return( TRUE );
    }

    pCertObject = new CCertObject(
                           pChainEngine,
                           pCallContext,
                           pCertContext,
                           pHashBlob,
                           pKnownStatus,
                           hAdditionalStore,
                           fResult
                           );

    if ( pCertObject != NULL )
    {
        if ( fResult == TRUE )
        {
            *ppCertObject = pCertObject;
            *pfFromCallCreationCache = FALSE;
        }
        else
        {
            pCertObject->Release();
        }
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainGetIssuerQuality
//
//  Synopsis:   get the quality of the given issuer
//
//----------------------------------------------------------------------------
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
     )
{
    LONG               lQuality = 0;
    PCCERTOBJECT       pIssuer;
    CERT_TRUST_STATUS  Status;
    PCERT_TRUST_STATUS pSubjectStatus;
    DWORD              dwIssuerChainErrorStatus;

    memset( pAdditionalStatus, 0, sizeof( CERT_TRUST_STATUS ) );
    memset( pCtlStatus, 0, sizeof( CERT_TRUST_STATUS ) );
    memset( pCtlSignerRequestedUsage, 0, sizeof( CERT_USAGE_MATCH ) );
    memset( pCtlSignerRequestedTime, 0, sizeof( FILETIME ) );

    if ( pElement->pIssuer == NULL )
    {
        return( 0 );
    }

    if ( pElement->fCtlIssuer == TRUE )
    {
#ifdef CAPI_INCLUDE_CTL    
        if ( pElement->pCtlIssuerData->pSSCtlObject->GetSignerRequestedUsageAndTime(
                                             pCurrentTime,
                                             pCtlSignerRequestedUsage,
                                             pCtlSignerRequestedTime
                                             ) == FALSE )
#endif                                             
        {
            // NOTE: This causes a skip
            return( CERT_QUALITY_MINIMUM );
        }
    }

    pIssuer = pElement->pIssuer;
    Status.dwErrorStatus = pIssuer->ErrorStatus();
    Status.dwInfoStatus = pIssuer->InfoStatus();
    dwIssuerChainErrorStatus = pIssuer->ChainErrorStatus();

    pSubjectStatus = &pElement->SubjectStatus;

    pIssuer->CalculateAdditionalStatus(
                      ( pElement->fCtlIssuer == FALSE ) ? pTime :
                      pCtlSignerRequestedTime,
                      ( pElement->fCtlIssuer == FALSE ) ? pRequestedUsage :
                      pCtlSignerRequestedUsage,
                      dwFlags,
                      pAdditionalStatus
                      );

    ChainOrInStatusBits( &Status, pAdditionalStatus );

    // BUGBUG: Need to take list identifier into consideration

#ifdef CAPI_INCLUDE_CTL
    if ( pElement->fCtlIssuer == TRUE )
    {
        pElement->pCtlIssuerData->pSSCtlObject->CalculateStatus(
                                                         pCurrentTime,
                                                         pRequestedUsage,
                                                         dwFlags,
                                                         pCtlStatus
                                                         );
    }
#endif    

    if ( !( pSubjectStatus->dwErrorStatus & CERT_TRUST_IS_NOT_TIME_NESTED ) )
    {
        lQuality += CERT_QUALITY_ISSUER_TIME_NESTED;
    }

    if ( !( dwIssuerChainErrorStatus & CERT_TRUST_IS_NOT_TIME_NESTED ) )
    {
        lQuality += CERT_QUALITY_CHAIN_TIME_NESTED;
    }

    if ( !( Status.dwErrorStatus & CERT_TRUST_IS_NOT_TIME_VALID ) &&
         !( pCtlStatus->dwErrorStatus & CERT_TRUST_CTL_IS_NOT_TIME_VALID ) )
    {
        lQuality += CERT_QUALITY_ISSUER_TIME_VALID;
    }

    if ( !( Status.dwErrorStatus & CERT_TRUST_IS_NOT_VALID_FOR_USAGE ) &&
         !( pCtlStatus->dwErrorStatus & CERT_TRUST_CTL_IS_NOT_VALID_FOR_USAGE ) )
    {
        lQuality += CERT_QUALITY_MEETS_USAGE_CRITERIA;
    }

    if ( !( dwIssuerChainErrorStatus & CERT_TRUST_IS_UNTRUSTED_ROOT ) )
    {
        lQuality += CERT_QUALITY_HAS_PROTECTED_ROOT;
    }

    if ( !( pSubjectStatus->dwErrorStatus & CERT_TRUST_IS_NOT_SIGNATURE_VALID ) &&
         !( pCtlStatus->dwErrorStatus & CERT_TRUST_CTL_IS_NOT_SIGNATURE_VALID ) )
    {
        lQuality += CERT_QUALITY_ISSUER_SIGNATURE_VALID;
    }

    if ( !( dwIssuerChainErrorStatus & CERT_TRUST_IS_NOT_SIGNATURE_VALID ) )
    {
        lQuality += CERT_QUALITY_CHAIN_SIGNATURE_VALID;
    }

    return( lQuality );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainGetSubjectStatus
//
//  Synopsis:   get the subject status bits by checking the time nesting and
//              signature validity
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainGetSubjectStatus (
     IN PCCERTOBJECT pIssuer,
     IN OPTIONAL PCERT_PUBLIC_KEY_INFO pIssuerPublicKey,
     IN PCCERTOBJECT pSubject,
     OUT PCERT_TRUST_STATUS pStatus
     )
{
    memset( pStatus, 0, sizeof( CERT_TRUST_STATUS ) );

    if ( ( CompareFileTime(
                  &pIssuer->CertContext()->pCertInfo->NotBefore,
                  &pSubject->CertContext()->pCertInfo->NotBefore
                  ) > 0 ) ||
         ( CompareFileTime(
                  &pIssuer->CertContext()->pCertInfo->NotAfter,
                  &pSubject->CertContext()->pCertInfo->NotAfter
                  ) < 0 ) )
    {
        pStatus->dwErrorStatus |= CERT_TRUST_IS_NOT_TIME_NESTED;
    }

    if ( pIssuerPublicKey == NULL )
    {
#ifdef CMS_PKCS7
        if (!CryptVerifyCertificateSignatureEx(
                NULL,                   // hCryptProv
                X509_ASN_ENCODING,
                CRYPT_VERIFY_CERT_SIGN_SUBJECT_CERT,
                (void *) pSubject->CertContext(),
                CRYPT_VERIFY_CERT_SIGN_ISSUER_CERT,
                (void *) pIssuer->CertContext(),
                0,                      // dwFlags
                NULL                    // pvReserved
                ))
#else
        if ( CryptVerifyCertificateSignature(
                  NULL,
                  X509_ASN_ENCODING,
                  pSubject->CertContext()->pbCertEncoded,
                  pSubject->CertContext()->cbCertEncoded,
                  &pIssuer->CertContext()->pCertInfo->SubjectPublicKeyInfo
                  ) == FALSE )
#endif
        {
            pStatus->dwErrorStatus |= CERT_TRUST_IS_NOT_SIGNATURE_VALID;
        }
    }
    else
    {
        if ( CertComparePublicKeyInfo(
                 X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                 &pIssuer->CertContext()->pCertInfo->SubjectPublicKeyInfo,
                 pIssuerPublicKey
                 ) == FALSE )
        {
            pStatus->dwErrorStatus |= CERT_TRUST_IS_NOT_SIGNATURE_VALID;
        }
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainTouchAllChosenIssuerRelatedObjects
//
//  Synopsis:   touch the chosen issuer and all its buddies
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainTouchAllChosenIssuerRelatedObjects (
     IN PCCERTOBJECT pChosenIssuer
     )
{
    CRYPT_HASH_BLOB        HashBlob;
    CERT_OBJECT_IDENTIFIER ObjectIdentifier;

    I_CryptTouchLruEntry( pChosenIssuer->HashIndexEntry(), 0 );

    ChainTouchAllMatchingSubjectNames(
         pChosenIssuer->ChainEngine()->CertObjectCache(),
         &pChosenIssuer->CertContext()->pCertInfo->Subject,
         LRU_SUPPRESS_CLOCK_UPDATE
         );

    pChosenIssuer->GetObjectIdentifier( ObjectIdentifier );

    ChainTouchAllMatchingObjectIds(
         pChosenIssuer->ChainEngine()->CertObjectCache(),
         ObjectIdentifier,
         LRU_SUPPRESS_CLOCK_UPDATE
         );

    HashBlob.cbData = pChosenIssuer->KeyIdentifierSize();
    HashBlob.pbData = pChosenIssuer->KeyIdentifier();

    ChainTouchAllMatchingKeyIds(
         pChosenIssuer->ChainEngine()->CertObjectCache(),
         &HashBlob,
         LRU_SUPPRESS_CLOCK_UPDATE
         );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainTouchAllMatchingSubjectNames
//
//  Synopsis:   touch all matching subject names
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainTouchAllMatchingSubjectNames (
     IN PCCERTOBJECTCACHE pCertObjectCache,
     IN PCERT_NAME_BLOB pSubjectName,
     IN DWORD dwTouchFlags
     )
{
    ChainTouchAllMatchingEntries(
         pCertObjectCache->SubjectNameIndex(),
         pSubjectName,
         dwTouchFlags
         );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainTouchAllMatchingObjectIds
//
//  Synopsis:   touch all matching object ids
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainTouchAllMatchingObjectIds (
     IN PCCERTOBJECTCACHE pCertObjectCache,
     IN CERT_OBJECT_IDENTIFIER ObjectIdentifier,
     IN DWORD dwTouchFlags
     )
{
    CRYPT_DATA_BLOB DataBlob;

    DataBlob.cbData = MD5DIGESTLEN;
    DataBlob.pbData = ObjectIdentifier;

    ChainTouchAllMatchingEntries(
         pCertObjectCache->IdentifierIndex(),
         &DataBlob,
         dwTouchFlags
         );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainTouchAllMatchingKeyIds
//
//  Synopsis:   touch all matching key id objects
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainTouchAllMatchingKeyIds (
     IN PCCERTOBJECTCACHE pCertObjectCache,
     IN PCRYPT_HASH_BLOB pKeyId,
     IN DWORD dwTouchFlags
     )
{
    ChainTouchAllMatchingEntries(
         pCertObjectCache->KeyIdIndex(),
         pKeyId,
         dwTouchFlags
         );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainTouchAllMatchingEntries
//
//  Synopsis:   touch all matching entries
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainTouchAllMatchingEntries (
     IN HLRUCACHE hIndex,
     IN PCRYPT_DATA_BLOB pIdentifier,
     IN DWORD dwTouchFlags
     )
{
    HLRUENTRY    hEntry;
    PCCERTOBJECT pCertObject;

    hEntry = I_CryptFindLruEntry( hIndex, pIdentifier );

    if ( hEntry != NULL )
    {
        pCertObject = (PCCERTOBJECT)I_CryptGetLruEntryData( hEntry );

        I_CryptTouchLruEntry( pCertObject->HashIndexEntry(), dwTouchFlags );

        while ( ( hEntry = I_CryptEnumMatchingLruEntries( hEntry ) ) != NULL )
        {
            pCertObject = (PCCERTOBJECT)I_CryptGetLruEntryData( hEntry );

            I_CryptTouchLruEntry( pCertObject->HashIndexEntry(), dwTouchFlags );
        }
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainRemoveCertificateObjectFromSecondaryCacheIndexes
//
//  Synopsis:   removes certificate object from the identifier, subject name
//              and issuer name indexes in the certificate object cache
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainRemoveCertificateObjectFromSecondaryCacheIndexes (
     IN PCCERTOBJECT pCertObject
     )
{
    I_CryptRemoveLruEntry(
           pCertObject->IdentifierIndexEntry(),
           LRU_SUPPRESS_REMOVAL_NOTIFICATION,
           NULL
           );

    I_CryptRemoveLruEntry(
           pCertObject->SubjectNameIndexEntry(),
           LRU_SUPPRESS_REMOVAL_NOTIFICATION,
           NULL
           );

    I_CryptRemoveLruEntry(
           pCertObject->IssuerNameIndexEntry(),
           LRU_SUPPRESS_REMOVAL_NOTIFICATION,
           NULL
           );

    I_CryptRemoveLruEntry(
           pCertObject->KeyIdIndexEntry(),
           LRU_SUPPRESS_REMOVAL_NOTIFICATION,
           NULL
           );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainRemoveObjectsRelatedToThisIssuer
//
//  Synopsis:   remove objects related to this issuer
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainRemoveObjectsRelatedToThisIssuer (
     IN PCCERTOBJECT pRemovedIssuer
     )
{
    CRYPT_HASH_BLOB        HashBlob;
    CERT_OBJECT_IDENTIFIER ObjectIdentifier;

    ChainRemoveAllMatchingSubjectNames(
         pRemovedIssuer->ChainEngine()->CertObjectCache(),
         &pRemovedIssuer->CertContext()->pCertInfo->Subject,
         LRU_SUPPRESS_REMOVAL_NOTIFICATION
         );

    pRemovedIssuer->GetObjectIdentifier( ObjectIdentifier );

    ChainRemoveAllMatchingObjectIds(
         pRemovedIssuer->ChainEngine()->CertObjectCache(),
         ObjectIdentifier,
         LRU_SUPPRESS_REMOVAL_NOTIFICATION
         );

    HashBlob.cbData = pRemovedIssuer->KeyIdentifierSize();
    HashBlob.pbData = pRemovedIssuer->KeyIdentifier();

    ChainRemoveAllMatchingKeyIds(
         pRemovedIssuer->ChainEngine()->CertObjectCache(),
         &HashBlob,
         LRU_SUPPRESS_REMOVAL_NOTIFICATION
         );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainRemoveAllMatchingSubjectNames
//
//  Synopsis:   remove all matching subject name entries
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainRemoveAllMatchingSubjectNames (
     IN PCCERTOBJECTCACHE pCertObjectCache,
     IN PCERT_NAME_BLOB pSubjectName,
     IN DWORD dwRemovalFlags
     )
{
    ChainRemoveAllMatchingEntries(
         pCertObjectCache->SubjectNameIndex(),
         pSubjectName,
         dwRemovalFlags
         );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainRemoveAllMatchingObjectIds
//
//  Synopsis:   remove all matching object identifier entries
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainRemoveAllMatchingObjectIds (
     IN PCCERTOBJECTCACHE pCertObjectCache,
     IN CERT_OBJECT_IDENTIFIER ObjectIdentifier,
     IN DWORD dwRemovalFlags
     )
{
    CRYPT_DATA_BLOB DataBlob;

    DataBlob.cbData = MD5DIGESTLEN;
    DataBlob.pbData = ObjectIdentifier;

    ChainRemoveAllMatchingEntries(
         pCertObjectCache->IdentifierIndex(),
         &DataBlob,
         dwRemovalFlags
         );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainRemoveAllMatchingKeyIds
//
//  Synopsis:   remove matching key identifier entries
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainRemoveAllMatchingKeyIds (
     IN PCCERTOBJECTCACHE pCertObjectCache,
     IN PCRYPT_HASH_BLOB pKeyId,
     IN DWORD dwRemovalFlags
     )
{
    ChainRemoveAllMatchingEntries(
         pCertObjectCache->KeyIdIndex(),
         pKeyId,
         dwRemovalFlags
         );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainRemoveAllMatchingEntries
//
//  Synopsis:   remove matching index entries by removing them from the
//              appropriate primary index
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainRemoveAllMatchingEntries (
     IN HLRUCACHE hIndex,
     IN PCRYPT_DATA_BLOB pIdentifier,
     IN DWORD dwRemovalFlags
     )
{
    HLRUENTRY    hRemove;
    HLRUENTRY    hEntry;
    PCCERTOBJECT pCertObject;

    hEntry = I_CryptFindLruEntry( hIndex, pIdentifier );

    while ( hEntry != NULL )
    {
        I_CryptAddRefLruEntry( ( hRemove = hEntry ) );

        hEntry = I_CryptEnumMatchingLruEntries( hEntry );
        pCertObject = (PCCERTOBJECT)I_CryptGetLruEntryData( hRemove );

        I_CryptRemoveLruEntry(
               pCertObject->HashIndexEntry(),
               dwRemovalFlags,
               NULL
               );

        ChainProcessRemovalNotification( NULL, pCertObject, FALSE );

        I_CryptReleaseLruEntry( hRemove );
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFlushIssuerListsWithThisIssuer
//
//  Synopsis:   flush issuer lists with this issuer
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFlushIssuerListsWithThisIssuer (
     IN PCCERTOBJECT pIssuer
     )
{
    ChainFlushIssuerListsWithThisIssuerByName( pIssuer );
    ChainFlushIssuerListsWithThisIssuerByObjectOrKeyId( pIssuer );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFlushIssuerListsWithThisIssuerByName
//
//  Synopsis:   flush issuer lists with this issuer by name
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFlushIssuerListsWithThisIssuerByName (
     IN PCCERTOBJECT pIssuer
     )
{
    HLRUENTRY    hEntry;
    PCCERTOBJECT pCertObject;

    hEntry = I_CryptFindLruEntry(
                    pIssuer->ChainEngine()->CertObjectCache()->IssuerNameIndex(),
                    &pIssuer->CertContext()->pCertInfo->Subject
                    );

    while ( hEntry != NULL )
    {
        pCertObject = (PCCERTOBJECT)I_CryptGetLruEntryData( hEntry );
        pCertObject->IssuerList()->Flush();

        hEntry = I_CryptEnumMatchingLruEntries( hEntry );
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFlushIssuerListsWithThisIssuerByObjectOrKeyId
//
//  Synopsis:   flush issuer lists
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFlushIssuerListsWithThisIssuerByObjectOrKeyId (
     IN PCCERTOBJECT pIssuer
     )
{
    pIssuer->ChainEngine()->CertObjectCache()->EnumObjects(
                  ChainFlushIssuerListsWithThisIssuerByObjectOrKeyIdEnumFn,
                  pIssuer
                  );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFlushIssuerListsWithThisIssuerByObjectOrKeyIdEnumFn
//
//  Synopsis:   check to see if this issuer ( pvParameter ) is an exact
//              match or key match for the cert object and if so, flush
//              it's issuer list
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainFlushIssuerListsWithThisIssuerByObjectOrKeyIdEnumFn (
     IN LPVOID pvParameter,
     IN PCCERTOBJECT pCertObject
     )
{
    PCCERTOBJECT pIssuer = (PCCERTOBJECT)pvParameter;

    if ( pCertObject->InfoStatus() & CERT_TRUST_HAS_NAME_MATCH_ISSUER )
    {
        return( TRUE );
    }

    if ( ChainIsMatchingIssuerCertificate(
              pCertObject,
              pIssuer->CertContext()
              ) == TRUE )
    {
        pCertObject->IssuerList()->Flush();
    }

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFlushMatchingCtlIssuerLists
//
//  Synopsis:   flush matching lists
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFlushMatchingCtlIssuerLists (
     IN PCCERTOBJECT pCertObject
     )
{
    FLUSH_CTL_ISSUERS_ENUM_DATA EnumData;

    if ( ChainIsCertificateObjectCtlSigner( pCertObject ) == FALSE )
    {
        return;
    }

    EnumData.pCtlIssuer = pCertObject;

    pCertObject->ChainEngine()->CertObjectCache()->EnumObjects(
                                    ChainFlushMatchingCtlIssuerListsEnumFn,
                                    &EnumData
                                    );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainIsCertificateObjectCtlSigner
//
//  Synopsis:   does the certificate have the CTL signer EKU
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainIsCertificateObjectCtlSigner (
     IN PCCERTOBJECT pCertObject
     )
{
    CERT_TRUST_STATUS TrustStatus;
    LPSTR             pszUsage = szOID_KP_CTL_USAGE_SIGNING;
    CERT_ENHKEY_USAGE Usage;

    Usage.cUsageIdentifier = 1;
    Usage.rgpszUsageIdentifier = &pszUsage;

    ChainGetUsageStatus(
         &Usage,
         pCertObject->Usage(),
         0,
         &TrustStatus
         );

    if ( TrustStatus.dwErrorStatus & CERT_TRUST_IS_NOT_VALID_FOR_USAGE )
    {
        return( FALSE );
    }

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFlushMatchingCtlIssuerListsEnumFn
//
//  Synopsis:   flush matching CTL issuer list enumerator function
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainFlushMatchingCtlIssuerListsEnumFn (
     IN LPVOID pvParameter,
     IN PCCERTOBJECT pCertObject
     )
{
    PFLUSH_CTL_ISSUERS_ENUM_DATA pEnumData;

    if ( !( pCertObject->InfoStatus() & CERT_TRUST_IS_SELF_SIGNED ) )
    {
        return( TRUE );
    }

    pEnumData = (PFLUSH_CTL_ISSUERS_ENUM_DATA)pvParameter;
    pCertObject->IssuerList()->CtlIssuerFlush( pEnumData->pCtlIssuer );

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainProcessRemovalNotification
//
//  Synopsis:   process removal notification
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainProcessRemovalNotification (
     IN PCCHAINCALLCONTEXT pCallContext,
     IN PCCERTOBJECT pCertObject,
     IN BOOL fProcessMatchingObjects
     )
{

    ChainRemoveCertificateObjectFromSecondaryCacheIndexes( pCertObject );

    if ( fProcessMatchingObjects == TRUE )
    {
        ChainFlushIssuerListsWithThisIssuer( pCertObject );
        ChainFlushMatchingCtlIssuerLists( pCertObject );
        ChainRemoveObjectsRelatedToThisIssuer( pCertObject );
    }

    pCertObject->Release();
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainCreateCertificateObjectCache
//
//  Synopsis:   create certificate object cache object
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainCreateCertificateObjectCache (
     IN DWORD MaxIndexEntries,
     OUT PCCERTOBJECTCACHE* ppCertObjectCache
     )
{
    BOOL              fResult = FALSE;
    PCCERTOBJECTCACHE pCertObjectCache = NULL;

    pCertObjectCache = new CCertObjectCache( MaxIndexEntries, fResult );
    if ( pCertObjectCache != NULL )
    {
        if ( fResult == TRUE )
        {
            *ppCertObjectCache = pCertObjectCache;
        }
        else
        {
            delete pCertObjectCache;
        }
    }
    else
    {
        SetLastError( E_OUTOFMEMORY );
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFreeCertificateObjectCache
//
//  Synopsis:   free the certificate object cache object
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFreeCertificateObjectCache (
     IN PCCERTOBJECTCACHE pCertObjectCache
     )
{
    delete pCertObjectCache;
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainCreateCertificateChainContext
//
//  Synopsis:   create certificate chain context object
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainCreateCertificateChainContext (
     OUT PCCERTCHAINCONTEXT* ppCertChainContext
     )
{
    BOOL               fResult = FALSE;
    PCCERTCHAINCONTEXT pCertChainContext;

    pCertChainContext = new CCertChainContext( fResult );
    if ( pCertChainContext != NULL )
    {
        if ( fResult == TRUE )
        {
            *ppCertChainContext = pCertChainContext;
        }
        else
        {
            ChainFreeCertificateChainContext( pCertChainContext );
        }
    }
    else
    {
        SetLastError( E_OUTOFMEMORY );
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFreeCertificateChainContext
//
//  Synopsis:   free the certificate chain context object
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFreeCertificateChainContext (
     IN PCCERTCHAINCONTEXT pCertChainContext
     )
{
    delete pCertChainContext;
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainAddRefInternalChainContext
//
//  Synopsis:   addref the internal chain context
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainAddRefInternalChainContext (
     IN PINTERNAL_CERT_CHAIN_CONTEXT pChainContext
     )
{
    InterlockedIncrement( (LONG *)&pChainContext->cRefs );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainReleaseInternalChainContext
//
//  Synopsis:   release the internal chain context
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainReleaseInternalChainContext (
     IN PINTERNAL_CERT_CHAIN_CONTEXT pChainContext
     )
{
    if ( InterlockedDecrement( (LONG *)&pChainContext->cRefs ) == 0 )
    {
        ChainFreeInternalChainContext( pChainContext );
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFreeInternalChainContext
//
//  Synopsis:   free the internal chain context
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFreeInternalChainContext (
     IN PINTERNAL_CERT_CHAIN_CONTEXT pChainContext
     )
{
    DWORD                cChain;
    DWORD                cElement;
    PCERT_SIMPLE_CHAIN*  rgpChain;
    PCERT_CHAIN_ELEMENT* rgpElement;

    if ( pChainContext->rgElementCount == NULL )
    {
        return;
    }

    rgpChain = pChainContext->ChainContext.rgpChain;

    for ( cChain = 0; cChain < pChainContext->cSimpleChain; cChain++ )
    {
        if ( rgpChain[ cChain ] == NULL )
        {
            continue;
        }

        rgpElement = rgpChain[ cChain ]->rgpElement;

        for ( cElement = 0;
              cElement < pChainContext->rgElementCount[ cChain ];
              cElement++ )
        {
            if ( rgpElement[ cElement ] != NULL )
            {
                if ( rgpElement[ cElement ]->pCertContext != NULL )
                {
                    CertFreeCertificateContext(
                        rgpElement[ cElement ]->pCertContext
                        );
                }

                ChainFreeRevocationInfo(
                     rgpElement[ cElement ]->pRevocationInfo
                     );
            }

            delete rgpElement[ cElement ];
        }
#ifdef CAPI_INCLUDE_CTL
        if ( rgpChain[ cChain ]->pTrustListInfo != NULL )
        {
            SSCtlFreeTrustListInfo( rgpChain[ cChain ]->pTrustListInfo );
        }
#else
		assert(!rgpChain[ cChain ]->pTrustListInfo);
#endif		

        delete rgpElement;
        delete rgpChain[ cChain ];
    }

    delete rgpChain;
    delete pChainContext->rgElementCount;
    delete pChainContext;
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainUpdateSummaryStatusByTrustStatus
//
//  Synopsis:   update the summary status bits given new trust status bits
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainUpdateSummaryStatusByTrustStatus (
     IN PCERT_TRUST_STATUS pSummaryStatus,
     IN PCERT_TRUST_STATUS pTrustStatus
     )
{
    pSummaryStatus->dwErrorStatus |= pTrustStatus->dwErrorStatus;
    pSummaryStatus->dwInfoStatus |= pTrustStatus->dwInfoStatus;
    pSummaryStatus->dwInfoStatus &= ~CERT_TRUST_CERTIFICATE_ONLY_INFO_STATUS;
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainOrInStatusBits
//
//  Synopsis:   bit or in the status bits from the source into the destination
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainOrInStatusBits (
     IN PCERT_TRUST_STATUS pDestStatus,
     IN PCERT_TRUST_STATUS pSourceStatus
     )
{
    pDestStatus->dwErrorStatus |= pSourceStatus->dwErrorStatus;
    pDestStatus->dwInfoStatus |= pSourceStatus->dwInfoStatus;
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainGetChainErrorStatusQuality
//
//  Synopsis:   get the quality of a chain error status
//
//----------------------------------------------------------------------------
LONG WINAPI
ChainGetChainErrorStatusQuality (
     IN DWORD dwChainErrorStatus
     )
{
    LONG lQuality = 0;

    if ( !( dwChainErrorStatus & CERT_TRUST_IS_NOT_SIGNATURE_VALID ) )
    {
        lQuality += CERT_QUALITY_CHAIN_SIGNATURE_VALID;
    }

    if ( !( dwChainErrorStatus & CERT_TRUST_IS_UNTRUSTED_ROOT ) )
    {
        lQuality += CERT_QUALITY_HAS_PROTECTED_ROOT;
    }

    if ( !( dwChainErrorStatus & CERT_TRUST_IS_NOT_TIME_NESTED ) )
    {
        lQuality += CERT_QUALITY_CHAIN_TIME_NESTED;
    }

    return( lQuality );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainCacheEndCertificate
//
//  Synopsis:   cache an end certificate in the cert object cache
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainCacheEndCertificate (
     IN PCCHAINCALLCONTEXT pCallContext,
     IN PCCERTOBJECT pEndCertObject
     )
{
    BOOL               fResult;
    BOOL               fIssuerOfCertificates;
    PCCERT_CONTEXT     pCertContext = NULL;
    BYTE               CertificateHash[ MD5DIGESTLEN ];
    DWORD              cbData = MD5DIGESTLEN;
    PCCERTOBJECT       pCertObject;
    PCCERTOBJECTCACHE  pCertObjectCache;
    CRYPT_HASH_BLOB    HashBlob;
    BOOL               fFromCallCreationCache;

    fIssuerOfCertificates = ChainIsIssuerOfCertificates( pEndCertObject );
    pCertObjectCache = pEndCertObject->ChainEngine()->CertObjectCache();

    if ( fIssuerOfCertificates == TRUE )
    {
        pCertObjectCache->DisableLruOfObjects();
    }

    pCertObjectCache->AddObject( pCallContext, pEndCertObject, TRUE );

    if ( fIssuerOfCertificates == FALSE )
    {
        return( TRUE );
    }

    while ( ( pCertContext = CertEnumCertificatesInStore(
                                 pEndCertObject->ChainEngine()->OtherStore(),
                                 pCertContext
                                 ) ) != NULL )
    {
        if ( ChainIsInSameIssuerGrouping(
                  pEndCertObject,
                  pCertContext
                  ) == FALSE )
        {
            continue;
        }

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

            pCertObject = pCertObjectCache->FindObjectByHash( &HashBlob );
            if ( pCertObject == NULL )
            {
                fResult = ChainCreateCertificateObject(
                               pEndCertObject->ChainEngine(),
                               pCallContext,
                               pCertContext,
                               &HashBlob,
                               NULL,
                               NULL,
                               &pCertObject,
                               &fFromCallCreationCache
                               );

                if ( fResult == TRUE )
                {
                    pCertObjectCache->AddObject(
                                         NULL,
                                         pCertObject,
                                         fFromCallCreationCache
                                         );
                }
            }

            if ( pCertObject != NULL )
            {
                pCertObject->Release();
            }
        }
    }

    pCertObjectCache->EnableLruOfObjects( pCallContext );

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainIsIssuerOfCertificates
//
//  Synopsis:   returns TRUE if this certificate can issuer other certificates
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainIsIssuerOfCertificates (
     IN PCCERTOBJECT pCertObject
     )
{
    BOOL                          fResult = FALSE;
    BOOL                          fIssuerOfCertificates = TRUE;
    PCERT_EXTENSION               pExt;
    DWORD                         cb;
    PCERT_BASIC_CONSTRAINTS2_INFO pBasicConstraints;

    pExt = CertFindExtension(
               szOID_BASIC_CONSTRAINTS2,
               pCertObject->CertContext()->pCertInfo->cExtension,
               pCertObject->CertContext()->pCertInfo->rgExtension
               );

    if ( pExt != NULL )
    {
        fResult = CryptDecodeObject(
                       X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                       pExt->pszObjId,
                       pExt->Value.pbData,
                       pExt->Value.cbData,
                       0,
                       NULL,
                       &cb
                       );

        if ( fResult == TRUE )
        {
            pBasicConstraints = (PCERT_BASIC_CONSTRAINTS2_INFO)new BYTE [ cb ];
            if ( pBasicConstraints != NULL )
            {
                pBasicConstraints->fCA = TRUE;

                fResult = CryptDecodeObject(
                               X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                               pExt->pszObjId,
                               pExt->Value.pbData,
                               pExt->Value.cbData,
                               0,
                               pBasicConstraints,
                               &cb
                               );

                fIssuerOfCertificates = pBasicConstraints->fCA;

                delete pBasicConstraints;
            }
        }
    }

    return( fIssuerOfCertificates );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainConvertAuthKeyIdentifierFromV2ToV1
//
//  Synopsis:   convert authority key identifier from V2 to V1
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainConvertAuthKeyIdentifierFromV2ToV1 (
     IN PCERT_AUTHORITY_KEY_ID2_INFO pAuthKeyIdentifier2,
     OUT PCERT_AUTHORITY_KEY_ID_INFO* ppAuthKeyIdentifier
     )
{
    DWORD                       cb;
    PCERT_AUTHORITY_KEY_ID_INFO pAuthKeyIdentifier;
    BOOL                        fExactMatchAvailable = FALSE;

    if ( ( pAuthKeyIdentifier2->AuthorityCertIssuer.cAltEntry == 1 ) &&
         ( pAuthKeyIdentifier2->AuthorityCertIssuer.rgAltEntry[0].dwAltNameChoice ==
           CERT_ALT_NAME_DIRECTORY_NAME ) )
    {
        fExactMatchAvailable = TRUE;
    }

    cb = sizeof( CERT_AUTHORITY_KEY_ID_INFO );
    cb += pAuthKeyIdentifier2->KeyId.cbData;

    if ( fExactMatchAvailable == TRUE )
    {
        cb += pAuthKeyIdentifier2->AuthorityCertIssuer.rgAltEntry[0].DirectoryName.cbData;
        cb += pAuthKeyIdentifier2->AuthorityCertSerialNumber.cbData;
    }

    pAuthKeyIdentifier = (PCERT_AUTHORITY_KEY_ID_INFO)new BYTE [ cb ];
    if ( pAuthKeyIdentifier == NULL )
    {
        SetLastError( E_OUTOFMEMORY );
        return( FALSE );
    }

    memset( pAuthKeyIdentifier, 0, cb );

    pAuthKeyIdentifier->KeyId.cbData = pAuthKeyIdentifier2->KeyId.cbData;
    pAuthKeyIdentifier->KeyId.pbData = (LPBYTE)pAuthKeyIdentifier + sizeof( CERT_AUTHORITY_KEY_ID_INFO );

    memcpy(
       pAuthKeyIdentifier->KeyId.pbData,
       pAuthKeyIdentifier2->KeyId.pbData,
       pAuthKeyIdentifier->KeyId.cbData
       );

    if ( fExactMatchAvailable == TRUE )
    {
        pAuthKeyIdentifier->CertIssuer.cbData = pAuthKeyIdentifier2->AuthorityCertIssuer.rgAltEntry[0].DirectoryName.cbData;
        pAuthKeyIdentifier->CertIssuer.pbData = pAuthKeyIdentifier->KeyId.pbData + pAuthKeyIdentifier->KeyId.cbData;

        memcpy(
           pAuthKeyIdentifier->CertIssuer.pbData,
           pAuthKeyIdentifier2->AuthorityCertIssuer.rgAltEntry[0].DirectoryName.pbData,
           pAuthKeyIdentifier->CertIssuer.cbData
           );

        pAuthKeyIdentifier->CertSerialNumber.cbData = pAuthKeyIdentifier2->AuthorityCertSerialNumber.cbData;
        pAuthKeyIdentifier->CertSerialNumber.pbData = pAuthKeyIdentifier->CertIssuer.pbData + pAuthKeyIdentifier->CertIssuer.cbData;

        memcpy(
           pAuthKeyIdentifier->CertSerialNumber.pbData,
           pAuthKeyIdentifier2->AuthorityCertSerialNumber.pbData,
           pAuthKeyIdentifier->CertSerialNumber.cbData
           );
    }

    *ppAuthKeyIdentifier = pAuthKeyIdentifier;

    return( TRUE );
}

#ifdef CAPI_INCLUDE_CTL
//+---------------------------------------------------------------------------
//
//  Function:   ChainFindAndStoreCtlIssuersEnumFn
//
//  Synopsis:   find and store CTL based issuers in engine
//
//----------------------------------------------------------------------------
BOOL
ChainFindAndStoreCtlIssuersEnumFn (
     IN LPVOID pvParameter,
     IN PCSSCTLOBJECT pSSCtlObject
     )
{
    BOOL                   fResult;
    PCTL_ISSUERS_ENUM_DATA pEnumData;

    pEnumData = (PCTL_ISSUERS_ENUM_DATA)pvParameter;

    fResult = pEnumData->pCertObject->IssuerList()->AddCtlIssuer(
                                                       pEnumData->pCallContext,
                                                       pSSCtlObject,
                                                       FALSE,
                                                       NULL,
                                                       FALSE
                                                       );

    // NOTE: If we get an error due to not finding the subject in the CTL then
    //       we are ok.  However, we want to always continue enumerating so
    //       I'm not sure what to do on other types of errors.

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFreeCtlIssuerData
//
//  Synopsis:   free CTL issuer data
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFreeCtlIssuerData (
     IN PCTL_ISSUER_DATA pCtlIssuerData
     )
{
    if ( pCtlIssuerData->pTrustListInfo != NULL )
    {
        SSCtlFreeTrustListInfo( pCtlIssuerData->pTrustListInfo );
    }

    if ( pCtlIssuerData->pSSCtlObject != NULL )
    {
        pCtlIssuerData->pSSCtlObject->Release();
    }

    delete pCtlIssuerData;
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFixupCtlIssuers
//
//  Synopsis:   fixup the CTL issuers
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFixupCtlIssuers (
     IN PCCERTOBJECT pSubject,
     IN PCCHAINCALLCONTEXT pCallContext,
     IN HCERTSTORE hAdditionalStore
     )
{
    PCCERTISSUERLIST     pIssuerList;
    PCERT_ISSUER_ELEMENT pElement = NULL;
    PCSSCTLOBJECT        pSSCtlObject;
    BOOL                 fCtlSignatureValid;

    pIssuerList = pSubject->IssuerList();

    while ( ( pElement = pIssuerList->NextElement( pElement ) ) != NULL )
    {
        if ( ( pElement->fCtlIssuer == TRUE ) && ( pElement->pIssuer == NULL ) )
        {
            pSSCtlObject = pElement->pCtlIssuerData->pSSCtlObject;

            if ( pSSCtlObject->GetSigner(
                                  pSubject,
                                  pCallContext,
                                  hAdditionalStore,
                                  &pElement->pIssuer,
                                  &pElement->pCtlIssuerData->fAdditionalStoreSigner,
                                  &fCtlSignatureValid
                                  ) == TRUE )
            {
                pElement->ChainStatus.dwErrorStatus = pElement->pIssuer->ChainErrorStatus();

                if ( fCtlSignatureValid == FALSE )
                {
                    pElement->ChainStatus.dwErrorStatus |= CERT_TRUST_IS_NOT_SIGNATURE_VALID;
                }
            }
        }
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFindAndStoreCtlIssuersFromAdditionalStore
//
//  Synopsis:   find and store CTL based issuers from additional store
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainFindAndStoreCtlIssuersFromAdditionalStore (
     IN PCCERTOBJECT pSubject,
     IN PCCHAINCALLCONTEXT pCallContext,
     IN HCERTSTORE hAdditionalStore
     )
{
    BOOL          fResult;
    PCCTL_CONTEXT pCtlContext = NULL;
    PCSSCTLOBJECT pSSCtlObject;

    while ( ( pCtlContext = CertEnumCTLsInStore(
                                hAdditionalStore,
                                pCtlContext
                                ) ) != NULL )
    {
        fResult = SSCtlCreateCtlObject(
                       pSubject->ChainEngine(),
                       pCtlContext,
                       &pSSCtlObject
                       );

        if ( fResult == TRUE )
        {
            fResult = pSubject->IssuerList()->AddCtlIssuer(
                                                 pCallContext,
                                                 pSSCtlObject,
                                                 TRUE,
                                                 hAdditionalStore,
                                                 TRUE
                                                 );

            pSSCtlObject->Release();
        }
    }

    return( TRUE );
}
#endif
//+---------------------------------------------------------------------------
//
//  Function:   ChainFixupUntrustedRootElementStatus
//
//  Synopsis:   fixup untrusted root element status
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFixupUntrustedRootElementStatus (
     IN PCERT_CHAIN_CONTEXT pContext
     )
{
    DWORD cChain;
    DWORD cElement;
    PCERT_CHAIN_ELEMENT pElement;

    for ( cChain = 0; cChain < pContext->cChain - 1; cChain++ )
    {
        cElement = pContext->rgpChain[ cChain ]->cElement;
        pElement = pContext->rgpChain[ cChain ]->rgpElement[ cElement - 1 ];
        pElement->TrustStatus.dwErrorStatus &= ~CERT_TRUST_IS_UNTRUSTED_ROOT;
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainCreateWorldStore
//
//  Synopsis:   create the world store
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainCreateWorldStore (
     IN HCERTSTORE hRoot,
     IN DWORD cAdditionalStore,
     IN HCERTSTORE* rghAdditionalStore,
     IN DWORD dwStoreFlags,
     OUT HCERTSTORE* phWorld
     )
{
    BOOL       fResult;
    HCERTSTORE hWorld;
    HCERTSTORE hStore;
    DWORD      cCount;

    hWorld = CertOpenStore(
                 CERT_STORE_PROV_COLLECTION,
                 X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                 NULL,
                 CERT_STORE_DEFER_CLOSE_UNTIL_LAST_FREE_FLAG,
                 NULL
                 );

    if ( hWorld == NULL )
    {
        return( FALSE );
    }

    fResult = CertAddStoreToCollection( hWorld, hRoot, 0, 0 );

    for ( cCount = 0;
          ( cCount < cAdditionalStore ) && ( fResult == TRUE );
          cCount++ )
    {
        fResult = CertAddStoreToCollection(
                      hWorld,
                      rghAdditionalStore[ cCount ],
                      0,
                      0
                      );
    }

    dwStoreFlags |= CERT_STORE_MAXIMUM_ALLOWED_FLAG;

    if ( fResult == TRUE )
    {
        hStore = CertOpenStore(
                     CERT_STORE_PROV_SYSTEM_W,
                     X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                     NULL,
                     dwStoreFlags,
                     L"trust"
                     );

        if ( hStore != NULL )
        {
            fResult = CertAddStoreToCollection( hWorld, hStore, 0, 0 );
            CertCloseStore( hStore, 0 );
        }
        else
        {
            fResult = FALSE;
        }
    }

    if ( fResult == TRUE )
    {
        hStore = CertOpenStore(
                     CERT_STORE_PROV_SYSTEM_W,
                     X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                     NULL,
                     dwStoreFlags,
                     L"ca"
                     );

        if ( hStore != NULL )
        {
            fResult = CertAddStoreToCollection( hWorld, hStore, 0, 0 );
            CertCloseStore( hStore, 0 );
        }
        else
        {
            fResult = FALSE;
        }
    }

    if ( fResult == TRUE )
    {
        hStore = CertOpenStore(
                     CERT_STORE_PROV_SYSTEM_W,
                     X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                     NULL,
                     dwStoreFlags,
                     L"my"
                     );

        if ( hStore != NULL )
        {
            fResult = CertAddStoreToCollection( hWorld, hStore, 0, 0 );
            CertCloseStore( hStore, 0 );
        }
        else
        {
            fResult = FALSE;
        }
    }

    if ( fResult == TRUE )
    {
        *phWorld = hWorld;
    }
    else
    {
        CertCloseStore( hWorld, 0 );
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainCreateEngineStore
//
//  Synopsis:   create the engine store and the change event handle
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainCreateEngineStore (
     IN HCERTSTORE hRootStore,
     IN HCERTSTORE hTrustStore,
     IN HCERTSTORE hOtherStore,
     IN BOOL fDefaultEngine,
     IN DWORD dwFlags,
     OUT HCERTSTORE* phEngineStore,
     OUT HANDLE* phEngineStoreChangeEvent
     )
{
    BOOL       fResult = TRUE;
    HCERTSTORE hEngineStore;
    HANDLE     hEvent;

    hEngineStore = CertOpenStore(
                       CERT_STORE_PROV_COLLECTION,
                       X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                       NULL,
                       CERT_STORE_DEFER_CLOSE_UNTIL_LAST_FREE_FLAG,
                       NULL
                       );

    hEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

    if ( ( hEngineStore == NULL ) || ( hEvent == NULL ) )
    {
        fResult = FALSE;
    }

    if ( fResult == TRUE )
    {
        fResult = CertAddStoreToCollection( hEngineStore, hRootStore, 0, 0 );
    }

    if ( fResult == TRUE )
    {
        fResult = CertAddStoreToCollection( hEngineStore, hTrustStore, 0, 0 );
    }

    if ( fResult == TRUE )
    {
        fResult = CertAddStoreToCollection( hEngineStore, hOtherStore, 0, 0 );
    }

    if ( ( fResult == TRUE ) &&
         ( dwFlags & CERT_CHAIN_ENABLE_CACHE_AUTO_UPDATE ) )
    {
        // BUGBUG: Someday support a let me know about errors flag
        CertControlStore(
            hEngineStore,
            0,
            CERT_STORE_CTRL_NOTIFY_CHANGE,
            &hEvent
            );
    }

    if ( fResult == TRUE )
    {
        *phEngineStore = hEngineStore;
        *phEngineStoreChangeEvent = hEvent;
    }
    else
    {
        if ( hEngineStore != NULL )
        {
            CertCloseStore( hEngineStore, 0 );
        }

        if ( hEvent != NULL )
        {
            CloseHandle( hEvent );
        }
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainIsProperRestrictedRoot
//
//  Synopsis:   check to see if this restricted root store is a proper subset
//              of the real root store
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainIsProperRestrictedRoot (
     IN HCERTSTORE hRealRoot,
     IN HCERTSTORE hRestrictedRoot
     )
{
    PCCERT_CONTEXT  pCertContext = NULL;
    PCCERT_CONTEXT  pFound = NULL;
    DWORD           cbData = MD5DIGESTLEN;
    BYTE            CertificateHash[ MD5DIGESTLEN ];
    CRYPT_HASH_BLOB HashBlob;

    HashBlob.cbData = cbData;
    HashBlob.pbData = CertificateHash;

    while ( ( pCertContext = CertEnumCertificatesInStore(
                                 hRestrictedRoot,
                                 pCertContext
                                 ) ) != NULL )
    {
        if ( CertGetCertificateContextProperty(
                 pCertContext,
                 CERT_MD5_HASH_PROP_ID,
                 CertificateHash,
                 &cbData
                 ) == TRUE )
        {
            pFound = CertFindCertificateInStore(
                         hRealRoot,
                         X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                         0,
                         CERT_FIND_MD5_HASH,
                         &HashBlob,
                         NULL
                         );

            if ( pFound == NULL )
            {
                CertFreeCertificateContext( pCertContext );
                return( FALSE );
            }
            else
            {
                CertFreeCertificateContext( pFound );
            }
        }
    }

    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainCopyRequestedUsage
//
//  Synopsis:   copy the requested usage
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainCopyRequestedUsage (
     IN PCERT_USAGE_MATCH pSource,
     OUT PCERT_USAGE_MATCH pDest
     )
{
    BOOL  fResult = TRUE;
    DWORD cCount;
    DWORD cch;

    pDest->dwType = pSource->dwType;
    pDest->Usage.cUsageIdentifier = 0;

    pDest->Usage.rgpszUsageIdentifier = new LPSTR [ pSource->Usage.cUsageIdentifier ];
    if ( pDest->Usage.rgpszUsageIdentifier != NULL )
    {
        memset(
           pDest->Usage.rgpszUsageIdentifier,
           0,
           pSource->Usage.cUsageIdentifier * sizeof( LPSTR )
           );

        for ( cCount = 0;
              ( cCount < pSource->Usage.cUsageIdentifier ) &&
              ( fResult == TRUE );
              cCount++ )
        {
            cch = strlen( pSource->Usage.rgpszUsageIdentifier[ cCount ] ) + 1;

            pDest->Usage.rgpszUsageIdentifier[ cCount ] = new CHAR [ cch ];
            if ( pDest->Usage.rgpszUsageIdentifier[ cCount ] != NULL )
            {
                pDest->Usage.cUsageIdentifier += 1;

                memcpy(
                   pDest->Usage.rgpszUsageIdentifier[ cCount ],
                   pSource->Usage.rgpszUsageIdentifier[ cCount ],
                   cch
                   );
            }
            else
            {
                fResult = FALSE;
            }
        }
    }
    else
    {
        fResult = FALSE;
    }

    if ( fResult == FALSE )
    {
        ChainFreeRequestedUsage( pDest );
    }

    return( fResult );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFreeRequestedUsage
//
//  Synopsis:   free the usage structure
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFreeRequestedUsage (
     IN PCERT_USAGE_MATCH pRequestedUsage
     )
{
    DWORD cCount;

    for ( cCount = 0;
          cCount < pRequestedUsage->Usage.cUsageIdentifier;
          cCount++ )
    {
        delete pRequestedUsage->Usage.rgpszUsageIdentifier[ cCount ];
    }

    delete pRequestedUsage->Usage.rgpszUsageIdentifier;
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainIsCertificateObjectRepeatedInContext
//
//  Synopsis:   check if the certificate object appears multiple times in the
//              chain context using the hash of the certificate context
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainIsCertificateObjectRepeatedInContext (
     IN PCCERTOBJECT pCertObject,
     IN PCERT_CHAIN_CONTEXT pContext
     )
{
    DWORD              cChain;
    DWORD              cElement;
    PCERT_SIMPLE_CHAIN pChain;
    BYTE               CertificateHash[ MD5DIGESTLEN ];
    DWORD              cbData = MD5DIGESTLEN;
    DWORD              cFound = 0;

    for ( cChain = 0; cChain < pContext->cChain; cChain++ )
    {
        pChain = pContext->rgpChain[ cChain ];

        for ( cElement = 0; cElement < pChain->cElement; cElement++ )
        {
            if ( CertGetCertificateContextProperty(
                     pChain->rgpElement[ cElement ]->pCertContext,
                     CERT_MD5_HASH_PROP_ID,
                     CertificateHash,
                     &cbData
                     ) == TRUE )
            {
                if ( memcmp(
                        pCertObject->CertificateHash(),
                        CertificateHash,
                        cbData
                        ) == 0 )
                {
                    cFound += 1;
                }
            }
        }
    }

    if ( cFound > 1 )
    {
        return( TRUE );
    }

    return( FALSE );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainFindAndAdjustChainContextToCycle
//
//  Synopsis:   find and adjust the chain context
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFindAndAdjustChainContextToCycle (
     IN PCERT_CHAIN_CONTEXT pContext
     )
{
    DWORD iChain;
    DWORD iElement;
    DWORD iFoundChain = 0;
    DWORD iFoundElement = 0;

    for ( iChain = 0; iChain < pContext->cChain; iChain++ )
    {
        for ( iElement = 0;
              iElement < pContext->rgpChain[ iChain ]->cElement;
              iElement++ )
        {
            if ( ChainIsElementRepeatedInContext(
                      pContext,
                      iChain,
                      iElement,
                      &iFoundChain,
                      &iFoundElement
                      ) == TRUE )
            {
                pContext->cChain = iFoundChain + 1;

                if ( iFoundElement < ( pContext->rgpChain[ iFoundChain ]->cElement - 1 ) )
                {
#ifdef CAPI_INCLUDE_CTL                
                    if ( pContext->rgpChain[ iFoundChain ]->pTrustListInfo != NULL )
                    {
                        SSCtlFreeTrustListInfo(
                             pContext->rgpChain[ iFoundChain ]->pTrustListInfo
                             );

                        pContext->rgpChain[ iFoundChain ]->pTrustListInfo = NULL;
                    }
#else
					assert(!pContext->rgpChain[ iFoundChain ]->pTrustListInfo);
#endif					
                }

                pContext->rgpChain[ iFoundChain ]->cElement = iFoundElement + 1;

                pContext->rgpChain[ iFoundChain ]->rgpElement[
                           iFoundElement
                           ]->TrustStatus.dwErrorStatus |= CERT_TRUST_IS_CYCLIC;

                return;
            }
        }
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainIsElementRepeatedInContext
//
//  Synopsis:   is the element repeated in the context
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainIsElementRepeatedInContext (
     IN PCERT_CHAIN_CONTEXT pContext,
     IN DWORD iChain,
     IN DWORD iElement,
     OUT DWORD* piFoundChain,
     OUT DWORD* piFoundElement
     )
{
    DWORD cChain;
    DWORD cElement;
    DWORD cbHash = MD5DIGESTLEN;
    BYTE  ElementHash[ MD5DIGESTLEN ];
    BYTE  FoundHash[ MD5DIGESTLEN ];

    if ( CertGetCertificateContextProperty(
             pContext->rgpChain[ iChain ]->rgpElement[ iElement ]->pCertContext,
             CERT_MD5_HASH_PROP_ID,
             ElementHash,
             &cbHash
             ) == FALSE )
    {
        return( FALSE );
    }

    for ( cChain = iChain; cChain < pContext->cChain; cChain++ )
    {
        for ( cElement = ( cChain == iChain ) ? iElement + 1 : 0;
              cElement < pContext->rgpChain[ cChain ]->cElement;
              cElement++ )
        {
            if ( CertGetCertificateContextProperty(
                     pContext->rgpChain[ cChain ]->rgpElement[ cElement ]->pCertContext,
                     CERT_MD5_HASH_PROP_ID,
                     FoundHash,
                     &cbHash
                     ) == TRUE )
            {
                if ( memcmp( ElementHash, FoundHash, cbHash ) == 0 )
                {
                    *piFoundChain = cChain;
                    *piFoundElement = cElement;
                    return( TRUE );
                }
            }
        }
    }

    return( FALSE );
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainCreateCollectionIncludingCtlCertificates
//
//  Synopsis:   create a collection which includes the source store hStore and
//              any CTL certificates from it
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainCreateCollectionIncludingCtlCertificates (
     IN HCERTSTORE hStore,
     OUT HCERTSTORE* phCollection
     )
{
    BOOL          fResult = FALSE;
    HCERTSTORE    hCollection;
    PCCTL_CONTEXT pCtlContext = NULL;

    hCollection = CertOpenStore(
                      CERT_STORE_PROV_COLLECTION,
                      X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                      NULL,
                      CERT_STORE_DEFER_CLOSE_UNTIL_LAST_FREE_FLAG,
                      NULL
                      );

    if ( hCollection == NULL )
    {
        return( FALSE );
    }

    fResult = CertAddStoreToCollection( hCollection, hStore, 0, 0 );

#ifdef CAPI_INCLUDE_CTL
    HCERTSTORE    hCtlStore;
    while ( ( fResult == TRUE ) &&
            ( ( pCtlContext = CertEnumCTLsInStore(
                                  hStore,
                                  pCtlContext
                                  ) ) != NULL ) )
    {
        hCtlStore = CertOpenStore(
                        CERT_STORE_PROV_MSG,
                        X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
                        NULL,
                        0,
                        pCtlContext->hCryptMsg
                        );

        if ( hCtlStore != NULL )
        {
            fResult = CertAddStoreToCollection(
                          hCollection,
                          hCtlStore,
                          0,
                          0
                          );

            CertCloseStore( hCtlStore, 0 );
        }
    }
#endif

    if ( fResult == TRUE )
    {
        *phCollection = hCollection;
    }
    else
    {
        CertCloseStore( hCollection, 0 );
    }

    return( fResult );
}

#ifdef CAPI_INCLUDE_CRL
//+---------------------------------------------------------------------------
//
//  Function:   ChainPostProcessCheckRevocation
//
//  Synopsis:   check revocation
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainPostProcessCheckRevocation (
     IN HCERTSTORE hCrlStore,
     IN LPFILETIME pftTimeToUse,
     IN LPFILETIME pftCurrentTime,
     IN PCERT_CHAIN_CONTEXT pContext,
     IN DWORD dwFlags
     )
{
    DWORD                  cChain;
    DWORD                  cElement;
    DWORD                  cChainEnd;
    DWORD                  cLastChainElementEnd;
    DWORD                  cElementEnd;
    CERT_REVOCATION_PARA   RevPara;
    CERT_REVOCATION_STATUS RevStatus;
    BOOL                   fDoVerifyRevocation;

    memset( &RevPara, 0, sizeof( RevPara ) );
    memset( &RevStatus, 0, sizeof( RevStatus ) );

    RevPara.cbSize = sizeof( RevPara );
    RevStatus.cbSize = sizeof( RevStatus );

    assert( dwFlags & CERT_CHAIN_REVOCATION_CHECK_ALL );

    if ( dwFlags & CERT_CHAIN_REVOCATION_CHECK_END_CERT )
    {
        cChainEnd = 1;
        cLastChainElementEnd = 1;
    }
    else if ( ( dwFlags & CERT_CHAIN_REVOCATION_CHECK_CHAIN ) ||
              ( dwFlags & CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT ) )
    {
        cChainEnd = pContext->cChain;
        cLastChainElementEnd = pContext->rgpChain[ cChainEnd - 1 ]->cElement;
    }

    for ( cChain = 0; cChain < cChainEnd; cChain++ )
    {
        if ( cChain == ( cChainEnd - 1 ) )
        {
            cElementEnd = cLastChainElementEnd;
        }
        else
        {
            cElementEnd = pContext->rgpChain[ cChain ]->cElement;
        }

        for ( cElement = 0; cElement < cElementEnd; cElement++ )
        {
            fDoVerifyRevocation = TRUE;

            if ( cElement == ( pContext->rgpChain[ cChain ]->cElement - 1 ) )
            {
                if ( pContext->rgpChain[ cChain ]->rgpElement[ cElement ]->TrustStatus.dwInfoStatus &
                     CERT_TRUST_IS_SELF_SIGNED )
                {
                    if ( dwFlags & CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT )
                    {
                        RevStatus.dwError = 0;
                        RevStatus.dwIndex = 0;
                        RevStatus.dwReason = 0;

                        fDoVerifyRevocation = FALSE;
                    }
                    else
                    {
                        RevPara.pIssuerCert = pContext->rgpChain[ cChain ]->rgpElement[ cElement ]->pCertContext;
                    }
                }
                else
                {
                    RevStatus.dwError = CRYPT_E_NO_REVOCATION_CHECK;
                    RevStatus.dwIndex = 0;
                    RevStatus.dwReason = 0;

                    fDoVerifyRevocation = FALSE;
                }
            }
            else
            {
                RevPara.pIssuerCert = pContext->rgpChain[ cChain ]->rgpElement[ cElement + 1 ]->pCertContext;
            }

            if ( fDoVerifyRevocation == TRUE )
            {
                RevPara.hCrlStore = hCrlStore;

                if ( cChain > 0 )
                {
                    RevPara.pftTimeToUse = pftCurrentTime;
                }
                else
                {
                    RevPara.pftTimeToUse = pftTimeToUse;
                }

                if ( CertVerifyRevocation(
                         X509_ASN_ENCODING,
                         CERT_CONTEXT_REVOCATION_TYPE,
                         1,
                         (LPVOID *)&pContext->rgpChain[ cChain ]->rgpElement[ cElement ]->pCertContext,
                         ( dwFlags & CERT_CHAIN_REVOCATION_CHECK_CACHE_ONLY ) ?
                         CERT_VERIFY_CACHE_ONLY_BASED_REVOCATION : 0,
                         &RevPara,
                         &RevStatus
                         ) == TRUE )
                {
                    RevStatus.dwError = 0;
                    RevStatus.dwIndex = 0;
                    RevStatus.dwReason = 0;
                }
            }

            ChainSetRevocationInfo( pContext, &RevStatus, cChain, cElement );
        }
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainSetRevocationInfo
//
//  Synopsis:   set the revocation information on the element
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainSetRevocationInfo (
     IN PCERT_CHAIN_CONTEXT pContext,
     IN PCERT_REVOCATION_STATUS pRevStatus,
     IN DWORD iChain,
     IN DWORD iElement
     )
{
    PCERT_CHAIN_ELEMENT pElement;

    if ( pRevStatus->dwError == ERROR_SUCCESS )
    {
        return;
    }

    pElement = pContext->rgpChain[ iChain ]->rgpElement[ iElement ];

    if ( pRevStatus->dwError == CRYPT_E_REVOKED )
    {
        pElement->TrustStatus.dwErrorStatus |= CERT_TRUST_IS_REVOKED;
    }
    else
    {
        pElement->TrustStatus.dwErrorStatus |= CERT_TRUST_REVOCATION_STATUS_UNKNOWN;
    }

    pElement->pRevocationInfo = new CERT_REVOCATION_INFO;
    if ( pElement->pRevocationInfo == NULL )
    {
        return;
    }

    memset( pElement->pRevocationInfo, 0, sizeof( CERT_REVOCATION_INFO ) );

    pElement->pRevocationInfo->cbSize = sizeof( CERT_REVOCATION_INFO );
    pElement->pRevocationInfo->dwRevocationResult = pRevStatus->dwError;
}
#endif	//CAPI_INCLUDE_CRL

//+---------------------------------------------------------------------------
//
//  Function:   ChainFreeRevocationInfo
//
//  Synopsis:   free the revocation info data
//
//----------------------------------------------------------------------------
VOID WINAPI
ChainFreeRevocationInfo (
     IN PCERT_REVOCATION_INFO pRevocationInfo
     )
{
    delete pRevocationInfo;
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainGetObjectUrl
//
//  Synopsis:   thunk to CryptGetObjectUrl in cryptnet.dll
//
//----------------------------------------------------------------------------
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
     )
{
#ifndef _XBOX
    BOOL             fResult = FALSE;
    HMODULE          hModule;
    PFN_GETOBJECTURL pfn = NULL;

    hModule = ChainGetCryptnetModule();

    if ( hModule != NULL )
    {
        pfn = (PFN_GETOBJECTURL)GetProcAddress( hModule, CEUNICODE("CryptGetObjectUrl") );
    }

    if ( pfn != NULL )
    {
        fResult = ( *pfn )(
                      pszUrlOid,
                      pvPara,
                      dwFlags,
                      pUrlArray,
                      pcbUrlArray,
                      pUrlInfo,
                      pcbUrlInfo,
                      pvReserved
                      );
    }

    return( fResult );
#else
	ASSERT(!"chain.cpp called");
	return TRUE;
#endif
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainRetrieveObjectByUrlW
//
//  Synopsis:   thunk to CryptRetrieveObjectByUrlW in cryptnet.dll
//
//----------------------------------------------------------------------------
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
     )
{
#ifndef _XBOX
    BOOL                     fResult = FALSE;
    HMODULE                  hModule;
    PFN_RETRIEVEOBJECTBYURLW pfn = NULL;

    hModule = ChainGetCryptnetModule();

    if ( hModule != NULL )
    {
        pfn = (PFN_RETRIEVEOBJECTBYURLW)GetProcAddress(
                                          hModule,
                                          CEUNICODE("CryptRetrieveObjectByUrlW")
                                          );
    }

    if ( pfn != NULL )
    {
        fResult = ( *pfn )(
                      pszUrl,
                      pszObjectOid,
                      dwRetrievalFlags,
                      dwTimeout,
                      ppvObject,
                      hAsyncRetrieve,
                      pCredentials,
                      pvVerify,
                      pvReserved
                      );
    }

    return( fResult );
#else
	ASSERT(!"chain.cpp called");
	return TRUE;
#endif
}

//+---------------------------------------------------------------------------
//
//  Function:   ChainIsFileOrLdapUrl
//
//  Synopsis:   check if the URL given is a file or ldap one
//
//----------------------------------------------------------------------------
BOOL WINAPI
ChainIsFileOrLdapUrl (
     IN LPCWSTR pwszUrl
     )
{
    LPWSTR pwsz;

    pwsz = wcschr( pwszUrl, L':' );
    if ( pwsz != NULL )
    {
        if ( ( _wcsnicmp( pwszUrl, L"file", 4 ) == 0 ) ||
             ( _wcsnicmp( pwszUrl, L"ldap", 4 ) == 0 ) )
        {
            return( TRUE );
        }
        else
        {
            return( FALSE );
        }
    }

    return( TRUE );
}
