/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	fsaaperf.cpp

Abstract:

	Full Screen Anitaliasing performance tests

Author:

	Robert Heitkamp (robheit) 5-Feb-2001

Revision History:

	5-Feb-2001 robheit
		Initial version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <time.h>
#include "d3dlocus.h"
#include "fsaaperf.h"

//------------------------------------------------------------------------------
//	Macros:
//------------------------------------------------------------------------------
#define MAX2(A,B) ((A) >= (B) ? (A) : (B))
#define MAX3(A,B,C) MAX2((A), MAX2((B), (C)))
#define FUDGE(A,B,C) (fabs((A) - (B)) < (C) ? (B) : (A))
#ifndef PI 
#	define PI 3.14159265359f
#endif

//******************************************************************************
//
// Function:
//
//     ExhibitScene
//
// Description:
//
//     Create the scene, pump messages, process user input,
//     update the scene, render the scene, and release the scene when finished.
//
// Arguments:
//
//     CDisplay* pDisplay           - Pointer to the Display object
//
//     int* pnExitCode              - Optional pointer to an integer that will
//                                    be set to the exit value contained in the 
//                                    wParam parameter of the WM_QUIT message 
//                                    (if received)
//
// Return Value:
//
//     TRUE if the display remains functional on exit, FALSE otherwise.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ExhibitScene)(CDisplay* pDisplay, int* pnExitCode) {

    CFSAAPerf*   pTest;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay)
        return FALSE;

    // Create the scene
    pTest = new CFSAAPerf();
    if (!pTest)
        return FALSE;

    // Initialize the scene
    if (!pTest->Create(pDisplay)) {
        pTest->Release();
        return FALSE;
    }

    bRet = pTest->Exhibit(pnExitCode);

    // Clean up the scene
    pTest->Release();

    return bRet;
}

//******************************************************************************
//
// Function:
//
//     ValidateDisplay
//
// Description:
//
//     Evaluate the given display information in order to determine whether or
//     not the display is capable of rendering the scene.  If not, the given 
//     display will not be included in the display list.
//
// Arguments:
//
//     CDirect3D8* pd3d                 - Pointer to the Direct3D object
//
//     D3DCAPS8* pd3dcaps               - Capabilities of the device
//
//     D3DDISPLAYMODE*                  - Display mode into which the device
//                                        will be placed
//
// Return Value:
//
//     TRUE if the scene can be rendered using the given display, FALSE if
//     it cannot.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ValidateDisplay)(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {
    return TRUE;
}

