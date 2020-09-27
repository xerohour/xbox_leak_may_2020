//===================================================================
//
// Copyright Microsoft Corporation. All Rights Reserved.
//
//===================================================================
#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <xbox.h>
#include <wsockntp.h>
#include <xonlinep.h>

#include <assert.h>

#include "cabarc.h"
#include "rebldcab.h"
#include "metadata.h"

#include "rsa.h"
#include "sha.h"
#include "rc4.h"

#include "devkitkeys.c"

#define XBOX_HD_SECTOR_SIZE	512

#define RIP(a)

#ifndef RtlRip
VOID RtlRip(PVOID pv1, PVOID pv2, PVOID pv3)
{
	assert(pv2);
}
#endif

//
// Define our own crypto provider
//
#define CRYPTCAB_CRYPTO_PROVIDER		"XboxOnlineContent"


#include "cabinet.h"

#include "cryptcab.h"

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
			)
{
    HRESULT 	hr = S_OK;
	HCRYPTPROV	hCrypto = 0;
	HCRYPTKEY	hKeyPrivate = 0;
	HCRYPTKEY	hKeyPublic = 0;

	// Try to open our container
    if (!CryptAcquireContext(
    			&hCrypto, 
    			CRYPTCAB_CRYPTO_PROVIDER, 
    			MS_ENHANCED_PROV, 
    			PROV_RSA_FULL, 
    			0))
	{
		hCrypto = 0;
		if ((dwFlags & CRYPTCAB_FLAG_DONT_CREATE) == 0)
		{
			// Try to create it
		    if (!CryptAcquireContext(&hCrypto,
						CRYPTCAB_CRYPTO_PROVIDER,
						MS_ENHANCED_PROV,
						PROV_RSA_FULL,
						CRYPT_NEWKEYSET))
		    {
		        hr = GetLastError();
		        hCrypto = 0;
			    goto Error;
		    }
		}
		else
		{
			hr = GetLastError();
			goto Error;
		}
	}

    // Import the private key
    if (pbPrivateKey)
    {
	    if (!CryptImportKey(hCrypto, 
	    			pbPrivateKey, 
	    			cbPrivateKey, 
	    			0, 
	    			0, 
	    			&hKeyPrivate))
	    {
	        hr = GetLastError();
		    goto Error;
	    }
	}

    // Import the public key
    if (pbPublicKey)
    {
	    if (!CryptImportKey(hCrypto, 
	    			pbPublicKey, 
	    			cbPublicKey, 
	    			0, 
	    			0, 
	    			&hKeyPublic))
	    {
	        hr = GetLastError();
		    goto Error;
	    }
	}

	// Return these handles
	*phCrypto = hCrypto;
	if (phKeyPublic)
		*phKeyPublic = hKeyPublic;
	if (phKeyPrivate)
		*phKeyPrivate = hKeyPrivate;

Exit:
    return(hr);

Error:
	if (hCrypto)
        CryptReleaseContext(hCrypto, 0);
	goto Exit;
}

//
// Function to just generate a string of random numbers
//
HRESULT GenerateRandomBytes(
			HCRYPTPROV	hCrypto,
			PBYTE		pbData,
			DWORD		cbData
			)
{
	if (!CryptGenRandom(hCrypto, cbData, pbData))
		return(HRESULT_FROM_WIN32(GetLastError()));
	return(S_OK);
}

//
// Function to generate a confounder
//
HRESULT GenerateConfounder(
			HCRYPTPROV	hCrypto,
			PBYTE		pbConfounder
			)
{
	return(GenerateRandomBytes(hCrypto, pbConfounder, RC4_CONFOUNDER_LEN));
}

//
// Function to generate a random symmetric key
//
HRESULT GenerateSymmetricKey(
			HCRYPTPROV	hCrypto,
			PBYTE		pbSymmetricKey
			)
{
	return(GenerateRandomBytes(hCrypto, pbSymmetricKey, XONLINE_KEY_LENGTH));
}

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
    )
{
#define HMAC_K_PADSIZE              64
    BYTE Kipad[HMAC_K_PADSIZE];
    BYTE Kopad[HMAC_K_PADSIZE];
    BYTE HMACTmp[HMAC_K_PADSIZE+A_SHA_DIGEST_LEN];
    ULONG dwBlock;
    A_SHA_CTX shaHash;

    // truncate
    if (cbKeyMaterial > HMAC_K_PADSIZE)
        cbKeyMaterial = HMAC_K_PADSIZE;

    RtlZeroMemory(Kipad, HMAC_K_PADSIZE);
    RtlCopyMemory(Kipad, pbKeyMaterial, cbKeyMaterial);

    RtlZeroMemory(Kopad, HMAC_K_PADSIZE);
    RtlCopyMemory(Kopad, pbKeyMaterial, cbKeyMaterial);

    //
    // Kipad, Kopad are padded sMacKey. Now XOR across...
    //
    for(dwBlock=0; dwBlock<HMAC_K_PADSIZE/sizeof(DWORD); dwBlock++)
    {
        ((DWORD*)Kipad)[dwBlock] ^= 0x36363636;
        ((DWORD*)Kopad)[dwBlock] ^= 0x5C5C5C5C;
    }

    //
    // prepend Kipad to data, Hash to get H1
    //

    A_SHAInit(&shaHash);
    A_SHAUpdate(&shaHash, Kipad, HMAC_K_PADSIZE);
    if (cbData != 0)
    {
        A_SHAUpdate(&shaHash, pbData, cbData);
    }
    if (cbData2 != 0)
    {
        A_SHAUpdate(&shaHash, pbData2, cbData2);
    }

    // Finish off the hash
    A_SHAFinal(&shaHash,HMACTmp+HMAC_K_PADSIZE);

    // prepend Kopad to H1, hash to get HMAC
    RtlCopyMemory(HMACTmp, Kopad, HMAC_K_PADSIZE);

    // final hash: output value into passed-in buffer
    A_SHAInit(&shaHash);
    A_SHAUpdate(&shaHash,HMACTmp, sizeof(HMACTmp));
    A_SHAFinal(&shaHash,HmacData);
}

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
    )
{
    PRC4_SHA_HEADER CryptHeader = (PRC4_SHA_HEADER) pbHeader;
    BYTE LocalKey[A_SHA_DIGEST_LEN];
    RC4_KEYSTRUCT Rc4KeyStruct;

    //
    // Create the header - the confounder & checksum
    //
    RtlZeroMemory( CryptHeader->Checksum, A_SHA_DIGEST_LEN );
    RtlCopyMemory( CryptHeader->Confounder, confounder, RC4_CONFOUNDER_LEN );

    //
    // Checksum everything but the checksum
    //
    shaHmac( pbKey, cbKey,
             CryptHeader->Confounder, RC4_CONFOUNDER_LEN,
             pbInput, cbInput,
             CryptHeader->Checksum );

    //
    // HMAC the checksum into the key
    //
    shaHmac( pbKey, cbKey,
             CryptHeader->Checksum, A_SHA_DIGEST_LEN,
             NULL, 0,
             LocalKey );

    //
    // Use the generated key as the RC4 encryption key
    //
    rc4_key( &Rc4KeyStruct, A_SHA_DIGEST_LEN, LocalKey );

    //
    // Encrypt everything but the checksum
    //
    rc4( &Rc4KeyStruct, RC4_CONFOUNDER_LEN, CryptHeader->Confounder );
    rc4( &Rc4KeyStruct, cbInput, pbInput );
}

static BOOL rc4HmacDecrypt(
    IN PUCHAR pbKey,
    IN ULONG cbKey,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    IN PUCHAR pbHeader // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes
    )
{
    PRC4_SHA_HEADER CryptHeader = (PRC4_SHA_HEADER) pbHeader;
    RC4_SHA_HEADER TempHeader;
    BYTE Confounder[RC4_CONFOUNDER_LEN];
    BYTE LocalKey[A_SHA_DIGEST_LEN];
    RC4_KEYSTRUCT Rc4KeyStruct;

    RtlCopyMemory( TempHeader.Confounder, CryptHeader->Confounder, RC4_CONFOUNDER_LEN );

    //
    // HMAC the checksum into the key
    //
    shaHmac( pbKey, cbKey,
             CryptHeader->Checksum, A_SHA_DIGEST_LEN,
             NULL, 0,
             LocalKey );

    //
    // Use the generated key as the RC4 encryption key
    //
    rc4_key( &Rc4KeyStruct, A_SHA_DIGEST_LEN, LocalKey );

    //
    // Decrypt confounder and data
    //
    rc4( &Rc4KeyStruct, RC4_CONFOUNDER_LEN, TempHeader.Confounder );
    rc4( &Rc4KeyStruct, cbInput, pbInput );

    //
    // Now verify the checksum.
    //
    shaHmac( pbKey, cbKey,
             TempHeader.Confounder, RC4_CONFOUNDER_LEN,
             pbInput, cbInput,
             TempHeader.Checksum );

    //
    // Decrypt is successful only if checksum matches
    //
    return ( RtlEqualMemory(
                 TempHeader.Checksum,
                 CryptHeader->Checksum,
                 A_SHA_DIGEST_LEN) );
}

