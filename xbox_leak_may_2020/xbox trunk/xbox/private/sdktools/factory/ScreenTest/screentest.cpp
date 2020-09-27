/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    ScreenTest.cpp

Abstract:

    Screens XBOX units entering Repair & Refurbish.  Prints basic unit info
	 and validates the basic operation of the Duke, Audio, HDD

Author:

    Dennis Krueger (a-denkru) July 2001

Comments:
	This is intended as a high-level general purpose screening tool, not as a diagnostic.  Tests are
	intended to demonstrate only superficial functionality:
	The hard disk test tests only whether free space can be acquired.
	The Gamepad test tests only whether or not the game pad is accessable


Modified: 

--*/

#include "ScreenTest.h"


#define WHITECOLOR 0xffffffff
#define CRIMSONCOLOR 0xffDC143C
#define TITLECOLOR	0xffffd700


NTSTATUS
ResetEEPROMUserSettings(
    VOID
    )
/*++

Routine Description:

    Reset user settings in EEPROM to their defaults

Arguments:

    None

Return Value:

    Status of the operation

--*/
{
    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    EEPROM_LAYOUT* eeprom;
    XBOX_USER_SETTINGS* userSettings;
    NTSTATUS Status;
    DWORD type, size;

    eeprom = (EEPROM_LAYOUT*)buf;
    userSettings = (XBOX_USER_SETTINGS*) &eeprom->UserConfigSection;
    Status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, buf, sizeof(buf), &size);

    if (NT_SUCCESS(Status)) {
        RtlZeroMemory(userSettings, sizeof(*userSettings));

        if (XGetGameRegion() == XC_GAME_REGION_JAPAN) {
            userSettings->Language = XC_LANGUAGE_JAPANESE;
            userSettings->TimeZoneBias = -540;
            strcpy(userSettings->TimeZoneStdName, "TST");
            strcpy(userSettings->TimeZoneDltName, "TST");
        }

        userSettings->Checksum = ~XConfigChecksum(userSettings, sizeof(*userSettings));
        Status = ExSaveNonVolatileSetting(XC_MAX_ALL, REG_BINARY, buf, sizeof(buf));
    }

    return Status;
}


// support methods for TextScreenLine and TextScreenArray 
CTextScreenLine::CTextScreenLine()
{
	m_sx = 0;
	m_sy = 0;
	m_dwColor = 0;
	m_strText = NULL;
	m_dwFlags = 0;
}

CTextScreenLine::CTextScreenLine(float sx, float sy, DWORD Color, WCHAR * pTextStr, DWORD dwFlags)
{
	m_sx = sx;
	m_sy = sy;
	m_dwColor = Color;
	m_strText = pTextStr;
	m_dwFlags = dwFlags;
};



CTextScreenArray::CTextScreenArray()
{
	m_LineCount = 0;
}


CTextScreenArray::~CTextScreenArray()
{
	;
}



int CTextScreenArray::Add(CTextScreenLine * pThisLine)
{
	if(m_LineCount >= MAX_LINES) return -1;
	m_TextLines[m_LineCount] = pThisLine;
	m_LineCount++;
	return m_LineCount-1;  // return index
}

int CTextScreenArray::Replace(CTextScreenLine * pThisLine, int Index)
{
	if(Index >= MAX_LINES) return -1;
	m_TextLines[Index] = pThisLine;
	return Index;  // return index
}

int CTextScreenArray::GetLineCount()
{
	return m_LineCount;
}

CTextScreenLine *
CTextScreenArray::GetLine(int Index)
{
	if(Index >= m_LineCount) return NULL; // out of range
	return m_TextLines[Index];
}

CXBoxUtility::CXBoxUtility()	
		:CXBApplication()
{
	m_fAudioOK = TRUE;
	m_fPlaying = FALSE;
	m_iPlayIndex = 0;
	m_pAudioLine = NULL;
	m_GamePadLineIndex = 0;
}

//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
//_asm Int 3
    CXBoxUtility xbUtility;
    if( FAILED( xbUtility.Create() ) )
        return;
    xbUtility.Run();
}



