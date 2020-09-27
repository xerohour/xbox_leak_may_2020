#ifdef XHD_MAIN
// (c) Copyright 2000-2001 Intel Corp. All rights reserved.
//
// Title: xHardDisk
//
// History:
//
// 02/13/01 V0.01 BLI Just started working on the test module
/*
// Including these definitions here for quick reference.
D: The DVD Player. On the development console only, if the title is launched from the hard disk, D:\ will be mapped to the directory that the title was launched from, rather than being mapped to the DVD player. 
E: This drive does not actually exist on the console. It is used solely by the Xbox Development Kit as the drive where development titles are copied to the Xbox development console for execution. 
F: The title's user data on the MU located in the top slot of port 0. 
G: The title's user data on the MU located in the bottom slot of port 0. 
H: The title's user data on the MU located in the top slot of port 1. 
I: The title's user data on the MU located in the bottom slot of port 1. 
J: The title's user data on the MU located in the top slot of port 2. 
K: The title's user data on the MU located in the bottom slot of port 2. 
L: The title's user data on the MU located in the top slot of port 3. 
M: The title's user data on the MU located in the bottom slot of port 3. 
S: On the development console only, the root directory of the persistent data region, under which each title's specific persistent data directory resides. 
T: The title's persistent data region. 
U: The title's user data region on the hard disk. 
V: On the development console only, the root directory of the user data region, under which each title's specific user data directory resides. 
W: Used to map another title's user data region (from disk or an MU). 
X: Used to map another title's persistent data region. 
Y: Unused 
Z: The title's utility data region. 
*/

#include "..\stdafx.h"
#include "..\testobj.h"
#include "..\statkeys.h"
#include "xhard_disk.h"

#ifdef XHD_TAG
IMPLEMENT_MODULEUNLOCKED (CXModuleHardDisk);
#else
IMPLEMENT_MODULEUNLOCKED (CXModuleDVD);
#endif

// HardDisk test number definitions
#define FREE_SPACE_RANDOM_WRC           1
#define PHYSICAL_DEVICE_SEQUENTIAL_READ 2
#define FILE_SYSTEM_CHECKSUM            3
#define FILE_SEQUENTIAL_READ            4
#define DVD_RANDOM_READ                 5

#ifdef XHD_TAG
bool CXModuleHardDisk::InitializeParameters ()
#else
bool CXModuleDVD::InitializeParameters ()
#endif
{
	wchar_t wszTempString[256];
	DWORD i, j;
	LPCTSTR pwszTemp = NULL;

	m_pdwDVDChecksumAllowed = NULL;
	m_dwNumOfDVDChecksums = 0;
	m_pdwCDChecksumAllowed = NULL;
	m_dwNumOfCDChecksums = 0;
	m_pdwChecksumAllowed = NULL;
	m_pwszChecksumPath = NULL;
	m_dwNumOfChecksums = 0;
	m_pdwChecksumSectorStart = NULL;
	m_pdwChecksumSectorEnd = NULL;
	m_dwNumOfChecksumSectorRanges = 0;

	if (!CTestObj::InitializeParameters ())
		return false;

	// Get MinTestTime parameter
	m_dwMinTestTime = (DWORD)GetCfgUint (L"MinTestTime", 0);

	// Get MaxTestTime parameter
	m_dwMaxTestTime = (DWORD)GetCfgUint (L"MaxTestTime", 0xFFFFFFFF);

	// Get LoopCount parameter
	m_dwMaxLoops = (DWORD)GetCfgUint (L"LoopCount", 100);

	// Get MutexTimeout parameter
	m_dwMutexTimeout = (DWORD)GetCfgUint (L"MutexTimeout", 180000); // Number of milliseconds to wait for a mutex

	// Get BufferSize parameter
	m_dwBufferSize = (DWORD)GetCfgUint (L"BufferSize", 65536); // Number of bytes in the buffer

	// Get DriveLetter parameter
	pwszTemp = GetCfgString (L"DriveLetter", L"D");
	if ((towupper(*pwszTemp) < L'A') || (toupper(*pwszTemp) > L'Z'))
	{
		err_BADPARAMETER(L"DriveLetter");
		return false;
	}
	else
	{
		m_chDriveLetter[0] = towupper(*pwszTemp);
		m_chDriveLetter[1] = 0;
	}

	// Get FileName parameter
	m_wszFileName = GetCfgString (L"FileName", L"XMTA_HD_TEMP_FILE");

	// Get FreeSpace parameter
	pwszTemp = GetCfgString (L"FreeSpace", L"90.0");
	m_dblFreeSpacePercent = wcstod(pwszTemp, NULL);
	if ((m_dblFreeSpacePercent < 0) || (m_dblFreeSpacePercent > 100))
	{
		err_BADPARAMETER(L"FreeSpace");
		return false;
	}

	// Get DriveNumber parameter
	m_dwDriveNumber = (DWORD)GetCfgUint (L"DriveNumber", 0);

	// Get DVDChecksumAllowed[] parameter list
	i = 0;
	for (;;)
	{
		swprintf(wszTempString, L"DVDChecksumAllowed[%2.2lu]", i);
		// Get it as a string so that we can compare it to NULL
		if ((pwszTemp = GetCfgString (wszTempString, NULL)) == NULL)
			break;
		i++;
	}
	m_dwNumOfDVDChecksums = i;
	if (i > 0)
	{
		if ((m_pdwDVDChecksumAllowed = new DWORD[i]) == NULL)
		{
			err_RE_OUT_OF_MEMORY_P1(i*sizeof(DWORD));
			return false;
		}
		for (j = 0; j < i; j++) // Now get the checksum values
		{
			swprintf(wszTempString, L"DVDChecksumAllowed[%2.2lu]", j);
			m_pdwDVDChecksumAllowed[j] = (DWORD)GetCfgUint (wszTempString, 0);
		}
	}
	
	// Get CDChecksumAllowed[] parameter list
	i = 0;
	for (;;)
	{
		swprintf(wszTempString, L"CDChecksumAllowed[%2.2lu]", i);
		// Get it as a string so that we can compare it to NULL
		if ((pwszTemp = GetCfgString (wszTempString, NULL)) == NULL)
			break;
		i++;
	}
	m_dwNumOfCDChecksums = i;
	if (i > 0)
	{
		if ((m_pdwCDChecksumAllowed = new DWORD[i]) == NULL)
		{
			err_RE_OUT_OF_MEMORY_P1(i*sizeof(DWORD));
			return false;
		}
		for (j = 0; j < i; j++) // Now get the checksum values
		{
			swprintf(wszTempString, L"CDChecksumAllowed[%2.2lu]", j);
			m_pdwCDChecksumAllowed[j] = (DWORD)GetCfgUint (wszTempString, 0);
		}
	}

	// Get RandomSeed parameter
	m_dwRandomSeed = (DWORD)GetCfgUint (L"RandomSeed", 0);

	// Get NumberOfLoopsBetweenSpinDowns parameter
	m_dwNumberOfLoopsBetweenSpinDowns = (DWORD)GetCfgUint (L"NumberOfLoopsBetweenSpinDowns", 0);

	// Get SpinDownDelay parameter
	m_dwSpinDownDelay = (DWORD)GetCfgUint (L"SpinDownDelay", 0);
	

	// Get ChecksumAllowed[] parameter list
	i = 0;
	for (;;)
	{
		swprintf(wszTempString, L"ChecksumAllowed[%2.2lu]", i);
		// Get it as a string so that we can compare it to NULL
		if ((pwszTemp = GetCfgString (wszTempString, NULL)) == NULL)
			break;
		i++;
	}
	m_dwNumOfChecksums = i;
	if (i > 0)
	{
		if ((m_pdwChecksumAllowed = new DWORD[i]) == NULL)
		{
			err_RE_OUT_OF_MEMORY_P1(i*sizeof(DWORD));
			return false;
		}
		for (j = 0; j < i; j++) // Now get the checksum values
		{
			swprintf(wszTempString, L"ChecksumAllowed[%2.2lu]", j);
			m_pdwChecksumAllowed[j] = (DWORD)GetCfgUint (wszTempString, 0);
		}
	}

	// Get ChecksumPath[] parameter array
	i = 0;
	for (;;)
	{
		swprintf(wszTempString, L"ChecksumPath[%2.2lu]", i);
		// Get it as a string so that we can compare it to NULL
		if ((pwszTemp = GetCfgString (wszTempString, NULL)) == NULL)
			break;
		i++;
	}
	if (i > 0)
	{
		if ((m_pwszChecksumPath = new wchar_t *[i+1]) == NULL)
		{
			err_RE_OUT_OF_MEMORY_P1((i+1)*sizeof(wchar_t *));
			return false;
		}
		for (j = 0; j < i+1; j++) // Set the array of pointers to NULL to start
			m_pwszChecksumPath[j] = NULL;
		for (j = 0; j < i; j++) // Now get the path values
		{
			swprintf(wszTempString, L"ChecksumPath[%2.2lu]", j);
			m_pwszChecksumPath[j] = (wchar_t *)GetCfgString (wszTempString, NULL);
		}
//		for (j = 0; j < i; j++)
//		{
//			ReportDebug(BIT0, L"Path is %s", m_pwszChecksumPath[j]);
//		}
	}

	// Get ChecksumSectorStart[], ChecksumSectorEnd[] parameter array pairs
	i = 0;
	for (;;)
	{
		swprintf(wszTempString, L"ChecksumSectorStart[%2.2lu]", i);
		// Get it as a string so that we can compare it to NULL
		if ((pwszTemp = GetCfgString (wszTempString, NULL)) == NULL)
			break;
		i++;
	}
	m_dwNumOfChecksumSectorRanges = i;
	if (i > 0)
	{
		if ((m_pdwChecksumSectorStart = new DWORD[i]) == NULL)
		{
			err_RE_OUT_OF_MEMORY_P1(i*sizeof(DWORD));
			return false;
		}
		if ((m_pdwChecksumSectorEnd = new DWORD[i]) == NULL)
		{
			err_RE_OUT_OF_MEMORY_P1(i*sizeof(DWORD));
			return false;
		}
		for (j = 0; j < i; j++) // Now get the list of start sectors
		{
			swprintf(wszTempString, L"ChecksumSectorStart[%2.2lu]", j);
			m_pdwChecksumSectorStart[j] = (DWORD)GetCfgUint (wszTempString, 0);
		}
		for (j = 0; j < i; j++) // Now get the list of end sectors
		{
			swprintf(wszTempString, L"ChecksumSectorEnd[%2.2lu]", j);
			m_pdwChecksumSectorEnd[j] = (DWORD)GetCfgUint (wszTempString, m_pdwChecksumSectorStart[j]);
			if (m_pdwChecksumSectorEnd[j] < m_pdwChecksumSectorStart[j]) // Make sure that the end sector is not less than the start sector
				m_pdwChecksumSectorEnd[j] = m_pdwChecksumSectorStart[j];
		}
	}

	// Get StartPercent parameter
	pwszTemp = GetCfgString (L"StartPercent", L"0.0");
	m_dblStartPercent = wcstod(pwszTemp, NULL);
	if ((m_dblStartPercent < 0) || (m_dblStartPercent > 100))
	{
		err_BADPARAMETER(L"StartPercent");
		return false;
	}

	// Get EndPercent parameter
	pwszTemp = GetCfgString (L"EndPercent", L"100.0");
	m_dblEndPercent = wcstod(pwszTemp, NULL);
	if ((m_dblEndPercent < 0) || (m_dblEndPercent > 100))
	{
		err_BADPARAMETER(L"EndPercent");
		return false;
	}

	if(m_dblStartPercent > m_dblEndPercent)
	{
		err_RE_START_END_PERCENTAGE_P2(m_dblStartPercent, m_dblEndPercent);
		return false;
	}		

	return true;
}

