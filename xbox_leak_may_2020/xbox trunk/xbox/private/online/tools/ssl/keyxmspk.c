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

#ifdef __cplusplus
extern "C" {
#endif

#include <rsa.h>
#ifdef __cplusplus
}
#endif


static VOID
ReverseMemCopy(
    PUCHAR      Dest,
    PUCHAR      Source,
    ULONG       Size)
{
    PUCHAR  p;

    p = Dest + Size - 1;
    do
    {
        *p-- = *Source++;
    } while (p >= Dest);
}

VOID
ByteSwapDwords(
    DWORD *     Dest,
    DWORD *     Source,
    DWORD       Count)
{
    DWORD   Swap;

    while (Count--)
    {
        Swap = *Source++;
        Swap = htonl(Swap);
        *Dest++ = Swap;
    }
}



long
WINAPI
PkcsDecodePublic(
    PUCHAR          pbEncoded,
    DWORD           cEncoded,
    PctPublicKey   **ppKey);


long
WINAPI
PkcsDecodePrivate(
    PUCHAR          pbEncoded,
    DWORD           cEncoded,
    PctPrivateKey   **ppKey);


BOOL
WINAPI
PkcsEncrypt(
    PUCHAR          pbClean,
    DWORD           cbClean,
    PUCHAR          pbEncrypted,
    DWORD           *pcbEncrypted,
    PctPublicKey    *pKey,
    DWORD           fProtocol);

BOOL
WINAPI
PkcsDecrypt(
    PUCHAR          pbEncrypted,
    DWORD           cbEncrypted,
    PUCHAR          pbClean,
    DWORD           *pcbClean,
    PctPrivateKey   *pKey,
    DWORD           fProtocol);


const KeyExchangeSystem keyexchPKCS = {
    SP_EXCH_RSA_PKCS1,
    "RSA",
    PkcsDecodePublic,
    PkcsDecodePrivate,
    PkcsEncrypt,
    PkcsDecrypt,
};

long
WINAPI
PkcsDecodePublic(
    PUCHAR          pbEncoded,
    DWORD           cEncoded,
    PctPublicKey   **ppKey)
{

    BSAFE_PUB_KEY * pk;
    long            Result;
    long            Bitstring;
    DWORD           dwLen, Aligned;
    DWORD           dwKeySize;
    long            index;
    PctPublicKey    *pCreatedKey = NULL;


    SP_BEGIN("PkcsDecodePublic");

    SP_ASSERT(ppKey != NULL);

    index = 0;

    if(index > (index += DecodeHeader(&dwLen, pbEncoded, cEncoded)))
       goto DecodeKey_CleanUp;

    Bitstring = index + dwLen;

    /* Determine the size of the modulus */
    /* Grab the modulus into a buffer. */
    Result = DecodeInteger(NULL,
                           0,
                           &dwLen,
                           pbEncoded+index,
                           cEncoded - index,
                           FALSE);
    if (Result < 0)
    {
        goto DecodeKey_CleanUp;
    }

    /* rsa code requires QUADWORD size-aligned keys */
    Aligned = ((dwLen + sizeof(DWORD)*2-1) & ~(sizeof(DWORD)*2 - 1));

    /* rsa code requires two extra null DWORDS padded on the end of the aligned key */
    Aligned += sizeof(DWORD)*2;
    dwKeySize = (DWORD)((PctPublicKey *)NULL)->pKey + sizeof(BSAFE_PUB_KEY) + Aligned;
    pCreatedKey = (PctPublicKey *)SPExternalAlloc(dwKeySize);
    if (NULL == pCreatedKey)
    {
        SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY);
        goto DecodeKey_CleanUp;
    }
    pk = (BSAFE_PUB_KEY *)pCreatedKey->pKey;

    Result = DecodeInteger((BYTE *)(pk+1),
                           dwLen,
                           &dwLen,
                           pbEncoded+index,
                           cEncoded - index,
                           TRUE);
    if (Result < 0)
    {
        goto DecodeKey_CleanUp;
    }


    /* repeat calculation, just in case initial length was exaggerated */
    Aligned = ((dwLen + sizeof(DWORD)*2-1) & ~(sizeof(DWORD)*2 - 1));
    Aligned += sizeof(DWORD)*2;

    pk->magic = RSA1;

    pk->keylen = Aligned;
    pk->bitlen = dwLen * 8;
    pk->datalen = dwLen-1;


    pCreatedKey->cbKey = ( dwKeySize );
    pCreatedKey->Type = SP_EXCH_RSA_PKCS1;



    index += Result;

    Result = DecodeInteger((PUCHAR)&pk->pubexp,
                           sizeof(DWORD),
                           &dwLen,
                           pbEncoded+index,
                           cEncoded - index,
                           TRUE);
    if (Result < 0)
    {
        goto DecodeKey_CleanUp;
    }

    *ppKey = pCreatedKey;

    SP_RETURN(Bitstring);

