//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "BasicRst.h"

#define FVF_TLT1VERTEX ( D3DFVF_XYZRHW | D3DFVF_TEX1 )

//************************************************************************
// NoDiffuse Test functions

CNoDiffuseTest::CNoDiffuseTest()
{
	m_szTestName = TEXT("BasicRst NoDiffuse");
	m_szCommandKey = TEXT("NoDiffuse");
}

CNoDiffuseTest::~CNoDiffuseTest()
{
}

UINT CNoDiffuseTest::TestInitialize(void)
{
	// Setup the Test range
	SetTestRange(1, m_dwRPFullAPIListSize);

    // Tell RenderPrimitive to cycle through all APIs
    SetAPI(m_pRPFullAPIList, m_dwRPFullAPIListSize, 1);

	return D3DTESTINIT_RUN;
}

bool CNoDiffuseTest::ExecuteTest(UINT uTestNum)
{
    DWORD dwAPI;

	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = '\0';

	// Let's build some triangles.
	DrawNonDiffuseGrid();

	// Get the current API
    dwAPI = RemapAPI(GetAPI(), D3DPT_TRIANGLELIST, FVF_TLT1VERTEX);

    // Tell log which API we are using
    switch (dwAPI)
    {
        case RP_BE:             sprintf(msgString, "%sFVF: 0x%08x, API: Begin/Vertex/End", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_BIE:            sprintf(msgString, "%sFVF: 0x%08x, API: BeginIndexed/Index/End", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DP:             sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitive", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DIP:            sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitive", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DPS:            sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveStrided", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DIPS:           sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveStrided", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DPVB:           sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DIPVB:          sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DPVB_PV:        sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB after ProcessVertices", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DIPVB_PV:       sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB after ProcessVertices", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DPVB_OPT:       sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB with Optimized VB", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DIPVB_OPT:      sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB with Optimized VB", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DPVB_OPT_PV:    sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB after ProcessVert with Optimized VB", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DIPVB_OPT_PV:   sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB after ProcessVert with Optimized VB", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DPVB_PVS:       sprintf(msgString, "%sFVF: 0x%08x, API: DrawPrimitiveVB after ProcessVerticesStrided", msgString, FVF_TLT1VERTEX);
                                break;
        case RP_DIPVB_PVS:      sprintf(msgString, "%sFVF: 0x%08x, API: DrawIndexedPrimitiveVB after ProcessVerticesStrided", msgString, FVF_TLT1VERTEX);
                                break;
    }

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CNoDiffuseTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,FVF_TLT1VERTEX,NoDiffuseList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

//************************************************************************
// Internal API:    DrawNonDiffuseGrid
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CNoDiffuseTest::DrawNonDiffuseGrid(void)
{
	// Use standard ((0,0)(1,0)(0,1)(1,1)) mapping
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(cD3DVECTOR(  9.5f,   9.5f, 0.9f), (1.0f / 0.9f), 0.0f,0.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(cD3DVECTOR(265.5f, 265.5f, 0.9f), (1.0f / 0.9f), 1.0f,1.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(cD3DVECTOR(  9.5f, 265.5f, 0.9f), (1.0f / 0.9f), 0.0f,1.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(cD3DVECTOR(  9.5f,   9.5f, 0.9f), (1.0f / 0.9f), 0.0f,0.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(cD3DVECTOR(265.5f,   9.5f, 0.9f), (1.0f / 0.9f), 1.0f,0.0f);
	NoDiffuseList[dwVertexCount++] = ND_VERTEX(cD3DVECTOR(265.5f, 265.5f, 0.9f), (1.0f / 0.9f), 1.0f,1.0f);
}

