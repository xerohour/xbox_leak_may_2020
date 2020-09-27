////////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) Microsoft Corporation, 1999.
//
// CTextureStage.cpp
//
// CTextureStage class - Tests many possible texture blend combinations
//
// History: 5/15/99 Jeff Vezina     - Created
//
////////////////////////////////////////////////////////////////////////////////////////////
//
// Test Description:
//    Tests many combinations of different stage tests with every supported texture format.
//    A stage test consists of a color op, 2 color arguments, alpha op, 2 alpha arguments,
//    and a texture depending on the arguments used.  Instead of doing a permutation with
//    every test on all stages, it limits the test permutation of lower stages to 2 (default).
//    So this results in running every test on all stage, and using common stage tests for
//    the lower stages (when testing stages above 0).
//
//    Here is an example of how the algorithm works with 4 stage tests on a card that
//    supports 2 blend stages.  This series of test would be repeated using all supported
//    texture formats (default).
//
//    Test #    Stage 0        Stage 1       Stage 2     ...    Stage 7
//       1    Stage test 1     Disabled      Disabled    ...    Disabled
//       2    Stage test 2     Disabled      Disabled    ...    Disabled
//       3    Stage test 3     Disabled      Disabled    ...    Disabled
//       4    Stage test 4     Disabled      Disabled    ...    Disabled
//       5    Stage test 1   Stage test 1    Disabled    ...    Disabled
//       6    Stage test 2   Stage test 1    Disabled    ...    Disabled
//       7    Stage test 3   Stage test 1    Disabled    ...    Disabled
//      ...       ...            ...           ...       ...      ...  
//      16    Stage test 4   Stage test 4    Disabled    ...    Disabled
//
////////////////////////////////////////////////////////////////////////////////////////////

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "CStage.h"
#include "CTextureStage.h"
//#include "math.h"

//CD3DWindowFramework TheApp;
CTextureStage* g_pTextureStage;

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

    CTextureStage*  TextureStage;
    BOOL      bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    TextureStage = new CTextureStage();
    if (!TextureStage) {
        return FALSE;
    }

    g_pTextureStage = TextureStage;

    // Initialize the scene
    if (!TextureStage->Create(pDisplay)) {
        TextureStage->Release();
        return FALSE;
    }

    bRet = TextureStage->Exhibit(pnExitCode);

    // Clean up the scene
    TextureStage->Release();

    return bRet;
}

void CTextureStage::CommandLineHelp()
{
/*
    WriteCommandLineHelp("$y-!ALLBLENDMODES\t$wRemove all blend modes from test");
    WriteCommandLineHelp("$y-!ALLTEXFORMATS\t$wRemove all texture formats from test");
    WriteCommandLineHelp("$y-!ALPHA\t\t$wTurns off alpha blending");
    WriteCommandLineHelp("$y-!SPECULAR\t\t$wTurns off specular");
    WriteCommandLineHelp("$y-PERSPECTIVE\t$wSets vertices to different depth");
    WriteCommandLineHelp("$y-FOG\t\t$wTurns on fog (detects drivers faking multitexture)");
    WriteCommandLineHelp("$y-MULTIPASS\t$wRenders using multiple passes");
    WriteCommandLineHelp("$y-STAGEMAX #\t$wTests all stages until #");
    WriteCommandLineHelp("$y-COMBINATION #\t$wCombinations to try with all texture ops");
    WriteCommandLineHelp("$y\t\t$wDefault is 2, must be within 1-%d",MAX_BLENDMODES);
    WriteCommandLineHelp("");

    // Pixel formats are not enumerated yet, so manually output all Ref pixel formats
    WriteCommandLineHelp("$y[+|!]32-0888 \t$w- Remove/Add 32 bit 888 from test");
    WriteCommandLineHelp("$y[+|!]32-8888 \t$w- Remove/Add 8 bit 8888 from test");
    WriteCommandLineHelp("$y[+|!]16-0565 \t$w- Remove/Add 8 bit 565 from test");
    WriteCommandLineHelp("$y[+|!]16-0555 \t$w- Remove/Add 8 bit 555 from test");
    WriteCommandLineHelp("$y[+|!]PAL4 \t$w- Remove/Add 4 bit PAL4 from test");
    WriteCommandLineHelp("$y[+|!]PAL8 \t$w- Remove/Add 8 bit PAL8 from test");
    WriteCommandLineHelp("$y[+|!]16-1555 \t$w- Remove/Add 16 bit 1555 from test");
    WriteCommandLineHelp("$y[+|!]16-4444 \t$w- Remove/Add 16 bit 4444 from test");
    WriteCommandLineHelp("$y[+|!]8-0332 \t$w- Remove/Add 8 bit 332 from test");
    WriteCommandLineHelp("$y[+|!]8-8000L \t$w- Remove/Add 8 bit 8 LUMINANCE from test");
    WriteCommandLineHelp("$y[+|!]16-8800L \t$w- Remove/Add 16 bit 88 LUMINANCE from test");
    WriteCommandLineHelp("$y[+|!]UYVY \t$w- Remove/Add UYVY from test");
    WriteCommandLineHelp("$y[+|!]YUY2 \t$w- Remove/Add YUY2 from test");
    WriteCommandLineHelp("$y[+|!]DXT1 \t$w- Remove/Add DXT1 from test");
    WriteCommandLineHelp("$y[+|!]DXT2 \t$w- Remove/Add DXT2 from test");
    WriteCommandLineHelp("$y[+|!]DXT3 \t$w- Remove/Add DXT3 from test");
    WriteCommandLineHelp("$y[+|!]DXT4 \t$w- Remove/Add DXT4 from test");
    WriteCommandLineHelp("$y[+|!]DXT5 \t$w- Remove/Add DXT5 from test");
    WriteCommandLineHelp("$y[+|!]16-8800B \t$w- Remove/Add 16 bit 88 BUMPDUDV from test");
    WriteCommandLineHelp("$y[+|!]16-0565B \t$w- Remove/Add 16 bit 565 BUMPDUDV from test");
    WriteCommandLineHelp("$y[+|!]24-0888B \t$w- Remove/Add 24 bit 888 BUMPDUDV from test");
    WriteCommandLineHelp("");

    for (UINT i=0;i<MAX_BLENDMODES;i++)
        WriteCommandLineHelp("$y[+|!]%s \t$w- Remove/Add %s from test",g_TextureStage.m_rgBlendModes[i].szShortName,
                                                                       g_TextureStage.m_rgBlendModes[i].szLongName);
*/
}

