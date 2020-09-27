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
#include <ssl2msg.h>
#include <ssl3msg.h>
#include <wincrypt.h>

const BYTE g_DER_OID_SERVER_GATED_CRYPTO[] = //1.3.6.1.4.1.311.10.3.3
    {0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x0a, 0x03, 0x03};
const BYTE g_DER_OID_NETSCAPE_SGC[] = //2.16.840.1.113730.4.1
    {0x60, 0x86, 0x48, 0x01, 0x86, 0xf8, 0x42, 0x04, 0x01};
extern PPctCertificate g_pGenevaRoot, g_pVerisign3Root;

SP_STATUS
SPLoadCertificate(
    DWORD      fProtocol,
    CertSpec   Spec,
    PUCHAR     pCertificate,
    DWORD      cbCertificate,
    PPctCertificate *ppCert,
    DWORD       *pdwSGC          // out,optional: 1 if SGC enabled
    )
{

    SP_STATUS           pctRet;
    PPctCertificate pResult, pThis, pChild;
    const CertSystem *  pCertSys = NULL;
    SignatureSystem *   pSigSys = NULL;
    PUCHAR pCurrentRaw;
    LONG cbCurrentRaw;

    pCertSys = CertFromSpec(Spec, fProtocol);

    if(ppCert == NULL)
    {
        return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
    }

    if(pCertSys == NULL)
    {
        return  SP_LOG_RESULT(PCT_INT_BAD_CERT);
    }


    // Dereference the cert that we are replacing.
    if (*ppCert)
    {
        DereferenceCert(*ppCert);
        *ppCert = NULL;
    }
    if (pdwSGC)
        *pdwSGC = 0;

    pResult = NULL;
    pChild = NULL;
    pCurrentRaw = pCertificate;
    cbCurrentRaw = cbCertificate;
    pctRet = PCT_ERR_BAD_CERTIFICATE;

    do {
        if(fProtocol & SP_PROT_SSL3)
        {
            // SSL3 style cert chain, where the length
            // of earch cert is prepended.
            pCurrentRaw += 3;
            cbCurrentRaw -= 3;
        }

        if(!pCertSys->Decode(pCurrentRaw,
                             cbCurrentRaw,
                             &pThis))
        {
            if (pResult)
            {
                DereferenceCert(pResult);
            }
            return SP_LOG_RESULT(PCT_ERR_BAD_CERTIFICATE);
        }
        // We successfully loaded this cert,
        // so move our pointers to the next cert
        // raw data.
        pCurrentRaw += pThis->cbRawCert;
        cbCurrentRaw -= pThis->cbRawCert;

        // Verify this certificates child with this certificate.
        if(pChild)
        {
            const SignatureSystem     *pSigSys = NULL;
            pSigSys = SigFromSpec(pChild->SigType, SP_PROT_ALL);
            if(pSigSys == NULL)
            {
                if (pThis) DereferenceCert(pThis);
                if (pResult) DereferenceCert(pResult);
                return SP_LOG_RESULT(PCT_ERR_BAD_CERTIFICATE);
            }


            if(!pSigSys->Verify(pChild->pSignedPortion,
                                pChild->cbSignedPortion,
                                pChild->pSig,
                                pChild->cbSig,
                                pThis->pPublicKey))
            {
                if (pThis) DereferenceCert(pThis);
                if (pResult) DereferenceCert(pResult);
                return SP_LOG_RESULT(PCT_ERR_BAD_CERTIFICATE);
            }
        }
        // We successfully loaded and verified, so
        // move on to the next cert in the chain.
        if(pChild)
        {
            pChild->pIssuerCertificate = pThis;
        }
        else
        {
            pResult = pThis;
        }
        pChild = pThis;
        // We've run off the end of our cert list without
        // Successfully finding a CA, so this chain is
        // unverified.
        if(cbCurrentRaw <= 0)
        {
            break;
        }
    } while(pThis->pIssuerCertificate == NULL);

    // Verify the last certificate against the root
    // certificate
    if(pThis->pIssuerCertificate)
    {
        const SignatureSystem     *pSigSys = NULL;
        BOOL fGenevaRoot = FALSE;
        BOOL fVerisign3Root = FALSE;
        pSigSys = SigFromSpec(pThis->SigType, SP_PROT_ALL);
        if(pSigSys == NULL)
        {
            if (pResult) DereferenceCert(pResult);
            return SP_LOG_RESULT(PCT_ERR_BAD_CERTIFICATE);
        }
        if(!pSigSys->Verify(pThis->pSignedPortion,
                            pThis->cbSignedPortion,
                            pThis->pSig,
                            pThis->cbSig,
                            pThis->pIssuerCertificate->pPublicKey))
        {
            if (pResult) DereferenceCert(pResult);
            return SP_LOG_RESULT(PCT_ERR_BAD_CERTIFICATE);
        }
        // check for special Server Gated Crypto root issuers
        while (pThis && pThis->pIssuerCertificate != pThis)
            pThis = pThis->pIssuerCertificate;

        if (g_pVerisign3Root && pThis == g_pVerisign3Root)
            fVerisign3Root = TRUE;
        else if (g_pGenevaRoot && pThis == g_pGenevaRoot)
            fGenevaRoot = TRUE;
            
        // check for Server Gated Crypto OIDs
        if ( pdwSGC && (fVerisign3Root || fGenevaRoot) && pResult->pEnhancedKeyUsage)
        {
            UINT i;
            for (i = 0; i < pResult->pEnhancedKeyUsage->cObjIds; i++)
            {
                ObjIdDesc *poid = &pResult->pEnhancedKeyUsage->aObjIds[i];
                if (poid->cbObjId == sizeof(g_DER_OID_NETSCAPE_SGC)
                    && memcmp(poid->pbObjId,g_DER_OID_NETSCAPE_SGC,sizeof(g_DER_OID_NETSCAPE_SGC)) == 0
                    )
                {
                    if (fVerisign3Root)
                    {
                        (*pdwSGC) |= 1;
                          DEBUGMSG(ZONE_CERT,(TEXT("SPLoadCertificate: NETSCAPE SGC\r\n")));
                    }
                }
                else if (poid->cbObjId == sizeof(g_DER_OID_SERVER_GATED_CRYPTO)
                    && memcmp(poid->pbObjId,g_DER_OID_SERVER_GATED_CRYPTO,sizeof(g_DER_OID_SERVER_GATED_CRYPTO)) == 0
                    )

                {
                    if (fVerisign3Root || fGenevaRoot)
                    {
                        (*pdwSGC) |= 1;
                          DEBUGMSG(ZONE_CERT,(TEXT("SPLoadCertificate: MS SGC\r\n")));
                    }
                }
            }
        }
    }


    *ppCert = pResult;

    return PCT_ERR_OK;

}