DecodeKey_CleanUp:
    if (pCreatedKey)
    {
        SPExternalFree(pCreatedKey);
    }

    SP_RETURN(-1);
}


long
WINAPI
PkcsDecodePrivate(
    PUCHAR          pbEncoded,
    DWORD           cEncoded,
    PctPrivateKey   **ppKey)
{
    DWORD           dwLen;
    long            Result;
    DWORD           dwKeySize=0;
    DWORD           dwPub, dwPriv, dwBits;
    BSAFE_PRV_KEY * pKey;
    PctPrivateKey   *pNewKey = NULL;
    DWORD           Aligned;
    BSAFE_KEY_PARTS parts;
    long            index;

    SP_BEGIN("PkcsDecodePrivate");

    index = DecodeHeader( &dwLen, pbEncoded , cEncoded);
    if (index < 0)
    {
        SP_RETURN(-1);
    }


    /* Skip past the version */


    Result = DecodeInteger( NULL, 0, &dwLen, pbEncoded+index, cEncoded-index, FALSE );
    if (Result < 0)
    {
        goto DecodePrivate_CleanUp;
    }

    index += Result;
    // Get the first part of the key to compute the number of
    // bits
    Result = DecodeInteger( NULL,
                            0,
                            &dwLen, pbEncoded+index,
                            cEncoded-index,
                            FALSE );
    if (Result < 0)
    {
        goto DecodePrivate_CleanUp;
    }
    dwBits = dwLen * 8;

    BSafeComputeKeySizes(&dwPub, &dwPriv, &dwBits );

    dwKeySize = (DWORD)((PctPrivateKey *)NULL)->pKey + dwPriv;
    pNewKey = (PctPrivateKey *)SPExternalAlloc(dwKeySize);
    if (!pNewKey)
    {
        SP_LOG_RESULT(PCT_INT_OUT_OF_MEMORY);
    	SP_RETURN(-1);
    }
    pNewKey->cbKey = dwKeySize;
    pNewKey->Type = SP_EXCH_RSA_PKCS1;
    pKey = (BSAFE_PRV_KEY *)pNewKey->pKey;

    /* Get private key */
    Result = DecodeInteger( (PUCHAR)(pKey+1),
                            dwLen,
                            &dwLen, pbEncoded+index,
                            cEncoded-index,
                            TRUE );
    if (Result < 0)
    {
        goto DecodePrivate_CleanUp;
    }

    pKey->magic = RSA2;

    /* RSA code requires 2 null dwords padded on the end of the aligned key */
    Aligned = (dwLen + sizeof(DWORD)-1) / sizeof(DWORD);

    pKey->keylen = (Aligned+2) * 4;
    pKey->bitlen = dwLen * 8;
    pKey->datalen = dwLen-1;

    BSafeGetPrvKeyParts(pKey, &parts);

    index += Result;

    Result = DecodeInteger( (PUCHAR)&pKey->pubexp,
                            sizeof(DWORD),
                            &dwLen,
                            pbEncoded+index,
                            cEncoded-index,
                            TRUE );
    if ((Result <0))
    {
        goto DecodePrivate_CleanUp;
    }

    index += Result;
    Result = DecodeInteger( parts.prvexp,
                            pKey->datalen+1,
                            &dwLen,
                            pbEncoded+index,
                            cEncoded-index,
                            TRUE );
    if ((Result <0))
    {
        goto DecodePrivate_CleanUp;
    }

    index += Result;

    Result = DecodeInteger( parts.prime1,
                            pKey->datalen+1,
                            &dwLen,
                            pbEncoded+index,
                            cEncoded-index,
                            TRUE );
    if ((Result <0))
    {
        goto DecodePrivate_CleanUp;
    }


    index += Result;

    Result = DecodeInteger( parts.prime2,
                            pKey->datalen+1,
                            &dwLen,
                            pbEncoded+index,
                            cEncoded-index,
                            TRUE );
    if ((Result <0))
    {
        goto DecodePrivate_CleanUp;
    }


    index += Result;

    Result = DecodeInteger( parts.exp1,
                            pKey->datalen+1,
                            &dwLen,
                            pbEncoded+index,
                            cEncoded-index,
                            TRUE );
    if ((Result <0))
    {
        goto DecodePrivate_CleanUp;
    }


    index += Result;

    Result = DecodeInteger( parts.exp2,
                            pKey->datalen+1,
                            &dwLen,
                            pbEncoded+index,
                            cEncoded-index,
                            TRUE );
    if ((Result <0))
    {
        goto DecodePrivate_CleanUp;
    }


    index += Result;

    Result = DecodeInteger( parts.coef,
                            pKey->datalen+1,
                            &dwLen,
                            pbEncoded+index,
                            cEncoded-index,
                            TRUE );
    if ((Result <0))
    {
        goto DecodePrivate_CleanUp;
    }


    index += Result;
    *ppKey = pNewKey;

    SP_RETURN( index);

DecodePrivate_CleanUp:
    if(pNewKey) {
        FillMemory(pNewKey, dwKeySize, 0);
        SPExternalFree( pNewKey );
    }

    SP_RETURN(-1);
}



