/////////////////////////////////////////////////////////////////////////////
//  FILE          : ssl3.c                                               //
//  DESCRIPTION   : Code for performing the SSL3 protocol:                 //
//  AUTHOR        :                                                        //
//  HISTORY       :                                                        //
//  Dec  2 1996 jeffspel Created                                           //
//  Apr 8 1997  jeffspel Added PCT1 support
//                                                                         //
//  Copyright (C) 1993 Microsoft Corporation   All Rights Reserved         //
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"
#include "nt_rsa.h"
#include "nt_blobs.h"
#include "manage.h"
#include "ssl3.h"
#ifdef CSP_USE_3DES
#include "tripldes.h"
#endif

#define HMAC_K_PADSIZE              64

BOOL FIsLegalSGCKeySize(
                     IN ALG_ID Algid,
                     IN DWORD cbKey,
                     IN BOOL fRC2BigKeyOK,
                     IN BOOL fGenKey,
                     OUT BOOL *pfPubKey
                     );

BOOL FIsLegalKeySize(
                     IN ALG_ID Algid,
                     IN DWORD cbKey,
                     IN BOOL fRC2BigKeyOK,
                     IN DWORD dwFlags,
                     OUT BOOL *pfPubKey
                     );

BOOL FIsLegalKey(
                 PNTAGUserList pTmpUser,
                 PNTAGKeyList pKey,
                 BOOL fRC2BigKeyOK
                 );

void FreeNewKey(PNTAGKeyList pOldKey);

PNTAGKeyList MakeNewKey(
        ALG_ID      aiKeyAlg,
        DWORD       dwRights,
        DWORD       dwKeyLen,
        HCRYPTPROV  hUID,
        BYTE        *pbKeyData,
        BOOL        fUsePassedKeyBuffer
    );

BOOL MyPrimitiveSHA(
			PBYTE       pbData, 
			DWORD       cbData,
            BYTE        rgbHash[A_SHA_DIGEST_LEN])
{
    BOOL fRet = FALSE;
    A_SHA_CTX   sSHAHash;

            
    A_SHAInit(&sSHAHash);
    A_SHAUpdate(&sSHAHash, (BYTE *) pbData, cbData);
    A_SHAFinal(&sSHAHash, rgbHash);

    fRet = TRUE;
//Ret:

    return fRet;
}                                

BOOL MyPrimitiveMD5(
			PBYTE       pbData, 
			DWORD       cbData,
            BYTE        rgbHash[MD5DIGESTLEN])
{
    BOOL fRet = FALSE;
    MD5_CTX   sMD5Hash;

            
    MD5Init(&sMD5Hash);
    MD5Update(&sMD5Hash, (BYTE *) pbData, cbData);
    MD5Final(&sMD5Hash);
    memcpy(rgbHash, sMD5Hash.digest, MD5DIGESTLEN);

    fRet = TRUE;
//Ret:

    return fRet;
}                                

BOOL MyPrimitiveHMACParam(
        PBYTE       pbKeyMaterial, 
        DWORD       cbKeyMaterial,
        PBYTE       pbData, 
        DWORD       cbData,
        ALG_ID      Algid,
        BYTE        rgbHMAC[A_SHA_DIGEST_LEN])
{
    BYTE    rgbFirstHash[A_SHA_DIGEST_LEN];
    BYTE    rgbHMACTmp[HMAC_K_PADSIZE+A_SHA_DIGEST_LEN];
    BOOL    fRet = FALSE;

    BYTE    rgbKipad[HMAC_K_PADSIZE];
    BYTE    rgbKopad[HMAC_K_PADSIZE];
    DWORD   dwBlock;

    // truncate
    if (cbKeyMaterial > HMAC_K_PADSIZE)
        cbKeyMaterial = HMAC_K_PADSIZE;

    
    ZeroMemory(rgbKipad, HMAC_K_PADSIZE);
    CopyMemory(rgbKipad, pbKeyMaterial, cbKeyMaterial);

    ZeroMemory(rgbKopad, HMAC_K_PADSIZE);
    CopyMemory(rgbKopad, pbKeyMaterial, cbKeyMaterial);

    // Kipad, Kopad are padded sMacKey. Now XOR across...
    for(dwBlock=0; dwBlock<HMAC_K_PADSIZE/sizeof(DWORD); dwBlock++)
    {
        ((DWORD*)rgbKipad)[dwBlock] ^= 0x36363636;
        ((DWORD*)rgbKopad)[dwBlock] ^= 0x5C5C5C5C;
    }

    // prepend Kipad to data, Hash to get H1
    if (CALG_SHA1 == Algid)
    {
        // do this inline since it would require data copy
        A_SHA_CTX   sSHAHash;

        A_SHAInit(&sSHAHash);
        A_SHAUpdate(&sSHAHash, rgbKipad, HMAC_K_PADSIZE);
        A_SHAUpdate(&sSHAHash, pbData, cbData);

        // Finish off the hash
        A_SHAFinal(&sSHAHash, sSHAHash.HashVal);

        // prepend Kopad to H1, hash to get HMAC
        CopyMemory(rgbHMACTmp, rgbKopad, HMAC_K_PADSIZE);
        CopyMemory(rgbHMACTmp+HMAC_K_PADSIZE, sSHAHash.HashVal, A_SHA_DIGEST_LEN);

        if (!MyPrimitiveSHA(
			    rgbHMACTmp, 
			    HMAC_K_PADSIZE + A_SHA_DIGEST_LEN,
                rgbHMAC))
            goto Ret;
    }
    else
    {
        // do this inline since it would require data copy
        MD5_CTX   sMD5Hash;
            
        MD5Init(&sMD5Hash);
        MD5Update(&sMD5Hash, rgbKipad, HMAC_K_PADSIZE);
        MD5Update(&sMD5Hash, pbData, cbData);
        MD5Final(&sMD5Hash);

        // prepend Kopad to H1, hash to get HMAC
        CopyMemory(rgbHMACTmp, rgbKopad, HMAC_K_PADSIZE);
        CopyMemory(rgbHMACTmp+HMAC_K_PADSIZE, sMD5Hash.digest, MD5DIGESTLEN);

        if (!MyPrimitiveMD5(
			    rgbHMACTmp, 
			    HMAC_K_PADSIZE + MD5DIGESTLEN,
                rgbHMAC))
            goto Ret;
    }

    fRet = TRUE;
Ret:

    return fRet;    
}

//+ ---------------------------------------------------------------------
// the P_Hash algorithm from TLS 
BOOL P_Hash
(
    PBYTE  pbSecret,
    DWORD  cbSecret, 

    PBYTE  pbSeed,  
    DWORD  cbSeed,  

    ALG_ID Algid,

    PBYTE  pbKeyOut, //Buffer to copy the result...
    DWORD  cbKeyOut  //# of bytes of key length they want as output.
)
{
    BOOL    fRet = FALSE;
    BYTE    rgbDigest[A_SHA_DIGEST_LEN];      
    DWORD   iKey;
    DWORD   cbHash;

    PBYTE   pbAofiDigest = NULL;

    if (NULL == (pbAofiDigest = (BYTE*)_nt_malloc(cbSeed + A_SHA_DIGEST_LEN)))
        goto Ret;

    if (CALG_SHA1 == Algid)
    {
        cbHash = A_SHA_DIGEST_LEN;
    }
    else
    {
        cbHash = MD5DIGESTLEN;
    }

//   First, we define a data expansion function, P_hash(secret, data)
//   which uses a single hash function to expand a secret and seed into
//   an arbitrary quantity of output:

//       P_hash(secret, seed) = HMAC_hash(secret, A(1) + seed) +
//                              HMAC_hash(secret, A(2) + seed) +
//                              HMAC_hash(secret, A(3) + seed) + ...

//   Where + indicates concatenation.

//   A() is defined as:
//       A(0) = seed
//       A(i) = HMAC_hash(secret, A(i-1))


    // build A(1)
    if (!MyPrimitiveHMACParam(pbSecret, cbSecret, pbSeed, cbSeed,
                              Algid, pbAofiDigest))
        goto Ret;

    // create Aofi: (  A(i) | seed )
    CopyMemory(&pbAofiDigest[cbHash], pbSeed, cbSeed);

    for (iKey=0; cbKeyOut; iKey++)
    {
        // build Digest = HMAC(key | A(i) | seed);
        if (!MyPrimitiveHMACParam(pbSecret, cbSecret, pbAofiDigest,
                                  cbSeed + cbHash, Algid, rgbDigest))
            goto Ret;

        // append to pbKeyOut
        if(cbKeyOut < cbHash)
        {
            CopyMemory(pbKeyOut, rgbDigest, cbKeyOut);
            break;
        }
        else
        {
            CopyMemory(pbKeyOut, rgbDigest, cbHash);
            pbKeyOut += cbHash;
        }

        cbKeyOut -= cbHash;

        // build A(i) = HMAC(key, A(i-1))
        if (!MyPrimitiveHMACParam(pbSecret, cbSecret, pbAofiDigest, cbHash,
                                  Algid, pbAofiDigest))
            goto Ret;
    }

    fRet = TRUE;
Ret:
    if (pbAofiDigest)
        _nt_free(pbAofiDigest, cbSeed + A_SHA_DIGEST_LEN);

    return fRet;
}

