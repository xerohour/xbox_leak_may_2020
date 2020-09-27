//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992 - 1997.
//
//  File:       pfx.h
//
//  Contents:   PFX (PKCS #12) function defintions and types
//
//----------------------------------------------------------------------------
#ifndef PFX_H
#define PFX_H

// Flag definitions for PFXExportCertStore
#define REPORT_NO_PRIVATE_KEY                   0x0001
#define REPORT_NOT_ABLE_TO_EXPORT_PRIVATE_KEY   0x0002
#define EXPORT_PRIVATE_KEYS                     0x0004
#define PKCS12_IMPORT_RESERVED_MASK             0xffff0000


//+-------------------------------------------------------------------------
//      PFXExportCertStore
//
//  Export the certificates and private keys referenced in the passed-in store 
//
//  This is an old API kept for compatibility with IE4 clients. New applications
//  should call PfxExportCertStoreEx for enhanced security.
//
//  The value passed in the password parameter will be used to encrypt and 
//  verify the integrity of the PFX packet. If any problems encoding the store
//  are encountered, the function will return FALSE and the error code can 
//  be found from GetLastError(). 
//
//  The dwFlags parameter may be set to any combination of 
//      EXPORT_PRIVATE_KEYS
//      REPORT_NO_PRIVATE_KEY
//      REPORT_NOT_ABLE_TO_EXPORT_PRIVATE_KEY
//  These flags are as documented in the CertExportSafeContents Crypt32 API
//--------------------------------------------------------------------------
BOOL
WINAPI
PFXExportCertStore(
    HCERTSTORE hStore,
    CRYPT_DATA_BLOB* pPFX,
    LPCWSTR szPassword,
    DWORD   dwFlags);


// Flag definitions for PFXImportCertStore
//#define CRYPT_EXPORTABLE        0x00000001  // reserved by wincrypt.h; can't reuse
//#define CRYPT_USER_PROTECTED    0x00000002  // reserved by wincrypt.h; can't reuse
//#define CRYPT_MACHINE_KEYSET    0x00000020  // reserved by wincrypt.h; can't reuse
#define CRYPT_USER_KEYSET         0x00001000     

#define PKCS12_IMPORT_RESERVED_MASK    0xffff0000 

//+-------------------------------------------------------------------------
//      PFXImportCertStore
//
//  Import the PFX blob and return a store containing certificates
//
//  if the password parameter is incorrect or any other problems decoding
//  the PFX blob are encountered, the function will return NULL and the
//      error code can be found from GetLastError(). 
//
//  The dwFlags parameter may be set to the following:
//  CRYPT_EXPORTABLE - specify that any imported keys should be marked as 
//                     exportable (see documentation on CryptImportKey)
//  CRYPT_USER_PROTECTED - (see documentation on CryptImportKey)
//  CRYPT_MACHINE_KEYSET - used to force the private key to be stored in the
//                        the local machine and not the current user.
//  CRYPT_USER_KEYSET - used to force the private key to be stored in the
//                      the current user and not the local machine, even if
//                      the pfx blob specifies that it should go into local machine.
//--------------------------------------------------------------------------
HCERTSTORE
WINAPI
PFXImportCertStore(
    CRYPT_DATA_BLOB* pPFX,
    LPCWSTR szPassword,
    DWORD   dwFlags);


//+-------------------------------------------------------------------------
//      IsPFXBlob
//
//  This function will try to decode the outer layer of the blob as a pfx 
//  blob, and if that works it will return TRUE, it will return FALSE otherwise
//
//--------------------------------------------------------------------------
BOOL
WINAPI
PFXIsPFXBlob(
    CRYPT_DATA_BLOB* pPFX);



////////////////////////////////
// New interfaces are c-style

#if defined(__cplusplus)
extern "C" {
#endif

//+-------------------------------------------------------------------------
//      VerifyPassword
//
//  This function will attempt to decode the outer layer of the blob as a pfx 
//  blob and decrypt with the given password. No data from the blob will be imported.
//  Return value is TRUE if password appears correct, FALSE otherwise.
//
//--------------------------------------------------------------------------
BOOL 
WINAPI
PFXVerifyPassword(
    CRYPT_DATA_BLOB* pPFX,
    LPCWSTR szPassword,
    DWORD dwFlags);


//+-------------------------------------------------------------------------
//      PFXExportCertStoreEx
//
//  Export the certificates and private keys referenced in the passed-in store 
//
//  This API encodes the blob under a stronger algorithm. The resulting
//  PKCS12 blobs are incompatible with the earlier APIs.
//
//  The value passed in the password parameter will be used to encrypt and 
//  verify the integrity of the PFX packet. If any problems encoding the store
//  are encountered, the function will return FALSE and the error code can 
//  be found from GetLastError(). 
//
//  The dwFlags parameter may be set to any combination of 
//      EXPORT_PRIVATE_KEYS
//      REPORT_NO_PRIVATE_KEY
//      REPORT_NOT_ABLE_TO_EXPORT_PRIVATE_KEY
//  These flags are as documented in the CertExportSafeContents Crypt32 API
//--------------------------------------------------------------------------
BOOL
WINAPI
PFXExportCertStoreEx(
    HCERTSTORE hStore,
    CRYPT_DATA_BLOB* pPFX,
    LPCWSTR szPassword,
    void*   pvReserved,
    DWORD   dwFlags);



#if defined(__cplusplus)
}
#endif

#endif // PFX_H
