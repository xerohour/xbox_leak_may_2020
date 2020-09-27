/////////////////////////////////////////////////////////////////////////////
//  FILE          : nt_crypt.c                                             //
//  DESCRIPTION   : Crypto CP interfaces:                                  //
//                  CPEncrypt                                              //
//                  CPDecrypt                                              //
//  AUTHOR        :                                                        //
//  HISTORY       :                                                        //
//      Jan 25 1995 larrys  Changed from Nametag                           //
//      Jan 30 1995 larrys  Cleanup code                                   //
//      Feb 23 1995 larrys  Changed NTag_SetLastError to SetLastError      //
//      Apr 10 1995 larrys  Added freeing of RC4 key data on final flag    //
//      May  8 1995 larrys  Changes for MAC hashing                        //
//      May  9 1995 larrys  Added check for double encryption              //
//      May 10 1995 larrys  added private api calls                        //
//      Jul 13 1995 larrys  Changed MAC stuff                              //
//      Aug 16 1995 larrys  Removed exchange key stuff                     //
//      Oct 05 1995 larrys  Fixed bugs 50 & 51                             //
//      Nov  8 1995 larrys  Fixed SUR bug 10769                            //
//      May 15 1996 larrys  Changed NTE_NO_MEMORY to ERROR_NOT_ENOUGHT...  //
//                                                                         //
//  Copyright (C) 1993 Microsoft Corporation   All Rights Reserved         //
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"
#include "nt_rsa.h"
#include "mac.h"
#include "tripldes.h"
#include "swnt_pk.h"
#include "protstor.h"
#include "ntagum.h"

#ifndef STRONG
#define DE_BLOCKLEN             8       // size of double encrypt block

BYTE        dbDEncrypt[DE_BLOCKLEN];    // First 8 bytes of last encrypt
BOOL        fDEncrypt = FALSE;          // Flag for Double encrypt
BYTE        dbDDecrypt[DE_BLOCKLEN];    // First 8 bytes of last Decrypt
DWORD       fDDecrypt = FALSE;          // Flag for Double Decrypt
#endif // STRONG

extern CSP_STRINGS g_Strings;

BOOL FIsLegalKey(
                 PNTAGUserList pTmpUser,
                 PNTAGKeyList pKey,
                 BOOL fRC2BigKeyOK
                 );

BOOL CPInflateKey(
                  IN PNTAGKeyList pTmpKey
                  );

/* BlockEncrypt -

        Run a block cipher over a block of size *pdwDataLen.

*/

