/*-----------------------------------------------------------------------------
* Copyright (C) Microsoft Corporation, 1995 - 1996.
* All rights reserved.
*
*	Owner 			:ramas
*	Date			:4/16/96
*	description		: Main Crypto functions for SSL3
*----------------------------------------------------------------------------*/

#include <spbase.h>
#include <ssl3key.h>
#include <ssl2msg.h>
#include <ssl3msg.h>
#include <ssl2prot.h>
#include <md5.h>
#include <sha.h>
#include <rsa.h>


#define CB_SSL3_MAX_MAC_PAD 48
#define CB_SSL3_MD5_MAC_PAD 48
#define CB_SSL3_SHA_MAC_PAD 40

#define PAD1_CONSTANT 0x36
#define PAD2_CONSTANT 0x5c

void Ssl3BuildMasterKeys(PSPContext pContext, PUCHAR pbPreMaster)
{

    WORD i;
    PBYTE sz[3] = { "A","BB","CCC" } ;
    MD5_CTX Md5Hash;
    A_SHA_CTX ShaHash;
    BYTE bShaHashValue[A_SHA_DIGEST_LEN];


    //UNDONE DO WE NEED THESE MEMSETs
    // zero buffers
#if DBG
    DebugLog((DEB_TRACE, "  Pre Master Secret\n"));
    DbgDumpHexString(pbPreMaster, CB_SSL3_PRE_MASTER_SECRET);

#endif





    for ( i = 0 ; i < 3 ; i++)
    {
        // SHA('A' or 'BB' or 'CCC' + pre_master_secret + ClientHello.random + ServerHello.random)
        A_SHAInit(&ShaHash);
        A_SHAUpdate(&ShaHash, sz[i], i + 1);
        A_SHAUpdate(&ShaHash, pbPreMaster, CB_SSL3_PRE_MASTER_SECRET);
        A_SHAUpdate(&ShaHash, pContext->rgbS3CRandom, CB_SSL3_RANDOM);
        A_SHAUpdate(&ShaHash, pContext->rgbS3SRandom, CB_SSL3_RANDOM);
        A_SHAFinal(&ShaHash, bShaHashValue);

        // MD5(pre_master_secret + SHA-hash)
        MD5Init(&Md5Hash);
        MD5Update(&Md5Hash, pbPreMaster, CB_SSL3_PRE_MASTER_SECRET);
        MD5Update(&Md5Hash, bShaHashValue, A_SHA_DIGEST_LEN);
        MD5Final(&Md5Hash);
        CopyMemory(pContext->RipeZombie->pMasterKey + (i * MD5DIGESTLEN), Md5Hash.digest, MD5DIGESTLEN);
    }

    pContext->RipeZombie->cbMasterKey = 3*MD5DIGESTLEN;

#if DBG
    DebugLog((DEB_TRACE, "  Master Secret\n"));
    DbgDumpHexString(pContext->RipeZombie->pMasterKey, pContext->RipeZombie->cbMasterKey);

#endif


}


SP_STATUS
Ssl3MakeMasterKeyBlock(PSPContext pContext)
{

    MD5_CTX 	Md5Hash;
    A_SHA_CTX 	ShaHash;
    BYTE rgbShaHashValue[A_SHA_DIGEST_LEN];
    PBYTE sz[5] = { "A","BB","CCC","DDDD","EEEEE" };
    DWORD ib;


    //pContext->RipeZombe->pMasterKey containst the master secret.

#if DBG
    DebugLog((DEB_TRACE, "  Master Secret\n"));
    DbgDumpHexString(pContext->RipeZombie->pMasterKey, pContext->RipeZombie->cbMasterKey);

#endif

    for(ib=0 ; ib<5 ; ib++)
    {
        // SHA('A' + master_secret + ServerHello.random + ClientHello.random )
        A_SHAInit  (&ShaHash);
        A_SHAUpdate(&ShaHash, sz[ib], ib + 1);
        A_SHAUpdate(&ShaHash, pContext->RipeZombie->pMasterKey, pContext->RipeZombie->cbMasterKey);
        A_SHAUpdate(&ShaHash, pContext->rgbS3SRandom, CB_SSL3_RANDOM);
        A_SHAUpdate(&ShaHash, pContext->rgbS3CRandom, CB_SSL3_RANDOM);
        A_SHAFinal (&ShaHash, rgbShaHashValue);

        // MD5(master_secret + SHA-hash)
        MD5Init  (&Md5Hash);
        MD5Update(&Md5Hash, pContext->RipeZombie->pMasterKey, pContext->RipeZombie->cbMasterKey);
        MD5Update(&Md5Hash, rgbShaHashValue, A_SHA_DIGEST_LEN);
        MD5Final (&Md5Hash);
        CopyMemory(pContext->Ssl3MasterKeyBlock + ib * MD5DIGESTLEN, Md5Hash.digest, MD5DIGESTLEN);
	}
 #if DBG
    DebugLog((DEB_TRACE, "  Master Key Block\n"));
    DbgDumpHexString(pContext->Ssl3MasterKeyBlock, MD5DIGESTLEN*5);

#endif
   return( PCT_ERR_OK );
}

