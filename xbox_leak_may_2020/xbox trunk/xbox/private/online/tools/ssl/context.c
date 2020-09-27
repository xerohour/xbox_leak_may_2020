/*-----------------------------------------------------------------------------
* Copyright (C) Microsoft Corporation, 1995 - 1996.
* All rights reserved.
*
* This file is part of the Microsoft Private Communication Technology
* reference implementation, version 1.0
*
* The Private Communication Technology reference implementation, version 1.0
* ("SPRef"), is being provided by Microsoft to encourage the development and
* enhancement of an open standard for secure general-purpose business and
* personal communications on open networks.  Microsoft is distributing SPRef
* at no charge irrespective of whether you use SPRef for non-commercial or
* commercial use.
*
* Microsoft expressly disclaims any warranty for SPRef and all derivatives of
* it.  SPRef and any related documentation is provided "as is" without
* warranty of any kind, either express or implied, including, without
* limitation, the implied warranties or merchantability, fitness for a
* particular purpose, or noninfringement.  Microsoft shall have no obligation
* to provide maintenance, support, upgrades or new releases to you or to anyone
* receiving from you SPRef or your modifications.  The entire risk arising out
* of use or performance of SPRef remains with you.
*
* Please see the file LICENSE.txt,
* or http://SP.microsoft.com/SP/SPlicen.txt
* for more information on licensing.
*
* Please see http://SP.microsoft.com/SP/SP.htm for The Private
* Communication Technology Specification version 1.0 ("SP Specification")
*
* 1/23/96
*----------------------------------------------------------------------------*/


/* TBD: add redo capability? */

#include <spbase.h>
#include <pct1msg.h>
#include <ssl2msg.h>
#include <pct1prot.h>
#include <ssl2prot.h>
#include <certmap.h>


DWORD g_cContext; // = 0;

/*
 *
 * Creation and Deletion of Context Objects.
 *
 */

/************************************************************************
* SPContext SPCreateContext(void)
*
* Create a new SPContext, and initialize it.
*
* Returns - PSPContext pointer to context object.
*
\***********************************************************************/

PSPContext SPContextCreate(PUCHAR pszTarget)
{

    PSPContext pContext;

    SP_BEGIN("SPCreateContext");

    pContext = (PSPContext)SPExternalAlloc( sizeof(SPContext));
    if(!pContext)
    {
        SP_RETURN(NULL);
    }

    FillMemory(pContext, sizeof(SPContext), 0);

    pContext->Magic = SP_CONTEXT_MAGIC;
    pContext->Flags = 0;
    if(!SPCacheRetrieveNew(pszTarget, &pContext->RipeZombie))
    {
        SPExternalFree(pContext);
        SP_RETURN(NULL);
    }

    g_cContext++;
    SP_RETURN(pContext);
}


/************************************************************************
* VOID SPDeleteContext(PSPContext pContext)
*
* Delete an existing context object.
*
\***********************************************************************/

BOOL
SPContextDelete(PSPContext pContext)
{
    SP_BEGIN("SPDeleteContext");

	if(pContext == NULL || pContext->Magic != SP_CONTEXT_MAGIC) {
		DebugLog((DEB_WARN, "Attempt to delete invalid context\n"));
		SP_RETURN(FALSE);
	}

    if(pContext->pbIssuerList)
    {
        SPExternalFree(pContext->pbIssuerList);
    }

    if (pContext->pReadState)
    {
        pContext->pSystem->Discard( &pContext->pReadState );
    }

    if (pContext->pWriteState)
    {
        pContext->pSystem->Discard( &pContext->pWriteState );
    }

    if (pContext->pClHello)
    {
        SPExternalFree(pContext->pClHello);
    }
    
    SPDereferenceCredential(pContext->pCred);


    SPCacheDereference(pContext->RipeZombie);

    FillMemory( pContext, sizeof( SPContext ), 0 );
    g_cContext--;

    SPExternalFree( pContext );
    SP_RETURN(TRUE);
}