#ifdef XHD_TAG
void CXModuleHardDisk::vInitializeTestVariables()
#else
void CXModuleDVD::vInitializeTestVariables()
#endif
{
	// Initialize some globals here
	m_hDevice = INVALID_HANDLE;
	m_hXMTAHardDiskMutex = (HANDLE) -1;
	m_pdwBuffer1 = NULL;
	m_pdwBuffer2 = NULL;
	m_pdwMapArray = NULL;
	m_szFilePath[0] = 0;
	hDir = INVALID_HANDLE_VALUE;
	LARGE_INTEGER liLocalTSC;

	_asm
	{
		rdtsc
		mov liLocalTSC.LowPart, eax       ; Preserve the Least Significant 32 bits
		mov liLocalTSC.HighPart, edx      ; Preserve the Most Significant 32 bits
	}

	m_dwlStartTime = liLocalTSC.QuadPart/gi64CPUSpeed;
}

#ifdef XHD_TAG
void CXModuleHardDisk::vCleanup()
#else
void CXModuleDVD::vCleanup()
#endif
{
	if (hDir != INVALID_HANDLE_VALUE)
	{
		FindClose(hDir);
	}
	if (m_pdwMapArray != NULL)
	{
		delete [] m_pdwMapArray;
		m_pdwMapArray = NULL;
	}
	if (m_pdwBuffer1 != NULL)
	{
		delete [] m_pdwBuffer1;
		m_pdwBuffer1 = NULL;
	}
	if (m_pdwBuffer2 != NULL)
	{
		delete [] m_pdwBuffer2;
		m_pdwBuffer2 = NULL;
	}

	vReleasePossessionOfHardDiskResources();
	if (m_hDevice != INVALID_HANDLE)
	{
		CloseHandle(m_hDevice);
		m_hDevice = INVALID_HANDLE;
	}
}

// m_hXMTAHardDiskMutex is a stack-based parameter so it is different for each
// executing thread (each test).  In this way, different tests can use this parameter
// to reference different named mutexes at the same time (such as a mutex for a drive C test
// and a mutex for a drive d test at the same time) that don't collide.
// On exit, m_hXMTAHardDiskMutex is either -1 or a true mutex handle
#ifdef XHD_TAG
bool CXModuleHardDisk::bGetPossessionOfHardDiskResources (char *MutexName)
#else
bool CXModuleDVD::bGetPossessionOfHardDiskResources (char *MutexName)
#endif
{
	DWORD x;
	int onesecond = 1000;

	if (m_hXMTAHardDiskMutex != (HANDLE) -1) // We already have mutex possession, so free it up before continuing
		vReleasePossessionOfHardDiskResources();
	m_hXMTAHardDiskMutex = CreateMutexA (NULL, FALSE, MutexName);
	if (m_hXMTAHardDiskMutex == NULL) // NULL?, don't even try accessing
	{
		m_hXMTAHardDiskMutex = (HANDLE) -1;
		err_MutexHandle(MutexName);
		return (false);
	}
	for (x = 0; x < (m_dwMutexTimeout/(DWORD)onesecond); x++)
	{
		if (WaitForSingleObject(m_hXMTAHardDiskMutex, onesecond) == WAIT_OBJECT_0)
			return (true);
		if (CheckAbort(HERE))
		{
			vReleasePossessionOfHardDiskResources();
			return (false);
		}
	}
	vReleasePossessionOfHardDiskResources();
	err_MutexTimeout(MutexName);
	return (false);
}

#ifdef XHD_TAG
void CXModuleHardDisk::vReleasePossessionOfHardDiskResources ()
#else
void CXModuleDVD::vReleasePossessionOfHardDiskResources ()
#endif
{
	if (m_hXMTAHardDiskMutex != (HANDLE) -1)
	{
		ReleaseMutex (m_hXMTAHardDiskMutex);
		CloseHandle (m_hXMTAHardDiskMutex);
		m_hXMTAHardDiskMutex = (HANDLE) -1;
	}
}

#ifdef XHD_TAG
void CXModuleHardDisk::vCompareTestTime()
#else
void CXModuleDVD::vCompareTestTime()
#endif
{
	TCHAR tcStatString[100];
	LARGE_INTEGER liLocalTSC;
	DWORDLONG dwlEndTime;

	_asm
	{
		rdtsc
		mov liLocalTSC.LowPart, eax       ; Preserve the Least Significant 32 bits
		mov liLocalTSC.HighPart, edx      ; Preserve the Most Significant 32 bits
	}

	dwlEndTime = liLocalTSC.QuadPart/gi64CPUSpeed;
	_stprintf(tcStatString, _T("%s %s %lu"), HD_TEST_TIME, _T("Test"), (DWORD)GetTestNumber());
	ReportStatistic(tcStatString, _T("%lu"), (DWORD)(dwlEndTime-m_dwlStartTime));
	if ((dwlEndTime-m_dwlStartTime) > (DWORDLONG)m_dwMaxTestTime)
	{
		if (err_TEST_TIME_TOO_LONG((DWORD)(dwlEndTime-m_dwlStartTime), m_dwMaxTestTime))
		return;
	}
	else if ((dwlEndTime-m_dwlStartTime) < (DWORDLONG)m_dwMinTestTime)
	{
		if (err_TEST_TIME_TOO_SHORT((DWORD)(dwlEndTime-m_dwlStartTime), m_dwMinTestTime))
		return;
	}
}

//******************************************************************
// Title: free_space_random_wrc
//
// Abstract: Create a large file on the hard drive and perform a random
//           write-read-compare test within the file
//
// Uses config parameters:
//           LoopCount
//           MutexTimeout
//           BufferSize
//           DriveLetter
//           FileName
//           FreeSpace
//******************************************************************
#ifdef XHD_TAG
IMPLEMENT_TESTUNLOCKED (HardDisk, free_space_random_wrc, FREE_SPACE_RANDOM_WRC)
#else
IMPLEMENT_TESTUNLOCKED (DVD, free_space_random_wrc, FREE_SPACE_RANDOM_WRC)
#endif
{
//	#ifdef _DEBUG
		static int qqFreeSpaceRandomWRC;
		qqFreeSpaceRandomWRC++;
		ReportDebug(BIT0, _T("FreeSpaceRandomWRC Test - Loop %d"), qqFreeSpaceRandomWRC);
//	#endif
	vInitializeTestVariables();
	vFreeSpaceRandomWRC();
	vCompareTestTime();
	vCleanup();
	return;
}

