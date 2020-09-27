//-----------------------------------------------------------------------------
// File: DisplacementMap.cpp
//
// Desc: Displacement mapping on the Xbox GPU takes advantage of the
//  unified memory architecture.
// 
//  A preprocess step is required to split the "carrier" mesh into
//  small neighborhoods of vertices that can fit into vertex shader
//  constants.
//
//  Instead of using vertex shader constants to do indexed skinning,
//  we use the constants as a way to apply a "geometry texture."  The
//  carrier mesh vertices define a local, warped coordinate system
//  that maps the displacement height field to the surface of the
//  carrier mesh.
//
//  The weights and indices are obtained by rendering to three small
//  textures. The first texture sets the blending weights for each
//  triangle in the carrier mesh.  The second texture sets the palette
//  indices for each triangle.  The third texture samples the
//  displacement map at the current region and level-of-detail.
//  
//  Finally, the blending weights, indices, and offsets calculated in
//  the previous step are passed as textures through the displacement
//  vertex shader. The output position is a blend of the carrier mesh
//  vertices plus the displacement map offset times a blend of the
//  carrier mesh normals.
//
//  NOTE: The weight, index, and sampled displacement textures
//  are used as vertex buffer data and sent through the vertex shader.
//
// Hist: 11.14.01 - Created
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include "Media/Shaders/Displacement.h"

#ifdef _DEBUG
// DebugPixels is useful to see the intermediate texture results.  Set
// a breakpoint in the RenderToDisplacementTextures function, and then
// in a watch window, type DebugPixels(m_pWeightTexture, 0) to display
// the weight texture, or DebugPixels(m_pIndexTexture, 0) to display
// the indices, or DebugPixels(m_pDisplacementTexture, 0) to display
// the displacements.
#pragma comment( linker, "/include:_DebugPixels" )
#include <XBMediaDebug.h>
#endif


// The following header file is generated from "Resource.rdf" file using the
// Bundler tool. In addition to the header, the tool outputs a binary file
// (Resource.xpr) which contains compiled (i.e. bundled) resources and is
// loaded at runtime using the CXBPackedResource class.
#include "Resource.h"


//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
	{ XBHELP_LEFTSTICK,    XBHELP_PLACEMENT_2, L"Rotate\nObject" },
	{ XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Next\nDraw Mode" },
	{ XBHELP_B_BUTTON,     XBHELP_PLACEMENT_2, L"Previous\nDraw Mode" },
	{ XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_2, L"Toggle\nPoint Drawing" },
	{ XBHELP_DPAD,         XBHELP_PLACEMENT_2, L"Select Parameter\nand Channel" },
	{ XBHELP_LEFT_BUTTON,  XBHELP_PLACEMENT_2, L"Decrease\nParameter" },
	{ XBHELP_RIGHT_BUTTON, XBHELP_PLACEMENT_2, L"Increase\nParameter" },
	{ XBHELP_MISC_CALLOUT, XBHELP_PLACEMENT_2, L"Triggers adjust current parameter" },
	{ XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
};

#define NUM_HELP_CALLOUTS ( sizeof( g_HelpCallouts ) / sizeof( g_HelpCallouts[0] ) )


//-----------------------------------------------------------------------------
// Vertex for carrier mesh
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position;   // The position
    D3DXVECTOR3 normal;     // The vertex normals
	FLOAT tu, tv;			// texture coords
};

// Our custom FVF, which describes our custom vertex structure
#define FVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)


struct WEIGHTVERTEX {
	D3DXVECTOR4 xyzrhw;
	D3DCOLOR diffuse;
};

#define FVF_WEIGHTVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

struct INDEXVERTEX {
	D3DXVECTOR4 xyzrhw;
	D3DCOLOR diffuse;
};
#define FVF_INDEXVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)



//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//		 from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
	CXBPackedResource  m_xprResource;	   // Packed resources for the app
	CXBFont			   m_Font;			   // Font object
	CXBHelp			   m_Help;			   // Help object
	BOOL		m_bDrawHelp;		    // TRUE to draw help screen
	
    LPDIRECT3DTEXTURE8 m_rpDisplacementMap[4];	   // Displacement maps
    LPDIRECT3DVERTEXBUFFER8 m_pCarrierVB;  // carrier mesh vertices
    LPDIRECT3DINDEXBUFFER8  m_pCarrierIB;  // carrier mesh triangles
    LPDIRECT3DVERTEXBUFFER8 m_pWeightVB;  // carrier mesh weight vertices
    LPDIRECT3DVERTEXBUFFER8 m_pIndexVB;  // carrier mesh index vertices
