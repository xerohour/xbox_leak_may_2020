//
//	hdperf.cpp
//	Hard drive performance test using low level commands
//

#include <ntos.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntddscsi.h>
#include <scsi.h>
#include <align.h>
#include <stdio.h>
#include <stdlib.h>
#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <dxconio.h>
#include <xlogconio.h>


#define KB              1024
#define MB              (KB*KB)
#define MINBUFSIZE      (4*KB)
#define MAXBUFSIZE      (128*KB)
#define TESTFILESIZE    (64*MB)
#define TESTDIRNAME     "z:\\test"
#define TESTFILENAME    "z:\\perftest.dat"
#define SAMPLEBLOCKSIZE (256*MB)
#define SAMPLEREADSIZE  (16*MB)
#define SECTORSIZE      512
#define MAXSMALLFILESIZE (4*KB)
#define FILECOUNT        2048
#define READBUFSIZE     (4*KB)
#define WRITEBUFSIZE    512
#define CACHEFILESIZE   (256*KB)
#define STARTINGPOS     (16*MB)

// AT Commands
#define AT_STANDBY_IMMEDIATE	0xE0	// 0x94 or 0xE0
#define AT_IDLE_IMMEDIATE		0xE1	// 0x95 or 0xE1
#define AT_STANDBY				0xE2	// 0x96 or 0xE2
#define AT_IDLE					0xE3	// 0x97 or 0xE3
#define AT_CHECK_POWER_MODE		0xE5	// 0x98 or 0xE5


// Globals
static HANDLE gLogHandle;
static DISK_GEOMETRY gDiskGeometry;
static LARGE_INTEGER gFrequency;


VOID
LogAtaPassThrough(ATA_PASS_THROUGH AtaPassThrough) {
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "AtaPassThrough.IdeReg.bFeaturesReg = %#x", AtaPassThrough.IdeReg.bFeaturesReg);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "AtaPassThrough.IdeReg.bSectorCountReg = %#x", AtaPassThrough.IdeReg.bSectorCountReg);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "AtaPassThrough.IdeReg.bSectorNumberReg = %#x", AtaPassThrough.IdeReg.bSectorNumberReg);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "AtaPassThrough.IdeReg.bCylLowReg = %#x", AtaPassThrough.IdeReg.bCylLowReg);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "AtaPassThrough.IdeReg.bCylHighReg = %#x", AtaPassThrough.IdeReg.bCylHighReg);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "AtaPassThrough.IdeReg.bDriveHeadReg = %#x", AtaPassThrough.IdeReg.bDriveHeadReg);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "AtaPassThrough.IdeReg.bCommandReg = %#x", AtaPassThrough.IdeReg.bCommandReg);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "AtaPassThrough.IdeReg.bHostSendsData = %#x", AtaPassThrough.IdeReg.bHostSendsData);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "AtaPassThrough.DataBufferSize = %#x", AtaPassThrough.DataBufferSize);
}

VOID
LogDiskCacheInformation(DISK_CACHE_INFORMATION DiskCacheInformation) {
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "DiskCacheInformation.ParametersSavable = %#x", DiskCacheInformation.ParametersSavable);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "DiskCacheInformation.ReadCacheEnabled = %#x", DiskCacheInformation.ReadCacheEnabled);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "DiskCacheInformation.WriteCacheEnabled = %#x", DiskCacheInformation.WriteCacheEnabled);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "DiskCacheInformation.ReadRetentionPriority = %#x", DiskCacheInformation.ReadRetentionPriority);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "DiskCacheInformation.WriteRetentionPriority = %#x", DiskCacheInformation.WriteRetentionPriority);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "DiskCacheInformation.DisablePrefetchTransferLength = %#x", DiskCacheInformation.DisablePrefetchTransferLength);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "DiskCacheInformation.PrefetchScalar = %#x", DiskCacheInformation.PrefetchScalar);
	if(DiskCacheInformation.PrefetchScalar == TRUE ) {
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "DiskCacheInformation.ScalarPrefetch.Minimum = %#x", DiskCacheInformation.ScalarPrefetch.Minimum);
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "DiskCacheInformation.ScalarPrefetch.Maximum = %#x", DiskCacheInformation.ScalarPrefetch.Maximum);
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "DiskCacheInformation.ScalarPrefetch.MaximumBlocks = %#x", DiskCacheInformation.ScalarPrefetch.MaximumBlocks);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "DiskCacheInformation.BlockPrefetch.Minimum = %#x", DiskCacheInformation.BlockPrefetch.Minimum);
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "DiskCacheInformation.BlockPrefetch.Maximum = %#x", DiskCacheInformation.BlockPrefetch.Maximum);
	}
}


LARGE_INTEGER
ReadLBA(HANDLE hDevice, ULONG LBA, ULONG Sectors) {
	IO_STATUS_BLOCK ioStatusBlock;
    NTSTATUS status;
	UCHAR *Buffer;
	ULONG BufferSize;
	LARGE_INTEGER StartTime, EndTime, ReturnValue, Offset;

	// Allocate Data Buffer
	BufferSize = Sectors*gDiskGeometry.BytesPerSector;
	Buffer = (UCHAR *)GlobalAlloc(GPTR, BufferSize);
	if(Buffer == NULL) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "GlobalAlloc(Buffer %u bytes) FAILED LastError = %d", BufferSize, GetLastError());
		ReturnValue.QuadPart = -1;
		return ReturnValue;
	}

	// Calculate Offset
	Offset.QuadPart = LBA*gDiskGeometry.BytesPerSector;

	// Start timer
	QueryPerformanceCounter(&StartTime);

	// Read bytes at offset
	status = NtReadFile(hDevice, NULL, NULL, NULL, &ioStatusBlock, Buffer, BufferSize, &Offset);

	// Stop timer
	QueryPerformanceCounter(&EndTime);

	if(!NT_SUCCESS(status)) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "NtReadFile(%u Bytes at Offset %#x) FAILED Status = %d", 
			BufferSize, Offset.QuadPart, status);
		ReturnValue.QuadPart = -1;
	} else {
		// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "NtReadFile = %u Bytes at Offset %#x, %f ms", Bytes, Offset.QuadPart, 1000.0*(EndTime.QuadPart - StartTime.QuadPart)/gFrequency.QuadPart);
		ReturnValue.QuadPart = EndTime.QuadPart - StartTime.QuadPart;
	}

	GlobalFree(Buffer);

	return ReturnValue;
}


