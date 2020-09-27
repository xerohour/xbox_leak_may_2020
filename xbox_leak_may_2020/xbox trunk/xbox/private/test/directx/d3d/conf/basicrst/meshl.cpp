//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "BasicRst.h"

//#define FVF_LVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 )

//************************************************************************
// MeshL Test functions

CMeshLTest::CMeshLTest()
{
	m_szTestName = TEXT("BasicRst Mesh L Vertex");
	m_szCommandKey = TEXT("MeshL");
}

CMeshLTest::~CMeshLTest()
{
}

bool CMeshLTest::SetDefaultRenderStates(void)
{
	// Turn off Lighting for DX7
	if (m_dwVersion >= 0x0700)
		SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)FALSE);

	return true;
}

bool CMeshLTest::SetDefaultMatrices(void)
{
    D3DMATRIX proj, view;

	// Set the projection matrix.
	InitMatrix(&proj,
                	D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
					D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
					D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(1.0),
					D3DVAL(0.0), D3DVAL(0.0), D3DVAL(-1.0), D3DVAL(0.0)
				  );

	// the projection looks good when set to window height and width
	proj._11 = D3DVAL(pi/1.5f);
	proj._22 = D3DVAL(pi/1.5f);

    if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&proj))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

	InitMatrix(&view,
	                D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
					D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
					D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
					D3DVAL(0.0), D3DVAL(0.0), D3DVAL(2.0), D3DVAL(1.0)
				  );

    if (!SetTransform(D3DTRANSFORMSTATE_VIEW,&view))
	{
		WriteToLog("SetTransform(ViewMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

	D3DMATRIX identity = IdentityMatrix();

    if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&identity))
	{
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CMeshLTest::TestInitialize(void)
{
	// Setup the Test range
	SetTestRange(1,MESH_L_TESTS);

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return D3DTESTINIT_RUN;
}

bool CMeshLTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = 0;
	sprintf(msgString, "");

	// Let's build some triangles.
	DrawMeshTests(uTestNum);

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CMeshLTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		if (m_uLastTestNum < 16)
		{
			// Use Triangle Strips
			RenderPrimitive(D3DPT_TRIANGLESTRIP,FVF_LVERTEX,LVertexList,dwVertexCount,NULL,0,0);
		}
		else if (m_uLastTestNum < 18)
		{
			// Use Triangle Strips
			RenderPrimitive(D3DPT_TRIANGLESTRIP,FVF_LVERTEX,LVertexList,dwVertexCount/2,NULL,0,0);
			RenderPrimitive(D3DPT_TRIANGLESTRIP,FVF_LVERTEX,LVertexList+8,dwVertexCount/2,NULL,0,0);
		}
		else
		{
			// Use Triangle Fans
			RenderPrimitive(D3DPT_TRIANGLEFAN,FVF_LVERTEX,LVertexList,dwVertexCount,NULL,0,0);
		}

		EndScene();
	}	
}

