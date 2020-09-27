// FILE:        emboss.cpp
// DESC:        emboss class methods for emboss conformance tests
// AUTHOR:      Todd M. Frost

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "CShapesFVF.h"
#include "flags.h"
#include "emboss.h"

const UINT MINF = 1; // min factor for lemniscate formula (r = sin(factor*theta))
const UINT MAXF = 8; // max factor for lemniscate formula (r = sin(factor*theta))

const UINT MINI = 0;   // min intervals for lemniscate traversal
const UINT MAXI = 256; // max intervals for lemniscate traversal

const UINT TEXTURE_WIDTH  = 256; // texture width
const UINT TEXTURE_HEIGHT = 256; // texture height

const FLOAT LEMNISCATE_SCALE = 0.1f; // lemniscate scale

//CD3DWindowFramework App; // global instance of D3D window framework class

FLOAT gfConstant = 1.0f; // global helper variable for fConstant()

// NAME:        fConstant()
// DESC:        constant
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      1.0
// COMMENTS:    [-1,1] maps to [0,255] in LoadChannelData() (CIL_UNSIGNED)

FLOAT fConstant(FLOAT u, FLOAT v)
{
    return gfConstant;
}

// NAME:        fHeight()
// DESC:        height
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      1.0..if (u,v) lie inside circle (u - 0.5)**2 + (v - 0.5) = 0.04
//             -1.0..otherwise
// COMMENTS:    [-1,1] maps to [0,255] in LoadChannelData() (CIL_UNSIGNED)

FLOAT fHeight(FLOAT u, FLOAT v)
{
    FLOAT fTemp;
    FLOAT fVector[2];

    fVector[0] = u - 0.5f;
    fVector[1] = v - 0.5f;
    fTemp = fVector[0]*fVector[0] + fVector[1]*fVector[1];

    if (fTemp < 0.04f)
        return 1.0f;
    else
        return -1.0f;
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

    CEmboss*  pEmboss;
    BOOL      bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    pEmboss = new CEmboss();
    if (!pEmboss) {
        return FALSE;
    }

    // Initialize the scene
    if (!pEmboss->Create(pDisplay)) {
        pEmboss->Release();
        return FALSE;
    }

    bRet = pEmboss->Exhibit(pnExitCode);

    // Clean up the scene
    pEmboss->Release();

    return bRet;
}

// NAME:        CEmboss()
// DESC:        constructor for emboss class
// INPUT:       none
// OUTPUT:      none

CEmboss::CEmboss(VOID)
{
    m_szTestName = TEXT("Emboss bump mapping");
    m_szCommandKey = TEXT("Emboss");

    m_fFactor = 0.0;

    m_fOffset[0] = 0.0;
    m_fOffset[1] = 0.0;

    m_dwFormat = NOFORMAT;

    m_dwFVF = (DWORD) 0;

    m_uIntervals = (UINT) 0;

    m_pVertices = NULL;

//    m_pBaseImage = NULL;
//    m_pHeightImage = NULL;

    m_pBaseTexture = NULL;
    m_pHeightTexture = NULL;

//    m_pMaterial = NULL;
}

// NAME:        ~CEmboss()
// DESC:        destructor for emboss class
// INPUT:       none
// OUTPUT:      none

