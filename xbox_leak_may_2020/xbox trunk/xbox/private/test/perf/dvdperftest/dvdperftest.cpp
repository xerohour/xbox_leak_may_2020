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


// Globals
HANDLE gLogHandle;
DISK_GEOMETRY gDiskGeometry;
LARGE_INTEGER gFrequency;
UCHAR *gcBuf;
ULONG gErrorLBA; // most recent Error LBA
USHORT gSecurityChunk = 4608;
USHORT gMaxErrors = 3;

struct ErrorLBA {
	ULONG LBA;
	USHORT NumErrors;
	ErrorLBA* Next;
};

// List of Bad LBAs
ErrorLBA* LBAList = NULL;


// List functions
VOID ListInsert(ErrorLBA*& LBAList, ULONG LBA) {
	ErrorLBA* NewLBA = new ErrorLBA;
	NewLBA->LBA = LBA;
	NewLBA->NumErrors = 1;
	NewLBA->Next = LBAList;
	LBAList = NewLBA;
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Added LBA = %u to bad list", LBA);
}


ErrorLBA* ListSearch(ErrorLBA*& LBAList, ULONG LBA) {
	ErrorLBA* CurrLBA;

	for(CurrLBA = LBAList; CurrLBA != NULL; CurrLBA = CurrLBA->Next) {
		
		if(CurrLBA->LBA == LBA) {
			// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Found LBA = %u, NumErrors = %u", CurrLBA->LBA, CurrLBA->NumErrors);
			return CurrLBA;
		}
	}
	// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Didn't find LBA = %u", LBA);
	return NULL;
}


