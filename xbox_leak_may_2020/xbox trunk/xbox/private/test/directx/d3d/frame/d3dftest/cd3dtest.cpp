/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    cd3dtest.cpp

Author:

    Matt Bronder

Description:

    Framework library to facilitate the porting of Direct3D tests from the
    DirectX test team.

*******************************************************************************/

#include "d3dlocus.h"
#ifdef UNDER_XBOX
#include "profilep.h"
#endif // UNDER_XBOX
#include "cd3dtest.h"

//******************************************************************************
// CD3DTest
//******************************************************************************

RENDERPRIMITIVEAPI CD3DTest::m_pRPDefaultAPIList[6] =
{
    RP_DP,                      // DrawPrimitive
    RP_DIP,                     // DrawIndexedPrimitive
    RP_DPS,                     // DrawPrimitiveStrided
    RP_DIPS,                    // DrawIndexedPrimitiveStrided
    RP_DPVB,                    // DrawPrimitiveVB
    RP_DIPVB                    // DrawIndexedPrimitiveVB
};

RENDERPRIMITIVEAPI CD3DTest::m_pRPFullAPIList[16] =
{
    RP_BE,                      // Begin/Vertex/End
    RP_BIE,                     // BeginIndexed/Index/End
    RP_DP,                      // DrawPrimitive
    RP_DIP,                     // DrawIndexedPrimitive
    RP_DPS,                     // DrawPrimitiveStrided
    RP_DIPS,                    // DrawIndexedPrimitiveStrided
    RP_DPVB,                    // DrawPrimitiveVB
    RP_DIPVB,                   // DrawIndexedPrimitiveVB
    RP_DPVB_PV,                 // DrawPrimitiveVB after ProcessVertices
    RP_DIPVB_PV,                // DrawIndexedPrimitiveVB after ProcessVertices
    RP_DPVB_OPT,                // DrawPrimitiveVB with optimized vertex buffer
    RP_DIPVB_OPT,               // DrawIndexedPrimitiveVB with optimized vertex buffer
    RP_DPVB_OPT_PV,             // DrawPrimitiveVB after ProcessVertices with optimized vertex buffer
    RP_DIPVB_OPT_PV,            // DrawIndexedPrimitiveVB after ProcessVertices with optimized vertex buffer
    RP_DPVB_PVS,                // DrawPrimitiveVB after ProcessVerticesStrided
    RP_DIPVB_PVS                // DrawIndexedPrimitiveVB after ProcessVerticesStrided
};

RENDERPRIMITIVEAPI CD3DTest::m_pRPFullDX5APIList[4] =
{
    RP_BE,                      // Begin/Vertex/End
    RP_BIE,                     // BeginIndexed/Index/End
    RP_DP,                      // DrawPrimitive
    RP_DIP,                     // DrawIndexedPrimitive
};

RENDERPRIMITIVEAPI CD3DTest::m_pRPFullDX6APIList[14] =
{
    RP_BE,                      // Begin/Vertex/End
    RP_BIE,                     // BeginIndexed/Index/End
    RP_DP,                      // DrawPrimitive
    RP_DIP,                     // DrawIndexedPrimitive
    RP_DPS,                     // DrawPrimitiveStrided
    RP_DIPS,                    // DrawIndexedPrimitiveStrided
    RP_DPVB,                    // DrawPrimitiveVB
    RP_DIPVB,                   // DrawIndexedPrimitiveVB
    RP_DPVB_PV,                 // DrawPrimitiveVB after ProcessVertices
    RP_DIPVB_PV,                // DrawIndexedPrimitiveVB after ProcessVertices
    RP_DPVB_OPT,                // DrawPrimitiveVB with optimized vertex buffer
    RP_DIPVB_OPT,               // DrawIndexedPrimitiveVB with optimized vertex buffer
    RP_DPVB_OPT_PV,             // DrawPrimitiveVB after ProcessVertices with optimized vertex buffer
    RP_DIPVB_OPT_PV,            // DrawIndexedPrimitiveVB after ProcessVertices with optimized vertex buffer
};

RENDERPRIMITIVEAPI CD3DTest::m_pRPFullDX7APIList[14] =
{
    RP_DP,                      // DrawPrimitive
    RP_DIP,                     // DrawIndexedPrimitive
    RP_DPS,                     // DrawPrimitiveStrided
    RP_DIPS,                    // DrawIndexedPrimitiveStrided
    RP_DPVB,                    // DrawPrimitiveVB
    RP_DIPVB,                   // DrawIndexedPrimitiveVB
    RP_DPVB_PV,                 // DrawPrimitiveVB after ProcessVertices
    RP_DIPVB_PV,                // DrawIndexedPrimitiveVB after ProcessVertices
    RP_DPVB_OPT,                // DrawPrimitiveVB with optimized vertex buffer
    RP_DIPVB_OPT,               // DrawIndexedPrimitiveVB with optimized vertex buffer
    RP_DPVB_OPT_PV,             // DrawPrimitiveVB after ProcessVertices with optimized vertex buffer
    RP_DIPVB_OPT_PV,            // DrawIndexedPrimitiveVB after ProcessVertices with optimized vertex buffer
    RP_DPVB_PVS,                // DrawPrimitiveVB after ProcessVerticesStrided
    RP_DIPVB_PVS                // DrawIndexedPrimitiveVB after ProcessVerticesStrided
};

RENDERPRIMITIVEAPI CD3DTest::m_pRPFullDX8APIList[6] =
{
    RP_DP,                      // DrawPrimitiveUP
    RP_DIP,                     // DrawIndexedPrimitiveUP
    RP_DPVB,                    // DrawPrimitive (FVF shader)
    RP_DIPVB,                   // DrawIndexedPrimitive (FVF shader)
    RP_DPS,                     // DrawPrimitive (multistream shader)
    RP_DIPS,                    // DrawIndexedPrimitive (multistream shader)
};

DWORD CD3DTest::m_dwRPDefaultAPIListSize = 6;
DWORD CD3DTest::m_dwRPFullAPIListSize = 16;
DWORD CD3DTest::m_dwRPFullDX5APIListSize = 4;
DWORD CD3DTest::m_dwRPFullDX6APIListSize = 14;
DWORD CD3DTest::m_dwRPFullDX7APIListSize = 14;
DWORD CD3DTest::m_dwRPFullDX8APIListSize = 6;

//******************************************************************************
//
// Method:
//
//     CD3DTest
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
CD3DTest::CD3DTest() {

    D3DFORMAT fmt[] = {
        D3DFMT_A8R8G8B8,
#ifndef DEMO_HACK
        D3DFMT_X8R8G8B8,
#endif
        D3DFMT_R5G6B5,
#ifndef DEMO_HACK
        D3DFMT_A1R5G5B5,
        D3DFMT_X1R5G5B5,
        D3DFMT_A4R4G4B4,
        D3DFMT_DXT1,
        D3DFMT_DXT2,
        D3DFMT_DXT3,
        D3DFMT_DXT4,
        D3DFMT_DXT5,
#ifdef UNDER_XBOX
        D3DFMT_LIN_A8R8G8B8,
        D3DFMT_LIN_X8R8G8B8,
        D3DFMT_LIN_R5G6B5,
        D3DFMT_LIN_A1R5G5B5,
        D3DFMT_LIN_X1R5G5B5,
        D3DFMT_LIN_A4R4G4B4,
#endif // UNDER_XBOX
#endif
    };

    D3DFORMAT fmtd[] = {
#ifndef UNDER_XBOX
        D3DFMT_D16,
        D3DFMT_D16,
        D3DFMT_D24S8,
        D3DFMT_D24S8
#else
        D3DFMT_LIN_D16,
        D3DFMT_LIN_D24S8,
        D3DFMT_LIN_F16,
        D3DFMT_LIN_F24S8
#endif // UNDER_XBOX
    };

    D3DFORMAT fmtv[] = {
        D3DFMT_A8R8G8B8,
#ifndef DEMO_HACK
        D3DFMT_X8R8G8B8,
#endif
        D3DFMT_R5G6B5,
#ifndef DEMO_HACK
        D3DFMT_A1R5G5B5,
        D3DFMT_X1R5G5B5,
        D3DFMT_A4R4G4B4,
        D3DFMT_DXT1,
        D3DFMT_DXT2,
        D3DFMT_DXT3,
        D3DFMT_DXT4,
        D3DFMT_DXT5,
#endif
    };

    D3DFORMAT fmtr[] = {
#ifndef UNDER_XBOX
        D3DFMT_R5G6B5,
        D3DFMT_X1R5G5B5,
        D3DFMT_A8R8G8B8,
        D3DFMT_X8R8G8B8,
#else
        D3DFMT_LIN_R5G6B5,
        D3DFMT_LIN_X1R5G5B5,
        D3DFMT_LIN_A8R8G8B8,
        D3DFMT_LIN_X8R8G8B8,
#endif // UNDER_XBOX
    };

    m_ptrSkipList = NULL;
    m_fInTestCase = false;
    m_dwNumFailed = 0;

    m_dwVersion = 0x0800;

    m_hrLastError = D3D_OK;

    m_fIncrementForward = true;

    m_dwRPAPICyclingFrequency = 3;
    m_dwRPCurrentAPI = RP_DIP;
    m_pRPAPIList = m_pRPFullDX8APIList;
    m_dwRPAPIListSize = m_dwRPFullDX8APIListSize;
    m_dwRPAPICyclingIndex = 0;
    m_pRPVertexArray = NULL;
    m_dwRPVertexArraySize = 0;
    m_pRPIndexArray = NULL;
    m_dwRPIndexArraySize = 0;

    m_uCommonTextureFormats = countof(fmt);
    memcpy(m_fmtCommon, fmt, m_uCommonTextureFormats * sizeof(D3DFORMAT));

    m_uCommonZBufferFormats = countof(fmtd);
    memcpy(m_fmtdCommon, fmtd, m_uCommonZBufferFormats * sizeof(D3DFORMAT));

    m_uCommonVolumeFormats = countof(fmtv);
    memcpy(m_fmtvCommon, fmtv, m_uCommonVolumeFormats * sizeof(D3DFORMAT));

    m_uCommonTargetFormats = countof(fmtr);
    memcpy(m_fmtrCommon, fmtr, m_uCommonTargetFormats * sizeof(D3DFORMAT));
}

//******************************************************************************
//
// Method:
//
//     ~CD3DTest
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
CD3DTest::~CD3DTest() {

    PTRANGE ptr;

    for (ptr = m_ptrSkipList; m_ptrSkipList; ptr = m_ptrSkipList) {
        m_ptrSkipList = m_ptrSkipList->ptrNext;
        MemFree(ptr);
    }

    if (NULL != m_pRPVertexArray)
        MemFree(m_pRPVertexArray);
    if (NULL != m_pRPIndexArray)
        MemFree(m_pRPIndexArray);
}

