//===================================================================
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright 1998-1999. Microsoft Corporation. All Right Reserved.
// File: CreateCert.c
//
// This sample demonstrates how to create Certificates using CryptoAPI.
// The sample lets the user create Self-Signed Certificates or
// create Certificates signed by another Certificate.
//
// When a Self-Signed Certificate is created a file called
// SelfSigned.cer is created.  The file can be ran to trust
// the certificate as a Certificate Authority.
//
// When a non Self-Signed Certificate is created a file called
// Certificate.cer is created.  This can be used on Windows 2000
// to install the certificate under the "AddressBook" store for
// example.  Only the public key information is stored in the
// certificate.
//
//===================================================================
#include <windows.h>
#include <wincrypt.h>
#include <rpc.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "CreateCert.h"


#define KEY_USAGE_SIZE 5

// This is 1024 bits
#define PK_KEY_SIZE 0x04000000

// This is 2048 bits
//#define PK_KEY_SIZE 0x08000000

#define REPEAT_LOOP 1
#define VERIFY_LOOP 1000
#define SIGN_LOOP 1000

#define PROVIDER MS_ENHANCED_PROV
//#define PROVIDER MS_DEF_PROV

#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)

// This structure will be used to calculate
// the Enhanced Key Usage Extension from an
// integer
typedef struct tagEnhKeyUsage
{
   DWORD dwCount;
   LPSTR szUsage[KEY_USAGE_SIZE];
   DWORD dwValue[KEY_USAGE_SIZE];
} ENHKEYUSAGE;

// Every Enhanced Usage Extension has a
// corresponding number to it.  Just add
// the number to set the appropriate
// extension:
// Server Authentication   1
// Client Authentication   2
// Code Signing            4
// Email Protection        8
// Time Stamping           16
ENHKEYUSAGE g_EnhKeyUsage =
{
   KEY_USAGE_SIZE,
   {
      szOID_PKIX_KP_SERVER_AUTH,
      szOID_PKIX_KP_CLIENT_AUTH,
      szOID_PKIX_KP_CODE_SIGNING,
      szOID_PKIX_KP_EMAIL_PROTECTION,
      szOID_PKIX_KP_TIMESTAMP_SIGNING
   },
   { 1, 2, 4, 8, 16}
};

LPSTR g_szSigAlg[] = {
   szOID_RSA_MD5RSA,
   szOID_RSA_SHA1RSA
};

// Default store and empty string
LPSTR g_szMyStore = "MY";
LPSTR g_szEmpty = "";

HRESULT VerifyKeyPair( LPBYTE pbEncodedCert, DWORD dwCertSize, HCRYPTPROV hPrvProv );

int __cdecl main(int argc, char *argv[])
{
   BOOL bResult;
   DWORD dwUsage = 0;
   DWORD dwKeyType = AT_KEYEXCHANGE;
   DWORD dwProvNum = 0;
   LPSTR szSigAlg = g_szSigAlg[1];
   BOOL bExport = FALSE;
   BOOL bSelfSigned = TRUE;
   BOOL bIssuerUser = TRUE;
   BOOL bSubjectUser = TRUE;
   BOOL bUseProv = FALSE;
   LPSTR szX509 = NULL;
   LPSTR szSubjectStore = g_szMyStore;
   LPSTR szIssuerStore = g_szMyStore;
   LPSTR szIssuerName = g_szEmpty;
   LPSTR szCertFile = g_szEmpty;
   LPSTR szKeyFile = g_szEmpty;
   LPSTR szPassword = g_szEmpty;
   WORD wMonths = 0;
   BOOL bCA = FALSE;
   int n;
   int i;
   DWORD dwStart;
   
   // Print Usage
   if (argc < 2)
   {
     PrintUsage();
     return 0;
   }


   // User wants to import a certificate
   if (_stricmp(argv[1], "-i") == 0)
   {
      BOOL bUser;
      DWORD num;

      if (argc != 8)
      {
         PrintUsage();
         return 0;
      }

      if (_stricmp(argv[6], "m") == 0)
         bUser = FALSE;
      else
         bUser = TRUE;

      num = (DWORD)atoi(argv[7]);

      bResult = ImportCertificate(argv[2],
                                argv[3],
                                argv[4],
                                argv[5],
                                bUser,
                                num);
      if (!bResult)
      {
         printf("Unable to Import Certificate\n");
         return 0;
      }
      else
      {
         printf("Certificate Imported successfully\n");
         return 1;
      }
   }

   // Set X509 Name
   szX509 = argv[1];

   // Parse rest of the options
   n = 2;
   while (n < argc)
   {
    // User selected Key Type
    if (_stricmp(argv[n], "-k") == 0)
    {
      if ((n+1) < argc)
      {
         if (_stricmp(argv[++n], "s") == 0)
           dwKeyType = AT_SIGNATURE;
         else
           dwKeyType = AT_KEYEXCHANGE;
      }
      else
      {
         PrintUsage();
         return 0;
      }
    }

    // User set number of months
    if (_stricmp(argv[n], "-m") == 0)
    {
       if ((n+1) < argc)
       {
          wMonths = (WORD)atoi(argv[++n]);
       }
       else
       {
          PrintUsage();
          return 0;
       }
    }

    // User wants this to be a CA.  Indicated in
    // Basic contraints
    if (_stricmp(argv[n], "-ca") == 0)
    {
       bCA = TRUE;
    }

    // User wants to set Enhanced Key usage extension
    if (_stricmp(argv[n], "-u") == 0)
    {
      if ((n+1) < argc)
      {
         dwUsage = (DWORD)atoi(argv[++n]);
      }
      else
      {
         PrintUsage();
         return 0;
      }
    }

    // User wants to set the Crypto Provider
    if (_stricmp(argv[n], "-p") == 0)
    {
       if ((n+1) < argc)
       {
          bUseProv = TRUE;
          dwProvNum = (DWORD)atoi(argv[++n]);
       }
       else
       {
          PrintUsage();
          return 0;
       }
    }

    // User selected signature algorithm
    if (_stricmp(argv[n], "-s") == 0)
    {
       if ((n+1) < argc)
       {
          if (_stricmp(argv[++n], "md5") == 0)
             szSigAlg = g_szSigAlg[0];
          else
             szSigAlg = g_szSigAlg[1];
       }
       else
       {
          PrintUsage();
          return 0;
       }
    }

    // User selected the Subject Location
    if (_stricmp(argv[n], "-sl") == 0)
    {
       if ((n+2) < argc)
       {
          bExport = FALSE;
          szSubjectStore = argv[++n];
          if (_stricmp(argv[++n], "m") == 0)
             bSubjectUser = FALSE;
          else
             bSubjectUser = TRUE;
       }
       else
       {
          PrintUsage();
          return 0;
       }
    }

    // User wants to export the Certificate
    if (_stricmp(argv[n], "-ex") == 0)
    {
       if ((n+3) < argc)
       {
          bExport = TRUE;
          szCertFile = argv[++n];
          szKeyFile = argv[++n];
          szPassword = argv[++n];
       }
       else
       {
          PrintUsage();
          return 0;
       }
    }

    // User has selected an issuer
    if (_stricmp(argv[n], "-is") == 0)
    {
       if ((n+3) < argc)
       {
          bSelfSigned = FALSE;
          szIssuerName = argv[++n];
          szIssuerStore = argv[++n];
          if (_stricmp(argv[++n], "m") == 0)
             bIssuerUser = FALSE;
          else
             bIssuerUser = TRUE;
       }
       else
       {
          PrintUsage();
          return 0;
       }
    }

    n++;
   }

   dwStart = GetTickCount();

   for (i=0; i<REPEAT_LOOP; ++i)
   {
       // Create Certificate
       bResult = CreateCertificate(
                         szX509,        // Certificate Name
                         dwKeyType,     // Key Type
                         szSigAlg,      // Signature Algorithm
                         wMonths,       // Number of Months
                         dwUsage,       // Enhanced Key Usage
                         bCA,           // CA or not
                         bUseProv,      // Use dwProvNum
                         dwProvNum,     // Provider Number
                         szSubjectStore,// Certificate Store
                         bSubjectUser,  // User or Machine flag
                         bSelfSigned,   // Self Signed flag
                         szIssuerName,  // Issuer of Certificate
                         szIssuerStore, // Store of Issuer
                         bIssuerUser,   // User or Machine flag
                         bExport,       // Export flag
                         szCertFile,    // Certificate File
                         szKeyFile,     // Key File
                         szPassword);   // Password for Key File
       if (!bResult)
       {
         printf("Unable to create certificate\n");
       }
   }

   printf("Creating %d certificates took %d milliseconds\n", REPEAT_LOOP, GetTickCount() - dwStart);

   return 1;
}

