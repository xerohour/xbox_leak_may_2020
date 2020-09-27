// FILE:        register.cpp
// DESC:        pixel shader reg conformance tests
// AUTHOR:      Cliff Hudson

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "PShader.h"
//#include "CImageLoader.h"

//CPSRegister PSRegisterTest;

extern MYTLVERTEX g_FourColorVertices[];

CPSRegister::CPSRegister()
{
    m_szTestName = TEXT("PSRegister");
    m_szCommandKey = TEXT("PSRegister");

//    m_pCheckerData = NULL;
    m_pCheckerTex = NULL;
    m_pColorCheckerTex = NULL;
//    m_pColorCheckerData = NULL;
    m_pDXLogoTex = NULL;
//    m_pDXLogoData = NULL;
    m_pDP3Tex = NULL;
//    m_pDP3Data = NULL;
}

bool CPSRegister::TestTerminate()
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

UINT CPSRegister::TestInitialize()
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

void CPSRegister::PopulateShaderInfo()
{
    float fConstants[24] =
        {
            0.0f, 0.0f, 0.0f, 0.0f,
            0.2f, 0.2f, 0.2f, 0.2f,  
            0.4f, 0.4f, 0.4f, 0.4f,
            0.6f, 0.6f, 0.6f, 0.6f,
            0.8f, 0.8f, 0.8f, 0.8f,
            1.0f, 1.0f, 1.0f, 1.0f
        };

    // (1) t0
	AddShaderDef("mov from t0",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mov r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);

    // (2) t1
	AddShaderDef("mov from t1",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mov r0, t1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    
    // (3) t2
	AddShaderDef("mov from t2",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mov r0, t2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);

    // (4) t3
	AddShaderDef("mov from t3",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mov r0, t3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);

    // (5) v0
	AddShaderDef("mov from v0",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mov r0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);

    // (6) v1
	AddShaderDef("mov from v1",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mov r0, v1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);

    // (7) t0 + v0
	AddShaderDef("t0 + v0",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
   
    // (8) t0 + c0
	AddShaderDef("t0 + c0",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 4);
    SetConstantsStart(0);
    
    // (9) t0 + c1
    AddShaderDef("t0 + c1",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t0, c1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants + 4, 4);
    SetConstantsStart(1);
     
    // (10) t0 + c2
    AddShaderDef("t0 + c2",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t0, c2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants + 8, 4);
    SetConstantsStart(2);

    // (11) t0 + c3
	AddShaderDef("t0 + c3",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t0, c3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants + 12, 4);
    SetConstantsStart(3);

    // (12) t0 + c4
	AddShaderDef("t0 + c4",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t0, c4\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants + 16, 4);
    SetConstantsStart(4);

    // (13) t0 + c5
	AddShaderDef("t0 + c5",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "add r0, t0, c5\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants + 20, 4);
    SetConstantsStart(5);

    // (14) c1*t0 + c0
	AddShaderDef("c0*t0 + c1",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mad r0, c1, t0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 24);

    // (15) c2*t0 + c0
    AddShaderDef("c2*t0 + c0",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mad r0, c2, t0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 24);

    // (16) c3*t0 + c0
	AddShaderDef("c3*t0 + c0",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mad r0, c3, t0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 24);

    // (17) c4*t0 + c0
	AddShaderDef("c4*t0 + c0",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mad r0, c4, t0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 24);

    // (18) c5*t0 + c0
	AddShaderDef("c5*t0 + c0",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mad r0, c5, t0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 24);

    // (19) c3*v0 + c0
	AddShaderDef("c3*v0 + c0",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mad r0, c3, v0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 24);

    // (20) c3*v1 + c0
    AddShaderDef("c3*v1 + c0",
                 "tex t0\n"
                 "tex t1\n"
                 "tex t2\n"
                 "tex t3\n"
                 "mad r0, c3, v1, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef(m_pColorCheckerTex);
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pDXLogoTex);
    AddTextureToLastShaderDef(m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 24);


    // (21) add to t0 
	AddShaderDef("add to t0",
                 "mov r0, c0\n"
                 "add t0, v0, r0\n"
				 "add r0, r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1,1));
    AddConstantsToLastShaderDef(fConstants, 4);
	
    // (22) sub to t1 
	AddShaderDef("sub to t1",
                 "tex t0\n"
                 "mov r1, c0\n"
                 "sub t1, t0, v0\n"
				 "add r0, r1, t1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1,1));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);

	// (23) mul to t2
	AddShaderDef("mul to t2",
                 "tex t0\n"
                 "mov r0, c0\n"
                 "mul t2, t0, c1\n"
				 "add r0, r0, t2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1,1));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);
    AddConstantsToLastShaderDef(fConstants + 16, 4);

	// (24) lrp to t3
	AddShaderDef("lrp to t3",
                 "tex t0\n"
                 "mov r0, c0\n"
                 "lrp t3, t0, c1, v0\n"
				 "add r0, r0, t3\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1,1));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 4);
    AddConstantsToLastShaderDef(fConstants + 16, 4);
}

bool CPSRegister::LoadTextures()
{
//    CImageLoader    Loader;
    
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

bool CPSRegister::ExecuteTest(UINT uiTest)
{
    return CPShader::ExecuteTest(uiTest);
}

void CPSRegister::CommandLineHelp()
{
}
