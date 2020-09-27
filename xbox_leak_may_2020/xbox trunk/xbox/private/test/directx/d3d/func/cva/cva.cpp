/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	cva.cpp

Abstract:

	Compressed Vertex Formats tests

Author:

	Robert Heitkamp (robheit) 5-Feb-2001

Revision History:

	24-Feb-2001 robheit
		Initial version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <time.h>
#include "d3dlocus.h"
#include "cva.h"

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

    CCVA*   pTest;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay)
        return FALSE;

    // Create the scene
    pTest = new CCVA();
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
// CCVA
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CCVA
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
CCVA::CCVA() {
	m_zoomFactor		= 400.0f;
	m_rotateXFactor		= 1.0f;
	m_rotateYFactor		= 1.0f;
	m_rotateZFactor		= 1.0f;
	m_ballRadius		= 10000.0f;
	m_radius			= (float)90000.0f;
	m_radius2			= m_radius * m_radius;
	m_numBalls			= 0;

	// Objects drawn with compressed position information
	m_useBall[m_numBalls++]	= FALSE;	// D3DVSDT_FLOAT1 		Red				Points
	m_useBall[m_numBalls++]	= FALSE;	// D3DVSDT_FLOAT2 		Green			Points
	m_useBall[m_numBalls++]	= FALSE;	// D3DVSDT_SHORT2 		Blue			Points
	m_useBall[m_numBalls++]	= TRUE;		// D3DVSDT_NORMSHORT1 	Cyan			Points
	m_useBall[m_numBalls++]	= TRUE;		// D3DVSDT_NORMSHORT2 	Magenta			Points
	m_useBall[m_numBalls++]	= TRUE;		// D3DVSDT_SHORT1 		Yellow			Points
	m_useBall[m_numBalls++]	= TRUE;		// D3DVSDT_PBYTE1 		Med Yellow		Points
	m_useBall[m_numBalls++]	= TRUE;		// D3DVSDT_PBYTE2 		Med Magenta		Points
	m_useBall[m_numBalls++]	= FALSE;	// D3DVSDT_FLOAT3 		Med Red			Solid
	m_useBall[m_numBalls++]	= FALSE;	// D3DVSDT_FLOAT4 		Dark Red		Solid
	m_useBall[m_numBalls++]	= FALSE;	// D3DVSDT_SHORT4 		Med Green		Solid
	m_useBall[m_numBalls++]	= TRUE;		// D3DVSDT_NORMSHORT3 	Dark Green		Solid
	m_useBall[m_numBalls++]	= FALSE;	// D3DVSDT_NORMSHORT4 	Med Blue		Solid
	m_useBall[m_numBalls++]	= TRUE;		// D3DVSDT_NORMPACKED3 	Dark Blue		Solid
	m_useBall[m_numBalls++]	= TRUE;		// D3DVSDT_SHORT3 		Dark Yellow		Solid
	m_useBall[m_numBalls++]	= TRUE;		// D3DVSDT_PBYTE3 		Dark Magenta	Solid
	m_useBall[m_numBalls++]	= FALSE;	// D3DVSDT_PBYTE4 		Dark Cyan		Solid
	m_useBall[m_numBalls++]	= FALSE;	// D3DVSDT_FLOAT2H 		Med Cyan		Points

	// Objects drawn with compressed normals
	m_useBall[m_numBalls++]	= TRUE;		// D3DVSDT_NORMSHORT3 	Dark Green		Solid
	m_useBall[m_numBalls++]	= TRUE;		// D3DVSDT_NORMPACKED3 	Dark Blue		Solid
}  

