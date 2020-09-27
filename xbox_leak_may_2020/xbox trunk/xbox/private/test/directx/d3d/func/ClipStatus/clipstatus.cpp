#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "ClipStatus.h"

TEST    ClipStatus ::Test[22] = 
{

TEST(CSVERTEX(-2.9f,-0.9f, 0.0f,'r'),CSVERTEX(-2.0f, 0.9f, 0.0f,'g'),CSVERTEX(-1.1f,-0.9f, 0.0f,'b')),
TEST(CSVERTEX(-2.0f,-0.9f, 0.0f,'r'),CSVERTEX(-1.1f, 0.9f, 0.0f,'g'),CSVERTEX(-0.2f,-0.9f, 0.0f,'b')),
TEST(CSVERTEX(-1.8f,-0.9f, 0.0f,'r'),CSVERTEX(-0.9f, 0.9f, 0.0f,'g'),CSVERTEX( 0.0f,-0.9f, 0.0f,'b')),
TEST(CSVERTEX(-0.9f,-0.9f, 0.0f,'r'),CSVERTEX( 0.0f, 0.9f, 0.0f,'g'),CSVERTEX( 0.9f,-0.9f, 0.0f,'b')),
TEST(CSVERTEX( 0.0f,-0.9f, 0.0f,'r'),CSVERTEX( 0.9f, 0.9f, 0.0f,'g'),CSVERTEX( 1.8f,-0.9f, 0.0f,'b')),
TEST(CSVERTEX( 0.2f,-0.9f, 0.0f,'r'),CSVERTEX( 1.1f, 0.9f, 0.0f,'g'),CSVERTEX( 2.0f,-0.9f, 0.0f,'b')),
TEST(CSVERTEX( 1.1f,-0.9f, 0.0f,'r'),CSVERTEX( 2.0f, 0.9f, 0.0f,'g'),CSVERTEX( 2.9f,-0.9f, 0.0f,'b')),


TEST(CSVERTEX(-0.9f,-2.9f, 0.0f,'r'),CSVERTEX( 0.0f,-1.1f, 0.0f,'g'),CSVERTEX( 0.9f,-2.9f, 0.0f,'b')),
TEST(CSVERTEX(-0.9f,-2.0f, 0.0f,'r'),CSVERTEX( 0.0f,-0.2f, 0.0f,'g'),CSVERTEX( 0.9f,-2.0f, 0.0f,'b')),
TEST(CSVERTEX(-0.9f,-1.8f, 0.0f,'r'),CSVERTEX( 0.0f, 0.0f, 0.0f,'g'),CSVERTEX( 0.9f,-1.8f, 0.0f,'b')),
TEST(CSVERTEX(-0.9f,-0.9f, 0.0f,'r'),CSVERTEX( 0.0f, 0.9f, 0.0f,'g'),CSVERTEX( 0.9f,-0.9f, 0.0f,'b')),
TEST(CSVERTEX(-0.9f, 0.0f, 0.0f,'r'),CSVERTEX( 0.0f, 1.8f, 0.0f,'g'),CSVERTEX( 0.9f, 0.0f, 0.0f,'b')),
TEST(CSVERTEX(-0.9f, 0.2f, 0.0f,'r'),CSVERTEX( 0.0f, 2.0f, 0.0f,'g'),CSVERTEX( 0.9f, 0.2f, 0.0f,'b')),
TEST(CSVERTEX(-0.9f, 1.1f, 0.0f,'r'),CSVERTEX( 0.0f, 2.9f, 0.0f,'g'),CSVERTEX( 0.9f, 1.1f, 0.0f,'b')),


TEST(CSVERTEX(-0.9f,-0.9f,-2.0f,'r'),CSVERTEX( 0.0f, 0.9f,-2.0f,'g'),CSVERTEX( 0.9f,-0.9f,-2.0f,'b')),
TEST(CSVERTEX(-0.9f,-0.9f,-2.0f,'r'),CSVERTEX( 0.0f, 0.9f,-2.0f,'g'),CSVERTEX( 0.9f,-0.9f, 0.0f,'b')),
TEST(CSVERTEX(-0.9f,-0.9f,-2.0f,'r'),CSVERTEX( 0.0f, 0.9f, 0.0f,'g'),CSVERTEX( 0.9f,-0.9f, 0.0f,'b')),
TEST(CSVERTEX(-0.9f,-0.9f,-2.0f,'r'),CSVERTEX( 0.0f, 0.9f, 0.0f,'g'),CSVERTEX( 0.9f,-0.9f, 2.0f,'b')),
TEST(CSVERTEX(-0.9f,-0.9f,-2.0f,'r'),CSVERTEX( 0.0f, 0.9f, 2.0f,'g'),CSVERTEX( 0.9f,-0.9f, 2.0f,'b')),
TEST(CSVERTEX(-0.9f,-0.9f, 0.0f,'r'),CSVERTEX( 0.0f, 0.9f, 0.0f,'g'),CSVERTEX( 0.9f,-0.9f, 2.0f,'b')),
TEST(CSVERTEX(-0.9f,-0.9f, 0.0f,'r'),CSVERTEX( 0.0f, 0.9f, 2.0f,'g'),CSVERTEX( 0.9f,-0.9f, 2.0f,'b')),
TEST(CSVERTEX(-0.9f,-0.9f, 2.0f,'r'),CSVERTEX( 0.0f, 0.9f, 2.0f,'g'),CSVERTEX( 0.9f,-0.9f, 2.0f,'b')),

};

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

    ClipStatus* pClipStatus;
    BOOL        bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pClipStatus = new ClipStatus();
    if (!pClipStatus) {
        return FALSE;
    }

    // Initialize the scene
    if (!pClipStatus->Create(pDisplay)) {
        pClipStatus->Release();
        return FALSE;
    }

    bRet = pClipStatus->Exhibit(pnExitCode);

    // Clean up the scene
    pClipStatus->Release();

    return bRet;
}