HRESULT CXBoxUtility::Initialize()
{
	NTSTATUS ntStatus;

//_asm Int 3
    // Create font
    if( FAILED( m_Font16.Create( g_pd3dDevice, "Font16.xpr" ) ) )
        return E_FAIL;

	// Title Line
	CTextScreenLine * pTitleLine = new CTextScreenLine(210,60,TITLECOLOR,L"OOBA Screen Test",0);
	m_ThisScreen.Add(pTitleLine);


	// build text array for screen

	// read Disk Power On date and Power Cycle Count from eeprom
	ntStatus = ExReadWriteRefurbInfo( &m_RefurbInfo, sizeof(XBOX_REFURB_INFO),FALSE);
	if(NT_SUCCESS(ntStatus))
	{
		// Display power on date
		SYSTEMTIME SystemTime;
		FileTimeToSystemTime((FILETIME *)&m_RefurbInfo.FirstSetTime,&SystemTime);
		PWCHAR pPowerOnBuff = new WCHAR[60];
		wsprintf(pPowerOnBuff,L"PowerOn Date:  %02d.%02d.%04d",SystemTime.wMonth,SystemTime.wDay,SystemTime.wYear);
		CTextScreenLine * pPowerOnLine = new CTextScreenLine(160,140,WHITECOLOR,pPowerOnBuff,0);
		m_ThisScreen.Add(pPowerOnLine);

		// Display Number of Power Cycles since Power On Date
		PWCHAR pPowerCyclesBuff = new WCHAR[60];
		wsprintf(pPowerCyclesBuff,L"Power Cycles: %d", m_RefurbInfo.PowerCycleCount);
		CTextScreenLine * pPowerCycles = new CTextScreenLine(160,160,WHITECOLOR,pPowerCyclesBuff,0);
		m_ThisScreen.Add(pPowerCycles);
	} else
	{
		CTextScreenLine * pEEPROMFailLine = new CTextScreenLine(160,140,CRIMSONCOLOR,L"Hard disk access error");
		m_ThisScreen.Add(pEEPROMFailLine);
	}
	// Duke functions will be tested in the framemove loop in order to detect removals and inserts
	

	// check Hard Drive function
	// going to get the disk free space, if the query succeeds the hd is at least accessable
	BOOL bResult;
	OBJECT_STRING c_cDrive = CONSTANT_OBJECT_STRING("\\??\\C:");
	OBJECT_STRING c_cPath  = CONSTANT_OBJECT_STRING("\\Device\\Harddisk0\\partition1");
	ULONG ulAvail, ulTotal, ulFree;
	ULARGE_INTEGER AvailableBytes,TotalBytes,FreeBytes;
	IoCreateSymbolicLink(&c_cDrive, &c_cPath);
	bResult = GetDiskFreeSpaceEx("C:\\",&AvailableBytes,&TotalBytes,&FreeBytes);
	CTextScreenLine * pHDLine;
	if(bResult == FALSE)
	{
		pHDLine = new CTextScreenLine(160,200,CRIMSONCOLOR,L"Hard Disk Failure!!",0);
	} else
	{
		pHDLine = new CTextScreenLine(160,200,WHITECOLOR,L"Hard Disk Detected OK",0);
	}
	m_ThisScreen.Add(pHDLine);


	// setup Audio test
	HRESULT hResult;
	hResult = m_LeftSound.Create("Left.wav");
	if(!FAILED(hResult))
	{
		DSMIXBINVOLUMEPAIR dsmbvp = { DSMIXBIN_FRONT_LEFT, DSBVOLUME_MAX };
		DSMIXBINS dsmb = { 1, &dsmbvp };

		m_LeftSound.SetMixBins(&dsmb);
		hResult = m_RightSound.Create("Right.wav");
	}

	if(!FAILED(hResult))
	{
		DSMIXBINVOLUMEPAIR dsmbvp = { DSMIXBIN_FRONT_RIGHT, DSBVOLUME_MAX };
		DSMIXBINS dsmb = { 1, &dsmbvp };

		m_RightSound.SetMixBins(&dsmb);
		hResult = m_StereoSound.Create("Stereo.wav");
	}
	if(FAILED(hResult))
	{
		m_fAudioOK = FALSE;
		m_pAudioLine = new CTextScreenLine(160,220,CRIMSONCOLOR,L"Did not find Wave Files for Audio Test",0);
		m_ThisScreen.Add(m_pAudioLine);
		return S_OK;
	}

	return S_OK;
}


