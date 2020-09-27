//-----------------------------------------------------------------------------
// File: XODash.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


#include "std.h"

#include "XODash.h"

#include "utilities.h"
#include "XOConst.h"
#include "Materiallibrary.h"
#include "globals.h"
#include "DSoundManager.h"
#include "ntiosvc.h"
#include "Translator.h"

#include "keys.cpp"

CNtIoctlCdromService g_cdrom;
int g_nDiscType = eDiscNone;
int GetDiscType();
// CInputHandle g_szUserInput;
//-----------------------------------------------------------------------------
// Global access to common members
//-----------------------------------------------------------------------------
XOApplication*    g_pXBApp     = NULL;
LPDIRECT3DDEVICE8  g_pd3dDevice = NULL;
CMaterialLibrary   g_MaterialLib;

float* g_fAppTime = 0;
TCHAR* g_szAppDir = MEDIA_PATH; // _T("d:\\media\\");

//
// Global task handle for update task
//
XONLINETASK_HANDLE	g_hTask = NULL;
XONLINETASK_HANDLE	g_hLogon = NULL;

//
// Autoupdate service
//
#define NUM_SERVICES	1
DWORD			g_rgServices[NUM_SERVICES] =
{
	XONLINE_AUTO_UPDATE_SERVICE
};



//-----------------------------------------------------------------------------
// Name: XOApplication()
// Desc: Constructor
//-----------------------------------------------------------------------------
XOApplication::XOApplication()
{
    // Initialize member variables
    g_pXBApp          = this;

    // Direct3D variables
    m_pD3D            = NULL;
    m_pd3dDevice      = NULL;
    m_pDepthBuffer    = NULL;
    m_pBackBuffer     = NULL;

    // Variables to perform app timing
    m_fTime           = 0.0f;
    m_fStartTime      = 0.0f;
    m_fElapsedTime    = 0.0f;
	m_fSecsPerTick    = 0.0f;

    // Set up the presentation parameters for a double-bufferd, 640x480,
    // 32-bit display using depth-stencil. Override these parameters in
    // your derived class as your app requires.
    ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
    m_d3dpp.BackBufferWidth        = 640;
    m_d3dpp.BackBufferHeight       = 480;
    m_d3dpp.BackBufferFormat       = D3DFMT_A8R8G8B8;
    m_d3dpp.BackBufferCount        = 1;
    m_d3dpp.EnableAutoDepthStencil = TRUE;
    m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    m_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.FullScreen_RefreshRateInHz      = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    m_d3dpp.MultiSampleType        = D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN;

    // Specify number and type of input devices this app will be using. By
    // default, you can use 0 and NULL, which triggers XInputDevices() to
    // prealloc the default number and types of devices. To use chat or
    // other devices, override these variables in your derived class.
    m_dwNumInputDeviceTypes = 0;
    m_InputDeviceTypes      = NULL;
	g_fAppTime = &m_fTime;
    _tcscpy( m_szAppDir,g_szAppDir);
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create the app
//-----------------------------------------------------------------------------
HRESULT XOApplication::Create()
{
    HRESULT hr;
	XONLINE_STARTUP_PARAMS	xosp = { 0 };

    float zero = 0.0f;
	float one = 1.0f;


    // Create the Direct3D object
    DbgPrint( "XBApp: Creating Direct3D...\n" );
    if( NULL == ( m_pD3D = Direct3DCreate8(D3D_SDK_VERSION) ) )
    {
        DbgPrint( "XBApp: Unable to create Direct3D!\n");
        return E_FAIL;
    }

    // Create the device
    DbgPrint( "XBApp: Creating the D3D device...\n");
    if( FAILED( hr = m_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL, 
                                           D3DCREATE_HARDWARE_VERTEXPROCESSING, 
                                           &m_d3dpp, &m_pd3dDevice ) ) )
    {
        DbgPrint( "XBApp: Could not create D3D device!\n");
        return hr;
    }

    // Allow global access to the device
    g_pd3dDevice = m_pd3dDevice;

    // Store pointers to the depth and back buffers
    m_pd3dDevice->GetDepthStencilSurface( &m_pDepthBuffer );
    m_pd3dDevice->GetBackBuffer( 0, 0, &m_pBackBuffer );

    // Initialize core peripheral port support. Note: If these parameters
    // are 0 and NULL, respectively, then the default number and types of 
    // controllers will be initialized.
    XInitDevices( m_dwNumInputDeviceTypes, m_InputDeviceTypes );

    // Create the gamepad devices
    DbgPrint( "XBApp: Creating gamepad devices...\n");
    if( FAILED( hr = XBInput_CreateGamepads( &m_Gamepad ) ) )
    {
        DbgPrint( "XBApp: Call to CreateGamepads() failed!\n" );
        return hr;
    }

    // Initialize the app's device-dependent objects
    DbgPrint("XBApp: Initializing the app...\n");
    if( FAILED( hr = Initialize() ) )
    {
        DbgPrint("XBApp: Call to Initialize() failed!\n" );
        return hr;
    }

	
	LARGE_INTEGER qwTicksPerSec;
	QueryPerformanceFrequency( &qwTicksPerSec );
	m_fSecsPerTick = 1.0f / (FLOAT)qwTicksPerSec.QuadPart;


    g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	g_pd3dDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, 0);
	g_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(LPDWORD)&one);
	g_pd3dDevice->SetRenderState(D3DRS_EDGEANTIALIAS, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZBIAS, 0);
	g_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILREF, 0);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	g_pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	g_pd3dDevice->SetRenderState(D3DRS_WRAP0, 0);
	g_pd3dDevice->SetRenderState(D3DRS_WRAP1, 0);
	g_pd3dDevice->SetRenderState(D3DRS_WRAP2, 0);
	g_pd3dDevice->SetRenderState(D3DRS_WRAP3, 0);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0);
	g_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_LOCALVIEWER, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR2);
	g_pd3dDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
	g_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE, *(LPDWORD)&one);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE_MIN, *(LPDWORD)&one);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_A, *(LPDWORD)&one);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_B, *(LPDWORD)&zero);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_C, *(LPDWORD)&zero);
	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEMASK, 0xffffffff);
	g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

    OutputDebugStringA("Calling XOnlineStartup...\n");
    hr = XOnlineStartup(&xosp);
    if (FAILED(hr))
    {
        OutputDebugStringA("Error in XOnlineStartup\n");
    }


    return hr;
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Cleanup objects
//-----------------------------------------------------------------------------
VOID XOApplication::Destroy()
{
    // Perform app-specific cleanup
    Cleanup();

    // Release display objects
    SAFE_RELEASE( m_pd3dDevice );
    SAFE_RELEASE( m_pD3D );
}