SP_STATUS
Ssl3MakeWriteSessionKeys(PSPContext pContext)
{

//    PCheckSumBuffer     CWriteHash;
//    HashBuf             CWriteHB;
    MD5_CTX 	        Md5Hash;
//    A_SHA_CTX 	        ShaHash;

//    BYTE rgbShaHashValue[A_SHA_DIGEST_LEN];
    BYTE rgbWriteKey[SSL3_MASTER_KEY_SIZE];
    BYTE rgbPad[CB_SSL3_MAX_MAC_PAD];
    WORD cbPad = CB_SSL3_MD5_MAC_PAD;
//    DWORD ib;
    BOOL fSHA = FALSE;
    DWORD dwKeyLen;
    BOOL fClient;
    DWORD dwMACOffset;
    DWORD dwKEYOffset;
    DWORD dwIVOffset;



    fClient = (0 != (pContext->RipeZombie->fProtocol & SP_PROT_SSL3_CLIENT));

    /* find bit strength of cipher */
    dwKeyLen = (pContext->RipeZombie->SessCiphSpec & SP_CIPHER_STRENGTH) >> SP_CSTR_POS;
    /* convert to bytes */
    dwKeyLen = dwKeyLen / 8;

    dwMACOffset = (fClient?0:pContext->pCheck->cbCheckSum);
    dwKEYOffset = 2*pContext->pCheck->cbCheckSum + (fClient?0:dwKeyLen);
    dwIVOffset = 2*pContext->pCheck->cbCheckSum + 2*dwKeyLen; // + IV Size;

    //
    // extract keys from key block
    //




    pContext->KeySize = SSL3_MASTER_KEY_SIZE;

    // Initialize the Write Key and Write State
    CopyMemory(rgbWriteKey, pContext->Ssl3MasterKeyBlock + dwKEYOffset, dwKeyLen);

    // If key is exportable, perform export calculations
    if(dwKeyLen != SSL3_MASTER_KEY_SIZE)
    {
        MD5Init  (&Md5Hash);
        MD5Update(&Md5Hash, rgbWriteKey, dwKeyLen);
        if(fClient)
        {
            MD5Update(&Md5Hash, pContext->rgbS3CRandom, CB_SSL3_RANDOM);
            MD5Update(&Md5Hash, pContext->rgbS3SRandom, CB_SSL3_RANDOM);
        }
        else
        {
            MD5Update(&Md5Hash, pContext->rgbS3SRandom, CB_SSL3_RANDOM);
            MD5Update(&Md5Hash, pContext->rgbS3CRandom, CB_SSL3_RANDOM);
        }
        MD5Final (&Md5Hash);
        CopyMemory(rgbWriteKey, Md5Hash.digest, pContext->KeySize);
    }
#if DBG
    DebugLog((DEB_TRACE, "  Write Key\n"));
    DbgDumpHexString(rgbWriteKey, pContext->KeySize);

#endif

    if (!pContext->pSystem->Initialize(  rgbWriteKey,
                                        pContext->KeySize,
                                        &pContext->pWriteState))
    {
            return SP_LOG_RESULT( PCT_INT_INTERNAL_ERROR );
    }

    // Initialize the MAC Key and Mac State.
    CopyMemory(pContext->WriteMACKey,
               pContext->Ssl3MasterKeyBlock + dwMACOffset,
               pContext->pCheck->cbCheckSum);
#if DBG
    DebugLog((DEB_TRACE, "  Write MAC Key\n"));
    DbgDumpHexString(pContext->WriteMACKey, pContext->pCheck->cbCheckSum);

#endif

    // Precalc the two states needed to do an SSL3 mac.
    InitHashBuf(pContext->WrMACBuf, pContext);
    InitHashBuf(pContext->WrMACBuf1, pContext);

    pContext->WriteMACState = (PCheckSumBuffer)pContext->WrMACBuf;
    pContext->WriteMACState1 = (PCheckSumBuffer)pContext->WrMACBuf1;

    // Precalc hash(MAC_write_secret + pad_1)

    pContext->pCheck->Sum(  pContext->WriteMACState,
                            pContext->pCheck->cbCheckSum,
                            pContext->WriteMACKey);

    cbPad = (fSHA?CB_SSL3_SHA_MAC_PAD:CB_SSL3_MD5_MAC_PAD);
    FillMemory(rgbPad, cbPad, PAD1_CONSTANT);

    pContext->pCheck->Sum(  pContext->WriteMACState,
                            cbPad,
                            rgbPad);

    // Precalc hash(MAC_write_secret + pad_2)

    pContext->pCheck->Sum(  pContext->WriteMACState1,
                            pContext->pCheck->cbCheckSum,
                            pContext->WriteMACKey);

    FillMemory(rgbPad, cbPad, PAD2_CONSTANT);

    pContext->pCheck->Sum(  pContext->WriteMACState1,
                            cbPad,
                            rgbPad);

    DebugLog((DEB_TRACE, "Write Keys are Computed\n"));
    return (PCT_ERR_OK);
}

