#define COUNTOF(x) (sizeof(x) / sizeof(x[0]))
#define TheSize 65536*3*5*7

//-----------------------------------------------------------------------------
// File: CubeMap.cpp
//
// Desc: Sample code showing how to do environment cube-mapping.
//
// Hist: 11.01.00 - New for November XDK release
//       12.15.00 - Changes for December XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XBUtil.h>
#include <XBResource.h>
#include <xgraphics.h>

// The following header file is generated from "Resource.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (Resource.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "Resource.h"


BOOL gLightingEnabled = TRUE;

//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "LobbyXP.bmp",    resource_LobbyXP_OFFSET },
    { "LobbyXN.bmp",    resource_LobbyXN_OFFSET },
    { "LobbyYP.bmp",    resource_LobbyYP_OFFSET },
    { "LobbyYN.bmp",    resource_LobbyYN_OFFSET },
    { "LobbyZP.bmp",    resource_LobbyZP_OFFSET },
    { "LobbyZN.bmp",    resource_LobbyZN_OFFSET },
    { "BiHull.bmp",     resource_BiHull_OFFSET },
    { "Wings.bmp",      resource_Wings_OFFSET },
    { NULL, 0 },
};


   

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Rotate\nscene" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display\nhelp" },
    { XBHELP_X_BUTTON,  XBHELP_PLACEMENT_1, L"Pitch++" },
    { XBHELP_Y_BUTTON,  XBHELP_PLACEMENT_1, L"Pitch--" },
    { XBHELP_A_BUTTON,  XBHELP_PLACEMENT_1, L"Flags++" },
    { XBHELP_B_BUTTON,  XBHELP_PLACEMENT_1, L"Flags--" },
    { XBHELP_BLACK_BUTTON,  XBHELP_PLACEMENT_1, L"Tex Src/Copy" },
	{ XBHELP_LEFT_BUTTON, XBHELP_PLACEMENT_1, L"Dest Flags++" },
	{ XBHELP_RIGHT_BUTTON, XBHELP_PLACEMENT_1, L"Dest Flags--" },
	{ XBHELP_DPAD, XBHELP_PLACEMENT_1, L"Dest Pitch+-" },
};

#define NUM_HELP_CALLOUTS COUNTOF(g_HelpCallouts)




// CUBEMAP_RESOLUTION indicates how big to make the cubemap texture.
#define CUBEMAP_RESOLUTION 256

int gPitches[] = {
	D3DTILE_PITCH_0200,
	D3DTILE_PITCH_0400,
	D3DTILE_PITCH_0600,
	D3DTILE_PITCH_0800,
	D3DTILE_PITCH_0A00,
	D3DTILE_PITCH_0C00,
	D3DTILE_PITCH_0E00,
	D3DTILE_PITCH_1000,
	D3DTILE_PITCH_1400,
	D3DTILE_PITCH_1800,
	D3DTILE_PITCH_1C00,
	D3DTILE_PITCH_2800,
	D3DTILE_PITCH_3000,
	D3DTILE_PITCH_3800,
	D3DTILE_PITCH_5000,
	D3DTILE_PITCH_7000,
};
int gPitchCount = 0;
int gPitchCount2 = 0;




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource m_xprResource;      // Packed resources for the app
    CXBFont           m_Font;             // Font class
    CXBHelp           m_Help;             // Help class
    int            m_bUseCopyRects;  //
    BOOL              m_bDrawHelp;        // Whether to draw help

    CXBMesh       m_ShinyTeapot;       // A teapot (made shiny with the envmap)
    CXBMesh       m_SkyBox;            // A skybox of a lobby
    CXBMesh       m_Object;            // An object to fly around the scene
    D3DXMATRIX    m_matObject;          // Matrix to position object

    LPDIRECT3DCUBETEXTURE8 m_pCubeMap;  // The cubemap, and access to its surfaces
    LPDIRECT3DCUBETEXTURE8 m_pCubeMap2;  // The cubemap, and access to its surfaces
    LPDIRECT3DSURFACE8     m_pCubeMapFaces[6];
    LPDIRECT3DSURFACE8     m_pCubeMapFaces2[6];

    D3DXMATRIX    m_matIdentity;        // Transform matrices
    D3DXMATRIX    m_matView;
    D3DXMATRIX    m_matProj;

protected:
    BOOL          m_bRenderTeapot;      // To toggle the teapot (so it doesn't render
	int   m_TileType;
	int   m_TileType2;
                                        // its own reflection)
    HRESULT RenderSceneIntoCubeMap();   // Functions to render the scene
    HRESULT RenderScene();

