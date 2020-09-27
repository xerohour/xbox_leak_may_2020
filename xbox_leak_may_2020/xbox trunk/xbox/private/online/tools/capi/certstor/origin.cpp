//+---------------------------------------------------------------------------
//
//  Microsoft Windows NT Security
//  Copyright (C) Microsoft Corporation, 1997 - 1998
//
//  File:       origin.cpp
//
//  Contents:   Origin Identifier implementation
//
//  History:    10-Sep-97    kirtd    Created
//
//----------------------------------------------------------------------------
#include <global.hxx>
#include <dbgdef.h>
//+---------------------------------------------------------------------------
//
//  Function:   CertGetOriginIdentifier
//
//  Synopsis:   get the origin identifier for a certificate
//
//----------------------------------------------------------------------------
BOOL WINAPI CertGetOriginIdentifier (
                IN PCCERT_CONTEXT pCertContext,
                IN DWORD dwFlags,
                OUT CRYPT_ORIGIN_IDENTIFIER OriginIdentifier
                )
{
    MD5_CTX    md5ctx;
    PCERT_INFO pCertInfo = pCertContext->pCertInfo;

    MD5Init( &md5ctx );

    MD5Update( &md5ctx, pCertInfo->Issuer.pbData, pCertInfo->Issuer.cbData );
    MD5Update( &md5ctx, pCertInfo->Subject.pbData, pCertInfo->Subject.cbData );

    MD5Update(
       &md5ctx,
       (LPBYTE)pCertInfo->SubjectPublicKeyInfo.Algorithm.pszObjId,
       strlen( pCertInfo->SubjectPublicKeyInfo.Algorithm.pszObjId )
       );

    MD5Update(
       &md5ctx,
       pCertInfo->SubjectPublicKeyInfo.Algorithm.Parameters.pbData,
       pCertInfo->SubjectPublicKeyInfo.Algorithm.Parameters.cbData
       );

    // BUGBUG: We assume that the unused public key bits are zero
    MD5Update(
       &md5ctx,
       pCertInfo->SubjectPublicKeyInfo.PublicKey.pbData,
       pCertInfo->SubjectPublicKeyInfo.PublicKey.cbData
       );

    MD5Final( &md5ctx );

    memcpy( OriginIdentifier, md5ctx.digest, MD5DIGESTLEN );
    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Function:   CtlGetOriginIdentifier
//
//  Synopsis:   get the origin identifier for a CTL
//
//----------------------------------------------------------------------------
BOOL WINAPI CtlGetOriginIdentifier (
                IN PCCTL_CONTEXT pCtlContext,
                IN DWORD dwFlags,
                OUT CRYPT_ORIGIN_IDENTIFIER OriginIdentifier
                )
{
    MD5_CTX    md5ctx;
    PCTL_INFO  pCtlInfo = pCtlContext->pCtlInfo;
    HCRYPTMSG  hCryptMsg = pCtlContext->hCryptMsg;
    DWORD      cSigners;
    DWORD      cCount;
    DWORD      cbData;
    CERT_INFO  CertInfo;
    PCTL_USAGE pCtlUsage = &( pCtlContext->pCtlInfo->SubjectUsage );

    MD5Init( &md5ctx );

    if ( CryptMsgGetParam(
              hCryptMsg,
              CMSG_SIGNER_COUNT_PARAM,
              0,
              &cSigners,
              &cbData
              ) == FALSE )
    {
        return( FALSE );
    }

    for ( cCount = 0; cCount < cSigners; cCount++ )
    {
        if ( CryptMsgGetParam(
                  hCryptMsg,
                  CMSG_SIGNER_CERT_INFO_PARAM,
                  cCount,
                  &CertInfo,
                  &cbData
                  ) == FALSE )
        {
            return( FALSE );
        }

        MD5Update( &md5ctx, CertInfo.Issuer.pbData, CertInfo.Issuer.cbData );

        MD5Update(
           &md5ctx,
           CertInfo.SerialNumber.pbData,
           CertInfo.SerialNumber.cbData
           );
    }

    for ( cCount = 0; cCount < pCtlUsage->cUsageIdentifier; cCount++ )
    {
        MD5Update(
           &md5ctx,
           (LPBYTE)pCtlUsage->rgpszUsageIdentifier[cCount],
           strlen( pCtlUsage->rgpszUsageIdentifier[cCount] )
           );
    }

    MD5Update(
       &md5ctx,
       pCtlInfo->ListIdentifier.pbData,
       pCtlInfo->ListIdentifier.cbData
       );

    MD5Final( &md5ctx );

    memcpy( OriginIdentifier, md5ctx.digest, MD5DIGESTLEN );
    return( TRUE );
}

//+---------------------------------------------------------------------------
//
//  Function:   CrlGetOriginIdentifier
//
//  Synopsis:   get the origin identifier for a CRL
//
//----------------------------------------------------------------------------
BOOL WINAPI CrlGetOriginIdentifier (
                IN PCCRL_CONTEXT pCrlContext,
                IN DWORD dwFlags,
                OUT CRYPT_ORIGIN_IDENTIFIER OriginIdentifier
                )
{
    MD5_CTX    md5ctx;
    PCRL_INFO  pCrlInfo = pCrlContext->pCrlInfo;

    MD5Init( &md5ctx );

    MD5Update( &md5ctx, pCrlInfo->Issuer.pbData, pCrlInfo->Issuer.cbData );

    MD5Final( &md5ctx );

    memcpy( OriginIdentifier, md5ctx.digest, MD5DIGESTLEN );
    return( TRUE );
}

