//-----------------------------------------------------------------------------
// File: ShaderSplicer.cpp
//
// Desc: Example code showing how to splice two vertex shaders together.
//
// Hist: 07.23.01 - Changes for August XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XBUtil.h>
#include <xgraphics.h> 


//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Rotate" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_1, L"Zoom" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, L"More Lights" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_1, L"Fewer Lights" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_1, L"Change Optimization Level" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Toggle help" },
};

#define NUM_HELP_CALLOUTS 6




//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define MAX_OPTIMIZATION_LEVELS 2

const DWORD MAX_LIGHTS[MAX_OPTIMIZATION_LEVELS]    = { 22, 28 };
const BOOL  OPTIMIZATIONS[MAX_OPTIMIZATION_LEVELS] = { FALSE, TRUE };




//-----------------------------------------------------------------------------
// A simple string buffer utility class
//-----------------------------------------------------------------------------
class StringBuffer 
{
public:
    DWORD m_size;
    CHAR* m_buf;
    DWORD m_used;

public:
    StringBuffer() { m_size = 0; m_buf = NULL; m_used = 0; }

    ~StringBuffer() { delete [] m_buf; }

    HRESULT Initialize( DWORD size )
    {
        m_used = 0;
        return GrowTo( size );
    }

    HRESULT GrowTo( DWORD size )
    {
        if( size+1 > m_size )
        {
            size += 4*1024;
            char* newBuf = new char[size];

            if( NULL == newBuf )
                return E_OUTOFMEMORY;
            else
            {
                memcpy( newBuf,m_buf, m_used );
                delete [] m_buf;
                m_buf = newBuf;
                m_size = size;
                m_buf[m_used] = 0;
            }
        }
        return S_OK;
    }

    HRESULT Printf( const char *msg, ... )
    {
        if( m_size - m_used < 1000 )
        {
            if( FAILED( GrowTo( m_size + 2000 ) ) )
                return E_OUTOFMEMORY;
        }

        va_list list;
        va_start(list, msg );
        m_used += _vsnprintf( m_buf + m_used, m_size - m_used, msg, list );
        va_end( list );
        return S_OK;
    }

    HRESULT Append( const VOID* data, DWORD length )
    {
        if( FAILED( GrowTo( m_used + length ) ) )
            return E_OUTOFMEMORY;

        memcpy( m_buf + m_used, data, length );
        m_used += length;
        m_buf[m_used] = '\0';
        return S_OK;
    }

    HRESULT Append( char c )               { return Append( &c, sizeof(char) ); }
    HRESULT Append( const char* c )        { return Append( c, strlen(c) ); }
    HRESULT Append( StringBuffer& buffer ) { return Append( buffer.GetText(), buffer.GetUsed() ); }
    char*   GetText()                      { return m_buf; }
    char*   GetTextEnd()                   { return m_buf + m_used; }
    DWORD   GetUsed()                      { return m_used; }

    void Clear()
    {
        m_used = 0;
        if( m_buf )
            m_buf[0] = 0;
    }
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont    m_Font;                     // Font class
    CXBHelp    m_Help;                     // Help class
    BOOL       m_bDrawHelp;                // Whether to draw help

    D3DXMATRIX m_matProj;

    DWORD      m_dwNumLights;              // Num lights to render
    DWORD      m_dwShader;                 // The current shader
    DWORD      m_dwOptimizationLevel;      // The current optimization number

    CXBMesh    m_Teapot;

    LPXGBUFFER m_pHeader;                  // Buffers for assembling the shaders
    LPXGBUFFER m_pFooter;
    LPXGBUFFER m_pLight4[7];
    LPXGBUFFER m_pLight1[28];

    HRESULT CreateShader();
    HRESULT ComputeLightPositionAndColor( DWORD dwIndex, FLOAT* fLightPosition,
                                          FLOAT* pfLightDiffuse );

public:
    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();
    HRESULT SetLights();
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
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    m_bDrawHelp           = FALSE;
    m_dwNumLights         = 4;
    m_dwShader            = 0;
    m_dwOptimizationLevel = MAX_OPTIMIZATION_LEVELS-1;

