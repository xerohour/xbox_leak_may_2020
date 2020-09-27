//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "BasicRst.h"

//************************************************************************
// Culling Test functions

CCullingTest::CCullingTest()
{
	m_szTestName = TEXT("BasicRst Culling");
	m_szCommandKey = TEXT("Culling");
}

CCullingTest::~CCullingTest()
{
}

UINT CCullingTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Setup the Test range
	SetTestRange(1,CULLING_TESTS);

	// Check the caps we are interested in
	DWORD dwMiscCaps = m_d3dcaps.PrimitiveMiscCaps;

	// Check the appropriate Culling caps
	if (!(dwMiscCaps & D3DPMISCCAPS_CULLNONE))
	{
		WriteToLog("Device capability not supported: Cull None.\n");
		return D3DTESTINIT_ABORT;
	}

	if (!(dwMiscCaps & D3DPMISCCAPS_CULLCW))
	{
		WriteToLog("Device capability not supported: Cull CW.\n");
		return D3DTESTINIT_ABORT;
	}

	if (!(dwMiscCaps & D3DPMISCCAPS_CULLCCW))
	{
		WriteToLog("Device capability not supported: Cull CCW.\n");
		return D3DTESTINIT_ABORT;
	}

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return D3DTESTINIT_RUN;
}

bool CCullingTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = 0;
	sprintf(msgString, "");

	// Let's build some triangles.
	DrawCullTests();

	// Set the appropriate renderstate
	switch (uTestNum)
	{
		case (1):
			sprintf(msgString, "%sNone, Visible: 24 triangles (All)", msgString);
			SetRenderState(D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_NONE);
			break;
		case (2):
			sprintf(msgString, "%sClockwise, Visible: 12 triangles (Right half)", msgString);
			SetRenderState(D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_CW);
			break;
		case (3):
			sprintf(msgString, "%sCounterClockwise, Visible: 12 triangles (Left half)", msgString);
			SetRenderState(D3DRENDERSTATE_CULLMODE, (DWORD)D3DCULL_CCW);
			break;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}


//************************************************************************
// Internal API:    DrawCullTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CCullingTest::DrawCullTests(void)
{
	int nRow, delta;

	// Vertices for culling triangles
	float x1 = 5.0f;
	float x2[] = {5.0f, 5.0f, 25.0f, 25.0f};
	float x3 = 45.0f;
	float y1[] = {45.0f, 105.0f, 165.0f, 205.0f};
	float y2[] = { 5.0f,  65.0f, 125.0f, 185.0f};
	float y3[] = {45.0f,  85.0f, 165.0f, 225.0f};

	// Color of triangles
	D3DCOLOR Color[] = { RGBA_MAKE(255,0,0,255),		// Red
						 RGBA_MAKE(0,255,0,255),		// Green
						 RGBA_MAKE(0,0,255,255),		// Blue
						 RGBA_MAKE(255,255,0,255) };	// Yellow


	// Build the 24 triangles for the culling tests
	for (nRow=0; nRow<4; nRow++)
	{
		// Column 0
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x1,      y1[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x2[nRow],y2[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 1.0f,1.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x3,      y3[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,1.0f);
	}

	delta = 50;
	for (nRow=0; nRow<4; nRow++)
	{
		// Column 1
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x2[nRow]+delta,y2[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x3+delta,      y3[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 1.0f,1.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x1+delta,      y1[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,1.0f);
	}

	delta = 2*50;
	for (nRow=0; nRow<4; nRow++)
	{
		// Column 2
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x3+delta,      y3[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x1+delta,      y1[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 1.0f,1.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x2[nRow]+delta,y2[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,1.0f);
	}

	delta = 3*50;
	for (nRow=0; nRow<4; nRow++)
	{
		// Column 3
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x3+delta,      y3[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x2[nRow]+delta,y2[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 1.0f,1.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x1+delta,      y1[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,1.0f);
	}

	delta = 4*50;
	for (nRow=0; nRow<4; nRow++)
	{
		// Column 4
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x2[nRow]+delta,y2[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x1+delta,      y1[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 1.0f,1.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x3+delta,      y3[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,1.0f);
	}

	delta = 5*50;
	for (nRow=0; nRow<4; nRow++)
	{
		// Column 5
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x1+delta,      y1[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,0.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x3+delta,      y3[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 1.0f,1.0f);
		VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR((float)m_vpTest.X + x2[nRow]+delta,y2[nRow] + (float)m_vpTest.Y,0.9f),(1.0f / 0.9f), Color[nRow], 0, 0.0f,1.0f);
	}
}
