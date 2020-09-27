//-----------------------------------------------------------------------------
// File: Donuts.cpp
//
// Desc: DirectInput semantic mapper version of Donuts3D game
//
// Copyright (C) 1995-2000 Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xgraphics.h>
#include <XBApp.h>
#include "XBCamera.h"
#include <XBFont.h>
#include <XBInput.h>
#include <XBMesh.h>
#include <XBResource.h>
#include <XBSound.h>
#include <XBUtil.h>
#include "donuts.h"
#include "gamemenu.h"
#include "Resource.h"




//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "SkyBoxXN.bmp", resource_SkyBoxXN_OFFSET },
    { "SkyBoxXP.bmp", resource_SkyBoxXP_OFFSET },
    { "SkyBoxYN.bmp", resource_SkyBoxYN_OFFSET },
    { "SkyBoxYP.bmp", resource_SkyBoxYP_OFFSET },
    { "SkyBoxZN.bmp", resource_SkyBoxZN_OFFSET },
    { "SkyBoxZP.bmp", resource_SkyBoxZP_OFFSET },
    { "Splash.bmp",   resource_Splash_OFFSET },
    { "Seafloor.bmp", resource_Seafloor_OFFSET },
    { "Donuts1.tga",  resource_Donuts1_OFFSET },
    { "Donuts2.tga",  resource_Donuts2_OFFSET },
    { NULL, 0 },
};

CXBPackedResource  g_xprResource;      // Packed resources for the app

   



extern XBGAMEPAD g_Gamepads[4];


FLOAT g_fMaxTerrainX = -1e10f;
FLOAT g_fMinTerrainX = +1e10f;
FLOAT g_fMaxTerrainY = -1e10f;
FLOAT g_fMinTerrainY = +1e10f;
FLOAT g_fMaxTerrainZ = -1e10f;
FLOAT g_fMinTerrainZ = +1e10f;


LPDIRECT3DTEXTURE8 g_pLevelTexture  = NULL;
LPDIRECT3DTEXTURE8 g_pPausedTexture = NULL;




//-----------------------------------------------------------------------------
// Custom Direct3D vertex types
//-----------------------------------------------------------------------------
struct SCREENVERTEX
{
    D3DXVECTOR4 p;
    DWORD       color;
};

struct SPRITEVERTEX
{
    D3DXVECTOR3 p;
    DWORD       color;
    FLOAT       tu, tv;
};

struct TERRAINVERTEX
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    FLOAT       tu, tv;
};

#define D3DFVF_SCREENVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)
#define D3DFVF_SPRITEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)




//-----------------------------------------------------------------------------
// Application globals
//-----------------------------------------------------------------------------
BOOL                 g_bDisplayReady  = FALSE; // Whether display class is initialized
DWORD                g_dwAppState;              // Current state the app is in
DWORD                g_dwLevel        = 0;      // Current game level
DWORD                g_dwScore        = 0;      // Current game score

// Player view mode
#define NUMVIEWMODES 3
CXBCamera            g_Camera;                       // Camera used for 3D scene
DWORD                g_dwViewMode           = 0;     // Which view mode is being used
FLOAT                g_fViewTransition      = 0.0f;  // Amount used to transittion views
BOOL                 g_bAnimatingViewChange = FALSE; // Whether view is transitioning
BOOL                 g_bFirstPersonView     = TRUE;  // Whether view is first-person

// Bullet mode
FLOAT                g_fBulletRechargeTime  = 0.0f;  // Recharge time for firing bullets
DWORD                g_dwBulletType         = 0L;    // Current bullet type

// Display list and player ship
DisplayObject*       g_pDisplayList = NULL;          // Global display list
CShip*               g_pShip        = NULL;          // Player's display object

// DirectDraw/Direct3D objects
LPDIRECT3DVERTEXBUFFER8 g_pViewportVB       = NULL;
LPDIRECT3DVERTEXBUFFER8 g_pSpriteVB         = NULL;


// Support for the ship model
CXBMesh*             g_pShipFileObject   = NULL;      // Geometry model of player's ship
DWORD                g_dwNumShipTypes    = 10L;
DWORD                g_dwCurrentShipType = 0L;
CHAR*                g_strShipFiles[]    = { "Models\\Heli.xbg",            "Models\\Spaceship 2.xbg", "Models\\Shusui.xbg",
                                             "Models\\Space Station 7.xbg", "Models\\Spaceship 8.xbg", "Models\\Orbiter.xbg",
                                             "Models\\Spaceship 13.xbg",    "Models\\Spaceship 5.xbg", "Models\\Star Sail.xbg", 
                                             "Models\\Concept Plane 3.xbg", };
WCHAR*               g_strShipNames[]    = { L"Helicopter",    L"Green Machine",  L"Purple Prowler",
                                             L"Drone Clone",   L"Canyon Fighter", L"Roundabout",
                                             L"Tie-X7",        L"Gunner",         L"Star Sail", 
                                             L"Concept Plane", };

// Sound objects
#ifdef SOUND_ACTUALLY_WORKS
CXBSound             g_BeginLevelSound;  // Sounds for the app
CXBSound             g_EngineIdleSound;
CXBSound             g_EngineRevSound;
CXBSound             g_ShieldBuzzSound;
CXBSound             g_ShipExplodeSound;
CXBSound             g_FireBulletSound;
CXBSound             g_ShipBounceSound;
CXBSound             g_DonutExplodeSound;
CXBSound             g_PyramidExplodeSound;
CXBSound             g_CubeExplodeSound;
CXBSound             g_SphereExplodeSound;
#endif // SOUND_ACTUALLY_WORKS

// Game objects
LPDIRECT3DTEXTURE8   g_pSplashTexture  = NULL;    // Texture with game object animations
LPDIRECT3DTEXTURE8   g_pGameTexture1   = NULL;    // Texture with game object animations
LPDIRECT3DTEXTURE8   g_pGameTexture2   = NULL;    // Texture with game object animations
CXBMesh              g_SkyBoxObject;              // Skybox object
CXBMesh*             g_pTerrain        = NULL;    // Geometry model of terrain
CXBFont              g_GameFont;                  // Font for displaying score, etc.
CXBFont              g_MenuFont;                  // Font for displaying in-game menus


// Menu objects
CMenuItem*           g_pMainMenu       = NULL;    // Menu class for in-game menus
CMenuItem*           g_pQuitMenu       = NULL;
CMenuItem*           g_pCurrentMenu    = NULL;

// Defines for the in-game menu
#define MENU_MAIN           1
#define MENU_SOUND          2
#define MENU_VIDEO          3
#define MENU_INPUT          4
#define MENU_VIEWDEVICES    5
#define MENU_CONFIGDEVICES  6
#define MENU_WINDOWED       7
#define MENU_640x480        8
#define MENU_800x600        9
#define MENU_1024x768      10
#define MENU_BACK          11
#define MENU_SOUNDON       12
#define MENU_SOUNDOFF      13
#define MENU_QUIT          14


// Game input variables
FLOAT                g_fBank           = 0.0f;
FLOAT                g_fThrust         = 0.0f;
BOOL                 g_bFiringWeapons  = FALSE;
BOOL                 g_bChangeView     = FALSE;
BOOL                 g_bPaused         = FALSE;

// Menu input variables
BOOL                 g_bMenuLeft       = FALSE;
BOOL                 g_bMenuRight      = FALSE;
BOOL                 g_bMenuUp         = FALSE;
BOOL                 g_bMenuDown       = FALSE;
BOOL                 g_bMenuSelect     = FALSE;




//-----------------------------------------------------------------------------
// Inline helper functions
//-----------------------------------------------------------------------------

// Simple function to define "hilliness" for terrain
inline FLOAT HeightField( FLOAT x, FLOAT z )
{
    return (cosf(x/2.0f+0.2f)*cosf(z/1.5f-0.2f)+1.0f) - 2.0f;
}

// Simple function for generating random numbers
inline FLOAT rnd( FLOAT low, FLOAT high )
{
    return low + ( high - low ) * ( (FLOAT)rand() ) / RAND_MAX;
}




