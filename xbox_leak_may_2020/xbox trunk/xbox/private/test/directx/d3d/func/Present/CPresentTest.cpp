//
//  CPresentTest.cpp
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
#include "CPresentTest.h"



// need to access window handle
//extern CD3DWindowFramework *g_pD3DFramework;

//
// Declare the framework
//

//CD3DWindowFramework	App;

//
// Define the groups here
//

//GROUP_CREATE(CPresentTest);


#ifdef UNDER_XBOX

//******************************************************************************
BOOL IntersectRect(LPRECT lprcDst, CONST RECT *lprcSrc1, CONST RECT *lprcSrc2) {

    if (lprcSrc1->left >= lprcSrc2->right ||
        lprcSrc1->right <= lprcSrc2->left ||
        lprcSrc1->top >= lprcSrc2->bottom ||
        lprcSrc1->bottom <= lprcSrc2->top)
    {
        lprcDst->left = lprcDst->right = lprcDst->top = lprcDst->bottom = 0;
        return FALSE;
    }

    lprcDst->left = lprcSrc1->left > lprcSrc2->left ? lprcSrc1->left : lprcSrc2->left;
    lprcDst->right = lprcSrc1->right < lprcSrc2->right ? lprcSrc1->right : lprcSrc2->right;
    lprcDst->top = lprcSrc1->top > lprcSrc2->top ? lprcSrc1->top : lprcSrc2->top;
    lprcDst->bottom = lprcSrc1->bottom < lprcSrc2->bottom ? lprcSrc1->bottom : lprcSrc2->bottom;

    return TRUE;
}

#endif // UNDER_XBOX

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

    CPresentTest*   pPresentTest;
    BOOL            bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pPresentTest = new CPresentTest();
    if (!pPresentTest) {
        return FALSE;
    }

    // Initialize the scene
    if (!pPresentTest->Create(pDisplay)) {
        pPresentTest->Release();
        return FALSE;
    }

    bRet = pPresentTest->Exhibit(pnExitCode);

    // Clean up the scene
    pPresentTest->Release();

    return bRet;
}

//
// CDevice8Test Member functions
//

CPresentTest::CPresentTest()
{
    m_ModeOptions.fReference = false;
    m_ModeOptions.fTextures = false;
    m_ModeOptions.fZBuffer = false;
	m_ModeOptions.bSWDevices = true;
	m_ModeOptions.bHWDevices = true;
//    m_ModeOptions.dwDeviceTypes = DEVICETYPE_HAL;
    m_ModeOptions.uMinDXVersion = 0x800;

    m_szTestName = TEXT("Present");
    m_szCommandKey = TEXT("Present");

    m_pTex = NULL;
    m_pVB = NULL;
//    m_hWnd = NULL;
//    m_hWndOverride = NULL;
//    m_hWndFramework = NULL;

    m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY_VSYNC;
    m_d3dpp.BackBufferCount = 1;
    m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
}