bool IsTextureOp(UINT i) {

    switch ((D3DTEXTUREOP)i) {

        case D3DTOP_DISABLE:
        case D3DTOP_SELECTARG1:
        case D3DTOP_SELECTARG2:
        case D3DTOP_MODULATE:
        case D3DTOP_MODULATE2X:
        case D3DTOP_MODULATE4X:
        case D3DTOP_ADD:
        case D3DTOP_ADDSIGNED:
        case D3DTOP_ADDSIGNED2X:
        case D3DTOP_SUBTRACT:
        case D3DTOP_ADDSMOOTH:
        case D3DTOP_BLENDDIFFUSEALPHA:
        case D3DTOP_BLENDTEXTUREALPHA:
        case D3DTOP_BLENDFACTORALPHA:
        case D3DTOP_BLENDTEXTUREALPHAPM:
        case D3DTOP_BLENDCURRENTALPHA:
        case D3DTOP_PREMODULATE:
        case D3DTOP_MODULATEALPHA_ADDCOLOR:
        case D3DTOP_MODULATECOLOR_ADDALPHA:
        case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
        case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
        case D3DTOP_BUMPENVMAP:
        case D3DTOP_BUMPENVMAPLUMINANCE:
//        case D3DTOP_DOTPRODUCT3:
//        case D3DTOP_MULTIPLYADD:
//        case D3DTOP_LERP:
            return true;
    }

    return false;
}

CTextureStage::CTextureStage()
{
    strcpy(m_rgBlendModes[D3DTOP_DISABLE-1].szShortName,"DISABLE"); strcpy(m_rgBlendModes[0].szLongName,"D3DTOP_DISABLE");
    strcpy(m_rgBlendModes[D3DTOP_SELECTARG1-1].szShortName,"SELECTARG1"); strcpy(m_rgBlendModes[1].szLongName,"D3DTOP_SELECTARG1");
    strcpy(m_rgBlendModes[D3DTOP_SELECTARG2-1].szShortName,"SELECTARG2"); strcpy(m_rgBlendModes[2].szLongName,"D3DTOP_SELECTARG2");
    strcpy(m_rgBlendModes[D3DTOP_MODULATE-1].szShortName,"MODULATE");   strcpy(m_rgBlendModes[3].szLongName,"D3DTOP_MODULATE");
    strcpy(m_rgBlendModes[D3DTOP_MODULATE2X-1].szShortName,"MODULATE2X"); strcpy(m_rgBlendModes[4].szLongName,"D3DTOP_MODULATE2X");
    strcpy(m_rgBlendModes[D3DTOP_MODULATE4X-1].szShortName,"MODULATE4X"); strcpy(m_rgBlendModes[5].szLongName,"D3DTOP_MODULATE4X");
    strcpy(m_rgBlendModes[D3DTOP_ADD-1].szShortName,"ADD");        strcpy(m_rgBlendModes[6].szLongName,"D3DTOP_ADD");
    strcpy(m_rgBlendModes[D3DTOP_ADDSIGNED-1].szShortName,"ADDSIGNED");  strcpy(m_rgBlendModes[7].szLongName,"D3DTOP_ADDSIGNED");
    strcpy(m_rgBlendModes[D3DTOP_ADDSIGNED2X-1].szShortName,"ADDSIGNED2X");strcpy(m_rgBlendModes[8].szLongName,"D3DTOP_ADDSIGNED2X");
    strcpy(m_rgBlendModes[D3DTOP_SUBTRACT-1].szShortName,"SUBTRACT");   strcpy(m_rgBlendModes[9].szLongName,"D3DTOP_SUBTRACT");
    strcpy(m_rgBlendModes[D3DTOP_ADDSMOOTH-1].szShortName,"ADDSMOOTH");  strcpy(m_rgBlendModes[10].szLongName,"D3DTOP_ADDSMOOTH");
    strcpy(m_rgBlendModes[D3DTOP_BLENDDIFFUSEALPHA-1].szShortName,"BLENDDIFFUSEALPHA");     strcpy(m_rgBlendModes[11].szLongName,"D3DTOP_BLENDDIFFUSEALPHA");
    strcpy(m_rgBlendModes[D3DTOP_BLENDTEXTUREALPHA-1].szShortName,"BLENDTEXTUREALPHA");     strcpy(m_rgBlendModes[12].szLongName,"D3DTOP_BLENDTEXTUREALPHA");
    strcpy(m_rgBlendModes[D3DTOP_BLENDFACTORALPHA-1].szShortName,"BLENDFACTORALPHA");      strcpy(m_rgBlendModes[13].szLongName,"D3DTOP_BLENDFACTORALPHA");
    strcpy(m_rgBlendModes[D3DTOP_BLENDTEXTUREALPHAPM-1].szShortName,"BLENDTEXTUREALPHAPM");   strcpy(m_rgBlendModes[14].szLongName,"D3DTOP_BLENDTEXTUREALPHAPM");
    strcpy(m_rgBlendModes[D3DTOP_BLENDCURRENTALPHA-1].szShortName,"BLENDCURRENTALPHA");     strcpy(m_rgBlendModes[15].szLongName,"D3DTOP_BLENDCURRENTALPHA");
    strcpy(m_rgBlendModes[D3DTOP_PREMODULATE-1].szShortName,"PREMODULATE");           strcpy(m_rgBlendModes[16].szLongName,"D3DTOP_PREMODULATE");
    strcpy(m_rgBlendModes[D3DTOP_MODULATEALPHA_ADDCOLOR-1].szShortName,"MODULATEALPHA_ADDCOLOR");    strcpy(m_rgBlendModes[17].szLongName,"D3DTOP_MODULATEALPHA_ADDCOLOR");
    strcpy(m_rgBlendModes[D3DTOP_MODULATECOLOR_ADDALPHA-1].szShortName,"MODULATECOLOR_ADDALPHA");    strcpy(m_rgBlendModes[18].szLongName,"D3DTOP_MODULATECOLOR_ADDALPHA");
    strcpy(m_rgBlendModes[D3DTOP_MODULATEINVALPHA_ADDCOLOR-1].szShortName,"MODULATEINVALPHA_ADDCOLOR"); strcpy(m_rgBlendModes[19].szLongName,"D3DTOP_MODULATEINVALPHA_ADDCOLOR");
    strcpy(m_rgBlendModes[D3DTOP_MODULATEINVCOLOR_ADDALPHA-1].szShortName,"MODULATEINVCOLOR_ADDALPHA"); strcpy(m_rgBlendModes[20].szLongName,"D3DTOP_MODULATEINVCOLOR_ADDALPHA");
    strcpy(m_rgBlendModes[D3DTOP_BUMPENVMAP-1].szShortName,"BUMPENVMAP");    strcpy(m_rgBlendModes[21].szLongName,"D3DTOP_BUMPENVMAP");
    strcpy(m_rgBlendModes[D3DTOP_BUMPENVMAPLUMINANCE-1].szShortName,"BUMPENVMAPLUMINANCE");   strcpy(m_rgBlendModes[22].szLongName,"D3DTOP_BUMPENVMAPLUMINANCE");
//    strcpy(m_rgBlendModes[D3DTOP_DOTPRODUCT3-1].szShortName,"DOTPRODUCT3");   strcpy(m_rgBlendModes[23].szLongName,"D3DTOP_DOTPRODUCT3");
//    strcpy(m_rgBlendModes[D3DTOP_MULTIPLYADD-1].szShortName,"MULTIPLYADD");   strcpy(m_rgBlendModes[23].szLongName,"D3DTOP_MULTIPLYADD");
//    strcpy(m_rgBlendModes[D3DTOP_LERP-1].szShortName,"LERP");   strcpy(m_rgBlendModes[23].szLongName,"D3DTOP_LERP");

    // By default, use all blend modes
    for (UINT i=0;i<MAX_BLENDMODES;i++)
        m_rgBlendModes[i].bUsed=IsTextureOp(i+1);

    // By default, use all texture formats
    for (i=0;i<MAX_D3D_TEXTURES;i++)
        m_rgTexFormats[i].bUsed=true;

    m_uTexFormatMax=0;
    m_uStageListMax=0;
    m_cStageMax=0;
    m_bMultiPass=false;
    m_bPassValidate=true;
    m_uLastFailedStage=0;

    m_bSpecular=true;
    m_bAlpha=true;
    m_bFog=false;

    // Reset all stages to DISABLED
    for (i=0;i<8;i++)
        m_rgpStage[i]=&m_StageList;

    m_szTestName=TEXT("TextureStage");
    m_szCommandKey=TEXT("Range");
}

