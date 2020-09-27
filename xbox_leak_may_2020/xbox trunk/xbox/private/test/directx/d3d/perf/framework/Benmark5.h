//-----------------------------------------------------------------------------
// FILE: BENMARK5.H
//
// Desc: benmark5 header
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _BENMARK_5_H_
#define _BENMARK_5_H_

#include "Test.h"
#include "BitFont.h"
//#include "Inifile.h"

#define BENMARK_INI_PATH	"D:\\benmark5.ini"

const DWORD cdwRibbonCount  = 25;

const FLOAT cfRibbonCurl    = 60.0f * D3DX_PI;
const FLOAT cfRibbonPitch   = 3.25f;
const FLOAT cfRibbonWidth   = 0.04f;
const FLOAT cfRibbonRadius  = 0.05f;

const DWORD cdwSubsInLength = 1200;
const DWORD cdwSubsInWidth  = 5;


// Ripped from dxutil.h


struct ScenarioMenu
{
	BYTE m_MenuType;

	const WCHAR *m_szMenuName;

	const WCHAR *m_szActions[6]; // for A, B, C, D, E, F
	unsigned m_cActions;        // Number of filled actions.
};

#define BENMARK_NUM_MENUS 3

//-----------------------------------------------------------------------------
// Structure for holding information about an adapter, and its display modes.
//-----------------------------------------------------------------------------

struct Adapter
{
	struct Mode
	{
		// Mode data
		DWORD      Width;
		DWORD      Height;
		D3DFORMAT  Format;
		DWORD      dwBehavior;
	};
	
	// Adapter data
	D3DADAPTER_IDENTIFIER8 AdapterIdentifier;
	D3DCAPS8   Caps;

	// Mode data
	DWORD      dwNumModes;
	Mode       modes[150];

	Adapter()
	{
		ZeroMemory(this, sizeof(*this));
	}
};


// end Ripped from dxutil.h

/*
* Geometry Constants
*/

enum DRAWFUNC
{
	DF_DRAWINDEXEDPRIMITIVE=0,    // use vertexbuffer drawprim
    DF_DRAWINDEXEDPRIMITIVEUP=1,  // use user ptr drawprim
    DRAWMAX=2
};

class CBenMark : public CTest
{

public:
	/*
	* Constructor
	*/
	
	CBenMark();

	// Background thread management.
    HANDLE                  m_hThread;
    BOOL                    m_fTerminateThread;

	LONG                    m_lDivides;
    INT                     m_iMaxDivides;

private:

	/*
	 * Support structures.
	 */

	// The different options for the test.
	struct BMXSTATE
	{
		UINT        cApplyTextures;
		BOOL		bUseTriList;
		BOOL        bZenable;
		BOOL        bSpecularenable;
		BOOL        bLocalviewer;

		INT         nDirectionalLights;
		INT         nSpotLights;
		INT         nPointLights;

		INT         iDrawFunc;
        BOOL        fPrecompile;
		D3DFILLMODE iFillMode; 

		DWORD		dwTime;
	};

	BMXSTATE				m_bmxState;
	//CIniFile				m_IniFile;


    typedef VOID (CBenMark::*PFNDRAWRIBBON)(
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
	 * Data members
	 */

	// the following variables were added to remove dependencies on d3dapp
	LPDIRECT3D8				m_pD3D;
	LPDIRECT3DDEVICE8		m_pDevice;
	D3DPRESENT_PARAMETERS	m_Presentation;
	WCHAR					m_szTestStats[256];   // String to hold the test stats.
	Adapter					m_Adapter;
	HANDLE					m_hDevice;			// handle to gamepad device
	BOOL					m_bUseDepthBuffer;
    D3DFORMAT				m_DepthBufferFormat;
	IDirect3DSurface8	   *m_pBackBuffer;
	D3DSURFACE_DESC			m_BackBufferDesc;    // Surface desc of the backbuffer
	HANDLE					m_hGamePad1;
	XINPUT_STATE			m_InputState;
	BitFont					m_Font;
	// menu variables
	BOOL					m_bShowHelp;
	BOOL					m_bShowStats;
	ScenarioMenu*			m_rgpMenus[BENMARK_NUM_MENUS];	// Descriptor for each menu.
	DWORD					m_iActiveKey;
	DWORD					m_iActiveMenu;
	BOOL					m_bRunFullTest;
	BOOL					m_bFrameMoving;
	BOOL					m_bSingleStep;
	DWORD					m_dwCurrentMode;
	D3DMULTISAMPLE_TYPE		m_iSampleType;		 // Current sample mode.
	DWORD					m_dwSampleMask;      // Mask of available FSAA modes.
	// timing variables
	FLOAT					m_fBaseTime;
	FLOAT					m_fStopTime;
	DWORD					m_dwStartLoopTime;
	UINT					m_wLoops;
	UINT					m_wLoop;

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
    FLOAT                   m_fDelta;

	INT						m_nNumLights;	// # of allocated lights.
    
	// long-term stats.
	double					m_fdAveTriPerSec;
	double					m_fdMaxTriPerSec;
	double					m_fdMinTriPerSec;

	DWORD					m_dwAveCount;


	/*
	 * Overridden members of the base class
	 */

	HRESULT Render();
	HRESULT Update();
	HRESULT InitGraphics();
	HRESULT DestroyGraphics();
	HRESULT ProcessInput();
	HRESULT DestroyInput();

	HRESULT ReadIni();

	/*
	 * Implementation
	 */

	VOID InitMaterials(D3DMATERIAL8 *pmatIn, D3DMATERIAL8 *pmatOut);
	VOID InitLights();
	IDirect3DTexture8 *CreateTexture(DWORD dwWidth, DWORD dwHeight);
	VOID InitIndices();
	VOID InitVBs();

	VOID DrawIndexedPrimitive(UINT iIndex, D3DPRIMITIVETYPE dwPrimType, DWORD PrimitiveCount);
	VOID DrawIndexedPrimitiveUP(UINT iIndex, D3DPRIMITIVETYPE dwPrimType, DWORD PrimitiveCount);
	VOID PrecompiledDraw(UINT iIndex, D3DPRIMITIVETYPE dwPrimType, DWORD PrimitiveCount);

    VOID InitDeviceObjects();
    VOID RestoreDeviceObjects();
	VOID UpdateStats(FLOAT fElapsedTime);
	VOID ShowInfo(INT iY, INT cHeight);
	VOID ShowMenu();
	VOID ProcessKeyPress(DWORD iMenu, DWORD iKey);
	VOID Initialize3DEnvironment();
	VOID Change3DEnvironment();
	
	VOID TimeStart();
	VOID TimeStop();
	VOID TimeAdvance();
	FLOAT GetTime();

};

#endif // #ifndef _BENMARK_5_H_