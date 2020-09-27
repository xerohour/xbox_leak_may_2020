/////////////////////////////////////////////////////////////////////////////
//  FILE          : swnt_pk.c                                              //
//  DESCRIPTION   :                                                        //
//  Software nametag public key management functions.  These functions     //
//  isolate the peculiarities of public key management without a token     // 
//                                                                         //
//  AUTHOR        :                                                        //
//  HISTORY       :                                                        //
//  Jan 25 1995 larrys   Changed from Nametag                              //
//  Mar 01 1995 terences Fixed key pair handle creation                    //
//  Mar 08 1995 larrys   Fixed warning                                     //
//  Mar 23 1995 larrys   Added variable key length                         //
//  Apr 17 1995 larrys   Added 1024 key gen                                //
//  Apr 19 1995 larrys   Changed CRYPT_EXCH_PUB to AT_KEYEXCHANGE          //
//  Aug 16 1995 larrys   Removed exchange key stuff                        //
//  Sep 12 1995 larrys   Removed 2 DWORDS from exported keys               //
//  Sep 28 1995 larrys   Changed format of PKCS                            //
//  Oct 04 1995 larrys   Fixed problem with PKCS format                    //
//  Oct 27 1995 rajeshk  RandSeed Stuff added hUID to PKCS2Encrypt         //
//  Nov  3 1995 larrys   Merge for NT checkin                              //
//  Dec 11 1995 larrys   Added check for error return from RSA routine     //
//  May 15 1996 larrys  Changed NTE_NO_MEMORY to ERROR_NOT_ENOUGHT...      //
//  Oct 14 1996 jeffspel Changed GenRandoms to NewGenRandoms               //
//                                                                         //
//  Copyright (C) 1993 Microsoft Corporation   All Rights Reserved         //
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"
#include "randlib.h"
#include "ntagum.h"
#include "swnt_pk.h"
#include "protstor.h"
#include "sha.h"
#include "rng.h"

extern CSP_STRINGS g_Strings;

void FIPS186GenRandomWithException(
                      IN HANDLE *phRNGDriver,
                      IN BYTE **ppbContextSeed,
                      IN DWORD *pcbContextSeed,
                      IN OUT BYTE *pb,
                      IN DWORD cb
                      );

// do the modular exponentiation calculation M^PubKey mod N
DWORD RSAPublicEncrypt(
                       IN PEXPO_OFFLOAD_STRUCT pOffloadInfo,
                       IN BSAFE_PUB_KEY *pBSPubKey,
                       IN BYTE *pbInput,
                       IN BYTE *pbOutput
                       )
{
    BOOL            fOffloadSuccess = FALSE;
    DWORD           cbMod;
    DWORD           dw = 1;

    //
    // Two checks (needed for FIPS) before offloading to the offload
    // module.
    //
    // First check is if there is an offload module, this
    // will only be the case if the pOffloadInfo is not NULL.
    //
    // Second check is if this public key is OK, by checking
    // the magic value in the key struct.
    //
    if (NULL != pOffloadInfo)
    {
        if (RSA1 != pBSPubKey->magic)
        {
            SetLastError((DWORD)NTE_BAD_KEY);
            goto Ret;
        }

        cbMod = (pBSPubKey->bitlen + 7) / 8;
        fOffloadSuccess = ModularExpOffload(pOffloadInfo,
                                            pbInput,
                                            (BYTE*)&(pBSPubKey->pubexp),
                                            sizeof(pBSPubKey->pubexp),
                                            (BYTE*)pBSPubKey +
                                                   sizeof(BSAFE_PUB_KEY),
                                            cbMod, pbOutput, NULL, 0);
    }

    if (!fOffloadSuccess)
    {
        BSafeEncPublic(pBSPubKey, pbInput, pbOutput);
        dw = 0;
    }
    else
    {
        dw = 0;
    }
Ret:
    return dw;
}

// do the modular exponentiation calculation M^PrivKey Exponent mod N
DWORD RSAPrivateDecrypt(
                        IN PEXPO_OFFLOAD_STRUCT pOffloadInfo,
                        IN BSAFE_PRV_KEY *pBSPrivKey,
                        IN BYTE *pbInput,
                        IN BYTE *pbOutput
                        )
{
    BOOL            fOffloadSuccess = FALSE;
    DWORD           cbMod;
    DWORD           cbHalfKeylen;
    DWORD           dw = 1;

    //
    // Two checks (needed for FIPS) before offloading to the offload
    // module.
    //
    // First check is if there is an offload module, this
    // will only be the case if the pOffloadInfo is not NULL.
    //
    // Second check is if this private key is OK, by checking
    // the magic value in the key struct.
    //
    if (NULL != pOffloadInfo)
    {
        if (RSA2 != pBSPrivKey->magic)
        {
            SetLastError((DWORD)NTE_BAD_KEY);
            goto Ret;
        }

        cbMod = (pBSPrivKey->bitlen + 7) / 8;
        cbHalfKeylen = (pBSPrivKey->keylen + 1) / 2;
        fOffloadSuccess = ModularExpOffload(pOffloadInfo,
                                    pbInput,
                                    (BYTE*)pBSPrivKey + sizeof(BSAFE_PUB_KEY)
                                           + cbHalfKeylen * 7,
                                    cbMod,
                                    (BYTE*)pBSPrivKey + sizeof(BSAFE_PUB_KEY),
                                    cbMod, pbOutput, NULL, 0);
    }

    if (!fOffloadSuccess)
    {
        BSafeDecPrivate(pBSPrivKey, pbInput, pbOutput)
        dw = 0;
    }
    else
    {
        dw = 0;
    }
Ret:
    return dw;
}