UINT
CPresentTest::TestInitialize()
{
    if( (m_pD3D8 == NULL) || (m_pSrcDevice8 == NULL) )
    {
        return(D3DTESTINIT_ABORT);
    }


    LPTSTR szInfo = 
        TEXT("Failures are reported as follows:\n")
        TEXT("\n")
        TEXT("    dest {(DX,DY) [DR DG DB]} != src {(SX,SY)(SWxSH) [SRL-SRH SGL-SGH SBL-SBH]}    diff{[LR LG LB]}\n")
        TEXT("\n")
        TEXT("with the following symbols:\n")
        TEXT("\n")
        TEXT("    DX,DY     - the X,Y coordinates of the destination pixel in error\n")
        TEXT("    DR,DG,DB  - the red, green, and blue components of the destination pixel in error\n")
        TEXT("    SX,SY     - the X,Y coordinates of the source area contributing to this destination pixel\n")
        TEXT("    SW,SH     - the width and height of the source area contributing to this destination pixel\n")
        TEXT("    SRL,SRH   - the low and high values for the red component in this source area\n")
        TEXT("    SGL,SGH   - the low and high values for the green component in this source area\n")
        TEXT("    SBL,SBH   - the low and high values for the blue component in this source area\n")
        TEXT("    LR,LG,LB  - the magnitude of error for each color component\n")
        TEXT("\n")
        TEXT("\n")
        TEXT("\n");

    // output info to log
    WriteToLog( szInfo );


    // set test range
    ExecuteTest( (UINT)(-1) );


    m_SrcWidth = m_pDisplay->GetWidth();
    m_SrcHeight = m_pDisplay->GetHeight();

//    m_hWndFramework = g_pD3DFramework->m_DisplayWindow.m_hWindow;
    m_FBColor = D3DCOLOR_RGBA( 0x12, 0x34, 0x56, 0x0 );


    UINT iRet = D3DTESTINIT_RUN;
    HRESULT hr = D3D_OK;

    do
    {
        D3DTLVERTEX pVertices[] =
        {
        //  { sx, sy, sz, rhw, color, specular, tu, tv },
            { 0.0f, 0.0f, 0.0, 1.0, 0xffffffff, 0, 0.0f, 0.0f },
            { (float)m_SrcWidth, 0.0f, 0.0, 1.0, 0xffffffff, 0, 1.0f, 0.0f },
            { 0.0f, (float)m_SrcHeight, 0.0, 1.0, 0xffffffff, 0, 0.0f, 1.0f },
            { (float)m_SrcWidth, (float)m_SrcHeight, 0.0, 1.0, 0xffffffff, 0, 1.0f, 1.0f },
        };
        UINT nVertices = COUNTOF(pVertices);


        D3DDISPLAYMODE dm;
        hr = m_pSrcDevice8->GetDisplayMode( &dm );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("GetDisplayMode() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        m_ScreenWidth = dm.Width;
        m_ScreenHeight = dm.Height;


        LPBYTE pData = NULL;

/*
        hr = D3DXCreateTextureFromResource(
            m_pSrcDevice8,
            NULL,
            TEXT("TEXTURE"),
            &m_pTex
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("D3DXCreateTextureFromResource() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }
*/
        m_pTex = (CTexture8*)CreateTexture(m_pDevice, TEXT("texture.bmp"));
        if (!m_pTex) {
            continue;
        }

        hr = m_pSrcDevice8->SetTexture( 0, m_pTex );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetTexture() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice8->CreateVertexBuffer(
            nVertices * sizeof(D3DTLVERTEX),
            0,
            D3DFVF_TLVERTEX,
            POOL_DEFAULT,
            &m_pVB
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateVertexBuffer(%d,%d,0x%x,%d,) returned %s (0x%x)\n"),
                nVertices * sizeof(D3DTLVERTEX),
                0,
                D3DFVF_TLVERTEX,
                D3DPOOL_DEFAULT,
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        hr = m_pVB->Lock( 0, 0, &pData, 0 );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("Lock(0,0,,0) returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        CopyMemory( pData, pVertices, nVertices * sizeof(D3DTLVERTEX) );

        m_pVB->Unlock();

        hr = m_pSrcDevice8->SetStreamSource( 0, m_pVB, sizeof(D3DTLVERTEX) );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetStreamSource(0,,) returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice8->SetVertexShader( D3DFVF_TLVERTEX );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("SetVertexShader(D3DFVF_TLVERTEX) returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

/*
        // create window for hwnd override
        WNDCLASS wc =
        {
            0,  // style
            (WNDPROC)DefWindowProc,
            0,  // cbClsExtra
            0,  // cbWndExtra
            NULL,
            NULL,
            LoadCursor( NULL, IDC_CROSS ),
            (HBRUSH)(COLOR_3DFACE+1),
            NULL,  // lpszMenuName
            TEXT("noname")
        };

        if( !RegisterClass(&wc) )
        {
            DWORD dwErr = GetLastError();

            if( (dwErr != 0) && (dwErr != ERROR_CLASS_ALREADY_EXISTS) )
            {
                WriteToLog(
                    TEXT("RegisterClass() failed, last error %d (0x%x)\n"),
                    dwErr,
                    dwErr
                );
                hr = E_FAIL;
                continue;
            }
        }

        m_hWndOverride = CreateWindow(
            TEXT("noname"),
            TEXT("this space for rent"),
            WS_POPUP,
            m_ScreenWidth / 3,
            m_ScreenHeight / 3,
            m_ScreenWidth / 3,
            m_ScreenHeight / 3,
            NULL,
            NULL,
            NULL,
            NULL
        );
        if( m_hWndOverride == NULL )
        {
            WriteToLog(
                TEXT("CreateWindow() failed, last error %d (0x%x)\n"),
                GetLastError(),
                GetLastError()
            );
            hr = E_FAIL;
            continue;
	    }
*/
    }
    while(0);

    if( FAILED(hr) )
    {
        // release stuff
//        DestroyWindow(m_hWndOverride);
        ReleaseTexture(m_pTex);
        ReleaseVertexBuffer(m_pVB);

        iRet = D3DTESTINIT_ABORT;
    }

    return(iRet);
}


bool
CPresentTest::TestTerminate()
{
//    if( m_hWndOverride )
//    {
//        DestroyWindow(m_hWndOverride);
//    }

    ReleaseTexture(m_pTex);
    ReleaseVertexBuffer(m_pVB);

    return(true);
}


bool
CPresentTest::ExecuteTest
(
    UINT nTest
)
{
    static const RECT_SCALE RectScale[] =
    {
        { 0.0f, 0.0f, 1.0f, 1.0f },  // full area
        { 0.0f, 0.0f, 0.5f, 0.5f },  // half width, half height
        { 0.0f, 0.0f, 1.0f, 0.5f },  // full width, half height
        { 0.0f, 0.0f, 0.5f, 1.0f },  // half width, full height
        { -0.5f, -0.5f, 0.5f, 0.5f },  // full area, offset on top left

        { 0.5f, 0.5f, 1.5f, 1.5f },  // full area, offset on bottom right
        { -0.25f, -0.25f, 0.25f, 0.25f },  // half width/height, offset on top left
        { 0.33f, 0.33f, 0.66f, 0.66f },  // third width/height
        { 0.0f, 0.0f, 0.1f, 1.0f },  // vertical strip
        { 0.0f, 0.0f, 1.0f, 0.1f },  // horizontal strip

        { -0.5f, -0.5f, 1.5f, 1.5f },  // double height/width
        { -0.5f, 0.5f, 0.5f, 1.5f },  // full area, offset top right
        { 0.5f, -0.5f, 1.5f, 0.5f },  // full area, offset bottom left
        { 0.0f, -0.5f, 1.0f, 0.5f },  // full area, offset top
        { 0.0f, 0.5f, 1.0f, 1.5f },  // full area, offset bottom

        { -0.5f, 0.0f, 0.5f, 1.0f },  // full area, offset left
        { 0.5f, 0.0f, 1.5f, 1.0f },  // full area, offset right
    };

    static const RECT Rects[] =
    {
        // absolute rects
        { 0, 0, 1, 1 },
        { 0, 0, 13, 31 },  // unaligned
        { 3, 7, 99, 131 },  // unaligned
        { 11, 11, 67, 67 },  // unaligned
    };

    static const HWND Hwnds[] =
    {
        NULL,
    };

    static const D3DTEST_RECT TestRects[] =
    {
    //  { szName, bScale, pRectScale, pRect },
        { TEXT("default"), FALSE, NULL, NULL },
        { TEXT("full area"), TRUE, &RectScale[0], NULL },
        { TEXT("half width/height"), TRUE, &RectScale[1], NULL },
        { TEXT("half height"), TRUE, &RectScale[2], NULL },
        { TEXT("half width"), TRUE, &RectScale[3], NULL },
        { TEXT("offset top left"), TRUE, &RectScale[4], NULL },
        { TEXT("offset bottom right"), TRUE, &RectScale[5], NULL },
        { TEXT("half width/height, offset top left"), TRUE, &RectScale[6], NULL },
        { TEXT("third width/height"), TRUE, &RectScale[7], NULL },
        { TEXT("vertical strip"), TRUE, &RectScale[8], NULL },
        { TEXT("horizontal strip"), TRUE, &RectScale[9], NULL },
        { TEXT("double width/height"), TRUE, &RectScale[10], NULL },
        { TEXT("offset top right"), TRUE, &RectScale[11], NULL },
        { TEXT("offset bottom left"), TRUE, &RectScale[12], NULL },
        { TEXT("offset top"), TRUE, &RectScale[13], NULL },
        { TEXT("offset bottom"), TRUE, &RectScale[14], NULL },
        { TEXT("offset left"), TRUE, &RectScale[15], NULL },
        { TEXT("offset right"), TRUE, &RectScale[16], NULL },
        { TEXT("one pixel"), FALSE, NULL, &Rects[0] },
        { TEXT("unaligned 13x31"), FALSE, NULL, &Rects[1] },
        { TEXT("unaligned 96x124"), FALSE, NULL, &Rects[2] },
        { TEXT("unaligned 56x56"), FALSE, NULL, &Rects[3] },
    };

    static const D3DTEST_PRESENT TestList[] =
    {
    //  { Name, bScaleSrc, pSrcRectScale, pSrcRect, bScaleDest, pDestRectScale, pDestRect, bDestWndOverride, hDestWindowOverride, pDirtyRegion },

        // boring call
        { TEXT("source = NULL, dest = NULL"), FALSE, NULL, NULL, FALSE, NULL, NULL, FALSE, NULL },//, NULL },

        // random test cases can go here
    };
    static const UINT nTestList = COUNTOF(TestList);


    static RECT SrcRect;
    static RECT DestRect;


    typedef union __TESTCASE
    {
        struct
        {
            UINT SrcRect : 6;
            UINT DestRect : 6;
            UINT WndOverride : 2;
            UINT ListIndex : 3;
            UINT DoList : 1;
        };
        UINT nTest;
    }
    TESTCASE;

    static const TESTCASE tcCount =
    {
        COUNTOF(TestRects),
        COUNTOF(TestRects),
//        2,  // WndOverride
        1,  // WndOverride
        COUNTOF(TestList),
        0,  // DoList; count not used
    };


    TESTCASE tc;
    tc.nTest = (nTest - 1);

    if( nTest == (UINT)(-1) )
    {
        ClearRangeList();

        tc.nTest = 0;
        for( tc.SrcRect = 0; tc.SrcRect < tcCount.SrcRect; tc.SrcRect++ )
        {
            for( tc.DestRect = 0; tc.DestRect < tcCount.DestRect; tc.DestRect++ )
            {
                for( tc.WndOverride = 0; tc.WndOverride < tcCount.WndOverride; tc.WndOverride++ )
                {
                    AddTestRange( (tc.nTest+1), (tc.nTest+1) );
                }
            }
        }

        tc.nTest = 0;
        tc.DoList = 1;
        for( tc.ListIndex = 0; tc.ListIndex < tcCount.ListIndex; tc.ListIndex++ )
        {
            AddTestRange( (tc.nTest+1), (tc.nTest+1) );
        }

        return( FALSE );
    }

    if(
        (
            (tc.DoList == 0) &&
            (
                (tc.SrcRect >= tcCount.SrcRect) ||
                (tc.DestRect >= tcCount.DestRect) ||
                (tc.WndOverride >= tcCount.WndOverride)
            )
        ) ||
        (
            (tc.DoList == 1) &&
            (tc.ListIndex >= tcCount.ListIndex)
        )
    )
    {
        return(FALSE);
    }

    TCHAR szName[MAX_PATH];

    if( tc.DoList )
    {
        const D3DTEST_PRESENT *curtest = &TestList[tc.ListIndex];
/*
        if( curtest->bDestWndOverride )
        {
            m_hDestWnd = m_hWndOverride;
            m_hWnd = m_hWndOverride;
            ShowWindow( m_hWndOverride, SW_SHOWNORMAL );
        }
        else
        {
            m_hDestWnd = curtest->hDestWindowOverride;
            m_hWnd = m_hWndFramework;
            ShowWindow( m_hWndOverride, SW_HIDE );
        }
*/

//        RECT rc;
//        GetClientRect( m_hWnd, &rc );
        m_DestWidth = m_pDisplay->GetWidth();//rc.right;
        m_DestHeight = m_pDisplay->GetHeight();//rc.bottom;


        if( curtest->bScaleSrc )
        {
            SrcRect.left = (LONG)(curtest->pSrcRectScale->left * (float)m_SrcWidth);
            SrcRect.right = (LONG)(curtest->pSrcRectScale->right * (float)m_SrcWidth);
            SrcRect.top = (LONG)(curtest->pSrcRectScale->top * (float)m_SrcHeight);
            SrcRect.bottom = (LONG)(curtest->pSrcRectScale->bottom * (float)m_SrcHeight);
            m_pSrcRect = &SrcRect;
        }
        else
        {
            m_pSrcRect = curtest->pSrcRect;
        }

        if( curtest->bScaleDest )
        {
            DestRect.left = (LONG)(curtest->pDestRectScale->left * (float)m_DestWidth);
            DestRect.right = (LONG)(curtest->pDestRectScale->right * (float)m_DestWidth);
            DestRect.top = (LONG)(curtest->pDestRectScale->top * (float)m_DestHeight);
            DestRect.bottom = (LONG)(curtest->pDestRectScale->bottom * (float)m_DestHeight);
            m_pDestRect = &DestRect;
        }
        else
        {
            m_pDestRect = curtest->pDestRect;
        }

//        m_pDirtyRegion = curtest->pDirtyRegion;

        wsprintf( szName, TEXT("%s"), curtest->szName );
    }
    else
    {
/*
        if( tc.WndOverride )
        {
            m_hDestWnd = m_hWndOverride;
            m_hWnd = m_hWndOverride;
            ShowWindow( m_hWndOverride, SW_SHOWNORMAL );
        }
        else
        {
            m_hDestWnd = NULL;
            m_hWnd = m_hWndFramework;
            ShowWindow( m_hWndOverride, SW_HIDE );
        }
*/
        RECT rc;
//        GetClientRect( m_hWnd, &rc );
        m_DestWidth = m_pDisplay->GetWidth();//rc.right;
        m_DestHeight = m_pDisplay->GetHeight();//rc.bottom;

        const D3DTEST_RECT *pSrc = &TestRects[tc.SrcRect];
        const D3DTEST_RECT *pDest = &TestRects[tc.DestRect];


        if( pSrc->bScale )
        {
            SrcRect.left = (LONG)(pSrc->pRectScale->left * (float)m_SrcWidth);
            SrcRect.right = (LONG)(pSrc->pRectScale->right * (float)m_SrcWidth);
            SrcRect.top = (LONG)(pSrc->pRectScale->top * (float)m_SrcHeight);
            SrcRect.bottom = (LONG)(pSrc->pRectScale->bottom * (float)m_SrcHeight);
            m_pSrcRect = &SrcRect;
        }
        else
        {
            m_pSrcRect = pSrc->pRect;
        }

        if( pDest->bScale )
        {
            DestRect.left = (LONG)(pDest->pRectScale->left * (float)m_DestWidth);
            DestRect.right = (LONG)(pDest->pRectScale->right * (float)m_DestWidth);
            DestRect.top = (LONG)(pDest->pRectScale->top * (float)m_DestHeight);
            DestRect.bottom = (LONG)(pDest->pRectScale->bottom * (float)m_DestHeight);
            m_pDestRect = &DestRect;
        }
        else
        {
            m_pDestRect = pDest->pRect;
        }

//        m_pDirtyRegion = NULL;

        wsprintf( szName,
            TEXT("dest{%s} <= src{%s}, WndOverride = %s"),
            pDest->szName,
            pSrc->szName,
            tc.WndOverride ? TEXT("true") : TEXT("false")
        );


        if( !ValidateRects( m_pSrcRect, m_pDestRect ) )
        {
            return(FALSE);
        }
    }


    BeginTestCase( szName, nTest );


    HRESULT hr;

    do
    {
        // clear front buffer some funky color
#ifndef UNDER_XBOX
        HWND hWnd = m_pDisplay->GetWindow();
        HDC hDC = GetDC( hWnd );
        HGDIOBJ hBrushPrev = SelectObject( hDC, CreateSolidBrush( m_FBColor ) );
        PatBlt( hDC, 0, 0, m_DestWidth, m_DestHeight, PATCOPY );
        DeleteObject( SelectObject( hDC, hBrushPrev ) );
        ReleaseDC( hWnd, hDC );
#else
        CSurface8* pd3ds;
        D3DSURFACE_DESC d3dsd;
        D3DLOCKED_RECT d3dlr;
        DWORD dwColor;
        UINT i, j;

        hr = m_pSrcDevice8->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
        if ( FAILED(hr) )
        {
            WriteToLog(
                TEXT("GetBackBuffer() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        hr = pd3ds->GetDesc(&d3dsd);
        if ( FAILED(hr) )
        {
            WriteToLog(
                TEXT("GetDesc() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        hr = pd3ds->LockRect(&d3dlr, NULL, 0);
        if ( FAILED(hr) )
        {
            WriteToLog(
                TEXT("LockRect() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        dwColor = ColorToPixel(d3dsd.Format, NULL, m_FBColor);

        switch (FormatToBitDepth(d3dsd.Format)) {

            case 16: {
                LPWORD pwPixel = (LPWORD)d3dlr.pBits;
                for (i = 0; i < d3dsd.Height; i++) {
                    for (j = 0; j < d3dsd.Width; j++) {
                        pwPixel[j] = (WORD)dwColor;
                    }
                    pwPixel += d3dlr.Pitch / 2;
                }
                break;
            }

            case 32: {
                LPDWORD pdwPixel = (LPDWORD)d3dlr.pBits;
                for (i = 0; i < d3dsd.Height; i++) {
                    for (j = 0; j < d3dsd.Width; j++) {
                        pdwPixel[j] = dwColor;
                    }
                    pdwPixel += d3dlr.Pitch / 4;
                }
                break;
            }

            default:
                Log(LOG_FAIL, TEXT("IDirect3DSurface8::GetDesc on the front buffer returned an incorrect format of %d"), d3dsd.Format);
                pd3ds->UnlockRect();
                pd3ds->Release();
                continue;
        }

        pd3ds->UnlockRect();
        pd3ds->Release();

#endif // UNDER_XBOX

        m_pDevice->SetViewport(&m_vpFull);

        hr = m_pSrcDevice8->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 0.0f, 0 );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("Clear() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice8->BeginScene();
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("BeginScene() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice8->SetStreamSource( 0, m_pVB, sizeof(D3DTLVERTEX) );
        hr = m_pSrcDevice8->SetVertexShader( D3DFVF_TLVERTEX );

        hr = m_pSrcDevice8->DrawPrimitive(
            D3DPT_TRIANGLESTRIP,
            0,
            2
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2) returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice8->EndScene();
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("EndScene() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }
    }
    while(0);

    if( FAILED(hr) )
    {
        Abort();
    }
    else
    {
        hr = m_pSrcDevice8->Present(
            (LPRECT)m_pSrcRect,
            (LPRECT)m_pDestRect,
            NULL,
            NULL
//            m_hDestWnd,
//            m_pDirtyRegion
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("Present() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            Fail();
        }
    }

    ProcessFrame();

    return( false );
}


// override present call
bool
CPresentTest::DisplayFrame(void)
{
    return(true);
}



bool
CPresentTest::CompareRangePixel
(
    MYCOLOR SrcMin,
    MYCOLOR SrcMax,
    MYCOLOR Dest,
    D3DFORMAT Format,
    MYCOLOR *Diff
)
{
    bool bRet = true;
    MYCOLOR Bits;
    MYCOLOR Mask;
    MYCOLOR Error;


    Mask.alpha = 0x00;

    switch(Format)
    {
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
        case D3DFMT_P8:
#ifdef UNDER_XBOX
        case D3DFMT_LIN_A8R8G8B8:
        case D3DFMT_LIN_X8R8G8B8:
#endif
            Bits.red = 8;
            Bits.green = 8;
            Bits.blue = 8;
            break;

        case D3DFMT_R5G6B5:
            Bits.red = 5;
            Bits.green = 6;
            Bits.blue = 5;
            break;

        case D3DFMT_X1R5G5B5:
#ifdef UNDER_XBOX
        case D3DFMT_LIN_X1R5G5B5:
#endif
            Bits.red = 5;
            Bits.green = 5;
            Bits.blue = 5;
            break;

        default:
            WriteToLog(
                TEXT("Format is not supported by comparison: %d (0x%x)\n"),
                Format
            );
            bRet = false;
            break;
    }

    Error.red = ((0xff >> (8 - Bits.red / 2)) << (8 - Bits.red));
    Error.green = ((0xff >> (8 - Bits.green / 2)) << (8 - Bits.green));
    Error.blue = ((0xff >> (8 - Bits.blue / 2)) << (8 - Bits.blue));

    Mask.red = (0xff << (8 - Bits.red));
    Mask.green = (0xff << (8 - Bits.green));
    Mask.blue = (0xff << (8 - Bits.blue));

    if(bRet)
    {
        SrcMin.color &= Mask.color;
        SrcMax.color &= Mask.color;
        Dest.color &= Mask.color;

        int DiffRed;
        int DiffGreen;
        int DiffBlue;

        DiffRed =
            ( Dest.red < SrcMin.red ) ? (int)Dest.red - (int)SrcMin.red :
            ( Dest.red > SrcMax.red ) ? (int)Dest.red - (int)SrcMax.red :
            0;

        DiffGreen =
            ( Dest.green < SrcMin.green ) ? (int)Dest.green - (int)SrcMin.green :
            ( Dest.green > SrcMax.green ) ? (int)Dest.green - (int)SrcMax.green :
            0;

        DiffBlue =
            ( Dest.blue < SrcMin.blue ) ? (int)Dest.blue - (int)SrcMin.blue :
            ( Dest.blue > SrcMax.blue ) ? (int)Dest.blue - (int)SrcMax.blue :
            0;

        bRet =
            (DiffRed <= (int)Error.red) && (DiffRed >= -(int)Error.red) &&
            (DiffGreen <= (int)Error.green) && (DiffGreen >= -(int)Error.green) &&
            (DiffBlue <= (int)Error.blue) && (DiffBlue >= -(int)Error.blue);

        if( Diff )
        {
            Diff->red = (BYTE)DiffRed;
            Diff->green = (BYTE)DiffGreen;
            Diff->blue = (BYTE)DiffBlue;
        }
    }

    return(bRet);
}



bool
CPresentTest::CompareSurfaces
(
    D3DLOCKED_RECT *pSrcLocked,
    D3DLOCKED_RECT *pDestLocked,
    D3DFORMAT Format
)
{
    static const UINT nMaxErrors = 15;  // number of error pixels to report in detail
    static const FLOAT fThreshold = 0.05f;  // percentage of destination pixels that are allowed to be errors


    INT nErrors = 0;

    RECT SrcRect;
    RECT DestRect;
    FLOAT ScaleX;
    FLOAT ScaleY;

    MYCOLOR FBColor;
    MYCOLOR Diff;


    if( m_pSrcRect )
    {
        SrcRect.left = m_pSrcRect->left;
        SrcRect.top = m_pSrcRect->top;
        SrcRect.right = m_pSrcRect->right;
        SrcRect.bottom = m_pSrcRect->bottom;
    }
    else
    {
        SrcRect.left = 0;
        SrcRect.top = 0;
        SrcRect.right = m_SrcWidth;
        SrcRect.bottom = m_SrcHeight;
    }

    if( m_pDestRect )
    {
        DestRect.left = m_pDestRect->left;
        DestRect.top = m_pDestRect->top;
        DestRect.right = m_pDestRect->right;
        DestRect.bottom = m_pDestRect->bottom;
    }
    else
    {
        DestRect.left = 0;
        DestRect.top = 0;
        DestRect.right = m_DestWidth;
        DestRect.bottom = m_DestHeight;
    }


    ScaleX = (FLOAT)(SrcRect.right - SrcRect.left)/(FLOAT)(DestRect.right - DestRect.left);
    ScaleY = (FLOAT)(SrcRect.bottom - SrcRect.top)/(FLOAT)(DestRect.bottom - DestRect.top);

    FBColor.red = (BYTE)RGBA_GETRED(m_FBColor);
    FBColor.green = (BYTE)RGBA_GETGREEN(m_FBColor);
    FBColor.blue = (BYTE)RGBA_GETBLUE(m_FBColor);
    FBColor.alpha = 0;


    for( int y = 0; y < (int)m_DestHeight; y++ )
    {
        for( int x = 0; x < (int)m_DestWidth; x++ )
        {
            MYCOLOR DestPix;

            DestPix.color =
                *(DWORD*)( (UINT_PTR)pDestLocked->pBits +
                y * pDestLocked->Pitch +
                x * 4);

            DestPix.alpha = 0;  // ignore alpha

            if(
                (x >= DestRect.left) && (x < DestRect.right) &&
                (y >= DestRect.top) && (y < DestRect.bottom)
            )
            {
                INT SrcX = (INT)((FLOAT)(x - DestRect.left) * ScaleX) + SrcRect.left;
                INT SrcY = (INT)((FLOAT)(y - DestRect.top) * ScaleY) + SrcRect.top;
                INT RangeX;
                INT RangeY;

                if( ScaleX == 1.0f )
                {
                    RangeX = 1;
                }
                else if( ScaleX < 1.0f )
                {
                    RangeX = 3;
                    SrcX--;
                }
                else if( ScaleX > 1.0f )
                {
                    RangeX = (int)ceil(ScaleX);

                    SrcX -= RangeX / 2;
                    RangeX += RangeX;
                }

                if( ScaleY == 1.0f )
                {
                    RangeY = 1;
                }
                else if( ScaleY < 1.0f )
                {
                    RangeY = 3;
                    SrcY--;
                }
                else if( ScaleY > 1.0f )
                {
                    RangeY = (int)ceil(ScaleY);

                    SrcY -= RangeY / 2;
                    RangeY += RangeY;
                }


                MYCOLOR SrcPix;
                MYCOLOR SrcMin;
                MYCOLOR SrcMax;

                SrcMin.color = 0xffffffff;
                SrcMax.color = 0;

                UINT nSrcPix = 0;
                UINT nSrcPixSkip = 0;

                for( int yoff = SrcY; yoff < (SrcY + RangeY); yoff++ )
                {
                    for( int xoff = SrcX; xoff < (SrcX + RangeX); xoff++ )
                    {
                        if(
                            (xoff >= 0) && (xoff < (int)m_SrcWidth) &&
                            (yoff >= 0) && (yoff < (int)m_SrcHeight)
                        )
                        {
                            SrcPix.color =
                                *(DWORD*)( (UINT_PTR)pSrcLocked->pBits +
                                yoff * pSrcLocked->Pitch +
                                xoff * 4 );

                            SrcMin.red = (SrcPix.red < SrcMin.red) ? SrcPix.red : SrcMin.red;
                            SrcMax.red = (SrcPix.red > SrcMax.red) ? SrcPix.red : SrcMax.red;
                            SrcMin.green = (SrcPix.green < SrcMin.green) ? SrcPix.green : SrcMin.green;
                            SrcMax.green = (SrcPix.green > SrcMax.green) ? SrcPix.green : SrcMax.green;
                            SrcMin.blue = (SrcPix.blue < SrcMin.blue) ? SrcPix.blue : SrcMin.blue;
                            SrcMax.blue = (SrcPix.blue > SrcMax.blue) ? SrcPix.blue : SrcMax.blue;

                            nSrcPix++;
                        }
                        else
                        {
                            nSrcPixSkip++;
                        }
                    }
                }

                if( nSrcPix > nSrcPixSkip )
                {
                    // valid pixel
                    if( !CompareRangePixel( SrcMin, SrcMax, DestPix, Format, &Diff ) )
                    {
                        if( nErrors++ < nMaxErrors )
                        {
                            WriteToLog(
                                TEXT("dest {(%d,%d) [%.2x %.2x %.2x]} != src {(%d,%d)(%dx%d) [%.2x-%.2x %.2x-%.2x %.2x-%.2x]}   diff{[%.2x %.2x %.2x]}\n"),
                                x,
                                y,
                                DestPix.red,
                                DestPix.green,
                                DestPix.blue,
                                SrcX,
                                SrcY,
                                RangeX,
                                RangeY,
                                SrcMin.red,
                                SrcMax.red,
                                SrcMin.green,
                                SrcMax.green,
                                SrcMin.blue,
                                SrcMax.blue,
                                Diff.red,
                                Diff.green,
                                Diff.blue
                            );
                        }
                    }
                }
                else if( nSrcPix == 0 )
                {
                    // skipped pixel
                    if( !CompareRangePixel( FBColor, FBColor, DestPix, Format, &Diff ) )
                    {
                        if( nErrors++ < nMaxErrors )
                        {
                            WriteToLog(
                                TEXT("dest {(%d,%d) [%.2x %.2x %.2x]} != src {(%d,%d)(%dx%d) [%.2x %.2x %.2x] (outside source surface)}   diff{[%d %d %d]}\n"),
                                x,
                                y,
                                DestPix.red,
                                DestPix.green,
                                DestPix.blue,
                                SrcX,
                                SrcY,
                                RangeX,
                                RangeY,
                                FBColor.red,
                                FBColor.green,
                                FBColor.blue,
                                (char)Diff.red,
                                (char)Diff.green,
                                (char)Diff.blue
                            );
                        }
                    }
                }
                else  // 0 < nSrcPix <= nSrcPixSkip
                {
                    // allow either of above
                    if(
                        !CompareRangePixel( SrcMin, SrcMax, DestPix, Format, &Diff ) &&
                        !CompareRangePixel( FBColor, FBColor, DestPix, Format, NULL )
                    )
                    {
                        if( nErrors++ < nMaxErrors )
                        {
                            WriteToLog(
                                TEXT("dest {(%d,%d) [%.2x %.2x %.2x]} != src {(%d,%d)(%dx%d) [%.2x-%.2x %.2x-%.2x %.2x-%.2x] || [%.2x %.2x %.2x]}   diff{[%.2x %.2x %.2x]}\n"),
                                x,
                                y,
                                DestPix.red,
                                DestPix.green,
                                DestPix.blue,
                                SrcX,
                                SrcY,
                                RangeX,
                                RangeY,
                                SrcMin.red,
                                SrcMax.red,
                                SrcMin.green,
                                SrcMax.green,
                                SrcMin.blue,
                                SrcMax.blue,
                                FBColor.red,
                                FBColor.green,
                                FBColor.blue,
                                Diff.red,
                                Diff.green,
                                Diff.blue
                            );
                        }
                    }
                }
            }
            else
            {
                // make sure points outside dest rect are funk colored
                if( !CompareRangePixel( FBColor, FBColor, DestPix, Format, &Diff ) )
                {
                    if( nErrors++ < nMaxErrors )
                    {
                        WriteToLog(
                            TEXT("dest {(%d,%d) [%.2x %.2x %.2x]} != src {[%.2x %.2x %.2x] (outside blit destination)}   diff{[%d %d %d]}\n"),
                            x,
                            y,
                            DestPix.red,
                            DestPix.green,
                            DestPix.blue,
                            FBColor.red,
                            FBColor.green,
                            FBColor.blue,
                            (char)Diff.red,
                            (char)Diff.green,
                            (char)Diff.blue
                        );
                    }
                }
            }
        }
    }

    if( nErrors )
    {
        WriteToLog(
            TEXT("    destination pixels (incorrect/total):  %d / %d\n"),
            nErrors,
            DestRect.right * DestRect.bottom
        );
    }

    return(nErrors <= (fThreshold * DestRect.right * DestRect.bottom));
}


bool
CPresentTest::ProcessFrame(void)
{
    HRESULT hr;
    BOOL bRet;
    D3DDISPLAYMODE dm;
    CSurface8* pTempSurf = NULL;
    CSurface8* pSrcSurf = NULL;
    CSurface8* pDestSurf = NULL;
//    WINDOWINFO wi = { sizeof(wi) };
    RECT DestRect;
    D3DLOCKED_RECT SrcLocked;
    D3DLOCKED_RECT DestLocked;
#ifdef UNDER_XBOX
    CSurface8* pd3dsFront;
#endif

    do
    {
        hr = m_pSrcDevice8->GetDisplayMode( &dm );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("GetDisplayMode() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        // source surface, backbuffer
        hr = m_pSrcDevice8->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pTempSurf );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,) returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        hr = m_pSrcDevice8->CreateImageSurface(
            m_SrcWidth,
            m_SrcHeight,
#ifndef UNDER_XBOX
            D3DFMT_A8R8G8B8,
#else
            D3DFMT_LIN_A8R8G8B8,
#endif
            &pSrcSurf
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateImageSurface(,,D3DFMT_A8R8G8B8,) returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

        hr = D3DXLoadSurfaceFromSurface(
            pSrcSurf->GetIDirect3DSurface8(),
            NULL,
            NULL,
            pTempSurf->GetIDirect3DSurface8(),
            NULL,
            NULL,
            D3DX_DEFAULT,
            0
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("D3DXLoadSurfaceFromSurface() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }
        // Sync the surface on the server side (##REVIEW: Is this even necessary since we are manually verifying the bits on the client?)
        pSrcSurf->LockRect(&SrcLocked, NULL, 0);
        pSrcSurf->UnlockRect();

        hr = pSrcSurf->LockRect( &SrcLocked, NULL, D3DLOCK_READONLY );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("LockRect(,NULL,D3DLOCK_READONLY) returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }


        // destination surface, front buffer
        hr = m_pSrcDevice8->CreateImageSurface(
            m_ScreenWidth,
            m_ScreenHeight,
#ifndef UNDER_XBOX
            D3DFMT_A8R8G8B8,
#else
            D3DFMT_LIN_A8R8G8B8,
#endif
            &pDestSurf
        );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("CreateImageSurface(,,D3DFMT_A8R8G8B8,) returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

#ifndef UNDER_XBOX

        hr = m_pSrcDevice8->GetFrontBuffer( pDestSurf );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("GetFrontBuffer() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

#else

        hr = m_pSrcDevice8->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pd3dsFront);
        if (FAILED(hr)) {
            WriteToLog(
                TEXT("GetBackBuffer(-1, ...) returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }            

        hr = D3DXLoadSurfaceFromSurface(
            pDestSurf->GetIDirect3DSurface8(),
            NULL,
            NULL,
            pd3dsFront->GetIDirect3DSurface8(),
            NULL,
            NULL,
            D3DX_DEFAULT,
            0
        );

        RELEASE(pd3dsFront);

        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("D3DXLoadSurfaceFromSurface() returned %s (0x%x)\n"),
                DecodeHResult(hr),
                hr
            );
            continue;
        }

#endif // UNDER_XBOX

/*
        bRet = GetWindowInfo( m_hWnd, &wi );
        if( !bRet )
        {
            WriteToLog(
                TEXT("GetWindowInfo() failed, last error %d (0x%x)\n"),
                GetLastError(),
                GetLastError()
            );
            hr = E_FAIL;
            continue;
        }
*/
        DestRect.left = 0;//wi.rcClient.left;
        DestRect.top = 0;//wi.rcClient.top;
        DestRect.right = DestRect.left + m_DestWidth;
        DestRect.bottom = DestRect.top + m_DestHeight;

        hr = pDestSurf->LockRect( &DestLocked, &DestRect, D3DLOCK_READONLY );
        if( FAILED(hr) )
        {
            WriteToLog(
                TEXT("LockRect(,(%d,%d,%d,%d),0x%x) returned %s (0x%x)\n"),
                DestRect.left,
                DestRect.top,
                DestRect.right,
                DestRect.bottom,
                D3DLOCK_READONLY,
                DecodeHResult(hr),
                hr
            );
            continue;
        }


        // run comparison
        if(!CompareSurfaces( &SrcLocked, &DestLocked, dm.Format ))
        {
            Fail();
        }
    }
    while(0);

    if( FAILED(hr) )
    {
        Abort();
    }

    pSrcSurf->UnlockRect();
    pDestSurf->UnlockRect();
    RELEASE(pTempSurf);
    RELEASE(pSrcSurf);
    RELEASE(pDestSurf);


    return(true);
}



BOOL
CPresentTest::ValidateRects
(
    const RECT *pSrc,
    const RECT *pDest
)
{
    if( (pSrc == NULL) || (pDest == NULL) )
    {
        return(TRUE);
    }

    RECT rcSrcSurf;
    RECT rcDestSurf;
    RECT rcSrcVis;
    RECT rcDestVis;
    RECT rcTemp;
    RECT rcTemp2;

    UINT SrcWidth = pSrc->right - pSrc->left;
    UINT SrcHeight = pSrc->bottom - pSrc->top;
    UINT DestWidth = pDest->right - pDest->left;
    UINT DestHeight = pDest->bottom - pDest->top;


    rcSrcSurf.left = 0;
    rcSrcSurf.top = 0;
    rcSrcSurf.right = m_SrcWidth;
    rcSrcSurf.bottom = m_SrcHeight;

    rcDestSurf.left = 0;
    rcDestSurf.top = 0;
    rcDestSurf.right = m_DestWidth;
    rcDestSurf.bottom = m_DestHeight;

    IntersectRect( &rcSrcVis, pSrc, &rcSrcSurf );
    IntersectRect( &rcDestVis, pDest, &rcDestSurf );

    rcTemp.left = pDest->left + (((rcSrcVis.left - pSrc->left) * DestWidth) / SrcWidth);
    rcTemp.top = pDest->top + (((rcSrcVis.top - pSrc->top) * DestHeight) / SrcHeight);
    rcTemp.right = rcTemp.left + (((rcSrcVis.right - rcSrcVis.left) * DestWidth) / SrcWidth);
    rcTemp.bottom = rcTemp.top + (((rcSrcVis.bottom - rcSrcVis.top) * DestHeight) / SrcHeight);

    return( IntersectRect( &rcTemp2, &rcTemp, &rcDestVis ) );
}


