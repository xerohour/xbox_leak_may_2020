/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing asynchronous directory operations

Module Name:

    dirops.c

--*/

#include "xonp.h"
#include "xonver.h"

//
// Helper functions
//

//
// Scans the path string backwards to find the parent dir.
//
// Note: *pdwIndex does not include the NULL terminator
//
HRESULT CXo::FindParentPath(
			PSTR		szPath,
			DWORD		*pdwIndex
			)
{
	DWORD	dwIndex = *pdwIndex;
	PSTR	pCurrent = szPath + dwIndex;

	// Make sure we have more data, we should never get here
	if (!dwIndex)
		return(E_FAIL);
		
	Assert(*pCurrent == '\0');

	// Walk backwards and find the next backslash
	do 
	{
		dwIndex--;
		pCurrent--;
		if (*pCurrent == '\\')
		{
			*pCurrent = '\0';
			*pdwIndex = dwIndex;
			return(S_OK);
		}

	} while (dwIndex);

	// This is an invalid path!
	return(E_FAIL);
}

//
// Scans the path string forward to restore the child path.
//
// Note: cbPath does not include the NULL terminator
//
HRESULT CXo::RestoreChildPath(
			PSTR		szPath,
			DWORD		cbPath,
			DWORD		*pdwIndex
			)
{
	DWORD	dwIndex = *pdwIndex;
	PSTR	pCurrent = szPath + dwIndex;

	// Make sure we're not at the end already
	if (dwIndex >= cbPath)
		return(E_FAIL);
		
	Assert(*pCurrent == '\0');

	// Restore the backslash
	*pCurrent = '\\';

	// Walk forward until we find a NULL terminator
	while (dwIndex < cbPath)
	{
		pCurrent++;
		dwIndex++;
		if (*pCurrent == '\0')
		{
			*pdwIndex = dwIndex;

			// See if this is the entire file path
			if (dwIndex == cbPath)
				return(S_FALSE);

			// This is just a directory
			return(S_OK);
		}
	}

	// This is an invalid path!
	return(E_FAIL);
}

//
// Function to take care of creating directories
//
HRESULT CXo::DoCreateDirectories(
			PXONLINETASK_DIROPS	pdirops
			)
{
	HRESULT						hr = S_OK;
	WIN32_FILE_ATTRIBUTE_DATA	fad;
	DWORD						dwError;

	Assert(pdirops != NULL);

	// See if we are in create mode, or still trying to find a directory
	// that exists
	if ((pdirops->dwFlags & XONLINETASK_CDFLAG_CREATE_MODE) == 0)
	{
		// Test if the this directory exists
		if (!GetFileAttributesEx(pdirops->szPath, GetFileExInfoStandard, &fad))
		{
			dwError = GetLastError();
			if ((dwError == ERROR_PATH_NOT_FOUND) ||
				(dwError == ERROR_FILE_NOT_FOUND))
			{
				// If we're at the front and still the directory does not exist,
				// then we are quire screwed ...
				if (!pdirops->dwIndex)
				{
					hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
					goto Error;
				}

				// Go back and get the parent
				hr = FindParentPath(pdirops->szPath, &pdirops->dwIndex);
				if (FAILED(hr))
					goto Error;

				// Keep searching
				goto Exit;
			}
			
			// If we are at the root, then start creating directories
			if ((strlen(pdirops->szPath) == 2) && (pdirops->szPath[1] == ':'))
				goto CreateDirectory;

			// Failed to get file attributes for some reason.
			hr = HRESULT_FROM_WIN32(dwError);
			goto Error;
		}

		// Make sure it's a directory
		if ((fad.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			hr = HRESULT_FROM_WIN32(ERROR_FILE_EXISTS);
			goto Error;
		}

CreateDirectory:

		// OK, this directory exists, set the flag to create subdirectories
		// from here
		pdirops->dwFlags |= XONLINETASK_CDFLAG_CREATE_MODE;
	}

	// See if we should be creating directories
	if ((pdirops->dwFlags & XONLINETASK_CDFLAG_CREATE_MODE) != 0)
	{
		// Find the child and recursively create directories
		hr = RestoreChildPath(pdirops->szPath, pdirops->cbPath, &pdirops->dwIndex);
		if (FAILED(hr))
			goto Error;

		if (hr == S_FALSE)
		{
			// We came back to the full oringinal path. Now, if the path
			// contains a file name, then we are done. Alternatively, if
			// the path is a directory, we will also create this dir.
			if (pdirops->dwFlags & XONLINETASK_CDFLAG_FILE_PATH)
			{
				// We are done
				hr = XONLINETASK_S_SUCCESS;
				goto Exit;
			}
			hr = S_OK;
		}

		// Create the directory
//		TraceSz1(Verbose, "Creating directory: %s", pdirops->szPath);
		if (!CreateDirectory(pdirops->szPath, NULL))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Error;
		}

		if (hr == S_FALSE)
		{
			// We are done
			hr = XONLINETASK_S_SUCCESS;
		}
	}

Exit:
	pdirops->hr = hr;
	return(hr);

Error:
	// Error, abort the task.
	goto Exit;
}