DOUBLE
HDAccessTime(HANDLE hDevice, ULONG Start, ULONG End, ULONG Reps) {
	LARGE_INTEGER ReturnTime, TotalTime;
	DOUBLE ReturnVal;
	ULONG LBA;
	NTSTATUS status;
	IO_STATUS_BLOCK ioStatusBlock;

	// Initialize
	TotalTime.QuadPart = 0;
	ReturnVal = 1.0/(DOUBLE)Reps;

	// Flush Buffers
	status = NtFlushBuffersFile(hDevice, &ioStatusBlock);

	// Goto End
	ReturnTime = ReadLBA(hDevice, End, 1);
	if(ReturnTime.QuadPart == -1) {
		return 0;
	}

	LBA = Start;
	while(Reps > 0) {
		// Access the LBA
		ReturnTime = ReadLBA(hDevice, LBA, 1);
		if(ReturnTime.QuadPart == -1) {
			return 0;
		} else {
			TotalTime.QuadPart += ReturnTime.QuadPart;
		}

		// Swap LBA between Start and End
		if(LBA == Start) {
			LBA = End;
		} else {
			LBA = Start;
		}
		Reps--;
	}

	// return average access time in ms
	ReturnVal *= (DOUBLE)TotalTime.QuadPart;
	ReturnVal /= (DOUBLE)gFrequency.QuadPart;
	ReturnVal *= 1000.0;	// convert to ms
	return ReturnVal;
}


DOUBLE
StreamTime(HANDLE hDevice, ULONG Start, ULONG End, ULONG MaxSectors) {
	LARGE_INTEGER ReturnTime, TotalTime;
	DOUBLE ReturnVal;
	ULONG i, Reps;
	USHORT RemainSectors;
	NTSTATUS status;
	IO_STATUS_BLOCK ioStatusBlock;

	// Init Totaltime
	TotalTime.QuadPart = 0;

	// Compute Reps based on Sectors
	Reps = (End - Start)/MaxSectors;
	RemainSectors = (USHORT)((End - Start)%MaxSectors);

	// Flush Buffers
	status = NtFlushBuffersFile(hDevice, &ioStatusBlock);

	// Goto Start
	ReturnTime = ReadLBA(hDevice, Start, 1);
	if(ReturnTime.QuadPart == -1) {
		return 0;
	}

	for(i=0; i<Reps; i++) {
		ReturnTime = ReadLBA(hDevice, i*MaxSectors + Start, MaxSectors);
		if(ReturnTime.QuadPart == -1) {
			return 0;
		} else {
			TotalTime.QuadPart += ReturnTime.QuadPart;
			// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "ReturnTime = %d TotalTime = %u", ReturnTime.QuadPart, TotalTime.QuadPart);
		}
	}

	if(RemainSectors > 0) {
		ReturnTime = ReadLBA(hDevice, Reps*MaxSectors + Start, RemainSectors);
		if(ReturnTime.QuadPart == -1) {
			return 0;
		} else {
			TotalTime.QuadPart += ReturnTime.QuadPart;
			// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "ReturnTime = %d TotalTime = %u", ReturnTime.QuadPart, TotalTime.QuadPart);
		}
	}

	// return elapsed time in mbits/s
	ReturnVal = (DOUBLE)(End - Start);
	ReturnVal *= (DOUBLE)gDiskGeometry.BytesPerSector;
	ReturnVal *= (DOUBLE)gFrequency.QuadPart;
	ReturnVal /= (DOUBLE)TotalTime.QuadPart;
	ReturnVal /= (1024*1024);	// convert to MBytes
	return ReturnVal;
}


//
// SetPowerMode
//		Send AT command to drive
//
BOOL
SetPowerMode(HANDLE hDevice, UCHAR Command) {
	ATA_PASS_THROUGH	AtaPassThrough;
	DWORD cbBytesReturned;
	BOOL bReturn;

	memset(&AtaPassThrough, 0, sizeof(ATA_PASS_THROUGH));
	AtaPassThrough.IdeReg.bCommandReg = Command;
	AtaPassThrough.IdeReg.bDriveHeadReg = 0xa0;						// Device 0
	AtaPassThrough.DataBuffer = NULL;

	bReturn = DeviceIoControl(hDevice, IOCTL_IDE_PASS_THROUGH,
								&AtaPassThrough, sizeof(ATA_PASS_THROUGH),
								&AtaPassThrough, sizeof(ATA_PASS_THROUGH),
								&cbBytesReturned, NULL);

	if(!bReturn) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "SetPowerMode FAILED StatusReg=%#x ErrorReg=%#x LastError = %d", 
			AtaPassThrough.IdeReg.bCommandReg, AtaPassThrough.IdeReg.bFeaturesReg, GetLastError());
		LogAtaPassThrough(AtaPassThrough);
		return FALSE;
	} else {
		return TRUE;
	}
}


