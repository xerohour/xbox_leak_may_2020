/*++

Copyright (c) Microsoft Corporation

Author:
	Keith Lau (keithlau)

Description:
	Implementation of the CFFILE cache

Module Name:

    cfcache.c

--*/

#include "xonp.h"
#include "xonver.h"

//
// Initialization routine
//
HRESULT CXo::CffileCacheInitialize(
			HANDLE						hFile,
			DWORD						dwCffileOffset,
			WORD						wCffiles,
			PBYTE						pbKey, 
			DWORD						cbKey,
			DWORD						cbLength,
			PBYTE						pbBuffer,
			DWORD						cbBuffer,
			PRC4_SHA_HEADER				pHeader,
			HANDLE						hEventWorkAvailable,
			PXONLINECONTENT_CFFILECACHE	pcffilecache
			)
{
	Assert(pcffilecache != NULL);
	Assert(hFile != INVALID_HANDLE_VALUE);
	Assert(hFile != NULL);
	Assert(wCffiles > 0);
	Assert(pbKey != NULL);
	Assert(cbKey > sizeof(DWORD));
	Assert(cbLength > 0);
	Assert(pHeader != NULL);

	pcffilecache->hFile = hFile;
	pcffilecache->uliReadOffset.QuadPart = dwCffileOffset;
	pcffilecache->wCffiles = wCffiles;
	pcffilecache->bFlags = CFFILECACHE_FLAG_DISCARD_PREDATA;
	pcffilecache->bPadding = 0;
	pcffilecache->wCurrentIndex = 0;
	pcffilecache->wCursor = 0;
	pcffilecache->wDataSize = 0;
	pcffilecache->hEventWork = hEventWorkAvailable;

	if (pbBuffer)
	{
		pcffilecache->pbBuffer = pbBuffer;
		pcffilecache->cbBuffer = cbBuffer;
	}
	else
	{
		pcffilecache->pbBuffer = pcffilecache->rgbBuffer;
		pcffilecache->cbBuffer = CFFILECACHE_SIZE;
	}

	// Initialize our RC4 decoder
	return(SymmdecInitialize(pbKey, cbKey, cbLength,
				pHeader, &pcffilecache->symmdec));
}

