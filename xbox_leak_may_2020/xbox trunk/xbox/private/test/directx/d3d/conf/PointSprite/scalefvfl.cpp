// FILE:        scalefvfl.cpp
// DESC:        scale class methods for point sprite conformance test
// AUTHOR:      Todd M. Frost
// COMMENTS:    D3DFVF_LVERTEX

#define STRICT
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "PointSprite.h"

// NAME:        CSpriteScaleFVFL()
// DESC:        constructor for sprite scale class (D3DFVF_LVERTEX)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFL::CSpriteScaleFVFL(VOID)
{
    m_szTestName=TEXT("Point sprite scale (D3DFVF_LVERTEX)");
    m_szCommandKey=TEXT("ScaleFVFL");

    m_dvPointSizeMax = 0.0f;
}

// NAME:        ~CSpriteScaleFVFL()
// DESC:        destructor for sprite scale class (D3DFVF_LVERTEX)
// INPUT:       none
// OUTPUT:      none

CSpriteScaleFVFL::~CSpriteScaleFVFL(VOID)
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSpriteScaleFVFL::CommandLineHelp(void)
{
     CSprite::CommandLineHelp();
	 return;
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteScaleFVFL::TestInitialize(VOID)
{
    UINT uResult = CSprite::TestInitialize();

    SetTestRange((UINT) 1, (UINT) (m_nFormats*m_nVariationsPerFormat));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSpriteScaleFVFL::ExecuteTest(UINT uTest)
{
    bool bResult = true;
    D3DVALUE dvT;

    dvT = ((D3DVALUE) ((uTest - 1) % m_nVariationsPerFormat))/((D3DVALUE) (m_nVariationsPerFormat - 1));
    m_Sprite.PointSize.f = dvT*(m_dvPointSizeMax*1.5f);

    if (!CSprite::ExecuteTest(uTest))
        return false;

    sprintf(m_szBuffer, "Point size (state):  %f", m_Sprite.PointSize.f);
    BeginTestCase(m_szBuffer);

    SwitchAPI((UINT)m_fFrame);

    m_pDevice->SetViewport(&m_vpFull);
    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, RGB_MAKE(74, 150, 202), 1.0f, 0);

    m_pDevice->SetViewport(&m_vpTest);

    ClearFrame();

    if (BeginScene())
    {
        DWORD dwFormat = MYD3DFVF_LVERTEX;
        MYD3DLVERTEX Vertex;

        Vertex.dvX = 0.0f;
        Vertex.dvY = 0.0f;
        Vertex.dvZ = 0.0f;
        Vertex.dcColor = m_dcBackground;
        Vertex.dcSpecular = m_dcBackground;
        Vertex.dvTU = 0.0f;
        Vertex.dvTV = 0.0f;

        if (!RenderPrimitive(D3DPT_POINTLIST, dwFormat, &Vertex, (DWORD) 1))
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
