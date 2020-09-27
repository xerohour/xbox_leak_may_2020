//
//  GetRasterStatus.cpp
//
//  IDirect3DDevice8::GetRasterStatus Tests.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2000 Microsoft Corporation.
//
//

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cgetrstat.h"

//
// Declare the framework
//

//CD3DWindowFramework	App;

//
// Define the groups here
//

//CGetRStat classCGetRStat;

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

    return (pd3dcaps->Caps & D3DCAPS_READ_SCANLINE);
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

    CGetRStat*  pGetRStat;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pGetRStat = new CGetRStat();
    if (!pGetRStat) {
        return FALSE;
    }

    // Initialize the scene
    if (!pGetRStat->Create(pDisplay)) {
        pGetRStat->Release();
        return FALSE;
    }

    bRet = pGetRStat->Exhibit(pnExitCode);

    // Clean up the scene
    pGetRStat->Release();

    return bRet;
}


//-----------------------------------------------------------------------------
// Constructor / Destructor
//-----------------------------------------------------------------------------

CGetRStat::CGetRStat()
{
    m_szTestName = _T("GetRasterStatus");
    m_szCommandKey = _T("CGetRStat");

    m_ModeOptions.uMinDXVersion = 0x800;
    m_ModeOptions.uMaxDXVersion = 0x800;

    m_ModeOptions.fReference = false;
    m_ModeOptions.fTextures = false;
    m_ModeOptions.fZBuffer = false;

    m_ModeOptions.bSWDevices = true;
    m_ModeOptions.bHWDevices = true;
//    m_ModeOptions.dwDeviceTypes = DEVICETYPE_ALL;

//    m_pDevice = NULL;
//    m_pD3D = NULL;
}

//-----------------------------------------------------------------------------
// Member Functions
//-----------------------------------------------------------------------------

UINT CGetRStat::TestInitialize()
{
    UINT uResult = CD3DTest::TestInitialize();

    if(uResult == D3DTESTINIT_RUN)
    {
//		m_pDevice = m_pSrcDevice8;
//		m_pD3D =  m_pD3D8;

        // set the test range
        SetTestRange(1,1);


        // log everything
//        g_pD3DFramework->m_pLog->SetOption( MAXFAILS, 0 );


        // set window title
//        TCHAR szText[MAX_PATH];

//        ConstructTestCaption(szText);
//        SetWindowText( g_pD3DFramework->m_hWindow, szText );
    }

    return uResult;
}

//-----------------------------------------------------------------------------

bool
CGetRStat::ExecuteTest
(
    UINT nTest
)
{
    static const FLOAT fRunTime = 5.0f;  // how long in seconds to run test for
    static const FLOAT fMaxDeviation = 0.15f;  // amount deviation allowed on display frequency
    static const FLOAT fScanLineRangeDeviation = 0.50f;  // deviation allowed on min/max scan lines

    static const INT iMinReasonableFrequency = 30;  // minimum bound for 'reasonable' refresh rates
    static const INT iMaxReasonableFrequency = 4000;  // maximum bound for 'reasonable' refresh rates


//    ClearFrame();
    if (m_pDisplay->IsDepthBuffered()) {
        m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, RGB_MAKE(0, 0, 50), 1.0f, 0);
    }
    else {
        m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, RGB_MAKE(0, 0, 50), 1.0f, 0);
    }

    BeginTestCase( TEXT("GetRasterStatus()"), nTest );


    HRESULT hr;

    D3DCAPS8 caps;

    hr = m_pDevice->GetDeviceCaps(&caps);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDeviceCaps")))
