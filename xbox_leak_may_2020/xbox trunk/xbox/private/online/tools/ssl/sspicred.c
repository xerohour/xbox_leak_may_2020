//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1995.
//
//  File:       cred.c
//
//  Contents:
//
//  Classes:
//
//  Functions:
//
//  History:    8-07-95   RichardW   Created
//
//----------------------------------------------------------------------------

#include <spbase.h>
#include <security.h>
#include "spsspi.h"

#include "hack.h"

#define lstrcmpiA stricmp

#define LockCredential(p)   EnterCriticalSection(&((PSPCredential) p)->csLock)
#define UnlockCredential(p) LeaveCriticalSection(&((PSPCredential) p)->csLock)

typedef struct _PACKAGEMAP {
    DWORD Type;
    SEC_CHAR SEC_FAR * pszPackageName;
    SEC_WCHAR SEC_FAR * wszPackageName;
} PACKAGEMAP;

const PACKAGEMAP SupportedClientPackages[] = {
    { SP_PROT_UNI_CLIENT, UNISP_NAME_A, UNISP_NAME_W },
#ifdef SCHANNEL_PCT
    { SP_PROT_PCT1_CLIENT, PCT1SP_NAME_A, PCT1SP_NAME_W },
#endif
    { SP_PROT_SSL3_CLIENT, SSL3SP_NAME_A, SSL3SP_NAME_W },
    { SP_PROT_SSL2_CLIENT, SSL2SP_NAME_A, SSL2SP_NAME_W }
};

#ifndef SCHANNEL_CLIENT_ONLY
const PACKAGEMAP SupportedServerPackages[] = {
    { SP_PROT_UNI_SERVER, UNISP_NAME_A, UNISP_NAME_W },
#ifdef SCHANNEL_PCT
    { SP_PROT_PCT1_SERVER, PCT1SP_NAME_A, PCT1SP_NAME_W },
#endif
    { SP_PROT_SSL3_SERVER, SSL3SP_NAME_A, SSL3SP_NAME_W },
    { SP_PROT_SSL2_SERVER, SSL2SP_NAME_A, SSL2SP_NAME_W }
};
#endif    

SECURITY_STATUS SEC_ENTRY
AcquireCredentialsHandleA(
    SEC_CHAR SEC_FAR *          pszPrincipal,       // Name of principal
    SEC_CHAR SEC_FAR *          pszPackageName,     // Name of package
    unsigned long               fCredentialUse,     // Flags indicating use
    void SEC_FAR *              pvLogonId,          // Pointer to logon ID
    void SEC_FAR *              pAuthData,          // Package specific data
    SEC_GET_KEY_FN              pGetKeyFn,          // Pointer to GetKey() func
    void SEC_FAR *              pvGetKeyArgument,   // Value to pass to GetKey()
    PCredHandle                 phCredential,       // (out) Cred Handle
    PTimeStamp                  ptsExpiry           // (out) Lifetime (optional)
    )
{
    SP_STATUS           pctRet;
    PSPCredential      pCred;
    SCH_CRED            CredData;
    SCH_CRED_PUBLIC_CERTCHAIN  Cert;
    SCH_CRED_SECRET_PRIVKEY    Key;
    PSSL_CREDENTIAL_CERTIFICATE  pSslCert;

    PSCH_CRED_PUBLIC_CERTCHAIN aCerts[1] = {&Cert};
    PSCH_CRED_SECRET_PRIVKEY aKeys[1] = {&Key};

    PVOID               pData;
    DWORD               Type = 0;
    DWORD               i;

    SP_BEGIN("AcquireCredentialsHandleA");

    pData = pAuthData;
    FillMemory(&CredData, sizeof(CredData), 0);
    if(pszPackageName == NULL)
    {
        SP_RETURN(SEC_E_SECPKG_NOT_FOUND);
    }
    if(0 == (fCredentialUse & SECPKG_CRED_INBOUND))
    {
        for(i=0; i< sizeof(SupportedClientPackages)/sizeof(PACKAGEMAP); i++ ) {
            if(!lstrcmpiA(pszPackageName, SupportedClientPackages[i].pszPackageName)) {
                Type = SupportedClientPackages[i].Type;
                break;
            }
        }
    }
    else
    {
#ifndef SCHANNEL_CLIENT_ONLY
        for(i=0; i< sizeof(SupportedServerPackages)/sizeof(PACKAGEMAP); i++ ) {
            if(!lstrcmpiA(pszPackageName, SupportedServerPackages[i].pszPackageName)) {
                Type = SupportedServerPackages[i].Type;
                break;
            }
        }
#endif        
    }
    if(Type == 0) {
        SP_RETURN(SEC_E_SECPKG_NOT_FOUND);
    }

    // HACK to make the old style credentials work
    if((pAuthData) &&
        (fCredentialUse & SECPKG_CRED_INBOUND) &&
        (*(DWORD *)pAuthData != SCH_CRED_VERSION))
    {
        pSslCert = pAuthData;
        pData = &CredData;
        CredData.dwVersion = SCH_CRED_VERSION;
        CredData.cCreds = 1;
        CredData.paSecret = aKeys;
        CredData.paPublic = aCerts;
        Cert.dwType = SCH_CRED_X509_CERTCHAIN;
        Cert.pCertChain = pSslCert->pCertificate + 17;
        Cert.cbCertChain = pSslCert->cbCertificate - 17;

        Key.dwType = SCHANNEL_SECRET_PRIVKEY;
        Key.pPrivateKey = pSslCert->pPrivateKey;
        Key.cbPrivateKey = pSslCert->cbPrivateKey;
        Key.pszPassword = pSslCert->pszPassword;

    }

    pctRet = SPCreateCredential(&pCred,
                                 Type,
                                 pData);

    if (PCT_ERR_OK == pctRet)
    {
        phCredential->dwUpper = (DWORD) pCred;
        phCredential->dwLower = i;
        SP_RETURN(SEC_E_OK);

    }
    SP_RETURN(PctTranslateError(pctRet));
}