//******************************************************************************
// CFSAAPerf
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CFSAAPerf
//
// Description:
//
//     Initialize the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CFSAAPerf::CFSAAPerf() {
	m_enableJoystick		= FALSE;
	m_zoomFactor			= 2.0f;
	m_rotateXFactor			= 2.0f;
	m_rotateYFactor			= 2.0f;
	m_rotateZFactor			= 2.0f;
	m_numBalls				= 50;
	m_radius				= (float)5.5f;
	m_radius2				= m_radius * m_radius;
	m_balls					= new CBall [m_numBalls];
	m_msListIndex			= -1;
	m_msListSize			= 16;
	m_msList				= new MultiSampleList [m_msListSize];
	m_numFramesPerTest		= 100;	// Number of frame to run per type
	m_numLayers				= 100;
	m_backgroundTriangles	= new FSAAPERFVertex [m_numLayers * 3 * 2];

	m_msList[0].type	= D3DMULTISAMPLE_NONE;
	m_msList[0].edgeAA	= FALSE;
	m_msList[0].enabled	= FALSE;
	_tcscpy(m_msList[0].name, TEXT("D3DMULTISAMPLE_NONE"));
	_tcscpy(m_msList[0].state, TEXT("Disabled"));
	_tcscpy(m_msList[0].edge, TEXT("Disabled"));

	m_msList[1].type	= D3DMULTISAMPLE_NONE;
	m_msList[1].edgeAA	= TRUE;
	m_msList[1].enabled	= FALSE;
	_tcscpy(m_msList[1].name, TEXT("D3DMULTISAMPLE_NONE"));
	_tcscpy(m_msList[1].state, TEXT("Disabled"));
	_tcscpy(m_msList[1].edge, TEXT("Enabled"));

    m_msList[2].type	= D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;
	m_msList[2].edgeAA	= FALSE;
	m_msList[2].enabled	= TRUE;
	_tcscpy(m_msList[2].name, TEXT("D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX"));
	_tcscpy(m_msList[2].state, TEXT("Enabled"));
	_tcscpy(m_msList[2].edge, TEXT("Disabled"));

    m_msList[3].type	= D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN;
	m_msList[3].edgeAA	= FALSE;
	m_msList[3].enabled	= TRUE;
	_tcscpy(m_msList[3].name, TEXT("D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN"));
	_tcscpy(m_msList[3].state, TEXT("Enabled"));
	_tcscpy(m_msList[3].edge, TEXT("Disabled"));

    m_msList[4].type	= D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR;
	m_msList[4].edgeAA	= FALSE;
	m_msList[4].enabled	= TRUE;
	_tcscpy(m_msList[4].name, TEXT("D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR"));
	_tcscpy(m_msList[4].state, TEXT("Enabled"));
	_tcscpy(m_msList[4].edge, TEXT("Disabled"));

    m_msList[5].type	= D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR;
	m_msList[5].edgeAA	= FALSE;
	m_msList[5].enabled	= TRUE;
	_tcscpy(m_msList[5].name, TEXT("D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR"));
	_tcscpy(m_msList[5].state, TEXT("Enabled"));
	_tcscpy(m_msList[5].edge, TEXT("Disabled"));

    m_msList[6].type	= D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR;
	m_msList[6].edgeAA	= FALSE;
	m_msList[6].enabled	= TRUE;
	_tcscpy(m_msList[6].name, TEXT("D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR"));
	_tcscpy(m_msList[6].state, TEXT("Enabled"));
	_tcscpy(m_msList[6].edge, TEXT("Disabled"));

    m_msList[7].type	= D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR;
	m_msList[7].edgeAA	= FALSE;
	m_msList[7].enabled	= TRUE;
	_tcscpy(m_msList[7].name, TEXT("D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR"));
	_tcscpy(m_msList[7].state, TEXT("Enabled"));
	_tcscpy(m_msList[7].edge, TEXT("Disabled"));

    m_msList[8].type	= D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN;
	m_msList[8].edgeAA	= FALSE;
	m_msList[8].enabled	= TRUE;
	_tcscpy(m_msList[8].name, TEXT("D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN"));
	_tcscpy(m_msList[8].state, TEXT("Enabled"));
	_tcscpy(m_msList[8].edge, TEXT("Disabled"));

    m_msList[9].type	= D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX;
	m_msList[9].edgeAA	= FALSE;
	m_msList[9].enabled	= FALSE;
	_tcscpy(m_msList[9].name, TEXT("D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX"));
	_tcscpy(m_msList[9].state, TEXT("Disabled"));
	_tcscpy(m_msList[9].edge, TEXT("Disabled"));

    m_msList[10].type		= D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN;
	m_msList[10].edgeAA		= FALSE;
	m_msList[10].enabled	= FALSE;
	_tcscpy(m_msList[10].name, TEXT("D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN"));
	_tcscpy(m_msList[10].state, TEXT("Disabled"));
	_tcscpy(m_msList[10].edge, TEXT("Disabled"));

    m_msList[11].type		= D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR;
	m_msList[11].edgeAA		= FALSE;
	m_msList[11].enabled	= FALSE;
	_tcscpy(m_msList[11].name, TEXT("D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR"));
	_tcscpy(m_msList[11].state, TEXT("Disabled"));
	_tcscpy(m_msList[11].edge, TEXT("Disabled"));

    m_msList[12].type		= D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR;
	m_msList[12].edgeAA		= FALSE;
	m_msList[12].enabled	= FALSE;
	_tcscpy(m_msList[12].name, TEXT("D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR"));
	_tcscpy(m_msList[12].state, TEXT("Disabled"));
	_tcscpy(m_msList[12].edge, TEXT("Disabled"));

    m_msList[13].type		= D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR;
	m_msList[13].edgeAA		= FALSE;
	m_msList[13].enabled	= FALSE;
	_tcscpy(m_msList[13].name, TEXT("D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR"));
	_tcscpy(m_msList[13].state, TEXT("Disabled"));
	_tcscpy(m_msList[13].edge, TEXT("Disabled"));

    m_msList[14].type		= D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR;
	m_msList[14].edgeAA		= FALSE;
	m_msList[14].enabled	= FALSE;
	_tcscpy(m_msList[14].name, TEXT("D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR"));
	_tcscpy(m_msList[14].state, TEXT("Disabled"));
	_tcscpy(m_msList[14].edge, TEXT("Disabled"));

    m_msList[15].type		= D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN;
	m_msList[15].edgeAA		= FALSE;
	m_msList[15].enabled	= FALSE;
	_tcscpy(m_msList[15].name, TEXT("D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN"));
	_tcscpy(m_msList[15].state, TEXT("Disabled"));
	_tcscpy(m_msList[15].edge, TEXT("Disabled"));

	LONGLONG frequency;
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	m_frequency = (double) frequency;
}  

