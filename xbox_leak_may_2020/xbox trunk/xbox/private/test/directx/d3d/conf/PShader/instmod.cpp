// FILE:        instmod.cpp
// DESC:        pixel shader inst mod conformance tests
// AUTHOR:      Cliff Hudson

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "PShader.h"
//#include "CImageLoader.h"

//CPSInstMod PSInstModTest;

extern MYTLVERTEX g_FourColorVertices[];

CPSInstMod::CPSInstMod()
{
    m_szTestName = TEXT("PSInstMod");
    m_szCommandKey = TEXT("PSInstMod");

//    m_pDXLogoData = NULL;
    m_pDXLogoTex = NULL;
}

bool CPSInstMod::TestTerminate()
{
    ReleaseTexture(m_pDXLogoTex);
//    RELEASE(m_pDXLogoData);
    return CPShader::TestTerminate();
}

UINT CPSInstMod::TestInitialize()
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

void CPSInstMod::PopulateShaderInfo()
{
    float fConstants[4] = { 0.8f, 0.8f, 0.8f, 0.8f };

    // (1) _x2
	AddShaderDef("Instruction Mod - _x2",
                 "tex t0\n"
                 "mov_x2 r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pDXLogoTex);

    // (2) _x4
    AddShaderDef("Instruction Mod - _x4",
                 "tex t0\n"
                 "mov_x4 r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pDXLogoTex);

    // (3) _d2
    AddShaderDef("Instruction Mod - _d2",
                 "tex t0\n"
                 "mov_d2 r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pDXLogoTex);

#ifdef UNDER_XBOX

    if (m_pDisplay->GetConnectionStatus() == SERVER_CONNECTED) {
        return;
    }

    // (4) _bx2
    AddShaderDef("Instruction Mod - _bx2",
                 "mul_bx2 r1, v0, c0\n"
				 "mov r0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(6, 9));
    AddConstantsToLastShaderDef(fConstants, 8);


/*
    AddShaderDef("Instruction Mod - _x8",
                 "tex t0\n"
                 "mov_x8 r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pDXLogoTex);

    AddShaderDef("Instruction Mod - _d4",
                 "tex t0\n"
                 "mov_d4 r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pDXLogoTex);

    AddShaderDef("Instruction Mod - _d8",
                 "tex t0\n"
                 "mov_d8 r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pDXLogoTex);
*/
    AddShaderDef("Instruction Mod - Bias",
                 "tex t0\n"
                 "mov_bias r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(6,9));
    AddTextureToLastShaderDef(m_pDXLogoTex);
/*
    AddShaderDef("Instruction Mod - Unbias",
                 "tex t0\n"
                 "mov_unbias r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pDXLogoTex);
*/
#endif // UNDER_XBOX
}

bool CPSInstMod::LoadTextures()
{
//    CImageLoader Loader;
    
//    if (NULL == (m_pDXLogoData = Loader.Load(CIL_BMP, "dxlogo.bmp", 1.0f)))
//    {
//        WriteToLog("(ERROR) Failed to load dxlogo.bmp.  Aborting.\n");
//        return false;
//    }
    
//    if (NULL == (m_pDXLogoTex = CreateTexture(64, 64, CDDS_TEXTURE_VIDEO, m_pDXLogoData)))
    if (NULL == (m_pDXLogoTex = (CTexture8*)CreateTexture(m_pDevice, TEXT("dxlogo.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, POOL_DEFAULT, 64, 64)))
    {
        WriteToLog("(ERROR) Failed to create DXLogo texture.  Aborting.\n");
        return false;
    }
    
    return true;
}

bool CPSInstMod::ExecuteTest(UINT uiTest)
{
    return CPShader::ExecuteTest(uiTest);
}

void CPSInstMod::CommandLineHelp()
{
}
