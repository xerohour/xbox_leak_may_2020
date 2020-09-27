//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include <windows.h>
#include <objbase.h>
#include <stdio.h>
#include <assert.h>
#include <xboxp.h>
#include <av.h>
#include "PerBoxData.h"
#include "cryptkeys.h"

VOID
WriteFileFromMemory(
    LPCSTR lpFileName,
    LPVOID pvFileContents,
    DWORD dwFileSize
    )
{
    HANDLE hFile;
    DWORD dwBytesWrote;

    hFile = CreateFile(lpFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "GenTest: error: unable to open %s\n", lpFileName);
        exit(1);
    }

    if (!WriteFile(hFile, pvFileContents, dwFileSize, &dwBytesWrote, NULL) ||
        (dwBytesWrote != dwFileSize)) {
        fprintf(stderr, "GenTest: error: cannot write to %s\n", lpFileName);
        exit(1);
    }

    CloseHandle(hFile);
}

void PrintUsage()
{
    printf("Usage: GenTest -l n -p\n");    
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
    DWORD GameRegion = XC_GAME_REGION_NA;
    DWORD AVRegion = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
    BYTE MACAddress[ETHERNET_MAC_ADDR_LEN] = { 0x00,0x50,0xF2,0x00,0x66,0x66 };
    char XboxSerialNumber[XBOX_SERIAL_NUMBER_LEN] = { '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' };
    char HardDriveKey[25];
    char RecoveryKey[RECOVERY_KEY_LEN];
    DWORD dwOnlineKeyVersion = 0;
    char OnlineKey[173];
    BYTE buffer[512];
    DWORD dwBufferSize = 0;
    DWORD tickCt;
    int i,n;
    int loop=1;
    BOOL pause = FALSE;

    n = 1;
    while (n < argc)
    {
       // loop a number of iterations
       if (_stricmp(argv[n], "-l") == 0)
       {
          if ((n+1) < argc)
          {
             loop = atoi(argv[++n]);
          }
          else
          {
             PrintUsage();
             return 1;
          }
       }
       // Pause between loops
       else if (_stricmp(argv[n], "-p") == 0)
       {
          if (n < argc)
          {
              pause = TRUE;
          }
          else
          {
              PrintUsage();
              return 1;
          }
       }
       
       ++n;
    }
    
    printf("Looping %d iterations\n", loop);
    tickCt = GetTickCount();

    MACAddress[4] = (BYTE)(tickCt & 0xff);
    MACAddress[5] = (BYTE)((tickCt>>8) & 0xfe);
    
    for (i=0; i<loop; ++i)
    {
        dwBufferSize = sizeof(buffer);
        hr = GeneratePerBoxData (
                GameRegion,
                AVRegion,
                MACAddress,
                XboxSerialNumber,
                HardDriveKey,
                RecoveryKey,
                &dwOnlineKeyVersion,
                OnlineKey,
                buffer,
                &dwBufferSize );
            
        if ( FAILED( hr ) )
        {
            printf("Failed GeneratePerBoxData - [%x]\n", hr);
            return 1;
        }
        
        if (pause)
        {
            printf("Finished iteration %d, hit enter for next iteration\n", i);
            getchar();
        }
    }
    
    printf("Took %d ms\n", GetTickCount() - tickCt);
    
    WriteFileFromMemory( "perBoxImage.dat", buffer, dwBufferSize );

    printf("HardDriveKey: %s\n", HardDriveKey);

    printf("RecoveryKey: %.8s\n", RecoveryKey);

    printf("dwOnlineKeyVersion: %d\n", dwOnlineKeyVersion);

    printf("OnlineKey: %s\n", OnlineKey);

    for (i=0; i<loop; ++i)
    {
        dwBufferSize = sizeof(buffer);
        hr = GeneratePerBoxDataEx (
                HardDriveKey,
                GameRegion,
                AVRegion,
                MACAddress,
                XboxSerialNumber,
                HardDriveKey,
                RecoveryKey,
                &dwOnlineKeyVersion,
                OnlineKey,
                buffer,
                &dwBufferSize );
            
        if ( FAILED( hr ) )
        {
            printf("Failed GeneratePerBoxData - [%x]\n", hr);
            return 1;
        }
        
        if (pause)
        {
            printf("Finished iteration %d, hit enter for next iteration\n", i);
            getchar();
        }
    }
    
    printf("Took %d ms\n", GetTickCount() - tickCt);
    
    WriteFileFromMemory( "perBoxImage.dat", buffer, dwBufferSize );

    printf("HardDriveKey: %s\n", HardDriveKey);

    printf("RecoveryKey: %.8s\n", RecoveryKey);

    printf("dwOnlineKeyVersion: %d\n", dwOnlineKeyVersion);

    printf("OnlineKey: %s\n", OnlineKey);

    return 0;
}
