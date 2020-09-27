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

#define AT_STANDBY_IMMEDIATE	0xE0
#define AT_IDLE_IMMEDIATE		0xE1


// Globals
HANDLE gLogHandle;
DISK_GEOMETRY gDiskGeometry;
LARGE_INTEGER gFrequency;
UCHAR *gcBuf;


VOID
LogSenseData(SENSE_DATA SenseData) {
	// Log SenseData
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData ErrorCode:  0x%.2x", SenseData.ErrorCode);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData Valid:  0x%.2x", SenseData.Valid);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData SegmentNumber:  0x%.2x", SenseData.SegmentNumber);	
    xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData SenseKey:  0x%.2x", SenseData.SenseKey);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData Reserved:  0x%.2x", SenseData.Reserved);	
    xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData IncorrectLength:  0x%.2x", SenseData.IncorrectLength);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData EndOfMedia:  0x%.2x", SenseData.EndOfMedia);	
    xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData FileMark:  0x%.2x", SenseData.FileMark);	
    xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData Information[0]:  0x%.2x", SenseData.Information[0]);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData Information[1]:  0x%.2x", SenseData.Information[1]);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData Information[2]:  0x%.2x", SenseData.Information[2]);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData Information[3]:  0x%.2x", SenseData.Information[3]);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData AdditionalSenseLength:  0x%.2x", SenseData.AdditionalSenseLength);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData CommandSpecificInformation[0]:  0x%.2x", SenseData.CommandSpecificInformation[0]);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData CommandSpecificInformation[1]:  0x%.2x", SenseData.CommandSpecificInformation[1]);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData CommandSpecificInformation[2]:  0x%.2x", SenseData.CommandSpecificInformation[2]);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData CommandSpecificInformation[3]:  0x%.2x", SenseData.CommandSpecificInformation[3]);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData AdditionalSenseCode:  0x%.2x", SenseData.AdditionalSenseCode);	
    xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData AdditionalSenseCodeQualifier:  0x%.2x", SenseData.AdditionalSenseCodeQualifier);	
    xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData FieldReplaceableUnitCode:  0x%.2x", SenseData.FieldReplaceableUnitCode);	
    xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData SenseKeySpecific[0]:  0x%.2x", SenseData.SenseKeySpecific[0]);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData SenseKeySpecific[1]:  0x%.2x", SenseData.SenseKeySpecific[1]);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData SenseKeySpecific[2]:  0x%.2x", SenseData.SenseKeySpecific[2]);	
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SenseData SenseKeySpecific[3]:  0x%.2x", SenseData.SenseKeySpecific[3]);	
}


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


LARGE_INTEGER
ReadLBA(HANDLE hDevice, ULONG LBA, USHORT Sectors) {
	SCSI_PASS_THROUGH_DIRECT PassThrough;
	SENSE_DATA SenseData;
	DWORD cbBytesReturned;
	PCDB Cdb;
	LARGE_INTEGER StartTime, EndTime, ReturnValue;
	BOOL bReturn;

    RtlZeroMemory(&PassThrough, sizeof(PassThrough));

    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
    PassThrough.DataBuffer = gcBuf;
    PassThrough.DataTransferLength = Sectors*gDiskGeometry.BytesPerSector;

	Cdb = (PCDB)&PassThrough.Cdb;
    Cdb->CDB10.OperationCode = SCSIOP_READ;
	Cdb->CDB10.ForceUnitAccess = 1;
	Cdb->CDB10.LogicalBlock = RtlUlongByteSwap(LBA);
	Cdb->CDB10.TransferBlocks = RtlUshortByteSwap(Sectors);

	// Start timer
	QueryPerformanceCounter(&StartTime);

    bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
								&PassThrough, sizeof(PassThrough), 
								&SenseData, sizeof(SenseData),
								&cbBytesReturned, NULL);

	// Stop timer
	QueryPerformanceCounter(&EndTime);

	if((!bReturn) || (cbBytesReturned > 0)) {
		if(!bReturn) {
			xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "SCSIOP_READ FAILED LBA = %u, Sectors = %u, LastError = %d", LBA, Sectors, GetLastError());
		}

		if(cbBytesReturned > 0) {
			xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "SCSIOP_READ FAILED LBA = %u, cbBytesReturned > 0", LBA);
			LogSenseData(SenseData);
		}
		ReturnValue.QuadPart = -1;
	} else {
		// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SCSIOP_READ LBA = %u, Sectors = %u, %f ms", LBA, Sectors, 1000.0*(EndTime.QuadPart - StartTime.QuadPart)/gFrequency.QuadPart);
		ReturnValue.QuadPart = EndTime.QuadPart - StartTime.QuadPart;
	}
	return ReturnValue;
}


