//-----------------------------------------------------------------------------
// FILE: BENMARK5.CPP
//
// Desc: Main BenMark file. This was all based on the dx7
//  BenMark5 Ribbons exe written by Ben DeWaal of nVidia.
//
// Ben says: "The key point here is NOT to use D3DVBCAPS_SYSTEMMEMORY.
//  It will halve the triangle rate."
//
// Copyright (c) 1999-2000 NVIDIA Corporation. All rights reserved.
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT


#include <stdio.h>
#include <math.h>
#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <macros.h>
#include <xgraphics.h>

#include "benmark5.h"

static ScenarioMenu s_LeftTriggerMenu = 
{
	XINPUT_GAMEPAD_LEFT_TRIGGER,
	L"Toggle Effects",
	{
		L"Toggle specular lighting",
		L"Toggle Z buffer",
		L"Toggle local view",
		NULL,
		NULL,
		NULL
	},
	3
};

static ScenarioMenu s_RightTriggerMenu = 
{
	XINPUT_GAMEPAD_RIGHT_TRIGGER,
	L"Lighting",
	{
		L"Add directional light",
		L"Add spot light",
		L"Add point light",
		L"Remove directional light",
		L"Remove spot light",
		L"Remove point light"
	},
	6
};

static ScenarioMenu s_LeftThumbMenu = 
{
	XINPUT_GAMEPAD_LEFT_THUMB,
	L"Performance",
	{
		L"Toggle number of Textures from 0 - 2",
		L"Toggle Triangle List / Triangle Strip",
		L"Toggle Solid/Point/Wireframe fillmode",
		L"Toggle DrawIndexedPrimitive/DrawIndexedPrimitiveUP routine",
        L"Toggle realtime/precompiled draws",
        NULL
	},
	5
};

//=========================================================================
// Given a D3DFORMAT, return string description.
//=========================================================================

const WCHAR *SzFmtName(D3DFORMAT d3dfmt)
{
    #define XTAG(x) {x, L#x}
    static const struct FMTD3DFMT
    {
        D3DFORMAT d3dfmt;
        WCHAR *szName;
    } rgFmtD3dFmt[] =
    {
        XTAG(D3DFMT_X8R8G8B8),
        XTAG(D3DFMT_A1R5G5B5),
        XTAG(D3DFMT_A8R8G8B8),
        XTAG(D3DFMT_A4R4G4B4),
        XTAG(D3DFMT_R5G6B5),
        XTAG(D3DFMT_A1R5G5B5),

        XTAG(D3DFMT_UNKNOWN),
    };
    static const WCHAR szD3dUnkwn[] = L"D3DFMT_???";

    for(int i = 0; i < sizeof(rgFmtD3dFmt) / sizeof(rgFmtD3dFmt[0]); i++)
    {
        if(d3dfmt == rgFmtD3dFmt[i].d3dfmt)
            return rgFmtD3dFmt[i].szName;
    }

    return szD3dUnkwn;
}

//=========================================================================
// Background thread loop.
//=========================================================================

DWORD WINAPI ThreadProc
(
  LPVOID lpParameter   // thread data
)
{
    CBenMark *pMark = (CBenMark *)lpParameter;

    while (!pMark->m_fTerminateThread)
    {
        for (volatile double i = 1000000000.0f; i != 0; i = i / 10.0f)
            ;

        InterlockedIncrement(&pMark->m_lDivides);
    }

    return 0;
}


//=========================================================================
// CBenMark constructor
//=========================================================================

CBenMark::CBenMark()
//	: m_IniFile( BENMARK_INI_PATH )
{
    m_pwIndices = NULL;
    m_pIndexBuffer = NULL;

    m_pVBRibbon[0] = NULL;
    m_pVBRibbon[1] = NULL;

    m_pVertices[0] = NULL;
    m_pVertices[1] = NULL;

    m_pPushBuffer  = NULL;

    m_pTexture1 = NULL;
    m_pTexture2 = NULL;
    m_pBackSurf = NULL;

    m_dwFVF = 0;
    m_dwFVFSize = 0;

    m_dwIndexCount = 0;
    m_dwNumVertices = 0;

    m_lDivides      = 0;
    m_dwAveCount     = 0;
    m_fdAveTriPerSec = 0.0;
    m_fdMaxTriPerSec = 0.0;
    m_fdMinTriPerSec = 1e199;

	m_nNumLights = 0;

    m_hThread = 0;

	m_wLoop			= 0;
	m_wLoops			= GetPrivateProfileIntA( "BENMARK5", "Loops", 1, BENMARK_INI_PATH );
	ReadIni();

    m_bUseDepthBuffer = TRUE;
    m_DepthBufferFormat = D3DFMT_D16;

	m_hGamePad1		=	NULL;

	m_bShowHelp = FALSE;
	m_bShowStats = TRUE;
	ZeroMemory(m_rgpMenus, sizeof(m_rgpMenus));
	m_iActiveKey = -1;
	m_iActiveMenu = 0;
	m_bRunFullTest = FALSE;
	m_bFrameMoving = TRUE;
	m_bSingleStep = FALSE;
	m_dwCurrentMode = 0;
	m_iSampleType = (D3DMULTISAMPLE_TYPE)0;		 // Current sample mode.
	m_dwSampleMask = 0;      // Mask of available FSAA modes.

	m_dwTotalTimes = 0;

	m_dwStartLoopTime = 0;
}

HRESULT CBenMark::ReadIni()
{
	HRESULT hr = S_OK;

	CHAR szSectionName[256];

	sprintf( szSectionName, "LOOP%d", m_wLoop );

	ZeroMemory(&m_bmxState, sizeof(m_bmxState));

	m_bmxState.cApplyTextures	= GetPrivateProfileIntA( szSectionName, "ApplyTextures", 0, BENMARK_INI_PATH );
	m_bmxState.bUseTriList		= GetPrivateProfileIntA( szSectionName, "UseTriList", 0, BENMARK_INI_PATH );
	m_bmxState.bZenable			= GetPrivateProfileIntA( szSectionName, "Zenable", TRUE, BENMARK_INI_PATH );
	m_bmxState.bSpecularenable	= GetPrivateProfileIntA( szSectionName, "Specularenable", 0, BENMARK_INI_PATH );
	m_bmxState.bLocalviewer		= GetPrivateProfileIntA( szSectionName, "Localviewer", 0, BENMARK_INI_PATH );
	
	m_bmxState.nDirectionalLights = GetPrivateProfileIntA( szSectionName, "DirectionalLights", 1, BENMARK_INI_PATH );
	m_bmxState.nSpotLights		= GetPrivateProfileIntA( szSectionName, "SpotLights", 0, BENMARK_INI_PATH );
	m_bmxState.nPointLights		= GetPrivateProfileIntA( szSectionName, "PointLights", 0, BENMARK_INI_PATH );
	
	m_bmxState.iDrawFunc		= GetPrivateProfileIntA( szSectionName, "DrawFunc", DF_DRAWINDEXEDPRIMITIVE, BENMARK_INI_PATH );
	m_bmxState.fPrecompile		= GetPrivateProfileIntA( szSectionName, "Precompile", 0, BENMARK_INI_PATH );
	m_bmxState.iFillMode		= (D3DFILLMODE)GetPrivateProfileIntA( szSectionName, "FillMode", D3DFILL_SOLID, BENMARK_INI_PATH );

	m_bmxState.dwTime			= GetPrivateProfileIntA( szSectionName, "Time", 60000, BENMARK_INI_PATH );

	return hr;
}

HRESULT CBenMark::InitGraphics()
{
	HRESULT hr = S_OK;

	Initialize3DEnvironment();

	m_dwStartLoopTime = timeGetTime();

	return hr;
}

