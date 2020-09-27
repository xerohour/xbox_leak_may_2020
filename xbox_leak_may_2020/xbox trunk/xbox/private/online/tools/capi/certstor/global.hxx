//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1996 - 1996
//
//  File:       global.h
//
//  Contents:   Top level internal header file for CertStor APIs. This file
//              includes all base header files and contains other global
//              stuff.
//
//  History:    14-May-96   kevinr   created
//
//--------------------------------------------------------------------------
#ifndef _XBOX	// dont enable CMS_PKCS7 for Xbox
#define CMS_PKCS7       1
#endif
// #define DEBUG_CRYPT_ASN1_MASTER  1

#ifdef CMS_PKCS7
#define CMSG_SIGNER_ENCODE_INFO_HAS_CMS_FIELDS      1
#define CRYPT_SIGN_MESSAGE_PARA_HAS_CMS_FIELDS      1
#define CMSG_ENVELOPED_ENCODE_INFO_HAS_CMS_FIELDS   1
#endif  // CMS_PKCS7

#define STRUCT_CBSIZE(StructName, FieldName)   \
    (offsetof(StructName, FieldName) + sizeof(((StructName *) 0)->FieldName))


#include <windows.h>
typedef ULONG REGSAM;
#define assert(exp)     ((void)0)
//#include <assert.h>
#include <malloc.h>
//#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
//#include <time.h>
//#include <crtdbg.h>

//#include <userenv.h>
//#include <shlobj.h>

#include "crtem.h"

#include "wincrypt.h"
#include "unicode.h"
#include "crypttls.h"
#include "crypthlp.h"
//#include "certprot.h"
#include "pkialloc.h"
#include "asn1util.h"
#include "pkiasn1.h"
#include "utf8.h"

#include "msr_md5.h"
#include "sha.h"


extern HMODULE hCertStoreInst;
#include "resource.h"