BOOL PRF(
    PBYTE  pbSecret,
    DWORD  cbSecret, 

    PBYTE  pbLabel,  
    DWORD  cbLabel,
    
    PBYTE  pbSeed,  
    DWORD  cbSeed,  

    PBYTE  pbKeyOut, //Buffer to copy the result...
    DWORD  cbKeyOut  //# of bytes of key length they want as output.
    )
{
    BYTE    *pbBuff = NULL;
    BYTE    *pbLabelAndSeed = NULL;
    DWORD   cbLabelAndSeed;
    DWORD   cbOdd;
    DWORD   cbHalfSecret;
    DWORD   i;
    BOOL    fRet = FALSE;

    cbOdd = cbSecret % 2;
    cbHalfSecret = cbSecret / 2;

    cbLabelAndSeed = cbLabel + cbSeed;
    if (NULL == (pbLabelAndSeed = (BYTE*)_nt_malloc(cbLabelAndSeed)))
        goto Ret;
    if (NULL == (pbBuff = (BYTE*)_nt_malloc(cbKeyOut)))
        goto Ret;

    // copy label and seed into one buffer
    memcpy(pbLabelAndSeed, pbLabel, cbLabel);
    memcpy(pbLabelAndSeed + cbLabel, pbSeed, cbSeed);

    // Use P_hash to calculate MD5 half
    if (!P_Hash(pbSecret, cbHalfSecret + cbOdd, pbLabelAndSeed,  
                cbLabelAndSeed, CALG_MD5, pbKeyOut, cbKeyOut))
        goto Ret;

    // Use P_hash to calculate SHA half
    if (!P_Hash(pbSecret + cbHalfSecret, cbHalfSecret + cbOdd, pbLabelAndSeed,  
                cbLabelAndSeed, CALG_SHA1, pbBuff, cbKeyOut))
        goto Ret;

    // XOR the two halves
    for (i=0;i<cbKeyOut;i++)
    {
        pbKeyOut[i] = pbKeyOut[i] ^ pbBuff[i];
    }
    fRet = TRUE;
Ret:
    if (pbBuff)
        _nt_free(pbBuff, cbKeyOut);
    if (pbLabelAndSeed)
        _nt_free(pbLabelAndSeed, cbLabelAndSeed);
    return fRet;
}

void FreeSChHash(
                 PSCH_HASH       pSChHash
                 )
{
    if (pSChHash->pbCertData)
        _nt_free(pSChHash->pbCertData, pSChHash->cbCertData);
}

void FreeSChKey(
                PSCH_KEY    pSChKey
                )
{
    if (pSChKey->pbCertData)
        _nt_free(pSChKey->pbCertData, pSChKey->cbCertData);
}

BOOL SCHSetKeyParam(
                    IN PNTAGUserList pTmpUser,
                    IN OUT PNTAGKeyList pKey,
                    IN DWORD dwParam,
                    IN PBYTE pbData
                    )
{
    PCRYPT_DATA_BLOB    pDataBlob = (PCRYPT_DATA_BLOB)pbData;
    PSCH_KEY            pSChKey;
    PSCHANNEL_ALG       pSChAlg;
    BOOL                fPubKey = FALSE;
    BOOL                fRet = FALSE;

    if ((CALG_SSL3_MASTER != pKey->Algid) &&
        (CALG_PCT1_MASTER != pKey->Algid) &&
        (CALG_TLS1_MASTER != pKey->Algid) &&
        (CALG_SSL2_MASTER != pKey->Algid))
    {
        SetLastError((DWORD) NTE_BAD_TYPE);
        goto Ret;
    }

    if (NULL == pKey->pData)
    {
        if (NULL == (pKey->pData = (BYTE*)_nt_malloc(sizeof(SCH_KEY))))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
        memset(pKey->pData, 0, sizeof(SCH_KEY));
    }

    pSChKey = (PSCH_KEY)pKey->pData;

    if (KP_SCHANNEL_ALG == dwParam)
    {
        pSChAlg = (PSCHANNEL_ALG)pbData;
        pSChKey->dwFlags = pSChAlg->dwFlags;   // set the international version indicator

        switch(pSChAlg->dwUse)
        {
            case SCHANNEL_MAC_KEY:
                switch(pSChAlg->Algid)
                {
                    case CALG_MD5:
                        if (CALG_PCT1_MASTER == pKey->Algid)
                        {
                            pSChKey->cbHash = MD5DIGESTLEN;
                            pSChKey->cbEncMac = pSChAlg->cBits / 8;
                        }
                        else
                        {
                            if (pSChAlg->cBits != (MD5DIGESTLEN * 8))
                            {
                                SetLastError((DWORD)NTE_BAD_DATA);
                                goto Ret;
                            }
                            pSChKey->cbEncMac = MD5DIGESTLEN;
                        }
                        break;

                    case CALG_SHA1:
                        if (CALG_PCT1_MASTER == pKey->Algid)
                        {
                            pSChKey->cbHash = A_SHA_DIGEST_LEN;
                            pSChKey->cbEncMac = pSChAlg->cBits / 8;
                        }
                        else
                        {
                            if (pSChAlg->cBits != (A_SHA_DIGEST_LEN * 8))
                            {
                                SetLastError((DWORD)NTE_BAD_DATA);
                                goto Ret;
                            }
                            pSChKey->cbEncMac = A_SHA_DIGEST_LEN;
                        }
                        break;

                    default:
                        SetLastError((DWORD)NTE_BAD_DATA);
                        goto Ret;
                }
                pSChKey->HashAlgid = pSChAlg->Algid;
                break;

            case SCHANNEL_ENC_KEY:
                if (pSChAlg->cBits % 8)
                {
                    SetLastError((DWORD)NTE_BAD_DATA);
                    goto Ret;
                }

                if ((PROV_RSA_SCHANNEL == pTmpUser->dwProvType) &&
                    (0 != pTmpUser->dwSGCFlags))
                {
                    if (!FIsLegalSGCKeySize(pSChAlg->Algid, pSChAlg->cBits / 8,
                                            FALSE, FALSE, &fPubKey))
                    {
                        goto Ret;
                    }
                }
                else
                {
                    if (!FIsLegalKeySize(pSChAlg->Algid, pSChAlg->cBits / 8,
                                         FALSE, 0, &fPubKey))
                    {
                        goto Ret;
                    }
                }

                switch(pSChAlg->Algid)
                {
#ifdef CSP_USE_RC4
                    case CALG_RC4:
                        pSChKey->cbIV = 0;
                        break;
#endif

#ifdef CSP_USE_RC2
                    case CALG_RC2:
                        pSChKey->cbIV = RC2_BLOCKLEN;
                        break;
#endif

#ifdef CSP_USE_DES
                    case CALG_DES:
                        pSChKey->cbIV = DES_BLOCKLEN;
                        break;
#endif
                        
#ifdef CSP_USE_3DES
                    case CALG_3DES_112:
                        pSChKey->cbIV = DES_BLOCKLEN;
                        break;

                    case CALG_3DES:
                        pSChKey->cbIV = DES_BLOCKLEN;
                        break;
#endif

                    default:
                        SetLastError((DWORD)NTE_BAD_DATA);
                        goto Ret;
                }

                // For SSL2 check that the length of the master key matches the
                // the requested encryption length
                if ((CALG_SSL2_MASTER == pKey->Algid) &&
                    ((pSChAlg->cBits / 8) != pKey->cbKeyLen))
                {
                    SetLastError((DWORD)NTE_BAD_KEY);
                    goto Ret;
                }

                pSChKey->cbEnc = (pSChAlg->cBits / 8);
                pSChKey->EncAlgid = pSChAlg->Algid;
                break;

            default:
                SetLastError((DWORD)NTE_BAD_DATA);
                goto Ret;
        }
    }
    else
    {
        switch (dwParam)
        {
            case KP_CLIENT_RANDOM:
                if (pDataBlob->cbData > MAX_RANDOM_LEN)
                {
                    SetLastError((DWORD)NTE_BAD_DATA);
                    goto Ret;
                }

                pSChKey->cbClientRandom = pDataBlob->cbData;
                memcpy(pSChKey->rgbClientRandom, pDataBlob->pbData, pDataBlob->cbData);
                break;

            case KP_SERVER_RANDOM:
                if (pDataBlob->cbData > MAX_RANDOM_LEN)
                {
                    SetLastError((DWORD)NTE_BAD_DATA);
                    goto Ret;
                }

                pSChKey->cbServerRandom = pDataBlob->cbData;
                memcpy(pSChKey->rgbServerRandom, pDataBlob->pbData, pDataBlob->cbData);
                break;

            case KP_CERTIFICATE:
                if (CALG_PCT1_MASTER != pKey->Algid)
                {
                    SetLastError((DWORD) NTE_BAD_TYPE);
                    goto Ret;
                }

                if(pSChKey->pbCertData)
                {
                    _nt_free(pSChKey->pbCertData, pSChKey->cbCertData);
                }
                pSChKey->cbCertData = pDataBlob->cbData;
                if (NULL == (pSChKey->pbCertData = (BYTE*)_nt_malloc(pSChKey->cbCertData)))
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    goto Ret;
                }
                memcpy(pSChKey->pbCertData, pDataBlob->pbData, pDataBlob->cbData);
                break;

            case KP_CLEAR_KEY:
                if (pDataBlob->cbData > MAX_RANDOM_LEN)
                {
                    SetLastError((DWORD)NTE_BAD_DATA);
                    goto Ret;
                }

                if ((CALG_PCT1_MASTER != pKey->Algid) &&
                    (CALG_SSL2_MASTER != pKey->Algid))
                {
                    SetLastError((DWORD) NTE_BAD_TYPE);
                    goto Ret;
                }

                pSChKey->cbClearData = pDataBlob->cbData;
                memcpy(pSChKey->rgbClearData, pDataBlob->pbData, pDataBlob->cbData);
                break;

            default:
                SetLastError((DWORD) NTE_BAD_TYPE);
                goto Ret;
        }
    }

    fRet = TRUE;
