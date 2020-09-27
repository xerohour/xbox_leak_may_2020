/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    xcrypt.h

Abstract:

    This module contains definitions the XBox encryption and certificate

--*/


#ifndef _XCRYPT_H
#define _XCRYPT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "guiddef.h"

//
// Number of bits for keys in the public key algorithms
//
#define XC_KEY_BITS               (2048)

//
// Size of sigunature corresponding to the keysize.  This is the
// buffer size needed for public key encryption of a digest
//
#define XC_ENC_SIGNATURE_SIZE     ((XC_KEY_BITS + 7) / 8)

//
// Number of bytes in the public key based on the key size
//
#define XC_PUBLIC_KEYDATA_SIZE    (284)

//
// Size of our symmetric key
//
#define XC_SYMMETRIC_KEY_SIZE     16

//
// Number of bytes in a digest (same as A_SHA_DIGEST_LEN)
// NOTE: do not change this value.  XE needs this to be 20
//
#define XC_DIGEST_LEN             20

//
// Size for SHA digest
//
#define XC_SERVICE_DIGEST_SIZE    XC_DIGEST_LEN

//
// Number of bytes in SHA Buffer
//
#define XC_SERVICE_SHA_CONTEXT_SIZE  116
 
//
// Number of bytes in RC4 Buffer
//
#define XC_SERVICE_RC4_KEYSTRUCT_SIZE   258

//
// Constanst needed for DES
//
#define XC_SERVICE_DES_TABLESIZE	128
#define XC_SERVICE_DES_BLOCKLEN	    8
#define XC_SERVICE_DES_KEYSIZE	    8

#define XC_SERVICE_DES3_TABLESIZE	(3 * XC_SERVICE_DES_TABLESIZE)
#define XC_SERVICE_DES3_KEYSIZE	    24

// constants for Block operations
#define XC_SERVICE_ENCRYPT		1
#define XC_SERVICE_DECRYPT		0

// constants for selecting DES or triple DES cipher
#define XC_SERVICE_DES_CIPHER		0
#define XC_SERVICE_DES3_CIPHER		1

//
// Low level crypto API
//

VOID
XCCalcDigest(
    IN  PUCHAR MsgData,
    IN  ULONG  MsgDataLen,
    IN  PUCHAR Digest
    );

VOID
XCSymmetricEncDec(
    IN  PUCHAR MsgData,
    IN  ULONG  MsgDataLen,
    IN  PUCHAR Key,
    IN  ULONG  KeyBytes
    );

ULONG
XCCalcSigSize(
    IN  PUCHAR  pbPrivateKey
    );

ULONG
XCCalcKeyLen(
    IN  PUCHAR  pbPublicKey
    );

BOOLEAN
XCSignDigest(
    IN   PUCHAR    pbDigest,
    IN   PUCHAR    pbPrivateKey,
    OUT  PUCHAR    pbSig
    );

BOOLEAN
XCVerifyDigest(
    IN   PUCHAR   pbSig,
    IN   PUCHAR   pbPublicKey,
    IN   PUCHAR   pbWorkspace,
    IN   PUCHAR   pbCompareDigest
    );

//
// Crypto APIs the ROM exports
//

//
// Equivalent to A_SHAInit. Byte array pointed to by pbSHAContext
// must be at least XC_SERVICE_SHA_CONTEXT_SIZE (116) bytes long.
//
void
XcSHAInit(
    OUT PUCHAR pbSHAContext
    );
   
//
// Equivalent to A_SHAUpdate. Byte array pointed to by pbSHAContext
// must be at least XC_SERVICE_SHA_CONTEXT_SIZE (116) bytes long.
//
void
XcSHAUpdate(
    IN OUT PUCHAR pbSHAContext,
    IN PUCHAR pbInput,
    IN ULONG dwInputLength
    );
   
//
// Equivalent to A_SHAFinal.
// Byte array pbDigest must be able to hold XC_SERVICE_DIGEST_SIZE (20) bytes
//
void
XcSHAFinal(
    IN PUCHAR pbSHAContext,
    OUT PUCHAR pbDigest
    );
   
