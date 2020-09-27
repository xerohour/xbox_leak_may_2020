// FILE:        bump.cpp
// DESC:        bump class methods for
//                  D3DTOP_BUMPENVMAP
//                  D3DTOP_BUMPENVMAPLUMINANCE
//              conformance tests
// AUTHOR:      Todd M. Frost

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "cshapesfvf.h"
#include "bump.h"

//CD3DWindowFramework App;

const UINT DEFAULT_BUMPS = 6;  // default number of bumps
const UINT MIN_BUMPS     = 1;  // min number of bumps
const UINT MAX_BUMPS     = 64; // max number of bumps

const UINT DEFAULT_VARIATIONS = 8;    // default number of variations
const UINT MIN_VARIATIONS     = 2;    // min number of variations
const UINT MAX_VARIATIONS     = 1024; // max number of variations

UINT guBumps = (UINT) DEFAULT_BUMPS;

CTexture8* CreateBumpChannelDataTexture(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, PCHANNELDATA pData, D3DFORMAT fmt);

// NAME:        fBumpDu()
// DESC:        bumpenvmap du
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      fx = df/dx = cos(x)*sin(y)
// COMMENTS:    (u,v) scaled from [0,0]x[1,1] to [-guBumps*pi/2,guBumps*pi/2]
//              [-1,1] maps to [-127,127] in LoadChannelData()

FLOAT fBumpDu(FLOAT u, FLOAT v)
{
    FLOAT fTemp, x, y;

    fTemp = (FLOAT) (guBumps*pi); 
    x = fTemp*(u - 0.5f);
    y = fTemp*(v - 0.5f);
    return (FLOAT) (cos(x)*sin(y));
}

// NAME:        fBumpDv()
// DESC:        bumpenvmap dv
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      fy = df/dy = sin(x)*cos(y)
// COMMENTS:    (u,v) scaled from [0,0]x[1,1] to [-guBumps*pi/2,guBumps*pi/2]
//              [-1,1] maps to [-127,127] in LoadChannelData()

FLOAT fBumpDv(FLOAT u, FLOAT v)
{
    FLOAT fTemp, x, y;

    fTemp = (FLOAT) (guBumps*pi); 
    x = fTemp*(u - 0.5f);
    y = fTemp*(v - 0.5f);
    return (FLOAT) (sin(x)*cos(y));
}

// NAME:        fBumpLuminance()
// DESC:        bumpenvmap luminance
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      f= 0.2*sin(x)*sin(y)
// COMMENTS:    (u,v) scaled from [0,0]x[1,1] to [-guBumps*pi/2,guBumps*pi/2]
//              [-1,1] maps to [-127,127] in LoadChannelData()

FLOAT fBumpLuminance(FLOAT u, FLOAT v)
{
    FLOAT fTemp, x, y;

    fTemp = (FLOAT) (guBumps*pi);
    x = fTemp*(u - 0.5f);
    y = fTemp*(v - 0.5f);
    return (FLOAT) (sin(x)*sin(y));
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

    CBump*  pBump;
    BOOL    bQuit = FALSE, bRet = TRUE;
    UINT    i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 5 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pBump = new CBumpAddress();
                break;
            case 1:
                pBump = new CBumpBlend();
                break;
            case 2:
                pBump = new CBumpMatrix();
                break;
            case 3:
                pBump = new CBumpOffset();
                break;
            case 4:
                pBump = new CBumpScale();
                break;
        }

        if (!pBump) {
            return FALSE;
        }

        // Initialize the scene
        if (!pBump->Create(pDisplay)) {
            pBump->Release();
            return FALSE;
        }

        bRet = pBump->Exhibit(pnExitCode);

        bQuit = pBump->AbortedExit();

        // Clean up the scene
        pBump->Release();
    }

    return bRet;
}

// NAME:        CBump()
// DESC:        constructor for bump class
// INPUT:       none
// OUTPUT:      none