//******************************************************************************
//
// Method:
//
//     ~CFSAAPerf
// 
// Description:
//
//     Clean up the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
CFSAAPerf::~CFSAAPerf() {
	delete [] m_balls;
	delete [] m_msList;
	delete [] m_backgroundTriangles;
}

//******************************************************************************
//
// Method:
//
//     Create
// 
// Description:
//
//     Prepare the test for rendering.
//
// Arguments:
//
//     CDisplay* pDisplay               - Pointer to a Display object.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CFSAAPerf::Create(CDisplay* pDisplay) {
    return CScene::Create(pDisplay);
}

//******************************************************************************
//
// Method:
//
//     Exhibit
//
// Description:
//
//     Execute the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     The exit value contained in the wParam parameter of the WM_QUIT message.
//
//******************************************************************************
int CFSAAPerf::Exhibit(int *pnExitCode) {
    return CScene::Exhibit(pnExitCode);
}

//******************************************************************************
//
// Method:
//
//     Prepare
//
// Description:
//
//     Initialize all device-independent data to be used in the scene.  This
//     method is called only once at creation (as opposed to Setup and
//     Initialize, which get called each time the device is Reset).
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the scene was successfully prepared, FALSE if it was not.
//
//******************************************************************************
BOOL CFSAAPerf::Prepare()  {

	// Log header
	Log(LOG_COMMENT, TEXT("================================================================"));
	Log(LOG_COMMENT, TEXT("MultisampleMode, State, EdgeAntialiasing, NumTris, NumVerts, FPS"));

	return CScene::Prepare();
}

//******************************************************************************
//
// Method:
//
//     Setup
//
// Description:
//
//     Obtain the device interface pointer from the display, save the current
//     state of the device, and initialize the background vertices to the
//     dimensions of the render target.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the scene was successfully prepared for initialization, FALSE if
//     it was not.
//
//******************************************************************************
BOOL CFSAAPerf::Setup() {
	float	lineWidth = 1.0f;

	if(CScene::Setup())	{
		m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		m_pDevice->SetRenderState(D3DRS_MULTISAMPLEMASK, 0xffffffff);
		m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_LINEWIDTH, *(DWORD*)&lineWidth);
		return TRUE;
	}
	return FALSE;
}
  