//-----------------------------------------------------------------------------
// Name: RebootWhilePreservingDDrive()
// Desc: This is a straight port of XLaunchNewImage!!
//-----------------------------------------------------------------------------
const OBJECT_STRING	   g_DDosDevicePrefix = CONSTANT_OBJECT_STRING(OTEXT("\\??\\D:"));
const CHAR             g_szDVDDevicePrefix[] = "\\Device\\Cdrom0";

DWORD XOApplication::RebootWhilePreservingDDrive(
		    LPCSTR lpTitlePath,
		    PLD_FROM_TITLE_UPDATE pldftu
		    )
{
    //
    // We require the path is NULL or D:\something
    //

    if ((NULL != lpTitlePath) &&
        ((('D' != lpTitlePath[0]) && ('d' != lpTitlePath[0])) ||
         (':' != lpTitlePath[1]) ||
         ('\\' != lpTitlePath[2])))
    {
    	RIP_ON_NOT_TRUE("XOApplication::RebootWhilePreservingDDrive", (FALSE));
        return(ERROR_INVALID_PARAMETER);
    }

    if (NULL == lpTitlePath)
    {
        //
        // NULL lpTitlePath means reboot to the dashboard - don't preserve D: drive mapping
        //

        return(XWriteTitleInfoAndReboot(NULL,
                                        NULL,
                                        LDT_FROM_TITLE_UPDATE,
                                        XeImageHeader()->Certificate->TitleID,
                                        (PLAUNCH_DATA)pldftu));
    }
    else
    {
        NTSTATUS Status;
        OBJECT_ATTRIBUTES Obja;
        HANDLE LinkHandle;
        OCHAR szLinkTarget[MAX_PATH * 2];
        OCHAR szDDrivePath[MAX_PATH * 2];
        OBJECT_STRING LinkTarget;
        int cch;

        szDDrivePath[0] = '\0';

        //
        // If the D: drive is mapped in some unique fashion, preserve that mapping
        // across the reboot
        //

        InitializeObjectAttributes(&Obja,
                                   (POBJECT_STRING) &g_DDosDevicePrefix,
                                   OBJ_CASE_INSENSITIVE,
                                   NULL,
                                   NULL);

        Status = NtOpenSymbolicLinkObject(&LinkHandle, &Obja);

        if (NT_SUCCESS(Status))
        {
            LinkTarget.Buffer = szLinkTarget;
            LinkTarget.Length = 0;
            LinkTarget.MaximumLength = sizeof(szLinkTarget);

            Status = NtQuerySymbolicLinkObject(LinkHandle, &LinkTarget, NULL);

            NtClose(LinkHandle);
        }

        if (NT_SUCCESS(Status))
        {
            lstrcpynA(szDDrivePath,
                      szLinkTarget,
                      min(ARRAYSIZE(szDDrivePath), (LinkTarget.Length / sizeof(CHAR)) + 1));
        }
        else
        {
            /* Got no path.  The D: drive should always be mapped to something, so
             * we're in trouble if we get here.  Fall back on the DVD device
             * path */
            ASSERT(FALSE);
            strcpy(szDDrivePath, g_szDVDDevicePrefix);
        }

        return(XWriteTitleInfoAndReboot(&(lpTitlePath[3]),
                                        szDDrivePath,
                                        LDT_FROM_TITLE_UPDATE,
                                        XeImageHeader()->Certificate->TitleID,
                                        (PLAUNCH_DATA)pldftu));
    }
}

