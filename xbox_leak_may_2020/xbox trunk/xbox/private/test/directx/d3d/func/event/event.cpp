/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	event.cpp

Abstract:

	Event Notifier tests

Author:

	Robert Heitkamp (robheit) 22-Feb-2001

Revision History:

	22-Feb-2001 robheit
		Initial version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <time.h>
#include "d3dlocus.h"
#include "event.h"

//------------------------------------------------------------------------------
//	Macros:
//------------------------------------------------------------------------------
#define MAX2(A,B) ((A) >= (B) ? (A) : (B))
#define MAX3(A,B,C) MAX2((A), MAX2((B), (C)))
#define FUDGE(A,B,C) (fabs((A) - (B)) < (C) ? (B) : (A))
#ifndef PI 
#	define PI 3.14159265359f
#endif

//------------------------------------------------------------------------------
//	Globals
//------------------------------------------------------------------------------
static double				g_frequency;
static long					g_callbacks				= 0;
static IDirect3DDevice8*	g_pDevice				= NULL;
static long					g_vblanks				= 0;
static float				g_runTime				= 60.0; // In seconds
static BOOL					g_enableVBlankCallback	= TRUE;
static BOOL					g_enableCallbacks		= TRUE;
static BOOL					g_enableInput			= FALSE;
static long					g_fencePending			= 0;

//------------------------------------------------------------------------------
//	::VerticalBlankCallback
//------------------------------------------------------------------------------
void __cdecl 
VerticalBlankCallback(
					  IN D3DVBLANKDATA *pData
					  ) 
/*++

Routine Description:

	Called for every vertical blank

Arguments:

	IN context -	?

Return Value:

	None

--*/
{
	// Increment the vblank counter
	InterlockedIncrement(&g_vblanks);
}

//------------------------------------------------------------------------------
//	::Callback
//------------------------------------------------------------------------------
void __cdecl
Callback(
		 IN DWORD fenceHandle
		 ) 
/*++

Routine Description:

	Basic callback. Tests to ensure a fence handle is really not pending

Arguments:

	IN fenceHandle -	Fence handle

Return Value:

	None

--*/
{
	InterlockedDecrement(&g_callbacks);
	if(g_pDevice->IsFencePending(fenceHandle)) 
		InterlockedIncrement(&g_fencePending);
}

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

    CEvent*	pTest;
    BOOL	bRet;

    // Verify that the display has been initialized
    if (!pDisplay)
        return FALSE;

    // Create the scene
    pTest = new CEvent();
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
// CEvent
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CEvent
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
CEvent::CEvent() {
	unsigned int			seed = (unsigned int)time(NULL);

	m_zoomFactor		= 2.0f;
	m_rotateXFactor		= 2.0f;
	m_rotateYFactor		= 2.0f;
	m_rotateZFactor		= 2.0f;
	m_numBalls			= 100;
	m_radius			= (float)8.0f;
	m_radius2			= m_radius * m_radius;
	m_balls				= new CBall [m_numBalls];
	m_fenceHandles		= new DWORD [m_numBalls];

	LONGLONG frequency;
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	g_frequency = (double)frequency;

	// Make sure every pass through the test uses the same seed to generate the
	// exact dataset for all runs
	srand(seed);
	Log(LOG_COMMENT, TEXT("Seed: %u"), seed);
}  

