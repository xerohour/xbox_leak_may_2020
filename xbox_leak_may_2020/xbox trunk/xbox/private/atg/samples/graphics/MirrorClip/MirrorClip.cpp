//-----------------------------------------------------------------------------
// File: MirrorClip.cpp
//
// Desc: Example code showing how to clip to a mirror plane on XBox.
//
// Hist: 02.21.00 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XBResource.h>
#include <XBUtil.h>

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
    { "Floor.bmp",     resource_Floor_OFFSET },
    { "Ceiling.bmp",   resource_Ceiling_OFFSET },
    { "WestWall.bmp",  resource_WestWall_OFFSET },
    { "EastWall.bmp",  resource_EastWall_OFFSET },
    { "NorthWall.bmp", resource_NorthWall_OFFSET },
    { "SouthWall.bmp", resource_SouthWall_OFFSET },
    { NULL, 0 },
};




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,   XBHELP_PLACEMENT_1, L"Move camera" },
    { XBHELP_X_BUTTON,    XBHELP_PLACEMENT_1, L"Toggle Clipping" },
    { XBHELP_BACK_BUTTON, XBHELP_PLACEMENT_1, L"Display help" },
};

#define NUM_HELP_CALLOUTS (sizeof(g_HelpCallouts)/sizeof(g_HelpCallouts[0]))




//-----------------------------------------------------------------------------
// Custom vertex types
//-----------------------------------------------------------------------------
struct MIRRORVERT
{
    D3DVECTOR Pos;
    D3DCOLOR  Color;
};


