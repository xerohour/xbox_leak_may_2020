/*-----------------------------------------------------------------------------
* Copyright (C) Microsoft Corporation, 1995 - 1996.
* All rights reserved.
*
* This file is part of the Microsoft Private Communication Technology
* reference implementation, version 1.0
*
* The Private Communication Technology reference implementation, version 1.0
* ("PCTRef"), is being provided by Microsoft to encourage the development and
* enhancement of an open standard for secure general-purpose business and
* personal communications on open networks.  Microsoft is distributing PCTRef
* at no charge irrespective of whether you use PCTRef for non-commercial or
* commercial use.
*
* Microsoft expressly disclaims any warranty for PCTRef and all derivatives of
* it.  PCTRef and any related documentation is provided "as is" without
* warranty of any kind, either express or implied, including, without
* limitation, the implied warranties or merchantability, fitness for a
* particular purpose, or noninfringement.  Microsoft shall have no obligation
* to provide maintenance, support, upgrades or new releases to you or to anyone
* receiving from you PCTRef or your modifications.  The entire risk arising out
* of use or performance of PCTRef remains with you.
*
* Please see the file LICENSE.txt,
* or http://pct.microsoft.com/pct/pctlicen.txt
* for more information on licensing.
*
* Please see http://pct.microsoft.com/pct/pct.htm for The Private
* Communication Technology Specification version 1.0 ("PCT Specification")
*
* 1/23/96
*----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <spbase.h>
#include <wincrypt.h>


#ifdef MULTI_THREADED
#define LockCredential(p)   EnterCriticalSection(&((PSPCredential) p)->csLock)
#define UnlockCredential(p) LeaveCriticalSection(&((PSPCredential) p)->csLock)
#else
#define LockCredential(p)
#define UnlockCredential(p)
#endif

SP_STATUS
UnpackCredSecret(
    PPctPrivateKey *ppOutKey,
    PVOID pSecret
    );

SP_STATUS
UnpackCredCertificate(
    PPctCertificate *ppOutCert,
    PVOID pCertificate
    );



SP_STATUS
SPCreateCredential(
   PSPCredential *ppCred,
   DWORD Type,
   PSCH_CRED pCredData)
{
    PSPCredential  pCred = NULL;
    SP_STATUS       Result;
    PUCHAR          pPrivateKeySave = NULL;


    SP_BEGIN("SPCreateCredential");



    pCred = (PSPCredential)SPExternalAlloc(sizeof(SPCredential));

    if(pCred == NULL)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY));
    }

    pCred->Magic = PCT_CRED_MAGIC;
    pCred->Type = Type;

#ifdef MULTI_THREADED
    InitializeCriticalSection(&pCred->csLock);
#endif

    pCred->RefCount = 0;
    pCred->cCerts = 0;
    pCred->paCerts = NULL;
    pCred->paPrivateKeys = NULL;
    pCred->cMappers = 0;
    pCred->pahMappers = NULL;
    Result = PCT_ERR_OK;
    pCred->Flags = 0;



    if(pCredData != NULL)
    {

        switch(pCredData->dwVersion)
        {
            case SCH_CRED_VERSION:
            pCred->cMappers = pCredData->cMappers;
            if(pCred->cMappers)
            {
                pCred->pahMappers = SPExternalAlloc(pCred->cMappers*sizeof(HMAPPER *));
                if(pCred->pahMappers == NULL)
                {
                    Result = SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY);
                    goto error;
                }
                CopyMemory(pCred->pahMappers, pCredData->aphMappers, pCred->cMappers * sizeof(HMAPPER *));
            }

            // fall through to v1 cred
            case SCH_CRED_V1:  // Our current version.
            {
                DWORD iCertData, iCred;

                // Allocate the certificate array.
                pCred->paCerts =
                    (PctCertificate **)SPExternalAlloc(sizeof(PctCertificate *) *
                                                             pCredData->cCreds);

                if(!pCred->paCerts)
                {
                    Result = SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY);
                    goto error; // bail
                }


                // Allocate memory for the private keys.
                pCred->paPrivateKeys =
                    (PctPrivateKey **)SPExternalAlloc(sizeof(PctPrivateKey *) *
                                                             pCredData->cCreds);

                ZeroMemory(pCred->paPrivateKeys, sizeof(PctPrivateKey *) *
                                                             pCredData->cCreds);

                if(!pCred->paPrivateKeys)
                {
                    Result = SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY);
                    goto error; // Leave polish loop
                }

                pCred->cCerts = pCredData->cCreds;
                // Loop through each credential, unpacking it, and adding it to our list.
                for (iCertData = 0, iCred = 0; iCertData < pCredData->cCreds; iCertData++)
                {
                    BYTE aVerifyClean[16];
                    BYTE aVerifySigned[255];
                    DWORD dwSigned;
                    DWORD dwClean;
                    const SignatureSystem *System;

                    Result = UnpackCredSecret(&pCred->paPrivateKeys[iCred], pCredData->paSecret[iCertData]);

                    if(Result != PCT_ERR_OK)
                    {
                        goto error;
                    }

                    Result = UnpackCredCertificate(&pCred->paCerts[iCred], pCredData->paPublic[iCertData]);
                    if(Result != PCT_ERR_OK)
                    {
                        goto error;
                    }

                    System = SigFromSpec(SP_SIG_RSA_MD5, SP_PROT_ALL);

                    if(System == NULL)
                    {
                        Result = SP_LOG_RESULT(PCT_INT_UNKNOWN_CREDENTIAL);
                        goto error;
                    }

                    // Verify that the credential and secret match.
                    dwClean = sizeof(aVerifyClean);
                    dwSigned = sizeof(aVerifySigned);
                    GenerateRandomBits(aVerifyClean, dwClean);
                    if(!System->Sign(aVerifyClean,
                                    dwClean,
                                    aVerifySigned,
                                    &dwSigned,
                                    pCred->paPrivateKeys[iCred]))
                    {
                        Result = SP_LOG_RESULT(PCT_INT_UNKNOWN_CREDENTIAL);
                        goto error;
                    }


                    if(!System->Verify(aVerifyClean,
                                    dwClean,
                                    aVerifySigned,
                                    dwSigned,
                                    pCred->paCerts[iCred]->pPublicKey))
                    {
                        Result = SP_LOG_RESULT(PCT_INT_UNKNOWN_CREDENTIAL);
                        goto error;
                    }

                    iCred++;
                }
                pCred->cCerts = iCred;
                break;
            }
            default:
                Result = SP_LOG_RESULT(PCT_INT_UNKNOWN_CREDENTIAL);
                goto error;

        }
    }

    SPReferenceCredential(pCred);
    *ppCred = pCred;
    SP_RETURN(PCT_ERR_OK);


error:
    // Error case, free the credential
    SPDeleteCredential(pCred);
    SP_RETURN(Result);
}


BOOL
SPDeleteCredential(
    PSPCredential  pCred)
{
    DWORD i;

    SP_BEGIN("SPDeleteCredential");


#ifdef MULTI_THREADED
    DeleteCriticalSection(&pCred->csLock);
#endif



    for (i = 0; i < pCred->cCerts; i++)
    {
       if(pCred->paPrivateKeys[i])
       {
          if(pCred->paPrivateKeys[i])
          {
              FillMemory(pCred->paPrivateKeys[i], pCred->paPrivateKeys[i]->cbKey, 0);
              SPExternalFree(pCred->paPrivateKeys[i]);
              pCred->paPrivateKeys[i]=NULL;
          }
        }

        if(pCred->paCerts[i])
        {
            DereferenceCert(pCred->paCerts[i]);
            pCred->paCerts[i]=NULL;
        }
    }
    if(pCred->cMappers && pCred->pahMappers)
    {
        for(i=0; i < pCred->cMappers; i++)
        {
            pCred->pahMappers[i]->m_vtable->DeReferenceMapper(pCred->pahMappers[i]);
        }
        SPExternalFree(pCred->pahMappers);
    }


    pCred->Magic = PCT_INVALID_MAGIC;

    SPExternalFree(pCred);
    SP_RETURN(TRUE);
}


// Reference a credential.
// Note: This should only be called by someone who already
// has a reference to the credential, or by the CreateCredential
// call.

BOOL
SPReferenceCredential(
    PSPCredential  pCred)
{
    BOOL fRet = FALSE;
    SP_BEGIN("PctReferenceCredential");

    fRet =  (InterlockedIncrement(&pCred->RefCount) > 0);

    DebugLog((SP_LOG_TRACE, "Reference Cred %lx: %d\n", pCred, pCred->RefCount));
    SP_RETURN(fRet);
}


BOOL
SPDereferenceCredential(
    PSPCredential  pCred)
{
//    LONG    Ref;
    BOOL fRet = FALSE;

    SP_BEGIN("PctDereferenceCredential");
    if(pCred == NULL)
    {
        return FALSE;
    }
    if(pCred->Magic != PCT_CRED_MAGIC)
    {
        return FALSE;
    }

    if(0 >= pCred->RefCount)
    {
        return FALSE;
    }
    fRet = TRUE;

    DebugLog((SP_LOG_TRACE, "Reference Cred %lx: %d\n", pCred, pCred->RefCount-1));

    if(0 ==  InterlockedDecrement(&pCred->RefCount))
    {
	    fRet = SPDeleteCredential(pCred);
    }

    SP_RETURN(fRet);

}


SP_STATUS
UnpackCredSecret(
    PPctPrivateKey *ppOutKey,
    PVOID pSecret
    )
{

    DWORD          dwType;
    PPctPrivateKey pResult;
//    DWORD Result;

    /*
     * We accept the following kinds of keys
     * in the pKeyData array.
     * 1) Der encoded private key
     * 2) PCT_CAPI_PRIVATE_KEY Structure
     * 3) WinSock2 LSP callback function
     */

    if(pSecret == NULL)
    {
        return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
    }
    dwType = *(DWORD *)pSecret;

    switch(dwType)
    {
        case SCHANNEL_SECRET_TYPE_WINSOCK2:
        {
            pResult = (PPctPrivateKey)SPExternalAlloc((DWORD)(((PPctPrivateKey)NULL)->pKey)+sizeof(SCH_CRED_SECRET_WINSOCK2));
            if(pResult == NULL)
            {
                return SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY);
            }
            CopyMemory(pResult->pKey,
                       pSecret,
                       sizeof(SCH_CRED_SECRET_WINSOCK2));
            pResult->cbKey = sizeof(SCH_CRED_SECRET_WINSOCK2);
            pResult->Type = SP_EXCH_RSA_PKCS1;   // probably not used

            *ppOutKey = pResult;
            break;
        }

        case SCHANNEL_SECRET_TYPE_CAPI:
        {
            PSCH_CRED_SECRET_CAPI pCapiKey;

            pCapiKey = (PSCH_CRED_SECRET_CAPI)pSecret;

            pResult = (PPctPrivateKey)SPExternalAlloc(SP_OFFSET_OF(PPctPrivateKey, pKey)+sizeof(HCRYPTPROV));
            if(pResult == NULL)
            {
                return SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY);
            }
            *(HCRYPTPROV *)(pResult->pKey) = pCapiKey->hProv;
            pResult->cbKey = sizeof(HCRYPTPROV);
            pResult->Type = SP_EXCH_RSA_PKCS1;

            *ppOutKey = pResult;
            break;
        }

        case SCHANNEL_SECRET_PRIVKEY:
        {
            PUCHAR pPrivateKeySave;
            PSCH_CRED_SECRET_PRIVKEY pPrivKey;
            long lRet;

            pPrivKey = (PSCH_CRED_SECRET_PRIVKEY)pSecret;
            // Make a copy of the private key, as it is
            // decrypted in place, and we don't want
            // to damage the key.
            pPrivateKeySave = SPExternalAlloc(pPrivKey->cbPrivateKey);
            if(pPrivateKeySave == NULL)
            {
                return SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY);
            }

            CopyMemory(pPrivateKeySave,
                       pPrivKey->pPrivateKey,
                       pPrivKey->cbPrivateKey);

            lRet = DecodePrivateKeyFile( ppOutKey,
                                    pPrivateKeySave,
                                    pPrivKey->cbPrivateKey,
                                    pPrivKey->pszPassword );

            SPExternalFree(pPrivateKeySave);
            if(lRet < 0)
            {
                return SP_LOG_RESULT(PCT_INT_UNKNOWN_CREDENTIAL);
            }
            break;
        }
        default:
        {
            return SP_LOG_RESULT(PCT_INT_UNKNOWN_CREDENTIAL);
        }
    }



    return PCT_ERR_OK;
}

