/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	scene.cpp

Abstract:

	3D Scene

Author:

	Robert Heitkamp (robheit) 27-Apr-2001

Environment:

	Xbox only

Revision History:

	27-Apr-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <stdio.h>
#include <xtl.h>
#include <xdbg.h>
#include <xgraphics.h>
#include "media.h"
#include "scene.h"
#include "helpers.h"
#include "music.h"

using namespace WFVO;

extern IDirect3D8* g_d3d;

namespace WFVO {

extern CMusic g_music;
extern LPDIRECTSOUND g_dsound;

//------------------------------------------------------------------------------
//	Macros:
//------------------------------------------------------------------------------
#define ADJUST(A) (((-(A)) + 1.0f) / 2.0f)
#define MIN2(A,B) ((A) <= (B) ? (A) : (B))
#define MAX2(A,B) ((A) >= (B) ? (A) : (B))
#define MAX3(A,B,C) MAX2((A), MAX2((B), (C)))
#define FUDGE(A,B,C) (fabs((A) - (B)) < (C) ? (B) : (A))
#ifndef PI 
#	define PI 3.14159265359f
#endif

#define BALL_RADIUS		0.5f
#define LIGHT_RADIUS	0.125f
#define BALL_MASS		10.0f
#define LIGHT_MASS		1.0f

#define DRAWTEXT( a, b, c, d, e, f, g ) \
if ( m_bDrawText )						\
{										\
	m_font.DrawText( a, b, c, d, e, f, g ); \
}											


//------------------------------------------------------------------------------
//	File IO handlers
//------------------------------------------------------------------------------
VOID CompleteIO(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
    {
    if(!lpOverlapped || !lpOverlapped->hEvent) 
        {
        // this is bad - memory is corrupt - and because of this we will leak
        return;
        }

    FileIOHandle *info = (FileIOHandle*)lpOverlapped->hEvent;

    if(dwNumberOfBytesTransfered != info->buffSize)
        {
        // error
        }

    // clean up all the handles and structs
    delete info->buffer;
    delete info;
    delete lpOverlapped;
    }


//------------------------------------------------------------------------------
//	CScene::CScene
//------------------------------------------------------------------------------
CScene::CScene(void)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
    m_pd3ddm = NULL;
}  

//------------------------------------------------------------------------------
//	CScene::~CScene
//------------------------------------------------------------------------------
CScene::~CScene(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
	delete [] m_balls;
	delete [] m_backgroundTriangles;
    ReleaseShaders();
    if (m_pd3ddm) {
        delete [] m_pd3ddm;
    }
}

//------------------------------------------------------------------------------
//	CScene::Initialize
//------------------------------------------------------------------------------
HRESULT
CScene::Initialize(
				   IN IDirect3DDevice8*	pDevice
				   ) 
/*++

Routine Description:

	Intializes the scene

Arguments:

	IN pDevice -	IDirect3DDevice8

Return Value:

	S_OK on success, any other value on failure

--*/
{
	LONGLONG	timer;
	HRESULT		hr;
	UINT		i;
	UINT		ii;
	BOOL		goodLoc;
	XGVECTOR3	loc;
	XGVECTOR3	dir;
	float		speed;
	float		dxyz;
	double		phi;
	double		rho;
	LONGLONG frequency;
    D3DLOCKED_RECT d3dlr;

    m_hFile                 = INVALID_HANDLE_VALUE;
	m_ballDetail			= g_TestParams.dwBallDetail;	// Default = 12
    m_lightDetail			= g_TestParams.dwLightDetail;   // Default = 12
	m_wallDetail			= g_TestParams.dwWallDetail;	// Default = 20
	m_numLayers				= g_TestParams.dwNumLayers;		// Default = 0
	m_numBalls				= g_TestParams.dwNumBalls;		// Default = 20
	m_maxBalls				= g_TestParams.dwMaxBalls;		// Default = 50
    m_numLights				= g_TestParams.dwNumLights;     // Default = 5
    m_maxLights				= g_TestParams.dwMaxLights;     // Default = 10
	m_radius				= g_TestParams.fRadius;	        // Default = 5.5f
	m_maxSpeed				= g_TestParams.fMaxSpeed;		// Default = 15.0f
	m_minSpeed				= g_TestParams.fMinSpeed;		// Default = 0.1f
	m_bDrawWireframe		= g_TestParams.bDrawWireframe;	// Default = TRUE
	m_bRelativeSpeed		= g_TestParams.bRelativeSpeed;	// Default = TRUE
	m_radius2				= m_radius * m_radius;
	m_elapsedTime			= 0.0;
	m_lastTime				= 0.0;
	m_lastCycle				= 0.0;
	m_avgFrameRate			= 0.0;
	m_currentFrameRate		= 0.0;
	m_frame					= 0;
	m_numTris				= 0;
	m_drawRate				= 60.0;

	// this is set by button state, not the ini

	m_bDrawText				= TRUE;

    // We need to store the device in order to delete our programmable shaders on exit
    m_pDevice = pDevice;

	// Cheesey error detection
	m_balls = new CBall [m_maxBalls + m_maxLights];
	ASSERT(m_balls != NULL);
	
	m_backgroundTriangles = new CScene::Vertex [m_numLayers * 3 * 2];
	ASSERT(m_backgroundTriangles != NULL);

	// Query the performance frequency
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	m_frequency = (double)frequency;

    // Build a display mode list
    m_uNumDisplayModes = g_d3d->GetAdapterModeCount(D3DADAPTER_DEFAULT);
    if (!m_uNumDisplayModes) {
        return E_FAIL;
    }

    m_pd3ddm = new D3DDISPLAYMODE[m_uNumDisplayModes];
    if (!m_pd3ddm) {
        return E_OUTOFMEMORY;
    }

    for (i = 0; i < m_uNumDisplayModes; i++) {
        hr = g_d3d->EnumAdapterModes(D3DADAPTER_DEFAULT, i, &m_pd3ddm[i]);
        if (FAILED(hr)) {
            return hr;
        }
    }
    m_uDisplayMode = 0;
	
	// Create the sphere
	hr = m_invertedSphere.Create(pDevice, m_radius, m_wallDetail * 2, m_wallDetail);
	if(hr != S_OK)
		return hr;
	m_numTris += m_invertedSphere.GetNumTris();

	m_invertedSphere.SetPosition(XGVECTOR3(0.0f, 0.0f, 0.0f));

	// Create the balls
	for(i=0; i<m_maxBalls; ++i)
	{
		hr = m_balls[i].Create(pDevice, BALL_RADIUS, BALL_MASS, m_ballDetail, 
							   (int)((float)m_ballDetail * 0.75f), FALSE, i);
		if(hr != S_OK)
			return hr;
	}

    // Create the lights
    for (; i < m_maxBalls + m_maxLights; i++) {
        hr = m_balls[i].Create(pDevice, LIGHT_RADIUS, LIGHT_MASS, m_lightDetail, 
							   (int)((float)m_lightDetail * 0.75f), TRUE, i);
		if(hr != S_OK)
			return hr;
    }

	m_sparks.Create(pDevice);

	//initialize the sparks


	// Create the background triangles
	for(ii=0, i=0; i<m_numLayers; ++i)
	{
		m_backgroundTriangles[ii].x			= 0.0f;
		m_backgroundTriangles[ii].y			= 0.0f;
		m_backgroundTriangles[ii].z			= 1.0f;
		m_backgroundTriangles[ii].rhw		= 1.0f;
		m_backgroundTriangles[ii++].color	= RandomColor();
		m_backgroundTriangles[ii].x			= 640.0f;
		m_backgroundTriangles[ii].y			= 0.0f;
		m_backgroundTriangles[ii].z			= 1.0f;
		m_backgroundTriangles[ii].rhw		= 1.0f;
		m_backgroundTriangles[ii++].color	= m_backgroundTriangles[ii-1].color;
		m_backgroundTriangles[ii].x			= 0.0f;
		m_backgroundTriangles[ii].y			= 480.0f;
		m_backgroundTriangles[ii].z			= 1.0f;
		m_backgroundTriangles[ii].rhw		= 1.0f;
		m_backgroundTriangles[ii++].color	= m_backgroundTriangles[ii-1].color;
		m_backgroundTriangles[ii].x			= 0.0f;
		m_backgroundTriangles[ii].y			= 480.0f;
		m_backgroundTriangles[ii].z			= 1.0f;
		m_backgroundTriangles[ii].rhw		= 1.0f;
		m_backgroundTriangles[ii++].color	= m_backgroundTriangles[ii-1].color;
		m_backgroundTriangles[ii].x			= 640.0f;
		m_backgroundTriangles[ii].y			= 0.0f;
		m_backgroundTriangles[ii].z			= 1.0f;
		m_backgroundTriangles[ii].rhw		= 1.0f;
		m_backgroundTriangles[ii++].color	= m_backgroundTriangles[ii-1].color;
		m_backgroundTriangles[ii].x			= 640.0f;
		m_backgroundTriangles[ii].y			= 480.0f;
		m_backgroundTriangles[ii].z			= 1.0f;
		m_backgroundTriangles[ii].rhw		= 1.0f;
		m_backgroundTriangles[ii++].color	= m_backgroundTriangles[ii-1].color;

		m_numTris += 2;
	}

    // Create the shaders
    hr = CreateShaders(pDevice);
    if (FAILED(hr)) {
        return hr;
    }

    // Create the cube texture renormalization map
    hr = CreateNormalMap(pDevice, 256, 0, &m_pd3dtcNormal);
    if (FAILED(hr)) {
        return hr;
    }

    // Create a buffer for the text
    hr = pDevice->CreateTexture(320, 240, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &m_pd3dtText);
    if (FAILED(hr)) {
        return hr;
    }

    // Clear the texture
    m_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
    for (i = 0; i < 240; i++) {
        memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
    }
    m_pd3dtText->UnlockRect(0);

    m_prText[0].x = 0.0f;
    m_prText[0].y = 480.0f;
    m_prText[0].u = 0.0f;
    m_prText[0].v = 240.0f;

    m_prText[1].x = 0.0f;
    m_prText[1].y = 0.0f;
    m_prText[1].u = 0.0f;
    m_prText[1].v = 0.0f;

    m_prText[2].x = 640.0f;
    m_prText[2].y = 0.0f;
    m_prText[2].u = 320.0f;
    m_prText[2].v = 0.0f;

    m_prText[3].x = 640.0f;
    m_prText[3].y = 480.0f;
    m_prText[3].u = 320.0f;
    m_prText[3].v = 240.0f;

    for (i = 0; i < 4; i++) {
        m_prText[i].z = 0.0f; //0.000001f;
        m_prText[i].rhw = 1.0f; ///1000000.0f;
    }

	// Setup the camera (resuse some other vars)
	m_camera.SetViewport(0, 0, 640, 480, 0.0f, 1.0f);
	m_camera.LookAt(XGVECTOR3(0.0f, 0.0f, -2.0f * m_radius), 
					XGVECTOR3(0.0f, 0.0f, 0.0f), XGVECTOR3(0.0f, 1.0f, 0.0f));
	m_camera.SetPerspectiveFov(45.0f, 640.0f / 480.0f, 0.99f * m_radius, 3.01f * m_radius);
	m_camera.EnableLight(TRUE, FALSE);
	m_camera.RotateLightY(-30.0f);
	m_camera.RotateLightX(-30.0f);
	XGMatrixIdentity(&m_identityMatrix);

	// Set the position, speed and direction for the balls
	for(i=0; i<m_maxBalls + m_numLights; ++i) {

        if (i == m_numBalls) {
			if(m_numLights == 0)
				break;
            i = m_maxBalls;
        }

		// Make sure the new ball is not within 1 unit of another
		do {
			goodLoc = TRUE;
			loc.x = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			loc.y = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			loc.z = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			dir.x = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
			dir.y = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
			dir.z = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
			XGVec3Normalize(&dir, &dir);
			speed = (float)rand() / (float)RAND_MAX * (m_maxSpeed - m_minSpeed) + m_minSpeed;
			for(ii=0; goodLoc && ii<i; ++ii) {
                if (ii == m_numBalls) {
                    ii = m_maxBalls;
                }
				if(XGVec3Length(&XGVECTOR3(loc - m_balls[ii].GetLocation())) <= m_balls[ii].GetRadius() + m_balls[i].GetRadius())
					goodLoc = FALSE;
			}
		} while(!goodLoc);

		m_balls[i].SetPosition(loc);
		m_balls[i].SetDirection(dir);
		m_balls[i].SetSpeed(speed);
		
		m_numTris += m_balls[i].GetNumTris();
	}

	RecalculateSpeeds();

    // Global scene ambient lighting
    m_dcvAmbient.r = 0.2f;
    m_dcvAmbient.g = 0.2f;
    m_dcvAmbient.b = 0.2f;
    m_dcvAmbient.a = 1.0f;

    pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA((BYTE)(m_dcvAmbient.r * 255.0f), (BYTE)(m_dcvAmbient.g * 255.0f), (BYTE)(m_dcvAmbient.b * 255.0f), (BYTE)(m_dcvAmbient.a * 255.0f)));

