/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    terrain.cpp

Author:


Description:

    Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "terrain.h"
#include "perlin.h"

static VERTEX	    grid[NUM_GRID][NUM_GRID];
static WORD		    grid_indices[NUM_GRID-1][NUM_GRID-1][2][3];

//******************************************************************************
// NOTE: The UpdateTerrain and InitTerrain functions were taken from the
// DirectX "fog" sample which in turn makes use of turbulence and noise 
// algorithms developed by Ken Perlin for procedural modelling.  The
// UpdateTerrain function has been modified for better optimization.

//******************************************************************************
//	UpdateTerrain() -- updates the terrain grid and returns a new altitude
//		for the viewpoint.
//******************************************************************************
float
UpdateTerrain(void)
{
	static float	tic = 0.0f;
	D3DXVECTOR3		pnt, tmp;
    float           frac = M_PI / (2.0f * (float)(NUM_GRID));
	int				i, j;
	float			alt;
	float			size = (float)GRID_WIDTH/(NUM_GRID-1.0f);
	float			offset = (float)GRID_WIDTH/2.0f;

	tic += 1.0f/NUM_GRID;

    tmp.x = 0.0f;
    tmp.y = tic * M_PIDIV2;
    tmp.z = (float)(NUM_GRID-1) / (float)(NUM_GRID) * M_PIDIV2;

	for (i=0;i<NUM_GRID; i++) {
		for (j=1; j<NUM_GRID; j++) {
			grid[i][j-1].vPosition.y = grid[i][j].vPosition.y;
			grid[i][j-1].vNormal.x = grid[i][j].vNormal.x;
			grid[i][j-1].vNormal.y = grid[i][j].vNormal.y;
			grid[i][j-1].vNormal.z = grid[i][j].vNormal.z;
			grid[i][j-1].v0 = grid[i][j].v0;
		}
		// add new info to last row
		j = NUM_GRID-1;
//		pnt = D3DVECTOR(i/(float)NUM_GRID, tic, j/(float)NUM_GRID);
//		pnt *= M_PI * 0.5f;
        pnt = tmp;
		alt = PerlinTurbulence1D(pnt, 6);
		//alt *= 8.0f;
		if (alt < 0.0f) {
			alt = 0.0f;
		} else {
			alt = (float)pow(alt*4.f, 2.0f);
		}
		grid[i][j].vPosition.y = alt * 4.0f;
		grid[i][j].v0 = alt*0.2f + 0.18f;

		if (grid[i][j].v0 < 0.0) {
			grid[i][j].v0 = 0.0f;
		} else if (grid[i][j].v0 > 0.95f) {
			grid[i][j].v0 = 0.95f;
		}
		grid[i][j].v0 = 1.0f - grid[i][j].v0;
        tmp.x += frac;
	}
	// now calc normal for next to last row.
	j = NUM_GRID-2;
	for (i=1; i<NUM_GRID-1; i++) {
		pnt.y = 1.0f;
		pnt.x = (grid[i-1][j].vPosition.y-grid[i][j].vPosition.y) - (grid[i+1][j].vPosition.y-grid[i][j].vPosition.y);
		pnt.z = (grid[i][j-1].vPosition.y-grid[i][j].vPosition.y) - (grid[i][j+1].vPosition.y-grid[i][j].vPosition.y);
		D3DXVec3Normalize(&pnt, &pnt);
		grid[i][j].vNormal.x = pnt.x;
		grid[i][j].vNormal.y = pnt.y;
		grid[i][j].vNormal.z = pnt.z;
	}

	return max(grid[NUM_GRID/2][NUM_GRID/8].vPosition.y, grid[NUM_GRID/2][0].vPosition.y);
}	// end of UpdateTerrain()

//******************************************************************************
void
InitTerrain()
{
	int		i, j;
	float	size = (float)GRID_WIDTH/(NUM_GRID-1.0f);
	float	offset = (float)GRID_WIDTH/2.0f;

	for (i=0; i<NUM_GRID; i++) {
		for (int j=0; j<NUM_GRID; j++) {
			grid[i][j] = VERTEX(D3DXVECTOR3(i*size-offset, 0.0f, j*size-offset), D3DXVECTOR3(0.0f, 1.0f, 0.0f), (float)j/(float)NUM_GRID, (float)i/(float)NUM_GRID);
		}
	}
	for (i=0; i<NUM_GRID-1; i++) {
		for (j=0; j<NUM_GRID-1; j++) {
			grid_indices[i][j][0][0] = i + j*NUM_GRID;
			grid_indices[i][j][0][1] = i + j*NUM_GRID + 1;
			grid_indices[i][j][0][2] = i + j*NUM_GRID + NUM_GRID + 1;
			grid_indices[i][j][1][0] = i + j*NUM_GRID;
			grid_indices[i][j][1][1] = i + j*NUM_GRID + NUM_GRID + 1;
			grid_indices[i][j][1][2] = i + j*NUM_GRID + NUM_GRID;
		}
	}

	// force the Perlin noise to be randomly initilized
	D3DXVECTOR3 pnt(0.0f, 0.0f, 0.0f);

	PerlinTurbulence1D(pnt, 1);		// call it once to force automatic init
	InitPerlinNoise(GetTickCount());	// reseed with a random number

}	// end of InitTerrain()

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

    CTerrain*   pTerrain;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