BOOL CheckDataLenForRSAEncrypt(
                               IN DWORD cbMod,   // length of the modulus
                               IN DWORD cbData,  // length of the data
                               IN DWORD dwFlags  // flags
                               )
{
    BOOL    fRet = FALSE;

    // if the OAEP flag is set then check for that length
    if (dwFlags & CRYPT_OAEP)
    {
        if (cbMod < (cbData + A_SHA_DIGEST_LEN * 2 + 1))
            goto Ret;
    }
    // Check for PKCS 1 type 2 padding
    else
    {
        // one byte for the top zero byte, one byte for the type,
        // and one byte for the low zero byte,
        // plus a minimum padding string is 8 bytes
        if (cbMod < (cbData + 11))
            goto Ret;
    }
    fRet = TRUE;
Ret:
    return fRet;
}

/************************************************************************/
/* MaskGeneration generates a mask for OAEP based on the SHA1 hash      */
/* function.                                                            */
/* NULL for the ppbMask parameter indicates the buffer is to be alloced.*/
/************************************************************************/
BOOL MaskGeneration(
                    IN BYTE *pbSeed,
                    IN DWORD cbSeed,
                    IN DWORD cbMask,
                    OUT BYTE **ppbMask,
                    IN BOOL fAlloc
                    )
{
    DWORD       dwCount;
    BYTE        rgbCount[sizeof(DWORD)];
    BYTE        *pbCount;
    A_SHA_CTX   SHA1Ctxt;
    DWORD       cb = cbMask;
    BYTE        *pb;
    DWORD       i;
    DWORD       j;
    BOOL        fRet = FALSE;

    // NULL for *ppbMask indicates the buffer is to be alloced
    if (fAlloc)
    {
        if (NULL == (*ppbMask = (BYTE*)_nt_malloc(cbMask)))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
    }
    pb = *ppbMask;

    dwCount = (cbMask + (A_SHA_DIGEST_LEN - 1)) / A_SHA_DIGEST_LEN;

    for (i = 0; i < dwCount; i++)
    {
        // clear the hash context
        memset(&SHA1Ctxt, 0, sizeof(SHA1Ctxt));

        // hash the seed and the count
        A_SHAInit(&SHA1Ctxt);
        A_SHAUpdate(&SHA1Ctxt, pbSeed, cbSeed);
        // Reverse the count bytes
        pbCount = (BYTE*)&i;
        for (j = 0; j < sizeof(DWORD); j++)
        {
            rgbCount[j] = pbCount[sizeof(DWORD) - j - 1];
        }
        A_SHAUpdate(&SHA1Ctxt, rgbCount, sizeof(DWORD));
        A_SHAFinal(&SHA1Ctxt, SHA1Ctxt.HashVal);

        // copy the bytes from this hash into the mask buffer
        if (cb >= A_SHA_DIGEST_LEN)
        {
            memcpy(pb, SHA1Ctxt.HashVal, A_SHA_DIGEST_LEN);
        }
        else
        {
            memcpy(pb, SHA1Ctxt.HashVal, cb);
            break;
        }
        cb -= A_SHA_DIGEST_LEN;
        pb += A_SHA_DIGEST_LEN;
    }

    fRet = TRUE;
Ret:
    return fRet;
}

/************************************************************************/
/* ApplyPadding applies OAEP (Bellare-Rogoway) padding to a RSA key     */
/* blob.  The function does the seed generation, MGF and masking.       */
/************************************************************************/
BOOL ApplyPadding (
                   IN PNTAGUserList pTmpUser,
                   IN OUT BYTE* pb,             // buffer
                   IN DWORD cb                  // length of the data to mask
                                                // not including seed
                   )
{
    BYTE        rgbSeed[A_SHA_DIGEST_LEN];
    BYTE        rgbDigest[A_SHA_DIGEST_LEN];
    BYTE        *pbMask = NULL;
    BYTE        rgbSeedMask[A_SHA_DIGEST_LEN];
    BYTE        *pbSeedMask;
    DWORD       i;
    BOOL        fRet = FALSE;

    // generate the random seed
    if (!FIPS186GenRandom(&pTmpUser->hRNGDriver,
                          &pTmpUser->ContInfo.pbRandom,
                          &pTmpUser->ContInfo.ContLens.cbRandom,
                          rgbSeed, A_SHA_DIGEST_LEN))
    {
        SetLastError((DWORD)NTE_FAIL);
        goto Ret;
    }

    // generate the data mask from the seed
    if (!MaskGeneration(rgbSeed, sizeof(rgbSeed), cb, &pbMask, TRUE))
    {
        goto Ret;
    }

    // XOR the data mask with the data
    for (i = 0; i < cb; i++)
    {
        pb[i + A_SHA_DIGEST_LEN + 1] = pb[i + A_SHA_DIGEST_LEN + 1] ^ pbMask[i];
    }

    // generate the seed mask from the masked data
    pbSeedMask = rgbSeedMask;
    if (!MaskGeneration(pb + A_SHA_DIGEST_LEN + 1, cb,
                        A_SHA_DIGEST_LEN, &pbSeedMask, FALSE))
    {
        goto Ret;
    }

    // XOR the seed mask with the seed and put that into the
    // pb buffer
    for (i = 0; i < A_SHA_DIGEST_LEN; i++)
    {
        pb[i + 1] = rgbSeed[i] ^ rgbSeedMask[i];
    }

    fRet = TRUE;
Ret:
    if (pbMask)
        _nt_free(pbMask, cb);
    return fRet;
}

