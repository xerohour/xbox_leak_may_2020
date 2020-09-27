// FILE:        lightmap.cpp
// DESC:        lightmap class methods for D3DTOP_DOTPRODUCT3 conformance test
// AUTHOR:      Todd M. Frost

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "flags.h"
#include "lightmap.h"
#include "global.h"

typedef enum _COORDINDEX
{
    CIX = 0,
    CIY = 1,
    CIZ = 2
} COORDINDEX;

const DWORD SCALE_TO_WINDOW = 0x00000001; // scale to window
const DWORD VALIDATE        = 0x00000002; // validate src/ref devices

const UINT TEXTURE_WIDTH  = 256;
const UINT TEXTURE_HEIGHT = 256;

const FLOAT LEMNISCATE_SCALE = 0.4f; // lemniscate scale

const FLOAT ZERO = 1.0e-8f; // floating point zero

const FLOAT RADIUS         = 0.5f;  // sphere radius
const FLOAT RADIUS_SQUARED = 0.25f; // sphere radius squared

// factor for lemniscate formula (r = sin(factor*theta))

const UINT DEFAULT_FACTOR = 2; // default factor
const UINT MIN_FACTOR     = 1; // min     factor
const UINT MAX_FACTOR     = 8; // max     factor

// intervals for lemniscate traversal

const UINT DEFAULT_INTERVALS = 8;   // default intervals
const UINT MIN_INTERVALS     = 0;   // min     intervals
const UINT MAX_INTERVALS     = 256; // min     intervals

typedef struct FlexVert {
    D3DVALUE fx;
    D3DVALUE fy;
    D3DVALUE fz;
    D3DVALUE frhw;
    D3DCOLOR dwDiffuse;
    D3DVALUE fu;
    D3DVALUE fv;
    D3DVALUE fu2;
    D3DVALUE fv2;
} FLEXVERT, *PFLEXVERT;

bool gbSouthern = false;

// NAME:        fGetVectorCoord()
// DESC:        get vector coord from hemisphere vector field
// INPUT:       ci...coord index
//              u....u texture coord
//              v....v texture coord
// OUTPUT:      fVector[coord]
// COMMENTS:    vector field based on sphere equation:
//                  x**2 + y**2 + z**2 = r**2
//              where
//                  x = u - r
//                  y = v - r
//                  z = sqrt(r**2 - x**2 - y**2)
//                  z = 0 if x**2 + y**2 > r**2 

FLOAT fGetVectorCoord(COORDINDEX ci, FLOAT u, FLOAT v)
{
    double fTemp;
    double fVector[3];

    fVector[CIX] = u - 0.5;
    fVector[CIY] = v - 0.5;
    fVector[CIZ] = 0.0;

    fTemp = fVector[CIX]*fVector[CIX] + fVector[CIY]*fVector[CIY];

    if (fTemp < ZERO)
    {
        fVector[CIX] = 0.0;
        fVector[CIY] = 0.0;
        fVector[CIZ] = RADIUS;
    }
    else if (fTemp < RADIUS_SQUARED)
        fVector[CIZ] = sqrt(RADIUS_SQUARED - fTemp);
    
    if (gbSouthern)
        fVector[CIZ] = -fVector[CIZ];

    fTemp = sqrt(fTemp + fVector[CIZ]*fVector[CIZ]);

    return (FLOAT) fVector[ci]/(float)fTemp;
}

// NAME:        fVectorX()
// DESC:        vector field x coord 
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      fGetVectorCoord(CIX, u, v)
// COMMENTS:    [-1,1] maps to [0,255] in LoadChannelData() (CIL_UNSIGNED)

FLOAT fVectorX(FLOAT u, FLOAT v)
{
    return fGetVectorCoord(CIX, u, v);
}

// NAME:        fVectorY()
// DESC:        vector field y coord
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      fGetVectorCoord(CIY, u, v)
// COMMENTS:    [-1,1] maps to [0,255] in LoadChannelData() (CIL_UNSIGNED)

FLOAT fVectorY(FLOAT u, FLOAT v)
{
    return fGetVectorCoord(CIY, u, v);
}

// NAME:        fVectorZ()
// DESC:        vector field z coord
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      fGetVectorCoord(CIZ, u, v)
// COMMENTS:    [-1,1] maps to [0,255] in LoadChannelData() (CIL_UNSIGNED)

