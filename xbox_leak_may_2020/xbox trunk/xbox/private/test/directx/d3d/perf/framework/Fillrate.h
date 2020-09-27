//-----------------------------------------------------------------------------
// FILE: FILLRATE.H
//
// Desc: fillrate header
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _FILL_RATE_H_
#define _FILL_RATE_H_
	
#include "Test.h"

#include "utils.h"
//#include "inifile.h"
#include "Bitfont.h"

#define FILLRATE_INI_PATH "D:\\fillrate.ini"
#define FILLRATE_NUM_TEXTURES 4
#define FILLRATE_NUM_TESTS 4

enum { TEX_None, TEX_2d, TEX_Cubemap, TEX_Volume, TEX_Max };
enum { TIME_Present = 0x1, TIME_Render = 0x2 };

struct TIMETESTRESULTS
{
    float   fillrate;
    float   fps;
    int     cFrames;
    float   TimeTillDoneDrawing;
    float   TimeTillIdle;
    UINT64  cNumPixelsDrawn;
};

struct FRSETTINGS
{
    UINT					overdraw;				// number of overdraws in a scene
    UINT					verboselevel;			// chatty when loading pixelshader defs, etc.
    UINT					runtestonreload;		// after reloading fillrate.ini run test

    FLOAT					screenwidth;			// screen width
    FLOAT					screenheight;			// screen height
    D3DFORMAT				BackBufferFormat;       // back buffer d3dformat
    D3DFORMAT				AutoDepthStencilFormat; // z buffer format (0 for none)

    D3DMULTISAMPLE_TYPE		MSType;					// multisample type
    D3DMULTISAMPLE_TYPE		MSFormat;				// multisample format

    DWORD					FVF;					// fvf type
    FLOAT					quadwidth;				// quad width
    FLOAT					quadheight;				// quad height
    FLOAT					quadx;
    FLOAT					quady;

    BOOL					zenable;				// enable z buffer
    BOOL					zwriteenable;			// enable z write
    D3DCMPFUNC				d3dcmpfunc;				// z compare function

    BOOL					primez;					// prime z with quad/D3DCMP_ALWAYS before drawing scene
    D3DCMPFUNC				primezcmpfunc;			// z prime compare function

    UINT					timeflags;				// what to time: TIME_Present, TIME_Render, ...
    INT						numberscenes;			// number of scenes to time (0 to use testtime)
    FLOAT					testtime;				// number of seconds to time

    DWORD					clearflags;				// Clear() flags
    DWORD					cclearval;				// color clear val
    FLOAT					zclearval;				// z clear val
    DWORD					sclearval;				// stencil clear val

    BOOL					userpixelshader;		// use a userpixelshader
    D3DPIXELSHADERDEF		shaderdef;				// the users pixel shader def

    BOOL					alphablendenable;		// turn on alphablending
    BOOL					alphatestenable;		// turn on alphatesting
    BOOL					rotatetex;				// rotate texture

	D3DSWATHWIDTH			SwathWidth;				// swadth width

    D3DTEXTUREOP			ColorOp;				// color op (if !userpixelshader)
    D3DTEXTUREOP			AlphaOp;				// alpha op (if !userpixelshader)

    D3DFILLMODE				d3dfillmode;            // SOLID, WIREFRAME, ...
    DWORD					colorwriteenable;		// D3DRS_COLORWRITEENABLE flags

    struct FRTEX
    {
        UINT				type;					// type of texture (TEX_None, TEX_2d, TEX_Cubemap, TEX_Volume)
        UINT				texsize;				// size of texture
        D3DFORMAT			d3dFormat;				// format of tetxure
        D3DTEXTUREFILTERTYPE filtertype;			// filtertype
    } rgtex[FILLRATE_NUM_TEXTURES];
};

// macros to help us define the size of our texcoords in fillrate.ini
static const DWORD T0_SIZE1 = D3DFVF_TEXCOORDSIZE1(0);
static const DWORD T0_SIZE2 = D3DFVF_TEXCOORDSIZE2(0);
static const DWORD T0_SIZE3 = D3DFVF_TEXCOORDSIZE3(0);
static const DWORD T0_SIZE4 = D3DFVF_TEXCOORDSIZE4(0);

static const DWORD T1_SIZE1 = D3DFVF_TEXCOORDSIZE1(1);
static const DWORD T1_SIZE2 = D3DFVF_TEXCOORDSIZE2(1);
static const DWORD T1_SIZE3 = D3DFVF_TEXCOORDSIZE3(1);
static const DWORD T1_SIZE4 = D3DFVF_TEXCOORDSIZE4(1);

static const DWORD T2_SIZE1 = D3DFVF_TEXCOORDSIZE1(2);
static const DWORD T2_SIZE2 = D3DFVF_TEXCOORDSIZE2(2);
static const DWORD T2_SIZE3 = D3DFVF_TEXCOORDSIZE3(2);
static const DWORD T2_SIZE4 = D3DFVF_TEXCOORDSIZE4(2);

static const DWORD T3_SIZE1 = D3DFVF_TEXCOORDSIZE1(3);
static const DWORD T3_SIZE2 = D3DFVF_TEXCOORDSIZE2(3);
static const DWORD T3_SIZE3 = D3DFVF_TEXCOORDSIZE3(3);
static const DWORD T3_SIZE4 = D3DFVF_TEXCOORDSIZE4(3);


class CFillRate : public CTest
{

public:
	CFillRate();
	HRESULT InitGraphics();
	HRESULT DestroyGraphics();
	HRESULT DestroyInput();

	HRESULT ProcessInput();
	HRESULT Update();
	HRESULT Render();

	// fillrate.cpp
	float DoTimedTest(TIMETESTRESULTS *pttres = NULL);
	void DoCompleteTimedTest();
	void DoQuickTimedTest();
	void DoMultiSampleTimedTest();
	void DumpCurrentSettings();
	void DoTextureTest();
	
	void ReadPixelShaderIni(D3DPIXELSHADERDEF *pshaderdef);
	BOOL ReadFRIniFile();

	IDirect3DBaseTexture8* MyCreateTexture(D3DRESOURCETYPE d3dtype, D3DFORMAT d3dFormat, DWORD dwWidth, DWORD dwHeight);
	void CreateQuadList(int nquads);
	BOOL InitFR(BOOL freadINIFile);
	BOOL InitD3D();
	void DumpZ();
	void RenderFrame(int overdraw);
	FLOAT EstimateTimeForScene();

private:
	LPDIRECT3DDEVICE8		m_pDevice;
	LPDIRECT3DVERTEXBUFFER8 m_pVB;                   // our vertex buffer
	LPDIRECT3DBASETEXTURE8	m_pTexture[FILLRATE_NUM_TEXTURES];       // 4 textures
	LPDIRECT3DSURFACE8      m_pBackBuffer;
	HANDLE                  m_hInpDevice;            // input device
	DWORD                   m_ShaderHandle;             // pixel shader handle
	UINT                    m_cIndices;                // count of indices
	WORD*                   m_pIndices;              // indices allocation

	DWORD                   m_dwFVFSize;                    // FVF size

	FRSETTINGS              m_var;                    // current settings

	BitFont					m_Font;
	
	BOOL                    m_fRedrawScreen;         // refresh screen?
	BOOL                    m_fdumpZ;               // dump Z val

	UINT					m_wLoops;
	UINT					m_wLoop;

	struct
	{
		LPCSTR szDesc;
		void (CFillRate::*pFunc)();
	} m_rgTests[FILLRATE_NUM_TESTS];
	UINT m_iCurTest;

};

#endif // #ifndef _FILL_RATE_H_
