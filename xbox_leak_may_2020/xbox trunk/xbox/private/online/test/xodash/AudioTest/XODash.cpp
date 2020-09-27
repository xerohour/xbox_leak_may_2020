//-----------------------------------------------------------------------------
// File: XBApp.cpp
//
// Desc: Application class for the XBox samples.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//       12.19.01 - Changes for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "std.h"
#include "XODash.h"

#include "XOConst.h"
#include "utilities.h"
#include "DSoundManager.h"
#include "ntiosvc.h"


//-----------------------------------------------------------------------------
// Global access to common members
//-----------------------------------------------------------------------------
XOApplication*    g_pXBApp     = NULL;
LPDIRECT3DDEVICE8  g_pd3dDevice = NULL;
DSoundManager* g_pDSoundManager = NULL;
CNtIoctlCdromService g_cdrom;
int g_nDiscType = eDiscNone;


float* g_fAppTime = 0;
TCHAR* g_szAppDir = NULL;

static TCHAR* szInputFiles[] = 
{			_T("cd:1"),
			_T("d:\\media\\8k16bs.wav"),
			_T("d:\\media\\becky.wma"),
			
			
	
		};
int nNumOfFiles = countof( szInputFiles);


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
    m_bPaused         = FALSE;
    m_fTime           = 0.0f;
    m_fElapsedTime    = 0.0f;
    m_fAppTime        = 0.0f;
    m_fElapsedAppTime = 0.0f;
    m_fFPS            = 0.0f;
    m_strFrameRate[0] = L'\0';

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

    // Specify number and type of input devices this app will be using. By
    // default, you can use 0 and NULL, which triggers XInputDevices() to
    // prealloc the default number and types of devices. To use chat or
    // other devices, override these variables in your derived class.
    m_dwNumInputDeviceTypes = 0;
    m_InputDeviceTypes      = NULL;
	g_fAppTime = &m_fAppTime;
	g_szAppDir = m_szAppDir;

	m_nCurrentFile = 0;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create the app