//******************************************************************
// Title: physical_device_sequential_read
//
// Abstract: Read every sector of a physical drive and ensure there are
//           no read errors.
//
// Uses config parameters:
//           BufferSize
//           DriveNumber
//           StartPercent
//           EndPercent
//******************************************************************
#ifdef XHD_TAG
IMPLEMENT_TESTUNLOCKED (HardDisk, physical_device_sequential_read, PHYSICAL_DEVICE_SEQUENTIAL_READ)
#else
IMPLEMENT_TESTUNLOCKED (DVD, physical_device_sequential_read, PHYSICAL_DEVICE_SEQUENTIAL_READ)
#endif
{
//	#ifdef _DEBUG
		static int qqPhysicalDeviceSequentialRead;
		qqPhysicalDeviceSequentialRead++;
		ReportDebug(BIT0, _T("PhysicalDeviceSequentialRead Test - Loop %d"), qqPhysicalDeviceSequentialRead);
//	#endif
	vInitializeTestVariables();
	vPhysicalDeviceSequentialRead();
	vCompareTestTime();
	vCleanup();
	return;
}

//******************************************************************
// Title: file_system_checksum
//
// Abstract: Calculates the checksum of the contents of all files in and the
//           names of all files and directories in specified paths.
//
// Uses config parameters:
//           ChecksumAllowed[XX]
//           ChecksumPath[XX]
//******************************************************************
#ifdef XHD_TAG
IMPLEMENT_TESTUNLOCKED (HardDisk, file_system_checksum, FILE_SYSTEM_CHECKSUM)
#else
IMPLEMENT_TESTUNLOCKED (DVD, file_system_checksum, FILE_SYSTEM_CHECKSUM)
#endif
{
//	#ifdef _DEBUG
		static int qqFileSystemChecksum;
		qqFileSystemChecksum++;
		ReportDebug(BIT0, _T("FileSystemChecksum Test - Loop %d"), qqFileSystemChecksum);
//	#endif
	vInitializeTestVariables();
	vPerformChecksum();
	vCompareTestTime();
	vCleanup();
	return;
}

//******************************************************************
// Title: file_read
//
// Abstract: Reads and discards the contents from a specified file.
//
// Uses config parameters:
//           BufferSize
//           DriveLetter
//           FileName
//******************************************************************
#ifdef XHD_TAG
IMPLEMENT_TESTUNLOCKED (HardDisk, file_sequential_read, FILE_SEQUENTIAL_READ)
#else
IMPLEMENT_TESTUNLOCKED (DVD, file_sequential_read, FILE_SEQUENTIAL_READ)
#endif
{
//	#ifdef _DEBUG
		static int qqFileSequentialRead;
		qqFileSequentialRead++;
		ReportDebug(BIT0, _T("FileSequentialRead Test - Loop %d"), qqFileSequentialRead);
//	#endif
	vInitializeTestVariables();
	vFileSequentialRead();
	vCompareTestTime();
	vCleanup();
	return;
}

//******************************************************************
// Title: file_read
//
// Abstract: Reads and discards the contents from a specified file.
//
// Uses config parameters:
//           BufferSize
//           DriveLetter
//           FileName
//******************************************************************
#ifdef XHD_TAG
IMPLEMENT_TESTUNLOCKED (HardDisk, dvd_file_sequential_read, FILE_SEQUENTIAL_READ)
#else
IMPLEMENT_TESTUNLOCKED (DVD, dvd_file_sequential_read, FILE_SEQUENTIAL_READ)
#endif
{
//	#ifdef _DEBUG
		static int qqdvdFileSequentialRead;
		qqdvdFileSequentialRead++;
		ReportDebug(BIT0, _T("DVDFileSequentialRead Test - Loop %d"), qqdvdFileSequentialRead);
//	#endif
	vInitializeTestVariables();
	vFileSequentialRead();
	vCompareTestTime();
	vCleanup();
	return;
}

//******************************************************************
// Title: dvd_random_read
//
// Abstract: Randomly read regions within the valid data space of the DVD
//           and checksum the contents read
//
// Uses config parameters:
//           LoopCount
//           BufferSize
//           DriveLetter
//           DVDChecksumAllowed[]
//           CDChecksumAllowed[]
//           RandomSeed
//           NumberOfLoopsBetweenSpinDowns
//           SpinDownDelay
//******************************************************************
#ifdef XHD_TAG
IMPLEMENT_TESTUNLOCKED (HardDisk, dvd_random_read, DVD_RANDOM_READ)
#else
IMPLEMENT_TESTUNLOCKED (DVD, dvd_random_read, DVD_RANDOM_READ)
#endif
{
//	#ifdef _DEBUG
		static int qqDVDRandomRead;
		qqDVDRandomRead++;
		ReportDebug(BIT0, _T("DVDRandomRead Test - Loop %d"), qqDVDRandomRead);
//	#endif
	vInitializeTestVariables();
	vDVDRandomRead();
	vCompareTestTime();
	vCleanup();
	return;
}

