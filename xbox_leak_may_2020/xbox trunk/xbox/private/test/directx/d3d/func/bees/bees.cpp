/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    bees.cpp

Author:

    Matt Bronder

Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "bees.h"

// sparkle mesh
LVERTEX     s_mesh[4];
WORD		s_indices[6] = { 0, 1, 3, 1, 2, 3 };

D3DXMATRIX	proj, view;

Bee		bee[NUM_BEES];

Bee
SetBee(int rot, float scale, D3DXVECTOR3 position, D3DXVECTOR3 velocity, D3DCOLOR color)
{
	Bee	ret;

	ret.rot = rot;
	ret.scale = scale;
	ret.position = position;
	ret.velocity = velocity;
	ret.color = color;

	return ret;
}

void
InitBees()
{	
	for (int i=0; i<NUM_BEES; i++) {
		bee[i] = SetBee(rand()%9,1.0f,10.0f*D3DXVECTOR3(RND()-RND(),RND()-RND(),RND()-RND()),D3DXVECTOR3(0.0f, 0.0f, 0.0f), RGB_MAKE(rand() % 256, rand() % 51, rand() % 256));
	}
}

void
UpdateBees(D3DXVECTOR3 goal)
{
	int			i;
	D3DXVECTOR3	force;

	for (i=0; i<NUM_BEES; i++) {
		force = goal - bee[i].position;
		D3DXVec3Normalize(&force, &force);
		bee[i].velocity += force * MAX_FORCE;
		bee[i].velocity *= DAMP;
		bee[i].position += bee[i].velocity;
		if (D3DXVec3Length(&bee[i].velocity) > MAX_VELOCITY)
			bee[i].velocity *= 0.9f;

		bee[i].rot++;
		if (bee[i].rot > 8)
			bee[i].rot = 0;
	}
}	// end of UpdateBees()

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

    CBees*   pBees;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pBees = new CBees();
    if (!pBees) {
        return FALSE;
    }

    // Initialize the scene
    if (!pBees->Create(pDisplay)) {
        pBees->Release();
        return FALSE;
    }

    bRet = pBees->Exhibit(pnExitCode);

    // Clean up the scene
    pBees->Release();

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
// CBees
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CBees
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
CBees::CBees() {

    m_pd3dtSparkle = NULL;
    m_tic = 0.0f;
}

//******************************************************************************
//
// Method:
//
//     ~CBees
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
CBees::~CBees() {
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
BOOL CBees::Create(CDisplay* pDisplay) {

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
int CBees::Exhibit(int *pnExitCode) {

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
BOOL CBees::Prepare() {

    InitBees();

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
BOOL CBees::Setup() {

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
BOOL CBees::Initialize() {

    m_pd3dtSparkle = (CTexture8*)CreateTexture(m_pDevice, TEXT("sparklen.bmp"));
    if (!m_pd3dtSparkle) {
        return FALSE;
    }
#ifdef UNDER_XBOX
    m_pd3dtSparkle->GetIDirect3DTexture8()->MoveResourceMemory(D3DMEM_VIDEO);
#endif

    return CScene::Initialize();
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
void CBees::Efface() {

    ReleaseTexture(m_pd3dtSparkle);

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
void CBees::Update() {

    D3DXVECTOR3 position;

	// tic to move things around
	m_tic += 0.08f;

	// mess with the target position
	position = D3DXVECTOR3(float(sin(m_tic*0.73f)*12), 5.0f+10.0f*(float)sin(m_tic*0.678f), float(sin(m_tic*0.895f)*12));

	// ok, now play with bees
	UpdateBees(position);
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
BOOL CBees::Render() {

    CAMERA      cam;
	D3DXVECTOR3 view_dir, dx, dy, position;
    DWORD       dwClearFlags = D3DCLEAR_TARGET;
    UINT        i;
    HRESULT     hr;

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
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
//    m_pDisplay->SetView();

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_LVERTEX);

    m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
    m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    SetColorStage(m_pDevice, 0, D3DTA_TEXTURE, D3DTA_DIFFUSE, D3DTOP_MODULATE);

    m_pDevice->SetTexture(0, m_pd3dtSparkle);

    m_pDisplay->GetCamera(&cam);

	    // draw the bees
	    D3DXVec3Normalize(&view_dir, &(cam.vInterest - cam.vPosition));
	    D3DXVec3Cross(&dx, &view_dir, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	    D3DXVec3Cross(&dy, &view_dir, &dx);
	    D3DXVec3Cross(&dx, &view_dir, &dy);

	    float	global_scale = 1.0f;
	    for (i=0; i<NUM_BEES; i++) {
		    D3DXVECTOR3	sx = dx * bee[i].scale * global_scale;
		    D3DXVECTOR3	sy = dy * bee[i].scale * global_scale;

		    position = bee[i].position;

		    int		iu, iv;
		    float	u, v;
		    float	dt = 0.33333333f;

		    iu = bee[i].rot%3;
		    iv = bee[i].rot/3;

		    u = iu * dt;
		    v = iv * dt;
		    
		    s_mesh[0] = LVERTEX(position+sx+sy, bee[i].color, 0, u, v);
		    s_mesh[1] = LVERTEX(position-sx+sy, bee[i].color, 0, u+dt, v);
		    s_mesh[2] = LVERTEX(position-sx-sy, bee[i].color, 0, u+dt, v+dt);
		    s_mesh[3] = LVERTEX(position+sx-sy, bee[i].color, 0, u, v+dt);

            m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, s_indices, D3DFMT_INDEX16, s_mesh, sizeof(LVERTEX));
	    }

    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
    m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

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
void CBees::ProcessInput() {

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
BOOL CBees::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_camInitial.vPosition     = D3DXVECTOR3(0.0f, 0.0f, -40.0f);
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
BOOL CBees::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
