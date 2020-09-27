// xTOOL.h - manufacturing process TOOL

#if !defined(_XTOOL_H_)
#define _XTOOL_H_

// supress futile warnings about assignment operators
#pragma warning (disable:4512)
#include "..\parameter.h"
#ifndef PASS
#define PASS 0
#endif
#ifndef FAIL
#define FAIL 0xFFFFFFFF
#endif

#define XTOOL_MODNUM   13

#define TOOL_HOST_TIMEOUT 180 // Wait for 3 minutes for the host to respond to any communications

// NTSTATUS
typedef LONG NTSTATUS; 
/*lint -save -e624 */  // Don't complain about different typedefs.
typedef NTSTATUS *PNTSTATUS;
/*lint -restore */  // Resume checking for different typedefs.
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L) 
#define NTHALAPI     DECLSPEC_IMPORT

// SMBus functions.
extern "C"
{
NTHALAPI
NTSTATUS
NTAPI
HalReadSMBusValue(
	IN UCHAR SlaveAddress,
	IN UCHAR CommandCode,
	IN BOOLEAN ReadWordValue,
	OUT ULONG *DataValue
	);

#define HalReadSMBusByte(SlaveAddress, CommandCode, DataValue) \
	HalReadSMBusValue(SlaveAddress, CommandCode, FALSE, DataValue)

#define HalReadSMBusWord(SlaveAddress, CommandCode, DataValue) \
	HalReadSMBusValue(SlaveAddress, CommandCode, TRUE, DataValue)

NTHALAPI
NTSTATUS 
NTAPI
HalWriteSMBusValue(
	IN UCHAR SlaveAddress,
	IN UCHAR CommandCode,
	IN BOOLEAN WriteWordValue,
	IN ULONG DataValue
	);

#define HalWriteSMBusByte(SlaveAddress, CommandCode, DataValue) \
	HalWriteSMBusValue(SlaveAddress, CommandCode, FALSE, DataValue)

#define HalWriteSMBusWord(SlaveAddress, CommandCode, DataValue) \
	HalWriteSMBusValue(SlaveAddress, CommandCode, TRUE, DataValue)

} // end extern "C"


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




//
// Information about the current hardware that the system is running on
//
extern "C"
{
typedef struct _XBOX_HARDWARE_INFO {
    ULONG Flags;
    UCHAR GpuRevision;
    UCHAR McpRevision;
    UCHAR reserved[2];
} XBOX_HARDWARE_INFO;

extern const XBOX_HARDWARE_INFO* XboxHardwareInfo;
} // end extern "C"


//Some defines used for talking to the TIU board
#define AVMODEDISABLE	0
#define AVMODEVGA		1
#define AVMODESCART		6
#define AVMODENTSCCOMP	3
#define AVMODEHDTV		2
#define AVMODENTSCENH	4
//end TIU defines.


//-----------------------------------------------------------------------------------------------
//DISPLAY MODE DEFINES - required for auto-switching the output modes without restarting the DUT.
extern "C"
{
	extern DWORD D3D__AvInfo;
}

//End of DISPLAY MODE DEFINES
//-----------------------------------------------------------------------------------------------


// Naming of class test and module classes.
// 
// module class: CX<module>
// test class:   CX<module>Test<test>
//
// The names are important because the modules and exective are 
// all one project.  Since the classes are presented alphebetically, 
// using this naming convention makes the code easier to navigate.
//

class CXModuleTool : public CTestObj
{
public:
	DECLARE_XMTAMODULE (CXModuleTool, "tool", XTOOL_MODNUM);  //(module name, module number)

protected:
	// Error messages declared here ...
	//(Note: 0x000 - 0x00f reserved for XMTA)
	//

