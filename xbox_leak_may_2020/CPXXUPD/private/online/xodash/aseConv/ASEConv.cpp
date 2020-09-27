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
#include "ASEConv.h"
#include "ASEScene.h"

#include "utilities.h"
#include "XOConst.h"
#include "XBInput.h"
#include "Globals.h"
#include "CSVFile.h"
#include "TextureCache.h"
#include "MaterialLibrary.h"

#include "DSoundManager.h"
#include "ntiosvc.h"

//#include "ntiosvc.h"
//-----------------------------------------------------------------------------
// Global access to common members
//-----------------------------------------------------------------------------
ASEConv*    g_pAseApp     = NULL;
LPDIRECT3DDEVICE8  g_pd3dDevice = NULL;
CMaterialLibrary g_MaterialLib;

float* g_fAppTime = 0;
TCHAR* g_szAppDir = NULL;

CNtIoctlCdromService g_cdrom;
int g_nDiscType = eDiscNone;
int GetDiscType();



//-----------------------------------------------------------------------------
// Name: ASEConv()
// Desc: Constructor
//-----------------------------------------------------------------------------
ASEConv::ASEConv()
{
    // Initialize member variables
    g_pAseApp          = this;

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
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	m_d3dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN;
	m_d3dpp.EnableAutoDepthStencil = 0;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    m_d3dpp.Flags = D3DPRESENTFLAG_INTERLACED;	
	m_d3dpp.hDeviceWindow = NULL;
	m_d3dpp.Windowed = FALSE;


    // Specify number and type of input devices this app will be using. By
    // default, you can use 0 and NULL, which triggers XInputDevices() to
    // prealloc the default number and types of devices. To use chat or
    // other devices, override these variables in your derived class.
	g_fAppTime = &m_fAppTime;
    _tcscpy( m_szAppDir, _T("d:\\") );
	g_szAppDir = m_szAppDir;

    curFile = 0;

    
    m_ConvertedFiles = NULL;
}

ASEConv::~ASEConv()
{
   
}


//-----------------------------------------------------------------------------
// Name: Create()
// Desc: Create the app
//-----------------------------------------------------------------------------
HRESULT ASEConv::Create()
{
    HRESULT hr = S_OK;

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

	// retreive the status of DVD/CD Rom 
		g_cdrom.Open(1);
        g_nDiscType = GetDiscType();
	
		// initialize DSound 
	if(!DSoundManager::Instance())
	{
		hr = E_OUTOFMEMORY; 
	}
	
	hr = DSoundManager::Instance()->Initialize();
    if ( FAILED( hr ) )
    {
	    ASSERT( !"XOApplication::InitAudio fail to init DSound");
	}

	return hr;
}




//-----------------------------------------------------------------------------
// Name: Destroy()
// Desc: Cleanup objects
//-----------------------------------------------------------------------------
VOID ASEConv::Destroy()
{
    // Perform app-specific cleanup
    Cleanup();

    // Release display objects
    m_pd3dDevice->Release();
    m_pD3D->Release(  );

    CTranslator::Instance()->Cleanup();

     for ( ULONG i =0; i < m_nConvertedFileCount; i++ )
    {
        if ( m_ConvertedFiles[i].m_pAseName )
            delete [] m_ConvertedFiles[i].m_pAseName;

       for ( int j = 0; j < m_ConvertedFiles[i].m_nTextButtonCount; j++ )
       {
           delete [] m_ConvertedFiles[i].m_pTextButtonNames[j];
       }
    }

    delete [] m_ConvertedFiles;
    m_ConvertedFiles = 0;
}




//-----------------------------------------------------------------------------
// Name: Run()
// Desc: 
//-----------------------------------------------------------------------------
INT ASEConv::Run()
{
    
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



HRESULT ASEConv::Update()
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
        float oldTime = m_fTime;
        m_fTime           = fSecsPerTick * ((FLOAT)(qwTime.QuadPart));
        m_fElapsedTime    = oldTime - m_fTime;
        if ( m_fElapsedTime >.1f || m_fElapsedTime < 0.f )
            m_fElapsedTime = .1f;
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

        if ( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] == 1)
        {
            curFile++;
            if ( curFile >= m_nConvertedFileCount )
                curFile = 0;
            m_pScene->SaveBinary( m_ConvertedFiles[curFile].m_pAseName );
            DbgPrint( "Saving\n" );
        }
        else if ( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] == 1 )
        {
            curFile++;
            if ( curFile >= m_nConvertedFileCount )
                curFile = 0;

            m_pScene->Cleanup();
            m_pScene->LoadFromXBG(g_pd3dDevice, m_ConvertedFiles[curFile].m_pAseName, m_ConvertedFiles[curFile].m_pTextButtonNames[0] );
            DbgPrint( "Loading\n" );
        }

		return hr;
}