BOOL BlockEncrypt(void EncFun(BYTE *In, BYTE *Out, void *key, int op),
                  PNTAGKeyList pKey,
                  int BlockLen,
                  BOOL Final,
                  BYTE  *pbData,
                  DWORD *pdwDataLen,
                  DWORD dwBufLen)
{
    DWORD   cbPartial, dwPadVal, dwDataLen;
    BYTE    *pbBuf;

    dwDataLen = *pdwDataLen;
    
    // Check to see if we are encrypting something already

    if (pKey->InProgress == FALSE)
    {
        pKey->InProgress = TRUE;
        if (pKey->Mode == CRYPT_MODE_CBC || pKey->Mode == CRYPT_MODE_CFB)
        {
            memcpy(pKey->FeedBack, pKey->IV, BlockLen);
        }
    }

    // check length of the buffer and calculate the pad
    // (if multiple of blocklen, do a full block of pad)
    
    cbPartial = (dwDataLen % BlockLen);
    if (Final)
    {
        dwPadVal = BlockLen - cbPartial;
        if (pbData == NULL || dwBufLen < dwDataLen + dwPadVal)
        {
            // set what we need
            *pdwDataLen = dwDataLen + dwPadVal;
            if (pbData == NULL)
            {
                return NTF_SUCCEED;
            }

            SetLastError(ERROR_MORE_DATA);
            return NTF_FAILED;
        }
        else
        {
            // Clear encryption flag
            pKey->InProgress = FALSE;
        }

    }
    else
    {

        if (pbData == NULL)
        {
            *pdwDataLen = dwDataLen;
            return NTF_SUCCEED;
        }

        // Non-Final make multiple of the blocklen
        if (cbPartial)
        {
            // set what we need
            *pdwDataLen = dwDataLen + cbPartial;
            ASSERT((*pdwDataLen % BlockLen) == 0);
            SetLastError((DWORD) NTE_BAD_DATA);
            return NTF_FAILED;
        }
        dwPadVal = 0;
    }

    // allocate memory for a temporary buffer
    if ((pbBuf = (BYTE *)_nt_malloc(BlockLen)) == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NTF_FAILED;
    }

    if (dwPadVal)
    {
        // Fill the pad with a value equal to the
        // length of the padding, so decrypt will
        // know the length of the original data
        // and as a simple integrity check.
        
        memset(pbData + dwDataLen, (int)dwPadVal, (size_t)dwPadVal);
    }

    dwDataLen += dwPadVal;
    *pdwDataLen = dwDataLen;

    ASSERT((dwDataLen % BlockLen) == 0);
    
    // pump the full blocks of data through
    while (dwDataLen)
    {
        ASSERT(dwDataLen >= (DWORD)BlockLen);

        // put the plaintext into a temporary
        // buffer, then encrypt the data
        // back into the caller's buffer
            
        memcpy(pbBuf, pbData, BlockLen);

        switch(pKey->Mode)
        {
            case CRYPT_MODE_CBC:
                CBC(EncFun, BlockLen, pbData, pbBuf, pKey->pData,
                    ENCRYPT, pKey->FeedBack);
                break;

            case CRYPT_MODE_ECB:
                EncFun(pbData, pbBuf, pKey->pData, ENCRYPT);
                break;

            case CRYPT_MODE_CFB:
                CFB(EncFun, BlockLen, pbData, pbBuf, pKey->pData,
                    ENCRYPT, pKey->FeedBack);
                break;

            default:
                _nt_free(pbBuf, BlockLen);
                SetLastError((DWORD) NTE_BAD_ALGID);
                return NTF_FAILED;
                break;
        }
        pbData += BlockLen;
        dwDataLen -= BlockLen;
    }
    
    _nt_free(pbBuf, BlockLen);

    return NTF_SUCCEED;

}

BOOL BlockDecrypt(void DecFun(BYTE *In, BYTE *Out, void *key, int op),
                  PNTAGKeyList pKey,
                  int BlockLen,
                  BOOL Final,
                  BYTE  *pbData,
                  DWORD *pdwDataLen)
{
    BYTE    *pbBuf;
    DWORD   dwDataLen, BytePos, dwPadVal, i;

    dwDataLen = *pdwDataLen;
    
    // Check to see if we are decrypting something already

    if (pKey->InProgress == FALSE)
    {
        pKey->InProgress = TRUE;
        if (pKey->Mode == CRYPT_MODE_CBC ||
            pKey->Mode == CRYPT_MODE_CFB)
        {
            memcpy(pKey->FeedBack, pKey->IV, BlockLen);
        }
    }

    // The data length must be a multiple of the algorithm
    // pad size.
    if (dwDataLen % BlockLen)
    {
        SetLastError((DWORD) NTE_BAD_DATA);
        return NTF_FAILED;
    }

    // allocate memory for a temporary buffer
    if ((pbBuf = (BYTE *)_nt_malloc(BlockLen)) == NULL)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NTF_FAILED;
    }

    // pump the data through the decryption, including padding
    // NOTE: the total length is a multiple of BlockLen
    
    for (BytePos = 0; (BytePos + BlockLen) <= dwDataLen; BytePos += BlockLen)
    {
        // put the encrypted text into a temp buffer
        memcpy (pbBuf, pbData + BytePos, BlockLen);

        switch (pKey->Mode)
        {
            case CRYPT_MODE_CBC:
                CBC(DecFun, BlockLen, pbData + BytePos, pbBuf, pKey->pData,
                    DECRYPT, pKey->FeedBack);
                break;

            case CRYPT_MODE_ECB:
                DecFun(pbData + BytePos, pbBuf, pKey->pData, DECRYPT);
                break;

            case CRYPT_MODE_CFB:
                CFB(DecFun, BlockLen, pbData + BytePos, pbBuf, pKey->pData,
                    DECRYPT, pKey->FeedBack);
                break;

            default:
                _nt_free(pbBuf, BlockLen);
                SetLastError((DWORD) NTE_BAD_ALGID);
                return NTF_FAILED;
                break;
        }

    }

    _nt_free(pbBuf, BlockLen);

    // if this is the final block of data then
    // verify the padding and remove the pad size
    // from the data length. NOTE: The padding is
    // filled with a value equal to the length
    // of the padding and we are guaranteed >= 1
    // byte of pad.
    // ## NOTE: if the pad is wrong, the user's
    // buffer is hosed, because
    // ## we've decrypted into the user's
    // buffer -- can we re-encrypt it?

    if (Final)
    {
        pKey->InProgress = FALSE;
        
        dwPadVal = (DWORD)*(pbData + dwDataLen - 1);
        if (dwPadVal == 0 || dwPadVal > (DWORD) BlockLen)
        {
            SetLastError((DWORD) NTE_BAD_DATA);
            return NTF_FAILED;
        }
        
        // Make sure all the (rest of the) pad bytes are correct.
        for (i=1; i<dwPadVal; i++)
        {
            if (pbData[dwDataLen - (i + 1)] != dwPadVal)
            {
                SetLastError((DWORD) NTE_BAD_DATA);
                return NTF_FAILED;
            }
        }

        // Only need to update the length on final
        *pdwDataLen -= dwPadVal;
    }

    return NTF_SUCCEED;
}

