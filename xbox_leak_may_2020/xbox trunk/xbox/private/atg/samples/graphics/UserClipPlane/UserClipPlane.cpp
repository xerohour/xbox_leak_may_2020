//-----------------------------------------------------------------------------
// File: UserClipPlane.cpp
//
// Desc: Demonstrates how to use pixel shaders to create user-defined clip
//       planes
//
// Hist: 4.9.01 - Created
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <xgraphics.h>




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_2, L"Pause" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Enable/\nDisable" },
    { XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Next plane" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Rotate\nplane X/Z" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Rotate\nplane Y and\nmove" },
};

#define NUM_HELP_CALLOUTS 6

D3DXMATRIX g_matView, g_matProj;

#define NUM_PLANES 4

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBoxSample();

    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();

    HRESULT InitVertexShader();
    HRESULT InitPixelShader();
    HRESULT DrawPlanes();

    BOOL        m_bDrawHelp;        // True if we should draw help
    CXBFont     m_Font;             // Font object
    CXBHelp     m_Help;             // Help object

    CXBMesh     m_Model;            // Teapot model

    DWORD       m_dwPixelShader;    // Pixel shader
    DWORD       m_dwVertexShader;   // Vertex shader


    DWORD       m_dwCurrentPlane;               // Selected plane
    D3DXVECTOR3 m_avNormals[ NUM_PLANES ];      // Plane normals
    D3DXVECTOR3 m_avIntercepts[ NUM_PLANES ];   // Plane intercepts
    BOOL        m_abActive[ NUM_PLANES];        // Enabled planes
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
    m_bDrawHelp = FALSE;
    m_dwCurrentPlane = 0;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( m_Model.Create( m_pd3dDevice, "Models\\Teapot.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Set projection transform
    D3DXMatrixPerspectiveFovLH( &g_matProj, D3DX_PI/4, 640.0f/480.0f, 0.1f, 100.0 );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_matProj );

    // Set view position
    D3DXVECTOR3 vCameraPos( 0.0f, 2.0f, -5.0f );
    D3DXVECTOR3 vLookAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &g_matView, &vCameraPos, &vLookAt, &vUp );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &g_matView );

    // Create our shaders
    InitVertexShader();
    InitPixelShader();

    //
    // Initialize our clip planes:
    // * Normal points in the positive Y direction
    // * Intercept is at the origin
    // * Only plane 0 is active
    //
    for( int i = 0; i < NUM_PLANES; i++ )
    {
        m_avNormals[i]      = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
        m_avIntercepts[i]   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
        m_abActive[i]       = ( i == 0 );
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Toggle frame on/off
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
        m_abActive[ m_dwCurrentPlane ] = !m_abActive[ m_dwCurrentPlane ];
    }

    // Cycle through frames
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_B ] )
    {
        m_dwCurrentPlane = ( m_dwCurrentPlane + 1 ) % NUM_PLANES;
    }

    // If frame is active, allow it to be manipulated
    if( m_abActive[ m_dwCurrentPlane ] )
    {
        FLOAT fRotX = m_DefaultGamepad.fY1 * m_fElapsedTime;
        FLOAT fRotY = m_DefaultGamepad.fX2 * m_fElapsedTime;
        FLOAT fRotZ = m_DefaultGamepad.fX1 * m_fElapsedTime;
        D3DXMATRIX matTransformNormal;

        D3DXMatrixRotationYawPitchRoll( &matTransformNormal, fRotY, fRotX, -fRotZ );
        D3DXVec3TransformNormal( &m_avNormals[ m_dwCurrentPlane ], &m_avNormals[ m_dwCurrentPlane ], &matTransformNormal );

        m_avIntercepts[ m_dwCurrentPlane ] += m_avNormals[ m_dwCurrentPlane ] * m_DefaultGamepad.fY2 * m_fElapsedTime;
    }

    // Set up our clip planes
    D3DXMATRIX matClipPlanes;
    ZeroMemory( &matClipPlanes, sizeof( D3DXMATRIX ) );
    for( int i = 0; i < NUM_PLANES; i++ )
    {
        if( m_abActive[ i ] )
        {
            matClipPlanes( i, 0 ) = m_avNormals[ i ].x;
            matClipPlanes( i, 1 ) = m_avNormals[ i ].y;
            matClipPlanes( i, 2 ) = m_avNormals[ i ].z;
            matClipPlanes( i, 3 ) = -D3DXVec3Dot( &m_avNormals[ i ], 
                                                  &m_avIntercepts[ i ] );
        }
    }

    m_pd3dDevice->SetVertexShaderConstant( 8, &matClipPlanes, 4 );

	D3DXMATRIX matWorld, m;
	static float fRot = 0.0f;

	// Rotate teapot
	fRot += 1.57f*m_fElapsedAppTime;
	D3DXMatrixRotationY(&matWorld, fRot);
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set WVP and World transforms for vertex shader
    D3DXMATRIX matWVP;
    D3DXMatrixMultiply( &matWVP, &g_matView, &g_matProj );
    D3DXMatrixMultiply( &matWVP, &matWorld, &matWVP );
    D3DXMatrixTranspose( &matWVP, &matWVP );
    m_pd3dDevice->SetVertexShaderConstant( 0, &matWVP, 4 );
    D3DXMatrixTranspose( &matWorld, &matWorld );
    m_pd3dDevice->SetVertexShaderConstant( 4, &matWorld, 4 );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    // Set default state
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE ); 
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         FALSE);
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_NONE );

    // Render the object using the clip planes
    m_pd3dDevice->SetVertexShader( m_dwVertexShader );
    m_pd3dDevice->SetPixelShader( m_dwPixelShader );
    m_pd3dDevice->SetRenderState( D3DRS_PSCONSTANT0_0, 0xFFFFFF00 );
    m_Model.Render( m_pd3dDevice, XBMESH_NOFVF | XBMESH_NOMATERIALS | XBMESH_NOTEXTURES );
    m_pd3dDevice->SetPixelShader( NULL );

    // Show the active clipping planes
    DrawPlanes();

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        WCHAR sz[100];

        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"UserClipPlane" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        swprintf( sz, L"Plane %d of %d (%s)", m_dwCurrentPlane + 1, 
                                              NUM_PLANES, 
                                              m_abActive[ m_dwCurrentPlane ] ? L"Enabled" : L"Disabled" );
        m_Font.DrawText( 64, 80, 0xffffff00, sz );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitVertexShader
// Desc: Initializes and creates our vertex shader.  Also sets up vertex
//       shader constants that don't change:
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::InitVertexShader()
{
    // Setup the vertex declaration
    DWORD dwDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // v0 = Position
        D3DVSD_REG( 2, D3DVSDT_FLOAT3 ), // v2 = Normal
        D3DVSD_END()
    };

    // Create the vertex shader
    XBUtil_CreateVertexShader( m_pd3dDevice, "shaders\\UserClipPlane.xvu", dwDecl, &m_dwVertexShader );

    D3DXVECTOR4 vLightDir( 0.7f, 0.7f, -0.14f, 0.0f );
    m_pd3dDevice->SetVertexShaderConstant( 13, &vLightDir, 1 );
    D3DXVECTOR3 vColor( 1.0f, 0.7f, 0.0f );
    m_pd3dDevice->SetVertexShaderConstant( 14, &vColor, 1 );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: InitPixelShader
// Desc: Creates our pixel shader via D3DPIXELSHADERDEF.
//       Texture stage 0 performs a standard 2d projection
//       Texture stage 1 performs the clip plane operation
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::InitPixelShader()
{
    D3DPIXELSHADERDEF psd;

    ZeroMemory( &psd, sizeof( D3DPIXELSHADERDEF ) );
    psd.PSCombinerCount = PS_COMBINERCOUNT( 1, 0 );
    psd.PSTextureModes  = PS_TEXTUREMODES( PS_TEXTUREMODES_CLIPPLANE,  // Clip Plane(s)
                                           PS_TEXTUREMODES_NONE,       // Not used
                                           PS_TEXTUREMODES_NONE,       // Not used
                                           PS_TEXTUREMODES_NONE );     // Not used

    psd.PSInputTexture  = PS_INPUTTEXTURE( 0, 0, 0, 0 );
    psd.PSDotMapping    = PS_DOTMAPPING( 0,
                                         0,
                                         0,
                                         0);
    psd.PSCompareMode   = PS_COMPAREMODE( 
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT );

    //--------------------------
    // Stage 0 
    //--------------------------

    // A=T0.rgb, B=V0, C=0, D=0 (so that AB.rgb = C0 and CD = 0 )
    psd.PSRGBInputs[0]    = PS_COMBINERINPUTS(
        PS_REGISTER_V0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY );
    
    // A=0, B=0, C=0, D=0 (so that AB = 0 and CD = 0 )
    psd.PSAlphaInputs[0]  = PS_COMBINERINPUTS(
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY );

    // R0.rgb = AB
    psd.PSRGBOutputs[0]   = PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );
    
    // R0.a = AB
    psd.PSAlphaOutputs[0] = PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM );
    
    psd.PSConstant0[0]    = 0x00000000;
    psd.PSConstant1[0]    = 0x00000000;

    //--------------------------
    // Final combiner
    //--------------------------
    psd.PSFinalCombinerInputsABCD=PS_COMBINERINPUTS(
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_R0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_R0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY );

    // E = 0, F = 0, G = 1. (From above, EF is not used. G is alpha and is set to 1.)
    psd.PSFinalCombinerInputsEFG=PS_COMBINERINPUTS(
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        0 | 0 | 0 );
    psd.PSC0Mapping = 0x00000000;
    psd.PSC1Mapping = 0x00000000;

    if( FAILED( m_pd3dDevice->CreatePixelShader( &psd, &m_dwPixelShader ) ) )
    {
        OUTPUT_DEBUG_STRING( "Couldn't create pixel shader.\n" );
        return E_FAIL;
    }

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: DrawPlanes
// Desc: Draws the active clipping planes and normals
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::DrawPlanes()
{
    typedef struct
    {
        D3DXVECTOR3 p;
        D3DCOLOR c;
    } PLANE_VERTEX;
#define FVF_PLANE_VERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

    // Set up our rendering state
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ); 
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    m_pd3dDevice->SetVertexShader( FVF_PLANE_VERTEX );
    m_pd3dDevice->SetPixelShader( NULL );
    
    // Loop over each plane, drawing it if it's active
    for( int i = 0; i < NUM_PLANES; i++ )
    {
        PLANE_VERTEX apvNormalLine[2];
        PLANE_VERTEX apvPlane[4];

        const DWORD colors[ NUM_PLANES ] =
        {
            0x00FF0000,
            0x0000FF00,
            0x000000FF,
            0x00646464,
        };

        if( m_abActive[i] )
        {
            DWORD dwAlpha = i == m_dwCurrentPlane ? 0x40000000 : 0x20000000;

            //
            // Create the normal vector
            //
            apvNormalLine[0].p = m_avIntercepts[i];
            apvNormalLine[0].c = 0xFF000000 | colors[i];
            apvNormalLine[1].p = m_avIntercepts[i] + m_avNormals[i];
            apvNormalLine[1].c = 0xFF000000 | colors[i];

            //
            // Draw the normal vector
            //
            D3DXMATRIX matIdentity;
            D3DXMatrixIdentity( &matIdentity );
            m_pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
            m_pd3dDevice->DrawVerticesUP( D3DPT_LINELIST, 2, apvNormalLine, sizeof( PLANE_VERTEX ) );

            //
            // Create the plane:
            // Since we're going to use the cross-product of the quad
            // normal and the plane normal as our rotation axis, we
            // want to make sure they're not close to parallel.  So
            // we face the quad into x or y depending on which is
            // less represented in the plane normal
            // Note that this will give some artifacts when we switch
            // from one to the other, but the plane quads are just a
            // visual aid.
            //
            for( int j = 0; j < 4; j++ )
                apvPlane[j].c = dwAlpha | colors[i];

            D3DXVECTOR3 vQuadNorm;
            if( fabs( m_avNormals[i].x ) > fabs( m_avNormals[i].y ) )
            {
                // X has greater magnitude, so face quad in y
                vQuadNorm = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
                apvPlane[0].p = D3DXVECTOR3( -2.0f, 0.0f, -2.0f );
                apvPlane[1].p = D3DXVECTOR3( -2.0f, 0.0f, 2.0f );
                apvPlane[2].p = D3DXVECTOR3(  2.0f, 0.0f, -2.0f );
                apvPlane[3].p = D3DXVECTOR3(  2.0f, 0.0f, 2.0f );
            }
            else
            {
                // Y has greater magnitude, so face quad in x
                vQuadNorm = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
                apvPlane[0].p = D3DXVECTOR3( 0.0f, -2.0f, -2.0f );
                apvPlane[1].p = D3DXVECTOR3( 0.0f, -2.0f, 2.0f );
                apvPlane[2].p = D3DXVECTOR3( 0.0f,  2.0f, -2.0f );
                apvPlane[3].p = D3DXVECTOR3( 0.0f,  2.0f, 2.0f );
            }

            // 
            // Now, calculate the cross product of the quad normal
            // and plane normal to get our rotation axis.
            // Then, take the dot product to get the angle between
            // them.  Set up a rotation matrix to rotate that much
            // around the axis, then translate to the intercept
            // point.
            //
            D3DXVECTOR3 vAxis;
            D3DXMATRIX matRotation, matTranslation, matWorld;
            FLOAT fDot;
            FLOAT fAngle;

            // Calculate rotation axis
            D3DXVec3Cross( &vAxis, &vQuadNorm, &m_avNormals[i] );
            D3DXVec3Normalize( &vAxis, &vAxis );

            // Calculate angle
            fDot = D3DXVec3Dot( &vQuadNorm, &m_avNormals[i] );
            fAngle = (FLOAT)acos( fDot );

            // Set up transform matrix
            D3DXMatrixRotationAxis( &matRotation, &vAxis, fAngle );
            D3DXMatrixTranslation( &matTranslation, m_avIntercepts[i].x, m_avIntercepts[i].y, m_avIntercepts[i].z );
            D3DXMatrixMultiply( &matWorld, &matRotation, &matTranslation );
            m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
            m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

            // Draw the plane
            m_pd3dDevice->DrawVerticesUP( D3DPT_TRIANGLESTRIP, 4, apvPlane, sizeof( PLANE_VERTEX ) );
        }
    }

    return S_OK;
}
