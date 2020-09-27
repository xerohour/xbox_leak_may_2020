/*
 *
 * preldr32.c
 *
 * Boot pre-loader
 *
 */

#include "ntos.h"
typedef ULONG DWORD, *PDWORD, *LPDWORD;
typedef UCHAR BYTE, *PBYTE, *LPBYTE;
typedef ULONG BOOL;
#define far
#include <xcrypt.h>
#include <sha.h>
#include <rsa.h>
#include <rc4.h>
#include <bldr.h>
#include <stdio.h>

BOOLEAN
VerifyDigest(
    PUCHAR PubKey,
    PUCHAR EncryptedDigest, 
    PUCHAR PlaintextDigest
);

VOID
DbgInitialize(
    VOID
    );

VOID
DbgPrintCharacter(
    UCHAR
    );

#ifndef MCP_XMODE3P
static UCHAR BldrEncKey[] =
{
    0x57,0x42,0x29,0x0C,0x30,0x1E,0xD3,0x01,
    0xB3,0xE5,0x5D,0x28,0x50,0x31,0xE1,0xCE
};
#endif

// Public key matches the XBE public key

UCHAR EncryptedPubKey[] =
{
#ifdef RETAILPUBKEY
#else
    0x52,0x53,0x41,0x31,0x08,0x01,0x00,0x00,
    0x00,0x08,0x00,0x00,0xFF,0x00,0x00,0x00,
    0x01,0x00,0x01,0x00,0x9B,0x83,0xD4,0xD5,
    0xDE,0x16,0x25,0x8E,0xE5,0x15,0xF2,0x18,
    0x9D,0x19,0x1C,0xF8,0xFE,0x91,0xA5,0x83,
    0xAE,0xA5,0xA8,0x95,0x3F,0x01,0xB2,0xC9,
    0x34,0xFB,0xC7,0x51,0x2D,0xAC,0xFF,0x38,
    0xE6,0xB6,0x7B,0x08,0x4A,0xDF,0x98,0xA3,
    0xFD,0x31,0x81,0xBF,0xAA,0xD1,0x62,0x58,
    0xC0,0x6C,0x8F,0x8E,0xCD,0x96,0xCE,0x6D,
    0x03,0x44,0x59,0x93,0xCE,0xEA,0x8D,0xF4,
    0xD4,0x6F,0x6F,0x34,0x5D,0x50,0xF1,0xAE,
    0x99,0x7F,0x1D,0x92,0x15,0xF3,0x6B,0xDB,
    0xF9,0x95,0x8B,0x3F,0x54,0xAD,0x37,0xB5,
    0x4F,0x0A,0x58,0x7B,0x48,0xA2,0x9F,0x9E,
    0xA3,0x16,0xC8,0xBD,0x37,0xDA,0x9A,0x37,
    0xE6,0x3F,0x10,0x1B,0xA8,0x4F,0xA3,0x14,
    0xFA,0xBE,0x12,0xFB,0xD7,0x19,0x4C,0xED,
    0xAD,0xA2,0x95,0x8F,0x39,0x8C,0xC4,0x69,
    0x0F,0x7D,0xB8,0x84,0x0A,0x99,0x5C,0x53,
    0x2F,0xDE,0xF2,0x1B,0xC5,0x1D,0x4C,0x43,
    0x3C,0x97,0xA7,0xBA,0x8F,0xC3,0x22,0x67,
    0x39,0xC2,0x62,0x74,0x3A,0x0C,0xB5,0x57,
    0x01,0x3A,0x67,0xC6,0xDE,0x0C,0x0B,0xF6,
    0x08,0x01,0x64,0xDB,0xBD,0x81,0xE4,0xDC,
    0x09,0x2E,0xD0,0xF1,0xD0,0xD6,0x1E,0xBA,
    0x38,0x36,0xF4,0x4A,0xDD,0xCA,0x39,0xEB,
    0x76,0xCF,0x95,0xDC,0x48,0x4C,0xF2,0x43,
    0x8C,0xD9,0x44,0x26,0x7A,0x9E,0xEB,0x99,
    0xA3,0xD8,0xFB,0x30,0xA8,0x14,0x42,0x82,
    0x8D,0xB4,0x31,0xB3,0x1A,0xD5,0x2B,0xF6,
    0x32,0xBC,0x62,0xC0,0xFE,0x81,0x20,0x49,
    0xE7,0xF7,0x58,0x2F,0x2D,0xA6,0x1B,0x41,
    0x62,0xC7,0xE0,0x32,0x02,0x5D,0x82,0xEC,
    0xA3,0xE4,0x6C,0x9B,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00
#endif
};