LARGE_INTEGER
SeekLBA(HANDLE hDevice, ULONG LBA) {
	SCSI_PASS_THROUGH_DIRECT PassThrough;
	SENSE_DATA SenseData;
	DWORD cbBytesReturned;
	PCDB Cdb;
	BOOL bReturn;
	LARGE_INTEGER StartTime, EndTime, ReturnValue;

	RtlZeroMemory(&PassThrough, sizeof(PassThrough));

    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);

	Cdb = (PCDB)&PassThrough.Cdb;
	Cdb->SEEK.OperationCode = SCSIOP_SEEK;
	*((PULONG)Cdb->SEEK.LogicalBlockAddress) = RtlUlongByteSwap(LBA);

	// Start timer
	QueryPerformanceCounter(&StartTime);

	bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
								&PassThrough, sizeof(PassThrough), 
								&SenseData, sizeof(SenseData),
								&cbBytesReturned, NULL);

	// Stop timer
	QueryPerformanceCounter(&EndTime);

	if((!bReturn) || (cbBytesReturned > 0)) {
		if(!bReturn) {
			xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "SCSIOP_SEEK FAILED LastError = %d", GetLastError());
		}

		if(cbBytesReturned > 0) {
			xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "SCSIOP_SEEK FAILED cbBytesReturned > 0");
			LogSenseData(SenseData);
		}

		ReturnValue.QuadPart = -1;
 	} else {
		// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SCSIOP_SEEK LBA = %u, %f ms",
		//	LBA, 1000.0*(EndTime.QuadPart - StartTime.QuadPart)/gFrequency.QuadPart);
		ReturnValue.QuadPart = EndTime.QuadPart - StartTime.QuadPart;
	}
	return ReturnValue;
}


DOUBLE
AccessTime(HANDLE hDevice, ULONG Start, ULONG End, ULONG Reps) {
	LARGE_INTEGER ReturnTime, TotalTime;
	DOUBLE ReturnVal;
	ULONG LBA;

	// Initialize
	TotalTime.QuadPart = 0;
	ReturnVal = 1.0/(DOUBLE)Reps;

	// Goto End
	ReturnTime = ReadLBA(hDevice, End,1);
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

	// return average access time in ms (already initialized to 1/Reps)
	ReturnVal *= (DOUBLE)TotalTime.QuadPart;
	ReturnVal /= (DOUBLE)gFrequency.QuadPart;
	ReturnVal *= 1000.0;	// convert to ms
	return ReturnVal;
}


DOUBLE
StreamTime(HANDLE hDevice, ULONG Start, ULONG End, USHORT MaxSectors) {
	LARGE_INTEGER ReturnTime, TotalTime;
	DOUBLE ReturnVal;
	ULONG i, Reps;
	USHORT RemainSectors;

	// Init Totaltime
	TotalTime.QuadPart = 0;

	// Compute Reps based on Sectors and 64 sector limit per read
	Reps = (End - Start)/MaxSectors;
	RemainSectors = (USHORT)((End - Start)%MaxSectors);

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
	ReturnVal /= 131072.0;	// convert to mbits
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
StartUpTime(HANDLE hDevice, UCHAR Command, ULONG Reps) {
	LARGE_INTEGER StartTime, EndTime, ReturnTime, TotalTime;
	ULONG i;
	DOUBLE ReturnVal;
	SCSI_PASS_THROUGH_DIRECT PassThrough;
	SENSE_DATA SenseData;
	DWORD cbBytesReturned;
	PCDB Cdb;
	BOOL bReturn;

	// Goto read location
	ReturnTime = ReadLBA(hDevice, 1, 1);

	TotalTime.QuadPart = 0;

	for(i = 0; i < Reps; i++) {
		// Put drive in Standby mode
		if(!SetPowerMode(hDevice, Command)) {
			xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "SetPowerMode(%#x) FAILED", Command);
			return 0;
		}
	
		// Wait for drive to spin down
		Sleep(6000);

		// Set up pass through to read
	    RtlZeroMemory(&PassThrough, sizeof(PassThrough));
		PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
		PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
		PassThrough.DataBuffer = gcBuf;
		PassThrough.DataTransferLength = 1*gDiskGeometry.BytesPerSector;
		Cdb = (PCDB)&PassThrough.Cdb;
		Cdb->CDB10.OperationCode = SCSIOP_READ;
		Cdb->CDB10.ForceUnitAccess = 1;
		Cdb->CDB10.LogicalBlock = RtlUlongByteSwap(1);
		Cdb->CDB10.TransferBlocks = RtlUshortByteSwap(1);

		// Start timer
		QueryPerformanceCounter(&StartTime);
		
		// Read from drive until successful
		do {
			bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
										&PassThrough, sizeof(PassThrough), 
										&SenseData, sizeof(SenseData),
										&cbBytesReturned, NULL);
		} while(bReturn == FALSE);

		// Stop timer
		QueryPerformanceCounter(&EndTime);

		TotalTime.QuadPart += (EndTime.QuadPart - StartTime.QuadPart);
	}

	// Return average Startup time
	ReturnVal = (DOUBLE)TotalTime.QuadPart/(DOUBLE)gFrequency.QuadPart;
	ReturnVal /= (DOUBLE)Reps;
	return ReturnVal;
}


