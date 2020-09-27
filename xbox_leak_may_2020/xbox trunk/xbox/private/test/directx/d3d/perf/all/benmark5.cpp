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

#ifdef XBOX
#include <xtl.h>
#include <xgraphics.h>
#else
#include <windows.h>
#include <D3DX8.h>
#endif
#include <stdio.h>
#include <math.h>
#include "D3DApp.h"
#include "DXUtil.h"

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

extern "C" ULONG DebugPrint(PCHAR Format, ...);

const DWORD cdwRibbonCount  = 25;

const float cfRibbonCurl    = 60.0f * D3DX_PI;
const float cfRibbonPitch   = 3.25f;
const float cfRibbonWidth   = 0.04f;
const float cfRibbonRadius  = 0.05f;

const DWORD cdwSubsInLength = 1200;
const DWORD cdwSubsInWidth  = 5;

static ScenarioMenu s_Button7Menu = 
{
	MT_BUTTON7,
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

static ScenarioMenu s_Button8Menu = 
{
	MT_BUTTON8,
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

static ScenarioMenu s_Button9Menu = 
{
	MT_BUTTON9,
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

/*
 * Application class. The base class provides just about all the
 * functionality we want, so we're just supplying stubs to interface with
 * the non-C++ functions of the app.
 */

class BenMark : public CD3DApplication
{

public:

	/*
	 * Support structures.
	 */

	// The different options for the test.
	struct BMXSTATE
	{
		UINT        cApplyTextures;
		bool		bUseTriList;
		bool        bZenable;
		bool        bSpecularenable;
		bool        bLocalviewer;

		int         nDirectionalLights;
		int         nSpotLights;
		int         nPointLights;

		int         iDrawFunc;
        bool        fPrecompile;
		D3DFILLMODE iFillMode; 
	};

	/*
	 * Geometry Constants
	 */

    enum DRAWFUNC
    {
        DF_DRAWINDEXEDPRIMITIVE,    // use vertexbuffer drawprim
        DF_DRAWINDEXEDPRIMITIVEUP,  // use user ptr drawprim
        DRAWMAX
    };

	void DrawIndexedPrimitive(UINT iIndex, D3DPRIMITIVETYPE dwPrimType, DWORD PrimitiveCount);
	void DrawIndexedPrimitiveUP(UINT iIndex, D3DPRIMITIVETYPE dwPrimType, DWORD PrimitiveCount);
	void PrecompiledDraw(UINT iIndex, D3DPRIMITIVETYPE dwPrimType, DWORD PrimitiveCount);

    typedef void (BenMark::*PFNDRAWRIBBON)(
        UINT                iIndex,
        D3DPRIMITIVETYPE    Type,
        DWORD               PrimitiveCount);

    PFNDRAWRIBBON m_pfnDrawRibbon;

	/*
	 * Our materials
	 */

	D3DMATERIAL8 m_matIn;
	D3DMATERIAL8 m_matOut[cdwRibbonCount];

	/*
	 * Implementation
	 */

	void InitMaterials(D3DMATERIAL8 *pmatIn, D3DMATERIAL8 *pmatOut);
	void InitLights();
	IDirect3DTexture8 *CreateTexture(DWORD dwWidth, DWORD dwHeight);
	void InitIndices();
	void InitVBs();

    /* 
	 * Data members
	 */

	BMXSTATE				m_bmxState;
    WORD                   *m_pwIndices;    // user ptr to indices
    IDirect3DIndexBuffer8  *m_pIndexBuffer; // dx8 index buffer

    IDirect3DVertexBuffer8 *m_pVBRibbon[2]; // vb of verts
    BYTE                   *m_pVertices[2]; // user ptr to verts

#ifdef XBOX
    IDirect3DPushBuffer8   *m_pPushBuffer;  // precompiled push buffer
#endif

    IDirect3DTexture8      *m_pTexture1;    // texture1
    IDirect3DTexture8      *m_pTexture2;    // texture2
    IDirect3DSurface8      *m_pBackSurf;    // back surface ptr

    DWORD                   m_dwFVF;        // our current FVF
    DWORD                   m_dwFVFSize;    // FVF size

    DWORD                   m_dwIndexCount; // # indices
    DWORD                   m_dwNumVertices;// # verts

	DWORD					m_dwTotalTimes; // # of times the ribbons have been drawn

    DWORD                   m_dwTriCount;
    _D3DPRIMITIVETYPE       m_dwPrimType;
    float                   m_fDelta;

	int						m_nNumLights;	// # of allocated lights.

    LONG                    m_lDivides;
    int                     m_iMaxDivides;
    
	// long-term stats.
	double					m_fdAveTriPerSec;
	double					m_fdMaxTriPerSec;
	double					m_fdMinTriPerSec;

	DWORD					m_dwAveCount;

    // Background thread management.
    HANDLE                  m_hThread;
    bool                    m_fTerminateThread;

	/*
	 * Overridden members of the base class
	 */

    void InitDeviceObjects();
    void RestoreDeviceObjects();
    void DeleteDeviceObjects();
    void Render();
	void UpdateStats(float fElapsedTime);
	void ShowInfo(int iY, int cHeight);
	unsigned GetMenus(ScenarioMenu **rgMenus);
	void ProcessKeyPress(DWORD iMenu, DWORD iKey);

	/*
	 * Constructor
	 */

	BenMark();
};

//=========================================================================
// Entrypoint into the test.
//=========================================================================

#ifdef XBOX
void __cdecl main()
#else
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
#endif
{
#ifdef USE_SOUND
    PlaySineWave();
#endif // USE_SOUND

    BenMark app;

#ifdef XBOX
    app.Create(NULL);
#else
    app.Create(hInst);
#endif

    app.Run();

#ifndef XBOX
	return 0;
#endif  // ! XBOX
}

//=========================================================================
// BenMark constructor
//=========================================================================

BenMark::BenMark()
{
    m_pwIndices = NULL;
    m_pIndexBuffer = NULL;

    m_pVBRibbon[0] = NULL;
    m_pVBRibbon[1] = NULL;

    m_pVertices[0] = NULL;
    m_pVertices[1] = NULL;

#ifdef XBOX
    m_pPushBuffer  = NULL;
#endif

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

	ZeroMemory(&m_bmxState, sizeof(m_bmxState));

    m_bmxState.iDrawFunc   = DF_DRAWINDEXEDPRIMITIVE;
    m_bmxState.iFillMode   = D3DFILL_SOLID;

    m_bmxState.fPrecompile = false;

    m_bmxState.bZenable = true;

    m_bmxState.nDirectionalLights   = 1;
    m_bmxState.nSpotLights          = 0;
    m_bmxState.nPointLights         = 0;

    m_bUseDepthBuffer = TRUE;
    m_DepthBufferFormat = D3DFMT_D16;
}

//=========================================================================
// This creates all device-dependant managed objects, such as managed
// textures and managed vertex buffers.
//=========================================================================

void BenMark::InitDeviceObjects()
{
    InitMaterials(&m_matIn, m_matOut);

    m_pTexture1 = CreateTexture(8, 256);
    m_pTexture2 = CreateTexture(8, 256);
};

//=========================================================================
// Background thread loop.
//=========================================================================

DWORD WINAPI ThreadProc
(
  LPVOID lpParameter   // thread data
)
{
    BenMark *pMark = (BenMark *)lpParameter;

    while (!pMark->m_fTerminateThread)
    {
        for (volatile double i = 1000000000.0f; i != 0; i = i / 10.0f)
            ;

        InterlockedIncrement(&pMark->m_lDivides);
    }

    return 0;
}

//=========================================================================
// Restore device-memory objects and state after a device is created or 
// resized.
//
// This will manually get called any time an option changes so any
// option-specific initialization should go here.
//=========================================================================

void BenMark::RestoreDeviceObjects()
{
    // initalize our lights
    InitLights();

    // create our vertex buffers and indices
    InitVBs();

	// Set up our transformations.
    D3DXMATRIX mux;
    D3DXMatrixIdentity(&mux);
    D3DXMatrixTranslation(&mux, 0.0f, 0.0f, -10.0f);

    CheckHR(m_pDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&mux));

    D3DXMatrixPerspectiveFovRH(&mux,
                              D3DX_PI * 30.0f / 360.0f,
//                              (float)m_BackBufferDesc.Height / m_BackBufferDesc.Width,
                              (float)m_BackBufferDesc.Width / m_BackBufferDesc.Height,
                              1.0f,
                              19.0f);

    CheckHR(m_pDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&mux));

    // init some render states
    CheckHR(m_pDevice->SetRenderState(D3DRS_ZENABLE, m_bmxState.bZenable));
    CheckHR(m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, m_bmxState.bSpecularenable));
    CheckHR(m_pDevice->SetRenderState(D3DRS_LOCALVIEWER, m_bmxState.bLocalviewer));
	CheckHR(m_pDevice->SetRenderState(D3DRS_FILLMODE, m_bmxState.iFillMode));
    CheckHR(m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE));

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
// Called when the app is exitting, or the device is being changed,
// this function deletes any device dependant objects.
//=========================================================================