//-----------------------------------------------------------------------------
// Name: RenderGradientBackground()
// Desc: Draws a gradient filled background
//-----------------------------------------------------------------------------
HRESULT ASEConv::RenderGradientBackground( DWORD dwTopColor, 
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

HRESULT ASEConv::Cleanup()
{
	HRESULT hr = S_OK;


	if ( m_pScene )
		delete m_pScene;


    hr = DSoundManager::Instance()->Cleanup();
	if (FAILED(hr))
	{
		DbgPrint("XOApplication::Cleanup - fail to cleanup DirectSound, hr = 0x%x", hr);
	}
	else
	{
		DbgPrint("XOApplication::Cleanup - DirectSound clean");
	}

	return hr;
}

HRESULT ASEConv::Render()
{
	if ( m_pd3dDevice )
	{
	
		m_pd3dDevice->BeginScene();

		// Clear the backbuffer 
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         D3DCOLOR_XRGB( (int)0, (int)0, (int)0 ), 1.0f, 0L );

		if ( m_Gamepad->bPressedAnalogButtons[0] )
		{
//			m_pButton->ToggleRotation();
		}

		m_pScene->Render( (DWORD)m_fElapsedTime );


		m_pd3dDevice->EndScene();
	}

	return S_OK;
}


HRESULT ASEConv::FrameMove()
{

	m_pScene->FrameMove( m_DefaultGamepad, m_fElapsedTime);
	return S_OK;
}



HRESULT ASEConv::Initialize()
{
	HRESULT hr = S_OK;
    CTranslator::Instance()->Initialize();
    if ( !m_pScene )
	{

        GetFilesToOpen( ASE_PATH_ANSI, ".ase" );

        char textPath[256];
        strcpy( textPath, MEDIA_PATH_ANSI );
        strcat( textPath, "text.csv" );
        CSVFile csvFile;
        csvFile.open( textPath );

		// BugBug: Testing
		//strcpy( pFileName, "d:\\media\\ase\\sphere.ase");

		//-----------------------------------------------------
		// Fully loads and parses an ASE File.  These files are
		// output by 3D Studio MAX's ASCII Scene Exporter.
        ULONG i = 0;
        while( true )
        {
            if ( i >= m_nConvertedFileCount )
                break;

		    HANDLE hFile = CreateFile( m_ConvertedFiles[i].m_pAseName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );

		    if ( hFile == INVALID_HANDLE_VALUE )
            {
                DbgPrint( "Couldn't open the ase file\n" );
                ASSERT( !"Couldn't open an ASE file\n" );
                return -1;
            }

		    //------------------------------------------
		    // Check if exists by getting size
		    long aseFileSize =  ::GetFileSize( hFile, 0 );

		    _ASSERT(aseFileSize > 0);

		    //---------------------------------------
		    // Create Buffer to read entire file into
		    char *aseContents = new char[aseFileSize+1];
		    aseContents[aseFileSize] = 0;
		    _ASSERT(aseContents != NULL);

		    unsigned long nBytesRead = 0;
		    
		    int dataRead = ::ReadFile( hFile, aseContents,sizeof( char )*aseFileSize, &nBytesRead, NULL);
		    _ASSERT(nBytesRead == aseFileSize);



		    ::CloseHandle( hFile );

            m_pScene = new ASEScene;

            char* pExt = strstr( m_ConvertedFiles[i].m_pAseName, ".ase" );
            *pExt = 0;
            char* pFileNoPath = ( m_ConvertedFiles[i].m_pAseName + strlen(  ASE_PATH_ANSI ) );

            char tmpFileName[256];
            strcpy( tmpFileName, pFileNoPath );

            void    LoadStrings( CSVFile* pFile, const char* aseName, const char** pSceneName, long sceneCount   );

            m_pScene->LoadStrings( &csvFile, pFileNoPath, (const char** )m_ConvertedFiles[i].m_pTextButtonNames, m_ConvertedFiles[i].m_nTextButtonCount );

            strcpy( m_ConvertedFiles[i].m_pAseName, XBG_PATH_ANSI );
            strcat( m_ConvertedFiles[i].m_pAseName, tmpFileName );
            strcat( m_ConvertedFiles[i].m_pAseName, ".xbg" );


		    m_pScene->Load( aseContents);

		    delete [] aseContents;
		    aseContents = NULL;

            m_pScene->SaveBinary( m_ConvertedFiles[i].m_pAseName );

            i++;
        }


	}	
    
    float fZero = 0.0f;
	float fOne = 1.0f;


    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, 0 );
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
//	g_pd3dDevice->SetRenderState(D3DRS_FOGSTART, ); // Undocumented!
//	g_pd3dDevice->SetRenderState(D3DRS_FOGEND, ); // Undocumented!
	g_pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(LPDWORD)&fOne);
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
//	g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, ); // Undocumented!
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
	g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE, *(LPDWORD)&fOne);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE_MIN, *(LPDWORD)&fOne);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_A, *(LPDWORD)&fOne);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_B, *(LPDWORD)&fZero);
	g_pd3dDevice->SetRenderState(D3DRS_POINTSCALE_C, *(LPDWORD)&fZero);
	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEMASK, 0xffffffff);
