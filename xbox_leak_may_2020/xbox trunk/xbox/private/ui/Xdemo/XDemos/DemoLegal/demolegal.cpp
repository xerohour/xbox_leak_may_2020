#include <xtl.h>
#include <stdlib.h>
#include <stdio.h>
#include <xgraphics.h>
#include "demolegal.h"

//-----------------------------------------------------------------------------
struct DEMO_LAUNCH_DATA
{
	DWORD	dwID;				// I don't know what I'll use this for, yet...
	DWORD	dwRunmode;
	DWORD	dwTimeout;
	char	szLauncherXBE[64];	// name of XBE to launch to return
	char	szLaunchedXBE[64];	// name of your XBE, useful to get path info from
	char	szDemoInfo[MAX_LAUNCH_DATA_SIZE - (128 * sizeof(char)) - (3 * sizeof(DWORD))];
								// set in XDI file, additional info/parameters/whatever
};

#define RUNMODE_KIOSKMODE		0x01
#define RUNMODE_USERSELECTED	0x02

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D8             g_pD3D = NULL;		 // Used to create the D3DDevice
LPDIRECT3DDEVICE8       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER8 g_pVB = NULL;		 // Buffer to hold vertices
LPDIRECT3DTEXTURE8		g_ptexture;			 // The texture surface to hold the warning


//
// XB texture variables
//
#define resource_NUM_RESOURCES 1UL
#define resource_background_OFFSET 0UL
BYTE*	g_pResourceSysMemData = NULL; // Sysmem data for the packed resource
BYTE*   g_pResourceVidMemData = NULL; // Vidmem data for the packed resource

//
// Timer to exit
//
#define EXIT_TIME 10  // 10 seconds
int g_nStarttime;

DEMO_LAUNCH_DATA		g_ld;
char *					g_pszDirectory;

void DbgPrint(char *str)
{
	OutputDebugString(str);	
}

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D(void)
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Get the current desktop display mode, so we can set up a back
    // buffer of the same format
    D3DDISPLAYMODE d3ddm;
    if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );

    d3dpp.Windowed						  = FALSE;
    d3dpp.SwapEffect					  = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferWidth                 = SCREEN_WIDTH;
	d3dpp.BackBufferHeight                = SCREEN_HEIGHT;
	d3dpp.BackBufferFormat                = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount                 = 1;
	d3dpp.MultiSampleType				  = D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN ;
	d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    // Create the D3DDevice
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT,
									  D3DDEVTYPE_HAL, 
									  NULL,
                                      D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                      &d3dpp, 
									  &g_pd3dDevice ) ) )
    {
		DbgPrint( "Initialize():Failed to CreateDevice!\n" );
        return E_FAIL;
    }

	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    return S_OK;
}

