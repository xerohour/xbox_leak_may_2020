// (c) Copyright 2000-2001 Intel Corp. All rights reserved.
//
#include "..\stdafx.h"
#include "..\testobj.h"
#include "..\statkeys.h"
#include "xsmc.h"


//////////////////////////////////////////////////////////////////////////////////
// Function name	: InitializeParameters
// Description	    : Tries to see if the configuration parameters were defined
//					  or not.
// Return type		: bool 
//////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_MODULEUNLOCKED (CXModuleSMC); 

bool CXModuleSMC::InitializeParameters ()
{
	#define DVD_TIMEOUT_BETWEEN_OPERATIONS 25000  // Maximum number of milliseconds to wait between each DVD state

	TCHAR chBuf[80];

	if (!CTestObj::InitializeParameters ())
		return false;

	if (!bSMCRegsiterCriticalSectionInitialized)
	{
		bSMCRegsiterCriticalSectionInitialized = true;
		InitializeCriticalSection(&SMCRegisterCriticalSection);
	}

	// Get a list of Allowed Revision Ids. Default is Null.
	_stprintf(chBuf, pwsRevAllowed, 0);
	pwsAllowed = GetCfgString(chBuf, NULL);

	// Get a list of Not Allowed Revision Ids. Default is Null.
	_stprintf(chBuf, pwsRevNotAllowed, 0);
	pwsNotAllowed = GetCfgString(chBuf, NULL);

	// Get the AudioClamp (Yes/No) Parameter. 
	pwsAudioClamp = GetCfgString(_T("AudioClamp"), NULL);

	// Get the FanSpeedxLL and FanSpeedxUL parameters for the lower and upper limits of
	// the fan voltages, where x = 50, 20 or any other fan speed for which the voltage
	// is verified.
	// Get the param as string and then convert it to double/float.
	pwsFan50LL = GetCfgString(_T("FanSpeed50LL"), _T("0.0"));
	fFanSpeed50LL = (float)wcstod(pwsFan50LL, NULL);
	
	pwsFan50UL = GetCfgString(_T("FanSpeed50UL"), _T("0.2"));
	fFanSpeed50UL = (float)wcstod(pwsFan50UL, NULL);
	
	pwsFan20LL = GetCfgString(_T("FanSpeed20LL"), _T("3.6"));
	fFanSpeed20LL = (float)wcstod(pwsFan20LL, NULL);
	
	pwsFan20UL = GetCfgString(_T("FanSpeed20UL"), _T("3.8"));
	fFanSpeed20UL = (float)wcstod(pwsFan20UL, NULL);

	// Get the cfg parameters for the lower and upper limits of CPU Junction and Internal
	// Air temperatures.
	uiCpuJunTempLL = GetCfgUint(_T("CpuJunctionTempLL"), 0);
	uiCpuJunTempUL = GetCfgUint(_T("CpuJunctionTempUL"), 0);
	uiInternalAirTempLL = GetCfgUint(_T("InternalAirTempLL"), 0);
	uiInternalAirTempUL = GetCfgUint(_T("InternalAirTempUL"), 0);
	uiCpuJunTempCritical = GetCfgUint(_T("CpuJunctionTempCritical"), 100);

	dwRegulatedCPUTemperatureFanOff = (DWORD)GetCfgUint(_T("RegulatedCPUTemperatureFanOff"), 0);
	dwRegulatedCPUTemperatureFanOn = (DWORD)GetCfgUint(_T("RegulatedCPUTemperatureFanOn"), 100);

	dwRegulatedTemperatureMinTimeFanSteady = (DWORD)GetCfgUint(_T("RegulatedTemperatureMinTimeFanSteady"), 0xFFFFFFFF);
	dwRegulatedTemperatureMaxTimeFanSteady = (DWORD)GetCfgUint(_T("RegulatedTemperatureMaxTimeFanSteady"), 0);

	dwRegulatedTemperatureFanSteadyValue = (DWORD)GetCfgUint(_T("RegulatedTemperatureFanSteadyValue"), 0);

	gdwBootCountGateValue = (DWORD)GetCfgUint(_T("RebootGateCount"), 0);

	dwLEDSystemTestComplexity = (DWORD)GetCfgUint(_T("LEDSystemTestComplexity"), 0);

	dwLEDSystemTestTriesAllowed = (DWORD)GetCfgUint(_T("LEDSystemTestRetriesAllowed"), 0);
	dwLEDSystemTestTriesAllowed = dwLEDSystemTestTriesAllowed + 1; // Make this tries instead of retries
	if (dwLEDSystemTestTriesAllowed > 10)
		dwLEDSystemTestTriesAllowed = 10;

	dwDVDMediaDetectTimeout = (DWORD)GetCfgUint(_T("DVDMediaDetectTimeout"), DVD_TIMEOUT_BETWEEN_OPERATIONS/1000);
	dwDVDMediaDetectTimeout = dwDVDMediaDetectTimeout * 1000;

	return true;

} // end bool CXModuleSMC::InitializeParameters ()
  

NTSTATUS WriteSMBusValue(UCHAR SlaveAddress, UCHAR CommandCode, BOOLEAN WriteWordValue, ULONG DataValue)
{
	NTSTATUS NtStatusTemp;

	EnterCriticalSection(&SMCRegisterCriticalSection);
	NtStatusTemp = HalWriteSMBusValue(SlaveAddress, CommandCode, WriteWordValue, DataValue);
	LeaveCriticalSection(&SMCRegisterCriticalSection);
	return NtStatusTemp;
}

NTSTATUS ReadSMBusValue(UCHAR SlaveAddress, UCHAR CommandCode, BOOLEAN ReadWordValue, ULONG *DataValue)
{
	NTSTATUS NtStatusTemp;

	EnterCriticalSection(&SMCRegisterCriticalSection);
	NtStatusTemp = HalReadSMBusValue(SlaveAddress, CommandCode, ReadWordValue, DataValue);
	LeaveCriticalSection(&SMCRegisterCriticalSection);
	return NtStatusTemp;
}

//////////////////////////////////////////////////////////////////////////////////
// Function name	: GetLEDStateFromTIUForBoardTest
// Description	    : Uses MID_XBOX_SENSOR_LINES to read the state/color of the LED
//					  that was changed using the SMC LED State register. Returns 
//					  the LED state that was requested through iState.
// Input			: int iState - Off, Green, Red or Orange LED State to be tested.
// Return type		: BYTE
//////////////////////////////////////////////////////////////////////////////////
BYTE CXModuleSMC::GetLEDStateFromTIUForBoardTest(int iState)
{
	CHostResponse CHR;
	int iError;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost;
	BYTE* bpTemp;
	BYTE  bGreen, bRed;

	// Use the full version of iSendHost because a response is expected.
	// No message is sent, only response is received after writing to the LED state register.
	// This message will receive a number indicating the state of the sensors on the XBOX.
	if ((iError = g_host.iSendHost(MID_XBOX_SENSOR_LINES, NULL, NULL, CHR, &dwActualBytesReceived, 
		 &dwErrorCodeFromHost, HOST_TIMEOUT)) != EVERYTHINGS_OK)
	{
		if (dwErrorCodeFromHost != 0)
		{
			// Process error from host
			bErrorFlag = TRUE;
			err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
			if(CheckAbort(HERE))
				return 0;
		}
		
		else
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			bErrorFlag = TRUE;
			err_HostCommunicationError(iError);
			if(CheckAbort(HERE))
				return 0;
		}
	}

	// Don't access the response buffer if it is NULL
	else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) 
	{
		// The response buffer is empty, so take appropriate action (like an error message)
		bErrorFlag = TRUE;
		err_BufferIsEmpty();
		if(CheckAbort(HERE))
			return 0;
	}
	
	else // The response is not NULL, and is not an error message, so process it
	{
		// Copy the data received to a local buffer for processing
		bpTemp = (BYTE*)CHR.pvBufferFromHost;  
		
		// Byte 10 in the response field is for Green LED and byte 11 for Red. 
		// 1 = on, 0 = off.
		bGreen = bpTemp[10];
		bRed = bpTemp[11];

		// return the LED state as requested
		switch(iState)
		{
		case Off:
			if((bRed == 0) && (bGreen == 0))
				return 1;
			else
				return 0;

		case Green:
			return bGreen;
		
		case Red:
			return bRed;
			
		case Orange:
			if((bRed == 1) && (bGreen == 1))
				return 1;
			else
				return 0;
		}
	}
 
	// Everything ok
	return 1;

} // end BYTE CXModuleSMC::GetLEDStateFromTIUForBoardTest(int iState)



//////////////////////////////////////////////////////////////////////////////////
// Function name	: GetLEDStateFromTIUForSystemTest
// Description	    : Uses MID_ASK_QUESTION to load the .bmp file for iState LED
//					  and asks the questions and records the response accordingly.
// Return type		: BYTE
//////////////////////////////////////////////////////////////////////////////////
BYTE CXModuleSMC::GetLEDStateFromTIUForSystemTest(int iState, bool bVerifyQuestion)
{
	CHostResponse CHR;
	int iError, iTotal = 0;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost;
	BYTE* bpTemp;
	BYTE  bAnswer;
	char chResponse[256];

	switch(iState)
	{
		// since the strings are unicode, # of bytes to be copied = sizeof string * 2 + 2
		// e.g. sizeof "LEDOff.bmp" = 10. So bytes to be copied = 10 * 2 + 2 = 22
	case 0:
		memcpy(chResponse, _T("LEDOff.bmp"), 22);  // Bitmap file to be loaded in the dialog box
		memcpy(&chResponse[22], _T(""), 2);        // The "extended" file can be .JPG, .AVI or .MPG. Defaults the same as the first file
		memcpy(&chResponse[24], _T("NORMAL_LED_OFF_QUESTION"), 48);  // Text to be prompted with
		if (bVerifyQuestion)
			memcpy(&chResponse[24], _T("VERIFY"), 12);  // Make this the verify version of the question
		memcpy(&chResponse[72], _T("Yes\n"), 10);  // Text for the left button
		memcpy(&chResponse[82], _T("No\n"), 8);   // Text for the right button
		iTotal = 90;
		break;
		
	case 1:
		memcpy(chResponse, _T("LEDGreen.bmp"), 26);
		memcpy(&chResponse[26], _T(""), 2);
		memcpy(&chResponse[28], _T("NORMAL_LED_GREEN_QUESTION"), 52);
		if (bVerifyQuestion)
			memcpy(&chResponse[28], _T("VERIFY"), 12);  // Make this the verify version of the question
		memcpy(&chResponse[80], _T("Yes\n"), 10); 
		memcpy(&chResponse[90], _T("No\n"), 8);  
		iTotal = 98;
		break;

	case 2:
		memcpy(chResponse, _T("LEDRed.bmp"), 22);
		memcpy(&chResponse[22], _T(""), 2);
		memcpy(&chResponse[24], _T("NORMAL_LED_RED_QUESTION"), 48);
		if (bVerifyQuestion)
			memcpy(&chResponse[24], _T("VERIFY"), 12);  // Make this the verify version of the question
		memcpy(&chResponse[72], _T("Yes\n"), 10); 
		memcpy(&chResponse[82], _T("No\n"), 8);  
		iTotal = 90;
		break;

	case 3:
		memcpy(chResponse, _T("LEDOrange.bmp"), 28);
		memcpy(&chResponse[28], _T(""), 2);
		memcpy(&chResponse[30], _T("NORMAL_LED_ORANGE_QUESTION"), 54);
		if (bVerifyQuestion)
			memcpy(&chResponse[30], _T("VERIFY"), 12);  // Make this the verify version of the question
		memcpy(&chResponse[84], _T("Yes\n"), 10); 
		memcpy(&chResponse[94], _T("No\n"), 8);  
		iTotal = 102;
		break;

	} // end switch

if ((iError = g_host.iSendHost(MID_ASK_QUESTION, chResponse, iTotal, CHR, &dwActualBytesReceived, 
		 &dwErrorCodeFromHost, 0x7FFFFFFF)) != EVERYTHINGS_OK)
	{
		if (dwErrorCodeFromHost != 0)
		{
			// Process error from host
			bErrorFlag = TRUE;
			err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
			if(CheckAbort(HERE))
				return 0;  
		}
		
		else
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			bErrorFlag = TRUE;
			err_HostCommunicationError(iError);
			if(CheckAbort(HERE))
				return 0;
		}
	}

	// Don't access the response buffer if it is NULL
	else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) 
	{
		// The response buffer is empty, so take appropriate action (like an error message)
		bErrorFlag = TRUE;
		err_BufferIsEmpty();
		if(CheckAbort(HERE))
			return 0;
	}

	else
	{
		// Copy the data received to a local buffer for processing
		bpTemp = (BYTE*)CHR.pvBufferFromHost; 
		bAnswer = bpTemp[0];

		return bAnswer;
	}

	// An error of some type happened
	return 0;
	
} // end BYTE CXModuleSMC::GetLEDStateFromTIUForSystemTest(int iState)