//-----------------------------------------------------------------------------
// Name: Run()
// Desc: 
//-----------------------------------------------------------------------------

INT XOApplication::Run()
{
    DbgPrint("XBApp: Running the application...\n");
	
	HRESULT hrRender = S_OK;
    HRESULT hrFrameMove = S_OK;

	HRESULT	hr = S_OK;
    DWORD		dwError;
    DWORD		dwType;
    LAUNCH_DATA	ld;
    PLAUNCH_UPDATE_INFO plui = (PLAUNCH_UPDATE_INFO)&ld;
    LD_FROM_TITLE_UPDATE ldftu;

    PBYTE	pbSymmetricKey;
    PBYTE	pbPublicKey;
    PBYTE	pbTitleKey;
    DWORD	cbPublicKey;

    CHAR	szMessage[MAX_PATH];

	LARGE_INTEGER qwTime;
	QueryPerformanceCounter( &qwTime ); 
	m_fStartTime =  m_fSecsPerTick * ((FLOAT)(qwTime.QuadPart));

	// Get the launch info
	dwError = XGetLaunchInfo(&dwType, &ld);
	if (dwError != ERROR_NOT_FOUND)
	{
		OutputDebugStringA("Got Launch data ...\n");
		
		if (dwError != ERROR_SUCCESS)
		{
			sprintf(szMessage, "Launch data error: %x\n", dwError);
			OutputDebugStringA(szMessage);
			hr = HRESULT_FROM_WIN32(dwError);
			goto Error;
		}
		if (dwType != LDT_TITLE_UPDATE)
		{
			sprintf(szMessage, "Bad LDT: %u\n", dwType);
			OutputDebugStringA(szMessage);
			hr = HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
			goto Error;
		}

		// Kick off the update
		if (plui->dwUpdateFlags & LAUNCH_UPDATE_FLAG_ONLINE)
		{

			// Logon with no users
			XONLINE_USER Users[4];
		    DWORD i;
                                                
			OutputDebugStringA("Doing Online update\n");

			for (i = 0; i < 4; i += 1)
				Users[i].xuid.qwUserID = 0;
			hr = XOnlineLogon(Users, g_rgServices, NUM_SERVICES, NULL, &g_hLogon);
			if (FAILED(hr))
			{
				sprintf(szMessage, "Logon failed hr: %08x\n", hr);
				OutputDebugStringA(szMessage);				
				goto Error;
			}

			// Pump until logon returns
			while( (hr = XOnlineTaskContinue(g_hLogon)) == XONLINETASK_S_RUNNING )
				;
			if (FAILED(hr))
			{
				sprintf(szMessage, "Logon TaskContinue failed hr: %08x\n", hr);
				OutputDebugStringA(szMessage);				
				goto Error;
			}
			
			// Online update
			hr = XOnlineTitleUpdateInternal(
						plui->dwTitleId,
						plui->dwBaseVersion,
						plui->rgbTitleKey,
						NULL, &g_hTask);
		}
		else
		{
			if (plui->dwUpdateFlags & LAUNCH_UPDATE_FLAG_USE_DEFAULT_KEYS)
			{
				OutputDebugStringA("Using default keys\n");
			
				// We use the built-in keys for this
				pbSymmetricKey = (PBYTE)g_rgbSymmetricKey;
				pbPublicKey = g_rgbPublicKey;
				cbPublicKey = g_cbPublicKey;
				pbTitleKey = g_rgbTitleKey;
			}
			else
			{
				pbSymmetricKey = plui->rgbSymmetricKey;
				pbPublicKey = plui->rgbPublicKey;
				cbPublicKey = plui->cbPublicKey;
				pbTitleKey = plui->rgbTitleKey;
			}

			OutputDebugStringA("Doing DVD update\n");
			sprintf(szMessage, "  Update path: %s\n", plui->szUpdatePath);
			OutputDebugStringA(szMessage);
			sprintf(szMessage, "  Title ID: %08x\n", plui->dwTitleId);
			OutputDebugStringA(szMessage);
			sprintf(szMessage, "  dwBaseVersion: %08x\n", plui->dwBaseVersion);
			OutputDebugStringA(szMessage);
			sprintf(szMessage, "  cbPublicKey: %u\n", cbPublicKey);
			OutputDebugStringA(szMessage);

			// Offline update
			hr = XOnlineTitleUpdateFromDVDInternal(
						plui->szUpdatePath,
						plui->dwTitleId,
						plui->dwBaseVersion,
						pbTitleKey,
						pbSymmetricKey,
						XONLINE_KEY_LENGTH,
						pbPublicKey,
						cbPublicKey,
						NULL, &g_hTask);
		}
	}
	else
	{
		hr = E_FAIL;
		RIP("Failed");
	}

	if (FAILED(hr))
	{
		// BUGBUG: Need to display some UI here
		sprintf(szMessage, "Fialed hr: %08x\n", hr);
		OutputDebugStringA(szMessage);
		goto Error;
	}

    // Run the game loop, animating and rendering frames
    while( SUCCEEDED(hrFrameMove) && SUCCEEDED(hrRender) )
    {

		// Pump the task
		if (g_hLogon)
		{
			hr = XOnlineTaskContinue(g_hLogon);
			if (FAILED(hr))
			{
				// BUGBUG: Need to display some UI here
				sprintf(szMessage, "Logon TaskContinue Fialed hr: %08x\n", hr);
				OutputDebugStringA(szMessage);
				goto Error;
			}
		}

		hr = XOnlineTaskContinue(g_hTask);
		if (XONLINETASK_STATUS_AVAILABLE(hr))
			goto Reboot;
			
		if (FAILED(hr))
		{
			// BUGBUG: Need to display some UI here
			sprintf(szMessage, "TaskContinue Fialed hr: %08x\n", hr);
			OutputDebugStringA(szMessage);
			goto Error;
		}
    
        //-----------------------------------------
        // Animate and render a frame
        //-----------------------------------------

        hrFrameMove = FrameMove();

        // Render the scene
        // honestly, this really can't fail...
        hrRender = Render();

        // Finally, show the frame (swaps the backbuffer to the front)
        m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    }

Reboot:

	Destroy();
	
	ldftu.dwContext = plui->dwContext;
	ldftu.hr = hr;

	OutputDebugStringA("Rebooting ...\n");

	// Reboot to where we're supposed to go
	RebootWhilePreservingDDrive(
			(plui->dwTitleId == AUTOUPD_DASH_TITLE_ID)?NULL:"d:\\default.xbe",
			&ldftu);

    return hrFrameMove;

Error:
	// BUGBUG: Best place to display UI here ...

	goto Reboot;
}




