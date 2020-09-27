//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include <windows.h>
#include <wincrypt.h>
#include <rpc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include "base64.h"
#include "sha.h"
#include "cryptkeys.h"

#define PROVIDER L"nCipher Enhanced Cryptographic Provider"

//
// This is for the nCipher byte reversal bug. When we install with a new
// nCipher CSP that fixes this bug, this define will need to be removed.
//
#define NCIPHER_BYTE_REVERSAL_BUG 1

//#define USE_TEST_KEY 1

#define REPEAT_COUNT 1

#ifdef USE_TEST_KEY
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

//
// Print Usage
//
void PrintUsage(void)
{
    printf("Usage: DecryptTest base64String\n");
}

int __cdecl main(int argc, char *argv[])
{
    BOOL bResult;
    char* pszBase64String;

    HCRYPTPROV hProv = 0;
    HCRYPTKEY hPrvKey = 0;
    HRESULT hr;

    BYTE buffer[256];
    char buffer1[64];
    BYTE buffer2[64];
    DWORD dwSize = 0;
    PXBOX_ENCRYPTED_ONLINE_DATA pData;
    char pcRecoveryKey[16];
    DWORD dwStart,dwEnd;
    int i;
    
    // Print Usage
    if (argc < 2)
    {
        PrintUsage();
        return 0;
    }

    dwStart = GetTickCount();
    printf("CryptDecrypt starting...\n");

#ifndef USE_TEST_KEY
    // open crypto context with the private key
    bResult = CryptAcquireContext(&hProv,
                            L"XboxOnlineKeyV1",
                            PROVIDER,
                            PROV_RSA_FULL,
                            CRYPT_MACHINE_KEYSET);
    if (!bResult)
    {
       hr = GetLastError();
       printf("CryptAcquireContext failed with %x\n", hr);
       goto ErrorExit;
    }

    dwEnd = GetTickCount();
    printf("CryptAcquireContext took %d milliseconds\n", dwEnd - dwStart);
    dwStart = dwEnd;

	bResult = CryptGetUserKey(hProv, AT_KEYEXCHANGE, &hPrvKey );
    if (!bResult)
    {
       hr = GetLastError();
       printf("CryptGetUserKey failed with %x\n", hr);
       goto ErrorExit;
    }
    
    dwEnd = GetTickCount();
    printf("CryptGetUserKey took %d milliseconds\n", dwEnd - dwStart);
    dwStart = dwEnd;
#else
    //
    // Create a temporay provider context to import the keys
    //
    bResult = CryptAcquireContext(&hProv,
                               L"XboxTemp002",
                               MS_ENHANCED_PROV,
                               PROV_RSA_FULL,
                               CRYPT_NEWKEYSET);
    if (!bResult)
    {
       // If Temporary Provider exists already,
       // open it
       if (GetLastError() == NTE_EXISTS)
       {
          bResult = CryptAcquireContext(&hProv,
                                      L"XboxTemp002",
                                      MS_ENHANCED_PROV,
                                      PROV_RSA_FULL,
                                      0);
          if (!bResult)
          {
             printf("Unable to get temporary provider handle\n");
             goto ErrorExit;
          }
       }
       else
       {
          printf("Unable to create temporary provider handle\n");
          goto ErrorExit;
       }
    }
    
    //
    // Import the private key
    //
    bResult = CryptImportKey(hProv, g_PrivateKey, sizeof(g_PrivateKey), NULL, 0, &hPrvKey);
    if (!bResult)
    {
        hr = GetLastError();
        printf("Failed CryptImportKey [%x]\n", hr);
        goto ErrorExit;
    }
#endif

    pszBase64String = argv[1];

    for (i=0; i<REPEAT_COUNT; ++i) {

        bResult = Base64Decode( pszBase64String, strlen(pszBase64String), buffer, &dwSize );
        assert( bResult );
        assert( dwSize == ONLINE_KEY_PKENC_SIZE );

        //
        // Decrypt the online key with the private key.
        //
        bResult = CryptDecrypt( hPrvKey, NULL, TRUE, 0, buffer, &dwSize );
        if(!bResult)
        {
            hr = GetLastError();
            printf("Error %x during CryptDecrypt! Bytes output:%lu\n", hr, dwSize);
            goto ErrorExit;
        }
    
    }

    dwEnd = GetTickCount();
    printf("CryptDecrypt %d times took %d milliseconds\n", REPEAT_COUNT, dwEnd - dwStart);
    dwStart = dwEnd;

    assert( dwSize == sizeof(XBOX_ENCRYPTED_ONLINE_DATA) );

#if !defined(USE_TEST_KEY) && defined(NCIPHER_BYTE_REVERSAL_BUG)
    BYTE t,*p,*q;
    p = &buffer[0];
    q = &buffer[dwSize-1];
    while ( q > p )
    {
        t = *p;
        *p = *q;
        *q = t;
        ++p;
        --q;
    }
#endif

    pData = (PXBOX_ENCRYPTED_ONLINE_DATA)&buffer[0];
    
    //
    // Print out the keys
    //
    printf("OnlineKey: %02x%02x\n",
        pData->OnlineKey[0], pData->OnlineKey[1]
        );

    printf("HardDriveKey: %02x%02x\n",
        pData->HardDriveKey[0], pData->HardDriveKey[1]
        );

    bResult = Base64Encode( pData->HardDriveKey, HARD_DRIVE_KEY_LEN, buffer1 );
    assert( bResult );
    printf("HardDriveKey encoded: %s\n", buffer1);

    bResult = Base64Decode( buffer1, strlen(buffer1), buffer2, &dwSize );
    assert( bResult );
    assert( dwSize == HARD_DRIVE_KEY_LEN );
    assert( memcmp( buffer2, pData->HardDriveKey, dwSize ) == 0 );
    
    //
    // Compute recovery key
    //
    ComputeRecoveryKey( pData->HardDriveKey, pcRecoveryKey );

    printf("RecoveryKey: %.8s\n", pcRecoveryKey);

    printf("MACAddress: %02x%02x%02x%02x%02x%02x\n",
        pData->MACAddress[0], pData->MACAddress[1],
        pData->MACAddress[2], pData->MACAddress[3],
        pData->MACAddress[4], pData->MACAddress[5]
        );

    printf("XboxSerialNumber: %.12s\n", pData->XboxSerialNumber);
    
    printf("GameRegion: %X\n", pData->GameRegion);

    printf("AVRegion: %X\n", pData->AVRegion);

    dwEnd = GetTickCount();
    printf("printing took %d milliseconds\n", dwEnd - dwStart);
    dwStart = dwEnd;

    CryptReleaseContext( hProv, 0 );

    dwEnd = GetTickCount();
    printf("CryptReleaseContext took %d milliseconds\n", dwEnd - dwStart);
    dwStart = dwEnd;

    return 0;
    
ErrorExit:

    CryptReleaseContext( hProv, 0 );

    printf("Failed\n");

    return 1;
}


//////////////////////////////////////////////////////////////////////////
//
// SHA1 HMAC calculation
//
//////////////////////////////////////////////////////////////////////////
void shaHmac(
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

