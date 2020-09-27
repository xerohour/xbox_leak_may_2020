//-----------------------------------------------------------------------------
// File: FocusBlur.cpp
//
// Desc: Two pass focus blur effect.  First, use the backbuffer as a
//   	 texture and draw to a separate blur texture with a blur pixel
//   	 shader.  For computing the blurred back buffer, this sample
//   	 shows several different filters of varying visual quality and
//   	 performance. Second, use the depthbuffer as a texture to
//   	 choose a range of z values to show in sharp focus vs blurry
//   	 focus.
//
// Copyright (c) 2001-2002 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBUtil.h>

// The following header file is generated from "Resource.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (Resource.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "Resource.h"

#ifndef ASSERT
#define ASSERT(X) { if (!(X)) { __asm int 3 } }
#endif

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_1, L"Move camera" },
    { XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Rotate camera" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Help" },
    { XBHELP_START_BUTTON,  XBHELP_PLACEMENT_2, L"Timing\nBaseline" },
    { XBHELP_A_BUTTON,  XBHELP_PLACEMENT_2, L"Next\nMode" },
    { XBHELP_B_BUTTON,  XBHELP_PLACEMENT_2, L"Previous\nMode" },
    { XBHELP_X_BUTTON,  XBHELP_PLACEMENT_2, L"Next\nFilter" },
    { XBHELP_Y_BUTTON,  XBHELP_PLACEMENT_2, L"Previous\nFilter" },
    { XBHELP_WHITE_BUTTON,  XBHELP_PLACEMENT_2, L"Filter\nComparison" },
    { XBHELP_BLACK_BUTTON,  XBHELP_PLACEMENT_2, L"Depth\nMode" },
    { XBHELP_LEFT_BUTTON,   XBHELP_PLACEMENT_1, L"Focus -" },
    { XBHELP_RIGHT_BUTTON,  XBHELP_PLACEMENT_1, L"Focus +" },
    
};
#define NUM_HELP_CALLOUTS (sizeof(g_HelpCallouts)/sizeof(XBHELP_CALLOUT))


//-----------------------------------------------------------------------------
// Vertex type for the cylinder objects in the simple scene.
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position;   // The position
    D3DXVECTOR3 normal;     // The vertex normals
    float tu, tv;           // texture coords
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

//-----------------------------------------------------------------------------
// A filter sample holds a subpixel offset and a filter value
// to be multiplied by a source texture to compute an arbitrary
// filter.  See FilterCopy for more details.
//-----------------------------------------------------------------------------
struct FilterSample {
    float fValue;               // coefficient
    float fOffsetX, fOffsetY;   // subpixel offsets of supersamples in destination coordinates
};

//-----------------------------------------------------------------------------
// The depth-mapping pixel shader attempts to do higher precision
// math with eight-bit color registers.  The _x4 instruction modifier
// is used twice to get a 16x range of values.
//-----------------------------------------------------------------------------
float g_fPixelShaderScale = 16.f;   // to get into the right range, we scale up the value in the pixel shader

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBPackedResource  m_xprResource;        // Packed resources for the app
    CXBFont      m_Font;
    CXBHelp      m_Help;
    bool         m_bDrawHelp;
    FLOAT        m_fFPSBase; // frames-per-second starting point for comparing the efficiency of different techniques

    //////////////////////////////////////////////////////////////////////
    // To demonstrate the focus blur effect, this sample draws a simple
    // scene with texture-mapped cylinders.
    // 
    D3DVertexBuffer *m_pCylinderVB;          // Buffer for cylinder vertices, which is the basic object
    UINT m_nObjectCount;
    struct Object {                         // put a few objects around in the scene
        D3DXVECTOR3 m_vPosition;
        D3DXCOLOR m_color;
        UINT m_iTexture;                    // texture index
    } *m_rObjects;
#define TEXTURE_COUNT 3
    D3DTexture *m_rpTexture[TEXTURE_COUNT]; // object textures, use several for variety
    
    //////////////////////////////////////////////////////////////////////
    // Current viewing parameters
    //
    D3DXVECTOR3     m_vFrom, m_vAt, m_vUp;  // Viewing parameters
    D3DXMATRIX      m_matWorld;
    D3DXMATRIX      m_matView;
    D3DXMATRIX      m_matViewInverse;
    D3DXMATRIX      m_matProjection;
    
    //////////////////////////////////////////////////////////////////////
    // Some of the blur filters are multipass and need temporary space.
    //
#define BLUR_COUNT 5
    D3DTexture *m_rpBlur[BLUR_COUNT];   // textures of decreasing size, used for blurring the backbuffer
    D3DTexture *m_pBlur;                // current blur texture, set by Blur function

    //////////////////////////////////////////////////////////////////////
    // Techniques for mapping the z-buffer to the in-focus range
    //
    enum DEPTHMODE {
        DM_RAW,         // show the raw z-buffer for demonstration purposes
        DM_RANGE,       // use arithmetic in the pixel shader to map z 
        DM_LOOKUP,      // use a dependent-texture read to map z
        _DM_MAX } m_eDepthMode;
    
    //////////////////////////////////////////////////////////////////////
    // Overall modes for the sample.
    // 
    enum EFFECTMODE { 
        EM_NOEFFECT,    // draw the basic scene, with no focus effect
        EM_BLUR,        // show the blurred back-buffer texture
        EM_DEPTH,       // show the mapping of z-values to in-focus areas
        EM_FOCUS,       // the full focus effect
        _EM_MAX } m_eEffectMode;
	
    //////////////////////////////////////////////////////////////////////
    // Enumeration of blur filters available in this sample to compare
    // the speed and quality of different types of blur filters for
    // the out-of-focus parts of the scene.
    //
    enum FILTERMODE {
        FM_VERT2_HORIZ2,
        FM_HORIZ2_VERT2,
        FM_VERT2_HORIZ,
        FM_HORIZ2_VERT,
        FM_IDENTITY,
        FM_BOX,
        FM_VERT,
        FM_HORIZ, 
        FM_BOX2,
        FM_VERT2,
        FM_HORIZ2,
        FM_BOX2_BOX2,
        FM_VERT2_HORIZ2_BOX2,
        FM_BOX2_BOX2_BOX2,
        FM_VERT2_HORIZ2_VERT2,
        FM_HORIZ2_VERT2_HORIZ2,
        FM_VERT2_HORIZ2_VERT2_HORIZ2,
        FM_HORIZ2_VERT2_HORIZ2_VERT2,
        FM_BOX2_BOX2_BOX2_BOX2,
        _FM_MAX
    } m_eFilterMode, m_eFilterMode2;    // keep two indices to visually compare the blur filters
	WCHAR *m_strFilterName;			// name of filter
	WCHAR *m_strFilterDescription;	// description of filter

    //////////////////////////////////////////////////////////////////////
    // Constants to choose the focus range. The lookup texture maps z
    // to focus values when DM_LOOKUP is active (for
    // FocusLookupPixelShader or DepthLookupPixelShader).
    //
    float m_fDepth0, m_fDepth1;         // range of depths to map
    float m_fFraction0, m_fFraction1;   // fractions of (at-from) vector to use for setting focus depths
    D3DTexture *m_pTextureFocusRange;   // lookup table for range of z values to use

    //////////////////////////////////////////////////////////////////////
    // Pixel shader handles
    // 
    DWORD m_dwBlurPixelShader;          // blur the back-buffer
    DWORD m_dwDepthPixelShader;         // use pixel shader arithmetic to map z to focus value
    DWORD m_dwDepthLookupPixelShader;   // use a lookup texture to map z to the focus value
    // Blend the in-focus back-buffer with the blurred texture...
    DWORD m_dwFocusPixelShader;         // ... based on DepthPixelShader focus value
    DWORD m_dwFocusLookupPixelShader;   // ... or based on DepthLookupPixelShader focus value

    //////////////////////////////////////////////////////////////////////
    // Main filtering routine that draws the source texture multiple
    // times, with sub-pixel offsets and filter coefficients.
    // 
    HRESULT FilterCopy(LPDIRECT3DTEXTURE8 pTextureDst,
                       LPDIRECT3DTEXTURE8 pTextureSrc,
                       UINT nSample,
                       FilterSample rSample[],
                       UINT nSuperSampleX,
                       UINT nSuperSampleY,
                       RECT *pRectDst = NULL,  // The destination texture is written only within this region.
                       RECT *pRectSrc = NULL); // The source texture is read outside of this region by the halfwidth of the filter.
    
    //////////////////////////////////////////////////////////////////////
    // Blur backbuffer and set m_pBlur.  Calls FilterCopy with
    // different filter coefficients and offsets, based on the current
    // FILTERMODE setting.
	//
    HRESULT Blur();
    
    //////////////////////////////////////////////////////////////////////
	// The z-values from the depth buffer are mapped to a focus range
	// based on the current range of depths m_fDepth0 and m_fDepth1,
	// which are set based on a fraction (m_fFraction0, m_fFraction1)
	// of the distance of the near and far z-clip planes.
	//
    HRESULT FillFocusRangeTexture(bool bRamp);	// fill texture using current focus mapping 
    HRESULT CalculateFocusDepths();     // Use fractions along viewing vector to set focus depths
    HRESULT CalculateDepth(float *pfDepth, const D3DXVECTOR3 &vPosition);	// calculate the depth value of the 3D point
    HRESULT CalculateDepthMapping(float fDepth0,		// compute constants for pixel shader arithmetic
                                  float fDepth1,
                                  float *pfAlphaOffset,
                                  float *pfAlphaSlope, 
                                  float *pfBlueOffset,
                                  float *pfBlueSlope);
	
    //////////////////////////////////////////////////////////////////////
    // For demonstrating the inputs to the full focus effect, the sample
	// can draw just the blurred texture or the z-buffer as a texture.
	//
    HRESULT DrawBlur();			// draw blurred texture
    HRESULT DrawDepthRaw();		// draw raw z values as rgb
    HRESULT DrawDepthRange();	// map z to focus values using pixel shader arithmetic
    HRESULT DrawDepthLookup();	// map z to focus values using a lookup texture
	
    //////////////////////////////////////////////////////////////////////
    // Full focus blur effect, using either of the two z-to-focus mapping
	// techniques.
	//
    HRESULT DrawFocusRange();	// use pixel shader arithmetic for mapping z to focus value
    HRESULT DrawFocusLookup();	// map z through lookup texture to focus value

    //////////////////////////////////////////////////////////////////////
	// Set current transformation matrices based on current view
	// position and orientation.
	//
    HRESULT SetCameraTransformations();