Ret:
    return fRet;
}

BOOL SCHGetKeyParam(
                    PNTAGKeyList pKey,
                    DWORD dwParam,
                    PBYTE pbData
                    )
{
    return TRUE;
}

BOOL SSL3SingleHash(
                    HCRYPTPROV hUID,
                    PBYTE pbString,
                    DWORD cbString,
                    PBYTE pbSecret,
                    DWORD cbSecret,
                    PBYTE pbRand1,
                    DWORD cbRand1,
                    PBYTE pbRand2,
                    DWORD cbRand2,
                    PBYTE pbResult
                    )
{
    HCRYPTHASH hHashSHA = 0;
    HCRYPTHASH hHashMD5 = 0;
    BYTE rgb[A_SHA_DIGEST_LEN];
    DWORD cb;
    BOOL fRet = FALSE;

    // perform the SHA hashing
    if (!CPCreateHash(hUID, CALG_SHA1, 0, 0, &hHashSHA))
        goto Ret;

    if (!CPHashData(hUID, hHashSHA, pbString, cbString, 0))
        goto Ret;

    if (!CPHashData(hUID, hHashSHA, pbSecret, cbSecret, 0))
        goto Ret;

    if (!CPHashData(hUID, hHashSHA, pbRand1, cbRand1, 0))
        goto Ret;
    if (!CPHashData(hUID, hHashSHA, pbRand2, cbRand2, 0))
        goto Ret;

    cb = A_SHA_DIGEST_LEN;
    if (!CPGetHashParam(hUID, hHashSHA, HP_HASHVAL, rgb, &cb, 0))
        goto Ret;

    // perform the MD5 hashing
    if (!CPCreateHash(hUID, CALG_MD5, 0, 0, &hHashMD5))
        goto Ret;

    if (!CPHashData(hUID, hHashMD5, pbSecret, cbSecret, 0))
        goto Ret;

    if (!CPHashData(hUID, hHashMD5, rgb, A_SHA_DIGEST_LEN, 0))
        goto Ret;

    cb = MD5DIGESTLEN;
    if (!CPGetHashParam(hUID, hHashMD5, HP_HASHVAL, pbResult, &cb, 0))
        goto Ret;

    fRet = TRUE;
Ret:
    if (hHashSHA)
        CPDestroyHash(hUID, hHashSHA);
    if (hHashMD5)
        CPDestroyHash(hUID, hHashMD5);
    return fRet;
}

BOOL SSL3HashPreMaster(
                       HCRYPTPROV hUID,
                       PBYTE pbSecret,
                       DWORD cbSecret,
                       PBYTE pbRand1,
                       DWORD cbRand1,
                       PBYTE pbRand2,
                       DWORD cbRand2,
                       PBYTE pbFinal,
                       DWORD cbFinal
                       )
{
    BYTE    rgbString[17];   // know max length from MAX_RANDOM_LEN
    DWORD   cLimit;
    DWORD   cbIndex = 0;
    long    i;
    BOOL    fRet = FALSE;

    if ((cbFinal > MAX_RANDOM_LEN) || ((cbFinal % MD5DIGESTLEN) != 0))
    {
        SetLastError((DWORD)NTE_FAIL);
        goto Ret;
    }

    cLimit = cbFinal / MD5DIGESTLEN;

    for (i=0;i<(long)cLimit;i++)
    {
        memset(rgbString, 0x41 + i, i + 1);
        if (!SSL3SingleHash(hUID, rgbString, i + 1, pbSecret,
                            cbSecret, pbRand1, cbRand1,
                            pbRand2, cbRand2, pbFinal + cbIndex))
            goto Ret;
        cbIndex += MD5DIGESTLEN;
    }
    fRet = TRUE;
Ret:
    return fRet;
}