//////////////////////////////////////////////////////////////////////////////////
// Function name	: GetFanVoltageFromTIU
// Description	    : Uses MID_XBOX_SENSOR_LINES to read the fan volatge that was
//					  changed through the fan speed by using the SMC Fan Speed 
//					  register. 
// Return type		: float
//////////////////////////////////////////////////////////////////////////////////
float CXModuleSMC::GetFanVoltageFromTIU()
{
	float fFanVoltage;
	CHostResponse CHR;
	int iError;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost;

	// Use the full version of iSendHost because a response is expected.
	// No message is sent, only response is received after writing to the Fan Speed register.
	// This message will receive a number indicating the state of the Fan Voltage on the XBOX.
	if ((iError = g_host.iSendHost(MID_XBOX_SENSOR_LINES, NULL, NULL, CHR, &dwActualBytesReceived, 
		 &dwErrorCodeFromHost, HOST_TIMEOUT)) != EVERYTHINGS_OK)
	{
		if (dwErrorCodeFromHost != 0)
		{
			// Process error from host
			bErrorFlag = TRUE;
			err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
			if(CheckAbort(HERE))
				return 0.0f;
		}
		
		else
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			bErrorFlag = TRUE;
			err_HostCommunicationError(iError);
			if(CheckAbort(HERE))
				return 0.0f;
		}
	}

	// Don't access the response buffer if it is NULL
	else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) 
	{
		// The response buffer is empty, so take appropriate action (like an error message)
		bErrorFlag = TRUE;
		err_BufferIsEmpty();
		if(CheckAbort(HERE))
			return 0.0f;
	} 
	
	else // The response is not NULL, and is not an error message, so process it
	{
		// Copy the data received to a local buffer for processing. The fan voltage is in "float"
		// so copy as many bytes as are represented by a float variable (mostly 4).
		memcpy(&fFanVoltage, CHR.pvBufferFromHost, sizeof(float));
		
		// debug
		//ReportDebug(BIT0,_T("fan voltage from TIU = %f\n"), fFanVoltage);
		return fFanVoltage;			
	}

	return 0.0f;

} // end float CXModuleSMC::GetFanVoltageFromTIU()



//////////////////////////////////////////////////////////////////////////////////
// Function name	: GetAirSensorFromTIU
// Description	    : Uses MID_XBOX_SENSOR_LINES to read the Air that was turned
//					  on or off through the fan speed by using the SMC Fan Speed 
//					  register. 
// Return type		: BYTE
//////////////////////////////////////////////////////////////////////////////////
BYTE CXModuleSMC::GetAirSensorFromTIU()
{
	CHostResponse CHR;
	int iError;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost;
	BYTE* bpTemp;
	BYTE  bAirSensor;

	// Use the full version of iSendHost because a response is expected.
	// No message is sent, only response is received after writing to the Fan Speed register.
	// This message will receive a number indicating the state of the Air sensor on the XBOX.
	if ((iError = g_host.iSendHost(MID_XBOX_SENSOR_LINES, NULL, NULL, CHR, &dwActualBytesReceived, 
		 &dwErrorCodeFromHost, HOST_TIMEOUT)) != EVERYTHINGS_OK)
	{
		if (dwErrorCodeFromHost != 0)
		{
			// Process error from host
			bErrorFlag = TRUE;
			err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
			if(CheckAbort(HERE))
				return 0;
		}
		
		else
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			bErrorFlag = TRUE;
			err_HostCommunicationError(iError);
			if(CheckAbort(HERE))
				return 0;
		}
	}

	// Don't access the response buffer if it is NULL
	else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) 
	{
		// The response buffer is empty, so take appropriate action (like an error message)
		bErrorFlag = TRUE;
		err_BufferIsEmpty();
		if(CheckAbort(HERE))
			return 0;
	}
	
	else // The response is not NULL, and is not an error message, so process it
	{
		// Copy the data received to a local buffer for processing
		bpTemp = (BYTE*)CHR.pvBufferFromHost;  
		
		// Byte 9 in the response field is for for the Fan Air Sensor.
		// 1 = Sensor Off, 0 = Sensor On.
		bAirSensor = bpTemp[9];
		return bAirSensor;
	}

	return 0;
} // end BYTE CXModuleSMC::GetAirSensorFromTIU()



//////////////////////////////////////////////////////////////////////////////////
// Function name	: PutVModeStateToTIU
// Description	    : Uses MID_XBOX_SET_VIDEO_MODE_LINES to write the Video mode
//					  states requested through iState.
// Input			: int iState - Video Mode to be tested: DISABLE , SDTVNTSC,
//					               PALRFU, SDTVPAL, NTSCRFU, VGA, HDTV, SCART   
// Return type		: void
//////////////////////////////////////////////////////////////////////////////////
void CXModuleSMC::PutVModeStateToTIU(int iState)
{
	CHostResponse CHR;
	int iError;
	BYTE bVmode;

	// set the video mode
	switch(iState)
	{
		case DISABLE:
			bVmode = 0;
			break;

		case SDTVNTSC:
			bVmode = 1;
			break;

		case PALRFU:
			bVmode = 2;
			break;

		case SDTVPAL:
			bVmode = 3;
			break;

		case NTSCRFU:
			bVmode = 4;
			break;
		
		case VGA:
			bVmode = 5;
			break;

		case HDTV:
			bVmode = 6;
			break;

		case SCART:
			bVmode = 7;
			break;
	}	

	// Use the partial version of iSendHost because a response is not expected
	// No response is received, only message is sent to change the VMode lines
	if ((iError = g_host.iSendHost(MID_XBOX_SET_VIDEO_MODE_LINES, (char *)&bVmode, sizeof(BYTE)))
		 != EVERYTHINGS_OK)
	{
		// Process a communication error
		// This test module just reports the number of the communication error that was received
		err_HostCommunicationError(iError);
		if(CheckAbort(HERE))
			return;
	}

} // end BYTE CXModuleSMC::PutVModeStateToTIU(int iState)



///////////////////////////////////////////////////////////////////////////////////
// Actual tests
///////////////////////////////////////////////////////////////////////////////////


// This test verifes that the XBOX can switch to different video modes.
IMPLEMENT_TESTUNLOCKED (SMC, VModeTest, 1)
{
	ULONG uDataValue, uSave;
		int iError;

	// Read the original video mode 
	NtStatus = ReadSMBusValue(0x20, 0x04, FALSE, &uSave);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Read(_T("VMode State"), NtStatus);
		return;
	}
	// debug
	//ReportDebug(BIT0,_T("Original VMode = %d\n"), uSave);

	// Change the video mode through the TIU. After a delay of 500ms, read the VMode state 
	// SMC register to verify that the mode changed.

	// Vide Mode = SDTV PAL
	PutVModeStateToTIU(SDTVPAL);
	Sleep(500);	

	NtStatus = ReadSMBusValue(0x20, 0x04, FALSE, &uDataValue);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Read(_T("VMode State"), NtStatus);
		return;
	}

	// Verify that the Vmode state read by the SMC is the same as the state that was set thru TIU
	if(uDataValue != SMCSDTVPAL) 
	{
		err_VModeState(_T("SDTV PAL"));
		return;
	}

	// Vide Mode = HDTV
	PutVModeStateToTIU(HDTV);
	Sleep(500);

	NtStatus = ReadSMBusValue(0x20, 0x04, FALSE, &uDataValue);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Read(_T("VMode State"), NtStatus);
		return;
	}

	// Verify that the Vmode state read by the SMC is the same as the state that was set thru TIU
	if(uDataValue != SMCHDTV)
	{
		err_VModeState(_T("HDTV"));
		return;
	}
	
	// Vide Mode = SDTV NTSC
	PutVModeStateToTIU(SDTVNTSC);
	Sleep(500);
	
	NtStatus = ReadSMBusValue(0x20, 0x04, FALSE, &uDataValue);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Read(_T("VMode State"), NtStatus);
		return;
	}

	// Verify that the Vmode state read by the SMC is the same as the state that was set thru TIU
	if(uDataValue != SMCSDTVNTSC)
	{
		err_VModeState(_T("SDTV NTSC"));
		return; 
	}
	
	// Write back the original video mode
		if ((iError = g_host.iSendHost(MID_XBOX_SET_VIDEO_MODE_LINES, (char *)&uSave, sizeof(BYTE)))
		 != EVERYTHINGS_OK)
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			err_HostCommunicationError(iError);
			return;
		}
	
} // end IMPLEMENT_TESTUNLOCKED (SMC, VModeTest, 1)



// This test verifies that the XBOX can change different LED states - Off, Green, Red, Orange.
IMPLEMENT_TESTUNLOCKED (SMC, LEDBoardTest, 2)
{
	BYTE bResult;
	
	// Set LED to desired behavior
	// Off
	NtStatus = WriteSMBusValue(0x20, 0x08, FALSE, 0x00);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("LED State"), NtStatus);
		return;
	}

	// Set LED Override to use the requested LED Behavior
	NtStatus = WriteSMBusValue(0x20, 0x07, FALSE, 0x01);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("LED Override"), NtStatus);
		return;
	}

	Sleep(500);
	bResult = GetLEDStateFromTIUForBoardTest(Off);
	if((bErrorFlag == FALSE) && (bResult != 1))
	{
		err_LEDStateFromTIU(_T("Off"));
		return;
	}

	// Reset LED Override to use the default LED behavior
	NtStatus = WriteSMBusValue(0x20, 0x07, FALSE, 0x00);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("LED Override"), NtStatus);
		return;
	}


	// Green
	NtStatus = WriteSMBusValue(0x20, 0x08, FALSE, 0x0F);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("LED State"), NtStatus);
		return;
	}

	// Set LED Override to use the requested LED Behavior
	NtStatus = WriteSMBusValue(0x20, 0x07, FALSE, 0x01);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("LED Override"), NtStatus);
		return;
	}

	Sleep(500);
	bResult = GetLEDStateFromTIUForBoardTest(Green);
	if((bErrorFlag == FALSE) && (bResult != 1))
	{
		err_LEDStateFromTIU(_T("Green"));
		return;
	}

	// Reset LED Override to use the default LED behavior
	NtStatus = WriteSMBusValue(0x20, 0x07, FALSE, 0x00);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("LED Override"), NtStatus);
		return;
	}

	// Red
	NtStatus = WriteSMBusValue(0x20, 0x08, FALSE, 0xF0);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("LED State"), NtStatus);
		return;
	}

	// Set LED Override to use the requested LED Behavior
	NtStatus = WriteSMBusValue(0x20, 0x07, FALSE, 0x01);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("LED Override"), NtStatus);
		return;
	}

	Sleep(500);
	bResult = GetLEDStateFromTIUForBoardTest(Red);
	if((bErrorFlag == FALSE) && (bResult != 1))
	{
		err_LEDStateFromTIU(_T("Red"));
		return;
	}

	// Reset LED Override to use the default LED behavior
	NtStatus = WriteSMBusValue(0x20, 0x07, FALSE, 0x00);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("LED Override"), NtStatus);
		return;
	}

	// Orange
	NtStatus = WriteSMBusValue(0x20, 0x08, FALSE, 0xFF);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("LED State"), NtStatus);
		return;
	}

	// Set LED Override to use the requested LED Behavior
	NtStatus = WriteSMBusValue(0x20, 0x07, FALSE, 0x01);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("LED Override"), NtStatus);
		return;
	}

	Sleep(500);
	bResult = GetLEDStateFromTIUForBoardTest(Orange);
	if((bErrorFlag == FALSE) && (bResult != 1))
	{
		err_LEDStateFromTIU(_T("Orange"));
		return;
	}

	// Reset LED Override to use the default LED behavior
	NtStatus = WriteSMBusValue(0x20, 0x07, FALSE, 0x00);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("LED Override"), NtStatus);
		return;
	}

} // end IMPLEMENT_TESTUNLOCKED (SMC, LEDBoardTest, 2)



