// FILE:        batch.cpp
// DESC:        batch methods for point sprite conformance test
// AUTHOR:      Anthony Clarke
// COMMENTS:    The idea behind this test is to check that points sprites
//				are render correctly when changes occur in both
//				render state and vertex data.


#define STRICT
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "PointSprite.h"

MYD3DTLVERTEX g_Vertices[4] = 
{
    { 128,  172, .5, 1, D3DCOLOR_RGBA(255, 0, 0, 255), D3DCOLOR_RGBA(0, 0, 0, 255), 0, 1 },
    { 128,  108, .5, 1, D3DCOLOR_RGBA(0, 255, 0, 255), D3DCOLOR_RGBA(0, 0, 0, 255), 0, 0 },
	{ 192,  172, .5, 1, D3DCOLOR_RGBA(255, 255, 255, 255), D3DCOLOR_RGBA(255, 255, 255, 255), 1, 1 },
    { 192,  108, .5, 1, D3DCOLOR_RGBA(0, 0, 255, 255), D3DCOLOR_RGBA(255, 255, 255, 255), 1, 0 }
};



// NAME:        CSpriteBatch()
// DESC:        
// INPUT:       none
// OUTPUT:      none

CSpriteBatch::CSpriteBatch(VOID)
{
    m_szTestName=TEXT("Point sprite batches");
    m_szCommandKey=TEXT("Batch");

    m_dvPointSizeMax = 0.0f;
	m_fChangeMode = false;
	return;
}

// NAME:        CSpriteBatch()
// DESC:        
// INPUT:       none
// OUTPUT:      none

