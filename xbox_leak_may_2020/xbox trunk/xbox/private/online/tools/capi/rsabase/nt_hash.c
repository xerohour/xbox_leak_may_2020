/////////////////////////////////////////////////////////////////////////////
//  FILE          : nt_hash.c                                              //
//  DESCRIPTION   : Crypto CP interfaces:                                  //
//                  CPBeginHash                                            //
//                  CPUpdateHash                                           //
//                  CPDestroyHash                                          //
//  AUTHOR        :                                                        //
//  HISTORY       :                                                        //
//      Jan 25 1995 larrys  Changed from Nametag                           //
//      Feb 23 1995 larrys  Changed NTag_SetLastError to SetLastError      //
//      May  8 1995 larrys  Changes for MAC hashing                        //
//      May 10 1995 larrys  added private api calls                        //
//      Jul 13 1995 larrys  Changed MAC stuff                              //
//      Aug 07 1995 larrys  Added Auto-Inflate to CryptBeginHash           //
//      Aug 30 1995 larrys  Removed RETURNASHVALUE from CryptGetHashValue  //
//      Sep 19 1995 larrys  changed USERDATA to CRYPT_USERDATA             //
//      Oct 03 1995 larrys  check for 0 on Createhash for hKey             //
//      Oct 05 1995 larrys  Changed HashSessionKey to hash key material    //
//      Oct 13 1995 larrys  Removed CPGetHashValue                         //
//      Oct 17 1995 larrys  Added MD2                                      //
//      Nov  3 1995 larrys  Merge for NT checkin                           //
//      Nov 14 1995 larrys  Fixed memory leak                              //
//      Mar 01 1996 rajeshk Added check for Hash Values                    //
//      May 15 1996 larrys  Changed NTE_NO_MEMORY to ERROR_NOT_ENOUGHT...  //
//      Jun  6 1996 a-johnb Added support for SSL 3.0 signatures           //
//      Apr 25 1997 jeffspel Fix for Bug 76393, GPF on pbData = NULL       //
//      May 23 1997 jeffspel Added provider type checking                  //
//                                                                         //
//  Copyright (C) 1993 Microsoft Corporation   All Rights Reserved         //
/////////////////////////////////////////////////////////////////////////////

#include "precomp.h"
#include "nt_rsa.h"
#include "tripldes.h"
#include "mac.h"
#include "ssl3.h"

BOOL FIsLegalKey(
                 PNTAGUserList pTmpUser,
                 PNTAGKeyList pKey,
                 BOOL fRC2BigKeyOK
                 );

BOOL CPInflateKey(
                  IN PNTAGKeyList pTmpKey
                  );

BOOL BlockEncrypt(void EncFun(BYTE *In, BYTE *Out, void *key, int op),
                  PNTAGKeyList pKey,
                  int BlockLen,
                  BOOL Final,
                  BYTE  *pbData,
                  DWORD *pdwDataLen,
                  DWORD dwBufLen);

#ifdef CSP_USE_MD5
//
// Function : TestMD5
//
// Description : This function hashes the passed in message with the MD5 hash
//               algorithm and returns the resulting hash value.
//
BOOL TestMD5(
             BYTE *pbMsg,
             DWORD cbMsg,
             BYTE *pbHash
             )
{
    MD5_CTX	MD5;
    BOOL    fRet = FALSE;

    // Check length for input data
    if (0 == cbMsg)
    {
        goto Ret;
    }

    // Initialize MD5
    MD5Init(&MD5);

    // Compute MD5 
    MD5Update(&MD5, pbMsg, cbMsg);

    MD5Final(&MD5);
    memcpy(pbHash, MD5.digest, MD5DIGESTLEN);

    fRet = TRUE;
Ret:
    return fRet;
}
#endif // CSP_USE_MD5

#ifdef CSP_USE_SHA1
//
// Function : TestSHA1
//
// Description : This function hashes the passed in message with the SHA1 hash
//               algorithm and returns the resulting hash value.
//
BOOL TestSHA1(
              BYTE *pbMsg,
              DWORD cbMsg,
              BYTE *pbHash
              )
{
    A_SHA_CTX	HashContext;
    BOOL        fRet = FALSE;

    // Check length for input data
    if (0 == cbMsg)
    {
        goto Ret;
    }

    // Initialize SHA
    A_SHAInit(&HashContext);

    // Compute SHA 
    A_SHAUpdate(&HashContext, pbMsg, cbMsg);

    A_SHAFinal(&HashContext, pbHash);

    fRet = TRUE;
Ret:
    return fRet;
}
#endif // CSP_USE_SHA1

BOOL ValidHashAlgid(
                    PNTAGUserList pTmpUser,
                    ALG_ID Algid
                    )
{
    if ((PROV_RSA_SCHANNEL == pTmpUser->dwProvType) &&
        ((CALG_MD2 == Algid) || (CALG_MD4 == Algid)))
        return FALSE;
    else
        return TRUE;
}