void BenMark::DeleteDeviceObjects()
{
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

    SAFE_RELEASE(m_pIndexBuffer);

    SAFE_RELEASE(m_pVBRibbon[0]);
    SAFE_RELEASE(m_pVBRibbon[1]);

    SAFE_RELEASE(m_pTexture1);
    SAFE_RELEASE(m_pTexture2);

#ifdef XBOX
    SAFE_RELEASE(m_pPushBuffer);
#endif
}

//=========================================================================
// Draw Ben's ribbons
//=========================================================================

void BenMark::Render()
{
    //float fRotation2 = 2.0f * D3DX_PI / 2047.0f * float(dwNow & 2047);
    float fRotation2 = 2.0f * D3DX_PI / 2047.0f * m_fTime * 1023;

    // clear -- worth moving this calculation?
    DWORD dwFlags = (m_Presentation.AutoDepthStencilFormat != D3DFMT_UNKNOWN) ?
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER : D3DCLEAR_TARGET;

    CheckHR(m_pDevice->Clear(0, NULL, dwFlags, 0x00000000, 1.0f, 0L));

    // begin scene
    CheckHR(m_pDevice->BeginScene());

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
        CheckHR(m_pDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&matWorldT));

        // draw outside
        if(m_bmxState.cApplyTextures)
        {
            CheckHR(m_pDevice->SetTexture          (0, m_pTexture1));

            CheckHR(m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1,D3DTA_TEXTURE));
            CheckHR(m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_MODULATE));
            CheckHR(m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2,D3DTA_DIFFUSE));

            CheckHR(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,D3DTA_TEXTURE));
            CheckHR(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_MODULATE));
            CheckHR(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2,D3DTA_DIFFUSE));

            if(m_bmxState.cApplyTextures > 1)
            {
                CheckHR(m_pDevice->SetTexture          (1, m_pTexture2));

                CheckHR(m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG1,D3DTA_TEXTURE));
                CheckHR(m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,  D3DTOP_ADD));
                CheckHR(m_pDevice->SetTextureStageState(1, D3DTSS_COLORARG2,D3DTA_CURRENT));

                CheckHR(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,D3DTA_TEXTURE));
                CheckHR(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_ADD));
                CheckHR(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2,D3DTA_CURRENT));
            }
            else
            {
                CheckHR(m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,  D3DTOP_DISABLE));
                CheckHR(m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,  D3DTOP_DISABLE));
            }
        }

        CheckHR(m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));
        CheckHR(m_pDevice->SetMaterial(&m_matOut[i]));

        // draw outside ribbon
        (this->*m_pfnDrawRibbon)(0, m_dwPrimType, m_dwTriCount);

        // draw inside
        if(m_bmxState.cApplyTextures)
        {
            CheckHR(m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1,D3DTA_DIFFUSE));
            CheckHR(m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP,  D3DTOP_SELECTARG1));
            CheckHR(m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP,  D3DTOP_DISABLE));

            CheckHR(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,D3DTA_DIFFUSE));
            CheckHR(m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_SELECTARG1));
            CheckHR(m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,  D3DTOP_DISABLE));
        }

        CheckHR(m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));
        CheckHR(m_pDevice->SetMaterial(&m_matIn));

        // draw inside ribbon
        (this->*m_pfnDrawRibbon)(1, m_dwPrimType, m_dwTriCount);

        // next
        m_dwTotalTimes += 2;
    }

    // end
    CheckHR(m_pDevice->EndScene());
}