SP_STATUS
UnpackCredCertificate(
    PPctCertificate *ppOutCert,
    PVOID pCertificate
    )
{
    DWORD          dwType;
//    PPctCertificate pResult;
//    DWORD Result;

    if(pCertificate == NULL)
    {
        return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
    }
    dwType = *(DWORD *)pCertificate;


    switch(dwType)
    {
        case SCH_CRED_X509_CERTCHAIN:
        {
            PSCH_CRED_PUBLIC_CERTCHAIN pCertChain;
//            BOOL bRet;
//            PPctCertificate pResult, pThis, pChild;
//            PUCHAR pCurrentRaw;
//            long  cbCurrentRaw;

            pCertChain = (PSCH_CRED_PUBLIC_CERTCHAIN)pCertificate;
            /* Decode the certificate */
            return SPLoadCertificate(SP_PROT_CRED,
                                     SP_CERT_X509,
                                     pCertChain->pCertChain,
                                     pCertChain->cbCertChain,
                                     ppOutCert,
                                     NULL);
        }
        default:
        {
            return SP_LOG_RESULT(PCT_INT_UNKNOWN_CREDENTIAL);
        }
    }

    return PCT_ERR_OK;
}


SP_STATUS
SPCredPickCertificate(
    DWORD               fProtocol,
    PSPCredential       pCred,
    CertSpec *          aServerPref,
    DWORD               cServerPref,
    CertSpec *          aClientPref,
    DWORD               cClientPref,
    PPctCertificate *   ppCert,
    PPctPrivateKey  *   ppPrivateKey)
{

    DWORD i, j, k;

    /* pick a cert, any cert   */
    /* Server order of preference for certificates:
     * Hardcoded Preference (CertRank)
     *    Client Preference    (Hello)
     *        What server has     (pCred)
     */

    if(ppCert == NULL || ppPrivateKey == NULL)
    {
        return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
    }

    for(i=0; i < cServerPref; i++)
    {
        // Is this cert type enabled?
        if(NULL == CertFromSpec(aServerPref[i], fProtocol))
        {
            continue;
        }

        for(j=0; j<cClientPref; j++)
        {
            // Does the client want this type of cert
            if(aServerPref[i] != aClientPref[j])
            {
                continue;
            }

            for(k=0; k<pCred->cCerts; k++)
            {
                // Do we have one of these certs
                if(pCred->paCerts[k]->Spec == aClientPref[j])
                {
                    /* Set up the certificate */

                    // We're referencing the certificate for whomever
                    // we're passing it out to.
                    if(ReferenceCert(pCred->paCerts[k]) <= 0)
                    {
                        // We couldn't reference this cert, so try
                        // another
                        break;
                    }
                    // Now dereference the cert that we are replacing.
                    DereferenceCert(*ppCert);

                    *ppCert = pCred->paCerts[k];
                    *ppPrivateKey = pCred->paPrivateKeys[k];
                    return PCT_ERR_OK;

                }
            }
        }
    }

    return SP_LOG_RESULT(PCT_ERR_SPECS_MISMATCH)    ;
}
