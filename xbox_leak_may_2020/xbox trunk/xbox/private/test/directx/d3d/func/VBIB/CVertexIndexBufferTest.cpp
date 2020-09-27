//
//  CVertexIndexBufferTest.cpp
//
//  Class implementation for CDevice8Test.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2000 Microsoft Corporation.
//

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "CVertexIndexBufferTest.h"

//
// Declare the framework
//
                   
//CD3DWindowFramework	App;

//
// Define the groups here
//

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

    CVertexIndexBufferTest* pVBIB;
    BOOL                    bQuit = FALSE, bRet = TRUE;
    UINT                    i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

#ifdef UNDER_XBOX
    if (!(GetStartupContext() & TSTART_STRESS)) {
        pDisplay->ReleaseDevice();
        pDisplay->GetDirect3D8()->SetPushBufferSize(786432, 32768);
        pDisplay->CreateDevice();
    }
#endif

    for (i = 0; i < 4 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pVBIB = new NormalBuffer();
                break;
            case 1:
                pVBIB = new LockActive();
                break;
            case 2:
                pVBIB = new MultipleLocks();
                break;
            case 3:
                pVBIB = new HugeBuffer();
                break;
        }

        if (!pVBIB) {
            return FALSE;
        }

        // Initialize the scene
        if (!pVBIB->Create(pDisplay)) {
            pVBIB->Release();
            return FALSE;
        }

        bRet = pVBIB->Exhibit(pnExitCode);

        bQuit = pVBIB->AbortedExit();

        // Clean up the scene
        pVBIB->Release();
    }

#ifdef UNDER_XBOX
    if (!(GetStartupContext() & TSTART_STRESS)) {
        pDisplay->ReleaseDevice();
        pDisplay->GetDirect3D8()->SetPushBufferSize(524288, 32768);
        pDisplay->CreateDevice();
    }
#endif

    return bRet;
}

//
// CDevice8Test Member functions
//

CVertexIndexBufferTest::CVertexIndexBufferTest()
{
    m_ModeOptions.fReference = true;
    m_ModeOptions.fRefEqualsSrc = false;
    m_ModeOptions.fTextures = false;
    m_ModeOptions.fZBuffer = false;
	m_ModeOptions.bSWDevices = true;
	m_ModeOptions.bHWDevices = true;
//    m_ModeOptions.dwDeviceTypes = DEVICETYPE_ALL;
    m_ModeOptions.uMinDXVersion = 0x800;

    m_pD3D =  NULL;
//    m_pDevice = NULL;
//    m_pRefDevice = NULL;

    m_bExit = FALSE;
}


UINT
CVertexIndexBufferTest::TestInitialize()
{
//    if( m_pD3D8 && m_pSrcDevice8 && m_pRefDevice8 )
//    {
        m_pD3D =  m_pDisplay->GetDirect3D8();
//        m_pDevice = m_pSrcDevice8;
//        m_pRefDevice = m_pRefDevice8;
//    }
//    else
//        return D3DTESTINIT_ABORT;

    // Set the test range to 1 - one pass through
    SetTestRange(1,1);

    return D3DTESTINIT_RUN;    
}


UINT
CVertexIndexBufferTest::GetAvailMemory
(
    D3DPOOL Pool
)
{
    UINT iRet = 0;

    // since there is no real way to get vertex and index buffer memory, assume it's in sysmem

    MEMORYSTATUS memstat;

    // check if really out of memory
    GlobalMemoryStatus( &memstat );

    iRet = memstat.dwAvailVirtual;

    return( iRet );
}

//******************************************************************************
BOOL CVertexIndexBufferTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CVertexIndexBufferTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CVertexIndexBufferTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;

#ifndef UNDER_XBOX

    switch (uMsg) {

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_ESCAPE: // Exit
                    m_bExit = TRUE;
            }

            break;
    }

#endif // !UNDER_XBOX

    return CD3DTest::WndProc(plr, hWnd, uMsg, wParam, lParam);
}