	// Get the backbuffer for the font
	hr = pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_backBuffer);
	if(hr != D3D_OK)
		return hr;

	// Setup the timer
	QueryPerformanceCounter((LARGE_INTEGER*)&timer);
	m_lastTime	= (double)timer / m_frequency;
	m_startTime	= m_lastTime;
	m_lastCycle	= m_lastTime;

	XGVECTOR4 sparkcolor(1.0f, 0.25f, 0.0f, 0.6f/(float)g_TestParams.dwSparkMaxAge);
	pDevice->SetVertexShaderConstant(23, &sparkcolor, 1);


    //
    // audio variables
    //


    m_dwAudioReadPtr = 0;
    m_dwAudioWritePtr = 0;
    m_dwAudioReadTotal = 0;
    m_dwAudioWriteTotal = 0;
    m_dwAudioDelta = 100000;

    return S_OK;
}

//------------------------------------------------------------------------------
//	CScene::NextFrame
//------------------------------------------------------------------------------
void CScene::NextFrame(void)
/*++

Routine Description:

	Sets up for the next frame

Arguments:

	None

Return Value:

	None

--*/
{
	XGVECTOR3	point;
	float		distance;
	float		saveDistance;
	UINT		hit;
	float		moveLeft;
	float		moved;
	XGVECTOR3	normal;
	XGVECTOR3	inverse;
	XGVECTOR3	reflection;
	UINT		i;
	UINT		ii;
	CScene::Hit	hitWhat;
	XGVECTOR3	loc;
    static BOOL doUpdate = TRUE;
	XGVECTOR3	saveNormal;
	float		speed;
	XGVECTOR3	dirA;
	XGVECTOR3	dirB;
	float		speedA;
	float		speedB;
	long		lastBallHit;
	UINT		numObjects = m_maxBalls + m_numLights;
	UINT		hits;

	// To disable ball or test changes for debug purposes
	if(!doUpdate)
		return;

	//update sparks
	m_sparks.Update(1/(float)m_drawRate);

	// Move all the balls
	for(i=0; i<numObjects; ++i) 
	{
        if (i == m_numBalls) {
			if(m_numLights == 0)
				break;
            i = m_maxBalls;
        }

		// Generate a speed based on the framerate so changes to framerates do not
		// change the actual distance a ball moves
		speed		= m_balls[i].GetSpeed() / (float)m_drawRate;
		moveLeft	= speed;
		moved		= 0.0f;	
		lastBallHit	= -1;
		hits		= 0;

		while((moveLeft > 0.0) && (hits < numObjects))
		{
			++hits;
			saveDistance	= 1.0e30f;
			hitWhat			= CScene::HIT_NOTHING;
			hit				= i;

			// Get the intersection with the bounding sphere
			if(!IntersectRayInSphere(m_balls[i].GetLocation(), m_balls[i].GetDirection(), 
								     XGVECTOR3(0.0f, 0.0f, 0.0f), m_radius, point)) 
			{
				__asm int 3;	// This should never happen!
				doUpdate = FALSE;
				return;
			}

			distance = XGVec3Length(&XGVECTOR3(point - m_balls[i].GetLocation())) -
					   m_balls[i].GetRadius();
			if(distance < 0.0f)
				distance = 0.0f;

			// Will the bounding sphere be hit?
			if(distance <= moveLeft) 
			{
				saveDistance	= distance;
				hitWhat			= CScene::HIT_WALL;
			}

			// Check for intersection with all other balls
			for(ii=0; ii<numObjects; ++ii) 
			{
                if (ii == m_numBalls) {
					if(m_numLights == 0)
						break;
                    ii = m_maxBalls;
                }

				if(i != ii)
				{
					if(GetSphereIntersection(m_balls[i].GetLocation(),
											 m_balls[i].GetRadius(),
											 m_balls[i].GetDirection(),
											 m_balls[ii].GetLocation(),
											 m_balls[ii].GetRadius(),
											 distance)) 
					{
						distance = FUDGE(distance, moveLeft, 0.0001);
						if(distance < 0.0f)
							distance = 0.0f;
						if((distance <= moveLeft) && (distance < saveDistance))
						{
							normal.x = (m_balls[i].GetLocation().x + m_balls[i].GetDirection().x * distance) - 
										m_balls[ii].GetLocation().x;
							normal.y = (m_balls[i].GetLocation().y + m_balls[i].GetDirection().y * distance) -
										m_balls[ii].GetLocation().y;
							normal.z = (m_balls[i].GetLocation().z + m_balls[i].GetDirection().z * distance) -
										m_balls[ii].GetLocation().z;

							// Check for valid hit
							if(XGVec3Dot(&normal, &m_balls[i].GetDirection()) < 0.0f)
							{
								XGVec3Normalize(&saveNormal, &normal);
								saveDistance	= distance;
								hit				= ii;
								hitWhat			= CScene::HIT_BALL;
							}
						}
					}
				}
			}

			// What was hit?
			switch(hitWhat)
			{
			case CScene::HIT_WALL:	// Hit the wall

				// Normal at inner point on sphere
				normal = -point;
				XGVec3Normalize(&normal, &normal);

				// Callback
				BallHitWall(i, normal);

				// Move the ball
				m_balls[i].Move(saveDistance);
				moved = saveDistance;

				// Inverse of direction
				inverse = -m_balls[i].GetDirection();
				
				// Calculate the reflection
				reflection = normal * 2.0f * (XGVec3Dot(&normal, &inverse)) - inverse;

				// Callback
				BallDirectionChanged(i, m_balls[i].GetDirection(), reflection);

				// Change the ball's direction
				m_balls[i].SetDirection(reflection);
				break;

			case CScene::HIT_BALL:	// Hit another ball
			
				// Prevent a ball from running into the same ball multiple times, 
				// and make sure the colliding ball overtakes the target
				if(((long)hit != lastBallHit) &&
				   (m_balls[i].GetSpeed() > (m_balls[hit].GetSpeed() * 
											 XGVec3Dot(&m_balls[i].GetDirection(),
													   &m_balls[hit].GetDirection()))))
				{
					lastBallHit = (long)hit;

					// Move the ball
					m_balls[i].Move(saveDistance);
					moved = saveDistance;

					dirA	= m_balls[i].GetDirection();
					dirB	= m_balls[hit].GetDirection();
					speedA	= m_balls[i].GetSpeed();
					speedB	= m_balls[hit].GetSpeed();

					// If both balls are of the same type, collide as normal
					CollideBalls(m_balls[i].GetLocation(), dirA, speedA, m_balls[i].GetMass(),
								 m_balls[hit].GetLocation(), dirB, speedB, m_balls[hit].GetMass());

					// If the ball's speed changes so should moveLeft...
					moveLeft -= (m_balls[i].GetSpeed() - speedA) / (float)m_drawRate;
					
					// Callbacks
					BallHitBall(i, hit);
					BallDirectionChanged(i, m_balls[i].GetDirection(), dirA);
					BallDirectionChanged(hit, m_balls[hit].GetDirection(), dirB);
					BallSpeedChanged(i, m_balls[i].GetSpeed(), speedA);
					BallSpeedChanged(hit, m_balls[hit].GetSpeed(), speedB);
					
					m_balls[i].SetDirection(dirA);
					m_balls[hit].SetDirection(dirB);
					m_balls[i].SetSpeed(speedA);
					m_balls[hit].SetSpeed(speedB);

					RecalculateSpeeds();
				}
				else
					moveLeft = 0.0f;
				break;

			default:	// Otherwise, just move the ball forward

				m_balls[i].Move(moveLeft);
				moved = moveLeft;

				break;
			}
			moveLeft -= moved;
		}
	}
}

