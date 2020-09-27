// FILE:        stencil.cpp
// DESC:        stencil plane conformance tests
// AUTHOR:      Todd M. Frost

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "flags.h"
#include "stencil.h"

//CD3DWindowFramework App;

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

    CStencil*       pStencil;
    BOOL            bQuit = FALSE, bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 7 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pStencil = new CStencilCap();
                break;
            case 1:
                pStencil = new CStencilClear();
                break;
            case 2:
                pStencil = new CStencilFunc();
                break;
            case 3:
                pStencil = new CStencilIgnore();
                break;
            case 4:
                pStencil = new CStencilOp();
                break;
            case 5:
                pStencil = new CStencilSCorrupt();
                break;
            case 6:
                pStencil = new CStencilZCorrupt();
                break;
        }

        if (!pStencil) {
            return FALSE;
        }

        // Initialize the scene
        if (!pStencil->Create(pDisplay)) {
            pStencil->Release();
            return FALSE;
        }

        bRet = pStencil->Exhibit(pnExitCode);

        bQuit = pStencil->AbortedExit();

        // Clean up the scene
        pStencil->Release();
    }

    return bRet;
}

// NAME:        CStencil()
// DESC:        stencil class constructor
// INPUT:       none
// OUTPUT:      none

CStencil::CStencil()
{
    strcpy(m_szBuffer, "");

    m_dwFailColor = (D3DCOLOR) 0;
    m_dwPassColor = (D3DCOLOR) 0;

    m_dwRedMask = (DWORD) 0;

    m_dwRequestedDepth = (DWORD) 0;

    m_dwStencilBitDepth = (DWORD) 0;
    m_dwStencilBitDepthReported = (DWORD) 0;
    m_dwStencilBitMask = (DWORD) 0;
    m_dwStencilCaps = (DWORD) 0;

    m_uRange = (UINT) 1;

    m_ModeOptions.fReference = false;

    m_bExit = FALSE;
}

// NAME:        ~CStencil()
// DESC:        stencil class destructor
// INPUT:       none
// OUTPUT:      none

CStencil::~CStencil()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CStencil::CommandLineHelp(void)
{
//    WriteCommandLineHelp("$yverbose: $wverbose $c($wOn$c/Off)");
//    WriteCommandLineHelp("$ysubrect: $wsubrect validation $c($wOn$c/Off)");
//    WriteCommandLineHelp("$ydepth: $wstencil depth $c(default=0)");
}

// NAME:        AddModeList()
// DESC:        add mode list
// INPUT:       pMode........address of MODELIST structure
//              pData........address of VOID data
//              dwDataSize...data size
// OUTPUT:      true
// COMMENTS:    for non-conformance, add only modes having appropriate stencil buffers
/*
bool CStencil::AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize)
{
    UINT n;
    PBUFFER pBuffers = NULL;

    vProcessArgs();

    if (IsConformance())
    {
        for (n = 0; n < m_uCommonZBufferFormats; n++)
            if (m_pCommonZBufferFormats[n].ddpfPixelFormat.dwFlags & PF_STENCILBUFFER)
                if (m_dwRequestedDepth)
                {
                    if (m_pCommonZBufferFormats[n].ddpfPixelFormat.dwStencilBitDepth == m_dwRequestedDepth)
                        if (bUnique(&pBuffers, &m_pCommonZBufferFormats[n].ddpfPixelFormat))
                        {
                            pMode->D3DMode.nZBufferFormat = n;
                            CD3DTest::AddModeList(pMode);
                        }
                }
                else
                {
                    if (m_pCommonZBufferFormats[n].ddpfPixelFormat.dwStencilBitDepth >= 1)
                        if (bUnique(&pBuffers, &m_pCommonZBufferFormats[n].ddpfPixelFormat))
                        {
                            pMode->D3DMode.nZBufferFormat = n;
                            CD3DTest::AddModeList(pMode);
                        }
                }
    }
    else
    {
        n = pMode->D3DMode.nZBufferFormat;

        if (m_pCommonZBufferFormats[n].ddpfPixelFormat.dwFlags & PF_STENCILBUFFER)
            if (m_dwRequestedDepth)
            {
                if (m_pCommonZBufferFormats[n].ddpfPixelFormat.dwStencilBitDepth == m_dwRequestedDepth)
                    CD3DTest::AddModeList(pMode);
            }
            else
            {
                if (m_pCommonZBufferFormats[n].ddpfPixelFormat.dwStencilBitDepth >= 1)
                    CD3DTest::AddModeList(pMode);
            }
    }

    if (pBuffers)
    {
        PBUFFER pBuffer, pTemp;

        for (pBuffer = pBuffers; pBuffer; pBuffer = pTemp)
        {
            pTemp = pBuffer->pNext;
            free(pBuffer);
        }
    }

    return true;
}
*/

