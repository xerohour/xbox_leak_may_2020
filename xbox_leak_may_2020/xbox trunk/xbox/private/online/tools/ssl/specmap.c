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

#include <spbase.h>


/* supported cipher type arrays */

CipherInfo  SPAvailableCiphers[] = {
    {
#ifndef SCHANNEL_EXPORT
        SP_PROT_ALL,
#else
        SP_PROT_CRED | SP_PROT_SGC,
#endif
        SP_PROT_ALL,
        SP_CIPHER_RC4 | SP_ENC_BITS_128 | SP_MAC_BITS_128,
        TEXT("RC4 128/128"),
        &csRC4
    },

    {

#ifdef ENABLE_SERVER_SELECTIVE_CRYPTO
        SP_PROT_SERVERS | SP_PROT_CRED,
#else
        SP_PROT_CRED | SP_PROT_SGC,
#endif
        SP_PROT_ALL,
        SP_CIPHER_RC4 | SP_ENC_BITS_64 | SP_MAC_BITS_128,
        TEXT("RC4 64/128"),
        &csRC4
    },
    {
        SP_PROT_ALL,
        SP_PROT_ALL,
        SP_CIPHER_RC4 | SP_ENC_BITS_40  | SP_MAC_BITS_128,
        TEXT("RC4 40/128"),
        &csRC4
    }
        ,
    {
        SP_PROT_SSL3,
        SP_PROT_SSL3,
        SP_CIPHER_NONE | SP_ENC_BITS_128 | SP_MAC_BITS_128,
        TEXT("NULL 0/128"),
        &csNONE
    }

};

const DWORD SPNumAvailableCiphers = sizeof(SPAvailableCiphers)/sizeof(CipherInfo);

HashInfo  SPAvailableHashes[] =
{
    {
        SP_PROT_ALL,
        SP_PROT_ALL,
        SP_HASH_MD5,
        TEXT("MD5"),
        &ckMD5
    },
    {
        SP_PROT_ALL,
        SP_PROT_ALL,
        SP_HASH_SHA,
        TEXT("SHA"),
        &ckSHA
    }
};

const DWORD SPNumAvailableHashes = sizeof(SPAvailableHashes)/sizeof(HashInfo);

CertSysInfo   SPAvailableCerts[] =
{
    {
        SP_PROT_ALL,
        SP_PROT_ALL,
        SP_CERT_X509,
        TEXT("X.509"),
        &certX509
    }
};

const DWORD SPNumAvailableCerts = sizeof(SPAvailableCerts)/sizeof(CertSysInfo);

SigInfo SPAvailableSigs[] =
{
    {
        SP_PROT_ALL,
        SP_PROT_ALL,
        SP_SIG_RSA_MD2,
        TEXT("RSA Signed MD2"),
        &sigRSAMD2
    },
    {
        SP_PROT_ALL,
        SP_PROT_ALL,
        SP_SIG_RSA_MD5,
        TEXT("RSA Signed MD5"),
        &sigRSAMD5
    },
    {
        SP_PROT_ALL,
        SP_PROT_ALL,
        SP_SIG_RSA_SHA,
        TEXT("RSA Signed SHA"),
        &sigRSASHA
    },
    {
        SP_PROT_SSL3,
        SP_PROT_SSL3,
        SP_SIG_RSA_SHAMD5,
        TEXT("RSA Signed MD5/SHA combination (SSL3 only)"),
        &sigRSASHAMD5
    }
};

const DWORD SPNumAvailableSigs = sizeof(SPAvailableSigs)/sizeof(SigInfo);

KeyExchangeInfo SPAvailableExch[] =
{
    {
        SP_PROT_ALL,
        SP_PROT_ALL,
        SP_EXCH_RSA_PKCS1,
        TEXT("PKCS"),
        &keyexchPKCS
    }
};

const DWORD SPNumAvailableExch = sizeof(SPAvailableExch)/sizeof(KeyExchangeInfo);

PCipherInfo GetCipherInfo(CipherSpec Spec)
{
    DWORD i;
    for (i=0; i<SPNumAvailableCiphers; i++ )
    {
        if(SPAvailableCiphers[i].Spec == Spec)
        {
            return &SPAvailableCiphers[i];
        }
    }
    return NULL;
}

PHashInfo GetHashInfo(HashSpec Spec)
{
    DWORD i;
    for (i=0; i<SPNumAvailableHashes; i++ )
    {
        if(SPAvailableHashes[i].Spec == Spec)
        {
            return &SPAvailableHashes[i];
        }
    }
    return NULL;
}

PKeyExchangeInfo GetKeyExchangeInfo(ExchSpec Spec)
{
    DWORD i;
    for (i=0; i<SPNumAvailableExch; i++ )
    {
        if(SPAvailableExch[i].Spec == Spec)
        {
            return &SPAvailableExch[i];
        }
    }
    return NULL;
}

PCertSysInfo GetCertSysInfo(CertSpec Spec)
{
    DWORD i;
    for (i=0; i<SPNumAvailableCerts; i++ )
    {
        if(SPAvailableCerts[i].Spec == Spec)
        {
            return &SPAvailableCerts[i];
        }
    }
    return NULL;
}


PSigInfo GetSigInfo(SigSpec Spec)
{
    DWORD i;
    for (i=0; i<SPNumAvailableSigs; i++ )
    {
        if(SPAvailableSigs[i].Spec == Spec)
        {
            return &SPAvailableSigs[i];
        }
    }
    return NULL;
}

const
CryptoSystem *
CipherFromSpec(CipherSpec Spec, DWORD fProtocol)
{
    PCipherInfo pInfo;
    pInfo = GetCipherInfo(Spec);
    if(pInfo == NULL)
    {
        return NULL;
    }
    if(pInfo->fProtocol & fProtocol)
    {
        return pInfo->System;
    }
    return NULL;
}

const
CheckSumFunction *
HashFromSpec(HashSpec Spec, DWORD fProtocol)
{
    PHashInfo pInfo;
    pInfo = GetHashInfo(Spec);
    if(pInfo == NULL)
    {
        return NULL;
    }
    if(pInfo->fProtocol & fProtocol)
    {
        return pInfo->System;
    }
    return NULL;
}

const
KeyExchangeSystem *
KeyExchangeFromSpec(ExchSpec Spec, DWORD fProtocol)
{
    PKeyExchangeInfo pInfo;
    pInfo = GetKeyExchangeInfo(Spec);
    if(pInfo == NULL)
    {
        return NULL;
    }
    if(pInfo->fProtocol & fProtocol)
    {
        return pInfo->System;
    }
    return NULL;
}

const
CertSystem *
CertFromSpec(CertSpec Spec, DWORD fProtocol)
{
    PCertSysInfo pInfo;
    pInfo = GetCertSysInfo(Spec);
    if(pInfo == NULL)
    {
        return NULL;
    }
    if(pInfo->fProtocol & fProtocol)
    {
        return pInfo->System;
    }
    return NULL;
}

const
SignatureSystem *
SigFromSpec(SigSpec Spec, DWORD fProtocol)
{
    PSigInfo pInfo;
    pInfo = GetSigInfo(Spec);
    if(pInfo == NULL)
    {
        return NULL;
    }
    if(pInfo->fProtocol & fProtocol)
    {
        return pInfo->System;
    }
    return NULL;
}

