//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\linkedstr.h"
#include "..\tncommon\strutils.h"
#include "..\tncommon\fileutils.h"
#include "..\tncommon\symbols.h"

#include "tncontrl.h"
#include "main.h"

#include "binmgmt.h"





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNBinary::CTNBinary()"
//==================================================================================
// CTNBinary constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNBinary object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNBinary::CTNBinary(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNBinary));

	this->m_pszPath = NULL;
	this->m_fDebug = FALSE;
	this->m_dwMS = 0;
	this->m_dwLS = 0;
	ZeroMemory(&(this->m_createtime), sizeof (FILETIME));
	ZeroMemory(&(this->m_writetime), sizeof (FILETIME));
	this->m_dwFileSizeHigh = 0xFFFFFFFF;
	this->m_dwFileSizeLow = 0xFFFFFFFF;
} // CTNBinary::CTNBinary
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNBinary::~CTNBinary()"
//==================================================================================
// CTNBinary destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNBinary object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNBinary::~CTNBinary(void)
{
	//DPL(0, "this = %x", 1, this);

	if (this->m_pszPath != NULL)
	{
		LocalFree(this->m_pszPath);
		this->m_pszPath = NULL;
	} // end if (allocated a path)
} // CTNBinary::~CTNBinary
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNBinary::LoadInfo()"
//==================================================================================
// CTNBinary::LoadInfo
//----------------------------------------------------------------------------------
//
// Description: Attempts to load the info for the file at the path already specified
//				for the object.
//				Based on checkver.c by unknown.
//
// Arguments: None.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNBinary::LoadInfo(void)
{
	HRESULT				hr;
	BOOL				fHasVersionInfo = TRUE;
	PVOID				pvVersionInfo = NULL;
	DWORD				dwVersionInfoSize = 0;
	DWORD				dwReserved = 0;
	VS_FIXEDFILEINFO*	pVSFFI = NULL;
	UINT				uSize = 0;
	HANDLE				hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		founddata;


	if (this->m_pszPath == NULL)
	{
		DPL(0, "Object has no file path specified!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (there's no path for this object)

#ifndef _XBOX // no versioning supported

	// Retrieve version information about the file, if available.

	dwVersionInfoSize = GetFileVersionInfoSize(this->m_pszPath, &dwReserved);
	if (dwVersionInfoSize == 0)
	{
		hr = GetLastError();

		// If the binary doesn't contain any version information, then
		// Win9x returns ERROR_BAD_FORMAT
		// NT5 returns S_OK and ERROR_RESOURCE_DATA_NOT_FOUND sometimes.
		if ((hr == S_OK) ||
			(hr == ERROR_BAD_FORMAT) ||
			(hr == ERROR_RESOURCE_DATA_NOT_FOUND))
		{
			DPL(0, "WARNING: %s does not contain version information!",
				1, this->m_pszPath);
			fHasVersionInfo = FALSE;
		} // end if (file doesn't contain version information)
		else
		{
			DPL(0, "GetFileVersionInfoSize failed for %s!", 1, this->m_pszPath);
			return (hr);
		} // end else (other kind of error)
	} // end if (could get file version info size)

	if (fHasVersionInfo)
	{
		pvVersionInfo = LocalAlloc(LPTR, dwVersionInfoSize);
		if (pvVersionInfo == NULL)
			return (E_OUTOFMEMORY);

		if (! GetFileVersionInfo(this->m_pszPath, 0, dwVersionInfoSize, pvVersionInfo))
		{
			hr = GetLastError();
			DPL(0, "GetFileVersionInfo failed for %s!", 1, this->m_pszPath);
			LocalFree(pvVersionInfo);
			pvVersionInfo = NULL;
			return (hr);
		} // end if (getting file version info failed)

		if (! VerQueryValue(pvVersionInfo, "\\", (LPVOID*) &pVSFFI, &uSize))
		{
			hr = GetLastError();
			DPL(0, "VerQueryValue failed for %s!", 1, this->m_pszPath);
			LocalFree(pvVersionInfo);
			pvVersionInfo = NULL;
			return (hr);
		} // end if (getting querying version value failed)

		this->m_fDebug = (pVSFFI->dwFileFlags & VS_FF_DEBUG) ? TRUE : FALSE;
		this->m_dwMS = pVSFFI->dwFileVersionMS;
		this->m_dwLS = pVSFFI->dwFileVersionLS;

		LocalFree(pvVersionInfo);
		pvVersionInfo = NULL;
	} // end if (the file has version information)

#else // ! XBOX
#pragma TODO(tristanj, "Currently avoiding all GetFileVersionInfo calls")
#endif // XBOX


	// We can't use the GetFileTime and GetFileSize() APIs because they
	// require a handle to an open file, and Windows yells at us if the
	// file is already in use (like a loaded DLL or executing EXE).  Thus
	// we need to grab the info from a FindFile call that searches only
	// for the specified file, because that function happens to return the
	// info we need.

#ifndef _XBOX // Damn Ascii to Unicode conversion...
	hFind = FindFirstFile(this->m_pszPath, &founddata);
#else // ! XBOX
	LPWSTR	pszWidePath = NULL;
	int     nWideSize = 0;

	//Determine the size of the wide string we should be allocating, then allocate it and copy the wide string into it
	nWideSize = MultiByteToWideChar(CP_ACP, 0, this->m_pszPath, strlen(this->m_pszPath), NULL, 0) + 1;
	pszWidePath = (LPWSTR) LocalAlloc(LPTR, nWideSize * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, this->m_pszPath, strlen(this->m_pszPath), pszWidePath, nWideSize);
#pragma TODO(tristanj, "Might want to check to make sure this worked")
	hFind = FindFirstFile(pszWidePath, &founddata);
	LocalFree(pszWidePath);
#endif // XBOX

	if (hFind == INVALID_HANDLE_VALUE)
	{
		hr = GetLastError();
		DPL(0, "Couldn't retrieve find info (times and size) for %s!",
			1, this->m_pszPath);
		return (hr);
	} // end if (couldn't find first file)

	CopyMemory(&(this->m_createtime), &(founddata.ftCreationTime), sizeof (FILETIME));
	CopyMemory(&(this->m_writetime), &(founddata.ftLastWriteTime), sizeof (FILETIME));

	this->m_dwFileSizeHigh = founddata.nFileSizeHigh;
	this->m_dwFileSizeLow = founddata.nFileSizeLow;

	if (! FindClose(hFind))
	{
		hr = GetLastError();
		DPL(0, "Couldn't close find file search for %s!",
			1, this->m_pszPath);
		return (hr);
	} // end if (couldn't close search)


	return (S_OK);
} // CTNBinary::LoadInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNBinary::IsEqualTo()"
//==================================================================================
// CTNBinary::IsEqualTo
//----------------------------------------------------------------------------------
//
// Description: Determines whether the info previously loaded for this object is
//				the same as the passed in object's, excluding the actual files'
//				locations. (i.e., whether the files are the same but at possibly
//				different paths).
//
// Arguments:
//	PTNBINARY pOtherBinary	Pointer to binary to compare this object with.
//
// Returns: TRUE if the files described are the same.
//==================================================================================
BOOL CTNBinary::IsEqualTo(PTNBINARY pOtherBinary)
{
#ifndef _XBOX // no versioning supported

	if (this->m_fDebug != pOtherBinary->m_fDebug)
		return (FALSE);

	if (this->m_dwMS != pOtherBinary->m_dwMS)
		return (FALSE);

	if (this->m_dwLS != pOtherBinary->m_dwLS)
		return (FALSE);

#endif // ! XBOX

	if (CompareFileTime(&(this->m_createtime), &(pOtherBinary->m_createtime)) != 0)
		return (FALSE);

	if (CompareFileTime(&(this->m_writetime), &(pOtherBinary->m_writetime)) != 0)
		return (FALSE);

	if ((this->m_dwFileSizeHigh != pOtherBinary->m_dwFileSizeHigh) ||
		(this->m_dwFileSizeLow != pOtherBinary->m_dwFileSizeLow))
	{
		return (FALSE);
	} // end if (file size isn't the same)

	return (TRUE);
} // CTNBinary::IsEqualTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNBinaryLocsList::CTNBinaryLocsList()"
//==================================================================================
// CTNBinaryLocsList constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNBinaryLocsList object.  Initializes the data
//				structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNBinaryLocsList::CTNBinaryLocsList(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNBinaryLocsList));

	this->m_pszName = NULL;
} // CTNBinaryLocsList::CTNBinaryLocsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNBinaryLocsList::~CTNBinaryLocsList()"
//==================================================================================
// CTNBinaryLocsList destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNBinaryLocsList object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNBinaryLocsList::~CTNBinaryLocsList(void)
{
	//DPL(0, "this = %x", 1, this);

	if (this->m_pszName != NULL)
	{
		LocalFree(this->m_pszName);
		this->m_pszName = NULL;
	} // end if (allocated a name)
} // CTNBinaryLocsList::~CTNBinaryLocsList
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTrackedBinaries::CTNTrackedBinaries()"
//==================================================================================
// CTNTrackedBinaries constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNTrackedBinaries object.  Initializes the data
//				structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNTrackedBinaries::CTNTrackedBinaries(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNTrackedBinaries));
} // CTNTrackedBinaries::CTNTrackedBinaries
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTrackedBinaries::~CTNTrackedBinaries()"
//==================================================================================
// CTNTrackedBinaries destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNTrackedBinaries object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNTrackedBinaries::~CTNTrackedBinaries(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNTrackedBinaries::~CTNTrackedBinaries
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTrackedBinaries::LocateAndAddBinary()"
//==================================================================================
// CTNTrackedBinaries::LocateAndAddBinary
//----------------------------------------------------------------------------------
//
// Description: Adds all found files with the specified name to the list of tracked
//				binaries.
//
// Arguments:
//	char* szBinaryName		Name of binary to identify and manage.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNTrackedBinaries::LocateAndAddBinary(char* szBinaryName)
{
#ifndef _XBOX // no binary management supported
	HRESULT				hr = S_OK;
	PTNBINARYLOCSLIST	pBinLocs = NULL;
	CLStringList		searchpaths;
	CLStringList		foundpaths;
	char*				pszTemp = NULL;
	char*				pszPathVar = NULL;
	PTNBINARY			pBin = NULL;
	DWORD				dwBufferSize = 0;
	int					i;
	DWORD				dwSize;
	DWORD				dwField;
	DWORD				dwNumFields;
	char*				pszField;

	if (szBinaryName == NULL)
	{
		DPL(0, "Attempting to add NULL data!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (the binary name doesn't exist)

	pBinLocs = new (CTNBinaryLocsList);
	if (pBinLocs == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	pBinLocs->m_pszName = (char*) LocalAlloc(LPTR, (strlen(szBinaryName) + 1));
	if (pBinLocs->m_pszName == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	
	strcpy(pBinLocs->m_pszName, szBinaryName);


	// We should be checking (in order):
	//		
	//		 1. The directory from which the application loaded.
	//		 2. The current directory.
	//			Windows 95: The Windows system directory. Use the
	//				GetSystemDirectory function to get the path of this directory.
	//			Windows NT: The 32-bit Windows system directory. Use the
	//				GetSystemDirectory function to get the path of this directory.
	//				The name of this directory is SYSTEM32.
	//			Windows NT: The 16-bit Windows system directory. There is no Win32
	//				function that obtains the path of this directory, but it is
	//				searched. The name of this directory is SYSTEM.
	//		 6. The Windows directory. Use the GetWindowsDirectory function to get
	//				the path of this directory.
	//		 7. The directories that are listed in the PATH environment variable.
	//
	//		 The Windows API SearchPath() indicates the one that will be used.

	/*
	// Search C:\ for the binaries

	hr = this->SearchFor(szBinaryName, "C:\\", TRUE, &foundpaths);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't search for %s in %s!", 2, szBinaryName, "C:\\");
		goto DONE;
	} // end if (couldn't find paths)
	*/

	// Search current directory for the binaries.

	dwSize = GetCurrentDirectory(0, NULL) + 1; // +1 for possible trailing backslash

	// Increase buffer size if necessary.
	if (dwSize > dwBufferSize)
	{
		if (pszTemp != NULL)
			LocalFree(pszTemp);

		dwBufferSize = dwSize;

		pszTemp = (char*) LocalAlloc(LPTR, dwBufferSize);
		if (pszTemp == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)
	} // end if (need to reallocate buffer)

	GetCurrentDirectory(dwSize, pszTemp); // ignoring error

	// Make sure it ends in a backslash to reduce number of searches.
	if (! StringEndsWith(pszTemp, "\\", TRUE))
		strcat(pszTemp, "\\");

	// Add it to the paths to search, if it's not already there.
	if (searchpaths.GetStringIndex(pszTemp, 0, FALSE) == -1)
	{
		hr = searchpaths.AddString(pszTemp);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add \"%s\" to search paths list!", 1, pszTemp);
			goto DONE;
		} // end if (couldn't add search path)
	} // end if (didn't find path)


	// Search system directory for the binaries.

	dwSize = GetSystemDirectory(NULL, 0) + 1; // +1 for possible trailing backslash

	// Increase buffer size if necessary.
	if (dwSize > dwBufferSize)
	{
		if (pszTemp != NULL)
			LocalFree(pszTemp);

		dwBufferSize = dwSize;

		pszTemp = (char*) LocalAlloc(LPTR, dwBufferSize);
		if (pszTemp == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)
	} // end if (need to reallocate buffer)

	GetSystemDirectory(pszTemp, dwSize); // ignoring error

	// Make sure it ends in a backslash to reduce number of searches.
	if (! StringEndsWith(pszTemp, "\\", TRUE))
		strcat(pszTemp, "\\");

	// Add it to the paths to search, if it's not already there.
	if (searchpaths.GetStringIndex(pszTemp, 0, FALSE) == -1)
	{
		hr = searchpaths.AddString(pszTemp);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add \"%s\" to search paths list!", 1, pszTemp);
			goto DONE;
		} // end if (couldn't add search path)
	} // end if (didn't find path)


#pragma TODO(vanceo, "Check 16-bit system directory on NT")


	// Search Windows directory for the binaries.

	dwSize = GetWindowsDirectory(NULL, 0) + 1; // +1 for possible trailing backslash

	// Increase buffer size if necessary.
	if (dwSize > dwBufferSize)
	{
		if (pszTemp != NULL)
			LocalFree(pszTemp);

		dwBufferSize = dwSize;

		pszTemp = (char*) LocalAlloc(LPTR, dwBufferSize);
		if (pszTemp == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)
	} // end if (need to reallocate buffer)

	GetWindowsDirectory(pszTemp, dwSize); // ignoring error

	// Make sure it ends in a backslash to reduce number of searches.
	if (! StringEndsWith(pszTemp, "\\", TRUE))
		strcat(pszTemp, "\\");

	// Add it to the paths to search, if it's not already there.
	if (searchpaths.GetStringIndex(pszTemp, 0, FALSE) == -1)
	{
		hr = searchpaths.AddString(pszTemp);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add \"%s\" to search paths list!", 1, pszTemp);
			goto DONE;
		} // end if (couldn't add search path)
	} // end if (didn't find path)




	// Get the PATH environment variable

	dwSize = GetEnvironmentVariable("PATH", NULL, 0);
	pszPathVar = (char*) LocalAlloc(LPTR, dwSize);
	if (pszPathVar == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	GetEnvironmentVariable("PATH", pszPathVar, dwSize);

	// It's okay to pass the string to split in as the output buffer because we don't
	// care if it gets destroyed.
	dwNumFields = StringSplitIntoFields(pszPathVar, ";", pszPathVar, &dwSize);

	// Loop through all the paths in the environment variable
	for(dwField = 0; dwField < dwNumFields; dwField++)
	{
		pszField = StringGetFieldPtr(pszPathVar, dwField);

		dwSize = strlen(pszField) + 2; // + 1 for possible backslash, + 1 for NULL termination

		// Increase buffer size if necessary.
		if (dwSize > dwBufferSize)
		{
			if (pszTemp != NULL)
				LocalFree(pszTemp);

			dwBufferSize = dwSize;

			pszTemp = (char*) LocalAlloc(LPTR, dwBufferSize);
			if (pszTemp == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)
		} // end if (need to reallocate buffer)

		strcpy(pszTemp, pszField);

		// Make sure it ends in a backslash to reduce number of searches.
		if (! StringEndsWith(pszTemp, "\\", TRUE))
			strcat(pszTemp, "\\");

		// Add it to the paths to search, if it's not already there.
		if (searchpaths.GetStringIndex(pszTemp, 0, FALSE) == -1)
		{
			hr = searchpaths.AddString(pszTemp);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add \"%s\" to search paths list!",
					1, StringGetFieldPtr(pszTemp, dwSize));
				goto DONE;
			} // end if (couldn't add search path)
		} // end if (didn't find path)
	} // end for (each path)

	// Loop through each of the search paths we accumulated.
	for(i = 0; i < searchpaths.Count(); i++)
	{
		hr = this->SearchFor(szBinaryName, searchpaths.GetIndexedString(i),
							FALSE, &foundpaths);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't search for \"%s\" in \"%s\"!",
				2, szBinaryName, searchpaths.GetIndexedString(i));
			goto DONE;
		} // end if (couldn't find paths)
	} // end for (each path)



	// Loop through all the found binary paths and add them to our list

	while(foundpaths.Count() > 0)
	{
		pBin = new (CTNBinary);
		if (pBin == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)

		pBin->m_pszPath = (char*) LocalAlloc(LPTR, (strlen(foundpaths.GetIndexedString(0)) + 1));
		if (pBin->m_pszPath == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)
		
		strcpy(pBin->m_pszPath, foundpaths.GetIndexedString(0));

		foundpaths.Remove(0); // ignoring error

		hr = pBin->LoadInfo();
		if (hr != S_OK)
		{
			DPL(0, "Couldn't load info for %s!", 1, pBin->m_pszPath);
			goto DONE;
		} // end if (add failed)

		hr = pBinLocs->m_bins.Add(pBin);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add the item to the list!", 0);
			goto DONE;
		} // end if (add failed)

		pBin = NULL; // forget about it so we don't free it below
	} // end for (each found path)

	hr = this->Add(pBinLocs);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't add the binaries to the list!", 0);
		goto DONE;
	} // end if (add failed)

	pBinLocs = NULL; // forget about it so we don't free it below