// This test verifies that fan speed can be varied and the fan voltage for each speed is
// within its predefined Lower and Upper limits.
//The correct sequence for controlling fan speed, according to the SMC firmware author is:
//1) Write the fan speed you want first to reg 0x06
//2) Now write the fan override bit in reg 0x05
//3) When finished with fan speed clear override bit in reg 0x05
//4) Repeat steps 1 - 3 for each new fan speed.
IMPLEMENT_TESTUNLOCKED (SMC, FanSpeedBoardTest, 3)
{
	float fFanVoltage;

	// Set fan speed to desired speed
	// Max fan speed = 50
	NtStatus = WriteSMBusValue(0x20, 0x06, FALSE, 0x32); 
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Speed"), NtStatus); 
		return;
	}

	// Set fan Override to use the requested fan speed
	NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x01);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Override"), NtStatus);
		return;
	}

	Sleep(500);
	fFanVoltage = GetFanVoltageFromTIU();
	
	// Make sure that the fan voltage is within the lower and upper limits
	if((fFanVoltage < fFanSpeed50LL) || (fFanVoltage > fFanSpeed50UL))
	{
		err_FanVoltLimit(fFanVoltage, 50, fFanSpeed50LL, fFanSpeed50UL);
		return;
	}

	// Reset fan override
	NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x00);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Override"), NtStatus);
		return;
	}

	// fan speed = 20
	NtStatus = WriteSMBusValue(0x20, 0x06, FALSE, 0x14); 
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Speed"), NtStatus);
		return;
	}

	// Set fan Override to use the requested fan speed
	NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x01);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Override"), NtStatus);
		return;
	}
	
	Sleep(500);
	fFanVoltage = GetFanVoltageFromTIU();

	// Make sure that the fan voltage is within the lower and upper limits
	if((fFanVoltage < fFanSpeed20LL) || (fFanVoltage > fFanSpeed20UL)) 
	{
		err_FanVoltLimit(fFanVoltage, 20, fFanSpeed20LL, fFanSpeed20UL);
		return;
	}

	// Reset fan override
	NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x00);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Override"), NtStatus);
		return;
	}

} // end IMPLEMENT_TESTUNLOCKED (SMC, FanSpeedBoardTest, 3)



// This test verifies that the actual firmware revision read from the SMC Firmware Revision
// register matches(does't match) with the list of allowed(not allowed) Revision IDs. If no
// allowed or not allowed list is specified, it is assumed that any revision read from
// the SMC register is allowed.
IMPLEMENT_TESTUNLOCKED (SMC, RevisionCheckTest, 4)
{
	LPCTSTR pwsRevType, pwsRevCnt[MAX_REVISIONS];
	wchar_t* pwsCfgRev[MAX_REVISIONS];
	TCHAR chBuf[80], chRev[4];
	ULONG uDataValue;
	int i, iCnt, iResult;

	// The list of allowed and not allowed revisions is mutually exclusive. So the
	// RevisionAllowed[##] and RevisionNotAllowed[##] parameters both cannot exist.
	if((pwsAllowed != NULL) && (pwsNotAllowed != NULL))
	{
		err_RevisionXor();
		return;
	}

	// If neither RevisionAllowed[##] nor RevisionNotAllowed[##] parameters exist,
	// there are no revision restrictions.
	if((pwsAllowed == NULL) && (pwsNotAllowed == NULL))
	{
		ReportDebug(BIT0,_T("RevisionAllowed[##] or RevisionNotAllowed[##] list is not specified")
					  _T(" so passing the test.\n"));
		return;
	}

	pwsRevType = pwsRevAllowed; // Default to allowed

	if(pwsNotAllowed != NULL)
		pwsRevType = pwsRevNotAllowed;

	// Write to the register first to reset it 
	for(i = 0; i < 3; i++)
	{			
		NtStatus = WriteSMBusValue(0x20, 0x01, FALSE, 0x00);

		if(NtStatus != STATUS_SUCCESS)
		{
			err_Write(_T("Firmware Revision"), NtStatus);
			return;
		}
	}

	// Get the actual SMC Firmware Revision ID from the SMC Revision Register
	// The Rev ID is 3 bytes long (e.g. B1C, A4E)
	for(i = 0; i < 3; i++)
	{			
		NtStatus = ReadSMBusValue(0x20, 0x01, FALSE, &uDataValue);

		if(NtStatus != STATUS_SUCCESS)
		{
			err_Read(_T("Firmware Revision"), NtStatus);
			return;
		}

		// Store the value in a char array so that it can be used as a string that
		// can be compared with the Rev ID string obtained from the  allowed or not
		// allowed list of Cfg parameters.
		chRev[i] = (TCHAR)uDataValue;
		
		// debug
		//ReportDebug(BIT0,_T("chRev[%d] = %c\n"), i, chRev[i]);
	
	} // end for(i = 0; i < 3; i++)

	// Terminate with Null
	chRev[3] = '\0';

	// Initialize pwsCfgRev 
	for (iCnt = 0; iCnt < MAX_REVISIONS; iCnt++)
		pwsCfgRev[iCnt] = NULL;

	// Get the list of allowed OR not allowed Revisions specified in the Cfg parameters
	for (iCnt = 0; iCnt < MAX_REVISIONS; iCnt++)
	{
		_stprintf(chBuf, pwsRevType, iCnt);
		pwsRevCnt[iCnt] = GetCfgString(chBuf, NULL); 
		
		if(pwsRevCnt[iCnt] == NULL)
			break;
		
		// Convert the string to uppercase as the actual Rev ID will always be upper case
		else
			pwsCfgRev[iCnt] = _tcsupr((wchar_t*)pwsRevCnt[iCnt]);

	} // end for (iCnt = 0; iCnt < MAX_REVISIONS; iCnt++)

	// Compare the Actual Rev Id witht the Rev Id(s) obtained from the allowed OR not allowed
	// list of Cfg parameters
	for (i = 0; i < MAX_REVISIONS; i++)
	{
		if(pwsCfgRev[i] != NULL)
		{
			iResult = _tcscmp(chRev, (LPCTSTR)pwsCfgRev[i]);
			
			// one of the allowed Rev Ids matches with the actual Rev Id
			if(pwsAllowed && (iResult == 0))
				return;

			// One of the not allowed Rev Ids matches with the actual Rev Id so report error
			if(pwsNotAllowed && (iResult == 0))
			{
				err_RevisionNotAllowed(chRev);
				return; 
			}
			
		} // end if(pwsCfgRev[i] != NULL)

	} // end for (i = 0; i < MAX_REVISIONS; i++)

	// The list of allowed Revision Id Cfg parameters did not contain the actual
	// Rev Id that was read from the SMC Revision register so report error
	if(pwsAllowed)
		err_RevisionNotAllowed(chRev);
	
} // end IMPLEMENT_TESTUNLOCKED (SMC, RevisionCheckTest, 4)



// This utility resets the system.
IMPLEMENT_TESTUNLOCKED (SMC, SystemResetUtility, 5)
{
	TCHAR tcBuffer[300]; // Holds data to send to the host in messages
	int iError;
	DWORD dwBufferLength;

	// Use the partial version of iSendHost because a response is not expected
	// No response is received, only message is sent
	// Send MID_UUT_TESTEND command for this test
//	_stprintf(tcBuffer, _T("%s.%s"), GetModuleName(), GetTestName());
	_stprintf(tcBuffer, _T("%s"), GetFullName());
	dwBufferLength = (_tcslen(tcBuffer) + 1) * sizeof(TCHAR);
	// Add the 2 DWORDs here (10 and 1)
	*((DWORD *)&(((char *)tcBuffer)[dwBufferLength])) = 10; // Elapsed time for this test (set to 10 ms)
	dwBufferLength = dwBufferLength + sizeof(DWORD);
	*((DWORD *)&(((char *)tcBuffer)[dwBufferLength])) = 1; // PASSED indication
	dwBufferLength = dwBufferLength + sizeof(DWORD);

	if ((iError = g_host.iSendHost(MID_UUT_TESTEND, (char *)&tcBuffer[0], dwBufferLength))
		 != EVERYTHINGS_OK)
	{
		// Process a communication error
		// This test module just reports the number of the communication error that was received
		err_HostCommunicationError(iError);
		if(CheckAbort(HERE))
			return;
	}
	// Use the partial version of iSendHost because a response is not expected
	// No response is received, only message is sent
	// Send MID_UUT_RESTARTING command for this test
	if ((iError = g_host.iSendHost(MID_UUT_RESTARTING, NULL, 0))
		 != EVERYTHINGS_OK)
	{
		// The test has officially ended due to the message above, so don't send any more error messages.
		// Restart the unit below and it will fail on the next boot.
	}

	// Reset the system by writing 0x01 to the SMC System Reset Register
	NtStatus = WriteSMBusValue(0x20, 0x02, FALSE, 0x01);
	if(NtStatus != STATUS_SUCCESS)
	{
		// Again, don't report an error because no one is listening
//		err_Reset(NtStatus);
//		return;
	}
	Sleep(INFINITE);

} // end IMPLEMENT_TESTUNLOCKED (SMC, SystemResetUtility, 5)