BOOL
WINAPI
PkcsEncrypt(
    PUCHAR          pbClean,
    DWORD           cbClean,
    PUCHAR          pbEncrypted,
    DWORD           *pcbEncrypted,
    PctPublicKey    *pKey,
    DWORD 			fProtocol)
{


    UCHAR LocalBuffer[500];
    UCHAR OutputBuffer[500];
    DWORD iLoc;

    BSAFE_PUB_KEY *pk;
    SP_ASSERT(pbClean != NULL);
    SP_ASSERT(pbEncrypted != NULL);
    SP_ASSERT(pKey != NULL);
    pk = (BSAFE_PUB_KEY *)pKey->pKey;
    if(cbClean > pk->datalen - 11) return FALSE;

    FillMemory(LocalBuffer, pk->keylen, 0);
    /* BSafeEncPublic wants the data in opposite of network order */
    ReverseMemCopy(LocalBuffer, pbClean, cbClean);
    LocalBuffer[cbClean] = 0;  /* Separator */

    iLoc = cbClean+1;
    // Rollback prevention checks
    switch(fProtocol)
    {
        case SP_PROT_SSL2_CLIENT:
            if(g_ProtEnabled & SP_PROT_SSL3)
            {
                // If we're a client doing SSL2, and
                // SSL3 is enabled, then for some reason
                // the server requested SSL2.  Maybe
                // A man in the middle changed the server
                // version in the server hello to roll
                // back.  Pad with 8 0x03's so the server
                // can detect this.
                if(iLoc + 8 >= pk->datalen-1)
                {
                    return FALSE;
                }
                FillMemory(&LocalBuffer[iLoc], 8, 0x03);
                iLoc += 8;
                if(fProtocol & SP_PROT_PCT1)
                {
                }
           }

    }


    for(; iLoc < pk->datalen-1; iLoc++ )
    {
        do {
	        GenerateRandomBits(&LocalBuffer[iLoc], 1);
        } while(!LocalBuffer[iLoc]);
    }

    /* Make into pkcs block type 2 */
    LocalBuffer[pk->datalen] = 0;
    LocalBuffer[pk->datalen-1] = 2;

    if (!BSafeEncPublic(pk, LocalBuffer, OutputBuffer))
    {
	    return FALSE;
    }
    	
    *pcbEncrypted = pk->datalen+1;

    ReverseMemCopy(pbEncrypted, OutputBuffer, *pcbEncrypted);

    return TRUE;
}


BOOL
WINAPI
PkcsDecrypt(
    PUCHAR          pbEncrypted,
    DWORD           cbEncrypted,
    PUCHAR          pbClean,
    DWORD           *pcbClean,
    PctPrivateKey   *pKey,
    DWORD           fProtocol)
{
    UCHAR InputBuffer[500];
    UCHAR LocalBuffer[500];
    UCHAR *pbLocal;
    BSAFE_PRV_KEY *pk = (BSAFE_PRV_KEY *)pKey->pKey;

    if(pk->magic != RSA2)
    {
        // This isn't a bsafe key,  so it must be a WinSock 2
        // LSP key.
        SSLKEYEXCHANGEFUNC pKeyExchHook;
        LPVOID pKeyExchArg;

        // Get pointer to callback function.
        pKeyExchHook = ((PSCH_CRED_SECRET_WINSOCK2)pKey->pKey)->pKeyExchangeHookFunc;
        pKeyExchArg  = ((PSCH_CRED_SECRET_WINSOCK2)pKey->pKey)->pKeyExchangeHookArg;

        // Invoke the callback function.
        if(pKeyExchHook(SSL_EXCH_RSADECRYPT,
                        pKeyExchArg,
                        pbEncrypted,
                        cbEncrypted,
                        0, //flags
                        pbClean,
                        pcbClean) != SSL_ERR_OKAY)
        {
            return FALSE;
        }

        // Return success.
        return TRUE;
    }

    if(cbEncrypted != pk->datalen+1) return FALSE;
    FillMemory(InputBuffer, pk->keylen, 0);

    ReverseMemCopy(InputBuffer, pbEncrypted, cbEncrypted);
    BSafeDecPrivate(pk, InputBuffer, LocalBuffer);
    pbLocal = &LocalBuffer[pk->datalen-1];
    if(*pbLocal-- != 2)
    {
        return FALSE;  /* Invalid block type */
    }
    // Search backwards for a 0 byte indicating
    // the start of data

    while(*pbLocal)
    {
        if(pbLocal-- <= LocalBuffer)
        {

            // we walked off the end of our buffer
            // without finding data.
            return FALSE;
        }
    }

    *pcbClean = pbLocal - LocalBuffer;

    ReverseMemCopy(pbClean, LocalBuffer, *pcbClean);
    FillMemory(LocalBuffer, cbEncrypted, 0);

    return TRUE;
}