//-----------------------------------------------------------------------------
// Name: GameInitialize()
// Desc: This creates all device-dependant display objects.
//-----------------------------------------------------------------------------
HRESULT GameInitialize()
{
    HRESULT hr;

    // Initialize the DirectSound stuff.
    if( FAILED( hr = CreateSoundObjects() ) )
        return hr;

    // Create the display objects
    if( FAILED( hr = CreateDisplayObjects() ) )
        return hr;

    // Add a ship to the displaylist
    g_pShip        = new CShip( D3DXVECTOR3(0.0f,0.0f,0.0f) );
    g_pDisplayList = g_pShip;

    // Construct the game menus
    ConstructMenus();

    // Initial program state is to display the splash screen
    g_dwAppState = APPSTATE_LOADSPLASH;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GameFrameMove()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT GameFrameMove()
{
    switch( g_dwAppState )
    {
        case APPSTATE_LOADSPLASH:
            // load the splash bitmap
			g_pSplashTexture = g_xprResource.GetTexture( "Splash.bmp" );
            
            // Set the app state to displaying splash
            g_dwAppState = APPSTATE_DISPLAYSPLASH;

            break;

        case APPSTATE_DISPLAYSPLASH:
            // Move from splash screen when user presses a button
            if( g_Gamepads[0].wPressedButtons )
            {
                // Advance to the first level
                AdvanceLevel();
                XBUtil_Timer( TIMER_START );

                g_dwAppState = APPSTATE_BEGINLEVELSCREEN;
            }
            break;

        case APPSTATE_BEGINLEVELSCREEN:
            {
                WCHAR strLevel[80];
                swprintf( strLevel, L"Level %ld", g_dwLevel );
                g_pLevelTexture = g_GameFont.CreateTexture( strLevel );
            }
#ifdef SOUND_ACTUALLY_WORKS
            g_BeginLevelSound.Play();
#endif // SOUND_ACTUALLY_WORKS
            XBUtil_Timer( TIMER_RESET );
            g_dwAppState = APPSTATE_DISPLAYLEVELSCREEN;
            break;

        case APPSTATE_DISPLAYLEVELSCREEN:
            // Only show the Level intro screen for 3 seconds
            if( XBUtil_Timer( TIMER_GETAPPTIME ) > 3.0f )
            {
                g_dwAppState = APPSTATE_ACTIVE;

                SAFE_RELEASE( g_pLevelTexture );            

                SAFE_RELEASE( g_pPausedTexture );            
                g_pPausedTexture = g_GameFont.CreateTexture( L"Paused" );
            }
            break;

        case APPSTATE_ACTIVE:
            UpdateDisplayList();
            CheckForHits();

            if( IsDisplayListEmpty() )
            {
                AdvanceLevel();
                g_dwAppState = APPSTATE_BEGINLEVELSCREEN;
            }
            break;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DestroyGameObjects()
// Desc:
//-----------------------------------------------------------------------------
VOID DestroyGameObjects()
{
    DestroyDisplayObjects();
    DestroySoundObjects();
    DestroyMenus();
}




//-----------------------------------------------------------------------------
// Name: AdvanceLevel()
// Desc:
//-----------------------------------------------------------------------------
VOID AdvanceLevel()
{
    // Up the level
    g_dwLevel++;

    srand( timeGetTime() );

    // Clear any stray objects (anything but the ship) out of the display list
    while( g_pShip->pNext )
    {
        DeleteFromList( g_pShip->pNext );
    }

    // Create donuts for the new level
    for( WORD i=0; i<(2*g_dwLevel+3); i++ )
    {
        D3DVECTOR vPosition = 3.0f * D3DXVECTOR3( rnd(), 0.0f, rnd() );
        D3DVECTOR vVelocity = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

        AddToList( new CDonut( vPosition, vVelocity ) );
    }

    // Delay for 2 seconds before displaying ship
    g_pShip->vPos       = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    g_pShip->vVel       = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    g_pShip->bVisible   = FALSE;
    g_pShip->bExploded  = FALSE;
    g_pShip->fShowDelay = 2.0f;

    // Clear out iput states
    g_fBank          = 0.0f;
    g_fThrust        = 0.0f;
    g_bFiringWeapons = FALSE;

    // Stop engine sounds
#ifdef SOUND_ACTUALLY_WORKS
    g_EngineIdleSound.Stop();
    g_EngineRevSound.Stop();
#endif // SOUND_ACTUALLY_WORKS
}




//-----------------------------------------------------------------------------
// Name: DisplayObject()
// Desc:
//-----------------------------------------------------------------------------
DisplayObject::DisplayObject( DWORD type, D3DVECTOR p, D3DVECTOR v )
{
    // Set object attributes
    pNext    = NULL;
    pPrev    = NULL;
    bVisible = TRUE;
    dwType   = type;
    vPos     = p;
    vVel     = v;
}




//-----------------------------------------------------------------------------
// Name: C3DSprite()
// Desc:
//-----------------------------------------------------------------------------
C3DSprite::C3DSprite( DWORD type, D3DVECTOR p, D3DVECTOR v )
          :DisplayObject( type, p, v )
{
    dwColor = 0xffffffff;
}




//-----------------------------------------------------------------------------
// Name: CDonut()
// Desc:
//-----------------------------------------------------------------------------
CDonut::CDonut( D3DVECTOR p, D3DVECTOR v )
       :C3DSprite( OBJ_DONUT, p, v )
{
    // Set object attributes
    dwTextureWidth   = DONUT_WIDTH;
    dwTextureHeight  = DONUT_HEIGHT;
    dwTextureOffsetX = 0;
    dwTextureOffsetY = 0;

    fSize           = dwTextureWidth / 256.0f;
    vVel            += 0.5f * D3DXVECTOR3( rnd(), 0.0f, rnd() );

    delay           = rnd( 3.0f, 12.0f );
    dwFramesPerLine = 8;
    frame           = rnd( 0.0f, 30.0f );
    fMaxFrame       = NUM_DONUT_FRAMES;
}




//-----------------------------------------------------------------------------
// Name: CPyramid()
// Desc:
//-----------------------------------------------------------------------------
CPyramid::CPyramid( D3DVECTOR p, D3DVECTOR v )
         :C3DSprite( OBJ_PYRAMID, p, v )
{
    // Set object attributes
    dwTextureWidth   = PYRAMID_WIDTH;
    dwTextureHeight  = PYRAMID_HEIGHT;
    dwTextureOffsetX = 0;
    dwTextureOffsetY = 0;

    fSize           = dwTextureWidth / 256.0f;
    vVel            += 0.5f * D3DXVECTOR3( rnd(), 0.0f, rnd() );

    delay           = rnd( 12.0f, 40.0f );
    dwFramesPerLine = 8;
    frame           = rnd( 0.0f, 30.0f );
    fMaxFrame       = NUM_PYRAMID_FRAMES;

}




//-----------------------------------------------------------------------------
// Name: CSphere()
// Desc:
//-----------------------------------------------------------------------------
CSphere::CSphere( D3DVECTOR p, D3DVECTOR v )
        :C3DSprite( OBJ_SPHERE, p, v )
{
    // Set object attributes
    dwTextureWidth   = SPHERE_WIDTH;
    dwTextureHeight  = SPHERE_HEIGHT;
    dwTextureOffsetX = 0;
    dwTextureOffsetY = 128;

    fSize           = dwTextureWidth / 256.0f;
    vVel            += 0.5f * D3DXVECTOR3( rnd(), 0.0f, rnd() );

    delay           = rnd( 60.0f, 80.0f );
    dwFramesPerLine = 16;
    frame           = rnd( 0.0f, 30.0f );
    fMaxFrame       = NUM_SPHERE_FRAMES;
}





//-----------------------------------------------------------------------------
// Name: CCube()
// Desc:
//-----------------------------------------------------------------------------
CCube::CCube( D3DVECTOR p, D3DVECTOR v )
      :C3DSprite( OBJ_CUBE, p, v )
{
    // Set object attributes
    dwTextureWidth   = CUBE_WIDTH;
    dwTextureHeight  = CUBE_HEIGHT;
    dwTextureOffsetX = 0;
    dwTextureOffsetY = 176;

    fSize           = dwTextureWidth / 256.0f;
    vVel            += 0.5f * D3DXVECTOR3( rnd(), 0.0f, rnd() );

    delay           = rnd( 32.0f, 80.0f );
    dwFramesPerLine = 16;
    frame           = rnd( 0.0f, 30.0f );
    fMaxFrame       = NUM_CUBE_FRAMES;
}




//-----------------------------------------------------------------------------
// Name: CCloud()
// Desc:
//-----------------------------------------------------------------------------
CCloud::CCloud( D3DVECTOR p, D3DVECTOR v )
       :C3DSprite( OBJ_CLOUD, p, v )
{
    // Set object attributes
    dwTextureWidth   = CLOUD_WIDTH;
    dwTextureHeight  = CLOUD_WIDTH;
    dwTextureOffsetX = 224;
    dwTextureOffsetY = 224;

    fSize           = dwTextureWidth / 256.0f;
    delay           = rnd( 1.0f, 3.0f );
    dwFramesPerLine = 1;
    frame           = 0.0f;
    fMaxFrame       = 1;
}




//-----------------------------------------------------------------------------
// Name: CBullet()
// Desc:
//-----------------------------------------------------------------------------
CBullet::CBullet( D3DVECTOR p, D3DVECTOR v, DWORD dwCType )
        :C3DSprite( OBJ_BULLET, p, v )
{
    // Set object attributes
    dwTextureWidth   = CLOUD_WIDTH;
    dwTextureHeight  = CLOUD_HEIGHT;
    dwTextureOffsetX = 224;
    dwTextureOffsetY = 224;

    if( dwCType == 0 )
        dwColor = 0xff2020ff;
    if( dwCType == 1 )
        dwColor = 0xff208020;
    if( dwCType == 2 )
        dwColor = 0xff208080;
    if( dwCType == 3 )
        dwColor = 0xff802020;

    fSize           = 4 / 256.0f;
    fMaxFrame       = NUM_BULLET_FRAMES;

    delay           = 1000.0f;
    dwFramesPerLine = 1;
    frame           = 0.0f;
}




//-----------------------------------------------------------------------------
// Name: CShip()
// Desc:
//-----------------------------------------------------------------------------
CShip::CShip( D3DVECTOR p )
      :DisplayObject( OBJ_SHIP, p, D3DXVECTOR3(0,0,0) )
{
    fSize           = 10.0f / 256.0f;
    bExploded       = FALSE;
    fShowDelay      = 0.0f;

    fRoll           = 0.0f;
    fAngle          = 0.0f;
}




//-----------------------------------------------------------------------------
// Name: AddToList()
// Desc:
//-----------------------------------------------------------------------------
VOID AddToList( DisplayObject* pObject )
{
    pObject->pNext = g_pDisplayList->pNext;
    pObject->pPrev = g_pDisplayList;

    if( g_pDisplayList->pNext )
        g_pDisplayList->pNext->pPrev = pObject;
    g_pDisplayList->pNext = pObject;
}




//-----------------------------------------------------------------------------
// Name: IsDisplayListEmpty()
// Desc:
//-----------------------------------------------------------------------------
BOOL IsDisplayListEmpty()
{
    DisplayObject* pObject = g_pDisplayList->pNext;

    while( pObject )
    {
        if( pObject->dwType != OBJ_BULLET )
            return FALSE;

        pObject = pObject->pNext;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: LoadTerrainModel()
// Desc: Loads the 3D geometry for the terrain
//-----------------------------------------------------------------------------
HRESULT LoadTerrainModel()
{
    LPDIRECT3DVERTEXBUFFER8 pVB;
    DWORD          dwNumVertices;
    TERRAINVERTEX* pVertices;

    // Delete old object
    SAFE_DELETE( g_pTerrain );

    // Create new object
    g_pTerrain = new CXBMesh();
    if( FAILED( g_pTerrain->Create( g_pd3dDevice, "Models\\SeaFloor.xbg", &g_xprResource ) ) )
        return E_FAIL;

    // Gain access to the model's vertices
    dwNumVertices =  g_pTerrain->GetMesh(0)->m_dwNumVertices;
    pVB           = &g_pTerrain->GetMesh(0)->m_VB;
    pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

    for( DWORD i=0; i<dwNumVertices; i++ )
    {
        pVertices[i].p.x *= 0.1f;
        pVertices[i].p.z *= 0.1f;
        pVertices[i].p.y = HeightField( pVertices[i].p.x, pVertices[i].p.z );

        if( pVertices[i].p.x > g_fMaxTerrainX )   g_fMaxTerrainX = pVertices[i].p.x;
        if( pVertices[i].p.x < g_fMinTerrainX )   g_fMinTerrainX = pVertices[i].p.x;
        if( pVertices[i].p.y > g_fMaxTerrainY )   g_fMaxTerrainY = pVertices[i].p.y;
        if( pVertices[i].p.y < g_fMinTerrainY )   g_fMinTerrainY = pVertices[i].p.y;
        if( pVertices[i].p.z > g_fMaxTerrainZ )   g_fMaxTerrainZ = pVertices[i].p.z;
        if( pVertices[i].p.z < g_fMinTerrainZ )   g_fMinTerrainZ = pVertices[i].p.z;
    }

    // Done with the vertex buffer
    pVB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: LoadShipModel()
// Desc: Loads the 3D geometry for the player's ship
//-----------------------------------------------------------------------------
HRESULT LoadShipModel()
{
    LPDIRECT3DVERTEXBUFFER8 pVB;
    DWORD        dwNumVertices;
    BYTE*        pVertices;
    D3DXVECTOR3  vCenter;
    FLOAT        fRadius;
    DWORD        dwFVF;
    DWORD        dwVertexSize;

    // Make sure the ship is not selected
    g_pd3dDevice->SetStreamSource( 0, NULL, 0 );
    g_pd3dDevice->SetIndices( NULL, 0 );

    // Delete old object
    SAFE_DELETE( g_pShipFileObject );

    // Create new object
    g_pShipFileObject = new CXBMesh();
    if( FAILED( g_pShipFileObject->Create( g_pd3dDevice, g_strShipFiles[g_dwCurrentShipType], &g_xprResource ) ) )
        return E_FAIL;

    // Gain access to the model's vertices
    pVB           = &g_pShipFileObject->GetMesh(0)->m_VB;
    dwNumVertices =  g_pShipFileObject->GetMesh(0)->m_dwNumVertices;
    dwFVF         =  g_pShipFileObject->GetMesh(0)->m_dwFVF;
    dwVertexSize  =  g_pShipFileObject->GetMesh(0)->m_dwVertexSize;
    pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 );

    // Scale the new object to a standard size  
    D3DXComputeBoundingSphere( pVertices, dwNumVertices, dwFVF, 
                               &vCenter, &fRadius );
    for( DWORD i=0; i<dwNumVertices; i++ )
    {
        *((D3DXVECTOR3*)pVertices) /= 12*fRadius;
        pVertices += dwVertexSize;
    }

    // Done with the vertex buffer
    pVB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SwitchModel()
// Desc:
//-----------------------------------------------------------------------------
HRESULT SwitchModel()
{
    // Select next model
    g_dwCurrentShipType++;
    if( g_dwCurrentShipType >= g_dwNumShipTypes )
        g_dwCurrentShipType = 0L;

    // Create new object
    if( FAILED( LoadShipModel() ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderFrame()
// Desc:
//-----------------------------------------------------------------------------
HRESULT DisplaySplash()
{
    struct BACKGROUNDVERTEX { D3DXVECTOR4 p; FLOAT tu, tv; } v[4];
    v[0].p = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 1.0f, 1.0f );  v[0].tu = 0.0f; v[0].tv = 0.0f; 
    v[1].p = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 1.0f, 1.0f );  v[1].tu = 1.0f; v[1].tv = 0.0f; 
    v[2].p = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 1.0f, 1.0f );  v[2].tu = 0.0f; v[2].tv = 1.0f; 
    v[3].p = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 1.0f, 1.0f );  v[3].tu = 1.0f; v[3].tv = 1.0f; 

    // Set states
    g_pd3dDevice->SetTexture( 0, g_pSplashTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADSTRIP, 1, v, sizeof(BACKGROUNDVERTEX) );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GameRender()
// Desc:
//-----------------------------------------------------------------------------
HRESULT GameRender()
{
    // Render the scene based on current state of the app
    switch( g_dwAppState )
    {
        case APPSTATE_LOADSPLASH:
            // Nothing to render while loading the splash screen
            break;

        case APPSTATE_DISPLAYSPLASH:
            DisplaySplash();
            break;

        case APPSTATE_BEGINLEVELSCREEN:
            // Nothing to render while starting sound to advance a level
            break;

        case APPSTATE_DISPLAYLEVELSCREEN:
            DisplayLevelIntroScreen();
            break;

        case APPSTATE_ACTIVE:
            DrawDisplayList();
            break;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DarkenScene()
// Desc:
//-----------------------------------------------------------------------------
VOID DarkenScene( FLOAT fAmount )
{
    if( g_pd3dDevice==NULL )
        return;

    // Setup a dark square to cover the scene
    DWORD dwAlpha = (fAmount<1.0f) ? ((DWORD)(255*fAmount))<<24L : 0xff000000;
    SCREENVERTEX* v;
    g_pViewportVB->Lock( 0, 0, (BYTE**)&v, 0 );
    v[0].color = v[1].color = v[2].color = v[3].color = dwAlpha;
    g_pViewportVB->Unlock();

    // Set renderstates
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,   FALSE );
    g_pd3dDevice->SetTexture( 0, NULL );

    // Draw a big, gray square
    g_pd3dDevice->SetVertexShader( D3DFVF_SCREENVERTEX );
    g_pd3dDevice->SetStreamSource( 0, g_pViewportVB, sizeof(SCREENVERTEX) );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP,0, 2 );

    // Restore states
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
}


struct MYVERTEX { D3DXVECTOR4 p; FLOAT tu, tv; };
    



//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
VOID RenderFieryEffect( LPDIRECT3DTEXTURE8 pTexture, FLOAT fScale )
{
    // Get texture dimensions
    D3DSURFACE_DESC desc;
    pTexture->GetLevelDesc( 0, &desc );
    FLOAT fWidth  = fScale * 0.5f * ((FLOAT)desc.Width);
    FLOAT fHeight = fScale * 0.5f * ((FLOAT)desc.Height);

    MYVERTEX vtx[4];
    vtx[0].p = D3DXVECTOR4( 320-fWidth - 0.5f, 240-fHeight - 0.5f, 1.0f, 1.0f ); 
    vtx[1].p = D3DXVECTOR4( 320+fWidth - 0.5f, 240-fHeight - 0.5f, 1.0f, 1.0f ); 
    vtx[2].p = D3DXVECTOR4( 320+fWidth - 0.5f, 240+fHeight - 0.5f, 1.0f, 1.0f ); 
    vtx[3].p = D3DXVECTOR4( 320-fWidth - 0.5f, 240+fHeight - 0.5f, 1.0f, 1.0f ); 
    vtx[0].tu = 0.0f; vtx[0].tv = 0.0f; 
    vtx[1].tu = 1.0f; vtx[1].tv = 0.0f; 
    vtx[2].tu = 1.0f; vtx[2].tv = 1.0f; 
    vtx[3].tu = 0.0f; vtx[3].tv = 1.0f; 

    // Scale tex coordinates for linear textures
    if( FALSE == XGIsSwizzledFormat( desc.Format ) )
    {
        vtx[0].tu *= ((FLOAT)desc.Width);   vtx[0].tv *= ((FLOAT)desc.Height); 
        vtx[1].tu *= ((FLOAT)desc.Width);   vtx[1].tv *= ((FLOAT)desc.Height); 
        vtx[2].tu *= ((FLOAT)desc.Width);   vtx[2].tv *= ((FLOAT)desc.Height); 
        vtx[3].tu *= ((FLOAT)desc.Width);   vtx[3].tv *= ((FLOAT)desc.Height); 
    }
    
    g_pd3dDevice->SetTexture( 0, pTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );

    // Render the fiery portion of the text
    for( DWORD i=0; i<20; i++ )
    {
        FLOAT a1 = rnd(0.0f, 2*D3DX_PI);
        FLOAT r1 = rnd(0.0f, 1.0f);


        FLOAT x = 320.0f + 10.0f * r1 * sinf(a1);
        FLOAT y = 240.0f + 10.0f * r1 * cosf(a1);

        if( cosf(a1) < 0.0f )
            y -= 2 * 10.0f * r1 * cosf(a1) * cosf(a1);

        vtx[0].p = D3DXVECTOR4( x-fWidth - 0.5f, y-fHeight - 0.5f, 1.0f, 1.0f ); 
        vtx[1].p = D3DXVECTOR4( x+fWidth - 0.5f, y-fHeight - 0.5f, 1.0f, 1.0f ); 
        vtx[2].p = D3DXVECTOR4( x+fWidth - 0.5f, y+fHeight - 0.5f, 1.0f, 1.0f ); 
        vtx[3].p = D3DXVECTOR4( x-fWidth - 0.5f, y+fHeight - 0.5f, 1.0f, 1.0f ); 

        DWORD r = (CHAR)( 256.0f * ( 1.0f-r1*r1*r1 ) );
        DWORD g = (CHAR)( 256.0f * ( 1.0f-r1*r1 ) );
        DWORD b = (CHAR)( 256.0f * ( 1.0f-r1 ) );
        DWORD dwColor = 0xff000000 + (r<<16) + (g<<8) + b;
        g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwColor );

        g_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, vtx, sizeof(vtx[0]) );
    }

    // Render the plain, black portion of the text
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    FLOAT x = -0.5f;
    FLOAT y =  1.8f;

    vtx[0].p = D3DXVECTOR4( 320-fWidth - 0.5f, 240-fHeight - 0.5f, 1.0f, 1.0f ); 
    vtx[1].p = D3DXVECTOR4( 320+fWidth - 0.5f, 240-fHeight - 0.5f, 1.0f, 1.0f ); 
    vtx[2].p = D3DXVECTOR4( 320+fWidth - 0.5f, 240+fHeight - 0.5f, 1.0f, 1.0f ); 
    vtx[3].p = D3DXVECTOR4( 320-fWidth - 0.5f, 240+fHeight - 0.5f, 1.0f, 1.0f ); 

    g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xff000000 );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 1, vtx, sizeof(vtx[0]) );
}




//-----------------------------------------------------------------------------
// Name: DisplayLevelIntroScreen()
// Desc:
//-----------------------------------------------------------------------------
VOID DisplayLevelIntroScreen()
{
    // Erase the screen
    g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0L, 1.0f, 0L );

    RenderFieryEffect( g_pLevelTexture, 2.0f );

    DarkenScene( 1.0f - sinf(D3DX_PI*XBUtil_Timer( TIMER_GETAPPTIME )/3.0f) );
}




//-----------------------------------------------------------------------------
// Name: UpdateDisplayList()
// Desc:
//-----------------------------------------------------------------------------
VOID UpdateDisplayList()
{
    DisplayObject* pObject;

        // Get the time lapsed since the last frame
        static FLOAT fLastTime = 0.0f;
    FLOAT fTimeLapsed = XBUtil_Timer( TIMER_GETAPPTIME ) - fLastTime;
        if( fTimeLapsed <= 0.0f )
                fTimeLapsed = 0.01f;
        fLastTime = XBUtil_Timer( TIMER_GETAPPTIME );

    // Read input from the joystick/keyboard/etc
    GetInput();

    // Check for game menu condition
    if( g_pCurrentMenu )
    {
        UpdateMenus();
        return;
    }

    if( g_bPaused )
        return;

    if( g_pShip->fShowDelay > 0.0f )
    {
        g_pShip->fShowDelay -= fTimeLapsed;

        if( g_pShip->fShowDelay <= 0.0f )
        {
            g_pShip->vVel       = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
            g_pShip->fShowDelay = 0.0f;
            g_pShip->bVisible   = TRUE;
            g_pShip->bExploded  = FALSE;
        }
    }

    // Update the ship
    if( g_pShip->bVisible )
    {
        g_pShip->vPos += g_pShip->vVel * fTimeLapsed;
    }

    // Apply banking motion
    g_pShip->fRoll += g_fBank * 1.0f * fTimeLapsed;
    if( g_pShip->fRoll > 0.5f )
        g_pShip->fRoll = 0.5f;
    if( g_pShip->fRoll < -0.5f )
        g_pShip->fRoll = -0.5f;

    g_pShip->fAngle += 5 * g_pShip->fRoll * fTimeLapsed;

    if( g_fBank < 0.2f && g_fBank > -0.2f )
    {
        g_pShip->fRoll *= 0.95f;
    }

    // Slow the ship down
    g_pShip->vVel.x *= 0.97f;
    g_pShip->vVel.z *= 0.97f;

    // Apply thrust
    g_pShip->vVel.x +=  sinf( g_pShip->fAngle ) * g_fThrust * 5.0f * fTimeLapsed;
    g_pShip->vVel.z += -cosf( g_pShip->fAngle ) * g_fThrust * 5.0f * fTimeLapsed;

    // Play thrusting sounds
    {
        static bPlayingEngineRevSound = FALSE;

        if( g_fThrust > 0.5f )
        {
            if( FALSE == bPlayingEngineRevSound )
            {
                bPlayingEngineRevSound = TRUE;
            }
        }
        else
        {
            if( TRUE == bPlayingEngineRevSound )
            {
#ifdef SOUND_ACTUALLY_WORKS
                g_EngineRevSound.Stop();
#endif // SOUND_ACTUALLY_WORKS
                bPlayingEngineRevSound = FALSE;
            }
        }
    }

    g_fBulletRechargeTime -= fTimeLapsed;

     // Fire a bullet
    if( g_bFiringWeapons && g_fBulletRechargeTime <= 0.0f )
    {
        // Ship must be visible and have no shields on to fire
        if( g_pShip->bVisible )
        {
            // Bullets cost one score point
            if( g_dwScore )
                g_dwScore--;

            // Play the "fire" effects
#ifdef SOUND_ACTUALLY_WORKS
            g_FireBulletSound.Play();
#endif // SOUND_ACTUALLY_WORKS

            // Add a bullet to the display list
            if( g_dwBulletType == 0 )
            {
                D3DXVECTOR3 vDir = D3DXVECTOR3( sinf( g_pShip->fAngle ), 0.0f, -cosf( g_pShip->fAngle ) );

                AddToList( new CBullet( g_pShip->vPos, g_pShip->vVel + 5*vDir, 0 ) );
                g_fBulletRechargeTime = 0.05f;
            }
            else if( g_dwBulletType == 1 )
            {
                D3DXVECTOR3 vOffset = 0.02f * D3DXVECTOR3( cosf(g_pShip->fAngle), 0.0f, sinf(g_pShip->fAngle) );
                D3DXVECTOR3 vDir    = 1.00f * D3DXVECTOR3( sinf(g_pShip->fAngle), 0.0f, -cosf(g_pShip->fAngle) );

                AddToList( new CBullet( g_pShip->vPos + vOffset, g_pShip->vVel + 5*vDir, 1 ) );
                AddToList( new CBullet( g_pShip->vPos - vOffset, g_pShip->vVel + 5*vDir, 1 ) );
                g_fBulletRechargeTime = 0.10f;
            }
            else if( g_dwBulletType == 2 )
            {
                FLOAT fBulletAngle = g_pShip->fAngle + 0.2f*rnd();
                D3DXVECTOR3 vDir = D3DXVECTOR3( sinf(fBulletAngle), 0.0f, -cosf(fBulletAngle) );

                AddToList( new CBullet( g_pShip->vPos, g_pShip->vVel + 5*vDir, 2 ) );
                g_fBulletRechargeTime = 0.01f;
            }
            else
            {
                for( DWORD i=0; i<50; i++ )
                {
                    FLOAT fBulletAngle = g_pShip->fAngle + D3DX_PI*rnd();
                    D3DXVECTOR3 vDir = D3DXVECTOR3( sinf(fBulletAngle), 0.0f, -cosf(fBulletAngle) );

                    AddToList( new CBullet( g_pShip->vPos, 5*vDir, 3 ) );
                }

                g_fBulletRechargeTime = 1.0f;
            }
        }
    }

    // Keep ship in bounds
    if( g_pShip->vPos.x < g_fMinTerrainX || g_pShip->vPos.x > g_fMaxTerrainX ||
        g_pShip->vPos.z < g_fMinTerrainZ || g_pShip->vPos.z > g_fMaxTerrainZ )
    {
         D3DXVec3Normalize( &g_pShip->vVel, &g_pShip->vPos );
         g_pShip->vVel.x *= -1.0f;
         g_pShip->vVel.y *= -1.0f;
         g_pShip->vVel.z *= -1.0f;
    }

    // Finally, move all objects on the screen
    for( pObject = g_pDisplayList; pObject; pObject = pObject->pNext )
    {
        // The ship is moved by the code above
        if( pObject->dwType == OBJ_SHIP )
            continue;

        C3DSprite* pSprite = (C3DSprite*)pObject;

        // Update the position and animation frame
        pSprite->vPos  += pSprite->vVel * fTimeLapsed;
        pSprite->frame += pSprite->delay * fTimeLapsed;

        // If this is an "expired" cloud, removed it from list
        if( pObject->dwType == OBJ_CLOUD )
        {
            if( pSprite->frame >= pSprite->fMaxFrame )
            {
                DisplayObject* pVictim = pObject;
                pObject = pObject->pPrev;
                DeleteFromList( pVictim );
            }
        }
        else if( pObject->dwType == OBJ_BULLET )
        {
            // Remove bullets when they leave the scene
            if( pObject->vPos.x < g_fMinTerrainX || pObject->vPos.x > g_fMaxTerrainX ||
                pObject->vPos.z < g_fMinTerrainZ || pObject->vPos.z > g_fMaxTerrainZ )
            {
                DisplayObject* pVictim = pObject;
                pObject = pObject->pPrev;
                DeleteFromList( pVictim );
            }
        }
        else if( pObject->dwType != OBJ_CLOUD )
        {
            // Keep object in bounds in X
            if( pObject->vPos.x < g_fMinTerrainX || pObject->vPos.x > g_fMaxTerrainX )
            {
                if( pObject->vPos.x < g_fMinTerrainX ) pObject->vPos.x = g_fMinTerrainX;
                if( pObject->vPos.x > g_fMaxTerrainZ ) pObject->vPos.x = g_fMaxTerrainX;
                pObject->vVel.x = -pObject->vVel.x;
            }

            // Keep object in bounds in Z
            if( pObject->vPos.z < g_fMinTerrainZ || pObject->vPos.z > g_fMaxTerrainZ )
            {
                if( pObject->vPos.z < g_fMinTerrainZ ) pObject->vPos.z = g_fMinTerrainZ;
                if( pObject->vPos.z > g_fMaxTerrainZ ) pObject->vPos.z = g_fMaxTerrainZ;
                pObject->vVel.z = -pObject->vVel.z;
            }

            // Keep animation frame in bounds
            if( pSprite->frame < 0.0f )
                pSprite->frame += pSprite->fMaxFrame;
            if( pSprite->frame >= pSprite->fMaxFrame )
                pSprite->frame -= pSprite->fMaxFrame;
        }
    }

    D3DXVECTOR3 vEyePt[NUMVIEWMODES];
    D3DXVECTOR3 vLookatPt[NUMVIEWMODES];
    D3DXVECTOR3 vUpVec[NUMVIEWMODES];

    // Update the view
    if( g_bChangeView )
    {
        g_bAnimatingViewChange = TRUE;
        g_bChangeView = FALSE;
    }

    if( g_bAnimatingViewChange )
    {
        g_fViewTransition += fTimeLapsed;

        if( g_fViewTransition >= 1.0f )
        {
            g_dwViewMode++;
            if( g_dwViewMode >= NUMVIEWMODES )
                g_dwViewMode = 0;

            g_fViewTransition      = 0.0f;
            g_bAnimatingViewChange = FALSE;
        }
    }

    FLOAT fX =  g_pShip->vPos.x;
    FLOAT fZ = -g_pShip->vPos.z;
    FLOAT fY = 0.1f + HeightField( fX, fZ );

    // View mode 0 (third person)
    vEyePt[0]      = D3DXVECTOR3( fX-sinf(g_pShip->fAngle)/2, fY+0.2f, fZ-cosf(g_pShip->fAngle)/2 );
    vLookatPt[0]   = D3DXVECTOR3( fX+sinf(g_pShip->fAngle)/2, fY, fZ+cosf(g_pShip->fAngle)/2 );
    vUpVec[0]      = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

    // View mode 1 (first person)
    FLOAT fX2 = fX+sinf(g_pShip->fAngle);
    FLOAT fZ2 = fZ+cosf(g_pShip->fAngle);
    FLOAT fY2 = 0.1f + HeightField( fX2, fZ2 );
    vEyePt[1]    = D3DXVECTOR3( fX, fY+0.1f, fZ );
    vLookatPt[1] = D3DXVECTOR3( fX2, fY2+0.1f, fZ2 );
    vUpVec[1]    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

    // View mode 2 (top down view)
    vEyePt[2]    = D3DXVECTOR3( fX+1.5f, fY+1.5f, fZ+1.5f );
    vLookatPt[2] = D3DXVECTOR3( fX, fY, fZ );
    vUpVec[2]    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

    DWORD start = g_dwViewMode;
    DWORD end   = ( start < (NUMVIEWMODES-1) ) ? g_dwViewMode+1: 0;

    if( start == 1 && g_fViewTransition<0.2f)
        g_bFirstPersonView = TRUE;
    else
        g_bFirstPersonView = FALSE;

    D3DXVECTOR3 vEyePt0    = (1.0f-g_fViewTransition)*vEyePt[start]    + g_fViewTransition*vEyePt[end];
    D3DXVECTOR3 vLookatPt0 = (1.0f-g_fViewTransition)*vLookatPt[start] + g_fViewTransition*vLookatPt[end];
    D3DXVECTOR3 vUpVec0    = (1.0f-g_fViewTransition)*vUpVec[start]    + g_fViewTransition*vUpVec[end];

    // Shake screen if ship exploded
    if( g_pShip->bExploded == TRUE )
        vEyePt0 += D3DXVECTOR3( rnd(), rnd(), rnd() ) * g_pShip->fShowDelay / 50.0f;

    g_Camera.SetViewParams( vEyePt0, vLookatPt0, vUpVec0 );
}




//-----------------------------------------------------------------------------
// Name: CheckForHits()
// Desc:
//-----------------------------------------------------------------------------
VOID CheckForHits()
{
    DisplayObject* pObject;
    DisplayObject* pBullet;

    for( pBullet = g_pDisplayList; pBullet; pBullet = pBullet->pNext )
    {
        BOOL bBulletHit = FALSE;

        // Only bullet objects and the ship (if shieleds are on) can hit
        // other objects. Skip all others.
        if( (pBullet->dwType != OBJ_BULLET) && (pBullet->dwType != OBJ_SHIP) )
            continue;

        for( pObject = g_pDisplayList->pNext; pObject; pObject = pObject->pNext )
        {
            // Only trying to hit explodable targets
            if( ( pObject->dwType != OBJ_DONUT ) &&
                ( pObject->dwType != OBJ_PYRAMID ) &&
                ( pObject->dwType != OBJ_SPHERE ) &&
                ( pObject->dwType != OBJ_CUBE ) )
                continue;

            // Check if bullet is in radius of object
            FLOAT fDistance = D3DXVec3Length( &(pBullet->vPos - pObject->vPos) );

            if( fDistance < (pObject->fSize+pBullet->fSize) )
            {
                // The object was hit
                switch( pObject->dwType )
                {
                    case OBJ_DONUT:
#ifdef SOUND_ACTUALLY_WORKS
                        g_DonutExplodeSound.Play();
#endif // SOUND_ACTUALLY_WORKS
                        AddToList( new CPyramid( pObject->vPos, pObject->vVel ) );
                        AddToList( new CPyramid( pObject->vPos, pObject->vVel ) );
                        AddToList( new CPyramid( pObject->vPos, pObject->vVel ) );
                        AddToList( new CPyramid( pObject->vPos, pObject->vVel ) );
                        g_dwScore += 10;
                        break;

                    case OBJ_PYRAMID:
#ifdef SOUND_ACTUALLY_WORKS
                        g_PyramidExplodeSound.Play();
#endif // SOUND_ACTUALLY_WORKS
                        AddToList( new CCube( pObject->vPos, pObject->vVel ) );
                        AddToList( new CCube( pObject->vPos, pObject->vVel ) );
                        AddToList( new CCube( pObject->vPos, pObject->vVel ) );
                        AddToList( new CCube( pObject->vPos, pObject->vVel ) );
                        g_dwScore += 20;
                        break;

                    case OBJ_CUBE:
#ifdef SOUND_ACTUALLY_WORKS
                        g_CubeExplodeSound.Play();
#endif // SOUND_ACTUALLY_WORKS
                        AddToList( new CSphere( pObject->vPos, pObject->vVel ) );
                        g_dwScore += 40;
                        break;

                    case OBJ_SPHERE:
#ifdef SOUND_ACTUALLY_WORKS
                        g_SphereExplodeSound.Play();
#endif // SOUND_ACTUALLY_WORKS
                        g_dwScore += 20;
                        break;
                }

                // Add explosion effects to scene
                for( DWORD c=0; c<4; c++ )
                    AddToList( new CCloud( pObject->vPos, 0.05f*D3DXVECTOR3(rnd(),0.0f,rnd()) ) );

                // Remove the victim from the scene
                DisplayObject* pVictim = pObject;
                pObject = pObject->pPrev;
                DeleteFromList( pVictim );

                bBulletHit = TRUE;
            }

            if( bBulletHit )
            {
                if( pBullet->dwType == OBJ_SHIP )
                {
                    bBulletHit = FALSE;

                    if( g_pShip->bVisible )
                    {
                        // Ship has exploded
#ifdef SOUND_ACTUALLY_WORKS
                        g_ShipExplodeSound.Play();
#endif // SOUND_ACTUALLY_WORKS

                        if( g_dwScore < 150 )
                            g_dwScore = 0;
                        else
                            g_dwScore -= 150;

                        // Add explosion debris to scene
                        for( DWORD sphere=0; sphere<4; sphere++ )
                            AddToList( new CSphere( g_pShip->vPos, pObject->vVel ) );

                        for( DWORD bullet=0; bullet<20; bullet++ )
                        {
                            FLOAT     angle     = D3DX_PI * rnd();
                            D3DVECTOR vDir      = D3DXVECTOR3(cosf(angle),0.0f,sinf(angle));
                            AddToList( new CBullet( g_pShip->vPos, 500.0f*vDir, 0 ) );
                        }

                        for( DWORD cloud=0; cloud<100; cloud++ )
                        {
                            FLOAT     magnitude = 1.0f + 0.1f*rnd();
                            FLOAT     angle     = D3DX_PI * rnd();
                            D3DVECTOR vDir      = D3DXVECTOR3(cosf(angle),0.0f,sinf(angle));

                            AddToList( new CCloud( g_pShip->vPos, magnitude*vDir ) );
                        }

                        // Clear out ship params
                        g_pShip->vVel.x   = 0.0f;
                        g_pShip->vVel.z   = 0.0f;
                        g_fThrust         = 0.0f;
                        g_fBank           = 0.0f;
                        g_bFiringWeapons  = FALSE;

                        // Delay for 2 seconds before displaying ship
                        g_pShip->fShowDelay = 2.0f;
                        g_pShip->bVisible   = FALSE;
                        g_pShip->bExploded  = TRUE;
                    }
                }

                break;
            }
        }

        if( bBulletHit )
        {
            DisplayObject* pLastBullet = pBullet;
            pBullet = pBullet->pPrev;
            DeleteFromList( pLastBullet );
        }
    }
}




//-----------------------------------------------------------------------------
// Name: DrawDisplayList()
// Desc:
//-----------------------------------------------------------------------------
VOID DrawDisplayList()
{
    WCHAR strBuffer[80];

    // Set the world matrix
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set the app view matrix for normal viewing
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &g_Camera.GetViewMatrix() );

    // Set up the default state
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

    // Render the Skybox
    {
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
        g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
        g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
        
        // Center view matrix for skybox
        D3DXMATRIX matView, matViewSave;
        g_pd3dDevice->GetTransform( D3DTS_VIEW,      &matViewSave );
        matView = matViewSave;
        matView._41 = 0.0f; matView._42 = -0.0f; matView._43 = 0.0f;
        g_pd3dDevice->SetTransform( D3DTS_VIEW,      &matView );

        // Render the skybox
        g_SkyBoxObject.Render( g_pd3dDevice );

        // Restore the render states
        g_pd3dDevice->SetTransform( D3DTS_VIEW,      &matViewSave );
    }

    // Set default state
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x33333333 );

    // Begin the scene
    {
        // Draw the terrain
        g_pTerrain->Render( g_pd3dDevice );

        // Render the ship
        if( g_pShip->bVisible && g_bFirstPersonView == FALSE )
        {
            // Point of ship, on terrain
            D3DXVECTOR3 vShipPt;
            vShipPt.x =  g_pShip->vPos.x;
            vShipPt.z = -g_pShip->vPos.z;
            vShipPt.y = 0.1f + HeightField( vShipPt.x, vShipPt.z );

            // Point ahead of ship, on terrain
            D3DXVECTOR3 vForwardPt;
            vForwardPt.x = vShipPt.x+sinf(g_pShip->fAngle);
            vForwardPt.z = vShipPt.z+cosf(g_pShip->fAngle);
            vForwardPt.y = 0.1f + HeightField( vForwardPt.x, vForwardPt.z );

            // Point to side of ship, on terrain
            D3DXVECTOR3 vSidePt;
            vSidePt.x = vShipPt.x+sinf(g_pShip->fAngle + D3DX_PI/2.0f);
            vSidePt.z = vShipPt.z+cosf(g_pShip->fAngle + D3DX_PI/2.0f);
            vSidePt.y = 0.1f + HeightField( vSidePt.x, vSidePt.z );

            // Compute vectors of the ship's orientation
            D3DXVECTOR3 vForwardDir = vForwardPt - vShipPt;
            D3DXVECTOR3 vSideDir    = vSidePt - vShipPt;
            D3DXVECTOR3 vNormalDir;
            D3DXVec3Cross( &vNormalDir, &vForwardDir, &vSideDir );

            // Construct matrix to orient ship
            D3DXMATRIX matWorld, matLookAt, matRotateZ;
            D3DXMatrixRotationZ( &matRotateZ, g_pShip->fRoll );
            D3DXMatrixLookAtLH( &matLookAt, &vShipPt, &(vShipPt-vForwardDir), &vNormalDir );
            D3DXMatrixInverse( &matLookAt, NULL, &matLookAt );
            D3DXMatrixIdentity( &matWorld );
            D3DXMatrixMultiply( &matWorld, &matWorld, &matRotateZ );
            D3DXMatrixMultiply( &matWorld, &matWorld, &matLookAt );

            // Set renderstates for rendering the ship
            g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
            g_pd3dDevice->SetRenderState( D3DRS_LIGHTING,           TRUE );
            g_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS,   TRUE );
            g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
            g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,    FALSE );

            // Render the ship - opaque parts
            g_pShipFileObject->Render( g_pd3dDevice, XBMESH_OPAQUEONLY );

            // Render the ship - transparent parts
            g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
            g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
            g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,     D3DBLEND_ONE );
            g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,    D3DBLEND_ONE );
            g_pShipFileObject->Render( g_pd3dDevice, XBMESH_ALPHAONLY );
            g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
        }

        // Remaining objects don't need lighting
        g_pd3dDevice->SetRenderState( D3DRS_LIGHTING,           FALSE );

        // Enable alpha blending and testing
        g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

        // Display all visible objects in the display list
        for( DisplayObject* pObject = g_pDisplayList; pObject; pObject = pObject->pNext )
        {
            if( !pObject->bVisible )
                continue;
            if( pObject->dwType == OBJ_SHIP )
                continue;
            if( pObject->dwType == OBJ_BULLET )
                continue;

            // This is really a 3D sprite
            C3DSprite* pSprite = (C3DSprite*)pObject;

            FLOAT fX =  pObject->vPos.x;
            FLOAT fZ = -pObject->vPos.z;
            FLOAT fY =  HeightField( fX, fZ );

            FLOAT x1 = -pObject->fSize;
            FLOAT x2 =  pObject->fSize;
            FLOAT y1 = -pObject->fSize;
            FLOAT y2 =  pObject->fSize;

            FLOAT u1 = (FLOAT)(pSprite->dwTextureOffsetX + pSprite->dwTextureWidth *(((int)pSprite->frame)%pSprite->dwFramesPerLine));
            FLOAT v1 = (FLOAT)(pSprite->dwTextureOffsetY + pSprite->dwTextureHeight*(((int)pSprite->frame)/pSprite->dwFramesPerLine));

            FLOAT tu1 = u1 / (256.0f-1.0f);
            FLOAT tv1 = v1 / (256.0f-1.0f);
            FLOAT tu2 = (u1 + pSprite->dwTextureWidth -1) / (256.0f-1.0f);
            FLOAT tv2 = (v1 + pSprite->dwTextureHeight-1) / (256.0f-1.0f);

            // Set the game texture
            switch( pObject->dwType )
            {
                case OBJ_DONUT:
                case OBJ_CUBE:
                case OBJ_SPHERE:
                    g_pd3dDevice->SetTexture( 0, g_pGameTexture1 );
                    break;
                case OBJ_PYRAMID:
                case OBJ_CLOUD:
                    g_pd3dDevice->SetTexture( 0, g_pGameTexture2 );
                    break;
            }

            // Translate the billboard into place
            D3DXMATRIX mat = g_Camera.GetBillboardMatrix();
            mat._41 = fX;
            mat._42 = fY;
            mat._43 = fZ;
            g_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );

            DWORD dwColor = pSprite->dwColor;

            g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
            g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
            g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
            g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

            if( pObject->dwType == OBJ_CLOUD )
            {
                DWORD red = 255-(int)(pSprite->frame*255.0f);
                DWORD grn = 255-(int)(pSprite->frame*511.0f);
                DWORD blu = 255-(int)(pSprite->frame*1023.0f);
                if( grn > 255 ) grn = 0;
                if( blu > 255 ) blu = 0;
                dwColor = 0xff000000 + (red<<16) + (grn<<8) + blu;

                g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
                g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
                g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
                g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
            }

            FLOAT h = 300.0f*pObject->vPos.y + 0.1f;

            SPRITEVERTEX* v;
            g_pSpriteVB->Lock( 0, 0, (BYTE**)&v, 0 );
            v[0].p = D3DXVECTOR3(x1,y1+h,0); v[0].color=dwColor; v[0].tu=tu1; v[0].tv=tv2;
            v[1].p = D3DXVECTOR3(x1,y2+h,0); v[1].color=dwColor; v[1].tu=tu1; v[1].tv=tv1;
            v[2].p = D3DXVECTOR3(x2,y1+h,0); v[2].color=dwColor; v[2].tu=tu2; v[2].tv=tv2;
            v[3].p = D3DXVECTOR3(x2,y2+h,0); v[3].color=dwColor; v[3].tu=tu2; v[3].tv=tv1;
            g_pSpriteVB->Unlock();

            // Render the billboarded sprite
            g_pd3dDevice->SetVertexShader( D3DFVF_SPRITEVERTEX );
            g_pd3dDevice->SetStreamSource( 0, g_pSpriteVB, sizeof(SPRITEVERTEX) );
            g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
        }

        // Display all bullets
        for( pObject = g_pDisplayList; pObject; pObject = pObject->pNext )
        {
            if( pObject->dwType != OBJ_BULLET )
                continue;

            // This is really a 3D sprite
            C3DSprite* pSprite = (C3DSprite*)pObject;

            FLOAT u1 = (FLOAT)(pSprite->dwTextureOffsetX + pSprite->dwTextureWidth *(((int)pSprite->frame)%pSprite->dwFramesPerLine));
            FLOAT v1 = (FLOAT)(pSprite->dwTextureOffsetY + pSprite->dwTextureHeight*(((int)pSprite->frame)/pSprite->dwFramesPerLine));
            u1 = (FLOAT)(pSprite->dwTextureOffsetX);
            v1 = (FLOAT)(pSprite->dwTextureOffsetY);

            FLOAT tu1 = u1 / (256.0f-1.0f);
            FLOAT tv1 = v1 / (256.0f-1.0f);
            FLOAT tu2 = (u1 + pSprite->dwTextureWidth -1) / (256.0f-1.0f);
            FLOAT tv2 = (v1 + pSprite->dwTextureHeight-1) / (256.0f-1.0f);

            // Set render states
            g_pd3dDevice->SetTexture( 0, g_pGameTexture2 );
            g_pd3dDevice->SetVertexShader( D3DFVF_SPRITEVERTEX );
            g_pd3dDevice->SetStreamSource( 0, g_pSpriteVB, sizeof(SPRITEVERTEX) );
            g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,     D3DBLEND_ONE );
            g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,    D3DBLEND_ONE );
            g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
            g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

            FLOAT x1 = -0.01f;
            FLOAT x2 =  0.01f;
            FLOAT y1 = -0.01f;
            FLOAT y2 =  0.01f;

            DWORD dwColor = pSprite->dwColor;

            for( DWORD a=0; a<6; a++ )
            {
                FLOAT fX =  pObject->vPos.x - a*a*0.0005f*pObject->vVel.x;
                FLOAT fZ = -pObject->vPos.z + a*a*0.0005f*pObject->vVel.z;
                FLOAT fY =  HeightField( fX, fZ );

                // Translate the billboard into place
                D3DXMATRIX mat = g_Camera.GetBillboardMatrix();
                mat._41 = fX;
                mat._42 = fY;
                mat._43 = fZ;
                g_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );

                FLOAT h = 300.0f*pObject->vPos.y + 0.1f;

                SPRITEVERTEX* v;
                g_pSpriteVB->Lock( 0, 0, (BYTE**)&v, 0 );
                v[0].p = D3DXVECTOR3(x1,y1+h,0); v[0].color=dwColor; v[0].tu=tu1; v[0].tv=tv2;
                v[1].p = D3DXVECTOR3(x1,y2+h,0); v[1].color=dwColor; v[1].tu=tu1; v[1].tv=tv1;
                v[2].p = D3DXVECTOR3(x2,y1+h,0); v[2].color=dwColor; v[2].tu=tu2; v[2].tv=tv2;
                v[3].p = D3DXVECTOR3(x2,y2+h,0); v[3].color=dwColor; v[3].tu=tu2; v[3].tv=tv1;
                g_pSpriteVB->Unlock();

                // Render the billboarded sprite
                g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
            }
        }

        // Restore state
        g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
        g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

        // Display score
        swprintf( strBuffer, L"Score: %08ld", g_dwScore );
        g_GameFont.DrawText( 64.0f, 50.0f, 0xffffff00, strBuffer );

        // Display ship type
        swprintf( strBuffer, L"Ship: %s", g_strShipNames[g_dwCurrentShipType] );
        g_GameFont.DrawText( 64.0f, 75.0f, 0xffffff00, strBuffer );

        // Display weapon type
        WCHAR* strWeapon;
        if( g_dwBulletType == 0 )      strWeapon = L"Weapon: Blaster";
        else if( g_dwBulletType == 1 ) strWeapon = L"Weapon: Double blaster";
        else if( g_dwBulletType == 2 ) strWeapon = L"Weapon: Spray gun";
        else                           strWeapon = L"Weapon: Proximity killer";
        g_GameFont.DrawText( 64.0f, 100.0f, 0xffffff00, strWeapon );

        // Render "Paused" text if game is paused
        if( g_bPaused )
        {
            DarkenScene( 0.5f );
            RenderFieryEffect( g_pPausedTexture, 2.0f );
        }

        if( g_pShip->fShowDelay > 0.0f )
            DarkenScene( g_pShip->fShowDelay/2.0f );

        // Render game menu
        if( g_pCurrentMenu )
        {
            DarkenScene( 0.5f );
            g_pCurrentMenu->Render( g_pd3dDevice );
        }
    }
}