// local function for creating hashes
BOOL LocalCreateHash(
                     IN ALG_ID Algid,
                     OUT BYTE **ppbHashData,
                     OUT DWORD *pcbHashData
                     )
{
    BOOL    fRet = FALSE;

    switch (Algid)
    {
#ifdef CSP_USE_MD2
        case CALG_MD2:
        {
            MD2_object      *pMD2Hash;

            *pcbHashData = sizeof(MD2_object);

            if (NULL == (pMD2Hash = (MD2_object *)_nt_malloc(*pcbHashData)))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            memset ((BYTE *)pMD2Hash, 0, sizeof(MD2_object));

            *ppbHashData = (BYTE*)pMD2Hash;

            // Set up the Initial MD2 Hash State
            pMD2Hash->FinishFlag = FALSE;
        
            break;
        }
#endif

#ifdef CSP_USE_MD4
        case CALG_MD4:
        {
            MD4_object      *pMD4Hash;

            *pcbHashData = sizeof(MD4_object);

            if (NULL == (pMD4Hash = (MD4_object *)_nt_malloc(*pcbHashData)))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            memset ((BYTE *)pMD4Hash, 0, sizeof(MD4_object));

            // Set up our State
            *ppbHashData = (BYTE*)pMD4Hash;
            
            // Set up the Initial MD4 Hash State
            pMD4Hash->FinishFlag = FALSE;
            MDbegin(&pMD4Hash->MD);

            break;
        }
#endif

#ifdef CSP_USE_MD5
        case CALG_MD5:
        {
            MD5_object      *pMD5Hash;

            *pcbHashData = sizeof(MD5_object);

            if (NULL == (pMD5Hash = (MD5_object *)_nt_malloc(*pcbHashData)))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            // Set up the our state
            *ppbHashData = (BYTE*)pMD5Hash;

            pMD5Hash->FinishFlag = FALSE;

            // call the code to actually begin an MD5 hash
            MD5Init(pMD5Hash);

            break;
        }
#endif

#ifdef CSP_USE_SHA
        case CALG_SHA:
        {
            A_SHA_CTX       *pSHAHash;

            *pcbHashData = sizeof(A_SHA_CTX);
                
            if (NULL == (pSHAHash = (A_SHA_CTX *)_nt_malloc(*pcbHashData)))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            // Set up our state
            *ppbHashData = (BYTE*)pSHAHash;
            
            A_SHAInit(pSHAHash);
            pSHAHash->FinishFlag = FALSE;

            break;
        }
#endif

        default:
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
    }

    fRet = TRUE;
Ret:
    return fRet;
}

/*
 -  CPBeginHash
 -
 *  Purpose:
 *                initate the hashing of a stream of data
 *
 *
 *  Parameters:
 *               IN  hUID    -  Handle to the user identifcation
 *               IN  Algid   -  Algorithm identifier of the hash algorithm
 *                              to be used
 *               IN  hKey    -  Optional key for MAC algorithms
 *               IN  dwFlags -  Flags values
 *               OUT pHash   -  Handle to hash object
 *
 *  Returns:
 */
BOOL CPCreateHash(IN HCRYPTPROV hUID,
                  IN ALG_ID Algid,
                  IN HCRYPTKEY hKey,
                  IN DWORD dwFlags,
                  OUT HCRYPTHASH *phHash)
{
    PNTAGUserList   pTmpUser;
    PNTAGHashList   pCurrentHash = NULL;
    PNTAGKeyList    pTmpKey;
    WORD            count = 0;
//#ifdef CSP_USE_SSL3
    PSCH_HASH       pSChHash;
//#endif // CSP_USE_SSL3
    BOOL            fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputCreateHash(TRUE, FALSE, hUID, Algid, hKey, dwFlags, phHash);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (dwFlags != 0)
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    // check if the user handle is valid
    if (NULL == (pTmpUser = NTLCheckList (hUID, USER_HANDLE)))
    {
        goto Ret;
    }

    if (!ValidHashAlgid(pTmpUser, Algid))
    {
        SetLastError((DWORD)NTE_BAD_ALGID);
        goto Ret;
    }

    // Prepare the structure to be used as the hash handle
    if (NULL == (pCurrentHash =
                  (PNTAGHashList) _nt_malloc(sizeof(NTAGHashList))))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    memset(pCurrentHash, 0, sizeof(NTAGHashList));
    pCurrentHash->Algid = Algid;
    pCurrentHash->hUID = hUID;

    // determine which hash algorithm is to be used
    switch (Algid)
    {
#ifdef CSP_USE_MAC

        case CALG_MAC:
        {
            MACstate        *pMACVal;

            if (hKey == 0)
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            if ((pTmpKey = (PNTAGKeyList) NTLValidate(hKey, hUID,
                                                      KEY_HANDLE)) == NULL)
            {
                if (GetLastError() == NTE_FAIL)
                {
                    SetLastError((DWORD) NTE_BAD_KEY);
                }
                goto Ret;
            }

            if (pTmpKey->Mode != CRYPT_MODE_CBC)
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            // Check if we should do an auto-inflate
            if (pTmpKey->pData == NULL)
            {
                if (NTAG_FAILED(CPInflateKey(pTmpKey)))
                {
                    goto Ret;
                }
            }

            if ((pMACVal = (MACstate *)_nt_malloc(sizeof(MACstate)))==NULL)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            pCurrentHash->pHashData = pMACVal;
            pCurrentHash->dwDataLen = sizeof(MACstate);
            pMACVal->dwBufLen = 0;
            pMACVal->hKey = hKey;
            pMACVal->FinishFlag = FALSE;

            break;
        }
#endif

        case CALG_HMAC:
        {
            if (hKey == 0)
            {
                SetLastError(NTE_BAD_KEY);
                goto Ret;
            }

            if ((pTmpKey = (PNTAGKeyList) NTLValidate(hKey, hUID,
                                                      KEY_HANDLE)) == NULL)
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            pCurrentHash->hKey = hKey;
            break;
        }

#ifdef CSP_USE_SSL3SHAMD5
        case CALG_SSL3_SHAMD5:
        {
       
            if ((pCurrentHash->pHashData = _nt_malloc(SSL3_SHAMD5_LEN)) == NULL)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            pCurrentHash->dwDataLen = SSL3_SHAMD5_LEN;

            break;
        }
#endif

#ifdef CSP_USE_SSL3
        case CALG_SCHANNEL_MASTER_HASH:
        {

            if (0 == hKey)
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            if (NULL == (pTmpKey = (PNTAGKeyList) NTLValidate(hKey, hUID, KEY_HANDLE)))
            {
                if (GetLastError() == NTE_FAIL)
                    SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            if ((CALG_SSL3_MASTER != pTmpKey->Algid) &&
                (CALG_PCT1_MASTER != pTmpKey->Algid) &&
                (pTmpKey->cbKeyLen > MAX_PREMASTER_LEN))
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            pCurrentHash->dwDataLen = sizeof(SCH_HASH);
            if (NULL == (pCurrentHash->pHashData = (BYTE *)_nt_malloc(pCurrentHash->dwDataLen)))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }
            memset(pCurrentHash->pHashData, 0, pCurrentHash->dwDataLen);
            pSChHash = (PSCH_HASH)pCurrentHash->pHashData;

            pSChHash->ProtocolAlgid = pTmpKey->Algid;

            if (!SChGenMasterKey(pTmpKey, pSChHash))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            break;
        }

        case CALG_TLS1PRF:
        {
            PRF_HASH    *pPRFHash;
            PSCH_KEY    pSChKey;

            if (0 == hKey)
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            if (NULL == (pTmpKey = (PNTAGKeyList) NTLValidate(hKey, hUID, KEY_HANDLE)))
            {
                if (GetLastError() == NTE_FAIL)
                    SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }

            if (CALG_TLS1_MASTER != pTmpKey->Algid)
            {
                SetLastError((DWORD) NTE_BAD_KEY);
                goto Ret;
            }
            // check if the master key is finished
            pSChKey = (PSCH_KEY)pTmpKey->pData;
            if ((!pSChKey->fFinished) || (TLS_MASTER_LEN != pTmpKey->cbKeyLen))
            {
                SetLastError((DWORD) NTE_BAD_KEY_STATE);
                goto Ret;
            }

            pCurrentHash->dwDataLen = sizeof(PRF_HASH);
            if (NULL == (pCurrentHash->pHashData = (BYTE *)_nt_malloc(pCurrentHash->dwDataLen)))
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }
            memset(pCurrentHash->pHashData, 0, pCurrentHash->dwDataLen);
            pPRFHash = (PRF_HASH*)pCurrentHash->pHashData;

            memcpy(pPRFHash->rgbMasterKey, pTmpKey->pKeyValue, TLS_MASTER_LEN);
            break;
        }
#endif // CSP_USE_SSL3

        default:
            if (hKey != 0)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                goto Ret;
            }

            if (!LocalCreateHash(Algid, (BYTE**)&pCurrentHash->pHashData,
                                 &pCurrentHash->dwDataLen))
            {
                goto Ret;
            }
    }
  
    if (NTLMakeItem(phHash, HASH_HANDLE, pCurrentHash) == NTF_FAILED)
    {
        goto Ret;          // error already set
    }

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputCreateHash(FALSE, fRet, 0, 0, 0, 0, phHash);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if ((NTF_FAILED == fRet) && pCurrentHash)
    {
        if (pCurrentHash->pHashData)
            _nt_free(pCurrentHash->pHashData, pCurrentHash->dwDataLen);
        _nt_free(pCurrentHash, sizeof(NTAGHashList));
    }

    return fRet;
}