CTextureStage::~CTextureStage()
{
}

bool CTextureStage::SetDefaultMaterials(void)
{
    return true;
}

bool CTextureStage::SetDefaultLights(void)
{
    return true;
}

bool CTextureStage::SetDefaultLightStates(void)
{
    return true;
}

bool CTextureStage::SetDefaultRenderStates(void)
{
//    DEVICEDESC SrcDesc=m_pAdapter->Devices[m_pMode->nSrcDevice].Desc;
    float flStart=0.6f;
    float flEnd=1.0f;
	int i;

//    SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE,(DWORD)TRUE);
    SetRenderState(D3DRENDERSTATE_TEXTUREFACTOR,(DWORD)0xAA005500);
    SetRenderState(D3DRENDERSTATE_LIGHTING,(DWORD)FALSE);
    SetRenderState(D3DRENDERSTATE_AMBIENT,(DWORD)0xFF000000);
    SetRenderState(D3DRENDERSTATE_DITHERENABLE,(DWORD)TRUE);

    if (KeySet("!SPECULAR"))
        m_bSpecular=false;
    else if (!(m_d3dcaps.ShadeCaps&D3DPSHADECAPS_SPECULARGOURAUDRGB))
    {
//        if (!(m_d3dcaps.ShadeCaps&D3DPSHADECAPS_SPECULARFLATRGB))
//        {
//            m_bSpecular=false;
//            WriteToLog("Gouraud and flat specular not supported, disabling specular");
//        }
//        else                        // Flat is supported, make all speculars the same
        {
            WriteToLog("Gouraud specular not supported, using flat specular");
            for (i=1;i<m_cVertexMax;i++)
                m_rgTLVertex[i].specular=m_rgTLVertex[0].specular;
        }
    }

    if (KeySet("FOG"))
    {
        if (m_d3dcaps.ShadeCaps&D3DPSHADECAPS_FOGGOURAUD)
            m_bFog=true;
        else
            WriteToLog("Gouraud fog not supported, disabling fog");
    }

    if (KeySet("!ALPHA"))
        m_bAlpha=false;
    else if (!(m_d3dcaps.ShadeCaps&D3DPSHADECAPS_ALPHAGOURAUDBLEND))
    {
//        if (!(m_d3dcaps.ShadeCaps&D3DPSHADECAPS_ALPHAFLATBLEND))
//        {
//            m_bAlpha=false;
//            WriteToLog("Gouraud and flat alpha blending not supported, disabling alpha blending");
//        }
//        else                        // Flat is supported, make all alphas the same
        {
            WriteToLog("Gouraud alpha blending not supported, using flat alpha blending");
            for (i=1;i<m_cVertexMax;i++)
            {
                m_rgTLVertex[i].specular=(m_rgTLVertex[0].specular&0xFF000000)|(m_rgTLVertex[i].specular&~0xFF000000);
                m_rgTLVertex[i].color=(m_rgTLVertex[0].color&0xFF000000)|(m_rgTLVertex[i].color&~0xFF000000);
            }
        }
    }

    if (m_bSpecular)
        SetRenderState(D3DRENDERSTATE_SPECULARENABLE,(DWORD)TRUE);
    else
        SetRenderState(D3DRENDERSTATE_SPECULARENABLE,(DWORD)FALSE);

    if (m_bAlpha)
    {
        SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,(DWORD)TRUE);
        SetRenderState(D3DRENDERSTATE_SRCBLEND,(DWORD)D3DBLEND_SRCALPHA);
        SetRenderState(D3DRENDERSTATE_DESTBLEND,(DWORD)D3DBLEND_INVSRCALPHA);
    }
	else
        SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,(DWORD)FALSE);

    if (m_bFog)
    {
        SetRenderState(D3DRENDERSTATE_FOGENABLE,(DWORD)TRUE);
#ifndef UNDER_XBOX
        SetRenderState(D3DRENDERSTATE_FOGTABLEMODE,(DWORD)D3DFOG_NONE);
        SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE,(DWORD)D3DFOG_LINEAR);
#else
        SetRenderState(D3DRENDERSTATE_FOGTABLEMODE,(DWORD)D3DFOG_LINEAR);
#endif // UNDER_XBOX
        SetRenderState(D3DRENDERSTATE_FOGSTART,*((DWORD*)&flStart));
        SetRenderState(D3DRENDERSTATE_FOGEND,*((DWORD*)&flEnd));
        SetRenderState(D3DRENDERSTATE_FOGCOLOR,(DWORD)0xFFFFFFFF);
    }
	else
        SetRenderState(D3DRENDERSTATE_FOGENABLE,(DWORD)FALSE);

    return true;
}