//-----------------------------------------------------------------------------
// Name: DeleteFromList()
// Desc:
//-----------------------------------------------------------------------------
VOID DeleteFromList( DisplayObject* pObject )
{
    if( pObject->pNext )
        pObject->pNext->pPrev = pObject->pPrev;
    if( pObject->pPrev )
        pObject->pPrev->pNext = pObject->pNext;
    delete( pObject );
}



//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
VOID ConstructMenus()
{
    // Build video sub menu
    CMenuItem* pVideoSubMenu = new CMenuItem( g_MenuFont.CreateTexture( L"Video Menu" ), MENU_VIDEO );
    pVideoSubMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"Windowed" ), MENU_WINDOWED ) );
    pVideoSubMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"640x480" ),  MENU_640x480 ) );
    pVideoSubMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"800x600" ),  MENU_800x600 ) );
    pVideoSubMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"1024x768" ), MENU_1024x768 ) );
    pVideoSubMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"Back" ),     MENU_BACK ) );

    // Build sound menu
    CMenuItem* pSoundSubMenu = new CMenuItem( g_MenuFont.CreateTexture( L"Sound Menu" ), MENU_SOUND );
    pSoundSubMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"Sound On" ),  MENU_SOUNDON ) );
    pSoundSubMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"Sound Off" ), MENU_SOUNDOFF ) );
    pSoundSubMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"Back" ),      MENU_BACK ) );

    // Build input menu
    CMenuItem* pInputSubMenu = new CMenuItem( g_MenuFont.CreateTexture( L"Input Menu" ), MENU_INPUT );
    pInputSubMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"View Devices" ),   MENU_VIEWDEVICES ) );
    pInputSubMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"Config Devices" ), MENU_CONFIGDEVICES ) );
    pInputSubMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"Back" ),           MENU_BACK ) );

    // Build main menu
    g_pMainMenu = new CMenuItem( g_MenuFont.CreateTexture( L"Main Menu" ),  MENU_MAIN );
    g_pMainMenu->Add( pVideoSubMenu );
    g_pMainMenu->Add( pSoundSubMenu );
    g_pMainMenu->Add( pInputSubMenu );
    g_pMainMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"Back to Game" ), MENU_BACK ) );

    // Build "quit game?" menu
    g_pQuitMenu = new CMenuItem( g_MenuFont.CreateTexture( L"Quit Game?" ), MENU_MAIN );
    g_pQuitMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"Yes" ), MENU_QUIT ) );
    g_pQuitMenu->Add( new CMenuItem( g_MenuFont.CreateTexture( L"No" ),  MENU_BACK ) );

    return;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
