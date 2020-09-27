//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "PointTex.h"
//#include "cimageloader.h"

//CD3DWindowFramework     theApp;

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

    PointTex*       TestOne;
    BOOL            bRet = TRUE;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    TestOne = new PointTex();
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

PointTex::PointTex()
{
//    m_pMaterial = NULL;
//    m_pLight = NULL;
    for (int i = 0; i < 8; i++)
        m_pTextureArray[i] = NULL;

    m_pd3dr = NULL;
    m_pVertices = NULL;
    m_dwVertices = 0;

    m_dwCurrentTestNumber = 0;
    m_dwNumFailed = 0;

	m_szTestName = TEXT("PointTex");
	m_szCommandKey = TEXT("pointtex");
}

PointTex::~PointTex()
{
#ifdef UNDER_XBOX
    if (!(GetStartupContext() & TSTART_STRESS)) {
        m_pDisplay->ReleaseDevice();
        m_pDisplay->GetDirect3D8()->SetPushBufferSize(524288, 32768);
        m_pDisplay->CreateDevice();
    }
#endif

//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
    for (int i = 0; i < 8; i++)
    {
        ReleaseTexture(m_pTextureArray[i]);
    }

    if (m_pd3dr) {
        ReleaseVertexBuffer(m_pd3dr);
        m_pd3dr = NULL;
        m_dwVertices = 0;
    }
    if (m_pVertices)
    {
        delete [] m_pVertices;
        m_pVertices = NULL;
        m_dwVertices = 0;
    }
}

//******************************************************************************
//
// Method:
//
//     Create
//
// Description:
//
//     Prepare the scene for rendering.
//
// Arguments:
//
//     CDisplay* pDisplay               - Pointer to a Display object.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL PointTex::Create(CDisplay* pDisplay) {

#ifdef UNDER_XBOX
    if (!(GetStartupContext() & TSTART_STRESS)) {
        pDisplay->ReleaseDevice();
        pDisplay->GetDirect3D8()->SetPushBufferSize(8388608, 32768);
        pDisplay->CreateDevice();
    }
#endif

    return CD3DTest::Create(pDisplay);
}

bool PointTex::GeneratePointList(void) {
	int		iSegments = 250;
    float   fStart = 0.75f;
	int		i, j;
	float	fX = -fStart;
	float	fY = -fStart;
	float	fXInc = (2.f * fStart) / float(iSegments);
	float	fYInc = (2.f * fStart) / float(iSegments);
    D3DVERTEX* pr;

	m_dwVertices = iSegments * iSegments;
    if (GetStartupContext() & TSTART_STRESS) {
        m_pd3dr = CreateVertexBuffer(m_pDevice, NULL, m_dwVertices * sizeof(D3DVERTEX), 0, D3DFVF_VERTEX);
        if (!m_pd3dr) { 
            return false;
        }

        m_pd3dr->Lock(0, 0, (LPBYTE*)&pr, 0);
    }
    else {
	    m_pVertices = new D3DVERTEX [ m_dwVertices ];
	    if (m_pVertices == NULL)
		    return false;
        pr = m_pVertices;        
    }

	for (i = 0; i < iSegments; i++) 
    {
    	for (j = 0; j < iSegments; j++) 
        {
            int k = i*iSegments + j;
		    pr[k] = cD3DVERTEX(cD3DVECTOR(fX, fY, 0.f), Normalize(cD3DVECTOR(0.f, 1.f, 1.f)), 0.f, 0.f);

            pr[k].tu = (pr[k].x + fStart) / (2.f * fStart);
            pr[k].tv = (pr[k].y + fStart) / (2.f * fStart);

    		fX += fXInc;
        }
        fY += fYInc;
        fX = -fStart;
	}

    if (GetStartupContext() & TSTART_STRESS) {
        m_pd3dr->Unlock();
    }

	return true;
}



