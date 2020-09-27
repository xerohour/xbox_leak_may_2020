//-----------------------------------------------------------------------------
// File: PushBuffer.cpp
//
// Desc: Demonstrates using static pushbuffers. Pushbuffers can be thought of
//       as display lists or instruction buffers to the GPU. Rather than
//       rendering a scene via a myriad of D3D calls each frame, the calls can
//       be recorded into a static pushpuffer. Any dynamic data within a
//       pushbuffer, such as vertex shader constants that control the rotation
//       of an object, can be modified on the fly via "fixup" objects.
//
// Hist: 03.20.01 - New for April XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XBUtil.h>




//------------------------------------------------------------------------------
// Define a triangle.
//------------------------------------------------------------------------------
struct VERTEX
{ 
    FLOAT x,y,z; 
    DWORD color; 
};

VERTEX m_Vertices[] =
{
    {-0.666f,  -1.0f, 0.0f, 0xffff0000, },
    { 0.0f,     1.0f, 0.0f, 0xff00ff00, },
    { 0.666f,  -1.0f, 0.0f, 0xff0000ff, },
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont               m_Font;        // Font for text display

    DWORD                 m_dwVertexShader;
    
    LPDIRECT3DPUSHBUFFER8 m_pPushBufer;  // The static push buffer
    DWORD                 m_dwVertexShaderConstantOffset; // Offset into the push buffer

    LPDIRECT3DFIXUP8      m_pFixups[2];  // Buffer of fixup objects
    DWORD                 m_dwFixup;     // Which fixup is being used
    DWORD                 m_dwFixupSize; // Size of the fixup object

    // The following function is called once, to record the pushbuffer
    HRESULT RecordPushBuffer();

    // The next two functions are called every frame to update the transform
    // matrices (world, view, and projection) and apply the fixups to the
    // pushbuffer.
    VOID    UpdateTransformMatrices( D3DXMATRIX* pmatWVP );
    VOID    ApplyFixups( LPDIRECT3DPUSHBUFFER8 pPushBuffer, 
                         LPDIRECT3DFIXUP8 pFixup, D3DXMATRIX* pmatWVP );

public:
    HRESULT Initialize();                // Initialize the sample
    HRESULT Render();                    // Render the scene
    HRESULT FrameMove();                 // Perform per-frame updates

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
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :CXBApplication()
{
    m_dwFixup    = 0;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create vertex shader.
    DWORD dwShaderVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),   // Position
        D3DVSD_REG( 1, D3DVSDT_D3DCOLOR ), // Diffuse color
        D3DVSD_END()
    };

    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\Shader.xvu",
                                           dwShaderVertexDecl, &m_dwVertexShader ) ) )
       return E_FAIL;

    //-------------------------------------------------------------------------
    // Create the push buffer
    //-------------------------------------------------------------------------

    // Create a push-buffer, two fix-up buffers that we'll ping-pong between,
    // and a fix-up buffer that we'll use just for determining our fix-up
    // size.
    m_pd3dDevice->CreatePushBuffer( 16 * 4096, FALSE, &m_pPushBufer );

    // Now record the push-buffer.
    RecordPushBuffer();

    //-------------------------------------------------------------------------
    // Determine the fixup size
    //-------------------------------------------------------------------------

    // Create an empty fixup object, which will be used to determine the size
    // needed for fixup objects
    LPDIRECT3DFIXUP8 pEmptyFixup;
    m_pd3dDevice->CreateFixup( 0, &pEmptyFixup );

    // Call the app-dependent function which builds the fixup objects
    D3DXMATRIX mat;
    ApplyFixups( m_pPushBufer, pEmptyFixup, &mat );
    
    // Finally, get the size of the fixup, and release it
    pEmptyFixup->GetSize( &m_dwFixupSize );
    pEmptyFixup->Release();
    
    //-------------------------------------------------------------------------
    // Create the fixup objects
    //-------------------------------------------------------------------------

    // Create two (for double-buffering) fixup objects. These objects will be
    // filled by the app-dependent ApplyFixups() function, and then used when
    // push buffer is rendered.
    m_pd3dDevice->CreateFixup( 1024, &m_pFixups[0] );
    m_pd3dDevice->CreateFixup( 1024, &m_pFixups[1] );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RecordPushBuffer()