/************************************************************************/
/* RemovePadding checks OAEP (Bellare-Rogoway) padding on a RSA decrypt */
/* blob.                                                                */
/************************************************************************/
BOOL RemovePadding (
                    IN OUT BYTE* pb,
                    IN DWORD cb
                    )
{
    BYTE    rgbSeedMask[A_SHA_DIGEST_LEN];
    BYTE    *pbSeedMask;
    BYTE    *pbMask = NULL;
    DWORD   i;
    BOOL    fRet = FALSE;

    memset(rgbSeedMask, 0, A_SHA_DIGEST_LEN);

    // check the most significant byte is 0x00
    if (0x00 != pb[0])
    {
        SetLastError((DWORD)NTE_BAD_DATA);
        goto Ret;
    }

    // generate the seed mask from the masked data
    pbSeedMask = rgbSeedMask;
    if (!MaskGeneration(pb + A_SHA_DIGEST_LEN + 1, cb - (A_SHA_DIGEST_LEN + 1),
                        A_SHA_DIGEST_LEN, &pbSeedMask, FALSE))
    {
        goto Ret;
    }

    // XOR the seed mask with the seed and put that into the
    // pb buffer
    for (i = 0; i < A_SHA_DIGEST_LEN; i++)
    {
        pb[i + 1] = pb[i + 1] ^ rgbSeedMask[i];
    }

    // generate the data mask from the seed
    if (!MaskGeneration(pb + 1, A_SHA_DIGEST_LEN,
                        cb - (A_SHA_DIGEST_LEN + 1), &pbMask, TRUE))
    {
        goto Ret;
    }

    // XOR the data mask with the data
    for (i = 0; i < cb - (A_SHA_DIGEST_LEN + 1); i++)
    {
        pb[i + A_SHA_DIGEST_LEN + 1] =
            pb[i + A_SHA_DIGEST_LEN + 1] ^ pbMask[i];
    }

    fRet = TRUE;
Ret:
    if (pbMask)
        _nt_free(pbMask, cb - (A_SHA_DIGEST_LEN + 1));
    return fRet;
}

/************************************************************************/
/* OAEPEncrypt performs a RSA encryption using OAEP (Bellare-Rogoway)   */
/* as the padding scheme.  The current implementation uses SHA1 as the  */
/* hash function.                                                       */
/************************************************************************/
BOOL OAEPEncrypt(
                IN PNTAGUserList pTmpUser,
                IN BSAFE_PUB_KEY *pBSPubKey,
                IN BYTE *pbPlaintext,
                IN DWORD cbPlaintext,
                IN BYTE *pbParams,
                IN DWORD cbParams,
                OUT BYTE *pbOut
                )
{
    BYTE        *pbInput = NULL;
    BYTE        *pbOutput = NULL;
    BYTE        *pbReverse = NULL;
    DWORD       dwLen;
    A_SHA_CTX   SHA1Ctxt;
    DWORD       i;
    DWORD       cb;
    DWORD       dwLastErr = 0;
    BOOL        fSucc = FALSE;

    memset(&SHA1Ctxt, 0, sizeof(SHA1Ctxt));

    // start off by hashing the Encoding parameters (pbParams)
    A_SHAInit(&SHA1Ctxt);
    if (0 != cbParams)
    {
        A_SHAUpdate(&SHA1Ctxt, pbParams, cbParams);
    }
    A_SHAFinal(&SHA1Ctxt, SHA1Ctxt.HashVal);

    // alloc space for an internal buffer
    if(NULL == (pbInput = (BYTE *)_nt_malloc(pBSPubKey->keylen * 2 +
                                             ((pBSPubKey->bitlen + 7) / 8))))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }
    pbOutput = pbInput + pBSPubKey->keylen;
    pbReverse = pbInput + pBSPubKey->keylen * 2;

    // add the pHash
    memcpy(pbReverse + A_SHA_DIGEST_LEN + 1, SHA1Ctxt.HashVal,
           A_SHA_DIGEST_LEN);

    // figure the length of PS, 

    // put the 0x01 byte in, skipping past the PS,
    // note that the PS is zero bytes so it is just there
    cb = ((pBSPubKey->bitlen + 7) / 8) - (1 + cbPlaintext);
    pbReverse[cb] = 0x01;
    cb++;

    // copy in the message bytes
    memcpy(pbReverse + cb, pbPlaintext, cbPlaintext);

    // do the seed generation, MGF and masking
    cb = ((pBSPubKey->bitlen + 7) / 8) - (A_SHA_DIGEST_LEN + 1);
    if (!ApplyPadding(pTmpUser, pbReverse, cb))
    {
        goto Ret;
    }

    // byte reverse the whole thing before RSA encrypting
    for (i = 0; i < (pBSPubKey->bitlen + 7) / 8; i++)
    {
        pbInput[i] = pbReverse[((pBSPubKey->bitlen + 7) / 8) - i - 1];
    }

    // RSA encrypt this
    if (0 != RSAPublicEncrypt(pTmpUser->pOffloadInfo, pBSPubKey, pbInput, pbOutput))
    {
        SetLastError((DWORD)NTE_FAIL);
        goto Ret;
    }

    memcpy(pbOut, pbOutput, (pBSPubKey->bitlen + 7) / 8);

    fSucc = TRUE;