#ifdef __cplusplus
extern "C" {
#endif

// # of bytes for a hash. Such as, SHA (20) or MD5 (16)
#define MAX_HASH_LEN                20
#define SHA1_HASH_LEN               20

// Null terminated ascii hex characters of the hash.
// For Win95 Remote Registry Access:: Need extra character
#define MAX_CERT_REG_VALUE_NAME_LEN (2 * MAX_HASH_LEN + 1 + 1)
#define MAX_HASH_NAME_LEN           MAX_CERT_REG_VALUE_NAME_LEN

//+-------------------------------------------------------------------------
//  Compares the certificate's public key with the provider's public key
//  to see if they are identical.
//
//  Returns TRUE if the keys are identical.
//
//  Implemented in certhlpr.cpp.
//--------------------------------------------------------------------------
BOOL
WINAPI
I_CertCompareCertAndProviderPublicKey(
    IN PCCERT_CONTEXT pCert,
    IN HCRYPTPROV hProv,
    IN DWORD dwKeySpec
    );

//+-------------------------------------------------------------------------
//  Register wait for callback functions
//
//  Implemented in logstor.cpp.
//--------------------------------------------------------------------------
typedef VOID (NTAPI * ILS_WAITORTIMERCALLBACK) (PVOID, BOOLEAN );

HANDLE
WINAPI
ILS_RegisterWaitForSingleObjectEx(
    HANDLE hObject,
    ILS_WAITORTIMERCALLBACK Callback,
    PVOID Context,
    ULONG dwMilliseconds,
    ULONG dwFlags
    );

BOOL
WINAPI
ILS_UnregisterWait(
    HANDLE WaitHandle
    );

BOOL
WINAPI
ILS_ExitWait(
    HANDLE WaitHandle
    );

//+-------------------------------------------------------------------------
//  Registry support functions
//
//  Implemented in logstor.cpp.
//--------------------------------------------------------------------------

void
ILS_CloseRegistryKey(
    IN HKEY hKey
    );

BOOL
ILS_ReadDWORDValueFromRegistry(
    IN HKEY hKey,
    IN LPCWSTR pwszValueName,
    IN DWORD *pdwValue
    );

BOOL
ILS_ReadBINARYValueFromRegistry(
    IN HKEY hKey,
    IN LPCWSTR pwszValueName,
    OUT BYTE **ppbValue,
    OUT DWORD *pcbValue
    );

//+-------------------------------------------------------------------------
//  Key Identifier registry and roaming file support functions
//
//  Implemented in logstor.cpp.
//--------------------------------------------------------------------------
BOOL
ILS_ReadKeyIdElement(
    IN const CRYPT_HASH_BLOB *pKeyIdentifier,
    IN BOOL fLocalMachine,
    IN OPTIONAL LPCWSTR pwszComputerName,
    OUT BYTE **ppbElement,
    OUT DWORD *pcbElement
    );
BOOL
ILS_WriteKeyIdElement(
    IN const CRYPT_HASH_BLOB *pKeyIdentifier,
    IN BOOL fLocalMachine,
    IN OPTIONAL LPCWSTR pwszComputerName,
    IN const BYTE *pbElement,
    IN DWORD cbElement
    );
BOOL
ILS_DeleteKeyIdElement(
    IN const CRYPT_HASH_BLOB *pKeyIdentifier,
    IN BOOL fLocalMachine,
    IN OPTIONAL LPCWSTR pwszComputerName
    );

typedef BOOL (*PFN_ILS_OPEN_KEYID_ELEMENT)(
    IN const CRYPT_HASH_BLOB *pKeyIdentifier,
    IN const BYTE *pbElement,
    IN DWORD cbElement,
    IN void *pvArg
    );

BOOL
ILS_OpenAllKeyIdElements(
    IN BOOL fLocalMachine,
    IN OPTIONAL LPCWSTR pwszComputerName,
    IN void *pvArg,
    IN PFN_ILS_OPEN_KEYID_ELEMENT pfnOpenKeyId
    );

//+-------------------------------------------------------------------------
//  Protected Root functions
//
//  Implemented in protroot.cpp.
//--------------------------------------------------------------------------
#ifndef CE_BUILD
BOOL
IPR_IsCurrentUserRootsAllowed();

BOOL
IPR_OnlyLocalMachineGroupPolicyRootsAllowed();

void
IPR_InitProtectedRootInfo();

BOOL
IPR_DeleteUnprotectedRootsFromStore(
    IN HCERTSTORE hStore,
    OUT BOOL *pfProtected
    );

int
IPR_ProtectedRootMessageBox(
    IN PCCERT_CONTEXT pCert,
    IN UINT wActionID,
    IN UINT uFlags
    );
#else
#define IPR_IsCurrentUserRootsAllowed() (TRUE)
#define IPR_OnlyLocalMachineGroupPolicyRootsAllowed() (FALSE)
inline void IPR_InitProtectedRootInfo() {return;}
inline BOOL IPR_DeleteUnprotectedRootsFromStore(IN HCERTSTORE hStore, OUT BOOL *pfProtected)
	{
		*pfProtected = FALSE;
		return TRUE;
	}
// BUGBUG: This removes UI notification on root changes. Should look for a way to conditionally enable it	
#define IPR_ProtectedRootMessageBox(pCert, wActionID, uFlags) (IDYES)
#endif
//+-------------------------------------------------------------------------
//  Find chain helper functions
//
//  Implemented in fndchain.cpp.
//--------------------------------------------------------------------------
BOOL IFC_IsEndCertValidForUsage(
    IN PCCERT_CONTEXT pCert,
    IN LPCSTR pszUsageIdentifier
    );

BOOL IFC_IsEndCertValidForUsages(
    IN PCCERT_CONTEXT pCert,
    IN PCERT_ENHKEY_USAGE pUsage,
    IN BOOL fOrUsage
    );

#include "x509.h"


//+-------------------------------------------------------------------------
//  Convert to/from EncodedOID
//
//  Implemented in oidconv.cpp.
//--------------------------------------------------------------------------
BOOL
WINAPI
I_CryptOIDConvDllMain(
        HMODULE hInst,
        ULONG  ulReason,
        LPVOID lpReserved
        );

BOOL
WINAPI
I_CryptSetEncodedOID(
        IN LPSTR pszObjId,
        OUT ASN1encodedOID_t *pEncodedOid
        );
void
WINAPI
I_CryptGetEncodedOID(
        IN ASN1encodedOID_t *pEncodedOid,
        IN DWORD dwFlags,
        OUT LPSTR *ppszObjId,
        IN OUT BYTE **ppbExtra,
        IN OUT LONG *plRemainExtra
        );


#ifdef __cplusplus
}       // Balance extern "C" above
#endif

#include "ekuhlpr.h"
#include "origin.h"
#include <scrdcert.h>
#include <scstore.h>

#pragma hdrstop