FLOAT fVectorZ(FLOAT u, FLOAT v)
{
    return fGetVectorCoord(CIZ, u, v);
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

    CLightmap*      pLightmap;
    BOOL            bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pLightmap = new CLightmap();
    if (!pLightmap) {
        return FALSE;
    }

    // Initialize the scene
    if (!pLightmap->Create(pDisplay)) {
        pLightmap->Release();
        return FALSE;
    }

    bRet = pLightmap->Exhibit(pnExitCode);

    // Clean up the scene
    pLightmap->Release();

    return bRet;
}

// NAME:        CLightmap()
// DESC:        constructor for lightmap class
// INPUT:       none
// OUTPUT:      none

CLightmap::CLightmap(VOID)
{
    m_szTestName=TEXT("DOTPRODUCT3");
    m_szCommandKey=TEXT("Lightmap");

    m_fFactor = 0.0f;

    m_dwFormat = NOFORMAT;

    m_dwIntervals = (DWORD) 0;
    m_dwStages = (DWORD) 1;

//    m_pNorthernImage = NULL;
//    m_pSouthernImage = NULL;

    m_pNorthernTexture = NULL;
    m_pSouthernTexture = NULL;
}

// NAME:        ~CLightmap()
// DESC:        destructor for lightmap class
// INPUT:       none
// OUTPUT:      none

