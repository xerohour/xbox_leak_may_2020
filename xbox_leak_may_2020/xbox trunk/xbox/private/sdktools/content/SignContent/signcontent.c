//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cryptcab.h"

#include "privkey.c"
#include "pubkey.c"

void PrintUsage()
{
	puts("SignContent - Xbox content package signing tool\n");
	puts("Usage:");
	puts("  SignContent <package_path>\n");
	puts("<package_path> - Name and location of package to sign\n");
}

int _cdecl main(
			int argc,
			char** argv
			)
{
	HRESULT		hr = S_OK;
	HANDLE		hFile = INVALID_HANDLE_VALUE;
	HCRYPTPROV	hCrypto = 0;
	HCRYPTKEY	hKeyPublic = 0;

	if (argc != 2)
	{
		PrintUsage();
		goto Exit;
	}

	// Initialize the crypto provider
	hr = InitializeCrypto(
				rgbPrivateKey, sizeof(rgbPrivateKey),
				rgbPublicKey, sizeof(rgbPublicKey),
				&hCrypto, &hKeyPublic, NULL, 0);
	if (hr != S_OK)
	{
		printf("Filaed to initialize crypto (%u)\n", hr);
		goto Exit;
	}

	// Open the pacakge
	hFile = CreateFile(
				argv[1],
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = GetLastError();
		printf("Cannot open package %s (%u)\n", argv[1], hr);
		goto Exit;
	}

	// Just sign the package with the DevKit key
	// hr = SignCabinetHeaderDevKit(hFile);
	hr = SignCabinetHeader(hFile, hCrypto, hKeyPublic);
	if (hr == S_OK)
	{
		printf("Content package %s successfully signed.\n", argv[1]);
	}
	else
	{
		printf("Failed to sign content package %s. (Error: %u)\n", argv[1], hr);
	}

Exit:

	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return(0);
}

