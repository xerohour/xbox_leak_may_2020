/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing patching utility functions

Module Name:

    patchutl.cpp

--*/

#include "xonp.h"
#include "xonver.h"

//
// Function to decode a compressed 32-bit unsigned quantity
//
PUCHAR
__fastcall
VariableLengthUnsignedDecode(
			IN  PUCHAR Buffer,
			IN	DWORD  *pcbBuffer,
			OUT PULONG ReturnValue
			)
{
	PUCHAR p = Buffer;
	ULONG Value = 0;
	ULONG Shift = 0;

	do
	{
		if (!*pcbBuffer)
			return(NULL);
		(*pcbBuffer)--;
		
		Value |= (( *p & 0x7F ) << Shift );
		Shift += 7;

	} while (( ! ( *p++ & 0x80 )) && ( Shift < 32 ));

	*ReturnValue = Value;

	return(p);
}

//
// Function to decode a compressed 32-bit signed quantity
//
PUCHAR
__fastcall
VariableLengthSignedDecode(
			IN  PUCHAR Buffer,
			IN	DWORD  *pcbBuffer,
			OUT PLONG  ReturnValue
			)
{
	PUCHAR p = Buffer;
	ULONG Shift;
	LONG  Value;

	Value = *p & 0x3F;
	Shift = 6;

	if ( ! ( *p++ & 0x80 )) 
	{
		do
		{
			if (!*pcbBuffer)
				return(NULL);
			(*pcbBuffer)--;
			
			Value |= (( *p & 0x7F ) << Shift );
			Shift += 7;
			
		} while (( ! ( *p++ & 0x80 )) && ( Shift < 32 ));
	}

	if ( *Buffer & 0x40 ) 
	{
		Value = -Value;
	}

	*ReturnValue = Value;

	return(p);
}

//
// Function to parse patch file information into our own structures
//
// Note: this function is kind of complicated because of the file
// format chosen by the MSR people. They really tried to squeeze out 
// every single extraneous byte out of the header. This is most
// prominently demonstrated by the optional fields and compressed
// 32-bit numbers. This makes it impossible for us to just overlay a
// fixed structure to access its constituent fields.
//
// There is no documentation on the format and I spent quite a while
// figuring this out, so I am documenting this so that we will not
// have to go back to the patching source code over and over to 
// get this info.
//
// The patch file header is described here. Fields are prefixed with
// the corresponding datatype. Of note, the vs and vu prefixes stand
// for variable-length signed and unsigned 32-bit values, respectively.
// see the decoding functions at the top of this file for details.
// A 'u' among fields indicate a union switched by the condidion 
// on the right. Fields marked with an 'x' indicates those that we
// will not support and thus will not be present in Xbox patch files.
//
// Main Header:
//
//	dwSignature
//	dwOptionFlags
//		|
//		+-	dwExtendedOptionFlags	(PATCH_OPTION_EXTENDED_OPTIONS) ----+
//x		+-	dwNewFileTime			(~PATCH_OPTION_NO_TIMESTAMP)		|
//x		+-	wNewFileCoffBase		(~PATCH_OPTION_NO_REBASE)			|
//x		|		+u	vsDelta				(dwNewFileTime != 0)			|
//x		|		+u	dwNewFileCoffTime	(dwNewFileTime == 0)			|
//x		+-							(~PATCH_OPTION_NO_RESTIMEFIX)		|
//x				+u	vsDelta				(NewFileCoffTime != 0)			|
//x				+u	dwNewFileResTime	(NewFileCoffTime == 0)			|
//																		|
//	vuNewFileSize														|
//	dwNewFileCRC														|
//			bWindowSizeBits -----( PATCH_OPTION_SPECIFIED_WINDOW )------+
//	bOldFileCount
//
// The main header is followed by bOldFileCount records, each indicating
// an old file version from which we know how to patch up to the current
// version. For Xbox patch files, bOldFileCount will always be 1, this is
// because the autoupdate front doors will always choose the right update
// package for us so we will not need multiple updateversions in a package.
//
// Subfields with a '*' next to is means the parent value indicates how 
// many sets of values there are (see bIgnoreRangeCount). Similarly, 
// fields with a 'x' to the left are not supported and thus omitted in
// Xbox patch files.
//
// Old File Record:
//
//	vsSizeDelta
//	dwOldFileCRC
//	bIgnoreRangeCount	(=0)
//x		+*	vsDelta
//x		+*	vuLength 	(bIgnoreRangeCount sets of {Delta, Length} pairs)
//	bRetainRangeCount	(=0)
//x		+*	vsDelta
//x		+*	vsDeltaNew
//x		+*	vuLength
//	vuRiftEntryCount	(=0)
//x		+*	vuDeltaPos
//x		+*	vsDeltaNew
//	vuCountInterleaveRanges	(dwOptionFlags & PATCH_OPTION_INTERLEAVE_FILES)
//		+*	vsDeltaOldOffset
//		+*	vsDeltaOldLength
//		+*	vsDeltaNewLength
//	vuPatchDataSize
//
// The following are more details about which flags are used in the Xbox
// patch file format:
//
// We use the following fixed values for dwOptionFlags:
// PATCH_OPTION_EXTENDED_OPTIONS |
// PATCH_OPTION_NO_TIMESTAMP |
// PATCH_OPTION_NO_REBASE |
// PATCH_OPTION_NO_RESTIMEFIX |
// PATCH_OPTION_INTERLEAVE_FILES
// 
// And the following fixed values for dwExtendedOptionFlags:
// PATCH_OPTION_SPECIFIED_WINDOW
//

