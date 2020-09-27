#define STRICT
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "VBuffer.h"
#include "TestShapes.h"

// Global Test and App instance
//CD3DWindowFramework	App;

//******************************************************************************
//
// Function:
//
//     ValidateDisplay
//
// Description:
//
//     Evaluate the given display information in order to determine whether or
//     not the display is capable of rendering the scene.  If not, the given 
//     display will not be included in the display list.
//
// Arguments:
//
//     CDirect3D8* pd3d                 - Pointer to the Direct3D object
//
//     D3DCAPS8* pd3dcaps               - Capabilities of the device
//
//     D3DDISPLAYMODE*                  - Display mode into which the device
//                                        will be placed
//
// Return Value:
//
//     TRUE if the scene can be rendered using the given display, FALSE if
//     it cannot.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ValidateDisplay)(CDirect3D8* pd3d, D3DCAPS8* pd3dcaps, D3DDISPLAYMODE* pd3ddm) {

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     ExhibitScene
//
// Description:
//
//     Create the scene, pump messages, process user input,
//     update the scene, render the scene, and release the scene when finished.
//
// Arguments:
//
//     CDisplay* pDisplay           - Pointer to the Display object
//
//     int* pnExitCode              - Optional pointer to an integer that will
//                                    be set to the exit value contained in the 
//                                    wParam parameter of the WM_QUIT message 
//                                    (if received)
//
// Return Value:
//
//     TRUE if the display remains functional on exit, FALSE otherwise.
//
//******************************************************************************
extern "C" BOOL PREPEND_MODULE(_ExhibitScene)(CDisplay* pDisplay, int* pnExitCode) {

    CVBufferTest*   TestOne;
    BOOL            bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    TestOne = new CVBufferTest();
    if (!TestOne) {
        return FALSE;
    }

    // Initialize the scene
    if (!TestOne->Create(pDisplay)) {
        TestOne->Release();
        return FALSE;
    }

    bRet = TestOne->Exhibit(pnExitCode);

    // Clean up the scene
    TestOne->Release();

    return bRet;
}

// --------------------------------------------------------------------------------------

CVBufferTest::CVBufferTest()
{
	pVBSrc = 0;
	pVBRef = 0;
	nCurObjFrame = 0;
    m_szTestName = TEXT("VBufferTest");
}

// --------------------------------------------------------------------------------------

CVBufferTest::~CVBufferTest()
{
    UINT i;
//	delete pVBSrc;
//	delete pVBRef;
    ReleaseVertexBuffer(pVBSrc);
    for (i = 0; i < NUMOBJECTS; i++) {
        ReleaseIndexBuffer(Objects[i].pIBSrc);
    }
}

// --------------------------------------------------------------------------------------

bool CVBufferTest::SetDefaultMatrices(void)
{
	D3DMATRIX Matrix;
	D3DVECTOR from = cD3DVECTOR(0.0f, 1.5f, -3.0f);
	D3DVECTOR at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
	D3DVECTOR up = cD3DVECTOR(0.0f, 1.0f, 0.0f);

 	Matrix = ProjectionMatrix(1.0f, 10.0f, pi/3.0f);
	SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix);
 
	Matrix = ViewMatrix(from,at,up);
    SetTransform(D3DTRANSFORMSTATE_VIEW, &Matrix);

	Matrix = IdentityMatrix();
    SetTransform(D3DTRANSFORMSTATE_WORLD, &Matrix);

	return true;
}

// --------------------------------------------------------------------------------------

bool CVBufferTest::SetDefaultMaterials(void)
{
	D3DMATERIAL8 MaterialData;
    HRESULT hr;

//	pMaterial = CreateMaterial();

	memset(&MaterialData, 0, sizeof(D3DMATERIAL8));

	MaterialData.Diffuse.r = 1.0f;
	MaterialData.Diffuse.g = 1.0f;
	MaterialData.Diffuse.b = 1.0f;
	MaterialData.Specular.r = 1.0f;
	MaterialData.Specular.g = 1.0f;
	MaterialData.Specular.b = 1.0f;
	MaterialData.Power = 15.0f;
//	MaterialData.dwRampSize = 16;

//	pMaterial->SetMaterial(&MaterialData);

//	SetMaterial(pMaterial);

//    return true;

    hr = m_pDevice->SetMaterial(&MaterialData);
    return (hr == D3D_OK);
}

// --------------------------------------------------------------------------------------

