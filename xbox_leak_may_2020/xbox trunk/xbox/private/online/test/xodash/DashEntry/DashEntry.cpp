//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "std.h"
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))

// #include "xonp.h"
// #include "xonver.h"
#include "dm.h"
#include "xboxp.h"
#include "xlaunch.h"

static const OBJECT_STRING g_DDosDevicePrefix =  CONSTANT_OBJECT_STRING(OTEXT("\\??\\D:"));
static const CHAR g_szDVDDevicePrefix[] =        "\\Device\\Cdrom0";
static const int  g_cchDVDDevicePrefix =         ARRAYSIZE(g_szDVDDevicePrefix) - 1;


static wchar_t* EntryPoints[] = {
	L"Network Configuration",
	L"New Account Sign-up",
	L"Message Server delivery section",
	L"Online Menu",
	L"Forced Name Change (New pick name screen)",
	L"Forced Edit Billing (the Edit Billing menu of Account Management)",
	L"Subscription Agreement (Policy document)",
	L"Terms of Use (Policy document)",
	L"Code of Conduct (Policy document)",
	L"Privacy Statement (Policy document)",

};


static DWORD dwReasons[] = 
{
	XLD_LAUNCH_DASHBOARD_NETWORK_CONFIGURATION,	
	XLD_LAUNCH_DASHBOARD_NEW_ACCOUNT_SIGNUP,         
	XLD_LAUNCH_DASHBOARD_MESSAGE_SERVER_INFO,
	XLD_LAUNCH_DASHBOARD_ONLINE_MENU,	
	XLD_LAUNCH_DASHBOARD_FORCED_NAME_CHANGE,
	XLD_LAUNCH_DASHBOARD_FORCED_BILLING_EDIT,
	XLD_LAUNCH_DASHBOARD_POLICY_DOCUMENT,    
};

static DWORD dwParameter1s[] = 
{
	XLD_POLICY_SUBSCRIPTION_AGREEMENT,
	XLD_POLICY_TERMS_OF_USE,
	XLD_POLICY_CODE_OF_CONDUCT,
	XLD_POLICY_PRIVACY_STATEMENT
};

#define countof(n) (sizeof (n) / sizeof (n[0]))

// This function courtesy Jim Helm will reboot you to a private xbe on the hard drive without further ado
NTSTATUS WriteTitleInfoAndReboot(LPCSTR pszLaunchPath, LPCSTR pszDDrivePath,
    DWORD dwLaunchDataType, DWORD dwTitleId, PLAUNCH_DATA pLaunchData)
{
    extern PLAUNCH_DATA_PAGE *LaunchDataPage;
    NTSTATUS Status;
    LARGE_INTEGER ByteOffset;
    PSTR pszOutput;
    int cch;

    if (NULL == *LaunchDataPage)
    {
        *LaunchDataPage = (PLAUNCH_DATA_PAGE)MmAllocateContiguousMemory(PAGE_SIZE);
    }

    if (NULL == *LaunchDataPage)
    {
        return STATUS_NO_MEMORY;
    }

    MmPersistContiguousMemory(*LaunchDataPage, PAGE_SIZE, TRUE);

    if (!pszDDrivePath)

    {
        pszDDrivePath = "\\Device\\Cdrom0";
    }

    pszOutput = ((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath;

    ASSERT(PAGE_SIZE == sizeof(LAUNCH_DATA_PAGE));

    RtlZeroMemory(*LaunchDataPage, PAGE_SIZE);

    ((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.dwLaunchDataType = dwLaunchDataType;
    ((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.dwTitleId = dwTitleId;

    if (dwLaunchDataType != LDT_NONE)
    {
        memcpy(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->LaunchData,
               pLaunchData,
               sizeof(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->LaunchData));
    }

    if (NULL != pszLaunchPath)
    {
        if (lstrcmpiA(pszDDrivePath, "\\Device\\Harddisk0\\Partition2") == 0)
        {
            lstrcpynA(pszOutput, "\\Device\\Harddisk0\\Partition2\\",
                      ARRAYSIZE(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath) - 1);

            cch = strlen(pszOutput);
            lstrcpynA(&(pszOutput[cch]),
                      pszLaunchPath,
                      ARRAYSIZE(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath) - (cch));
        }
        else
        {
            lstrcpynA(pszOutput,
                      pszDDrivePath,
                      ARRAYSIZE(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath) - 1);

            cch = strlen(pszOutput);
            pszOutput[cch++] = TITLE_PATH_DELIMITER;

            lstrcpynA(&(pszOutput[cch]),
                      pszLaunchPath,
                      ARRAYSIZE(((PLAUNCH_DATA_PAGE) *LaunchDataPage)->Header.szLaunchPath) - (cch));
        }
    }

    // Notify the debugger that we're about to reboot and then reboot
    DmTell(DMTELL_REBOOT, NULL);

    HalReturnToFirmware(HalQuickRebootRoutine);
    return Status;
}

 
//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBoxSample();

    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();

    // Font and help
    CXBFont         m_Font;
private:
	int m_nActiveEntry;
  
};



//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}



//-----------------------------------------------------------------------------
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :m_nActiveEntry(0), CXBApplication()
{

}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    return S_OK;
}



const char cszXbeName[] = "xonlinedash.xbe";
const char cszXbePath[] = "\\Device\\Harddisk0\\Partition2";

//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
	if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
			m_DefaultGamepad.fY1 == -1)
	{
		m_nActiveEntry++; 
		m_nActiveEntry = m_nActiveEntry < countof(EntryPoints) ? m_nActiveEntry : 0;

	}

	if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ||
			m_DefaultGamepad.fY1 == 1)
	{
		m_nActiveEntry--; 
		m_nActiveEntry = m_nActiveEntry >= 0 ? m_nActiveEntry : 0;

	}

    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
		LD_LAUNCH_DASHBOARD launchData;
		ZeroMemory( &launchData, sizeof( launchData ) );
		// are we in the boundaries of the array
		ASSERT(m_nActiveEntry < countof(dwReasons));

		launchData.dwReason = (m_nActiveEntry > 5)? dwReasons[5] :dwReasons[m_nActiveEntry];
		if(m_nActiveEntry > 5)
		{
			// are we in the boundaries of the array
			ASSERT((m_nActiveEntry - 6) < countof(dwParameter1s));
		}
		launchData.dwParameter1 = (m_nActiveEntry > 5)? dwParameter1s[m_nActiveEntry - 6] : 0; 
		launchData.dwContext = launchData.dwReason;		

		NTSTATUS status = WriteTitleInfoAndReboot( cszXbeName, cszXbePath, LDT_LAUNCH_DASHBOARD, 0xFFFE0000, (PLAUNCH_DATA)&launchData);
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    m_Font.Begin();

	float fYPos = 50.0f;

	for (int i = 0; i<countof(EntryPoints); i++)
	{
		
		m_Font.DrawText( 64,fYPos , i == m_nActiveEntry? 0xffff8080: 0xffffffff, EntryPoints[i]);
		fYPos += 30.0f;
	}

    m_Font.End();

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

