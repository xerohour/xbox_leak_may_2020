//#define D3D_OVERLOADS

#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "CShapes.h"
#include "VBlend.h"
#include "Groups.h"

// ---------------------------------------------------------------------------------------
//  Global Test and App instance
// ---------------------------------------------------------------------------------------

//CD3DWindowFramework App;

// --------------------------------------------------------------------------------------- 
//  Helper functions
// --------------------------------------------------------------------------------------- 

//bool CopyVB(CVertexBuffer8 *pSrc, CVertexBuffer8 *pDst, DWORD dwSize, DWORD dwCount);
//bool CopyIB(CIndexBuffer8 *pSrc, CIndexBuffer8 *pDst, DWORD dwSize, DWORD dwCount);

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

    CVBlend*        pVBlend;
    BOOL            bQuit = FALSE, bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 1 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pVBlend = new BlendPos();
                break;
/*
            case 1:
                pVBlend = new BlendNml();
                break;
            case 2:
                pVBlend = new BlendClr();
                break;
            case 3:
                pVBlend = new BlendTxc();
                break;
            case 4:
                pVBlend = new BlendPosNml();
                break;
            case 5:
                pVBlend = new BlendPosClr();
                break;
            case 6:
                pVBlend = new BlendPosTxc();
                break;
            case 7:
                pVBlend = new BlendPosNmlClr();
                break;
            case 8:
                pVBlend = new BlendPosNmlTxc();
                break;
            case 9:
                pVBlend = new BlendPosNmlClrTxc();
                break;
            case 10:
                pVBlend = new BlendNmlClr();
                break;
            case 11:
                pVBlend = new BlendNmlTxc();
                break;
            case 12:
                pVBlend = new BlendNmlClrTxc();
                break;
            case 13:
                pVBlend = new BlendClrTxc();
                break;
*/
        }

        if (!pVBlend) {
            return FALSE;
        }

        // Initialize the scene
        if (!pVBlend->Create(pDisplay)) {
            pVBlend->Release();
            return FALSE;
        }

        bRet = pVBlend->Exhibit(pnExitCode);

        bQuit = pVBlend->AbortedExit();

        // Clean up the scene
        pVBlend->Release();
    }

    return bRet;
}

// --------------------------------------------------------------------------------------- 
//  CVBlend Functions
// ---------------------------------------------------------------------------------------  

CVBlend::CVBlend()
{
	m_wFlags    = 0;
	m_nVertices = 0;
	m_nIndices  = 0;
	m_pVBO_S    = 0; m_pVBO_R = 0;
	m_pVBF_S    = 0; m_pVBF_R = 0;
	m_pIB_S     = 0; m_pIB_R  = 0;

    m_bExit = FALSE;
}

// --------------------------------------------------------------------------------------- 

CVBlend::~CVBlend()
{
}
 
// --------------------------------------------------------------------------------------- 

bool CVBlend::SetDefaultMatrices(void)
{
	D3DMATRIX  Matrix;
	D3DVECTOR  from = { 0.0f, 0.0f, -250.0f };
	D3DVECTOR  at   = { 0.0f, 0.0f, 0.0f };
	D3DVECTOR  up   = { 0.0f, 1.0f, 0.0f };

	Matrix = ProjectionMatrix(1.0f, 1000.0f, TESTFRAME(pi)/4.0f);
	if(!SetTransform(D3DTS_PROJECTION,&Matrix))
		return false;

	Matrix = ViewMatrix(from,at,up);
	if(!SetTransform(D3DTS_VIEW, &Matrix))
		return false;

	Matrix = IdentityMatrix();
	if(!SetTransform(D3DTS_WORLD, &Matrix))
		return false;

	return true;
}

// ----------------------------------------------------------------------------

bool CVBlend::SetDefaultMaterials(void)
{
	D3DMATERIAL8 MaterialData;
    HRESULT hr;

//	pMaterial = CreateMaterial();

	memset(&MaterialData, 0, sizeof(D3DMATERIAL8));

	MaterialData.Diffuse.r  = 1.0f;
	MaterialData.Diffuse.g  = 1.0f;
	MaterialData.Diffuse.b  = 1.0f;
	MaterialData.Specular.r = 1.0f;
	MaterialData.Specular.g = 1.0f;
	MaterialData.Specular.b = 1.0f;
	MaterialData.Power      = 15.0f;

//	pMaterial->SetMaterial(&MaterialData);

//	if(!KeySet(_T("!Material"), true))
//		SetMaterial(pMaterial);
    
    hr = m_pDevice->SetMaterial(&MaterialData);
//	return true;
    return SUCCEEDED(hr);
}


