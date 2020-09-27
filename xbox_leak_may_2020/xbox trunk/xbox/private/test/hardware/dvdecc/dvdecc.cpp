/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

   DVD ECC - dvdecc.c

Abstract:

   Tally data from DVD ECC Mode Sense page for every DVD cache size read

Author:

   Emeron Bachhuber

Notes:


--*/


//#include <idex.h>
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

#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <dxconio.h>
#include <xlogconio.h>
#include <devioctl.h>
#include <scsi.h>
#include <ntddscsi.h>

// Macros to swap the byte order of a USHORT or ULONG at compile time.
#define IdexConstantUshortByteSwap(ushort) \
    ((((USHORT)ushort) >> 8) + ((((USHORT)ushort) & 0x00FF) << 8))

#define IdexConstantUlongByteSwap(ulong) \
    ((((ULONG)ulong) >> 24) + ((((ULONG)ulong) & 0x00FF0000) >> 8) + \
    ((((ULONG)ulong) & 0x0000FF00) << 8) + ((((ULONG)ulong) & 0x000000FF) << 24))


#define BytesPerSector 2048				// 2KB sectors
#define MaxSectors 64					// Maximum sectors for SCSIOP_READ
#define CacheLBAs 192					// Thomson says to read 192 LBAs per ECC read
#define BufSize  CacheLBAs*BytesPerSector


struct ECCData {
	LARGE_INTEGER Bytes;
	DWORD BytesCorrected;
	DWORD BlocksCorrected;
	DWORD BlocksUncorrectable;
	DWORD BlocksProcessed;
};


// Globals
ECCData FileECC, TotalECC;
BOOL LogPerRead;
HANDLE hDevice;
UCHAR *Buf;


VOID
DVDECCLogSenseData(HANDLE LogHandle, SENSE_DATA SenseData) {
	// Log SenseData
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData ErrorCode:  0x%.2x", SenseData.ErrorCode);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData Valid:  0x%.2x", SenseData.Valid);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData SegmentNumber:  0x%.2x", SenseData.SegmentNumber);	
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData SenseKey:  0x%.2x", SenseData.SenseKey);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData Reserved:  0x%.2x", SenseData.Reserved);	
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData IncorrectLength:  0x%.2x", SenseData.IncorrectLength);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData EndOfMedia:  0x%.2x", SenseData.EndOfMedia);	
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData FileMark:  0x%.2x", SenseData.FileMark);	
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData Information[0]:  0x%.2x", SenseData.Information[0]);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData Information[1]:  0x%.2x", SenseData.Information[1]);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData Information[2]:  0x%.2x", SenseData.Information[2]);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData Information[3]:  0x%.2x", SenseData.Information[3]);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData AdditionalSenseLength:  0x%.2x", SenseData.AdditionalSenseLength);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData CommandSpecificInformation[0]:  0x%.2x", SenseData.CommandSpecificInformation[0]);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData CommandSpecificInformation[1]:  0x%.2x", SenseData.CommandSpecificInformation[1]);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData CommandSpecificInformation[2]:  0x%.2x", SenseData.CommandSpecificInformation[2]);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData CommandSpecificInformation[3]:  0x%.2x", SenseData.CommandSpecificInformation[3]);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData AdditionalSenseCode:  0x%.2x", SenseData.AdditionalSenseCode);	
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData AdditionalSenseCodeQualifier:  0x%.2x", SenseData.AdditionalSenseCodeQualifier);	
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData FieldReplaceableUnitCode:  0x%.2x", SenseData.FieldReplaceableUnitCode);	
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData SenseKeySpecific[0]:  0x%.2x", SenseData.SenseKeySpecific[0]);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData SenseKeySpecific[1]:  0x%.2x", SenseData.SenseKeySpecific[1]);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData SenseKeySpecific[2]:  0x%.2x", SenseData.SenseKeySpecific[2]);	
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "SenseData SenseKeySpecific[3]:  0x%.2x", SenseData.SenseKeySpecific[3]);	
}