DWORD CXModuleSMC::dwLEDSystemTest(DWORD dwLEDSystemTestTriesLeft)
{
#define LED_FAIL 0
#define LED_RETRY 1
#define LED_OK 2
	int ii, iIndex, iLedArray[5], iCount, iIncorrectLED = 0xFF;
	BYTE bResult;
	bool bArrayCount[5], bOff, bRed, bGreen, bOrange, bIncorrect;
	ULONG uLED = 0;
	bool bPreviousResponseWrong = false;
	UINT uiSeed, uiRandScratch;

	// Seed the random number with the tick count which gives the count of system ticks (or 
	// milliseconds) since boot time.
	vNewSeed(&uiSeed);
	uiRandScratch=uiSeed;

	if (dwLEDSystemTestComplexity != 1) // Do the simpler test
	{
		// initialize arrays
		for(ii = 0; ii < 5; ii++)
		{
			iLedArray[ii] = 0xFF; // didn't initialize this to 0, since Off = 0
			bArrayCount[ii] = false; // to track which slot(0..4) in the array is filled 
		}

		// to track which color LED is filled in the array slot
		bOff = false;
		bRed = false;
		bGreen = false;
		bOrange = false;
		
		bIncorrect = false;
		iCount = 0;

		// First randomize the LEDs into an array of 4. eg. Green, Red, Off, Orange OR Red, Off, Green,
		// Orange...
		while(iCount != 4)
		{
			// Get the random number, divide it by 4 and use the remainder (0, 1, 2, 3) to fill up
			// the array for LEDs randomly
			iIndex = random(&uiRandScratch) % 4; 

			// Make sure that there are no collisions so that each location will have a unique
			// LED color (Off, Red, Green Orange)
			// Off
			if((bArrayCount[iIndex] == false) && ((iLedArray[iIndex] != Red) || (iLedArray[iIndex] != Green)||
				(iLedArray[iIndex] != Orange)) && (bOff == false))
			{
				bOff = true;
				bArrayCount[iIndex] = true;
				iLedArray[iIndex] = Off; 
				iCount++;
			}
			
			// Green
			else if((bArrayCount[iIndex] == false) && ((iLedArray[iIndex] != Red) || (iLedArray[iIndex] != Orange) ||
				(iLedArray[iIndex] != Off)) && (bGreen == false))
			{
				bGreen = true;
				bArrayCount[iIndex] = true;
				iLedArray[iIndex] = Green;
				iCount++;
			}

			// Red
			else if((bArrayCount[iIndex] == false) && ((iLedArray[iIndex] != Off) || (iLedArray[iIndex] != Green) ||
				(iLedArray[iIndex] != Orange)) && (bRed == false))
			{
				bRed = true;
				bArrayCount[iIndex] = true;
				iLedArray[iIndex] = Red;
				iCount++;
			}

			// Orange
			else if((bArrayCount[iIndex] == false) && ((iLedArray[iIndex] != Red) || (iLedArray[iIndex] != Green) ||
				(iLedArray[iIndex] != Off)) && (bOrange == false))
			{
				bOrange = true;
				bArrayCount[iIndex] = true;
				iLedArray[iIndex] = Orange;
				iCount++;
			}

		} // end while(iCount != 4)

		// debug
		/*for(ii = 0; ii < 4; ii++)
			ReportDebug(BIT0,_T("iLedArray[%d] = %d\n"), ii, iLedArray[ii]);
		*/

		// Get another random #. Use this as an index into iLedArray such that the content at that
		// index is copied to the last slot of the array. This is done to randomize the operator 
		// response so that an incorrect LED bitmap is shown for iLedArray[iIndex] and a correct
		// bitmap is shown for iLedArray[4] eventhough the actual LED shown on the system will be 
		// correct.
		// e.g. iIndex = 0 and iLedArray[0] = 2. So iLedArray[4] will also be 2. 2 = Red. The system
		//		displays Red LED. However, for iLedArray[0] either Green, Orange or Off LED bitmap 
		//      is shown and asked if it was red (the correct response is "No") and for iLedArray[4] 
		//		Red LED bitmap is shown (the correct response is "Yes").
		//      Next time iIndex could be 2 and so on...
		iIndex = random(&uiRandScratch) % 4;
		
		iLedArray[4] = iLedArray[iIndex];

		// debug
		/*for(ii = 0; ii < 5; ii++)
			ReportDebug(BIT0,_T("iLedArray[%d] = %d\n"), ii, iLedArray[ii]);
		*/

		// Get another random # that is used to randomly load an incorrect .bmp. This is done so that
		// same color .bmp will not be loaded every time.
		do
		{
			iIncorrectLED = random(&uiRandScratch) %4;
		} while(iIncorrectLED == iLedArray[4]);
		
		// Now set the desired LED color, ask the questions and record response
		for(ii = 0; ii < 5; ii++)
		{	
			// 0 = Off = 0x00, 1 = Green = 0x0F, 2 = Red = 0xF0, 3 = Orange = 0xFF
			switch(iLedArray[ii])
			{
				case 0:
					uLED = 0x00;
					break;
				case 1:
					uLED = 0x0F;
					break;
				case 2:
					uLED = 0xF0;
					break;
				case 3:
					uLED = 0xFF;
					break;
			} // end switch

			// Set LED to desired color - Off, Green, Red, Orange
			NtStatus = WriteSMBusValue(0x20, 0x08, FALSE, uLED);
			if(NtStatus != STATUS_SUCCESS)
			{
				err_Write(_T("LED State"), NtStatus);
				return LED_FAIL;
			}

			// Set LED Override to use the requested LED Behavior
			NtStatus = WriteSMBusValue(0x20, 0x07, FALSE, 0x01);
			if(NtStatus != STATUS_SUCCESS)
			{
				err_Write(_T("LED Override"), NtStatus);
				return LED_FAIL;
			}

			Sleep(500);

			// This is to set the LED on the system with the correct color but load up the
			// wrong .bmp file and make sure that the operator answered "No" when asked about
			// the LED color.
			if(ii == iIndex)
			{
				bResult = GetLEDStateFromTIUForSystemTest(iIncorrectLED, bPreviousResponseWrong);		

				if ((bResult == 0) && CheckAbort(HERE))
					break;
				else
				{
					if(bResult != IDCANCEL)
					{
						if (bPreviousResponseWrong || (dwLEDSystemTestTriesLeft <= 1)) // Bad response, and previous response is wrong or there are no more retries
						{
							err_LEDResponse(_T("No"), _T("Yes"));
							return LED_FAIL;
						}
						else // Bad response, but previous response was right and there are retreis left, so ask the question again
						{
							bPreviousResponseWrong = true;
							ii--; // Do this question over again
						}
					}
					else // This was a correct response
					{
						if (bPreviousResponseWrong) // The previous response was wrong, but this one was right, so retry the whole test.
							return LED_RETRY;
					}
				}
			}
			
			// Load the correct .bmp for LED and set the system with the correct LED.
			else
			{
				bResult = GetLEDStateFromTIUForSystemTest(iLedArray[ii], bPreviousResponseWrong);		
			
				if ((bResult == 0) && CheckAbort(HERE))
					break;
				else
				{
					if(bResult != IDOK) // This was an incorrect response
					{
						if (bPreviousResponseWrong || (dwLEDSystemTestTriesLeft <= 1)) // Bad response, and previous response is wrong or there are no more retries
						{
							err_LEDResponse(_T("Yes"), _T("No"));
							return LED_FAIL;
						}
						else // Bad response, but previous response was right and there are retreis left, so ask the question again
						{
							bPreviousResponseWrong = true;
							ii--; // Do this question over again
						}
					}
					else // This was a correct response
					{
						if (bPreviousResponseWrong) // The previous response was wrong, but this one was right, so retry the whole test.
							return LED_RETRY;
					}
				}
			}

			// Reset LED Override to use the default LED behavior
			NtStatus = WriteSMBusValue(0x20, 0x07, FALSE, 0x00);
			if(NtStatus != STATUS_SUCCESS)
			{
				err_Write(_T("LED Override"), NtStatus);
				return LED_FAIL;
			}

		} // end for(ii = 0; ii < 4; ii++)
	}
	else  // Perform the complex version of the test
	{
#define LED_YES 1
#define LED_NO  0
#define LED_NOT_ASSIGNED  100
#define MAX_NUM_QUESTIONS 6
	int iResponseExpected[MAX_NUM_QUESTIONS];
	DWORD dwYesColors[MAX_NUM_QUESTIONS];
	DWORD dwDisplayColor[MAX_NUM_QUESTIONS];
	DWORD dwQuestionColor[MAX_NUM_QUESTIONS];
	DWORD dwColorsUsed[4];
	DWORD dwNoLocation[2];
	DWORD i, j;
	DWORD dwNumQuestions, dwNumNoResponses=0, dwNumColors;
		for (i = 0; i < MAX_NUM_QUESTIONS; i++) // Start out assuming that all responses are supposed to be "Yes"
			iResponseExpected[i] = LED_YES;
		dwNumQuestions = (random(&uiRandScratch) % 2) + 5;  // 5 or 6 questions
		if (dwNumQuestions == 5)
		{
			if ((random(&uiRandScratch) % 5) == 4) // 80% weight on 1 No reponses; 20% weight on 0 No responses
				dwNumNoResponses = 0;
			else
				dwNumNoResponses = 1;
		}
		else
			dwNumNoResponses = (random(&uiRandScratch) % 2) + 1;  // 50% weight on 1 or 2 No responses
//ReportWarning(L"Questions = %lu, Nos = %lu", dwNumQuestions, dwNumNoResponses);
		for (i = 0; i < dwNumNoResponses; i++) // Fill the No response array
		{
			do {
				dwNoLocation[i] = (random(&uiRandScratch) % (3 + dwNumNoResponses));
			} while (iResponseExpected[dwNoLocation[i]] == LED_NO);
			iResponseExpected[dwNoLocation[i]] = LED_NO;
		}
		if (dwNumNoResponses == 2)
		{
			if (dwNoLocation[0] > dwNoLocation[1]) // Put the lower location first
			{
				j = dwNoLocation[0];
				dwNoLocation[0] = dwNoLocation[1];
				dwNoLocation[1] = j;
			}
		}
		// Choose the order of the colors for YES responses
		for (i = 0; i < 4; i++)  // No colors have been selected yet
			dwColorsUsed[i] = LED_NOT_ASSIGNED;
		for (i = 0; i < MAX_NUM_QUESTIONS; i++)
			dwDisplayColor[i] = LED_NOT_ASSIGNED;

		j = 0;
		for (i = 0; i < 4; i++)
		{
			do {
				dwYesColors[i] = (random(&uiRandScratch) % 4);
			} while (dwColorsUsed[dwYesColors[i]] != LED_NOT_ASSIGNED);
			dwColorsUsed[dwYesColors[i]] = 1;
			while (iResponseExpected[j] != LED_YES)
				j++;
			dwDisplayColor[j] = dwYesColors[i];
			j++;
		}
		if ((dwNumQuestions-dwNumNoResponses) == 5) // We need one more Yes color
		{
			dwYesColors[4] = (random(&uiRandScratch) % 4);
			while (iResponseExpected[j] != LED_YES)
				j++;
			dwDisplayColor[j] = dwYesColors[4];
		}
		for (i = 0; i < MAX_NUM_QUESTIONS; i++)  // Make the question and display arrays identical
			dwQuestionColor[i] = dwDisplayColor[i];
		for (i = 0; i < dwNumNoResponses; i++)  // Fill No slots of question array with colors
		{
			for (j = 0; j < 4; j++)  // No colors have been selected yet
				dwColorsUsed[j] = LED_NOT_ASSIGNED;
			for (j = 0; j < dwNoLocation[i]; j++) // Find all Yes colors so far up to this No question
			{
				if (iResponseExpected[j] == LED_YES)
					dwColorsUsed[dwQuestionColor[j]] = 1;
			}
			do {
				dwQuestionColor[j] = (random(&uiRandScratch) % 4);
			} while (dwColorsUsed[dwQuestionColor[j]] != LED_NOT_ASSIGNED);
		}
		// Put colors in the No locations of the display array.  Use colors not yet seen, if possible.
		for (i = 0; i < dwNumNoResponses; i++)  // Fill No slots of display array with colors
		{
			for (j = 0; j < 4; j++)  // No colors have been selected yet
				dwColorsUsed[j] = LED_NOT_ASSIGNED;
			for (j = 0; j < dwNoLocation[i]; j++) // Find all display colors (either yes or no) so far up to this No question
			{
//				if (iResponseExpected[j] == LED_YES)
					dwColorsUsed[dwDisplayColor[j]] = 1;
			}
			dwColorsUsed[dwQuestionColor[dwNoLocation[i]]] = 1; // Don't display the color we ask in the No question!
			dwNumColors = 0;
			for (j = 0; j < 4; j++) // Count how many colors have been used
			{
				if (dwColorsUsed[j] != LED_NOT_ASSIGNED)
					dwNumColors++;
			}
			if (dwNumColors < 4) // There is at least one color that has not been displayed, so find it
			{
				do {
					dwDisplayColor[dwNoLocation[i]] = (random(&uiRandScratch) % 4);
				} while (dwColorsUsed[dwDisplayColor[dwNoLocation[i]]] != LED_NOT_ASSIGNED);
			}
			else // All colors have already been displayed.  Just select a random one that isn't the one being asked in the No question
			{
				for (j = 0; j < 4; j++)  // No colors have been selected yet
					dwColorsUsed[j] = LED_NOT_ASSIGNED;
				dwColorsUsed[dwQuestionColor[dwNoLocation[i]]] = 1; // Don't display the color we ask in the No question!
				do {
					dwDisplayColor[dwNoLocation[i]] = (random(&uiRandScratch) % 4);
				} while (dwColorsUsed[dwDisplayColor[dwNoLocation[i]]] != LED_NOT_ASSIGNED);
			}
		}
//for (i = 0; i < dwNumQuestions; i++)
//ReportWarning(L"Question %lu = %lu, question color %lu display %lu", i, iResponseExpected[i], dwQuestionColor[i], dwDisplayColor[i]);
//for (i = 0; i < dwNumNoResponses; i++)
//ReportWarning(L"No Response %lu location = %lu", i, dwNoLocation[i]);

		// Now set the desired LED color, ask the questions and record response
		for(ii = 0; ii < (int)dwNumQuestions; ii++)
		{	
			// 0 = Off = 0x00, 1 = Green = 0x0F, 2 = Red = 0xF0, 3 = Orange = 0xFF
			switch(dwDisplayColor[ii])
			{
				case 0:
					uLED = 0x00;
					break;
				case 1:
					uLED = 0x0F;
					break;
				case 2:
					uLED = 0xF0;
					break;
				case 3:
					uLED = 0xFF;
					break;
			} // end switch

			// Set LED to desired color - Off, Green, Red, Orange
			NtStatus = WriteSMBusValue(0x20, 0x08, FALSE, uLED);
			if(NtStatus != STATUS_SUCCESS)
			{
				err_Write(_T("LED State"), NtStatus);
				return LED_FAIL;
			}

			// Set LED Override to use the requested LED Behavior
			NtStatus = WriteSMBusValue(0x20, 0x07, FALSE, 0x01);
			if(NtStatus != STATUS_SUCCESS)
			{
				err_Write(_T("LED Override"), NtStatus);
				return LED_FAIL;
			}

			Sleep(500);

			// This is to set the LED on the system with the correct color but load up the
			// wrong .bmp file and make sure that the operator answered "No" when asked about
			// the LED color.
			if(iResponseExpected[ii] == LED_NO)
			{
				bResult = GetLEDStateFromTIUForSystemTest((int)dwQuestionColor[ii], bPreviousResponseWrong);

				if ((bResult == 0) && CheckAbort(HERE))
					break;
				else
				{
					if(bResult != IDCANCEL)
					{
						if (bPreviousResponseWrong || (dwLEDSystemTestTriesLeft <= 1)) // Bad response, and previous response is wrong or there are no more retries
						{
							err_LEDResponse(_T("No"), _T("Yes"));
							return LED_FAIL;
						}
						else // Bad response, but previous response was right and there are retreis left, so ask the question again
						{
							bPreviousResponseWrong = true;
							ii--; // Do this question over again
						}
					}
					else // This was a correct response
					{
						if (bPreviousResponseWrong) // The previous response was wrong, but this one was right, so retry the whole test.
							return LED_RETRY;
					}
				}
			}
			
			// Load the correct .bmp for LED and set the system with the correct LED.
			else
			{
				bResult = GetLEDStateFromTIUForSystemTest((int)dwQuestionColor[ii], bPreviousResponseWrong);
			
				if ((bResult == 0) && CheckAbort(HERE))
					break;
				else
				{
					if(bResult != IDOK) // This was an incorrect response
					{
						if (bPreviousResponseWrong || (dwLEDSystemTestTriesLeft <= 1)) // Bad response, and previous response is wrong or there are no more retries
						{
							err_LEDResponse(_T("Yes"), _T("No"));
							return LED_FAIL;
						}
						else // Bad response, but previous response was right and there are retreis left, so ask the question again
						{
							bPreviousResponseWrong = true;
							ii--; // Do this question over again
						}
					}
					else // This was a correct response
					{
						if (bPreviousResponseWrong) // The previous response was wrong, but this one was right, so retry the whole test.
							return LED_RETRY;
					}
				}
			}

			// Reset LED Override to use the default LED behavior
			NtStatus = WriteSMBusValue(0x20, 0x07, FALSE, 0x00);
			if(NtStatus != STATUS_SUCCESS)
			{
				err_Write(_T("LED Override"), NtStatus);
				return LED_FAIL;
			}

		} // end for(ii = 0; ii < dwNumQuestions; ii++)
	}
	return LED_OK;
}