//------------------------------------------------------------------------------
//	CScene::Render
//------------------------------------------------------------------------------
void 
CScene::Render(
			   IN IDirect3DDevice8*	pDevice
			   ) 
/*++

Routine Description:

	Renders the scene

Arguments:

	IN pDevice -	IDirect3DDevice8

Return Value:

	None

--*/
{
    DWORD				dwValue;
	LONGLONG			timer;
	LONGLONG			start;
	WCHAR				buffer[256];
	UINT				i;
	UINT				j;
	UINT				k;
	UINT				y;
    HRESULT				hr;
	double				currentTime;
	float				lineWidth = 1.0f;
    XGMATRIX			mView;
	XGMATRIX			mProj;
	XGMATRIX			mViewProj;
    XGVECTOR3			vEyePos;
    D3DLIGHT8			light;
    IDirect3DSurface8*	pd3ds;
	int					hours;
	int					minutes;
	int					seconds;
	int					fraction;
	BOOL				bCycle;
	double				dt;
    D3DLOCKED_RECT		d3dlr;
    DWORD				dwNormal;
    DWORD				dwPhysical;        

	// For draw rate
	QueryPerformanceCounter((LARGE_INTEGER*)&start);
	
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_MULTISAMPLEMASK, 0xffffffff);
	pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_LINEWIDTH, *(DWORD*)&lineWidth);

    for (i = 0; i < 2; i++) {
        pDevice->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        pDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        pDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_POINT);
    }
    for (i = 2; i < 4; i++) {
        pDevice->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_POINT);
        pDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_POINT);
        pDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_POINT);
    }

    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
    pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

    pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, F2DW(1.0f));
    pDevice->SetRenderState(D3DRS_POINTSCALE_A, F2DW(0.0f));
    pDevice->SetRenderState(D3DRS_POINTSCALE_B, F2DW(0.0f));
    pDevice->SetRenderState(D3DRS_POINTSCALE_C, F2DW(1.0f));
    pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);

    pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xff000000, 1.0f, 0);

    // Begin the scene
    pDevice->BeginScene();

    // Screen space offset
    if (m_pd3ddm[m_uDisplayMode].Flags & D3DPRESENTFLAG_FIELD) {
        D3DFIELD_STATUS fstatus;
        pDevice->GetDisplayFieldStatus(&fstatus);
        pDevice->SetScreenSpaceOffset(0.0f, (fstatus.Field == D3DFIELD_EVEN) ? 0.0f : 0.5f);
    }

	// World matrix
	pDevice->SetTransform(D3DTS_WORLD, &m_identityMatrix);

	// Camera
	m_camera.Render(pDevice);

    // Get the view and projection matrices
    pDevice->GetTransform(D3DTS_VIEW, &mView);
    pDevice->GetTransform(D3DTS_PROJECTION, &mProj);

    XGMatrixMultiply(&mViewProj, &mView, &mProj);

    // Get the camera position
    m_camera.GetPosition(&vEyePos);

	// Draw the background triangles with depth write disabled to ensure all are written
	// without affecting other draws
    if(m_numLayers) {
        pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        pDevice->SetVertexShader(FVF_CSCENE_BACKGROUND_VERTEX);
        pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_numLayers * 2, m_backgroundTriangles,
                                 sizeof(CScene::Vertex));
        pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    }
    
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    pDevice->SetVertexShader(m_dwVShader);
    pDevice->SetPixelShader(m_dwPShader);

    pDevice->SetTexture(2, m_pd3dtcNormal);
    pDevice->SetTexture(3, m_pd3dtcNormal);

	// Draw balls
    for (i = m_maxBalls; i < m_maxBalls + m_numLights; i++) {
        for (j = 0; j < m_numBalls; j++) {
            m_balls[j].Render(pDevice, &mViewProj, &vEyePos, m_balls[i].GetLight(), &m_dcvAmbient, i - m_maxBalls);
        }
    }
	if(g_TestParams.bSparksAreLights) {
		for(i=0, k=m_sparks.GetNumSparks(); i<k; ++i) {
			for (j = 0; j < m_numBalls; j++) {
    			m_balls[j].Render(pDevice, &mViewProj, &vEyePos, m_sparks.GetLight(i), &m_dcvAmbient, 1);
			}
		}
	}

    // Disable ambient audio for any balls not being drawn
    for (i = m_numBalls; i < m_maxBalls; i++) {
        m_balls[i].DisableAmbientAudio();
    }

    for (i = m_maxBalls + m_numLights; i < m_maxBalls + m_maxLights; i++) {
        m_balls[i].DisableAmbientAudio();
    }

    // Clear the texture
    m_pd3dtText->LockRect(0, &d3dlr, NULL, 0);
    for (i = 0; i < 240; i++) {
        memset((LPBYTE)d3dlr.pBits + i * d3dlr.Pitch, 0, 320 * 4);
    }
    m_pd3dtText->UnlockRect(0);

    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    pDevice->SetVertexShader(FVF_CSCENE_BACKGROUND_VERTEX);
    pDevice->SetPixelShader(NULL);
    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// Draw the sphere
	pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    for (i = m_maxBalls; i < m_maxBalls + m_numLights; i += 8) {

        for (j = 0; j < 8 && j + i < m_maxBalls + m_numLights; j++) {
            memcpy(&light, m_balls[i+j].GetLight(), sizeof(D3DLIGHT8));
            light.Attenuation0 = 0.0f;
            light.Attenuation1 = 0.0f;
            light.Attenuation2 = 2.0f;
            m_pDevice->SetLight(j, &light);
            m_pDevice->LightEnable(j, TRUE);
        }
        for (; j < 8; j++) {
            m_pDevice->LightEnable(j, FALSE);
        }

        m_invertedSphere.Render(pDevice, i - m_maxBalls, m_bDrawWireframe);
    }

	pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Draw lights
    for (i = m_maxBalls; i < m_maxBalls + m_numLights; i++) {
        m_balls[i].Render(pDevice, NULL, NULL, NULL, NULL, 0);
    }

	//draw sparks
    pDevice->SetVertexShader(m_dwVShader2);
	m_sparks.Render(pDevice, &mViewProj);

	// Init matrices
	pDevice->SetTransform(D3DTS_VIEW, &m_identityMatrix);
	pDevice->SetTransform(D3DTS_PROJECTION, &m_identityMatrix);

    WCHAR tempbuffer[256];
    DWORD dwColors[NUM_SELECTIONS];
    for (i=0; i<NUM_SELECTIONS; i++)
    {
        if (g_TestParams.dwDMSelected == i)
            dwColors[i] = 0xFFFFFF00;
        else
            dwColors[i] = 0xFFFFFFFF;
    }

    m_pd3dtText->GetSurfaceLevel(0, &pd3ds);

    //
    // Commit 3D audio
    //

    g_dsound->CommitDeferredSettings();

	//
	// Info that does not get changed
	//
	y = 15;

