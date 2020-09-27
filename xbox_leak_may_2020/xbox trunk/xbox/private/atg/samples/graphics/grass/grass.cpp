//-----------------------------------------------------------------------------
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
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
#include "XBGrass.h"
#include "Terrain.h"

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
	{ XBHELP_LEFTSTICK,	   XBHELP_PLACEMENT_2, L"Move in-out and left-right" },
	{ XBHELP_RIGHTSTICK,   XBHELP_PLACEMENT_2, L"Rotate and tilt view" },
	{ XBHELP_DPAD,		   XBHELP_PLACEMENT_2, L"Move up and down" },
	{ XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_1, L"Toggle grass" },
	{ XBHELP_WHITE_BUTTON, XBHELP_PLACEMENT_1, L"Toggle wire-frame" },
	{ XBHELP_A_BUTTON,	   XBHELP_PLACEMENT_2, L"Launch ball" },
	{ XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Toggle help" },
    { XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
};
#define NUM_HELP_CALLOUTS (sizeof(g_HelpCallouts) / sizeof(XBHELP_CALLOUT))

//-----------------------------------------------------------------------------
// Global scale parameters
//-----------------------------------------------------------------------------
D3DXVECTOR3 g_vGravity(0.f, 0.f, -9.8f);
D3DXVECTOR3 g_vLightDirection = D3DXVECTOR3(-0.577350269f, 0.577350269f, 0.577350269f);
D3DXVECTOR3 g_vMin(-16.f, -16.f, 0.f), g_vMax(16.f, 16.f, 0.5f);	// scale of height field in world coords
float g_fGrassHeight = 0.2f;
bool g_bWireFrame = false;
bool g_bDrawTerrain = true;
bool g_bDrawGrass = true;
bool g_bDrawBall = true;
bool g_bFollowBall = true;
bool g_bDebugBall = false;
bool g_bDebugDraw = false;
bool g_bDrawHelp = false;			// Whether to draw help

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
	D3DXVECTOR3		m_vFrom, m_vAt, m_vUp;	// Viewing parameters
	D3DXMATRIX		m_matView;
	D3DXMATRIX		m_matViewInverse;
	D3DXMATRIX		m_matProjection;
	Terrain		   *m_pTerrain; // current terrain representation
	CXBGrass		m_grass;	// grass slice texture
	D3DXVECTOR4		m_vTextureScaleGrass;
	D3DXVECTOR4		m_vOffsetGrass;
	float			m_fLODGrass;
	CXBMesh			m_meshBall;
	D3DXVECTOR3		m_vBallPosition;
	D3DXVECTOR3		m_vBallVelocity;
	D3DXMATRIX		m_matBallRoll;
	DWORD			m_dwBallVertexShader;
	float			m_fLaunchCountDown;
public:
	CXBoxSample();
	HRESULT DrawGrass();
	HRESULT LaunchBall();
	HRESULT MoveBall();
	
	// CXBApplication overrides
	HRESULT Initialize();
	HRESULT FrameMove();
	HRESULT Render();
	HRESULT Cleanup();
};

#ifndef _XBOX
//-----------------------------------------------------------------------------
// Name: wWinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//		 message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, INT )
{
	CXBoxSample xbApp;
	if( FAILED( xbApp.Create() ) )
		return 0;
	return xbApp.Run();
}
#else
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
#endif