// This is an interractive test. It arranges the LED colors in a random order in an array of 4. Then 
// picks another random # as an index such that the LED color at that index is also copied to the 
// last slot of an arrayof 5. So there will be 3 unique and two duplicate LEDs. For the unique LEDs, 
// the system turns on that LED color and asks the "correct" question for that color. For the 
// duplicate LEDs, correct quesiton is asked for the last slot (5) and correct LED color is set on
// the system however, "incorrect" question is asked by loading an incorrect .bmp from any of the
// the three unique colors. This is also done by generating another random # so that an incorrect
// LED color .bmp is not loaded all the time.
//
// This test also includes a more random version that is enabled when the
// LEDSystemTestComplexity parameter is set to 1.  This version asks up to 6
// questions and can include up to 2 No reponses.  This version requires more
// operator attention, and guarantees that test operators observe the
// state of the LED for at least 85% of their responses (where only 47% of
// the responses need to be observed with the simpler test).
IMPLEMENT_TESTUNLOCKED (SMC, LEDSystemTest, 6)
{
	DWORD dwResult;
	DWORD dwLEDSystemTestTriesLeft = dwLEDSystemTestTriesAllowed;

	while (dwLEDSystemTestTriesLeft > 0)
	{
		if ((dwResult = dwLEDSystemTest(dwLEDSystemTestTriesLeft)) == LED_FAIL)
			return; // This was a failure that could not be recovered, or all retries are used up
		else if(dwResult == LED_OK) // We passed the entire test
			break;
		dwLEDSystemTestTriesLeft--;
	}
} // end IMPLEMENT_TESTUNLOCKED (SMC, LEDSystemTest, 6)



// This test reads the CPU Junction and Internal Air temps and compares them against their lower
// and upper limits as specified in the cfg params. The purpose of this test is to make sure 
// that the temp sensor is not stuck at one value (at board and system level).
IMPLEMENT_TESTUNLOCKED (SMC, ThermalTest, 7)
{
	ULONG uDataValue;

	// Read the CPU Junction Temp
	NtStatus = ReadSMBusValue(0x20, 0x09, FALSE, &uDataValue); 
	ReportStatistic(CPU_JUNCTION_TEMP, _T("%d"), uDataValue);

	// Compare the CPU Junction Temp with lower and upper limits 
	if((uDataValue < uiCpuJunTempLL) || (uDataValue > uiCpuJunTempUL))
	{
		err_CpuJunctionTemp(uDataValue, uiCpuJunTempLL, uiCpuJunTempUL);
		return;
	}

	// Read the Internal Air Temp
	NtStatus = ReadSMBusValue(0x20, 0x0A, FALSE, &uDataValue);
	ReportStatistic(INTERNAL_AIR_TEMP, _T("%d"), uDataValue);
	
	// Compare the Internal Air Temp with lower and upper limits 
	if((uDataValue < uiInternalAirTempLL) || (uDataValue > uiInternalAirTempUL))
	{
		err_InternalAirTemp(uDataValue, uiInternalAirTempLL, uiInternalAirTempUL);
		return;
	}

} // end IMPLEMENT_TESTUNLOCKED (SMC, ThermalTest, 7)



// This utility clamps and unclamps the audio depending on the AudioClamp
// configuration parameter (yes = clamp audio, no = unclamp audio).
IMPLEMENT_TESTUNLOCKED (SMC, AudioClampUtility, 8)
{
	int iResult;

	// Configuration parameter = "No" implies, release the audio clamp
	if((iResult = _tcsicmp(pwsAudioClamp, _T("No"))) == 0)
	{
		NtStatus = WriteSMBusValue(0x20, 0x0B, FALSE, 0x00);

		if(NtStatus != STATUS_SUCCESS)
		{
			err_AudioClampRelease();
			return;
		}
	}

	// Assume configuration parameter = "Yes" implies, clamp the audio
	else
	{
		NtStatus = WriteSMBusValue(0x20, 0x0B, FALSE, 0x01);

		if(NtStatus != STATUS_SUCCESS)
		{
			err_AudioClamp();
			return;
		}
	}

} // end IMPLEMENT_TESTUNLOCKED (SMC, AudioClampUtility, 8)
 


IMPLEMENT_TESTUNLOCKED (SMC, DVDTrayStatesandEmptyTest, 9)
{
	#define DVD_DELAY_BEWTEEN_POLLS 20  // Number of milliseconds to sleep between polls of SMC registers
	ULONG uDataValue, i, j, NumLoops; 
	bool bJustEjected;
	DWORD dwMediaDetectNumLoops;

	NumLoops = DVD_TIMEOUT_BETWEEN_OPERATIONS/DVD_DELAY_BEWTEEN_POLLS;
	dwMediaDetectNumLoops = dwDVDMediaDetectTimeout/DVD_DELAY_BEWTEEN_POLLS;
	// Wait for detection of media in the DVD drive
	for (i = 0; i < NumLoops; i++)
	{
		if ((NtStatus = ReadSMBusValue(0x20, 0x03, FALSE, &uDataValue)) != STATUS_SUCCESS)
		{
			err_Read(_T("DVD TrayState"), NtStatus); 
			return;
		}
		if (uDataValue == 0x60)  // Media is detected with the tray closed
			break;
		Sleep(DVD_DELAY_BEWTEEN_POLLS);
	}
	if (i == NumLoops)
	{
		err_NoDVDMediaDetected();
		return;
	}

	// Issue an Eject Command to open the DVD drive
	if ((NtStatus = WriteSMBusValue(0x20, 0x0C, FALSE, 0x00)) != STATUS_SUCCESS)
	{
		err_Write(_T("DVD Tray Operation"), NtStatus); 
		return;
	}
	bJustEjected = TRUE;
	
	// Wait for activity to be indicated
	for (i = 0; i < NumLoops; i++)
	{
		if ((NtStatus = ReadSMBusValue(0x20, 0x03, FALSE, &uDataValue)) != STATUS_SUCCESS)
		{
			err_Read(_T("DVD TrayState"), NtStatus); 
			return;
		}
		if ((uDataValue & 0x01) == 0x01)  // Activity is indicated by the DVD drive
			break;
		Sleep(DVD_DELAY_BEWTEEN_POLLS);
	}
	if (i == NumLoops)
	{
		err_NoDVDActivityDetected();
		return;
	}

	for (j=5; j > 0; j--) // Keep re-ejecting the tray forever until media is removed
	{
		if (!bJustEjected)
		{
			// Issue an Eject Command to open the DVD drive (again for repeated loops)
			if ((NtStatus = WriteSMBusValue(0x20, 0x0C, FALSE, 0x00)) != STATUS_SUCCESS)
			{
				err_Write(_T("DVD Tray Operation"), NtStatus); 
				return;
			}
		}
		bJustEjected = FALSE;

		// Wait for tray open state to be indicated
		for (i = 0; i < NumLoops; i++)
		{
			if ((NtStatus = ReadSMBusValue(0x20, 0x03, FALSE, &uDataValue)) != STATUS_SUCCESS)
			{
				err_Read(_T("DVD TrayState"), NtStatus); 
				return;
			}
			if (uDataValue == 0x10)  // The tray is open
				break;
			Sleep(DVD_DELAY_BEWTEEN_POLLS);
		}
		if (i == NumLoops)
		{
			err_DVDTrayNotOpen();
			return;
		}

		// Wait forever for a state change from Tray Open; send a keep-alive every 5 seconds
		i = 5000/DVD_DELAY_BEWTEEN_POLLS;
		for (;;)
		{
			if ((NtStatus = ReadSMBusValue(0x20, 0x03, FALSE, &uDataValue)) != STATUS_SUCCESS)
			{
				err_Read(_T("DVD TrayState"), NtStatus); 
				return;
			}
			if (uDataValue != 0x10)  // The tray is not open
				break;
			if (CheckAbort(HERE))
				return;
			if (--i == 0)
			{
				ReportDebug(BIT0, _T("Waiting for DVD state to change from tray open"));
				i = 5000/DVD_DELAY_BEWTEEN_POLLS;
			}
			Sleep(DVD_DELAY_BEWTEEN_POLLS);
		}

		// Wait for tray closing to be indicated
		for (i = 0; i < NumLoops; i++)
		{
			if ((NtStatus = ReadSMBusValue(0x20, 0x03, FALSE, &uDataValue)) != STATUS_SUCCESS)
			{
				err_Read(_T("DVD TrayState"), NtStatus); 
				return;
			}
			if ((uDataValue & 0x70) == 0x50)  // Tray closing is indicated by the DVD drive
				break;
			Sleep(DVD_DELAY_BEWTEEN_POLLS);
		}
		if (i == NumLoops)
		{
			continue;
		}

		// Wait for tray closed media detected or tray closed no media detected state to be indicated
		for (i = 0; i < dwMediaDetectNumLoops; i++)
		{
			if ((NtStatus = ReadSMBusValue(0x20, 0x03, FALSE, &uDataValue)) != STATUS_SUCCESS)
			{
				err_Read(_T("DVD TrayState"), NtStatus); 
				return;
			}
			if ((uDataValue == 0x40) || (uDataValue == 0x60)) // The tray is closed
				break;
			Sleep(DVD_DELAY_BEWTEEN_POLLS);
		}
		if (i == dwMediaDetectNumLoops)
		{
			continue;
		}

		if (uDataValue == 0x60) // The media is still in?  Let's debounce this with a no-media detect just to make sure
		{
			// Wait for 1 second for tray closed no media detected state to be indicated
			for (i = 0; i < 1000/DVD_DELAY_BEWTEEN_POLLS; i++)
			{
				if ((NtStatus = ReadSMBusValue(0x20, 0x03, FALSE, &uDataValue)) != STATUS_SUCCESS)
				{
					err_Read(_T("DVD TrayState"), NtStatus); 
					return;
				}
				if (uDataValue == 0x40) // The tray is closed and empty
					break;
				Sleep(DVD_DELAY_BEWTEEN_POLLS);
			}
			if (i == 1000/DVD_DELAY_BEWTEEN_POLLS)
			{
				continue;
			}
		}

		// The tray is closed with no media, so exit with a PASS
		break;
	}
	if (j == 0) // We never saw the tray close without media, so fail
		err_NoDVDMediaNotDetected();


// Test shutdown when the door is open
} // end IMPLEMENT_TESTUNLOCKED (SMC, DVDTrayStatesandEmptyTest, 9)