//=========================================================================
// Called when the app is exitting, or the device is being changed,
// this function deletes any device dependant objects.
//=========================================================================
HRESULT CBenMark::DestroyGraphics()
{
	HRESULT hr = S_OK;

	UpdateStats( (FLOAT)(m_dwCurrentTime-m_dwStartLoopTime) );

	if (m_hThread)
    {
        m_fTerminateThread = TRUE;
        WaitForSingleObject(m_hThread, 2000);
    }
	
    delete[] m_pwIndices;
    m_pwIndices = NULL;
	
    delete[] m_pVertices[0];
    m_pVertices[0] = NULL;
	
    delete[] m_pVertices[1];
    m_pVertices[1] = NULL;
	
    RELEASE(m_pIndexBuffer);
	
    RELEASE(m_pVBRibbon[0]);
    RELEASE(m_pVBRibbon[1]);

    RELEASE(m_pTexture1);
    RELEASE(m_pTexture2);

    RELEASE(m_pPushBuffer);

	RELEASE( m_pBackBuffer );

	RELEASE( m_pDevice );

	RELEASE( m_pD3D );

	return hr;
}

HRESULT CBenMark::DestroyInput()
{
	HRESULT hr = S_OK;

	XInputClose( m_hGamePad1 );

	return hr;
}

HRESULT CBenMark::Update()
{
	HRESULT hr = S_OK;

	if ( m_dwCurrentTime-m_dwStartLoopTime > m_bmxState.dwTime ) // run for time specified in ini file
	{
		m_wLoop++;
		if ( m_wLoop >= m_wLoops )
		{
			m_bQuit = TRUE;
		}
		else
		{
			EXECUTE( DestroyGraphics() );
			EXECUTE( ReadIni() );
			EXECUTE( InitGraphics() );
		}
	}

	return hr;
}

//=========================================================================
// Draw Ben's ribbons
//=========================================================================
HRESULT CBenMark::Render()
{
	HRESULT hr = S_OK;

    FLOAT fRotation2 = 2.0f * D3DX_PI / 2047.0f * (FLOAT)(m_dwCurrentTime-m_dwStartTime)/1000 * 1023;

    // clear -- worth moving this calculation?
    DWORD dwFlags = (m_Presentation.AutoDepthStencilFormat != D3DFMT_UNKNOWN) ?
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER : D3DCLEAR_TARGET;

    EXECUTE(m_pDevice->Clear(0, NULL, dwFlags, 0x00000000, 1.0f, 0L));

    // begin scene
    EXECUTE(m_pDevice->BeginScene());

	// Set up the world translation.
    D3DXMATRIX matWorld;
    D3DXMatrixTranslation(&matWorld, 0.0f, -0.5f * 0.75f * cfRibbonPitch, 0.0f);

    // draw ribbons
    for (DWORD i = 0; i < cdwRibbonCount ; i++)
    {
        // setup matrix
        D3DXMATRIX matWorldT;

        D3DXMatrixRotationAxis(&matWorldT, &D3DXVECTOR3(1.0f, 0.0f, 0.0f),
            fRotation2 + i * 0.4f);
        matWorldT *= matWorld;

        // translate y by fDelta for the next line
        matWorld.m[3][1] += m_fDelta;

        // set our world transform
        EXECUTE(m_pDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&matWorldT));

        // draw outside
        if(m_bmxState.cApplyTextures)
        {
            EXECUTE(m_pDevice->SetTexture          (0, m_pTexture1));

            EXECUTE(m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1,D3DTA_TEXTURE));
            EXECUTE(m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_MODULATE));
            EXECUTE(m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2,D3DTA_DIFFUSE));

            EXECUTE(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,D3DTA_TEXTURE));
            EXECUTE(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_MODULATE));
            EXECUTE(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2,D3DTA_DIFFUSE));

            if(m_bmxState.cApplyTextures > 1)
            {
                EXECUTE(m_pDevice->SetTexture          (1, m_pTexture2));

                EXECUTE(m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1,D3DTA_TEXTURE));
                EXECUTE(m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,  D3DTOP_ADD));
                EXECUTE(m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2,D3DTA_CURRENT));

                EXECUTE(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,D3DTA_TEXTURE));
                EXECUTE(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_ADD));
                EXECUTE(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2,D3DTA_CURRENT));
            }
            else
            {
                EXECUTE(m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,  D3DTOP_DISABLE));
                EXECUTE(m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,  D3DTOP_DISABLE));
            }
        }

        EXECUTE(m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));
        EXECUTE(m_pDevice->SetMaterial(&m_matOut[i]));

        // draw outside ribbon
        (this->*m_pfnDrawRibbon)(0, m_dwPrimType, m_dwTriCount);

        // draw inside
        if(m_bmxState.cApplyTextures)
        {
            EXECUTE(m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1,D3DTA_DIFFUSE));
            EXECUTE(m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_SELECTARG1));
            EXECUTE(m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,  D3DTOP_DISABLE));

            EXECUTE(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,D3DTA_DIFFUSE));
            EXECUTE(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_SELECTARG1));
            EXECUTE(m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,  D3DTOP_DISABLE));
        }

        EXECUTE(m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));
        EXECUTE(m_pDevice->SetMaterial(&m_matIn));

        // draw inside ribbon
        (this->*m_pfnDrawRibbon)(1, m_dwPrimType, m_dwTriCount);

        // next
        m_dwTotalTimes += 1;//2;
    }


	//ShowInfo( 35, 12 );


	if ( m_iActiveMenu )
	{
		ShowMenu();
	}

    // end
    EXECUTE(m_pDevice->EndScene());
	EXECUTE( m_pDevice->Present( NULL, NULL, NULL, NULL));

	//UpdateStats( (FLOAT)(m_dwCurrentTime-m_dwStartTime) );

	return hr;
}


//=========================================================================
// This creates all device-dependant managed objects, such as managed
// textures and managed vertex buffers.
//=========================================================================

void CBenMark::InitDeviceObjects()
{
	HRESULT hr = S_OK;

    InitMaterials(&m_matIn, m_matOut);

    m_pTexture1 = CreateTexture(8, 256);
    m_pTexture2 = CreateTexture(8, 256);
};

//=========================================================================
// Restore device-memory objects and state after a device is created or 
// resized.
//
// This will manually get called any time an option changes so any
// option-specific initialization should go here.
//=========================================================================

void CBenMark::RestoreDeviceObjects()
{
	HRESULT hr = S_OK;
    // initalize our lights
    InitLights();

    // create our vertex buffers and indices
    InitVBs();

	// Set up our transformations.
    D3DXMATRIX mux;
    D3DXMatrixIdentity(&mux);
    D3DXMatrixTranslation(&mux, 0.0f, 0.0f, -10.0f);

    EXECUTE(m_pDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&mux));

    D3DXMatrixPerspectiveFovRH(&mux,
                              D3DX_PI * 30.0f / 360.0f,
                              (float)m_BackBufferDesc.Width / m_BackBufferDesc.Height,
                              1.0f,
                              19.0f);

    EXECUTE(m_pDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&mux));

    // init some render states
    EXECUTE(m_pDevice->SetRenderState(D3DRS_ZENABLE, m_bmxState.bZenable));
    EXECUTE(m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, m_bmxState.bSpecularenable));
    EXECUTE(m_pDevice->SetRenderState(D3DRS_LOCALVIEWER, m_bmxState.bLocalviewer));
	EXECUTE(m_pDevice->SetRenderState(D3DRS_FILLMODE, m_bmxState.iFillMode));
    EXECUTE(m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE));

	// init some random states.
    m_fDelta     = 0.75f * cfRibbonPitch / (cdwRibbonCount - 1);

    // Create the background thread.
    m_fTerminateThread = FALSE;
    m_hThread = CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
    SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL);

    DWORD dwTime = GetTickCount();
    m_lDivides = 0;
    
    Sleep(2000);

    LONG Divides = InterlockedExchange(&m_lDivides, 0);
    dwTime = GetTickCount() - dwTime;

    m_iMaxDivides = Divides * 1000 / dwTime;
};


//=========================================================================
// Called once a second or after a test run to update the 
// test status string.
//=========================================================================