CBump::CBump(VOID)
{
    D3DFORMAT fmt[] = {
        D3DFMT_X8L8V8U8,
        D3DFMT_L6V5U5,
        D3DFMT_V8U8,
        D3DFMT_Q8W8V8U8,
        D3DFMT_V16U16,
#ifndef DEMO_HACK
        D3DFMT_W11V11U10,
#endif
    };

    m_dwModes[0] = DEFAULT_MODE;
    m_dwModes[1] = DEFAULT_MODE;

    m_dwFormat = NOFORMAT;

    m_dwFVF = (DWORD) 0;

//    m_dwInclusion = (DWORD) PF_BUMPDUDV;

    m_dwModeCapsVals[0][0] = D3DPTADDRESSCAPS_BORDER;
    m_dwModeCapsVals[0][1] = D3DTADDRESS_BORDER;
    m_dwModeCapsVals[1][0] = D3DPTADDRESSCAPS_CLAMP;
    m_dwModeCapsVals[1][1] = D3DTADDRESS_CLAMP;
    m_dwModeCapsVals[2][0] = D3DPTADDRESSCAPS_MIRROR;
    m_dwModeCapsVals[2][1] = D3DTADDRESS_MIRROR;
    m_dwModeCapsVals[3][0] = D3DPTADDRESSCAPS_WRAP;
    m_dwModeCapsVals[3][1] = D3DTADDRESS_WRAP;

    m_fOffset.f = DEFAULT_OFFSET;
    m_fScale.f = DEFAULT_SCALE;

    vSetMatrix((DWORD) MATRIX_SCALE_DUDV, DEFAULT_MAG, DEFAULT_MAG);

    m_pVertices = NULL;

//    m_pBaseImage = NULL;
//    m_pBumpImage = NULL;

    m_pBaseTexture = NULL;
    m_pBumpTexture = NULL;

//    m_pMaterial = NULL;

    m_uCommonTextureFormats = countof(fmt);
    memcpy(m_fmtCommon, fmt, m_uCommonTextureFormats * sizeof(D3DFORMAT));

    m_bExit = FALSE;
}

// NAME:        ~CBump()
// DESC:        destructor for bump class
// INPUT:       none
// OUTPUT:      none

