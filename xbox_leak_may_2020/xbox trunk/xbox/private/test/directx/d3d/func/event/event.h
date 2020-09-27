/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	event.h

Abstract:

	Event Notifier Test

Author:

	Robert Heitkamp (robheit) 22-Feb-2001

Revision History:

	5-Feb-2001	robheit
		Initial Version

--*/

#ifndef __EVENT_h__
#define __EVENT_h__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Camera.h"
#include "Ball.h"
#include "WireBall.h"
#include "InvertedBall.h"

//------------------------------------------------------------------------------
//	CEvent:
//------------------------------------------------------------------------------
class CEvent : public CScene {

public:

                                    CEvent();
                                    ~CEvent();

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

	UINT					m_numBalls;
	CBall*					m_balls;
	int						m_numVertices;
	int						m_numTriangles;
	CCamera					m_camera;
	float					m_zoomFactor;
	float					m_rotateXFactor;
	float					m_rotateYFactor;
	float					m_rotateZFactor;
	D3DXMATRIX				m_worldMatrix;
	float					m_radius;
	float					m_radius2;
	CWireBall				m_sphere;
	CInvertedBall			m_invertedSphere;
	D3DPRESENT_PARAMETERS	m_savedd3dpp;
	BOOL					m_saveParms;
	DWORD*					m_fenceHandles;	

};

#endif