//
// Function the sanity check the CAB header
//
HRESULT VerifyCabHeader(
			PXONLINECONTENT_HEADER	pheader,
			PLARGE_INTEGER			pliFileSize
			)
{
	CFHEADER				*pcfheader;
	
	pcfheader = &pheader->cfheader;
	if (pcfheader->sig != sigCFHEADER)
	{
		printf("Invalid signature.\n");
		goto ErrorInvalidData;
	}

	if (pcfheader->version != verCF)
	{
		printf("Invalid CAB version.\n");
		goto ErrorInvalidData;
	}

	if (pliFileSize && (pcfheader->cbCabinet != pliFileSize->QuadPart))
	{
		printf("Invalid file size.\n");
		goto ErrorInvalidData;
	}

	if ((pcfheader->flags & cfhdrFLAGS_ALL) != cfhdrRESERVE_PRESENT)
	{
		printf("Invalid CAB flags.\n");
		goto ErrorInvalidData;
	}

	if (pcfheader->iCabinet != 0)
	{
		printf("iCabinet != 0.\n");
		goto ErrorInvalidData;
	}

	if (pheader->cfreserve.cbCFHeader != 
				 XONLINECONTENT_HEADER_RESERVE_SIZE)
	{
		printf("Invalid header reserve size.\n");
		goto ErrorInvalidData;
	}

	if (pheader->cfreserve.cbCFFolder != 
				 XONLINECONTENT_PERFOLDER_RESERVE_SIZE)
	{				 
		printf("Invalid per-folder reserve size.\n");
		goto ErrorInvalidData;
	}

	if (pcfheader->cFolders > XONLINECONTENT_MAX_CFFOLDER)
	{
		printf("Too many folders.\n");
		goto ErrorInvalidData;
	}
	
	return(S_OK);

ErrorInvalidData:
	return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
}

//
// Function the sanity check the CFFOLDER records
//
HRESULT VerifyFolderEntries(
			PXONLINECONTENT_HEADER	pHeader,
			PCFFOLDER_HMAC			rgcffolderhmac
			)
{
	HRESULT	hr = S_OK;
	DWORD	i;

	for (i = 0; i < pHeader->cfheader.cFolders; i++)
	{
		if (rgcffolderhmac[i].cffolder.coffCabStart >= 
					(DWORD)(pHeader->cfheader.cbCabinet))
			return(ERROR_INVALID_DATA);
	}
	return(S_OK);
}

//
// Function to load a cabinet header
//
HRESULT LoadCabinetHeader(
			HANDLE					hFile,
			PXONLINECONTENT_HEADER	pheader
			)
{
	HRESULT					hr = S_OK;
	LARGE_INTEGER			liTemp;
	DWORD					cbData;

	// Read the CAB header
	liTemp.QuadPart = 0;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	if (!ReadFile(hFile, pheader, XONLINECONTENT_FIXED_HEADER_SIZE, &cbData, NULL))
	{
		hr = GetLastError();
		printf("Read header failed with %x\n", hr);
		goto Error;
	}

	if (cbData != XONLINECONTENT_FIXED_HEADER_SIZE)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		printf("Size mismatch.\n");
		goto Error;
	}

	if (!GetFileSizeEx(hFile, &liTemp))
	{
		hr = GetLastError();
		printf("GetFileSizeEx failed with %x\n", hr);
		goto Error;
	}

	// Validate the results
	hr = VerifyCabHeader(pheader, &liTemp);
	if (hr != S_OK)
		goto Error;

Error:
	return(hr);
}

//
// Function to sign the cabinet file header, and the signature will be verified
// with the supplied public key
//
HRESULT SignCabinetHeader(
			HANDLE		hFile,
			HCRYPTPROV	hCrypto,
			HCRYPTKEY	hKeyPublic
			)
{
	HRESULT					hr = S_OK;
	HCRYPTHASH				hHash = 0;
	BYTE					rgbSignature[2048];
	BYTE					rgbHeader[XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC];
	PBYTE					pbSignature = rgbSignature;
	LARGE_INTEGER			liTemp;
	DWORD					cbData;

	// Read the CAB header
	liTemp.QuadPart = 0;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	if (!ReadFile(hFile, rgbHeader, 
				XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC, 
				&cbData, NULL))
	{
		hr = GetLastError();
		printf("Read header failed with %x\n", hr);
		goto Error;
	}

	if (cbData != XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		printf("Size mismatch.\n");
		goto Error;
	}

	// Create hash
	if (!CryptCreateHash(hCrypto, CALG_SHA1, 0, 0, &hHash))
	{
		hr = GetLastError();
		printf("Cannot create hash %x.\n", hr);
		goto Error;
	}

	if (!CryptHashData(hHash, rgbHeader, 
				XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC, 0))
	{
		hr = GetLastError();
		printf("Cannot hash data %x.\n", hr);
		goto Error;
	}

	// Get the hash size
	if (!CryptSignHash(hHash, AT_KEYEXCHANGE, NULL, 0, NULL, &cbData))
	{
		hr = GetLastError();
		printf("Cannot sign hash %x.\n", hr);
		goto Error;
	}

	if (cbData > sizeof(rgbSignature))
	{
		// Allocate new buffer
		pbSignature = LocalAlloc(LMEM_FIXED, cbData);
		if (!pbSignature)
		{
			hr = E_OUTOFMEMORY;
			printf("Cannot allocate signing buffer %x.\n", hr);
			goto Error;
		}
	}

	// Sign the hash
	if (!CryptSignHash(hHash, AT_KEYEXCHANGE, NULL, 0, pbSignature, &cbData))
	{
		hr = GetLastError();
		printf("Cannot sign hash %x.\n", hr);
		goto Error;
	}

	if (cbData != XONLINECONTENT_PK_SIGNATURE_SIZE)
	{
		hr = ERROR_INVALID_DATA;
		printf("Unexpected signature size (expected: %u, got %u)\n", 
					XONLINECONTENT_PK_SIGNATURE_SIZE, cbData);
		goto Error;
	}

	// Close the hash
	CryptDestroyHash(hHash);
	hHash = 0;

	//
	// Verify that the public key properly verifies the signature
	//

	// Create hash
	if (!CryptCreateHash(hCrypto, CALG_SHA1, 0, 0, &hHash))
	{
		hr = GetLastError();
		printf("Cannot create hash %x.\n", hr);
		goto Error;
	}

	if (!CryptHashData(hHash, rgbHeader, 
				XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC, 0))
	{
		hr = GetLastError();
		printf("Cannot hash data %x.\n", hr);
		goto Error;
	}

	// Verify the signature with the public key
	if (!CryptVerifySignature(hHash, pbSignature, cbData, hKeyPublic, NULL, 0))
	{
		hr = GetLastError();
		printf("Hash and signature do not match up %x.\n", hr);
		goto Error;
	}

	// Close the hash
	CryptDestroyHash(hHash);
	hHash = 0;
	
	// Write out the signature
	liTemp.QuadPart = XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC_AND_CHECKPOINT;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	if (!WriteFile(hFile, pbSignature, 
				XONLINECONTENT_PK_SIGNATURE_SIZE, &cbData, NULL) ||
		(cbData != XONLINECONTENT_PK_SIGNATURE_SIZE))
	{
		hr = GetLastError();
		printf("Cannot write header, %x.\n", hr);
		goto Error;
	}

Exit:
	if (hHash)
		CryptDestroyHash(hHash);

	return(hr);

Error:
	goto Exit;
}

#if 0
//
// Function to sign the cabinet file header using the DevKit private key, and the 
// signature will be verified with the DevKit public key
//
HRESULT SignCabinetHeaderDevKit(
			HANDLE		hFile
			)
{
	HRESULT					hr = S_OK;
	BYTE					rgbHash[XC_DIGEST_LEN];
	BYTE					rgbSignature[2048];
	BYTE					rgbWorkspace[2048];
	BYTE					rgbHeader[XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC];
	PBYTE					pbSignature = rgbSignature;
	LARGE_INTEGER			liTemp;
	DWORD					cbData;

	// Read the CAB header
	liTemp.QuadPart = 0;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	if (!ReadFile(hFile, rgbHeader, 
				XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC, 
				&cbData, NULL))
	{
		hr = GetLastError();
		printf("Read header failed with %x\n", hr);
		goto Error;
	}

	if (cbData != XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		printf("Size mismatch.\n");
		goto Error;
	}

	// Sign the digest
	XCCalcDigest(rgbHeader, 
				XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC, rgbHash);
	XCSignDigest(rgbHash, ImgbPrivateKeyData, rgbSignature);				

	//
	// Verify that the public key properly verifies the signature
	//
	if (!XCVerifyDigest(rgbSignature, ImgbPublicKeyData, 
				rgbWorkspace, rgbHash))
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		printf("Failed to verify signed digest.\n");
		goto Error;
	}

	// Write out the signature
	liTemp.QuadPart = XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC_AND_CHECKPOINT;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	if (!WriteFile(hFile, pbSignature, 
				XONLINECONTENT_PK_SIGNATURE_SIZE, &cbData, NULL) ||
		(cbData != XONLINECONTENT_PK_SIGNATURE_SIZE))
	{
		hr = GetLastError();
		printf("Cannot write header, %x.\n", hr);
		goto Error;
	}

Exit:
	return(hr);

Error:
	goto Exit;
}

#endif

