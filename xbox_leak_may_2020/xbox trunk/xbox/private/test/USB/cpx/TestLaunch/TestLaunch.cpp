/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    TestLaunch.cpp

Abstract:
	General purpose simple launch program.  Enumerates all .xbe files, presents them in a list
	and allows the users to select and launch them
    

Author:

    Dennis Krueger (a-denkru)

Environment:

    XBox

Comments:



*/

#include "TestLaunch.h"

CXBoxUtility::CXBoxUtility()
		:CXBApplication()
{

	m_iTestCount = 0;
	m_SelLine = 0;

}


//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
// _asm Int 3
    CXBoxUtility xbUtility;
    if( FAILED( xbUtility.Create() ) )
        return;
    xbUtility.Run();
}


HRESULT CXBoxUtility::Initialize()
{

//	_asm Int 3
	HANDLE hSearchHandle;
	WIN32_FIND_DATA FoundData;

    if( FAILED( m_Font16.Create( g_pd3dDevice, "Font16.xpr" ) ) )
        return E_FAIL;

	// build array of .xbe files on cd
	hSearchHandle = FindFirstFile("D:\\*.xbe",&FoundData);
	if(hSearchHandle == INVALID_HANDLE_VALUE)
	{
		// display error message
		return FALSE;
	}
    // Display each file and ask for the next.
	do
    {
        if(!(strcmp(FoundData.cFileName,"TestLaunch.xbe")) ||
			!(strcmp(FoundData.cFileName,"Default.xbe")))
				continue;
		int size = strlen(FoundData.cFileName);
		
		m_pwszTestNames[m_iTestCount] = new WCHAR[(size *2)+2];
		wsprintf(m_pwszTestNames[m_iTestCount],L"%S",FoundData.cFileName);
		m_iTestCount++;				

    } while( FindNextFile( hSearchHandle, &FoundData ) );

	DebugPrint("Finished File with %d Files\n m_pwszTestNames[0] = %x : %s\n",m_iTestCount,m_pwszTestNames[0],m_pwszTestNames[0]);
    // Close the find handle.
    FindClose( hSearchHandle );

	// got all the tests
	return TRUE;

}	

HRESULT CXBoxUtility::FrameMove()
{
	DWORD dwResult;
	
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
	{
		if(m_SelLine != 0)
			m_SelLine--;
	}
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN)
	{
		if(m_SelLine != m_iTestCount -1)
			m_SelLine++;
	}

	if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A])
	{
		// launch selected test
		char cLaunchBuffer[100];
		sprintf(cLaunchBuffer,"D:\\%S",m_pwszTestNames[m_SelLine]);
		dwResult = XLaunchNewImage(cLaunchBuffer,	NULL);

	}

	return S_OK;
}

#define SelColor 0xffFFFFE0  // Antique White
#define StdColor 0xff32CD32  // YELLOW
#define TitleColor 0xffffd700
#define BaseLine 120


HRESULT CXBoxUtility::Render()
{

    RenderGradientBackground(0xff00000f, 0xff000fff );
	// draw text on the screen
	m_Font16.Begin();

	m_Font16.DrawText(200,50,TitleColor,L"Test Launch Menu",0);
	for(int i = 0; i < m_iTestCount; i++)
	{
		m_Font16.DrawText(
			(float) 200,
			(float) BaseLine + (i * 35),
			i == m_SelLine ? SelColor : StdColor,
			m_pwszTestNames[i],
			0
			);
	}
	m_Font16.End();

	return S_OK;

}