#ifdef XHD_TAG
void CXModuleHardDisk::vDVDRandomRead ()
#else
void CXModuleDVD::vDVDRandomRead ()
#endif
{
	DWORD         nBlocks, n, dwMapSize, dwChecksum = 0, dwKeepAlive = 0;
	DWORD         cdwLength, nReads, dwSpinDownCounter = 0;
	UINT          nReadsNeeded,theseed,uiRandScratch;
	DWORD         i, dwTestLoops, dwNumberOfLoopsBetweenSpinDowns, dwKeepAliveMax;
	bool          bDVD;
	DWORDLONG qwDisturbanceStart, qwDisturbanceEnd;
	DWORD dwBlockLow, dwBlockHigh, dwTotalDisturbanceBlocks = 0, dwTotalDisturbanceSectors = 0;
	typedef	struct {DWORD dwStartSector; DWORD dwEndSector;} DVD_DISTURBANCE_REGION;
/*	DVD_DISTURBANCE_REGION DisturbanceRegion[] =   {{0x00A8D2, 0x00B8D1}, // XM2 DVD mapping
													{0x02FDBE, 0x030DBD},
													{0x07B3EF, 0x07C3EE},
													{0x0A0623, 0x0A1622},
													{0x0D6C06, 0x0D7C05},
													{0x0FCE77, 0x0FDE76},
													{0x133A4C, 0x134A4B},
													{0x159D59, 0x15AD58},
													{0x1C08E2, 0x1C18E1},
													{0x20BF12, 0x20CF11},
													{0x230D44, 0x231D43},
													{0x25493F, 0x25593E},
													{0x28CF5B, 0x28DF5A},
													{0x2B1B51, 0x2B2B50},
													{0x2E8E17, 0x2E9E16},
													{0x333531, 0x334530}

													{0x00BCAF, 0x00CCAE}, // XM3 DVD mapping
													{0x030B1B, 0x031B1A},
													{0x0563FB, 0x0573FA},
													{0x07B7CF, 0x07C7CE},
													{0x09F82A, 0x0A0829},
													{0x0C5CE3, 0x0C6CE2},
													{0x0EA890, 0x0EB88F},
													{0x10E684, 0x10F683},
													{0x1C0FF5, 0x1C1FF4},
													{0x1E573E, 0x1E673D},
													{0x20BCD3, 0x20CCD2},
													{0x2301AB, 0x2311AA},
													{0x254606, 0x255605},
													{0x279F33, 0x27AF32},
													{0x29F2D7, 0x2A02D6},
													{0x333AA8, 0x334AA7}
	
	};
*/
	DVD_DISTURBANCE_REGION DisturbanceRegion[] =   {{0x00A8D2, 0x00B8D1}, // Combined XM2/XM3 DVD mapping
													{0x00BCAF, 0x00CCAE},
													{0x02FDBE, 0x031B1A},
													{0x0563FB, 0x0573FA},
													{0x07B3EF, 0x07C7CE},
													{0x09F82A, 0x0A1622},
													{0x0C5CE3, 0x0C6CE2},
													{0x0D6C06, 0x0D7C05},
													{0x0EA890, 0x0EB88F},
													{0x0FCE77, 0x0FDE76},
													{0x10E684, 0x10F683},
													{0x133A4C, 0x134A4B},
													{0x159D59, 0x15AD58},
													{0x1C08E2, 0x1C1FF4},
													{0x1E573E, 0x1E673D},
													{0x20BCD3, 0x20CF11},
													{0x2301AB, 0x231D43},
													{0x254606, 0x25593E},
													{0x279F33, 0x27AF32},
													{0x28CF5B, 0x28DF5A},
													{0x29F2D7, 0x2A02D6},
													{0x2B1B51, 0x2B2B50},
													{0x2E8E17, 0x2E9E16},
													{0x333531, 0x334AA7}
	};

	if(!PhysicalDeviceOpen(false, true))
		return;

	if (m_qwDeviceSize > 1073741824)
	{
		bDVD = true;
		dwTestLoops = m_dwMaxLoops;
		dwKeepAliveMax = 20;
		dwNumberOfLoopsBetweenSpinDowns = m_dwNumberOfLoopsBetweenSpinDowns;
	}
	else // CD
	{
		bDVD = false;
		dwTestLoops = m_dwMaxLoops/3;  // CDs are over 3 times slower than DVDs
		dwKeepAliveMax = 7;
		dwNumberOfLoopsBetweenSpinDowns = m_dwNumberOfLoopsBetweenSpinDowns/3;
	}

	cdwLength = m_dwBufferSize / 4;  // Number of DWORDs in the buffer
	// This is OK as long as the hard drive is < 2048 GB
	nBlocks  = (DWORD)(m_qwDeviceSize / m_dwBufferSize);

	if(nBlocks < 2)
	{
		err_NO_BLOCKS_TO_TEST();
		return;
	}

	if (m_pdwBuffer2 != NULL) // If for some reason this is not NULL, delete the old buffer
		delete [] m_pdwBuffer2;
	if ((m_pdwBuffer2 = new DWORD[cdwLength]) == NULL)
	{
		err_RE_OUT_OF_MEMORY_P1(m_dwBufferSize);
		return;
	}

	if (dwTestLoops < nBlocks/2) // Determine the required number of entries in the m_MapArray
		dwMapSize = dwTestLoops;
	else
		dwMapSize = nBlocks; // Make this nBlocks instead of nBlocks/2 beause we are also setting aside blocks that overlap disturbance regions on the disk

	// Get a rough count of how many blocks are consumed by disturbances
	dwTotalDisturbanceSectors = 0;
	if (bDVD)
	{
		for (i = 0; i < sizeof(DisturbanceRegion)/sizeof(DVD_DISTURBANCE_REGION); i++)
		{
			dwTotalDisturbanceSectors = dwTotalDisturbanceSectors + (DisturbanceRegion[i].dwEndSector - DisturbanceRegion[i].dwStartSector) + 1;
		}
		dwTotalDisturbanceBlocks = (DWORD)(((DWORDLONG)dwTotalDisturbanceSectors * (DWORDLONG)m_dwBytesPerSector)/(DWORDLONG)m_dwBufferSize);
		dwTotalDisturbanceBlocks = dwTotalDisturbanceBlocks + sizeof(DisturbanceRegion)/sizeof(DVD_DISTURBANCE_REGION);
	}

	if (m_pdwMapArray != NULL) // If for some reason this is not NULL, delete the old buffer
		delete [] m_pdwMapArray;
	if ((m_pdwMapArray = new DWORD[dwMapSize+dwTotalDisturbanceBlocks]) == NULL)
	{
		err_RE_OUT_OF_MEMORY_P1((dwMapSize+dwTotalDisturbanceBlocks)*sizeof(DWORD));
		return;
	}
	// 0xFFFFFFFF will indicate that an array element is empty
	memset(m_pdwMapArray, 0xFF, (dwMapSize+dwTotalDisturbanceBlocks)*sizeof(DWORD));

	vNewSeed(&theseed); // Get a new seed value based on the current time
	if (m_dwRandomSeed != 0) // Use the specified seed if not equal to zero
		theseed = m_dwRandomSeed;
	uiRandScratch = theseed;

/*
	// A debug routine that reads every sector on the device and displays whether they are good or bad
	ReportDebug(BIT0, L"0x000000 Start good");
	bool bGood = true;
	for (n = 0; n < nBlocks; n++)
	{
		if(LocalRead((DWORDLONG)n * (DWORDLONG)m_dwBufferSize,m_pdwBuffer2,m_dwBufferSize))
		{
			if (!bGood)
			{
				ReportDebug(BIT0, L"0x%6.6lx End bad", n-1);
				ReportDebug(BIT0, L"0x%6.6lx Start good", n);
				bGood = true;
			}
		}
		else
		{
			if (bGood)
			{
				ReportDebug(BIT0, L"0x%6.6lx End good", n-1);
				ReportDebug(BIT0, L"0x%6.6lx Start bad", n);
				bGood = false;
			}
		}
	}

return;
*/
	dwTotalDisturbanceBlocks = 0;
	if (bDVD)
	{
		for (i = 0; i < sizeof(DisturbanceRegion)/sizeof(DVD_DISTURBANCE_REGION); i++)
		{
			qwDisturbanceStart = (DWORDLONG)DisturbanceRegion[i].dwStartSector * (DWORDLONG)m_dwBytesPerSector;
			qwDisturbanceEnd = ((DWORDLONG)(DisturbanceRegion[i].dwEndSector + 1) * (DWORDLONG)m_dwBytesPerSector) - (DWORDLONG)1;
			dwBlockLow = (DWORD)(qwDisturbanceStart/m_dwBufferSize);
			dwBlockHigh = (DWORD)(qwDisturbanceEnd/m_dwBufferSize);
			// Map the blocks as unusable.  Assume that no two disturbance regions overlapp each other
			for (n = dwBlockLow; n <= dwBlockHigh; n++)
			{
				vSet(m_pdwMapArray, n);
				dwTotalDisturbanceBlocks++;
			}
		}
	}
	ReportDebug(BIT0, L"%lu test blocks consumed by media disturbances", dwTotalDisturbanceBlocks); // A keep-alive message
	if (CheckAbort(HERE))
		return;
	nReadsNeeded = 0;
	for(nReads = 0; nReads <= dwTestLoops && !CheckAbort(HERE); )
	{
		if ((dwTestLoops - nReads) > (nBlocks / 2))
		{
			nReadsNeeded = nBlocks / 2;
			nReads = nReads + nBlocks / 2;
		}
		else
		{
			nReadsNeeded = dwTestLoops - nReads;
			nReads = dwTestLoops;
		}

		for(UINT count = 0; count < nReadsNeeded && !CheckAbort(HERE); count++)
		{
			do {
				n = random(&uiRandScratch) % nBlocks;
			} while(bLookup(m_pdwMapArray, n));
			vSet(m_pdwMapArray, n);

			if(LocalRead((DWORDLONG)n * (DWORDLONG)m_dwBufferSize,m_pdwBuffer2,m_dwBufferSize))
			{
//				ReportDebug(BIT0, L"Read sector %lu contains");
				// Add the checksum
				for(DWORD i = 0; i < cdwLength; i++)
				{
//					ReportDebug(BIT0, L"%8.8lx", m_pdwBuffer2[i]);
					dwChecksum = dwChecksum + m_pdwBuffer2[i];
				}
			}
			dwKeepAlive++;
			if (dwKeepAlive == dwKeepAliveMax)
			{
				ReportDebug(BIT0, L"reading offset %I64u", (DWORDLONG)n * (DWORDLONG)m_dwBufferSize);
				dwKeepAlive = 0;
			}
			dwSpinDownCounter = dwSpinDownCounter + 1;
			if (dwSpinDownCounter == dwNumberOfLoopsBetweenSpinDowns)
			{
				if (m_dwSpinDownDelay != 0)
				{
					DWORD m;
					for (m = 0; m < m_dwSpinDownDelay/10; m++)
					{
						Sleep(10000);
						ReportDebug(BIT0, L"DVD spinning down");
						if (CheckAbort(HERE))
							break;
					}
					Sleep((m_dwSpinDownDelay - (m*10)) * 1000);
				}
				dwSpinDownCounter = 0;
			}
		}

		if(nReads == dwTestLoops)
			nReads++;
		else
		{
			memset(&m_pdwMapArray[dwTotalDisturbanceBlocks], 0xFF, (dwMapSize)*sizeof(DWORD)); // Don't overwrite disturbance regions info
			nReadsNeeded = 0;
			theseed = random(&uiRandScratch); // Get a new seed based on the next random number
			uiRandScratch = theseed;
		}
	}
	if (!CheckAbort(HERE))
	{
		if (bDVD)
		{
			// Only compare checksums if the number of checksums is greater than 1, or if there is a single checksum that is not 0, or if a random seed was specified
			if ((m_dwNumOfDVDChecksums != 0) && (m_dwRandomSeed != 0))
			{
				if ((m_dwNumOfDVDChecksums > 1) || (m_pdwDVDChecksumAllowed[0] != 0))
				{
					for (i = 0; i < m_dwNumOfDVDChecksums; i++)
					{
						if (m_pdwDVDChecksumAllowed[i] == dwChecksum)
							break;
					}
					if (i == m_dwNumOfDVDChecksums)
					{
						err_CHECKSUM_BAD_P1(dwChecksum);
						return;
					}
				}
			}
		}
		else // CD
		{
			// Only compare checksums if the number of checksums is greater than 1, or if there is a single checksum that is not 0, or if a random seed was specified
			if ((m_dwNumOfCDChecksums != 0) && (m_dwRandomSeed != 0))
			{
				if ((m_dwNumOfCDChecksums > 1) || (m_pdwCDChecksumAllowed[0] != 0))
				{
					for (i = 0; i < m_dwNumOfCDChecksums; i++)
					{
						if (m_pdwCDChecksumAllowed[i] == dwChecksum)
							break;
					}
					if (i == m_dwNumOfCDChecksums)
					{
						err_CHECKSUM_BAD_P1(dwChecksum);
						return;
					}
				}
			}
		}
	}

	return;
}