BOOL SChGenMasterKey(
                     PNTAGKeyList pKey,
                     PSCH_HASH pSChHash
                     )
{
    PSCH_KEY    pSChKey;
    DWORD       cb;
    BYTE        *pbClientAndServer = NULL;
    DWORD       cbClientAndServer;
    BOOL        fRet = FALSE;

    pSChKey = (PSCH_KEY)pKey->pData;
    pSChHash->dwFlags = pSChKey->dwFlags;   // set the international flag from the key

    switch(pKey->Algid)
    {
        case CALG_SSL3_MASTER:
            if (!pSChKey->fFinished)
            {
                // copy the premaster secret
                pSChKey->cbPremaster = pKey->cbKeyLen;
                memcpy(pSChKey->rgbPremaster, pKey->pKeyValue, pSChKey->cbPremaster);

                // hash the pre-master secret
                if (!SSL3HashPreMaster(pKey->hUID,
                                       pSChKey->rgbPremaster,
                                       pSChKey->cbPremaster,
                                       pSChKey->rgbClientRandom,
                                       pSChKey->cbClientRandom,
                                       pSChKey->rgbServerRandom,
                                       pSChKey->cbServerRandom,
                                       pKey->pKeyValue,
                                       pKey->cbKeyLen))
                    goto Ret;
            }

            // copy the necessary information to the hash
            pSChHash->EncAlgid = pSChKey->EncAlgid;
            pSChHash->cbEnc = pSChKey->cbEnc;
            pSChHash->cbEncMac = pSChKey->cbEncMac;
            pSChHash->cbIV = pSChKey->cbIV;
            pSChHash->cbClientRandom = pSChKey->cbClientRandom;
            memcpy(pSChHash->rgbClientRandom, pSChKey->rgbClientRandom, pSChHash->cbClientRandom);
            pSChHash->cbServerRandom = pSChKey->cbServerRandom;
            memcpy(pSChHash->rgbServerRandom, pSChKey->rgbServerRandom, pSChHash->cbServerRandom);

            cb = pSChHash->cbEnc * 2 + pSChHash->cbEncMac * 2 + pSChHash->cbIV * 2;
            pSChHash->cbFinal = (cb / MD5DIGESTLEN) * MD5DIGESTLEN;
            if (cb % MD5DIGESTLEN)
            {
                pSChHash->cbFinal += MD5DIGESTLEN;
            }

            // hash the master secret
            if (!SSL3HashPreMaster(pKey->hUID,
                                   pKey->pKeyValue,
                                   pKey->cbKeyLen,
                                   pSChKey->rgbServerRandom,
                                   pSChKey->cbServerRandom,
                                   pSChKey->rgbClientRandom,
                                   pSChKey->cbClientRandom,
                                   pSChHash->rgbFinal,
                                   pSChHash->cbFinal))
                goto Ret;

            pSChKey->fFinished = TRUE;
            break;

        case CALG_TLS1_MASTER:
            cbClientAndServer = pSChKey->cbClientRandom + pSChKey->cbServerRandom;
            if (NULL == (pbClientAndServer =
                    (BYTE*)_nt_malloc(cbClientAndServer)))
            {
                goto Ret;
            }

            if (!pSChKey->fFinished)
            {
                // copy the premaster secret
                pSChKey->cbPremaster = pKey->cbKeyLen;
                memcpy(pSChKey->rgbPremaster, pKey->pKeyValue, pSChKey->cbPremaster);

                // concatenate the client random and server random
                memcpy(pbClientAndServer, pSChKey->rgbClientRandom,
                       pSChKey->cbClientRandom);
                memcpy(pbClientAndServer + pSChKey->cbClientRandom,
                       pSChKey->rgbServerRandom, pSChKey->cbServerRandom);

                // hash the pre-master secret
                if (!PRF(pSChKey->rgbPremaster, pSChKey->cbPremaster,
                         "master secret", 13,
                         pbClientAndServer, cbClientAndServer,
                         pKey->pKeyValue, TLS_MASTER_LEN))
                {
                    goto Ret;
                }
            }

            // copy the necessary information to the hash
            pSChHash->EncAlgid = pSChKey->EncAlgid;
            pSChHash->cbEnc = pSChKey->cbEnc;
            pSChHash->cbEncMac = pSChKey->cbEncMac;
            pSChHash->cbIV = pSChKey->cbIV;
            pSChHash->cbClientRandom = pSChKey->cbClientRandom;
            memcpy(pSChHash->rgbClientRandom, pSChKey->rgbClientRandom,
                   pSChHash->cbClientRandom);
            pSChHash->cbServerRandom = pSChKey->cbServerRandom;
            memcpy(pSChHash->rgbServerRandom, pSChKey->rgbServerRandom,
                   pSChHash->cbServerRandom);

            pSChHash->cbFinal = pSChHash->cbEnc * 2 + pSChHash->cbEncMac * 2 +
                                pSChHash->cbIV * 2;

            // concatenate the server random and client random 
            memcpy(pbClientAndServer, pSChKey->rgbServerRandom,
                   pSChKey->cbServerRandom);
            memcpy(pbClientAndServer + pSChKey->cbServerRandom,
                   pSChKey->rgbClientRandom, pSChKey->cbClientRandom);

            // hash the master secret
            if (!PRF(pKey->pKeyValue, pKey->cbKeyLen,
                     "key expansion", 13,
                     pbClientAndServer, cbClientAndServer,
                     pSChHash->rgbFinal, pSChHash->cbFinal))
                goto Ret;

            pSChKey->fFinished = TRUE;
            break;

        case CALG_PCT1_MASTER:
            pSChHash->cbFinal = pKey->cbKeyLen;
            memcpy(pSChHash->rgbFinal, pKey->pKeyValue, pSChHash->cbFinal);

            // copy the necessary information to the hash
            pSChHash->EncAlgid = pSChKey->EncAlgid;
            pSChHash->HashAlgid = pSChKey->HashAlgid;
            pSChHash->cbEnc = pSChKey->cbEnc;
            pSChHash->cbEncMac = pSChKey->cbEncMac;
            pSChHash->cbHash = pSChKey->cbHash;
            pSChHash->cbIV = pSChKey->cbIV;
            pSChHash->cbClientRandom = pSChKey->cbClientRandom;
            memcpy(pSChHash->rgbClientRandom, pSChKey->rgbClientRandom, pSChHash->cbClientRandom);
            pSChHash->cbServerRandom = pSChKey->cbServerRandom;
            memcpy(pSChHash->rgbServerRandom, pSChKey->rgbServerRandom, pSChHash->cbServerRandom);

            pSChHash->cbCertData = pSChKey->cbCertData;
            if (NULL == (pSChHash->pbCertData = (BYTE*)_nt_malloc(pSChHash->cbCertData)))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }
            memcpy(pSChHash->pbCertData, pSChKey->pbCertData, pSChHash->cbCertData);
            pSChHash->cbClearData = pSChKey->cbClearData;
            memcpy(pSChHash->rgbClearData, pSChKey->rgbClearData, pSChHash->cbClearData);
            break;

        case CALG_SSL2_MASTER:
            pSChHash->cbFinal = pKey->cbKeyLen;
            memcpy(pSChHash->rgbFinal, pKey->pKeyValue, pSChHash->cbFinal);

            // copy the necessary information to the hash
            pSChHash->EncAlgid = pSChKey->EncAlgid;
            pSChHash->HashAlgid = pSChKey->HashAlgid;
            pSChHash->cbEnc = pSChKey->cbEnc;
            pSChHash->cbEncMac = pSChKey->cbEncMac;
            pSChHash->cbHash = pSChKey->cbHash;
            pSChHash->cbIV = pSChKey->cbIV;
            pSChHash->cbClientRandom = pSChKey->cbClientRandom;
            memcpy(pSChHash->rgbClientRandom, pSChKey->rgbClientRandom, pSChHash->cbClientRandom);
            pSChHash->cbServerRandom = pSChKey->cbServerRandom;
            memcpy(pSChHash->rgbServerRandom, pSChKey->rgbServerRandom, pSChHash->cbServerRandom);
            pSChHash->cbClearData = pSChKey->cbClearData;
            memcpy(pSChHash->rgbClearData, pSChKey->rgbClearData, pSChHash->cbClearData);
            break;
    }

    fRet = TRUE;
Ret:
    if (pbClientAndServer)
        _nt_free(pbClientAndServer, cbClientAndServer);
    return fRet;
}

BOOL HelperHash(
                HCRYPTPROV hProv,
                BYTE *pb,
                DWORD cb,
                ALG_ID Algid,
                BYTE **ppbHash,
                DWORD *pcbHash,
                BOOL fAlloc
                )
{
    HCRYPTHASH  hHash = 0;
    BOOL        fRet = FALSE;

    if (fAlloc)
    {
        *ppbHash = NULL;
    }

    // hash the key and stuff into a usable key
    if (!CPCreateHash(hProv, Algid, 0, 0, &hHash))
        goto Ret;

    if (!CPHashData(hProv, hHash, pb, cb, 0))
        goto Ret;

    if (fAlloc)
    {
        if (!CPGetHashParam(hProv, hHash, HP_HASHVAL, NULL, pcbHash, 0))
            goto Ret;

        if (NULL == (*ppbHash = (BYTE*)_nt_malloc(*pcbHash)))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
    }

    if (!CPGetHashParam(hProv, hHash, HP_HASHVAL, *ppbHash, pcbHash, 0))
        goto Ret;

    fRet = TRUE;
Ret:
    if (hHash)
        CPDestroyHash(hProv, hHash);
    if ((FALSE == fRet) && fAlloc && *ppbHash)
    {
        _nt_free(*ppbHash, *pcbHash);
        *ppbHash = NULL;
    }
    return fRet;
}

