//-----------------------------------------------------------------------------
// File: PlayField.cpp
//
// Desc: Grassy sports field.
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include "Ball.h"

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
	{ XBHELP_LEFTSTICK,	   XBHELP_PLACEMENT_1, L"Move" },
	{ XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_1, L"Tilt" },
	{ XBHELP_A_BUTTON,	   XBHELP_PLACEMENT_1, L"Launch ball" },
	{ XBHELP_X_BUTTON,	   XBHELP_PLACEMENT_2, L"Next\nfield texture" },
	{ XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_2, L"Reload\nvertex buffers" },
	{ XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_1, L"Wire-frame" },
	{ XBHELP_MISC_CALLOUT, XBHELP_PLACEMENT_1, L"Use triggers for height" },
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Display help" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};

#define NUM_HELP_CALLOUTS (sizeof(g_HelpCallouts) / sizeof(XBHELP_CALLOUT))

//-----------------------------------------------------------------------------
// The grass on the sports field is drawn as a series of strips, drawn from
// back-to-front and tilted with respect to the viewer.  The tilt allows a
// wider range of viewing angles.
//-----------------------------------------------------------------------------
#define FEET 0.3048f	/* meters / foot */

#define GRASS_FIELD_LENGTH	(400.f * FEET)
#define GRASS_FIELD_WIDTH	(200.f * FEET)
#define GRASS_HEIGHT		(0.5f * FEET)

D3DXVECTOR3 g_vMin(-GRASS_FIELD_LENGTH / 2.f, 0.f, -GRASS_FIELD_WIDTH / 2.f);
D3DXVECTOR3 g_vSize(GRASS_FIELD_LENGTH, GRASS_HEIGHT, GRASS_FIELD_WIDTH);

struct FieldVertex {
	D3DXVECTOR3	position;
	D3DXVECTOR2	tex0;	// field texture coords
	D3DXVECTOR2	tex1;	// grass top-view texture coords
};

struct GrassVertex {
	D3DXVECTOR3	position;
	D3DXVECTOR2	tex0;	// field texture coords
	D3DXVECTOR2	tex1;	// grass strip side-view texture coords
};

//-----------------------------------------------------------------------------
// There are three textures used to draw the grass.
//
// 1) The whole field texture gives large-scale variation to the grass,
// 	  such as chaulk lines or dirt patches. The alpha channel of the
// 	  whole-field texture modulates where the grass slices are
// 	  drawn. Look closely at the edge of the green while using the
// 	  "Fairway" texture to see the transition.
//
// 2) The GrassStrip texture is applied to the grass strips to give the side
//    view of the grass.
//
// 3) The top-view Grass texture gives high-frequency detail to regions
//    where the slice-texture grass is bare.
//
//-----------------------------------------------------------------------------

#include "Resource.h"
XBRESOURCE g_ResourceNames[Resource_NUM_RESOURCES] =
{
	{ "Fairway", Resource_Fairway_OFFSET },		// whole-field golf-style texture
	{ "Football", Resource_Football_OFFSET },		// whole-field football texture
    { "GrassStrip",  Resource_GrassStrip_OFFSET },	// grass side-view texture
	{ "Grass", Resource_Grass_OFFSET }, // grass top-view texture
};