BOOL RSAEncryptionAllowed(
                          PNTAGUserList pTmpUser,
                          DWORD dwFlags
                          )
{
    BOOL    fRet = FALSE;

    if (FIsWinNT())
    {
        if (((dwFlags == 0) || (0x9C580000 != dwFlags)) &&
            (pTmpUser->fIsLocalSystem))
        {
            fRet = TRUE;
        }
    }
    return fRet;
}

/*
 -      SymEncrypt
 -
 *      Purpose:
 *                Encrypt data with symmetric algorithms.  This function is used
 *                by the LocalEncrypt function as well as the WrapSymKey (nt_key.c)
 *                function.
 *
 *      Parameters:
 *               IN  pKey          -  Handle to the key
 *               IN  fFinal        -  Boolean indicating if this is the final
 *                                    block of plaintext
 *               IN OUT pbData     -  Data to be encrypted
 *               IN OUT pcbData    -  Pointer to the length of the data to be
 *                                    encrypted
 *               IN cbBuf          -  Size of Data buffer
 *
 *      Returns:
 */

BOOL SymEncrypt(
                IN PNTAGKeyList pKey,
                IN BOOL fFinal,
                IN OUT BYTE *pbData,
                IN OUT DWORD *pcbData,
                IN DWORD cbBuf
                )
{
    BOOL    fRet = FALSE;

    // determine which algorithm is to be used
    switch (pKey->Algid)
    {
#ifdef CSP_USE_RC2
        case CALG_RC2:
            if (!BlockEncrypt(RC2, pKey, RC2_BLOCKLEN, fFinal, pbData,
                              pcbData, cbBuf))
            {
                goto Ret;
            }
            break;
#endif

#ifdef CSP_USE_DES
        case CALG_DES:
            if (!BlockEncrypt(des, pKey, DES_BLOCKLEN, fFinal, pbData,
                              pcbData, cbBuf))
            {
                goto Ret;
            }
            break;
#endif

#ifdef CSP_USE_3DES
        case CALG_3DES_112:
        case CALG_3DES:
            if (!BlockEncrypt(tripledes, pKey, DES_BLOCKLEN, fFinal, pbData,
                              pcbData, cbBuf))
            {
                goto Ret;
            }
            break;
#endif

#ifdef CSP_USE_RC4

        case CALG_RC4:
            if (pbData == NULL)
            {
                fRet = TRUE;
                goto Ret;
            }
            if (*pcbData > cbBuf)
            {
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }
            rc4((struct RC4_KEYSTRUCT *)pKey->pData, *pcbData, pbData);

            if (fFinal)
            {
                if (pKey->pData)
                    _nt_free (pKey->pData, pKey->cbDataLen);
                pKey->pData = 0;
                pKey->cbDataLen = 0;
            }

            break;
#endif

        default:
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
            break;
    }

    fRet = TRUE;
Ret:
    return fRet;
}

