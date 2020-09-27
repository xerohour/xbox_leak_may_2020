// ---------------------------------------------------------------------------------------
// rebldcab.h
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#ifndef __REBLDCAB_H__
#define __REBLDCAB_H__

// ---------------------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------------------

#include <xcrypt.h>

#include "dgstfile.h"

// ---------------------------------------------------------------------------------------

//
// Define a callback context for rebuild processing
//
typedef struct 
{
	HFCI				hfci;
	BYTE				rgbSymmetricKey[XONLINE_KEY_LENGTH];

	// Digest generation stuff
	PDGST_CONTEXT		pdc;

	// Name of temp file
	CHAR				szTempFile[MAX_PATHNAME_LENGTH];

	// File name to be stored in CAB for insert
	CHAR				szFullName[MAX_PATHNAME_LENGTH];

	// Install size of the package. This factors in the fact that
	// files must be an integer number of blocks.
	ULONGLONG			InstallSize;	

} REBLDCBK_CONTEXT, *PREBLDCBK_CONTEXT;


//
// Function to rebuild a cabinet file and insert a digest file
//
HRESULT RebuildCabinetWithDigest(
			char *szOldCabinet,
			char *szNewCabinet,
			DWORD cFiles,
			char *szTempFile,
			char *szDigestFileName,
			PDGSTFILE_IDENTIFIER pdfi,
			PBYTE pbSymmetricKey,
			DWORD cbSymmetricKey,
			LARGE_INTEGER *pliPackageSize,
			LARGE_INTEGER *pliInstallSize
			);

// ---------------------------------------------------------------------------------------

#endif


