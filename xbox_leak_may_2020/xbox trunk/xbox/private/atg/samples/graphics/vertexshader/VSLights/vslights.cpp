//-----------------------------------------------------------------------------
// File: VSLights.cpp
//
// Desc: Example code showing how to assemble vertex shaders on the fly.
//
// Hist: 03.15.00 - Changes for April XDK release
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
#define MAX_OPTIMIZATION_LEVELS 3

const DWORD MAX_LIGHTS[MAX_OPTIMIZATION_LEVELS]    =  {22,26,28};
const int   OPTIMIZATIONS[MAX_OPTIMIZATION_LEVELS] = { SASM_DONOTOPTIMIZE, SASM_DISABLE_GLOBAL_OPTIMIZATIONS, 0};




//-----------------------------------------------------------------------------
// A simple string buffer utility class
//-----------------------------------------------------------------------------
class StringBuffer 
{
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
        HRESULT hr = S_OK;
        if(size+1>m_size)
        {
            size += 4*1024;
            char* newBuf = new char[size];

            if(!newBuf)
                hr = E_OUTOFMEMORY;
            else
            {
                memcpy(newBuf,m_buf, m_used);
                delete [] m_buf;
                m_buf = newBuf;
                m_size = size;
                m_buf[m_used] = 0;
            }
        }
        return hr;
    }

    HRESULT Printf( const char *msg, ... )
    {
        HRESULT hr = S_OK;
        if(m_size - m_used < 1000)
            hr = GrowTo(m_size + 2000);

        if(SUCCEEDED(hr))
        {
            va_list list;
            va_start(list, msg );
            m_used += _vsnprintf(m_buf + m_used, m_size - m_used, msg, list);
            va_end(list);
        }
        return hr;
    }

    HRESULT Append(LPCVOID data, DWORD length)
    {
        HRESULT hr = GrowTo(m_used + length);

        if(SUCCEEDED(hr))
        {
            memcpy(m_buf + m_used, data, length);
            m_used += length;
            m_buf[m_used] = '\0';
        }
        return hr;
    }

    HRESULT Append(char c) {return Append(&c, sizeof(char));}
    HRESULT Append(const char* c) {return Append(c, strlen(c));}
    HRESULT Append(StringBuffer& buffer) 
                {return Append(buffer.GetText(), buffer.GetUsed());}
    char *GetText() {return m_buf;}
    char *GetTextEnd() {return m_buf + m_used;}
    DWORD GetUsed() {return m_used;}

    void Clear()
    {
        m_used = 0;
        if(m_buf)
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
    int        m_iOpt;                     // The current optimization number

    CXBMesh    m_Teapot;

    HRESULT CreateShader();
    HRESULT ComputeLightPositionAndColor( DWORD dwIndex, FLOAT* fLightPosition,
                                          FLOAT* pfLightDiffuse );

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
    // Allow unlimited frame rate
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    m_bDrawHelp = FALSE;
    m_dwNumLights = 4;
    m_dwShader = 0;
    m_iOpt = MAX_OPTIMIZATION_LEVELS-1;
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
    bool lightsChanged = false;
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
    {
        m_iOpt++; 
        if(m_iOpt >= MAX_OPTIMIZATION_LEVELS) 
        {
            m_iOpt = 0;
        }
        lightsChanged = true;
        if(m_dwNumLights > MAX_LIGHTS[m_iOpt]) {
            m_dwNumLights = MAX_LIGHTS[m_iOpt];
        }
    }

    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
    {
        if( m_dwNumLights < MAX_LIGHTS[m_iOpt] )
        {
            m_dwNumLights++;
            lightsChanged = true;
        }
    }
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
    {
        if( m_dwNumLights > 0 )
        {
            m_dwNumLights--;
            lightsChanged = true;
        }
    }
    if(lightsChanged)
    {
        HRESULT hr = CreateShader();
        if(FAILED(hr))
        {
            m_dwNumLights = 0;
            hr = CreateShader();
            if(FAILED(hr))
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




void HSVToRGB(float h, float s, float v, float* pRGB)
{
    float hue;
    int sector;
    float fraction, m, n;

    hue = (float)(h - floor(h))* 6.0f; // 0..6
    sector = (int)floor(hue);
    fraction = hue - sector;

    if((sector&1)== 0) // if i is even
        fraction = 1.0f - fraction;

    m = v * (1-s);  
    n = v * (1-s*fraction);

    switch (sector) 
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




HRESULT CXBoxSample::ComputeLightPositionAndColor(DWORD index, float* lightPosition, float* lightDiffuse)
{
    float u, theta, lightRadius;
    float hue, sat, value;

    u = (float)index / m_dwNumLights;
    theta = 3.14159f * 2 * u;
    lightRadius = 20.0f;

    lightPosition[0] = cosf(theta) * lightRadius; // x
    lightPosition[1] = sinf(theta) * lightRadius; // y
    lightPosition[2] = sinf(theta) * lightRadius; // z
    lightPosition[3] = 1.0;             // w

    hue = (float)index / 6.0f;
    sat = 1.0;
    value = (m_dwNumLights<5.0f) ? 1.0f : 5.0f/m_dwNumLights;

    HSVToRGB(hue, sat, value, lightDiffuse);

    lightDiffuse[3] = 1.0;              // alpha
    
    return S_OK;
}




HRESULT CXBoxSample::CreateShader()
{
    HRESULT hr = S_OK;

    // Delete the old vertex shader
    if(m_dwShader)
    {
        m_pd3dDevice->DeleteVertexShader(m_dwShader);
        m_dwShader = 0;
    }

    // Compile the new vertex shader
    StringBuffer buffer;

    hr = buffer.Initialize(1024);
    if(FAILED(hr))
        return hr;

    // Constants 0..3 - world view position matrix
    // Constant 4 - useful constants { 0.f, 0.f, 0.f, 1.f}
    const int dwLightPositionBase = 6;
    int       dwLightDiffuseBase  = dwLightPositionBase + MAX_LIGHTS[m_iOpt];

    // How many instructions in an Xbox vertex shader? It depends.
    // There are 136 slots, and shaders have an overhead of two slots,
    // which leaves 134 slots for instructions.
    // The prefix takes 5 instructions.
    // Each light macro takes 7 instructions
    // Each group of 4 lights takes 22 instructions
    // And the postfix takes 1 instruction.
    // So there should be room for about 22 lights.
    // However, the shader assembler is able to optimize the code,
    // to use fewer slots. With global or local optimizations, there
    // is room for 28 lights.

    const char* strPrefix =
        "xvs.1.1\n"
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
        "\n"
        "m4x4 oPos,v0,c0\n"
        "mov R_FINAL_COLOR,c4\n"; // Initial color - opaque black

    const char* strLightMacro =
        "addLight %d,%d\n";
    const char* strLightMacro4 =
        "addLight4 %d,%d\n";

    const char* strPostfix =
        "mov oD0,R_FINAL_COLOR\n";

    // Emit the prefix
    hr = buffer.Append( strPrefix );
    if( FAILED(hr) )
        return hr;

    DWORD light;
    for( light = 0; light < (m_dwNumLights & ~3); light += 4 ) 
    {
        hr = buffer.Printf( strLightMacro4, dwLightPositionBase + light, dwLightDiffuseBase + light );
        if( FAILED(hr) )
            return hr;
    }
    for( ; light < m_dwNumLights; light++ ) 
    {
        hr = buffer.Printf( strLightMacro, dwLightPositionBase + light, dwLightDiffuseBase + light );
        if( FAILED(hr) )
            return hr;
    }

    for(light = 0; light < m_dwNumLights; light++)
    {
        float lightPosition[4];
        float lightDiffuse[4];
        ComputeLightPositionAndColor(light, lightPosition, lightDiffuse);
        m_pd3dDevice->SetVertexShaderConstant( dwLightPositionBase + light, lightPosition, 1 );
        m_pd3dDevice->SetVertexShaderConstant( dwLightDiffuseBase + light,  lightDiffuse,  1 );
    };

    hr = buffer.Append( strPostfix );
    if( FAILED(hr) )
        return hr;

    float fConstants[4] = {0.f,0.f,0.f,1.f};
    m_pd3dDevice->SetVertexShaderConstant( 4, fConstants, 1 );

    LPXGBUFFER pUcode;
    hr = AssembleShader( "", buffer.GetText(), buffer.GetUsed(), OPTIMIZATIONS[m_iOpt], NULL,
                         &pUcode, NULL, NULL, NULL, NULL, NULL );
    if( FAILED(hr) )
        return hr;

    // Create the vertex shader
    DWORD dwDecl[] =
    {
        D3DVSD_STREAM(0),
        D3DVSD_REG( 0,  D3DVSDT_FLOAT3 ), // Position
        D3DVSD_REG( 1,  D3DVSDT_FLOAT3 ), // Normal
        D3DVSD_END()
    };

    hr = m_pd3dDevice->CreateVertexShader( dwDecl, (DWORD*)pUcode->pData,
                                           &m_dwShader, 0 );

    return hr;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L );
    
    // Draw a gradient filled background
    RenderGradientBackground( 0xff000000, 0xff0000ff );

    // Set default state
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE ); 
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE);

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
        WCHAR buf[20];
        swprintf( buf, L"Lights: %d", m_dwNumLights );
        WCHAR* strOptName[MAX_OPTIMIZATION_LEVELS] = {L"Optimizations: None", L"Optimizations: Local",L"Optimizations: Global"};

        m_Font.DrawText(  64, 50, 0xffffffff, L"VSLights" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.DrawText(  64, 80, 0xffffffff, buf );
        m_Font.DrawText(  64, 110,0xffffffff, strOptName[m_iOpt] );
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