// ----------------------------------------------------------------------------

bool CVBlend::SetDefaultLights(void)
{
	D3DLIGHT8  LightData;
    HRESULT hr1, hr2;

//	pLight1 = CreateLight();
//	pLight2 = CreateLight();
//	pLight3 = CreateLight();

//	if((!pLight1) || (!pLight2) || (!pLight3))
//		return false;

	memset(&LightData, 0, sizeof(D3DLIGHT8));
//	LightData.dwSize = sizeof(D3DLIGHT2);

	LightData.Type           = D3DLIGHT_POINT;
	LightData.Diffuse.r        = 1.0f;
	LightData.Diffuse.g        = 1.0f;
	LightData.Diffuse.b        = 1.0f;
	LightData.Diffuse.a        = 1.0f;
	LightData.Position.x      = -SPACING;
	LightData.Position.y      =  0.0f;
	LightData.Position.z      = -40.0f;
	LightData.Attenuation0    = 0.99f;
	LightData.Attenuation1    = 0.009f;
	LightData.Attenuation2    = 0.001f;
	LightData.Range           = 200.0f;
//	LightData.dwFlags           = D3DLIGHT_ACTIVE;

//	pLight1->SetLight(&LightData);

//	if(!SetLight(0, pLight1))
    hr1 = m_pDevice->SetLight(0, &LightData);
    hr2 = m_pDevice->LightEnable(0, TRUE);
    if (FAILED(hr1) || FAILED(hr2))
	{
		OutputDebugString(_T("CColVert::SetDefaultLights -- Setting light 0 failed\n"));
		Fail();
		return false;
	}

	LightData.Position.x = 0.0f;

//	pLight2->SetLight(&LightData);

//	if(!SetLight(1, pLight2))
    hr1 = m_pDevice->SetLight(1, &LightData);
    hr2 = m_pDevice->LightEnable(1, TRUE);
	{
		OutputDebugString(_T("CColVert::SetDefaultLights -- Setting light 1 failed\n"));
		Fail();
		return false;
	}

	LightData.Position.x = SPACING;

//	pLight3->SetLight(&LightData);

//	if(!SetLight(2, pLight3))
    hr1 = m_pDevice->SetLight(2, &LightData);
    hr2 = m_pDevice->LightEnable(2, TRUE);
	{
		OutputDebugString(_T("CColVert::SetDefaultLights -- Setting light 2 failed\n"));
		Fail();
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CVBlend::SetDefaultLightStates(void)
{
	return true;
}

// ----------------------------------------------------------------------------

bool CVBlend::SetDefaultRenderStates(void)
{
	SetRenderState(D3DRENDERSTATE_AMBIENT,RGBA_MAKE(50,50,50,255));
	SetRenderState(D3DRENDERSTATE_COLORVERTEX, (DWORD)true);
	SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE, (DWORD)D3DMCS_COLOR1);
	SetRenderState(D3DRENDERSTATE_SPECULARMATERIALSOURCE, (DWORD)D3DMCS_COLOR2);

	return true;
}
// --------------------------------------------------------------------------------------- 

UINT CVBlend::TestInitialize(void)
{
	//
	// check to ensure the test can run on this config!
	// 

	if(m_dwVersion < 0x0800)
	{
		WriteToLog(_T("VBlend - Test Requires DX8 or better to run!\n"));
		return D3DTESTINIT_SKIPALL;
	}
	if(D3DSHADER_VERSION_MAJOR(m_d3dcaps.VertexShaderVersion) < 1)
	{
		WriteToLog(_T("VBlend - Vertex Shaders not supported by hardware!  Skipping all tests...\n"));
		return D3DTESTINIT_SKIPALL;
	}

	//
	// attempt to set up the test
	//

	SetTestRange(1, MAX_TESTS);

	// create a grid that we'll deform using vertex blending
	CShapes Shape;	

	if(!Shape.NewShape(CS_MESH, 20, 20))
	{
		WriteToLog(_T("Failed to create new grid!  Aborting test...\n"));
		return D3DTESTINIT_ABORT;
	}

	m_nVertices = Shape.m_nVertices;
	m_nIndices  = Shape.m_nIndices;

	// create 3 vertex buffers.  One for the initial state, one for the final state,
	// and one for the blended state.
//	m_pVBO_S = CreateVertexBuffer(FVF_CVERTEX, m_nVertices, 0, false);
	m_pVBO_S = CreateVertexBuffer(m_pDevice, NULL, m_nVertices * GetVertexSize(FVF_CVERTEX_), 0, FVF_CVERTEX_);
//	m_pVBO_R = CreateVertexBuffer(FVF_CVERTEX, m_nVertices, 0, true);
//	m_pVBF_S = CreateVertexBuffer(0, sizeof(CVERTEX) * m_nVertices, 0, false);
	m_pVBF_S = CreateVertexBuffer(m_pDevice, NULL, sizeof(CVERTEX_) * m_nVertices, 0, 0);
//	m_pVBF_R = CreateVertexBuffer(0, sizeof(CVERTEX) * m_nVertices, 0, true);
//	m_pIB_S  = CreateIndexBuffer(m_nIndices, D3DFMT_INDEX16, false);
	m_pIB_S  = CreateIndexBuffer(m_pDevice, Shape.m_pIndices, m_nIndices * sizeof(WORD));
//	m_pIB_R  = CreateIndexBuffer(m_nIndices, D3DFMT_INDEX16, true);

//	if(!m_pVBO_S || !m_pVBO_R || !m_pVBF_S || !m_pVBF_R || !m_pIB_S || !m_pIB_R)
	if(!m_pVBO_S || !m_pVBF_S || !m_pIB_S)
	{
		WriteToLog(_T("Failed to create vertex/index buffers!  Aborting test...\n"));
		return D3DTESTINIT_ABORT;
	}
/*
	// copy the index info into the index buffer
	WORD *pIndices  = 0;

	m_pIB_S->Lock((void**)&pIndices);

	if(pIndices)
	{
		memcpy(pIndices, Shape.m_pIndices, sizeof(WORD) * m_nIndices);
		m_pIB_S->Unlock();  pIndices = 0;
	}
	else
	{
		WriteToLog(_T("Failed to lock index buffer!  Aborting test...\n"));
		return D3DTESTINIT_ABORT;
	}
*/
//	CopyIB(m_pIB_S, m_pIB_R, sizeof(WORD), m_nIndices);

	// now copy the info from the shape into the first vertex buffer.
	CVERTEX_ *pVertices = 0;
	int nR = 255, nG = 0, nB = 0;

//	m_pVBO_S->Lock((void**)&pVertices);
	m_pVBO_S->Lock(0, 0, (BYTE**)&pVertices, 0);

	if(pVertices)
	{
		for(int i = 0; i < m_nVertices; i++)
		{
			nG = (0.5f + pVertices[i].x) * 255;
			nB = (0.5f + pVertices[i].y) * 255;

			pVertices[i].x     = ((VERTEX_*)Shape.m_pVertices)[i].x * 3.0f;
			pVertices[i].y     = ((VERTEX_*)Shape.m_pVertices)[i].y * 3.0f;
			pVertices[i].z     = ((VERTEX_*)Shape.m_pVertices)[i].z * 3.0f;
			pVertices[i].nx    = ((VERTEX_*)Shape.m_pVertices)[i].nx;
			pVertices[i].ny    = ((VERTEX_*)Shape.m_pVertices)[i].ny;
			pVertices[i].nz    = ((VERTEX_*)Shape.m_pVertices)[i].nz;
			pVertices[i].color = D3DCOLOR_RGBA(nR, nG, nB, 255); 
			pVertices[i].tu    = ((VERTEX_*)Shape.m_pVertices)[i].tu;
			pVertices[i].tv    = ((VERTEX_*)Shape.m_pVertices)[i].tv;
		}
		m_pVBO_S->Unlock(); pVertices = 0;
	}

	// copy info from source VB to ref VB.
//	CopyVB(m_pVBO_S, m_pVBO_R, sizeof(CVERTEX), m_nVertices);

	// change the ref VB
//	m_pVBF_S->Lock((void**)&pVertices);
	m_pVBF_S->Lock(0, 0, (BYTE**)&pVertices, 0);

	if(pVertices)
	{
		for(int i = 0; i < m_nVertices; i++)
		{
			nG = (0.5f + pVertices[i].x) * 255;
			nB = (0.5f + pVertices[i].y) * 255;

			pVertices[i].x     = ((VERTEX_*)Shape.m_pVertices)[i].x * 3.0f;
			pVertices[i].y     = ((VERTEX_*)Shape.m_pVertices)[i].y * 3.0f;
			pVertices[i].z     = ((VERTEX_*)Shape.m_pVertices)[i].z * 3.0f;
			pVertices[i].nx    = ((VERTEX_*)Shape.m_pVertices)[i].nx;
			pVertices[i].ny    = ((VERTEX_*)Shape.m_pVertices)[i].ny;
			pVertices[i].nz    = ((VERTEX_*)Shape.m_pVertices)[i].nz;
			pVertices[i].color = D3DCOLOR_RGBA(nR, nG, nB, 255); 
			pVertices[i].tu    = ((VERTEX_*)Shape.m_pVertices)[i].tu;
			pVertices[i].tv    = ((VERTEX_*)Shape.m_pVertices)[i].tv;
		}
		m_pVBF_S->Unlock(); pVertices = 0;
	}

	if(m_wFlags & BLEND_POS)  // blend vertex positions
	{
//		m_pVBF_S->Lock((void**)&pVertices);
		m_pVBF_S->Lock(0, 0, (BYTE**)&pVertices, 0);
		if(pVertices)
		{
			// apply a ripple to the mesh
			for(int i = 0; i < m_nVertices; i++) 
			{
				pVertices[i].z *= (float)cos(pVertices[i].x);
			}
			m_pVBF_S->Unlock(); pVertices = 0;
		}
	}

	if(m_wFlags & BLEND_NML)  // blend vertex normals
	{
//		m_pVBF_S->Lock((void**)&pVertices);
		m_pVBF_S->Lock(0, 0, (BYTE**)&pVertices, 0);
		if(pVertices)
		{
			m_pVBF_S->Unlock(); pVertices = 0;
		}
	}

	if(m_wFlags & BLEND_CLR)  // blend vertex colors
	{
//		m_pVBF_S->Lock((void**)&pVertices);
		m_pVBF_S->Lock(0, 0, (BYTE**)&pVertices, 0);
		if(pVertices)
		{
			m_pVBF_S->Unlock(); pVertices = 0;
		}
	}

	if(m_wFlags & BLEND_TXC)  // blend vertex texture coordinates
	{
//		m_pVBF_S->Lock((void**)&pVertices);
		m_pVBF_S->Lock(0, 0, (BYTE**)&pVertices, 0);
		if(pVertices)
		{
			m_pVBF_S->Unlock(); pVertices = 0;
		}
	}

	// now copy the source VB to the ref VB.
//	CopyVB(m_pVBF_S, m_pVBF_R, sizeof(CVERTEX), m_nVertices);

	if(!CreateShader())
		return D3DTESTINIT_ABORT;

	return D3DTESTINIT_RUN;
}

// --------------------------------------------------------------------------------------- 

bool CVBlend::ClearFrame(void)
{
	return Clear(D3DCOLOR_RGBA(0,0,0,255));
}

// --------------------------------------------------------------------------------------- 
#define TL_DESCWIDTH 256
bool CVBlend::ExecuteTest(UINT uTestNum)
{
	m_fBlendFactor = MAX_TESTS / uTestNum;

	TCHAR tcsTestCase[TL_DESCWIDTH] = {0};

	_stprintf(
		tcsTestCase, _T("%d%% VBO, %d%% VBF"),
		m_fBlendFactor * 100, (1.0f - m_fBlendFactor) * 100);

	// Tell the log that we are starting
	BeginTestCase(tcsTestCase);

	return true;
}

// --------------------------------------------------------------------------------------- 

void CVBlend::SceneRefresh(void)
{
//	DebugOut.Write(_T("CVBlend::SceneRefresh()\n"));

	if(BeginScene())
	{
		struct {
			float TransMatrix[16];
			float BlendConsts[4];
		} Consts = {
			(1, 0, 0, 0,
			 0, 1, 0, 0,
			 0, 0, 1, 0,
			 0, 0, 0, 1),
			(m_fBlendFactor, 1.0f - m_fBlendFactor, 0, 0)
		};

		SetIndices(m_pIB_S, m_pIB_R, 0, 0);
		SetVertexShaderConstant(0, &Consts, 1);
		SetVertexShader(m_hShader);
		m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_nVertices, 0, m_nIndices/3);

		EndScene();
	}
}

