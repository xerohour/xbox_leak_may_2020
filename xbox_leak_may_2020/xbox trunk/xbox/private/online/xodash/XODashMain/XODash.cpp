//-----------------------------------------------------------------------------
// File: XODash.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


#include "std.h"

#include "XODash.h"
#include "Scene.h"

#include "utilities.h"
#include "XOConst.h"
#include "DSoundManager.h"
#include "ntiosvc.h"
#include "globals.h"
#include "XOConst.h"

#include "SceneNavigator.h"
#include "Translator.h"
#include "InputHandle.h"
#include "AccountData.h"
#include "NetConfig.h"

//-----------------------------------------------------------------------------
// Global access to common members
//-----------------------------------------------------------------------------
XOApplication*    g_pXBApp     = NULL;
LPDIRECT3DDEVICE8  g_pd3dDevice = NULL;
CMaterialLibrary   g_MaterialLib;

float* g_fAppTime = 0;
TCHAR* g_szAppDir = MEDIA_PATH; // _T("d:\\media\\");

// audio related const
CNtIoctlCdromService g_cdrom;
int g_nDiscType = eDiscNone;
int GetDiscType();
CInputHandle g_szUserInput;
bool g_ResetAutomation = true;
CNetConfig g_NetConfig;

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

	
	if(FAILED(hr = MapDrive(DASH_DRIVE_LETTER, DASH_PARTITION)) )
	{
		DbgPrint("XOApplication:Create - fail to map drive\n");
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
	// erase the drive mapping
	UnMapDrive(DASH_DRIVE_LETTER); 
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

	LARGE_INTEGER qwTime;
	QueryPerformanceCounter( &qwTime ); 
	m_fStartTime =  m_fSecsPerTick * ((FLOAT)(qwTime.QuadPart));

    // Run the game loop, animating and rendering frames
    while( SUCCEEDED(hrFrameMove) && SUCCEEDED(hrRender) )
    {
    
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

    return hrFrameMove;
}



void XOApplication::ProcessUserInput()
{
	HRESULT hr = S_OK;

    // Read the input for all connected gampads
    XBInput_GetInput( m_Gamepad );

    // Lump inputs of all connected gamepads into one common structure.
    // This is done so apps that need only one gamepad can function with
    // any gamepad.
    
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

}

HRESULT XOApplication::FrameMove()
{
	HRESULT hr = S_OK;
	ZeroMemory( &m_DefaultGamepad, sizeof(m_DefaultGamepad) );

    //-----------------------------------------
    // Handle input
    //-----------------------------------------
	// if automation test are active, ignore all inputs from the pereferral devices
	if (m_AutomationSocket.IsAutomationActive())
	{
		CAutomationMsg msg;
		if(m_AutomationSocket.ReceiveMessage(&msg))
		{
			msg.SetGamepadData(&m_DefaultGamepad);
		}
	}
	else
	{
		ProcessUserInput();
	}
  
	//-----------------------------------------
    // Perform app timing
    //-----------------------------------------

    // Get the current time (keep in LARGE_INTEGER format for precision)
	LARGE_INTEGER qwTime;
    QueryPerformanceCounter( &qwTime );
    
    // Store the current time values as floating point
    
    m_fElapsedTime    = m_fSecsPerTick * ((FLOAT)(qwTime.QuadPart)) - m_fTime;
    
    // don't screw up animation's or anything just because we're debugging
    if ( m_fElapsedTime > .1f )
        m_fElapsedTime = .1f;
    m_fTime           = m_fSecsPerTick * ((FLOAT)(qwTime.QuadPart));
    
	hr = CSceneNavigator::Instance()->FrameMove(m_DefaultGamepad, m_fElapsedTime);
	return hr;
}


HRESULT XOApplication::Cleanup()
{
	HRESULT hr = S_OK;

    // clean up more sounds
    CPrimitiveScene::CleanupSounds();

	hr = DSoundManager::Instance()->Cleanup();
	if (FAILED(hr))
	{
		DbgPrint("XOApplication::Cleanup - fail to cleanup DirectSound, hr = 0x%x", hr);
	}
	else
	{
		DbgPrint("XOApplication::Cleanup - DirectSound clean");
	}

    // clean the translator

	CTranslator::Instance()->Cleanup();

	// now the navigator
    hr = CSceneNavigator::Instance()->Cleanup();
	if (FAILED(hr))
	{
		DbgPrint("XOApplication::Cleanup - fail to cleanup SceneNavigator, hr = 0x%x", hr);
	}
	else
	{
		DbgPrint("XOApplication::Cleanup - SceneNavigator clean");
	}

    // clean up accounts
    hr = Accounts::CleanUp();
   	if (FAILED(hr))
	{
		DbgPrint("XOApplication::Cleanup - fail to cleanup Accounts, hr = 0x%x", hr);
	}
	else
	{
        DbgPrint("XOApplication::Cleanup - Accounts clean");
	}


//	m_AutomationSocket.Close(true);
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

		CSceneNavigator::Instance()->Render(0);

		m_pd3dDevice->EndScene();
	}

	return S_OK;
}