/* We dole out RAM from below the relocated boot loader.  We'll overwrite this
 * when we decrypt the boot loader */
ULONG pbNextAlloc = BLDR_RELOCATED_ORIGIN;

PVOID RSA32Alloc(ULONG u1)
{
    /* Round up to 32 bytes */
    u1 = (u1 + 31) & ~31;
    pbNextAlloc -= u1;
    return (PVOID)pbNextAlloc;
}
    
VOID RSA32Free(PVOID pv) {}

typedef struct _PRELDR_FUNCS {
    void (*pfnSHAInit)(A_SHA_CTX *);
    void (*pfnSHAUpdate)(A_SHA_CTX *, unsigned char *, unsigned int);
    void (*pfnSHAFinal)(A_SHA_CTX *, unsigned char [A_SHA_DIGEST_LEN]);
    BOOLEAN (*pfnVerifyDigest)(PUCHAR, PUCHAR, PUCHAR);
} PRELDR_FUNCS, *PPRELDR_FUNCS;

PRELDR_FUNCS PreloaderFuncTable = {
    A_SHAInit,
    A_SHAUpdate,
    A_SHAFinal,
    VerifyDigest
};

//
// Reverse ASN.1 Encodings of possible hash identifiers.
//
static PBYTE shaEncodings[] = {
            //      1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18
            "\x0f\x14\x04\x00\x05\x1a\x02\x03\x0e\x2b\x05\x06\x09\x30\x21\x30",
            "\x0d\x14\x04\x1a\x02\x03\x0e\x2b\x05\x06\x07\x30\x1f\x30",
            "\x00" };

BOOLEAN VerifyDigest(PUCHAR PubKey, PUCHAR EncryptedDigest, PUCHAR PlaintextDigest)
{
    UCHAR Encrypt[264];
    UCHAR Decrypt[264];
    BYTE      rgbTmpHash[A_SHA_DIGEST_LEN];
    DWORD     i;
    DWORD     cb;
    BYTE*     pbStart;
    DWORD     cbTmp;
    BSAFE_PUB_KEY *pKey = (LPBSAFE_PUB_KEY)PubKey;

    /* We need a 264-byte work area, so copy the digest into it and fill out
     * with zeroes */
    memcpy(Encrypt, EncryptedDigest, 256);
    memset(Encrypt+256, 0, 8);
    if(!BSafeEncPublic(pKey, Encrypt, Decrypt))
        return FALSE;

    //
    // reverse the hash to match the signature.
    //
    for (i = 0; i < A_SHA_DIGEST_LEN; i++) {
        rgbTmpHash[i] = PlaintextDigest[A_SHA_DIGEST_LEN - (i + 1)];
    }

    //
    // see if it matches.
    //
    if (memcmp(rgbTmpHash, Decrypt, A_SHA_DIGEST_LEN)) {
        return FALSE;
    }

    cb = A_SHA_DIGEST_LEN;

    //
    // check for any signature type identifiers
    //
    for (i = 0; 0 != *shaEncodings[i]; i += 1) {
        pbStart = (LPBYTE)shaEncodings[i];
        cbTmp = *pbStart++;
        if (0 == memcmp(&Decrypt[cb], pbStart, cbTmp)) {
            // adjust the end of the hash data.
            cb += cbTmp;
            break;
        }
    }

    //
    // check to make sure the rest of the PKCS #1 padding is correct
    //

    if ((0x00 != Decrypt[cb]) || (0x00 != Decrypt[pKey->datalen]) ||
         (0x1 != Decrypt[pKey->datalen - 1])) {
        return FALSE;
    }

    for (i = cb + 1; i < (DWORD)pKey->datalen - 1; i++) {
        if (0xff != Decrypt[i]) {
            return FALSE;
        }
    }

    return TRUE;
}

