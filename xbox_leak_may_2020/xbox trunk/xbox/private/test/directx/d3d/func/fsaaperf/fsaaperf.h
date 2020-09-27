/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	fsaaperf.h

Abstract:

	Full Screen Anitaliasing performance tests

Author:

	Robert Heitkamp (robheit) 5-Feb-2001

Revision History:

	5-Feb-2001	robheit
		Initial Version

--*/

#ifndef __FSAAPERF_h__
#define __FSAAPERF_h__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Camera.h"
#include "Ball.h"
#include "WireBall.h"
#include "InvertedBall.h"


//------------------------------------------------------------------------------
//	Vertex for background triangles
//------------------------------------------------------------------------------
struct FSAAPERFVertex
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
	DWORD color;        // The vertex color
};
#define FVF_FSAAPERF_BACKGROUND_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

//------------------------------------------------------------------------------
//	CFSAAPerf:
//------------------------------------------------------------------------------
class CFSAAPerf : public CScene {

public:

	struct MultiSampleList {
		D3DMULTISAMPLE_TYPE	type;
		BOOL				edgeAA;
		BOOL				enabled;
		TCHAR				name[128];
		TCHAR				state[128];
		TCHAR				edge[128];
	};

public:

                                    CFSAAPerf();
                                    ~CFSAAPerf();

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
	DWORD RandomColor(void);

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
	MultiSampleList*		m_msList;
	UINT					m_msListSize;
	UINT					m_msListIndex;
	D3DPRESENT_PARAMETERS	m_savedd3dpp;
	BOOL					m_saveParms;
	double					m_currentFrameRate;
	double					m_frameRate;
	double					m_numFrames;
	double					m_avgFrameRate;
	double					m_frequency;
	double					m_maxFrameRate;
	double					m_minFrameRate;
	UINT					m_frameCounter;
	UINT					m_numFramesPerTest;
	UINT					m_numLayers;
	FSAAPERFVertex*			m_backgroundTriangles;
	BOOL					m_enableJoystick;
};

#endif
