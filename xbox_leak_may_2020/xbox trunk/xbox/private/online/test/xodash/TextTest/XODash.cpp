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


#include "utilities.h"
#include "XOConst.h"

#include "Translator.h"
#include "SmartObject.h"


//-----------------------------------------------------------------------------
// Global access to common members
//-----------------------------------------------------------------------------
XOApplication*    g_pXBApp     = NULL;
LPDIRECT3DDEVICE8  g_pd3dDevice = NULL;

float* g_fAppTime = 0;
TCHAR* g_szAppDir = NULL;

void Text_Exit();
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
    m_fElapsedTime    = 0.0f;

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
	g_fAppTime = &m_fTime;
	
    _tcscpy (m_szAppDir, _T("D:\\"));
	g_szAppDir = m_szAppDir;

	m_fLastChangeLanguge = 0.0f;
	m_nCurLanguage = 0;
}




//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create the app
//-----------------------------------------------------------------------------
HRESULT XOApplication::Create()
{
    HRESULT hr;

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

		// Get the frequency of the timer
		LARGE_INTEGER qwTicksPerSec;
		QueryPerformanceFrequency( &qwTicksPerSec );
		FLOAT fSecsPerTick = 1.0f / (FLOAT)qwTicksPerSec.QuadPart;

		// Save the start time
		LARGE_INTEGER qwTime;
		QueryPerformanceCounter( &qwTime );

        // Store the current time values as floating point
        
        m_fElapsedTime    = fSecsPerTick * ((FLOAT)(qwTime.QuadPart)) - m_fTime;
		m_fTime           = fSecsPerTick * ((FLOAT)(qwTime.QuadPart));
        
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


		for (SmartObjIter iter = m_SmartObjects.begin(); iter != m_SmartObjects.end();iter++)
		{
			(*iter)->Advance(m_fElapsedTime);
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
		for (SmartObjIter iter = m_SmartObjects.begin(); iter != m_SmartObjects.end();iter++)
		{
			(*iter)->Render();
		}

		m_pd3dDevice->EndScene();
	}

	return S_OK;
}



HRESULT XOApplication::FrameMove()
{
	HRESULT hr = S_OK;

	
	if ((m_fTime - m_fLastChangeLanguge) > 200.0f)
	{
		m_fLastChangeLanguge = m_fTime;
		CTranslator::Instance()->Cleanup();
		m_nCurLanguage +=1;
		if (m_nCurLanguage > 6)
			m_nCurLanguage = 1;
		
		XSetValue(XC_LANGUAGE, REG_DWORD, (DWORD*)&m_nCurLanguage, 4);
		hr = CTranslator::Instance()-> Initialize();

		while(!m_SmartObjects.empty())
		{

				SmartObject* pNext = m_SmartObjects.back();
				m_SmartObjects.pop_back();
				delete pNext;
		}
		
		Text_Exit();
		AddText();
	}
	
	D3DXMATRIX viewMatrix;

	D3DXVECTOR3 vPosition(1.8f, 0.3f, 2.0f);
	
	D3DXVECTOR3 vLookAt(1.8f,0.3f,0.0f);
	D3DXVECTOR3 vUp (0 ,1 ,0);

	D3DXMatrixLookAtRH(
	  &viewMatrix,
	  &vPosition,
	  &vLookAt,
	  &vUp );
	
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &viewMatrix );

	D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovRH( &matProj, D3DX_PI/2, 1.333f, 1.0, 100.f );
	
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

	D3DXMATRIX curWorld;
//	D3DXMatrixIdentity(&curWorld);
	D3DXMatrixScaling(&curWorld, 0.2f, 0.2f, 0.2f);
	
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &curWorld );

	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
	g_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS, TRUE );

	return S_OK;
}


HRESULT XOApplication::Initialize()
{
	
	HRESULT hr;
	hr = CTranslator::Instance()-> Initialize();
	if(FAILED(hr))
	{
		DbgPrint("XOApplication::Initialize - Fail to init XLate table");
		return hr;
	}
	
	AddText();
	return hr;
}

void XOApplication::AddText()
{
//	for(int i = 0; i<200; i++)
	{
		SmartObject* pSmartObj = new CText();
		if(pSmartObj)
		{
			CText* pText = (CText*) (pSmartObj);
			static TCHAR szTest[] = _T("SystemINFO");
			const TCHAR szFontBody[] = _T("Body");
			
			pText->m_szTextId = new TCHAR [_tcslen(szTest) + 1];
			_tcscpy(pText->m_szTextId,szTest);
			pText->m_szFont = new TCHAR [_tcslen(szFontBody) + 1];
			_tcscpy(pText->m_szFont, szFontBody);

			pText->m_width = 10.0f;
			pText->m_height = 4.0f;

			pText->m_scrollRate = 0.1f; 
			pText->m_scrollDelay = 5.0f;

			pText->Initialize();
			m_SmartObjects.push_back(pText);
		}

	}
// Bad input
	{// invalid text id
		SmartObject* pSmartObj = new CText();
		if(pSmartObj)
		{
			CText* pText = (CText*) (pSmartObj);

			static TCHAR szFontBody[] = _T("Body");
			static TCHAR szTest[] = _T("InvalidTextId");
			pText->m_szTextId = new TCHAR [_tcslen(szTest) + 1];
			_tcscpy(pText->m_szTextId,szTest);
			pText->m_szFont = new TCHAR [_tcslen(szFontBody) + 1];
			_tcscpy(pText->m_szFont, szFontBody);
//			m_SmartObjects.push_back(pText);
		}
	}

	{ // invalid font
		SmartObject* pSmartObj = new CText();
		if(pSmartObj)
		{
			CText* pText = (CText*) (pSmartObj);
			static TCHAR szTest[] = _T("SystemINFO");
			static TCHAR szFontBody[] = _T("InvalidFont");
			
			pText->m_szTextId = new TCHAR [_tcslen(szTest) + 1];
			_tcscpy(pText->m_szTextId, szTest);
			pText->m_szFont = new TCHAR [_tcslen(szFontBody) + 1];
			_tcscpy(pText->m_szFont, szFontBody);
//			m_SmartObjects.push_back(pText);
		}
	}

	{// no font specified
		SmartObject* pSmartObj = new CText();
		if(pSmartObj)
		{
			CText* pText = (CText*) (pSmartObj);
			static TCHAR szTest[] = _T("InValidTextId");
			
			pText->m_szTextId = new TCHAR [_tcslen(szTest) + 1];
			_tcscpy(pText->m_szTextId, szTest);
			
//			m_SmartObjects.push_back(pText);
		}
	}


	{
		// no translation
		SmartObject* pSmartObj = new CText();
		if(pSmartObj)
		{
			CText* pText = (CText*) (pSmartObj);
			static TCHAR szTest[] = _T("SystemINFO");
			static TCHAR szFontBody[] = _T("Body");
			
			pText->m_szText = new TCHAR [_tcslen(szTest) + 1];
			_tcscpy(pText->m_szText,szTest);
			pText->m_szFont = new TCHAR [_tcslen(szFontBody) + 1];
			_tcscpy(pText->m_szFont, szFontBody);

			pText->m_width = -4.0f;
			pText->m_height = 2.0f;

			pText->m_scrollRate = 10.0f; 
			pText->m_scrollDelay = 1.0f;

			pText-> m_bTranslate = false;
			pText->Initialize();
//			m_SmartObjects.push_back(pText);
		}

	}
}


