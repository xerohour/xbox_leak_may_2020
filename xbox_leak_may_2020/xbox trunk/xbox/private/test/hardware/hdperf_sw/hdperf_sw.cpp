//
//	hdperf_sw.cpp
//
//		Hard disk performance test using higher level functions to represent
//		game like usage.
//		Based on David Xu's test.
//		
#include <ntos.h>
#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ntdddisk.h>
#include <ntddscsi.h>
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
HDPerfSWStartTest(HANDLE LogHandle) {

	xSetOwnerAlias(LogHandle, "a-emebac");
	xSetComponent(LogHandle, "Hardware", "HDPerf_sw");

	if(GetProfileIntA("hdperf_sw", "FileReadWriteTest", 1)) {
		FileReadWriteTest(LogHandle);
	}
    
	if(GetProfileIntA("hdperf_sw", "RawReadTest", 1)) {
		RawReadTest(LogHandle, 64*KB);
	}

	if(GetProfileIntA("hdperf_sw", "SmallFileCreationTest", 1)) {
		SmallFileCreationTest(LogHandle);
	}

	if(GetProfileIntA("hdperf_sw", "RandomRawDiskRead", 1)) {
		RandomRawDiskRead(LogHandle, 64*KB);
	}

	if(GetProfileIntA("hdperf_sw", "DiskCacheTest", 1)) {
		DiskCacheTest(LogHandle);
	}
}

VOID 
HDPerfSWEndTest() {
}


#if !defined(HARNESSLIB)
void __cdecl main() {
	HANDLE LogHandle;


	// Do the initialization that the harness does
	// dxconio
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);

	// xSetFontA(12.0, 18.0, NULL);
	
	// xlog
	LogHandle = xCreateLog_A("t:\\hdperf_sw.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("HDPerf_sw: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
		xDebugStringA("HDPerf_sw: End - Waiting for reboot...\n");
		Sleep(INFINITE);
    }

	gbConsoleOut = TRUE;

	// Start Test
	HDPerfSWStartTest(LogHandle);

	// End Test
	HDPerfSWEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("HDPerf_sw: End - Waiting for reboot...\n");
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
DECLARE_EXPORT_DIRECTORY( hdperf_sw )
#pragma data_seg()

BEGIN_EXPORT_TABLE( hdperf_sw )
    EXPORT_TABLE_ENTRY( "StartTest", HDPerfSWStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", HDPerfSWEndTest )
END_EXPORT_TABLE( hdperf_sw )


