//-----------------------------------------------------------------------------
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//
//  Tree rendering with a hierarchy of slice texture level-of-detail 
// representations.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <XBMesh.h>
#include <XPath.h>
#include <XBResource.h>
#include <XBUtil.h>
#include <assert.h>
#include <xgraphics.h>
#include "Clip.h"
#include "Terrain.h"
#include "Tree.h"
#include "mipmap.h"

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
	{ XBHELP_LEFTSTICK,	   XBHELP_PLACEMENT_2, L"Move in-out\nand left-right" },
	{ XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Rotate and\ntilt view" },
	{ XBHELP_DPAD,		   XBHELP_PLACEMENT_2, L"Move up\nand down" },
	{ XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_1, L"Wire-frame" },
	{ XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_1, L"Slice debug" },
	{ XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Toggle help" },
	{ XBHELP_A_BUTTON,     XBHELP_PLACEMENT_1, L"Tree count +" },
	{ XBHELP_B_BUTTON,     XBHELP_PLACEMENT_1, L"Tree count -" },
	{ XBHELP_MISC_CALLOUT, XBHELP_PLACEMENT_2, L"Left trigger shows\nview-frustum culling" },
};
#define NUM_HELP_CALLOUTS (sizeof(g_HelpCallouts) / sizeof(XBHELP_CALLOUT))

//-----------------------------------------------------------------------------
// Global parameters and view toggles
//-----------------------------------------------------------------------------
D3DLIGHT8 g_d3dLight; 
D3DXVECTOR3 g_vLightDirection = D3DXVECTOR3(0.1f, 0.95f, 0.2f);
D3DXVECTOR3 g_vMin(-1000.f, 0.f, -1000.f), g_vMax(1000.f, 20.f, 1000.f);	// scale of height field in world coords
D3DXVECTOR2 g_vTerrainTextureScale(150.f, 150.f), g_vTerrainTextureOffset(0.f, 0.f);

bool g_bWireFrame = false;
bool g_bDrawHelp = false;

bool g_bCompressTextures = false;	// compress slice textures
bool g_bDebugSlice = false;			// draw slices with color coding and print out stats
bool g_bDebugSliceOpaque = false;	// draw slices without using slice texture

// tree and slice stats
struct TREESTATS {
	DWORD	dwActiveCount;
	DWORD	dwFullGeometryCount;
	DWORD	dwBranchSliceCount;
	DWORD	dwSliceCount;
} g_TREESTATS;
DWORD	g_dwTotalSliceCount = 0;

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//		 from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
	CXBFont			m_Font;					// Font for rendering stats and help
	CXBHelp			m_Help;					// Help class
    CXBPackedResource m_xprResources;		// Packed texture resources
    LPDIRECT3DTEXTURE8 m_pBlendTexture;		// Render target for level-of-detail transitions
    LPDIRECT3DSURFACE8 m_pBlendDepthBuffer; // Depth buffer for level-of-detail transitions
	float			m_fBlendFactor;			// Blend factor that smoothly changes from 0 to 1
	D3DXVECTOR3		m_vFrom, m_vAt, m_vUp;	// Viewing parameters
	D3DXMATRIX		m_matWorld;
	D3DXMATRIX		m_matView;
	D3DXMATRIX		m_matViewInverse;
	D3DXMATRIX		m_matProjection;
	Terrain		   *m_pTerrain; // current terrain representation
	LPDIRECT3DTEXTURE8 m_pTextureTerrainShadow;	// shadows cast on terrain by trees
#define NTREELIBRARY 1
	CTree			m_rTreeLibrary[NTREELIBRARY];
#define NTREEX 50
#define NTREEZ 50
#define NMAXTREEINSTANCE (NTREEX * NTREEZ)
    float			m_fLevelOfDetail;			// scale factor for level of detail calculation
	D3DXVECTOR3		m_rvFromLevelOfDetail[2];	// position of previous level-of-detail update
	D3DXVECTOR3		m_rvAtLevelOfDetail[2];		// view position of previous level-of-detail update
    int				m_riNumTrees[2];			// current and previous number of trees
	float			m_fNumTrees;				// for incrementing/decrementing number of trees
    struct TreeSort {
        float fDist2;			// squared distance from eye. This field must be first for qsort to work.
		UINT iTree;				// index into m_rTree array
    } *m_rrTreeSort[2];			// model instances, sorted by distance from eye
	struct TreeData {
        D3DXVECTOR3 vPosition;	// position of instance
		float fYRot;			// Y rotation of instance
		D3DXMATRIX mat;			// tree to world coords
		D3DXMATRIX matInv;		// world to tree coords
		struct LOD {
			float fLevelOfDetail;   // level-of-detail values for this object
			D3DXVECTOR3 vFrom;		// local eye position determines sorting order for branches, etc.
			DWORD dwDrawFlags;		// set to TREE_DRAWTRUNK | TREE_DRAWBRANCHES depending on level of detail
			float fTreeSliceTextureLOD;
			float fBranchSliceTextureLOD;
		} rLOD[2];
		bool bSameLevelOfDetail; // the level-of-detail representation is the same for both rendering passes
		bool bVisible;			// is tree visible?
		D3DXVECTOR3 vFromFade;	// current vFrom value in local tree coords
		UINT iTreeLibrary;		// index into m_rTreeLibrary
	} *m_rTree;
public:
	CXBoxSample();
    HRESULT UpdateTrees();	// update sorting and level-of-detail parameters
    HRESULT CullTrees();	// cull trees based on current viewing matrices
	HRESULT ShadowTrees();	// render current set of trees onto terrain texture
	HRESULT BlendScreenTexture(LPDIRECT3DTEXTURE8 pTexture, D3DCOLOR colorBlend); // blend texture with backbuffer
	
	// CXBApplication overrides
	HRESULT Initialize();
	HRESULT FrameMove();
	HRESULT Render();
	HRESULT Cleanup();
};

