//-----------------------------------------------------------------------------
// File: DebugKeyboard.cpp
//
// Desc: This sample is a demonstration of using the debug keyboard.
//
// Hist: 06.11.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xgraphics.h>
#include <stdio.h>
#include <XBApp.h>
#include <XBInput.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBUtil.h>
#include <XBMesh.h>
#include "console.h"
#include "commands.h"
#include "keyboard.h"
#include "resource.h"


//-----------------------------------------------------------------------------
// Graphics helper function
//-----------------------------------------------------------------------------
VOID DrawFilledRect( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, DWORD dwFillColor, 
                     DWORD dwOutlineColor );



//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "Wings.bmp",  resource_Wings_OFFSET },
    { "BiHull.bmp", resource_BiHull_OFFSET },
    { NULL, 0 },
};




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallout[] = 
{
    { XBHELP_START_BUTTON,  XBHELP_PLACEMENT_2, L"Pause" },
    { XBHELP_BACK_BUTTON,   XBHELP_PLACEMENT_2, L"Display help" },
};  

#define NUM_HELP_CALLOUTS 2




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    // Fonts
    CXBFont            m_Font;
    CXBFont            m_ConsoleFont;

    // Help system
    CXBHelp            m_Help;
    BOOL               m_bDrawHelp;

    // Plane model details
    CXBPackedResource  m_xprResource;
    CXBMesh*           m_pPlaneMesh;

public:
    HRESULT Initialize();
    HRESULT Render();
    HRESULT FrameMove();

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
    m_bDrawHelp = FALSE;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create fonts
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return E_FAIL;

    if( FAILED( m_ConsoleFont.Create( m_pd3dDevice, "SystemFont.xpr" ) ) )
        return E_FAIL;

    // Create help
    if( FAILED( m_Help.Create( g_pd3dDevice, "Gamepad.xpr" ) ) )
        return E_FAIL;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Initialize the airplanes
    m_pPlaneMesh = new CXBMesh;
    m_pPlaneMesh ->Create(g_pd3dDevice, "Models\\Airplane.xbg", &m_xprResource );

    // Initialize the debug keyboard
    if( FAILED( XBInput_InitDebugKeyboard() ) )
        return E_FAIL;

    // Initialize the console
    InitConsole( &m_ConsoleFont );

    // Set projection transform
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f/480.0f, 0.1f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set view position
    D3DXMATRIX matView;
    D3DXMatrixTranslation( &matView, 0.0f, 0.0f, 40.0f);
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene. As this code only changes text, there is no real animation
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
        m_bDrawHelp = !m_bDrawHelp;

    if( IsConsoleActive() != TRUE )
    {
        if( XBInput_GetKeyboardInput() == ESC_KEY )
        {
            ToggleConsole();
        }
    }

    // Move plane
    FLOAT x =  20.0f * cosf(1.57f*m_fAppTime);
    FLOAT z = -20.0f * sinf(1.57f*m_fAppTime);
    FLOAT y =   4.0f;
    D3DXMATRIX matWorld, matTrans;
    D3DXMatrixRotationY( &matWorld, 1.57f*m_fAppTime );
    D3DXMatrixTranslation( &matTrans, x, y, z );
    D3DXMatrixMultiply( &matWorld, &matWorld, &matTrans );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

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
    // Render the scene plane
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         D3DCOLOR_XRGB( 0x00, 0x50, 0x00), 1.0f, 0L );
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
    m_pPlaneMesh->Render(0);

    // Display the console
    if( IsConsoleActive() == TRUE )
    {
        ProcessConsole();
        DrawConsole();
    }

    // Display help and application text
    if( m_bDrawHelp )
    {
        m_Help.Render( &m_Font, g_HelpCallout, NUM_HELP_CALLOUTS );
    }
    else
    {
        // Draw a rectangle and a semitrans poly to put the text in
        DrawFilledRect( 64, 248, 564, 398, 0x40000000, 0xff000000 );

        m_Font.Begin();
        m_Font.DrawText(  48, 36, 0xffffff00, L"DebugKeyboard" );
        m_Font.DrawText( 450, 36, 0xffffff00, m_strFrameRate );
        m_Font.DrawText(  66, 250, 0xffffff00, L"Press the ESC key on the Xbox debug keyboard to\n"
                                               L"open the console window. Console supports help,\n"
                                               L"clear, and reboot. Other text will just be displayed\n"
                                               L"in the console window." );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawFilledRect()
// Desc: Draws filled rectangle on screen
//-----------------------------------------------------------------------------
VOID DrawFilledRect( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, DWORD dwFillColor, 
                     DWORD dwOutlineColor )
{
    D3DXVECTOR4 v[4];
    v[0] = D3DXVECTOR4( x1-0.5f, y1-0.5f, 1.0f, 1.0f );
    v[1] = D3DXVECTOR4( x2-0.5f, y1-0.5f, 1.0f, 1.0f );
    v[2] = D3DXVECTOR4( x2-0.5f, y2-0.5f, 1.0f, 1.0f );
    v[3] = D3DXVECTOR4( x1-0.5f, y2-0.5f, 1.0f, 1.0f );
    
    // Set render states
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW );

    // Draw rectangle inside
    g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwFillColor );
    g_pd3dDevice->DrawVerticesUP( D3DPT_QUADLIST, 4, v, sizeof(D3DXVECTOR4) );

    // Draw rectangle outline
    g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwOutlineColor );
    g_pd3dDevice->DrawVerticesUP( D3DPT_LINELOOP, 4, v, sizeof(D3DXVECTOR4) );
}





