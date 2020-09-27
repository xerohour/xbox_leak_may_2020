/*******************************************************************************

Copyright (c) 2001 Microsoft Corporation.  All rights reserved.

File Name:

    hop.h

Author:

    Robert Heitkamp

Description:

    Higher Order Primitive tests

*******************************************************************************/

#ifndef __CHOP_h__
#define __CHOP_h__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Camera.h"

//------------------------------------------------------------------------------
//	Forward Declarations:
//------------------------------------------------------------------------------
struct HopVertex;

//------------------------------------------------------------------------------
//	CHop:
//------------------------------------------------------------------------------
class CHop : public CScene {

public:

                                    CHop();
                                    ~CHop();

    virtual BOOL                    Create(CDisplay* pDisplay);
    virtual int                     Exhibit(int *pnExitCode);

protected:

    virtual BOOL                    Prepare();
    virtual BOOL                    Setup();
    virtual BOOL                    Initialize();
    virtual void                    Efface();
    virtual void                    Update();
    virtual BOOL                    Render();

    virtual void                    ProcessInput();
    virtual BOOL                    InitView();

    virtual BOOL                    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                                            WPARAM wParam, LPARAM lParam);

private:

	void CalcMinMax(D3DXVECTOR3&, D3DXVECTOR3&, const HopVertex*, UINT);
	void ChangeState(void);

private:

    CVertexBuffer8*				m_frameVertices;
    CVertexBuffer8*				m_hopVertices;
	CIndexBuffer8*				m_frameIndex;
	D3DMATERIAL8				m_frameMaterial;
	CCamera						m_camera;
	float						m_zoomFactor;
	float						m_rotateXFactor;
	float						m_rotateYFactor;
	float						m_rotateZFactor;
	DWORD						m_hVShader;
	D3DXMATRIX					m_worldMatrix;
	UINT						m_numVerts;
	UINT						m_numLines;
	UINT						m_numPatches;
	const D3DRECTPATCH_INFO*	m_rectInfo;
	const D3DTRIPATCH_INFO*		m_triInfo;
	const float*				m_numSegs[16];	// Supports up to 16 patches
	BOOL						m_rectPatch;
//	CTexture8*					m_pTexture;
	IDirect3DTexture8*			m_pTexture;
};

#endif