ULONG PreloaderStartup2(PUCHAR DecryptAddress)
{
    struct RC4_KEYSTRUCT RC4KeyStruct;
    PUCHAR EncBldr;
    ULONG EntryAddress;
    UCHAR Digest[XC_DIGEST_LEN];
    UCHAR PubKey[sizeof EncryptedPubKey];
    A_SHA_CTX SHAHash;
    PUCHAR SignTop;
    PUCHAR Signature;
    PUCHAR SignBase;
    PUCHAR InitTop;
    PUCHAR KernelBase;
    PULONG ROMRegions;
    const PUCHAR SBKey = (PUCHAR)(0LU - ROMDEC_N);
#ifdef MCP_XMODE3P
    int i;
    UCHAR EncKey[XC_DIGEST_LEN];
#endif

    /* The keyset we have is encrypted.  We decrypt it using the SB key */
    rc4_key(&RC4KeyStruct, 12, SBKey);
    memcpy(PubKey, EncryptedPubKey, sizeof EncryptedPubKey);
    rc4(&RC4KeyStruct, sizeof PubKey, PubKey);

    /* Find the pieces */
    SignTop = (PUCHAR)0 - ROM_DEC_SIZE;
    Signature = SignTop - 0x180;
    ROMRegions = (PULONG)(Signature + 0x100);

    /* We ask this ROM what its size is, but we only allow 256k - 1MB in
     * multiples of 256k */
    SignBase = (PUCHAR)ROMRegions[0];
    SignBase = (PUCHAR)(((ULONG)SignBase & 0xFFFC0000) | 0xFFF00000);
    InitTop = SignBase + ROMRegions[1];
    KernelBase = (PUCHAR)ROMRegions[2];

    /* Do some sanity checking on the ROM sizes so we don't try to hash all
     * 4 GB of address space */
    if(InitTop < SignBase || KernelBase < InitTop)
        return 0;
    
    /* SHA digest the ROM contents (top 128, then init table, then the rest)
     * and verify the signature */
    A_SHAInit(&SHAHash);
    A_SHAUpdate(&SHAHash, (PUCHAR)ROMRegions, SignTop - (PUCHAR)ROMRegions);
    A_SHAUpdate(&SHAHash, SignBase, InitTop - SignBase);
    A_SHAUpdate(&SHAHash, KernelBase, Signature - KernelBase);
    A_SHAFinal(&SHAHash, Digest);

    if(!VerifyDigest(PubKey, Signature, Digest))
        return 0;

#if 0
    PULONG pdwTimes = (PULONG)0x8F020;

    DbgInitialize();
    DbgPrint("Got to preldr!\n");

    DbgPrint("Before times: %08X %08X\n", pdwTimes[0], pdwTimes[1]);
    DbgPrint("After times: %08X %08X\n", pdwTimes[2], pdwTimes[3]);
#endif

    /* Now we decrypt the boot loader */

#ifdef MCP_XMODE3P
    /* Build the bldr key */
    A_SHAInit(&SHAHash);
    A_SHAUpdate(&SHAHash, SBKey, 16);
    A_SHAUpdate(&SHAHash, (PUCHAR)(0LU - ROM_DEC_SIZE - 16), 16);
    for(i = 0; i < 16; ++i)
        EncKey[i] = SBKey[i] ^ 0x5C;
    A_SHAUpdate(&SHAHash, EncKey, 16);
    A_SHAFinal(&SHAHash, EncKey);
    rc4_key(&RC4KeyStruct, sizeof EncKey, EncKey);
    memset(EncKey, 0, sizeof EncKey);
#else
    rc4_key(&RC4KeyStruct, sizeof BldrEncKey, BldrEncKey);
#endif

    EncBldr = (PUCHAR)0xFFFFFE00 - BLDR_BLOCK_SIZE;
    memcpy(DecryptAddress, EncBldr, BLDR_BLOCK_SIZE);
    rc4(&RC4KeyStruct, BLDR_BLOCK_SIZE, DecryptAddress);
    
    EntryAddress = ((PULONG)(DecryptAddress + BLDR_BLOCK_SIZE -
        PRELDR_BLOCK_SIZE))[-2];
    return (ULONG)DecryptAddress + EntryAddress;
}