//
// Equivalent to rc4_key. Byte array pointed to by pbKeyStruct
// must be at least XC_SERVICE_RC4_KEYSTRUCT_SIZE (258) bytes long.
//
void
XcRC4Key(
    OUT PUCHAR pbKeyStruct,
    IN ULONG dwKeyLength,
    IN PUCHAR pbKey
    );
   
//
// Equivalent to rc4. Byte array pointed to by pbKeyStruct
// must be at least XC_SERVICE_RC4_KEYSTRUCT_SIZE (258) bytes long.
//
void
XcRC4Crypt(
    IN PUCHAR pbKeyStruct,
    IN ULONG dwInputLength,
    IN OUT PUCHAR pbInput
    );
   
//
// Computes SHA1-HMAC according to RFC 2104 for 2 pieces of input data.
// Byte array pbDigest must be able to hold XC_SERVICE_DIGEST_SIZE (20) bytes
//
void
XcHMAC(
    IN PUCHAR pbKey,
    IN ULONG dwKeyLength,
    IN PUCHAR pbInput,
    IN ULONG dwInputLength,
    IN PUCHAR pbInput2,
    IN ULONG dwInputLength2,
    OUT PUCHAR pbDigest
    );
   
//
// Equivalent to BSafeEncPublic.
//
ULONG
XcPKEncPublic(
    IN PUCHAR pbPubKey,
    IN PUCHAR pbInput,
    OUT PUCHAR pbOutput
    );
   
//
// Equivalent to BSafeDecPrivate.
//
ULONG
XcPKDecPrivate(
    IN PUCHAR pbPrvKey,
    IN PUCHAR pbInput,
    OUT PUCHAR pbOutput
    );

//
// Get keylen for public key
//
ULONG
XcPKGetKeyLen(
    IN PUCHAR pbPubKey
    );

//
// Verify PKCS1 signature for given XC_SERVICE_DIGEST_SIZE (20) byte digest
//
BOOLEAN
XcVerifyPKCS1Signature(
    IN PUCHAR pbSig,
    IN PUCHAR pbPubKey,
    IN PUCHAR pbDigest
    );
  
//
// Equivalent to BenalohModExp
// computes A = B ^ C mod D, N = len of params in DWORDs
//
ULONG
XcModExp(
    OUT ULONG* pA,
    IN ULONG* pB,
    IN ULONG* pC,
    IN ULONG* pD,
    IN ULONG dwN
    );
   
//
// Equivalent to desparityonkey
//
void
XcDESKeyParity(
    IN OUT PUCHAR pbKey,
    IN ULONG dwKeyLength
    );
   
//
// Equivalent to deskey or tripledes3key.
// dwKeyLength must be either XC_SERVICE_DES_KEYSIZE (8) or XC_SERVICE_DES3_KEYSIZE (24)
// Byte array pbKeyTable must be able to hold XC_SERVICE_DES_TABLESIZE (128)
// or XC_SERVICE_DES3_TABLESIZE (384) bytes
//
void
XcKeyTable(
    IN ULONG dwCipher,
    OUT PUCHAR pbKeyTable,
    IN PUCHAR pbKey
    );

//
// Equivalent to des or tripledes.
// dwCipher must be either XC_SERVICE_DES_CIPHER (0) or XC_SERVICE_DES3_CIPHER (1)
// dwOp must be either XC_SERVICE_DECRYPT (0) or XC_SERVICE_ENCRYPT (1)
//
void
XcBlockCrypt(
    IN ULONG dwCipher,
    OUT PUCHAR pbOutput,
    IN PUCHAR pbInput,
    IN PUCHAR pbKeyTable,
    IN ULONG dwOp
    );
   
//
// Roughly equivalent to CBC. Will loop through multiple blocks.
// dwInputLength can contain any multiple of XC_SERVICE_DES_BLOCKLEN (8)
// dwCipher must be either XC_SERVICE_DES_CIPHER (0) or XC_SERVICE_DES3_CIPHER (1)
// dwOp must be either XC_SERVICE_DECRYPT (0) or XC_SERVICE_ENCRYPT (1)
//
void
XcBlockCryptCBC(
    IN ULONG dwCipher,
    IN ULONG dwInputLength,
    OUT PUCHAR pbOutput,
    IN PUCHAR pbInput,
    IN PUCHAR pbKeyTable,
    IN ULONG dwOp,
    IN PUCHAR pbFeedback
    );