//=========================================================================
// Called once a second or after a test run to update the 
// test status string.
//=========================================================================

void BenMark::UpdateStats(float fElapsedTime)
{
    LONG Divides = InterlockedExchange(&m_lDivides, 0);

    // next experiment?
    double rcp = 1e-6 / (double)fElapsedTime;
    double tps = double(m_dwTotalTimes * m_dwTriCount) * rcp;
    double sps = double(m_dwTotalTimes * m_dwIndexCount) * rcp;
    double ips = double(m_dwTotalTimes * m_dwIndexCount * 2) * rcp;
    double vps = double(m_dwTotalTimes * m_dwIndexCount * m_dwFVFSize) * rcp;
    double dps = double(Divides) / (double)fElapsedTime;

    swprintf(m_szTestStats, L"%i KTri/s, %i KVerts/s, %i KB/s index traffic, "
        L"%iKB/s [effective] vertex traffic, %i background thread loops/second", 
             (INT) (tps * 1000.0f), (INT) (sps * 1000.0f), 
             (INT) (ips * 1000.0f), (INT) (vps * 1000.0f),
             (INT) (dps));

    static DWORD count = 0;

    if (count++ == 0)
    {
        count = 0;
    
        DebugPrint("%i KTri/s, %i KVerts/s, %i KB/s index traffic, "
              "%iKB/s [effective] vertex traffic, %i K/s background thread loops\n", 
             (INT) (tps * 1000.0f), (INT) (sps * 1000.0f), 
             (INT) (ips * 1000.0f), (INT) (vps * 1000.0f),
             (INT) (dps * 1000.0f));
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

void BenMark::InitMaterials(D3DMATERIAL8 *pmatIn, D3DMATERIAL8 *pmatOut)
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

void BenMark::InitLights()
{
    // setup lights
    float fIntensity = 1.0f /
        (m_bmxState.nDirectionalLights +
         m_bmxState.nPointLights +
         m_bmxState.nSpotLights);

    // disable all our existing lights
    while(m_nNumLights--)
        CheckHR(m_pDevice->LightEnable(m_nNumLights, FALSE));

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
        CheckHR(m_pDevice->SetLight(dwIndex, &lig));
        CheckHR(m_pDevice->LightEnable(dwIndex, TRUE));

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
        CheckHR(m_pDevice->SetLight(dwIndex, &lig));
        CheckHR(m_pDevice->LightEnable(dwIndex, TRUE));

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
        CheckHR(m_pDevice->SetLight(dwIndex, &lig));
        CheckHR(m_pDevice->LightEnable(dwIndex, TRUE));

        dwIndex++;
    }
}


//=========================================================================
// create a texture
//=========================================================================

IDirect3DTexture8 *BenMark::CreateTexture(DWORD dwWidth, DWORD dwHeight)
{
    IDirect3DTexture8 *pTexture = NULL;

    CheckHR(D3DXCreateTexture(m_pDevice, dwWidth, dwHeight, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &pTexture));
    if(pTexture)
    {
        D3DLOCKED_RECT lockRect;

        HRESULT hr = pTexture->LockRect(0, &lockRect, NULL, 0);

        D3DSURFACE_DESC desc;
        pTexture->GetLevelDesc(0, &desc);

#ifdef XBOX
        void *pBits = (void *)LocalAlloc(0, dwWidth * dwHeight * 16);
        DWORD dwLine = (DWORD)pBits;
#else
        DWORD dwLine = (DWORD)lockRect.pBits;
#endif

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

#ifdef XBOX
        XGSwizzleRect(pBits, 
                      0,
                      NULL,
                      lockRect.pBits,
                      dwWidth,
                      dwHeight,
                      NULL, 
                      2);

         LocalFree(pBits);
#endif

        // Unlock the map so it can be used
        pTexture->UnlockRect(0);
   }

   return pTexture;
}

//=========================================================================
// Renders geometric primitive using Vertex Buffer.
//=========================================================================

void BenMark::DrawIndexedPrimitive
(
	UINT iIndex,
    D3DPRIMITIVETYPE dwPrimType, 
	DWORD PrimitiveCount
)
{
    CheckHR(m_pDevice->SetVertexShader(m_dwFVF));
    CheckHR(m_pDevice->SetStreamSource(0, m_pVBRibbon[iIndex], m_dwFVFSize));
    CheckHR(m_pDevice->SetIndices(m_pIndexBuffer, 0));

    CheckHR(m_pDevice->DrawIndexedPrimitive(
        dwPrimType,         // PrimitiveType
        0,                  // minIndex
        m_dwNumVertices,    // NumIndices
        0,                  // startIndex
        PrimitiveCount));   // PrimitiveCount
}

//=========================================================================
// Renders geometric primitive with data specified by a user memory pointer.
//=========================================================================

void BenMark::DrawIndexedPrimitiveUP
(
	UINT iIndex,
    D3DPRIMITIVETYPE dwPrimType, 
	DWORD PrimitiveCount
)
{
    CheckHR(m_pDevice->DrawIndexedPrimitiveUP(
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

void BenMark::PrecompiledDraw
(
	UINT iIndex,
    D3DPRIMITIVETYPE dwPrimType, 
	DWORD PrimitiveCount
)
{
    CheckHR(m_pDevice->SetVertexShader(m_dwFVF));
    CheckHR(m_pDevice->SetStreamSource(0, m_pVBRibbon[iIndex], m_dwFVFSize));

#ifdef XBOX
    m_pDevice->RunPushBuffer(m_pPushBuffer, 0);
#endif
}

//=========================================================================
// initialize our VBs
//=========================================================================

void BenMark::InitVBs()
{
    // clean these guys up if they've already been inited
    delete[] m_pwIndices;
    m_pwIndices = NULL;

    SAFE_RELEASE(m_pIndexBuffer);
    SAFE_RELEASE(m_pVBRibbon[0]);
    SAFE_RELEASE(m_pVBRibbon[1]);

    if (m_bmxState.fPrecompile)
    {
        m_pfnDrawRibbon = BenMark::PrecompiledDraw;
    }
    else if (m_bmxState.iDrawFunc == DF_DRAWINDEXEDPRIMITIVE)
    {
        m_pfnDrawRibbon = BenMark::DrawIndexedPrimitive;
    }
    else
    {
        m_pfnDrawRibbon = BenMark::DrawIndexedPrimitiveUP;
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
#ifdef XBOX
    DWORD dwCaps = D3DUSAGE_WRITEONLY;
#else
    DWORD dwCaps = D3DUSAGE_WRITEONLY | D3DUSAGE_DONOTCLIP | D3DUSAGE_DYNAMIC;
#endif
    D3DPOOL dwPool = D3DPOOL_DEFAULT;

    float *pfVerticesO;
    float *pfVerticesI;

    // Create our vertex buffers if we're using DrawIndexedPrimitive.
    if(m_bmxState.iDrawFunc == DF_DRAWINDEXEDPRIMITIVE)
    {
        CheckHR(m_pDevice->CreateVertexBuffer(Length, dwCaps, m_dwFVF, dwPool, &m_pVBRibbon[0]));
        CheckHR(m_pDevice->CreateVertexBuffer(Length, dwCaps, m_dwFVF, dwPool, &m_pVBRibbon[1]));

        CheckHR(m_pVBRibbon[0]->Lock(0, Length, (BYTE **)&pfVerticesO, 0));
        CheckHR(m_pVBRibbon[1]->Lock(0, Length, (BYTE **)&pfVerticesI, 0));
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
        CheckHR(m_pVBRibbon[0]->Unlock());
        CheckHR(m_pVBRibbon[1]->Unlock());
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
            DisplayError(L"Index out of range - reduce geometry complexity", 0);
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

#ifdef XBOX

    // Create our precompiled push buffers
    if (m_bmxState.fPrecompile)
    {
        CheckHR(m_pDevice->CreatePushBuffer(256*1024, FALSE, &m_pPushBuffer));
        CheckHR(m_pDevice->BeginPushBuffer(m_pPushBuffer));
        CheckHR(m_pDevice->DrawIndexedVertices(m_dwPrimType, D3DVERTEXCOUNT(m_dwPrimType, m_dwTriCount), m_pwIndices));
        CheckHR(m_pDevice->EndPushBuffer());
    }
    else 
     
#endif 

    // Create our index buffer if we're using DrawIndexedPrimitive.
    if (m_bmxState.iDrawFunc == DF_DRAWINDEXEDPRIMITIVE)
    {
        CheckHR(m_pDevice->CreateIndexBuffer(m_dwIndexCount * sizeof(WORD),
            dwCaps, D3DFMT_INDEX16, dwPool, &m_pIndexBuffer));

        WORD *pVertexIndices;
        CheckHR(m_pIndexBuffer->Lock(0, m_dwIndexCount * sizeof(WORD),
            (BYTE**)&pVertexIndices, 0));

        for(DWORD i = 0; i < m_dwIndexCount; i++)
            pVertexIndices[i] = m_pwIndices[i];

        m_pIndexBuffer->Unlock();

        delete[] m_pwIndices;
        m_pwIndices = NULL;
    }

#ifdef XBOX
    
    if(m_bmxState.iDrawFunc == DF_DRAWINDEXEDPRIMITIVE)
    {
        m_pVBRibbon[0]->MoveResourceMemory(D3DMEM_VIDEO);
        m_pVBRibbon[1]->MoveResourceMemory(D3DMEM_VIDEO);
    }

#endif XBOX
}

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
// Display the various options we we're started with.
//=========================================================================

void BenMark::ShowInfo(int iY, int cHeight)
{
    WCHAR str[128];
    D3DCOLOR colFore = D3DCOLOR_XRGB(255, 255, 0);

	iY += cHeight;

    _snwprintf(str, sizeof(str), L"Xbox BenMark");

    DisplayText(str, iY, colFore);
    iY += cHeight;

    _snwprintf(str, sizeof(str), L"Adapter: %S",
        m_Adapter.AdapterIdentifier.Description);
    DisplayText(str, iY, colFore);
    iY += cHeight;

    DWORD HighPart = m_Adapter.AdapterIdentifier.DriverVersion.HighPart;
    DWORD LowPart = m_Adapter.AdapterIdentifier.DriverVersion.LowPart;
    _snwprintf(str, sizeof(str), L"Driver: %S (%d.%d.%04d.%04d)",
        m_Adapter.AdapterIdentifier.Driver,
        HIWORD(HighPart), LOWORD(HighPart),
        HIWORD(LowPart), LOWORD(LowPart));
    DisplayText(str, iY, colFore);
    iY += cHeight * 2;

    _snwprintf(str, sizeof(str), L"Primitive Type: '%s'  Textures: %d",
        m_bmxState.bUseTriList ? L"trilist" : L"tristrip",
        m_bmxState.cApplyTextures);
    DisplayText(str, iY, colFore);
    iY += cHeight;

    _snwprintf(str, sizeof(str), L"Dirlights: %d  Spotlights: %d  Pointlights: %d",
        m_bmxState.nDirectionalLights,
        m_bmxState.nSpotLights,
        m_bmxState.nPointLights);
    DisplayText(str, iY, colFore);
    iY += cHeight;

    _snwprintf(str, sizeof(str), L"Z-Buffer: %s  Specular Lighting: %s  Local Viewer: %s",
        m_bmxState.bZenable ? L"true" : L"false",
        m_bmxState.bSpecularenable ? L"true" : L"false",
        m_bmxState.bLocalviewer ? L"true" : L"false");
    DisplayText(str, iY, colFore);
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
    DisplayText(str, iY, colFore);
    iY += cHeight;

    _snwprintf(str, sizeof(str), L"Triangles: %d  Indices: %d  FVFSize: %d",
        m_dwTriCount,
        m_dwIndexCount,
        m_dwFVFSize);
    DisplayText(str, iY, colFore);
    iY += cHeight;

    _snwprintf(str, sizeof(str), L"Maximum possible background loops per second: %i", (int)m_iMaxDivides);
	DisplayText(str, iY, colFore);
	iY += cHeight * 2;

	if (m_fdMaxTriPerSec != 0)
	{
		_snwprintf(str, sizeof(str), L"AveTriPerSec(%d): %i  MaxTriPerSec: %i  MinTriPerSec: %i",
			m_dwAveCount,
			(INT) ((m_fdAveTriPerSec / m_dwAveCount) * 1000.0f),
			(INT) (m_fdMaxTriPerSec * 1000.0f),
			(INT) (m_fdMinTriPerSec * 1000.0f));
		DisplayText(str, iY, colFore);
		iY += cHeight;
	}

}

//=========================================================================
// Set up the menus.
//=========================================================================

unsigned BenMark::GetMenus(ScenarioMenu **rgMenus)
{
	rgMenus[0] = &s_Button7Menu;
	rgMenus[1] = &s_Button8Menu;
	rgMenus[2] = &s_Button9Menu;

	return 3;
}

//=========================================================================
// Handle keyboard input.
//=========================================================================

void BenMark::ProcessKeyPress(DWORD iMenu, DWORD iKey)
{
	switch(iMenu)
	{
	case MT_BUTTON7:
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

	case MT_BUTTON8:
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

	case MT_BUTTON9:
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

#ifdef XBOX
        case 4:
            m_bmxState.fPrecompile = !m_bmxState.fPrecompile;
            break;
#endif 

		default:
			return;
		}
		break;

	default:
		return;
	}

	Change3DEnvironment();
}