//******************************************************************************
//
// Method:
//
//     Initialize
//
// Description:
//
//     Initialize the device and all device objects to be used in the test (or
//     at least all device resource objects to be created in the video, 
//     non-local video, or default memory pools).
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE if the test was successfully initialized for rendering, FALSE if
//     it was not.
//
//******************************************************************************
BOOL CFSAAPerf::Initialize() 
{
	UINT					i;
	UINT					ii;
	BOOL					goodLoc;
	D3DXVECTOR3				loc;
	D3DXVECTOR3				dir;
	float					speed;
	float					dxyz;
	D3DPRESENT_PARAMETERS	d3dpp;
	double					phi;
	double					rho;
	static unsigned int		seed = 1; // (unsigned int)time(NULL);
	static BOOL				first = TRUE;

	// Make sure every pass through the test uses the same seed to generate the
	// exact dataset for all runs
	srand(seed);
	
	// Initialize frame rate counters
	m_numFrames		= 0.0;
	m_frameRate		= 0.0;
	m_maxFrameRate	= -1.0e30;
	m_minFrameRate	= 1.0e30;
	m_frameCounter	= 0;

	// Setup the device for the current antialiasing type
    m_pDisplay->GetPresentParameters(&d3dpp);

	// Save the parameters the first time through
	if(m_saveParms) {
		CopyMemory(&m_savedd3dpp, &d3dpp, sizeof(D3DPRESENT_PARAMETERS));
		m_saveParms = FALSE;
	}

	// Setup for next mode, and possible end of test
	if(++m_msListIndex >= m_msListSize) {
		m_msListIndex = 0;
		//FadeOut();
		m_bQuit = TRUE;
		return FALSE;
	}

	// Turn off vsync to get an accurate reading of the frame rate
	d3dpp.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.MultiSampleType					= m_msList[m_msListIndex].type;

	// Reset the device
	m_pDevice->Reset(&d3dpp);

	// Setup the render state for the antialiasing
	m_pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, 
							  (m_msList[m_msListIndex].type != D3DMULTISAMPLE_NONE) &&
							  m_msList[m_msListIndex].enabled);

	// Enable alpha blending for edge antialiasing
	if(m_msList[m_msListIndex].edgeAA)
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	else
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	// First time through, create all the data
	if(first) {
		first = FALSE;

		// Initialize the number of triangles
		m_numTriangles	= 0;
		m_numVertices	= 0;

		// Create the sphere
		m_sphere.Create(m_pDevice->GetIDirect3DDevice8(), 
						D3DXVECTOR3(0.0f, 0.0f, 0.0f), m_radius, 16, 8);
		m_invertedSphere.Create(m_pDevice->GetIDirect3DDevice8(), m_radius, 16, 8);
		m_invertedSphere.SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		m_numTriangles	+= m_invertedSphere.GetNumTris();
		m_numVertices	+= m_invertedSphere.GetNumVerts();
		m_numVertices	+= m_sphere.GetNumVerts();

		// Create the balls
		for(i=0; i<m_numBalls; ++i) {
			m_balls[i].Create(m_pDevice->GetIDirect3DDevice8(), 0.5f, 64, 32);
			m_numTriangles	+= m_balls[i].GetNumTris();
			m_numVertices	+= m_balls[i].GetNumVerts();
		}

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
		}
		m_numTriangles += m_numLayers * 2;
		m_numVertices += m_numLayers * 6;

		// Setup the camera (resuse some other vars)
		first = FALSE;
		m_camera.SetViewport(0, 0, m_pDisplay->GetWidth(), m_pDisplay->GetHeight(),
							 0.0f, 1.0f);
		m_camera.LookAt(D3DXVECTOR3(0.0f, 0.0f, 2.0f * m_radius), 
						D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
		m_camera.SetPerspectiveFov(45.0f, (float)m_pDisplay->GetWidth() / 
										  (float)m_pDisplay->GetHeight(),
								   0.9f * m_radius, 3.1f * m_radius);
		m_camera.EnableLight(TRUE, FALSE);
		m_camera.RotateLightY(-30.0f);
		m_camera.RotateLightX(-30.0f);
		D3DXMatrixIdentity(&m_worldMatrix);
	}

	// Set the position, speed and direction for the balls
	for(i=0; i<m_numBalls; ++i) {

		// Make sure the new ball is not within 1 unit of another
		do {
			goodLoc = TRUE;
			loc.x = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			loc.y = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			loc.z = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			dir.x = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
			dir.y = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
			dir.z = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
			D3DXVec3Normalize(&dir, &dir);
			speed = (float)rand() / (float)RAND_MAX * 0.5f;
			for(ii=0; goodLoc && ii<i; ++ii) {
				if(D3DXVec3Length(&D3DXVECTOR3(loc - m_balls[ii].GetLocation())) < 1.0f)
					goodLoc = FALSE;
			}
		} while(!goodLoc);

		m_balls[i].SetPosition(loc);
		m_balls[i].SetDirection(dir);
		m_balls[i].SetSpeed(speed);
	}

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     Efface
//
// Description:
//
//     Release all device resource objects (or at least those objects created
//     in video memory, non-local video memory, or the default memory pools)
//     and restore the device to its initial state.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CFSAAPerf::Efface() {

	// Log 
	Log(LOG_COMMENT, TEXT("================================================================"));

	m_pDevice->Reset(&m_savedd3dpp);
    CScene::Efface();
}