Ret:
    if (pbInput)
    {
        memset(pbInput, 0, pBSPubKey->keylen * 2 + (pBSPubKey->bitlen + 7) / 8);
        _nt_free(pbInput, pBSPubKey->keylen * 2 + (pBSPubKey->bitlen + 7) / 8);
    }

    return fSucc;
}

/************************************************************************/
/* OAEPDecrypt performs a RSA decryption checking that OAEP             */
/* (Bellare-Rogoway) is the padding scheme.  The current implementation */
/* uses SHA1 as the hash function.                                      */
/************************************************************************/
BOOL OAEPDecrypt(
                IN PNTAGUserList pTmpUser,
                IN BSAFE_PRV_KEY *pBSPrivKey,
                IN BYTE *pbBlob,
                IN DWORD cbBlob,
                IN BYTE *pbParams,
                IN DWORD cbParams,
                OUT BYTE **ppbPlaintext,
                OUT DWORD *pcbPlaintext
                )
{
    BYTE*       pbOutput = NULL;
    BYTE*       pbInput = NULL;
    BYTE*       pbReverse = NULL;
    A_SHA_CTX   SHA1Ctxt;
    DWORD       cb;
    DWORD       i;
    BOOL        fRet = FALSE;

    memset(&SHA1Ctxt, 0, sizeof(SHA1Ctxt));

    cb = (pBSPrivKey->bitlen + 7) / 8;
    if (cbBlob > cb)
    {
        SetLastError((DWORD)NTE_BAD_DATA);
        goto Ret;
    }

    if (NULL == (pbOutput = (BYTE *)_nt_malloc((pBSPrivKey->keylen + 2) * 2 + cb)))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }
    pbInput = pbOutput + pBSPrivKey->keylen + 2;
    pbReverse = pbOutput + (pBSPrivKey->keylen + 2) * 2;

    // perform the RSA decryption
    memcpy(pbInput, pbBlob, cb);
    if (0 != RSAPrivateDecrypt(pTmpUser->pOffloadInfo, pBSPrivKey, pbInput, pbOutput))
    {
        SetLastError((DWORD)NTE_FAIL);
        goto Ret;
    }

    for (i = 0; i < cb; i++)
    {
        pbReverse[i] = pbOutput[cb - i - 1];
    }
    // remove OAEP (Bellare-Rogoway) padding
    if (!RemovePadding(pbReverse, cb))
    {
        goto Ret;
    }

    // hash the Encoding parameters (pbParams)
    A_SHAInit(&SHA1Ctxt);
    if (0 != cbParams)
    {
        A_SHAUpdate(&SHA1Ctxt, pbParams, cbParams);
    }
    A_SHAFinal(&SHA1Ctxt, SHA1Ctxt.HashVal);

    // check the hash of the encoding parameters against the message
    if (0 != memcmp(SHA1Ctxt.HashVal, pbReverse + A_SHA_DIGEST_LEN + 1,
                    A_SHA_DIGEST_LEN))
    {
        SetLastError((DWORD)NTE_BAD_DATA);
        goto Ret;
    }

    // check the zero bytes and check the 0x01 byte
    for (i = A_SHA_DIGEST_LEN * 2 + 1; i < cb; i++)
    {
        if (0x01 == pbReverse[i])
        {
            i++;
            break;
        }
        else if (0x00 != pbReverse[i])
        {
            SetLastError((DWORD)NTE_BAD_DATA);
            goto Ret;
        }
    }

    *pcbPlaintext = cb - i;
    if (NULL == (*ppbPlaintext = (BYTE*)_nt_malloc(*pcbPlaintext)))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }
    memcpy(*ppbPlaintext, pbReverse + i, *pcbPlaintext);

    fRet = TRUE;