UINT CTextureStage::TestInitialize(void)
{
    int i;
    char szStr[500];
//    DEVICEDESC SrcDesc=m_pAdapter->Devices[m_pMode->nSrcDevice].Desc;

    // Create top view of a pyramid, using first 5 vertices for fan, next 3 for list to cover special case
    m_cVertexMax=0;
    m_rgTLVertex[m_cVertexMax++]=cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 160.0f,(float)m_vpTest.Y + 140.0f,0.9f),1.0f/0.9f,0xFF4444FF,0xFF333333,0.5f,0.5f);
    m_rgTLVertex[m_cVertexMax++]=cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,(float)m_vpTest.Y +   9.5f,0.3f),1.0f/0.3f,0x55222222,0x55999999,0.0f,0.0f);
    m_rgTLVertex[m_cVertexMax++]=cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 309.5f,(float)m_vpTest.Y +   9.5f,0.3f),1.0f/0.3f,0x55222222,0x55999999,1.0f,0.0f);
    m_rgTLVertex[m_cVertexMax++]=cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 309.5f,(float)m_vpTest.Y + 269.5f,0.3f),1.0f/0.3f,0x55222222,0x55999999,1.0f,1.0f);
    m_rgTLVertex[m_cVertexMax++]=cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,(float)m_vpTest.Y + 269.5f,0.3f),1.0f/0.3f,0x55222222,0x55999999,0.0f,1.0f);

    m_rgTLVertex[m_cVertexMax++]=cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 160.0f,(float)m_vpTest.Y + 140.0f,0.9f),1.0f/0.9f,0xFF4444FF,0xFF333333,0.5f,0.5f);
    m_rgTLVertex[m_cVertexMax++]=cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,(float)m_vpTest.Y + 269.5f,0.3f),1.0f/0.3f,0x55222222,0x55999999,0.0f,1.0f);
    m_rgTLVertex[m_cVertexMax++]=cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   9.5f,(float)m_vpTest.Y +   9.5f,0.3f),1.0f/0.3f,0x55222222,0x55999999,0.0f,0.0f);

    // Create texture format short and long names
    CreateTexFormatNames();

    // Make sure we are on DX6
    if (m_dwVersion<0x0600)
    {
        WriteToLog("This test requires Dx6.\n");
        SetTestRange(1,1);
        return D3DTESTINIT_SKIPALL;
    }

    // Run ONLY 32bit 8888 format when src device is ref or rgb, to shorten test
/*
    if ((m_pAdapter->Devices[m_pMode->nSrcDevice].Guid==IID_IDirect3DRefDevice) ||
        (m_pAdapter->Devices[m_pMode->nSrcDevice].Guid==IID_IDirect3DRGBDevice))
    {
        for (i=0;i<m_uCommonTextureFormats;i++)
            m_rgTexFormats[i].bUsed=false;

        for (i=0;i<m_uCommonTextureFormats;i++)
        {
            if (strcmp(m_rgTexFormats[i].szShortName,"32-8888")==0)
                m_rgTexFormats[i].bUsed=true;
        }
    }
*/
    if (!KeySet("PERSPECTIVE") && !m_bFog)
    {
        for (i=1;i<m_cVertexMax;i++)
        {
            m_rgTLVertex[i].sz=m_rgTLVertex[0].sz;
            m_rgTLVertex[i].rhw=m_rgTLVertex[0].rhw;
        }
    }

    ReadInteger("STAGEMAX",-1,&m_cStageMax);
	if (m_cStageMax>8)
		m_cStageMax=8;
    if (m_cStageMax==-1)
    {
        // Make sure card supports at least 1 blend stage
#ifndef UNDER_XBOX
        m_cStageMax=m_d3dcaps.MaxTextureBlendStages;
        if (m_cStageMax<1)
        {
            WriteToLog("Driver does not support 1 blend stage.\n");
            SetTestRange(1,1);
            return D3DTESTINIT_SKIPALL;
        }
#else
        m_cStageMax = D3DTSS_MAXSTAGES;
#endif
    }

    if (KeySet("!ALLBLENDMODES"))
    {
        for (i=0;i<MAX_BLENDMODES;i++)
            m_rgBlendModes[i].bUsed=false;
    }

    if (KeySet("!ALLTEXFORMATS"))
    {
        for (i=0;i<(int)m_uCommonTextureFormats;i++)
            m_rgTexFormats[i].bUsed=false;
    }

    if (KeySet("MULTIPASS"))
        m_bMultiPass=true;

    // Check if COMBINATION was used, default is 2, min is 1, max is MAX_BLENDMODES
    ReadInteger("COMBINATION",2,&m_cCombinationMax);
    if (m_cCombinationMax<1)
        m_cCombinationMax=1;
    if (m_cCombinationMax>MAX_BLENDMODES)
        m_cCombinationMax=MAX_BLENDMODES;

    // Find out which texture formats will be used
    for (UINT iFormat=0;iFormat<m_uCommonTextureFormats;iFormat++)
    {
        // Check if removing a texture format
        strcpy(szStr,"!");
        strcat(szStr,m_rgTexFormats[iFormat].szShortName);
        if (KeySet(szStr))
            m_rgTexFormats[iFormat].bUsed=false;

        // Check if adding a texture format
        strcpy(szStr,"+");
        strcat(szStr,m_rgTexFormats[iFormat].szShortName);
        if (KeySet(szStr))
            m_rgTexFormats[iFormat].bUsed=true;
    }

    // Find out which texture blend modes will be used
    for (UINT iBlend=0;iBlend<MAX_BLENDMODES;iBlend++)
    {
        // Check if removing a texture blend mode, or remove it if not supported
        strcpy(szStr,"!");
        strcat(szStr,m_rgBlendModes[iBlend].szShortName);
        if (KeySet(szStr) || !(m_d3dcaps.TextureOpCaps&(DWORD)pow(2,iBlend)))
            m_rgBlendModes[iBlend].bUsed=false;

        // Check if adding a texture blend mode and check if it is supported
        strcpy(szStr,"+");
        strcat(szStr,m_rgBlendModes[iBlend].szShortName);
        if (KeySet(szStr) && (m_d3dcaps.TextureOpCaps&(DWORD)pow(2,iBlend)))
            m_rgBlendModes[iBlend].bUsed=true;
    }

    // Load all stages that will be used
    if (!LoadStages())
    {
        WriteToLog("Driver does not support enough blend stages.  Use a lower COMBINATION.\n");
        SetTestRange(1,1);
        return D3DTESTINIT_SKIPALL;
    }

    // Find out the # of tests per texture format
    m_uTestMax=0;
    for (i=0;i<m_cStageMax;i++)
        m_uTestMax+=m_uStageListMax*(UINT)pow(m_cCombinationMax,i);

    // Find out the # of texture formats used
    for (i=0;i<(int)m_uCommonTextureFormats;i++)
        if (m_rgTexFormats[i].bUsed)
            m_uTexFormatMax++;

    // Set Total # of tests
    m_uTestMax*=m_uTexFormatMax;
    SetTestRange(1,m_uTestMax);

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

    return D3DTESTINIT_RUN;
}