DONE:

	if (pBin != NULL)
	{
		delete (pBin);
		pBin = NULL;
	} // end if (have object)

	if (pszTemp != NULL)
	{
		LocalFree(pszTemp);
		pszTemp = NULL;
	} // end if (allocated memory)

	if (pszPathVar != NULL)
	{
		LocalFree(pszPathVar);
		pszPathVar = NULL;
	} // end if (allocated memory)

	if (pBinLocs != NULL)
	{
		delete (pBinLocs);
		pBinLocs = NULL;
	} // end if (have object)

	return (hr);
#else // ! XBOX
	return S_OK;
#endif // XBOX
} // CTNTrackedBinaries::LocateAndAddBinary
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTrackedBinaries::GetBinary()"
//==================================================================================
// CTNTrackedBinaries::GetBinary
//----------------------------------------------------------------------------------
//
// Description: Returns a pointer to a list of all locations of tracked binaries
//				with the given name.
//
// Arguments:
//	char* szBinaryName		Binary name to retrieve.
//
// Returns: Pointer to list of locations for binary object if found, NULL if an
//			error occurred.
//==================================================================================
PTNBINARYLOCSLIST CTNTrackedBinaries::GetBinary(char* szBinaryName)
{
	int					i;
	PTNBINARYLOCSLIST	pItem = NULL;


	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pItem = (PTNBINARYLOCSLIST) this->GetItem(i);
		if (pItem == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get item %i!", 1, i);
			return (NULL);
		} // end if (couldn't get that item)

		// If we found the binary we want, return it
		if (strcmp(pItem->m_pszName, szBinaryName) == 0)
		{
			this->LeaveCritSection();
			return (pItem);
		} // end if (couldn't get that item)
	} // end for (each data item in the list)

	this->LeaveCritSection();
	DPL(0, "Couldn't find binary %s in tracked binaries list!", 1, szBinaryName);

	return (NULL);
} // CTNTrackedBinaries::GetBinary
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTrackedBinaries::FirstBinIsVersion()"
//==================================================================================
// CTNTrackedBinaries::FirstBinIsVersion
//----------------------------------------------------------------------------------
//
// Description: Sets the passed in boolean to TRUE if the first occurrence of a
//				binary is the specified version, FALSE otherwise.
//
// Arguments:
//	char* szBinaryName		Binary name to examine.
//	DWORD dwHighVersion		High DWORD of file version to compare.
//	DWORD dwLowVersion		Low DWORD of file version to compare.
//	BOOL* pfResult			Pointer to place to store result.
//
// Returns: S_OK if trying to compare was successful, error code otherwise.  Note
//			lpbResult holds the actual comparison result.
//==================================================================================
HRESULT CTNTrackedBinaries::FirstBinIsVersion(char* szBinaryName,
												DWORD dwHighVersion,
												DWORD dwLowVersion,
												BOOL* pfResult)
{
	PTNBINARYLOCSLIST	pList = NULL;
	PTNBINARY			pItem = NULL;

#ifndef _XBOX // no versioning supported

	this->EnterCritSection();

	pList = this->GetBinary(szBinaryName);
	if (pList == NULL)
	{
		this->LeaveCritSection();
		DPL(0, "Couldn't get item binary locations for \"%s\"!", 1, szBinaryName);

		//return (E_FAIL);

		(*pfResult) = FALSE;
		return (S_OK);
	} // end if (couldn't get that item)

	// We use the first instance.
	pItem = (PTNBINARY) pList->m_bins.GetItem(0);
	if (pItem == NULL)
	{
		this->LeaveCritSection();
		DPL(0, "Couldn't get first location of %s!", 1, szBinaryName);
		return (E_FAIL);
	} // end if (couldn't get that item)


	if ((pItem->m_dwMS == dwHighVersion) && (pItem->m_dwLS == dwLowVersion))
		(*pfResult) = TRUE;
	else
		(*pfResult) = FALSE;


	this->LeaveCritSection();

#else // ! XBOX
#pragma BUGBUG(tristanj, "Hardcoded FirstBinIsVersion to always return TRUE")
	(*pfResult) = TRUE;
#endif // XBOX

	return (S_OK);
} // CTNTrackedBinaries::FirstBinIsVersion
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTrackedBinaries::FirstBinIsAtLeastVersion()"
//==================================================================================
// CTNTrackedBinaries::FirstBinIsAtLeastVersion
//----------------------------------------------------------------------------------
//
// Description: Sets the passed in boolean to TRUE if the first occurrence of a
//				binary is the specified version or higher, FALSE otherwise.
//
// Arguments:
//	char* szBinaryName		Binary name to examine.
//	DWORD dwHighVersion		High DWORD of file version to compare.
//	DWORD dwLowVersion		Low DWORD of file version to compare.
//	BOOL* pfResult			Pointer to place to store result.
//
// Returns: S_OK if trying to compare was successful, error code otherwise.  Note
//			pfResult holds the actual comparison result.
//==================================================================================
HRESULT CTNTrackedBinaries::FirstBinIsAtLeastVersion(char* szBinaryName,
													DWORD dwHighVersion,
													DWORD dwLowVersion,
													BOOL* pfResult)
{
	PTNBINARYLOCSLIST	pList = NULL;
	PTNBINARY			pItem = NULL;

#ifndef _XBOX // no versioning supported

	this->EnterCritSection();

	pList = this->GetBinary(szBinaryName);
	if (pList == NULL)
	{
		this->LeaveCritSection();
		DPL(0, "Couldn't get item binary locations for \"%s\"!", 1, szBinaryName);

		//return (E_FAIL);

		(*pfResult) = FALSE;
		return (S_OK);
	} // end if (couldn't get that item)

	// We use the first instance.
	pItem = (PTNBINARY) pList->m_bins.GetItem(0);
	if (pItem == NULL)
	{
		this->LeaveCritSection();
		DPL(0, "Couldn't get first location of %s!", 1, szBinaryName);
		return (E_FAIL);
	} // end if (couldn't get that item)


	if (pItem->m_dwMS > dwHighVersion)
		(*pfResult) = TRUE;
	else if ((pItem->m_dwMS == dwHighVersion) && (pItem->m_dwLS >= dwLowVersion))
		(*pfResult) = TRUE;
	else
		(*pfResult) = FALSE;


	this->LeaveCritSection();

#else // ! XBOX
#pragma BUGBUG(tristanj, "Hardcoded FirstBinIsVersion to always return TRUE")
	(*pfResult) = TRUE;
#endif // XBOX

	return (S_OK);
} // CTNTrackedBinaries::FirstBinIsAtLeastVersion
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTrackedBinaries::LoadSymbolsForFirstBinaries()"
//==================================================================================
// CTNTrackedBinaries::LoadSymbolsForFirstBinaries
//----------------------------------------------------------------------------------
//
// Description: Attempts to load symbols for the first instance of all of the
//				DLLs (binaries that don't end in ".EXE", actually) in the list.
//
// Arguments: None.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNTrackedBinaries::LoadSymbolsForFirstBinaries(void)
{
	HRESULT					hr = S_OK;
	PTNBINARYLOCSLIST		pBinaryLocsList = NULL;
	PTNBINARY				pBinary = NULL;
#ifndef _XBOX // no versioning supported
	int						i;
	char					szExtension[32];
#endif // ! XBOX

#ifndef _XBOX // no versioning supported

	this->EnterCritSection();
	for(i = 0; i < this->Count(); i++)
	{
		pBinaryLocsList = (PTNBINARYLOCSLIST) this->GetItem(i);
		if (pBinaryLocsList == NULL)
		{
			DPL(0, "Couldn't get binary location list item %i!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		pBinary = (PTNBINARY) pBinaryLocsList->m_bins.GetItem(0);
		if (pBinary == NULL)
		{
			DPL(0, "WARNING: Couldn't get first binary location for %s!",
				1, pBinaryLocsList->m_pszName);
			/*
			hr = E_FAIL;
			goto DONE;
			*/
			continue;
		} // end if (couldn't get item)

		FileGetExtension(pBinary->m_pszPath, szExtension, FALSE);

		_strlwr(szExtension);
		if (strcmp(szExtension, "exe") != 0)
		{
			hr = TNSymLoadSymbolsForModules(pBinary->m_pszPath);
			if (hr != S_OK)
			{
				DPL(0, "WARNING: Couldn't load symbols for binary \"%s\"!  %e",
					2, pBinary->m_pszPath, hr);
				//goto DONE;
				hr = S_OK;
			} // end if (couldn't load symbols for binary)
		} // end if (the object is not an executable)
		else
		{
			DPL(6, "Not loading symbols for executable \"%s\".", 1, pBinary->m_pszPath);
		} // end if (the object is an executable)
	} // end for (each binary location)

DONE:

	this->LeaveCritSection();

#else // ! XBOX
#pragma BUGBUG(tristanj, "Hardcoded to report that symbols have been loaded successfully")
#endif // XBOX

	return (hr);
} // CTNTrackedBinaries::LoadSymbolsForFirstBinaries
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTrackedBinaries::PackIntoBuffer()"
//==================================================================================
// CTNTrackedBinaries::PackIntoBuffer
//----------------------------------------------------------------------------------
//
// Description: Flattens this list into a contiguous buffer for easy transmission.
//				If the pointer to store the results in is NULL, then no memory
//				is copied, but the buffer size is still set to the amount of memory
//				required.
//
// Arguments:
//	PVOID pvBuffer			Pointer to buffer to use.
//	DWORD* pdwBufferSize	Pointer to size of buffer, or place to fill in with
//							buffer size required.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNTrackedBinaries::PackIntoBuffer(PVOID pvBuffer, DWORD* pdwBufferSize)
{
#ifndef _XBOX // No tracked binaries supported
	LPBYTE				lpCurrent = (LPBYTE) pvBuffer; // start at the beginning
	int					i;
	PTNBINARYLOCSLIST	pBinLocsList = NULL;
	int					iTemp = 0;
	int					j;
	PTNBINARY			pBinLoc = NULL;
	DWORD				dwStringLength = 0;



	(*pdwBufferSize) = 0;

	if (this->Count() <= 0)
	{
		return (S_OK);
	} // end if (there aren't any items in this list)

	this->EnterCritSection();

	// Make sure the unpacker knows how many binary location lists we have

	(*pdwBufferSize) += sizeof (int); // number of items in the list
	if (pvBuffer != NULL)
	{
		iTemp = this->Count();
		CopyAndMoveDestPointer(lpCurrent, &iTemp, sizeof (int))
	} // end if (there's a buffer to copy to)

	for(i = 0; i < this->Count(); i++)
	{
		pBinLocsList = (PTNBINARYLOCSLIST) this->GetItem(i);
		if (pBinLocsList == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get bin locs list %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get that item)

		// Make sure the unpacker knows how many binaries this list has

		(*pdwBufferSize) += sizeof (int); // number of items in the list
		if (pvBuffer != NULL)
		{
			iTemp = pBinLocsList->m_bins.Count();
			CopyAndMoveDestPointer(lpCurrent, &iTemp, sizeof (int))
		} // end if (there's a buffer to copy to)



		dwStringLength = strlen(pBinLocsList->m_pszName);

		(*pdwBufferSize) += sizeof (DWORD)		// string length
							+ dwStringLength;	// actual string

		if (pvBuffer != NULL)
		{
			CopyAndMoveDestPointer(lpCurrent, &(dwStringLength), sizeof (DWORD))
			CopyAndMoveDestPointer(lpCurrent, pBinLocsList->m_pszName, dwStringLength)
		} // end if (there's a buffer to copy to)



		for(j = 0; j < pBinLocsList->m_bins.Count(); j++)
		{
			pBinLoc = (PTNBINARY) pBinLocsList->m_bins.GetItem(j);
			if (pBinLoc == NULL)
			{
				this->LeaveCritSection();
				DPL(0, "Couldn't get binary location %i!", 1, j);
				return (E_FAIL);
			} // end if (couldn't get that item)

			dwStringLength = strlen(pBinLoc->m_pszPath);

			(*pdwBufferSize) += sizeof (DWORD)			// string length
								+ dwStringLength		// actual string
								+ sizeof (BOOL)			// debug
								+ sizeof (DWORD) * 2	// version DWORDs
								+ sizeof (FILETIME)		// create time
								+ sizeof (FILETIME)		// write time
								+ sizeof (DWORD) * 2;	// file size

			if (pvBuffer != NULL)
			{
				CopyAndMoveDestPointer(lpCurrent, &(dwStringLength), sizeof (DWORD))
				CopyAndMoveDestPointer(lpCurrent, pBinLoc->m_pszPath, dwStringLength)
				CopyAndMoveDestPointer(lpCurrent, &(pBinLoc->m_fDebug), sizeof (BOOL))
				CopyAndMoveDestPointer(lpCurrent, &(pBinLoc->m_dwMS), sizeof (DWORD))
				CopyAndMoveDestPointer(lpCurrent, &(pBinLoc->m_dwLS), sizeof (DWORD))
				CopyAndMoveDestPointer(lpCurrent, &(pBinLoc->m_createtime), sizeof (FILETIME))
				CopyAndMoveDestPointer(lpCurrent, &(pBinLoc->m_writetime), sizeof (FILETIME))
				CopyAndMoveDestPointer(lpCurrent, &(pBinLoc->m_dwFileSizeHigh), sizeof (DWORD))
				CopyAndMoveDestPointer(lpCurrent, &(pBinLoc->m_dwFileSizeLow), sizeof (DWORD))
			} // end if (there's a buffer to copy to)
		} // end for (each binary location)
	} // end for (each group of like-named binaries)
	this->LeaveCritSection();

	if (pvBuffer == NULL)
		return (ERROR_BUFFER_TOO_SMALL);

	return (S_OK);
#else // ! XBOX
	*pdwBufferSize = 0;
	return S_OK;
#endif // XBOX
} // CTNTrackedBinaries::PackIntoBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTrackedBinaries::UnpackFromBuffer()"
//==================================================================================
// CTNTrackedBinaries::UnpackFromBuffer
//----------------------------------------------------------------------------------
//
// Description: Populates this list with the contents previously stored in a
//				contiguous buffer.
//
// Arguments:
//	PVOID pvBuffer			Pointer to buffer to use.
//	DWORD dwBufferSize		Size of buffer.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNTrackedBinaries::UnpackFromBuffer(PVOID pvBuffer, DWORD dwBufferSize)
{
	HRESULT					hr;
	LPBYTE					lpCurrent = (LPBYTE) pvBuffer; // start at the beginning
	int						iNumLists = 0;
	int						i;
	PTNBINARYLOCSLIST		pNewBinLocsList = NULL;
	int						iNumBins = 0;
	int						j;
	PTNBINARY				pNewBinLoc = NULL;
	DWORD					dwStringLength = 0;



	this->EnterCritSection();

	CopyAndMoveSrcPointer(&iNumLists, lpCurrent, sizeof (int));

	for(i = 0; i < iNumLists; i++)
	{
		pNewBinLocsList = new (CTNBinaryLocsList);
		if (pNewBinLocsList == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)

		CopyAndMoveSrcPointer(&iNumBins, lpCurrent, sizeof (int));

		CopyAndMoveSrcPointer(&dwStringLength, lpCurrent, sizeof (DWORD));
		pNewBinLocsList->m_pszName = (char*) LocalAlloc(LPTR, dwStringLength + 1);
		if (pNewBinLocsList->m_pszName == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)

		CopyAndMoveSrcPointer(pNewBinLocsList->m_pszName, lpCurrent, dwStringLength);


		for(j = 0; j < iNumBins; j++)
		{
			pNewBinLoc = new (CTNBinary);
			if (pNewBinLoc == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate object)

			CopyAndMoveSrcPointer(&dwStringLength, lpCurrent, sizeof (DWORD));
			pNewBinLoc->m_pszPath = (char*) LocalAlloc(LPTR, dwStringLength + 1);
			if (pNewBinLoc->m_pszPath == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)
	
			CopyAndMoveSrcPointer(pNewBinLoc->m_pszPath, lpCurrent, dwStringLength);


			CopyAndMoveSrcPointer(&(pNewBinLoc->m_fDebug), lpCurrent, sizeof (BOOL));
			CopyAndMoveSrcPointer(&(pNewBinLoc->m_dwMS), lpCurrent, sizeof (DWORD));
			CopyAndMoveSrcPointer(&(pNewBinLoc->m_dwLS), lpCurrent, sizeof (DWORD));
			CopyAndMoveSrcPointer(&(pNewBinLoc->m_createtime), lpCurrent, sizeof (FILETIME));
			CopyAndMoveSrcPointer(&(pNewBinLoc->m_writetime), lpCurrent, sizeof (FILETIME));
			CopyAndMoveSrcPointer(&(pNewBinLoc->m_dwFileSizeHigh), lpCurrent, sizeof (DWORD));
			CopyAndMoveSrcPointer(&(pNewBinLoc->m_dwFileSizeLow), lpCurrent, sizeof (DWORD));
			

			hr = pNewBinLocsList->m_bins.Add(pNewBinLoc);
			if (hr != S_OK)
			{
				DPL(0, "Adding binary %s failed!", 1, pNewBinLoc->m_pszPath);
				goto DONE;
			} // end if (couldn't add item)

			pNewBinLoc = NULL; // forget about it so we don't free it below
		} // end for (each binary location)
			

		hr = this->Add(pNewBinLocsList);
		if (hr != S_OK)
		{
			DPL(0, "Adding binary location list %s failed!", 1, pNewBinLocsList->m_pszName);
			goto DONE;
		} // end if (couldn't add item)

		pNewBinLocsList = NULL; // forget about it so we don't free it below
	} // end for (each binary location list)


DONE:

	this->LeaveCritSection();

	if (pNewBinLoc != NULL)
	{
		delete (pNewBinLoc);
		pNewBinLoc = NULL;
	} // end if (have a left over binary)

	if (pNewBinLocsList != NULL)
	{
		delete (pNewBinLocsList);
		pNewBinLocsList = NULL;
	} // end if (have a left over loc object)

	return (hr);
} // CTNTrackedBinaries::UnpackFromBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""







#ifndef _XBOX // no file printing supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTrackedBinaries::PrintToFile()"
//==================================================================================
// CTNTrackedBinaries::PrintToFile
//----------------------------------------------------------------------------------
//
// Description: Prints all the objects in this list to the file specified.
//
// Arguments:
//	HANDLE hFile		File to print to.
//
// Returns: None.
//==================================================================================
void CTNTrackedBinaries::PrintToFile(HANDLE hFile)
{
	int					i;
	PTNBINARYLOCSLIST	pBinaries = NULL;
	PTNBINARY			pBinaryLoc = NULL;
	char				szBuffer[1024];
	int					j;

	FileWriteLine(hFile, "\t(Binaries)"); //ignoring errors

	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pBinaries = (PTNBINARYLOCSLIST) this->GetItem(i);
		if (pBinaries == NULL)
		{
			this->LeaveCritSection();
			DPL(0, "Couldn't get binary item %i!", 1, i);
			return;
		} // end if (couldn't get that item)

		// ignoring errors
		FileSprintfWriteLine(hFile, "\t\t<%s>", 1, pBinaries->m_pszName);

		for(j = 0; j < pBinaries->m_bins.Count(); j++)
		{
			pBinaryLoc = (PTNBINARY) pBinaries->m_bins.GetItem(j);
			if (pBinaryLoc == NULL)
			{
				this->LeaveCritSection();
				DPL(0, "Couldn't get binary location %i!", 1, i);
				return;
			} // end if (couldn't get that item)

			FileSprintfWriteLine(hFile, "\t\t\tVersion= %04u.%04u.%04u.%04u",
								4, HIWORD(pBinaryLoc->m_dwMS), LOWORD(pBinaryLoc->m_dwMS),
								HIWORD(pBinaryLoc->m_dwLS), LOWORD(pBinaryLoc->m_dwLS));

			FileSprintfWriteLine(hFile, "\t\t\tBuild= %s",
								1, ((pBinaryLoc->m_fDebug) ? "Debug" : "Retail"));

			if (pBinaryLoc->m_dwFileSizeHigh > 0)
			{
				FileSprintfWriteLine(hFile, "\t\t\tFilesizeHigh= %u",
									1, pBinaryLoc->m_dwFileSizeHigh);
			} // end if (there's a top 32 bits of file size
			FileSprintfWriteLine(hFile, "\t\t\tFilesize= %u",
								1, pBinaryLoc->m_dwFileSizeLow);

			StringGetStrFromFiletime(&(pBinaryLoc->m_createtime), FALSE, szBuffer);
			FileSprintfWriteLine(hFile, "\t\t\tCreated= %s", 1, szBuffer);

			StringGetStrFromFiletime(&(pBinaryLoc->m_writetime), FALSE, szBuffer);
			FileSprintfWriteLine(hFile, "\t\t\tLastWritten= %s", 1, szBuffer);
		} // end for (each location the binary has)

	} // end for (each data item in the list)

	this->LeaveCritSection();
} // CTNTrackedBinaries::PrintToFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // no tracked binaries supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNTrackedBinaries::SearchFor()"
//==================================================================================
// CTNTrackedBinaries::SearchFor
//----------------------------------------------------------------------------------
//
// Description: Searches for all matching files with the name specified in the
//				directory given, and all subdirectories if specified.  The paths to
//				the matches are added to the passed in string list.
//
// Arguments:
//	char* szFilename				Filename to search for.
//	char* szSearchDirPath			Directory to start search in.
//	BOOL fSubdirs					Should subdirectories be searched?
//	PLSTRINGLIST pResultPaths		Pointer to string list to attach paths of found
//									matching filenames to.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNTrackedBinaries::SearchFor(char* szFilename, char* szSearchDirPath,
									BOOL fSubdirs, PLSTRINGLIST pResultPaths)
{
	HRESULT				hr;
	char				szLwrSearchFilename[MAX_PATH];
	char				szLwrFoundFilename[MAX_PATH];
	DWORD				dwSearchPatternSize = 0;
	char*				pszSearchPattern = NULL;
	HANDLE				hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		founddata;
	DWORD				dwSubItemPathSize = 0;
	char*				pszSubItemPath = NULL;


	strcpy(szLwrSearchFilename, szFilename);
	_strlwr(szLwrSearchFilename);

	//StringPrepend(blah, "\\\\?\\");

	dwSearchPatternSize = strlen(szSearchDirPath) + strlen("*.*") + 1;

	if (! StringEndsWith(szSearchDirPath, "\\", FALSE))
		dwSearchPatternSize += 1;

	pszSearchPattern = (char*) LocalAlloc(LPTR, dwSearchPatternSize);
	if (pszSearchPattern == NULL)
		return (E_OUTOFMEMORY);

	strcpy(pszSearchPattern, szSearchDirPath);
	if (! StringEndsWith(szSearchDirPath, "\\", FALSE))
		strcat(pszSearchPattern, "\\");
	strcat(pszSearchPattern, "*.*");

#ifndef _XBOX // Damn Ascii to Unicode conversion...
	hFind = FindFirstFile(pszSearchPattern, &founddata);
#else // ! XBOX
	LPWSTR	pszWideSearchPattern = NULL;
	int     nWideSize = 0;

	//Determine the size of the wide string we should be allocating, then allocate it and copy the wide string into it
	nWideSize = MultiByteToWideChar(CP_ACP, 0, pszSearchPattern, strlen(pszSearchPattern), NULL, 0) + 1;
	pszWideSearchPattern = (LPWSTR) LocalAlloc(LPTR, nWideSize * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, pszSearchPattern, strlen(pszSearchPattern), pszWideSearchPattern, nWideSize);
#pragma TODO(tristanj, "Might want to check to make sure this worked")
	hFind = FindFirstFile(pszWideSearchPattern, &founddata);
	LocalFree(pszWideSearchPattern);
#endif // XBOX
	if (hFind == INVALID_HANDLE_VALUE)
	{
		hr = GetLastError();

		DPL(0, "Couldn't start search in \"%s\"!  %e",
			2, szSearchDirPath, hr);

		LocalFree(pszSearchPattern);
		pszSearchPattern = NULL;

	     // We ignore the error because it's probably because the directory didn't
		// exist.  The user could have some bogus info in his PATH environment
		// variable, for example.
		return (S_OK);
	} // end if (couldn't find first file)

	LocalFree(pszSearchPattern);
	pszSearchPattern = NULL;


	do
	{
		if ((strcmp(founddata.cFileName, ".") != 0) &&
			(strcmp(founddata.cFileName, "..") != 0))
		{
			dwSubItemPathSize = strlen(szSearchDirPath)+ strlen(founddata.cFileName) + 1;
			if (! StringEndsWith(szSearchDirPath, "\\", FALSE))
				dwSubItemPathSize += 1;

			// If this found item is a directory and we're allowed, then search it
			// recursively.  Otherwise, check to see if it's a matching file.
			if (fSubdirs &&
				(founddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				dwSubItemPathSize++; // include backslash for this directory, too

				pszSubItemPath = (char*) LocalAlloc(LPTR, dwSubItemPathSize);
				if (pszSubItemPath == NULL)
				{
					FindClose(hFind);
					return (E_OUTOFMEMORY);
				} // end if (couldn't allocate memory)

				strcpy(pszSubItemPath, szSearchDirPath);
				if (! StringEndsWith(szSearchDirPath, "\\", FALSE))
					strcat(pszSubItemPath, "\\");
				strcat(pszSubItemPath, founddata.cFileName);
				strcat(pszSubItemPath, "\\");

				// This is a sub dir, so duh, we're searching subdirs, just pass
				// TRUE for that parameter.
				hr = this->SearchFor(szFilename, pszSubItemPath, TRUE, pResultPaths);
				if (hr != S_OK)
				{
					//DPL(0, "Couldn't search for %s in %s!", 2, szFilename, pszSubItemPath);
					LocalFree(pszSubItemPath);
					FindClose(hFind);
					return (hr);
				} // end if (couldn't find paths)

				LocalFree(pszSubItemPath);
				pszSubItemPath = NULL;
			} // end if (this found item is a directory)
			else
			{
				strcpy(szLwrFoundFilename, founddata.cFileName);
				_strlwr(szLwrFoundFilename);

				if (strcmp(szLwrFoundFilename, szLwrSearchFilename) == 0)
				{
					pszSubItemPath = (char*) LocalAlloc(LPTR, dwSubItemPathSize);
					if (pszSubItemPath == NULL)
					{
						FindClose(hFind);
						return (E_OUTOFMEMORY);
					} // end if (couldn't allocate memory)

					strcpy(pszSubItemPath, szSearchDirPath);
					if (! StringEndsWith(szSearchDirPath, "\\", FALSE))
						strcat(pszSubItemPath, "\\");
					strcat(pszSubItemPath, founddata.cFileName);

					hr = pResultPaths->AddString(pszSubItemPath);
					if (hr != S_OK)
					{
						DPL(0, "Couldn't add string %s to result paths!", 1, pszSubItemPath);
						LocalFree(pszSubItemPath);
						FindClose(hFind);
						return (hr);
					} // end if (couldn't add string)

					LocalFree(pszSubItemPath);
					pszSubItemPath = NULL;
				} // end if (this file is a match)
			} // end else (this found item is not a directory)
		} // end if (didn't find current or parent directory)

		if (! FindNextFile(hFind, &founddata))
		{
			hr = GetLastError();

			// If there aren't any more files, get out of the loop.
			if (hr == ERROR_NO_MORE_FILES)
			{
				hr = S_OK;
				break;
			} // end if (that was the last file)
		} // end if (finding next file failed)
		else
			hr = S_OK;

	} // end do
	while (hr == S_OK);


	if (! FindClose(hFind))
	{
		hr = GetLastError();

		DPL(0, "Couldn't close search!", 0);
		return (hr);
	} // end if (couldn't close search)

	return (S_OK);
} // CTNTrackedBinaries::SearchFor
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNBinaryManager::CTNBinaryManager()"
//==================================================================================
// CTNBinaryManager constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNBinaryManager object.  Initializes the data
//				structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNBinaryManager::CTNBinaryManager(void)
{
	//DPL(0, "this = %x, sizeof (this) = %i", 2, this, sizeof (CTNBinaryManager));

	this->m_pPlacedBins = NULL;
	this->m_pSavedBins = NULL;
} //CTNBinaryManager::CTNBinaryManager
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNBinaryManager::~CTNBinaryManager()"
//==================================================================================
// CTNBinaryManager destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNBinaryManager object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNBinaryManager::~CTNBinaryManager(void)
{
	//DPL(0, "this = %x", 1, this);

	if (this->m_pPlacedBins != NULL)
	{
		delete (this->m_pPlacedBins);
		this->m_pPlacedBins = NULL;
	} // end if (allocated a placed bins object)

	if (this->m_pSavedBins != NULL)
	{
		delete (this->m_pSavedBins);
		this->m_pSavedBins = NULL;
	} // end if (allocated a saved bins object)
} // CTNBinaryManager::~CTNBinaryManager
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#ifndef _XBOX // no swapping supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNBinaryManager::SaveSnapshot()"
//==================================================================================
// CTNBinaryManager::SaveSnapshot
//----------------------------------------------------------------------------------
//
// Description: Saves the state of the binaries being managed so they can be
//				restored at a later time.
//
// Arguments:
//	PTNTRACKEDBINARIES pBinsToSave		Pointer to list of tracked binaries to save.
//	char* szBinSnapshotPath				Directory to store the saved binaries.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNBinaryManager::SaveSnapshot(PTNTRACKEDBINARIES pBinsToSave,
										char* szBinSnapshotPath)
{
	HRESULT				hr;
	int					i;
	PTNBINARYLOCSLIST	pTrackedBinary = NULL;
	PTNBINARYLOCSLIST	pSavedBinary = NULL;
	int					j;
	PTNBINARY			pTrackedBinaryLoc = NULL;
	PTNBINARY			pSavedBinaryLoc = NULL;
	char*				pszMungedName = NULL;
	DWORD				dwAttemptNumber = 0;


	if ((this->m_pPlacedBins != NULL) || (this->m_pSavedBins != NULL))
	{
		DPL(0, "This object already has a snapshot stored, can't create another one!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (this object already has a snapshot)

	this->m_pPlacedBins = new (CTNTrackedBinaries);
	if (this->m_pPlacedBins == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	this->m_pSavedBins = new (CTNTrackedBinaries);
	if (this->m_pSavedBins == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)


	// Loop through all the binaries to save, and copy all instances (locations) to
	// our save directory.

	for(i = 0; i < pBinsToSave->Count(); i++)
	{
		// Grab the list of locations for this binary
		pTrackedBinary = (PTNBINARYLOCSLIST) pBinsToSave->GetItem(i);
		if (pTrackedBinary == NULL)
		{
			DPL(0, "Couldn't get binary %i to save!", 1, i);
			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't get item)

		// Add the list to our own so we know where to restore them to
		hr = this->m_pPlacedBins->Add(pTrackedBinary);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add tracked binary %s to placed binaries list!",
				1, pTrackedBinary->m_pszName);
			goto DONE;
		} // end if (couldn't get item)


		// Duplicate the tracked binary object

		pSavedBinary = new (CTNBinaryLocsList);
		if (pSavedBinary == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)

		pSavedBinary->m_pszName = (char*) LocalAlloc(LPTR, (strlen(pTrackedBinary->m_pszName) + 1));
		if (pSavedBinary->m_pszName == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate memory)
		strcpy(pSavedBinary->m_pszName, pTrackedBinary->m_pszName);


		// Loop through all the locations and save them, noting in our duplicated
		// list where we put the backup copy.

		for(j = 0; j < pTrackedBinary->m_bins.Count(); j++)
		{
			// Grab the location
			pTrackedBinaryLoc = (PTNBINARY) pTrackedBinary->m_bins.GetItem(j);
			if (pTrackedBinaryLoc == NULL)
			{
				DPL(0, "Couldn't get binary location %i to save!", 1, j);
				hr = E_FAIL;
				goto DONE;
			} // end if (couldn't get item)

			// Build a name for the object that is based off of the original
			// binary's name, but contains no periods or spaces.
			pszMungedName = (char*) LocalAlloc(LPTR, (strlen(pTrackedBinary->m_pszName) + 1));
			if (pszMungedName == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)

			strcpy(pszMungedName, pTrackedBinary->m_pszName);
			StringReplaceAll(pszMungedName, ".", "_", false, 0, strlen(pszMungedName) + 1);
			StringReplaceAll(pszMungedName, " ", "", false, 0, strlen(pszMungedName) + 1);


			// Build a duplicate object but with the new location.

			pSavedBinaryLoc = new (CTNBinary);
			if (pSavedBinaryLoc == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate object)

			// Yeah, so we allocated more memory than necessary, deal with it.
			// I don't want to bother calculating size.
			pSavedBinaryLoc->m_pszPath = (char*) LocalAlloc(LPTR, (strlen(szBinSnapshotPath) + MAX_PATH + 1));
			if (pSavedBinaryLoc->m_pszPath == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)

			// Keep trying to copy the file to our stored location until we
			// successfully generate a unique name and copy it, or find an
			// identical backup already.
			dwAttemptNumber = 1;
			do
			{
				wsprintf(pSavedBinaryLoc->m_pszPath, "%s%s_%s%i.bak",
						szBinSnapshotPath,
						((StringEndsWith(szBinSnapshotPath, "\\", false)) ? "b": "\\b"),
						pszMungedName,
						dwAttemptNumber);

				// Attempt to load any file with that name already there.  If it
				// exists, determine whether the info is the same.  If the info does
				// match, then we don't have to bother copying the file again, we'll
				// just use the existing one.
				hr = pSavedBinaryLoc->LoadInfo();
				if (hr != S_OK)
				{
					// Assume an error means the file wasn't found.
					// We should be okay to copy the file then.

					if (! CopyFile(pTrackedBinaryLoc->m_pszPath,
									pSavedBinaryLoc->m_pszPath, TRUE))
					{
						DPL(0, "Couldn't copy file %s to %s!", 2,
							pTrackedBinaryLoc->m_pszPath,
							pSavedBinaryLoc->m_pszPath);
						hr = GetLastError();
						goto DONE;
					} // end if (couldn't copy the file)

					//BUGBUG write textfile log info

					// We copied the file successfully.  We're done.
					break;
				} // end if (there wasn't a file by that name already)

				// If we're here, it means found an object with that name already.

				DPL(0, "Binary %s already exists...",
					1, pSavedBinaryLoc->m_pszPath);

				if (pSavedBinaryLoc->IsEqualTo(pTrackedBinaryLoc))
				{
					DPL(0, "...appears to be identical, not copying.", 0);
					break; // stop trying
				} // end if (the binary is the same)

				// We need to make another attempt but with a different
				// number.  Make sure we're not in an endless loop though.

				if (dwAttemptNumber == MAX_NUMBER_ATTEMPTS)
				{
					DPL(0, "ERROR: Something is wrong!  Failed attempt %i!",
						1, dwAttemptNumber);
					DPL(0, "       Make sure you don't have a ton of binaries building up in %s.",
							1, szBinSnapshotPath);
					hr = E_FAIL;
					goto DONE;
				} // end if (exceeded max number of attempts)

				dwAttemptNumber++;

				DPL(0, "...does not appear to be identical, moving to attempt %i.",
					1, dwAttemptNumber);

			} // end do
			while (TRUE);


			hr = pSavedBinary->m_bins.Add(pSavedBinaryLoc);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add tracked binary location %s to list!",
					1, pSavedBinaryLoc->m_pszPath);
				goto DONE;
			} // end if (couldn't get item)

			pSavedBinaryLoc = NULL; // forget about it so we don't free it below
		} // end for (each location of the tracked binary)


		hr = this->m_pSavedBins->Add(pSavedBinary);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add tracked binary %s to placed binaries list!",
				1, pSavedBinary->m_pszName);
			goto DONE;
		} // end if (couldn't get item)

		pSavedBinary = NULL; // forget about it so we don't free it below
	} // end for (each binary to save)