#ifdef XHD_TAG
void CXModuleHardDisk::vFreeSpaceRandomWRC ()
#else
void CXModuleDVD::vFreeSpaceRandomWRC ()
#endif
{
	DWORD         nBlocks, n, dwMapSize;
	DWORD         cdwLength, nWrites;
	UINT          nReadsNeeded,theseed,uiRandScratch;

	if(!FreeSpaceDeviceOpen())
		return;

	cdwLength = m_dwBufferSize / 4;  // Number of DWORDs in the buffer
	// This is OK as long as the hard drive is < 2048 GB
	nBlocks  = (DWORD)(m_qwDeviceSize / m_dwBufferSize);

	if(nBlocks < 2)
	{
		err_NO_BLOCKS_TO_TEST();
		return;
	}

	// Force a write to actually commit the disk storage.
/*
	BYTE buf[512];
	for(DWORDLONG idx = 0; idx < m_qwDeviceSize && !CheckAbort(HERE); idx += m_dwBufferSize)
	{
		Write(idx,buf,512);
	}
	if(CheckAbort(HERE))
		return;
*/

	if (m_pdwBuffer1 != NULL) // If for some reason this is not NULL, delete the old buffer
		delete [] m_pdwBuffer1;
	if ((m_pdwBuffer1 = new DWORD[cdwLength]) == NULL)
	{
		err_RE_OUT_OF_MEMORY_P1(m_dwBufferSize);
		return;
	}

	if (m_pdwBuffer2 != NULL) // If for some reason this is not NULL, delete the old buffer
		delete [] m_pdwBuffer2;
	if ((m_pdwBuffer2 = new DWORD[cdwLength]) == NULL)
	{
		err_RE_OUT_OF_MEMORY_P1(m_dwBufferSize);
		return;
	}

	if (m_dwMaxLoops < nBlocks/2) // Determine the required number of entries in the m_MapArray
		dwMapSize = m_dwMaxLoops;
	else
		dwMapSize = nBlocks/2;

	if (m_pdwMapArray != NULL) // If for some reason this is not NULL, delete the old buffer
		delete [] m_pdwMapArray;
	if ((m_pdwMapArray = new DWORD[dwMapSize]) == NULL)
	{
		err_RE_OUT_OF_MEMORY_P1(dwMapSize);
		return;
	}
	// 0xFFFFFFFF will indicate that an array element is empty
	memset(m_pdwMapArray, 0xFF, dwMapSize*sizeof(DWORD));

	vNewSeed(&theseed); // Get a new seed value based on the current time
	uiRandScratch = theseed;

	nReadsNeeded = 0;
	for(nWrites = 0; nWrites <= m_dwMaxLoops && !CheckAbort(HERE); )
	{
		if(nReadsNeeded < nBlocks / 2 && nWrites != m_dwMaxLoops)
		{
			do {
				n = random(&uiRandScratch) % nBlocks;
			} while(bLookup(m_pdwMapArray, n));

			vSet(m_pdwMapArray, n);

			for(DWORD i = 0; i < cdwLength; i++)
				m_pdwBuffer1[i] = random(&uiRandScratch);

//ReportDebug(BIT0, L"write block %ld\n", n);

			LocalWrite((DWORDLONG)n * (DWORDLONG)m_dwBufferSize,m_pdwBuffer1,m_dwBufferSize);
			nWrites++;
			nReadsNeeded++;
		}
		else
		{
			uiRandScratch = theseed; // Re-seed the random number generator
			memset(m_pdwMapArray, 0xFF, dwMapSize*sizeof(DWORD));

			for(UINT count = 0; count < nReadsNeeded && !CheckAbort(HERE); count++)
			{
				do {
					n = random(&uiRandScratch) % nBlocks;
				} while(bLookup(m_pdwMapArray, n));

				vSet(m_pdwMapArray, n);

				for(DWORD i = 0; i < cdwLength; i++)
					m_pdwBuffer1[i] = random(&uiRandScratch);

//ReportDebug(BIT0, L"read block %ld\n", n);

				if(LocalRead((DWORDLONG)n * (DWORDLONG)m_dwBufferSize,m_pdwBuffer2,m_dwBufferSize))
				{
					// Compare the buffers as DWORDs
					for(DWORD i = 0; i < cdwLength; i++)
					{
						if(m_pdwBuffer1[i] != m_pdwBuffer2[i])
						{
							DWORDLONG offset = (DWORDLONG)n * (DWORDLONG)m_dwBufferSize + (DWORDLONG)i * 4;
							err_RE_COMPAREBUF_P3(offset, m_pdwBuffer1[i], m_pdwBuffer2[i]);
							break;
						}
					}
				}

			}

			if(nWrites == m_dwMaxLoops)
				nWrites++;
			else
			{
				memset(m_pdwMapArray, 0xFF, dwMapSize*sizeof(DWORD));
				nReadsNeeded = 0;
				vNewSeed(&theseed); // Get a new seed value based on the current time
				uiRandScratch = theseed;
			}
		}
	}

	return;

}

// This "device" creates a file that uses up 90% (overridable) of the remaining
// free space on a drive, allowing read/write testing of the free space on a device.
#ifdef XHD_TAG
bool CXModuleHardDisk::FreeSpaceDeviceOpen()
#else
bool CXModuleDVD::FreeSpaceDeviceOpen()
#endif
{
	BOOL fWrite;
	char MutexName[] = "DiskdrvX";
	char szRootPathName[5];
	LARGE_INTEGER eof;
	DWORD dwBytesPerSector;


	// FreeSpace testing always needs write testing, because it has to
	// create the temporary file, even during read testing.
	fWrite = true;

	sprintf(m_szFilePath, "%S:\\%S", m_chDriveLetter, m_wszFileName);

	m_hDevice = CreateFileA(
		m_szFilePath,
		GENERIC_READ | (fWrite ? GENERIC_WRITE : 0),
		0,
		NULL,
		CREATE_ALWAYS, //TRUNCATE_EXISTING,
		FILE_FLAG_NO_BUFFERING | FILE_FLAG_DELETE_ON_CLOSE | FILE_FLAG_WRITE_THROUGH,
		NULL); 

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		err_OPEN_FILE_P1L(m_szFilePath);
		return false;
	}

	MutexName[strlen(MutexName)-1] = m_szFilePath[0]; // Make the drive letter correct

	// Get access to the mutex for this drive
	if (!bGetPossessionOfHardDiskResources(MutexName))
		return false; // Didn't get mutex access in time

	sprintf(szRootPathName, "%S:\\", m_chDriveLetter);

#if defined( _XBOX ) 
	ULARGE_INTEGER uliFreeBytesAvailable, uliTotalNumberOfBytes;
	if (!GetDiskFreeSpaceExA(
		szRootPathName,
		&uliFreeBytesAvailable,
		&uliTotalNumberOfBytes,
		NULL))
	{
		err_COULDNT_DETERMINE_FREE_SPACE(m_chDriveLetter);
		return false;
	}

	m_qwDeviceSize  = uliFreeBytesAvailable.QuadPart;
	dwBytesPerSector = XGetDiskSectorSizeA(szRootPathName);
#else // Win32
	DWORD dwSectorsPerCluster;
	DWORD dwNumberOfFreeClusters,dwTotalNumberOfClusters;

	GetDiskFreeSpaceA(
		szRootPathName,
		&dwSectorsPerCluster,
		&dwBytesPerSector,
		&dwNumberOfFreeClusters,
		&dwTotalNumberOfClusters);

	m_qwDeviceSize  = (DWORDLONG)dwNumberOfFreeClusters;
	m_qwDeviceSize *= (DWORDLONG)dwSectorsPerCluster;
	m_qwDeviceSize *= (DWORDLONG)dwBytesPerSector;
#endif
	m_qwDeviceSize  = (DWORDLONG)((LONGLONG)m_qwDeviceSize * m_dblFreeSpacePercent / 100.0);
	m_qwDeviceSize -= m_qwDeviceSize % dwBytesPerSector;
	m_dwBufferSize -= m_dwBufferSize % dwBytesPerSector;

	eof.QuadPart = m_qwDeviceSize;

	DWORD result = SetFilePointer(m_hDevice,eof.LowPart,&eof.HighPart,FILE_BEGIN);
	DWORD result2 = GetLastError();

	if((result == 0xFFFFFFFF) && (result2 != NO_ERROR))
	{
		err_RE_EXTEND_FILE_P1L(m_qwDeviceSize);
		return false;
	}

	// Extend the EOF position.
	// For some reason in the March XDK, we can't set the end of file beyond 4 GB.
	// This is interesting because the SetFilePointer works above 4GB.
	if(!SetEndOfFile(m_hDevice))
	{
//		DWORD dwErr = GetLastError();
		err_RE_EXTEND_FILE_P1L(m_qwDeviceSize);
		return false;
	}

	// Clean up the resources we used here
	vReleasePossessionOfHardDiskResources();

	return true;
}

