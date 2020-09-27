#ifndef _XSMC_H
#define _XSMC_H

#include "..\parameter.h"

// XSMC Module Number for getting registered with the test executive
#define XSMC_MODNUM 6

#ifndef PASS
#define PASS 0
#endif
#ifndef FAIL
#define FAIL 0xFFFFFFFF
#endif

#define REG_TEMP_UTILITY_EVENT_NAME  "XMTA_REG_TEMP_UTILITY_EVENT"
#define DVD_SPIN_DOWN_UTILITY_EVENT_NAME  "XMTA_DVD_SPIN_DOWN_UTILITY_EVENT"


LPCTSTR pwsRevAllowed = _T("RevisionAllowed[%d]");
LPCTSTR pwsRevNotAllowed = _T("RevisionNotAllowed[%d]");
BOOL bErrorFlag = FALSE;
CRITICAL_SECTION SMCRegisterCriticalSection;
BOOL bSMCRegsiterCriticalSectionInitialized = false;

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

class CXModuleSMC : public CTestObj
{
public: // Get module number from numbers.h
	DECLARE_XMTAMODULE(CXModuleEval, "smc", XSMC_MODNUM);

protected:
	// Error messages can be declared here ...
	// (Note: 0x000 - 0x00f reserved for XMTA)
	DWORD err_BadParameter(LPCTSTR s1)
	{
		ReportError(0x010, (_T("Can't find configuration parameter \"%s\"\nThis configuration ")
					 _T("parameter doesn't exist or is invalid")), s1); 
		return FAIL;
	}

	DWORD err_RevisionXor()
	{
		ReportError(0x011, (_T("\"RevisionAllowed[##]\" and \"RevisionNotAllowed[##]\" configuration")
        					_T(" parameters are mutually exclusive. Use one set or the other."))); 
		return FAIL;
	}
	
	DWORD err_RevisionNotAllowed(LPCTSTR s1)
	{
		ReportError(0x12, _T("SMC Revision ID \"%s\" is not allowed."), s1);
		return FAIL;
	}

	DWORD err_Write(LPCTSTR s1, LONG l1)
	{
		ReportError(0x13, _T("Write to SMC %s Register Failed. NTSTATUS = 0x%08lX"), s1, l1);
		return FAIL;
	}

	DWORD err_Read(LPCTSTR s1, LONG l1)
	{
		ReportError(0x14, _T("Read from SMC %s Register Failed. NTSTATUS = 0x%08lX"), s1, l1);
		return FAIL;
	}

	DWORD err_Reset(LONG l1)
	{
		ReportError(0x15, _T("Unable to Reset the System. NTSTATUS = 0x%08lX"), l1);
		return FAIL;
	}

	DWORD err_HostResponseError(DWORD dwErrorCodeFromHost, LPCTSTR s1)
	{
		ReportError ((unsigned short)dwErrorCodeFromHost, _T("The host responded with the following")
					  _T(" error message:\n%s"), s1);
		return FAIL;
	}

	DWORD err_HostCommunicationError(int i1)
	{
		ReportError (0x017, _T("iSendHost communication routine returned an error code of 0x%x"), i1); 
		return FAIL;
	}

	DWORD err_BufferIsEmpty()
	{
		ReportError (0x018, _T("The buffer from the host is empty.")); 
		return FAIL;
	}

	DWORD err_LEDStateFromTIU(LPCTSTR s1)
	{
		ReportError (0x019, _T("Error in getting the LED to %s State."), s1); 
		return FAIL;
	}

	DWORD err_VModeState(LPCTSTR s1)
	{
		ReportError (0x01A, _T("Error in getting the VMode to %s State."), s1); 
		return FAIL;
	}

	DWORD err_AudioClampRelease()
	{
		ReportError (0x01B, _T("Could not release the Audio Clamp.")); 
		return FAIL;
	}

	DWORD err_AudioClamp()
	{
		ReportError (0x01C, _T("Could not clamp the Audio.")); 
		return FAIL;
	}

	DWORD err_AirSensor(int i1, BYTE b1, LPCTSTR s1, BYTE b2, LPCTSTR s2)
	{
		ReportError (0x01D, _T("Fan Speed = %d and Air Sensor = %d(%s). It should have been %d(%s)"), 
					 i1, b1, s1, b2, s2); 
		return FAIL;
	}

	DWORD err_FanVoltLimitBadParameter(LPCTSTR s1, LPCTSTR s2, LPCTSTR s3)
	{
		ReportError (0x01E, _T("Configuration Parameter \"%s\" has a(n) %s limit of %s"), s1, s2, s3); 
		return FAIL;
	}

	DWORD err_FanVoltLimit(float f1, int i1, float f2, float f3)
	{
		ReportError (0x01F, _T("Invalid Voltage of %f for Fan Speed of %d. Lower limit is %f and upper")
					 _T(" limit is %f"), f1, i1, f2, f3); 
		return FAIL;
	}