CEmboss::~CEmboss(VOID)
{
    if (m_pVertices)
        free(m_pVertices);

//    RELEASE(m_pMaterial);

    ReleaseTexture(m_pBaseTexture);
//    RELEASE(m_pBaseImage);

    ReleaseTexture(m_pHeightTexture);
//    RELEASE(m_pHeightImage);
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

VOID CEmboss::CommandLineHelp(VOID)
{
//    WriteCommandLineHelp("$yfactor: $wlemniscate factor (r = sin(factor*theta)) $c(default=2)");
//    WriteCommandLineHelp("$yintervals: $wtraversal intervals $c(default=32)");
//    WriteCommandLineHelp("$yvalidate: $wvalidate $c($wOn$c/Off)");
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code
// COMMENTS:    material released before returning D3DTESTINIT_SKIPALL or D3DTESTINIT_ABORT
//              RELEASE(m_pMaterial) causes access violations in TestTerminate otherwise

UINT CEmboss::TestInitialize(VOID)
{
    bool bFound;
    int iDefault, iResult;
    CHANNELDATA Data = { 0 };
    CHAR szBuffer[MAXBUFFER];
//    CImageLoader Loader;
    DWORD dwCaps;
    DWORD dwWidth = (DWORD) TEXTURE_WIDTH, dwHeight = (DWORD) TEXTURE_HEIGHT;
//    INT nDevice = m_pMode->nSrcDevice;
    UINT i, j, k;

    m_dwFormat = NOFORMAT;

    vProcessArgs();

    // check supported texture blend stages 

    if (m_d3dcaps.MaxTextureBlendStages < 2)
    {
        WriteToLog(_T("Device does NOT support sufficient number of texture stages (2)."));
//        RELEASE(m_pMaterial);
        SetTestRange((UINT) 1, (UINT) 1);
        return D3DTESTINIT_SKIPALL;
    }

    // check supported simultaneous textures 

    if (m_d3dcaps.MaxSimultaneousTextures < 2)
    {
        WriteToLog(_T("Device does NOT support sufficient number of simultaneous textures (2)."));
//        RELEASE(m_pMaterial);
        SetTestRange((UINT) 1, (UINT) 1);
        return D3DTESTINIT_SKIPALL;
    }

    // check texture op caps

    dwCaps = m_d3dcaps.TextureOpCaps;

    if (!(dwCaps & D3DTEXOPCAPS_ADDSIGNED))
    {
        WriteToLog(_T("D3DTEXOPCAPS_ADDSIGNED required."));
//        RELEASE(m_pMaterial);
        SetTestRange((UINT) 1, (UINT) 1);
        return D3DTESTINIT_SKIPALL;
    }

    if (!(dwCaps & D3DTEXOPCAPS_MODULATE))
    {
        WriteToLog(_T("D3DTEXOPCAPS_MODULATE required."));
//        RELEASE(m_pMaterial);
        SetTestRange((UINT) 1, (UINT) 1);
        return D3DTESTINIT_SKIPALL;
    }

    // check source blend caps

    dwCaps = m_d3dcaps.SrcBlendCaps;

    if (!(dwCaps & D3DPBLENDCAPS_SRCALPHA))
    {
        WriteToLog(_T("D3DPBLENDCAPS_SRCALPHA blend src alpha required."));
//        RELEASE(m_pMaterial);
        SetTestRange((UINT) 1, (UINT) 1);
        return D3DTESTINIT_SKIPALL;
    }

    // check destination blend caps

    dwCaps = m_d3dcaps.DestBlendCaps;

    if (!(dwCaps & D3DPBLENDCAPS_ZERO))
    {
        WriteToLog(_T("D3DPBLENDCAPS_ZERO blend dest zero required."));
//        RELEASE(m_pMaterial);
        SetTestRange((UINT) 1, (UINT) 1);
        return D3DTESTINIT_SKIPALL;
    }

    // scan for ARGB texture formats

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
    else if (!RequestTextureFormats((DWORD) PF_ALPHAPIXELS))
        bFound = false;

    if (!bFound || (0 == m_uCommonTextureFormats))
    {
        WriteToLog(_T("No non-palettized ARGB texture formats enumerated."));
        RELEASE(m_pMaterial);
        SetTestRange((UINT) 1, (UINT) 1);
        return D3DTESTINIT_SKIPALL;
    }
*/
    // initialize triangle mesh

    if (!m_Shape.NewShape(CS_MESH))
    {
        WriteToLog(_T("Unable to create mesh."));
//        RELEASE(m_pMaterial);
        return D3DTESTINIT_ABORT;
    }

    m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2;
    m_Shape.ConvertToFVF(m_dwFVF);
    m_Shape.ScaleCoords((DWORD) 0, (D3DVALUE) 3.0, (D3DVALUE) 3.0);
    m_Shape.OffsetCoords((DWORD) 0, (D3DVALUE) -1.0, (D3DVALUE) -1.0);
    m_Shape.ScaleCoords((DWORD) 1, (D3DVALUE) 3.0, (D3DVALUE) 3.0);
    m_Shape.OffsetCoords((DWORD) 1, (D3DVALUE) -1.0, (D3DVALUE) -1.0);

    m_pVertices = (PFLEXVERT) malloc(sizeof(FLEXVERT)*m_Shape.m_nVertices);

    if (!m_pVertices)
    {
        WriteToLog(_T("Unable to create non strided data."));
//        RELEASE(m_pMaterial);
        return D3DTESTINIT_ABORT;
    }

    for (i = 0; i < (UINT)m_Shape.m_nVertices; i++)
    {
        m_pVertices[i].fPx = m_Shape.m_pVertices[i].x;
        m_pVertices[i].fPy = m_Shape.m_pVertices[i].y;
        m_pVertices[i].fPz = m_Shape.m_pVertices[i].z;

        m_pVertices[i].fNx = m_Shape.m_pVertices[i].nx;
        m_pVertices[i].fNy = m_Shape.m_pVertices[i].ny;
        m_pVertices[i].fNz = m_Shape.m_pVertices[i].nz;

        m_pVertices[i].fSu = m_Shape.m_TexCoords[i].Pair[0].tu;
        m_pVertices[i].fSv = m_Shape.m_TexCoords[i].Pair[0].tv;

        m_pVertices[i].fTu = m_Shape.m_TexCoords[i].Pair[1].tu;
        m_pVertices[i].fTv = m_Shape.m_TexCoords[i].Pair[1].tv;
    }

    // initialize texture image data

    Data.dwAlpha = (DWORD) 0;
    Data.dwRed   = (DWORD) CIL_UNSIGNED;
    Data.dwGreen = (DWORD) CIL_UNSIGNED;
    Data.dwBlue =  (DWORD) CIL_UNSIGNED;
    Data.fAlpha = fHeight;
    Data.fRed   = fHeight;
    Data.fGreen = fHeight;
    Data.fBlue  = fHeight;

//    if (!(m_pBaseImage = Loader.LoadChannelData(dwWidth, dwHeight, &Data)))
//    {
//        WriteToLog(_T("Unable to load base image."));
//        RELEASE(m_pMaterial);
//        return D3DTESTINIT_ABORT;
//    }
    memcpy(&m_DataBase, &Data, sizeof(CHANNELDATA));

    Data.dwAlpha = (DWORD) 0;
    Data.dwRed   = (DWORD) CIL_UNSIGNED;
    Data.dwGreen = (DWORD) CIL_UNSIGNED;
    Data.dwBlue  = (DWORD) CIL_UNSIGNED;
    Data.fAlpha = fHeight;
    Data.fRed   = fConstant;
    Data.fGreen = fConstant;
    Data.fBlue  = fConstant;

//    if (!(m_pHeightImage = Loader.LoadChannelData(dwWidth, dwHeight, &Data)))
//    {
//        WriteToLog(_T("Unable to load height image."));
//        RELEASE(m_pMaterial);
//        return D3DTESTINIT_ABORT;
//    }
    memcpy(&m_DataHeight, &Data, sizeof(CHANNELDATA));

    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test executed successfully
//              false....otherwise
// COMMENTS:    embossed bump mapping technique from ATI web page (Rage 128)
//
//              render states:
//                  multiply alpha (monochrome emboss) with source color (lighted texture)
//
//              stages:
//                  0: modulate texture:diffuse
//                  1: texture coords (1) are shifted, invert alpha, perform signed add
//
//              result:
//                  (height - shifted_height)*tex.RGB*diffuse.RGB
//
//              compute texture coord offset from scaled lemniscate:  radius = sin(2*theta)

bool CEmboss::ExecuteTest(UINT uTest)
{
    bool bFound = false, bResult = true, bValid = true;
    int iStage = (int) 0;
    CHAR szBuffer[MAXBUFFER];
    DOUBLE fRadius, fTheta;
    DWORD dwFormat;
    UINT i, j, uTemp = (UINT) 1;

    for (i = 0; (i < m_uCommonTextureFormats) && !bFound; i++)
        for (j = 0; (j <= m_uIntervals) && !bFound; j++, uTemp++)
                if (uTemp == uTest)
                {
                    dwFormat = (INT) i;
                    bFound = true;
                }

    if (!bFound) {
        SetFrameDuration(1.0f);
        return false;
    }

    if (dwFormat != m_dwFormat)
    {
//        DWORD dwFlags = CDDS_TEXTURE_VIDEO;
        DWORD dwWidth = (DWORD) TEXTURE_WIDTH, dwHeight = (DWORD) TEXTURE_HEIGHT;

        m_dwFormat = dwFormat;
//        m_pMode->nTextureFormat = dwFormat;

        if (m_pBaseTexture) {
            ReleaseTexture(m_pBaseTexture);
            m_pBaseTexture = NULL;
        }

        if (m_pHeightTexture) {
            ReleaseTexture(m_pHeightTexture);
            m_pHeightTexture = NULL;
        }

//    RELEASE(m_pBaseImage);

//        if (!(m_pBaseTexture = CreateTexture(dwWidth, dwHeight, dwFlags, m_pBaseImage)))
        if (!(m_pBaseTexture = CreateChannelDataTexture(m_pDevice, dwWidth, dwHeight, &m_DataBase, m_fmtCommon[dwFormat])))
        {
            WriteToLog(_T("Unable to create base texture."));
            return false;
        }

//        if (!(m_pHeightTexture = CreateTexture(dwWidth, dwHeight, dwFlags, m_pHeightImage)))
        if (!(m_pHeightTexture = CreateChannelDataTexture(m_pDevice, dwWidth, dwHeight, &m_DataHeight, m_fmtCommon[dwFormat])))
        {
            WriteToLog(_T("Unable to create height texture."));
            return false;
        }
    }

    m_Shape.OffsetCoords((DWORD) 1, (D3DVALUE) -m_fOffset[0], (D3DVALUE) -m_fOffset[1]);
    fTheta = (2.0f*pi*(uTest - 1))/((DOUBLE) m_uIntervals);
    fTheta = (fTheta > 2.0f*pi) ? (fTheta - 2.0f*pi) : fTheta;
    fRadius = LEMNISCATE_SCALE*sin(m_fFactor*fTheta);
    m_fOffset[0] = -fRadius*cos(fTheta);
    m_fOffset[1] = -fRadius*sin(fTheta);
    m_Shape.OffsetCoords((DWORD) 1, (D3DVALUE) m_fOffset[0], (D3DVALUE) m_fOffset[1]);

    SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD) TRUE);
    SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD) D3DBLEND_SRCALPHA);
    SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD) D3DBLEND_ZERO);

    SetTextureStageState(iStage, D3DTSS_ADDRESSU, (DWORD) D3DTADDRESS_WRAP);
    SetTextureStageState(iStage, D3DTSS_ADDRESSV, (DWORD) D3DTADDRESS_WRAP);
    SetTextureStageState(iStage, D3DTSS_MAGFILTER, (DWORD) TEXF_LINEAR);
    SetTextureStageState(iStage, D3DTSS_MINFILTER, (DWORD) TEXF_LINEAR);
    SetTextureStageState(iStage, D3DTSS_MIPFILTER, (DWORD) TEXF_NONE);
    SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE); // D3DTOP_MODULATE2X
    SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_COLORARG2, (DWORD) D3DTA_DIFFUSE);
    SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_CURRENT);
    SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 0);
    SetTexture(0, m_pBaseTexture);
    iStage++;

    SetTextureStageState(iStage, D3DTSS_ADDRESSU, (DWORD) D3DTADDRESS_WRAP);
    SetTextureStageState(iStage, D3DTSS_ADDRESSV, (DWORD) D3DTADDRESS_WRAP);
    SetTextureStageState(iStage, D3DTSS_MAGFILTER, (DWORD) TEXF_LINEAR);
    SetTextureStageState(iStage, D3DTSS_MINFILTER, (DWORD) TEXF_LINEAR);
    SetTextureStageState(iStage, D3DTSS_MIPFILTER, (DWORD) TEXF_NONE);
    SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_SELECTARG2);
    SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_COLORARG2, (DWORD) D3DTA_CURRENT);
    SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_ADDSIGNED); // ADDSIGNED2X
    SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) (D3DTA_TEXTURE | D3DTA_COMPLEMENT));
    SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_CURRENT);
    SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 1);
    SetTexture(iStage, m_pHeightTexture);
    iStage++;

    SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
    SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);
    iStage++;

    if (m_Flags.bCheck(VALIDATE))
    {
        DWORD dwSrc, dwRef;
        HRESULT hSrc, hRef;

        if (!ValidateDevice(&hSrc, &dwSrc, &hRef, &dwRef))
        {
            if (D3D_OK != hSrc)
            {
                WriteToLog(_T("Src requires %d passes:  %X."), dwSrc, hSrc);
                bValid = false;
            }

            if (D3D_OK != hRef)
            {
                WriteToLog(_T("Ref requires %d passes:  %X."), dwRef, hRef);
                bValid = false;
            }
        }
    }

    if (!bValid)
    {
        SkipTests((UINT) 1);
        return true;
    }

    sprintf(szBuffer, "Test:  %d", uTest);
    BeginTestCase(szBuffer);

    SwitchAPI((UINT)m_fFrame);

    m_pDevice->SetViewport(&m_vpFull);
    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, RGB_MAKE(74, 150, 202), 1.0f, 0);

    m_pDevice->SetViewport(&m_vpTest);

    ClearFrame();

    if (BeginScene())
    {
        if (!RenderPrimitive(D3DPT_TRIANGLELIST, m_dwFVF,
                             m_pVertices, m_Shape.m_nVertices,
                             m_Shape.m_pIndices, m_Shape.m_nIndices, (DWORD) 0))
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

//    return bResult;
    return false;
}