// This opens an existing file for reading.
#ifdef XHD_TAG
bool CXModuleHardDisk::FileReadDeviceOpen()
#else
bool CXModuleDVD::FileReadDeviceOpen()
#endif
{
	BOOL fWrite;
//	char MutexName[] = "DiskdrvX";
	char szRootPathName[5];
	LARGE_INTEGER liFileSize;
	DWORD dwBytesPerSector;


	// FreeSpace testing always needs write testing, because it has to
	// create the temporary file, even during read testing.
	fWrite = false;

	sprintf(m_szFilePath, "%S:\\%S", m_chDriveLetter, m_wszFileName);

	m_hDevice = CreateFileA(
		m_szFilePath,
		GENERIC_READ | (fWrite ? GENERIC_WRITE : 0),
		0,
		NULL,
		OPEN_EXISTING, //TRUNCATE_EXISTING,
		FILE_FLAG_NO_BUFFERING,
		NULL); 

	if (m_hDevice == INVALID_HANDLE_VALUE)
	{
		err_OPEN_FILE_P1L(m_szFilePath);
		return false;
	}

//	MutexName[strlen(MutexName)-1] = m_szFilePath[0]; // Make the drive letter correct

	// Get access to the mutex for this drive
//	if (!bGetPossessionOfHardDiskResources(MutexName))
//		return false; // Didn't get mutex access in time

	sprintf(szRootPathName, "%S:\\", m_chDriveLetter);

#if defined( _XBOX )
/*
	ULARGE_INTEGER uliFreeBytesAvailable, uliTotalNumberOfBytes;
	if (!GetDiskFreeSpaceExA(
		szRootPathName,
		&uliFreeBytesAvailable,
		&uliTotalNumberOfBytes,
		NULL))
	{
		err_COULDNT_DETERMINE_FREE_SPACE(m_chDriveLetter);
		return false;
	}
	

	m_qwDeviceSize  = uliFreeBytesAvailable.QuadPart;
*/
	dwBytesPerSector = XGetDiskSectorSizeA(szRootPathName);
#else // Win32
	DWORD dwSectorsPerCluster;
	DWORD dwNumberOfFreeClusters,dwTotalNumberOfClusters;

	GetDiskFreeSpaceA(
		szRootPathName,
		&dwSectorsPerCluster,
		&dwBytesPerSector,
		&dwNumberOfFreeClusters,
		&dwTotalNumberOfClusters);
/*
	m_qwDeviceSize  = (DWORDLONG)dwNumberOfFreeClusters;
	m_qwDeviceSize *= (DWORDLONG)dwSectorsPerCluster;
	m_qwDeviceSize *= (DWORDLONG)dwBytesPerSector;
*/
#endif

	if (!GetFileSizeEx(m_hDevice, &liFileSize))
	{
		err_COULDNT_DETERMINE_FILE_SIZE(m_szFilePath);
		return false;
	}
	m_qwDeviceSize  = liFileSize.QuadPart;
	m_qwDeviceSize -= m_qwDeviceSize % dwBytesPerSector;
	m_dwBufferSize -= m_dwBufferSize % dwBytesPerSector;
/*
	eof.QuadPart = m_qwDeviceSize;

	DWORD result = SetFilePointer(m_hDevice,eof.LowPart,&eof.HighPart,FILE_BEGIN);
	DWORD result2 = GetLastError();

	if((result == 0xFFFFFFFF) && (result2 != NO_ERROR))
	{
		err_RE_EXTEND_FILE_P1L(m_qwDeviceSize);
		return false;
	}

	// Extend the EOF position.
	// For some reason in the March XDK, we can't set the end of file beyond 4 GB.
	// This is interesting because the SetFilePointer works above 4GB.
	if(!SetEndOfFile(m_hDevice))
	{
//		DWORD dwErr = GetLastError();
		err_RE_EXTEND_FILE_P1L(m_qwDeviceSize);
		return false;
	}

	// Clean up the resources we used here
//	vReleasePossessionOfHardDiskResources();
*/
	return true;
}

#ifdef XHD_TAG
void CXModuleHardDisk::vPhysicalDeviceSequentialRead ()
#else
void CXModuleDVD::vPhysicalDeviceSequentialRead ()
#endif
{
	DWORD cbRemain;
	DWORDLONG nBlocks,nStartBlock,nEndBlock, n;
	LPBYTE lpBuffer;
	DWORD i;

	if(!PhysicalDeviceOpen(false, false))
		return;

	// This is OK as long as the hard drive is < 2048 GB
	nBlocks  = (DWORD)(m_qwDeviceSize / m_dwBufferSize);
	cbRemain = (DWORD)(m_qwDeviceSize % m_dwBufferSize);
	if(nBlocks == 0)
	{
		err_NO_BLOCKS_TO_TEST();
		return;
	}

	nStartBlock = (DWORDLONG)(((LONGLONG)nBlocks - 1) * m_dblStartPercent / 100.0);
	nEndBlock   = (DWORDLONG)(((LONGLONG)nBlocks - 1) * m_dblEndPercent / 100.0);

	ReportDebug(BIT0, L"Starting Percentage = %lg\n"
		L"Ending Percentage = %lg\n"
		L"Total Blocks = %I64d\n"
		L"Starting Block = %I64d\n"
		L"Ending Block = %I64d",
		m_dblStartPercent,
		m_dblEndPercent,
		nBlocks,
		nStartBlock,
		nEndBlock);


	// Change nBlocks to represent actual number of blocks we will test for progress computation.
	nBlocks = (nEndBlock - nStartBlock) + 1;

	if ((m_pdwBuffer1 = new DWORD[m_dwBufferSize/4]) == NULL)
	{
		err_RE_OUT_OF_MEMORY_P1(m_dwBufferSize);
		return;
	}
	lpBuffer = (BYTE *)m_pdwBuffer1; // We use this buffer as a BYTE pointer

	i = 0;
	for(n = nStartBlock; n <= nEndBlock && !CheckAbort(HERE); n++)
	{
		LocalRead((DWORDLONG)n * (DWORDLONG)m_dwBufferSize,m_pdwBuffer1,m_dwBufferSize);
		i++;
		if (i == 200)
		{
			ReportDebug(BIT0, L"reading offset %I64u", (DWORDLONG)n * (DWORDLONG)m_dwBufferSize);
			i = 0;
		}
	}
	
	if((cbRemain != 0) && !CheckAbort(HERE))
		LocalRead((DWORDLONG)n * (DWORDLONG)m_dwBufferSize,m_pdwBuffer1,cbRemain);
}

#ifdef XHD_TAG
bool CXModuleHardDisk::PhysicalDeviceOpen(bool fWrite, bool bDVD)
#else
bool CXModuleDVD::PhysicalDeviceOpen(bool fWrite, bool bDVD)
#endif
{
	DWORD cb;
	OBJECT_ATTRIBUTES ObjAttributes;
	IO_STATUS_BLOCK IoStatusBlock;
	NTSTATUS NtStatus;
	OBJECT_STRING ObjString;
	char szRootPathName[5];

	if (bDVD)
	{
		sprintf(m_szFilePath, "cdrom0:");
		m_hDevice = CreateFileA(
			m_szFilePath,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING, //TRUNCATE_EXISTING,
			FILE_FLAG_NO_BUFFERING,
			NULL); 

		if (m_hDevice == INVALID_HANDLE_VALUE)
		{
			err_OPEN_FILE_P1L(m_szFilePath);
			return false;
		}

		sprintf(szRootPathName, "%S:\\", m_chDriveLetter);
		m_dwBytesPerSector = XGetDiskSectorSizeA(szRootPathName);
		ULARGE_INTEGER uliFreeBytesAvailable, uliTotalNumberOfBytes;
		if (!GetDiskFreeSpaceExA(
			szRootPathName,
			&uliFreeBytesAvailable,
			&uliTotalNumberOfBytes,
			NULL))
		{
			err_COULDNT_DETERMINE_FREE_SPACE(m_chDriveLetter);
			return false;
		}
		m_qwDeviceSize = uliTotalNumberOfBytes.QuadPart;
		m_qwDeviceSize -= m_qwDeviceSize % m_dwBytesPerSector;
		m_dwBufferSize -= m_dwBufferSize % m_dwBytesPerSector;
	}
	else // Hard disk
	{
		sprintf(m_szFilePath, "\\Device\\Harddisk0\\partition%lu", m_dwDriveNumber);

		ObjString.Length = (USHORT)strlen(m_szFilePath);
		ObjString.MaximumLength = ObjString.Length + 1;
		ObjString.Buffer = m_szFilePath;

		ObjAttributes.RootDirectory = NULL;
		ObjAttributes.ObjectName = &ObjString;
		ObjAttributes.Attributes = OBJ_CASE_INSENSITIVE;

		m_hDevice = INVALID_HANDLE_VALUE;
		NtStatus = NtOpenFile(
			&m_hDevice,
			SYNCHRONIZE | FILE_READ_DATA | (fWrite ? FILE_WRITE_DATA : 0),
			&ObjAttributes,
			&IoStatusBlock,
			0,
			FILE_SYNCHRONOUS_IO_ALERT | FILE_NO_INTERMEDIATE_BUFFERING); 

		if (NtStatus != STATUS_SUCCESS)
		{
			err_OPEN_DEVICE_P1L(m_szFilePath);
			return false;
		}

		if(!DeviceIoControl(
			m_hDevice,
			IOCTL_DISK_GET_DRIVE_GEOMETRY,
			NULL,
			0,
			&m_Geometry,
			sizeof(m_Geometry),
			&cb,
			NULL))
		{
			err_RE_DRIVE_GEOMETRY_P1L(m_szFilePath, GetLastError());
			return false;
		}

		m_qwDeviceSize =
			m_Geometry.BytesPerSector    *
			m_Geometry.SectorsPerTrack   *
			m_Geometry.TracksPerCylinder *
			m_Geometry.Cylinders.QuadPart;

		m_dwBufferSize -= m_dwBufferSize % m_Geometry.BytesPerSector;
		m_dwBytesPerSector = m_Geometry.BytesPerSector;
	}

	ReportDebug(BIT0, L"DeviceSize = %I64d bytes", m_qwDeviceSize);

	return true;
}