// Desc: Records the push buffer. At any point in recording the push buffer,
//       you can call GetPushBufferOffset() to save offsets where you can later
//       fix up the data there.
//
//       Note that the following APIs are not permitted while recording a push-buffer:
//          Present()
//          InsertFence()
//          CopyRects()
//          UpdateTexture()
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RecordPushBuffer()
{
    // Note that this matrix isn't initialized, which is okay because we'll
    // always fix it (the push buffer) up before playback.
    D3DXMATRIX mat;

    // Start recording the push buffer. 
    m_pd3dDevice->BeginPushBuffer( m_pPushBufer );

    // Set some state
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    // At this point, save the push buffer offset so we know exactly where to
    // fixup the push shader before rendering it
    m_pd3dDevice->GetPushBufferOffset( &m_dwVertexShaderConstantOffset );
    m_pd3dDevice->SetVertexShaderConstant( 0, &mat, 4 );

    // Draw the vertex streamsetup in stream 0.
    m_pd3dDevice->SetVertexShader( m_dwVertexShader );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 1, m_Vertices, 
                                   sizeof(m_Vertices[0]) );

    // Stop recording the push buffer
    m_pd3dDevice->EndPushBuffer();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateTransformMatrices()
// Desc: Rotates the world, and builds the world, view, and proj matrices.
//-----------------------------------------------------------------------------
VOID CXBoxSample::UpdateTransformMatrices( D3DXMATRIX* pmatWVP )
{
    // Rotate around the Y axis
    D3DXMATRIX matWorld;
    D3DXMatrixRotationY( &matWorld, m_fAppTime );

    // Set up our view matrix.
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 0.0f,-3.0f ), 
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );

    // Set up our projection matrix.
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 480.0f/640.0f, 1.0f, 800.0f );

    // Calculate concatenated World x ( View x Projection) matrix.
    D3DXMatrixMultiply( pmatWVP, &matWorld, &matView );
    D3DXMatrixMultiply( pmatWVP, pmatWVP,   &matProj );
}




//-----------------------------------------------------------------------------
// Name: ApplyFixups()
// Desc: Fixes up the pushbuffer.
//
//       Note that when fixing up a push buffer, any of the following functions
//       can be used:
//          RunPushBuffer()
//          SetModelView()
//          SetVertexBlendModelView()
//          SetVertexShaderInput()
//          SetRenderTarget()
//          SetTexture()
//          SetPalette()
//          EndVisibilityTest()
//          SetVertexShaderConstant()
//-----------------------------------------------------------------------------
VOID CXBoxSample::ApplyFixups( LPDIRECT3DPUSHBUFFER8 pPushBuffer, 
                               LPDIRECT3DFIXUP8 pFixup, 
                               D3DXMATRIX* pmatWVP )
{
    // Begin the pushbuffer fixup
    m_pPushBufer->BeginFixup( pFixup, 0 );

    // Pass the new matrix (don't forget to transpose it) to the vertex shader.
    // The offset value used here was determined when the pushbuffer was
    // recorded.
    D3DXMATRIX matWVPT;
    D3DXMatrixTranspose( &matWVPT, pmatWVP );
    m_pPushBufer->SetVertexShaderConstant( m_dwVertexShaderConstantOffset, 0, &matWVPT, 4 );
    
    // End the pushbuffer fixup
    m_pPushBufer->EndFixup();
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    D3DXMATRIX mat;
    UpdateTransformMatrices( &mat );

    // Chheck the space of the current fixup object
    DWORD dwSpace;
    m_pFixups[m_dwFixup]->GetSpace( &dwSpace );

    // Check if we overflowed the fix-up buffer that we wanted to use. If so,
    // use the other fixup buffer
    if( dwSpace < m_dwFixupSize )
    {
        // Note: If we tried to reset the same buffer we were just using, D3D
        // would have to sit and spin until the GPU is idle, since we just
        // used that buffer.
        m_dwFixup = (m_dwFixup+1) % 2;
        m_pFixups[m_dwFixup]->Reset();
    }

    // Okay, do the fix-up for real
    ApplyFixups( m_pPushBufer, m_pFixups[m_dwFixup], &mat );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Dets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the render target and z-buffer
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
                         0xff0000ff, 1.0, 0 );

    // Actually run the push-buffer, which renders the whole scene.
    m_pd3dDevice->RunPushBuffer( m_pPushBufer, m_pFixups[m_dwFixup] );

    // Draw the app title
    m_Font.Begin();
    m_Font.DrawText(  64, 48, 0xffffffff, L"PushBuffer" );
    m_Font.DrawText( 450, 48, 0xffffff00, m_strFrameRate );
    m_Font.End();

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




