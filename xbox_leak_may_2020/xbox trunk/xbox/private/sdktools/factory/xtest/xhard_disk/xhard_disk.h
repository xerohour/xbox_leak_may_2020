// xHard_Disk.h - Hard Disk test

#if !defined(_XHARD_DISK_H_)
#define _XHARD_DISK_H_

// supress futile warnings about assignment operators
#pragma warning (disable:4512)

#include "..\parameter.h"


#ifndef PASS
#define PASS 0
#endif
#ifndef FAIL
#define FAIL 0xFFFFFFFF
#endif

#define XHARD_DISK_MODNUM   14
#define XDVD_MODNUM   17


extern "C"
{
typedef struct _XBOX_KRNL_VERSION {
    USHORT Major;
    USHORT Minor;
    USHORT Build;
    USHORT Qfe;
} XBOX_KRNL_VERSION, *PXBOX_KRNL_VERSION;

extern const PXBOX_KRNL_VERSION XboxKrnlVersion;
} // end extern "C"

// These are a bunch of OS defines that allow us to access the hard drive at the physical level
// ********************************************************************************
// ********************************************************************************
// ********************************************************************************
#define InitializeObjectAttributes(p, n, a, r, s){\
	(p)->RootDirectory = r;   \
	(p)->Attributes = a;      \
	(p)->ObjectName = n;      \
	}

typedef struct _LSA_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PCHAR Buffer;
} LSA_STRING, *PLSA_STRING;

#ifndef _NTDEF_
typedef LSA_STRING STRING, *PSTRING ;
#endif

typedef STRING OBJECT_STRING;
typedef PSTRING POBJECT_STRING;

typedef struct _OBJECT_ATTRIBUTES {
	HANDLE RootDirectory;
	POBJECT_STRING ObjectName;
	ULONG Attributes;
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

// NTSTATUS
//
typedef LONG NTSTATUS;
/*lint -save -e624 */  // Don't complain about different typedefs.
typedef NTSTATUS *PNTSTATUS;
/*lint -restore */  // Resume checking for different typedefs.


// Other defines for accessing Nt kernal drive routines
#define STATUS_SUCCESS                          ((NTSTATUS)0x00000000L) // ntsubauth
#define FILE_READ_DATA            ( 0x0001 )    // file & pipe
#define FILE_WRITE_DATA           ( 0x0002 )    // file & pipe
#define SYNCHRONIZE                      (0x00100000L)
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008
#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define OBJ_CASE_INSENSITIVE                    0x00000040L

//
//  Status values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-------------------------+-------------------------------+
//  |Sev|C|       Facility          |               Code            |
//  +---+-+-------------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
 
//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

// begin_ntddk begin_wdm begin_nthal
//
// Define the base asynchronous I/O argument types
//
typedef struct _IO_STATUS_BLOCK {
	union {
		NTSTATUS Status;
		PVOID Pointer;
	};

	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;
#define NTSYSCALLAPI DECLSPEC_IMPORT

extern "C"
{
NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess, // SYNCHRONIZE | FILE_READ_DATA
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG ShareAccess, // 0
    IN ULONG OpenOptions // FILE_SYNCHRONOUS_IO_ALERT | FILE_NO_INTERMEDIATE_BUFFERING
    );
}

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#define FILE_DEVICE_DISK                0x00000007
#define IOCTL_DISK_BASE                 FILE_DEVICE_DISK
#define METHOD_BUFFERED                 0
#define FILE_ANY_ACCESS                 0
#define IOCTL_DISK_GET_DRIVE_GEOMETRY   CTL_CODE(IOCTL_DISK_BASE, 0x0000, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef enum _MEDIA_TYPE {
    Unknown,                // Format is unknown
    F5_1Pt2_512,            // 5.25", 1.2MB,  512 bytes/sector
    F3_1Pt44_512,           // 3.5",  1.44MB, 512 bytes/sector
    F3_2Pt88_512,           // 3.5",  2.88MB, 512 bytes/sector
    F3_20Pt8_512,           // 3.5",  20.8MB, 512 bytes/sector
    F3_720_512,             // 3.5",  720KB,  512 bytes/sector
    F5_360_512,             // 5.25", 360KB,  512 bytes/sector
    F5_320_512,             // 5.25", 320KB,  512 bytes/sector
    F5_320_1024,            // 5.25", 320KB,  1024 bytes/sector
    F5_180_512,             // 5.25", 180KB,  512 bytes/sector
    F5_160_512,             // 5.25", 160KB,  512 bytes/sector
    RemovableMedia,         // Removable media other than floppy
    FixedMedia,             // Fixed hard disk media
    F3_120M_512,            // 3.5", 120M Floppy
    F3_640_512,             // 3.5" ,  640KB,  512 bytes/sector
    F5_640_512,             // 5.25",  640KB,  512 bytes/sector
    F5_720_512,             // 5.25",  720KB,  512 bytes/sector
    F3_1Pt2_512,            // 3.5" ,  1.2Mb,  512 bytes/sector
    F3_1Pt23_1024,          // 3.5" ,  1.23Mb, 1024 bytes/sector
    F5_1Pt23_1024,          // 5.25",  1.23MB, 1024 bytes/sector
    F3_128Mb_512,           // 3.5" MO 128Mb   512 bytes/sector
    F3_230Mb_512,           // 3.5" MO 230Mb   512 bytes/sector
    F8_256_128              // 8",     256KB,  128 bytes/sector
} MEDIA_TYPE, *PMEDIA_TYPE;

