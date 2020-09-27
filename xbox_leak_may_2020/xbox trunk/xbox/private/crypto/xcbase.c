/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    xcbase.c

Abstract:

    Contains implementation of base level crypto function

--*/

#include "umkm.h"
#include <windows.h>
#include <assert.h>
#include <rsa.h>
#include <sha.h>
#include <rc4.h>
#include <xcrypt.h>
#include <benaloh.h>

#include "des.h"
#include "tripldes.h"
#include "modes.h"

//
// Reverse ASN.1 Encodings of possible hash identifiers.  
//
static PBYTE shaEncodings[] = {
            //      1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18
            "\x0f\x14\x04\x00\x05\x1a\x02\x03\x0e\x2b\x05\x06\x09\x30\x21\x30",
            "\x0d\x14\x04\x1a\x02\x03\x0e\x2b\x05\x06\x07\x30\x1f\x30",
            "\x00" };

VOID
XCCalcDigest(
    IN  PBYTE   pbMsgData,
    IN  DWORD   dwMsgDataLen,
    IN  PBYTE   pbDigest
    )
/*++

Routine Description:

    This function calculates a digest (hash) for the given daata

Arguments:

Return Value:

    None

--*/
{
    A_SHA_CTX   SHAHash;
    BYTE        abSHADigest[A_SHA_DIGEST_LEN];

    A_SHAInit(&SHAHash);
    A_SHAUpdate(&SHAHash, (PBYTE)&dwMsgDataLen, sizeof(DWORD));
    A_SHAUpdate(&SHAHash, pbMsgData, dwMsgDataLen);
    A_SHAFinal(&SHAHash, abSHADigest);

    memcpy(pbDigest, abSHADigest, A_SHA_DIGEST_LEN);
}



VOID
XCSymmetricEncDec(
    IN PBYTE  pbMsgData,
    IN DWORD  dwMsgDataLen,
    IN PBYTE  pbKey,
    IN DWORD  dwKeyBytes
    )
/*++

Routine Description:

    Encrypt or decrypt the given data buffer in-place using RC4 which is symmetric algorithm.

Arguments:


Return Value:

    None

--*/
{
    struct RC4_KEYSTRUCT rc4KeyCtl;

    rc4_key(&rc4KeyCtl, (UINT)dwKeyBytes, pbKey);

    rc4(&rc4KeyCtl, (UINT)dwMsgDataLen, pbMsgData);
}



VOID
XCApplyPKCS1SigningFmt(
    IN  PBYTE pbKey,
    IN  PBYTE pbDigest,
    OUT PBYTE pbPKCS1Format
    )
/*++

Routine Description:

    Formats a buffer with PKCS 1 for signing for SHA digests

Arguments:


Return Value:

    None

--*/
{
    PBYTE   pbStart;
    PBYTE   pbEnd;
    BYTE    bTmp;
    DWORD   i;
    LPBSAFE_PUB_KEY pPubKey = (LPBSAFE_PUB_KEY)pbKey;

    // 
    // insert the block type
    //
    pbPKCS1Format[pPubKey->datalen - 1] = 0x01;

    // 
    // insert the type I padding
    //
    memset(pbPKCS1Format, 0xff, pPubKey->datalen - 1);

    // 
    // reverse it
    //
    for (i = 0; i < A_SHA_DIGEST_LEN; i++) {
        pbPKCS1Format[i] = pbDigest[A_SHA_DIGEST_LEN - (i + 1)];
    }

    // 
    // PKCS delimit the hash value
    //
    pbEnd = (LPBYTE)shaEncodings[0];
    pbStart = pbPKCS1Format + A_SHA_DIGEST_LEN;
    bTmp = *pbEnd++;
    while (0 < bTmp--) {
        *pbStart++ = *pbEnd++;
    }
    *pbStart++ = 0;
}


