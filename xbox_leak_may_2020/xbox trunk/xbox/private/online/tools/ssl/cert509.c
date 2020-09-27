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

/*-----------------------------------------------------------------------------
* RSA Public Key Cryptosystem, RC4, MD2, MD5 and RSA are trademarks
* of RSA Data Security, Inc.
*----------------------------------------------------------------------------*/

#include <spbase.h>
#include "cert509.h"


#ifdef DEBUG
DWORD g_cPctCertDebugIds = 0;
#endif // DEBUG

const BYTE g_DER_OID_EnhancedKeyUsage[] = {0x55, 0x1d, 0x25};	//2.5.29.37

#ifdef DEBUG
// Convert the object id to dot format and dump it
void
DumpOid(PBYTE pbObjId, UINT cbObjId)
{
    TCHAR szDumpBuf[128];   // should be long enough
    int i;
    if (!cbObjId)
        return;

    i = wsprintf(szDumpBuf,TEXT(" %d.%d"),(*pbObjId)/40, (*pbObjId)%40);
    --cbObjId;
    while (cbObjId)
    {
        UINT value = 0;
        do
        {
            ++pbObjId;
            --cbObjId;
            value = (value << 7) + ((*pbObjId) & 0x7f);
        }
        while (*pbObjId & 0x80);

        i += wsprintf(&szDumpBuf[i], TEXT(".%d"), value);

    }
    DEBUGMSG(ZONE_CERT,
                (TEXT("%s"),
                 szDumpBuf)
                );
   
}
#endif

