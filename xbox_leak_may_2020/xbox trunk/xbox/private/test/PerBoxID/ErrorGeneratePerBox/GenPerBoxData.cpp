//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "PerBoxData.h"


HRESULT GeneratePerBoxData (
    IN DWORD GameRegion,
    IN DWORD AVRegion,
    IN LPBYTE pbMACAddress, // ETHERNET_MAC_ADDR_LEN bytes
    IN char* pcXboxSerialNumber, // XBOX_SERIAL_NUMBER_LEN bytes
    OUT char* pcHardDriveKey, // HARD_DRIVE_KEY_LEN bytes
    OUT char* pcRecoveryKey, // RECOVERY_KEY_LEN bytes
    OUT DWORD* pdwOnlineKeyVersion,
    OUT char* pcOnlineKey, // ONLINE_KEY_PKENC_SIZE bytes
    OUT LPBYTE pbOutputBuffer,
    IN OUT DWORD* pdwOutputBufferSize
    )
{

/*

        return S_OK;
        return E_INVALIDARG;
            return E_INVALIDARG;
    hr = getProviderAndKey();
    if ( FAILED( hr) )
    {
        goto error;
    }

        hr = GetLastError();
        printf("CryptGenRandom failed with %x\n", hr);
        goto error;

        hr = GetLastError();
        printf("CryptGenRandom failed with %x\n", hr);
        goto error;

        hr = GetLastError();
        printf("CryptGenRandom failed with %x\n", hr);
        goto error;
    hr = EncryptOnlineKey( output.XboxSerialNumber,
                           output.OnlineKey, output.HardDriveKey,
                           output.MACAddress, output.GameRegion, output.AVRegion,
                           pdwOnlineKeyVersion, pbOnlineKey );
    if ( FAILED( hr) )
    {
        goto error;
    }

    hr = S_OK;
    
error:

    return hr;

*/
        return S_OK;
}