#ifdef XHD_TAG
void CXModuleHardDisk::vFileSequentialRead ()
#else
void CXModuleDVD::vFileSequentialRead ()
#endif
{
	DWORD         nBlocks, n;
	DWORD         cdwLength;

	if(!FileReadDeviceOpen())
		return;

	cdwLength = m_dwBufferSize / 4;  // Number of DWORDs in the buffer
	// This is OK as long as the hard drive is < 2048 GB
	nBlocks  = (DWORD)(m_qwDeviceSize / m_dwBufferSize);

	if(nBlocks < 2)
	{
		err_NO_BLOCKS_TO_TEST();
		return;
	}


	if (m_pdwBuffer1 != NULL) // If for some reason this is not NULL, delete the old buffer
		delete [] m_pdwBuffer1;
	if ((m_pdwBuffer1 = new DWORD[cdwLength]) == NULL)
	{
		err_RE_OUT_OF_MEMORY_P1(m_dwBufferSize);
		return;
	}

	for(n = 0; n <= nBlocks && !CheckAbort(HERE); n++)
	{
		if(LocalRead((DWORDLONG)n * (DWORDLONG)m_dwBufferSize,m_pdwBuffer1,m_dwBufferSize))
		{
			// Don't do anything with the data right now, we just want to read
		}
		else
			break;
	}
	// We aren't going to worry about reading the last partial block because
	// it should only be a small fraction of the large size of the files that
	// this test is intended to read.

	return;

}