bool CTextureStage::ExecuteTest(UINT uTestNum)
{
    UINT iStageList;
    UINT uTest=uTestNum-1;
    UINT uTexFormat;
    HRESULT hRes=NULL;
    DWORD dwPasses=0;
#if 0
    DEVICEDESC SrcDesc=m_pAdapter->Devices[m_pMode->nSrcDevice].Desc;
#endif
    char szBuffer[256];

#ifdef DEMO_HACK // The Win32 NV20 driver doesn't correctly handle D3DTA_SPECULAR as an alpha argument, causing verification to incorrectly report failure
    m_pDisplay->EnableFrameVerification(TRUE);
#endif

    // Find out which texture format # we are at
    uTexFormat=uTest/(m_uTestMax/m_uTexFormatMax);
    uTest-=uTexFormat*(m_uTestMax/m_uTexFormatMax);

    // Find the texture format that will be used
    for (UINT i=0;i<m_uCommonTextureFormats;i++)
    {
        if (m_rgTexFormats[i].bUsed)
        {
            if (uTexFormat==0)
            {
                nTextureFormat=i;
                break;
            }
            uTexFormat--;
        }
    }

    // Reset all stages to DISABLED
    for (int iStage=0;iStage<m_cStageMax;iStage++)
    {
        m_rgpStage[iStage]=&m_StageList;
        m_rgpStage[iStage]->SetStates(iStage);
    }

    // Find out which stage will be used and fill m_rgpStage[iStage]
    for (iStage=0;iStage<m_cStageMax;iStage++)
    {
        // Which stage are we testing?
        if (uTest>=(m_uStageListMax*(UINT)pow(m_cCombinationMax,iStage)))
        {
            uTest-=m_uStageListMax*(UINT)pow(m_cCombinationMax,iStage);
        }
        else
        {
            // Find out which CStage will be used
            iStageList=uTest/(UINT)pow(m_cCombinationMax,iStage)+1; // (+1) skip disabled test
            m_rgpStage[iStage]=m_StageList.GetStageList(iStageList);
            uTest=uTest%(UINT)pow(m_cCombinationMax,iStage);

            for (UINT i=0;i<(UINT)iStage;i++)
            {
                // For the previous stages, cycle thru the first m_cCombinationMax stages
                iStageList=(uTest%(UINT)pow(m_cCombinationMax,i+1))/(UINT)pow(m_cCombinationMax,i)+1; // (+1) skip disabled test
                m_rgpStage[i]=m_StageList.GetStageList(iStageList);
            }
            break;
        }
    }

    sprintf(szBuffer,"Texture blending with %d stage(s)",iStage+1);
//    BeginTestCase(szBuffer);
    WriteStatus("$gTexture format","$y%s",m_rgTexFormats[nTextureFormat].szLongName);

    m_bPassValidate=true;

    // Load textures, set states, and validate device
    for (iStage=0;iStage<m_cStageMax;iStage++)
    {
        m_uLastFailedStage=iStage;
        m_rgpStage[iStage]->WriteToStatus(iStage);
        m_rgpStage[iStage]->LoadTexture();
        if (m_bMultiPass)
            m_rgpStage[iStage]->SetStates(0);
        else
            m_rgpStage[iStage]->SetStates(iStage);

#if 0
        hRes=ValidateDevice(&dwPasses);
        if (FAILED(hRes))
        {
#if 0
            if ((hRes==D3DERR_UNSUPPORTEDCOLOROPERATION) &&
                (SrcDesc.dwTextureOpCaps&(DWORD)pow(2,m_rgpStage[iStage]->m_dwColorOp-1)) &&
                ((iStage==0) || (m_bMultiPass)))
            {
                // Error with ValidateDevice, should be supported
                WriteToLog("ValidateDevice should not have failed! On stage 0 and color op is supported in the caps.\n");
                m_bPassValidate=false;
                return true;                         // Do not skip the test so that it fails
            }
            else 
#endif
#if 0   // I don't know what brain death was going on here
        // but if the device says it cannot use the specified texture
        // format, then it's not a failure.
            if (hRes==D3DERR_WRONGTEXTUREFORMAT)
            {
                // Error with ValidateDevice or driver, texture format should be supported
                WriteToLog("ValidateDevice should not have failed! Texture format was enumerated.\n");
                m_bPassValidate=false;
                return true;                         // Do not skip the test so that it fails
            }
            else
#endif
            {
//                SkipTests(1);

                // Log test info
//                WriteToLog("Texture format: %s\n",m_rgTexFormats[m_pD3DMode->nTextureFormat].szLongName);
//                for (UINT i=0;i<=iStage;i++)
//                    m_rgpStage[i]->WriteToLog(i);

                return false;                        // Skip the test
            }
        }

        if (dwPasses>1)
        {
            WriteToLog("Skipping.. Test requires more than one pass\n");
            return false;
        }
#else
        dwPasses = 1;
#endif

#ifdef UNDER_XBOX
        // Using a disable texture op following a premodulate op is unsupported on Xbox
        if (iStage) {
            if (m_rgpStage[iStage-1]->m_dwColorOp == D3DTOP_PREMODULATE && m_rgpStage[iStage]->m_dwColorOp == D3DTOP_DISABLE) {
                m_pDevice->SetTextureStageState(iStage, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                m_pDevice->SetTextureStageState(iStage, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
            }
            if (m_rgpStage[iStage-1]->m_dwAlphaOp == D3DTOP_PREMODULATE && m_rgpStage[iStage]->m_dwAlphaOp == D3DTOP_DISABLE) {
                m_pDevice->SetTextureStageState(iStage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                m_pDevice->SetTextureStageState(iStage, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
            }
        }
#endif
    }

    BeginTestCase(szBuffer);

    return true;
}

void CTextureStage::SceneRefresh()
{
    if (BeginScene())
    {
        Clear(0x00000000);

        if (m_bPassValidate)
        {
            if (m_bMultiPass)
            {
                for (int iStage=0;iStage<m_cStageMax;iStage++)
                {
                    if ((m_rgpStage[iStage]->m_dwColorOp!=D3DTOP_DISABLE) ||
                        (m_rgpStage[iStage]->m_dwAlphaOp!=D3DTOP_DISABLE))
                    {
                        m_rgpStage[iStage]->SetStates(0);
                        RenderPrimitive(D3DPT_TRIANGLEFAN,D3DFVF_TLVERTEX,m_rgTLVertex,5,NULL,0,0);
                        RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,&m_rgTLVertex[5],3,NULL,0,0);
                    }
                }
            }
            else
            {
                RenderPrimitive(D3DPT_TRIANGLEFAN,D3DFVF_TLVERTEX,m_rgTLVertex,5,NULL,0,0);
                RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,&m_rgTLVertex[5],3,NULL,0,0);
            }
        }

        EndScene();
    }
}

bool CTextureStage::TestTerminate()
{
#ifdef DEMO_HACK // The Win32 NV20 driver doesn't correctly handle D3DTA_SPECULAR as an alpha argument, causing verification to incorrectly report failure
    m_pDisplay->EnableFrameVerification(TRUE);
#endif
    return true;
}

bool CTextureStage::ProcessFrame(void)
{
    char    szBuffer[80];
    static  int nPass = 0;
    static  int nFail = 0;
    bool    bResult;

     // Use the standard 15%
    if (m_bPassValidate)
        bResult=GetCompareResult(0.15f, 0.85f, 0);
    else
        bResult=false;

    // Tell the logfile how the compare went
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

        // Log test info
        WriteToLog("Texture format: %s\n",m_rgTexFormats[nTextureFormat].szLongName);
        for (UINT iStage=0;iStage<=m_uLastFailedStage;iStage++)
            m_rgpStage[iStage]->WriteToLog(iStage);
    }

//    for (UINT iStage=0;iStage<=m_uLastFailedStage;iStage++)
//        m_rgpStage[iStage]->WriteToStatus(iStage);

    WriteStatus("$gOverall Results",szBuffer);

    return bResult;
}

BOOL CTextureStage::LoadStages()
// Adds CStages into linked list
// NOTE: First node of linked list has ColorOp=DISABLED and AlphaOp=DISABLED
//       The first m_cCombinationMax nodes in the list will be repeated in all possible
//        combinations for every stage.
{
    CStage *pStage;
    DWORD rgdwColor1[4]={0x992222FF,0x99FF2222,0x9922FF22,0x99FF22FF};
    DWORD rgdwColor2[4]={0x77AAAA22,0x7722AAAA,0x77AA22AA,0x77222222};
    DWORD rgdwColor3[4]={0xFFAA2222,0x22AA2222,0xFF2222AA,0x222222AA};
    DWORD rgdwColor4[4]={0xFFFF2222,0x00FF2222,0xFF2222FF,0x002222FF};
    DWORD rgdwColor5[4]={0x0000FF00,0x0000FF00,0x0000FF00,0x0000FF00};
    DWORD rgdwColor6[4]={0xD00000FF,0xE000FFFF,0xF0FFFF00,0xFFFF0000};

    if (m_rgBlendModes[D3DTOP_ADDSIGNED-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_ADDSIGNED;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_TEXTURE;
            pStage->m_dwAlphaOp=D3DTOP_ADDSIGNED;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor6,sizeof(rgdwColor6));
        }
    }

    if (m_rgBlendModes[D3DTOP_MODULATE-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_DIFFUSE;
            pStage->m_dwColorOp=D3DTOP_MODULATE;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_DIFFUSE;
            pStage->m_dwAlphaOp=D3DTOP_MODULATE;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
        }
    }

    if (m_rgBlendModes[D3DTOP_MODULATE-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_MODULATE;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_TEXTURE;
            pStage->m_dwAlphaOp=D3DTOP_MODULATE;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor1,sizeof(rgdwColor1));
        }
    }

    if (m_rgBlendModes[D3DTOP_PREMODULATE-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_PREMODULATE;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_TEXTURE;
            pStage->m_dwAlphaOp=D3DTOP_PREMODULATE;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor1,sizeof(rgdwColor4));
        }
    }

    if (m_rgBlendModes[D3DTOP_SELECTARG1-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_DIFFUSE;
            pStage->m_dwColorOp=D3DTOP_SELECTARG1;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_TEXTURE;
            pStage->m_dwAlphaOp=D3DTOP_SELECTARG2;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor1,sizeof(rgdwColor1));
        }
    }

    if (m_rgBlendModes[D3DTOP_ADD-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_ADD;
            pStage->m_dwColorArg2=D3DTA_SPECULAR;
            pStage->m_dwAlphaArg1=D3DTA_SPECULAR;
            pStage->m_dwAlphaOp=D3DTOP_ADD;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor3,sizeof(rgdwColor3));
        }
    }

    if (m_rgBlendModes[D3DTOP_MODULATE-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_MODULATE;
            pStage->m_dwColorArg2=D3DTA_DIFFUSE;
            pStage->m_dwAlphaArg1=D3DTA_DIFFUSE;
            pStage->m_dwAlphaOp=D3DTOP_SELECTARG1;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor3,sizeof(rgdwColor3));
        }
    }

    if (m_rgBlendModes[D3DTOP_MODULATE2X-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_MODULATE2X;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_DIFFUSE;
            pStage->m_dwAlphaOp=D3DTOP_SELECTARG1;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor2,sizeof(rgdwColor2));
        }
    }

    if (m_rgBlendModes[D3DTOP_MODULATE4X-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_MODULATE4X;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_DIFFUSE;
            pStage->m_dwAlphaOp=D3DTOP_SELECTARG1;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor2,sizeof(rgdwColor2));
        }
    }

    if (m_rgBlendModes[D3DTOP_ADD-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TFACTOR;
            pStage->m_dwColorOp=D3DTOP_ADD;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_TFACTOR;
            pStage->m_dwAlphaOp=D3DTOP_MODULATE;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor3,sizeof(rgdwColor3));
        }
    }

    if (m_rgBlendModes[D3DTOP_ADDSIGNED-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_ADDSIGNED;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_DIFFUSE;
            pStage->m_dwAlphaOp=D3DTOP_ADDSIGNED;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor2,sizeof(rgdwColor2));
        }
    }

    if (m_rgBlendModes[D3DTOP_ADDSIGNED2X-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_ADDSIGNED2X;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_DIFFUSE;
            pStage->m_dwAlphaOp=D3DTOP_SELECTARG1;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor2,sizeof(rgdwColor2));
        }
    }

    if (m_rgBlendModes[D3DTOP_SUBTRACT-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_SUBTRACT;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_TEXTURE;
            pStage->m_dwAlphaOp=D3DTOP_ADD;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor2,sizeof(rgdwColor2));
        }
    }

    if (m_rgBlendModes[D3DTOP_ADDSMOOTH-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_ADDSMOOTH;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_DIFFUSE;
            pStage->m_dwAlphaOp=D3DTOP_SELECTARG1;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor2,sizeof(rgdwColor2));
        }
    }

    if (m_rgBlendModes[D3DTOP_BLENDDIFFUSEALPHA-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_BLENDDIFFUSEALPHA;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_DIFFUSE;
            pStage->m_dwAlphaOp=D3DTOP_ADD;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor3,sizeof(rgdwColor3));
        }
    }

    if (m_rgBlendModes[D3DTOP_BLENDTEXTUREALPHA-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_BLENDTEXTUREALPHA;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_TEXTURE;
            pStage->m_dwAlphaOp=D3DTOP_BLENDTEXTUREALPHA;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor4,sizeof(rgdwColor4));
        }
    }

    if (m_rgBlendModes[D3DTOP_BLENDTEXTUREALPHA-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_BLENDTEXTUREALPHA;
            pStage->m_dwColorArg2=D3DTA_TFACTOR;
            pStage->m_dwAlphaArg1=D3DTA_TEXTURE;
            pStage->m_dwAlphaOp=D3DTOP_BLENDTEXTUREALPHA;
            pStage->m_dwAlphaArg2=D3DTA_TFACTOR;
            memcpy(pStage->m_rgdwColor,rgdwColor4,sizeof(rgdwColor4));
        }
    }

    if (m_rgBlendModes[D3DTOP_BLENDFACTORALPHA-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_BLENDFACTORALPHA;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_TEXTURE;
            pStage->m_dwAlphaOp=D3DTOP_ADD;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor4,sizeof(rgdwColor4));
        }
    }

    if (m_rgBlendModes[D3DTOP_BLENDTEXTUREALPHAPM-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_DIFFUSE;
            pStage->m_dwColorOp=D3DTOP_BLENDTEXTUREALPHAPM;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_DIFFUSE;
            pStage->m_dwAlphaOp=D3DTOP_ADD;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor4,sizeof(rgdwColor4));
        }
    }

    if (m_rgBlendModes[D3DTOP_BLENDCURRENTALPHA-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_BLENDCURRENTALPHA;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_DIFFUSE;
            pStage->m_dwAlphaOp=D3DTOP_MODULATE;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor5,sizeof(rgdwColor5));
        }
    }

    if (m_rgBlendModes[D3DTOP_MODULATEALPHA_ADDCOLOR-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_MODULATEALPHA_ADDCOLOR;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_DIFFUSE;
            pStage->m_dwAlphaOp=D3DTOP_MODULATE;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor3,sizeof(rgdwColor3));
        }
    }

    if (m_rgBlendModes[D3DTOP_MODULATECOLOR_ADDALPHA-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_MODULATECOLOR_ADDALPHA;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_DIFFUSE;
            pStage->m_dwAlphaOp=D3DTOP_MODULATE;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor3,sizeof(rgdwColor3));
        }
    }

    if (m_rgBlendModes[D3DTOP_MODULATEINVALPHA_ADDCOLOR-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_MODULATEINVALPHA_ADDCOLOR;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_TEXTURE;
            pStage->m_dwAlphaOp=D3DTOP_SELECTARG1;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor6,sizeof(rgdwColor6));
        }
    }

    if (m_rgBlendModes[D3DTOP_MODULATEINVCOLOR_ADDALPHA-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TEXTURE;
            pStage->m_dwColorOp=D3DTOP_MODULATEINVCOLOR_ADDALPHA;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_DIFFUSE;
            pStage->m_dwAlphaOp=D3DTOP_SELECTARG1;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor5,sizeof(rgdwColor5));
        }
    }

    if (m_rgBlendModes[D3DTOP_SELECTARG1-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorArg1=D3DTA_TFACTOR;
            pStage->m_dwColorOp=D3DTOP_SELECTARG1;
            pStage->m_dwColorArg2=D3DTA_CURRENT;
            pStage->m_dwAlphaArg1=D3DTA_TFACTOR;
            pStage->m_dwAlphaOp=D3DTOP_SELECTARG1;
            pStage->m_dwAlphaArg2=D3DTA_CURRENT;
            memcpy(pStage->m_rgdwColor,rgdwColor3,sizeof(rgdwColor3));
        }
    }

    if (m_rgBlendModes[D3DTOP_DISABLE-1].bUsed)
    {
        pStage=m_StageList.Create();
        if (pStage!=NULL)
        {
            pStage->m_dwColorOp=D3DTOP_DISABLE;
            pStage->m_dwAlphaOp=D3DTOP_DISABLE;
            memcpy(pStage->m_rgdwColor,rgdwColor5,sizeof(rgdwColor5));
        }
    }

    m_uStageListMax=m_StageList.Count()-1;          // Don't count DISABLED CStage (fist CStage)
    if (m_uStageListMax<(UINT)m_cCombinationMax)          // Combination max must be > list max
        return false;

    return true;
}