/*
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
BOOL CD3DTest::Create(CDisplay* pDisplay) {

    return CScene::Create(pDisplay);
}
*/
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
int CD3DTest::Exhibit(int *pnExitCode) {

    BOOL            bMsgReady;
    float           fTime, fLastTime, fPrevTime, fTimeFreq;
    LARGE_INTEGER   qwCounter;
    DWORD           dwNumFrames = 0;
    UINT            uTest, uLastTest;
    PTRANGE         ptr;
    bool            bResult;
#ifndef UNDER_XBOX
    MSG             msg;
#endif // !UNDER_XBOX

    if (!m_bCreated) {
        return TRUE;
    }

    QueryPerformanceFrequency(&qwCounter);
    fTimeFreq = 1.0f / (float)qwCounter.QuadPart;
    QueryPerformanceCounter(&qwCounter);
    fLastTime = (float)qwCounter.QuadPart * fTimeFreq;
    fPrevTime = fLastTime;

    do {

#ifndef UNDER_XBOX
        // Pump messages
        if (m_pDisplay->IsActive()) {

            // When the application is active, check for new
            // messages without blocking to wait on them
            bMsgReady = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
        }
        else {

            // The application has lost the focus and is suspended
            // so we can afford to block here until a new message
            // has arrived (and conserve CPU usage in the process)
            bMsgReady = GetMessage(&msg, NULL, 0, 0);
        }

        if (bMsgReady) {

            // If a message is ready, process it and proceed to
            // check for another message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else if (msg.message != WM_QUIT) {
#endif // !UNDER_XBOX

            // Otherwise process user input, update the next frame,
            // and draw it
            QueryPerformanceCounter(&qwCounter);
            fTime = (float)qwCounter.QuadPart * fTimeFreq;

            // Process user input
            m_pDisplay->ProcessInput();
            if (!m_pDisplay->ConsoleVisible()) {
                ProcessInput();
            }

            if (m_pDisplay->m_bTimeSync) {
                m_pDisplay->m_bTimeSync = FALSE;
            }
            else if (!m_bPaused || m_bAdvance) {
                m_fTime += (fTime - fLastTime) * m_fTimeDilation;
                m_fFrame += 1.0f;//(1.0f * m_fTimeDilation);
                m_fTimeDelta = m_fTime - m_fLastTime;
                m_fFrameDelta = m_fFrame - m_fLastFrame;

                if (m_fTimeDuration != FLT_INFINITE && m_fTime > m_fTimeDuration) {
                    break;
                }

                uTest = (UINT)m_fFrame;
                uLastTest = uTest - 1;

                do {
                    // Check the skip list
                    for (ptr = m_ptrSkipList; ptr; ) {
                        if (uTest >= ptr->uStart && uTest <= ptr->uStop) {
                            uTest = ptr->uStop + 1;
                            ptr = m_ptrSkipList;
                        }
                        else {
                            ptr = ptr->ptrNext;
                        }
                    }

                    if (uTest == uLastTest) {
                        break;
                    }

                    m_fFrame = (float)uTest;

                    if (m_fFrameDuration != FLT_INFINITE && m_fFrame > m_fFrameDuration) {
                        break;
                    }

                    uLastTest = uTest;

                    bResult = ExecuteTest(uTest);

                } while (!bResult);

                if (m_fFrameDuration != FLT_INFINITE && m_fFrame > m_fFrameDuration) {
                    break;
                }

                m_fLastTime = m_fTime;
                m_fLastFrame = m_fFrame;
                m_bAdvance = FALSE;
            }

            fLastTime = fTime;
            m_uLastTestNum = uTest;

            if (bResult) {
                if (!Render()) {
                    break;
                }
            }

            EndTestCase();

            dwNumFrames++;

            if (fTime - fPrevTime > 1.0f) {
                m_fFPS = (float)dwNumFrames / (fTime - fPrevTime);
                fPrevTime = fTime;
                dwNumFrames = 0;
            }
#ifndef UNDER_XBOX
        }

    } while (msg.message != WM_QUIT);
#else
    } while (!m_bQuit);
#endif // UNDER_XBOX

    if (pnExitCode) {
#ifndef UNDER_XBOX
        *pnExitCode = msg.wParam;
#else
        *pnExitCode = 0;
#endif // UNDER_XBOX
    }

    return m_bDisplayOK;
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
BOOL CD3DTest::Initialize() {

    HRESULT hr;

    m_pD3D8 = m_pDisplay->GetDirect3D8();
    m_pSrcDevice8 = m_pDevice;

    hr = m_pDevice->GetDeviceCaps(&m_d3dcaps);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDeviceCaps"))) {
        return FALSE;
    }

    // Restrict the common depth buffer formats if running under verification
    if (m_pDisplay->GetConnectionStatus() == SERVER_CONNECTED) {
        m_uCommonZBufferFormats = 2;
    }

    // Initialize the viewports
    m_vpFull.X = 0;
    m_vpFull.Y = 0;
    m_vpFull.Width = m_pDisplay->GetWidth();
    m_vpFull.Height = m_pDisplay->GetHeight();
    m_vpFull.MinZ = 0.0f;
    m_vpFull.MaxZ = 1.0f;

    m_vpTest.Width = m_pDisplay->GetHeight();
    m_vpTest.Height = m_vpTest.Width;
    m_vpTest.X = m_pDisplay->GetWidth() / 2 - m_vpTest.Width / 2;
    m_vpTest.Y = 0;
    m_vpTest.MinZ = 0.0f;
    m_vpTest.MaxZ = 1.0f;

m_pDevice->SetViewport(&m_vpTest);

    // Set the initial device states
    SetDefaultMatrices();
    SetDefaultMaterials();
    SetDefaultLights();
    SetDefaultLightStates();
    SetDefaultRenderStates();

    LogFunction(m_szTestName);

    return (TestInitialize() == D3DTESTINIT_RUN);
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
void CD3DTest::Efface() {

    m_bDisplayOK = (BOOL)TestTerminate();
}
/*
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
//     float fTime                      - The time to use in the update
//
// Return Value:
//
//     None.
//
//******************************************************************************
void CD3DTest::Update(float fTime) {

    m_fFrame += (1.0f * m_fTimeDilation);
    m_fFrameDelta = m_fFrame - m_fLastFrame;


    m_fLastTime = fTime;
    m_fLastFrame = m_fFrame;
}
*/
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
BOOL CD3DTest::Render() {

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
    m_pDevice->SetViewport(&m_vpFull);
    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, RGB_MAKE(74, 150, 202), 1.0f, 0);

    m_pDevice->SetViewport(&m_vpTest);
    if (m_pDisplay->IsDepthBuffered()) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
    }