//******************************************************************************
//
// Method:
//
//     Update
//
// Description:
//
//     Update the state of the scene to coincide with the given time.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CFSAAPerf::Update() {
	D3DXVECTOR3	point;
	float		distance;
	float		saveDistance;
	UINT		hit;
	float		moveLeft;
	float		moved;
	D3DXVECTOR3	normal;
	D3DXVECTOR3	inverse;
	D3DXVECTOR3	reflection;
	float		newSpeed;
	UINT		i;
	UINT		ii;
	BOOL		hitSphere;
	D3DXVECTOR3	loc;
	static BOOL doUpdate = TRUE;
	D3DXVECTOR3	saveNormal;
	
    CScene::Update();

	// To disable ball or test changes for debug purposes
	if(!doUpdate)
		return;

	// Switch to the next test?
	if(m_frameCounter > m_numFramesPerTest) {

		// Dump frame rate information to log file
		Log(LOG_COMMENT, TEXT("%s, %s, %s, %d, %d, %.2lf"), 
			m_msList[m_msListIndex].name, m_msList[m_msListIndex].state, 
			m_msList[m_msListIndex].edge, m_numTriangles, m_numVertices, 
			m_avgFrameRate);
		
		if(!Initialize())
			doUpdate = FALSE;
		m_frameCounter = 0;
		return;
	}

	// Move all the balls
	for(i=0; i<m_numBalls; ++i) {
		moved = 0.0f;	

		while(moved < m_balls[i].GetSpeed()) {
			moveLeft		= m_balls[i].GetSpeed() - moved;
			saveDistance	= 1.0e30f;
			hitSphere		= FALSE;
			hit				= i;
			
#if 0
			// Debug check
			if(D3DXVec3Length(&m_balls[i].GetLocation()) >= m_radius) {
				__asm int 3;
				doUpdate = FALSE;
				return;
			}
#endif

			// Get the intersection with the bounding sphere
			loc = m_balls[i].GetLocation() + (m_balls[i].GetDirection() * (m_radius * 3.0f));
			if(!IntersectRaySphere(loc, -m_balls[i].GetDirection(), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
								   m_radius, point)) {
				__asm int 3;
				doUpdate = FALSE;
				return;
			}

			distance = D3DXVec3Length(&D3DXVECTOR3(point - m_balls[i].GetLocation())) -
					   m_balls[i].GetRadius();
	
			// Will the bounding sphere be hit?
			if(distance <= moveLeft) {
				saveDistance = distance;
				hitSphere = TRUE;
			}

			// Check for intersection with all other balls
			for(ii=0; ii<m_numBalls; ++ii) {
				if(i != ii) {

					if(GetSphereIntersection(m_balls[i].GetLocation(),
											 m_balls[i].GetRadius(),
											 m_balls[i].GetDirection(),
											 m_balls[ii].GetLocation(),
											 m_balls[ii].GetRadius(),
											 distance)) {
						distance = FUDGE(distance, moveLeft, 0.0001);
						if((distance < moveLeft) && (distance < saveDistance)) {

							normal.x = (m_balls[i].GetLocation().x + m_balls[i].GetDirection().x * distance) - 
										m_balls[ii].GetLocation().x;
							normal.y = (m_balls[i].GetLocation().y + m_balls[i].GetDirection().y * distance) -
										m_balls[ii].GetLocation().y;
							normal.z = (m_balls[i].GetLocation().z + m_balls[i].GetDirection().z * distance) -
										m_balls[ii].GetLocation().z;

							// Check for valid hit
							if(D3DXVec3Dot(&normal, &m_balls[i].GetDirection()) < 0.0f) {
								D3DXVec3Normalize(&saveNormal, &normal);
								saveDistance	= distance;
								hit				= ii;
								hitSphere		= FALSE;
							}
						}
					}
				}
			}

			// Was the bounding sphere the closest hit?
			if(hitSphere) {

				if(saveDistance > m_balls[i].GetSpeed()) 
					saveDistance = m_balls[i].GetSpeed();

				// Move the ball
				m_balls[i].Move(saveDistance);
				moved += saveDistance;

				// Normal at inner point on sphere
				normal = -point;
				D3DXVec3Normalize(&normal, &normal);

				// Inverse of direction
				inverse = -m_balls[i].GetDirection();
				
				// Calculate the reflection
				reflection = normal * 2.0f * (D3DXVec3Dot(&normal, &inverse)) - inverse;

				// Change the ball's direction
				m_balls[i].SetDirection(reflection);
			}

			// Otherwise, was another ball hit?
			else if(hit != i) {

				if(saveDistance > m_balls[i].GetSpeed()) 
					saveDistance = m_balls[i].GetSpeed();

				// Move the ball
				m_balls[i].Move(saveDistance);
				moved += saveDistance;

				// Inverse of direction
				inverse = -m_balls[i].GetDirection();
				
				// Calculate the reflection
				reflection = saveNormal * 2.0f * (D3DXVec3Dot(&saveNormal, &inverse)) - inverse;

				// Change the ball's direction
				m_balls[i].SetDirection(reflection);

				// Normal at point on sphere
				saveNormal = -saveNormal;

				// Inverse of direction
				inverse = -m_balls[hit].GetDirection();
				
				// Calculate the reflection
				reflection = saveNormal * 2.0f * (D3DXVec3Dot(&saveNormal, &inverse)) - inverse;

				// Change the other ball's direction
				m_balls[hit].SetDirection(reflection);

				// Cheap physics: Adjust the speeds
				newSpeed = (m_balls[i].GetSpeed() + m_balls[hit].GetSpeed()) / 2.0f;
				m_balls[i].SetSpeed(newSpeed);
				m_balls[hit].SetSpeed(newSpeed);

			}

			// Otherwise, just move the ball forward
			else {
				moved = m_balls[i].GetSpeed();
				m_balls[i].Move(moved);
			}
		}
	}
}