//
// ImportCertificate installs the crpto provider and certificate
// on the machine.
//
BOOL ImportCertificate(LPSTR szCertFile, LPSTR szKeyFile, LPSTR szPassword,
                  LPSTR szStore, BOOL bUser, DWORD dwProviderNum)
{
   LPWSTR szwContainer = NULL;
   HCRYPTPROV hProv = 0;
   HCRYPTHASH hHash = 0;
   HCERTSTORE hStore = NULL;
   HCRYPTKEY hKey = 0;
   HCRYPTKEY hPubKey = 0;
   HANDLE hFile = INVALID_HANDLE_VALUE;
   LPBYTE pbEncodedCert = NULL;
   LPBYTE pbEncodedKey = NULL;
   PCCERT_CONTEXT pCertContext = NULL;
   CRYPT_KEY_PROV_INFO CertKeyInfo;
   CERT_KEY_CONTEXT certKeyContext;
   BOOL fReturn = FALSE;
   RPC_STATUS Status;
   HANDLE hHeap;
   UUID Uuid;
   BOOL bResult;
   DWORD dwSize, dwRead;
   WCHAR szwStore[200];
   WCHAR szwCertFile[200];
   WCHAR szwKeyFile[200];
   DWORD dwAcquireFlags = 0;
   DWORD dwCertOpenFlags = CERT_SYSTEM_STORE_CURRENT_USER;
   WCHAR szwProvider[260] = { PROVIDER };
   DWORD dwProviderType = PROV_RSA_FULL;
   HRESULT hr;
   int i;

   // Get process heap
   hHeap = GetProcessHeap();

   __try
   {
     if (!bUser)
     {
        dwAcquireFlags = CRYPT_MACHINE_KEYSET;
        dwCertOpenFlags = CERT_SYSTEM_STORE_LOCAL_MACHINE;
     }

     dwSize = 260;
     bResult = MyCryptEnumProviders(dwProviderNum,
                                    &dwProviderType,
                                    szwProvider,
                                    &dwSize);
     if (!bResult)
     {
        printf("Unable to get provider\n");
        __leave;
     }

     // Convert multibyte store name to wide char string
     i = MultiByteToWideChar(CP_ACP, 0, szKeyFile, -1, szwKeyFile, 200);
     if (i == 0)
     {
       printf("Unable to convert szKeyFile to Unicode string\n");
       __leave;
     }

     // Open Key File
     hFile = CreateFile(szwKeyFile,
                      GENERIC_READ,
                      0,
                      NULL,
                      OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL,
                      NULL);
     if (hFile == INVALID_HANDLE_VALUE)
     {
         printf("Unable to open key file\n");
         __leave;
     }

     // Get file length
     dwSize = GetFileSize(hFile, NULL);
     if (dwSize == 0xFFFFFFFF)
     {
       printf("Unable to get size of certificate file\n");
       __leave;
     }

     // Allocate memory for encoded key
     pbEncodedKey = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
     if (!pbEncodedKey)
     {
       printf("Unable to allocate memory for encoded key\n");
       __leave;
     }

     // Read encoded key data
     bResult = ReadFile(hFile,
                    (LPVOID)pbEncodedKey,
                    dwSize,
                    &dwRead,
                    NULL);
     if (!bResult)
     {
       printf("Unable to read encoded key\n");
       __leave;
     }

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
           if (GetLastError() == NTE_EXISTS)
           {
              bResult = CryptAcquireContext(&hTemp,
                                          L"Temp000",
                                          PROVIDER,
                                          PROV_RSA_FULL,
                                          0);
              if (!bResult)
              {
                 printf("Unable to get temporary provider handle\n");
                 __leave;
              }
           }
           else
           {
              printf("Unable to create temporary provider handle\n");
              __leave;
           }
        }

        // Place random data in Uuid
        // Could have used UuidCreate but it is not supported
        // under Win9x.
        bResult = CryptGenRandom(hTemp, sizeof(Uuid), (LPBYTE)&Uuid);
        if (!bResult)
        {
           printf("CryptGenRandom failed with %x\n", GetLastError());
           __leave;
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
          printf("Unable to convert uuid to string\n");
          __leave;
        }
     }

     // Create new crypto context
     bResult = CryptAcquireContext(&hProv,
                            szwContainer,
                            szwProvider,
                            dwProviderType,
                            CRYPT_NEWKEYSET | dwAcquireFlags);
     if (!bResult)
     {
       printf("CryptAcquireContext failed with %x\n", GetLastError());
       __leave;
     }

     if (_stricmp( szPassword, "null" ) != 0)
     {
         // Create Hash to hash password
         bResult = CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash);
         if (!bResult)
         {
           printf("CryptCreateHash failed with %x\n", GetLastError());
           __leave;
         }

         // Hash Password
         bResult = CryptHashData(hHash, (LPBYTE)szPassword, strlen(szPassword), 0);
         if (!bResult)
         {
           printf("CryptHashData failed with %x\n", GetLastError());
           __leave;
         }

         // Derive Session Key from password hash
         bResult = CryptDeriveKey(hProv, CALG_RC4, hHash, CRYPT_EXPORTABLE, &hKey);
         if (!bResult)
         {
           printf("CryptDeriveKey failed with %x\n", GetLastError());
           __leave;
         }
     }

     // Import Private/Public Key into Provider
     bResult = CryptImportKey(hProv, pbEncodedKey, dwSize, hKey,
                        CRYPT_EXPORTABLE, &hPubKey);
     if (!bResult)
     {
       printf("CryptImportKey failed with %x\n", GetLastError());
       __leave;
     }

     // Convert multibyte store name to wide char string
     i = MultiByteToWideChar(CP_ACP, 0, szCertFile, -1, szwCertFile, 200);
     if (i == 0)
     {
       printf("Unable to convert szCertFile to Unicode string\n");
       __leave;
     }
     
     // Open Certificate file
     hFile = CreateFile(szwCertFile,
                   GENERIC_READ,
                   0,
                   NULL,
                   OPEN_EXISTING,
                   FILE_ATTRIBUTE_NORMAL,
                   NULL);
     if (hFile == INVALID_HANDLE_VALUE)
     {
       printf("Unable to open certificate file\n");
       __leave;
     }

     // Get file length
     dwSize = GetFileSize(hFile, NULL);
     if (dwSize == 0xFFFFFFFF)
     {
       printf("Unable to get size of certificate file\n");
       __leave;
     }

     // Allocate memory for encoded certificate
     pbEncodedCert = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
     if (!pbEncodedCert)
     {
       printf("Unable to allocate memory for encoded certificate\n");
       __leave;
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
       __leave;
     }

     // Close file handle
     CloseHandle(hFile);
     hFile = INVALID_HANDLE_VALUE;

	 //
	 // Verify that the key pair matches
	 //
     hr = VerifyKeyPair( pbEncodedCert, dwSize, hProv );
	 if ( hr == S_OK )
     {
		printf("VerifyKeyPair succeeded\n");
     }
		
     // Convert multibyte store name to wide char string
     i = MultiByteToWideChar(CP_ACP, 0, szStore, -1, szwStore, 200);
     if (i == 0)
     {
       printf("Unable to convert store to Unicode string\n");
       __leave;
     }

     // Open Certificate Store
     hStore = CertOpenStore(CERT_STORE_PROV_SYSTEM,
                      ENCODING,
                      0,
                      dwCertOpenFlags,
                      (LPVOID)szwStore);
     if (!hStore)
     {
       printf("CertOpenStore failed with %x\n", GetLastError());
       __leave;
     }

     // Add Certificate to store
     bResult = CertAddEncodedCertificateToStore(hStore,
                                     X509_ASN_ENCODING,
                                     pbEncodedCert,
                                     dwSize,
                                     CERT_STORE_ADD_REPLACE_EXISTING,
                                     &pCertContext);
     if (!bResult)
     {
       printf("CertAddEncodedCertificateToStore failed with %x\n", GetLastError());
       __leave;
     }

     {
        ZeroMemory(&CertKeyInfo, sizeof(CertKeyInfo));
        CertKeyInfo.pwszContainerName = szwContainer;
        CertKeyInfo.pwszProvName = szwProvider;
        CertKeyInfo.dwProvType = dwProviderType;

        CertKeyInfo.dwFlags = CERT_SET_KEY_CONTEXT_PROP_ID;
        CertKeyInfo.cProvParam = 0;
        CertKeyInfo.rgProvParam = NULL;
        CertKeyInfo.dwFlags |= CERT_SET_KEY_CONTEXT_PROP_ID;

        dwSize = sizeof(CertKeyInfo.dwKeySpec);
        bResult = CryptGetKeyParam(hPubKey, KP_ALGID, (LPBYTE)&CertKeyInfo.dwKeySpec,
                            &dwSize, 0);
        if (!bResult)
        {
          printf("CryptGetKeyParam failed with %x\n", GetLastError());
          __leave;
        }

        // Set Key property for Certificate
        bResult = CertSetCertificateContextProperty(pCertContext,
                                         CERT_KEY_PROV_INFO_PROP_ID,
                                         0,
                                         (LPVOID)&CertKeyInfo);
        if (!bResult)
        {
          printf("CertSetCertificateContextProperty failed with %x\n", GetLastError());
          __leave;
        }
        
        certKeyContext.cbSize = sizeof(CERT_KEY_CONTEXT);
        certKeyContext.hCryptProv = hProv;
        certKeyContext.dwKeySpec = CertKeyInfo.dwKeySpec;

        bResult = CertSetCertificateContextProperty(
                    pCertContext,
                    CERT_KEY_CONTEXT_PROP_ID,
                    0,
                    (void *)&certKeyContext);
        if (!bResult)
        {
          printf("CertSetCertificateContextProperty CERT_KEY_CONTEXT_PROP_ID failed with %x\n", GetLastError());
          __leave;
        }
     }

     fReturn = TRUE;
   }
   _finally
   {
     // Clean up
     if (szwContainer) RpcStringFree(&szwContainer);
     if (hHash) CryptDestroyHash(hHash);
     if (hKey) CryptDestroyKey(hKey);
     if (hPubKey) CryptDestroyKey(hPubKey);
     if (hProv) CryptReleaseContext(hProv, 0);
     if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
     if (pbEncodedCert) HeapFree(hHeap, 0, pbEncodedCert);
     if (pbEncodedKey) HeapFree(hHeap, 0, pbEncodedKey);
     if (pCertContext) CertFreeCertificateContext(pCertContext);
     if (hStore) CertCloseStore(hStore, 0);
   }

   return fReturn;
}