//
// Function to get the first/next CFFILE entry
//
HRESULT CXo::CffileCacheGetNextEntry(
			PXONLINECONTENT_CFFILECACHE	pcffilecache,
			DWORD						*pdwIndex,
			CFFILE						**ppcffile,
			PSTR						*ppszName,
			DWORD						*pcbName
			)
{
	HRESULT	hr = S_OK;
	
	Assert(pdwIndex != NULL);
	Assert(ppcffile != NULL);
	Assert(ppszName != NULL);
	Assert(pcbName != NULL);

	// Make sure there is more data to read
	if (pcffilecache->wCurrentIndex >= pcffilecache->wCffiles)
		return(S_CFFILECACHE_NO_MORE_ITEMS);

	// First, see if we are actually in a pending state waiting
	// more data.
	if (pcffilecache->bFlags & CFFILECACHE_FLAG_READ_PENDING)
	{
		DWORD				dwBytesRead;
		DWORD				dwRemaining;
		PBYTE				pbBuffer = NULL;
		XONLINETASK_HANDLE	hfileio = (XONLINETASK_HANDLE)&pcffilecache->fileio;
		
		// Continue until the receive completes asynchronously
		hr = XOnlineTaskContinue(hfileio);
		if (XONLINETASK_STATUS_AVAILABLE(hr))
		{
			// Mark as non-pending state
			pcffilecache->bFlags &= (~CFFILECACHE_FLAG_READ_PENDING);

			// Receive had returned results, see if it succeeded
			if (FAILED(hr))
				return(hr);

			// Succeeded, now get the results and analyze the data
			hr = GetReadWriteFileResults(hfileio, &dwBytesRead, &pbBuffer);
			if (FAILED(hr))
				return(hr);

			// If we read zero bytes, there is something wrong with the
			// data. Return failure
			if (dwBytesRead == 0)
				return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));

			// Now decrypt our data
			Assert(pbBuffer != NULL);

			// Adjust our pointers
			pcffilecache->uliReadOffset.QuadPart += dwBytesRead;

			// Now, we make sure if we are asked to discard the bytes
			// up to the data marker, we will do so, and not factor that
			// into valid data.
			if (pcffilecache->bFlags & CFFILECACHE_FLAG_DISCARD_PREDATA)
			{
				dwBytesRead -= pcffilecache->wDataSize;
				pcffilecache->bFlags &= (~CFFILECACHE_FLAG_DISCARD_PREDATA);
			}
			
			// Because of sector alignment, we could have read too
			// much. The decoder does not like that, so we make sure
			// we give it the exact size so the checksum matches up.
			hr = SymmdecGetRemainingCount(
						&pcffilecache->symmdec, &dwRemaining);
			if (FAILED(hr))
				return(hr);

			if (dwBytesRead > dwRemaining)
				dwBytesRead = dwRemaining;

#ifndef NO_ENCRYPT

			// Make sure we only decrypt what is new, and that we don't
			// overrun
			hr = SymmdecDecrypt(&pcffilecache->symmdec, 
						pcffilecache->pbBuffer + pcffilecache->wDataSize, 
						dwBytesRead);
			if (FAILED(hr))
				return(hr);
				
#endif				

			// Now reflect the real data size
			pcffilecache->wDataSize += (WORD)dwBytesRead;
		}
		else
		{
			// Return pending error
			return(S_CFFILECACHE_IO_PENDING);
		}
	}

	// See if there is any more data to process
	if (pcffilecache->wCursor < pcffilecache->wDataSize)
	{
		CFFILE	*pcffile = (CFFILE *)(pcffilecache->pbBuffer + pcffilecache->wCursor);
		WORD	wDataLeft = pcffilecache->wDataSize - pcffilecache->wCursor;
		
		// Make sure the data is at least the size of a CFFILE
		if (wDataLeft > sizeof(CFFILE))
		{
			PSTR	pszName = (PSTR)(pcffile + 1);
			
			// OK, we'll try to parse the remaining data for a
			// complete CFFILE record.
			wDataLeft -= sizeof(CFFILE);
			while (wDataLeft--)
				if (*pszName++ == '\0')
				{
					// We found a complete CFFILE record
					*pdwIndex = pcffilecache->wCurrentIndex++;
					*ppcffile = pcffile;
					*ppszName = (PSTR)(pcffile + 1);
					*pcbName = pszName - (*ppszName);

					// Advance the cursor
					pcffilecache->wCursor = pszName - (PSTR)pcffilecache->pbBuffer;

					return(S_OK);
				}
		}
	}

	// OK, since we're here, we are not pending for more data,
	// and the remaining data is incomplete. Let's read more 
	// data from the disk.
	{
		DWORD	dwDataSize;
		DWORD	dwReadSize;

		// Figure out how mauch data is left
		dwDataSize = pcffilecache->wDataSize - pcffilecache->wCursor;

		// Figure out how much padding we need for DWORD alignment
		pcffilecache->bPadding = (BYTE)(sizeof(DWORD) - (dwDataSize & (sizeof(DWORD) - 1)));
		pcffilecache->bPadding &= (sizeof(DWORD) - 1);

        Assert(((dwDataSize + pcffilecache->bPadding) & (sizeof(DWORD) - 1)) == 0);
		
		// First, move all unprocessed data to the front of the buffer
		if (dwDataSize)
		{
			MoveMemory(pcffilecache->pbBuffer + pcffilecache->bPadding, 
						pcffilecache->pbBuffer + pcffilecache->wCursor, dwDataSize);
		}
	
		// Reads need to be sector-aligned.
		dwDataSize += pcffilecache->bPadding;
		dwReadSize = pcffilecache->cbBuffer - dwDataSize;
		dwReadSize &= ~(XBOX_HD_SECTOR_SIZE - 1);

		// Move our pointers to the right places for new data
		pcffilecache->wCursor = pcffilecache->bPadding;
		pcffilecache->wDataSize = (WORD)dwDataSize;

		Assert((dwDataSize >> 16) == 0);

		if (dwReadSize == 0)
		{
			// The next CFFILE entry is so large that it does not leave
			// enough space for us to read the next sector. This means
			// that it is more than 1.5k. This is certainly invalid, and
			// since there is no way we can process this entry, so we fail.
			return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
		}

#if DBG
		if (dwReadSize < 2048)
		{
			TraceSz1(Verbose, "Buffer underutilized by %u bytes", 2048-dwReadSize);
		}
#endif

		// Final caveat, the initial file offset may not be aligned
		if (pcffilecache->bFlags & CFFILECACHE_FLAG_DISCARD_PREDATA)
		{
			DWORD	dwTemp;
			
			dwTemp = SECTOR_ALIGNMENT_ROUND_DOWN(
						pcffilecache->uliReadOffset.u.LowPart);
			pcffilecache->wCursor = (WORD)(pcffilecache->uliReadOffset.u.LowPart - dwTemp);
			pcffilecache->wDataSize = pcffilecache->wCursor;
			pcffilecache->uliReadOffset.u.LowPart = dwTemp;
			dwDataSize = 0;
		}

		// Set up the read file context
		ReadWriteFileInitializeContext(pcffilecache->hFile,
					pcffilecache->pbBuffer + dwDataSize,
					dwReadSize,
					pcffilecache->uliReadOffset,
					pcffilecache->hEventWork,
					&pcffilecache->fileio);

		hr = ReadFileInternal(&pcffilecache->fileio);
		if (FAILED(hr))
			return(hr);

		// Mark as pending state
		pcffilecache->bFlags |= CFFILECACHE_FLAG_READ_PENDING;
	}

	// Return IO pending
	return(S_CFFILECACHE_IO_PENDING);
}