ClipStatus::ClipStatus()
{
	m_szTestName = TEXT("ClipStatus");
	m_szCommandKey = TEXT("clipstatus");

    m_pSrcSrcVB = NULL;
//    m_pSrcRefVB = NULL;
//    m_pDstSrcVB = NULL;
//    m_pMaterial = NULL;
//    m_pLight = NULL;
}

ClipStatus::~ClipStatus()
{
	ReleaseVertexBuffer(m_pSrcSrcVB);
//	ReleaseVertexBuffer(m_pSrcRefVB);
//	ReleaseVertexBuffer(m_pDstSrcVB);
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
}


UINT ClipStatus::TestInitialize(void)
{
    m_dwTotalTableTests = sizeof(Test) / sizeof(TEST);
    m_dwTotalTests = m_dwTotalTableTests * 4;
    
    SetTestRange(1, m_dwTotalTests);

    // Recalculate clip status for tests;
    for (DWORD i = 0; i < m_dwTotalTableTests; i++)
    {
        Test[i].dwClipUnion = 0;
        Test[i].dwClipIntersection = 
            CLIPSTATUS_RIGHT | CLIPSTATUS_LEFT | CLIPSTATUS_TOP | 
            CLIPSTATUS_BOTTOM | CLIPSTATUS_FRONT | CLIPSTATUS_BACK;


        for (DWORD j = 0; j < 3; j++)
        {
            if (Test[i].v[j].x < -1.f)  Test[i].dwClipUnion |= CLIPSTATUS_LEFT;
            else                        Test[i].dwClipIntersection &= ~(CLIPSTATUS_LEFT);

            if (Test[i].v[j].x > 1.f)   Test[i].dwClipUnion |= CLIPSTATUS_RIGHT;
            else                        Test[i].dwClipIntersection &= ~(CLIPSTATUS_RIGHT);

            if (Test[i].v[j].y < -1.f)  Test[i].dwClipUnion |= CLIPSTATUS_BOTTOM;
            else                        Test[i].dwClipIntersection &= ~(CLIPSTATUS_BOTTOM);

            if (Test[i].v[j].y > 1.f)   Test[i].dwClipUnion |= CLIPSTATUS_TOP;
            else                        Test[i].dwClipIntersection &= ~(CLIPSTATUS_TOP);

            if (Test[i].v[j].z < -1.f)  Test[i].dwClipUnion |= CLIPSTATUS_FRONT;
            else                        Test[i].dwClipIntersection &= ~(CLIPSTATUS_FRONT);

            if (Test[i].v[j].z > 1.f)   Test[i].dwClipUnion |= CLIPSTATUS_BACK;
            else                        Test[i].dwClipIntersection &= ~(CLIPSTATUS_BACK);
        }
    }
   
    return D3DTESTINIT_RUN;
}