#ifdef UNDER_XBOX
    if (!(GetStartupContext() & TSTART_STRESS)) {
        pDisplay->ReleaseDevice();
        pDisplay->GetDirect3D8()->SetPushBufferSize(4194304, 32768);
        pDisplay->CreateDevice();
    }
#endif

    // Create the scene
    pTerrain = new CTerrain();
    if (!pTerrain) {
        return FALSE;
    }

    // Initialize the scene
    if (!pTerrain->Create(pDisplay)) {
        pTerrain->Release();
        return FALSE;
    }

    bRet = pTerrain->Exhibit(pnExitCode);

    // Clean up the scene
    pTerrain->Release();

#ifdef UNDER_XBOX
    if (!(GetStartupContext() & TSTART_STRESS)) {
        pDisplay->ReleaseDevice();
        pDisplay->GetDirect3D8()->SetPushBufferSize(524288, 32768);
        pDisplay->CreateDevice();
    }
#endif

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
// CTerrain
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CTerrain
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
CTerrain::CTerrain() {

    m_pd3dr = NULL;
}

//******************************************************************************
//
// Method:
//
//     ~CTerrain
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
CTerrain::~CTerrain() {
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
BOOL CTerrain::Create(CDisplay* pDisplay) {

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
int CTerrain::Exhibit(int *pnExitCode) {

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
BOOL CTerrain::Prepare() {

    // Initialize the terrain vertices
    InitTerrain();

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
BOOL CTerrain::Setup() {

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
BOOL CTerrain::Initialize() {

    D3DMATERIAL8 material;
    UINT         i;

    SetMaterial(&material, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
    m_pDevice->SetMaterial(&material);

    m_pd3dt = (CTexture8*)CreateTexture(m_pDevice, TEXT("terrain1.bmp"));
    if (!m_pd3dt) {
        return FALSE;
    }

    m_pDevice->SetRenderState(D3DRS_AMBIENT, 0);

    if (!SetPointLight(m_pDevice, 0, D3DXVECTOR3(0.0f, 40.0f, -40.0f), RGB_MAKE(255, 255, 255), 120.0f)) {
        return FALSE;
    }

    if (!SetPixelFog(m_pDevice, 0x00b5b5ff, 50.0f, 80.0f, 0.02f)) {
        return FALSE;
    }

    m_pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

#ifdef UNDER_XBOX
    if (GetStartupContext() & TSTART_STRESS) {
        m_pd3dr = CreateVertexBuffer(m_pDevice, NULL, NUM_GRID * NUM_GRID * sizeof(VERTEX), 0, FVF_VERTEX);
        if (!m_pd3dr) {
            return FALSE;
        }
    }
#endif

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
void CTerrain::Efface() {

    if (m_pd3dr) {
        ReleaseVertexBuffer(m_pd3dr);
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
void CTerrain::Update() {

	static float alt = 10.0f;	
	static float dy = 0.0f;	
	float        new_alt;
	float		 new_dy;
    LPBYTE       pdata;

	// Update the viewpoint altitude
	new_alt = UpdateTerrain();
	new_alt = (9.0f * alt + new_alt)*0.1f;
	new_dy = new_alt - alt;
	dy = (9.0f * dy + new_dy) *0.1f;
	alt = new_alt;

	// Update the viewpoint
    m_cam.vPosition = D3DXVECTOR3(0.0f, alt + 2.0f, -40.0f);
	m_cam.vInterest = D3DXVECTOR3(0.0f, alt + 2.0f + dy, -39.0f);

#ifdef UNDER_XBOX
    if (GetStartupContext() & TSTART_STRESS) {
        m_pd3dr->Lock(0, 0, &pdata, 0);
        memcpy(pdata, grid, NUM_GRID * NUM_GRID * sizeof(VERTEX));
        m_pd3dr->Unlock();
    }
#endif
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
BOOL CTerrain::Render() {

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
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(179, 179, 255), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView(&m_cam);

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VERTEX);

    m_pDevice->SetTexture(0, m_pd3dt);

#ifdef UNDER_XBOX
    if (!(GetStartupContext() & TSTART_STRESS)) {
#endif
        m_pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, NUM_GRID_VERTICES, NUM_GRID_INDICES / 3, grid_indices, D3DFMT_INDEX16, grid, sizeof(VERTEX));
#ifdef UNDER_XBOX
    }
    else {
        m_pDevice->SetStreamSource(0, m_pd3dr, sizeof(VERTEX));
        m_pDevice->DrawIndexedVertices(D3DPT_TRIANGLELIST, NUM_GRID_INDICES, &grid_indices[0][0][0][0]);
    }
#endif

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
void CTerrain::ProcessInput() {

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
BOOL CTerrain::InitView() {

    if (!m_pDisplay) {
        return FALSE;
    }

    // Set the view position
    m_cam.vPosition     = D3DXVECTOR3(0.0f, 1.0f, -40.0f);
    m_cam.vInterest     = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    m_cam.fRoll         = 0.0f;
    m_cam.fFieldOfView  = M_PI / 4.0f;
    m_cam.fNearPlane    = 0.1f;
    m_cam.fFarPlane     = 10000.0f;
    m_pDisplay->SetCamera(&m_cam);

    return m_pDisplay->SetView(&m_cam);
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
BOOL CTerrain::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