//
// Function to verify encrypted data and compare it with original
//
BOOL VerifyEncryptedData(
			PBYTE				pbSymmetricKey,
			DWORD				cbSymmetricKey,
			PBYTE				pbEncrypted,
			PBYTE				pbReference,
			DWORD				cbData,
			PRC4_SHA_HEADER	pheader
			)
{
	if (!rc4HmacDecrypt(pbSymmetricKey, cbSymmetricKey,
				pbEncrypted, cbData, (PUCHAR)pheader))
		return(FALSE);

	if (memcmp(pbEncrypted, pbReference, cbData) != 0)
		return(FALSE);
	return(TRUE);
}

//
// Generic function to load a block from disk, encrypt, and write
// back to disk
//
HRESULT EncryptDiskBlock(
			HANDLE				hFile,
			HCRYPTPROV			hCrypto,
			DWORD				dwOffset,
			PBYTE				pbData,
			DWORD				cbData,
			PBYTE				pbSymmetricKey,
			DWORD				cbSymmetricKey,
			PBYTE				pbReference,
			PRC4_SHA_HEADER		pheader
			)
{
	HRESULT					hr = S_OK;
	BYTE					rgbConfounder[RC4_CONFOUNDER_LEN];
	RC4_SHA_HEADER			rc4sha1Header;
	LARGE_INTEGER			liTemp;
	DWORD					cbDone;

	// Now load the data
	liTemp.QuadPart = dwOffset;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	if (!ReadFile(hFile, pbData, cbData, &cbDone, NULL) ||
		(cbData != cbDone))
	{
		hr = GetLastError();
		printf("Cannot read data, %x.\n", hr);
		goto Error;
	}

	// Save a copy in the reference buffer
	if (pbReference)
		CopyMemory(pbReference, pbData, cbData);

	// Encrypt the data
	GenerateConfounder(hCrypto, rgbConfounder);
	rc4HmacEncrypt(
				rgbConfounder, 
				pbSymmetricKey,
				cbSymmetricKey,
				pbData, 
				cbData,
				(PBYTE)&rc4sha1Header);

	// Store the Hmac checksum in the header
	CopyMemory(pheader, &rc4sha1Header, sizeof(RC4_SHA_HEADER));

	// Write out the encrypted data
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	if (!WriteFile(hFile, pbData, cbData, &cbDone, NULL) ||
		(cbDone != cbData))
	{
		hr = GetLastError();
		printf("Cannot write data, %x.\n", hr);
		goto Error;
	}

	// Verify that the data decrypts back to the onriginal
	if (pbReference)
	{
		if (!VerifyEncryptedData(pbSymmetricKey, cbSymmetricKey,
					pbData, pbReference, cbData, &rc4sha1Header))
		{
			hr = ERROR_INVALID_DATA;
			printf("Decrypted and original don't match, %x.\n", hr);
			goto Error;
		}
	}

Exit:
	return(hr);

Error:
	goto Exit;
}			

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
			)
{
	HRESULT					hr;
	XONLINECONTENT_HEADER	header;
	CFFOLDER_HMAC			rgcffolderhmac[XONLINECONTENT_MAX_CFFOLDER];
	DWORD					cbSize[XONLINECONTENT_MAX_CFFOLDER];
	BYTE					rgbConfounder[RC4_CONFOUNDER_LEN];
	RC4_SHA_HEADER			rc4sha1Header;
	LARGE_INTEGER			liTemp;
	PBYTE					pbData = NULL;
	PBYTE					pbReference = NULL;
	DWORD					cbData;
	DWORD					cbMaxChunk;
	DWORD					cFolders;
	DWORD					i;

	// Read the CAB header
	liTemp.QuadPart = 0;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	if (!ReadFile(hFile, &header, sizeof(XONLINECONTENT_HEADER), &cbData, NULL))
	{
		hr = GetLastError();
		printf("Read header failed with %x\n", hr);
		goto Error;
	}

	if (cbData != sizeof(XONLINECONTENT_HEADER))
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		printf("Size mismatch.\n");
		goto Error;
	}

	if (!GetFileSizeEx(hFile, &liTemp))
	{
		hr = GetLastError();
		printf("GetFileSizeEx failed with %x\n", hr);
		goto Error;
	}

	// Validate the results
	hr = VerifyCabHeader(&header, &liTemp);
	if (hr != S_OK)
		goto Error;

	// Now read the CFFOLDER_HMAC entries
	liTemp.QuadPart = XONLINECONTENT_COMPLETE_HEADER_SIZE;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	if (!ReadFile(hFile, rgcffolderhmac, 
				(sizeof(CFFOLDER_HMAC) * header.cfheader.cFolders),
				&cbData, NULL))
	{
		hr = GetLastError();
		printf("Cannot read CFFOLDER entries %x.\n", hr);
		goto Error;
	}

	// Find the largest chunk we will have to encrypt
	cFolders = header.cfheader.cFolders;
	cbMaxChunk = rgcffolderhmac[0].cffolder.coffCabStart - 
					header.cfheader.coffFiles;
	if ((sizeof(CFFOLDER_HMAC) * cFolders) > cbMaxChunk)
		cbMaxChunk = sizeof(CFFOLDER_HMAC) * cFolders;

	for (i = 0; i < cFolders; i++)
	{
		if (i == (cFolders - 1))
			cbSize[i] = header.cfheader.cbCabinet;
		else
			cbSize[i] = rgcffolderhmac[i + 1].cffolder.coffCabStart;

		cbSize[i] -= rgcffolderhmac[i].cffolder.coffCabStart;

		if (cbSize[i] > cbMaxChunk)
			cbMaxChunk = cbSize[i];
	}

	// Allocate the buffer
	pbData = LocalAlloc(LMEM_FIXED, cbMaxChunk);
	if (!pbData)
	{
		hr = ERROR_NOT_ENOUGH_MEMORY;
		printf("Cannot allocate memory %x.\n", hr);
		goto Error;
	}

	pbReference = LocalAlloc(LMEM_FIXED, cbMaxChunk);
	if (!pbReference)
	{
		hr = ERROR_NOT_ENOUGH_MEMORY;
		printf("Cannot allocate memory %x.\n", hr);
		goto Error;
	}

	// Encrypt all the blocks
	for (i = 0; i < cFolders; i++)
	{
		hr = EncryptDiskBlock(hFile, 
					hCrypto,
					rgcffolderhmac[i].cffolder.coffCabStart,
					pbData,
					cbSize[i],
					pbSymmetricKey,
					cbSymmetricKey,
					pbReference,
					&(rgcffolderhmac[i].hmac));
		if (hr != S_OK)
		{
			printf("Cannot encrypt folder %u data %x.\n", i, hr);
			goto Error;
		}
	}

	cbSize[0] = rgcffolderhmac[0].cffolder.coffCabStart - 
					header.cfheader.coffFiles;
	hr = EncryptDiskBlock(hFile, 
				hCrypto,
				header.cfheader.coffFiles,
				pbData,
				cbSize[0],
				pbSymmetricKey,
				cbSymmetricKey,
				pbReference,
				&(header.digestFiles));
	if (hr != S_OK)
	{
		printf("Cannot encrypt CFFILE entries %x.\n", hr);
		goto Error;
	}

	// Save a copy in the reference buffer
	cbData = sizeof(CFFOLDER_HMAC) * cFolders;
	CopyMemory(pbReference, rgcffolderhmac, cbData);

	// OK, now we encrypt the CFFOLDER entries
	GenerateConfounder(hCrypto, rgbConfounder);
	rc4HmacEncrypt(
				rgbConfounder, 
				pbSymmetricKey,
				cbSymmetricKey,
				(PBYTE)rgcffolderhmac, 
				cbData,
				(PBYTE)&rc4sha1Header);

	// Store the Hmac checksum in the header
	CopyMemory(&(header.digestFolders), 
				&rc4sha1Header, sizeof(RC4_SHA_HEADER));
				
	// Write out the encrypted CFFOLDER entries
	liTemp.QuadPart = XONLINECONTENT_COMPLETE_HEADER_SIZE;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	cbSize[0] = cbData;
	if (!WriteFile(hFile, rgcffolderhmac, cbSize[0], &cbData, NULL) ||
		(cbSize[0] != cbData))
	{
		hr = GetLastError();
		printf("Cannot write CFFOLDER entries, %x.\n", hr);
		goto Error;
	}

	// Verify that the data decrypts back to the onriginal
	if (!VerifyEncryptedData(pbSymmetricKey, cbSymmetricKey,
				(PBYTE)rgcffolderhmac, pbReference, cbData, &rc4sha1Header))
	{
		hr = ERROR_INVALID_DATA;
		printf("Decrypted and original don't match, %x.\n", hr);
		goto Error;
	}

	// Write out the updated header with the new hashes
	liTemp.QuadPart = 0;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	if (!WriteFile(hFile, &header, 
				XONLINECONTENT_FIXED_HEADER_SIZE, &cbData, NULL) ||
		(cbData != XONLINECONTENT_FIXED_HEADER_SIZE))
	{
		hr = GetLastError();
		printf("Cannot write header, %x.\n", hr);
		goto Error;
	}

Exit:
	if (pbData)
		LocalFree(pbData);
	if (pbReference)
		LocalFree(pbReference);

	return(hr);

Error:
	goto Exit;
}