//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
			:CXBApplication()
{
/*
#ifdef _DEBUG	
	// Allow unlimited frame rate
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif
*/
	m_pTerrain = NULL;
	m_dwBallVertexShader = 0;
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
	HRESULT hr;
	m_bPaused = false;

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
    if( FAILED( m_xprResources.Create( m_pd3dDevice, "Resource.xpr", 
                                       resource_NUM_RESOURCES ) ) )
        return E_FAIL;

	// load the terrain image and create the height field mesh
	m_pTerrain = new Terrain;
	if (m_pTerrain == NULL)
		return E_OUTOFMEMORY;
	LPDIRECT3DTEXTURE8 pTextureTerrain = m_xprResources.GetTexture( (DWORD)resource_TerrainTexture_OFFSET );
	hr = m_pTerrain->Initialize(pTextureTerrain, pTextureTerrain, g_vMin, g_vMax, g_fGrassHeight); // use the terrain heightfield image as a texture map, too
	if (FAILED(hr))
		return hr;

	// Set camera parameters
	m_vFrom = D3DXVECTOR3( 1.0f, 2.f, 4.0f );
	m_vAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vUp = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );

	// initialize the grass slice texture
	{
		DWORD numfuzz = 8000;
		DWORD numfuzzlib = 32;
		DWORD numslices = 10;
		DWORD slicexsize = 256; // 128;
		DWORD slicezsize = 256; // 128;
		m_grass.m_dwSeed = 51101213;
		m_grass.m_fXSize = 0.1f;
		m_grass.m_fYSize = 0.01f;
		m_grass.m_fZSize = 0.1f;
		m_grass.m_dwNumSegments = 4;
		m_grass.m_fuzzRandom.colorBase = D3DXCOLOR(0.2f, 0.2f, 0.2f, 0.0f);
		m_grass.m_fuzzCenter.colorBase = D3DXCOLOR(0.2f, 1.0f, 0.2f, 1.f) - 0.5f * m_grass.m_fuzzRandom.colorBase;
		m_grass.m_fuzzRandom.colorTip = D3DXCOLOR(0.0f, 0.0f, 0.f, 0.0f);
		m_grass.m_fuzzCenter.colorTip = D3DXCOLOR(0.0f, 0.1f, 0.f, 0.1f) - 0.5f * m_grass.m_fuzzRandom.colorTip;
		m_grass.m_fuzzCenter.dp = D3DXVECTOR3(0.0f, 4.f, 0.5f);
		m_grass.m_fuzzRandom.dp = D3DXVECTOR3(0.25f, 0.25f, 0.25f);
		m_grass.m_fuzzCenter.ddp = D3DXVECTOR3(0.f, 0.f, 0.0f);
		m_grass.m_fuzzRandom.ddp = D3DXVECTOR3(0.5f, 0.5f, 0.5f);
		OUTPUT_DEBUG_STRING("Grass: distribute...");
		m_grass.InitFuzz(numfuzz, numfuzzlib);
		OUTPUT_DEBUG_STRING(" slice...");
		m_grass.GenSlices(numslices, slicexsize, slicezsize);
		OUTPUT_DEBUG_STRING(" LOD...");
		m_grass.ComputeLevelOfDetailTextures();
		m_grass.SetLevelOfDetail(0.f);
		D3DMATERIAL8 material;
		ZeroMemory(&material, sizeof(material));
		material.Ambient  = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
		material.Diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
		material.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		material.Power = 40.0f;
		m_grass.SetHairLightingMaterial(&material);
		m_vTextureScaleGrass = D3DXVECTOR4(g_vMax.x - g_vMin.x, g_vMax.y - g_vMin.y, 0.f, 0.f);
		m_vOffsetGrass = D3DXVECTOR4(0.f, 0.f, 0.2f * 0.5f / (g_vMax.z - g_vMin.z), 0.f); // Z is up
		OUTPUT_DEBUG_STRING(" compress...");
		m_grass.SetTextureFormat(D3DFMT_DXT4);
		m_fLODGrass = 0.f;
		OUTPUT_DEBUG_STRING(" done.\n");
	}

	// Load the ball mesh
	if( FAILED( m_meshBall.Create( m_pd3dDevice, "Models\\GBall.xbg" ) ) )
		return XBAPPERR_MEDIANOTFOUND;
	LaunchBall();
	D3DXMatrixIdentity(&m_matBallRoll);

	// Load the ball vertex shader
	{
		DWORD vsdecl[] = 
		{
			D3DVSD_STREAM(0),
			D3DVSD_REG(0, D3DVSDT_FLOAT3),		// position
			D3DVSD_REG(1, D3DVSDT_FLOAT3),		// normal
			D3DVSD_END()
		};
		XBUtil_CreateVertexShader(m_pd3dDevice, _FNA("Shaders\\golfball.xvu"), vsdecl, &m_dwBallVertexShader);
		if(!m_dwBallVertexShader)
			OUTPUT_DEBUG_STRING("Initialize : error loading golfball.xvu\n");
	}
	
	// set light parameters, but disable light by default
	D3DLIGHT8 light;
	ZeroMemory(&light, sizeof(D3DLIGHT8));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Position = D3DXVECTOR3(10000.f, 10000.f, 10000.f); // sun
	light.Direction = g_vLightDirection;
	light.Ambient  = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	light.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	light.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->LightEnable(0, FALSE);
	return hr;
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Clean up after ourselves.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Cleanup()
{
	SAFE_DELETE(m_pTerrain);
	if (m_dwBallVertexShader)
		g_pd3dDevice->DeleteVertexShader(m_dwBallVertexShader);
	return S_OK;
}