Ret:
    // scrub the output buffer
    if (pbOutput)
    {
        memset(pbOutput, 0, pBSPrivKey->keylen * 2 + cb);
        _nt_free(pbOutput, pBSPrivKey->keylen * 2 + cb);
    }

    return fRet;
}


BOOL PKCS2Encrypt(
                  PNTAGUserList pTmpUser,
                  DWORD dwFlags,
                  BSAFE_PUB_KEY *pKey,
                  BYTE *InBuf,
                  DWORD InBufLen,
                  BYTE *OutBuf
                  )
{
    DWORD   i;
    BYTE    *pScratch = NULL;
    BYTE    *pScratch2 = NULL;
    BYTE    *pLocal;
    DWORD   temp;
    DWORD   z;
    BOOL    fRet = FALSE;

    if ((pScratch = (BYTE *)_nt_malloc(pKey->keylen)) == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    if ((pScratch2 = (BYTE *)_nt_malloc(pKey->keylen)) == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }
    
    memset(pScratch, 0, pKey->keylen);

    pScratch[pKey->datalen - 1] = PKCS_BLOCKTYPE_2;
    if (!FIPS186GenRandom(&pTmpUser->hRNGDriver,
                          &pTmpUser->ContInfo.pbRandom,
                          &pTmpUser->ContInfo.ContLens.cbRandom,
                          pScratch+InBufLen+1, (pKey->datalen)-InBufLen-2))
    {
        SetLastError(NTE_FAIL);
        goto Ret;
    }

    pLocal = pScratch + InBufLen + 1;

    // Need to insure that none of the padding bytes are zero.
    temp = pKey->datalen - InBufLen - 2;
    while (temp)
    {
        if (*pLocal == 0)
        {
            if (!FIPS186GenRandom(&pTmpUser->hRNGDriver,
                                  &pTmpUser->ContInfo.pbRandom,
                                  &pTmpUser->ContInfo.ContLens.cbRandom,
                                  pLocal, 1))
            {
                SetLastError(NTE_FAIL);
                goto Ret;
            }
        }
        else
        {
            pLocal++;
            temp--;
        }
    }
    
#ifdef CSP_USE_SSL3
    // if SSL2_FALLBACK has been specified then put threes in the 8
    // least significant bytes of the random padding
    if (CRYPT_SSL2_FALLBACK & dwFlags)
    {
        memset(pScratch + InBufLen + 1, 0x03, 8);
    }
#endif

    // Reverse the session key bytes
    for (z = 0; z < InBufLen; ++z) pScratch[z] = InBuf[InBufLen - z - 1];

    if (0 != RSAPublicEncrypt(pTmpUser->pOffloadInfo, pKey, pScratch, pScratch2))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    memcpy(OutBuf, pScratch2, pKey->keylen - 2*sizeof(DWORD));

    fRet = TRUE;
Ret:
    if (pScratch)
        _nt_free(pScratch, pKey->keylen);
    if (pScratch2)
        _nt_free(pScratch2, pKey->keylen);
    
    return fRet;
}

BOOL PKCS2Decrypt(
                  IN PNTAGUserList pTmpUser,
                  IN BSAFE_PRV_KEY *pKey,
                  IN DWORD dwFlags,
                  IN BYTE *InBuf,
                  OUT BYTE **ppbOutBuf,
                  OUT DWORD *pcbOutBuf)
{
    DWORD   i;
    BYTE    *pScratch = NULL;
    BYTE    *pScratch2 = NULL;
    DWORD   z;
    BOOL    fRet = FALSE;

    if (NULL == (pScratch = (BYTE *)_nt_malloc(pKey->keylen * 2)))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }
    pScratch2 = pScratch + pKey->keylen;

    memcpy(pScratch2, InBuf, pKey->keylen - 2*sizeof(DWORD));

    if (0 != RSAPrivateDecrypt(pTmpUser->pOffloadInfo, pKey, pScratch2, pScratch))
    {
        SetLastError((DWORD)NTE_FAIL);
        goto Ret;
    }

    if ((pScratch[pKey->datalen - 1] != PKCS_BLOCKTYPE_2) ||
        (pScratch[pKey->datalen] != 0))
    {
        SetLastError((DWORD) NTE_BAD_DATA);
        goto Ret;
    }

    i = pKey->datalen - 2;

    while((i > 0) && (pScratch[i]))
        i--;

    if ((*ppbOutBuf = (BYTE *)_nt_malloc(i)) == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }
    *pcbOutBuf = i;

#ifdef CSP_USE_SSL3
    // if SSL2_FALLBACK has been specified then check if threes
    // are in the 8  least significant bytes of the random padding
    if (CRYPT_SSL2_FALLBACK & dwFlags)
    {
        BOOL fFallbackError = TRUE;

        for(z = i + 1; z < i + 9; z++)
        {
            if (0x03 != pScratch[z])
            {
                fFallbackError = FALSE;
                break;
            }
        }
        if(fFallbackError)
        {
            SetLastError((DWORD)NTE_BAD_VER);
            goto Ret;
        }
    }
#endif

    // Reverse the session key bytes
    for (z = 0; z < i; ++z)
        (*ppbOutBuf)[z] = pScratch[i - z - 1];

    fRet = TRUE;
Ret:
    if (pScratch)
        _nt_free(pScratch, pKey->keylen);

    return fRet;
}