HRESULT XOApplication::InitAudio()
{
	HRESULT hr = S_OK;
	do
	{
	// retreive the status of DVD/CD Rom 
		g_cdrom.Open(1);
        g_nDiscType = GetDiscType();
	
		// initialize DSound 
		if(!DSoundManager::Instance())
		{
			hr = E_OUTOFMEMORY; 
			BREAKONFAIL(hr,"XOApplication::InitAudio - fail to create DSoundManager");
		}
		
		hr = DSoundManager::Instance()->Initialize();
		BREAKONFAIL(hr, "XOApplication::InitAudio fail to init DSound");
	
	}while(0);

	return hr;

}

HRESULT XOApplication::Initialize()
{
	HRESULT hr = S_OK;
	do
	{
		hr = InitAudio();
		BREAKONFAIL(hr, "XOApplication::Initialize - Fail to init DirectSound");
		DbgPrint("XOApplication::Initialize - Audio is initialized\n");
		
		hr = CTranslator::Instance()-> Initialize();
		BREAKONFAIL(hr, "XOApplication::Initialize - Fail to init XLate table");
		DbgPrint("XOApplication::Initialize - Translator is initialized\n");

		hr = CSceneNavigator::Instance()->Initialize();
		BREAKONFAIL(hr, "XOApplication::Initialize - Fail to init SceneNavigator");
		DbgPrint("XOApplication::Initialize - SceneNavigator is initialized\n");

        hr = Accounts::Initialize();
   		BREAKONFAIL(hr, "XOApplication::Initialize - Fail to init accounts");
		DbgPrint("XOApplication::Initialize - accounts is initialized\n");
		
	}while(0);

	return hr;
}



// Map a drive to a partiticular drive letter
HRESULT XOApplication::MapDrive( char cDriveLetter, char* pszPartition )
{
    ASSERT( pszPartition != NULL );

	UnMapDrive( cDriveLetter);
	DbgPrint("Mapping partition [%s] to drive [%c]\n", pszPartition, cDriveLetter);
    char pszDrive[20];
    ZeroMemory( pszDrive, 20 );

    sprintf( pszDrive, "\\??\\%c:", cDriveLetter );

    ANSI_STRING ostDrive, ostPath;

    RtlInitObjectString( &ostDrive, pszDrive );
    RtlInitObjectString( &ostPath, pszPartition );
    NTSTATUS status = IoCreateSymbolicLink( &ostDrive, &ostPath );

	DbgPrint("MapDrive, status = [%d]\n", status);

    return HRESULT_FROM_WIN32(RtlNtStatusToDosError( status ));
}


// Un-map a drive from the specified drive letter
HRESULT XOApplication::UnMapDrive( char cDriveLetter )
{
    char pszDrive[20];
    ZeroMemory( pszDrive, 20 );

    sprintf( pszDrive, "\\??\\%c:", cDriveLetter );

    ANSI_STRING ostDrive;

    RtlInitObjectString( &ostDrive, pszDrive );
    NTSTATUS status = IoDeleteSymbolicLink(&ostDrive);

    return HRESULT_FROM_WIN32(RtlNtStatusToDosError( status ));
}