// The first range of textures can be switched to give different types of fields.
UINT g_nFieldTexture = 2;

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

    // Font and help
    CXBFont     m_Font;
    CXBHelp     m_Help;
    bool        m_bDrawHelp;
	bool		m_bWireFrame;

	// Textures
    CXBPackedResource m_xprResources;
	UINT m_iCurrentFieldTexture;	// index into g_FieldNames to set m_pFieldTexture
    LPDIRECT3DTEXTURE8 m_pFieldTexture;
    LPDIRECT3DTEXTURE8 m_pFieldGrassTexture;
	LPDIRECT3DTEXTURE8 m_pGrassStripTexture;
	HRESULT SetCurrentFieldTexture(UINT i);
	
	// Field geometry
	LPDIRECT3DVERTEXBUFFER8 m_pFieldVertexBuffer;
	HRESULT	FillFieldVertexBuffer();
	
	// Grass geometry
	UINT m_nGrassStripCount;
	UINT m_nGrassQuadCountPerStrip;
	LPDIRECT3DVERTEXBUFFER8 m_pGrassVertexBuffer;
	HRESULT	FillGrassVertexBuffer();

	// Camera position and direction
	D3DXVECTOR3 m_vFrom;
	float m_fAzim, m_fTilt;
	D3DXMATRIX m_matWorld;

	// A ball to bounce and interact with the grass
#define BALLCOUNT 2
	CBall *m_rpBall[BALLCOUNT];
	float m_fCountDown;
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
// Name: GetTerrainColors
// Desc: Get the 4 texture colors in the neighborhood of vPosition
//-----------------------------------------------------------------------------
static LPDIRECT3DTEXTURE8 s_pFieldTexture = NULL;
static Swizzler s_swizzler;

HRESULT GetTerrainColors(const D3DXVECTOR3 &vPosition, D3DCOLOR *rColor, float *pfXFraction, float *pfZFraction)
{
	LPDIRECT3DTEXTURE8 pTexture = s_pFieldTexture;
	if (pTexture)
	{
		D3DSURFACE_DESC desc;
		pTexture->GetLevelDesc(0, &desc);
		float fx = (vPosition.x - g_vMin.x) / g_vSize.x * (float)desc.Width - 0.5f;
		float fz = (vPosition.z - g_vMin.z) / g_vSize.z * (float)desc.Height - 0.5f;
		LONG x = (LONG)fx;
		LONG z = (LONG)fz;
		if (pfXFraction)
			*pfXFraction = fx - (float)x;
		if (pfZFraction)
			*pfZFraction = fz - (float)z;
		if (x >= -1 && x < (LONG)desc.Width &&
			z >= -1 && z < (LONG)desc.Height)
		{
			D3DLOCKED_RECT lockedRect;
			if ( SUCCEEDED(pTexture->LockRect(0, &lockedRect, NULL, 0) ))
			{
				SWIZNUM s;
				if (x >= 0 
					&& z >= 0)
				{
					s = s_swizzler.Swizzle(x, z, 0);
					rColor[0] = *((DWORD *)(lockedRect.pBits) + s);	// we're using a A8R8G8B8 texture
				}
				if (x + 1 < (LONG)desc.Width
					&& z >= 0)
				{
					s = s_swizzler.Swizzle(x + 1, z, 0);
					rColor[1] = *((DWORD *)(lockedRect.pBits) + s);
				}
				if (x >= 0 
					&& z + 1 < (LONG)desc.Height)
				{
					s = s_swizzler.Swizzle(x, z + 1, 0);
					rColor[2] = *((DWORD *)(lockedRect.pBits) + s);
				}
				if (x + 1 < (LONG)desc.Width 
					&& z + 1  < (LONG)desc.Height)
				{
					s = s_swizzler.Swizzle(x + 1, z + 1, 0);
					rColor[3] = *((DWORD *)(lockedRect.pBits) + s);
				}
				pTexture->UnlockRect(0);
				return S_OK;
			}
		}
	}
	return S_FALSE;
}