BOOL LocalHashData(
                   IN ALG_ID Algid,
                   IN OUT BYTE *pbHashData,
                   IN BYTE *pbData,
                   IN DWORD cbData
                   )
{
    BYTE    *ptmp;
    DWORD   BytePos;
    BOOL    fRet = FALSE;

    switch(Algid)
    {
#ifdef CSP_USE_MD2
        case CALG_MD2:
        {
            MD2_object      *pMD2Hash;

            // make sure the hash is updatable
            pMD2Hash = (MD2_object *)pbHashData;
            if (pMD2Hash->FinishFlag == TRUE)
            {
                SetLastError((DWORD) NTE_BAD_HASH_STATE);
                goto Ret;
            }

            if (0 != MD2Update(&pMD2Hash->MD, pbData, cbData))
            {
                SetLastError((DWORD)NTE_FAIL);
                goto Ret;
            }

	        break;
        }
#endif

#ifdef CSP_USE_MD4
        case CALG_MD4:
        {
            MD4_object      *pMD4Hash;

            pMD4Hash = (MD4_object *)pbHashData;

            // make sure the hash is updatable
            if (pMD4Hash->FinishFlag == TRUE)
            {
                SetLastError((DWORD)NTE_BAD_HASH_STATE);
                goto Ret;
            }

            // MD4 hashes when the size == MD4BLOCKSIZE and finishes the
            // hash when the given size is < MD4BLOCKSIZE.
            // So, ensure that the user always gives a full block here --
            // when NTagFinishHash is called, we'll send the last bit and
            // that'll finish off the hash.

            ptmp = (BYTE *)pbData;
            for (;;)
            {
                // check if there's plenty of room in the buffer
                if (cbData < (MD4BLOCKSIZE - pMD4Hash->BufLen))
                {
                    // just append to whatever's already
                    memcpy(pMD4Hash->Buf + pMD4Hash->BufLen, ptmp, cbData);

                    // set of the trailing buffer length field
                    pMD4Hash->BufLen += (BYTE)cbData;
                    break;
                }

                // determine what we need to fill the buffer, then do it.
                BytePos = MD4BLOCKSIZE - pMD4Hash->BufLen;
                memcpy(pMD4Hash->Buf + pMD4Hash->BufLen, ptmp, BytePos);

                // The buffer is now full, process it.
                
                if (MD4_SUCCESS != MDupdate(&pMD4Hash->MD, pMD4Hash->Buf,
                                            MD4BYTESTOBITS(MD4BLOCKSIZE)))
                {
                    SetLastError((DWORD) NTE_FAIL);
                    goto Ret;
                }

                // now it's empty.
                pMD4Hash->BufLen = 0;

                // we processed some bytes, so reflect that and try again
                cbData -= BytePos;
                ptmp += BytePos;

                if (cbData == 0)
                {
                    break;
                }
            }
            break;
        }
#endif

#ifdef CSP_USE_MD5
        case CALG_MD5:
        {
            MD5_object      *pMD5Hash;

            // make sure the hash is updatable
            pMD5Hash = (MD5_object *)pbHashData;
            if (pMD5Hash->FinishFlag == TRUE)
            {
                SetLastError((DWORD) NTE_BAD_HASH_STATE);
                goto Ret;
            }
            MD5Update(pMD5Hash, pbData, cbData);

            break;
        }
#endif

#ifdef CSP_USE_SHA
        case CALG_SHA:
        {
            A_SHA_CTX   *pSHAHash;

            // make sure the hash is updatable
            pSHAHash = (A_SHA_CTX *)pbHashData;
            if (pSHAHash->FinishFlag == TRUE)
            {
                SetLastError((DWORD) NTE_BAD_HASH_STATE);
                goto Ret;
            }
            A_SHAUpdate(pSHAHash, pbData, cbData);

            break;
        }
#endif

        default:
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
    }

    fRet = TRUE;
Ret:
    return fRet;
}

