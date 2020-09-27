/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing utility functions for content delivery

Module Name:

    contutil.c

--*/

#include "xonp.h"
#include "xonver.h"

//
// Builds the target directory.
//
// Note: Coming in, *pcbTargetDirectory must count the NULL terminator,
// coming out, *pcbTargetDirectory does NOT include NULL terminator
//
HRESULT BuildTargetDirectory(
			DWORD				dwTitleId,
			XONLINEOFFERING_ID	OfferingId,
			DWORD				dwBitFlags,
			XONLINEDIR_TYPE		dirtype,
			DWORD				*pcbTargetDirectory,
			CHAR				*szTargetDirectory
			)
{
	DWORD	dwSize;

	// Build a switch statement to handle different directory types
	switch (dirtype)
	{
	case dirtypeTitleContent:
	case dirtypeUserContent:
	
		// Target is content
		dwSize = strlen(XONLINECONTENT_BASE_PATH) + 8;
		if (dwSize >= *pcbTargetDirectory)
			goto Error;
		
		*pcbTargetDirectory = sprintf(
					szTargetDirectory, 
					XONLINECONTENT_BASE_PATH,
					(dirtype==dirtypeUserContent)?'u':
						((dirtype==dirtypeTitleContent)?'t':'d'), 
					OfferingId, dwBitFlags);
		break;

	case dirtypeDVDContent:
	
		// Target is content
		dwSize = strlen(XONLINECONTENT_DVD_PATH) + 13;
		if (dwSize >= *pcbTargetDirectory)
			goto Error;
		
		*pcbTargetDirectory = sprintf(
					szTargetDirectory, 
					XONLINECONTENT_DVD_PATH,
					dwTitleId, OfferingId, dwBitFlags);
		break;
		
	case dirtypeTitleUpdate:
	
		// Target is title update
		dwSize = strlen(AUTOUPD_TITLE_READ_PATH);
		if (dwSize >= *pcbTargetDirectory)
			goto Error;

		strcpy(szTargetDirectory, AUTOUPD_TITLE_READ_PATH);
		*pcbTargetDirectory = dwSize;
		break;

	case dirtypeTitleUpdateOnF:
	
		// Target is title update
		dwSize = strlen(AUTOUPD_TITLE_TARGET_PATH);
		if (dwSize >= *pcbTargetDirectory)
			goto Error;

		strcpy(szTargetDirectory, AUTOUPD_TITLE_TARGET_PATH);
		*pcbTargetDirectory = dwSize;
		break;
		
	case dirtypeDashUpdate0:
	case dirtypeDashUpdate1:

		// Target is dash update
		dwSize = strlen(AUTOUPD_DASH_TARGET_PATH);
		if (dwSize >= *pcbTargetDirectory)
			goto Error;

		*pcbTargetDirectory = sprintf(
					szTargetDirectory,
					AUTOUPD_DASH_TARGET_PATH,
					(dirtype==dirtypeDashUpdate0)?'0':'1');
		break;
		
	case dirtypeTempDiff:

		// Target is temp directory for file diffs
		dwSize = strlen(AUTOUPD_TEMP_DIFF);
		if (dwSize >= *pcbTargetDirectory)
			goto Error;

		strcpy(szTargetDirectory, AUTOUPD_TEMP_DIFF);
		*pcbTargetDirectory = dwSize;
		break;
		
	case dirtypeTempUpdate:

		// Target is temp directory for expanded dash files
		dwSize = strlen(AUTOUPD_TEMP_UPDATE);
		if (dwSize >= *pcbTargetDirectory)
			goto Error;

		strcpy(szTargetDirectory, AUTOUPD_TEMP_UPDATE);
		*pcbTargetDirectory = dwSize;
		break;

	case dirtypeDashRoot:

		// Target is the root of dash
		dwSize = strlen(AUTOUPD_DASH_ROOT);
		if (dwSize >= *pcbTargetDirectory)
			goto Error;

		strcpy(szTargetDirectory, AUTOUPD_DASH_ROOT);
		*pcbTargetDirectory = dwSize;
		break;

	case dirtypeDVDTitleRoot:

		// Target is the root of dash
		dwSize = strlen(AUTOUPD_DVD_TITLE_ROOT);
		if (dwSize >= *pcbTargetDirectory)
			goto Error;

		strcpy(szTargetDirectory, AUTOUPD_DVD_TITLE_ROOT);
		*pcbTargetDirectory = dwSize;
		break;

	case dirtypeTitleContentOnF:
	
		// Target is title update
		dwSize = strlen(XONLINECONTENT_TITLE_CONTENT_F) + 8;
		if (dwSize >= *pcbTargetDirectory)
			goto Error;

		sprintf(szTargetDirectory, XONLINECONTENT_TITLE_CONTENT_F,
					OfferingId, dwBitFlags);
		*pcbTargetDirectory = dwSize;
		break;
		
	default:
		AssertSz(FALSE, "BuildTargetDirectory: Bad dirtype");
	}

	return(S_OK);
	
Error:
	*pcbTargetDirectory = dwSize;
	return(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));
}