VOID
GetECCPage(HANDLE LogHandle, DWORD Bytes) {
	UCHAR ECCPage[24];
	SENSE_DATA SenseData;
	SCSI_PASS_THROUGH_DIRECT PassThrough;
	PCDB Cdb = (PCDB)&PassThrough.Cdb;
	BOOL bReturn;
	DWORD cbBytesReturned;
	DWORD BytesCorrected, BlocksCorrected, BlocksUncorrectable, BlocksProcessed;

	ZeroMemory(ECCPage, sizeof(ECCPage));
	ZeroMemory(&SenseData, sizeof(SENSE_DATA));
	ZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));

	PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
	PassThrough.DataBuffer = ECCPage;
	PassThrough.DataTransferLength = sizeof(ECCPage);
	Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SENSE10;
	Cdb->MODE_SENSE10.PageCode = 0x24;
	*((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) =
		(USHORT)IdexConstantUshortByteSwap(sizeof(ECCPage));

	bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
							&PassThrough, sizeof(PassThrough), 
							&SenseData, sizeof(SenseData),
							&cbBytesReturned, NULL);

	if((!bReturn) || (cbBytesReturned > 0)) {
		if(!bReturn) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SCSIOP_MODE_SENSE10 FAILED LastError = %d", GetLastError());
		}

		if(cbBytesReturned > 0) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SCSIOP_MODE_SENSE10 FAILED cbBytesReturned > 0");
			DVDECCLogSenseData(LogHandle, SenseData);
		}
	}

	// Increment Counters
	BytesCorrected = (ECCPage[8]<<24) + (ECCPage[9]<<16) + (ECCPage[10]<<8) + ECCPage[11];
	BlocksCorrected = (ECCPage[12]<<24) + (ECCPage[13]<<16) + (ECCPage[14]<<8) + ECCPage[15];
	BlocksUncorrectable = (ECCPage[16]<<24) + (ECCPage[17]<<16) + (ECCPage[18]<<8) + ECCPage[19];
	BlocksProcessed = (ECCPage[20]<<24) + (ECCPage[21]<<16) + (ECCPage[22]<<8) + ECCPage[23];
	
	FileECC.Bytes.QuadPart += Bytes;
	FileECC.BytesCorrected += BytesCorrected;
	FileECC.BlocksCorrected += BlocksCorrected;
	FileECC.BlocksUncorrectable += BlocksUncorrectable;
	FileECC.BlocksProcessed += BlocksProcessed;
	
	
	TotalECC.BytesCorrected += BytesCorrected;
	TotalECC.BlocksCorrected += BlocksCorrected;
	TotalECC.BlocksUncorrectable += BlocksUncorrectable;
	TotalECC.BlocksProcessed += BlocksProcessed;
	TotalECC.Bytes.QuadPart += Bytes;
		
	// Log
	if(LogPerRead) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO,	"Bytes Read: %#x (%u)", Bytes, Bytes);
		xLog(gbConsoleOut, LogHandle, XLL_INFO,	"ECC Bytes Corrected: %#.8x (%u)", BytesCorrected, BytesCorrected);
		xLog(gbConsoleOut, LogHandle, XLL_INFO,	"ECC Blocks Corrected: %#.8x (%u)", BlocksCorrected, BlocksCorrected);
		xLog(gbConsoleOut, LogHandle, XLL_INFO,	"ECC Blocks Uncorrectable: %#.8x (%u)", BlocksUncorrectable, BlocksUncorrectable);
		xLog(gbConsoleOut, LogHandle, XLL_INFO,	"ECC Blocks Processed: %#.8x (%u)", BlocksProcessed, BlocksProcessed);
	}
}