public:

	// Overrides of XbApp framework functions
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
#ifdef _DEBUG
    m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;    // Allow unlimited frame rate
#endif
    m_bDrawHelp     = false;
    m_fFPSBase = 0.f;
    m_eDepthMode = DM_RANGE;
    m_eEffectMode   = EM_FOCUS;
    m_eFilterMode   = m_eFilterMode2 = (FILTERMODE)0;
	m_strFilterName = NULL;
	m_strFilterDescription = NULL;
    ZeroMemory(m_rpTexture, sizeof(m_rpTexture));
    m_pCylinderVB      = NULL;
    ZeroMemory(m_rpBlur, sizeof(m_rpBlur));
    m_pBlur = NULL;
    m_pTextureFocusRange = NULL;
    m_dwBlurPixelShader = 0;
    m_dwDepthPixelShader = 0;
    m_dwDepthLookupPixelShader = 0;
    m_dwFocusPixelShader = 0;
    m_dwFocusLookupPixelShader = 0;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create the font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create the help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Get size of render target
    LPDIRECT3DSURFACE8 pRenderTarget;
    g_pd3dDevice->GetRenderTarget(&pRenderTarget);
    D3DSURFACE_DESC descRenderTarget;
    pRenderTarget->GetDesc(&descRenderTarget);
    UINT Width = descRenderTarget.Width;
    UINT Height = descRenderTarget.Height;
    D3DFORMAT Format = descRenderTarget.Format;
    pRenderTarget->Release();

    // Create the blur textures
    for (UINT i = 0; i < BLUR_COUNT; i++)
    {
        // make the size a factor of 2 smaller each time
        m_pd3dDevice->CreateTexture(Width >> i, Height >> i, 1, D3DUSAGE_RENDERTARGET, Format, 0, &m_rpBlur[i]);
    }

    // Create the pixel shaders
#pragma warning( push )
#pragma warning( disable : 4245 )   // ignore conversion of int to DWORD
    {
#include "blur.inl"
        g_pd3dDevice->CreatePixelShader(&psd, &m_dwBlurPixelShader);
    }
    {
#include "depth.inl"
        g_pd3dDevice->CreatePixelShader(&psd, &m_dwDepthPixelShader);
    }
    {
#include "depthlookup.inl"
        g_pd3dDevice->CreatePixelShader(&psd, &m_dwDepthLookupPixelShader);
    }
    {
#include "focus.inl"
        g_pd3dDevice->CreatePixelShader(&psd, &m_dwFocusPixelShader);
    }
    {
#include "focuslookup.inl"
        g_pd3dDevice->CreatePixelShader(&psd, &m_dwFocusLookupPixelShader);
    }
