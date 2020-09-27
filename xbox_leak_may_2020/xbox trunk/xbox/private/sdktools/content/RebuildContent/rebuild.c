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

void PrintUsage()
{
	puts("RebuildContent - Xbox content package rebuilder tool\n");
	puts("Usage:");
	puts("  RebuildContent <Content Path> <Content ID> <Title ID> [<VerHi> <VerLo>]\n");
	puts("<Content Path> - Name and location of content package to rebuild");
	puts("<Content ID> - Hexadecimal Content ID assigned to this package");
	puts("<Title ID> - Optional, Hexadecimal Title ID for which this package is built");
	puts("<VerHi> - Optional, Major version of title after application of patch");
	puts("<VerLo> - Optional, Minor version of title after application of patch\n");
	puts("Notes:");
	puts("This application takes a publisher-supplied content package");
	puts("and extracts all the metadata info and repackages it using a");
	puts("new symmetric key.\n");
	puts("This tool rebuilds content packages for both title content and");
	puts("security updates. If the package is title content, then VerHi,");
	puts("and VerLo must be omitted. For security updates, Title ID");
	puts("VerHi and VerLo must be supplied, and non-zero\n");
	puts("On success, the specified update will be rebuilt into a new file, and");
	puts("the following files will be generated:\n");
	puts("<Content Path>.new - the rebuilt update package.");
	puts("<Content Path>.cab - the raw cabinet file in the original package.");
	puts("<Content Path>.xbx - the metadata file submitted with the package.");
	puts("symmkey.c - the new symmetric key exported as a C file.\n");
	puts("These files are generated in the directory from which this");
	puts("application is launched.\n");
}

int _cdecl main(
			int argc,
			char** argv
			)
{
	HRESULT		hr = S_OK;
	DWORD		dwTitleId = 0;
	DWORD		OfferingId;
	WORD		wVerHi = 0;
	WORD		wVerLo = 0;
	
	if ((argc != 4) && (argc != 6))
		PrintUsage();
	else
	{
		// Parse the Content ID
		if (sscanf(argv[2], "%x", &OfferingId) != 1)
		{
			PrintUsage();
			return(1);
		}
		if (sscanf(argv[3], "%x", &dwTitleId) != 1)
		{
			PrintUsage();
			return(1);
		}
		if (dwTitleId == 0)
		{
			puts("Title ID must be a non-zero hexadecimal value\n");
			PrintUsage();
			return(1);
		}

		if (argc == 6)
		{
			// Convert the other values
			wVerHi = (WORD)atoi(argv[4]);
			wVerLo = (WORD)atoi(argv[5]);
		}
		
		printf("Rebuilding content package %s with the following parameters:\n"
				"    Content ID: 0x%08X\n"
				"    Title ID: 0x%08X\n",
				argv[1], OfferingId, dwTitleId);
		if (argc == 6)
		{
			printf("    Update version: %u.%u\n\n", wVerHi, wVerLo);
		}
		else
			puts("");
		
		hr = RebuildContentPackage(
					argv[1], OfferingId, dwTitleId, wVerHi, wVerLo,
					rgbPrivateKey, sizeof(rgbPrivateKey));
		if (hr == S_OK)
		{
			printf("\nContent package %s successfully processed and rebuilt\n", argv[1]);
		}
		else
		{
			printf("\nFailed to rebuild content package %s. (Error: %u)\n", argv[1], hr);
		}
	}

	return(0);
}