BOOL CreateCertificate(LPSTR szX509, DWORD dwKeyType,
                       LPSTR szSigAlg, WORD wMonths,
                       DWORD dwUsage, BOOL bCA, BOOL bUseProv,
                       DWORD dwProviderNum, LPSTR szSubjectStore,
                       BOOL bSubjectUser, BOOL bSelfSigned,
                       LPSTR szIssuerName, LPSTR szIssuerStore,
                       BOOL bIssuerUser, BOOL bExport,
                       LPSTR szCertFile, LPSTR szKeyFile,
                       LPSTR szPassword)
{
   BOOL bReturn = FALSE;
   BOOL bResult;
   LPBYTE pbNameBlob = NULL;
   LPWSTR szwContainer = NULL;
   RPC_STATUS Status;
   HCRYPTPROV hProv = 0;
   HCRYPTPROV hIssuerProv = 0;
   HCRYPTKEY hPubKey = 0;
   HCRYPTKEY hSessionKey = 0;
   HCRYPTHASH hHash = 0;
   HCERTSTORE hStore = 0;
   HANDLE hCertFile = INVALID_HANDLE_VALUE;
   HANDLE hKeyFile = INVALID_HANDLE_VALUE;
   PCRYPT_DATA_BLOB KeyId = NULL;
   PCERT_PUBLIC_KEY_INFO PublicKeyInfo = NULL;
   PCCERT_CONTEXT pIssuerCert = NULL;
   PCCERT_CONTEXT pCertContext = NULL;
   LPBYTE pbKeyIdentifier = NULL;
   LPBYTE SubjectKeyIdentifier = NULL;
   LPBYTE pbKeyUsage = NULL;
   LPBYTE pbEnhKeyUsage = NULL;
   LPBYTE pbBasicConstraints = NULL;
   LPBYTE pbAuthorityKeyId = NULL;
   LPBYTE bpEncodedCert = NULL;
   LPBYTE pbExportedKey = NULL;
   CERT_ENHKEY_USAGE CertEnhKeyUsage = { 0, NULL };
   BYTE SerialNumber[8];
   CERT_BASIC_CONSTRAINTS2_INFO BasicConstraints;
   CERT_AUTHORITY_KEY_ID_INFO AuthorityKeyId;
   BYTE ByteData;
   CRYPT_BIT_BLOB KeyUsage;
   CERT_EXTENSION CertExtension[5];
   CRYPT_DATA_BLOB CertKeyIdentifier;
   CERT_NAME_BLOB IssuerName;
   HANDLE hHeap = GetProcessHeap();
   CERT_INFO CertInfo;
   DWORD dwSize, m, q;
   DWORD dwAcquireFlags, dwIssuerKeyType = 0;
   DWORD dwIssuerFlags, dwSubjectFlags;
   FILETIME ftTime;
   SYSTEMTIME stTime;
   WCHAR szwCertFile[200];
   WCHAR szwKeyFile[200];
   WCHAR szwX509[2000];
   WCHAR szwProvider[260] = { PROVIDER };
   DWORD dwProviderType = PROV_RSA_FULL;
   BOOL bAddAuthorityExtension = FALSE;
   WCHAR szwStoreContainer[20];
   DWORD dwStart;
   int i;
   
   __try
   {
      // Enumerate Provider
      if (bUseProv)
      {
         dwSize = 260;
         bResult = MyCryptEnumProviders(
                           dwProviderNum,
                           &dwProviderType,
                           szwProvider,
                           &dwSize);
         if (!bResult)
         {
            printf("Unable to get provider\n");
            __leave;
         }

      }

      if (bSubjectUser)
      {
         // Certificate will be in the User store and
         // key container will be a User container
         dwAcquireFlags = 0;
         dwSubjectFlags = CERT_SYSTEM_STORE_CURRENT_USER;
      }
      else
      {
         // Certificate will be in the Machine store and
         // key container will be a Mahince container
         dwAcquireFlags = CRYPT_MACHINE_KEYSET;
         dwSubjectFlags = CERT_SYSTEM_STORE_LOCAL_MACHINE;
      }

      if (bIssuerUser)
      {
         // Get Issuer Certificate from User store
         dwIssuerFlags = CERT_SYSTEM_STORE_CURRENT_USER;
      }
      else
      {
         // Get Issuer Certificate from Machine store
         dwIssuerFlags = CERT_SYSTEM_STORE_LOCAL_MACHINE;
      }

      // Convert multibyte store name to wide char string
   	  i = MultiByteToWideChar(CP_ACP, 0, szX509, -1, szwX509, 2000);
      if (i == 0)
   	  {
	       printf("Unable to convert szX509 to Unicode string\n");
	       __leave;
      }
     
      // Get X509 Name and convert it to a Name Blob
      bResult = CertStrToName(X509_ASN_ENCODING,
                          szwX509,
                          CERT_X500_NAME_STR,
                          NULL,
                          NULL,
                          &dwSize,
                          NULL);
      if (!bResult)
      {
         printf("CertStrToName failed with %x\n", GetLastError());
         __leave;
      }

      // Allocate memory for Name Blob
      pbNameBlob = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
      if (!pbNameBlob)
      {
         printf("Unable to allocate memory for name blob\n");
         __leave;
      }

      // Convert X509 Name to Name Blob
      bResult = CertStrToName(X509_ASN_ENCODING,
                          szwX509,
                          CERT_X500_NAME_STR,
                          NULL,
                          (LPBYTE)pbNameBlob,
                          &dwSize,
                          NULL);
      if (!bResult)
      {
         printf("CertStrToName failed with %x\n", GetLastError());
         __leave;
      }

      IssuerName.cbData = dwSize;
      IssuerName.pbData = pbNameBlob;

      // Convert Enhanced Key usage number to
      // Enhanced Key Usage extension

      // Find Enhanced Key Usage extensions count
      for (m = 0; m < g_EnhKeyUsage.dwCount; m++)
      {
        if ((g_EnhKeyUsage.dwValue[m] & dwUsage) == g_EnhKeyUsage.dwValue[m])
        {
          CertEnhKeyUsage.cUsageIdentifier++;
        }
      }

      // If extensions exist continue
      if (CertEnhKeyUsage.cUsageIdentifier != 0)
      {
         // Allocate memory for Enhanced Key Usage array
         CertEnhKeyUsage.rgpszUsageIdentifier =
             (LPSTR *)HeapAlloc(hHeap, 0, CertEnhKeyUsage.cUsageIdentifier * sizeof(LPSTR));
         if (!CertEnhKeyUsage.rgpszUsageIdentifier)
         {
           printf("Unable to allocate memory for Enhanced Usage array\n");
           __leave;
         }

         // Initialize Enhanced Key Usage array to NULL
         for (m = 0; m < CertEnhKeyUsage.cUsageIdentifier; m++)
         {
           CertEnhKeyUsage.rgpszUsageIdentifier[m] = NULL;
         }

         // Add proper extension OID to array
         q = 0;
         for (m = 0; m < g_EnhKeyUsage.dwCount; m++)
         {
           if ((g_EnhKeyUsage.dwValue[m] & dwUsage) == g_EnhKeyUsage.dwValue[m])
           {
             CertEnhKeyUsage.rgpszUsageIdentifier[q++] = g_EnhKeyUsage.szUsage[m];
           }
         }
      }

      // Convert Store string to unicode
      i = MultiByteToWideChar(0, 0, szSubjectStore, -1, szwStoreContainer, 20);
      if (i == 0)
      {
          printf("MultiByteToWideChar failed with %d\n", GetLastError());
          __leave;
      }

      // Create Crypto Context

      {
         UUID Uuid;
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
            if (GetLastError() == NTE_EXISTS)
            {
               bResult = CryptAcquireContext(&hTemp,
                                          L"Temp000",
                                          PROVIDER,
                                          PROV_RSA_FULL,
                                          0);
               if (!bResult)
               {
                  printf("Unable to get temporary provider handle\n");
                  __leave;
               }
            }
            else
            {
               printf("Unable to create temporary provider handle\n");
               __leave;
            }
         }

         // Place random data in Uuid
         // Could have used UuidCreate but it is not supported
         // under Win9x.
         bResult = CryptGenRandom(hTemp, sizeof(Uuid), (LPBYTE)&Uuid);
         if (!bResult)
         {
            printf("CryptGenRandom failed with %x\n", GetLastError());
            __leave;
         }

         // Close Provider handle
         CryptReleaseContext(hTemp, 0);

         // Delete Container
         CryptAcquireContext(&hTemp,
                           L"Temp000",
                           PROVIDER,
                           PROV_RSA_FULL,
                           CRYPT_DELETEKEYSET);

         // Create a random uuid
         /*Status = UuidCreate(&Uuid);
         if (Status != RPC_S_OK)
         {
            printf("Unable to create random container\n");
            __leave;
         }*/

         // convert random uuid to a string, we will use it as a container
         Status = UuidToString(&Uuid, &szwContainer);
         if (Status != RPC_S_OK)
         {
            printf("Unable to convert uuid to string\n");
            __leave;
         }

         // Create new crypto context
         bResult = CryptAcquireContext(&hProv,
                            szwStoreContainer, //szwContainer,
                            szwProvider,
                            dwProviderType,
                            CRYPT_NEWKEYSET | dwAcquireFlags);
         if (!bResult)
         {
            printf("CryptAcquireContext failed with %x\n", GetLastError());
            __leave;
         }
      }

      // Generate Private/Public key pair
      dwStart = GetTickCount();
      bResult = CryptGenKey(hProv, dwKeyType, PK_KEY_SIZE, &hPubKey);
      //printf("CryptGenKey took %d milliseconds\n", GetTickCount() - dwStart);
      if (!bResult)
      {
         printf("CryptGenKey failed with %x\n", GetLastError());
         __leave;
      }

      // Zero CERT_INFO structure
      ZeroMemory(&CertInfo, sizeof(CertInfo));

      // Set Version of Certificate
      CertInfo.dwVersion = CERT_V3;

      // Create Random Serial Number
      bResult = CryptGenRandom(hProv, 8, SerialNumber);
      if (!bResult)
      {
         printf("CryptGenRandom failed with %x\n", GetLastError());
         __leave;
      }

      // Set Serial Number of Certificate
      CertInfo.SerialNumber.cbData = 8;
      CertInfo.SerialNumber.pbData = SerialNumber;

      // Set Signature Algorithm of Certificate
      CertInfo.SignatureAlgorithm.pszObjId = szSigAlg;
      CertInfo.SignatureAlgorithm.Parameters.cbData = 0;
      CertInfo.SignatureAlgorithm.Parameters.pbData = NULL;

      // set NotBefore date
      GetSystemTime(&stTime);
      SystemTimeToFileTime(&stTime, &ftTime);
      CertInfo.NotBefore = ftTime;

      if (wMonths == 0)
      {
         if (bSelfSigned)
            wMonths = 24;
         else
            wMonths = 6;
      }

      //
      // Set After Date
      //
      stTime.wMonth = (USHORT)(stTime.wMonth + wMonths);
      if ((stTime.wMonth / 12) > 0)
      {
         stTime.wYear = (USHORT)(stTime.wYear + (stTime.wMonth / 12));
         stTime.wMonth = (USHORT)(stTime.wMonth % 12);
      }

      SystemTimeToFileTime(&stTime, &ftTime);
      CertInfo.NotAfter = ftTime;

      // Get Public Key Info size
      bResult = CryptExportPublicKeyInfo(hProv, dwKeyType,
                                       X509_ASN_ENCODING, NULL, &dwSize);
      if (!bResult)
      {
         printf("CryptExportPublicKeyInfo failed with %x\n", GetLastError());
         __leave;
      }

      // Allocate memory for Public Key Info
      PublicKeyInfo = (PCERT_PUBLIC_KEY_INFO)HeapAlloc(hHeap, 0, dwSize);
      if (!PublicKeyInfo)
      {
         printf("Unable to allocate memory for public key info\n");
         __leave;
      }

      // Get Public Key Info
      bResult = CryptExportPublicKeyInfo(hProv, dwKeyType,
                                       X509_ASN_ENCODING,
                                       PublicKeyInfo, &dwSize);
      if (!bResult)
      {
         printf("CryptExportPublicKeyInfo failed with %x\n", GetLastError());
         __leave;
      }

      // Set Public Key info of Certificate
      CertInfo.SubjectPublicKeyInfo = *PublicKeyInfo;

      // Create Hash
      bResult = CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash);
      if (!bResult)
      {
         printf("CryptCreateHash failed with %x\n", GetLastError());
         __leave;
      }

      // Hash Public Key Info
      bResult = CryptHashData(hHash, (LPBYTE)PublicKeyInfo, dwSize, 0);
      if (!bResult)
      {
         printf("CryptHashData failed with %x\n", GetLastError());
         __leave;
      }

      // Get Size of Hash
      bResult = CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwSize, 0);
      if (!bResult)
      {
         printf("CryptGetHashParam failed with %x\n", GetLastError());
         __leave;
      }

      // Allocate Memory for Key Identifier (hash of Public Key info)
      pbKeyIdentifier = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
      if (!pbKeyIdentifier)
      {
         printf("Unable to allocate memory for Hashed Public key Info\n");
         __leave;
      }

      // Get Hash of Public Key Info
      bResult = CryptGetHashParam(hHash, HP_HASHVAL, pbKeyIdentifier, &dwSize, 0);
      if (!bResult)
      {
         printf("CryptGetHashParam failed with %x\n", GetLastError());
         __leave;
      }

      // We will use this to set the Key Identifier extension
      CertKeyIdentifier.cbData = dwSize;
      CertKeyIdentifier.pbData = pbKeyIdentifier;

      // Set Subject of Certificate
      CertInfo.Subject = IssuerName;

      // Set Issuer of Certificate
      if (bSelfSigned)
      {
         CertInfo.Issuer = IssuerName;
      }
      else
      {
         pIssuerCert = FindCertificate(szIssuerName, szIssuerStore,
                                       dwIssuerFlags, &KeyId,
                                       &hIssuerProv, &dwIssuerKeyType);
         if (!pIssuerCert)
         {
            printf("Unable to find Issuer Certificate\n");
            __leave;
         }

         CertInfo.Issuer = pIssuerCert->pCertInfo->Subject;
      }

      // Get Subject Key Identifier Extension size
      bResult = CryptEncodeObject(ENCODING,
                                  szOID_SUBJECT_KEY_IDENTIFIER,
                                  (LPVOID)&CertKeyIdentifier,
                                  NULL, &dwSize);
      if (!bResult)
      {
         printf("CryptEncodeObject failed with %x\n", GetLastError());
         __leave;
      }

      // Allocate Memory for Subject Key Identifier Blob
      SubjectKeyIdentifier = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
      if (!SubjectKeyIdentifier)
      {
         printf("Unable to allocate memory for Subject Key Identifier\n");
         __leave;
      }

      // Get Subject Key Identifier Extension
      bResult = CryptEncodeObject(ENCODING,
                                  szOID_SUBJECT_KEY_IDENTIFIER,
                                  (LPVOID)&CertKeyIdentifier,
                                  SubjectKeyIdentifier, &dwSize);
      if (!bResult)
      {
         printf("CryptEncodeObject failed with %x\n", GetLastError());
         __leave;
      }

      // Set Subject Key Identifier
      CertExtension[CertInfo.cExtension].pszObjId = szOID_SUBJECT_KEY_IDENTIFIER;
      CertExtension[CertInfo.cExtension].fCritical = FALSE;
      CertExtension[CertInfo.cExtension].Value.cbData = dwSize;
      CertExtension[CertInfo.cExtension].Value.pbData = SubjectKeyIdentifier;

      // Increase extension count
      CertInfo.cExtension++;

      // Set Key Usage according to Public Key Type
      ZeroMemory(&KeyUsage, sizeof(KeyUsage));
      KeyUsage.cbData = 1;
      KeyUsage.pbData = &ByteData;

      if (dwKeyType == AT_SIGNATURE)
      {
         ByteData = CERT_DIGITAL_SIGNATURE_KEY_USAGE|
                    CERT_NON_REPUDIATION_KEY_USAGE|
                    CERT_KEY_CERT_SIGN_KEY_USAGE |
                    CERT_CRL_SIGN_KEY_USAGE;
      }

      if (dwKeyType == AT_KEYEXCHANGE)
      {
         ByteData = CERT_DIGITAL_SIGNATURE_KEY_USAGE |
                    CERT_DATA_ENCIPHERMENT_KEY_USAGE|
                    CERT_KEY_ENCIPHERMENT_KEY_USAGE |
                    CERT_KEY_AGREEMENT_KEY_USAGE;
      }

      // Get Key Usage blob size
      bResult = CryptEncodeObject(ENCODING,
                                  X509_KEY_USAGE,
                                  (LPVOID)&KeyUsage,
                                  NULL, &dwSize);
      if (!bResult)
      {
         printf("CryptEncodeObject failed with %x\n", GetLastError());
         __leave;
      }

      // Allocate Memory for Key Usage Blob
      pbKeyUsage = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
      if (!pbKeyUsage)
      {
         printf("Unable to allocate memory for Subject Key Identifier\n");
         __leave;
      }

      // Get Key Usage Extension blob
      bResult = CryptEncodeObject(ENCODING,
                                  X509_KEY_USAGE,
                                  (LPVOID)&KeyUsage,
                                  pbKeyUsage, &dwSize);
      if (!bResult)
      {
         printf("CryptEncodeObject failed with %x\n", GetLastError());
         __leave;
      }

      // Set Key Usage extension
      CertExtension[CertInfo.cExtension].pszObjId = szOID_KEY_USAGE;
      CertExtension[CertInfo.cExtension].fCritical = FALSE;
      CertExtension[CertInfo.cExtension].Value.cbData = dwSize;
      CertExtension[CertInfo.cExtension].Value.pbData = pbKeyUsage;

      // Increase extension count
      CertInfo.cExtension++;

      if (CertEnhKeyUsage.cUsageIdentifier != 0)
      {
         // Get Enhanced Key Usage size
         bResult = CryptEncodeObject(ENCODING,
                                     X509_ENHANCED_KEY_USAGE,
                                     (LPVOID)&CertEnhKeyUsage,
                                     NULL, &dwSize);
         if (!bResult)
         {
            printf("CryptEncodeObject failed with %x\n", GetLastError());
            __leave;
         }

         // Allocate Memory for Enhanced Key usage Blob
         pbEnhKeyUsage = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
         if (!pbEnhKeyUsage)
         {
            printf("Unable to allocate memory for Subject Key Identifier\n");
            __leave;
         }

         // Get Enhanced Key Usage Extension blob
         bResult = CryptEncodeObject(ENCODING,
                                     X509_ENHANCED_KEY_USAGE,
                                     (LPVOID)&CertEnhKeyUsage,
                                     pbEnhKeyUsage, &dwSize);
         if (!bResult)
         {
            printf("CryptEncodeObject failed with %x\n", GetLastError());
            __leave;
         }

         // Set Enhanced Key Usage extension
         CertExtension[CertInfo.cExtension].pszObjId = szOID_ENHANCED_KEY_USAGE;
         CertExtension[CertInfo.cExtension].fCritical = FALSE;
         CertExtension[CertInfo.cExtension].Value.cbData = dwSize;
         CertExtension[CertInfo.cExtension].Value.pbData = pbEnhKeyUsage;

         // Increase extension count
         CertInfo.cExtension++;
      }

      // Zero Basic Constraints structure
      ZeroMemory(&BasicConstraints, sizeof(BasicConstraints));

      // Self-signed is always a CA
      if (bSelfSigned)
      {
         BasicConstraints.fCA = TRUE;
         BasicConstraints.fPathLenConstraint = TRUE;
         BasicConstraints.dwPathLenConstraint = 2;
      }
      else
      {
         BasicConstraints.fCA = bCA;
         if ( bCA )
         {
             BasicConstraints.fPathLenConstraint = TRUE;
             BasicConstraints.dwPathLenConstraint = 1;
         }
      }

      // Get Basic Constraints blob size
      bResult = CryptEncodeObject(ENCODING,
                                  X509_BASIC_CONSTRAINTS2,
                                  (LPVOID)&BasicConstraints,
                                  NULL, &dwSize);
      if (!bResult)
      {
         printf("CryptEncodeObject failed with %x\n", GetLastError());
         __leave;
      }

      // Allocate Memory for Basic Constraints Blob
      pbBasicConstraints = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
      if (!pbBasicConstraints)
      {
         printf("Unable to allocate memory for Subject Key Identifier\n");
         __leave;
      }

      // Get Basic Constraints Extension blob
      bResult = CryptEncodeObject(ENCODING,
                                  X509_BASIC_CONSTRAINTS2,
                                  (LPVOID)&BasicConstraints,
                                  pbBasicConstraints, &dwSize);
      if (!bResult)
      {
         printf("CryptEncodeObject failed with %x\n", GetLastError());
         __leave;
      }

      // Set Basic Constraints extension
      CertExtension[CertInfo.cExtension].pszObjId = szOID_BASIC_CONSTRAINTS2;
      CertExtension[CertInfo.cExtension].fCritical = FALSE;
      CertExtension[CertInfo.cExtension].Value.cbData = dwSize;
      CertExtension[CertInfo.cExtension].Value.pbData = pbBasicConstraints;

      // Increase extension count
      CertInfo.cExtension++;

      if (bSelfSigned)
      {
         AuthorityKeyId.KeyId = CertKeyIdentifier;
         AuthorityKeyId.CertIssuer = CertInfo.Issuer;
         AuthorityKeyId.CertSerialNumber = CertInfo.SerialNumber;
         bAddAuthorityExtension = TRUE;
      }
      else
      {
         if (KeyId)
         {
            AuthorityKeyId.KeyId = *KeyId;
            AuthorityKeyId.CertIssuer = pIssuerCert->pCertInfo->Subject;
            AuthorityKeyId.CertSerialNumber = pIssuerCert->pCertInfo->SerialNumber;
            bAddAuthorityExtension = TRUE;
         }
      }

      if (bAddAuthorityExtension)
      {
         // Get Authority Key Id blob size
         bResult = CryptEncodeObject(ENCODING,
                                     X509_AUTHORITY_KEY_ID,
                                     (LPVOID)&AuthorityKeyId,
                                     NULL, &dwSize);
         if (!bResult)
         {
            printf("CryptEncodeObject failed with %x\n", GetLastError());
            __leave;
         }

         // Allocate Authority Key Id Blob
         pbAuthorityKeyId = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
         if (!pbAuthorityKeyId)
         {
            printf("Unable to allocate memory for Subject Key Identifier\n");
            __leave;
         }

         // Get Authority Key Id blob
         bResult = CryptEncodeObject(ENCODING,
                                     X509_AUTHORITY_KEY_ID,
                                     (LPVOID)&AuthorityKeyId,
                                     pbAuthorityKeyId, &dwSize);
         if (!bResult)
         {
            printf("CryptEncodeObject failed with %x\n", GetLastError());
            __leave;
         }

         // Set Authority Key Id extension
         CertExtension[CertInfo.cExtension].pszObjId = szOID_AUTHORITY_KEY_IDENTIFIER;
         CertExtension[CertInfo.cExtension].fCritical = FALSE;
         CertExtension[CertInfo.cExtension].Value.cbData = dwSize;
         CertExtension[CertInfo.cExtension].Value.pbData = pbAuthorityKeyId;

         // Increase extension count
         CertInfo.cExtension++;
      }

      CertInfo.rgExtension = CertExtension;

      if (bSelfSigned)
      {
         // Get Encoded Certificate Size
         bResult = CryptSignAndEncodeCertificate(hProv, dwKeyType,
                                                 X509_ASN_ENCODING, X509_CERT_TO_BE_SIGNED,
                                                 (LPVOID)&CertInfo,
                                                 &(CertInfo.SignatureAlgorithm),
                                                 NULL, NULL, &dwSize);
         if (!bResult)
         {
            printf("CryptSignAndEncodeCertificate failed with %x\n", GetLastError());
            __leave;
         }

         // Allocate memory for encoded certificate
         bpEncodedCert = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
         if (!bpEncodedCert)
         {
            printf("Unable to allocate memory for encoded certficate\n");
            __leave;
         }

         // Sign and Encode certificate
         dwStart = GetTickCount();
         bResult = CryptSignAndEncodeCertificate(hProv, dwKeyType,
                                                 X509_ASN_ENCODING, X509_CERT_TO_BE_SIGNED,
                                                 (LPVOID)&CertInfo,
                                                 &(CertInfo.SignatureAlgorithm),
                                                 NULL, bpEncodedCert, &dwSize);
         //printf("CryptSignAndEncodeCertificate took %d milliseconds\n", GetTickCount() - dwStart);
         if (!bResult)
         {
            printf("CryptSignAndEncodeCertificate failed with %x\n", GetLastError());
            __leave;
         }
      }
      else
      {
         // Get Encoded Certificate Size
         bResult = CryptSignAndEncodeCertificate(hIssuerProv, dwIssuerKeyType,
                                                 X509_ASN_ENCODING, X509_CERT_TO_BE_SIGNED,
                                                 (LPVOID)&CertInfo,
                                                 //&(CertInfo.SignatureAlgorithm),
                                                 &(pIssuerCert->pCertInfo->SignatureAlgorithm),
                                                 NULL, NULL, &dwSize);
         if (!bResult)
         {
            printf("CryptSignAndEncodeCertificate failed with %x\n", GetLastError());
            __leave;
         }

         // Allocate memory for encoded certificate
         bpEncodedCert = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
         if (!bpEncodedCert)
         {
            printf("Unable to allocate memory for encoded certficate\n");
            __leave;
         }

         // Sign and Encode certificate
         dwStart = GetTickCount();
         bResult = CryptSignAndEncodeCertificate(hIssuerProv, dwIssuerKeyType,
                                                 X509_ASN_ENCODING, X509_CERT_TO_BE_SIGNED,
                                                 (LPVOID)&CertInfo,
                                                 //&(CertInfo.SignatureAlgorithm),
                                                 &(pIssuerCert->pCertInfo->SignatureAlgorithm),
                                                 NULL, bpEncodedCert, &dwSize);
         //printf("CryptSignAndEncodeCertificate took %d milliseconds\n", GetTickCount() - dwStart);
         if (!bResult)
         {
            printf("CryptSignAndEncodeCertificate failed with %x\n", GetLastError());
            __leave;
         }
      }
      