DONE:

	if (pSavedBinaryLoc != NULL)
	{
		delete (pSavedBinaryLoc);
		pSavedBinaryLoc = NULL;
	} // end if (have a saved binary loc)

	if (pszMungedName != NULL)
	{
		LocalFree(pszMungedName);
		pszMungedName = NULL;
	} // end if (have a munged name)

	if (pSavedBinary != NULL)
	{
		delete (pSavedBinary);
		pSavedBinary = NULL;
	} // end if (have a saved binary object)

	return (hr);
} // CTNBinaryManager::SaveSnapshot
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // !	XBOX






#ifndef _XBOX // no swapping supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNBinaryManager::RestoreSnapshot()"
//==================================================================================
// CTNBinaryManager::RestoreSnapshot
//----------------------------------------------------------------------------------
//
// Description: Restores the binaries being managed to the state they were in when
//				the snapshot was taken.
//
// Arguments: None.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNBinaryManager::RestoreSnapshot(void)
{
	HRESULT					hr;
	int						i;
	PTNBINARYLOCSLIST		pPlacedBinLocs = NULL;
	PTNBINARYLOCSLIST		pSavedBinLocs = NULL;
	int						j;
	PTNBINARY				pPlacedBinary = NULL;
	PTNBINARY				pSavedBinary = NULL;


	if ((this->m_pPlacedBins == NULL) || (this->m_pSavedBins == NULL))
	{
		DPL(0, "This object does not have a snapshot stored, can't restore it!", 0);
		//hr = ERROR_BAD_ENVIRONMENT;
		//goto DONE;
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (this object doesn't have a snapshot)


	for(i = 0; i < this->m_pSavedBins->Count(); i++)
	{
		pPlacedBinLocs = (PTNBINARYLOCSLIST) this->m_pPlacedBins->GetItem(i);
		if (pPlacedBinLocs == NULL)
		{
			DPL(0, "Couldn't get placed item %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get item)

		pSavedBinLocs = (PTNBINARYLOCSLIST) this->m_pSavedBins->GetItem(i);
		if (pSavedBinLocs == NULL)
		{
			DPL(0, "Couldn't get saved item %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get item)

		for(j = 0; j < pSavedBinLocs->m_bins.Count(); j++)
		{
			pPlacedBinary = (PTNBINARY) pPlacedBinLocs->m_bins.GetItem(j);
			if (pPlacedBinary == NULL)
			{
				DPL(0, "Couldn't get placed binary %i!", 1, j);
				return (E_FAIL);
			} // end if (couldn't get item)

			pSavedBinary = (PTNBINARY) pSavedBinLocs->m_bins.GetItem(j);
			if (pSavedBinary == NULL)
			{
				DPL(0, "Couldn't get saved binary %i!", 1, j);
				return (E_FAIL);
			} // end if (couldn't get item)

			if (! CopyFile(pSavedBinary->m_pszPath, pPlacedBinary->m_pszPath, FALSE))
			{
				hr = GetLastError();
				DPL(0, "Couldn't restore binary from %s to %s!",
					2, pSavedBinary->m_pszPath, pPlacedBinary->m_pszPath);
				return (hr);
			} // end if (couldn't copy file)
		} // end for (each saved binary)
	} // end for (each saved binary)

	return (S_OK);
} // CTNBinaryManager::RestoreSnapshot
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // no swapping supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNBinaryManager::SwitchBinaryTo()"
//==================================================================================
// CTNBinaryManager::SwitchBinaryTo
//----------------------------------------------------------------------------------
//
// Description: Changes all found instances of the passed in binary to the one
//				located in the path specified.
//
// Arguments:
//	char* szBinaryName		Name of binary to swap.
//	char* szNewBinaryPath	Directory path to retrieve new binary from.
//
// Returns: S_OK if successful, or error code otherwise.
//==================================================================================
HRESULT CTNBinaryManager::SwitchBinaryTo(char* szBinaryName, char* szNewBinaryPath)
{
	//HRESULT					hr;
	PTNBINARYLOCSLIST		pBinaryLocs = NULL;
	char*					pszCopyPath = NULL;
	int						i;
	PTNBINARY				pBinary = NULL;


	//BUGBUG don't save & restore every snapshot item, only the one's SwitchBinaryTo'd.

	if (this->m_pPlacedBins == NULL)
	{
		DPL(0, "No shapshot has been taken, we'd have no way to restore to previous state!", 0);
		return (ERROR_BAD_ENVIRONMENT);
	} // end if (a snapshot hasn't been taken)

	this->m_pPlacedBins->EnterCritSection();

	pBinaryLocs = this->m_pPlacedBins->GetBinary(szBinaryName);
	if (pBinaryLocs == NULL)
	{
		this->m_pPlacedBins->LeaveCritSection();
		DPL(0, "Couldn't get binary %s!", 1, szBinaryName);
		return (E_FAIL);
	} // end if (couldn't find specified binary)

	pszCopyPath = (char*) LocalAlloc(LPTR, strlen(szNewBinaryPath) + strlen(szBinaryName) + 1);
	if (pszCopyPath == NULL)
	{
		return (E_OUTOFMEMORY);
	} // end if (couldn't allocate memory)

	strcpy(pszCopyPath, szNewBinaryPath);
	if (! StringEndsWith(szNewBinaryPath, "\\", false))
		strcat(pszCopyPath, "\\");

	pBinaryLocs->m_bins.EnterCritSection();
	for(i = 0; i < pBinaryLocs->m_bins.Count(); i++)
	{
		pBinary = (PTNBINARY) pBinaryLocs->m_bins.GetItem(i);
		if (pBinary == NULL)
		{
			pBinaryLocs->m_bins.LeaveCritSection();
			DPL(0, "Couldn't get binary location %i!", 1, i);
			return (E_FAIL);
		} // end if (couldn't get item)

		if (! CopyFile(pszCopyPath, pBinary->m_pszPath, FALSE))
		{
			HRESULT		hr;


			hr = GetLastError();

			pBinaryLocs->m_bins.LeaveCritSection();
			DPL(0, "Couldn't copy file from %s to %s!",
				2, pszCopyPath, pBinary->m_pszPath);
			return (hr);
		} // end if (couldn't replace file)
	} // end for (each location in which we found the binary)
	pBinaryLocs->m_bins.LeaveCritSection();


	return (S_OK);
} // CTNBinaryManager::SwitchBinaryTo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX

