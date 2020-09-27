//-----------------------------------------------------------------------------
// File: xbview.cpp
//
// Desc: Demonstrates how to load resources from an XBR (Xbox Resource) file.
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include "XBR.h"
#include "texture.h"
#include "print.h"
#include "DebugCmd.h"
#include "util.h"

//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
	{ XBHELP_LEFTSTICK,	   XBHELP_PLACEMENT_2, L"Texture size" },
	{ XBHELP_A_BUTTON,	   XBHELP_PLACEMENT_1, L"Next" },
	{ XBHELP_B_BUTTON,	   XBHELP_PLACEMENT_1, L"Previous" },
	{ XBHELP_X_BUTTON,	   XBHELP_PLACEMENT_1, L"Toggle Alpha" },
	{ XBHELP_BLACK_BUTTON, XBHELP_PLACEMENT_1, L"Reload" },
	{ XBHELP_DPAD,		   XBHELP_PLACEMENT_1, L"Level" },
	{ XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_1, L"Help" },
	{ XBHELP_START_BUTTON, XBHELP_PLACEMENT_1, L"Pause" },
	{ XBHELP_LEFT_BUTTON,  XBHELP_PLACEMENT_1, L"Rewind" },
	{ XBHELP_RIGHT_BUTTON, XBHELP_PLACEMENT_1, L"Fast-Forward" },
};
#define NUM_HELP_CALLOUTS (sizeof(g_HelpCallouts)/sizeof(XBHELP_CALLOUT))

//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//		 from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
public:
	// Member variables
	CXBHelp					m_Help;					// Help data
	BOOL					m_bDrawHelp;			// Should we draw help info?
	CXBFont					m_Font;					// Font for rendering text

	XBR						m_XBR;					// Resource handler
	CHAR				   *m_strBundle;			// Name of the xpr bundle to load
	DWORD					m_nCurrent;				// Currently displayed resource
	DWORD					m_dwStateBlock;			// State block used to insulate effect drawing

	LPDIRECT3DVERTEXBUFFER8 m_pQuadVB;				// Quad for displaying textures
	FLOAT					m_fWidth;
	FLOAT					m_fHeight;
	
	DWORD					m_nLevel;				// which texture mipmap or skeleton level to display
	BOOL					m_bAlpha;				// draw just the alpha channel

	FLOAT					m_fTimeLocal;			// m_fAppTime * m_fTimeScale + m_fTimeOrigin
	FLOAT					m_fTimeOrigin;			// origin of time for animation shifting
	FLOAT					m_fTimeScale;			// scale of time

	D3DXMATRIX				m_matProjection;		// current projection matrix
	D3DXMATRIX				m_matView;				// current view  matrix
	D3DXMATRIX				m_matWorld;				// current world matrix

	FLOAT					m_fScrollX, m_fScrollY;		// position of debugging output string

	// Creates and sets up our vertex buffer
	HRESULT InitGeometry( FLOAT fWidth, FLOAT fHeight, FLOAT fMaxU, FLOAT fMaxV );

	// Texture quad drawing
	HRESULT DrawTexture();	// draw current texture

	// Effect drawing
	HRESULT DrawEffect(Effect *pEffect);
	
	// Skeleton drawing
	HRESULT DrawSkeleton(Skeleton *pSkeleton);

	// Time setting
	HRESULT SetLocalTime(FLOAT fTimeLocal)
	{
		m_fTimeLocal = fTimeLocal;
		m_fTimeOrigin = m_fTimeLocal - m_fAppTime * m_fTimeScale;	// adjust origin to match desired time
		return m_XBR.SetTime(m_fTimeLocal);
	}

	// Resource index setting
	HRESULT SetResource(DWORD ResourceIndex)
	{
		if (ResourceIndex >= m_XBR.Count())
			return E_INVALIDARG;
		m_nCurrent	= ResourceIndex;
		m_fWidth	= 1.0f;
		m_fHeight	= 1.0f;
		m_nLevel = (DWORD)-1;
		g_strOut[0] = 0;	// clear debug string
		m_fScrollX = m_fScrollY = 0.f;

		// Tell the custom debugger about the new resource
		DCPrintf("SetResource(%d)\n", ResourceIndex);
		return S_OK;
	}
	
public:
	CXBoxSample();
	HRESULT Initialize();
	HRESULT FrameMove();
	HRESULT Render();

	// Debug channel variable and command handlers
	static const DCVARDEF s_rgDCVars[];
	static const UINT s_nDCVars;
	static const DCCMDDEF s_rgDCCmds[];
	static const UINT s_nDCCmds;
	static void DC_set_time(LPVOID pvAddr);
	static void DC_resource(int argc, char *argv[]);
	static void DC_frame(int argc, char *argv[]);

} g_xbApp;



//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
void __cdecl main()
{
	if( FAILED( g_xbApp.Create() ) )
		return;

	g_xbApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Initialize member varaibles
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
			:CXBApplication()
{
#ifdef _DEBUG
	m_d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	// Allow unlimited frame rate
#endif
	m_bDrawHelp		= FALSE;
	m_pQuadVB		= NULL;
	m_fWidth		= 1.0f;
	m_fHeight		= 1.0f;
	m_nCurrent		= 0;
	m_dwStateBlock  = 0;
	m_nLevel	    = (DWORD)-1;
	m_bAlpha	    = false;
	m_strBundle = "D:\\Media\\xbrc_out.xbr";
	m_fScrollX = m_fScrollY = 0.f;
	m_fTimeOrigin = 0.f;
	m_fTimeScale = 1.f;
	m_bPaused = true;
}



//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::InitGeometry( FLOAT fWidth, FLOAT fHeight, FLOAT fMaxU, FLOAT fMaxV )
{
	// Create and lock our vertex buffer
	if( NULL == m_pQuadVB )
	{
		if( FAILED( m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( CUSTOMVERTEX ), D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &m_pQuadVB ) ) )
			return E_FAIL;
	}
	
	// Set up a quad in the vertex buffer
	CUSTOMVERTEX* pVertices;
	if( FAILED( m_pQuadVB->Lock( 0, 0, (BYTE **)&pVertices, 0L ) ) )
		return E_FAIL;

	pVertices[0].p	= D3DXVECTOR3( -fWidth/2, -fHeight/2, 1.0f );
	pVertices[0].n	= D3DXVECTOR3(	0.0f, 0.0f, -1.0f );
	pVertices[0].diffuse = 0xFFFFFFFF;
	pVertices[0].tu = 0.0f; 
	pVertices[0].tv = fMaxV;

	pVertices[1].p	= D3DXVECTOR3( -fWidth/2, fHeight/2, 1.0f );
	pVertices[1].n	= D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	pVertices[1].diffuse = 0xFFFFFFFF;
	pVertices[1].tu = 0.0f; 
	pVertices[1].tv = 0.0f;

	pVertices[2].p	= D3DXVECTOR3( fWidth/2, -fHeight/2, 1.0f );
	pVertices[2].n	= D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	pVertices[2].diffuse = 0xFFFFFFFF;
	pVertices[2].tu = fMaxU; 
	pVertices[2].tv = fMaxV;

	pVertices[3].p	= D3DXVECTOR3( fWidth/2, fHeight/2, 1.0f );
	pVertices[3].n	= D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
	pVertices[3].diffuse = 0xFFFFFFFF;
	pVertices[3].tu = fMaxU; 
	pVertices[3].tv = 0.0f;

	m_pQuadVB->Unlock();

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Performs whatever set-up is needed, for example, loading textures,
//		 creating vertex buffers, etc.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
	// Create the font
	if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
		return XBAPPERR_MEDIANOTFOUND;

	// Create the help
	if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
		return XBAPPERR_MEDIANOTFOUND;

	// Set up our vertex buffer
	if( FAILED( InitGeometry( m_fWidth, m_fHeight, 1.0f, 1.0f ) ) )
		return E_FAIL;

	// Set up our projection, view, and world matrices
	// For now, hard-code tiny's projection matrix
	D3DXMATRIX matProj( 1.86326f, 0.f, 0.f, 0.f,
						0.f, 2.41421f, 0.f, 0.f,
						0.f, 0.f, -1.00008f, -1.f, 
						0.f, 0.f, -5.91465f, 0.f );
	m_matProjection = matProj;
	D3DXMatrixIdentity( &m_matView );
	D3DXMatrixIdentity( &m_matWorld );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjection );
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );

	// Set up a light
	D3DLIGHT8 m_Light;
	ZeroMemory( &m_Light, sizeof( D3DLIGHT8 ) );
	m_Light.Type = D3DLIGHT_DIRECTIONAL;
	m_Light.Diffuse.r = 1.0f;
	m_Light.Diffuse.g = 1.0f;
	m_Light.Diffuse.b = 1.0f;
	m_Light.Diffuse.a = 1.0f;
	m_Light.Direction = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
	m_pd3dDevice->SetLight( 0, &m_Light );
	m_pd3dDevice->LightEnable( 0, TRUE );

	// Start loading our resources from the file
	m_XBR.StartLoading( m_pd3dDevice, m_strBundle );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Handles controller input each frame
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	bool bSetTime = !m_bPaused;

	// Toggle help
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK )
		m_bDrawHelp = !m_bDrawHelp;

	// reload the resources
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK] )
	{
		if( FAILED( m_XBR.StartLoading( m_pd3dDevice, m_strBundle ) ) )
			return E_FAIL;
		SetResource(0);
		m_fTimeOrigin = -m_fAppTime * m_fTimeScale; // sets animation time to 0
	}

	if( m_XBR.PollLoadingState() != LOADING_DONE )
		return S_FALSE; // if loading or failed, do not do the rest of the updates

	// Rewind with left trigger and fast-forward with right trigger
	static float fRewindScale = 0.0001f;
	static float fFastForwardScale = 0.0001f;
	float fLeftTrigger = (float)m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER];
	fLeftTrigger *= fLeftTrigger;	// left trigger squared
	float fRightTrigger = (float)m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER];
	fRightTrigger *= fRightTrigger; // right trigger squared
	float fRewind = fRewindScale * fLeftTrigger;
	float fFastForward = fFastForwardScale * fRightTrigger;
	float fDelta = (fFastForward - fRewind);
	if (fDelta != 0.f)
	{
		m_fTimeOrigin += m_fElapsedTime * fDelta;
		bSetTime = true;	// update animations
	}

	// Cube thumbsticks for finer control
	static float fDeadZone = 0.005f;
	float fX1 = m_DefaultGamepad.fX1;
	fX1 = fX1 * fX1 * fX1;
	if (fabsf(fX1) < fDeadZone) fX1 = 0.f;
	float fY1 = m_DefaultGamepad.fY1;
	fY1 = fY1 * fY1 * fY1;
	if (fabsf(fY1) < fDeadZone) fY1 = 0.f;
	float fX2 = m_DefaultGamepad.fX2;
	fX2 = fX2 * fX2 * fX2;
	if (fabsf(fX2) < fDeadZone) fX2 = 0.f;
	float fY2 = m_DefaultGamepad.fY2;
	fY2 = fY2 * fY2 * fY2;
	if (fabsf(fY2) < fDeadZone) fY2 = 0.f;

	// Left thumbstick resizes texture quad
	m_fWidth  = max( 0.01f, min( 2.0f, m_fWidth	 *= (0.01f*fX1+1) ) );
	m_fHeight = max( 0.01f, min( 2.0f, m_fHeight *= (0.01f*fY1+1) ) );

	// Right thumbstick scrolls debug display
	static float fScrollXScale =  200.f;
	static float fScrollYScale = -400.f;
	m_fScrollX += fScrollXScale * fX2 * m_fElapsedTime;
	m_fScrollY += fScrollYScale * fY2 * m_fElapsedTime;

	// move forward through resources
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A] )
	{
		SetResource(( m_nCurrent + 1 ) % m_XBR.Count());
	}


	// move backward through resources
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_B] )
	{
		SetResource( ( m_nCurrent + m_XBR.Count() - 1 ) % m_XBR.Count());
	}
	
	// toggle alpha
	if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_X] )
		m_bAlpha = !m_bAlpha;

	// move between mipmap levels or skeleton hierarchy
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN)
		m_nLevel++;
	if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP)
	{
		if( m_nLevel != -1 )
			m_nLevel--;
	}

	// set the current animation time
	if (bSetTime)
	{
		m_fTimeLocal = m_fAppTime * m_fTimeScale + m_fTimeOrigin;
		m_XBR.SetTime(m_fTimeLocal);
	}
	
    // Process any pending commands from the debug channel
    DCHandleCmds();
	
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawTexture
// Desc: draw current texture resource
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawTexture()
{
	if (m_XBR.GetTypeByIndex(m_nCurrent) != D3DCOMMON_TYPE_TEXTURE)
		return E_INVALIDARG;

	// Set up our view, projection, and world matrices
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/3, 1.0f, 1.0f, 10.0f );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	D3DXMATRIX matView;
	D3DXMatrixTranslation( &matView, 0.0f, 0.0f, 1.0f );
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	// Get texture format
	DWORD *pRes	 = (DWORD *)m_XBR.GetResourceByIndex(m_nCurrent);
	DWORD dwFmt = (*(pRes+3) & D3DFORMAT_FORMAT_MASK) >> D3DFORMAT_FORMAT_SHIFT;
	for(UINT x=0; g_TextureFormats[x].id != 0; x++)
		if(g_TextureFormats[x].id == dwFmt)
			break;
	if( g_TextureFormats[x].id != 0 )
	{
		// Set up quad to render
		if(g_TextureFormats[x].type == FMT_LINEAR)
		{
			DWORD dwU = (*(pRes+4) & D3DSIZE_WIDTH_MASK)+1;
			DWORD dwV = ((*(pRes+4) & D3DSIZE_HEIGHT_MASK) >> D3DSIZE_HEIGHT_SHIFT) + 1;
			InitGeometry( m_fWidth, m_fHeight, (FLOAT)dwU, (FLOAT)dwV );
		}
		else
		{
			InitGeometry( m_fWidth, m_fHeight, 1.0f, 1.0f );
		}
	}

	// Set up our render and texture stage states
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,	TRUE );
	m_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
	
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
	if (m_bAlpha)
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE );
	else
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,	 D3DTADDRESS_CLAMP );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,	 D3DTADDRESS_CLAMP );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSW,	 D3DTADDRESS_CLAMP );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	if( m_nLevel == -1 )
	{
		// blend between miplevels for ordinary texture mapping
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAXMIPLEVEL, 0);	// use the most-detailed level as appropriate
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPMAPLODBIAS, 0);		
	}
	else
	{
		// draw just the desired miplevel
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAXMIPLEVEL, m_nLevel);	 // use just the desired miplevel
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPMAPLODBIAS, FtoDW(-100.f));	// bias choice to desired miplevel
	}
 
	// Select our texture and quad
	m_pd3dDevice->SetTexture( 0, (D3DBaseTexture *)m_XBR.GetResourceByIndex(m_nCurrent) );
	m_pd3dDevice->SetStreamSource( 0, m_pQuadVB, sizeof( CUSTOMVERTEX ) );

	// Draw the quad
	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	// Unselect the texture
	m_pd3dDevice->SetTexture( 0, NULL );
	
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DrawEffect
// Desc: save state, draw effect, restore state
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawEffect(Effect *pEffect)
{
	// Take state snapshot
	HRESULT hr = S_OK;
	if (m_dwStateBlock == 0)
	{
		hr = g_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &m_dwStateBlock);
		if (FAILED(hr))
			return hr;
	}

	// Draw the effect
	hr = m_XBR.DrawEffect(pEffect);
					
	// Restore state
	g_pd3dDevice->SetVertexShaderInput(0, 0, 0);	// this is not handled by the state block
	g_pd3dDevice->ApplyStateBlock(m_dwStateBlock);
	return hr;
}

//-----------------------------------------------------------------------------
// Name: DrawSkeleton
// Desc: Draw the hierarchy of coordinate frames
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::DrawSkeleton(Skeleton *pSkeleton)
{
	BeginAxes();
	
	// Iterate through frame hierarchy
	struct FrameStackElement {
		Frame *pFrame;
		UINT Indent;
	} rFrameStack[FRAME_MAX_STACK];
	rFrameStack[0].pFrame = pSkeleton->m_pRoot;
	rFrameStack[0].Indent = 0;
	UINT iMatrix = 0;	// index to current entry in m_rMatrix
	UINT iFrameStack = 1;	// depth of stack
	while (iFrameStack)
	{
		// Pop the stack
		iFrameStack--;
		Frame *pFrame = rFrameStack[iFrameStack].pFrame;
		UINT Indent = rFrameStack[iFrameStack].Indent;

		// Draw the current frame
		m_pd3dDevice->SetTransform(D3DTS_WORLD, &pSkeleton->m_rMatrix[iMatrix]);
		DrawAxes();

		// Print current frame
		if (iMatrix == m_nLevel)
		{
			PrintFrame(pFrame);
			Out(L"	cumulative\n");
			PrintMatrix(&pSkeleton->m_rMatrix[iMatrix]);
			Out(L"\n");
		}

		// Push sibling
		if (pFrame->m_pNext != NULL)
		{
			if (iFrameStack >= FRAME_MAX_STACK)
				return E_FAIL; // we hit our hard-coded stack-depth limit
			rFrameStack[iFrameStack].pFrame = pFrame->m_pNext;
			rFrameStack[iFrameStack].Indent = Indent;
			iFrameStack++;
		}
		
		// Push child
		if (pFrame->m_pChild != NULL)
		{
			if (iFrameStack >= FRAME_MAX_STACK)
				return E_FAIL; // we hit our hard-coded stack-depth limit
			rFrameStack[iFrameStack].pFrame = pFrame->m_pChild;
			rFrameStack[iFrameStack].Indent = Indent + 1;
			iFrameStack++;
		}
		
		iMatrix++;
	}

	EndAxes();
	return S_OK;
}
	
//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Performs the graphics operations to render the texture on screen
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
	const int DescriptionLength = 1000;
	WCHAR strDescription[DescriptionLength];

	// Begin the scene
	m_pd3dDevice->BeginScene();

	m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
						 0x00000000, 1.0f, 0L );

	RenderGradientBackground( 0x0000ffff, 0x00ffffff );

	wcscpy(strDescription, L"");
	LOADINGSTATE LoadingState = m_XBR.CurrentLoadingState();
	if( LoadingState != LOADING_DONE )
	{
		// If we're not done reading from disk, display a message
		if (LoadingState == LOADING_NOTSTARTED)
			wcscpy( strDescription, L"Loading not yet started." );
		else if (LoadingState == LOADING_HEADER)
			wcscpy( strDescription, L"Loading resource header.." );
		else if (LoadingState == LOADING_DATA)
			wcscpy( strDescription, L"Loading resource data..." );
		else if (LoadingState == LOADING_FAILED)
		{
			WCHAR strBundle[_MAX_PATH];
			mbstowcs( strBundle, m_strBundle, _MAX_PATH );
			strBundle[_MAX_PATH - 1] = 0;	// make sure string is null-terminated
			_snwprintf( strDescription, DescriptionLength, L"Resources failed to load from\n\"%s\"", strBundle);
		}
		strDescription[DescriptionLength-1] = 0;	// make sure string is null terminated
	}
	else if (LoadingState == LOADING_DONE)
	{
		// Get info about resource and prepare description string
		DWORD dwType = m_XBR.GetTypeByIndex(m_nCurrent);
		if (dwType == D3DCOMMON_TYPE_TEXTURE)
		{
			DWORD dwFmt, *pRes, dwDim, dwU, dwV, dwP, dwLevels, dwCube;
			WCHAR *pTexType, strRes[256], strLevels[256];
			pRes  = (DWORD *)m_XBR.GetResourceByIndex(m_nCurrent);
			dwFmt = (*(pRes+3) & D3DFORMAT_FORMAT_MASK) >> D3DFORMAT_FORMAT_SHIFT;
			for(UINT x=0; g_TextureFormats[x].id != 0; x++)
			{
				if(g_TextureFormats[x].id == dwFmt)
					break;
			}
			if(g_TextureFormats[x].id != 0)
			{
				// get information about the texture
				dwDim	 = (*(pRes+3) & D3DFORMAT_DIMENSION_MASK) >> D3DFORMAT_DIMENSION_SHIFT;
				dwLevels = (*(pRes+3) & D3DFORMAT_MIPMAP_MASK) >> D3DFORMAT_MIPMAP_SHIFT;
				dwCube	 = (*(pRes+3) & D3DFORMAT_CUBEMAP);
				if(g_TextureFormats[x].type == FMT_LINEAR)
				{
					dwU = (*(pRes+4) & D3DSIZE_WIDTH_MASK)+1;
					dwV = ((*(pRes+4) & D3DSIZE_HEIGHT_MASK) >> D3DSIZE_HEIGHT_SHIFT) + 1;
					dwP = 0;
				}
				else
				{
					dwU = exptbl[(*(pRes+3) & D3DFORMAT_USIZE_MASK) >> D3DFORMAT_USIZE_SHIFT];
					dwV = exptbl[(*(pRes+3) & D3DFORMAT_VSIZE_MASK) >> D3DFORMAT_VSIZE_SHIFT];
					dwP = exptbl[(*(pRes+3) & D3DFORMAT_PSIZE_MASK) >> D3DFORMAT_PSIZE_SHIFT];
				}
				
				if(dwDim == 2)
				{
					swprintf( strRes, L"%dx%d", dwU, dwV );
					pTexType = L"2D Texture";
				}
				else if(dwCube)
				{
					swprintf( strRes, L"%dx%d", dwU, dwV );
					pTexType = L"Cubemap";
				}
				else
				{
					pTexType = L"3D Texture";
					swprintf( strRes, L"%dx%dx%d", dwU, dwV, dwP );
				}
				
				if( dwLevels == 1 )
					swprintf( strLevels, L"1 level");
				else
				{
					if( m_nLevel == -1 )
						swprintf( strLevels, L"%d levels", dwLevels );
					else
					{
						if( m_nLevel >= dwLevels )
							m_nLevel = dwLevels - 1; // constrain choice of miplevel to appropriate range
						swprintf( strLevels, L"level %d of %d levels", m_nLevel, dwLevels );
					}
				}

				swprintf( strDescription, L"Resource %d of %d	 %s\n%s %s %s\n%s", 
						  m_nCurrent + 1, m_XBR.Count(), pTexType, strRes, 
						  g_TextureFormats[x].name, strLevels,
						  m_bAlpha ? L"alpha channel only" : L"");
			}
			else
			{
				swprintf( strDescription, L"Resource %d of %d	  Texture\n", 
						  m_nCurrent + 1, m_XBR.Count() );
			}
			DrawTexture();	// draw textured quad
		}
		else if (dwType == D3DCOMMON_TYPE_VERTEXBUFFER)
		{
			swprintf( strDescription, L"Resource %d of %d	  VertexBuffer\n", 
					  m_nCurrent + 1, m_XBR.Count());
		}
		else if ((dwType & D3DCOMMON_TYPE_MASK) == D3DCOMMON_TYPE_INDEXBUFFER)
		{
			swprintf( strDescription, L"Resource %d of %d	  IndexBuffer\n", 
					  m_nCurrent + 1, m_XBR.Count());
		}
		else if (dwType == XBRC_VERTEXSHADER)
		{
			VertexShader *pVertexShader = (VertexShader *)m_XBR.GetResourceByIndex(m_nCurrent);
			swprintf( strDescription, L"Resource %d of %d	  VertexShader\nHandle=0x%08x\nVertexShaderReference=%d",
					  m_nCurrent + 1, m_XBR.Count(),
					  pVertexShader->Handle,
					  pVertexShader->VertexShaderReference );
			if (m_nLevel == -1)
			{
				g_strOut[0] = 0;	// clear debug string
				m_XBR.PrintVertexShaderInputs(m_nCurrent + 1);
				m_XBR.PrintVertexShaderDeclaration(m_nCurrent + 1);
				//m_nLevel = 0;
			}
		}
		else if (dwType == XBRC_PIXELSHADER)
		{
			swprintf( strDescription, L"Resource %d of %d	  PixelShader\n", 
					  m_nCurrent + 1, m_XBR.Count());
		}
		else if (dwType == XBRC_EFFECT)
		{
			Effect *pEffect = (Effect *)m_XBR.GetResourceByIndex(m_nCurrent);
			WCHAR strIdentifier[EFFECT_IDENTIFIER_SIZE];
			mbstowcs( strIdentifier, pEffect->Identifier, EFFECT_IDENTIFIER_SIZE );
			strIdentifier[EFFECT_IDENTIFIER_SIZE - 1] = 0;	// make sure string is null-terminated
			_snwprintf( strDescription, DescriptionLength, L"Resource %d of %d	   Effect\nID=\"%s\"\nRenderTarget=%d PassCount=%d",
						m_nCurrent + 1, m_XBR.Count(),
						strIdentifier, pEffect->RenderTargetResource, pEffect->PassCount );
			strDescription[DescriptionLength - 1] = 0;	// make sure string is terminated
			DrawEffect(pEffect);
		}
		else if (dwType == XBRC_SKELETON)
		{
			Skeleton *pSkeleton = (Skeleton *)m_XBR.GetResourceByIndex(m_nCurrent);
			WCHAR strIdentifier[FRAME_IDENTIFIER_SIZE];
			if (pSkeleton->m_FrameCount > 0)
			{
				int len = strlen(pSkeleton->m_pRoot->m_strName);
				mbstowcs( strIdentifier, pSkeleton->m_pRoot->m_strName, len );
			}
			strIdentifier[FRAME_IDENTIFIER_SIZE - 1] = 0;	// make sure string is null-terminated
			_snwprintf( strDescription, DescriptionLength, L"Resource %d of %d	   Skeleton\nID=\"%s\" FrameCount=%d",
						m_nCurrent + 1, m_XBR.Count(),
						strIdentifier, pSkeleton->m_FrameCount);
			strDescription[DescriptionLength - 1] = 0;	// make sure string is terminated

			// Set projection, view, and world matrices
			m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjection);
			m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_matView );
			m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matWorld );
					
			g_strOut[0] = 0;	// clear debug string
			DrawSkeleton((Skeleton *)m_XBR.GetResourceByIndex(m_nCurrent));
		}
		else if (dwType == XBRC_ANIMATION)
		{
			Animation *pAnimation = (Animation *)m_XBR.GetResourceByIndex(m_nCurrent);
			_snwprintf( strDescription, DescriptionLength, L"Resource %d of %d	   Animation\nTargetCount=%d AnimationCount=%d",
						m_nCurrent + 1, m_XBR.Count(),
						pAnimation->m_VSPCount, pAnimation->m_AnimCount);
			strDescription[DescriptionLength - 1] = 0;	// make sure string is terminated
		}
		else if (dwType & 0x80000000)
			swprintf( strDescription, L"Resource %d of %d	  UserData\ntype=0x%08x\n", 
					  m_nCurrent + 1, m_XBR.Count(), dwType);
		else
			swprintf( strDescription, L"Resource %d of %d	  Unknown\ntype=0x%08x\n", 
					  m_nCurrent + 1, m_XBR.Count(), dwType);
	}

	// Show title, frame rate, resource description, and help
	if( m_bDrawHelp )
		m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
	else
	{
		m_Font.Begin();
		m_Font.DrawText(  64,  50, 0xffffffff, L"xbview" );
		m_Font.DrawText( 450,  50, 0xffffff00, m_strFrameRate );
		WCHAR strAppTime[100];
		swprintf( strAppTime, L"%0.02f s", m_fAppTime * m_fTimeScale + m_fTimeOrigin );
		m_Font.DrawText( 450,  80, 0xffffff00, strAppTime );
		if (m_bPaused)
			m_Font.DrawText( 450,  110, 0xff888800, L"Paused");
		m_Font.DrawText(  64,  80, 0xff0000ff, strDescription );
		m_Font.DrawText(  64 + m_fScrollX, 150 + m_fScrollY, 0xff888888, g_strOut);
		m_Font.End();
	}

	m_pd3dDevice->EndScene();

	// Present the scene
	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: TypeName
// Desc: Return a string for the given type
//-----------------------------------------------------------------------------
CHAR *TypeName(DWORD dwType)
{
	if (dwType == D3DCOMMON_TYPE_TEXTURE)
		return "Texture";
	else if (dwType == D3DCOMMON_TYPE_VERTEXBUFFER)
		return "VertexBuffer";
	else if ((dwType & D3DCOMMON_TYPE_MASK) == D3DCOMMON_TYPE_INDEXBUFFER)
		return "IndexBuffer";
	else if (dwType == XBRC_VERTEXSHADER)
		return "VertexShader";
	else if (dwType == XBRC_PIXELSHADER)
		return "PixelShader";
	else if (dwType == XBRC_EFFECT)
		return "Effect";
	else if (dwType == XBRC_SKELETON)
		return "Skeleton";
	else if (dwType == XBRC_ANIMATION)
		return "Animation";
	else if (dwType & 0x80000000)
		return "UserData";
	else
		return "<Unknown>";
}

//-----------------------------------------------------------------------------
// Debug command handlers
//-----------------------------------------------------------------------------
bool g_bQuiet = false;

void CXBoxSample::DC_set_time(LPVOID pvAddr)
{
	FLOAT *pfTime = (FLOAT *)pvAddr;
	g_xbApp.SetLocalTime(*pfTime);
}