void CBenMark::UpdateStats(float fElapsedTime)
{
    LONG Divides = InterlockedExchange(&m_lDivides, 0);

    // next experiment?
    double rcp = 1e-6 / (double)fElapsedTime;
	//double rcp = 1.0;
    double tps = (double)m_dwTotalTimes * (double)m_dwTriCount * rcp;
    double sps = (double)m_dwTotalTimes * (double)m_dwIndexCount * rcp;
    double ips = (double)m_dwTotalTimes * (double)m_dwIndexCount * 2.0 * rcp;
    double vps = (double)m_dwTotalTimes * (double)m_dwIndexCount * (double)m_dwFVFSize * rcp;
    double dps = (double)Divides / (double)fElapsedTime;

    swprintf(m_szTestStats, L"%i KTri/s, %i KVerts/s, %i KB/s index traffic, "
        L"%iKB/s [effective] vertex traffic, %i background thread loops/second", 
             (INT) (tps * 1000.0f), (INT) (sps * 1000.0f), 
             (INT) (ips * 1000.0f), (INT) (vps * 1000.0f),
             (INT) (dps));

    static DWORD count = 0;

    if (count++ == 0)
    {
        count = 0;
    
        DbgPrint("\n%i KTri/s, %i KVerts/s, %i KB/s index traffic, "
              "%iKB/s [effective] vertex traffic, %i K/s background thread loops "
			  "%d seconds, %d times\n\n", 
             (INT) (tps * 1000.0f), (INT) (sps * 1000.0f), 
             (INT) (ips * 1000.0f), (INT) (vps * 1000.0f),
             (INT) (dps * 1000.0f), (DWORD)fElapsedTime, m_dwTotalTimes);
    }

	static bool isFirst = true;
	
	if (!isFirst)
	{
		m_dwAveCount++;
		m_fdAveTriPerSec += tps;
		m_fdMaxTriPerSec = max(tps, m_fdMaxTriPerSec);
		m_fdMinTriPerSec = min(tps, m_fdMinTriPerSec);
	}

	isFirst = false;

	m_dwTotalTimes = 0;
}

//=========================================================================
// initialize our materials
//=========================================================================

void CBenMark::InitMaterials(D3DMATERIAL8 *pmatIn, D3DMATERIAL8 *pmatOut)
{
    // set material
    ZeroMemory(pmatOut, sizeof(D3DMATERIAL8) * cdwRibbonCount);
    for (DWORD i = 0; i < cdwRibbonCount; i++)
    {
        float r = 0.3f + 0.5f * float(rand()) / float(RAND_MAX);
        float g = 0.3f + 0.5f * float(rand()) / float(RAND_MAX);
        float b = 0.3f + 0.5f * float(rand()) / float(RAND_MAX);

        pmatOut[i].Diffuse.r  = r;
        pmatOut[i].Diffuse.g  = g;
        pmatOut[i].Diffuse.b  = b;
        pmatOut[i].Diffuse.a  = 1.0f;
        pmatOut[i].Ambient.r  = r * 0.3f;
        pmatOut[i].Ambient.g  = g * 0.3f;
        pmatOut[i].Ambient.b  = b * 0.3f;
        pmatOut[i].Ambient.a  = 1.0f;
    }

    // set material
    ZeroMemory(pmatIn, sizeof(D3DMATERIAL8));
    pmatIn->Diffuse.r  = 0.7f;
    pmatIn->Diffuse.g  = 0.3f;
    pmatIn->Diffuse.b  = 0.3f;
    pmatIn->Diffuse.a  = 0.7f;
    pmatIn->Ambient.r  = 0.3f;
    pmatIn->Ambient.g  = 0.1f;
    pmatIn->Ambient.b  = 0.1f;
    pmatIn->Ambient.a  = 1.0f;
}

//=========================================================================
// initialize our lights
//=========================================================================

void CBenMark::InitLights()
{
	HRESULT hr = S_OK;
    // setup lights
    float fIntensity = 1.0f /
        (m_bmxState.nDirectionalLights +
         m_bmxState.nPointLights +
         m_bmxState.nSpotLights);

    // disable all our existing lights
    while(m_nNumLights--)
        EXECUTE(m_pDevice->LightEnable(m_nNumLights, FALSE));

    // get a count of the new dudes
    m_nNumLights =
        m_bmxState.nDirectionalLights +
        m_bmxState.nPointLights +
        m_bmxState.nSpotLights;

    DWORD dwIndex = 0;
    for (int nT = 0; nT < m_bmxState.nDirectionalLights; nT++)
    {
        D3DLIGHT8 lig;

        ZeroMemory(&lig, sizeof(lig));
        lig.Type        =  D3DLIGHT_DIRECTIONAL;
        lig.Diffuse.r   =  1.0f * fIntensity;
        lig.Diffuse.g   =  1.0f * fIntensity;
        lig.Diffuse.b   =  1.0f * fIntensity;
        lig.Direction.x =  0.2f;
        lig.Direction.y =  0.3f;
        lig.Direction.z = -0.8f;
        EXECUTE(m_pDevice->SetLight(dwIndex, &lig));
        EXECUTE(m_pDevice->LightEnable(dwIndex, TRUE));

        dwIndex++;
    }
    for (nT = 0; nT < m_bmxState.nPointLights; nT++)
    {
        D3DLIGHT8 lig;

        ZeroMemory(&lig, sizeof(lig));
        lig.Type         =  D3DLIGHT_POINT;
        lig.Diffuse.r    =  1.0f * fIntensity;
        lig.Diffuse.g    =  1.0f * fIntensity;
        lig.Diffuse.b    =  1.0f * fIntensity;
        lig.Position.x   =  0.0f;
        lig.Position.y   =  0.0f;
        lig.Position.z   =  5.0f;
        lig.Range        = 9999.0f;
        lig.Attenuation0 = 1.0f;
        EXECUTE(m_pDevice->SetLight(dwIndex, &lig));
        EXECUTE(m_pDevice->LightEnable(dwIndex, TRUE));

        dwIndex++;
    }
    for (nT = 0; nT < m_bmxState.nSpotLights; nT++)
    {
        D3DLIGHT8 lig;

        ZeroMemory(&lig, sizeof(lig));
        lig.Type         =  D3DLIGHT_SPOT;
        lig.Diffuse.r    =  1.0f * fIntensity;
        lig.Diffuse.g    =  1.0f * fIntensity;
        lig.Diffuse.b    =  1.0f * fIntensity;
        lig.Position.x   = -1.0f;
        lig.Position.y   = -5.0f;
        lig.Position.z   =  6.0f;
        lig.Direction.x  =  0.1f;
        lig.Direction.y  =  0.5f;
        lig.Direction.z  = -0.6f;
        lig.Range        = 999.0f;
        lig.Theta        = 0.1f;
        lig.Phi          = 0.5f;
        lig.Falloff      = 1.0f;
        lig.Attenuation0 = 1.0f;
        EXECUTE(m_pDevice->SetLight(dwIndex, &lig));
        EXECUTE(m_pDevice->LightEnable(dwIndex, TRUE));

        dwIndex++;
    }
}


//=========================================================================
// create a texture
//=========================================================================

IDirect3DTexture8 *CBenMark::CreateTexture(DWORD dwWidth, DWORD dwHeight)
{
	HRESULT hr = S_OK;
    IDirect3DTexture8 *pTexture = NULL;

    EXECUTE(D3DXCreateTexture(m_pDevice, dwWidth, dwHeight, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &pTexture));
    if(pTexture)
    {
        D3DLOCKED_RECT lockRect;

        HRESULT hr = pTexture->LockRect(0, &lockRect, NULL, 0);

        D3DSURFACE_DESC desc;
        pTexture->GetLevelDesc(0, &desc);

        void *pBits = (void *)LocalAlloc(0, dwWidth * dwHeight * 16);
        DWORD dwLine = (DWORD)pBits;

        LONG lPitch = lockRect.Pitch;

        for (DWORD y = 0; y < dwHeight; y++)
        {
            DWORD dwAddr = dwLine;
            BOOL  by     = y & 1;
            for (DWORD x = 0; x < dwWidth; x++)
            {
                DWORD c = x + (y >> 5);

                c = (c | (c << 4) | (c << 8) | 0xf000);

                if(by)
                    c ^= 0x0777;

                *(WORD*)dwAddr = (WORD)c;
                dwAddr += 2;
            }
            dwLine += lPitch;
        }
		
        XGSwizzleRect(pBits, 0, NULL, lockRect.pBits, dwWidth, dwHeight, NULL, 2);
		
		LocalFree(pBits);
		
        // Unlock the map so it can be used
        pTexture->UnlockRect(0);
	}

   return pTexture;
}