//	g_pd3dDevice->SetRenderState(D3DRS_PATCHEDGESTYLE, ); // Undocumented!
//	g_pd3dDevice->SetRenderState(D3DRS_PATCHSEGMENTS, ); // Undocumented!
//	g_pd3dDevice->SetRenderState(D3DRS_DEBUGMONITORTOKEN, ); // Undocumented!
//	g_pd3dDevice->SetRenderState(D3DRS_POINTSIZE_MAX, ); // Undocumented!
	g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
//	g_pd3dDevice->SetRenderState(D3DRS_TWEENFACTOR, ); // Undocumented!
	return hr;
}


void    ASEConv::GetFilesToOpen( const char* path, const char* pExtension )
{   
    curFile = 0;
    char pInputFileName[255];
    strcpy( pInputFileName, MEDIA_PATH_ANSI );
    strcat( pInputFileName, "sceneToLoad.csv" );

    CSVFile file;
    long sceneToLoadFileOpenResult = file.open( pInputFileName );
    if ( NO_ERR == sceneToLoadFileOpenResult )
    {
        long rowCount = file.getTotalRows();
        m_ConvertedFiles = new XBGFileData[rowCount];
        memset( m_ConvertedFiles, 0, sizeof( XBGFileData ) * rowCount );

        m_nConvertedFileCount = rowCount;
        for ( int i = 0; i < rowCount; i++ )
        {
            char aseFileName[256];
            file.readString( i+1, 1, aseFileName, 255 );

            long len = strlen( path ) + strlen( pExtension ) + strlen( aseFileName ) + 1;

            m_ConvertedFiles[i].m_pAseName = new char[len];
            // make the path
            strcpy( m_ConvertedFiles[i].m_pAseName, path );
            strcat( m_ConvertedFiles[i].m_pAseName, aseFileName );

            // swap extensions
            char* pExt = strstr( m_ConvertedFiles[i].m_pAseName, "." );
            if ( pExt )
                strcpy( pExt, pExtension );
            else
                strcat( m_ConvertedFiles[i].m_pAseName, pExtension );

            // count the number of button and text objects
            for ( int j = 1; j < file.getTotalCols(); j++ )
            {
                char buttonTextName[256];
                buttonTextName[0] = 0;
                file.readString( i+1, j+1, buttonTextName, 255 );

                if ( strlen( buttonTextName ) )
                {
                    m_ConvertedFiles[i].m_nTextButtonCount++;
                }
            }

            // now figure go ahead and allocate...
            if ( m_ConvertedFiles[i].m_nTextButtonCount )
            {
                m_ConvertedFiles[i].m_pTextButtonNames = new char*[m_ConvertedFiles[i].m_nTextButtonCount];
            }

            // do it again
            for ( int j = 0; j < m_ConvertedFiles[i].m_nTextButtonCount; j++ )
            {
                char buttonTextName[256];
                file.readString( i+1, j+2, buttonTextName, 255 );

                if ( strlen( buttonTextName ) )
                {
                    m_ConvertedFiles[i].m_pTextButtonNames[j] = new char[strlen(buttonTextName)+1];
                    strcpy( m_ConvertedFiles[i].m_pTextButtonNames[j], buttonTextName );
                }
            }


            
        }
    }
    else
        ASSERT( sceneToLoadFileOpenResult == 0 );
}