CLightmap::~CLightmap(VOID)
{
    ReleaseTexture(m_pNorthernTexture);
    ReleaseTexture(m_pSouthernTexture);

//    RELEASE(m_pNorthernImage);
//    RELEASE(m_pSouthernImage);
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CLightmap::CommandLineHelp(void)
{
//    WriteCommandLineHelp("$yfactor: $wlemniscate = sin(factor*theta) $c(default=2)");
//    WriteCommandLineHelp("$yintervals: $wintervals per test $c(default=8)");
//    WriteCommandLineHelp("$yscale:  $wscale to window $c($wOn$c/Off)");
//    WriteCommandLineHelp("$yvalidate: $wvalidate $c(On/$wOff$c)");
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CLightmap::TestInitialize(VOID)
{
    bool bFound;
//    int nDevice = m_pMode->nSrcDevice;
//    CImageLoader Loader;
    DWORD dwWidth = (DWORD) TEXTURE_WIDTH, dwHeight = (DWORD) TEXTURE_HEIGHT;
    WORD wMaxSimultaneousTextures;

    m_dwFormat = NOFORMAT;

    vProcessArgs();

//    CHANNELDATA Data;
    Data.dwAlpha = (DWORD) 0;
    Data.dwRed = (DWORD) CIL_UNSIGNED;
    Data.dwGreen = (DWORD) CIL_UNSIGNED;
    Data.dwBlue = (DWORD) CIL_UNSIGNED;
    Data.fAlpha = NULL;
    Data.fRed = fVectorX;
    Data.fGreen = fVectorY;
    Data.fBlue = fVectorZ;
//    gbSouthern = false;

//    if (!(m_pNorthernImage = Loader.LoadChannelData(dwWidth, dwHeight, &Data)))
//    {
//        WriteToLog(_T("Unable to load northern image."));
//        return D3DTESTINIT_ABORT;
//    }

//    gbSouthern = true;

//    if (!(m_pSouthernImage = Loader.LoadChannelData(dwWidth, dwHeight, &Data)))
//    {
//        WriteToLog(_T("Unable to load southern image."));
//        return D3DTESTINIT_ABORT;
//    }

    bFound = true;

/*
    if (!RequestTextureFormats((DWORD) PF_RGB))
        bFound = false;
    else if (!RequestTextureFormats((DWORD) PF_RGB, (DWORD) PF_PALETTEINDEXED1))
        bFound = false;
    else if (!RequestTextureFormats((DWORD) PF_RGB, (DWORD) PF_PALETTEINDEXED2))
        bFound = false;
    else if (!RequestTextureFormats((DWORD) PF_RGB, (DWORD) PF_PALETTEINDEXED4))
        bFound = false;
    else if (!RequestTextureFormats((DWORD) PF_RGB, (DWORD) PF_PALETTEINDEXED8))
        bFound = false;
    else if (!RequestTextureFormats((DWORD) PF_RGB, (DWORD) PF_PALETTEINDEXEDTO8))
        bFound = false;

    if (!bFound || (0 == m_uCommonTextureFormats))
    {
        WriteToLog(_T("No non-palettized RGB texture formats enumerated."));
        SetTestRange((UINT) 1, (UINT) 1);
        return D3DTESTINIT_SKIPALL;
    }
*/
    m_dwStages = (m_d3dcaps.MaxTextureBlendStages < 2) ? 1 : 2;
    wMaxSimultaneousTextures =  (WORD)m_d3dcaps.MaxSimultaneousTextures;
    m_dwStages = (DWORD) ((wMaxSimultaneousTextures < 2) ? 1 : m_dwStages);
    SetTestRange((UINT) 1, (UINT) (m_dwStages*m_uCommonTextureFormats*(m_dwIntervals + 1)));

    if (!(m_d3dcaps.TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3))
    {
        WriteToLog(_T("Device does NOT support D3DTOP_DOTPRODUCT3."));
        return D3DTESTINIT_SKIPALL;
    }

    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise
// COMMENTS:    diffuse color used as normal for D3DTOP_DOTPRODUCT3
//              note:   [  0,127] maps to [-1.0, 0.0]
//                      [128,255] maps to [ 0.0, 1.0]

bool CLightmap::ExecuteTest(UINT uTest)
{
    bool bFound = false, bResult = true, bValid = true;
    int iStage = (DWORD) 0;
    CHAR szStages[NSTAGES][34] = {
        "one stage, blend(diffuse:texture)",
        "two stage, blend(texture:texture)"
    };
    DOUBLE fRadius, fTheta = 0.0;
    DOUBLE pfOffset[2];
    DWORD dwFormat, dwStages;
    UINT i, j, k, uPoint, uTemp = (UINT) 1;

    for (i = 1; (i <= m_dwStages) && !bFound; i++)
        for (j = 0; (j < m_uCommonTextureFormats) && !bFound; j++)
            for (k = 0; (k <= m_dwIntervals) && !bFound; k++, uTemp++)
                if (uTemp == uTest)
                {
                    dwStages = i;
                    dwFormat = j;
                    uPoint = k;
                    bFound = true;
                }

    if (!bFound)
        return false;

    // compute texture coord offset from scaled lemniscate:  radius = sin(2*theta)

    if (m_dwIntervals > 0)
    {
        fTheta = (2.0f*pi*uPoint)/((DOUBLE) m_dwIntervals);
        fTheta = (fTheta > 2.0f*pi) ? (fTheta - 2.0f*pi) : fTheta;
    }

    fRadius = LEMNISCATE_SCALE*sin(m_fFactor*fTheta);
    pfOffset[0] = -fRadius*cos(fTheta);
    pfOffset[1] = -fRadius*sin(fTheta);

    if (dwFormat != m_dwFormat)
    {
//        DWORD dwFlags = CDDS_TEXTURE_VIDEO;
        DWORD dwWidth = (DWORD) TEXTURE_WIDTH, dwHeight = (DWORD) TEXTURE_HEIGHT;

        ReleaseTexture(m_pNorthernTexture);
        m_pNorthernTexture = NULL;
        ReleaseTexture(m_pSouthernTexture);
        m_pSouthernTexture = NULL;

        m_dwFormat = dwFormat; 
//        m_pMode->nTextureFormat = m_dwFormat;

        gbSouthern = false;

//        if (!(m_pNorthernTexture = CreateTexture(dwWidth, dwHeight, dwFlags, m_pNorthernImage)))
        if (!(m_pNorthernTexture = (CTexture8*)CreateChannelDataTexture(m_pDevice, dwWidth, dwHeight, &Data, m_fmtCommon[m_dwFormat])))
        {
            WriteToLog(_T("Unable to create northern texture."));
            return false;
        }

        gbSouthern = true;

//        if (!(m_pSouthernTexture = CreateTexture(dwWidth, dwHeight, dwFlags, m_pSouthernImage)))
        if (!(m_pSouthernTexture = (CTexture8*)CreateChannelDataTexture(m_pDevice, dwWidth, dwHeight, &Data, m_fmtCommon[m_dwFormat])))
        {
            WriteToLog(_T("Unable to create southern texture."));
            return false;
        }
    }

//    SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD) TRUE);

    SetTextureStageState(iStage, D3DTSS_ADDRESSU, (DWORD) D3DTADDRESS_CLAMP);
    SetTextureStageState(iStage, D3DTSS_ADDRESSV, (DWORD) D3DTADDRESS_CLAMP);
    SetTextureStageState(iStage, D3DTSS_MAGFILTER, (DWORD) TEXF_LINEAR);
    SetTextureStageState(iStage, D3DTSS_MINFILTER, (DWORD) TEXF_LINEAR);
    SetTextureStageState(iStage, D3DTSS_MIPFILTER, (DWORD) TEXF_NONE);

    if (dwStages > 1)
        SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_SELECTARG1);
    else
        SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_DOTPRODUCT3);

    SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_COLORARG2, (DWORD) D3DTA_DIFFUSE);
    SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_DIFFUSE);
    SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 0);
    SetTexture(iStage, m_pNorthernTexture);
    iStage++;

    if (dwStages > 1)
    {
        SetTextureStageState(iStage, D3DTSS_ADDRESSU, (DWORD) D3DTADDRESS_WRAP);
        SetTextureStageState(iStage, D3DTSS_ADDRESSV, (DWORD) D3DTADDRESS_WRAP);
        SetTextureStageState(iStage, D3DTSS_MAGFILTER, (DWORD) TEXF_LINEAR);
        SetTextureStageState(iStage, D3DTSS_MINFILTER, (DWORD) TEXF_LINEAR);
        SetTextureStageState(iStage, D3DTSS_MIPFILTER, (DWORD) TEXF_NONE);
        SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_DOTPRODUCT3);
        SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
        SetTextureStageState(iStage, D3DTSS_COLORARG2, (DWORD) D3DTA_CURRENT);
        SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
        SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
        SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_CURRENT);
        SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 1);
        SetTexture(iStage, m_pSouthernTexture);
        iStage++;
    }
    
    SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
    SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);

