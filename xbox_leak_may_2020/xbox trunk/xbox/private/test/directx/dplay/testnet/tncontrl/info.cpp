//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#ifndef _XBOX
#include <winsock.h>
#else // ! XBOX
#include <stdio.h>		// Needed for ANSI/Unicode conversion
#include <stdlib.h>		// Needed for ANSI/Unicode conversion
#include <winsockx.h>	// Needed for XnetInitialize
#endif // XBOX


#include <wsipx.h>

#ifndef _XBOX // no TAPI supported
#include <tapi.h>
#endif

#include <mmreg.h> // NT BUILD requires this before dsound.h
#include <dsound.h>


// We set the version to 4 so we don't go getting extra elements in our RASCONN
// structure (otherwise we might build with a larger size that earlier versions of
// RAS barf on)
#ifdef WINVER
#undef WINVER
#endif // WINVER
#define WINVER	0x0400
#include <ras.h>
#include <raserror.h>


#include "..\tncommon\debugprint.h"
#include "..\tncommon\cppobjhelp.h"
#include "..\tncommon\linklist.h"
#include "..\tncommon\strutils.h"
#include "..\tncommon\verutils.h"

#include "tncontrl.h"
#include "main.h"

#include "tapidevs.h"
#include "faultsim.h"
#include "faultsimimtest.h"

#include "info.h"





//==================================================================================
// Defines
//==================================================================================
#define REQUIRED_TAPI_VERSION			0x00010003 // v1.3
//#define REQUIRED_TAPI_VERSION			0x00010004 // v1.4
//#define REQUIRED_TAPI_VERSION			0x00020000 // v2.0

#define MAX_TAPI_VERSION				REQUIRED_TAPI_VERSION
//#define MAX_TAPI_VERSION				TAPI_CURRENT_VERSION


#define LINEDEVCAPS_BUFFER_INCREMENT	1024 // in bytes




//==================================================================================
// RAS function type definitions
//==================================================================================
typedef DWORD	(FAR PASCAL *PRASENUMCONNECTIONSAPROC)		(LPRASCONNA, LPDWORD, LPDWORD);
typedef DWORD	(FAR PASCAL *PRASGETCONNECTSTATUSAPROC)		(HRASCONN, LPRASCONNSTATUSA);



//==================================================================================
// TAPI function type definitions
//==================================================================================
#ifndef _XBOX // no TAPI supported
typedef LONG	(FAR PASCAL *PLINEINITIALIZEPROC)			(LPHLINEAPP, HINSTANCE,
															LINECALLBACK, LPCSTR,
															LPDWORD);
typedef LONG	(FAR PASCAL *PLINENEGOTIATEAPIVERSIONPROC)	(HLINEAPP, DWORD, DWORD,
															DWORD, LPDWORD,
															LPLINEEXTENSIONID);
typedef LONG	(FAR PASCAL *PLINEGETDEVCAPSPROC)			(HLINEAPP, DWORD, DWORD,
															DWORD, LPLINEDEVCAPS);
typedef LONG	(FAR PASCAL *PLINESHUTDOWNPROC)				(HLINEAPP);
#endif // ! XBOX


//==================================================================================
// DSound function type definitions
//==================================================================================
#ifndef _XBOX // no DirectSound callbacks for now
typedef HRESULT		(FAR PASCAL *PDIRECTSOUNDENUMERATEAPROC)	(LPDSENUMCALLBACKA,
																LPVOID);
#endif // ! XBOX





//==================================================================================
// Local structures
//==================================================================================
typedef struct tagTAPIDEVICECONTEXT
{
	PTNTAPIDEVICE	pDevice; // device we're looking for
	BOOL			fInSection; // whether we're in the section or not
} TAPIDEVICECONTEXT, * PTAPIDEVICECONTEXT;

typedef struct tagCOMPORTCONTEXT
{
	PTNCOMPORT		pCOMPort; // COM port object we're looking for
	BOOL			fInSection; // whether we're in the section or not
} COMPORTCONTEXT, * PCOMPORTCONTEXT;



//==================================================================================
// Local prototypes
//==================================================================================
void FAR PASCAL InfoLineCallbackProc(DWORD hDevice, DWORD dwMsg, DWORD_PTR dwContext,
									DWORD_PTR dwParam1, DWORD_PTR dwParam2, DWORD_PTR dwParam3);

HRESULT SearchForTAPIDeviceReadFileCB(char* szLine, PFILELINEINFO pInfo,
									PVOID pvContext, BOOL* pfStopReading);
HRESULT SearchForCOMPortReadFileCB(char* szLine, PFILELINEINFO pInfo,
									PVOID pvContext, BOOL* pfStopReading);

BOOL CALLBACK DSEnumCallback(LPGUID lpGuid, LPCSTR lpcstrDescription,
							LPCSTR lpcstrModule, LPVOID lpvContext);





