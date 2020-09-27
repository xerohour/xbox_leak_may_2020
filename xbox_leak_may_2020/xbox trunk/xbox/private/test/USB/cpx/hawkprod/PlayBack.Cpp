//-----------------------------------------------------------------------------
// File: Playback.cpp
//

//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "Playback.h"

CXHawkPlayback*    g_pXBApp     = NULL;
LPDIRECT3DDEVICE8  g_pd3dDevice = NULL;

//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXHawkPlayback xbApp;

    if( FAILED(xbApp.Create()) )
        return;
	
    xbApp.Run();
}


//-----------------------------------------------------------------------------
// Name: CXHawkPlayback()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXHawkPlayback::CXHawkPlayback()
{
   	m_bStartedSound		= FALSE;
	m_bXHawkConnected	= FALSE;
	m_pNewSound			= NULL;
	m_eTestStep			= WAIT_FOR_ENUM;
    m_pVB				= NULL;

	memset(&m_pGamepadTexture,0,6*sizeof(void*));
	 // Initialize member variables
    g_pXBApp          = this;

    // Direct3D variables
    m_pD3D            = NULL;
    m_pd3dDevice      = NULL;

    // Variables to perform app timing
    m_bPaused         = FALSE;
   

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
}


//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create the app
//-----------------------------------------------------------------------------
HRESULT CXHawkPlayback::Create()
{
    HRESULT hr;

    // Create the Direct3D object
    OutputDebugString( _T("XBApp: Creating Direct3D...\n") );
    if( NULL == ( m_pD3D = Direct3DCreate8(D3D_SDK_VERSION) ) )
    {
        OutputDebugString( _T("XBApp: Unable to create Direct3D!\n") );
        return E_FAIL;
    }

    // Create the device
    OutputDebugString( _T("XBApp: Creating the D3D device...\n") );
    if( FAILED( hr = m_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL, 
                                           D3DCREATE_HARDWARE_VERTEXPROCESSING, 
                                           &m_d3dpp, &m_pd3dDevice ) ) )
    {
        OutputDebugString( _T("XBApp: Could not create D3D device!\n") );
        return hr;
    }

    // Allow global access to the device
    g_pd3dDevice = m_pd3dDevice;

    // Clear the backbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x00ff0000, 1.0f, 0L );
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    
    // Initialize the app's device-dependent objects
    OutputDebugString( _T("XBApp: Initializing the app...\n") );
    if( FAILED( hr = Initialize() ) )
    {
        OutputDebugString( _T("XBApp: Call to Initialize() failed!\n") );
        return hr;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXHawkPlayback::Initialize()
{
	 HRESULT hr;
    
     // Create the gamepad texture
    if( FAILED( XBUtil_CreateTexture( m_pd3dDevice, "Textures\\insertx.bmp",
                                      &m_pGamepadTexture[0] ) ) )
        return E_FAIL;
	if( FAILED( XBUtil_CreateTexture( m_pd3dDevice, "Textures\\Playx.bmp",
                                      &m_pGamepadTexture[1] ) ) )
        return E_FAIL;
	if( FAILED( XBUtil_CreateTexture( m_pd3dDevice, "Textures\\Recordx.bmp",
                                      &m_pGamepadTexture[2] ) ) )
        return E_FAIL;
	if( FAILED( XBUtil_CreateTexture( m_pd3dDevice, "Textures\\Listenx.bmp",
                                      &m_pGamepadTexture[3] ) ) )
        return E_FAIL;
	if( FAILED( XBUtil_CreateTexture( m_pd3dDevice, "Textures\\Outputx.bmp",
                                      &m_pGamepadTexture[4] ) ) )
        return E_FAIL;
	if( FAILED( XBUtil_CreateTexture( m_pd3dDevice, "Textures\\Removex.bmp",
                                      &m_pGamepadTexture[5] ) ) )
        return E_FAIL;
		
    D3DSURFACE_DESC desc;
    m_pGamepadTexture[0]->GetLevelDesc( 0, &desc );
    FLOAT fBorderWidth  = 0;
    FLOAT fBorderHeight = 0;

    // Create a vertex buffer for rendering the help screen
    m_pd3dDevice->CreateVertexBuffer( 4*6*sizeof(FLOAT), D3DUSAGE_WRITEONLY, 
                                      0L, D3DPOOL_DEFAULT, &m_pVB );
    struct VERTEX { D3DXVECTOR4 p; FLOAT tu, tv; };
    VERTEX* v;
    m_pVB->Lock( 0, 0, (BYTE**)&v, 0 /* not supported on XBox D3DLOCK_DISCARD */ );
    v[0].p = D3DXVECTOR4(   0,   0, 0.0f, 0.0f );
    v[1].p = D3DXVECTOR4( 640,   0, 0.0f, 0.0f );
    v[2].p = D3DXVECTOR4(   0, 480, 0.0f, 0.0f );
    v[3].p = D3DXVECTOR4( 640, 480, 0.0f, 0.0f );
    v[0].tu = 0.0f-fBorderWidth; v[0].tv = 0.0f-fBorderHeight;     
    v[1].tu = 1.0f+fBorderWidth; v[1].tv = 0.0f-fBorderHeight; 
    v[2].tu = 0.0f-fBorderWidth; v[2].tv = 1.0f+fBorderHeight; 
    v[3].tu = 1.0f+fBorderWidth; v[3].tv = 1.0f+fBorderHeight; 
    m_pVB->Unlock();

		//Initialize core peripheral port support
   XDEVICE_PREALLOC_TYPE deviceTypes[] = 
    {
	   {XDEVICE_TYPE_GAMEPAD,2},
        {XDEVICE_TYPE_VOICE_MICROPHONE,1},
        {XDEVICE_TYPE_VOICE_HEADPHONE,1}
    };
    XInitDevices(sizeof(deviceTypes)/sizeof(XDEVICE_PREALLOC_TYPE),deviceTypes);
    OutputDebugString( _T("XBApp: Creating gamepad devices...\n") );
    if( FAILED( hr = XBInput_CreateGamepads( &m_Gamepad ) ) )
    {
        OutputDebugString( _T("XBApp: Call to CreateGamepads() failed!\n") );
        return hr;
    }

    m_pHawkUnits = new CHawkUnits;
	
   // Create the sounds
    m_pSound[0].Create( "Sounds\\SOUND-B.wav", 0 /* DSBCAPS_CTRL3D */ );
    m_pSound[1].Create( "Sounds\\SOUND-A.wav", 0 /* DSBCAPS_CTRL3D */ );
    m_pSound[2].Create( "Sounds\\SOUND-1.wav", 0 /* DSBCAPS_CTRL3D */ );
    m_pSound[3].Create( "Sounds\\SOUND-2.wav", 0 /* DSBCAPS_CTRL3D */ );
	m_pSound[4].Create( "Sounds\\SOUND-3.wav", 0 /* DSBCAPS_CTRL3D */ );

	m_pActiveSound = &m_pSound[0];

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Run()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXHawkPlayback::Run()
{
    OutputDebugString( _T("XBApp: Running the application...\n") );

	// Run the game loop, animating and rendering frames
    while( TRUE )
    {
		switch(m_eTestStep)
		{
			case WAIT_FOR_ENUM:
			{
				m_bXHawkConnected = m_pHawkUnits->FindConnections();
				if(m_bXHawkConnected)
				{
					
					m_eTestStep = WAIT_FOR_RECORD_BUTTON;
				}
				break;
			}			
			case WAIT_FOR_RECORD_BUTTON:
			{
				if(m_Gamepad[0].wPressedButtons & XINPUT_GAMEPAD_BACK )
				{				
					m_eTestStep = RECORD_SOUND;
					if( m_bStartedSound )
					{
						if(SUCCEEDED(m_pActiveSound->Stop()))
						m_bStartedSound = FALSE;
					}
					if(m_pNewSound)
						m_pActiveSound = m_pNewSound;
					// Play one of the 6 sounds
					if(m_pActiveSound && SUCCEEDED(m_pActiveSound->Play(DSBPLAY_LOOPING)))
							m_bStartedSound = TRUE;
					m_pNewSound = NULL;
					Sleep(500);
				}
				break;
			}
			case RECORD_SOUND:
			{
				
				
				if(!m_pHawkUnits->RecordSound(0) )
				{
					if(SUCCEEDED(m_pActiveSound->Stop()))
							m_bStartedSound = FALSE;
					m_eTestStep = WAIT_FOR_PLAY_BUTTON;
				}
				break;
			}
			case WAIT_FOR_PLAY_BUTTON:
			{
				if(m_Gamepad[0].wPressedButtons & XINPUT_GAMEPAD_START )
				{		
					m_eTestStep = PLAY_SOUND;	 
				}
				break;
			}
			case PLAY_SOUND:
			{
				if(!m_pHawkUnits->PlaySound() )
				{					
					m_eTestStep = WAIT_FOR_UNPLUG;
				}
				break;
			}
			case WAIT_FOR_UNPLUG:
			{
				break;
			}
		}
		BOOL bXHawkConnected	 = m_pHawkUnits->FindConnections();
				
		if(m_bXHawkConnected && bXHawkConnected)
		{
			if( m_bStartedSound )
			{
				 if(SUCCEEDED(m_pActiveSound->Stop()))
					m_bStartedSound = FALSE;
			}
			m_bXHawkConnected = FALSE;
			m_eTestStep = WAIT_FOR_ENUM;
		}
		//-----------------------------------------
        // Handle input
		//-----------------------------------------

		XBInput_GetInput( m_Gamepad );
				
		// Frame move the scene
		FrameMove();

		// Render the scene
		Render();

		// Finally, show the frame (swaps the backbuffer to the front)
		m_pd3dDevice->Present( NULL, NULL, NULL, NULL );		
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for
//       changing the .wav to be played.
//-----------------------------------------------------------------------------
HRESULT CXHawkPlayback::FrameMove()
{
	CXBSound* pNewSound = NULL;
	if( m_Gamepad[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
	{
		pNewSound = &m_pSound[0];
	}
	if( m_Gamepad[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B])
	{
		pNewSound = &m_pSound[1];
	}
	if( m_Gamepad[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X])
	{
		pNewSound = &m_pSound[2];
	}
	if( m_Gamepad[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
	{
		pNewSound = &m_pSound[3];
	}
	if( m_Gamepad[0].bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
	{
		pNewSound = &m_pSound[4];
	}
	if( m_Gamepad[0].bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
	{
		pNewSound = &m_pSound[0];
	}
	if(pNewSound)
		m_pNewSound = pNewSound;
   
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXHawkPlayback::Render()
{
    // Begin the scene
    m_pd3dDevice->BeginScene();

    // Set state to render the gamepad image
    m_pd3dDevice->SetTexture( 0, m_pGamepadTexture[m_eTestStep ] );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,    D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,    D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );

    // Render the gamepad image
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 6*sizeof(FLOAT) );
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // End the scene
    m_pd3dDevice->EndScene();

    return S_OK;
}




