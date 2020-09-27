//-----------------------------------------------------------------------------
// File: PaintEffect.cpp
//
// Desc: Full screen paint effect
//
// Hist: 10.26.01 - Created
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include "resource.h"

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle\nRaw Pixels" },
    { XBHELP_X_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle\nStroke Texture" },
    { XBHELP_Y_BUTTON,     XBHELP_PLACEMENT_2, L"Toggle\nFixed-Sized Strokes" },
    { XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Toggle\nWhite Paper" },
    { XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Select Parameter" },
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Move\nCamera" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Rotate\nCamera" },
    { XBHELP_MISC_CALLOUT, XBHELP_PLACEMENT_2, L"Triggers Tweak\nCurrent Parameter" },
    { XBHELP_LEFT_BUTTON,  XBHELP_PLACEMENT_2, L"Decrease\nParameter" },
    { XBHELP_RIGHT_BUTTON, XBHELP_PLACEMENT_2, L"Increase\nParameter" },
};

#define NUM_HELP_CALLOUTS ( sizeof( g_HelpCallouts ) / sizeof( g_HelpCallouts[0] ) )


//-----------------------------------------------------------------------------
// Name: struct PAINTVERTEX
// Desc: vertex structure used for rendering texture as pointsprites
//-----------------------------------------------------------------------------
struct PAINTVERTEX {
    D3DXVECTOR4 Position;   // screen space position
};

//-----------------------------------------------------------------------------
// Name: struct STROKEVERTEX
// Desc: Per-vertex parameters to give random variation to the stroke.
//       These could be attached to the model as a texture instead and
//       rendered into a separate buffer.
//-----------------------------------------------------------------------------
struct STROKEVERTEX {
    FLOAT PointSize;
};


//-----------------------------------------------------------------------------
// Name: class CPaint
// Desc: draws a texture as pointsprites to create a painted look
//-----------------------------------------------------------------------------
class CPaint {
public:
    LPDIRECT3DTEXTURE8 m_pPaintTexture; // render target for scene, source for point sprites colors
    LPDIRECT3DSURFACE8 m_pPaintSurface; // most detailed texture level
    D3DVertexBuffer m_ColorVertexBuffer;    // fake vertex buffer that uses a pointer to the texture data
    LPDIRECT3DSURFACE8 m_pPaintDepthBuffer; // depth buffer that's the same size as m_pPaintSurface
#define CENTERCOUNT 37
    LPDIRECT3DVERTEXBUFFER8 m_rpPaintVertexBuffer[CENTERCOUNT]; // vertex buffer that maps to centers of strokes
    DWORD m_iPaintVertexBuffer; // current paint vertex buffer, for randomly moving centers
#define STROKECOUNT 11  // make this relatively prime to CENTERCOUNT for maximal variation
    LPDIRECT3DVERTEXBUFFER8 m_rpStrokeVertexBuffer[STROKECOUNT];    // vertex buffer that maps to centers of strokes
    DWORD m_iStrokeVertexBuffer; // current stroke vertex buffer, for randomly moving stroke attributes
    LPDIRECT3DTEXTURE8 m_pStrokeTexture;    // texture for drawing strokes
    DWORD m_dwVertexShader;
    DWORD m_Width, m_Height;
    DWORD m_ScreenWidth, m_ScreenHeight;
    BOOL m_bStrokeSizePerVertex;    // use per-vertex PointSize, otherwise use 0.5f * m_fPointSizeScale + m_fPointSizeOffset
    FLOAT m_fPointSizeOffset;
    FLOAT m_fPointSizeScale;

    CPaint()
    {
        m_pPaintTexture = NULL;
        m_pPaintSurface = NULL;
        m_pPaintDepthBuffer = NULL;
        ZeroMemory(m_rpPaintVertexBuffer, sizeof(m_rpPaintVertexBuffer));
        m_iPaintVertexBuffer = 0;
        ZeroMemory(m_rpStrokeVertexBuffer, sizeof(m_rpStrokeVertexBuffer));
        m_iStrokeVertexBuffer = 0;
        m_pStrokeTexture = NULL;
        m_dwVertexShader = 0;
        m_bStrokeSizePerVertex = true;
    }

    ~CPaint()
    {
        SAFE_RELEASE(m_pPaintTexture);
        SAFE_RELEASE(m_pPaintSurface);
        SAFE_RELEASE(m_pPaintDepthBuffer);
        for (DWORD iPaintVB = 0; iPaintVB < CENTERCOUNT; iPaintVB++)
            SAFE_RELEASE(m_rpPaintVertexBuffer[iPaintVB]);
        for (DWORD iStrokeVB = 0; iStrokeVB < STROKECOUNT; iStrokeVB++)
            SAFE_RELEASE(m_rpPaintVertexBuffer[iStrokeVB]);
        SAFE_RELEASE(m_pStrokeTexture);
        if (m_dwVertexShader)
            g_pd3dDevice->DeleteVertexShader(m_dwVertexShader);
    }

    HRESULT SetStrokeTexture(LPDIRECT3DTEXTURE8 pStrokeTexture)
    {
        LPDIRECT3DTEXTURE8 pStrokeTextureOld = m_pStrokeTexture;
        m_pStrokeTexture = pStrokeTexture;
        m_pStrokeTexture->AddRef();
        SAFE_RELEASE(pStrokeTextureOld);
        return S_OK;
    }

    HRESULT Initialize(UINT Width, UINT Height, LPDIRECT3DTEXTURE8 pStrokeTexture, UINT ScreenWidth, UINT ScreenHeight)
    {
        HRESULT hr;
        m_Width = Width;
        m_Height = Height;
        m_ScreenWidth = ScreenWidth;
        m_ScreenHeight = ScreenHeight;
        
        // Create the paint texture and get a pointer to the main surface image
        D3DFORMAT Format = D3DFMT_A8R8G8B8; // must use swizzled format texture, otherwise memory may have a stride
        SAFE_RELEASE(m_pPaintSurface);
        SAFE_RELEASE(m_pPaintTexture);
        hr = g_pd3dDevice->CreateTexture(Width, Height, 1, D3DUSAGE_RENDERTARGET, Format, 0, &m_pPaintTexture);
        if (FAILED(hr))
            return hr;
        hr = m_pPaintTexture->GetSurfaceLevel(0, &m_pPaintSurface);
        if (FAILED(hr))
            return hr;
        
        // make a depth buffer that is the same size as the texture
        SAFE_RELEASE(m_pPaintDepthBuffer);
        hr = g_pd3dDevice->CreateDepthStencilSurface(Width, Height, D3DFMT_LIN_D24S8, D3DMULTISAMPLE_NONE, &m_pPaintDepthBuffer);
        if (FAILED(hr))
            return hr;

        // Make a fake vertex buffer header that points to the texture data
        ZeroMemory( &m_ColorVertexBuffer, sizeof(D3DVertexBuffer) );
        XGSetVertexBufferHeader( Width * Height * 4, 0, D3DFVF_DIFFUSE, 0, &m_ColorVertexBuffer, m_pPaintSurface->Data);

        // Fill in the vertex buffer with output pixel offsets, accounting for
        // swizzling of the source paint texture
        FLOAT fXScale = (FLOAT)ScreenWidth / (FLOAT)Width;
        FLOAT fYScale = (FLOAT)ScreenHeight / (FLOAT)Height;
        FLOAT fXOffset = 0.f;
        FLOAT fYOffset = 0.f;
        Swizzler s(Width, Height, 0);
        UINT iVB;
        for (iVB = 0; iVB < CENTERCOUNT; iVB++)
        {
            // Create a vertex buffer that matches the paint texture size on input and maps to the screen on output
            SAFE_RELEASE(m_rpPaintVertexBuffer[iVB]);
            hr = g_pd3dDevice->CreateVertexBuffer( Width * Height * sizeof(PAINTVERTEX), 0, 0, D3DPOOL_DEFAULT, &m_rpPaintVertexBuffer[iVB]);
            if (FAILED(hr))
                return hr;
#define frand(a) ((FLOAT)rand()*(a)/32768.0f)

            PAINTVERTEX *pv;
            hr = m_rpPaintVertexBuffer[iVB]->Lock( 0, 0, (BYTE**)&pv, 0 );
            if (FAILED(hr))
                return E_FAIL;
            s.SetV(s.SwizzleV(0));
            s.SetU(s.SwizzleU(0));
            for (UINT iy = 0; iy < Height; iy++)
            {
                for (UINT ix = 0; ix < Width; ix++)
                {
                    UINT TexelOffset = s.Get2D();
                    PAINTVERTEX *p = pv + TexelOffset;
                    p->Position.x = fXScale * (ix + frand(1.f)) + fXOffset;
                    p->Position.y = fYScale * (iy + frand(1.f)) + fYOffset;
                    p->Position.z = 0.5f;
                    p->Position.w = 1.f;
                    s.IncU();
                }
                s.IncV();
            }
            m_rpPaintVertexBuffer[iVB]->Unlock();
        }

        // Set point scale and offset
        m_fPointSizeScale = 1.75f * 0.5f * (fXScale + fYScale);
        m_fPointSizeOffset = 0.25f * m_fPointSizeScale;

        // Fill in vertex buffers with stroke attributes
        for (iVB = 0; iVB < STROKECOUNT; iVB++)
        {
            // Create a vertex buffer that matches the paint texture size on input and maps to stroke attributes
            SAFE_RELEASE(m_rpStrokeVertexBuffer[iVB]);
            hr = g_pd3dDevice->CreateVertexBuffer( Width * Height * sizeof(STROKEVERTEX), 0, 0, D3DPOOL_DEFAULT, &m_rpStrokeVertexBuffer[iVB]);
            if (FAILED(hr))
                return hr;

            // Fill in the vertex buffer with output pixel offsets, accounting for
            // swizzling of the source stroke texture
            STROKEVERTEX *psv;
            hr = m_rpStrokeVertexBuffer[iVB]->Lock( 0, 0, (BYTE**)&psv, 0 );
            if (FAILED(hr))
                return E_FAIL;
            s.SetV(s.SwizzleV(0));
            s.SetU(s.SwizzleU(0));
            for (UINT iy = 0; iy < Height; iy++)
            {
                for (UINT ix = 0; ix < Width; ix++)
                {
                    UINT TexelOffset = s.Get2D();
                    STROKEVERTEX *p = psv + TexelOffset;
                    p->PointSize = frand(1.f);
                    s.IncU();
                }
                s.IncV();
            }
            m_rpStrokeVertexBuffer[iVB]->Unlock();
        }
        
        // Copy the stroke texture pointer
        SetStrokeTexture(pStrokeTexture);

        // Create the vertex shader
        if (m_dwVertexShader)
            g_pd3dDevice->DeleteVertexShader(m_dwVertexShader);
        DWORD vsdecl[] = 
        {
            D3DVSD_STREAM(0),
            D3DVSD_REG(0, D3DVSDT_FLOAT4),      // vertex
            D3DVSD_STREAM(1),
            D3DVSD_REG(1, D3DVSDT_D3DCOLOR),    // color
            D3DVSD_STREAM(2),
            D3DVSD_REG(2, D3DVSDT_FLOAT1),      // texture W coord
            D3DVSD_REG(3, D3DVSDT_FLOAT1),      // scale
            D3DVSD_END()
        };
        XBUtil_CreateVertexShader(g_pd3dDevice, "Shaders\\paint.xvu", vsdecl, &m_dwVertexShader );
        if (m_dwVertexShader == 0)
            return E_FAIL;
                
        return S_OK;
    }
    
    //-----------------------------------------------------------------------------
    // Name: DrawPointSprites
    // Desc:  
    //-----------------------------------------------------------------------------
    HRESULT DrawPointSprites()
    {
        // Set the render states for using point sprites. 
        // Note that pointsprites use texture stage 3 only.
        g_pd3dDevice->SetPixelShader( 0 );
        g_pd3dDevice->SetTexture( 0, NULL );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
        g_pd3dDevice->SetTexture( 1, NULL );
        g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
        g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
        g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
        g_pd3dDevice->SetTexture( 2, NULL );
        g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_CURRENT );
        g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
        g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_CURRENT );

        g_pd3dDevice->SetTexture( 3, m_pStrokeTexture );
        static DWORD dwColorOp = D3DTOP_MODULATE;
        static DWORD dwColorArg1 = D3DTA_TEXTURE;
        static DWORD dwColorArg2 = D3DTA_CURRENT;
        g_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   dwColorOp );
        g_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG1, dwColorArg1 );
        g_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG2, dwColorArg2 );
        g_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
        g_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        g_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
        
        static D3DCOLOR TFACTOR = 0xffffffff;
        g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, TFACTOR );
        
        g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
        g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
        g_pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
    
        // Turn on alphablending
        g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_ONE );
        g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x1 );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );

        // Turn on point sprites
        g_pd3dDevice->SetVertexShader( m_dwVertexShader );
        D3DXVECTOR4 vPointSize(m_fPointSizeScale, m_fPointSizeOffset, 0.f, 0.f);
        g_pd3dDevice->SetVertexShaderConstant( 0, &vPointSize, 1 );
        g_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
        FLOAT fDefaultPointSize = m_fPointSizeScale * 0.5f + m_fPointSizeOffset;
        g_pd3dDevice->SetRenderState( D3DRS_POINTSIZE,         FtoDW(fDefaultPointSize) );
        g_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  m_bStrokeSizePerVertex );
        static FLOAT fA = 1.f;
        static FLOAT fB = 0.f;
        static FLOAT fC = 0.f;
        g_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,      FtoDW(fA) );
        g_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,      FtoDW(fB) );
        g_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,      FtoDW(fC) );

        
        // Render particles
        g_pd3dDevice->SetStreamSource( 0, m_rpPaintVertexBuffer[m_iPaintVertexBuffer], sizeof(PAINTVERTEX));
        g_pd3dDevice->SetStreamSource( 1, &m_ColorVertexBuffer, sizeof(DWORD) );        // fake vertex buffer from texture
        g_pd3dDevice->SetStreamSource( 2, m_rpStrokeVertexBuffer[m_iStrokeVertexBuffer], sizeof(STROKEVERTEX));
        g_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, 0, m_Width * m_Height);   // lots of points
    
        // Reset render states
        g_pd3dDevice->SetTexture( 3, NULL );
        g_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
        g_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,  FALSE );
        return S_OK;
    }
    
    //-----------------------------------------------------------------------------
    // Name: DrawQuad
    // Desc:  
    //-----------------------------------------------------------------------------
    HRESULT DrawQuad()
    {
        struct BACKGROUNDVERTEX { D3DXVECTOR4 p; FLOAT tu, tv; } v[4];
        v[0].p = D3DXVECTOR4( -0.5f,                -0.5f,                 1.f, 1.f ); v[0].tu = 0.f; v[0].tv = 0.f;
        v[1].p = D3DXVECTOR4( m_ScreenWidth - 0.5f, -0.5f,                 1.f, 1.f ); v[1].tu = 1.f; v[1].tv = 0.f;
        v[2].p = D3DXVECTOR4( -0.5f,                m_ScreenHeight - 0.5f, 1.f, 1.f ); v[2].tu = 0.f; v[2].tv = 1.f;
        v[3].p = D3DXVECTOR4( m_ScreenWidth - 0.5f, m_ScreenHeight - 0.5f, 1.f, 1.f ); v[3].tu = 1.f; v[3].tv = 1.f;
    
        // Set states
        g_pd3dDevice->SetPixelShader( 0 );
        g_pd3dDevice->SetTexture( 0, m_pPaintTexture );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
        static DWORD dwColorArg1 = D3DTA_TEXTURE;
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, dwColorArg1 );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
        g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
        static D3DCOLOR colorBlend = 0xffffffff;
        g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, colorBlend );
        g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
        g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE ); 
        g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
        g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
        g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);

        // Render the screen-aligned quadrilateral
        g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
        g_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADSTRIP, 1, v, sizeof(BACKGROUNDVERTEX) );
        
        // Reset render states
        g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
        g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
        g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
        g_pd3dDevice->SetTexture( 0, NULL );
        g_pd3dDevice->SetTexture( 1, NULL );
        g_pd3dDevice->SetPixelShader( 0 );
        
        return S_OK;
    }

};