//=========================================================================
// Renders geometric primitive using Vertex Buffer.
//=========================================================================

void CBenMark::DrawIndexedPrimitive( UINT iIndex, D3DPRIMITIVETYPE dwPrimType, DWORD PrimitiveCount )
{
	HRESULT hr = S_OK;

    EXECUTE(m_pDevice->SetVertexShader(m_dwFVF));
    EXECUTE(m_pDevice->SetStreamSource(0, m_pVBRibbon[iIndex], m_dwFVFSize));
    EXECUTE(m_pDevice->SetIndices(m_pIndexBuffer, 0));

    EXECUTE(m_pDevice->DrawIndexedPrimitive(
        dwPrimType,         // PrimitiveType
        0,                  // minIndex
        m_dwNumVertices,    // NumIndices
        0,                  // startIndex
        PrimitiveCount));   // PrimitiveCount
}

//=========================================================================
// Renders geometric primitive with data specified by a user memory pointer.
//=========================================================================

void CBenMark::DrawIndexedPrimitiveUP( UINT iIndex, D3DPRIMITIVETYPE dwPrimType, DWORD PrimitiveCount )
{
	HRESULT hr = S_OK;

    EXECUTE(m_pDevice->DrawIndexedPrimitiveUP(
        dwPrimType,             // Type
        0,                      // MinVertexIndex
        m_dwNumVertices,        // NumVertexIndices
        PrimitiveCount,         // primCount
        m_pwIndices,            // pIndexData
        D3DFMT_INDEX16,         // IndexDataFormat
        m_pVertices[iIndex],    // pVertexStreamZeroData
        m_dwFVFSize));          // VertexStreamZeroStride
}

//=========================================================================
// Renders geometric primitive using Vertex Buffer.
//=========================================================================

void CBenMark::PrecompiledDraw( UINT iIndex, D3DPRIMITIVETYPE dwPrimType, DWORD PrimitiveCount )
{
	HRESULT hr = S_OK;

    EXECUTE(m_pDevice->SetVertexShader(m_dwFVF));
    EXECUTE(m_pDevice->SetStreamSource(0, m_pVBRibbon[iIndex], m_dwFVFSize));

    m_pDevice->RunPushBuffer(m_pPushBuffer, 0);
}

//=========================================================================
// initialize our VBs
//=========================================================================

void CBenMark::InitVBs()
{
	HRESULT hr = S_OK;
    // clean these guys up if they've already been inited
    delete[] m_pwIndices;
    m_pwIndices = NULL;

    RELEASE(m_pIndexBuffer);
    RELEASE(m_pVBRibbon[0]);
    RELEASE(m_pVBRibbon[1]);

    if (m_bmxState.fPrecompile)
    {
        m_pfnDrawRibbon = CBenMark::PrecompiledDraw;
    }
    else if (m_bmxState.iDrawFunc == DF_DRAWINDEXEDPRIMITIVE)
    {
        m_pfnDrawRibbon = CBenMark::DrawIndexedPrimitive;
    }
    else
    {
        m_pfnDrawRibbon = CBenMark::DrawIndexedPrimitiveUP;
    }

    // create vertex buffer
    m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL;

    if(m_bmxState.cApplyTextures > 1)
        m_dwFVF |= D3DFVF_TEX2;
    else if(m_bmxState.cApplyTextures)
        m_dwFVF |= D3DFVF_TEX1;

    m_dwFVFSize = D3DXGetFVFVertexSize(m_dwFVF);

    m_dwNumVertices = (cdwSubsInLength + 1) * (cdwSubsInWidth + 1);
    UINT Length = m_dwNumVertices * m_dwFVFSize;

    // Default when running is to NOT use D3DVBCAPS_SYSTEMMEMORY
    // Using this flag will slow the app to about half the triangle rate.

    DWORD dwCaps = D3DUSAGE_WRITEONLY;
    D3DPOOL dwPool = D3DPOOL_DEFAULT;

    float *pfVerticesO;
    float *pfVerticesI;

    // Create our vertex buffers if we're using DrawIndexedPrimitive.
    if(m_bmxState.iDrawFunc == DF_DRAWINDEXEDPRIMITIVE)
    {
        EXECUTE(m_pDevice->CreateVertexBuffer(Length, dwCaps, m_dwFVF, dwPool, &m_pVBRibbon[0]));
        EXECUTE(m_pDevice->CreateVertexBuffer(Length, dwCaps, m_dwFVF, dwPool, &m_pVBRibbon[1]));

        EXECUTE(m_pVBRibbon[0]->Lock(0, Length, (BYTE **)&pfVerticesO, 0));
        EXECUTE(m_pVBRibbon[1]->Lock(0, Length, (BYTE **)&pfVerticesI, 0));
    }
    else
    {
        // skip the vertex buffers and just use alloc'd mem
        m_pVertices[0] = new BYTE[Length];
        pfVerticesO = (float *)m_pVertices[0];

        m_pVertices[1] = new BYTE[Length];
        pfVerticesI = (float *)m_pVertices[1];
    }

    // populate with vertices
    for (DWORD y = 0; y <= cdwSubsInLength; y++)
    {
        for (DWORD x = 0; x <= cdwSubsInWidth; x++)
        {
            // compute point
            float angle  = cfRibbonCurl * float(y) / float(cdwSubsInLength);
            float height = cfRibbonWidth * float(x) / float(cdwSubsInWidth)
                         + cfRibbonPitch * float(y) / float(cdwSubsInLength)
                         - 0.5f * cfRibbonPitch;
            float fcosangle = cosf(angle);
            float fsinangle = sinf(angle);

            // start
            D3DXVECTOR3 n(0, fcosangle, fsinangle);
            D3DXVECTOR3 p(height, cfRibbonRadius * fcosangle,
                cfRibbonRadius * fsinangle);

            // copy to vertex buffer
            *(D3DXVECTOR3 *)&pfVerticesO[0] = p;
            *(D3DXVECTOR3 *)&pfVerticesO[3] = n;
            pfVerticesO += 6;

            *(D3DXVECTOR3 *)&pfVerticesI[0] = p;
            *(D3DXVECTOR3 *)&pfVerticesI[3] = -n;
            pfVerticesI += 6;

            if(m_bmxState.cApplyTextures)
            {
                float tv = float(y) / float(cdwSubsInLength);
                float tu = float(x) / float(cdwSubsInWidth);

                *pfVerticesO++ = tu;
                *pfVerticesO++ = tv;

                *pfVerticesI++ = tu;
                *pfVerticesI++ = tv;

                if(m_bmxState.cApplyTextures > 1)
                {
                    // add second set of texture coords
                    *pfVerticesO++ = tu;
                    *pfVerticesO++ = tv;

                    *pfVerticesI++ = tu;
                    *pfVerticesI++ = tv;
                }
            }
        }
    }

    if(m_bmxState.iDrawFunc == DF_DRAWINDEXEDPRIMITIVE)
    {
        EXECUTE(m_pVBRibbon[0]->Unlock());
        EXECUTE(m_pVBRibbon[1]->Unlock());
    }

    // setup indices
    m_dwIndexCount = 1 + cdwSubsInLength * (cdwSubsInWidth * 2 + 1);
    m_dwPrimType = m_bmxState.bUseTriList ? D3DPT_TRIANGLELIST : D3DPT_TRIANGLESTRIP;
    m_dwTriCount = (m_dwIndexCount - 2)  / (m_bmxState.bUseTriList ? 3 : 1);

    m_pwIndices = new WORD[m_dwIndexCount];
    WORD  *pwIndex  = m_pwIndices;
    DWORD dwCurrent = 0;

    pwIndex[0] = 0; pwIndex++;

    for (y = 0; y < cdwSubsInLength; y++)
    {
        if (dwCurrent > 0xffff)
        {
            DbgPrint("Index out of range - reduce geometry complexity", 0);
        }

        pwIndex[0] = WORD(dwCurrent + (cdwSubsInWidth + 1));
        pwIndex++;
        dwCurrent++;

        for (DWORD x = 0; x < cdwSubsInWidth; x++)
        {
            pwIndex[0]  = WORD(dwCurrent);
            pwIndex[1]  = WORD(dwCurrent + (cdwSubsInWidth + 1));
            pwIndex    += 2;
            dwCurrent  ++;
        }

        dwCurrent += (cdwSubsInWidth + 1) - 1;
        y++;

        if (y < cdwSubsInLength)
        {
            pwIndex[0] = WORD(dwCurrent + (cdwSubsInWidth + 1));
            pwIndex++;
            dwCurrent--;

            for (DWORD x = 0; x < cdwSubsInWidth; x++)
            {
                pwIndex[0]  = WORD(dwCurrent);
                pwIndex[1]  = WORD(dwCurrent + (cdwSubsInWidth + 1));
                pwIndex+= 2;
                dwCurrent --;
            }

            dwCurrent += (cdwSubsInWidth + 1) + 1;
        }
    }

    // if we use tri lists, we convert the strip to independent triangles here
    if(m_bmxState.bUseTriList)
    {
        DWORD dwNewIndexCount = (m_dwIndexCount - 2) * 3;
        WORD *pwNewIndices    = new WORD[dwNewIndexCount];

        DWORD a = m_pwIndices[0];
        DWORD b = m_pwIndices[1];
        for (DWORD i = 2,j = 0; i < m_dwIndexCount; i++,j+=3)
        {
            DWORD c = m_pwIndices[i];
            pwNewIndices[j + 0] = (WORD)a;
            pwNewIndices[j + 1] = (WORD)b;
            pwNewIndices[j + 2] = (WORD)c;
            if (i & 1)
                b = c;
            else
                a = c;
        }

        // adopt new
        delete[] m_pwIndices;
        m_dwIndexCount = dwNewIndexCount;
        m_pwIndices    = pwNewIndices;
    }

    // Create our precompiled push buffers
    if (m_bmxState.fPrecompile)
    {
        EXECUTE(m_pDevice->CreatePushBuffer(256*1024, FALSE, &m_pPushBuffer));
        EXECUTE(m_pDevice->BeginPushBuffer(m_pPushBuffer));
        EXECUTE(m_pDevice->DrawIndexedVertices(m_dwPrimType, D3DVERTEXCOUNT(m_dwPrimType, m_dwTriCount), m_pwIndices));
        EXECUTE(m_pDevice->EndPushBuffer());
    }
	// Create our index buffer if we're using DrawIndexedPrimitive.
    else if (m_bmxState.iDrawFunc == DF_DRAWINDEXEDPRIMITIVE)
    {
        EXECUTE(m_pDevice->CreateIndexBuffer(m_dwIndexCount * sizeof(WORD),
            dwCaps, D3DFMT_INDEX16, dwPool, &m_pIndexBuffer));

        WORD *pVertexIndices;
        EXECUTE(m_pIndexBuffer->Lock(0, m_dwIndexCount * sizeof(WORD),
            (BYTE**)&pVertexIndices, 0));

        for(DWORD i = 0; i < m_dwIndexCount; i++)
            pVertexIndices[i] = m_pwIndices[i];

        m_pIndexBuffer->Unlock();

        delete[] m_pwIndices;
        m_pwIndices = NULL;
    }
    
    if(m_bmxState.iDrawFunc == DF_DRAWINDEXEDPRIMITIVE)
    {
        m_pVBRibbon[0]->MoveResourceMemory(D3DMEM_VIDEO);
        m_pVBRibbon[1]->MoveResourceMemory(D3DMEM_VIDEO);
    }
}