/************************************************************************/
/* RSAEncrypt performs a RSA encryption.                                */
/************************************************************************/
BOOL RSAEncrypt(
                IN PNTAGUserList pTmpUser,
                IN BSAFE_PUB_KEY *pBSPubKey,
                IN BYTE *pbPlaintext,
                IN DWORD cbPlaintext,
                IN BYTE *pbParams,
                IN DWORD cbParams,
                IN DWORD dwFlags,
                OUT BYTE *pbOut
                )
{
    BOOL    fRet = FALSE;

    // check the length of the data
    if (!CheckDataLenForRSAEncrypt((pBSPubKey->bitlen + 7) / 8,
                                   cbPlaintext, dwFlags))
    {
        goto Ret;
    }

    // use OAEP if the flag is set
    if (dwFlags & CRYPT_OAEP)
    {
        if (!OAEPEncrypt(pTmpUser, pBSPubKey, pbPlaintext,
                         cbPlaintext, pbParams, cbParams, pbOut))
        {
            goto Ret;
        }
    }
    // use PKCS #1 Type 2
    else
    {
        if (!PKCS2Encrypt(pTmpUser, dwFlags, pBSPubKey,
                          pbPlaintext, cbPlaintext, pbOut))
        {
            goto Ret;
        }
    }

    fRet = TRUE;
Ret:
    return fRet;
}

/************************************************************************/
/* RSADecrypt performs a RSA decryption.                                */
/************************************************************************/
BOOL RSADecrypt(
                IN PNTAGUserList pTmpUser,
                IN BSAFE_PRV_KEY *pBSPrivKey,
                IN BYTE *pbBlob,
                IN DWORD cbBlob,
                IN BYTE *pbParams,
                IN DWORD cbParams,
                IN DWORD dwFlags,
                OUT BYTE **ppbPlaintext,
                OUT DWORD *pcbPlaintext
                )
{
    BOOL    fRet = FALSE;

    // use OAEP if the flag is set
    if (dwFlags & CRYPT_OAEP)
    {
        if (!OAEPDecrypt(pTmpUser, pBSPrivKey, pbBlob, cbBlob, pbParams,
                         cbParams, ppbPlaintext, pcbPlaintext))
        {
            goto Ret;
        }
    }
    // use PKCS #1 Type 2
    else
    {
        if (!PKCS2Decrypt(pTmpUser, pBSPrivKey, dwFlags, pbBlob, ppbPlaintext,
                          pcbPlaintext))
        {
            goto Ret;
        }
    }

    fRet = TRUE;
Ret:
    return fRet;
}

//
// Function : EncryptAndDecryptWithRSAKey
//
// Description : This function creates a buffer and then encrypts that with
//               the passed in private key and decrypts with the passed in
//               public key.  The function is used for FIPS 140-1 compliance
//               to make sure that newly generated/imported keys work and
//               in the self test during DLL initialization.
//

DWORD EncryptAndDecryptWithRSAKey(
                                  IN BYTE *pbRSAPub,
                                  IN BYTE *pbRSAPriv,
                                  IN BOOL fSigKey,
                                  IN BOOL fEncryptCheck
                                  )
{
    BSAFE_PRV_KEY   *pBSafePriv = (BSAFE_PRV_KEY*)pbRSAPriv;
    BYTE            *pb = NULL;
    DWORD           cb;
    DWORD           cbKey;
    DWORD           i;
    DWORD           dwErr = 0;

    // alloc space for the plaintext and ciphertext
    cb = pBSafePriv->keylen;
    cbKey = pBSafePriv->bitlen / 8;
    if (NULL == (pb = _nt_malloc(cb * 3)))
    {
        dwErr = ERROR_NOT_ENOUGH_MEMORY;
        goto Ret;
    }

    // reverse the hash so it is in little endian
    for (i = 0; i < 16; i++)
    {
        pb[i] = (BYTE)i + 1;
    }
    memset(pb + 17, 0xFF, cbKey - 18);

    if (fSigKey)
    {
        // encrypt with the private key
        BSafeDecPrivate(pBSafePriv, pb, pb + cb);
    }
    else
    {
        // encrypt with the public key
        BSafeEncPublic((BSAFE_PUB_KEY*)pbRSAPub,
                                      pb,
                                      pb + cb);
    }

    // we can't do this check when importing private keys since many
    // applications use private keys with exponent of 1 to import
    // plaintext symmetric keys
    if (fEncryptCheck)
    {
        if (0 == (memcmp(pb, pb + cb, cb)))
        {
            dwErr = NTE_BAD_KEY;
            goto Ret;
        }
    }

    if (fSigKey)
    {
        // decrypt with the public key
        BSafeEncPublic((BSAFE_PUB_KEY*)pbRSAPub,
                                      pb + cb,
                                      pb + (cb * 2));
    }
    else
    {
        // encrypt with the private key
        BSafeDecPrivate(pBSafePriv, pb + cb, pb + (cb * 2));
    }

    // compare to the plaintext and the decrypted text
    if (memcmp(pb, pb + cb * 2, cbKey))
    {
        dwErr = (DWORD)NTE_BAD_KEY;
        goto Ret;
    }
Ret:
    if (pb)
        _nt_free(pb, cbB * 3);

    return dwErr;
}