void CTextureStage::CreateTexFormatNames()
// Creates texture format short name (code name), and long name
{
//    PIXELFORMAT *pDDPF;
    DWORD dwAlphaBitCount = 0;
    DWORD dwRedBitCount   = 0;
    DWORD dwGreenBitCount = 0;
    DWORD dwBlueBitCount  = 0;
    DWORD dwMask;
    char szStr[50];
    ARGBPIXELDESC pixd;

    for (UINT i=0;i<m_uCommonTextureFormats;i++)
    {
//        pDDPF=&m_pCommonTextureFormats[i].ddpfPixelFormat;

        GetARGBPixelDesc(m_fmtCommon[i], &pixd);

        // Count number of bits for each color component
        dwAlphaBitCount=0;
        dwRedBitCount=0;
        dwGreenBitCount=0;
        dwBlueBitCount=0;
        for (dwMask = pixd.dwAMask; dwMask; dwMask>>=1)
            dwAlphaBitCount += (dwMask & 0x1);
        for (dwMask = pixd.dwRMask; dwMask; dwMask>>=1)
            dwRedBitCount += (dwMask & 0x1);
        for (dwMask = pixd.dwGMask; dwMask; dwMask>>=1)
            dwGreenBitCount += (dwMask & 0x1);
        for (dwMask = pixd.dwBMask; dwMask; dwMask>>=1)
            dwBlueBitCount += (dwMask & 0x1);

//        if (pDDPF->dwFourCC==MAKEFOURCC('U','Y','V','Y'))
        if (m_fmtCommon[i] == D3DFMT_UYVY)
        {
            strcpy(m_rgTexFormats[i].szShortName,"UYVY");
            strcpy(m_rgTexFormats[i].szLongName,"UYVY");
        }
//        else if (pDDPF->dwFourCC==MAKEFOURCC('Y','U','Y','2'))
        else if (m_fmtCommon[i] == D3DFMT_YUY2)
        {
            strcpy(m_rgTexFormats[i].szShortName,"YUY2");
            strcpy(m_rgTexFormats[i].szLongName,"YUY2");
        }
//        else if (pDDPF->dwFourCC==MAKEFOURCC('D','X','T','1'))
        else if (m_fmtCommon[i] == D3DFMT_DXT1)
        {
            strcpy(m_rgTexFormats[i].szShortName,"DXT1");
            strcpy(m_rgTexFormats[i].szLongName,"DXT1");
        }
//        else if (pDDPF->dwFourCC==MAKEFOURCC('D','X','T','2'))
        else if (m_fmtCommon[i] == D3DFMT_DXT2)
        {
            strcpy(m_rgTexFormats[i].szShortName,"DXT2");
            strcpy(m_rgTexFormats[i].szLongName,"DXT2");
        }
//        else if (pDDPF->dwFourCC==MAKEFOURCC('D','X','T','3'))
        else if (m_fmtCommon[i] == D3DFMT_DXT3)
        {
            strcpy(m_rgTexFormats[i].szShortName,"DXT3");
            strcpy(m_rgTexFormats[i].szLongName,"DXT3");
        }
//        else if (pDDPF->dwFourCC==MAKEFOURCC('D','X','T','4'))
        else if (m_fmtCommon[i] == D3DFMT_DXT4)
        {
            strcpy(m_rgTexFormats[i].szShortName,"DXT4");
            strcpy(m_rgTexFormats[i].szLongName,"DXT4");
        }
//        else if (pDDPF->dwFourCC==MAKEFOURCC('D','X','T','5'))
        else if (m_fmtCommon[i] == D3DFMT_DXT5)
        {
            strcpy(m_rgTexFormats[i].szShortName,"DXT5");
            strcpy(m_rgTexFormats[i].szLongName,"DXT5");
        }
//        else if (pDDPF->dwFlags&DDPF_PALETTEINDEXED4)
//        {
//            strcpy(m_rgTexFormats[i].szShortName,"PAL4");
//            strcpy(m_rgTexFormats[i].szLongName,"4 bit PAL4");
//        }
//        else if (pDDPF->dwFlags&DDPF_PALETTEINDEXED8)
        else if (m_fmtCommon[i] == D3DFMT_P8)
        {
            strcpy(m_rgTexFormats[i].szShortName,"PAL8");
            strcpy(m_rgTexFormats[i].szLongName,"8 bit PAL8");
        }
        else
        {
//            _itoa(pDDPF->dwRGBBitCount,szStr,10);
            _itoa(FormatToBitDepth(m_fmtCommon[i]),szStr,10);
            strcpy(m_rgTexFormats[i].szShortName,szStr);
            strcpy(m_rgTexFormats[i].szLongName,szStr);
            strcat(m_rgTexFormats[i].szShortName,"-");
            strcat(m_rgTexFormats[i].szLongName," bit ");
            _itoa(dwAlphaBitCount,szStr,10);
            strcat(m_rgTexFormats[i].szShortName,szStr);
            strcat(m_rgTexFormats[i].szLongName,szStr);
            _itoa(dwRedBitCount,szStr,10);
            strcat(m_rgTexFormats[i].szShortName,szStr);
            strcat(m_rgTexFormats[i].szLongName,szStr);
            _itoa(dwGreenBitCount,szStr,10);
            strcat(m_rgTexFormats[i].szShortName,szStr);
            strcat(m_rgTexFormats[i].szLongName,szStr);
            _itoa(dwBlueBitCount,szStr,10);
            strcat(m_rgTexFormats[i].szShortName,szStr);
            strcat(m_rgTexFormats[i].szLongName,szStr);

//            if (pDDPF->dwFlags&DDPF_BUMPDUDV)
            switch (m_fmtCommon[i]) {
                case D3DFMT_V8U8:
                case D3DFMT_L6V5U5:
                case D3DFMT_X8L8V8U8:
                case D3DFMT_Q8W8V8U8:
                case D3DFMT_V16U16:
                case D3DFMT_W11V11U10:
                    strcat(m_rgTexFormats[i].szShortName,"B");
                    strcat(m_rgTexFormats[i].szLongName," BUMPDUDV");
            }
//            if (pDDPF->dwFlags&DDPF_LUMINANCE)
            switch (m_fmtCommon[i]) {
                case D3DFMT_L6V5U5:
                case D3DFMT_X8L8V8U8:
                case D3DFMT_L8:
                case D3DFMT_A8L8:
                case D3DFMT_A4L4:
                    strcat(m_rgTexFormats[i].szShortName,"L");
                    strcat(m_rgTexFormats[i].szLongName," LUMINANCE");
            }
        }
    }
}