VOID DestroyMenus()
{
    SAFE_DELETE( g_pQuitMenu );
    SAFE_DELETE( g_pMainMenu );
}




//-----------------------------------------------------------------------------
// Name: UpdateMenus()
// Desc:
//-----------------------------------------------------------------------------
VOID UpdateMenus()
{
    if( g_pCurrentMenu == NULL )
        return;

    // Check for menu up/down input
    if( g_bMenuUp )
    {
        if( g_pCurrentMenu->GetSelectedMenu() > 0 )
        {
            g_pCurrentMenu->SetSelectedMenu( g_pCurrentMenu->GetSelectedMenu() - 1 );
#ifdef SOUND_ACTUALLY_WORKS
            g_SphereExplodeSound.Play();
#endif // SOUND_ACTUALLY_WORKS
        }
    }
    else if( g_bMenuDown )
    {
        if( g_pCurrentMenu->GetSelectedMenu() + 1 < g_pCurrentMenu->GetNumChildren()  )
        {
            g_pCurrentMenu->SetSelectedMenu( g_pCurrentMenu->GetSelectedMenu() + 1 );
#ifdef SOUND_ACTUALLY_WORKS
            g_SphereExplodeSound.Play();
#endif // SOUND_ACTUALLY_WORKS
        }
    }

    if( g_bMenuSelect )
    {
#ifdef SOUND_ACTUALLY_WORKS
        g_SphereExplodeSound.Play();
#endif // SOUND_ACTUALLY_WORKS

        DWORD dwID = g_pCurrentMenu->GetSelectedChild()->GetID();

        switch( dwID )
        {
            case MENU_BACK:
                g_pCurrentMenu = g_pCurrentMenu->GetParent();
                break;

            case MENU_VIDEO:
            case MENU_SOUND:
            case MENU_INPUT:
                g_pCurrentMenu = g_pCurrentMenu->GetSelectedChild();
                break;

            case MENU_WINDOWED:
                g_pCurrentMenu = NULL;
                break;

            case MENU_640x480:
                g_pCurrentMenu = NULL;
                break;

            case MENU_800x600:
                g_pCurrentMenu = NULL;
                break;

            case MENU_1024x768:
                g_pCurrentMenu = NULL;
                break;

            case MENU_SOUNDON:
                g_pCurrentMenu = NULL;
                break;

            case MENU_SOUNDOFF:
                g_pCurrentMenu = NULL;
                break;

            case MENU_QUIT:
                XLaunchNewImage( NULL, NULL );
                g_pCurrentMenu = NULL;
                break;
        }
    }

    // Clear menu inputs
    g_bMenuUp     = FALSE;
    g_bMenuDown   = FALSE;
    g_bMenuSelect = FALSE;
}