//-----------------------------------------------------------------------------
// Name: DrawGrass
// Desc:
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawGrass()
{
	// Setup pixel shader texture state for grass
	UINT xx = 0;
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAKILL, D3DTALPHAKILL_ENABLE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_TEXCOORDINDEX, 0);
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetTextureStageState( xx, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);	 // premultiplied alpha
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	static DWORD dwAlphaRef = 0;
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, dwAlphaRef);
	UINT nLayer;
	D3DXVECTOR4 vOffsetTotal;
	LPDIRECT3DTEXTURE8 *apSliceTexture;
	m_grass.SetLevelOfDetail(m_fLODGrass); 
	nLayer = m_grass.m_dwNumSlicesLOD;
	vOffsetTotal = m_vOffsetGrass;
	apSliceTexture = m_grass.m_pSliceTextureLOD;
	g_pd3dDevice->SetVertexShaderConstant( 2, &m_vTextureScaleGrass, 1);
	float fExtraShadowOffset = 0.5f;
	D3DXVECTOR4 vExtraShadow(-(1.f - fExtraShadowOffset) / vOffsetTotal.z, 1.f, vOffsetTotal.z, 1.f);
	g_pd3dDevice->SetVertexShaderConstant( 9, &vExtraShadow, 1);
	float fScale = 1.f/(float)nLayer;
	for (UINT iLayer = 0; iLayer < nLayer; iLayer++)
	{
		float fFraction = (iLayer+1)*fScale;
		D3DXVECTOR4 vShellOffset = fFraction * vOffsetTotal;
		g_pd3dDevice->SetTexture( 0, apSliceTexture[iLayer]);
		g_pd3dDevice->SetVertexShaderConstant( 1, &vShellOffset, 1);
		D3DXVECTOR4 vLayerFraction(1.f, 0.f, 1.f - fFraction, fFraction);
		g_pd3dDevice->SetVertexShaderConstant( 8, &vLayerFraction, 1);
		static float s_fSF = 0.f;
		float fShadowFraction = (iLayer + s_fSF) * fScale;
		float fShadow = 0.1f + 0.6f * fShadowFraction * fShadowFraction;
		D3DXVECTOR4 vColor(fShadow, fShadow, fShadow, 1.f);
		g_pd3dDevice->SetVertexShaderConstant( 3, &vColor, 1);
		m_pTerrain->DrawTerrain();
	}
	return S_OK;
}