//-----------------------------------------------------------------------------
// Vertex for cylinder drawing
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position;   // The position
    D3DXVECTOR3 normal;     // The vertex normals
    FLOAT tu, tv;           // texture coords
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
    CXBPackedResource  m_xprResource;        // Packed resources for the app
    CXBFont     m_Font;             // Font object
    CXBHelp     m_Help;             // Help object
    BOOL        m_bDrawHelp;        // TRUE to draw help screen
    BOOL        m_bClearPaper;      // TRUE to clear the paper before drawing point sprites
    BOOL        m_bDrawQuad;        // TRUE to draw the paint texture full screen
    FLOAT       m_fTimeScale;       // Target animation frame rate
    enum {
        TWEAK_TIME_SCALE,			// adjust rate of stepping through random center position VB's
        TWEAK_STROKE_SIZE,			// adjust both stroke size offset and scale
        TWEAK_STROKE_SIZE_SCALE,	// adjust only stroke size scale
    } m_TweakMode;                  // tweak mode detrmines what triggers do
    FLOAT           m_TweakActive;      // tweaking in progess if > zero
    D3DXVECTOR3     m_vFrom, m_vAt, m_vUp;  // Viewing parameters
    D3DXMATRIX      m_matWorld;
    D3DXMATRIX      m_matView;
    D3DXMATRIX      m_matViewInverse;
    D3DXMATRIX      m_matProjection;

    // Generic scene
    D3DVertexBuffer *m_pCylinderVB; // Buffer for cylinder vertices, which is the basic object
    UINT m_nObjectCount;
    struct Object {                 // put a few objects around in the scene
        D3DXVECTOR3 m_vPosition;
        D3DXCOLOR m_color;
        UINT m_iTexture;            // texture index
    } *m_rObjects;