CSpriteBatch::~CSpriteBatch(VOID)
{
	return;
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CSpriteBatch::CommandLineHelp(void)
{
    CSprite::CommandLineHelp();
	return;
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      appropriate D3DTESTINIT code

UINT CSpriteBatch::TestInitialize(VOID)
{   
	UINT uResult = CSprite::TestInitialize();
    	
	m_nVariationsPerFormat *= NCLIPTESTS / NUMPOINTSPRITES_ONSCREEN;
    m_nFormats++; //This one extra format is the texture off format.
	SetTestRange((UINT) 1, m_nVariationsPerFormat * m_nFormats);
	SetRenderState(D3DRS_POINTSCALEENABLE, (DWORD)FALSE);
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSpriteBatch::ExecuteTest(UINT uTest)
{
    bool bResult = true;
	int iFormat;

	iFormat = uTest / m_nVariationsPerFormat;
	// This is so the final no texture format is tested
	if (iFormat == m_nFormats-2) 
		 m_Flags.vClear(TEXTUREON);
    
	// every second sample change the shade mode	
	if (!(uTest % 2))
		m_fChangeMode = !m_fChangeMode;
	if (m_fChangeMode)
	{
		m_Flags.vSet(FLATSHADE);
	}
	else
	{		
		m_Flags.vClear(FLATSHADE);
	}

	if (!CSprite::ExecuteTest(uTest))
        return false;

    sprintf(m_szBuffer, "Sprite batch");
    BeginTestCase(m_szBuffer);

    SwitchAPI((UINT)m_fFrame);

    m_pDevice->SetViewport(&m_vpFull);
    m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, RGB_MAKE(74, 150, 202), 1.0f, 0);

    m_pDevice->SetViewport(&m_vpTest);

    ClearFrame();

	//// enable(odd)/disable(even) point sprites	
	m_dwPointSpriteEnable = (uTest & 1);
    SetRenderState(D3DRS_POINTSPRITEENABLE, (DWORD) m_dwPointSpriteEnable);

    if (BeginScene())
    {
        DWORD dwFormat = MYD3DFVF_TLVERTEX;
        MYD3DTLVERTEX Vertex;
		MYD3DTLVERTEXNOTEX VertexNoTex;

        Vertex.dvSX = 0.0;
        Vertex.dvSY = 0.0;
        Vertex.dvSZ = m_ViewportCenter.z;
        if (m_Flags.bCheck(TEXTUREON))
		{
			Vertex.dcColor = m_dcBackground;
			Vertex.dcSpecular = m_dcBackground;
		}
		else
		{
			Vertex.dcColor = D3DCOLOR_RGBA(0,255,0,255);
			Vertex.dcSpecular = D3DCOLOR_RGBA(255,255,255,255);
		}
		Vertex.dvRHW = 1.0f;
        Vertex.dvTU = 0.0f;
        Vertex.dvTV = 0.0f;		
		m_Sprite.PointSize.f = m_dvPointSizeMax;
		
		//With Texture Co-ordinates
		if (!DrawPointSprites(&Vertex, dwFormat, (uTest-1)%NCLIPTESTS, 2))
		{
			WriteToLog("DrawPointSprites FVF with Texture Co-Ords failed.\n");
			bResult = false;
		}
		
		// Now render a vertex just a vertex not a point sprite
		m_Sprite.PointSize.f = 1;
		if (!DrawPointSprites(&Vertex, dwFormat, (uTest+1)%NCLIPTESTS, 1))
		{
			WriteToLog("DrawPointSprites rendering a vertex FVF with Texture Co-Ords failed.\n");
			bResult = false;
		}

        SetRenderState(D3DRS_POINTSPRITEENABLE, (DWORD) FALSE);

		//Centre primative
		if (!RenderPrimitive(D3DPT_TRIANGLESTRIP, MYD3DFVF_TLVERTEX, g_Vertices, 4))
		{
			WriteToLog("RenderPrimitive() D3DPT_TRIANGLESTRIP failed.\n");
			bResult = false;
		}

        SetRenderState(D3DRS_POINTSPRITEENABLE, (DWORD) m_dwPointSpriteEnable);

		//Without Texture Co-ordinates
		//VertexNoTex
		dwFormat = MYD3DFVF_TLVERTEX_NOTEX;

		VertexNoTex.dvSX = 0.0;
        VertexNoTex.dvSY = 0.0;
        VertexNoTex.dvSZ = m_ViewportCenter.z;
        if (m_Flags.bCheck(TEXTUREON))
		{
			VertexNoTex.dcColor = m_dcBackground;
			VertexNoTex.dcSpecular = m_dcBackground;
		}
		else
		{
			VertexNoTex.dcColor = D3DCOLOR_RGBA(255,0,0,255);
			VertexNoTex.dcSpecular = D3DCOLOR_RGBA(255,255,255,255);
		}
		VertexNoTex.dvRHW = 1.0f;    
		m_Sprite.PointSize.f = m_dvPointSizeMax;

		if (!DrawPointSprites(&VertexNoTex, dwFormat, (uTest+2)%NCLIPTESTS, 2))
		{
			WriteToLog("DrawPointSprites FVF with out Texture Co-Ords failed.\n");
			bResult = false;
		}

		// Now render a vertex just a vertex not a point sprite
		m_Sprite.PointSize.f = 1;
		if (!DrawPointSprites(&VertexNoTex, dwFormat, (uTest+3)%NCLIPTESTS, 1))
		{
			WriteToLog("DrawPointSprites rendering a vertex FVF with out Texture Co-Ords failed.\n");
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

bool CSpriteBatch::ProcessFrame(void)
{
	DWORD dwTextureOn, dwShadeMode;
	bool fResult;

	fResult = CSprite::ProcessFrame();
	if (!fResult)
	{
		//TODO: m_dwPointSpriteEnable, this should be removed and GetRenderState
		// added to the framework....

		if (m_Flags.bCheck(TEXTUREON))
			dwTextureOn = 1;
		else
			dwTextureOn = 0;
		
		if (m_Flags.bCheck(FLATSHADE))
			dwShadeMode = D3DSHADE_FLAT;
		else
			dwShadeMode = D3DSHADE_GOURAUD;

		WriteToLog("RenderStates: ShadeMode %lu; PointSpriteEnable %lu; TexturesOn %lu\n", dwShadeMode, m_dwPointSpriteEnable, dwTextureOn);
	}
	return fResult;
}

// NAME:        DrawPointSprites()
// DESC:        simple wraper for drawing point sprites at increamental locations
// INPUT:       Vertex...  The point sprite vertex
//				dwFormat... The format of the vertex
//				StartPosNum... The start index into the NCLIPTESTS array
//				NumToDraw... The number of point sprites to draw.
// OUTPUT:      true.....if test is executed successfully
//              false....otherwise

bool CSpriteBatch::DrawPointSprites(LPVOID Vertex, DWORD dwFormat, UINT StartPosNum, UINT NumToDraw)
{
	int i;
	D3DVALUE *dvSX, *dvSY;

	if (!(D3DFVF_XYZRHW | dwFormat))
	{
		WriteToLog("DrawPointSprite failed. Points must be TnL\n");
		return false;
	}
	dvSX = (D3DVALUE*)Vertex;
	dvSY = (D3DVALUE*)(((BYTE*)Vertex)+sizeof(D3DVALUE));
	for (i = 0; i < (int)NumToDraw; ++i)
	{
		SetPosition((StartPosNum+i)%NCLIPTESTS, dvSX, dvSY);
		SetRenderState(D3DRS_POINTSIZE, F2DW(m_Sprite.PointSize.f) );
		if (!RenderPrimitive(D3DPT_POINTLIST, dwFormat, Vertex, (DWORD) 1))
		{
			WriteToLog("RenderPrimitive() D3DPT_POINTLIST failed.\n");
			return false;
		}
		m_Sprite.PointSize.f /= 2.0f;
	}
	return true;
}