//
// Helper function to determine if a file/dir is in a skip list
//
BOOL CXo::IsInSkipList(
			PXONLINEDIROPS_SKIP_LIST	pSkipList,
			PSTR						szPath
			)
{
	for (DWORD i = 0; i < pSkipList->dwEntries; i++)
		if (_stricmp(szPath, pSkipList->rgszSkipPath[i]) == 0)
			return(TRUE);
	return(FALSE);
}

//
// Function to take care of recursively removing a directory
//
HRESULT CXo::DoRemoveDirectory(
			PXONLINETASK_DIROPS	pdirops
			)
{
	HRESULT			hr = S_OK;
	WIN32_FIND_DATA	wfd;
	DWORD			dwError;

	Assert(pdirops != NULL);

	// If we are popping from a parent, we will try to delete the parent
	// directories until we hit a brick wall
	if ((pdirops->dwFlags & XONLINETASK_RDFLAG_POPDIR) != 0)
	{
//		TraceSz1(Verbose, "Removing directory %s\n", pdirops->szPath);
		if (!RemoveDirectory(pdirops->szPath))
		{
			dwError = GetLastError();
			if (dwError == ERROR_DIR_NOT_EMPTY)
			{
				// Well, this directory is not empty, we will
				// restart enumeration from here.
				pdirops->szPath[pdirops->cbCurrent++] = '\\';
				pdirops->szPath[pdirops->cbCurrent] = '\0';
				pdirops->dwFlags &= (~XONLINETASK_RDFLAG_POPDIR);
				goto Exit;
			}
			else if ((dwError != ERROR_PATH_NOT_FOUND) &&
				(dwError != ERROR_FILE_NOT_FOUND))
			{
				hr = HRESULT_FROM_WIN32(dwError);
				goto Error;
			}
		}

		// See if we are done. First we add back the trailing backslah, and if
		// this length is equal to the path length, then we are done.
		if ((pdirops->cbCurrent + 1) == pdirops->cbPath)
		{
			// Yup, set the don't continue flag
			hr = XONLINETASK_S_SUCCESS;
			goto Exit;
		}

		// OK, now revert to parent path
		hr = FindParentPath(pdirops->szPath, &(pdirops->cbCurrent));
		if (FAILED(hr))
			goto Error;

		goto Exit;
	}

	if (pdirops->hFind == INVALID_HANDLE_VALUE)
	{
		// Start off the find file operation
		strcpy(pdirops->szPath + pdirops->cbCurrent, "*.*");
		
		pdirops->hFind = FindFirstFile(pdirops->szPath, &wfd);
		pdirops->szPath[pdirops->cbCurrent] = '\0';
		
		if (pdirops->hFind == INVALID_HANDLE_VALUE)
		{
			dwError = GetLastError();
			if ((dwError == ERROR_PATH_NOT_FOUND) ||
				(dwError == ERROR_FILE_NOT_FOUND))
			{
				// No files are found. Enter pop mode to delete the current
				// directory.
				pdirops->szPath[--pdirops->cbCurrent] = '\0';
				pdirops->dwFlags |= XONLINETASK_RDFLAG_POPDIR;
				goto Exit;
			}

			// Some other error
			hr = HRESULT_FROM_WIN32(dwError);
			goto Error;
		}
	}
	else
	{
		// Continue ...
		if (!FindNextFile(pdirops->hFind, &wfd))
		{
			DWORD	dwError = GetLastError();
			if (dwError == ERROR_NO_MORE_FILES)
			{
				// Close the find handle properly
				FindClose(pdirops->hFind);
				pdirops->hFind = INVALID_HANDLE_VALUE;

				// No more items, see if we have to recurse
				// to a subdirectory
				if (pdirops->szDir[0] != '\0')
				{
					// Need to recurse, concatenate the directory
					if ((pdirops->cbCurrent + pdirops->cbDir + 1) >= pdirops->cbMax)
					{
						hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
						goto Error;
					}

					// Build the child directory path
					strcpy(pdirops->szPath + pdirops->cbCurrent, pdirops->szDir);
					pdirops->cbCurrent += pdirops->cbDir;
					pdirops->szPath[(pdirops->cbCurrent)++] = '\\';
					pdirops->szPath[pdirops->cbCurrent] = '\0';
					pdirops->szDir[0] = '\0';
					pdirops->cbDir = 0;
					goto Exit;
				}
				else
				{
					// Alright, we are done with this directory, delete it
					pdirops->szPath[--(pdirops->cbCurrent)] = '\0';

					// Set the state to popping
					pdirops->dwFlags |= XONLINETASK_RDFLAG_POPDIR;
					goto Exit;
				}
			}
			
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Error;
		}
	}

	// Special processing for directories
	if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		// Save the first directory we encounter so we know where
		// to recurse later
		if (pdirops->szDir[0] == '\0')
		{
			pdirops->cbDir = strlen(wfd.cFileName);
			if ((pdirops->cbDir + pdirops->cbCurrent) >= pdirops->cbMax)
			{
				hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
				goto Error;
			}

			// Create the full path to see if it is in the skip list
			strcpy(pdirops->szDir, pdirops->szPath);
			pdirops->szDir[pdirops->cbCurrent] = '\\';
			strcpy(pdirops->szDir + (pdirops->cbCurrent + 1), wfd.cFileName);

			// Accept or reject depending on whether the directory is in 
			// the skip list
			if (IsInSkipList(&pdirops->DirsToSkip, pdirops->szDir))
				pdirops->szDir[0] = '\0';
			else
				strcpy(pdirops->szDir, wfd.cFileName);
		}

		// Leave
		goto Exit;
	}

	// Build the file path
	if ((pdirops->cbCurrent + strlen(wfd.cFileName)) >= pdirops->cbMax)
	{
		// Name too long!
		hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
		goto Error;
	}

	// Delete the file if it's not in our skip list
	strcpy(pdirops->szPath + pdirops->cbCurrent, wfd.cFileName);
	if (!IsInSkipList(&pdirops->FilesToSkip, pdirops->szPath))
	{
		if (!DeleteFile(pdirops->szPath))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Error;
		}
	}

	// Restore the path
	pdirops->szPath[pdirops->cbCurrent] = '\0';