//
// Builds the target file path
//
// Note: Coming in, *pcbTargetDirectory must count the NULL terminator,
// coming out, *pcbTargetDirectory does NOT include NULL terminator
//
HRESULT BuildTargetPath(
			DWORD				dwTitleId,
			XONLINEOFFERING_ID	OfferingId,
			DWORD				dwBitFlags,
			CHAR				*szPostfix,
			XONLINEDIR_TYPE		dirtype,
			DWORD				*pcbTargetPath,
			CHAR				*szTargetPath
			)
{
	HRESULT	hr = S_OK;
	DWORD	dwLength;
	DWORD	cbPostfix;

	// Build the directory portion
	dwLength = *pcbTargetPath;
	cbPostfix = strlen(szPostfix);

	// Build target base directory
	hr = BuildTargetDirectory(dwTitleId, OfferingId, dwBitFlags,
				dirtype, &dwLength, szTargetPath);
	if (FAILED(hr))
		goto Error;

	(*pcbTargetPath) -= dwLength;
	szTargetPath += dwLength;

	// Check size
	if ((*pcbTargetPath) <= cbPostfix)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
		goto Error;
	}

	// Copy the postfix path over
	strcpy(szTargetPath, szPostfix);

Error:
	*pcbTargetPath = dwLength + cbPostfix;
	return(hr);
}

//
// This function first locates the content path for the specified
// content offering, then uses that path to build a full path to 
// the specified file. This function fails if a content directory 
// does not exist for the specified content offering.
//
HRESULT CXo::BuildExistingContentPath(
			DWORD				dwTitleId,
			XONLINEOFFERING_ID	OfferingId,
			CHAR				*szPostfix,
			XONLINEDIR_TYPE		dirtype,
			DWORD				*pcbTargetPath,
			CHAR				*szTargetPath
			)
{
	HRESULT	hr = S_OK;
	DWORD	dwLength;
	DWORD	cbPostfix;

	// Only support content offerings
    Assert((dirtype == dirtypeTitleContent) || (dirtype == dirtypeUserContent));

	// Build the directory portion
	dwLength = *pcbTargetPath;
	cbPostfix = strlen(szPostfix);

	// Find the base directory for the content
	hr = XOnlineContentGetRootDirectory(
				OfferingId, 
				(dirtype == dirtypeUserContent),
				&dwLength,
				szTargetPath);
	if (FAILED(hr))
		goto Error;

	// Append the specified postfix
	if ((dwLength + cbPostfix) >= (*pcbTargetPath))
	{
		hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
		goto Error;
	}
	strcpy(szTargetPath + dwLength, szPostfix);

Error:
	*pcbTargetPath = dwLength + cbPostfix;
	return(hr);
}

//
// Function to set the end-of-file position
//
HRESULT CXo::SetEndOfFileNt(
			HANDLE	hFile,
			DWORD	dwSize
			)
{
	FILE_END_OF_FILE_INFORMATION	feofi;
	IO_STATUS_BLOCK					iosb;
	NTSTATUS						Status;

	// Set the end of file location to the file size we need
	feofi.EndOfFile.QuadPart = dwSize;
	Status = NtSetInformationFile(hFile, 
				&iosb, 
				(PVOID)&feofi, sizeof(FILE_END_OF_FILE_INFORMATION),
				FileEndOfFileInformation);
    if (!NT_SUCCESS(Status)) 
		return(HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status)));
	return(S_OK);
}