#pragma warning( pop )
    
    // Create the resources
    if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", resource_NUM_RESOURCES, NULL ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Load the textures
    m_rpTexture[0] = m_xprResource.GetTexture( resource_Texture0_OFFSET );
    m_rpTexture[1] = m_xprResource.GetTexture( resource_Texture1_OFFSET );
    m_rpTexture[2] = m_xprResource.GetTexture( resource_Checker_OFFSET );
    
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
#define frand(a) ((float)rand()*(a)/32768.0f)
    float fScale = 100.f;
    srand(123456);
    for (UINT i = 0; i < m_nObjectCount; i++)
    {
        m_rObjects[i].m_vPosition = D3DXVECTOR3( frand(fScale), 0.f, frand(fScale));
        m_rObjects[i].m_color = D3DXCOLOR( frand(1.f), frand(1.f), frand(1.f), 1.f);
        m_rObjects[i].m_iTexture = irand(TEXTURE_COUNT);
    }
    
    // Set camera parameters and initialize camera matrices
    m_vAt = m_rObjects[0].m_vPosition;
    m_vFrom = m_vAt - D3DXVECTOR3(8.f, 0.f, 0.f);
    m_vUp = D3DXVECTOR3( 0.0f, 1.0f , 0.0f);
    SetCameraTransformations();

    // Set focus range to be around the object we're looking at
    m_fFraction0 = 0.9f;
    m_fFraction1 = 1.1f;
    CalculateFocusDepths();

    // Create and fill the focus range texture
    FillFocusRangeTexture(false);

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
    static float fNear = 1.f;
    static float fFar = 50.f;
    D3DXMatrixPerspectiveFovLH( &m_matProjection, D3DX_PI/4, 640.f / 480.f, fNear, fFar);
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjection );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    bool bUpdateDepth = false;
    
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
        m_bDrawHelp = !m_bDrawHelp;

    // Set base FPS
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START )
    {
        // alternate between showing FPS delta and not
        if (m_fFPSBase == 0.f)
            m_fFPSBase = m_fFPS;
        else
            m_fFPSBase = 0.f;
    }

    // Toggle depth mode when in depth or focus mode
    if (m_eEffectMode == EM_DEPTH
        || m_eEffectMode == EM_FOCUS)
    {
        if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] )
        {
            int iDepthMode = (int)m_eDepthMode + 1;
            if (iDepthMode >= (int)_DM_MAX)
                iDepthMode = 0;
            m_eDepthMode = (DEPTHMODE)iDepthMode;
            bUpdateDepth = true;
        }
    }

    // Change filter type only in the blur and focus modes
    if (m_eEffectMode == EM_BLUR 
        || m_eEffectMode == EM_FOCUS)
    {
        // Swap filter modes
        if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE] )
        {
            FILTERMODE fm = m_eFilterMode2;
            m_eFilterMode2 = m_eFilterMode;
            m_eFilterMode = fm;
        }

        // Go to next filter mode
        if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
        {
            int iFilterMode = (int)m_eFilterMode + 1;
            if (iFilterMode >= (int)_FM_MAX)
                iFilterMode = 0;
            m_eFilterMode = (FILTERMODE)iFilterMode;
        }

        // Go to previous filter mode
        if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
        {
            int iFilterMode = (int)m_eFilterMode - 1;
            if (iFilterMode < 0)
                iFilterMode = (int)_FM_MAX - 1;
            m_eFilterMode = (FILTERMODE)iFilterMode;
        }

		// Name and description is set by Blur()
		m_strFilterName = NULL;
		m_strFilterDescription = NULL;
    }

    // Go to next effect mode
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
    {
        int iEffectMode = (int)m_eEffectMode + 1;
        if (iEffectMode >= (int)_EM_MAX)
            iEffectMode = 0;
        m_eEffectMode = (EFFECTMODE)iEffectMode;
        bUpdateDepth = true;
    }

    // Go to previous effect mode
    if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
    {
        int iEffectMode = (int)m_eEffectMode - 1;
        if (iEffectMode < 0)
            iEffectMode = (int)_EM_MAX - 1;
        m_eEffectMode = (EFFECTMODE)iEffectMode;
        bUpdateDepth = true;
    }

    // update view position
    static float fOffsetScale = 3.f;
    float fX1 = m_DefaultGamepad.fX1;
    fX1 *= fX1 * fX1; // fX1 cubed
    float fY1 = m_DefaultGamepad.fY1;
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
    static float fAtOffsetScale = 8.f;
    D3DXVECTOR3 vAtOffset(0.f, 0.f, 0.f);
    float fX2 = m_DefaultGamepad.fX2;
    fX2 *= fX2 * fX2; // fX2 cubed
    float fY2 = m_DefaultGamepad.fY2;
    fY2 *= fY2 * fY2; // fY2 cubed
    vAtOffset.x += fAtOffsetScale * fX2 * m_fElapsedTime;
    D3DXVECTOR3 vE = m_vAt - m_vFrom;
    D3DXVec3Normalize(&vE, &vE);
    float fThreshold = 0.99f;
    float fEdotU = D3DXVec3Dot(&vE, &m_vUp);
    if ((fEdotU < -fThreshold && fY2 < 0.f) // near -vUp, but positive movement
        || (fEdotU > fThreshold && fY2 > 0.f)   // near vUp, but negative movement
        || (fEdotU > -fThreshold && fEdotU < fThreshold))       // ordinary case
        vAtOffset.y -= fAtOffsetScale * fY2 * m_fElapsedTime;   // screen-space Y displacement means up-down view turn
    D3DXVec3TransformNormal(&vAtOffset, &vAtOffset, &m_matViewInverse);
    m_vAt += vAtOffset;

    // Set focus depths
    int delta = m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] 
        - m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER];
    if (delta)
    {
        static float fDeltaScale = 0.001f;
        float fScale = 1.f + m_fElapsedTime * delta * fDeltaScale;
        m_fFraction0 *= fScale;
        m_fFraction1 *= fScale;
        if (m_fFraction0 < 0.1f) m_fFraction0 = 0.1f;
        if (m_fFraction0 > 10.f) m_fFraction0 = 10.f;
        if (m_fFraction1 < 0.1f) m_fFraction1 = 0.1f;
        if (m_fFraction1 > 10.f) m_fFraction1 = 10.f;
        bUpdateDepth = true;
    }

    if (bUpdateDepth)
    {
        if (m_eDepthMode == DM_RAW
            && m_eEffectMode == EM_FOCUS)
        {
            // skip raw depths when in focus effect mode
            m_eDepthMode = DM_RANGE;
        }

        // Set focus depths from m_vFrom, m_vAt, m_fFraction0, and m_fFraction1
        CalculateFocusDepths();

        // Fill the lookup texture, if needed
        if (m_eDepthMode == DM_LOOKUP)
            FillFocusRangeTexture(false);
    }

    SetCameraTransformations();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Draw a gradient filled background
    RenderGradientBackground( 0xff0000ff, 0xff000000 );

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
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
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

    // Draw the current effect
    switch (m_eEffectMode)
    {
    case EM_NOEFFECT:
        break;

    case EM_BLUR:
        Blur();                 // blur back buffer
        DrawBlur();             // display blurred texture
        break;
        
    case EM_DEPTH:
        switch (m_eDepthMode)
        {
        case DM_RAW:
            DrawDepthRaw();         // raw depths
            break;
        case DM_RANGE:
            DrawDepthRange();       // depths mapped with range in pixel shader
            break;
        case DM_LOOKUP:
            DrawDepthLookup();      // depths mapped through lookup table texture
            break;
        }
        break;

    case EM_FOCUS:
        Blur();                 // blur back buffer
        switch (m_eDepthMode)
        {
        case DM_RAW:    // this should not happen, but just in case, fall through to range depth mapping
        case DM_RANGE:
            DrawFocusRange();       // blur the backbuffer into a texture, then use current depth range to choose between sharp or blurred focus
            break;

        case DM_LOOKUP:
            DrawFocusLookup();      // blur the backbuffer into a texture, then use current depth mapped through a lookup table to choose between sharp or blurred focus
            break;
        }
        break;
    }

    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,        FALSE );

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        static WCHAR *rstrEffectMode[] = {
            L"No effect",
            L"Blur texture",
            L"Depth",
            L"Focus effect",
        };
        ASSERT(sizeof(rstrEffectMode)/sizeof(WCHAR *) == _EM_MAX);
        WCHAR str[200];
        m_Font.Begin();
        swprintf( str, L"FocusBlur: %s", rstrEffectMode[m_eEffectMode] );
        m_Font.DrawText(  64, 50, 0xffffffff, str );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        if (m_fFPSBase != 0.f)
        {
            FLOAT msDelta = 1000.f / m_fFPS - 1000.f / m_fFPSBase;  // milliseconds
            swprintf( str, L"%+0.02f ms", msDelta);
            m_Font.DrawText( 450, 110, 0xff00ff00, str);
        }
        if (m_eEffectMode == EM_FOCUS
            || m_eEffectMode == EM_DEPTH)
        {
            static WCHAR *rstrDepthMode[] = {
                L"Raw Depth",
                L"Depth Range",
                L"Depth Lookup",
            };
            m_Font.DrawText( 64, 80, 0xffffff00, rstrDepthMode[m_eDepthMode]);
            static WCHAR *rstrDepthModeDescription[] = {
                L"Z values are draw as red, green, blue.",
				
                L"Z values are mapped to focus value with\n"
				L"pixel shader arithmetic. Use triggers to change\n"
				L"focus range and black button to toggle Z mode.",
				
                L"Z values are mapped to focus values using\n"
				L"a lookup texture. Use triggers to change\n"
				L"focus range and black button to toggle Z mode.",
            };
			if (m_eEffectMode == EM_DEPTH)
				m_Font.DrawText( 64, 330, 0xffaaaa10, rstrDepthModeDescription[m_eDepthMode]);
        }
        if ( m_eEffectMode == EM_BLUR )
        {
			if (m_strFilterName != NULL)
				m_Font.DrawText( 64, 110, 0xffffff00, m_strFilterName);
			if (m_strFilterDescription != NULL)
				m_Font.DrawText( 64, 330, 0xffaaaa10, m_strFilterDescription);
			m_Font.DrawText( 64, 400, 0xffaaaa10, L"Use X and Y to choose blur filter." );
        }
		else if ( m_eEffectMode == EM_FOCUS )
		{
			if (m_strFilterName != NULL)
				m_Font.DrawText( 64, 110, 0xffffff00, m_strFilterName);
		}
        else if (m_eEffectMode == EM_DEPTH)
        {
            static bool bDebugDepth = false;    // set this in the debugger to see depth mapping values
            if (bDebugDepth)
            {
                float fAlphaOffset, fAlphaSlope, fBlueOffset, fBlueSlope;
                CalculateDepthMapping(m_fDepth0, m_fDepth1, &fAlphaOffset, &fAlphaSlope, &fBlueOffset, &fBlueSlope);
                swprintf( str, L"fAlphaOffset %f 0x%02x", fAlphaOffset, (int)(fAlphaOffset * 255 + 0.5f));
                m_Font.DrawText( 64, 140, 0xff00ff00, str);
                swprintf( str, L"fAlphaSlope %f 0x%02x", fAlphaSlope, (int)(fAlphaSlope * 255 + 0.5f));
                m_Font.DrawText( 64, 170, 0xff00ff00, str);
                swprintf( str, L"fBlueOffset %f 0x%02x", fBlueOffset, (int)(fBlueOffset * 255 + 0.5f));
                m_Font.DrawText( 64, 200, 0xff00ff00, str);
                swprintf( str, L"fBlueSlope %f 0x%02x", fBlueSlope, (int)(fBlueSlope * 255 + 0.5f));
                m_Font.DrawText( 64, 230, 0xff00ff00, str);
                swprintf( str, L"m_fDepth0 %f 0x%x", m_fDepth0, (int)(m_fDepth0 * 65536 + 0.5f));
                m_Font.DrawText( 64, 260, 0xff00ff00, str);
                swprintf( str, L"m_fDepth1 %f 0x%x", m_fDepth1, (int)(m_fDepth1 * 65536 + 0.5f));
                m_Font.DrawText( 64, 290, 0xff00ff00, str);
            }
        }
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Filter the source texture by rendering into the destination texture
// with subpixel offsets. Does 4 filter coefficients at a time, using
// all the stages of the pixel shader.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FilterCopy(LPDIRECT3DTEXTURE8 pTextureDst,
                                LPDIRECT3DTEXTURE8 pTextureSrc,
                                UINT nSample,
                                FilterSample rSample[],
                                UINT nSuperSampleX,
                                UINT nSuperSampleY,
                                RECT *pRectDst,
                                RECT *pRectSrc )
{
    // Save current render target and depth buffer
    LPDIRECT3DSURFACE8 pRenderTarget, pZBuffer;
    g_pd3dDevice->GetRenderTarget(&pRenderTarget);
    g_pd3dDevice->GetDepthStencilSurface(&pZBuffer);

    // Set destination as render target
    LPDIRECT3DSURFACE8 pSurface = NULL;
    pTextureDst->GetSurfaceLevel(0, &pSurface);
    g_pd3dDevice->SetRenderTarget(pSurface, NULL);  // no depth-buffering
    pSurface->Release();

    // Get descriptions of source and destination
    D3DSURFACE_DESC descSrc;
    pTextureSrc->GetLevelDesc(0, &descSrc);
    D3DSURFACE_DESC descDst;
    pTextureDst->GetLevelDesc(0, &descDst);

    // Setup rectangles if not specified on input
    RECT rectSrc = { 0, 0, descSrc.Width, descSrc.Height };
    if (pRectSrc == NULL) pRectSrc = &rectSrc;
    RECT rectDst = { 0, 0, descDst.Width, descDst.Height };
    if (pRectDst == NULL)
    {
        // If the destination rectangle is not specified, we change it to match the source rectangle
        rectDst.right = (pRectSrc->right - pRectSrc->left) / nSuperSampleX;
        rectDst.bottom = (pRectSrc->bottom - pRectSrc->top) / nSuperSampleY;
        pRectDst = &rectDst;
    }
    ASSERT((pRectDst->right - pRectDst->left) == (pRectSrc->right - pRectDst->left) / (INT)nSuperSampleX );
    ASSERT((pRectDst->bottom - pRectDst->top) == (pRectSrc->bottom - pRectDst->top) / (INT)nSuperSampleY );
    
    // set render state for filtering
    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);    // on first rendering, copy new value over current render target contents
    g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);    // setup subsequent renderings to add to previous value
    g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    // set texture state
    UINT xx; // texture stage index
    for (xx = 0; xx < 4; xx++)
    {
        g_pd3dDevice->SetTexture(xx, pTextureSrc);  // use our source texture for all four stages
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLOROP, D3DTOP_DISABLE);
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);  // pass texture coords without transformation
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_TEXCOORDINDEX, xx); // each texture has different tex coords
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MAXMIPLEVEL, 0);
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MIPFILTER, D3DTEXF_POINT);
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLORKEYOP, D3DTCOLORKEYOP_DISABLE);
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLORSIGN, 0);
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ALPHAKILL, D3DTALPHAKILL_DISABLE);
    }
    
    g_pd3dDevice->SetPixelShader(m_dwBlurPixelShader);          // use blur pixel shader
    g_pd3dDevice->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_TEX4);   // for screen-space texture-mapped quadrilateral

    // Prepare quadrilateral vertices
    float x0 = (float)pRectDst->left - 0.5f;
    float y0 = (float)pRectDst->top - 0.5f;
    float x1 = (float)pRectDst->right - 0.5f;
    float y1 = (float)pRectDst->bottom - 0.5f;
    struct quad {
        float x, y, z, w1;
        struct uv {
            float u, v;
        } tex[4];   // each texture has different offset
    } aQuad[4] =
      { //  X   Y     Z   1/W     u0  v0      u1  v1      u2  v2      u3  v3
          {x0, y0, 1.0f, 1.0f, }, // texture coords are set below
          {x1, y0, 1.0f, 1.0f, },
          {x0, y1, 1.0f, 1.0f, },
          {x1, y1, 1.0f, 1.0f, }
      };

    // Set rendering to just the destination rect
    g_pd3dDevice->SetScissors(1, FALSE, (D3DRECT *)pRectDst);

    // Draw a quad for each block of 4 filter coefficients
    xx = 0; // current texture stage
    float fOffsetScaleX, fOffsetScaleY; // convert destination coords to source texture coords
    float u0, v0, u1, v1;   // base source rectangle.
    if (XGIsSwizzledFormat(descSrc.Format))
    {
        float fWidthScale = 1.f / (float)descSrc.Width;
        float fHeightScale = 1.f / (float)descSrc.Height;
        fOffsetScaleX = (float)nSuperSampleX * fWidthScale;
        fOffsetScaleY = (float)nSuperSampleY * fHeightScale;
        u0 = (float)pRectSrc->left * fWidthScale;
        v0 = (float)pRectSrc->top * fHeightScale;
        u1 = (float)pRectSrc->right * fWidthScale;
        v1 = (float)pRectSrc->bottom * fHeightScale;
    }
    else
    {
        fOffsetScaleX = (float)nSuperSampleX;
        fOffsetScaleY = (float)nSuperSampleY;
        u0 = (float)pRectSrc->left;
        v0 = (float)pRectSrc->top;
        u1 = (float)pRectSrc->right;
        v1 = (float)pRectSrc->bottom;
    }
    D3DCOLOR rColor[4];
    DWORD rPSInput[4];
    for (UINT iSample = 0; iSample < nSample; iSample++)
    {
        // Set filter coefficients
        float fValue = rSample[iSample].fValue;
        /*
        float rf[4] = {fValue, fValue, fValue, fValue};
        g_pd3dDevice->SetPixelShaderConstant(xx, rf, 1);            // positive coeff
        */
        if (fValue < 0.f)
        {
            rColor[xx] = D3DXCOLOR(-fValue, -fValue, -fValue, -fValue);
            rPSInput[xx] = PS_INPUTMAPPING_SIGNED_NEGATE | ((xx % 2) ? PS_REGISTER_C1 : PS_REGISTER_C0);
        }
        else
        {
            rColor[xx] = D3DXCOLOR(fValue, fValue, fValue, fValue);
            rPSInput[xx] = PS_INPUTMAPPING_SIGNED_IDENTITY | ((xx % 2) ? PS_REGISTER_C1 : PS_REGISTER_C0);
        }

        // Align supersamples with center of destination pixels
        float fOffsetX = rSample[iSample].fOffsetX * fOffsetScaleX;
        float fOffsetY = rSample[iSample].fOffsetY * fOffsetScaleY;
        aQuad[0].tex[xx].u = u0 + fOffsetX;
        aQuad[0].tex[xx].v = v0 + fOffsetY;
        aQuad[1].tex[xx].u = u1 + fOffsetX;
        aQuad[1].tex[xx].v = v0 + fOffsetY;
        aQuad[2].tex[xx].u = u0 + fOffsetX;
        aQuad[2].tex[xx].v = v1 + fOffsetY;
        aQuad[3].tex[xx].u = u1 + fOffsetX;
        aQuad[3].tex[xx].v = v1 + fOffsetY;
        
        xx++; // Go to next stage
        if (xx == 4 || iSample == nSample - 1)  // max texture stages or last sample
        {
            // zero out unused texture stage coefficients 
            // (only for last filter sample, when number of samples is not divisible by 4)
            for (; xx < 4; xx++)
            {
                g_pd3dDevice->SetTexture(xx, NULL);
                rColor[xx] = 0;
                rPSInput[xx] = PS_INPUTMAPPING_UNSIGNED_IDENTITY | PS_REGISTER_ZERO;
            }
        
            // Set coefficients
            g_pd3dDevice->SetRenderState(D3DRS_PSCONSTANT0_0, rColor[0] );
            g_pd3dDevice->SetRenderState(D3DRS_PSCONSTANT1_0, rColor[1] );
            g_pd3dDevice->SetRenderState(D3DRS_PSCONSTANT0_1, rColor[2] );
            g_pd3dDevice->SetRenderState(D3DRS_PSCONSTANT1_1, rColor[3] );

            // Remap coefficients to proper sign
            g_pd3dDevice->SetRenderState(
                D3DRS_PSRGBINPUTS0,
                PS_COMBINERINPUTS( rPSInput[0] | PS_CHANNEL_RGB,   PS_REGISTER_T0 | PS_CHANNEL_RGB   | PS_INPUTMAPPING_SIGNED_IDENTITY,
                                   rPSInput[1] | PS_CHANNEL_RGB,   PS_REGISTER_T1 | PS_CHANNEL_RGB   | PS_INPUTMAPPING_SIGNED_IDENTITY ) );
            g_pd3dDevice->SetRenderState(
                D3DRS_PSALPHAINPUTS0,
                PS_COMBINERINPUTS( rPSInput[0] | PS_CHANNEL_ALPHA, PS_REGISTER_T0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_SIGNED_IDENTITY,
                                   rPSInput[1] | PS_CHANNEL_ALPHA, PS_REGISTER_T1 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_SIGNED_IDENTITY ) );
            g_pd3dDevice->SetRenderState(
                D3DRS_PSRGBINPUTS1,
                PS_COMBINERINPUTS( rPSInput[2] | PS_CHANNEL_RGB,   PS_REGISTER_T2 | PS_CHANNEL_RGB   | PS_INPUTMAPPING_SIGNED_IDENTITY,
                                   rPSInput[3] | PS_CHANNEL_RGB,   PS_REGISTER_T3 | PS_CHANNEL_RGB   | PS_INPUTMAPPING_SIGNED_IDENTITY ) );
            g_pd3dDevice->SetRenderState(
                D3DRS_PSALPHAINPUTS1,
                PS_COMBINERINPUTS( rPSInput[2] | PS_CHANNEL_ALPHA, PS_REGISTER_T2 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_SIGNED_IDENTITY,
                                   rPSInput[3] | PS_CHANNEL_ALPHA, PS_REGISTER_T3 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_SIGNED_IDENTITY ) );
            
            // draw the quad to filter the coefficients so far
            g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, aQuad, sizeof(quad)); // one quad blends 4 textures
            g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); // on subsequent renderings, add to what's in the render target 
            xx = 0;
        }
    }

    // clear texture stages
    for (xx=0; xx<4; xx++)
    {
        g_pd3dDevice->SetTexture(xx, NULL);
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_COLOROP, D3DTOP_DISABLE);
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        g_pd3dDevice->SetTextureStageState(xx, D3DTSS_MIPMAPLODBIAS, 0);
    }

    // Restore render target and zbuffer
    g_pd3dDevice->SetRenderTarget( pRenderTarget, pZBuffer );
    if (pRenderTarget != NULL) pRenderTarget->Release();
    if (pZBuffer != NULL) pZBuffer->Release();
    return S_OK;
}

