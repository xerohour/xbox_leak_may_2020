//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1995.
//
//  File:       keys.c
//
//  Contents:   Well known keys for certificate validation
//
//  Classes:
//
//  Functions:
//
//  History:    9-21-95   RichardW   Created
//
//----------------------------------------------------------------------------


#include "spbase.h"

#include <rsa.h>
#include <md5.h>
#include <rc4.h>

extern BOOL
WINAPI
SigRSAMD5Sign(
    PUCHAR          pData,
    DWORD           cbData,
    PUCHAR          pSigned,
    DWORD           *pcbSigned,
    PctPrivateKey    *pKey);

#define PRIVATE_KEY_TEXT    "private-key"

/*
UCHAR   bMD2SigPrefixReverse[] = { 0x10, 0x04, 0x00, 0x05, 0x02, 0x02, 0x0d,
                                   0xf7, 0x86, 0x48, 0x86, 0x2a, 0x08, 0x06,
                                   0x0c, 0x30, 0x20, 0x30 };

UCHAR   bMD5SigPrefixReverse[] = { 0x10, 0x04, 0x00, 0x05, 0x05, 0x02, 0x0d,
                                   0xf7, 0x86, 0x48, 0x86, 0x2a, 0x08, 0x06,
                                   0x0c, 0x30, 0x20, 0x30 };

UCHAR   bMD5SigPrefix[] =  { 0x30, 0x20, 0x30, 0x0c, 0x06, 0x08, 0x2a, 0x86,
                             0x48, 0x86, 0xf7, 0x0d, 0x02, 0x05, 0x05, 0x00,
                             0x04, 0x10 };
                             */


#define BIG_ENDIAN 2
#define NETWORK_ORDER BIG_ENDIAN