BOOL XCVerifyPKCS1SigningFmt(
    IN  BSAFE_PUB_KEY* pKey,
    IN  BYTE* pbDigest,
    IN  BYTE* pbPKCS1Format
    )
{
    BYTE**    rgEncOptions;
    BYTE      rgbTmpHash[A_SHA_DIGEST_LEN + 16];
    DWORD     i;
    DWORD     cb;
    BYTE*     pbStart;
    DWORD     cbTmp;

    rgEncOptions = shaEncodings;

    // 
    // reverse the hash to match the signature.
    //
    for (i = 0; i < A_SHA_DIGEST_LEN; i++) {
        rgbTmpHash[i] = pbDigest[A_SHA_DIGEST_LEN - (i + 1)];
    }

    // 
    // see if it matches.
    //
    if (memcmp(rgbTmpHash, pbPKCS1Format, A_SHA_DIGEST_LEN)) {
        return FALSE;
    }

    cb = A_SHA_DIGEST_LEN;

    //
    // check for any signature type identifiers
    //
    for (i = 0; 0 != *rgEncOptions[i]; i += 1) {
        pbStart = (LPBYTE)rgEncOptions[i];
        cbTmp = *pbStart++;
        if (0 == memcmp(&pbPKCS1Format[cb], pbStart, cbTmp)) {
            // adjust the end of the hash data. 
            cb += cbTmp;   
            break;
        }
    }

    // 
    // check to make sure the rest of the PKCS #1 padding is correct
    //
    if ((0x00 != pbPKCS1Format[cb]) || (0x00 != pbPKCS1Format[pKey->datalen]) ||
         (0x1 != pbPKCS1Format[pKey->datalen - 1])) {
        return FALSE;
    }

    for (i = cb + 1; i < (DWORD)pKey->datalen - 1; i++) {
        if (0xff != pbPKCS1Format[i]) {
            return FALSE;
        }
    }

    return TRUE;
}



DWORD
XCCalcSigSize(
    IN  PBYTE  pbPrivateKey
    )
/*++

Routine Description:

    Calculates size of the signature based on the private key

Arguments:

Return Value:

    Size of the signature

--*/
{
    LPBSAFE_PRV_KEY pPrvKey = (LPBSAFE_PRV_KEY)pbPrivateKey;

    return (pPrvKey->bitlen + 7) / 8;
}


DWORD
XCCalcKeyLen(
    IN  PBYTE  pbPublicKey
    )
/*++

Routine Description:

    Calculates the keylen stored in the key

Arguments:

Return Value:

    Returns the value of keylen 

--*/
{
    LPBSAFE_PUB_KEY pPubKey = (LPBSAFE_PUB_KEY)pbPublicKey;

    return pPubKey->keylen;
}




BOOLEAN
XCSignDigest(
    IN   PBYTE    pbDigest,
    IN   PBYTE    pbPrivateKey,
    OUT  PBYTE    pbSig
    )
/*++

Routine Description:

    This function signs a digest (hash) with the given private key

Arguments:

Return Value:

    TRUE if successful, FALSE otherwise

--*/
{
    PBYTE           pbInput;
    PBYTE           pbOutput;
    LPBSAFE_PRV_KEY pPrvKey = (LPBSAFE_PRV_KEY)pbPrivateKey;
    DWORD           dwSigLen;

    dwSigLen = (pPrvKey->bitlen + 7) / 8;

    pbInput = ALLOC(pPrvKey->keylen);
    pbOutput = ALLOC(pPrvKey->keylen);

    memset(pbInput, 0, pPrvKey->keylen);
    memset(pbOutput, 0, pPrvKey->keylen);

    // 
    // initialize the input buffer to PKCS 1 signing format 
    //
    XCApplyPKCS1SigningFmt(pbPrivateKey, pbDigest, pbInput);

    //
    // encrypt it
    //
    BSafeDecPrivate(pPrvKey, pbInput, pbOutput);

    //
    // copy results
    //
    memcpy(pbSig, pbOutput, dwSigLen);

    FREE(pbOutput);
    FREE(pbInput);

    return TRUE;
}



BOOLEAN
XCVerifyDigest(
    IN   PBYTE   pbSig,
    IN   PBYTE   pbPublicKey,
    IN   PBYTE   pbWorkspace,
    IN   PBYTE   pbCompareDigest
    )
/*++

Routine Description:

    This function decrypts and verifies a digest (hash) with the given public key

Arguments:

    pbSig - Supplies the encrypted signature

    pbPublicKey - Supplies a pointer to the public key data

    pbWorkspace - Supplies a buffer to be used as workspace.  This
                  must be at least 2 * keylen

    pbCompareDigest - Supplies the digest to compare 


Return Value:

    TRUE if signature matches, FALSE otherwise

--*/
{
    LPBSAFE_PUB_KEY pPubKey = (LPBSAFE_PUB_KEY)pbPublicKey;
    PBYTE           pbOutput;
    PBYTE           pbInput;
    DWORD           dwSigLen;

    dwSigLen = (pPubKey->bitlen + 7) / 8;

    pbOutput = pbWorkspace;
    pbInput = (PBYTE)((ULONG_PTR)pbWorkspace + pPubKey->keylen);

    memset(pbInput, 0, pPubKey->keylen);
    memcpy(pbInput, pbSig, dwSigLen);

    if (!BSafeEncPublic(pPubKey, pbInput, pbOutput)) {
        return FALSE;
    }


    if (!XCVerifyPKCS1SigningFmt(pPubKey, pbCompareDigest, pbOutput)) {
        return FALSE;
    }

    return TRUE;
}