bool ClipStatus::ExecuteTest(UINT iTestNumber)
{
    D3DCLIPSTATUS8  ClipStatus;
    CSVERTEX*       pVertex;
    DWORD           dwSize;
    DWORD           num;
    HRESULT         hr;
    
    
    m_dwCurrentTestNumber = (DWORD)iTestNumber;
    num = m_dwCurrentTestNumber - 1;
    m_dwTableTestNumber = num % m_dwTotalTableTests;
    num /= m_dwTotalTableTests;
    m_bOptimize = (num % 2) ? true : false;
    num /= 2;
    m_bLighting = (num % 2) ? true : false;
    num /= 2;

    // Clear old vertex buffers
    ReleaseVertexBuffer(m_pSrcSrcVB);
//    ReleaseVertexBuffer(m_pSrcRefVB);
//    ReleaseVertexBuffer(m_pDstSrcVB);

    BeginTestCase("ClipStatus test");

//    m_pDevice->SetRenderState(D3DRS_EXTENTS, TRUE);
   
    // Create vertex buffers
//    m_pSrcSrcVB = CreateVertexBuffer(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE, 3, D3DVBCAPS_SYSTEMMEMORY, false);
    m_pDevice->CreateVertexBuffer(3 * sizeof(CSVERTEX), 0, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE, POOL_DEFAULT, &m_pSrcSrcVB);
    if (NULL == m_pSrcSrcVB)
    {
        WriteToLog("ClipStatus: Unable to create source vertex buffer\n");
        Fail();
        EndTestCase();
        return false;
    }
/*
    m_pSrcRefVB = CreateVertexBuffer(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE, 3, D3DVBCAPS_SYSTEMMEMORY, true);
    if (NULL == m_pSrcRefVB)
    {
        WriteToLog("ClipStatus: Unable to create source vertex buffer\n");
        Fail();
        EndTestCase();
        return false;
    }
    m_pDstSrcVB = CreateVertexBuffer(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR, 3, D3DVBCAPS_SYSTEMMEMORY, false);
    if (NULL == m_pDstSrcVB)
    {
        WriteToLog("ClipStatus: Unable to create destination vertex buffer\n");
        Fail();
        EndTestCase();
        return false;
    }
*/
    // Copy vertices to the source vertex buffer
//    if (m_pSrcSrcVB->Lock((void**)&pVertex, &dwSize))
    hr =  m_pSrcSrcVB->Lock(0, 3 * sizeof(CSVERTEX), (BYTE**)&pVertex, 0);
    if (SUCCEEDED(hr))
    {
        memcpy(pVertex, &Test[m_dwTableTestNumber].v[0], 3*sizeof(CSVERTEX));
        m_pSrcSrcVB->Unlock();
    }
    else
    {
        WriteToLog("ClipStatus: Unable to lock source vertex buffer\n");
        Fail();
        EndTestCase();
        return false;
    }
/*
    if (m_pSrcRefVB->Lock((void**)&pVertex, &dwSize))
    {
        memcpy(pVertex, &Test[m_dwTableTestNumber].v[0], 3*sizeof(VERTEX));
        m_pSrcRefVB->Unlock();
    }
    else
    {
        WriteToLog("ClipStatus: Unable to lock source vertex buffer\n");
        Fail();
        EndTestCase();
        return false;
    }
*/
    // Setting clipping status
    ClipStatus.ClipUnion = 0;
    ClipStatus.ClipIntersection = CLIPSTATUS_ALL;
    hr = m_pDevice->SetClipStatus(&ClipStatus);
    if (FAILED(hr))
    {
        WriteToLog("ClipStatus: SetClipStatus failed\n");
        Fail();
        EndTestCase();
        return false;
    }
/*
    if (m_bOptimize)
    {
        if (!m_pSrcSrcVB->Optimize(false))
        {
            WriteToLog("ClipStatus: Failed to optimize source vertex buffer\n");
            Fail();
            EndTestCase();
            return false;
        }
    }
*/
    if (m_dwVersion > 0x0600)
    {
        if (m_bLighting)
        {
            SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)TRUE);
            SetRenderState(D3DRENDERSTATE_COLORVERTEX, (DWORD)TRUE);
            SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE, (DWORD)D3DMCS_COLOR1);
            SetRenderState(D3DRENDERSTATE_AMBIENTMATERIALSOURCE, (DWORD)D3DMCS_COLOR1);
        }
        else
        {
            SetRenderState(D3DRENDERSTATE_COLORVERTEX, (DWORD)FALSE);
            SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)FALSE);
        }
    }
