// FILE:        scalefvftl.cpp
// DESC:        scale class methods for point sprite conformance test
// AUTHOR:      Todd M. Frost
// COMMENTS:    D3DFVF_TLVERTEX

#define STRICT
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "PointSprite.h"

// NAME:        CSpriteScaleFVFTL()
// DESC:        constructor for sprite scale class (D3DFVF_TLVERTEX)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFTL::CSpriteScaleFVFTL(VOID)
{
    m_szTestName=TEXT("Point sprite scale (D3DFVF_TLVERTEX)");
    m_szCommandKey=TEXT("ScaleFVFTL");

    m_dvPointSizeMax = 0.0f;
}

// NAME:        ~CSpriteScaleFVFTL()
// DESC:        destructor for sprite scale class (D3DFVF_TLVERTEX)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFTL::~CSpriteScaleFVFTL(VOID)
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSpriteScaleFVFTL::CommandLineHelp(void)
{
     CSprite::CommandLineHelp();
	 return;
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteScaleFVFTL::TestInitialize(VOID)
{
    UINT uResult = CSprite::TestInitialize();

    SetTestRange((UINT) 1, (UINT) (m_nFormats*m_nVariationsPerFormat*NCLIPTESTS));	// *2 for the POINTSPRITEENABLE off/on states divide 2 because there are always two point sprites on screen.
    return uResult;
}


// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSpriteScaleFVFTL::ExecuteTest(UINT uTest)
{
    bool bResult = true;
    D3DVALUE dvT;

	bool	bPSEnable = (uTest&1);
	uTest = (uTest + 1) / 2;
	UINT	nPosition = (uTest-1) % NCLIPTESTS;
	uTest = (uTest + NCLIPTESTS -1) / NCLIPTESTS;
    dvT = ((D3DVALUE) ((uTest - 1) % m_nVariationsPerFormat))/((D3DVALUE) (m_nVariationsPerFormat - 1));
    m_Sprite.PointSize.f = dvT*(m_dvPointSizeMax * 1.5f);

    if (!CSprite::ExecuteTest(uTest))
        return false;

	// turn on/off texture stretching for this point
    SetRenderState(D3DRS_POINTSPRITEENABLE, (DWORD) bPSEnable);

    sprintf(m_szBuffer, "Point size (state):  %f", m_Sprite.PointSize.f);
    BeginTestCase(m_szBuffer);

    SwitchAPI((UINT)m_fFrame);

    m_pDevice->SetViewport(&m_vpFull);
    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, RGB_MAKE(74, 150, 202), 1.0f, 0);

    m_pDevice->SetViewport(&m_vpTest);

    ClearFrame();

    if (BeginScene())
    {
        DWORD dwFormat = MYD3DFVF_TLVERTEX;
        MYD3DTLVERTEX Vertex[2];

		SetPosition(nPosition, &Vertex[0].dvSX, &Vertex[0].dvSY);
        Vertex[0].dvSZ = m_ViewportCenter.z;
        Vertex[0].dvRHW = 1.0f;
        Vertex[0].dcColor = m_dcBackground;
        Vertex[0].dcSpecular = m_dcBackground;
        Vertex[0].dvTU = 0.75f;
        Vertex[0].dvTV = 0.50f;

		SetPosition((NCLIPTESTS - 1) - nPosition, &Vertex[1].dvSX, &Vertex[1].dvSY);
        Vertex[1].dvSZ = m_ViewportCenter.z + .01f;
        Vertex[1].dvRHW = 1.0f;
        Vertex[1].dcColor = m_dcBackground;
        Vertex[1].dcSpecular = m_dcBackground;
        Vertex[1].dvTU = 0.55f;
        Vertex[1].dvTV = 0.70f;

        if (!RenderPrimitive(D3DPT_POINTLIST, dwFormat, &Vertex, (DWORD) 2))
        {
            WriteToLog("RenderPrimitive() failed.\n");
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
