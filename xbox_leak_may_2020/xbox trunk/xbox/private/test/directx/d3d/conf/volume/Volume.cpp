//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Volume.h"

//#define FVF_LVERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 | 0x00010000 )

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

    CVolumeTest*    pVolumeTest;
    BOOL            bQuit = FALSE, bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 5 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pVolumeTest = new CVolTexTest();    // Volume texture test cases
                break;
            case 1:
                pVolumeTest = new CMipVolTest();    // Volume mipmap texture test cases
                break;
            case 2:
                pVolumeTest = new CVolTexGen();     // Volume texture generation test cases
                break;
            case 3:
                pVolumeTest = new CVolFilter();     // Volume texture filtering test cases
                break;
            case 4:
                pVolumeTest = new CVolBlend();      // Volume texture blending test cases
                break;
        }

        if (!pVolumeTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pVolumeTest->Create(pDisplay)) {
            pVolumeTest->Release();
            return FALSE;
        }

        bRet = pVolumeTest->Exhibit(pnExitCode);

        bQuit = pVolumeTest->AbortedExit();

        // Clean up the scene
        pVolumeTest->Release();
    }

    return bRet;
}

CVolumeTest::CVolumeTest()
{
	// Initialize some variables
//	pMaterial = NULL;
	pTVertex = NULL;
	pTGVertex = NULL;
	pVolTexture = NULL;
	nVertexMax = 0;
    fAngle = 0.0f;
	bMipMap = false;
    bTexGen = false;

    // Disable textures
    m_ModeOptions.fTextures = false;

    // Enable volumes
    m_ModeOptions.fVolumes = true;

    m_bExit = FALSE;
}

CVolumeTest::~CVolumeTest()
{
}

bool CVolumeTest::SetDefaultMaterials(void)
{
	D3DMATERIAL8 Material;
    HRESULT hr;
/*
	pMaterial = CreateMaterial();
	if (pMaterial == NULL)
	{
		WriteToLog("CreateMaterial() failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
        return false;
	}
*/
	ZeroMemory(&Material,sizeof(D3DMATERIAL8));

    // No alpha
	Material.Diffuse.a = 1.0f;

    // White 
    Material.Ambient.r = 1.0f;
	Material.Ambient.g = 1.0f;
	Material.Ambient.b = 1.0f;
/*
	if (!pMaterial->SetMaterial(&Material))
	{
		WriteToLog("SetMaterial() failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
        return false;
	}

    // Now that the material data has been set,
    // re-set the material so the changes get picked up.
	if (!SetMaterial(pMaterial))
	{
		WriteToLog("SetMaterial() failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
        return false;
	}
*/

    hr = m_pDevice->SetMaterial(&Material);
    return (hr == D3D_OK);

	return true;
}

bool CVolumeTest::SetDefaultRenderStates(void)
{
	SetRenderState(D3DRS_AMBIENT,(DWORD)0xFFFFFFFF);

    if (bTexGen)
        SetRenderState(D3DRS_NORMALIZENORMALS,(DWORD)TRUE);

/*
    SetRenderState(D3DRS_DITHERENABLE, (DWORD)FALSE);
    SetRenderState(D3DRS_CLIPPING, (DWORD)FALSE);
    SetRenderState(D3DRS_CULLMODE, (DWORD)D3DCULL_NONE);
    SetRenderState(D3DRS_LIGHTING, (DWORD)FALSE);
//    SetRenderState(D3DRS_ZENABLE, (DWORD)FALSE);
//    SetRenderState(D3DRS_ZWRITEENABLE, (DWORD)FALSE);

  //  SetStreamSource(0, m_pVB, sizeof(LVERTEX));
//    SetVertexShader( FVF_LVERTEX );

    SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
    SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD)D3DTA_DIFFUSE);

    if (bMipMap)
    {
        SetTextureStageState(0, D3DTSS_MIPFILTER, (DWORD)TEXF_POINT);
//        SetTextureStageState(0, D3DTSS_MINFILTER, (DWORD)TEXF_LINEAR);
//        SetTextureStageState(0, D3DTSS_MAGFILTER, (DWORD)D3DTEXF_LINEAR);
    }
*/
	return true;
}