SP_STATUS
SPSerializeCertificate(
    DWORD      fProtocol,
    PUCHAR *   ppCertificate,
    DWORD  *   pcbCertificate,
    PPctCertificate pCert
)
{
    PPctCertificate pThisCert;
    DWORD cbCert;
    PUCHAR pByte;

    pThisCert = pCert;
    cbCert = 0;
    if(pcbCertificate == NULL)
    {
        return SP_LOG_RESULT(PCT_INT_INTERNAL_ERROR);
    }
    // First calculate the size
    while(pThisCert != NULL)
    {
        cbCert += pThisCert->cbRawCert;
        if(fProtocol & SP_PROT_SSL3)
        {
            cbCert += CB_SSL3_CERT_VECTOR;
        }
        // Is this cert self signed?
        if(pThisCert == pThisCert->pIssuerCertificate)
        {
            break;
        }
        pThisCert = pThisCert->pIssuerCertificate;
    }

    //cbCert now contains the number of bytes
    if(ppCertificate == NULL)
    {
        *pcbCertificate = cbCert;
        return (PCT_ERR_OK);
    }
    if(*ppCertificate == NULL)
    {
        *ppCertificate = (PUCHAR)SPExternalAlloc(cbCert);
        if(*ppCertificate == NULL)
        {
            return SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY);
        }
    }
    pThisCert = pCert;
    pByte = *ppCertificate;
    while(pThisCert != NULL)
    {
        if(fProtocol & SP_PROT_SSL3)
            {
		    pByte[0] = MS24BOF(pThisCert->cbRawCert);
		    pByte[1] = MSBOF(pThisCert->cbRawCert);
		    pByte[2] = LSBOF(pThisCert->cbRawCert);
		    pByte += CB_SSL3_CERT_VECTOR;
        }
        CopyMemory(pByte, pThisCert->pRawCert,pThisCert->cbRawCert);
        pByte += pThisCert->cbRawCert;
        // Is this cert self signed?
        if(pThisCert == pThisCert->pIssuerCertificate)
        {
            break;
        }
        pThisCert = pThisCert->pIssuerCertificate;
    }
    *pcbCertificate = cbCert;

    return(PCT_ERR_OK);
}


LONG
ReferenceCert(PPctCertificate pCert)
{
    if(pCert == NULL)
    {
        DEBUGMSG(ZONE_CERT,
                (TEXT("ReferenceCert(NULL).\r\n"))
                );

        return -1;
    }
    DEBUGMSG(ZONE_CERT,
            (TEXT("ReferenceCert(%d),   cRef = %d->%d\r\n"),
             pCert->dwDebugId,
             pCert->cRef,
             pCert->cRef + 1)
            );

    return InterlockedIncrement(&pCert->cRef);
}

LONG
DereferenceCert(PPctCertificate pCert)
{
    LONG cRet;
    PCertSysInfo pInfo;

    if(pCert == NULL)
    {
        DEBUGMSG(ZONE_CERT,
                (TEXT("DereferenceCert(NULL).\r\n"))
                );

        return -1;
    }

    DEBUGMSG(ZONE_CERT,
            (TEXT("DereferenceCert(%d), cRef = %d->%d.\r\n"),
             pCert->dwDebugId,
             pCert->cRef,
             pCert->cRef - 1)
            );

    if(0 == (cRet = InterlockedDecrement(&pCert->cRef)))
    {
        pInfo = GetCertSysInfo(pCert->Spec);
        if(pInfo == NULL)
        {
            return -1;
        }
        if(!pInfo->System->Delete(pCert))
        {
            return -1;
        }
        return 0;
    }
    return (cRet);
}

