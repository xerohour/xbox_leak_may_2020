//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "WCmp.h"

//************************************************************************
// Greater Test functions

CWGreaterTest::CWGreaterTest()
{
	m_szTestName = TEXT("W Compare Greater");
	m_szCommandKey = TEXT("Greater");

	// Tell parent class what cap to look for
	dwWCmpCap = D3DPCMPCAPS_GREATER;
}

CWGreaterTest::~CWGreaterTest()
{
}

bool CWGreaterTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using the W Buffer and compare func Greater
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)TRUE);
	SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_USEW);
	SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_GREATER);
	return true;
}

void CWGreaterTest::SetupBackground(void)
{
	float Zval = 0.0f;
	float RHW  = (1.0f - 0.0f);
	float fWidth  = (float)m_vpTest.Width;//m_pSrcTarget->m_dwWidth - 1;
	float fHeight = (float)m_vpTest.Height;//m_pSrcTarget->m_dwHeight - 1;

	// Black Background Triangle
	BackgroundList[0] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + fWidth, (float)m_vpTest.Y + 0.0f,    Zval), RHW, RGBA_MAKE(0,0,0,255),0, 1.0f,0.0f);
	BackgroundList[1] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + fWidth, (float)m_vpTest.Y + fHeight, Zval), RHW, RGBA_MAKE(0,0,0,255),0, 1.0f,1.0f);
	BackgroundList[2] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 0.0f,   (float)m_vpTest.Y + 0.0f,    Zval), RHW, RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
	BackgroundList[3] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 0.0f,   (float)m_vpTest.Y + fHeight, Zval), RHW, RGBA_MAKE(0,0,0,255),0, 0.0f,1.0f);
}

void CWGreaterTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Special setup for initing the background
		SetupBackground();

		// Special render state needed
		SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_LESSEQUAL);

		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,BackgroundList,4,NULL,0,0);

		// Set the appropriate renderstate
		SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_GREATER);

		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

//************************************************************************
// GreaterEqual Test functions

CWGreaterEqualTest::CWGreaterEqualTest()
{
	m_szTestName = TEXT("W Compare GreaterEqual");
	m_szCommandKey = TEXT("GreaterEqual");

	// Tell parent class what cap to look for
	dwWCmpCap = D3DPCMPCAPS_GREATEREQUAL;
}

CWGreaterEqualTest::~CWGreaterEqualTest()
{
}

bool CWGreaterEqualTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using the W Buffer and compare func GreaterEqual
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)TRUE);
	SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_USEW);
	SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_GREATEREQUAL);
	return true;
}

void CWGreaterEqualTest::SetupBackground(void)
{
	float Zval = 0.0f;
	float RHW  = (1.0f - 0.0f);
	float fWidth  = (float)m_vpTest.Width;//m_pSrcTarget->m_dwWidth - 1;
	float fHeight = (float)m_vpTest.Height;//m_pSrcTarget->m_dwHeight - 1;

	// Black Background Triangle
	BackgroundList[0] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + fWidth, (float)m_vpTest.Y + 0.0f,    Zval), RHW, RGBA_MAKE(0,0,0,255),0, 1.0f,0.0f);
	BackgroundList[1] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + fWidth, (float)m_vpTest.Y + fHeight, Zval), RHW, RGBA_MAKE(0,0,0,255),0, 1.0f,1.0f);
	BackgroundList[2] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 0.0f,   (float)m_vpTest.Y + 0.0f,    Zval), RHW, RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
	BackgroundList[3] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 0.0f,   (float)m_vpTest.Y + fHeight, Zval), RHW, RGBA_MAKE(0,0,0,255),0, 0.0f,1.0f);
}

void CWGreaterEqualTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Special setup for initing the background
		SetupBackground();

		// Special render state needed
		SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_LESSEQUAL);

		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,BackgroundList,4,NULL,0,0);

		// Set the appropriate renderstate
		SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_GREATEREQUAL);

		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}


