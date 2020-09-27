// FILE:        dstmod.cpp
// DESC:        pixel shader dst mod conformance tests
// AUTHOR:      Cliff Hudson

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "PShader.h"
//#include "CImageLoader.h"

//CPSDestMod PSDestModTest;

extern MYTLVERTEX g_FourColorVertices[];

CPSDestMod::CPSDestMod()
{
    m_szTestName = TEXT("PSDestMod");
    m_szCommandKey = TEXT("PSDestMod");

//    m_pCheckerData = NULL;
    m_pCheckerTex = NULL;
    m_pColorCheckerTex = NULL;
//    m_pColorCheckerData = NULL;
    m_pDXLogoTex = NULL;
//    m_pDXLogoData = NULL;
    m_pDP3Tex = NULL;
//    m_pDP3Data = NULL;
}

bool CPSDestMod::TestTerminate()
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

UINT CPSDestMod::TestInitialize()
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

void CPSDestMod::PopulateShaderInfo()
{
    float fConstants[8] = { 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f };
    
	// (1) Red, Green, Blue
	AddShaderDef("Dest Mask - Red, Green, Blue",
                 "tex t0\n"
                 "mov r0, c0\n"
                 "mov r1.rgb, t0\n"
                 "mul r0.rgb, t0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 8);

    // (2) Alpha
	AddShaderDef("Dest Mask - Alpha",
                 "tex t0\n"
                 "mov r0, t0\n"
                 "mov r0.a, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);

    // (3) Red, Green, Blue, Alpha
	AddShaderDef("Dest Mask - Red, Green, Blue, Alpha",
                 "tex t0\n"
                 "mov r0, c1\n"
                 "mov r1, c0\n"
                 "mov r1.rgb, t0\n"
                 "mov r0.rgba, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 8);

/*
    // (4) Red
	AddShaderDef("Dest Mask - Red",
                 "tex t0\n"
                 "mov r1, c0\n"
                 "mov r1.r, t0\n"
                 "mul r0, t0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);

    // (5) Green
	AddShaderDef("Dest Mask - Green",
                 "tex t0\n"
                 "mov r1, c0\n"
                 "mov r1.g, t0\n"
                 "mul r0, t0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);

    // (6) Blue
	AddShaderDef("Dest Mask - Blue",
                 "tex t0\n"
                 "mov r1, c0\n"
                 "mov r1.b, t0\n"
                 "mul r0, t0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);

    // (7) Red, Green
	AddShaderDef("Dest Mask - Red, Green",
                 "tex t0\n"
                 "mov r1, c0\n"
                 "mov r1.rg, t0\n"
                 "mul r0, t0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);

    // (8) Red, Blue
	AddShaderDef("Dest Mask - Red, Blue",
                 "tex t0\n"
                 "mov r1, c0\n"
                 "mov r1.rb, t0\n"
                 "mul r0, t0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);

    // (9) Green, Blue
	AddShaderDef("Dest Mask - Green, Blue",
                 "tex t0\n"
                 "mov r1, c0\n"
                 "mov r1.gb, t0\n"
                 "mul r0, t0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);

    // (10) Red, Alpha
	AddShaderDef("Dest Mask - Red, Alpha",
                 "tex t0\n"
                 "mov r0, c1\n"
                 "mov r1, c0\n"
                 "mov r1.r, t0\n"
                 "mov r0.ra, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 8);

    // (11) Green, Alpha
	AddShaderDef("Dest Mask - Green, Alpha",
                 "tex t0\n"
                 "mov r0, c1\n"
                 "mov r1, c0\n"
                 "mov r1.g, t0\n"
                 "mov r0.ga, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 8);

    // (12) Blue, Alpha
	AddShaderDef("Dest Mask - Blue, Alpha",
                 "tex t0\n"
                 "mov r0, c1\n"
                 "mov r1, c0\n"
                 "mov r1.b, t0\n"
                 "mov r0.ba, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 8);

    // (13) Red, Green, Alpha
	AddShaderDef("Dest Mask - Red, Green, Alpha",
                 "tex t0\n"
                 "mov r0, c1\n"
                 "mov r1, c0\n"
                 "mov r1.rg, t0\n"
                 "mov r0.rga, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 8);

    // (14) Red, Blue, Alpha
	AddShaderDef("Dest Mask - Red, Blue, Alpha",
                 "tex t0\n"
                 "mov r0, c1\n"
                 "mov r1, c0\n"
                 "mov r1.rb, t0\n"
                 "mov r0.rba, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 8);

    // (15) Green, Blue, Alpha
	AddShaderDef("Dest Mask - Green, Blue, Alpha",
                 "tex t0\n"
                 "mov r0, c1\n"
                 "mov r1, c0\n"
                 "mov r1.gb, t0\n"
                 "mov r0.gba, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 8);
*/

}

bool CPSDestMod::LoadTextures()
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
//    if (NULL == (m_pDP3Tex = (CTexture8*)CreateTexture(m_pDevice, TEXT("DP3.bmp"), D3DFMT_A8R8G8B8, TTYPE_NORMALMAP, NULL, 1.0f, COLORKEY_NONE, POOL_DEFAULT, 64, 64)))
    {
        WriteToLog("(ERROR) Failed to create DP3 texture.  Aborting.\n");
        return false;
    } 
    
    return true;
}

bool CPSDestMod::ExecuteTest(UINT uiTest)
{
    return CPShader::ExecuteTest(uiTest);
}

void CPSDestMod::CommandLineHelp()
{
}