//
// Function to convert a hex string to a ULONGLONG
//
HRESULT CXo::HexStringToUlonglong(
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
HRESULT CXo::ConvertToTitleId(
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
// Function to determine if a directory name is a valid offering ID.
// If so, then the directory name will be converted to the offering ID 
// value. Otherwise, this function will return ERROR_INVALID_DATA.
//
HRESULT CXo::ConvertToOfferingId(
			PSTR				szOfferingId,
			XONLINEOFFERING_ID	*pOfferingId
			)
{
	HRESULT		hr;
	ULONGLONG	ull;
	hr = HexStringToUlonglong(szOfferingId, 8, &ull);
	*pOfferingId = (DWORD)ull;
	return(hr);
}


//
// Function to parse a content directory name, validating and splitting
// it into its components
//
HRESULT CXo::ParseContentDirName(
			PSTR				szContentDir,
			XONLINEOFFERING_ID	*pOfferingId,
			DWORD				*pdwBitFilter
			)
{
	HRESULT	hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
	PSTR	pTemp = strchr(szContentDir, '.');

	if (pTemp)
	{
		*pTemp++ = '\0';
		hr = ConvertToOfferingId(szContentDir, pOfferingId);
		if (FAILED(hr))
			goto Error;

		hr = ConvertToTitleId(pTemp, pdwBitFilter);
		if (FAILED(hr))
			goto Error;
	}

Error:	
	return(hr);
}

//
// Function to kick off a referral request given an offering ID
// and a pointer to the XONLINETASK_CONTENT_DOWNLOAD data structure
// that is initiating this content referral action.
//
// Note that the xrlasync member of pcontdl must already been filled in 
// because this function will refer to members in pcontdl->xrlasync. In
// particular, the hEventWorkAvailable field.
//
HRESULT CXo::InitiateReferralRequest(
			XONLINEOFFERING_ID				OfferingId,
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
    HRESULT hr;
	PXRL_ASYNC_EXTENDED				pxrlasyncextCatref;
	XONLINE_SERVICE_INFO			serviceCatref;
	PXONLINECONTENT_REFERRAL_REQ	prefreq;
	PXONLINE_USER					pusers;
	CHAR							szContentType[XONLINE_MAX_CONTENT_TYPE_SIZE];
	DWORD							cbContentType;
	DWORD							i;
	
	Assert(pcontdl != NULL);

	// Set up the async context and the service
	pxrlasyncextCatref = &pcontdl->xrlasyncCatref;

	// Fill in the request data. We hijack the general buffer as the
	// request buffer
	prefreq = (PXONLINECONTENT_REFERRAL_REQ)(pcontdl->install.rgbBuffer);
	prefreq->Header.cbSize = sizeof(XONLINECONTENT_REFERRAL_REQ);
	prefreq->Header.dwReserved = 0;
	prefreq->dwTitleId = pcontdl->install.dwTitleId;
	prefreq->OfferingId = OfferingId;
	prefreq->dwRating = XGetParentalControlSetting();
	prefreq->MachineId = 0; // BUGBUG: Tony will provide API

	// Now get the user credentials
	pusers = XOnlineGetLogonUsers();
	for (i = 0; i < XONLINE_MAX_LOGON_USERS; i++)
	{
		// If the user is a guest then s/he has no rights
		if (!pusers)
			prefreq->Puid[i] = 0;
		else if (XOnlineIsUserGuest(pusers[i].xuid.dwUserFlags))
			prefreq->Puid[i] = 0;
		else
			prefreq->Puid[i] = pusers[i].xuid.qwUserID;
	}

	// Build the content type header
	cbContentType = sprintf(szContentType, 
				"Content-type: xon/%x\r\n", XONLINE_BILLING_OFFERING_SERVICE);

	// Kick off the data upload, note we use the LZX decompression
	// buffer as the scratch pad
	return(UploadFromMemoryInternal(
				XONLINE_BILLING_OFFERING_SERVICE,
				XONLINECONTENT_REFERRAL_XRL,
				pcontdl->install.rgbDecompress,
				XONLINECONTENT_LZX_OUTPUT_SIZE,
				(PBYTE)szContentType,
				cbContentType,
				(PBYTE)prefreq,
				prefreq->Header.cbSize,
				XONLINECONTENT_REFERRAL_TIMEOUT,
				pcontdl->xrlasync.xontask.hEventWorkAvailable,
				pxrlasyncextCatref));
}

//
// Function to parse referral results for both download referrals
// and autoupdate referrals.
//
HRESULT CXo::ParseReferralResults(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl,
			PBYTE							pbBuffer,
			DWORD							cbBuffer
			)
{
	HRESULT					hr = S_OK;
	PXONLINECONTENT_INSTALL	pinstall;
	PXONLINEREFERRAL_RESP	presphdr;
	PXONLINE_SERVICE_INFO	pxsi;
	PBYTE					pbURI;
	DWORD					cbURI;
	PBYTE					pbHost;
	DWORD					cbHost;
	DWORD					dwEnd;
	DWORD					i;
	ULARGE_INTEGER			uliFreeSpace;
	ULARGE_INTEGER			uliTotalSpace;
	PXONLINEREFERRAL_LOCATION	plocation;
	PXONLINEREFERRAL_LOCATION	pcurloc = NULL;

	Assert(pcontdl != NULL);
	Assert(pbBuffer != NULL);

	pinstall = &pcontdl->install;

	// Over lay the data structure on the buffer and extract the
	// results.
	presphdr = (PXONLINEREFERRAL_RESP)pbBuffer;

	// Do some validation. Since the data came from the wire, we 
	// will actually do run-time validation and fail if anything 
	// looks fishy. 
	if (presphdr->Header.cbSize != cbBuffer)
		goto InvalidData;
	if (presphdr->cLocations == 0)
		goto InvalidData;
	if (presphdr->cLocations <= pinstall->dwLastXrl)
		pinstall->dwLastXrl = 0;

	// Symmetric Key
	memcpy(pinstall->rgbSymmetricKey, 
				presphdr->rgbSymKey, XONLINEREFERRAL_SYM_KEY_SIZE);

	// Public Key
	memcpy(pinstall->rgbPublicKey, 
				presphdr->rgbPubKey, XONLINEREFERRAL_PUB_KEY_SIZE);

	// Package bit flags
	pinstall->dwBitFlags = presphdr->dwBitFlags;

	// Package size
	pinstall->dwPackageSize = presphdr->dwPackageSize;
	pinstall->dwInstallSize = presphdr->dwInstallSize;

	// OK, now we check to make sure there is sufficient disk space
	// for this package. Note if GetDiskFreeSpace fails, we will go 
	// ahead and install this simply because it's a better experience
	// than a cryptic error message. If the disk becomes full, then
	// ERROR_DISK_FULL will be returned later in XOnlineTaskContinue.
	if (GetDiskFreeSpaceEx("T:\\", &uliFreeSpace, &uliTotalSpace, NULL))
	{
		// See if there is sufficient space, note that install size is 
		// ,resured in blocks
		if ((uliFreeSpace.QuadPart / XBOX_CLUSTER_SIZE) < presphdr->dwInstallSize)
		{
			hr = HRESULT_FROM_WIN32(ERROR_DISK_FULL);
			goto Error;
		}
	}

	// The XRL locations are a packed array of XONLINEREFERRAL_LOCATION
	Assert(pinstall->dwLastXrl < presphdr->cLocations);
	
	dwEnd = sizeof(XONLINEREFERRAL_RESP);
	for (i = 0; i < presphdr->cLocations; i++)
	{
		plocation = (PXONLINEREFERRAL_LOCATION)(pbBuffer + dwEnd);
		dwEnd += (sizeof(WORD) + sizeof(WORD) + plocation->cbXrl);

		if (dwEnd > cbBuffer)
			goto InvalidData;
		if (plocation->cbXrl == 0)
			goto InvalidData;
		if (plocation->cbXrl > XONLINEREFERRAL_MAX_XRL_SIZE)
			goto InvalidData;
		if (pbBuffer[dwEnd - 1] != '\0')
			goto InvalidData;

		// Skip the ones that we already tried
		if (i >= pinstall->dwLastXrl)
		{
			// OK, extract the information into pcontdl.

			// XRL path to the content
			pbHost = (PBYTE)plocation->szXrl;
			cbHost = plocation->cbXrl - 1;
			hr = XRL_SkipToURI(pbHost, cbHost, &pbURI, &cbURI);
			if (FAILED(hr))
				continue;

			// Make sure we got a host
			if ((pbURI == pbHost) || (cbURI == 0))
				continue;

			// Copy the URI over
			memcpy(pinstall->szResourcePath, pbURI, cbURI);
			pinstall->szResourcePath[cbURI] = '\0';

			// Convert the host string to an IP address
			hr = XRL_ConvertToIPAddressAndPort(pbHost, cbHost - cbURI, 
						&(pinstall->xsiDownload.serviceIP.s_addr),
						&(pinstall->xsiDownload.wServicePort));
			if (FAILED(hr))
				continue;

			// Fill in the rest of the download service info
			pinstall->xsiDownload.dwServiceID = 0;
			pinstall->xsiDownload.wReserved = 0;

			// OK, done our best-effort verification, assign the donwload
			// service to our xrlasync structure
			RtlCopyMemory( &pcontdl->xrlasync.serviceInfo, &(pinstall->xsiDownload), sizeof(pcontdl->xrlasync.serviceInfo) );

			TraceSz2(Verbose, "Referral: downloading: %s on port %u\r\n", 
						pbHost, pinstall->xsiDownload.wServicePort);

			// Bump the XRL so we will try something else the next time
			pinstall->dwLastXrl = (i + 1);

			// Found a candidate, break out
			break;
		}
	}

	// If none of the XRLs are good then we fail
	if (i >= presphdr->cLocations)
		goto InvalidData;

	hr = S_OK;

Exit:
	return(hr);

Error:
	goto Exit;

InvalidData:
	hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
	goto Error;
}