//******************************************************************************
//
// Method:
//
//     ~CEvent
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
CEvent::~CEvent() {

	// Callback counter should be 0.
	if(g_enableCallbacks) {
		if(g_callbacks != 0) 
			Log(LOG_FAIL, TEXT("Unresolved callbacks: %ld"), g_callbacks);
	}

	// vblank counter should be aproximately g_runTime / 60
	if(g_enableVBlankCallback) {
		if(fabs(((float)g_vblanks / 60.0f) - g_runTime) > 1.0f) {
			Log(LOG_FAIL, TEXT("VBlank counter (%f) does not match runtime (%f)"), 
			(float)g_vblanks / 60.0f, g_runTime);
		}
	}

	// Pending fence
	if(g_fencePending != 0)
	{
		Log(LOG_FAIL, TEXT("Callback: IsFencePending: Returned TRUE, should be FALSE (%lu)"),
			g_fencePending);
	}

	delete [] m_balls;
	delete [] m_fenceHandles;
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
BOOL CEvent::Create(CDisplay* pDisplay) {
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
int CEvent::Exhibit(int *pnExitCode) {
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
BOOL CEvent::Prepare()  {
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
BOOL CEvent::Setup() {
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
BOOL CEvent::Initialize() 
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
	static BOOL				first = TRUE;

	// Setup the device for the current antialiasing type
    m_pDisplay->GetPresentParameters(&d3dpp);

	// Save the parameters the first time through
	if(m_saveParms) {
		CopyMemory(&m_savedd3dpp, &d3dpp, sizeof(D3DPRESENT_PARAMETERS));
		m_saveParms = FALSE;
	}

	// Turn off vsync to get a better test of vblank
	d3dpp.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;

	// Reset the device
	m_pDevice->Reset(&d3dpp);

	// Store the device
	g_pDevice = m_pDevice->GetIDirect3DDevice8();

	// Setup callbacks
	if(g_enableVBlankCallback)
		g_pDevice->SetVerticalBlankCallback(VerticalBlankCallback);

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

	// Only run the test for n seconds
	SetTimeDuration(g_runTime);

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
void CEvent::Efface() {

	// Turn off the vertical blank callbacks
	if(g_enableVBlankCallback)
		g_pDevice->SetVerticalBlankCallback(NULL);
	
	// Wait for the gpu to finish 
	g_pDevice->BlockUntilIdle();
	
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
void CEvent::Update() {
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
	float		lastMoveLeft;
	BOOL		checkForStick;

    CScene::Update();

	// To disable ball or test changes for debug purposes
	if(!doUpdate)
		return;

	// Move all the balls
	for(i=0; i<m_numBalls; ++i) {
		checkForStick	= FALSE;
		moved			= 0.0f;	
		lastMoveLeft	= -1.0f;
		while(moved < m_balls[i].GetSpeed()) {
			moveLeft		= m_balls[i].GetSpeed() - moved;
			if(moveLeft < 0.0)
				moveLeft = 0.0;
			if(moveLeft == lastMoveLeft)
			{
				if(checkForStick)
					break;
				else
					checkForStick = TRUE;
			}
			lastMoveLeft	= moveLeft;
			saveDistance	= 1.0e30f;
			hitSphere		= FALSE;
			hit				= i;
			
			// Get the intersection with the bounding sphere
			loc = m_balls[i].GetLocation() + (m_balls[i].GetDirection() * (m_radius * 3.0f));
			if(!IntersectRaySphere(loc, -m_balls[i].GetDirection(), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
								   m_radius, point)) {
				doUpdate = FALSE;
				return;
			}

			distance = D3DXVec3Length(&D3DXVECTOR3(point - m_balls[i].GetLocation())) -
					   m_balls[i].GetRadius();

			if(distance < 0.0)
				distance = 0.0;

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
BOOL CEvent::Render() {
	UINT					i;
    HRESULT					hr;
    DWORD					dwClearFlags	= D3DCLEAR_TARGET;
	double					interval;
	LONGLONG				current;
	static BOOL				doBigFence	= TRUE;
	static BOOL				block		= FALSE;
	static D3DCALLBACKTYPE	callbackType = D3DCALLBACK_READ;
	LONGLONG				waitForVBlankTime;

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
        dwClearFlags |= D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

	// World matrix
	m_pDevice->SetTransform(D3DTS_WORLD, &m_worldMatrix);

	// Camera
	m_camera.Render(m_pDevice);

	// Quick range check of fences
	if(doBigFence && g_pDevice->IsFencePending(0xffffffff))
		Log(LOG_FAIL, TEXT("IsFencePending: Should have returned FALSE for 0xffffffff"));

	// Draw balls
	for(i=0; i<m_numBalls; ++i) {
		m_balls[i].Render();

		// Insert a fence
		m_fenceHandles[i] = g_pDevice->InsertFence();

		// If the fence handle is our BIG test case, disable the BIG test case
		if(m_fenceHandles[i] == 0xffffffff)
			doBigFence = FALSE;

		// The fence should still be pending at this point
		if(!g_pDevice->IsFencePending(m_fenceHandles[i]))
			Log(LOG_FAIL, TEXT("Render: IsFencePending: Return FALSE, should be TRUE"));

		// Occasionally block on the fence and make sure it's passed
		if(block) {
			g_pDevice->BlockOnFence(m_fenceHandles[i]);
			if(g_pDevice->IsFencePending(m_fenceHandles[i])) 
				Log(LOG_FAIL, TEXT("Render: IsFencePending: Returned TRUE, should be FALSE"));
		}
	
		// Insert a callback
		if(g_enableCallbacks) {
			InterlockedIncrement(&g_callbacks);
			g_pDevice->InsertCallback(callbackType, (D3DCALLBACK)Callback, m_fenceHandles[i]);
		
			// Randomly set the callback type and block flag
			if(rand() % 2)
				callbackType = D3DCALLBACK_READ;
			else
				callbackType = D3DCALLBACK_WRITE;
		}
		block = (BOOL)(rand() % 2);
	}

	// Wait for a vblank
	if(g_enableVBlankCallback) {
		QueryPerformanceCounter((LARGE_INTEGER*)&waitForVBlankTime);
		g_pDevice->BlockUntilVerticalBlank();
		QueryPerformanceCounter((LARGE_INTEGER*)&current);

		interval = (double)(current - waitForVBlankTime) / g_frequency;
		if(interval > 1.0 / 55.0) 
			Log(LOG_FAIL, TEXT("BlockUntilVerticalBlank: Blocked too long: %lf"), interval);
	}

	// Draw the sphere
	m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
	m_invertedSphere.Render();
	m_sphere.Render();
	m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

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
void CEvent::ProcessInput() {
	if(!g_enableInput)
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
				rotateX = js.f2Y * m_rotateXFactor;
				if(rotateX != 0.0f)
					m_camera.RotateLightX(rotateX);

				rotateY = js.f2X * m_rotateYFactor;
				if(rotateY != 0.0f)
					m_camera.RotateLightY(-rotateY);
			}
		}

		// Update camera
		else {
			// Joystick 1 zooms & rotates around Z
			zoom = js.f1Y * m_zoomFactor;
			if(zoom != 0.0f)
				m_camera.Zoom(zoom);

			rotateZ = js.f1X * m_rotateZFactor;
			if(rotateZ != 0.0f)
				m_camera.RotateZ(rotateZ);

			// Joystick 2 rotates around X and Y
			rotateX = js.f2Y * m_rotateXFactor;
			if(rotateX != 0.0f)
				m_camera.RotateX(-rotateX);

			rotateY = js.f2X * m_rotateYFactor;
			if(rotateY != 0.0f)
				m_camera.RotateY(rotateY);
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
BOOL CEvent::InitView() {
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
BOOL CEvent::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
//	CEvent::IntersectRaySphere
//------------------------------------------------------------------------------
BOOL
CEvent::IntersectRaySphere(
						   IN const D3DXVECTOR3&	rayU, 
						   IN const D3DXVECTOR3&	rayV, 
						   IN const D3DXVECTOR3&	center,
						   IN float					radius,
						   OUT D3DXVECTOR3&			point
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
//	CEvent::GetSphereIntersection
//------------------------------------------------------------------------------
BOOL
CEvent::GetSphereIntersection(
							  IN const D3DXVECTOR3&	s0,
							  IN float				s0r,
							  IN const D3DXVECTOR3&	v,
							  IN const D3DXVECTOR3&	s1,
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