static float fBallRadius = 0.05f/2; // meters, current art.	 Actual golf ball should be 0.042672/2 radius (1.68 inches in diameter)
static float fBallMass = 0.0460227f; // 1.62 ounces * 1 pound / 16 ounces * 1 kg / 2.2 pound

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//		 the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
		g_bDrawHelp = !g_bDrawHelp;

	if (m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
		g_bDrawGrass = !g_bDrawGrass;
	
	if (m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
		g_bWireFrame = !g_bWireFrame;
	
	if (m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		LaunchBall();

	// Scale translation by height above z-plane
	float fTranslateScale = fabsf(m_vFrom.z) + 0.01f;

	// update view position
	static float fOffsetScale = 0.5f;
	float fX1 = m_DefaultGamepad.fX1;
	fX1 *= fX1 * fX1; // fX1 cubed
	float fY1 = m_DefaultGamepad.fY1;
	fY1 *= fY1 * fY1; // fY1 cubed
	D3DXVECTOR3 vOffset(fX1, 0.f, fY1);
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
		vVerticalOffset.z += fTranslateScale * fVerticalScale * m_fElapsedTime;
	if(m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
		vVerticalOffset.z -= fTranslateScale * fVerticalScale * m_fElapsedTime;
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
		vAtOffset.y -= fAtOffsetScale * fY2 * m_fElapsedTime;
	D3DXVec3TransformNormal(&vAtOffset, &vAtOffset, &m_matViewInverse);
	m_vAt += vAtOffset;

	// For the projection matrix, we set up a perspective transform (which
	// transforms geometry from 3D view space to 2D viewport space, with
	// a perspective divide making objects smaller in the distance). To build
	// a perpsective transform, we need the field of view (1/4 pi is common),
	// the aspect ratio, and the near and far clipping planes (which define at
	// what distances geometry should be no longer be rendered).
	D3DXMatrixPerspectiveFovLH( &m_matProjection, D3DX_PI/4, 640.f/480.f, 0.01f, 100.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjection );

	// Set up our view matrix.
	D3DXMatrixLookAtLH( &m_matView, &m_vFrom, &m_vAt, &m_vUp);
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
	D3DXMatrixInverse(&m_matViewInverse, NULL, &m_matView);

	// Set world matrix to identity
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

	// Set the vertex shader constants for the ball
	//	c20-c23 = world*view*proj matrix
	//	c24 = light direction in object coords
	//	c25 = clamping color
	MoveBall();
	D3DXMATRIX matBallScale;
	static float fBallScale = 8.f; // ball looks too small, so make it appear bigger
	D3DXMatrixScaling(&matBallScale, fBallScale, fBallScale, fBallScale);
	// Make the ball roll in the direction of travel.  A better physics model would produce the rolling velocity directly.
	float fVelocity = D3DXVec2Length((D3DXVECTOR2 *)&m_vBallVelocity);
	if (fVelocity > 1e-4f)
	{
		static float fBallRollFactor = -10.f;
		float fBallRollAngle = fBallRollFactor * D3DXVec2Dot((D3DXVECTOR2 *)&m_vBallVelocity, (D3DXVECTOR2 *)&m_vBallPosition) / fVelocity;
		D3DXVECTOR3 vBallRollAxis;
		D3DXVec3Cross(&vBallRollAxis, &m_vBallVelocity, &m_vUp);
		D3DXVec3Normalize(&vBallRollAxis, &vBallRollAxis);
		D3DXMatrixRotationAxis(&m_matBallRoll, &vBallRollAxis, fBallRollAngle);
	}
	D3DXMATRIX matBallTranslate;
	D3DXMatrixTranslation(&matBallTranslate, m_vBallPosition.x, m_vBallPosition.y, m_vBallPosition.z);
	D3DXMATRIX matBall = matBallScale * m_matBallRoll * matBallTranslate;
	D3DXMATRIX matBallWorldViewProjection = matBall * m_matView * m_matProjection;
	D3DXMATRIX matBallTranspose;
	D3DXMatrixTranspose(&matBallTranspose, &matBallWorldViewProjection);
	g_pd3dDevice->SetVertexShaderConstant( 20, &matBallTranspose, 4 );

	// transform light direction
	D3DXVECTOR3 vLightDirection;
	D3DXMATRIX matBallRollTranspose;
	D3DXMatrixTranspose(&matBallRollTranspose, &m_matBallRoll);
	D3DXVec3TransformNormal(&vLightDirection, &g_vLightDirection, &matBallRollTranspose);
	g_pd3dDevice->SetVertexShaderConstant( 24, &vLightDirection, 1 );
	
	static D3DXCOLOR rColorBall[] =
	{
		D3DXCOLOR(2.0f, 2.0f, 2.0f, 1.f),		// diffuse hemisphere conditioning
		D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f),		// ambient hemisphere conditioning should be larger than clamp value
		D3DXCOLOR(-0.1f, -0.1f, -0.1f, 1.f),	// clamp to negative number to let a light "bleed" around edges
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.f),		// diffuse dimple
		D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f),		// ambient dimple
	};
	g_pd3dDevice->SetVertexShaderConstant( 25, &rColorBall, 5 );
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: LaunchBall
// Desc: Fire new ball in a random direction
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::LaunchBall()
{
#define fUnitRand(fmin, fmax) ((float)rand()/32768.0f*((fmax) - (fmin)) + (fmin))
	float fTheta = fUnitRand(0.f, 2.f * D3DX_PI);
	float fRadius = 16.f;
	float fX = cosf(fTheta);
	float fY = sinf(fTheta);
	m_vBallPosition = D3DXVECTOR3(fX*fRadius, fY*fRadius, 5.f);
	m_vBallVelocity = D3DXVECTOR3(-fX*fUnitRand(2.f, 10.f), -fY*fUnitRand(2.f, 10.f), fUnitRand(0.5f, 2.5f));
	if (g_bFollowBall)
	{
		m_vAt.x = m_vBallPosition.x;
		m_vAt.y = m_vBallPosition.y;
		m_vAt.z = m_vBallPosition.z;
//		m_vFrom.x = m_vAt.x + vBallOffset.x;
//		m_vFrom.y = m_vAt.y + vBallOffset.y;
	}
	static bool bAutoRefine = true;
	if (bAutoRefine)
	{
		// Get rough estimate of when ball is going to hit z=0 plane
		float a = 0.5f * g_vGravity.z;
		float b = m_vBallVelocity.z;
		float c = m_vBallPosition.z;
		// Solve quadratic a t^2 + b t + c = 0
		float q = b*b - 4.0f * a * c;
		if (q < 0.0)
			return 0; // no real solution
		if (b < 0.0) // q = -(1/2)*(b + sgn(b)*sqrt(b^2 - 4ac))
			q = -0.5f * (b - sqrtf(q));
		else
			q = -0.5f * (b + sqrtf(q));
		// Roots are q/a and c/q.
		float t = q / a;
		float t1 = c / q;
		if (t < 0.f || (t1 > 0.f && t1 < t)) // use smaller positive root
			t = t1;
		D3DXVECTOR3 vBallEstimatedHit = m_vBallPosition + t * m_vBallVelocity + 0.5f * t * t * g_vGravity;
		static float fRollTime = 0.2f;
		vBallEstimatedHit.x += fRollTime * m_vBallVelocity.x;
		vBallEstimatedHit.y += fRollTime * m_vBallVelocity.y;
		m_pTerrain->Refine(vBallEstimatedHit, -1.f); // add a refinement below the ball
		// TODO: Instead of making a rough estimate of the final position, compute the whole
		// trajectory now and then play it back at real-speed.	That way, we could adjust the
		// mesh and choose a good camera angle ahead of time.
	}
	m_fLaunchCountDown = 0.f; // stop any other pending launches
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: MoveBall
// Desc: Called once per frame (if not paused) to update ball position
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::MoveBall()
{
	// Get forces from grass and terrain
	static float fGrassHeight = 0.2f;
	static float fGrassRest = 0.1f; // must be less than fGrassHeight
	static float fGrassSpring = 9.8f / (fGrassHeight - fGrassRest); // cancel gravity when ball is at grass resting point
	D3DXVECTOR3 vBallPosition0 = m_vBallPosition; // save current ball position for update call
	float fTerrainHeight0;
	D3DXVECTOR3 vTerrainNormal0;
	m_pTerrain->GetTerrainPoint(m_vBallPosition, &fTerrainHeight0, &vTerrainNormal0);
	float fDepth = fTerrainHeight0 + fGrassHeight - (m_vBallPosition.z - fBallRadius);	// depth of penetration of bottom of ball
	D3DXVECTOR3 vAcceleration = g_vGravity;
	if (fDepth > 0.f)
	{
		// Apply grass force.  
		// TODO: use correct normal and better friction model that separates
		// normal and tangential components. Add rolling to the model, too.
		if (fDepth > fGrassHeight) fDepth = fGrassHeight; // Terrain collision is modeled below as impulse.
		float fAcceleration = fGrassSpring * fDepth;
		vAcceleration += vTerrainNormal0 * fAcceleration;

		// Dampen velocity
		static D3DXVECTOR3 vGrassDamping(0.9f, 0.9f, 0.8f);
		m_vBallVelocity.x *= vGrassDamping.x;
		m_vBallVelocity.y *= vGrassDamping.y;
		m_vBallVelocity.z *= vGrassDamping.z;
	}
	
	// Update the position of the ball
	float dt = m_fElapsedAppTime;
	float dtThreshold = 1.f/10.f;
	if (dt > dtThreshold) dt = dtThreshold;
	m_vBallPosition += dt * m_vBallVelocity + 0.5f * dt * dt * vAcceleration;
	m_vBallVelocity += dt * vAcceleration;
	
	// Check for collisions with terrain and bounding box
	float fTerrainHeight;
	D3DXVECTOR3 vTerrainNormal; // TODO: use normal to compute new bounce direction
	m_pTerrain->GetTerrainPoint(m_vBallPosition, &fTerrainHeight, &vTerrainNormal);
	float fHeight = fTerrainHeight + fBallRadius;
	if (m_vBallPosition.x < g_vMin.x && m_vBallVelocity.x < 0.f)
		m_vBallVelocity.x = -m_vBallVelocity.x;
	else if (m_vBallPosition.x > g_vMax.x && m_vBallVelocity.x > 0.f)
		m_vBallVelocity.x = -m_vBallVelocity.x;
	if (m_vBallPosition.y < g_vMin.y && m_vBallVelocity.y < 0.f)
		m_vBallVelocity.y = -m_vBallVelocity.y;
	else if (m_vBallPosition.y > g_vMax.y && m_vBallVelocity.y > 0.f)
		m_vBallVelocity.y = -m_vBallVelocity.y;
	if (m_vBallPosition.z < fHeight /*g_vMin.z*/ && m_vBallVelocity.z < 0.f)
	{
		m_vBallPosition.z = fHeight + fGrassRest; // slide ball out of intersection
		m_vBallVelocity.z = -m_vBallVelocity.z; // reverse velocity

		// Dampen velocity
		static D3DXVECTOR3 vTerrainDamping(0.9f, 0.9f, 0.7f);
		m_vBallVelocity.x *= vTerrainDamping.x;
		m_vBallVelocity.y *= vTerrainDamping.y;
		m_vBallVelocity.z *= vTerrainDamping.z;
	}
//	else if (m_vBallPosition.z > g_vMax.z && m_vBallVelocity.z > 0.f)
//		m_vBallVelocity.z = -vBounce.z * m_vBallVelocity.z;

	// Update terrain
	static float fBallRadiusExtra = fBallRadius * 10.f;
	m_pTerrain->BallUpdate(m_fElapsedAppTime, vBallPosition0, m_vBallPosition, fBallRadiusExtra);

	// Adjust camera
	if (!m_bPaused)
	{
		static D3DXVECTOR3 sBallAt(0.2f, 0.2f, 0.04f);	// gradually follow ball with eye, x and y are faster than z
		static D3DXVECTOR3 sBallFrom(0.02f, 0.02f, 0.001f); // gradually move closer to the ball
		static D3DXVECTOR3 vBallOffset(1.f, 1.f, 1.f);	// don't get right on top of the ball
		static float fBallAnticipation = 0.1f;	// prediction time
		if (g_bFollowBall)
		{
			float dt = fBallAnticipation;
			D3DXVECTOR3 vBallPositionNext = m_vBallPosition + dt * m_vBallVelocity + 0.5f * dt * dt * g_vGravity;
			m_vAt.x = m_vAt.x + sBallAt.x * (vBallPositionNext.x - m_vAt.x);
			m_vAt.y = m_vAt.y + sBallAt.y * (vBallPositionNext.y - m_vAt.y);
			m_vAt.z = m_vAt.z + sBallAt.z * (vBallPositionNext.z - m_vAt.z);
			m_vFrom.x = m_vFrom.x + sBallFrom.x * ((vBallPositionNext.x + vBallOffset.x) - m_vFrom.x);
			m_vFrom.y = m_vFrom.y + sBallFrom.y * ((vBallPositionNext.y + vBallOffset.y) - m_vFrom.y);
			//m_vFrom.z = m_vFrom.z + sBallFrom.z * ((vBallPositionNext.z + vBallOffset.z) - m_vFrom.z);
		}
	}

	// Launch a new ball if the ball has stopped moving and there's been a while to
	// look at the effect of the ball on the grass.
	static float fVelocityThreshold = 0.01f;
	static float fCountDownStart = 5.f; // seconds until new ball is launched
	if (m_fLaunchCountDown > 0.f)
	{
		m_fLaunchCountDown -= m_fElapsedAppTime;
		if (m_fLaunchCountDown <= 0.f)
		{
			LaunchBall();	// Launch new ball
			m_fLaunchCountDown = 0.f;
		}
	}
	else if (D3DXVec3LengthSq(&m_vBallVelocity) < fVelocityThreshold)
	{
		m_fLaunchCountDown = fCountDownStart;
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//		 rendering. This function sets up render states, clears the
//		 viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
	RenderGradientBackground(D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f), D3DXCOLOR(0.3f, 0.3f, 0.6f, 1.f));
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	if (g_bWireFrame)
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
	// draw the ball
	if (g_bDrawBall)
	{
		g_pd3dDevice->SetVertexShader( m_dwBallVertexShader );
		m_meshBall.Render( g_pd3dDevice, XBMESH_NOTEXTURES | XBMESH_NOFVF );
	}

	// draw the landscape
	m_pTerrain->Begin();
	if (g_bDrawTerrain)
		m_pTerrain->DrawTerrain();
	if (g_bDrawGrass)
		DrawGrass();
	if (g_bDebugDraw)
		m_pTerrain->DrawDebug();
	m_pTerrain->End();

	// show game title or help
	if( g_bDrawHelp )
		m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
	else
	{
		m_Font.Begin();
		m_Font.DrawText(60, 30, 0xffffffff, L"GRASS");
		m_Font.DrawText(600, 30, 0xffffff00, m_strFrameRate, XBFONT_RIGHT);
		m_Font.End();
	}
	
    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}