BOOL
EncodePrivateKey(
    PctPrivateKey * pKey,
    DWORD           InitialSize,
    PSTR            pszPassword,
    PUCHAR *        ppBuffer,
    DWORD *         pcbBuffer)
{
    BSAFE_PRV_KEY *pk;
    BSAFE_KEY_PARTS parts;
    PUCHAR  pBuffer;
    PUCHAR  pbEncoded;
    PUCHAR  pSave;
    int     Result;
    BYTE    Zero;
    DWORD   BigLen;
    DWORD   ShortLen;
    PUCHAR  pSequence;
    DWORD   Length;
    PUCHAR  pSave2;
    long    Delta2;
    UCHAR   Key[16];
    struct RC4_KEYSTRUCT  rc4Key;
    MD5_CTX md5Ctx;

    pk = (BSAFE_PRV_KEY *) pKey->pKey;
    BSafeGetPrvKeyParts(pk, &parts);

    pBuffer = SPExternalAlloc(InitialSize);
    if (NULL == pBuffer)
    {
        return(FALSE);
    }

    pbEncoded = pBuffer;

    BigLen = pk->bitlen / 8;
    ShortLen = BigLen / 2;

    //
    // We just do the key now, and then we'll add the other stuff later
    //
    // Encode the maximum length for now.  We'll patch this up later
    //

    Result = EncodeHeader(pbEncoded, InitialSize, TRUE);

    pbEncoded += Result;

    pSave = pbEncoded;

    Zero = 0;
    Result = EncodeInteger(pbEncoded, &Zero, sizeof(Zero), TRUE);
    pbEncoded += Result;

    Result = EncodeInteger(pbEncoded, parts.modulus, BigLen, TRUE);
    pbEncoded += Result;

    Result = EncodeInteger(pbEncoded, (PBYTE) &pk->pubexp, sizeof(DWORD), TRUE);
    pbEncoded += Result;

    Result = EncodeInteger(pbEncoded, parts.prvexp, BigLen, TRUE);
    pbEncoded += Result;

    Result = EncodeInteger(pbEncoded, parts.prime1, ShortLen, TRUE);
    pbEncoded += Result;

    Result = EncodeInteger(pbEncoded, parts.prime2, ShortLen, TRUE);
    pbEncoded += Result;

    Result = EncodeInteger(pbEncoded, parts.exp1, ShortLen, TRUE);
    pbEncoded += Result;

    Result = EncodeInteger(pbEncoded, parts.exp2, ShortLen, TRUE);
    pbEncoded += Result;

    Result = EncodeInteger(pbEncoded, parts.coef, ShortLen, TRUE);
    pbEncoded += Result;

    Length = pbEncoded - pSave;

    Result = EncodeHeader(pBuffer, Length, TRUE);
    Length += Result;

    pSequence = SPExternalAlloc(Length + 64);
    if (NULL == pSequence)
    {
        SPExternalFree(pBuffer);
        return(FALSE);
    }

    pbEncoded = pSequence;

    Result = EncodeHeader(pbEncoded, Length + 64, TRUE);
    pbEncoded += Result;

    pSave = pbEncoded;

    Result = EncodeInteger(pbEncoded, &Zero, sizeof(Zero), TRUE);
    pbEncoded += Result;

    Result = EncodeAlgorithm(pbEncoded, ALGTYPE_KEYEXCH_RSA_MD5, TRUE);
    pbEncoded += Result;

    Result = EncodeOctetString(pbEncoded, pBuffer, Length, TRUE);
    pbEncoded += Result;

    FillMemory(pBuffer, Length, 0);

    Length = pbEncoded - pSave;

    Result = EncodeHeader(pSequence, Length, TRUE);
    Length += Result;

    SPExternalFree(pBuffer);

    pBuffer = SPExternalAlloc(Length + 128);
    if (NULL == pBuffer)
    {
        SPExternalFree(pSequence);
        return(FALSE);
    }

    pbEncoded = pBuffer;

    Result = EncodeHeader(pBuffer, Length + 128, TRUE);
    pbEncoded += Result;

    pSave = pbEncoded;

    Result = EncodeOctetString(
			pbEncoded,
			PRIVATE_KEY_TEXT,
			strlen(PRIVATE_KEY_TEXT),
			TRUE);
    pbEncoded += Result;

    Result = EncodeHeader(pbEncoded, Length + 64, TRUE);
    pbEncoded += Result;

    pSave2 = pbEncoded;
    Delta2 = Result;

    Result = EncodeAlgorithm(pbEncoded, ALGTYPE_CIPHER_RC4_MD5, TRUE);
    pbEncoded += Result;

    //
    // Encrypt the data with the password
    //

    MD5Init(&md5Ctx);
    MD5Update(&md5Ctx, pszPassword, strlen(pszPassword));
    MD5Final(&md5Ctx);
    CopyMemory(Key, md5Ctx.digest, 16);

    FillMemory(&md5Ctx, sizeof(md5Ctx), 0);

    rc4_key(&rc4Key, 16, Key);
    rc4(&rc4Key, Length, pSequence);
    FillMemory(&rc4Key, sizeof(rc4Key), 0);


    Result = EncodeOctetString(pbEncoded, pSequence, Length, TRUE);

    SPExternalFree(pSequence);

    pbEncoded += Result;

    //
    // Now, back up and fill in the correct lengths:
    //

    Result = EncodeHeader(pSave2 - Delta2, pbEncoded - pSave2, TRUE);
    Result = EncodeHeader(pBuffer, pbEncoded - pSave2, TRUE);

    *pcbBuffer = pbEncoded - pBuffer;
    *ppBuffer = pBuffer;

    return(TRUE);
}