//	swprintf(buffer, L"FPS: %.2lf (%.2lf)", m_avgFrameRate, m_currentFrameRate);
//	DRAWTEXT(pd3ds, buffer, 25, y, 0, 0xffffffff, 0);
//	y += 10;
	swprintf(buffer, L"Seed: %u", g_TestParams.dwSeed);
    DRAWTEXT(pd3ds, buffer, 25, y, 0, 0xffffffff, 0);
	y += 10;

	swprintf(buffer, L"Sparks: %u", m_sparks.GetNumSparks());
    DRAWTEXT(pd3ds, buffer, 25, y, 0, 0xffffffff, 0);
	y += 10;

	swprintf(buffer, L"Layers: %u", m_numLayers);
    DRAWTEXT(pd3ds, buffer, 25, y, 0, 0xffffffff, 0);
	y += 10;

	swprintf(buffer, L"Triangles: %u", m_numTris);
    DRAWTEXT(pd3ds, buffer, 25, y, 0, 0xffffffff, 0);
	y += 40;

    if (_strcmpi(g_TestParams.szDMHeap, "Tracking") == 0)
    {
        g_music.GetHeapAllocation(&dwNormal, &dwPhysical);
        swprintf(buffer, L"DM Heap (N) = %3.2f MB", float(dwNormal) / 1024.f / 1024.f);
        DRAWTEXT(pd3ds, buffer, 25, y, 0, 0xffffffff, 0);
        y+=10;
        swprintf(buffer, L"DM Heap (P) = %3.2f MB", float(dwPhysical) / 1024.f / 1024.f);
        DRAWTEXT(pd3ds, buffer, 25, y, 0, 0xffffffff, 0);
	    y+=10;
/*
        g_music.GetHeapAllocationBlocks(&dwNormal, &dwPhysical);
        swprintf(buffer, L"DMusic Heap Blocks = %d", dwNormal + dwPhysical);
        DRAWTEXT(pd3ds, buffer, 25, 75, 0, 0xffffffff, 0);
*/
    }
    else if (_strcmpi(g_TestParams.szDMHeap, "FixedUser") == 0)
    {
        g_music.GetHeapAllocation(&dwNormal, &dwPhysical);
        swprintf(buffer, L"DM Heap (N) = %3.2f/%3.2f MB", float(dwNormal) / 1024.f / 1024.f, float(g_TestParams.dwDMFixedHeapSizeNorm)  / 1024.f / 1024.f);
        DRAWTEXT(pd3ds, buffer, 25, y, 0, 0xffffffff, 0);
	    y+=10;
        swprintf(buffer, L"DM Heap (P) = %3.2f/%3.2f MB", float(dwPhysical) / 1024.f / 1024.f, float(g_TestParams.dwDMFixedHeapSizePhys)  / 1024.f / 1024.f);
        DRAWTEXT(pd3ds, buffer, 25, y, 0, 0xffffffff, 0);
	    y+=10;

    }

    swprintf(buffer, L"bDMLoaderThread = %d", g_TestParams.bDMLoaderThread);
    DRAWTEXT(pd3ds, buffer, 25, y, 0, 0xffffffff, 0);
	y += 10;

    swprintf(buffer, L"bDMDoWorkLocally = %d", g_TestParams.bDMDoWorkLocally);
    DRAWTEXT(pd3ds, buffer, 25, y, 0, 0xffffffff, 0);
	y += 10;

    swprintf(buffer, L"dwFileIOSize = %d", g_TestParams.dwFileIOSize);
    DRAWTEXT(pd3ds, buffer, 25, y, 0, 0xffffffff, 0);
	y += 60;

	swprintf(buffer, L"DirectSound Playing: %S", g_TestParams.szBounceSound);
	DRAWTEXT(pd3ds, buffer, 25, y, 0, 0xffffffff, 0); 
	y += 10;


	//
	// Info that does get updated
	//
	y = 15;
	swprintf(buffer, L"Balls: %u / %u", g_TestParams.dwNumBalls, m_maxBalls);
    DRAWTEXT(pd3ds, buffer, 160, y, 0, dwColors[0], 0);
	y += 10;

	swprintf(buffer, L"Lights: %u / %u", g_TestParams.dwNumLights, m_maxLights);
    DRAWTEXT(pd3ds, buffer, 160, y, 0, dwColors[1], 0);
	y += 10;

	swprintf(buffer, L"bDrawWireframe=%u", g_TestParams.bDrawWireframe);
    DRAWTEXT(pd3ds, buffer, 160, y, 0, dwColors[2], 0);
	y += 10;

	swprintf(buffer, L"bRelativeSpeed=%u", g_TestParams.bRelativeSpeed);
    DRAWTEXT(pd3ds, buffer, 160, y, 0, dwColors[3], 0);
	y += 10;

	swprintf(buffer, L"bSparksAreLights=%u", g_TestParams.bSparksAreLights);
    DRAWTEXT(pd3ds, buffer, 160, y, 0, dwColors[4], 0);
	y += 10;

	swprintf(buffer, L"dwSparksPerCollision=%u", g_TestParams.dwSparksPerCollision);
    DRAWTEXT(pd3ds, buffer, 160, y, 0, dwColors[5], 0);
	y += 20;

    g_music.GetSegmentTitleCurrent(tempbuffer);
    swprintf(buffer, L"Playing %s", tempbuffer);
    DRAWTEXT(pd3ds, buffer, 160, y, 0, dwColors[6], 0);
	y += 10;

    swprintf(buffer, L"Master Volume = %d", g_TestParams.lDMMasterVolume);
    DRAWTEXT(pd3ds, buffer, 160, y, 0, dwColors[7], 0);
	y += 10;

    swprintf(buffer, L"Master Tempo Factor = %.2f", g_TestParams.fDMMasterTempo);
    DRAWTEXT(pd3ds, buffer, 160, y, 0, dwColors[8], 0);
	y += 10;

    swprintf(buffer, L"Master Transpose = %d", g_TestParams.lDMMasterTranspose);
    DRAWTEXT(pd3ds, buffer, 160, y, 0, dwColors[9], 0);
	y += 10;

    DWORD dwTemp;
    g_music.GetReverb(&dwTemp);
    if (dwTemp != 0xFFFFFFFF)
        swprintf(buffer, L"Reverb Send Level = %d", dwTemp);
    else
        swprintf(buffer, L"Reverb Send Level = Default");
    DRAWTEXT(pd3ds, buffer, 160, y, 0, dwColors[10], 0);
	y += 20;

	swprintf(buffer, L"DirectSound Volume = %d", g_TestParams.lDSVolume);
	DRAWTEXT(pd3ds, buffer, 160, y, 0, dwColors[11], 0);
	y += 10;

	swprintf(buffer, L"DirectSound Frequency = %u", g_TestParams.dwDSFreq);
	DRAWTEXT(pd3ds, buffer, 160, y, 0, dwColors[12], 0);
	y += 10;

	// Display the current run time
	currentTime	= m_lastTime - m_startTime;
	hours		= (int)(currentTime / 3600.0);
	currentTime	-= (3600.0 * hours);
	minutes		= (int)(currentTime / 60.0);
	currentTime	-= (60.0 * minutes);
	seconds		= (int)currentTime;
	currentTime	-= (double)seconds;
	fraction	= (int)(currentTime * 100);

	swprintf (buffer,L"Time: %02d:%02d:%02d.%02d ", hours, minutes, seconds, fraction);
	m_font.DrawText(pd3ds, buffer, 25, 215, 0, 0xffffffff, 0);

	// Display the frame rate
	swprintf(buffer, L"FPS: %.2lf", m_avgFrameRate);
    m_font.DrawText(pd3ds, buffer, 240, 215, 0, 0xffffffff, 0);

	pd3ds->Release();

	pDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);

	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

#if 0
	pDevice->SetTextureStageState(0, D3DTSS_COLORKEYOP, D3DTCOLORKEYOP_RGBA);
	pDevice->SetTextureStageState(0, D3DTSS_COLORKEYCOLOR, 0);
#else
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
#endif

	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	pDevice->SetTexture(0, m_pd3dtText);

	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prText, sizeof(TVertex));

	pDevice->SetTexture(0, NULL);
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

#if 0
	pDevice->SetTextureStageState(0, D3DTSS_COLORKEYOP, D3DTCOLORKEYOP_DISABLE);
#else
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
#endif

	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    // End the scene
    pDevice->EndScene();

    // Update the screen
    pDevice->Present(NULL, NULL, NULL, NULL);

	// Calculate the frame rate
	QueryPerformanceCounter((LARGE_INTEGER*)&timer);
	currentTime		= (double)timer / m_frequency;
	dt				= currentTime - m_lastTime;
	m_elapsedTime	+= dt;

	// Current frame rate
	m_currentFrameRate = 1.0 / dt;

	// Cycle flag
	bCycle = ((DWORD)((currentTime - m_lastCycle)  * 1000.0) >= g_TestParams.dwCycleTime) ? TRUE : FALSE;

	m_lastTime = currentTime;

	// Frame counter
	++m_frame;

	// Average frame rate calculations
	if(m_elapsedTime > 1.0)
	{
		m_avgFrameRate	= (double)m_frame / m_elapsedTime;
		m_frame			= 0;
		m_elapsedTime	= 0.0;
	}

	// Draw rate
	if(m_bRelativeSpeed)
		m_drawRate = 1.0 / (currentTime - ((double)start / m_frequency));

    if (g_TestParams.bCycleDisplayModes && bCycle) //!(m_frame % 480))
	{
		m_lastCycle = currentTime;
        CycleDisplayMode(); // Reset the display mode
	}
}