/*
 -      LocalEncrypt
 -
 *      Purpose:
 *                Encrypt data
 *
 *
 *      Parameters:
 *               IN  hUID          -  Handle to the CSP user
 *               IN  hKey          -  Handle to the key
 *               IN  hHash         -  Optional handle to a hash
 *               IN  Final         -  Boolean indicating if this is the final
 *                                    block of plaintext
 *               IN OUT pbData     -  Data to be encrypted
 *               IN OUT pdwDataLen -  Pointer to the length of the data to be
 *                                    encrypted
 *               IN dwBufLen       -  Size of Data buffer
 *               IN fIsExternal    -  Flag to tell if the call is for internal 
 *                                    CSP use or external caller
 *
 *      Returns:
 */

BOOL LocalEncrypt(IN HCRYPTPROV hUID,
                  IN HCRYPTKEY hKey,
                  IN HCRYPTHASH hHash,
                  IN BOOL Final,
                  IN DWORD dwFlags,
                  IN OUT BYTE *pbData,
                  IN OUT DWORD *pdwDataLen,
                  IN DWORD dwBufSize,
                  IN BOOL fIsExternal)
{
    DWORD               dwDataLen;
    PNTAGUserList       pTmpUser;
    PNTAGKeyList        pTmpKey;
    PNTAGKeyList        pTmpKey2;
    PNTAGHashList       pTmpHash;
    DWORD               dwLen;
    MACstate            *pMAC;
    BSAFE_PUB_KEY       *pBsafePubKey;
    BYTE                *pbOutput = NULL;
    BOOL                fRet = FALSE;

    if ((dwFlags != 0) && (0x9C580000 != dwFlags))
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    dwDataLen = *pdwDataLen;

    if ((Final == FALSE) && (dwDataLen == 0))
    {
        // If no data to encrypt and this isn't the last block,
        // then we're done. (if Final, we need to pad)
        fRet = TRUE;
        goto Ret;
    }

    if ((pTmpUser = (PNTAGUserList) NTLCheckList (hUID, USER_HANDLE)) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    //
    // Check if encryption allowed
    //
    if (fIsExternal &&
        (PROV_RSA_SCHANNEL != pTmpUser->dwProvType) &&
        ((pTmpUser->Rights & CRYPT_DISABLE_CRYPT) == CRYPT_DISABLE_CRYPT))
    {
        SetLastError((DWORD) NTE_PERM);
        goto Ret;
    }

    if (NULL == (pTmpKey = (PNTAGKeyList) NTLValidate(hKey, hUID, KEY_HANDLE)))
    {
        if (NULL == (pTmpKey = (PNTAGKeyList) NTLValidate(hKey, hUID, EXCHPUBKEY_HANDLE)))
        {
            // NTLValidate doesn't know what error to set
            // so it set NTE_FAIL -- fix it up.
            if (GetLastError() == NTE_FAIL)
            {
                SetLastError((DWORD) NTE_BAD_KEY);
            }
            goto Ret;
        }
    }

    if ((pTmpKey->Algid != CALG_RSA_KEYX) &&
        (!FIsLegalKey(pTmpUser, pTmpKey, FALSE)))
    {
        SetLastError((DWORD) NTE_BAD_KEY);
        goto Ret;
    }

    if ((Final == FALSE) && (pTmpKey->Algid != CALG_RC4))
    {
        if (dwDataLen < CRYPT_BLKLEN)
        {
            *pdwDataLen = CRYPT_BLKLEN;
            SetLastError((DWORD) NTE_BAD_DATA);
            goto Ret;
        }
    }

#ifndef STRONG
    if (fDEncrypt && pbData != NULL && *pdwDataLen != 0)
    {
        if (memcmp(dbDEncrypt, pbData, DE_BLOCKLEN) == 0)
        {
            SetLastError((DWORD) NTE_DOUBLE_ENCRYPT);
            goto Ret;
        }
    }
#endif // STRONG

    // Check if we should do an auto-inflate
    if ((pTmpKey->pData == NULL) && (pTmpKey->Algid != CALG_RSA_KEYX))
    {
        if (NTAG_FAILED(CPInflateKey(pTmpKey)))
        {
            goto Ret;
        }
    }

    if ((hHash != 0) && (NULL != pbData))
    {
        if ((pTmpHash = (PNTAGHashList)NTLValidate(hHash, hUID,
            HASH_HANDLE)) == NULL)
        {
            SetLastError((DWORD) NTE_BAD_HASH);
            goto Ret;
        }

	    if (pTmpHash->Algid == CALG_MAC)
        {
            // Check if we should do an auto-inflate
            pMAC = pTmpHash->pHashData;
            if ((pTmpKey2 = (PNTAGKeyList) NTLValidate(pMAC->hKey, hUID,
                                                      KEY_HANDLE)) == NULL)
            {
                // NTLValidate doesn't know what error to set
                // so it set NTE_FAIL -- fix it up.
                if (GetLastError() == NTE_FAIL)
                {
                    SetLastError((DWORD) NTE_BAD_KEY);
                }
                goto Ret;
            }
            if (pTmpKey2->pData == NULL)
            {
                if (NTAG_FAILED(CPInflateKey(pTmpKey2)))
                {
                    goto Ret;
                }
            }
        }

        if (!CPHashData(hUID, hHash, pbData, *pdwDataLen, 0))
        {
            goto Ret;
        }

    }

    // determine which algorithm is to be used
    switch (pTmpKey->Algid)
    {
        case CALG_RSA_KEYX:
#ifndef STRONG
            if (!RSAEncryptionAllowed(pTmpUser, dwFlags))
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }
#endif // STRONG

            pBsafePubKey = (BSAFE_PUB_KEY *) pTmpKey->pKeyValue;

            if (pBsafePubKey == NULL)
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            // compute length of resulting data
            dwLen = (pBsafePubKey->bitlen + 7) / 8;
        
            if (!CheckDataLenForRSAEncrypt(dwLen, *pdwDataLen, dwFlags))
            {
                SetLastError((DWORD) NTE_BAD_LEN);
                goto Ret;
            }

            if (pbData == NULL || dwBufSize < dwLen)
            {
                *pdwDataLen = dwLen;    // set what we need
                if (pbData == NULL)
                {
                    fRet = TRUE;
                    goto Ret;
                }
                SetLastError(ERROR_MORE_DATA);
                goto Ret;
            }
            
            if (NULL == (pbOutput = (BYTE*)_nt_malloc(dwLen)))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            // perform the RSA encryption
            if (!RSAEncrypt(pTmpUser, pBsafePubKey, pbData, *pdwDataLen,
                            pTmpKey->pbParams, pTmpKey->cbParams, dwFlags,
                            pbOutput))
            {
                goto Ret;
            }

            *pdwDataLen = dwLen;
            memcpy(pbData, pbOutput, *pdwDataLen);

            break;

        default:
            if (!SymEncrypt(pTmpKey, Final, pbData, pdwDataLen, dwBufSize))
            {
                goto Ret;
            }
    }

#ifndef STRONG
    if (pbData != NULL && *pdwDataLen >= DE_BLOCKLEN)
    {
        memcpy(dbDEncrypt, pbData, DE_BLOCKLEN);
        fDEncrypt = TRUE;
    }
    else
    {
        fDEncrypt = FALSE;
    }
#endif  // STRONG

    fRet = TRUE;
Ret:
    if (pbOutput)
        _nt_free(pbOutput, dwLen);

    return fRet;
}