#define WIDTH 128
#define HEIGHT 128	
    LPDIRECT3DTEXTURE8 m_pWeightTexture;	// render target for weight texture
	LPDIRECT3DSURFACE8 m_pWeightSurface;	// most detailed weight texture level
	D3DVertexBuffer m_WeightVertexBuffer;	// fake vertex buffer header that points to the weight texture data
	
    LPDIRECT3DTEXTURE8 m_pIndexTexture;	 	// render target for Index texture
	LPDIRECT3DSURFACE8 m_pIndexSurface;	 	// most detailed Index texture level
	D3DVertexBuffer m_IndexVertexBuffer; 	// fake vertex buffer header that points to the index texture data
	
    LPDIRECT3DTEXTURE8 m_pDisplacementTexture;	 	// render target for Displacement texture
	LPDIRECT3DSURFACE8 m_pDisplacementSurface;	 	// most detailed Displacement texture level
	D3DVertexBuffer m_DisplacementVertexBuffer; 	// fake vertex buffer header that points to the index texture data
	LPDIRECT3DINDEXBUFFER8 m_pDisplacementIndexBuffer;	// index buffer to draw quads
	
	DWORD m_dwDisplacementVertexShader;  	// displacement vertex shader
	FLOAT m_rfDisplacementScale[4];
	FLOAT m_rfDisplacementOffset[4];
	FLOAT m_rfMipmapBias[4];
	
	UINT m_iCurrentDisplacementMap;
	enum DrawMode {
		DRAW_DISPLACED_MESH,
		DRAW_MESH,
		DRAW_WEIGHTS,
		DRAW_INDICES,
		_DRAW_MAX
	} m_DrawMode;
	BOOL		m_bDrawPoints;		// TRUE to draw displaced points instead of mesh
	enum TweakMode {
		TWEAK_DISPLACEMENT_SCALE,
		TWEAK_DISPLACEMENT_OFFSET,
		TWEAK_MIPMAP_BIAS,
		_TWEAK_MAX
	} m_TweakMode;					// tweak mode detrmines what triggers do
	FLOAT		m_TweakActive;		// tweaking in progess if > zero
public:
	CXBoxSample();

	HRESULT RenderToDisplacementTextures();
	HRESULT DrawMesh();
	HRESULT DrawWeights();
	HRESULT DrawIndices();
	HRESULT DrawDisplacedMesh();
	

	virtual HRESULT Initialize();
	virtual HRESULT Render();
	virtual HRESULT FrameMove();	
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
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	// Allow unlimited frame rate
#endif
	m_bDrawHelp = FALSE;
	m_dwDisplacementVertexShader = 0;
	m_rpDisplacementMap[0] = NULL;
	m_rpDisplacementMap[1] = NULL;
	m_rpDisplacementMap[2] = NULL;
	m_rpDisplacementMap[3] = NULL;
	m_pCarrierVB = NULL;
	m_pCarrierIB = NULL;
	m_pWeightVB = NULL;
	m_pIndexVB = NULL;
	m_pWeightTexture = NULL;
	m_pWeightSurface = NULL;
	m_pIndexTexture = NULL;
	m_pIndexSurface = NULL;
	m_pDisplacementTexture = NULL;
	m_pDisplacementSurface = NULL;
	m_pDisplacementIndexBuffer = NULL;
	m_dwDisplacementVertexShader = 0;
	m_rfDisplacementScale[0] = 0.1f;
	m_rfDisplacementScale[1] = 0.05f;
	m_rfDisplacementScale[2] = 0.025f;
	m_rfDisplacementScale[3] = 0.0125f;
	m_rfDisplacementOffset[0] = -0.5f * m_rfDisplacementScale[0];
	m_rfDisplacementOffset[1] = -0.5f * m_rfDisplacementScale[1];
	m_rfDisplacementOffset[2] = -0.5f * m_rfDisplacementScale[2];
	m_rfDisplacementOffset[3] = -0.5f * m_rfDisplacementScale[3];
	m_rfMipmapBias[0] =
		m_rfMipmapBias[1] =
		m_rfMipmapBias[2] =
		m_rfMipmapBias[3] = -2.f; // bias displacement maps mip level to be sharper
	m_iCurrentDisplacementMap = 0;
	m_DrawMode = DRAW_DISPLACED_MESH;
	m_bDrawPoints = FALSE;
	m_TweakMode = TWEAK_DISPLACEMENT_SCALE;
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
	if( FAILED( m_xprResource.Create( m_pd3dDevice, "Resource.xpr", 
									  Resource_NUM_RESOURCES, NULL) ) )
		return XBAPPERR_MEDIANOTFOUND;

    // Get texture pointers
    m_rpDisplacementMap[0] = m_xprResource.GetTexture( Resource_BaseOctave1_OFFSET );
	m_rpDisplacementMap[1] = m_xprResource.GetTexture( Resource_Octave2_OFFSET );
	m_rpDisplacementMap[2] = m_xprResource.GetTexture( Resource_Octave3_OFFSET );
	m_rpDisplacementMap[3] = m_xprResource.GetTexture( Resource_Octave2_OFFSET );	// Use scaled version of octave 2 for octave 4

	// Set the carrier mesh vertices