//
// Crypto APIs the ROM exports
//

void
XCryptSHAInit(
    IN PUCHAR pbSHAContext
    )
{
    A_SHAInit((A_SHA_CTX*)pbSHAContext);
}
   
void
XCryptSHAUpdate(
    IN PUCHAR pbSHAContext,
    IN PUCHAR pbInput,
    IN ULONG dwInputLength
    )
{
    A_SHAUpdate((A_SHA_CTX*)pbSHAContext, pbInput, dwInputLength);
}
   
void
XCryptSHAFinal(
    IN PUCHAR pbSHAContext,
    IN PUCHAR pbDigest
    )
{
    A_SHAFinal((A_SHA_CTX*)pbSHAContext, pbDigest);
}
   
void
XCryptRC4Key(
    IN PUCHAR pbKeyStruct,
    IN ULONG dwKeyLength,
    IN PUCHAR pbKey
    )
{
    rc4_key((RC4_KEYSTRUCT*)pbKeyStruct, dwKeyLength, pbKey);
}
   
void
XCryptRC4Crypt(
    IN PUCHAR pbKeyStruct,
    IN ULONG dwInputLength,
    IN PUCHAR pbInput
    )
{
    rc4((RC4_KEYSTRUCT*)pbKeyStruct, dwInputLength, pbInput);
}
   
void
XCryptHMAC(
    IN PBYTE pbKeyMaterial,
    IN ULONG cbKeyMaterial,
    IN PBYTE pbData,
    IN ULONG cbData,
    IN PBYTE pbData2,
    IN ULONG cbData2,
    OUT PBYTE HmacData // length must be A_SHA_DIGEST_LEN
    )
{
#define HMAC_K_PADSIZE              64
    BYTE Kipad[HMAC_K_PADSIZE];
    BYTE Kopad[HMAC_K_PADSIZE];
    BYTE HMACTmp[HMAC_K_PADSIZE+A_SHA_DIGEST_LEN];
    ULONG dwBlock;
    A_SHA_CTX shaHash;

    // truncate
    if (cbKeyMaterial > HMAC_K_PADSIZE)
        cbKeyMaterial = HMAC_K_PADSIZE;

    RtlZeroMemory(Kipad, HMAC_K_PADSIZE);
    RtlCopyMemory(Kipad, pbKeyMaterial, cbKeyMaterial);

    RtlZeroMemory(Kopad, HMAC_K_PADSIZE);
    RtlCopyMemory(Kopad, pbKeyMaterial, cbKeyMaterial);

    //
    // Kipad, Kopad are padded sMacKey. Now XOR across...
    //
    for(dwBlock=0; dwBlock<HMAC_K_PADSIZE/sizeof(DWORD); dwBlock++)
    {
        ((DWORD*)Kipad)[dwBlock] ^= 0x36363636;
        ((DWORD*)Kopad)[dwBlock] ^= 0x5C5C5C5C;
    }

    //
    // prepend Kipad to data, Hash to get H1
    //

    A_SHAInit(&shaHash);
    A_SHAUpdate(&shaHash, Kipad, HMAC_K_PADSIZE);
    if (cbData != 0)
    {
        A_SHAUpdate(&shaHash, pbData, cbData);
    }
    if (cbData2 != 0)
    {
        A_SHAUpdate(&shaHash, pbData2, cbData2);
    }

    // Finish off the hash
    A_SHAFinal(&shaHash,HMACTmp+HMAC_K_PADSIZE);

    // prepend Kopad to H1, hash to get HMAC
    RtlCopyMemory(HMACTmp, Kopad, HMAC_K_PADSIZE);

    // final hash: output value into passed-in buffer
    A_SHAInit(&shaHash);
    A_SHAUpdate(&shaHash,HMACTmp, sizeof(HMACTmp));
    A_SHAFinal(&shaHash,HmacData);
}