IMPLEMENT_TESTUNLOCKED (SMC, AirSensorCalibrationUtility, 10)
{
	// AirSensor = 1 => Off
	//	         = 0 => On
	BYTE bAirSensor;
	
	// Set fan speed to desired speed
	// Max fan speed = 25
	NtStatus = WriteSMBusValue(0x20, 0x06, FALSE, 0x19); 
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Speed"), NtStatus); 
		return;
	}

	// Set fan Override to use the requested fan speed
	NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x01);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Override"), NtStatus);
		return;
	}

	Sleep(500);
	// debug
	//ReportDebug(BIT0,_T("Fan at speed 25\n"));	

	bAirSensor = GetAirSensorFromTIU();
	while(bAirSensor != 0)
	{
		bAirSensor = GetAirSensorFromTIU();
		// debug
		ReportDebug(BIT0,_T("AireSensor = %d, implies its Off\n"), bAirSensor);	
	}

	// debug
	ReportDebug(BIT0,_T("Stop Calibration. AireSensor = %d, implies its On\n"), bAirSensor);	

	// Reset fan override
	NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x00);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Override"), NtStatus);
		return;
	}

} // end IMPLEMENT_TESTUNLOCKED (SMC, AirSensorCalibrationUtility, 10)



IMPLEMENT_TESTUNLOCKED (SMC, FanSpeedSystemTest, 11)
{
	BYTE bAirSensor;
	
	// Set fan speed to desired speed
	// Fan speed = 0
	NtStatus = WriteSMBusValue(0x20, 0x06, FALSE, 0x00); 
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Speed"), NtStatus); 
		return;
	}

	// Set fan Override to use the requested fan speed
	NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x01);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Override"), NtStatus);
		return;
	}

	Sleep(15000);
	bAirSensor = GetAirSensorFromTIU();

	// debug
	ReportDebug(BIT0,_T("Fan at speed 0\n"));	
	if(bAirSensor == 0)
		err_AirSensor(00, bAirSensor, _T("On"), 1, _T("Off"));
	// debug
	//else
	//	ReportDebug(BIT0,_T("AirSensor = %d, Off.\n"), bAirSensor);

	// Reset fan override
	NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x00);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Override"), NtStatus);
		return;
	}


	// Max fan speed = 50 
	NtStatus = WriteSMBusValue(0x20, 0x06, FALSE, 0x32); 
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Speed"), NtStatus); 
		return;
	}

	// Set fan Override to use the requested fan speed
	NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x01);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Override"), NtStatus);
		return;
	}

	Sleep(15000);
	bAirSensor = GetAirSensorFromTIU();

	// debug
//	ReportDebug(BIT0,_T("Fan at speed 50\n"));	

	if(bAirSensor == 1)
		err_AirSensor(50, bAirSensor, _T("Off"), 0, _T("On"));
	// debug
	//else
	//	ReportDebug(BIT0,_T("AirSensor = %d, On.\n"), bAirSensor);

	// Reset fan override
	NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x00);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Override"), NtStatus);
		return;
	}
	
} // end IMPLEMENT_TESTUNLOCKED (SMC, FanSpeedSystemTest, 11)



// This utility takes the system into thermal overload. This utility shouldn't be used
// with the regular test package. 
// The fan is turned off and the CPU Junction and Internal Air temperatures are read 
// every 5 secs. As soon as the thermal overload condition is reached, the system will 
// automatically flash the LED and turn on the fan to full speed (50).
IMPLEMENT_TESTUNLOCKED (SMC, ThermalOverloadUtility, 12)
{	
	ULONG uDataValue; 
	// Turn the fan off.
	NtStatus = WriteSMBusValue(0x20, 0x06, FALSE, 0x00); 
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Speed"), NtStatus);  
		return;
	} 

	// Set fan Override to use the requested fan speed
	NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x01);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Override"), NtStatus);
		return;
	}
 
	Sleep(500);
	
	// Keep on reading the CPU Junction and Internal Air Temperatures every 5 secs.
	// As soon as the system reaches thermal overload, it'll shut down, the fan will
	// turn on to full speed and orange LED will flash. After about a minute, the
	// system will completely shut off.
	for (;;)
	{ 
		NtStatus = ReadSMBusValue(0x20, 0x09, FALSE, &uDataValue); 
		ReportStatistic(CPU_JUNCTION_TEMP, _T("%d"), uDataValue);
		NtStatus = ReadSMBusValue(0x20, 0x0A, FALSE, &uDataValue);
		ReportStatistic(INTERNAL_AIR_TEMP, _T("%d"), uDataValue);
		Sleep(5000);
	}

} // end IMPLEMENT_TESTUNLOCKED (SMC, ThermalOverloadUtility, 12)



// This utility keeps on reading the CPU Junction and Internal Air Temperatures every 5 secs.
IMPLEMENT_TESTUNLOCKED (SMC, ReadTempUtility, 13)
{
	ULONG uDataValue;
 
	// Read the CPU Juntion Temp
	NtStatus = ReadSMBusValue(0x20, 0x09, FALSE, &uDataValue); 
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Read(_T("CPU Juntion Temperature"), NtStatus);
		return;
	}
	ReportStatistic(CPU_JUNCTION_TEMP, _T("%d"), uDataValue);

	// Read the Internal Air Temp
	NtStatus = ReadSMBusValue(0x20, 0x0A, FALSE, &uDataValue);
	if(NtStatus != STATUS_SUCCESS)
	{
		err_Read(_T("Internal Air Temperature"), NtStatus);
		return;
	}
	ReportStatistic(INTERNAL_AIR_TEMP, _T("%d"), uDataValue);
	Sleep(5000);

} // end IMPLEMENT_TESTUNLOCKED (SMC, ReadTempUtility, 13)


// This utility shuts down the system.
IMPLEMENT_TESTUNLOCKED (SMC, SystemShutdownUtility, 14)
{
	TCHAR tcBuffer[300]; // Holds data to send to the host in messages
	int iError;
	DWORD dwBufferLength;

	// Use the partial version of iSendHost because a response is not expected
	// No response is received, only message is sent
	// Send MID_UUT_TESTEND command for this test
//	_stprintf(tcBuffer, _T("%s.%s"), GetModuleName(), GetTestName());
	_stprintf(tcBuffer, _T("%s"), GetFullName());
	dwBufferLength = (_tcslen(tcBuffer) + 1) * sizeof(TCHAR);
	// Add the 2 DWORDs here (10 and 1)
	*((DWORD *)&(((char *)tcBuffer)[dwBufferLength])) = 10; // Elapsed time for this test (set to 10 ms)
	dwBufferLength = dwBufferLength + sizeof(DWORD);
	*((DWORD *)&(((char *)tcBuffer)[dwBufferLength])) = 1; // PASSED indication
	dwBufferLength = dwBufferLength + sizeof(DWORD);

	if ((iError = g_host.iSendHost(MID_UUT_TESTEND, (char *)&tcBuffer[0], dwBufferLength))
		 != EVERYTHINGS_OK)
	{
		// Process a communication error
		// This test module just reports the number of the communication error that was received
		err_HostCommunicationError(iError);
		if(CheckAbort(HERE))
			return;
	}
	// Use the partial version of iSendHost because a response is not expected
	// No response is received, only message is sent
	// Send MID_UUT_ENDING command for this test
	if ((iError = g_host.iSendHost(MID_UUT_ENDING, NULL, 0))
		 != EVERYTHINGS_OK)
	{
		// The test has officially ended due to the message above, so don't send any more error messages.
		// Restart the unit below and it will fail on the next boot.
	}

	// Shutdown the system by writing 0x80 to the SMC System Reset Register
	NtStatus = WriteSMBusValue(0x20, 0x02, FALSE, 0x80);
	if(NtStatus != STATUS_SUCCESS)
	{
		// Again, don't report an error because no one is listening
//		err_Reset(NtStatus);
//		return;
	}
	Sleep(INFINITE);

} // end IMPLEMENT_TESTUNLOCKED (SMC, SystemShutdownUtility, 14)

// This utility resets the system.
IMPLEMENT_TESTUNLOCKED (SMC, SystemPowerCycleUtility, 15)
{
	TCHAR tcBuffer[300]; // Holds data to send to the host in messages
	int iError;
	DWORD dwBufferLength;

	// Use the partial version of iSendHost because a response is not expected
	// No response is received, only message is sent
	// Send MID_UUT_TESTEND command for this test
//	_stprintf(tcBuffer, _T("%s.%s"), GetModuleName(), GetTestName());
	_stprintf(tcBuffer, _T("%s"), GetFullName());
	dwBufferLength = (_tcslen(tcBuffer) + 1) * sizeof(TCHAR);
	// Add the 2 DWORDs here (10 and 1)
	*((DWORD *)&(((char *)tcBuffer)[dwBufferLength])) = 10; // Elapsed time for this test (set to 10 ms)
	dwBufferLength = dwBufferLength + sizeof(DWORD);
	*((DWORD *)&(((char *)tcBuffer)[dwBufferLength])) = 1; // PASSED indication
	dwBufferLength = dwBufferLength + sizeof(DWORD);

	if ((iError = g_host.iSendHost(MID_UUT_TESTEND, (char *)&tcBuffer[0], dwBufferLength))
		 != EVERYTHINGS_OK)
	{
		// Process a communication error
		// This test module just reports the number of the communication error that was received
		err_HostCommunicationError(iError);
		if(CheckAbort(HERE))
			return;
	}
	// Use the partial version of iSendHost because a response is not expected
	// No response is received, only message is sent
	// Send MID_UUT_RESTARTING command for this test
	if ((iError = g_host.iSendHost(MID_UUT_RESTARTING, NULL, 0))
		 != EVERYTHINGS_OK)
	{
		// The test has officially ended due to the message above, so don't send any more error messages.
		// Restart the unit below and it will fail on the next boot.
	}

	// Power cycle the system by writing 0x40 to the SMC System Reset Register
	NtStatus = WriteSMBusValue(0x20, 0x02, FALSE, 0x40);
	if(NtStatus != STATUS_SUCCESS)
	{
		// Again, don't report an error because no one is listening
//		err_Reset(NtStatus);
//		return;
	}
	Sleep(INFINITE);

} // end IMPLEMENT_TESTUNLOCKED (SMC, SystemPowerCycleUtility, 15)

