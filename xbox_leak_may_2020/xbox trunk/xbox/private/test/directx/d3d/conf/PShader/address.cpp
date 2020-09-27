// FILE:        address.cpp
// DESC:        pixel shader address conformance tests
// AUTHOR:      Cliff Hudson

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "PShader.h"

//CPSAddress PSAddressTest;

extern MYTLVERTEX g_FourColorVertices[];
extern MYTLVERTEX g_TexKillVertices[];
extern MYTLVERTEX g_IndexARGBVertices[];
extern MYCUBETLVERTEX2 g_Cube2Vertices[];
extern MYCUBETLVERTEX3 g_Cube3Vertices[];
extern MYCUBETLVERTEX2 g_Texm2Vertices[];
extern MYCUBETLVERTEX3 g_Texm3Vertices[];

CTexture8* CreateBumpMapTexture(CDevice8* pDevice, LPTSTR szName);

CPSAddress::CPSAddress()
{
    m_szTestName = TEXT("PSAddress");
    m_szCommandKey = TEXT("PSAddress");

//    m_pCheckerData = NULL;
    m_pCheckerTex = NULL;
//    m_pColorCheckerData = NULL;
    m_pColorCheckerTex = NULL;
//    m_pBEM1Data = NULL;
    m_pBEM1Tex = NULL;
//    m_pBEM3Data = NULL;
    m_pBEM3Tex = NULL;
//    m_pBrickData = NULL;
    m_pBrickTex = NULL;
    m_pCubeTex = NULL;
    m_pSysCubeTex = NULL;
//    m_pCubeData[0] = NULL;
//    m_pCubeData[1] = NULL;
//    m_pCubeData[2] = NULL;
//    m_pCubeData[3] = NULL;
//    m_pCubeData[4] = NULL;
//    m_pCubeData[5] = NULL;
}

bool CPSAddress::TestTerminate()
{
    ReleaseTexture(m_pCheckerTex);
    m_pCheckerTex = NULL;
//    RELEASE(m_pCheckerData);
    ReleaseTexture(m_pColorCheckerTex);
    m_pColorCheckerTex = NULL;
//    RELEASE(m_pColorCheckerData);
//    RELEASE(m_pBEM1Data);
    ReleaseTexture(m_pBEM1Tex);
    m_pBEM1Tex = NULL;
//    RELEASE(m_pBEM3Data);
    ReleaseTexture(m_pBEM3Tex);
    m_pBEM3Tex = NULL;
    ReleaseTexture(m_pBrickTex);
    m_pBrickTex = NULL;
//    RELEASE(m_pBrickData);
    ReleaseTexture(m_pCubeTex); 
    m_pCubeTex = NULL;
    ReleaseTexture(m_pSysCubeTex); 
    m_pSysCubeTex = NULL;
//    RELEASE(m_pCubeData[0]); 
//    RELEASE(m_pCubeData[1]); 
//    RELEASE(m_pCubeData[2]); 
//    RELEASE(m_pCubeData[3]); 
//    RELEASE(m_pCubeData[4]); 
//    RELEASE(m_pCubeData[5]); 
    return CPShader::TestTerminate();
}

UINT CPSAddress::TestInitialize()
{
    CPShader::TestInitialize();
        
    if (!LoadTextures())
        return D3DTESTINIT_ABORT;

    PopulateShaderInfo();
    SetTestRange(1, m_uNumShaders);

//    if (m_bSkipAllForCaps)
//        SkipTestRange(1, m_uNumShaders);

    return D3DTESTINIT_RUN;
}

