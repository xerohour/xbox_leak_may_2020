//-----------------------------------------------------------------------------
// File: Explosion.cpp
//
// Desc: Demonstrates a way of using ZSprites (see ZSprite sample) to create
//       an explosion effect.  Typical billboarded explosions suffer from 
//       artifacts when intersecting with objects - because the billboard is
//       a simple quad it intersects in a line.  Using ZSprites to give
//       depth to these billboards, the intersection appears more correct
//
// Hist: 04.03.01 - New for May XDK
//       04.09.01 - Added bundling of resources
//       04.30.01 - Updated comments, added texture coord transformation
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XBResource.h>
#include <XBUtil.h>
#include <xgraphics.h>
#include "xbterrain.h"

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
    { "Ground.bmp",  resource_Ground_OFFSET },
    { "Exp1.tga",    resource_Exp1_OFFSET },
    { "Exp1d.bmp",   resource_Exp1d_OFFSET },
    { "Exp2.tga",    resource_Exp2_OFFSET },
    { "Exp2d.bmp",   resource_Exp2d_OFFSET },
    { "Exp3.tga",    resource_Exp3_OFFSET },
    { "Exp3d.bmp",   resource_Exp3d_OFFSET },
    { NULL, 0 },
};




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Move camera" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Zoom" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle quad\nwireframes" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle ZSprite/\nBillboard" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};

#define NUM_HELP_CALLOUTS 6


//-----------------------------------------------------------------------------
// Name: fHeightFn
// Desc: Height field function for our terrain
//-----------------------------------------------------------------------------
FLOAT __stdcall fHeightFn( FLOAT fX, FLOAT fZ )
{
    return 1.5f * ( cosf( fX / 4 + 0.2f ) * cosf( fZ / 3 - 0.2f ) - 0.5f );
}


//-----------------------------------------------------------------------------
// Externals and globals
//-----------------------------------------------------------------------------

class CExplosion;
#define MAX_EXPLOSIONS 150

D3DXVECTOR3 g_vEyePos;              // Global viewer position
D3DXVECTOR3 g_vViewAngle;           // View angle of camera
D3DXMATRIX  g_matView;              // View transform
D3DXMATRIX  g_matProj;              // Projection transform
BOOL        g_bWireFrame = FALSE;   // Render wireframe quads
BOOL        g_bZSprite = TRUE;      // Render Zsprite or billboard




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

    D3DXVECTOR3        m_vCameraPos;         // Camera position, untransformed

    LPDIRECT3DTEXTURE8 m_pGroundTex;
    LPDIRECT3DTEXTURE8 m_pExp[3];
    LPDIRECT3DTEXTURE8 m_pExpD[3];

    VOID    SetViewTransform();         // Sets the view transform

    CXBTerrain m_Terrain;

    CExplosion * m_apExplosion[MAX_EXPLOSIONS];

public:
    HRESULT Initialize();       // Initialize the sample
    HRESULT Render();           // Render the scene
    HRESULT FrameMove();        // Perform per-frame updates

    CXBoxSample();
};




//-----------------------------------------------------------------------------
// Name: class CExplosion
// Desc: Contains data for one instance of an explosion.
//-----------------------------------------------------------------------------
class CExplosion
{
public:
    CExplosion( LPDIRECT3DDEVICE8 pDevice,
                LPDIRECT3DTEXTURE8 pExplosionImage, 
                LPDIRECT3DTEXTURE8 pExplosionDepth,
                D3DXVECTOR3 vCenter,
                FLOAT fMaxRadius,
                FLOAT fDuration );
    ~CExplosion();
    HRESULT Render();

    HRESULT InitPixelShader();          // Initialize and create pixel shader
    HRESULT InitVertexShader();         // Initialize and create vertex shader

    // Member data
    LPDIRECT3DDEVICE8 m_pd3dDevice;     // Device to render to

    LPDIRECT3DTEXTURE8 m_pImage;        // Image texture
    LPDIRECT3DTEXTURE8 m_pDepth;        // Depth texture

    D3DXVECTOR3 m_vPos;                 // Center of explosion
    D3DXVECTOR3 m_vCorrectedPos;        // Center of quad 
    FLOAT m_fCurrentRadius;             // Current radius of explosion
    FLOAT m_fMaxRadius;                 // Maximum size
    FLOAT m_fDuration;                  // Duration of explosion
    FLOAT m_fStartTime;                 // When explosion began
    FLOAT m_fAlpha;                     // Fade-out alpha of explosion
    D3DXVECTOR4 m_vTransform1;          // Texture coords transform
    D3DXVECTOR4 m_vTransform2;          // (2 x 2) matrix