//------------------------------------------------------------------------------
//	CScene::CycleDisplayMode
//------------------------------------------------------------------------------
void
CScene::CycleDisplayMode()
{
    D3DPRESENT_PARAMETERS d3dpp;
    float fAspectWidth, fAspectHeight, fFieldOfView;
    HRESULT hr;

    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.BackBufferWidth					= m_pd3ddm[m_uDisplayMode].Width;
    d3dpp.BackBufferHeight					= m_pd3ddm[m_uDisplayMode].Height;
    d3dpp.BackBufferFormat					= m_pd3ddm[m_uDisplayMode].Format;
    d3dpp.BackBufferCount					= g_TestParams.dwBackBufferCount;
    d3dpp.Flags								= m_pd3ddm[m_uDisplayMode].Flags;
    d3dpp.MultiSampleType					= g_TestParams.dwMultiSampleType;
    d3dpp.SwapEffect						= D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow						= NULL;
    d3dpp.Windowed							= FALSE;
    d3dpp.EnableAutoDepthStencil			= TRUE;
    d3dpp.AutoDepthStencilFormat			= D3DFMT_LIN_D24S8;
    d3dpp.FullScreen_RefreshRateInHz		= m_pd3ddm[m_uDisplayMode].RefreshRate;

    if (m_pd3ddm[m_uDisplayMode].Flags & D3DPRESENTFLAG_FIELD) {
        d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    }
    else {
        d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    }

//    m_pDevice->PersistDisplay();

    Log("Resetting the device to mode %d:\n", m_uDisplayMode);
    Log("    %d x %d @ %d Hz\n", m_pd3ddm[m_uDisplayMode].Width, m_pd3ddm[m_uDisplayMode].Height, m_pd3ddm[m_uDisplayMode].RefreshRate);
    switch (m_pd3ddm[m_uDisplayMode].Format) {
        case D3DFMT_LIN_A8R8G8B8:
            Log("    D3DFMT_LIN_A8R8G8B8\n");
            break;
        case D3DFMT_LIN_X8R8G8B8:
            Log("    D3DFMT_LIN_X8R8G8B8\n");
            break;
        case D3DFMT_LIN_R5G6B5:
            Log("    D3DFMT_LIN_R5G6B5\n");
            break;
        case D3DFMT_LIN_X1R5G5B5:
            Log("    D3DFMT_LIN_X1R5G5B5\n");
            break;
    }
    if (m_pd3ddm[m_uDisplayMode].Flags & D3DPRESENTFLAG_WIDESCREEN) {
        Log("    D3DPRESENTFLAG_WIDESCREEN\n");
    }
    if (m_pd3ddm[m_uDisplayMode].Flags & D3DPRESENTFLAG_INTERLACED) {
        Log("    D3DPRESENTFLAG_INTERLACED\n");
    }
    if (m_pd3ddm[m_uDisplayMode].Flags & D3DPRESENTFLAG_PROGRESSIVE) {
        Log("    D3DPRESENTFLAG_PROGRESSIVE\n");
    }
    if (m_pd3ddm[m_uDisplayMode].Flags & D3DPRESENTFLAG_FIELD) {
        Log("    D3DPRESENTFLAG_FIELD\n");
    }
    if (m_pd3ddm[m_uDisplayMode].Flags & D3DPRESENTFLAG_10X11PIXELASPECTRATIO) {
        Log("    D3DPRESENTFLAG_10X11PIXELASPECTRATIO\n");
    }
    Log("\n");

    hr = m_pDevice->Reset(&d3dpp);
    if (FAILED(hr)) {
        __asm int 3;
    }

    fFieldOfView = 45.0f;
    fAspectWidth = (float)(m_pd3ddm[m_uDisplayMode].Width);
    fAspectHeight = (float)(m_pd3ddm[m_uDisplayMode].Height);
    if (m_pd3ddm[m_uDisplayMode].Flags & D3DPRESENTFLAG_FIELD) {
        fAspectHeight *= 2.0f;
    }
    if (m_pd3ddm[m_uDisplayMode].Flags & D3DPRESENTFLAG_10X11PIXELASPECTRATIO) {
        fFieldOfView *= 0.90909f;
        fAspectWidth *= 1.1f;
    }
    if (m_pd3ddm[m_uDisplayMode].Flags & D3DPRESENTFLAG_WIDESCREEN) {
        fFieldOfView *= 1.33333f;
    }
    if (m_pd3ddm[m_uDisplayMode].Width == 720) {
        fFieldOfView *= 1.125f;
    }

    // Update the projection matrix, etc. as specified by the display mode flags
	m_camera.SetPerspectiveFov(fFieldOfView, fAspectWidth / fAspectHeight, 0.99f * m_radius, 3.01f * m_radius);

    // Update the text quad
    if (m_pd3ddm[m_uDisplayMode].Flags & D3DPRESENTFLAG_FIELD) {
        m_prText[0].y = 240.0f;
        m_prText[3].y = 240.0f;
    }
    else {
        m_prText[0].y = 480.0f;
        m_prText[3].y = 480.0f;
    }

    if (++m_uDisplayMode >= m_uNumDisplayModes) {
        m_uDisplayMode = 0;
    }
}

//------------------------------------------------------------------------------
//	CScene::IntersectRaySphere
//------------------------------------------------------------------------------
BOOL
CScene::IntersectRaySphere(
						   IN const XGVECTOR3&	rayU, 
						   IN const XGVECTOR3&	rayV, 
						   IN const XGVECTOR3&	center,
						   IN float				radius,
						   OUT XGVECTOR3&		point
						   )
/*++

Routine Description:

	Calculates the intersection of a sphere and a line segment

Arguments:

	IN rayU -	Ray origin
	IN rayV -	Ray direction	(must be normalized)
	IN center -	Center of sphere
	IN radius -	Radius of sphere
	OUT point -	Point of intersection

Return Value:

	TRUE if the ray intersects the sphere, FALSE otherwise

--*/
{
	XGVECTOR3	uc(center - rayU);
	float		v = XGVec3Dot(&uc, &rayV);
	float		d = (radius * radius) - (XGVec3Dot(&uc, &uc) - (v * v));

	if(d < 0.0f)
		return FALSE;

	d = (float)sqrt(d);
	point = rayU + (rayV * (v - d));
	return TRUE;
}

//------------------------------------------------------------------------------
//	CScene::IntersectRayInSphere
//------------------------------------------------------------------------------
BOOL
CScene::IntersectRayInSphere(
						     IN const XGVECTOR3&	rayU, 
						     IN const XGVECTOR3&	rayV, 
						     IN const XGVECTOR3&	center,
						     IN float				radius,
						     OUT XGVECTOR3&			point
						     )
/*++

Routine Description:

	Calculates the intersection of a sphere and a line segment within the sphere

Arguments:

	IN rayU -	Ray origin
	IN rayV -	Ray direction	(must be normalized)
	IN center -	Center of sphere
	IN radius -	Radius of sphere
	OUT point -	Point of intersection

Return Value:

	TRUE if the ray intersects the sphere, FALSE otherwise

--*/
{
	XGVECTOR3	p;
	float		d;

	// Point on line closest to center of sphere
	p	= rayU + (rayV * (-(-XGVec3Dot(&center, &rayV) + XGVec3Dot(&rayU, &rayV)) / 
		  XGVec3Dot(&rayV, &rayV)));

	// Distance from center of sphere to point
	d	= XGVec3Length(&XGVECTOR3(center - p));
	
	// Is the point outside of the sphere?
	if(d > radius)
		return FALSE;

	// Distance from point to point on sphere edge
	d = (float)sqrt((radius * radius) - (d * d));

	// Point on sphere
	point = p + (rayV * d);

	return TRUE;
}

//------------------------------------------------------------------------------
//	CScene::GetSphereIntersection
//------------------------------------------------------------------------------
BOOL
CScene::GetSphereIntersection(
							  IN const XGVECTOR3&	s0,
							  IN float				s0r,
							  IN const XGVECTOR3&	v,
							  IN const XGVECTOR3&	s1,
							  IN float				s1r,
							  OUT float&			t
							  )