HRESULT InitVB()
{

    // Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4 * sizeof(CUSTOMVERTEX),
                                                  0, 
												  D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, 
												  &g_pVB ) ) )
    {
        return E_FAIL;
    }

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required becuase vertex
    // buffers may be in device memory.
    VOID* pVertices;
    if( FAILED( g_pVB->Lock( 0, sizeof(g_Vertices), (BYTE**)&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
    g_pVB->Unlock();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: LoadPackedResource()
// Desc: Loads all the texture resources from the given XPR.
//-----------------------------------------------------------------------------
HRESULT LoadPackedResource()
{
	char legalTexture[MAX_PATH];

	strcpy(legalTexture, g_pszDirectory);
	strcat(legalTexture, "legaltext.xbx");

    // Open the fileto read the XPR headers
    FILE* file = fopen( legalTexture, "rb" );
    if( NULL == file )
        return E_FAIL;

    // Read in and verify the XPR magic header
    XPR_HEADER xprh;
    fread( &xprh, sizeof(XPR_HEADER), 1, file );
    if( xprh.dwMagic != XPR_MAGIC_VALUE )
    {
        OutputDebugStringA( "ERROR: Invalid Xbox Packed Resource (.xpr) file" );
        fclose( file );
        return E_INVALIDARG;
    }

    // Compute memory requirements
    DWORD dwSysMemDataSize = xprh.dwHeaderSize - sizeof(XPR_HEADER);
    DWORD dwVidMemDataSize = xprh.dwTotalSize - xprh.dwHeaderSize;

    // Allocate memory
    g_pResourceSysMemData = new BYTE[dwSysMemDataSize];
    g_pResourceVidMemData = (BYTE*)D3D_AllocContiguousMemory( dwVidMemDataSize, D3DTEXTURE_ALIGNMENT );

    // Read in the data from the file
    fread( g_pResourceSysMemData, dwSysMemDataSize, 1, file );
    fread( g_pResourceVidMemData, dwVidMemDataSize, 1, file );

    // Done with the file
    fclose( file );
    
    // Loop over resources, calling Register()
    BYTE* pData = g_pResourceSysMemData;

    for( DWORD i = 0; i < resource_NUM_RESOURCES; i++ )
    {
        // Get the resource
        LPDIRECT3DRESOURCE8 pResource = (LPDIRECT3DRESOURCE8)pData;

        // Register the resource
        pResource->Register( g_pResourceVidMemData );
    
        // Advance the pointer
        switch( pResource->GetType() )
        {
            case D3DRTYPE_TEXTURE:       pData += sizeof(D3DTexture);       break;
            case D3DRTYPE_VOLUMETEXTURE: pData += sizeof(D3DVolumeTexture); break;
            case D3DRTYPE_CUBETEXTURE:   pData += sizeof(D3DCubeTexture);   break;
            case D3DRTYPE_VERTEXBUFFER:  pData += sizeof(D3DVertexBuffer);  break;
            case D3DRTYPE_INDEXBUFFER:   pData += sizeof(D3DIndexBuffer);   break;
            case D3DRTYPE_PALETTE:       pData += sizeof(D3DPalette);       break;
            default:                     return E_FAIL;
        }
    }

    return S_OK;
}

void InitTexture(void)
{
	LoadPackedResource();

	//
    // Access the menu background texture from the xbx file
	//
    g_ptexture = (LPDIRECT3DTEXTURE8)&g_pResourceSysMemData[ resource_background_OFFSET ];

	if( FAILED( g_pd3dDevice->SetTexture( 0, g_ptexture )))
    {
        DbgPrint( "InitTexture():Failed to set texture!!\n" );
    }
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
}

//-----------------------------------------------------------------------------
// Name: DrawScreen()
// Desc: Renders the texture to the screen
//-----------------------------------------------------------------------------

bool DrawScreen (void)
{
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

    //
	// Begin the scene
	//
    g_pd3dDevice->BeginScene();
	g_pd3dDevice->SetTexture( 0, g_ptexture );
    g_pd3dDevice->SetStreamSource( 0, g_pVB, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	//
    // End the scene
	//
    g_pd3dDevice->EndScene();

    //
	// Present the backbuffer contents to the display
	//
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	return (TRUE);
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if( g_pVB != NULL )        
        g_pVB->Release();

    if( g_pd3dDevice != NULL ) 
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )       
        g_pD3D->Release();	 
}

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
	static char string[256];
	DWORD dwLaunchInfo;

	if ((XGetLaunchInfo(&dwLaunchInfo, (LAUNCH_DATA *)&g_ld)==ERROR_SUCCESS) && ((dwLaunchInfo&3) == LDT_TITLE))
	{
		// we were probably launched from XDemos, so get the path from it
		strcpy(string, g_ld.szLaunchedXBE);
		char *p=string;
		for ( ; *p ; p++);
		for ( ; *p != '\\' && p>string ; p--);
		strcpy(p, "\\media\\");
		g_pszDirectory = string;
	}
	else
	{
		g_ld.szLauncherXBE[0]=0;
		g_pszDirectory = "d:\\Media\\";
	}

	//
    // Initialize Direct3D
	//
    if( SUCCEEDED( InitD3D() ) )
    {
		//
        // Create the vertex buffer
		//
        if( SUCCEEDED( InitVB() ) )
        {
			//
			// Initalize the texture canvas
			//
			InitTexture();
			InitInput();

			g_nStarttime = GetTickCount();
			
			while (1)
			{
				DrawScreen();

				//
				// Exit if controller input
				//
				if(PollControllers())
					break;

				//
				// Exit if longer than 10 seconds
				//
				if(((GetTickCount() - g_nStarttime) / 1000) > EXIT_TIME)
					break;
				
			}

        }
    }

	//
    // Clean up everything and exit the app
	//
    Cleanup();

	XLaunchNewImage(g_ld.szLauncherXBE, (LAUNCH_DATA *)&g_ld);
}