//
// Function to encrypt the header portion of the CAB file. This should
// be the last step following placing the digital signature in the slot
//
HRESULT EncryptCabinetHeader(
			HANDLE		hFile,
			HCRYPTPROV	hCrypto,
			PBYTE		pbSymmetricKey,
			DWORD		cbSymmetricKey
			)
{
	HRESULT					hr = S_OK;
	BYTE					rgbReference[XONLINECONTENT_FIXED_HEADER_SIZE];
	BYTE					rgbConfounder[RC4_CONFOUNDER_LEN];
	XONLINECONTENT_HEADER	header;
	LARGE_INTEGER			liTemp;
	RC4_SHA_HEADER			rc4sha1Header;
	DWORD					dwCheckpoint = 0;
	DWORD					cbData;

	// Read the CAB header
	hr = LoadCabinetHeader(hFile, &header);
	if (hr != S_OK)
	{
		printf("Cannot load cabinet header %x.\n", hr);
		goto Error;
	}

	// Save a copy in the reference buffer
	CopyMemory(rgbReference, &header, XONLINECONTENT_FIXED_HEADER_SIZE);

	// OK, now we encrypt the CFFOLDER entries
	GenerateConfounder(hCrypto, rgbConfounder);
	rc4HmacEncrypt(
				rgbConfounder, 
				pbSymmetricKey,
				cbSymmetricKey,
				(PBYTE)&header,
				XONLINECONTENT_FIXED_HEADER_SIZE,
				(PBYTE)&rc4sha1Header);

	// Write out the encrypted header
	liTemp.QuadPart = 0;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	if (!WriteFile(hFile, &header, 
				XONLINECONTENT_FIXED_HEADER_SIZE, &cbData, NULL) ||
		(cbData != XONLINECONTENT_FIXED_HEADER_SIZE))
	{
		hr = GetLastError();
		printf("Cannot write header, %x.\n", hr);
		goto Error;
	}

	// Write out the final HMAC
	if (!WriteFile(hFile, &rc4sha1Header, 
				sizeof(RC4_SHA_HEADER), &cbData, NULL) ||
		(cbData != sizeof(RC4_SHA_HEADER)))
	{
		hr = GetLastError();
		printf("Cannot write header HMAC, %x.\n", hr);
		goto Error;
	}

	// Generate some confounding bytes as the checkpoint
	GenerateRandomBytes(hCrypto, (PBYTE)&dwCheckpoint, sizeof(DWORD));

	// Write out the checkpoint
	if (!WriteFile(hFile, &dwCheckpoint, 
				sizeof(DWORD), &cbData, NULL) ||
		(cbData != sizeof(DWORD)))
	{
		hr = GetLastError();
		printf("Cannot write checkpoint, %x.\n", hr);
		goto Error;
	}

	// Verify that the data decrypts back to the onriginal
	if (!VerifyEncryptedData(pbSymmetricKey, cbSymmetricKey,
				(PBYTE)&header, rgbReference, 
				XONLINECONTENT_FIXED_HEADER_SIZE, &rc4sha1Header))
	{
		hr = ERROR_INVALID_DATA;
		printf("Decrypted and original don't match, %x.\n", hr);
		goto Error;
	}

Exit:
	return(hr);

Error:
	goto Exit;
}

//
// Generic function to load a block from disk, decrypt, and write
// back to disk
//
HRESULT DecryptDiskBlock(
			HANDLE				hFile,
			HCRYPTPROV			hCrypto,
			DWORD				dwOffset,
			PBYTE				pbData,
			DWORD				cbData,
			PBYTE				pbSymmetricKey,
			DWORD				cbSymmetricKey,
			PRC4_SHA_HEADER		pVerifyHeader
			)
{
	HRESULT					hr = S_OK;
	LARGE_INTEGER			liTemp;
	DWORD					cbDone;

	// Now load the data
	liTemp.QuadPart = dwOffset;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	if (!ReadFile(hFile, pbData, cbData, &cbData, NULL))
	{
		hr = GetLastError();
		printf("Cannot read data, %x.\n", hr);
		goto Error;
	}

	// Decrypt the data
	if (!rc4HmacDecrypt(
				pbSymmetricKey,
				cbSymmetricKey,
				pbData, 
				cbData,
				(PBYTE)pVerifyHeader))
	{
		hr = ERROR_INVALID_DATA;
		printf("Decrypted data does not match HMAC\n");
		goto Error;
	}

	// Write out the decrypted data
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	if (!WriteFile(hFile, pbData, cbData, &cbDone, NULL))
	{
		hr = GetLastError();
		printf("Cannot write data, %x.\n", hr);
		goto Error;
	}
	if (cbDone != cbData)
	{
		hr = ERROR_WRITE_FAULT;
		goto Error;
	}

Exit:
	return(hr);

Error:
	goto Exit;
}			
//
// Function to decrypt the header portion of the package file. The
// symmetric key provided should be the key retrieved from the signature
// slot of the same package. The decryption is done in-place.
//
HRESULT DecryptPackageHeader(
			HANDLE		hFile,
			HCRYPTPROV	hCrypto,
			PBYTE		pbSymmetricKey,
			DWORD		cbSymmetricKey,
			PXONLINECONTENT_HEADER	pHeader
			)
{
	HRESULT					hr = S_OK;
	BYTE					rgbHeader[XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC];
	PXONLINECONTENT_HEADER	pCabHeader;
	PRC4_SHA_HEADER			pRc4Header;
	LARGE_INTEGER			liTemp;
	DWORD					cbData;

	// Read the encrypted package header
	liTemp.QuadPart = 0;
	if (!SetFilePointerEx(hFile, liTemp, &liTemp, FILE_BEGIN))
	{
		hr = GetLastError();
		goto Error;
	}

	if (!ReadFile(hFile, rgbHeader, 
				XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC,
				&cbData, NULL))
	{
		hr = GetLastError();
		goto Error;
	}
	if (cbData != XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC)
	{
		hr = ERROR_READ_FAULT;
		goto Error;
	}

	// Overlay the data structures
	pCabHeader = (PXONLINECONTENT_HEADER)rgbHeader;
	pRc4Header = (PRC4_SHA_HEADER)(pCabHeader + 1);

	// Decrypt
	if (!rc4HmacDecrypt(
				pbSymmetricKey,
				cbSymmetricKey,
				rgbHeader,
				XONLINECONTENT_FIXED_HEADER_SIZE,
				(PBYTE)pRc4Header))
	{
		// Dont match, this is an error
		hr = ERROR_INVALID_DATA;
		goto Error;
	}

	// Write out the decrypted package header (and zero out the
	// HMAC slot)
	liTemp.QuadPart = 0;
	if (!SetFilePointerEx(hFile, liTemp, &liTemp, FILE_BEGIN))
	{
		hr = GetLastError();
		goto Error;
	}

	ZeroMemory(rgbHeader + XONLINECONTENT_FIXED_HEADER_SIZE, 
				sizeof(RC4_SHA_HEADER));
	if (!WriteFile(hFile, rgbHeader, 
				XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC,
				&cbData, NULL))
	{
		hr = GetLastError();
		goto Error;
	}
	if (cbData != XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC)
	{
		hr = ERROR_WRITE_FAULT;
		goto Error;
	}

	// That's it, copy the header out
	memcpy(pHeader, pCabHeader, sizeof(XONLINECONTENT_HEADER));

Exit:
	return(hr);

Error:
	goto Exit;
}