VOID 
LogFileECC(HANDLE LogHandle, CHAR *strFile) {
	xLog(gbConsoleOut, LogHandle, XLL_INFO,	"%s Bytes Read: %#I64x (%I64u)", strFile, FileECC.Bytes.QuadPart, FileECC.Bytes.QuadPart);
	xLog(gbConsoleOut, LogHandle, XLL_INFO,	"%s ECC Bytes Corrected: %#.8x (%u)", strFile, FileECC.BytesCorrected, FileECC.BytesCorrected);
	xLog(gbConsoleOut, LogHandle, XLL_INFO,	"%s ECC Blocks Corrected: %#.8x (%u)", strFile, FileECC.BlocksCorrected, FileECC.BlocksCorrected);
	xLog(gbConsoleOut, LogHandle, XLL_INFO,	"%s ECC Blocks Uncorrectable: %#.8x (%u)", strFile, FileECC.BlocksUncorrectable, FileECC.BlocksUncorrectable);
	xLog(gbConsoleOut, LogHandle, XLL_INFO,	"%s ECC Blocks Processed: %#.8x (%u)", strFile, FileECC.BlocksProcessed, FileECC.BlocksProcessed);
}


VOID 
LogTotalECC(HANDLE LogHandle) {
	xLog(gbConsoleOut, LogHandle, XLL_INFO,	"Total Bytes Read: %#I64x (%I64u)", TotalECC.Bytes.QuadPart, TotalECC.Bytes.QuadPart);
	xLog(gbConsoleOut, LogHandle, XLL_INFO,	"Total ECC Bytes Corrected: %#.8x (%u)", TotalECC.BytesCorrected, TotalECC.BytesCorrected);
	xLog(gbConsoleOut, LogHandle, XLL_INFO,	"Total ECC Blocks Corrected: %#.8x (%u)", TotalECC.BlocksCorrected, TotalECC.BlocksCorrected);
	xLog(gbConsoleOut, LogHandle, XLL_INFO,	"Total ECC Blocks Uncorrectable: %#.8x (%u)", TotalECC.BlocksUncorrectable, TotalECC.BlocksUncorrectable);
	xLog(gbConsoleOut, LogHandle, XLL_INFO,	"Total ECC Blocks Processed: %#.8x (%u)", TotalECC.BlocksProcessed, TotalECC.BlocksProcessed);
}


VOID
ReadFileECC(HANDLE LogHandle, CHAR *strFile) {
	DWORD Bytes;
    HANDLE hFile;
	BOOL bReturn, bEOF;

	xSetFunctionName(LogHandle, "ReadFileECC");
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "File: %s", strFile);

	// Clear per file counters
	ZeroMemory(&FileECC, sizeof(FileECC));

	// Open file
	hFile = CreateFile(strFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if(hFile == INVALID_HANDLE_VALUE) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"CreateFile(%s) returned INVALID_HANDLE_VALUE, Error Code = %u", strFile, GetLastError());
		return;
	}

	bEOF = FALSE;
	while(!bEOF) {
		// Read from file
		bReturn = ReadFile(hFile, Buf, BufSize, &Bytes, NULL);
		if(bReturn) {
			if(Bytes == 0) {
				// EOF
				bEOF = TRUE;
			} else {
				// Log ECC for this read
				GetECCPage(LogHandle, Bytes);
			}
		} else {
			// error
			bEOF = TRUE;
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
				"ReadFile(%s) Failed Error Code = %u", strFile, GetLastError());
		}
	}

	// Log ECC for this file
	LogFileECC(LogHandle, strFile);

	// Close file
	if(!CloseHandle(hFile)) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"CloseHande(%s) Error Code = %u", strFile, GetLastError());
	}
}


