/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vistest2.cpp

Author:

    Robert Heitkamp

Description:

    Visibility Test functions.

*******************************************************************************/

#include "d3dlocus.h"
#include "vistest2.h"

//------------------------------------------------------------------------------
//	Constants
//------------------------------------------------------------------------------
static const unsigned int testMinX		= 0;
static const unsigned int testMaxX		= 1;
static const unsigned int testMinY		= 2;
static const unsigned int testMaxY		= 3;
static const unsigned int obscureMinX	= 4;
static const unsigned int obscureMaxX	= 5;
static const unsigned int obscureMinY	= 6;
static const unsigned int obscureMaxY	= 7;

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

    CVisibilityTest*	pTest;
    BOOL							bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pTest = new CVisibilityTest();
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
// CVisibilityTest
//******************************************************************************

//******************************************************************************
//
// Method:
//
//     CVisibilityTest
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
CVisibilityTest::CVisibilityTest() 
{
	//__asm int 3
}

//******************************************************************************
//
// Method:
//
//     ~CVisibilityTest
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
CVisibilityTest::~CVisibilityTest() {
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
BOOL CVisibilityTest::Create(CDisplay* pDisplay) {

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
int 
CVisibilityTest::Exhibit(int *pnExitCode) {
    BOOL            bMsgReady;
    float           fTime, fLastTime, fPrevTime, fTimeFreq;
    LARGE_INTEGER   qwCounter;
    DWORD           dwNumFrames = 0;

    if (!m_bCreated) 
        return TRUE;

    QueryPerformanceFrequency(&qwCounter);
    fTimeFreq = 1.0f / (float)qwCounter.QuadPart;
    QueryPerformanceCounter(&qwCounter);
    fLastTime = (float)qwCounter.QuadPart * fTimeFreq;
    fPrevTime = fLastTime;

	do
	{
		// and draw it
		QueryPerformanceCounter(&qwCounter);
		fTime = (float)qwCounter.QuadPart * fTimeFreq;

		// Process user input
		m_pDisplay->ProcessInput();
		if (!m_pDisplay->ConsoleVisible()) 
			ProcessInput();

		if (m_pDisplay->m_bTimeSync) 
			m_pDisplay->m_bTimeSync = FALSE;
		else 
		{
			m_fTime += (fTime - fLastTime) * m_fTimeDilation;
			m_fFrame += 1.0f;//(1.0f * m_fTimeDilation); // ##REVIEW
			if (m_fFrameDuration != FLT_INFINITE && m_fFrame > m_fFrameDuration) 
				break;
			if (m_fTimeDuration != FLT_INFINITE && m_fTime > m_fTimeDuration) 
				break;
			m_fTimeDelta = m_fTime - m_fLastTime;
			m_fFrameDelta = m_fFrame - m_fLastFrame;
			Update();
			m_fLastTime = m_fTime;
			m_fLastFrame = m_fFrame;
		}

		fLastTime = fTime;

		if (!Render()) 
			break;

		dwNumFrames++;

		if (fTime - fPrevTime > 1.0f) 
		{
			m_fFPS = (float)dwNumFrames / (fTime - fPrevTime);
			fPrevTime = fTime;
			dwNumFrames = 0;
		}
	} while(!m_bQuit);

    if (pnExitCode) 
        *pnExitCode = 0;
    
    return m_bDisplayOK;;
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
BOOL CVisibilityTest::Prepare() {
	float	intervalX;
	float	intervalY;

	// Setup indices 
	m_indices[0] = 0;
	m_indices[1] = 1;
	m_indices[2] = 0;
	m_indices[3] = 1;
	m_indices[4] = 0;
	m_indices[5] = 1;
	m_indices[6] = 0;
	m_indices[7] = 1;

	// Store local copy of width and height
	m_width		= m_pDisplay->GetWidth();
	m_height	= m_pDisplay->GetHeight();
	intervalY	= (float)(m_height / 9);
	intervalX	= (float)(m_width / 9);

	// Y coordinates of test triangles
	m_testY[0]	= 0.0f;					
	m_testY[1]	= intervalY * 3.0f;
	m_testY[2]	= intervalY * 6.0f;
	m_testY[3]	= (float)m_height;

	// X coordinates of test triangles
	m_testX[0]	= 0.0f;					
	m_testX[1]	= intervalX * 3.0f;
	m_testX[2]	= intervalX * 6.0f;
	m_testX[3]	= (float)m_width;
	
	// Y coordinates of obscure triangles
	m_obscureY[0]	= 0.0f;					
	m_obscureY[1]	= intervalY;
	m_obscureY[2]	= intervalY * 2.0f;
	m_obscureY[3]	= intervalY * 3.0f;
	m_obscureY[4]	= intervalY * 4.0f;
	m_obscureY[5]	= intervalY * 5.0f;
	m_obscureY[6]	= intervalY * 6.0f;
	m_obscureY[7]	= intervalY * 7.0f;
	m_obscureY[8]	= intervalY * 8.0f;
	m_obscureY[9]	= (float)m_height;

	// X coordinates of obscure triangles
	m_obscureX[0]	= 0.0f;					
	m_obscureX[1]	= intervalX;
	m_obscureX[2]	= intervalX * 2.0f;
	m_obscureX[3]	= intervalX * 3.0f;
	m_obscureX[4]	= intervalX * 4.0f;
	m_obscureX[5]	= intervalX * 5.0f;
	m_obscureX[6]	= intervalX * 6.0f;
	m_obscureX[7]	= intervalX * 7.0f;
	m_obscureX[8]	= intervalX * 8.0f;
	m_obscureX[9]	= (float)m_width;

	// Setup color and rhw component of triangles
	for(int i=0; i<6; ++i)
	{
		m_testTris[i].rhw	= 1.0f;
		m_testTris[i].color	= 0xff0000ff;

		m_obscureTris[i].rhw	= 1.0f;
		m_obscureTris[i].color	= 0xffff0000;
	}

	// Test Id to query
	m_testId = -1;

	// Test state to run
	m_stage = 0;

	m_nextStage = 1;

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
BOOL CVisibilityTest::Setup() {

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
BOOL CVisibilityTest::Initialize() {
	D3DPRESENT_PARAMETERS	d3dpp;

    m_pDisplay->GetPresentParameters(&d3dpp);
	CopyMemory(&m_savedD3dpp, &d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	m_pDevice->Reset(&d3dpp);
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
void CVisibilityTest::Efface() {
	m_pDevice->Reset(&m_savedD3dpp);
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
void CVisibilityTest::Update() {
	UINT	minX;
	UINT	maxX;
	UINT	minY;
	UINT	maxY;
	UINT	osTestMinX;
	UINT	osTestMaxX;
	UINT	osTestMinY;
	UINT	osTestMaxY;
	UINT	osObscureMinX;
	UINT	osObscureMaxX;
	UINT	osObscureMinY;
	UINT	osObscureMaxY;
	UINT	width1	= m_width - 1;
	UINT	height1	= m_height - 1;

	// Not created? do nothing
	if(!m_bCreated)
		return;
    
	// Adjust the stage
	m_stage = m_nextStage;

	switch(m_stage)
	{
	case 1:	// Basic functionality testing
		{
			// Setup the triangles
			m_testTris[0].x = m_testX[m_indices[testMinX]];
			m_testTris[0].y = m_testY[m_indices[testMaxY]];
			m_testTris[0].z	= 0.75f;

			m_testTris[1].x = m_testX[m_indices[testMinX]];
			m_testTris[1].y = m_testY[m_indices[testMinY]];
			m_testTris[1].z	= 0.75f;

			m_testTris[2].x = m_testX[m_indices[testMaxX]];
			m_testTris[2].y = m_testY[m_indices[testMaxY]];
			m_testTris[2].z	= 0.75f;

			m_testTris[3].x = m_testX[m_indices[testMinX]];
			m_testTris[3].y = m_testY[m_indices[testMinY]];
			m_testTris[3].z	= 0.75f;

			m_testTris[4].x = m_testX[m_indices[testMaxX]];
			m_testTris[4].y = m_testY[m_indices[testMinY]];
			m_testTris[4].z	= 0.75f;

			m_testTris[5].x = m_testX[m_indices[testMaxX]];
			m_testTris[5].y = m_testY[m_indices[testMaxY]];
			m_testTris[5].z	= 0.75f;

			m_obscureTris[0].x	= m_obscureX[m_indices[obscureMinX]];
			m_obscureTris[0].y	= m_obscureY[m_indices[obscureMaxY]];
			m_obscureTris[0].z	= 0.25f;

			m_obscureTris[1].x	= m_obscureX[m_indices[obscureMinX]];
			m_obscureTris[1].y	= m_obscureY[m_indices[obscureMinY]];
			m_obscureTris[1].z	= 0.25f;

			m_obscureTris[2].x	= m_obscureX[m_indices[obscureMaxX]];
			m_obscureTris[2].y	= m_obscureY[m_indices[obscureMaxY]];
			m_obscureTris[2].z	= 0.25f;

			m_obscureTris[3].x	= m_obscureX[m_indices[obscureMinX]];
			m_obscureTris[3].y	= m_obscureY[m_indices[obscureMinY]];
			m_obscureTris[3].z	= 0.25f;

			m_obscureTris[4].x	= m_obscureX[m_indices[obscureMaxX]];
			m_obscureTris[4].y	= m_obscureY[m_indices[obscureMinY]];
			m_obscureTris[4].z	= 0.25f;

			m_obscureTris[5].x	= m_obscureX[m_indices[obscureMaxX]];
			m_obscureTris[5].y	= m_obscureY[m_indices[obscureMaxY]];
			m_obscureTris[5].z	= 0.25f;

			// -----Calculate the number of visible pixels-----

			// Shotcuts for clearer code
			osTestMinX		= (UINT)m_testX[m_indices[testMinX]];
			osTestMaxX		= (UINT)m_testX[m_indices[testMaxX]];
			osTestMinY		= (UINT)m_testY[m_indices[testMinY]];
			osTestMaxY		= (UINT)m_testY[m_indices[testMaxY]];
			osObscureMinX	= (UINT)m_obscureX[m_indices[obscureMinX]];
			osObscureMaxX	= (UINT)m_obscureX[m_indices[obscureMaxX]];
			osObscureMinY	= (UINT)m_obscureY[m_indices[obscureMinY]];
			osObscureMaxY	= (UINT)m_obscureY[m_indices[obscureMaxY]];

			// Is box completely offscreen or
			// does the obscuring box completely cover the screen or
			// does the obscuring box completely cover the test box?
			if((osTestMinX > osTestMaxX) || (osTestMinY > osTestMaxY) ||
			   ((osObscureMinX == 0) && (osObscureMinY == 0) &&
				(osObscureMaxX == width1) && (osObscureMaxY == height1)) ||
			   ((osObscureMinX <= osTestMinX) && (osObscureMinY <= osTestMinY) &&
				(osObscureMaxX >= osTestMaxX) && (osObscureMaxY >= osTestMaxY)))
			{
				m_uVisible = 0;
			}

			// Is the obscuring box completely offscreen or
			// does the obscuring box not obscure the test box at all?
			else if((osObscureMinX > osObscureMaxX) || (osObscureMinY > osObscureMaxY) ||
					(osObscureMaxX < osTestMinX) || (osObscureMaxY < osTestMinY) ||
					(osObscureMinX > osTestMaxX) || (osObscureMinY > osTestMaxY))
			{
				m_uVisible = (osTestMaxX - osTestMinX) * 
							 (osTestMaxY - osTestMinY);
			}

			// Calculate the unobscured area
			else
			{
				// Intersection of two boxes (obscured area)
				minX = Max(osTestMinX, osObscureMinX);
				minY = Max(osTestMinY, osObscureMinY);
				maxX = Min(osTestMaxX, osObscureMaxX);
				maxY = Min(osTestMaxY, osObscureMaxY);

				// Unobscured pixels
				m_uVisible = ((osTestMaxX - osTestMinX) * 
							  (osTestMaxY - osTestMinY)) - 
							 ((maxX - minX) * (maxY - minY));
			}	

			// Test Id
			// Note: 0-9 are reserved for hard coded valid tests
			// 10-19 are reserved for invalid test and should never
			// be used with EndVisibilityTest()
			++m_testId;

			// The following number is taken from REPORTS_MAX_INDEX in
			// private\windows\directx\dxg\d3d8\se\device.h
			// I don't believe this header file is publicly available
			// and will need to be checked over time to ensure for a valid
			// test id.
			if(m_testId >= 2048)
				m_testId = 20;

			// HACK: Bug in EndVisibilityTest() when passed id 768 
			// (maybe >= 768) to renable testing of id's >= 768, comment
			// or remove the next two lines
//			else if(m_testId >= 768)
//				m_testId = 20;

			// Save slots 10-19 for invalid tests
			else if(m_testId < 20)
				m_testId = 20;

			// Update for next test (TRUE == end of test)
			if(UpdateIndices())
				m_nextStage = m_stage + 1;;
		}
		break;

	case 2:	// Invalid parameters
		{
			m_testTris[0].x = 50.0f;
			m_testTris[0].y = 50.0f;
			m_testTris[0].z	= 0.75f;

			m_testTris[1].x = 100.0f;
			m_testTris[1].y = 50.0f;
			m_testTris[1].z	= 0.75f;

			m_testTris[2].x = 50.0f;
			m_testTris[2].y = 100.0f;
			m_testTris[2].z	= 0.75f;

			m_testTris[3].x = 50.0f;
			m_testTris[3].y = 100.0f;
			m_testTris[3].z	= 0.75f;

			m_testTris[4].x = 100.0f;
			m_testTris[4].y = 50.0f;
			m_testTris[4].z	= 0.75f;

			m_testTris[5].x = 100.0f;
			m_testTris[5].y = 100.0f;
			m_testTris[5].z	= 0.75f;

			// 50 x 50
			m_uVisible = 2500;
			m_nextStage = m_stage + 1;
		}
		break;

	case 3: // valid count
	case 4: // Overflow
		{
			m_testTris[0].x		= 0.0f;
			m_testTris[0].y		= 0.0f;
			m_testTris[0].z		= 1.0f;
			m_testTris[0].color	= 0xff00ff00;

			m_testTris[1].x		= (float)m_width;
			m_testTris[1].y		= 0.0f;
			m_testTris[1].z		= 1.0f;
			m_testTris[1].color	= 0xff00ff00;

			m_testTris[2].x		= 0.0f;
			m_testTris[2].y		= (float)m_height;
			m_testTris[2].z		= 1.0f;
			m_testTris[2].color	= 0xff00ff00;

			m_testTris[3].x		= 0.0f;
			m_testTris[3].y		= (float)m_height;
			m_testTris[3].z		= 1.0f;
			m_testTris[3].color	= 0xff00ff00;

			m_testTris[4].x		= (float)m_width;
			m_testTris[4].y		= 0.0f;
			m_testTris[4].z		= 1.0f;
			m_testTris[4].color	= 0xff00ff00;

			m_testTris[5].x		= (float)m_width;
			m_testTris[5].y		= (float)m_height;
			m_testTris[5].z		= 1.0f;
			m_testTris[5].color	= 0xff00ff00;

			m_nextStage = m_stage + 1;
		}
		break;

	case 5:	// Exit without end

		m_nextStage = m_stage + 1;
		break;

	default:
		// End the test
		m_bQuit = TRUE;
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
BOOL CVisibilityTest::Render() {
    if(!m_bCreated) 
        return FALSE;

#ifndef UNDER_XBOX
	HRESULT	hr;

    hr = m_pDevice->TestCooperativeLevel();
    if (FAILED(hr)) {
        if (hr == D3DERR_DEVICELOST) 
            return TRUE;
        if (hr == D3DERR_DEVICENOTRESET) {
            if (!Reset()) 
                return FALSE;
        }
    }
#endif // !UNDER_XBOX

	switch(m_stage) {
	case 1:
		RenderStage1();
		break;
	case 2:
		RenderStage2();
		break;
	case 3:
		RenderStage3();
		break;
	case 4:
		RenderStage4();
		break;
	case 5:
		RenderStage5();
		break;
	}

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
void CVisibilityTest::ProcessInput() {

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
BOOL CVisibilityTest::InitView() {

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
BOOL CVisibilityTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
//	CVisibilityTest::UpdateIndices
//------------------------------------------------------------------------------
BOOL 
CVisibilityTest::UpdateIndices(void)
/*++

Routine Description:

	Updates the index list to adjust for the next state of operation.
	There are 8 indices: 76543210
	Indices 7-4 are the states of the obscuring box.
	Indices 3-0 are the states of the obscuring box

Arguments:

	None

Return Value:

	TRUE when the indices have completely cycled through their entire
	range, FALSE otherwise.

--*/
{
	int		i;
	BOOL	reset;

	++m_indices[7];

	// Obscure m_indices
	for(i=7; i>=4; --i) {

		// Odd numbered m_indices cannot be > 9
		if(i & 1) {
			if(m_indices[i] > 9) {
				++m_indices[i-1];
				m_indices[i]	= 0;
				reset			= TRUE;
			}
			else
				reset = FALSE;
		}

		// Even numbered m_indices cannot be > 8
		else {
			if(m_indices[i] > 8) {
				++m_indices[i-1];
				m_indices[i] = 0;
			}

			if(reset)
				m_indices[i+1] = m_indices[i] + 1;
		}		
	}

	// Test m_indices
	for(; i>=0; --i) {

		// Odd numbered m_indices cannot be > 3
		if(i & 1) {
			if(m_indices[i] > 3) {
				++m_indices[i-1];
				m_indices[i]	= 0;
				reset			= TRUE;
			}
			else
				reset = FALSE;
		}

		// Even numbered m_indices cannot be > 2
		else {
			if(m_indices[i] > 2) {
				if(i) {
					++m_indices[i-1];
					m_indices[i] = 0;
				}
				else
					return TRUE;
			}

			if(reset)
				m_indices[i+1] = m_indices[i] + 1;
		}		
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	CVisibilityTest::Max
//------------------------------------------------------------------------------
UINT 
CVisibilityTest::Max(
					 IN	UINT a, 
					 IN	UINT b
					 )
/*++

Routine Description:

	Returns the max of two unsinged ints

Arguments:

	IN a	- First value
	IN b	- Second value

Return Value:

	The larger of the two values

--*/
{
	return (a >= b ? a : b);
}
	
//------------------------------------------------------------------------------
//	CVisibilityTest::Min
//------------------------------------------------------------------------------
UINT 
CVisibilityTest::Min(
					 IN	UINT a, 
					 IN	UINT b
					 )
/*++

Routine Description:

	Returns the min of two unsigned ints

Arguments:

	IN a	- First value
	IN b	- Second value

Return Value:

	The lesser of the two values

--*/
{
	return (a <= b ? a : b);
}

//------------------------------------------------------------------------------
//	CVisibilityTest::RenderStage1
//------------------------------------------------------------------------------
void
CVisibilityTest::RenderStage1(void)
/*++

Routine Description:

	Handles all the rendering for stage 1 testing
	This is mostly a stress test. aprox 72,899 tests are performed with 
	varying degrees of obscurity.

Arguments:

	None

Return Value:

	None

--*/
{
    DWORD	dwClearFlags = D3DCLEAR_TARGET;
    HRESULT	hr;
	UINT	uResult;
	BOOL	failure = FALSE;
	
    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) 
        dwClearFlags |= (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VISTEST_VERTEX);

	// Render obscuring triangles
	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, m_obscureTris, 
							  sizeof(VISTEST_VERTEX));

	// Setup for the test?
	hr = m_pDevice->GetIDirect3DDevice8()->BeginVisibilityTest();
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::BeginVisibilityTest")))
	{
		m_pDevice->EndScene();
		return;
	}

	// Render triangles to test
	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, m_testTris, 
							   sizeof(VISTEST_VERTEX));

	// Do the test?
	hr = m_pDevice->GetIDirect3DDevice8()->EndVisibilityTest(m_testId);
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::EndVisibilityTest")))
	{
		m_pDevice->EndScene();
		return;
	}

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

	// Query the test results
	hr = GetVisibilityTestResult(m_testId, uResult);
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::GetVisibilitTestResult")))
		failure = TRUE;

	// Is the count correct
	else if(uResult != m_uVisible)
	{
		WCHAR buffer[128];
		wsprintf(buffer, TEXT("vistest: RenderStage1: Incorrect pixel count: Result=%u  Expected=%u  Diff=%d"), 
				 uResult, m_uVisible, abs((long)uResult - (long)m_uVisible));
		Log(LOG_FAIL, buffer);
		__asm int 3;
	}
}

//------------------------------------------------------------------------------
//	CVisibilityTest::RenderStage2
//------------------------------------------------------------------------------
void
CVisibilityTest::RenderStage2(void)
/*++

Routine Description:

	Handles all the rendering for stage 2 testing
	Simple testing of api used in invalid manners

Arguments:

	None

Return Value:

	None

--*/
{
    DWORD	dwClearFlags = D3DCLEAR_TARGET;
    HRESULT	hr;
	UINT	uResult;

	// Test 1: Begin - End0 - Get1 -> Error

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) 
        dwClearFlags |= (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VISTEST_VERTEX);

	// Setup for the test?
	hr = m_pDevice->GetIDirect3DDevice8()->BeginVisibilityTest();
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::BeginVisibilityTest")))
	{
		m_pDevice->EndScene();
		return;
	}

	// Render triangles to test
	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, m_testTris, 
							   sizeof(VISTEST_VERTEX));

	// Valid call
	hr = m_pDevice->GetIDirect3DDevice8()->EndVisibilityTest(0);
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::EndVisibilityTest")))
	{
		m_pDevice->EndScene();
		return;
	}

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

	// Invalid query the test results
#if !defined(DEBUG) && !defined(_DEBUG)
	hr = GetVisibilityTestResult(5, uResult);
	if(SUCCEEDED(hr))
	{
		Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVisibilitTestResult failed by not failing"));
		return;
	}
#endif

	// Valid call
	hr = m_pDevice->GetIDirect3DDevice8()->EndVisibilityTest(1);
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::EndVisibilityTest")))
		return;

	// Valid query the test results
	hr = GetVisibilityTestResult(0, uResult);
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::GetVisibilityTestResult")))
		return;
	else if(uResult != m_uVisible)
		return;

	// Valid query the test results
	hr = GetVisibilityTestResult(1, uResult);
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::GetVisibilityTestResult")))
		return;
	else if(uResult != m_uVisible)
		return;

	// Test 2: Get3 -> Error

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) 
        dwClearFlags |= (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VISTEST_VERTEX);

	// Render triangles to test
	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, m_testTris, 
							   sizeof(VISTEST_VERTEX));

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

	// Invalid query the test results
#if !defined(DEBUG) && !defined(_DEBUG)
	hr = GetVisibilityTestResult(3, uResult);
	if(SUCCEEDED(hr))
	{
		Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVisibilityTestResult failed by not failing"));
		return;
	}
#endif
}

//------------------------------------------------------------------------------
//	CVisibilityTest::RenderStage3
//------------------------------------------------------------------------------
void
CVisibilityTest::RenderStage3(void)
/*++

Routine Description:

	Handles all the rendering for stage 3 testing
	This test verifies that multiple poltgons drawn on top of each other
	generates the correct result (which will be more pixels than those
	normally visible). The counter should include all pixels drawn, not just
	those that show up on the screen.

Arguments:

	None

Return Value:

	None

--*/
{
    DWORD	dwClearFlags = D3DCLEAR_TARGET;
    HRESULT	hr;
	UINT	uResult;
	float	z;
	float	zInc;
	BOOL	failure = FALSE;
	UINT	expected = m_width * m_height * 5;

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) 
        dwClearFlags |= (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VISTEST_VERTEX);

	// Render obscuring triangles
	m_pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, m_testTris, 
							  sizeof(VISTEST_VERTEX));

	// Setup for the test?
	hr = m_pDevice->GetIDirect3DDevice8()->BeginVisibilityTest();
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::BeginVisibilityTest")))
	{
		m_pDevice->EndScene();
		return;
	}

	// Draw 5 layers
	zInc = 0.25;
	for(z=1.0f; z>=0.0f; z-=zInc)
	{
		m_testTris[0].z	= z;
		m_testTris[1].z	= z;
		m_testTris[2].z	= z;
		m_testTris[3].z	= z;
		m_testTris[4].z	= z;
		m_testTris[5].z	= z;

		// Render triangles to test
		m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, m_testTris, 
								   sizeof(VISTEST_VERTEX));
	}

	// Do the test?
	hr = m_pDevice->GetIDirect3DDevice8()->EndVisibilityTest(4);
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::EndVisibilityTest")))
	{
		m_pDevice->EndScene();
		return;
	}

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

	// Query the test results
	hr = GetVisibilityTestResult(4, uResult);
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::GetVisibilityTestResult")))
		failure = TRUE;

	// Is the count correct
	else if(uResult != expected)
	{
		WCHAR buffer[128];
		wsprintf(buffer, TEXT("vistest: RenderStage3: Incorrect pixel count: Result=%u  Expected=%u  Diff=%d"), 
				 uResult, expected, abs((long)uResult - (long)expected));
		Log(LOG_FAIL, buffer);
		__asm int 3;
	}
}

//------------------------------------------------------------------------------
//	CVisibilityTest::RenderStage4
//------------------------------------------------------------------------------
void
CVisibilityTest::RenderStage4(void)
/*++

Routine Description:

	Handles all the rendering for stage 4 testing
	Test for internal overflow by drawing more than UINT_MAX pixels.

Arguments:

	None

Return Value:

	None

--*/
{
    DWORD	dwClearFlags = D3DCLEAR_TARGET;
    HRESULT	hr;
	UINT	uResult;
	float	z;
	float	zInc;
	BOOL	failure = FALSE;

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) 
        dwClearFlags |= (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VISTEST_VERTEX);

	// Render obscuring triangles
	m_pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, m_testTris, 
							  sizeof(VISTEST_VERTEX));

	// Setup for the test?
	hr = m_pDevice->GetIDirect3DDevice8()->BeginVisibilityTest();
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::BeginVisibilityTest")))
	{
		m_pDevice->EndScene();
		return;
	}

	// Calculate how many layers to draw: width*height pixels per screen
	// need to draw more than UINT_MAX pixels
	zInc = (float)(1.0 / ((double)UINT_MAX / ((double)m_width * (double)m_height) + 1.0));
	for(z=1.0f; z>0.0f; z-=zInc)
	{
		m_testTris[0].z	= z;
		m_testTris[1].z	= z;
		m_testTris[2].z	= z;
		m_testTris[3].z	= z;
		m_testTris[4].z	= z;
		m_testTris[5].z	= z;

		// Render triangles to test
		m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, m_testTris, 
								   sizeof(VISTEST_VERTEX));
	}

	m_testTris[0].z	= 0.0f;
	m_testTris[1].z	= 0.0f;
	m_testTris[2].z	= 0.0f;
	m_testTris[3].z	= 0.0f;
	m_testTris[4].z	= 0.0f;
	m_testTris[5].z	= 0.0f;

	// Render 1 more triangle to ensure overflow won't wrap to valid count
	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, m_testTris, 
							   sizeof(VISTEST_VERTEX));

	// Do the test?
	hr = m_pDevice->GetIDirect3DDevice8()->EndVisibilityTest(5);
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::EndVisibilityTest")))
	{
		m_pDevice->EndScene();
		return;
	}

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

	// Query the test results
	hr = GetVisibilityTestResult(5, uResult);
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::GetVisibilityTestResult")))
		failure = TRUE;

	// Is the count correct
	else if(uResult != UINT_MAX)
	{
		WCHAR buffer[128];
		wsprintf(buffer, TEXT("vistest: RenderStage4: Incorrect pixel count: Result=%u  Expected=%u  Diff=%d"), 
				 uResult, UINT_MAX, abs(UINT_MAX - (long)uResult));
		Log(LOG_FAIL, buffer);
		__asm int 3;
	}
}

//------------------------------------------------------------------------------
//	CVisibilityTest::RenderStage5
//------------------------------------------------------------------------------
void
CVisibilityTest::RenderStage5(void)
/*++

Routine Description:

	Handles all the rendering for stage 5 testing
	Starts a test, but never finishes to test for possible leaks on exit.

Arguments:

	None

Return Value:

	None

--*/
{
    DWORD	dwClearFlags = D3DCLEAR_TARGET;
    HRESULT	hr;
	UINT	uResult;

    // Clear the rendering target
    if (m_pDisplay->IsDepthBuffered()) 
        dwClearFlags |= (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);

    // Begin the scene
    m_pDevice->BeginScene();

    // Position the view using the default camera
    m_pDisplay->SetView();

    // Use a fixed function shader
    m_pDevice->SetVertexShader(FVF_VISTEST_VERTEX);

	// Render obscuring triangles
	m_pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 1, m_testTris, 
							  sizeof(VISTEST_VERTEX));

	// Setup for the test?
	hr = m_pDevice->GetIDirect3DDevice8()->BeginVisibilityTest();
	if(ResultFailed(hr, TEXT("IDirect3DDevice8::BeginVibilityTest")))
	{
		m_pDevice->EndScene();
		return;
	}
	
    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // End the scene
    m_pDevice->EndScene();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);
}

//------------------------------------------------------------------------------
//	CVisibilityTest::GetVisibilityTestResult
//------------------------------------------------------------------------------
HRESULT 
CVisibilityTest::GetVisibilityTestResult(
										 IN  DWORD	id, 
										 OUT UINT&	uResult
										 )
/*++

Routine Description:

	Blocks the device and ensures GetVisibilityTestResult returns a valid 
	answer (errors are still valid).

Arguments:

	IN id		- Test id
	OUT uResult	- Result of test

Return Value:

	None

--*/
{
	HRESULT	hr;

	// Intialize result
	uResult = 0;

	// Block to flush the push-buffer
	m_pDevice->GetIDirect3DDevice8()->BlockUntilIdle();

	// Just to make sure the test returns something valid (error or not), spin...
	do
	{
		hr = m_pDevice->GetIDirect3DDevice8()->GetVisibilityTestResult(id, &uResult, NULL);
	}
	while(hr == D3DERR_TESTINCOMPLETE);
	return hr;
}