//
// Function to Decrypt a Xbox content package body using the specified
// symmetric key. Decryption is done in-place
//
HRESULT DecryptPackageBody(
			HANDLE					hFile,
			HCRYPTPROV				hCrypto,
			PXONLINECONTENT_HEADER	pHeader,
			PBYTE					pbSymmetricKey,
			DWORD					cbSymmetricKey
			)
{
	HRESULT					hr;
	CFFOLDER_HMAC			rgcffolderhmac[XONLINECONTENT_MAX_CFFOLDER];
	DWORD					cbSize[XONLINECONTENT_MAX_CFFOLDER];
	LARGE_INTEGER			liTemp;
	PBYTE					pbData = NULL;
	DWORD					cbData;
	DWORD					cbMaxChunk;
	DWORD					cFolders;
	DWORD					i;

	if (!GetFileSizeEx(hFile, &liTemp))
	{
		hr = GetLastError();
		printf("GetFileSizeEx failed with %x\n", hr);
		goto Error;
	}

	// Validate the results
	hr = VerifyCabHeader(pHeader, &liTemp);
	if (hr != S_OK)
		goto Error;

	// Now read the CFFOLDER_HMAC entries
	liTemp.QuadPart = XONLINECONTENT_COMPLETE_HEADER_SIZE;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}
	cbSize[0] = sizeof(CFFOLDER_HMAC) * pHeader->cfheader.cFolders;
	if (!ReadFile(hFile, rgcffolderhmac, cbSize[0], &cbData, NULL))
	{
		hr = GetLastError();
		printf("Cannot read CFFOLDER entries %x.\n", hr);
		goto Error;
	}
	if (cbSize[0] != cbData)
	{
		hr = ERROR_READ_FAULT;
		goto Error;
	}

	// OK, now we decrypt the CFFOLDER entries
	if (!rc4HmacDecrypt(
				pbSymmetricKey,
				cbSymmetricKey,
				(PBYTE)rgcffolderhmac, 
				cbData,
				(PBYTE)&pHeader->digestFolders))
	{
		hr = ERROR_INVALID_DATA;
		printf("CFFOLDER entries HMAC mismatch %x.\n", hr);
		goto Error;
	}

	// Verify that the folder entries make sense
	hr = VerifyFolderEntries(pHeader, rgcffolderhmac);
	if (hr != S_OK)
	{
		printf("Invalid CFFOLDER entries.\n");
		goto Error;
	}
				
	// Write out the decrypted CFFOLDER entries
	liTemp.QuadPart = XONLINECONTENT_COMPLETE_HEADER_SIZE;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		printf("Cannot set file pointer %x.\n", hr);
		goto Error;
	}

	i = cbData;
	if (!WriteFile(hFile, rgcffolderhmac, i, &cbData, NULL))
	{
		hr = GetLastError();
		printf("Cannot write CFFOLDER entries, %x.\n", hr);
		goto Error;
	}
	if (i != cbData)
	{
		hr = ERROR_WRITE_FAULT;
		goto Error;
	}

	// Find the largest chunk we will have to decrypt
	cFolders = pHeader->cfheader.cFolders;
	cbMaxChunk = rgcffolderhmac[0].cffolder.coffCabStart - 
					pHeader->cfheader.coffFiles;

	for (i = 0; i < cFolders; i++)
	{
		if (i == (cFolders - 1))
			cbSize[i] = pHeader->cfheader.cbCabinet;
		else
			cbSize[i] = rgcffolderhmac[i + 1].cffolder.coffCabStart;

		cbSize[i] -= rgcffolderhmac[i].cffolder.coffCabStart;

		if (cbSize[i] > cbMaxChunk)
			cbMaxChunk = cbSize[i];
	}

	// Allocate the buffer
	pbData = LocalAlloc(LMEM_FIXED, cbMaxChunk);
	if (!pbData)
	{
		hr = ERROR_NOT_ENOUGH_MEMORY;
		printf("Cannot allocate memory %x.\n", hr);
		goto Error;
	}

	// Now decrypt the CFFILE entries
	i = rgcffolderhmac[0].cffolder.coffCabStart - 
					pHeader->cfheader.coffFiles;
	hr = DecryptDiskBlock(hFile, 
				hCrypto,
				pHeader->cfheader.coffFiles,
				pbData,
				i,
				pbSymmetricKey,
				cbSymmetricKey,
				&(pHeader->digestFiles));
	if (hr != S_OK)
	{
		printf("Cannot decrypt CFFILE entries %x.\n", hr);
		goto Error;
	}

	// Encrypt all the blocks
	for (i = 0; i < cFolders; i++)
	{
		hr = DecryptDiskBlock(hFile, 
					hCrypto,
					rgcffolderhmac[i].cffolder.coffCabStart,
					pbData,
					cbSize[i],
					pbSymmetricKey,
					cbSymmetricKey,
					&(rgcffolderhmac[i].hmac));
		if (hr != S_OK)
		{
			printf("Cannot decrypt folder %u data %x.\n", i, hr);
			goto Error;
		}
	}

Exit:
	if (pbData)
		LocalFree(pbData);

	return(hr);

Error:
	goto Exit;
}

//
// Function to encrypt a symmetric key with a supplied public key and place
// the encrypted contents into the signature slot of the package
//
HRESULT EncloseSymmetricKey(
			HANDLE		hFile,
			HCRYPTKEY	hKeyPublic,
			BYTE		*pbSymmetricKey,
			DWORD		cbSymmetricKey
			)
{
	HRESULT			hr = S_OK;
	BYTE			rgbSignature[XONLINECONTENT_PK_SIGNATURE_SIZE];
	DWORD			cbData;
	LARGE_INTEGER	liTemp;

	// Encrypt key
	cbData = cbSymmetricKey;
	memcpy(rgbSignature, pbSymmetricKey, cbData);
	if (!CryptEncrypt(hKeyPublic, 0, TRUE, 0, 
				rgbSignature, &cbData, XONLINECONTENT_PK_SIGNATURE_SIZE))
	{
		hr = GetLastError();
		goto Error;
	}

	// Write out the encrypted key
	liTemp.QuadPart = XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC_AND_CHECKPOINT;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		goto Error;
	}

	if (!WriteFile(hFile, rgbSignature,
				XONLINECONTENT_PK_SIGNATURE_SIZE, &cbData, NULL) ||
		(cbData != XONLINECONTENT_PK_SIGNATURE_SIZE))
	{
		hr = GetLastError();
		goto Error;
	}

Exit:
	return(hr);

Error:
	goto Exit;
}

//
// Function to retrieve a symmetric key previously encrypted with a
// public key, whose corresponding private key is supplied.
//
HRESULT RetrieveSymmetricKey(
			HANDLE		hFile,
			HCRYPTKEY	hKeyPrivate,
			BYTE		*pbSymmetricKey,
			DWORD		cbSymmetricKey
			)
{
	HRESULT			hr = S_OK;
	BYTE			rgbSignature[XONLINECONTENT_PK_SIGNATURE_SIZE];
	DWORD			cbData;
	LARGE_INTEGER	liTemp;

	// Read the encrypted key
	liTemp.QuadPart = XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC_AND_CHECKPOINT;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		goto Error;
	}

	if (!ReadFile(hFile, rgbSignature,
				XONLINECONTENT_PK_SIGNATURE_SIZE, &cbData, NULL))
	{
		hr = GetLastError();
		goto Error;
	}
	if (cbData != XONLINECONTENT_PK_SIGNATURE_SIZE)
	{
		hr = ERROR_READ_FAULT;
		goto Error;
	}

	// Decrypt key
	cbData = XONLINECONTENT_PK_SIGNATURE_SIZE;
	if (!CryptDecrypt(hKeyPrivate, 0, TRUE, 0, 
				rgbSignature, &cbData))
	{
		hr = GetLastError();
		goto Error;
	}

	// Make sure size is correct
	if (cbData != cbSymmetricKey)
	{
		hr = ERROR_INVALID_DATA;
		goto Error;
	}

	// Copy the data over
	memcpy(pbSymmetricKey, rgbSignature, cbSymmetricKey);

	// Zero out the signature slot for good measure
	liTemp.QuadPart = XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC_AND_CHECKPOINT;
	if (!SetFilePointerEx(hFile, liTemp, NULL, FILE_BEGIN))
	{
		hr = GetLastError();
		goto Error;
	}

	ZeroMemory(rgbSignature, XONLINECONTENT_PK_SIGNATURE_SIZE);
	if (!WriteFile(hFile, rgbSignature,
				XONLINECONTENT_PK_SIGNATURE_SIZE, &cbData, NULL))
	{
		hr = GetLastError();
		goto Error;
	}
	if (cbData != XONLINECONTENT_PK_SIGNATURE_SIZE)
	{
		hr = ERROR_WRITE_FAULT;
		goto Error;
	}

Exit:
	return(hr);

