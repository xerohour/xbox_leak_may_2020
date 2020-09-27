/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	scene.h

Abstract:

	3D scene

Author:

	Robert Heitkamp (robheit) 27-Apr-2001

Environment:

	Xbox only

Revision History:

	27-Apr-2001 robheit
		Initial Version

--*/
#ifndef __SCENE_H__
#define __SCENE_H__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "xtl.h"
#include "xgmath.h"
#include "Camera.h"
#include "Ball.h"
#include "InvertedBall.h"
#include "bitfont.h"
#include "spark.h"

namespace Media {

//------------------------------------------------------------------------------
//	Vertex for background triangles
//------------------------------------------------------------------------------
#define FVF_CSCENE_BACKGROUND_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

//------------------------------------------------------------------------------
//	Number of selectable items in UI
//------------------------------------------------------------------------------
#define NUM_SELECTIONS 13

//------------------------------------------------------------------------------
//	CScene:
//------------------------------------------------------------------------------
class CScene {

public:

	struct Vertex
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex
		DWORD color;        // The vertex color
	};

	struct TVertex
	{
		FLOAT x, y, z, rhw; // The transformed position for the vertex
        float u, v;         // Texture coordinates
	};

	enum Hit
	{
		HIT_NOTHING,
		HIT_WALL,
		HIT_BALL
	};

public:

	CScene(void);
    ~CScene(void);

    HRESULT Initialize(IDirect3DDevice8* pDevice);
	void Render(IDirect3DDevice8* pDevice);
	void NextFrame(void);

	void AddBall(void);
	void RemoveBall(void);

	void AddLight(void);
	void RemoveLight(void);

	BOOL DrawText( void ) { return m_bDrawText; }
	void SetDrawText( BOOL bDrawText ) { m_bDrawText = bDrawText; }

private:

	BOOL IntersectRaySphere(const XGVECTOR3&, const XGVECTOR3&, 
							const XGVECTOR3&, float, XGVECTOR3&);
	BOOL IntersectRayInSphere(const XGVECTOR3&, const XGVECTOR3&, 
							  const XGVECTOR3&, float, XGVECTOR3&);
	BOOL GetSphereIntersection(const XGVECTOR3&, float, const XGVECTOR3&,
							   const XGVECTOR3&, float, float&);
	DWORD RandomColor(void);
	void CollideBalls(const XGVECTOR3&, XGVECTOR3&, float&, float,
					  const XGVECTOR3&, XGVECTOR3&, float&, float);

	void BallHitBall(UINT, UINT);
	void BallHitWall(UINT, const XGVECTOR3&);
	void BallSpeedChanged(UINT, float, float);
	void BallDirectionChanged(UINT, const XGVECTOR3&, const XGVECTOR3&);

    HRESULT CreateShaders(IDirect3DDevice8*);
    void ReleaseShaders();
    HRESULT CreateNormalMap(IDirect3DDevice8* pDevice, UINT uLength, UINT uLevels, IDirect3DCubeTexture8** ppd3dtc);
	void RecalculateSpeeds(void);
	
    void CycleDisplayMode(void);

public:
	CCamera				m_camera;

private:
    HANDLE              m_hFile;
	UINT				m_numBalls;
	UINT				m_maxBalls;
    UINT                m_numLights;
    UINT                m_maxLights;
	CBall*				m_balls;
	XGMATRIX			m_identityMatrix;
	float				m_radius;
	float				m_radius2;
	CInvertedBall		m_invertedSphere;
	UINT				m_numLayers;
	Vertex*				m_backgroundTriangles;
	IDirect3DSurface8*	m_backBuffer;
	BitFont				m_font;
	double				m_elapsedTime;
	double				m_lastTime;
	double				m_avgFrameRate;
	double				m_currentFrameRate;
	double				m_frequency;
	double				m_startTime;
	double				m_lastCycle;
	UINT				m_frame;
	int					m_ballDetail;
    int                 m_lightDetail;
	int					m_wallDetail;
	float				m_maxSpeed;
	float				m_minSpeed;
	double				m_drawRate;
	UINT				m_numTris;
    DWORD               m_dwVShader;
    DWORD               m_dwVShader2;
    DWORD               m_dwVShaderAddress;
    DWORD               m_dwVShaderAddress2;
    DWORD               m_dwPShader;
    IDirect3DCubeTexture8* m_pd3dtcNormal;
    D3DCOLORVALUE       m_dcvAmbient;
    IDirect3DDevice8*   m_pDevice;
	BOOL				m_bDrawWireframe;
	BOOL				m_bRelativeSpeed;
	BOOL				m_bDrawText;
	CSpark				m_sparks;
    IDirect3DTexture8*  m_pd3dtText;
    TVertex            m_prText[4];
	float				m_dSpeed;
	float				m_currentMaxSpeed;
	float				m_currentMinSpeed;
    D3DDISPLAYMODE*     m_pd3ddm;
    UINT                m_uNumDisplayModes;
    UINT                m_uDisplayMode;

    //
    // audio debug variables
    //

    DWORD               m_dwAudioReadPtr;
    DWORD               m_dwAudioWritePtr;
    DWORD               m_dwAudioReadTotal;
    DWORD               m_dwAudioWriteTotal;

    DWORD               m_dwAudioDelta;

};


//------------------------------------------------------------------------------
//	File IO event handler
//------------------------------------------------------------------------------
struct FileIOHandle
    {
    DWORD buffSize;
    char *buffer;
    };
}
#endif