SECURITY_STATUS SEC_ENTRY
AcquireCredentialsHandleW(
    SEC_WCHAR SEC_FAR *         pszPrincipal,       // Name of principal
    SEC_WCHAR SEC_FAR *         pszPackageName,     // Name of package
    unsigned long               fCredentialUse,     // Flags indicating use
    void SEC_FAR *              pvLogonId,          // Pointer to logon ID
    void SEC_FAR *              pAuthData,          // Package specific data
    SEC_GET_KEY_FN              pGetKeyFn,          // Pointer to GetKey() func
    void SEC_FAR *              pvGetKeyArgument,   // Value to pass to GetKey()
    PCredHandle                 phCredential,       // (out) Cred Handle
    PTimeStamp                  ptsExpiry           // (out) Lifetime (optional)
    )
{
    PCHAR   pszAnsiPrincipal;
    PCHAR   szPackage;
    DWORD   cchPackage;
    DWORD   cchPrincipal;
    SECURITY_STATUS scRet;


    if(pszPackageName == NULL) return SEC_E_SECPKG_NOT_FOUND;
    if (pszPrincipal)
    {
        cchPrincipal = lstrlenW(pszPrincipal) + 1;
        pszAnsiPrincipal = SPExternalAlloc(cchPrincipal * sizeof(WCHAR));
        if (pszAnsiPrincipal == NULL)
        {
            return(SEC_E_INSUFFICIENT_MEMORY);
        }

        WideCharToMultiByte(
                    CP_ACP,
                    0,
                    pszPrincipal,
                    cchPrincipal,
                    pszAnsiPrincipal,
                    cchPrincipal * sizeof(WCHAR),
                    NULL,
                    NULL);
    }
    else
    {
        pszAnsiPrincipal = NULL;
    }
    cchPackage = lstrlenW(pszPackageName) + 1;
    szPackage = SPExternalAlloc(cchPackage * sizeof(WCHAR));
    if (szPackage == NULL)
    {
        scRet = SEC_E_INSUFFICIENT_MEMORY;
        goto error;
    }

    WideCharToMultiByte(
                CP_ACP,
                0,
                pszPackageName,
                cchPackage,
                szPackage,
                cchPackage * sizeof(WCHAR),
                NULL,
                NULL);

    scRet = AcquireCredentialsHandleA(
                        pszAnsiPrincipal,
                        szPackage,
                        fCredentialUse,
                        pvLogonId,
                        pAuthData,
                        pGetKeyFn,
                        pvGetKeyArgument,
                        phCredential,
                        ptsExpiry);

    SPExternalFree(szPackage);
error:
    if (pszAnsiPrincipal != NULL)
    {
        SPExternalFree(pszAnsiPrincipal);
    }
    return(scRet);
}


SECURITY_STATUS SEC_ENTRY
FreeCredentialsHandle(
    PCredHandle                 phCredential        // Handle to free
    )
{
    PSPCredential  pCred;

    pCred = ValidateCredentialHandle(phCredential);

    if (pCred)
    {
        SPDereferenceCredential(pCred);
        return(SEC_E_OK);
    }

    return(SEC_E_INVALID_HANDLE);

}

SECURITY_STATUS SEC_ENTRY
QueryCredentialsAttributes(
    PCredHandle phCredential,
    ULONG ulAttribute,
    PVOID pBuffer
    )
{
    PSPCredential   pCred;
    SECURITY_STATUS error = 0;

    pCred = ValidateCredentialHandle(phCredential);

    if (pCred == NULL)
    {
        return (SEC_E_INVALID_HANDLE);
    }

    switch (ulAttribute)
    {
        case SECPKG_ATTR_SUPPORTED_PROTOCOLS:
        {
            PSecPkgCred_SupportedProtocols pProtocols = pBuffer;

            if (pBuffer == NULL)
            {
                error = SEC_E_INVALID_HANDLE;
            }
            else
            {
                pProtocols->grbitProtocol = g_ProtEnabled;
            }

        }
            break;

        default:

            error = SEC_E_INVALID_TOKEN;
            break;
    }

    return (error);
}


PSPCredential
ValidateCredentialHandle(
    PCredHandle     phCred)
{
    BOOL            fReturn;
    PSPCredential  pCred;
    SP_BEGIN("PctpValidateCredentialHandle");

    fReturn = FALSE;

    if (phCred)
    {
#ifdef USE_EXCEPTIONS
        try
        {
#endif
            pCred = (PSPCredential)phCred->dwUpper;
            if(pCred == NULL)
            {
                SP_RETURN(NULL);
            }
            if (pCred->Magic == PCT_CRED_MAGIC)
            {
                fReturn = 1;
            }
#ifdef USE_EXCEPTIONS
        }
#ifdef SECURITY_LINUX
        catch(...)
#else
        except (EXCEPTION_EXECUTE_HANDLER)
#endif
        {
            pCred = NULL;
        }
#endif /* USE_EXCEPTIONS */
    }

    if (fReturn)
    {
        SP_RETURN(pCred);
    }

    SP_RETURN(NULL);
}