BOOL WINAPI
X509DecodeCertificate(
    PUCHAR              pbCertificate,
    DWORD               cbCertificate,
    PPctCertificate *   ppCertificate)
{
    DWORD   cbCertInfo;
    PUCHAR  pCertInfo;
    PUCHAR  pbCert;
    int     Result;
    EnhancedKeyUsage *peku = NULL;
//    PUCHAR  SignedPortion;

    DWORD   cCurCert;

    PX509Certificate    pCertificate;
    const KeyExchangeSystem *pSys = NULL;
    DWORD       PubKeyAlg;
    PUCHAR      PubKeyBuffer;
    DWORD       SigAlg;

    DWORD       dwLength;


    DWORD       dwIssuerOffset;
    DWORD       dwSubjectOffset;
    DWORD       dwSignedOffset;
    DWORD       cbIssuer;
    DWORD       cbSubject;

    SP_BEGIN("X509DecodeCertificate");
    *ppCertificate = NULL;
    pbCert = pbCertificate;
    cCurCert = cbCertificate;


    cCurCert -= Result = DecodeHeader(&cbCertInfo, pbCert, cCurCert);
    if (Result < 0)
    {
        DebugLog((DEB_WARN, "Initial header wrong\n"));
        SP_RETURN(FALSE);
    }

    pCertInfo = pbCert + Result;
    dwSignedOffset = Result;
    cbCertInfo += Result;  // This is the complete length of the cert.


    /* Break Out certificate info */


    cCurCert -= Result = DecodeHeader(&dwLength, pCertInfo, cCurCert);
    if (Result < 0)
    {
        DebugLog((DEB_WARN, "Cert Data header wrong\n"));
        SP_RETURN(FALSE);
    }

    pCertificate = (PX509Certificate)SPExternalAlloc(sizeof(X509Certificate) );
    if (!pCertificate)
    {
        SP_RETURN(FALSE);
    }

    pCertInfo += Result;

    if (0xa0 == *pCertInfo)
    {
        pCertInfo += 1; // Skip the [0] tag.
        cCurCert -= 1;
        cCurCert -= Result = DecodeLength(&dwLength, pCertInfo, cCurCert);
        if (Result < 0)
        {
            DebugLog((DEB_WARN, "Bad Version Number in certificate\n"));
            goto Crack_CleanUp;
        }
        pCertInfo += Result;

        DebugLog((DEB_TRACE, "%3d:Decoding Version\n", cCurCert-cbCertificate));
        cCurCert -= Result = DecodeInteger( (BYTE *) &pCertificate->Version,
                                sizeof(pCertificate->Version),
                                &dwLength,
                                pCertInfo,
                                cCurCert,
                                TRUE );

        if (Result < 0)
        {
            DebugLog((DEB_WARN, "Bad Version Number in certificate\n"));
            goto Crack_CleanUp;
        }
        pCertInfo += Result;
    }

    DebugLog((DEB_TRACE, "%3d:Decoding Serial Number\n", cCurCert-cbCertificate));
    cCurCert -= Result = DecodeInteger( (BYTE *) pCertificate->SerialNumber,
                            sizeof(pCertificate->SerialNumber),
                            &dwLength,
                            pCertInfo,
                            cCurCert,
                            TRUE );

    if (Result < 0)
    {
        DebugLog((DEB_WARN, "No Serial Number in certificate\n"));
        goto Crack_CleanUp;
    }

    pCertInfo += Result;

    DebugLog((DEB_TRACE, "%3d:Decoding Algorithm\n", cCurCert-cbCertificate));
    cCurCert -= Result = DecodeSigAlg(&SigAlg,
                                pCertInfo, cCurCert,
                                TRUE);

    if (Result < 0)
    {
        DebugLog((DEB_WARN, "Algorithm has non-null parameters!\n"));
        goto Crack_CleanUp;
    }

    pCertInfo += Result;

    dwIssuerOffset = (DWORD)pCertInfo-(DWORD)pbCertificate;

    Result = DecodeDN( NULL, &dwLength, pCertInfo, cCurCert, FALSE);
    if (Result < 0)
    {
        DebugLog((DEB_WARN, "No issuer name in certificate\n"));
        goto Crack_CleanUp;
    }

    cbIssuer = Result;

    pCertificate->pszIssuer = (PSTR)SPExternalAlloc(dwLength + 2);
    if (!pCertificate->pszIssuer)
    {
        DebugLog((DEB_WARN, "Out of memory\n"));
        goto Crack_CleanUp;
    }

    DebugLog((DEB_TRACE, "%3d:Decoding Issuer\n", cCurCert-cbCertificate));
    cCurCert -= Result = DecodeDN(pCertificate->pszIssuer,
                        &dwLength,
                        pCertInfo, cCurCert,
                        TRUE);
    pCertInfo += Result;

    cCurCert -= Result = DecodeHeader(&dwLength, pCertInfo, cCurCert);
    if (Result < 0)
    {
        DebugLog((DEB_WARN, "Header for Validity times not found\n"));
        goto Crack_CleanUp;
    }

    DebugLog((DEB_TRACE, "%3d:Decoding From Time\n", cCurCert-cbCertificate));
    pCertInfo += Result;
    cCurCert -= Result = DecodeFileTime(&pCertificate->ValidFrom,
                            pCertInfo, cCurCert,
                            TRUE);

    if (Result < 0)
    {
        DebugLog((DEB_WARN, "Valid from not found\n"));
        goto Crack_CleanUp;
    }

    DebugLog((DEB_TRACE, "%3d:Decoding Until Time\n", cCurCert-cbCertificate));
    pCertInfo += Result;
    cCurCert -= Result = DecodeFileTime(&pCertificate->ValidUntil,
                            pCertInfo, cCurCert,
                            TRUE);

    if (Result < 0)
    {
        DebugLog((DEB_WARN, "Valid until not found\n"));
        goto Crack_CleanUp;
    }

    pCertInfo += Result;

    dwSubjectOffset = (DWORD)pCertInfo-(DWORD)pbCertificate;

    Result = DecodeDN(NULL, &dwLength, pCertInfo, cCurCert, FALSE);

    if (Result < 0)
    {
        DebugLog((DEB_WARN, "Subject name not found\n"));
        goto Crack_CleanUp;
    }

    cbSubject = Result;

    pCertificate->pszSubject = (PSTR)SPExternalAlloc(dwLength + 2);

    if (!pCertificate->pszSubject)
    {
        DebugLog((DEB_WARN, "Out of memory\n"));
        goto Crack_CleanUp;
    }

    DebugLog((DEB_TRACE, "%3d:Decoding Subject\n", cCurCert-cbCertificate));
    cCurCert -= Result = DecodeDN(  pCertificate->pszSubject,
                        &dwLength,
                        pCertInfo, cCurCert,
                        TRUE);

    if (Result < 0)
    {
        DebugLog((DEB_WARN, "Could not decode DN of subject\n"));
        goto Crack_CleanUp;
    }

    DebugLog((DEB_TRACE, "Subject: %s\n", pCertificate->pszSubject));


    pCertInfo += Result;


    /* Now, get subjectPublicKeyInfo */


    cCurCert -= Result = DecodeHeader(&dwLength, pCertInfo, cCurCert);
    if (Result < 0)
    {
        DebugLog((DEB_WARN, "No header for pubkey\n"));
        goto Crack_CleanUp;
    }

    pCertInfo += Result;

    DebugLog((DEB_TRACE, "%3d:Get Pubkey Alg\n", cCurCert-cbCertificate));
    cCurCert -= Result = DecodeKeyType(&PubKeyAlg,
                             pCertInfo, cCurCert,
                             TRUE);

    if (Result < 0)
    {
        DebugLog((DEB_WARN, "No algorithm\n"));
        goto Crack_CleanUp;
    }

    pCertInfo += Result;

    DebugLog((DEB_TRACE, "%3d:Get Pubkey\n", cCurCert-cbCertificate));
    Result = DecodeBitString(NULL, &dwLength, pCertInfo, cCurCert, FALSE);
    if (Result < 0)
    {
        DebugLog((DEB_WARN, "No pubkey bitstring\n"));
        goto Crack_CleanUp;
    }

    PubKeyBuffer = (PUCHAR)SPExternalAlloc(dwLength);
    if (!PubKeyBuffer)
    {
        DebugLog((DEB_WARN, "Out of memory\n"));
        goto Crack_CleanUp;
    }

    cCurCert -= Result = DecodeBitString(PubKeyBuffer, &dwLength, pCertInfo, cCurCert, TRUE);
    DebugLog((DEB_TRACE, "Filled Bit Buffer with %d bytes\n",dwLength));
    pCertInfo += Result;

    pCertificate->pPublicKey = NULL;
    DebugLog((DEB_TRACE, "Looking for decode method for algid %lx\n", PubKeyAlg));

    pSys =  KeyExchangeFromSpec(PubKeyAlg, SP_PROT_CRED);
    if(pSys == NULL) goto Crack_CleanUp;
    if( 0 > pSys->DecodePublic(PubKeyBuffer, dwLength, &pCertificate->pPublicKey))
    {
        goto Crack_CleanUp;
    }

    if(pCertificate->pPublicKey == NULL) goto Crack_CleanUp;

    //
    // Suck up  optional X.509 V2 fields:
    //  issuerUniqueId and subjectUniqueId
    //

    while (0xa1 == (*pCertInfo) || 0xa2 == (*pCertInfo))
    {
        pCertInfo += 1; // Skip the tag.
        cCurCert -= 1;
        cCurCert -= Result = DecodeLength(&dwLength, pCertInfo, cCurCert);
        if (Result < 0)
        {
            DebugLog((DEB_WARN, "Bad optional field in certificate\n"));
            goto Crack_CleanUp;
        }
        DebugLog((DEB_TRACE, "%4x:Skipping (%d)\n", cbCertificate-cCurCert, dwLength));
        pCertInfo += Result + dwLength;
        cCurCert -= dwLength;   // georgej added
    }

    // look at the extensions
    if (cCurCert > 0 && 0xa3 == *pCertInfo) {
        DWORD cbExtensions;
        pCertInfo += 1;    // skip the tag
        cCurCert -= 1;
        cCurCert -= Result = DecodeLength(&dwLength, pCertInfo, cCurCert);
        if (Result < 0)
        {
            DebugLog((DEB_WARN, "Bad extensions header in certificate\n"));
            goto Crack_CleanUp;
        }
        pCertInfo += Result;
        // skip past Extensions header (SEQ OF)
        cCurCert -= Result = DecodeHeader(&dwLength, pCertInfo, cCurCert);
        if (Result < 0)
        {
            DebugLog((DEB_WARN, "Bad extensions header in certificate\n"));
            goto Crack_CleanUp;
        }
        pCertInfo += Result;
        cbExtensions = dwLength;
        while (cbExtensions)
        {
            CertExtension certExt;
            cCurCert -= Result = DecodeExtension(&certExt, pCertInfo, cbExtensions);
            if (Result < 0)
            {
                DebugLog((DEB_WARN, "Bad extension field in certificate\n"));
                goto Crack_CleanUp;
            }
            pCertInfo += Result;
            cbExtensions -= Result;
#ifdef DEBUG
            
            DumpOid(certExt.pExtId, certExt.cbExtId);

            DEBUGMSG(ZONE_CERT,
                    (TEXT(" extn length %d.\r\n"),
                     certExt.cbValue)
                    );
#endif
            // is this an extension we're interested in?
            if (certExt.cbExtId == sizeof(g_DER_OID_EnhancedKeyUsage)
                && memcmp(certExt.pExtId, g_DER_OID_EnhancedKeyUsage, certExt.cbExtId) == 0)
            {
                // decode the Enhanced Key Usage extension
                // will allocate memory for EnhancedKeyUsage struct and oids
                
                if (DecodeEnhancedKeyUsage(&peku, certExt.pbValue, certExt.cbValue) < 0)
                {
                    DebugLog((DEB_WARN, "Error decoding Enhanced Key Usage\n"));
                }
#ifdef DEBUG
                else
                {
                    UINT i;
                    DEBUGMSG(ZONE_CERT,(TEXT("Enhanced Key Usage objIds\r\n")));
                    for (i=0;i<peku->cObjIds;i++)
                    {
                        DumpOid(peku->aObjIds[i].pbObjId, peku->aObjIds[i].cbObjId);
                        DEBUGMSG(ZONE_CERT,(TEXT("\r\n")));
                    }
                }
#endif

            }
            
        }

    }

    *ppCertificate = (PPctCertificate)SPExternalAlloc(sizeof(PctCertificate));
    if (NULL == *ppCertificate)
    {
        DebugLog((DEB_WARN, "Out of memory\n"));
        goto Crack_CleanUp;
    }
#ifdef DEBUG
    (*ppCertificate)->dwDebugId = ++g_cPctCertDebugIds;
#endif // DEBUG
    (*ppCertificate)->fTrusted = FALSE;
    (*ppCertificate)->pRawCert = NULL;
    (*ppCertificate)->pRawPubKey = PubKeyBuffer;
    (*ppCertificate)->cbRawPubKey = dwLength;
    (*ppCertificate)->pCert = (PUCHAR)pCertificate;
    (*ppCertificate)->pszSubject = (PUCHAR)pCertificate->pszSubject;
    (*ppCertificate)->pPublicKey = pCertificate->pPublicKey;
    (*ppCertificate)->pIssuerCertificate = FindIssuerKey(pCertificate->pszIssuer);
    (*ppCertificate)->szSigningAuthority = pCertificate->pszIssuer;
    (*ppCertificate)->ValidFrom = pCertificate->ValidFrom;
    (*ppCertificate)->ValidUntil = pCertificate->ValidUntil;
    (*ppCertificate)->Spec = SP_CERT_X509;

    (*ppCertificate)->cbRawCert = cbCertInfo;
    (*ppCertificate)->pRawCert = (PUCHAR)SPExternalAlloc(cbCertInfo);
    if (NULL == (*ppCertificate)->pRawCert)
    {
        DebugLog((DEB_WARN, "Out of memory\n"));
        goto Crack_CleanUp;
    }
    CopyMemory((*ppCertificate)->pRawCert, pbCertificate, cbCertInfo);
    (*ppCertificate)->cbSignedPortion = ((DWORD)pCertInfo - (DWORD)pbCert) - dwSignedOffset;
    (*ppCertificate)->pSignedPortion = (*ppCertificate)->pRawCert + dwSignedOffset;

    (*ppCertificate)->pbIssuer =(*ppCertificate)->pRawCert + dwIssuerOffset;
    (*ppCertificate)->cbIssuer = cbIssuer;

    (*ppCertificate)->pbSubject =(*ppCertificate)->pRawCert + dwSubjectOffset;
    (*ppCertificate)->cbSubject = cbSubject;
	(*ppCertificate)->pEnhancedKeyUsage = peku;

    DebugLog((DEB_TRACE, "%3d:Get Sig Alg\n", cCurCert-cbCertificate));
    cCurCert -= Result = DecodeSigAlg( &(*ppCertificate)->SigType, pCertInfo, cCurCert, TRUE );
    if (Result < 0)
    {
        DebugLog((DEB_WARN, "No signature algorithm...\n"));
        goto Crack_CleanUp;
    }

    pCertInfo += Result;

    DebugLog((DEB_TRACE, "%3d:Get Signature\n", cCurCert-cbCertificate));
    Result = DecodeBitString(NULL, &dwLength, pCertInfo, cCurCert, FALSE );
    if (Result < 0)
    {
        DebugLog((DEB_WARN, "No signature bitstring?\n"));
        goto Crack_CleanUp;
    }

    (*ppCertificate)->pSig = (PUCHAR)SPExternalAlloc(dwLength);
    (*ppCertificate)->cbSig = dwLength;
    if (!(*ppCertificate)->pSig)
    {
        DebugLog((DEB_WARN, "Out of memory\n"));
        goto Crack_CleanUp;
    }

    cCurCert -= Result = DecodeBitString((*ppCertificate)->pSig, &dwLength, pCertInfo, cCurCert, TRUE );
    DebugLog((DEB_TRACE, "Bytes left (%d)\n", cCurCert, dwLength));

    SPExternalFree(PubKeyBuffer);
    ReferenceCert(*ppCertificate);

#ifdef DEBUG

    {
        SYSTEMTIME stValidFrom;
        SYSTEMTIME stValidUntil;

        FileTimeToSystemTime(&pCertificate->ValidFrom, &stValidFrom);
        FileTimeToSystemTime(&pCertificate->ValidUntil, &stValidUntil);

        DEBUGMSG(ZONE_CERT,
                (TEXT("Certificate %d.\r\n"),
                 (*ppCertificate)->dwDebugId)
                );

        DEBUGMSG(ZONE_CERT,
                (TEXT("Issuer = %hs\r\n"), pCertificate->pszIssuer)
                );

        DEBUGMSG(ZONE_CERT,
                (TEXT("Subject= %hs\r\n"), pCertificate->pszSubject)
                );

        DEBUGMSG(ZONE_CERT,
                (TEXT("\t\tYr\tMo\tDay\tHr\tMin\tSec\tms\r\n"))
                );

        DEBUGMSG(ZONE_CERT,
                (TEXT("From\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\r\n"),
                 stValidFrom.wYear,      stValidFrom.wMonth,
                 stValidFrom.wDayOfWeek, stValidFrom.wDay,
                 stValidFrom.wHour,      stValidFrom.wMinute,
                 stValidFrom.wSecond,    stValidFrom.wMilliseconds)
                );

        DEBUGMSG(ZONE_CERT,
                (TEXT("Until\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\r\n"),
                 stValidUntil.wYear,      stValidUntil.wMonth,
                 stValidUntil.wDayOfWeek, stValidUntil.wDay,
                 stValidUntil.wHour,      stValidUntil.wMinute,
                 stValidUntil.wSecond,    stValidUntil.wMilliseconds)
                );

        DEBUGMSG(ZONE_CERT, (TEXT("\r\n")));

    }

#endif // DEBUG

    SP_RETURN( TRUE );

Crack_CleanUp:

    DebugLog((DEB_WARN, "Error return from CrackCertificate, Cert at %x, current pointer at %x\n",
                pbCertificate, pCertInfo));

    if (NULL != *ppCertificate)
    {
        if (NULL != (*ppCertificate)->pRawCert)
        {
            SPExternalFree((*ppCertificate)->pRawCert);
        }
        SPExternalFree(*ppCertificate);
        *ppCertificate = NULL;
    }

    if (pCertificate->pszSubject)
    {
        SPExternalFree(pCertificate->pszSubject);
    }

    if (pCertificate->pszIssuer)
    {
        SPExternalFree(pCertificate->pszIssuer);
    }
    if (peku)
    {
        SPExternalFree(peku);
    }

    SPExternalFree(pCertificate);

    SP_RETURN(FALSE);


}