HRESULT CXBoxUtility::FrameMove()
{
	DWORD dwElapsedSecs;
	SYSTEMTIME CurrTime;
	WCHAR DebugBuffer[80];
	NTSTATUS EEPROMResetStatus;

// check Duke functionality
	CTextScreenLine * pGamePadLine;
	bool bGamepadFound = FALSE;
	for (int i = 0; i<4; i++)
	{
		if(g_Gamepads[i].hDevice) bGamepadFound = TRUE;

	}

	if(bGamepadFound== TRUE)
	{
		PWCHAR pPadBuffer = new WCHAR[60];
		swprintf(pPadBuffer,L"Game Pad detected on port ");
		for(int i = 0; i< 4; i++)
		{
			WCHAR cPort[4];
			if(g_Gamepads[i].hDevice)
			{
				swprintf(cPort,L"%d ",i+1);
				lstrcat(pPadBuffer,cPort);
			}
		}
		pGamePadLine = new CTextScreenLine(160,180,WHITECOLOR,pPadBuffer);
	} else
	{
		pGamePadLine = new CTextScreenLine(160,180,CRIMSONCOLOR,L"No Game Pads Found");
	}

	if(m_GamePadLineIndex == 0)
	{

		m_GamePadLineIndex = m_ThisScreen.Add(pGamePadLine);
	} else
	{
		m_ThisScreen.Replace(pGamePadLine,m_GamePadLineIndex);
	}

	if(m_fAudioOK == FALSE) return S_OK;  // failed audio file load

	if(m_fPlaying == FALSE )
	{
		// start audio - increment through left,right,stereo & stop
		switch (m_iPlayIndex)
		{
		case 0:
			m_LeftSound.Play(DSBPLAY_LOOPING);
			m_pAudioLine = new CTextScreenLine(160,220,WHITECOLOR,L"Audio Test Left Output ...");
			m_AudioLineIndex = m_ThisScreen.Add(m_pAudioLine); // add first
			m_fPlaying = TRUE;
			m_iPlayIndex++;

			break;
		case 1:
			m_LeftSound.Stop();
			m_RightSound.Play(DSBPLAY_LOOPING);
			m_pAudioLine = new CTextScreenLine(160,220,WHITECOLOR,L"Audio Test Right Output ...");
			m_ThisScreen.Replace(m_pAudioLine,m_AudioLineIndex);
			m_fPlaying = TRUE;
			m_iPlayIndex++;
			break;
		
		case 2:	
			m_RightSound.Stop();
			m_StereoSound.Play(DSBPLAY_LOOPING); // play for 5 secs
			m_pAudioLine = new CTextScreenLine(160,220,WHITECOLOR,L"Audio Stereo Output ...");
			m_ThisScreen.Replace(m_pAudioLine,m_AudioLineIndex);
			m_fPlaying = TRUE;
			m_iPlayIndex++;
			break;
		case 3:
			m_StereoSound.Stop();
			EEPROMResetStatus = ResetEEPROMUserSettings();
			m_pAudioLine = new CTextScreenLine(160,220,WHITECOLOR,L"Audio Test Concluded");
			m_ThisScreen.Replace(m_pAudioLine,m_AudioLineIndex);
			if (NT_SUCCESS(EEPROMResetStatus))
			{
				m_ThisScreen.Add(new CTextScreenLine(230,270,0xff7cfc00,L"Test Finished"));
			}
			else
			{
				m_ThisScreen.Add(new CTextScreenLine(230,270,CRIMSONCOLOR,L"Test Aborted - User EEPROM Reset Failed"));
			}
			m_iPlayIndex++;
			
		}
		m_AudioStartTicks = GetTickCount();
	}
    
	
	// get elapsed time for this audio loop
	GetSystemTime(&CurrTime);
	DWORD CurrTicks = GetTickCount();
	dwElapsedSecs = (CurrTicks - m_AudioStartTicks)/1000;
	if(dwElapsedSecs >= 6 && m_fPlaying == TRUE)
	{
		m_fPlaying = FALSE;
	}	
	return S_OK;
}


HRESULT CXBoxUtility::Render()
{
	CTextScreenLine * pThisLine;
	// Draw a gradient filled background
    RenderGradientBackground(0xff00000f, 0xff000fff );
	// draw text on the screen
	int ScreenLines = m_ThisScreen.GetLineCount();
	m_Font16.Begin();

	for(int i = 0; i<ScreenLines;i++)
	{
		// get current line
		pThisLine = m_ThisScreen.GetLine(i);
		if(pThisLine)
		{
			m_Font16.DrawText(pThisLine->m_sx,
				pThisLine->m_sy,
				pThisLine->m_dwColor,
				pThisLine->m_strText,
				pThisLine->m_dwFlags
				);
		}
	}
	m_Font16.End();

    return S_OK;

}