BOOL SSL3DeriveWriteKey(
                        PNTAGUserList pTmpUser,
                        PNTAGHashList pHash,
                        DWORD dwFlags,
                        HCRYPTKEY *phKey
                        )
{
    PSCH_HASH       pSChHash;
    DWORD           cbOffset;
    BYTE            rgbTmp[MD5DIGESTLEN];
    DWORD           cbTmp;
    BYTE            *pbIV = NULL;
    DWORD           cbIV;
    BOOL            fUseIV = FALSE;
    BYTE            *pbKey = NULL;
    DWORD           cbKey;
    PNTAGKeyList    pTmpKey = NULL;
    BYTE            rgbBuff[MAX_RANDOM_LEN * 2 + MAX_PREMASTER_LEN];
    DWORD           cbBuff;
    BOOL            fRet = FALSE;
    DWORD           dwRights;

    pSChHash = (PSCH_HASH)pHash->pHashData;
    cbOffset = 2 * pSChHash->cbEncMac;

    // get the IV
    if (CALG_RC4 != pSChHash->EncAlgid)
        fUseIV = TRUE;

    // if not flagged as a server key then default is client
    if (pSChHash->dwFlags & INTERNATIONAL_USAGE)
    {
        if (CRYPT_SERVER & dwFlags)
        {
            cbBuff = pSChHash->cbEnc + pSChHash->cbServerRandom + pSChHash->cbClientRandom;
            if (cbBuff > sizeof(rgbBuff))
                goto Ret;
            memcpy(rgbBuff, pSChHash->rgbFinal + cbOffset + pSChHash->cbEnc, pSChHash->cbEnc);
            memcpy(rgbBuff + pSChHash->cbEnc, pSChHash->rgbServerRandom, pSChHash->cbServerRandom);
            memcpy(rgbBuff + pSChHash->cbEnc + pSChHash->cbServerRandom,
                   pSChHash->rgbClientRandom, pSChHash->cbClientRandom);
            if (!HelperHash(pHash->hUID, rgbBuff, cbBuff, CALG_MD5,
                            &pbKey, &cbKey, TRUE))
                goto Ret;

            if (fUseIV)
            {
                cbBuff = pSChHash->cbServerRandom + pSChHash->cbClientRandom;
                memcpy(rgbBuff, pSChHash->rgbServerRandom, pSChHash->cbServerRandom);
                memcpy(rgbBuff + pSChHash->cbServerRandom,
                       pSChHash->rgbClientRandom, pSChHash->cbClientRandom);
                if (!HelperHash(pHash->hUID, rgbBuff, cbBuff, CALG_MD5,
                                &pbIV, &cbIV, TRUE))
                    goto Ret;
            }
        }
        else
        {
            cbBuff = pSChHash->cbEnc + pSChHash->cbServerRandom + pSChHash->cbClientRandom;
            if (cbBuff > sizeof(rgbBuff))
                goto Ret;
            memcpy(rgbBuff, pSChHash->rgbFinal + cbOffset, pSChHash->cbEnc);
            memcpy(rgbBuff + pSChHash->cbEnc, pSChHash->rgbClientRandom, pSChHash->cbClientRandom);
            memcpy(rgbBuff + pSChHash->cbEnc + pSChHash->cbClientRandom,
                   pSChHash->rgbServerRandom, pSChHash->cbServerRandom);
            if (!HelperHash(pHash->hUID, rgbBuff, cbBuff, CALG_MD5,
                            &pbKey, &cbKey, TRUE))
                goto Ret;

            if (fUseIV)
            {
                cbBuff = pSChHash->cbServerRandom + pSChHash->cbClientRandom;
                memcpy(rgbBuff, pSChHash->rgbClientRandom, pSChHash->cbClientRandom);
                memcpy(rgbBuff + pSChHash->cbClientRandom,
                       pSChHash->rgbServerRandom, pSChHash->cbServerRandom);
                if (!HelperHash(pHash->hUID, rgbBuff, cbBuff, CALG_MD5,
                                &pbIV, &cbIV, TRUE))
                    goto Ret;
            }
        }
    }
    else
    {
        cbKey = pSChHash->cbEnc;
        if (NULL == (pbKey = (BYTE*)_nt_malloc(cbKey)))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }

        cbIV = pSChHash->cbIV;
        if (NULL == (pbIV = (BYTE*)_nt_malloc(cbIV)))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }

        if (CRYPT_SERVER & dwFlags)
        {
            memcpy(pbKey, pSChHash->rgbFinal + cbOffset + pSChHash->cbEnc,
                   pSChHash->cbEnc);

            memcpy(pbIV, pSChHash->rgbFinal + cbOffset + pSChHash->cbEnc * 2 +
                   pSChHash->cbIV, pSChHash->cbIV);
        }
        else
        {
            memcpy(pbKey, pSChHash->rgbFinal + cbOffset, pSChHash->cbEnc);

            memcpy(pbIV, pSChHash->rgbFinal + cbOffset + pSChHash->cbEnc * 2,
                   pSChHash->cbIV);
        }
    }

    // check if the key is CRYPT_EXPORTABLE
    if (dwFlags & CRYPT_EXPORTABLE)
        dwRights = CRYPT_EXPORTABLE;

    // make the new key
    if (NULL == (pTmpKey = MakeNewKey(pSChHash->EncAlgid,
                                      dwRights,
                                      pSChHash->cbEnc,
                                      pHash->hUID,
                                      pbKey,
                                      FALSE)))
    {
        goto Ret;
    }

    if (CALG_RC2 == pSChHash->EncAlgid)
    {
        pTmpKey->EffectiveKeyLen = RC2_SCHANNEL_DEFAULT_EFFECTIVE_KEYLEN;
    }

    if ((pSChHash->dwFlags & INTERNATIONAL_USAGE) &&
        ((CALG_RC2 == pSChHash->EncAlgid) || (CALG_RC4 == pSChHash->EncAlgid)))
    {
        pTmpKey->cbSaltLen = RC_KEYLEN - pSChHash->cbEnc;
        memcpy(pTmpKey->rgbSalt, pbKey + pSChHash->cbEnc, pTmpKey->cbSaltLen);
    }

    // check keylength...
    if (!FIsLegalKey(pTmpUser, pTmpKey, FALSE))
        goto Ret;

    // set the IV if necessary
    if (fUseIV)
    {
        // set the mode to CBC
        pTmpKey->Mode = CRYPT_MODE_CBC;

        // set the IV
        memcpy(pTmpKey->IV, pbIV, CRYPT_BLKLEN);       // Initialization vector
    }

    if (NTF_FAILED == NTLMakeItem(phKey, KEY_HANDLE, (void *)pTmpKey))
        goto Ret;

    fRet = TRUE;
Ret:
    if (pbKey)
        _nt_free(pbKey, cbKey);
    if (pbIV)
        _nt_free(pbIV, cbIV);
    if (FALSE == fRet)
    {
        if (pTmpKey)
            FreeNewKey(pTmpKey);
    }
    return fRet;
}

BOOL PCT1MakeKeyHash(
                        PNTAGHashList pHash,
                        DWORD c,
                        DWORD dwFlags,
                        BOOL fWriteKey,
                        BYTE *pbBuff,
                        DWORD *pcbBuff
                        )
{
    BYTE        *pb = NULL;
    DWORD       cb = 0;
    DWORD       cbIndex;
    PSCH_HASH   pSChHash;
    BYTE        *pbStr;
    DWORD       cbStr;
    DWORD       i;
    BYTE        *pbHash = NULL;
    DWORD       cbHash;
    DWORD       fRet = FALSE;

    pSChHash = (PSCH_HASH)pHash->pHashData;

    // For reasons of backward compatibility, use the formula:
    //     hash( i, "foo"^i, MASTER_KEY, ...
    // rather than:
    //     hash( i, "foo", MASTER_KEY, ...
    // when deriving encryption keys.

    if (fWriteKey)
    {
        if (CRYPT_SERVER & dwFlags)
        {
            pbStr = PCT1_S_WRT;
            cbStr = PCT1_S_WRT_LEN;
            cb = cbStr * c;
        }
        else
        {
            pbStr = PCT1_C_WRT;
            cbStr = PCT1_C_WRT_LEN;
            cb = pSChHash->cbCertData + cbStr * c * 2;
        }
    }
    else
    {
        if (CRYPT_SERVER & dwFlags)
        {
            pbStr = PCT1_S_MAC;
            cbStr = PCT1_S_MAC_LEN;
        }
        else
        {
            pbStr = PCT1_C_MAC;
            cbStr = PCT1_C_MAC_LEN;
            cb = pSChHash->cbCertData + cbStr * c;
        }
    }

    cb += 1 + (3 * cbStr * c) + pSChHash->cbFinal +
          + pSChHash->cbClientRandom  + pSChHash->cbServerRandom;

    if (NULL == (pb = (BYTE*)_nt_malloc(cb)))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    // form the buffer to be hashed
    pb[0] = (BYTE)c;
    cbIndex = 1;

    if (fWriteKey)
    {
        for(i=0;i<c;i++)
        {
            memcpy(pb + cbIndex, pbStr, cbStr);
            cbIndex += cbStr;
        }
    }

    memcpy(pb + cbIndex, pSChHash->rgbFinal, pSChHash->cbFinal);
    cbIndex += pSChHash->cbFinal;
    for(i=0;i<c;i++)
    {
        memcpy(pb + cbIndex, pbStr, cbStr);
        cbIndex += cbStr;
    }
    memcpy(pb + cbIndex, pSChHash->rgbServerRandom, pSChHash->cbServerRandom);
    cbIndex += pSChHash->cbServerRandom;
    for(i=0;i<c;i++)
    {
        memcpy(pb + cbIndex, pbStr, cbStr);
        cbIndex += cbStr;
    }

    if (!(CRYPT_SERVER & dwFlags))
    {
        memcpy(pb + cbIndex, pSChHash->pbCertData, pSChHash->cbCertData);
        cbIndex += pSChHash->cbCertData;
        for(i=0;i<c;i++)
        {
            memcpy(pb + cbIndex, pbStr, cbStr);
            cbIndex += cbStr;
        }
    }

    memcpy(pb + cbIndex, pSChHash->rgbClientRandom, pSChHash->cbClientRandom);
    cbIndex += pSChHash->cbClientRandom;
    for(i=0;i<c;i++)
    {
        memcpy(pb + cbIndex, pbStr, cbStr);
        cbIndex += cbStr;
    }

    if (!HelperHash(pHash->hUID, pb, cb, pSChHash->HashAlgid,
                    &pbHash, &cbHash, TRUE))
        goto Ret;

    *pcbBuff = cbHash;
    memcpy(pbBuff, pbHash, *pcbBuff);

    fRet = TRUE;
Ret:
    if (pb)
        _nt_free(pb, cb);
    if (pbHash)
        _nt_free(pbHash, cbHash);
    return fRet;
}

