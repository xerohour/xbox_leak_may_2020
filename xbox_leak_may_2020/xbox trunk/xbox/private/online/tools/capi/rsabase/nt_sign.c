/////////////////////////////////////////////////////////////////////////////
//  FILE          : nt_sign.c                                              //
//  DESCRIPTION   : Crypto CP interfaces:                                  //
//                  CPSignHash                                             //
//                  CPVerifySignature                                      //
//  AUTHOR        :                                                        //
//  HISTORY       :                                                        //
//      Jan 25 1995 larrys  Changed from Nametag                           //
//      Feb 23 1995 larrys  Changed NTag_SetLastError to SetLastError      //
//      Mar 23 1995 larrys  Added variable key length                      //
//      May 10 1995 larrys  added private api calls                        //
//      Aug 03 1995 larrys  Fix for bug 10                                 //
//      Aug 22 1995 larrys  Added descriptions to sign and verify hash     //
//      Aug 30 1995 larrys  Changed Algid to dwKeySpec                     //
//      Aug 30 1995 larrys  Removed RETURNASHVALUE from CryptGetHashValue  //
//      Aug 31 1995 larrys  Fixed CryptSignHash for pbSignature == NULL    //
//      Aug 31 1995 larrys  Fix for Bug 28                                 //
//      Sep 12 1995 Jeffspel/ramas  Merged STT onto SCP                    //
//      Sep 12 1995 Jeffspel/ramas  BUGS FIXED PKCS#1 Padding.             //
//      Sep 18 1995 larrys  Removed flag fro CryptSignHash                 //
//      Oct 13 1995 larrys  Changed GetHashValue to GetHashParam           //
//      Oct 23 1995 larrys  Added MD2                                      //
//      Oct 25 1995 larrys  Change length of sDescription string           //
//      Nov 10 1995 DBarlow Bug #61                                        //
//      Dec 11 1995 larrys  Added error return check                       //
//      May 15 1996 larrys  Changed NTE_NO_MEMORY to ERROR_NOT_ENOUGHT...  //
//      May 29 1996 larrys  Bug 101                                        //
//      Jun  6 1996 a-johnb Added support for SSL 3.0 signatures           //
//      May 23 1997 jeffspel Added provider type checking                  //
//                                                                         //
//  Copyright (C) 1993 Microsoft Corporation   All Rights Reserved         //
/////////////////////////////////////////////////////////////////////////////

//#include <wtypes.h>
#include "precomp.h"
#include "ntagum.h"
#include "nt_rsa.h"
#include "protstor.h"
#include "swnt_pk.h"

extern HINSTANCE hInstance;
extern CSP_STRINGS g_Strings;

//
// Reverse ASN.1 Encodings of possible hash identifiers.  The leading byte is the length
// of the byte string.
//

static const BYTE
#ifdef CSP_USE_MD2
    *md2Encodings[]
            //      1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18
        = {
            "\x12\x10\x04\x00\x05\x02\x02\x0d\xf7\x86\x48\x86\x2a\x08\x06\x0c\x30\x20\x30",
            "\x10\x10\x04\x02\x02\x0d\xf7\x86\x48\x86\x2a\x08\x06\x0a\x30\x1e\x30",
            "\x00" },
#endif
#ifdef CSP_USE_MD4
    *md4Encodings[]
        = {
            "\x12\x10\x04\x00\x05\x04\x02\x0d\xf7\x86\x48\x86\x2a\x08\x06\x0c\x30\x20\x30",
            "\x10\x10\x04\x04\x02\x0d\xf7\x86\x48\x86\x2a\x08\x06\x0a\x30\x1e\x30",
            "\x00" },
#endif
#ifdef CSP_USE_MD5
    *md5Encodings[]
        = {
            "\x12\x10\x04\x00\x05\x05\x02\x0d\xf7\x86\x48\x86\x2a\x08\x06\x0c\x30\x20\x30",
            "\x10\x10\x04\x05\x02\x0d\xf7\x86\x48\x86\x2a\x08\x06\x0a\x30\x1e\x30",
            "\x00" },
#endif
#ifdef CSP_USE_SHA
    *shaEncodings[]
        = {
            //      1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18
            "\x0f\x14\x04\x00\x05\x1a\x02\x03\x0e\x2b\x05\x06\x09\x30\x21\x30",
            "\x0d\x14\x04\x1a\x02\x03\x0e\x2b\x05\x06\x07\x30\x1f\x30",
            "\x00" },