//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program. Initializes everything, and goes into a
//		 message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
void __cdecl main()
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
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	// Allow unlimited frame rate
#else
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_TWO;	// 30Hz
#endif
	m_pTerrain = NULL;
	m_pTextureTerrainShadow = NULL;
    m_fLevelOfDetail = 1.f; // 0.045f;
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
	HRESULT hr;
	m_bPaused = true; // false;

	XPath_SetBasePath(_T("d:\\media\\"));

	// Create a font
	if(FAILED(hr = m_Font.Create(m_pd3dDevice, "Font.xpr")))
		return XBAPPERR_MEDIANOTFOUND;

	// Create the help
	if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
		return XBAPPERR_MEDIANOTFOUND;

    // Load resources from the packed resource file. Resources (textures, vertex
    // buffers,etc.) can be packed this way so they load super quick and require
    // no load-time processing (like parsing a .bmp file, changing formats,
    // swizzling, etc.). See the code in XBResource.cpp to see how simple it is.
	static XBRESOURCE rResource[resource_NUM_RESOURCES] = {
		{ "TerrainTexture",	 resource_TerrainTexture_OFFSET },
		{ "undergrass", 	resource_undergrass_OFFSET },
		{ "leaf2",		resource_leaf2_OFFSET },
	};
    if( FAILED( m_xprResources.Create( m_pd3dDevice, "Resource.xpr", resource_NUM_RESOURCES, rResource ) ) )
        return E_FAIL;

    // create the texture used for blending level-of-detail transitions
	D3DSURFACE_DESC descBackBuffer;
	m_pBackBuffer->GetDesc(&descBackBuffer);
    hr = m_pd3dDevice->CreateTexture(descBackBuffer.Width, descBackBuffer.Height, 1, 0, D3DFMT_LIN_A8R8G8B8, D3DPOOL_DEFAULT, &m_pBlendTexture);
	if (FAILED(hr))
		return hr;
	hr = m_pd3dDevice->CreateDepthStencilSurface(descBackBuffer.Width, descBackBuffer.Height, D3DFMT_LIN_D24S8, D3DMULTISAMPLE_NONE, &m_pBlendDepthBuffer);
	if (FAILED(hr))
		return hr;

	// set light parameters
	ZeroMemory(&g_d3dLight, sizeof(D3DLIGHT8));
	g_d3dLight.Type = D3DLIGHT_DIRECTIONAL;
	g_d3dLight.Position = D3DXVECTOR3(10000.f, 10000.f, 10000.f); // sun
	g_d3dLight.Direction = g_vLightDirection;
	g_d3dLight.Ambient  = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f); // D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	g_d3dLight.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	g_d3dLight.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	g_pd3dDevice->SetLight(0, &g_d3dLight);
	g_pd3dDevice->LightEnable(0, TRUE);

	// load the terrain image and create the height field mesh
	m_pTerrain = new Terrain;
	if (m_pTerrain == NULL)
		return E_OUTOFMEMORY;
	LPDIRECT3DTEXTURE8 pTextureTerrain = m_xprResources.GetTexture( (DWORD)resource_TerrainTexture_OFFSET );
	LPDIRECT3DTEXTURE8 pTextureUndergrass = m_xprResources.GetTexture( (DWORD)resource_undergrass_OFFSET );
	hr = m_pd3dDevice->CreateTexture(512, 512, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTextureTerrainShadow);
	if (FAILED(hr))
		return hr;
	UINT widthTerrain = 40, heightTerrain = 40;	// number of samples in terrain mesh
	hr = m_pTerrain->Initialize(pTextureTerrain, pTextureUndergrass, m_pTextureTerrainShadow,
								g_vMin, g_vMax, g_vTerrainTextureScale, g_vTerrainTextureOffset,
								widthTerrain, heightTerrain);
	if (FAILED(hr))
		return hr;

	// create the tree library