// NAME:        EmptyModeList()
// DESC:        empty mode list handler
// INPUT:       none
// OUTPUT:      D3DTESTINIT_SKIPALL

UINT CStencil::EmptyModeList()
{
    SetTestRange((UINT) 1, (UINT) 1);
    return (UINT) D3DTESTINIT_SKIPALL;
}

// NAME:        PreModeSetup()
// DESC:        pre mode setup
// INPUT:       none
// OUTPUT:      none

//bool CStencil::PreModeSetup(void)
//{
//    m_pMode->nTextureFormat = DISABLE_TEXTURE;
//    return CD3DTest::PreModeSetup();
//}

// NAME:        SetDefaultRenderStates()
// DESC:        set default render states
// INPUT:       none
// OUTPUT:      SetRenderState()

bool CStencil::SetDefaultRenderStates(VOID)
{
    if (m_dwVersion < 0x0700)
        return true;
    else
        return SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD) FALSE);
}

// NAME:        ClearFrame()
// DESC:        clear frame
// INPUT:       none
// OUTPUT:      true
// COMMENTS:    defeat framework's ClearFrame()

bool CStencil::ClearFrame(VOID)
{
    return true;
}

// NAME:        BeginScene()
// DESC:        begin scene
// INPUT:       dvZ.........clear z
//              dwStencil...clear stencil
//              dwColor.....clear color
//              dwFlags.....clear flags
// OUTPUT:      true.....if clear/begin scene successful
//              false....otherwise

bool CStencil::BeginScene(D3DVALUE dvZ, DWORD dwStencil, DWORD dwColor, DWORD dwFlags)
{
    m_pDevice->SetViewport(&m_vpFull);

    if (dwFlags && !Clear(dwColor, dvZ, dwStencil, dwFlags))
    {
        WriteToLog(_T("Unable to clear z buffer."));
        return false;
    }
    else if (!CD3DTest::BeginScene())
    {
        WriteToLog(_T("Unable to clear z buffer."));
        return false;
    }

    return true;
}

// NAME:        SetTestRange()
// DESC:        set test range
// INPUT:       uStart...start test range
//              uEnd.....end test range
// OUTPUT:      true