BOOL PCT1MakeExportableWriteKey(
                                PNTAGHashList pHash,
                                BYTE *pbBuff,
                                DWORD *pcbBuff
                                )
{
    BYTE        *pb = NULL;
    DWORD       cb;
    BYTE        *pbHash = NULL;
    DWORD       cbHash;
    PSCH_HASH   pSChHash;
    DWORD       fRet = FALSE;

    pSChHash = (PSCH_HASH)pHash->pHashData;

    // assumption is made that exportable keys are 16 bytes in length (RC4 & RC2)
    cb = 5 + *pcbBuff + pSChHash->cbClearData;

    if (NULL == (pb = (BYTE*)_nt_malloc(cb)))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    // form the buffer to be hashed
    pb[0] = 1;
    memcpy(pb + 1, "sl", 2);
    memcpy(pb + 3, pbBuff, *pcbBuff);
    memcpy(pb + 3 + *pcbBuff, "sl", 2);
    memcpy(pb + 5 + *pcbBuff, pSChHash->rgbClearData, pSChHash->cbClearData);

    if (!HelperHash(pHash->hUID, pb, cb, pSChHash->HashAlgid,
                    &pbHash, &cbHash, TRUE))
        goto Ret;

    *pcbBuff = cbHash;
    memcpy(pbBuff, pbHash, *pcbBuff);

    fRet = TRUE;
Ret:
    if (pb)
        _nt_free(pb, cb);
    if (pbHash)
        _nt_free(pbHash, cbHash);
    return fRet;
}


BOOL PCT1DeriveKey(
                   PNTAGUserList pTmpUser,
                   ALG_ID Algid,
                   PNTAGHashList pHash,
                   DWORD dwFlags,
                   HCRYPTKEY *phKey
                   )
{
    BYTE            rgbHashBuff[A_SHA_DIGEST_LEN * 2];  // SHA is largest hash and max is two concatenated
    DWORD           cbHashBuff = 0;
    BYTE            rgbTmpBuff[A_SHA_DIGEST_LEN * 2];  // SHA is largest hash and max is two concatenated
    DWORD           cbTmpBuff;
    DWORD           cb;
    DWORD           cbKey;
    PNTAGKeyList    pTmpKey = NULL;
    DWORD           i;
    DWORD           cHashes;
    ALG_ID          KeyAlgid;
    BOOL            fWriteKey = FALSE;
    PSCH_HASH       pSChHash;
    BYTE            rgbSalt[MAX_SALT_LEN];
    DWORD           cbSalt;
    BOOL            fRet = FALSE;
    DWORD           dwRights;

    memset(rgbSalt, 0, sizeof(rgbSalt));
    pSChHash = (PSCH_HASH)pHash->pHashData;

    switch(Algid)
    {
        case CALG_SCHANNEL_MAC_KEY:
            cbKey = pSChHash->cbEncMac;
            KeyAlgid = Algid;
            break;

        case CALG_SCHANNEL_ENC_KEY:
            fWriteKey = TRUE;
            cbKey = pSChHash->cbEnc;
            KeyAlgid = pSChHash->EncAlgid;
            break;

        default:
            SetLastError((DWORD)NTE_BAD_ALGID);
            goto Ret;
    }

    cHashes = (cbKey + (pSChHash->cbHash - 1)) / pSChHash->cbHash;
    if (cHashes > 2)
        goto Ret;

    for (i=0;i<cHashes;i++)
    {
        if (!PCT1MakeKeyHash(pHash, i + 1, dwFlags, fWriteKey,
                             rgbHashBuff + cbHashBuff, &cb))
            goto Ret;
        cbHashBuff += cb;
    }

    if ((CALG_SCHANNEL_ENC_KEY == Algid) &&
        (EXPORTABLE_KEYLEN == pSChHash->cbEnc))
    {
        cbHashBuff = cbKey;
        if (!PCT1MakeExportableWriteKey(pHash, rgbHashBuff, &cbHashBuff))
            goto Ret;

        cbSalt = EXPORTABLE_SALTLEN;
        memcpy(rgbSalt, rgbHashBuff + pSChHash->cbEnc, cbSalt);
    }

    // check if the key is CRYPT_EXPORTABLE
    if (dwFlags & CRYPT_EXPORTABLE)
        dwRights = CRYPT_EXPORTABLE;

    // make the new key
    if (NULL == (pTmpKey = MakeNewKey(KeyAlgid, dwRights, cbKey,
                                      pHash->hUID, rgbHashBuff,
                                      FALSE)))
        goto Ret;
    if (CALG_RC2 == KeyAlgid)
    {
        pTmpKey->EffectiveKeyLen = RC2_SCHANNEL_DEFAULT_EFFECTIVE_KEYLEN;
    }

    if ((CALG_SCHANNEL_ENC_KEY == Algid) &&
        (EXPORTABLE_KEYLEN == pSChHash->cbEnc))
    {
        pTmpKey->cbSaltLen = cbSalt;
        memcpy(pTmpKey->rgbSalt, rgbSalt, cbSalt);
    }

    // check keylength...
    if (!FIsLegalKey(pTmpUser, pTmpKey, FALSE))
        goto Ret;

    if (NTF_FAILED == NTLMakeItem(phKey, KEY_HANDLE, (void *)pTmpKey))
        goto Ret;

    fRet = TRUE;
Ret:
    if (FALSE == fRet)
    {
        if (pTmpKey)
        {
            FreeNewKey(pTmpKey);
        }
    }
    return fRet;
}

BOOL TLSDeriveExportableRCKey(
                              PSCH_HASH pSChHash,
                              BYTE *pbClientAndServer,
                              DWORD cbClientAndServer,
                              BYTE **ppbKey,
                              DWORD *pcbKey,
                              BYTE *pbSalt,
                              DWORD *pcbSalt,
                              DWORD dwFlags)
{
    BOOL    fRet = FALSE;

    // use key length 16 because this should only occur with RC2 and RC4
    // and those key lengths should be 16
    if ((CALG_RC2 == pSChHash->EncAlgid) || (CALG_RC4 == pSChHash->EncAlgid))
    {
        *pcbKey = RC_KEYLEN;
        *pcbSalt = RC_KEYLEN - pSChHash->cbEnc;
    }
    else
    {
        *pcbKey = pSChHash->cbEnc;
    }
    if (NULL == (*ppbKey = (BYTE*)_nt_malloc(*pcbKey)))
        goto Ret;

    // check if it is a server key or client key
    if (dwFlags & CRYPT_SERVER)
    {
        if (!PRF(pSChHash->rgbFinal + pSChHash->cbEncMac * 2 + pSChHash->cbEnc,
                 pSChHash->cbEnc, "server write key", 16,
                 pbClientAndServer, cbClientAndServer,
                 *ppbKey, *pcbKey))
        {
            goto Ret;
        }
    }
    else
    {
        if (!PRF(pSChHash->rgbFinal + pSChHash->cbEncMac * 2,
                 pSChHash->cbEnc, "client write key", 16,
                 pbClientAndServer, cbClientAndServer,
                 *ppbKey, *pcbKey))
        {
            goto Ret;
        }
    }

    if (0 != *pcbSalt)
    {
        memcpy(pbSalt, (*ppbKey) + pSChHash->cbEnc, *pcbSalt);
    }

    fRet = TRUE;
Ret:
    return fRet;
}

