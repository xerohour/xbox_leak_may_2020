/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    xcbase.c

Abstract:

    Contains implementation of base level crypto function

--*/

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

    XcSHAInit((PUCHAR)&SHAHash);
    XcSHAUpdate((PUCHAR)&SHAHash, (PBYTE)&dwMsgDataLen, sizeof(DWORD));
    XcSHAUpdate((PUCHAR)&SHAHash, pbMsgData, dwMsgDataLen);
    XcSHAFinal((PUCHAR)&SHAHash, abSHADigest);

    memcpy(pbDigest, abSHADigest, A_SHA_DIGEST_LEN);
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

    if (!XcPKEncPublic((PUCHAR)pPubKey, pbInput, pbOutput)) {
        return FALSE;
    }


    if (!XCVerifyPKCS1SigningFmt(pPubKey, pbCompareDigest, pbOutput)) {
        return FALSE;
    }

    return TRUE;
}