VOID
ListFiles(HANDLE LogHandle, CHAR *strFind) {
	CHAR strPath[MAX_PATH], strNewFind[MAX_PATH], strFile[MAX_PATH];
	int i, LastBackSlash;
    WIN32_FIND_DATA wfd;
    HANDLE hFind;

    xSetFunctionName(LogHandle, "List Files");

	LastBackSlash = 0;
	for(i = 0; i < lstrlenA(strFind); i++) {
		if(strFind[i] == '\\') {
			LastBackSlash = i;
		}
	}
	lstrcpynA(strPath, strFind, LastBackSlash + 1);


    // Start the find and check for failure.
    hFind = FindFirstFile(strFind, &wfd);

    if(INVALID_HANDLE_VALUE == hFind) {
        xLog(gbConsoleOut, LogHandle, XLL_FAIL, "FindFirstFile failed");
    } else {
        // Display each file and ask for the next.
        do {
			if((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
				// Recurse on sub directory
				lstrcpyA(strNewFind, strPath);
				lstrcatA(strNewFind, "\\");
				lstrcatA(strNewFind, wfd.cFileName);					// add directory name
				lstrcatA(strNewFind, "\\*");							// add wild card
				ListFiles(LogHandle, strNewFind);
			} else {
				// Read file
				lstrcpyA(strFile, strPath);
				lstrcatA(strFile, "\\");
				lstrcatA(strFile, wfd.cFileName);
				ReadFileECC(LogHandle, strFile);
			}
        } while(FindNextFile(hFind, &wfd));

        // Close the find handle.
        FindClose(hFind);
    }
}


VOID
ReadLBA(HANDLE LogHandle, ULONG LBA, USHORT Sectors) {
	SCSI_PASS_THROUGH_DIRECT PassThrough;
	SENSE_DATA SenseData;
	DWORD cbBytesReturned;
	PCDB Cdb;
	BOOL bReturn;
	ULONG i, Reps;
	USHORT RemainSectors;

	// Divide sectors into number of MaxSector reads
	Reps = Sectors/MaxSectors;
	RemainSectors = (USHORT)(Sectors%MaxSectors);

	// Prepare read CDB
	ZeroMemory(&PassThrough, sizeof(PassThrough));

	PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
	PassThrough.DataBuffer = Buf;
	PassThrough.DataTransferLength = MaxSectors*BytesPerSector;

	Cdb = (PCDB)&PassThrough.Cdb;
	Cdb->CDB10.OperationCode = SCSIOP_READ;
	Cdb->CDB10.ForceUnitAccess = 1;
	Cdb->CDB10.TransferBlocks = IdexConstantUshortByteSwap(MaxSectors);
	
	// Read MaxSector size chunks
	for(i = 0; i < Reps; i++) {
		// Set starting LBA
		Cdb->CDB10.LogicalBlock = IdexConstantUlongByteSwap(LBA + i*MaxSectors);

		bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
									&PassThrough, sizeof(PassThrough), 
									&SenseData, sizeof(SenseData),
									&cbBytesReturned, NULL);

		if((!bReturn) || (cbBytesReturned > 0)) {
			// Read error
			if(!bReturn) {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SCSIOP_READ FAILED LBA = %u, Sectors = %u, LastError = %d", LBA + i*MaxSectors, MaxSectors, GetLastError());
			}

			if(cbBytesReturned > 0) {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SCSIOP_READ FAILED LBA = %u, Sectors = %u, cbBytesReturned > 0", LBA, MaxSectors);
				DVDECCLogSenseData(LogHandle, SenseData);
			}
		}
	}

	// Read remaining LBAs
	if(RemainSectors > 0) {
		PassThrough.DataTransferLength = RemainSectors*BytesPerSector;
		Cdb->CDB10.LogicalBlock = IdexConstantUlongByteSwap(LBA + Reps*MaxSectors);
		Cdb->CDB10.TransferBlocks = IdexConstantUshortByteSwap(RemainSectors);

		bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
									&PassThrough, sizeof(PassThrough), 
									&SenseData, sizeof(SenseData),
									&cbBytesReturned, NULL);

		if((!bReturn) || (cbBytesReturned > 0)) {
			// Read error
			if(!bReturn) {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SCSIOP_READ FAILED LBA = %u, Sectors = %u, LastError = %d", LBA + i*MaxSectors, RemainSectors, GetLastError());
			}

			if(cbBytesReturned > 0) {
				xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SCSIOP_READ FAILED LBA = %u, Sectors = %u, cbBytesReturned > 0", LBA, RemainSectors);
				DVDECCLogSenseData(LogHandle, SenseData);
			}
		}
	}

	// Log ECC
	GetECCPage(LogHandle, Sectors*BytesPerSector);
}