#define SAMPLE_COUNT 8
#define VERTEX_COUNT (SAMPLE_COUNT * SAMPLE_COUNT)
#define TRIANGLE_COUNT ((SAMPLE_COUNT - 1)*(SAMPLE_COUNT - 1)*2)
#define INDEX_COUNT (3*TRIANGLE_COUNT)
	if( FAILED( m_pd3dDevice->CreateVertexBuffer( VERTEX_COUNT*sizeof(CUSTOMVERTEX),
                                                  0, FVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &m_pCarrierVB ) ) )
        return E_FAIL;
    CUSTOMVERTEX *rVertex;
    if( FAILED( m_pCarrierVB->Lock( 0, 0, (BYTE**)&rVertex, 0 ) ) )
        return E_FAIL;
	FLOAT fScale = 1.f / (SAMPLE_COUNT - 1);
	FLOAT fRadius = 1.2f;
    for(UINT i=0; i<SAMPLE_COUNT; i++ )
	{
		FLOAT theta = 0.6f * D3DX_PI * (i * fScale - 1.2f);
		FLOAT s = sinf(theta);
		FLOAT c = cosf(theta);
		for(UINT j=0; j<SAMPLE_COUNT; j++ )
		{
			CUSTOMVERTEX *pv = &rVertex[ i * SAMPLE_COUNT + j ];
			pv->position = D3DXVECTOR3( fScale * j * 2.f - 1.f, fRadius * c, fRadius * s);
			pv->normal = D3DXVECTOR3( 0, fRadius * c, fRadius * s);
			pv->tu = fScale * j;
			pv->tv = fScale * i;
		}
	}

	// Keep the carrier vertex buffer locked to copy out the weight and index vertices
    // m_pCarrierVB->Unlock();

	// Set the carrier mesh indices
	if( FAILED( m_pd3dDevice->CreateIndexBuffer( INDEX_COUNT*sizeof(WORD),
												 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pCarrierIB ) ) )
        return E_FAIL;
	WORD *pIndices;
	if( FAILED( m_pCarrierIB->Lock( 0, 0, (BYTE**)&pIndices, 0 ) ) )
		return E_FAIL;
	UINT iiTri = 0;
	for (WORD iY = 0; iY < SAMPLE_COUNT - 1; iY++ )
	{
		for (WORD iX = 0; iX < SAMPLE_COUNT - 1; iX++ )
		{
			WORD iVertex = iY * SAMPLE_COUNT + iX;

			// tri 0
			pIndices[iiTri] = iVertex;
			pIndices[iiTri+1] = iVertex + SAMPLE_COUNT;
			pIndices[iiTri+2] = iVertex + SAMPLE_COUNT + 1;
			
			// tri 1
			pIndices[iiTri+3] = iVertex;
			pIndices[iiTri+4] = iVertex + SAMPLE_COUNT + 1;
			pIndices[iiTri+5] = iVertex + 1;

			iiTri += 6;
		}
	}
	// Keep the carrier index buffer locked to copy out the weight and index vertices
	// m_pCarrierIB->Unlock();

	// Set the weight mesh, which consists of all the triangles
	// of the base mesh annotated with blend weights r,g,b.
	if( FAILED( m_pd3dDevice->CreateVertexBuffer( INDEX_COUNT*sizeof(CUSTOMVERTEX),
                                                  0, FVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &m_pWeightVB ) ) )
        return E_FAIL;
	WEIGHTVERTEX *pWeightVertex;
	if( FAILED( m_pWeightVB->Lock( 0, 0, (BYTE**)&pWeightVertex, 0 ) ) )
		return E_FAIL;
	FLOAT fXScale = WIDTH;
	FLOAT fXOffset = 0.f;
	FLOAT fYScale = HEIGHT;
	FLOAT fYOffset = 0.f;
	WORD *pIndex = pIndices;
	for (UINT iTriangle = 0; iTriangle < TRIANGLE_COUNT; iTriangle++)
	{
		CUSTOMVERTEX *pV0 = &rVertex[*pIndex++];
		CUSTOMVERTEX *pV1 = &rVertex[*pIndex++];
		CUSTOMVERTEX *pV2 = &rVertex[*pIndex++];
		pWeightVertex->xyzrhw = D3DXVECTOR4(pV0->tu * fXScale + fXOffset, pV0->tv * fYScale + fYOffset, 0.5f, 1.f);
		pWeightVertex->diffuse = D3DXCOLOR( 1.f, 0.f, 0.f, 1.f );
		pWeightVertex++;
		pWeightVertex->xyzrhw = D3DXVECTOR4(pV1->tu * fXScale + fXOffset, pV1->tv * fYScale + fYOffset, 0.5f, 1.f);
		pWeightVertex->diffuse = D3DXCOLOR( 0.f, 1.f, 0.f, 1.f );
		pWeightVertex++;
		pWeightVertex->xyzrhw = D3DXVECTOR4(pV2->tu * fXScale + fXOffset, pV2->tv * fYScale + fYOffset, 0.5f, 1.f);
		pWeightVertex->diffuse = D3DXCOLOR( 0.f, 0.f, 1.f, 1.f );
		pWeightVertex++;
	}
	m_pWeightVB->Unlock();
	
	// Set the index mesh, which consists of all the triangles
	// of the base mesh annotated with blend indices
	if( FAILED( m_pd3dDevice->CreateVertexBuffer( INDEX_COUNT*sizeof(CUSTOMVERTEX),
                                                  0, FVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &m_pIndexVB ) ) )
        return E_FAIL;
	INDEXVERTEX *pIndexVertex;
	if( FAILED( m_pIndexVB->Lock( 0, 0, (BYTE**)&pIndexVertex, 0 ) ) )
		return E_FAIL;
	pIndex = pIndices;
	FLOAT fIndexScale = 1.f / VERTEX_COUNT;
	for (UINT iTriangle = 0; iTriangle < TRIANGLE_COUNT; iTriangle++)
	{
		FLOAT fIndex0 = (*pIndex + 0.5f) * fIndexScale;
		CUSTOMVERTEX *pV0 = &rVertex[*pIndex++];
		FLOAT fIndex1 = (*pIndex + 0.5f) * fIndexScale;
		CUSTOMVERTEX *pV1 = &rVertex[*pIndex++];
		FLOAT fIndex2 = (*pIndex + 0.5f) * fIndexScale;
		CUSTOMVERTEX *pV2 = &rVertex[*pIndex++];
		D3DCOLOR IndexColor = D3DXCOLOR(fIndex0, fIndex1, fIndex2, 1.f);
		pIndexVertex->xyzrhw = D3DXVECTOR4(pV0->tu * fXScale + fXOffset, pV0->tv * fYScale + fYOffset, 0.5f, 1.f);
		pIndexVertex->diffuse = IndexColor;
		pIndexVertex++;
		pIndexVertex->xyzrhw = D3DXVECTOR4(pV1->tu * fXScale + fXOffset, pV1->tv * fYScale + fYOffset, 0.5f, 1.f);
		pIndexVertex->diffuse = IndexColor;
		pIndexVertex++;
		pIndexVertex->xyzrhw = D3DXVECTOR4(pV2->tu * fXScale + fXOffset, pV2->tv * fYScale + fYOffset, 0.5f, 1.f);
		pIndexVertex->diffuse = IndexColor;
		pIndexVertex++;
	}
	m_pIndexVB->Unlock();
	
	// Unlock the carrier mesh vertex and index buffers
	m_pCarrierIB->Unlock();
    m_pCarrierVB->Unlock();

	// Create the weight texture
	D3DFORMAT Format = D3DFMT_A8R8G8B8;	// must use swizzled format texture, otherwise memory may have a stride
	hr = m_pd3dDevice->CreateTexture(WIDTH, HEIGHT, 1, D3DUSAGE_RENDERTARGET, Format, 0, &m_pWeightTexture);
	if (FAILED(hr))
		return hr;
	hr = m_pWeightTexture->GetSurfaceLevel(0, &m_pWeightSurface);
	if (FAILED(hr))
		return hr;
	ZeroMemory( &m_WeightVertexBuffer, sizeof(D3DVertexBuffer) );
	XGSetVertexBufferHeader( WIDTH * HEIGHT * 4, 0, D3DFVF_DIFFUSE, 0, &m_WeightVertexBuffer, m_pWeightSurface->Data);
		
	// Create the index texture
	hr = m_pd3dDevice->CreateTexture(WIDTH, HEIGHT, 1, D3DUSAGE_RENDERTARGET, Format, 0, &m_pIndexTexture);
	if (FAILED(hr))
		return hr;
	hr = m_pIndexTexture->GetSurfaceLevel(0, &m_pIndexSurface);
	if (FAILED(hr))
		return hr;
	ZeroMemory( &m_IndexVertexBuffer, sizeof(D3DVertexBuffer) );
	XGSetVertexBufferHeader( WIDTH * HEIGHT * 4, 0, D3DFVF_DIFFUSE, 0, &m_IndexVertexBuffer, m_pIndexSurface->Data);

	// Create the displacement texture
	hr = m_pd3dDevice->CreateTexture(WIDTH, HEIGHT, 1, D3DUSAGE_RENDERTARGET, Format, 0, &m_pDisplacementTexture);
	if (FAILED(hr))
		return hr;
	hr = m_pDisplacementTexture->GetSurfaceLevel(0, &m_pDisplacementSurface);
	if (FAILED(hr))
		return hr;
	ZeroMemory( &m_DisplacementVertexBuffer, sizeof(D3DVertexBuffer) );
	XGSetVertexBufferHeader( WIDTH * HEIGHT * 4, 0, D3DFVF_DIFFUSE, 0, &m_DisplacementVertexBuffer, m_pDisplacementSurface->Data);

	// Create and fill the displacement index buffer for drawing quads.
	// Since the textures above are swizzled, we have to account for
	// this in the quad-drawing index buffer.  We should have good memory
	// locality by following the swizzled order.