bool CVBufferTest::SetDefaultLights(void)
{
    D3DLIGHT8 LightData;
    HRESULT hr;

//    pLight = CreateLight();

//	if(!pLight)	return false;

    memset(&LightData, 0, sizeof(D3DLIGHT8));
//    LightData.dwSize = sizeof(D3DLIGHT8);

    LightData.Type           = D3DLIGHT_DIRECTIONAL;
    LightData.Diffuse.r        = 1.0f;
    LightData.Diffuse.g        = 1.0f;
    LightData.Diffuse.b        = 1.0f;
    LightData.Diffuse.a        = 0.0f;
	LightData.Direction.x     = 0.577735f;
	LightData.Direction.y     = -0.577735f;
	LightData.Direction.z     = -0.577735f;
//	LightData.Flags           = D3DLIGHT_ACTIVE;

//    pLight->SetLight(&LightData);

//    if(!SetLight(0, pLight))
//    {
//        OutputDebugString(_T("CColVert::SetDefaultLights -- Setting light 0 failed\n"));
//        Fail();
//        return false;
//    }

//    return true;

    hr = m_pDevice->SetLight(0, &LightData);
    if (FAILED(hr)) {
        return false;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
	
	return (hr == D3D_OK);
}

// --------------------------------------------------------------------------------------

UINT CVBufferTest::TestInitialize(void)
{
	D3DVERTEX ArrowVtx[3] = {
		cD3DVERTEX(cD3DVECTOR(0.0f, 0.0f, 0.0f), cD3DVECTOR(0.0f, 0.0f, -1.0f), 0.5f, 0.0f),
		cD3DVERTEX(cD3DVECTOR(0.7f, -1.0f, 0.0f), cD3DVECTOR(0.0f, 0.0f, -1.0f), 1.0f, 1.0f),
		cD3DVERTEX(cD3DVECTOR(-0.7f, -1.0f, 0.0f), cD3DVECTOR(0.0f, 0.0f, -1.0f), 0.0f, 1.0f)
	};
	CTestShapes ts[NUMOBJECTS];
	int nTotalVertices = 3;

	// Setup the Test range 
	SetTestRange(1, NUMTESTS);

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	// make some shapes
	ts[0].NewShape(TS_POINTGRID);
	_tcscpy(Objects[0].tcsName, _T("point list"));
	ts[1].NewShape(TS_LINES);
	_tcscpy(Objects[1].tcsName, _T("line list"));
	ts[2].NewShape(TS_LINES);
	_tcscpy(Objects[2].tcsName, _T("line strip"));
	ts[3].NewShape(TS_RANDTRIS);
	_tcscpy(Objects[3].tcsName, _T("triangle list"));
	ts[4].NewShape(TS_FANBALL);
	_tcscpy(Objects[4].tcsName, _T("triangle fan"));
	ts[5].NewShape(TS_WAVESTRIP);
	_tcscpy(Objects[5].tcsName, _T("triangle strip"));

	// copy some of the shape info to the INFO structs
	for(int i = 0; i < NUMOBJECTS; i++)
	{
		Objects[i].nStart = nTotalVertices - 1;
		Objects[i].nVertices = ts[i].m_nVertices;
		Objects[i].PrimType = ts[i].m_Type;
		Objects[i].nIndices = ts[i].m_nIndices;
	
		Objects[i].pIndices = (WORD*)calloc(ts[i].m_nIndices, sizeof(WORD));
		if(!Objects[i].pIndices)
		{
			WriteToLog(_T("Failed to allocate memory!\n"));
			return D3DTESTINIT_ABORT;
		}
		else
			memcpy(Objects[i].pIndices, ts[i].m_pIndices, sizeof(WORD) * ts[i].m_nIndices);

        if (ts[i].m_nIndices) {
            Objects[i].pIBSrc = CreateIndexBuffer(m_pDevice, ts[i].m_pIndices, sizeof(WORD) * Objects[i].nVertices);
            if (!Objects[i].pIBSrc) {
                return D3DTESTINIT_ABORT;
            }
        }
        else {
            LPWORD pw;
            int j;
            Objects[i].pIBSrc = CreateIndexBuffer(m_pDevice, NULL, sizeof(WORD) * Objects[i].nVertices);
            if (!Objects[i].pIBSrc) {
                return D3DTESTINIT_ABORT;
            }
            Objects[i].pIBSrc->Lock(0, 0, (LPBYTE*)&pw, NULL);
            for (j = 0; j < Objects[i].nVertices; j++) {
                pw[j] = (WORD)j;
            }
            Objects[i].pIBSrc->Unlock();
        }

		nTotalVertices += ts[i].m_nVertices;
	}

	// setup the main vertex buffers
	{
		D3DVERTEX *pSrcVertices = NULL, *pRefVertices = NULL;
//		pVBSrc = CreateVertexBuffer(D3DFVF_VERTEX, nTotalVertices, 0);
        pVBSrc = CreateVertexBuffer(m_pDevice, NULL, sizeof(D3DVERTEX) * nTotalVertices, 0, D3DFVF_VERTEX);
//		pVBRef = CreateVertexBuffer(D3DFVF_VERTEX, nTotalVertices, D3DVBCAPS_SYSTEMMEMORY);

//		if(!pVBSrc || !pVBRef)
		if(!pVBSrc)
		{
			WriteToLog(_T("Failed to create vertex buffer!\n"));
			return D3DTESTINIT_ABORT;
		}

		nTotalVertices = 0;
//		if(pVBSrc->Lock((PVOID*)&pSrcVertices) && pVBRef->Lock((PVOID*)&pRefVertices))
        if (SUCCEEDED(pVBSrc->Lock(0, 0, (LPBYTE*)&pSrcVertices, 0)))
		{
			for(i = 0; i < NUMOBJECTS; i++)
			{
				memcpy(&(pSrcVertices[nTotalVertices]), ts[i].m_pVertices, sizeof(D3DVERTEX) * ts[i].m_nVertices);
//				memcpy(&(pRefVertices[nTotalVertices]), ts[i].m_pVertices, sizeof(D3DVERTEX) * ts[i].m_nVertices);
				nTotalVertices += ts[i].m_nVertices;
			}
			pVBSrc->Unlock();
//			pVBRef->Unlock();
		}
		else
		{
			WriteToLog(_T("Failed to lock vertex buffer!\n"));
			return D3DTESTINIT_ABORT;
		}
	}

	return D3DTESTINIT_RUN;
}

// --------------------------------------------------------------------------------------

bool CVBufferTest::ClearFrame(void)
{
	return Clear(RGBA_MAKE(0,0,0,255));
}

// --------------------------------------------------------------------------------------

bool CVBufferTest::ExecuteTest(UINT uTestNum)
{
	float     fRot = ROTFACTOR * nCurObjFrame;
	D3DMATRIX Matrix;
	
	// Tell the log that we are starting
	_sntprintf(
		tcsTestDesc, TL_DESCWIDTH, _T("%s, rotated %0.3fr"),
		Objects[nCurObj].tcsName, fRot);

	Matrix = RotateYMatrix(fRot);
    SetTransform(D3DTRANSFORMSTATE_WORLD, &Matrix);

	BeginTestCase(tcsTestDesc);
    return true;
}

// --------------------------------------------------------------------------------------

void CVBufferTest::SceneRefresh(void)
{
    UINT uNumPrimitives;

    switch (Objects[nCurObj].PrimType) {
        case D3DPT_POINTLIST:
            uNumPrimitives = Objects[nCurObj].nIndices;
            break;
        case D3DPT_LINELIST:
            uNumPrimitives = Objects[nCurObj].nIndices / 2;
            break;
        case D3DPT_LINESTRIP:
            uNumPrimitives = Objects[nCurObj].nIndices - 1;
            break;
        case D3DPT_TRIANGLELIST:
            uNumPrimitives = Objects[nCurObj].nIndices / 3;
            break;
        case D3DPT_TRIANGLESTRIP:
        case D3DPT_TRIANGLEFAN:
            uNumPrimitives = Objects[nCurObj].nIndices - 2;
            break;
    }

	if(BeginScene())
	{
        m_pDevice->SetVertexShader(D3DFVF_VERTEX);

        m_pDevice->SetStreamSource(0, pVBSrc, sizeof(VERTEX));
        m_pDevice->SetIndices(Objects[nCurObj].pIBSrc, Objects[nCurObj].nStart);

        m_pDevice->DrawIndexedPrimitive(Objects[nCurObj].PrimType, 
            Objects[nCurObj].nStart, Objects[nCurObj].nVertices,
            0, uNumPrimitives);
/*
		m_pSrcDevice7->DrawIndexedPrimitiveVB(
			Objects[nCurObj].PrimType, pVBSrc->m_pVB7,
			Objects[nCurObj].nStart, Objects[nCurObj].nVertices,
			Objects[nCurObj].pIndices, Objects[nCurObj].nIndices, 0);

		m_pRefDevice7->DrawIndexedPrimitiveVB(
			Objects[nCurObj].PrimType, pVBRef->m_pVB7,
			Objects[nCurObj].nStart, Objects[nCurObj].nVertices,
			Objects[nCurObj].pIndices, Objects[nCurObj].nIndices, 0);
*/
		EndScene();
	}

	nCurObjFrame++;

	if(nCurObjFrame == FRAMESPEROBJ)
	{
		nCurObjFrame = 0;
		nCurObj++;
	}
}

// --------------------------------------------------------------------------------------

bool CVBufferTest::ProcessFrame(void)
{
	TCHAR	szBuffer[80];
	static	int nPass = 0;
	static  int nFail = 0;
    bool	bResult;

	// Use the standard 15% comparison
	bResult = GetCompareResult(0.15f, 0.78f, 0);

	// Tell the logfile how they did
	if (bResult)
	{
		(m_fIncrementForward) ? nPass++ : nPass--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
		Pass();
	}
	else
	{
		(m_fIncrementForward) ? nFail++ : nFail--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
		Fail();
	}

	WriteStatus("$gOverall Results",szBuffer);

	EndTestCase();

    return bResult;
}

// --------------------------------------------------------------------------------------

bool CVBufferTest::TestTerminate(void)
{
	// Cleanup texture & image data
	SetTexture(0, NULL);
//	RELEASE(pMaterial);
//	RELEASE(pLight);
	return true;
}

