//===================================================================
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright 1998-1999. Microsoft Corporation. All Right Reserved.
// File: CreateCert.h
//
//===================================================================
#ifndef __CreateCert__
#define __CreateCert__

void PrintUsage(void);
BOOL ImportCertificate(
         LPSTR szCertFile,
         LPSTR szKeyFile,
         LPSTR szPassword,
         LPSTR szStore,
         BOOL bUser,
         DWORD dwProviderNum);

BOOL CreateCertificate(
         LPSTR szX509,
         DWORD dwKeyType,
         LPSTR szSigAlg,
         WORD wMonths,
         DWORD dwCertEnhKeyUsage,
         BOOL bCA,
         BOOL bUseProv,
         DWORD dwProviderNum,
         LPSTR szSubjectStore,
         BOOL bSubjectUser,
         BOOL bSelfSigned,
         LPSTR szIssuerName,
         LPSTR szIssuerStore,
         BOOL bIssuerUser,
         BOOL bExport,
         LPSTR szCertFile,
         LPSTR szKeyFile,
         LPSTR szPassword);

PCCERT_CONTEXT FindCertificate(
         LPSTR szCertName,
         LPSTR szStore,
         DWORD dwFlags,
         PCRYPT_DATA_BLOB *KeyId,
         HCRYPTPROV *hProv,
         LPDWORD dwKeyType);

BOOL WINAPI MyCryptEnumProviders(
         DWORD dwIndex,
         LPDWORD pdwProvType,
         LPTSTR szProvName,
         LPDWORD pcbProvName);

#endif