VOID
LBAECC(HANDLE LogHandle, ULONG StartLBA, ULONG EndLBA) {
	ULONG i, NumLBAs, Reps;
	USHORT RemainLBAs;
	CHAR strFile[MAX_PATH];

	xSetFunctionName(LogHandle, "LBAECC");

	wsprintfA(strFile, "LBA (%u - %u)", StartLBA, EndLBA);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Reading %s", strFile);

	// Clear per file counters
	ZeroMemory(&FileECC, sizeof(FileECC));

	// Compute Reps based on the number of LBAs that fit in the cache
	NumLBAs = EndLBA - StartLBA + 1;
	Reps = NumLBAs/CacheLBAs;
	RemainLBAs = (USHORT)(NumLBAs%CacheLBAs);


	// Read maximum allowed sectors
	for(i = 0; i < Reps; i++) {
		ReadLBA(LogHandle, i*CacheLBAs + StartLBA, CacheLBAs);
		
	}

	// Read the remaining LBAs
	if(RemainLBAs > 0) {
		ReadLBA(LogHandle, Reps*CacheLBAs + StartLBA, RemainLBAs);
	}

	// Log ECC for this LBA range
	LogFileECC(LogHandle, strFile);
}


VOID
DVDECCStartTest(HANDLE LogHandle) {
	CHAR strFile[MAX_PATH];
	ULONG StartLBA, EndLBA;


	xSetOwnerAlias(LogHandle, "a-emebac");
	xSetComponent(LogHandle, "Hardware", "DVD ECC");

	// Get ini settings
	GetProfileStringA("dvdecc", "FileName", "", strFile, MAX_PATH);
	StartLBA = GetProfileIntA("dvdecc", "StartLBA", 0);
	EndLBA = GetProfileIntA("dvdecc", "EndLBA", 0);
	if(GetProfileIntA("dvdecc", "LogPerRead", 0) == 1) {
		LogPerRead = TRUE;
	} else {
		LogPerRead = FALSE;
	}

  	// Allocate Buf
	Buf = (UCHAR *)VirtualAlloc(NULL, BufSize, MEM_COMMIT, PAGE_READWRITE);
	if(Buf == NULL) {
		xSetFunctionName(LogHandle, "Allocate Buffer");
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "VirtualAlloc (%u bytes) FAILED LastError = %d", 
				BufSize, GetLastError());
		return;
	}

	// Open DVD device
	hDevice = CreateFile("cdrom0:",	GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, NULL);

	if(hDevice == INVALID_HANDLE_VALUE) {
		xSetFunctionName(LogHandle, "Open DVD Device");
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "CreateFile FAILED LastError = %d", GetLastError());
		return;
	}

	// Read files
	if(lstrlenA(strFile) > 0) {
		ZeroMemory(&TotalECC, sizeof(TotalECC));
		ListFiles(LogHandle, strFile);
		LogTotalECC(LogHandle);
	}

	// Read LBAs
	if(EndLBA > 0) {
		ZeroMemory(&TotalECC, sizeof(TotalECC));
		LBAECC(LogHandle, StartLBA, EndLBA);
		LogTotalECC(LogHandle);
	}

	// Close device
	if(!CloseHandle(hDevice)) {
		xSetFunctionName(LogHandle, "Close Device");
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"CloseHande(DVD Device) Error Code = %u", GetLastError());
	}
}


VOID
DVDECCEndTest() {
}


#if !defined(HARNESSLIB)
void __cdecl main() {
	HANDLE LogHandle;


	// Do the initialization that the harness does
	// dxconio
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);
	
	// xlog
	LogHandle = xCreateLog_A("t:\\dvdecc.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("DVDECC: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
		xDebugStringA("DVDECC: End - Waiting for reboot...\n");
		Sleep(INFINITE);
    }
	
	gbConsoleOut = TRUE;

	// Copy ini to t:\\ if not already there

	// Start Test
	DVDECCStartTest(LogHandle);

	// End Test
	DVDECCEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("DVDECC: End - Waiting for reboot...\n");
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
DECLARE_EXPORT_DIRECTORY( dvdecc )
#pragma data_seg()

BEGIN_EXPORT_TABLE( dvdecc )
    EXPORT_TABLE_ENTRY( "StartTest", DVDECCStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DVDECCEndTest )
END_EXPORT_TABLE( dvdecc )