BOOL
WINAPI X509DeleteCert(
       PPctCertificate  pCertificate)
{
      PX509Certificate    pX509Cert;

      DEBUGMSG(ZONE_CERT,
              (TEXT("X509DeleteCert(0x%.8X) - DebugId = %d.\r\n"),
               pCertificate,
               pCertificate->dwDebugId)
              );

      SPExternalFree(pCertificate->pPublicKey);
      SPExternalFree(pCertificate->pSig);
      SPExternalFree(pCertificate->pRawCert);
      if (pCertificate->pEnhancedKeyUsage)
      {
          SPExternalFree(pCertificate->pEnhancedKeyUsage);
      }
      
      pX509Cert = (PX509Certificate)pCertificate->pCert;
      if (pX509Cert->pszSubject)
      {
          SPExternalFree(pX509Cert->pszSubject);
      }

      if (pX509Cert->pszIssuer)
      {
          SPExternalFree(pX509Cert->pszIssuer);
      }

      if (pCertificate->pIssuerCertificate)
        DereferenceCert(pCertificate->pIssuerCertificate);
      SPExternalFree(pCertificate->pCert);
      SPExternalFree(pCertificate);
      return TRUE;
}


const CertSystem certX509 = { SP_CERT_X509, X509DecodeCertificate, X509DeleteCert };