DOUBLE
StartUpTime(HANDLE hDevice, ULONG Reps) {
	LARGE_INTEGER ReturnTime, TotalTime;
	ULONG i;
	DOUBLE ReturnVal;

	TotalTime.QuadPart = 0;

	for(i = 0; i < Reps; i++) {
		// Put drive in Standby mode
		if(!SetPowerMode(hDevice, AT_STANDBY_IMMEDIATE)) {
			xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "SetPowerMode(AT_STANDBY_IMMEDIATE) FAILED");
			return 0;
		}
	
		// Wait for drive to spin down
		Sleep(6000);

		// Read from drive
		ReturnTime = ReadLBA(hDevice, 0, 1);
		if(ReturnTime.QuadPart == -1) {
			return 0;
		} else {
			TotalTime.QuadPart += ReturnTime.QuadPart;
		}
	}

	// Return average Startup time
	ReturnVal = (DOUBLE)TotalTime.QuadPart/(DOUBLE)gFrequency.QuadPart;
	ReturnVal /= (DOUBLE)Reps;
	return ReturnVal;
}


//
// Peak sequential streaming throughput - both write and read
//
VOID
FileReadWriteTest(HANDLE LogHandle) {
	HANDLE hFile;
	VOID* Buffer;
	DWORD BufferSize;
	DWORD Bytes, TotalBytes;
	LARGE_INTEGER BytesToMove, StartTime, EndTime, Frequency;
	DOUBLE Rate;

	// Initialize
	xSetFunctionName(LogHandle, "FileReadWriteTest");
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Begin");
	QueryPerformanceFrequency(&Frequency);

	// Allocate buffer
	Buffer = (PUCHAR)VirtualAlloc(NULL, MAXBUFSIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(Buffer == NULL) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "GlobalAlloc(%u bytes) FAILED LastError = %d", MAXBUFSIZE, GetLastError());
		return;
	}
	
	for(BufferSize = MINBUFSIZE; BufferSize <= MAXBUFSIZE; BufferSize *= 2) {
		// Creat file for write
		hFile = CreateFile(TESTFILENAME, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING, NULL);
		if(hFile == INVALID_HANDLE_VALUE) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "CreateFile() FAILED LastError = %d", GetLastError());
			return;
		}

		// Size file
		BytesToMove.QuadPart = TESTFILESIZE;
		if(!SetFilePointerEx(hFile, BytesToMove, NULL, FILE_BEGIN)) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SetFilePointerEx(%u bytes) FAILED LastError = %d", BytesToMove.QuadPart, GetLastError());
			return;
		}
		if(!SetEndOfFile(hFile)) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SetEndOfFile() FAILED LastError = %d", GetLastError());
			return;
		}

		// Go back to the beginning
		BytesToMove.QuadPart = 0;
		if(!SetFilePointerEx(hFile, BytesToMove, NULL, FILE_BEGIN)) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SetFilePointerEx(%u bytes) FAILED LastError = %d", BytesToMove.QuadPart, GetLastError());
			return;
		}

		// Write file
		TotalBytes = 0;
		QueryPerformanceCounter(&StartTime);
		do {
			if(!WriteFile(hFile, Buffer, BufferSize, &Bytes, NULL)) {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "WriteFile(%u bytes) FAILED LastError = %d", 
					BufferSize, GetLastError());
				return;
			}
			TotalBytes += Bytes;
		} while(TotalBytes < TESTFILESIZE);
		QueryPerformanceCounter(&EndTime);

		CloseHandle(hFile);

		Rate = (DOUBLE)TESTFILESIZE/(DOUBLE)(EndTime.QuadPart - StartTime.QuadPart);
		Rate *= (DOUBLE)Frequency.QuadPart;
		Rate /= (DOUBLE)KB;

		xLog(gbConsoleOut, LogHandle, XLL_PASS, "Sequential File Write (%3uKB Buffer) %f KB/s",
			BufferSize/KB, Rate);

		// Open file for read
		hFile = CreateFile(TESTFILENAME, GENERIC_READ, 0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_NO_BUFFERING, NULL);
		if(hFile == INVALID_HANDLE_VALUE) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "CreateFile() FAILED LastError = %d", GetLastError());
			return;
		}

		// Read file
		TotalBytes = 0;
		QueryPerformanceCounter(&StartTime);
		do {
			if(!ReadFile(hFile, Buffer, BufferSize, &Bytes, NULL)) {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "ReadFile(%u bytes) FAILED LastError = %d", 
					BufferSize, GetLastError());
				return;
			}
			TotalBytes += Bytes;
		} while(TotalBytes < TESTFILESIZE);
		QueryPerformanceCounter(&EndTime);

		Rate = (DOUBLE)TESTFILESIZE/(DOUBLE)(EndTime.QuadPart - StartTime.QuadPart);
		Rate *= (DOUBLE)Frequency.QuadPart;
		Rate /= (DOUBLE)KB;

		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Sequential File Read  (%3uKB Buffer) %f KB/s", BufferSize/KB, Rate);

		CloseHandle(hFile);

		DeleteFile(TESTFILENAME);
	}

	VirtualFree(Buffer, 0, MEM_RELEASE);
}