#if REPEAT_LOOP == 1
      if (bExport)
      {
          DWORD dwWritten;

	      // Convert multibyte store name to wide char string
     	  i = MultiByteToWideChar(CP_ACP, 0, szCertFile, -1, szwCertFile, 200);
	      if (i == 0)
     	  {
		       printf("Unable to convert szCertFile to Unicode string\n");
		       __leave;
	      }
     
          // Create Certificate file
          hCertFile = CreateFile(szwCertFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL, NULL);
          if (hCertFile == INVALID_HANDLE_VALUE)
          {
              printf("CreateFile failed with %d\n", GetLastError());
              __leave;
          }

          // Write encoded Certificate to file
          bResult = WriteFile(hCertFile, (LPVOID)bpEncodedCert, dwSize, &dwWritten, NULL);
          if (!bResult)
          {
              printf("WriteFile failed with %d\n", GetLastError());
              __leave;
          }

          if (_stricmp( szPassword, "null" ) != 0)
          {
              // Destroy previous hash
              if (hHash) CryptDestroyHash(hHash);
              hHash = 0;

              // Create hash
              bResult = CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash);
              if (!bResult)
              {
                  printf("CryptCreateHash failed with %x\n", GetLastError());
                  __leave;
              }

              // Hash password
              bResult = CryptHashData(hHash, (LPBYTE)szPassword, strlen(szPassword), 0);
              if (!bResult)
              {
                  printf("CryptHashData failed with %x\n", GetLastError());
                  __leave;
              }

              // Derive Session Key from hash
              bResult = CryptDeriveKey(hProv, CALG_RC4, hHash, CRYPT_EXPORTABLE, &hSessionKey);
              if (!bResult)
              {
                  printf("CryptDeriveKey failed with %x\n", GetLastError());
                  __leave;
              }
          }

          //
          // Export the private key
          //
          // Get size of exported key blob
          bResult = CryptExportKey(hPubKey, hSessionKey, PRIVATEKEYBLOB, 0, NULL, &dwSize);
          if (!bResult)
          {
              printf("CryptExportKey failed with %x\n", GetLastError());
              __leave;
          }
          
          // Allocate memory for exported key blob
          pbExportedKey = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
          if (!pbExportedKey)
          {
              printf("Unable to allocate memory for exported key\n");
              __leave;
          }

          // Get exported key blob
          bResult = CryptExportKey(hPubKey, hSessionKey, PRIVATEKEYBLOB,
                                   0, pbExportedKey, &dwSize);
          if (!bResult)
          {
              printf("CryptExportKey failed with %x\n", GetLastError());
              __leave;
          }

	      // Convert multibyte store name to wide char string
     	  i = MultiByteToWideChar(CP_ACP, 0, szKeyFile, -1, szwKeyFile, 200);
	      if (i == 0)
     	  {
		       printf("Unable to convert szKeyFile to Unicode string\n");
		       __leave;
	      }
     
          // Create Exported Key File
          hKeyFile = CreateFile(szwKeyFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL, NULL);
          if (hKeyFile == INVALID_HANDLE_VALUE)
          {
              printf("CreateFile failed with %d\n", GetLastError());
              __leave;
          }

          // Write exported key to file
          bResult = WriteFile(hKeyFile, (LPVOID)pbExportedKey, dwSize, &dwWritten, NULL);
          if (!bResult)
          {
              printf("WriteFile failed with %d\n", GetLastError());
              __leave;
          }

          assert( dwWritten == dwSize );

          //
          // Export the public key
          //
          if (pbExportedKey) HeapFree(hHeap, 0, pbExportedKey);
          if (hKeyFile) CloseHandle(hKeyFile);
          
          // Get size of exported key blob
          bResult = CryptExportKey(hPubKey, NULL, PUBLICKEYBLOB, 0, NULL, &dwSize);
          if (!bResult)
          {
              printf("CryptExportKey failed with %x\n", GetLastError());
              __leave;
          }
          
          // Allocate memory for exported key blob
          pbExportedKey = (LPBYTE)HeapAlloc(hHeap, 0, dwSize);
          if (!pbExportedKey)
          {
              printf("Unable to allocate memory for exported key\n");
              __leave;
          }

          // Get exported key blob
          bResult = CryptExportKey(hPubKey, NULL, PUBLICKEYBLOB,
                                   0, pbExportedKey, &dwSize);
          if (!bResult)
          {
              printf("CryptExportKey failed with %x\n", GetLastError());
              __leave;
          }

          // Create Exported Key File
          hKeyFile = CreateFile(L"publickey.key", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL, NULL);
          if (hKeyFile == INVALID_HANDLE_VALUE)
          {
              printf("CreateFile failed with %d\n", GetLastError());
              __leave;
          }

          // Write exported key to file
          bResult = WriteFile(hKeyFile, (LPVOID)pbExportedKey, dwSize, &dwWritten, NULL);
          if (!bResult)
          {
              printf("WriteFile failed with %d\n", GetLastError());
              __leave;
          }

          assert( dwWritten == dwSize );
          
          // delete crypto context
          bResult = CryptAcquireContext(&hProv,
                            szwContainer,
                            szwProvider,
                            dwProviderType,
                            CRYPT_DELETEKEYSET | dwAcquireFlags);
          if (!bResult)

          {
              printf("CryptAcquireContext failed with %x\n", GetLastError());
              __leave;
          }

          bReturn = TRUE;

          printf("Certificate and Key information stored to files\n");
      }
      else
      {
          WCHAR szwStore[20];
          CRYPT_KEY_PROV_INFO CryptKeyProvInfo;
          LPWSTR szwFile;
          int i;
          DWORD dwWritten;

          if (bSelfSigned)
          {
              szwFile = L"SelfSigned.cer";
          }
          else
          {
              szwFile = L"Certificate.cer";
          }

          // Create Certificate file
          hCertFile = CreateFile(szwFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL, NULL);
          if (hCertFile == INVALID_HANDLE_VALUE)
          {
              printf("CreateFile failed with %d\n", GetLastError());
              __leave;
          }

          // Write encoded Certificate to file
          bResult = WriteFile(hCertFile, (LPVOID)bpEncodedCert, dwSize, &dwWritten, NULL);
          if (!bResult)
          {
              printf("WriteFile failed with %d\n", GetLastError());
              __leave;
          }

          printf("File called %ls has been saved.\n", szwFile);

          // Convert Store string to unicode
          i = MultiByteToWideChar(0, 0, szSubjectStore, -1, szwStore, 20);
          if (i == 0)
          {
              printf("MultiByteToWideChar failed with %d\n", GetLastError());
              __leave;
          }

          // Open Certificate store
          hStore = CertOpenStore(CERT_STORE_PROV_SYSTEM,
                                 ENCODING,
                                 0, dwSubjectFlags, (LPVOID)szwStore);
          if (!hStore)
          {
              printf("CertOpenStore failed with %x\n", GetLastError());
              __leave;
          }

          // Place Certificate in store
          bResult = CertAddEncodedCertificateToStore(hStore, X509_ASN_ENCODING,
                                    bpEncodedCert, dwSize,
                                    CERT_STORE_ADD_REPLACE_EXISTING,
                                    &pCertContext);
          if (!bResult)
          {
              printf("CertAddEncodedCertificateToStore failed with %x\n", GetLastError());
              __leave;
          }

          // Initialize CRYPT_KEY_PROV_INFO structure
          ZeroMemory(&CryptKeyProvInfo, sizeof(CryptKeyProvInfo));
          CryptKeyProvInfo.pwszContainerName = szwContainer;
          CryptKeyProvInfo.pwszProvName = szwProvider;
          CryptKeyProvInfo.dwProvType = dwProviderType;
          CryptKeyProvInfo.dwKeySpec = dwKeyType;

          // Set Certificate's Key Provider info
          bResult = CertSetCertificateContextProperty(pCertContext,
                                        CERT_KEY_PROV_INFO_PROP_ID,
                                        0, (LPVOID)&CryptKeyProvInfo);
          if (!bResult)
          {
              printf("CertSetCertificateContextProperty failed with %x\n", GetLastError());
              __leave;
          }

          bReturn = TRUE;

          printf("Certificate created successfully and installed\n");
      }
