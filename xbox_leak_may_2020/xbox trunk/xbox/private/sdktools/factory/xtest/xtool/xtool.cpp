// (c) Copyright 2000-2001 Intel Corp. All rights reserved.
//
// Title: xTool
//
// History:
//
// 01/24/01 V1.00 BLI Release V1.00.

#include "..\stdafx.h"
#include "..\testobj.h"
#include "..\xboxkeys.h"
#include "..\statkeys.h"
#include "xtool.h"
#ifdef _XBOX
#include "perboxdata.h"

extern "C"
{
#include "av.h"
}
#endif

IMPLEMENT_MODULEUNLOCKED (CXModuleTool);

bool CXModuleTool::InitializeParameters ()
{
	if (!CTestObj::InitializeParameters ())
		return false;

	dwRandomNumberLength = GetCfgUint (L"random_number_length", 32);
	
	//Get the display parameters - there are a lot of them...
	//Get parameters from ini file for tests.
	//Required parameters.
	m_vidMode = GetCfgInt( L"videomode", -1);
	m_TIUStatus = GetCfgString( L"tiu", NULL);
	m_pack = GetCfgString( L"pack", NULL);
	m_standard = GetCfgString( L"standard", NULL);
	//50 or 60 Hz rate must be specified.
	m_flagRefreshRate = GetCfgInt( L"refresh_rate", 0);
	
	//Optional flags.
	m_HDTVmode = GetCfgString( L"hdtv_mode", NULL);
	m_flagWidescreen = GetCfgInt( L"widescreen", 0);
#ifdef _XBOX		
	D3D__AvInfo = 0x00000000;
#endif	
	return true;
}