#define QUAD_COUNT ((WIDTH-1)*(HEIGHT-1))
	if( FAILED( m_pd3dDevice->CreateIndexBuffer( 4*QUAD_COUNT*sizeof(WORD),
												 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pDisplacementIndexBuffer ) ) )
		return E_FAIL;
	Swizzler s(WIDTH, HEIGHT, 0);
	WORD *pQuadIndices;
	if( FAILED( m_pDisplacementIndexBuffer->Lock( 0, 0, (BYTE**)&pQuadIndices, 0 ) ) )
		return E_FAIL;
	// walk through vertices in swizzled order
	UINT iiQuad = 0;
	for (SWIZNUM iVertex = 0; iVertex < WIDTH * HEIGHT; iVertex++)
	{
		DWORD U = s.UnswizzleU(iVertex);
		if (U == WIDTH - 1)
			continue; // skip U boundary
		DWORD V = s.UnswizzleV(iVertex);
		if (V == HEIGHT - 1)
			continue; // skip V boundary
		pQuadIndices[iiQuad++] = (WORD)iVertex; // s.Swizzle(U, V, 0);
		pQuadIndices[iiQuad++] = (WORD)s.Swizzle(U + 1, V, 0);
		pQuadIndices[iiQuad++] = (WORD)s.Swizzle(U + 1, V + 1, 0);
		pQuadIndices[iiQuad++] = (WORD)s.Swizzle(    U, V + 1, 0);
	}
	m_pDisplacementIndexBuffer->Unlock();

	// Load the displacement vertex shader
	DWORD vsdecl[] = 
	{
		D3DVSD_STREAM(0),
		D3DVSD_REG(0, D3DVSDT_D3DCOLOR),    // indices
		D3DVSD_STREAM(1),
		D3DVSD_REG(1, D3DVSDT_D3DCOLOR),    // weights
		D3DVSD_STREAM(2),
		D3DVSD_REG(2, D3DVSDT_D3DCOLOR),    // displacement
		D3DVSD_END()
	};
	XBUtil_CreateVertexShader(g_pd3dDevice, "Shaders\\displacement.xvu", vsdecl, &m_dwDisplacementVertexShader );
	if (m_dwDisplacementVertexShader == 0)
		return E_FAIL;
				
    // Setup a base material
    D3DMATERIAL8 mtrl;
    XBUtil_InitMaterial( mtrl, 1.0f, 1.0f, 1.0f, 1.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

	// Perform object rotation
	D3DXMATRIX matWorld( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// Set up view matrix
	D3DXMATRIX	matView;
	D3DXVECTOR3 vEyePt	  = D3DXVECTOR3( 0.0f, 0.0f,-4.0f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec	  = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	// Set up proj matrix
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 4.0f/3.0f, 1.0f, 200.0f );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

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

    // Cycle through draw modes
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
    {
		m_DrawMode = (DrawMode)((INT)m_DrawMode + 1);
		if (m_DrawMode == _DRAW_MAX)
			m_DrawMode = (DrawMode)0;
    }
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
    {
		if (m_DrawMode == (DrawMode)0)
			m_DrawMode = (DrawMode)((INT)_DRAW_MAX - 1);
		else
			m_DrawMode = (DrawMode)((INT)m_DrawMode - 1);
    }

	// Toggle point drawing
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE] )
    {
		m_bDrawPoints = !m_bDrawPoints;
    }

	// Cycle through displacement maps
	if ( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
	{
		if (m_iCurrentDisplacementMap == 3)
			m_iCurrentDisplacementMap = 0;
		else
			m_iCurrentDisplacementMap++;
	}
	if ( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
	{
		if (m_iCurrentDisplacementMap == 0)
			m_iCurrentDisplacementMap = 3;
		else
			m_iCurrentDisplacementMap--;
	}

	// Cycle through tweak modes
	if ( m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP 
		 || m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN 
		 || m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT 
		 || m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
		m_TweakActive = 2.f; // keep tweak display showing while DPAD is pressed
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
    {
		m_TweakMode = (TweakMode)((INT)m_TweakMode + 1);
		if (m_TweakMode == _TWEAK_MAX)
			m_TweakMode = (TweakMode)0;
    }
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
    {
		if (m_TweakMode == (TweakMode)0)
			m_TweakMode = (TweakMode)((INT)_TWEAK_MAX - 1);
		else
			m_TweakMode = (TweakMode)((INT)m_TweakMode - 1);
    }


	// Handling parameter tweaking
	int delta = m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] 
		- m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER];
	if (delta)
	{
		if (m_TweakMode == TWEAK_DISPLACEMENT_SCALE)
		{
			static float fDeltaScale = 0.001f;
			m_rfDisplacementScale[m_iCurrentDisplacementMap] += m_fElapsedTime * delta * fDeltaScale;
			m_TweakActive = 1.f;
		}
		else if (m_TweakMode == TWEAK_DISPLACEMENT_OFFSET)
		{
			static float fDeltaScale = 0.001f;
			m_rfDisplacementOffset[m_iCurrentDisplacementMap] += m_fElapsedTime * delta * fDeltaScale;
			m_TweakActive = 1.f;
		}
		else if (m_TweakMode == TWEAK_MIPMAP_BIAS)
		{
			static float fDeltaScale = 0.01f;
			m_rfMipmapBias[m_iCurrentDisplacementMap] += m_fElapsedTime * delta * fDeltaScale;
			m_TweakActive = 1.f;
		}
	}
	else if (m_TweakActive > 0.f)	// keep the tweaking debug display active for a while
		m_TweakActive -= m_fElapsedTime;

	// Perform object rotation
	static D3DXMATRIX matWorld( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
	D3DXMATRIX matRotate;
	FLOAT fXRotate = m_DefaultGamepad.fX1 * m_fElapsedTime * D3DX_PI * 0.5f;
	FLOAT fYRotate = m_DefaultGamepad.fY1 * m_fElapsedTime * D3DX_PI * 0.5f;
	D3DXMatrixRotationYawPitchRoll( &matRotate, -fXRotate, fYRotate, 0.0f );
	D3DXMatrixMultiply( &matWorld, &matWorld, &matRotate );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: RenderToDisplacementTextures
// Desc: Draw the UV triangles into the weight and index textures
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::RenderToDisplacementTextures()
{
    // Set render state
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,	  FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,   FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,     FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,		  D3DCULL_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,         FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,      TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTexture( 0, 0 );

	// To the weight texture, render the mesh UV triangles with
	// interpolated RGB weights.
	m_pd3dDevice->SetRenderTarget(m_pWeightSurface, NULL);
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0xffff00ff, 1.0f, 0L );
    m_pd3dDevice->SetStreamSource( 0, m_pWeightVB, sizeof(WEIGHTVERTEX) );
	m_pd3dDevice->SetVertexShader( FVF_WEIGHTVERTEX );
	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, TRIANGLE_COUNT);
	
	// To the index texture, render the mesh UV triangles with vertex
	// index colors constant across the triangle.
	m_pd3dDevice->SetRenderTarget(m_pIndexSurface, NULL);
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0xff00ff00, 1.0f, 0L );
    m_pd3dDevice->SetStreamSource( 0, m_pIndexVB, sizeof(INDEXVERTEX) );
	m_pd3dDevice->SetVertexShader( FVF_INDEXVERTEX );
	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, TRIANGLE_COUNT);

	// To the displacement texture, render current displacement region.
	m_pd3dDevice->SetRenderTarget(m_pDisplacementSurface, NULL);
	FLOAT WIDTHOFFSET = 0.5f / WIDTH;
	FLOAT HEIGHTOFFSET = 0.5f / HEIGHT;
	struct Vertex {
		FLOAT xyzrhw[4];
		FLOAT tex1[2];
		FLOAT tex2[2];
		FLOAT tex3[2];
		FLOAT tex4[2];
	} rVertex[4] = {
		// The texture scaling should also be tweakable to change the
		// character of how the displacements scales interact.  Here
		// we hard-code powers of two.
		{       - WIDTHOFFSET,        - HEIGHTOFFSET, 0.5f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f },
		{ WIDTH - WIDTHOFFSET,        - HEIGHTOFFSET, 0.5f, 1.f, 1.f, 0.f, 2.f, 0.f, 4.f, 0.f, 8.f, 0.f },
		{ WIDTH - WIDTHOFFSET, HEIGHT - HEIGHTOFFSET, 0.5f, 1.f, 1.f, 1.f, 2.f, 2.f, 4.f, 4.f, 8.f, 8.f },
		{       - WIDTHOFFSET, HEIGHT - HEIGHTOFFSET, 0.5f, 1.f, 0.f, 1.f, 0.f, 2.f, 0.f, 4.f, 0.f, 8.f },
	};
	m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX4 );
	{
#pragma warning( push )
#pragma warning( disable : 4245 )	// ignore conversion of int to DWORD
		
		// Use pre-compiled channel pixel shader that maps texture 0
		// to red, 1 to green, 2 to blue, and 3 to alpha.
#include "Media/Shaders/channel.lst"
		m_pd3dDevice->SetPixelShaderProgram( &psd );
		
#pragma warning( pop )
	}
	UINT i;
	for (i = 0; i < 4; i++)
	{
		m_pd3dDevice->SetTexture( i, m_rpDisplacementMap[i] );
		m_pd3dDevice->SetTextureStageState( i, D3DTSS_TEXCOORDINDEX, i );
		m_pd3dDevice->SetTextureStageState( i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		m_pd3dDevice->SetTextureStageState( i, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		m_pd3dDevice->SetTextureStageState( i, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
		m_pd3dDevice->SetTextureStageState( i, D3DTSS_MIPMAPLODBIAS, FtoDW(m_rfMipmapBias[i]));
		m_pd3dDevice->SetTextureStageState( i, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
		m_pd3dDevice->SetTextureStageState( i, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
	}
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_QUADLIST, 1, rVertex, sizeof(Vertex));

	// Reset state
	m_pd3dDevice->SetPixelShader(0);
	for (i = 0; i < 4; i++)
	{
		m_pd3dDevice->SetTexture( i, NULL );
	}
	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: DrawDisplacedMesh
// Desc: Send the displacement textures through the vertex shader
//       to draw the displaced mesh.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawDisplacedMesh()
{
	// Larger mesh objects need to be broken into contiguous vertex
	// neighborhoods that will fit into the vertex shader constants
	// for this technique to work.
	//
	// Here, we have a relative small number of vertices in the
	// carrier mesh, so we need just one pass.
	m_pd3dDevice->SetShaderConstantMode(D3DSCM_192CONSTANTS);
	
	m_pd3dDevice->SetVertexShader( m_dwDisplacementVertexShader );
	
	// Set the vertex shader transformation matrix
	D3DXMATRIX matWorld;
	m_pd3dDevice->GetTransform( D3DTS_WORLD, &matWorld );
	D3DXMATRIX matView;
	m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView);
	D3DXMATRIX matProjection;
	m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProjection);
	D3DXMATRIX matComposite;
	D3DXMatrixMultiply( &matComposite, &matWorld, &matView );
	D3DXMatrixMultiply( &matComposite, &matComposite, &matProjection );
	D3DXMatrixTranspose( &matComposite, &matComposite );
	m_pd3dDevice->SetVertexShaderConstant( VSC_WORLDVIEWPROJECTION, &matComposite, 4 );

	// Set index scaling
	D3DXVECTOR4 Index( VERTEX_COUNT, 0, 0, 0 );
	m_pd3dDevice->SetVertexShaderConstant( VSC_INDEX, &Index, 1 );

	// Set displacement scaling and offset
	for (UINT i = 0; i < 4; i++)
	{
		D3DXVECTOR4 Displacement( m_rfDisplacementScale[i], m_rfDisplacementOffset[i], 0, 0 );
		m_pd3dDevice->SetVertexShaderConstant( VSC_DISPLACEMENT + i, &Displacement, 1 );
	}

	// Set other vertex shader constants
	D3DXVECTOR4 Ones(1.f, 1.f, 1.f, 1.f);
	m_pd3dDevice->SetVertexShaderConstant( VSC_ONES, &Ones, 1 );
	FLOAT fScale = (FLOAT)(1 << m_iCurrentDisplacementMap);
	D3DXVECTOR4 Scale(fScale, fScale, fScale, fScale);
	m_pd3dDevice->SetVertexShaderConstant( VSC_TEXTURESCALE, &Scale, 1 );

	// Set vertex palette
	// TODO: create a PushBuffer resource with all of this data once to
	// avoid all this copying.
    CUSTOMVERTEX *pVertex;
    if( FAILED( m_pCarrierVB->Lock( 0, 0, (BYTE**)&pVertex, 0 ) ) )
        return E_FAIL;
    for(INT i=0; i<VERTEX_COUNT; i++ )
    {
		CUSTOMVERTEX *pv = pVertex++;
		D3DXVECTOR4 c0(pv->position.x, pv->position.y, pv->position.z, pv->tu);
		m_pd3dDevice->SetVertexShaderConstant( VSC_CARRIER_VERTEX_PALETTE + i, &c0, 1);
		D3DXVECTOR4 c1(pv->normal.x, pv->normal.y, pv->normal.z, pv->tv);
		m_pd3dDevice->SetVertexShaderConstant( VSC_CARRIER_NORMAL_PALETTE + i, &c1, 1);
	}
    m_pCarrierVB->Unlock();
	
    // Set render state
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,	TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,   TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,       FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetTexture( 0, m_rpDisplacementMap[m_iCurrentDisplacementMap] );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPMAPLODBIAS, FtoDW(m_rfMipmapBias[m_iCurrentDisplacementMap]) );	
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
 
    // Draw the object
    m_pd3dDevice->SetStreamSource( 0, &m_IndexVertexBuffer, sizeof(D3DCOLOR) );
    m_pd3dDevice->SetStreamSource( 1, &m_WeightVertexBuffer, sizeof(D3DCOLOR) );
    m_pd3dDevice->SetStreamSource( 2, &m_DisplacementVertexBuffer, sizeof(D3DCOLOR) );
	
	if (m_bDrawPoints)
	{
		// Draw as points for showing the sampling grid
		static float fPointSize = 2.f;
		m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE, FtoDW(fPointSize) );
		m_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, 0, WIDTH * HEIGHT);
	}
	else
	{
		// Draw as quads
		m_pd3dDevice->SetIndices(m_pDisplacementIndexBuffer, 0);
		m_pd3dDevice->DrawIndexedPrimitive( D3DPT_QUADLIST, 0, 4*QUAD_COUNT, 0, QUAD_COUNT);
	}

	m_pd3dDevice->SetShaderConstantMode(D3DSCM_96CONSTANTS);
	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: DrawMesh
