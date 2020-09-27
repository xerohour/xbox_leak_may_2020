//-----------------------------------------------------------------------------
// File: Notifier.cpp
//
// Desc: Illustrates how to use vblank routines and notifiers on the Xbox.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xpath.h>
#include <xmenu.h>
#include <xobj.h>

// The following header file is generated from "Resource.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (Resource.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "Resource.h"




//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "Floor.bmp",    resource_Floor_OFFSET },
    { "BiHull.bmp",   resource_BiHull_OFFSET },
    { "Wings.bmp",    resource_Wings_OFFSET },
    { NULL, 0 },
};

extern CXBPackedResource* g_pModelResource;




//-----------------------------------------------------------------------------
// Help screen definitions
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] =
{
    { XBHELP_LEFTSTICK,   XBHELP_PLACEMENT_1, L"Move Camera X, Z" },
    { XBHELP_RIGHTSTICK,  XBHELP_PLACEMENT_1, L"Move Camera Y" },
    { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_1, L"Display\nhelp" },
    { XBHELP_A_BUTTON,    XBHELP_PLACEMENT_1, L"Toggle WaitVBlank" },
};
#define NUM_HELP_CALLOUTS 4




// Vertical blank callback
void __cdecl VBlankCallback( D3DVBLANKDATA *pData );
DWORD g_dwVBCount = 0;

// Pushbuffer callback
void __cdecl PushBufferCallback( DWORD Context );
DWORD g_dwPBCount = 0;

// # of spheres to draw
#define MAX_SPHERE 128




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource m_xprResource;
    CXBFont     m_Font;
    CXBHelp     m_Help;
    BOOL        m_bDrawHelp;
    BOOL        m_bWaitVBlank;

    D3DXVECTOR3 m_vCameraPos;
    D3DXVECTOR3 m_vCameraRot;

    CXModel*    m_pSphereModel;
    CXObject    m_SphereObj[MAX_SPHERE];
    CXObject    m_PlaneObj;

    float       m_fTime[8];

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
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // initialize our stuff
    m_bDrawHelp  = FALSE;
    m_bWaitVBlank = FALSE;

    m_vCameraPos = D3DXVECTOR3( 0.0f, 0.0f,-15.0f );
    m_vCameraRot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT hr;

    // Create a font
    if( FAILED( hr = m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Initialize the help system
    if( FAILED( hr = m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the packed resource
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr",
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    g_pModelResource = &m_xprResource;

    // Set projection transform
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f/480.0f, 0.1f, 1000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set the view matrix based on the camera position
    D3DXMATRIX matView;
    D3DXMatrixTranslation( &matView, -m_vCameraPos.x, -m_vCameraPos.y, -m_vCameraPos.z );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set base path for creating/loading geometry
    XPath_SetBasePath( _T("D:\\Media\\") );

    // Create the sphere objects
    m_pSphereModel = new CXModel;
    m_pSphereModel->Sphere( 5.0f, 16, 16, FVF_XYZTEX1, "Floor.bmp", 5.0f );

    for( DWORD i=0; i<MAX_SPHERE; i++ )
    {
        m_SphereObj[i].SetModel(m_pSphereModel);
        m_SphereObj[i].SetPosition( (float)rand()*100.0f/32768.0f-50.0f, 
                                    (float)rand()*100.0f/32768.0f-50.0f, 
                                    30.0f+(float)rand()*100.0f/32768.0f );
    }

    // Load up the plane object
    CXBMesh *xbm = new CXBMesh;
    xbm->Create(g_pd3dDevice, "models\\airplane.xbg", &m_xprResource );
    m_PlaneObj.SetXBMesh( xbm );
    m_PlaneObj.SetPosition( 0.0f, 0.0f, 10.0f );

    // Register our vblank callback
    g_pd3dDevice->SetVerticalBlankCallback( VBlankCallback );

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

    // Toggle vblank wait
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
        m_bWaitVBlank = !m_bWaitVBlank;

    // Rotate the spheres...
    for( DWORD i=0; i<MAX_SPHERE; i++ )
        m_SphereObj[i].m_vRotation.y += m_fElapsedTime*1.0f;

    // ...and the plane
    m_PlaneObj.m_vRotation.y += m_fElapsedTime*0.5f;
    m_PlaneObj.m_vRotation.z += m_fElapsedTime;

    // Adjust the camera
    D3DXMATRIX matView;
    m_vCameraPos.x += m_DefaultGamepad.fX1*m_fElapsedTime*8.0f;
    m_vCameraPos.y += m_DefaultGamepad.fY2*m_fElapsedTime*8.0f;
    m_vCameraPos.z += m_DefaultGamepad.fY1*m_fElapsedTime*8.0f;

    D3DXMatrixTranslation( &matView, -m_vCameraPos.x, -m_vCameraPos.y, -m_vCameraPos.z);
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    WCHAR str[80];
    float fBaseTime;
    DWORD dwFence0;

    fBaseTime = XBUtil_Timer(TIMER_GETABSOLUTETIME);

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
                         0xff400000, 1.0f, 0L );

    // Restore state that text clobbers
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

    // Render some spheres
    for( DWORD i=0; i<MAX_SPHERE/4; i++ )
        m_SphereObj[i].Render(0);

    // Do a callback at this point
    m_pd3dDevice->InsertCallback( D3DCALLBACK_READ, (D3DCALLBACK)PushBufferCallback, 0 );

    // Render some more spheres
    for( i=MAX_SPHERE/4; i<MAX_SPHERE; i++ )
        m_SphereObj[i].Render(0);

    // Insert a fence here
    m_fTime[0] = XBUtil_Timer(TIMER_GETABSOLUTETIME) - fBaseTime;
    dwFence0 = m_pd3dDevice->InsertFence();

    // Render the plane
    m_PlaneObj.Render(0);

    // Wait for the fence
    m_pd3dDevice->BlockOnFence(dwFence0);
    m_fTime[1] = XBUtil_Timer(TIMER_GETABSOLUTETIME) - fBaseTime;

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"Notifier" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        swprintf( str, L"VBCount: %d", g_dwVBCount );
        m_Font.DrawText( 64, 70, 0xff00ff00, str );
        swprintf( str, L"PBCount: %d", g_dwPBCount );
        m_Font.DrawText( 64, 90, 0xff00ff00, str );

        swprintf( str, L"Fence Start: %5.4f, Stop: %5.4f", m_fTime[0], m_fTime[1] );
        m_Font.DrawText( 64, 110, 0xff00ff00, str );

        if( m_bWaitVBlank )
            m_Font.DrawText( 64, 130, 0xffffffff, L"WaitVBlank" );

        m_Font.End();
    }

    // If waiting for the vblank, then block until we get it
    if( m_bWaitVBlank )
        m_pd3dDevice->BlockUntilVerticalBlank();

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: VBlankCallback
// Desc: This routine is called at the beginning of the vertical blank.
//-----------------------------------------------------------------------------
void __cdecl VBlankCallback( D3DVBLANKDATA *pData )
{
    g_dwVBCount++;
}




//-----------------------------------------------------------------------------
// Name: PushBufferCallback
// Desc: This routine is called when the pushbuffer hits the point where
//       the callback was added.
//-----------------------------------------------------------------------------
void __cdecl PushBufferCallback( DWORD Context )
{
    g_dwPBCount++;
}
