//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "BasicRst.h"

//************************************************************************
// ZWrite Test functions

CZWriteTest::CZWriteTest()
{
	m_szTestName = TEXT("BasicRst ZWrite");
	m_szCommandKey = TEXT("ZWrite");
}

CZWriteTest::~CZWriteTest()
{
}

UINT CZWriteTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Setup the Test range
	SetTestRange(1,ZWRITE_TESTS);

	// Only need to check cap for Hardware devices
//	if (m_pAdapter->Devices[n].fHardware)
//	{
		// Check the caps we are interested in
		DWORD dwMiscCaps = m_d3dcaps.PrimitiveMiscCaps;

		// Check the Z mask cap
		if (!(dwMiscCaps & D3DPMISCCAPS_MASKZ))
		{
			WriteToLog("Device capability not supported: Misc MaskZ.\n");
			return D3DTESTINIT_ABORT;
		}
//	}

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return D3DTESTINIT_RUN;
}

bool CZWriteTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = 0;
	sprintf(msgString, "");

	// Let's build some triangles.
	DrawZWriteTests();

	// Set the appropriate renderstate
	switch (uTestNum)
	{
		case (1):
			sprintf(msgString, "%sZEnable: True, ZWriteEnable: True, Visible: Green on Blue", msgString);
			SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_TRUE);
			SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, (DWORD)TRUE);
			break;
		case (2):
			sprintf(msgString, "%sZEnable: True, ZWriteEnable: False, Visible: Blue on Green", msgString);
			SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_TRUE);
			SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, (DWORD)FALSE);
			break;
		case (3):
			sprintf(msgString, "%sZEnable: False, ZWriteEnable: True, Visible: Blue on Green", msgString);
			SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_FALSE);
			SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, (DWORD)TRUE);
			break;
		case (4):
			sprintf(msgString, "%sZEnable: False, ZWriteEnable: False, Visible: Blue on Green", msgString);
			SetRenderState(D3DRENDERSTATE_ZENABLE, (DWORD)D3DZB_FALSE);
			SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, (DWORD)FALSE);
			break;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

//************************************************************************
// Internal API:    DrawZWriteTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CZWriteTest::DrawZWriteTests(void)
{
	// Green Triangle
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(115.0f + (float)m_vpTest.X, 40.0f + (float)m_vpTest.Y,0.5f),(1.0f / 0.5f), RGBA_MAKE(0,255,0,255), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(205.0f + (float)m_vpTest.X,225.0f + (float)m_vpTest.Y,0.5f),(1.0f / 0.5f), RGBA_MAKE(0,255,0,255), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR( 25.0f + (float)m_vpTest.X,225.0f + (float)m_vpTest.Y,0.5f),(1.0f / 0.5f), RGBA_MAKE(0,255,0,255), 0, 0.0f,1.0f);

	// Blue Triangle
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(130.0f + (float)m_vpTest.X, 40.0f + (float)m_vpTest.Y,0.7f),(1.0f / 0.7f), RGBA_MAKE(0,0,255,255), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR(225.0f + (float)m_vpTest.X,240.0f + (float)m_vpTest.Y,0.7f),(1.0f / 0.7f), RGBA_MAKE(0,0,255,255), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(cD3DVECTOR( 40.0f + (float)m_vpTest.X,240.0f + (float)m_vpTest.Y,0.7f),(1.0f / 0.7f), RGBA_MAKE(0,0,255,255), 0, 0.0f,1.0f);
}