public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();

    CXBoxSample();
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
// Name: CXBoxSample()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    m_bDrawHelp         = FALSE;
    m_pCubeMap          = NULL;
    m_pCubeMap2          = NULL;
	m_TileType= 0;
	m_TileType2= 0;
    m_bUseCopyRects=0;
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_TWO;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{

    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the file objects
    if( FAILED( m_SkyBox.Create( m_pd3dDevice, "Models\\Lobby.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    if( FAILED( m_ShinyTeapot.Create( m_pd3dDevice, "Models\\Teapot.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    if( FAILED( m_Object.Create( m_pd3dDevice, "Models\\Airplane.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;


DWORD mem;


	D3DTILE tile;
	m_pCubeMap = new IDirect3DCubeTexture8;
    // Create the cubemap, with a format that matches the backbuffer, since
    // we'll be rendering into it
    mem = (DWORD)D3D_AllocContiguousMemory(tile.Size = TheSize,
        D3DTILE_ALIGNMENT);
	m_pCubeMap->Common = 0x0840001;
	m_pCubeMap->Format = 0x881072e;
	m_pCubeMap->Data = mem;
	m_pCubeMap->Size = 0;
	m_pCubeMap->Register(0);
	D3D_CopyContiguousMemoryToVideo((void *)mem);


	D3DTILE lastReservedTile; // tiles 0 and 1 are reserved
	D3DDevice_GetTile( 1, &lastReservedTile );
	tile.ZStartTag = D3DTILE_ZENDTAG( &lastReservedTile );
	tile.Flags = D3DTILE_FLAGS_ZBUFFER |D3DTILE_FLAGS_ZCOMPRESS | D3DTILE_FLAGS_Z32BITS;
	tile.Pitch = gPitches[gPitchCount];
	tile.pMemory = (void *)mem/* m_pCubeMap->Data*/;
	tile.Size = TheSize;
	tile.ZOffset = 0;
    D3DDevice_SetTile(2, &tile);


//*
    m_pCubeMap2 = new IDirect3DCubeTexture8;
    mem = (DWORD)D3D_AllocContiguousMemory(TheSize, D3DTILE_ALIGNMENT);
    m_pCubeMap2->Common = 0x0840001;
    m_pCubeMap2->Format = 0x881072e;
    m_pCubeMap2->Data = mem;
    m_pCubeMap2->Size = 0;
    m_pCubeMap2->Register(0);
	D3D_CopyContiguousMemoryToVideo((void *)mem);
/*/
    if( FAILED( m_pd3dDevice->CreateCubeTexture( CUBEMAP_RESOLUTION, 1, 
                                         D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, 
                                         D3DPOOL_DEFAULT, &m_pCubeMap2 ) ) )

        return E_FAIL;
/**/

	D3DDevice_GetTile( 2, &lastReservedTile );
	tile.ZStartTag = 0;
	tile.Flags = D3DTILE_FLAGS_ZBUFFER;
	tile.Pitch = gPitches[gPitchCount2];
	tile.pMemory = (void *)mem/* m_pCubeMap->Data*/;
	tile.Size = TheSize;
	tile.ZOffset = 0;
    D3DDevice_SetTile(3, &tile);

    // Get access the the cube map's surfaces (so we can render into them)
    for( DWORD i=0; i<6; i++ )
        m_pCubeMap->GetCubeMapSurface( (D3DCUBEMAP_FACES)i, 0, &m_pCubeMapFaces[i] );
    for( DWORD i=0; i<6; i++ )
        m_pCubeMap2->GetCubeMapSurface( (D3DCUBEMAP_FACES)i, 0, &m_pCubeMapFaces2[i] );

    // Set default render states
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x00aaaaaa );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, gLightingEnabled );

    // Set the transform matrices
    D3DXMatrixIdentity( &m_matIdentity );
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 4.0f/3.0f, 0.5f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,      &m_matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    // Setup a material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Set up a light
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.0f, 0.0f, 1.0f );
    light.Ambient.r = 0.3f;
    light.Ambient.g = 0.3f;
    light.Ambient.b = 0.3f;

    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );

    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, gLightingEnabled );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB )
        gLightingEnabled = !gLightingEnabled;

	bool bTileChanged = false;
	bool bTileChanged2 = false;

    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] )
	{
        m_bUseCopyRects = (m_bUseCopyRects + 1) % 3;
	}
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
	{
		m_TileType = (m_TileType+1)&3;
		bTileChanged = true;
	}
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
	{
		m_TileType = (m_TileType-1)&3;
		bTileChanged = true;
	}
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
	{
		gPitchCount = (gPitchCount+1) % COUNTOF(gPitches);
		bTileChanged = true;
	}
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
	{
		gPitchCount = (gPitchCount+COUNTOF(gPitches)-1) % COUNTOF(gPitches);
		bTileChanged = true;
	}

	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] )
	{
		m_TileType2 = (m_TileType2+1)&3;
		bTileChanged2 = true;
	}
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] )
	{
		m_TileType2 = (m_TileType2-1)&3;
		bTileChanged2 = true;
	}
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
	{
		gPitchCount2 = (gPitchCount2+1) % COUNTOF(gPitches);
		bTileChanged2 = true;
	}
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
	{
		gPitchCount2 = (gPitchCount2+COUNTOF(gPitches)-1) % COUNTOF(gPitches);
		bTileChanged2 = true;
	}