/*
 -      CPEncrypt
 -
 *      Purpose:
 *                Encrypt data
 *
 *
 *      Parameters:
 *               IN  hUID          -  Handle to the CSP user
 *               IN  hKey          -  Handle to the key
 *               IN  hHash         -  Optional handle to a hash
 *               IN  Final         -  Boolean indicating if this is the final
 *                                    block of plaintext
 *               IN  dwFlags       -  Flags values
 *               IN OUT pbData     -  Data to be encrypted
 *               IN OUT pdwDataLen -  Pointer to the length of the data to be
 *                                    encrypted
 *               IN dwBufLen       -  Size of Data buffer
 *
 *      Returns:
 */

BOOL CPEncrypt(IN HCRYPTPROV hUID,
               IN HCRYPTKEY hKey,
               IN HCRYPTHASH hHash,
               IN BOOL Final,
               IN DWORD dwFlags,
               IN OUT BYTE *pbData,
               IN OUT DWORD *pdwDataLen,
               IN DWORD dwBufSize)
{
    BOOL    fRet;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputEncrypt(TRUE, FALSE, hUID, hKey, hHash, Final,
                          dwFlags, pbData, pdwDataLen, dwBufSize);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    fRet = LocalEncrypt(hUID, hKey, hHash, Final, dwFlags,
                        pbData, pdwDataLen, dwBufSize, TRUE);

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputEncrypt(FALSE, fRet, 0, 0, 0, FALSE, 0, NULL, NULL, 0);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    return fRet;
}

