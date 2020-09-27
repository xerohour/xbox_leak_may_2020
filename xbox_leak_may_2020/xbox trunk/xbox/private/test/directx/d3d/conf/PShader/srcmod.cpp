// FILE:        srcmod.cpp
// DESC:        pixel shader src mod conformance tests
// AUTHOR:      Cliff Hudson

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "PShader.h"
//#include "CImageLoader.h"

//CPSSrcMod PSSrcModTest;

extern MYTLVERTEX g_FourColorAlphaVertices[];
extern MYTLVERTEX g_FourColorVertices[];

CPSSrcMod::CPSSrcMod()
{
    m_szTestName = TEXT("PSSrcMod");
    m_szCommandKey = TEXT("PSSrcMod");

//    m_pCheckerData = NULL;
    m_pCheckerTex = NULL;
    m_pColorCheckerTex = NULL;
//    m_pColorCheckerData = NULL;
    m_pDXLogoTex = NULL;
//    m_pDXLogoData = NULL;
    m_pDP3Tex = NULL;
//    m_pDP3Data = NULL;
}

bool CPSSrcMod::TestTerminate()
{
    ReleaseTexture(m_pCheckerTex);
//    RELEASE(m_pCheckerData);
    ReleaseTexture(m_pColorCheckerTex);
//    RELEASE(m_pColorCheckerData);
    ReleaseTexture(m_pDXLogoTex);
//    RELEASE(m_pDXLogoData);
    ReleaseTexture(m_pDP3Tex);
//    RELEASE(m_pDP3Data);
    return CPShader::TestTerminate();
}

UINT CPSSrcMod::TestInitialize()
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

void CPSSrcMod::PopulateShaderInfo()
{
    float fConstants[24] =
        { 0.0f, 0.0f, 0.0f, 0.0f,
          0.2f, 0.2f, 0.2f, 0.2f,
          0.4f, 0.4f, 0.4f, 0.4f,
          0.6f, 0.6f, 0.6f, 0.6f,
          0.8f, 0.8f, 0.8f, 0.8f,
          1.0f, 1.0f, 1.0f, 1.0f
        };

    AddShaderDef("Source Negate - TEXTURE 0",
                 "tex t0\n"
                 "mov r0, -t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);

    AddShaderDef("Source Negate - TEXTURE 1",
                 "tex t1\n"
                 "mov r0, -t1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);

    AddShaderDef("Source Negate - TEXTURE 2",
                 "tex t2\n"
                 "mov r0, -t2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);

    AddShaderDef("Source Negate - TEXTURE 3",
                 "tex t3\n"
                 "mov r0, -t3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);

    AddShaderDef("Source Negate - INPUT 0",
                 "mov r0, -v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);

    AddShaderDef("Source Negate - INPUT 1",
                 "mov r0, -v1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);

    AddShaderDef("Source Negate - CONSTANT 0",
                 "mov r0, -c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

    AddShaderDef("Source Negate - CONSTANT 1",
                 "mov r0, -c1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

    AddShaderDef("Source Negate - CONSTANT 2",
                 "mov r0, -c2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

    AddShaderDef("Source Negate - CONSTANT 3",
                 "mov r0, -c3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

    AddShaderDef("Source Negate - CONSTANT 4",
                 "mov r0, -c4\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

    AddShaderDef("Source Negate - CONSTANT 5",
                 "mov r0, -c5\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);
    
    AddShaderDef("Invert - TEXTURE 0",
                 "tex t0\n"
                 "mov r0, 1-t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);

    AddShaderDef("Invert - TEXTURE 1",
                 "tex t1\n"
                 "mov r0, 1-t1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);

    AddShaderDef("Invert - TEXTURE 2",
                 "tex t2\n"
                 "mov r0, 1-t2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);

    AddShaderDef("Invert - TEXTURE 3",
                 "tex t3\n"
                 "mov r0, 1-t3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);

    AddShaderDef("Invert - INPUT 0",
                 "mov r0, 1-v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);

    AddShaderDef("Invert - INPUT 1",
                 "mov r0, 1-v1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);

    AddShaderDef("Invert - CONSTANT 0",
                 "mov r0, 1-c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

    AddShaderDef("Invert - CONSTANT 1",
                 "mov r0, 1-c1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

    AddShaderDef("Invert - CONSTANT 2",
                 "mov r0, 1-c2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

    AddShaderDef("Invert - CONSTANT 3",
                 "mov r0, 1-c3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

    AddShaderDef("Invert - CONSTANT 4",
                 "mov r0, 1-c4\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

    AddShaderDef("Invert - CONSTANT 5",
                 "mov r0, 1-c5\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

    AddShaderDef("Alpha Replicate",
                 "tex t0\n"
                 "mov r0, t0.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);
}

bool CPSSrcMod::LoadTextures()
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

//    if (NULL == (m_pDXLogoData = Loader.Load(CIL_BMP, "DXLogo.bmp", 1.0f)))
//    {
//        WriteToLog("(ERROR) Failed to load DXLogo.bmp.  Aborting.\n");
//        return false;
//    }

//    if (NULL == (m_pDXLogoTex = CreateTexture(64, 64, CDDS_TEXTURE_VIDEO, m_pDXLogoData)))
    if (NULL == (m_pDXLogoTex = (CTexture8*)CreateTexture(m_pDevice, TEXT("dxlogo.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, POOL_DEFAULT, 64, 64)))
    {
        WriteToLog("(ERROR) Failed to create DX Logo texture.  Aborting.\n");
        return false;
    }

//    if (NULL == (m_pDP3Data = Loader.Load(CIL_BMP, "DP3.bmp", 1.0f)))
//    {
//        WriteToLog("(ERROR) Failed to load DP3.bmp.  Aborting.\n");
//        return false;
//    }

//    if (NULL == (m_pDP3Tex = CreateTexture(64, 64, CDDS_TEXTURE_VIDEO, m_pDP3Data)))
    if (NULL == (m_pDP3Tex = (CTexture8*)CreateTexture(m_pDevice, TEXT("dp3.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, POOL_DEFAULT, 64, 64)))
    {
        WriteToLog("(ERROR) Failed to create DP3 texture.  Aborting.\n");
        return false;
    } 
    
    return true;
}

bool CPSSrcMod::ExecuteTest(UINT uiTest)
{
    return CPShader::ExecuteTest(uiTest);
}

void CPSSrcMod::CommandLineHelp()
{
}