#define TEXTURE_COUNT 3
    D3DTexture *m_rpTexture[TEXTURE_COUNT]; // object textures

    // Paint effect
    CPaint m_Paint;

    // Constructor
    CXBoxSample();

    // Overrides of XBApp virtuals
    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();

    // Helpers
    HRESULT SetCameraTransformations();
    HRESULT RenderToPaintTexture();
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
#ifdef _DEBUG
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;    // Allow unlimited frame rate
#endif
    m_bDrawHelp = FALSE;
    m_bClearPaper = FALSE;
    m_bDrawQuad = FALSE;
    m_fTimeScale = 12.f; // Target animation frame rate
    m_TweakMode = TWEAK_STROKE_SIZE;
    m_TweakActive = 0.f;
}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    HRESULT hr;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", resource_NUM_RESOURCES, NULL ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Get the texture pointers
    m_rpTexture[0] = m_xprResource.GetTexture( resource_Texture0_OFFSET );
    m_rpTexture[1] = m_xprResource.GetTexture( resource_Texture1_OFFSET );
    m_rpTexture[2] = m_xprResource.GetTexture( resource_Checker_OFFSET );

    // Initialize the Paint effect
    srand(0x123456);
    LPDIRECT3DTEXTURE8 pStrokeTexture = m_xprResource.GetTexture( resource_Stroke_OFFSET );
    hr = m_Paint.Initialize(64, 64, pStrokeTexture, m_d3dpp.BackBufferWidth, m_d3dpp.BackBufferHeight);
    if (FAILED(hr))
        return hr;
    
    // Create geometry for a cylinder