/*
 -      SymDecrypt
 -
 *      Purpose:
 *                Decrypt data with symmetric algorithms.  This function is used
 *                by the LocalDecrypt function as well as the UnWrapSymKey (nt_key.c)
 *                function.
 *
 *      Parameters:
 *               IN  pKey          -  Handle to the key
 *               IN  pHash         -  Handle to a hash if needed
 *               IN  fFinal        -  Boolean indicating if this is the final
 *                                    block of plaintext
 *               IN OUT pbData     -  Data to be decrypted
 *               IN OUT pcbData    -  Pointer to the length of the data to be
 *                                    decrypted
 *
 *      Returns:
 */

BOOL SymDecrypt(
                IN PNTAGKeyList pKey,
                IN PNTAGHashList pHash,
                IN BOOL fFinal,
                IN OUT BYTE *pbData,
                IN OUT DWORD *pcbData
                )
{
    MACstate    *pMAC;
    BOOL        fRet = FALSE;

    // determine which algorithm is to be used
    switch (pKey->Algid)
    {
#ifdef CSP_USE_RC2
        // the decryption is to be done with the RC2 algorithm
        case CALG_RC2:

            if (BlockDecrypt(RC2, pKey, RC2_BLOCKLEN, fFinal, pbData,
                             pcbData) == NTF_FAILED)
            {
                goto Ret;
            }

            if ((fFinal) && (NULL != pHash) && (pHash->Algid == CALG_MAC) &&
                (pKey->Mode == CRYPT_MODE_CBC))
            {
                pMAC = (MACstate *)pHash->pHashData;
                memcpy(pMAC->Feedback, pKey->FeedBack, RC2_BLOCKLEN);
                pHash->dwHashState |= DATA_IN_HASH;
            }

            break;
#endif

#ifdef CSP_USE_DES
        // the decryption is to be done with DES
        case CALG_DES:
            if (BlockDecrypt(des, pKey, DES_BLOCKLEN, fFinal, pbData,
                             pcbData) == NTF_FAILED)
            {
                goto Ret;
            }

            if ((fFinal) && (NULL != pHash) &&  (pHash->Algid == CALG_MAC) &&
                (pKey->Mode == CRYPT_MODE_CBC))
            {
                pMAC = (MACstate *)pHash->pHashData;
                memcpy(pMAC->Feedback, pKey->FeedBack, RC2_BLOCKLEN);
                pHash->dwHashState |= DATA_IN_HASH;
            }
            break;
#endif

#ifdef CSP_USE_3DES
        // the decryption is to be done with the triple DES
        case CALG_3DES_112:
        case CALG_3DES:
            if (BlockDecrypt(tripledes, pKey, DES_BLOCKLEN, fFinal, pbData,
                             pcbData) == NTF_FAILED)
            {
                goto Ret;
            }

            if ((fFinal) && (NULL != pHash) &&  (pHash->Algid == CALG_MAC) &&
                (pKey->Mode == CRYPT_MODE_CBC))
            {
                pMAC = (MACstate *)pHash->pHashData;
                memcpy(pMAC->Feedback, pKey->FeedBack, RC2_BLOCKLEN);
                pHash->dwHashState |= DATA_IN_HASH;
            }
            break;
#endif

#ifdef CSP_USE_RC4
        case CALG_RC4:
            rc4((struct RC4_KEYSTRUCT *)pKey->pData, *pcbData, pbData);
            if (fFinal)
            {
                _nt_free (pKey->pData, pKey->cbDataLen);
                pKey->pData = 0;
                pKey->cbDataLen = 0;
            }

            break;
#endif

        default:
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
            break;
    }

    fRet = TRUE;
Ret:
    return fRet;
}

