/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	hop.cpp

Abstract:

	Testing for high order primitives

Author:

	Robert Heitkamp (robheit) 22-Jan-2001

Revision History:

	22-Jan-2001 robheit
		Initial version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "d3dlocus.h"
#include "hop.h"
#include "Vertex.h"
#include "rectlinearbezier.h"
#include "rectcubicbezier.h"
#include "rectquinticbezier.h"
#include "rectlinearbspline.h"
#include "rectcubicbspline.h"
#include "rectquinticbspline.h"
#include "rectlinearinterpolate.h"
#include "rectcubicinterpolate.h"
#if 0 // Quintic interpolate (catmull rom) not supported 
#include "rectquinticinterpolate.h"
#endif
#include "trilinearbezier.h"
#include "tricubicbezier.h"
#include "triquinticbezier.h"
#if 0 // The following types are not supported on the xbox
#include "trilinearbspline.h"
#include "tricubicbspline.h"
#include "triquinticbspline.h"
#include "trilinearinterpolate.h"
#include "tricubicinterpolate.h"
#include "triquinticinterpolate.h"
#endif
#include "texture.h"

//------------------------------------------------------------------------------
//	Globals:
//------------------------------------------------------------------------------
static BOOL		g_cacheEnabled	= TRUE;
static BOOL		g_useCache		= FALSE;
static int		g_lastState		= -1;	// Forces a change
static int		g_state			= 0;
static int		g_maxState		= 10;
static BOOL		g_twoSided		= FALSE;
static D3DCULL	g_cull			= D3DCULL_CCW; // NONE = 1, CW = 2, CCW = 3
#ifndef PI
	static const float PI = 3.14159265359f;
#endif

static const float c_rotateX	= 20.0f * PI / 180.0f;
static const float c_rotateY	= 20.0f * PI / 180.0f;
static const float c_rotateZ	= 20.0f * PI / 180.0f;
static const float c_zoom		= 0.5f;

