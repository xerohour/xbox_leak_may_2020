// FILE:        scalefvftls.cpp
// DESC:        scale class methods for point sprite conformance test
// AUTHOR:      Todd M. Frost
// COMMENTS:    D3DFVF_TLVERTEX | D3DFVF_PSIZE

#define STRICT
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "PointSprite.h"

// NAME:        CSpriteScaleFVFTLS()
// DESC:        constructor for sprite scale class (D3DFVF_TLVERTEX | D3DFVF_PSIZE)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFTLS::CSpriteScaleFVFTLS(VOID)
{
    m_szTestName=TEXT("Point sprite scale (D3DFVF_TLVERTEX | D3DFVF_PSIZE)");
    m_szCommandKey=TEXT("ScaleFVFTLS");

    m_dvPointSizeMax = 0.0f;
}

// NAME:        ~CSpriteScaleFVFTLS()
// DESC:        destructor for sprite scale class (D3DFVF_TLVERTEX | D3DFVF_PSIZE)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFTLS::~CSpriteScaleFVFTLS(VOID)
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSpriteScaleFVFTLS::CommandLineHelp(void)
{
    CSprite::CommandLineHelp();
//    WriteCommandLineHelp("$yinvalid: $wrun invalid parameter tests$c($wYes$c/No)");
	return;
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteScaleFVFTLS::TestInitialize(VOID)
{
    UINT uResult = CSprite::TestInitialize();

    m_Sprite.PointSize.f = 1.0f;
    SetTestRange((UINT) 1, (UINT) (m_nFormats*m_nVariationsPerFormat*NCLIPTESTS*2));	// *2 for the POINTSPRITEENABLE off/on states

#ifndef UNDER_XBOX
    if (!m_Flags.bCheck(VERTEXPOINTSIZE) && !m_Flags.bCheck(INVALID))
        uResult = D3DTESTINIT_SKIPALL;
#endif

    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSpriteScaleFVFTLS::ExecuteTest(UINT uTest)
{
    bool bResult = true;
    D3DVALUE dvS, dvT;

	uTest = (uTest + 1) / 2;
	UINT nPosition = (uTest - 1) % NCLIPTESTS;
	uTest = (uTest + NCLIPTESTS -1) / NCLIPTESTS;
    dvT = ((D3DVALUE) ((uTest - 1) % m_nVariationsPerFormat))/((D3DVALUE) (m_nVariationsPerFormat - 1));
    dvS = dvT*(m_dvPointSizeMax*1.5f);

    if (!CSprite::ExecuteTest(uTest))
        return false;

	// enable(odd)/disable(even) point sprites

    SetRenderState(D3DRS_POINTSPRITEENABLE, (DWORD) (uTest & 1));

    sprintf(m_szBuffer, "Point size (vertex):  %f", dvS);
    BeginTestCase(m_szBuffer);

    SwitchAPI((UINT)m_fFrame);

    m_pDevice->SetViewport(&m_vpFull);
    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, RGB_MAKE(74, 150, 202), 1.0f, 0);

    m_pDevice->SetViewport(&m_vpTest);

    ClearFrame();

    if (BeginScene())
    {
        DWORD dwFormat = MYD3DFVF_TLSVERTEX;
        MYD3DTLSVERTEX Vertex;

		SetPosition(nPosition, &Vertex.dvSX, &Vertex.dvSY);
        Vertex.dvSZ = m_ViewportCenter.z;
        Vertex.dvRHW = 1.0f;
        Vertex.dcColor = m_dcBackground;
        Vertex.dcSpecular = m_dcBackground;
        Vertex.dvTU = 0.75f;
        Vertex.dvTV = 0.50f;
#ifndef UNDER_XBOX
        Vertex.dvS = dvS;

#else

        m_pDevice->SetRenderState(D3DRS_POINTSIZE, F2DW(dvS));
#endif

        if (!RenderPrimitive(D3DPT_POINTLIST, dwFormat, &Vertex, (DWORD) 1))
        {
            WriteToLog("RenderPrimitive() failed.\n");
            bResult = false;
        }

#ifdef UNDER_XBOX
        m_pDevice->SetRenderState(D3DRS_POINTSIZE, F2DW(1.0f));
#endif

        // Display the adapter mode
        ShowDisplayMode();

        // Display the frame rate
        ShowFrameRate();

        // Display the console
        m_pDisplay->ShowConsole();

        if (!EndScene())
        {
            WriteToLog("EndScene() failed.\n");
            bResult = false;
        }
    }
    else
    {
        WriteToLog("BeginScene() failed.\n");
        bResult = false;
    }

    // Update the screen
    m_pDevice->Present(NULL, NULL, NULL, NULL);

//    return bResult;
    return false;
}
