// FILE:        op.cpp
// DESC:        pixel shader op conformance tests
// AUTHOR:      Cliff Hudson

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "cshapes.h"
#include "PShader.h"
//#include "CImageLoader.h"

#ifndef D3DRGBA
    #define D3DRGBA(r, g, b, a) \
        ((D3DCOLOR) \
         (   (((long)((a) * 255)) << 24) | (((long)((r) * 255)) << 16) \
         |   (((long)((g) * 255)) << 8) | (long)((b) * 255) \
         ))
#endif

//CPSOps PSOpTest;

extern MYTLVERTEX g_FourColorVertices[];
extern MYTLVERTEX g_FourColorVertices2[];
extern MYTLVERTEX g_FourBrickVertices[];

CPSOps::CPSOps()
{
    m_szTestName = TEXT("PSOps");
    m_szCommandKey = TEXT("PSOps");

//    m_pCheckerData = NULL;
    m_pCheckerTex = NULL;
//    m_pChecker2Data = NULL;
    m_pChecker2Tex = NULL;
//    m_pDP3Data = NULL;
    m_pDP3Tex = NULL;
//    m_pBrickData = NULL;
    m_pBrickTex = NULL;
//    m_pGradientData = NULL;
    m_pGradientTex = NULL;
//	m_pDXLogoData = NULL;
	m_pDXLogoTex = NULL;
}

bool CPSOps::TestTerminate()
{
    ReleaseTexture(m_pCheckerTex);
//    RELEASE(m_pCheckerData);
    ReleaseTexture(m_pChecker2Tex);
//    RELEASE(m_pChecker2Data);
    ReleaseTexture(m_pDP3Tex);
//    RELEASE(m_pDP3Data);
    ReleaseTexture(m_pBrickTex);
//    RELEASE(m_pBrickData);
    ReleaseTexture(m_pGradientTex);
//    RELEASE(m_pGradientData);
    ReleaseTexture(m_pDXLogoTex);
//    RELEASE(m_pDXLogoData);
    return CPShader::TestTerminate();
}

UINT CPSOps::TestInitialize()
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