HRESULT XOApplication::FrameMove()
{
	HRESULT hr = S_OK;

    //-----------------------------------------
    // Handle input
    //-----------------------------------------

    // Read the input for all connected gampads
    XBInput_GetInput( m_Gamepad );

    // Lump inputs of all connected gamepads into one common structure.
    // This is done so apps that need only one gamepad can function with
    // any gamepad.
    ZeroMemory( &m_DefaultGamepad, sizeof(m_DefaultGamepad) );
    for( DWORD i=0; i<4; i++ )
    {
        if( m_Gamepad[i].hDevice )
        {
            m_DefaultGamepad.sThumbLX += m_Gamepad[i].sThumbLX;
            m_DefaultGamepad.sThumbLY += m_Gamepad[i].sThumbLY;
            m_DefaultGamepad.sThumbRX += m_Gamepad[i].sThumbRX;
            m_DefaultGamepad.sThumbRY += m_Gamepad[i].sThumbRY;

            m_DefaultGamepad.fX1      += m_Gamepad[i].fX1;
            m_DefaultGamepad.fY1      += m_Gamepad[i].fY1;
            m_DefaultGamepad.fX2      += m_Gamepad[i].fX2;
            m_DefaultGamepad.fY2      += m_Gamepad[i].fY2;
            m_DefaultGamepad.wButtons        |= m_Gamepad[i].wButtons;
            m_DefaultGamepad.wPressedButtons |= m_Gamepad[i].wPressedButtons;
            m_DefaultGamepad.wLastButtons    |= m_Gamepad[i].wLastButtons;

            for( DWORD b=0; b<8; b++ )
            {
                m_DefaultGamepad.bAnalogButtons[b]        |= m_Gamepad[i].bAnalogButtons[b];
                m_DefaultGamepad.bPressedAnalogButtons[b] |= m_Gamepad[i].bPressedAnalogButtons[b];
                m_DefaultGamepad.bLastAnalogButtons[b]    |= m_Gamepad[i].bLastAnalogButtons[b];
            }
        }
    }


    // Handle special input combo to trigger a reboot to the Xbox Dashboard
    if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 0 )
    {
        if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 0 )
        {
            if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] )
                XLaunchNewImage( NULL, NULL );
        }
    }

    //-----------------------------------------
    // Perform app timing
    //-----------------------------------------

    // Get the current time (keep in LARGE_INTEGER format for precision)
	LARGE_INTEGER qwTime;
    QueryPerformanceCounter( &qwTime );
    
    // Store the current time values as floating point
    
    m_fElapsedTime    = m_fSecsPerTick * ((FLOAT)(qwTime.QuadPart)) - m_fTime;
    m_fTime           = m_fSecsPerTick * ((FLOAT)(qwTime.QuadPart));

	m_ProgressBar.FrameMove(m_DefaultGamepad, m_fElapsedTime);
	return hr;
}