Exit:
	pdirops->hr = hr;
	return(hr);

Error:
	// Error, abort the task.
	goto Exit;
}

//
// Function to take care of enumerating the contents of a directory
//
HRESULT CXo::DoEnumerateDirectory(
			PXONLINETASK_DIROPS	pdirops
			)
{
	HRESULT			hr = S_OK;
	WIN32_FIND_DATA	wfd;
	BOOL			fDir;
	DWORD			dwWorkFlags;
	DWORD			dwError;

	Assert(pdirops != NULL);

	// There are only two states: Find the next match, or pump a
	// caller-supplied subtask until it is done.
	if ((pdirops->dwFlags & XONLINETASK_EDFLAG_SUBTASK) == 0)
	{
		// We need to get the first/next match
		if (pdirops->hFind == INVALID_HANDLE_VALUE)
		{
			pdirops->hFind = FindFirstFile(pdirops->szPath, &wfd);
			if (pdirops->hFind == INVALID_HANDLE_VALUE)
			{
				dwError = GetLastError();
				if ((dwError == ERROR_FILE_NOT_FOUND) ||
					(dwError == ERROR_PATH_NOT_FOUND))
				{
					// We are done enumerating.
					hr = XONLINETASK_S_SUCCESS;
					goto NoMoreFiles;
				}
				
				hr = HRESULT_FROM_WIN32(dwError);
				goto Error;
			}
		}
		else if (!FindNextFile(pdirops->hFind, &wfd))
		{
			dwError = GetLastError();
			if (dwError == ERROR_NO_MORE_FILES)
			{
				// We are done enumerating.
				hr = XONLINETASK_S_SUCCESS;
				goto NoMoreFiles;
			}
			
			hr = HRESULT_FROM_WIN32(dwError);
			goto Error;
		}

		// Do simple filetering based on user flags
		fDir = ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)?TRUE:FALSE;
		if ((fDir && ((pdirops->dwFlags & XONLINETASK_EDFLAG_ENUMERATE_DIRS) != 0)) ||
			(!fDir && ((pdirops->dwFlags & XONLINETASK_EDFLAG_ENUMERATE_FILES) != 0)))
		{
			// Got a match, proceed to subtask
			Assert(pdirops->hTaskOnData != NULL);

			// Save a copy of the filename in the space provided
			pdirops->cbPath = strlen(wfd.cFileName);
			if (pdirops->cbPath > pdirops->cbMax)
			{
				hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
				goto Error;
			}
			strcpy(pdirops->szPath, wfd.cFileName);

			pdirops->dwFlags |= XONLINETASK_EDFLAG_SUBTASK;
		}
	}
	else
	{
		// We are in the subtask, just keep pumping until the subtask
		// finishes, and get the next item
		hr = XOnlineTaskContinue(pdirops->hTaskOnData);
		if (XONLINETASK_STATUS_AVAILABLE(hr))
		{
			// Bail if the subtask indicated failure
			if (FAILED(hr))
				goto Error;

			// Move on if we are done, otherwise, let the result
			// code percolate up
			pdirops->dwFlags &= (~XONLINETASK_EDFLAG_SUBTASK);
			if (XONLINETASK_STATUS_SUCCESSFUL_COMPLETION(hr))
				hr = S_OK;
		}
	}

Exit:
	return(hr);

Error:
	// Error, abort the task.
	goto Exit;

NoMoreFiles:
	// No more files, see if we have an OnDone subtask
	if (pdirops->hTaskOnDone)
	{
		hr = XOnlineTaskContinue(pdirops->hTaskOnDone);
		if (XONLINETASK_STATUS_AVAILABLE(hr))
		{
			if (FAILED(hr))
				goto Error;

			Assert(hr == XONLINETASK_S_SUCCESS);
			hr = XONLINETASK_S_SUCCESS;
		}
	}
	else
		hr = XONLINETASK_S_SUCCESS;
		
	goto Exit;
}

