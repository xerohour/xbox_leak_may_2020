// FILE:        luminance.cpp
// DESC:        luminance class methods for [alpha]luminance texture format conformance tests
// AUTHOR:      Todd M. Frost

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "CShapesFVF.h"
#include "flags.h"
#include "luminance.h"

//CD3DWindowFramework App;

UINT g_uBumps = (UINT) DEFAULT_BUMPS; // global helper variable for fAlpha(), fLuminance()

// NAME:        fAlpha()
// DESC:        alpha
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      f = sin(u)*sin(v)
// COMMENTS:    (u,v) scaled from [0,0]x[1,1] to [-g_uBumps*pi/2,g_uBumps*pi/2]
//              [-1,1] maps to [0,255] in LoadChannelData() (CIL_UNSIGNED)

FLOAT fAlpha(FLOAT u, FLOAT v)
{
    FLOAT fTemp, x, y;

    fTemp = (FLOAT) (g_uBumps*pi);
    x = fTemp*(u - 0.5f);
    y = fTemp*(v - 0.5f);

    return (FLOAT) (sin(x)*sin(y));
}

#define P0 1.0f
#define P1 -1.0f
#define P2 1.0f
#define P3 -1.0f

// NAME:        fLuminance()
// DESC:        luminance
// INPUT:       u....u texture coord
//              v....v texture coord
// OUTPUT:      bilinear blend:
//
//                  (1 - v)[(1 - u)*P0 + u*P3] + v*[(1 - u)*P1 + u*P2]
//
//                    P1 P2
//                  v P0 P3
//                  + u
//
// COMMENTS:    [-1,1] maps to [0,255] in LoadChannelData() (CIL_UNSIGNED)

FLOAT fLuminance(FLOAT u, FLOAT v)
{
    FLOAT s = 1.0f - u;

    return (1 - v)*(s*P0 + u*P3) + v*(s*P1 + u*P2);
}

#undef P0
#undef P1
#undef P2
#undef P3

//******************************************************************************
CTexture8* CreateLumChannelDataTexture(CDevice8* pDevice, DWORD dwWidth, DWORD dwHeight, PCHANNELDATA pData, D3DFORMAT fmt) {

    CTexture8*      pd3dt;
    D3DLOCKED_RECT  d3dlr;
    LPDWORD         pdwPixel;
    LPWORD          pwPixel;
    LPBYTE          pPixel;
    UINT            uStride;
    FLOAT           u, v;
    FLOAT           fDelta[2];
    FLOAT           fChannel[4];
    UINT            i, j;
    BYTE            a, r, g, b;
    BOOL            bBump;
    HRESULT         hr;

    if (!pData)
    {
        return NULL;
    }

    pd3dt = (CTexture8*)CreateTexture(pDevice, dwWidth, dwHeight, fmt);
    if (!pd3dt) {
        return NULL;
    }

    hr = pd3dt->LockRect(0, &d3dlr, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"))) {
        pd3dt->Release();
        return NULL;
    }

    switch (fmt) {
        case D3DFMT_L8 :
            pPixel = (LPBYTE)d3dlr.pBits;
            uStride = d3dlr.Pitch;
            break;
        case D3DFMT_A8L8:
        case D3DFMT_L6V5U5:
            pwPixel = (LPWORD)d3dlr.pBits;
            uStride = d3dlr.Pitch / 2;
            break;
        case D3DFMT_X8L8V8U8:
            pdwPixel = (LPDWORD)d3dlr.pBits;
            uStride = d3dlr.Pitch / 4;
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

            switch (fmt) {
                case D3DFMT_L8 :
                    pPixel[i] = r;
                    break;
                case D3DFMT_A8L8:
                    pwPixel[i] = a << 8 | r;
                    break;
                case D3DFMT_L6V5U5:
                    pwPixel[i] = (a & 0xFC) << 8;
                    break;
                case D3DFMT_X8L8V8U8:
                    pdwPixel[i] = r << 16;
                    break;
            }
        }

        pPixel += uStride;
        pwPixel += uStride;
        pdwPixel += uStride;
    }

    hr = pd3dt->UnlockRect(0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::UnlockRect"))) {
        pd3dt->Release();
        return NULL;
    }

    return pd3dt;
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

    CLuminanceAddress*  LuminanceAddress;
    BOOL                bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    LuminanceAddress = new CLuminanceAddress();
    if (!LuminanceAddress) {
        return FALSE;
    }

    // Initialize the scene
    if (!LuminanceAddress->Create(pDisplay)) {
        LuminanceAddress->Release();
        return FALSE;
    }

    bRet = LuminanceAddress->Exhibit(pnExitCode);

    // Clean up the scene
    LuminanceAddress->Release();

    return bRet;
}