//
// Raw read streaming from various part of the hard disk
//
VOID 
RawReadTest(HANDLE LogHandle, DWORD BufferSize) {
	HANDLE hDevice;
	DISK_GEOMETRY DiskGeometry;
	VOID* Buffer;
	DWORD BytesReturned;
	LARGE_INTEGER DeviceOffset, SampleOffset, MaxOffset;
	LARGE_INTEGER StartTime, EndTime, Frequency;
	DOUBLE Rate;
	BOOL bReturn;

    OBJECT_STRING objname;
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK iosb;
    NTSTATUS status;


	// Initialize
	xSetFunctionName(LogHandle, "RawReadTest");
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Sequential Raw Read: %3uKB Buffer", BufferSize/KB);
	QueryPerformanceFrequency(&Frequency);

	// Allocate buffer
	Buffer = (PUCHAR)VirtualAlloc(NULL, BufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(Buffer == NULL) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "GlobalAlloc(%u bytes) FAILED LastError = %d", BufferSize, GetLastError());
		return;
	}

	// Open device
    RtlInitObjectString(&objname, OTEXT("\\Device\\Harddisk0\\partition0"));
    InitializeObjectAttributes(&oa, &objname, OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = NtOpenFile(&hDevice, GENERIC_READ|SYNCHRONIZE, &oa, &iosb, 0, FILE_SYNCHRONOUS_IO_ALERT);
    if(!NT_SUCCESS(status)) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "NtOpenFile() FAILED Status = %d", status);
		return;
	}

	// get geometry
	memset(&DiskGeometry, 0, sizeof(DiskGeometry));
	bReturn = DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY,
								NULL, 0, 
								&DiskGeometry, sizeof(DiskGeometry), 
								&BytesReturned, NULL);

	// Maximum Offset
	MaxOffset.QuadPart = DiskGeometry.BytesPerSector*(DiskGeometry.Cylinders.QuadPart*DiskGeometry.TracksPerCylinder*DiskGeometry.SectorsPerTrack - 1);

	for(DeviceOffset.QuadPart = 0; DeviceOffset.QuadPart <= MaxOffset.QuadPart - SAMPLEREADSIZE; DeviceOffset.QuadPart += SAMPLEBLOCKSIZE) {
		// Read SAMPLEREADSIZE at Offset
		SampleOffset.QuadPart = DeviceOffset.QuadPart;
		QueryPerformanceCounter(&StartTime);
        do {
			status = NtReadFile(hDevice, 0, NULL, NULL, &iosb, Buffer, BufferSize, &SampleOffset);
            if(!NT_SUCCESS(status)) {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "NtReadFile() FAILED Status = %d", status);
				return;
			}
			// Increment the SampleOffset
            SampleOffset.QuadPart += BufferSize;
        } while ((SampleOffset.QuadPart -  DeviceOffset.QuadPart) <= SAMPLEREADSIZE);
		QueryPerformanceCounter(&EndTime);

		// Compute read rate
		Rate = (DOUBLE)SAMPLEREADSIZE;
		Rate /= (DOUBLE)(EndTime.QuadPart - StartTime.QuadPart);
		Rate *= (DOUBLE)Frequency.QuadPart;
		Rate /= (DOUBLE)1024;
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Sequential Raw Read Block %#.9I64x %f KB/s", DeviceOffset.QuadPart, Rate);
    }

    NtClose(hDevice);
	VirtualFree(Buffer, 0, MEM_RELEASE);
}


//
// Random raw reads to measure seek time
//
VOID RandomRawDiskRead(HANDLE LogHandle, DWORD BufferSize) {
    OBJECT_STRING objname;
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK iosb;
    NTSTATUS status;
    HANDLE hDevice;
	VOID* Buffer;
	DISK_GEOMETRY DiskGeometry;
	DWORD BytesReturned;
	LARGE_INTEGER StartTime, EndTime, Frequency;
	LARGE_INTEGER Offset, MaxOffset;
	DWORD MaxBuffers, TotalBytesRead;
	DOUBLE Rate;
	BOOL bReturn;

	// Initialize
	xSetFunctionName(LogHandle, "RandomRawDiskRead");
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Random Raw Read: %3uKB Buffer", BufferSize/KB);
	QueryPerformanceFrequency(&Frequency);
	srand('RAWR');

	// Allocate buffer
	Buffer = (PUCHAR)VirtualAlloc(NULL, BufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(Buffer == NULL) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "GlobalAlloc(%u bytes) FAILED LastError = %d", BufferSize, GetLastError());
		return;
	}

	// Open device
    RtlInitObjectString(&objname, OTEXT("\\Device\\Harddisk0\\partition0"));
    InitializeObjectAttributes(&oa, &objname, OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = NtOpenFile(&hDevice, GENERIC_READ|SYNCHRONIZE, &oa, &iosb, 0, FILE_SYNCHRONOUS_IO_ALERT);
    if(!NT_SUCCESS(status)) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "NtOpenFile() FAILED Status = %d", status);
		return;
	}

	// get geometry
	memset(&DiskGeometry, 0, sizeof(DiskGeometry));
	bReturn = DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY,
								NULL, 0, 
								&DiskGeometry, sizeof(DiskGeometry), 
								&BytesReturned, NULL);
	// Maximum Offset
	MaxOffset.QuadPart = DiskGeometry.BytesPerSector*(DiskGeometry.Cylinders.QuadPart*DiskGeometry.TracksPerCylinder*DiskGeometry.SectorsPerTrack - 1);

	//Maximum number of buffer sized chunks
    MaxBuffers = (DWORD)MaxOffset.QuadPart/BufferSize;
    TotalBytesRead = 0;

	// Read TESTFILESIZE bytes at random locations
	QueryPerformanceCounter(&StartTime);
    do {
        Offset.QuadPart = rand()*MaxBuffers/RAND_MAX;
        Offset.QuadPart *= BufferSize;
        status = NtReadFile(hDevice, 0, NULL, NULL, &iosb, Buffer, BufferSize, &Offset);
        TotalBytesRead += BufferSize;
    } while (TotalBytesRead < TESTFILESIZE);
	QueryPerformanceCounter(&EndTime);

	// Compute read rate
	Rate = (DOUBLE)TESTFILESIZE/(DOUBLE)(EndTime.QuadPart - StartTime.QuadPart);
	Rate *= (DOUBLE)Frequency.QuadPart;
	Rate /= (DOUBLE)KB;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Random Raw Read: %f KB/s", Rate);

    NtClose(hDevice);
	VirtualFree(Buffer, 0, MEM_RELEASE);
}


