/*
#include <ntos.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntddcdvd.h>
#include <ntddscsi.h>
#include <scsi.h>
#include <dvdx2.h>
#include <idex.h>
#include <align.h>
#include <stdio.h>
*/

#include <idex.h>
#include <rsa_sys.h>
#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <dxconio.h>
#include <xlogconio.h>

// structs
typedef struct _SelectFlags {
	UCHAR CDFValidFlag;
	UCHAR AuthenticationFlag;
	UCHAR PartitionFlag;
} SelectFlags;


VOID
IdexCdRomDecryptHostChallengeResponseTable(
    IN PDVDX2_CONTROL_DATA ControlData
    )
/*++

Routine Description:

    This routine decrypts the host challenge response table of the supplied
    DVD-X2 control data structure.

Arguments:

    ControlData - Specifies the control data structure that contains the host
        challenge response table to be decrypted.

Return Value:

    None.

--*/
{
    A_SHA_CTX SHAHash;
    UCHAR SHADigest[A_SHA_DIGEST_LEN];
    struct RC4_KEYSTRUCT RC4KeyStruct;

    //
    // Compute the SHA-1 hash of the data between bytes 1183 to 1226 of the
    // control data structure.
    //

    A_SHAInit(&SHAHash);
    A_SHAUpdate(&SHAHash, (PUCHAR)&ControlData->AuthoringTimeStamp,
        FIELD_OFFSET(DVDX2_CONTROL_DATA, AuthoringHash) -
        FIELD_OFFSET(DVDX2_CONTROL_DATA, AuthoringTimeStamp));
    A_SHAFinal(&SHAHash, SHADigest);

    //
    // Compute a 56-bit RC4 session key from the SHA-1 hash.
    //

    rc4_key(&RC4KeyStruct, 56 / 8, SHADigest);

    //
    // Decrypt the host challenge response table in place using the RC4 session
    // key.
    //

    rc4(&RC4KeyStruct, sizeof(ControlData->HostChallengeResponseTable.Entries),
        (PUCHAR)&ControlData->HostChallengeResponseTable.Entries);
}


VOID
DVDAuthLogSenseData(HANDLE LogHandle, SENSE_DATA SenseData) {
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
LogAuthentication(HANDLE LogHandle, DVDX2_AUTHENTICATION Authentication) {
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Authentication ModeDataLength[0]: %#x", Authentication.Header.ModeDataLength[0]);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Authentication ModeDataLength[1]: %#x", Authentication.Header.ModeDataLength[1]);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Authentication Page Code: %#x", Authentication.AuthenticationPage.PageCode);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Authentication Page Length: %#x", Authentication.AuthenticationPage.PageLength);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Authentication PartitionArea: %#x", Authentication.AuthenticationPage.PartitionArea);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Authentication CDFValid: %#x", Authentication.AuthenticationPage.CDFValid);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Authentication Authentication: %#x", Authentication.AuthenticationPage.Authentication);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Authentication DiscCategoryAndVersion: %#x", Authentication.AuthenticationPage.DiscCategoryAndVersion);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Authentication DrivePhaseLevel: %#x", Authentication.AuthenticationPage.DrivePhaseLevel);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Authentication ChallengeID: %#x", Authentication.AuthenticationPage.ChallengeID);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Authentication ChallengeValue: %#x", Authentication.AuthenticationPage.ChallengeValue);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Authentication ResponseValue: %#x", Authentication.AuthenticationPage.ResponseValue);
}

VOID
LogLayerDescriptor(HANDLE LogHandle, DVD_LAYER_DESCRIPTOR LayerDescriptor) {
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "LayerDescriptor BookVersion: %#x", LayerDescriptor.BookVersion);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "LayerDescriptor BookType: %#x", LayerDescriptor.BookType);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "LayerDescriptor MinimumRate: %#x", LayerDescriptor.MinimumRate);
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "LayerDescriptor DiskSize: %#x", LayerDescriptor.DiskSize);
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "LayerDescriptor LayerType: %#x", LayerDescriptor.LayerType);
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "LayerDescriptor TrackPath: %#x", LayerDescriptor.TrackPath);
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "LayerDescriptor NumberOfLayers: %#x", LayerDescriptor.NumberOfLayers);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "LayerDescriptor LinearDensity: %#x", LayerDescriptor.LinearDensity);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "LayerDescriptor TrackDensity: %#x", LayerDescriptor.TrackDensity);
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "LayerDescriptor StartingDataSector: %#x", LayerDescriptor.StartingDataSector);
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "LayerDescriptor EndDataSector: %#x", LayerDescriptor.EndDataSector);
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "LayerDescriptor EndLayerZeroSector: %#x", LayerDescriptor.EndLayerZeroSector);
    xLog(gbConsoleOut, LogHandle, XLL_INFO, "LayerDescriptor BCAFlag: %#x", LayerDescriptor.BCAFlag);
}