#define BOUNDSET_XMIN 001
#define BOUNDSET_YMIN 002
#define BOUNDSET_ZMIN 004
#define BOUNDSET_XMAX 010
#define BOUNDSET_YMAX 020
#define BOUNDSET_ZMAX 040
#define BOUNDSET_XZ (BOUNDSET_XMIN|BOUNDSET_XMAX|BOUNDSET_ZMIN|BOUNDSET_ZMAX)
	static struct TreeLibraryData {
		CHAR *strName;
		DWORD dwFlags;
		struct Vector { float x, y, z; } vScale, vMin, vMax;
	} rTreeLibraryData[NTREELIBRARY] = 	{
		{ "tree1",	0,        { 1.f, 1.f, 1.f} },	// use tree's bbox
	};
	for (UINT iTreeLibrary = 0; iTreeLibrary < NTREELIBRARY; iTreeLibrary++)
	{
		CTree *pTreeLibrary = &m_rTreeLibrary[iTreeLibrary];
		TreeLibraryData *pTreeLibraryData = &rTreeLibraryData[iTreeLibrary];
		if ( FAILED( pTreeLibrary->Create( pTreeLibraryData->strName, &m_xprResources ) ) )
			return XBAPPERR_MEDIANOTFOUND;
		// pTreeLibrary->Scale(*(D3DXVECTOR3 *)&pTreeLibraryData->vScale); // scale top-level frames and then get new bounding box
		// Set bounds to be different than actual geometry bounding box
		if (pTreeLibraryData->dwFlags & BOUNDSET_XMIN) pTreeLibrary->m_vMin.x = pTreeLibraryData->vMin.x;
		if (pTreeLibraryData->dwFlags & BOUNDSET_YMIN) pTreeLibrary->m_vMin.y = pTreeLibraryData->vMin.y;
		if (pTreeLibraryData->dwFlags & BOUNDSET_ZMIN) pTreeLibrary->m_vMin.z = pTreeLibraryData->vMin.z;
		if (pTreeLibraryData->dwFlags & BOUNDSET_XMAX) pTreeLibrary->m_vMax.x = pTreeLibraryData->vMax.x;
		if (pTreeLibraryData->dwFlags & BOUNDSET_YMAX) pTreeLibrary->m_vMax.y = pTreeLibraryData->vMax.y;
		if (pTreeLibraryData->dwFlags & BOUNDSET_ZMAX) pTreeLibrary->m_vMax.z = pTreeLibraryData->vMax.z;
		pTreeLibrary->Slice();	// turn geometry into texture by taking slices
	}

	// position the tree instances
	srand(123456);