// Desc: Draws the carrier mesh.  This is used to demonstrate the visual 
//       difference and the performace difference between standard texture
//       mapping and displacement mapping.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawMesh()
{
    // Set render state
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,	TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,   TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,		0xffffffff );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,       TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPMAPLODBIAS, FtoDW(m_rfMipmapBias[m_iCurrentDisplacementMap]));	
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
    m_pd3dDevice->SetTexture( 0, m_rpDisplacementMap[m_iCurrentDisplacementMap] );
	D3DXMATRIX matTexture;
	FLOAT fScale = (FLOAT)(1 << m_iCurrentDisplacementMap);
	D3DXMatrixScaling(&matTexture, fScale, fScale, fScale);
	m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matTexture );

    // Draw the object
    m_pd3dDevice->SetStreamSource( 0, m_pCarrierVB, sizeof(CUSTOMVERTEX) );
    m_pd3dDevice->SetIndices( m_pCarrierIB, 0 );
	m_pd3dDevice->SetVertexShader( FVF_CUSTOMVERTEX );
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, INDEX_COUNT, 0, TRIANGLE_COUNT);

	// Restore state
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawWeights
// Desc: Draws the carrier mesh colored with the weight assignments. This 
//       is used to demonstrate the per-displaced-vertex weight computed
//       on the fly using the GPU.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawWeights()
{
    // Set render state
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,	TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,   TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,       FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTexture( 0, m_pWeightTexture );

    // Draw the object
    m_pd3dDevice->SetStreamSource( 0, m_pCarrierVB, sizeof(CUSTOMVERTEX) );
    m_pd3dDevice->SetIndices( m_pCarrierIB, 0 );
	m_pd3dDevice->SetVertexShader( FVF_CUSTOMVERTEX );
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, INDEX_COUNT, 0, TRIANGLE_COUNT);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawIndices
// Desc: Draws the carrier mesh textures with the index assignments. This 
//       is used to demonstrate the computed per-displaced-vertex indices.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawIndices()
{
    // Set render state
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,	TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,   TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,		D3DCULL_NONE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,       FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetTexture( 0, m_pIndexTexture );

    // Draw the object
    m_pd3dDevice->SetStreamSource( 0, m_pCarrierVB, sizeof(CUSTOMVERTEX) );
    m_pd3dDevice->SetIndices( m_pCarrierIB, 0 );
	m_pd3dDevice->SetVertexShader( FVF_CUSTOMVERTEX );
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, INDEX_COUNT, 0, TRIANGLE_COUNT);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
	// Draw into the current displacement map textures
	RenderToDisplacementTextures();

	// Set the render target to the backbuffer
	m_pd3dDevice->SetRenderTarget(m_pBackBuffer, m_pDepthBuffer);
	
	// Clear the zbuffer
	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0L );

	// Draw a gradient filled background
	RenderGradientBackground( 0xff404040, 0xff404080 );

	// Draw the object
	if (m_DrawMode == DRAW_MESH)
		DrawMesh();
	else if (m_DrawMode == DRAW_WEIGHTS)
		DrawWeights();
	else if (m_DrawMode == DRAW_INDICES)
		DrawIndices();
	else if (m_DrawMode == DRAW_DISPLACED_MESH)
		DrawDisplacedMesh();
	
	// Show title, frame rate, and help
	if( m_bDrawHelp )
		m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
	else
	{
		m_Font.Begin();
		if (m_DrawMode == DRAW_MESH)
			m_Font.DrawText(  64, 50, 0xffffffff, L"Texture Map" );
		else if (m_DrawMode == DRAW_WEIGHTS)
			m_Font.DrawText(  64, 50, 0xffffffff, L"Weight Map" );
		else if (m_DrawMode == DRAW_INDICES)
			m_Font.DrawText(  64, 50, 0xffffffff, L"Index Map" );
		else if (m_DrawMode == DRAW_DISPLACED_MESH)
			m_Font.DrawText(  64, 50, 0xffffffff, L"Displacement Map" );
		m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
		if (m_TweakActive > 0.f)
		{
			WCHAR buf[200];
			FLOAT x = 64, y = 50, dy = 30;
			swprintf(buf, L"scale[%d]=%f", m_iCurrentDisplacementMap, m_rfDisplacementScale[m_iCurrentDisplacementMap]);
			m_Font.DrawText(x, y += dy, (m_TweakMode == TWEAK_DISPLACEMENT_SCALE) ? 0xffffffff : 0xff00ff00 , buf);
			swprintf(buf, L"offset[%d]=%f", m_iCurrentDisplacementMap, m_rfDisplacementOffset[m_iCurrentDisplacementMap]);
			m_Font.DrawText(x, y += dy, (m_TweakMode == TWEAK_DISPLACEMENT_OFFSET) ? 0xffffffff : 0xff00ff00 , buf);
			swprintf(buf, L"mipmap bias[%d]=%f", m_iCurrentDisplacementMap, m_rfMipmapBias[m_iCurrentDisplacementMap]);
			m_Font.DrawText(x, y += dy, (m_TweakMode == TWEAK_MIPMAP_BIAS) ? 0xffffffff : 0xff00ff00 , buf);
		}
		m_Font.End();
	}

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