typedef struct _DISK_GEOMETRY {
    LARGE_INTEGER Cylinders;
    MEDIA_TYPE MediaType;
    DWORD TracksPerCylinder;
    DWORD SectorsPerTrack;
    DWORD BytesPerSector;
} DISK_GEOMETRY, *PDISK_GEOMETRY;

// ********************************************************************************
// ********************************************************************************
// ********************************************************************************


// Naming of class test and module classes.
// 
// module class: CX<module>
// test class:   CX<module>Test<test>
//
// The names are important because the modules and exective are 
// all one project.  Since the classes are presented alphebetically, 
// using this naming convention makes the code easier to navigate.
//
#ifdef XHD_TAG
class CXModuleHardDisk : public CTestObj
{
public:
	DECLARE_XMTAMODULE (CXModuleHardDisk, "HardDisk", XHARD_DISK_MODNUM);  //(module name, module number)
#else
class CXModuleDVD : public CTestObj
{
public:
	DECLARE_XMTAMODULE (CXModuleDVD, "DVD", XDVD_MODNUM);  //(module name, module number)
#endif

protected:
	// Error messages declared here ...
	//(Note: 0x000 - 0x00f reserved for XMTA)
	//

	// Error routines return true if the calling routine should abort, and false otherwise
	bool err_BADPARAMETER (LPCTSTR s1)
	{
		ReportError (0x010, L"Configuration parameter \"%s\" doesn't exist or is invalid", s1); return (CheckAbort (HERE));
	}
	bool err_OPEN_FILE_P1L (char *s1)
	{
		ReportError (0x011, L"Failed to open the file \"%S\" that is needed to perform the drive test", s1); return (CheckAbort (HERE));
	}
	bool err_MutexHandle (char *s1)
	{
		ReportError (0x012,L"Could not get a handle for mutex %S ", s1); return (CheckAbort (HERE));
	}
	bool err_MutexTimeout (char *s1)
	{
		ReportError (0x013,L"Resource conflict, timed out waiting for mutex %S", s1); return (CheckAbort (HERE));
	}
	bool err_COULDNT_DETERMINE_FREE_SPACE (LPCTSTR s1)
	{
		ReportError (0x014, L"Could not determine how much free space is available on drive \"%s\"", s1); return (CheckAbort (HERE));
	}
	bool err_RE_EXTEND_FILE_P1L (DWORDLONG dwl1)
	{
		ReportError (0x015, L"Failed to extend free space file size to %I64u bytes.", dwl1); return (CheckAbort (HERE));
	}
	bool err_NO_BLOCKS_TO_TEST ()
	{
		ReportError (0x016, L"No blocks to test!  Reduce the BufferSize parameter to create more blocks."); return (CheckAbort (HERE));
	}
	bool err_RE_OUT_OF_MEMORY_P1 (DWORD dw1)
	{
		ReportError (0x017, L"Out-of-memory error.  Could not allocate %d bytes of memory.", dw1); return (CheckAbort (HERE));
	}
	bool err_RE_COMPAREBUF_P3 (DWORDLONG dwl1, DWORD dw1, DWORD dw2)
	{
		ReportError (0x018, L"Data miscompare @ device offset %I64u.\nExpected:0x%08X   Received:0x%08X", dwl1, dw1, dw2); return (CheckAbort (HERE));
	}
	bool err_RE_SET_FILE_POINTER (DWORDLONG dwl1, DWORD dw1)
	{
		ReportError (0x019, L"Failed to set file to offset %I64u.  GetLastError() returned %lu", dwl1, dw1); return (CheckAbort (HERE));
	}
	bool err_RE_WRITE_LOGICAL_P3L (char *cp1, DWORD dw1, DWORDLONG dwl1)
	{
		ReportError (0x01A, L"Error writing device \"%S\".\nCould not write %lu bytes to offset %I64u.", cp1, dw1, dwl1); return (CheckAbort (HERE));
	}
	bool err_RE_READ_LOGICAL_P4L (char *cp1, DWORD dw1, DWORDLONG dwl1, DWORD dw2)
	{
		ReportError (0x01B, L"Error reading device \"%S\".\nCould not read %u bytes from offset %I64u.  GetLastError() returned %lu", cp1, dw1, dwl1, dw2); return (CheckAbort (HERE));
	}
	bool err_OPEN_DEVICE_P1L (char *s1)
	{
		ReportError (0x01C, L"Failed to open device \"%S\" that is needed to perform this test", s1); return (CheckAbort (HERE));
	}
	bool err_RE_FILE_NOT_FOUND_P2 (char *s1, DWORD dw1)
	{
		ReportError (0x01D, L"No files found of type \"%S\", GetLastError() returned 0x%8.8lx", s1, dw1); return (CheckAbort (HERE));
	}
	bool err_RE_START_END_PERCENTAGE_P2 (double dbl1, double dbl2)
	{
		ReportError (0x01E, L"Starting percentage must be less than ending percentage.\nStarting Percentage = %lg   Ending Percentage = %lg", dbl1, dbl2); return (CheckAbort (HERE));
	}
	bool err_RE_DRIVE_GEOMETRY_P1L (char *s1, DWORD dw1)
	{
		ReportError (0x01F, L"GetDriveGeometry() failed for device \"%S\", GetLastError() returned 0x%8.8lx", s1, dw1); return (CheckAbort (HERE));
	}
	bool err_COULDNT_DETERMINE_FILE_SIZE (char *s1)
	{
		ReportError (0x020, L"Could not determine the size of file \"%S\"", s1); return (CheckAbort (HERE));
	}
	bool err_FILE_ENUMERATION_P2 (char *s1, DWORD dw1)
	{
		ReportError (0x021, L"An error happened while trying to find files of the type \"%S\", GetLastError() returned 0x%8.8lx", s1, dw1); return (CheckAbort (HERE));
	}
	bool err_GETTING_FILE_ATTRIBUTES_P2 (char *s1)
	{
		ReportError (0x022, L"Could not determine the attributes of file \"%S\"", s1); return (CheckAbort (HERE));
	}
	bool err_NOTHING_TO_CHECKSUM ()
	{
		ReportError (0x023, L"No files or directories were specified to checksum"); return (CheckAbort (HERE));
	}
	bool err_NO_CHECKSUMS ()
	{
		ReportError (0x024, L"No checksum values were specified"); return (CheckAbort (HERE));
	}
	bool err_CHECKSUM_BAD_P1 (DWORD dw1)
	{
		ReportError (0x025, L"The computed checksum 0x%8.8lx is not in the list of valid checksums", dw1); return (CheckAbort (HERE));
	}
	bool err_TEST_TIME_TOO_LONG (DWORD dw1, DWORD dw2)
	{
		ReportError (0x026, L"The test took %lu seconds to complete, which is longer than the maximum allowed time of %lu seconds", dw1, dw2); return (CheckAbort (HERE));
	}
	bool err_TEST_TIME_TOO_SHORT (DWORD dw1, DWORD dw2)
	{
		ReportError (0x027, L"The test took %lu seconds to complete, which is shorter than the minimum allowed time of %lu seconds", dw1, dw2); return (CheckAbort (HERE));
	}
	bool err_WRONG_CHECKSUM_FOR_KERNEL_P3 (DWORD dw1, LPCTSTR s1, LPCTSTR s2)
	{
		ReportError (0x028, L"The computed checksum 0x%8.8lx is valid for kernel %s, not kernel %s", dw1, s1, s2); return (CheckAbort (HERE));
	}



