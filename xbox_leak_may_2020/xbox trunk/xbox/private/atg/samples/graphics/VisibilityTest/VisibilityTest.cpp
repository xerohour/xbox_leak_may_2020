//-----------------------------------------------------------------------------
// File: VisibilityTest.cpp
//
// Desc: Illustrates how to do visibility testing on the XBox.
//
//       This sample draws two objects. One is possibly occluded by the other.
//       After starting the vis test on the back object, we wait for the
//       test to complete before either rendering or not rendering the object.
//       In a real game situation, you would do several different things...
//       First, you would intertwine vis tests with actual rendering so the
//       vis test would have time to complete before you check the results.
//       Second when you finally got around to checking the results, if a test
//       had not yet concluded, you would either draw additional geometry, or
//       if no additional geometry was able to be drawn, proceed as if the 
//       object was visible. The point is to not sit around waiting for tests 
//       to conclude. The graphics pipe needs to be busy 100% of the time.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>

#include "resource.h"

#include <xpath.h>
#include <xmenu.h>
#include <xobj.h>




//-----------------------------------------------------------------------------
// Help screen definitions
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] =
{
    { XBHELP_LEFTSTICK,   XBHELP_PLACEMENT_1, L"Move wall in X, Y" },
    { XBHELP_RIGHTSTICK,  XBHELP_PLACEMENT_1, L"Move wall in Z" },
    { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_1, L"Display\nhelp" },
};
#define NUM_HELP_CALLOUTS 3


//-----------------------------------------------------------------------------
// ASCII names for the resources used by the app
//-----------------------------------------------------------------------------
XBRESOURCE g_ResourceNames[] = 
{
    { "floor.bmp", resource_Floor_OFFSET },
    { NULL, 0 },
};



//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont     m_Font;
    CXBHelp     m_Help;
    BOOL        m_bDrawHelp;

    D3DXVECTOR3 m_vCameraPos;
    D3DXVECTOR3 m_vCameraRot;

    CXBPackedResource  m_xprResource;

    CXObject    m_WallObj;
    CXObject    m_SphereObj;

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
    m_bDrawHelp  = FALSE;

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

    // Set projection transform
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 640.0f/480.0f, 0.1f, 1000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // Set the view matrix based on the camera position
    D3DXMATRIX matView;
    D3DXMatrixTranslation( &matView, -m_vCameraPos.x, -m_vCameraPos.y, -m_vCameraPos.z);
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set base path for creating/loading geometry
    XPath_SetBasePath( _T("D:\\Media\\") );

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

	// set pointer to resources for models
	g_pModelResource = &m_xprResource;

    // Create the wall object
    m_WallObj.m_Model = new CXModel;
    m_WallObj.m_Model->GridXZ( 16, 16, 20.0f, 20.0f, FVF_XYZTEX1, "floor.bmp", 20.0f, 20.0f);
    m_WallObj.SetPosition(0.0f, 0.0f, 20.0f);
    m_WallObj.m_vRotation.y = +(D3DX_PI/2.0f);
    m_WallObj.m_vRotation.z = -(D3DX_PI/2.0f);

    // Create the sphere object
    m_SphereObj.m_Model = new CXModel;
    m_SphereObj.m_Model->Sphere( 10.0f, 16, 16, FVF_XYZTEX1, "floor.bmp", 5.0f);
    m_SphereObj.SetPosition( 0.0f, 0.0f, 30.0f );

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
    if( m_DefaultGamepad.wPressedButtons&XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Move the wall around
    m_WallObj.m_vPosition.x += m_DefaultGamepad.fX1*m_fElapsedTime*15.0f;
    m_WallObj.m_vPosition.y += m_DefaultGamepad.fY1*m_fElapsedTime*15.0f;
    m_WallObj.m_vPosition.z += m_DefaultGamepad.fY2*m_fElapsedTime*25.0f;

    // Rotate the sphere
    m_SphereObj.m_vRotation.y += m_fElapsedTime*2.0f;

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
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         0xff400000, 1.0f, 0L );

    // Restore state that text clobbers
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    // Render the wall
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_WallObj.Render( 0 );

    // Do the visibility test on the sphere.
    //
    // Note 1: We would normally send simple test geometry for the vis test
    //         and not the full object.
    // Note 2: We could send multiple objects between the
    //         BeginVisibilityTest() EndVisibilityTest() pair. The call to 
    //         GetVisibilityTestResult() would then return the total number of
    //         pixels drawn by all objects.

    // We dont want color or z buffer updated, so we disable them.
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x00000000 );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,     FALSE );

    m_pd3dDevice->BeginVisibilityTest();
    m_SphereObj.Render( 0 );
    m_pd3dDevice->EndVisibilityTest( 0 );

    // Re-enable color and z writes
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );

    // Check the number of pixels that would have been drawn by 
    // the visibility test geometry. keep trying till we get a
    // complete result. (***SEE NOTE AT TOP OF FILE***)
    UINT    dwNumPixelsDrawn;
    HRESULT hr;
    do
    {
        hr = m_pd3dDevice->GetVisibilityTestResult( 0, &dwNumPixelsDrawn, NULL );
    }
    while( hr==D3DERR_TESTINCOMPLETE );

    // If pixels would have been drawn, we send the object again
    // with all writes enabled
    if( dwNumPixelsDrawn > 0 )
        m_SphereObj.Render(0);

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
		m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"VisibilityTest" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        // Report if sphere was rendered, and if so, show number of pixels
        if( dwNumPixelsDrawn == 0 )
            m_Font.DrawText( 320, 405, 0xffff0000, L"Sphere not rendered.", XBFONT_CENTER_X );
        else
        {
            WCHAR str[80];
            swprintf( str, L" %d", dwNumPixelsDrawn );
            m_Font.DrawText( 320, 380, 0xff00ff00, L"Sphere rendered.", XBFONT_CENTER_X );
            m_Font.DrawText( 320, 405, 0xff00ff00, L"# Pixels:", XBFONT_RIGHT );
            m_Font.DrawText( 320, 405, 0xff00ff00, str, XBFONT_LEFT );
        }
		m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