//******************************************************************************
//
// Method:
//
//     Render
//
// Description:
//
//     Render the test scene.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CFSAAPerf::Render() {
	UINT		i;
    HRESULT		hr;
    DWORD		dwClearFlags	= D3DCLEAR_TARGET;
	LONGLONG	start;
	LONGLONG	stop;

    if (!m_bCreated) 
        return FALSE;

#ifndef UNDER_XBOX
    hr = m_pDevice->TestCooperativeLevel();
    if (FAILED(hr)) {
        if (hr == D3DERR_DEVICELOST) {
            return TRUE;
        }
        if (hr == D3DERR_DEVICENOTRESET) {
            if (!Reset()) {
                return FALSE;
            }
        }
    }
#endif // !UNDER_XBOX

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) 
        dwClearFlags |= D3DCLEAR_ZBUFFER;

	// Start timer
	QueryPerformanceCounter((LARGE_INTEGER*)&start);

    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

	// World matrix
	m_pDevice->SetTransform(D3DTS_WORLD, &m_worldMatrix);

	// Camera
	m_camera.Render(m_pDevice);

	// Draw the background triangles with depth write disabled to ensure all are written
	// without affecting other draws
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pDevice->SetVertexShader(FVF_FSAAPERF_BACKGROUND_VERTEX);
	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_numLayers, m_backgroundTriangles,
							   sizeof(FSAAPERFVertex));
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// Draw balls
	for(i=0; i<m_numBalls; ++i)
		m_balls[i].Render();

	// Draw the sphere
	m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	m_invertedSphere.Render();
	if(m_msList[m_msListIndex].edgeAA) 
		m_pDevice->SetRenderState(D3DRS_EDGEANTIALIAS, TRUE);
	m_sphere.Render();
	if(m_msList[m_msListIndex].edgeAA) 
		m_pDevice->SetRenderState(D3DRS_EDGEANTIALIAS, FALSE);
	m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    m_pDevice->SetTexture(0, NULL);

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // Fade out the scene on exit
    if (m_pfnFade) 
        (this->*m_pfnFade)();

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

	// Stop timer
	QueryPerformanceCounter((LARGE_INTEGER*)&stop);

	// Frame rate calculations
	m_currentFrameRate	= 1.0 / ((double)(stop - start) / m_frequency);
	m_frameRate			+= m_currentFrameRate;
	m_numFrames			+= 1.0;
	m_avgFrameRate		= m_frameRate / m_numFrames;
	if(m_currentFrameRate > m_maxFrameRate)
		m_maxFrameRate = m_currentFrameRate;
	if(m_currentFrameRate < m_minFrameRate)
		m_minFrameRate = m_currentFrameRate;
	++m_frameCounter;

    return TRUE;
}