CBump::~CBump(VOID)
{
    if (m_pVertices)
        free (m_pVertices);

//    RELEASE(m_pMaterial);

    ReleaseTexture(m_pBumpTexture);
    m_pBumpTexture = NULL;
//    RELEASE(m_pBumpImage);

    ReleaseTexture(m_pBaseTexture);
    m_pBaseTexture = NULL;
//    RELEASE(m_pBaseImage);
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CBump::CommandLineHelp(void)
{
//    WriteCommandLineHelp("$ybumps: $wbumps per texture $c(default=6)");
//    WriteCommandLineHelp("$yvalidate: $wvalidate $c(On/$wOff$c)");
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code
// COMMENTS:    material released before returning D3DTESTINIT_SKIPALL or D3DTESTINIT_ABORT
//              RELEASE(m_pMaterial) causes access violations in TestTerminate otherwise

UINT CBump::TestInitialize(VOID)
{
    bool bFound;
//    CImageLoader Loader;
    DWORD dwCount, dwFlags, dwFormat;
    DWORD dwWidth = (DWORD) 256, dwHeight = (DWORD) 256;
    D3DCOLOR pdwColors[] = {
        RGBA_MAKE(255, 255, 255, 255),
        RGBA_MAKE(255,   0,   0, 255),
        RGBA_MAKE(  0, 255,   0, 255),
        RGBA_MAKE(  0,   0, 255, 255)
    };
//    INT nDevice = m_pMode->nSrcDevice;
    UINT i, j, k;

    if (m_pDisplay->GetConnectionStatus() == SERVER_CONNECTED) {
        m_uCommonTextureFormats--;
    }

    // set defaults

    m_dwModes[0] = DEFAULT_MODE;
    m_dwModes[1] = DEFAULT_MODE;

    m_dwFormat = NOFORMAT;

    m_fOffset.f = DEFAULT_OFFSET;
    m_fScale.f = DEFAULT_SCALE;

    vSetMatrix((DWORD) MATRIX_SCALE_DUDV, DEFAULT_MAG, DEFAULT_MAG);
    vProcessArgs();

    // scan for base texture (non-palettized rgb texture with largest number of green bits)

    bFound = true;
/* 
    if (!RequestTextureFormats((DWORD) PF_RGB, (DWORD) PF_ALPHAPIXELS))
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
        RELEASE(m_pMaterial);
        return D3DTESTINIT_ABORT;
    }

//    m_pMode->nTextureFormat = (int) 0;
    dwCount = (DWORD) 0;

    // TODO: need to handle DX8 formats (m_pCommonTextureFormats[m_pMode->nTextureFormat].d3dfFormat)
    for (dwFormat = 0; dwFormat < m_uCommonTextureFormats; dwFormat++)
        if (dwCount < dwGetCountFromMask(m_pCommonTextureFormats[dwFormat].ddpfPixelFormat.dwGBitMask))
        {
            dwCount = dwGetCountFromMask(m_pCommonTextureFormats[dwFormat].ddpfPixelFormat.dwGBitMask);
            m_pMode->nTextureFormat = (int) dwFormat;
        }

    dwCount = sizeof(pdwColors)/sizeof(D3DCOLOR);

    if (!(m_pBaseImage = Loader.LoadStripes(dwWidth, dwHeight, dwCount, pdwColors, true, true)))
    {
        WriteToLog(_T("Unable to load base image."));
        RELEASE(m_pMaterial);
        return D3DTESTINIT_ABORT;
    }

    if (!(m_pBaseTexture = CreateTexture(dwWidth, dwHeight, CDDS_TEXTURE_VIDEO, m_pBaseImage)))
*/
    m_pBaseTexture = CreateStripedTexture(m_pDevice, dwWidth, dwHeight, 
                            sizeof(pdwColors)/sizeof(D3DCOLOR), pdwColors,
                            true, true, D3DFMT_X8R8G8B8);
    if (!m_pBaseTexture)
    {
        WriteToLog(_T("Unable to create base texture."));
//        RELEASE(m_pMaterial);
        return D3DTESTINIT_ABORT;
    }

    // scan for bump textures

//    ResetTextureFormats();
/*
    if (!RequestTextureFormats(m_dwInclusion))
    {
        WriteToLog(_T("No BUMPDUDV texture formats enumerated."));
        RELEASE(m_pMaterial);
        SetTestRange((UINT) 1, (UINT) 1);
        return D3DTESTINIT_SKIPALL;
    }
*/
    // check caps and log cap/texture enumeration mismatches

    m_dwTexAddressCaps = m_d3dcaps.TextureAddressCaps;
    m_dwTexOpCaps = m_d3dcaps.TextureOpCaps;
    m_dwSrcBlendCaps = m_d3dcaps.SrcBlendCaps;
    m_dwDestBlendCaps = m_d3dcaps.DestBlendCaps;

    if (!(m_dwTexOpCaps & D3DTEXOPCAPS_BUMPENVMAP))
        WriteToLog(_T("BUMPDUDV textures enumerated, D3DTOP_BUMPENVMAP not supported."));

    // TODO: need to handle DX8 formats (m_pCommonTextureFormats[m_pMode->nTextureFormat].d3dfFormat)
    if (!(m_dwTexOpCaps & D3DTEXOPCAPS_BUMPENVMAPLUMINANCE))
//        for (i = 0; i < m_uCommonTextureFormats; i++)
//            if (m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags & PF_BUMPLUMINANCE)
                WriteToLog(_T("BUMPLUMINANCE textures enumerated, D3DTOP_BUMPENVMAPLUMINANCE not supported."));

    if (m_d3dcaps.MaxTextureBlendStages < 2)
    {
        WriteToLog(_T("Device does NOT support sufficient number of texture stages (2)."));
//        RELEASE(m_pMaterial);
        return D3DTESTINIT_ABORT;
    }

    // initialize triangle mesh and texture image data

    if (m_dwTexAddressCaps & D3DPTADDRESSCAPS_INDEPENDENTUV)
        m_Flags.vSet(INDEPENDENT);

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

    m_Data.dwAlpha = (DWORD) CIL_UNSIGNED;
    m_Data.dwRed   = (DWORD) 0;
    m_Data.dwGreen = (DWORD) 0;
    m_Data.dwBlue  = (DWORD) CIL_UNSIGNED;
    m_Data.fAlpha = NULL;
    m_Data.fRed   = fBumpDu;
    m_Data.fGreen = fBumpDv;
    m_Data.fBlue  = fBumpLuminance;

    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CBump::ExecuteTest(UINT uTest)
{
    bool bResult = true, bValid = true;
    int iStage = (int) 0;
//    CHAR szFormat[MAXBUFFER];
    CHAR szBlend[NBLENDS+1][11] = {
        "MODULATE",
        "MODULATE2X",
        "MODULATE4X",
        "SELECTARG1"
    };
    CHAR szMatrixFormat[NMATRICES][14] = {
        "scale Du",
        "scale Dv",
        "scale DuDv",
        "skew Du",
        "skew Dv",
        "skew DuDv"
    };
    CHAR szMode[NMODES][7] = {
        "wrap",
        "mirror",
        "clamp",
        "border"
    };
    LPCSTR szFormat[] = {
        "D3DFMT_V8U8",
        "D3DFMT_L6V5U5",
        "D3DFMT_X8L8V8U8",
        "D3DFMT_Q8W8V8U8",
        "D3DFMT_V16U16",
        "D3DFMT_W11V11U10",
    };
    D3DCOLOR dwColor;
    D3DTEXTUREOP dwOp;
    UINT uBlend;

    // TODO: need to handle DX8 formats (m_pCommonTextureFormats[m_pMode->nTextureFormat].d3dfFormat)
//    if (m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat.dwFlags & PF_BUMPLUMINANCE)
    if (m_dwFormat > 3)
        dwOp = D3DTOP_BUMPENVMAPLUMINANCE;
    else
        dwOp = D3DTOP_BUMPENVMAP;

    dwColor = RGBA_MAKE(128, 128, 128, 255);    // (Du,Dv,Lu) = (0.0,0.0,0.5)
    SetTextureStageState(iStage, D3DTSS_BORDERCOLOR, dwColor);

    if (m_Flags.bCheck(INDEPENDENT))
    {
        SetTextureStageState(iStage, D3DTSS_ADDRESSU, (DWORD) m_dwModes[0]);
        SetTextureStageState(iStage, D3DTSS_ADDRESSV, (DWORD) m_dwModes[1]);
    }
    else
    {
        SetTextureStageState(iStage, D3DTSS_ADDRESSU, (DWORD) m_dwModes[0]);
        SetTextureStageState(iStage, D3DTSS_ADDRESSV, (DWORD) m_dwModes[0]);
    }

    SetTextureStageState(iStage, D3DTSS_MAGFILTER, (DWORD) TEXF_LINEAR);
    SetTextureStageState(iStage, D3DTSS_MINFILTER, (DWORD) TEXF_LINEAR);
    SetTextureStageState(iStage, D3DTSS_MIPFILTER, (DWORD) TEXF_NONE);
    SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) dwOp);
    SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_COLORARG2, (DWORD) D3DTA_DIFFUSE);
    SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_DIFFUSE);
    SetTextureStageState(iStage, D3DTSS_BUMPENVMAT00, m_fMatrix[0][0].dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVMAT01, m_fMatrix[0][1].dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVMAT10, m_fMatrix[1][0].dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVMAT11, m_fMatrix[1][1].dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVLOFFSET, m_fOffset.dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVLSCALE, m_fScale.dw);
    SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 1);
    SetTexture(iStage, m_pBumpTexture);
    iStage++;

    if (m_Flags.bCheck(MODULATE))
    {
        dwOp = D3DTOP_MODULATE;
        uBlend = (UINT) 0;
    }
    else if (m_Flags.bCheck(MODULATE2X))
    {
        dwOp = D3DTOP_MODULATE2X;
        uBlend = (UINT) 1;
    }
    else if (m_Flags.bCheck(MODULATE4X))
    {
        dwOp = D3DTOP_MODULATE4X;
        uBlend = (UINT) 2;
    }
    else
    {
        dwOp = D3DTOP_SELECTARG1;
        uBlend = (UINT) 3;
    }

    dwColor = RGBA_MAKE(0, 0, 255, 255);
    SetTextureStageState(iStage, D3DTSS_BORDERCOLOR, dwColor);

    if (m_Flags.bCheck(INDEPENDENT))
    {
        SetTextureStageState(iStage, D3DTSS_ADDRESSU, (DWORD) m_dwModes[0]);
        SetTextureStageState(iStage, D3DTSS_ADDRESSV, (DWORD) m_dwModes[1]);
    }
    else
    {
        SetTextureStageState(iStage, D3DTSS_ADDRESSU, (DWORD) m_dwModes[0]);
        SetTextureStageState(iStage, D3DTSS_ADDRESSV, (DWORD) m_dwModes[0]);
    }

    SetTextureStageState(iStage, D3DTSS_MAGFILTER, (DWORD) TEXF_LINEAR);
    SetTextureStageState(iStage, D3DTSS_MINFILTER, (DWORD) TEXF_LINEAR);
    SetTextureStageState(iStage, D3DTSS_MIPFILTER, (DWORD) TEXF_NONE);
    SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) dwOp);
    SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_COLORARG2, (DWORD) D3DTA_CURRENT);
    SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_CURRENT);
    SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 0);

    SetTextureStageState(iStage, D3DTSS_BUMPENVMAT00, m_fMatrix[0][0].dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVMAT01, m_fMatrix[0][1].dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVMAT10, m_fMatrix[1][0].dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVMAT11, m_fMatrix[1][1].dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVLOFFSET, m_fOffset.dw);
    SetTextureStageState(iStage, D3DTSS_BUMPENVLSCALE, m_fScale.dw);

    SetTexture(iStage, m_pBaseTexture);
    iStage++;

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
#endif
   
    // TODO: need to handle DX8 formats (m_pCommonTextureFormats[m_pMode->nTextureFormat].d3dfFormat)