//
// Small file creation test to measure caching effects
//
VOID SmallFileCreationTest(HANDLE LogHandle) {
	HANDLE hFile;
    CHAR FileName[MAX_PATH];
	VOID* Buffer;
	LARGE_INTEGER StartTime, EndTime, Frequency;
	DWORD FileSize, Bytes;
	DOUBLE Time;
	int i;

	// Initialize
	xSetFunctionName(LogHandle, "SmallFileCreationTest");
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Small File Creation Test: %u files", FILECOUNT);
	QueryPerformanceFrequency(&Frequency);
	srand('DIRS');

	// Allocate buffer
	Buffer = (PUCHAR)VirtualAlloc(NULL, MAXSMALLFILESIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(Buffer == NULL) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "GlobalAlloc(%u bytes) FAILED LastError = %d", MAXSMALLFILESIZE, GetLastError());
		return;
	}

	// Create test directory
    CreateDirectory(TESTDIRNAME, NULL);
		
	// Create files
	QueryPerformanceCounter(&StartTime);
    for (i = 0; i < FILECOUNT; i++) {
		sprintf(FileName, "%s\\%04x", TESTDIRNAME, i);
        hFile = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_WRITE_THROUGH,
                        NULL);
		if(hFile == INVALID_HANDLE_VALUE) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "CreateFile(%s) FAILED LastError = %d", FileName, GetLastError());
			return;
		}

		FileSize= rand() % MAXSMALLFILESIZE + 1;
        if(!WriteFile(hFile, Buffer, FileSize, &Bytes, NULL)) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "WriteFile(%s, %u bytes) FAILED LastError = %d", 
					FileName, FileSize, GetLastError());
			return;
		}

		CloseHandle(hFile);
    }
	QueryPerformanceCounter(&EndTime);

	Time = (DOUBLE)(EndTime.QuadPart - StartTime.QuadPart)/(DOUBLE)Frequency.QuadPart*1000.0;
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Create %u Small Files: %f ms", FILECOUNT, Time);

    // Delete files
	QueryPerformanceCounter(&StartTime);
    for (i = 0; i < FILECOUNT; i++) {
		sprintf(FileName, "%s\\%04x", TESTDIRNAME, i);
        DeleteFile(FileName);
    }
	QueryPerformanceCounter(&EndTime);

	Time = (DOUBLE)(EndTime.QuadPart - StartTime.QuadPart)/(DOUBLE)Frequency.QuadPart*1000.0;
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Delete %u Small Files: %f ms", FILECOUNT, Time);

    RemoveDirectory(TESTDIRNAME);

	VirtualFree(Buffer, 0, MEM_RELEASE);
}