BOOL LocalMACData(IN HCRYPTPROV hUID,
                  IN PNTAGHashList pTmpHash,
                  IN CONST BYTE *pbData,
                  IN DWORD cbData
                  )
{
    MACstate        *pMAC;
    PNTAGKeyList    pTmpKey;
    BYTE            *pbTmp;
    DWORD           dwTmpLen;
    BYTE            *pb = NULL;
    DWORD           cb;
    DWORD           i;
    BYTE            *pbJunk = NULL;
    DWORD           dwBufSlop;
    DWORD           dwEncLen;
    BOOL            fRet = FALSE;

    dwTmpLen = cbData;
    pbTmp = (BYTE *) pbData;

    switch(pTmpHash->Algid)
    {
#ifdef CSP_USE_MAC
        case CALG_MAC:
        {
            pMAC = (MACstate *)pTmpHash->pHashData;

            // make sure the hash is updatable
            if (pMAC->FinishFlag == TRUE)
            {
                SetLastError((DWORD) NTE_BAD_HASH_STATE);
                goto Ret;
            }

            if ((pTmpKey = (PNTAGKeyList) NTLValidate(pMAC->hKey, hUID,
                                                      KEY_HANDLE)) == NULL)
            {
                if (GetLastError() == NTE_FAIL)
                {
                    SetLastError((DWORD) NTE_BAD_KEY);
                }
                goto Ret;
            }

            if (pMAC->dwBufLen + dwTmpLen <= CRYPT_BLKLEN)
            {
                memcpy(pMAC->Buffer + pMAC->dwBufLen, pbTmp, dwTmpLen);
                pMAC->dwBufLen += dwTmpLen;
                fRet = NTF_SUCCEED;
                goto Ret;
            }

            memcpy(pMAC->Buffer+pMAC->dwBufLen, pbTmp,
                   (CRYPT_BLKLEN - pMAC->dwBufLen));

            dwTmpLen -= (CRYPT_BLKLEN - pMAC->dwBufLen);
            pbTmp += (CRYPT_BLKLEN - pMAC->dwBufLen);

            pMAC->dwBufLen = CRYPT_BLKLEN;

            switch (pTmpKey->Algid)
            {
                case CALG_RC2:
                    if (BlockEncrypt(RC2, pTmpKey, RC2_BLOCKLEN, FALSE,
                                     pMAC->Buffer,  &pMAC->dwBufLen,
                                     CRYPT_BLKLEN) == NTF_FAILED)
                    {
                        goto Ret;
                    }
                    break;

                case CALG_DES:
                    if (BlockEncrypt(des, pTmpKey, DES_BLOCKLEN, FALSE,
                                     pMAC->Buffer,  &pMAC->dwBufLen,
                                     CRYPT_BLKLEN) == NTF_FAILED)
                    {
                        goto Ret;
                    }
                    break;

#ifdef CSP_USE_3DES
                case CALG_3DES_112:
                case CALG_3DES:
                    if (BlockEncrypt(tripledes, pTmpKey, DES_BLOCKLEN,
                                     FALSE, pMAC->Buffer,  &pMAC->dwBufLen,
                                     CRYPT_BLKLEN) == NTF_FAILED)
                    {
                        goto Ret;
                    }
                    break;
#endif
            }

            pMAC->dwBufLen = 0;
        
            dwBufSlop = dwTmpLen % CRYPT_BLKLEN;
            if (dwBufSlop == 0)
            {
                dwBufSlop = CRYPT_BLKLEN;
            }

            if ((pbJunk = _nt_malloc(dwTmpLen)) == NULL)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            memcpy(pbJunk, pbTmp, dwTmpLen - dwBufSlop);

            dwEncLen = dwTmpLen - dwBufSlop;

            switch (pTmpKey->Algid)
            {
                case CALG_RC2:
                    if (BlockEncrypt(RC2, pTmpKey, RC2_BLOCKLEN, FALSE,
                                     pbJunk,  &dwEncLen,
                                     dwTmpLen) == NTF_FAILED)
                    {
                        goto Ret;
                    }
                    break;

                case CALG_DES:
                    if (BlockEncrypt(des, pTmpKey, DES_BLOCKLEN, FALSE,
                                     pbJunk,  &dwEncLen,
                                     dwTmpLen) == NTF_FAILED)
                    {
                        goto Ret;
                    }
                    break;

#ifdef CSP_USE_3DES
                case CALG_3DES_112:
                case CALG_3DES:
                    if (BlockEncrypt(tripledes, pTmpKey, DES_BLOCKLEN,
                                     FALSE, pbJunk,  &dwEncLen,
                                     dwTmpLen) == NTF_FAILED)
                    {
                        goto Ret;
                    }
                    break;
#endif
            }

            memcpy(pMAC->Buffer, pbTmp + dwEncLen, dwBufSlop);
            pMAC->dwBufLen = dwBufSlop;

            break;
        }
#endif

        case CALG_HMAC:
        {
            if (!(pTmpHash->HMACState & HMAC_STARTED))
            {
                if ((pTmpKey = (PNTAGKeyList) NTLValidate(pTmpHash->hKey, hUID,
                                                          KEY_HANDLE)) == NULL)
                {
                    if (GetLastError() == NTE_FAIL)
                    {
                        SetLastError((DWORD) NTE_BAD_KEY);
                    }
                    goto Ret;
                }

                if (pTmpKey->cbKeyLen < pTmpHash->cbHMACInner)
                    cb = pTmpHash->cbHMACInner;
                else
                    cb = pTmpKey->cbKeyLen;

                if (NULL == (pb = (BYTE *)_nt_malloc(cb)))
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    goto Ret;
                }
                memcpy(pb, pTmpHash->pbHMACInner, pTmpHash->cbHMACInner);

                // currently no support for byte reversed keys with HMAC
                for(i=0;i<pTmpKey->cbKeyLen;i++)
                    pb[i] ^= (pTmpKey->pKeyValue)[i];

                if (!LocalHashData(pTmpHash->HMACAlgid, pTmpHash->pHashData, pb, cb))
                {
                    goto Ret;
                }

                pTmpHash->HMACState |= HMAC_STARTED;
            }

            if (!LocalHashData(pTmpHash->HMACAlgid, pTmpHash->pHashData,
                               (BYTE*)pbData, cbData))        
            {
                goto Ret;
            }

            break;
        }
    }

    fRet = TRUE;
