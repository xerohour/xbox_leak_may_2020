//===================================================================
//
// Copyright Microsoft Corporation. All Rights Reserved.
//
//===================================================================

#ifndef __CRYPTCAB_H__
#define __CRYPTCAB_H__

#include <wincrypt.h>
#include <cabinet.h>

#include "xcabinet.h"
#include "cabarc.h"

#ifndef RIP_ON_NOT_TRUE
#if DBG
#define RIP_ON_NOT_TRUE(s, c)
#else
#define RIP_ON_NOT_TRUE(s, c)
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// Well-known symmetric key length
//
#define XONLINE_KEY_LENGTH							16

//
// Function to initialize the crypto facilities 
//
HRESULT InitializeCrypto(
			PBYTE		pbPrivateKey,
			DWORD		cbPrivateKey,
			PBYTE		pbPublicKey,
			DWORD		cbPublicKey,
			HCRYPTPROV 	*phCrypto,
			HCRYPTKEY	*phKeyPublic,
			HCRYPTKEY	*phKeyPrivate,
			DWORD		dwFlags
			);

#define CRYPTCAB_FLAG_DONT_CREATE		(0x00000001)			

//
// Function to just generate a string of random numbers
//
HRESULT GenerateRandomBytes(
			HCRYPTPROV	hCrypto,
			PBYTE		pbData,
			DWORD		cbData
			);

//
// Function to generate a confounder
//
HRESULT GenerateConfounder(
			HCRYPTPROV	hCrypto,
			PBYTE		pbConfounder
			);

//
// Function to generate a random symmetric key
//
HRESULT GenerateSymmetricKey(
			HCRYPTPROV	hCrypto,
			PBYTE		pbSymmetricKey
			);

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
    );

/*
#define RC4_CONFOUNDER_LEN 8

typedef struct _RC4_SHA_HEADER {
    UCHAR Checksum[A_SHA_DIGEST_LEN];
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
} RC4_SHA_HEADER, *PRC4_SHA_HEADER;
*/

static void rc4HmacEncrypt(
    IN PUCHAR confounder, // RC4_CONFOUNDER_LEN bytes
    IN PUCHAR pbKey,
    IN ULONG cbKey,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    OUT PUCHAR pbHeader // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes
    );

static BOOL rc4HmacDecrypt(
    IN PUCHAR pbKey,
    IN ULONG cbKey,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    IN PUCHAR pbHeader // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes
    );

//
// Function to sign the cabinet file header, and the signature will be verified
// with the supplied public key
//
HRESULT SignCabinetHeader(
			HANDLE		hFile,
			HCRYPTPROV	hCrypto,
			HCRYPTKEY	hKeyPublic
			);

//
// Function to sign the cabinet file header using the DevKit private key, and the 
// signature will be verified with the DevKit public key
//
HRESULT SignCabinetHeaderDevKit(
			HANDLE		hFile
			);

//
// Function to encrypt a Xbox content cabinet file using the specified
// symmetric key.
//
// The cabinet file must be created in the correct Xbox content format
// with apporpriate space reservations for hashes and signatures.
//
HRESULT EncryptCabinetBody(
			HANDLE		hFile,
			HCRYPTPROV	hCrypto,
			PBYTE		pbSymmetricKey,
			DWORD		cbSymmetricKey
			);

//
// Function to encrypt the header portion of the CAB file. This should
// be the last step following placing the digital signature in the slot
//
HRESULT EncryptCabinetHeader(
			HANDLE		hFile,
			HCRYPTPROV	hCrypto,
			PBYTE		pbSymmetricKey,
			DWORD		cbSymmetricKey
			);

//
// Function to encrypt a symmetric key with a supplied public key and place
// the encrypted contents into the signature slot of the package
//
HRESULT EncloseSymmetricKey(
			HANDLE		hFile,
			HCRYPTKEY	hKeyPublic,
			BYTE		*pbSymmetricKey,
			DWORD		cbSymmetricKey
			);

//
// Function to retrieve a symmetric key previously encrypted with a
// public key, whose corresponding private key is supplied.
//
HRESULT RetrieveSymmetricKey(
			HANDLE		hFile,
			HCRYPTKEY	hKeyPrivate,
			BYTE		*pbSymmetricKey,
			DWORD		cbSymmetricKey
			);
			
//
// Function to append a file (by name) to the end of an open file (by handle).
// The appended contents can be optionally encrypted if a symmetric key is provided
//
HRESULT AppendEncryptedFile(
			HANDLE				hFile,
			CHAR				*szFileToAppend,
			HCRYPTPROV			hCrypto,
			PBYTE				pbSymmetricKey,
			DWORD				cbSymmetricKey
			);

//
// Function to create a manifest file and save it to a file named
// XONLINECONTENT_MANIFEST_FILE_NAME. The manifest file will then 
// be signed using the current symmetric key. If this file exists, 
// this function will fail.
//
HRESULT CreateNewManifestFile(
			HCRYPTPROV	hCrypto,
			PBYTE		pbKey,
			CHAR		*szPath
			);

//
// Function to determine if a directory name is a valid title ID.
// If so, then the directory name will be converted to the Title ID 
// value. Otherwise, this function will return ERROR_INVALID_DATA.
//
HRESULT ConvertToTitleId(
			PSTR	szTitleId,
			DWORD	*pdwTitleId
			);

//
// Helper function to convert a symmetric key from a hex string
// to its binary representation
//
HRESULT ConvertSymmetricKeyToBinary(
			CHAR		*szSymmetricKey,
			DWORD		cchSymmetricKey,
			PBYTE		pbSymmetricKey,
			DWORD		*pcbSymmetricKey
			);

//
// Function to create a symmetric key and save it to a named file.
// If this file exists, this function will fail.
//
HRESULT ExportSymmetricKey(
			HCRYPTPROV	hCrypto,
			CHAR		*szFileName,
			PBYTE		pbSymmetricKey,
			DWORD		cbSymmetricKey
			);
			
//
// Function to import a symmetric key from the named file. 
// If this file does not exist or is malformed, this function 
// fails
//
HRESULT ImportSymmetricKey(
			CHAR	*szFileName,
			PBYTE	pbSymmetricKey,
			DWORD	cbSymmetricKey
			);

//
// Function to read key information from a binary file
//
HRESULT ReadPrivatePublicKeyFile(
			PSTR	szFilename,
			PBYTE	*ppbKey,
			DWORD	*pcbKey
			);

//
// Munge a public key exported by Crypto into a format that
// can directly be used by XcVerifyPKCS1Signature (i.e. Xbox 
// client). The exported blob must not have a password.
//
HRESULT MungePublicKey(
            PBYTE pbKey, 
            DWORD cbKey,
            PBYTE *ppbMungedKey, 
            DWORD *pcbMungedKey
            );

//
// Function to rebuild a package received from a publisher. This consists of
// the following steps:
//
// - Make a copy of the original package
// - Extract and decrypt symmetric key from package
// - Extract and decrypt pusblisher-supplied metadata file from package
// - Verify and decrypt package and write out raw cabinet file
// - Re-encrypt package with new symmetric key
// - Export new symmetric key to a file
//
HRESULT RebuildContentPackage(
			CHAR		*szFilePath,
			DWORD		OfferingId,
			DWORD		dwTitleId,
			WORD		wVerHi,
			WORD		wVerLo,
			PBYTE		pbPrivateKey,
			DWORD		cbPrivateKey
			);

#ifdef __cplusplus
}
#endif

#endif  //__CRYPTCAB_H__