BOOL TLSDeriveExportableEncKey(
                               PSCH_HASH pSChHash,
                               BYTE **ppbKey,
                               DWORD *pcbKey,
                               BYTE **ppbRealKey,
                               BYTE *pbSalt,
                               DWORD *pcbSalt,
                               BYTE *pbIV,
                               DWORD dwFlags)
{
    BYTE    *pbClientAndServer = NULL;
    DWORD   cbClientAndServer;
    BYTE    *pbIVBlock = NULL;
    BOOL    fRet = FALSE;

    cbClientAndServer = pSChHash->cbClientRandom + pSChHash->cbServerRandom;
    if (NULL == (pbClientAndServer = (BYTE*)_nt_malloc(cbClientAndServer)))
        goto Ret;

    if (NULL == (pbIVBlock = (BYTE*)_nt_malloc(pSChHash->cbIV * 2)))
        goto Ret;

    // concatenate the server random and client random 
    memcpy(pbClientAndServer, pSChHash->rgbClientRandom,
           pSChHash->cbClientRandom);
    memcpy(pbClientAndServer + pSChHash->cbClientRandom,
           pSChHash->rgbServerRandom, pSChHash->cbServerRandom);

    // calculate the IV block
    if (pSChHash->cbIV)
    {
        if (!PRF(NULL, 0, "IV block", 8, pbClientAndServer,
                 cbClientAndServer, pbIVBlock, pSChHash->cbIV * 2))
        {
            goto Ret;
        }
        if (dwFlags & CRYPT_SERVER)
            memcpy(pbIV, pbIVBlock + pSChHash->cbIV, pSChHash->cbIV);
        else
            memcpy(pbIV, pbIVBlock, pSChHash->cbIV);
    }

    // check if it is a server key or client key
    if (!TLSDeriveExportableRCKey(pSChHash,
                                  pbClientAndServer,
                                  cbClientAndServer,
                                  ppbKey,
                                  pcbKey,
                                  pbSalt,
                                  pcbSalt,
                                  dwFlags))
    {
        goto Ret;
    }
    *ppbRealKey = *ppbKey;

    fRet = TRUE;
Ret:
    if (pbIVBlock)
        _nt_free(pbIVBlock, pSChHash->cbIV * 2);
    if (pbClientAndServer)
        _nt_free(pbClientAndServer, cbClientAndServer);
    return fRet;
}

BOOL TLSDeriveKey(
                  PNTAGUserList pTmpUser,
                  ALG_ID Algid,
                  PNTAGHashList pHash,
                  DWORD dwFlags,
                  HCRYPTKEY *phKey
                  )
{
    PSCH_HASH       pSChHash;
    PNTAGKeyList    pTmpKey = NULL;
    BYTE            *pbKey;
    DWORD           cbKey;
    BYTE            rgbSalt[MAX_SALT_LEN];
    DWORD           cbSalt = 0;
    BYTE            rgbIV[CRYPT_BLKLEN];
    DWORD           cbIVIndex;
    ALG_ID          KeyAlgid;
    BYTE            *pbAllocKey = NULL;
    DWORD           cbAllocKey;
    DWORD           dwRights = 0;
    BOOL            fRet = FALSE;

    memset(rgbIV, 0, sizeof(rgbIV));
    memset(rgbSalt, 0, sizeof(rgbSalt));
    pSChHash = (PSCH_HASH)pHash->pHashData;

    switch(Algid)
    {
        case CALG_SCHANNEL_MAC_KEY:
            cbKey = pSChHash->cbEncMac;
            KeyAlgid = Algid;

            // check if it is a server key or client key
            if (dwFlags & CRYPT_SERVER)
            {
                pbKey = pSChHash->rgbFinal + pSChHash->cbEncMac;
            }
            else
            {
                pbKey = pSChHash->rgbFinal;
            }
            break;

        case CALG_SCHANNEL_ENC_KEY:
            cbKey = pSChHash->cbEnc;
            KeyAlgid = pSChHash->EncAlgid;

            // if in exportable situation then call the exportable routine
            if (pSChHash->dwFlags & INTERNATIONAL_USAGE)
            {
                if (!TLSDeriveExportableEncKey(pSChHash, &pbAllocKey,
                                               &cbAllocKey, &pbKey, rgbSalt,
                                               &cbSalt, rgbIV, dwFlags))
                {
                    goto Ret;
                }
            }
            else
            {
                if (dwFlags & CRYPT_SERVER)
                {
                    pbKey = pSChHash->rgbFinal + pSChHash->cbEncMac * 2 +
                            pSChHash->cbEnc;

                    if (pSChHash->cbIV)
                    {
                        cbIVIndex = pSChHash->cbEncMac * 2 + 
                                    pSChHash->cbEnc * 2 + pSChHash->cbIV;
                    }
                }
                else
                {
                    pbKey = pSChHash->rgbFinal + pSChHash->cbEncMac * 2;

                    if (pSChHash->cbIV)
                    {
                        cbIVIndex = pSChHash->cbEncMac * 2 +
                                    pSChHash->cbEnc * 2;
                    }
                }
                memcpy(rgbIV, pSChHash->rgbFinal + cbIVIndex,
                       pSChHash->cbIV);
            }
            break;

        default:
            SetLastError((DWORD)NTE_BAD_ALGID);
            goto Ret;
    }

    // check if the key is CRYPT_EXPORTABLE
    if (dwFlags & CRYPT_EXPORTABLE)
        dwRights = CRYPT_EXPORTABLE;

    // make the new key
    if (NULL == (pTmpKey = MakeNewKey(KeyAlgid, dwRights, cbKey,
                                      pHash->hUID, pbKey,
                                      FALSE)))
        goto Ret;
    if (CALG_RC2 == KeyAlgid)
    {
        pTmpKey->EffectiveKeyLen = RC2_SCHANNEL_DEFAULT_EFFECTIVE_KEYLEN;
    }

    // set up the salt
    memcpy(pTmpKey->rgbSalt, rgbSalt, cbSalt);
    pTmpKey->cbSaltLen = cbSalt;

    // copy IV if necessary
    if (pSChHash->cbIV)
    {
        memcpy(pTmpKey->IV, rgbIV, pSChHash->cbIV);
    }

    // check keylength...
    if (!FIsLegalKey(pTmpUser, pTmpKey, FALSE))
        goto Ret;

    if (NTF_FAILED == NTLMakeItem(phKey, KEY_HANDLE, (void *)pTmpKey))
        goto Ret;

    fRet = TRUE;
Ret:
    if (pbAllocKey)
        _nt_free(pbAllocKey, cbAllocKey);
    if (FALSE == fRet)
    {
        if (pTmpKey)
        {
            FreeNewKey(pTmpKey);
        }
    }
    return fRet;
}

