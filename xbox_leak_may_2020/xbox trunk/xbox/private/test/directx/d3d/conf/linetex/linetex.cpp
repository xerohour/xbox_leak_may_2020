//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "LineTex.h"
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

    LineTex*        TestOne;
    BOOL            bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    TestOne = new LineTex();
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

LineTex::LineTex()
{
//    m_pMaterial = NULL;
//    m_pLight = NULL;
    for (int i = 0; i < 8; i++)
        m_pTextureArray[i] = NULL;

    m_pVertices = NULL;
    m_dwVertices = 0;

    m_dwCurrentTestNumber = 0;
    m_dwNumFailed = 0;

	m_szTestName = TEXT("LineTex");
	m_szCommandKey = TEXT("linetex");
}

LineTex::~LineTex()
{
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
    for (int i = 0; i < 8; i++)
    {
        ReleaseTexture(m_pTextureArray[i]);
    }

    if (m_pVertices)
    {
        delete [] m_pVertices;
        m_pVertices = NULL;
        m_dwVertices = 0;
    }
}



bool LineTex::GenerateLineList(void) {
	int		iSegments = 500;
	int		iCount;
	float	fX = -0.9f;
	float	fY = 0.f;
	float	fXInc = 1.8f / iSegments;

	m_dwVertices = 2 * iSegments;
	m_pVertices = new D3DVERTEX [ m_dwVertices ];
	if (m_pVertices == NULL)
		return false;

	for (iCount = 0; iCount < iSegments; iCount++) {
		m_pVertices[2*iCount+0] = cD3DVERTEX(cD3DVECTOR(fX, fY+0.9f, 0.f), Normalize(cD3DVECTOR(0.f, 1.f, 1.f)), 0.f, 0.f);
		m_pVertices[2*iCount+1] = cD3DVERTEX(cD3DVECTOR(fX, fY-0.9f, 0.f), Normalize(cD3DVECTOR(1.f, 0.f, 1.f)), 0.f, 0.f);

        m_pVertices[2*iCount+0].tu = (m_pVertices[2*iCount+0].x + 1.f) / 2.f;
        m_pVertices[2*iCount+0].tv = (m_pVertices[2*iCount+0].y + 1.f) / 2.f;

        m_pVertices[2*iCount+1].tu = (m_pVertices[2*iCount+1].x + 1.f) / 2.f;
        m_pVertices[2*iCount+1].tv = (m_pVertices[2*iCount+1].y + 1.f) / 2.f;

		fX += fXInc;
	}
	return true;
}



UINT LineTex::TestInitialize(void)
{
//	CImageLoader    Loader;
//	CImageData*		pImage = NULL;
    
    SetTestRange(1, 10);

    // Get number of supported texture stages with textures
    DWORD   dwTexCount = m_d3dcaps.MaxSimultaneousTextures;

    // Check that the driver supports line texturing
    if (m_dwVersion >= 0x0800)
    {
        if (NULL == (m_d3dcaps.TextureCaps != NULL)) // ##REVIEW: How is line texturing reported in D3DCAPS8?
        {
            WriteToLog("LineTex: driver does not support line texturing\n");
            return D3DTESTINIT_SKIPALL;
        }
    }
    else
    {
//        if (NULL == (m_d3dcaps.TextureBlendCaps & D3DPTBLENDCAPS_MODULATE))
//        {
//            WriteToLog("LineTex: driver does not support line texturing\n");
//            return D3DTESTINIT_SKIPALL;
//        }
    }
    
    // Generate line strip vertices
    if (!GenerateLineList())
    {
        WriteToLog("LineTex: not enough memory to generate line list.\n");
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
//            WriteToLog("LineTex: failed to load %s image\n", fbuf);
//            return D3DTESTINIT_ABORT;
//        }
//		m_pTextureArray[i] = CreateTexture(256,256,NULL,pImage);
//		RELEASE(pImage);
        m_pTextureArray[i] = (CTexture8*)CreateStripedTexture(m_pDevice, 256, 256, 8, tex[i], true);
        if (NULL == m_pTextureArray[i]) 
        {
			WriteToLog("LineTex: failed to create texture from %s image\n", fbuf);
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


bool LineTex::ExecuteTest(UINT iTest)
{
    m_dwCurrentTestNumber = (DWORD)iTest;

    BeginTestCase("");
    return true;
}

void LineTex::SceneRefresh(void) 
{
    if (BeginScene())
    {
        RenderPrimitive(D3DPT_LINELIST, D3DFVF_VERTEX, m_pVertices, m_dwVertices, NULL, 0, 0);
        EndScene();
    }
}

bool LineTex::ProcessFrame(void)
{
	bool    bResult;

    bResult = GetCompareResult(0.15f);

    if (!bResult) {
		WriteToLog("$rLineTex: Test fails on frame %d\n", m_dwCurrentTestNumber);
		Fail();
		m_dwNumFailed++;
    }
	else {
		Pass();
	}
	EndTestCase();

	return true;
}

bool LineTex::TestTerminate(void)
{
//    RELEASE(m_pMaterial);
//    RELEASE(m_pLight);
    for (int i = 0; i < 8; i++)
    {
        ReleaseTexture(m_pTextureArray[i]);
        m_pTextureArray[i] = NULL;
    }

    if (m_pVertices)
    {
        delete [] m_pVertices;
        m_pVertices = NULL;
        m_dwVertices = 0;
    }
	return true;
}