//******************************************************************************
//
// Method:
//
//     ProcessInput
//
// Description:
//
//     Process user input for the scene.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CFSAAPerf::ProcessInput() {
	if(!m_enableJoystick)
		return;

#ifdef UNDER_XBOX
	JOYSTATE	js;
	float		zoom;
	float		rotateX;
	float		rotateY;
	float		rotateZ;
	static BOOL	b0		= FALSE;
	static BOOL	b1		= FALSE;
	static BOOL	b2		= FALSE;
	static BOOL	b3		= FALSE;
	static BOOL	b4		= FALSE;
	static BOOL	b5		= FALSE;
	static BOOL fixed	= TRUE;

	// Query the state of the joysticks
    GetJoystickState(&js);

	m_camera.SetDrawAxis(FALSE);

	// Button 0: Changed lighting from fixed-free
	if(js.buttons[0]) {
		if(!b0) {
			fixed	= !fixed;
			b0		= TRUE;
			m_camera.EnableLight(TRUE, fixed);
		}
	}
	else
		b0 = FALSE;

	// Button 1:
	if(js.buttons[1]) {
		if(!b1) {
			b1	= TRUE;
		}
	}
	else
		b1 = FALSE;

	// Button 2:
	if(js.buttons[2]) {
		if(!b2) {
			b2	= TRUE;
		}
	}
	else
		b2 = FALSE;

	// Button 4:
	if(js.buttons[4]) {
		if(!b4) {
			b4	= TRUE;
		}
	}
	else
		b4 = FALSE;

	// Button 3:
	if(js.buttons[3]) {
		if(!b3) {
			b3	= TRUE;
		}
	}
	else
		b3 = FALSE;

	// Button 5:
	if(js.buttons[5]) {
		if(!b5) {
			b5	= TRUE;
		}
	}
	else
		b5 = FALSE;

	// My joystick and camera processing
	if(js.buttons[0] || js.buttons[10] || js.buttons[1] || 
		js.buttons[4] || js.buttons[3] || js.buttons[5] ||
		js.buttons[2] ||
	   (js.f1X != 0.0f) || (js.f1Y != 0.0f) || 
	   (js.f2X != 0.0f) || (js.f2Y != 0.0f)) {
		m_camera.SetDrawAxis(TRUE);

		// Is button 1 down? Update light
		if(js.buttons[
			10]) {
			if(!fixed) {
				// Joystick 2 rotates around X and Y
				if(js.f2Y != 0.0f)
				{
					rotateX = js.f2Y * m_rotateXFactor;
					m_camera.RotateLightX(rotateX);
				}

				if(js.f2X != 0.0f)
				{
					rotateY = js.f2X * m_rotateYFactor;
					m_camera.RotateLightY(-rotateY);
				}
			}
		}

		// Update camera
		else {
			// Joystick 1 zooms & rotates around Z
			if(js.f1Y != 0.0f)
			{
				zoom = js.f1Y * m_zoomFactor;
				m_camera.Zoom(zoom);
			}

			if(js.f1X != 0.0f)
			{
				rotateZ = js.f1X * m_rotateZFactor;
				m_camera.RotateZ(rotateZ);
			}

			// Joystick 2 rotates around X and Y
			if(js.f2Y != 0.0f)
			{
				rotateX = js.f2Y * m_rotateXFactor;
				m_camera.RotateX(-rotateX);
			}

			if(js.f2X != 0.0f)
			{
				rotateY = js.f2X * m_rotateYFactor;
				m_camera.RotateY(rotateY);
			}
		}
	}

	// Default processing
	else
		CScene::ProcessInput();