Error:
	goto Exit;
}

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
			)
{
	HRESULT			hr = S_OK;
	LARGE_INTEGER	liSize;
	LARGE_INTEGER	liOffset;
	HANDLE			hAppend = INVALID_HANDLE_VALUE;
	PBYTE			pbBuffer = NULL;
	DWORD			cbSize;
	BYTE			rgbConfounder[RC4_CONFOUNDER_LEN];
	RC4_SHA_HEADER	Header;

	// Seek to the end of file
	if (!GetFileSizeEx(hFile, &liOffset))
		goto Error;

	// Save some space for the encrypted header if the symmetric key
	// is provided
	if (pbSymmetricKey)
	{
		liOffset.QuadPart += sizeof(RC4_SHA_HEADER);
	}
	if (!SetFilePointerEx(hFile, liOffset, &liOffset, FILE_BEGIN))
		goto Error;

	// Open the file to append
	hAppend = CreateFile(
				szFileToAppend,
				GENERIC_READ, 
				FILE_SHARE_READ, 
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
	if (hAppend == INVALID_HANDLE_VALUE)
		goto Error;

	if (!GetFileSizeEx(hAppend, &liSize))
		goto Error;

	// Allocate a buffer large enough for the entire file
	pbBuffer = (PBYTE)LocalAlloc(0, liSize.u.LowPart);
	if (!pbBuffer)
	{
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		goto Error;
	}

	// Do a copy
	if (!ReadFile(hAppend, pbBuffer, liSize.u.LowPart, &cbSize, NULL))
		goto Error;

	// Let's see if we need to encrypt the file as well ...
	if (pbSymmetricKey)
	{
		// Encrypt the CFFILE entries
		GenerateConfounder(hCrypto, rgbConfounder);
		rc4HmacEncrypt(
					rgbConfounder, 
					pbSymmetricKey,
					cbSymmetricKey,
					pbBuffer, 
					cbSize,
					(PBYTE)&Header);
	}
	
	if (cbSize != liSize.u.LowPart)
	{
		SetLastError(ERROR_INVALID_DATA);
		goto Error;
	}
		
	if (!WriteFile(hFile, pbBuffer, cbSize, &cbSize, NULL))
		goto Error;

	// Write out the encryption header if necessary
	if (pbSymmetricKey)
	{
		liOffset.QuadPart -= sizeof(RC4_SHA_HEADER);
		if (!SetFilePointerEx(hFile, liOffset, &liOffset, FILE_BEGIN))
			goto Error;

		if (!WriteFile(hFile, &Header, sizeof(RC4_SHA_HEADER), &cbSize, NULL))
			goto Error;

		if (cbSize != sizeof(RC4_SHA_HEADER))
		{
			SetLastError(ERROR_INVALID_DATA);
			goto Error;
		}
	}

Exit:
	if (pbBuffer)
		LocalFree(pbBuffer);
	if (hAppend != INVALID_HANDLE_VALUE)
		CloseHandle(hAppend);
	return(hr);

Error:
	hr = GetLastError();
	goto Exit;
}

//
// Function to detach a file form the end of a package that was previously appended
// using AppendEncryptedFile. The file will be saved in the specified file path,
// and its contents can be optionally decrypted if a symmetric key is provided.
//
// The package header is required to perform this operation.
//
// On success, the caller may optionally obtain a handle to the detached file, if
// this is the case, the caller is responsible for closing the obtained file handle
// when done.
//
HRESULT DetachEncryptedFile(
			HANDLE					hFile,
			CHAR					*szDestinationFile,
			PXONLINECONTENT_HEADER	pHeader,
			HCRYPTPROV				hCrypto,
			PBYTE					pbSymmetricKey,
			DWORD					cbSymmetricKey,
			HANDLE					*phDetachedFile
			)
{
	HRESULT			hr = S_OK;
	LARGE_INTEGER	liSize;
	LARGE_INTEGER	liOffset;
	HANDLE			hDetach = INVALID_HANDLE_VALUE;
	PBYTE			pbBuffer = NULL;
	DWORD			cbSize;
	DWORD			cbData;
	RC4_SHA_HEADER	Header;

	// Figure out the incision point and the size of the file
	if (!GetFileSizeEx(hFile, &liSize))
		goto Error;

	cbSize = pHeader->cfheader.cbCabinet;
	liOffset.QuadPart = cbSize;
	
	if (liSize.QuadPart <= liOffset.QuadPart)
	{
		SetLastError(ERROR_INVALID_DATA);
		goto Error;
	}
	cbSize = (DWORD)(liSize.QuadPart - liOffset.QuadPart);
	if (cbSize <= sizeof(Header))
	{
		SetLastError(ERROR_INVALID_DATA);
		goto Error;
	}

	// Set the file pointer to the starting position
	if (!SetFilePointerEx(hFile, liOffset, NULL, FILE_BEGIN))
		goto Error;

	// Read in the encrypted header if decryption is needed
	if (pbSymmetricKey)
	{
		if (!ReadFile(hFile, &Header, sizeof(Header), &cbData, NULL))
			goto Error;
		if (cbData != sizeof(Header))
		{
			SetLastError(ERROR_READ_FAULT);
			goto Error;
		}

		// Decrement the file size
		cbSize -= cbData;
	}

	// Open the file to detach into
	hDetach = CreateFile(
				szDestinationFile,
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL, 
				CREATE_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
	if (hDetach == INVALID_HANDLE_VALUE)
		goto Error;

	// Allocate a buffer large enough for the entire file
	pbBuffer = (PBYTE)LocalAlloc(0, cbSize);
	if (!pbBuffer)
	{
		SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		goto Error;
	}

	// Do a copy
	if (!ReadFile(hFile, pbBuffer, cbSize, &cbData, NULL))
		goto Error;
	if (cbData != cbSize)
	{
		SetLastError(ERROR_READ_FAULT);
		goto Error;
	}

	// Let's see if we need to decrypt the file as well ...
	if (pbSymmetricKey)
	{
		if (!rc4HmacDecrypt(
					pbSymmetricKey,
					cbSymmetricKey,
					pbBuffer, 
					cbSize,
					(PBYTE)&Header))
		{
			SetLastError(ERROR_INVALID_DATA);
			goto Error;
		}
	}

	// Write it out
	if (!WriteFile(hDetach, pbBuffer, cbSize, &cbData, NULL))
		goto Error;
	if (cbSize != cbData)
	{
		SetLastError(ERROR_WRITE_FAULT);
		goto Error;
	}

	// Chop off the original file
	if (!SetFilePointerEx(hFile, liOffset, NULL, FILE_BEGIN))
		goto Error;
	if (!SetEndOfFile(hFile))
		goto Error;

	// Return the handle if requested
	if (phDetachedFile)
	{
		*phDetachedFile = hDetach;
		hDetach = INVALID_HANDLE_VALUE;
	}

Exit:
	if (pbBuffer)
		LocalFree(pbBuffer);
	if (hDetach != INVALID_HANDLE_VALUE)
		CloseHandle(hDetach);
	return(hr);

Error:
	hr = GetLastError();
	goto Exit;
}

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
			)
{
	HRESULT			hr = S_OK;
	HANDLE			hFile;
	char			rgbBuffer[450];
	DWORD			cbBuffer;
	DWORD			cbDone;

	hFile = CreateFile(
				szPath?szPath:XONLINECONTENT_MANIFEST_FILE_NAME, 
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL, 
				CREATE_NEW, 
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = GetLastError();
		goto Exit;
	}

	// 
	// BUGBUG: Generate random junk for the manifest file for now
	// Fill in the rigt data once we define it
	//
	cbBuffer = sizeof(rgbBuffer);
	GenerateRandomBytes(hCrypto, 
				rgbBuffer + A_SHA_DIGEST_LEN, cbBuffer - A_SHA_DIGEST_LEN);

	// Do an HMAC using the supplied key over the random data, place the
	// HMAC signature at the beginning of the file
    shaHmac( pbKey, XONLINE_KEY_LENGTH,
             rgbBuffer + A_SHA_DIGEST_LEN, cbBuffer - A_SHA_DIGEST_LEN,
             NULL, 0,
             rgbBuffer );

	if (!WriteFile(hFile, rgbBuffer, cbBuffer, &cbDone, NULL) ||
		(cbBuffer != cbDone))
	{
		hr = GetLastError();
		goto Exit;
	}
					
Exit:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return(hr);
}

//
// Function to convert a hex string to a ULONGLONG
//
HRESULT HexStringToUlonglong(
			PSTR		szHexValue,
			DWORD		cbHexValue,
			ULONGLONG	*pullValue
			)
{
	ULONGLONG	ull = 0;
	DWORD 		i, c, len;

	len = cbHexValue?cbHexValue:strlen(szHexValue);
	if (len <= 16)
	{
		for (i = 0; i < len; i++)
		{
			c = szHexValue[(len - 1) - i];
			if ((c >= '0') && (c <= '9'))
				c = c - '0';
			else if ((c >= 'a') && ( c <= 'f'))
				c = (c - 'a') + 10;
			else if ((c >= 'A') && ( c <= 'F'))
				c = (c - 'A') + 10;
			else
				goto Error;
				
			ull |= (c << (i * 4));
		}

		*pullValue = ull;
		return(S_OK);
	}

Error:	
	return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
}

//
// Function to determine if a directory name is a valid title ID.
// If so, then the directory name will be converted to the Title ID 
// value. Otherwise, this function will return ERROR_INVALID_DATA.
//
HRESULT ConvertToTitleId(
			PSTR	szTitleId,
			DWORD	*pdwTitleId
			)
{
	HRESULT		hr;
	ULONGLONG	ull;
	hr = HexStringToUlonglong(szTitleId, 8, &ull);
	*pdwTitleId = (DWORD)ull;
	return(hr);
}

//
// Helper function to convert a symmetric key to a hex string
// representation
//
HRESULT ConvertSymmetricKeyToString(
			PBYTE		pbSymmetricKey,
			DWORD		cbSymmetricKey,
			CHAR		*szSymmetricKey,
			DWORD		*pcchSymmetricKey
			)
{
	if (cbSymmetricKey != XONLINE_KEY_LENGTH)
		return(ERROR_INVALID_DATA);
	if (((cbSymmetricKey * 2) + 3) > *pcchSymmetricKey)
	{
		*pcchSymmetricKey = (cbSymmetricKey * 2) + 3;
		return(ERROR_INSUFFICIENT_BUFFER);
	}

	*pcchSymmetricKey = sprintf(szSymmetricKey, 
				"0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
				pbSymmetricKey[0], pbSymmetricKey[1], pbSymmetricKey[2], pbSymmetricKey[3], 
				pbSymmetricKey[4], pbSymmetricKey[5], pbSymmetricKey[6], pbSymmetricKey[7], 
				pbSymmetricKey[8], pbSymmetricKey[9], pbSymmetricKey[10], pbSymmetricKey[11], 
				pbSymmetricKey[12], pbSymmetricKey[13], pbSymmetricKey[14], pbSymmetricKey[15]);
	return(S_OK);				
}

static BOOL xtob(
			CHAR	*szX,
			BYTE	*pb
			)
{
	DWORD	i;

	*pb = 0;
	for (i = 0; i < 2; i++)
	{
		if ((*szX >= '0') && (*szX <= '9'))
			*pb += (*szX - '0');
		else if ((*szX >= 'A') && (*szX <= 'F'))
			*pb += (*szX + 10 - 'A');
		else if ((*szX >= 'a') && (*szX <= 'f'))
			*pb += (*szX + 10 - 'a');
		else 
			return(FALSE);

		szX++;
		if (!i)
			(*pb) <<= 4;
	}

	return(TRUE);
}

//
// Helper function to convert a symmetric key from a hex string
// to its binary representation
//
HRESULT ConvertSymmetricKeyToBinary(
			CHAR		*szSymmetricKey,
			DWORD		cchSymmetricKey,
			PBYTE		pbSymmetricKey,
			DWORD		*pcbSymmetricKey
			)
{
	DWORD	i;

	if (cchSymmetricKey != (XONLINE_KEY_LENGTH * 2))
		return(ERROR_INVALID_DATA);
	if ((cchSymmetricKey / 2) > *pcbSymmetricKey)
	{
		*pcbSymmetricKey = cchSymmetricKey / 2;
		return(ERROR_INSUFFICIENT_BUFFER);
	}

	for (i = 0; i < XONLINE_KEY_LENGTH; i++)
	{
		if (!xtob(szSymmetricKey + (i*2), pbSymmetricKey + i))
			return(ERROR_INVALID_DATA);
	}

	*pcbSymmetricKey = i;
	return(S_OK);				
}

//
// Function to create a symmetric key and save it to a named file.
// If this file exists, this function will fail.
//
HRESULT ExportSymmetricKey(
			HCRYPTPROV	hCrypto,
			CHAR		*szFileName,
			PBYTE		pbSymmetricKey,
			DWORD		cbSymmetricKey
			)
{
	HRESULT			hr = S_OK;
	HANDLE			hFile;
	char			rgbBuffer[4096];
	DWORD			cbBuffer;
	DWORD			cbDone;

	if (cbSymmetricKey != XONLINE_KEY_LENGTH)
		return(ERROR_INVALID_DATA);

	hFile = CreateFile(
				szFileName, 
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL, 
				CREATE_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = GetLastError();
		goto Exit;
	}

	cbBuffer = sprintf(rgbBuffer, 
				"\n"\
				"// 0x%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n"\
				"\n"\
				"const unsigned char rgbSymmetricKey[] = {\n"\
				"\t0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X,\n"\
				"\t0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\n"\
				"};\n"\
				"\n",
				pbSymmetricKey[0], pbSymmetricKey[1], pbSymmetricKey[2], pbSymmetricKey[3], 
				pbSymmetricKey[4], pbSymmetricKey[5], pbSymmetricKey[6], pbSymmetricKey[7], 
				pbSymmetricKey[8], pbSymmetricKey[9], pbSymmetricKey[10], pbSymmetricKey[11], 
				pbSymmetricKey[12], pbSymmetricKey[13], pbSymmetricKey[14], pbSymmetricKey[15],
				pbSymmetricKey[0], pbSymmetricKey[1], pbSymmetricKey[2], pbSymmetricKey[3], 
				pbSymmetricKey[4], pbSymmetricKey[5], pbSymmetricKey[6], pbSymmetricKey[7], 
				pbSymmetricKey[8], pbSymmetricKey[9], pbSymmetricKey[10], pbSymmetricKey[11], 
				pbSymmetricKey[12], pbSymmetricKey[13], pbSymmetricKey[14], pbSymmetricKey[15]);

	if (!WriteFile(hFile, rgbBuffer, cbBuffer, &cbDone, NULL) ||
		(cbBuffer != cbDone))
	{
		hr = GetLastError();
		goto Exit;
	}
					
Exit:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return(hr);
}

//
// Function to import a symmetric key from the named file. 
// If this file does not exist or is malformed, this function 
// fails
//
HRESULT ImportSymmetricKey(
			CHAR	*szFileName,
			PBYTE	pbSymmetricKey,
			DWORD	cbSymmetricKey
			)
{
	HRESULT			hr = S_OK;
	HANDLE			hFile;
	char			rgbBuffer[1024];
	DWORD			cbDone;
	DWORD			cItems;

	hFile = CreateFile(
				szFileName, 
				GENERIC_READ, 
				FILE_SHARE_READ, 
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = GetLastError();
		goto Exit;
	}

	if (!ReadFile(hFile, rgbBuffer, sizeof(rgbBuffer), &cbDone, NULL))
	{
		hr = GetLastError();
		goto Exit;
	}

	// File should be small!
	if (cbDone >= sizeof(rgbBuffer))
	{
		hr = ERROR_INVALID_DATA;
		goto Exit;
	}

	rgbBuffer[cbDone] = '\0';

	cItems = sscanf(rgbBuffer, 	
				"\n"\
				"const unsigned char rgbSymmetricKey[] = {\n"\
				"\t%i, %i, %i, %i, %i, %i, %i, %i,\n"\
				"\t%i, %i, %i, %i, %i, %i, %i, %i\n"\
				"};\n",
				&pbSymmetricKey[0], &pbSymmetricKey[1], 
				&pbSymmetricKey[2], &pbSymmetricKey[3], 
				&pbSymmetricKey[4], &pbSymmetricKey[5], 
				&pbSymmetricKey[6], &pbSymmetricKey[7], 
				&pbSymmetricKey[8], &pbSymmetricKey[9], 
				&pbSymmetricKey[10], &pbSymmetricKey[11], 
				&pbSymmetricKey[12], &pbSymmetricKey[13], 
				&pbSymmetricKey[14], &pbSymmetricKey[15]);
	if (cItems != cbSymmetricKey)
	{
		hr = ERROR_INVALID_DATA;
		goto Exit;
	}

Exit:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return(hr);
}

//
// Function to read key information from a binary file
//
HRESULT ReadPrivatePublicKeyFile(
			PSTR	szFilename,
			PBYTE	*ppbKey,
			DWORD	*pcbKey
			)
{
	HRESULT			hr = S_OK;
	HANDLE			hFile;
	char			rgbBuffer[1024];
	DWORD			cbDone;
	DWORD			cItems;
	LARGE_INTEGER	liFileSize;
	PBYTE			pbKey = NULL;

	hFile = CreateFile(
				szFilename, 
				GENERIC_READ, 
				FILE_SHARE_READ, 
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = GetLastError();
		goto Exit;
	}

	if (!GetFileSizeEx(hFile, &liFileSize))
	{
		hr = GetLastError();
		goto Exit;
	}

	pbKey = LocalAlloc(LMEM_FIXED, liFileSize.u.LowPart);
	if (!pbKey)
	{
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	if (!ReadFile(hFile, pbKey, liFileSize.u.LowPart, &cbDone, NULL))
	{
		hr = GetLastError();
		goto Exit;
	}

	if (liFileSize.u.LowPart != cbDone)
	{
		hr = ERROR_INVALID_DATA;
		goto Exit;
	}

	*pcbKey = cbDone;
	*ppbKey = pbKey;

Exit:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	if (hr != S_OK && pbKey)
		LocalFree(pbKey);

	return(hr);
}

//
// Munge a public key exported by Crypto into a format that
// can directly be used by XcVerifyPKCS1Signature (i.e. Xbox 
// client). The exported blob must not have a password.
//
// Note: the caller must LocalFree the munged key when done.
//
typedef struct
{
    BSAFE_PUB_KEY   pkheader;
    BYTE            rgbKey[1];

} PUBLIC_KEY_OVERLAY, *PPUBLIC_KEY_OVERLAY;

HRESULT MungePublicKey(
            PBYTE pbKey, 
            DWORD cbKey,
            PBYTE *ppbMungedKey, 
            DWORD *pcbMungedKey
            )
{
	PPUBLIC_KEY_OVERLAY ppkoOld, ppkoNew;
	DWORD               cbOriginal;
	DWORD			    cbTmpLen;

    *ppbMungedKey = NULL;
    *pcbMungedKey = 0;

    // Overlay the struct over theunmunged key
    ppkoOld = (PPUBLIC_KEY_OVERLAY)pbKey;

	// Fix up the munged data exported by the Crypto API
	ppkoOld->pkheader.magic = ppkoOld->pkheader.bitlen;
	ppkoOld->pkheader.bitlen = ppkoOld->pkheader.datalen;
	ppkoOld->pkheader.datalen = ((ppkoOld->pkheader.bitlen + 7) >> 3) - 1;
    
    // Make sure the size is correct
    cbOriginal = ppkoOld->pkheader.datalen;
    if ((cbOriginal + sizeof(BSAFE_PUB_KEY)) != cbKey)
        return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));

    // Calculate the new size
    cbTmpLen = (sizeof(DWORD) * 2) -
               (((ppkoOld->pkheader.bitlen + 7) / 8) % (sizeof(DWORD) * 2));
    if ((sizeof(DWORD) * 2) != cbTmpLen)
        cbTmpLen += sizeof(DWORD) * 2;
	
	ppkoOld->pkheader.keylen = 
	            (ppkoOld->pkheader.bitlen >> 3) + cbTmpLen;

	ppkoNew = (PPUBLIC_KEY_OVERLAY)LocalAlloc(0, 
	            ppkoOld->pkheader.keylen + sizeof(BSAFE_PUB_KEY));
    if (!ppkoNew)
        return(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));

    // Copy the header over
    CopyMemory(&(ppkoNew->pkheader), &(ppkoOld->pkheader), sizeof(BSAFE_PUB_KEY));

    // Copy the rest
    CopyMemory(ppkoNew->rgbKey, ppkoOld->rgbKey, cbOriginal);

    // Pad any required trailing zeroes
    ZeroMemory(ppkoNew->rgbKey + cbOriginal, cbTmpLen);

    *ppbMungedKey = (PBYTE)ppkoNew;
    *pcbMungedKey = ppkoOld->pkheader.keylen + sizeof(BSAFE_PUB_KEY);

    return(S_OK);
}

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
#define REBUILD_REBUILD_EXTENSION	".new"
#define REBUILD_METADATA_EXTENSION	".xbx"
#define REBUILD_SQL_QUERY_EXTENSION	".sql"
#define REBUILD_CABINET_EXTENSION	".cab"
#define REBUILD_SYMMKEY_FILENAME	"symmkey.c"
#define REBUILD_TEMPFILE_NAME		"tempfile.$$$"

