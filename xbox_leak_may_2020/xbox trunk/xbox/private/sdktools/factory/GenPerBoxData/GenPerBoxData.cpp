//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include <windows.h>
#include <wincrypt.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "sha.h"
#include "rc4.h"
#include "PerBoxData.h"
#include "cryptKeys.h"
#include "base64.h"
#include "gpbdver.h"

//#define DETAILED_CHECKS 1

//#define DETAILED_TRACE 1

//#define DETAILED_LOG_FILE 1

//#define USE_TEST_KEY 1

//#define REPEAT_COUNT 1000

//////////////////////////////////////////////////////////////////////////
//
// SHA1 HMAC calculation
//
//////////////////////////////////////////////////////////////////////////
static void shaHmac(
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


#define RC4_CONFOUNDER_LEN 8

typedef struct _RC4_SHA1_HEADER {
    UCHAR Checksum[A_SHA_DIGEST_LEN];
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
} RC4_SHA1_HEADER, *PRC4_SHA1_HEADER;

static void rc4HmacEncrypt(
    IN PUCHAR confounder, // RC4_CONFOUNDER_LEN bytes
    IN PUCHAR pbKey,
    IN ULONG cbKey,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    OUT PUCHAR pbHeader // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes
    )
{
    PRC4_SHA1_HEADER CryptHeader = (PRC4_SHA1_HEADER) pbHeader;
    BYTE LocalKey[A_SHA_DIGEST_LEN];
    RC4_KEYSTRUCT Rc4KeyStruct;

    //
    // Create the header - the confounder & checksum
    //
    RtlZeroMemory( CryptHeader->Checksum, A_SHA_DIGEST_LEN );
    RtlCopyMemory( CryptHeader->Confounder, confounder, RC4_CONFOUNDER_LEN );

    //
    // Checksum everything but the checksum
    //
    shaHmac( pbKey, cbKey,
             CryptHeader->Confounder, RC4_CONFOUNDER_LEN,
             pbInput, cbInput,
             CryptHeader->Checksum );

    //
    // HMAC the checksum into the key
    //
    shaHmac( pbKey, cbKey,
             CryptHeader->Checksum, A_SHA_DIGEST_LEN,
             NULL, 0,
             LocalKey );

    //
    // Use the generated key as the RC4 encryption key
    //
    rc4_key( &Rc4KeyStruct, A_SHA_DIGEST_LEN, LocalKey );

    //
    // Encrypt everything but the checksum
    //
    rc4( &Rc4KeyStruct, RC4_CONFOUNDER_LEN, CryptHeader->Confounder );
    rc4( &Rc4KeyStruct, cbInput, pbInput );
}

static BOOL rc4HmacDecrypt(
    IN PUCHAR pbKey,
    IN ULONG cbKey,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    IN PUCHAR pbHeader // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes
    )
{
    PRC4_SHA1_HEADER CryptHeader = (PRC4_SHA1_HEADER) pbHeader;
    RC4_SHA1_HEADER TempHeader;
    BYTE Confounder[RC4_CONFOUNDER_LEN];
    BYTE LocalKey[A_SHA_DIGEST_LEN];
    RC4_KEYSTRUCT Rc4KeyStruct;

    RtlCopyMemory( TempHeader.Confounder, CryptHeader->Confounder, RC4_CONFOUNDER_LEN );

    //
    // HMAC the checksum into the key
    //
    shaHmac( pbKey, cbKey,
             CryptHeader->Checksum, A_SHA_DIGEST_LEN,
             NULL, 0,
             LocalKey );

    //
    // Use the generated key as the RC4 encryption key
    //
    rc4_key( &Rc4KeyStruct, A_SHA_DIGEST_LEN, LocalKey );

    //
    // Decrypt confounder and data
    //
    rc4( &Rc4KeyStruct, RC4_CONFOUNDER_LEN, TempHeader.Confounder );
    rc4( &Rc4KeyStruct, cbInput, pbInput );

    //
    // Now verify the checksum.
    //
    shaHmac( pbKey, cbKey,
             TempHeader.Confounder, RC4_CONFOUNDER_LEN,
             pbInput, cbInput,
             TempHeader.Checksum );

    //
    // Decrypt is successful only if checksum matches
    //
    return ( RtlEqualMemory(
                 TempHeader.Checksum,
                 CryptHeader->Checksum,
                 A_SHA_DIGEST_LEN) );
}

VOID AppendFileFromMemory(
    LPCSTR lpFileName,
    LPVOID pvFileContents,
    DWORD dwFileSize
    )
{
    HANDLE hFile;
    DWORD dwBytesWrote;
    DWORD dwPos;

    hFile = CreateFile(lpFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        printf("GeneratePerBoxData: unable to open %s, error %X\n", lpFileName, GetLastError());
        return;
    }

    dwPos = SetFilePointer( hFile, 0, NULL, FILE_END );
    if ( dwPos == INVALID_SET_FILE_POINTER )
    {
        printf("GeneratePerBoxData: unable to seek %s, error %X\n", lpFileName, GetLastError());
        return;        
    }

    if (!WriteFile(hFile, pvFileContents, dwFileSize, &dwBytesWrote, NULL) || (dwBytesWrote != dwFileSize))
    {
        printf("GeneratePerBoxData: cannot write to %s, error %X\n", lpFileName, GetLastError() );
        return;
    }

    CloseHandle(hFile);
}

#ifndef USE_TEST_KEY
//
// The private key matching this public key is in an nCipher box
//
BYTE g_PublicKey[] = {
    0x06, 0x02, 0x00, 0x00, 0x00, 0xa4, 0x00, 0x00, 0x52, 0x53, 0x41, 0x31, 0x00, 0x04, 0x00, 0x00,
    0x01, 0x00, 0x01, 0x00, 0xff, 0xb8, 0x5b, 0x38, 0x9a, 0x63, 0x9e, 0x72, 0x5c, 0x32, 0x85, 0x17,
    0x04, 0xe2, 0xdf, 0xe4, 0xc7, 0x13, 0xb5, 0xe6, 0x02, 0xd9, 0x2f, 0x3d, 0x24, 0x03, 0x43, 0xda,
    0x03, 0xf6, 0x9c, 0xcf, 0xaa, 0x7d, 0x9e, 0x6e, 0xc2, 0xb9, 0xe4, 0x76, 0x16, 0xf9, 0xe9, 0x4e,
    0x2c, 0x6c, 0xf3, 0x1b, 0x95, 0x3c, 0x94, 0x0f, 0x69, 0x5d, 0xdd, 0x09, 0xfd, 0x27, 0x57, 0xa7,
    0x95, 0x02, 0xc6, 0xfb, 0x4d, 0x96, 0xb4, 0x5e, 0x8b, 0xfa, 0xc1, 0x34, 0x60, 0xac, 0x06, 0xbb,
    0xa9, 0x85, 0x56, 0xbb, 0xeb, 0x40, 0xf3, 0xb2, 0xb7, 0xc3, 0x28, 0x75, 0x3d, 0xb9, 0x53, 0x26,
    0x9b, 0x1e, 0x49, 0x48, 0x80, 0x43, 0xc9, 0x09, 0xb2, 0xa3, 0x20, 0x49, 0x28, 0x21, 0xe4, 0x31,
    0xb1, 0xad, 0x55, 0x9e, 0xce, 0x7c, 0xa5, 0x97, 0xee, 0xe1, 0x26, 0xe9, 0xbc, 0x13, 0x4f, 0x1e,
    0xa0, 0x5d, 0xa2, 0xbe };
#else
BYTE g_PublicKey[] = {
    0x06, 0x02, 0x00, 0x00, 0x00, 0xa4, 0x00, 0x00, 0x52, 0x53, 0x41, 0x31, 0x00, 0x04, 0x00, 0x00,
    0x01, 0x00, 0x01, 0x00, 0xd7, 0xe4, 0x7a, 0x30, 0xe5, 0x8e, 0x1e, 0x87, 0xc8, 0xda, 0x7f, 0xae,
    0x22, 0xb5, 0x20, 0x35, 0x26, 0x78, 0x17, 0x11, 0x8d, 0xb6, 0xd7, 0xb1, 0xaa, 0xdb, 0x1f, 0x54,
    0xc8, 0x0f, 0xca, 0x38, 0x56, 0x21, 0x88, 0x4b, 0x9d, 0x60, 0x4a, 0x8e, 0x36, 0xff, 0x81, 0x72,
    0x62, 0x00, 0xe0, 0xad, 0xaa, 0x9e, 0x22, 0x44, 0xd9, 0xcb, 0x0b, 0xbd, 0x7a, 0xf3, 0x87, 0x5f,
    0x01, 0x76, 0x9e, 0x3f, 0xcf, 0x0c, 0x07, 0x3e, 0xe8, 0xaf, 0xa1, 0xe1, 0xdf, 0x6b, 0x6a, 0x38,
    0x77, 0x60, 0x42, 0x6e, 0xee, 0x10, 0x0a, 0x9e, 0xd8, 0x5c, 0x45, 0x59, 0x29, 0xe0, 0x21, 0x47,
    0x4c, 0x6a, 0xbf, 0xfc, 0x72, 0xfd, 0xdb, 0xa2, 0x64, 0x6e, 0x59, 0x1d, 0x36, 0x54, 0x28, 0xf1,
    0x8f, 0xca, 0xa7, 0x5d, 0x03, 0xaf, 0x31, 0x07, 0xbc, 0x47, 0x89, 0x5e, 0x6e, 0x44, 0x01, 0x3b,
    0xed, 0x0f, 0xd2, 0xd2 };

BYTE g_PrivateKey[] = {
    0x07, 0x02, 0x00, 0x00, 0x00, 0xa4, 0x00, 0x00, 0x52, 0x53, 0x41, 0x32, 0x00, 0x04, 0x00, 0x00,
    0x01, 0x00, 0x01, 0x00, 0xd7, 0xe4, 0x7a, 0x30, 0xe5, 0x8e, 0x1e, 0x87, 0xc8, 0xda, 0x7f, 0xae,
    0x22, 0xb5, 0x20, 0x35, 0x26, 0x78, 0x17, 0x11, 0x8d, 0xb6, 0xd7, 0xb1, 0xaa, 0xdb, 0x1f, 0x54,
    0xc8, 0x0f, 0xca, 0x38, 0x56, 0x21, 0x88, 0x4b, 0x9d, 0x60, 0x4a, 0x8e, 0x36, 0xff, 0x81, 0x72,
    0x62, 0x00, 0xe0, 0xad, 0xaa, 0x9e, 0x22, 0x44, 0xd9, 0xcb, 0x0b, 0xbd, 0x7a, 0xf3, 0x87, 0x5f,
    0x01, 0x76, 0x9e, 0x3f, 0xcf, 0x0c, 0x07, 0x3e, 0xe8, 0xaf, 0xa1, 0xe1, 0xdf, 0x6b, 0x6a, 0x38,
    0x77, 0x60, 0x42, 0x6e, 0xee, 0x10, 0x0a, 0x9e, 0xd8, 0x5c, 0x45, 0x59, 0x29, 0xe0, 0x21, 0x47,
    0x4c, 0x6a, 0xbf, 0xfc, 0x72, 0xfd, 0xdb, 0xa2, 0x64, 0x6e, 0x59, 0x1d, 0x36, 0x54, 0x28, 0xf1,
    0x8f, 0xca, 0xa7, 0x5d, 0x03, 0xaf, 0x31, 0x07, 0xbc, 0x47, 0x89, 0x5e, 0x6e, 0x44, 0x01, 0x3b,
    0xed, 0x0f, 0xd2, 0xd2, 0xdd, 0x54, 0xef, 0x5d, 0xf9, 0xb1, 0x7b, 0xc5, 0x89, 0x69, 0x2a, 0x5f,
    0xbc, 0x42, 0x29, 0xec, 0xd2, 0x59, 0x6e, 0x1d, 0x82, 0x98, 0x87, 0x32, 0x93, 0xff, 0x44, 0xd5,
    0x6f, 0x01, 0x92, 0x81, 0x67, 0x3b, 0x3e, 0xe7, 0x27, 0xf8, 0xdf, 0x69, 0x33, 0xd9, 0x4f, 0xd1,
    0xf5, 0xda, 0xae, 0x77, 0x11, 0xa8, 0x31, 0xdc, 0x7f, 0x40, 0x47, 0x14, 0x04, 0xc6, 0x4e, 0x2e,
    0x3c, 0x41, 0xf0, 0xf9, 0x43, 0xfb, 0x7f, 0x2d, 0x4a, 0xe8, 0xb0, 0x7c, 0x78, 0x18, 0xec, 0x8e,
    0x96, 0xf9, 0xea, 0x9d, 0x83, 0x6f, 0x9f, 0x14, 0xc6, 0x37, 0x52, 0x07, 0x37, 0x57, 0xa7, 0x30,
    0x64, 0xa8, 0x32, 0xe6, 0xb0, 0xea, 0xb6, 0x98, 0x3d, 0x54, 0x73, 0x5e, 0x44, 0xcf, 0x15, 0x61,
    0x21, 0xee, 0x42, 0x6e, 0x50, 0x33, 0xba, 0x74, 0xae, 0x37, 0xb0, 0x3b, 0xde, 0x71, 0xc0, 0xdd,
    0x7e, 0xf1, 0xee, 0xd7, 0x9d, 0x8b, 0xa7, 0x0f, 0x90, 0xd6, 0x23, 0xbc, 0xe0, 0x5a, 0x93, 0x42,
    0x08, 0xf9, 0x0a, 0xa4, 0x03, 0x33, 0xbe, 0x4c, 0xe1, 0xbf, 0xf7, 0x54, 0xf5, 0x57, 0xf6, 0x57,
    0x1c, 0xbb, 0x79, 0xec, 0x0a, 0x71, 0x14, 0x64, 0x9c, 0xb4, 0xab, 0xd3, 0x74, 0x1a, 0x4c, 0xd2,
    0x57, 0x1d, 0xac, 0x4d, 0x10, 0xc3, 0xa2, 0xea, 0x33, 0x1b, 0x33, 0x32, 0xb2, 0x66, 0xab, 0x68,
    0x24, 0xea, 0xf1, 0xf1, 0xef, 0x1a, 0xef, 0xf7, 0x90, 0xb1, 0x2c, 0xf4, 0x1d, 0x7e, 0xd4, 0x1b,
    0x1f, 0x96, 0x14, 0x81, 0xa6, 0x48, 0x0d, 0xa3, 0x88, 0x07, 0x8e, 0x93, 0xfe, 0x63, 0xd0, 0x19,
    0x70, 0xbc, 0x09, 0xbb, 0x19, 0xcb, 0x04, 0xf9, 0x6f, 0xc1, 0x6b, 0x07, 0x09, 0xf0, 0x9c, 0x7b,
    0xb7, 0x77, 0x9d, 0xae, 0x07, 0x82, 0xfc, 0x89, 0x0e, 0x08, 0x68, 0x3f, 0x42, 0x21, 0x36, 0x22,
    0xf7, 0xb1, 0x65, 0xa5, 0xd8, 0xd8, 0x06, 0x50, 0x9e, 0xf3, 0xef, 0xbf, 0xf9, 0x29, 0xf2, 0xa1,
    0x89, 0x7a, 0xc9, 0x90, 0xea, 0x73, 0xfe, 0x71, 0xfb, 0x3a, 0x01, 0xa3, 0xe4, 0xae, 0x1d, 0x8e,
    0x51, 0x07, 0xfb, 0xd2, 0x40, 0xe2, 0x87, 0x49, 0x8d, 0xc1, 0xc0, 0x92, 0x77, 0x9a, 0x2d, 0xbb,
    0xff, 0x06, 0xbd, 0xb1, 0xb8, 0xb2, 0xae, 0x6d, 0x03, 0xcb, 0x0e, 0xbb, 0x47, 0x1d, 0x9c, 0x66,
    0xc2, 0xa6, 0xb7, 0x5d, 0xe9, 0x33, 0x23, 0x3a, 0xe2, 0xca, 0x45, 0xb2, 0x06, 0x82, 0x6b, 0x51,
    0x32, 0xa2, 0x06, 0x77, 0xbd, 0xc4, 0x40, 0x47, 0x83, 0x5c, 0x89, 0xd3, 0x6a, 0x59, 0x0b, 0xc2,
    0x7f, 0x3b, 0xba, 0x3c, 0x46, 0x0d, 0xd9, 0x71, 0x62, 0x24, 0x8d, 0x40, 0xe5, 0x68, 0x4e, 0x68,
    0x7a, 0xa7, 0x3e, 0xbd, 0x09, 0x33, 0x54, 0x7e, 0x1f, 0xe6, 0x50, 0x28, 0x83, 0x29, 0x25, 0x13,
    0x5a, 0x3b, 0xe0, 0xa4, 0x89, 0x98, 0xaa, 0x51, 0xe7, 0x49, 0x20, 0xef, 0x7e, 0xe5, 0xcc, 0x94,
    0x53, 0x84, 0xfd, 0xd0, 0x13, 0xd0, 0x28, 0x9e, 0xfe, 0x37, 0xc8, 0x96, 0x24, 0x1a, 0xa3, 0x91,
    0x6d, 0x28, 0xcb, 0xee, 0xa5, 0xf5, 0xa0, 0x70, 0xc3, 0xb0, 0xf6, 0xca, 0xe3, 0xc4, 0xa8, 0x2e,
    0xaf, 0xde, 0xe4, 0x7e, 0x6b, 0x93, 0x62, 0xa2, 0x02, 0x1e, 0xf4, 0x38, 0x5d, 0x97, 0x3b, 0x87,
    0x28, 0x81, 0x35, 0x61 };
#endif

HCRYPTPROV g_hProv = 0;
HCRYPTKEY g_hPubKey = 0;
#ifdef USE_TEST_KEY
HCRYPTKEY g_hPrvKey = 0;
#endif

static HRESULT getProviderAndKey()
{
    BOOL bResult;
    HRESULT hr;

    if ( g_hProv )
    {
        return S_OK;
    }
    
    CryptAcquireContext( &g_hProv, "XboxTemp001", MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_DELETEKEYSET);
    g_hProv = 0;

    //
    // Create a temporay provider context to import the keys
    //
    bResult = CryptAcquireContext(&g_hProv,
                               "XboxTemp001",
                               MS_ENHANCED_PROV,
                               PROV_RSA_FULL,
                               CRYPT_NEWKEYSET);
    if (!bResult)
    {
        hr = GetLastError();
        printf("Unable to create temporary provider handle\n");
        g_hProv = 0;
        return hr;
    }

    //
    // Import the public key
    //
    bResult = CryptImportKey(g_hProv, g_PublicKey, sizeof(g_PublicKey), NULL, 0, &g_hPubKey);
    if (!bResult)
    {
        hr = GetLastError();
        printf("Failed CryptImportKey [%x]\n", hr);
        CryptReleaseContext( g_hProv, 0 );
        g_hPubKey = 0;
        return hr;
    }

#ifdef USE_TEST_KEY
    //
    // Import the private key
    //
    bResult = CryptImportKey(g_hProv, g_PrivateKey, sizeof(g_PrivateKey), NULL, 0, &g_hPrvKey);
    if (!bResult)
    {
        hr = GetLastError();
        printf("Failed CryptImportKey [%x]\n", hr);
        CryptReleaseContext( g_hProv, 0 );
        g_hPrvKey = 0;
        return hr;
    }
#endif

    return S_OK;
}

HRESULT EncryptOnlineKey (
    IN char* pcXboxSerialNumber,
    IN LPBYTE pbOnlineKey,
    IN LPBYTE pbHardDriveKey,
    IN LPBYTE pbMACAddress,
    IN DWORD GameRegion,
    IN DWORD AVRegion,
    OUT DWORD* pdwOnlineKeyVersion,
    OUT LPBYTE pbEncOnlineKey
    )
{
    BOOL bResult;
    HRESULT hr;
    
    XBOX_ENCRYPTED_ONLINE_DATA EncData;
    DWORD dwDataLen;
    
    RtlZeroMemory( &EncData, sizeof(EncData) );
    RtlCopyMemory( EncData.XboxSerialNumber, pcXboxSerialNumber, XBOX_SERIAL_NUMBER_LEN );
    RtlCopyMemory( EncData.OnlineKey, pbOnlineKey, ONLINE_KEY_LEN );
    RtlCopyMemory( EncData.HardDriveKey, pbHardDriveKey, HARD_DRIVE_KEY_LEN );
    RtlCopyMemory( EncData.MACAddress, pbMACAddress, ETHERNET_MAC_ADDR_LEN );
    EncData.GameRegion = GameRegion;
    EncData.AVRegion = AVRegion;

#ifdef REPEAT_COUNT
    DWORD dwStart = GetTickCount();
    int i;
    for (i=0; i<REPEAT_COUNT; ++i) {
#endif

    RtlZeroMemory( pbEncOnlineKey, ONLINE_KEY_PKENC_SIZE );
    RtlCopyMemory( pbEncOnlineKey, &EncData, sizeof( XBOX_ENCRYPTED_ONLINE_DATA ) );
    
    //
    // Encrypt the online key with the public key.
    //
    dwDataLen = sizeof(XBOX_ENCRYPTED_ONLINE_DATA);
    bResult = CryptEncrypt( g_hPubKey, NULL, TRUE, 0, pbEncOnlineKey, &dwDataLen, ONLINE_KEY_PKENC_SIZE );
    if(!bResult)
    {
        hr = GetLastError();
        printf("Error %x during CryptEncrypt! Bytes required:%lu\n", hr, dwDataLen);
        goto error;
    }

#ifdef REPEAT_COUNT
    }
    printf("CryptEncrypt %d times took %d milliseconds\n", REPEAT_COUNT, GetTickCount() - dwStart);
#endif

    assert( dwDataLen == ONLINE_KEY_PKENC_SIZE );

#if defined(DETAILED_CHECKS) && defined(USE_TEST_KEY)
    //
    // Decrypt the online key with the private key.
    //
    BYTE buffer[ONLINE_KEY_PKENC_SIZE];
    RtlCopyMemory( buffer, pbEncOnlineKey, ONLINE_KEY_PKENC_SIZE );
    
    dwDataLen = ONLINE_KEY_PKENC_SIZE;
    bResult = CryptDecrypt( g_hPrvKey, NULL, TRUE, 0, buffer, &dwDataLen );
    if(!bResult)
    {
        hr = GetLastError();
        printf("Error %x during CryptDecrypt! Bytes output:%lu\n", hr, dwDataLen);
        goto error;
    }
    
    assert( dwDataLen == sizeof(XBOX_ENCRYPTED_ONLINE_DATA) );

    assert( RtlEqualMemory( buffer, &EncData, sizeof( XBOX_ENCRYPTED_ONLINE_DATA ) ) );

#endif

    *pdwOnlineKeyVersion = 1;
    
    hr = S_OK;
    
error:

    return hr;
}


HRESULT GeneratePerBoxData (
    IN DWORD GameRegion,
    IN DWORD AVRegion,
    IN LPBYTE pbMACAddress, // ETHERNET_MAC_ADDR_LEN bytes
    IN char* pcXboxSerialNumber, // XBOX_SERIAL_NUMBER_LEN bytes
    OUT char* pcHardDriveKey,
    OUT char* pcRecoveryKey, // RECOVERY_KEY_LEN bytes
    OUT DWORD* pdwOnlineKeyVersion,
    OUT char* pcOnlineKey,
    OUT LPBYTE pbOutputBuffer,
    IN OUT DWORD* pdwOutputBufferSize
    )
{
    return GeneratePerBoxDataEx(
                NULL, // generate a random one
                GameRegion,
                AVRegion,
                pbMACAddress, // ETHERNET_MAC_ADDR_LEN bytes
                pcXboxSerialNumber, // XBOX_SERIAL_NUMBER_LEN bytes
                pcHardDriveKey,
                pcRecoveryKey, // RECOVERY_KEY_LEN bytes
                pdwOnlineKeyVersion,
                pcOnlineKey,
                pbOutputBuffer,
                pdwOutputBufferSize
                );
}


HRESULT GeneratePerBoxDataEx (
    IN char* pcInputHardDriveKey,
    IN DWORD GameRegion,
    IN DWORD AVRegion,
    IN LPBYTE pbMACAddress, // ETHERNET_MAC_ADDR_LEN bytes
    IN char* pcXboxSerialNumber, // XBOX_SERIAL_NUMBER_LEN bytes
    OUT char* pcHardDriveKey,
    OUT char* pcRecoveryKey, // RECOVERY_KEY_LEN bytes
    OUT DWORD* pdwOnlineKeyVersion,
    OUT char* pcOnlineKey,
    OUT LPBYTE pbOutputBuffer,
    IN OUT DWORD* pdwOutputBufferSize
    )
{
    HRESULT hr;
    BOOL bResult;
    DWORD i;
    
    // Data and buffers
    XBOX_ENCRYPTED_PER_BOX_DATA output;
    BYTE Confounder[RC4_CONFOUNDER_LEN];
    BYTE pbOnlineKey[ONLINE_KEY_PKENC_SIZE];

    BYTE decodeBuffer[256];
    DWORD dwSize;
    
    if ( pbOutputBuffer == NULL )
    {
        *pdwOutputBufferSize = sizeof(XBOX_ENCRYPTED_PER_BOX_DATA) + sizeof(RC4_SHA1_HEADER);      
        return S_OK;
    }
    if ( *pdwOutputBufferSize < sizeof(XBOX_ENCRYPTED_PER_BOX_DATA) + sizeof(RC4_SHA1_HEADER) )
    {
        return E_INVALIDARG;
    }
    *pdwOutputBufferSize = sizeof(XBOX_ENCRYPTED_PER_BOX_DATA) + sizeof(RC4_SHA1_HEADER);

    for( i = 0; i < XBOX_SERIAL_NUMBER_LEN; ++i )
    {
        if (!isalnum(pcXboxSerialNumber[i]))
        {
            return E_INVALIDARG;
        }
    }
    
    RtlZeroMemory( &output, sizeof(output) );
    output.GameRegion = GameRegion;
    output.AVRegion = AVRegion;
    RtlCopyMemory( output.MACAddress, pbMACAddress, sizeof(output.MACAddress) );
    RtlCopyMemory( output.XboxSerialNumber, pcXboxSerialNumber, sizeof(output.XboxSerialNumber) );

    //
    // Get Crypto API provider and public key.
    //
    hr = getProviderAndKey();
    if ( FAILED( hr) )
    {
        goto error;
    }

    //
    // Generate the random bytes we need
    //
    if (pcInputHardDriveKey)
    {
        //
        // Use supplied key if one exists
        //
        bResult = Base64Decode( pcInputHardDriveKey, 24, decodeBuffer, &dwSize );
        if( !bResult || dwSize != sizeof(output.HardDriveKey) )
        {
            printf("Input Hard Drive Key failed to decode\n");
            goto error;
        }
        RtlCopyMemory( (LPBYTE)&output.HardDriveKey, decodeBuffer, sizeof(output.HardDriveKey) );
    }
    else
    {
        bResult = CryptGenRandom(g_hProv, sizeof(output.HardDriveKey), (LPBYTE)&output.HardDriveKey);
        if (!bResult)
        {
            hr = GetLastError();
            printf("CryptGenRandom failed with %x\n", hr);
            goto error;
        }
    }
    
    bResult = CryptGenRandom(g_hProv, sizeof(output.OnlineKey), (LPBYTE)&output.OnlineKey);
    if (!bResult)
    {
        hr = GetLastError();
        printf("CryptGenRandom failed with %x\n", hr);
        goto error;
    }

    bResult = CryptGenRandom(g_hProv, sizeof(Confounder), (LPBYTE)&Confounder);
    if (!bResult)
    {
        hr = GetLastError();
        printf("CryptGenRandom failed with %x\n", hr);
        goto error;
    }

    //
    // Package bytes to be sent to WritePerBoxData
    //
    RtlCopyMemory( pbOutputBuffer + sizeof(RC4_SHA1_HEADER), &output, sizeof(XBOX_ENCRYPTED_PER_BOX_DATA) );
    rc4HmacEncrypt(
        Confounder,
        (LPBYTE)FACTORY_RAND_KEY, FACTORY_RAND_KEY_LEN,
        pbOutputBuffer + sizeof(RC4_SHA1_HEADER), sizeof(XBOX_ENCRYPTED_PER_BOX_DATA),
        pbOutputBuffer );

    //
    // Encrypt the online key
    //
    hr = EncryptOnlineKey( output.XboxSerialNumber,
                           output.OnlineKey, output.HardDriveKey,
                           output.MACAddress, output.GameRegion, output.AVRegion,
                           pdwOnlineKeyVersion, pbOnlineKey );
    if ( FAILED( hr) )
    {
        goto error;
    }

    assert( BASE64_ENCODE_BUFFER_SIZE( ONLINE_KEY_PKENC_SIZE ) == 173 );
    bResult = Base64Encode( pbOnlineKey, ONLINE_KEY_PKENC_SIZE, pcOnlineKey );    

    assert( BASE64_ENCODE_BUFFER_SIZE( sizeof(output.HardDriveKey) ) == 25 );
    bResult = Base64Encode( output.HardDriveKey, sizeof(output.HardDriveKey), pcHardDriveKey );    

#ifdef DETAILED_CHECKS
    //
    // Double check if decode gets back the same thing
    //
    bResult = Base64Decode( pcOnlineKey, 172, decodeBuffer, &dwSize );    
    assert( dwSize == ONLINE_KEY_PKENC_SIZE );
    assert( RtlEqualMemory( decodeBuffer, pbOnlineKey, dwSize ) );

    bResult = Base64Decode( pcHardDriveKey, 24, decodeBuffer, &dwSize );   
    assert( dwSize == sizeof(output.HardDriveKey) );
    assert( RtlEqualMemory( decodeBuffer, output.HardDriveKey, dwSize ) );
#endif

    //
    // Compute recovery key
    //
    ComputeRecoveryKey( output.HardDriveKey, pcRecoveryKey );

#ifdef DETAILED_TRACE
    //
    // Print out the keys
    //
    printf("OnlineKey: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
        output.OnlineKey[0], output.OnlineKey[1],
        output.OnlineKey[2], output.OnlineKey[3],
        output.OnlineKey[4], output.OnlineKey[5],
        output.OnlineKey[6], output.OnlineKey[7],
        output.OnlineKey[8], output.OnlineKey[9],
        output.OnlineKey[10], output.OnlineKey[11],
        output.OnlineKey[12], output.OnlineKey[13],
        output.OnlineKey[14], output.OnlineKey[15]
        );

    printf("HardDriveKey: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
        output.HardDriveKey[0], output.HardDriveKey[1],
        output.HardDriveKey[2], output.HardDriveKey[3],
        output.HardDriveKey[4], output.HardDriveKey[5],
        output.HardDriveKey[6], output.HardDriveKey[7],
        output.HardDriveKey[8], output.HardDriveKey[9],
        output.HardDriveKey[10], output.HardDriveKey[11],
        output.HardDriveKey[12], output.HardDriveKey[13],
        output.HardDriveKey[14], output.HardDriveKey[15]
        );

    printf("RecoveryKey: %.8s\n", pcRecoveryKey);

    printf("MACAddress: %.6s\n", output.MACAddress);

    printf("XboxSerialNumber: %.12s\n", output.XboxSerialNumber);
    
    printf("GameRegion: %X\n", output.GameRegion);

    printf("AVRegion: %X\n", output.AVRegion);
#endif

    //
    // Clear the temp output buffer
    //
    RtlZeroMemory( &output, sizeof(XBOX_ENCRYPTED_PER_BOX_DATA) );

#ifdef DETAILED_LOG_FILE
    char buffer[1024];

    sprintf( buffer, "%.12s %08X %08X %02X:%02X:%02X:%02X:%02X:%02X %s %.8s %02X %s\r\n",
        pcXboxSerialNumber, GameRegion, AVRegion,
        pbMACAddress[0], pbMACAddress[1], pbMACAddress[2],
        pbMACAddress[3], pbMACAddress[4], pbMACAddress[5],
        pcHardDriveKey,
        pcRecoveryKey,
        *pdwOnlineKeyVersion,
        pcOnlineKey );
        
    AppendFileFromMemory( "C:\\GeneratePerBoxData_log.txt", buffer, strlen(buffer) );
#endif

    hr = S_OK;
    
error:

    return hr;
}