ULONG
XCryptPKEncPublic(
    IN PUCHAR pbPubKey,
    IN PUCHAR pbInput,
    IN PUCHAR pbOutput
    )
{
    return BSafeEncPublic((LPBSAFE_PUB_KEY)pbPubKey, pbInput, pbOutput);
}
   
ULONG
XCryptPKDecPrivate(
    IN PUCHAR pbPrvKey,
    IN PUCHAR pbInput,
    IN PUCHAR pbOutput
    )
{
    return BSafeDecPrivate((LPBSAFE_PRV_KEY)pbPrvKey, pbInput, pbOutput);
}

ULONG
XCryptPKGetKeyLen(
    IN PUCHAR pbPubKey
    )
{
    return XCCalcKeyLen(pbPubKey);
}

BOOLEAN
XCryptVerifyPKCS1Signature(
    IN PUCHAR pbSig,
    IN PUCHAR pbPubKey,
    IN PUCHAR pbDigest
    )
{
    BYTE* pbWorkspace = _alloca( 2 * XCCalcKeyLen(pbPubKey) );
    if ( pbWorkspace == NULL )
    {
        return FALSE;
    }
    return XCVerifyDigest(pbSig, pbPubKey, pbWorkspace, pbDigest);
}
   
// compute A = B ^ C mod D, N = len of params in DWORDs
ULONG
XCryptModExp(
    IN LPDWORD pA,
    IN LPDWORD pB,
    IN LPDWORD pC,
    IN LPDWORD pD,
    IN ULONG dwN
    )
{
    //
    // compute A = B ^ C mod D
    //
    return BenalohModExp(pA, pB, pC, pD, dwN);
}
   
void
XCryptDESKeyParity(
    IN PUCHAR pbKey,
    IN ULONG dwKeyLength
    )
{
    desparityonkey(pbKey, dwKeyLength);
}
   
void
XCryptKeyTable(
    IN ULONG dwCipher,
    OUT PUCHAR pbKeyTable,
    IN PUCHAR pbKey
    )
{
    if (dwCipher == XC_SERVICE_DES_CIPHER)
    {
        deskey((DESTable*)pbKeyTable, pbKey);
    }
    else
    {
        // Assume XC_SERVICE_DES3_CIPHER:
        tripledes3key((PDES3TABLE)pbKeyTable, pbKey);
    }
}
   
void
XCryptBlockCrypt(
    IN ULONG dwCipher,
    IN PUCHAR pbOutput,
    IN PUCHAR pbInput,
    IN PUCHAR pbKeyTable,
    IN ULONG dwOp
    )
{
    void (RSA32API *pCipher)(BYTE *, BYTE *, void *, int) = (dwCipher == XC_SERVICE_DES_CIPHER) ? des : tripledes;
    (*pCipher)( pbOutput, pbInput, pbKeyTable, dwOp );
}
   
void
XCryptBlockCryptCBC(
    IN ULONG dwCipher,
    IN ULONG dwInputLength,
    IN PUCHAR pbOutput,
    IN PUCHAR pbInput,
    IN PUCHAR pbKeyTable,
    IN ULONG dwOp,
    IN PUCHAR pbFeedback
    )
{
    BYTE *pbInputEnd = pbInput + dwInputLength;
    void (RSA32API *pCipher)(BYTE *, BYTE *, void *, int) = (dwCipher == XC_SERVICE_DES_CIPHER) ? des : tripledes;
    while (pbInput < pbInputEnd)
    {
        CBC(pCipher, XC_SERVICE_DES_BLOCKLEN, pbOutput, pbInput, pbKeyTable, dwOp, pbFeedback);
        pbInput += XC_SERVICE_DES_BLOCKLEN;
        pbOutput += XC_SERVICE_DES_BLOCKLEN;
    }
}

ULONG
XCryptCryptService(
    IN ULONG dwOp,
    IN void* pArgs
    )
{
    return 0;
}

//
// original ROM Crypto vector, never gets changed.
//
const CRYPTO_VECTOR originalCryptoVector =
{
    XCryptSHAInit,
    XCryptSHAUpdate,
    XCryptSHAFinal,
    XCryptRC4Key,
    XCryptRC4Crypt,
    XCryptHMAC,
    XCryptPKEncPublic,
    XCryptPKDecPrivate,
    XCryptPKGetKeyLen,
    XCryptVerifyPKCS1Signature,
    XCryptModExp,
    XCryptDESKeyParity,
    XCryptKeyTable,
    XCryptBlockCrypt,
    XCryptBlockCryptCBC,
    XCryptCryptService
};