Ret:
    if (pbJunk)
        _nt_free(pbJunk, dwTmpLen);
    if (pb)
         _nt_free(pb, cb);

    return fRet;
}

/*
 -  CPHashData
 -
 *  Purpose:
 *                Compute the cryptograghic hash on a stream of data
 *
 *
 *  Parameters:
 *               IN  hUID      -  Handle to the user identifcation
 *               IN  hHash     -  Handle to hash object
 *               IN  pbData    -  Pointer to data to be hashed
 *               IN  dwDataLen -  Length of the data to be hashed
 *               IN  dwFlags   -  Flags values
 *
 *  Returns:
 */
BOOL CPHashData(IN HCRYPTPROV hUID,
                IN HCRYPTHASH hHash,
                IN CONST BYTE *pbData,
                IN DWORD dwDataLen,
                IN DWORD dwFlags)
{
    PNTAGHashList               pTmpHash;
    PNTAGUserList               pUser;
    DWORD                       dwBufSlop;
    DWORD                       dwEncLen;
    BOOL                        fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputHashData(TRUE, FALSE, hUID, hHash, (BYTE*)pbData,
                           dwDataLen, dwFlags);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (0 != (dwFlags & ~(CRYPT_USERDATA)))
    {
        SetLastError((DWORD) NTE_BAD_FLAGS);
        goto Ret;
    }

    if ((pUser = (PNTAGUserList) NTLCheckList(hUID, USER_HANDLE)) == NULL)
    {
        SetLastError((DWORD)NTE_BAD_UID);
        goto Ret;
    }

    if (0 == dwDataLen)
    {
        fRet = NTF_SUCCEED;
        goto Ret;
    }

    if (NULL == pbData)
    {
        SetLastError((DWORD)NTE_BAD_DATA);
        goto Ret;
    }

    if ((pTmpHash = (PNTAGHashList) NTLValidate(hHash, hUID,
                                                HASH_HANDLE)) == NULL)
    {
        // NTLValidate doesn't know what error to set
        // so it set NTE_FAIL -- fix it up.
        if (GetLastError() == NTE_FAIL)
            SetLastError((DWORD) NTE_BAD_HASH);
        
        goto Ret;
    }

    if (pTmpHash->HashFlags & HF_VALUE_SET)
    {
        SetLastError((DWORD) NTE_BAD_HASH_STATE);
        goto Ret;
    }

    switch (pTmpHash->Algid)
    {
#ifdef CSP_USE_MAC
        case CALG_MAC:
#endif // CSP_USE_MAC
        case CALG_HMAC:
            if (!LocalMACData(hUID, pTmpHash, pbData, dwDataLen))
            {
                goto Ret;
            }
            break;

/*
        {
            pMAC = (MACstate *)pTmpHash->pHashData;

            // make sure the hash is updatable
            if (pMAC->FinishFlag == TRUE)
            {
                SetLastError((DWORD) NTE_BAD_HASH_STATE);
                goto Ret;
            }

            if ((pTmpKey = (PNTAGKeyList) NTLValidate(pMAC->hKey, hUID,
                                                      KEY_HANDLE)) == NULL)
            {
                if (GetLastError() == NTE_FAIL)
                {
                    SetLastError((DWORD) NTE_BAD_KEY);
                }
                goto Ret;
            }

            if (pMAC->dwBufLen + dwTmpLen <= CRYPT_BLKLEN)
            {
                memcpy(pMAC->Buffer+pMAC->dwBufLen, pbTmp, dwTmpLen);
                pMAC->dwBufLen += dwTmpLen;
                fRet = NTF_SUCCEED;
                goto Ret;
            }

            memcpy(pMAC->Buffer+pMAC->dwBufLen, pbTmp,
                   (CRYPT_BLKLEN - pMAC->dwBufLen));

            dwTmpLen -= (CRYPT_BLKLEN - pMAC->dwBufLen);
            pbTmp += (CRYPT_BLKLEN - pMAC->dwBufLen);

            pMAC->dwBufLen = CRYPT_BLKLEN;

            switch (pTmpKey->Algid)
            {
                case CALG_RC2:
                    if (BlockEncrypt(RC2, pTmpKey, RC2_BLOCKLEN, FALSE,
                                     pMAC->Buffer,  &pMAC->dwBufLen,
                                     CRYPT_BLKLEN) == NTF_FAILED)
                    {
                        goto Ret;
                    }
                    break;

                case CALG_DES:
                    if (BlockEncrypt(des, pTmpKey, DES_BLOCKLEN, FALSE,
                                     pMAC->Buffer,  &pMAC->dwBufLen,
                                     CRYPT_BLKLEN) == NTF_FAILED)
                    {
                        goto Ret;
                    }
            }

            pMAC->dwBufLen = 0;
        
            dwBufSlop = dwTmpLen % CRYPT_BLKLEN;
            if (dwBufSlop == 0)
            {
                dwBufSlop = CRYPT_BLKLEN;
            }

            if ((pbJunk = _nt_malloc(dwTmpLen)) == NULL)
            {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                goto Ret;
            }

            memcpy(pbJunk, pbTmp, dwTmpLen - dwBufSlop);

            dwEncLen = dwTmpLen - dwBufSlop;

            switch (pTmpKey->Algid)
            {
                case CALG_RC2:
                    if (BlockEncrypt(RC2, pTmpKey, RC2_BLOCKLEN, FALSE,
                                     pbJunk,  &dwEncLen,
                                     dwTmpLen) == NTF_FAILED)
                    {
                        goto Ret;
                    }
                    break;

                case CALG_DES:
                    if (BlockEncrypt(des, pTmpKey, DES_BLOCKLEN, FALSE,
                                     pbJunk,  &dwEncLen,
                                     dwTmpLen) == NTF_FAILED)
                    {
                        goto Ret;
                    }
            }

        
            memcpy(pMAC->Buffer, pbTmp + dwEncLen, dwBufSlop);
            pMAC->dwBufLen = dwBufSlop;

            break;
        }
#endif

        case CALG_HMAC:
        {
            if (!(pTmpHash->HMACState & HMAC_STARTED))
            {
                if ((pTmpKey = (PNTAGKeyList) NTLValidate(pTmpHash->hKey, hUID,
                                                          KEY_HANDLE)) == NULL)
                {
                    if (GetLastError() == NTE_FAIL)
                    {
                        SetLastError((DWORD) NTE_BAD_KEY);
                    }
                    goto Ret;
                }

                if (pTmpKey->cbKeyLen < pTmpHash->cbHMACInner)
                    cb = pTmpHash->cbHMACInner;
                else
                    cb = pTmpKey->cbKeyLen;

                if (NULL == (pb = (BYTE *)_nt_malloc(cb)))
                {
                    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                    goto Ret;
                }
                memcpy(pb, pTmpHash->pbHMACInner, pTmpHash->cbHMACInner);

                // currently no support for byte reversed keys with HMAC
                for(i=0;i<pTmpKey->cbKeyLen;i++)
                    pb[i] ^= (pTmpKey->pKeyValue)[i];

                if (!LocalHashData(pTmpHash->HMACAlgid, pTmpHash->pHashData, pb, cb))
                {
                    goto Ret;
                }

                pTmpHash->HMACState |= HMAC_STARTED;
            }

            if (!LocalHashData(pTmpHash->HMACAlgid, pTmpHash->pHashData,
                               (BYTE*)pbData, dwDataLen))        
            {
                goto Ret;
            }

            break;
        }
*/
        default:
            if (!LocalHashData(pTmpHash->Algid, pTmpHash->pHashData, (BYTE*)pbData, dwDataLen))
            {
                goto Ret;
            }
    }

    pTmpHash->dwHashState |= DATA_IN_HASH;

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputHashData(FALSE, fRet, 0, 0, NULL, 0, 0);
#endif // DBG -- NOTE:  This section not compiled for retail builds
/*
    if (pbJunk)
        _nt_free(pbJunk, dwTmpLen);
    if (pb)
         _nt_free(pb, cb);
*/
    return fRet;
}