//******************************************************************************
//
// Method:
//
//     ~CCVA
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
CCVA::~CCVA() {
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
BOOL CCVA::Create(CDisplay* pDisplay) {
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
int CCVA::Exhibit(int *pnExitCode) {
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
BOOL CCVA::Prepare()  {
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
BOOL CCVA::Setup() {
	if(CScene::Setup())	
	{
		m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
		m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
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
BOOL CCVA::Initialize() 
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
	static unsigned int		seed = (unsigned int)time(NULL);
	static BOOL				first = TRUE;

	// Make sure every pass through the test uses the same seed to generate the
	// exact dataset for all runs
	srand(seed);

	// First time through, create all the data
	if(first) 
	{
		first = FALSE;

		// Create the sphere
		m_sphere.Create(m_pDevice->GetIDirect3DDevice8(), 
						D3DXVECTOR3(0.0f, 0.0f, 0.0f), m_radius, 16, 8);
		m_invertedSphere.Create(m_pDevice->GetIDirect3DDevice8(), m_radius, 16, 8);
		m_invertedSphere.SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

		// Drawn with points
		m_balls[0].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_FLOAT1, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[0].SetColor(1.0f, 0.0f, 0.0f);
		m_balls[1].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_FLOAT2, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[1].SetColor(0.0f, 1.0f, 0.0f);
		m_balls[2].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_SHORT2, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[2].SetColor(0.0f, 0.0f, 1.0f);
		m_balls[3].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_NORMSHORT1, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[3].SetColor(0.0f, 1.0f, 1.0f);
		m_balls[4].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_NORMSHORT2, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[4].SetColor(1.0f, 0.0f, 1.0f);
		m_balls[5].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_SHORT1, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[5].SetColor(1.0f, 1.0f, 0.0f);
		m_balls[6].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_PBYTE1, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[6].SetColor(0.5f, 0.5f, 0.0f);
		m_balls[7].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_PBYTE2, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[7].SetColor(0.5f, 0.0f, 0.5f);

		// Drawn with Triangles
		m_balls[8].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_FLOAT3, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[8].SetColor(0.5f, 0.0f, 0.0f);
		m_balls[9].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_FLOAT4, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[9].SetColor(0.3f, 0.0f, 0.0f);
		m_balls[10].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_SHORT4, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[10].SetColor(0.0f, 0.5f, 0.0f);
		m_balls[11].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_NORMSHORT3, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[11].SetColor(0.0f, 0.3f, 0.0f);
		m_balls[12].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_NORMSHORT4, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[12].SetColor(0.0f, 0.0f, 0.5f);
		m_balls[13].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_NORMPACKED3, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[13].SetColor(0.0f, 0.0f, 0.3f);
		m_balls[14].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_SHORT3, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[14].SetColor(0.3f, 0.3f, 0.0f);
		m_balls[15].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_PBYTE3, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[15].SetColor(0.3f, 0.0f, 0.3f);
		m_balls[16].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_PBYTE4, D3DVSDT_FLOAT3, m_ballRadius, 64, 32);
		m_balls[16].SetColor(0.0f, 0.3f, 0.3f);

//		m_balls[17].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_FLOAT2H, D3DVSDT_FLOAT3, m_ballRadius, 64, 32); // Points?
//		m_balls[17].SetColor(0.0f, 0.5f, 0.5f);
		m_useBall[17] = FALSE; // Not supported yet (in my code)

		m_balls[18].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_FLOAT3, D3DVSDT_NORMSHORT3, m_ballRadius, 64, 32);
		m_balls[18].SetColor(1.0f, 0.6f, 0.3f);
		m_balls[19].Create(m_pDevice->GetIDirect3DDevice8(), D3DVSDT_FLOAT3, D3DVSDT_NORMPACKED3, m_ballRadius, 64, 32);
		m_balls[19].SetColor(0.3f, 0.6f, 1.0f);

		// Setup the camera (reuse some other vars)
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
	for(i=0; i<m_numBalls; ++i) 
	{
		// Make sure the new ball is not within 1 unit of another
		do 
		{
			goodLoc = TRUE;
			loc.x = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			loc.y = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			loc.z = (1.0f - (2.0f * (float)rand() / (float)RAND_MAX)) * (m_radius / 2.0f);
			dir.x = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
			dir.y = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
			dir.z = 1.0f - (2.0f * (float)rand() / (float)RAND_MAX);
			D3DXVec3Normalize(&dir, &dir);
			speed = (float)rand() / (float)RAND_MAX * 500.0f;
			for(ii=0; goodLoc && ii<i; ++ii) 
			{
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
void CCVA::Efface() {
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
void CCVA::Update() {
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

	// Move all the balls
	for(i=0; i<m_numBalls; ++i) 
	{
		if(!m_useBall[i])
			continue;

		moved = 0.0f;	

		while(moved < m_balls[i].GetSpeed()) 
		{
			moveLeft		= m_balls[i].GetSpeed() - moved;
			saveDistance	= 1.0e30f;
			hitSphere		= FALSE;
			hit				= i;
			
			// Get the intersection with the bounding sphere
			loc = m_balls[i].GetLocation() + (m_balls[i].GetDirection() * (m_radius * 3.0f));
			if(!IntersectRaySphere(loc, -m_balls[i].GetDirection(), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
								   m_radius, point)) 
			{
				__asm int 3;
				doUpdate = FALSE;
				return;
			}

			distance = D3DXVec3Length(&D3DXVECTOR3(point - m_balls[i].GetLocation())) -
					   m_balls[i].GetRadius();
	
			// Will the bounding sphere be hit?
			if(distance <= moveLeft) 
			{
				saveDistance = distance;
				hitSphere = TRUE;
			}

			// Check for intersection with all other balls
			for(ii=0; ii<m_numBalls; ++ii) 
			{
				if(!m_useBall[ii])
					continue;

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
						if((distance < moveLeft) && (distance < saveDistance)) 
						{
							normal.x = (m_balls[i].GetLocation().x + m_balls[i].GetDirection().x * distance) - 
										m_balls[ii].GetLocation().x;
							normal.y = (m_balls[i].GetLocation().y + m_balls[i].GetDirection().y * distance) -
										m_balls[ii].GetLocation().y;
							normal.z = (m_balls[i].GetLocation().z + m_balls[i].GetDirection().z * distance) -
										m_balls[ii].GetLocation().z;

							// Check for valid hit
							if(D3DXVec3Dot(&normal, &m_balls[i].GetDirection()) < 0.0f) 
							{
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
			if(hitSphere) 
			{
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
			else if(hit != i) 
			{
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
			else 
			{
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
BOOL CCVA::Render() {
	UINT		i;
    HRESULT		hr;
    DWORD		dwClearFlags	= D3DCLEAR_TARGET;

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

    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

	// World matrix
	m_pDevice->SetTransform(D3DTS_WORLD, &m_worldMatrix);

	// Camera
	m_camera.Render(m_pDevice);

	// Draw balls
	for(i=0; i<m_numBalls; ++i)
	{
		if(m_useBall[i])
			m_balls[i].Render();
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
void CCVA::ProcessInput() {
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
BOOL CCVA::InitView() {
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
BOOL CCVA::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
//	CCVA::IntersectRaySphere
//------------------------------------------------------------------------------
BOOL
CCVA::IntersectRaySphere(
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
//	CCVA::GetSphereIntersection
//------------------------------------------------------------------------------
BOOL
CCVA::GetSphereIntersection(
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