#else
	CScene::ProcessInput();
#endif
}

//******************************************************************************
//
// Method:
//
//     InitView
//
// Description:
//
//     Initialize the camera view in the scene.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CFSAAPerf::InitView() {
    return CScene::InitView();
}

//******************************************************************************
// Scene window procedure (pseudo-subclassed off the main window procedure)
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     WndProc
//
// Description:
//
//     Scene window procedure to process messages received by the main 
//     application window.
//
// Arguments:
//
//     LRESULT* plr             - Result of the message processing
//
//     HWND hWnd                - Application window
//
//     UINT uMsg                - Message to process
//
//     WPARAM wParam            - First message parameter
//
//     LPARAM lParam            - Second message parameter
//
// Return Value:
//
//     TRUE if the message was handled, FALSE otherwise.
//
//******************************************************************************
BOOL CFSAAPerf::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;
#ifndef UNDER_XBOX

    switch (uMsg) {

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_F12: // Toggle pause state
                    m_bPaused = !m_bPaused;
                    return TRUE;
            }

            break;
    }
#endif // !UNDER_XBOX

    return CScene::WndProc(plr, hWnd, uMsg, wParam, lParam);
}

//------------------------------------------------------------------------------
//	CFSAAPerf::IntersectRaySphere
//------------------------------------------------------------------------------
BOOL
CFSAAPerf::IntersectRaySphere(
							  IN const D3DXVECTOR3&	rayU, 
							  IN const D3DXVECTOR3&	rayV, 
							  IN const D3DXVECTOR3&	center,
							  IN float				radius,
							  OUT D3DXVECTOR3&		point
							  )
/*++

Routine Description:

	Returns the distance from a point to a line segment

Arguments:

	IN rayU -	Ray origin
	IN rayV -	Ray direction
	IN center -	Center of sphere
	IN radius -	Radius of sphere
	OUT point -	Point of intersection

Return Value:

	TRUE if the ray intersects the sphere, FALSE otherwise

--*/
{
	D3DXVECTOR3	uc(center - rayU);
	float		v = D3DXVec3Dot(&uc, &rayV);
	float		d = (radius * radius) - (D3DXVec3Dot(&uc, &uc) - (v * v));

	if(d < 0.0f)
		return FALSE;

	d = (float)sqrt(d);
	point = rayU + (rayV * (v - d));
	return TRUE;
}

//------------------------------------------------------------------------------
//	CFSAAPerf::GetSphereIntersection
//------------------------------------------------------------------------------
BOOL
CFSAAPerf::GetSphereIntersection(
								 IN const D3DXVECTOR3&	s0,
								 IN float				s0r,
								 IN const D3DXVECTOR3&	v,
								 IN const D3DXVECTOR3&	s1,
								 IN float				s1r,
								 OUT float&				t
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
	if(D3DXVec3Dot(&v, &D3DXVECTOR3(s1 - s0)) <= 0.0f)
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
//	CFSAAPerf::RandomColor
//------------------------------------------------------------------------------
DWORD
CFSAAPerf::RandomColor(void)
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

