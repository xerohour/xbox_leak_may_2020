// ---------------------------------------------------------------------------------------
// dgstfile.h
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#ifndef __DGSTFILE_H__
#define __DGSTFILE_H__

// ---------------------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------

//
// Define the current digest file version
//
#define DGSTFILE_VERSION_HI			((WORD)1)
#define DGSTFILE_VERSION_LO			((WORD)0)

//
// Define the various structures that constitute the digest info
//

//
// In-memory digest info
//
typedef struct 
{
	DWORD		dwSectionStart;
	DWORD		dwSectionSize;
	BYTE		rgbSignature[XCALCSIG_SIGNATURE_SIZE];

} DGST_SECTION_DATA, *PDGST_SECTION_DATA;

typedef struct
{
	CHAR				szFileName[MAX_PATH];
	DWORD				dwFileSize;
	BYTE				bFlags;
	BYTE				bReservedSize;
	WORD				wSections;
	DGST_SECTION_DATA	rgSectionData[1]; // wSections records

	// bReservedSize bytes here ...
	
} DGST_FILE_DATA, *PDGST_FILE_DATA;

typedef struct
{
	DWORD				dwFileSize;
	BYTE				bFlags;
	BYTE				bReservedSize;
	WORD				wSections;
	DGST_SECTION_DATA	rgSectionData[1]; // wSections records

	// bReservedSize bytes here ...
	
} DGSTFILE_FILE_DATA, *PDGSTFILE_FILE_DATA;
	
typedef struct
{
	DWORD				dwHeaderReserved;
	DWORD				dwPerFileReserved;
	
	DWORD				dwEntriesAlloc;
	DWORD				dwEntriesCur;
	PDGST_FILE_DATA		rgdfd;

} DGST_CONTEXT, *PDGST_CONTEXT;

typedef struct
{
	XONLINEOFFERING_ID	OfferingId;
	DWORD				dwTitleId;
	WORD				wTitleVersionHi;
	WORD				wTitleVersionLo;

} DGSTFILE_IDENTIFIER, *PDGSTFILE_IDENTIFIER;

typedef struct
{
	BYTE				rgbSignature[XCALCSIG_SIGNATURE_SIZE];
	BYTE				bDigestVersionHi;
	BYTE				bDigestVersionLo;
	WORD				wReservedSize;
	WORD				wFlags;
	WORD				wRating;
	DWORD				cDigestEntries;
	DGSTFILE_IDENTIFIER	dfi;

} DGSTFILE_HEADER, *PDGSTFILE_HEADER;	

//
// Header flags
//
#define DGSTFILE_HEADER_FLAG_SHARED		((WORD)0x8000)

typedef struct
{
	PDGSTFILE_HEADER	pHeader;
	DWORD				*rgdwOffsetVector;
	DWORD				dwCurrent;
	DWORD				cbDigestData;
	PBYTE				pbDigestData;

} DGSTFILE_ENUM_CONTEXT, *PDGSTFILE_ENUM_CONTEXT;

#ifndef XBOX

//
// API to initialize the digest builder
//
HRESULT CreateDigest(
			DWORD			dwMaxEntries,
			DWORD			dwHeaderReservedSize,
			DWORD			dwPerFileReservedSize,
			PDGST_CONTEXT	*ppContext
			);

//
// API to Insert a file record into the digest
//
HRESULT InsertFileIntoDigest(
			PDGST_CONTEXT		pContext,
			PDGST_FILE_DATA		pdfd
			);

//
// API to serialze an in-memory digest to a file
//
HRESULT SerializeDigestToFile(
			PDGST_CONTEXT		pContext,
			CHAR				*szDigestFile,
			PDGSTFILE_HEADER	pHeader,
			PBYTE				pbSymmetricKey,
			DWORD				cbSymmetricKey,
			PBYTE				pbHeaderReservedData
			);

//
// API to free a digest context
//
VOID FreeDigest(
			PDGST_CONTEXT	pContext
			);

//
// API to create a digest file from a file list
//
HRESULT CreateDigestFromFileList(
			cab_file_list_header	*pFileList,
			PDGSTFILE_HEADER		pdfh,
			PSTR					szDigestPath,
			PBYTE					pbSymmetricKey,
			DWORD					cbSymmetricKey
			);

//
// API to dump a digest file
//
HRESULT DumpDigest(
			CHAR	*szDigestFile,
			BOOL	fVerify,
			PBYTE	pbSymmetricKey,
			DWORD	cbSymmetricKey
			);

#else

//
// API to begin a digest file enumeration
//
HRESULT EnumerateDigestInitializeContext(
			PBYTE					pbDigestData,
			DWORD					cbDigestData,
			PDGSTFILE_ENUM_CONTEXT	pContext
			);

//
// API to get entries in a digest file (returns a reference to the
// data instead of a copy)
//
HRESULT EnumerateDigestNext(
			PDGSTFILE_ENUM_CONTEXT	pContext,
			PSTR					*ppszFileName,
			PDGSTFILE_FILE_DATA		*ppData
			);			

//
// API to find a specific entry in a digest file (returns a reference to the
// data instead of a copy)
//
HRESULT EnumerateDigestFind(
			PDGSTFILE_ENUM_CONTEXT	pContext,
			PSTR					pszFileName,
			PDGSTFILE_FILE_DATA		*ppData
			);

#endif			

// ---------------------------------------------------------------------------------------

#endif