//	bTileChanged = true;
	if(bTileChanged) 
	{
		D3DTILE tile;
		D3DTILE lastReservedTile; // tiles 0 and 1 are reserved
		D3DDevice_GetTile( 1, &lastReservedTile );
		D3DDevice_GetTile( 2, &tile );
        D3DDevice_SetTile( 2, NULL) ;
		tile.ZStartTag = D3DTILE_ZENDTAG( &lastReservedTile );

		switch (m_TileType) {
		case 0:
			tile.pMemory = (void*)(0x80000000|m_pCubeMap->Data);
			tile.ZStartTag = D3DTILE_ZENDTAG( &lastReservedTile );
			tile.Pitch = gPitches[gPitchCount];
			tile.Size = TheSize;
			tile.ZOffset = 0;
			tile.Flags = D3DTILE_FLAGS_ZBUFFER;
			break;
		case 1:
			tile.pMemory = (void*)(0x80000000|m_pCubeMap->Data);
			tile.ZStartTag = D3DTILE_ZENDTAG( &lastReservedTile );
			tile.Pitch = gPitches[gPitchCount];
			tile.Size = TheSize;
			tile.ZOffset = 0;
			tile.Flags = D3DTILE_FLAGS_ZBUFFER |D3DTILE_FLAGS_ZCOMPRESS | D3DTILE_FLAGS_Z32BITS;
			break;
		case 2:
			tile.pMemory = (void*)(0x80000000|m_pCubeMap->Data);
			tile.ZStartTag = D3DTILE_ZENDTAG( &lastReservedTile );
			tile.Pitch = gPitches[gPitchCount];
			tile.Size = TheSize;
			tile.ZOffset = 0;
			tile.Flags = 0;
			break;
		case 3:
			tile.pMemory = 0;
			break;
		}
        if(tile.pMemory) D3DDevice_SetTile( 2, &tile);
//        D3DDevice_BlockUntilIdle();
	}

	if(bTileChanged2) 
	{
		D3DTILE tile;
		D3DTILE lastReservedTile; // tiles 0 and 1 are reserved
		D3DDevice_GetTile( 2, &lastReservedTile );
		D3DDevice_GetTile( 3, &tile );
//            D3DDevice_BlockUntilIdle();
        D3DDevice_SetTile( 3, NULL) ;
		tile.ZStartTag = D3DTILE_ZENDTAG( &lastReservedTile );

		switch (m_TileType2) {
		case 0:
			tile.pMemory = (void*)(0x80000000|m_pCubeMap2->Data);
			tile.ZStartTag = D3DTILE_ZENDTAG( &lastReservedTile );
			tile.Pitch = gPitches[gPitchCount2];
			tile.Size = TheSize;
			tile.ZOffset = 0;
			tile.Flags = D3DTILE_FLAGS_ZBUFFER;
			break;
		case 1:
/*			tile.pMemory = (void*)(0x80000000|m_pCubeMap2->Data);
			tile.ZStartTag = D3DTILE_ZENDTAG( &lastReservedTile );
			tile.Pitch = gPitches[gPitchCount];
			tile.Size = TheSize;
			tile.ZOffset = 0;
			tile.Flags = D3DTILE_FLAGS_ZBUFFER |D3DTILE_FLAGS_ZCOMPRESS | D3DTILE_FLAGS_Z32BITS;
			break;
*/
		case 2:
			tile.pMemory = (void*)(0x80000000|m_pCubeMap2->Data);
			tile.ZStartTag = D3DTILE_ZENDTAG( &lastReservedTile );
			tile.Pitch = gPitches[gPitchCount2];
			tile.Size = TheSize;
			tile.ZOffset = 0;
			tile.Flags = 0;
			break;
		case 3:
			tile.pMemory = 0;
			break;
		}
        if(tile.pMemory) D3DDevice_SetTile( 3, &tile);
//        D3DDevice_BlockUntilIdle();
	}	

    // Animate the object
    D3DXMATRIX  mat1;
    D3DXMatrixScaling( &m_matObject, 0.2f, 0.2f, 0.2f );
    D3DXMatrixTranslation( &mat1, 0.0f, 2.0f, 0.0f );
    D3DXMatrixMultiply( &m_matObject, &m_matObject, &mat1 );
    D3DXMatrixRotationX( &mat1, -2.9f*m_fAppTime );
    D3DXMatrixMultiply( &m_matObject, &m_matObject, &mat1 );
    D3DXMatrixRotationY( &mat1, 1.055f*m_fAppTime );
    D3DXMatrixMultiply( &m_matObject, &m_matObject, &mat1 );

    // Rotate the camera with the gamepad
    static D3DXMATRIX matTrackBall( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
    D3DXMATRIX matRotate, matTrans;
    FLOAT fXRotate = m_DefaultGamepad.fX1*m_fElapsedTime*D3DX_PI*0.5f;
    FLOAT fYRotate = m_DefaultGamepad.fY1*m_fElapsedTime*D3DX_PI*0.5f;
    D3DXMatrixRotationYawPitchRoll( &matRotate, fXRotate, fYRotate, 0.0f );
    D3DXMatrixMultiply( &matTrackBall, &matTrackBall, &matRotate );
    D3DXMatrixTranslation( &matTrans, 0.0f, 0.0f, 5.0f );
    D3DXMatrixMultiply( &m_matView, &matTrackBall, &matTrans );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

    // Render the scene into the surfaces of the cubemap. Note: don' render the
    // teapot, since it won't reflect into itself
    m_bRenderTeapot = FALSE;
    RenderSceneIntoCubeMap();
    m_bRenderTeapot = TRUE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderSceneIntoCubeMap()
// Desc: Renders the scene to each of the 6 faces of the cube map
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderSceneIntoCubeMap()
{
    // Save the transforms that will be overwritten
    D3DXMATRIX matViewSave = m_matView;
    D3DXMATRIX matProjSave = m_matProj;

    // Get the current view matrix rotations, to concat it with the view matrix
    // for rendering into the cubemap's surfaces
    D3DXMATRIX matViewDir = m_matView;
    matViewDir._41 = 0.0f; matViewDir._42 = 0.0f; matViewDir._43 = 0.0f;

    // Set the projection matrix for a field of view of 90 degrees
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/2, 1.0f, 0.5f, 100.0f );

    // Get the current backbuffer and zbuffer
    LPDIRECT3DSURFACE8 pBackBuffer, pZBuffer;
    m_pd3dDevice->GetRenderTarget( &pBackBuffer );
    m_pd3dDevice->GetDepthStencilSurface( &pZBuffer );

    // Render to the six faces of the cube map
    for( DWORD i=0; i<6; i++ )
    {
        // Set the i'th cubemap surface as the rendertarget
        m_pd3dDevice->SetRenderTarget( m_pCubeMapFaces[i], pZBuffer );

        // Set the viewport to be the correct size
        D3DVIEWPORT8 vpCubeMap = { 0, 0, CUBEMAP_RESOLUTION, CUBEMAP_RESOLUTION, 0.0f, 1.0f };
        m_pd3dDevice->SetViewport( &vpCubeMap );

        // Set the view transform for this cubemap surface
        m_matView = XBUtil_GetCubeMapViewMatrix( (D3DCUBEMAP_FACES)i );
        D3DXMatrixMultiply( &m_matView, &matViewDir, &m_matView );

        // Render the scene
        m_pd3dDevice->BeginScene();
        RenderScene();
        m_pd3dDevice->EndScene();
    }

    // Change the rendertarget back to the main backbuffer
    m_pd3dDevice->SetRenderTarget( pBackBuffer, pZBuffer );
    pBackBuffer->Release();
    pZBuffer->Release();

    D3DLOCKED_RECT lr[2][6];
    for( DWORD i=0; i<6; i++ )
        m_pCubeMapFaces2[i]->LockRect(&lr[1][i],0,D3DLOCK_TILED);
    for( DWORD i=0; i<6; i++ )
        m_pCubeMapFaces[i]->LockRect(&lr[0][i],0,D3DLOCK_TILED);
    if(m_bUseCopyRects == 1) {
        for( DWORD i=0; i<6; i++ ) {
            D3DDevice_CopyRects(m_pCubeMapFaces[i],0,0,m_pCubeMapFaces2[i],0);
        }
    } else if (m_bUseCopyRects == 2) {
        for( DWORD i=0; i<6; i++ ) {
            memcpy(lr[1][i].pBits, lr[0][i].pBits, 256*256*4);
        }
    }
    for( DWORD i=0; i<6; i++ )
        m_pCubeMapFaces2[i]->UnlockRect();
    for( DWORD i=0; i<6; i++ )
        m_pCubeMapFaces[i]->UnlockRect();

    // Restore the viewport to the backbuffer's dimensions
    D3DVIEWPORT8 vpBackBuffer = { 0, 0, 640, 480, 0.0f, 1.0f };
    m_pd3dDevice->SetViewport( &vpBackBuffer );

    // Restore the modified transforms
    m_matView = matViewSave;
    m_matProj = matProjSave;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderScene()
// Desc: Renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderScene()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         0x000000ff, 1.0f, 0L );





    // Set default render states
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,  0x00aaaaaa );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, gLightingEnabled );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );

    // Render the Skybox
    {
        // Disable zbuffer
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

        // Center view matrix, and set FOV to 90 degrees
        D3DXMATRIX matView, matProj;
        matView = m_matView; matView._41 = matView._42 = matView._43 = 0.0f;
        D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/2, 1.0f, 0.5f, 10000.0f );
        m_pd3dDevice->SetTransform( D3DTS_WORLD,      &m_matIdentity );
        m_pd3dDevice->SetTransform( D3DTS_VIEW,       &matView );
        m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

        // Render the skybox
        m_SkyBox.Render( m_pd3dDevice );

        // Restore the render states
        m_pd3dDevice->SetTransform( D3DTS_VIEW,       &m_matView );
        m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,  TRUE );
    }

    // Render the Object
    {
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matObject );
        m_Object.Render( m_pd3dDevice );
    }

    // Render the object with the environment-mapped body
    if( m_bRenderTeapot )
    {
        // Turn on texture-coord generation for cubemapping
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR );

//test for bug5370 (if lighting affects teapot, try removing this line)
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );

        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
        m_pd3dDevice->SetTexture( 0, m_bUseCopyRects?m_pCubeMap2:m_pCubeMap );

        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

        // Render the object with the environment-mapped body
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matIdentity );
        m_ShinyTeapot.Render( m_pd3dDevice, XBMESH_NOMATERIALS|XBMESH_NOTEXTURES );

        // Restore the render states
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, this call renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Begin the scene
    m_pd3dDevice->BeginScene();

    // Render the scene
    RenderScene();

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"CubeMap" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

		WCHAR blah[60];
		WCHAR *tiletitles[4] = {
			L"zbuffer",
			L"all flags",
			L"no flags",
			L"no tile"};
		swprintf(blah, L"Tile: %s", tiletitles[m_TileType]);
		m_Font.DrawText(  64, 70, 0xffff00ff, blah);
		swprintf(blah, L"pitch: %d", gPitches[gPitchCount]);
		m_Font.DrawText(  64, 90, 0xffff0000, blah);
        WCHAR *CopyTypes[] = {
            L"FromTiled",
            L"CopyRects",
            L"CpuCopy"};
        swprintf(blah, L"Copy:%s", CopyTypes[m_bUseCopyRects]);
        m_Font.DrawText(  64, 110, 0xff0000ff, blah);

		swprintf(blah, L"Dest: %s", tiletitles[m_TileType2==1?2:m_TileType2]);
		m_Font.DrawText(  64, 130, 0xff00ffff, blah);
		swprintf(blah, L"dpitch: %d", gPitches[gPitchCount2]);
		m_Font.DrawText(  64, 150, 0xffffff00, blah);

        swprintf(blah, L"Lights: %s", gLightingEnabled?L"TRUE":L"FALSE");
        m_Font.DrawText(  64, 170, 0xffffffff, blah);

        m_Font.End();

    }

    // End the scene
    m_pd3dDevice->EndScene();

    return S_OK;
}