// --------------------------------------------------------------------------------------- 

bool CVBlend::ProcessFrame(void)
{
	TCHAR	tcsResult[80] = { 0 };
	static	int nPass = 0;
	static  int nFail = 0;
	bool	bResult;

	// Use the standard 15% comparison
	bResult = GetCompareResult(0.15f, 0.78f, 0);

	// Tell the logfile how they did
	if (bResult)
	{
		(m_fIncrementForward) ? nPass++ : nPass--;
		_stprintf(tcsResult, _T("$yPass: %d, Fail: %d"), nPass, nFail);
		Pass();
	}
	else
	{
		(m_fIncrementForward) ? nFail++ : nFail--;
		_stprintf(tcsResult, _T("$yPass: %d, Fail: %d"), nPass, nFail);
		Fail();
	}

	WriteStatus(_T("$gOverall Results"), tcsResult);

	EndTestCase();

	return bResult;
}

// --------------------------------------------------------------------------------------- 

bool CVBlend::TestTerminate(void)
{
	ReleaseVertexBuffer(m_pVBO_S); //RELEASE(m_pVBO_R);
	ReleaseVertexBuffer(m_pVBF_S); //RELEASE(m_pVBF_R);
	ReleaseIndexBuffer(m_pIB_S);  //RELEASE(m_pIB_R);

	return true;
}