//=========================================================================
// Display the various options we we're started with.
//=========================================================================

void CBenMark::ShowInfo(int iY, int cHeight)
{
    WCHAR str[128];
    D3DCOLOR colFore = D3DCOLOR_XRGB(255, 255, 128);
	D3DCOLOR colBack = D3DCOLOR_XRGB(0, 0, 0);
	DWORD dwFlags = 0;//DRAWTEXT_TRANSPARENTBKGND;

	iY += cHeight;
	INT iX = 35;

    _snwprintf(str, sizeof(str), L"Xbox BenMark");

	m_Font.DrawText( m_pBackBuffer, str, iX, iY, dwFlags, colFore, colBack);
//#pragma MESSAGE( "TODO: DisplayText(str, iY, colFore);" )
    iY += cHeight;

    _snwprintf(str, sizeof(str), L"Adapter: %S",
        m_Adapter.AdapterIdentifier.Description);
	m_Font.DrawText( m_pBackBuffer, str, iX, iY, dwFlags, colFore, colBack);
// #pragma MESSAGE( "TODO:    DisplayText(str, iY, colFore); ")
    iY += cHeight;

    DWORD HighPart = m_Adapter.AdapterIdentifier.DriverVersion.HighPart;
    DWORD LowPart = m_Adapter.AdapterIdentifier.DriverVersion.LowPart;
    _snwprintf(str, sizeof(str), L"Driver: %S (%d.%d.%04d.%04d)",
        m_Adapter.AdapterIdentifier.Driver,
        HIWORD(HighPart), LOWORD(HighPart),
        HIWORD(LowPart), LOWORD(LowPart));
	m_Font.DrawText( m_pBackBuffer, str, iX, iY, dwFlags, colFore, colBack);
//#pragma MESSAGE( "TODO:     DisplayText(str, iY, colFore); ")
    iY += cHeight * 2;

    _snwprintf(str, sizeof(str), L"Primitive Type: '%s'  Textures: %d",
        m_bmxState.bUseTriList ? L"trilist" : L"tristrip",
        m_bmxState.cApplyTextures);
	m_Font.DrawText( m_pBackBuffer, str, iX, iY, dwFlags, colFore, colBack);
//#pragma MESSAGE( "TODO:     DisplayText(str, iY, colFore); ")
    iY += cHeight;

    _snwprintf(str, sizeof(str), L"Dirlights: %d  Spotlights: %d  Pointlights: %d",
        m_bmxState.nDirectionalLights,
        m_bmxState.nSpotLights,
        m_bmxState.nPointLights);
	m_Font.DrawText( m_pBackBuffer, str, iX, iY, dwFlags, colFore, colBack);
//#pragma MESSAGE( "TODO:     DisplayText(str, iY, colFore); ")
    iY += cHeight;

    _snwprintf(str, sizeof(str), L"Z-Buffer: %s  Specular Lighting: %s  Local Viewer: %s",
        m_bmxState.bZenable ? L"true" : L"false",
        m_bmxState.bSpecularenable ? L"true" : L"false",
        m_bmxState.bLocalviewer ? L"true" : L"false");
	m_Font.DrawText( m_pBackBuffer, str, iX, iY, dwFlags, colFore, colBack);
//#pragma MESSAGE( "TODO:     DisplayText(str, iY, colFore); ")
    iY += cHeight;

    static const WCHAR *rgszFillMode[] =
        { L"D3DFILL_POINT", L"D3DFILL_WIREFRAME", L"D3DFILL_SOLID" };
    static const WCHAR *rgszDrawFunc[] =
        { L"DrawIndexedPrimitive", L"DrawIndexedPrimitiveUP" };

    DWORD iFillMode = 0;
    switch (m_bmxState.iFillMode)
    {
    case D3DFILL_POINT: iFillMode = 0; break;
    case D3DFILL_WIREFRAME: iFillMode = 1; break;
    case D3DFILL_SOLID: iFillMode = 2; break;
    }

    _snwprintf(str, sizeof(str), L"Rendering: %s  Precompiled: %s  Fillmode: %s",
        rgszDrawFunc[m_bmxState.iDrawFunc], 
        m_bmxState.fPrecompile ? L"true" : L"false",
        rgszFillMode[iFillMode]);
	m_Font.DrawText( m_pBackBuffer, str, iX, iY, dwFlags, colFore, colBack);
//#pragma MESSAGE( "TODO:     DisplayText(str, iY, colFore); ")
    iY += cHeight;

    _snwprintf(str, sizeof(str), L"Triangles: %d  Indices: %d  FVFSize: %d",
        m_dwTriCount,
        m_dwIndexCount,
        m_dwFVFSize);
	m_Font.DrawText( m_pBackBuffer, str, iX, iY, dwFlags, colFore, colBack);
//#pragma MESSAGE( "TODO:     DisplayText(str, iY, colFore); ")
    iY += cHeight;

    _snwprintf(str, sizeof(str), L"Maximum possible background loops per second: %i", (int)m_iMaxDivides);
	m_Font.DrawText( m_pBackBuffer, str, iX, iY, dwFlags, colFore, colBack);
//#pragma MESSAGE( "TODO: 	DisplayText(str, iY, colFore); ")
	iY += cHeight * 2;

	if (m_fdMaxTriPerSec != 0)
	{
		_snwprintf(str, sizeof(str), L"AveTriPerSec(%d): %i  MaxTriPerSec: %i  MinTriPerSec: %i",
			m_dwAveCount,
			(INT) ((m_fdAveTriPerSec / m_dwAveCount) * 1000.0f),
			(INT) (m_fdMaxTriPerSec * 1000.0f),
			(INT) (m_fdMinTriPerSec * 1000.0f));
		m_Font.DrawText( m_pBackBuffer, str, iX, iY, dwFlags, colFore, colBack);
//#pragma MESSAGE( "TODO: 		DisplayText(str, iY, colFore); ")
		iY += cHeight;
	}

}