//------------------------------------------------------------------------------
//	Material
//------------------------------------------------------------------------------
static D3DMATERIAL8	g_matWhite = {
	{1.0f, 1.0f, 1.0f, 1.0f},
	{0.2f, 0.2f, 0.2f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
	50.0f 
};
static D3DMATERIAL8	g_matLightRed= {
	{1.0f, 0.8f, 0.8f, 1.0f},
	{0.2f, 0.16f, 0.16f, 1.0f},
	{1.0f, 1.0f, 1.0f, 1.0f},
	{0.0f, 0.0f, 0.0f, 1.0f},
	50.0f 
};

//------------------------------------------------------------------------------
//	Macros:
//------------------------------------------------------------------------------
#define MAX2(A,B) ((A) >= (B) ? (A) : (B))
#define MAX3(A,B,C) MAX2((A), MAX2((B), (C)))

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

    CHop*   pTest;
    BOOL     bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pTest = new CHop();
    if (!pTest) {
        return FALSE;
    }

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
// CHop
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CHop
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
CHop::CHop() {
	m_frameVertices	= NULL;
	m_hopVertices	= NULL;
	m_frameIndex	= NULL;
	m_zoomFactor	= 0.02f;
	m_rotateXFactor	= 2.0f;
	m_rotateYFactor	= 2.0f;
	m_rotateZFactor	= 2.0f;
	m_hVShader		= 0;
	D3DXMatrixIdentity(&m_worldMatrix);
}  

//******************************************************************************
//
// Method:
//
//     ~CHop
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
CHop::~CHop() {
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
BOOL CHop::Create(CDisplay* pDisplay) {
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
int CHop::Exhibit(int *pnExitCode) {
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
BOOL CHop::Prepare()  {
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
BOOL CHop::Setup() {
	if(CScene::Setup())	{
		m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
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
BOOL CHop::Initialize() {

	// This call just initializes everything to the first selected state
	ChangeState();

	ResultFailed(D3DXCreateTextureFromFileInMemory(m_pDevice->GetIDirect3DDevice8(),
												   (LPCVOID)g_texture, g_sizeofTexture, 
												   &m_pTexture),
												   TEXT("D3DXCreateTextureFromFileInMemory"));
//    m_pTexture = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture.bmp"), 
//										  D3DFMT_A1R5G5B5);
    if(!m_pTexture)
        return FALSE;

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
void CHop::Efface() {
    m_pDevice->DeleteVertexShader(m_hVShader);
	if(m_pTexture)
		m_pTexture->Release();
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
void CHop::Update() {
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
BOOL CHop::Render() {
	UINT	i;
    HRESULT	hr;
    DWORD	dwClearFlags	= D3DCLEAR_TARGET;

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

	// Change the state?
	if(g_state != g_lastState)
		ChangeState();

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) 
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 100), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

	// Two sided lighting?
//	m_pDevice->SetRenderState(D3DRS_TWOSIDEDLIGHTING, g_twoSided);

	// Culling
	m_pDevice->SetRenderState(D3DRS_CULLMODE, g_cull);
	
	// World matrix
	m_pDevice->SetTransform(D3DTS_WORLD, &m_worldMatrix);

	// Camera
	m_camera.Render(m_pDevice);

	// Turn off lighting to draw the frame
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Draw the frame
    m_pDevice->SetVertexShader(HOP_COLORVERTEX);
	m_pDevice->SetStreamSource(0, m_frameVertices, sizeof(HopColorVertex));
	m_pDevice->SetIndices(m_frameIndex, 0);
	m_pDevice->DrawIndexedPrimitive(D3DPT_LINELIST, 0, m_numVerts, 0, m_numLines);

	// Turn on lighting
	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// Enable normalization of normals
	m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// Draw the rect patch
    m_pDevice->SetVertexShader(m_hVShader);
	m_pDevice->SetStreamSource(0, m_hopVertices, sizeof(HopVertex));

	// Material
	if(g_cacheEnabled) {
		m_pDevice->SetMaterial(&g_matWhite);
//		if(g_twoSided)
//			m_pDevice->SetBackMaterial(&g_matWhite);
	}
	else {
		m_pDevice->SetMaterial(&g_matLightRed);
//		if(g_twoSided)
//			m_pDevice->SetBackMaterial(&g_matLightRed);
	}

	// Texture
//    m_pDevice->GetIDirect3DDevice8()->SetTexture(0, m_pTexture);
//	m_pDevice->GetIDirect3DDevice8()->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
//	m_pDevice->GetIDirect3DDevice8()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//	m_pDevice->GetIDirect3DDevice8()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//	m_pDevice->GetIDirect3DDevice8()->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
	
	// Draw all the patches
	for(i=0; i<m_numPatches; ++i) {
		if(g_cacheEnabled) {
			if(g_useCache) {
				if(m_rectPatch) {
					ResultFailed(m_pDevice->DrawRectPatch(i+1, (float*)m_numSegs[i], NULL), 
								 TEXT("IDirect3DDevice8::DrawRectPatch(>0, ..., NULL)"));
				}
				else {
					ResultFailed(m_pDevice->DrawTriPatch(i+1, (float*)m_numSegs[i], NULL), 
								 TEXT("IDirect3DDevice8::DrawTriPatch(>0, ..., NULL)"));
				}
			}
			else {
				if(m_rectPatch)	{
					ResultFailed(m_pDevice->DrawRectPatch(i+1, (float*)m_numSegs[i], 
														  (D3DRECTPATCH_INFO*)&m_rectInfo[i]),
								 TEXT("IDirect3DDevice8::DrawRectPatch(>0, ..., ...)"));
				}
				else {
					ResultFailed(m_pDevice->DrawTriPatch(i+1, (float*)m_numSegs[i], 
														 (D3DTRIPATCH_INFO*)&m_triInfo[i]),
								 TEXT("IDirect3DDevice8::DrawTriPatch(>0, ..., ...)"));
				}
			}
		}
		else {
			if(m_rectPatch) {
				ResultFailed(m_pDevice->DrawRectPatch(0, (float*)m_numSegs[i], 
												      (D3DRECTPATCH_INFO*)&m_rectInfo[i]),
						     TEXT("IDirect3DDevice8::DrawRectPatch(0, ..., ...)"));
			}
			else {
				ResultFailed(m_pDevice->DrawTriPatch(0, (float*)m_numSegs[i], 
												     (D3DTRIPATCH_INFO*)&m_triInfo[i]),
						     TEXT("IDirect3DDevice8::DrawTriPatch(0, ..., ...)"));
			}
		}
	}
//    m_pDevice->SetTexture(0, NULL);
	if(g_cacheEnabled)
		g_useCache = TRUE;

	// Disable normalization of normals
	m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);

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
void CHop::ProcessInput() {
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

	// Query the state of the joysticks
    GetJoystickState(&js);

	m_camera.SetDrawAxis(FALSE);

	// Button 0: Make lighting fixed
	if(js.buttons[0]) {
		if(!b0) {
			b0		= TRUE;
			m_camera.EnableLight(TRUE, TRUE);
		}
	}
	else
		b0 = FALSE;

	// Button 1: Next state
	if(js.buttons[1]) {
		if(!b1) {
			b1	= TRUE;
			++g_state;
			if(g_state > g_maxState)
				g_state = 0;
		}
	}
	else
		b1 = FALSE;

	// Button 2: Cull modes
	if(js.buttons[2]) {
		if(!b2) {
			b2	= TRUE;
			switch(g_cull)
			{
			case D3DCULL_NONE:
				g_cull = D3DCULL_CW;
				break;
			case D3DCULL_CW:
				g_cull = D3DCULL_CCW;
				break;
			default:
				g_cull = D3DCULL_NONE;
				break;
			}
		}
	}
	else
		b2 = FALSE;

	// Button 4: Prev state
	if(js.buttons[4]) {
		if(!b4) {
			b4	= TRUE;
			--g_state;
			if(g_state < 0)
				g_state = g_maxState;
		}
	}
	else
		b4 = FALSE;

	// Button 3: Use/Don't use cache
	if(js.buttons[3]) {
		if(!b3) {
			b3	= TRUE;
			g_cacheEnabled = !g_cacheEnabled;
			if(!g_cacheEnabled)
				g_useCache = FALSE;
		}
	}
	else
		b3 = FALSE;

	// Button 5: Use/Don't use two sided lighting
	if(js.buttons[5]) {
		if(!b5) {
			b5	= TRUE;
			g_twoSided= !g_twoSided;
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

		// Is button 10 down? Update light
		if(js.buttons[10]) {
			m_camera.EnableLight(TRUE, FALSE);

			// Joystick 2 rotates around X and Y
			rotateX = js.f2Y * m_rotateXFactor;
			if(rotateX != 0.0f)
				m_camera.RotateLightX(rotateX);

			rotateY = js.f2X * m_rotateYFactor;
			if(rotateY != 0.0f)
				m_camera.RotateLightY(-rotateY);
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
BOOL CHop::InitView() {
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
BOOL CHop::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;
#ifndef UNDER_XBOX
	static BOOL fixed = TRUE;
	static BOOL wireframe = FALSE;

	switch(uMsg) {
	case WM_KEYUP:
		m_camera.SetDrawAxis(FALSE);
		return 0;
		
	case WM_KEYDOWN:
		m_camera.SetDrawAxis(TRUE);
		if(wParam == VK_NUMPAD1)
			m_camera.Zoom(c_zoom);
		else if(wParam == VK_NUMPAD2)
			m_camera.RotateX(-c_rotateX);
		else if(wParam == VK_NUMPAD3)
			m_camera.RotateZ(c_rotateZ);
		else if(wParam == VK_NUMPAD4)
			m_camera.RotateY(-c_rotateY);
		else if(wParam == VK_NUMPAD6)
			m_camera.RotateY(c_rotateY);
		else if(wParam == VK_NUMPAD7)
			m_camera.Zoom(-c_zoom);
		else if(wParam == VK_NUMPAD8)
			m_camera.RotateX(c_rotateX);
		else if(wParam == VK_NUMPAD9)
			m_camera.RotateZ(-c_rotateZ);
		else if(wParam == 0x46) {
			fixed	= !fixed;
			m_camera.EnableLight(TRUE, fixed);
		}
		else if(wParam == VK_UP)
			m_camera.RotateLightX(-c_rotateX);
		else if(wParam == VK_DOWN)
			m_camera.RotateLightX(c_rotateX);
		else if(wParam == VK_LEFT)
			m_camera.RotateLightY(c_rotateY);
		else if(wParam == VK_RIGHT)
			m_camera.RotateLightY(-c_rotateY);
		else if(wParam == VK_ADD) {
			++g_state;
			if(g_state > g_maxState)
				g_state = 0;
		}
		else if(wParam == VK_SUBTRACT) {
			--g_state;
			if(g_state < 0)
				g_state = g_maxState;
		}
		else if(wParam == 0x57) {
			wireframe = !wireframe;
			if(wireframe) 
				m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			else 
				m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}
		return 0;
	}
#endif // !UNDER_XBOX

    return CScene::WndProc(plr, hWnd, uMsg, wParam, lParam);
}

//------------------------------------------------------------------------------
//	CHop::CalcMinMax
//------------------------------------------------------------------------------
void
CHop::CalcMinMax(
				 OUT	D3DXVECTOR3&		min,
				 OUT	D3DXVECTOR3&		max,
				 IN		const HopVertex*	pVerts,
				 IN		UINT				numVerts
				 )
/*++

Routine Description:

	Calculates the min and max values of a set of vertices

Arguments:

	OUT min			- Minimum values
	OUT max			- Maximum values
	IN	pVerts		- Vertices
	IN	numVerts	- Number of vertices

Return Value:

	None

--*/
{
	min = D3DXVECTOR3(1.0e10f, 1.0e10f, 1.0e10f);
	max = D3DXVECTOR3(-1.0e10f, -1.0e10f, -1.0e10f);

	for(UINT i=0; i<numVerts; ++i) {
		if(pVerts[i].x < min.x)
			min.x = pVerts[i].x;
		if(pVerts[i].y < min.y)
			min.y = pVerts[i].y;
		if(pVerts[i].z < min.z)
			min.z = pVerts[i].z;
		if(pVerts[i].x > max.x)
			max.x = pVerts[i].x;
		if(pVerts[i].y > max.y)
			max.y = pVerts[i].y;
		if(pVerts[i].z > max.z)
			max.z = pVerts[i].z;
	}
}

//------------------------------------------------------------------------------
//	CHop::ChangeState
//------------------------------------------------------------------------------
void 
CHop::ChangeState(void)
/*++

Routine Description:

	Changes the rendering state by loading a different hop

Arguments:

	None

Return Value:

	None

--*/
{
	D3DXVECTOR3		min;
	D3DXVECTOR3		max; 
	float			dxyz;
	UINT			i;
	HopColorVertex*	frameVertices;
	UINT			sizeofFrameVertices;
	WORD*			frameIndices;
	UINT			sizeofFrameIndices;
	HopVertex*		patchVertices;
	UINT			sizeofPatchVertices;
	float			fx;
	float			fy;
	float			xInc;
	float			yInc;
	UINT			v;
	UINT			x;
	UINT			y;
	float			numCopiesX;
	float			numCopiesY;

	// Release old buffers
	if(m_frameVertices)
	{
		m_frameVertices->Release();
		m_frameVertices = NULL;
	}
	if(m_frameIndex)
	{
		m_frameIndex->Release();
		m_frameIndex = NULL;
	}
	if(m_hopVertices)
	{
		m_hopVertices->Release();
		m_hopVertices = NULL;
	}

	// Set the data based on the current state
	switch(g_state)
	{
	case 0:	// Linear Bezier Rect Patch
		m_rectPatch			= TRUE;
		frameVertices		= RectLinearBezierFrameVertices;
		sizeofFrameVertices	= sizeof(RectLinearBezierFrameVertices);
		frameIndices		= RectLinearBezierFrameIndices;
		sizeofFrameIndices	= sizeof(RectLinearBezierFrameIndices);
		patchVertices		= RectLinearBezierVertices;
		sizeofPatchVertices	= sizeof(RectLinearBezierVertices);
		m_numVerts			= RectLinearBezierNumVertices;
		m_numLines			= RectLinearBezierNumLines;
		m_numPatches		= RectLinearBezierNumPatches;
		m_rectInfo			= RectLinearBezierInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = RectLinearBezierNumSegments[i];
		break;

	case 1:	// Cubic Bezier Rect Patch
		m_rectPatch			= TRUE;
		frameVertices		= RectCubicBezierFrameVertices;
		sizeofFrameVertices	= sizeof(RectCubicBezierFrameVertices);
		frameIndices		= RectCubicBezierFrameIndices;
		sizeofFrameIndices	= sizeof(RectCubicBezierFrameIndices);
		patchVertices		= RectCubicBezierVertices;
		sizeofPatchVertices	= sizeof(RectCubicBezierVertices);
		m_numVerts			= RectCubicBezierNumVertices;
		m_numLines			= RectCubicBezierNumLines;
		m_numPatches		= RectCubicBezierNumPatches;
		m_rectInfo			= RectCubicBezierInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = RectCubicBezierNumSegments[i];
		break;

	case 2:	// Quintic Bezier Rect Patch
		m_rectPatch			= TRUE;
		frameVertices		= RectQuinticBezierFrameVertices;
		sizeofFrameVertices	= sizeof(RectQuinticBezierFrameVertices);
		frameIndices		= RectQuinticBezierFrameIndices;
		sizeofFrameIndices	= sizeof(RectQuinticBezierFrameIndices);
		patchVertices		= RectQuinticBezierVertices;
		sizeofPatchVertices	= sizeof(RectQuinticBezierVertices);
		m_numVerts			= RectQuinticBezierNumVertices;
		m_numLines			= RectQuinticBezierNumLines;
		m_numPatches		= RectQuinticBezierNumPatches;
		m_rectInfo			= RectQuinticBezierInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = RectQuinticBezierNumSegments[i];
		break;

	case 3:	// Linear B-Spline Rect Patch
		m_rectPatch			= TRUE;
		frameVertices		= RectLinearBSplineFrameVertices;
		sizeofFrameVertices	= sizeof(RectLinearBSplineFrameVertices);
		frameIndices		= RectLinearBSplineFrameIndices;
		sizeofFrameIndices	= sizeof(RectLinearBSplineFrameIndices);
		patchVertices		= RectLinearBSplineVertices;
		sizeofPatchVertices	= sizeof(RectLinearBSplineVertices);
		m_numVerts			= RectLinearBSplineNumVertices;
		m_numLines			= RectLinearBSplineNumLines;
		m_numPatches		= RectLinearBSplineNumPatches;
		m_rectInfo			= RectLinearBSplineInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = RectLinearBSplineNumSegments[i];
		break;

	case 4:	// Cubic B-Spline Rect Patch
		m_rectPatch			= TRUE;
		frameVertices		= RectCubicBSplineFrameVertices;
		sizeofFrameVertices	= sizeof(RectCubicBSplineFrameVertices);
		frameIndices		= RectCubicBSplineFrameIndices;
		sizeofFrameIndices	= sizeof(RectCubicBSplineFrameIndices);
		patchVertices		= RectCubicBSplineVertices;
		sizeofPatchVertices	= sizeof(RectCubicBSplineVertices);
		m_numVerts			= RectCubicBSplineNumVertices;
		m_numLines			= RectCubicBSplineNumLines;
		m_numPatches		= RectCubicBSplineNumPatches;
		m_rectInfo			= RectCubicBSplineInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = RectCubicBSplineNumSegments[i];
		break;

	case 5:	// Quintic B-Spline Rect Patch
		m_rectPatch			= TRUE;
		frameVertices		= RectQuinticBSplineFrameVertices;
		sizeofFrameVertices	= sizeof(RectQuinticBSplineFrameVertices);
		frameIndices		= RectQuinticBSplineFrameIndices;
		sizeofFrameIndices	= sizeof(RectQuinticBSplineFrameIndices);
		patchVertices		= RectQuinticBSplineVertices;
		sizeofPatchVertices	= sizeof(RectQuinticBSplineVertices);
		m_numVerts			= RectQuinticBSplineNumVertices;
		m_numLines			= RectQuinticBSplineNumLines;
		m_numPatches		= RectQuinticBSplineNumPatches;
		m_rectInfo			= RectQuinticBSplineInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = RectQuinticBSplineNumSegments[i];
		break;

	case 6: // Linear Interpolate Rect Patch
		m_rectPatch			= TRUE;
		frameVertices		= RectLinearInterpolateFrameVertices;
		sizeofFrameVertices	= sizeof(RectLinearInterpolateFrameVertices);
		frameIndices		= RectLinearInterpolateFrameIndices;
		sizeofFrameIndices	= sizeof(RectLinearInterpolateFrameIndices);
		patchVertices		= RectLinearInterpolateVertices;
		sizeofPatchVertices	= sizeof(RectLinearInterpolateVertices);
		m_numVerts			= RectLinearInterpolateNumVertices;
		m_numLines			= RectLinearInterpolateNumLines;
		m_numPatches		= RectLinearInterpolateNumPatches;
		m_rectInfo			= RectLinearInterpolateInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = RectLinearInterpolateNumSegments[i];
		break;

	case 7: // Cubic Interpolate Rect Patch
		m_rectPatch			= TRUE;
		frameVertices		= RectCubicInterpolateFrameVertices;
		sizeofFrameVertices	= sizeof(RectCubicInterpolateFrameVertices);
		frameIndices		= RectCubicInterpolateFrameIndices;
		sizeofFrameIndices	= sizeof(RectCubicInterpolateFrameIndices);
		patchVertices		= RectCubicInterpolateVertices;
		sizeofPatchVertices	= sizeof(RectCubicInterpolateVertices);
		m_numVerts			= RectCubicInterpolateNumVertices;
		m_numLines			= RectCubicInterpolateNumLines;
		m_numPatches		= RectCubicInterpolateNumPatches;
		m_rectInfo			= RectCubicInterpolateInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = RectCubicInterpolateNumSegments[i];
		break;

	case 8: // Linear Bezier Tri Patch
		m_rectPatch			= FALSE;
		frameVertices		= TriLinearBezierFrameVertices;
		sizeofFrameVertices	= sizeof(TriLinearBezierFrameVertices);
		frameIndices		= TriLinearBezierFrameIndices;
		sizeofFrameIndices	= sizeof(TriLinearBezierFrameIndices);
		patchVertices		= TriLinearBezierVertices;
		sizeofPatchVertices	= sizeof(TriLinearBezierVertices);
		m_numVerts			= TriLinearBezierNumVertices;
		m_numLines			= TriLinearBezierNumLines;
		m_numPatches		= TriLinearBezierNumPatches;
		m_triInfo			= TriLinearBezierInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = TriLinearBezierNumSegments[i];
		break;

	case 9: // Cubic Bezier Tri Patch
		m_rectPatch			= FALSE;
		frameVertices		= TriCubicBezierFrameVertices;
		sizeofFrameVertices	= sizeof(TriCubicBezierFrameVertices);
		frameIndices		= TriCubicBezierFrameIndices;
		sizeofFrameIndices	= sizeof(TriCubicBezierFrameIndices);
		patchVertices		= TriCubicBezierVertices;
		sizeofPatchVertices	= sizeof(TriCubicBezierVertices);
		m_numVerts			= TriCubicBezierNumVertices;
		m_numLines			= TriCubicBezierNumLines;
		m_numPatches		= TriCubicBezierNumPatches;
		m_triInfo			= TriCubicBezierInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = TriCubicBezierNumSegments[i];
		break;

	case 10: // Quintic Bezier Tri Patch
		m_rectPatch			= FALSE;
		frameVertices		= TriQuinticBezierFrameVertices;
		sizeofFrameVertices	= sizeof(TriQuinticBezierFrameVertices);
		frameIndices		= TriQuinticBezierFrameIndices;
		sizeofFrameIndices	= sizeof(TriQuinticBezierFrameIndices);
		patchVertices		= TriQuinticBezierVertices;
		sizeofPatchVertices	= sizeof(TriQuinticBezierVertices);
		m_numVerts			= TriQuinticBezierNumVertices;
		m_numLines			= TriQuinticBezierNumLines;
		m_numPatches		= TriQuinticBezierNumPatches;
		m_triInfo			= TriQuinticBezierInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = TriQuinticBezierNumSegments[i];
		break;

#if 0 // (currently) Unsupported surfaces
	case 11:	// Quintic Interpolate Rect Patch
		m_rectPatch			= TRUE;
		frameVertices		= RectQuinticInterpolateFrameVertices;
		sizeofFrameVertices	= sizeof(RectQuinticInterpolateFrameVertices);
		frameIndices		= RectQuinticInterpolateFrameIndices;
		sizeofFrameIndices	= sizeof(RectQuinticInterpolateFrameIndices);
		patchVertices		= RectQuinticInterpolateVertices;
		sizeofPatchVertices	= sizeof(RectQuinticInterpolateVertices);
		m_numVerts			= RectQuinticInterpolateNumVertices;
		m_numLines			= RectQuinticInterpolateNumLines;
		m_numPatches		= RectQuinticInterpolateNumPatches;
		m_rectInfo			= RectQuinticInterpolateInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = RectQuinticInterpolateNumSegments[i];
		break;

	case 9: // Linear BSpline Tri Patch
		m_rectPatch			= FALSE;
		frameVertices		= TriLinearBSplineFrameVertices;
		sizeofFrameVertices	= sizeof(TriLinearBSplineFrameVertices);
		frameIndices		= TriLinearBSplineFrameIndices;
		sizeofFrameIndices	= sizeof(TriLinearBSplineFrameIndices);
		patchVertices		= TriLinearBSplineVertices;
		sizeofPatchVertices	= sizeof(TriLinearBSplineVertices);
		m_numVerts			= TriLinearBSplineNumVertices;
		m_numLines			= TriLinearBSplineNumLines;
		m_numPatches		= TriLinearBSplineNumPatches;
		m_triInfo			= TriLinearBSplineInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = TriLinearBSplineNumSegments[i];
		break;

	case 10: // Cubic BSpline Tri Patch
		m_rectPatch			= FALSE;
		frameVertices		= TriCubicBSplineFrameVertices;
		sizeofFrameVertices	= sizeof(TriCubicBSplineFrameVertices);
		frameIndices		= TriCubicBSplineFrameIndices;
		sizeofFrameIndices	= sizeof(TriCubicBSplineFrameIndices);
		patchVertices		= TriCubicBSplineVertices;
		sizeofPatchVertices	= sizeof(TriCubicBSplineVertices);
		m_numVerts			= TriCubicBSplineNumVertices;
		m_numLines			= TriCubicBSplineNumLines;
		m_numPatches		= TriCubicBSplineNumPatches;
		m_triInfo			= TriCubicBSplineInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = TriCubicBSplineNumSegments[i];
		break;

	case 11: // Quintic BSpline Tri Patch
		m_rectPatch			= FALSE;
		frameVertices		= TriQuinticBSplineFrameVertices;
		sizeofFrameVertices	= sizeof(TriQuinticBSplineFrameVertices);
		frameIndices		= TriQuinticBSplineFrameIndices;
		sizeofFrameIndices	= sizeof(TriQuinticBSplineFrameIndices);
		patchVertices		= TriQuinticBSplineVertices;
		sizeofPatchVertices	= sizeof(TriQuinticBSplineVertices);
		m_numVerts			= TriQuinticBSplineNumVertices;
		m_numLines			= TriQuinticBSplineNumLines;
		m_numPatches		= TriQuinticBSplineNumPatches;
		m_triInfo			= TriQuinticBSplineInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = TriQuinticBSplineNumSegments[i];
		break;
		
	case -15: // Linear Interpolate Tri Patch
		m_rectPatch			= FALSE;
		frameVertices		= TriLinearInterpolateFrameVertices;
		sizeofFrameVertices	= sizeof(TriLinearInterpolateFrameVertices);
		frameIndices		= TriLinearInterpolateFrameIndices;
		sizeofFrameIndices	= sizeof(TriLinearInterpolateFrameIndices);
		patchVertices		= TriLinearInterpolateVertices;
		sizeofPatchVertices	= sizeof(TriLinearInterpolateVertices);
		m_numVerts			= TriLinearInterpolateNumVertices;
		m_numLines			= TriLinearInterpolateNumLines;
		m_numPatches		= TriLinearInterpolateNumPatches;
		m_triInfo			= TriLinearInterpolateInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = TriLinearInterpolateNumSegments[i];
		break;

	case -16: // Cubic Interpolate Tri Patch
		m_rectPatch			= FALSE;
		frameVertices		= TriCubicInterpolateFrameVertices;
		sizeofFrameVertices	= sizeof(TriCubicInterpolateFrameVertices);
		frameIndices		= TriCubicInterpolateFrameIndices;
		sizeofFrameIndices	= sizeof(TriCubicInterpolateFrameIndices);
		patchVertices		= TriCubicInterpolateVertices;
		sizeofPatchVertices	= sizeof(TriCubicInterpolateVertices);
		m_numVerts			= TriCubicInterpolateNumVertices;
		m_numLines			= TriCubicInterpolateNumLines;
		m_numPatches		= TriCubicInterpolateNumPatches;
		m_triInfo			= TriCubicInterpolateInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = TriCubicInterpolateNumSegments[i];
		break;

	case -17: // Quintic Interpolate Tri Patch
		m_rectPatch			= FALSE;
		frameVertices		= TriQuinticInterpolateFrameVertices;
		sizeofFrameVertices	= sizeof(TriQuinticInterpolateFrameVertices);
		frameIndices		= TriQuinticInterpolateFrameIndices;
		sizeofFrameIndices	= sizeof(TriQuinticInterpolateFrameIndices);
		patchVertices		= TriQuinticInterpolateVertices;
		sizeofPatchVertices	= sizeof(TriQuinticInterpolateVertices);
		m_numVerts			= TriQuinticInterpolateNumVertices;
		m_numLines			= TriQuinticInterpolateNumLines;
		m_numPatches		= TriQuinticInterpolateNumPatches;
		m_triInfo			= TriQuinticInterpolateInfo;
		for(i=0; i<m_numPatches; ++i)
			m_numSegs[i] = TriQuinticInterpolateNumSegments[i];
		break;
#endif

	default:	// This should never happen
		__asm int 3;
		g_state = 0;
		ChangeState();
		return;
	}

	// Create the vertex shader for the hop's
	if(m_hVShader == 0) {
		m_pDevice->CreateVertexShader((unsigned long*)VertexShader, NULL, 
									  &m_hVShader, 0);
	}

	// Generate the texture coordinates
	if(m_rectPatch) {

		// Texture coordinate increments
		numCopiesX	= 1.0f;
		numCopiesY	= 1.0f;
		xInc = numCopiesX / (float)(m_rectInfo->Width - 1);
		yInc = numCopiesY / (float)(m_rectInfo->Height - 1);

		// Loop to generate the texture coords
		for(v=0, i=0; i<m_numPatches; ++i) {
			for(fx=0.0, x=0; x<m_rectInfo->Width; ++x, fx+=xInc) {
				for(fy=0.0, y=0; y<m_rectInfo->Height; ++y, ++v, fy+=yInc) {
					patchVertices[v].tu = fx;
					patchVertices[v].tv = fy;
				}
			}
		}
	}
	else {
		
	}

	// Create the buffers
	m_frameVertices	= CreateVertexBuffer(m_pDevice, (void*)frameVertices, 
										 sizeofFrameVertices, 0, 
										 HOP_COLORVERTEX);
	m_frameIndex	= CreateIndexBuffer(m_pDevice, (void*)frameIndices, 
										sizeofFrameIndices, 0);
	m_hopVertices	= CreateVertexBuffer(m_pDevice, (void*)patchVertices, 
										 sizeofPatchVertices, D3DUSAGE_RTPATCHES,
										 HOP_VERTEX);
	CalcMinMax(min, max, patchVertices, m_numVerts);

	// Setup the camera based on the hop's bounds
	dxyz = MAX3(max.x - min.x, max.y - min.y, max.z - min.z);
	m_camera.SetViewport(0, 0, m_pDisplay->GetWidth(), m_pDisplay->GetHeight(),
						 0.0f, 1.0f);
	m_camera.LookAt(D3DXVECTOR3(((max + min) / 2.0f) - 
					D3DXVECTOR3(0.0f, 0.0f, dxyz * 3.0f)),
					D3DXVECTOR3((max + min) / 2.0f), 
					D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	m_camera.SetPerspectiveFov(45.0f, (float)m_pDisplay->GetWidth() / 
									  (float)m_pDisplay->GetHeight(),
							   0.0001f, 100.0f);

	// Save the current state
	g_lastState = g_state;

	// Reset cache flag
	g_useCache = FALSE;
}