//******************************************************************
// Title: get_random_number
//
// Abstract: Gets a random number of a specified length from the host
//
// Uses config parameters:
//           random_number_length
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, get_random_number, 1)
{
	CHostResponse CHR;
	DWORD dwActualBytesReceived = 0, dwErrorCodeFromHost = 0;
	int iError;

	// Use the full version of iSendHost because a response is expected
	if ((iError = g_host.iSendHost(MID_XBOX_RANDOM_STRING, (char *)&dwRandomNumberLength, sizeof(DWORD), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
	{
		if (dwErrorCodeFromHost != 0)
		{
			// Process error from host
			if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
				return;
		}
		else
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			if (err_HostCommunicationError(iError))
				return;
		}
	}
	else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
	{
		// The response buffer is empty, so take appropriate action (like an error message)
		if (err_RandomBufferIsEmpty())
			return;
	}
	else // The response is not NULL, and is not an error message, so process it
	{
		if (dwActualBytesReceived != dwRandomNumberLength)
		{
			if (err_RandomBufferIsEmpty())
				return;
		}
		// We received the random number buffer the size that we wanted

		// For now, just display the numbers
		for (DWORD q = 0; q < dwRandomNumberLength; q++)
			ReportDebug(BIT0, L"Byte %2.2lu = 0x%2.2lx\n", q, (DWORD)(*(((BYTE *)CHR.pvBufferFromHost) + q)));
	}
}


//******************************************************************
// Title: set_display_mode
//
// Abstract: Sets the display outmode of the device under test.
//			Also used to send a message to the host to instruct the
//			interface board to change it's output configuration accordingly.
//
// Uses config parameters:
//           
//******************************************************************
IMPLEMENT_TESTLOCKED (Tool, set_display_mode, 2)
{
#ifdef _XBOX
	if(m_vidMode < 0)
	{
		err_BADPARAMETER(L"videomode");
		return;
	}
	if(m_TIUStatus == NULL)
	{
		err_BADPARAMETER(L"tiu");
		return;
	}
	if(m_pack == NULL)
	{
		err_BADPARAMETER(L"pack");
		return;
	}
	//if(m_standard == NULL)
	//{
	//	err_BADPARAMETER(L"standard");
	//	return;
	//}
	//50 or 60 Hz rate must be specified.
	if(m_flagRefreshRate == 0)
	{
		err_BADPARAMETER(L"refresh_rate");
		return;
	}

	//Set the display mode parameters to create the DX8 device/surface interface.
	if(!InitializeDisplaySettings(m_vidMode))
		return;
	
#endif	
}


//******************************************************************
// Title: InitializeDisplaySettings(int mode, int backBufferFormat)
//
// Abstract: Sets the display outmode of the device under test.
//			Also used to send a message to the host to instruct the
//			interface board to change it's output configuration accordingly.
//
// Uses config parameters:
//           
//******************************************************************
#ifdef _XBOX
bool CXModuleTool::InitializeDisplaySettings(int mode)
{
	HRESULT hr;
	
	//Set up the parameters that are global to any setting to be used.
	ZeroMemory( &g_d3dpp, sizeof(g_d3dpp) );
	// Create one backbuffer and a zbuffer
	g_d3dpp.BackBufferCount        = 1;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    g_d3dpp.FullScreen_RefreshRateInHz = m_flagRefreshRate;
	g_d3dpp.BackBufferFormat = D3DFMT_LIN_X8R8G8B8;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	if(m_HDTVmode != NULL)
	{
		if(!wcsicmp(m_HDTVmode, L"1080i"))
		{
			g_d3dpp.FullScreen_PresentationInterval	= 0;//Zero value needed for 1080i sync issues.
			g_d3dpp.Flags |= D3DPRESENTFLAG_INTERLACED;
		}
		else
		{
			g_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			g_d3dpp.Flags |= D3DPRESENTFLAG_PROGRESSIVE;
		}
	}
	else
		g_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	g_d3dpp.Flags |= m_flagWidescreen ? D3DPRESENTFLAG_WIDESCREEN : 0;
	g_d3dpp.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	
	//Clear the AVPACK FLAG settings before resetting the device.
	D3D__AvInfo = 0x00000000;
	AvSendTVEncoderOption(NULL, AV_OPTION_ZERO_MODE, 0, NULL);
	
	switch(mode)
	{
		case 1://640x480 32Bpp (NTSC/PAL)
			g_d3dpp.BackBufferWidth   = 640;
			g_d3dpp.BackBufferHeight  = 480;
			break;

		case 2://704x480 32Bpp (NTSC/PAL)
			g_d3dpp.BackBufferWidth   = 704;
			g_d3dpp.BackBufferHeight  = 480;
			break;

		case 3://720x480 32Bpp (NTSC/PAL/HDTV_480p)
			g_d3dpp.BackBufferWidth   = 720;
			g_d3dpp.BackBufferHeight  = 480;
			break;
				
		case 4://640x576 32Bpp (PAL/SECAM)
			g_d3dpp.BackBufferWidth   = 640;
			g_d3dpp.BackBufferHeight  = 576;
			break;

		case 5://704x576 32Bpp (PAL/SECAM)
			g_d3dpp.BackBufferWidth   = 704;
			g_d3dpp.BackBufferHeight  = 576;
			break;

		case 6://720x576 32Bpp (PAL/SECAM)
			g_d3dpp.BackBufferWidth   = 720;
			g_d3dpp.BackBufferHeight  = 576;
			break;

		case 7://1280x720 32Bpp (HDTV_720p)
			g_d3dpp.BackBufferWidth   = 1280;
			g_d3dpp.BackBufferHeight  = 720;
			break;

		case 8://1920x1080 32Bpp (HDTV_1080i)
			g_d3dpp.BackBufferWidth   = 1920;
			g_d3dpp.BackBufferHeight  = 1080;
			break;

		default:
			//No valid display mode detected - return an error.
			err_SetDisplayMode(L"Invalid display mode specified in mode parameter");
			return FALSE;
			break;
	}


	//Now determine the flag settings for the AV Pack from config parameters.
	//Determine the standard flags to set.
	if(!wcsicmp(m_pack, L"composite"))
	{
		D3D__AvInfo |= AV_PACK_STANDARD;
		m_AVMode = AVMODENTSCCOMP;
	}
	else if(!wcsicmp(m_pack, L"svideo"))
	{
		D3D__AvInfo |= AV_PACK_SVIDEO;
		m_AVMode = AVMODENTSCENH;
	}
	else if(!wcsicmp(m_pack, L"rfu"))
	{
		D3D__AvInfo |= AV_PACK_RFU;
		m_AVMode = AVMODEDISABLE;
	}
	else if(!wcsicmp(m_pack, L"scart"))
	{
		D3D__AvInfo |= AV_PACK_SCART;
		m_AVMode = AVMODESCART;
	}
	else if(!wcsicmp(m_pack, L"hdtv"))
	{
		D3D__AvInfo |= AV_PACK_HDTV;
		m_AVMode = AVMODEHDTV;
	}
	else if(!wcsicmp(m_pack, L"vga"))
	{
		D3D__AvInfo |= AV_PACK_VGA;
		m_AVMode = AVMODEVGA;
	}
	else
	{
		err_BADPARAMETER(L"pack");
		return FALSE;
	}
	
	//Determine the region flags to set.
	if(m_standard != NULL)
	{
		if(!wcsicmp(m_standard, L"ntsc_m"))
			D3D__AvInfo |= AV_STANDARD_NTSC_M;
		else if(!wcsicmp(m_standard, L"ntsc_j"))
			D3D__AvInfo |= AV_STANDARD_NTSC_J;
		else if(!wcsicmp(m_standard, L"pal_i"))
			D3D__AvInfo |= AV_STANDARD_PAL_I;
		else if(!wcsicmp(m_standard, L"pal_m"))
			D3D__AvInfo |= AV_STANDARD_PAL_M;
		else
		{
			err_BADPARAMETER(L"standard");
			return FALSE;
		}
	}
	else
		D3D__AvInfo |= AV_STANDARD_MASK;

	//Determine the refresh rate.
	if(m_flagRefreshRate == 50)
		D3D__AvInfo |= AV_FLAGS_50Hz;
	else if(m_flagRefreshRate == 60)
		D3D__AvInfo |= AV_FLAGS_60Hz;
	else
	{
		err_BADPARAMETER(L"refresh_rate");
		return FALSE;
	}
	
	//Optional parameters.
	//Determine the desired HDTV mode to output.
	if(m_HDTVmode != NULL)//If null, then optional and not needed (unless it was left out by mistake!).
	{
		if(!wcsicmp(m_HDTVmode, L"480p"))
			D3D__AvInfo |= AV_FLAGS_HDTV_480p;
		else if(!wcsicmp(m_HDTVmode, L"720p"))
			D3D__AvInfo |= AV_FLAGS_HDTV_720p;
		else if(!wcsicmp(m_HDTVmode, L"1080i"))
		{
			D3D__AvInfo |= AV_FLAGS_HDTV_1080i;
			D3D__AvInfo |= AV_FLAGS_INTERLACED;
		}
		else
		{
			err_BADPARAMETER(L"hdtv_mode");
			return FALSE;
		}
	}

	//Do we want widescreen unlocked?
	if(m_flagWidescreen)
		D3D__AvInfo |= AV_FLAGS_WIDESCREEN;
	
	
	//If enabled, setup the TIU board for the correct output mode.	
	ReportDebug(0x1, L"TIU Status=%s", m_TIUStatus);
	if(!wcsicmp(m_TIUStatus, L"enabled"))
	{
		if(!InitializeInterfaceBoard(m_AVMode))
			return FALSE;
	}
		

   //OK - now all the required parameters are set to create a device.  Let's call the
	//Reset() function to clear out and create the newly defined DX8 device.
	ReportDebug(0x1, L"Resetting D3D Device...");
	hr = g_pd3dDevice->Reset(&g_d3dpp);
	if(hr != D3D_OK)
	{
		if(hr == D3DERR_INVALIDCALL)
		{
			err_D3DError(L"d3dDevice->Reset() - INVALID CALL: Method parameter may have invalid value.");
			return FALSE;
		}
		else if(hr == D3DERR_OUTOFVIDEOMEMORY)
		{
			err_D3DError(L"d3dDevice->Reset() - OUT_OF_VIDEOMEMORY: Not enough display memory to perform the operation.");
			return FALSE;
		}
		else if(hr == E_OUTOFMEMORY)
		{
			err_D3DError(L"d3dDevice->Reset() - E_OUTOFMEMORY: Could not allocate enough memory to complete the call.");
			return FALSE;
		}
		else
		{
			err_D3DError(L"d3dDevice->Reset() - ERROR: Unknown return value.");
			return FALSE;
		}

	}

	//Output the current display mode flag settings.
	ReportVideoSettings();

	return TRUE;
}
#endif


//******************************************************************
// Title: InitializeInterfaceBoard()
//
// Abstract: Sends a message to the host to set the appropriate output
//			circuits for the required av output mode. 
//
// Uses config parameters:
//
//					pack
//					(i.e. pack=sdtv sets output mode for composite and svideo)         
//******************************************************************
#ifdef _XBOX
bool CXModuleTool::InitializeInterfaceBoard(BYTE avMode)
{
	CHostResponse CHR;
	BYTE *send_buf = NULL;
	DWORD dwBufferToHostLength;
	int iError;
	
	send_buf = &avMode;
	dwBufferToHostLength = 1; //The avmode command is one byte long.

	ReportDebug(0x1, L"Sending message to set the TIU AVmode parameters (mode=%d).\n",avMode);
	// Use the short version of iSendHost because no response is expected
	//if ((iError = g_host.iSendHost(MID_XBOX_SET_VIDEO_MUX, send_buf, dwBufferToHostLength)) != EVERYTHINGS_OK)
	if ((iError = g_host.iSendHost(MID_XBOX_SET_VIDEO_MUX, send_buf, dwBufferToHostLength)) != EVERYTHINGS_OK)
	{
		// Process a communication error
		// This test module just reports the number of the communication error that was received
		err_HostCommunicationError(iError);
		return FALSE;
	}
	return TRUE;
}
#endif

//******************************************************************
// Title: ReportVideoSettings
//
// Abstract: Outputs the interpreted display mode settings.
//			Useful for debugging graphics and video problems.
//
// Uses config parameters:
//           
//******************************************************************
#ifdef _XBOX
void CXModuleTool::ReportVideoSettings()
{
	//Determine what display mode flags have been set.
	ReportDebug(0x1, L"The DX8 Device has been reset with the following settings:\n");
	ReportDebug(0x1, _T("AV Pack Flag Settings = 0x%x\n"), D3D__AvInfo);
		
	if(!wcsicmp(m_pack, L"composite"))
		ReportDebug(0x1, _T("AV_PACK_STANDARD Flag set TRUE.\n"));
	else if(!wcsicmp(m_pack, L"svideo"))
		ReportDebug(0x1, _T("AV_PACK_SVIDEO Flag set TRUE.\n"));
	else if(!wcsicmp(m_pack, L"rfu"))
		ReportDebug(0x1, _T("AV_PACK_RFU Flag set TRUE.\n"));
	else if(!wcsicmp(m_pack, L"scart"))
		ReportDebug(0x1, _T("AV_PACK_SCART Flag set TRUE.\n"));
	else if(!wcsicmp(m_pack, L"hdtv"))
		ReportDebug(0x1, _T("AV_PACK_HDTV Flag set TRUE.\n"));
	else if(!wcsicmp(m_pack, L"vga"))
		ReportDebug(0x1, _T("AV_PACK_VGA Flag set TRUE.\n"));
	
	if(m_standard != NULL)
	{
		if(!wcsicmp(m_standard, L"ntsc_m"))
			ReportDebug(0x1, _T("AV_STANDARD_NTSC_M Flag set TRUE.\n"));
		else if(!wcsicmp(m_standard, L"ntsc_j"))
			ReportDebug(0x1, _T("AV_STANDARD_NTSC_J Flag set TRUE.\n"));
		else if(!wcsicmp(m_standard, L"pal_i"))
			ReportDebug(0x1, _T("AV_STANDARD_PAL_I Flag set TRUE.\n"));
		else if(!wcsicmp(m_standard, L"pal_m"))
			ReportDebug(0x1, _T("AV_STANDARD_PAL_M Flag set TRUE.\n"));
	}
	if(D3D__AvInfo & AV_FLAGS_HDTV_480p)
		ReportDebug(0x1, _T("AV_FLAGS_HDTV_480p Flag set TRUE.\n"));
	
	if(D3D__AvInfo & AV_FLAGS_HDTV_720p)
		ReportDebug(0x1, _T("AV_FLAGS_HDTV_720p Flag set TRUE.\n"));
	
	if(D3D__AvInfo & AV_FLAGS_HDTV_1080i)
		ReportDebug(0x1, _T("AV_FLAGS_HDTV_1080i Flag set TRUE.\n"));
	
	if(D3D__AvInfo & AV_FLAGS_WIDESCREEN)
		ReportDebug(0x1, _T("AV_FLAGS_WIDESCREEN Flag set TRUE.\n"));
	
	if(D3D__AvInfo & AV_FLAGS_60Hz)
		ReportDebug(0x1, _T("AV_FLAGS_60Hz Flag set TRUE.\n"));

	if(D3D__AvInfo & AV_FLAGS_50Hz)
		ReportDebug(0x1, _T("AV_FLAGS_50Hz Flag set TRUE.\n"));

	//Now output all of the surface description settings.
	ReportDebug(0x1, _T("The display has been set to mode %d (%lu x %lu 32Bpp).\n"),
									m_vidMode,
									g_d3dpp.BackBufferWidth,
									g_d3dpp.BackBufferHeight);

}
#endif

IMPLEMENT_TESTUNLOCKED (Tool, find_free_memory, 3)
{
	BYTE *pByte = NULL;
	DWORD dwSize, dwLastGood, dwIncrement;
	bool bSuccess;


	dwLastGood = 0;
	dwSize = 0x4000000; // 64 MB
	dwIncrement = dwSize/2;

	while (dwIncrement > 0x20)
	{
		bSuccess = false;
		try
		{
			if ((pByte = new BYTE[dwSize]) != NULL)
			{
				ReportDebug(BIT1, L"Allocated %lu bytes, I = %8.8lx", dwSize, dwIncrement);
				bSuccess = true;
			}
			else
			{
				ReportDebug(BIT1, L"Failed to allocate %lu bytes, I = %8.8lx", dwSize, dwIncrement);
			}
		}
		catch (CXmtaException &e)
		{
			UNREFERENCED_PARAMETER(e);
			ReportDebug(BIT1, L"Failed to allocate %lu bytes, I = %8.8lx", dwSize, dwIncrement);
		}
		if (bSuccess)
		{
			memset(pByte, 0x55, dwSize);
			delete [] pByte;
			pByte = NULL;
			dwLastGood = dwSize;
			dwSize = dwSize + dwIncrement;
		}
		else
		{
			dwSize = dwSize - dwIncrement;
		}
		dwIncrement = dwIncrement/2;
	}
	ReportDebug(BIT0, L"Allocated %lu bytes, I = %8.8lx", dwLastGood, dwIncrement);
}

//******************************************************************
// Title: write_per_box_information
//
// Abstract: This utility reads values specific to each XBox from the
//           factory control database, calls a utility on the host computer
//           to generate per-box keys based on this information, records
//           the generated keys back in the factory control database, and
//           calls a function to write per-box data to the eeprom on the XBox.
//
// Uses config parameters:
//           none
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, write_per_box_information, 4)
{
#ifdef _XBOX
	#define GAME_REGION_CODE_SIZE      8
	#define TV_REGION_CODE_SIZE        8
	#define MAC_ADDRESS_SIZE          12
	#define SERIAL_NUMBER_SIZE        20
	#define SERIAL_NUMBER_TO_USE_SIZE 12
	#define SMC_REVISION_SIZE          3
	#define OS_VERSION_SIZE           20
	#define HARD_DRIVE_KEY_SIZE       24
	#define RECOVERY_KEY_SIZE          8
	#define ONLINE_KEY_VERSION_SIZE    4
	#define ONLINE_KEY_SIZE          172

	CHostResponse CHR;
	DWORD i, dwActualBytesReceived = 0 , dwErrorCodeFromHost = 0;
	int iError;
	TCHAR XBoxGameRegionCodeLabel[] = XBOX_GAME_REGION_CODE; // Read from host
	TCHAR XBoxTVRegionCodeLabel[] = XBOX_TV_REGION_CODE; // Read from host
	TCHAR XBoxHardDriveKeyLabel[] = XBOX_HARD_DRIVE_KEY; // Written to host
	TCHAR XBoxRecoveryKeyLabel[] = XBOX_RECOVERY_KEY; // Written to host
	TCHAR XBoxOnlineKeyVersionLabel[] = XBOX_ONLINE_KEY_VERSION; // Written to host
	TCHAR XBoxOnlineKeyLabel[] = XBOX_ONLINE_KEY; // Written to host
	TCHAR XBoxOSVersionLabel[] = XBOX_ROM_VERSION; // Written to host
	TCHAR XBoxSMCRevLabel[] = XBOX_SMC_VERSION; // Written to host
	NTSTATUS NtStatus;

	TCHAR XBoxGameRegionCodeResult[GAME_REGION_CODE_SIZE+1];
	DWORD dwXBoxGameRegion = 0;
	TCHAR XBoxTVRegionCodeResult[TV_REGION_CODE_SIZE+1];
	DWORD dwXBoxTVRegion = 0;
	TCHAR XBoxMACAddressResult[MAC_ADDRESS_SIZE+5];
	DWORD dwXBoxMACAddressLow = 0;
	DWORD dwXBoxMACAddressHigh = 0;
	TCHAR XBoxSerialNumberResult[SERIAL_NUMBER_SIZE+1];
	char XBoxSerialNumberToUse[SERIAL_NUMBER_TO_USE_SIZE+1]; // Pointer to the 12 digit number
	char cSMCRev[SMC_REVISION_SIZE+1];
	TCHAR XBoxSMCRev[SMC_REVISION_SIZE+1];
	TCHAR XBoxOSVersion[OS_VERSION_SIZE+1];
	TCHAR XBoxHardDriveKey[HARD_DRIVE_KEY_SIZE+1];
	TCHAR XBoxRecoveryKey[RECOVERY_KEY_SIZE+1];
	DWORD dwXBoxOnlineKeyVersion = 0;
	TCHAR XBoxOnlineKeyVersion[ONLINE_KEY_VERSION_SIZE+1];
	TCHAR XBoxOnlineKey[ONLINE_KEY_SIZE+1];
	DWORD dwXBoxOutputBufferSize = 0;
	BYTE XBoxOutputBuffer[512];
	DWORD dwWritePerResult = S_OK;
	DWORD dwMessageLength;
	TCHAR XBoxReportToHostBuffer[256]; // Should be large enough to contain any key/value pair
	LPCTSTR pwszTemp = NULL;
	bool bVerifyEepromContents = false;
#ifdef  _UNICODE
	bool bUnicode = true;
#else
	bool bUnicode = false;
#endif

	// Before doing anything else, unlock the hard drive.  Don't mind any errors
	// because they are normal if the drive was not locked in the first place
	UnlockXboxHardDrive (NULL, 0);

	// Get the Game Region Code
	{
		CHostResponse CHR;
		if ((iError = g_host.iSendHost(MID_FACTORY_GET_VALUE, (char *)&XBoxGameRegionCodeLabel[0], (_tcslen(XBoxGameRegionCodeLabel) + 1) * sizeof(TCHAR), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
					return;
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			if (err_NoGameRegionCode())
				return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			// Process the string here.  Report an error if it is too big or cannot be converted
			if (dwActualBytesReceived > sizeof(XBoxGameRegionCodeResult))
			{
				err_GameRegionCodeTooLarge();
				return;
			}
			memset(XBoxGameRegionCodeResult, 0, sizeof(XBoxGameRegionCodeResult)); // Clear out the buffer first
			_tcsncpy(XBoxGameRegionCodeResult, (TCHAR *)CHR.pvBufferFromHost, (sizeof(XBoxGameRegionCodeResult)/sizeof(TCHAR)) - 1);
			if ((dwXBoxGameRegion = _tcstoul(XBoxGameRegionCodeResult, 0, 16)) == 0)
			{
				if (err_GameRegionInvalid(XBoxGameRegionCodeResult))
					return;

			}
			ReportDebug(BIT0, _T("Game region found: %8.8lx"), dwXBoxGameRegion);
		}
	}

	// Get the TV Region Code
	{
		CHostResponse CHR;
		if ((iError = g_host.iSendHost(MID_FACTORY_GET_VALUE, (char *)&XBoxTVRegionCodeLabel[0], (_tcslen(XBoxTVRegionCodeLabel) + 1) * sizeof(TCHAR), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
					return;
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			if (err_NoTVRegionCode())
				return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			// Process the string here.  Report an error if it is too big or cannot be converted
			if (dwActualBytesReceived > sizeof(XBoxTVRegionCodeResult))
			{
				err_TVRegionCodeTooLarge();
				return;
			}
			memset(XBoxTVRegionCodeResult, 0, sizeof(XBoxTVRegionCodeResult)); // Clear out the buffer first
			_tcsncpy(XBoxTVRegionCodeResult, (TCHAR *)CHR.pvBufferFromHost, (sizeof(XBoxTVRegionCodeResult)/sizeof(TCHAR)) - 1);
			if ((dwXBoxTVRegion = _tcstoul(XBoxTVRegionCodeResult, 0, 16)) == 0)
			{
				if (err_TVRegionInvalid(XBoxTVRegionCodeResult))
					return;

			}
			ReportDebug(BIT0, _T("TV region found: %8.8lx"), dwXBoxTVRegion);

		}
	}

	// Get the MAC address
	{
		CHostResponse CHR;
		if ((iError = g_host.iSendHost(MID_GET_MAC_ADDRESSES, NULL, 0, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
					return;
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			if (err_NoMACAddress())
				return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			// Process the string here.  Report an error if it is too big or cannot be converted
			if (dwActualBytesReceived > sizeof(XBoxMACAddressResult))
			{
				err_MACAddressTooLarge();
				return;
			}
			memset(XBoxMACAddressResult, 0, sizeof(XBoxMACAddressResult)); // Clear out the buffer first
			_tcsncpy(XBoxMACAddressResult, (TCHAR *)CHR.pvBufferFromHost, 4);
			if ((dwXBoxMACAddressHigh = _tcstoul(XBoxMACAddressResult, 0, 16)) == 0)
			{
				memset(XBoxMACAddressResult, 0, sizeof(XBoxMACAddressResult)); // Clear out the buffer first
				_tcsncpy(XBoxMACAddressResult, (TCHAR *)CHR.pvBufferFromHost, 12);
				if (err_MACAddressInvalid(XBoxMACAddressResult))
					return;

			}
			memset(XBoxMACAddressResult, 0, sizeof(XBoxMACAddressResult)); // Clear out the buffer first
			_tcsncpy(XBoxMACAddressResult, &((TCHAR *)CHR.pvBufferFromHost)[4], 8);
			if ((dwXBoxMACAddressLow = _tcstoul(XBoxMACAddressResult, 0, 16)) == 0)
			{
				memset(XBoxMACAddressResult, 0, sizeof(XBoxMACAddressResult)); // Clear out the buffer first
				_tcsncpy(XBoxMACAddressResult, (TCHAR *)CHR.pvBufferFromHost, 12);
				if (err_MACAddressInvalid(XBoxMACAddressResult))
					return;

			}
			ReportDebug(BIT0, _T("MAC Address found: %4.4lx%8.8lx"), dwXBoxMACAddressHigh, dwXBoxMACAddressLow);
		}
	}

	// Get the serial number
	{
		CHostResponse CHR;
		if ((iError = g_host.iSendHost(MID_GET_SERIAL_NUMBER, NULL, 0, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
					return;
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			if (err_NoSerialNumber())
				return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			// Process the string here.  Report an error if it is too big or cannot be converted
			if (dwActualBytesReceived > sizeof(XBoxSerialNumberResult))
			{
				err_SerialNumberTooLarge();
				return;
			}
			memset(XBoxSerialNumberResult, 0, sizeof(XBoxSerialNumberResult)); // Clear out the buffer first
			_tcsncpy(XBoxSerialNumberResult, (TCHAR *)CHR.pvBufferFromHost, (sizeof(XBoxSerialNumberResult)/sizeof(TCHAR)) - 1);
			// Need to strip the serial number down to 12 characters from 20 before using it
			if (bUnicode)
			{
				sprintf(XBoxSerialNumberToUse, "%S", &XBoxSerialNumberResult[_tcslen(XBoxSerialNumberResult)-12]);
				ReportDebug(BIT0, _T("MB Serial Number found: %S"), XBoxSerialNumberToUse);
			}
			else
			{
				sprintf(XBoxSerialNumberToUse, "%s", &XBoxSerialNumberResult[_tcslen(XBoxSerialNumberResult)-12]);
				ReportDebug(BIT0, _T("Serial Number found: %s"), XBoxSerialNumberToUse);
			}
		}
	}

	// Get the OS version
	{
		memset(XBoxOSVersion, 0, sizeof(XBoxOSVersion)); // Clear out the buffer first
		_sntprintf(XBoxOSVersion, sizeof(XBoxOSVersion)/sizeof(TCHAR), _T("%d.%02d.%d.%02d"), XboxKrnlVersion->Major, XboxKrnlVersion->Minor, XboxKrnlVersion->Build, (XboxKrnlVersion->Qfe & 0x7FFF));

		ReportDebug(BIT0, _T("OS Version found: %s"), XBoxOSVersion);
	}

	// Get the SMC revision
	{
		ULONG uDataValue;
		// Write to the register first to reset it 
		for(i = 0; i < 3; i++)
		{			
			if ((NtStatus = HalWriteSMBusValue(0x20, 0x01, FALSE, 0x00)) != STATUS_SUCCESS)
			{
				if(err_SMCWrite(_T("Firmware Revision"), NtStatus))
					return;
			}
		}

		// Get the actual SMC Firmware Revision ID from the SMC Revision Register
		// The Rev ID is 3 bytes long (e.g. B1C, A4E)
		for(i = 0; i < 3; i++)
		{			
			if ((NtStatus = HalReadSMBusValue(0x20, 0x01, FALSE, &uDataValue)) != STATUS_SUCCESS)
			{
				if(err_SMCRead(_T("Firmware Revision"), NtStatus))
					return;
			}

			// Store the value in a char array so that it can be used as a string
			cSMCRev[i] = (char)uDataValue;
		} // end for(i = 0; i < 3; i++)

		// Terminate with Null
		cSMCRev[3] = '\0';
		if (bUnicode)
			_stprintf(XBoxSMCRev, _T("%S"), cSMCRev);
		else
			_stprintf(XBoxSMCRev, _T("%s"), cSMCRev);

		ReportDebug(BIT0, _T("SMC Revision found: %s"), XBoxSMCRev);
	}

	// Now that we are done querying values, lets have GenPerBoxData() do its job
	{
		CHostResponse CHR;
		char cMessage[64]; // Buffer in which to store the message for the host
		char *pcReturnedVals;

		*((DWORD *)&(cMessage[0])) = dwXBoxGameRegion;
		*((DWORD *)&(cMessage[4])) = dwXBoxTVRegion;
		*((BYTE *)&(cMessage[8])) = (BYTE)((dwXBoxMACAddressHigh >> 24) & 0xFF);
		*((BYTE *)&(cMessage[9])) = (BYTE)((dwXBoxMACAddressHigh >> 16) & 0xFF);
		*((BYTE *)&(cMessage[10])) = (BYTE)((dwXBoxMACAddressHigh >> 8) & 0xFF);
		*((BYTE *)&(cMessage[11])) = (BYTE)((dwXBoxMACAddressHigh) & 0xFF);
		*((BYTE *)&(cMessage[12])) = (BYTE)((dwXBoxMACAddressLow >> 24) & 0xFF);
		*((BYTE *)&(cMessage[13])) = (BYTE)((dwXBoxMACAddressLow >> 16) & 0xFF);
		*((BYTE *)&(cMessage[14])) = (BYTE)((dwXBoxMACAddressLow >> 8) & 0xFF);
		*((BYTE *)&(cMessage[15])) = (BYTE)((dwXBoxMACAddressLow) & 0xFF);
		memcpy(&cMessage[16], XBoxSerialNumberToUse, 13);

		if ((iError = g_host.iSendHost(MID_GENPERBOXDATA, cMessage, 29, CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
					return;
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			if (err_NoGenPerBoxDataResponse())
				return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			// Process the returned data here.
			// Start with the Hard Drive Key
			pcReturnedVals = (char *)CHR.pvBufferFromHost;
			if (strlen(pcReturnedVals) + 1 > sizeof(XBoxHardDriveKey)/sizeof(TCHAR))
			{
				err_HardDriveKeyTooLarge();
				return;
			}
			if (bUnicode)
				_stprintf(XBoxHardDriveKey, _T("%S"), pcReturnedVals);
			else
				_stprintf(XBoxHardDriveKey, _T("%s"), pcReturnedVals);
			pcReturnedVals = pcReturnedVals + strlen(pcReturnedVals) + 1; // Point to the next entry in the returned buffer

			// Now read the Recovery Key
			if (strlen(pcReturnedVals) + 1 > sizeof(XBoxRecoveryKey)/sizeof(TCHAR))
			{
				err_RecoveryKeyTooLarge();
				return;
			}
			if (bUnicode)
				_stprintf(XBoxRecoveryKey, _T("%S"), pcReturnedVals);
			else
				_stprintf(XBoxRecoveryKey, _T("%s"), pcReturnedVals);
			pcReturnedVals = pcReturnedVals + strlen(pcReturnedVals) + 1; // Point to the next entry in the returned buffer
			
			// Now read the Online Key Version
			dwXBoxOnlineKeyVersion = *((DWORD *)pcReturnedVals);
			_stprintf(XBoxOnlineKeyVersion, _T("%4.4lx"), dwXBoxOnlineKeyVersion & 0xFFFF);
			pcReturnedVals = pcReturnedVals + sizeof(DWORD); // Point to the next entry in the returned buffer
			
			// Now read the Online Key
			if (strlen(pcReturnedVals) + 1 > sizeof(XBoxOnlineKey)/sizeof(TCHAR))
			{
				err_OnlineKeyTooLarge();
				return;
			}
			if (bUnicode)
				_stprintf(XBoxOnlineKey, _T("%S"), pcReturnedVals);
			else
				_stprintf(XBoxOnlineKey, _T("%s"), pcReturnedVals);
			pcReturnedVals = pcReturnedVals + strlen(pcReturnedVals) + 1; // Point to the next entry in the returned buffer

			// Now read the Output Buffer Size
			dwXBoxOutputBufferSize = *((DWORD *)pcReturnedVals);
			pcReturnedVals = pcReturnedVals + sizeof(DWORD); // Point to the next entry in the returned buffer
			
			// Now read the Output Buffer contents
			if (dwXBoxOutputBufferSize > sizeof(XBoxOutputBuffer))
			{
				err_OutputBufferTooLarge();
				return;
			}
			memcpy(XBoxOutputBuffer, pcReturnedVals, dwXBoxOutputBufferSize);
			pcReturnedVals = pcReturnedVals + dwXBoxOutputBufferSize; // Point to the next entry in the returned buffer

			ReportDebug(BIT0, _T("Hard Drive Key found: %s"), XBoxHardDriveKey);
			ReportDebug(BIT0, _T("Recovery Key found: %s"), XBoxRecoveryKey);
			ReportDebug(BIT0, _T("Online key version found: %s"), XBoxOnlineKeyVersion);
			ReportDebug(BIT0, _T("Online Key found: %s"), XBoxOnlineKey);
			ReportDebug(BIT0, _T("Output buffer size found: %lx"), dwXBoxOutputBufferSize);
		}
	}

	// Write the proper data to the eeprom

	if ((dwWritePerResult = WritePerBoxData(XBoxOutputBuffer, dwXBoxOutputBufferSize)) != S_OK)
	{
		if (dwWritePerResult == NTE_BAD_LEN)
		{
			if (err_WritePerBadLength())
				return;
		}
		else if (dwWritePerResult == NTE_BAD_DATA)
		{
			if (err_WritePerBadData())
				return;
		}
		else if (dwWritePerResult == E_FAIL)
		{
			if (err_WritePerBadAccess())
				return;
		}
		else if (dwWritePerResult == NTE_NO_KEY)
		{
			if (err_WritePerBadKey())
				return;
		}
		else
		{
			if (err_WritePerUnknown())
				return;
		}
	}

	// Pad any remaining space of the keys with spaces (at Microsoft's request)
	for (i = _tcslen(XBoxHardDriveKey); i < HARD_DRIVE_KEY_SIZE; i++)
	{
		XBoxHardDriveKey[i] = ' '; // Pad the end of the string with spaces
	}
	XBoxHardDriveKey[i] = '\0'; // Null terminate

	for (i = _tcslen(XBoxRecoveryKey); i < RECOVERY_KEY_SIZE; i++)
	{
		XBoxRecoveryKey[i] = ' '; // Pad the end of the string with spaces
	}
	XBoxRecoveryKey[i] = '\0'; // Null terminate

	for (i = _tcslen(XBoxOnlineKey); i < ONLINE_KEY_SIZE; i++)
	{
		XBoxOnlineKey[i] = ' '; // Pad the end of the string with spaces
	}
	XBoxOnlineKey[i] = '\0'; // Null terminate

	for (i = _tcslen(XBoxOSVersion); i < OS_VERSION_SIZE; i++)
	{
		XBoxOSVersion[i] = ' '; // Pad the end of the string with spaces
	}
	XBoxOSVersion[i] = '\0'; // Null terminate

	// Now report the data values back to the host so that they can be stored in the database
	// Report the Hard Drive Key
	{
		dwMessageLength = (_tcslen(XBoxHardDriveKeyLabel) + _tcslen(XBoxHardDriveKey) + 2) * sizeof(TCHAR);
		if (dwMessageLength > sizeof(XBoxReportToHostBuffer))
		{
			if (err_MessageTooLarge(dwMessageLength))
				return;
		}
		else
		{
			memcpy(&XBoxReportToHostBuffer[0], XBoxHardDriveKeyLabel, (_tcslen(XBoxHardDriveKeyLabel) + 1) * sizeof(TCHAR)); // Copy the label key and the terminator
			memcpy(&XBoxReportToHostBuffer[_tcslen(XBoxHardDriveKeyLabel) + 1], XBoxHardDriveKey, (_tcslen(XBoxHardDriveKey) + 1) * sizeof(TCHAR)); // Copy the value and the terminator
		}
		if ((iError = g_host.iSendHost(MID_FACTORY_SET_VALUE, (char *)XBoxReportToHostBuffer, dwMessageLength)) != EVERYTHINGS_OK)
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			if (err_HostCommunicationError(iError))
				return;
		}
	}
	// Report the Recovery Key
	{
		dwMessageLength = (_tcslen(XBoxRecoveryKeyLabel) + _tcslen(XBoxRecoveryKey) + 2) * sizeof(TCHAR);
		if (dwMessageLength > sizeof(XBoxReportToHostBuffer))
		{
			if (err_MessageTooLarge(dwMessageLength))
				return;
		}
		else
		{
			memcpy(&XBoxReportToHostBuffer[0], XBoxRecoveryKeyLabel, (_tcslen(XBoxRecoveryKeyLabel) + 1) * sizeof(TCHAR)); // Copy the label key and the terminator
			memcpy(&XBoxReportToHostBuffer[_tcslen(XBoxRecoveryKeyLabel) + 1], XBoxRecoveryKey, (_tcslen(XBoxRecoveryKey) + 1) * sizeof(TCHAR)); // Copy the value and the terminator
		}
		if ((iError = g_host.iSendHost(MID_FACTORY_SET_VALUE, (char *)XBoxReportToHostBuffer, dwMessageLength)) != EVERYTHINGS_OK)
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			if (err_HostCommunicationError(iError))
				return;
		}
	}
	// Report the Online Key Version
	{
		dwMessageLength = (_tcslen(XBoxOnlineKeyVersionLabel) + _tcslen(XBoxOnlineKeyVersion) + 2) * sizeof(TCHAR);
		if (dwMessageLength > sizeof(XBoxReportToHostBuffer))
		{
			if (err_MessageTooLarge(dwMessageLength))
				return;
		}
		else
		{
			memcpy(&XBoxReportToHostBuffer[0], XBoxOnlineKeyVersionLabel, (_tcslen(XBoxOnlineKeyVersionLabel) + 1) * sizeof(TCHAR)); // Copy the label key and the terminator
			memcpy(&XBoxReportToHostBuffer[_tcslen(XBoxOnlineKeyVersionLabel) + 1], XBoxOnlineKeyVersion, (_tcslen(XBoxOnlineKeyVersion) + 1) * sizeof(TCHAR)); // Copy the value and the terminator
		}
		if ((iError = g_host.iSendHost(MID_FACTORY_SET_VALUE, (char *)XBoxReportToHostBuffer, dwMessageLength)) != EVERYTHINGS_OK)
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			if (err_HostCommunicationError(iError))
				return;
		}
	}
	// Report the Online Key
	{
		dwMessageLength = (_tcslen(XBoxOnlineKeyLabel) + _tcslen(XBoxOnlineKey) + 2) * sizeof(TCHAR);
		if (dwMessageLength > sizeof(XBoxReportToHostBuffer))
		{
			if (err_MessageTooLarge(dwMessageLength))
				return;
		}
		else
		{
			memcpy(&XBoxReportToHostBuffer[0], XBoxOnlineKeyLabel, (_tcslen(XBoxOnlineKeyLabel) + 1) * sizeof(TCHAR)); // Copy the label key and the terminator
			memcpy(&XBoxReportToHostBuffer[_tcslen(XBoxOnlineKeyLabel) + 1], XBoxOnlineKey, (_tcslen(XBoxOnlineKey) + 1) * sizeof(TCHAR)); // Copy the value and the terminator
		}
		if ((iError = g_host.iSendHost(MID_FACTORY_SET_VALUE, (char *)XBoxReportToHostBuffer, dwMessageLength)) != EVERYTHINGS_OK)
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			if (err_HostCommunicationError(iError))
				return;
		}
	}
	// Report the OS Version
	{
		dwMessageLength = (_tcslen(XBoxOSVersionLabel) + _tcslen(XBoxOSVersion) + 2) * sizeof(TCHAR);
		if (dwMessageLength > sizeof(XBoxReportToHostBuffer))
		{
			if (err_MessageTooLarge(dwMessageLength))
				return;
		}
		else
		{
			memcpy(&XBoxReportToHostBuffer[0], XBoxOSVersionLabel, (_tcslen(XBoxOSVersionLabel) + 1) * sizeof(TCHAR)); // Copy the label key and the terminator
			memcpy(&XBoxReportToHostBuffer[_tcslen(XBoxOSVersionLabel) + 1], XBoxOSVersion, (_tcslen(XBoxOSVersion) + 1) * sizeof(TCHAR)); // Copy the value and the terminator
		}
		if ((iError = g_host.iSendHost(MID_FACTORY_SET_VALUE, (char *)XBoxReportToHostBuffer, dwMessageLength)) != EVERYTHINGS_OK)
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			if (err_HostCommunicationError(iError))
				return;
		}
	}
	// Report the SMC Revision
	{
		dwMessageLength = (_tcslen(XBoxSMCRevLabel) + _tcslen(XBoxSMCRev) + 2) * sizeof(TCHAR);
		if (dwMessageLength > sizeof(XBoxReportToHostBuffer))
		{
			if (err_MessageTooLarge(dwMessageLength))
				return;
		}
		else
		{
			memcpy(&XBoxReportToHostBuffer[0], XBoxSMCRevLabel, (_tcslen(XBoxSMCRevLabel) + 1) * sizeof(TCHAR)); // Copy the label key and the terminator
			memcpy(&XBoxReportToHostBuffer[_tcslen(XBoxSMCRevLabel) + 1], XBoxSMCRev, (_tcslen(XBoxSMCRev) + 1) * sizeof(TCHAR)); // Copy the value and the terminator
		}
		if ((iError = g_host.iSendHost(MID_FACTORY_SET_VALUE, (char *)XBoxReportToHostBuffer, dwMessageLength)) != EVERYTHINGS_OK)
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			if (err_HostCommunicationError(iError))
				return;
		}
	}

	bVerifyEepromContents = false;
	if ((pwszTemp = GetCfgString (_T("VerifyEepromContents"), NULL)) != NULL)
	{
		if (_tcsicmp(pwszTemp, _T("true")) == 0)
			bVerifyEepromContents = true;
	}
	// Make sure that the serial number and MAC address were really written correctly.
	if (bVerifyEepromContents)
	{
		char LocalSerialNumber[13];
		BYTE LocalMACAddress[6];
		WORD wLocalMACAddressHigh;
		DWORD dwLocalMACAddressLow;
		ULONG Size, Type, Error;

		// Check the serial number
		Error = XQueryValue(0x100, &Type, LocalSerialNumber, sizeof LocalSerialNumber, &Size);
		if(Error != NO_ERROR)
		{
			if (err_NoEepromSerialNumber())
				return;
		}
		else // Do the comparison
		{
			LocalSerialNumber[12] = 0;
			ReportDebug(BIT0, _T("MB Serial Number from eeprom: %S"), LocalSerialNumber);
			if (stricmp(XBoxSerialNumberToUse, LocalSerialNumber) != 0)
			{
				// Report a mismatch
				if (err_SerialNumberMismatch(XBoxSerialNumberToUse, LocalSerialNumber))
					return;
			}
		}
		// Check the MAC address
		Error = XQueryValue(0x101, &Type, LocalMACAddress, sizeof LocalMACAddress, &Size);
		if(Error != NO_ERROR)
		{
			if (err_NoMACAddress())
				return;
		}
		else // Do the comparison
		{
			wLocalMACAddressHigh = ((WORD)LocalMACAddress[0] << 8) | (WORD)LocalMACAddress[1];
			dwLocalMACAddressLow = ((DWORD)LocalMACAddress[2] << 24) | ((DWORD)LocalMACAddress[3] << 16) | ((DWORD)LocalMACAddress[4] << 8) | (DWORD)LocalMACAddress[5];
			ReportDebug(BIT0, _T("MAC Address found from eeprom: %4.4lx%8.8lx"), (DWORD)wLocalMACAddressHigh, dwLocalMACAddressLow);
			if ((dwLocalMACAddressLow != dwXBoxMACAddressLow) || ((DWORD)wLocalMACAddressHigh != dwXBoxMACAddressHigh))
			{
				// Report a mismatch
				if (err_MACAddressMismatch(dwXBoxMACAddressHigh, dwXBoxMACAddressLow, (DWORD)wLocalMACAddressHigh, dwLocalMACAddressLow))
					return;
			}
		}
	}

#endif
}

//******************************************************************
// Title: verify_per_box_information
//
// Abstract: This utility checks the eeprom on the XBox to make sure it contains
//           valid data, and then disables the manufacturing region so that the
//           XBox can no longer boot from the manufacturing boot CD/DVD.
//
// Uses config parameters:
//           none
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, verify_per_box_information, 5)
{
#ifdef _XBOX
	DWORD dwVerifyPerResult = S_OK;

	// Verify the eeprom data and lock the hard drive
	if ((dwVerifyPerResult = VerifyPerBoxData()) != S_OK)
	{
		if (dwVerifyPerResult == NTE_BAD_DATA)
		{
			if (err_WritePerBadData())
				return;
		}
		else if (dwVerifyPerResult == E_FAIL)
		{
			if (err_VerifyPerBadAccess())
				return;
		}
		else if (dwVerifyPerResult == NTE_NO_KEY)
		{
			if (err_VerifyPerBadKey())
				return;
		}
		else
		{
			if (err_VerifyPerUnknown())
				return;
		}
	}
#endif
}

//******************************************************************
// Title: report_xmta_versions_and_checksums
//
// Abstract: This utility reports the versions and chechecksums of
//           the XMTA test executive and the XMTA configuration files
//           (xmta.ini and xmta2.ini)
//
// Uses config parameters:
//           none
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, report_xmta_versions_and_checksums, 6)
{
	DWORD dwActualBytesReceived = 0, dwErrorCodeFromHost = 0;
	int iError;
	TCHAR XMTAXBOXName[] = _T("xmtaxbox.xbe");
	TCHAR XMTAININame1[] = _T("xmta.ini");
	TCHAR XMTAININame2[] = _T("xmta2.ini");
	TCHAR XMTAXBOXVersionLabel[] = XMTA_SOFTWARE_REV; // Written to host
	TCHAR INIVersionLabel[] = XMTA_SOFTWARE_CFG; // Written to host
	DWORD dwXMTAChecksum = 0;
	DWORD dwINIChecksum = 0;
	DWORD i;
	DWORD *pdwReturnedBuffer = NULL;
	DWORD dwRemainder, dwRemValue;
	DWORD dwMessageLength;
	TCHAR XMTAXBOXVersion[31];
	TCHAR INIVersion[31];
	TCHAR XBoxReportToHostBuffer[100]; // Should be large enough to contain these key/value pairs

	// Get the XMTAXBOX.XBE file and checksum it
	{
		CHostResponse CHR;
		if ((iError = g_host.iSendHost(MID_GET_FILE, (char *)&XMTAXBOXName[0], (_tcslen(XMTAXBOXName) + 1) * sizeof(TCHAR), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
					return;
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			if (err_NoFileReturned(XMTAXBOXName))
				return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			dwXMTAChecksum = dwActualBytesReceived;
			pdwReturnedBuffer = (DWORD *)CHR.pvBufferFromHost;
			for (i = 0; i < dwActualBytesReceived/sizeof(DWORD); i++)
			{
				dwXMTAChecksum = dwXMTAChecksum + pdwReturnedBuffer[i];
			}
			dwRemainder = dwActualBytesReceived - (i*sizeof(DWORD));
			if (dwRemainder > 0)
			{
				dwRemValue = 0;
				memcpy(&dwRemValue, &pdwReturnedBuffer[i], dwRemainder);
				dwXMTAChecksum = dwXMTAChecksum + dwRemValue;
			}
		}
	}

	// Get the XMTA.INI file and checksum it
	{
		CHostResponse CHR;
		if ((iError = g_host.iSendHost(MID_GET_FILE, (char *)&XMTAININame1[0], (_tcslen(XMTAININame1) + 1) * sizeof(TCHAR), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
					return;
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
					return;
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			if (err_NoFileReturned(XMTAININame1))
				return;
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			dwINIChecksum = dwActualBytesReceived;
			pdwReturnedBuffer = (DWORD *)CHR.pvBufferFromHost;
			for (i = 0; i < dwActualBytesReceived/sizeof(DWORD); i++)
			{
				dwINIChecksum = dwINIChecksum + pdwReturnedBuffer[i];
			}
			dwRemainder = dwActualBytesReceived - (i*sizeof(DWORD));
			if (dwRemainder > 0)
			{
				dwRemValue = 0;
				memcpy(&dwRemValue, &pdwReturnedBuffer[i], dwRemainder);
				dwINIChecksum = dwINIChecksum + dwRemValue;
			}
		}
	}

	// Get the XMTA2.INI file and checksum it; don't error if the file does not exist
	{
		CHostResponse CHR;
		if ((iError = g_host.iSendHost(MID_GET_FILE, (char *)&XMTAININame2[0], (_tcslen(XMTAININame2) + 1) * sizeof(TCHAR), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				// Don't care if XMTA2.INI doesn't exist
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
					return;
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// Don't care if XMTA2.INI doesn't exist
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			dwINIChecksum = dwActualBytesReceived;
			pdwReturnedBuffer = (DWORD *)CHR.pvBufferFromHost;
			for (i = 0; i < dwActualBytesReceived/sizeof(DWORD); i++)
			{
				dwINIChecksum = dwINIChecksum + pdwReturnedBuffer[i];
			}
			dwRemainder = dwActualBytesReceived - (i*sizeof(DWORD));
			if (dwRemainder > 0)
			{
				dwRemValue = 0;
				memcpy(&dwRemValue, &pdwReturnedBuffer[i], dwRemainder);
				dwINIChecksum = dwINIChecksum + dwRemValue;
			}
		}
	}

	// Now let's create the full version:checksum strings
	m_standard = GetCfgString( L"standard", NULL);
	_sntprintf(XMTAXBOXVersion, (sizeof(XMTAXBOXVersion)/sizeof(TCHAR)) - 1, _T("%s:%8.8lx"), g_tcXMTAVersionNumber, dwXMTAChecksum);
	_sntprintf(INIVersion, (sizeof(INIVersion)/sizeof(TCHAR)) - 1, _T("%s:%8.8lx"), GetCfgString( _T("ini_file_version_number"), _T("0000.0000.0000.0000")), dwINIChecksum);

	ReportDebug(BIT0, _T("XMTA Version:Checksum: %s"), XMTAXBOXVersion);
	ReportDebug(BIT0, _T("INI Version:Checksum: %s"), INIVersion);

	// Report XMTAXBOX version to the host
	dwMessageLength = (_tcslen(XMTAXBOXVersionLabel) + _tcslen(XMTAXBOXVersion) + 2) * sizeof(TCHAR);
	if (dwMessageLength > sizeof(XBoxReportToHostBuffer))
	{
		if (err_MessageTooLarge(dwMessageLength))
			return;
	}
	else
	{
		memcpy(&XBoxReportToHostBuffer[0], XMTAXBOXVersionLabel, (_tcslen(XMTAXBOXVersionLabel) + 1) * sizeof(TCHAR)); // Copy the label key and the terminator
		memcpy(&XBoxReportToHostBuffer[_tcslen(XMTAXBOXVersionLabel) + 1], XMTAXBOXVersion, (_tcslen(XMTAXBOXVersion) + 1) * sizeof(TCHAR)); // Copy the value and the terminator
	}
	if ((iError = g_host.iSendHost(MID_FACTORY_SET_VALUE, (char *)XBoxReportToHostBuffer, dwMessageLength)) != EVERYTHINGS_OK)
	{
		// Process a communication error
		// This test module just reports the number of the communication error that was received
		if (err_HostCommunicationError(iError))
			return;
	}

	// Report XMTA.INI version to the host
	dwMessageLength = (_tcslen(INIVersionLabel) + _tcslen(INIVersion) + 2) * sizeof(TCHAR);
	if (dwMessageLength > sizeof(XBoxReportToHostBuffer))
	{
		if (err_MessageTooLarge(dwMessageLength))
			return;
	}
	else
	{
		memcpy(&XBoxReportToHostBuffer[0], INIVersionLabel, (_tcslen(INIVersionLabel) + 1) * sizeof(TCHAR)); // Copy the label key and the terminator
		memcpy(&XBoxReportToHostBuffer[_tcslen(INIVersionLabel) + 1], INIVersion, (_tcslen(INIVersion) + 1) * sizeof(TCHAR)); // Copy the value and the terminator
	}
	if ((iError = g_host.iSendHost(MID_FACTORY_SET_VALUE, (char *)XBoxReportToHostBuffer, dwMessageLength)) != EVERYTHINGS_OK)
	{
		// Process a communication error
		// This test module just reports the number of the communication error that was received
		if (err_HostCommunicationError(iError))
			return;
	}

}

//******************************************************************
// Title: delete_file
//
// Abstract: This utility deletes the file specified in the configuration parameter
//
// Uses config parameters:
//           none
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, delete_file, 7)
{
	char szFilename[MAX_PATH];
#ifdef  _UNICODE
	bool bUnicode = true;
#else
	bool bUnicode = false;
#endif

	LPCTSTR	tcParameter = GetCfgString( _T("file_to_delete"), NULL);
	
	if (tcParameter != NULL)
	{
		if (bUnicode)
		{
			sprintf(szFilename, "%S", tcParameter);
		}
		else
		{
			sprintf(szFilename, "%s", tcParameter);
		}
		if (!DeleteFileA(szFilename))
		{
			ReportDebug(BIT0, _T("GetLastError reports: %lu"), GetLastError());
		}
	}
}

bool CXModuleTool::bMountUtilityDrive ()
{
#ifdef _XBOX
	BOOL bResult;

	if (GetFileAttributesA("Z:\\") == -1) // Only mount the drive if it is not already mounted (otherwise could hang)
	{
		bResult = XMountUtilityDrive(FALSE);
		if (!bResult)
		{
			if (err_CouldNotMountUtilityDrive(GetLastError()))
				return false;
		}
	}
#endif
	return true;
}

//******************************************************************
// Title: mount_utility_drive
//
// Abstract: This utility mounts the XBOX utility drive (drive Z) for XMTA
//
// Uses config parameters:
//           none
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, mount_utility_drive, 8)
{
	bMountUtilityDrive();
}

//******************************************************************
// Title: check_chipset_revisions
//
// Abstract: This utility compares the revisions of the XBox chipset to
//           expected values.
//
// Uses config parameters:
//           none
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, check_chipset_revisions, 9)
{
#ifdef _XBOX
	TCHAR RevParamName[80];
	LPCTSTR pwsAllowed, pwsNotAllowed;
	DWORD x;
	int RevRead;

	ReportStatistic(GPU_REVISION, _T("%2.2lx"), (DWORD)XboxHardwareInfo->GpuRevision);
	ReportStatistic(MCP_REVISION, _T("%2.2lx"), (DWORD)XboxHardwareInfo->McpRevision);

	pwsAllowed = GetCfgString(_T("GPURevisionAllowed[0]"), NULL);
	pwsNotAllowed = GetCfgString(_T("GPURevisionNotAllowed[0]"), NULL);

	// The list of allowed and not allowed revisions is mutually exclusive. So the
	// GPURevisionAllowed[0] and GPURevisionNotAllowed[0] parameters both cannot exist.
	if((pwsAllowed != NULL) && (pwsNotAllowed != NULL))
	{
		if (err_GPURevisionXor())
			return;
	}
	// If neither ChipsetRevisionAllowed[#] nor ChipsetRevisionNotAllowed[#] parameters exist,
	// there are no revision restrictions.
	else if((pwsAllowed == NULL) && (pwsNotAllowed == NULL))
	{
		ReportDebug(BIT0,_T("Neither GPURevisionAllowed[#] nor GPURevisionNotAllowed[#] list is specified so passing the test.\n"));
	}
	else
	{
		if (pwsNotAllowed != NULL) // Check the not-allowed list
		{
			for(x=0;x<100;x++)
			{
				_stprintf(RevParamName,_T("GPURevisionNotAllowed[%d]"),x);
				if ((RevRead = GetCfgInt (RevParamName, -1)) == -1)
					break;
				// Now see if this matches the actual revision
				if((DWORD)RevRead == (DWORD)XboxHardwareInfo->GpuRevision)
				{
					if (err_GPURevisionNotAllowed((DWORD)XboxHardwareInfo->GpuRevision))
						return;
					break;
				}
			}
		}
		else // Check the allowed list
		{
			for(x=0;x<100;x++)
			{
				_stprintf(RevParamName,_T("GPURevisionAllowed[%d]"),x);
				if ((RevRead = GetCfgInt (RevParamName, -1)) == -1)
				{
					if (err_GPURevisionNotAllowed((DWORD)XboxHardwareInfo->GpuRevision))
						return;
					break;
				}
				// Now see if this matches the actual revision
				if((DWORD)RevRead == (DWORD)XboxHardwareInfo->GpuRevision)
				{
					break;
				}
			}
		}
	}


	// Do this all over again for the MCP Revision

	pwsAllowed = GetCfgString(_T("MCPRevisionAllowed[0]"), NULL);
	pwsNotAllowed = GetCfgString(_T("MCPRevisionNotAllowed[0]"), NULL);

	// The list of allowed and not allowed revisions is mutually exclusive. So the
	// MCPRevisionAllowed[0] and MCPRevisionNotAllowed[0] parameters both cannot exist.
	if((pwsAllowed != NULL) && (pwsNotAllowed != NULL))
	{
		if (err_MCPRevisionXor())
			return;
	}
	// If neither ChipsetRevisionAllowed[#] nor ChipsetRevisionNotAllowed[#] parameters exist,
	// there are no revision restrictions.
	else if((pwsAllowed == NULL) && (pwsNotAllowed == NULL))
	{
		ReportDebug(BIT0,_T("Neither MCPRevisionAllowed[#] nor MCPRevisionNotAllowed[#] list is specified so passing the test.\n"));
	}
	else
	{
		if (pwsNotAllowed != NULL) // Check the not-allowed list
		{
			for(x=0;x<100;x++)
			{
				_stprintf(RevParamName,_T("MCPRevisionNotAllowed[%d]"),x);
				if ((RevRead = GetCfgInt (RevParamName, -1)) == -1)
					break;
				// Now see if this matches the actual revision
				if((DWORD)RevRead == (DWORD)XboxHardwareInfo->McpRevision)
				{
					if (err_MCPRevisionNotAllowed((DWORD)XboxHardwareInfo->McpRevision))
						return;
					break;
				}
			}
		}
		else // Check the allowed list
		{
			for(x=0;x<100;x++)
			{
				_stprintf(RevParamName,_T("MCPRevisionAllowed[%d]"),x);
				if ((RevRead = GetCfgInt (RevParamName, -1)) == -1)
				{
					if (err_MCPRevisionNotAllowed((DWORD)XboxHardwareInfo->McpRevision))
						return;
					break;
				}
				// Now see if this matches the actual revision
				if((DWORD)RevRead == (DWORD)XboxHardwareInfo->McpRevision)
				{
					break;
				}
			}
		}
	}
#endif
}

//******************************************************************
// Title: mount_dashboard_partition
//
// Abstract: This utility mounts the XBOX dashboard partition as drive Y:
//
// Uses config parameters:
//           none
//******************************************************************
#ifdef _XBOX
// NTSTATUS
typedef LONG NTSTATUS;
/*lint -save -e624 */  // Don't complain about different typedefs.
typedef NTSTATUS *PNTSTATUS;
/*lint -restore */  // Resume checking for different typedefs.
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L) 
#define NTHALAPI     DECLSPEC_IMPORT



typedef struct _LSA_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PCHAR Buffer;
} LSA_STRING, *PLSA_STRING;

#ifndef _NTDEF_
typedef LSA_STRING STRING, *PSTRING ;
#endif

typedef STRING OBJECT_STRING;
typedef PSTRING POBJECT_STRING;


extern "C"
{
NTSYSCALLAPI
NTSTATUS
NTAPI
IoCreateSymbolicLink(
	IN POBJECT_STRING SymbolicLinkName,
	IN POBJECT_STRING DeviceName
	);
}
#endif // #ifdef XBOX

bool CXModuleTool::bMountDashboardPartition ()
{
#ifdef _XBOX
    OBJECT_STRING strDosDevice, strPath;
    NTSTATUS status;

	if (GetFileAttributesA("Y:\\") == -1) // Only mount the drive if it is not already mounted (otherwise could hang)
	{
		strDosDevice.Buffer = "\\??\\Y:";
		strDosDevice.Length = (USHORT)strlen(strDosDevice.Buffer);
		strDosDevice.MaximumLength = strDosDevice.Length + 1;

		strPath.Buffer = "\\Device\\Harddisk0\\partition2";
		strPath.Length = (USHORT)strlen(strPath.Buffer);
		strPath.MaximumLength = strPath.Length + 1;

		status = IoCreateSymbolicLink(&strDosDevice, &strPath);
		if (status != STATUS_SUCCESS)
		{
			if (err_CouldNotMountDashboardPartition(GetLastError()))
				return false;
		}
	}
#endif // #ifdef XBOX
	return true;
}


IMPLEMENT_TESTUNLOCKED (Tool, mount_dashboard_partition, 10)
{
	bMountDashboardPartition();
}

//******************************************************************
// Title: unlock_xbox_hard_drive
//
// Abstract: This utility unlocks the hard drive using the Microsoft-supplied API
//
// Uses config parameters:
//           none
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, unlock_xbox_hard_drive, 11)
{
#ifdef _XBOX
	UnlockXboxHardDrive (NULL, 0);
#endif // #ifdef XBOX
}

//******************************************************************
// Title: compare_scart_status_voltage
//
// Abstract: This utility compares the voltage of the SCART Status line
//           to a specified upper and lower limit
//
// Uses config parameters:
//           scart_status_lower_limit
//           scart_status_upper_limit
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, compare_scart_status_voltage, 12)
{
	float fStatusVoltage;
	CHostResponse CHR;
	int iError;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost;
	double dblSCARTStatusLowerLimit = 0.0, dblSCARTStatusUpperLimit = 0.0;
	LPCTSTR pwszStringParam;

	if ((pwszStringParam = GetCfgString(_T("scart_status_lower_limit"), NULL)) == NULL)
	{
		if (err_BADPARAMETER(_T("scart_status_lower_limit")))
			return;
	}
	else
	{
		if ((dblSCARTStatusLowerLimit = wcstod(pwszStringParam, NULL)) == 0)
			dblSCARTStatusLowerLimit = 0.0;
	}
	if ((pwszStringParam = GetCfgString(_T("scart_status_upper_limit"), NULL)) == NULL)
	{
		if (err_BADPARAMETER(_T("scart_status_upper_limit")))
			return;
	}
	else
	{
		if ((dblSCARTStatusUpperLimit = wcstod(pwszStringParam, NULL)) == 0)
			dblSCARTStatusUpperLimit = 0.0;
	}

	// Use the full version of iSendHost because a response is expected.
	// No message is sent, only response is received after writing to the Fan Speed register.
	// This message will receive several values, one of which is the STATUS line voltage
	if ((iError = g_host.iSendHost(MID_XBOX_SENSOR_LINES, NULL, NULL, CHR, &dwActualBytesReceived, 
		 &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
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
	else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived < 19)) 
	{
		// The response buffer is empty, so take appropriate action (like an error message)
		err_SCARTStatusNotReceivedFromHost();
		return;
	} 
	
	else // The response is not NULL, and is not an error message, so process it
	{
		// Copy the data received to a local buffer for processing. The fan voltage is in "float"
		// so copy as many bytes as are represented by a float variable (mostly 4).
		memcpy(&fStatusVoltage, (char *)(CHR.pvBufferFromHost) + 12, sizeof(float));

		fStatusVoltage = fStatusVoltage * 2;  // Multiply by 2 because the TIU board has a divide-by-two resistor netork on this line
		if ((fStatusVoltage < dblSCARTStatusLowerLimit) || (fStatusVoltage > dblSCARTStatusUpperLimit))
		{
			if (err_SCARTStatusWrongVoltage(fStatusVoltage, dblSCARTStatusLowerLimit, dblSCARTStatusUpperLimit))
				return;
		}
	}

	return;
}

//******************************************************************
// Title: compare_dvd_firmware_revision
//
// Abstract: This test compares the DVD firmware revision with expected values
//
// Uses config parameters:
//           DVDRevisionAllowed[X]
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, compare_dvd_firmware_revision, 13)
{
#define FILE_ANY_ACCESS                 0
#define FILE_SPECIAL_ACCESS    (FILE_ANY_ACCESS)
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe

#define CTL_CODE(DeviceType, Function, Method, Access) ( \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) )

#define FILE_DEVICE_CD_ROM              0x00000002
#define FILE_DEVICE_DVD                 0x00000033
#define FILE_DEVICE_MASS_STORAGE        0x0000002d
#define FILE_DEVICE_CONTROLLER          0x00000004

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

#define FILE_ANY_ACCESS                 0

#define IOCTL_SCSI_BASE                 FILE_DEVICE_CONTROLLER

#define IOCTL_IDE_PASS_THROUGH          CTL_CODE(IOCTL_SCSI_BASE, 0x040a, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)



#pragma pack(push, 1)

 typedef struct _IDEREGS {
        UCHAR    bFeaturesReg;           // Used for specifying SMART "commands".
        UCHAR    bSectorCountReg;        // IDE sector count register
        UCHAR    bSectorNumberReg;       // IDE sector number register
        UCHAR    bCylLowReg;             // IDE low order cylinder value
        UCHAR    bCylHighReg;            // IDE high order cylinder value
        UCHAR    bDriveHeadReg;          // IDE drive/head register
        UCHAR    bCommandReg;            // Actual IDE command.
        UCHAR    bHostSendsData;         // Non-zero if host sends data to drive
} IDEREGS, *PIDEREGS, *LPIDEREGS;

typedef struct _ATA_PASS_THROUGH {
    IDEREGS IdeReg;
    ULONG   DataBufferSize;             // byte size of DataBuffer[]
    PVOID   DataBuffer;
} ATA_PASS_THROUGH, *PATA_PASS_THROUGH;

typedef struct _IDE_IDENTIFY_DATA {
    USHORT GeneralConfiguration;                // word 0
    USHORT NumberOfCylinders;                   // word 1
    USHORT Reserved1;                           // word 2
    USHORT NumberOfHeads;                       // word 3
    USHORT Reserved2[2];                        // word 4-5
    USHORT NumberOfSectorsPerTrack;             // word 6
    USHORT Reserved3[3];                        // word 7-9
    UCHAR SerialNumber[20];                     // word 10-19
    USHORT Reserved4[3];                        // word 20-22
    UCHAR FirmwareRevision[8];                  // word 23-26
    UCHAR ModelNumber[40];                      // word 27-46
    USHORT MaximumBlockTransfer : 8;            // word 47
    USHORT Reserved5 : 8;
    USHORT Reserved6;                           // word 48
    USHORT Capabilities;                        // word 49
    USHORT Reserved7;                           // word 50
    USHORT Reserved8 : 8;                       // word 51
    USHORT PioCycleTimingMode : 8;
    USHORT Reserved9;                           // word 52
    USHORT TranslationFieldsValid : 3;          // word 53
    USHORT Reserved10 : 13;
    USHORT NumberOfCurrentCylinders;            // word 54
    USHORT NumberOfCurrentHeads;                // word 55
    USHORT CurrentSectorsPerTrack;              // word 56
    ULONG CurrentSectorCapacity;                // word 57-58
    USHORT CurrentMultiSectorSetting;           // word 59
    ULONG UserAddressableSectors;               // word 60-61
    USHORT SingleWordDMASupport : 8;            // word 62
    USHORT SingleWordDMAActive : 8;
    USHORT MultiWordDMASupport : 8;             // word 63
    USHORT MultiWordDMAActive : 8;
    USHORT AdvancedPIOModes : 8;                // word 64
    USHORT Reserved11 : 8;
    USHORT MinimumMWXferCycleTime;              // word 65
    USHORT RecommendedMWXferCycleTime;          // word 66
    USHORT MinimumPIOCycleTime;                 // word 67
    USHORT MinimumPIOCycleTimeIORDY;            // word 68
    USHORT Reserved12[11];                      // word 69-79
    USHORT MajorRevision;                       // word 80
    USHORT MinorRevision;                       // word 81
    USHORT Reserved13[6];                       // word 82-87
    USHORT UltraDMASupport : 8;                 // word 88
    USHORT UltraDMAActive : 8;
    USHORT Reserved14[38];                      // word 89-126
    USHORT MediaStatusNotification : 2;         // word 127
    USHORT Reserved15 : 6;
    USHORT DeviceWriteProtect : 1;
    USHORT Reserved16 : 7;
    USHORT SecuritySupported : 1;               // word 128
    USHORT SecurityEnabled : 1;
    USHORT SecurityLocked : 1;
    USHORT SecurityFrozen : 1;
    USHORT SecurityCountExpired : 1;
    USHORT SecurityEraseSupported : 1;
    USHORT Reserved17 : 2;
    USHORT SecurityLevel : 1;
    USHORT Reserved18 : 7;
    USHORT Reserved19[127];                     // word 129-255
} IDE_IDENTIFY_DATA, *PIDE_IDENTIFY_DATA;

#pragma pack(pop)

	unsigned i;
	LPCTSTR pwszTemp = NULL;
	DWORD dwNumOfRevisions;
	wchar_t wszTempString[64];
	HANDLE hDevice = INVALID_HANDLE_VALUE;
    DWORD returned;
    char buffer[sizeof(ATA_PASS_THROUGH) + 512];
    PATA_PASS_THROUGH atapt = (PATA_PASS_THROUGH)buffer;
	char model[64], serial[64], firmware[64];
	wchar_t XBoxDVDVersion[20];

	memset(model, 0, 64);
	memset(serial, 0, 64);
	memset(firmware, 0, 64);

	// Get DVDRevisionAllowed[] parameter list
	i = 0;
	for (;;)
	{
		swprintf(wszTempString, L"DVDRevisionAllowed[%lu]", i);
		// Get it as a string so that we can compare it to NULL
		if ((pwszTemp = GetCfgString (wszTempString, NULL)) == NULL)
			break;
		i++;
	}
	dwNumOfRevisions = i;

	// Now get the actual revision information from the DVD drive
	hDevice = CreateFileA(
		"cdrom0:",
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING,
		NULL); 

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		err_OPEN_FILE_P1L("cdrom0:");
		return;
	}

	atapt->DataBufferSize = 512;
	atapt->DataBuffer = atapt + 1;

	atapt->IdeReg.bFeaturesReg     = 0;
	atapt->IdeReg.bSectorCountReg  = 0;
	atapt->IdeReg.bSectorNumberReg = 0;
	atapt->IdeReg.bCylLowReg       = 0;
	atapt->IdeReg.bCylHighReg      = 0;
	atapt->IdeReg.bDriveHeadReg    = 0;
	atapt->IdeReg.bHostSendsData   = 0;

	atapt->IdeReg.bCommandReg = 0xA1;

	DeviceIoControl(hDevice, IOCTL_IDE_PASS_THROUGH, atapt, sizeof(ATA_PASS_THROUGH), atapt, sizeof(ATA_PASS_THROUGH), &returned, FALSE);
	CloseHandle(hDevice);
	PIDE_IDENTIFY_DATA IdData = (PIDE_IDENTIFY_DATA)atapt->DataBuffer;

	for(i=0; i<sizeof(IdData->ModelNumber); i+=2)
	{
		model[i + 0] = IdData->ModelNumber[i + 1];
		model[i + 1] = IdData->ModelNumber[i + 0];
	}
	model[i] = 0;

	for(i=0; i<sizeof(IdData->SerialNumber); i+=2)
	{
		serial[i + 0] = IdData->SerialNumber[i + 1];
		serial[i + 1] = IdData->SerialNumber[i + 0];
	}
	serial[i] = 0;

	for(i=0; i<sizeof(IdData->FirmwareRevision); i+=2)
	{
		firmware[i + 0] = IdData->FirmwareRevision[i + 1];
		firmware[i + 1] = IdData->FirmwareRevision[i + 0];
	}
	firmware[i] = 0;
	while (i > 0)
	{
		if (firmware[i-1] == ' ')
		{
			firmware[i-1] = 0;
			i--;
		}
		else
			break;
	}
	memset(XBoxDVDVersion, 0, sizeof(XBoxDVDVersion)); // Clear out the buffer first
	_snwprintf(XBoxDVDVersion, sizeof(XBoxDVDVersion)/sizeof(wchar_t), L"%S", firmware);
	ReportDebug(BIT0, L"DVD Firmware rev = %s", XBoxDVDVersion);
	ReportStatistic(DVD_FIRMWARE_REVISION, L"%s", XBoxDVDVersion);
	for (i = 0; i < dwNumOfRevisions; i++)
	{
		swprintf(wszTempString, L"DVDRevisionAllowed[%lu]", i);
		if ((pwszTemp = GetCfgString (wszTempString, NULL)) == NULL)
			err_BADPARAMETER(wszTempString);
		else
		{
			if(wcsicmp(pwszTemp, XBoxDVDVersion) == 0) // Strings match
				break;
		}
	}
	if (i == dwNumOfRevisions)
		err_BadDVDRevision(XBoxDVDVersion);

}

//******************************************************************
// Title: compare_kernel_revision
//
// Abstract: This test compares the kernel revision with expected values
//
// Uses config parameters:
//           KernelRevisionAllowed[X]
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, compare_kernel_revision, 14)
{
#ifdef _XBOX
	#define OS_VERSION_SIZE           20
	wchar_t XBoxOSVersion[OS_VERSION_SIZE+1];
	DWORD i;
	LPCTSTR pwszTemp = NULL;
	DWORD dwNumOfRevisions;
	wchar_t wszTempString[64];

	// Get KernelRevisionAllowed[] parameter list
	i = 0;
	for (;;)
	{
		swprintf(wszTempString, L"KernelRevisionAllowed[%lu]", i);
		// Get it as a string so that we can compare it to NULL
		if ((pwszTemp = GetCfgString (wszTempString, NULL)) == NULL)
			break;
		i++;
	}
	dwNumOfRevisions = i;

	// Now get the actual revision information from the Kernel
	memset(XBoxOSVersion, 0, sizeof(XBoxOSVersion)); // Clear out the buffer first
	_snwprintf(XBoxOSVersion, sizeof(XBoxOSVersion)/sizeof(wchar_t), L"%d.%02d.%d.%02d", XboxKrnlVersion->Major, XboxKrnlVersion->Minor, XboxKrnlVersion->Build, (XboxKrnlVersion->Qfe & 0x7FFF));
	ReportDebug(BIT0, L"OS Version found: %s", XBoxOSVersion);

	for (i = 0; i < dwNumOfRevisions; i++)
	{
		swprintf(wszTempString, L"KernelRevisionAllowed[%lu]", i);
		if ((pwszTemp = GetCfgString (wszTempString, NULL)) == NULL)
			err_BADPARAMETER(wszTempString);
		else
		{
			if(wcsicmp(pwszTemp, XBoxOSVersion) == 0) // Strings match
				break;
		}
	}
	if (i == dwNumOfRevisions)
		err_BadKernelRevision(XBoxOSVersion);
#endif // #ifdef XBOX
}