void CXBoxSample::DC_resource(int argc, char *argv[])
{
	XBR *pXBR = &g_xbApp.m_XBR;
	
	// Use argv[0] to limit the types of resources to look for.
	static struct MAPTYPE {
		CHAR *str;
		DWORD dwType;
		DWORD dwMask;
	} s_rMapType[] = {
	  {"effect",    XBRC_EFFECT,		~0ul },
	  {"skeleton",  XBRC_SKELETON,		~0ul },
	  {"anim",   	XBRC_ANIMATION,		~0ul },
	  {"vsh",    	XBRC_VERTEXSHADER,	~0ul },
	  {"psh",    	XBRC_PIXELSHADER,	~0ul },
	  {"ib",     	XBRC_INDEXBUFFER,	~0ul },
	  {"texture",   D3DCOMMON_TYPE_TEXTURE,			XBRC_USER_DATA_FLAG | D3DCOMMON_TYPE_MASK},
	  {"vb",     	D3DCOMMON_TYPE_VERTEXBUFFER,	XBRC_USER_DATA_FLAG | D3DCOMMON_TYPE_MASK},
	};
	static UINT s_MapTypeCount = sizeof(s_rMapType) / sizeof(MAPTYPE);
	DWORD dwTypeDesired = (DWORD)-1;
	DWORD dwMask = ~0ul;
	for (UINT iMapType = 0; iMapType < s_MapTypeCount; iMapType++)
	{
		if (!_stricmp(argv[0], s_rMapType[iMapType].str))
		{
			dwTypeDesired = s_rMapType[iMapType].dwType;
			dwMask = s_rMapType[iMapType].dwMask;
			break;
		}
	}
	
    if(argc == 1)
    {
        // if we aren't passed any arguments, then just list all the resources
        for (DWORD SymbolIndex = 0; SymbolIndex < pXBR->SymbolCount(); SymbolIndex++)
        {
			CONST CHAR *strName = pXBR->GetSymbolName(SymbolIndex);
			if (dwTypeDesired != -1)
			{
				// Only print out symbols matching the desired type
				DWORD ResourceIndex = pXBR->GetResourceIndexBySymbolIndex(SymbolIndex);
				if (ResourceIndex == (DWORD)-1)
					DCPrintf("resource '%s' not found with symbol index %d\n", argv[1], SymbolIndex);
				DWORD dwType = pXBR->GetTypeByIndex(ResourceIndex);
				if ((dwType & dwMask) != dwTypeDesired)
					continue;	// skip it, since it's not the right type
			}
			DCPrintf("%s\n", strName);
		}
    }
    else
    {
		// Look through symbol table for matching symbol
		for (DWORD SymbolIndex = 0; SymbolIndex < pXBR->SymbolCount(); SymbolIndex++)
		{
			CONST CHAR *strName = pXBR->GetSymbolName(SymbolIndex);
			if (!_stricmp(argv[1], strName))
			{
				DWORD ResourceIndex = pXBR->GetResourceIndexBySymbolIndex(SymbolIndex);
				if (ResourceIndex == (DWORD)-1)
				{
					DCPrintf("resource '%s' not found with symbol index %d\n", argv[1], SymbolIndex);
					return;
				}
				DWORD dwType = pXBR->GetTypeByIndex(ResourceIndex);
				if (dwTypeDesired != -1 
					&& (dwType & dwMask) != dwTypeDesired)
					continue;	// skip it, since it's not the right type
				g_xbApp.SetResource(ResourceIndex);
				DCPrintf("resource '%s' index=%d type=%s(0x%.08x)\n", argv[1], ResourceIndex, TypeName(dwType), dwType);
				return;
			}
		}
		DCPrintf("resource '%s' not found\n", argv[1]);
    }
}