//
// Define patch signature
//
#define PATCH_SIGNATURE				'91AP'

#define PATCH_OPTION_SPECIFIED_WINDOW   0x00010000  // lzx window size encoded
#define PATCH_OPTION_USE_LZX_A          0x00000001  // normal
#define PATCH_OPTION_USE_LZX_B          0x00000002  // better on some x86 binaries
#define PATCH_OPTION_USE_LZX_LARGE      0x00000004  // better support for large files (version 1.97 and higher)
#define PATCH_OPTION_NO_REBASE          0x00040000  // PE rebased image
#define PATCH_OPTION_NO_RESTIMEFIX      0x00400000  // PE resource timestamps
#define PATCH_OPTION_NO_TIMESTAMP       0x00800000  // don't store new file timestamp in patch
#define PATCH_OPTION_INTERLEAVE_FILES   0x40000000  // better support for large files (version 1.99 and higher)
#define PATCH_OPTION_RESERVED1          0x80000000  // (used internally)
#define PATCH_OPTION_EXTENDED_OPTIONS   PATCH_OPTION_RESERVED1

//
// Define the required set of options flags
//
#define	PATCH_REQUIRED_FLAGS	\
		(PATCH_OPTION_NO_REBASE |\
		 PATCH_OPTION_NO_RESTIMEFIX |\
		 PATCH_OPTION_USE_LZX_A)

//
// Define the optional flags
//
#define PATCH_OPTIONAL_FLAGS	\
		(PATCH_OPTION_EXTENDED_OPTIONS |\
		 PATCH_OPTION_NO_TIMESTAMP |\
		 PATCH_OPTION_INTERLEAVE_FILES |\
		 PATCH_OPTION_USE_LZX_LARGE)

//
// Define the set of invalid flags
//
#define PATCH_INVALID_FLAGS		\
		(~(PATCH_REQUIRED_FLAGS | PATCH_OPTIONAL_FLAGS))

//
// Define the smallest possible valid header size
//
#define PATCH_MIN_HEADER_SIZE		(28)

HRESULT ParsePatchHeader(
			PBYTE	pbData,
			DWORD	*pcbData,
			PBYTE	pbParsedData,
			DWORD	*pcbParsedData
			)
{
	HRESULT			hr = S_OK;
	PBYTE			pbCur = pbData;
	DWORD			cbData;
	DWORD			dwOutSize;
	DWORD			dwValue;
	DWORD			i;
	LONG			lValue;
	DWORD			RemainingNew;
	DWORD			LastOldOffset = 0;
	DWORD			LastOldLength = 0;
	DWORD			LastNewLength = 0;
	LONG			DeltaDeltaOldOffset;
	LONG			DeltaDeltaOldLength;
	LONG			DeltaNewLength;
	LONG			LastDeltaOldOffset = 0;
	LONG			LastDeltaOldLength = 0;
	PAUTOUPD_PATCH_FILE_INFO	pInfo;
	PAUTOUPD_INTERLEAVE_ENTRY	pEntry;

	BOOL			fWindowSpecified = FALSE;
	BOOL			fTimeStamp = FALSE;
	BOOL			fInterleaveMap = FALSE;
		
	Assert(pbData);
	Assert(pcbData);
	Assert(pbParsedData);
	Assert(pcbParsedData);

	cbData = *pcbData;
	if (cbData < PATCH_MIN_HEADER_SIZE)
		goto InvalidData;			
		
	dwOutSize = sizeof(AUTOUPD_PATCH_FILE_INFO);		
	if (*pcbParsedData < dwOutSize)
		goto InsufficientBuffer;

	// Overlay the info struct
	pInfo = (PAUTOUPD_PATCH_FILE_INFO)pbParsedData;
	pInfo->dwDataSize = 0;
	pInfo->dwCurrent = 0;
	pInfo->dwTargetWritten = 0;
	pInfo->dwInterleaveCurrent = 0;

	// Verify signature		
	if (PATCH_SIGNATURE != *(DWORD *)pbCur)
		goto InvalidData;
	pbCur += sizeof(DWORD);
	cbData -= sizeof(DWORD);

	// Get and verify options
	dwValue = *(DWORD *)pbCur;
	pbCur += sizeof(DWORD);
	cbData -= sizeof(DWORD);
	if (((dwValue & PATCH_REQUIRED_FLAGS) != PATCH_REQUIRED_FLAGS) ||
		((dwValue & PATCH_INVALID_FLAGS) != 0))
		goto InvalidData;

	// Timestamp is stored reversed 
	dwValue ^= PATCH_OPTION_NO_TIMESTAMP;

	// Check for explicit Interleave map 
	fInterleaveMap = ((dwValue & PATCH_OPTION_INTERLEAVE_FILES) != 0);
	fTimeStamp = ((dwValue & PATCH_OPTION_NO_TIMESTAMP) == 0);

	// Check for extended options
	if (dwValue & PATCH_OPTION_EXTENDED_OPTIONS)
	{
		// Get and verify extended options
		dwValue = *(DWORD *)pbCur;
		pbCur += sizeof(DWORD);
		cbData -= sizeof(DWORD);

		// Currently we only support window size specification
		if (dwValue & PATCH_OPTION_SPECIFIED_WINDOW)
			fWindowSpecified = TRUE;
	}

	// Skip over timestamp
	if (fTimeStamp)
	{
		pbCur += sizeof(DWORD);
		cbData -= sizeof(DWORD);
	}

	// Get new file size
	pbCur = VariableLengthUnsignedDecode(
				pbCur, &cbData, &(pInfo->dwNewFileSize));
	if (!pbCur)
		goto InvalidData;

	// Skip CRC 
	if (cbData < sizeof(DWORD))
		goto InvalidData;
	pbCur += sizeof(DWORD);
	cbData -= sizeof(DWORD);

	if (fWindowSpecified)
	{
		// Get window size
		if (cbData < sizeof(BYTE))
			goto InvalidData;
		pInfo->dwWindowSizeBits = *pbCur;
		pbCur += sizeof(BYTE);
		cbData -= sizeof(BYTE);
	}

	// Get old file count
	if (cbData < sizeof(BYTE))
		goto InvalidData;
	Assert(*pbCur == 1);
	if (*pbCur != 1)
		goto InvalidData;
	pbCur += sizeof(BYTE);
	cbData -= sizeof(BYTE);

	// Get the file size delta
	pbCur = VariableLengthSignedDecode(pbCur, &cbData, &lValue);
	if (!pbCur)
		goto InvalidData;

	if ((lValue + pInfo->dwNewFileSize) < (LONG)0)
		goto InvalidData;
	pInfo->dwOldFileSize = lValue + pInfo->dwNewFileSize;

	// Skip CRC 
	if (cbData < sizeof(DWORD))
		goto InvalidData;
	pbCur += sizeof(DWORD);
	cbData -= sizeof(DWORD);

	// Ignore and retain ranges should be 0
	if (cbData < sizeof(WORD))
		goto InvalidData;
	if (*(WORD *)pbCur != 0)
		goto InvalidData;
	pbCur += sizeof(WORD);
	cbData -= sizeof(WORD);

	// Get Rift entry count, should be zero
	pbCur = VariableLengthUnsignedDecode(
				pbCur, &cbData, &dwValue);
	if (!pbCur)
		goto InvalidData;
	Assert(dwValue == 0);
	if (dwValue != 0)
		goto InvalidData;

	// Set up for interleave map
	RemainingNew = pInfo->dwNewFileSize;
	LastOldOffset = 0;
	LastOldLength = 0;
	LastNewLength = 0;
	LastDeltaOldOffset = 0;
	LastDeltaOldLength = 0;

	if (fInterleaveMap)
	{
		// Get the interleave map entry count
		pbCur = VariableLengthUnsignedDecode(
					pbCur, &cbData, &(pInfo->dwInterleaveEntries));
		if (!pbCur)
			goto InvalidData;

		// Make sure the output buffer is big enough
		Assert(pInfo->dwInterleaveEntries > 0);
		if (!pInfo->dwInterleaveEntries)
			goto InvalidData;
			
		dwOutSize += ((pInfo->dwInterleaveEntries - 1) * 
					sizeof(AUTOUPD_INTERLEAVE_ENTRY));
		if (*pcbParsedData < dwOutSize)				
			goto InsufficientBuffer;

		// Load the interleave map entries
		for (i = 0; i < pInfo->dwInterleaveEntries; i++)
		{
			pEntry = &(pInfo->rgInterleaveInfo[i]);

			pbCur = VariableLengthSignedDecode(
						pbCur, &cbData, &DeltaDeltaOldOffset);
			if (!pbCur)
				goto InvalidData;
			pbCur = VariableLengthSignedDecode(
						pbCur, &cbData, &DeltaDeltaOldLength);
			if (!pbCur)
				goto InvalidData;

			LastDeltaOldOffset += DeltaDeltaOldOffset;
			LastDeltaOldLength += DeltaDeltaOldLength;

			LastOldLength  += LastDeltaOldLength;
			LastOldOffset  += LastDeltaOldOffset;

			pEntry->dwOldOffset = LastOldOffset;
			pEntry->dwOldLength = LastOldLength;

			if ((LastOldOffset + LastOldLength) > pInfo->dwOldFileSize)
				goto InvalidData;

			if (i < (pInfo->dwInterleaveEntries - 1)) 
			{
				pbCur = VariableLengthSignedDecode(
							pbCur, &cbData, &DeltaNewLength);
				if (!pbCur)
					goto InvalidData;

				DeltaNewLength *= CHUNK_SIZE;	// LZX specific
				LastNewLength  += DeltaNewLength;

				pEntry->dwNewLength = LastNewLength;

				if (RemainingNew < LastNewLength)
					goto InvalidData;

				RemainingNew -= LastNewLength;
			}
			else 
			{
			    pEntry->dwNewLength = RemainingNew;
			}
		}
	}
	else
	{
		// There is only one interleaved section, mark the section
		// as the entire file (size will be validated later)
		pInfo->dwInterleaveEntries = 1;
		
		pEntry = &(pInfo->rgInterleaveInfo[0]);
		pEntry->dwOldOffset = 0;
		pEntry->dwOldLength = pInfo->dwOldFileSize;
	    pEntry->dwNewLength = RemainingNew;
	}

	// Finally, get the patch data size
	pbCur = VariableLengthUnsignedDecode(
				pbCur, &cbData, &(pInfo->dwPatchDataSize));
	if (!pbCur)
		goto InvalidData;

	// Return the actual header data size
	*pcbData = pbCur - pbData;

	// Return the parsed data size
	*pcbParsedData = dwOutSize;

Exit:
	return(hr);

InsufficientBuffer:
	hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
	goto Error;

InvalidData:
	hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);

Error:
	goto Exit;
}