//
// Main functions
//

//
// Cleanup function
//
VOID CXo::DiropsCleanup(
			PXONLINETASK_DIROPS	pdirops
			)
{
	if (pdirops->hFind != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pdirops->hFind);
		pdirops->hFind = INVALID_HANDLE_VALUE;
	}
}

//
// Main do work function
//
HRESULT CXo::DiropsContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT				hr = S_OK;
	PXONLINETASK_DIROPS	pdirops = (PXONLINETASK_DIROPS)hTask;

	Assert(hTask != NULL);

	// Always signal the event. If we actually do any async work, we 
	// will reset the event. This makes sure we don't stall
	if (pdirops->xontask.hEventWorkAvailable)
		SetEvent(pdirops->xontask.hEventWorkAvailable);

	// Call the function to take care of this
	switch (pdirops->dirop)
	{
	case diropCreateDirectory:
		hr = DoCreateDirectories(pdirops);
		break;

	case diropRemoveDirectory:
		hr = DoRemoveDirectory(pdirops);
		break;

	case diropEnumerateDirectory:
		hr = DoEnumerateDirectory(pdirops);
		break;
	}
		
	pdirops->hr = hr;
	return(hr);
}

//
// Function to close a dirops context
//
VOID CXo::DiropsClose(
			XONLINETASK_HANDLE	hTask
			)
{
	PXONLINETASK_DIROPS	pdirops = (PXONLINETASK_DIROPS)hTask;
	
	DiropsCleanup(pdirops);
	SysFree(pdirops);
}

//
// Function to initialize a dirops context
//
HRESULT CXo::DiropsInitializeContext(
			XONLINEDIROPS_OPS	dirop,
			PSTR				szPath,
			DWORD				cbPath,
			DWORD				cbMax,
			PSTR				szDir,
			DWORD				dwFlags,
			XONLINETASK_HANDLE	hTaskOnData,
			XONLINETASK_HANDLE	hTaskOnDone,
			HANDLE				hWorkEvent,
			PXONLINETASK_DIROPS	pdirops
			)
{
	HRESULT	hr = S_OK;
	DWORD	i;

	Assert(dirop < diropMaxOp);
	Assert(pdirops != NULL);
	Assert(szPath != NULL);
	Assert(cbPath != 0);
	Assert(szPath[cbPath] == '\0');

	// Fill in the blanks
	ZeroMemory(pdirops, sizeof(XONLINETASK_DIROPS));
	pdirops->hr = S_OK;
	pdirops->dirop = dirop;
	pdirops->szPath = szPath;
	pdirops->cbPath = cbPath;
	pdirops->cbCurrent = cbPath;
	pdirops->cbMax = cbMax;
	pdirops->dwIndex = cbPath;
	pdirops->szDir = szDir;
	pdirops->dwFlags = dwFlags & XONLINETASK_CDFLAGS_USER;
	pdirops->hFind = INVALID_HANDLE_VALUE;
	pdirops->hTaskOnData = hTaskOnData;
	pdirops->hTaskOnDone = hTaskOnDone;

	TaskInitializeContext(&pdirops->xontask);
	pdirops->xontask.pfnContinue = DiropsContinue;
	pdirops->xontask.pfnClose = DiropsClose;
	pdirops->xontask.hEventWorkAvailable = hWorkEvent;
	if (hWorkEvent)
		SetEvent(hWorkEvent);

	// See if we need to do special work here
	switch (pdirops->dirop)
	{
	case diropCreateDirectory:

		// Validate user flags
        Assert((dwFlags & (~XONLINETASK_CDFLAG_FILE_PATH)) == 0);
		
		if (pdirops->dwFlags & XONLINETASK_CDFLAG_FILE_PATH)
		{
			// It's a file, so chop off the last part		
			hr = FindParentPath(pdirops->szPath, &pdirops->dwIndex);
			if (FAILED(hr))
				return(hr);
		}
		break;

	case diropRemoveDirectory:

		// Validate user flags
		Assert(dwFlags == 0);
		Assert(cbPath > 0);

		// Add trailiing backslash if necessary
		if (szPath[cbPath - 1] != '\\')
		{
			strcpy(szPath + pdirops->cbCurrent, "\\");
			pdirops->cbCurrent++;
			pdirops->cbPath++;
		}

		// Clear up the skip lists. This will have to be explicitly set
		// later on
		pdirops->DirsToSkip.dwEntries = 0;
		pdirops->FilesToSkip.dwEntries = 0;
		break;

	case diropEnumerateDirectory:

		// We need hTaskOnData for enumerate
        Assert(hTaskOnData != NULL);
		
		// Validate user flags
        Assert((dwFlags & (~XONLINETASK_EDFLAG_ENUMERATE_ALL)) == 0);
        Assert((dwFlags & (XONLINETASK_EDFLAG_ENUMERATE_ALL)) != 0);
		break;
	}

	return(S_OK);
}