//
// Simple disk cache test
//
VOID DiskCacheTest(HANDLE LogHandle) {
	HANDLE hFile;
	VOID* Buffer;
	LARGE_INTEGER ReadStartTime, ReadEndTime, WriteStartTime, WriteEndTime, Frequency;
	DWORD BufferSize, Bytes, TotalBytes, Offset;
	DOUBLE ReadTime[READBUFSIZE/WRITEBUFSIZE], WriteTime[READBUFSIZE/WRITEBUFSIZE];
	DOUBLE ReadTimeSeq[6][4], ReadTimeScat[6][4];
	int i, j;

	// Initialize
	xSetFunctionName(LogHandle, "DiskCacheTest");
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Begin");
	QueryPerformanceFrequency(&Frequency);

	// Allocate buffer
	Buffer = (PUCHAR)VirtualAlloc(NULL, MAXBUFSIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if(Buffer == NULL) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "GlobalAlloc(%u bytes) FAILED LastError = %d", MAXBUFSIZE, GetLastError());
		return;
	}

	// Open file
    hFile = CreateFile("z:", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,
                    NULL, OPEN_EXISTING, 
					FILE_ATTRIBUTE_NORMAL| FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_NO_BUFFERING, NULL);
	
	if(hFile == INVALID_HANDLE_VALUE) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "CreateFile() FAILED LastError = %d", GetLastError());
		return;
	}

    //
    // Interleaved reads and writes
    //
	j = 0;
    for(Offset=0; Offset < READBUFSIZE; Offset += WRITEBUFSIZE) {
		// Goto STARTINGPOS
        if(SetFilePointer(hFile, STARTINGPOS, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SetFilePointer() FAILED LastError = %d", GetLastError());
			return;
		}

		TotalBytes = 0;

		// Read CACHEFILESIZE bytes
		QueryPerformanceCounter(&ReadStartTime);
        do {
            if(!ReadFile(hFile, Buffer, READBUFSIZE, &Bytes, NULL)) {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "ReadFile() FAILED LastError = %d", GetLastError());
				return;
			}
            TotalBytes += Bytes;
        } while (TotalBytes < CACHEFILESIZE);
		QueryPerformanceCounter(&ReadEndTime);

		// Goto write spot
        if(SetFilePointer(hFile, STARTINGPOS + CACHEFILESIZE - READBUFSIZE + Offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SetFilePointer() FAILED LastError = %d", GetLastError());
			return;
		}

		// Write WRITEBUFSIZE bytes
		QueryPerformanceCounter(&WriteStartTime);
		if(!WriteFile(hFile, (CHAR*) Buffer + Offset, WRITEBUFSIZE, &Bytes, NULL)) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "WriteFile() FAILED LastError = %d", GetLastError());
			return;
		}
		QueryPerformanceCounter(&WriteEndTime);

		// Store results
		ReadTime[j] = (DOUBLE)(ReadEndTime.QuadPart - ReadStartTime.QuadPart)/(DOUBLE)Frequency.QuadPart*1000;
		WriteTime[j] = (DOUBLE)(WriteEndTime.QuadPart - WriteStartTime.QuadPart)/(DOUBLE)Frequency.QuadPart*1000;
		j++;
    }

    //
    // Reads same 256KB of data using different buffer sizes
    //
	j = 0;
    for(BufferSize = MINBUFSIZE; BufferSize <= MAXBUFSIZE; BufferSize *= 2) {
		for(i = 0; i < 4; i++) {
			// Goto STARTINGPOS
			if(SetFilePointer(hFile, STARTINGPOS, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SetFilePointer() FAILED LastError = %d", GetLastError());
				return;
			}
            
            TotalBytes = 0;
			QueryPerformanceCounter(&ReadStartTime);
			do {
	            if(!ReadFile(hFile, Buffer, BufferSize, &Bytes, NULL)) {
					xLog(gbConsoleOut, LogHandle, XLL_FAIL, "ReadFile() FAILED LastError = %d", GetLastError());
					return;
				}
	            TotalBytes += Bytes;
		    } while (TotalBytes < CACHEFILESIZE);
			QueryPerformanceCounter(&ReadEndTime);

			// Store results
			ReadTimeSeq[j][i] = (DOUBLE)(ReadEndTime.QuadPart - ReadStartTime.QuadPart)/(DOUBLE)Frequency.QuadPart*1000.0;
        }
		j++;
    }

    //
    // Read 256KB scattered
    //
	j = 0;
	for(BufferSize = MINBUFSIZE; BufferSize <= MAXBUFSIZE; BufferSize *= 2) {
		for(i = 0; i < 4; i++) {
			Offset = 0;
			TotalBytes = 0;
			QueryPerformanceCounter(&ReadStartTime);
			do {
				SetFilePointer(hFile, STARTINGPOS + Offset, NULL, FILE_BEGIN);
				ReadFile(hFile, Buffer, BufferSize, &Bytes, NULL);
				//
				if(SetFilePointer(hFile, STARTINGPOS + Offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
					xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SetFilePointer() FAILED LastError = %d", GetLastError());
					return;
				}

	            if(!ReadFile(hFile, Buffer, BufferSize, &Bytes, NULL)) {
					xLog(gbConsoleOut, LogHandle, XLL_FAIL, "ReadFile() FAILED LastError = %d", GetLastError());
					return;
				}
				//
	            TotalBytes += Bytes;
				Offset += MB;
	        } while (TotalBytes < CACHEFILESIZE);
			QueryPerformanceCounter(&ReadEndTime);

			// Store results
			ReadTimeScat[j][i] = (DOUBLE)(ReadEndTime.QuadPart - ReadStartTime.QuadPart)/(DOUBLE)Frequency.QuadPart*1000.0;
        }
		j++;
    }

    CloseHandle(hFile);
		
	VirtualFree(Buffer, 0, MEM_RELEASE);

	// Log Interleaved results
	j = 0;
    for(Offset=0; Offset < READBUFSIZE; Offset += WRITEBUFSIZE) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Read %uKB %fms Write (Offset %u bytes): %fms", CACHEFILESIZE/KB, ReadTime[j], Offset, WriteTime[j]); 
		j++;
	}

	// Log Sequential results
	j = 0;
    for(BufferSize = MINBUFSIZE; BufferSize <= MAXBUFSIZE; BufferSize *= 2) {
		for(i = 0; i < 4; i++) {
			xLog(gbConsoleOut, LogHandle, XLL_INFO, "Read %u: %uKB (%3dKB Buffer): %fms", i, CACHEFILESIZE/KB, BufferSize/KB, ReadTimeSeq[j][i]); 
		}
		j++;
	}

	// Log Scattered results
	j = 0;
    for(BufferSize = MINBUFSIZE; BufferSize <= MAXBUFSIZE; BufferSize *= 2) {
		for(i = 0; i < 4; i++) {
			xLog(gbConsoleOut, LogHandle, XLL_INFO, "Read %u Scattered: %uKB (%3dKB Buffer): %fms", i, CACHEFILESIZE/KB, BufferSize/KB, ReadTimeScat[j][i]); 
		}
		j++;
	}
}