//-----------------------------------------------------------------------------
// Blur backbuffer and set m_pBlur to the current blur texture.  Calls
// FilterCopy with different filter coefficients and offsets, based on
// the current FILTERMODE setting.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Blur()
{
    // Make D3DTexture wrapper around current render target
    LPDIRECT3DSURFACE8 pRenderTarget;
    g_pd3dDevice->GetRenderTarget(&pRenderTarget);
    D3DSURFACE_DESC descRenderTarget;
    pRenderTarget->GetDesc(&descRenderTarget);
    D3DTexture RenderTargetTexture;
    ZeroMemory( &RenderTargetTexture, sizeof(RenderTargetTexture) );
    XGSetTextureHeader( descRenderTarget.Width, descRenderTarget.Height, 1, 0, descRenderTarget.Format, 0, &RenderTargetTexture, pRenderTarget->Data, descRenderTarget.Width * 4);
    pRenderTarget->Release();
    
    // Filters align to blurriest point in supersamples, on the 0.5 boundaries.
    // This takes advantage of the bilinear filtering in the texture map lookup.
    static FilterSample BoxFilter[] = {     // for 2x2 downsampling
        { 0.25f, -0.5f, -0.5f },
        { 0.25f,  0.5f, -0.5f },
        { 0.25f, -0.5f,  0.5f },
        { 0.25f,  0.5f,  0.5f },
    };
    static FilterSample YFilter[] = {       // 1221 4-tap filter in Y
        { 1.f/6.f, 0.f, -1.5f },
        { 2.f/6.f, 0.f, -0.5f },
        { 2.f/6.f, 0.f,  0.5f },
        { 1.f/6.f, 0.f,  1.5f },
    };
    static FilterSample XFilter[] = {       // 1221 4-tap filter in X
        { 1.f/6.f, -1.5f, 0.f },
        { 2.f/6.f, -0.5f, 0.f },
        { 2.f/6.f,  0.5f, 0.f },
        { 1.f/6.f,  1.5f, 0.f },
    };
    static FilterSample Y141Filter[] = {    // 141 3-tap filter in Y
        { 1.f/6.f, 0.f, -1.0f },
        { 4.f/6.f, 0.f,  0.0f },
        { 1.f/6.f, 0.f,  1.0f },
    };
    static FilterSample X141Filter[] = {        // 141 3-tap filter in X
        { 1.f/6.f, -1.0f, 0.f },
        { 4.f/6.f,  0.0f, 0.f },
        { 1.f/6.f,  1.0f, 0.f },
    };
    m_pBlur = NULL;
    switch (m_eFilterMode)
    {
    case FM_IDENTITY:
    {
		m_strFilterName = L"FM_IDENTITY";
		m_strFilterDescription =
			L"Identity filter.";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[0];
        static FilterSample IdentityFilter[] = {
            { 1.f, 0.f, 0.f },
        };
        FilterCopy(pTextureDst, pTextureSrc, 1, IdentityFilter, 1, 1);
        m_pBlur = pTextureDst;
        break;
    }

    case FM_BOX:
    {
		m_strFilterName = L"FM_BOX";
		m_strFilterDescription =
			L"2x2 box filter, no decimation";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[0];
        FilterCopy(pTextureDst, pTextureSrc, 4, BoxFilter, 1, 1);
        m_pBlur = pTextureDst;
        break;
    }

    case FM_VERT:
    {
		m_strFilterName = L"FM_VERT";
		m_strFilterDescription =
			L"Vertical gaussian (1221), no decimation";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[0];
        FilterCopy(pTextureDst, pTextureSrc, 4, YFilter, 1, 1);
        m_pBlur = pTextureDst;
        break;
    }

    case FM_HORIZ:
    {
		m_strFilterName = L"FM_HORIZ";
		m_strFilterDescription =
			L"Horizontal gaussian (1221), no decimation";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[0];
        FilterCopy(pTextureDst, pTextureSrc, 4, XFilter, 1, 1);
        m_pBlur = pTextureDst;
        break;
    }

    case FM_BOX2:
    {
		m_strFilterName = L"FM_BOX2";
		m_strFilterDescription =
			L"2x2 box filter, 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];
        FilterCopy(pTextureDst, pTextureSrc, 4, BoxFilter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }

    case FM_VERT2:
    {
		m_strFilterName = L"FM_VERT2";
		m_strFilterDescription =
			L"Vertical gaussian (1221), 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, YFilter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }

    case FM_HORIZ2:
    {
		m_strFilterName = L"FM_HORIZ2";
		m_strFilterDescription =
			L"Horizontal gaussian (1221), 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, XFilter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }

    case FM_VERT2_HORIZ2:
    {
		m_strFilterName = L"FM_VERT2_HORIZ2";
		m_strFilterDescription =
			L"2 passes: Vertical gaussian (1221) followed by\n"
			L"horizontal gaussian (1221), with 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, YFilter, 2, 2);
        pTextureSrc = pTextureDst;  // source is previous blur texture
        pTextureDst = m_rpBlur[2];  // destination is next blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, XFilter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }
    case FM_HORIZ2_VERT2:
    {
		m_strFilterName = L"FM_HORIZ2_VERT2";
		m_strFilterDescription =
			L"2 passes: Horizontal gaussian (1221) followed by\n"
			L"vertical gaussian (1221), with 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, XFilter, 2, 2);
        pTextureSrc = pTextureDst;  // source is previous blur texture
        pTextureDst = m_rpBlur[2];  // destination is next blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, YFilter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }
    case FM_VERT2_HORIZ:
    {
		m_strFilterName = L"FM_VERT2_HORIZ";
		m_strFilterDescription =
			L"2 passes: Vertical gaussian (1221) followed by\n"
			L"narrow horizontal gaussian (141), with 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, YFilter, 2, 2);
        pTextureSrc = pTextureDst;  // source is previous blur texture
        pTextureDst = m_rpBlur[2];  // destination is next blur texture
        FilterCopy(pTextureDst, pTextureSrc, 3, X141Filter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }
    case FM_HORIZ2_VERT:
    {
		m_strFilterName = L"FM_HORIZ2_VERT";
		m_strFilterDescription =
			L"2 passes: Horizontal gaussian (1221) followed by\n"
			L"narrow vertical gaussian (141), with 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, XFilter, 2, 2);
        pTextureSrc = pTextureDst;  // source is previous blur texture
        pTextureDst = m_rpBlur[2];  // destination is next blur texture
        FilterCopy(pTextureDst, pTextureSrc, 3, Y141Filter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }
    case FM_BOX2_BOX2:
    {
		m_strFilterName = L"FM_BOX2_BOX2";
		m_strFilterDescription =
			L"2 passes: Box filter followed by box filter,\n"
			L"with 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, BoxFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[2];
        FilterCopy(pTextureDst, pTextureSrc, 4, BoxFilter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }
    case FM_VERT2_HORIZ2_BOX2:
    {
		m_strFilterName = L"FM_VERT2_HORIZ2_BOX2";
		m_strFilterDescription =
			L"3 passes: Vertical gaussian (1441) followed by\n"
			L"horizontal gaussian (1441) followed by box filter,\n"
			L"with 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, YFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[2];
        FilterCopy(pTextureDst, pTextureSrc, 4, XFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[3];
        FilterCopy(pTextureDst, pTextureSrc, 4, BoxFilter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }
    case FM_BOX2_BOX2_BOX2:
    {
		m_strFilterName = L"FM_BOX2_BOX2_BOX2";
		m_strFilterDescription =
			L"3 passes: Box filter followed by box filter\n"
			L"followed by box filter, with 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, BoxFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[2];
        FilterCopy(pTextureDst, pTextureSrc, 4, BoxFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[3];
        FilterCopy(pTextureDst, pTextureSrc, 4, BoxFilter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }
    case FM_VERT2_HORIZ2_VERT2:
    {
		m_strFilterName = L"FM_VERT2_HORIZ2_VERT2";
		m_strFilterDescription =
			L"3 passes: Vertical gaussian (1441) then horizontal\n"
			L"gaussian (1441) then vertical gaussian (1441)\n"
			L"with 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, YFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[2];
        FilterCopy(pTextureDst, pTextureSrc, 4, XFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[3];
        FilterCopy(pTextureDst, pTextureSrc, 4, YFilter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }
    case FM_HORIZ2_VERT2_HORIZ2:
    {
		m_strFilterName = L"FM_HORIZ2_VERT2_HORIZ2";
		m_strFilterDescription =
			L"3 passes: Horizontal gaussian (1441) then vertical\n"
			L"gaussian (1441) then horizontal gaussian (1441)\n"
			L"with 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, XFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[2];
        FilterCopy(pTextureDst, pTextureSrc, 4, YFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[3];
        FilterCopy(pTextureDst, pTextureSrc, 4, XFilter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }
    case FM_VERT2_HORIZ2_VERT2_HORIZ2:
    {
		m_strFilterName = L"FM_VERT2_HORIZ2_VERT2_HORIZ2";
		m_strFilterDescription =
			L"4 passes, alternating vertical gaussian (1441)\n"
			L"then horizontal gaussian (1441), with 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, YFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[2];
        FilterCopy(pTextureDst, pTextureSrc, 4, XFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[3];
        FilterCopy(pTextureDst, pTextureSrc, 4, YFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[4];
        FilterCopy(pTextureDst, pTextureSrc, 4, XFilter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }
    case FM_HORIZ2_VERT2_HORIZ2_VERT2:
    {
		m_strFilterName = L"FM_HORIZ2_VERT2_HORIZ2_VERT2";
		m_strFilterDescription =
			L"4 passes, alternating horizontal gaussian (1441)\n"
			L"then vertical gaussian (1441), with 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, XFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[2];
        FilterCopy(pTextureDst, pTextureSrc, 4, YFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[3];
        FilterCopy(pTextureDst, pTextureSrc, 4, XFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[4];
        FilterCopy(pTextureDst, pTextureSrc, 4, YFilter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }
    case FM_BOX2_BOX2_BOX2_BOX2:
    {
		m_strFilterName = L"FM_BOX2_BOX2_BOX2_BOX2";
		m_strFilterDescription =
			L"4 passes of box filter with 2x2 downsampling";
        D3DTexture *pTextureSrc = &RenderTargetTexture; // source is backbuffer 
        D3DTexture *pTextureDst = m_rpBlur[1];      // destination is blur texture
        FilterCopy(pTextureDst, pTextureSrc, 4, BoxFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[2];
        FilterCopy(pTextureDst, pTextureSrc, 4, BoxFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[3];
        FilterCopy(pTextureDst, pTextureSrc, 4, BoxFilter, 2, 2);
        pTextureSrc = pTextureDst;
        pTextureDst = m_rpBlur[4];
        FilterCopy(pTextureDst, pTextureSrc, 4, BoxFilter, 2, 2);
        m_pBlur = pTextureDst;
        break;
    }
    }
    return S_OK;
}
    
//-----------------------------------------------------------------------------
// Display the blurry texture
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawBlur()
{
    static D3DCOLOR colorBlend = 0xffffffff;
    if (!m_pBlur)
        return S_FALSE;
    LPDIRECT3DTEXTURE8 pTexture = m_pBlur;
    
    // Get size of backbuffer
    LPDIRECT3DSURFACE8 pRenderTarget;
    g_pd3dDevice->GetRenderTarget(&pRenderTarget);
    D3DSURFACE_DESC descRenderTarget;
    pRenderTarget->GetDesc(&descRenderTarget);
    UINT Width = descRenderTarget.Width;
    UINT Height = descRenderTarget.Height;
    pRenderTarget->Release();

    // Texture coordinates in linear format textures go from 0 to n-1 rather
    // than the 0 to 1 that is used for swizzled textures.
    D3DSURFACE_DESC desc;
    pTexture->GetLevelDesc(0, &desc);
    struct BACKGROUNDVERTEX { D3DXVECTOR4 p; FLOAT tu, tv; } v[4];
    v[0].p = D3DXVECTOR4( -0.5f,        -0.5f,         1.0f, 1.0f ); v[0].tu = 0.0f;              v[0].tv = 0.0f;
    v[1].p = D3DXVECTOR4( Width - 0.5f, -0.5f,         1.0f, 1.0f ); v[1].tu = (float)desc.Width; v[1].tv = 0.0f;
    v[2].p = D3DXVECTOR4( -0.5f,        Height - 0.5f, 1.0f, 1.0f ); v[2].tu = 0.0f;              v[2].tv = (float)desc.Height;
    v[3].p = D3DXVECTOR4( Width - 0.5f, Height - 0.5f, 1.0f, 1.0f ); v[3].tu = (float)desc.Width; v[3].tv = (float)desc.Height;
    
    // Set states
    g_pd3dDevice->SetPixelShader( 0 );
    g_pd3dDevice->SetTexture( 0, pTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    static D3DTEXTUREFILTERTYPE dwMin0 = D3DTEXF_LINEAR;
    static D3DTEXTUREFILTERTYPE dwMag0 = D3DTEXF_LINEAR;
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, dwMin0 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, dwMag0 );
    g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, colorBlend );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);

    // Render the screen-aligned quadrilateral
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADSTRIP, 1, v, sizeof(BACKGROUNDVERTEX) );

    // Reset render states
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    // Remove linear texture before setting address mode to Wrap
    g_pd3dDevice->SetTexture( 0, NULL );
    return S_OK;
}

//-----------------------------------------------------------------------------
// Display the depth buffer as color values, with the
// most significant bits in red and green
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawDepthRaw()
{
    // Make a D3DTexture wrapper around the depth buffer surface
    D3DSURFACE_DESC desc;
    D3DSurface *pZBuffer;
    m_pd3dDevice->GetDepthStencilSurface( &pZBuffer );
    pZBuffer->GetDesc(&desc);
    D3DTexture ZBufferTexture;
    ZeroMemory( &ZBufferTexture, sizeof(ZBufferTexture) );
    static D3DFORMAT fmtOverride = D3DFMT_LIN_R8G8B8A8;     // Put the most significant bits in the red and green channels
    XGSetTextureHeader( desc.Width, desc.Height, 1, 0, fmtOverride, 0, &ZBufferTexture, pZBuffer->Data, desc.Width * 4);
    pZBuffer->Release();

    D3DTexture *pTexture = &ZBufferTexture;
    static D3DCOLOR colorBlend = 0xffffffff;

    struct BACKGROUNDVERTEX { D3DXVECTOR4 p; FLOAT tu, tv; } v[4];
    v[0].p = D3DXVECTOR4( -0.5f,             -0.5f,              1.0f, 1.0f ); v[0].tu = 0.0f;              v[0].tv = 0.0f;
    v[1].p = D3DXVECTOR4( desc.Width - 0.5f, -0.5f,              1.0f, 1.0f ); v[1].tu = (float)desc.Width; v[1].tv = 0.0f;
    v[2].p = D3DXVECTOR4( -0.5f,             desc.Height - 0.5f, 1.0f, 1.0f ); v[2].tu = 0.0f;              v[2].tv = (float)desc.Height;
    v[3].p = D3DXVECTOR4( desc.Width - 0.5f, desc.Height - 0.5f, 1.0f, 1.0f ); v[3].tu = (float)desc.Width; v[3].tv = (float)desc.Height;
    
    // Set states
    g_pd3dDevice->SetPixelShader( 0 );
    g_pd3dDevice->SetTexture( 0, pTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    static DWORD dwColorArg1 = D3DTA_TEXTURE;
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, dwColorArg1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, colorBlend );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);

    // Render the screen-aligned quadrilateral
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADSTRIP, 1, v, sizeof(BACKGROUNDVERTEX) );

    // Reset render states
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    g_pd3dDevice->SetTexture( 0, NULL );
    g_pd3dDevice->SetTexture( 1, NULL );
    g_pd3dDevice->SetPixelShader( 0 );

    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// For mapping from the depth buffer to blend values using
// a texture map lookup. See media\shaders\depthlookup.psh
//
// This is more general than computing the range as in
// media\shaders\depth.psh, since the ramp can be filled in
// arbitrarily, but may be more expensive due to the extra texture
// lookup.
//
float FUnitMap(float fAlpha, float fBlue, float fAlphaOffset, float fAlphaSlope, float fBlueOffset, float fBlueSlope)
{
    //return g_fPixelShaderScale * fAlphaSlope * (fAlpha - fAlphaOffset) + fBlueSlope * fBlue + fBlueOffset - 0.5f;
    return g_fPixelShaderScale * fAlphaSlope * (fAlpha - fAlphaOffset) + fBlueSlope * (fBlue - fBlueOffset);
}

float FQuantizedDepth(float fDepth, float *pfAlpha, float *pfBlue)
{
    float fDepth16 = fDepth * (float)(1 << 16);
    DWORD dwDepth16 = (DWORD)(fDepth16 /*+ 0.5f*/);
    *pfAlpha = (dwDepth16 >> 8) * (1.f / 255.f);
    *pfBlue = (dwDepth16 & 0xff) * (1.f / 255.f);
    return (float)dwDepth16 / (float)(1 << 16);
}

HRESULT CXBoxSample::FillFocusRangeTexture(bool bRamp)
{
    HRESULT hr;
    static UINT Width = 256;
    static UINT Height = 64;
    if (m_pTextureFocusRange)
        m_pTextureFocusRange->Release();
    m_pd3dDevice->CreateTexture( Width, Height, 1, 0, D3DFMT_A8, 0, &m_pTextureFocusRange);
    
    D3DLOCKED_RECT lockedRect;
    hr = m_pTextureFocusRange->LockRect(0, &lockedRect, NULL, 0l);
    if (FAILED(hr))
        return hr;
    DWORD dwPixelStride = 1;
    Swizzler s(Width, Height, 0);
    s.SetV(s.SwizzleV(0));
    s.SetU(s.SwizzleU(0));
    if (bRamp)
    {
        for (UINT j = 0; j < Height; j++)
        {
            for (UINT i = 0; i < Width; i++)
            {
                BYTE *p = (BYTE *)lockedRect.pBits + dwPixelStride * s.Get2D();
                *p = (BYTE)i;
                s.IncU();
            }
            s.IncV();
        }
    }
    else
    {
        float fAlphaOffset, fAlphaSlope, fBlueOffset, fBlueSlope;
        CalculateDepthMapping(m_fDepth0, m_fDepth1, &fAlphaOffset, &fAlphaSlope, &fBlueOffset, &fBlueSlope);
        for (UINT i = 0; i < Width; i++)
        {
            for (UINT j = 0; j < Height; j++)
            {
                BYTE *p = (BYTE *)lockedRect.pBits + dwPixelStride * s.Get2D();
                float fAlpha = (float)i / (Width - 1);
                float fBlue = (float)j / (Height - 1);
                float fUnit = 2.f * (FUnitMap(fAlpha, fBlue, fAlphaOffset, fAlphaSlope, fBlueOffset, fBlueSlope) - 0.5f);
                float fMap = 1.f - fUnit * fUnit;
                if (fMap < 0.f) fMap = 0.f;
                if (fMap > 1.f) fMap = 1.f;
                *p = (BYTE)(255 * fMap + 0.5f);
                s.IncV();   // vertical is minor axis
            }
            s.IncU();   // horizontal is major axis
        }
    }
    m_pTextureFocusRange->UnlockRect(0);
    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Use fractions along viewing vector to set focus depths
//
HRESULT CXBoxSample::CalculateFocusDepths()
{
    D3DXVECTOR3 vEye = m_vAt - m_vFrom;
    D3DXVECTOR3 v0 = m_vFrom + m_fFraction0 * vEye;
    D3DXVECTOR3 v1 = m_vFrom + m_fFraction1 * vEye;
    CalculateDepth(&m_fDepth0, v0);
    CalculateDepth(&m_fDepth1, v1);
    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Transform a point through our world, view, and projection 
// matrices to obtain a depth value.
//
HRESULT CXBoxSample::CalculateDepth(float *pfDepth, const D3DXVECTOR3 &vPosition)
{
    D3DXVECTOR4 v(vPosition.x, vPosition.y, vPosition.z, 1.f);
    D3DXVec4Transform(&v, &v, &m_matWorld);
    D3DXVec4Transform(&v, &v, &m_matView);
    D3DXVec4Transform(&v, &v, &m_matProjection);
    *pfDepth = v.z / v.w;
    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Calculate offsets and slope to map given z range to 0,1 in
// the depth and focus pixel shaders.
//
HRESULT CXBoxSample::CalculateDepthMapping(float fDepth0,
                                           float fDepth1,
                                           float *pfAlphaOffset,
                                           float *pfAlphaSlope, 
                                           float *pfBlueOffset,
                                           float *pfBlueSlope)
{
    // check range of args
    if (fDepth0 < 0.f) fDepth0 = 0.f;
    if (fDepth0 > 1.f) fDepth0 = 1.f;
    if (fDepth1 < 0.f) fDepth1 = 0.f;
    if (fDepth1 > 1.f) fDepth1 = 1.f;
    if (fDepth1 < fDepth0)
    {
        // swap depth to make fDepth0 <= fDepth1
        float t = fDepth1;
        fDepth1 = fDepth0;
        fDepth0 = t;
    }
    
    // calculate quantized values
    float fAlpha0, fBlue0;
    float fQuantizedDepth0 = FQuantizedDepth(fDepth0, &fAlpha0, &fBlue0);
    float fAlpha1, fBlue1;
    float fQuantizedDepth1 = FQuantizedDepth(fDepth1, &fAlpha1, &fBlue1);

    // calculate offset and slopes
    float fScale = 1.f / (fQuantizedDepth1 - fQuantizedDepth0);
    if (fScale > g_fPixelShaderScale)
    {
        fScale = g_fPixelShaderScale; // this is the steepest slope we can handle
        fDepth0 = 0.5f * (fDepth0 + fDepth1) - 0.5f / fScale; // move start so that peak is in middle of fDepth0 and fDepth1
        fDepth1 = fDepth0 + 1.f / fScale;
        fQuantizedDepth0 = FQuantizedDepth(fDepth0, &fAlpha0, &fBlue0);
        fQuantizedDepth1 = FQuantizedDepth(fDepth1, &fAlpha1, &fBlue1);
    }
    *pfAlphaOffset = fAlpha0;
    *pfAlphaSlope = fScale / g_fPixelShaderScale;
    *pfBlueSlope = fScale * (1.f/255.f); // blue ramp adds more levels to the ramp

    // align peak of map to center by calculating the quantized alpha value
    /*
    *pfBlueOffset = 0.5f;   // zero biased up by 0.5f
    float fZeroDesired = (fQuantizedDepth0 - fDepth0) / (fDepth1 - fDepth0);
    float fZero = FUnitMap(fAlpha0, fBlue0, *pfAlphaOffset, *pfAlphaSlope, *pfBlueOffset, *pfBlueSlope);
    float fOneDesired = (fQuantizedDepth1 - fDepth0) / (fDepth1 - fDepth0);
    float fOne = FUnitMap(fAlpha1, fBlue1, *pfAlphaOffset, *pfAlphaSlope, *pfBlueOffset, *pfBlueSlope);
    *pfBlueOffset = 0.5f * (fZeroDesired-fZero + fOneDesired-fOne) + 0.5f;  // biased up by 0.5f
    */
    *pfBlueOffset = fBlue0;
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawDepthRange
// Desc: Display the depth buffer mapped to focus values using pixel shader
//       arithmetic.  See media/shaders/depth.psh for more details.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawDepthRange()
{
    // Make a D3DTexture wrapper around the depth buffer surface
    D3DSURFACE_DESC desc;
    D3DSurface *pZBuffer;
    m_pd3dDevice->GetDepthStencilSurface( &pZBuffer );
    pZBuffer->GetDesc(&desc);
    D3DTexture ZBufferTexture;
    ZeroMemory( &ZBufferTexture, sizeof(ZBufferTexture) );
    static D3DFORMAT fmtOverride = D3DFMT_LIN_A8B8G8R8;
    XGSetTextureHeader( desc.Width, desc.Height, 1, 0, fmtOverride, 0, &ZBufferTexture, pZBuffer->Data, desc.Width * 4);
    pZBuffer->Release();

    D3DTexture *pTexture = &ZBufferTexture;
    static D3DCOLOR colorBlend = 0xffffffff;

    struct BACKGROUNDVERTEX { D3DXVECTOR4 p; FLOAT tu, tv; } v[4];
    v[0].p = D3DXVECTOR4( -0.5f,             -0.5f,              1.0f, 1.0f ); v[0].tu = 0.0f;              v[0].tv = 0.0f;
    v[1].p = D3DXVECTOR4( desc.Width - 0.5f, -0.5f,              1.0f, 1.0f ); v[1].tu = (float)desc.Width; v[1].tv = 0.0f;
    v[2].p = D3DXVECTOR4( -0.5f,             desc.Height - 0.5f, 1.0f, 1.0f ); v[2].tu = 0.0f;              v[2].tv = (float)desc.Height;
    v[3].p = D3DXVECTOR4( desc.Width - 0.5f, desc.Height - 0.5f, 1.0f, 1.0f ); v[3].tu = (float)desc.Width; v[3].tv = (float)desc.Height;
    
    // Set pixel shader states
    g_pd3dDevice->SetPixelShader( m_dwDepthPixelShader );
    float fAlphaOffset, fAlphaSlope, fBlueOffset, fBlueSlope;
    CalculateDepthMapping(m_fDepth0, m_fDepth1, &fAlphaOffset, &fAlphaSlope, &fBlueOffset, &fBlueSlope);
    float rfConstants[] = {
        0.f, 0.f, fBlueOffset, fAlphaOffset,        // offset
        0.f, 0.f, fBlueSlope, 0.f,                  // 1x
        0.f, 0.f, 0.f, 0.f,                         // 4x
        0.f, 0.f, 0.f, fAlphaSlope,                 // 16x
    };
    g_pd3dDevice->SetPixelShaderConstant( 0, rfConstants, 4 );
    D3DXCOLOR blend(colorBlend);
    g_pd3dDevice->SetPixelShaderConstant( 4, &blend, 1 );

    g_pd3dDevice->SetTexture( 0, pTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );

    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);

    // Render the screen-aligned quadrilateral
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADSTRIP, 1, v, sizeof(BACKGROUNDVERTEX) );

    // Reset render states
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    g_pd3dDevice->SetTexture( 0, NULL );
    g_pd3dDevice->SetTexture( 1, NULL );
    g_pd3dDevice->SetPixelShader( 0 );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawDepthLookup
// Desc: Display the depth buffer mapped through the lookup texture. This
//       function is for demonstrating the range of z values mapped to
//       focus values.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawDepthLookup()
{
    // Make a D3DTexture wrapper around the depth buffer surface
    D3DSURFACE_DESC desc;
    D3DSurface *pZBuffer;
    m_pd3dDevice->GetDepthStencilSurface( &pZBuffer );
    pZBuffer->GetDesc(&desc);
    D3DTexture ZBufferTexture;
    ZeroMemory( &ZBufferTexture, sizeof(ZBufferTexture) );
    static D3DFORMAT fmtOverride = D3DFMT_LIN_A8R8G8B8;
    XGSetTextureHeader( desc.Width, desc.Height, 1, 0, fmtOverride, 0, &ZBufferTexture, pZBuffer->Data, desc.Width * 4);
    pZBuffer->Release();

    D3DTexture *pTexture = &ZBufferTexture;
    static D3DCOLOR colorBlend = 0xffffffff;

    struct BACKGROUNDVERTEX { D3DXVECTOR4 p; FLOAT tu, tv; } v[4];
    v[0].p = D3DXVECTOR4( -0.5f,             -0.5f,              1.0f, 1.0f ); v[0].tu = 0.0f;              v[0].tv = 0.0f;
    v[1].p = D3DXVECTOR4( desc.Width - 0.5f, -0.5f,              1.0f, 1.0f ); v[1].tu = (float)desc.Width; v[1].tv = 0.0f;
    v[2].p = D3DXVECTOR4( -0.5f,             desc.Height - 0.5f, 1.0f, 1.0f ); v[2].tu = 0.0f;              v[2].tv = (float)desc.Height;
    v[3].p = D3DXVECTOR4( desc.Width - 0.5f, desc.Height - 0.5f, 1.0f, 1.0f ); v[3].tu = (float)desc.Width; v[3].tv = (float)desc.Height;

    // Set the filter modes
    D3DXCOLOR blend(colorBlend);
    g_pd3dDevice->SetTexture( 0, pTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    static D3DTEXTUREFILTERTYPE dwMin0 = D3DTEXF_LINEAR;
    static D3DTEXTUREFILTERTYPE dwMag0 = D3DTEXF_LINEAR;
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, dwMin0 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, dwMag0 );
    g_pd3dDevice->SetTexture( 1, m_pTextureFocusRange );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    static D3DTEXTUREFILTERTYPE dwMin1 = D3DTEXF_LINEAR;
    static D3DTEXTUREFILTERTYPE dwMag1 = D3DTEXF_LINEAR;
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, dwMin1 );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, dwMag1 );
    g_pd3dDevice->SetPixelShader( m_dwDepthLookupPixelShader );
    g_pd3dDevice->SetPixelShaderConstant( 0, &blend, 1 );

    // Set render state
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);

    // Render the screen-aligned quadrilateral
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADSTRIP, 1, v, sizeof(BACKGROUNDVERTEX) );

    // Reset render states
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    g_pd3dDevice->SetTexture( 0, NULL );
    g_pd3dDevice->SetTexture( 1, NULL );
    g_pd3dDevice->SetPixelShader( 0 );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawFocusRange
//
// Desc: Choose the focus range by mapping z to a focus value using pixel
//       shader arithmetic.  See media/shaders/focus.psh for more details.
//
//       High focus values leave the back-buffer unchanged.
//       Low focus values blend in the blurred texture computed by Blur().
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawFocusRange()
{
    // Make a D3DTexture wrapper around the depth buffer surface
    D3DSURFACE_DESC desc;
    D3DSurface *pZBuffer;
    m_pd3dDevice->GetDepthStencilSurface( &pZBuffer );
    pZBuffer->GetDesc(&desc);
    D3DTexture ZBufferTexture;
    ZeroMemory( &ZBufferTexture, sizeof(ZBufferTexture) );
    static D3DFORMAT fmtOverride = D3DFMT_LIN_A8B8G8R8;
    XGSetTextureHeader( desc.Width, desc.Height, 1, 0, fmtOverride, 0, &ZBufferTexture, pZBuffer->Data, desc.Width * 4);
    pZBuffer->Release();
    D3DTexture *pZBufferTexture = &ZBufferTexture;

    // Get size of blur texture for setting texture coords of final blur
    D3DSURFACE_DESC descBlur;
    m_pBlur->GetLevelDesc(0, &descBlur);
    float fOffsetX = 0.f;
    float fOffsetY = 0.5f / (float)descBlur.Height; // vertical blur
    struct VERTEX {
        D3DXVECTOR4 p;
        FLOAT tu0, tv0;
        FLOAT tu1, tv1;
    } v[4];
    v[0].p = D3DXVECTOR4( -0.5f,             -0.5f,              1.0f, 1.0f );
    v[1].p = D3DXVECTOR4( desc.Width - 0.5f, -0.5f,              1.0f, 1.0f );
    v[2].p = D3DXVECTOR4( -0.5f,             desc.Height - 0.5f, 1.0f, 1.0f );
    v[3].p = D3DXVECTOR4( desc.Width - 0.5f, desc.Height - 0.5f, 1.0f, 1.0f );
    v[0].tu0 = 0.0f;              v[0].tv0 = 0.0f;
    v[1].tu0 = (float)desc.Width; v[1].tv0 = 0.0f;
    v[2].tu0 = 0.0f;              v[2].tv0 = (float)desc.Height;
    v[3].tu0 = (float)desc.Width; v[3].tv0 = (float)desc.Height;
    v[0].tu1 = fOffsetX;                         v[0].tv1 = fOffsetY;
    v[1].tu1 = fOffsetX + (float)descBlur.Width; v[1].tv1 = fOffsetY;
    v[2].tu1 = fOffsetX;                         v[2].tv1 = fOffsetY + (float)descBlur.Height;
    v[3].tu1 = fOffsetX + (float)descBlur.Width; v[3].tv1 = fOffsetY + (float)descBlur.Height;
    
    // Set pixel shader state
    g_pd3dDevice->SetPixelShader( m_dwFocusPixelShader );
    float fAlphaOffset, fAlphaSlope, fBlueOffset, fBlueSlope;
    CalculateDepthMapping(m_fDepth0, m_fDepth1, &fAlphaOffset, &fAlphaSlope, &fBlueOffset, &fBlueSlope);
    float rfConstants[] = {
        0.f, 0.f, fBlueOffset, fAlphaOffset,        // offset
        0.f, 0.f, fBlueSlope, 0.f,                  // 1x
        0.f, 0.f, 0.f, 0.f,                         // 4x
        0.f, 0.f, 0.f, fAlphaSlope,                 // 16x
    };
    g_pd3dDevice->SetPixelShaderConstant( 0, rfConstants, 4 );
    
    // Set render state
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);

    // Set texture state
    g_pd3dDevice->SetTexture( 0, pZBufferTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    static D3DTEXTUREFILTERTYPE dwMin0 = D3DTEXF_LINEAR;
    static D3DTEXTUREFILTERTYPE dwMag0 = D3DTEXF_LINEAR;
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, dwMin0 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, dwMag0 );
    g_pd3dDevice->SetTexture( 1, m_pBlur );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    static D3DTEXTUREFILTERTYPE dwMin1 = D3DTEXF_LINEAR;
    static D3DTEXTUREFILTERTYPE dwMag1 = D3DTEXF_LINEAR;
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, dwMin1 );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, dwMag1 );

    // Disable texture stages 2 and 3
    g_pd3dDevice->SetTexture( 2, NULL );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    g_pd3dDevice->SetTexture( 3, NULL );
    g_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP, D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 3, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    
    // Render the screen-aligned quadrilateral
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX4 );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADSTRIP, 1, v, sizeof(VERTEX) );

    // Reset render states
    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    g_pd3dDevice->SetTexture( 0, NULL );
    g_pd3dDevice->SetTexture( 1, NULL );
    g_pd3dDevice->SetTexture( 2, NULL );
    g_pd3dDevice->SetTexture( 3, NULL );
    g_pd3dDevice->SetPixelShader( 0 );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawFocusLookup
//
// Desc: Choose the focus range by mapping z through a lookup texture.
//
//       See media/shaders/focuslookup.psh for more detail.
//
//       This technique has lower performance than using DrawFocus(),
//       but the focus values can be arbitrary, rather than the
//       limited types of z-to-focus value mappings available with
//       pixel shader arithmetic.
//
//       High focus values leave the back-buffer unchanged.
//       Low focus values blend in the blurred texture computed by Blur().
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawFocusLookup()
{
    // Make a D3DTexture wrapper around the depth buffer surface
    D3DSURFACE_DESC desc;
    D3DSurface *pZBuffer;
    m_pd3dDevice->GetDepthStencilSurface( &pZBuffer );
    pZBuffer->GetDesc(&desc);
    D3DTexture ZBufferTexture;
    ZeroMemory( &ZBufferTexture, sizeof(ZBufferTexture) );
    static D3DFORMAT fmtOverride = D3DFMT_LIN_A8R8G8B8;
    XGSetTextureHeader( desc.Width, desc.Height, 1, 0, fmtOverride, 0, &ZBufferTexture, pZBuffer->Data, desc.Width * 4);
    pZBuffer->Release();
    D3DTexture *pZBufferTexture = &ZBufferTexture;

    // Get size of blur texture for setting texture coords of final blur
    D3DSURFACE_DESC descBlur;
    m_pBlur->GetLevelDesc(0, &descBlur);
    float fOffsetX = 0.f;
    float fOffsetY = 0.5f / (float)descBlur.Height; // vertical blur

    struct VERTEX {
        D3DXVECTOR4 p;
        FLOAT tu0, tv0;
        FLOAT tu1, tv1;
        FLOAT tu2, tv2;
        FLOAT tu3, tv3;
    } v[4];
    v[0].p = D3DXVECTOR4( -0.5f,             -0.5f,              1.0f, 1.0f );
    v[1].p = D3DXVECTOR4( desc.Width - 0.5f, -0.5f,              1.0f, 1.0f );
    v[2].p = D3DXVECTOR4( -0.5f,             desc.Height - 0.5f, 1.0f, 1.0f );
    v[3].p = D3DXVECTOR4( desc.Width - 0.5f, desc.Height - 0.5f, 1.0f, 1.0f );
    v[0].tu0 = 0.0f;              v[0].tv0 = 0.0f;
    v[1].tu0 = (float)desc.Width; v[1].tv0 = 0.0f;
    v[2].tu0 = 0.0f;              v[2].tv0 = (float)desc.Height;
    v[3].tu0 = (float)desc.Width; v[3].tv0 = (float)desc.Height;
    // tu1 and tv1 are ignored
    // offset final set of texture coords to apply an additional blur
    v[0].tu2 = -fOffsetX;                         v[0].tv2 = -fOffsetY;
    v[1].tu2 = -fOffsetX + (float)descBlur.Width; v[1].tv2 = -fOffsetY;
    v[2].tu2 = -fOffsetX;                         v[2].tv2 = -fOffsetY + (float)descBlur.Height;
    v[3].tu2 = -fOffsetX + (float)descBlur.Width; v[3].tv2 = -fOffsetY + (float)descBlur.Height;
    v[0].tu3 =  fOffsetX;                         v[0].tv3 =  fOffsetY;
    v[1].tu3 =  fOffsetX + (float)descBlur.Width; v[1].tv3 =  fOffsetY;
    v[2].tu3 =  fOffsetX;                         v[2].tv3 =  fOffsetY + (float)descBlur.Height;
    v[3].tu3 =  fOffsetX + (float)descBlur.Width; v[3].tv3 =  fOffsetY + (float)descBlur.Height;

    // Set pixel shader
    g_pd3dDevice->SetPixelShader( m_dwFocusLookupPixelShader );

    // Set render state
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);

    // Set texture state
    g_pd3dDevice->SetTexture( 0, pZBufferTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    static D3DTEXTUREFILTERTYPE dwMin0 = D3DTEXF_LINEAR;
    static D3DTEXTUREFILTERTYPE dwMag0 = D3DTEXF_LINEAR;
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, dwMin0 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, dwMag0 );
    g_pd3dDevice->SetTexture( 1, m_pTextureFocusRange );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    static D3DTEXTUREFILTERTYPE dwMin1 = D3DTEXF_LINEAR;
    static D3DTEXTUREFILTERTYPE dwMag1 = D3DTEXF_LINEAR;
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, dwMin1 );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, dwMag1 );
    g_pd3dDevice->SetTexture( 2, m_pBlur );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    static D3DTEXTUREFILTERTYPE dwMin2 = D3DTEXF_LINEAR;
    static D3DTEXTUREFILTERTYPE dwMag2 = D3DTEXF_LINEAR;
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_MINFILTER, dwMin2 );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_MAGFILTER, dwMag2 );
    g_pd3dDevice->SetTexture( 3, m_pBlur );
    g_pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
    static D3DTEXTUREFILTERTYPE dwMin3 = D3DTEXF_LINEAR;
    static D3DTEXTUREFILTERTYPE dwMag3 = D3DTEXF_LINEAR;
    g_pd3dDevice->SetTextureStageState( 3, D3DTSS_MINFILTER, dwMin3 );
    g_pd3dDevice->SetTextureStageState( 3, D3DTSS_MAGFILTER, dwMag3 );
    
    // Render the screen-aligned quadrilateral
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX4 );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADSTRIP, 1, v, sizeof(VERTEX) );

    // Reset render states
    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    g_pd3dDevice->SetTexture( 0, NULL );
    g_pd3dDevice->SetTexture( 1, NULL );
    g_pd3dDevice->SetTexture( 2, NULL );
    g_pd3dDevice->SetTexture( 3, NULL );
    g_pd3dDevice->SetPixelShader( 0 );

    return S_OK;
}