#endif
    *endEncodings[]
        = { "\x00" };

/*
 -      ApplyPKCS1SigningFormat
 -
 *      Purpose:
 *                Format a buffer with PKCS 1 for signing
 *
 */
BOOL ApplyPKCS1SigningFormat(IN  BSAFE_PUB_KEY *pPubKey,
                             IN  ALG_ID HashAlgid,
                             IN  BYTE *pbHash,
                             IN  DWORD cbHash,
                             IN  DWORD dwFlags,
                             OUT BYTE *pbPKCS1Format)
{
    BYTE    *pbStart;
    BYTE    *pbEnd;
    BYTE    bTmp;
    DWORD   i;
    BOOL    fRet = FALSE;

    // insert the block type
    pbPKCS1Format[pPubKey->datalen - 1] = 0x01;

    // insert the type I padding
    memset(pbPKCS1Format, 0xff, pPubKey->datalen-1);

    // Reverse it
    for (i = 0; i < cbHash; i++)
    {
        pbPKCS1Format[i] = pbHash[cbHash - (i + 1)];
    }

    if (!(CRYPT_NOHASHOID && dwFlags))
    {
        switch (HashAlgid)
        {
#ifdef CSP_USE_MD2
            case CALG_MD2:
            {
                // PKCS delimit the hash value
                pbEnd = (LPBYTE)md2Encodings[0];
                pbStart = pbPKCS1Format + cbHash;
                bTmp = *pbEnd++;
                while (0 < bTmp--)
                    *pbStart++ = *pbEnd++;
                *pbStart++ = 0;
                break;
            }

#endif

#ifdef CSP_USE_MD4
            case CALG_MD4:
            {
                // PKCS delimit the hash value
                pbEnd = (LPBYTE)md4Encodings[0];
                pbStart = pbPKCS1Format + cbHash;
                bTmp = *pbEnd++;
                while (0 < bTmp--)
                    *pbStart++ = *pbEnd++;
                *pbStart++ = 0;
                break;
            }
#endif

#ifdef CSP_USE_MD5
            case CALG_MD5:
            {
                // PKCS delimit the hash value
                pbEnd = (LPBYTE)md5Encodings[0];
                pbStart = pbPKCS1Format + cbHash;
                bTmp = *pbEnd++;
                while (0 < bTmp--)
                    *pbStart++ = *pbEnd++;
                *pbStart++ = 0;
                break;
            }

#endif

#ifdef CSP_USE_SHA
            case CALG_SHA:
            {
                // PKCS delimit the hash value
                pbEnd = (LPBYTE)shaEncodings[0];
                pbStart = pbPKCS1Format + cbHash;
                bTmp = *pbEnd++;
                while (0 < bTmp--)
                    *pbStart++ = *pbEnd++;
                *pbStart++ = 0;
                break;
            }
#endif

#ifdef CSP_USE_SSL3SHAMD5
            case CALG_SSL3_SHAMD5:
                // Don't put in any PKCS crud
                pbStart = pbPKCS1Format + cbHash;
                *pbStart++ = 0;
                break;
#endif

            default:
                SetLastError((DWORD) NTE_BAD_ALGID);
                goto Ret;
        }
    }
    else
    {
        pbPKCS1Format[cbHash] = 0x00;
    }

    fRet = TRUE;
Ret:
    return fRet;
}

/*
 -      ApplyX931SigningFormat
 -
 *      Purpose:
 *                Format a buffer with X.9.31 for signing, assumes
 *                the buffer to be formatted is at least the length
 *                of the signature (cbSig).
 *
 */
void ApplyX931SigningFormat(IN  DWORD cbSig,
                            IN  BYTE *pbHash,
                            IN  DWORD cbHash,
                            IN  BOOL fDataInHash,
                            OUT BYTE *pbFormatted)
{
    DWORD   i;

    // insert P3
    pbFormatted[0] = 0xcc;
    pbFormatted[1] = 0x33;

    // Reverse it
    for (i = 0; i < cbHash; i++)
    {
        pbFormatted[i + 2] = pbHash[cbHash - (i + 1)];
    }
    pbFormatted[22] = 0xba;

    // insert P2
    memset(pbFormatted + 23, 0xbb, cbSig - 24);

    // insert P1
    if (fDataInHash)
    {
        pbFormatted[cbSig - 1] = 0x6b;
    }
    else
    {
        pbFormatted[cbSig - 1] = 0x4b;
    }
}