VOID
WINAPI
HDPerf_llStartTest(HANDLE LogHandle) {
	HANDLE hDevice;
	BOOL bReturn;
    DWORD cbBytesReturned;
    OBJECT_STRING objectName;
    OBJECT_ATTRIBUTES objectAttributes;
    IO_STATUS_BLOCK ioStatusBlock;
    NTSTATUS status;
	ATA_PASS_THROUGH	AtaPassThrough, AtaPassThroughOut;
	IDE_IDENTIFY_DATA	IdeIdData;
	DOUBLE dTime;
	ULONG StartUpReps;
	ULONG TrackToTrackLBA, TrackToTrackReps;
	ULONG FullStartLBA, FullEndLBA, FullReps;
	ULONG StreamStartLBA, StreamEndLBA;
	ULONG MaxLBA, NextTrack;
	ULONG MaxSectors = 0, i;

	// Set global log handle
	gLogHandle = LogHandle;

	xSetOwnerAlias(gLogHandle, "a-emebac");
	xSetComponent(gLogHandle, "Hardware", "HDPerf");
	xSetFunctionName(gLogHandle, "Initialization");

	// Get timer frequency (ticks/sec)
	QueryPerformanceFrequency(&gFrequency);


	// open device
	RtlInitObjectString(&objectName, "\\Device\\Harddisk0\\Partition0");
	InitializeObjectAttributes(&objectAttributes, &objectName, OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = NtOpenFile(&hDevice, 
							GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE,
							&objectAttributes,
							&ioStatusBlock,
							0,
							FILE_SYNCHRONOUS_IO_ALERT | FILE_NO_INTERMEDIATE_BUFFERING);

	if(!NT_SUCCESS(status)) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "NtOpenFile FAILED Status = %d", status);
		return;
	}

	// get geometry
	memset(&gDiskGeometry, 0, sizeof(gDiskGeometry));
	bReturn = DeviceIoControl(hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY,
								NULL, 0, 
								&gDiskGeometry, sizeof(gDiskGeometry), 
								&cbBytesReturned, NULL);

	if(!bReturn) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "IOCTL_DISK_GET_DRIVE_GEOMETRY FAILED LastError = %d", GetLastError());
		CloseHandle(hDevice);
		return;
	}

	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Cylinders = %u", gDiskGeometry.Cylinders.QuadPart);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "TracksPerCylinder = %u", gDiskGeometry.TracksPerCylinder);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SectorsPerTrack = %u", gDiskGeometry.SectorsPerTrack);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "BytesPerSector = %u", gDiskGeometry.BytesPerSector);

	// Populate pass through command for Identify Device
	memset(&IdeIdData, 0, sizeof(IDE_IDENTIFY_DATA));
	memset(&AtaPassThrough, 0, sizeof(ATA_PASS_THROUGH));
	AtaPassThrough.IdeReg.bCommandReg = 0xec;						// Identify Device DMA
	AtaPassThrough.IdeReg.bDriveHeadReg = 0xa0;						// Device 0
	AtaPassThrough.DataBufferSize = sizeof(IDE_IDENTIFY_DATA);
	AtaPassThrough.DataBuffer = &IdeIdData;

	bReturn = DeviceIoControl(hDevice, IOCTL_IDE_PASS_THROUGH,
								&AtaPassThrough, sizeof(ATA_PASS_THROUGH),
								&AtaPassThrough, sizeof(ATA_PASS_THROUGH),
								&cbBytesReturned, NULL);
	if(!bReturn) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "IDENTIFY DEVICE FAILED StatusReg=%#x ErrorReg=%#x LastError = %d", 
			AtaPassThrough.IdeReg.bCommandReg, AtaPassThrough.IdeReg.bFeaturesReg, GetLastError());
		LogAtaPassThrough(AtaPassThrough);
		CloseHandle(hDevice);
		return;
	}

	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Cylinders = %u",						IdeIdData.NumberOfCylinders);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Heads = %u",							IdeIdData.NumberOfHeads);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SectorsPerTrack = %u",					IdeIdData.NumberOfSectorsPerTrack);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Current Cylinders = %u",				IdeIdData.NumberOfCurrentCylinders);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Current Heads = %u",					IdeIdData.NumberOfCurrentHeads);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Current SectorsPerTrack = %u",			IdeIdData.CurrentSectorsPerTrack);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Current Sector Capacity = %u",			IdeIdData.CurrentSectorCapacity);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Current Multi Sector Setting = %u",	IdeIdData.CurrentMultiSectorSetting);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "User Addressable Sectors = %u",		IdeIdData.UserAddressableSectors);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "MultiWordDMASupport = %u",				IdeIdData.MultiWordDMASupport);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "MultiWordDMAActive = %u",				IdeIdData.MultiWordDMAActive);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "UltraDMASupport = %u",					IdeIdData.UltraDMASupport);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "UltraDMAActive = %u",					IdeIdData.UltraDMAActive);

	// Populate pass through command for Disable read look-ahead
	memset(&AtaPassThrough, 0, sizeof(ATA_PASS_THROUGH));
	AtaPassThrough.IdeReg.bCommandReg = 0xef;						// Set features
	AtaPassThrough.IdeReg.bFeaturesReg = 0x55;						// Disable read look-ahead
	AtaPassThrough.IdeReg.bSectorCountReg = 0;
	AtaPassThrough.IdeReg.bDriveHeadReg = 0xa0;						// Device 0
	AtaPassThrough.DataBufferSize = 0;
	AtaPassThrough.DataBuffer = NULL;

	memset(&AtaPassThroughOut, 0, sizeof(ATA_PASS_THROUGH));

	bReturn = DeviceIoControl(hDevice, IOCTL_IDE_PASS_THROUGH,
								&AtaPassThrough, sizeof(ATA_PASS_THROUGH),
								&AtaPassThrough, sizeof(ATA_PASS_THROUGH),
								&cbBytesReturned, NULL);
	if(!bReturn) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "SET FEATURES: Disable read look-ahead FAILED StatusReg=%#x ErrorReg=%#x LastError = %d", 
			AtaPassThrough.IdeReg.bCommandReg, AtaPassThrough.IdeReg.bFeaturesReg, GetLastError());
		LogAtaPassThrough(AtaPassThrough);
		return;
	}

	// Get parameters from ini file
	MaxLBA = IdeIdData.UserAddressableSectors - 1;
	StartUpReps = GetProfileInt(TEXT("hdperf"), TEXT("StartUpReaps"), 10);
	TrackToTrackLBA = GetProfileInt(TEXT("hdperf"), TEXT("TrackToTrackLBA"), MaxLBA/2);
	TrackToTrackReps = GetProfileInt(TEXT("hdperf"), TEXT("TrackToTrackReps"), 200);
	FullStartLBA = GetProfileInt(TEXT("hdperf"), TEXT("FullStartLBA"), 0);
	FullEndLBA = GetProfileInt(TEXT("hdperf"), TEXT("FullEndLBA"), MaxLBA);
	FullReps = GetProfileInt(TEXT("hdperf"), TEXT("FullReps"), 200);
	StreamStartLBA = GetProfileInt(TEXT("hdperf"), TEXT("StreamStartLBA"), 0);
	StreamEndLBA = GetProfileInt(TEXT("hdperf"), TEXT("StreamEndLBA"), 1048576/gDiskGeometry.BytesPerSector);	// 1MB

	// Startup time
	xSetFunctionName(gLogHandle, "StartUpTime");
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Average Startup Time: Reps = %u", StartUpReps);
	dTime = StartUpTime(hDevice, StartUpReps);
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Average Startup Time = %f s", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Average Startup Time FAILED");
	}

	// Measure Track to Track access times
	xSetFunctionName(gLogHandle, "AccessTime");
	NextTrack = gDiskGeometry.SectorsPerTrack + 1;
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Track to Track: Start LBA = %u End LBA = %u Reps = %u", 0, NextTrack, TrackToTrackReps);
	dTime = HDAccessTime(hDevice, 0, NextTrack, TrackToTrackReps);
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Track to Track Access Time = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Track to Track Access Time FAILED");
	}

	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Track to Track: Start LBA = %u, End LBA = %u, Reps = %u", MaxLBA/2, MaxLBA/2 + NextTrack, TrackToTrackReps);
	dTime = HDAccessTime(hDevice, MaxLBA/2, MaxLBA/2 + NextTrack, TrackToTrackReps);
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Track to Track Access Time = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Track to Track Access Time FAILED");
	}

	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Track to Track: Start LBA = %u End LBA = %u Reps = %u", MaxLBA - NextTrack, MaxLBA, TrackToTrackReps);
	dTime = HDAccessTime(hDevice, MaxLBA - NextTrack, MaxLBA, TrackToTrackReps);
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Track to Track Access Time = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Track to Track Access Time FAILED");
	}

	// Measure full-stroke access time
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Full-Stroke Start LBA = %u, End LBA = %u, Reps = %u", FullStartLBA, FullEndLBA, FullReps);
	dTime = HDAccessTime(hDevice, FullStartLBA, FullEndLBA, FullReps);
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Average Full-Stroke Access Time = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Average Full-Stroke Access Time FAILED");
	}

	// Measure data stream rate
	xSetFunctionName(gLogHandle, "StreamTime");
	for(i = 0; i <= 8; i++) {
		MaxSectors = 1<<i;
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Stream Rate Start LBA = %u, End LBA = %u, Read Buffer = %u bytes", StreamStartLBA, StreamEndLBA, MaxSectors*gDiskGeometry.BytesPerSector);
		dTime = StreamTime(hDevice, StreamStartLBA, StreamEndLBA, MaxSectors);
		if(dTime > 0) {
			xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Average Sustained Data Rate = %f MBytes/s", dTime);
		} else {
			xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Average Sustained Data Rate FAILED");
		}
	}

	// close device
	if(!CloseHandle(hDevice)) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "CloseHandle FAILED LastError = %d", GetLastError());
	}

	// File tests
	if(GetProfileIntA("hdperf", "FileReadWriteTest", 1)) {
		FileReadWriteTest(LogHandle);
	}
    
	if(GetProfileIntA("hdperf", "RawReadTest", 1)) {
		RawReadTest(LogHandle, 64*KB);
	}

	if(GetProfileIntA("hdperf", "SmallFileCreationTest", 1)) {
		SmallFileCreationTest(LogHandle);
	}

	if(GetProfileIntA("hdperf", "RandomRawDiskRead", 1)) {
		RandomRawDiskRead(LogHandle, 64*KB);
	}

	if(GetProfileIntA("hdperf", "DiskCacheTest", 1)) {
		DiskCacheTest(LogHandle);
	}
}