//    sprintf(szFormat, "Du%d:Dv%d:Lu%d", 
//            dwGetCountFromMask(m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat.dwBumpDuBitMask),
//            dwGetCountFromMask(m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat.dwBumpDvBitMask),
//            dwGetCountFromMask(m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat.dwBumpLuminanceBitMask));
    sprintf(m_szBuffer, "%s, %s (%s,%s)",
            szFormat[m_dwFormat],
            szBlend[uBlend],
            szMode[m_dwModes[0] - 1], szMode[m_dwModes[1] - 1]);
    BeginTestCase(m_szBuffer);

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

    sprintf(m_szBuffer, "$y%s", szFormat);
    WriteStatus("$gFormat", m_szBuffer);
    sprintf(m_szBuffer, "$y%s", szMatrixFormat[m_dwMatrixFormat]);
    WriteStatus("$gMatrix", m_szBuffer);
    sprintf(m_szBuffer, "$y%f", m_fOffset.f);
    WriteStatus("$gOffset", m_szBuffer);
    sprintf(m_szBuffer, "$y%f", m_fScale.f);
    WriteStatus("$gScale", m_szBuffer);
    sprintf(m_szBuffer, "$y%s", szBlend[uBlend]);
    WriteStatus("$gBlend", m_szBuffer);
    sprintf(m_szBuffer, "$y%s", szMode[m_dwModes[0] - 1]);
    WriteStatus("$gU Mode", m_szBuffer);
    sprintf(m_szBuffer, "$y%s", szMode[m_dwModes[1] - 1]);
    WriteStatus("$gV Mode", m_szBuffer);

    ProcessFrame();

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