#define UEM_MINIMUM_KERNAL_BUILD 4031
//******************************************************************
// Title: set_uem_state
//
// Abstract: This test sets the EUEM reboot behavior to either retry boot
//           on UEM (true), or fail on UEM (false)
//
// Uses config parameters:
//           SetFatalErrorRetryFlag
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, set_uem_state, 15)
{
	LPCTSTR pwszTemp = NULL;
	bool bSetFatalErrorRetryFlag = true;

	// Get the revision information from the Kernel
	if (XboxKrnlVersion->Build >= UEM_MINIMUM_KERNAL_BUILD)
	{
		if ((pwszTemp = GetCfgString (_T("SetFatalErrorRetryFlag"), NULL)) != NULL)
		{
			if (_tcsicmp(pwszTemp, _T("false")) == 0)
				bSetFatalErrorRetryFlag = false;
		}
		if (FAILED(SetFatalErrorRetryFlag(bSetFatalErrorRetryFlag)))
		{
			err_SettingFatalErrorRetryFlag();
			return;
		}
	}
}

//******************************************************************
// Title: clear_uem_error_history
//
// Abstract: This utility clears the UEM error history.  This is run right before the end of XTest
//
// Uses config parameters:
//           None.
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, clear_uem_error_history, 16)
{
	// Get the revision information from the Kernel
	if (XboxKrnlVersion->Build >= UEM_MINIMUM_KERNAL_BUILD)
	{
		if (FAILED(ClearFatalErrorHistory()))
		{
			err_ClearFatalErrorHistory();
			return;
		}
	}
}