// NAME:        ProcessFrame()
// DESC:        process frame
// INPUT:       none
// OUTPUT:      bResult..result of src:ref image comparison

bool CEmboss::ProcessFrame(VOID)
{
    bool bResult;
    CHAR szBuffer[MAXBUFFER];
    static int nPass = 0;
    static int nFail = 0;

    bResult = GetCompareResult(0.15f, 0.78f, (int) 0);

    if (bResult)
    {
        (m_fIncrementForward) ? nPass++ : nPass--;
        sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Pass();
    }
    else
    {
        (m_fIncrementForward) ? nFail++ : nFail--;
        sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Fail();
    }

    WriteStatus("$gSummary", szBuffer);
    return bResult;
}

// NAME:        TestTerminate()
// DESC:        terminate test
// INPUT:       none
// OUTPUT:      true

bool CEmboss::TestTerminate(VOID)
{
//    RELEASE(m_pMaterial);

    ReleaseTexture(m_pBaseTexture);
    m_pBaseTexture = NULL;
//    RELEASE(m_pBaseImage);

    ReleaseTexture(m_pHeightTexture);
    m_pHeightTexture = NULL;
//    RELEASE(m_pHeightImage);
    return true;
}

// NAME:        SetDefaultLightStates()
// DESC:        set default light states
// INPUT:       none
// OUTPUT:      true.....if SetLightState() succeeds
//              false....otherwise