HRESULT SetTerrainColors(const D3DXVECTOR3 &vPosition, D3DCOLOR *rColor)
{
	LPDIRECT3DTEXTURE8 pTexture = s_pFieldTexture;
	if (pTexture)
	{
		D3DSURFACE_DESC desc;
		pTexture->GetLevelDesc(0, &desc);
		float fx = (vPosition.x - g_vMin.x) / g_vSize.x * (float)desc.Width - 0.5f;
		float fz = (vPosition.z - g_vMin.z) / g_vSize.z * (float)desc.Height - 0.5f;
		LONG x = (LONG)fx;
		LONG z = (LONG)fz;
		if (x >= -1 && x < (LONG)desc.Width &&
			z >= -1 && z < (LONG)desc.Height)
		{
			D3DLOCKED_RECT lockedRect;
			if ( SUCCEEDED(pTexture->LockRect(0, &lockedRect, NULL, 0) ))
			{
				SWIZNUM s;
				if (x >= 0 
					&& z >= 0)
				{
					s = s_swizzler.Swizzle(x, z, 0);
					*((DWORD *)(lockedRect.pBits) + s) = rColor[0];	// we're using a A8R8G8B8 texture
				}
				if (x + 1 < (LONG)desc.Width
					&& z >= 0)
				{
					s = s_swizzler.Swizzle(x + 1, z, 0);
					*((DWORD *)(lockedRect.pBits) + s) = rColor[1];
				}
				if (x >= 0 
					&& z + 1 < (LONG)desc.Height)
				{
					s = s_swizzler.Swizzle(x, z + 1, 0);
					*((DWORD *)(lockedRect.pBits) + s) = rColor[2];
				}
				if (x + 1 < (LONG)desc.Width 
					&& z + 1  < (LONG)desc.Height)
				{
					s = s_swizzler.Swizzle(x + 1, z + 1, 0);
					*((DWORD *)(lockedRect.pBits) + s) = rColor[3];
				}
				pTexture->UnlockRect(0);
				return S_OK;
			}
		}
	}
	return S_FALSE;
}


