//-----------------------------------------------------------------------------
// FILE: FSAA.H
//
// Desc: fsaa header
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _FSAA_H_
#define _FSAA_H_

#include "Test.h"


#define NUMPRIMS 240
#define DA (360.0/(NUMPRIMS*2.0))
#define DR (DA*3.141592654/180.0)
// Define our screen space triangles.

static struct TheVerts { float x,y,z,w; DWORD color; } Verts[NUMPRIMS*3];

//------------------------------------------------------------------------------

struct StateDescription 
{
    DWORD MultisampleFormat;
    WCHAR* Text;
};

#define FSAA_INI_PATH "D:\\fsaa.ini"
#define FSAA_NUM_AA_TYPES 11
#define FSAA_NUM_AA_FORMATS 4

class CFsaa : public CTest
{
public:
	CFsaa();

protected:
	HRESULT InitGraphics();
	HRESULT DestroyGraphics();
	HRESULT InitResources();
	HRESULT DestroyResources();

	HRESULT ReadIni();

	HRESULT ProcessInput();
	HRESULT Update();
	HRESULT Render();

private:
	LPDIRECT3D8					m_pD3D;
	LPDIRECT3DDEVICE8			m_pDevice;                  // our device
	LPDIRECT3DVERTEXBUFFER8		m_pVB;                   // our vertex buffer

	UINT						m_wLoops;
	UINT						m_wLoop;
	DWORD						m_dwStartLoopTime;
	
	INT							m_iType;
	INT							m_iFormat;
	INT							m_iFlicker;
	DWORD						m_dwTime;
	
	DWORD						m_dwFVF;
	
	HANDLE						m_hDevice;      // handle of the gamepad
	BOOL						m_bKeyDown[6];
	BYTE						m_PreviousState[6];

	BOOL						m_bPause;
	BOOL						m_bUseReset;  // Use Reset API instead of CreateDevice/Release
	BOOL						m_bUpdate;

	StateDescription			m_AntialiasTypes[FSAA_NUM_AA_TYPES];
	StateDescription			m_AntialiasFormats[FSAA_NUM_AA_FORMATS];

	DWORD						m_dwFrames;


	VOID InitPresentationParameters(D3DPRESENT_PARAMETERS *ppp);
	BOOL InitGammaRamp();
	VOID DumpStats();
};

#endif // #ifndef _FSAA_H_