IMPLEMENT_TESTUNLOCKED (SMC, RegulateTemperatureUtility, 16)
{
	#define REGULATE_TEMP_UTILITY_WAIT_TIME 5000  // Wait for this many milliseconds between temperature reads
	HANDLE hAbortRegTempUtilityEvent = NULL;
	ULONG uDataValue = 0, uDataValue1 = 0;
	DWORD dwErrorCount = 10;
	DWORD dwCurrentElapsedTime = 0;
	CHostResponse CHR;
	int iError;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost;
	bool bEventSignalled;

	if (dwRegulatedCPUTemperatureFanOff >= dwRegulatedCPUTemperatureFanOn)
	{
		ReportWarning(_T("RegulatedCPUTemperatureFanOff parameter is not less than RegulatedCPUTemperatureFanOn parameter, so aborting utility"));
		return;
	}


	if ((iError = g_host.iSendHost(MID_UUT_GET_ELAPSED_TIME, NULL, NULL, CHR, &dwActualBytesReceived, 
		 &dwErrorCodeFromHost, HOST_TIMEOUT)) != EVERYTHINGS_OK)
	{
		if (dwErrorCodeFromHost != 0)
		{
			// Process error from host
			err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
			return;
		}
		else
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			err_HostCommunicationError(iError);
			return;
		}
	}
	// Don't access the response buffer if it is NULL
	else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived < 4)) 
	{
		// The response buffer is empty, so take appropriate action (like an error message)
		err_BufferIsEmpty();
		return;
	}
	
	else // The response is not NULL, and is not an error message, so process it
	{
		// Copy the data received to a local buffer for processing
		dwCurrentElapsedTime = *(DWORD*)CHR.pvBufferFromHost;  
	}

	// Create a manual-reset event.  Non-signaled to start with
	if ((hAbortRegTempUtilityEvent = CreateEventA(NULL, TRUE, FALSE, REG_TEMP_UTILITY_EVENT_NAME)) == NULL)
	{
		err_CreateEventFailed(REG_TEMP_UTILITY_EVENT_NAME);
		return;
	}

	if ((dwCurrentElapsedTime > dwRegulatedTemperatureMinTimeFanSteady) && (dwCurrentElapsedTime < dwRegulatedTemperatureMaxTimeFanSteady))
	{
		// Fan speed to set
		if ((NtStatus = WriteSMBusValue(0x20, 0x06, FALSE, dwRegulatedTemperatureFanSteadyValue)) != STATUS_SUCCESS)
		{
			err_Write(_T("Fan Speed"), NtStatus); 
		}
		// Set fan Override to use the requested fan speed
		if ((NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x01)) != STATUS_SUCCESS)
		{
			err_Write(_T("Fan Override"), NtStatus);
		}

		// Take an initial reading of the CPU temperature
		if ((NtStatus = ReadSMBusValue(0x20, 0x09, FALSE, &uDataValue)) != STATUS_SUCCESS)
		{
			err_Read(_T("CPU Juntion Temperature"), NtStatus);
		}

		bEventSignalled = false;
		for(;;)
		{
			if (WaitForSingleObject(hAbortRegTempUtilityEvent, REGULATE_TEMP_UTILITY_WAIT_TIME) != WAIT_TIMEOUT)
			{
				bEventSignalled = true;
				break;
			}
			if (CheckAbort(HERE)) // Keep-alive to XMTA
				break;
			if ((NtStatus = ReadSMBusValue(0x20, 0x09, FALSE, &uDataValue)) != STATUS_SUCCESS)
			{
				err_Read(_T("CPU Juntion Temperature"), NtStatus);
				break;
			}
			if ((NtStatus = ReadSMBusValue(0x20, 0x0A, FALSE, &uDataValue1)) != STATUS_SUCCESS)
			{
				err_Read(_T("Internal air Temperature"), NtStatus);
				break;
			}
			ReportDebug(BIT0,_T("CPU temperature: %lu, internal air: %lu"), uDataValue, uDataValue1); // Keep-alive to UHC
		}

		if (bEventSignalled)
		{
			ReportStatistic(CPU_JUNCTION_TEMP, _T("%d"), uDataValue);
			ReportStatistic(INTERNAL_AIR_TEMP, _T("%d"), uDataValue1);
			// Now compare the temperatures to the acceptable ranges
			if((uDataValue < uiCpuJunTempLL) || (uDataValue > uiCpuJunTempUL))
			{
				err_CpuJunctionTemp(uDataValue, uiCpuJunTempLL, uiCpuJunTempUL);
			}

			// Compare the Internal Air Temp with lower and upper limits 
			if((uDataValue1 < uiInternalAirTempLL) || (uDataValue1 > uiInternalAirTempUL))
			{
				err_InternalAirTemp(uDataValue1, uiInternalAirTempLL, uiInternalAirTempUL);
			}
		}
	}
	else // Regulate the temperature normally
	{
		// Max fan speed = 50 
		if ((NtStatus = WriteSMBusValue(0x20, 0x06, FALSE, 0x32)) != STATUS_SUCCESS)
		{
			err_Write(_T("Fan Speed"), NtStatus); 
		}
		// Set fan Override to use the requested fan speed
		if ((NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x01)) != STATUS_SUCCESS)
		{
			err_Write(_T("Fan Override"), NtStatus);
		}

		// Take an initial reading of the CPU temperature
		if ((NtStatus = ReadSMBusValue(0x20, 0x09, FALSE, &uDataValue)) != STATUS_SUCCESS)
		{
			err_Read(_T("CPU Juntion Temperature"), NtStatus);
		}
		for(;;)
		{
			// Now wait for the temperature to drop below the off temperature
			while (uDataValue > dwRegulatedCPUTemperatureFanOff)
			{
				if (WaitForSingleObject(hAbortRegTempUtilityEvent, REGULATE_TEMP_UTILITY_WAIT_TIME) != WAIT_TIMEOUT)
					break;
				if (CheckAbort(HERE)) // Keep-alive to XMTA
					break;
				if ((NtStatus = ReadSMBusValue(0x20, 0x09, FALSE, &uDataValue)) != STATUS_SUCCESS)
				{
					err_Read(_T("CPU Juntion Temperature"), NtStatus);
					break;
				}
				if (uDataValue > uiCpuJunTempCritical)
				{
					if (dwErrorCount > 0)
					{
						dwErrorCount--;
						err_CpuJunctionTempHigh(uDataValue, uiCpuJunTempCritical);
					}
				}

				NtStatus = ReadSMBusValue(0x20, 0x0A, FALSE, &uDataValue1);
				ReportDebug(BIT0,_T("CPU temperature = %lu, internal air = %lu"), uDataValue, uDataValue1); // Keep-alive to UHC
			}
			if (uDataValue > dwRegulatedCPUTemperatureFanOff) // Exited for a reason other than temp satisfied
				break;

			// Reset fan override
			if ((NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x00)) != STATUS_SUCCESS)
			{
				err_Write(_T("Fan Override"), NtStatus);
				break;
			}
			// Turn fan off
			if ((NtStatus = WriteSMBusValue(0x20, 0x06, FALSE, 0)) != STATUS_SUCCESS)
			{
				err_Write(_T("Fan Speed"), NtStatus); 
				break;
			}
			// Set fan Override to use the requested fan speed
			if ((NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x01)) != STATUS_SUCCESS)
			{
				err_Write(_T("Fan Override"), NtStatus);
				break;
			}
			// Now wait for the temperature to rise above the on temperature
			while (uDataValue < dwRegulatedCPUTemperatureFanOn)
			{
				if (WaitForSingleObject(hAbortRegTempUtilityEvent, REGULATE_TEMP_UTILITY_WAIT_TIME) != WAIT_TIMEOUT)
					break;
				if (CheckAbort(HERE)) // Keep-alive to XMTA
					break;
				if ((NtStatus = ReadSMBusValue(0x20, 0x09, FALSE, &uDataValue)) != STATUS_SUCCESS)
				{
					err_Read(_T("CPU Juntion Temperature"), NtStatus);
					break;
				}
				NtStatus = ReadSMBusValue(0x20, 0x0A, FALSE, &uDataValue1);
				ReportDebug(BIT0,_T("CPU temperature = %lu, internal air = %lu"), uDataValue, uDataValue1); // Keep-alive to UHC
			}
			if (uDataValue < dwRegulatedCPUTemperatureFanOn) // Exited for a reason other than temp satisfied
				break;
			// Reset fan override
			if ((NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x00)) != STATUS_SUCCESS)
			{
				err_Write(_T("Fan Override"), NtStatus);
				break;
			}
			// Kick fan on full speed
			if ((NtStatus = WriteSMBusValue(0x20, 0x06, FALSE, 0x32)) != STATUS_SUCCESS)
			{
				err_Write(_T("Fan Speed"), NtStatus); 
				break;
			}
			// Set fan Override to use the requested fan speed
			if ((NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x01)) != STATUS_SUCCESS)
			{
				err_Write(_T("Fan Override"), NtStatus);
				break;
			}
		}
	}

	// Reset fan override
	if ((NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x00)) != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Override"), NtStatus);
	}
	// Kick fan on full speed
	if ((NtStatus = WriteSMBusValue(0x20, 0x06, FALSE, 0x32)) != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Speed"), NtStatus); 
	}
	// Set fan Override to use the requested fan speed
	if ((NtStatus = WriteSMBusValue(0x20, 0x05, FALSE, 0x01)) != STATUS_SUCCESS)
	{
		err_Write(_T("Fan Override"), NtStatus);
	}

	CloseHandle(hAbortRegTempUtilityEvent);
} // end IMPLEMENT_TESTUNLOCKED (SMC, RegulateTemperatureUtility, 16)

IMPLEMENT_TESTUNLOCKED (SMC, AbortRegulateTemperatureUtility, 17)
{
	HANDLE hAbortRegTempUtilityEvent = NULL;

	// Create a manual-reset event.  Non-signaled to start with
	if ((hAbortRegTempUtilityEvent = CreateEventA(NULL, TRUE, FALSE, REG_TEMP_UTILITY_EVENT_NAME)) == NULL)
	{
		err_CreateEventFailed(REG_TEMP_UTILITY_EVENT_NAME);
		return;
	}

	SetEvent(hAbortRegTempUtilityEvent);

	CloseHandle(hAbortRegTempUtilityEvent);
} // end IMPLEMENT_TESTUNLOCKED (SMC, AbortRegulateTemperatureUtility, 17)