	// Error routines return true if the calling routine should abort, and false otherwise
	bool err_BADPARAMETER (LPCTSTR s1)
	{
		ReportError (0x010, L"Can't find configuration parameter \"%s\"\nThis configuration parameter doesn't exist or is invalid", s1); return (CheckAbort (HERE));
	}
	bool err_HostCommunicationError (int i1)
	{
		ReportError (0x011, L"iSendHost communication routine returned an error code of 0x%x", i1); return (CheckAbort (HERE)); 
	}
	bool err_RandomBufferIsEmpty ()
	{
		ReportError (0x012, L"The random number buffer from the host is empty"); return (CheckAbort (HERE)); 
	}
	bool err_HostResponseError (DWORD dwErrorCodeFromHost, LPCTSTR s1)
	{
		ReportError ((unsigned short)dwErrorCodeFromHost, L"The host responded with the following error message:\n%s", s1); return (CheckAbort (HERE));
	}
//	bool err_CouldNotAllocateRandomBuffer (DWORD dw1)
//	{
//		ReportError (0x011, L"Could not allocate a buffer to hold a random number of length %lu bytes", dw1); return (CheckAbort (HERE));
//	}
	bool err_SetDisplayMode (LPCTSTR s1)
	{
		ReportError (0x013, L"An error occured with the set_display_mode tool: %s", s1); return (CheckAbort (HERE));
	}
	bool err_D3DError (LPCTSTR s1)
	{
		ReportError (0x014, L"The following D3D function failed: %s", s1); return (CheckAbort (HERE));
	}
	bool err_NoGameRegionCode ()
	{
		ReportError (0x015, L"Could not obtain the game region code from the host computer"); return (CheckAbort (HERE)); 
	}
	bool err_GameRegionCodeTooLarge ()
	{
		ReportError (0x016, L"The game region returned by the host computer is too large"); return (CheckAbort (HERE)); 
	}
	bool err_GameRegionInvalid (LPCTSTR s1)
	{
		ReportError (0x017, L"The game region returned by the host computer \"%s\" cannot be converted to an integer", s1); return (CheckAbort (HERE)); 
	}
	bool err_NoTVRegionCode ()
	{
		ReportError (0x018, L"Could not obtain the TV region code from the host computer"); return (CheckAbort (HERE)); 
	}
	bool err_TVRegionCodeTooLarge ()
	{
		ReportError (0x019, L"The TV region returned by the host computer is too large"); return (CheckAbort (HERE)); 
	}
	bool err_TVRegionInvalid (LPCTSTR s1)
	{
		ReportError (0x01A, L"The TV region returned by the host computer \"%s\" cannot be converted to an integer", s1); return (CheckAbort (HERE)); 
	}
	bool err_NoMACAddress ()
	{
		ReportError (0x01B, L"Could not obtain the XBox MAC address from the host computer"); return (CheckAbort (HERE)); 
	}
	bool err_MACAddressTooLarge ()
	{
		ReportError (0x01C, L"The MAC Address returned by the host computer is too large"); return (CheckAbort (HERE)); 
	}
	bool err_MACAddressInvalid (LPCTSTR s1)
	{
		ReportError (0x01D, L"The MAC Address returned by the host computer \"%s\" cannot be converted to integers", s1); return (CheckAbort (HERE)); 
	}
	bool err_NoSerialNumber ()
	{
		ReportError (0x01E, L"Could not obtain the XBox serial number from the host computer"); return (CheckAbort (HERE)); 
	}
	bool err_SerialNumberTooLarge ()
	{
		ReportError (0x01F, L"The serial number returned by the host computer is too large"); return (CheckAbort (HERE)); 
	}
	bool err_SMCWrite(LPCTSTR s1, DWORD dw1)
	{
		ReportError (0x020, L"Write to SMC %s Register Failed. NTSTATUS = 0x%08lX", s1, dw1); return (CheckAbort (HERE));
	}
	bool err_SMCRead(LPCTSTR s1, DWORD dw1)
	{
		ReportError (0x021, L"Read from SMC %s Register Failed. NTSTATUS = 0x%08lX", s1, dw1); return (CheckAbort (HERE));
	}
	bool err_NoGenPerBoxDataResponse ()
	{
		ReportError (0x022, L"Could not obtain a GenPerBoxData response from the host computer"); return (CheckAbort (HERE)); 
	}
	bool err_HardDriveKeyTooLarge ()
	{
		ReportError (0x023, L"The Hard Drive Key returned by the host computer is too large"); return (CheckAbort (HERE)); 
	}
	bool err_RecoveryKeyTooLarge ()
	{
		ReportError (0x024, L"The Recovery Key returned by the host computer is too large"); return (CheckAbort (HERE)); 
	}
	bool err_OnlineKeyTooLarge ()
	{
		ReportError (0x025, L"The Online Key returned by the host computer is too large"); return (CheckAbort (HERE)); 
	}
	bool err_OutputBufferTooLarge ()
	{
		ReportError (0x026, L"The Output Buffer returned by the host computer is too large"); return (CheckAbort (HERE)); 
	}
	bool err_WritePerBadLength ()
	{
		ReportError (0x027, L"WritePerBoxData indicated that the Output Buffer data is the wrong length"); return (CheckAbort (HERE)); 
	}
	bool err_WritePerBadData ()
	{
		ReportError (0x028, L"WritePerBoxData indicated that it could not decrypt the Output Buffer data"); return (CheckAbort (HERE)); 
	}
	bool err_WritePerBadAccess ()
	{
		ReportError (0x029, L"WritePerBoxData indicated that it could not read or write the eeprom"); return (CheckAbort (HERE)); 
	}
	bool err_WritePerBadKey ()
	{
		ReportError (0x02A, L"WritePerBoxData indicated that the eeprom key is not random"); return (CheckAbort (HERE)); 
	}
	bool err_WritePerUnknown ()
	{
		ReportError (0x02B, L"WritePerBoxData indicated that an unknown error happened"); return (CheckAbort (HERE)); 
	}
	bool err_MessageTooLarge(DWORD dw1)
	{
		ReportError (0x02C, L"A message of %lu bytes to the host is larger than the message buffer size", dw1); return (CheckAbort (HERE));
	}
	bool err_VerifyPerBadData ()
	{
		ReportError (0x02D, L"VerifyPerBoxData indicated that it could not decrypt the eeprom keys"); return (CheckAbort (HERE)); 
	}
	bool err_VerifyPerBadAccess ()
	{
		ReportError (0x02E, L"VerifyPerBoxData indicated that it could not read or write the eeprom"); return (CheckAbort (HERE)); 
	}
	bool err_VerifyPerBadKey ()
	{
		ReportError (0x02F, L"VerifyPerBoxData indicated that the eeprom key is not random"); return (CheckAbort (HERE)); 
	}
	bool err_VerifyPerUnknown ()
	{
		ReportError (0x030, L"VerifyPerBoxData indicated that an unknown error happened"); return (CheckAbort (HERE)); 
	}
	bool err_NoFileReturned (LPCTSTR s1)
	{
		ReportError (0x031, L"Could not obtain the file \"%s\" from the host computer", s1); return (CheckAbort (HERE)); 
	}
	bool err_CouldNotMountUtilityDrive (DWORD dw1)
	{
		ReportError (0x032, L"Could not mount the XBox utility drive.  GetLastError returned: %lu", dw1); return (CheckAbort (HERE)); 
	}
	bool err_GPURevisionXor()
	{
		ReportError (0x033, L"\"GPURevisionAllowed[#]\" and \"GPURevisionNotAllowed[#]\" configuration parameters are mutually exclusive. Use one set or the other."); return (CheckAbort (HERE));
	}
	bool err_GPURevisionNotAllowed (DWORD dw1)
	{
		ReportError (0x034, L"GPU Revision is not allowed: 0x%2.2x.", dw1); return (CheckAbort (HERE));
	}
	bool err_MCPRevisionXor()
	{
		ReportError (0x035, L"\"MCPRevisionAllowed[#]\" and \"MCPRevisionNotAllowed[#]\" configuration parameters are mutually exclusive. Use one set or the other."); return (CheckAbort (HERE));
	}
	bool err_MCPRevisionNotAllowed (DWORD dw1)
	{
		ReportError (0x036, L"MCP Revision is not allowed: 0x%2.2x.", dw1); return (CheckAbort (HERE));
	}
	bool err_CouldNotMountDashboardPartition (DWORD dw1)
	{
		ReportError (0x037, L"Could not mount the XBox dashboard partition.  GetLastError returned: %lu", dw1); return (CheckAbort (HERE)); 
	}
	bool err_SCARTStatusNotReceivedFromHost ()
	{
		ReportError (0x038, L"Could not receive the voltage of the SCART Status line from the host computer"); return (CheckAbort (HERE)); 
	}
	bool err_SCARTStatusWrongVoltage (float f1, double dbl1, double dbl2)
	{
		ReportError (0x039, L"Actual SCART Status line voltage value (%.2lf) did not fall in the expected range (%.2lf - %.2lf)", (double)f1, dbl1, dbl2); return (CheckAbort (HERE)); 
	}
	bool err_RE_OUT_OF_MEMORY_P1 (DWORD dw1)
	{
		ReportError (0x03A, L"Out-of-memory error.  Could not allocate %d bytes of memory.", dw1); return (CheckAbort (HERE));
	}
	bool err_OPEN_FILE_P1L (char *s1)
	{
		ReportError (0x03B, L"Failed to open the file \"%S\" that is needed to perform the drive test", s1); return (CheckAbort (HERE));
	}
	bool err_BadKernelRevision (LPCTSTR s1)
	{
		ReportError (0x03C, L"The kernel revision of the XBox \"%s\" is not in the list of allowed kernels", s1); return (CheckAbort (HERE));
	}
	bool err_BadDVDRevision (LPCTSTR s1)
	{
		ReportError (0x03D, L"The DVD firmware revision of \"%s\" is not in the list of allowed DVD revisions", s1); return (CheckAbort (HERE));
	}
	bool err_NoImageFileSpecified ()
	{
		ReportError (0x03E, L"Cannot image the hard drive without a source file name"); return (CheckAbort (HERE));
	}
	bool err_RE_WRITE_LOGICAL_P3L (char *cp1, DWORD dw1, DWORDLONG dwl1)
	{
		ReportError (0x03F, L"Error writing device \"%S\".\nCould not write %lu bytes to offset %I64u.", cp1, dw1, dwl1); return (CheckAbort (HERE));
	}
	bool err_ImageFileWrongSize (char *cp1, DWORD dw1, DWORD dw2)
	{
		ReportError (0x040, L"The image file \"%S\" written to the hard drive is not the right size\nExpected the file to be %lu bytes, but is is %lu bytes instead", cp1, dw1, dw2); return (CheckAbort (HERE));
	}
	bool err_NoFileSizeReturned (LPCTSTR s1)
	{
		ReportError (0x041, L"Could not obtain the size of file \"%s\" from the host computer", s1); return (CheckAbort (HERE)); 
	}
	bool err_ImageFileZeroLength (LPCTSTR s1)
	{
		ReportError (0x042, L"The image file \"%s\" on the server is empty", s1); return (CheckAbort (HERE));
	}
	bool err_SettingFatalErrorRetryFlag ()
	{
		ReportError (0x043, L"Could not set the state of the Fatal Error Retry Flag"); return (CheckAbort (HERE));
	}
	bool err_QueryingFatalErrorCode ()
	{
		ReportError (0x044, L"Could not query the last UEM fatal error code from the XBox"); return (CheckAbort (HERE));
	}
	bool err_NamedStateBufferIsEmpty ()
	{
		ReportError (0x045, L"The named state data buffer from the host is empty"); return (CheckAbort (HERE)); 
	}
	bool err_UEMTooManyTotalErrors (DWORD dw1)
	{
		ReportError (0x046, L"There have been a total of %ld UEM errors, which is more than the permitted number", dw1); return (CheckAbort (HERE));
	}
	// Errors 0x47 through 0x66 are reserved for UEM errors
	bool err_UEMTooManyErrors (DWORD dw1, DWORD dw2, DWORD dw3, LPCTSTR s1)
	{
		ReportError ((WORD)dw1, L"There have been %ld UEM errors of type 0x%2.2lx: \"%s\", which is more than the permitted number", dw2, dw3, s1); return (CheckAbort (HERE));
	}
	bool err_ClearFatalErrorHistory ()
	{
		ReportError (0x067, L"Could not clear the UEM fatal error history"); return (CheckAbort (HERE));
	}
	bool err_NoEepromSerialNumber ()
	{
		ReportError (0x068, L"Could not read the XBox serial number from the eeprom"); return (CheckAbort (HERE)); 
	}
	bool err_SerialNumberMismatch (char *c1, char *c2)
	{
		ReportError (0x069, L"The serial number obtained from the host, %S, does not match the number read from the eeprom, %S", c1, c2); return (CheckAbort (HERE)); 
	}
	bool err_NoEepromMACAddress ()
	{
		ReportError (0x06A, L"Could not read the XBox MAC address from the eeprom"); return (CheckAbort (HERE)); 
	}
	bool err_MACAddressMismatch (DWORD dw1, DWORD dw2, DWORD dw3, DWORD dw4)
	{
		ReportError (0x06B, L"The MAC address obtained from the host, %4.4lx%8.8lx, does not match the number read from the eeprom, %4.4lx%8.8lx", dw1, dw2, dw3, dw4); return (CheckAbort (HERE)); 
	}

	
	// Parameters declared here ...
	//
	DWORD dwRandomNumberLength;
//	int m_cfgInt;
//	UINT m_cfgUint;
//	LPCTSTR m_cfgString;
//	int m_cfgChoice;

	int m_vidMode;//The display mode (resoulution) to be used.
	LPCTSTR m_pack;
	LPCTSTR m_standard;
	LPCTSTR	m_HDTVmode;
	LPCTSTR	m_TIUStatus;
	int m_flagWidescreen;
	int m_flagRefreshRate;
	BYTE m_AVMode;

	virtual bool InitializeParameters();
	
	bool InitializeDisplaySettings(int mode);
	bool InitializeInterfaceBoard(BYTE avMode);
	//Output some dubug messages to give visibility of flag settings.
	void ReportVideoSettings();
	bool bMountUtilityDrive();
	bool bMountDashboardPartition();
	void vNewSeed(UINT *pSeed);
	DWORD random(UINT *pScratch);

public:
	// Common functions go here.  Put any functions that need 
	// to be global to the tests in the module class
	//
	void HelperFunction (); // delete this
};

#endif // _XTOOL_H_