#define irand(a) ((rand()*(a))>>15)
#define frand(a) ((float)rand()*(a)/32768.0f)
	m_rrTreeSort[0] = new TreeSort [ NMAXTREEINSTANCE ];
	m_rrTreeSort[1] = new TreeSort [ NMAXTREEINSTANCE ];
	m_rTree = new TreeData [ NMAXTREEINSTANCE ];
	D3DXVECTOR3 vTerrainCell = g_vMax - g_vMin;
	vTerrainCell.x /= NTREEX;
	vTerrainCell.z /= NTREEZ;
	for (int iTreeZ = 0; iTreeZ < NTREEZ; iTreeZ++)
	for (int iTreeX = 0; iTreeX < NTREEX; iTreeX++)
	{
		int iTree = iTreeZ * NTREEX + iTreeX;
		TreeData *pTree = &m_rTree[iTree];
		pTree->iTreeLibrary = irand(NTREELIBRARY);
		CTree *pTreeLibrary = &m_rTreeLibrary[pTree->iTreeLibrary];
		static float fCenter = 0.75f;	// radius from center of cell for semi-random tree placement
		pTree->vPosition = g_vMin +
			D3DXVECTOR3((iTreeX + 0.5f  + (frand(1.f) - 0.5f) * fCenter) * vTerrainCell.x,
						0.f,
						(iTreeZ + 0.5f  + (frand(1.f) - 0.5f) * fCenter) * vTerrainCell.z);
		float fTerrainHeight;
		D3DXVECTOR3 vTerrainNormal;
		m_pTerrain->GetTerrainPoint(pTree->vPosition, &fTerrainHeight, &vTerrainNormal);
		pTree->vPosition.y = fTerrainHeight - pTreeLibrary->m_vMin.y;
		pTree->fYRot = frand(2.f * D3DX_PI);			// Y rotation of instance
		D3DXMATRIX matRotY;
		D3DXMatrixRotationY(&matRotY, pTree->fYRot);
		D3DXMATRIX matTrans;
		D3DXMatrixTranslation(&matTrans, pTree->vPosition.x, pTree->vPosition.y, pTree->vPosition.z);
		pTree->mat = matRotY * matTrans;
		D3DXMatrixInverse(&pTree->matInv, NULL, &pTree->mat);
		pTree->vPosition.y += 0.5f * (pTreeLibrary->m_vMax.y + pTreeLibrary->m_vMin.y);	// offset level-of-detail center to center of tree
	}
	// randomize tree ordering so that a subset will appear in random places
	for (int iTree = 0; iTree < NMAXTREEINSTANCE; iTree++)
	{
		TreeData t = m_rTree[iTree];
		int jTree = irand(NMAXTREEINSTANCE);
		m_rTree[iTree] = m_rTree[jTree];
		m_rTree[jTree] = t;
	}
	m_fNumTrees = NMAXTREEINSTANCE; // default number of trees to start
	if (m_fNumTrees > NMAXTREEINSTANCE)
		m_fNumTrees = NMAXTREEINSTANCE;
	m_riNumTrees[0] = m_riNumTrees[1] = 0;	// set when UpdateTrees is called

	// Set camera parameters
	m_vFrom = D3DXVECTOR3( 27.1533f, 30.f, -6.41251f);
	m_vAt = m_vFrom + D3DXVECTOR3(-0.97f, -0.1f, 0.2f);
	m_vUp = D3DXVECTOR3( 0.0f, 1.0f , 0.0f);
	D3DXMatrixPerspectiveFovLH( &m_matProjection, D3DX_PI/4, 640.f/480.f, 0.4f, 4000.0f );
	D3DXMatrixLookAtLH( &m_matView, &m_vFrom, &m_vAt, &m_vUp);
	D3DXMatrixInverse(&m_matViewInverse, NULL, &m_matView);
	D3DXMatrixIdentity(&m_matWorld);

	// Set up level-of-detail processing
	m_fBlendFactor = 0.f;
	UpdateTrees();
	UpdateTrees();	// call twice to initialize double-buffered level-of-detail values
	ShadowTrees();	// render current set of tree shadows onto terrain texture
	return hr;
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Clean up after ourselves.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Cleanup()
{
	SAFE_DELETE(m_pTerrain);
	SAFE_RELEASE(m_pTextureTerrainShadow);
	SAFE_RELEASE(m_pBlendDepthBuffer);
	SAFE_RELEASE(m_pBlendTexture);
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//		 the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
		g_bDrawHelp = !g_bDrawHelp;

	if (m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
		g_bWireFrame = !g_bWireFrame;

	if (m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
	{
		if (g_bDebugSlice)
		{
			if (!g_bDebugSliceOpaque)
				g_bDebugSliceOpaque = true;
			else
			{
				g_bDebugSlice = false;
				g_bDebugSliceOpaque = false;
			}
		}
		else
			g_bDebugSlice = true;
	}
	
	// Scale translation by height above ground plane
	float fTranslateScale = fabsf(m_vFrom.y) + 0.01f;

	// update view position
	static float fOffsetScale = 0.5f;
	float fX1 = m_DefaultGamepad.fX1;
	fX1 *= fX1 * fX1; // fX1 cubed
	float fY1 = m_DefaultGamepad.fY1;
	fY1 *= fY1 * fY1; // fY1 cubed
	D3DXVECTOR3 vOffset(fX1, 0.f, fY1);	// screen space offset, X moves left-right, Y moves in-out in depth
	D3DXVec3TransformNormal(&vOffset, &vOffset, &m_matViewInverse);
	D3DXVec3Normalize(&m_vUp, &m_vUp);
	vOffset -= D3DXVec3Dot(&vOffset, &m_vUp) * m_vUp; // don't move up or down with thumb sticks
	D3DXVec3Normalize(&vOffset, &vOffset);
	vOffset *= fTranslateScale * fOffsetScale * m_fElapsedTime;
	m_vFrom += vOffset;
	m_vAt += vOffset;
	
	// move up and down with DPAD
	static float fVerticalScale = 1.f;
	D3DXVECTOR3 vVerticalOffset(0.f, 0.f, 0.f);
	if (m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
		vVerticalOffset.y += fTranslateScale * fVerticalScale * m_fElapsedTime;
	if(m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
		vVerticalOffset.y -= fTranslateScale * fVerticalScale * m_fElapsedTime;
	m_vFrom += vVerticalOffset;
	m_vAt += vVerticalOffset;

	// update view angle
	static float fAtOffsetScale = 4.f;
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
		|| (fEdotU > fThreshold && fY2 > 0.f)	// near vUp, but negative movement
		|| (fEdotU > -fThreshold && fEdotU < fThreshold))		// ordinary case
		vAtOffset.y -= fAtOffsetScale * fY2 * m_fElapsedTime;	// screen-space Y displacement means up-down view turn
	D3DXVec3TransformNormal(&vAtOffset, &vAtOffset, &m_matViewInverse);
	m_vAt += vAtOffset;

	// Check to make sure we're not beneath the ground plane

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).
	D3DXMatrixPerspectiveFovLH( &m_matProjection, D3DX_PI/4, 640.f/480.f, 0.4f, 4000.0f );

	// Set up our view matrix.
	D3DXMatrixLookAtLH( &m_matView, &m_vFrom, &m_vAt, &m_vUp);
	D3DXMatrixInverse(&m_matViewInverse, NULL, &m_matView);

	// Set world matrix to identity
	D3DXMatrixIdentity(&m_matWorld);

	// change number of trees
	BYTE buttonA = m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_A];
	BYTE buttonB = m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_B];
	static int iThreshold = 1;
	if (buttonA >= iThreshold || buttonB >= iThreshold)
	{
		static float fMinTreesPerSecond = 0.5f;
		static float fMaxTreesPerSecond = 100.f;
		float fx = (buttonA - iThreshold) / (255.f - iThreshold);
		fx = (fx <= 0.f) ? 0.f : fMinTreesPerSecond + fMaxTreesPerSecond * fx * fx;
		float fy = (buttonB - iThreshold) / (255.f - iThreshold);
		fy = (fy <= 0.f) ? 0.f : fMinTreesPerSecond + fMaxTreesPerSecond * fy * fy;
		m_fNumTrees += m_fElapsedTime * (fx - fy);
		if (m_fNumTrees < 1) 
			m_fNumTrees = 1;
		if (m_fNumTrees > NMAXTREEINSTANCE) 
			m_fNumTrees = NMAXTREEINSTANCE;
		static float fBlendTreeFactor = 5.f;
		m_fBlendFactor += fBlendTreeFactor * m_fElapsedTime;
	}

	// smoothly go to next level-of-detail representation
	D3DXVECTOR3 vFromDelta = m_vFrom - m_rvFromLevelOfDetail[0];
	float fFromDelta = D3DXVec3LengthSq(&vFromDelta);
	static float fFromDeltaThreshold = 1e-4f;
	static float fBlendTimeFactor = 1.f;
	static float fBlendTimeFactorSlow = 0.25f;

	static float fBlendHeight0 = 10.f;
	static float fBlendHeight1 = 200.f;
	float fBlendHeightScale = expf((m_vFrom.z - fBlendHeight0) * logf(3.f) / (fBlendHeight1 - fBlendHeight0));
	static float fBlendHeightScaleMax = 3.f;
	static float fBlendHeightScaleMin = 1.f;
	if (fBlendHeightScale > fBlendHeightScaleMax)
		fBlendHeightScale = fBlendHeightScaleMax;
	if (fBlendHeightScale < fBlendHeightScaleMin)
		fBlendHeightScale = fBlendHeightScaleMin;

	if (fFromDelta > fFromDeltaThreshold)
	{
		// change LOD's quickly when moving
		m_fBlendFactor += fBlendHeightScale * fBlendTimeFactor * m_fElapsedTime;
	}
	else
	{
		// change LOD slowly when stopped
		m_fBlendFactor += fBlendHeightScale * fBlendTimeFactorSlow * m_fElapsedTime;
	}
	if (m_fBlendFactor > 1.f)
	{
		m_fBlendFactor = 0.f;	// triggers level-of-detail update

		// update tree level of detail and sorting order
		UINT iNumTrees = m_riNumTrees[1];
		UpdateTrees();
		if (iNumTrees != m_riNumTrees[1])
			ShadowTrees();	// render current set of trees onto terrain texture
	}
	CullTrees();		// update tree visibility
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CompareDist2()
// Desc: Used by UpdateTrees as an ordering function to sort models from
//   near to far.
//-----------------------------------------------------------------------------
static int __cdecl CompareDist2(const void *arg1, const void *arg2 )
{
    float f1 = *(float *)arg1;
    float f2 = *(float *)arg2;
    if (f1 < f2) 
        return -1;
    else if (f1 > f2) 
        return 1;
    else
        return 0;
}

//-----------------------------------------------------------------------------
// Name: UpdateTrees()
// Desc: set level-of-detail and sort the instances by distance from the eye
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::UpdateTrees()
{
	memcpy(m_rrTreeSort[0], m_rrTreeSort[1], sizeof(TreeSort) * m_riNumTrees[1]);
	m_riNumTrees[0] = m_riNumTrees[1];
	m_riNumTrees[1] = (int)floorf(m_fNumTrees);
	m_rvFromLevelOfDetail[0] = m_rvFromLevelOfDetail[1];
	m_rvAtLevelOfDetail[0] = m_rvAtLevelOfDetail[1];
	m_rvFromLevelOfDetail[1] = m_vFrom;
	m_rvAtLevelOfDetail[1] = m_vAt;
	for (int iTree = 0; iTree < m_riNumTrees[1]; iTree++)
	{
		TreeData *pTree = &m_rTree[iTree];
		
		// save previous LOD factors
		pTree->rLOD[0] = pTree->rLOD[1];

		// compute distance squared
		D3DXVECTOR3 vEye = pTree->vPosition - m_vFrom;
		m_rrTreeSort[1][iTree].fDist2 = D3DXVec3LengthSq(&vEye);
		m_rrTreeSort[1][iTree].iTree = iTree;

		// compute level of detail based on squared distance
		pTree->rLOD[1].fLevelOfDetail = m_fLevelOfDetail * m_rrTreeSort[1][iTree].fDist2;
		D3DXVec3TransformCoord(&pTree->rLOD[1].vFrom, &m_vFrom, &pTree->matInv );

		// TODO: move all of this level-of-detail stuff down into CTree
		static float fTreeSliceLODMin = 6000.f;
		static float fTreeSliceLODScale = 1.f/100000.f;
		static float fBranchSliceLODMin = 1000.f;
		static float fBranchSliceLODScale = 1.f/30000.f;
		if (pTree->rLOD[1].fLevelOfDetail > fTreeSliceLODMin)
		{
			pTree->rLOD[1].dwDrawFlags = 0;						// draw the whole tree as slices
			pTree->rLOD[1].fTreeSliceTextureLOD = (pTree->rLOD[1].fLevelOfDetail - fTreeSliceLODMin) * fTreeSliceLODScale;
			pTree->bSameLevelOfDetail = (pTree->rLOD[0].dwDrawFlags == 0)
				&& ((DWORD)(pTree->rLOD[1].fTreeSliceTextureLOD) == (DWORD)(pTree->rLOD[1].fTreeSliceTextureLOD));
		}
		else if (pTree->rLOD[1].fLevelOfDetail > fBranchSliceLODMin)
		{
			pTree->rLOD[1].dwDrawFlags = TREE_DRAWTRUNK;	// draw the trunk as geometry and the branches as slice textures
			pTree->rLOD[1].fBranchSliceTextureLOD = (pTree->rLOD[1].fLevelOfDetail - fBranchSliceLODMin) * fBranchSliceLODScale;
			pTree->bSameLevelOfDetail = (pTree->rLOD[0].dwDrawFlags == TREE_DRAWTRUNK)
				&& ((DWORD)(pTree->rLOD[1].fBranchSliceTextureLOD) == (DWORD)(pTree->rLOD[1].fBranchSliceTextureLOD));
		}
		else
		{
			pTree->rLOD[1].dwDrawFlags = TREE_DRAWFULLGEOMETRY;		// if close enough, draw all the geometry of the tree
			pTree->bSameLevelOfDetail = (pTree->rLOD[0].dwDrawFlags == TREE_DRAWFULLGEOMETRY);
		}
	}
    qsort( (void *)m_rrTreeSort[1], m_riNumTrees[1], sizeof(TreeSort), &CompareDist2 );
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CullTrees()
// Desc: cull trees based on current transformation matrices
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CullTrees()
{
	// get Blinn-style clipping matrix for bounding box culling
	D3DXMATRIX matViewProj = m_matView * m_matProjection;
	D3DXMATRIX matViewProjClip;
	BlinnClipMatrix(&matViewProjClip, &matViewProj);

	g_TREESTATS.dwActiveCount = 0;	// keep track of number of visible trees
	
	// cull trees
#if NTREELIBRARY == 1
	CTree *pTreeLibrary = &m_rTreeLibrary[0];
#endif
	int iLOD = m_riNumTrees[0] > m_riNumTrees[1] ? 0 : 1;
	for (int iTree = m_riNumTrees[iLOD] - 1; iTree >= 0; iTree--)
	{
		TreeData *pTree = &m_rTree[m_rrTreeSort[iLOD][iTree].iTree];
#if NTREELIBRARY > 1			
		CTree *pTreeLibrary = &m_rTreeLibrary[pTree->iTreeLibrary];
#endif
		// Cull tree if bounding box is outside of current frustum
		D3DXMATRIX matWorldViewProjClip = pTree->mat * matViewProjClip;
		pTree->bVisible = BoundingBoxInFrustum(matWorldViewProjClip, pTreeLibrary->m_vMin, pTreeLibrary->m_vMax);

		if (pTree->bVisible)
			g_TREESTATS.dwActiveCount++;

		// Transform current from vector
		D3DXVec3TransformCoord(&pTree->vFromFade, &m_vFrom, &pTree->matInv );
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ShadowTrees()
// Desc: render trees onto ground plane
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::ShadowTrees()
{
	HRESULT	hr;
	LPDIRECT3DSURFACE8 pSurface = NULL;
	hr = m_pTextureTerrainShadow->GetSurfaceLevel( 0, &pSurface );
	if (FAILED(hr))
		return hr;
	hr = m_pd3dDevice->SetRenderTarget( pSurface, NULL );
	pSurface->Release();
	if (FAILED(hr))
		return hr;
	g_pd3dDevice->BeginScene();

	// start with all the transformations set to identity
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matIdentity );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matIdentity );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	// clear to white
    struct BACKGROUNDVERTEX { D3DXVECTOR3 p; };
    BACKGROUNDVERTEX v[4];
    v[0].p = D3DXVECTOR3(-1.f,  1.f, 0.5f);
    v[1].p = D3DXVECTOR3( 1.f,  1.f, 0.5f);
    v[2].p = D3DXVECTOR3(-1.f, -1.f, 0.5f);
    v[3].p = D3DXVECTOR3( 1.f, -1.f, 0.5f);
    m_pd3dDevice->SetTexture( 0, NULL );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xffffffff );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ); 
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZ );
 	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(BACKGROUNDVERTEX));	

	// draw shadows for each tree
	D3DXMATRIX matProj;
	D3DXMatrixIdentity(&matProj);
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	D3DXVECTOR3 vSize = g_vMax - g_vMin;
	D3DXMATRIX matSize(      1.f/vSize.x,               0.f,               0.f, 0.f,
		                             0.f,               0.f,       1.f/vSize.y, 0.f,
					                 0.f,       1.f/vSize.z,               0.f, 0.f,
					   -g_vMin.x/vSize.x, -g_vMin.z/vSize.z, -g_vMin.y/vSize.y, 1.f);	// map to 0,1 range, swap Y and Z
	D3DXMATRIX mat2(  2.f,  0.f, 0.f, 0.f,
		              0.f, -2.f, 0.f, 0.f,
					  0.f,  0.f, 1.f, 0.f,
					 -1.f,  1.f, 0.f, 1.f);	// map X and -Y from 0,1 to -1,1 range, leave Z alone
	D3DXMATRIX matView = matSize * mat2;
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	D3DXVECTOR3 vFrom(g_d3dLight.Direction.x, g_d3dLight.Direction.y, g_d3dLight.Direction.z);
	// This assumes we're mapping to a planar shadow receiver, and so works only 
	// when the terrain is not too hilly.
	D3DXMATRIX matShadow(1.f,  0.f,	 0.f, 0.f,
		                -vFrom.x/vFrom.y,  0.f, -vFrom.z/vFrom.y, 0.f,
						 0.f,  0.f,  1.f, 0.f,
						 0.f,  0.f,  0.f, 1.f);
	UINT iLOD = 1;
	DWORD dwCubeFadeFlags = 0;
	for (int iTree = m_riNumTrees[iLOD] - 1; iTree >= 0; iTree--)
	{
		TreeData *pTree = &m_rTree[m_rrTreeSort[iLOD][iTree].iTree];
		D3DXMATRIX mat = pTree->mat * matShadow;
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &mat);
		CTree *pTreeLibrary = &m_rTreeLibrary[pTree->iTreeLibrary];
		pTreeLibrary->BeginDrawCubeSlices();
		pTreeLibrary->SetCubeFade(vFrom, dwCubeFadeFlags);
		static float fShadowFactor = 0.25f;
		for (UINT iDir = 0; iDir < pTreeLibrary->m_nDirection; iDir++)
		{
			pTreeLibrary->m_rfFade[iDir] *= fShadowFactor;		// adjust the fade value to make the shadow less dark
			pTreeLibrary->m_rSliceTexture[iDir].SetLevelOfDetail(0.f);
		}
		pTreeLibrary->DrawCubeSlices(vFrom);
		pTreeLibrary->EndDrawCubeSlices();
	}
	g_pd3dDevice->EndScene();
	return GenerateMipmaps(m_pTextureTerrainShadow, 0, D3DTADDRESS_CLAMP, D3DTADDRESS_CLAMP);
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//		 rendering. This function sets up render states, clears the
//		 viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
	const float fZeroBlend = 0.5f/255.f;	// blend values below this quantize to zero
	
	// reset tree stats
	g_TREESTATS.dwFullGeometryCount = 0;
	g_TREESTATS.dwBranchSliceCount = 0;	
	g_TREESTATS.dwSliceCount = 0;	
	g_dwTotalSliceCount = 0;
	
	// To make smooth level-of-detail transitions, we draw the scene in
	// passes and then blend the results.  The schedule of blending is updated in
	// FrameMove and depends on whether the camera is moving or the rendering
	// load is getting too high.  The typical blending schedule is to make a
	// complete level-of-detail transition every second.  This lets all the
	// objects in the scene transition in a smooth way.
	
	// Each object in the scene has two LOD's associated with it, the current
	// and the LOD that is being transitioned to.  Once the new LOD is reached,
	// all the LOD targets are updated.
	
	// First pass: render sky, terrain, and non-LOD-changing non-transparent geometry,
	// then copy current result to temporary texture.
	// Second pass: Render previous LOD to backbuffer
	// Third pass: Render next LOD to temporary texture, then blend with backbuffer.
	for (UINT iPass = 0; iPass < 3; iPass++)
	{
		UINT iLOD = (iPass == 0) ? 0 : iPass - 1;
		if (iPass == 0 || iPass == 1)
			m_pd3dDevice->SetRenderTarget( m_pBackBuffer, m_pDepthBuffer );
		else
		{
			if (m_fBlendFactor < fZeroBlend)
				break;			// No need to render a whole image and then multiply by zero
			LPDIRECT3DSURFACE8 pBlendSurface = NULL;
			m_pBlendTexture->GetSurfaceLevel( 0, &pBlendSurface );
			m_pd3dDevice->SetRenderTarget( pBlendSurface, m_pBlendDepthBuffer );
			pBlendSurface->Release();
		}
		g_pd3dDevice->BeginScene();

		g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjection );
		g_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
		g_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld);

		// If culling debug is turned on, render "from-the-side" to show the current projection frustum.
		bool bDebugCulling = m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 0;
		if (bDebugCulling)
		{
			extern HRESULT DebugSetFrustum();	// replaces current projection matrix with offset projection
			DebugSetFrustum();		// replaces current projection matrix with offset projection
		}
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		if (iPass == 0)	// draw sky background
			RenderGradientBackground(D3DXCOLOR(0.3f, 0.3f, 0.4f, 1.f), D3DXCOLOR(0.45f, 0.45f, 0.9f, 1.f));
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

		if (g_bWireFrame)
			g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		else
			g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		if (iPass == 0)
		{
			// draw the ground
			m_pTerrain->DrawTerrain();
		}

		// draw our trees from back to front
