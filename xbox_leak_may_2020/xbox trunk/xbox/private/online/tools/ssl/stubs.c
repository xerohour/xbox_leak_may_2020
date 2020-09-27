//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1995.
//
//  File:       stubs.c
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    8-01-95   RichardW   Created
//
//----------------------------------------------------------------------------


#include <spbase.h>
#include <security.h>
#include "spsspi.h"
#include <cert509.h>


const CHAR CertTag[ 13 ] = { 0x04, 0x0b, 'c', 'e', 'r', 't', 'i', 'f', 'i', 'c', 'a', 't', 'e' };

const SecurityFunctionTableW SPFunctionTable = {
    SECURITY_SUPPORT_PROVIDER_INTERFACE_VERSION,
    EnumerateSecurityPackagesW,
    NULL,
    AcquireCredentialsHandleW,
    FreeCredentialsHandle,
    NULL,
    InitializeSecurityContextW,
    AcceptSecurityContext,
    CompleteAuthToken,
    DeleteSecurityContext,
    ApplyControlToken,
    QueryContextAttributesW,
    ImpersonateSecurityContext,
    RevertSecurityContext,
    MakeSignature,
    VerifySignature,
    FreeContextBuffer,
    QuerySecurityPackageInfoW,
    SealMessage,
    UnsealMessage,
    NULL, /* GrantProxyW */
    NULL, /* RevokeProxyW */
    NULL, /* InvokeProxyW */
    NULL, /* RenewProxyW */
    QuerySecurityContextToken
    };

const SecurityFunctionTableA SPFunctionTableA = {
    SECURITY_SUPPORT_PROVIDER_INTERFACE_VERSION,
    EnumerateSecurityPackagesA,
    NULL,
    AcquireCredentialsHandleA,
    FreeCredentialsHandle,
    NULL,
    InitializeSecurityContextA,
    AcceptSecurityContext,
    CompleteAuthToken,
    DeleteSecurityContext,
    ApplyControlToken,
    QueryContextAttributesA,
    ImpersonateSecurityContext,
    RevertSecurityContext,
    MakeSignature,
    VerifySignature,
    FreeContextBuffer,
    QuerySecurityPackageInfoA,
    SealMessage,
    UnsealMessage,
    NULL, /* GrantProxyA */
    NULL, /* RevokeProxyA */
    NULL, /* InvokeProxyA */
    NULL, /* RenewProxyA */
    QuerySecurityContextToken
    };


PSecurityFunctionTableW SEC_ENTRY
InitSecurityInterfaceW(
    VOID )
{
    return((PSecurityFunctionTableW)&SPFunctionTable);
}

PSecurityFunctionTableA SEC_ENTRY
InitSecurityInterfaceA(
    VOID )
{
    return((PSecurityFunctionTableA)&SPFunctionTableA);
}



SECURITY_STATUS SEC_ENTRY
FreeContextBuffer(
    void SEC_FAR *      pvContextBuffer
    )
{
    SPExternalFree( pvContextBuffer );
    return(SEC_E_OK);
}



SECURITY_STATUS SEC_ENTRY
SecurityPackageControl(
    SEC_WCHAR SEC_FAR *      pszPackageName,
    unsigned long           dwFunctionCode,
    unsigned long           cbInputBuffer,
    unsigned char SEC_FAR * pbInputBuffer,
    unsigned long SEC_FAR * pcbOutputBuffer,
    unsigned char SEC_FAR * pbOutputBuffer)
{
    return(SEC_E_UNSUPPORTED_FUNCTION);
}