#ifndef UNDER_XBOX

    if (m_Flags.bCheck(VALIDATE))
    {
        DWORD dwSrc, dwRef;
        HRESULT hSrc, hRef;

        if (!ValidateDevice(&hSrc, &dwSrc, &hRef, &dwRef))
        {
            if (D3D_OK != hSrc)
            {
                WriteToLog(_T("Src requires %d passes:  %s."), dwSrc, hSrc);
                bValid = false;
            }

            if (D3D_OK != hRef)
            {
                WriteToLog(_T("Ref requires %d passes:  %s."), dwRef, hRef);
                bValid = false;
            }
        }
    }

#endif // !UNDER_XBOX

    if (!bValid)
    {
        SkipTests((UINT) 1);
        return true;
    }

    sprintf(m_szBuffer, "%s", szStages[dwStages - 1]);
    BeginTestCase(m_szBuffer);

    SwitchAPI((UINT)m_fFrame);

    m_pDevice->SetViewport(&m_vpFull);
    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, RGB_MAKE(74, 150, 202), 1.0f, 0);

    m_pDevice->SetViewport(&m_vpTest);

    ClearFrame();

    if (BeginScene())
    {
        D3DPRIMITIVETYPE dwType = D3DPT_TRIANGLEFAN;
        DWORD dwWidth = (DWORD) TEXTURE_WIDTH, dwHeight = (DWORD) TEXTURE_HEIGHT;
        DWORD dwFormat = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2;
        FLEXVERT Vertices[4];

//        if (m_Flags.bCheck(SCALE_TO_WINDOW))
//        {
            dwWidth = m_vpTest.Width;
            dwHeight = m_vpTest.Height;
//        }

        Vertices[0].fx = (float)m_vpTest.X;
        Vertices[0].fy = (float)m_vpTest.Y + (D3DVALUE) dwHeight;
        Vertices[0].fz = (D3DVALUE) 0.5;
        Vertices[0].frhw = 1.0f;
        Vertices[0].dwDiffuse = RGB_MAKE(128, 128, 255);
        Vertices[0].fu = (D3DVALUE) (0.0 + pfOffset[0]);
        Vertices[0].fv = (D3DVALUE) (0.0 + pfOffset[1]);
        Vertices[0].fu2 = 0.0f;
        Vertices[0].fv2 = 0.0f;

        Vertices[1].fx = (float)m_vpTest.X;
        Vertices[1].fy = (float)m_vpTest.Y;
        Vertices[1].fz = (D3DVALUE) 0.5;
        Vertices[1].frhw = 1.0f;
        Vertices[1].dwDiffuse = RGB_MAKE(128, 128, 255);
        Vertices[1].fu = (D3DVALUE) (0.0 + pfOffset[0]);
        Vertices[1].fv = (D3DVALUE) (1.0 + pfOffset[1]);
        Vertices[1].fu2 = 0.0f;
        Vertices[1].fv2 = 1.0f;

        Vertices[2].fx = (float)m_vpTest.X + (D3DVALUE) dwWidth;
        Vertices[2].fy = (float)m_vpTest.Y;
        Vertices[2].fz = (D3DVALUE) 0.5;
        Vertices[2].frhw = 1.0f;
        Vertices[2].dwDiffuse = RGB_MAKE(128, 128, 255);
        Vertices[2].fu = (D3DVALUE) (1.0 + pfOffset[0]);
        Vertices[2].fv = (D3DVALUE) (1.0 + pfOffset[1]);
        Vertices[2].fu2 = 1.0f;
        Vertices[2].fv2 = 1.0f;

        Vertices[3].fx = (float)m_vpTest.X + (D3DVALUE) dwWidth;
        Vertices[3].fy = (float)m_vpTest.Y + (D3DVALUE) dwHeight;
        Vertices[3].fz = (D3DVALUE) 0.5;
        Vertices[3].frhw = 1.0f;
        Vertices[3].dwDiffuse = RGB_MAKE(128, 128, 255);
        Vertices[3].fu = (D3DVALUE) (1.0f + pfOffset[0]);
        Vertices[3].fv = (D3DVALUE) (0.0f + pfOffset[1]);
        Vertices[3].fu2 = 1.0f;
        Vertices[3].fv2 = 0.0f;

        if (!RenderPrimitive(dwType, dwFormat, (LPVOID) Vertices, (DWORD) 4, NULL, 0, (DWORD) 0))
        {
            WriteToLog(_T("RenderPrimitive() failed."));
            bResult = false;
        }

        // Display the adapter mode
        ShowDisplayMode();

        // Display the frame rate
        ShowFrameRate();

        // Display the console
        m_pDisplay->ShowConsole();

        if (!EndScene())
        {
            WriteToLog(_T("EndScene() failed."));
            bResult = false;
        }
    }
    else
    {
        WriteToLog(_T("BeginScene() failed."));
        bResult = false;
    }

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

    sprintf(m_szBuffer, "$y%s", szStages[dwStages - 1]);
    WriteStatus("$gMode", m_szBuffer);