MIRRORVERT g_MirrorVerts[] =
{
    { { 0.0f, -0.5f, -0.1f }, D3DCOLOR_ARGB(60, 0, 0, 255) },
    { { 0.0f,  0.5f, -0.1f }, D3DCOLOR_ARGB(60, 0, 0, 255) },
    { { 0.0f, -0.5f,  2.0f }, D3DCOLOR_ARGB(60, 0, 0, 255) },
    { { 0.0f,  0.5f,  2.0f }, D3DCOLOR_ARGB(60, 0, 0, 255) },
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource  m_xprResource;        // Packed resources for the app
    CXBFont            m_Font;               // Font class
    CXBHelp            m_Help;               // Help class
    BOOL               m_bDrawHelp;          // Whether to draw help

    CXBMesh            m_Room;               // XBG file object to render

    bool               m_bUseClipPlane;

    D3DXVECTOR3        m_vEye;

    HRESULT RenderScene();
    HRESULT RenderMirror();

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
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
            :CXBApplication()
{
    m_bDrawHelp     = false;
    m_bUseClipPlane = false;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the main file object
    if( FAILED( m_Room.Create( m_pd3dDevice, "Models\\Room.xbg", &m_xprResource ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set the matrices
    D3DXMATRIX matWorld, matView, matProj;

    D3DXMatrixIdentity( &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    m_vEye = D3DXVECTOR3( 2.0f, 0.0f, 1.0f );
    D3DXVECTOR3 vAt( 0.0f, 0.0f, 1.0f );
    D3DXVECTOR3 vUp( 0.0f, 0.0f, 1.0f );

    D3DXMatrixLookAtLH( &matView, &m_vEye, &vAt, &vUp );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/3, 4.0f/3.0f, 0.1f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    D3DXMATRIX matView, matRotate;

    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Toggle clipping plane.
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        m_bUseClipPlane = !m_bUseClipPlane;

    D3DXVECTOR3 vAt( 0.0f, 0.0f, 1.0f );
    D3DXVECTOR3 vUp( 0.0f, 0.0f, 1.0f );

    // Rotate eye around z axis.
    D3DXMatrixRotationAxis( &matRotate, &D3DXVECTOR3(0.0f,0.0f,1.0f), 
                                        m_DefaultGamepad.fX1*m_fElapsedTime );
    D3DXVec3TransformCoord( &m_vEye, &m_vEye, &matRotate );

    // Rotate eye points around side axis.
    D3DXVECTOR3 vView = (m_vEye - vAt);
    D3DXVec3Normalize( &vView, &vView );

    // Place limits so we dont go over the top or under the bottom.
    FLOAT dot = D3DXVec3Dot( &vView, &vUp );
    if( (dot > 0.0f || m_DefaultGamepad.fY1 < 0.0f) && (dot < 0.99f || m_DefaultGamepad.fY1 > 0.0f) )
    {
        D3DXVECTOR3 axis;
        D3DXVec3Cross( &axis, &vUp, &vView );
        D3DXMatrixRotationAxis( &matRotate, &axis, m_DefaultGamepad.fY1*m_fElapsedTime );
        D3DXVec3TransformCoord( &m_vEye, &m_vEye, &matRotate );
    }

    D3DXMatrixLookAtLH( &matView, &m_vEye, &vAt, &vUp );

    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderMirror()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderMirror()
{
    // Turn on stencil writes.
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILREF, 0x1 );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILMASK, 0xffffffff );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE );

    // Disable writing to the z and color buffers.
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0 );

    // Draw the mirror into the stencil buffer
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZ | D3DFVF_DIFFUSE );

    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_MirrorVerts, 
                                   sizeof(MIRRORVERT) );

    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Save the view and projection matrix.
    D3DXMATRIX matViewSaved, matProjectionSaved;
    m_pd3dDevice->GetTransform( D3DTS_VIEW, &matViewSaved );
    m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProjectionSaved );

    if( m_bUseClipPlane )
    {
        // Build the reflection transform.
        D3DXMATRIX matView, matReflect;

        // Reflector in world space.
        D3DXVECTOR3 point(0.0f,0.0f,0.0f);
        D3DXVECTOR3 normal(-1.0f,0.0f,0.0f);
        D3DXPLANE   plane;

        // Move mirror point and normal into eye space.
        D3DXVec3TransformCoord(&point, &point, &matViewSaved);
        D3DXVec3TransformNormal(&normal, &normal, &matViewSaved);

        // Compute mirror plane.
        D3DXPlaneFromPointNormal( &plane, &point, &normal );

        D3DXMatrixReflect( &matReflect, &plane );

        D3DXMatrixMultiply( &matView, &matViewSaved, &matReflect );

        m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
    }
    else
    {
        // Build the reflection transform.
        D3DXMATRIX matView, matReflect;

        // Reflector in world space.
        D3DXVECTOR3 point(0.0f,0.0f,0.0f);
        D3DXVECTOR3 normal(-1.0f,0.0f,0.0f);
        D3DXPLANE plane;

        // Move mirror point and normal into eye space.
        D3DXVec3TransformCoord(&point, &point, &matViewSaved);
        D3DXVec3TransformNormal(&normal, &normal, &matViewSaved);

        // Compute mirror plane.
        D3DXPlaneFromPointNormal( &plane, &point, &normal );

        D3DXMatrixReflect( &matReflect, &plane );
        D3DXMatrixMultiply( &matView, &matViewSaved, &matReflect );

        m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

        //
        // Adjust the projection matrix so that the near clip plane is aligned
        // with the mirror plane.
        //
        float zn = -plane.d / plane.c;
        float zf = 100.0f;

        D3DXMATRIX matProjection;

        matProjection._11 = matProjectionSaved._11;
        matProjection._21 = 0.0f;
        matProjection._31 = 0.0f;
        matProjection._41 = 0.0f;

        matProjection._12 = 0.0f;
        matProjection._22 = matProjectionSaved._22;
        matProjection._32 = 0.0f;
        matProjection._42 = 0.0f;

        // z's dependance on x and y.
        float nxnz = normal.x / normal.z;
        float nynz = normal.y / normal.z;

        // Maximum z contributed by x and y at farclip.
        float maxxz = zf * fabsf(nxnz) * (1.0f / matProjection._11);
        float maxyz = zf * fabsf(nynz) * (1.0f / matProjection._22);

        // Scale factor for the z range.
        float zscale = zf / (zf + maxxz + maxyz - zn);

        matProjection._13 = nxnz * zscale;
        matProjection._23 = nynz * zscale;
        matProjection._33 = zscale;
        matProjection._43 = -zn * zscale;

        matProjection._14 = 0.0f;
        matProjection._24 = 0.0f;
        matProjection._34 = 1.0f;
        matProjection._44 = 0.0f;

        m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjection );
    }

    // Setup render states to a blended render scene against mask in stencil
    // buffer. An important step here is to reverse the cull-order of the
    // polygons, since the view matrix is being relected.
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

    m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | 
                                                          D3DCOLORWRITEENABLE_BLUE |
                                                          D3DCOLORWRITEENABLE_GREEN |
                                                          D3DCOLORWRITEENABLE_RED  );

    m_pd3dDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

    // Render the scene
    RenderScene();

    // Restore the transform, projection and culling.
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matViewSaved );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjectionSaved );

    // Blend the mirror on top of the geometry.
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    // Ignore z values in stenciled area.
    m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );

    m_pd3dDevice->SetTexture( 0, NULL );

    m_pd3dDevice->SetVertexShader( D3DFVF_XYZ | D3DFVF_DIFFUSE );

    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_MirrorVerts, 
                                   sizeof(MIRRORVERT) );

    m_pd3dDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: RenderScene()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderScene()
{
    // Set up the light
    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.7071067f, -0.7071067f, 0.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );

    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00555555 );

    // Draw the room.
    m_Room.Render( m_pd3dDevice );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport, zbuffer, and stencil buffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00ff0000, 1.0f, 0L );

    // Set up misc render states
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

    // Draw the reflected stuff first.
    RenderMirror();

    // Render the scene.
    RenderScene();

    // Show title, frame rate, and help
    if( m_bDrawHelp )
    {
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    }
    else
    {
        // Show frame rate
        m_Font.DrawText(  64, 50, 0xffffffff, L"MirrorClip" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