/*++

Routine Description:

	Calculates the intersection of two sphere. Sphere 0 is moving in the 
	direction v. Sphere 1 is stationary. For the complete derivation of
	this equation, see robheit

Arguments:

	IN s0 -		Starting location of sphere 0
	IN s0r -	Radius of sphere 0
	IN v -		Direction of travel for sphere 0
	IN s1 -		Location of sphere 1
	IN s1r -	Radius of sphere 1
	OUT t -		Distance along path the sphere touch

Return Value:

	TRUE if the spheres will touch, FALSE otherwise

--*/
{
	float	a;
	float	b;
	float	c;
	float	d;
//	float	t1;

	// Quick dot product check
	if(XGVec3Dot(&v, &XGVECTOR3(s1 - s0)) <= 0.0f)
		return FALSE;

	a = ((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	b = 2.0f * ((s0.x * v.x) - (s1.x * v.x) + (s0.y * v.y) - 
		        (s1.y * v.y) + (s0.z * v.z) - (s1.z * v.z));
	c = ((s0r + s1r) * (s0r + s1r)) - (s0.x * s0.x) + 
		(2.0f * ((s1.x * s0.x) + (s1.y * s0.y) + (s1.z * s0.z))) - 
		(s1.x * s1.x) - (s0.y * s0.y) - (s1.y * s1.y) - (s0.z * s0.z) - 
		(s1.z * s1.z);

	if(a != 0.0f) {
		d = (b * b) + 4.0f * a * c;
		if(d >= 0.0f) {
			d = (float)sqrt(d);
			t = (-d - b) / (2.0f * a);
			if(t < 0.000001f)
				t = 0.0f;

			// Second solution to quadratic equation
			//t1 = (d - b) / (2.0f * a);
			return TRUE;
		}
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	CScene::RandomColor
//------------------------------------------------------------------------------
DWORD
CScene::RandomColor(void)
/*++

Routine Description:

	Returns a random color

Arguments:

	None

Return Value:

	A random color 0xaarrggbb

--*/
{
	DWORD	a = (DWORD)(rand() % 0xff);
	DWORD	r = (DWORD)(rand() % 0xff);
	DWORD	g = (DWORD)(rand() % 0xff);
	DWORD	b = (DWORD)(rand() % 0xff);

	return (a << 24) | (r << 16) | (g << 8) | b;
}

//------------------------------------------------------------------------------
//	CScene::BallHitBall
//------------------------------------------------------------------------------
void 
CScene::BallHitBall(
					IN UINT	ball0,
					IN UINT ball1
					)
/*++

Routine Description:

	Called whenever 2 balls touch

Arguments:

	IN ball0 -	First ball (0 - m_numBalls-1)
	IN ball1 -	Second ball (0 - m_numBalls-1)

Return Value:

	None

--*/
{
	//
	// play the audio buffer on one of the balls that was hit
	//
	for(UINT i = 0; i < g_TestParams.dwSparksPerCollision; i++) {
		m_sparks.AddSpark((m_balls[ball0].GetLocation() + m_balls[ball1].GetLocation()) / 2);
	}

    m_balls[ball0].PlayBuffer((MAX2(m_balls[ball0].GetSpeed(), m_balls[ball1].GetSpeed()) - m_currentMinSpeed) / m_dSpeed,
							   ADJUST(XGVec3Dot(&m_balls[ball0].GetDirection(), &m_balls[ball1].GetDirection())));


    //
    // Set up an async file write
    //
    if(g_TestParams.dwFileIOSize != 0)
        {
        FileIOHandle *info = new FileIOHandle;
        OVERLAPPED *overlapped = new OVERLAPPED;
        if(!info || !overlapped)
            {
            delete info;
            delete overlapped;
            return;
            }

        info->buffSize = g_TestParams.dwFileIOSize;
        info->buffer = new char[info->buffSize];
        if(!info->buffer)
            {
            delete info;
            delete overlapped;
            return;
            }

        // open the initial file if necessary
        if(m_hFile == INVALID_HANDLE_VALUE)
            {
            m_hFile = CreateFile("t:\\randomdata.dat", GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
            SetFilePointer(m_hFile, (m_numBalls+1) * 4096, NULL, FILE_BEGIN);
            SetEndOfFile(m_hFile);
            }

        overlapped->Internal = 0;
        overlapped->InternalHigh = 0;
        overlapped->Offset = ball0*info->buffSize + ball1;
        overlapped->OffsetHigh = 0;
        overlapped->hEvent = info;

        for(unsigned i=0; i<info->buffSize; i++)
            {
            info->buffer[i] = (char)i;
            }

        if(WriteFileEx(m_hFile, info->buffer, info->buffSize, overlapped, CompleteIO) == 0)
            {
            // error
            }
        SleepEx(15000, true); // allows the CompleteIO function to be called
        }

}

//------------------------------------------------------------------------------
//	CScene::BallHitWall
//------------------------------------------------------------------------------
void 
CScene::BallHitWall(
					IN UINT				ball,
					IN const XGVECTOR3&	normal
					)
/*++

Routine Description:

	Called whenever a ball hits the wall

Arguments:

	IN ball -	Ball (0 - m_numBalls-1)
	IN normal -	Normal at point on wall that was hit

Return Value:

	None

--*/
{
	//
	// play the audio buffer on the ball that was hit
	//
    m_balls[ball].PlayBuffer((m_balls[ball].GetSpeed() - m_currentMinSpeed) / m_dSpeed,
							 ADJUST(XGVec3Dot(&m_balls[ball].GetDirection(), &normal)));
}

//------------------------------------------------------------------------------
//	CScene::BallSpeedChanged
//------------------------------------------------------------------------------
void 
CScene::BallSpeedChanged(
						 IN UINT	ball,
						 IN float	oldSpeed,
						 IN float	newSpeed
						 )
/*++

Routine Description:

	Called whenever a ball's speed changes

Arguments:

	IN ball -		Ball (0 - m_numBalls-1)
	IN oldSpeed -	Old speed
	IN newSpeed -	New speed

Return Value:

	None

--*/
{
}

//------------------------------------------------------------------------------
//	CScene::BallDirectionChanged
//------------------------------------------------------------------------------
void 
CScene::BallDirectionChanged(
							 IN UINT				ball,
							 IN const XGVECTOR3&	oldDirection,
							 IN const XGVECTOR3&	newDirection
							 )
/*++

Routine Description:

	Called whenever a ball's direction changes

Arguments:

	IN ball -			Ball (0 - m_numBalls-1)
	IN oldDirection -	Old direction
	IN newDirection -	New direction

Return Value:

	None

--*/
{
}

//------------------------------------------------------------------------------
//	CScene::AddBall
//------------------------------------------------------------------------------
void
CScene::AddBall(void)
/*++

Routine Description:

	Adds a ball to the scene (up to m_maxBalls)

Arguments:

	None

Return Value:

	None

--*/
{
	BOOL		goodLoc;
	XGVECTOR3	loc;
	XGVECTOR3	dir;
	float		speed;
	UINT		i;

	// Don't add past the end of the array
	if(m_numBalls < m_maxBalls)
	{
		// Find a good place to insert the ball
		// (Make sure the new ball is not within 1 unit of another)
		do 
		{
			goodLoc = TRUE;
			loc.x = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			loc.y = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			loc.z = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			for(i=0; goodLoc && i<m_numBalls; ++i) 
			{
				if(XGVec3Length(&XGVECTOR3(loc - m_balls[i].GetLocation())) <= m_balls[m_numBalls].GetRadius() + m_balls[i].GetRadius())
					goodLoc = FALSE;
			}
			for(i=m_maxBalls; goodLoc && i<m_maxBalls + m_numLights; ++i) 
			{
				if(XGVec3Length(&XGVECTOR3(loc - m_balls[i].GetLocation())) <= m_balls[m_numBalls].GetRadius() + m_balls[i].GetRadius())
					goodLoc = FALSE;
			}
		}
		while(!goodLoc);

		dir.x = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
		dir.y = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
		dir.z = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
		XGVec3Normalize(&dir, &dir);
		speed = (float)rand() / (float)RAND_MAX * (m_maxSpeed - m_minSpeed) + m_minSpeed;

		
		// Set the new balls information
		m_balls[m_numBalls].SetPosition(loc);
		m_balls[m_numBalls].SetDirection(dir);
		m_balls[m_numBalls].SetSpeed(speed);

		m_numTris += m_balls[m_numBalls].GetNumTris();

		// Increase the number of balls
		++m_numBalls;

		RecalculateSpeeds();
	}
}

//------------------------------------------------------------------------------
//	CScene::RemoveBall
//------------------------------------------------------------------------------
void
CScene::RemoveBall(void)
/*++

Routine Description:

	Removes a ball from the scene

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_numBalls > 0)
	{
		--m_numBalls;
		m_numTris -= m_balls[m_numBalls].GetNumTris();
		RecalculateSpeeds();
	}
}

//------------------------------------------------------------------------------
//	CScene::AddBall
//------------------------------------------------------------------------------
void
CScene::AddLight(void)
/*++

Routine Description:

	Adds a light to the scene (up to m_maxLights)

Arguments:

	None

Return Value:

	None

--*/
{
	BOOL		goodLoc;
	XGVECTOR3	loc;
	XGVECTOR3	dir;
	float		speed;
	UINT		i;

	// Don't add past the end of the array
	if(m_numLights < m_maxLights)
	{
		// Find a good place to insert the light
		// (Make sure the new light is not within 1 unit of another)
		do 
		{
			goodLoc = TRUE;
			loc.x = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			loc.y = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			loc.z = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			for(i=0; goodLoc && i<m_numBalls; ++i) 
			{
				if(XGVec3Length(&XGVECTOR3(loc - m_balls[i].GetLocation())) <= m_balls[m_maxBalls + m_numLights].GetRadius() + m_balls[i].GetRadius())
					goodLoc = FALSE;
			}
			for(i=m_maxBalls; goodLoc && i<m_maxBalls + m_numLights; ++i) 
			{
				if(XGVec3Length(&XGVECTOR3(loc - m_balls[i].GetLocation())) <= m_balls[m_maxBalls + m_numLights].GetRadius() + m_balls[i].GetRadius())
					goodLoc = FALSE;
			}
		}
		while(!goodLoc);

		dir.x = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
		dir.y = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
		dir.z = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
		XGVec3Normalize(&dir, &dir);
		speed = (float)rand() / (float)RAND_MAX * (m_maxSpeed - m_minSpeed) + m_minSpeed;

		
		// Set the new balls information
		m_balls[m_maxBalls + m_numLights].SetPosition(loc);
		m_balls[m_maxBalls + m_numLights].SetDirection(dir);
		m_balls[m_maxBalls + m_numLights].SetSpeed(speed);

		m_numTris += m_balls[m_maxBalls + m_numLights].GetNumTris();

		// Increase the number of balls
		++m_numLights;

		RecalculateSpeeds();
	}
}

//------------------------------------------------------------------------------
//	CScene::RemoveLight
//------------------------------------------------------------------------------
void
CScene::RemoveLight(void)
/*++

Routine Description:

	Removes a ball from the scene

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_numLights > 0)
	{
		--m_numLights;
		m_numTris -= m_balls[m_maxBalls + m_numLights].GetNumTris();
		RecalculateSpeeds();
	}
}

//------------------------------------------------------------------------------
//	CScene::CollideBalls
//------------------------------------------------------------------------------
void 
CScene::CollideBalls(
					 IN const XGVECTOR3&	locA,
					 IN OUT XGVECTOR3&		dirA, 
					 IN OUT float&			speedA, 
					 IN float				massA,
					 IN const XGVECTOR3&	locB,
					 IN OUT XGVECTOR3&		dirB, 
					 IN OUT float&			speedB,
					 IN float				massB
					 )
/*++

Routine Description:

	Collides two balls and adjusts their velocities

Arguments:

	IN locA -		Location of ball A
	IN OUT dirA -	Direction of Ball A
	IN OUT speedA -	Speed of Ball A
	IN massA -		Mass of Ball A
	IN locB -		Location of ball B
	IN OUT dirB -	Direction of Ball B
	IN OUT speedB -	Speed of Ball B
	IN massB -		Mass of Ball B
	
Return Value:

	None

--*/
{
	XGVECTOR3	AB;
	XGVECTOR3	BA;
	float		dot;
	XGVECTOR3	momentumAB;	// Momentum of A in the direction of B
	XGVECTOR3	momentumBA;	// Momentum of B in the direction of A
	float		totalMomentum = (speedA * massA) + (speedB * massB);
	float		mass = MIN2(massA, massB);

	// Vector of interection
	AB = locB - locA;
	XGVec3Normalize(&AB, &AB);
	BA = -AB;

	// Momentum of A in the direction of B
	dot = XGVec3Dot(&AB, &dirA);
	if(dot > 0.0f)
		momentumAB = AB * speedA * mass * dot;
	else
	{
		momentumAB.x = 0.0f;
		momentumAB.y = 0.0f;
		momentumAB.z = 0.0f;
	}

	// Momentum of B in the direction of A
	dot = XGVec3Dot(&BA, &dirB);
	if(dot > 0.0f)
		momentumBA = BA * speedB * mass * dot;
	else
	{
		momentumBA.x = 0.0f;
		momentumBA.y = 0.0f;
		momentumBA.z = 0.0f;
	}

	// Calculate the momentum of each ball
	dirA = (dirA * speedA * massA) - momentumAB + momentumBA;
	dirB = (dirB * speedB * massB) - momentumBA + momentumAB;

	// Calculate the speed (magnitude of the momentum vector)
	speedA = XGVec3Length(&dirA);
	speedB = XGVec3Length(&dirB);

	// Floating point error remover
	// At this point the following should be true: momentumA + momentumB == totalMomentum
	// but do to floating point error probably will not be
	speedA	= (speedA / (speedA + speedB)) * totalMomentum;
	speedB	= (totalMomentum - speedA) / massB;
	speedA	/= massA;

	// Normalize the direction vectors
	XGVec3Normalize(&dirA, &dirA);
	XGVec3Normalize(&dirB, &dirB);
}

//******************************************************************************
HRESULT CScene::CreateShaders(IDirect3DDevice8* pDevice) {

    LPXGBUFFER          pxgbufShader;
    HANDLE              hFile;
    DWORD               dwRead;
    D3DPIXELSHADERDEF   d3dpsd;
    DWORD               dwSize;
    LPBYTE              pData;
    HRESULT             hr;

    DWORD dwSDecl[] = {
            D3DVSD_STREAM(0),
            D3DVSD_REG(0, D3DVSDT_FLOAT3),
            D3DVSD_REG(1, D3DVSDT_FLOAT2),
            D3DVSD_REG(2, D3DVSDT_FLOAT3),
            D3DVSD_REG(3, D3DVSDT_FLOAT3),
            D3DVSD_REG(4, D3DVSDT_FLOAT3),
            D3DVSD_END()
    };
    DWORD dwSDecl2[] = {
            D3DVSD_STREAM(0),
            D3DVSD_REG(0, D3DVSDT_FLOAT3),
            D3DVSD_REG(1, D3DVSDT_FLOAT1),
            D3DVSD_END()
    };

    hFile = CreateFile(g_TestParams.szLightVSH, GENERIC_READ, FILE_SHARE_READ, NULL, 
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        return E_FAIL;
    }

    // Get the size of the file
    dwSize = GetFileSize(hFile, NULL);
    if (dwSize == 0xFFFFFFFF) {
        return E_FAIL;
    }

    // Allocate a buffer for the file data
    pData = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, dwSize);
    if (!pData) {
        CloseHandle(hFile);
        return E_FAIL;
    }                

    // Read the file into the buffer
    if (!ReadFile(hFile, pData, dwSize, &dwRead, NULL)
        || dwSize != dwRead)
    {
        HeapFree(GetProcessHeap(), 0, pData);
        CloseHandle(hFile);
        return E_FAIL;
    }

    // Close the file
    CloseHandle(hFile);

    hr = AssembleShader("", pData, dwSize, 0, NULL, 
                        &pxgbufShader, NULL, NULL, NULL, NULL, NULL);
    if (FAILED(hr)) {
        HeapFree(GetProcessHeap(), 0, pData);
        return hr;
    }

    hr = pDevice->CreateVertexShader(dwSDecl, (LPDWORD)pxgbufShader->GetBufferPointer(), &m_dwVShader, 0);
//	m_dwVShaderAddress = pDevice->GetVertexShaderSize(&dwVShader);

    pxgbufShader->Release();
    HeapFree(GetProcessHeap(), 0, pData);

    if (FAILED(hr)) {
        return E_FAIL;
    }


    hFile = CreateFile(g_TestParams.szSparkVSH, GENERIC_READ, FILE_SHARE_READ, NULL, 
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        return E_FAIL;
    }

    // Get the size of the file
    dwSize = GetFileSize(hFile, NULL);
    if (dwSize == 0xFFFFFFFF) {
        return E_FAIL;
    }

    // Allocate a buffer for the file data
    pData = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, dwSize);
    if (!pData) {
        CloseHandle(hFile);
        return E_FAIL;
    }                

    // Read the file into the buffer
    if (!ReadFile(hFile, pData, dwSize, &dwRead, NULL)
        || dwSize != dwRead)
    {
        HeapFree(GetProcessHeap(), 0, pData);
        CloseHandle(hFile);
        return E_FAIL;
    }

    // Close the file
    CloseHandle(hFile);

    hr = AssembleShader("", pData, dwSize, 0, NULL, 
                        &pxgbufShader, NULL, NULL, NULL, NULL, NULL);
    if (FAILED(hr)) {
        HeapFree(GetProcessHeap(), 0, pData);
        return hr;
    }

    hr = pDevice->CreateVertexShader(dwSDecl2, (LPDWORD)pxgbufShader->GetBufferPointer(), &m_dwVShader2, 0);
//	m_dwVShaderAddress2 = m_dwVShaderAddress + pDevice->GetVertexShaderSize(&dwVShader2);

    pxgbufShader->Release();
    HeapFree(GetProcessHeap(), 0, pData);

    if (FAILED(hr)) {
        return E_FAIL;
    }

//	pDevice->LoadVertexShader(m_dwVShader, m_dwVShaderAddress);
//	pDevice->LoadVertexShader(m_dwVShader2, m_dwVShaderAddress2);


    // Complete light with specular power of 16
    memset(&d3dpsd, 0, sizeof(D3DPIXELSHADERDEF));
    d3dpsd.PSCombinerCount = PS_COMBINERCOUNT(4, PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
    d3dpsd.PSTextureModes = PS_TEXTUREMODES(PS_TEXTUREMODES_PROJECT2D, 
                                            PS_TEXTUREMODES_PROJECT2D, 
                                            PS_TEXTUREMODES_CUBEMAP, 
                                            PS_TEXTUREMODES_CUBEMAP);

    d3dpsd.PSRGBInputs[0] = PS_COMBINERINPUTS(
                                PS_REGISTER_T1 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB,
                                PS_REGISTER_T2 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB,
                                PS_REGISTER_T1 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB,
                                PS_REGISTER_T3 | PS_INPUTMAPPING_EXPAND_NORMAL | PS_CHANNEL_RGB);

    d3dpsd.PSRGBOutputs[0] = PS_COMBINEROUTPUTS(PS_REGISTER_R0, PS_REGISTER_R1, PS_REGISTER_DISCARD,
                                PS_COMBINEROUTPUT_AB_DOT_PRODUCT | PS_COMBINEROUTPUT_CD_DOT_PRODUCT |
                                PS_COMBINEROUTPUT_CD_BLUE_TO_ALPHA);

    d3dpsd.PSRGBInputs[1] = PS_COMBINERINPUTS(
                                PS_REGISTER_V0 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_C0 | PS_CHANNEL_RGB,
                                PS_REGISTER_V0 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_C1 | PS_CHANNEL_RGB);

    d3dpsd.PSRGBOutputs[1] = PS_COMBINEROUTPUTS(PS_REGISTER_T1, PS_REGISTER_T2, PS_REGISTER_DISCARD, 0);

    d3dpsd.PSAlphaInputs[1] = PS_COMBINERINPUTS(
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA);

    d3dpsd.PSAlphaOutputs[1] = PS_COMBINEROUTPUTS(PS_REGISTER_R1, PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, 0);

    d3dpsd.PSRGBInputs[2] = PS_COMBINERINPUTS(
                                PS_REGISTER_V0 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_C0 | PS_CHANNEL_RGB,
                                PS_REGISTER_R0 | PS_CHANNEL_RGB,
                                PS_REGISTER_T1 | PS_CHANNEL_RGB);

    d3dpsd.PSRGBOutputs[2] = PS_COMBINEROUTPUTS(PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, PS_REGISTER_R0, 0);

    d3dpsd.PSAlphaInputs[2] = PS_COMBINERINPUTS(
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA);

    d3dpsd.PSAlphaOutputs[2] = PS_COMBINEROUTPUTS(PS_REGISTER_R1, PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, 0);

    d3dpsd.PSRGBInputs[3] = PS_COMBINERINPUTS(
                                PS_REGISTER_T0 | PS_CHANNEL_RGB,
                                PS_REGISTER_C0 | PS_CHANNEL_RGB,
                                PS_REGISTER_T0 | PS_CHANNEL_RGB,
                                PS_REGISTER_R0 | PS_CHANNEL_RGB);

    d3dpsd.PSRGBOutputs[3] = PS_COMBINEROUTPUTS(PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, PS_REGISTER_R0, 0);

    d3dpsd.PSAlphaInputs[3] = PS_COMBINERINPUTS(
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA,
                                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA);

    d3dpsd.PSAlphaOutputs[3] = PS_COMBINEROUTPUTS(PS_REGISTER_R1, PS_REGISTER_DISCARD, PS_REGISTER_DISCARD, 0);

    d3dpsd.PSFinalCombinerInputsABCD = PS_COMBINERINPUTS(
                                PS_REGISTER_T2 | PS_CHANNEL_RGB, 
                                PS_REGISTER_EF_PROD | PS_CHANNEL_RGB,
                                PS_REGISTER_ZERO | PS_CHANNEL_RGB,
                                PS_REGISTER_R0 | PS_CHANNEL_RGB);

    d3dpsd.PSFinalCombinerInputsEFG = PS_COMBINERINPUTS(
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_R1 | PS_CHANNEL_ALPHA,
                                PS_REGISTER_T0 | PS_CHANNEL_ALPHA,
                                0);

    hr = pDevice->CreatePixelShader(&d3dpsd, &m_dwPShader);
    if (FAILED(hr)) {
        return hr;
    }

    return TRUE;
}

//******************************************************************************
void CScene::ReleaseShaders() {

    if (m_dwVShader) {
        m_pDevice->DeleteVertexShader(m_dwVShader);
    }
    if (m_dwPShader) {
        m_pDevice->DeletePixelShader(m_dwPShader);
    }
}

//******************************************************************************
//
// Function:
//
//     CreateNormalMap
//
// Description:
//
//     Create a cube map and populate it with normal vector information.
//
// Arguments:
//
//     IDirect3DDevice8* pDevice       - Pointer to the device object
//
//     UINT uLength                    - Dimensions of the map
//
//     float fBumpDelta                - Maximum component offset to randomly 
//                                       perturb the surface normals
//
//     D3DFORMAT fmt                   - Format of the texture
//
//     BOOL bMipmap                    - Indicates whether or not to
//                                       use a mip map
//
//     D3DPOOL pool                    - Memory pool in which to create the map
//
// Return Value:
//
//     A pointer to the created diffuse map on success, NULL on failure.
//
//******************************************************************************
HRESULT CScene::CreateNormalMap(IDirect3DDevice8* pDevice, UINT uLength, UINT uLevels, IDirect3DCubeTexture8** ppd3dtc)
{
    IDirect3DCubeTexture8* pd3dtc;
    Swizzler        swz(uLength, uLength, 1);
    D3DLOCKED_RECT  d3dlr;
    LPDWORD         pdwPixel;
    XGVECTOR3       vDir;
    float           fU, fV;
    UINT            i, j, u, v;
    HRESULT         hr;

    if (!ppd3dtc) {
        return E_FAIL;
    }

    *ppd3dtc = NULL;

    hr = pDevice->CreateCubeTexture(uLength, uLevels, 0, D3DFMT_X8R8G8B8, 0, &pd3dtc);
    if (FAILED(hr)) {
        return hr;
    }

    uLevels = pd3dtc->GetLevelCount();

    // Generate the normals
    for (i = 0; i < uLevels; i++) {

        for (j = 0; j < 6; j++) {

            pd3dtc->LockRect((D3DCUBEMAP_FACES)j, i, &d3dlr, NULL, 0);

            pdwPixel = (LPDWORD)d3dlr.pBits;

            if (uLevels > 1) {
                uLength = 0x1 << ((uLevels - 1) - i);
            }

            Swizzler swz(uLength, uLength, 1);
            swz.SetU(0);
            swz.SetV(0);

            for (v = 0; v < uLength; v++) {

                for (u = 0; u < uLength; u++) {

                    if (uLength > 1) {
                        fU = (float)u / (float)(uLength - 1);
                        fV = (float)v / (float)(uLength - 1);
                    }
                    else {
                        fU = 0.5f;
                        fV = 0.5f;
                    }

                    switch ((D3DCUBEMAP_FACES)j) {
                        case D3DCUBEMAP_FACE_POSITIVE_X:
                            XGVec3Normalize(&vDir, &XGVECTOR3(1.0f, 1.0f - (fV * 2.0f), 1.0f - (fU * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_NEGATIVE_X:
                            XGVec3Normalize(&vDir, &XGVECTOR3(-1.0f, 1.0f - (fV * 2.0f), -1.0f + (fU * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_POSITIVE_Y:
                            XGVec3Normalize(&vDir, &XGVECTOR3(-1.0f + (fU * 2.0f), 1.0f, -1.0f + (fV * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_NEGATIVE_Y:
                            XGVec3Normalize(&vDir, &XGVECTOR3(-1.0f + (fU * 2.0f), -1.0f, 1.0f - (fV * 2.0f)));
                            break;
                        case D3DCUBEMAP_FACE_POSITIVE_Z:
                            XGVec3Normalize(&vDir, &XGVECTOR3(-1.0f + (fU * 2.0f), 1.0f - (fV * 2.0f), 1.0f));
                            break;
                        case D3DCUBEMAP_FACE_NEGATIVE_Z:
                            XGVec3Normalize(&vDir, &XGVECTOR3(1.0f - (fU * 2.0f), 1.0f - (fV * 2.0f), -1.0f));
                            break;
                    }
                    vDir = -vDir;
				    XGVec3Normalize(&vDir, &vDir);

                    pdwPixel[swz.Get2D()] = VectorToColor(&vDir);

                    swz.IncU();
                }

                swz.IncV();
            }

            pd3dtc->UnlockRect((D3DCUBEMAP_FACES)j, i);
        }
    }

    *ppd3dtc = pd3dtc;

    return D3D_OK;
}

//------------------------------------------------------------------------------
//	CScene::RecalculateSpeeds
//------------------------------------------------------------------------------
void
CScene::RecalculateSpeeds(void)
{
	UINT i;

	m_currentMinSpeed	= 1.0e30f;
	m_currentMaxSpeed	= -1.0e30f;
	for(i=0; i<m_numBalls; ++i)
	{
		if(m_balls[i].GetSpeed() < m_currentMinSpeed)
			m_currentMinSpeed = m_balls[i].GetSpeed();
		if(m_balls[i].GetSpeed() > m_currentMaxSpeed)
			m_currentMaxSpeed = m_balls[i].GetSpeed();
	}
	for(i=m_maxBalls; i<m_maxBalls+m_numLights; ++i)
	{
		if(m_balls[i].GetSpeed() < m_currentMinSpeed)
			m_currentMinSpeed = m_balls[i].GetSpeed();
		if(m_balls[i].GetSpeed() > m_currentMaxSpeed)
			m_currentMaxSpeed = m_balls[i].GetSpeed();
	}
	m_dSpeed = m_currentMaxSpeed - m_currentMinSpeed;
}

}