//    return bResult;
    return false;
}

// NAME:        ProcessFrame()
// DESC:        process frame
// INPUT:       none
// OUTPUT:      bResult..result of src:ref image comparison

bool CLightmap::ProcessFrame(void)
{
    bool bResult;
    static int nPass = 0;
    static int nFail = 0;

    bResult = GetCompareResult(0.15f, 0.78f, (int) 0);

    if (bResult)
    {
        (m_fIncrementForward) ? nPass++ : nPass--;
        sprintf(m_szBuffer, "$yPass: %d, Fail: %d", nPass, nFail);
        Pass();
    }
    else
    {
        (m_fIncrementForward) ? nFail++ : nFail--;
        sprintf(m_szBuffer, "$yPass: %d, Fail: %d", nPass, nFail);
        Fail();
    }

    WriteStatus("$gSummary", m_szBuffer);
    return bResult;
}

// NAME:        TestTerminate()
// DESC:        terminate test
// INPUT:       none
// OUTPUT:      true

bool CLightmap::TestTerminate(VOID)
{
    return true;
}

// NAME:        vProcessArgs()
// DESC:        process args
// INPUT:       none
// OUTPUT:      none

VOID CLightmap::vProcessArgs(VOID)
{
    int iDefault, iResult;

    if (KeySet("WHQL"))
    {
        m_Flags.vSet(VALIDATE);
        m_fFactor = (DOUBLE) DEFAULT_FACTOR;
        m_dwIntervals = (DWORD) DEFAULT_INTERVALS;
        return;
    }

    // factor
    // default:  DEFAULT_FACTOR

    iDefault = (int) DEFAULT_FACTOR;
    ReadInteger("factor", iDefault, &iResult);
    m_fFactor = (DOUBLE) ((iResult < MIN_FACTOR) ? MIN_FACTOR : ((iResult >= MAX_FACTOR) ? MAX_FACTOR: iResult));

    // intervals
    // default:  DEFAULT_INTERVALS 

    iDefault = (int) DEFAULT_INTERVALS;
    ReadInteger("intervals", iDefault, &iResult);
    m_dwIntervals = (DWORD) ((iResult < MIN_INTERVALS) ? MIN_INTERVALS : ((iResult >= MAX_INTERVALS) ? MAX_INTERVALS: iResult));

    // scale
    // default:  off

    if (KeySet("scale"))
        m_Flags.vSet(SCALE_TO_WINDOW);

    // validate
    // default:  true

    if (!KeySet("!validate"))
        m_Flags.vSet(VALIDATE);
}