#if NTREELIBRARY == 1
		CTree *pTreeLibrary = &m_rTreeLibrary[0];
		pTreeLibrary->Begin(true);
#endif
		for (int iTree = m_riNumTrees[iLOD] - 1; iTree >= 0; iTree--)
		{
			TreeData *pTree = &m_rTree[m_rrTreeSort[iLOD][iTree].iTree];
			if (!pTree->bVisible)
				continue;

#if NTREELIBRARY > 1			
			CTree *pTreeLibrary = &m_rTreeLibrary[pTree->iTreeLibrary];
#endif
			bool bFullGeometryBothPasses = pTree->rLOD[0].dwDrawFlags == TREE_DRAWFULLGEOMETRY
					&& pTree->rLOD[1].dwDrawFlags == TREE_DRAWFULLGEOMETRY;
			if (iPass == 0)
			{
				// for the pass shared by both blend buffers, draw the
				// tree only if the most-detailed geometry-only
				// version is active for both LOD's
				if (!bFullGeometryBothPasses)
					continue;
			}
			else
			{
				// don't redraw the tree, if already drawn in pass 0
				if (bFullGeometryBothPasses)
					continue;
			}	
			
			g_pd3dDevice->SetTransform( D3DTS_WORLD, &pTree->mat );
#if NTREELIBRARY > 1			
			pTreeLibrary->Begin(true);
#endif
			// Draw the current level-of-detail representation of the tree
			// TODO: Move all the LOD stuff to the tree class
			if (pTree->rLOD[iLOD].dwDrawFlags == 0)
			{
				// set texture level-of-detail for whole tree
				for (UINT iDir = 0; iDir < pTreeLibrary->m_nDirection; iDir++)
					pTreeLibrary->m_rSliceTexture[iDir].SetLevelOfDetail(pTree->rLOD[iLOD].fTreeSliceTextureLOD);
				g_TREESTATS.dwSliceCount++;
			}
			else if (!(pTree->rLOD[iLOD].dwDrawFlags & TREE_DRAWBRANCHES))
			{
				// set texture level-of-detail for branches
				for (UINT iDir = 0; iDir < pTreeLibrary->m_TreeBranch.m_nDirection; iDir++)
					pTreeLibrary->m_TreeBranch.m_rSliceTexture[iDir].SetLevelOfDetail(pTree->rLOD[iLOD].fBranchSliceTextureLOD);
				g_TREESTATS.dwBranchSliceCount++;
			}
			else
				g_TREESTATS.dwFullGeometryCount++;
			pTreeLibrary->DrawLOD(pTree->vFromFade, pTree->rLOD[iLOD].vFrom, pTree->rLOD[iLOD].dwDrawFlags);
#if NTREELIBRARY > 1			
			pTreeLibrary->End();
#endif			
		}