long
EncodeSubjectPubKeyInfo(
    PctPrivateKey * pKey,
    PUCHAR          pBuffer)
{
    PUCHAR  pbEncoded;
    LONG    Result, ResultHeader;
    LONG    PkResult, PkResultHeader;
    PUCHAR  pSave;
    PUCHAR  pBitString;
    PUCHAR  pBitStringBase;
    PUCHAR  pTop;
    DWORD   EstimatedLength;
    BSAFE_PRV_KEY * pk = (BSAFE_PRV_KEY *)pKey->pKey;

    //
    // Encode public key now...
    //

    EstimatedLength = pk->datalen + 32;

    pbEncoded = pBuffer;

    ResultHeader = EncodeHeader(pbEncoded, EstimatedLength, TRUE);
    pbEncoded += ResultHeader;

    pTop = pbEncoded;

    Result = EncodeAlgorithm(pbEncoded, ALGTYPE_KEYEXCH_RSA_MD5, TRUE);
    if (Result < 0)
    {
        return(-1);
    }
    pbEncoded += Result;

    //
    // now, serialize the rsa key data:
    //

    pBitString = SPExternalAlloc(EstimatedLength);
    if (NULL == pBitString)
    {
        return(-1);
    }
    pBitStringBase = pBitString;

    // Encode the Sequence header, public key base and exponent as integers

    PkResultHeader = EncodeHeader(pBitString, EstimatedLength, TRUE);
    pBitString += PkResultHeader;

    pSave = pBitString;

    PkResult = EncodeInteger(pBitString, (PBYTE) (pk + 1), pk->keylen, TRUE);
    pBitString += PkResult;

    PkResult = EncodeInteger(
			pBitString,
			(PBYTE) &pk->pubexp,
			sizeof(DWORD),
			TRUE);
    pBitString += PkResult;

    // Rewrite the bitstring header with an accurate length.

    PkResult = EncodeHeader(pBitStringBase, pBitString - pSave, TRUE);
    SP_ASSERT(PkResult == PkResultHeader);

    // Encode the public key sequence as a raw bitstring, and free the memory.

    Result = EncodeBitString(
			pbEncoded,
			pBitStringBase,
			pBitString - pBitStringBase,
			TRUE);
    pbEncoded += Result;

    SPExternalFree(pBitStringBase);

    // Rewrite the header with an accurate length.

    Result = EncodeHeader(pBuffer, pbEncoded - pTop, TRUE);
    SP_ASSERT(Result == ResultHeader);
    return(Result + (pbEncoded - pTop));
}


BOOL
EncodePublicKey(
    PctPrivateKey * pKey,
    PSTR            pszDN,
    PUCHAR *        ppBuffer,
    DWORD *         pcbBuffer)
{
    PUCHAR  pBuffer;
    PUCHAR  pBufferToSign;
    PBYTE   pBufferBase;		// base of allocated memory

    PUCHAR  pPubEncoded;
    PUCHAR  pPubEncodedBase = NULL;	// base of allocated memory
    PUCHAR  pbEncoded;

    PUCHAR  pSave;
    PUCHAR  pSave2;

    int     Result;
    BYTE    Zero;
    DWORD   ShortLen;
    DWORD   Length;
    LONG    PubRes;
    LONG    PubLen;
    int     ResultSave;
    int     LengthOfReqInfo;
    BSAFE_PRV_KEY * pk = (BSAFE_PRV_KEY *)pKey->pKey;

    ShortLen = EncodeDN(NULL, pszDN, FALSE);
    if (ShortLen < 0)
    {
	return(FALSE);
    }

    Length = pk->datalen + 32 + ShortLen + 16;

    pBuffer = SPExternalAlloc(Length);
    if (NULL == pBuffer)
    {
        return(FALSE);
    }
    pBufferBase = pBuffer;

    pbEncoded = pBuffer;

    // Encode SEQUENCE_TAG, <length>

    Result = EncodeHeader(pBuffer, Length, TRUE);
    ResultSave = Result;

    pbEncoded += Result;

    pSave = pbEncoded;

    // Encode integer 0

    Zero = 0;
    Result = EncodeInteger(pbEncoded, &Zero, sizeof(Zero), TRUE);
    pbEncoded += Result;

    // Encode sequence of names

    Result = EncodeDN(pbEncoded, pszDN, TRUE);
    if (Result < 0)
    {
	goto error;
    }
    pbEncoded += Result;

    Result = EncodeSubjectPubKeyInfo(pKey, pbEncoded);
    if (Result < 0)
    {
	goto error;
    }
    pbEncoded += Result;

    // Encode empty attributes set

    Result = EncodeAttributeHeader(pbEncoded, 0, TRUE);
    pbEncoded += Result;

    Result = EncodeHeader(pBuffer, pbEncoded - pSave, TRUE);

    // If the header sequence length takes up less space than we anticipated,
    // add the difference to the base pointer and encode the header again,
    // right before the encoded data.

    if (Result != ResultSave)
    {
	SP_ASSERT(ResultSave > Result);
        pBuffer += ResultSave - Result;
        Result = EncodeHeader(pBuffer, pbEncoded - pSave, TRUE);
    }

    LengthOfReqInfo = Result + (pbEncoded - pSave);

    pBufferToSign = pBuffer;

    // How much space do we need?

    PubLen = LengthOfReqInfo + pk->datalen + 32;
    pPubEncoded = SPExternalAlloc(PubLen);
    if (NULL == pPubEncoded)
    {
	goto error;
    }
    pPubEncodedBase = pPubEncoded;

    Result = EncodeHeader(pPubEncoded, PubLen, TRUE);

    pSave2 = pPubEncoded;

    ResultSave = Result;

    pPubEncoded += Result;

    pSave = pPubEncoded;

    CopyMemory(pPubEncoded, pBuffer, LengthOfReqInfo);

    pPubEncoded += LengthOfReqInfo;

    Result = EncodeAlgorithm(pPubEncoded, ALGTYPE_SIG_RSA_MD5, TRUE);

    pPubEncoded += Result;


    // pBufferBase has the real pointer

    pBuffer = SPExternalAlloc(pk->datalen + 16);
    if (NULL == pBuffer)
    {
        goto error;
    }

    Result = pk->datalen + 16;
    SigRSAMD5Sign(pBufferToSign, LengthOfReqInfo, pBuffer, &Result, pKey);


    PubRes = EncodeBitString(pPubEncoded, pBuffer, Result, TRUE);

    pPubEncoded += PubRes;

    Result = EncodeHeader(pSave2, pPubEncoded - pSave, TRUE);

    if (Result != ResultSave)
    {
        if (Result > ResultSave)
        {
            //
            // Yuck.  The chunk has actually grown from the estimate.
            //

            *ppBuffer = SPExternalAlloc(pPubEncoded - pSave + Result);
            if (NULL != *ppBuffer)
            {
                EncodeHeader(*ppBuffer, pPubEncoded - pSave, TRUE);
                CopyMemory(*ppBuffer + Result, pSave, pPubEncoded - pSave);

                SPExternalFree(pSave2);
                pSave2 = *ppBuffer;
            }
            else
            {
                SPExternalFree(pBufferBase);
                return(FALSE);
            }
        }
        else
        {
	    SP_ASSERT(Result == ResultSave - 1)
            pSave2++;
            Result = EncodeHeader(pSave2, pPubEncoded - pSave, TRUE);

        }
    }

    *ppBuffer = pSave2;

    *pcbBuffer = Result + (pPubEncoded - pSave);

    SPExternalFree(pBuffer);
    SPExternalFree(pBufferBase);
    return(TRUE);

error:
    SPExternalFree(pBufferBase);
    if (NULL != pPubEncodedBase)
    {
	SPExternalFree(pPubEncodedBase);
    }
    return(FALSE);
}