#define NSAMPLE 25
   if( FAILED( m_pd3dDevice->CreateVertexBuffer( NSAMPLE*2*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &m_pCylinderVB ) ) )
        return E_FAIL;
   
    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    CUSTOMVERTEX* v;
    if( FAILED( m_pCylinderVB->Lock( 0, 0, (BYTE**)&v, 0 ) ) )
        return E_FAIL;
    for(UINT i=0; i<NSAMPLE; i++ )
    {
        FLOAT theta = (2*D3DX_PI*i)/(NSAMPLE-1);

        v[2*i+0].position = D3DXVECTOR3( sinf(theta),-1.2f, cosf(theta) );
        v[2*i+0].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );
        v[2*i+0].tu = ((FLOAT)i*2.0f)/(NSAMPLE-1);
        v[2*i+0].tv = 1.0f;
        
        v[2*i+1].position = D3DXVECTOR3( sinf(theta), 1.2f, cosf(theta) );
        v[2*i+1].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );
        v[2*i+1].tu = ((FLOAT)i*2.0f)/(NSAMPLE-1);
        v[2*i+1].tv = 0.0f;
    }
    m_pCylinderVB->Unlock();

    // Position the objects
    m_nObjectCount = 100;
    m_rObjects = new Object [ m_nObjectCount ];
    if (m_rObjects == NULL)
        return E_OUTOFMEMORY;