void CPSAddress::PopulateShaderInfo()
{
    float fConstant[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

    // ****************************** add ******************************
	// (1) tex
    AddShaderDef("Address: tex",
                 "tex t0\n"
                 "mov r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(0, 5));
    AddTextureToLastShaderDef(m_pColorCheckerTex);


	// (2) texcoord
    AddShaderDef("Address: texcoord",
                 "texcoord t0\n"
                 "mov r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);

    // (3) texkill
	AddShaderDef("Address: texkill",
                 "texkill t0\n"
                 "mov r0, v0\n",
                 MYTLVERTEX_FVF, g_TexKillVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);

    // (4) texbem
	AddShaderDef("Address: texbem",
                 "tex t0\n"
                 "texbem t1, t0\n"
                 "mov r0, t1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
VerifyTexture(m_pBEM1Tex);
    AddTextureToLastShaderDef(m_pBEM1Tex);
VerifyTexture(m_pBEM1Tex);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    
    // (5) texbeml
	AddShaderDef("Address: texbeml",
                 "tex t0\n"
                 "texbeml t1, t0\n"
                 "mov r0, t1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
VerifyTexture(m_pBEM1Tex);
    AddTextureToLastShaderDef(m_pBEM1Tex);
VerifyTexture(m_pBEM1Tex);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
   
    // (6) texreg2ar
	AddShaderDef("Address: texreg2ar",
				 "tex t0\n"
				 "texreg2ar t1, t0\n"
                 "mov r0, t1\n",
                 MYTLVERTEX_FVF, g_IndexARGBVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
VerifyTexture(m_pBEM1Tex);
    AddTextureToLastShaderDef(m_pBEM1Tex);
VerifyTexture(m_pBEM1Tex);

    // (7) texreg2gb
	AddShaderDef("Address: texreg2gb",
				 "tex t0\n"
                 "texreg2gb t1, t0\n"
                 "mov r0, t1\n",
                 MYTLVERTEX_FVF, g_IndexARGBVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
VerifyTexture(m_pBEM1Tex);
    AddTextureToLastShaderDef(m_pBEM1Tex);
VerifyTexture(m_pBEM1Tex);

    // (8) texm3x2tex
	AddShaderDef("Address: texm3x2tex",
                 "tex t0\n"
                 "texm3x2pad t1, t0\n"
                 "texm3x2tex t2, t0\n"
                 "mov r0, t2\n",
                 MYCUBETLVERTEX2_FVF, g_Texm2Vertices);
    AddTextureToLastShaderDef(m_pBrickTex);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(m_pColorCheckerTex);

    // (9) texm3x3tex
	AddShaderDef("Address: texm3x3tex",
                 "tex t0\n"
                 "texm3x3pad t1, t0\n"
                 "texm3x3pad t2, t0\n"
                 "texm3x3tex t3, t0\n"
                 "dp3 r0, t3, v0\n",
                 MYCUBETLVERTEX3_FVF, g_Cube3Vertices);
VerifyTexture(m_pBEM1Tex);
    AddTextureToLastShaderDef(m_pBEM1Tex);
VerifyTexture(m_pBEM1Tex);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(m_pCubeTex);

    // (10) texm3x3spec
	AddShaderDef("Address: texm3x3spec",
                 "tex t0\n"
                 "texm3x3pad t1, t0\n"
                 "texm3x3pad t2, t0\n"
                 "texm3x3spec t3, t0, c0\n"
                 "mov r0, t3\n",
                 MYCUBETLVERTEX3_FVF, g_Cube3Vertices);
VerifyTexture(m_pBEM1Tex);
    AddTextureToLastShaderDef(m_pBEM1Tex);
VerifyTexture(m_pBEM1Tex);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(m_pCubeTex);
    AddConstantsToLastShaderDef(fConstant, 4);

    // (11) texm3x3diff and texm3x3spec
	AddShaderDef("Address - texm3x3diff and texm3x3spec",
                 "tex t0\n"
                 "texm3x3pad t1, t0\n"
                 "texm3x3diff t2, t0\n"
                 "texm3x3spec t3, t0, c0\n"
                 "mov r0.rgb, t3\n"
                 "mov r0.a, t2\n",
                 MYCUBETLVERTEX3_FVF, g_Cube3Vertices, PSVERSION(255, 255));
VerifyTexture(m_pBEM1Tex);
    AddTextureToLastShaderDef(m_pBEM1Tex);
VerifyTexture(m_pBEM1Tex);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(m_pCubeTex);
    AddTextureToLastShaderDef(m_pCubeTex);
    AddConstantsToLastShaderDef(fConstant, 4);
}

bool CPSAddress::LoadTextures()
{
//    CImageLoader Loader;

//    if (NULL == (m_pCheckerData = Loader.Load(CIL_BMP, "checker.bmp", 1.0f)))
//    {
//        WriteToLog("(ERROR) Failed to load checker.bmp.  Aborting.\n");
//        return false;
//    }

//    if (NULL == (m_pCheckerTex = CreateTexture(64, 64, CDDS_TEXTURE_VIDEO, m_pCheckerData)))
    if (NULL == (m_pCheckerTex = (CTexture8*)CreateTexture(m_pDevice, TEXT("checker.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, POOL_DEFAULT, 64, 64)))
    {
        WriteToLog("(ERROR) Failed to create checker texture.  Aborting.\n");
        return false; 
    }

//    if (NULL == (m_pColorCheckerData = Loader.Load(CIL_BMP, "colorchecker.bmp", 1.0f)))
//    {
//        WriteToLog("(ERROR) Failed to load Colorchecker.bmp.  Aborting.\n");
//        return false;
//    }

//    if (NULL == (m_pColorCheckerTex = CreateTexture(64, 64, CDDS_TEXTURE_VIDEO, m_pColorCheckerData)))
    if (NULL == (m_pColorCheckerTex = (CTexture8*)CreateTexture(m_pDevice, TEXT("colorchecker.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, POOL_DEFAULT, 64, 64)))
    {
        WriteToLog("(ERROR) Failed to create color checker texture.  Aborting.\n");
        return false; 
    }

	// Bricks
//	if (NULL == (m_pBrickData = GenerateBrickTexture()))
//    {
//        WriteToLog("(ERROR) Failed to generate dp3 data.  Aborting.\n");
//        return false;
//    }
    if (NULL == (m_pBrickTex = GenerateBrickTexture()))
    {
        WriteToLog("(ERROR) Failed to create DP3 texture.  Aborting.\n");
        return false;
    }

/*
    m_pCubeData[0] = Loader.Load(CIL_BMP, "skyxpos.bmp", 1.0f);
    m_pCubeData[1] = Loader.Load(CIL_BMP, "skyxneg.bmp", 1.0f);
    m_pCubeData[2] = Loader.Load(CIL_BMP, "skyypos.bmp", 1.0f);
    m_pCubeData[3] = Loader.Load(CIL_BMP, "skyyneg.bmp", 1.0f);
    m_pCubeData[4] = Loader.Load(CIL_BMP, "skyzpos.bmp", 1.0f);
    m_pCubeData[5] = Loader.Load(CIL_BMP, "skyzneg.bmp", 1.0f);

    if (!(m_pCubeData[0] && m_pCubeData[1] && m_pCubeData[2] && m_pCubeData[3] && m_pCubeData[4] && m_pCubeData[5]))
    {
        WriteToLog("(ERROR) Failed to load cube map data.  Aborting.\n");
        return false;
    }

    m_pMode->nCubeFormat = 0;

    if (NULL == (m_pCubeTex = CreateCubicTexture(512, 512, 1, CDDS_TEXTURE_VIDEO | CDDS_TEXTURE_ALLFACES)))
    {
        WriteToLog("(ERROR) Failed to create cubemap (%08X).  Aborting.\n", GetLastError());
        return false;
    }
    
    if (NULL == (m_pSysCubeTex = CreateCubicTexture(512, 512, 1, CDDS_TEXTURE_SYSTEM | CDDS_TEXTURE_ALLFACES | CDDS_TEXTURE_LOCK)))
    {
        WriteToLog("(ERROR) Failed to create system memory cubemap (%08X).  Aborting.\n", GetLastError());
        return false;
    }
    
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < m_pSysCubeTex->m_pSrcSurface->GetNumAttachedSurfaces(); j++)
        {
            CDirectDrawSurface * pSurface = m_pSysCubeTex->m_pSrcSurface->GetCubicSurface(i)->GetAttachedSurface(j);
            pSurface->SetImageData(m_pCubeData[j]);
            RELEASE(pSurface);

            if (NULL != m_pRefTarget)
            {
                CDirectDrawSurface * pSurface = m_pSysCubeTex->m_pRefSurface->GetCubicSurface(i)->GetAttachedSurface(j);
                pSurface->SetImageData(m_pCubeData[j]);
                RELEASE(pSurface);
            }
        }
    } 
    
    if (!UpdateCubicTexture8(m_pSysCubeTex, m_pCubeTex))
        return false;
*/
    if (NULL == (m_pCubeTex = (CTexture8*)CreateTexture(m_pDevice, TEXT("sky0.bmp"), D3DFMT_A8R8G8B8, TTYPE_CUBEMAP, NULL, 1.0f, COLORKEY_NONE, POOL_DEFAULT, 512, 512)))
    {
        WriteToLog("(ERROR) Failed to create cubemap.  Aborting.\n");
        return false;
    }
     
//    ResetTextureFormats();

//    if (!RequestTextureFormats(DDPF_BUMPLUMINANCE, DDPF_RGB))
//    {
//        SkipTestRange(4, 4);
//        WriteToLog("(WARNING) No common BumpDuDv formats.\n");
//    }
//    else
//    {
//        m_pMode->nTextureFormat = 0;

//        if (NULL == (m_pBEM1Data = Loader.Load(CIL_BMP, "bem1.bmp", 1.0f)))
//        {
//            WriteToLog("(ERROR) Failed to load bem1.bmp.  Aborting.\n");
//            return false;
//        }
    
//        if (NULL == (m_pBEM1Tex = CreateTexture(64, 64, CDDS_TEXTURE_VIDEO, m_pBEM1Data)))

//        if (NULL == (m_pBEM1Tex = (CTexture8*)CreateTexture(m_pDevice, TEXT("bem1.bmp"), D3DFMT_V8U8, TTYPE_BUMPMAP, NULL, 1.0f, COLORKEY_NONE, POOL_DEFAULT, 64, 64)))
        if (NULL == (m_pBEM1Tex = CreateBumpMapTexture(m_pDevice, TEXT("bem1.bmp"))))
        {
            WriteToLog("(ERROR) Failed to create bump environment map texture.  Aborting.\n");
            return false; 
        }
VerifyTexture(m_pBEM1Tex);


//        if (NULL == (m_pBEM3Data = Loader.Load(CIL_BMP, "bem3.bmp", 1.0f)))
//        {
//            WriteToLog("(ERROR) Failed to load bem3.bmp.  Aborting.\n");
//            return false;
//        }
    
//        if (NULL == (m_pBEM3Tex = CreateTexture(128,128, CDDS_TEXTURE_VIDEO, m_pBEM1Data)))
//        if (NULL == (m_pBEM3Tex = (CTexture8*)CreateTexture(m_pDevice, TEXT("bem3.bmp"), D3DFMT_V8U8, TTYPE_BUMPMAP, NULL, 1.0f, COLORKEY_NONE, POOL_DEFAULT, 128, 128)))
        if (NULL == (m_pBEM3Tex = CreateBumpMapTexture(m_pDevice, TEXT("bem3.bmp"))))
        {
            WriteToLog("(ERROR) Failed to create second bump environment map texture.  Aborting.\n");
            return false; 
        }
//    }
 
    return true;
}

bool CPSAddress::ExecuteTest(UINT uiTest)
{
    if (CPShader::ExecuteTest(uiTest))
    {
        SetTextureStageState(1, D3DTSS_BUMPENVMAT00, F2DW(1.0f));
        SetTextureStageState(1, D3DTSS_BUMPENVMAT01, F2DW(0.0f));
        SetTextureStageState(1, D3DTSS_BUMPENVMAT10, F2DW(0.0f));
        SetTextureStageState(1, D3DTSS_BUMPENVMAT11, F2DW(1.0f));
        SetTextureStageState(1, D3DTSS_BUMPENVLSCALE, F2DW(1.0f));
        SetTextureStageState(1, D3DTSS_BUMPENVLOFFSET, F2DW(0.0f));
        return true;
    }

    return false;
}

void CPSAddress::CommandLineHelp()
{
}

//******************************************************************************
CTexture8* CreateBumpMapTexture(CDevice8* pDevice, LPTSTR szName) {

    CTexture8* pd3dtSrc, *pd3dtDst;
    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT d3dlrSrc, d3dlrDst;
    LPDWORD pdwSrc;
    LPWORD pwDst;
    UINT i, j;

    pd3dtSrc = (CTexture8*)CreateTexture(pDevice, szName, D3DFMT_A8R8G8B8);
    if (!pd3dtSrc) {
        return NULL;
    }
    
    pd3dtSrc->GetLevelDesc(0, &d3dsd);

    pd3dtDst = (CTexture8*)CreateTexture(pDevice, d3dsd.Width, d3dsd.Height, D3DFMT_L6V5U5);
    if (!pd3dtDst) {
        ReleaseTexture(pd3dtSrc);
        return NULL;
    }

    pd3dtSrc->LockRect(0, &d3dlrSrc, NULL, 0);
    pd3dtDst->LockRect(0, &d3dlrDst, NULL, 0);

    pdwSrc = (LPDWORD)d3dlrSrc.pBits;
    pwDst = (LPWORD)d3dlrDst.pBits;

    for (i = 0; i < d3dsd.Height; i++) {

        for (j = 0; j < d3dsd.Width; j++) {

            pwDst[j] = (WORD)(((pdwSrc[j] & 0xFC) << 8) |
                       (((pdwSrc[j] >> 8) & 0xF8) << 2) |
                       (((pdwSrc[j] >> 16) & 0xF8) >> 3));
        }

        pdwSrc += d3dlrSrc.Pitch / 4;
        pwDst += d3dlrDst.Pitch / 2;
    }

    pd3dtSrc->UnlockRect(0);
    pd3dtDst->UnlockRect(0);
    ReleaseTexture(pd3dtSrc);

    return pd3dtDst;
}
