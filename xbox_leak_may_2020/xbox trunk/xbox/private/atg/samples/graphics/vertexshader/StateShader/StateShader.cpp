//-----------------------------------------------------------------------------
// File: StateShader.cpp
//
// Desc: Illustrates using vertex state shaders.
//       This example uses a shader that inverts a matrix.
//
// Hist: 01.10.00 - New for February XDK release
//       
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBUtil.h>




//-----------------------------------------------------------------------------
// Define our triangle.
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{ 
    FLOAT x,y,z; 
    DWORD color; 
};

CUSTOMVERTEX g_Vertices[] =
{
    {-0.6f,-1.0f, 0.0f, 0xffff0000, },
    { 0.0f, 1.0f, 0.0f, 0xff00ff00, },
    { 0.6f,-1.0f, 0.0f, 0xff0000ff, },
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
	CXBFont	m_Font;

    DWORD   m_dwVertexShaderHandle;
    DWORD   m_dwVertexShaderAddress;
    DWORD   m_dwVertexStateShaderHandle;
    DWORD   m_dwVertexStateShaderAddress;

	D3DXMATRIX m_matProj;
 
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

	// Calc projection matrix
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 480.0f / 640.0f, 1.0f, 800.0f );

	// Disable lighting so our diffuse colors show up
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

    DWORD dwShaderVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // Position
        D3DVSD_REG( 1, D3DVSDT_D3DCOLOR ), // Diffuse color
        D3DVSD_END()
    };

	// Create the vertex shader
	if( FAILED(XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\VertShdr.xvu", 
									      dwShaderVertexDecl, &m_dwVertexShaderHandle ) ) )
		return E_FAIL;

	// Load it into vertex shader memory at m_dwVertexShaderAddress
    DWORD ip = 0;
    UINT  length;

    m_dwVertexShaderAddress = ip;

	// get the size of the shader so we can load the state shader at the
	// following address
    if( FAILED( m_pd3dDevice->GetVertexShaderSize( m_dwVertexShaderHandle, &length ) ) )
        return E_FAIL;
    ip += length;

    // Create state shader and load it into vertex shader program memory
	// note that we pass null for the shader declaration because the
	// state shader does not use the vertex stream at all
    m_dwVertexStateShaderAddress = ip;
	if(FAILED(XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\MatInv.xvu", 
									     NULL, &m_dwVertexStateShaderHandle)))
		return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Rotate the object about its y axis
    D3DXMATRIX matWorld;
    D3DXMatrixRotationY( &matWorld, 1.6f*m_fAppTime );

	// Set our view matrix
    D3DXMATRIX matView;
	D3DXMatrixTranslation( &matView, 0.0f, 0.0f, 6.0f );

	// Set the WVP matrix in the vertex shader    
    D3DXMATRIX mat;
    D3DXMatrixMultiply( &mat, &matWorld, &matView );
    D3DXMatrixMultiply( &mat, &mat, &m_matProj );
    D3DXMatrixTranspose( &mat, &mat );
    m_pd3dDevice->SetVertexShaderConstant( 0, &mat, 4 );

	// Run the state shader twice to invert and 'revert' the display matrix
	m_pd3dDevice->LoadVertexShader( m_dwVertexStateShaderHandle, m_dwVertexStateShaderAddress );
	m_pd3dDevice->SelectVertexShader( m_dwVertexStateShaderHandle, m_dwVertexStateShaderAddress ); 
	m_pd3dDevice->RunVertexStateShader( m_dwVertexStateShaderAddress, NULL );
	m_pd3dDevice->RunVertexStateShader( m_dwVertexStateShaderAddress, NULL );

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
                         0x00404040, 1.0, 0 );

	// Restore state that font clobbers
    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Display the test triangle
	m_pd3dDevice->LoadVertexShader( m_dwVertexShaderHandle, m_dwVertexShaderAddress );
    m_pd3dDevice->SelectVertexShader( m_dwVertexShaderHandle, m_dwVertexShaderAddress ); 
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE  );
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 1, g_Vertices, 
                                   sizeof(g_Vertices[0]) );

	// Show title
	m_Font.Begin();
	m_Font.DrawText(  64, 50, 0xffffffff, L"StateShader" );
	m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
	m_Font.End();

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}