#if NTREELIBRARY == 1
		pTreeLibrary->End();
#endif

		g_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );	// restore world transform
		g_pd3dDevice->EndScene();

		if (bDebugCulling)
		{
			extern HRESULT DebugDrawFrustum();
			DebugDrawFrustum();
		}

		if (iPass == 0 && m_fBlendFactor >= fZeroBlend)
		{
			// Copy backbuffer to temporary texture, and depth buffer to temporary depth buffer
			LPDIRECT3DSURFACE8 pBlendSurface = NULL;
			m_pBlendTexture->GetSurfaceLevel( 0, &pBlendSurface );
			m_pd3dDevice->CopyRects(m_pBackBuffer, NULL, 1, pBlendSurface, NULL);
			pBlendSurface->Release();
			m_pd3dDevice->CopyRects(m_pDepthBuffer, NULL, 1, m_pBlendDepthBuffer, NULL);
		}
		else if (iPass == 2)
		{
			// Add result to frame buffer, blending in smoothly according to the blend factor
			m_pd3dDevice->SetRenderTarget( m_pBackBuffer, NULL );
			D3DXCOLOR colorBlend(m_fBlendFactor, m_fBlendFactor, m_fBlendFactor, m_fBlendFactor);
			BlendScreenTexture(m_pBlendTexture, colorBlend);
		}
	}

	// show game title or help
	g_pd3dDevice->BeginScene();
	if( g_bDrawHelp )
		m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
	else
	{
		m_Font.Begin();
		m_Font.DrawText(64, 50, 0xffffffff, L"TREES");
#ifdef _DEBUG
		m_Font.DrawText(575, 50, 0xffffff00, m_strFrameRate, XBFONT_RIGHT);
#endif
		if (m_fNumTrees >= 1.f)
		{
			float x = 64.f, y = 200.f, dy = 30.f;	// text positioning
			const int buflen = 100;
			WCHAR buf[buflen];
			_snwprintf(buf, buflen, L"Tree count %d", (int)floorf(m_fNumTrees));
			buf[buflen - 1] = L'\0';
 			m_Font.DrawText(x, y += dy, 0xffffffff, buf);
			if (g_bDebugSlice)
			{
				if (g_bDebugSliceOpaque)
	 				m_Font.DrawText(64, 80, 0xffffffff, L"Debug opaque");
				else
	 				m_Font.DrawText(64, 80, 0xffffffff, L"Debug");

				_snwprintf(buf, buflen, L"Active trees %d", g_TREESTATS.dwActiveCount);
				buf[buflen - 1] = L'\0';
				m_Font.DrawText(x, y += dy, 0xffffffff, buf);

				_snwprintf(buf, buflen, L"Full geometry trees %d", g_TREESTATS.dwFullGeometryCount);
				buf[buflen - 1] = L'\0';
				m_Font.DrawText(x, y += dy, 0xffffffff, buf);

				_snwprintf(buf, buflen, L"Branch slice trees %d", g_TREESTATS.dwBranchSliceCount);
				buf[buflen - 1] = L'\0';
				m_Font.DrawText(x, y += dy, 0xffffffff, buf);

				_snwprintf(buf, buflen, L"Slice trees %d", g_TREESTATS.dwSliceCount);
				buf[buflen - 1] = L'\0';
				m_Font.DrawText(x, y += dy, 0xffffffff, buf);

				_snwprintf(buf, buflen, L"Total slices %d", g_dwTotalSliceCount);
				buf[buflen - 1] = L'\0';
				m_Font.DrawText(x, y += dy, 0xffffffff, buf);
			}
		}
		m_Font.End();
	}
	g_pd3dDevice->EndScene();

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: BlendScreenTexture
// Desc: This function renders the level-of-detail blend texture to
//       the screen with a specified blending factor.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::BlendScreenTexture(LPDIRECT3DTEXTURE8 pTexture, D3DCOLOR colorBlend )
{
	// Texture coordinates in linear format textures go from 0 to n-1 rather
	// than the 0 to 1 that is used for swizzled textures.
	D3DSURFACE_DESC desc;
	pTexture->GetLevelDesc(0, &desc);
    struct BACKGROUNDVERTEX { D3DXVECTOR4 p; FLOAT tu, tv; } v[4];
    v[0].p = D3DXVECTOR4( -0.5f,             -0.5f,              1.0f, 1.0f ); v[0].tu = 0.0f;              v[0].tv = 0.0f;
    v[1].p = D3DXVECTOR4( desc.Width - 0.5f, -0.5f,              1.0f, 1.0f ); v[1].tu = (float)desc.Width; v[1].tv = 0.0f;
    v[2].p = D3DXVECTOR4( -0.5f,             desc.Height - 0.5f, 1.0f, 1.0f ); v[2].tu = 0.0f;              v[2].tv = (float)desc.Height;
    v[3].p = D3DXVECTOR4( desc.Width - 0.5f, desc.Height - 0.5f, 1.0f, 1.0f ); v[3].tu = (float)desc.Width; v[3].tv = (float)desc.Height;
	
    // Set states
    g_pd3dDevice->SetPixelShader( 0 );
    g_pd3dDevice->SetTexture( 0, pTexture );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP );
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
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
    return S_OK;
}