	DWORD err_CpuJunctionTemp(int i1, int i2, int i3)
	{
		ReportError (0x020, _T("Invalid CPU Junction Temp of %dC. Lower limit is %dC and Upper")
					 _T("limit is %dC"), i1, i2, i3); 
		return FAIL;
	}

	DWORD err_InternalAirTemp(int i1, int i2, int i3)
	{
		ReportError (0x021, _T("Invlaid Internal Air Temp of %dC. Lower limit is %dC and Upper")
					 _T("limit is %dC"), i1, i2, i3); 
		return FAIL;
	} 

	DWORD err_LEDResponse(LPCTSTR s1, LPCTSTR s2)
	{
		ReportError (0x022, _T("Incorrect Response. It should have been %s instead of %s"), s1, s2); 
		return FAIL;
	} 

	DWORD err_NoDVDMediaDetected()
	{
		ReportError (0x023, _T("Media was not detected in the DVD drive")); 
		return FAIL;
	} 

	DWORD err_NoDVDActivityDetected()
	{
		ReportError (0x024, _T("Did not detect a signal on the DVD Activity line")); 
		return FAIL;
	} 

	DWORD err_DVDTrayNotOpen()
	{
		ReportError (0x025, _T("DVD drive did not report a tray open status")); 
		return FAIL;
	} 

	DWORD err_NoDVDMediaNotDetected()
	{
		ReportError (0x026, _T("The DVD trays state was never closed and empty (media not present)")); 
		return FAIL;
	} 
	DWORD err_CreateEventFailed(char *c1)
	{
		ReportError (0x027, _T("Could not create an event named %S")); 
		return FAIL;
	}

	DWORD err_CpuJunctionTempHigh(int i1, int i2)
	{
		ReportError (0x028, _T("Invalid CPU Junction Temp of %dC. Temperature exceeded upper limit of %dC"), i1, i2); 
		return FAIL;
	}

	virtual bool InitializeParameters ();
//	~CXModuleSMC();

	// Four LED states that are tested
	#define Off 0
	#define Green 1
	#define Red 2
	#define Orange 3

	// Video Modes written thru TIU
	#define DISABLE  0 
	#define SDTVNTSC 1
	#define PALRFU   2
	#define SDTVPAL  3
	#define NTSCRFU  4
	#define VGA      5
	#define HDTV     6
	#define SCART    7

	// Video Modes read thru SMC
	#define SMCDISABLE  7
	#define SMCSDTVNTSC 6
	#define SMCPALRFU   5
	#define SMCSDTVPAL  4
	#define SMCNTSCRFU  3
	#define SMCVGA      2
	#define SMCHDTV     1
	#define SMCSCART    0

	// buttons for dialog box for the LEDSystemTest
	#define IDOK        1
	#define IDCANCEL    2


	// Wait for 3 minutes for the host to respond to any communications
	#define HOST_TIMEOUT 180 
	#define MAX_REVISIONS 20
	LPCTSTR pwsAllowed, pwsNotAllowed, pwsAudioClamp;	
	LPCTSTR pwsFan50LL, pwsFan50UL, pwsFan20LL, pwsFan20UL;
	float fFanSpeed50LL, fFanSpeed50UL, fFanSpeed20LL, fFanSpeed20UL;
	UINT uiCpuJunTempLL, uiCpuJunTempUL, uiInternalAirTempLL, uiInternalAirTempUL, uiCpuJunTempCritical;
	DWORD dwRegulatedCPUTemperatureFanOff, dwRegulatedCPUTemperatureFanOn;
	DWORD gdwBootCountGateValue;
	DWORD dwRegulatedTemperatureMinTimeFanSteady;
	DWORD dwRegulatedTemperatureMaxTimeFanSteady;
	DWORD dwRegulatedTemperatureFanSteadyValue;
	DWORD dwLEDSystemTestComplexity;
	DWORD dwDVDMediaDetectTimeout;
	DWORD dwLEDSystemTestTriesAllowed;
	NTSTATUS NtStatus;

public:
	// Common functions go here.  Put any functions that need 
	// to be global to the tests in the module class
	BYTE GetLEDStateFromTIUForBoardTest(int iState);
	BYTE GetLEDStateFromTIUForSystemTest(int iState, bool bVerifyQuestion);
	void PutVModeStateToTIU(int iState);
	float GetFanVoltageFromTIU();
	BYTE GetAirSensorFromTIU();
	DWORD dwLEDSystemTest(DWORD dwLEDSystemTestTriesLeft);
	void vNewSeed(UINT *pSeed);
	DWORD random(UINT *pScratch);
};

#endif	// _XSMC_H