HRESULT XOApplication::Cleanup()
{
	HRESULT hr = S_OK;
	CTranslator::Instance()->Cleanup();
	return hr;
}

HRESULT XOApplication::Render()
{
	if ( m_pd3dDevice )
	{
	
		m_pd3dDevice->BeginScene();

		// Clear the backbuffer 
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         D3DCOLOR_XRGB( (int)0, (int)0, (int)0 ), 1.0f, 0L );

		m_ProgressBar.Render(0);
		m_pd3dDevice->EndScene();
	}

	return S_OK;
}



HRESULT XOApplication::Initialize()
{
	HRESULT hr = S_OK;
	do
	{
		hr = CTranslator::Instance()-> Initialize();
		BREAKONFAIL(hr, "XOApplication::Initialize - Fail to init XLate table");
		DbgPrint("XOApplication::Initialize - Translator is initialized\n");

		hr = CreateProgressBar();
		BREAKONFAIL(hr, "XOApplication::Initialize - Fail to create progress bar");
		DbgPrint("XOApplication::Initialize - the screen is initialized\n");
		
	}while(0);

	return hr;
}


HRESULT AutoUpdateCB( float* pProgress )
{
	DWORD	dwPercent;
	float	fPercent;

	*pProgress = 0.0;
	if (g_hTask)
	{
		if (SUCCEEDED(XOnlineTitleUpdateGetProgress(g_hTask, &dwPercent, NULL, NULL)))
		{
			fPercent = (float)dwPercent;
			*pProgress = fPercent / 100.0f;
		}
	}
	return(S_OK);
}

HRESULT XOApplication::CreateProgressBar()
{
	static const TCHAR cszText[] = _T("AUTOUPDATE");
	HRESULT hr = m_ProgressBar.Initialize(XBG_PATH_ANSI"gen_mess_panel.xbg", "gen_mess_panel", eProgressBarSceneId, false);
	MSGPARAMETERS params;
	memset( &params, 0, sizeof( MSGPARAMETERS ) );
	params.m_SceneID = eProgressBarSceneId;
	params.m_pStrMessageID = cszText;
	params.m_pFunction = AutoUpdateCB;
	m_ProgressBar.SetParameters(params);
	return hr;
}