CDisplay* CTextureStage::GetDisplay(void) {

    return m_pDisplay;
}

HRESULT CTextureStage::ValidateDevice(DWORD *pdwPasses)
{
//    HRESULT hRes=NULL;

    // Check appropriate ref device
//    if (m_dwVersion >= 0x0800)
        return m_pDevice->ValidateDevice( pdwPasses );
//    else if (g_TextureStage.m_pRefDevice7!=NULL)
//        hRes=g_TextureStage.m_pRefDevice7->ValidateDevice(pdwPasses);
//    else if (g_TextureStage.m_pRefDevice3!=NULL)
//        hRes=g_TextureStage.m_pRefDevice3->ValidateDevice(pdwPasses);
/*
    if (FAILED(hRes))
    {
#if 0
        switch (hRes)
        {
            case D3DERR_CONFLICTINGTEXTUREFILTER:   g_TextureStage.WriteToLog("Validate Ref: Conflicting texture filter\n"); break;
            case D3DERR_CONFLICTINGTEXTUREPALETTE:  g_TextureStage.WriteToLog("Validate Ref: Conflicting texture palette\n"); break;
            case D3DERR_TOOMANYOPERATIONS:          g_TextureStage.WriteToLog("Validate Ref: Too many operations\n"); break;
            case D3DERR_UNSUPPORTEDALPHAARG:        g_TextureStage.WriteToLog("Validate Ref: Unsupported alpha arg\n"); break;
            case D3DERR_UNSUPPORTEDALPHAOPERATION:  g_TextureStage.WriteToLog("Validate Ref: Unsupported alpha op\n"); break;
            case D3DERR_UNSUPPORTEDCOLORARG:        g_TextureStage.WriteToLog("Validate Ref: Unsupported color arg\n"); break;
            case D3DERR_UNSUPPORTEDCOLOROPERATION:  g_TextureStage.WriteToLog("Validate Ref: Unsupported color op\n"); break;
            case D3DERR_UNSUPPORTEDFACTORVALUE:     g_TextureStage.WriteToLog("Validate Ref: Unsupported factor value\n"); break;
            case D3DERR_UNSUPPORTEDTEXTUREFILTER:   g_TextureStage.WriteToLog("Validate Ref: Unsupported texture filter\n"); break;
            case D3DERR_WRONGTEXTUREFORMAT:         g_TextureStage.WriteToLog("Validate Ref: Wrong texture format\n"); break;
        }
#endif
        return hRes;
    }

    // Check appropriate src device
    if (g_TextureStage.m_pSrcDevice7!=NULL)
        hRes=g_TextureStage.m_pSrcDevice7->ValidateDevice(pdwPasses);
    else if (g_TextureStage.m_pSrcDevice3!=NULL)
        hRes=g_TextureStage.m_pSrcDevice3->ValidateDevice(pdwPasses);

    if (FAILED(hRes))
    {
#if 0
        switch (hRes)
        {
            case D3DERR_CONFLICTINGTEXTUREFILTER:  g_TextureStage.WriteToLog("Validate Src: Conflicting texture filter\n"); break;
            case D3DERR_CONFLICTINGTEXTUREPALETTE: g_TextureStage.WriteToLog("Validate Src: Conflicting texture palette\n"); break;
            case D3DERR_TOOMANYOPERATIONS:         g_TextureStage.WriteToLog("Validate Src: Too many operations\n"); break;
            case D3DERR_UNSUPPORTEDALPHAARG:       g_TextureStage.WriteToLog("Validate Src: Unsupported alpha arg\n"); break;
            case D3DERR_UNSUPPORTEDALPHAOPERATION: g_TextureStage.WriteToLog("Validate Src: Unsupported alpha op\n"); break;
            case D3DERR_UNSUPPORTEDCOLORARG:       g_TextureStage.WriteToLog("Validate Src: Unsupported color arg\n"); break;
            case D3DERR_UNSUPPORTEDCOLOROPERATION: g_TextureStage.WriteToLog("Validate Src: Unsupported color op\n"); break;
            case D3DERR_UNSUPPORTEDFACTORVALUE:    g_TextureStage.WriteToLog("Validate Src: Unsupported factor value\n"); break;
            case D3DERR_UNSUPPORTEDTEXTUREFILTER:  g_TextureStage.WriteToLog("Validate Src: Unsupported texture filter\n"); break;
            case D3DERR_WRONGTEXTUREFORMAT:        g_TextureStage.WriteToLog("Validate Src: Wrong texture format\n"); break;
        }
#endif
        return hRes;
    }

    return hRes;
*/
}