void CXBoxSample::DC_frame(int argc, char *argv[])
{
	XBR *pXBR = &g_xbApp.m_XBR;
	if (argc == 1)
	{
		DWORD SkeletonResourceIndex = g_xbApp.m_nCurrent;
		DWORD dwType = pXBR->GetTypeByIndex(SkeletonResourceIndex);
		if (dwType != XBRC_SKELETON)
		{
			// Dump list of skeletons
			DCPrintf("use 'skeleton' to set current frame hierarchy\n");
			argv[0] = "skeleton";
			DC_resource(argc, argv);
			return;
		}

		// Dump frame hierarchy
		Skeleton *pSkeleton = (Skeleton *)pXBR->GetResourceByIndex(SkeletonResourceIndex);
		struct FrameStackElement {
			Frame *pFrame;
			bool bChildrenDone;
		} rFrameStack[FRAME_MAX_STACK];
		rFrameStack[0].pFrame = pSkeleton->m_pRoot;
		rFrameStack[0].bChildrenDone = false;
		int iFrameStack = 1;	// depth of stack
		while (iFrameStack)
		{
			// Get the top of the stack
			iFrameStack--;
			Frame *pFrame = rFrameStack[iFrameStack].pFrame;
			bool bChildrenDone = rFrameStack[iFrameStack].bChildrenDone;

			if (!bChildrenDone)
			{
				// Get full frame path
				const UINT lenName = FRAME_MAX_STACK * FRAME_IDENTIFIER_SIZE;
				CHAR strName[lenName];
				strName[0] = 0;
				for (int i = 0; i <= iFrameStack; i++)
				{
					strncat(strName, "/", lenName);
					strncat(strName, rFrameStack[i].pFrame->m_strName, lenName);
				}
				strName[lenName - 1] = 0;	// make sure string is terminated

				// Print current frame
				FLOAT *pf = (FLOAT *)pFrame->m_Matrix;
				DCPrintf("frame %s = %g %g %g %g    %g %g %g %g    %g %g %g %g    %g %g %g %g\n",
						 strName,
						 pf[0], pf[1], pf[2], pf[3],
						 pf[4], pf[5], pf[6], pf[7],
						 pf[8], pf[9], pf[10], pf[11],
						 pf[12], pf[13], pf[14], pf[15]);
		
				// Push child
				if (pFrame->m_pChild != NULL)
				{
					// keep current node on stack for path, but mark as done
					rFrameStack[iFrameStack].bChildrenDone = true;
					iFrameStack++;
					if (iFrameStack >= FRAME_MAX_STACK)
						return; // we hit our hard-coded stack-depth limit
					rFrameStack[iFrameStack].pFrame = pFrame->m_pChild;
					rFrameStack[iFrameStack].bChildrenDone = false;
					iFrameStack++;
				}
				else
					bChildrenDone = true;

			}

			if (bChildrenDone)
			{
				// Push sibling
				if (pFrame->m_pNext != NULL)
				{
					if (iFrameStack >= FRAME_MAX_STACK)
						return; // we hit our hard-coded stack-depth limit
					rFrameStack[iFrameStack].pFrame = pFrame->m_pNext;
					rFrameStack[iFrameStack].bChildrenDone = false;
					iFrameStack++;
				}
			}
		}
		return;
	}

	// Parse frame path to get skeleton
	CHAR *pStart = argv[1];
	if (*pStart == '/')		// skip initial separator
		pStart++;
	CHAR *pEnd = strchr(pStart, '/');
	if (pEnd) *pEnd = 0;
	DWORD SkeletonResourceIndex = (DWORD)-1;
	for (DWORD SymbolIndex = 0; SymbolIndex < pXBR->SymbolCount(); SymbolIndex++)
	{
		CONST CHAR *strName = pXBR->GetSymbolName(SymbolIndex);
		if (!_stricmp(pStart, strName))
		{
			DWORD ResourceIndex = pXBR->GetResourceIndexBySymbolIndex(SymbolIndex);
			if (ResourceIndex == (DWORD)-1)
			{
				DCPrintf("resource '%s' not found with symbol index %d\n", pStart, SymbolIndex);
				return;
			}
			DWORD dwType = pXBR->GetTypeByIndex(ResourceIndex);
			if (dwType == XBRC_SKELETON)
			{
				SkeletonResourceIndex = ResourceIndex;
				break;
			}
			// else skip it, since it's not a skeleton
		}
	}
	if (SkeletonResourceIndex == -1)
	{
		DCPrintf("skeleton '%s' not found\n", pStart);
		return;
	}
	Skeleton *pSkeleton = (Skeleton *)pXBR->GetResourceByIndex(SkeletonResourceIndex);
	if (pEnd) *pEnd = '/';	// restore slash
	
	// Parse frame path to get desired frame
	Frame *pFrame = pSkeleton->m_pRoot;
	while (pEnd)
	{
		pStart = pEnd + 1;
		if (*pStart == 0) break;
		pEnd = strchr(pStart, '/');
		if (pEnd) *pEnd = 0;

		// Look through children for matching name
		Frame *pFrameChild = pFrame->m_pChild;
		while (pFrameChild)
		{
			if (!strcmp(pStart, pFrameChild->m_strName))
			{
				pFrame = pFrameChild;
				break;
			}
			pFrameChild = pFrameChild->m_pNext;
		}
		if (pFrame != pFrameChild)
		{
			// Did not find a match
			if (pFrameChild == NULL)
				DCPrintf("frame '%s' not found\n", argv[1]);
			else
				DCPrintf("frame '%s' does not match\n", argv[1]);
			return;
		}
		if (pEnd) *pEnd = '/';	// restore slash
	}

	// Print or set frame
	FLOAT *pf = (FLOAT *)pFrame->m_Matrix;
	if (argc > 2)
	{
		INT iarg = 2;
        if (argv[iarg][0] == '=')
			iarg++;
		for (INT i = 0; iarg < argc && i < 16; iarg++, i++)
			pf[i] = (FLOAT)atof(argv[iarg]);
	}
	DCPrintf("frame %s = %g %g %g %g    %g %g %g %g    %g %g %g %g    %g %g %g %g\n",
			 argv[1],
			 pf[0], pf[1], pf[2], pf[3],
			 pf[4], pf[5], pf[6], pf[7],
			 pf[8], pf[9], pf[10], pf[11],
			 pf[12], pf[13], pf[14], pf[15]);
}