//
// Implementation of directory crawler
//
typedef enum
{
	dircrawlCrawlDir = 0,
	dircrawlOnDir,
	dircrawlOnFile,
	dircrawlCleanup,
	dircrawlDone

} XONLINEDIRCRAWL_PHASE;

//
// Implement handlers for each state
//
HRESULT CXo::dircrawlCrawlDirHandler(
			PXONLINETASK_DIRCRAWL	pdircrawl
			)
{
	HRESULT			hr = S_OK;
	DWORD			dwError;
	DWORD			dwDepth;
	DWORD			cbSize;

	Assert(pdircrawl != NULL);

	// If we don't have an active find handle for the current search
	// depth, establish one
	dwDepth = pdircrawl->dwDepth;
	if (pdircrawl->rghFind[dwDepth] == INVALID_HANDLE_VALUE)
	{
		// Start off the find file operation
		strcpy(pdircrawl->szPath + pdircrawl->cbPath, "*.*");
		
		pdircrawl->rghFind[dwDepth] = 
					FindFirstFile(pdircrawl->szPath, &pdircrawl->wfd);
		pdircrawl->szPath[pdircrawl->cbPath] = '\0';
		
		if (pdircrawl->rghFind[dwDepth] == INVALID_HANDLE_VALUE)
		{
			dwError = GetLastError();
			if ((dwError == ERROR_PATH_NOT_FOUND) ||
				(dwError == ERROR_FILE_NOT_FOUND))
				goto NoMoreFiles;

			// Some other error
			hr = HRESULT_FROM_WIN32(dwError);
			goto Error;
		}
	}
	else
	{
		// Continue ...
		if (!FindNextFile(pdircrawl->rghFind[dwDepth], &pdircrawl->wfd))
		{
			dwError = GetLastError();
			if (dwError == ERROR_NO_MORE_FILES)
			{
				// Close the find handle properly
				FindClose(pdircrawl->rghFind[dwDepth]);
				pdircrawl->rghFind[dwDepth] = INVALID_HANDLE_VALUE;

NoMoreFiles:

				// Done with this directory, pop back to parent
				if (!dwDepth)
				{
					// We are already at the root node, we're done
					pdircrawl->dwState = dircrawlCleanup;
				}
				else
				{
					// Pop to parent, and fix up the paths
					pdircrawl->dwDepth = --dwDepth;

					// Chomp off the triailing backslash
					pdircrawl->szPath[--(pdircrawl->cbPath)] = '\0';
					hr = FindParentPath(pdircrawl->szPath, &(pdircrawl->cbPath));
					if (FAILED(hr))
						goto Error;

					// Add back the trailing backslash
					pdircrawl->szPath[(pdircrawl->cbPath)++] = '\\';
					pdircrawl->szPath[pdircrawl->cbPath] = '\0';

					if (pdircrawl->szTarget)
					{
						// Chomp off the triailing backslash
						pdircrawl->szTarget[--(pdircrawl->cbTarget)] = '\0';
						hr = FindParentPath(pdircrawl->szTarget, &(pdircrawl->cbTarget));
						if (FAILED(hr))
							goto Error;

						// Add back the trailing backslash
						pdircrawl->szTarget[(pdircrawl->cbTarget)++] = '\\';
						pdircrawl->szTarget[pdircrawl->cbTarget] = '\0';
					}
				}

				goto Exit;
			}

			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Error;
		}
	}

	// We found an entry, make sure we don't overrun the buffer
	cbSize = strlen(pdircrawl->wfd.cFileName);
    Assert((pdircrawl->cbPath + cbSize) < pdircrawl->cbMax);

	// See if this is a directory
	if (pdircrawl->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		// This is a directory, so we will recurse into this directory
		// but make sure we are not overrunning the depth limitation
        Assert(dwDepth < (MAX_CRAWL_DEPTH - 1));

		// Compute the recurse path
		strcpy(pdircrawl->szPath + pdircrawl->cbPath, pdircrawl->wfd.cFileName);
		pdircrawl->cbPath += cbSize;
		pdircrawl->szPath[(pdircrawl->cbPath)++] = '\\';
		pdircrawl->szPath[pdircrawl->cbPath] = '\0';

		// Maintain the optional target path if the caller specified one
		if (pdircrawl->szTarget)
		{
            Assert((pdircrawl->cbTarget + cbSize) < pdircrawl->cbMax);
			strcpy(pdircrawl->szTarget + pdircrawl->cbTarget, pdircrawl->wfd.cFileName);
			pdircrawl->cbTarget += cbSize;
			pdircrawl->szTarget[(pdircrawl->cbTarget)++] = '\\';
			pdircrawl->szTarget[pdircrawl->cbTarget] = '\0';
		}

		// Increase the depth
		pdircrawl->dwDepth = ++dwDepth;

		// If there is a per-directory task installed, then go and process
		// this directory record
		if (pdircrawl->hTaskOnDir)
		{
			pdircrawl->dwState = dircrawlOnDir;
		}

		// Leave
		goto Exit;
	}

	// If there is a per-file task installed, then go and process
	// this file record
	if (pdircrawl->hTaskOnFile)
	{
		pdircrawl->dwState = dircrawlOnFile;
	}

Exit:
	return(S_OK);

Error:
	// Error, abort the task.
	pdircrawl->hr = hr;
	return(hr);
}
	