//    m_pDevice->Clear(0, NULL, dwClearFlags, RGB_MAKE(0, 0, 0), 1.0f, 0);
    ClearFrame();

    SwitchAPI((UINT)m_fFrame);

    SceneRefresh();

    // Begin the scene
    m_pDevice->BeginScene();

    // Display the adapter mode
    ShowDisplayMode();

    // Display the frame rate
    ShowFrameRate();

    // Display the console
    m_pDisplay->ShowConsole();

    // End the scene
    m_pDevice->EndScene();

    CompareImages();

    ProcessFrame();

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
void CD3DTest::ProcessInput() {

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
BOOL CD3DTest::InitView() {

    return CScene::InitView();
}

//******************************************************************************
bool CD3DTest::GetCompareResult(float fColorVariation, float fColorRange, 
                                UINT uMinPixels, UINT uMax)
{
    // Even though comparisons are automatically made between frame buffers
    // at Present time, those comparisons use state values already set by the
    // scene to determine sensitivity.  Since DirectX tests request sensitivity
    // each frame, make another comparison between the reference and xbox
    // images using the given parameters.

    return true;
}

//******************************************************************************
bool CD3DTest::SetTestRange(UINT uStart, UINT uStop) {

    DWORD dwMaxDuration = 0xFFFFFFFF;

    // Set the test duration
    if (GetStartupContext() & TSTART_SEQUENTIAL) {

        TCHAR szName[MAX_PATH];
        DWORD dwDuration;

        if (GetModuleName(GetTestInstance(), szName, MAX_PATH)) {
            dwDuration = GetProfileInt(szName, TEXT("FrameDuration"), 0xFFFFFFFE);
            if (dwDuration != 0xFFFFFFFE) {
                if (dwDuration != 0) {
                    dwMaxDuration = dwDuration;
                }
            }
        }
    }

    if (uStart > dwMaxDuration) {
        return false;
    }

    if (uStop > dwMaxDuration) {
        uStop = dwMaxDuration;
    }

    m_fFrame = (float)(uStart - 1);
    m_fFrameDuration = (float)uStop;

    return true;
}

//******************************************************************************
bool CD3DTest::SkipTests(UINT uCount) {

    PTRANGE ptr;

    ptr = (PTRANGE)MemAlloc(sizeof(TRANGE));
    if (!ptr) {
        return false;
    }

    ptr->uStart = (UINT)m_fFrame;
    ptr->uStop = ptr->uStart + uCount - 1;
    ptr->ptrNext = m_ptrSkipList;
    m_ptrSkipList = ptr;

    Log(LOG_SKIP, TEXT("Skipping tests %d to %d"), ptr->uStart, ptr->uStop);

    return true;
}

//******************************************************************************
bool CD3DTest::AddTestRange(UINT uStart, UINT uStop, bool bConstraint) {

    UINT i;
    bool bRet = true;

    for (i = uStart; i <= uStop; i++) {
        bRet = AddTest(i) && bRet;
    }
    return bRet;
}

//******************************************************************************
bool CD3DTest::AddTest(UINT uTest) {

    PTRANGE ptr;

    if ((float)uTest < m_fFrame) {
        return false;   // Don't add a test that occurs in the past
    }

    if ((float)uTest > m_fFrameDuration) {

        // Increase the duration, adding a skip list between the old duration and the new
        if (uTest != (UINT)m_fFrameDuration + 1) {

            ptr = (PTRANGE)MemAlloc(sizeof(TRANGE));
            if (!ptr) {
                return false;
            }

            ptr->uStart = (UINT)m_fFrameDuration + 1;
            ptr->uStop = uTest - 1;
            ptr->ptrNext = m_ptrSkipList;
            m_ptrSkipList = ptr;
        }

        m_fFrameDuration = (float)uTest;
    }
    else {

        PTRANGE ptr2;

        // See if the test falls within the range of a skip entry
        for (ptr = m_ptrSkipList; ptr; ) {
            if (uTest >= ptr->uStart && uTest <= ptr->uStop) {
                // Split the skip entry in two at the test.  First, remove the node from the list.
                if (ptr == m_ptrSkipList) {
                    m_ptrSkipList = ptr->ptrNext;
                }
                else {
                    for (ptr2 = m_ptrSkipList; ptr2->ptrNext != ptr; ptr2 = ptr2->ptrNext);
                    ptr2->ptrNext = ptr->ptrNext;
                }

                if (uTest != ptr->uStart) {
                    
                    ptr2 = (PTRANGE)MemAlloc(sizeof(TRANGE));
                    if (!ptr2) {
                        ptr->ptrNext = m_ptrSkipList;
                        m_ptrSkipList = ptr;
                        return false;
                    }

                    ptr2->uStart = ptr->uStart;
                    ptr2->uStop = uTest - 1;
                    ptr2->ptrNext = m_ptrSkipList;
                    m_ptrSkipList = ptr2;
                }

                if (uTest != ptr->uStop) {
                    
                    ptr2 = (PTRANGE)MemAlloc(sizeof(TRANGE));
                    if (!ptr2) {
                        ptr->ptrNext = m_ptrSkipList;
                        m_ptrSkipList = ptr;
                        return false;
                    }

                    ptr2->uStart = uTest + 1;
                    ptr2->uStop = ptr->uStop;
                    ptr2->ptrNext = m_ptrSkipList;
                    m_ptrSkipList = ptr2;
                }

                MemFree(ptr);
                break;
            }
            else {
                ptr = ptr->ptrNext;
            }
        }
    }

    return true;
}

//******************************************************************************
void CD3DTest::ClearRangeList() {

    PTRANGE ptr;

    // Release the test range list
    for (ptr = m_ptrSkipList; m_ptrSkipList; ptr = m_ptrSkipList) {
        m_ptrSkipList = m_ptrSkipList->ptrNext;
        MemFree(ptr);
    }

    m_fFrame = 0.0f;
    m_fFrameDuration = 1.0f;
}

//******************************************************************************
void CD3DTest::Pass(void) {

	Log(LOG_PASS, TEXT("CD3DTest::Pass"));
}

//******************************************************************************
void CD3DTest::Fail(void) {

    Log(LOG_FAIL, TEXT("CD3DTest::Fail"));
}

//******************************************************************************
void CD3DTest::Abort(void) {

    Log(LOG_ABORT, TEXT("CD3DTest::Abort"));
}

/*
//******************************************************************************
void CD3DTest::BeginTestCase(LPCSTR szTestCase, UINT uTestNumber) {

    m_fInTestCase=true;
    if (uTestNumber == 0) {
        uTestNumber = (UINT)m_fFrame;
    }
    LPTSTR szTCase = (LPTSTR)MemAlloc(2048 * sizeof(TCHAR));
    if (!szTCase) {
        OutputDebugString(TEXT("Insufficient memory for case buffer allocation\n"));
        __asm int 3;
        return;
    }
    wsprintf(szTCase + _tcslen(szTestCase), TEXT(" (Test %d)"), uTestNumber);
    LogBeginVariation(szTCase);
    MemFree(szTCase);
}
*/

//******************************************************************************
void CD3DTest::BeginTestCase(LPCSTR szTestCase, UINT uTestNumber) {

    if (m_fInTestCase) {
        EndTestCase();
    }
    m_fInTestCase=true;
    if (uTestNumber == 0) {
        uTestNumber = (UINT)m_fFrame;
    }
#ifndef UNICODE
    char szVariation[2048];
    sprintf(szVariation, "%s (Test %d)", szTestCase, uTestNumber);
    LogBeginVariation(szVariation);
#else
#ifndef UNDER_XBOX
    WCHAR wszTestCase[2048];
#else
    LPWSTR wszTestCase = (LPWSTR)MemAlloc(2048 * sizeof(WCHAR));
    if (!wszTestCase) {
        OutputDebugString(TEXT("Insufficient memory for case buffer allocation\n"));
//        __asm int 3;
        return;
    }
#endif // UNDER_XBOX
    mbstowcs(wszTestCase, szTestCase, 2048);
    wsprintf(wszTestCase + wcslen(wszTestCase), TEXT(" (Test %d)"), uTestNumber);
    LogBeginVariation(wszTestCase);
#ifdef UNDER_XBOX
    MemFree(wszTestCase);
#endif // UNDER_XBOX
#endif // UNICODE
}

//******************************************************************************
void CD3DTest::BeginTestCase(LPCWSTR szTestCase, UINT uTestNumber) {

    if (m_fInTestCase) {
        EndTestCase();
    }
    m_fInTestCase=true;
    if (uTestNumber == 0) {
        uTestNumber = (UINT)m_fFrame;
    }
#ifndef UNICODE
    char szVariation[2048];
    sprintf(szVariation, "%S (Test %d)", szTestCase, uTestNumber);
    LogBeginVariation(szVariation);
#else
    LPWSTR wszTestCase = (LPWSTR)MemAlloc(2048 * sizeof(WCHAR));
    if (!wszTestCase) {
        OutputDebugString(TEXT("Insufficient memory for case buffer allocation\n"));
//        __asm int 3;
        return;
    }
    wsprintf(wszTestCase, L"%s (Test %d)", szTestCase, uTestNumber);
    LogBeginVariation(wszTestCase);
    MemFree(wszTestCase);
#endif // UNICODE
}

//******************************************************************************
void CD3DTest::EndTestCase(void) {

    if (m_fInTestCase) {
        LogEndVariation();
    }

    m_fInTestCase = false;
}
/*
//******************************************************************************
void CD3DTest::WriteToLog(LPCSTR szFormat, ...) {

    LPTSTR szBuffer = (LPWSTR)MemAlloc(2048 * sizeof(TCHAR));
    if (!szBuffer) {
        OutputDebugString(TEXT("Insufficient memory for log buffer allocations\n"));
        __asm int 3;
        return;
    }
    va_list vl;
    va_start(vl, szFormat);
//    wvsprintf(wszBuffer, szFormat, vl);
    _vstprintf(szBuffer, szFormat, vl);
    va_end(vl);
    Log(LOG_COMMENT, szBuffer);
    MemFree(szBuffer);
}

//******************************************************************************
void CD3DTest::WriteToLog(LOGLEVEL ll, LPCSTR szFormat, ...) {

    LPTSTR szBuffer = (LPWSTR)MemAlloc(2048 * sizeof(TCHAR));
    if (!szBuffer) {
        OutputDebugString(TEXT("Insufficient memory for log buffer allocations\n"));
        __asm int 3;
        return;
    }
    va_list vl;
    va_start(vl, szFormat);
//    wvsprintf(wszBuffer, szFormat, vl);
    _vstprintf(szBuffer, szFormat, vl);
    va_end(vl);
    Log(ll, szBuffer);
    MemFree(szBuffer);
}
*/

//******************************************************************************
void CD3DTest::WriteToLog(LPCSTR szFormat, ...) {

#ifndef UNICODE
    char szBuffer[2048];
    va_list vl;
    va_start(vl, szFormat);
    wvsprintf(szBuffer, szFormat, vl);
    va_end(vl);
    Log(LOG_COMMENT, szBuffer);
#else
#ifndef UNDER_XBOX
    WCHAR wszFormat[2048], wszBuffer[2048];
#else
    LPWSTR wszBuffer, wszFormat = (LPWSTR)MemAlloc(4096 * sizeof(WCHAR));
    if (!wszFormat) {
        OutputDebugString(TEXT("Insufficient memory for log buffer allocations\n"));
//        __asm int 3;
        return;
    }
    wszBuffer = wszFormat + 2048;
#endif // UNDER_XBOX
    va_list vl;
    mbstowcs(wszFormat, szFormat, 2048);
    va_start(vl, wszFormat);
//    wvsprintf(wszBuffer, szFormat, vl);
    _vstprintf(wszBuffer, wszFormat, vl);
    va_end(vl);
    Log(LOG_COMMENT, wszBuffer);
#ifdef UNDER_XBOX
    MemFree(wszFormat);
#endif // UNDER_XBOX
#endif // UNICODE
}

//******************************************************************************
void CD3DTest::WriteToLog(LPCWSTR szFormat, ...) {

#ifndef UNICODE
    char szFmt[2048];
    char szBuffer[2048];
    va_list vl;
    wcstombs(szFmt, szFormat, 2048);
    va_start(vl, szFmt);
    wvsprintf(szBuffer, szFmt, vl);
    va_end(vl);
    Log(LOG_COMMENT, szBuffer);
#else
#ifndef UNDER_XBOX
    WCHAR wszBuffer[2048];
#else
    LPWSTR wszBuffer = (LPWSTR)MemAlloc(4096 * sizeof(WCHAR));
    if (!wszBuffer) {
        OutputDebugString(TEXT("Insufficient memory for log buffer allocations\n"));
//        __asm int 3;
        return;
    }
#endif // UNDER_XBOX
    va_list vl;
    va_start(vl, szFormat);
//    wvsprintf(wszBuffer, szFormat, vl);
    _vstprintf(wszBuffer, szFormat, vl);
    va_end(vl);
    Log(LOG_COMMENT, wszBuffer);
#ifdef UNDER_XBOX
    MemFree(wszBuffer);
#endif // UNDER_XBOX
#endif // UNICODE
}

//******************************************************************************
void CD3DTest::WriteToLog(LOGLEVEL ll, LPCSTR szFormat, ...) {

#ifndef UNICODE
    char szBuffer[2048];
    va_list vl;
    va_start(vl, szFormat);
    wvsprintf(szBuffer, szFormat, vl);
    va_end(vl);
    Log(ll, szBuffer);
#else
#ifndef UNDER_XBOX
    WCHAR wszFormat[2048], wszBuffer[2048];
#else
    LPWSTR wszBuffer, wszFormat = (LPWSTR)MemAlloc(4096 * sizeof(WCHAR));
    if (!wszFormat) {
        OutputDebugString(TEXT("Insufficient memory for log buffer allocations\n"));
//        __asm int 3;
        return;
    }
    wszBuffer = wszFormat + 2048;
#endif // UNDER_XBOX
    va_list vl;
    mbstowcs(wszFormat, szFormat, 2048);
    va_start(vl, wszFormat);
//    wvsprintf(wszBuffer, szFormat, vl);
    _vstprintf(wszBuffer, wszFormat, vl);
    va_end(vl);
    Log(ll, wszBuffer);
#ifdef UNDER_XBOX
    MemFree(wszFormat);
#endif // UNDER_XBOX
#endif // UNICODE
}

//******************************************************************************
void CD3DTest::WriteToLog(LOGLEVEL ll, LPCWSTR szFormat, ...) {

#ifndef UNICODE
    char szFmt[2048];
    char szBuffer[2048];
    va_list vl;
    wcstombs(szFmt, szFormat, 2048);
    va_start(vl, szFmt);
    wvsprintf(szBuffer, szFmt, vl);
    va_end(vl);
    Log(LOG_COMMENT, szBuffer);
#else
#ifndef UNDER_XBOX
    WCHAR wszBuffer[2048];
#else
    LPWSTR wszBuffer = (LPWSTR)MemAlloc(4096 * sizeof(WCHAR));
    if (!wszBuffer) {
        OutputDebugString(TEXT("Insufficient memory for log buffer allocations\n"));
//        __asm int 3;
        return;
    }
#endif // UNDER_XBOX
    va_list vl;
    va_start(vl, szFormat);
//    wvsprintf(wszBuffer, szFormat, vl);
    _vstprintf(wszBuffer, szFormat, vl);
    va_end(vl);
    Log(LOG_COMMENT, wszBuffer);
#ifdef UNDER_XBOX
    MemFree(wszBuffer);
#endif // UNDER_XBOX
#endif // UNICODE
}

//******************************************************************************
void CD3DTest::ReadInteger(LPSTR pKey, int iDefault, int * szValue, BOOL bRemove)
{
    *szValue = iDefault;
}

//******************************************************************************
void CD3DTest::ReadInteger(LPWSTR pKey, int iDefault, int * szValue, BOOL bRemove)
{
    *szValue = iDefault;
}

//******************************************************************************
bool CD3DTest::SetLastError(HRESULT hrResult)
{
    if (FAILED(hrResult))
    {
        m_hrLastError = hrResult;
    }

    if (FAILED(hrResult))
        return false;

    return true;
}

//******************************************************************************
HRESULT CD3DTest::GetLastError(void)
{
    return m_hrLastError;
}

//******************************************************************************
bool CD3DTest::BeginScene(void) {

    HRESULT hr = m_pDevice->BeginScene();
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::EndScene(void) {

    HRESULT hr = m_pDevice->EndScene();
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::Clear(D3DCOLOR c) {

    CSurface8*      pd3ds;
    DWORD           dwClearFlags = D3DCLEAR_TARGET;
    D3DSURFACE_DESC d3dsd;
    HRESULT         hr;

    hr = m_pDevice->GetDepthStencilSurface(&pd3ds);
    if (SUCCEEDED(hr) && pd3ds) {
        dwClearFlags |= D3DCLEAR_ZBUFFER;
        hr = pd3ds->GetDesc(&d3dsd);
#ifndef UNDER_XBOX
        if (SUCCEEDED(hr) && (d3dsd.Format == D3DFMT_D15S1 ||
                              d3dsd.Format == D3DFMT_D24S8 ||
                              d3dsd.Format == D3DFMT_D24X4S4)) 
#else
        if (SUCCEEDED(hr) && (d3dsd.Format == D3DFMT_D24S8 ||
                              d3dsd.Format == D3DFMT_F24S8 ||
                              d3dsd.Format == D3DFMT_LIN_D24S8 ||
                              d3dsd.Format == D3DFMT_LIN_F24S8)) 
#endif // UNDER_XBOX
        {
            dwClearFlags |= D3DCLEAR_STENCIL;
        }
        pd3ds->Release();
    }

    hr = m_pDevice->GetRenderTarget(&pd3ds);
    if (SUCCEEDED(hr) && pd3ds) {
        hr = pd3ds->GetDesc(&d3dsd);
        if (SUCCEEDED(hr)) {
            m_dwClearColor = ColorToPixel(d3dsd.Format, NULL, c);
        }
        pd3ds->Release();
    }

    hr = m_pDevice->Clear(0, NULL, dwClearFlags, c, 1.0f, 0);
    return SUCCEEDED(hr);
}

//******************************************************************************
bool CD3DTest::Clear(DWORD dwRGB, float fZ, DWORD dwStencil, DWORD dwFlags, DWORD dwCount, D3DRECT* lpRects) {

    CSurface8*      pd3ds;
    DWORD           dwClearFlags = dwFlags;
    D3DSURFACE_DESC d3dsd;
    HRESULT         hr;

    hr = m_pDevice->GetDepthStencilSurface(&pd3ds);
    if (SUCCEEDED(hr) && pd3ds) {
        hr = pd3ds->GetDesc(&d3dsd);
#ifndef UNDER_XBOX
        if (!(SUCCEEDED(hr) && (d3dsd.Format == D3DFMT_D15S1 ||
                              d3dsd.Format == D3DFMT_D24S8 ||
                              d3dsd.Format == D3DFMT_D24X4S4))) 
#else
        if (!(SUCCEEDED(hr) && (d3dsd.Format == D3DFMT_D24S8 ||
                              d3dsd.Format == D3DFMT_F24S8 ||
                              d3dsd.Format == D3DFMT_LIN_D24S8 ||
                              d3dsd.Format == D3DFMT_LIN_F24S8))) 
#endif // UNDER_XBOX
        {
            dwClearFlags &= ~(D3DCLEAR_STENCIL);
        }
        pd3ds->Release();
    }
    else {
        dwClearFlags &= ~(D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
    }

    hr = m_pDevice->GetRenderTarget(&pd3ds);
    if (SUCCEEDED(hr) && pd3ds) {
        hr = pd3ds->GetDesc(&d3dsd);
        if (SUCCEEDED(hr)) {
            m_dwClearColor = ColorToPixel(d3dsd.Format, NULL, dwRGB);
        }
        pd3ds->Release();
    }

    hr = m_pDevice->Clear(dwCount, lpRects, dwClearFlags, dwRGB, fZ, dwStencil);
    return SUCCEEDED(hr);
}

//******************************************************************************
bool CD3DTest::ClearFrame(void) {

    return Clear(RGB_MAKE(0,0,50));
}

//******************************************************************************
bool CD3DTest::SetTexture(int nIndex, CBaseTexture8* pTextures, DWORD dwFlags) {

    HRESULT hr = m_pDevice->SetTexture(nIndex, pTextures);
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::SetRenderState(D3DRENDERSTATETYPE Type, DWORD dwValue, DWORD dwFlags) {

    HRESULT hr;
    
    if (Type == D3DRENDERSTATE_TEXTUREMAPBLEND) {

        switch (dwValue) {

            case D3DTBLEND_ADD:

                m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
                m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE); 

                m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2); 
                m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

                break;
 
            case D3DTBLEND_COPY:
            case D3DTBLEND_DECAL:

                m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
 
                m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

                break;
                 
            case D3DTBLEND_DECALALPHA:

                m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
                m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

                m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2); 
                m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
 
                break;

            case D3DTBLEND_MODULATE:
            case D3DTBLEND_MODULATE_ALPHATEXTURE:

                m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE); 
 
                if ( dwValue == D3DTBLEND_MODULATE_ALPHATEXTURE )
                {
                    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1); 
                    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                }
                else
                { 
                    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2); 
                    m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
                } 

                break;

            case D3DTBLEND_MODULATEALPHA:

                m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE); 

                m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE); 

                break;
        }

        return SetLastError(D3D_OK);
    }
    else {
        hr = m_pDevice->SetRenderState(Type, dwValue);
        return SetLastError(hr);
    }
}

