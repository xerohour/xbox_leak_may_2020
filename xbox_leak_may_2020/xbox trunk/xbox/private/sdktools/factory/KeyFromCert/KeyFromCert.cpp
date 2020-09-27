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

#define PROVIDER MS_ENHANCED_PROV

#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)

//
// Print Usage
//
void PrintUsage(void)
{
    printf("Usage: KeyFromCert CertFileName PublicKeyFileName\n");
}

int __cdecl main(int argc, char *argv[])
{
    BOOL bResult;
    char* pszCertFile;
    char* pszKeyFile;
    DWORD dwSize;
    DWORD dwWritten,dwRead;

    HCRYPTPROV hProv = 0;
    UUID Uuid;
    
    PCCERT_CONTEXT pCertificateContext;

    HCRYPTKEY hPubKey = 0;
    HANDLE hFile;
    HANDLE hKeyFile;
    HRESULT hr;
    
    BYTE* pbExportedKey;
    BYTE* pbEncodedCert;
    LPWSTR szwContainer = NULL;
    RPC_STATUS Status;

    // Print Usage
    if (argc < 3)
    {
        PrintUsage();
        return 0;
    }

    // Set Certificate file Name
    pszCertFile = argv[1];
    pszKeyFile = argv[2];

    // Open Certificate file
    hFile = CreateFileA(pszCertFile,
                   GENERIC_READ,
                   0,
                   NULL,
                   OPEN_EXISTING,
                   FILE_ATTRIBUTE_NORMAL,
                   NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
       printf("Unable to open certificate file\n");
       goto ErrorExit;
    }

    // Get file length
    dwSize = GetFileSize(hFile, NULL);
    if (dwSize == 0xFFFFFFFF)
    {
       printf("Unable to get size of certificate file\n");
       goto ErrorExit;
    }

    // Allocate memory for encoded certificate
    pbEncodedCert = (LPBYTE)_alloca(dwSize);
    if (!pbEncodedCert)
    {
       printf("Unable to allocate memory for encoded certificate\n");
       goto ErrorExit;
    }

    // Read encoded certificate data
    bResult = ReadFile(hFile,
                    (LPVOID)pbEncodedCert,
                    dwSize,
                    &dwRead,
                    NULL);
    if (!bResult)
    {
       printf("Unable to read encoded certificate\n");
       goto ErrorExit;
    }

    assert( dwRead == dwSize );

    // Close file handle
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;

    {
        HCRYPTPROV hTemp;

        // Create Temporary Provider
        bResult = CryptAcquireContext(&hTemp,
                                      L"Temp000",
                                      PROVIDER,
                                      PROV_RSA_FULL,
                                      CRYPT_NEWKEYSET);
        if (!bResult)
        {
           // If Temporary Provider exists already,
           // open it
           hr = GetLastError();
           if (hr == NTE_EXISTS)
           {
              bResult = CryptAcquireContext(&hTemp,
                                          L"Temp000",
                                          PROVIDER,
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

        // Place random data in Uuid
        // Could have used UuidCreate but it is not supported
        // under Win9x.
        bResult = CryptGenRandom(hTemp, sizeof(Uuid), (LPBYTE)&Uuid);
        if (!bResult)
        {
           hr = GetLastError();
           printf("CryptGenRandom failed with %x\n", hr);
           goto ErrorExit;
        }

        // Close Provider handle
        CryptReleaseContext(hTemp, 0);

        // Delete Container
        CryptAcquireContext(&hTemp,
                           L"Temp000",
                           PROVIDER,
                           PROV_RSA_FULL,
                           CRYPT_DELETEKEYSET);

        // convert random uuid to a string, we will use it as a container
        Status = UuidToString(&Uuid, &szwContainer);
        if (Status != RPC_S_OK)
        {
            hr = Status;
            printf("Unable to convert uuid to string\n");
            goto ErrorExit;
        }
    }

    // Create new crypto context
    bResult = CryptAcquireContext(&hProv,
                            szwContainer,
                            PROVIDER,
                            PROV_RSA_FULL,
                            CRYPT_NEWKEYSET);
    if (!bResult)
    {
       hr = GetLastError();
       printf("CryptAcquireContext failed with %x\n", hr);
       goto ErrorExit;
    }

    pCertificateContext = CertCreateCertificateContext( X509_ASN_ENCODING, pbEncodedCert, dwRead );
    if (!pCertificateContext)
    {
        hr = GetLastError();
        printf("Failed CertCreateCertificateContext [%x]\n", hr);
        goto ErrorExit;
    }

	bResult = CryptImportPublicKeyInfoEx(hProv, ENCODING, &(pCertificateContext->pCertInfo->SubjectPublicKeyInfo), CALG_RSA_KEYX, 0, NULL, &hPubKey );
    if (!bResult)
    {
       hr = GetLastError();
       printf("CryptImportPublicKeyInfoEx failed with %x\n", hr);
       goto ErrorExit;
    }

    // Get size of exported key blob
    bResult = CryptExportKey(hPubKey, NULL, PUBLICKEYBLOB, 0, NULL, &dwSize);
    if (!bResult)
    {
          printf("CryptExportKey failed with %x\n", GetLastError());
          goto ErrorExit;
    }
      
    // Allocate memory for exported key blob
    pbExportedKey = (LPBYTE)_alloca(dwSize);
    if (!pbExportedKey)
    {
          printf("Unable to allocate memory for exported key\n");
          goto ErrorExit;
    }

    // Get exported key blob
    bResult = CryptExportKey(hPubKey, NULL, PUBLICKEYBLOB,
                               0, pbExportedKey, &dwSize);
    if (!bResult)
    {
          printf("CryptExportKey failed with %x\n", GetLastError());
          goto ErrorExit;
    }

    // Create Exported Key File
    hKeyFile = CreateFileA(pszKeyFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                            FILE_ATTRIBUTE_NORMAL, NULL);
    if (hKeyFile == INVALID_HANDLE_VALUE)
    {
          printf("CreateFile failed with %d\n", GetLastError());
          goto ErrorExit;
    }

    // Write exported key to file
    bResult = WriteFile(hKeyFile, (LPVOID)pbExportedKey, dwSize, &dwWritten, NULL);
    if (!bResult)
    {
          printf("WriteFile failed with %d\n", GetLastError());
          goto ErrorExit;
    }

    assert( dwWritten == dwSize );

    // Close file handle
    CloseHandle(hKeyFile);
    hKeyFile = INVALID_HANDLE_VALUE;

    printf("Succeeded\n");

    return 0;
    
ErrorExit:

    printf("Failed\n");

    return 1;
}