// NAME:        CLuminance()
// DESC:        constructor for luminance class
// INPUT:       none
// OUTPUT:      none

CLuminance::CLuminance(VOID)
{
    D3DFORMAT fmt[] = {
        D3DFMT_L8,
        D3DFMT_A8L8,
        D3DFMT_L6V5U5,
        D3DFMT_X8L8V8U8,
//#ifdef UNDER_XBOX
#if 0 // ##TODO: Add these formats when the new driver comes online
        D3DFMT_AL8,
        D3DFMT_L16,
        D3DFMT_LIN_A8L8,
        D3DFMT_LIN_AL8,
        D3DFMT_LIN_L16,
#endif // UNDER_XBOX
    };

    m_dwModes[0] = DEFAULT_MODE;
    m_dwModes[1] = DEFAULT_MODE;

    m_dwFormat = NOFORMAT;

    m_dwFVF = (DWORD) 0;

    m_dwStages = (DWORD) 1;
    m_dwStagesMax = (DWORD) 0;
    m_dwTexAddressCaps = (DWORD) 0;
    m_dwTexOpCaps = (DWORD) 0;

    m_dwModeCapsVals[0][0] = D3DPTADDRESSCAPS_BORDER;
    m_dwModeCapsVals[0][1] = D3DTADDRESS_BORDER;
    m_dwModeCapsVals[1][0] = D3DPTADDRESSCAPS_CLAMP;
    m_dwModeCapsVals[1][1] = D3DTADDRESS_CLAMP;
    m_dwModeCapsVals[2][0] = D3DPTADDRESSCAPS_MIRROR;
    m_dwModeCapsVals[2][1] = D3DTADDRESS_MIRROR;
    m_dwModeCapsVals[3][0] = D3DPTADDRESSCAPS_WRAP;
    m_dwModeCapsVals[3][1] = D3DTADDRESS_WRAP;

    m_pVertices = NULL;
    
//    m_pBaseImage = NULL;
//    m_pLuminanceImage = NULL;

    m_pBaseTexture = NULL;
    m_pLuminanceTexture = NULL;

//    m_pMaterial = NULL;

//#ifndef UNDER_XBOX
#if 1 // ##TODO: Add additional formats when the new driver comes online
    m_uCommonTextureFormats = 4;
#else
    m_uCommonTextureFormats = 9;
#endif // UNDER_XBOX
    memcpy(m_fmtCommon, fmt, m_uCommonTextureFormats * sizeof(D3DFORMAT));
}

// NAME:        ~CLuminance()
// DESC:        destructor for luminance class
// INPUT:       none
// OUTPUT:      none