void CPSOps::PopulateShaderInfo()
{
    // ****************************** add ******************************
	// (1) add
	AddShaderDef("add",
                 "tex t0\n"
                 "add r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);

    // (2) add_x2
	AddShaderDef("add_x2",
                 "tex t0\n"
                 "add_x2 r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices2, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pChecker2Tex);

    // (3) add_x4
	AddShaderDef("add_x4",
                 "tex t0\n"
                 "add_x4 r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices2, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pChecker2Tex);

    // (4) add_d2
	AddShaderDef("add_d2",
                 "tex t0\n"
                 "add_d2 r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);

    // (5) add_sat
	AddShaderDef("add_sat",
                 "tex t0\n"
#ifndef UNDER_XBOX
                 "add_sat r1, t0, v0\n"
				 "mov_d2 r0, r1\n",
#else
                 "add r1, t0, v0\n"
//				 "mov_d2 r0, r1_usgn\n",
				 "mov_d2 r0, r1\n",
#endif
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);

    
	// ****************************** dp3 ******************************
    // (6) dp3
	AddShaderDef("dp3",
                 "tex t0\n"
				 "dp3 r0, c0_sgn, t0_sgn\n"
				 "mul r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourBrickVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pBrickTex);
    AddConstantsToLastShaderDef(0.408248f, 0.408248f, -0.816497f, 1.0f);

    // (7) dp3_x2
	AddShaderDef("dp3_x2",
                 "tex t0\n"
				 "dp3_x2 r0, c0_sgn, t0_sgn\n"
				 "mul_d2 r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourBrickVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pBrickTex);
    AddConstantsToLastShaderDef(0.408248f, 0.408248f, -0.816497f, 1.0f);

    // (8) dp3_x4
	AddShaderDef("dp3_x4",
                 "tex t0\n"
				 "dp3_x4 r0, c0_sgn, t0_sgn\n"
				 "mul_d2 r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourBrickVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pBrickTex);
    AddConstantsToLastShaderDef(0.204124f, 0.204124f, -0.408248f, 1.0f);

    // (9) dp3_d2
	AddShaderDef("dp3_d2",
                 "tex t0\n"
				 "dp3_d2 r0, c0_sgn, t0_sgn\n"
				 "mul_x2 r0, v0, r0\n",
                 MYTLVERTEX_FVF, g_FourBrickVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pBrickTex);
    AddConstantsToLastShaderDef(0.408248f, 0.408248f, -0.816497f, 1.0f);

    // (10) dp3_sat
	AddShaderDef("dp3_sat",
                 "tex t0\n"
#ifndef UNDER_XBOX
				 "dp3_sat r0, c0, t0_sgn\n"
				 "mul_d2 r0, v0, r0\n",
#else
				 "dp3 r0, c0_sgn, t0_sgn\n"
//				 "mul_d2 r0, v0, r0_usgn\n",
				 "mul_d2 r0, v0, r0\n",
#endif
                 MYTLVERTEX_FVF, g_FourBrickVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pBrickTex);
    AddConstantsToLastShaderDef(0.408248f, 0.408248f, -0.816497f, 1.0f);

    
	// ****************************** lrp ******************************
	// (11) lrp
    AddShaderDef("lrp",
                 "tex t0\n"
                 "lrp r0, c0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(0.5, 0.5, 0.5, 0.5);

	// (12) lrp_x2
    AddShaderDef("lrp_x2",
                 "tex t0\n"
                 "lrp_x2 r0, c0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(0.5, 0.5, 0.5, 0.5);

	// (13) lrp_x4
    AddShaderDef("lrp_x4",
                 "tex t0\n"
                 "lrp_x4 r0, c0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(0.5, 0.5, 0.5, 0.5);

	// (14) lrp_d2
    AddShaderDef("lrp_d2",
                 "tex t0\n"
                 "lrp_d2 r0, c0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(0.3f, 0.3f, 0.3f, 0.3f);

	// (15) lrp_sat
    AddShaderDef("lrp_sat",
                 "tex t0\n"
#ifndef UNDER_XBOX
                 "lrp_sat r0, c0, t0, v0\n"
				 "mov_d2 r0, r0\n",
#else
                 "lrp r0, c0, t0, v0\n"
//				 "mov_d2 r0, r0_usgn\n",
				 "mov_d2 r0, r0\n",
#endif
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(0.7f, 0.7f, 0.7f, 0.7f);


    // ****************************** mad ******************************
    // (16) mad
	AddShaderDef("mad",
                 "tex t0\n"
                 "mad r0, c0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(0.5, 0.5, 0.5, 0.5);

    // (17) mad_x2
	AddShaderDef("mad_x2",
                 "tex t0\n"
                 "mad_x2 r0, v0, c0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(0.25, 0.25, 0.25, 0.25);

    // (18) mad_x4
	AddShaderDef("mad_x4",
                 "tex t0\n"
                 "mad_x4 r0, v0, c0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pChecker2Tex);
    AddConstantsToLastShaderDef(0.25, 0.25, 0.25, 0.25);

    // (19) mad_d2
	AddShaderDef("mad_d2",
                 "tex t0\n"
                 "mad_d2 r0, c0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(0.5, 0.5, 0.5, 0.5);

    // (20) mad_sat
	AddShaderDef("mad_sat",
                 "tex t0\n"
#ifndef UNDER_XBOX
                 "mad_sat r0, c0, t0, v0\n"
				 "mov_d2 r0, r0\n",
#else
                 "mad r0, c0, t0, v0\n"
//				 "mov_d2 r0, r0_usgn\n",
				 "mov_d2 r0, r0\n",
#endif
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(0.5, 0.5, 0.5, 0.5);

    // ****************************** mov ******************************
    // (21) mov
    AddShaderDef("mov",
                 "tex t0\n"
                 "mov r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pChecker2Tex);

    // (22) mov_x2
    AddShaderDef("mov_x2",
                 "tex t0\n"
                 "mov_x2 r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pChecker2Tex);

    // (23) mov_x4
    AddShaderDef("mov_x4",
                 "tex t0\n"
                 "mov_x4 r1, v0\n"
				 "mul r0, r1, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(0.2f, 0.2f, 0.2f, 0.2f);

    // (24) mov_d2
    AddShaderDef("mov_d2",
                 "tex t0\n"
                 "mov_d2 r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);

    // (25) mov_sat
    AddShaderDef("mov_sat",
                 "tex t0\n"
                 "mov_x2 r0, t0\n"
#ifndef UNDER_XBOX
				 "mov_sat r1, r0\n"
				 "mov_d2 r0.rgb, r1\n",
#else
				 "mov r1, r0\n"
//				 "mov_d2 r0.rgb, r1_usgn\n",
				 "mov_d2 r0.rgb, r1\n",
#endif
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pGradientTex);

    // ****************************** mul ******************************
    // (26) mul
    AddShaderDef("mul",
                 "tex t0\n"
                 "mul r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pChecker2Tex);

    // (27) mul_x2
    AddShaderDef("mul_x2",
                 "tex t0\n"
                 "mul_x2 r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pChecker2Tex);

    // (28) mul_x4
    AddShaderDef("mul_x4",
                 "tex t0\n"
                 "mul_x4 r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pChecker2Tex);

    // (29) mul_d2
    AddShaderDef("mul_d2",
                 "tex t0\n"
                 "mul_d2 r0, t0, v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);

    // (30) mul_sat
    AddShaderDef("mul_sat",
                 "tex t0\n"
                 "add r1, t0, c0\n"
#ifndef UNDER_XBOX
				 "mul_sat r0, r1, v0\n"
				 "add r0.rgb, r0, -c0\n",
#else
				 "mul r0, r1, v0\n"
//				 "add r0.rgb, r0_usgn, -c0\n",
				 "add r0.rgb, r0, -c0\n",
#endif
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(0.5f, 0.5f, 0.5f, 0.5f);

    // ****************************** sub ******************************
    // (31) sub
	AddShaderDef("sub",
                 "tex t0\n"
                 "tex t1\n"
                 "mov r1, t1\n"
				 "sub r0, t0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pGradientTex);

    // (32) sub_x2
	AddShaderDef("sub_x2",
                 "tex t0\n"
				 "tex t1\n"
				 "mov r1, t1\n"
                 "sub_x2 r0, t0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddTextureToLastShaderDef(m_pGradientTex);

    // (33) sub_x4
	AddShaderDef("sub_x4",
                 "tex t0\n"
                 "sub_x4 r0, t0, c0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(0.8f, 0.8f, 0.8f, 0.8f);

    // (34) sub_d2
	AddShaderDef("sub_d2",
                 "tex t0\n"
                 "sub_d2 r0, c0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(0.5, 0.5, 0.5, 0.5);

    // (35) sub_sat
	AddShaderDef("sub_sat",
                 "tex t0\n"
#ifndef UNDER_XBOX
                 "sub_sat r1, c0, t0\n"
				 "mov_d2 r0, r1\n",
#else
                 "sub r1, c0, t0\n"
//				 "mov_d2 r0, r1_usgn\n",
				 "mov_d2 r0, r1\n",
#endif
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);
    AddConstantsToLastShaderDef(1.5f, 1.5f, 1.5f, 1.5f);

    // ****************************** nop ******************************
	// (36) nop
    AddShaderDef("nop",
                 "tex t0\n" 
                 "nop\n"
                 "mov r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pCheckerTex);

	// (37) nop_x2
    AddShaderDef("nop",
                 "tex t0\n" 
                 "nop_x2\n"
                 "mov r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pChecker2Tex);

	// (38) nop_x4
    AddShaderDef("nop",
                 "tex t0\n" 
                 "nop_x4\n"
                 "mov r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pGradientTex);

	// (39) nop_d2
    AddShaderDef("nop",
                 "tex t0\n" 
                 "nop_d2\n"
                 "mov r0, t0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pDXLogoTex);

	// (40) nop_sat
    AddShaderDef("nop",
                 "tex t0\n" 
#ifndef UNDER_XBOX
                 "nop_sat\n"
#else
                 "nop\n"
#endif
                 "mov r0, t0\n",
                 MYTLVERTEX_FVF, g_FourBrickVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pBrickTex);

    // ****************************** cnd ******************************
    // (41) cnd
	AddShaderDef("cnd",
                 "tex t0\n"
                 "tex t1\n"
                 "mov r0, t0\n"
                 "cnd r0, r0.a, r0, t1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pGradientTex);
    AddTextureToLastShaderDef(m_pCheckerTex);

    // (42) cnd_x2
	AddShaderDef("cnd_x2",
                 "tex t0\n"
                 "tex t1\n"
                 "mov r0, t0\n"
                 "cnd_x2 r0, r0.a, r0, t1\n"
				 "mov r0, 1-r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pGradientTex);
    AddTextureToLastShaderDef(m_pCheckerTex);

    // (43) cnd_x4
	AddShaderDef("cnd_x4",
                 "tex t0\n"
                 "tex t1\n"
                 "mov r0, t0\n"
                 "cnd_x4 r0, r0.a, r0, t1\n"
				 "mov_d2 r0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pGradientTex);
    AddTextureToLastShaderDef(m_pCheckerTex);

    // (44) cnd_d2
	AddShaderDef("cnd_d2",
                 "tex t0\n"
                 "tex t1\n"
                 "mov r0, t0\n"
                 "cnd_d2 r0, r0.a, r0, t1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pGradientTex);
    AddTextureToLastShaderDef(m_pCheckerTex);

    // (45) cnd_sat
	AddShaderDef("cnd_sat",
                 "tex t0\n"
                 "tex t1\n"
                 "mov_x2 r0, t0\n"
#ifndef UNDER_XBOX
                 "cnd_sat r1, r0.a, r0, t1\n"
				 "mov_d2 r0, r1\n",
#else
                 "cnd r1, r0.a, r0, t1\n"
//				 "mov_d2 r0, r1_usgn\n",
				 "mov_d2 r0, r1\n",
#endif
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
    AddTextureToLastShaderDef(m_pGradientTex);
    AddTextureToLastShaderDef(m_pCheckerTex);

    // *************************** cnd pairing ***************************
    // (46) cnd pairing
	AddShaderDef("cnd: pairing",
				 "def c0, 0, 0, 0, 1.0\n"
				 "def c1, 0, 0, 0, 0.4\n"
				 "mov r0, c0\n"
				 "mov r1, c1\n"
				 "mov r0.a, r1.a\n"
				 "+cnd r0.rgb, r0.a, c0.a, r0.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 0));
}

bool CPSOps::LoadTextures()
{
//    CImageLoader Loader;
    
    // Checker
//	if (NULL == (m_pCheckerData = Loader.Load(CIL_BMP, "checker.bmp", 1.0f)))
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
    
    // Checker2
//	if (NULL == (m_pChecker2Data = Loader.Load(CIL_BMP, "checker2.bmp", 1.0f)))
//    {
//        WriteToLog("(ERROR) Failed to load checker2.bmp.  Aborting.\n");
//        return false;
//    }
//    if (NULL == (m_pChecker2Tex = CreateTexture(64, 64, CDDS_TEXTURE_VIDEO, m_pChecker2Data)))
    if (NULL == (m_pChecker2Tex = (CTexture8*)CreateTexture(m_pDevice, TEXT("checker2.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, POOL_DEFAULT, 64, 64)))
    {
        WriteToLog("(ERROR) Failed to create checker texture.  Aborting.\n");
        return false;
    }
    
    // DP3
//	if (NULL == (m_pDP3Data = Loader.Load(CIL_BMP, "DP3.bmp", 1.0f)))
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

    // DXLogo
//	if (NULL == (m_pDXLogoData = Loader.Load(CIL_BMP, "DXLogo.bmp", 1.0f)))
//    {
//        WriteToLog("(ERROR) Failed to load DXLogo.bmp.  Aborting.\n");
//        return false;
//    }
//    if (NULL == (m_pDXLogoTex = CreateTexture(64, 64, CDDS_TEXTURE_VIDEO, m_pDXLogoData)))
    if (NULL == (m_pDXLogoTex = (CTexture8*)CreateTexture(m_pDevice, TEXT("dxlogo.bmp"), D3DFMT_A8R8G8B8, TTYPE_TEXTURE, NULL, 1.0f, COLORKEY_NONE, POOL_DEFAULT, 64, 64)))
    {
        WriteToLog("(ERROR) Failed to create DXLogo texture.  Aborting.\n");
        return false;
    }

	// Bricks
//	if (NULL == (m_pBrickData = GenerateBrickTexture()))
//    {
//        WriteToLog("(ERROR) Failed to generate dp3 data.  Aborting.\n");
//        return false;
//    }
//    if (NULL == (m_pBrickTex = CreateTexture(64, 64, CDDS_TEXTURE_VIDEO, m_pBrickData)))
    if (NULL == (m_pBrickTex = GenerateBrickTexture()))
    {
        WriteToLog("(ERROR) Failed to create DP3 texture.  Aborting.\n");
        return false;
    }

    // Gradient
	DWORD dwColors[4] =
    {
        D3DRGBA(0, 1, 0, 1),
        D3DRGBA(0, 1, 0, 1),
        D3DRGBA(1, 1, 1, 0),
        D3DRGBA(1, 1, 1, 0)
    };
//    if (NULL == (m_pGradientData = Loader.LoadGradient(64, 64, dwColors)))
//    {
//        WriteToLog("(ERROR) Failed to create gradient data.  Aborting.\n");
//        return false;
//    }
//    if (NULL == (m_pGradientTex = CreateTexture(64, 64, CDDS_TEXTURE_VIDEO, m_pGradientData)))
    if (NULL == (m_pGradientTex = CreateGradientTexture(m_pDevice, 64, 64, dwColors)))
    {
        WriteToLog("(ERROR) Failed to create gradient texture.  Aborting.\n");
        return false;
    }

    return true;
}

bool CPSOps::ExecuteTest(UINT uiTest)
{
    return CPShader::ExecuteTest(uiTest);
}

void  CPSOps::CommandLineHelp()
{
}