HRESULT CXo::dircrawlOnDirHandler(
			PXONLINETASK_DIRCRAWL	pdircrawl
			)
{
	HRESULT	hr = S_OK;

	Assert(pdircrawl->hTaskOnDir != NULL);

	// We won't be here unless we have an on-directory task. So
	// we keep pumping until the task completes
	hr = XOnlineTaskContinue(pdircrawl->hTaskOnDir);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// See if it succeeded
		if (FAILED(hr))
			goto Error;

		// Succeeded. Now continue to crawl
		pdircrawl->dwState = dircrawlCrawlDir;
	}		

	return(S_OK);

Error:
	// Error, abort the task.
	pdircrawl->hr = hr;
	return(hr);
}

HRESULT CXo::dircrawlOnFileHandler(
			PXONLINETASK_DIRCRAWL	pdircrawl
			)
{
	HRESULT	hr = S_OK;

	Assert(pdircrawl->hTaskOnFile != NULL);

	// We won't be here unless we have an on-file task. So
	// we keep pumping until the task completes
	hr = XOnlineTaskContinue(pdircrawl->hTaskOnFile);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// See if it succeeded
		if (FAILED(hr))
			goto Error;

		// Succeeded. Now continue to crawl
		pdircrawl->dwState = dircrawlCrawlDir;
	}		

	return(S_OK);

Error:
	// Error, abort the task.
	pdircrawl->hr = hr;
	return(hr);
}

//
// Cleanup function
//
VOID CXo::DircrawlCleanup(
			PXONLINETASK_DIRCRAWL	pdircrawl
			)
{
	for (DWORD i = 0; i < pdircrawl->dwDepth; i++)
		if (pdircrawl->rghFind[i] != INVALID_HANDLE_VALUE)
		{
			CloseHandle(pdircrawl->rghFind[i]);
			pdircrawl->rghFind[i] = INVALID_HANDLE_VALUE;
		}
}

//
// Cleanup Handler
//
HRESULT CXo::dircrawlCleanupHandler(
			PXONLINETASK_DIRCRAWL	pdircrawl
			)
{
	DircrawlCleanup(pdircrawl);
	pdircrawl->dwState = dircrawlDone;
	return(S_OK);
}

//
// Define the handler function type
//

//
// Define the handler array
//
const PFNDIRCRAWL_HANDLER CXo::s_rgpfndircrawlHandlers[] =
{
    CXo::dircrawlCrawlDirHandler,
    CXo::dircrawlOnDirHandler,
    CXo::dircrawlOnFileHandler,
    CXo::dircrawlCleanupHandler
};
			
//
// Main do work function
//
HRESULT CXo::DircrawlContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT					hr = S_OK;
	PXONLINETASK_DIRCRAWL	pdircrawl = (PXONLINETASK_DIRCRAWL)hTask;

	Assert(hTask != NULL);

	// Always signal the event. If we actually do any async work, we 
	// will reset the event. This makes sure we don't stall
	if (pdircrawl->xontask.hEventWorkAvailable)
		SetEvent(pdircrawl->xontask.hEventWorkAvailable);

	// Call the function to take care of this
	if (pdircrawl->dwState < dircrawlDone)
	{
		// Call our handler to do more work
		hr = (this->*s_rgpfndircrawlHandlers[pdircrawl->dwState])(pdircrawl);
	}
	else if (pdircrawl->dwState != dircrawlDone)
	{
        AssertSz(FALSE, "DircrawlContinue: Bad state");
		pdircrawl->dwState = dircrawlDone;
		pdircrawl->hr = E_NOTIMPL;
	}

	// Make sure we exit the state machine if we're done
	if (pdircrawl->dwState == dircrawlDone)
	{
		hr = pdircrawl->hr;
		XONLINETASK_SET_COMPLETION_STATUS(hr);
		goto Exit;
	}
	
Exit:
	return(hr);
}

//
// Function to close a dircrawl
//
VOID CXo::DircrawlClose(
			XONLINETASK_HANDLE	hTask
			)
{
	PXONLINETASK_DIRCRAWL	pdircrawl = (PXONLINETASK_DIRCRAWL)hTask;

	DircrawlCleanup(pdircrawl);		
	SysFree(pdircrawl);
}		