//-----------------------------------------------------------------------------
// Name: GetTerrainPoint
// Desc: Get the value of the terrain below the given position.
//-----------------------------------------------------------------------------
HRESULT GetTerrainPoint(const D3DXVECTOR3 &vPosition, float *pfGrassThickness, float *pfHeight)
{
	*pfHeight = 0.f;	// this should look up a value in a height field for more interesting terrain
	*pfGrassThickness = 0.3f * FEET;	// default grass thickness
	D3DCOLOR rColor[4];
	if (GetTerrainColors(vPosition, rColor, NULL, NULL) == S_OK)
	{
		DWORD dwAlpha = rColor[0] >> 24;
		*pfGrassThickness *= (float)dwAlpha * (1.f/255.f);
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: MarkTerrain
// Desc: Adds color to current value
//-----------------------------------------------------------------------------
HRESULT MarkTerrain(const D3DXVECTOR3 &vPosition, 
					const D3DXCOLOR &colorScale, 
					const D3DXCOLOR &colorOffset, 
					const D3DXCOLOR &colorMin, 
					D3DCOLOR *rColorPrevious)
{
	D3DCOLOR rColorTmp[4];
	if (rColorPrevious == NULL)
		rColorPrevious = rColorTmp;
	float xfrac, zfrac;
	if (GetTerrainColors(vPosition, rColorPrevious, &xfrac, &zfrac) == S_OK)
	{
		int i;
		float frac[4] = {
			(1.f - xfrac) * (1.f - zfrac),
			xfrac * (1.f - zfrac),
			(1.f - xfrac) * zfrac,
			xfrac * zfrac 
		};
		// Compute interpolated color
		D3DXCOLOR color0 = D3DXCOLOR(rColorPrevious[0]) * frac[0];
		for (i = 1; i < 4; i++)
			color0 += frac[i] * D3DXCOLOR(rColorPrevious[i]);

		// Apply color scaling to interpolated value
		D3DXCOLOR color = color0;
		color.r *= colorScale.r;
		color.g *= colorScale.g;
		color.b *= colorScale.b;
		color.a *= colorScale.a;
		color += colorOffset;
		if (color.r < colorMin.r) color.r = colorMin.r;
		if (color.g < colorMin.g) color.g = colorMin.g;
		if (color.b < colorMin.b) color.b = colorMin.b;
		if (color.a < colorMin.a) color.a = colorMin.a;

		// Add weighted delta to the four corner points
		D3DXCOLOR colorDelta = color - color0;
		D3DCOLOR rColor[4];
		for (i = 0; i < 4; i++)
		{
			D3DXCOLOR colorNew = D3DXCOLOR(rColorPrevious[i]) + frac[i] * colorDelta;
			rColor[i] = (D3DCOLOR)colorNew;
		}
		return SetTerrainColors(vPosition, rColor);
	}
	return S_FALSE;
}

//-----------------------------------------------------------------------------
// Name: CXBoxSample (constructor)
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :CXBApplication()
{
#ifdef _DEBUG	
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	// Allow unlimited frame rate
#endif
    m_bDrawHelp = false;
    m_bWireFrame = false;
    m_pFieldTexture = NULL;
    m_pFieldGrassTexture = NULL;
	m_pGrassStripTexture = NULL;
	m_pFieldVertexBuffer = NULL;
	m_pGrassVertexBuffer = NULL;
	for (UINT iBall = 0; iBall < BALLCOUNT; iBall++)
		m_rpBall[iBall] = NULL;
}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

	// Load the field and grass textures
    if( FAILED( m_xprResources.Create( m_pd3dDevice, "Resource.xpr", Resource_NUM_RESOURCES, g_ResourceNames ) ) )
        return E_FAIL;
	SetCurrentFieldTexture(0);
	m_pFieldGrassTexture = m_xprResources.GetTexture( Resource_Grass_OFFSET );
	m_pGrassStripTexture = m_xprResources.GetTexture( Resource_GrassStrip_OFFSET );
	
	// Fill the field vertex buffer
	if ( FAILED( FillFieldVertexBuffer() ) )
		return E_FAIL;

	// Fill the grass vertex buffer
	if ( FAILED( FillGrassVertexBuffer() ) )
		return E_FAIL;

	// Camera position and orientation
	m_vFrom = D3DXVECTOR3(50.f * 3.f * FEET, 6.f * FEET, 0.f);
	m_fAzim = 0.f;
	m_fTilt = 0.f;
	
	// set light parameters
	D3DLIGHT8 light;
	ZeroMemory(&light, sizeof(D3DLIGHT8));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Position = D3DXVECTOR3(10000.f, 10000.f, 10000.f); // sun
	light.Direction = D3DXVECTOR3(0.577350269f, -0.577350269f, 0.577350269f);
	light.Ambient  = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f);
	light.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	light.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->LightEnable(0, TRUE);

	// Load the balls
	for (UINT iBall = 0; iBall < BALLCOUNT; iBall++)
	{
		m_rpBall[iBall] = new CBall;
		if ( FAILED(m_rpBall[iBall]->Initialize()) )
			return XBAPPERR_MEDIANOTFOUND;
	}
	m_fCountDown = 0.f;
	
    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: SetCurrentFieldTexture
// Desc: Assign current field texture and setup the GetTerrainPoint functions
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::SetCurrentFieldTexture(UINT i)
{ 
	m_iCurrentFieldTexture = i;
	if (m_iCurrentFieldTexture >= g_nFieldTexture)
		m_iCurrentFieldTexture = 0;
	m_pFieldTexture = m_xprResources.GetTexture( g_ResourceNames[m_iCurrentFieldTexture].dwOffset );

	s_pFieldTexture = m_pFieldTexture;
	D3DSURFACE_DESC desc;
	s_pFieldTexture->GetLevelDesc(0, &desc);
	s_swizzler.Init(desc.Width, desc.Height, 1);

	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FillFieldVertexBuffer
// Desc: Set vertex positions for whole field
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FillFieldVertexBuffer()
{
	HRESULT hr;
	UINT cbFieldVertexSize = sizeof(FieldVertex);
	UINT length = 2;
	UINT width = 2;
	static float fXScale = 60.f;	// repeat scaling of grass top-view texture
	static float fZScale = 30.f;
	D3DXVECTOR3 vMin(-GRASS_FIELD_LENGTH / 2.f, 0.f, -GRASS_FIELD_WIDTH / 2.f);
	D3DXVECTOR3 vSize(GRASS_FIELD_LENGTH, 1.f, GRASS_FIELD_WIDTH);
	SAFE_RELEASE(m_pFieldVertexBuffer);
	hr = g_pd3dDevice->CreateVertexBuffer(length * width * cbFieldVertexSize, 0, 0, D3DPOOL_DEFAULT, &m_pFieldVertexBuffer);
	if (FAILED(hr))
		return hr;
	FieldVertex *pVertices;
	hr = m_pFieldVertexBuffer->Lock(0, 0, (BYTE**)&pVertices, 0);
	if (FAILED(hr))
		return hr;
	for (UINT iXGrid = 0; iXGrid < length; iXGrid++)
	{
		float fX = (float)iXGrid / (float)(length - 1);
		for (UINT iZGrid = 0; iZGrid < width; iZGrid++)
		{
			float fZ = (float)iZGrid / (float)(width - 1);
			FieldVertex *pVertex;
			pVertex = &pVertices[iXGrid * width + iZGrid];
			pVertex->position.x = vMin.x + vSize.x * fX;
			pVertex->position.y = vMin.y;
			pVertex->position.z = vMin.z + vSize.z * fZ;
			pVertex->tex0.x = fX;
			pVertex->tex0.y = fZ;
			pVertex->tex1.x = fX * fXScale;
			pVertex->tex1.y = fZ * fZScale;
		}
	}
	m_pFieldVertexBuffer->Unlock();
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FillGrassVertexBuffer
// Desc: Set vertex positions for grass vertex buffer
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::FillGrassVertexBuffer()
{
	HRESULT hr;
	UINT cbGrassVertexSize = sizeof(GrassVertex);
	static UINT length = 800;	// number of strips
	static UINT width = 2;	// number of horizontal samples per strip, must be >= 2
	static float fGrassHeight = GRASS_HEIGHT;
	static float fTilt = -1.f * FEET;			// strips are tilted away from the current view
	static float fZScale = 50.f / 8.f;	// texture coordinate scaling
	static float fYScale = 1.f;
	static UINT seed = 0x123456;	// start with the same random seed so the per-strip offsets are the same from call to call
	srand(seed);
	UINT depth = 2;	// number of vertical samples per strip, currently must be 2 due the drawprimitive call used below
	// Set a break-point here and press the reload button to hit it.  Then, adjust the
	// above parameters to see how they affect the appearance of the grass.
	D3DXVECTOR3 vMin(-GRASS_FIELD_LENGTH / 2.f, 0.f, -GRASS_FIELD_WIDTH / 2.f);
	D3DXVECTOR3 vSize(GRASS_FIELD_LENGTH, fGrassHeight, GRASS_FIELD_WIDTH);
	SAFE_RELEASE(m_pGrassVertexBuffer);
	hr = g_pd3dDevice->CreateVertexBuffer(length * width * depth * cbGrassVertexSize, 0, 0, D3DPOOL_DEFAULT, &m_pGrassVertexBuffer);
	if (FAILED(hr))
		return hr;
	GrassVertex *pVertices;
	hr = m_pGrassVertexBuffer->Lock(0, 0, (BYTE**)&pVertices, 0);
	if (FAILED(hr))
		return hr;
	for (UINT iXGrid = 0; iXGrid < length; iXGrid++)
	{
		float fX = (float)iXGrid / (float)(length - 1);
		float fZOffset = fZScale * (float)rand() / (float)0xffff;	// pick random horizontal texture offset for each strip
		for (UINT iZGrid = 0; iZGrid < width; iZGrid++)
		{
			float fZ = (float)iZGrid / (float)(width - 1);
			for (UINT iYGrid = 0; iYGrid < depth; iYGrid++)
			{
				float fY = (float)iYGrid / (depth - 1);
				GrassVertex *pVertex;
				pVertex = &pVertices[depth * (iXGrid * width + iZGrid) + iYGrid ];
				pVertex->position.x = vMin.x + vSize.x * fX + fTilt * fY;
				pVertex->position.y = vMin.y + vSize.y * fY;
				pVertex->position.z = vMin.z + vSize.z * fZ;
				pVertex->tex0.x = (pVertex->position.x - vMin.x) / vSize.x;
				pVertex->tex0.y = (pVertex->position.z - vMin.z) / vSize.z;
				pVertex->tex1.x = fZ * fZScale + fZOffset;
				pVertex->tex1.y = fYScale * (1.f - fY);
			}
		}
	}
	m_pGrassVertexBuffer->Unlock();
	m_nGrassStripCount = length;
	m_nGrassQuadCountPerStrip = width - 1;
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
    if ( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Toggle wire-frame drawing
    if ( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE]  )
    {
        m_bWireFrame = !m_bWireFrame;
    }

	// Refill vertex buffers
    if ( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] ) 
    {
		FillFieldVertexBuffer();
        FillGrassVertexBuffer();
    }

	// Go to next field texture
	if ( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_X ] )
	{
		SetCurrentFieldTexture(m_iCurrentFieldTexture + 1);
	}

	// Update view position
	float fX1 = m_DefaultGamepad.fX1;
	fX1 *= fX1 * fX1; // fX1 cubed
	float fY1 = m_DefaultGamepad.fY1;
	fY1 *= fY1 * fY1; // fY1 cubed
	static float fScaleFromX = 60.f * FEET;
	static float fScaleFromY = 20.f * FEET;
	static float fScaleFromZ = 60.f * FEET;
	float fDelta = (1.f/255.f) * 
		(m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] 
		 - m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]);
	m_vFrom.x -= fScaleFromX * fY1 * m_fElapsedTime;
	m_vFrom.y += fScaleFromY * fDelta * m_fElapsedTime;
	m_vFrom.z += fScaleFromZ * fX1 * m_fElapsedTime;
	static D3DXVECTOR3 vFromMin = D3DXVECTOR3(-150.f, 2.f, -100.f) * FEET;
	static D3DXVECTOR3 vFromMax = D3DXVECTOR3( 220.f, 125.f, 100.f) * FEET;
	if (m_vFrom.x < vFromMin.x) m_vFrom.x = vFromMin.x;
	if (m_vFrom.x > vFromMax.x) m_vFrom.x = vFromMax.x;
	if (m_vFrom.y < vFromMin.y) m_vFrom.y = vFromMin.y;
	if (m_vFrom.y > vFromMax.y) m_vFrom.y = vFromMax.y;
	if (m_vFrom.z < vFromMin.z) m_vFrom.z = vFromMin.z;
	if (m_vFrom.z > vFromMax.z) m_vFrom.z = vFromMax.z;

	// Update view orientation
	float fX2 = m_DefaultGamepad.fX2;
	fX2 *= fX2 * fX2; // fX2 cubed
	float fY2 = m_DefaultGamepad.fY2;
	fY2 *= fY2 * fY2; // fY2 cubed
	static float fAzimScale = 10.f;
	static float fAzimMin = -0.6f;
	static float fAzimMax = 0.6f;
	m_fAzim += fAzimScale * fX2 * m_fElapsedTime;
	if (m_fAzim < fAzimMin) m_fAzim = fAzimMin;
	if (m_fAzim > fAzimMax) m_fAzim = fAzimMax;
	static float fTiltScale = 3.f;
	static float fTiltMin = -0.3f;
	static float fTiltMax = 0.6f;
	m_fTilt += fTiltScale * fY2 * m_fElapsedTime;
	if (m_fTilt < fTiltMin) m_fTilt = fTiltMin;
	if (m_fTilt > fTiltMax) m_fTilt = fTiltMax;
	D3DXVECTOR3 vAt = m_vFrom + D3DXVECTOR3(-cosf(m_fAzim) * cosf(m_fTilt), -sinf(m_fTilt), sinf(m_fAzim) * cosf(m_fTilt));
	D3DXVECTOR3 vUp(0.f, 1.f, 0.f);

	// Launch a ball
	m_fCountDown -= m_fElapsedAppTime;
	if ( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ]
		|| m_fCountDown < 0.f)
	{
		// Re-use the slowest ball
		UINT iBall = 0;
		float fSpeedMin = D3DXVec3Length(&m_rpBall[iBall]->m_vVelocity);
		for (UINT jBall = 1; jBall < BALLCOUNT; jBall++)
		{
			float fSpeed = D3DXVec3Length(&m_rpBall[jBall]->m_vVelocity);
			if (fSpeed < fSpeedMin)
			{
				fSpeedMin = fSpeed;
				iBall = jBall;
			}
		}
		
		// Make target be intersection of view direction with plane
		D3DXVECTOR3 vRay = vAt - m_vFrom;
		float t;
		if (vRay.y >= 0)
			t = m_vFrom.x * 0.5f;
		else 
			t = -m_vFrom.y / vRay.y;
		D3DXVECTOR3 vTarget = m_vFrom + vRay * t;
		static D3DXVECTOR3 vTargetMin(-GRASS_FIELD_LENGTH / 2.f, 0.f, -GRASS_FIELD_WIDTH / 2.f);
		static D3DXVECTOR3 vTargetMax( GRASS_FIELD_LENGTH / 2.f, 0.f,  GRASS_FIELD_WIDTH / 2.f);
		if (vTarget.x < vTargetMin.x) vTarget.x = vTargetMin.x;
		if (vTarget.x > vTargetMax.x) vTarget.x = vTargetMax.x;
		if (vTarget.y < vTargetMin.y) vTarget.y = vTargetMin.y;
		if (vTarget.y > vTargetMax.y) vTarget.y = vTargetMax.y;
		if (vTarget.z < vTargetMin.z) vTarget.z = vTargetMin.z;
		if (vTarget.z > vTargetMax.z) vTarget.z = vTargetMax.z;
		m_rpBall[iBall]->LaunchBall(m_vFrom, vTarget);

		if (m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ])
			m_fCountDown = 30.f; // if user launched the ball, wait a long time before autolaunching
		else
			m_fCountDown = 5.f;	// countdown between launches
	}

	// Move the balls
	for (UINT iBall = 0; iBall < BALLCOUNT; iBall++)
		m_rpBall[iBall]->MoveBall(m_fElapsedAppTime);

	// Set matrices
	D3DXMATRIX matProjection;
	D3DXMatrixPerspectiveFovLH( &matProjection, D3DX_PI/4, 640.f/480.f, 0.4f, 4000.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProjection);

	D3DXMATRIX matView;
	D3DXMatrixLookAtLH( &matView, &m_vFrom, &vAt, &vUp);
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	
	D3DXMatrixIdentity(&m_matWorld);
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
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0L );

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

	// Draw in wire-frame, if desired
	if (m_bWireFrame)
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	// Draw the balls
	for (UINT iBall = 0; iBall < BALLCOUNT; iBall++)
		m_rpBall[iBall]->Render();

	// Draw the ground below the grass
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
    g_pd3dDevice->SetPixelShader( 0 );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE ); 
    g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE ); 
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);
    g_pd3dDevice->SetTexture( 0, m_pFieldTexture );
	static DWORD dwGroundColor = 0xffffffff;
 	g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwGroundColor );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
	static DWORD dwMaxAnisotropy0 = 4;
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAXANISOTROPY, dwMaxAnisotropy0 ); // make the field texture sharper as it recedes
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTexture( 1, m_pFieldGrassTexture );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
	static DWORD dwMaxAnisotropy1 = 2;
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAXANISOTROPY, dwMaxAnisotropy1 );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTexture( 2, NULL );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZ|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE2(1) );
	g_pd3dDevice->SetStreamSource( 0, m_pFieldVertexBuffer, sizeof(FieldVertex) );
	g_pd3dDevice->DrawPrimitive( D3DPT_QUADSTRIP, 0, 1);

	// Draw the field of grass modulated by the field texture
    g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE ); 
	g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
	g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	// Stage 0 is used for the whole-field texture. Most settings are inherited from above.
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE );	// multiply color by alpha
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	// Stage 1 is used for the side-view grass strip texture
	g_pd3dDevice->SetTexture( 1, m_pGrassStripTexture );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	// Stage 2 is used to fade the strips in the distance
	g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwGroundColor );
	g_pd3dDevice->SetTexture( 2, NULL );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_CURRENT );
	g_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_TFACTOR );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
	g_pd3dDevice->SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZ|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE2(1) );
	g_pd3dDevice->SetStreamSource( 0, m_pGrassVertexBuffer, sizeof(GrassVertex) );
	
	// Fade the grass strips as they recede in the distance to avoid 
	// aliasing near the horizon and to give better performance. 
	// The following parameters can be adjusted to increase or decrease the
	// grass fade-out distance and range.
	static DWORD dwAlphaMin = 5;
	static float fDistMin = 50.f * FEET;	// starting distance for alpha fade
	static float fDistMax = 100.f * FEET;	// strips fade to 0 alpha at this distance
	DWORD dwFade = 0xffffffff;
	g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwFade );

	// get range of active grass strips
	float fScale = GRASS_FIELD_LENGTH / (float)(m_nGrassStripCount - 1);
	float fOffset = -0.5f * GRASS_FIELD_LENGTH;
	UINT iMin;
	float fiMin = (m_vFrom.x - fDistMax - fOffset) / fScale - 1.f;
	if (fiMin < 0) 
		iMin = 0;
	else
		iMin = (UINT)fiMin;
	UINT iMax;
	float fiMax = (m_vFrom.x - fOffset) / fScale + 1.f;
	if (fiMax < 0)
		iMax = 0;
	else
	{
		iMax = (UINT)fiMax;
		if (iMax > m_nGrassStripCount)
			iMax = m_nGrassStripCount;
	}

	// draw each active grass strip
	for (UINT i = iMin; i < iMax; i++)
	{
		float fX = (float)i * fScale + fOffset;
		float fDist = m_vFrom.x - fX;
		if (fDist < 0.f) continue;
		DWORD dwFadeNew;
		if (fDist > fDistMax)
			continue;				// completely off
		else if (fDist < fDistMin)
			dwFadeNew = 0xffffffff;	// fully on
		else
		{
			// in fade region
			float t = (fDist - fDistMin) / (fDistMax - fDistMin);
			float fAlpha = 1.f - (-2.f * t + 3.f) * t * t;
			DWORD dwAlpha = (DWORD)(fAlpha * 255);
			if (dwAlpha < dwAlphaMin) continue;
			if (dwAlpha > 255) dwAlpha = 255;
			dwFadeNew = (dwAlpha << 24) | (dwAlpha << 16) | (dwAlpha << 8) | dwAlpha;
		}
		if (dwFadeNew != dwFade)
		{
			dwFade = dwFadeNew;
			g_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwFade );
		}
		// draw the current grass quad
		g_pd3dDevice->DrawPrimitive( D3DPT_QUADSTRIP, i * (m_nGrassQuadCountPerStrip + 1) * 2, m_nGrassQuadCountPerStrip);
	}	

    // Show title, frame rate, and help
    if( m_bDrawHelp )
        m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
    else
    {
        m_Font.Begin();
        m_Font.DrawText(  64, 50, 0xffffffff, L"PlayField" );
        m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
        m_Font.End();
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}