//    return bResult;
    return false;
}

// NAME:        ProcessFrame()
// DESC:        process frame
// INPUT:       none
// OUTPUT:      bResult..result of src:ref image comparison

bool CBump::ProcessFrame(VOID)
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

bool CBump::TestTerminate(VOID)
{
//    RELEASE(m_pMaterial);

    ReleaseTexture(m_pBaseTexture);
    m_pBaseTexture = NULL;
//    RELEASE(m_pBaseImage);

    ReleaseTexture(m_pBumpTexture);
    m_pBumpTexture = NULL;
//    RELEASE(m_pBumpImage);
    return true;
}

// NAME:        SetDefaultLightStates()
// DESC:        set default light states
// INPUT:       none
// OUTPUT:      true.....if SetLightState() succeeds
//              false....otherwise

bool CBump::SetDefaultLightStates(VOID)
{
//    return SetLightState(D3DLIGHTSTATE_AMBIENT, RGBA_MAKE(164, 164, 164, 255));
    return SetRenderState(D3DRENDERSTATE_AMBIENT, RGBA_MAKE(164, 164, 164, 255));
}

// NAME:        SetDefaultLights()
// DESC:        set default lights
// INPUT:       none
// OUTPUT:      true.....if default lights set successfully
//              false....otherwise