    static LPDIRECT3DVERTEXBUFFER8 m_pvbQuad;   // Pointer to quad VB
    static DWORD    m_dwPixelShader;            // Pixel shader handle
    static DWORD    m_dwVertexShader;           // Vertex shader handle


    typedef struct 
    {
        D3DXVECTOR3 p;
        D3DXVECTOR2 t;
    } EXPLOSION_QUAD_VERTEX;
#define FVF_EXPLOSION_QUAD_VERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

};
LPDIRECT3DVERTEXBUFFER8 CExplosion::m_pvbQuad = NULL;
DWORD CExplosion::m_dwPixelShader = 0;
DWORD CExplosion::m_dwVertexShader = 0;




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
    m_bDrawHelp  = FALSE;
    memset( m_apExplosion, 0, MAX_EXPLOSIONS * sizeof(void *) );
}




//-----------------------------------------------------------------------------
// Name: InitPixelShader
// Desc: Sets up our hard-coded pixel shader
//
// Texture Stage 0: 2-D texture lookup of explosion image
// Texture Stage 1: 2-D texture lookup into depth texture
// Texture Stage 2: DotProduct for numerator of Z / W.  The texture coords
//      came from the vertex shader, and are < 0, ZScale, ZBias >
// Texture Stage 3: DotProduct for denominator and division of Z / W.  The
//      texture coords here were a constant passed through the vertex shader
//      and are < 0, 0, 1 >
// Since texture stages 2 and 3 use the HILO_1 dot mapping, the <A, R, G, B>
//      lookup from Stage 1 is converted to <AR, GB, 1>.  So our Z / W
//      calculation looks like:
//      < AR, GB, 1 > . < 0, ZScale, ZBias > / < AR, GB, 1 > . < 0, 0, 1 >
//    = ZScale * GB + ZBias / 1
//      GB is the depth value from the depth texture scaled to [0, 1], so we
//      end up with
//      ZScale * Depth + ZBias
//
// Combiner Stage 0: RO.rgb = T0.rgb(explosion image) * V0.rgb(vertex color)
//                   R0.a   = T0.a(explosion alpha) * C0(alpha fade)
//-----------------------------------------------------------------------------
HRESULT CExplosion::InitPixelShader()
{
    D3DPIXELSHADERDEF psd;

    ZeroMemory( &psd, sizeof( D3DPIXELSHADERDEF ) );
    psd.PSCombinerCount = PS_COMBINERCOUNT( 1, 0 );
    psd.PSTextureModes  = PS_TEXTUREMODES( PS_TEXTUREMODES_PROJECT2D,  // Real texture
                                           PS_TEXTUREMODES_PROJECT2D,  // ZW texture
                                           PS_TEXTUREMODES_DOTPRODUCT, // (s0, t0, r0).(z, w, 1)
                                           PS_TEXTUREMODES_DOT_ZW );   // (s1, t1, r1).(z, w, 1)

    psd.PSInputTexture  = PS_INPUTTEXTURE( 0, 0, 1, 1 );
    psd.PSDotMapping    = PS_DOTMAPPING( 0,
                                         0,
                                         PS_DOTMAPPING_HILO_1,
                                         PS_DOTMAPPING_HILO_1 );
    psd.PSCompareMode   = PS_COMPAREMODE( 
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT );

    //--------------------------
    // Stage 0 
    //--------------------------

    // A=T0.rgb, B=V0, C=0, D=0 (so that AB.rgb = T0*V0 and CD = 0 )
    psd.PSRGBInputs[0]    = PS_COMBINERINPUTS(
        PS_REGISTER_T0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_V0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY );
    
    // A=T0.a, B=C0, C=0, D=0 (so that AB.a = T0.a*C0.a and CD = 0 )
    psd.PSAlphaInputs[0]  = PS_COMBINERINPUTS(
        PS_REGISTER_T0   | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_C0   | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
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

    // E = 0, F = 0, G = 1. (From above, EF is not used. G is alpha and is set to R0.a.)
    psd.PSFinalCombinerInputsEFG=PS_COMBINERINPUTS(
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_R0   | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
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
// Name: InitVertexShader
// Desc: Initializes and creates our vertex shader.  Also sets up vertex
//       shader constants that don't change:
//       * texture coordinates for stages 2 and 3
//       * diffuse color
//-----------------------------------------------------------------------------
HRESULT CExplosion::InitVertexShader()
{
    // Setup the vertex declaration
    DWORD dwDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ), // v0 = Position
        D3DVSD_REG( 7, D3DVSDT_FLOAT2 ), // v7 = Texcoords
        D3DVSD_END()
    };

    // Create the vertex shader
    XBUtil_CreateVertexShader( m_pd3dDevice, "shaders\\zsprite.xvu", dwDecl, &m_dwVertexShader );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize
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

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
                                      resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load all our textures
    m_pGroundTex = m_xprResource.GetTexture( "Ground.bmp" );
    m_pExp[0]    = m_xprResource.GetTexture( "exp1.tga" );
    m_pExpD[0]   = m_xprResource.GetTexture( "exp1d.bmp" );
    m_pExp[1]    = m_xprResource.GetTexture( "exp2.tga" );
    m_pExpD[1]   = m_xprResource.GetTexture( "exp2d.bmp" );
    m_pExp[2]    = m_xprResource.GetTexture( "exp3.tga" );
    m_pExpD[2]   = m_xprResource.GetTexture( "exp3d.bmp" );

    // Create our terrain
    m_Terrain.Initialize( m_pd3dDevice );
    m_Terrain.Generate( 12, 12,
                        D3DXVECTOR2( -10.0f, -10.0f ),
                        D3DXVECTOR2( 10.0f, 10.0f ),
                        m_pGroundTex,
                        fHeightFn );

    // Set up stuff for view transform
    g_vViewAngle = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vCameraPos = D3DXVECTOR3( 0.0f, 0.0f, -14.0f );

    // World transform is just identity
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set the view transform based on view angle
    SetViewTransform();

    // Projection transform
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/3, 4.0f/3.0f, 1.0f, 30.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
    g_matProj = matProj;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetViewTransform
// Desc: Sets the view transform based on the view angle
//-----------------------------------------------------------------------------
VOID CXBoxSample::SetViewTransform()
{
    D3DXMATRIX matView;
    D3DXMATRIX m;
    D3DXVECTOR3 vLookAt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f);

    // Create a rotation matrix for our view angle
    D3DXMatrixRotationYawPitchRoll(&m, g_vViewAngle.y, g_vViewAngle.x, g_vViewAngle.z);

    // Transform our camera position (on the Z axis)
    D3DXVec3TransformCoord(&g_vEyePos, &m_vCameraPos, &m);
    
    // Use that eye position and look at the origin
    D3DXMatrixLookAtLH(&matView, &g_vEyePos, &vLookAt, &vUp);

    m_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
    g_matView = matView;
}




//-----------------------------------------------------------------------------
// Name: ExpSortCB()
// Desc: Callback function for sorting explosions in back-to-front order
// Note: Even though we're sorting the explosions back-to-front, that doesn't
//       mean that each _pixel_ will be rendered back-to-front, so there still
//       may be alpha-blending artifacts when explosions overlap.  Sorting 
//       minimizes these artifacts greatly.
//-----------------------------------------------------------------------------
int _cdecl ExpSortCB( const VOID* arg1, const VOID* arg2 )
{
    CExplosion * p1 = *( (CExplosion **) arg1 );
    CExplosion * p2 = *( (CExplosion **) arg2 );

#if 0
    FLOAT dx1 = p1->m_vPos.x - g_vEyePos.x;
    FLOAT dx2 = p2->m_vPos.x - g_vEyePos.x;
    FLOAT dy1 = p1->m_vPos.y - g_vEyePos.y;
    FLOAT dy2 = p2->m_vPos.y - g_vEyePos.y;
    FLOAT dz1 = p1->m_vPos.z - g_vEyePos.z;
    FLOAT dz2 = p2->m_vPos.z - g_vEyePos.z;
#else
    FLOAT dx1 = p1->m_vCorrectedPos.x - g_vEyePos.x;
    FLOAT dx2 = p2->m_vCorrectedPos.x - g_vEyePos.x;
    FLOAT dy1 = p1->m_vCorrectedPos.y - g_vEyePos.y;
    FLOAT dy2 = p2->m_vCorrectedPos.y - g_vEyePos.y;
    FLOAT dz1 = p1->m_vCorrectedPos.z - g_vEyePos.z;
    FLOAT dz2 = p2->m_vCorrectedPos.z - g_vEyePos.z;
#endif

    if( ( dx1 * dx1 + dy1 * dy1 + dz1 * dz1 ) < ( dx2 * dx2 + dy2 * dy2 + dz2 * dz2 ) )
        return +1;

    return -1;
}




//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
        m_bDrawHelp = !m_bDrawHelp;

    //
    // Move the camera based off the gamepad
    //
    g_vViewAngle.y -= m_DefaultGamepad.fX1*1.0f*m_fElapsedTime;
    if( g_vViewAngle.y > D3DX_PI*2 )
        g_vViewAngle.y -= D3DX_PI*2;
    if( g_vViewAngle.y < 0.0f)
        g_vViewAngle.y += D3DX_PI*2;

    g_vViewAngle.x += m_DefaultGamepad.fY1*1.0f*m_fElapsedTime;
    if( g_vViewAngle.x > D3DX_PI / 2.1f )
        g_vViewAngle.x = D3DX_PI / 2.1f;
    if( g_vViewAngle.x < -D3DX_PI / 2.1f )
        g_vViewAngle.x = -D3DX_PI / 2.1f;

    m_vCameraPos.z += m_DefaultGamepad.fY2*1.0f*m_fElapsedTime;
    if( m_vCameraPos.z > -0.1f )
        m_vCameraPos.z = -0.1f;

    // Update the view transform
    SetViewTransform();

    // Update each explosion - if it's faded out, recreate it
    for( int i = 0; i < MAX_EXPLOSIONS; i++ )
    {
        // No explosion here - make a new one.
        if( m_apExplosion[i] == NULL ||
            m_fAppTime - m_apExplosion[i]->m_fStartTime > m_apExplosion[i]->m_fDuration )
        {
            delete m_apExplosion[i];

            D3DXVECTOR3 vPos;
            FLOAT fRadius;
            FLOAT fDuration;
            DWORD dwExp;

            // 3rd explosion texture doesn't look very good
            // when rotated, so we'll just use the first 2.
            dwExp = rand() % 2;

            vPos.x = 20.0f * rand() / RAND_MAX - 10.0f;
            vPos.z = 20.0f * rand() / RAND_MAX - 10.0f;
            vPos.y = fHeightFn( vPos.x, vPos.z );

            fRadius = 0.6f + 0.5f * rand() / RAND_MAX;
            fDuration = 4.0f * rand() / RAND_MAX + 3.0f;

            m_apExplosion[i] = new CExplosion( m_pd3dDevice,
                                               m_pExp[ dwExp ],
                                               m_pExpD[ dwExp ],
                                               vPos,
                                               fRadius,
                                               fDuration );
            m_apExplosion[i]->m_fStartTime = m_fAppTime;
        }

        //
        // Update explosion parameters
        //

        // Radius is based off of time
        FLOAT fExpTime = m_fAppTime - m_apExplosion[i]->m_fStartTime;
        m_apExplosion[i]->m_fCurrentRadius = fExpTime / m_apExplosion[i]->m_fDuration * m_apExplosion[i]->m_fMaxRadius;

        // Update corrected position of quad based off current eye position
        D3DXVECTOR3 vEyeToCenter = m_apExplosion[i]->m_vPos - g_vEyePos;
        D3DXVec3Normalize( &vEyeToCenter, &vEyeToCenter );
        m_apExplosion[i]->m_vCorrectedPos = m_apExplosion[i]->m_vPos - m_apExplosion[i]->m_fCurrentRadius * vEyeToCenter;

        // Adjust alpha for time
        m_apExplosion[i]->m_fAlpha = fExpTime < m_apExplosion[i]->m_fDuration / 2.0f ?
                                        1.0f :
                                        ( m_apExplosion[i]->m_fDuration - fExpTime ) * 2.0f / m_apExplosion[i]->m_fDuration;

    }

    // Sort explosions in back-to-front order (depends on global g_vEyePos)
    qsort( m_apExplosion, MAX_EXPLOSIONS, sizeof(CExplosion *), ExpSortCB );

    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_Y ] )
    {
        g_bZSprite = !g_bZSprite;
    }
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
    {
        g_bWireFrame = !g_bWireFrame;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    // Set our render state
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

    // Render the terrain
    m_Terrain.Render();

    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    // Render each explosion
    for( int i = 0; i < MAX_EXPLOSIONS; i++ )
        if( m_apExplosion[i] )
            m_apExplosion[i]->Render();
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"Explosion" );
        m_Font.DrawText(  64, 80, 0xffffff00, g_bZSprite ? L"Rendering with ZSprites" : L"Rendering with Billboards" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CExplosion (constructor)
// Desc: Sets up an instance of the explosion.
//-----------------------------------------------------------------------------
CExplosion::CExplosion( LPDIRECT3DDEVICE8 pDevice,
                        LPDIRECT3DTEXTURE8 pExplosionImage, 
                        LPDIRECT3DTEXTURE8 pExplosionDepth,
                        D3DXVECTOR3 vCenter,
                        FLOAT fMaxRadius,
                        FLOAT fDuration )
{
    m_pd3dDevice = pDevice;
    m_pd3dDevice->AddRef();
    m_pImage = pExplosionImage;
    m_pImage->AddRef();
    m_pDepth = pExplosionDepth;
    m_pDepth->AddRef();

    m_vPos = vCenter;
    m_fMaxRadius = fMaxRadius;
    m_fDuration = fDuration;

    // First explosion gets to create the quad and set up shaders
    if( m_pvbQuad == NULL )
    {
        EXPLOSION_QUAD_VERTEX * pVertices;
        m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( EXPLOSION_QUAD_VERTEX ), 0, 0, 0, &m_pvbQuad );
        m_pvbQuad->Lock( 0, 0, (BYTE **)&pVertices, 0 );
        pVertices[0].p = D3DXVECTOR3( -1.0f, -1.0f, 0.0f ); pVertices[0].t = D3DXVECTOR2( 0.0f, 1.0f );
        pVertices[1].p = D3DXVECTOR3( -1.0f,  1.0f, 0.0f ); pVertices[1].t = D3DXVECTOR2( 0.0f, 0.0f );
        pVertices[2].p = D3DXVECTOR3(  1.0f, -1.0f, 0.0f ); pVertices[2].t = D3DXVECTOR2( 1.0f, 1.0f );
        pVertices[3].p = D3DXVECTOR3(  1.0f,  1.0f, 0.0f ); pVertices[3].t = D3DXVECTOR2( 1.0f, 0.0f );
        m_pvbQuad->Unlock();

        // Create our shaders
        InitVertexShader();
        InitPixelShader();
    }

    switch( rand() * 4 / RAND_MAX )
    {
    case 0:
        m_vTransform1 = D3DXVECTOR4( 0.0f, -1.0f, 0.0f, 0.0f );
        m_vTransform2 = D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 0.0f );
        break;
    case 1:
        m_vTransform1 = D3DXVECTOR4( -1.0f, 0.0f, 0.0f, 0.0f );
        m_vTransform2 = D3DXVECTOR4( 0.0f, -1.0f, 0.0f, 0.0f );
        break;
    case 2:
        m_vTransform1 = D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 0.0f );
        m_vTransform2 = D3DXVECTOR4( -1.0f, 0.0f, 0.0f, 0.0f );
        break;
    case 3:
        m_vTransform1 = D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 0.0f );
        m_vTransform2 = D3DXVECTOR4( 0.0f, 1.0f, 0.0f, 0.0f );
        break;
    }
}




//-----------------------------------------------------------------------------
// Name: ~CExplosion (destructor)
// Desc: Releases resources held by the object
//-----------------------------------------------------------------------------
CExplosion::~CExplosion()
{
    m_pImage->Release();
    m_pDepth->Release();
    m_pd3dDevice->Release();
}




//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the explosion
// Consider: Transforming stage 0/1 texture coords to get different explosions
//-----------------------------------------------------------------------------
HRESULT CExplosion::Render()
{
    D3DXMATRIX matWorldSave;
    D3DXMATRIX matScale, matRotate, matTranslate, matWorld, matView, matProj;
    D3DXVECTOR3 vEyeToCenter = m_vPos - g_vEyePos;
    D3DXVECTOR3 vCorrectedPos;

    // Scale by our radius
    D3DXMatrixScaling( &matScale, m_fCurrentRadius, m_fCurrentRadius, 0.0f );

    // Rotate to face viewer
    D3DXMatrixRotationYawPitchRoll(&matRotate, g_vViewAngle.y, g_vViewAngle.x, g_vViewAngle.z);

    // Translate to center - fRadius * vEyeToCenter
    D3DXMatrixTranslation( &matTranslate, m_vCorrectedPos.x, m_vCorrectedPos.y, m_vCorrectedPos.z );

    // Caculate our world transform 
    D3DXMatrixMultiply( &matWorld, &matScale, &matRotate );
    D3DXMatrixMultiply( &matWorld, &matWorld, &matTranslate );

    // For ZSprites, we have to do some additional work for
    // our vertex shader
    if( g_bZSprite )
    {

        // For transforming vertices, we'll need (WVP)t
        // For transforming world positions, we'll need (VP)t
        // Calculate World * View * Projection and transposes
        D3DXMATRIX matVP, matVPt;
        D3DXMATRIX matWVP, matWVPt;
        D3DXMatrixMultiply( &matVP, &g_matView, &g_matProj );
        D3DXMatrixMultiply( &matWVP, &matWorld, &matVP );
        D3DXMatrixTranspose( &matVPt, &matVP );
        D3DXMatrixTranspose( &matWVPt, &matWVP );

        // We use Vec3's internally, but need to give vec4's to
        // the vertex shader
        D3DXVECTOR4 vCorrect, vPos;
        vCorrect.x = m_vCorrectedPos.x;
        vCorrect.y = m_vCorrectedPos.y;
        vCorrect.z = m_vCorrectedPos.z;
        vCorrect.w = 1.0f;
        vPos.x = m_vPos.x;
        vPos.y = m_vPos.y;
        vPos.z = m_vPos.z;
        vPos.w = 1.0f;

        // More vertex shader constant initialization
        D3DXVECTOR3 c4(0,0,0);
        D3DXVECTOR3 c5 = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
        D3DXVECTOR3 c6 = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
        D3DXVECTOR4 c9 = D3DXVECTOR4( D3DZ_MAX_D24S8, D3DZ_MAX_D24S8, D3DZ_MAX_D24S8, D3DZ_MAX_D24S8 );
        D3DXVECTOR4 c16 = D3DXVECTOR4( 0.5f, 0.5f, 0.0f, 0.0f );

        // Set vertex shader constants
        m_pd3dDevice->SetVertexShaderConstant( 0, &matWVPt, 4 );   // WVP(transpose)
        m_pd3dDevice->SetVertexShaderConstant( 4, &c4, 1 );        // 0 vector for initial T2 coords
        m_pd3dDevice->SetVertexShaderConstant( 5, &c5, 1 );        // T3 coords
        m_pd3dDevice->SetVertexShaderConstant( 6, &c6, 1 );        // Diffuse color
        m_pd3dDevice->SetVertexShaderConstant( 7, &vPos, 1 );      // Center of explosion
        m_pd3dDevice->SetVertexShaderConstant( 8, &vCorrect, 1 );  // Center of quad
        m_pd3dDevice->SetVertexShaderConstant( 9, &c9, 1 );        // Scale vector
        m_pd3dDevice->SetVertexShaderConstant( 10, &matVPt, 4 );   // VP(transpose)
        m_pd3dDevice->SetVertexShaderConstant( 14, &m_vTransform1, 1 );
        m_pd3dDevice->SetVertexShaderConstant( 15, &m_vTransform2, 1 );
        m_pd3dDevice->SetVertexShaderConstant( 16, &c16, 1 );
    }

    // Set state for rendering
    m_pd3dDevice->SetStreamSource( 0, m_pvbQuad, sizeof( EXPLOSION_QUAD_VERTEX ) );
    m_pd3dDevice->SetTexture( 0, m_pImage );

    // Save old transform, set new one
    m_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorldSave );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    if( g_bZSprite )
    {
        // Set up to render our ZSprite
        m_pd3dDevice->SetTexture( 1, m_pDepth );
        m_pd3dDevice->SetTexture( 2, m_pDepth );
        m_pd3dDevice->SetTexture( 3, m_pDepth );

        // Select our shaders
        m_pd3dDevice->SetVertexShader( m_dwVertexShader );
        m_pd3dDevice->SetPixelShader( m_dwPixelShader );

        // Calculate alpha constant for pixel shader based on fade of explosion
        DWORD dwC0 = DWORD(m_fAlpha * 255.0f) << 24;
        m_pd3dDevice->SetRenderState( D3DRS_PSCONSTANT0_0, dwC0 );

    }
    else
    {
        // Set up to render a billboard
        m_pd3dDevice->SetVertexShader( D3DFVF_XYZ|D3DFVF_TEX1 );
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, DWORD( m_fAlpha * 255 ) << 24 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    }

    // Actually draw the quad
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
    m_pd3dDevice->SetPixelShader( NULL );

    if( g_bWireFrame )
    {
        // Render wireframe quad
        m_pd3dDevice->SetStreamSource( 0, m_pvbQuad, sizeof( EXPLOSION_QUAD_VERTEX ) );
        m_pd3dDevice->SetVertexShader( FVF_EXPLOSION_QUAD_VERTEX );
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xFF00FF00 );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
    }

    // Restore transform
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorldSave );

    return S_OK;
}