#define irand(a) ((rand()*(a))>>15)
#define frand(a) ((FLOAT)rand()*(a)/32768.0f)
    FLOAT fScale = 100.f;
    srand(123456);
    for (UINT i = 0; i < m_nObjectCount; i++)
    {
        m_rObjects[i].m_vPosition = D3DXVECTOR3( frand(fScale), 0.f, frand(fScale));
        m_rObjects[i].m_color = D3DXCOLOR( frand(1.f), frand(1.f), frand(1.f), 1.f);
        m_rObjects[i].m_iTexture = irand(TEXTURE_COUNT);
    }
    
    // Set camera parameters and initialize camera matrices
    m_vAt = m_rObjects[0].m_vPosition;
    m_vFrom = m_vAt - D3DXVECTOR3(8.f, -0.1f, 0.f);
    m_vUp = D3DXVECTOR3( 0.0f, 1.0f , 0.0f);
    SetCameraTransformations();

    // Setup the light
    D3DLIGHT8 light;
    ZeroMemory( &light, sizeof(D3DLIGHT8) );
    light.Type         = D3DLIGHT_DIRECTIONAL;
    light.Ambient      = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f);
    light.Diffuse      = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
    light.Direction    = D3DXVECTOR3( 1.f, 1.f, 1.f );
    light.Range        = 1000.0f;
    light.Attenuation0 = 1.0f;
    g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SetCameraTransformations()