CLuminance::~CLuminance(VOID)
{
    if (m_pVertices)
        free(m_pVertices);

//    RELEASE(m_pMaterial);

    ReleaseTexture(m_pLuminanceTexture);
//    RELEASE(m_pLuminanceImage);

    ReleaseTexture(m_pBaseTexture);
//    RELEASE(m_pBaseImage);
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CLuminance::CommandLineHelp(void)
{
//    WriteCommandLineHelp("$ybumps: $wbumps per texture $c(default=8)");
//    WriteCommandLineHelp("$yvalidate: $wvalidate $c(On/$wOff$c)");
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code
// COMMENTS:    material released before returning D3DTESTINIT_SKIPALL or D3DTESTINIT_ABORT
//              RELEASE(m_pMaterial) causes access violations in TestTerminate otherwise

UINT CLuminance::TestInitialize(VOID)
{
    bool bFound;
//    CImageLoader Loader;
    DWORD dwCount, dwFlags, dwFormat, dwWidth = (DWORD) 256, dwHeight = (DWORD) 256;
    DWORD dwColors[4] = {
        RGBA_MAKE( 255, 255, 255, 255),
        RGBA_MAKE( 255,   0,   0, 255),
        RGBA_MAKE(   0, 255,   0, 255),
        RGBA_MAKE(   0,   0, 255, 255)
    };
//    INT nDevice = m_pMode->nSrcDevice;
    UINT i;
    WORD wMaxSimultaneousTextures;

    m_dwModes[0] = DEFAULT_MODE;
    m_dwModes[1] = DEFAULT_MODE;

    m_dwFormat = NOFORMAT;

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

    m_pMode->nTextureFormat = (int) 0;
    dwCount = (DWORD) 0;

    for (dwFormat = 0; dwFormat < m_uCommonTextureFormats; dwFormat++)
        if (dwCount < dwGetCountFromMask(m_pCommonTextureFormats[dwFormat].ddpfPixelFormat.dwGBitMask))
        {
            dwCount = dwGetCountFromMask(m_pCommonTextureFormats[dwFormat].ddpfPixelFormat.dwGBitMask);
            m_pMode->nTextureFormat = (int) dwFormat;
        }
*/

//    if (!(m_pBaseImage = Loader.LoadGradient(dwWidth, dwHeight, dwColors)))
//    {
//        WriteToLog(_T("Unable to load base image."));
//        RELEASE(m_pMaterial);
//        return D3DTESTINIT_ABORT;
//    }

//    if (!(m_pBaseTexture = CreateTexture(dwWidth, dwHeight, CDDS_TEXTURE_VIDEO, m_pBaseImage)))
    if (!(m_pBaseTexture = CreateGradientTexture(m_pDevice, dwWidth, dwHeight, dwColors, D3DFMT_X8R8G8B8)))
    {
        WriteToLog(_T("Unable to create base texture."));
//        RELEASE(m_pMaterial);
        return D3DTESTINIT_ABORT;
    }

    // scan for [alpha]luminance textures
/*
    ResetTextureFormats();

    if (!RequestTextureFormats((DWORD) PF_LUMINANCE))
    {
        WriteToLog(_T("No LUMINANCE texture formats enumerated."));
        RELEASE(m_pMaterial);
        SetTestRange((UINT) 1, (UINT) 1);
        return D3DTESTINIT_SKIPALL;
    }
*/
    m_dwStagesMax = m_d3dcaps.MaxTextureBlendStages;
    wMaxSimultaneousTextures =  (WORD)m_d3dcaps.MaxSimultaneousTextures;
    m_dwStagesMax = (DWORD) ((wMaxSimultaneousTextures < 2) ? 1 : m_dwStagesMax);

    m_dwTexAddressCaps = m_d3dcaps.TextureAddressCaps;
    m_dwTexOpCaps = m_d3dcaps.TextureOpCaps;

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

//    CHANNELDATA Data;
    Data.dwAlpha = (DWORD) CIL_UNSIGNED;
    Data.dwRed = (DWORD) CIL_UNSIGNED;
    Data.dwGreen = (DWORD) 0;
    Data.dwBlue = (DWORD) 0;
    Data.fAlpha = fAlpha;
    Data.fRed = fLuminance;
    Data.fGreen = NULL;
    Data.fBlue = NULL;
/*
    if (!(m_pLuminanceImage = Loader.LoadChannelData(dwWidth, dwHeight, &Data)))
    {
        WriteToLog(_T("Unable to load luminance image."));
        RELEASE(m_pMaterial);
        return D3DTESTINIT_ABORT;
    }
*/
    return D3DTESTINIT_RUN;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CLuminance::ExecuteTest(UINT uTest)
{
    bool bResult = true, bValid = true;
    int iStage;
    CHAR szFormat[MAXBUFFER];
    CHAR szMode[NMODES][7] = {
        "wrap",
        "mirror",
        "clamp",
        "border"
    };
    CHAR szStages[NSTAGES][33] = {
        "1 stage, blend(diffuse:texture)",
        "2 stage, blend(gradient:texture)",
        "3 stage, blend(gradient:texture)"
    };
    D3DCOLOR dwColors[NSTAGES] = {
        RGBA_MAKE(  0,   0, 255, 255),
        RGBA_MAKE(  0, 255,   0, 255),
        RGBA_MAKE(  0, 255, 255,   0)
    };
    D3DTEXTUREOP dwOp = D3DTOP_MODULATE;

//    if (m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat.dwFlags & PF_ALPHAPIXELS)
    if (m_fmtCommon[nTextureFormat] == D3DFMT_A8L8
#ifdef UNDER_XBOX
        || m_fmtCommon[nTextureFormat] == D3DFMT_LIN_A8L8
#endif // UNDER_XBOX
    )
        if (m_dwTexOpCaps & D3DTEXOPCAPS_BLENDTEXTUREALPHA)
            dwOp = D3DTOP_BLENDTEXTUREALPHA;

    // variation independent, texture stage state settings

    for (iStage = 0; iStage < 3; iStage++)
    {
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

        SetTextureStageState(iStage, D3DTSS_BORDERCOLOR, dwColors[iStage]);
        SetTextureStageState(iStage, D3DTSS_MAGFILTER, (DWORD) TEXF_LINEAR);
        SetTextureStageState(iStage, D3DTSS_MINFILTER, (DWORD) TEXF_LINEAR);
        SetTextureStageState(iStage, D3DTSS_MIPFILTER, (DWORD) TEXF_NONE);
        SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_TEXTURE);
        SetTextureStageState(iStage, D3DTSS_COLORARG2, (DWORD) D3DTA_CURRENT);
        SetTextureStageState(iStage, D3DTSS_ALPHAARG1, (DWORD) D3DTA_TEXTURE);
        SetTextureStageState(iStage, D3DTSS_ALPHAARG2, (DWORD) D3DTA_CURRENT);
        SetTexture(iStage, NULL);
    }
    
    // variation dependent, texture stage state settings

    iStage = 0;

    switch (m_dwStages)
    {
        case 1:
            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) dwOp);
            SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 1);
            SetTexture(iStage, m_pLuminanceTexture);
            iStage++;
        break;

        case 2:
            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE);
            SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 0);
            SetTexture(iStage, m_pBaseTexture);
            iStage++;

            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) dwOp);
            SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 1);
            SetTexture(iStage, m_pLuminanceTexture);
            iStage++;
        break;
 
        case 3:
            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 0);
            SetTexture(iStage, m_pBaseTexture);
            iStage++;

            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) dwOp);
            SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG1);
            SetTextureStageState(iStage, D3DTSS_TEXCOORDINDEX, (DWORD) 1);
            SetTexture(iStage, m_pLuminanceTexture);
            iStage++;

            SetTextureStageState(iStage, D3DTSS_COLOROP, (DWORD) D3DTOP_MODULATE);
            SetTextureStageState(iStage, D3DTSS_COLORARG1, (DWORD) D3DTA_DIFFUSE);
            SetTextureStageState(iStage, D3DTSS_ALPHAOP, (DWORD) D3DTOP_SELECTARG2);
            iStage++;
        break;
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