//************************************************************************
// Internal API:    GreaterTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CWCmpTest::GreaterTests(int nTest)
{
	switch (nTest)
	{
		case (1):	// Left Handed Flat Triangles
			if (dwWCmpCap == D3DPCMPCAPS_GREATER)
				sprintf(msgString, "%sThree flat triangles (left), Visible: Green, Red, & Blue",msgString);
			else
				sprintf(msgString, "%sThree flat triangles (left), Visible: Blue, Green, & Red",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f, 10.0f + (float)m_vpTest.Y,0.5f),(1.0f - 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f, 10.0f + (float)m_vpTest.Y,0.5f),(1.0f - 0.5f), RGBA_MAKE(255,0,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f,110.0f + (float)m_vpTest.Y,0.5f),(1.0f - 0.5f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  70.0f, 70.0f + (float)m_vpTest.Y,0.9f),(1.0f - 0.9f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 170.0f, 70.0f + (float)m_vpTest.Y,0.9f),(1.0f - 0.9f), RGBA_MAKE(0,255,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 170.0f,170.0f + (float)m_vpTest.Y,0.9f),(1.0f - 0.9f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 130.0f,130.0f + (float)m_vpTest.Y,0.9f),(1.0f - 0.9f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f,130.0f + (float)m_vpTest.Y,0.9f),(1.0f - 0.9f), RGBA_MAKE(0,0,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f,230.0f + (float)m_vpTest.Y,0.9f),(1.0f - 0.9f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			break;
		case (2):	// Right Handed Flat Triangles
			if (dwWCmpCap == D3DPCMPCAPS_GREATER)
				sprintf(msgString, "%sThree flat triangles (right), Visible: Green, Red, & Blue",msgString);
			else
				sprintf(msgString, "%sThree flat triangles (right), Visible: Blue, Green, & Red",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f, 10.0f + (float)m_vpTest.Y,0.7f),(1.0f - 0.7f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f,110.0f + (float)m_vpTest.Y,0.7f),(1.0f - 0.7f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f,110.0f + (float)m_vpTest.Y,0.7f),(1.0f - 0.7f), RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  70.0f, 70.0f + (float)m_vpTest.Y,0.75f),(1.0f - 0.75f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 170.0f,170.0f + (float)m_vpTest.Y,0.75f),(1.0f - 0.75f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  70.0f,170.0f + (float)m_vpTest.Y,0.75f),(1.0f - 0.75f), RGBA_MAKE(0,255,0,255),0, 0.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 130.0f,130.0f + (float)m_vpTest.Y,0.75f),(1.0f - 0.75f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f,230.0f + (float)m_vpTest.Y,0.75f),(1.0f - 0.75f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 130.0f,230.0f + (float)m_vpTest.Y,0.75f),(1.0f - 0.75f), RGBA_MAKE(0,0,255,255),0, 0.0f,1.0f);
			break;
		case (3):	// Single Flat Triangles
			if (dwWCmpCap == D3DPCMPCAPS_GREATER)
				sprintf(msgString, "%sThree flat triangles, Visible: Cyan & Magenta",msgString);
			else
				sprintf(msgString, "%sThree flat triangles, Visible: White, Cyan, & Magenta",msgString);

			// Magenta Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 150.0f, 10.0f + (float)m_vpTest.Y,0.11f),(1.0f - 0.11f), RGBA_MAKE(255,0,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 300.0f,270.0f + (float)m_vpTest.Y,0.11f),(1.0f - 0.11f), RGBA_MAKE(255,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   0.0f,270.0f + (float)m_vpTest.Y,0.11f),(1.0f - 0.11f), RGBA_MAKE(255,0,255,255),0, 0.0f,1.0f);
			// Teal Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 150.0f, 20.0f + (float)m_vpTest.Y,0.12f),(1.0f - 0.12f), RGBA_MAKE(0,255,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 280.0f,260.0f + (float)m_vpTest.Y,0.12f),(1.0f - 0.12f), RGBA_MAKE(0,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  20.0f,260.0f + (float)m_vpTest.Y,0.12f),(1.0f - 0.12f), RGBA_MAKE(0,255,255,255),0, 0.0f,1.0f);
			// White Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 150.0f, 30.0f + (float)m_vpTest.Y,0.12f),(1.0f - 0.12f), RGBA_MAKE(255,255,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 260.0f,250.0f + (float)m_vpTest.Y,0.12f),(1.0f - 0.12f), RGBA_MAKE(255,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  40.0f,250.0f + (float)m_vpTest.Y,0.12f),(1.0f - 0.12f), RGBA_MAKE(255,255,255,255),0, 0.0f,1.0f);
			break;
		case (4):	// Left Handed Non-Flat Triangles
			sprintf(msgString, "%sThree intersecting triangles (left), Visible: All",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f, 10.0f + (float)m_vpTest.Y,0.8f), (1.0f - 0.8f),  RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f, 10.0f + (float)m_vpTest.Y,0.75f),(1.0f - 0.75f), RGBA_MAKE(255,0,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f,110.0f + (float)m_vpTest.Y,0.75f),(1.0f - 0.75f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  70.0f, 70.0f + (float)m_vpTest.Y,0.8f), (1.0f - 0.8f),  RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 170.0f, 70.0f + (float)m_vpTest.Y,0.65f),(1.0f - 0.65f), RGBA_MAKE(0,255,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 170.0f,170.0f + (float)m_vpTest.Y,0.65f),(1.0f - 0.65f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 130.0f,130.0f + (float)m_vpTest.Y,0.7f), (1.0f - 0.7f),  RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f,130.0f + (float)m_vpTest.Y,0.65f),(1.0f - 0.65f), RGBA_MAKE(0,0,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f,230.0f + (float)m_vpTest.Y,0.65f),(1.0f - 0.65f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			break;
		case (5):	// Right Handed Non-Flat Triangles
			sprintf(msgString, "%sThree intersecting triangles (right), Visible: All",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f, 10.0f + (float)m_vpTest.Y,0.8f), (1.0f - 0.8f),  RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f,110.0f + (float)m_vpTest.Y,0.75f),(1.0f - 0.75f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f,110.0f + (float)m_vpTest.Y,0.8f), (1.0f - 0.8f),  RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  70.0f, 70.0f + (float)m_vpTest.Y,0.8f), (1.0f - 0.8f),  RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 170.0f,170.0f + (float)m_vpTest.Y,0.65f),(1.0f - 0.65f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  70.0f,170.0f + (float)m_vpTest.Y,0.7f), (1.0f - 0.7f),  RGBA_MAKE(0,255,0,255),0, 0.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 130.0f,130.0f + (float)m_vpTest.Y,0.7f), (1.0f - 0.7f),  RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f,230.0f + (float)m_vpTest.Y,0.65f),(1.0f - 0.65f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 130.0f,230.0f + (float)m_vpTest.Y,0.65f),(1.0f - 0.65f), RGBA_MAKE(0,0,255,255),0, 0.0f,1.0f);
			break;
		case (6):	// Combo Test: 0.0f
		case (7):	// Combo Test: 0.1f
		case (8):	// Combo Test: 0.2f
		case (9):	// Combo Test: 0.3f	
		case (10):	// Combo Test: 0.4f	
		case (11):	// Combo Test: 0.5f
		case (12):	// Combo Test: 0.6f
		case (13):	// Combo Test: 0.7f	
		case (14):	// Combo Test: 0.8f
		case (15):	// Combo Test: 0.9f
			if (nTest == 6)
				sprintf(msgString, "%sTwo triangles (%d%% overlap), Visible: 100%% Red",msgString,10 * (nTest - 6));
			else
				sprintf(msgString, "%sTwo triangles (%d%% overlap), Visible: %d%% Green & %d%% Red",msgString,(10 * (nTest - 6)),(10 * (nTest - 6)),100 - (10 * (nTest - 6)));

			ComboTests(0.1f * (nTest - 6));
			break;
		case (16):	// Combo Test: 0.99f
			sprintf(msgString, "%sTwo triangles (100%% overlap), Visible: 100%% Green",msgString);

			ComboTests(0.99f);
			break;
		default:
			break;
	}
}