SECURITY_STATUS PctTranslateError(SP_STATUS spRet)
{
    switch(spRet) {
        case PCT_ERR_OK: return SEC_E_OK;
        case PCT_ERR_BAD_CERTIFICATE: return SEC_E_INVALID_TOKEN;
        case PCT_ERR_CLIENT_AUTH_FAILED: return SEC_E_INVALID_TOKEN;
        case PCT_ERR_ILLEGAL_MESSAGE: return SEC_E_INVALID_TOKEN;
        case PCT_ERR_INTEGRITY_CHECK_FAILED: return SEC_E_MESSAGE_ALTERED;
        case PCT_ERR_SERVER_AUTH_FAILED: return SEC_E_INVALID_TOKEN;
        case PCT_ERR_SPECS_MISMATCH: return SEC_E_INVALID_TOKEN;
        case PCT_ERR_SSL_STYLE_MSG: return SEC_E_INVALID_TOKEN;
        case PCT_ERR_INCOMPLETE_CREDS: return SEC_I_INCOMPLETE_CREDENTIALS;
        case PCT_ERR_RENEGOTIATE: return SEC_I_RENEGOTIATE;
        case PCT_ERR_UNKNOWN_CREDENTIAL: return SEC_E_UNKNOWN_CREDENTIALS;


        case PCT_INT_BAD_CERT: return SEC_E_INVALID_TOKEN;
        case PCT_INT_CLI_AUTH: return SEC_E_INVALID_TOKEN;
        case PCT_INT_ILLEGAL_MSG: return  SEC_E_INVALID_TOKEN;
        case PCT_INT_INCOMPLETE_MSG: return SEC_E_INCOMPLETE_MESSAGE;
        case PCT_INT_MSG_ALTERED: return SEC_E_MESSAGE_ALTERED;
        case PCT_INT_INTERNAL_ERROR: return SEC_E_INTERNAL_ERROR;
        case PCT_INT_DATA_OVERFLOW: return SEC_E_INTERNAL_ERROR;
        case PCT_INT_INCOMPLETE_CREDS: return SEC_E_INCOMPLETE_CREDENTIALS;
        case PCT_INT_RENEGOTIATE: return SEC_I_RENEGOTIATE;
        case PCT_INT_UNKNOWN_CREDENTIAL: return SEC_E_UNKNOWN_CREDENTIALS;
        default: return SEC_E_INTERNAL_ERROR;
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   SslGenerateRandomBits
//
//  Synopsis:   Hook for setup to get a good random stream
//
//  Arguments:  [pRandomData] --
//              [cRandomData] --
//
//
//  Notes:
//
//----------------------------------------------------------------------------
VOID
WINAPI
SslGenerateRandomBits(
    PUCHAR      pRandomData,
    LONG        cRandomData
    )
{
        GenerateRandomBits(pRandomData, (ULONG)cRandomData);
}

//+---------------------------------------------------------------------------
//
//  Function:   SslGetDefaultIssuers
//
//  Synopsis:   Get a list of the default issuers
//
//  Arguments:  [pRandomData] --
//              [cRandomData] --
//
//
//  Notes:
//
//----------------------------------------------------------------------------
BOOL
WINAPI SslGetDefaultIssuers(
    PBYTE pbIssuers,
    DWORD *pcbIssuers)
{
     return  GetDefaultIssuers(pbIssuers, pcbIssuers);
}

//+---------------------------------------------------------------------------
//
//  Function:   SslGenerateKeyPair
//
//  Synopsis:   Generates a public/private key pair, protected by password
//
//  Arguments:  [pCerts]      --
//              [pszDN]       --
//              [pszPassword] --
//              [Bits]        --
//
//  Notes:
//
//----------------------------------------------------------------------------
BOOL
WINAPI
SslGenerateKeyPair(
    PSSL_CREDENTIAL_CERTIFICATE pCerts,
    PSTR pszDN,
    PSTR pszPassword,
    DWORD Bits )
{

#ifdef SCHANNEL_EXPORT
    if ( Bits > 512 )
    {
        SetLastError( ERROR_INVALID_PARAMETER );
        return( FALSE );
    }
#endif
    return GenerateKeyPair(pCerts, pszDN, pszPassword, Bits);
}


//+---------------------------------------------------------------------------
//
//  Function:   SslGetMaximumKeySize
//
//  Synopsis:   Returns maximum public key size
//
//  Arguments:  [Reserved] --
//
//  Notes:
//
//----------------------------------------------------------------------------
DWORD
WINAPI
SslGetMaximumKeySize(
    DWORD   Reserved )
{

#ifdef SCHANNEL_EXPORT
    return( 512 );
#else
    return( 2048 );
#endif

}


//+---------------------------------------------------------------------------
//
//  Function:   SslFreeCertificate
//
//  Synopsis:   Frees a certificate created from SslCrackCertificate
//
//  Arguments:  [pCertificate] --
//
//  Notes:
//
//----------------------------------------------------------------------------
VOID
WINAPI
SslFreeCertificate(
    PX509Certificate    pCertificate)
{
    if ( pCertificate )
    {

        if (pCertificate->pszSubject)
        {
                SPExternalFree(pCertificate->pszSubject);
        }

        if (pCertificate->pszIssuer)
        {
                SPExternalFree(pCertificate->pszIssuer);
        }

        SPExternalFree(pCertificate);

    }
}

extern BOOL WINAPI
X509DecodeCertificate(
    PUCHAR              pbCertificate,
    DWORD               cbCertificate,
    PPctCertificate *   ppCertificate);

extern BOOL WINAPI
X509DeleteCert(
    PPctCertificate pCertificate
    );

//+---------------------------------------------------------------------------
//
//  Function:   SslCrackCertificate
//
//  Synopsis:   Cracks a X509 certificate into remotely easy format
//
//  Arguments:  [pbCertificate] --
//              [cbCertificate] --
//              [dwFlags]       --
//              [ppCertificate] --
//
//  Notes:
//
//  SHolden - Originally this code would allocate a PctCertificate and then
//            return the pointer to the X509Certificate in the PctCertificate.
//            The PctCertificate would never be freed.
//
//            Now, I still create a PctCertificate so that we can validate
//            the cert, but then I copy the X509Certificate and delete
//            the PctCertificate.
//
//----------------------------------------------------------------------------
BOOL
WINAPI
SslCrackCertificate(
    PUCHAR              pbCertificate,
    DWORD               cbCertificate,
    DWORD               dwFlags,
    PX509Certificate *  ppCertificate)
{
    PX509Certificate        pX509Cert = NULL;
    PPctCertificate         pPctCert  = NULL;
    const SignatureSystem   *pSigSys  = NULL;
    BOOL                    VerifySignature;
    BOOL                    fSuccess;

    *ppCertificate = NULL;

    VerifySignature = dwFlags & CF_VERIFY_SIG;

    if (dwFlags & CF_CERT_FROM_FILE)
    {
        if (cbCertificate < CERT_HEADER_LEN + 1 )
        {
            return (FALSE);
        }

        //
        // Sleazy quick check.  Some CAs wrap certs in a cert wrapper.
        // Some don't.  Some do both, but we won't mention any names.
        // Quick check for the wrapper tag.  If so, scoot in by enough
        // to bypass it (17 bytes. Sheesh).
        //

        if ( memcmp( pbCertificate + 4, CertTag, sizeof( CertTag ) ) == 0 )
        {
            pbCertificate += CERT_HEADER_LEN;
            cbCertificate -= CERT_HEADER_LEN;
        }
    }

    fSuccess = X509DecodeCertificate(pbCertificate, cbCertificate, &pPctCert);

    if (fSuccess == FALSE)
    {
        goto done;
    }

    if(VerifySignature == TRUE)
    {
        pSigSys = SigFromSpec(pPctCert->SigType, SP_PROT_ALL);

        if ((pSigSys == NULL)  ||
            (pPctCert == NULL) ||
            (pPctCert->pIssuerCertificate == NULL) ||
            (pPctCert->pIssuerCertificate->pPublicKey == NULL)
            )
        {
            fSuccess = FALSE;
            goto done;
        }

        fSuccess = pSigSys->Verify(
            pPctCert->pSignedPortion,
            pPctCert->cbSignedPortion,
            pPctCert->pSig,
            pPctCert->cbSig,
            pPctCert->pIssuerCertificate->pPublicKey
            );

        if (fSuccess == FALSE)
        {
            goto done;
        }
    }

    ASSERT(fSuccess == TRUE);

    pX509Cert = (PX509Certificate)SPExternalAlloc(sizeof(X509Certificate));

    if (pX509Cert == NULL)
    {
        fSuccess = FALSE;
        goto done;
    }

    memcpy(pX509Cert, pPctCert->pCert, sizeof(X509Certificate));

    pX509Cert->pszIssuer  = (PSTR)SPExternalAlloc(
        strlen(((PX509Certificate)pPctCert->pCert)->pszIssuer) + 1
        );
    pX509Cert->pszSubject = (PSTR)SPExternalAlloc(
        strlen(((PX509Certificate)pPctCert->pCert)->pszSubject) + 1
        );

    if ((pX509Cert->pszIssuer == NULL) ||
        (pX509Cert->pszSubject == NULL)
        )
    {
        fSuccess = FALSE;
        goto done;
    }

    strcpy(pX509Cert->pszIssuer,  ((PX509Certificate)pPctCert->pCert)->pszIssuer);
    strcpy(pX509Cert->pszSubject, ((PX509Certificate)pPctCert->pCert)->pszSubject);

    *ppCertificate = pX509Cert;

done:

    // Always delete the PctCertificate.
    if (pPctCert)
    {
        X509DeleteCert(pPctCert);
    }

    if (fSuccess == FALSE)
    {
        SslFreeCertificate(pX509Cert);
    }

    return (fSuccess);
}

//+---------------------------------------------------------------------------
//
//  Function:   SslLoadCertificate
//
//  Synopsis:   Not supported.
//
//  Notes:
//
//----------------------------------------------------------------------------
BOOL
WINAPI
SslLoadCertificate(
    PUCHAR      pbCertificate,
    DWORD       cbCertificate,
    BOOL        AddToWellKnownKeys)
{
    return FALSE;
}

BOOL IsEncryptionPermitted(VOID);

/*++

Function:       SslSetProtocols

Description:

Arguments:

Returns:

History:        mm-dd-yyyy   Author    Comment
                7/14/1997    sholden   author

Comments:

--*/

BOOL
WINAPI
SslSetProtocols(
    DWORD rgbitsProtocols
    )
{
    BOOL fSuccess;

    if (rgbitsProtocols & ~(SP_PROT_PCT1 | SP_PROT_SSL2 | SP_PROT_SSL3))
    {
        return (FALSE);
    }
#ifndef SCHANNEL_PCT
    // quietly disable PCT
    rgbitsProtocols &= ~SP_PROT_PCT1;
#endif

    // Enable unified hello if more than one protocol is enabled.
    if (rgbitsProtocols)
    {
        rgbitsProtocols |= SP_PROT_UNI;
    }

    // Enable "credential" protocol.
    rgbitsProtocols |= SP_PROT_CRED;

    // BUGBUG: add critical section??
    g_ProtEnabled   = rgbitsProtocols;
    g_ProtSupported = rgbitsProtocols;
    // Should always return TRUE. Will disable PCT in France.
    fSuccess = IsEncryptionPermitted();
    return (fSuccess);
}

/*++

Function:       SslGetProtocols

Description:

Arguments:

Returns:

History:        mm-dd-yyyy   Author    Comment
                7/14/1997    sholden   author

Comments:

--*/

DWORD
WINAPI
SslGetProtocols()
{
    return (g_ProtSupported);
}