/************************************************************************
* SPContext SPContextSetCredentials
*
* Associate a set of credentials with a context.
*
* Returns - PSPContext pointer to context object.
*
\***********************************************************************/
SP_STATUS
SPContextSetCredentials(
    PSPContext pContext,
    PSPCredential  pCred)
{

    SP_BEGIN("SPContextSetCredentials");

    if(pContext->Magic != SP_CONTEXT_MAGIC)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }

    // Reference the new credential
    if(pCred)
    {
        if(!SPReferenceCredential(pCred))
        {
            SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
        }
    }

    // Dereference the credential we are replacing.
    if(pContext->pCred)
    {
        SPDereferenceCredential(pContext->pCred);
        pContext->pCred = pCred;
    }
    else
    {
        pContext->pCred = pCred;
        pContext->RipeZombie->fProtocol = pCred->Type;
        switch(pContext->RipeZombie->fProtocol)
        {
            case SP_PROT_UNI_CLIENT:
            case SP_PROT_UNI_SERVER:
            case SP_PROT_PCT1_CLIENT:
            case SP_PROT_PCT1_SERVER:
            case SP_PROT_SSL2_CLIENT:
            case SP_PROT_SSL2_SERVER:
            case SP_PROT_SSL3_CLIENT:
            case SP_PROT_SSL3_SERVER:

                pContext->ProtocolHandler = ServerProtocolHandler;
                pContext->InitiateHello =  GenerateHello;
                break;

            default:
                return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);

        }
    }

    SP_RETURN(PCT_ERR_OK);

}


SP_STATUS
ContextInitCiphers(SPContext *pContext)
{

    SP_BEGIN("ContextInitCiphers");


    if((pContext == NULL) ||
        (pContext->RipeZombie == NULL))
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR));
    }

    pContext->pSystem = CipherFromSpec(
                            pContext->RipeZombie->SessCiphSpec,
                            pContext->RipeZombie->fProtocol | (pContext->RipeZombie->fSGC ? SP_PROT_SGC : 0));
    if (NULL == pContext->pSystem)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_SPECS_MISMATCH));
    }

    pContext->pCheck = HashFromSpec(pContext->RipeZombie->SessHashSpec, pContext->RipeZombie->fProtocol);
    if (NULL == pContext->pCheck)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_SPECS_MISMATCH));
    }

    pContext->pKeyExch = KeyExchangeFromSpec(pContext->RipeZombie->SessExchSpec, pContext->RipeZombie->fProtocol);
    if (NULL == pContext->pKeyExch)
    {
        SP_RETURN(SP_LOG_RESULT(PCT_INT_SPECS_MISMATCH));
    }

    /* we want to minimize allocations, so we'll do one here by starting */
    /* up the initMACstate using the regular hashing functions.  We'll then */
    /* clone the hash state and throw away the allocated one.  This lets */
    /* us get rid of a few alloc'ed hashes in the future. */
    pContext->InitMACState = (PCheckSumBuffer)(pContext->InHSHBuf);
    pContext->pCheck->Initialize(pContext->InitMACState, 0);

    SP_RETURN(PCT_ERR_OK);

}


SP_STATUS
SPContextDoMapping(
    PSPContext pContext)
{
    PSessCacheItem      pZombie;
    PSPCredential       pCred;
//    DWORD               cbCert;
//    PUCHAR              pCert;
//    SP_STATUS           pctRet;
    DWORD               iMapper;
    MAPPER_X509_ASN_CHAIN *pChain;
    DWORD               cCerts;
    DWORD               cbChain;

    pZombie = pContext->RipeZombie;
    pCred = pContext->pCred;


    if(pCred->cMappers)
    {

        PPctCertificate pThis;
        X509Certificate *pX509Cert;
        DWORD iCert;
        // we have a cert mapper, so now we
        // build a credential and then call the
        // cert mapper

        cCerts = 0;

        pThis = pZombie->pClientCert;
        while(pThis)
        {
            cCerts++;
            if(pThis == pThis->pIssuerCertificate)
            {
                break;
            }
            pThis = pThis->pIssuerCertificate;
        }
        cbChain = sizeof(MAPPER_X509_ASN_CHAIN)+sizeof(MAPPER_BLOB)*cCerts;

        pChain = (MAPPER_X509_ASN_CHAIN *)SPExternalAlloc(cbChain);

        if(pChain == NULL)
        {
            return(SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY));
        }
        pChain->cCerts = cCerts;

        iCert = 0;
        pThis = pZombie->pClientCert;
        while(pThis)
        {
            pChain->aCerts[iCert].cbBytes = pThis->cbRawCert;
            pChain->aCerts[iCert].pBytes = pThis->pRawCert;

            iCert++;
            if(pThis == pThis->pIssuerCertificate)
            {
                break;
            }
            pThis = pThis->pIssuerCertificate;
        }
        if(NULL == pThis)
        {
            pChain->dwFlags |= X509_UNKNOWN_ISSUER;
        }
        pThis = pZombie->pClientCert;
        pX509Cert = (X509Certificate *)pThis->pCert;
        pChain->ValidFrom = pThis->ValidFrom;
        pChain->ValidUntil = pThis->ValidUntil;
        pChain->SerialNumber[0] = pX509Cert->SerialNumber[0];
        pChain->SerialNumber[1] = pX509Cert->SerialNumber[1];
        pChain->SerialNumber[2] = pX509Cert->SerialNumber[2];
        pChain->SerialNumber[3] = pX509Cert->SerialNumber[3];
        pChain->pIssuer = pThis->pbIssuer;
        pChain->cbIssuer = pThis->cbIssuer;
        pChain->pSubject = pThis->pbSubject;
        pChain->cbSubject = pThis->cbSubject;

        for(iMapper = 0; iMapper < pCred->cMappers; iMapper++)
        {
            if(pCred->pahMappers[iMapper]->m_vtable->MapCredential(pCred->pahMappers[iMapper],
                                        X509_ASN_CHAIN,
                                        (PUCHAR)pChain,
                                        cbChain,
                                        &pZombie->hLocator))
            {
                pZombie->phMapper = pCred->pahMappers[iMapper];
                pZombie->phMapper->m_vtable->ReferenceMapper(pZombie->phMapper);
                break;
            }
        }
        SPExternalFree(pChain);

    }
    return PCT_ERR_OK;
}