//-----------------------------------------------------------------------------
HRESULT XOApplication::Create()
{
    HRESULT hr;

    // Create the Direct3D object
    DbgPrint("XBApp: Creating Direct3D...\n");
    if( NULL == ( m_pD3D = Direct3DCreate8(D3D_SDK_VERSION) ) )
    {
        DbgPrint("XBApp: Unable to create Direct3D!\n");
        return E_FAIL;
    }

    // Create the device
    DbgPrint( "XBApp: Creating the D3D device...\n" );
    if( FAILED( hr = m_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL, 
                                           D3DCREATE_HARDWARE_VERTEXPROCESSING, 
                                           &m_d3dpp, &m_pd3dDevice ) ) )
    {
        DbgPrint( "XBApp: Could not create D3D device!\n" );
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
        DbgPrint( "XBApp: Call to Initialize() failed!\n");
        return hr;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Cleanup objects
//-----------------------------------------------------------------------------
VOID XOApplication::Destroy()
{
    // Perform app-specific cleanup
    Cleanup();

	DSoundManager::Instance()->Cleanup();
    // Release display objects
    SAFE_RELEASE( m_pd3dDevice );
    SAFE_RELEASE( m_pD3D );
}




//-----------------------------------------------------------------------------
// Name: Run()
// Desc: 
//-----------------------------------------------------------------------------
INT XOApplication::Run()
{
    DbgPrint("XBApp: Running the application...\n");
	
	HRESULT hr;
    // Run the game loop, animating and rendering frames
    while( TRUE )
    {
    
        //-----------------------------------------
        // Animate and render a frame
        //-----------------------------------------
		
		hr = Update();
        // Frame move the scene
        FrameMove();

        // Render the scene
        Render();

        // Finally, show the frame (swaps the backbuffer to the front)
        m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    }

    return 0;
}



HRESULT XOApplication::Update()
{
	HRESULT hr = S_OK;
	// Get the frequency of the timer
    LARGE_INTEGER qwTicksPerSec;
    QueryPerformanceFrequency( &qwTicksPerSec );
    FLOAT fSecsPerTick = 1.0f / (FLOAT)qwTicksPerSec.QuadPart;

    // Save the start time
    LARGE_INTEGER qwTime, qwLastTime, qwElapsedTime;
    QueryPerformanceCounter( &qwTime );
    qwLastTime.QuadPart = qwTime.QuadPart;

    LARGE_INTEGER qwAppTime, qwElapsedAppTime;
    qwAppTime.QuadPart        = 0;
    qwElapsedTime.QuadPart    = 0;
    qwElapsedAppTime.QuadPart = 0;
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

        // Check Start button
        if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START )
            m_bPaused = !m_bPaused;

        // Get the current time (keep in LARGE_INTEGER format for precision)
        QueryPerformanceCounter( &qwTime );
        qwElapsedTime.QuadPart = qwTime.QuadPart - qwLastTime.QuadPart;
        qwLastTime.QuadPart    = qwTime.QuadPart;
        if( m_bPaused )
            qwElapsedAppTime.QuadPart = 0;
        else
            qwElapsedAppTime.QuadPart = qwElapsedTime.QuadPart;
        qwAppTime.QuadPart    += qwElapsedAppTime.QuadPart;

        // Store the current time values as floating point
        m_fTime           = fSecsPerTick * ((FLOAT)(qwTime.QuadPart));
        m_fElapsedTime    = fSecsPerTick * ((FLOAT)(qwElapsedTime.QuadPart));
        m_fAppTime        = fSecsPerTick * ((FLOAT)(qwAppTime.QuadPart));
        m_fElapsedAppTime = fSecsPerTick * ((FLOAT)(qwElapsedAppTime.QuadPart));

        // Compute the FPS (frames per second) once per second
        static FLOAT fLastTime = 0.0f;
        static DWORD dwFrames  = 0L;
        dwFrames++;

        if( m_fTime - fLastTime > 1.0f )
        {
            m_fFPS    = dwFrames / ( m_fTime - fLastTime );
            fLastTime = m_fTime;
            dwFrames  = 0L;
            swprintf( m_strFrameRate, L"%0.02f fps", m_fFPS );
        }
		return hr;
}

//-----------------------------------------------------------------------------
// Name: RenderGradientBackground()
// Desc: Draws a gradient filled background
//-----------------------------------------------------------------------------
HRESULT XOApplication::RenderGradientBackground( DWORD dwTopColor, 
                                                  DWORD dwBottomColor )
{
    // First time around, allocate a vertex buffer
    static LPDIRECT3DVERTEXBUFFER8 g_pVB  = NULL;
    if( g_pVB == NULL )
    {
        m_pd3dDevice->CreateVertexBuffer( 4*5*sizeof(FLOAT), D3DUSAGE_WRITEONLY, 
                                          0L, D3DPOOL_DEFAULT, &g_pVB );
        struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
        BACKGROUNDVERTEX* v;
        g_pVB->Lock( 0, 0, (BYTE**)&v, 0L );
        v[0].p = D3DXVECTOR4(   0,   0, 1.0f, 1.0f ); v[0].color = dwTopColor;
        v[1].p = D3DXVECTOR4( 640,   0, 1.0f, 1.0f ); v[1].color = dwTopColor;
        v[2].p = D3DXVECTOR4(   0, 480, 1.0f, 1.0f ); v[2].color = dwBottomColor;
        v[3].p = D3DXVECTOR4( 640, 480, 1.0f, 1.0f ); v[3].color = dwBottomColor;
        g_pVB->Unlock();
    }

    // Set states
    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
    m_pd3dDevice->SetStreamSource( 0, g_pVB, 5*sizeof(FLOAT) );

    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Clear the zbuffer
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0L );

    return S_OK;
}

HRESULT XOApplication::Cleanup()
{
	m_AudioClip.Cleanup();

	return 0;
}

