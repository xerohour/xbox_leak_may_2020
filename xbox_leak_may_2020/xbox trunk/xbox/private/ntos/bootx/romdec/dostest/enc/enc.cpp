/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    rombld.cpp

Abstract:


--*/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <xcrypt.h>
#include <bldr.h>


static BYTE EncKey[] = 

    {
           0x57,0x42,0x29,0x0C,0x30,0x1E,0xD3,0x01,
           0xB3,0xE5,0x5D,0x28,0x50,0x31,0xE1,0xCE
    };




void
BuildFiles(
    PBYTE pData,
    DWORD dwDataSize
    )
{

    char szOrgFile[MAX_PATH];
    char szEncFile[MAX_PATH];
    HANDLE hWriteFile = INVALID_HANDLE_VALUE;
    DWORD dwBytes;
    PBYTE pEncBuffer = NULL;
    DWORD dwEncBufSize;
    DWORD dwFileNum = 0;
    PBYTE p = pData;
    DWORD dwLeft = dwDataSize;
    DWORD dwCopySize;

    //
    // allocate buffer for encryption
    //
    dwEncBufSize = BLDR_BLOCK_SIZE;
    pEncBuffer = (PBYTE)malloc(dwEncBufSize);


    printf("File size set to boot loader filesize (%d)\n", BLDR_BLOCK_SIZE);

    for (;;)
    {

        sprintf(szOrgFile, "%d.ORG", dwFileNum);
        sprintf(szEncFile, "%d.ENC", dwFileNum);
        dwFileNum++;

        printf("Creating %s and %s files\n", szOrgFile, szEncFile);


        dwCopySize = BLDR_BLOCK_SIZE;
        if (dwCopySize >= dwLeft)
        {
            dwCopySize = dwLeft;
            dwLeft = 0;
            memset(pEncBuffer, 0, dwEncBufSize);
        }
        else
        {
            dwLeft -= dwCopySize;
        }
        memcpy(pEncBuffer, p, dwCopySize);
        p += dwCopySize;


        hWriteFile = CreateFile(szOrgFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hWriteFile == INVALID_HANDLE_VALUE) {
            printf("Failed to open output file\n");
            goto Cleanup;
        }
        if (!WriteFile(hWriteFile, pEncBuffer, dwEncBufSize, &dwBytes, NULL))
        {
            printf("Failed to write to output file\n");
            goto Cleanup;
        }
        CloseHandle(hWriteFile);

        
        //
        // encrypt the buffer
        //
        XCSymmetricEncDec(pEncBuffer, dwEncBufSize, EncKey, 16);

        hWriteFile = CreateFile(szEncFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hWriteFile == INVALID_HANDLE_VALUE) {
            printf("Failed to open output file\n");
            goto Cleanup;
        }
        if (!WriteFile(hWriteFile, pEncBuffer, dwEncBufSize, &dwBytes, NULL))
        {
            printf("Failed to write to output file\n");
            goto Cleanup;
        }
        CloseHandle(hWriteFile);

        hWriteFile = INVALID_HANDLE_VALUE;

        if (dwLeft == 0)
        {
            break;
        }

    }
        

Cleanup:
    if (hWriteFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hWriteFile);
    }

    if (pEncBuffer != NULL)
    {
        free(pEncBuffer);
    }
 
}





extern "C"
int
_cdecl
main(
	int,
	char** argv
	)
{
	HANDLE hFile;
	DWORD dwBytes;
    DWORD dwSize;
    PBYTE p;

    hFile = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        dwSize = GetFileSize(hFile, NULL);
        if (dwSize > 0)
        {
            p = (PBYTE)malloc(dwSize);
            if (p != NULL)
            {
                if (ReadFile(hFile, p, dwSize, &dwBytes, NULL))
                {
                    BuildFiles(p, dwSize);
                }
            }

            free(p);
        }
        CloseHandle(hFile);
    }

    return 0;
    
}



