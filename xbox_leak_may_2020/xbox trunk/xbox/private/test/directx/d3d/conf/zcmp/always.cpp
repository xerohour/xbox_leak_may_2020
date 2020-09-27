//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "ZCmp.h"

//************************************************************************
// Never Test functions

CZNeverTest::CZNeverTest()
{
	m_szTestName = TEXT("Z Compare Never");
	m_szCommandKey = TEXT("Never");

	// Tell parent class what cap to look for
	dwZCmpCap = D3DPCMPCAPS_NEVER;
}

CZNeverTest::~CZNeverTest()
{
}

bool CZNeverTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using the Z Buffer and compare func Never
	SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_TRUE);
	SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_NEVER);
	return true;
}

//************************************************************************
// Always Test functions

CZAlwaysTest::CZAlwaysTest()
{
	m_szTestName = TEXT("Z Compare Always");
	m_szCommandKey = TEXT("Always");

	// Tell parent class what cap to look for
	dwZCmpCap = D3DPCMPCAPS_ALWAYS;
}

CZAlwaysTest::~CZAlwaysTest()
{
}

bool CZAlwaysTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using the Z Buffer and compare func Always
	SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_TRUE);
	SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD)D3DCMP_ALWAYS);
	return true;
}

//************************************************************************
// Internal API:    AlwaysTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CZCmpTest::AlwaysTests(int nTest)
{
	switch (nTest)
	{
		case (1):	// Left Handed Flat Triangles
			if (dwZCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sFour flat triangles (left), Visible: None",msgString);
			else
				sprintf(msgString, "%sFour flat triangles (left), Visible: All",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f, 10.0f + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f, 10.0f + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f,110.0f + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  70.0f, 70.0f + (float)m_vpTest.Y,0.5f),(1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 170.0f, 70.0f + (float)m_vpTest.Y,0.5f),(1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 170.0f,170.0f + (float)m_vpTest.Y,0.5f),(1.0f / 0.5f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 130.0f,130.0f + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f,130.0f + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f,230.0f + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			// Yellow Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  90.0f, 45.0f + (float)m_vpTest.Y,0.5f),(1.0f / 0.5f), RGBA_MAKE(255,255,0,255),0, 0.0f, 0.25f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f, 10.0f + (float)m_vpTest.Y,0.5f),(1.0f / 0.5f), RGBA_MAKE(255,255,0,255),0, 1.0f, 0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 195.0f,150.0f + (float)m_vpTest.Y,0.5f),(1.0f / 0.5f), RGBA_MAKE(255,255,0,255),0, 0.75f,1.0f);
			break;
		case (2):	// Right Handed Flat Triangles
			if (dwZCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sFour flat triangles (right), Visible: None",msgString);
			else
				sprintf(msgString, "%sFour flat triangles (right), Visible: All",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f, 10.0f + (float)m_vpTest.Y,0.35f),(1.0f / 0.35f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f,110.0f + (float)m_vpTest.Y,0.35f),(1.0f / 0.35f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f,110.0f + (float)m_vpTest.Y,0.35f),(1.0f / 0.35f), RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  70.0f, 70.0f + (float)m_vpTest.Y,0.4f),(1.0f / 0.4f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 170.0f,170.0f + (float)m_vpTest.Y,0.4f),(1.0f / 0.4f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  70.0f,170.0f + (float)m_vpTest.Y,0.4f),(1.0f / 0.4f), RGBA_MAKE(0,255,0,255),0, 0.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 130.0f,130.0f + (float)m_vpTest.Y,0.3f),(1.0f / 0.3f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f,230.0f + (float)m_vpTest.Y,0.3f),(1.0f / 0.3f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 130.0f,230.0f + (float)m_vpTest.Y,0.3f),(1.0f / 0.3f), RGBA_MAKE(0,0,255,255),0, 0.0f,1.0f);
			// Yellow Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  45.0f, 90.0f + (float)m_vpTest.Y,0.5f),(1.0f / 0.5f), RGBA_MAKE(255,255,0,255),0, 0.25f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 150.0f,195.0f + (float)m_vpTest.Y,0.5f),(1.0f / 0.5f), RGBA_MAKE(255,255,0,255),0, 1.0f, 0.75f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f,230.0f + (float)m_vpTest.Y,0.5f),(1.0f / 0.5f), RGBA_MAKE(255,255,0,255),0, 0.0f, 1.0f);
			break;
		case (3):	// Single Flat Triangles
			if (dwZCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sFour flat triangles, Visible: None",msgString);
			else
				sprintf(msgString, "%sFour flat triangles, Visible: All",msgString);

			// Magenta Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 150.0f, 10.0f + (float)m_vpTest.Y,0.0001f),(1.0f / 0.0001f), RGBA_MAKE(255,0,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 300.0f,270.0f + (float)m_vpTest.Y,0.0001f),(1.0f / 0.0001f), RGBA_MAKE(255,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   0.0f,270.0f + (float)m_vpTest.Y,0.0001f),(1.0f / 0.0001f), RGBA_MAKE(255,0,255,255),0, 0.0f,1.0f);
			// Teal Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 150.0f, 20.0f + (float)m_vpTest.Y,0.0f),(1.0f / 0.00001f), RGBA_MAKE(0,255,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 280.0f,260.0f + (float)m_vpTest.Y,0.0f),(1.0f / 0.00001f), RGBA_MAKE(0,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  20.0f,260.0f + (float)m_vpTest.Y,0.0f),(1.0f / 0.00001f), RGBA_MAKE(0,255,255,255),0, 0.0f,1.0f);
			// White Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 150.0f, 30.0f + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 260.0f,250.0f + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  40.0f,250.0f + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), RGBA_MAKE(255,255,255,255),0, 0.0f,1.0f);
			// Red Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 150.0f, 40.0f + (float)m_vpTest.Y,0.9999f),(1.0f / 0.9999f), RGBA_MAKE(255,0,0,255),0, 0.5f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 240.0f,240.0f + (float)m_vpTest.Y,0.9999f),(1.0f / 0.9999f), RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  60.0f,240.0f + (float)m_vpTest.Y,0.9999f),(1.0f / 0.9999f), RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
			break;
		case (4):	// Left Handed Non-Flat Triangles
			if (dwZCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sFour non-flat triangles (left), Visible: None",msgString);
			else
				sprintf(msgString, "%sFour non-flat triangles (left), Visible: All",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f, 10.0f + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f),     RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f, 10.0f + (float)m_vpTest.Y,0.0f),(1.0f / 0.00001f), RGBA_MAKE(255,0,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f,110.0f + (float)m_vpTest.Y,0.5f),(1.0f / 0.5f),     RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  70.0f, 70.0f + (float)m_vpTest.Y,0.0f),    (1.0f / 0.00001f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 170.0f, 70.0f + (float)m_vpTest.Y,0.00001f),(1.0f / 0.00001f), RGBA_MAKE(0,255,0,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 170.0f,170.0f + (float)m_vpTest.Y,0.9f),    (1.0f / 0.9f),     RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 130.0f,130.0f + (float)m_vpTest.Y,0.1f),(1.0f / 0.1f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f,130.0f + (float)m_vpTest.Y,0.2f),(1.0f / 0.2f), RGBA_MAKE(0,0,255,255),0, 1.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f,230.0f + (float)m_vpTest.Y,0.3f),(1.0f / 0.3f), RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			// Yellow Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  90.0f, 45.0f + (float)m_vpTest.Y,0.25f), (1.0f / 0.25f),  RGBA_MAKE(255,255,0,255),0, 0.0f, 0.25f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f, 10.0f + (float)m_vpTest.Y,0.55f), (1.0f / 0.55f),  RGBA_MAKE(255,255,0,255),0, 1.0f, 0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 195.0f,150.0f + (float)m_vpTest.Y,0.005f),(1.0f / 0.005f), RGBA_MAKE(255,255,0,255),0, 0.75f,1.0f);
			break;
		case (5):	// Right Handed Non-Flat Triangles
			if (dwZCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sFour non-flat triangles (right), Visible: None",msgString);
			else
				sprintf(msgString, "%sFour non-flat triangles (right), Visible: All",msgString);

			// Red Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f, 10.0f + (float)m_vpTest.Y,0.00805f),(1.0f / 0.00805f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 110.0f,110.0f + (float)m_vpTest.Y,0.309f),  (1.0f / 0.309f),   RGBA_MAKE(255,0,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f,110.0f + (float)m_vpTest.Y,0.351f),  (1.0f / 0.351f),   RGBA_MAKE(255,0,0,255),0, 0.0f,1.0f);
			// Green Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  70.0f, 70.0f + (float)m_vpTest.Y,0.123f), (1.0f / 0.123f),  RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 170.0f,170.0f + (float)m_vpTest.Y,0.0987f),(1.0f / 0.0987f), RGBA_MAKE(0,255,0,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  70.0f,170.0f + (float)m_vpTest.Y,0.4444f),(1.0f / 0.4444f), RGBA_MAKE(0,255,0,255),0, 0.0f,1.0f);
			// Blue Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 130.0f,130.0f + (float)m_vpTest.Y,0.099f),(1.0f / 0.099f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 230.0f,230.0f + (float)m_vpTest.Y,0.75f), (1.0f / 0.75f),  RGBA_MAKE(0,0,255,255),0, 1.0f,1.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 130.0f,230.0f + (float)m_vpTest.Y,0.4f),  (1.0f / 0.4f),   RGBA_MAKE(0,0,255,255),0, 0.0f,1.0f);
			// Yellow Triangle
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  45.0f, 90.0f + (float)m_vpTest.Y,0.5001f),(1.0f / 0.5001f), RGBA_MAKE(255,255,0,255),0, 0.25f,0.0f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 150.0f,195.0f + (float)m_vpTest.Y,0.007f), (1.0f / 0.007f),  RGBA_MAKE(255,255,0,255),0, 1.0f, 0.75f);
			VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +  10.0f,230.0f + (float)m_vpTest.Y,0.2f),   (1.0f / 0.2f),    RGBA_MAKE(255,255,0,255),0, 0.0f, 1.0f);
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
			if (dwZCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sTwo triangles (%d%% overlap), Visible: None",msgString,10 * (nTest - 6));
			else
				sprintf(msgString, "%sTwo triangles (%d%% overlap), Visible: 100%% Green",msgString,10 * (nTest - 6));

			ComboTests(0.1f * (nTest - 6));
			break;
		case (16):	// Combo Test: 0.9999999f
			if (dwZCmpCap == D3DPCMPCAPS_NEVER)
				sprintf(msgString, "%sTwo triangles (100%% overlap), Visible: None",msgString);
			else
				sprintf(msgString, "%sTwo triangles (100%% overlap), Visible: 100%% Green",msgString);

			ComboTests(0.9999999f);
			break;
		default:
			break;
	}
}