BOOL ReGenKey(HCRYPTPROV hUser,
              DWORD dwFlags,
              DWORD dwWhichKey,
              HCRYPTKEY *phKey,
              DWORD bits)
{
    BYTE                **ThisPubKey, **ThisPrivKey;
    DWORD               *pThisPubLen, *pThisPrivLen;
    BYTE                *pNewPubKey = NULL;
    BYTE                *pNewPrivKey = NULL;
    DWORD               PrivateKeySize, PublicKeySize;
    DWORD               localbits;
    PNTAGUserList       pOurUser;
    BOOL                fSigKey;
    LPWSTR              szPrompt;
    BOOL                *pfExportable;
    BOOL                fAlloc = FALSE;
    BOOL                fInCritSec = FALSE;
    DWORD               dwErr;
    BSAFE_OTHER_INFO    OtherInfo;
    BSAFE_OTHER_INFO    *pOtherInfo = NULL;
    BOOL                fRet = FALSE;

    memset(&OtherInfo, 0, sizeof(OtherInfo));

    // ## MTS: No user structure locking
    if ((pOurUser = (PNTAGUserList) NTLCheckList(hUser, USER_HANDLE)) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    // wrap with a try since there is a critical sections in here
    try
    {
        EnterCriticalSection(&pOurUser->CritSec);
        fInCritSec = TRUE;

        localbits = bits;

        if (!BSafeComputeKeySizes(&PublicKeySize, &PrivateKeySize, &localbits))
        {
            SetLastError((DWORD) NTE_FAIL);
            goto Ret;
        }

        if ((pNewPubKey = (BYTE *)_nt_malloc(PublicKeySize)) == NULL)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
        fAlloc = TRUE;

        // allocate space for the new key exchange public key
        if ((pNewPrivKey = (BYTE *)_nt_malloc(PrivateKeySize)) == NULL)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
    
        // initialize the user's state in the random number generation
#ifndef _XBOX
        if (!InitRand(&pOurUser->ContInfo.pbRandom,
                      &pOurUser->ContInfo.ContLens.cbRandom))
        {
            SetLastError((DWORD) NTE_FAIL);
            goto Ret;
        }
#endif

        // generate the key exchange key pair
        if (INVALID_HANDLE_VALUE != pOurUser->hRNGDriver)
        {
            OtherInfo.pRNGInfo = &pOurUser->hRNGDriver;
            OtherInfo.pFuncRNG = FIPS186GenRandomWithException;
            pOtherInfo = &OtherInfo;
        }
        if (!BSafeMakeKeyPairEx2(pOtherInfo,
                                 (BSAFE_PUB_KEY *) pNewPubKey,
                                 (BSAFE_PRV_KEY *) pNewPrivKey,
                                 bits,
                                 0x10001))
        {
            SetLastError((DWORD) NTE_FAIL);
            goto Ret;
        }

        // uninitialize the user's state from the random number generation
        if (!XRNGGenerateBytes(pOurUser->ContInfo.pbRandom,
                        pOurUser->ContInfo.ContLens.cbRandom))
        {
            SetLastError((DWORD) NTE_FAIL);
            goto Ret;
        }

        // test the RSA key to make sure it works
        if (0 != (dwErr = EncryptAndDecryptWithRSAKey(pNewPubKey, pNewPrivKey, TRUE, TRUE)))
        {
            SetLastError(dwErr);
            goto Ret;
        }

        // test the RSA key to make sure it works
        if (0 != (dwErr = EncryptAndDecryptWithRSAKey(pNewPubKey, pNewPrivKey, FALSE, TRUE)))
        {
            SetLastError(dwErr);
            goto Ret;
        }

        if (dwWhichKey == NTPK_USE_SIG)
        {
            ThisPubKey = &pOurUser->ContInfo.pbSigPub;
            ThisPrivKey = &pOurUser->pSigPrivKey;
            pThisPubLen = &pOurUser->ContInfo.ContLens.cbSigPub;
            pThisPrivLen = &pOurUser->SigPrivLen;
            pfExportable = &pOurUser->ContInfo.fSigExportable;
            fSigKey = TRUE;
            szPrompt = g_Strings.pwszCreateRSASig;
        }
        else
        {
            ThisPubKey = &pOurUser->ContInfo.pbExchPub;
            ThisPrivKey = &pOurUser->pExchPrivKey;
            pThisPubLen = &pOurUser->ContInfo.ContLens.cbExchPub;
            pThisPrivLen = &pOurUser->ExchPrivLen;
            pfExportable = &pOurUser->ContInfo.fExchExportable;
            fSigKey = FALSE;
            szPrompt = g_Strings.pwszCreateRSAExch;
        }

        if (*ThisPubKey)
        {
            ASSERT(*pThisPubLen);
            ASSERT(*pThisPrivLen);
            ASSERT(*ThisPrivKey);
    
            _nt_free (*ThisPubKey, *pThisPubLen);
    
            _nt_free (*ThisPrivKey, *pThisPrivLen);
        }
#ifdef NTAGDEBUG
        else
        {
            ASSERT(*pThisPrivLen == 0);
            ASSERT(*pThisPubLen == 0);
            ASSERT(*ThisPrivKey == 0);
            ASSERT(*ThisPubKey == 0);
        }
#endif

        fAlloc = FALSE;

        *pThisPrivLen = PrivateKeySize;
        *pThisPubLen = PublicKeySize;
        *ThisPrivKey = pNewPrivKey;
        *ThisPubKey = pNewPubKey;

        if (dwFlags & CRYPT_EXPORTABLE)
            *pfExportable = TRUE;
        else
            *pfExportable = FALSE;

        // if the context being used is a Verify Context then the key is not
        // persisted to storage
        if (!(pOurUser->Rights & CRYPT_VERIFYCONTEXT))
        {
            // write the new keys to the user storage file
            if (!ProtectPrivKey(pOurUser, szPrompt, dwFlags, fSigKey))
            {
                goto Ret;          // error already set
            }
        }

        if (dwWhichKey == NTPK_USE_SIG)
        {
            if (!CPGetUserKey(hUser, AT_SIGNATURE, phKey))
                goto Ret;
        }
        else
        {
            if (!CPGetUserKey(hUser, AT_KEYEXCHANGE, phKey))
                goto Ret;
        }

        fRet = TRUE;
    }
    except ( EXCEPTION_EXECUTE_HANDLER )
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }
Ret:
    if (fInCritSec)
    {
        LeaveCriticalSection(&pOurUser->CritSec);
    }
    if (fAlloc)
    {
        if (pNewPrivKey)
            _nt_free(pNewPrivKey, PrivateKeySize);
        if (pNewPubKey)
            _nt_free(pNewPubKey, PublicKeySize);
    }
    
    return fRet;
}