HRESULT XOApplication::Render()
{
	if ( m_pd3dDevice )
	{
	
		m_pd3dDevice->BeginScene();

		// Clear the backbuffer 
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         D3DCOLOR_XRGB( (int)0, (int)0, (int)0 ), 1.0f, 0L );
//////////////
    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"BackgroundMusic" );
  /*      m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        WCHAR szSoundtrack[ MAX_SOUNDTRACK_NAME ];
        WCHAR szSong[ MAX_SONG_NAME ];
        DWORD dwLength;
        WCHAR sz[100];

        // Get information about what song/soundtrack is currently selected
        m_pMusicManager->GetCurrentInfo( szSoundtrack, szSong, &dwLength );
        swprintf( sz, L"Soundtrack: %s", szSoundtrack );
        m_Font.DrawText( 64, 80, 0xffffffff, sz );

        swprintf( sz, L"Song: %s", szSong );
        m_Font.DrawText( 64, 110, 0xffffffff, sz );

        FLOAT fPos = m_pMusicManager->GetPlaybackPosition();
        swprintf( sz, L"Position: %02d:%02d / %02d:%02d", DWORD(fPos / 60), 
                                                          (DWORD)fPos % 60, 
                                                          ( dwLength / 60000 ), 
                                                          ( dwLength / 1000 ) % 60 );
        m_Font.DrawText( 64, 140, m_pMusicManager->GetStatus() == MM_PLAYING ? 0xffffffff : 0xff808080, sz );

        swprintf( sz, L"Mode: %s %s", m_pMusicManager->GetRandom() ? L"Random" : L"Sequential", 
                                      m_pMusicManager->GetGlobal() ? L"Global" : L"Local" );
        m_Font.DrawText( 64, 170, 0xffffff00, sz );

        swprintf( sz, L"Volume %ddB", m_pMusicManager->GetVolume() / 100 );
        m_Font.DrawText( 64, 200, 0xffffff00, sz );
            
*/
        m_Font.End();
		static bool bStartedPlay = false;
		if(!bStartedPlay )
		{
			m_AudioClip.Play();
		}
		bStartedPlay = true;
/////////////
		m_pd3dDevice->EndScene();
	}

	return S_OK;
}


HRESULT XOApplication::FrameMove()
{
	if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
    {
        m_AudioClip.PlayOrPause();
	}

	if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
    {
        m_AudioClip.Stop();
	}
	if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A] )
    {
        m_AudioClip.Play();
	}
	if (m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
	{
		m_AudioClip.Stop();
		m_nCurrentFile--;
		if(m_nCurrentFile < 0 )
		{
			m_nCurrentFile = 0;
		}
		m_AudioClip.m_url = szInputFiles[m_nCurrentFile];
		m_AudioClip.Play();
	}
	
	if (m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
	{
		m_AudioClip.Stop();
		m_nCurrentFile++;
		if(m_nCurrentFile >= nNumOfFiles)
		{
			m_nCurrentFile = 0;
		}

		m_AudioClip.m_url = szInputFiles[m_nCurrentFile];
		m_AudioClip.Play();
	}

    return S_OK;
}


HRESULT XOApplication::Initialize()
{
	HRESULT hr = S_OK;
	do 
	{
		if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
		{
			hr = XBAPPERR_MEDIANOTFOUND;
		}

		
		DiscDrive_Init();
		g_pDSoundManager = DSoundManager::Instance();
		if(!g_pDSoundManager)
		{
			DbgPrint("XOApplication::Initialize() fail to create DSoundManager");
			hr = E_OUTOFMEMORY;
			break;
		}
		hr = g_pDSoundManager->Initialize();
		if(FAILED(hr))
		{
			DbgPrint("XOApplication::Initialize() fail to init DSound");
			break;
		}

		m_AudioClip.m_url = szInputFiles[m_nCurrentFile];

		m_AudioClip.m_volume = 1.00f;
		m_AudioClip.Initialize();
	
	}while (0);

	return hr;
}

int GetDiscType();
void XOApplication::DiscDrive_Init()
{
    g_cdrom.Open(1);
    g_nDiscType = GetDiscType();
}