    ZeroMemory( &m_pLight4, sizeof(m_pLight4) );
    ZeroMemory( &m_pLight1, sizeof(m_pLight1) );
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

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set up proj matrix
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 200.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    // Create a mesh (vertex and index buffers) for the teapot
    if( FAILED( m_Teapot.Create( m_pd3dDevice, "Models\\Teapot.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set up a shader
    HRESULT hr = CreateShader();
    if( FAILED(hr) )
        return hr;

    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

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

    // Add or remove lights
    BOOL bLightsChanged = FALSE;

    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
    {
        if( ++m_dwOptimizationLevel >= MAX_OPTIMIZATION_LEVELS) 
            m_dwOptimizationLevel = 0;

        bLightsChanged = TRUE;
        
        if( m_dwNumLights > MAX_LIGHTS[m_dwOptimizationLevel] ) 
            m_dwNumLights = MAX_LIGHTS[m_dwOptimizationLevel];
    }

    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
    {
        if( m_dwNumLights < MAX_LIGHTS[m_dwOptimizationLevel] )
        {
            m_dwNumLights++;
            bLightsChanged = TRUE;
        }
    }
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
    {
        if( m_dwNumLights > 0 )
        {
            m_dwNumLights--;
            bLightsChanged = TRUE;
        }
    }
    if( bLightsChanged )
    {
        HRESULT hr = SetLights();
        if( FAILED(hr) )
        {
            m_dwNumLights = 0;
            hr = SetLights();
            if( FAILED(hr) )
                return hr;
        }
    }

    // Set up view matrix
    static D3DXVECTOR3 vEyePt( 0,0,-6 );
    if( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP )
        vEyePt.z += 10.0f*m_fElapsedTime;
    if( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN )
        vEyePt.z -= 10.0f*m_fElapsedTime;

    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,1,0) );

    // Setup object matrix
    D3DXMATRIX matRotate;
    FLOAT fXRotate = m_DefaultGamepad.fX1*D3DX_PI*m_fElapsedTime;
    FLOAT fYRotate = m_DefaultGamepad.fY1*D3DX_PI*m_fElapsedTime;
    D3DXMatrixRotationYawPitchRoll( &matRotate, -fXRotate, -fYRotate, 0.0f );
    
    static D3DXMATRIX  matWorld( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
    matWorld._41 = matWorld._42 = 0.f;
    D3DXMatrixMultiply( &matWorld, &matWorld, &matRotate );

    // Set the WVP matrix in the vertex shader    
    D3DXMATRIX mat;
    D3DXMatrixMultiply( &mat, &matWorld, &matView );
    D3DXMatrixMultiply( &mat, &mat, &m_matProj );
    D3DXMatrixTranspose( &mat, &mat );
    m_pd3dDevice->SetVertexShaderConstant( 0, &mat, 4 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: HSVToRGB()
// Desc: 
//-----------------------------------------------------------------------------
VOID HSVToRGB( FLOAT h, FLOAT s, FLOAT v, FLOAT* pRGB )
{
    FLOAT hue      = (FLOAT)(h - floor(h))* 6.0f; // 0..6
    INT   sector   = (INT)floor(hue);
    FLOAT fraction = hue - sector;

    if( (sector&1)== 0 ) // if i is even
        fraction = 1.0f - fraction;

    FLOAT m = v * (1-s);  
    FLOAT n = v * (1-s*fraction);

    switch( sector ) 
    {  
        case 6:  
        case 0: pRGB[0] = v; pRGB[1] = n; pRGB[2] = m; break;  
        case 1: pRGB[0] = n; pRGB[1] = v; pRGB[2] = m; break;  
        case 2: pRGB[0] = m; pRGB[1] = v; pRGB[2] = n; break;  
        case 3: pRGB[0] = m; pRGB[1] = n; pRGB[2] = v; break;  
        case 4: pRGB[0] = n; pRGB[1] = m; pRGB[2] = v; break;  
        case 5: pRGB[0] = v; pRGB[1] = m; pRGB[2] = n; break;  
    }
}




//-----------------------------------------------------------------------------
// Name: ComputeLightPositionAndColor()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::ComputeLightPositionAndColor( DWORD  index, 
                                                   FLOAT* vLightPosition, 
                                                   FLOAT* vLightDiffuse )
{
    FLOAT u = (FLOAT)index / m_dwNumLights;
    FLOAT fTheta = D3DX_PI * 2 * u;
    FLOAT fLightRadius = 20.0f;

    vLightPosition[0] = cosf(fTheta) * fLightRadius; // x
    vLightPosition[1] = sinf(fTheta) * fLightRadius; // y
    vLightPosition[2] = sinf(fTheta) * fLightRadius; // z
    vLightPosition[3] = 1.0f;                        // w

    FLOAT hue   = (FLOAT)index / 6.0f;
    FLOAT sat   = 1.0f;
    FLOAT value = (m_dwNumLights<5.0f) ? 1.0f : 5.0f/m_dwNumLights;

    HSVToRGB( hue, sat, value, vLightDiffuse );
    
    vLightDiffuse[3] = 1.0f; // Alpha
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateShader()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateShader()
{
    HRESULT hr;

    // Delete the old vertex shader
    if( m_dwShader )
    {
        m_pd3dDevice->DeleteVertexShader( m_dwShader );
        m_dwShader = 0;
    }

    // Compile the new vertex shader
    StringBuffer buffer;
    hr = buffer.Initialize( 1024 );
    if( FAILED(hr) )
        return hr;

    // How many instructions in an Xbox vertex shader? It depends. There are
    // 136 slots, and shaders have an overhead of two slots, which leaves 134
    // slots for instructions. The prefix takes 5 instructions. Each light
    // macro takes 7 instructions. Each group of 4 lights takes 22
    // instructions. And the postfix takes 1 instruction. So there should be
    // room for about 22 lights. However, the shader assembler is able to
    // optimize the code, to use fewer slots. With global or local
    // optimizations, there is room for 28 lights.

    const char* strPrefix =
        "xvs.1.1\n"
        "#pragma screenspace\n"
        "\n"
        "#define R_LIGHT_COEF r8\n"
        "#define R_VERTEX_TO_LIGHT r9\n"
        "#define R_FINAL_COLOR r11\n"
        "#define XYZW_LIGHT_N_DOT_L x\n"
        "#define XYZW_V_TO_L_DIST w\n"
        "#define ZERO c4.x\n"
        "\n"
        "macro addLight Position, Diffuse\n"
        "add R_VERTEX_TO_LIGHT, c[%Position],-v0\n"
        "dp3 R_VERTEX_TO_LIGHT.XYZW_V_TO_L_DIST, R_VERTEX_TO_LIGHT, R_VERTEX_TO_LIGHT\n"
        "rsq R_VERTEX_TO_LIGHT.XYZW_V_TO_L_DIST, R_VERTEX_TO_LIGHT.XYZW_V_TO_L_DIST\n"
        "dp3 R_LIGHT_COEF.XYZW_LIGHT_N_DOT_L, v1, R_VERTEX_TO_LIGHT\n"
        "max R_LIGHT_COEF.XYZW_LIGHT_N_DOT_L, R_LIGHT_COEF.XYZW_LIGHT_N_DOT_L, ZERO\n"
        "mul R_LIGHT_COEF.XYZW_LIGHT_N_DOT_L, R_LIGHT_COEF.XYZW_LIGHT_N_DOT_L, R_VERTEX_TO_LIGHT.XYZW_V_TO_L_DIST\n"
        "mad R_FINAL_COLOR, R_LIGHT_COEF.XYZW_LIGHT_N_DOT_L, c[%Diffuse], R_FINAL_COLOR\n"
        "endm\n"
        "\n"
        "macro addLight4 Position, Diffuse\n"
        "add r9,		c[%Position],					-v0\n"
        "dp3 r9.w,	r9,		r9\n"
        "rsq r1.w,	r9.w\n"
        "dp3 r2.w,	v1,		r9\n"

        "add r7,		c[%inc(%Position)],				-v0\n"
        "dp3 r7.w,	r7,		r7\n"
        "rsq r1.z,	r7.w\n"
        "dp3 r2.z,	v1,		r7\n"

        "add r5,		c[%inc(%inc(%Position))],		-v0\n"
        "dp3 r5.w,	r5,		r5\n"
        "rsq r1.y,	r5.w\n"
        "dp3 r2.y,	v1,		r5\n"

        "add r3,		c[%inc(%inc(%inc(%Position)))],	-v0\n"
        "dp3 r3.w,	r3,		r3\n"
        "rsq r1.x,	r3.w\n"
        "dp3 r2.x,	v1,		r3\n"
        "\n"
        "max r2.xyzw,r2.xyzw,c4.xxxx\n"
        "mul r2.xyzw,r2.xyzw,r1.xyzw\n"
        "\n"
        "mad R_FINAL_COLOR,	r2.w,	c[%Diffuse],					R_FINAL_COLOR\n"
        "mad R_FINAL_COLOR,	r2.z,	c[%inc(%Diffuse)],				R_FINAL_COLOR\n"
        "mad R_FINAL_COLOR,	r2.y,	c[%inc(%inc(%Diffuse))],		R_FINAL_COLOR\n"
        "mad R_FINAL_COLOR,	r2.x,	c[%inc(%inc(%inc(%Diffuse)))],	R_FINAL_COLOR\n"
        "endm\n"
        "\n";

    const char* strHeaderString = 
        "xvs.1.1\n"
        "#pragma screenspace\n"
        "#define R_FINAL_COLOR r11\n"
        "m4x4 oPos,v0,c0\n"
        "mov R_FINAL_COLOR,c4\n";

    const char* strLightMacro =
        "addLight %d,%d\n";
    const char* strLightMacro4 =
        "addLight4 %d,%d\n";
 
    const char* strPostfix =
        "xvs.1.1\n"
        "#define R_FINAL_COLOR r11\n"
        "mov oD0,R_FINAL_COLOR\n";

    // Emit the prefix
    const int dwLightPositionBase = 6;
    int       dwLightDiffuseBase  = dwLightPositionBase + 32;

    // Do single-lights first
    for( DWORD light = 0; light < 28; light++ )
    {
        buffer.Clear();
        buffer.Append( strPrefix );
        buffer.Printf( strLightMacro, dwLightPositionBase + light, dwLightDiffuseBase + light );
        
        hr = AssembleShader( "", buffer.GetText(), buffer.GetUsed(), SASM_DONOTOPTIMIZE | SASM_SKIPVALIDATION,
                             NULL, &m_pLight1[light], NULL, NULL, NULL, NULL, NULL );
    }

    for( light = 0; light < 28; light += 4 )
    {
        buffer.Clear();
        buffer.Append( strPrefix );
        buffer.Printf( strLightMacro4, dwLightPositionBase + light, dwLightDiffuseBase + light );
        hr = AssembleShader( "", buffer.GetText(), buffer.GetUsed(), SASM_DONOTOPTIMIZE | SASM_SKIPVALIDATION,
                             NULL, &m_pLight4[light >> 2], NULL, NULL, NULL, NULL, NULL);
    }

    buffer.Clear();
    buffer.Printf( strPostfix );
    AssembleShader( "", buffer.GetText(), buffer.GetUsed(), SASM_DONOTOPTIMIZE | SASM_SKIPVALIDATION,
                    NULL, &m_pFooter, NULL, NULL, NULL, NULL, NULL);

    buffer.Clear();
    buffer.Printf( strHeaderString );
    AssembleShader( "", buffer.GetText(), buffer.GetUsed(), SASM_DONOTOPTIMIZE | SASM_SKIPVALIDATION,
                    NULL, &m_pHeader, NULL, NULL, NULL, NULL, NULL);

    SetLights();

    return hr;
}




//-----------------------------------------------------------------------------
// Name: SetLights()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::SetLights()
{
    HRESULT hr; 

    // Set the vertex shader constants as:
    //    c0..3 = World view position matrix
    //    c4    = Useful constants { 0.0f, 0.0f, 0.0f, 1.0f}
    const int dwLightPositionBase = 6;
    int       dwLightDiffuseBase  = dwLightPositionBase + 32;

    for( DWORD light = 0; light < m_dwNumLights; light++ )
    {
        float afLightPosition[4];
        float afLightDiffuse[4];
        ComputeLightPositionAndColor( light, afLightPosition, afLightDiffuse );
        m_pd3dDevice->SetVertexShaderConstant( dwLightPositionBase + light, afLightPosition, 1 );
        m_pd3dDevice->SetVertexShaderConstant( dwLightDiffuseBase + light,  afLightDiffuse,  1 );
    };

    float afConstants[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    m_pd3dDevice->SetVertexShaderConstant( 4, afConstants, 1 );

    // Splice the vertex shaders together
    DWORD  i;
    int    index=0;
    DWORD* shaderarray[32] = {0};

    shaderarray[index++] = (DWORD*)m_pHeader->pData;
    for( i = 0; i < (m_dwNumLights & ~3); i += 4 )
    {
        shaderarray[index++] = (DWORD*)m_pLight4[i >> 2]->pData;
    }
    for( ; i < m_dwNumLights; i++ ) 
    {
        shaderarray[index++] = (DWORD*)m_pLight1[i]->pData;
    }
    shaderarray[index++] = (DWORD*)m_pFooter->pData;

    // Get the size needed to splice the vertex shaders together
    DWORD size = 0;
    XGSpliceVertexShaders( NULL, &size, NULL, shaderarray, index,
                           OPTIMIZATIONS[m_dwOptimizationLevel] );

    // Splice the shaders together
    DWORD* pSplicedVertexShaderData = new DWORD[size/4];
    XGSpliceVertexShaders( pSplicedVertexShaderData, NULL, NULL, shaderarray, index,
                           OPTIMIZATIONS[m_dwOptimizationLevel] );

    // Create the vertex shader
    DWORD dwDecl[] =
    {
        D3DVSD_STREAM(0),
        D3DVSD_REG( 0,  D3DVSDT_FLOAT3 ), // Position
        D3DVSD_REG( 1,  D3DVSDT_FLOAT3 ), // Normal
        D3DVSD_END()
    };

    hr = m_pd3dDevice->CreateVertexShader( dwDecl, pSplicedVertexShaderData, &m_dwShader, 0 );
    delete[] pSplicedVertexShaderData;

    return hr;
}

 
 

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x00000000, 1.0f, 0L );
    
    // Draw a gradient filled background
    RenderGradientBackground( 0xff000000, 0xff0000ff );

    // Set default state
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE ); 
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         FALSE );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );

    // Draw the teapot
    m_pd3dDevice->SetVertexShader( m_dwShader );
    m_Teapot.Render( m_pd3dDevice, XBMESH_NOFVF|XBMESH_NOMATERIALS|XBMESH_NOTEXTURES );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        WCHAR strBuffer[20];
        swprintf( strBuffer, L"Lights: %d", m_dwNumLights );
        WCHAR* strOptName[MAX_OPTIMIZATION_LEVELS] = { L"Optimizations: Off", L"Optimizations: On" };

        m_Font.DrawText(  64, 50, 0xffffffff, L"ShaderSplicer" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.DrawText(  64, 80, 0xffffffff, strBuffer );
        m_Font.DrawText(  64, 110,0xffffffff, strOptName[m_dwOptimizationLevel] );

    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

 