VOID CBenMark::ShowMenu()
{

	ScenarioMenu* pMenu;
	D3DCOLOR colFore = D3DCOLOR_XRGB( 255, 255, 128 );
	D3DCOLOR colBack = D3DCOLOR_XRGB( 0, 0, 0 );
	DWORD dwFlags = 0;
	INT iX, iY;

	switch ( m_iActiveMenu )
	{
	case XINPUT_GAMEPAD_LEFT_TRIGGER:
		pMenu = &s_LeftTriggerMenu;
		break;
	case XINPUT_GAMEPAD_RIGHT_TRIGGER:
		pMenu = &s_RightTriggerMenu;
		break;
	case XINPUT_GAMEPAD_LEFT_THUMB:
		pMenu = &s_LeftThumbMenu;
		break;
	default:
		return;
	}

	iX = 240;
	iY = 200;
	m_Font.DrawText( m_pBackBuffer, pMenu->m_szMenuName, iX, iY, dwFlags, colFore, colBack);
	for ( UINT i=0; i<pMenu->m_cActions; i++ )
	{
		iY += 12;
		m_Font.DrawText( m_pBackBuffer, pMenu->m_szActions[i], iX, iY, dwFlags, colFore, colBack);
	}
}

//=========================================================================
// Handle input.
//=========================================================================
HRESULT CBenMark::ProcessInput()
{
	HRESULT hr = S_OK;

	DWORD dwDeviceMap;
	DWORD dwSuccess;
	BOOL bDeviceChanges;
	DWORD dwInsertions;
	DWORD dwRemovals;

	bDeviceChanges = XGetDeviceChanges( XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals );
	if ( bDeviceChanges )
	{
		if ( dwInsertions & XDEVICE_PORT0_MASK )
		{
			m_hGamePad1 = XInputOpen( XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0, XDEVICE_NO_SLOT, NULL );
			if ( m_hGamePad1 == NULL )
			{
				DbgPrint( "Error Opening Input\n" );
				return E_FAIL;
			}
		}
		
		if ( dwRemovals & XDEVICE_PORT0_MASK )
		{
			DbgPrint( "Gamepad removed from Port 0\n" );
			return E_FAIL;
		}
	}

	dwDeviceMap = XGetDevices( XDEVICE_TYPE_GAMEPAD );
	if ( !(dwDeviceMap & XDEVICE_PORT0_MASK) ) 
	{
		//DbgPrint( "No Device in Port 0\n" );
		//return E_FAIL;
		return S_OK; // live with it!
	}

	dwSuccess = XInputGetState( m_hGamePad1, &m_InputState );
	if ( dwSuccess != ERROR_SUCCESS )
	{
		DbgPrint( "Error Getting Input State\n" );
		return E_FAIL;
	}

	//------------------------------------------------------------------------------
	//	Fgure out which menu to display
	//------------------------------------------------------------------------------
#if 0
	if ( m_iActiveMenu )
	{
		if ( m_iActiveMenu == XINPUT_GAMEPAD_LEFT_TRIGGER || m_iActiveMenu == XINPUT_GAMEPAD_RIGHT_TRIGGER )
		{
			if ( !m_InputState.Gamepad.bAnalogButtons[m_iActiveMenu])
			{
				m_iActiveMenu = 0;
			}
		}
		else
		{
			if ( !(m_InputState.Gamepad.wButtons & m_iActiveMenu) )
			{
				m_iActiveMenu = 0;
			}
		}
	}
	
	if ( !m_iActiveMenu )
	{
		if (m_InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			m_iActiveMenu = XINPUT_GAMEPAD_LEFT_TRIGGER; 
		} 
		else if (m_InputState.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			m_iActiveMenu = XINPUT_GAMEPAD_RIGHT_TRIGGER;
		} 
		else if (m_InputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
		{
			m_iActiveMenu = XINPUT_GAMEPAD_LEFT_THUMB;
		} 
		else if (m_InputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
		{
			m_iActiveMenu = XINPUT_GAMEPAD_RIGHT_THUMB;
		} 
		else if (m_InputState.Gamepad.wButtons & XINPUT_GAMEPAD_START)
		{
			m_iActiveMenu = XINPUT_GAMEPAD_START;
		} 
		else if (m_InputState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
		{
			m_iActiveMenu = XINPUT_GAMEPAD_BACK;
		}
	}

	if ( !m_iActiveMenu )
	{
		return hr;
	}
	
	//------------------------------------------------------------------------------
	//	Figure out which action key is active
	//------------------------------------------------------------------------------
	
	m_iActiveKey = -1;
	for (unsigned i = 0; i < 6; i++)
	{
		if ( m_InputState.Gamepad.bAnalogButtons[i] > 50 )
		{
			m_iActiveKey = i;
			break;
		}
	}

	switch ( m_iActiveMenu )
	{

	case XINPUT_GAMEPAD_START:
		switch(m_iActiveKey)
		{
			// Display stats.
		case XINPUT_GAMEPAD_A:
			m_bShowStats = !m_bShowStats;
			break;
			
			// Run a full test.
		case XINPUT_GAMEPAD_B:
			m_bRunFullTest = TRUE;
			break;
			
			// Pause the test.
		case XINPUT_GAMEPAD_X:
			// Toggle frame movement
			m_bFrameMoving = !m_bFrameMoving;
			if (m_bFrameMoving)
			{
				TimeStart();
			}
			else
			{
				TimeStop();
			}
			break;
			
			// Single-step.
		case XINPUT_GAMEPAD_Y:
			// Single-step frame movement
			if (FALSE == m_bFrameMoving)
			{
				TimeAdvance();
			}
			
			TimeStop();
			m_bFrameMoving = FALSE;
			m_bSingleStep  = TRUE;
			break;
			
			// Quit?
		case XINPUT_GAMEPAD_BLACK:
			m_bQuit = TRUE;
			break;

		default:
			break;
		}
		break;


	case XINPUT_GAMEPAD_BACK:
		switch(m_iActiveKey)
		{
			// Cycle the screen mode.
		case XINPUT_GAMEPAD_A:
			m_dwCurrentMode = (m_dwCurrentMode + 1) % m_Adapter.dwNumModes;
			
			Change3DEnvironment(); 
			break;
			
			// Cycle to the next antialias mode.
		case XINPUT_GAMEPAD_B:
			UINT i, iNewSampleType;
			
			// If no bits are set,then iNewSampleType will wind
			// up with the same value as m_iSampleType;
			//
			for (i = 1; i <= 32; i++)
			{
				iNewSampleType = (m_iSampleType + i) % 32;
				
				if (m_dwSampleMask & (1 << iNewSampleType ))
				{
					break;
				}
			}
			
			if (m_iSampleType != (D3DMULTISAMPLE_TYPE)iNewSampleType)
			{
				m_iSampleType = (D3DMULTISAMPLE_TYPE)iNewSampleType;						
				
				Change3DEnvironment();
			}
			break;

		default:
			break;
		}
		break;

	default:
		ProcessKeyPress(m_iActiveMenu, m_iActiveKey);
		break;
	}
#endif 0
	return hr;
}


VOID CBenMark::ProcessKeyPress(DWORD iMenu, DWORD iKey)
{
	switch(iMenu)
	{
	case XINPUT_GAMEPAD_LEFT_TRIGGER:
		switch(iKey)
		{
		case 0:
			m_bmxState.bSpecularenable = !m_bmxState.bSpecularenable;
			break;
		case 1:
			m_bmxState.bZenable = !m_bmxState.bZenable;
			break;
		case 2:
			m_bmxState.bLocalviewer = !m_bmxState.bLocalviewer;
			break;

		default:
			return;
		}
		break;

	case XINPUT_GAMEPAD_RIGHT_TRIGGER:
		switch(iKey)
		{
		case 0:
			m_bmxState.nDirectionalLights++;
			break;

		case 1:
			m_bmxState.nSpotLights++;
			break;

		case 2:
			m_bmxState.nPointLights++;
			break;

		case 3:
			if (m_bmxState.nDirectionalLights)
			{
				m_bmxState.nDirectionalLights--;
			}
			else
			{
				return;
			}
			break;

		case 4:
			if (m_bmxState.nSpotLights)
			{
				m_bmxState.nSpotLights--;
			}
			else
			{
				return;
			}
			break;

		case 5:
			if (m_bmxState.nPointLights)
			{
				m_bmxState.nPointLights--;
			}
			else
			{
				return;
			}
			break;
		}

		InitLights();

		return;

	case XINPUT_GAMEPAD_LEFT_THUMB:
		switch(iKey)
		{
		case 0:
			m_bmxState.cApplyTextures = (m_bmxState.cApplyTextures + 1) % 3;
			break;
		case 1:
			m_bmxState.bUseTriList = !m_bmxState.bUseTriList;
			break;

		case 2:
			m_bmxState.iFillMode = D3DFILLMODE((m_bmxState.iFillMode + 1) % D3DFILL_SOLID + 1);
			break;

		case 3:
			m_bmxState.iDrawFunc = (DRAWFUNC)((m_bmxState.iDrawFunc + 1) % DRAWMAX);
			break;

        case 4:
            m_bmxState.fPrecompile = !m_bmxState.fPrecompile;
            break;

		default:
			return;
		}
		break;

	default:
		return;
	}

	Change3DEnvironment(); 
}

//-----------------------------------------------------------------------------
// Create the rendering device and set up the back buffers.
//-----------------------------------------------------------------------------

void CBenMark::Initialize3DEnvironment()
{
	HRESULT hr = S_OK;
	DWORD dwCurrentMode;

	m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);
    if( m_pD3D == NULL )
    {
		DbgPrint( "FATAL: Unable to create Direct3D.");
    }


	// We only support one adapter.
	const unsigned iAdapter = 0;

	if (m_pD3D->GetAdapterCount() != 1)
	{
		DbgPrint( "FATAL: Multiple adapters found, only one adapter is supported." );
	}

    // Get the adapter attributes
    EXECUTE(m_pD3D->GetAdapterIdentifier(iAdapter, 0L, &m_Adapter.AdapterIdentifier));

    // Enumerate display modes
    D3DDISPLAYMODE modes[100];
    D3DFORMAT      formats[20];
    DWORD dwNumFormats      = 0;
    DWORD dwNumModes        = 0;
    DWORD dwNumAdapterModes = m_pD3D->GetAdapterModeCount(iAdapter);

    // Add the current desktop format to list of formats
    D3DDISPLAYMODE DesktopMode;
    EXECUTE(m_pD3D->GetAdapterDisplayMode(iAdapter, &DesktopMode));
    if (DesktopMode.Format != 0)
    {
        formats[dwNumFormats++] = DesktopMode.Format;
    }

    for (UINT iMode = 0; iMode < dwNumAdapterModes; iMode++)
    {
        // Get the display mode attributes
        D3DDISPLAYMODE DisplayMode;

        EXECUTE(m_pD3D->EnumAdapterModes(iAdapter, iMode, &DisplayMode));

        // Filter out unknown modes
        if (DisplayMode.Format == D3DFMT_UNKNOWN)
		{
            continue;
		}

        // Filter out low-resolution modes
        if (DisplayMode.Width  < 640 || DisplayMode.Height < 400) 
		{
            continue;
		}

        // Check if the mode already exists (to filter out refresh rates)
        for (DWORD m = 0; m < dwNumModes; m++)
        {
            if ((modes[m].Width  == DisplayMode.Width ) &&
                (modes[m].Height == DisplayMode.Height) &&
                (modes[m].Format == DisplayMode.Format)) 
			{
                break;
			}
        }

        // If we found a new mode, add it to the list
        if (m == dwNumModes)
        {
            modes[dwNumModes].Width       = DisplayMode.Width;
            modes[dwNumModes].Height      = DisplayMode.Height;
            modes[dwNumModes].Format      = DisplayMode.Format;
            modes[dwNumModes].RefreshRate = 0;
            dwNumModes++;

            // Check if the mode's format already exists
            for (DWORD f = 0; f < dwNumFormats; f++)
            {
                if (DisplayMode.Format == formats[f])
				{
                    break;
				}
            }

            // If the format is new, add it to the list
            if (f == dwNumFormats)
			{
                formats[dwNumFormats++] = DisplayMode.Format;
			}
        }
    }

    // Sort the list of display modes (by format, then width, then height)

    // Get the device attributes
    EXECUTE(m_pD3D->GetDeviceCaps(iAdapter, D3DDEVTYPE_HAL, &m_Adapter.Caps));

    // Call the app's ConfirmDevice() callback to see if the device
    // caps and the enumerated formats meet the app's requirements.
	//
    BOOL  bFormatConfirmed[20];
    DWORD dwBehavior[20];

    for (DWORD f = 0; f < dwNumFormats; f++)
    {
		HRESULT hr;

        bFormatConfirmed[f] = FALSE;

        hr = m_pD3D->CheckDeviceFormat(iAdapter, 
			                           D3DDEVTYPE_HAL, 
                                       formats[f], 
									   D3DUSAGE_RENDERTARGET, 
                                       D3DRTYPE_SURFACE, 
									   formats[f]);

		if (FAILED(hr))
		{
            continue;
		}

#ifdef XBOX
		dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		bFormatConfirmed[f] = TRUE;
#else // !XBOX

        // Confirm the device for HW vertex processing
        if (m_Adapter.Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        {
            dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING;

            if (ConfirmDevice(&m_Adapter.Caps, dwBehavior[f], formats[f]))
			{
                bFormatConfirmed[f] = TRUE;
			}
        }

        // Confirm the device for SW vertex processing
        if (FALSE == bFormatConfirmed[f])
        {
            dwBehavior[f] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

            if (ConfirmDevice(&m_Adapter.Caps, dwBehavior[f], formats[f]))
			{
                bFormatConfirmed[f] = TRUE;
			}
        }
#endif // XBOX
    }


    // Add all enumerated display modes with confirmed formats to the
    // device's list of valid modes
	//
    for (DWORD m = 0; m < dwNumModes; m++)
    {
        for (DWORD f = 0; f < dwNumFormats; f++)
        {
            if (modes[m].Format == formats[f])
            {
                if (bFormatConfirmed[f] == TRUE)
                {
                    // Add this mode to the device's list of valid modes
                    m_Adapter.modes[m_Adapter.dwNumModes].Width      = modes[m].Width;
                    m_Adapter.modes[m_Adapter.dwNumModes].Height     = modes[m].Height;
                    m_Adapter.modes[m_Adapter.dwNumModes].Format     = modes[m].Format;
                    m_Adapter.modes[m_Adapter.dwNumModes].dwBehavior = dwBehavior[f];
                    m_Adapter.dwNumModes++;
                }
            }
        }
    }

    // Select 640x480, 16-bit mode for default
    for (m = 0; m < m_Adapter.dwNumModes; m++)
    {
        if (m_Adapter.modes[m].Width==640 && 
			m_Adapter.modes[m].Height==480 
#ifdef XBOX
			 && m_Adapter.modes[m].Format == D3DFMT_LIN_R5G6B5
#endif
            )
        {
            dwCurrentMode = m;
            break;
        }
    }

	// Generate an error if we didn't find the default mode.
	if (m == m_Adapter.dwNumModes)
	{
		DbgPrint( "FATAL: Unable to obtain 640x480x16 mode." );
	}



    Adapter::Mode *pModeInfo = &m_Adapter.modes[dwCurrentMode];

    // Set up the presentation parameters
    ZeroMemory(&m_Presentation, sizeof(m_Presentation));

    switch(pModeInfo->Format)
    {
#ifdef XBOX
    case D3DFMT_LIN_A8R8G8B8:
    case D3DFMT_LIN_X8R8G8B8:
#endif
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
        m_DepthBufferFormat = D3DFMT_D24S8;
        break;

#ifdef XBOX
    case D3DFMT_LIN_R5G6B5:
    case D3DFMT_LIN_X1R5G5B5:
#endif
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
        m_DepthBufferFormat = D3DFMT_D16;
        break;
    }

    m_Presentation.Windowed               = FALSE;
    m_Presentation.BackBufferCount        = 2;
    m_Presentation.MultiSampleType        = m_iSampleType;
    m_Presentation.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    m_Presentation.EnableAutoDepthStencil = m_bUseDepthBuffer;
    m_Presentation.AutoDepthStencilFormat = m_DepthBufferFormat;
    m_Presentation.hDeviceWindow          = NULL;
    m_Presentation.BackBufferWidth        = pModeInfo->Width;
    m_Presentation.BackBufferHeight       = pModeInfo->Height;
    m_Presentation.BackBufferFormat       = pModeInfo->Format;
    m_Presentation.Flags                  = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    m_Presentation.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    // Create the device
    EXECUTE(m_pD3D->CreateDevice(0, 
		                     D3DDEVTYPE_HAL, 
                             NULL, 
						     pModeInfo->dwBehavior, 
							 &m_Presentation, 
                             &m_pDevice));

    // Store device Caps
//    EXECUTE(m_pDevice->GetDeviceCaps(&m_Caps));

	// Figure out the FSAA modes that are supported by the device.
	m_dwSampleMask = 0;
	m_iSampleType = (D3DMULTISAMPLE_TYPE)0;

	for (unsigned i = 2; i < 32; i++)
	{
		HRESULT hr = m_pD3D->CheckDeviceMultiSampleType(0, 
			                                            D3DDEVTYPE_HAL, 
												        pModeInfo->Format, 
														FALSE, 
														(D3DMULTISAMPLE_TYPE)i);

		if (SUCCEEDED(hr))
		{
            m_dwSampleMask |= 1 << i;
		}
	}

    // Store render target surface descriptor
    EXECUTE(m_pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer));
    m_pBackBuffer->GetDesc(&m_BackBufferDesc);

    // Initialize the app's device-dependant objects
	InitDeviceObjects();
    RestoreDeviceObjects();

//	m_bActive = TRUE;
}

//-----------------------------------------------------------------------------
// Handles driver, device, and/or mode changes for the app.
//-----------------------------------------------------------------------------

VOID CBenMark::Change3DEnvironment()
{
    // Release all scene objects that will be re-created for the new device
//	InvalidateDeviceObjects();
//    DeleteDeviceObjects();

    if (m_pBackBuffer)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = NULL;
	}

    // Release display objects, so a new device can be created
	//
	// UNDONE andrewso: Make sure this is a safe assumption.
	//
    if (m_pDevice->Release() > 0)
    {
		DbgPrint( "FATAL: Device still has an outstanding reference." );
    }

	// Just to be safe.
	m_pDevice = NULL;

    // Inform the display class of the driver change. It will internally
    // re-create valid surfaces, a d3ddevice, etc.
	//
    Initialize3DEnvironment();

    // If the app is paused, trigger the rendering of the current frame
    if (FALSE == m_bFrameMoving)
    {
        m_bSingleStep = TRUE;
		
		TimeStart();
		TimeStop();
    }
}


VOID CBenMark::TimeStart()
{
	m_fBaseTime += GetTime() - m_fStopTime;
}

VOID CBenMark::TimeStop()
{
	m_fStopTime = GetTime();
}

VOID CBenMark::TimeAdvance()
{
	m_fBaseTime += GetTime() - ( m_fStopTime + 0.1f );
}

FLOAT CBenMark::GetTime() 
{
	LARGE_INTEGER qwTime;
	LARGE_INTEGER qwTicksPerSec;
	
	QueryPerformanceCounter( &qwTime );
	QueryPerformanceFrequency(&qwTicksPerSec);

	return ((float)qwTime.QuadPart) * 1.0f / (FLOAT)qwTicksPerSec.QuadPart;
}

#ifdef USE_SOUND
#include "dsound.h"

///
/// Set up necessary types
/// 

typedef short* LPSHORT;

///
/// Setup the frequency, and define PI
///

static const double FREQ = 440;
static const double PI = 3.1415926535;

///
/// Scaling factors
///
/// ISCALE is used to convert an interger [0, 44099] to a double [0, 2PI]
/// DATASCALE is used to conver a double [-1.0, 1.0] to an integer [-32767, 32767]

static const double ISCALE = 2.0 * PI / 44099.0;
static const double DATASCALE = 32767.0 / 1.0;

///
/// the main sine player code
///

HRESULT PlaySineWave( void )
{

	///
	/// create a dsound buffer and a data buffer
	///

    HRESULT hr = S_OK;
	LPDIRECTSOUNDBUFFER pDSBuffer = NULL;
	LPSHORT pData = NULL;

	///
	/// place to hold the number of bytes allocated by dsound
	///

	DWORD dwBytes = 0;

	///
	/// allocate space for calculate argument to sin, return value from sine, and actual integer value
	///

	double dArg = 0.0;
	double dSinVal = 0.0;
	SHORT sVal = 0;
   
	///
	/// allocate the dsound buffer description and the wave format
	///

	DSBUFFERDESC dsbd;
	WAVEFORMATEX wfx;

	///
	/// It's always safe to zero out stack allocated structs
	///

	ZeroMemory( &dsbd, sizeof( DSBUFFERDESC ) );
	ZeroMemory( &wfx, sizeof( DSBUFFERDESC ) );

	///
	/// setup the waveformat description to be PCM, 44100hz, 16bit, mono
    ///

	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 44100;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = 2;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	///
	/// setup the dsound buffer description with the waveformatex
	///

	dsbd.dwBufferBytes = wfx.nAvgBytesPerSec;
	dsbd.dwFlags = 0;
	dsbd.dwSize = sizeof( DSBUFFERDESC );
	dsbd.lpwfxFormat = &wfx;
    //dsbd.dwMixBins = DSMIXBIN_FXSEND_0;

	/// 
	/// Create the dsound buffer
	///

	hr = DirectSoundCreateBuffer( &dsbd, &pDSBuffer );

	///
	/// start the dsound buffer playing in a loop mode
	///

	if ( SUCCEEDED( hr ) )
	{
		hr = pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING );
	}

	///
	/// continue forever (or until an error occurs)
	///

	if ( SUCCEEDED( hr ) )
	{

		///
		/// get a buffer from dsound using lock
		///

		hr = pDSBuffer->Lock( 0, dsbd.dwBufferBytes, (LPVOID*) &pData, &dwBytes, NULL, NULL, 0 );

		///
		/// go through the buffer 2 bytes (1 short) at a time
		///

		for( DWORD i = 0; SUCCEEDED( hr ) && i < dsbd.dwBufferBytes / 2; i++ )
		{	
			///
			/// calculate the sin value
			///

			dArg = (double) i * ISCALE;
			dSinVal = sin( FREQ * dArg );
			sVal = (SHORT) ( dSinVal * DATASCALE );

			///
			/// copy the value into the dsound buffer
			///

			pData[i] = sVal;
		}

    }
    
    //
    // leave with the buffer still playing..
    //

    return hr;
}

#endif // USE_SOUND