////////////////////////////////////////////////////////////////////////////////
// qwPosition and cbLength must be multiples of the sector size of the device.
// m_szFilePath must be defined for error reporting
#ifdef XHD_TAG
bool CXModuleHardDisk::LocalWrite(LONGLONG qwPosition,LPVOID lpBuffer,DWORD cbLength)
#else
bool CXModuleDVD::LocalWrite(LONGLONG qwPosition,LPVOID lpBuffer,DWORD cbLength)
#endif
{
	DWORD cb;

	LARGE_INTEGER qwOffset;
	LARGE_INTEGER qwSave;
	DWORD dwLastError;

	qwOffset.QuadPart = qwPosition;
	qwSave.QuadPart = qwPosition;

	qwOffset.LowPart =
		SetFilePointer(m_hDevice,qwOffset.LowPart,&qwOffset.HighPart,FILE_BEGIN);

//	if((qwSave.QuadPart != qwOffset.QuadPart) || (GetLastError() != NO_ERROR))
	if ((qwOffset.LowPart == INVALID_SET_FILE_POINTER) && ((dwLastError = GetLastError()) != NO_ERROR))
	{
		err_RE_SET_FILE_POINTER((DWORDLONG)qwSave.QuadPart, dwLastError);
		return false;
	}

	if(!WriteFile(
		m_hDevice,
		lpBuffer,
		cbLength,
		&cb,
		NULL))
	{
		err_RE_WRITE_LOGICAL_P3L((m_szFilePath[0] != 0) ? m_szFilePath : "Unknown", cbLength, (DWORDLONG)qwPosition);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// qwPosition and cbLength must be multiples of the sector size of the device.
// m_szFilePath must be defined for error reporting
#ifdef XHD_TAG
bool CXModuleHardDisk::LocalRead(LONGLONG qwPosition,LPVOID lpBuffer,DWORD cbLength)
#else
bool CXModuleDVD::LocalRead(LONGLONG qwPosition,LPVOID lpBuffer,DWORD cbLength)
#endif
{
	DWORD cb;

	LARGE_INTEGER qwOffset;
	LARGE_INTEGER qwSave;
	DWORD dwLastError;

	qwOffset.QuadPart = qwPosition;
	qwSave.QuadPart = qwPosition;

	qwOffset.LowPart =
		SetFilePointer(m_hDevice,qwOffset.LowPart,&qwOffset.HighPart,FILE_BEGIN);

//	if((qwSave.QuadPart != qwOffset.QuadPart) || (dwLastError = GetLastError() != NO_ERROR))
	if ((qwOffset.LowPart == INVALID_SET_FILE_POINTER) && ((dwLastError = GetLastError()) != NO_ERROR))
	{
		err_RE_SET_FILE_POINTER((DWORDLONG)qwSave.QuadPart, dwLastError);
		return false;
	}

	if(!ReadFile(
		m_hDevice,
		lpBuffer,
		cbLength,
		&cb,
		NULL))
	{
		err_RE_READ_LOGICAL_P4L((m_szFilePath[0] != 0) ? m_szFilePath : "Unknown", cbLength, (DWORDLONG)qwPosition, GetLastError());
		return false;
	}

	return true;
}

#ifdef XHD_TAG
void CXModuleHardDisk::vNewSeed(UINT *pSeed)
#else
void CXModuleDVD::vNewSeed(UINT *pSeed)
#endif
{
	SYSTEMTIME systimeSeed;
	FILETIME filetimeSeed;

	GetSystemTime(&systimeSeed);
	SystemTimeToFileTime(&systimeSeed, &filetimeSeed);
	*pSeed = (UINT)filetimeSeed.dwLowDateTime;

	return;
}

#ifdef XHD_TAG
DWORD CXModuleHardDisk::random(UINT *pScratch)
#else
DWORD CXModuleDVD::random(UINT *pScratch)
#endif
{
	DWORD ret;
	ret  = (*pScratch = *pScratch * 214013L + 2531011L) >> 16;
	ret |= (*pScratch = *pScratch * 214013L + 2531011L) & 0xFFFF0000;
	return ret;
}

// pArray must contain 0xFFFFFFFF as its list terminator
#ifdef XHD_TAG
bool CXModuleHardDisk::bLookup(DWORD *pArray, DWORD dwValue)
#else
bool CXModuleDVD::bLookup(DWORD *pArray, DWORD dwValue)
#endif
{
	DWORD i = 0;

	// Find either the next empty location in the list, or the first instance of this dwValue
	while ((pArray[i] != 0xFFFFFFFF) && (pArray[i] != dwValue))
		i++;
	if (pArray[i] == 0xFFFFFFFF)
		return false;
	else
		return true;
}

// pArray must contain 0xFFFFFFFF as its list terminator
#ifdef XHD_TAG
void CXModuleHardDisk::vSet(DWORD *pArray, DWORD dwValue)
#else
void CXModuleDVD::vSet(DWORD *pArray, DWORD dwValue)
#endif
{
	DWORD i = 0;

	// Find either the next empty location in the list, or the first instance of this dwValue
	while ((pArray[i] != 0xFFFFFFFF) && (pArray[i] != dwValue))
		i++;
	if (pArray[i] == 0xFFFFFFFF)
		pArray[i] = dwValue;
}

// This function will checksum the contents and names of all files listed
// in configuration parameters.  This does NOT recurse into subdirectories,
// so all subdirectories that need to be checksummed should be included in
// the parameter list.
// The function will also checksum specified ranges of Logical Blocks on the media
// and combine the checksum with any checksum calculated from the file checksumming
// portion of the function.

#ifdef XHD_TAG
void CXModuleHardDisk::vPerformChecksum ()
{
	bool bChecksumDVD = false;
#else
void CXModuleDVD::vPerformChecksum ()
{
	bool bChecksumDVD = true;
#endif
	#define READ_BUFFER_SIZE 524288 // Half a MB should be good to start with
	WIN32_FIND_DATA FindFileData;
	BOOL bFileResult;
	DWORD dwChecksum = 0;
	DWORD dwFileAttributes, dwFileSize, dwReadThisTime, dwBytesRead, dwBytesReadSoFar;
	HANDLE hFile;
	char *pcDataBuffer;
	DWORD i, dwLastError;
	DWORD dwTotalRangeSize, dwNumBytesToRead;
	DWORDLONG dwlOffsetToRead;
	char szWildcard[MAX_PATH];
	char szFullPath[MAX_PATH];
	wchar_t wszTempString[256];
	LPCTSTR pwszTemp = NULL;

	if ((pcDataBuffer = new char[READ_BUFFER_SIZE]) == NULL)
	{
		// Return a memory allocation error
		err_RE_OUT_OF_MEMORY_P1(READ_BUFFER_SIZE);
		return;
	}
	if (m_pwszChecksumPath != NULL)
	{
		i = 0;
		while (m_pwszChecksumPath[i] != NULL)
		{
			sprintf(szWildcard, "%S\\*", m_pwszChecksumPath[i]);

			if ((hDir = FindFirstFileA(szWildcard, &FindFileData)) == INVALID_HANDLE_VALUE)
			{
				dwLastError = GetLastError();
				if (err_RE_FILE_NOT_FOUND_P2(szWildcard, dwLastError))
				{
					delete [] pcDataBuffer;
					return;
				}
			}
			else
			{
				do {
//					ReportDebug(BIT0, L"Checksum is: 0x%8.8lx", dwChecksum);
					sprintf(szFullPath, "%S\\%s", m_pwszChecksumPath[i], FindFileData.cFileName);
					// Add the name of the file or directory to the checksum
					vAddToChecksum(szFullPath, strlen(szFullPath), &dwChecksum);
					// Check to see if this is a file or directory
					if ((dwFileAttributes = GetFileAttributes(szFullPath)) != (DWORD)(-1))
					{
						if ((dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // This is not a directory
						{
							ReportDebug(BIT0, L"Found file %S", szFullPath);
							// Now open the file for reading
							hFile = CreateFileA (szFullPath,
								GENERIC_READ, //|GENERIC_WRITE,
								0, NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
							if (hFile != INVALID_HANDLE_VALUE)
							{
								if ((dwFileSize = GetFileSize (hFile, NULL)) != -1) // Don't worry about files larger than 4 GB
								{
//									ReportDebug(BIT0, L"%S is %lu bytes", szFullPath, dwFileSize);
									dwBytesReadSoFar = 0;
									while (dwFileSize > 0)
									{
										if (dwFileSize > READ_BUFFER_SIZE)
											dwReadThisTime = READ_BUFFER_SIZE;
										else
											dwReadThisTime = dwFileSize;
										if (ReadFile (hFile, pcDataBuffer, dwReadThisTime, &dwBytesRead, NULL))
										{
											// Add the read data to the checksum
											vAddToChecksum(pcDataBuffer, dwReadThisTime, &dwChecksum);
										}
										else
										{
											err_RE_READ_LOGICAL_P4L(szFullPath, dwReadThisTime, (DWORDLONG)dwBytesReadSoFar, GetLastError());
											CloseHandle(hFile);
											FindClose(hDir);
											delete [] pcDataBuffer;
											return;
										}
										dwFileSize = dwFileSize - dwBytesRead;
										dwBytesReadSoFar = dwBytesReadSoFar + dwBytesRead;
									}
								}
								else // Could not get the file size
								{
									err_COULDNT_DETERMINE_FILE_SIZE(szFullPath);
									CloseHandle(hFile);
									FindClose(hDir);
									delete [] pcDataBuffer;
									return;
								}
								CloseHandle(hFile); // Close out the handle
							}
							else  // We were not able to open the file
							{
								err_OPEN_DEVICE_P1L(szFullPath);
								FindClose(hDir);
								delete [] pcDataBuffer;
								return;
							}
						}
						else // This is a directory
						{
							ReportDebug(BIT0, L"Found directory %S", szFullPath);
						}
					}
					else // Error getting file attributes
					{
						err_GETTING_FILE_ATTRIBUTES_P2(szFullPath);
						FindClose(hDir);
						delete [] pcDataBuffer;
						return;
					}
				}while ((bFileResult = FindNextFile(hDir, &FindFileData)) == TRUE);
				if ((dwLastError = GetLastError()) != ERROR_NO_MORE_FILES) // Some error happened looking for files in this directory
				{
					if (err_FILE_ENUMERATION_P2(szWildcard, dwLastError))
					{
						FindClose(hDir);
						delete [] pcDataBuffer;
						return;
					}
				}
			}
			FindClose(hDir);
			i++;
		}
	}
	// Now checksum the logical blocks, if any, that were specified
	if (m_dwNumOfChecksumSectorRanges > 0)
	{
		if(!PhysicalDeviceOpen(false, bChecksumDVD))
		{
			delete [] pcDataBuffer;
			return;
		}
		for (i = 0; i < m_dwNumOfChecksumSectorRanges; i++)
		{
			dwlOffsetToRead = (DWORDLONG)m_pdwChecksumSectorStart[i] * (DWORDLONG)m_dwBytesPerSector;
			dwTotalRangeSize = ((m_pdwChecksumSectorEnd[i] - m_pdwChecksumSectorStart[i]) + 1) * m_dwBytesPerSector;
			while (dwTotalRangeSize > 0)
			{
				dwNumBytesToRead = dwTotalRangeSize;
				if (dwTotalRangeSize > READ_BUFFER_SIZE)
					dwNumBytesToRead = READ_BUFFER_SIZE;
				ReportDebug(BIT0, L"Starting sector 0x%lx, reading 0x%lx sectors", (DWORD)(dwlOffsetToRead/(DWORDLONG)m_dwBytesPerSector), (DWORD)(dwNumBytesToRead/(DWORDLONG)m_dwBytesPerSector));
				if(LocalRead(dwlOffsetToRead,pcDataBuffer,dwNumBytesToRead))
				{
					vAddToChecksum(pcDataBuffer, dwNumBytesToRead, &dwChecksum);
				}
				else // Read error
				{
					delete [] pcDataBuffer;
					return;
				}
				dwTotalRangeSize = dwTotalRangeSize - dwNumBytesToRead;
				dwlOffsetToRead = dwlOffsetToRead + (DWORDLONG)dwNumBytesToRead;
			}
		}
	}

	delete [] pcDataBuffer;

	// Now compare the checksum with the valid, expected checksums
	if (m_dwNumOfChecksums != 0)
	{
		if (m_pdwChecksumAllowed != NULL)
		{
			if (m_pdwChecksumAllowed[0] != 0) // The first checksum of 0 is a special case that indicates not to do a checksum comparison
			{
				for (i = 0; i < m_dwNumOfChecksums; i++)
				{
					if (m_pdwChecksumAllowed[i] == dwChecksum)
					{


						swprintf(wszTempString, L"ChecksumKernelRevisionAllowed[%2.2lu]", i);
						if ((pwszTemp = GetCfgString (wszTempString, NULL)) != NULL)
						{
							#define OS_VERSION_SIZE           20
							wchar_t XBoxOSVersion[OS_VERSION_SIZE+1];

							// Now get the actual revision information from the Kernel
							memset(XBoxOSVersion, 0, sizeof(XBoxOSVersion)); // Clear out the buffer first
							_snwprintf(XBoxOSVersion, sizeof(XBoxOSVersion)/sizeof(wchar_t), L"%d.%02d.%d.%02d", XboxKrnlVersion->Major, XboxKrnlVersion->Minor, XboxKrnlVersion->Build, (XboxKrnlVersion->Qfe & 0x7FFF));
							if(wcsicmp(pwszTemp, XBoxOSVersion) != 0) // Strings do not match
							{
								err_WRONG_CHECKSUM_FOR_KERNEL_P3(dwChecksum, pwszTemp, XBoxOSVersion);
								return;
							}
						}
						break;

					}
				}
				if (i == m_dwNumOfChecksums)
				{
					err_CHECKSUM_BAD_P1(dwChecksum);
					return;
				}
			}
		}
	}
}

// Add all DWORDs in the buffer passed in.  All dwLengthToAdd
// bytes are added, even if the total is not an even number of DWORDs
#ifdef XHD_TAG
void CXModuleHardDisk::vAddToChecksum (char *cStringToAdd, DWORD dwLengthToAdd, DWORD *pdwChecksum)
#else
void CXModuleDVD::vAddToChecksum (char *cStringToAdd, DWORD dwLengthToAdd, DWORD *pdwChecksum)
#endif
{
	DWORD i, dwRemainder, dwRemValue;
	DWORD *pdwLocal = (DWORD *)cStringToAdd;

//	ReportDebug(BIT0, _T("Adding a buffer of %lu bytes to the checksum"), dwLengthToAdd);
	// This will add all but the last fractional DWORD
	for (i = 0; i < dwLengthToAdd/sizeof(DWORD); i++)
	{
		*pdwChecksum = *pdwChecksum + pdwLocal[i];
	}
	dwRemainder = dwLengthToAdd - (i*sizeof(DWORD));
	if (dwRemainder > 0)
	{
		dwRemValue = 0;
		memcpy(&dwRemValue, &pdwLocal[i], dwRemainder);
		*pdwChecksum = *pdwChecksum + dwRemValue;
	}
}
#endif // XHD_MAIN
#undef XHD_MAIN