//
// Function to initialize a directory crawl context. 
//
// Note: Both source and target paths must be directory paths with
// a trailing backslash '\\'. The source and target path buffers
// must remain valid throughout the copy process.
//
HRESULT CXo::DircrawlInitializeContext(
			PSTR					szPath,
			PSTR					szTarget,
			DWORD					cbMaxPath,
			DWORD					dwFlags,
			XONLINETASK_HANDLE		hTaskOnDir,
			XONLINETASK_HANDLE		hTaskOnFile,
			HANDLE					hWorkEvent,
			PXONLINETASK_DIRCRAWL	pdircrawl
			)
{
	HRESULT	hr = S_OK;
	DWORD	cbPath;
	DWORD	i;

	Assert(pdircrawl != NULL);
	Assert(szPath != NULL);

	ZeroMemory(pdircrawl, sizeof(XONLINETASK_DIRCRAWL));
	
	cbPath = strlen(szPath);
    Assert(cbPath && (cbPath < cbMaxPath) && (szPath[cbPath-1] == '\\'));
	pdircrawl->cbPath = cbPath;
	pdircrawl->szPath = szPath;
	
	if (szTarget)
	{
		cbPath = strlen(szTarget);
        Assert(cbPath && (cbPath < cbMaxPath) && (szTarget[cbPath-1] =='\\'));
		pdircrawl->cbTarget = cbPath;
		pdircrawl->szTarget = szTarget;
	}

	// Fill in the blanks
	pdircrawl->hr = S_OK;
	pdircrawl->cbMax = cbMaxPath;
	pdircrawl->dwState = dircrawlCrawlDir;
	pdircrawl->dwFlags = dwFlags;
	pdircrawl->hTaskOnDir = hTaskOnDir;
	pdircrawl->hTaskOnFile = hTaskOnFile;

	for (i = 0; i < MAX_CRAWL_DEPTH; i++)
		pdircrawl->rghFind[i] = INVALID_HANDLE_VALUE;
	pdircrawl->dwDepth = 0;

	TaskInitializeContext(&pdircrawl->xontask);
	pdircrawl->xontask.pfnContinue = DircrawlContinue;
	pdircrawl->xontask.pfnClose = DircrawlClose;
	pdircrawl->xontask.hEventWorkAvailable = hWorkEvent;
	if (hWorkEvent)
		SetEvent(hWorkEvent);

	return(S_OK);
}

//
// Simple per-directory task handler to do a directory copy
//
HRESULT CXo::DirCopyDirContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	PXONLINETASK_DIRCOPY	pdircopy;

	Assert(hTask != NULL);

	// Locate the containing object
	pdircopy = CONTAINING_RECORD(hTask, XONLINETASK_DIRCOPY, xontaskOnDir);

	// All we do is create the directory
	if (!CreateDirectory(pdircopy->dircrawl.szTarget, NULL))
		return(HRESULT_FROM_WIN32(GetLastError()));

	return(XONLINETASK_S_SUCCESS);
}

//
// Simple per-file task handler to do a directory copy
//
HRESULT CXo::DirCopyFileContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT					hr = XONLINETASK_S_SUCCESS;
	PXONLINETASK_DIRCOPY	pdircopy;
	PXONLINETASK_DIRCRAWL	pdircrawl;
	ULARGE_INTEGER			uliSize;
	DWORD					cbSize;
	DWORD					cbTarget;

	Assert(hTask != NULL);

	// Locate the containing object
	pdircopy = CONTAINING_RECORD(hTask, XONLINETASK_DIRCOPY, xontaskOnFile);
	pdircrawl = &pdircopy->dircrawl;

	// Build the source and target paths for copy, making sure not to overrun
	// the buffers
	pdircrawl = &pdircopy->dircrawl;
	cbSize = strlen(pdircrawl->wfd.cFileName);
	Assert(pdircrawl->szTarget != NULL);
    Assert(pdircrawl->cbPath + cbSize < pdircrawl->cbMax);
    Assert(pdircrawl->cbTarget + cbSize < pdircrawl->cbMax);

	// Build the fill copy paths
	cbSize = pdircrawl->cbPath;
	cbTarget = pdircrawl->cbTarget;
	strcpy(pdircrawl->szPath + cbSize, pdircrawl->wfd.cFileName);
	strcpy(pdircrawl->szTarget + cbTarget, pdircrawl->wfd.cFileName);

	// All we do is create the directory
	if (!CopyFile(pdircrawl->szPath, pdircrawl->szTarget, 
				(pdircrawl->dwFlags & XONLINETASK_DCFLAG_NOREPLACE)?TRUE:FALSE))
	{
		DWORD dwError = GetLastError();

		if (!(pdircrawl->dwFlags & XONLINETASK_DCFLAG_NOREPLACE) ||
			(dwError != ERROR_ALREADY_EXISTS))
			hr = HRESULT_FROM_WIN32(dwError);
	}
	else
	{
		// Update the statistics
		(pdircopy->dwFilesCopied)++;
		uliSize.u.LowPart = pdircrawl->wfd.nFileSizeLow;
		uliSize.u.HighPart = pdircrawl->wfd.nFileSizeHigh;
		pdircopy->uliBytesCopied.QuadPart += uliSize.QuadPart;
	}

	// Restore the path strings
	pdircrawl->cbPath = cbSize;
	pdircrawl->szPath[cbSize] = '\0';
	pdircrawl->cbTarget = cbTarget;
	pdircrawl->szTarget[cbTarget] = '\0';

	return(hr);
}