#else
      bReturn = TRUE;
#endif
   }
   _finally
   {
      // Clean up
      if (pbNameBlob) HeapFree(hHeap, 0, pbNameBlob);
      if (CertEnhKeyUsage.rgpszUsageIdentifier)
         HeapFree(hHeap, 0, CertEnhKeyUsage.rgpszUsageIdentifier);
      if (PublicKeyInfo) HeapFree(hHeap, 0, PublicKeyInfo);
      if (pbKeyIdentifier) HeapFree(hHeap, 0, pbKeyIdentifier);
      if (SubjectKeyIdentifier) HeapFree(hHeap, 0, SubjectKeyIdentifier);
      if (pbKeyUsage) HeapFree(hHeap, 0, pbKeyUsage);
      if (pbEnhKeyUsage) HeapFree(hHeap, 0, pbEnhKeyUsage);
      if (pbBasicConstraints) HeapFree(hHeap, 0, pbBasicConstraints);
      if (KeyId) HeapFree(hHeap, 0, KeyId);
      if (pbAuthorityKeyId) HeapFree(hHeap, 0, pbAuthorityKeyId);
      if (bpEncodedCert) HeapFree(hHeap, 0, bpEncodedCert);
      if (pbExportedKey) HeapFree(hHeap, 0, pbExportedKey);
      if (szwContainer) RpcStringFree(&szwContainer);
      if (hCertFile) CloseHandle(hCertFile);
      if (hKeyFile) CloseHandle(hKeyFile);
      if (hPubKey) CryptDestroyKey(hPubKey);
      if (hSessionKey) CryptDestroyKey(hSessionKey);
      if (hHash) CryptDestroyHash(hHash);
      if (hProv) CryptReleaseContext(hProv, 0);
      if (hIssuerProv) CryptReleaseContext(hIssuerProv, 0);
      if (pIssuerCert) CertFreeCertificateContext(pIssuerCert);
      if (pCertContext) CertFreeCertificateContext(pCertContext);
      if (hStore) CertCloseStore(hStore, 0);
   }

   return bReturn;
}

