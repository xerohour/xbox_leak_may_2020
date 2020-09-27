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


#define SERVO_PAGE_LENGTH 0x30

VOID
DVDServoPageLogSenseData(HANDLE LogHandle, SENSE_DATA SenseData) {
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
DVDServoPageStartTest(HANDLE LogHandle) {
	HANDLE hDevice;
	UCHAR ServoPage[SERVO_PAGE_LENGTH];
	SENSE_DATA SenseData;
	SCSI_PASS_THROUGH_DIRECT PassThrough;
	PCDB Cdb = (PCDB)&PassThrough.Cdb;
	BOOL bRet;
	DWORD cbBytesReturned, i;

	// Open DVD device
	// xSetFunctionName(LogHandle, "Open DVD Device");
	hDevice = CreateFile("cdrom0:",	GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, NULL);

	if(hDevice == INVALID_HANDLE_VALUE) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, "CreateFile FAILED LastError = %d", GetLastError());
		return;
	}

	ZeroMemory(ServoPage, sizeof(ServoPage));
	ZeroMemory(&SenseData, sizeof(SENSE_DATA));
	ZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));

	PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
	PassThrough.DataBuffer = ServoPage;
	PassThrough.DataTransferLength = sizeof(ServoPage);
	Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SENSE10;
	Cdb->MODE_SENSE10.Dbd = 1;
	Cdb->MODE_SENSE10.PageCode = 0x25;
	*((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) =
		(USHORT)IdexConstantUshortByteSwap(sizeof(ServoPage));

	bRet = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH_DIRECT,
							&PassThrough, sizeof(PassThrough), 
							&SenseData, sizeof(SenseData),
							&cbBytesReturned, NULL);

	if((!bRet) || (cbBytesReturned > 0)) {
		if(!bRet) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SCSIOP_MODE_SENSE10 FAILED LastError = %d", GetLastError());
		}

		if(cbBytesReturned > 0) {
			xLog(gbConsoleOut, LogHandle, XLL_FAIL, "SCSIOP_MODE_SENSE10 FAILED cbBytesReturned > 0");
			DVDServoPageLogSenseData(LogHandle, SenseData);
		}
	}

	for(i = 0; i < SERVO_PAGE_LENGTH; i++) {
		xLog(gbConsoleOut, LogHandle, XLL_INFO, "Servo Page Byte[%u] = 0x%.2x", i, ServoPage[i]);
	}

		// Close device
	if(!CloseHandle(hDevice)) {
		xLog(gbConsoleOut, LogHandle, XLL_FAIL, 
			"CloseHande(DVD Device) Error Code = %u", GetLastError());
	}
}

VOID
DVDServoPageEndTest() {
}


#if !defined(HARNESSLIB)
void __cdecl main() {
	HANDLE LogHandle;


	// Do the initialization that the harness does
	// dxconio
	xCreateConsole(NULL);
	xSetBackgroundImage(NULL);
	
	// xlog
	LogHandle = xCreateLog_W(L"t:\\dvdservopage.log",
                            NULL,
                            INVALID_SOCKET,
                            XLL_LOGDEFAULT,
                            XLO_DEBUG);
	if(LogHandle == INVALID_HANDLE_VALUE ) {
		xDebugStringA("DVDServoPage: couldn't create log (%s)\n", WinErrorSymbolicName(GetLastError()));
		xDebugStringA("DVDServoPage: End - Waiting for reboot...\n");
		Sleep(INFINITE);
    }
	
	gbConsoleOut = TRUE;

	// Start Test
	DVDServoPageStartTest(LogHandle);

	// End Test
	DVDServoPageEndTest();

	// xlog
	xCloseLog(LogHandle);
	
	// Wait forever, must reboot xbox
	// Future: add wait for game control input to go back to dash 
	xDebugStringA("DVDServoPage: End - Waiting for reboot...\n");
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
DECLARE_EXPORT_DIRECTORY( dvdservopage )
#pragma data_seg()

BEGIN_EXPORT_TABLE( dvdservopage )
    EXPORT_TABLE_ENTRY( "StartTest", DVDServoPageStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", DVDServoPageEndTest )
END_EXPORT_TABLE( dvdservopage )