#endif // !UNDER_XBOX

    if (!bValid)
    {
        SkipTests((UINT) 1);
        return true;
    }

//    sprintf(szFormat, "Al%d:Lu%d",
//            dwGetCountFromMask(m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat.dwRGBAlphaBitMask),
//            dwGetCountFromMask(m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat.dwLuminanceBitMask));
    sprintf(szFormat, "%d", m_fmtCommon[nTextureFormat]);
    sprintf(m_szBuffer, "%s, %s (%s,%s)",
             szFormat,
             szStages[m_dwStages - 1],
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

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

    sprintf(m_szBuffer, "$y%s", szStages[m_dwStages - 1]);
    WriteStatus("$gMode", m_szBuffer);
    sprintf(m_szBuffer, "$y%s", szFormat);
    WriteStatus("$gFormat", m_szBuffer);
    sprintf(m_szBuffer, "$y%s", szMode[m_dwModes[0] - 1]);
    WriteStatus("$gU Mode", m_szBuffer);
    sprintf(m_szBuffer, "$y%s", szMode[m_dwModes[1] - 1]);
    WriteStatus("$gV Mode", m_szBuffer);
//    return bResult;
    return false;
}

// NAME:        ProcessFrame()
// DESC:        process frame
// INPUT:       none
// OUTPUT:      bResult..result of src:ref image comparison