SP_STATUS
SPContextGetIssuers(
    PSPContext pContext,
    PBYTE pbIssuers,
    DWORD *pcbIssuers)
{
    PSPCredential       pCred;
    DWORD               iMapper;
    DWORD               cbIssuerLen;
    DWORD               cbTotalIssuerLen = 0;
    PBYTE               pbCurIssuer;



    pCred = pContext->pCred;


    if(pCred->cMappers)
    {
        pbCurIssuer = pbIssuers;
        for(iMapper = 0; iMapper < pCred->cMappers; iMapper++)
        {
            cbIssuerLen = (*pcbIssuers) - cbTotalIssuerLen;
            if(pCred->pahMappers[iMapper]->m_vtable->GetIssuerList(pCred->pahMappers[iMapper],
                                                                    NULL,
                                                                    (PUCHAR)pbCurIssuer,
                                                                    &cbIssuerLen))
            {

                cbTotalIssuerLen += cbIssuerLen;
                if(pbIssuers)
                {
                    pbCurIssuer += cbIssuerLen;
                }
            }
        }
    }
    else
    {
        // Issuer name
        if(!GetDefaultIssuers(pbIssuers, pcbIssuers))
        {
    	    return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
        }
    }
    return PCT_ERR_OK;
}

/*
 *
 * Misc Utility functions.
 *
 */



#if DBG
typedef struct _DbgMapCrypto {
    DWORD   C;
    PSTR    psz;
} DbgMapCrypto;

DbgMapCrypto    DbgCryptoNames[] = { {SP_CIPHER_RC4, "RC4 "},
};

CHAR    DbgNameSpace[100];
PSTR    DbgAlgNames[] = { "Basic RSA", "RSA with MD2", "RSA with MD5", "RC4 stream"};
#define AlgName(x) ((x < sizeof(DbgAlgNames) / sizeof(PSTR)) ? DbgAlgNames[x] : "Unknown")

PSTR
DbgGetNameOfCrypto(DWORD x)
{
    int i;
    for (i = 0; i < sizeof(DbgCryptoNames) / sizeof(DbgMapCrypto) ; i++ )
    {
        if ((x & SP_CIPHER_ALG) == DbgCryptoNames[i].C)
        {
            sprintf(DbgNameSpace, "%s %d / %d MACbits",
                    (DbgCryptoNames[i].psz),
                    (x & SP_CIPHER_STRENGTH) >> SP_CSTR_POS,
                    (x & SP_CIPHER_MAC));
            return DbgNameSpace;
        }
    }

    return("Unknown");
}
#endif

/* allow quick initialization of hashes */

void InitHashBuf(HashBuf Buf,
                 PSPContext pContext
                 )
{
    CloneHashBuf(Buf, pContext->InitMACState, pContext->pCheck);

    return;
}

PSTR
CopyString(
           PSTR        pszString)
{
    PSTR    pszNewString;
    DWORD   cchString;

    cchString = strlen(pszString) + 1;

    pszNewString = (PSTR)SPExternalAlloc(cchString);

    if (pszNewString)
    {
        CopyMemory(pszNewString, pszString, cchString);
    }

    return(pszNewString);
}




#ifdef LTS

int __cdecl memcmp (
        const void * buf1,
        const void * buf2,
        size_t count
        )
{
        if (!count)
                return(0);

        while ( --count && *(char *)buf1 == *(char *)buf2 ) {
                buf1 = (char *)buf1 + 1;
                buf2 = (char *)buf2 + 1;
        }

        return( *((unsigned char *)buf1) - *((unsigned char *)buf2) );
}

#endif
