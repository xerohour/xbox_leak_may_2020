//-----------------------------------------------------------------------------
// File: MultiShader.cpp
//
// Desc: Illustrates having multiple vertex shaders loaded at once.
//
//       Typically, since an app only can use one vertex shader at a time, they
//       use the SetVertexShader() API to load and select a vertex shader.
//       However, there are 136 instruction slots available, so several small
//       vertex shaders could potentially be resident in memory at the same
//       time. To do this, an app uses the LoadVertexShader() and 
//       SelectVertexShader() API, as this sample demonstrates.
//
// Hist: 01.10.00 - New for February XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBUtil.h>




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Select new\nvertex shader" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display\nhelp" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};

#define NUM_HELP_CALLOUTS 3




//-----------------------------------------------------------------------------
// Define our triangle.
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{ 
    FLOAT x,y,z; // Position
    DWORD color; // Diffuse color
};


CUSTOMVERTEX g_Vertices[] =
{
    {-0.6f,-1.0f, 0.0f, 0xffff0000, },
    { 0.0f, 1.0f, 0.0f, 0xff00ff00, },
    { 0.6f,-1.0f, 0.0f, 0xff0000ff, },
};


DWORD g_dwVertexDecl[] =
{
    D3DVSD_STREAM( 0 ),
    D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),   // Position
    D3DVSD_REG( 1, D3DVSDT_D3DCOLOR ), // Diffuse color
    D3DVSD_END()
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont m_Font;
    CXBHelp m_Help;               // Help class
    BOOL    m_bDrawHelp;          // Whether to draw help

    DWORD   m_dwWhichShader;
 
    DWORD   m_dwVertexShader1;
    DWORD   m_dwVertexShader2;
    DWORD   m_dwVertexShader3;
    DWORD   m_dwVertexShader1Address;
    DWORD   m_dwVertexShader2Address;
    DWORD   m_dwVertexShader3Address;

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
    m_bDrawHelp     = FALSE;

    m_dwWhichShader = 1;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Now, load the vertex shaders into memory. 
    DWORD ip = 0;
    UINT  length;

    // NOTE: THIS IS VERY IMPORTANT. Ater loading vertex shaders into memory with
    // the LoadVertexShader API, it will stay resident until it gets kicked out,
    // such as by a call to SetVertexShader(). What is very important to keep in
    // mind, is that the fixed-pipeline, when used with vertices of type
    // D3DXYZ_RHW, uses the first several vertex shader address locations.
    // Therefore, any vertex shaders loaded there will be overwritten.

    // As per the comment above, skip the reserved instruction slots
    ip = D3DVS_XBOX_RESERVEDXYZRHWSLOTS;

    // Create the first vertex shader
    m_dwVertexShader1Address = ip;
    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\Shader1.xvu", 
                                           g_dwVertexDecl, &m_dwVertexShader1 ) ) )
        return E_FAIL;

    // Up the instruction pointer
    m_pd3dDevice->GetVertexShaderSize( m_dwVertexShader1, &length );
    ip += length;

    // Create the second vertex shader
    m_dwVertexShader2Address = ip;
    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\Shader2.xvu", 
                                           g_dwVertexDecl, &m_dwVertexShader2 ) ) )
        return E_FAIL;

    // Up the instruction pointer
    m_pd3dDevice->GetVertexShaderSize( m_dwVertexShader2, &length );
    ip += length;

    // Create the third vertex shader
    m_dwVertexShader3Address = ip;
    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\Shader3.xvu", 
                                           g_dwVertexDecl, &m_dwVertexShader3 ) ) )
        return E_FAIL;

    // Up the instruction pointer
    m_pd3dDevice->GetVertexShaderSize( m_dwVertexShader3, &length );
    ip += length;

    // Load the vertex shaders. It's safe to do this here, since nothing in our
    // rendering loop will "unload" them. However, if we had any calls to
    // SetVertexShader(), etc., then we would need to reload them afterwards.
    m_pd3dDevice->LoadVertexShader( m_dwVertexShader1, m_dwVertexShader1Address );
    m_pd3dDevice->LoadVertexShader( m_dwVertexShader2, m_dwVertexShader2Address );
    m_pd3dDevice->LoadVertexShader( m_dwVertexShader3, m_dwVertexShader3Address );
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Let user select a new vertex shader
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] ) 
        if( ++m_dwWhichShader > 3 )
            m_dwWhichShader = 1;

    // Build some different rotation matrices
    D3DXMATRIX matWorld1, matWorld2, matWorld3;
    D3DXMatrixRotationX( &matWorld1, 1.6f*m_fAppTime );
    D3DXMatrixRotationY( &matWorld2, 1.6f*m_fAppTime );
    D3DXMatrixRotationZ( &matWorld3, 1.6f*m_fAppTime );

    // Set our view matrix
    D3DXMATRIX matView;
    D3DXMatrixTranslation( &matView, 0.0f, 0.0f, 6.0f );

    // Calc projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 480.0f / 640.0f, 1.0f, 800.0f );

    // Build three sets of WVP matrices: one for each vertex shader
    D3DXMATRIX mat1, mat2, mat3;
    D3DXMatrixMultiply( &mat1, &matWorld1, &matView );
    D3DXMatrixMultiply( &mat1, &mat1, &matProj );
    D3DXMatrixMultiply( &mat2, &matWorld2, &matView );
    D3DXMatrixMultiply( &mat2, &mat2, &matProj );
    D3DXMatrixMultiply( &mat3, &matWorld3, &matView );
    D3DXMatrixMultiply( &mat3, &mat3, &matProj );
    
    D3DXMatrixTranspose( &mat1, &mat1 );
    D3DXMatrixTranspose( &mat2, &mat2 );
    D3DXMatrixTranspose( &mat3, &mat3 );
    
    m_pd3dDevice->SetVertexShaderConstant( 0, &mat1, 4 ); // This matrix rotates in X
    m_pd3dDevice->SetVertexShaderConstant( 4, &mat2, 4 ); // This matrix rotates in Y
    m_pd3dDevice->SetVertexShaderConstant( 8, &mat3, 4 ); // This matrix rotates in Z

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the frame buffer, Zbuffer.
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
                         0x00000000, 1.0f, 0L );

    // Set state
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_NONE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE  );

    // Select a shader. Since the vertex shaders were previously loaded with
    // LoadVertexShader(), they can now be selected with the SelectVertexShader()
    // API. Note that this replaces the conventional call to SetVertexShader().
    // Also note that SetVertexShader() only allows one vertex shader to be
    // resident at one time. Remember that the whole point of using the 
    // load/select API is to keep several shaders resident at once.
    if( 1 == m_dwWhichShader )
        m_pd3dDevice->SelectVertexShader( m_dwVertexShader1, m_dwVertexShader1Address ); 
    if( 2 == m_dwWhichShader )
        m_pd3dDevice->SelectVertexShader( m_dwVertexShader2, m_dwVertexShader2Address ); 
    if( 3 == m_dwWhichShader )
        m_pd3dDevice->SelectVertexShader( m_dwVertexShader3, m_dwVertexShader3Address ); 

    // Display the test triangle
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 1, g_Vertices, 
                                   sizeof(CUSTOMVERTEX) );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();

        m_Font.DrawText(  64, 50, 0xffffffff, L"MultiShader" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );

        if( 1 == m_dwWhichShader )
            m_Font.DrawText( 64, 75, 0xffffff00, L"Shader 1: Rotate about X" );
        if( 2 == m_dwWhichShader )
            m_Font.DrawText( 64, 75, 0xffffff00, L"Shader 2: Rotate about Y" );
        if( 3 == m_dwWhichShader )
            m_Font.DrawText( 64, 75, 0xffffff00, L"Shader 3: Rotate about Z" );
    
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}