bool CEmboss::SetDefaultLightStates(VOID)
{
    return SetRenderState(D3DRENDERSTATE_AMBIENT, RGBA_MAKE(164, 164, 164, 255));
}

// NAME:        SetDefaultLights()
// DESC:        set default lights
// INPUT:       none
// OUTPUT:      true.....if default lights set successfully
//              false....otherwise

bool CEmboss::SetDefaultLights(VOID)
{
    bool bSet;
//    CLight *pLight = CreateLight();
    D3DLIGHT8 Data;
    HRESULT     hr;

    memset(&Data, 0, sizeof(Data));
//    Data.dwSize = sizeof(D3DLIGHT2);
    Data.Type = D3DLIGHT_POINT;
    Data.Diffuse.r = 0.4f;
    Data.Diffuse.g = 0.4f;
    Data.Diffuse.b = 0.4f;
    Data.Diffuse.a = 1.0f;
    Data.Position.x = 0.0f;
    Data.Position.y = 0.0f;
    Data.Position.z = -500.0f;
    Data.Range = D3DLIGHT_RANGE_MAX;
    Data.Attenuation0 = 1.0f;
    Data.Attenuation1 = 0.0f;
    Data.Attenuation2 = 0.0f;
//    Data.Flags = D3DLIGHT_ACTIVE;

//    pLight->SetLight(&Data);
//    bSet = SetLight((UINT) 0, pLight);
//    delete pLight;
//    return bSet;

    hr = m_pDevice->SetLight(0, &Data);
    if (FAILED(hr)) {
        return false;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
	
	return (hr == D3D_OK);
}

// NAME:        SetDefaultMaterials()
// DESC:        set default materials
// INPUT:       none
// OUTPUT:      true.....if default materials set successfully
//              false....otherwise

bool CEmboss::SetDefaultMaterials(VOID)
{
    bool bSet;
    D3DMATERIAL8 Data;
    HRESULT hr;

//    RELEASE(m_pMaterial);
//    m_pMaterial = CreateMaterial();

    memset(&Data, 0 , sizeof(Data));
//    Data.dwSize = sizeof(Data);
    Data.Diffuse.r = 0.8f;
    Data.Diffuse.g = 0.8f;
    Data.Diffuse.b = 0.8f;
    Data.Diffuse.a = 0.5f;
    Data.Ambient.r = 0.4f;
    Data.Ambient.g = 0.4f;
    Data.Ambient.b = 0.4f;
    Data.Ambient.a = 0.0f;
    Data.Specular.r = 0.2f;
    Data.Specular.g = 0.2f;
    Data.Specular.b = 0.2f;
    Data.Specular.a = 0.0f;
    Data.Power = 20.0f;
//  Data.dwRampSize = (DWORD) 32;

//    m_pMaterial->SetMaterial(&Data);
//    bSet = SetMaterial(m_pMaterial);
//    return bSet;

    hr = m_pDevice->SetMaterial(&Data);
    return (hr == D3D_OK);
}

// NAME:        SetDefaultMatrices()
// DESC:        setup default matrices
// INPUT:       none
// OUTPUT:      none

bool CEmboss::SetDefaultMatrices(VOID)
{
    D3DMATRIX Matrix;
    D3DVALUE dvNear = 1.0f, dvFar = 8.0f, dvAngle = 30.0f*pi/180.0f;
    D3DVECTOR At, From, Up;

    Matrix = ProjectionMatrix(dvNear, dvFar, dvAngle);
    Matrix = MatrixMult(Matrix, (float) (1.0/Matrix._34));

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION, &Matrix))
        return false;

    At.x = 0.0f;
    At.y = 0.0f;
    At.z = 0.0f;

    From.x = 0.0f;
    From.y = 0.0f;
    From.z = -2.0f;

    Up.x = 0.0f;
    Up.y = 1.0f;
    Up.z = 0.0f;

    Matrix = ViewMatrix(From, At, Up);

    if (!SetTransform(D3DTRANSFORMSTATE_VIEW, &Matrix))
        return false;

    return true;
}

// NAME:        vProcessArgs()
// DESC:        process args
// INPUT:       none
// OUTPUT:      none

VOID CEmboss::vProcessArgs(VOID)
{
    int iDefault, iResult;

    if (KeySet("WHQL"))
    {
        m_Flags.vSet(VALIDATE);
        m_fFactor = (DOUBLE) 2.0;
        m_uIntervals = (UINT) 32;
        return;
    }

    // factor
    // default:  2

    iDefault = (int) 2;
    ReadInteger("factor", iDefault, &iResult);
    m_fFactor = (DOUBLE) ((iResult < MINF) ? MINF : ((iResult >= MAXF) ? MAXF: iResult));

    // intervals
    // default:  32

    iDefault = (int) 32;
    ReadInteger("intervals", iDefault, &iResult);
    m_uIntervals = (UINT) ((iResult < MINI) ? MINI : ((iResult >= MAXI) ? MAXI: iResult));

    // validate
    // default: false

    if (KeySet("validate"))
        m_Flags.vSet(VALIDATE);
}