/*
 -      CPSignHash
 -
 *      Purpose:
 *                Create a digital signature from a hash
 *
 *
 *      Parameters:
 *               IN  hUID         -  Handle to the user identifcation
 *               IN  hHash        -  Handle to hash object
 *               IN  dwKeySpec    -  Key pair that is used to sign with
 *                                   algorithm to be used
 *               IN  sDescription -  Description of data to be signed
 *               IN  dwFlags      -  Flags values
 *               OUT pbSignture   -  Pointer to signature data
 *               OUT dwHashLen    -  Pointer to the len of the signature data
 *
 *      Returns:
 */
BOOL CPSignHash(IN  HCRYPTPROV hUID,
                IN  HCRYPTHASH hHash,
                IN  DWORD dwKeySpec,
                IN  LPCWSTR sDescription,
                IN  DWORD dwFlags,
                OUT BYTE *pbSignature,
                OUT DWORD *pdwSigLen)
{
    PNTAGUserList           pTmpUser = NULL;
    PNTAGHashList           pTmpHash;
    BSAFE_PRV_KEY           *pPrivKey = NULL;
    DWORD                   cbPrivKey = 0;
    MD2_object              *pMD2Hash;
    MD4_object              *pMD4Hash;
    MD5_object              *pMD5Hash;
    A_SHA_CTX               *pSHAHash;
    BYTE                    rgbTmpHash[SSL3_SHAMD5_LEN];
    DWORD                   cbTmpHash = SSL3_SHAMD5_LEN;
    BYTE                    *pbInput = NULL;
    BYTE                    *pbSigT = NULL;
    LPBYTE                  pbStart, pbEnd;
    DWORD                   cbSigLen;
    BYTE                    bTmp;
    BOOL                    fSigKey;
    BSAFE_PUB_KEY           *pPubKey;
    LPWSTR                  szPrompt;
    BOOL                    fDataInHash = FALSE;
    BOOL                    fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputSignHash(TRUE, FALSE, hUID, hHash, dwKeySpec, sDescription,
                           dwFlags, pbSignature, pdwSigLen);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if ((dwFlags & ~(CRYPT_NOHASHOID | CRYPT_X931_FORMAT)) != 0)
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    if ((pTmpUser = (PNTAGUserList) NTLCheckList (hUID, USER_HANDLE)) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    // get the user's public key
    if (dwKeySpec == AT_KEYEXCHANGE)
    {
        fSigKey = FALSE;
        pPubKey = (BSAFE_PUB_KEY *)pTmpUser->ContInfo.pbExchPub;
        szPrompt = g_Strings.pwszSignWExch;
    }
    else if (dwKeySpec == AT_SIGNATURE)
    {
        if (PROV_RSA_SCHANNEL == pTmpUser->dwProvType)
        {
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
        }

        fSigKey = TRUE;
        pPubKey = (BSAFE_PUB_KEY *)pTmpUser->ContInfo.pbSigPub;
        szPrompt = g_Strings.pwszSigning;
    }
    else
    {
        SetLastError((DWORD) NTE_BAD_ALGID);
        goto Ret;
    }

    // check to make sure the key exists
    if (NULL == pPubKey)
    {
        SetLastError((DWORD) NTE_BAD_KEYSET);
        goto Ret;
    }

    cbSigLen = (pPubKey->bitlen + 7) / 8;
    if (pbSignature == NULL || *pdwSigLen < cbSigLen)
    {
        *pdwSigLen = cbSigLen;
        if (pbSignature == NULL)
        {
            fRet = NTF_SUCCEED;
            goto Ret;
        }
        SetLastError(ERROR_MORE_DATA);
        goto Ret;
    }

    // check to see if the hash is in the hash list
    if ((pTmpHash = (PNTAGHashList)NTLValidate(hHash, hUID,
                                               HASH_HANDLE)) == NULL)
    {
        if (GetLastError() == NTF_FAILED)
            SetLastError((DWORD) NTE_BAD_HASH);
        goto Ret;
    }

    // zero the output buffer
    memset (pbSignature, 0, cbSigLen);

    switch (pTmpHash->Algid)
    {
#ifdef CSP_USE_MD2
        case CALG_MD2:
            pMD2Hash = (MD2_object *)pTmpHash->pHashData;
            break;

#endif

#ifdef CSP_USE_MD4
        case CALG_MD4:
            pMD4Hash = (MD4_object *)pTmpHash->pHashData;
            break;
#endif

#ifdef CSP_USE_MD5
        case CALG_MD5:
            pMD5Hash = (MD5_object *)pTmpHash->pHashData;
            break;

#endif

#ifdef CSP_USE_SHA
        case CALG_SHA:
        {
            pSHAHash = (A_SHA_CTX *)pTmpHash->pHashData;
            break;
        }
#endif

#ifdef CSP_USE_SSL3SHAMD5
        case CALG_SSL3_SHAMD5:

            // Hash value must have already been set.
            if ((pTmpHash->HashFlags & HF_VALUE_SET) == 0)
            {
                SetLastError((DWORD) NTE_BAD_HASH_STATE);
                goto Ret;
            }
            break;
#endif

        default:
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
    }

    // WARNING :due to vulnerabilities sDescription field is should NO longer be used
    if (sDescription != NULL)
    {
        if (!CPHashData(hUID, hHash, (CHAR *) sDescription,
                        lstrlenW(sDescription) * sizeof(WCHAR), 0))
        {
            SetLastError((DWORD) NTE_BAD_HASH);
            goto Ret;
        }
    }

    // check if this is a NULL hash (no data hashed) for X.9.31 format
    if (pTmpHash->dwHashState & DATA_IN_HASH)
    {
        fDataInHash = TRUE;
    }

    // now copy the hash into the input buffer
    cbTmpHash = pPubKey->keylen;
    if (!CPGetHashParam(hUID, hHash, HP_HASHVAL, rgbTmpHash, &cbTmpHash, 0))
        goto Ret;

    if ((pbInput = (BYTE *)_nt_malloc(pPubKey->keylen)) == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    // use X.9.31 padding for FIPS certification
    if (dwFlags & CRYPT_X931_FORMAT)
    {
        if (pTmpHash->Algid != CALG_SHA1)
        {
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
        }

        ApplyX931SigningFormat(cbSigLen,
                               rgbTmpHash,
                               cbTmpHash,
                               fDataInHash,
                               pbInput);
    }
    // use PKCS #1 padding
    else
    {
        if (!ApplyPKCS1SigningFormat(pPubKey,
                                     pTmpHash->Algid,
                                     rgbTmpHash,
                                     cbTmpHash,
                                     dwFlags,
                                     pbInput))
        {
            // last error set
            goto Ret;
        }
    }

    // encrypt the hash value in input
    if (NULL == (pbSigT = (BYTE *)_nt_malloc(pPubKey->keylen)))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    // load the appropriate key if necessary
    if (!UnprotectPrivKey(pTmpUser, szPrompt, fSigKey, FALSE))
    {
        SetLastError((DWORD) NTE_BAD_KEYSET);
        goto Ret;
    }

    // get the user's private key
    if (fSigKey)
        pPrivKey = (BSAFE_PRV_KEY *)pTmpUser->pSigPrivKey;
    else
        pPrivKey = (BSAFE_PRV_KEY *)pTmpUser->pExchPrivKey;

    if (pPrivKey == NULL)
    {
        SetLastError((DWORD) NTE_NO_KEY);
        goto Ret;
    }

    if (pPubKey->keylen != pPrivKey->keylen)
    {
        SetLastError((DWORD) NTE_BAD_KEYSET);
        goto Ret;
    }

    RSAPrivateDecrypt(pTmpUser->pOffloadInfo, pPrivKey, pbInput, pbSigT);

    memcpy(pbSignature, pbSigT, cbSigLen);

    *pdwSigLen = cbSigLen;

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputSignHash(FALSE, fRet, 0, 0, 0, NULL, 0, NULL, NULL);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (pbSigT)
        _nt_free (pbSigT, pPubKey->keylen);
    if (pbInput)
        _nt_free(pbInput, pPubKey->keylen);

    return fRet;
}


/*
 -      VerifyPKCS1SigningFormat
 -
 *      Purpose:
 *                Check the format on a buffer to make sure the PKCS 1
 *                formatting is correct.
 *
 */
BOOL VerifyPKCS1SigningFormat(IN  BSAFE_PUB_KEY *pKey,
                              IN  PNTAGHashList pTmpHash,
                              IN  BYTE *pbHash,
                              IN  DWORD cbHash,
                              IN  DWORD dwFlags,
                              OUT BYTE *pbPKCS1Format)
{
    MD2_object      *pMD2Hash;
    MD4_object      *pMD4Hash;
    MD5_object      *pMD5Hash;
    A_SHA_CTX       *pSHAHash;
    BYTE            *pbTmp;
    const BYTE      **rgEncOptions;
    BYTE            rgbTmpHash[SSL3_SHAMD5_LEN];
    DWORD           i;
    DWORD           cb;
    BYTE            *pbStart;
    DWORD           cbTmp;
    BOOL            fRet = FALSE;

    switch(pTmpHash->Algid)
    {

#ifdef CSP_USE_MD2

        case CALG_MD2:
            pMD2Hash = (MD2_object *)pTmpHash->pHashData;
            pbTmp = (BYTE *) &(pMD2Hash->MD);
            rgEncOptions = md2Encodings;
            break;

#endif

#ifdef CSP_USE_MD4

        case CALG_MD4:
            pMD4Hash = (MD4_object *)pTmpHash->pHashData;
            pbTmp = (BYTE *) &(pMD4Hash->MD);
            rgEncOptions = md4Encodings;
            break;

#endif

#ifdef CSP_USE_MD5
        case CALG_MD5:
            pMD5Hash = (MD5_object *)pTmpHash->pHashData;
            pbTmp = (BYTE *)pMD5Hash->digest;
            rgEncOptions = md5Encodings;
            break;

#endif

#ifdef CSP_USE_SHA
        case CALG_SHA:
            pSHAHash = (A_SHA_CTX *)pTmpHash->pHashData;
            pbTmp = (BYTE *)pSHAHash->HashVal;
            rgEncOptions = shaEncodings;
            break;

#endif

#ifdef CSP_USE_SSL3SHAMD5
        case CALG_SSL3_SHAMD5:

            // Hash value must have already been set.
            if ((pTmpHash->HashFlags & HF_VALUE_SET) == 0)
            {
                SetLastError((DWORD) NTE_BAD_HASH);
                goto Ret;
            }
            pbTmp = pTmpHash->pHashData;
            rgEncOptions = NULL;

            break;
#endif

        default:
            SetLastError((DWORD) NTE_BAD_HASH);
            goto Ret;
    }

    // Reverse the hash to match the signature.
    for (i = 0; i < cbHash; i++)
    {
        rgbTmpHash[i] = pbHash[cbHash - (i + 1)];
    }

    // See if it matches.
    if (memcmp (rgbTmpHash, pbPKCS1Format, cbHash))
    {
        SetLastError((DWORD) NTE_BAD_SIGNATURE);
        goto Ret;
    }

    cb = cbHash;
    if (!(CRYPT_NOHASHOID & dwFlags))
    {
        // Check for any signature type identifiers
        if(rgEncOptions != NULL) {
            for (i = 0; 0 != *rgEncOptions[i]; i += 1)
            {
                pbStart = (LPBYTE)rgEncOptions[i];
                cbTmp = *pbStart++;
                if (0 == memcmp(&pbPKCS1Format[cb], pbStart, cbTmp))
                {
                    cb += cbTmp;   // Adjust the end of the hash data.
                    break;
                }
            }
        }
    }

    // check to make sure the rest of the PKCS #1 padding is correct
    if ((0x00 != pbPKCS1Format[cb]) || (0x00 != pbPKCS1Format[pKey->datalen]) ||
         (0x1 != pbPKCS1Format[pKey->datalen - 1]))
    {
        SetLastError((DWORD) NTE_BAD_SIGNATURE);
        goto Ret;
    }

    for (i = cb + 1; i < (DWORD)pKey->datalen - 1; i++)
    {
        if (0xff != pbPKCS1Format[i])
        {
            SetLastError((DWORD) NTE_BAD_SIGNATURE);
            goto Ret;
        }
    }

    fRet = TRUE;
Ret:
    return fRet;
}

/*
 -      VerifyX931SigningFormat
 -
 *      Purpose:
 *                Check the format on a buffer to make sure the X.9.31
 *                formatting is correct.
 *
 */
BOOL VerifyX931SigningFormat(IN  BYTE *pbHash,
                             IN  DWORD cbHash,
                             IN  BOOL fDataInHash,
                             IN  BYTE *pbFormatted,
                             IN  DWORD cbFormatted)
{
    BYTE    rgbTmpHash[SSL3_SHAMD5_LEN];
    DWORD   i;
    BOOL    fRet = FALSE;

    // check P3
    if ((0xcc != pbFormatted[0]) || (0x33 != pbFormatted[1]) ||
        (0xba != pbFormatted[cbHash + 2]))
    {
        SetLastError((DWORD) NTE_BAD_SIGNATURE);
        goto Ret;
    }

    // Reverse the hash to match the signature and check if it matches.
    for (i = 0; i < cbHash; i++)
    {
        rgbTmpHash[i] = pbHash[cbHash - (i + 1)];
    }

    if (0 != memcmp(rgbTmpHash, pbFormatted + 2, cbHash))
    {
        SetLastError((DWORD) NTE_BAD_SIGNATURE);
        goto Ret;
    }

    // check P2
    for (i = 23; i < (cbFormatted - 24); i++)
    {
        if (0xbb != pbFormatted[i])
        {
            SetLastError((DWORD) NTE_BAD_SIGNATURE);
            goto Ret;
        }
    }

    // check P1
    if (fDataInHash)
    {
        if (0x6b != pbFormatted[cbFormatted - 1])
        {
            SetLastError((DWORD) NTE_BAD_SIGNATURE);
            goto Ret;
        }
    }
    else
    {
        if (0x4b != pbFormatted[cbFormatted - 1])
        {
            SetLastError((DWORD) NTE_BAD_SIGNATURE);
            goto Ret;
        }
    }

    fRet = TRUE;
Ret:
    return fRet;
}

/*
 -      CPVerifySignature
 -
 *      Purpose:
 *                Used to verify a signature against a hash object
 *
 *
 *      Parameters:
 *               IN  hUID         -  Handle to the user identifcation
 *               IN  hHash        -  Handle to hash object
 *               IN  pbSignture   -  Pointer to signature data
 *               IN  dwSigLen     -  Length of the signature data
 *               IN  hPubKey      -  Handle to the public key for verifying
 *                                   the signature
 *               IN  Algid        -  Algorithm identifier of the signature
 *                                   algorithm to be used
 *               IN  sDescription -  String describing the signed data
 *               IN  dwFlags      -  Flags values
 *
 *      Returns:
 */
BOOL CPVerifySignature(IN HCRYPTPROV hUID,
                       IN HCRYPTHASH hHash,
                       IN CONST BYTE *pbSignature,
                       IN DWORD dwSigLen,
                       IN HCRYPTKEY hPubKey,
                       IN LPCWSTR sDescription,
                       IN DWORD dwFlags)
{
    PNTAGUserList           pTmpUser;
    PNTAGHashList           pTmpHash;
    PNTAGKeyList            pPubKey;
    BSAFE_PUB_KEY           *pKey;
    BYTE                    *pOutput = NULL;
    BSAFE_PUB_KEY           *pBsafePubKey;
    BYTE                    *pbSigT = NULL;
    BYTE                    *pbStart;
    BYTE                    *pbEnd;
    int                     i;
    BYTE                    rgbTmpHash[SSL3_SHAMD5_LEN];
    DWORD                   cbTmpHash = SSL3_SHAMD5_LEN;
    BYTE                    bTmp;
    DWORD                   cbLocalSigLen;
    BOOL                    fDataInHash = FALSE;
    BOOL                    fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputVerifySignature(TRUE, FALSE, hUID, hHash, (BYTE*)pbSignature,
                                  dwSigLen, hPubKey, sDescription, dwFlags);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if ((dwFlags & ~(CRYPT_NOHASHOID | CRYPT_X931_FORMAT)) != 0)
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    // check the user identification
    if ((pTmpUser = (PNTAGUserList) NTLCheckList (hUID, USER_HANDLE)) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    // check to see if the hash is in the hash list
    if ((pTmpHash = (PNTAGHashList) NTLValidate(hHash, hUID,
                                                HASH_HANDLE)) == NULL)
    {
        // NTLValidate doesn't know what error to set
        // so it set NTE_FAIL -- fix it up.
        if (GetLastError() == NTE_FAIL)
            SetLastError((DWORD) NTE_BAD_HASH);

        goto Ret;
    }

    if ((CALG_MD2 != pTmpHash->Algid) &&
        (CALG_MD4 != pTmpHash->Algid) &&
        (CALG_MD5 != pTmpHash->Algid) &&
        (CALG_SHA != pTmpHash->Algid) &&
        (CALG_SSL3_SHAMD5 != pTmpHash->Algid))
    {
        SetLastError((DWORD) NTE_BAD_HASH);
        goto Ret;
    }

    switch(HNTAG_TO_HTYPE((HNTAG)hPubKey))
    {
        case SIGPUBKEY_HANDLE:
        case EXCHPUBKEY_HANDLE:
            if ((pPubKey = (PNTAGKeyList) NTLValidate((HNTAG)hPubKey,
                              hUID, HNTAG_TO_HTYPE((HNTAG)hPubKey))) == NULL)
            {
                // NTLValidate doesn't know what error to set
                // so it set NTE_FAIL -- fix it up.
                if (GetLastError() == NTE_FAIL)
                    SetLastError((DWORD) NTE_BAD_KEY);

                goto Ret;
            }
            break;

        default:
            SetLastError((DWORD) NTE_BAD_KEY);
            goto Ret;
    }


    pKey = (BSAFE_PUB_KEY *)pPubKey->pKeyValue;
    pBsafePubKey = (BSAFE_PUB_KEY *) pKey;
    cbLocalSigLen = (pBsafePubKey->bitlen+7)/8;

    if (dwSigLen != cbLocalSigLen)
    {
        SetLastError((DWORD) NTE_BAD_SIGNATURE);
        goto Ret;
    }

    if ((pOutput = (BYTE *)_nt_malloc(pBsafePubKey->keylen)) == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    // encrypt the hash value in output
    if (NULL == (pbSigT = (BYTE *)_nt_malloc(pBsafePubKey->keylen)))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }
    memset(pbSigT, 0, pBsafePubKey->keylen);
    memcpy(pbSigT, pbSignature, cbLocalSigLen);
    if (0 != RSAPublicEncrypt(pTmpUser->pOffloadInfo, pBsafePubKey, pbSigT, pOutput))
    {
        SetLastError((DWORD) NTE_BAD_SIGNATURE);
        goto Ret;
    }

    // WARNING : due to vulnerabilities sDescription field is should NO longer be used
    if (sDescription != NULL)
    {
        if (!CPHashData(hUID, hHash, (CHAR *) sDescription,
                        lstrlenW(sDescription) * sizeof(WCHAR), 0))
        {
            goto Ret;
        }
    }

    // check if this is a NULL hash (no data hashed) for X.9.31 format
    if (pTmpHash->dwHashState & DATA_IN_HASH)
    {
        fDataInHash = TRUE;
    }

    if (!CPGetHashParam(hUID, hHash, HP_HASHVAL, rgbTmpHash, &cbTmpHash, 0))
    {
        goto Ret;
    }

    // use X.9.31 padding for FIPS certification
    if (dwFlags & CRYPT_X931_FORMAT)
    {
        if (!VerifyX931SigningFormat(rgbTmpHash,
                                     cbTmpHash,
                                     fDataInHash,
                                     pOutput,
                                     (pBsafePubKey->bitlen + 7) / 8))
        {
            goto Ret;
        }
    }
    // use PKCS #1 padding
    else
    {
        if (!VerifyPKCS1SigningFormat(pKey,
                                      pTmpHash,
                                      rgbTmpHash,
                                      cbTmpHash,
                                      dwFlags,
                                      pOutput))
        {
            goto Ret;
        }
    }

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputVerifySignature(FALSE, fRet, 0, 0, NULL, 0, 0, NULL, 0);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (pbSigT)
        _nt_free(pbSigT, pBsafePubKey->keylen);
    if (pOutput)
        _nt_free(pOutput, pBsafePubKey->keylen);

    return fRet;
}