BOOL SetupKeyToBeHashed(
                        PNTAGHashList pHash,
                        PNTAGKeyList pKey,
                        BYTE **ppbData,
                        DWORD *pcbData,
                        DWORD dwFlags)
{
    DWORD   cb;
    DWORD   i;
    BOOL    fRet = FALSE;
    
    *ppbData = NULL;
    cb = pKey->cbKeyLen;

    if (NULL == (*ppbData = (BYTE *)_nt_malloc(cb)))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    if (CRYPT_LITTLE_ENDIAN & dwFlags)
    {
        memcpy(*ppbData, pKey->pKeyValue, cb);
    }
    else
    {
        // Reverse the session key bytes
        for (i = 0; i < cb; i++)
            (*ppbData)[i] = (pKey->pKeyValue)[cb - i - 1];
    }

    *pcbData = cb;

    fRet = TRUE;
Ret:
    return fRet;
}

/*
 -      CPHashSessionKey
 -
 *      Purpose:
 *                Compute the cryptograghic hash on a key object.
 *
 *
 *      Parameters:
 *               IN  hUID      -  Handle to the user identifcation
 *               IN  hHash     -  Handle to hash object
 *               IN  hKey      -  Handle to a key object
 *               IN  dwFlags   -  Flags values
 *
 *      Returns:
 *               CRYPT_FAILED
 *               CRYPT_SUCCEED
 */