//    if(FAILED(hr))
    {
//        WriteToLog(
//            TEXT("GetDeviceCaps() returned %s (0x%x)\n"),
//            DecodeHResult(hr),
//            hr
//        );
        Fail();
    }
    else if( caps.Caps & D3DCAPS_READ_SCANLINE )
    {
        LARGE_INTEGER liFreq;
        LARGE_INTEGER liTime;
        LARGE_INTEGER liLast;
        LARGE_INTEGER liSumTime = {0};
        LARGE_INTEGER liSumBlankTime = {0};
        LARGE_INTEGER liStopTime;

        D3DRASTER_STATUS rast;
        D3DRASTER_STATUS rastlast;

        BOOL fInVBlank = FALSE;

        UINT nInVBlank = 0;
        UINT nSumScanDiff = 0;
        UINT nMinScanLine = 0xffffffff;
        UINT nMaxScanLine = 0;


        // get counter frequency
        QueryPerformanceFrequency( &liFreq );


        // initial time, rasterstatus
        QueryPerformanceCounter( &liLast );
        m_pDevice->GetRasterStatus( &rastlast );

        // end time
        liStopTime.QuadPart = liLast.QuadPart + ((LONGLONG)fRunTime * liFreq.QuadPart);

        // loop for a while, to get good averages
        while( liLast.QuadPart < liStopTime.QuadPart )
        {
            QueryPerformanceCounter( &liTime );
            hr = m_pDevice->GetRasterStatus( &rast );
            if( FAILED(hr) )
            {
                WriteToLog(
                    TEXT("GetRasterStatus() returned 0x%x\n"),
                    hr
                );
                Fail();
            }

            if( rast.InVBlank )
            {
                // make sure scanline is zero while in vblank
                if( rast.ScanLine != 0 )
                {
                    WriteToLog(
                        TEXT("GetRasterStatus() returned scanline %d when InVBlank was true\n"),
                        rast.ScanLine
                    );
                    Fail();
                }
            }
            else
            {
                // keep tally of min/max scan lines
                if( rast.ScanLine > nMaxScanLine )  nMaxScanLine = rast.ScanLine;
                if( rast.ScanLine < nMinScanLine )  nMinScanLine = rast.ScanLine;
            }


            if( !rast.InVBlank )
            {
                if( rastlast.InVBlank )
                {
                    // if coming out of blank, save blank time
                    rastlast.InVBlank = FALSE;
                    liSumBlankTime.QuadPart += (liTime.QuadPart - liLast.QuadPart);
                }
                else
                {
                    // compute time between last two calls, and count scan lines
                    if( rast.ScanLine > rastlast.ScanLine )
                    {
                        nSumScanDiff += (rast.ScanLine - rastlast.ScanLine);
                    }
#if 0  // can't be sure every vblank will be hit
                    else
                    {
                        WriteToLog(
                            TEXT("scan lines decreased without VBlank (%d after %d)\n"),
                            rast.ScanLine,
                            rastlast.ScanLine
                        );
                        Fail();
                    }
#endif

                    liSumTime.QuadPart += (liTime.QuadPart - liLast.QuadPart);
                }

                rastlast.ScanLine = rast.ScanLine;
                liLast = liTime;
            }
            else
            {
                if( !rastlast.InVBlank )
                {
                    // if starting blank, save time and increment counter
                    rastlast.InVBlank = TRUE;
                    nInVBlank++;
                    liLast = liTime;
                }
            }

        }


        if( nInVBlank == 0 )
        {
            WriteToLog( TEXT("no VBlanks detected\n") );
            Fail();
        }


        D3DDISPLAYMODE displaymode;

        // get display height, refresh rate
        m_pDevice->GetDisplayMode( &displaymode );

        // make sure scan line is reasonable
        if(
            nMinScanLine > (fScanLineRangeDeviation * displaymode.Height) ||
            nMaxScanLine < ((1.0f - fScanLineRangeDeviation) * displaymode.Height) ||
            nMaxScanLine > ((1.0f + fScanLineRangeDeviation) * displaymode.Height)
        )
        {
            WriteToLog(
                TEXT("min/max scan line unreasonable (min = %d, max = %d, height = %d)\n"),
                nMinScanLine,
                nMaxScanLine,
                displaymode.Height
            );
            Fail();
        }


        // calculate refresh rate based on scanline/vblank timings
        FLOAT fFreq = liFreq.QuadPart / (
            (FLOAT)liSumBlankTime.QuadPart / (FLOAT)nInVBlank +
            (FLOAT)displaymode.Height * (FLOAT)liSumTime.QuadPart / (FLOAT)nSumScanDiff
        );

        if( displaymode.RefreshRate > 1 )
        {
            // calculate percent error
            FLOAT fFreqDiff = ((FLOAT)displaymode.RefreshRate - fFreq) / (FLOAT)displaymode.RefreshRate;
            if( fFreqDiff < 0 ) fFreqDiff = -fFreqDiff;

            // check if error is reasonable
            if( fFreqDiff > fMaxDeviation )
            {
                WriteToLog(
                    TEXT("estimated frequency (%f) differed from actual (%d) by too much (%.2f%%)\n"),
                    fFreq,
                    displaymode.RefreshRate,
                    fFreqDiff * 100.0f
                );
                Fail();
            }
        }
        else
        {
            WriteToLog(
                TEXT("estimated frequency (%f) cannot be checked against optimal/default (%d)\n"),
                fFreq,
                displaymode.RefreshRate
            );

            if( (fFreq < iMinReasonableFrequency) || (fFreq > iMaxReasonableFrequency) )
            {
                WriteToLog(
                    TEXT("estimated frequency (%f) is not reasonable\n"),
                    fFreq
                );
                Fail();
            }
        }
    }
    else
    {
        // not supported, so should return INVALIDCALL
        D3DRASTER_STATUS rast;

        hr = m_pDevice->GetRasterStatus(&rast);
        if( hr != D3DERR_INVALIDCALL )
        {
            WriteToLog(
                TEXT("GetRasterStatus() returned %s (0x%x), expected %s (0x%x)\n"),
//                DecodeHResult(hr),
                TEXT(""),
                hr,
//                DecodeHResult(D3DERR_INVALIDCALL),
                TEXT("D3DERR_INVALIDCALL"),
                D3DERR_INVALIDCALL
            );
            Fail();
        }
    }


    EndTestCase();


    return(TRUE);
}

