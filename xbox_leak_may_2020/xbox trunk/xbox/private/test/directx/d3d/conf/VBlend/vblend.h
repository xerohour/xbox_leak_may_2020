// =======================================================================================
//
//  VBlend.h
//
//  Original Code: Eron Hennessey
//
//  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
//
//  -------------------------------------------------------------------------------------- 
// 
//  Minimun DX Version: 8
//
// ======================================================================================= 

#ifndef __VBLEND_H__
#define __VBLEND_H__

//#include "CD3DTest.h"
//#include <d3d8.h>

//USETESTFRAME

#define SPACING 50.0f

// ---------------------------------------------------------------------------------------
//  CVERTEX definition - this will be used to store the vertices
// ---------------------------------------------------------------------------------------

const DWORD FVF_CVERTEX_ = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_DIFFUSE;
struct CVERTEX_ {
	D3DVALUE x, y, z;
	D3DVALUE nx, ny, nz;
	DWORD    color;
	D3DVALUE tu, tv;
};

//const DWORD FVF_VERTEX = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1;
struct VERTEX_ {
	D3DVALUE x, y, z;
	D3DVALUE nx, ny, nz;
	D3DVALUE tu, tv;
};

// ---------------------------------------------------------------------------------------
//  Other constants
// ---------------------------------------------------------------------------------------

const int  MAX_TESTS = 20;

const WORD BLEND_POS = 0x0001;
const WORD BLEND_NML = 0x0002;
const WORD BLEND_CLR = 0x0004;
const WORD BLEND_TXC = 0x0008;

// ---------------------------------------------------------------------------------------
// Class definition
// ---------------------------------------------------------------------------------------

class CVBlend : public CD3DTest
{
protected:
	WORD          m_wFlags;

	DWORD         m_hShader;
	UINT          m_nVertices;
	UINT          m_nIndices;

	float         m_fBlendFactor;

	CVertexBuffer8 *m_pVBO_S, *m_pVBO_R;  // source and reference vertex buffers
	CVertexBuffer8 *m_pVBF_S, *m_pVBF_R;  // same here
	CIndexBuffer8  *m_pIB_S,  *m_pIB_R;   // and likewise for the index buffers

    BOOL            m_bExit;

public:
	CVBlend();
	~CVBlend();

	// Framework functions
	virtual bool SetDefaultMatrices();
	virtual bool SetDefaultMaterials();
	virtual bool SetDefaultLights();
	virtual bool SetDefaultLightStates();
	virtual bool SetDefaultRenderStates();
	virtual UINT TestInitialize(void);
	virtual bool ClearFrame(void);
	virtual bool ExecuteTest(UINT);
	virtual void SceneRefresh(void);
	virtual bool ProcessFrame(void);
	virtual bool TestTerminate(void);

	virtual bool CreateShader() = 0; // pure virtual, will exist in derived classes

	// all derived classes will call this in CreateShader()
	bool SetupShader(DWORD *pDecl, char *pcsCode);

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};

#endif