VOID
GetDriveID(char* device, char* model, char* serial, char* firmware) {
    unsigned i;
    DWORD returned;
    NTSTATUS status;
    HANDLE fileHandle;
    OBJECT_ATTRIBUTES ObjA;
    OBJECT_STRING VolumeString;
    IO_STATUS_BLOCK IoStatusBlock;
    char buffer[sizeof(ATA_PASS_THROUGH) + 512];
    PATA_PASS_THROUGH atapt = (PATA_PASS_THROUGH)buffer;


    RtlInitObjectString(&VolumeString, device);
    InitializeObjectAttributes(&ObjA,&VolumeString,OBJ_CASE_INSENSITIVE,NULL,NULL);

    status = NtCreateFile(&fileHandle,
                            SYNCHRONIZE|GENERIC_READ,
                            &ObjA,
                            &IoStatusBlock,
                            0,
                            FILE_ATTRIBUTE_NORMAL,
                            FILE_SHARE_READ, FILE_OPEN,
                            FILE_SYNCHRONOUS_IO_NONALERT);

    atapt->DataBufferSize = 512;
    atapt->DataBuffer = atapt + 1;

    atapt->IdeReg.bFeaturesReg     = 0;
    atapt->IdeReg.bSectorCountReg  = 0;
    atapt->IdeReg.bSectorNumberReg = 0;
    atapt->IdeReg.bCylLowReg       = 0;
    atapt->IdeReg.bCylHighReg      = 0;
    atapt->IdeReg.bDriveHeadReg    = 0;
    atapt->IdeReg.bHostSendsData   = 0;

    if(strstr(device, "CdRom") != NULL) atapt->IdeReg.bCommandReg = 0xa1;
    else atapt->IdeReg.bCommandReg = 0xec;

    status = DeviceIoControl(fileHandle,
                             IOCTL_IDE_PASS_THROUGH,
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             atapt,
                             sizeof(ATA_PASS_THROUGH),
                             &returned,
                             FALSE);
    NtClose(fileHandle);

    PIDE_IDENTIFY_DATA IdData = (PIDE_IDENTIFY_DATA)atapt->DataBuffer;

    for ( i=0; i<sizeof(IdData->ModelNumber); i+=2 ) {
        model[i + 0] = IdData->ModelNumber[i + 1];
        model[i + 1] = IdData->ModelNumber[i + 0];
    }
    model[i] = 0;
    //DebugPrint("Model Number: %s\n", model);

    for ( i=0; i<sizeof(IdData->SerialNumber); i+=2 ) {
        serial[i + 0] = IdData->SerialNumber[i + 1];
        serial[i + 1] = IdData->SerialNumber[i + 0];
    }
    serial[i] = 0;
    //DebugPrint("Serial Number: %s\n", serial);

    for ( i=0; i<sizeof(IdData->FirmwareRevision); i+=2 ) {
        firmware[i + 0] = IdData->FirmwareRevision[i + 1];
        firmware[i + 1] = IdData->FirmwareRevision[i + 0];
    }
    firmware[i] = 0;
    //DebugPrint("Firmware Revision: %s\n", firmware);

	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Model Number: %s", model);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Serial Number: %s", serial);
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Firmware Revision: %s", firmware);

}


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

		ReturnValue.QuadPart = -1;
		if(!bReturn) {
			xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "SCSIOP_READ FAILED LBA = %u, Sectors = %u, LastError = %d", LBA, Sectors, GetLastError());
			//ReturnValue.QuadPart = -1;
		}

		if(cbBytesReturned > 0) {

			// If got a command error, set ReturnValue to indicate result
			if(SenseData.SenseKey == 0x05 && SenseData.AdditionalSenseCode == 0x81) {
				gErrorLBA = LBA;
				ReturnValue.QuadPart = 0;
			}

			// If got a read, cache fill, insufficient time or hardware error, keep track of error LBA and set ReturnValue to indicate result
			else if( (SenseData.SenseKey == 0x03 && SenseData.AdditionalSenseCode == 0x80) ||
					 (SenseData.SenseKey == 0x01 && SenseData.AdditionalSenseCode == 0x80) ||
					 (SenseData.SenseKey == 0x06 && SenseData.AdditionalSenseCode == 0x2e) ||
					 (SenseData.SenseKey == 0x04 && SenseData.AdditionalSenseCode == 0x00) ) {
				gErrorLBA = LBA;
				ReturnValue.QuadPart = -2;
				
				// Is this LBA already on the bad list?
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Looking for LBA = %u on bad list", LBA);
				ErrorLBA* ThisLBA = ListSearch(LBAList, LBA);

				
				// If LBA was found, increment its number of errors
				if(ThisLBA != NULL) {
					xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Incrementing LBA = %u's NumErrors from %u to %u", ThisLBA->LBA, ThisLBA->NumErrors, (ThisLBA->NumErrors)+1);
					++(ThisLBA->NumErrors);
				}

				// If LBA was not found, add it
				else {
					xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Adding LBA = %u to bad list", LBA);
					ListInsert(LBAList, LBA);
				}
			}

			else {
				xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "SCSIOP_READ FAILED LBA = %u, cbBytesReturned > 0", LBA);
				LogSenseData(SenseData);
			}
		}
	} else {
		// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "SCSIOP_READ LBA = %u, Sectors = %u, %f ms", LBA, Sectors, 1000.0*(EndTime.QuadPart - StartTime.QuadPart)/gFrequency.QuadPart);
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
	/*
	// Goto End
	ReturnTime = ReadLBA(hDevice, End,1);
	if(ReturnTime.QuadPart == -1) {
			return -1;
	} 
	*/
	// LBA = Start;

	LBA = End;
	while(Reps > 0) {

		// Is Start LBA on the bad list?
		// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Looking for LBA = %u on bad list before attempting to read", i*MaxSectors + Start);
		ErrorLBA* StartLBA = ListSearch(LBAList, Start);

		// If Start LBA is on list with 3 errors, skip it
		if(StartLBA != NULL && StartLBA->NumErrors == 3) {
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "More than 3 Read/Cache Fill/Insufficient Time/Hardware Errors encountered at LBA = %u. Terminating test...", StartLBA->LBA);
			return -1;
		}

		// Is End LBA on the bad list?
		// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Looking for LBA = %u on bad list before attempting to read", i*MaxSectors + Start);
		ErrorLBA* EndLBA = ListSearch(LBAList, End);

		// If End LBA is on list with 3 errors, skip it
		if(EndLBA != NULL && EndLBA->NumErrors == 3) {
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "More than 3 Read/Cache Fill/Insufficient Time/Hardware Errors encountered at LBA = %u. Terminating test...", EndLBA->LBA);
			return -1;
		}

		// Access the LBA
		ReturnTime = ReadLBA(hDevice, LBA, 1);
		if(ReturnTime.QuadPart == -1) {
			return -1;
		} 
		else if(ReturnTime.QuadPart == 0) { // if got a command error, in security chunk so can't complete test
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Command Error encountered at LBA = %u. Terminating test...", gErrorLBA);
			return -1;
		} 
		
		else if(ReturnTime.QuadPart == -2) { // if got a read, cache fill, insufficient time or hardware error, try again
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Read/Cache Fill/Insufficient Time/Hardware Error encountered at LBA = %u.  Retrying read...", gErrorLBA);
		} 
		else { // if last read was successful, swap LBA between Start and End and go on to next rep
			TotalTime.QuadPart += ReturnTime.QuadPart;
			if(LBA == Start) {
				LBA = End;
			} 
			else {
				LBA = Start;
			}
			--Reps;
		}
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
	ULONG i, Reps, SectorsRead = 0;
	// ULONG TotalSectors = End - Start + 1;
	USHORT RemainSectors, Retry;

	// Init Totaltime
	TotalTime.QuadPart = 0;

	// Compute Reps based on Sectors and 64 sector limit per read
	Reps = (End - Start + 1)/MaxSectors;
	RemainSectors = (USHORT)((End - Start + 1)%MaxSectors);

	/*
	// Is Start LBA on the bad list?
	// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Looking for LBA = %u on bad list before attempting to read", i*MaxSectors + Start);
	ErrorLBA* StartLBA = ListSearch(LBAList, Start);

	// If Start LBA is on list with 3 errors, skip it
	if(StartLBA != NULL && StartLBA->NumErrors == 3) {
		xLog(gbConsoleOut, gLogHandle, XLL_INFO, "More than 3 Read/Cache Fill/Insufficient Time/Hardware Errors encountered at LBA = %u. Skipping...", ThisLBA->LBA);
		continue;
	}

	ReturnTime = ReadLBA(hDevice, Start, 1);
	if(ReturnTime.QuadPart == -1) {
		return -1;
	}
	*/
	for(i=0; i<Reps; ++i) {
 
		// Is this LBA on the bad list?
		// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Looking for LBA = %u on bad list before attempting to read", i*MaxSectors + Start);
		ErrorLBA* ThisLBA = ListSearch(LBAList, i*MaxSectors + Start);

		// If LBA is on list with 3 errors, skip it
		if(ThisLBA != NULL && ThisLBA->NumErrors == 3) {
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "More than 3 Read/Cache Fill/Insufficient Time/Hardware Errors encountered at LBA = %u. Skipping...", ThisLBA->LBA);
			continue;
		}
				
		ReturnTime = ReadLBA(hDevice, i*MaxSectors + Start, MaxSectors);
		
		if(ReturnTime.QuadPart == -1) {
			return -1;
		} 
		
		else if(ReturnTime.QuadPart == 0) { // if got a command error, jump ahead 8.5 megs to get out of security chunk
			i += (gSecurityChunk/MaxSectors - 1);
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Command Error encountered at LBA = %u. Jumping out of security area...", gErrorLBA);
		} 
		
		else if(ReturnTime.QuadPart == -2) { // if got a read, cache fill, insufficient time or hardware error, check when it occurred
			
			if(i<Reps/4) { // got error early in test, so start over
				return 0;
			} 
			
			else { // got error late in test, so try again
				--i;
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Read/Cache Fill/Insufficient Time/Hardware Error encountered at LBA = %u.  Retrying read...", gErrorLBA);
			}
		} 
		
		else {
			TotalTime.QuadPart += ReturnTime.QuadPart;
			SectorsRead += MaxSectors;
			// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "ReturnTime = %d TotalTime = %u", ReturnTime.QuadPart, TotalTime.QuadPart);
		}
	}

	if(RemainSectors > 0) {
		do {
			// Is this LBA on the bad list?
			// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Looking for LBA = %u on bad list before attempting to read", i*MaxSectors + Start);
			ErrorLBA* ThisLBA = ListSearch(LBAList, Reps*MaxSectors + Start);

			// If LBA is on list with 3 errors, skip it
			if(ThisLBA != NULL && ThisLBA->NumErrors == 3) {
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "More than 3 Read/Cache Fill/Insufficient Time/Hardware Errors encountered at LBA = %u. Skipping...", ThisLBA->LBA);
				break;
			}

			Retry = 0;
			ReturnTime = ReadLBA(hDevice, Reps*MaxSectors + Start, RemainSectors);
		
			if(ReturnTime.QuadPart == -1) {
				return -1;
			} 
		
			else if(ReturnTime.QuadPart == 0) { // if got a command error, can't read remaining sectors
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Command Error encountered at LBA = %u. Jumping out of security area...", gErrorLBA);
			} 
		
			else if(ReturnTime.QuadPart == -2) { // if got a read error, try again
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Read/Cache Fill/Insufficient Time/Hardware Error encountered at LBA = %u.  Retrying read...", gErrorLBA);
				Retry = 1;
			} 
		
			else {
				TotalTime.QuadPart += ReturnTime.QuadPart;
				SectorsRead += RemainSectors;
				// xLog(gbConsoleOut, gLogHandle, XLL_INFO, "ReturnTime = %d TotalTime = %u", ReturnTime.QuadPart, TotalTime.QuadPart);
			}
		} while(Retry);
	}

	// return elapsed time in mbits/s
	ReturnVal = (DOUBLE)SectorsRead;
	ReturnVal *= (DOUBLE)gDiskGeometry.BytesPerSector;
	ReturnVal *= (DOUBLE)gFrequency.QuadPart;
	ReturnVal /= (DOUBLE)TotalTime.QuadPart;
	ReturnVal /= 131072.0;	// convert to mbits
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
	
	USHORT MaxSectors;
	char cdModel[64], cdSerial[64], cdFirmware[64];
	LARGE_INTEGER NumSectors;
	ULONG StartLBA = 0, NumReps = 200;

	// Set global log handle
	gLogHandle = LogHandle;

	// Get timer frequency (ticks/sec)
	QueryPerformanceFrequency(&gFrequency);

    xSetOwnerAlias(gLogHandle, "t-msanto");
	xSetComponent(gLogHandle, "Perf", "DVDPerfTest");
    xSetFunctionName(gLogHandle, "DriveInfo");

	// open device
	hDevice = CreateFile("cdrom0:",	GENERIC_READ, 0, NULL, OPEN_EXISTING, 
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING,NULL);

	if(hDevice == INVALID_HANDLE_VALUE) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "CreateFile FAILED LastError = %d", GetLastError());
		return;
	}

	xStartVariation( gbConsoleOut, gLogHandle, "DVD Drive Info" );

	// Get DVD drive info
	GetDriveID("\\Device\\CdRom0", cdModel, cdSerial, cdFirmware);
	
	// Get authentication parameter from ini
	bDoAuth = (BOOL)GetProfileInt(TEXT("dvdperftest"), TEXT("DoAuth"), 0);
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

	// get number of sectors
	NumSectors.QuadPart = gDiskGeometry.Cylinders.QuadPart * gDiskGeometry.TracksPerCylinder * gDiskGeometry.SectorsPerTrack;
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "NumberOfSectors = %u", NumSectors.QuadPart);

	// allocate gcBuf
	gcBuf = (UCHAR *)VirtualAlloc(NULL, 64*gDiskGeometry.BytesPerSector, MEM_COMMIT, PAGE_READWRITE);
	if(gcBuf == NULL) {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "VirtualAlloc (%u bytes) FAILED LastError = %d", 
				64*gDiskGeometry.BytesPerSector, GetLastError());
		CloseHandle(hDevice);
		return;
	}
	
	xSetFunctionName(gLogHandle, "AccessPerf");

	// Measure average access times
	xStartVariation( gbConsoleOut, gLogHandle, "Both Layers" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", StartLBA, (ULONG)NumSectors.QuadPart-1, NumReps);
	
	dTime = AccessTime(hDevice, StartLBA, (ULONG)NumSectors.QuadPart-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Both Layers) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Both Layers) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 0" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", StartLBA, ((ULONG)NumSectors.QuadPart/2)-1, NumReps);
	
	dTime = AccessTime(hDevice, StartLBA, ((ULONG)NumSectors.QuadPart/2)-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 0) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 0) FAILED");
	}

	xStartVariation( gbConsoleOut, gLogHandle, "1 Layer w/ Transition" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", StartLBA, ((ULONG)NumSectors.QuadPart/2)+1, NumReps);
	
	dTime = AccessTime(hDevice, StartLBA, ((ULONG)NumSectors.QuadPart/2)+1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (1 Layer w/ Transition) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (1 Layer w/ Transition) FAILED");
	}

	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 1" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", (ULONG)NumSectors.QuadPart/2, (ULONG)NumSectors.QuadPart-1, NumReps);
	
	dTime = AccessTime(hDevice, (ULONG)NumSectors.QuadPart/2, (ULONG)NumSectors.QuadPart-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 1) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 1) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 0, Inner 1/2" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", StartLBA, ((ULONG)NumSectors.QuadPart/4)-1, NumReps);
	
	dTime = AccessTime(hDevice, StartLBA, ((ULONG)NumSectors.QuadPart/4)-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 0, Inner 1/2) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 0, Inner 1/2) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "1/2 Layer w/ Transition" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", StartLBA, ((ULONG)NumSectors.QuadPart*3/4)+1, NumReps);
	
	dTime = AccessTime(hDevice, StartLBA, ((ULONG)NumSectors.QuadPart*3/4)+1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (1/2 Layer w/ Transition) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (1/2 Layer w/ Transition) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 0, Outer 1/2" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", (ULONG)NumSectors.QuadPart/4, ((ULONG)NumSectors.QuadPart/2)-1, NumReps);
	
	dTime = AccessTime(hDevice, (ULONG)NumSectors.QuadPart/4, ((ULONG)NumSectors.QuadPart/2)-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 0, Outer 1/2) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 0, Outer 1/2) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 1, Outer 1/2" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", (ULONG)NumSectors.QuadPart/2, ((ULONG)NumSectors.QuadPart*3/4)-1, NumReps);
	
	dTime = AccessTime(hDevice, (ULONG)NumSectors.QuadPart/2, ((ULONG)NumSectors.QuadPart*3/4)-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 1, Outer 1/2) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 1, Outer 1/2) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 1, Inner 1/2" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", (ULONG)NumSectors.QuadPart*3/4, (ULONG)NumSectors.QuadPart-1, NumReps);
	
	dTime = AccessTime(hDevice, (ULONG)NumSectors.QuadPart*3/4, (ULONG)NumSectors.QuadPart-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 1, Inner 1/2) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 1, Inner 1/2) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 0, Inner 1/3" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", StartLBA, ((ULONG)NumSectors.QuadPart/6)-1, NumReps);
	
	dTime = AccessTime(hDevice, StartLBA, ((ULONG)NumSectors.QuadPart/6)-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 0, Inner 1/3) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 0, Inner 1/3) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "1/3 Layer w/ Transition" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", StartLBA, ((ULONG)NumSectors.QuadPart*5/6)+1, NumReps);
	
	dTime = AccessTime(hDevice, StartLBA, ((ULONG)NumSectors.QuadPart*5/6)+1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (1/3 Layer w/ Transition) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (1/3 Layer w/ Transition) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 0, Outer 1/3" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", (ULONG)NumSectors.QuadPart/3, ((ULONG)NumSectors.QuadPart/2)-1, NumReps);
	
	dTime = AccessTime(hDevice, (ULONG)NumSectors.QuadPart/3, ((ULONG)NumSectors.QuadPart/2)-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 0, Outer 1/3) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 0, Outer 1/3) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 1, Outer 1/3" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", (ULONG)NumSectors.QuadPart/2, ((ULONG)NumSectors.QuadPart*2/3)-1, NumReps);
	
	dTime = AccessTime(hDevice, (ULONG)NumSectors.QuadPart/2, ((ULONG)NumSectors.QuadPart*2/3)-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 1, Outer 1/3) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 1, Outer 1/3) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 1, Inner 1/3" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", (ULONG)NumSectors.QuadPart*5/6, (ULONG)NumSectors.QuadPart-1, NumReps);
	
	dTime = AccessTime(hDevice, (ULONG)NumSectors.QuadPart*5/6, (ULONG)NumSectors.QuadPart-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 1, Inner 1/3) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 1, Inner 1/3) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 0, Inner 1/4" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", StartLBA, ((ULONG)NumSectors.QuadPart/8)-1, NumReps);
	
	dTime = AccessTime(hDevice, StartLBA, ((ULONG)NumSectors.QuadPart/8)-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 0, Inner 1/4) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 0, Inner 1/4) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "1/4 Layer w/ Transition" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", StartLBA, ((ULONG)NumSectors.QuadPart*7/8)+1, NumReps);
	
	dTime = AccessTime(hDevice, StartLBA, ((ULONG)NumSectors.QuadPart*7/8)+1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (1/4 Layer w/ Transition) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (1/4 Layer w/ Transition) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 0, Outer 1/4" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", (ULONG)NumSectors.QuadPart*3/8, ((ULONG)NumSectors.QuadPart/2)-1, NumReps);
	
	dTime = AccessTime(hDevice, (ULONG)NumSectors.QuadPart*3/8, ((ULONG)NumSectors.QuadPart/2)-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 0, Outer 1/4) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 0, Outer 1/4) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 1, Outer 1/4" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", (ULONG)NumSectors.QuadPart/2, ((ULONG)NumSectors.QuadPart*5/8)-1, NumReps);
	
	dTime = AccessTime(hDevice, (ULONG)NumSectors.QuadPart/2, ((ULONG)NumSectors.QuadPart*5/8)-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 1, Outer 1/4) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 1, Outer 1/4) FAILED");
	}
	// xEndVariation(gLogHandle);

	xStartVariation( gbConsoleOut, gLogHandle, "Layer 1, Inner 1/4" );
	xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, NumReps = %u", (ULONG)NumSectors.QuadPart*7/8, (ULONG)NumSectors.QuadPart-1, NumReps);
	
	dTime = AccessTime(hDevice, (ULONG)NumSectors.QuadPart*7/8, (ULONG)NumSectors.QuadPart-1, NumReps);
	
	if(dTime > 0) {
		xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Access Time (Layer 1, Inner 1/4) = %f ms", dTime);
	} else {
		xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Access Time (Layer 1, Inner 1/4) FAILED");
	}
	// xEndVariation(gLogHandle);


	xSetFunctionName(gLogHandle, "StreamPerf");
	
	// Measure data stream rates
	xStartVariation( gbConsoleOut, gLogHandle, "Layer 0, Quarter 1");
	for(USHORT i = 0; i <=6; ++i) {
		MaxSectors = 1<<i;

		//if(MaxSectors != 8 && MaxSectors != 32 ) {
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, MaxSectors = %u", StartLBA, ((ULONG)NumSectors.QuadPart/8)-1, MaxSectors);
		
			dTime = StreamTime(hDevice, StartLBA, ((ULONG)NumSectors.QuadPart/8)-1, MaxSectors);
		
			if(dTime > 0) {
				xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Sustained Data Rate (Layer 0, Q1, MS = %u) = %f Mbits/s", MaxSectors, dTime);
			} else if(dTime == 0) {
				--i;
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Read/Cache Fill/Insufficient Time/Hardware Error encountered at LBA = %u.  Restarting test...", gErrorLBA);
			} else {
				xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Sustained Data Rate (Layer 0, Q1, MS = %u) FAILED", MaxSectors);
			}
		//}
	}
	
	xStartVariation( gbConsoleOut, gLogHandle, "Layer 0, Quarter 2");
	for(USHORT i = 0; i <=6; ++i) {
		MaxSectors = 1<<i;

		//if(MaxSectors != 8 && MaxSectors != 32 ) {
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, MaxSectors = %u", (ULONG)NumSectors.QuadPart/8, ((ULONG)NumSectors.QuadPart/4)-1, MaxSectors);
		
			dTime = StreamTime(hDevice, (ULONG)NumSectors.QuadPart/8, ((ULONG)NumSectors.QuadPart/4)-1, MaxSectors);
		
			if(dTime > 0) {
				xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Sustained Data Rate (Layer 0, Q2, MS = %u) = %f Mbits/s", MaxSectors, dTime);
			} else if(dTime == 0) {
				--i;
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Read/Cache Fill/Insufficient Time/Hardware Error encountered at LBA = %u.  Restarting test...", gErrorLBA);
			} else {
				xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Sustained Data Rate (Layer 0, Q2, MS = %u) FAILED", MaxSectors);
			}
		//}
	}
	
	xStartVariation( gbConsoleOut, gLogHandle, "Layer 0, Quarter 3");
	for(USHORT i = 0; i <=6; ++i) {
		MaxSectors = 1<<i;

		//if(MaxSectors != 8 && MaxSectors != 32 ) {
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, MaxSectors = %u", (ULONG)NumSectors.QuadPart/4, ((ULONG)NumSectors.QuadPart*3/8)-1, MaxSectors);
		
			dTime = StreamTime(hDevice, (ULONG)NumSectors.QuadPart/4, ((ULONG)NumSectors.QuadPart*3/8)-1, MaxSectors);
		
			if(dTime > 0) {
				xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Sustained Data Rate (Layer 0, Q3, MS = %u) = %f Mbits/s", MaxSectors, dTime);
			} else if(dTime == 0) {
				--i;
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Read/Cache Fill/Insufficient Time/Hardware Error encountered at LBA = %u.  Restarting test...", gErrorLBA);
			} else {
				xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Sustained Data Rate (Layer 0, Q3, MS = %u) FAILED", MaxSectors);
			}
		//}
	}
	
	xStartVariation( gbConsoleOut, gLogHandle, "Layer 0, Quarter 4");
	for(USHORT i = 0; i <=6; ++i) {
		MaxSectors = 1<<i;

		//if(MaxSectors != 8 && MaxSectors != 32 ) {
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, MaxSectors = %u", (ULONG)NumSectors.QuadPart*3/8, ((ULONG)NumSectors.QuadPart/2)-1, MaxSectors);
		
			dTime = StreamTime(hDevice, (ULONG)NumSectors.QuadPart*3/8, ((ULONG)NumSectors.QuadPart/2)-1, MaxSectors);
		
			if(dTime > 0) {
				xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Sustained Data Rate (Layer 0, Q4, MS = %u) = %f Mbits/s", MaxSectors, dTime);
			} else if(dTime == 0) {
				--i;
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Read/Cache Fill/Insufficient Time/Hardware Error encountered at LBA = %u.  Restarting test...", gErrorLBA);
			} else {
				xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Sustained Data Rate (Layer 0, Q4, MS = %u) FAILED", MaxSectors);
			}
		//}
	}
	
	xStartVariation( gbConsoleOut, gLogHandle, "Layer Transition");
	for(USHORT i = 0; i <=6; ++i) {
		MaxSectors = 1<<i;

		//if(MaxSectors != 8 && MaxSectors != 32 ) {
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, MaxSectors = %u", (ULONG)NumSectors.QuadPart*7/16, ((ULONG)NumSectors.QuadPart*9/16)-1, MaxSectors);
		
			dTime = StreamTime(hDevice, (ULONG)NumSectors.QuadPart*7/16, ((ULONG)NumSectors.QuadPart*9/16)-1, MaxSectors);
		
			if(dTime > 0) {
				xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Sustained Data Rate (Layer Trans, MS = %u) = %f Mbits/s", MaxSectors, dTime);
			} else if(dTime == 0) {
				--i;
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Read/Cache Fill/Insufficient Time/Hardware Error encountered at LBA = %u.  Restarting test...", gErrorLBA);
			} else {
				xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Sustained Data Rate (Layer Trans, MS = %u) FAILED", MaxSectors);
			}
		//}
	}
	
	xStartVariation( gbConsoleOut, gLogHandle, "Layer 1, Quarter 1");
	for(USHORT i = 0; i <=6; ++i) {
		MaxSectors = 1<<i;

		//if(MaxSectors != 8 && MaxSectors != 32 ) {
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, MaxSectors = %u", (ULONG)NumSectors.QuadPart/2, ((ULONG)NumSectors.QuadPart*5/8)-1, MaxSectors);
		
			dTime = StreamTime(hDevice, (ULONG)NumSectors.QuadPart/2, ((ULONG)NumSectors.QuadPart*5/8)-1, MaxSectors);
		
			if(dTime > 0) {
				xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Sustained Data Rate (Layer 1, Q1, MS = %u) = %f Mbits/s", MaxSectors, dTime);
			} else if(dTime == 0) {
				--i;
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Read/Cache Fill/Insufficient Time/Hardware Error encountered at LBA = %u.  Restarting test...", gErrorLBA);
			} else {
				xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Sustained Data Rate (Layer 1, Q1, MS = %u) FAILED", MaxSectors);
			}
		//}
	}
	
	xStartVariation( gbConsoleOut, gLogHandle, "Layer 1, Quarter 2");
	for(USHORT i = 0; i <=6; ++i) {
		MaxSectors = 1<<i;

		//if(MaxSectors != 8 && MaxSectors != 32 ) {
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, MaxSectors = %u", (ULONG)NumSectors.QuadPart*5/8, ((ULONG)NumSectors.QuadPart*3/4)-1, MaxSectors);
		
			dTime = StreamTime(hDevice, (ULONG)NumSectors.QuadPart*5/8, ((ULONG)NumSectors.QuadPart*3/4)-1, MaxSectors);
		
			if(dTime > 0) {
				xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Sustained Data Rate (Layer 1, Q2, MS = %u) = %f Mbits/s", MaxSectors, dTime);
			} else if(dTime == 0) {
				--i;
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Read/Cache Fill/Insufficient Time/Hardware Error encountered at LBA = %u.  Restarting test...", gErrorLBA);
			} else {
				xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Sustained Data Rate (Layer 1, Q2, MS = %u) FAILED", MaxSectors);
			}
		//}
	}
	
	xStartVariation( gbConsoleOut, gLogHandle, "Layer 1, Quarter 3");
	for(USHORT i = 0; i <=6; ++i) {
		MaxSectors = 1<<i;

		//if(MaxSectors != 8 && MaxSectors != 32 ) {
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, MaxSectors = %u", (ULONG)NumSectors.QuadPart*3/4, ((ULONG)NumSectors.QuadPart*7/8)-1, MaxSectors);
		
			dTime = StreamTime(hDevice, (ULONG)NumSectors.QuadPart*3/4, ((ULONG)NumSectors.QuadPart*7/8)-1, MaxSectors);
		
			if(dTime > 0) {
				xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Sustained Data Rate (Layer 1, Q3, MS = %u) = %f Mbits/s", MaxSectors, dTime);
			} else if(dTime == 0) {
				--i;
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Read/Cache Fill/Insufficient Time/Hardware Error encountered  at LBA = %u.  Restarting test...", gErrorLBA);
			} else {
				xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Sustained Data Rate (Layer 1, Q3, MS = %u) FAILED", MaxSectors);
			}
		//}
	}
	
	xStartVariation( gbConsoleOut, gLogHandle, "Layer 1, Quarter 4");
	for(USHORT i = 0; i <=6; ++i) {
		MaxSectors = 1<<i;

		//if(MaxSectors != 8 && MaxSectors != 32 ) {
			xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Start LBA = %u, End LBA = %u, MaxSectors = %u", (ULONG)NumSectors.QuadPart*7/8, (ULONG)NumSectors.QuadPart-1, MaxSectors);
		
			dTime = StreamTime(hDevice, (ULONG)NumSectors.QuadPart*7/8, (ULONG)NumSectors.QuadPart-1, MaxSectors);
		
			if(dTime > 0) {
				xLog(gbConsoleOut, gLogHandle, XLL_PASS, "Avg Sustained Data Rate (Layer 1, Q4, MS = %u) = %f Mbits/s", MaxSectors, dTime);
			} else if(dTime == 0) {
				--i;
				xLog(gbConsoleOut, gLogHandle, XLL_INFO, "Read/Cache Fill/Insufficient Time/Hardware Error encountered at LBA = %u.  Restarting test...", gErrorLBA);
			} else {
				xLog(gbConsoleOut, gLogHandle, XLL_FAIL, "Avg Sustained Data Rate (Layer 1, Q4, MS = %u) FAILED", MaxSectors);
			}
		//}
	}
	
	// free gcBuf
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
	LogHandle = xCreateLog_W(L"t:\\dvdperftest.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("DVDPerfTest: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
		xDebugStringA("DVDPerfTest: End - Waiting for reboot...\n");
		Sleep(INFINITE);
    }
	
	gbConsoleOut = TRUE;

	// Start Test
	DVDPerfStartTest(LogHandle);

	// End Test
	DVDPerfEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("DVDPerfTest: End - Waiting for reboot...\n");
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
DECLARE_EXPORT_DIRECTORY( dvdperftest )
#pragma data_seg()

BEGIN_EXPORT_TABLE( dvdperftest )
    EXPORT_TABLE_ENTRY( "StartTest", DVDPerfStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DVDPerfEndTest )
END_EXPORT_TABLE( dvdperftest )

