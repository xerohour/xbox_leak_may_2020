//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "BasicRst.h"

//************************************************************************
// MeshD3D Test functions

CMeshD3DTest::CMeshD3DTest()
{
	m_szTestName = TEXT("BasicRst Mesh D3D Vertex");
	m_szCommandKey = TEXT("MeshD3D");
}

CMeshD3DTest::~CMeshD3DTest()
{
}

bool CMeshD3DTest::SetDefaultMatrices(void)
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

UINT CMeshD3DTest::TestInitialize(void)
{
	// Setup the Test range
	SetTestRange(1,MESH_D3D_TESTS);

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return D3DTESTINIT_RUN;
}

bool CMeshD3DTest::ExecuteTest(UINT uTestNum)
{
	D3DMATERIAL8 Material;
	D3DCOLOR	Color;

	// Initialize some variables
 	dwVertexCount = 0;
	msgString[0] = 0;
	sprintf(msgString, "");

	// Let's build some triangles.
	Color = DrawMeshTests(uTestNum);

	// set up the material for the lightstate.
	memset(&Material, NULL, sizeof(D3DMATERIAL8));
//	Material.dwSize = sizeof(D3DMATERIAL);
	Material.Emissive.r = (float)RGBA_GETRED(Color);
	Material.Emissive.g = (float)RGBA_GETGREEN(Color);
	Material.Emissive.b = (float)RGBA_GETBLUE(Color);
	Material.Emissive.a = (float)RGBA_GETALPHA(Color);

	// Set the appropriate material
//    RELEASE(pMaterial);
//	pMaterial = CreateMaterial();
//	pMaterial->SetMaterial(&Material);
//	SetMaterial(pMaterial);
    m_pDevice->SetMaterial(&Material);

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CMeshD3DTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		if (m_uLastTestNum < 13)
		{
			// Use Triangle Strips
			RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,D3DVertexList,dwVertexCount,NULL,0,0);
		}
		else if (m_uLastTestNum < 15)
		{
			D3DMATERIAL8 Material;

			// Use Triangle Strips
			RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,D3DVertexList+8,dwVertexCount/2,NULL,0,0);

			// Setup a red material
			memset(&Material, NULL, sizeof(D3DMATERIAL8));
			Material.Emissive.r = 255.0f;
			Material.Emissive.g =   0.0f;
			Material.Emissive.b =   0.0f;
			Material.Emissive.a = 255.0f;

			// Set the appropriate material
//		    RELEASE(pMaterial);
//			pMaterial = CreateMaterial();
//			pMaterial->SetMaterial(&Material);
//			SetMaterial(pMaterial);
            m_pDevice->SetMaterial(&Material);

			// Use Triangle Strips
			RenderPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,D3DVertexList,dwVertexCount/2,NULL,0,0);
		}
		else
		{
			// Use Triangle Fans
			RenderPrimitive(D3DPT_TRIANGLEFAN,D3DFVF_VERTEX,D3DVertexList,dwVertexCount,NULL,0,0);
		}

		EndScene();
	}	
}

bool CMeshD3DTest::TestTerminate()
{
	// Cleanup material
//    RELEASE(pMaterial);

	return true;
}

//************************************************************************
// Internal API:    DrawMeshTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