SP_STATUS
Ssl3MakeReadSessionKeys(PSPContext pContext)
{

//    PCheckSumBuffer     CWriteHash;
//    HashBuf             CWriteHB;
    MD5_CTX 	        Md5Hash;
//    A_SHA_CTX 	        ShaHash;

//    BYTE rgbShaHashValue[A_SHA_DIGEST_LEN];
    BYTE rgbReadKey[SSL3_MASTER_KEY_SIZE];
    BYTE rgbPad[CB_SSL3_MAX_MAC_PAD];
    WORD cbPad = CB_SSL3_MD5_MAC_PAD;
//    DWORD ib;
    BOOL fSHA = FALSE;
    DWORD dwKeyLen;
    BOOL fClient;
    DWORD dwMACOffset;
    DWORD dwKEYOffset;
    DWORD dwIVOffset;



    fClient = (0 != (pContext->RipeZombie->fProtocol & SP_PROT_SSL3_CLIENT));

    /* find bit strength of cipher */
    dwKeyLen = (pContext->RipeZombie->SessCiphSpec & SP_CIPHER_STRENGTH) >> SP_CSTR_POS;
    /* convert to bytes */
    dwKeyLen = dwKeyLen / 8;

    dwMACOffset = (fClient?pContext->pCheck->cbCheckSum:0);
    dwKEYOffset = 2*pContext->pCheck->cbCheckSum + (fClient?dwKeyLen:0);
    dwIVOffset = 2*pContext->pCheck->cbCheckSum + 2*dwKeyLen; // + IV Size;

    //
    // extract keys from key block
    //

    pContext->KeySize = SSL3_MASTER_KEY_SIZE;

    // Initialize the Write Key and Write State
    CopyMemory(rgbReadKey, pContext->Ssl3MasterKeyBlock + dwKEYOffset, dwKeyLen);

    // If key is exportable, perform export calculations
    if(dwKeyLen != SSL3_MASTER_KEY_SIZE)
    {
        MD5Init  (&Md5Hash);
        MD5Update(&Md5Hash, rgbReadKey, dwKeyLen);
        if(fClient)
        {
            MD5Update(&Md5Hash, pContext->rgbS3SRandom, CB_SSL3_RANDOM);
            MD5Update(&Md5Hash, pContext->rgbS3CRandom, CB_SSL3_RANDOM);
        }
        else
        {
            MD5Update(&Md5Hash, pContext->rgbS3CRandom, CB_SSL3_RANDOM);
            MD5Update(&Md5Hash, pContext->rgbS3SRandom, CB_SSL3_RANDOM);
       }
        MD5Final (&Md5Hash);
        CopyMemory(rgbReadKey, Md5Hash.digest, pContext->KeySize);
    }
#if DBG
    DebugLog((DEB_TRACE, "  Read Key\n"));
    DbgDumpHexString(rgbReadKey,  pContext->KeySize);

#endif

    if (!pContext->pSystem->Initialize(  rgbReadKey,
                                        pContext->KeySize,
                                        &pContext->pReadState))
    {
            return SP_LOG_RESULT( PCT_INT_INTERNAL_ERROR );
    }

    // Initialize the MAC Key and Mac State.
    CopyMemory(pContext->ReadMACKey,
               pContext->Ssl3MasterKeyBlock + dwMACOffset,
               pContext->pCheck->cbCheckSum);
#if DBG
    DebugLog((DEB_TRACE, "  Read MAC Key\n"));
    DbgDumpHexString(pContext->ReadMACKey, pContext->pCheck->cbCheckSum);

#endif

    // Precalc the two states needed to do an SSL3 mac.
    InitHashBuf(pContext->RdMACBuf, pContext);
    InitHashBuf(pContext->RdMACBuf1, pContext);

    pContext->ReadMACState = (PCheckSumBuffer)pContext->RdMACBuf;
    pContext->ReadMACState1 = (PCheckSumBuffer)pContext->RdMACBuf1;

    // Precalc hash(MAC_read_secret + pad_1)

    pContext->pCheck->Sum(  pContext->ReadMACState,
                            pContext->pCheck->cbCheckSum,
                            pContext->ReadMACKey);

    cbPad = (fSHA?CB_SSL3_SHA_MAC_PAD:CB_SSL3_MD5_MAC_PAD);
    FillMemory(rgbPad, cbPad, PAD1_CONSTANT);

    pContext->pCheck->Sum(  pContext->ReadMACState,
                            cbPad,
                            rgbPad);

    // Precalc hash(MAC_read_secret + pad_2)

    pContext->pCheck->Sum(  pContext->ReadMACState1,
                            pContext->pCheck->cbCheckSum,
                            pContext->ReadMACKey);

    FillMemory(rgbPad, cbPad, PAD2_CONSTANT);

    pContext->pCheck->Sum(  pContext->ReadMACState1,
                            cbPad,
                            rgbPad);

    DebugLog((DEB_TRACE, "Read Keys are Computed\n"));
    return (PCT_ERR_OK);
}