//
// Routine : DerivePublicFromPrivate
//
// Description : Derive the public RSA key from the private RSA key.  This is
//               done and the resulting public key is placed in the appropriate
//               place in the context pointer (pTmpUser).
//

BOOL DerivePublicFromPrivate(
                             IN PNTAGUserList pUser,
                             IN BOOL fSigKey
                             )
{
    DWORD           *pcbPubKey;
    BYTE            **ppbPubKey = NULL;
    BSAFE_PUB_KEY   *pBSafePubKey;
    BSAFE_PRV_KEY   *pBSafePrivKey;
    DWORD           cb;
    BOOL            fRet = FALSE;

    // variable assignments depending on if its sig or exch
    if (fSigKey)
    {
        pcbPubKey = &pUser->ContInfo.ContLens.cbSigPub;
        ppbPubKey = &pUser->ContInfo.pbSigPub;
        pBSafePrivKey = (BSAFE_PRV_KEY*)pUser->pSigPrivKey;
    }
    else
    {
        pcbPubKey = &pUser->ContInfo.ContLens.cbExchPub;
        ppbPubKey = &pUser->ContInfo.pbExchPub;
        pBSafePrivKey = (BSAFE_PRV_KEY*)pUser->pExchPrivKey;
    }

    // figure out how much space is needed for the public key
    cb = ((((pBSafePrivKey->bitlen >> 1) + 63) / 32) * 8); // 8 = 2 * DIGIT_BYTES (rsa_fast.h)
    cb += sizeof(BSAFE_PUB_KEY);

    // check if space has been alloced for the public key and if
    // so is it large enough
    if (cb > *pcbPubKey)
    {
        _nt_free(*ppbPubKey, *pcbPubKey);
        *pcbPubKey = cb;

        if (NULL == (*ppbPubKey = _nt_malloc(*pcbPubKey)))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
    }

    // copy over the public key components
    pBSafePubKey = (BSAFE_PUB_KEY*)*ppbPubKey;
    pBSafePubKey->magic = RSA1;
    pBSafePubKey->keylen = pBSafePrivKey->keylen;
    pBSafePubKey->bitlen = pBSafePrivKey->bitlen;
    pBSafePubKey->datalen = pBSafePrivKey->datalen;
    pBSafePubKey->pubexp = pBSafePrivKey->pubexp;
    memcpy(*ppbPubKey + sizeof(BSAFE_PUB_KEY),
           (BYTE*)pBSafePrivKey + sizeof(BSAFE_PRV_KEY),
           cb - sizeof(BSAFE_PUB_KEY));

    fRet = TRUE;
Ret:
    return fRet;
}


