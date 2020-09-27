//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include <windows.h>
#include <objbase.h>
#include <stdio.h>
#include <assert.h>
#include <wincrypt.h>
#include <cryptkeys.h>
#include <base64.h>

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

HCRYPTPROV g_hProv = 0;
HCRYPTKEY g_hPubKey = 0;

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

    assert( dwDataLen == ONLINE_KEY_PKENC_SIZE );

    *pdwOnlineKeyVersion = 1;
    
    hr = S_OK;
    
error:

    return hr;
}



void PrintUsage()
{
    printf("Usage: GenOnlineKey 16ByteKeyInHex\n");    
}

extern "C"
int
_cdecl
main(
    int argc,
    char** argv
    )
{
    HRESULT hr;

    DWORD GameRegion = 0;
    DWORD AVRegion = 0;
    BYTE pbMACAddress[ETHERNET_MAC_ADDR_LEN] = {0,0,0,0,0,0};
    char pcXboxSerialNumber[XBOX_SERIAL_NUMBER_LEN] = {'9','9','9','9','9','9','9','9','9','9','9','9'};
    DWORD dwOnlineKeyVersion;
    char pcOnlineKey[173];

    BOOL bResult;
    DWORD i;
    int byte;
    
    // Data and buffers
    XBOX_ENCRYPTED_PER_BOX_DATA output;
    BYTE pbOnlineKey[ONLINE_KEY_PKENC_SIZE];

    RtlZeroMemory( &output, sizeof(output) );

    if (argc != 2 || strlen(argv[1]) != 32)
    {
        PrintUsage();
        return 1;
    }

    for( i = 0; i < ONLINE_KEY_LEN; ++i )
    {
         sscanf(&(argv[1][i*2]), "%2x", &byte );
         output.OnlineKey[i] = (BYTE)byte;
    }
    
    for( i = 0; i < XBOX_SERIAL_NUMBER_LEN; ++i )
    {
        if (!isalnum(pcXboxSerialNumber[i]))
        {
            return E_INVALIDARG;
        }
    }
    
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
    bResult = CryptGenRandom(g_hProv, sizeof(output.HardDriveKey), (LPBYTE)&output.HardDriveKey);
    if (!bResult)
    {
        hr = GetLastError();
        printf("CryptGenRandom failed with %x\n", hr);
        goto error;
    }
    
    //
    // Encrypt the online key
    //
    hr = EncryptOnlineKey( output.XboxSerialNumber,
                           output.OnlineKey, output.HardDriveKey,
                           output.MACAddress, output.GameRegion, output.AVRegion,
                           &dwOnlineKeyVersion, pbOnlineKey );
    if ( FAILED( hr) )
    {
        goto error;
    }

    assert( BASE64_ENCODE_BUFFER_SIZE( ONLINE_KEY_PKENC_SIZE ) == 173 );
    bResult = Base64Encode( pbOnlineKey, ONLINE_KEY_PKENC_SIZE, pcOnlineKey );    

    //
    // Clear the temp output buffer
    //
    RtlZeroMemory( &output, sizeof(XBOX_ENCRYPTED_PER_BOX_DATA) );

    printf( pcOnlineKey );
    printf( "\n" );
    
    hr = S_OK;
    
error:

    return hr;
}