// ---------------------------------------------------------------------------------------

bool CVBlend::SetupShader(DWORD *pDecl, char *pcsCode)
{
//	DebugOut.Write(_T("CVBlend::SetupShader(%p, %s)\n"), pDecl, pcsCode);

	if(!pcsCode)
	{
//		DebugOut.Write(1, _T("CVBlend::SetupShader - pcsCode is NULL!\n"));
		DebugString(_T("CVBlend::SetupShader - pcsCode is NULL!"));
		return false;
	}

#if 0

	HRESULT      hr        = 0;
	LPD3DXBUFFER pCompiled = 0;
	LPD3DXBUFFER pErrors   = 0;

	hr = D3DXAssembleVertexShader(pcsCode, strlen(pcsCode), &pCompiled, &pErrors);

	if(FAILED(hr))
	{
		WriteToLog(_T("D3DXAssembleVertexShader failed with HRESULT %X\n"), hr);

//		DebugOut.Write(2,
//			_T("CVBlend::SetupShader - D3DXAssembleVertexShader failed with HRESULT %X\n"),
//			hr);
		if(pErrors)
		{
//			DebugOut.Write(2, (char*)pErrors->GetBufferPointer());
			pErrors->Release();
		}

		if(pCompiled)
			pCompiled->Release();

		return false;
	}

	if(!CreateVertexShader(pDecl, (DWORD*)pCompiled->GetBufferPointer(), &m_hShader, 0))
	{
		HRESULT hr = GetLastError();

		WriteToLog(_T("CreateVertexShader failed with HRESULT %X\n"), hr);

//		DebugOut.Write(2,
//			_T("CVBlend::SetupShader - CreateVertexShader failed with HRESULT %X\n"),
//			hr);

		return false;
	}

#else

	HRESULT hr;

    hr = m_pDevice->CreateVertexShaderAsm(pDecl, pcsCode, strlen(pcsCode), &m_hShader, 0);
    if (FAILED(hr)) {
        return false;
    }

#endif // UNDER_XBOX
	
	SetStreamSource(0, m_pVBO_S, m_pVBO_R, sizeof(CVERTEX_), 0);
	SetStreamSource(1, m_pVBF_S, m_pVBF_R, sizeof(CVERTEX_), 0);

	return true;
}