//
// Function to set up a context for a directory copy
//
// Note: Both source and target paths must be directory paths with
// a trailing backslash '\\'. The source and target path buffers
// must remain valid throughout the copy process.
//
HRESULT CXo::CopyDirectoryInitializeContext(
			PSTR					szSourceDirectory,
			PSTR					szTargetDirectory,
			DWORD					cbMaxPath,
			DWORD					dwFlags,
			HANDLE					hWorkEvent,
			PXONLINETASK_DIRCOPY	pdircopy
			)
{
	HRESULT	hr = S_OK;

	// Set up the subtasks
	TaskInitializeContext(&pdircopy->xontaskOnDir);
	pdircopy->xontaskOnDir.pfnContinue = DirCopyDirContinue;
	pdircopy->xontaskOnDir.hEventWorkAvailable = hWorkEvent;
	
	TaskInitializeContext(&pdircopy->xontaskOnFile);
	pdircopy->xontaskOnFile.pfnContinue = DirCopyFileContinue;
	pdircopy->xontaskOnFile.hEventWorkAvailable = hWorkEvent;

	// Reset the counters
	pdircopy->dwFilesCopied = 0;
	pdircopy->uliBytesCopied.QuadPart = 0;

	// Set up the crawl context
	hr = DircrawlInitializeContext(
				szSourceDirectory,
				szTargetDirectory,
				cbMaxPath,
				dwFlags,
				(XONLINETASK_HANDLE)&pdircopy->xontaskOnDir,
				(XONLINETASK_HANDLE)&pdircopy->xontaskOnFile,
				hWorkEvent,
				&pdircopy->dircrawl);
	return(hr);
}

// =====================================================================
// External APIs
//

//
// Function to kick off create directory
//
HRESULT CXo::DiropsCreateDirectory(
			PSTR				szDirectoryPath,
			DWORD				dwFlags,
			HANDLE				hWorkEvent,
			XONLINETASK_HANDLE	*phTask
			)
{
	HRESULT				hr = S_OK;
	PXONLINETASK_DIROPS	pdirops = NULL;
	PSTR				szPath = NULL;
	DWORD				cbPath;

	Assert(phTask != NULL);
	Assert(szDirectoryPath != NULL);

	cbPath = strlen(szDirectoryPath);

	// Allocate the task context, tack on the directory path
	// at the end
	pdirops = (PXONLINETASK_DIROPS)SysAlloc(sizeof(XONLINETASK_DIROPS) + cbPath + 1, PTAG_XONLINETASK_DIROPS);
	if (!pdirops)
		return(E_OUTOFMEMORY);

	// Set up the directory buffer
	szPath = (PSTR)(pdirops + 1);
	strcpy(szPath, szDirectoryPath);

	// Initialize the context
	hr = DiropsInitializeContext(
				diropCreateDirectory,
				szPath,
				cbPath,
				0,
				NULL,
				dwFlags,
				NULL,
				NULL,
				hWorkEvent, 
				pdirops);
	if (FAILED(hr))
		goto Error;

	// Return the opaque handle
	*phTask = (XONLINETASK_HANDLE)pdirops;

Cleanup:
	return(hr);

Error:
	if (pdirops)
	{
		SysFree(pdirops);
	}
	goto Cleanup;
}

//
// Function to kick off remove directory
//
HRESULT CXo::DiropsRemoveDirectory(
			PSTR				szDirectoryPath,
			DWORD				cbMaxPathSize,
			DWORD				dwFlags,
			HANDLE				hWorkEvent,
			XONLINETASK_HANDLE	*phTask
			)
{
	HRESULT				hr = S_OK;
	PXONLINETASK_DIROPS	pdirops = NULL;
	PSTR				szPath = NULL;
	PSTR				szDir = NULL;
	DWORD				cbPath;

	Assert(phTask != NULL);
	Assert(szDirectoryPath != NULL);

	cbPath = strlen(szDirectoryPath);
	if (cbMaxPathSize <= cbPath)
		return(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));

	// Allocate the task context, tack on the directory path
	// at the end
	pdirops = (PXONLINETASK_DIROPS)SysAlloc(sizeof(XONLINETASK_DIROPS) + (cbMaxPathSize * 2), PTAG_XONLINETASK_DIROPS);
	if (!pdirops)
		return(E_OUTOFMEMORY);

	// Set up the directory buffer
	szPath = (PSTR)(pdirops + 1);
	strcpy(szPath, szDirectoryPath);
	szDir = szPath + cbMaxPathSize;

	// Initialize the context
	hr = DiropsInitializeContext(
				diropRemoveDirectory,
				szPath,
				cbPath,
				cbMaxPathSize,
				szDir,
				dwFlags,
				NULL,
				NULL,
				hWorkEvent, 
				pdirops);
	if (FAILED(hr))
		goto Error;

	// Return the opaque handle
	*phTask = (XONLINETASK_HANDLE)pdirops;

Cleanup:
	return(hr);

Error:
	if (pdirops)
	{
		SysFree(pdirops);
	}
	goto Cleanup;
}