bool CVolumeTest::SetDefaultMatrices(void)
{
	D3DMATRIX Matrix;

	Matrix=ProjectionMatrix(pi/2.0f,320.0f/280.0f,1.0f,50.0f);
	if (!SetTransform(D3DTS_PROJECTION,&Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

	Matrix=IdentityMatrix();
	if (!SetTransform(D3DTS_WORLD,&Matrix))
	{
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

	Matrix=IdentityMatrix();
	if (!SetTransform(D3DTS_VIEW,&Matrix))
	{
		WriteToLog("SetTransform(ViewMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    if (bTexGen)
    {
	    Matrix=IdentityMatrix();
	    if (!SetTransform(D3DTS_TEXTURE0,&Matrix))
	    {
		    WriteToLog("SetTransform(Texture0) failed with HResult = %X.\n",GetLastError());
            return false;
	    }
    }

	return true;
}

//bool CVolumeTest::ClearFrame(void)
//{
//	return Clear(RGBA_MAKE(0,0,0,255));
//}
/*
bool CVolumeTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	int nTest = ((uTestNum-1) / m_uCommonVolumeFormats) + 1;
	float flAngle = 15.0f * (float)nTest;				// Get angle for current test
    DWORD dwWidth, dwHeight, dwMipLevels;
    DWORD dwDepth = 4;
	CMatrix Matrix;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Cleanup previous texture
	SetTexture(0, NULL);
	RELEASE(pVolTexture);

//    fAngle = (nTest-1) * (2.0f*3.141592654f / 90.0f);

	// Cycle the common volume formats
	m_pMode->nVolumeFormat = (uTestNum-1) % m_uCommonVolumeFormats;

    if (bMipMap)
    {
        dwWidth = pMipImage[0]->GetWidth();
        dwHeight = pMipImage[0]->GetHeight();
        dwMipLevels = 3;

	    // Create the mipmap texture
	    pVolTexture = CreateVolumeTexture8(dwWidth,dwHeight,dwDepth,dwMipLevels,CDDS_TEXTURE_VIDEO,pMipImage);
    }
    else
    {
        dwWidth = pImage->GetWidth();
        dwHeight = pImage->GetHeight();
        dwMipLevels = 1;

        // Create the texture
	    pVolTexture = CreateVolumeTexture8(dwWidth,dwHeight,dwDepth,dwMipLevels,CDDS_TEXTURE_VIDEO,&pImage);
    }

	// Tell the framework what texture to use
	if (NULL != pVolTexture) 
	{
        D3DVECTOR MinZ = {0.0f,0.0f,3.0f};

		SetTexture(0,pVolTexture);

        Matrix.Identity();									// Load Identity matrix
	    Matrix.Translate(MinZ);		                        // Translate sphere to min depth
	    Matrix.RotateYaw(flAngle);							// Rotate
	    if (!SetTransform(D3DTS_WORLD,&Matrix.m_Matrix))    // Set world matrix
	    {
		    WriteToLog("SetTransform(WorldMatrix) failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
		    TestFinish(D3DTESTFINISH_ABORT);
            return false;
	    }

        sprintf(msgString, "%sTex: ",msgString);
		GetTexFmtDesc(&m_pCommonVolumeFormats[m_pMode->nVolumeFormat].ddpfPixelFormat,msgString+strlen(msgString),60-strlen(msgString));
	}
	else
	{
		WriteToLog("CreateVolumeTexture() failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
		TestFinish(D3DTESTFINISH_ABORT);
		return false;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}
*/
void CVolumeTest::SceneRefresh(void)
{
	if (BeginScene())
	{
        // Rotate triangle about Y axis before drawing
/*        float fSin = (float) sin(fAngle);
        float fCos = (float) cos(fAngle);
        LVERTEX verts[3];
    
        memcpy( verts, VertexList, sizeof(LVERTEX)*dwVertexCount );
        
        for (int i = 0; i < dwVertexCount; i += 1)
        {
            verts[i].x = VertexList[i].x * fCos - VertexList[i].z * fSin;
            verts[i].y = VertexList[i].y;
            verts[i].z = VertexList[i].x * fSin + VertexList[i].z * fCos;

            verts[i].tu = VertexList[i].tu * fCos - VertexList[i].tw * fSin;
            verts[i].tv = VertexList[i].tv;
            verts[i].tw = VertexList[i].tu * fSin + VertexList[i].tw * fCos;

            // Push back the triangle by 0.5
            verts[i].z += 0.5f;
        }

        sprintf(szStatus, "%sZ: (%.2f, %.2f, %.2f), TW: (%.1f,%.1f,%.1f)",szStatus,verts[0].z,verts[1].z,verts[2].z,verts[0].tw,verts[1].tw,verts[2].tw);
     
        // Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,FVF_LVERTEX,verts,dwVertexCount,NULL,0,0);
*/
        if (bTexGen)
            RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_XYZ|D3DFVF_NORMAL,pTGVertex,nVertexMax,NULL,0,NULL);
        else     
            RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0),pTVertex,nVertexMax,NULL,0,NULL);

        EndScene();
	}	
}

bool CVolumeTest::ProcessFrame(void)
{
    bool	bResult;

	// Use the standard 15% comparison
	bResult = GetCompareResult(0.15f, 0.78f, 0);

	// Tell the logfile how they did
    UpdateStatus(bResult);

    return bResult;
}

bool CVolumeTest::TestTerminate(void)
{
    // Cleanup vertex data
	if (pTVertex!=NULL) 
		delete []pTVertex;

    if (pTGVertex!=NULL)
		delete []pTGVertex;

    // Cleanup material
//	RELEASE(pMaterial);

    // Cleanup texture
	SetTexture(0, NULL);
    ReleaseTexture(pVolTexture);

	return true;
}

void CVolumeTest::UpdateStatus(bool bResult)
{
	char	szBuffer[80];
	static	int nPass = 0;
	static  int nFail = 0;

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
}

//******************************************************************************
BOOL CVolumeTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CVolumeTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CVolumeTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