//-----------------------------------------------------------------------------
// Variable declaration block, exposed by the "set" command.
//-----------------------------------------------------------------------------
const DCVARDEF *g_rgDCVars = g_xbApp.s_rgDCVars;
const UINT      g_nDCVars  = g_xbApp.s_nDCVars;

const DCVARDEF CXBoxSample::s_rgDCVars[] = 
{
    { "time",       &g_xbApp.m_fTimeLocal,  DC_FLOAT1, DC_set_time },
    { "timeOrigin", &g_xbApp.m_fTimeOrigin, DC_FLOAT1, NULL },
    { "timeScale",  &g_xbApp.m_fTimeScale,  DC_FLOAT1, NULL },
    { "paused", 	&g_xbApp.m_bPaused,     DC_bool, NULL },
    { "proj", 		&g_xbApp.m_matProjection,     DC_MATRIX, NULL },
    { "view", 		&g_xbApp.m_matView,     DC_MATRIX, NULL },
    { "world", 		&g_xbApp.m_matWorld,    DC_MATRIX, NULL },
	{ "quiet",		&g_bQuiet,				DC_bool, NULL },
};
const UINT CXBoxSample::s_nDCVars = sizeof(CXBoxSample::s_rgDCVars) / sizeof(DCVARDEF);

//-----------------------------------------------------------------------------
// Command declaration block
//-----------------------------------------------------------------------------
const DCCMDDEF *g_rgDCCmds = g_xbApp.s_rgDCCmds;
const UINT      g_nDCCmds  = g_xbApp.s_nDCCmds;

const DCCMDDEF CXBoxSample::s_rgDCCmds[] =                   // List of app-defined commands
{
    {"help",      RCmdHelp,				" [CMD]: List commands / usage"},
    {"set",       RCmdSet,				" [VAR [= VAL]]: list variables or set a variable"},
    {"resource",  g_xbApp.DC_resource,  " [name]: current resource"},
    {"texture",   g_xbApp.DC_resource,  " [name]: current texture"},
    {"effect",    g_xbApp.DC_resource,  " [name]: current effect"},
    {"skeleton",  g_xbApp.DC_resource,  " [name]: current skeleton"},
    {"anim",   	  g_xbApp.DC_resource,  " [name]: current animation"},
    {"vsh",    	  g_xbApp.DC_resource,  " [name]: current vertex shader"},
    {"psh",    	  g_xbApp.DC_resource,  " [name]: current pixel shader"},
    {"ib",     	  g_xbApp.DC_resource,  " [name]: current index buffer"},
    {"vb",     	  g_xbApp.DC_resource,  " [name]: current vertex buffer"},
	{"frame",	  g_xbApp.DC_frame,     " [name]: current frame hierarchy"},
};
const UINT CXBoxSample::s_nDCCmds = sizeof(CXBoxSample::s_rgDCCmds) / sizeof(DCCMDDEF);