bool CLuminance::ProcessFrame(void)
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

bool CLuminance::TestTerminate(VOID)
{
//    RELEASE(m_pMaterial);

    ReleaseTexture(m_pLuminanceTexture);
    m_pLuminanceTexture = NULL;
//    RELEASE(m_pLuminanceImage);

    ReleaseTexture(m_pBaseTexture);
    m_pBaseTexture = NULL;
//    RELEASE(m_pBaseImage);
    return true;
}

// NAME:        SetDefaultLightStates()
// DESC:        set default light states
// INPUT:       none
// OUTPUT:      true.....if SetLightState() succeeds
//              false....otherwise

bool CLuminance::SetDefaultLightStates(VOID)
{
    return SetRenderState(D3DRS_AMBIENT, RGBA_MAKE(164, 164, 164, 255));
}

// NAME:        SetDefaultLights()
// DESC:        set default lights
// INPUT:       none
// OUTPUT:      true.....if default lights set successfully
//              false....otherwise

bool CLuminance::SetDefaultLights(VOID)
{
//    bool bSet;
//    CLight *pLight = CreateLight();
    D3DLIGHT8 Data;
    HRESULT hr;

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

bool CLuminance::SetDefaultMaterials(VOID)
{
//    bool bSet;
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

bool CLuminance::SetDefaultMatrices(VOID)
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

// NAME:        bSetLuminance()
// DESC:        set luminance map texture
// INPUT:       dwFormat.....texture format
//              dwWidth......texture width
//              dwHeight.....texture height
// OUTPUT:      true.....if requested format is set successfully
//              false....otherwise

bool CLuminance::bSetLuminance(DWORD dwFormat, DWORD dwWidth, DWORD dwHeight)
{
    DWORD dwFlags;

    if (m_dwFormat == dwFormat)
        return true;

    if (dwFormat >= m_uCommonTextureFormats)
        return false;

    ReleaseTexture(m_pLuminanceTexture);
//    dwFlags = CDDS_TEXTURE_VIDEO;
    nTextureFormat = dwFormat;
//    m_pLuminanceTexture = CreateTexture(dwWidth, dwHeight, dwFlags, m_pLuminanceImage);
//    m_pLuminanceTexture = (CTexture8*)CreateChannelDataTexture(m_pDevice, dwWidth, dwHeight, &Data, m_fmtCommon[nTextureFormat]);
    // Using custom create function due to a bug in D3DX conversion blits on luminance formats
    m_pLuminanceTexture = (CTexture8*)CreateLumChannelDataTexture(m_pDevice, dwWidth, dwHeight, &Data, m_fmtCommon[nTextureFormat]);

    if (!m_pLuminanceTexture)
    {
        WriteToLog(_T("Unable to create [alpha]luminance texture."));
        return false;
    }

    m_dwFormat = dwFormat;
    return true;
}

// NAME:        dwGetCountFromMask()
// DESC:        get count from mask
// INPUT:       dwMask...bit mask
// OUTPUT:      dwCount...bit count

DWORD CLuminance::dwGetCountFromMask(DWORD dwMask)
{
    DWORD dwCount, dwTemp;

    for (dwCount = (DWORD) 0, dwTemp = dwMask; dwTemp; dwTemp >>= 1) if (dwTemp & 1) dwCount++;
    return dwCount;
}

// NAME:        vProcessArgs()
// DESC:        process args
// INPUT:       none
// OUTPUT:      none

VOID CLuminance::vProcessArgs(VOID)
{
    int iDefault, iResult;

    if (KeySet("WHQL"))
    {
        m_Flags.vSet(VALIDATE);
        g_uBumps = (UINT) DEFAULT_BUMPS;
        return;
    }

    // bumps
    // default:  DEFAULT_BUMPS

    iDefault = (int) DEFAULT_BUMPS;
    ReadInteger("bumps", iDefault, &iResult);

    if (iResult < MIN_BUMPS)
        g_uBumps = (UINT) MIN_BUMPS;
    else if (iResult > MAX_BUMPS)
        g_uBumps = (UINT) MAX_BUMPS;
    else
        g_uBumps = (UINT) iResult;

    // validate
    // default:  true

    if (!KeySet("!validate"))
        m_Flags.vSet(VALIDATE);
}