	// Parameters declared here ...
	//
	wchar_t m_chDriveLetter[2];
	DWORD m_dwDriveNumber;
	DWORD m_dwRandomSeed;
	DWORD m_dwNumberOfLoopsBetweenSpinDowns;
	DWORD m_dwSpinDownDelay;
	double m_dblFreeSpacePercent;
	double m_dblStartPercent, m_dblEndPercent;
	DWORD m_dwMaxLoops;
	DWORD m_dwMutexTimeout;
	LPCTSTR m_wszFileName;
	char m_szFilePath[MAX_PATH];
	HANDLE m_hDevice;
	HANDLE m_hXMTAHardDiskMutex;
	DWORDLONG m_qwDeviceSize;
	DWORD m_dwBufferSize;
	DISK_GEOMETRY m_Geometry;
	DWORD m_dwBytesPerSector;
	LPDWORD m_pdwBuffer1, m_pdwBuffer2, m_pdwMapArray;
	DWORD m_dwNumOfChecksums;
	DWORD *m_pdwChecksumAllowed;
	wchar_t **m_pwszChecksumPath;
	DWORD m_dwNumOfDVDChecksums;
	DWORD *m_pdwDVDChecksumAllowed;
	DWORD m_dwNumOfCDChecksums;
	DWORD *m_pdwCDChecksumAllowed;
	DWORD m_dwNumOfChecksumSectorRanges;
	DWORD *m_pdwChecksumSectorStart;
	DWORD *m_pdwChecksumSectorEnd;
	DWORD m_dwMinTestTime;
	DWORD m_dwMaxTestTime;
	DWORDLONG m_dwlStartTime;
	HANDLE hDir;