//******************************************************************************
BOOL CVBlend::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CVBlend::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CVBlend::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    *plr = 0;

#ifndef UNDER_XBOX

    switch (uMsg) {

        case WM_KEYDOWN:

            switch (wParam) {

                case VK_ESCAPE: // Exit
                    m_bExit = TRUE;
            }

            break;
    }

#endif // !UNDER_XBOX

    return CD3DTest::WndProc(plr, hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------------------
/*
bool CopyVB(CVertexBuffer *pSrc, CVertexBuffer *pDst, DWORD dwSize, DWORD dwCount)
{
	VOID *pVB1 = 0, *pVB2 = 0;

	pSrc->Lock(&pVB1); pDst->Lock(&pVB2);

	if(!pVB1 || !pVB2)
		return false;

	memcpy(pVB2, pVB1, dwSize * dwCount);
	
	pSrc->Unlock(); pDst->Unlock();

	return true;
}

// ---------------------------------------------------------------------------------------

bool CopyIB(CIndexBuffer *pSrc, CIndexBuffer *pDst, DWORD dwSize, DWORD dwCount)
{
	VOID *pIB1 = 0, *pIB2 = 0;

	pSrc->Lock(&pIB1); pDst->Lock(&pIB2);

	if(!pIB1 || !pIB2)
		return false;

	memcpy(pIB2, pIB1, dwSize * dwCount);
	
	pSrc->Unlock(); pDst->Unlock();
	
	return true;
}
*/