bool CStencil::SetTestRange(UINT uStart, UINT uEnd)
{
    if (m_Flags.bCheck(VERBOSE) && (uEnd > 0))
        WriteToLog(_T("Current test range:  (%d,%d)."), uStart, uEnd);

    if (uStart < uEnd)
        m_uRange = uEnd - uStart;
    else
        m_uRange = (UINT) 1;

    return CD3DTest::SetTestRange(uStart, uEnd);
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      D3DTESTINIT_RUN
// COMMENTS:    pixel format bit depths can be unreliable, compute from bit masks

UINT CStencil::TestInitialize(VOID)
{
/*
    bool bMatch = false;
//    DDPIXELFORMAT Format;
    UINT i, j, uCount;

    i = m_pMode->nSrcDevice;
    j = m_pMode->nZBufferFormat;
    m_dwZCmpCaps = m_pAdapter->Devices[i].Desc.dpcTriCaps.dwZCmpCaps;
    memcpy(&Format, &m_pCommonZBufferFormats[j].ddpfPixelFormat, sizeof(Format));
    m_dwStencilBitDepthReported = Format.dwStencilBitDepth;
    m_dwStencilBitDepth = (DWORD) ((m_dwVersion > 0x0700) ? m_dwStencilBitDepthReported : 0);

    if (!(m_dwVersion > 0x0700))
    {
        uCount = Format.dwStencilBitMask;

        while (0 != uCount)
        {
            m_dwStencilBitDepth += (uCount & 1);
            uCount >>= 1;
        }
    }

    m_dwStencilBitMask = (DWORD) ((1 << m_dwStencilBitDepth) - 1);
    m_dwStencilCaps = m_pAdapter->Devices[i].Desc.dwStencilCaps;
    SetDisplayMode(DISPLAYMODE_WINDOW_SRCONLY);

    if (m_dwVersion > 0x0700)
        WriteToLog(_T("Stencil/Z depth:  %d/%d."), Format.dwStencilBitDepth, Format.dwZBufferBitDepth);
    else
        WriteToLog(_T("Stencil/Z masks:  0x%08X/0x%08X."), Format.dwStencilBitMask, Format.dwZBitMask);

    if (m_Flags.bCheck(VERBOSE) && m_pSrcTarget && m_pSrcTarget->m_pSurface4)
    {
        DDSCAPS2 Caps;
        LPDIRECTDRAWSURFACE4 lpSurface;

        memset(&Caps, 0, sizeof(Caps));
        Caps.dwCaps = DDSCAPS_ZBUFFER;

        if (DD_OK == m_pSrcTarget->m_pSurface4->GetAttachedSurface(&Caps, &lpSurface))
        {
            DDSURFACEDESC2 Desc;

            memset(&Desc, 0, sizeof(Desc));
            Desc.dwSize = sizeof(Desc);

            if (DD_OK == lpSurface->GetSurfaceDesc(&Desc))
            {
                if (Desc.ddpfPixelFormat.dwStencilBitDepth == m_dwStencilBitDepth)
                {
                    HRESULT hResult;

                    memset(&Desc, 0, sizeof(Desc));
                    Desc.dwSize = sizeof(Desc);
                    while ((hResult = lpSurface->Lock(NULL, &Desc, 0, NULL)) == DDERR_WASSTILLDRAWING);

                    if (DD_OK == hResult)
                    {
                        lpSurface->Unlock(NULL);

                        if (Desc.ddpfPixelFormat.dwStencilBitDepth == m_dwStencilBitDepth)
                            bMatch = true;
                        else
                            WriteToLog(_T("Stencil depth %d does not match expected depth %d (Lock)."),
                                       Desc.ddpfPixelFormat.dwStencilBitDepth, m_dwStencilBitDepth);
                    }
                    else
                        WriteToLog(_T("Lock() failed on z buffer."));
                }
                else
                    WriteToLog(_T("Stencil depth %d does not match expected depth %d (GetSurfaceDesc)."),
                               Desc.ddpfPixelFormat.dwStencilBitDepth, m_dwStencilBitDepth);
            }
            else
                WriteToLog(_T("GetSurfaceDesc() failed on z buffer."));
        }
        else
            WriteToLog(_T("GetAttachedSurface() failed on render target."));
    }
*/

    m_dwStencilCaps = m_d3dcaps.StencilCaps;

    m_dwRequestedDepth = 8;
    m_dwStencilBitDepth = 8;
    m_dwStencilBitDepthReported = 8;
    m_dwStencilBitMask = 0x000000FF;

    WriteToLog(_T("Stencil/Z depth:  %d/%d."), 8, 24);
    WriteToLog(_T("Stencil/Z masks:  0x%08X/0x%08X."), 0x000000FF, 0xFFFFFF00);

    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true
// COMMENTS:    fail color always has max red component (255)
//              pass color always has min red component (  0)

bool CStencil::ExecuteTest(UINT uTest)
{
    D3DVALUE dvDelta = 0.0f;
    DWORD dwColor[3];
    VOID *pvBuffer;
    CSurface8* pd3ds;
    D3DSURFACE_DESC d3dsd;
    ARGBPIXELDESC pixd;

    if (m_uRange > 1)
        dvDelta = ((D3DVALUE) (uTest - 1))/((D3DVALUE) (m_uRange - 1));
 
    dwColor[0] = (DWORD) 255;
    dwColor[1] = (DWORD) (255.0*(1.0f - dvDelta) + 0.5);
    dwColor[2] = (DWORD) (255.0*(0.0f + dvDelta) + 0.5);

    dwColor[0] = (dwColor[0] > 255) ? 255 : dwColor[0];
    dwColor[1] = (dwColor[1] > 255) ? 255 : dwColor[1];
    dwColor[2] = (dwColor[2] > 255) ? 255 : dwColor[2];

    m_dwFailColor = RGB_MAKE(dwColor[0], dwColor[1], dwColor[2]);
    m_dwPassColor = 0x00ffffff & ~m_dwFailColor;

    // bit masks valid between lock/unlock
/*
    if (!(pvBuffer = m_pSrcTarget->Lock()))
    {
        WriteToLog(_T("Unable to lock source target."));
        return false;
    }

    m_dwRedMask = m_pSrcTarget->m_dwRedMask << m_pSrcTarget->m_nRedShift;

    if (!m_pSrcTarget->Unlock())
    {
        WriteToLog(_T("Unable to unlock source target."));
        return false;
    }
*/

    if (FAILED(m_pDevice->GetRenderTarget(&pd3ds))) {
        return false;
    }

    pd3ds->GetDesc(&d3dsd);
    pd3ds->Release();

    GetARGBPixelDesc(d3dsd.Format, &pixd);

    m_dwRedMask = pixd.dwRMask;

    return true;
}

// NAME:        CompareImages()
// DESC:        compare images
// INPUT:       pSrc.....address of CDirectDrawSurface source
//              pRef.....address of CDirectDrawSurface reference
//              pResult..address of CDirectDrawSurface result
// OUTPUT:      true
// COMMENTS:    defeat framework's CompareImages()
/*
bool CStencil::CompareImages(CDirectDrawSurface *pSrc, CDirectDrawSurface *pRef, CDirectDrawSurface *pResult)
{
    return true;
}
*/
// NAME:        ProcessFrame()
// DESC:        process frame
// INPUT:       none
// OUTPUT:      true

bool CStencil::ProcessFrame(VOID)
{
    return true;
}

// NAME:        bCompare()
// DESC:        compare target to key
// INPUT:       none
// OUTPUT:      true.........if extract successful
//              false........otherwise

bool CStencil::bCompare(VOID)
{
    int i, j;
    DWORD dwPixel;
    DWORD dwFailed = (DWORD) 0, dwInspected = (DWORD) 0, dwPassed = (DWORD) 0;
    INT nMin[2], nMax[2];
    VOID *pvBuffer, *pvColor;
    CSurface8* pd3ds;
    D3DLOCKED_RECT d3dlr;
    D3DSURFACE_DESC d3dsd;
    ARGBPIXELDESC pixd;
    DWORD dwFilter;

//    if (!m_pSrcTarget)
//    {
//        WriteToLog(_T("Unable to analyze:  invalid target."));
//        return false;
//    }

    if (FAILED(m_pDevice->GetRenderTarget(&pd3ds))) {
        WriteToLog(_T("Unable to analyze:  invalid target."));
        return false;
    }

    pd3ds->GetDesc(&d3dsd);
    GetARGBPixelDesc(d3dsd.Format, &pixd);
    dwFilter = pixd.dwRMask | pixd.dwGMask | pixd.dwBMask;

//    if (!(pvBuffer = m_pSrcTarget->Lock()))
//    {
//        WriteToLog(_T("Unable to attempt extraction:  lock failed."));
//        return false;
//    }

    if (FAILED(pd3ds->LockRect(&d3dlr, NULL, 0))) {
        WriteToLog(_T("Unable to attempt extraction:  lock failed."));
        pd3ds->Release();
        return false;
    }
    pvBuffer = d3dlr.pBits;

    vGetRange(nMin, nMax);

    for (j = nMin[1]; j <= nMax[1]; j++)
    {
        pvColor = (char *) pvBuffer + (j*d3dlr.Pitch);

        for (i = 0; i < (int) d3dsd.Width; i++)
        {
//            dwPixel = m_pSrcTarget->ReadPixel(&pvColor);

            if (FormatToBitDepth(d3dsd.Format) == 16) {
                dwPixel = *(LPWORD)pvColor;
                dwPixel &= dwFilter;
                pvColor = ((LPWORD)pvColor) + 1;
            }
            else {
                dwPixel = *(LPDWORD)pvColor;
                dwPixel &= dwFilter;
                pvColor = ((LPDWORD)pvColor) + 1;
            }

            if ((nMin[0] <= i) && (i <= nMax[0]))
            {
                dwInspected++;

                if (!(dwPixel & m_dwRedMask))
                    dwPassed++;
                else
                    dwFailed++;
            }
        }
    }

//    if (!m_pSrcTarget->Unlock())
    if (FAILED(pd3ds->UnlockRect()))
    {
        WriteToLog(_T("Unable to terminate extraction:  unlock failed."));
        pd3ds->Release();
        return false;
    }

    pd3ds->Release();

    if (!dwInspected || (dwInspected != dwPassed))
    {
        if (!dwInspected)
            m_fPassPercentage = 0.0f;
        else
            m_fPassPercentage = ((float) dwPassed) / ((float) dwInspected);

        return false;
    }
    else
    {
        m_fPassPercentage = 1.0;
        return true;
    }
}

// NAME:        bSupportedOperation()
// DESC:        supported stencil operation indicator
// INPUT:       dwOp.....stencil operation
// OUTPUT:      true.....if stencil operation is supported
//              false....otherwise

bool CStencil::bSupportedOperation(DWORD dwOp)
{
    bool bSupported = false;

    switch (dwOp)
    {
        case D3DSTENCILOP_KEEP:
            if (D3DSTENCILCAPS_KEEP & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_ZERO:
            if (D3DSTENCILCAPS_ZERO & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_REPLACE:
            if (D3DSTENCILCAPS_REPLACE & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_INCRSAT:
            if (D3DSTENCILCAPS_INCRSAT & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_DECRSAT:
            if (D3DSTENCILCAPS_DECRSAT & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_INVERT:
            if (D3DSTENCILCAPS_INVERT & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_INCR:
            if (D3DSTENCILCAPS_INCR & m_dwStencilCaps)
                bSupported = true;
        break;

        case D3DSTENCILOP_DECR:
            if (D3DSTENCILCAPS_DECR & m_dwStencilCaps)
                bSupported = true;
        break;

        default:
        break;
    }

    return bSupported;
}

// NAME:        bUnique()
// DESC:        unique z/stencil mask check
// INPUT:       pBuffers.buffer list
//              pFormat..pixel format
// OUTPUT:      true.....if pixel format z/stencil masks not contained in pBuffers
//              false....otherwise

bool CStencil::bUnique(PBUFFER *pBuffers)
{
    bool bFound = false;
    PBUFFER pBuffer;

    return true;
/*
    if (m_dwVersion > 0x0700)
        return true;

    if (m_Flags.bCheck(LOMASK) && (pFormat->dwStencilBitMask & 0x00000001))
        return false;

    if (m_Flags.bCheck(HIMASK) && !(pFormat->dwStencilBitMask & 0x00000001))
        return false;

    for (pBuffer = *pBuffers; pBuffer && !bFound; pBuffer = pBuffer->pNext)
        if ((pFormat->dwStencilBitMask == pBuffer->dwStencilBitMask) &&
            (pFormat->dwZBitMask == pBuffer->dwZBitMask))
            bFound = true;

    if (!bFound)
    {
        pBuffer = (PBUFFER) malloc(sizeof(BUFFER));
        pBuffer->dwStencilBitMask = pFormat->dwStencilBitMask;
        pBuffer->dwZBitMask = pFormat->dwZBitMask;
        pBuffer->pNext = *pBuffers;
        *pBuffers = pBuffer;
    }

    return !bFound;
*/
}

// NAME:        vGetRange()
// DESC:        get compare range
// INPUT:       nMin.....range min
//              nMax.....range max
// OUTPUT:      none

VOID CStencil::vGetRange(INT nMin[2], INT nMax[2])
{
    CSurface8* pd3ds;

    if (m_Flags.bCheck(SUBRECT))
    {
        D3DVALUE dvMid[2];

        dvMid[0] = (D3DVALUE) (m_pDisplay->GetWidth() >> 1);
        dvMid[1] = (D3DVALUE) (m_pDisplay->GetHeight() >> 1);
        nMin[0] = (INT) (dvMid[0] - HALF_SUBRECT_WIDTH);
        nMin[1] = (INT) (dvMid[1] - HALF_SUBRECT_HEIGHT);
        nMax[0] = (INT) (dvMid[0] + HALF_SUBRECT_WIDTH);
        nMax[1] = (INT) (dvMid[1] + HALF_SUBRECT_HEIGHT);
    }
    else
    {
        nMin[0] = (DWORD) 0;
        nMin[1] = (DWORD) 0;
        nMax[0] = (DWORD) m_pDisplay->GetWidth() - 1;
        nMax[1] = (DWORD) m_pDisplay->GetHeight() - 1;
    }
}

// NAME:        vFlood()
// DESC:        flood rendering target
// INPUT:       dvDepth......flood depth
//              dwColor......flood color
// OUTPUT:      none

VOID CStencil::vFlood(D3DVALUE dvDepth, D3DCOLOR dwColor)
{
    D3DCOLOR dwSpecular = RGB_MAKE(0, 0, 0);
    D3DPRIMITIVETYPE dwType = D3DPT_TRIANGLEFAN;
    D3DTLVERTEX Vertices[4];
    DWORD dwCount = 4;
    DWORD dwFlags = (DWORD) CD3D_SRC;
    DWORD dwFormat = D3DFVF_TLVERTEX;
    INT nMin[2], nMax[2];

    vGetRange(nMin, nMax);

    Vertices[0].sx = (D3DVALUE) nMin[0];
    Vertices[0].sy = (D3DVALUE) nMax[1] + 1;
    Vertices[0].sz = dvDepth;
    Vertices[0].rhw = 1.0f;
    Vertices[0].color = dwColor;
    Vertices[0].specular = dwSpecular;
    Vertices[0].tu = 0.0f;
    Vertices[0].tv = 0.0f;

    Vertices[1].sx = (D3DVALUE) nMin[0];
    Vertices[1].sy = (D3DVALUE) nMin[1];
    Vertices[1].sz = dvDepth;
    Vertices[1].rhw = 1.0f;
    Vertices[1].color = dwColor;
    Vertices[1].specular = dwSpecular;
    Vertices[1].tu = 0.0f;
    Vertices[1].tv = 1.0f;

    Vertices[2].sx = (D3DVALUE) nMax[0] + 1;
    Vertices[2].sy = (D3DVALUE) nMin[1];
    Vertices[2].sz = dvDepth;
    Vertices[2].rhw = 1.0f;
    Vertices[2].color = dwColor;
    Vertices[2].specular = dwSpecular;
    Vertices[2].tu = 1.0f;
    Vertices[2].tv = 1.0f;

    Vertices[3].sx = (D3DVALUE) nMax[0] + 1;
    Vertices[3].sy = (D3DVALUE) nMax[1] + 1;
    Vertices[3].sz = dvDepth;
    Vertices[3].rhw = 1.0f;
    Vertices[3].color = dwColor;
    Vertices[3].specular = dwSpecular;
    Vertices[3].tu = 1.0f;
    Vertices[3].tv = 0.0f;

    RenderPrimitive(dwType, dwFormat, (LPVOID) Vertices, dwCount, NULL, 0, dwFlags);
}

// NAME:        vStipple()
// DESC:        stipple enable/disable
// INPUT:       bEnable..enable flag
// OUTPUT:      none

VOID CStencil::vStipple(bool bEnable)
{
    D3DRENDERSTATETYPE dwType;
    DWORD dwMask, dwState; 
    UINT i;

    // all stipple renderstates retired post DX6
    return;

/*
    if (m_dwVersion > 0x0600)
        return;

    if (bEnable)
    {
        dwMask = 0xffffffff;
        dwState = (DWORD) TRUE;
    }
    else
    {
        dwMask = 0x00000000;
        dwState = (DWORD) FALSE;
    }

    SetRenderState(D3DRENDERSTATE_STIPPLEENABLE, dwState);

    for (i = 0; i < 32; i++)
    {
        dwType = (D3DRENDERSTATETYPE) D3DRENDERSTATE_STIPPLEPATTERN(i);
        SetRenderState(dwType, dwMask);
    }
*/
}

// NAME:        vProcessArgs()
// DESC:        process args
// INPUT:       none
// OUTPUT:      none

VOID CStencil::vProcessArgs(VOID)
{
    int iDefault, iResult;

    if (KeySet("WHQL"))
    {
        m_dwRequestedDepth = (DWORD) 0;
        return;
    }

    // verbose
    // default:  off

    if (KeySet("verbose"))
        m_Flags.vSet(VERBOSE);

    // himask
    // default:  off

    if (KeySet("himask"))
        m_Flags.vSet(HIMASK);

    // lomask
    // default:  off

    if (KeySet("lomask"))
        m_Flags.vSet(LOMASK);

    // subrect
    // default:  off

    if (KeySet("subrect"))
        m_Flags.vSet(SUBRECT);

    // depth
    // default:  0 (no specified depth)

    iDefault = (int) 0;
    ReadInteger("depth", iDefault, &iResult);
    m_dwRequestedDepth = (DWORD) ((iResult < 0) ? 0 : iResult);
}

//******************************************************************************
BOOL CStencil::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CStencil::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CStencil::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