UINT PointTex::TestInitialize(void)
{
//	CImageLoader    Loader;
//	CImageData*		pImage = NULL;

    SetTestRange(1, 10);

    // Get number of supported texture stages with textures
    DWORD   dwTexCount = m_d3dcaps.MaxSimultaneousTextures;

    // Generate point vertices
    if (!GeneratePointList())
    {
        WriteToLog("PointTex: not enough memory to generate point list.\n");
        return D3DTESTINIT_ABORT;
    }

    // Loading textures
    static DWORD tex[8][8] = 
    {
        {0x00000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff},
        {0x00ffffff, 0x000000ff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff},
        {0x00ffffff, 0x00ffffff, 0x0000ff00, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff},
        {0x00ffffff, 0x00ffffff, 0x00ffffff, 0x0000ffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff},
        {0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ff0000, 0x00ffffff, 0x00ffffff, 0x00ffffff},
        {0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ff00ff, 0x00ffffff, 0x00ffffff},
        {0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffff00, 0x00ffffff},
        {0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00f0f0f0}
    };

    for (int i = 0; i < (int)dwTexCount; i++) {
		char fbuf[10];
		sprintf(fbuf, "tex%1d.bmp", i);
//		pImage = Loader.LoadStripes(256, 256, 8, tex[i], true);
//        if (NULL == pImage)
//        {
//            WriteToLog("PointTex: failed to load %s image\n", fbuf);
//            return D3DTESTINIT_ABORT;
//        }
		m_pTextureArray[i] = CreateStripedTexture(m_pDevice, 256,256,8,tex[i],true);
//		RELEASE(pImage);
        if (NULL == m_pTextureArray[i]) 
        {
			WriteToLog("PointTex: failed to create texture from image\n");
            return D3DTESTINIT_ABORT;
        }

		SetTexture(i, m_pTextureArray[i]);
        SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, (DWORD)0);
		SetTextureStageState(i, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
		if (i) 
		{
			SetTextureStageState(i, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);
			SetTextureStageState(i, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE);
		}
		else
		{
			SetTextureStageState(i, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
			SetTextureStageState(i, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
		}

    }

	return D3DTESTINIT_RUN;
}


bool PointTex::ExecuteTest(UINT iTest)
{
    m_dwCurrentTestNumber = (DWORD)iTest;

    BeginTestCase("");
    return true;
}

void PointTex::SceneRefresh(void) 
{
// ##REVIEW: The point pixels dim on the second test.  If scene refresh code is moved into ExecuteTest,
// a ClearFrame and Present are added, and false is returned from ExecuteTest, then things appear
// correctly.  Investigate what in CD3DTest::Render is causing the problem.
    if (BeginScene())
    {
        if (!(GetStartupContext() & TSTART_STRESS)) {
            RenderPrimitive(D3DPT_POINTLIST, D3DFVF_VERTEX, m_pVertices, m_dwVertices, NULL, 0, 0);
        }
        else {
            m_pDevice->SetStreamSource(0, m_pd3dr, sizeof(D3DVERTEX));
            m_pDevice->SetVertexShader(D3DFVF_VERTEX);
            m_pDevice->DrawPrimitive(D3DPT_POINTLIST, 0, m_dwVertices);
        }
        EndScene();
    }
}

bool PointTex::ProcessFrame(void)
{
	bool    bResult;

    bResult = GetCompareResult(0.15f);

    if (!bResult) {
		WriteToLog("$rPointTex: Test fails on frame %d\n", m_dwCurrentTestNumber);
		Fail();
		m_dwNumFailed++;
    }
	else {
		Pass();
	}
	EndTestCase();

	return true;
}

bool PointTex::TestTerminate(void)
{
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
    for (int i = 0; i < 8; i++)
    {
        ReleaseTexture(m_pTextureArray[i]);
        m_pTextureArray[i] = NULL;
    }

    if (m_pd3dr) {
        ReleaseVertexBuffer(m_pd3dr);
        m_pd3dr = NULL;
        m_dwVertices = 0;
    }

    if (m_pVertices)
    {
        delete [] m_pVertices;
        m_pVertices = NULL;
        m_dwVertices = 0;
    }
	return true;
}