HRESULT VerifyKeyPair( LPBYTE pbEncodedCert, DWORD dwCertSize, HCRYPTPROV hPrvProv )
{
    PCCERT_CONTEXT pCertContextRequest = NULL;
    HRESULT hr = S_OK;
    LPWSTR szwContainer = NULL;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    HCRYPTKEY hPubKey = 0;
    RPC_STATUS Status;
    UUID Uuid;
    BOOL bResult;
    WCHAR szwProvider[260] = { PROVIDER };
    DWORD dwProviderType = PROV_RSA_FULL;
    DWORD dwAcquireFlags = 0;
	BYTE *pbBuffer= (BYTE *)"The data that is to be hashed and signed.";
    DWORD dwBufferLen = strlen((char *)pbBuffer)+1;
	LPTSTR szDescription = NULL;
    BYTE pbSignature[200];
    DWORD dwSigLen = 200;
    //DWORD dwKeyType = AT_KEYEXCHANGE;
    DWORD tickct;
    int i;
    
	bResult = CryptCreateHash(
	   hPrvProv, 
	   CALG_SHA1, 
	   0, 
	   0, 
	   &hHash);
    if (!bResult)
    {
       hr = GetLastError();
       printf("CryptCreateHash failed with %x\n", hr);
       goto error;
    }

	bResult = CryptHashData(
	   hHash, 
	   pbBuffer, 
	   dwBufferLen, 
	   0);
    if (!bResult)
    {
       hr = GetLastError();
       printf("CryptHashData failed with %x\n", hr);
       goto error;
    }

    tickct = GetTickCount();
    for (i=0; i<SIGN_LOOP; ++i)
    {
        bResult = CryptSignHash(
           hHash, 
           AT_KEYEXCHANGE, 
           szDescription, 
           0, 
           pbSignature, 
           &dwSigLen);
        if (!bResult)
        {
           hr = GetLastError();
           printf("CryptSignHash failed with %x\n", hr);
           goto error;
        }
    }
    printf("CryptSignHash %d times took %d ms for %d byte Signing\n", i, GetTickCount() - tickct, dwSigLen );
		
	if (hHash) CryptDestroyHash(hHash);

	//
	// We now have something signed by the private key.
	// We now verify the it can be verified with the public key in the cert
	//
	
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
		         goto error;
              }
           }
           else
           {
              printf("Unable to create temporary provider handle\n");
              goto error;
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
           goto error;
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
          goto error;
        }
    }

    // Create new crypto context
    bResult = CryptAcquireContext(&hProv,
                            szwContainer,
                            szwProvider,
                            dwProviderType,
                            CRYPT_NEWKEYSET | dwAcquireFlags);
    if (!bResult)
    {
       hr = GetLastError();
       printf("CryptAcquireContext failed with %x\n", hr);
       goto error;
    }

    pCertContextRequest = CertCreateCertificateContext( X509_ASN_ENCODING, pbEncodedCert, dwCertSize );
    if (!pCertContextRequest)
    {
        hr = GetLastError();
        printf("Failed CertCreateCertificateContext [%x]\n", hr);
        goto error;
    }

	bResult = CryptImportPublicKeyInfoEx(hProv, ENCODING, &(pCertContextRequest->pCertInfo->SubjectPublicKeyInfo), CALG_RSA_KEYX, 0, NULL, &hPubKey );
    if (!bResult)
    {
       hr = GetLastError();
       printf("CryptImportPublicKeyInfoEx failed with %x\n", hr);
       goto error;
    }

	bResult = CryptCreateHash(
	   hProv, 
	   CALG_SHA1, 
	   0, 
	   0, 
	   &hHash);
    if (!bResult)
    {
       hr = GetLastError();
       printf("CryptCreateHash 2 failed with %x\n", hr);
       goto error;
    }

	bResult = CryptHashData(
	   hHash, 
	   pbBuffer, 
	   dwBufferLen, 
	   0);
    if (!bResult)
    {
       hr = GetLastError();
       printf("CryptHashData 2 failed with %x\n", hr);
       goto error;
    }

    tickct = GetTickCount();
    for (i=0; i<VERIFY_LOOP; ++i)
    {
    	bResult = CryptVerifySignature(
    	   hHash, 
    	   pbSignature, 
    	   dwSigLen, 
    	   hPubKey,
    	   szDescription, 
    	   0);
        if (!bResult)
        {
           hr = GetLastError();
           printf("CryptVerifySignature failed with %x\n", hr);
           goto error;
        }
    }
    printf("CryptVerifySignature %d times took %d ms for %d byte Sig verification\n", i, GetTickCount() - tickct, dwSigLen );