//
// Generic Crypt Service function for future extension
//
ULONG
XcCryptService(
    IN ULONG dwOp,
    IN void* pArgs
    );

//
// typedef's of all the ROM crypto exports
//
typedef void (*pfXcSHAInit)(PUCHAR pbSHAContext);
typedef void (*pfXcSHAUpdate)(PUCHAR pbSHAContext, PUCHAR pbInput, ULONG dwInputLength);
typedef void (*pfXcSHAFinal)(PUCHAR pbSHAContext, PUCHAR pbDigest);
typedef void (*pfXcRC4Key)(PUCHAR pbKeyStruct, ULONG dwKeyLength, PUCHAR pbKey);
typedef void (*pfXcRC4Crypt)(PUCHAR pbKeyStruct, ULONG dwInputLength, PUCHAR pbInput);
typedef void (*pfXcHMAC)(PUCHAR pbKey, ULONG dwKeyLength, PUCHAR pbInput, ULONG dwInputLength, PUCHAR pbInput2, ULONG dwInputLength2, PUCHAR pbDigest);
typedef ULONG (*pfXcPKEncPublic)(PUCHAR pbPubKey, PUCHAR pbInput, PUCHAR pbOutput);
typedef ULONG (*pfXcPKDecPrivate)(PUCHAR pbPrvKey, PUCHAR pbInput, PUCHAR pbOutput);
typedef ULONG (*pfXcPKGetKeyLen)(PUCHAR pbPubKey);
typedef BOOLEAN (*pfXcVerifyPKCS1Signature)(PUCHAR pbSig, PUCHAR pbPubKey, PUCHAR pbDigest);
typedef ULONG (*pfXcModExp)(ULONG* pA, ULONG* pB, ULONG* pC, ULONG* pD, ULONG dwN);
typedef void (*pfXcDESKeyParity)(PUCHAR pbKey, ULONG dwKeyLength);
typedef void (*pfXcKeyTable)(ULONG dwCipher, PUCHAR pbKeyTable, PUCHAR pbKey);
typedef void (*pfXcBlockCrypt)(ULONG dwCipher, PUCHAR pbOutput, PUCHAR pbInput, PUCHAR pbKeyTable, ULONG dwOp);
typedef void (*pfXcBlockCryptCBC)(ULONG dwCipher, ULONG dwInputLength, PUCHAR pbOutput, PUCHAR pbInput, PUCHAR pbKeyTable, ULONG dwOp, PUCHAR pbFeedback);
typedef ULONG (*pfXcCryptService)(ULONG dwOp, void* pArgs);

//
// Vector to store all ROM Crypto routines
//
typedef struct {
    pfXcSHAInit pXcSHAInit;
    pfXcSHAUpdate pXcSHAUpdate;
    pfXcSHAFinal pXcSHAFinal;
    pfXcRC4Key pXcRC4Key;
    pfXcRC4Crypt pXcRC4Crypt;
    pfXcHMAC pXcHMAC;
    pfXcPKEncPublic pXcPKEncPublic;
    pfXcPKDecPrivate pXcPKDecPrivate;
    pfXcPKGetKeyLen pXcPKGetKeyLen;
    pfXcVerifyPKCS1Signature pXcVerifyPKCS1Signature;
    pfXcModExp pXcModExp;
    pfXcDESKeyParity pXcDESKeyParity;
    pfXcKeyTable pXcKeyTable;
    pfXcBlockCrypt pXcBlockCrypt;
    pfXcBlockCryptCBC pXcBlockCryptCBC;
    pfXcCryptService pXcCryptService;
} CRYPTO_VECTOR, *PCRYPTO_VECTOR;

//
// Update the current set of crypto routines with updated ones.
// Also return back the original ROM routines if needed.
//
void
XcUpdateCrypto(
    IN PCRYPTO_VECTOR pNewVector,
    OUT OPTIONAL PCRYPTO_VECTOR pROMVector
    );

#ifdef __cplusplus
}      // extern "C"
#endif

#endif // _XCRYPT_H
