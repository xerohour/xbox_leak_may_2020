//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#ifdef _XBOX // conversion functions
#include "convhelp.h"
#endif

#include "main.h"
#ifndef NO_TNCOMMON_DEBUG_SPEW
#include "debugprint.h"
#endif // ! NO_TNCOMMON_DEBUG_SPEW

#include "verutils.h"




#undef DEBUG_SECTION
#define DEBUG_SECTION	"VerGetOSType()"
//==================================================================================
// VerGetOSType
//----------------------------------------------------------------------------------
//
// Description: Returns the OS type ID for the OS described by the passed in
//				OSVERSIONINFO structure.
//
// Arguments:
//	OSVERSIONINFO* pOSVersion	Pointer to structure describing OS.
//
// Returns: ID of OS, or VEROSID_UNKNOWN if couldn't determine or an error occurred.
//==================================================================================
DWORD VerGetOSType(OSVERSIONINFO* pOSVersion)
{
	if (pOSVersion->dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if ((pOSVersion->dwMajorVersion == 4) &&
			(pOSVersion->dwMinorVersion == 0) &&
			(pOSVersion->dwBuildNumber == 1381))
		{
			return (VEROSID_WINDOWSNT4);
		}
		else if (pOSVersion->dwMajorVersion == 5)
		{
			return (VEROSID_WINDOWS2000);
		}
	} // end if (an NT OS)
	else
	{
		if ((HIBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 4) &&
			(LOBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 0) &&
			(LOWORD(pOSVersion->dwBuildNumber) == 950))
		{
			return (VEROSID_WINDOWS95GOLD);
		}
		else if ((HIBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 4) &&
			(LOBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 0) &&
			(LOWORD(pOSVersion->dwBuildNumber) == 1111))
		{
			return (VEROSID_WINDOWS95OSR2);
		}
		else if ((HIBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 4) &&
				(LOBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 10) &&
				(LOWORD(pOSVersion->dwBuildNumber) == 1998))
		{
			return (VEROSID_WINDOWS98GOLD);
		}
		else if ((HIBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 4) &&
				(LOBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 10) &&
				(LOWORD(pOSVersion->dwBuildNumber) == 2222))
		{
			return (VEROSID_WINDOWS98SE);
		}
		else if ((HIBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 4) &&
				(LOBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 90))
		{
			return (VEROSID_MILLENNIUM);
		}
	} // end else (a Win9x OS)

	return (VEROSID_UNKNOWN);
} // VerGetOSType
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"VerGetCurrentOSType()"
//==================================================================================
// VerGetCurrentOSType
//----------------------------------------------------------------------------------
//
// Description: Returns the OS type ID for the OS this machine is currently running.
//
// Arguments: None.
//
// Returns: ID of OS, or VEROSID_UNKNOWN if couldn't determine or an error occurred.
//==================================================================================
DWORD VerGetCurrentOSType(void)
{
	OSVERSIONINFO		ovi;


	ZeroMemory(&ovi, sizeof (OSVERSIONINFO));
	ovi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	if (! GetVersionEx(&ovi))
	{
#ifndef NO_TNCOMMON_DEBUG_SPEW
		HRESULT		hr;


		hr = GetLastError();

		DPL(0, "Couldn't get OS version information!  %e", 1, hr);
#endif // NO_TNCOMMON_DEBUG_SPEW
		return (VEROSID_UNKNOWN);
	} // end if (couldn't get OS version info)

	return (VerGetOSType(&ovi));
} // VerGetCurrentOSType
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"VerGetStrFromOSVersion()"
//==================================================================================
// VerGetStrFromOSVersion
//----------------------------------------------------------------------------------
//
// Description: Converts the passed in OS version information into a string.
//
// Arguments:
//	OSVERSIONINFO* pOSVersion	Pointer to OS information to convert to a string.
//	char* szString				The string to store the results in.
//
// Returns: None.
//==================================================================================
#ifndef _XBOX
void VerGetStrFromOSVersion(OSVERSIONINFO* pOSVersion, char* szString)
#else
void VerGetStrFromOSVersion(OSVERSIONINFOA* pOSVersion, char* szString)
#endif
{
	char	szKnownOS[256];

	ZeroMemory(szKnownOS, 256);

	if (pOSVersion->dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		wsprintf(szString, "%i.%i.%i",
				pOSVersion->dwMajorVersion,
				pOSVersion->dwMinorVersion,
				pOSVersion->dwBuildNumber);

		if ((pOSVersion->dwMajorVersion == 4) &&
			(pOSVersion->dwMinorVersion == 0) &&
			(pOSVersion->dwBuildNumber == 1381))
		{
			strcpy(szKnownOS, "Windows NT4");
		}
		else if (pOSVersion->dwMajorVersion == 5)
		{
			wsprintf(szKnownOS, "Windows 2000 build %i", pOSVersion->dwBuildNumber);
		}
		else
		{
			strcpy(szKnownOS, "Unknown NT based OS");
		}
	} // end if (an NT OS)
	else
	{
		wsprintf(szString, "%i.%i.%i",
				HIBYTE(HIWORD(pOSVersion->dwBuildNumber)),
				LOBYTE(HIWORD(pOSVersion->dwBuildNumber)),
				LOWORD(pOSVersion->dwBuildNumber));

		if ((HIBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 4) &&
			(LOBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 0) &&
			(LOWORD(pOSVersion->dwBuildNumber) == 950))
		{
			strcpy(szKnownOS, "Windows 95 Gold");
		}
		else if ((HIBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 4) &&
			(LOBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 0) &&
			(LOWORD(pOSVersion->dwBuildNumber) == 1111))
		{
			strcpy(szKnownOS, "Windows 95 OSR2");
		}
		else if ((HIBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 4) &&
				(LOBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 10) &&
				(LOWORD(pOSVersion->dwBuildNumber) == 1998))
		{
			strcpy(szKnownOS, "Windows 98 Gold");
		}
		else if ((HIBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 4) &&
				(LOBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 10) &&
				(LOWORD(pOSVersion->dwBuildNumber) == 2222))
		{
			strcpy(szKnownOS, "Windows 98 SE");
		}
		else if ((HIBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 4) &&
				(LOBYTE(HIWORD(pOSVersion->dwBuildNumber)) == 90))
		{
			wsprintf(szKnownOS, "Millennium build %i", LOWORD(pOSVersion->dwBuildNumber));
		}
		else
		{
			strcpy(szKnownOS, "Unknown 9x based OS");
		}
	} // end else (a Win9x OS)

	if (strcmp(pOSVersion->szCSDVersion, "") != 0)
	{
		// If there was known OS info there before, tack on a separator
		//if (strcmp(szKnownOS, "") != 0)
			strcat(szKnownOS, " - ");
		
			strcat(szKnownOS, pOSVersion->szCSDVersion);
	} // end if (there's a service pack/extra info string)

	//if (strcmp(szKnownOS, "") != 0)
	//{
		strcat(szString, " (");
		strcat(szString, szKnownOS);
		strcat(szString, ")");
	//} // end if (we know about this OS/have an extra description string)
} // VerGetStrFromOSVersion
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"VerGetStrFromCurrentOSVersion()"
//==================================================================================
// VerGetStrFromCurrentOSVersion
//----------------------------------------------------------------------------------
//
// Description: Converts the OS version information for the current machine into a
//				string.
//
// Arguments:
//	char* szString		The string to store the results in.
//
// Returns: None.
//==================================================================================
void VerGetStrFromCurrentOSVersion(char* szString)
{
	OSVERSIONINFO		ovi;
#ifdef _XBOX
	OSVERSIONINFOA		osAnsi;
#endif

	ZeroMemory(&ovi, sizeof (OSVERSIONINFO));
	ovi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	if (! GetVersionEx(&ovi))
	{
#ifndef NO_TNCOMMON_DEBUG_SPEW
		HRESULT		hr;


		hr = GetLastError();

		DPL(0, "Couldn't get string from OS version!  %e", 1, hr);
#endif // NO_TNCOMMON_DEBUG_SPEW
		return;
	} // end if (couldn't get OS version info)

#ifdef _XBOX
	osAnsi.dwBuildNumber = ovi.dwBuildNumber;
	osAnsi.dwMajorVersion = ovi.dwMajorVersion;
	osAnsi.dwMinorVersion = ovi.dwMinorVersion;
	osAnsi.dwOSVersionInfoSize = ovi.dwOSVersionInfoSize;
	osAnsi.dwPlatformId = ovi.dwPlatformId;
	wcstombs(osAnsi.szCSDVersion, ovi.szCSDVersion, wcslen(ovi.szCSDVersion));
	osAnsi.szCSDVersion[wcslen(ovi.szCSDVersion)] = 0;
	VerGetStrFromOSVersion(&osAnsi, szString);
#else
	VerGetStrFromOSVersion(&ovi, szString);
#endif
} // VerGetStrFromCurrentOSVersion
#undef DEBUG_SECTION
#define DEBUG_SECTION	""