#define REBUILD_CONTENT_SECTION		"Metadata"
#define REBUILD_AUTO_SECTION		"All"
#define REBUILD_RATING_KEY			"Rating"
#define REBUILD_PACKAGE_SIZE_KEY	"PackageSize"
#define REBUILD_INSTALL_SIZE_KEY	"InstallSize"
#define REBUILD_SYMMETRIC_KEY		"SymmetricKey"
#define REBUILD_INVALID_VALUE		(0x7fffffff)

HRESULT RebuildContentPackage(
			CHAR		*szFilePath,
			DWORD		OfferingId,
			DWORD		dwTitleId,
			WORD		wVerHi,
			WORD		wVerLo,
			PBYTE		pbPrivateKey,
			DWORD		cbPrivateKey
			)
{
	HRESULT		hr = S_OK;
	HANDLE		hFile = INVALID_HANDLE_VALUE;
	HCRYPTPROV	hCrypto = 0;
	HCRYPTKEY	hKeyPrivate = 0;
	BYTE		rgbSymKey[XONLINE_KEY_LENGTH];
	BYTE		rgbBuffer[512];
	DWORD		cbSymKey;
	CHAR		szTempName[MAX_PATH];
	CHAR		szTempName2[MAX_PATH];
	DWORD		cbFilePath;
	DWORD		dwRating = 0;
	DWORD		cbDone;
	LARGE_INTEGER	liPackageSize;
	LARGE_INTEGER	liInstallSize;

	XONLINECONTENT_HEADER	Header;
	DGSTFILE_IDENTIFIER		dfi;

	// Initialize the crypto provider
	hr = InitializeCrypto(
				pbPrivateKey, cbPrivateKey,
				NULL, 0,
				&hCrypto, NULL, &hKeyPrivate, 0);
	if (hr != S_OK)
		goto Error;

	// Make a copy of the input file
	cbFilePath = strlen(szFilePath);
	if (cbFilePath + 5 >= MAX_PATH)
		return(ERROR_INVALID_NAME);

	strcpy(szTempName, szFilePath);
	strcat(szTempName, REBUILD_CABINET_EXTENSION);

	if (!CopyFile(szFilePath, szTempName, FALSE))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Cannot make copy of original package (%u)\n", hr);
		goto Error;
	}

	// Now we can party on the new file
	hFile = CreateFile(
				szTempName,
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Cannot open package %s (%u)\n", szTempName, hr);
		goto Error;
	}

	printf("Extracting ...\n");

	// First extract the symmetric key
	cbSymKey = XONLINE_KEY_LENGTH;
	hr = RetrieveSymmetricKey(hFile, hKeyPrivate, rgbSymKey, cbSymKey);
	if (hr != S_OK)
		goto Error;

	// Load the package header and decrypt it
	hr = DecryptPackageHeader(hFile, hCrypto, rgbSymKey, cbSymKey, &Header);
	if (hr != S_OK)
		goto Error;

	// Then detach the metatdata file and save it off
	strcpy(szTempName, szFilePath);
	strcat(szTempName, REBUILD_METADATA_EXTENSION);
	hr = DetachEncryptedFile(hFile, szTempName, 
				&Header, hCrypto, rgbSymKey, cbSymKey, NULL);
	if (hr != S_OK)
		goto Error;

#if 0
	// Now load the rating value from the metadata file
	dwRating = GetPrivateProfileInt(
				REBUILD_CONTENT_SECTION,
				REBUILD_RATING_KEY,
				REBUILD_INVALID_VALUE,
				szTempName);
	if (dwRating == REBUILD_INVALID_VALUE)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		printf("Cannot rebuild package because the rating of this package is not specified\n");
		goto Error;
	}
	if (dwRating > 0xffff)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		printf("Invalid content rating encountered (%u, %x)\n", dwRating, dwRating);
		goto Error;
	}

	printf(" * Content Rating is set to %u (%x)\n", dwRating, dwRating);
#endif	

	// Decrypt the rest of the package
	hr = DecryptPackageBody(hFile, hCrypto, &Header, rgbSymKey, cbSymKey);
	if (hr != S_OK)
		goto Error;

	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;

	// New we generate a new symmetric key
	GenerateSymmetricKey(hCrypto, rgbSymKey);

	printf("Rebuilding ...\n");

	// Rebild the content package with a digest signed with the new key
	dfi.OfferingId = OfferingId;
	dfi.dwTitleId = dwTitleId;
	dfi.wTitleVersionHi = wVerHi;
	dfi.wTitleVersionLo = wVerLo;
	strcpy(szTempName, szFilePath);
	strcat(szTempName, REBUILD_CABINET_EXTENSION);
	strcpy(szTempName2, szFilePath);
	strcat(szTempName2, REBUILD_REBUILD_EXTENSION);
	hr = RebuildCabinetWithDigest(szTempName, szTempName2,
				Header.cfheader.cFiles, REBUILD_TEMPFILE_NAME, 
				XONLINECONTENT_MANIFEST_FILE_NAME,
				&dfi, rgbSymKey, cbSymKey,
				&liPackageSize, &liInstallSize);
	if (FAILED(hr))
	{
		printf("Failed to rebuild cabinet file %s (%u)\n", szTempName2, hr);
		goto Error;
	}

	// Now save a copy of the output cabinet
	DeleteFile(szTempName);
	if (!CopyFile(szTempName2, szTempName, TRUE))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Cannot make copy of output cabinet (%u)\n", hr);
		goto Error;
	}
	
	hFile = CreateFile(
				szTempName2,
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Cannot open package %s (%u)\n", szTempName2, hr);
		goto Error;
	}

	printf("Encrypting ...\n");
	
	// Now we encrypt the package with the new symmetric key
	hr = EncryptCabinetBody(hFile, hCrypto, rgbSymKey, cbSymKey);
	if (hr != S_OK)
	{
		printf("Unable to re-encrypt package body (%x)\n", hr);
		goto Error;
	}

	hr = EncryptCabinetHeader(hFile, hCrypto, rgbSymKey, cbSymKey);
	if (hr != S_OK)
	{
		printf("Unable to re-encrypt package header (%x)\n", hr);
		goto Error;
	}

	// Finally export the symmetric key to a file
	hr = ExportSymmetricKey(hCrypto, 
				REBUILD_SYMMKEY_FILENAME, rgbSymKey, cbSymKey);
	if (hr != S_OK)
	{
		printf("Unable to export symmetric key (%x)\n", hr);
		goto Error;
	}

	printf("Exporting computed metadata ...\n");

	// Now append some computed values to the metadata file
	strcpy(szTempName, szFilePath);
	strcat(szTempName, REBUILD_METADATA_EXTENSION);

	// Export symmetric Key
	cbDone = (XONLINE_KEY_LENGTH * 2) + 3;
	hr = ConvertSymmetricKeyToString(
				rgbSymKey, cbSymKey, rgbBuffer, &cbDone);
	if (SUCCEEDED(hr))
	{
		if (!WritePrivateProfileString(
					REBUILD_AUTO_SECTION,
					REBUILD_SYMMETRIC_KEY,
					rgbBuffer, szTempName))
			hr = HRESULT_FROM_WIN32(GetLastError());
	}
	if (FAILED(hr))				
	{
		printf("Unable to export symmetric key (%x)\n", hr);
		goto Error;
	}

	// Export package size
	sprintf(rgbBuffer, "%I64u", liPackageSize.QuadPart);
	if (!WritePrivateProfileString(
					REBUILD_AUTO_SECTION,
					REBUILD_PACKAGE_SIZE_KEY,
					rgbBuffer, szTempName))
	{
		printf("Unable to export package size (%u)\n", GetLastError());
		goto Error;
	}

	// Export install size
	sprintf(rgbBuffer, "%I64u", liInstallSize.QuadPart / XBOX_CLUSTER_SIZE);
	if (!WritePrivateProfileString(
					REBUILD_AUTO_SECTION,
					REBUILD_INSTALL_SIZE_KEY,
					rgbBuffer, szTempName))
	{
		printf("Unable to export install size (%u)\n", GetLastError());
		goto Error;
	}

	// Now try to generate SQL queries for this package
	printf("Generating SQL command script ...\n");

	strcpy(szTempName2, szFilePath);
	strcat(szTempName2, REBUILD_SQL_QUERY_EXTENSION);

	hr = GenerateQueriesFromMetadataFile(
				dwTitleId, OfferingId, szTempName, szTempName2);
	if (FAILED(hr))
	{
		printf("%s\n", MetadataGetErrorString());
		goto Error;
	}

Exit:

	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return(hr);

Error:
	goto Exit;
}
			