/*
    // Processing vertices
    DWORD   dwProcessOp = D3DVOP_TRANSFORM | D3DVOP_CLIP;
    if (m_bLighting || m_dwVersion < 0x0700)
        dwProcessOp |= D3DVOP_LIGHT;

    if (m_dwVersion >= 0x0800)
    {
        if (!SetStreamSource(0, m_pSrcSrcVB, NULL, GetVertexSize(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE), CD3D_SRC) ||
            !SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE, 0) ||
            !ProcessVertices(0, 0, 3, m_pDstSrcVB, 0))
        {
            WriteToLog("ClipStatus: ProcessVertices failed\n");
            Fail();
            EndTestCase();
            return false;
        }

    }
    else
    {
        if (!m_pDstSrcVB->ProcessVertices(dwProcessOp, 0, 3, m_pSrcSrcVB, 0, false, 0))
        {
            WriteToLog("ClipStatus: ProcessVertices failed\n");
            Fail();
            EndTestCase();
            return false;
        }
    }
*/
    return true;
}

void ClipStatus::SceneRefresh(void)
{
    if (BeginScene())
    {
        m_pDevice->SetStreamSource(0, m_pSrcSrcVB, sizeof(CSVERTEX));
        m_pDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE);
        m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
//        DrawPrimitiveVB(D3DPT_TRIANGLELIST, m_pDstSrcVB, m_pSrcRefVB, (DWORD)0, (DWORD)3, NULL, 0, (DWORD)0);
        EndScene();
    }
}


bool ClipStatus::ProcessFrame(void)
{
    D3DCLIPSTATUS8  ClipStatus;
    HRESULT         hr;

    ZeroMemory(&ClipStatus, sizeof(ClipStatus));
    
    hr = m_pDevice->GetClipStatus(&ClipStatus);
    if (FAILED(hr))
    {
        WriteToLog("ClipStatus: SetClipStatus failed\n");
        return false;
    }
    
    if (ClipStatus.ClipIntersection != Test[m_dwTableTestNumber].dwClipIntersection)
    {
        WriteToLog("ClipStatus: Returned clip status incorrect on test N %d\n", m_dwCurrentTestNumber);
        WriteToLog("Returned dwClipIntersection:\t0x%08x\n", ClipStatus.ClipIntersection);
        WriteToLog("Correct dwClipIntersection:\t0x%08x\n", Test[m_dwTableTestNumber].dwClipIntersection);
        Fail();
    }
    else if (ClipStatus.ClipUnion != Test[m_dwTableTestNumber].dwClipUnion)
    {
        WriteToLog("ClipStatus: Returned clip status incorrect on test N %d\n", m_dwCurrentTestNumber);
        WriteToLog("Returned dwClipUnion:\t0x%08x\n", ClipStatus.ClipUnion);
        WriteToLog("Correct dwClipUnion:\t0x%08x\n", Test[m_dwTableTestNumber].dwClipUnion);
        Fail();
    }
    else
    {
    	bool    bResult;

        bResult = GetCompareResult(0.15f, 0.78f, 0);
        if (!bResult) {
	    	WriteToLog("$rTest fails on frame %d\n", m_dwCurrentTestNumber);
		    Fail();
        }
    	else {
	    	Pass();
	    }
    }

	EndTestCase();
	return true;
}


bool ClipStatus::TestTerminate(void)
{
	ReleaseVertexBuffer(m_pSrcSrcVB);
//	RELEASE(m_pSrcRefVB);
//	RELEASE(m_pDstSrcVB);
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
	return true;
}