BOOL CPHashSessionKey(IN HCRYPTPROV hUID,
                      IN HCRYPTHASH hHash,
                      IN HCRYPTKEY hKey,
                      IN DWORD dwFlags)
{
    PNTAGHashList       pTmpHash;
    PNTAGKeyList        pTmpKey;
    PNTAGUserList       pTmpUser;
    BSAFE_PUB_KEY       *pBsafePubKey;
    DWORD               dwDataLen;
    BYTE                *pbData = NULL;
    DWORD               BytePos;
    BOOL                f;
    BYTE                *pScratch;
    DWORD               z;
#ifdef CSP_USE_SSL3
    PSCH_KEY            pSChKey;
#endif // CSP_USE_SSL3
    BOOL                fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputHashSessionKey(TRUE, FALSE, hUID, hHash, hKey, dwFlags);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (dwFlags & ~(CRYPT_LITTLE_ENDIAN))
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

    if ((pTmpHash = (PNTAGHashList) NTLValidate(hHash, hUID,
                                HASH_HANDLE)) == NULL)
    {
        if (GetLastError() == NTE_FAIL)
        {
            SetLastError((DWORD) NTE_BAD_HASH);
        }

        goto Ret;
    }

    if (pTmpHash->HashFlags & HF_VALUE_SET)
    {
        SetLastError((DWORD) NTE_BAD_HASH_STATE);
        goto Ret;
    }

    if ((pTmpKey = (PNTAGKeyList) NTLValidate((HNTAG)hKey, hUID, KEY_HANDLE))
                == NULL)
    {
        if (GetLastError() == NTE_FAIL)
        {
            SetLastError((DWORD) NTE_BAD_KEY);
        }

        goto Ret;
    }

    if (!FIsLegalKey(pTmpUser, pTmpKey, FALSE))
    {
        SetLastError((DWORD) NTE_BAD_KEY);
        goto Ret;
    }

#ifdef CSP_USE_SSL3
    if ((CALG_SSL3_MASTER == pTmpKey->Algid) ||
        (CALG_TLS1_MASTER == pTmpKey->Algid) ||
        (CALG_PCT1_MASTER == pTmpKey->Algid))
    {
        if (NULL == pTmpKey->pData)
        {
            SetLastError((DWORD)NTE_BAD_KEY);
            goto Ret;
        }
        pSChKey = (PSCH_KEY)pTmpKey->pData;
        if (FALSE == pSChKey->fFinished)
        {
            SetLastError((DWORD)NTE_BAD_KEY);
            goto Ret;
        }
    }
#endif // CSP_USE_SSL3

#if 0
    // Check if we should do an auto-inflate
    if (pTmpKey->pData == NULL)
    {
        if (NTAG_FAILED(CPInflateKey(pTmpKey)))
        {
            goto Ret;
        }
    }
#endif

#ifdef CSP_USE_DES
    if ((CALG_DES == pTmpKey->Algid) || (CALG_3DES == pTmpKey->Algid) ||
        (CALG_3DES_112 == pTmpKey->Algid))
    {
        if (PROV_RSA_SCHANNEL != pTmpUser->dwProvType)
        {
#ifdef STRONG
            if (pTmpUser->fNewStrongCSP ||
                (!(pTmpUser->Rights & CRYPT_DES_HASHKEY_BACKWARDS)))
#endif // STRONG
            {
                desparityonkey(pTmpKey->pKeyValue, pTmpKey->cbKeyLen);
            }
        }
    }
#endif

    if (!SetupKeyToBeHashed(pTmpHash, pTmpKey, &pbData, &dwDataLen, dwFlags))
        goto Ret;

    switch (pTmpHash->Algid)
    {
#ifdef CSP_USE_MD2
        case CALG_MD2:
        {
            MD2_object      *pMD2Hash;

            pMD2Hash = (MD2_object *) pTmpHash->pHashData;

            // make sure the hash is updatable
            if (pMD2Hash->FinishFlag == TRUE)
            {
                SetLastError((DWORD) NTE_BAD_HASH_STATE);
                goto Ret;
            }

            f = MD2Update(&pMD2Hash->MD, pbData, dwDataLen);

            if (f != 0)
            {
                SetLastError((DWORD) NTE_FAIL);
                goto Ret;
            }

            break;
        }
#endif

#ifdef CSP_USE_MD4
        case CALG_MD4:
        {
            MD4_object      *pMD4Hash;
                        
            pMD4Hash = (MD4_object *) pTmpHash->pHashData;

            // make sure the hash is updatable
            if (pMD4Hash->FinishFlag == TRUE)
            {
                SetLastError((DWORD) NTE_BAD_HASH_STATE);
                goto Ret;
            }

            for (;;)
            {
                // check if there's plenty of room in the buffer
                if ((pMD4Hash->BufLen + dwDataLen) < MD4BLOCKSIZE)
                {
                    // just append to whatever's already
                    memcpy (pMD4Hash->Buf + pMD4Hash->BufLen, pbData,
                            dwDataLen);

                    // set of the trailing buffer length field
                    pMD4Hash->BufLen += (BYTE) dwDataLen;

                    break;
                }

                // determine what we need to fill the buffer, then do it.
                BytePos = MD4BLOCKSIZE - pMD4Hash->BufLen;
                memcpy (pMD4Hash->Buf + pMD4Hash->BufLen, pbData, BytePos);

                // The buffer is now full, process it.
                f = MDupdate(&pMD4Hash->MD, pMD4Hash->Buf,
                             MD4BYTESTOBITS(MD4BLOCKSIZE));
                
                if (f != MD4_SUCCESS)
                {
                    SetLastError((DWORD) NTE_FAIL);
                    goto Ret;
                }

                // now it's empty.
                pMD4Hash->BufLen = 0;

                // we processed some bytes, so reflect that and try again
                dwDataLen -= BytePos;

                if (dwDataLen == 0)
                {
                    break;
                }
            }
            break;
        }
#endif

#ifdef CSP_USE_MD5
        case CALG_MD5:
        {
            MD5_object  *pMD5Hash;

            pMD5Hash = (MD5_object *) pTmpHash->pHashData;

            // make sure the hash is updatable
            if (pMD5Hash->FinishFlag == TRUE)
            {
                SetLastError((DWORD) NTE_BAD_HASH_STATE);
                goto Ret;
            }

            MD5Update(pMD5Hash, pbData, dwDataLen);

            break;
        }
#endif

#ifdef CSP_USE_SHA
        case CALG_SHA:
        {
            A_SHA_CTX   *pSHAHash;

            pSHAHash = (A_SHA_CTX *)pTmpHash->pHashData;

            // make sure the hash is updatable
            if (pSHAHash->FinishFlag == TRUE)
            {
                SetLastError((DWORD) NTE_BAD_HASH_STATE);
                goto Ret;
            }

            A_SHAUpdate(pSHAHash, (BYTE *) pbData, dwDataLen);

            break;
        }
#endif

#ifdef CSP_USE_MAC
        case CALG_MAC:
#endif // CSP_USE_MAC
        case CALG_HMAC:
            if (!LocalMACData(hUID, pTmpHash, pbData, dwDataLen))
            {
                goto Ret;
            }
            break;

        default:
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
    }

    pTmpHash->dwHashState |= DATA_IN_HASH;

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputHashSessionKey(FALSE, fRet, 0, 0, 0, 0);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (pbData)
        _nt_free(pbData, dwDataLen);

    return fRet;
}


