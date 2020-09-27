/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    dlight.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "dlight.h"

VERTEX  	sphere[SPHERE_VERTICES];
WORD		sphere_indices[SPHERE_INDICES];

LVERTEX	    light_vertices[MAX_DLIGHTS];

float
Min (const D3DXVECTOR3& v)
{
   float ret = v.x;
   if (v.y < ret) ret = v.y;
   if (v.z < ret) ret = v.z;
   return ret;
}

float
Max (const D3DXVECTOR3& v)
{
   float ret = v.x;
   if (ret < v.y) ret = v.y;
   if (ret < v.z) ret = v.z;
   return ret;
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

    CDLight* pDLight;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pDLight = new CDLight();
    if (!pDLight) {
        return FALSE;
    }

    // Initialize the scene
    if (!pDLight->Create(pDisplay)) {
        pDLight->Release();
        return FALSE;
    }

    bRet = pDLight->Exhibit(pnExitCode);

    // Clean up the scene
    pDLight->Release();

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
// CDLight
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CDLight
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
CDLight::CDLight() {

    m_tic = 0.0f;
}

//******************************************************************************
//
// Method:
//
//     ~CDLight
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
CDLight::~CDLight() {
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
BOOL CDLight::Create(CDisplay* pDisplay) {

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
int CDLight::Exhibit(int *pnExitCode) {

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
BOOL CDLight::Prepare() {

    UINT i, j;

    // ##BUGBUG: Direct3D has a lighting bug where the fixed function lighting
    // engine can't handle scaling transforms
    m_mScale = m_mIdentity;
    m_mScale._11 = TARGET_RADIUS;
    m_mScale._22 = TARGET_RADIUS;
    m_mScale._33 = TARGET_RADIUS;

	// generate the sphere data, note the random texture coords
	float   dj = M_PI/(MESH_SIZE+1.0f);
	float	di = M_PI/MESH_SIZE;

	// vertices 0 and 1 are the north and south poles
	sphere[0] = VERTEX(D3DXVECTOR3(0.0f,  1.0f, 0.0f), D3DXVECTOR3(0.0f,  1.0f, 0.0f), RND(), RND());
	sphere[1] = VERTEX(D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), RND(), RND());

	for (j=0; j<MESH_SIZE; j++) {
		for (i=0; i<MESH_SIZE*2; i++) {
			D3DVECTOR	p;

			p.y = (float) cos((j+1) * dj);
			p.x = (float) sin(i * di) * (float) sin((j+1) * dj);
			p.z = (float) cos(i * di) * (float) sin((j+1) * dj);
			sphere[2+i+j*MESH_SIZE*2] = VERTEX(p, p, RND(), RND());
		}
	}

	// now generate the triangle indices
	// strip around north pole first
	for (i=0; i<MESH_SIZE*2; i++) {
		sphere_indices[3*i] = 0;
		sphere_indices[3*i+1] = i+2;
		sphere_indices[3*i+2] = i+3;
		if (i==MESH_SIZE*2-1)
			sphere_indices[3*i+2] = 2;
	}

	// now all the middle strips
	int	v;		// vertex offset
	int ind;	// indices offset
	for (j=0; j<MESH_SIZE-1; j++) {
		v = 2+j*MESH_SIZE*2;
		ind = 3*MESH_SIZE*2 + j*6*MESH_SIZE*2;
		for (i=0; i<MESH_SIZE*2; i++) {
			sphere_indices[6*i+ind] = v+i;
			sphere_indices[6*i+2+ind] = v+i+1;
			sphere_indices[6*i+1+ind] = v+i+MESH_SIZE*2;

			sphere_indices[6*i+ind+3] = v+i+MESH_SIZE*2;
			sphere_indices[6*i+2+ind+3] = v+i+1;
			sphere_indices[6*i+1+ind+3] = v+i+MESH_SIZE*2+1;
			if (i==MESH_SIZE*2-1) {
				sphere_indices[6*i+2+ind] = v+i+1-2*MESH_SIZE;
				sphere_indices[6*i+2+ind+3] = v+i+1-2*MESH_SIZE;
				sphere_indices[6*i+1+ind+3] = v+i+MESH_SIZE*2+1-2*MESH_SIZE;
			}
		}
	}

	// finally strip around south pole
	v = SPHERE_VERTICES-MESH_SIZE*2;
	ind = SPHERE_INDICES-3*MESH_SIZE*2;
	for (i=0; i<MESH_SIZE*2; i++) {
		sphere_indices[3*i+ind] = 1;
		sphere_indices[3*i+1+ind] = v+i+1;
		sphere_indices[3*i+2+ind] = v+i;
		if (i==MESH_SIZE*2-1)
			sphere_indices[3*i+1+ind] = (WORD)v;
	}

    return TRUE;
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
BOOL CDLight::Setup() {

    return CScene::Setup();
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
BOOL CDLight::Initialize() {

    D3DXVECTOR3 color;
    float       fMin;
    UINT        i;

    m_pDevice->SetRenderState(D3DRS_AMBIENT, RGB_MAKE(10, 10, 10));

    SetMaterial(m_pDevice, RGBA_MAKE(196, 196, 196, 196), 0, RGBA_MAKE(255, 255, 255, 255), 0, 20.0f);

	// create the lights, linear attenuation, range=MAX_RANGE
	for (i=0; i<MAX_DLIGHTS; i++) {
		m_vLPos[i] = D3DXVECTOR3(200.0f*RND() - 100.0f, 21.0f, 200.0f*RND() - 100.0f);
		color = D3DXVECTOR3(RND(), RND(), RND());
		// force full saturation
		fMin = Min(color);
        color.x -= fMin;
        color.y -= fMin;
        color.z -= fMin;
		color /= Max(color);
        m_cLCol[i] = D3DCOLOR_COLORVALUE(color.x, color.y, color.z, 1.0f);
        SetPointLight(m_pDevice, i, m_vLPos[i], m_cLCol[i],
                        MAX_RANGE, 1.0f, 0.0f, 0.0f);
        m_pDevice->LightEnable(i, FALSE);
		light_vertices[i] = LVERTEX(m_vLPos[i], m_cLCol[i], 0, 0.0f, 0.0f);
	}

    for (i = 0; i < 4; i++) {
        m_prBackground[i].cDiffuse = RGB_MAKE(26, 51, 102);
    }

    m_pDevice->GetDeviceCaps(&m_d3dcaps);
    DebugString(TEXT("Maximum simultaneously active lights: %d"), m_d3dcaps.MaxActiveLights);

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
void CDLight::Efface() {

    UINT i;

	for (i=0; i<MAX_DLIGHTS; i++) {
        m_pDevice->LightEnable(i, FALSE);
    }
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
void CDLight::Update() {

    D3DXVECTOR3 position, diff;
    float       thresh;
    float       dist;
    UINT        i;
    UINT        uEnabledLights = 0;
    D3DXMATRIX  mTranslate;

	m_tic += 0.01f;

	// calc the target position
	position = D3DXVECTOR3(100.0f*(float)sin(m_tic*0.87f), 21.0f, 100.0f*(float)cos(m_tic*0.79f));

	// Check each light's position and range against the position of the target
	// Only turn on the lights that are actually in range.

	thresh = TARGET_RADIUS+MAX_RANGE;
	thresh *= thresh;
	for (i=0; i<MAX_DLIGHTS; i++) {
		diff = position - m_vLPos[i];
		dist = D3DXVec3Dot(&diff, &diff);
		if ((dist > thresh) || (uEnabledLights >= m_d3dcaps.MaxActiveLights)) {
			// need to turn off the light if not already off
            m_pDevice->LightEnable(i, FALSE);
        	// set light's point to black
			light_vertices[i].cDiffuse = 0;
		} else {
			// need to turn on the light if not already on
            m_pDevice->LightEnable(i, TRUE);
			// set light's point to the light color
			light_vertices[i].cDiffuse = m_cLCol[i];

            uEnabledLights++;
		}
	}

    mTranslate = m_mIdentity;
    mTranslate._41 = position.x;
    mTranslate._42 = 0.0f;
    mTranslate._43 = position.z;

    D3DXMatrixMultiply(&m_mTranslate, &m_mScale, &mTranslate);

    CScene::Update();
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
BOOL CDLight::Render() {

    DWORD    dwClearFlags = D3DCLEAR_TARGET;
    HRESULT  hr;

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

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(26, 51, 102), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
/*
    m_pDevice->SetVertexShader(FVF_TLVERTEX);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_prBackground, sizeof(TLVERTEX));
*/
    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_LVERTEX);

    m_pDevice->SetTransform(D3DTS_WORLD, &m_mIdentity);

    m_pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, MAX_DLIGHTS, light_vertices, sizeof(LVERTEX));

    m_pDevice->SetVertexShader(FVF_VERTEX);
    m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

    m_pDevice->SetTransform(D3DTS_WORLD, &m_mTranslate);

    // Draw the sphere
    m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, SPHERE_VERTICES, SPHERE_INDICES / 3, sphere_indices, D3DFMT_INDEX16, sphere, sizeof(VERTEX));

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // Fade out the scene on exit
    if (m_pfnFade) {
        (this->*m_pfnFade)();
    }

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
void CDLight::ProcessInput() {

    CScene::ProcessInput();
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
BOOL CDLight::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 300.0f, -100.0f);
    m_camInitial.vInterest     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_camInitial.fRoll         = 0.0f;
    m_camInitial.fFieldOfView  = M_PI / 4.0f;
    m_camInitial.fNearPlane    = 0.1f;
    m_camInitial.fFarPlane     = 1000.0f;
    m_pDisplay->SetCamera(&m_camInitial);

    return m_pDisplay->SetView(&m_camInitial);
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
BOOL CDLight::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