/*
 -      LocalDecrypt
 -
 *      Purpose:
 *                Decrypt data
 *
 *
 *      Parameters:
 *               IN  hUID          -  Handle to the CSP user
 *               IN  hKey          -  Handle to the key
 *               IN  hHash         -  Optional handle to a hash
 *               IN  Final         -  Boolean indicating if this is the final
 *                                    block of ciphertext
 *               IN  dwFlags       -  Flags values
 *               IN OUT pbData     -  Data to be decrypted
 *               IN OUT pdwDataLen -  Pointer to the length of the data to be
 *                                    decrypted
 *               IN fIsExternal    -  Flag to tell if the call is for internal 
 *                                    CSP use or external caller
 *
 *      Returns:
 */
BOOL LocalDecrypt(IN HCRYPTPROV hUID,
                  IN HCRYPTKEY hKey,
                  IN HCRYPTHASH hHash,
                  IN BOOL Final,
                  IN DWORD dwFlags,
                  IN OUT BYTE *pbData,
                  IN OUT DWORD *pdwDataLen,
                  IN BOOL fIsExternal)
{
    PNTAGUserList       pTmpUser;
    PNTAGKeyList        pTmpKey;
    PNTAGHashList       pTmpHash = NULL;
    DWORD               dwLen;
    BSAFE_PRV_KEY       *pBsafePrvKey = NULL;
    DWORD               cbBsafePrvKey = 0;
    BYTE                *pbNewData = NULL;
    DWORD               cbNewData;
    BOOL                fMachineKeyset = FALSE;
    BOOL                fRet = FALSE;

    if ((dwFlags != 0) && (0x9C580000 != dwFlags))
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    // We're done if decrypting 0 bytes.
    if (*pdwDataLen == 0)
    {
        if (Final == TRUE)
        {
            SetLastError((DWORD) NTE_BAD_LEN);
            goto Ret;
        }
        fRet = TRUE;
        goto Ret;
    }

    if ((pTmpUser = (PNTAGUserList) NTLCheckList (hUID, USER_HANDLE)) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    //
    // Check if decryption allowed
    //
    if (fIsExternal &&
        (PROV_RSA_SCHANNEL != pTmpUser->dwProvType) &&
        ((pTmpUser->Rights & CRYPT_DISABLE_CRYPT) == CRYPT_DISABLE_CRYPT))
    {
        SetLastError((DWORD) NTE_PERM);
        goto Ret;
    }

    // Check the key against the user.
    if (NULL == (pTmpKey = (PNTAGKeyList) NTLValidate(hKey, hUID, KEY_HANDLE)))
    {
        if (NULL == (pTmpKey = (PNTAGKeyList) NTLValidate(hKey, hUID, EXCHPUBKEY_HANDLE)))
        {
            // NTLValidate doesn't know what error to set
            // so it set NTE_FAIL -- fix it up.
            if (GetLastError() == NTE_FAIL)
                SetLastError((DWORD) NTE_BAD_KEY);

            goto Ret;
        }
    }

#ifndef STRONG
    if (fDDecrypt)
    {
        if (memcmp(dbDDecrypt, pbData, DE_BLOCKLEN) == 0)
        {
            SetLastError((DWORD) NTE_DOUBLE_ENCRYPT);
            goto Ret;
        }
    }
#endif // STRONG

    if ((pTmpKey->Algid != CALG_RSA_KEYX) &&
        (!FIsLegalKey(pTmpUser, pTmpKey, TRUE)))
    {
        SetLastError((DWORD) NTE_BAD_KEY);
        goto Ret;
    }

    // Check if we should do an auto-inflate
    if ((pTmpKey->pData == NULL) && (pTmpKey->Algid != CALG_RSA_KEYX))
    {
        if (NTAG_FAILED(CPInflateKey(pTmpKey)))
        {
            goto Ret;
        }
    }

    // Check if we need to hash before encryption
    if (hHash != 0)
    {
        if ((pTmpHash = (PNTAGHashList)NTLValidate(hHash, hUID,
                                                   HASH_HANDLE)) == NULL)
        {
            SetLastError((DWORD) NTE_BAD_HASH);
            goto Ret;
        }

    }
    
    // determine which algorithm is to be used
    switch (pTmpKey->Algid)
    {
        case CALG_RSA_KEYX:
#ifndef STRONG
            if (!RSAEncryptionAllowed(pTmpUser, dwFlags))
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }
#endif // STRONG

            // check if the public key matches the private key
            if (pTmpUser->ContInfo.pbExchPub == NULL)
            {
                SetLastError((DWORD) NTE_NO_KEY);
                goto Ret;
            }

            if ((pTmpUser->ContInfo.ContLens.cbExchPub != pTmpKey->cbKeyLen)||
                memcmp(pTmpUser->ContInfo.pbExchPub, pTmpKey->pKeyValue,
                       pTmpUser->ContInfo.ContLens.cbExchPub))
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            // if using protected store then load the key now
            if (!UnprotectPrivKey(pTmpUser, g_Strings.pwszImportSimple,
                                  FALSE, FALSE))
            {
                SetLastError((DWORD) NTE_BAD_KEYSET);
                goto Ret;
            }
            pBsafePrvKey = (BSAFE_PRV_KEY *)pTmpUser->pExchPrivKey;

            if (NULL == pBsafePrvKey)
            {
                SetLastError((DWORD) NTE_NO_KEY);
                goto Ret;
            }

            //; perform the RSA decryption
            if (!RSADecrypt(pTmpUser, pBsafePrvKey, pbData, *pdwDataLen,
                            pTmpKey->pbParams, pTmpKey->cbParams,
                            dwFlags, &pbNewData, &cbNewData))
            {
                goto Ret;
            }

            *pdwDataLen = cbNewData;
            memcpy(pbData, pbNewData, *pdwDataLen);
            break;

        default:
            if (!SymDecrypt(pTmpKey, pTmpHash, Final, pbData, pdwDataLen))
            {
                goto Ret;
            }
    }

    if (hHash != 0)
    {
        if (pTmpHash->Algid != CALG_MAC)
        {
            if (!CPHashData(hUID, hHash, pbData, *pdwDataLen, 0))
            {
                goto Ret;
            }
        }
    }

#ifndef STRONG
    if (*pdwDataLen >= DE_BLOCKLEN)
    {
        memcpy(dbDDecrypt, pbData, DE_BLOCKLEN);
        fDDecrypt = TRUE;
    }
    else
    {
        fDDecrypt = FALSE;
    }
#endif // STRONG

    fRet = TRUE;
Ret:
    if (pbNewData)
        _nt_free(pbNewData, cbNewData);
    
    return fRet;
}