BOOL SSL2DeriveKey(
                   PNTAGUserList pTmpUser,
                   ALG_ID Algid,
                   PNTAGHashList pHash,
                   DWORD dwFlags,
                   HCRYPTKEY *phKey
                   )
{
    PSCH_HASH       pSChHash;
    BYTE            rgbHash[2 * MD5DIGESTLEN];
    BYTE            *pbHash;
    DWORD           cbHash = 2 * MD5DIGESTLEN;
    BYTE            *pbTmp = NULL;
    DWORD           cbTmp;
    BYTE            *pbKey;
    DWORD           cbKey;
    BYTE            rgbSalt[MAX_SALT_LEN];
    DWORD           cbSalt = 0;
    DWORD           cbIndex;
    DWORD           cbChangeByte;
    PNTAGKeyList    pTmpKey = NULL;
    BOOL            fRet = FALSE;
    DWORD           dwRights = 0;

    memset(rgbSalt, 0, sizeof(rgbSalt));

    if (CALG_SCHANNEL_ENC_KEY != Algid)
    {
        SetLastError((DWORD)NTE_BAD_ALGID);
        goto Ret;
    }

    pbHash = rgbHash;
    pSChHash = (PSCH_HASH)pHash->pHashData;

    // set up the buffer to be hashed 
    cbTmp = pSChHash->cbFinal + pSChHash->cbClearData +
            pSChHash->cbClientRandom + pSChHash->cbServerRandom + 1;

    if (NULL == (pbTmp = (BYTE*)_nt_malloc(cbTmp)))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }
    memcpy(pbTmp, pSChHash->rgbClearData,
           pSChHash->cbClearData);
    cbIndex = pSChHash->cbClearData;

    // exportability check
    memcpy(pbTmp + cbIndex, pSChHash->rgbFinal, pSChHash->cbFinal);
    cbIndex += pSChHash->cbFinal;

    cbChangeByte = cbIndex;
    cbIndex++;

    memcpy(pbTmp + cbIndex, pSChHash->rgbClientRandom,
           pSChHash->cbClientRandom);
    cbIndex += pSChHash->cbClientRandom;
    memcpy(pbTmp + cbIndex, pSChHash->rgbServerRandom,
           pSChHash->cbServerRandom);
    cbIndex += pSChHash->cbServerRandom;

    switch(pSChHash->EncAlgid)
    {
#ifdef CSP_USE_RC2
        case CALG_RC2:
#endif
#ifdef CSP_USE_RC4
        case CALG_RC4:
#endif
            if (CRYPT_SERVER & dwFlags)
            {
                pbTmp[cbChangeByte] = 0x30;
            }
            else
            {
                pbTmp[cbChangeByte] = 0x31;
            }
            
            // hash the data to get the key
            if (!HelperHash(pHash->hUID, pbTmp, cbTmp, CALG_MD5,
                            &pbHash, &cbHash, FALSE))
                goto Ret;

            pbKey = pbHash;

            // check for export
            if (pSChHash->cbClearData)
            {
                cbKey = 5;
                cbSalt = 11;
                memcpy(rgbSalt, pbKey + cbKey, cbSalt);
            }
            else
            {
                cbKey = 16;
            }
            break;

#ifdef CSP_USE_DES
        case CALG_DES:
            pbTmp[cbChangeByte] = 0x30;
            // hash the data to get the key
            if (!HelperHash(pHash->hUID, pbTmp, cbTmp,
                            CALG_MD5, &pbHash, &cbHash, FALSE))
                goto Ret;

            if (CRYPT_SERVER & dwFlags)
            {
                pbKey = pbHash;
            }
            else
            {
                pbKey = pbHash + DES_KEYSIZE;
            }
            cbKey = DES_KEYSIZE;
            break;
#endif

#ifdef CSP_USE_3DES
        case CALG_3DES:
            if (CRYPT_SERVER & dwFlags)
            {
                pbTmp[cbChangeByte] = 0x30;
                // hash the data to get the key
                if (!HelperHash(pHash->hUID, pbTmp, cbTmp,
                                CALG_MD5, &pbHash, &cbHash, FALSE))
                    goto Ret;

                pbTmp[cbChangeByte] = 0x31;
                pbHash = rgbHash + MD5DIGESTLEN;
                // hash the data to get the key
                if (!HelperHash(pHash->hUID, pbTmp, cbTmp, CALG_MD5,
                                &pbHash, &cbHash, FALSE))
                    goto Ret;
                pbKey = rgbHash;
            }
            else
            {
                pbTmp[cbChangeByte] = 0x31;
                // hash the data to get the key
                if (!HelperHash(pHash->hUID, pbTmp, cbTmp,
                                CALG_MD5, &pbHash, &cbHash, FALSE))
                    goto Ret;

                pbTmp[cbChangeByte] = 0x32;
                pbHash = rgbHash + MD5DIGESTLEN;
                // hash the data to get the key
                if (!HelperHash(pHash->hUID, pbTmp, cbTmp, CALG_MD5,
                                &pbHash, &cbHash, FALSE))
                    goto Ret;
                pbKey = rgbHash + DES_KEYSIZE;
            }
            cbKey = DES3_KEYSIZE;
            break;
#endif
    }

    // check if the key is CRYPT_EXPORTABLE
    if (dwFlags & CRYPT_EXPORTABLE)
        dwRights = CRYPT_EXPORTABLE;

    // make the new key
    if (NULL == (pTmpKey = MakeNewKey(pSChHash->EncAlgid,
                                      dwRights,
                                      cbKey,
                                      pHash->hUID,
                                      pbKey,
                                      FALSE)))
    {
        goto Ret;
    }

    if (CALG_RC2 == pSChHash->EncAlgid)
    {
        pTmpKey->EffectiveKeyLen = RC2_SCHANNEL_DEFAULT_EFFECTIVE_KEYLEN;
    }

    pTmpKey->cbSaltLen = cbSalt;
    memcpy(pTmpKey->rgbSalt, rgbSalt, cbSalt);

    // check keylength...
    if (!FIsLegalKey(pTmpUser, pTmpKey, FALSE))
        goto Ret;

    if (NTF_FAILED == NTLMakeItem(phKey, KEY_HANDLE, (void *)pTmpKey))
        goto Ret;

    fRet = TRUE;
Ret:
    if (pbTmp)
        _nt_free(pbTmp, cbTmp);
    if (FALSE == fRet)
    {
        if (pTmpKey)
        {
            FreeNewKey(pTmpKey);
        }
    }
    return fRet;
}

BOOL SecureChannelDeriveKey(
                            PNTAGUserList pTmpUser,
                            PNTAGHashList pHash,
                            ALG_ID Algid,
                            DWORD dwFlags,
                            HCRYPTKEY *phKey
                            )
{
    PSCH_HASH       pSChHash;
    BYTE            *pbKey = NULL;
    DWORD           cbKey;
    PNTAGKeyList    pTmpKey = NULL;
    BOOL            fRet = FALSE;
    DWORD           dwRights;

    pSChHash = (PSCH_HASH)pHash->pHashData;

    switch(pSChHash->ProtocolAlgid)
    {
        case CALG_SSL3_MASTER:
            switch(Algid)
            {
                case CALG_SCHANNEL_MAC_KEY:
                    cbKey = pSChHash->cbEncMac;
                    if (NULL == (pbKey = (BYTE*)_nt_malloc(cbKey)))
                    {
                        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                        goto Ret;
                    }

                    // if not flagged as a server key then default is client
                    if (CRYPT_SERVER & dwFlags)
                    {
                        memcpy(pbKey, pSChHash->rgbFinal + cbKey, cbKey);
                    }
                    else
                    {
                        memcpy(pbKey, pSChHash->rgbFinal, cbKey);
                    }

                    // check if the key is CRYPT_EXPORTABLE
                    if (dwFlags & CRYPT_EXPORTABLE)
                        dwRights = CRYPT_EXPORTABLE;

                    // make the new key
                    if (NULL == (pTmpKey = MakeNewKey(Algid,
                                                      dwRights,
                                                      cbKey,
                                                      pHash->hUID,
                                                      pbKey,
                                                      TRUE)))
                    {
                        goto Ret;
                    }
                    pbKey = NULL;

                    // check keylength...
                    if (!FIsLegalKey(pTmpUser, pTmpKey, FALSE))
                        goto Ret;

                    if (NTF_FAILED == NTLMakeItem(phKey, KEY_HANDLE, (void *)pTmpKey))
                        goto Ret;

                    break;

                case CALG_SCHANNEL_ENC_KEY:
                    // derive the write keys
                    if (!SSL3DeriveWriteKey(pTmpUser, pHash, dwFlags, phKey))
                        goto Ret;
                    break;

                default:
                    SetLastError((DWORD)NTE_BAD_ALGID);
                    goto Ret;
            }
            break;

        case CALG_PCT1_MASTER:
            // derive the PCT1 key
            if (!PCT1DeriveKey(pTmpUser, Algid, pHash, dwFlags, phKey))
                goto Ret;
            break;

        case CALG_TLS1_MASTER:
            // derive the PCT1 key
            if (!TLSDeriveKey(pTmpUser, Algid, pHash, dwFlags, phKey))
                goto Ret;
            break;

        case CALG_SSL2_MASTER:
            // derive the PCT1 key
            if (!SSL2DeriveKey(pTmpUser, Algid, pHash, dwFlags, phKey))
                goto Ret;
            break;
    }

    fRet = TRUE;
Ret:
    if (pbKey)
        _nt_free(pbKey, cbKey);
    if ((FALSE == fRet) && pTmpKey)
    {
        FreeNewKey(pTmpKey);
    }
    return fRet;
}

BOOL SetPRFHashParam(
                     PRF_HASH *pPRFHash,
                     DWORD dwParam,
                     BYTE *pbData
                     )
{
    CRYPT_DATA_BLOB *pBlob;
    BOOL            fRet = FALSE;

    pBlob = (CRYPT_DATA_BLOB*)pbData;

    if (HP_TLS1PRF_LABEL == dwParam)
    {
        if (pBlob->cbData > sizeof(pPRFHash->rgbLabel))
        {
            SetLastError((DWORD)NTE_BAD_DATA);
            goto Ret;
        }
        pPRFHash->cbLabel = pBlob->cbData;
        memcpy(pPRFHash->rgbLabel, pBlob->pbData, pBlob->cbData);
    }
    else
    {
        if (pBlob->cbData > sizeof(pPRFHash->rgbSeed))
        {
            SetLastError((DWORD)NTE_BAD_DATA);
            goto Ret;
        }
        pPRFHash->cbSeed = pBlob->cbData;
        memcpy(pPRFHash->rgbSeed, pBlob->pbData, pBlob->cbData);
    }

    fRet = TRUE;
Ret:
    return fRet;
}

BOOL CalculatePRF(
                  PRF_HASH *pPRFHash,
                  BYTE *pbData,
                  DWORD *pcbData
                  )
{
    BOOL    fRet = FALSE;

    if (NULL == pbData)
    {
        *pcbData = 0;
    }
    else
    {
        if ((0 == pPRFHash->cbSeed) || (0 == pPRFHash->cbLabel))
        {
            SetLastError((DWORD)NTE_BAD_HASH_STATE);
            goto Ret;
        }

        if (!PRF(pPRFHash->rgbMasterKey, sizeof(pPRFHash->rgbMasterKey),
                 pPRFHash->rgbLabel, pPRFHash->cbLabel,
                 pPRFHash->rgbSeed, pPRFHash->cbSeed,
                 pbData, *pcbData))
        {
            SetLastError((DWORD)NTE_FAIL);
            goto Ret;
        }
    }

    fRet = TRUE;
Ret:
    return fRet;
}