//******************************************************************************
bool CD3DTest::SetTransform(D3DTRANSFORMSTATETYPE Type, D3DMATRIX* Matrix, DWORD dwFlags) {

    HRESULT hr = m_pDevice->SetTransform(Type, Matrix);
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue, DWORD dwFlags) {

    HRESULT hr = m_pDevice->SetTextureStageState(dwStage, Type, dwValue);
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pdwSrcValue, DWORD *pdwRefValue, DWORD dwFlags) {

    DWORD dwValue;
    HRESULT hr = m_pDevice->GetTextureStageState(dwStage, Type, &dwValue);
    if (SUCCEEDED(hr)) {
        if (pdwSrcValue) {
            *pdwSrcValue = dwValue;
        }
        if (pdwRefValue) {
            *pdwRefValue = dwValue;
        }
    }
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::SetViewport(UINT nX, UINT nY, UINT nWidth, UINT nHeight, float fMinZ, float fMaxZ, DWORD dwFlags) {

    D3DVIEWPORT8    viewport;
    HRESULT         hr;

    // Set a viewport for the device
    viewport.X = nX;
    viewport.Y = nY;
    viewport.Width = nWidth;
    viewport.Height = nHeight;
    viewport.MinZ = fMinZ;
    viewport.MaxZ = fMaxZ;

    hr = m_pDevice->SetViewport(&viewport);
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::SetClipPlane(DWORD dwIndex, float* pPlane, DWORD dwFlags) {

    HRESULT hr;
    hr = m_pDevice->SetClipPlane(dwIndex, pPlane);
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::SetVertexShader(DWORD dwHandle, DWORD dwFlags)
{
    HRESULT hr;
    hr = m_pDevice->SetVertexShader(dwHandle);
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::SetStreamSource(DWORD dwStream, CVertexBuffer8 *pSrcVB, CVertexBuffer8 *pRefVB, DWORD dwStride, DWORD dwFlags)
{
    HRESULT hr;
    hr = m_pDevice->SetStreamSource(dwStream, pSrcVB, dwStride);
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::SetIndices(CIndexBuffer8 *pSrcIB, CIndexBuffer8 *pRefIB, DWORD dwBaseVertexIndex, DWORD dwFlags)
{
    HRESULT hr;
    hr = m_pDevice->SetIndices(pSrcIB, dwBaseVertexIndex);
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::LightEnable(DWORD dwLightNum, DWORD dwEnable, DWORD dwFlags) {

    HRESULT hr;
    hr = m_pDevice->LightEnable(dwLightNum, (BOOL)dwEnable);
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::CreateVertexShader(LPDWORD pdwDeclaration, LPDWORD pdwFunction, LPDWORD pdwHandle, DWORD dwUsage, DWORD dwFlags) {

    HRESULT hr;
    hr = m_pDevice->CreateVertexShader(pdwDeclaration, pdwFunction, pdwHandle, dwUsage);
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::DeleteVertexShader(DWORD dwHandle) {

    HRESULT hr;
    hr = m_pDevice->DeleteVertexShader(dwHandle);
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::SetVertexShaderConstant(DWORD dwRegister, LPVOID lpvConstantData, DWORD dwConstantCount, DWORD dwFlags) {

    HRESULT hr;
    hr = m_pDevice->SetVertexShaderConstant(dwRegister, lpvConstantData, dwConstantCount);
    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::ValidateDevice(HRESULT *phSrcResult, DWORD *pdwSrcPasses, HRESULT *phRefResult, DWORD *pdwRefPasses) {

    DWORD   dwPasses;
    HRESULT hr;

    hr = m_pDevice->ValidateDevice(&dwPasses);

    if (pdwSrcPasses) {
        *pdwSrcPasses = dwPasses;
    }
    if (pdwRefPasses) {
        *pdwRefPasses = dwPasses;
    }
    if (phSrcResult) {
        *phSrcResult = hr;
    }
    if (phRefResult) {
        *phRefResult = hr;
    }

    return SetLastError(hr);
}

//******************************************************************************
bool CD3DTest::SetupViewport() {

    CSurface8* pd3ds;
    D3DSURFACE_DESC d3dsd;
    m_pDevice->GetRenderTarget(&pd3ds);
    pd3ds->GetDesc(&d3dsd);
    pd3ds->Release();
    return SetViewport(0,0,d3dsd.Width,d3dsd.Height);
}

//******************************************************************************
void CD3DTest::SetAPI(RENDERPRIMITIVEAPI dwAPI)
{
    // Set new API and disable cycling
    m_dwRPCurrentAPI = dwAPI;
    m_pRPAPIList = NULL;
    m_dwRPAPIListSize = 0;
    m_dwRPAPICyclingFrequency = 0;
    m_dwRPAPICyclingIndex = 0;
}
    
//******************************************************************************
void CD3DTest::SetAPI(PRENDERPRIMITIVEAPI pAPIList, DWORD dwAPIListSize, DWORD dwAPICyclingFrequency)
{
    if (NULL == pAPIList || 0 == dwAPIListSize)
    {
        // Disbale API switching
        m_pRPAPIList = NULL;
        m_dwRPAPIListSize = 0;
        m_dwRPAPICyclingFrequency = 0;
        m_dwRPAPICyclingIndex = 0;
    }
    else
    {
        // Set new API list
        m_pRPAPIList = pAPIList;
        m_dwRPAPIListSize = dwAPIListSize;
        m_dwRPAPICyclingFrequency = dwAPICyclingFrequency;

        // Reset current API settings
        m_dwRPAPICyclingIndex = 0;
        m_dwRPCurrentAPI = m_pRPAPIList[m_dwRPAPICyclingIndex];
    }
}

//******************************************************************************
void CD3DTest::SwitchAPI(DWORD dwTestNumber)
{
    // Don't do anything if API switching is disabled
    if (0 == m_dwRPAPICyclingFrequency || 
        NULL == m_pRPAPIList ||
        0 == m_dwRPAPIListSize)
    {
        return;
    }

    // Check if we are ready to switch
    DWORD dwTest = dwTestNumber - 1; // zero-based test number
    m_dwRPAPICyclingIndex = (dwTest / m_dwRPAPICyclingFrequency) % m_dwRPAPIListSize;
    m_dwRPCurrentAPI = m_pRPAPIList[m_dwRPAPICyclingIndex];
}

//******************************************************************************
RENDERPRIMITIVEAPI CD3DTest::GetAPI(void)
{
    return m_dwRPCurrentAPI;
}

//******************************************************************************
// Remap invalid API combinations
// - APIs not supported in current version
// - Indexed to non-indexed for point primitives
// - APIs that require untransformed vertices
//******************************************************************************
DWORD CD3DTest::RemapAPI(DWORD dwCurrentAPI, D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD dwVertexTypeDesc)
{
    DWORD   dwAPI = dwCurrentAPI;

    // DX8: Remap old interfaces to DrawPrimitive and DrawIndexedPrimitive
    if (0x0800 == m_dwVersion)
    {
        switch (dwAPI)
        {
        case RP_BE:
        case RP_DPVB_PV:
        case RP_DPVB_OPT:
        case RP_DPVB_OPT_PV:
        case RP_DPVB_PVS:
            dwAPI = RP_DP;
            break;
        case RP_BIE:
        case RP_DIPVB_PV:
        case RP_DIPVB_OPT:
        case RP_DIPVB_OPT_PV:
        case RP_DIPVB_PVS:
            dwAPI = RP_DIP;
            break;
        }
    }

    // DX8: Remap multi-stream APIs to single-stream if devices do not support
    // enough streams
    DWORD   dwStreams = 0;
    if (0 != (dwVertexTypeDesc & D3DFVF_POSITION_MASK))
        dwStreams += 1;
//    if (0 != (dwVertexTypeDesc & (D3DFVF_NORMAL | D3DFVF_RESERVED1)))
    if (0 != (dwVertexTypeDesc & (D3DFVF_NORMAL)))
        dwStreams += 1;
    if (0 != (dwVertexTypeDesc & D3DFVF_DIFFUSE))
        dwStreams += 1;
    if (0 != (dwVertexTypeDesc & D3DFVF_SPECULAR))
        dwStreams += 1;
    dwStreams += (dwVertexTypeDesc & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT;

    if (m_d3dcaps.MaxStreams < dwStreams ||
        m_d3dcaps.MaxStreams < dwStreams)
    {
        switch(dwAPI)
        {
        case RP_DPS:
            dwAPI = RP_DPVB;
            break;
        case RP_DIPS:
            dwAPI = RP_DIPVB;
            break;
        }
    }

    // Remap indexed APIs to non-indexed analogs for POINTLIST primitive
    if (D3DPT_POINTLIST == d3dptPrimitiveType)
    {
        switch (dwAPI)
        {
        case RP_BIE:            dwAPI = RP_BE;              break;
        case RP_DIP:            dwAPI = RP_DP;              break;
        case RP_DIPS:           dwAPI = RP_DPS;             break;
        case RP_DIPVB:          dwAPI = RP_DPVB;            break;
        case RP_DIPVB_PV:       dwAPI = RP_DPVB_PV;         break;
        case RP_DIPVB_OPT:      dwAPI = RP_DPVB_OPT;        break;
        case RP_DIPVB_OPT_PV:   dwAPI = RP_DPVB_OPT_PV;     break;
        case RP_DIPVB_PVS:      dwAPI = RP_DPVB_PVS;        break;
        }
    }

    // Remap APIs that require untransfortmed vertices
    if (D3DFVF_XYZRHW == (dwVertexTypeDesc & D3DFVF_POSITION_MASK))
    {
        switch (dwAPI)
        {
        case RP_DPS:
        case RP_DPVB_PVS:
        case RP_DPVB_PV:
        case RP_DPVB_OPT:
        case RP_DPVB_OPT_PV:
            dwAPI = RP_DP;
            break;
        case RP_DIPS:
        case RP_DIPVB_PVS:
        case RP_DIPVB_PV:
        case RP_DIPVB_OPT:
        case RP_DIPVB_OPT_PV:
            dwAPI = RP_DIP;
            break;
        }
    }
    return dwAPI;
}

//******************************************************************************
bool CD3DTest::RenderPrimitive(D3DPRIMITIVETYPE d3dptPrimitiveType, DWORD dwVertexTypeDesc,
                               LPVOID lpvVertices, DWORD dwVertexCount,
                               LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags)
{
    DWORD       dwAPI = RemapAPI(m_dwRPCurrentAPI, d3dptPrimitiveType, dwVertexTypeDesc);
    DWORD       dwVertexSize = GetVertexSize(dwVertexTypeDesc);
    bool        bRet = true;

    LPVOID      pVertices;
    DWORD       dwVCount;
    LPWORD      pIndices;
    DWORD       dwICount;

    DWORD       dwClip;
    DWORD       dwLight;
//    DWORD       dwExtents;

    DWORD       dwPCount, dwNCount;
    HRESULT     hr;

    if ((dwVertexTypeDesc & D3DFVF_RESERVED0)) { // || (dwVertexTypeDesc & D3DFVF_RESERVED1)) {
        Log(LOG_ABORT, TEXT("Reserved field are unsupported in vertex formats in DX8"));
        return false;
    }

    // Filling local vertices and indices pointers/counters
    // with correct data for selected API
    switch(dwAPI)
    {
    case RP_DP:
    case RP_DPS:
    case RP_DPVB:
        dwVCount = dwVertexCount;
        pVertices = lpvVertices;

        // Non-indexed API selected: need to dereference vertices in case of indexed call
        if (NULL != lpwIndices)
        {
            dwVCount = dwIndexCount;
            pVertices = CreateVertexArray(lpvVertices, dwVertexCount, dwVertexSize, 
                                         lpwIndices, dwIndexCount);
            if (NULL == pVertices)
                bRet = false;
        }

        pIndices = NULL;
        dwICount = 0;
        dwNCount = dwVCount;

        break;
    case RP_DIP:
    case RP_DIPS:
    case RP_DIPVB:
        dwVCount = dwVertexCount;
        pVertices = lpvVertices;
        dwICount = dwIndexCount;
        pIndices = lpwIndices;
        
        // Indexed API selected: need to build index array in case of non-indexed call
        if (NULL == lpwIndices)
        {
            dwICount = dwVertexCount;
            pIndices = CreateIndexArray(dwICount);
            if (NULL == pIndices)
                bRet = false;
        }

        dwNCount = dwICount;

        break;
    default:
        // Incorrect API
        Log(LOG_ABORT, TEXT("RenderPrimitive: Render type %d is unsupported in DX8"), dwAPI);
        bRet = false;
        break;
    }
/*
    case RP_BE:
    case RP_BIE:
    case RP_DPVB_PV:
    case RP_DPVB_OPT_PV:
    case RP_DIPVB_PV:
    case RP_DIPVB_OPT_PV:
    case RP_DPVB_OPT:
    case RP_DPVB_PVS:
    case RP_DIPVB_OPT:
    case RP_DIPVB_PVS:
        Log(LOG_ABORT, TEXT("RenderPrimitive: Render type %d is unsupported in DX8"), dwAPI);
        return false;
*/    
    // Choose and call low level rendering function
    if (bRet)
    {
        switch (d3dptPrimitiveType) {
            case D3DPT_POINTLIST:
                dwPCount = dwNCount;
                break;
            case D3DPT_LINELIST:
                dwPCount = dwNCount / 2;
                break;
            case D3DPT_LINESTRIP:
                dwPCount = dwNCount - 1;
                break;
            case D3DPT_TRIANGLELIST:
                dwPCount = dwNCount / 3;
                break;
            case D3DPT_TRIANGLESTRIP:
            case D3DPT_TRIANGLEFAN:
                dwPCount = dwNCount - 2;
                break;
        }

#ifndef UNDER_XBOX
        if (dwFlags & D3DDP_DONOTCLIP) {
//            m_pDevice->GetRenderState(D3DRS_CLIPPING, &dwClip);
            dwClip = FALSE;
            hr = m_pDevice->SetRenderState(D3DRS_CLIPPING, FALSE);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_CLIPPING, ...)"))) {
                SetLastError(hr);
                return false;
            }
        }
#endif // !UNDER_XBOX
/*
        if (dwFlags & D3DDP_DONOTUPDATEEXTENTS) {
            m_pDevice->GetRenderState(D3DRS_EXTENTS, &dwExtents);
            hr = m_pDevice->SetRenderState(D3DRS_EXTENTS, FALSE);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_EXTENTS, ...)"))) {
                bRet = false;
            }
        }
*/
        if (dwFlags & D3DDP_DONOTLIGHT || !(dwVertexTypeDesc & D3DFVF_NORMAL)) {
//            m_pDevice->GetRenderState(D3DRS_LIGHTING, &dwLight);
            dwLight = FALSE;
            hr = m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_LIGHTING, ...)"))) {
                SetLastError(hr);
                return false;
            }
        }

        // TODO: Copy the vertices to a larger buffer and actually space them out in a strided
        // manner for the strided calls....modify dwVertexSize to be the strided amount and replace
        // pVertices with the strided buffer

        hr = m_pDevice->SetVertexShader(dwVertexTypeDesc);
        if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetVertexShader"))) {
            SetLastError(hr);
            return false;
        }

        switch (dwAPI)
        {

        case RP_DPS:
        case RP_DP:
            hr = m_pDevice->DrawPrimitiveUP(d3dptPrimitiveType, dwPCount, pVertices, dwVertexSize);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawPrimitiveUP"))) {
                SetLastError(hr);
                return false;
            }

            break;
        case RP_DIPS:
        case RP_DIP:
            hr = m_pDevice->DrawIndexedPrimitiveUP(d3dptPrimitiveType, 0, dwVCount, dwPCount, pIndices, D3DFMT_INDEX16, pVertices, dwVertexSize);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawPrimitiveUP"))) {
                SetLastError(hr);
                return false;
            }

            break;
        case RP_DPVB:
            {
                CVertexBuffer8  *pSrcVB;

                // Source vertex buffer
                pSrcVB = CreateVertexBuffer(m_pDevice, pVertices, dwVCount * dwVertexSize, 0, dwVertexTypeDesc);
                if (NULL == pSrcVB) {
                    SetLastError(hr);
                    return false;
                }

                // Rendering
                hr = m_pDevice->SetStreamSource(0, pSrcVB, dwVertexSize);
                if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetStreamSource"))) {
                    ReleaseVertexBuffer(pSrcVB);
                    SetLastError(hr);
                    return false;
                }
                else {
                    hr = m_pDevice->DrawPrimitive(d3dptPrimitiveType, 0, dwPCount);
                    if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawPrimitive"))) {
                        ReleaseVertexBuffer(pSrcVB);
                        SetLastError(hr);
                        return false;
                    }
                }

                ReleaseVertexBuffer(pSrcVB);
            }
            break;    
        case RP_DIPVB:
            {
                CVertexBuffer8  *pSrcVB;
                CIndexBuffer8   *pSrcIB;

                // Source vertex buffer
                pSrcVB = CreateVertexBuffer(m_pDevice, pVertices, dwVCount * dwVertexSize, 0, dwVertexTypeDesc);
                if (NULL == pSrcVB) {
                    SetLastError(hr);
                    return false;
                }

                pSrcIB = CreateIndexBuffer(m_pDevice, pIndices, dwICount * sizeof(WORD));
                if (NULL == pSrcIB) {
                    ReleaseVertexBuffer(pSrcVB);
                    SetLastError(hr);
                    return false;
                }

                // Rendering
                hr = m_pDevice->SetStreamSource(0, pSrcVB, dwVertexSize);
                if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetStreamSource"))) {
                    ReleaseIndexBuffer(pSrcIB);
                    ReleaseVertexBuffer(pSrcVB);
                    SetLastError(hr);
                    return false;
                }
                else {
                    hr = m_pDevice->SetIndices(pSrcIB, 0);
                    if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetIndices"))) {
                        ReleaseIndexBuffer(pSrcIB);
                        ReleaseVertexBuffer(pSrcVB);
                        SetLastError(hr);
                        return false;
                    }
                    else {
                        hr = m_pDevice->DrawIndexedPrimitive(d3dptPrimitiveType, 0, dwVCount, 0, dwPCount);
                        if (ResultFailed(hr, TEXT("IDirect3DDevice8::DrawPrimitive"))) {
                            ReleaseIndexBuffer(pSrcIB);
                            ReleaseVertexBuffer(pSrcVB);
                            SetLastError(hr);
                            return false;
                        }
                    }
                }

                ReleaseIndexBuffer(pSrcIB);
                ReleaseVertexBuffer(pSrcVB);
                break;
            }
            break;
        default:
            // Incorrect API
            bRet = false;
            break;
        }

#ifndef UNDER_XBOX
        if (dwFlags & D3DDP_DONOTCLIP) {
            hr = m_pDevice->SetRenderState(D3DRS_CLIPPING, dwClip);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_CLIPPING, ...)"))) {
                bRet = false;
            }
        }