//************************************************************************
// Internal API:    DrawMeshTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CMeshLTest::DrawMeshTests(int nTest)
{
	switch (nTest)
	{
		// Straight Triangle Strip (Horizontal), D3DPT_TRIANGLESTRIP
		//
		case (1):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Red", msgString);

			// Red Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.80f,-0.6f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.80f, 0.4f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.48f,-0.6f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.16f, 0.4f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.16f,-0.6f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.48f, 0.4f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.80f,-0.6f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.80f, 0.4f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			break;
		case (2):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Green", msgString);

			// Green Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.80f,-0.6f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.80f, 0.4f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.48f,-0.6f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.16f, 0.4f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.16f,-0.6f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.48f, 0.4f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.80f,-0.6f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.80f, 0.4f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			break;
		case (3):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Blue", msgString);

			// Blue Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.80f,-0.6f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.80f, 0.4f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.48f,-0.6f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.16f, 0.4f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.16f,-0.6f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.48f, 0.4f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.80f,-0.6f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.80f, 0.4f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			break;
		case (4):	
			sprintf(msgString, "%sHorizontal Triangle Strip: White", msgString);

			// White Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.80f,-0.6f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.80f, 0.4f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.48f,-0.6f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.16f, 0.4f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.16f,-0.6f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.48f, 0.4f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.80f,-0.6f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.80f, 0.4f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			break;
		case (5):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Multicolor", msgString);

			// Multicolor Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.80f,-0.6f, 0.5f), RGBA_MAKE(255,0,0,255),  0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.80f, 0.4f, 0.5f), RGBA_MAKE(255,128,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.48f,-0.6f, 0.5f), RGBA_MAKE(128,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.16f, 0.4f, 0.5f), RGBA_MAKE(0,255,0,255),  0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.16f,-0.6f, 0.5f), RGBA_MAKE(0,255,0,255),  0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.48f, 0.4f, 0.5f), RGBA_MAKE(0,255,128,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.80f,-0.6f, 0.5f), RGBA_MAKE(0,128,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.80f, 0.4f, 0.5f), RGBA_MAKE(0,0,255,255),  0, 0.0f,0.0f);
			break;
		//
		// Straight Triangle Strip (Vertical), D3DPT_TRIANGLESTRIP
		//
		case (6):	
			sprintf(msgString, "%sVertical Triangle Strip: Red", msgString);

			// Red Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.80f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.80f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.16f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.48f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.48f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.16f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.80f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.80f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			break;
		case (7):	
			sprintf(msgString, "%sVertical Triangle Strip: Green", msgString);

			// Green Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.80f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.80f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.16f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.48f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.48f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.16f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.80f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.80f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			break;
		case (8):	
			sprintf(msgString, "%sVertical Triangle Strip: Blue", msgString);

			// Blue Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.80f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.80f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.16f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.48f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.48f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.16f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.80f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.80f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			break;
		case (9):	
			sprintf(msgString, "%sVertical Triangle Strip: White", msgString);

			// White Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.80f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.80f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.16f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.48f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.48f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.16f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.80f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.80f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			break;
		case (10):	
			sprintf(msgString, "%sVertical Triangle Strip: Multicolor", msgString);

			// Multicolor Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.80f, 0.5f), RGBA_MAKE(255,0,0,255),  0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.80f, 0.5f), RGBA_MAKE(255,128,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.16f, 0.5f), RGBA_MAKE(128,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.48f, 0.5f), RGBA_MAKE(0,255,0,255),  0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.48f, 0.5f), RGBA_MAKE(0,255,0,255),  0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.16f, 0.5f), RGBA_MAKE(0,255,128,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.80f, 0.5f), RGBA_MAKE(0,128,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.80f, 0.5f), RGBA_MAKE(0,0,255,255),  0, 0.0f,0.0f);
			break;
		//
		// Bent Triangle Strip, D3DPT_TRIANGLESTRIP
		//
		case (11):	
			sprintf(msgString, "%sBent Triangle Strip: Red", msgString);

			// Red Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.6f,-0.6f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.6f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.2f,-0.4f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.4f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.6f,-0.2f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2f,-0.2f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.0f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.0f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2f, 0.2f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.2f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.0f, 0.6f, 0.5f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			break;
		case (12):	
			sprintf(msgString, "%sBent Triangle Strip: Green", msgString);

			// Green Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.6f,-0.6f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.6f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.2f,-0.4f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.4f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.6f,-0.2f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2f,-0.2f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.0f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.0f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2f, 0.2f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.2f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.0f, 0.6f, 0.5f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			break;
		case (13):	
			sprintf(msgString, "%sBent Triangle Strip: Blue", msgString);

			// Blue Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.6f,-0.6f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.6f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.2f,-0.4f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.4f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.6f,-0.2f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2f,-0.2f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.0f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.0f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2f, 0.2f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.2f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.0f, 0.6f, 0.5f), RGBA_MAKE(0,0,255,255),0, 0.0f,0.0f);
			break;
		case (14):	
			sprintf(msgString, "%sBent Triangle Strip: White", msgString);

			// White Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.6f,-0.6f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.6f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.2f,-0.4f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.4f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.6f,-0.2f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2f,-0.2f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.0f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.0f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2f, 0.2f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.2f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.0f, 0.6f, 0.5f), RGBA_MAKE(255,255,255,255),0, 0.0f,0.0f);
			break;
		case (15):	
			sprintf(msgString, "%sBent Triangle Strip: Multicolor", msgString);

			// Multicolor Mesh
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.6f,-0.6f, 0.5f), RGBA_MAKE(128,0,0,255),  0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.6f, 0.5f), RGBA_MAKE(255,0,0,255),  0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.2f,-0.4f, 0.5f), RGBA_MAKE(255,128,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f,-0.4f, 0.5f), RGBA_MAKE(128,128,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.6f,-0.2f, 0.5f), RGBA_MAKE(128,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2f,-0.2f, 0.5f), RGBA_MAKE(0,255,0,255),  0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f, 0.0f, 0.5f), RGBA_MAKE(0,255,128,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.0f, 0.5f), RGBA_MAKE(0,128,128,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2f, 0.2f, 0.5f), RGBA_MAKE(0,128,255,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.2f, 0.5f), RGBA_MAKE(0,0,255,255),  0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.0f, 0.6f, 0.5f), RGBA_MAKE(0,0,128,255),  0, 0.0f,0.0f);
			break;
		//
		// Two crossed strips, D3DPT_TRIANGLESTRIP
		//
		case (16):	
			sprintf(msgString, "%sCrossed Triangle Strips: Red & Green", msgString);

			// Red strip
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.8000f,-0.8f, 0.9000f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.8000f,-0.4f, 0.7857f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2666f,-0.4f, 0.6714f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2666f, 0.0f, 0.5571f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.2666f, 0.0f, 0.4429f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.2666f, 0.4f, 0.3286f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.8000f, 0.4f, 0.2143f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.8000f, 0.8f, 0.1000f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			// Green strip
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.8f,-0.8000f, 0.1000f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.8000f, 0.2143f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.2666f, 0.3286f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.0f,-0.2666f, 0.4429f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.0f, 0.2666f, 0.5571f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.4f, 0.2666f, 0.6714f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.4f, 0.8000f, 0.7857f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.8f, 0.8000f, 0.8000f), RGBA_MAKE(0,255,0,255),0, 0.0f,0.0f);
			break;
		case (17):	
			sprintf(msgString, "%sCrossed Triangle Strips: Red & Black", msgString);

			// Red strip
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.8000f,-0.8f, 0.9000f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.8000f,-0.4f, 0.7857f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2666f,-0.4f, 0.6714f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.2666f, 0.0f, 0.5571f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.2666f, 0.0f, 0.4429f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.2666f, 0.4f, 0.3286f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.8000f, 0.4f, 0.2143f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.8000f, 0.8f, 0.1000f), RGBA_MAKE(255,0,0,255),0, 0.0f,0.0f);
			// Black strip
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.8f,-0.8000f, 0.1000f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.8000f, 0.2143f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.4f,-0.2666f, 0.3286f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.0f,-0.2666f, 0.4429f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR( 0.0f, 0.2666f, 0.5571f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.4f, 0.2666f, 0.6714f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.4f, 0.8000f, 0.7857f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.8f, 0.8000f, 0.8000f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
			break;
		//
		// Triangle fan Square, D3DPT_TRIANGLEFAN
		//
		case (18):	
			sprintf(msgString, "%sSquare Triangle Fan: Red & Blue", msgString);

			{
				int i, red, green, blue;

				LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.6f, 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);

				for (i = -8; i < 9; i++)
				{
					LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR((float)i / 10.0f, 0.8f, 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}
				for (i = 8; i > -9; i--)
				{
					LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(0.8f, (float)i / 10.0f, 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}
				for (i = 8; i > -9; i--)
				{
					LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR((float)i / 10.0f,-0.8f, 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}
				for (i = -8; i < 9; i++)
				{
					LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.8f, (float)i / 10.0f, 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}

				for (i = 1; i < 69; i++)
				{
					red = (255 - (255 * i / 68));
					green = 0;
					blue = (255 * i / 68);
					LVertexList[i].color = RGBA_MAKE(red, green, blue, 255);
				}
			}
			break;
		//
		// Jagged Triangle fan Square, D3DPT_TRIANGLEFAN
		//
		case (19):	
			sprintf(msgString, "%sJagged Triangle Fan: Red & Blue", msgString);

			{
				int i, red, green, blue;
				bool bEven;

				LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR(-0.6f, 0.6f, 0.5f), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);

				bEven = true;
				for (i = -8; i < 9; i++)
				{
					bEven = (bEven ? false : true);
					LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR((float)i / 10.0f, (bEven ? 0.7f : 0.8f), (bEven ? 0.6f : 0.4f)), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}
				for (i = 8; i > -9; i--)
				{
					bEven = (bEven ? false : true);
					LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR((bEven ? 0.3f : 0.8f), (float)i / 10.0f, (bEven ? 0.6f : 0.4f)), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}
				for (i = 8; i > -9; i--)
				{
					bEven = (bEven ? false : true);
					LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR((float)i / 10.0f, (bEven ? -0.3f : -0.8f), (bEven ? 0.6f : 0.4f)), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}
				for (i = -8; i < 9; i++)
				{
					bEven = (bEven ? false : true);
					LVertexList[dwVertexCount++] = LLVERTEX(cD3DVECTOR((bEven ? -0.7f : -0.8f), (float)i / 10.0f, (bEven ? 0.6f : 0.4f)), RGBA_MAKE(0,0,0,255),0, 0.0f,0.0f);
				}

				for (i = 1; i < 69; i++)
				{
					red = (255 - (255 * i / 68));
					green = 0;
					blue = (255 * i / 68);
					LVertexList[i].color = RGBA_MAKE(red, green, blue, 255);
				}
			}
			break;
	}
}