#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::CTNMachineInfo()"
//==================================================================================
// CTNMachineInfo constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNMachineInfo object.  Initializes the data structures.
//
// Arguments: None.
//
// Returns: None (just the object).
//==================================================================================
CTNMachineInfo::CTNMachineInfo(void)
{
	//DPL(0, "this = %x, sizeof (this) = %u", 2, this, sizeof (CTNMachineInfo));


	ZeroMemory(this->m_szComputerName, MAX_COMPUTERNAME_SIZE * sizeof (char));
	ZeroMemory(this->m_szUserName, MAX_USERNAME_SIZE * sizeof (char));
#ifndef _XBOX
	ZeroMemory(&(this->m_os), sizeof (OSVERSIONINFO));
	this->m_os.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
#else
	ZeroMemory(&(this->m_os), sizeof (OSVERSIONINFOA));
	this->m_os.dwOSVersionInfoSize = sizeof (OSVERSIONINFOA);
#endif

	this->m_wProcessorArchitecture = 0;
	this->m_dwNumberOfProcessors = 0;
	ZeroMemory(&(this->m_memstats), sizeof (MEMORYSTATUS));
	this->m_wWinSock = 0;
	this->m_fIPXInstalled = FALSE;
	this->m_fFaultSimIMTestAvailable = FALSE;

	this->m_pvUserData = NULL;
} // CTNMachineInfo::CTNMachineInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::~CTNMachineInfo()"
//==================================================================================
// CTNMachineInfo destructor
//----------------------------------------------------------------------------------
//
// Description: Destroys the CTNMachineInfo object and any memory it may have
//				allocated during its life.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNMachineInfo::~CTNMachineInfo(void)
{
	//DPL(0, "this = %x", 1, this);
} // CTNMachineInfo::~CTNMachineInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::IsNTBasedOS()"
//==================================================================================
// CTNMachineInfo::IsNTBasedOS
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if this info represents a machine that is using a
//				Windows NT based operating system (NT4, Windows 2000 Professional).
//
// Arguments: None.
//
// Returns: TRUE if machine is NT, FALSE otherwise.
//==================================================================================
BOOL CTNMachineInfo::IsNTBasedOS(void)
{
	if (this->m_os.dwPlatformId == VER_PLATFORM_WIN32_NT)
		return (TRUE);

	return (FALSE);
} // CTNMachineInfo::IsNTBasedOS
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::IsMillenniumOS()"
//==================================================================================
// CTNMachineInfo::IsMillenniumOS
//----------------------------------------------------------------------------------
//
// Description: Returns TRUE if this info represents a machine that is using
//				Windows Millennium Edition.
//
// Arguments: None.
//
// Returns: TRUE if machine is Windows Millennium Edition, FALSE otherwise.
//==================================================================================
BOOL CTNMachineInfo::IsMillenniumOS(void)
{
	// 9x version 4.90.xxxx

	if (this->m_os.dwPlatformId == VER_PLATFORM_WIN32_NT)
		return (FALSE);

	if (HIBYTE(HIWORD(this->m_os.dwBuildNumber)) != 4)
		return (FALSE);

	if (LOBYTE(HIWORD(this->m_os.dwBuildNumber)) != 90)
		return (FALSE);

	return (TRUE);
} // CTNMachineInfo::IsMillenniumOS
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::GetOSString()"
//==================================================================================
// CTNMachineInfo::GetOSString
//----------------------------------------------------------------------------------
//
// Description: Copies a string representation of the machine's OS version/type into
//				the buffer provided.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMachineInfo::GetOSString(char* szString)
{
	VerGetStrFromOSVersion(&(this->m_os), szString);

	return (S_OK);
} // CTNMachineInfo::GetOSString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::LoadLocalMachineInfo()"
//==================================================================================
// CTNMachineInfo::LoadLocalMachineInfo
//----------------------------------------------------------------------------------
//
// Description: Fills in this object with the information pertaining to this
//		      machine.
//
// Arguments:
//	char* szTestnetRootPath									Path to root testnet
//															directory.  Should end
//															in a backslash.
//	PTNADDIMPORTANTBINARIESPROC pfnAddImportantBinaries		Pointer to optional
//															callback to use to
//															retrieve a list of
//															binaries relevant to the
//															upcoming testing.
//	int iComputerNameUniqueness								Value used to generate a
//															unique computer name
//															even when multiple
//															instances are on same
//															machine.
//
// Returns: S_OK if successful, failure code otherwise.
//==================================================================================
HRESULT CTNMachineInfo::LoadLocalMachineInfo(char* szTestnetRootPath,
											PTNADDIMPORTANTBINARIESPROC pfnAddImportantBinaries,
											int iComputerNameUniqueness)
{
	HRESULT			hr;
	char			szNumber[32];
#ifndef _XBOX
	DWORD			dwSize;				// GetComputerName not supported
	SYSTEM_INFO		systeminfo;			// SYSTEM_INFO not supported
#else // ! XBOX
	char			szHostName[MAX_COMPUTERNAME_SIZE + 32];
	OSVERSIONINFO	osWide;
	WCHAR			szWideNumber[256];
#endif // XBOX

#ifdef DEBUG
	if (this == NULL)
	{
		DPL(0, "Object pointer is NULL!", 0);
		hr = ERROR_BAD_ENVIRONMENT;
		goto DONE;
	} // end if (bad object pointer)

	if (szTestnetRootPath == NULL)
	{
		DPL(0, "Testnet root path is invalid/NULL!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (path is invalid)

	if (! StringEndsWith(szTestnetRootPath, "\\", TRUE))
	{
		DPL(0, "Testnet root path doesn't end in backslash!", 0);
		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (path is invalid)
#endif // DEBUG


#ifndef _XBOX // GetComputerName not supported
	// Get the user and computer names
	dwSize = MAX_COMPUTERNAME_SIZE;
	if (! GetComputerName(this->m_szComputerName, &dwSize))
	{
		hr = GetLastError();

		DPL(0, "Couldn't get computer name!  %e", 1, hr);

		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't get computer name)
#else // ! XBOX
	// Create a unique hostname by prepending the MAC address with "Xbox"
	gethostname(szHostName, MAX_COMPUTERNAME_SIZE);
	strcpy(this->m_szComputerName, "Xbox");
	strcat(this->m_szComputerName, szHostName);
	DebugPrint("Local host name is %hs\r\n", this->m_szComputerName);
#endif // XBOX


	// If the uniqueness value is not the first one (0), then tack on some
	// qualifiers to the computer name.
	if (iComputerNameUniqueness > 0)
	{
#ifndef _XBOX // Damn ANSI conversion
		wsprintf(szNumber, " #%i", (iComputerNameUniqueness + 1));
#else // ! XBOX
		// Use swprintf and then convert the result ot ANSI
		swprintf(szWideNumber, L" #%i", (iComputerNameUniqueness + 1));
		wcstombs(szNumber, szWideNumber, wcslen(szWideNumber));
		szNumber[wcslen(szWideNumber)] = 0;
#endif // XBOX
		strcat(this->m_szComputerName, szNumber);
	} // end if (not first uniqueness value)


#ifndef _XBOX // GetUserName not supported
	dwSize = MAX_USERNAME_SIZE;
	if (! GetUserName(this->m_szUserName, &dwSize))
	{
		hr = GetLastError();

		DPL(0, "Couldn't get user name!  %e", 1, hr);

		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't get user name)
#else // ! XBOX
#pragma TODO(tristanj, "Hardcoded local username to Xboxuser")
	strcpy(this->m_szUserName, "XboxUser");
#endif // XBOX

#ifndef _XBOX
	// Get OS version.
	// dwOSVersionInfoSize was set in the CTNMachineInfo object's constructor
	if (! GetVersionEx(&(this->m_os)))
	{
		hr = GetLastError();

		DPL(0, "Couldn't get OS version information!  %e", 1, hr);

		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't get computer name)

#else
	ZeroMemory(&osWide, sizeof(OSVERSIONINFO));
	osWide.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	// Get OS version.
	// dwOSVersionInfoSize was set in the CTNMachineInfo object's constructor
	if (! GetVersionEx(&osWide))
	{
		hr = GetLastError();

		DPL(0, "Couldn't get OS version information!  %e", 1, hr);

		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't get computer name)

	this->m_os.dwBuildNumber = osWide.dwBuildNumber;
	this->m_os.dwMajorVersion = osWide.dwMajorVersion;
	this->m_os.dwMinorVersion = osWide.dwMinorVersion;
	this->m_os.dwOSVersionInfoSize = osWide.dwOSVersionInfoSize;
	this->m_os.dwPlatformId = osWide.dwPlatformId;
	wcstombs(this->m_os.szCSDVersion, osWide.szCSDVersion, wcslen(osWide.szCSDVersion));
	this->m_os.szCSDVersion[wcslen(osWide.szCSDVersion)] = 0;
#endif

	// The extra info string often seems to have spaces in it, so get rid of those
	StringPopLeadingChars(this->m_os.szCSDVersion, " ", TRUE);
	StringPopTrailingChars(this->m_os.szCSDVersion, " ", TRUE);


	// Get information on the processor(s).

#ifndef _XBOX // GetSystemInfo not supported
	ZeroMemory(&systeminfo, sizeof (SYSTEM_INFO));
	GetSystemInfo(&systeminfo);
	this->m_wProcessorArchitecture = systeminfo.wProcessorArchitecture;
	this->m_dwNumberOfProcessors = systeminfo.dwNumberOfProcessors;
#else // ! XBOX
	this->m_wProcessorArchitecture = PROCESSOR_ARCHITECTURE_INTEL;
	this->m_dwNumberOfProcessors = 1;
#endif // XBOX


	// Get the current memory statistics
	GlobalMemoryStatus(&(this->m_memstats));


	hr = this->LoadLocalWinSockInfo();
	if (hr != S_OK)
	{
		DPL(0, "Loading local WinSock info failed!", 0);
		goto DONE;
	} // end if (couldn't load WinSock info)

	hr = this->LoadLocalRASInfo();
	if (hr != S_OK)
	{
		DPL(0, "Loading local RAS info failed!", 0);
		goto DONE;
	} // end if (couldn't load RAS info)

	hr = this->LoadLocalTAPIInfo(szTestnetRootPath);
	if (hr != S_OK)
	{
		DPL(0, "Loading local TAPI info failed!", 0);
		goto DONE;
	} // end if (couldn't load TAPI info)

	hr = this->LoadLocalCOMPortInfo(szTestnetRootPath);
	if (hr != S_OK)
	{
		DPL(0, "Loading local COM port info failed!", 0);
		goto DONE;
	} // end if (couldn't load COM port info)

	hr = this->LoadLocalSoundCardInfo();
	if (hr != S_OK)
	{
		DPL(0, "Loading local sound card info failed!", 0);
		goto DONE;
	} // end if (couldn't load sound card info)

	hr = this->GetFaultSimCapabilities();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't get fault simulator capabilites!", 0);
		goto DONE;
	} // end if (couldn't get fault sims)

	if (pfnAddImportantBinaries != NULL)
	{
		hr = pfnAddImportantBinaries(&(this->m_binaries));
		if (hr != S_OK)
		{
			DPL(0, "Module's AddImportantBinaries callback failed!", 0);
			goto DONE;
		} // end if (module's callback failed)
	} // end if (there are important binaries to get)


DONE:

	return (hr);
} // CTNMachineInfo::LoadLocalMachineInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::PackIntoBuffer()"
//==================================================================================
// CTNMachineInfo::PackIntoBuffer
//----------------------------------------------------------------------------------
//
// Description: Flattens this list into a contiguous buffer for easy transmission.
//				If the pointer to store the results in is NULL, then no memory
//				is copied, but the buffer size is still set to the amount of memory
//				required.
//
// Arguments:
//	PVOID pvBuffer		Pointer to buffer to use.
//	DWORD* pdwSize		Pointer to a DWORD with the size of the buffer, or will be
//						set to the size required.
//
// Returns: S_OK if successful, failure code otherwise.
//==================================================================================
HRESULT CTNMachineInfo::PackIntoBuffer(PVOID pvBuffer, DWORD* pdwSize)
{
	HRESULT		hr;
	DWORD		dwSize = 0;
	LPBYTE		lpCurrentItem = NULL;
	DWORD		dwIPListSize = 0;
	DWORD		dwTAPIDevListSize = 0;
	DWORD		dwCOMPortListSize = 0;
	DWORD		dwSoundCardsListSize = 0;
	DWORD		dwBinariesSize = 0;


	// Ignore errors, assume BUFFER_TOO_SMALL
	this->m_ipaddrs.PackIntoBuffer(NULL, &dwIPListSize);
	this->m_TAPIdevices.PackIntoBuffer(NULL, &dwTAPIDevListSize);
	this->m_COMports.PackIntoBuffer(NULL, &dwCOMPortListSize);
	this->m_soundcards.PackIntoBuffer(NULL, &dwSoundCardsListSize);
	this->m_binaries.PackIntoBuffer(NULL, &dwBinariesSize);

#ifdef _XBOX
//	DebugPrint("m_ipaddrs size:             %u\r\n", dwIPListSize);
//	DebugPrint("m_TAPIdevices size:         %u\r\n", dwTAPIDevListSize);
//	DebugPrint("m_COMports size:            %u\r\n", dwCOMPortListSize);
//	DebugPrint("m_soundcards size:          %u\r\n", dwSoundCardsListSize);
//	DebugPrint("m_binaries size:            %u\r\n", dwBinariesSize);
#endif

	// How big a buffer will we need?
	dwSize = sizeof (char) * MAX_COMPUTERNAME_SIZE
			+ sizeof (char) * MAX_USERNAME_SIZE
			+ sizeof (OSVERSIONINFOA)
			+ sizeof (WORD) // processor type
			+ sizeof (DWORD) // number of processors
			+ sizeof (MEMORYSTATUS)
			+ sizeof (WORD) // winsock version
			+ sizeof (BOOL) // IPX installed
			+ sizeof (BOOL) // IMTest available
			+ sizeof (DWORD) + dwIPListSize
			+ sizeof (DWORD) + dwTAPIDevListSize
			+ sizeof (DWORD) + dwCOMPortListSize
			+ sizeof (DWORD) + dwSoundCardsListSize
			+ sizeof (DWORD) + dwBinariesSize;

#ifdef _XBOX
//	DebugPrint("MAX_COMPUTERNAME_SIZE size: %u\r\n", sizeof (char) * MAX_COMPUTERNAME_SIZE);
//	DebugPrint("MAX_USERNAME_SIZE size:     %u\r\n", sizeof (char) * MAX_USERNAME_SIZE);
//	DebugPrint("OSVERSIONINFO size:         %u\r\n", sizeof (OSVERSIONINFOA));
//	DebugPrint("Processor type size:        %u\r\n", sizeof (WORD));
//	DebugPrint("Number of processors size:  %u\r\n", sizeof (DWORD));
//	DebugPrint("Memory status size:         %u\r\n", sizeof (MEMORYSTATUS));
//	DebugPrint("Winsock version size:       %u\r\n", sizeof (WORD));
//	DebugPrint("IPX installed size:         %u\r\n", sizeof (BOOL));
//	DebugPrint("IM test available size:     %u\r\n", sizeof (BOOL));
//	DebugPrint("Total size:                 %u\r\n", dwSize);
#endif

	(*pdwSize) = dwSize;

	// If the caller just wants the buffer size, we're done.
	if (pvBuffer == NULL)
		return (ERROR_BUFFER_TOO_SMALL);


	lpCurrentItem = (LPBYTE) pvBuffer; // start at beginning

	CopyAndMoveDestPointer(lpCurrentItem, &(this->m_szComputerName),
							(sizeof (char) * MAX_COMPUTERNAME_SIZE));
	CopyAndMoveDestPointer(lpCurrentItem, &(this->m_szUserName),
							(sizeof (char) * MAX_USERNAME_SIZE));
#ifndef _XBOX
	CopyAndMoveDestPointer(lpCurrentItem, &(this->m_os), (sizeof (OSVERSIONINFO)));
#else
	CopyAndMoveDestPointer(lpCurrentItem, &(this->m_os), (sizeof (OSVERSIONINFOA)));
#endif
	CopyAndMoveDestPointer(lpCurrentItem, &(this->m_wProcessorArchitecture), (sizeof (WORD)));
	CopyAndMoveDestPointer(lpCurrentItem, &(this->m_dwNumberOfProcessors), (sizeof (DWORD)));
	CopyAndMoveDestPointer(lpCurrentItem, &(this->m_memstats), (sizeof (MEMORYSTATUS)));
	CopyAndMoveDestPointer(lpCurrentItem, &(this->m_wWinSock), (sizeof (WORD)));

	CopyAndMoveDestPointer(lpCurrentItem, &(this->m_fIPXInstalled), (sizeof (BOOL)));
	CopyAndMoveDestPointer(lpCurrentItem, &(this->m_fFaultSimIMTestAvailable), (sizeof (BOOL)));

	CopyAndMoveDestPointer(lpCurrentItem, &(dwIPListSize), (sizeof (DWORD)));
	hr = this->m_ipaddrs.PackIntoBuffer(lpCurrentItem, &dwIPListSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't pack IP address list into buffer!", 0);
		return (hr);
	} // end if (couldn't pack IP address list into buffer)
	lpCurrentItem += dwIPListSize;

	CopyAndMoveDestPointer(lpCurrentItem, &(dwTAPIDevListSize), (sizeof (DWORD)));
	hr = this->m_TAPIdevices.PackIntoBuffer(lpCurrentItem, &dwTAPIDevListSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't pack TAPI device list into buffer!", 0);
		return (hr);
	} // end if (couldn't pack TAPI device list into buffer)
	lpCurrentItem += dwTAPIDevListSize;

	CopyAndMoveDestPointer(lpCurrentItem, &(dwCOMPortListSize), (sizeof (DWORD)));
	hr = this->m_COMports.PackIntoBuffer(lpCurrentItem, &dwCOMPortListSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't pack COM port list into buffer!", 0);
		return (hr);
	} // end if (couldn't pack COM port list into buffer)
	lpCurrentItem += dwCOMPortListSize;

	CopyAndMoveDestPointer(lpCurrentItem, &(dwSoundCardsListSize), (sizeof (DWORD)));
	hr = this->m_soundcards.PackIntoBuffer(lpCurrentItem, &dwSoundCardsListSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't pack sound cards list into buffer!", 0);
		return (hr);
	} // end if (couldn't pack sound cards list into buffer)
	lpCurrentItem += dwSoundCardsListSize;

	CopyAndMoveDestPointer(lpCurrentItem, &(dwBinariesSize), (sizeof (DWORD)));
	hr = this->m_binaries.PackIntoBuffer(lpCurrentItem, &dwBinariesSize);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't pack binaries' info into buffer!", 0);
		return (hr);
	} // end if (couldn't pack binaries info into buffer)
	//lpCurrentItem += dwBinariesSize;

	return (S_OK);
} // CTNMachineInfo::PackIntoBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::UnpackFromBuffer()"
//==================================================================================
// CTNMachineInfo::UnpackFromBuffer
//----------------------------------------------------------------------------------
//
// Description: Converts a previously packed CTNMachineInfo object back from the
//				contiguous buffer.
//
// Arguments:
//	PVOID pvBuffer	Pointer to the packed buffer to read from.
//	DWORD dwSize	Size of the buffer.
//
// Returns: S_OK if successful, failure code otherwise.
//==================================================================================
HRESULT CTNMachineInfo::UnpackFromBuffer(PVOID pvBuffer, DWORD dwSize)
{
	HRESULT		hr;
	DWORD		dwTempSize = 0;
	LPBYTE		lpCurrentItem = NULL;
	DWORD		dwIPListSize = 0;
	DWORD		dwTAPIDevListSize = 0;
	DWORD		dwCOMPortListSize = 0;
	DWORD		dwSoundCardsListSize = 0;
	DWORD		dwBinariesSize = 0;


	// How big a buffer do we expect?
	dwTempSize = sizeof (char) * MAX_COMPUTERNAME_SIZE
				+ sizeof (char) * MAX_USERNAME_SIZE
#ifndef _XBOX
				+ sizeof (OSVERSIONINFO)
#else
				+ sizeof (OSVERSIONINFOA)
#endif
				+ sizeof (WORD) // processor type
				+ sizeof (DWORD) // number of processors
				+ sizeof (MEMORYSTATUS)
				+ sizeof (WORD) // winsock version
				+ sizeof (BOOL) // IPX installed
				+ sizeof (BOOL); // IMTest available

	// Now check to see if we got what we expected so far
	if (dwSize < dwTempSize + sizeof (DWORD))
	{
		DPL(0, "Buffer to unpack machine info from (at fixed) is of unexpected size (%u is < %u)!",
			2, dwSize, dwTempSize);
		return (E_FAIL);
	} // end if (got unexpected size)

	lpCurrentItem = ((LPBYTE) pvBuffer) + dwTempSize;
    //  6/30/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
	dwIPListSize = *((UNALIGNED DWORD*) lpCurrentItem);

	dwTempSize += sizeof (DWORD) + dwIPListSize;

	// And check again
	if (dwSize < dwTempSize + sizeof (DWORD))
	{
		DPL(0, "Buffer to unpack machine info from (at IP list) is of unexpected size (%u is < %u)!",
			2, dwSize, dwTempSize);
		return (E_FAIL);
	} // end if (got unexpected size)

	lpCurrentItem = ((LPBYTE) pvBuffer) + dwTempSize;
    //  6/30/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
	dwTAPIDevListSize = *((UNALIGNED DWORD*) lpCurrentItem);

	dwTempSize += sizeof (DWORD) + dwTAPIDevListSize;

	// And check again
	if (dwSize < dwTempSize + sizeof (DWORD))
	{
		DPL(0, "Buffer to unpack machine info from (at TAPI device list) is of unexpected size (%u is < %u)!",
			2, dwSize, dwTempSize);
		return (E_FAIL);
	} // end if (got unexpected size)

	lpCurrentItem = ((LPBYTE) pvBuffer) + dwTempSize;
    //  6/30/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
	dwCOMPortListSize = *((UNALIGNED DWORD*) lpCurrentItem);

	dwTempSize += sizeof (DWORD) + dwCOMPortListSize;

	// And check again
	if (dwSize < dwTempSize + sizeof (DWORD))
	{
		DPL(0, "Buffer to unpack machine info from (at COM port list) is of unexpected size (%u is < %u)!",
			2, dwSize, dwTempSize);
		return (E_FAIL);
	} // end if (got unexpected size)

	lpCurrentItem = ((LPBYTE) pvBuffer) + dwTempSize;
    //  6/30/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
	dwSoundCardsListSize = *((UNALIGNED DWORD*) lpCurrentItem);

	dwTempSize += sizeof (DWORD) + dwSoundCardsListSize;

	// And check again
	if (dwSize < dwTempSize + sizeof (DWORD))
	{
		DPL(0, "Buffer to unpack machine info from (at sound cards list) is of unexpected size (%u is < %u)!",
			2, dwSize, dwTempSize);
		return (E_FAIL);
	} // end if (got unexpected size)

	lpCurrentItem = ((LPBYTE) pvBuffer) + dwTempSize;
    //  6/30/2000(RichGr) - IA64: Specifying UNALIGNED generates the correct IA64 code for non-8-byte alignment.
	dwBinariesSize = *((UNALIGNED DWORD*) lpCurrentItem);

	dwTempSize += sizeof (DWORD) + dwBinariesSize;

	// Now check to see if we got what we expected
	if (dwSize != dwTempSize)
	{
		DPL(0, "Buffer to unpack machine info from (at binaries list) is of unexpected size (%u is != %u)!",
			2, dwSize, dwTempSize);
		return (E_FAIL);
	} // end if (got unexpected size)


	lpCurrentItem = (LPBYTE) pvBuffer; // start at beginning

	CopyAndMoveSrcPointer(&(this->m_szComputerName), lpCurrentItem,
							(sizeof (char) * MAX_COMPUTERNAME_SIZE));
	CopyAndMoveSrcPointer(&(this->m_szUserName), lpCurrentItem,
							(sizeof (char) * MAX_USERNAME_SIZE));
#ifndef _XBOX
	CopyAndMoveSrcPointer(&(this->m_os), lpCurrentItem, (sizeof (OSVERSIONINFO)));
#else
	CopyAndMoveSrcPointer(&(this->m_os), lpCurrentItem, (sizeof (OSVERSIONINFOA)));
#endif
	CopyAndMoveSrcPointer(&(this->m_wProcessorArchitecture), lpCurrentItem, (sizeof (WORD)));
	CopyAndMoveSrcPointer(&(this->m_dwNumberOfProcessors), lpCurrentItem, (sizeof (DWORD)));
	CopyAndMoveSrcPointer(&(this->m_memstats), lpCurrentItem, (sizeof (MEMORYSTATUS)));
	CopyAndMoveSrcPointer(&(this->m_wWinSock), lpCurrentItem, (sizeof (WORD)));
	CopyAndMoveSrcPointer(&(this->m_fIPXInstalled), lpCurrentItem, (sizeof (BOOL)));
	CopyAndMoveSrcPointer(&(this->m_fFaultSimIMTestAvailable), lpCurrentItem, (sizeof (BOOL)));

	CopyAndMoveSrcPointer(&dwIPListSize, lpCurrentItem, (sizeof (DWORD)));
	if (dwIPListSize > 0)
	{
		hr = this->m_ipaddrs.UnpackFromBuffer(lpCurrentItem, dwIPListSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't unpack IP address list from buffer!", 0);
			return (hr);
		} // end if (failed to unpack from buffer)

		lpCurrentItem += dwIPListSize;
	} // end if (there are actually items in the address list)

	CopyAndMoveSrcPointer(&dwTAPIDevListSize, lpCurrentItem, (sizeof (DWORD)));
	if (dwTAPIDevListSize > 0)
	{
		hr = this->m_TAPIdevices.UnpackFromBuffer(lpCurrentItem, dwTAPIDevListSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't unpack TAPI device list from buffer!", 0);
			return (hr);
		} // end if (failed to unpack from buffer)

		lpCurrentItem += dwTAPIDevListSize;
	} // end if (there are actually items in the devices list)

	CopyAndMoveSrcPointer(&dwCOMPortListSize, lpCurrentItem, (sizeof (DWORD)));
	if (dwCOMPortListSize > 0)
	{
		hr = this->m_COMports.UnpackFromBuffer(lpCurrentItem, dwCOMPortListSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't unpack COM port list from buffer!", 0);
			return (hr);
		} // end if (failed to unpack from buffer)

		lpCurrentItem += dwCOMPortListSize;
	} // end if (there are actually items in the COM ports list)

	CopyAndMoveSrcPointer(&dwSoundCardsListSize, lpCurrentItem, (sizeof (DWORD)));
	if (dwSoundCardsListSize > 0)
	{
		hr = this->m_soundcards.UnpackFromBuffer(lpCurrentItem, dwSoundCardsListSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't unpack sound cards list from buffer!", 0);
			return (hr);
		} // end if (failed to unpack from buffer)

		lpCurrentItem += dwSoundCardsListSize;
	} // end if (there are actually items in the sound cards list)

	CopyAndMoveSrcPointer(&dwBinariesSize, lpCurrentItem, (sizeof (DWORD)));
	if (dwBinariesSize > 0)
	{
		hr = this->m_binaries.UnpackFromBuffer(lpCurrentItem, dwBinariesSize);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't unpack binaries' info list from buffer!", 0);
			return (hr);
		} // end if (failed to unpack from buffer)

		//lpCurrentItem += dwBinariesSize;
	} // end if (there are actually items in the binaries list)

	return (S_OK);
} // CTNMachineInfo::UnpackFromBuffer
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::LoadLocalWinSockInfo()"
//==================================================================================
// CTNMachineInfo::LoadLocalWinSockInfo
//----------------------------------------------------------------------------------
//
// Description: Loads the local machine's WinSock information.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMachineInfo::LoadLocalWinSockInfo(void)
{
	HRESULT		hr;
	WSADATA		wsadata;
	SOCKET		sCheckIPX = INVALID_SOCKET;


	ZeroMemory(&wsadata, sizeof (WSADATA));

	hr = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (hr != S_OK)
	{
		DPL(0, "Couldn't start up WinSock!", 0);
		return (hr);
	} // end if (we couldn't startup WinSock)

	this->m_wWinSock = wsadata.wHighVersion;

#pragma TODO(vanceo, "Load info")

	hr = this->m_ipaddrs.LoadLocalIPs();
	if (hr != S_OK)
	{
		DPL(0, "Couldn't load local IPs!", 0);
		goto DONE;
	} // end if (couldn't load local IPs)

	
	sCheckIPX = socket(AF_IPX, SOCK_DGRAM, NSPROTO_IPX);

	if (sCheckIPX != INVALID_SOCKET)
	{
		closesocket(sCheckIPX); // ignore error
		sCheckIPX = INVALID_SOCKET;

		this->m_fIPXInstalled = TRUE;
	} // end if (successfully created socket)


DONE:

	WSACleanup(); // ignoring error

	return (hr);
} // CTNMachineInfo::LoadLocalWinSockInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::LoadLocalRASInfo()"
//==================================================================================
// CTNMachineInfo::LoadLocalRASInfo
//----------------------------------------------------------------------------------
//
// Description: Loads the local machine's RAS information.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMachineInfo::LoadLocalRASInfo(void)
{
#ifndef _XBOX // no RAS API supported (for now)
	HRESULT						hr;
	HMODULE						hRASAPI32DLL = NULL;
	PRASENUMCONNECTIONSAPROC	pfnRasEnumConnectionsA = NULL;
	PRASGETCONNECTSTATUSAPROC	pfnRasGetConnectStatusA = NULL;
	LPRASCONN					lpRASConns = NULL;
	DWORD						dwSize = 0;
	DWORD						dwNumConnections = 0;
	DWORD						dwTemp;
	RASCONNSTATUS				rasconnstatus;


	// Check for any RAS/DUN connections.  We dynamically link to the RAS DLL
	// because on WinNT 4 (at least) RAS is not installed by default.

	hRASAPI32DLL = LoadLibrary("RASAPI32.DLL");
	if (hRASAPI32DLL == NULL)
	{
		DPL(0, "Couldn't load RASAPI32 DLL, assuming it's not installed.  Skipping.", 0);
	} // end if (couldn't load DLL)
	else
	{
		pfnRasEnumConnectionsA = (PRASENUMCONNECTIONSAPROC) GetProcAddress(hRASAPI32DLL, "RasEnumConnectionsA");
		pfnRasGetConnectStatusA = (PRASGETCONNECTSTATUSAPROC) GetProcAddress(hRASAPI32DLL, "RasGetConnectStatusA");

		if (pfnRasEnumConnectionsA == NULL)
		{
			DPL(0, "Couldn't find RasEnumConnectionsA in RASAPI32 DLL!?  Skipping.", 0);
		} // end if (couldn't find function)
		if (pfnRasGetConnectStatusA == NULL)
		{
			DPL(0, "Couldn't find RasGetConnectStatusA in RASAPI32 DLL!?  Skipping.", 0);
		} // end else if (couldn't find function)
		else
		{
			// We found the functions, so go ahead and check the stuff.


			// Start with one object
			dwSize = sizeof (RASCONN);
			lpRASConns = (LPRASCONN) LocalAlloc(LPTR, dwSize);
			if (lpRASConns == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)

			lpRASConns[0].dwSize = sizeof (RASCONN);


			hr = pfnRasEnumConnectionsA(lpRASConns, &dwSize, &dwNumConnections);
			if (hr != S_OK)
			{
				if (hr != ERROR_BUFFER_TOO_SMALL)
				{
					DPL(0, "Got unexpected error from RasEnumConnections (expected ERROR_BUFFER_TOO_SMALL)!", 0);
					goto DONE;
				} // end if (we couldn't enumerate RAS connections)

				LocalFree(lpRASConns);
				lpRASConns = (LPRASCONN) LocalAlloc(LPTR, dwSize);
				if (lpRASConns == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)

				lpRASConns[0].dwSize = sizeof (RASCONN);
				

				hr = pfnRasEnumConnectionsA(lpRASConns, &dwSize, &dwNumConnections);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't enumerate RAS connections!", 0);
					goto DONE;
				} // end if (we couldn't enumerate RAS connections)
			} // end if (we failed our first attempt to enumerate)

			ZeroMemory(&rasconnstatus, sizeof (RASCONNSTATUS));
			rasconnstatus.dwSize = sizeof (RASCONNSTATUS);
			for (dwTemp = 0; dwTemp < dwNumConnections; dwTemp++)
			{
				DPL(0, "Found RAS connection %s",
					1, lpRASConns[dwTemp].szEntryName);

				hr = pfnRasGetConnectStatusA(lpRASConns[dwTemp].hrasconn, &rasconnstatus);
				if (hr != S_OK)
				{
					DPL(0, "Couldn't get RAS connection status!", 0);
					goto DONE;
				} // end if (we couldn't get RAS connection status)

				if (rasconnstatus.rasconnstate == RASCS_Connected)
				{
					DPL(0, "RAS connection is connected.", 0);
				} // end if (this is connected)
				else if (rasconnstatus.rasconnstate == RASCS_Disconnected)
				{
					DPL(0, "RAS connection is disconnected.", 0);
				} // end if (this is disconnected)
				else
				{
					DPL(0, "RAS connection is in some wierd state.", 0);
				} // end if (this is not a handled state)

				DPL(0, "szDeviceType = %s, szDeviceName = %s",
					2, rasconnstatus.szDeviceType, rasconnstatus.szDeviceName);
			} // end for (each RAS connection)
		} // end else (we found valid functions)
	} // end else (we loaded the RAS DLL)
	

DONE:

	if (lpRASConns != NULL)
	{
		LocalFree(lpRASConns);
		lpRASConns = NULL;
	} // end if (have RAS connections)

	if (hRASAPI32DLL != NULL)
	{
		FreeLibrary(hRASAPI32DLL);
		hRASAPI32DLL = NULL;
	} // end if (have RAS DLL)

	return (hr);
#else // ! XBOX
	return (S_OK);
#endif // XBOX
} // CTNMachineInfo::LoadLocalRASInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::LoadLocalTAPIInfo()"
//==================================================================================
// CTNMachineInfo::LoadLocalTAPIInfo
//----------------------------------------------------------------------------------
//
// Description: Loads the local machine's TAPI information.
//
// Arguments:
//	char* szTestnetRootPath		Path to root testnet directory.  Should end in a
//								backslash.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMachineInfo::LoadLocalTAPIInfo(char* szTestnetRootPath)
{
#ifndef _XBOX // no TAPI supported
	HRESULT							hr;
	HRESULT							temphr;
	HMODULE							hTAPIDLL = NULL;
	PLINEINITIALIZEPROC				pfnLineInitialize = NULL;
	PLINENEGOTIATEAPIVERSIONPROC	pfnLineNegotiateAPIVersion = NULL;
	PLINEGETDEVCAPSPROC				pfnLineGetDevCaps = NULL;
	PLINESHUTDOWNPROC				pfnLineShutdown = NULL;
	char*							pszIniPath = NULL;
	BOOL							fTAPIInitted = FALSE;
	HLINEAPP						hLineApp;
	DWORD							dwNumDevices;
	DWORD							dwTemp;
	DWORD							dwDevTAPIVersion;
	LINEEXTENSIONID					leid;
	LPLINEDEVCAPS					lpLineDevCaps = NULL;
	DWORD							dwLineDevCapsSize = 0;
	char*							pszLineName;
	PTNTAPIDEVICE					pTAPIDev = NULL;
	TAPIDEVICECONTEXT				tdc;


	hTAPIDLL = LoadLibrary("tapi32.dll");
	if (hTAPIDLL == NULL)
	{
		hr = GetLastError();

		DPL(0, "Couldn't load TAPI32.DLL!  %e", 1, hr);
		
		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't load TAPI)

	pfnLineInitialize = (PLINEINITIALIZEPROC) GetProcAddress(hTAPIDLL,
															"lineInitialize");
	if (pfnLineInitialize == NULL)
	{
		hr = GetLastError();

		DPL(0, "Couldn't get \"lineInitialize\" function!  %e", 1, hr);
		
		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't get proc)

	pfnLineNegotiateAPIVersion = (PLINENEGOTIATEAPIVERSIONPROC) GetProcAddress(hTAPIDLL,
																				"lineNegotiateAPIVersion");
	if (pfnLineNegotiateAPIVersion == NULL)
	{
		hr = GetLastError();

		DPL(0, "Couldn't get \"lineNegotiateAPIVersion\" function!  %e", 1, hr);
		
		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't get proc)

	pfnLineGetDevCaps = (PLINEGETDEVCAPSPROC) GetProcAddress(hTAPIDLL,
															"lineGetDevCaps");
	if (pfnLineGetDevCaps == NULL)
	{
		hr = GetLastError();

		DPL(0, "Couldn't get \"lineGetDevCaps\" function!  %e", 1, hr);
		
		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't get proc)

	pfnLineShutdown = (PLINESHUTDOWNPROC) GetProcAddress(hTAPIDLL,
														"lineShutdown");
	if (pfnLineShutdown == NULL)
	{
		hr = GetLastError();

		DPL(0, "Couldn't get \"lineShutdown\" function!  %e", 1, hr);
		
		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't get proc)


	// Build the path to the ini file
	pszIniPath = (char*) LocalAlloc(LPTR, strlen(szTestnetRootPath) + strlen(TAPIDEVICELIST_FILENAME) + 1);
	if (pszIniPath == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	strcpy(pszIniPath, szTestnetRootPath);
	strcat(pszIniPath, TAPIDEVICELIST_FILENAME);


	hr = pfnLineInitialize(&hLineApp, s_hInstance, InfoLineCallbackProc, NULL,
							&dwNumDevices);
	if (hr != 0)
	{
		DPL(0, "Initializing TAPI failed!", 0);

		if (hr == LINEERR_REINIT)
		{
			DPL(0, "TAPI wants to reinitialize (a device was installed possibly), so shutdown all TAPI apps and try again.", 0);
		} // end if (reinit)

		goto DONE;
	} // end if (init failed)

	fTAPIInitted = TRUE;
	DPL(1, "TAPI reports %u devices.", 1, dwNumDevices);


	for(dwTemp = 0; dwTemp < dwNumDevices; dwTemp++)
	{
		hr = pfnLineNegotiateAPIVersion(hLineApp, dwTemp, REQUIRED_TAPI_VERSION,
										MAX_TAPI_VERSION, &dwDevTAPIVersion, &leid);
		if (hr != 0)
		{
			DPL(0, "Negotiating API version with device %u failed (doesn't like v%x - v%x), skipping.  %e",
				4, dwTemp, REQUIRED_TAPI_VERSION, MAX_TAPI_VERSION, hr);

			hr = S_OK;
			continue;
		} // end if (getting version failed)

		// Keep looping until we have a buffer large enough to hold the caps.
		do
		{
			if (lpLineDevCaps != NULL)
			{
				LocalFree(lpLineDevCaps);
				lpLineDevCaps = NULL;
			} // end if (already have buffer)

			dwLineDevCapsSize += LINEDEVCAPS_BUFFER_INCREMENT;
			lpLineDevCaps = (LPLINEDEVCAPS) LocalAlloc(LPTR, dwLineDevCapsSize);
			if (lpLineDevCaps == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)
			lpLineDevCaps->dwTotalSize = dwLineDevCapsSize;

			hr = pfnLineGetDevCaps(hLineApp, dwTemp, dwDevTAPIVersion, 0, lpLineDevCaps);
		} // end do (while need to increase buffer size)
		while (hr == LINEERR_STRUCTURETOOSMALL);

		if (hr != 0)
		{
			DPL(0, "Couldn't get device %u's capabilities!  Skipping.  %e",
				2, dwTemp, hr);
			hr = S_OK;
			continue;
		} // end if (getting caps failed)


		pszLineName = ((char *) lpLineDevCaps) + lpLineDevCaps->dwLineNameOffset;

		if (pszLineName[0] == '\0')
		{
			DPL(0, "Device %u's name was an empty string!", 0);
			hr = E_FAIL;
		    goto DONE;
		} //  end if (line name had no characters)

		// Make sure the device name is null terminated.
		if (pszLineName[lpLineDevCaps->dwLineNameSize - 1] != '\0')
		{
			char*	pszTemp;


			pszTemp = (char*) LocalAlloc(LPTR, lpLineDevCaps->dwLineNameSize + 1);
			if (pszTemp == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)

			CopyMemory(pszTemp, pszLineName, lpLineDevCaps->dwLineNameSize);

			pTAPIDev = new (CTNTAPIDevice)(pszTemp, dwTemp);

			LocalFree(pszTemp);
			pszTemp = NULL;
		} // end if (name isn't NULL terminated)
		else
		{
			pTAPIDev = new (CTNTAPIDevice)(pszLineName, dwTemp);
		} // end else (name is NULL terminated)

		if (pTAPIDev == NULL)
		{
			hr = E_OUTOFMEMORY;
			goto DONE;
		} // end if (couldn't allocate object)


		// Skip non-data-modems.
		if (! (lpLineDevCaps->dwMediaModes & LINEMEDIAMODE_DATAMODEM))
		{
			DPL(2, "Skipping non-data-modem \"%s\" (device %u).",
				2, pTAPIDev->GetString(), dwTemp);

			delete (pTAPIDev);
			pTAPIDev = NULL;

			continue;
		} // end if (not a data-modem)

		DPL(1, "Found data-modem \"%s\" (device %u).", 2, pTAPIDev->GetString(), dwTemp);


		ZeroMemory(&tdc, sizeof (TAPIDEVICECONTEXT));
		tdc.pDevice = pTAPIDev;
		//tdc.fInSection = FALSE;

		hr = FileReadTextFile(pszIniPath, FILERTFO_EXTENDEDINIOPTIONS,
							SearchForTAPIDeviceReadFileCB, &tdc);

		// Hmm.  For now, the file not existing is okay, we'll just treat it as if
		// the device didn't have a number specified
		if (hr == ERROR_FILE_NOT_FOUND)
		{
			DPL(0, "TAPI device list file \"%s\" doesn't exist, so there's no number for device \"%s\".",
				2, pszIniPath, pTAPIDev->GetString());
			hr = S_OK;
		} // end if (file doesn't exist)

		if (hr != S_OK)
		{
			DPL(0, "Couldn't read TAPI device list file \"%s\" to search for device \"%s\"!",
				2, pszIniPath, pTAPIDev->GetString());
			goto DONE;
		} // end if (couldn't allocate object)


		hr = this->m_TAPIdevices.Add(pTAPIDev);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add TAPI device %x (# %u) to list!",
				2, pTAPIDev, dwTemp);
			goto DONE;
		} // end if (couldn't allocate object)

		pTAPIDev = NULL; // forget it, so we don't free it below
	} // end for (each device)


DONE:

	if (fTAPIInitted)
	{
		temphr = pfnLineShutdown(hLineApp);
		if (temphr != 0)
		{
			DPL(0, "Couldn't shutdown TAPI!  %e", 1, hr);
			if (hr == S_OK)
				hr = temphr;
		} // end if (couldn't shutdown TAPI)

		fTAPIInitted = FALSE;
	} // end if (TAPI was initialized)

	if (pTAPIDev != NULL)
	{
		delete (pTAPIDev);
		pTAPIDev = NULL;
	} // end if (have leftover object)

	if (lpLineDevCaps != NULL)
	{
		LocalFree(lpLineDevCaps);
		lpLineDevCaps = NULL;
	} // end if (have buffer)

	if (pszIniPath != NULL)
	{
		LocalFree(pszIniPath);
		pszIniPath = NULL;
	} // end if (have string)

	if (hTAPIDLL != NULL)
	{
		FreeLibrary(hTAPIDLL); // ignore error
		hTAPIDLL = NULL;
	} // end if (have TAPI DLL)

	return (hr);
#else // ! XBOX
	return (S_OK);
#endif // XBOX

} // CTNMachineInfo::LoadLocalTAPIInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
	



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::LoadLocalCOMPortInfo()"
//==================================================================================
// CTNMachineInfo::LoadLocalCOMPortInfo
//----------------------------------------------------------------------------------
//
// Description: Loads the local machine's COM port information.
//
// Arguments:
//	char* szTestnetRootPath		Path to root testnet directory.  Should end in a
//								backslash.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMachineInfo::LoadLocalCOMPortInfo(char* szTestnetRootPath)
{
#ifndef _XBOX // no COM port crap supported
	HRESULT			hr = S_OK;
	char*			pszIniPath = NULL;
	DWORD			dwTemp;
	char			szCOMPort[8];
	HANDLE			hFile = INVALID_HANDLE_VALUE;
	PTNCOMPORT		pCOMPort = NULL;
	COMPORTCONTEXT	cpc;



	// Build the path to the ini file
	pszIniPath = (char*) LocalAlloc(LPTR, strlen(szTestnetRootPath) + strlen(COMPORTLIST_FILENAME) + 1);
	if (pszIniPath == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	strcpy(pszIniPath, szTestnetRootPath);
	strcat(pszIniPath, COMPORTLIST_FILENAME);


	for(dwTemp = 1; dwTemp <= MAX_COMPORT; dwTemp++)
	{
		wsprintf(szCOMPort, "COM%u", dwTemp);

		hFile = CreateFile(szCOMPort, GENERIC_READ | GENERIC_WRITE, 0, NULL,
							OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			hr = GetLastError();

			if ((hr == ERROR_FILE_NOT_FOUND) || (hr == ERROR_ACCESS_DENIED))
				hr = S_OK;
			else
			{
				DPL(0, "Couldn't open %s!  %e", 2, szCOMPort, hr);
		
				if (hr == S_OK)
					hr = E_FAIL;

				goto DONE;
			} // end else (error trying to open COM port)
		} // end if (couldn't open COM port)
		else
		{
			// We don't actually need the COM port, so just close it.
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;

			DPL(1, "Opened %s.", 1, szCOMPort);


			pCOMPort = new (CTNCOMPort)(NULL, dwTemp);
			if (pCOMPort == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate object)


			ZeroMemory(&cpc, sizeof (COMPORTCONTEXT));
			cpc.pCOMPort = pCOMPort;
			//cpc.fInSection = FALSE;

			hr = FileReadTextFile(pszIniPath, FILERTFO_EXTENDEDINIOPTIONS,
								SearchForCOMPortReadFileCB, &cpc);

			// Hmm.  For now, the file not existing is okay, we'll just treat it as if
			// the device didn't have a number specified
			if (hr == ERROR_FILE_NOT_FOUND)
			{
				DPL(0, "COM port list file \"%s\" doesn't exist, so there's no connection machine name for COM%u.",
					2, pszIniPath, pCOMPort->GetCOMPort());
				hr = S_OK;
			} // end if (file doesn't exist)

			if (hr != S_OK)
			{
				DPL(0, "Couldn't read COM port list file \"%s\" to search for COM%u!",
					2, pszIniPath, pCOMPort->GetCOMPort());
				goto DONE;
			} // end if (couldn't allocate object)


			hr = this->m_COMports.Add(pCOMPort);
			if (hr != S_OK)
			{
				DPL(0, "Couldn't add %s to list!", 1, szCOMPort);
				goto DONE;
			} // end if (couldn't add item)

			pCOMPort = NULL; // forget about it so we don't free it later
		} // end else (could open COM port)
	} // end for (each comport)


DONE:

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	} // end if (have open COM port)

	if (pCOMPort != NULL)
	{
		delete (pCOMPort);
		pCOMPort = NULL;
	} // end if (have left over object)

	if (pszIniPath != NULL)
	{
		LocalFree(pszIniPath);
		pszIniPath = NULL;
	} // end if (have string)

	return (hr);
#else // ! XBOX
	return (S_OK);
#endif // XBOX
} // CTNMachineInfo::LoadLocalCOMPortInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::LoadLocalSoundCardInfo()"
//==================================================================================
// CTNMachineInfo::LoadLocalSoundCardInfo
//----------------------------------------------------------------------------------
//
// Description: Loads the local machine's sound card information.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMachineInfo::LoadLocalSoundCardInfo(void)
{
#ifndef _XBOX // Not sure if our DSound works this way yet... skip for now
	HRESULT						hr;
	HMODULE						hDSoundDLL = NULL;
	PDIRECTSOUNDENUMERATEAPROC	pfnDirectSoundEnumerateA = NULL;


	hDSoundDLL = LoadLibrary("dsound.dll");

	if (hDSoundDLL == NULL)
	{
		hr = GetLastError();

		DPL(0, "Couldn't load dsound.dll!  %e", 1, hr);
		
		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't load DSound library)

	pfnDirectSoundEnumerateA = (PDIRECTSOUNDENUMERATEAPROC) GetProcAddress(hDSoundDLL,
																		"DirectSoundEnumerateA");
	if (pfnDirectSoundEnumerateA == NULL)
	{
		hr = GetLastError();

		DPL(0, "Couldn't find \"DirectSoundEnumerateA\" in dsound.dll!  %e", 1, hr);
		
		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't get proc)


	hr = pfnDirectSoundEnumerateA(DSEnumCallback, &(this->m_soundcards));
	if (hr != DS_OK)
	{
		DPL(0, "Couldn't enumerate sound cards!", 0);
		//goto DONE;
	} // end if (couldn't enum)


DONE:

	if (hDSoundDLL != NULL)
	{
		FreeLibrary(hDSoundDLL);
		hDSoundDLL = NULL;
	} // end if (have library)

	return (hr);
#else // ! XBOX
#pragma BUGBUG(tristanj, "We will probably want to do DSound enumeration later, but for now, skip it.")

	return S_OK;
#endif // XBOX
} // CTNMachineInfo::LoadLocalSoundCardInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNMachineInfo::GetFaultSimCapabilities()"
//==================================================================================
// CTNMachineInfo::GetFaultSimCapabilities
//----------------------------------------------------------------------------------
//
// Description: Determines whether or not the fault simulators are installed and
//				available.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNMachineInfo::GetFaultSimCapabilities(void)
{
	HRESULT				hr = S_OK;
	PTNFAULTSIMIMTEST	pIMTest = NULL;


	pIMTest = new (CTNFaultSimIMTest);
	if (pIMTest == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate object)

	this->m_fFaultSimIMTestAvailable = pIMTest->IsInstalledAndCanBeRun();

	delete (pIMTest);
	pIMTest = NULL;


DONE:

	return (hr);
} // CTNMachineInfo::GetFaultSimCapabilities
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNOtherMachineInfo::CTNOtherMachineInfo()"
//==================================================================================
// CTNOtherMachineInfo constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the CTNOtherMachineInfo object.  Initializes the data
//				structures.
//
// Arguments:
//	PTNCTRLMACHINEID pID	ID this machine info represents.
//
// Returns: None (just the object).
//==================================================================================
CTNOtherMachineInfo::CTNOtherMachineInfo(PTNCTRLMACHINEID pID)
{
	DPL(9, "this = %x, sizeof (this) = %u", 2, this, sizeof (CTNOtherMachineInfo));

	CopyMemory(&(this->m_id), pID, sizeof (TNCTRLMACHINEID));
} // CTNOtherMachineInfo::CTNOtherMachineInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNOtherMachineInfo::~CTNOtherMachineInfo()"
//==================================================================================
// CTNOtherMachineInfo destructor
//----------------------------------------------------------------------------------
//
// Description: Release the CTNOtherMachineInfo object and any memory it may have
//				allocated.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
CTNOtherMachineInfo::~CTNOtherMachineInfo(void)
{
	DPL(9, "this = %x", 1, this);
} // CTNOtherMachineInfo::~CTNOtherMachineInfo
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNOtherMachineInfosList::GetInfoForMachine()"
//==================================================================================
// CTNOtherMachineInfosList::GetInfoForMachine
//----------------------------------------------------------------------------------
//
// Description: Returns pointer to info object of machine with ID passed in.  If the
//				item was not found, NULL is returned.
//
// Arguments:
//	PTNCTRLMACHINEID pID	ID of machine to look up.
//
// Returns: Pointer to machine info object, or NULL if an error occurred.
//==================================================================================
PTNOTHERMACHINEINFO CTNOtherMachineInfosList::GetInfoForMachine(PTNCTRLMACHINEID pID)
{
	HRESULT					hr = S_OK;
	int						i;
	PTNOTHERMACHINEINFO		pInfo;


	this->EnterCritSection();

	for(i = 0; i < this->Count(); i++)
	{
		pInfo = (PTNOTHERMACHINEINFO) this->GetItem(i);
		if (pInfo == NULL)
		{
			DPL(0, "Couldn't get info item %i!", 1, i);
			goto DONE;
		} // end if (couldn't item)

		if (memcmp(&(pInfo->m_id), pID, sizeof (TNCTRLMACHINEID)) == 0)
		{
			goto DONE;
		} // end if (found ID)
	} // end for (each item in this list)


	// If we got here, then we didn't find the object.
	pInfo = NULL;


DONE:

	this->LeaveCritSection();

	return (pInfo);
} // CTNOtherMachineInfosList::GetInfoForMachine
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"InfoLineCallbackProc()"
//==================================================================================
// InfoLineCallbackProc
//----------------------------------------------------------------------------------
//
// Description: Loads the local machine's TAPI information.
//
// Arguments:
//	DWORD hDevice		Handle to device or call, depending on message.
//	DWORD dwMsg			Message to process.
//	DWORD dwContext		User defined context.
//	DWORD dwParam1		First parameter for the message.
//	DWORD dwParam2		Second parameter for the message.
//	DWORD dwParam3		Third parameter for the message.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
void FAR PASCAL InfoLineCallbackProc(DWORD hDevice, DWORD dwMsg, DWORD_PTR dwContext,
									DWORD_PTR dwParam1, DWORD_PTR dwParam2, DWORD_PTR dwParam3)
{
	DPL(0, "msg = %u", 1, dwMsg);
} // InfoLineCallbackProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"SearchForTAPIDeviceReadFileCB()"
//==================================================================================
// SearchForTAPIDeviceReadFileCB
//----------------------------------------------------------------------------------
//
// Description: Callback that handles a line read from a text file.
//
// Arguments:
//	char* szLine			Line that was read.
//	PFILELINEINFO pInfo		Information on line that was read.
//	PVOID pvContext			Pointer to context.  Cast as a TAPIDEVICECONTEXT
//							pointer.
//	BOOL* pfStopReading		Pointer to boolean to set to TRUE to discontinue reading
//							from the file.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT SearchForTAPIDeviceReadFileCB(char* szLine, PFILELINEINFO pInfo,
										PVOID pvContext, BOOL* pfStopReading)
{
	HRESULT				hr = S_OK;
	PTAPIDEVICECONTEXT	pContext = (PTAPIDEVICECONTEXT) pvContext;


	//DPL(9, "Reading line %u:\"%s\".", 2, lpInfo->dwLineNumber, szLine);

	switch (pInfo->dwType)
	{
		case FILERTFT_SECTION:
			if (StringCmpNoCase(szLine, pContext->pDevice->GetString()))
				pContext->fInSection = TRUE;
			else
				pContext->fInSection = FALSE;
		  break;

		default:
			// If we're not in the devices section, just skip the line.
			if (! pContext->fInSection)
				break;

			if (StringIsKey(szLine, "Number"))
			{
				hr = pContext->pDevice->SetPhoneNumber(StringGetKeyValue(szLine));
				if (hr != S_OK)
				{
					DPL(0, "Couldn't set \"%s\"'s phone number!",
						1, pContext->pDevice->GetString());
					return (hr);
				} // end if (couldn't save value)

				DPL(6, "Device \"%s\" uses number \"%s\".",
					2, pContext->pDevice->GetString(),
					pContext->pDevice->GetPhoneNumber());

				// Don't read any more
				(*pfStopReading) = TRUE;
			} // end if (found number for item)
		  break;
	} // end switch (on line type)

	return (S_OK);
} // SearchForTAPIDeviceReadFileCB 
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#ifndef _XBOX
#undef DEBUG_SECTION
#define DEBUG_SECTION	"SearchForCOMPortReadFileCB()"
//==================================================================================
// SearchForCOMPortReadFileCB
//----------------------------------------------------------------------------------
//
// Description: Callback that handles a line read from a text file.
//
// Arguments:
//	char* szLine			Line that was read.
//	PFILELINEINFO pInfo		Information on line that was read.
//	PVOID pvContext			Pointer to context.  Cast as a COMPORTCONTEXT pointer.
//	BOOL* pfStopReading		Pointer to boolean to set to TRUE to discontinue reading
//							from the file.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT SearchForCOMPortReadFileCB(char* szLine, PFILELINEINFO pInfo,
									PVOID pvContext, BOOL* pfStopReading)
{
	HRESULT				hr = S_OK;
	PCOMPORTCONTEXT		pContext = (PCOMPORTCONTEXT) pvContext;


	//DPL(9, "Reading line %u:\"%s\".", 2, lpInfo->dwLineNumber, szLine);

	switch (pInfo->dwType)
	{
		case FILERTFT_SECTION:
			char	szCOMPort[8];


			wsprintf(szCOMPort, "COM%u", pContext->pCOMPort->GetCOMPort());

			if (StringCmpNoCase(szLine, szCOMPort))
				pContext->fInSection = TRUE;
			else
				pContext->fInSection = FALSE;
		  break;

		default:
			// If we're not in the devices section, just skip the line.
			if (! pContext->fInSection)
				break;

			if (StringIsKey(szLine, "ConnectedTo"))
			{
				hr = pContext->pCOMPort->SetString(StringGetKeyValue(szLine));
				if (hr != S_OK)
				{
					DPL(0, "Couldn't set COM%u's machine connection name!",
						1, pContext->pCOMPort->GetCOMPort());
					return (hr);
				} // end if (couldn't save value)

				DPL(6, "COM%u is connected to \"%s\".",
					2, pContext->pCOMPort->GetCOMPort(),
					pContext->pCOMPort->GetString());

				// Don't read any more
				(*pfStopReading) = TRUE;
			} // end if (found number for item)
		  break;
	} // end switch (on line type)

	return (S_OK);
} // SearchForCOMPortReadFileCB 
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#undef DEBUG_SECTION
#define DEBUG_SECTION	"DSEnumCallback()"
//==================================================================================
// DSEnumCallback
//----------------------------------------------------------------------------------
//
// Description: Callback that handles a line read from a text file.
//
// Arguments:
//	LPGUID lpGuid				Pointer to GUID that identifies the DirectSound
//								driver being enumerated.
//	LPCSTR lpcstrDescription	Pointer to textual description of the DirectSound
//								device.
//	LPCSTR lpcstrModule			Pointer to module name of the DirectSound driver
//								for this device.
//	LPVOID lpvContext			User specified context.  Cast as a pointer to a
//								CTNSoundCardsList object.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
BOOL CALLBACK DSEnumCallback(LPGUID lpGuid, LPCSTR lpcstrDescription,
							LPCSTR lpcstrModule, LPVOID lpvContext)
{
	HRESULT				hr;
	PTNSOUNDCARDSLIST	pList = (PTNSOUNDCARDSLIST) lpvContext;
	BOOL				fHalfDuplex = FALSE;
	PTNSOUNDCARD		pSoundCard = NULL;


	if (lpGuid != NULL)
	{
		if (lpcstrDescription == NULL)
		{
			DPL(0, "GUID %g was enumerated, but it has no text description!",
				1, lpGuid);
			return (FALSE);
		} // end if (have no way to describe sound card)


		// These are funky devices that are known to be half-duplex.
		if (StringMatchesWithWildcard("Voice Modem Wave #* Line", (char*) lpcstrDescription, TRUE))
			fHalfDuplex = TRUE;
		else if (StringMatchesWithWildcard("Voice Modem Wave #* Headset", (char*) lpcstrDescription, TRUE))
			fHalfDuplex = TRUE;
		else if (strcmp((char*) lpcstrDescription, "Game Compatible Device") == 0)
			fHalfDuplex = TRUE;


		pSoundCard = new (CTNSoundCard)((char*) lpcstrDescription, lpGuid,
										fHalfDuplex);
		if (pSoundCard == NULL)
		{
			return (FALSE);
		} // end if (couldn't allocate memory)


		hr = pList->Add(pSoundCard);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add sound card %x to list!  %e", 2, pSoundCard, hr);

			delete (pSoundCard);
			pSoundCard = NULL;

			return (FALSE);
		} // end if (couldn't add sound card)
	} // end if (there's a device)

	return (TRUE);
} // DSEnumCallback 
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
