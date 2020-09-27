/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau   (keithlau)

Description:
	Implementation of piecemeal decryption routines using a
	symmetric key and the RC4-SHA1 Hmac algorithm.

Module Name:

	symmdec.c

--*/

#include "xonp.h"
#include "xonver.h"
#include <shahmac.h>

//
// API to initialize decryption context
//
HRESULT CXo::SymmdecInitialize(
			PBYTE					pbKey,
			DWORD					cbKey,
			DWORD					cbLength,
			PRC4_SHA_HEADER			pHeader,
			PXONLINESYMMDEC_CONTEXT	psymmdec
			)
{
	BYTE		rgbLocalKey[XC_DIGEST_LEN];
	BYTE		rgbConfounder[RC4_CONFOUNDER_LEN];

	Assert(pbKey != NULL);
	Assert(cbKey >= sizeof(DWORD));
	Assert(cbLength > 0);
	Assert(pHeader != NULL);
	Assert(psymmdec != NULL);

	// Set up the members of psymmdec
	psymmdec->cbLength = cbLength;
	psymmdec->dwCurrent = 0;
	psymmdec->pbKey = pbKey;
	psymmdec->cbKey = cbKey;
	memcpy(psymmdec->rgbChecksum, pHeader->Checksum, XC_DIGEST_LEN);

	// First, figure out our local key
	XShaHmacInitialize(pbKey, cbKey, psymmdec->shactx);
	XShaHmacUpdate(psymmdec->shactx, 
				pHeader->Checksum, XC_DIGEST_LEN);
	XShaHmacComputeFinal(psymmdec->shactx, 
				pbKey, cbKey, rgbLocalKey);

	// Establish our local key in the RC4 context
    XcRC4Key(psymmdec->rc4ctx, XC_DIGEST_LEN, rgbLocalKey);

	// Now, figure out the original confounder
	memcpy(rgbConfounder, pHeader->Confounder, RC4_CONFOUNDER_LEN);
	XcRC4Crypt(psymmdec->rc4ctx, RC4_CONFOUNDER_LEN, rgbConfounder);

	// Now, restart the SHA1-Hmac to compute the checksum of the
	// original confounder and the decrypted data
	XShaHmacInitialize(pbKey, cbKey, psymmdec->shactx);
	XShaHmacUpdate(psymmdec->shactx, 
				rgbConfounder, RC4_CONFOUNDER_LEN);
	
    return(S_OK);
}

//
// API to decrypt a buffer one piece at a time, decryption
// happens in-place.
//
HRESULT CXo::SymmdecDecrypt(
			PXONLINESYMMDEC_CONTEXT	psymmdec,
			PBYTE					pbData,
			DWORD					cbData
			)
{
	Assert(pbData != NULL);
	Assert(cbData >= 0);
	Assert(psymmdec != NULL);

	// Too much data is bad
	if ((psymmdec->cbLength != XONLINESYMMDEC_UNKNOWN_LENGTH) && 
		((psymmdec->dwCurrent + cbData) > psymmdec->cbLength))
		return(HRESULT_FROM_WIN32(ERROR_BAD_LENGTH));

	// Decrypt the data ...
	XcRC4Crypt(psymmdec->rc4ctx, cbData, pbData);

	// Update the Hmac with the decrypted data
	XShaHmacUpdate(psymmdec->shactx, pbData, cbData);

	// Track how much data we've processed
	psymmdec->dwCurrent += cbData;

	return(S_OK);
}

//
// API to terminate a decryption session. This makes sure
// the data processed equals the expected length of the encrypted
// block. The SHA1-Hmac checksum is also compared agains the 
// original checksum.
//
HRESULT CXo::SymmdecVerify(
			PXONLINESYMMDEC_CONTEXT	psymmdec
			)
{
	BYTE		rgbChecksum[XC_DIGEST_LEN];

	Assert(psymmdec != NULL);

	// Too much or too little data is bad
	if ((psymmdec->cbLength != XONLINESYMMDEC_UNKNOWN_LENGTH) && 
		(psymmdec->dwCurrent != psymmdec->cbLength))
		return(HRESULT_FROM_WIN32(ERROR_BAD_LENGTH));

	// Compute the final Hmac checksum, and make sure it matches
	// with the original checksum.
	XShaHmacComputeFinal(psymmdec->shactx, 
				psymmdec->pbKey, psymmdec->cbKey, rgbChecksum);	
	if (memcmp(psymmdec->rgbChecksum, rgbChecksum, XC_DIGEST_LEN) != 0)
		return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
		
	return(S_OK);
}

//
// API to return the number of bytes still expected by the decoder
//
HRESULT CXo::SymmdecGetRemainingCount(
			PXONLINESYMMDEC_CONTEXT	psymmdec,
			PDWORD					pdwBytesRemaining
			)
{
	Assert(psymmdec != NULL);
	Assert(pdwBytesRemaining != NULL);

	if (psymmdec->cbLength == XONLINESYMMDEC_UNKNOWN_LENGTH)
		*pdwBytesRemaining = XONLINESYMMDEC_UNKNOWN_LENGTH;
	else if (psymmdec->dwCurrent > psymmdec->cbLength)
		*pdwBytesRemaining = 0;
	else 
		*pdwBytesRemaining = psymmdec->cbLength - psymmdec->dwCurrent;

	return(S_OK);
}