D3DCOLOR CMeshD3DTest::DrawMeshTests(int nTest)
{
	D3DCOLOR	MaterialColor;

	switch (nTest)
	{
		// Straight Triangle Strip (Horizontal), D3DPT_TRIANGLESTRIP
		//
		case (1):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Red", msgString);

			// Red Mesh
			MaterialColor = RGBA_MAKE(255,0,0,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.80f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.80f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.48f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.16f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.16f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.48f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.80f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.80f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (2):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Green", msgString);

			// Green Mesh
			MaterialColor = RGBA_MAKE(0,255,0,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.80f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.80f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.48f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.16f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.16f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.48f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.80f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.80f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (3):	
			sprintf(msgString, "%sHorizontal Triangle Strip: Blue", msgString);

			// Blue Mesh
			MaterialColor = RGBA_MAKE(0,0,255,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.80f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.80f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.48f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.16f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.16f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.48f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.80f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.80f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (4):	
			sprintf(msgString, "%sHorizontal Triangle Strip: White", msgString);

			// White Mesh
			MaterialColor = RGBA_MAKE(255,255,255,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.80f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.80f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.48f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.16f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.16f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.48f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.80f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.80f, 0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		//
		// Straight Triangle Strip (Vertical), D3DPT_TRIANGLESTRIP
		//
		case (5):	
			sprintf(msgString, "%sVertical Triangle Strip: Red", msgString);

			// Red Mesh
			MaterialColor = RGBA_MAKE(255,0,0,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f,-0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f,-0.16f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.48f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f, 0.48f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.16f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f, 0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (6):	
			sprintf(msgString, "%sVertical Triangle Strip: Green", msgString);

			// Green Mesh
			MaterialColor = RGBA_MAKE(0,255,0,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f,-0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f,-0.16f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.48f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f, 0.48f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.16f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f, 0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (7):	
			sprintf(msgString, "%sVertical Triangle Strip: Blue", msgString);

			// Blue Mesh
			MaterialColor = RGBA_MAKE(0,0,255,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f,-0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f,-0.16f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.48f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f, 0.48f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.16f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f, 0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (8):	
			sprintf(msgString, "%sVertical Triangle Strip: White", msgString);

			// White Mesh
			MaterialColor = RGBA_MAKE(255,255,255,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f,-0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f,-0.16f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.48f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f, 0.48f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.16f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f, 0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.80f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		//
		// Bent Triangle Strip, D3DPT_TRIANGLESTRIP
		//
		case (9):	
			sprintf(msgString, "%sBent Triangle Strip: Red", msgString);

			// Red Mesh
			MaterialColor = RGBA_MAKE(255,0,0,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.6f,-0.6f, 0.5f), cD3DVECTOR(-0.3487f, -0.4650f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.2f,-0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.6f,-0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.2f,-0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f, 0.0f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.0f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.2f, 0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.0f, 0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (10):	
			sprintf(msgString, "%sBent Triangle Strip: Green", msgString);

			// Green Mesh
			MaterialColor = RGBA_MAKE(0,255,0,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.6f,-0.6f, 0.5f), cD3DVECTOR(-0.3487f, -0.4650f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.2f,-0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.6f,-0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.2f,-0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f, 0.0f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.0f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.2f, 0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.0f, 0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (11):	
			sprintf(msgString, "%sBent Triangle Strip: Blue", msgString);

			// Blue Mesh
			MaterialColor = RGBA_MAKE(0,0,255,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.6f,-0.6f, 0.5f), cD3DVECTOR(-0.3487f, -0.4650f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.2f,-0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.6f,-0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.2f,-0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f, 0.0f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.0f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.2f, 0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.0f, 0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		case (12):	
			sprintf(msgString, "%sBent Triangle Strip: White", msgString);

			// White Mesh
			MaterialColor = RGBA_MAKE(255,255,255,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.6f,-0.6f, 0.5f), cD3DVECTOR(-0.3487f, -0.4650f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.2f,-0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f,-0.4f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.6f,-0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.2f,-0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f, 0.0f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.0f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.2f, 0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.2f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.0f, 0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			break;
		//
		// Two crossed strips, D3DPT_TRIANGLESTRIP
		//
		case (13):	
			sprintf(msgString, "%sCrossed Triangle Strips: Red & Green", msgString);

			// Red strip
			MaterialColor = RGBA_MAKE(255,0,0,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.8000f,-0.8f, 0.9000f), cD3DVECTOR(-0.3487f, -0.4650f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.8000f,-0.4f, 0.7857f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.2666f,-0.4f, 0.6714f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.2666f, 0.0f, 0.5571f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.2666f, 0.0f, 0.4429f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.2666f, 0.4f, 0.3286f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.8000f, 0.4f, 0.2143f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.8000f, 0.8f, 0.1000f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			// Green strip
			MaterialColor = RGBA_MAKE(0,255,0,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.8f,-0.8000f, 0.1000f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f,-0.8000f, 0.2143f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f,-0.2666f, 0.3286f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.0f,-0.2666f, 0.4429f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.0f, 0.2666f, 0.5571f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.4f, 0.2666f, 0.6714f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.4f, 0.8000f, 0.7857f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.8f, 0.8000f, 0.8000f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			break;
		case (14):	
			sprintf(msgString, "%sCrossed Triangle Strips: Red & Black", msgString);

			// Red strip
			MaterialColor = RGBA_MAKE(255,0,0,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.8000f,-0.8f, 0.9000f), cD3DVECTOR(-0.3487f, -0.4650f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.8000f,-0.4f, 0.7857f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.2666f,-0.4f, 0.6714f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.2666f, 0.0f, 0.5571f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.2666f, 0.0f, 0.4429f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.2666f, 0.4f, 0.3286f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.8000f, 0.4f, 0.2143f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.8000f, 0.8f, 0.1000f), cD3DVECTOR(0.0f,0.0f,0.0f), 0.0f,0.0f);
			// Black strip
			MaterialColor = RGBA_MAKE(0,0,0,255);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.8f,-0.8000f, 0.1000f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f,-0.8000f, 0.2143f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.4f,-0.2666f, 0.3286f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.0f,-0.2666f, 0.4429f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR( 0.0f, 0.2666f, 0.5571f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.4f, 0.2666f, 0.6714f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.4f, 0.8000f, 0.7857f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.8f, 0.8000f, 0.8000f), cD3DVECTOR(-0.4650f, 0.3487f, -0.8137f), 0.0f,0.0f);
			break;
		//
		// Triangle fan Square, D3DPT_TRIANGLEFAN
		//
		case (15):	
		case (16):	
		case (17):	
		case (18):	
			{
				int i;

				D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,-1.0f), 0.0f,0.0f);

				for (i = -8; i < 9; i++)
				{
					D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR((float)i / 10.0f, 0.8f, 0.5f), cD3DVECTOR(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}
				for (i = 8; i > -9; i--)
				{
					D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(0.8f, (float)i / 10.0f, 0.5f), cD3DVECTOR(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}
				for (i = 8; i > -9; i--)
				{
					D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR((float)i / 10.0f,-0.8f, 0.5f), cD3DVECTOR(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}
				for (i = -8; i < 9; i++)
				{
					D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.8f, (float)i / 10.0f, 0.5f), cD3DVECTOR(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}

				switch (nTest)
				{
					case (15):	// Red fan
						sprintf(msgString, "%sSquare Triangle Fan: Red", msgString);
						MaterialColor = RGBA_MAKE(255,0,0,255);
						break;
					case (16):	// Green fan
						sprintf(msgString, "%sSquare Triangle Fan: Green", msgString);
						MaterialColor = RGBA_MAKE(0,255,0,255);
						break;
					case (17):	// Blue fan
						sprintf(msgString, "%sSquare Triangle Fan: Blue", msgString);
						MaterialColor = RGBA_MAKE(0,0,255,255);
						break;
					case (18):	// White fan
						sprintf(msgString, "%sSquare Triangle Fan: White", msgString);
						MaterialColor = RGBA_MAKE(255,255,255,255);
						break;
				}
			}
			break;
		//
		// Jagged Triangle fan Square, D3DPT_TRIANGLEFAN
		//
		case (19):	
		case (20):	
		case (21):	
		case (22):	
			{
				int i;
				bool bEven;

				D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR(-0.6f, 0.6f, 0.5f), cD3DVECTOR(0.0f,0.0f,-1.0f), 0.0f,0.0f);

				bEven = true;
				for (i = -8; i < 9; i++)
				{
					bEven = (bEven ? false : true);
					D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR((float)i / 10.0f, (bEven ? 0.7f : 0.8f), (bEven ? 0.6f : 0.4f)), cD3DVECTOR(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}
				for (i = 8; i > -9; i--)
				{
					bEven = (bEven ? false : true);
					D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR((bEven ? 0.3f : 0.8f), (float)i / 10.0f, (bEven ? 0.6f : 0.4f)), cD3DVECTOR(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}
				for (i = 8; i > -9; i--)
				{
					bEven = (bEven ? false : true);
					D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR((float)i / 10.0f, (bEven ? -0.3f : -0.8f), (bEven ? 0.6f : 0.4f)), cD3DVECTOR(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}
				for (i = -8; i < 9; i++)
				{
					bEven = (bEven ? false : true);
					D3DVertexList[dwVertexCount++] = cD3DVERTEX(cD3DVECTOR((bEven ? -0.7f : -0.8f), (float)i / 10.0f, (bEven ? 0.6f : 0.4f)), cD3DVECTOR(0.0f,0.0f,-1.0f), 0.0f,0.0f);
				}

				switch (nTest)
				{
					case (19):	// Red fan
						sprintf(msgString, "%sJagged Triangle Fan: Red", msgString);
						MaterialColor = RGBA_MAKE(255,0,0,255);
						break;
					case (20):	// Green fan
						sprintf(msgString, "%sJagged Triangle Fan: Green", msgString);
						MaterialColor = RGBA_MAKE(0,255,0,255);
						break;
					case (21):	// Blue fan
						sprintf(msgString, "%sJagged Triangle Fan: Blue", msgString);
						MaterialColor = RGBA_MAKE(0,0,255,255);
						break;
					case (22):	// White fan
						sprintf(msgString, "%sJagged Triangle Fan: White", msgString);
						MaterialColor = RGBA_MAKE(255,255,255,255);
						break;
				}
			}
			break;
	}

	return MaterialColor;
}