bool ClipStatus::SetDefaultMatrices(void) {
    D3DMATRIX           Matrix;
    // We need to be far from scane to receive correct specular highlights
	D3DVECTOR           from = cD3DVECTOR(0.0f, 0.0f, 1.0f); 
    D3DVECTOR           at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
    D3DVECTOR	        up = cD3DVECTOR(0.0f, 1.0f, 0.0f);
    float				fWidth = 100.f;
	float				fHeight = 100.f;
	float               fZn = 1.f;
	float               fZf = 3.f;

    
	// Projection matrix
	Matrix = OrthoMatrix(2.f, 2.f, -1.f, 1.f);
	if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&Matrix))
		return false;

    Matrix = IdentityMatrix();
    if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix))
        return false;

    Matrix = IdentityMatrix();
//  Matrix = ViewMatrix(from, at, up);
    if (!SetTransform(D3DTRANSFORMSTATE_VIEW,&Matrix))
        return false;

    return true;
}

bool ClipStatus::SetDefaultRenderStates(void) {
    if (m_dwVersion >= 0x0700)
		SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)FALSE);
	return true;
}


bool ClipStatus::SetDefaultMaterials(void) {
    D3DMATERIAL8        Material;
    HRESULT             hr;

    ZeroMemory(&Material,sizeof(D3DMATERIAL8));
    Material.Diffuse.r  = 1.0f;
    Material.Diffuse.g  = 1.0f;
    Material.Diffuse.b  = 1.0f;
    Material.Diffuse.a  = 1.0f;

    Material.Ambient.r  = 1.0f;
    Material.Ambient.g  = 1.0f;
    Material.Ambient.b  = 1.0f;
    Material.Ambient.a  = 1.0f;

    Material.Specular.r = 1.0f;
    Material.Specular.g = 1.0f;
    Material.Specular.b = 1.0f;
    Material.Specular.a = 1.0f;

    Material.Emissive.r = 0.0f;
    Material.Emissive.g = 0.0f;
    Material.Emissive.b = 0.0f;
    Material.Emissive.a = 0.0f;

    Material.Power      = 20.0f;

    hr = m_pDevice->SetMaterial(&Material);
    return (hr == D3D_OK);
}


bool ClipStatus::SetDefaultLights(void) {
    D3DLIGHT8   Light;
	D3DVECTOR	vLightPos = cD3DVECTOR(0.f, 0.f, 500.f);
	D3DVECTOR	vLightDir = cD3DVECTOR(0.f, -0.707f, 0.707f);
    HRESULT     hr;

	ZeroMemory(&Light,sizeof(D3DLIGHT8));

	Light.Type           = D3DLIGHT_DIRECTIONAL;
	Light.Diffuse.r		= 0.7f;
	Light.Diffuse.g		= 0.7f;
	Light.Diffuse.b		= 0.7f;
	Light.Diffuse.a		= 1.0f;
	Light.Specular.r	= 0.0f;
	Light.Specular.g	= 0.0f;
	Light.Specular.b	= 1.0f;
	Light.Specular.a	= 1.0f;
	Light.Position		= vLightPos;
	Light.Direction		= vLightDir;
	Light.Range         = D3DLIGHT_RANGE_MAX;
	Light.Attenuation0  = 0.f;
	Light.Attenuation1  = 0.f;
	Light.Attenuation2  = 0.f;
    Light.Theta			= pi/50;
    Light.Phi			= pi/20;

    hr = m_pDevice->SetLight(0, &Light);
    if (FAILED(hr)) {
        return false;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
	
	return (hr == D3D_OK);
}


bool ClipStatus::SetDefaultLightStates(void) {
    if (m_dwVersion >= 0x0700) 
	{
		SetRenderState(D3DRENDERSTATE_AMBIENT,RGB_MAKE(90,90,90));
		SetRenderState(D3DRENDERSTATE_CLIPPING, (DWORD)TRUE);
	}
	else 
	{
//	    SetLightState(D3DLIGHTSTATE_AMBIENT,RGB_MAKE(50,50,50));
	}
    return true;
}