VOID
LogResponseTable(HANDLE LogHandle, DVDX2_HOST_CHALLENGE_RESPONSE_TABLE ResponseTable) {
	UCHAR i;

	xLog(gbConsoleOut, LogHandle, XLL_INFO, "ResponseTable Version: %#x", ResponseTable.Version);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "ResponseTable NumberOfEntries: %#u", ResponseTable.NumberOfEntries);
	
	for(i = 0; i < ResponseTable.NumberOfEntries; i++) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "ResponseTable Entry[%u] ChallengeLevel: %#x", i, ResponseTable.Entries[i].ChallengeLevel);
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "ResponseTable Entry[%u] ChallengeID: %#x", i, ResponseTable.Entries[i].ChallengeID);
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "ResponseTable Entry[%u] ChallengeValue: %#x", i, ResponseTable.Entries[i].ChallengeValue);
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "ResponseTable Entry[%u] ResponseModifier: %#x", i, ResponseTable.Entries[i].ResponseModifier);
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "ResponseTable Entry[%u] ResponseValue: %#x", i, ResponseTable.Entries[i].ResponseValue);
	}
}

VOID
LogCDB(HANDLE LogHandle, PCDB Cdb) {
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Cdb OperationCode: %#x", Cdb->MODE_SENSE10.OperationCode);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Cdb PageCode: %#x", Cdb->MODE_SENSE10.PageCode);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Cdb AllocationLength[0]: %#x", Cdb->MODE_SENSE10.AllocationLength[0]);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Cdb AllocationLength[1]: %#x", Cdb->MODE_SENSE10.AllocationLength[1]);
}
	