//
// After reading all CFFILE entries, this must be called to
// verify the checksum
//
HRESULT CXo::CffileCacheVerifyChecksum(
			PXONLINECONTENT_CFFILECACHE	pcffilecache
			)
{
	// Make sure we have processed all CFFILE entries ...
	if (pcffilecache->wCurrentIndex < pcffilecache->wCffiles)
		return(HRESULT_FROM_WIN32(ERROR_MORE_DATA));

	// Just directly call the symmdec to verify the checksum
	return(SymmdecVerify(&pcffilecache->symmdec));
}

VOID CXo::CffileCacheDeinitialize(
			PXONLINECONTENT_CFFILECACHE	pcffilecache
			)
{
	if ((pcffilecache->hFile != INVALID_HANDLE_VALUE) && 
		(pcffilecache->hFile != NULL))
	{
		if (!CloseHandle(pcffilecache->hFile))
		{
			AssertSz(FALSE, "CffileCacheDeinitialize: cannot close handle!");
		}
		// Reset the handle
		pcffilecache->hFile = INVALID_HANDLE_VALUE;
	}
}

//
// Function to veriy the CFFILE entries before use
//
HRESULT CXo::CffileCacheVerify(
			PXONLINECONTENT_CFFILECACHE	pcffilecache
			)
{
	HRESULT	hr = S_OK;
	
	Assert(pcffilecache != NULL);

	// First, see if we are actually in a pending state waiting
	// more data.
	if (pcffilecache->bFlags & CFFILECACHE_FLAG_READ_PENDING)
	{
		DWORD				dwBytesRead;
		DWORD				dwRemaining;
		PBYTE				pbBuffer = NULL;
		XONLINETASK_HANDLE	hfileio = (XONLINETASK_HANDLE)&pcffilecache->fileio;
		
		// Continue until the receive completes asynchronously
		hr = XOnlineTaskContinue(hfileio);
		if (XONLINETASK_STATUS_AVAILABLE(hr))
		{
			// Mark as non-pending state
			pcffilecache->bFlags &= (~CFFILECACHE_FLAG_READ_PENDING);

			// Receive had returned results, see if it succeeded
			if (FAILED(hr))
				return(hr);

			// Succeeded, now get the results and analyze the data
			hr = GetReadWriteFileResults(hfileio, &dwBytesRead, &pbBuffer);
			if (FAILED(hr))
				return(hr);

			// If we read zero bytes, there is something wrong with the
			// data. Return failure
			if (dwBytesRead == 0)
				return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));

			// Now decrypt our data
			Assert(pbBuffer != NULL);

			// Adjust our pointers
			pcffilecache->uliReadOffset.QuadPart += dwBytesRead;

			// Now, we make sure if we are asked to discard the bytes
			// up to the data marker, we will do so, and not factor that
			// into valid data.
			if (pcffilecache->bFlags & CFFILECACHE_FLAG_DISCARD_PREDATA)
			{
				dwBytesRead -= pcffilecache->wDataSize;
				pcffilecache->bFlags &= (~CFFILECACHE_FLAG_DISCARD_PREDATA);
			}
			
			// Because of sector alignment, we could have read too
			// much. The decoder does not like that, so we make sure
			// we give it the exact size so the checksum matches up.
			hr = SymmdecGetRemainingCount(
						&pcffilecache->symmdec, &dwRemaining);
			if (FAILED(hr))
				return(hr);

			if (dwBytesRead > dwRemaining)
				dwBytesRead = dwRemaining;

#ifndef NO_ENCRYPT

			// Make sure we only decrypt what is new, and that we don't
			// overrun
			hr = SymmdecDecrypt(&pcffilecache->symmdec, 
						pcffilecache->pbBuffer + pcffilecache->wDataSize, 
						dwBytesRead);
			if (FAILED(hr))
				return(hr);
				
#endif				

			// Now reflect the real data size
			pcffilecache->wDataSize += (WORD)dwBytesRead;

			// See if we are done
			dwRemaining -= dwBytesRead;
			if (!dwRemaining)
			{
				// Verify the Hmac and leave
				return(SymmdecVerify(&pcffilecache->symmdec));
			}

			// We fall thru below ...
		}
		else
		{
			// Return pending error
			return(S_CFFILECACHE_IO_PENDING);
		}
	}

	// OK, since we're here, we are not pending for more data,
	// and the remaining data is incomplete. Let's read more 
	// data from the disk.
	pcffilecache->wCursor = 0;
	pcffilecache->wDataSize = 0;

	// Final caveat, the initial file offset may not be aligned
	if (pcffilecache->bFlags & CFFILECACHE_FLAG_DISCARD_PREDATA)
	{
		DWORD	dwTemp;
		
		dwTemp = SECTOR_ALIGNMENT_ROUND_DOWN(
					pcffilecache->uliReadOffset.u.LowPart);
		pcffilecache->wCursor = (WORD)(pcffilecache->uliReadOffset.u.LowPart - dwTemp);
		pcffilecache->wDataSize = pcffilecache->wCursor;
		pcffilecache->uliReadOffset.u.LowPart = dwTemp;
	}

	// Set up the read file context
	ReadWriteFileInitializeContext(pcffilecache->hFile,
				pcffilecache->pbBuffer,
				SECTOR_ALIGNMENT_ROUND_DOWN(pcffilecache->cbBuffer),
				pcffilecache->uliReadOffset,
				pcffilecache->hEventWork,
				&pcffilecache->fileio);

	hr = ReadFileInternal(&pcffilecache->fileio);
	if (FAILED(hr))
		return(hr);

	// Mark as pending state
	pcffilecache->bFlags |= CFFILECACHE_FLAG_READ_PENDING;

	// Return IO pending
	return(S_CFFILECACHE_IO_PENDING);
}