//
// Updated crypto vector, contains pointers to
// the routines that should get called through the ROM entry points.
// Initial is the same as originalCryptoVector.
//
CRYPTO_VECTOR updatedCryptoVector =
{
    XCryptSHAInit,
    XCryptSHAUpdate,
    XCryptSHAFinal,
    XCryptRC4Key,
    XCryptRC4Crypt,
    XCryptHMAC,
    XCryptPKEncPublic,
    XCryptPKDecPrivate,
    XCryptPKGetKeyLen,
    XCryptVerifyPKCS1Signature,
    XCryptModExp,
    XCryptDESKeyParity,
    XCryptKeyTable,
    XCryptBlockCrypt,
    XCryptBlockCryptCBC,
    XCryptCryptService
};

void
XcUpdateCrypto(
    IN PCRYPTO_VECTOR pNewVector,
    OUT OPTIONAL PCRYPTO_VECTOR pROMVector
    )
{
    //
    // Patch the non null routines
    //
    if ( pNewVector->pXcSHAInit )
    {
        updatedCryptoVector.pXcSHAInit = pNewVector->pXcSHAInit;
    }
    if ( pNewVector->pXcSHAUpdate )
    {
        updatedCryptoVector.pXcSHAUpdate = pNewVector->pXcSHAUpdate;
    }
    if ( pNewVector->pXcSHAFinal )
    {
        updatedCryptoVector.pXcSHAFinal = pNewVector->pXcSHAFinal;
    }
    if ( pNewVector->pXcRC4Key )
    {
        updatedCryptoVector.pXcRC4Key = pNewVector->pXcRC4Key;
    }
    if ( pNewVector->pXcRC4Crypt )
    {
        updatedCryptoVector.pXcRC4Crypt = pNewVector->pXcRC4Crypt;
    }
    if ( pNewVector->pXcHMAC )
    {
        updatedCryptoVector.pXcHMAC = pNewVector->pXcHMAC;
    }
    if ( pNewVector->pXcPKEncPublic )
    {
        updatedCryptoVector.pXcPKEncPublic = pNewVector->pXcPKEncPublic;
    }
    if ( pNewVector->pXcPKDecPrivate )
    {
        updatedCryptoVector.pXcPKDecPrivate = pNewVector->pXcPKDecPrivate;
    }
    if ( pNewVector->pXcPKGetKeyLen )
    {
        updatedCryptoVector.pXcPKGetKeyLen = pNewVector->pXcPKGetKeyLen;
    }
    if ( pNewVector->pXcVerifyPKCS1Signature )
    {
        updatedCryptoVector.pXcVerifyPKCS1Signature = pNewVector->pXcVerifyPKCS1Signature;
    }
    if ( pNewVector->pXcModExp )
    {
        updatedCryptoVector.pXcModExp = pNewVector->pXcModExp;
    }
    if ( pNewVector->pXcDESKeyParity )
    {
        updatedCryptoVector.pXcDESKeyParity = pNewVector->pXcDESKeyParity;
    }
    if ( pNewVector->pXcKeyTable )
    {
        updatedCryptoVector.pXcKeyTable = pNewVector->pXcKeyTable;
    }
    if ( pNewVector->pXcBlockCrypt )
    {
        updatedCryptoVector.pXcBlockCrypt = pNewVector->pXcBlockCrypt;
    }
    if ( pNewVector->pXcBlockCryptCBC )
    {
        updatedCryptoVector.pXcBlockCryptCBC = pNewVector->pXcBlockCryptCBC;
    }
    if ( pNewVector->pXcCryptService )
    {
        updatedCryptoVector.pXcCryptService = pNewVector->pXcCryptService;
    }

    //
    // return out the original ROM vector
    //
    if (pROMVector)
    {
        *pROMVector = originalCryptoVector;
    }
}

//
// Exposed ROM crypto routines that jump using the updated vector
//
void
XcSHAInit(
    IN PUCHAR pbSHAContext
    )
{
    (*updatedCryptoVector.pXcSHAInit)(pbSHAContext);
}
   
void
XcSHAUpdate(
    IN PUCHAR pbSHAContext,
    IN PUCHAR pbInput,
    IN ULONG dwInputLength
    )
{
    (*updatedCryptoVector.pXcSHAUpdate)(pbSHAContext, pbInput, dwInputLength);
}
   