error:
    if (szwContainer) RpcStringFree(&szwContainer);
    if (hHash) CryptDestroyHash(hHash);
    if (hPubKey) CryptDestroyKey(hPubKey);
    if (hProv) CryptReleaseContext(hProv, 0);
    if (pCertContextRequest) CertFreeCertificateContext(pCertContextRequest);
	
	return hr;
}     	

//
// Find Certificate with szCertName in the Subject name
//
PCCERT_CONTEXT FindCertificate(LPSTR szCertName, LPSTR szStore,
                               DWORD dwFlags, PCRYPT_DATA_BLOB *KeyId,
                               HCRYPTPROV *hProv, LPDWORD dwKeyType)
{
   HANDLE hHeap = GetProcessHeap();
   PCRYPT_KEY_PROV_INFO KeyProvInfo = NULL;
   PCCERT_CONTEXT pCertContext = NULL;
   PCERT_EXTENSION pExtension = NULL;
   HCERTSTORE hStore = 0;
   WCHAR szwStore[20];
   BOOL bResult, bSuccess = FALSE;
   DWORD dwSize, dwAcquireFlags = 0;
   int i;

   __try
   {
      *KeyId = NULL;
      *hProv = 0;

      if (dwFlags == CERT_SYSTEM_STORE_LOCAL_MACHINE)
         dwAcquireFlags = CRYPT_MACHINE_KEYSET;

      // Convert Store string to Unicode
      i = MultiByteToWideChar(0, 0, szStore, -1, szwStore, 20);
      if (i == 0)
      {
         __leave;
      }

      // Open Certificate store
      hStore = CertOpenStore(CERT_STORE_PROV_SYSTEM,
                           ENCODING,
                           0, dwFlags, (LPVOID)szwStore);
      if (!hStore)
      {
         __leave;
      }

      // Find Certificate with Subject name
      pCertContext = CertFindCertificateInStore(hStore,
                              ENCODING,
                              0, CERT_FIND_SUBJECT_STR_A,
                              (LPVOID)szCertName, NULL);
      if (pCertContext)
      {
         // Get Key Provider Info size
         bResult = CertGetCertificateContextProperty(pCertContext,
                                             CERT_KEY_PROV_INFO_PROP_ID,
                                             NULL,
                                             &dwSize);
         if (!bResult)
         {
            __leave;
         }

         // Allocate Key Provider Info
         KeyProvInfo = (PCRYPT_KEY_PROV_INFO)HeapAlloc(hHeap, 0, dwSize);
         if (!KeyProvInfo)
         {
            __leave;
         }

         // Get Key Provider Info
         bResult = CertGetCertificateContextProperty(pCertContext,
                                             CERT_KEY_PROV_INFO_PROP_ID,
                                             (LPVOID)KeyProvInfo,
                                             &dwSize);
         if (!bResult)
         {
            __leave;
         }

         // Get Crypto Context of Certificate
         bResult = CryptAcquireContext(hProv, KeyProvInfo->pwszContainerName, KeyProvInfo->pwszProvName,
                                       KeyProvInfo->dwProvType, dwAcquireFlags);
         if (!bResult)
         {
            __leave;
         }

         // Return Key Spec
         *dwKeyType = KeyProvInfo->dwKeySpec;

         bSuccess = TRUE;

         // Find Subject Key Identifier Extension
         pExtension = CertFindExtension(szOID_SUBJECT_KEY_IDENTIFIER,
                                        pCertContext->pCertInfo->cExtension,
                                        pCertContext->pCertInfo->rgExtension);

         // If Subject Key Identifier Extension Exists
         if (pExtension)
         {

            // Get Size of Data Blob
            bResult = CryptDecodeObject(ENCODING,
                                        szOID_SUBJECT_KEY_IDENTIFIER,
                                        pExtension->Value.pbData,
                                        pExtension->Value.cbData,
                                        0, NULL, &dwSize);
            if (!bResult)
            {
               __leave;
            }

            // Allocate Data Blob
            *KeyId = (PCRYPT_DATA_BLOB)HeapAlloc(hHeap, 0, dwSize);
            if (!(*KeyId))
            {
               __leave;
            }

            // Get Key Identifier Data Blob
            bResult = CryptDecodeObject(ENCODING,
                                        szOID_SUBJECT_KEY_IDENTIFIER,
                                        pExtension->Value.pbData,
                                        pExtension->Value.cbData,
                                        0, (LPVOID)*KeyId, &dwSize);
            if (!bResult)
            {
               __leave;
            }
         }
      }
      else
      {
         __leave;
      }

      bSuccess = TRUE;
   }
   __finally
   {
      // Clean up
      if (hStore) CertCloseStore(hStore, 0);
      if (KeyProvInfo) HeapFree(hHeap, 0, KeyProvInfo);
      if (!bSuccess)
      {
         if (pCertContext) CertFreeCertificateContext(pCertContext);
         pCertContext = NULL;
         if (*KeyId) HeapFree(hHeap, 0, *KeyId);
         *KeyId = NULL;
      }
   }

   return pCertContext;
}