VOID
WINAPI
DVDAuthStartTest(HANDLE LogHandle) {
	HANDLE hDevice;
	BOOL bReturn, bPass;
    DWORD cbBytesReturned;
	UCHAR DrivePhaseLevel, ChallengeAttempt, ChallengePass, LastTableEntry, i;
	SelectFlags FirstChallenge, OtherChallenge, LastChallenge;
	SENSE_DATA SenseData;
	DISK_GEOMETRY DiskGeometryBefore, DiskGeometryAfter;

    SCSI_PASS_THROUGH_DIRECT PassThrough;
    PCDB Cdb = (PCDB)&PassThrough.Cdb;
    DVDX2_AUTHENTICATION AuthChallenge, AuthResponse;
    DVDX2_CONTROL_DATA ControlData;

	// Set global log handle
	LogHandle = LogHandle;

    xSetOwnerAlias(LogHandle, "a-emebac");
	xSetComponent(LogHandle, "Hardware", "DVDAuth");
    
	// Get config from testini.ini
	xSetFunctionName(LogHandle, "Configuration");

	// Flags for First MODE_SELECT
	FirstChallenge.CDFValidFlag = (UCHAR)GetProfileInt(TEXT("dvdauth"), TEXT("FirstCDFValidFlag"), 1);
	FirstChallenge.AuthenticationFlag = (UCHAR)GetProfileInt(TEXT("dvdauth"), TEXT("FirstAuthenticationFlag"), 0);
	FirstChallenge.PartitionFlag = (UCHAR)GetProfileInt(TEXT("dvdauth"), TEXT("FirstPartitionFlag"), 0);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "First MODE_SELECT: CDFValid = %#x Authentication = %#x Partition = %#x", 
		FirstChallenge.CDFValidFlag, FirstChallenge.AuthenticationFlag, FirstChallenge.PartitionFlag);

	// Flags for Other MODE_SELECT
	OtherChallenge.CDFValidFlag = (UCHAR)GetProfileInt(TEXT("dvdauth"), TEXT("OtherCDFValidFlag"), 1);
	OtherChallenge.AuthenticationFlag = (UCHAR)GetProfileInt(TEXT("dvdauth"), TEXT("OtherAuthenticationFlag"), 1);
	OtherChallenge.PartitionFlag = (UCHAR)GetProfileInt(TEXT("dvdauth"), TEXT("OtherPartitionFlag"), 0);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Other MODE_SELECT: CDFValid = %#x Authentication = %#x Partition = %#x", 
		OtherChallenge.CDFValidFlag, OtherChallenge.AuthenticationFlag, OtherChallenge.PartitionFlag);

	// Flags for Last MODE_SELECT
	LastChallenge.CDFValidFlag = (UCHAR)GetProfileInt(TEXT("dvdauth"), TEXT("LastCDFValidFlag"), 1);
	LastChallenge.AuthenticationFlag = (UCHAR)GetProfileInt(TEXT("dvdauth"), TEXT("LastAuthenticationFlag"), 1);
	LastChallenge.PartitionFlag = (UCHAR)GetProfileInt(TEXT("dvdauth"), TEXT("LastPartitionFlag"), 1);
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Last MODE_SELECT: CDFValid = %#x Authentication = %#x Partition = %#x", 
		LastChallenge.CDFValidFlag, LastChallenge.AuthenticationFlag, LastChallenge.PartitionFlag);


	// open device
	xSetFunctionName(LogHandle, "Open Device");
	hDevice = CreateFile("cdrom0:",	GENERIC_READ, 0, NULL, OPEN_EXISTING, 
							FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING,NULL);

	if(hDevice == INVALID_HANDLE_VALUE) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "CreateFile FAILED LastError = %d", GetLastError());
		return;
	}

	// Get Before Authentication Geometry
	xSetFunctionName(LogHandle, "Disk Geometry Before Auth");
	RtlZeroMemory(&DiskGeometryBefore, sizeof(DiskGeometryBefore));
	bReturn = DeviceIoControl(hDevice, IOCTL_CDROM_GET_DRIVE_GEOMETRY, 
								NULL, 0, 
								&DiskGeometryBefore, sizeof(DiskGeometryBefore), 
								&cbBytesReturned, 
								NULL);

	if(!bReturn) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "IOCTL_CDROM_GET_DRIVE_GEOMETRY FAILED LastError = %d", GetLastError());
	} else {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Cylinders = %u", DiskGeometryBefore.Cylinders.QuadPart);
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "TracksPerCylinder = %u", DiskGeometryBefore.TracksPerCylinder);
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "SectorsPerTrack = %u", DiskGeometryBefore.SectorsPerTrack);
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "BytesPerSector = %u", DiskGeometryBefore.BytesPerSector);
	}

    //
    // Request the authentication page from the DVD-X2 drive.
    //
	xSetFunctionName(LogHandle, "Initial CDFValid Check");
	RtlZeroMemory(&SenseData, sizeof(SENSE_DATA));
	RtlZeroMemory(&AuthChallenge, sizeof(DVDX2_AUTHENTICATION));
    RtlZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));
 
	PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
    PassThrough.DataBuffer = &AuthChallenge;
    PassThrough.DataTransferLength = sizeof(DVDX2_AUTHENTICATION);
    Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SENSE10;
    Cdb->MODE_SENSE10.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
    *((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) =
        (USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION));

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
			DVDAuthLogSenseData(LogHandle, SenseData);
		}
		return;
	}

	LogAuthentication(LogHandle, AuthChallenge);
	DrivePhaseLevel = AuthChallenge.AuthenticationPage.DrivePhaseLevel;

	// Check for valid CDF header
	if(AuthChallenge.AuthenticationPage.CDFValid != DVDX2_CDF_VALID) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "CDF Valid FAILED (Unrecognized Media)");
	} else {
		xLog(gbConsoleOut, LogHandle, XLL_PASS, "CDF Valid PASSED (Media Recognized)");
	}


    //
    // Request the control data structure from the DVD-X2 drive.
    //
	xSetFunctionName(LogHandle, "Read DVD Structure");		
	RtlZeroMemory(&SenseData, sizeof(SENSE_DATA));
	RtlZeroMemory(&ControlData, sizeof(DVDX2_CONTROL_DATA));

    RtlZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));
    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
    PassThrough.DataBuffer = &ControlData;
    PassThrough.DataTransferLength = sizeof(DVDX2_CONTROL_DATA);
    Cdb->READ_DVD_STRUCTURE.OperationCode = SCSIOP_READ_DVD_STRUCTURE;
    *((PUSHORT)&Cdb->READ_DVD_STRUCTURE.AllocationLength) =
        (USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_CONTROL_DATA));
    *((PULONG)Cdb->READ_DVD_STRUCTURE.RMDBlockNumber) =
        IdexConstantUlongByteSwap((ULONG)(~DVDX2_CONTROL_DATA_BLOCK_NUMBER));
    Cdb->READ_DVD_STRUCTURE.LayerNumber = (UCHAR)(~DVDX2_CONTROL_DATA_LAYER);
    Cdb->READ_DVD_STRUCTURE.Format = DvdPhysicalDescriptor;
    Cdb->READ_DVD_STRUCTURE.Control = 0xC0;

	bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
								&PassThrough, sizeof(PassThrough), 
								&SenseData, sizeof(SenseData),
								&cbBytesReturned, NULL);

	if((!bReturn) || (cbBytesReturned > 0)) {
		if(!bReturn) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SCSIOP_READ_DVD_STRUCTURE FAILED LastError = %d", GetLastError());
		}

		if(cbBytesReturned > 0) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SCSIOP_READ_DVD_STRUCTURE FAILED cbBytesReturned > 0");
			DVDAuthLogSenseData(LogHandle, SenseData);
		}
	} else {
		xLog(gbConsoleOut, LogHandle, XLL_PASS, "SCSIOP_READ_DVD_STRUCTURE PASSED");
	}

	LogLayerDescriptor(LogHandle, ControlData.LayerDescriptor);
	
	// xLog(gbConsoleOut, LogHandle, XLL_INFO, "Before Decryption");
	// LogResponseTable(LogHandle, ControlData.HostChallengeResponseTable);
	
	IdexCdRomDecryptHostChallengeResponseTable(&ControlData);

	xLog(gbConsoleOut, LogHandle, XLL_INFO, "After Decryption");
	LogResponseTable(LogHandle, ControlData.HostChallengeResponseTable);
	
	//
	// Do all challenge response pairs
	//
	ChallengeAttempt = 0;
	ChallengePass = 0;
	LastTableEntry = 0;
	xSetFunctionName(LogHandle, "Challenge/Response");
	for(i = 0; i < ControlData.HostChallengeResponseTable.NumberOfEntries; i++) {
		// Only do Challenge/Response if ChallengeLevel == DrivePhaseLeve
		if(ControlData.HostChallengeResponseTable.Entries[i].ChallengeLevel == DrivePhaseLevel) {
			// Count number of challenges
			ChallengeAttempt++;
			LastTableEntry = i;
			bPass = TRUE;

			// Challenge to drive
			RtlZeroMemory(&SenseData, sizeof(SENSE_DATA));
			RtlZeroMemory(&AuthChallenge, sizeof(DVDX2_AUTHENTICATION));
			*((PUSHORT)&AuthChallenge.Header.ModeDataLength) = 
				IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION) - FIELD_OFFSET(DVDX2_AUTHENTICATION, Header.MediumType));
			AuthChallenge.AuthenticationPage.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
			AuthChallenge.AuthenticationPage.PageLength = sizeof(DVDX2_AUTHENTICATION_PAGE) -
				FIELD_OFFSET(DVDX2_AUTHENTICATION_PAGE, PartitionArea);

			if(ChallengeAttempt == 1) {
				// First Challenge
				AuthChallenge.AuthenticationPage.PartitionArea = FirstChallenge.PartitionFlag;
				AuthChallenge.AuthenticationPage.CDFValid = FirstChallenge.CDFValidFlag;
				AuthChallenge.AuthenticationPage.Authentication = FirstChallenge.AuthenticationFlag;
			} else {
				// Other Challenges
				AuthChallenge.AuthenticationPage.PartitionArea = OtherChallenge.PartitionFlag;
				AuthChallenge.AuthenticationPage.CDFValid = OtherChallenge.CDFValidFlag;
				AuthChallenge.AuthenticationPage.Authentication = OtherChallenge.AuthenticationFlag;
			}
			
			AuthChallenge.AuthenticationPage.DiscCategoryAndVersion =
				(ControlData.LayerDescriptor.BookType << 4) + ControlData.LayerDescriptor.BookVersion;
			AuthChallenge.AuthenticationPage.DrivePhaseLevel = DrivePhaseLevel;
			AuthChallenge.AuthenticationPage.ChallengeID = ControlData.HostChallengeResponseTable.Entries[i].ChallengeID;
			AuthChallenge.AuthenticationPage.ChallengeValue = ControlData.HostChallengeResponseTable.Entries[i].ChallengeValue;
			
			RtlZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));
			PassThrough.DataIn = SCSI_IOCTL_DATA_OUT;
			PassThrough.DataBuffer = &AuthChallenge;
			PassThrough.DataTransferLength = sizeof(DVDX2_AUTHENTICATION);
			PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
			Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SELECT10;
			// Cdb->MODE_SENSE10.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
			*((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) =
				(USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION));
  
			xLog(gbConsoleOut, LogHandle, XLL_INFO, "Challenge[%u] Before (MODE_SELECT)", i);
			LogCDB(LogHandle, Cdb);
			LogAuthentication(LogHandle, AuthChallenge);
			
			bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
										&PassThrough, sizeof(PassThrough), 
										&SenseData, sizeof(SenseData),
										&cbBytesReturned, NULL);

			if((!bReturn) || (cbBytesReturned > 0)) {
				bPass = FALSE;
				if(!bReturn) {
					xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Challenge[%u] FAILED LastError = %d", i, GetLastError());
				}

				if(cbBytesReturned > 0) {
					xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Challenge[%u] FAILED cbBytesReturned > 0", i);
					DVDAuthLogSenseData(LogHandle, SenseData);
				}
			}

			// xLog(gbConsoleOut, LogHandle, XLL_INFO, "Challenge[%u] After (MODE_SELECT)", i);
			// LogAuthentication(LogHandle, AuthChallenge);

			// Recieve response from drive
			RtlZeroMemory(&SenseData, sizeof(SENSE_DATA));
			RtlZeroMemory(&AuthResponse, sizeof(DVDX2_AUTHENTICATION));
			RtlZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));
			PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
			PassThrough.DataBuffer = &AuthResponse;
			PassThrough.DataTransferLength = sizeof(DVDX2_AUTHENTICATION);
			PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
			Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SENSE10;
			Cdb->MODE_SENSE10.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
			*((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) = (USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION));

			// xLog(gbConsoleOut, LogHandle, XLL_INFO, "Response[%u] Before (MODE_SENSE)", i);
			// LogCDB(LogHandle, Cdb);
			// LogAuthentication(LogHandle, AuthResponse);

			bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
										&PassThrough, sizeof(PassThrough), 
										&SenseData, sizeof(SenseData),
										&cbBytesReturned, NULL);

			if((!bReturn) || (cbBytesReturned > 0)) {
				bPass = FALSE;
				if(!bReturn) {
					xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Response[%u] FAILED LastError = %d", i, GetLastError());
				}

				if(cbBytesReturned > 0) {
					xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Response[%u] FAILED cbBytesReturned > 0", i);
					DVDAuthLogSenseData(LogHandle, SenseData);
				}
			} else {
				xLog(gbConsoleOut, LogHandle, XLL_INFO, "Response[%u] (After MODE_SENSE)", i);
				LogAuthentication(LogHandle, AuthResponse);

				if(AuthResponse.AuthenticationPage.ResponseValue != ControlData.HostChallengeResponseTable.Entries[i].ResponseValue) {
					bPass = FALSE;
					xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Response[%u] FAILED ResponseValue", i);
				} else {
					xLog(gbConsoleOut, LogHandle, XLL_PASS, "Response[%u] PASSED ResponseValue", i);
				}

				if(AuthResponse.AuthenticationPage.CDFValid != DVDX2_CDF_VALID) {
					bPass = FALSE;
					xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Response[%u] FAILED CDFValid not 1", i);
				} else {
					xLog(gbConsoleOut, LogHandle, XLL_PASS, "Response[%u] PASSED CDFValid", i);
				}

				if(AuthResponse.AuthenticationPage.Authentication != 1) {
					bPass = FALSE;
					xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Response[%u] FAILED Authentication not 1", i);
				} else {
					xLog(gbConsoleOut, LogHandle, XLL_PASS, "Response[%u] PASSED Authentication", i);
				}

				if(AuthResponse.AuthenticationPage.PartitionArea != 0) {
					bPass = FALSE;
					xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Response[%u] FAILED PartitionArea not 0", i);
				} else {
					xLog(gbConsoleOut, LogHandle, XLL_PASS, "Response[%u] PASSED PartitionArea", i);
				}
			}

			// Count successful challenges
			if(bPass) {
				ChallengePass++;
			}
		}
	}

	if((ChallengeAttempt > 0) && (ChallengeAttempt == ChallengePass)) {
		xLog(gbConsoleOut, LogHandle, XLL_PASS, "Challenge/Response PASSED TableEntries=%u Challenges Issued=%u Challenges Passed=%u", 
			ControlData.HostChallengeResponseTable.NumberOfEntries, ChallengeAttempt, ChallengePass);
	} else {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Challenge/Response FAILED TableEntries=%u Challenges Issued=%u Challenges Passed=%u", 
			ControlData.HostChallengeResponseTable.NumberOfEntries, ChallengeAttempt, ChallengePass);
	}

	// Switch to XBox Partition
	xSetFunctionName(LogHandle, "Switch to XBox Partition");
	RtlZeroMemory(&SenseData, sizeof(SENSE_DATA));
	RtlZeroMemory(&AuthChallenge, sizeof(DVDX2_AUTHENTICATION));
	*((PUSHORT)&AuthChallenge.Header.ModeDataLength) = 
		IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION) - FIELD_OFFSET(DVDX2_AUTHENTICATION, Header.MediumType));
	AuthChallenge.AuthenticationPage.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
	AuthChallenge.AuthenticationPage.PageLength = sizeof(DVDX2_AUTHENTICATION_PAGE) -
		FIELD_OFFSET(DVDX2_AUTHENTICATION_PAGE, PartitionArea);
	AuthChallenge.AuthenticationPage.PartitionArea = LastChallenge.PartitionFlag;
	AuthChallenge.AuthenticationPage.CDFValid = LastChallenge.CDFValidFlag;
	AuthChallenge.AuthenticationPage.Authentication = LastChallenge.AuthenticationFlag;
	AuthChallenge.AuthenticationPage.DiscCategoryAndVersion =
		(ControlData.LayerDescriptor.BookType << 4) + ControlData.LayerDescriptor.BookVersion;
	AuthChallenge.AuthenticationPage.DrivePhaseLevel = DrivePhaseLevel;
	AuthChallenge.AuthenticationPage.ChallengeID = ControlData.HostChallengeResponseTable.Entries[LastTableEntry].ChallengeID;
	AuthChallenge.AuthenticationPage.ChallengeValue = ControlData.HostChallengeResponseTable.Entries[LastTableEntry].ChallengeValue;
    			
	RtlZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));
	PassThrough.DataIn = SCSI_IOCTL_DATA_OUT;
	PassThrough.DataBuffer = &AuthChallenge;
	PassThrough.DataTransferLength = sizeof(DVDX2_AUTHENTICATION);
	PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SELECT10;
	Cdb->MODE_SENSE10.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
	*((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) =
		(USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION));
    
	xLog(gbConsoleOut, LogHandle, XLL_INFO, "Last Challenge (Before MODE_SELECT)");
	LogCDB(LogHandle, Cdb);
	LogAuthentication(LogHandle, AuthChallenge);
		
	bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
								&PassThrough, sizeof(PassThrough), 
								&SenseData, sizeof(SenseData),
								&cbBytesReturned, NULL);

	if((!bReturn) || (cbBytesReturned > 0)) {
		bPass = FALSE;
		if(!bReturn) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Last Challenge FAILED LastError = %d", GetLastError());
		}

		if(cbBytesReturned > 0) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Last Challenge FAILED cbBytesReturned > 0");
			DVDAuthLogSenseData(LogHandle, SenseData);
		}
	}

	// Recieve response from drive
	RtlZeroMemory(&SenseData, sizeof(SENSE_DATA));
	RtlZeroMemory(&AuthResponse, sizeof(DVDX2_AUTHENTICATION));
	RtlZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));
	PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
	PassThrough.DataBuffer = &AuthResponse;
	PassThrough.DataTransferLength = sizeof(DVDX2_AUTHENTICATION);
	PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SENSE10;
	Cdb->MODE_SENSE10.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
	*((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) = (USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION));

	bReturn = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
								&PassThrough, sizeof(PassThrough), 
								&SenseData, sizeof(SenseData),
								&cbBytesReturned, NULL);

	if((!bReturn) || (cbBytesReturned > 0)) {
		if(!bReturn) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Last Response FAILED LastError = %d", GetLastError());
		}

		if(cbBytesReturned > 0) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Last Response FAILED cbBytesReturned > 0");
			DVDAuthLogSenseData(LogHandle, SenseData);
		}
	} else {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Last Response (MODE_SENSE)");
		LogAuthentication(LogHandle, AuthResponse);

		if(AuthResponse.AuthenticationPage.ResponseValue != ControlData.HostChallengeResponseTable.Entries[LastTableEntry].ResponseValue) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Last Response FAILED ResponseValue");
		} else {
			xLog(gbConsoleOut, LogHandle, XLL_PASS, "Last Response PASSED ResponseValue");
		}

		if(AuthResponse.AuthenticationPage.CDFValid != DVDX2_CDF_VALID) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Last Response FAILED CDFValid not 1");
		} else {
			xLog(gbConsoleOut, LogHandle, XLL_PASS, "Last Response PASSED CDFValid");
		}

		if(AuthResponse.AuthenticationPage.Authentication != 1) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Last Response FAILED Authentication not 1");
		} else {
			xLog(gbConsoleOut, LogHandle, XLL_PASS, "Last Response PASSED Authentication");
		}

		if(AuthResponse.AuthenticationPage.PartitionArea != 1) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Last Response FAILED PartitionArea not 1");
		} else {
			xLog(gbConsoleOut, LogHandle, XLL_PASS, "Last Response PASSED PartitionArea");
		}
	}

	// Get Disk Geometry After Authentication
	xSetFunctionName(LogHandle, "Disk Geometry After Auth");
	RtlZeroMemory(&DiskGeometryAfter, sizeof(DiskGeometryAfter));
	bReturn = DeviceIoControl(hDevice, IOCTL_CDROM_GET_DRIVE_GEOMETRY, 
								NULL, 0, 
								&DiskGeometryAfter, sizeof(DiskGeometryAfter), 
								&cbBytesReturned, 
								NULL);

	if(!bReturn) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "IOCTL_CDROM_GET_DRIVE_GEOMETRY FAILED LastError = %d", GetLastError());
	} else {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Cylinders = %u", DiskGeometryAfter.Cylinders.QuadPart);
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "TracksPerCylinder = %u", DiskGeometryAfter.TracksPerCylinder);
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "SectorsPerTrack = %u", DiskGeometryAfter.SectorsPerTrack);
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "BytesPerSector = %u", DiskGeometryAfter.BytesPerSector);
	}

	// Check for increased size in partition 1
	if(DiskGeometryAfter.Cylinders.QuadPart <= DiskGeometryBefore.Cylinders.QuadPart) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "Partion after authentication is NOT larger than before");
	} else {
		xLog(gbConsoleOut, LogHandle, XLL_PASS, "Partion after authentication is larger than before");
	}

	// close device
	if(!CloseHandle(hDevice)) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "CloseHandle FAILED LastError = %d", GetLastError());
	}
}


VOID WINAPI
DVDAuthEndTest() {
}


#if !defined(HARNESSLIB)
void __cdecl main() {
	HANDLE LogHandle;


	// Do the initialization that the harness does
	// dxconio
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);
	
	// xlog
	LogHandle = xCreateLog_W(L"t:\\dvdauth.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGALL,
                            XLO_REFRESH | XLO_STATE | XLO_CONFIG | XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("DVDAuth: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
		xDebugStringA("DVDAuth: End - Waiting for reboot...\n");
		Sleep(INFINITE);
    }
	
	gbConsoleOut = TRUE;

	// Copy ini to t:\\ if not already there

	// Start Test
	DVDAuthStartTest(LogHandle);

	// End Test
	DVDAuthEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("DVDAuth: End - Waiting for reboot...\n");
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
DECLARE_EXPORT_DIRECTORY( dvdauth )
#pragma data_seg()

BEGIN_EXPORT_TABLE( dvdauth )
    EXPORT_TABLE_ENTRY( "StartTest", DVDAuthStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DVDAuthEndTest )
END_EXPORT_TABLE( dvdauth )