//-----------------------------------------------------------------------------
// Name: CreateDisplayObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CreateDisplayObjects()
{
    HRESULT hr;

    // Create the resources
    if( FAILED( g_xprResource.Create( g_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the game and menu fonts
    if( FAILED( hr = g_GameFont.Create( g_pd3dDevice, "FontTahoma.xpr" ) ) )
        return hr;

    if( FAILED( hr = g_MenuFont.Create( g_pd3dDevice, "FontImpact.xpr" ) ) )
        return hr;

    // Load the game textures
	g_pGameTexture1 = g_xprResource.GetTexture( "Donuts1.tga" );
	g_pGameTexture2 = g_xprResource.GetTexture( "Donuts2.tga" );

    // Load the geometry models
    if( FAILED( hr = LoadShipModel() ) )
        return hr;
    if( FAILED( hr = LoadTerrainModel() ) )
        return hr;

    // Create a viewport covering sqaure
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof(SCREENVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_SCREENVERTEX,
                                                  D3DPOOL_MANAGED, &g_pViewportVB ) ) )
        return E_FAIL;

    // Create a sqaure for rendering the sprites
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof(SPRITEVERTEX),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_SPRITEVERTEX,
                                                  D3DPOOL_MANAGED, &g_pSpriteVB ) ) )
        return E_FAIL;

    // Setup dimensions for the viewport covering sqaure
    SCREENVERTEX* v;
    g_pViewportVB->Lock( 0, 0, (BYTE**)&v, 0 );
    v[0].p = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 0.0f, 1.0f );
    v[1].p = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 0.0f, 1.0f );
    v[2].p = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 0.0f, 1.0f );
    v[3].p = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 0.0f, 1.0f );
    g_pViewportVB->Unlock();

    // Load the skybox
    if( FAILED( g_SkyBoxObject.Create( g_pd3dDevice, "Models\\SkyBox.xbg", &g_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set up the camera
    g_Camera.SetViewParams( D3DXVECTOR3(0.0f,0.0f,0.0f), D3DXVECTOR3(0.0f,0.0f,1.0f),
                            D3DXVECTOR3(0.0f,1.0f,0.0f) );
    g_Camera.SetProjParams( D3DX_PI/4, 1.0f, 0.1f, 100.0f );

    // Set up default matrices (using the CD3DCamera class)
    g_pd3dDevice->SetTransform( D3DTS_VIEW,       &g_Camera.GetViewMatrix() );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_Camera.GetProjMatrix() );

    // Setup a material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f );
    g_pd3dDevice->SetMaterial( &mtrl );

    // Set up lighting states
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 1.0f, -1.0f, 1.0f );
    g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );

    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x33333333 );

    // Set miscellaneous render states
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    // The display is now ready
    g_bDisplayReady = TRUE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DestroyDisplayObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT DestroyDisplayObjects()
{
    DisplayObject* pDO;
    while( g_pDisplayList != NULL )
    {
        pDO = g_pDisplayList;
        g_pDisplayList = g_pDisplayList->pNext;
        delete pDO;
        if( g_pDisplayList != NULL)
            g_pDisplayList->pPrev = NULL;
    }
    
    if( g_pShipFileObject != NULL )
        delete g_pShipFileObject;
    if( g_pTerrain != NULL )
        delete g_pTerrain;

    SAFE_DELETE( g_pTerrain );
    SAFE_DELETE( g_pShipFileObject );

    SAFE_RELEASE( g_pViewportVB );
    SAFE_RELEASE( g_pSpriteVB );
    SAFE_RELEASE( g_pd3dDevice );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Sound support code (using DMusic functionality from DMUtil.h)
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Name: CreateSoundObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CreateSoundObjects()
{
#ifdef SOUND_ACTUALLY_WORKS
    // Create the sounds
    g_BeginLevelSound.Create( "Sounds\\BEGINLEVEL.wav", DSBCAPS_CTRL3D );
    g_EngineIdleSound.Create( "Sounds\\ENGINEIDLE.wav", DSBCAPS_CTRL3D );
    g_EngineRevSound.Create( "Sounds\\ENGINEREV.wav", DSBCAPS_CTRL3D );
    g_ShieldBuzzSound.Create( "Sounds\\SHIELDBUZZ.wav", DSBCAPS_CTRL3D );
    g_ShipExplodeSound.Create( "Sounds\\SHIPEXPLODE.wav", DSBCAPS_CTRL3D );
    g_FireBulletSound.Create( "Sounds\\GUNFIRE.wav", DSBCAPS_CTRL3D );
    g_ShipBounceSound.Create( "Sounds\\SHIPBOUNCE.wav", DSBCAPS_CTRL3D );
    g_DonutExplodeSound.Create( "Sounds\\DONUTEXPLODE.wav", DSBCAPS_CTRL3D );
    g_PyramidExplodeSound.Create( "Sounds\\PYRAMIDEXPLODE.wav", DSBCAPS_CTRL3D );
    g_CubeExplodeSound.Create( "Sounds\\CUBEEXPLODE.wav", DSBCAPS_CTRL3D );
    g_SphereExplodeSound.Create( "Sounds\\SPHEREEXPLODE.wav", DSBCAPS_CTRL3D );
#endif // SOUND_ACTUALLY_WORKS

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DestroySoundObjects()
// Desc:
//-----------------------------------------------------------------------------
VOID DestroySoundObjects()
{
#ifdef SOUND_ACTUALLY_WORKS
    g_BeginLevelSound.Destroy();
    g_EngineIdleSound.Destroy();
    g_EngineRevSound.Destroy();
    g_ShieldBuzzSound.Destroy();
    g_ShipExplodeSound.Destroy();
    g_FireBulletSound.Destroy();
    g_ShipBounceSound.Destroy();
    g_DonutExplodeSound.Destroy();
    g_PyramidExplodeSound.Destroy();
    g_CubeExplodeSound.Destroy();
    g_SphereExplodeSound.Destroy();
#endif // SOUND_ACTUALLY_WORKS
}




//-----------------------------------------------------------------------------
// Name: GetInput()
// Desc: Processes data from the input device.  Uses GetDeviceState().
//-----------------------------------------------------------------------------
VOID GetInput()
{
    static FLOAT fLeftThrust    = 0.0f;
    static FLOAT fRightThrust   = 0.0f;
    static FLOAT fForwardThrust = 0.0f;
    static FLOAT fReverseThrust = 0.0f;

    if( g_pCurrentMenu )
    {
        g_bMenuLeft   = ( 0 != ( g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ) );
        g_bMenuRight  = ( 0 != ( g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) );
        g_bMenuUp     = ( 0 != ( g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ) );
        g_bMenuDown   = ( 0 != ( g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ) );
        g_bMenuSelect = ( 0 != ( g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_START ) );

        // End menu
        if( g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_BACK )
        {
#ifdef SOUND_ACTUALLY_WORKS
            g_SphereExplodeSound.Play();
#endif // SOUND_ACTUALLY_WORKS
            g_pCurrentMenu = NULL;
        }
    }
    else
    {
        if( g_pShip->bVisible  )
        {
            g_fBank          = g_Gamepads[0].fX1;
            g_fThrust        = g_Gamepads[0].fY1;
            g_bFiringWeapons = ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_A] > 0 );
            g_bChangeView    = ( g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_B] );

            if( g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_X] ) 
            {
                if( ++g_dwBulletType > 3 )
                    g_dwBulletType = 0L;
            }
    
            if( g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_Y] ) 
                SwitchModel();
        }

        // Pause
        if( g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_BACK )
            g_bPaused = !g_bPaused;
    
        // Display menu
        if( g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_START )
        {
#ifdef SOUND_ACTUALLY_WORKS
            g_SphereExplodeSound.Play();
#endif // SOUND_ACTUALLY_WORKS
            g_pCurrentMenu = g_pMainMenu;
        }
    }
}