bool CBump::SetDefaultLights(VOID)
{
//    bool bSet;
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
//    Data.dwFlags = D3DLIGHT_ACTIVE;

//    pLight->SetLight(&Data);
//    bSet = SetLight((UINT) 0, pLight);
//    RELEASE(pLight);
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

bool CBump::SetDefaultMaterials(VOID)
{
//    bool bSet;
    D3DMATERIAL8 Data;
    HRESULT      hr;

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
//    Data.dwRampSize = (DWORD) 32;

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

bool CBump::SetDefaultMatrices(VOID)
{
    D3DMATRIX Matrix;
    D3DVALUE dvNear = 1.0f, dvFar = 8.0f, dvAngle = 30.0f*pi/180.0f;
    D3DVECTOR At, From, Up;

    Matrix = ProjectionMatrix(dvNear, dvFar, dvAngle);
    
    Matrix = MatrixMult(Matrix, (float) (1.0/Matrix._34) );

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

// NAME:        bSetBump()
// DESC:        set bump map texture
// INPUT:       dwFormat.....texture format
//              dwWidth......texture width
//              dwHeight.....texture height
// OUTPUT:      true.....if requested format is set successfully
//              false....otherwise

bool CBump::bSetBump(DWORD dwFormat, DWORD dwWidth, DWORD dwHeight)
{
    DWORD dwFlags;

    if (m_dwFormat == dwFormat)
        return true;

    if (dwFormat >= m_uCommonTextureFormats)
        return false;

    ReleaseTexture(m_pBumpTexture);
    m_pBumpTexture = NULL;
//    dwFlags = CDDS_TEXTURE_VIDEO;
//    m_pMode->nTextureFormat = dwFormat;
//    m_pBumpTexture = CreateTexture(dwWidth, dwHeight, dwFlags, m_pBumpImage);
    m_pBumpTexture = CreateBumpChannelDataTexture(m_pDevice, dwWidth, dwHeight, &m_Data, m_fmtCommon[dwFormat]);
    if (!m_pBumpTexture)
    {
        WriteToLog(_T("Unable to create bump texture."));
        return false;
    }

    m_dwFormat = dwFormat;
    return true;
}

// NAME:        vSetMatrix()
// DESC:        set bump matrix
// INPUT:       dwType...matrix type
//              fDu......du
//              fDv......dv
// OUTPUT:      none

VOID CBump::vSetMatrix(DWORD dwType, FLOAT fDu, FLOAT fDv)
{
    switch (dwType)
    {
        case MATRIX_SCALE_DU:
            m_fMatrix[0][0].f = fDu;
            m_fMatrix[0][1].f = 0.0f;
            m_fMatrix[1][0].f = 0.0f;
            m_fMatrix[1][1].f = 0.0f;
            m_dwMatrixFormat = MATRIX_SCALE_DU;
        break;

        case MATRIX_SCALE_DV:
            m_fMatrix[0][0].f = 0.0f;
            m_fMatrix[0][1].f = 0.0f;
            m_fMatrix[1][0].f = 0.0f;
            m_fMatrix[1][1].f = fDv;
            m_dwMatrixFormat = MATRIX_SCALE_DV;
        break;

        case MATRIX_SCALE_DUDV:
        default:
            m_fMatrix[0][0].f = fDu;
            m_fMatrix[0][1].f = 0.0f;
            m_fMatrix[1][0].f = 0.0f;
            m_fMatrix[1][1].f = fDv;
            m_dwMatrixFormat = MATRIX_SCALE_DUDV;
        break;

        case MATRIX_SKEW_DU:
            m_fMatrix[0][0].f = 0.0f;
            m_fMatrix[0][1].f = fDu;
            m_fMatrix[1][0].f = 0.0f;
            m_fMatrix[1][1].f = 0.0f;
            m_dwMatrixFormat = MATRIX_SKEW_DU;
        break;

        case MATRIX_SKEW_DV:
            m_fMatrix[0][0].f = 0.0f;
            m_fMatrix[0][1].f = 0.0f;
            m_fMatrix[1][0].f = fDv;
            m_fMatrix[1][1].f = 0.0f;
            m_dwMatrixFormat = MATRIX_SKEW_DV;
        break;

        case MATRIX_SKEW_DUDV:
            m_fMatrix[0][0].f = 0.0f;
            m_fMatrix[0][1].f = fDu;
            m_fMatrix[1][0].f = fDv;
            m_fMatrix[1][1].f = 0.0f;
            m_dwMatrixFormat = MATRIX_SKEW_DUDV;
        break;
    }
}

// NAME:        dwGetCountFromMask()
// DESC:        get count from mask 
// INPUT:       dwMask...bit mask
// OUTPUT:      dwCount...bit count

DWORD CBump::dwGetCountFromMask(DWORD dwMask)
{
    DWORD dwCount, dwTemp;

    for (dwCount = (DWORD) 0, dwTemp = dwMask; dwTemp; dwTemp >>= 1) if (dwTemp & 1) dwCount++;
    return dwCount;
}

// NAME:        fNormalize()
// DESC:        normalize distance of dwCurrent along [dwMin, dwMax]
// INPUT:       dwCurrent....current interval location
//              dwMin........min interval endpoint
//              dwMax........max interval endpoint
// OUTPUT:      fRatio.......normalized distance of dwCurrent along [dwMin, dwMax]

FLOAT CBump::fNormalize(DWORD dwCurrent, DWORD dwMin, DWORD dwMax)
{
    DWORD dwClamped;
    FLOAT fTemp = 0.0f;

    if (dwMin == dwMax)
        return fTemp;

    if (dwCurrent > dwMax)
        dwClamped = dwMax;
    else if (dwCurrent < dwMin)
        dwClamped = dwMin;
    else
        dwClamped = dwCurrent;

    fTemp = ((FLOAT) (dwClamped - dwMin)) / ((FLOAT) (dwMax - dwMin));
    return fTemp;
}

// NAME:        vProcessArgs()
// DESC:        process args
// INPUT:       none
// OUTPUT:      none

VOID CBump::vProcessArgs(VOID)
{
    int iDefault, iResult;

    if (KeySet("WHQL"))
    {
        m_Flags.vSet(VALIDATE);
        guBumps = (UINT) DEFAULT_BUMPS;
        m_uVariations = (UINT) DEFAULT_VARIATIONS;
        return;
    }

    // bumps
    // default:  DEFAULT_BUMPS

    iDefault = (int) DEFAULT_BUMPS;
    ReadInteger("bumps", iDefault, &iResult);

    if (iResult < MIN_BUMPS)
        guBumps = (UINT) MIN_BUMPS;
    else if (iResult > MAX_BUMPS)
        guBumps = (UINT) MAX_BUMPS;
    else
        guBumps = (UINT) iResult;

    // variations
    // default:  DEFAULT_VARIATIONS

    iDefault = (int) DEFAULT_VARIATIONS;
    ReadInteger("variations", iDefault, &iResult);

    if (iResult < MIN_VARIATIONS)
        m_uVariations = (UINT) MIN_VARIATIONS;
    else if (iResult > MAX_VARIATIONS)
        m_uVariations = (UINT) MAX_VARIATIONS;
    else
        m_uVariations = (UINT) iResult;

    // validate
    // default:  true

    if (!KeySet("!validate"))
        m_Flags.vSet(VALIDATE);
}

//******************************************************************************
BOOL CBump::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CBump::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CBump::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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

//******************************************************************************
CTexture8* CreateBumpChannelDataTexture(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, PCHANNELDATA pData, D3DFORMAT fmt)
{
    CTexture8*      pd3dt;
    D3DLOCKED_RECT  d3dlr;
    LPBYTE          pPixel;
    UINT            uStride, uOffset;
    FLOAT           u, v;
    FLOAT           fDelta[2];
    FLOAT           fChannel[4];
    UINT            i, j;
    BYTE            a, r, g, b;
    HRESULT         hr;
#ifdef UNDER_XBOX
    Swizzler        swz(dwWidth, dwHeight, 1);
#endif

    if (!pData)
    {
        return NULL;
    }

    pd3dt = (CTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, fmt);

    if (!pd3dt) {
        return NULL;
    }

#ifdef UNDER_XBOX
    swz.SetU(0);
    swz.SetV(0);
#endif

    hr = pd3dt->LockRect(0, &d3dlr, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"))) {
        pd3dt->Release();
        return NULL;
    }

    pPixel = (LPBYTE)d3dlr.pBits;

    switch (fmt) {
        case D3DFMT_L6V5U5:
        case D3DFMT_V8U8:
            uStride = 2;
            break;
        case D3DFMT_X8L8V8U8:
        case D3DFMT_Q8W8V8U8:
        case D3DFMT_V16U16:
        case D3DFMT_W11V11U10:
            uStride = 4;
            break;
    }

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
            uOffset = i;
#else
            uOffset = swz.Get2D();
            swz.IncU();
#endif // UNDER_XBOX

            switch (fmt) {
                case D3DFMT_X8L8V8U8:
                    *((LPDWORD)pPixel + uOffset) = b << 16 | g << 8 | r;
                    break;
                case D3DFMT_L6V5U5:
                    *((LPWORD)pPixel + uOffset) = (b & 0xFC) << 8 | (g & 0xF8) << 2 | r >> 3;
                    break;
                case D3DFMT_V8U8:
                    *((LPWORD)pPixel + uOffset) = g << 8 | r;
                    break;
                case D3DFMT_Q8W8V8U8:
                    *((LPDWORD)pPixel + uOffset) = a << 24 | b << 16 | g << 8 | r;
                    break;
                case D3DFMT_V16U16:
                    *((LPDWORD)pPixel + uOffset) = g << 24 | r << 8;
                    break;
                case D3DFMT_W11V11U10:
                    *((LPDWORD)pPixel + uOffset) = b << 24 | g << 13 | r << 2;
                    break;
            }

        }
#ifndef UNDER_XBOX
        pPixel += d3dlr.Pitch;
#else
        swz.IncV();
#endif // UNDER_XBOX
    }

    hr = pd3dt->UnlockRect(0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::UnlockRect"))) {
        pd3dt->Release();
        return NULL;
    }

    return pd3dt;
}