VOID WINAPI
HDPerf_llEndTest() {
}


#if !defined(HARNESSLIB)
void __cdecl main() {
	// Do the initialization that the harness does
	// dxconio
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);

	// xSetFontA(12.0, 18.0, NULL);
	
	// xlog
	gLogHandle = xCreateLog_A("t:\\hdperf.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(gLogHandle == INVALID_HANDLE_VALUE ) {
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "HDPerf: couldn't create log (%s)", WinErrorSymbolicName(GetLastError()));
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "HDPerf: End - Waiting for reboot...");
		Sleep(INFINITE);
    }

	gbConsoleOut = TRUE;


	// Start Test
	HDPerf_llStartTest(gLogHandle);

	// End Test
	HDPerf_llEndTest();

	// xlog
	xCloseLog(gLogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "hdperf: End - Waiting for reboot...");
	Sleep(INFINITE);

	// Do the de-initialize that the harness does
	// dxconio
	xReleaseConsole();

}
#endif

//
// Export function pointers of StartTest and EndTest
//
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( hdperf_ll )
#pragma data_seg()

BEGIN_EXPORT_TABLE( hdperf_ll )
    EXPORT_TABLE_ENTRY( "StartTest", HDPerf_llStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", HDPerf_llEndTest )
END_EXPORT_TABLE( hdperf_ll )