	virtual bool InitializeParameters ();
public:
	// Common functions go here.  Put any functions that need 
	// to be global to the tests in the module class
	//
#ifdef XHD_TAG
	virtual ~CXModuleHardDisk () // Destructor
#else
	virtual ~CXModuleDVD () // Destructor
#endif
	{
		if (m_pdwDVDChecksumAllowed != NULL)
		{
			delete [] m_pdwDVDChecksumAllowed;
			m_pdwDVDChecksumAllowed = NULL;
		}
		if (m_pdwCDChecksumAllowed != NULL)
		{
			delete [] m_pdwCDChecksumAllowed;
			m_pdwCDChecksumAllowed = NULL;
		}
		if (m_pdwChecksumAllowed != NULL)
		{
			delete [] m_pdwChecksumAllowed;
			m_pdwChecksumAllowed = NULL;
		}
		if (m_pwszChecksumPath != NULL)
		{
			delete [] m_pwszChecksumPath;
			m_pwszChecksumPath = NULL;
		}
		if (m_pdwChecksumSectorStart != NULL)
		{
			delete [] m_pdwChecksumSectorStart;
			m_pdwChecksumSectorStart = NULL;
		}
		if (m_pdwChecksumSectorEnd != NULL)
		{
			delete [] m_pdwChecksumSectorEnd;
			m_pdwChecksumSectorEnd = NULL;
		}
	}
	void vInitializeTestVariables ();
	void vCleanup();
	bool bGetPossessionOfHardDiskResources (char *MutexName);
	void vReleasePossessionOfHardDiskResources ();
	void vCompareTestTime();
	void vDVDRandomRead();
	void vFreeSpaceRandomWRC();
	bool FreeSpaceDeviceOpen();
	bool FileReadDeviceOpen();
	void vPhysicalDeviceSequentialRead ();
	bool PhysicalDeviceOpen(bool fWrite, bool bDVD);
	void vFileSequentialRead();
	bool LocalWrite(LONGLONG qwPosition,LPVOID lpBuffer,DWORD cbLength);
	bool LocalRead(LONGLONG qwPosition,LPVOID lpBuffer,DWORD cbLength);
	void vNewSeed(UINT *pSeed);
	DWORD random(UINT *pScratch);
	bool bLookup(DWORD *pArray, DWORD dwValue);
	void vSet(DWORD *pArray, DWORD dwValue);
	void vPerformChecksum ();
	void vAddToChecksum (char *cStringToAdd, DWORD dwLengthToAdd, DWORD *pdwChecksum);
};

#endif // _XHARD_DISK_H_