VOID
WINAPI
DVDPerfStartTest(HANDLE LogHandle) {
	HANDLE hDevice;
	BOOL bReturn, bDoAuth;
    DWORD cbBytesReturned;
	SENSE_DATA SenseData;
	DOUBLE dTime;
	ULONG RestartReps;
	ULONG ThirdStartLBA, ThirdEndLBA, ThirdReps;
	ULONG FullStartLBA, FullEndLBA, FullReps;
	ULONG StreamStartLBA, StreamEndLBA;
	USHORT MaxSectors;

	// Set global log handle
	gLogHandle = LogHandle;

	// Get timer frequency (ticks/sec)
	QueryPerformanceFrequency(&gFrequency);

    xSetOwnerAlias(gLogHandle, "a-emebac");
	xSetComponent(gLogHandle, "Hardware", "DVDPerf");
	xSetFunctionName(gLogHandle, "StartTest");

	// open device
	hDevice = CreateFile("cdrom0:",	GENERIC_READ, 0, NULL, OPEN_EXISTING, 
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING,NULL);

	if(hDevice == INVALID_HANDLE_VALUE) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "CreateFile FAILED LastError = %d", GetLastError());
		return;
	}

	// Get authentication parameter from ini
	bDoAuth = (BOOL)GetProfileInt(TEXT("dvdperf"), TEXT("DoAuth"), 0);
	// Authenticate a Xbox DVD 
	// If the media is not a Xbox DVD, then don't care if this fails
	if(bDoAuth) {
		bReturn = DeviceIoControl(hDevice, IOCTL_CDROM_AUTHENTICATION_SEQUENCE, 
									NULL, 0, 
									&SenseData, sizeof(SenseData),
									&cbBytesReturned, NULL);

		if((!bReturn) || (cbBytesReturned > 0)) {
			if(!bReturn) {
				xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "IOCTL_CDROM_AUTHENTICATION_SEQUENCE FAILED LastError = %d", GetLastError());
			}

			if(cbBytesReturned > 0) {
				xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "IOCTL_CDROM_AUTHENTICATION_SEQUENCE cbBytesReturned > 0");
				LogSenseData(SenseData);
			}
		} else {
			xLog(gbConsoleOut, gLogHandle, XLL_PASS, "IOCTL_CDROM_AUTHENTICATION_SEQUENCE Succeeded");
		}
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "IOCTL_CDROM_AUTHENTICATION_SEQUENCE Not performed");
	}
	
	// get geometry
	bReturn = DeviceIoControl(hDevice, IOCTL_CDROM_GET_DRIVE_GEOMETRY, 
								NULL, 0, 
								&gDiskGeometry, sizeof(gDiskGeometry), 
								&cbBytesReturned, 
								NULL);

	if(!bReturn) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "IOCTL_CDROM_GET_DRIVE_GEOMETRY FAILED LastError = %d", GetLastError());
		CloseHandle(hDevice);
		return;
	} else {
		// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Cylinders = %08x:%08x", gDiskGeometry.Cylinders.HighPart, gDiskGeometry.Cylinders.LowPart);
		// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Cylinders = %u", gDiskGeometry.Cylinders.LowPart);
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Cylinders = %u", gDiskGeometry.Cylinders.QuadPart);
		// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "MediaType = %s", gDiskGeometry.MediaType);
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "TracksPerCylinder = %u", gDiskGeometry.TracksPerCylinder);
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SectorsPerTrack = %u", gDiskGeometry.SectorsPerTrack);
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "BytesPerSector = %u", gDiskGeometry.BytesPerSector);
	}
	
	// allocate gcBuf
	gcBuf = (UCHAR *)VirtualAlloc(NULL, 64*gDiskGeometry.BytesPerSector, MEM_COMMIT, PAGE_READWRITE);
	if(gcBuf == NULL) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "VirtualAlloc (%u bytes) FAILED LastError = %d", 
				64*gDiskGeometry.BytesPerSector, GetLastError());
		CloseHandle(hDevice);
		return;
	}

	// Get parameters from ini file
	RestartReps = GetProfileInt(TEXT("dvdperf"), TEXT("RestartReps"), 10);
	ThirdStartLBA = GetProfileInt(TEXT("dvdperf"), TEXT("ThirdStartLBA"), 0);
	ThirdEndLBA = GetProfileInt(TEXT("dvdperf"), TEXT("ThirdEndLBA"), (ULONG)gDiskGeometry.Cylinders.QuadPart/3);
	ThirdReps = GetProfileInt(TEXT("dvdperf"), TEXT("ThirdReps"), 200);
	FullStartLBA = GetProfileInt(TEXT("dvdperf"), TEXT("FullStartLBA"), 0);
	FullEndLBA = GetProfileInt(TEXT("dvdperf"), TEXT("FullEndLBA"), (ULONG)gDiskGeometry.Cylinders.QuadPart - 1);
	FullReps = GetProfileInt(TEXT("dvdperf"), TEXT("FullReps"), 200);
	StreamStartLBA = GetProfileInt(TEXT("dvdperf"), TEXT("StreamStartLBA"), 0);
	StreamEndLBA = GetProfileInt(TEXT("dvdperf"), TEXT("StreamEndLBA"), (ULONG)gDiskGeometry.Cylinders.QuadPart/4);

	// Average Recovery Time from Standby
	xSetFunctionName(gLogHandle, "StandbyTime");
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Average Recovery Time from Standby: Reps = %u", RestartReps);
	dTime = StartUpTime(hDevice, AT_STANDBY_IMMEDIATE, RestartReps);
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Average Recovery Time from Standby = %f s", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Average Recovery Time from Standby FAILED");
	}

	// Average Recovery Time from Idle
	xSetFunctionName(gLogHandle, "IdleTime");
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Average Recovery Time from Idle: Reps = %u", RestartReps);
	dTime = StartUpTime(hDevice, AT_IDLE_IMMEDIATE, RestartReps);
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Average Recovery Time from Idle = %f s", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Average Recovery Time from Idle FAILED");
	}


	// Measure average 1/3 stroke access times
	xSetFunctionName(gLogHandle, "Average (First 1/3 stroke)");
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, Reps = %u", ThirdStartLBA, ThirdEndLBA, ThirdReps);
	
	dTime = AccessTime(hDevice, ThirdStartLBA, ThirdEndLBA, ThirdReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Average Access Time (1/3 Stroke) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Average Access Time (1/3 Stroke) FAILED");
	}

	// Measure full-stroke access time
	xSetFunctionName(gLogHandle, "Full-Stroke");
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, Reps = %u", FullStartLBA, FullEndLBA, FullReps);
	
	dTime = AccessTime(hDevice, FullStartLBA, FullEndLBA, FullReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Average Full-Stroke Access Time = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Average Full-Stroke Access Time FAILED");
	}

	//
	// Measure data stream rate
	xSetFunctionName(gLogHandle, "Sustained Data Rate");
	for(USHORT i = 0; i <=6; i++) {
		MaxSectors = 1<<i;
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, MaxSectors = %u", StreamStartLBA, StreamEndLBA, MaxSectors);
	
		dTime = StreamTime(hDevice, StreamStartLBA, StreamEndLBA, MaxSectors);
	
		if(dTime > 0) {
			xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Average Sustained Data Rate = %f Mbits/s", dTime);
		} else {
			xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Average Sustained Data Rate FAILED");
		}
	}

	// free gcBuf
	xSetFunctionName(gLogHandle, "EndTest");
	if(!VirtualFree(gcBuf, 0, MEM_RELEASE)) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "VirtualFree FAILED LastError = %d", GetLastError());
	}

	// close device
	if(!CloseHandle(hDevice)) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "CloseHandle FAILED LastError = %d", GetLastError());
	}
}


VOID WINAPI
DVDPerfEndTest() {
}


#if !defined(HARNESSLIB)
void __cdecl main() {
	HANDLE LogHandle;


	// Do the initialization that the harness does
	// dxconio
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);
	
	// xlog
	LogHandle = xCreateLog_W(L"t:\\dvdperf.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("DVDPerf: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
		xDebugStringA("DVDPerf: End - Waiting for reboot...\n");
		Sleep(INFINITE);
    }
	
	gbConsoleOut = TRUE;

	// Copy ini to t:\\ if not already there

	// Start Test
	DVDPerfStartTest(LogHandle);

	// End Test
	DVDPerfEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("DVDPerf: End - Waiting for reboot...\n");
	Sleep(INFINITE);

	// Do the de-initialize that the harness does
	// dxconio
	xReleaseConsole();

	// xlog
	xCloseLog(LogHandle);
}
#endif

//
// Export function pointers of StartTest and EndTest
//
#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( dvdperf )
#pragma data_seg()

BEGIN_EXPORT_TABLE( dvdperf )
    EXPORT_TABLE_ENTRY( "StartTest", DVDPerfStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DVDPerfEndTest )
END_EXPORT_TABLE( dvdperf )