#ifndef _XBOX // GetFileVersionInfo not supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"VerGetBinaryVersionInfo()"
//==================================================================================
// VerGetBinaryVersionInfo
//----------------------------------------------------------------------------------
//
// Description: Retrieves the version information for the binary specified, if
//				possible.  If none exists or an error occurred, FALSE is returned.
//				Any of the 3 output parameters can be NULL, in which case no data
//				will be returned for that item (duh).
//
// Arguments:
//	char* szFile			The string to store the results in.
//	DWORD* pdwVersionMS		Optional pointer to store most significant version
//							digits.
//	DWORD* pdwVersionLS		Optional pointer to store least significant version
//							digits.
//	BOOL* pfDebug			Optional pointer to boolean which will be set to TRUE if
//							the binary is a debug version, FALSE otherwise.
//
// Returns: TRUE if got version information, FALSE otherwise.
//==================================================================================
BOOL VerGetBinaryVersionInfo(char* szFile, DWORD* pdwVersionMS, DWORD* pdwVersionLS,
							BOOL* pfDebug)
{
	BOOL				fResult = FALSE;
	HRESULT				hr;
	PVOID				pvVersionInfo = NULL;
	DWORD				dwVersionInfoSize = 0;
	DWORD				dwReserved;
	VS_FIXEDFILEINFO*	pVSFFI = NULL;
	UINT				uiSize;


	// Retrieve version information about the file, if available.
	dwVersionInfoSize = GetFileVersionInfoSize(szFile, &dwReserved);
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
			DPL(0, "%s does not contain version information.", 1, szFile);
		} // end if (file doesn't contain version information)
		else
		{
			DPL(0, "GetFileVersionInfoSize for %s failed!  %e", 2, szFile, hr);
		} // end else (other kind of error)

		goto DONE;
	} // end if (could get file version info size)


	pvVersionInfo = LocalAlloc(LPTR, dwVersionInfoSize);
	if (pvVersionInfo == NULL)
		goto DONE;

	if (! GetFileVersionInfo(szFile, 0, dwVersionInfoSize, pvVersionInfo))
	{
		hr = GetLastError();
		DPL(0, "GetFileVersionInfo for %s failed!  %e", 2, szFile, hr);
		goto DONE;
	} // end if (getting file version info failed)

	if (! VerQueryValue(pvVersionInfo, "\\", (PVOID*) &pVSFFI, &uiSize))
	{
		hr = GetLastError();
		DPL(0, "VerQueryValue for %s failed!  %e", 1, szFile);
		goto DONE;
	} // end if (getting version value failed)


	// Store the results.

	if (pdwVersionMS != NULL)
		(*pdwVersionMS) = pVSFFI->dwFileVersionMS;

	if (pdwVersionLS != NULL)
		(*pdwVersionLS) = pVSFFI->dwFileVersionLS;

	if (pfDebug != NULL)
		(*pfDebug) = (pVSFFI->dwFileFlags & VS_FF_DEBUG) ? TRUE : FALSE;



	// If we made it here, everything's cool.
	fResult = TRUE;

	
DONE:

	if (pvVersionInfo != NULL)
	{
		LocalFree(pvVersionInfo);
		pvVersionInfo = NULL;
	} // end if (have version info buffer)


	return (fResult);
} // VerGetBinaryVersionInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif