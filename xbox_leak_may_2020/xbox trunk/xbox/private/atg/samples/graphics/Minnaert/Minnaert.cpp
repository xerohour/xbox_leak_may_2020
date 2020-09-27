//-----------------------------------------------------------------------------
// File: Minnaert.cpp
//
// Desc: Sample to show how to use a 2D texture as a per-pixel lookup table
//       for lighting (the tu and tv coordinates are used to look up a value
//       for the LdotN contribution modulated with the EdotN contribution.)
//       This is a partial solution to a more general form of BRDF lighting,
//       and can be used to implement Minnaert lighting (nice, per-pixel self-
//       shadowing effect), as well as satin, velvet and other effects.
//
// Hist: 04.01.01 - New for May XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XBUtil.h>




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Rotate object" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Move light" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle custom\nlighting" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Change custom\nlighting effect" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display\nhelp" },
};

#define NUM_HELP_CALLOUTS 5




// BRDFVertex
struct BRDFVERTEX
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    FLOAT       tu, tv;
};

#define D3DFVF_BRDFVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0))

enum LIGHTING_EFFECT { MINNAERT, SATIN, VELVET };

#define MINNAERT_CONSTANT 0.22456f
#define DRESS_MESHSUBSET 7


class CXBWomanMesh : public CXBMesh
{
public:
    BOOL m_bRenderDress;

    BOOL RenderCallback( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwSubset,
                         XBMESH_SUBSET* pSubset, DWORD dwFlags );
};

BOOL CXBWomanMesh::RenderCallback( LPDIRECT3DDEVICE8 pd3dDevice, DWORD dwSubset,
                                   XBMESH_SUBSET* pSubset, DWORD dwFlags )
{
    if( dwSubset == DRESS_MESHSUBSET )
        return m_bRenderDress;
    else
        return !m_bRenderDress;
}




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class. The base class provides just about all the
//       functionality we want, so we're just supplying stubs to interface with
//       the non-C++ functions of the app.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont     m_Font;               // Font class
    CXBHelp     m_Help;               // Help class
    BOOL        m_bDrawHelp;          // Whether to draw help

    FLOAT       m_fCameraDistance;    // Camera and matrices
    D3DXVECTOR3 m_vEyePt;
    D3DXVECTOR3 m_vLookatPt;
    D3DXVECTOR3 m_vUp;
    D3DXMATRIX  m_matView;
    D3DXMATRIX  m_matProj;
    
    CXBMesh     m_LightObject;        // The light
    D3DXVECTOR3 m_vLightCenter;
    FLOAT       m_fLightRadius;
    D3DXMATRIX  m_matLightObject;
    D3DXMATRIX  m_matLightMatrix;
    FLOAT       m_fLightYaw;
    FLOAT       m_fLightPitch;
    D3DXVECTOR3 m_LightDir;
    D3DXVECTOR4 m_vVertexShaderLightDir;
    D3DXVECTOR4 m_vPixelShaderLightDir;
        
    CXBWomanMesh            m_WomanObject;   // The woman with the BRDF dress
    D3DXVECTOR3             m_vWomanCenter;
    FLOAT                   m_fWomanRadius;
    DWORD                   m_dwWomanVertexShader;
    DWORD                   m_dwWomanPixelShader1;
    DWORD                   m_dwWomanPixelShader2;
    D3DXMATRIX              m_matWomanObject;
    D3DXMATRIX              m_WomanRotationMatrix;
    D3DXMATRIX              m_matWomanMatrix;
    
    LPDIRECT3DCUBETEXTURE8  m_pNormalizationCubemap; // Textures for the BRDF effect
    LPDIRECT3DTEXTURE8      m_pMinnaertTexture;
    LPDIRECT3DTEXTURE8      m_pSatinTexture;
    LPDIRECT3DTEXTURE8      m_pVelvetTexture;

    BOOL                    m_bUseCustomLighting; // Lighting options
    LIGHTING_EFFECT         m_LightingEffect;

    HRESULT CreateMinnaertMap( DWORD dwWidth, DWORD dwHeight, FLOAT k );
    HRESULT CreateSatinMap( DWORD dwWidth, DWORD dwHeight );
    HRESULT CreateVelvetMap( DWORD dwWidth, DWORD dwHeight );
    HRESULT SetBRDFShaderConstants();

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

    // Initialize members
    m_bDrawHelp             = FALSE;

    m_bUseCustomLighting    = TRUE;
    m_LightingEffect        = MINNAERT;

    m_pMinnaertTexture      = NULL;
    m_pSatinTexture         = NULL;
    m_pVelvetTexture        = NULL;

    m_pNormalizationCubemap = NULL;

    m_LightDir.x  =  0.5f;
    m_LightDir.y  = -0.5f;
    m_LightDir.z  =  0.5f;
    D3DXVec3Normalize( &m_LightDir, &m_LightDir );
    m_fLightYaw   = -D3DX_PI/4;
    m_fLightPitch = +D3DX_PI/4;
    D3DXMatrixIdentity( &m_matLightObject );

    D3DXMatrixIdentity( &m_WomanRotationMatrix );
    D3DXMatrixIdentity( &m_matWomanObject );
}




//-----------------------------------------------------------------------------
// Name: CreatePixelShader1()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CreatePixelShader1( LPDIRECT3DDEVICE8 pd3dDevice, DWORD* pdwPixelShader )
{
    D3DPIXELSHADERDEF psd;
    ZeroMemory(&psd, sizeof(psd));

    psd.PSCombinerCount=PS_COMBINERCOUNT(
        3,
        PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
    psd.PSTextureModes=PS_TEXTUREMODES(
        PS_TEXTUREMODES_CUBEMAP,
        PS_TEXTUREMODES_DOTPRODUCT, 
        PS_TEXTUREMODES_DOT_ST, // argb = Texture2D( oT1 dot Cubemap(oT0), oT2 dot Cubemap(oT0) )
        PS_TEXTUREMODES_CUBEMAP);
    psd.PSInputTexture=PS_INPUTTEXTURE(0,0,0,0);
    psd.PSDotMapping=PS_DOTMAPPING(
        0,
        PS_DOTMAPPING_MINUS1_TO_1_D3D,
        PS_DOTMAPPING_MINUS1_TO_1_D3D,
        PS_DOTMAPPING_MINUS1_TO_1_D3D);
    psd.PSCompareMode=PS_COMPAREMODE(
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT);

    //------------- Stage 0 -------------
    psd.PSRGBInputs[0]=PS_COMBINERINPUTS(
        PS_REGISTER_C0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_EXPAND_NORMAL,
        PS_REGISTER_T0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_EXPAND_NORMAL,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
    psd.PSAlphaInputs[0]=PS_COMBINERINPUTS(
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

    psd.PSRGBOutputs[0]=PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_AB_DOT_PRODUCT );
    psd.PSAlphaOutputs[0]=PS_COMBINEROUTPUTS(
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY );

    //------------- Stage 1 -------------
    psd.PSRGBInputs[1]=PS_COMBINERINPUTS(
        PS_REGISTER_R0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_SIGNED_IDENTITY,
        PS_REGISTER_T2   | PS_CHANNEL_RGB | PS_INPUTMAPPING_SIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
    psd.PSAlphaInputs[1]=PS_COMBINERINPUTS(
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

    psd.PSRGBOutputs[1]=PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_AB_MULTIPLY );
    psd.PSAlphaOutputs[1]=PS_COMBINEROUTPUTS(
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY );

    //------------- Stage 2 -------------
    psd.PSRGBInputs[2]=PS_COMBINERINPUTS(
        PS_REGISTER_T2   | PS_CHANNEL_RGB | PS_INPUTMAPPING_SIGNED_IDENTITY,
        PS_REGISTER_V0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_SIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
    psd.PSAlphaInputs[2]=PS_COMBINERINPUTS(
        PS_REGISTER_V0   | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ONE  | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

    psd.PSRGBOutputs[2]=PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
    psd.PSAlphaOutputs[2]=PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_AB_MULTIPLY );

    //------------- Final combiner -------------
    psd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS( PS_REGISTER_ONE | PS_CHANNEL_RGB,
                                                      PS_REGISTER_ONE | PS_CHANNEL_RGB,
                                                      PS_REGISTER_R0  | PS_CHANNEL_ALPHA,
                                                      0 | 0 | 0 );

    psd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS( PS_REGISTER_ONE   | PS_CHANNEL_ALPHA,
                                                       PS_REGISTER_R0    | PS_CHANNEL_RGB,
                                                       PS_REGISTER_ZERO  | PS_CHANNEL_RGB,
                                                       PS_REGISTER_ZERO  | PS_CHANNEL_RGB );

    // Create the pixel shader
    if( FAILED( pd3dDevice->CreatePixelShader( &psd, pdwPixelShader ) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreatePixelShader2()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CreatePixelShader2( LPDIRECT3DDEVICE8 pd3dDevice, DWORD* pdwPixelShader )
{
    D3DPIXELSHADERDEF psd;
    ZeroMemory(&psd, sizeof(psd));

    psd.PSCombinerCount=PS_COMBINERCOUNT(
        1,
        PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
    psd.PSTextureModes=PS_TEXTUREMODES(
        PS_TEXTUREMODES_CUBEMAP,
        PS_TEXTUREMODES_CUBEMAP,
        PS_TEXTUREMODES_CUBEMAP,
        PS_TEXTUREMODES_CUBEMAP);
    psd.PSInputTexture=PS_INPUTTEXTURE(0,0,0,0);
    psd.PSDotMapping=PS_DOTMAPPING(
        0,
        PS_DOTMAPPING_MINUS1_TO_1_D3D,
        PS_DOTMAPPING_MINUS1_TO_1_D3D,
        PS_DOTMAPPING_MINUS1_TO_1_D3D);
    psd.PSCompareMode=PS_COMPAREMODE(
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
        PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT);

    //------------- Stage 0 -------------
    psd.PSRGBInputs[0]=PS_COMBINERINPUTS(
        PS_REGISTER_T1   | PS_CHANNEL_RGB | PS_INPUTMAPPING_EXPAND_NORMAL,
        PS_REGISTER_T0   | PS_CHANNEL_RGB | PS_INPUTMAPPING_EXPAND_NORMAL,
        PS_REGISTER_ONE  | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
    psd.PSAlphaInputs[0]=PS_COMBINERINPUTS(
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

    psd.PSRGBOutputs[0]=PS_COMBINEROUTPUTS(
        PS_REGISTER_R0,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_AB_DOT_PRODUCT );
    psd.PSAlphaOutputs[0]=PS_COMBINEROUTPUTS(
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_REGISTER_DISCARD,
        PS_COMBINEROUTPUT_IDENTITY );

    //------------- Final combiner -------------
    psd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS( PS_REGISTER_ONE | PS_CHANNEL_RGB,
                                                      PS_REGISTER_ONE | PS_CHANNEL_RGB,
                                                      PS_REGISTER_R0  | PS_CHANNEL_ALPHA,
                                                      0 | 0 | 0 );

    psd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS( PS_REGISTER_V0    | PS_CHANNEL_RGB,
                                                       PS_REGISTER_R0    | PS_CHANNEL_RGB,
                                                       PS_REGISTER_ZERO  | PS_CHANNEL_RGB,
                                                       PS_REGISTER_ZERO  | PS_CHANNEL_RGB );

    // Create the pixel shader
    if( FAILED( pd3dDevice->CreatePixelShader( &psd, pdwPixelShader ) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateMinnaertMap()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateMinnaertMap( DWORD dwWidth, DWORD dwHeight, FLOAT k )
{
    VOID*  pSrcBits = new DWORD[dwWidth*dwHeight];
    DWORD* pBits32  = (DWORD*)pSrcBits;

    for( DWORD y = 0; y < dwHeight; y++ )
    {
        for( DWORD x = 0; x < dwWidth; x++ )
        {
            FLOAT fx = ((FLOAT)x)/(dwWidth-1);
            FLOAT fy = ((FLOAT)y)/(dwHeight-1);

            fx = powf( fx, k );
            fy = powf( fy, 1-k );

            DWORD dwIntensity = (BYTE)(fx*fy* 255.0f);

            *pBits32++ = ( (dwIntensity<<16) | (dwIntensity<<8) | (dwIntensity<<0) );
        }
    }

    m_pd3dDevice->CreateTexture( dwWidth, dwHeight, 1, 0, D3DFMT_X8R8G8B8, 
                                 D3DPOOL_MANAGED, &m_pMinnaertTexture );
    
    D3DLOCKED_RECT lock;
    m_pMinnaertTexture->LockRect( 0, &lock, NULL, 0 );
    XGSwizzleRect( pSrcBits, lock.Pitch, NULL, lock.pBits, dwWidth, dwHeight, NULL, 4 );
    m_pMinnaertTexture->UnlockRect( 0 );

    delete pSrcBits;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateSatinMap()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateSatinMap( DWORD dwWidth, DWORD dwHeight )
{
    VOID*  pSrcBits = new DWORD[dwWidth*dwHeight];
    DWORD* pBits32  = (DWORD*)pSrcBits;

    for( DWORD y = 0; y < dwHeight; y++ )
    {
        for( DWORD x = 0; x < dwWidth; x++ )
        {
            // The light (L dot N) value is stored in tu
            FLOAT fLN = ((FLOAT)x)/(dwWidth-1);

            // The eye (E dot N) value is stored in tu
            FLOAT fEN = ((FLOAT)y)/(dwHeight-1);

            // For a satin effect, intensify the falloff around the edges.
            DWORD dwIntensity = (BYTE)( fLN * fEN * fEN * 255.0f);

            *pBits32++ = ( (dwIntensity<<16) | (dwIntensity<<8) | (dwIntensity<<0) );
        }
    }

    m_pd3dDevice->CreateTexture( dwWidth, dwHeight, 1, 0, D3DFMT_X8R8G8B8, 
                                 D3DPOOL_MANAGED, &m_pSatinTexture );
    
    D3DLOCKED_RECT lock;
    m_pSatinTexture->LockRect( 0, &lock, NULL, 0 );
    XGSwizzleRect( pSrcBits, lock.Pitch, NULL, lock.pBits, dwWidth, dwHeight, NULL, 4 );
    m_pSatinTexture->UnlockRect( 0 );

    delete pSrcBits;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CreateVelvetMap()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CreateVelvetMap( DWORD dwWidth, DWORD dwHeight )
{
    VOID*  pSrcBits = new DWORD[dwWidth*dwHeight];
    DWORD* pBits32  = (DWORD*)pSrcBits;

    for( DWORD y = 0; y < dwHeight; y++ )
    {
        for( DWORD x = 0; x < dwWidth; x++ )
        {
            // The light (L dot N) value is stored in tu
            FLOAT fLN = ((FLOAT)x)/(dwWidth-1);

            // The eye (E dot N) value is stored in tu
            FLOAT fEN = ((FLOAT)y)/(dwHeight-1);

            // For a velvet effect, modulate the light contribution by the
            // inverse of the eye value. This way, only edges get lit.
            DWORD dwIntensity = (BYTE)( (1-fEN)* 255.0f);

            *pBits32++ = ( (dwIntensity<<16) | (dwIntensity<<8) | (dwIntensity<<0) );
        }
    }

    m_pd3dDevice->CreateTexture( dwWidth, dwHeight, 1, 0, D3DFMT_X8R8G8B8, 
                                 D3DPOOL_MANAGED, &m_pVelvetTexture );
    
    D3DLOCKED_RECT lock;
    m_pVelvetTexture->LockRect( 0, &lock, NULL, 0 );
    XGSwizzleRect( pSrcBits, lock.Pitch, NULL, lock.pBits, dwWidth, dwHeight, NULL, 4 );
    m_pVelvetTexture->UnlockRect( 0 );

    delete pSrcBits;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: This creates all device-dependant display objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load geometry
    if( FAILED( m_LightObject.Create( m_pd3dDevice, "Models\\DirLight.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    m_vLightCenter = D3DXVECTOR3( 0, 0, 0 );
    m_fLightRadius = 0.5f;

    if( FAILED( m_WomanObject.Create( m_pd3dDevice, "Models\\Woman.xbg" ) ) )
        return XBAPPERR_MEDIANOTFOUND;
    m_vWomanCenter = D3DXVECTOR3( 0.0f, 0.75f, 0.0f );
    m_fWomanRadius = 1.0f;

    // Create vertex shader for the BRDF
    DWORD dwWomanVertexDecl[] =
    {
        D3DVSD_STREAM( 0 ),
        D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),   // v0 = Position
        D3DVSD_REG( 3, D3DVSDT_FLOAT3 ),   // v0 = Normal
        D3DVSD_REG( 6, D3DVSDT_FLOAT2 ),   // v3 = Texture coords
        D3DVSD_END()
    };

    if( FAILED( XBUtil_CreateVertexShader( m_pd3dDevice, "Shaders\\BRDF.xvu",
                                           dwWomanVertexDecl,
                                           &m_dwWomanVertexShader ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( CreatePixelShader1( m_pd3dDevice, &m_dwWomanPixelShader1 ) ) )
        return E_FAIL;
    if( FAILED( CreatePixelShader2( m_pd3dDevice, &m_dwWomanPixelShader2 ) ) )
        return E_FAIL;

    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );

    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    
    // Create a normalization cubemap
    if( FAILED( XBUtil_CreateNormalizationCubeMap( m_pd3dDevice, 256, &m_pNormalizationCubemap ) ) )
        return E_FAIL;

    // Set the view matrix, allowing enough room for the objects
    m_fCameraDistance = 3.0f;
    m_vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, -m_fCameraDistance );
    m_vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_matView, &m_vEyePt, &m_vLookatPt, &m_vUp );
    m_pd3dDevice->SetTransform( D3DTS_VIEW,       &m_matView );

    // Set the projection matrix
    D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 640.0f/480.0f, 
                                m_fCameraDistance/100.0f, m_fCameraDistance*100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

    // Create the (NdotL, EdotL) custom lighting maps
    CreateMinnaertMap( 256, 256, MINNAERT_CONSTANT );
    CreateSatinMap( 256, 256 );
    CreateVelvetMap( 256, 256 );

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

    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        m_bUseCustomLighting = !m_bUseCustomLighting;

    if( m_bUseCustomLighting )
    {
        if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
        {
            switch( m_LightingEffect )
            {
                case MINNAERT: m_LightingEffect = SATIN;    break;
                case SATIN:    m_LightingEffect = VELVET;   break;
                case VELVET:   m_LightingEffect = MINNAERT; break;
            }
        }
    }

    // Update the woman orientation
    {
        FLOAT fRotateX1 = m_fElapsedTime * m_DefaultGamepad.fX2;
        FLOAT fRotateY1 = m_fElapsedTime * m_DefaultGamepad.fY2;
        D3DXMATRIX matRotateWoman;
        D3DXMatrixRotationYawPitchRoll( &matRotateWoman, -fRotateX1, fRotateY1, 0.0f );
        D3DXMatrixMultiply( &m_WomanRotationMatrix, &m_WomanRotationMatrix, &matRotateWoman );

        D3DXMATRIX matTrans;
        D3DXMatrixTranslation( &matTrans, -m_vWomanCenter.x,
                                          -m_vWomanCenter.y,
                                          -m_vWomanCenter.z );

        D3DXMatrixMultiply( &m_matWomanMatrix, &matTrans, &m_WomanRotationMatrix );
    }

    // Update the light orientation
    {
        // Update yaw and pitch from the gamepad
        m_fLightYaw   += m_fElapsedTime * m_DefaultGamepad.fX1;
        m_fLightPitch += m_fElapsedTime * m_DefaultGamepad.fY1;
        m_fLightYaw   = min( +D3DX_PI/4, max( -D3DX_PI/4, m_fLightYaw ) );
        m_fLightPitch = min( +D3DX_PI/4, max( -D3DX_PI/4, m_fLightPitch ) );

        // Transform the light position
        D3DXMATRIX matTrans, matRotateX, matRotateY, matRotateLight;
        D3DXMatrixRotationX( &matRotateX,  m_fLightPitch );
        D3DXMatrixRotationY( &matRotateY, -m_fLightYaw );
        D3DXMatrixMultiply( &matRotateLight, &matRotateY, &matRotateX );
        D3DXMatrixTranslation( &matTrans, 0, 0, -0.4f * m_fCameraDistance );
        D3DXMatrixMultiply( &m_matLightMatrix, &matTrans, &matRotateLight );

        // Transform the light direction
        D3DXVec3TransformNormal( &m_LightDir, &D3DXVECTOR3(0,0,1), &matRotateLight );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SetBRDFShaderConstants()
// Desc: Sets up vertex shading constants for the BRDF calculations
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::SetBRDFShaderConstants()
{
    // Concat the matrices
    D3DXMATRIX matWorld, matWorldView, matWorldViewProj;
    matWorld = m_matWomanMatrix;
    D3DXMatrixMultiply( &matWorldView,     &matWorld,     &m_matView );
    D3DXMatrixMultiply( &matWorldViewProj, &matWorldView, &m_matProj );

    D3DXMatrixTranspose( &matWorld,         &matWorld );
    D3DXMatrixTranspose( &matWorldView,     &matWorldView );
    D3DXMatrixTranspose( &matWorldViewProj, &matWorldViewProj );

    // Put light in object space
    D3DXMATRIX matLightInverse;
    D3DXMatrixInverse( &matLightInverse, NULL, &m_matWomanMatrix );
    matLightInverse._41 = matLightInverse._42 = matLightInverse._43 = 0.0f;

    D3DXVECTOR3 vLightDir;
    D3DXVec3TransformCoord( &vLightDir, &m_LightDir, &matLightInverse );

    // Set light for vertex shader computations
    m_vVertexShaderLightDir.x = -vLightDir.x;
    m_vVertexShaderLightDir.y = -vLightDir.y;
    m_vVertexShaderLightDir.z = -vLightDir.z;
    m_vVertexShaderLightDir.w = 1.0f;

    // Set light for pixel shader computations. Bias and scale the light vector,
    // because the pixel shader will clamp values to [0,1].
    m_vPixelShaderLightDir.x = 0.5f * ( 1.0f - vLightDir.x );
    m_vPixelShaderLightDir.y = 0.5f * ( 1.0f - vLightDir.y );
    m_vPixelShaderLightDir.z = 0.5f * ( 1.0f - vLightDir.z );
    m_vPixelShaderLightDir.w = 1.0f;

    // Put eye in object space
    D3DXMATRIX matEyeInverse;
    D3DXMatrixInverse( &matEyeInverse, NULL, &m_matWomanMatrix );

    D3DXVECTOR4 vEyePos( 0.0f, 0.0f, 0.0f, 1.0f );
    D3DXVec3TransformCoord( (D3DXVECTOR3*)&vEyePos, &m_vEyePt, &matEyeInverse );

    // Constant 1.0f
    D3DXVECTOR4 v1( 1.0f, 1.0f, 1.0f, 1.0f );

    // Set vertex shader constants
    m_pd3dDevice->SetVertexShaderConstant(  0, &m_vVertexShaderLightDir, 1 );
    m_pd3dDevice->SetVertexShaderConstant(  5, &vEyePos,          1 );
    m_pd3dDevice->SetVertexShaderConstant(  7, &m_WomanObject.GetMesh(0)->m_pSubsets[DRESS_MESHSUBSET].mtrl.Diffuse, 1 );
    m_pd3dDevice->SetVertexShaderConstant(  6, &v1,               1 );
    m_pd3dDevice->SetVertexShaderConstant( 50, &matWorldViewProj, 4 );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Sets up render states, clears the viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x000000ff, 1.0f, 0L );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );

    D3DLIGHT8 light;
    XBUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, m_LightDir.x, m_LightDir.y, m_LightDir.z );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Draw the spotlight using the fixed function pipeline
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matLightMatrix );
    m_pd3dDevice->SetPixelShader( NULL );
    m_LightObject.Render( m_pd3dDevice );

    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWomanMatrix );
    m_pd3dDevice->SetPixelShader( NULL );
    
    // Draw the woman object (everything but the dress)
    m_WomanObject.m_bRenderDress = FALSE;
    m_WomanObject.Render( m_pd3dDevice );

    // Now for the dress, Minnaert light it in second pass
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_NONE );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_MIPFILTER, D3DTEXF_NONE );

    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );

    SetBRDFShaderConstants();
    m_pd3dDevice->SetVertexShader( m_dwWomanVertexShader );
    
    if( m_bUseCustomLighting )
    {
        m_pd3dDevice->SetPixelShader( m_dwWomanPixelShader1 );
        
        m_pd3dDevice->SetTexture( 0, m_pNormalizationCubemap );
        m_pd3dDevice->SetTexture( 1, m_pNormalizationCubemap );
        m_pd3dDevice->SetTexture( 3, m_pNormalizationCubemap );

        switch( m_LightingEffect )
        {
            case MINNAERT: m_pd3dDevice->SetTexture( 2, m_pMinnaertTexture ); break;
            case SATIN:    m_pd3dDevice->SetTexture( 2, m_pSatinTexture );    break;
            case VELVET:   m_pd3dDevice->SetTexture( 2, m_pVelvetTexture );   break;
        }
    }
    else
    {
        m_pd3dDevice->SetPixelShader( m_dwWomanPixelShader2 );

        m_pd3dDevice->SetTexture( 0, m_pNormalizationCubemap );
        m_pd3dDevice->SetTexture( 1, m_pNormalizationCubemap );
        m_pd3dDevice->SetTexture( 2, m_pNormalizationCubemap );
        m_pd3dDevice->SetTexture( 3, m_pNormalizationCubemap );
    }

    // Pass the light direction to the pixel shader (via constant 0)
    m_pd3dDevice->SetPixelShaderConstant( 0, &m_vPixelShaderLightDir, 1 );
    
    // Draw the woman's dress
    m_WomanObject.m_bRenderDress = TRUE;
    m_WomanObject.Render( m_pd3dDevice, XBMESH_NOFVF|XBMESH_NOTEXTURES );

    // Reset states
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

    // Show the custom lighting texture
    if( m_bUseCustomLighting )
    {
        struct SCREENVERTEX { D3DXVECTOR4 p; FLOAT tu, tv; };
        SCREENVERTEX v[4];
        v[0].p = D3DXVECTOR4(  0+64 - 0.5f,  0+100 - 0.5f, 1.0f, 1.0f );  v[0].tu = 0.0f; v[0].tv = 0.0f; 
        v[1].p = D3DXVECTOR4( 64+64 - 0.5f,  0+100 - 0.5f, 1.0f, 1.0f );  v[1].tu = 1.0f; v[1].tv = 0.0f; 
        v[2].p = D3DXVECTOR4(  0+64 - 0.5f, 64+100 - 0.5f, 1.0f, 1.0f );  v[2].tu = 0.0f; v[2].tv = 1.0f; 
        v[3].p = D3DXVECTOR4( 64+64 - 0.5f, 64+100 - 0.5f, 1.0f, 1.0f );  v[3].tu = 1.0f; v[3].tv = 1.0f; 

        switch( m_LightingEffect )
        {
            case MINNAERT: m_pd3dDevice->SetTexture( 0, m_pMinnaertTexture ); break;
            case SATIN:    m_pd3dDevice->SetTexture( 0, m_pSatinTexture );    break;
            case VELVET:   m_pd3dDevice->SetTexture( 0, m_pVelvetTexture );   break;
        }

        // Render the texture
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
        m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
    }

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.DrawText(  64, 50, 0xffffff00, L"Minnaert" );
        m_Font.DrawText( 450, 50, 0xffffffff, m_strFrameRate );
    
        if( m_bUseCustomLighting )
        {
            switch( m_LightingEffect )
            {
                case MINNAERT: m_Font.DrawText(  64, 75, 0xffffffff, L"Minnaert lighting" ); break;
                case SATIN:    m_Font.DrawText(  64, 75, 0xffffffff, L"Satin lighting" );    break;
                case VELVET:   m_Font.DrawText(  64, 75, 0xffffffff, L"Velvet lighting" );   break;
            }
        }
        else
            m_Font.DrawText(  64, 75, 0xffffffff, L"Normal lighting" );
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