/*
 -      CPDecrypt
 -
 *      Purpose:
 *                Decrypt data
 *
 *
 *      Parameters:
 *               IN  hUID          -  Handle to the CSP user
 *               IN  hKey          -  Handle to the key
 *               IN  hHash         -  Optional handle to a hash
 *               IN  Final         -  Boolean indicating if this is the final
 *                                    block of ciphertext
 *               IN  dwFlags       -  Flags values
 *               IN OUT pbData     -  Data to be decrypted
 *               IN OUT pdwDataLen -  Pointer to the length of the data to be
 *                                    decrypted
 *
 *      Returns:
 */
BOOL CPDecrypt(IN HCRYPTPROV hUID,
               IN HCRYPTKEY hKey,
               IN HCRYPTHASH hHash,
               IN BOOL Final,
               IN DWORD dwFlags,
               IN OUT BYTE *pbData,
               IN OUT DWORD *pdwDataLen)
{
    BOOL    fRet;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputDecrypt(TRUE, FALSE, hUID, hKey, hHash, Final,
                          dwFlags, pbData, pdwDataLen);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    fRet = LocalDecrypt(hUID, hKey, hHash, Final,
                        dwFlags, pbData, pdwDataLen, TRUE);

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputDecrypt(FALSE, fRet, 0, 0, 0, FALSE, 0, NULL, NULL);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    return fRet;
}