// Desc: Calculate camera matrices and set transformation state
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::SetCameraTransformations()
{
    // Set world matrix to identity
    D3DXMatrixIdentity(&m_matWorld);
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    
    // Set our view matrix.
    D3DXMatrixLookAtLH( &m_matView, &m_vFrom, &m_vAt, &m_vUp);
    D3DXMatrixInverse( &m_matViewInverse, NULL, &m_matView);
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );

    // Set projection 
    static FLOAT fNear = 1.f;
    static FLOAT fFar = 50.f;
    D3DXMatrixPerspectiveFovLH( &m_matProjection, D3DX_PI/4, 640.f / 480.f, fNear, fFar);
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjection );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Toggle quad drawing to show raw pixels or painterly effect
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
    {
        m_bDrawQuad = !m_bDrawQuad;
    }

    // Switch to next stroke texture
    if (m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X])
    {
        static UINT rStrokeOffset[] = {
            resource_Stroke_OFFSET,
            resource_StrokeCoffee_OFFSET,
        };
        static UINT nStrokeCount = sizeof(rStrokeOffset) / sizeof(UINT);
        
        // find current stroke texture
        UINT iStroke;
        for (iStroke = 0; iStroke < nStrokeCount; iStroke++)
            if (m_Paint.m_pStrokeTexture == m_xprResource.GetTexture( rStrokeOffset[iStroke] ))
                break;
        
        // go to next
        if (iStroke >= nStrokeCount - 1)
            iStroke = 0;
        else
            iStroke++;
        m_Paint.SetStrokeTexture( m_xprResource.GetTexture( rStrokeOffset[iStroke] ));
    }

    // Toggle paper clearing
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE] )
    {
        m_bClearPaper = !m_bClearPaper;
    }

    // Toggle stroke size per vertex
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
    {
        m_Paint.m_bStrokeSizePerVertex = !m_Paint.m_bStrokeSizePerVertex;
        
        // Set tweak mode according to new stroke-size per vertex setting
        if (m_Paint.m_bStrokeSizePerVertex)
            m_TweakMode = TWEAK_STROKE_SIZE_SCALE;
        else
            m_TweakMode = TWEAK_STROKE_SIZE;
        m_TweakActive = 2.f;
    }

    // Set tweak parameter
    if ( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP 
         || m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN 
         || m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT 
         || m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
        m_TweakActive = 2.f; // show tweak display when DPAD is pressed
    if ( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT
		 || m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
    {
        if (m_TweakMode == TWEAK_TIME_SCALE)
            m_TweakMode = TWEAK_STROKE_SIZE;
		else
			m_TweakMode = TWEAK_TIME_SCALE;
	}
    if ( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP
		 || m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
	{
		if (m_TweakMode == TWEAK_STROKE_SIZE
			&& m_Paint.m_bStrokeSizePerVertex) // enter stroke scale mode only if enabled
			m_TweakMode = TWEAK_STROKE_SIZE_SCALE;
		else
			m_TweakMode = TWEAK_STROKE_SIZE;
	}

    // Tweak stroke size offset and scale, or time scale
    int delta = m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] 
        - m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER];
    if (delta)
    {
        m_TweakActive = 1.f;
        if (m_TweakMode == TWEAK_TIME_SCALE)
        {
            static FLOAT fDeltaScale = 0.001f;
            FLOAT fScale = 1.f + m_fElapsedTime * delta * fDeltaScale;
            m_fTimeScale *= fScale;
        }
        else if (m_TweakMode == TWEAK_STROKE_SIZE)
        {
            static FLOAT fDeltaScale = 0.001f;
            FLOAT fScale = 1.f + m_fElapsedTime * delta * fDeltaScale;
            m_Paint.m_fPointSizeOffset *= fScale;
            m_Paint.m_fPointSizeScale *= fScale;    // tweak both to scale mean
        }
        else if (m_TweakMode == TWEAK_STROKE_SIZE_SCALE)
        {
            static FLOAT fDeltaScale = 0.001f;
            FLOAT fScale = 1.f + m_fElapsedTime * delta * fDeltaScale;
            m_Paint.m_fPointSizeScale *= fScale;    // moves mean, but always above offset
        }
    }
    else if (m_TweakActive > 0.f)   // keep the tweaking debug display active for a while
        m_TweakActive -= m_fElapsedTime;

    // Choose current paint vertex buffers, to give the impression of random center positions and stroke sizes
    UINT iOldPaintVB = m_Paint.m_iPaintVertexBuffer;
    UINT iOldStrokeVB = m_Paint.m_iStrokeVertexBuffer;
    m_Paint.m_iPaintVertexBuffer = (UINT)fmod(m_fAppTime * m_fTimeScale, CENTERCOUNT);
    if (m_Paint.m_iPaintVertexBuffer >= CENTERCOUNT)
        m_Paint.m_iPaintVertexBuffer = 0;
    m_Paint.m_iStrokeVertexBuffer = (UINT)fmod(m_fAppTime * m_fTimeScale, STROKECOUNT);
    if (m_Paint.m_iStrokeVertexBuffer >= STROKECOUNT)
        m_Paint.m_iStrokeVertexBuffer = 0;

    // update view position
    static FLOAT fOffsetScale = 3.f;
    FLOAT fX1 = m_DefaultGamepad.fX1;
    fX1 *= fX1 * fX1; // fX1 cubed
    FLOAT fY1 = m_DefaultGamepad.fY1;
    fY1 *= fY1 * fY1; // fY1 cubed
    D3DXVECTOR3 vOffset(fX1, 0.f, fY1); // screen space offset, X moves left-right, Y moves in-out in depth
    D3DXVec3TransformNormal(&vOffset, &vOffset, &m_matViewInverse);
    D3DXVec3Normalize(&m_vUp, &m_vUp);
    vOffset -= D3DXVec3Dot(&vOffset, &m_vUp) * m_vUp; // don't move up or down with thumb sticks
    D3DXVec3Normalize(&vOffset, &vOffset);
    vOffset *= fOffsetScale * m_fElapsedTime;
    m_vFrom += vOffset;
    m_vAt += vOffset;

    // update view angle
    static FLOAT fAtOffsetScale = 8.f;
    D3DXVECTOR3 vAtOffset(0.f, 0.f, 0.f);
    FLOAT fX2 = m_DefaultGamepad.fX2;
    fX2 *= fX2 * fX2; // fX2 cubed
    FLOAT fY2 = m_DefaultGamepad.fY2;
    fY2 *= fY2 * fY2; // fY2 cubed
    vAtOffset.x += fAtOffsetScale * fX2 * m_fElapsedTime;
    D3DXVECTOR3 vE = m_vAt - m_vFrom;
    D3DXVec3Normalize(&vE, &vE);
    FLOAT fThreshold = 0.99f;
    FLOAT fEdotU = D3DXVec3Dot(&vE, &m_vUp);
    if ((fEdotU < -fThreshold && fY2 < 0.f) // near -vUp, but positive movement
        || (fEdotU > fThreshold && fY2 > 0.f)   // near vUp, but negative movement
        || (fEdotU > -fThreshold && fEdotU < fThreshold))       // ordinary case
        vAtOffset.y -= fAtOffsetScale * fY2 * m_fElapsedTime;   // screen-space Y displacement means up-down view turn
    D3DXVec3TransformNormal(&vAtOffset, &vAtOffset, &m_matViewInverse);
    m_vAt += vAtOffset;

    SetCameraTransformations();
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: RenderToPaintTexture
// Desc: Renders the scene to a texture
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderToPaintTexture()
{
    // Set texture as render target
    g_pd3dDevice->SetRenderTarget(m_Paint.m_pPaintSurface, m_Paint.m_pPaintDepthBuffer);

    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 ); // This will render just the upper left corner of the regular gradient texture

    // Set default state
    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTexture( 1, NULL );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTexture( 2, NULL );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTexture( 3, NULL );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,         TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,    TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

    // Render some geometry
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetStreamSource( 0, m_pCylinderVB, sizeof(CUSTOMVERTEX) );
    m_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
    D3DXMATRIX matWorld;
    D3DMATERIAL8 material;
    XBUtil_InitMaterial( material, 1.0f, 1.0f, 1.0f );
    for (UINT i = 0; i < m_nObjectCount; i++)
    {
        m_pd3dDevice->SetTexture(0, m_rpTexture[m_rObjects[i].m_iTexture] );
        material.Diffuse = material.Ambient = m_rObjects[i].m_color;
        m_pd3dDevice->SetMaterial( &material );
        D3DXMatrixTranslation(&matWorld, m_rObjects[i].m_vPosition.x, m_rObjects[i].m_vPosition.y, m_rObjects[i].m_vPosition.z);
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*NSAMPLE-2 );
    }
    
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Draw the scene to a texture
    RenderToPaintTexture();

    // Set the render target to the backbuffer
    m_pd3dDevice->SetRenderTarget(m_pBackBuffer, m_pDepthBuffer);
    
    // Use the texture to draw a painterly rendering of the scene
    if (m_bClearPaper)
        m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0L );
    if (m_bDrawQuad)
        m_Paint.DrawQuad();
    else
        m_Paint.DrawPointSprites();

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"PaintEffect" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        if (m_TweakActive > 0.f)
        {
            CONST INT buflen = 200;
            WCHAR buf[buflen];
            if (m_TweakMode == TWEAK_TIME_SCALE)
            {
                m_Font.DrawText(64, 80, 0xff00ff00, L"Animation");
                _snwprintf(buf, buflen, L"rate=%f fps", m_fTimeScale);
                m_Font.DrawText(64, 110, 0xff66ff33, buf);
            }
            else if (!m_Paint.m_bStrokeSizePerVertex)
            {
                FLOAT fFixedPointSize = m_Paint.m_fPointSizeScale * 0.5f + m_Paint.m_fPointSizeOffset;
                m_Font.DrawText(64, 80, 0xff00ff00, L"Stroke Size, fixed");
                _snwprintf(buf, buflen, L"size=%f", fFixedPointSize);
                m_Font.DrawText(64, 110, 0xff66ff33, buf);
            }
            else
            {
                DWORD color1, color2;
                if (m_TweakMode == TWEAK_STROKE_SIZE)
                {
                    color1 = 0xff66ff33;
                    color2 = 0xff00ff00;
                }
                else
                {
                    color1 = 0xff00ff00;
                    color2 = 0xff66ff33;
                }
                m_Font.DrawText(64, 80, 0xff00ff00, L"Stroke Size, offset + random * scale");
                _snwprintf(buf, buflen, L"offset=%f", m_Paint.m_fPointSizeOffset);
                m_Font.DrawText(64, 110, color1, buf);
                _snwprintf(buf, buflen, L"scale=%f", m_Paint.m_fPointSizeScale);
                m_Font.DrawText(64, 140, color2, buf);
            }
        }
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}