void FreeHash(IN PNTAGHashList pHash)
{
    if (pHash)
    {
        if (pHash->pHashData)
            _nt_free(pHash->pHashData, pHash->dwDataLen);
        if (pHash->pbHMACInner)
            _nt_free(pHash->pbHMACInner, pHash->cbHMACInner);
        if (pHash->pbHMACOuter)
            _nt_free(pHash->pbHMACOuter, pHash->cbHMACOuter);
        _nt_free(pHash, sizeof(NTAGHashList));
    }
}

/*
-   CPDestroyHash
-
*   Purpose:
*                Destory the hash object
*
*
*   Parameters:
*               IN  hUID      -  Handle to the user identifcation
*               IN  hHash     -  Handle to hash object
*
*   Returns:
*/
BOOL CPDestroyHash(IN HCRYPTPROV hUID,
                   IN HCRYPTHASH hHash)
{
    PNTAGHashList   pTmpHash;
    BOOL            fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputDestroyHash(TRUE, FALSE, hUID, hHash);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    // check the user identification
    if (NTLCheckList (hUID, USER_HANDLE) == NULL)
    {
        SetLastError((DWORD) NTE_BAD_UID);
        goto Ret;
    }

    if ((pTmpHash = (PNTAGHashList) NTLValidate(hHash, hUID,
                                                HASH_HANDLE)) == NULL)
    {
        // NTLValidate doesn't know what error to set
        // so it set NTE_FAIL -- fix it up.
        if (GetLastError() == NTE_FAIL)
        {
            SetLastError((DWORD) NTE_BAD_HASH);
        }
        goto Ret;
    }

    switch (pTmpHash->Algid)
    {

#ifdef CSP_USE_MD2
            
        case CALG_MD2:

#endif

#ifdef CSP_USE_MD4
            
        case CALG_MD4:

#endif

#ifdef CSP_USE_MD5

        case CALG_MD5:

#endif

#ifdef CSP_USE_SHA

        case CALG_SHA:

#endif

#ifdef CSP_USE_SSL3SHAMD5

        case CALG_SSL3_SHAMD5:

#endif

#ifdef CSP_USE_MAC

        case CALG_MAC:
        case CALG_HMAC:

#endif
               
#ifdef CSP_USE_SSL3
        case CALG_SCHANNEL_MASTER_HASH:
        case CALG_TLS1PRF:
#endif

            if (CALG_SCHANNEL_MASTER_HASH == pTmpHash->Algid)
            {
                FreeSChHash((PSCH_HASH)pTmpHash->pHashData);
            }
            memnuke(pTmpHash->pHashData, pTmpHash->dwDataLen);
            break;

        default:
            SetLastError((DWORD) NTE_BAD_ALGID);
            goto Ret;
    }

    // Remove from internal list first so others can't get to it, then free.
    NTLDelete(hHash);
    FreeHash(pTmpHash);

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputDestroyHash(FALSE, fRet, 0, 0);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    return fRet;

}

BOOL CopyHash(
              IN PNTAGHashList pOldHash,
              OUT PNTAGHashList *ppNewHash)
{
    PNTAGHashList   pNewHash;
    BOOL            fRet = FALSE;

    if (NULL == (pNewHash = (PNTAGHashList)_nt_malloc(sizeof(NTAGHashList))))
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        goto Ret;
    }

    memcpy(pNewHash, pOldHash, sizeof(NTAGHashList));
    pNewHash->dwDataLen = pOldHash->dwDataLen;
    if (pNewHash->dwDataLen)
    {
        if (NULL == (pNewHash->pHashData = (BYTE*)_nt_malloc(pNewHash->dwDataLen)))
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            goto Ret;
        }
    }

    memcpy(pNewHash->pHashData, pOldHash->pHashData, pNewHash->dwDataLen);

    *ppNewHash = pNewHash;

    fRet = TRUE;
Ret:
    if (FALSE == fRet)
        FreeHash(pNewHash);
    return fRet;
}

/*
 -  CPDuplicateHash
 -
 *  Purpose:
 *                Duplicates the state of a hash and returns a handle to it
 *
 *  Parameters:
 *               IN      hUID           -  Handle to a CSP
 *               IN      hHash          -  Handle to a hash
 *               IN      pdwReserved    -  Reserved
 *               IN      dwFlags        -  Flags
 *               IN      phHash         -  Handle to the new hash
 *
 *  Returns:
 */
BOOL CPDuplicateHash(IN HCRYPTPROV hUID,
                    IN HCRYPTHASH hHash,
                    IN DWORD *pdwReserved,
                    IN DWORD dwFlags,
                    IN HCRYPTHASH *phHash)
{
    PNTAGHashList   pTmpHash;
    PNTAGHashList   pNewHash = NULL;
    BOOL            fRet = NTF_FAILED;

#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputDuplicateHash(TRUE, FALSE, hUID, hHash, pdwReserved,
                                dwFlags, phHash);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (NULL != pdwReserved)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        goto Ret;
    }

    if (0 != dwFlags)
    {
        SetLastError((DWORD)NTE_BAD_FLAGS);
        goto Ret;
    }

    if (NULL == (pTmpHash = (PNTAGHashList)NTLValidate((HNTAG)hHash, hUID, HASH_HANDLE)))
    {
        SetLastError((DWORD)NTE_BAD_HASH);
        goto Ret;
    }

    if (!CopyHash(pTmpHash, &pNewHash))
        goto Ret;          // error already set

            
    if (NTF_FAILED == NTLMakeItem(phHash, HASH_HANDLE, (void *)pNewHash))
        goto Ret;          // error already set

    fRet = NTF_SUCCEED;
Ret:
#if DBG         // NOTE:  This section not compiled for retail builds
    CSPDebugOutputDuplicateHash(FALSE, fRet, 0, 0, NULL, 0, NULL);
#endif // DBG -- NOTE:  This section not compiled for retail builds

    if (NTF_FAILED == fRet)
        FreeHash(pNewHash);
    return fRet;
}

