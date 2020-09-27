//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "BasicRst.h"

//************************************************************************
// Dither Test functions

CDitherTest::CDitherTest()
{
	m_szTestName = TEXT("BasicRst Dither");
	m_szCommandKey = TEXT("Dither");
}

CDitherTest::~CDitherTest()
{
}

UINT CDitherTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Setup the Test range
	SetTestRange(1,DITHER_TESTS);

	// Check the caps we are interested in
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;

	// Check the Dither cap
	if (!(dwRasterCaps & D3DPRASTERCAPS_DITHER))
	{
		WriteToLog("Device capability not supported: Raster Dither.\n");
		return D3DTESTINIT_ABORT;
	}

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return D3DTESTINIT_RUN;
}

bool CDitherTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = 0;
	sprintf(msgString, "");

	// Let's build some triangles.
	DrawDitherTests(uTestNum % 4);

	// Set the appropriate renderstate
	if (uTestNum <= 4)
	{
		sprintf(msgString, "%sTrue, ", msgString);
		SetRenderState(D3DRENDERSTATE_DITHERENABLE, (DWORD)TRUE);
	}
	else
	{
		sprintf(msgString, "%sFalse, ", msgString);
		SetRenderState(D3DRENDERSTATE_DITHERENABLE, (DWORD)FALSE);
	}

	switch (uTestNum % 4)
	{
		case (0):
			sprintf(msgString, "%sBlue Mesh", msgString);
			break;
		case (1):
			sprintf(msgString, "%sWhite Mesh", msgString);
			break;
		case (2):
			sprintf(msgString, "%sRed Mesh", msgString);
			break;
		case (3):
			sprintf(msgString, "%sGreen Mesh", msgString);
			break;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

//************************************************************************
// Internal API:    DrawDitherTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CDitherTest::DrawDitherTests(int nTest)
{
	// Color of triangles
	D3DCOLOR Color[] = { RGBA_MAKE(0,0,255,255),		// Blue
						 RGBA_MAKE(255,255,255,255),	// White
						 RGBA_MAKE(255,0,0,255),		// Red
						 RGBA_MAKE(0,255,0,255) };		// Green

	// Mesh with Black bottom right corner
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   0.0f,   0.0f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), Color[nTest],         0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 256.0f, 256.0f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,255), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   0.0f, 256.0f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), Color[nTest],         0, 0.0f,1.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X +   0.0f,   0.0f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), Color[nTest],         0, 0.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 256.0f,   0.0f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), Color[nTest],         0, 1.0f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + 256.0f, 256.0f + (float)m_vpTest.Y, 0.9f),(1.0f / 0.9f), RGBA_MAKE(0,0,0,255), 0, 1.0f,1.0f);
}