// This utility resets the system if the reboot count is not a multiple of a specified
// configuration parameter.  Otherwise it passes and allows other tests to execute
IMPLEMENT_TESTUNLOCKED (SMC, BootCountGateUtility, 18)
{
	TCHAR tcBuffer[300]; // Holds data to send to the host in messages
	int iError;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost;
	DWORD dwBufferLength, dwBootCountGateValue, dwCurrentBootCount;
	CHostResponse CHR;


	if (gdwBootCountGateValue > 1) // Just PASS this utility if the boot count gate value is not greater than 1
	{
#ifdef RNR_MODE
		dwBootCountGateValue = gdwBootCountGateValue; // There is 1 boot count per test boot: just one for the test
#else
		dwBootCountGateValue = gdwBootCountGateValue*2; // There are 2 boot counts per test boot: one for the DVD, and one for the test
#endif
		// Retrieve the Boot count from the host
		// Use the full version of iSendHost because a response is expected.
		// No message is sent, only response is received after writing to the LED state register.
		// This message will receive a number indicating the state of the sensors on the XBOX.
		if ((iError = g_host.iSendHost(MID_UUT_GET_BOOT_COUNT, NULL, NULL, CHR, &dwActualBytesReceived, 
			 &dwErrorCodeFromHost, HOST_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
				return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				err_HostCommunicationError(iError);
				return;
			}
		}
		// Don't access the response buffer if it is NULL
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived < 4)) 
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			err_BufferIsEmpty();
			return;
		}
		
		else // The response is not NULL, and is not an error message, so process it
		{
			// Copy the data received to a local buffer for processing
			dwCurrentBootCount = *(DWORD*)CHR.pvBufferFromHost;  
			ReportStatistic(BOOT_COUNT, _T("%ld"), dwCurrentBootCount);
#ifdef RNR_MODE
			dwCurrentBootCount = (dwCurrentBootCount + 1);
#else
			dwCurrentBootCount = (dwCurrentBootCount + 1) & 0xFFFFFFFE;  // Make it an even number
#endif
			if (((dwCurrentBootCount + dwBootCountGateValue) % dwBootCountGateValue) != 0)
			{
				// Use the partial version of iSendHost because a response is not expected
				// No response is received, only message is sent
				// Send MID_UUT_TESTEND command for this test
				_stprintf(tcBuffer, _T("%s"), GetFullName());
				dwBufferLength = (_tcslen(tcBuffer) + 1) * sizeof(TCHAR);
				// Add the 2 DWORDs here (10 and 1)
				*((DWORD *)&(((char *)tcBuffer)[dwBufferLength])) = 10; // Elapsed time for this test (set to 10 ms)
				dwBufferLength = dwBufferLength + sizeof(DWORD);
				*((DWORD *)&(((char *)tcBuffer)[dwBufferLength])) = 1; // PASSED indication
				dwBufferLength = dwBufferLength + sizeof(DWORD);

				if ((iError = g_host.iSendHost(MID_UUT_TESTEND, (char *)&tcBuffer[0], dwBufferLength))
					 != EVERYTHINGS_OK)
				{
					// Process a communication error
					// This test module just reports the number of the communication error that was received
					err_HostCommunicationError(iError);
					if(CheckAbort(HERE))
						return;
				}
				// Use the partial version of iSendHost because a response is not expected
				// No response is received, only message is sent
				// Send MID_UUT_RESTARTING command for this test
				if ((iError = g_host.iSendHost(MID_UUT_RESTARTING, NULL, 0))
					 != EVERYTHINGS_OK)
				{
					// The test has officially ended due to the message above, so don't send any more error messages.
					// Restart the unit below and it will fail on the next boot.
				}

				// Power cycle the system by writing 0x40 to the SMC System Reset Register
				NtStatus = WriteSMBusValue(0x20, 0x02, FALSE, 0x40);
				if(NtStatus != STATUS_SUCCESS)
				{
					// Again, don't report an error because no one is listening
				}
				Sleep(INFINITE);
			}
		}
	}
}

IMPLEMENT_TESTUNLOCKED (SMC, DVDSpinDownUtility, 19)
{
	#define DVD_DELAY_BEWTEEN_POLLS 20  // Number of milliseconds to sleep between polls of SMC registers
	ULONG uDataValue, i, NumLoops; 
	TCHAR tcBuffer[300]; // Holds data to send to the host in messages
	int iError;
	DWORD dwBufferLength;
//	HANDLE hStartDVDSpinDownUtilityEvent = NULL;

	// Create a manual-reset event.  Non-signaled to start with
//	if ((hStartDVDSpinDownUtilityEvent = CreateEventA(NULL, TRUE, FALSE, DVD_SPIN_DOWN_UTILITY_EVENT_NAME)) == NULL)
//	{
//		err_CreateEventFailed(DVD_SPIN_DOWN_UTILITY_EVENT_NAME);
//		return;
//	}

	NumLoops = DVD_TIMEOUT_BETWEEN_OPERATIONS/DVD_DELAY_BEWTEEN_POLLS;
	// Wait for detection of media in the DVD drive
	for (i = 0; i < NumLoops; i++)
	{
		if ((NtStatus = ReadSMBusValue(0x20, 0x03, FALSE, &uDataValue)) != STATUS_SUCCESS)
		{
			err_Read(_T("DVD TrayState"), NtStatus); 
			return;
		}
		if (uDataValue == 0x60)  // Media is detected with the tray closed
			break;
		Sleep(DVD_DELAY_BEWTEEN_POLLS);
	}
	if (i == NumLoops)
	{
		err_NoDVDMediaDetected();
		return;
	}

	// Use the partial version of iSendHost because a response is not expected
	// No response is received, only message is sent
	// Send MID_UUT_TESTEND command for this test
	_stprintf(tcBuffer, _T("%s"), GetFullName());
	dwBufferLength = (_tcslen(tcBuffer) + 1) * sizeof(TCHAR);
	// Add the 2 DWORDs here (10 and 1)
	*((DWORD *)&(((char *)tcBuffer)[dwBufferLength])) = 10; // Elapsed time for this test (set to 10 ms)
	dwBufferLength = dwBufferLength + sizeof(DWORD);
	*((DWORD *)&(((char *)tcBuffer)[dwBufferLength])) = 1; // PASSED indication
	dwBufferLength = dwBufferLength + sizeof(DWORD);

	if ((iError = g_host.iSendHost(MID_UUT_TESTEND, (char *)&tcBuffer[0], dwBufferLength))
		 != EVERYTHINGS_OK)
	{
		// Process a communication error
		// This test module just reports the number of the communication error that was received
		err_HostCommunicationError(iError);
		if(CheckAbort(HERE))
			return;
	}
	// Use the partial version of iSendHost because a response is not expected
	// No response is received, only message is sent
	// Send MID_UUT_ENDING command for this test
	if ((iError = g_host.iSendHost(MID_UUT_ENDING, NULL, 0))
		 != EVERYTHINGS_OK)
	{
		// The test has officially ended due to the message above, so don't send any more error messages.
		// Restart the unit below and it will fail on the next boot.
	}

	// Issue an Eject Command to open the DVD drive
	if ((NtStatus = WriteSMBusValue(0x20, 0x0C, FALSE, 0x00)) != STATUS_SUCCESS)
	{
//		err_Write(_T("DVD Tray Operation"), NtStatus); 
//		return;
	}

	// Wait for the tray to start opening
	for (i = 0; i < NumLoops; i++)
	{
		if ((NtStatus = ReadSMBusValue(0x20, 0x03, FALSE, &uDataValue)) != STATUS_SUCCESS)
		{
//			err_Read(_T("DVD TrayState"), NtStatus); 
//			return;
		}
		if (uDataValue == 0x31)  // The opening state
			break;
		Sleep(DVD_DELAY_BEWTEEN_POLLS);
	}
	if (i == NumLoops)
	{
//		err_DVDTrayNotOpen();
//		return;
	}
	
	// Issue a tray close Command to close the DVD drive
	if ((NtStatus = WriteSMBusValue(0x20, 0x0C, FALSE, 0x01)) != STATUS_SUCCESS)
	{
//		err_Write(_T("DVD Tray Operation"), NtStatus); 
//		return;
	}

	// Shutdown the system by writing 0x80 to the SMC System Reset Register
	NtStatus = WriteSMBusValue(0x20, 0x02, FALSE, 0x80);
	if(NtStatus != STATUS_SUCCESS)
	{
		// Again, don't report an error because no one is listening
//		err_Reset(NtStatus);
//		return;
	}
	Sleep(INFINITE);
} // end IMPLEMENT_TESTUNLOCKED (SMC, DVDSpinDownUtility, 19)

IMPLEMENT_TESTUNLOCKED (SMC, DVDOpenCloseMediaDetectTest, 20)
{
	#define DVD_DELAY_BEWTEEN_POLLS 20  // Number of milliseconds to sleep between polls of SMC registers
	ULONG uDataValue, i, NumLoops; 
	DWORD dwMediaDetectNumLoops;

	NumLoops = DVD_TIMEOUT_BETWEEN_OPERATIONS/DVD_DELAY_BEWTEEN_POLLS;
	dwMediaDetectNumLoops = dwDVDMediaDetectTimeout/DVD_DELAY_BEWTEEN_POLLS;
	// Wait for detection of media in the DVD drive
	for (i = 0; i < NumLoops; i++)
	{
		if ((NtStatus = ReadSMBusValue(0x20, 0x03, FALSE, &uDataValue)) != STATUS_SUCCESS)
		{
			err_Read(_T("DVD TrayState"), NtStatus); 
			return;
		}
		if (uDataValue == 0x60)  // Media is detected with the tray closed
			break;
		Sleep(DVD_DELAY_BEWTEEN_POLLS);
	}
	if (i == NumLoops)
	{
		err_NoDVDMediaDetected();
		return;
	}

	// Issue an Eject Command to open the DVD drive
	if ((NtStatus = WriteSMBusValue(0x20, 0x0C, FALSE, 0x00)) != STATUS_SUCCESS)
	{
		err_Write(_T("DVD Tray Operation"), NtStatus); 
		return;
	}

	// Wait for the tray to start opening
	for (i = 0; i < NumLoops; i++)
	{
		if ((NtStatus = ReadSMBusValue(0x20, 0x03, FALSE, &uDataValue)) != STATUS_SUCCESS)
		{
			err_Read(_T("DVD TrayState"), NtStatus); 
			return;
		}
		if ((uDataValue == 0x31) || (uDataValue == 0x10))  // The opening or the open state
			break;
		Sleep(DVD_DELAY_BEWTEEN_POLLS);
	}
	if (i == NumLoops)
	{
		err_DVDTrayNotOpen();
		return;
	}
	
	// Issue a tray close Command to close the DVD drive
	if ((NtStatus = WriteSMBusValue(0x20, 0x0C, FALSE, 0x01)) != STATUS_SUCCESS)
	{
		err_Write(_T("DVD Tray Operation"), NtStatus); 
		return;
	}
	// Wait for tray closed media detected or tray closed no media detected state to be indicated
	for (i = 0; i < dwMediaDetectNumLoops; i++)
	{
		if ((NtStatus = ReadSMBusValue(0x20, 0x03, FALSE, &uDataValue)) != STATUS_SUCCESS)
		{
			err_Read(_T("DVD TrayState"), NtStatus); 
			return;
		}
		if (uDataValue == 0x60) // The tray is closed and media is detected
			break;
		Sleep(DVD_DELAY_BEWTEEN_POLLS);
	}
	if (i == dwMediaDetectNumLoops)
	{
		err_NoDVDMediaDetected();
		return;
	}
	return;
} // end IMPLEMENT_TESTUNLOCKED (SMC, DVDOpenCloseMediaDetectTest, 20)

void CXModuleSMC::vNewSeed(UINT *pSeed)
{
	SYSTEMTIME systimeSeed;
	FILETIME filetimeSeed;

	GetSystemTime(&systimeSeed);
	SystemTimeToFileTime(&systimeSeed, &filetimeSeed);
	*pSeed = (UINT)filetimeSeed.dwLowDateTime;

	return;
}

DWORD CXModuleSMC::random(UINT *pScratch)
{
	DWORD ret;
	ret  = (*pScratch = *pScratch * 214013L + 2531011L) >> 16;
	ret |= (*pScratch = *pScratch * 214013L + 2531011L) & 0xFFFF0000;
	return ret;
}