void
XcSHAFinal(
    IN PUCHAR pbSHAContext,
    IN PUCHAR pbDigest
    )
{
    (*updatedCryptoVector.pXcSHAFinal)(pbSHAContext, pbDigest);
}
   
void
XcRC4Key(
    IN PUCHAR pbKeyStruct,
    IN ULONG dwKeyLength,
    IN PUCHAR pbKey
    )
{
    (*updatedCryptoVector.pXcRC4Key)(pbKeyStruct, dwKeyLength, pbKey);
}
   
void
XcRC4Crypt(
    IN PUCHAR pbKeyStruct,
    IN ULONG dwInputLength,
    IN PUCHAR pbInput
    )
{
    (*updatedCryptoVector.pXcRC4Crypt)(pbKeyStruct, dwInputLength, pbInput);
}
   
void
XcHMAC(
    IN PBYTE pbKeyMaterial,
    IN ULONG cbKeyMaterial,
    IN PBYTE pbData,
    IN ULONG cbData,
    IN PBYTE pbData2,
    IN ULONG cbData2,
    OUT PBYTE pbDigest
    )
{
    (*updatedCryptoVector.pXcHMAC)(pbKeyMaterial, cbKeyMaterial, pbData, cbData, pbData2, cbData2, pbDigest);
}

ULONG
XcPKEncPublic(
    IN PUCHAR pbPubKey,
    IN PUCHAR pbInput,
    IN PUCHAR pbOutput
    )
{
    return (*updatedCryptoVector.pXcPKEncPublic)(pbPubKey, pbInput, pbOutput);
}
   
ULONG
XcPKDecPrivate(
    IN PUCHAR pbPrvKey,
    IN PUCHAR pbInput,
    IN PUCHAR pbOutput
    )
{
    return (*updatedCryptoVector.pXcPKDecPrivate)(pbPrvKey, pbInput, pbOutput);
}

ULONG
XcPKGetKeyLen(
    IN PUCHAR pbPubKey
    )
{
    return (*updatedCryptoVector.pXcPKGetKeyLen)(pbPubKey);
}

BOOLEAN
XcVerifyPKCS1Signature(
    IN PUCHAR pbSig,
    IN PUCHAR pbPubKey,
    IN PUCHAR pbDigest
    )
{
    return (*updatedCryptoVector.pXcVerifyPKCS1Signature)(pbSig, pbPubKey, pbDigest);
}
   
ULONG
XcModExp(
    IN LPDWORD pA,
    IN LPDWORD pB,
    IN LPDWORD pC,
    IN LPDWORD pD,
    IN ULONG dwN
    )
{
    return (*updatedCryptoVector.pXcModExp)(pA, pB, pC, pD, dwN);
}
   
void
XcDESKeyParity(
    IN PUCHAR pbKey,
    IN ULONG dwKeyLength
    )
{
    (*updatedCryptoVector.pXcDESKeyParity)(pbKey, dwKeyLength);
}
   
void
XcKeyTable(
    IN ULONG dwCipher,
    OUT PUCHAR pbKeyTable,
    IN PUCHAR pbKey
    )
{
    (*updatedCryptoVector.pXcKeyTable)(dwCipher, pbKeyTable, pbKey);
}
   
void
XcBlockCrypt(
    IN ULONG dwCipher,
    IN PUCHAR pbOutput,
    IN PUCHAR pbInput,
    IN PUCHAR pbKeyTable,
    IN ULONG dwOp
    )
{
    (*updatedCryptoVector.pXcBlockCrypt)(dwCipher, pbOutput, pbInput, pbKeyTable, dwOp);
}
   
void
XcBlockCryptCBC(
    IN ULONG dwCipher,
    IN ULONG dwInputLength,
    IN PUCHAR pbOutput,
    IN PUCHAR pbInput,
    IN PUCHAR pbKeyTable,
    IN ULONG dwOp,
    IN PUCHAR pbFeedback
    )
{
    (*updatedCryptoVector.pXcBlockCryptCBC)(dwCipher, dwInputLength, pbOutput, pbInput, pbKeyTable, dwOp, pbFeedback);
}

ULONG
XcCryptService(
    IN ULONG dwOp,
    IN void* pArgs
    )
{
    return (*updatedCryptoVector.pXcCryptService)(dwOp, pArgs);
}