//******************************************************************
// Title: check_for_uem_error
//
// Abstract: This test checks for UEM errors and reports any UEM error condition
//
// Uses config parameters:
//           UEMTotalErrorsAllowed
//           UEMCoreDigitalErrorsAllowed
//           UEMBadEepromErrorsAllowed
//           UEMBadRamErrorsAllowed
//           UEMHDDNotLockedErrorsAllowed
//           UEMHDDCannotUnlockErrorsAllowed
//           UEMHDDTimeoutErrorsAllowed
//           UEMHDDNotFoundErrorsAllowed
//           UEMHDDBadConfigErrorsAllowed
//           UEMDVDTimeoutErrorsAllowed
//           UEMDVDNotFoundErrorsAllowed
//           UEMDVDBadConfigErrorsAllowed
//           UEMXBEDashGenericErrorsAllowed
//           UEMXBEDashErrorsAllowed
//           UEMXBEDashSettingsErrorsAllowed
//           UEMXBEDashX2PassErrorsAllowed
//           UEMRebootRoutineErrorsAllowed
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, check_for_uem_error, 17)
{
#define UEMTotalErrors _T("UEMTotalErrors")
#define UEMCoreDigitalError _T("UEMCoreDigitalError")
#define UEMBadEepromError _T("UEMBadEepromError")
#define UEMBadRamError _T("UEMBadRamError")
#define UEMHDDNotLockedError _T("UEMHDDNotLockedError")
#define UEMHDDCannotUnlockError _T("UEMHDDCannotUnlockError")
#define UEMHDDTimeoutError _T("UEMHDDTimeoutError")
#define UEMHDDNotFoundError _T("UEMHDDNotFoundError")
#define UEMHDDBadConfigError _T("UEMHDDBadConfigError")
#define UEMDVDTimeoutError _T("UEMDVDTimeoutError")
#define UEMDVDNotFoundError _T("UEMDVDNotFoundError")
#define UEMDVDBadConfigError _T("UEMDVDBadConfigError")
#define UEMXBEDashGenericError _T("UEMXBEDashGenericError")
#define UEMXBEDashError _T("UEMXBEDashError")
#define UEMXBEDashSettingsError _T("UEMXBEDashSettingsError")
#define UEMXBEDashX2PassError _T("UEMXBEDashX2PassError")
#define UEMRebootRoutineError _T("UEMRebootRoutineError")
#define UEMUnknownError _T("UEMUnknownError")


	TCHAR tszTemp[80];
	LPCTSTR ptszErrorType = NULL;
	DWORD dwFatalErrorCode;
	DWORD dwUEMTotalErrorsAllowed, dwUEMNumSpecificErrorsAllowed;
	DWORD dwUEMTotalErrorCount=0, dwUEMNumSpecificErrorCount=0;
	CHostResponse CHR;
	DWORD dwActualBytesReceived = 0, dwErrorCodeFromHost = 0;
	int iError;
	DWORD dwStandardErrorCode = 0;

	// Get the revision information from the Kernel
	if (XboxKrnlVersion->Build >= UEM_MINIMUM_KERNAL_BUILD)
	{
		if (FAILED(QueryFatalErrorLastCode(&dwFatalErrorCode)))
		{
			err_QueryingFatalErrorCode();
			return;
		}
		if (dwFatalErrorCode != FATAL_ERROR_NONE) // Only perform the rest of the utility is there was an error
		{
			switch(dwFatalErrorCode)
			{
				case FATAL_ERROR_CORE_DIGITAL:
					dwStandardErrorCode = 0x48;
					ptszErrorType = UEMCoreDigitalError;
					break;
				case FATAL_ERROR_BAD_EEPROM:
					dwStandardErrorCode = 0x49;
					ptszErrorType = UEMBadEepromError;
					break;
				case FATAL_ERROR_BAD_RAM:
					dwStandardErrorCode = 0x4A;
					ptszErrorType = UEMBadRamError;
					break;
				case FATAL_ERROR_HDD_NOT_LOCKED:
					dwStandardErrorCode = 0x4B;
					ptszErrorType = UEMHDDNotLockedError;
					break;
				case FATAL_ERROR_HDD_CANNOT_UNLOCK:
					dwStandardErrorCode = 0x4C;
					ptszErrorType = UEMHDDCannotUnlockError;
					break;
				case FATAL_ERROR_HDD_TIMEOUT:
					dwStandardErrorCode = 0x4D;
					ptszErrorType = UEMHDDTimeoutError;
					break;
				case FATAL_ERROR_HDD_NOT_FOUND:
					dwStandardErrorCode = 0x4E;
					ptszErrorType = UEMHDDNotFoundError;
					break;
				case FATAL_ERROR_HDD_BAD_CONFIG:
					dwStandardErrorCode = 0x4F;
					ptszErrorType = UEMHDDBadConfigError;
					break;
				case FATAL_ERROR_DVD_TIMEOUT:
					dwStandardErrorCode = 0x50;
					ptszErrorType = UEMDVDTimeoutError;
					break;
				case FATAL_ERROR_DVD_NOT_FOUND:
					dwStandardErrorCode = 0x51;
					ptszErrorType = UEMDVDNotFoundError;
					break;
				case FATAL_ERROR_DVD_BAD_CONFIG:
					dwStandardErrorCode = 0x52;
					ptszErrorType = UEMDVDBadConfigError;
					break;
				case FATAL_ERROR_XBE_DASH_GENERIC:
					dwStandardErrorCode = 0x53;
					ptszErrorType = UEMXBEDashGenericError;
					break;
				case FATAL_ERROR_XBE_DASH_ERROR:
					dwStandardErrorCode = 0x54;
					ptszErrorType = UEMXBEDashError;
					break;
				case FATAL_ERROR_XBE_DASH_SETTINGS:
					dwStandardErrorCode = 0x55;
					ptszErrorType = UEMXBEDashSettingsError;
					break;
				case FATAL_ERROR_XBE_DASH_X2_PASS:
					dwStandardErrorCode = 0x56;
					ptszErrorType = UEMXBEDashX2PassError;
					break;
				case FATAL_ERROR_REBOOT_ROUTINE:
					dwStandardErrorCode = 0x57;
					ptszErrorType = UEMRebootRoutineError;
					break;
				default:
					dwStandardErrorCode = 0x47;
					ptszErrorType = UEMUnknownError;
					break;
			}
			// Read in the specific and total error config parameter
			dwUEMTotalErrorsAllowed = GetCfgUint (_T("UEMTotalErrorsAllowed"), 0);
			_stprintf(tszTemp, _T("%ssAllowed"), ptszErrorType);
			dwUEMNumSpecificErrorsAllowed = GetCfgUint (tszTemp, 0);

			// Get the total and specific error counts from the host
			// Use the full version of iSendHost because a response is expected
			if ((iError = g_host.iSendHost(MID_GET_NAMED_STATE, (char *)UEMTotalErrors, (_tcslen(UEMTotalErrors) + 1)*sizeof(TCHAR), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
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
			else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived < 4)) // Don't access the response buffer if it is NULL
			{
				// The response buffer is empty, so take appropriate action (like an error message)
				err_NamedStateBufferIsEmpty();
				return;
			}
			else // The response is not NULL, and is not an error message, so process it
			{
				dwUEMTotalErrorCount = *((DWORD *)CHR.pvBufferFromHost);
			}
			// Use the full version of iSendHost because a response is expected
			if ((iError = g_host.iSendHost(MID_GET_NAMED_STATE, (char *)ptszErrorType, (_tcslen(ptszErrorType) + 1)*sizeof(TCHAR), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
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
			else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived < 4)) // Don't access the response buffer if it is NULL
			{
				// The response buffer is empty, so take appropriate action (like an error message)
				err_NamedStateBufferIsEmpty();
				return;
			}
			else // The response is not NULL, and is not an error message, so process it
			{
				dwUEMNumSpecificErrorCount = *((DWORD *)CHR.pvBufferFromHost);
			}

			// Now we know what the current counts are, so increment them and compare them to permitted values
			dwUEMTotalErrorCount ++;
			dwUEMNumSpecificErrorCount ++;
			// ReportStatistics
			_stprintf(tszTemp, _T("%sCount"), ptszErrorType);
			ReportStatistic(tszTemp, _T("%ld"), dwUEMNumSpecificErrorCount);
			ReportStatistic(_T("UEMTotalErrorCount"), _T("%ld"), dwUEMTotalErrorCount);

			if (dwUEMNumSpecificErrorCount > dwUEMNumSpecificErrorsAllowed)
			{
				if (err_UEMTooManyErrors(dwStandardErrorCode, dwUEMNumSpecificErrorCount, dwFatalErrorCode, ptszErrorType))
					return;
			}
			else // Update the count on the test server
			{
				_stprintf(&tszTemp[sizeof(DWORD)/sizeof(TCHAR)], _T("%s"), ptszErrorType);
				*((DWORD *)(&tszTemp[0])) = dwUEMNumSpecificErrorCount;
				// Use the short version of iSendHost because no response is expected
				if ((iError = g_host.iSendHost(MID_SET_NAMED_STATE, (char *)tszTemp, sizeof(DWORD) + (_tcslen(&tszTemp[sizeof(DWORD)/sizeof(TCHAR)]) + 1)*sizeof(TCHAR))) != EVERYTHINGS_OK)
				{
					// Process a communication error
					// This test module just reports the number of the communication error that was received
					if (err_HostCommunicationError(iError))
						return;
				}
			}

			if (dwUEMTotalErrorCount > dwUEMTotalErrorsAllowed)
			{
				if (err_UEMTooManyTotalErrors(dwUEMTotalErrorCount))
					return;
			}
			else // Update the count on the test server
			{
				_stprintf(&tszTemp[sizeof(DWORD)/sizeof(TCHAR)], _T("%s"), UEMTotalErrors);
				*((DWORD *)(&tszTemp[0])) = dwUEMTotalErrorCount;
				// Use the short version of iSendHost because no response is expected
				if ((iError = g_host.iSendHost(MID_SET_NAMED_STATE, (char *)tszTemp, sizeof(DWORD) + (_tcslen(&tszTemp[sizeof(DWORD)/sizeof(TCHAR)]) + 1)*sizeof(TCHAR))) != EVERYTHINGS_OK)
				{
					// Process a communication error
					// This test module just reports the number of the communication error that was received
					if (err_HostCommunicationError(iError))
						return;
				}
			}
		}
	}
}

//******************************************************************
// Title: image_hard_drive
//
// Abstract: This utility places the current software image on the hard drive
//
// Uses config parameters:
//           none
//******************************************************************
IMPLEMENT_TESTUNLOCKED (Tool, image_hard_drive, 18)
{
#define MAXIMUM_FILE_BYTES_PER_REQUEST 30000000
	DWORD dwMaximumFileBytesPerRequest, dwNumberRead, dwNumberWritten, dwActualBytesWritten;
	DWORD dwFileSize = 0;
#ifdef _XBOX
	char ImageFilenameOnXBox[] = "z:\\rawimage";
#else
	char ImageFilenameOnXBox[] = "c:\\rawimage00000000000000000000";
	if (g_host.m_strSerialNumber != NULL)
		sprintf(&ImageFilenameOnXBox[strlen(ImageFilenameOnXBox)-20], "%S", g_host.m_strSerialNumber);
#endif // #ifdef XBOX
	TCHAR szMessage[MAX_PATH + 8];  // Make sure we have enough space for the two DWORDs and the file name in the message
	CHostResponse CHR;
	DWORD dwActualBytesReceived = 0, dwErrorCodeFromHost = 0;
	int iError;
	HANDLE hDevice = INVALID_HANDLE_VALUE; 
	UINT uiSeed=0,uiRandScratch=0;

	LPCTSTR	tcParameter = GetCfgString( _T("image_file"), NULL);
	
	if (tcParameter == NULL) // No file name was specified
	{
		err_NoImageFileSpecified();
		return;
	}

	dwMaximumFileBytesPerRequest = GetCfgUint (L"maximum_file_bytes_per_request", MAXIMUM_FILE_BYTES_PER_REQUEST);

	// Get the size of the file from the test server
	if ((iError = g_host.iSendHost(MID_GET_FILE_SIZE, (char *)&tcParameter[0], (_tcslen(tcParameter) + 1) * sizeof(TCHAR), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
	{
		if (dwErrorCodeFromHost != 0)
		{
			// Process error from host
			if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
				return;
		}
		else
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			if (err_HostCommunicationError(iError))
				return;
		}
	}
	else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived < 4)) // Don't access the response buffer if it is NULL
	{
		// The response buffer is empty, so take appropriate action (like an error message)
		if (err_NoFileSizeReturned(tcParameter))
			return;
	}
	else // The response is not NULL, and is not an error message, so process it
	{
		dwFileSize = *(DWORD *)CHR.pvBufferFromHost;
		if (dwFileSize == 0)
		{
			err_ImageFileZeroLength(tcParameter);
			return;
		}
	}

	vNewSeed(&uiSeed);
	uiRandScratch=uiSeed;
	// Mount the utility drive if it is not already mounted
	if (!bMountUtilityDrive())
		return;

	// Mount the Dashboard partition if it is not already mounted
	if (!bMountDashboardPartition())
		return;

	// Open the File to write to the hard drive
	hDevice = CreateFileA(
		ImageFilenameOnXBox,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL); 

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		err_OPEN_FILE_P1L(ImageFilenameOnXBox);
		return;
	}

	// Get the file from the test server
	dwNumberRead = 0;
	dwNumberWritten = 0;
	while (dwNumberWritten < dwFileSize)
	{
		*(DWORD *)(&szMessage[0]) = dwNumberRead; // The byte offset
		*(DWORD *)(&szMessage[2]) = dwMaximumFileBytesPerRequest; // The maximum number of bytes to get
		memcpy (&szMessage[4], tcParameter, (_tcslen(tcParameter) + 1) * sizeof(TCHAR));

		if ((iError = g_host.iSendHost(MID_GET_FILE_EX, (char *)&szMessage[0], ((_tcslen(tcParameter) + 1) * sizeof(TCHAR)) + (sizeof(DWORD) * 2), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, TOOL_HOST_TIMEOUT)) != EVERYTHINGS_OK)
		{
			if (dwErrorCodeFromHost != 0)
			{
				if ((dwErrorCodeFromHost == 0xf1f) && (dwNumberRead == 0))
				{
					if (CheckAbort(HERE)) // If we haven't started reading the file yet, just exit on an abort gracefully
					{
						CloseHandle(hDevice);
						return;
					}
					else
					{
						Sleep(((random(&uiRandScratch) % 30)+10)*1000);  // Sleep between 10 and 40 seconds
						ReportDebug(BIT0, L"Trying to download the file %s", tcParameter);
						continue;
					}
				}
				// Process error from host
				if (err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost))
				{
					CloseHandle(hDevice);
					return;
				}
			}
			else
			{
				// Process a communication error
				// This test module just reports the number of the communication error that was received
				if (err_HostCommunicationError(iError))
				{
					CloseHandle(hDevice);
					return;
				}
			}
		}
		else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
		{
			// The response buffer is empty, so take appropriate action (like an error message)
			if (err_NoFileReturned(tcParameter))
			{
				CloseHandle(hDevice);
				return;
			}
		}
		else // The response is not NULL, and is not an error message, so process it
		{
			if(!WriteFile(
				hDevice,
				CHR.pvBufferFromHost,
				dwActualBytesReceived,
				&dwActualBytesWritten,
				NULL))
			{
				err_RE_WRITE_LOGICAL_P3L(ImageFilenameOnXBox, dwActualBytesReceived, (DWORDLONG)dwNumberWritten);
				CloseHandle(hDevice);
				return;
			}
//dwActualBytesWritten = dwActualBytesReceived;
			dwNumberRead = dwNumberRead + dwActualBytesReceived;
			dwNumberWritten = dwNumberWritten + dwActualBytesWritten;
			ReportDebug(BIT0, L"Read %lu bytes, Wrote %lu bytes.", dwActualBytesReceived, dwActualBytesWritten);
		}
	}
	CloseHandle(hDevice);  // Make sure the written file is closed

	if (dwNumberWritten != dwFileSize)
	{
		err_ImageFileWrongSize(ImageFilenameOnXBox, dwFileSize, dwNumberWritten);
		return;
	}
	// If we get here, then we wrote the file to the hard drive

}

void CXModuleTool::vNewSeed(UINT *pSeed)
{
	SYSTEMTIME systimeSeed;
	FILETIME filetimeSeed;

	GetSystemTime(&systimeSeed);
	SystemTimeToFileTime(&systimeSeed, &filetimeSeed);
	*pSeed = (UINT)filetimeSeed.dwLowDateTime;

	return;
}

DWORD CXModuleTool::random(UINT *pScratch)
{
	DWORD ret;
	ret  = (*pScratch = *pScratch * 214013L + 2531011L) >> 16;
	ret |= (*pScratch = *pScratch * 214013L + 2531011L) & 0xFFFF0000;
	return ret;
}