#endif // !UNDER_XBOX
/*
        if (dwFlags & D3DDP_DONOTUPDATEEXTENTS) {
            hr = m_pDevice->SetRenderState(D3DRS_EXTENTS, dwExtents);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_EXTENTS, ...)"))) {
                bRet = false;
            }
        }
*/
        if (dwFlags & D3DDP_DONOTLIGHT || !(dwVertexTypeDesc & D3DFVF_NORMAL)) {
            hr = m_pDevice->SetRenderState(D3DRS_LIGHTING, dwLight);
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::SetRenderState(D3DRS_LIGHTING, ...)"))) {
                bRet = false;
            }
        }
    }

    SetLastError(hr);

    return bRet;
}

//******************************************************************************
DWORD CD3DTest::GetVertexSize(DWORD dwVertexTypeDesc) {

    static const DWORD  dwTextureSize[4] = {2*sizeof(float), 3*sizeof(float), 4*sizeof(float), sizeof(float)};
    DWORD               dwVertexSize = 0;

    if (0 != (dwVertexTypeDesc & D3DFVF_POSITION_MASK))
    {
        switch (dwVertexTypeDesc & D3DFVF_POSITION_MASK)
        {
            case D3DFVF_XYZ:
                dwVertexSize += sizeof(float) * 3;
                break;
            case D3DFVF_XYZRHW:
                dwVertexSize += sizeof(float) * 4;
                break;
            case D3DFVF_XYZB1:
                dwVertexSize += sizeof(float) * 4;
                break;
            case D3DFVF_XYZB2:
                dwVertexSize += sizeof(float) * 5;
                break;
            case D3DFVF_XYZB3:
                dwVertexSize += sizeof(float) * 6;
                break;
            case D3DFVF_XYZB4:
                dwVertexSize += sizeof(float) * 7;
                break;
#ifndef UNDER_XBOX
            case D3DFVF_XYZB5:
                dwVertexSize += sizeof(float) * 8;
                break;
#endif // !UNDER_XBOX
            default:
                break;
        }
    }

//    if (0 != (dwVertexTypeDesc & D3DFVF_RESERVED0))
//        dwVertexSize += sizeof(DWORD);

#ifndef UNDER_XBOX
    if (0 != (dwVertexTypeDesc & D3DFVF_PSIZE))
        dwVertexSize += sizeof(DWORD);
#endif

    if (0 != (dwVertexTypeDesc & D3DFVF_NORMAL))
        dwVertexSize += sizeof(float) * 3;

    if (0 != (dwVertexTypeDesc & D3DFVF_DIFFUSE))
        dwVertexSize += sizeof(DWORD);

    if (0 != (dwVertexTypeDesc & D3DFVF_SPECULAR))
        dwVertexSize += sizeof(DWORD);

    // Texture formats:
	// 00 - D3DFVF_TEXTUREFORMAT2
	// 01 - D3DFVF_TEXTUREFORMAT3
	// 10 - D3DFVF_TEXTUREFORMAT4
	// 11 - D3DFVF_TEXTUREFORMAT1
    {
		DWORD dwTextureFormats = dwVertexTypeDesc >> CDIRECT3D_TEXTUREFORMATS_SHIFT;
		DWORD dwTexCoordSize = 0;
		DWORD dwNumTexCoords = ((dwVertexTypeDesc & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);
		if (0 == dwTextureFormats)
		{
			dwTexCoordSize = sizeof(float) * 2 * dwNumTexCoords;
		}
		else
		{
			for (DWORD i=0; i < dwNumTexCoords; i++)
			{
				dwTexCoordSize += dwTextureSize[dwTextureFormats & CDIRECT3D_TEXTUREFORMAT_MASK];
				dwTextureFormats >>= CDIRECT3D_TEXTUREFORMAT_SIZE;
			}
		}
		dwVertexSize += dwTexCoordSize; 
	}
    
    return dwVertexSize;
}

//******************************************************************************
LPVOID CD3DTest::CreateVertexArray(LPVOID lpvVertices, DWORD  dwVertexCount, DWORD dwVertexSize,
                                     LPWORD lpwIndices, DWORD  dwIndexCount) {

    DWORD   dwRequiredMemory = dwIndexCount * dwVertexSize;
    LPBYTE  p;

    // Check size of currenty allocated memory and reallocate if necessary
    if (m_dwRPVertexArraySize < dwRequiredMemory)
    {
        if (NULL != m_pRPVertexArray)
            MemFree(m_pRPVertexArray);
        m_pRPVertexArray = MemAlloc(dwRequiredMemory);
        m_dwRPVertexArraySize = dwRequiredMemory;
        if (NULL == m_pRPVertexArray) 
        {
            m_dwRPVertexArraySize = 0;
            return NULL;
        }
    }

    // Dereference vertices to the allocated memory
    p = (LPBYTE)m_pRPVertexArray;
    for (DWORD i = 0; i < dwIndexCount; i++)
    {
        if (dwIndexCount < lpwIndices[i])
            return NULL;
        memcpy(p, (((LPBYTE)lpvVertices) + dwVertexSize*lpwIndices[i]), 
               dwVertexSize);
        p += dwVertexSize;
    }
    return m_pRPVertexArray;
}

//******************************************************************************
LPWORD CD3DTest::CreateIndexArray(DWORD dwSize)
{
    if (m_dwRPIndexArraySize < dwSize)
    {
        if (NULL != m_pRPIndexArray)
            MemFree(m_pRPIndexArray);
        m_pRPIndexArray = (LPWORD)MemAlloc(dwSize * sizeof(WORD));
        m_dwRPIndexArraySize = dwSize;
        if (NULL == m_pRPIndexArray)
        {
            m_dwRPIndexArraySize = 0;
            return NULL;
        }

        for (unsigned short i = 0; i < dwSize; i++)
            m_pRPIndexArray[i] = i;
    }
    return m_pRPIndexArray;
}

//******************************************************************************
CTexture8* CD3DTest::CreateCommonTexture(CDevice8* pDevice, UINT uWidth, UINT uHeight, UINT uFormat, LPCTSTR szName) {

    CTexture8* pd3dt;
    
    if (!szName) {
        pd3dt = (CTexture8*)CreateTexture(pDevice, uWidth, uHeight, m_fmtCommon[uFormat]);
    }
    else {
        pd3dt = (CTexture8*)CreateTexture(pDevice, szName, m_fmtCommon[uFormat]);
    }

    if (pd3dt) {
        RemoveTexture(pd3dt);
    }

    return pd3dt;
}

//******************************************************************************
bool CD3DTest::ColorFillTarget(DWORD dwRGB, LPRECT pRect) {

    D3DRECT d3drect;
    HRESULT hr;

    if (pRect) {
        d3drect.x1 = pRect->left;
        d3drect.y1 = pRect->top;
        d3drect.x2 = pRect->right;
        d3drect.y2 = pRect->bottom;
    }
    else {
        d3drect.x1 = 0;
        d3drect.y1 = 0;
        d3drect.x2 = m_pDisplay->GetWidth();
        d3drect.y2 = m_pDisplay->GetHeight();
    }

    hr = m_pDevice->Clear(1, &d3drect, D3DCLEAR_TARGET, dwRGB, 0.0f, 0);

    return SUCCEEDED(hr);
}

//******************************************************************************
TCHAR* CD3DTest::D3DFmtToString(D3DFORMAT d3dfmt)
{
	switch(d3dfmt)
	{
        case D3DFMT_UNKNOWN:
			return _T("D3DFMT_UNKNOWN"); 

        case D3DFMT_R8G8B8:
			return _T("D3DFMT_R8G8B8"); 

        case D3DFMT_A8R8G8B8:
			return _T("D3DFMT_A8R8G8B8");

        case D3DFMT_X8R8G8B8:
			return _T("D3DFMT_X8R8G8B8");

        case D3DFMT_R5G6B5:
			return _T("D3DFMT_R5G6B5");

        case D3DFMT_X1R5G5B5:
			return _T("D3DFMT_X1R5G5B5");

        case D3DFMT_A1R5G5B5:
			return _T("D3DFMT_A1R5G5B5");

        case D3DFMT_A4R4G4B4:
			return _T("D3DFMT_A4R4G4B4");

#ifdef UNDER_XBOX
        case D3DFMT_LIN_A8R8G8B8:
			return _T("D3DFMT_LIN_A8R8G8B8");

        case D3DFMT_LIN_X8R8G8B8:
			return _T("D3DFMT_LIN_X8R8G8B8");

        case D3DFMT_LIN_R5G6B5:
			return _T("D3DFMT_LIN_R5G6B5");

        case D3DFMT_LIN_X1R5G5B5:
			return _T("D3DFMT_LIN_X1R5G5B5");

        case D3DFMT_LIN_A1R5G5B5:
			return _T("D3DFMT_LIN_A1R5G5B5");

        case D3DFMT_LIN_A4R4G4B4:
			return _T("D3DFMT_LIN_A4R4G4B4");
#endif

        case D3DFMT_R3G3B2:
			return _T("D3DFMT_R3G3B2");

        case D3DFMT_A8:
			return _T("D3DFMT_A8");

        case D3DFMT_A8R3G3B2:
			return _T("D3DFMT_A8R3G3B2");

        case D3DFMT_X4R4G4B4:
			return _T("D3DFMT_X4R4G4B4");

        case D3DFMT_A8P8:
			return _T("D3DFMT_A8P8");

        case D3DFMT_P8:
            return _T("D3DFMT_P8");

        case D3DFMT_L8:
			return _T("D3DFMT_L8");

		case D3DFMT_A8L8:
			return _T("D3DFMT_A8L8");

		case D3DFMT_A4L4:
			return _T("D3DFMT_A4L4");

		case D3DFMT_V8U8:
			return _T("D3DFMT_V8U8");

		case D3DFMT_L6V5U5:
			return _T("D3DFMT_L6V5U5");

#ifndef UNDER_XBOX
		case D3DFMT_X8L8V8U8:
			return _T("D3DFMT_X8L8V8U8");
#endif // !UNDER_XBOX

		case D3DFMT_Q8W8V8U8:
			return _T("D3DFMT_Q8W8V8U8");

		case D3DFMT_V16U16:
			return _T("D3DFMT_V16U16");

		case D3DFMT_W11V11U10:
			return _T("D3DFMT_W11V11U10");
	
		case D3DFMT_UYVY:
			return _T("D3DFMT_UYVY");
		
		case D3DFMT_YUY2:
			return _T("D3DFMT_YUY2");
		
		case D3DFMT_DXT1:
			return _T("D3DFMT_DXT1");

		case D3DFMT_DXT2:
			return _T("D3DFMT_DXT2");

#ifndef UNDER_XBOX
		case D3DFMT_DXT3:
			return _T("D3DFMT_DXT3");
#endif // !UNDER_XBOX

		case D3DFMT_DXT4:
			return _T("D3DFMT_DXT4");

#ifndef UNDER_XBOX
		case D3DFMT_DXT5:
			return _T("D3DFMT_DXT5");
#endif // !UNDER_XBOX
            
#ifndef UNDER_XBOX
		case D3DFMT_D16_LOCKABLE:
			return _T("D3DFMT_D16_LOCKABLE");
#endif // !UNDER_XBOX

		case D3DFMT_D32:
			return _T("D3DFMT_D32");

		case D3DFMT_D15S1:
			return _T("D3DFMT_D15S1");

		case D3DFMT_D24S8:
			return _T("D3DFMT_D24S8");

		case D3DFMT_D16:
			return _T("D3DFMT_D16");

#ifndef UNDER_XBOX
		case D3DFMT_D24X8:
			return _T("D3DFMT_D24X8");
#endif

		case D3DFMT_D24X4S4:
			return _T("D3DFMT_D24X4S4");

		case D3DFMT_VERTEXDATA:
			return _T("D3DFMT_VERTEXDATA");

		case D3DFMT_INDEX16:
			return _T("D3DFMT_INDEX16");

#ifndef UNDER_XBOX
		case D3DFMT_INDEX32:
			return _T("D3DFMT_INDEX32");
#endif

		default:
			return _T("<Unrecognized Format>");
	}
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
BOOL CD3DTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

//******************************************************************************
// Texture utilities
//******************************************************************************

//******************************************************************************
CTexture8* CreateGradientTexture(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, 
                            DWORD dwColors[4], D3DFORMAT fmt)
{
    CTexture8*      pd3dt;
    D3DLOCKED_RECT  d3dlr;
    LPDWORD         pdwPixel;
    UINT            x, y;
    HRESULT         hr;

	// Interpolants
	float fRed, fGreen, fBlue, fAlpha;
	float fRedPct[4], fGreenPct[4], fBluePct[4], fAlphaPct[4];

	for(int i=0; i<4; i++)
	{
		fRedPct[i] = CIL_MAKEPCT(CIL_RED(dwColors[i]));
		fGreenPct[i] = CIL_MAKEPCT(CIL_GREEN(dwColors[i]));
		fBluePct[i] = CIL_MAKEPCT(CIL_BLUE(dwColors[i]));
		fAlphaPct[i] = CIL_MAKEPCT(CIL_ALPHA(dwColors[i]));
	}

#ifndef UNDER_XBOX
    pd3dt = (CTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, D3DFMT_A8R8G8B8);
#else
    pd3dt = (CTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, D3DFMT_LIN_A8R8G8B8);
#endif // UNDER_XBOX
    if (!pd3dt) {
        return NULL;
    }

    hr = pd3dt->LockRect(0, &d3dlr, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"))) {
        ReleaseTexture(pd3dt);
        return NULL;
    }

    pdwPixel = (LPDWORD)d3dlr.pBits;

    for (x = 0; x < dwWidth; x++) {

        for (y = 0; y < dwHeight; y++) {

            float fXPct, fYPct;
            if(dwWidth > 1)
                fXPct = (float)x/(float)(dwWidth - 1);
            else
                fXPct = 0;
            if(dwHeight > 1)
                fYPct = (float)y/(float)(dwHeight - 1);
            else
                fYPct = 0;
			
			fRed = ((1.0f - fYPct) * (fXPct * fRedPct[1]   + (1.0f - fXPct) * fRedPct[0])) +
				   (fYPct          * (fXPct * fRedPct[3]   + (1.0f - fXPct) * fRedPct[2]));
			fGreen = ((1.0f - fYPct) * (fXPct * fGreenPct[1] + (1.0f - fXPct) * fGreenPct[0])) +
				   (fYPct            * (fXPct * fGreenPct[3] + (1.0f - fXPct) * fGreenPct[2]));				   
			fBlue = ((1.0f - fYPct) * (fXPct * fBluePct[1]  + (1.0f - fXPct) * fBluePct[0])) +
				   (fYPct           * (fXPct * fBluePct[3]  + (1.0f - fXPct) * fBluePct[2]));				   
			fAlpha = ((1.0f - fYPct)* (fXPct * fAlphaPct[1] + (1.0f - fXPct) * fAlphaPct[0])) +
				   (fYPct           * (fXPct * fAlphaPct[3] + (1.0f - fXPct) * fAlphaPct[2]));				   

			// Plug the color into our data
            pdwPixel[x + y * (d3dlr.Pitch / 4)] = RGBA_MAKE(CIL_MAKEDWORD(fRed),CIL_MAKEDWORD(fGreen),CIL_MAKEDWORD(fBlue),CIL_MAKEDWORD(fAlpha));
        }
    }

    hr = pd3dt->UnlockRect(0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::UnlockRect"))) {
        ReleaseTexture(pd3dt);
        return NULL;
    }

#ifndef UNDER_XBOX
    if (fmt != D3DFMT_A8R8G8B8) {
#else
    if (fmt != D3DFMT_LIN_A8R8G8B8) {
#endif // UNDER_XBOX

        CTexture8* pd3dt2;
        pd3dt2 = (CTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, fmt);
        if (!pd3dt2) {
            ReleaseTexture(pd3dt);
            return NULL;
        }

        hr = CopyTexture(pd3dt2, NULL, pd3dt, NULL, D3DX_FILTER_NONE);

        ReleaseTexture(pd3dt);

        if (FAILED(hr)) {
            ReleaseTexture(pd3dt2);
            return NULL;
        }

        pd3dt = pd3dt2;
    }

    return pd3dt;
}

//******************************************************************************
CVolumeTexture8* CreateGradientVolumeTexture(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, 
                            DWORD dwDepth, DWORD* dwColors, DWORD dwColorStride, D3DFORMAT fmt)
{
    CVolumeTexture8* pd3dt;
    D3DLOCKED_BOX    d3dlb;
    LPDWORD          pdwPixel;
    UINT             x, y, z;
    HRESULT          hr;
#ifdef UNDER_XBOX
    Swizzler         swz(dwWidth, dwHeight, dwDepth);
#endif

	// Interpolants
	float fRed, fGreen, fBlue, fAlpha;
	float fRedPct[4], fGreenPct[4], fBluePct[4], fAlphaPct[4];

    pd3dt = (CVolumeTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, dwDepth, D3DFMT_A8R8G8B8, TTYPE_VOLUME);
    if (!pd3dt) {
        return NULL;
    }

    hr = pd3dt->LockBox(0, &d3dlb, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::LockRect"))) {
        ReleaseTexture(pd3dt);
        return NULL;
    }

    pdwPixel = (LPDWORD)d3dlb.pBits;

    for (x = 0; x < dwWidth; x++) {

        for (y = 0; y < dwHeight; y++) {

            for (z = 0; z < dwDepth; z++) {

                float fXPct, fYPct;

	            for(int i=0; i<4; i++)
	            {
		            fRedPct[i] = CIL_MAKEPCT(CIL_RED(dwColors[i + z * dwColorStride]));
		            fGreenPct[i] = CIL_MAKEPCT(CIL_GREEN(dwColors[i + z * dwColorStride]));
		            fBluePct[i] = CIL_MAKEPCT(CIL_BLUE(dwColors[i + z * dwColorStride]));
		            fAlphaPct[i] = CIL_MAKEPCT(CIL_ALPHA(dwColors[i + z * dwColorStride]));
	            }

                if(dwWidth > 1)
                    fXPct = (float)x/(float)(dwWidth - 1);
                else
                    fXPct = 0;
                if(dwHeight > 1)
                    fYPct = (float)y/(float)(dwHeight - 1);
                else
                    fYPct = 0;
			    
			    fRed = ((1.0f - fYPct) * (fXPct * fRedPct[1]   + (1.0f - fXPct) * fRedPct[0])) +
				       (fYPct          * (fXPct * fRedPct[3]   + (1.0f - fXPct) * fRedPct[2]));
			    fGreen = ((1.0f - fYPct) * (fXPct * fGreenPct[1] + (1.0f - fXPct) * fGreenPct[0])) +
				       (fYPct            * (fXPct * fGreenPct[3] + (1.0f - fXPct) * fGreenPct[2]));				   
			    fBlue = ((1.0f - fYPct) * (fXPct * fBluePct[1]  + (1.0f - fXPct) * fBluePct[0])) +
				       (fYPct           * (fXPct * fBluePct[3]  + (1.0f - fXPct) * fBluePct[2]));				   
			    fAlpha = ((1.0f - fYPct)* (fXPct * fAlphaPct[1] + (1.0f - fXPct) * fAlphaPct[0])) +
				       (fYPct           * (fXPct * fAlphaPct[3] + (1.0f - fXPct) * fAlphaPct[2]));				   

			    // Plug the color into our data
#ifndef UNDER_XBOX
                pdwPixel[x + y * (d3dlb.RowPitch >> 2) + z * (d3dlb.SlicePitch >> 2)] = RGBA_MAKE(CIL_MAKEDWORD(fRed),CIL_MAKEDWORD(fGreen),CIL_MAKEDWORD(fBlue),CIL_MAKEDWORD(fAlpha));
#else
                pdwPixel[swz.SwizzleU(x) | swz.SwizzleV(y) | swz.SwizzleW(z)] = RGBA_MAKE(CIL_MAKEDWORD(fRed),CIL_MAKEDWORD(fGreen),CIL_MAKEDWORD(fBlue),CIL_MAKEDWORD(fAlpha));
#endif // UNDER_XBOX
            }
        }
    }

    hr = pd3dt->UnlockBox(0);
    if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::UnlockRect"))) {
        ReleaseTexture(pd3dt);
        return NULL;
    }

    if (fmt != D3DFMT_A8R8G8B8) {

        CVolumeTexture8* pd3dt2;
        pd3dt2 = (CVolumeTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, dwDepth, fmt, TTYPE_VOLUME);
        if (!pd3dt2) {
            ReleaseTexture(pd3dt);
            return NULL;
        }

        hr = CopyTexture(pd3dt2, NULL, pd3dt, NULL, D3DX_FILTER_NONE);

        ReleaseTexture(pd3dt);

        if (FAILED(hr)) {
            ReleaseTexture(pd3dt2);
            return NULL;
        }

        pd3dt = pd3dt2;
    }

    return pd3dt;
}

//******************************************************************************
CTexture8* CreateStripedTexture(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, 
                            DWORD dwStripes, DWORD dwColorArray[], bool bHorizontal,
                            bool bDiagonal, D3DFORMAT fmt)
{
    CTexture8*      pd3dt;
    D3DLOCKED_RECT  d3dlr;
    LPDWORD         pdwPixel;
	DWORD           dwStripeWidth;
    UINT            x, y;
    HRESULT         hr;

    if (dwWidth == 0)
        dwWidth = 1;
    if (dwHeight == 0)
        dwHeight = 1;

	if(true == bHorizontal)
		dwStripeWidth = dwWidth / dwStripes;
	else
		dwStripeWidth = dwHeight / dwStripes;

	if(dwStripeWidth < 1)
		dwStripeWidth = 1;

#ifndef UNDER_XBOX
    pd3dt = (CTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, D3DFMT_A8R8G8B8);
#else
    pd3dt = (CTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, D3DFMT_LIN_A8R8G8B8);
#endif // UNDER_XBOX
    if (!pd3dt) {
        return NULL;
    }

    hr = pd3dt->LockRect(0, &d3dlr, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"))) {
        ReleaseTexture(pd3dt);
        return NULL;
    }

    pdwPixel = (LPDWORD)d3dlr.pBits;

    for (x = 0; x < dwWidth; x++) {

        for (y = 0; y < dwHeight; y++) {

			// Determine which color we will be using.
			DWORD dwStripe;

			if(true == bHorizontal)
                if(true == bDiagonal)
                    dwStripe = (x + y) / dwStripeWidth;
                else
                    dwStripe = x / dwStripeWidth;
			else
                if(true == bDiagonal)
                    dwStripe = (y + x) / dwStripeWidth;
                else
                    dwStripe = y / dwStripeWidth;

			// Plug the color into our data
            pdwPixel[x + y * (d3dlr.Pitch / 4)] = dwColorArray[dwStripe % dwStripes];
        }
    }

    hr = pd3dt->UnlockRect(0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::UnlockRect"))) {
        ReleaseTexture(pd3dt);
        return NULL;
    }

#ifndef UNDER_XBOX
    if (fmt != D3DFMT_A8R8G8B8) {
#else
    if (fmt != D3DFMT_LIN_A8R8G8B8) {
#endif // UNDER_XBOX

        CTexture8* pd3dt2;
        pd3dt2 = (CTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, fmt);
        if (!pd3dt2) {
            ReleaseTexture(pd3dt);
            return NULL;
        }

        hr = CopyTexture(pd3dt2, NULL, pd3dt, NULL, D3DX_FILTER_NONE);

        ReleaseTexture(pd3dt);

        if (FAILED(hr)) {
            ReleaseTexture(pd3dt2);
            return NULL;
        }

        pd3dt = pd3dt2;
    }

    return pd3dt;
}

//******************************************************************************
CVolumeTexture8* CreateStripedVolumeTexture(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, 
                            DWORD dwDepth, DWORD dwStripes, DWORD dwColorArray[], DWORD dwArrayStride, 
                            bool bHorizontal, bool bDiagonal, D3DFORMAT fmt)
{
    CVolumeTexture8* pd3dt;
    D3DLOCKED_BOX    d3dlb;
    LPDWORD          pdwPixel;
	DWORD            dwStripeWidth;
    UINT             x, y, z;
    HRESULT          hr;
#ifdef UNDER_XBOX
    Swizzler         swz(dwWidth, dwHeight, dwDepth);
#endif

    if (dwWidth == 0)
        dwWidth = 1;
    if (dwHeight == 0)
        dwHeight = 1;
    if (dwDepth == 0)
        dwDepth = 1;

	if(true == bHorizontal)
		dwStripeWidth = dwWidth / dwStripes;
	else
		dwStripeWidth = dwHeight / dwStripes;

	if(dwStripeWidth < 1)
		dwStripeWidth = 1;

    pd3dt = (CVolumeTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, dwDepth, D3DFMT_A8R8G8B8, TTYPE_VOLUME);
    if (!pd3dt) {
        return NULL;
    }

    hr = pd3dt->LockBox(0, &d3dlb, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::LockRect"))) {
        ReleaseTexture(pd3dt);
        return NULL;
    }

    pdwPixel = (LPDWORD)d3dlb.pBits;

    for (x = 0; x < dwWidth; x++) {

        for (y = 0; y < dwHeight; y++) {

			// Determine which color we will be using.
			DWORD dwStripe;

			if(true == bHorizontal)
                if(true == bDiagonal)
                    dwStripe = (x + y) / dwStripeWidth;
                else
                    dwStripe = x / dwStripeWidth;
			else
                if(true == bDiagonal)
                    dwStripe = (y + x) / dwStripeWidth;
                else
                    dwStripe = y / dwStripeWidth;

            for (z = 0; z < dwDepth; z++) {

			    // Plug the color into our data
#ifndef UNDER_XBOX
                pdwPixel[x + y * (d3dlb.RowPitch >> 2) + z * (d3dlb.SlicePitch >> 2)] = dwColorArray[(dwStripe % dwStripes) + z * dwArrayStride];
#else
                pdwPixel[swz.SwizzleU(x) | swz.SwizzleV(y) | swz.SwizzleW(z)] = dwColorArray[(dwStripe % dwStripes) + z * dwArrayStride];
#endif // UNDER_XBOX
            }
        }
    }

    hr = pd3dt->UnlockBox(0);
    if (ResultFailed(hr, TEXT("IDirect3DVolumeTexture8::UnlockRect"))) {
        ReleaseTexture(pd3dt);
        return NULL;
    }

    if (fmt != D3DFMT_A8R8G8B8) {

        CVolumeTexture8* pd3dt2;
        pd3dt2 = (CVolumeTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, dwDepth, fmt, TTYPE_VOLUME);
        if (!pd3dt2) {
            ReleaseTexture(pd3dt);
            return NULL;
        }

        hr = CopyTexture(pd3dt2, NULL, pd3dt, NULL, D3DX_FILTER_NONE);

        ReleaseTexture(pd3dt);

        if (FAILED(hr)) {
            ReleaseTexture(pd3dt2);
            return NULL;
        }

        pd3dt = pd3dt2;
    }

    return pd3dt;
}

//******************************************************************************
CTexture8* CreateChannelDataTexture(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, PCHANNELDATA pData, D3DFORMAT fmt)
{
    CTexture8*      pd3dt;
    D3DLOCKED_RECT  d3dlr;
    LPDWORD         pdwPixel;
    FLOAT           u, v;
    FLOAT           fDelta[2];
    FLOAT           fChannel[4];
    UINT            i, j;
    BYTE            a, r, g, b;
    BOOL            bBump;
    HRESULT         hr;
#ifdef UNDER_XBOX
    Swizzler        swz(dwWidth, dwHeight, 1);
#endif

    if (!pData)
    {
        return NULL;
    }

    bBump = (fmt == D3DFMT_V8U8 || fmt == D3DFMT_Q8W8V8U8 || fmt == D3DFMT_V16U16 || fmt == D3DFMT_W11V11U10 || fmt == D3DFMT_L6V5U5 || fmt == D3DFMT_X8L8V8U8);

    if (bBump) {
        pd3dt = (CTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, D3DFMT_X8L8V8U8);
    }
    else {
        pd3dt = (CTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, D3DFMT_A8R8G8B8);
    }
    if (!pd3dt) {
        return NULL;
    }

#ifdef UNDER_XBOX
    swz.SetU(0);
    swz.SetV(0);
#endif

    hr = pd3dt->LockRect(0, &d3dlr, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"))) {
        ReleaseTexture(pd3dt);
        return NULL;
    }

    pdwPixel = (LPDWORD)d3dlr.pBits;

    fDelta[0] = (FLOAT) 1.0 / (FLOAT) (dwWidth - 1);
    fDelta[1] = (FLOAT) 1.0 / (FLOAT) (dwHeight - 1); 

    for (v = 1.0f, j = 0; j < dwHeight; j++, v -= fDelta[1])
    {
        for (u = 0.0f, i = 0; i < dwWidth; i++, u += fDelta[0])
        {
            fChannel[ALPHA_CHANNEL] = (pData->fAlpha) ? pData->fAlpha(u, v) : 0.0f;       
            fChannel[  RED_CHANNEL] = (pData->fRed)   ? pData->fRed(u, v) : 0.0f;       
            fChannel[GREEN_CHANNEL] = (pData->fGreen) ? pData->fGreen(u, v) : 0.0f;       
            fChannel[ BLUE_CHANNEL] = (pData->fBlue)  ? pData->fBlue(u, v) : 0.0f;       

            if (pData->dwAlpha & CIL_UNSIGNED)
                a = (BYTE)(UINT8) ((fChannel[ALPHA_CHANNEL] + 1.0)*127.5);
            else
                a = (BYTE)(INT8) (fChannel[ALPHA_CHANNEL]*127.0);

            if (pData->dwRed & CIL_UNSIGNED)
                r = (BYTE)(UINT8) ((fChannel[RED_CHANNEL] + 1.0)*127.5);
            else
                r = (BYTE)(INT8) (fChannel[RED_CHANNEL]*127.0);

            if (pData->dwGreen & CIL_UNSIGNED)
                g = (BYTE)(UINT8) ((fChannel[GREEN_CHANNEL] + 1.0)*127.5);
            else
                g = (BYTE)(INT8) (fChannel[GREEN_CHANNEL]*127.0);

            if (pData->dwBlue & CIL_UNSIGNED)
                b = (BYTE)(UINT8) ((fChannel[BLUE_CHANNEL] + 1.0)*127.5);
            else
                b = (BYTE)(INT8) (fChannel[BLUE_CHANNEL]*127.0);

#ifndef UNDER_XBOX
            if (bBump) {
                pdwPixel[i] = b << 16 | g << 8 | r;
            }
            else {
                pdwPixel[i] = a << 24 | r << 16 | g << 8 | b;
            }
#else
            if (bBump) {
                pdwPixel[swz.Get2D()] = b << 16 | g << 8 | r;
            }
            else {
                pdwPixel[swz.Get2D()] = a << 24 | r << 16 | g << 8 | b;
            }
            swz.IncU();
#endif // UNDER_XBOX
        }
#ifndef UNDER_XBOX
        pdwPixel += d3dlr.Pitch / 4;
#else
        swz.IncV();
#endif // UNDER_XBOX
    }

    hr = pd3dt->UnlockRect(0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::UnlockRect"))) {
        ReleaseTexture(pd3dt);
        return NULL;
    }

    if ((bBump && fmt != D3DFMT_X8L8V8U8) || (!bBump && fmt != D3DFMT_A8R8G8B8)) {

        CTexture8* pd3dt2;
        pd3dt2 = (CTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, fmt);
        if (!pd3dt2) {
            ReleaseTexture(pd3dt);
            return NULL;
        }

        hr = CopyTexture(pd3dt2, NULL, pd3dt, NULL, D3DX_FILTER_NONE);

        ReleaseTexture(pd3dt);

        if (FAILED(hr)) {
            ReleaseTexture(pd3dt2);
            return NULL;
        }

        pd3dt = pd3dt2;
    }

    return pd3dt;
}

// ----------------------------------------------------------------------------

void SetVertex(D3DVERTEX *lpVrt,
               float x, float y, float z,
               float nx, float ny, float nz)
{
    // set homogeneous coordinates
    //    These describe the location of the vertex in "model" coordinates.
    lpVrt->x = D3DVAL(x);  
    lpVrt->y = D3DVAL(y);  
    lpVrt->z = D3DVAL(z);  

    // set normal coordinates
    //    These describe a vector determining which way the vertex is
    //    facing, used by the lighting module.
    lpVrt->nx = D3DVAL(nx);            
    lpVrt->ny = D3DVAL(ny);           
    lpVrt->nz = D3DVAL(nz);

    // set texture coordinates
    lpVrt->tu = D3DVAL(0.0);
    lpVrt->tv = D3DVAL(0.0);
}

// ----------------------------------------------------------------------------

void SetVertex(D3DTLVERTEX *lpVrt, float x, float y, float z)
{
    // set coordinates
    //    These describe the location of the vertex in screen coordinates.
    lpVrt->sx = D3DVAL(x);
    lpVrt->sy = D3DVAL(y);
    lpVrt->sz = D3DVAL(z);

    // protect against a divide be zero fault for rhw.
    if (z == 0.0f)
        z = 0.00001f;

    // set homogeneous W value
    lpVrt->rhw = D3DVAL(1.0/z);
//  lpVrt->rhw = D3DVAL(z);
}