BOOL
GenerateKeyPair(
    PSSL_CREDENTIAL_CERTIFICATE pCerts,
    PSTR pszDN,
    PSTR pszPassword,
    DWORD Bits)
{
    DWORD BitsCopy;
    DWORD dwPrivateSize;
    DWORD dwPublicSize;
    PctPrivateKey *pPrivate;
    PctPublicKey *pPublic;


    BitsCopy = Bits;
    BSafeComputeKeySizes(&dwPublicSize, &dwPrivateSize, &BitsCopy);

    pPrivate = SPExternalAlloc(dwPrivateSize + sizeof(PctPrivateKey));
    if (NULL == pPrivate)
    {
        return(FALSE);
    }

    pPublic = SPExternalAlloc(dwPublicSize + sizeof(PctPublicKey));
    if (NULL == pPublic)
    {
	SPExternalFree(pPrivate);
        return(FALSE);
    }

    pPublic->cbKey = dwPublicSize;
    pPrivate->cbKey = dwPrivateSize;

    if (!BSafeMakeKeyPair(
		    (BSAFE_PUB_KEY *) pPublic->pKey,
		    (BSAFE_PRV_KEY *) pPrivate->pKey,
		    Bits))
    {
	goto error;
    }

    // Ah yes, now to encode them...
    //
    // First, the private key.  Why?  Well, it's at least straight-forward

    if (!EncodePrivateKey(
		    pPrivate,
		    dwPrivateSize,
		    pszPassword,
		    &pCerts->pPrivateKey,
		    &pCerts->cbPrivateKey))
    {
	goto error;
    }

    if (!EncodePublicKey(
		    pPrivate,
		    pszDN,
		    &pCerts->pCertificate,
		    &pCerts->cbCertificate))
    {
        SPExternalFree(pCerts->pPrivateKey);
	goto error;
    }

    return(TRUE);

error:
    SPExternalFree(pPublic);
    SPExternalFree(pPrivate);
    return(FALSE);
}