//
// Implement this is CryptEnumProviders is unavailble
//
BOOL WINAPI MyCryptEnumProviders(DWORD dwIndex, LPDWORD pdwProvType,
                                 LPTSTR szProvName, LPDWORD pcbProvName)
{
    static LPTSTR szKeyPath = TEXT("SOFTWARE\\Microsoft\\Cryptography\\Defaults\\Provider");
    BOOL bResult = TRUE;
    DWORD dwNumKeys;
    HKEY hKey = NULL;
    HKEY hSubKey = NULL;
    LONG lResult;
    FILETIME ft;

    __try
    {
       // Open registry provider registry key
        lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKeyPath, 0, KEY_READ, &hKey);
        if (lResult != ERROR_SUCCESS)
        {
            bResult = FALSE;
            __leave;
        }


        if (szProvName == NULL)
        {
           // Query for Number of Keys and Size maximum size of
           // provider
           lResult = RegQueryInfoKey(hKey, NULL, NULL, NULL, &dwNumKeys, pcbProvName,
                                     NULL, NULL, NULL, NULL, NULL, NULL);
           if (lResult != ERROR_SUCCESS)
           {
              bResult = FALSE;
              __leave;
           }

           // return Size of Provider Name
           *pcbProvName = (*pcbProvName + 1) * sizeof(TCHAR);

           if (dwIndex >= dwNumKeys) return FALSE;
        }
        else
        {
           DWORD dwSize;
           DWORD dwType;

           // Get Provider by Index
           dwSize = *pcbProvName/sizeof(TCHAR);
           lResult = RegEnumKeyEx(hKey, dwIndex, szProvName, &dwSize, NULL, NULL, NULL, &ft);
           if (lResult != ERROR_SUCCESS)
           {
              bResult = FALSE;
              __leave;
           }

           *pcbProvName = (dwSize + 1) * sizeof(TCHAR);

           // Open the Provider Registry Key
           lResult = RegOpenKeyEx(hKey, szProvName, 0, KEY_READ, &hSubKey);
           if (lResult != ERROR_SUCCESS)
           {
              bResult = FALSE;
              __leave;
           }

           // Get Provider Type
           dwSize = sizeof(DWORD);
           lResult = RegQueryValueEx(hSubKey, TEXT("Type"), NULL, &dwType, (LPBYTE)pdwProvType, &dwSize);
           if (lResult != ERROR_SUCCESS)
           {
              bResult = FALSE;
              __leave;
           }
        }
    }
    __finally
    {
       // Clean up
       if (hKey != NULL) RegCloseKey(hKey);
       if (hSubKey != NULL) RegCloseKey(hSubKey);
    }

    return bResult;
}

//
// Print Usage
//
void PrintUsage(void)
{
   DWORD nIndex = 0;
   DWORD dwProvType, dwSize;
   WCHAR szwProvider[160];

   printf("\nUsage: CreateCert [Import option] <X509 Name> [Options]\n");
   printf("\n[Import option] : If this option is present, everything else is ignored\n");
   printf("\t-i <cert file> <public key file> <password> <store> <u>ser|<m>achine <provider num>\n");
   printf("\n<X509 Name> (eg. CN=Certificate)\n");
   printf("\n[Options]\n");
   printf("Type of Certificate:\n");
   printf("\t-p <provider number> - default: PROVIDER\n");
   printf("\t-k <e>xchange key | <s>ignature key - default:exchange\n");
   printf("\t-s <sha>|<md5> - default:sha\n");
   printf("\t-m <number of months of validity>\n");
   printf("\t-ca - indicates that the certificate is to be a CA. True for selfsigned\n");
   printf("\t-u <enhanced key usage number> - add following numbers\n");
   printf("\t\tServer Authentication : 1\n");
   printf("\t\tClient Authentication : 2\n");
   printf("\t\tCode Signing          : 4\n");
   printf("\t\tEmail Protection      : 8\n");
   printf("\t\tTime Stamping         : 16\n");
   printf("\nLocation of Certificate:\n");
   printf("\t-sl <store> <u>ser|<m>achine - default:my u\n");
   printf("\t\tor\n");
   printf("\t-ex <cert file> <public key file> <password>\n");
   printf("\nSigner of the certificate - defaults to self-signed if -is is not present\n");
   printf("\t-is <CommonName or substring> <store> <u>ser|<m>achine\n");

   printf("\nProviders:\n");
   dwSize = 160;
   while (MyCryptEnumProviders(nIndex, &dwProvType, szwProvider, &dwSize))
   {
      printf("\t%d : %ls\n", nIndex, szwProvider);
      dwSize = 160;
      nIndex++;
   }

   printf("\nSelf-Signed Certificates create a file called SelfSigned.cer\n");
   printf("Non Self-Signed Certificates create a file called Certificate.cer\n");
}
