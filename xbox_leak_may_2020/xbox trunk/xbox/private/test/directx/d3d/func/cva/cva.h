/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	cva.h

Abstract:

	Compressed Vertex Formats tests

Author:

	Robert Heitkamp (robheit) 5-Feb-2001

Revision History:

	24-Feb-2001	robheit
		Initial Version

--*/

#ifndef __CVA_h__
#define __CVA_h__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Camera.h"
#include "Ball.h"
#include "WireBall.h"
#include "InvertedBall.h"

//------------------------------------------------------------------------------
//	CCVA:
//------------------------------------------------------------------------------
class CCVA: public CScene {

public:

                                    CCVA();
                                    ~CCVA();

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

	BOOL IntersectRaySphere(const D3DXVECTOR3&, const D3DXVECTOR3&, 
							const D3DXVECTOR3&, float, D3DXVECTOR3&);
	BOOL GetSphereIntersection(const D3DXVECTOR3&, float, const D3DXVECTOR3&,
							   const D3DXVECTOR3&, float, float&);


private:

	UINT			m_numBalls;
	CBall			m_balls[64];
	BOOL			m_useBall[64];
	CCamera			m_camera;
	float			m_zoomFactor;
	float			m_rotateXFactor;
	float			m_rotateYFactor;
	float			m_rotateZFactor;
	D3DXMATRIX		m_worldMatrix;
	float			m_radius;
	float			m_radius2;
	CWireBall		m_sphere;
	CInvertedBall	m_invertedSphere;
	float			m_ballRadius;
	
};

#endif
