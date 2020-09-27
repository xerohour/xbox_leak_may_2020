//////////////////////////////////////////////////////////////////////
// TexGen.cpp: implementation of the CTexGen class.
//////////////////////////////////////////////////////////////////////

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Cubemap.h"
#include "Matrix.h"

CTexGen::CTexGen()
{
	m_szTestName=TEXT("Cubic Texture Generation");
	m_szCommandKey=TEXT("TexGen");

	m_flDegInc=30.0f;									// Set degree increase per test
    bTexGen = true;
}

void CTexGen::CommandLineHelp()
{
//    WriteCommandLineHelp("$y-YAWDEGINC #\t$wSets the yaw degree increment (default=30.0)");
}

bool CTexGen::SetDefaultMatrices(void)
{
    CMatrix Matrix;

    if (!CCubemap::SetDefaultMatrices())
        return false;

    // Translate sphere to min depth
    Matrix.Identity();
	Matrix.Translate(cD3DVECTOR(0.0f,0.0f,m_flZDepthMin));		
	if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix.m_Matrix))
	{
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
}

UINT CTexGen::TestInitialize(void)
{
//	CImageLoader Image;
	DWORD dwColors[4];
	UINT Result;
	char szBuffer[256];
    bool bFormats = true;

//	if (ReadString("YAWDEGINC",szBuffer,false))
//		m_flDegInc=atof(szBuffer);

    // Setup the Test range
	SetTestRange(1, (int)(360.0f/m_flDegInc) * 3);

    // Check if cubemaps are available	
	Result = CCubemap::TestInitialize();					

	if (Result != D3DTESTINIT_RUN)			
		return Result;
 
	// Check for the TexGen cap
	DWORD dwVertexProcessingCaps = m_d3dcaps.VertexProcessingCaps;

    if (!(dwVertexProcessingCaps & D3DVTXPCAPS_TEXGEN))
    {
		WriteToLog("Device capability not supported: VertexProcessing TexGen.\n");
		return D3DTESTINIT_SKIPALL;
    }

    // Check if any cube formats were available
	if (!bFormats) 
    {
		WriteToLog("No cubemap formats found.\n");
		return D3DTESTINIT_SKIPALL;
    }

    dwColors[0]=0xFF0000FF;
	dwColors[1]=0xFF00FF00;
	dwColors[2]=0xFF00FFFF;
	dwColors[3]=0xFFFF0000;
//	pImage[0]=Image.LoadGradient(m_dTextureWidthMax,m_dTextureHeightMax,dwColors);
    memcpy(&dwGradient[0][0], &dwColors[0], 4 * sizeof(DWORD));
	dwColors[0]=0xFFFF00FF;
	dwColors[1]=0xFFFFFF00;
	dwColors[2]=0xFFFFFFFF;
	dwColors[3]=0xFF0000FF;
//	pImage[1]=Image.LoadGradient(m_dTextureWidthMax,m_dTextureHeightMax,dwColors);
    memcpy(&dwGradient[1][0], &dwColors[0], 4 * sizeof(DWORD));
	dwColors[0]=0xFF00FF00;
	dwColors[1]=0xFF00FFFF;
	dwColors[2]=0xFFFF0000;
	dwColors[3]=0xFFFF00FF;
//	pImage[2]=Image.LoadGradient(m_dTextureWidthMax,m_dTextureHeightMax,dwColors);
    memcpy(&dwGradient[2][0], &dwColors[0], 4 * sizeof(DWORD));
	dwColors[0]=0xFFFFFF00;
	dwColors[1]=0xFFFFFFFF;
	dwColors[2]=0xFF0000FF;
	dwColors[3]=0xFF00FF00;
//	pImage[3]=Image.LoadGradient(m_dTextureWidthMax,m_dTextureHeightMax,dwColors);
    memcpy(&dwGradient[3][0], &dwColors[0], 4 * sizeof(DWORD));
	dwColors[0]=0xFF00FFFF;
	dwColors[1]=0xFFFF0000;
	dwColors[2]=0xFFFF00FF;
	dwColors[3]=0xFFFFFF00;
//	pImage[4]=Image.LoadGradient(m_dTextureWidthMax,m_dTextureHeightMax,dwColors);
    memcpy(&dwGradient[4][0], &dwColors[0], 4 * sizeof(DWORD));
	dwColors[0]=0xFFFFFFFF;
	dwColors[1]=0xFF0000FF;
	dwColors[2]=0xFF00FF00;
	dwColors[3]=0xFF00FFFF;
//	pImage[5]=Image.LoadGradient(m_dTextureWidthMax,m_dTextureHeightMax,dwColors);
    memcpy(&dwGradient[5][0], &dwColors[0], 4 * sizeof(DWORD));

    return Result;
}

bool CTexGen::ExecuteTest(UINT uTestNum)
{
 	// Initialize some variables
	char szStr[100];
	CMatrix Matrix;
	int nTest = (uTestNum-1) % (int)(360.0f/m_flDegInc);
    int nCubeFormat;
	float flAngle = m_flDegInc * (float)nTest;				// Get angle for current test
	while (flAngle>=360.0f) { flAngle-=360.0f; }
    CTexture8* pd3dtTemp;
    CSurface8* pd3dsSrc, *pd3dsDst;
    UINT i;
    HRESULT hr;
	szStr[0] = '\0';

	// Cleanup previous texture
	SetTexture(0, NULL);
	ReleaseTexture(m_paTexture);
 	ReleaseTexture(m_paSysTexture);

	nCubeFormat = (uTestNum-1) % m_uCommonCubeFormats;

	// Create the cubic environment mapping texture
/*
    if (bVideo)
    	m_paTexture=CreateCubicTexture(m_dTextureWidthMax,m_dTextureHeightMax,1,CDDS_TEXTURE_VIDEO|CDDS_TEXTURE_ALLFACES);

    if (bManage)
    	m_paTexture=CreateCubicTexture(m_dTextureWidthMax,m_dTextureHeightMax,1,CDDS_TEXTURE_MANAGE|CDDS_TEXTURE_ALLFACES);

    if (bD3DManage)
    	m_paTexture=CreateCubicTexture(m_dTextureWidthMax,m_dTextureHeightMax,1,CDDS_TEXTURE_D3DMANAGE|CDDS_TEXTURE_ALLFACES);
*/
    m_paTexture = (CCubeTexture8*)CreateTexture(m_pDevice, m_dTextureWidthMax,m_dTextureHeightMax, m_fmtCommonCube[nCubeFormat], TTYPE_CUBEMAP);

	if (m_paTexture==NULL)
	{
		WriteToLog("CreateCubicTexture() failed with HResult = %X.\n",GetLastError());
  		TestFinish(D3DTESTFINISH_ABORT);
        return false;
	}

/*
    // Load the image data onto the texture
    if (m_dwVersion <= 0x0700)
    {
        if (bLock)
        {
            // Lock/Paint directly to vidmem method
	        for (int i=0;i<6;i++) 
            {
		        m_paTexture->m_pSrcSurface->GetCubicSurface(i)->SetImageData(pImage[i]);

       	        if (NULL != m_pRefTarget)
	    	        m_paTexture->m_pRefSurface->GetCubicSurface(i)->SetImageData(pImage[i]);
	        }
        }

        if (bBlt)
        {
    	    m_paSysTexture=CreateCubicTexture(m_dTextureWidthMax,m_dTextureHeightMax,1,CDDS_TEXTURE_SYSTEM|CDDS_TEXTURE_ALLFACES);

	        if (m_paSysTexture==NULL)
	        {
		        WriteToLog("Sys CreateCubicTexture() failed with HResult = %X.\n",GetLastError());
    		    TestFinish(D3DTESTFINISH_ABORT);
                return false;
	        }

	        for (int i=0;i<6;i++) 
            {
		        m_paSysTexture->m_pSrcSurface->GetCubicSurface(i)->SetImageData(pImage[i]);

       	        if (NULL != m_pRefTarget)
	    	        m_paSysTexture->m_pRefSurface->GetCubicSurface(i)->SetImageData(pImage[i]);
	        }

            // Blt cubemap from system surface to original surface
            if (!BltCubicTexture())
            {
    		    TestFinish(D3DTESTFINISH_ABORT);
                return false;
            }
        }
    }
    else // m_dwVersion >= 0x0800
    {
        // Need a system copy of the texture in order to lock
    	m_paSysTexture=CreateCubicTexture(m_dTextureWidthMax,m_dTextureHeightMax,1,CDDS_TEXTURE_SYSTEM|CDDS_TEXTURE_ALLFACES);

	    if (m_paSysTexture==NULL)
	    {
		    WriteToLog("Sys CreateCubicTexture() failed with HResult = %X.\n",GetLastError());
    		TestFinish(D3DTESTFINISH_ABORT);
            return false;
	    }

	    for (int i=0;i<6;i++) 
        {
		    m_paSysTexture->m_pSrcSurface->GetCubicSurface(i)->SetImageData(pImage[i]);

       	    if (NULL != m_pRefTarget)
	    	    m_paSysTexture->m_pRefSurface->GetCubicSurface(i)->SetImageData(pImage[i]);
	    }

        // Load cubemap from system surface to original surface
        if (!LoadCubicTexture8())
        {
    		TestFinish(D3DTESTFINISH_ABORT);
            return false;
        }
    }	
*/

    for (i = 0; i < 6; i++) {

        pd3dtTemp = CreateGradientTexture(m_pDevice, m_dTextureWidthMax, m_dTextureHeightMax, dwGradient[i], m_fmtCommonCube[nCubeFormat]);
        if (!pd3dtTemp) {
            return false;
        }

        hr = pd3dtTemp->GetSurfaceLevel(0, &pd3dsSrc);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
            ReleaseTexture(pd3dtTemp);
            return false;
        }

        hr = m_paTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)i, 0, &pd3dsDst);
        if (ResultFailed(hr, TEXT("IDirect3DCubeTexture8::GetCubeMapSurface"))) {
            pd3dsSrc->Release();
            ReleaseTexture(pd3dtTemp);
            return false;
        }

        hr = D3DXLoadSurfaceFromSurface(pd3dsDst->GetIDirect3DSurface8(), NULL, NULL, pd3dsSrc->GetIDirect3DSurface8(), NULL, NULL, D3DX_FILTER_POINT, 0);

        pd3dsDst->Release();
        pd3dsSrc->Release();
        ReleaseTexture(pd3dtTemp);

        if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
            return false;
        }
    }

    if (!SyncTexture(m_paTexture)) {
        return false;
    }

	SetTexture(0,m_paTexture);

    switch ((uTestNum-1) / (int)(360.0f/m_flDegInc))
    {
        case 0: SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,(DWORD)D3DTSS_TCI_CAMERASPACENORMAL);
            	sprintf(szStr,"%sCameraSpaceNormal, ",szStr);
                break;
        case 1: SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,(DWORD)D3DTSS_TCI_CAMERASPACEPOSITION);
            	sprintf(szStr,"%sCameraSpacePosition, ",szStr);
                break;
        case 2: SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,(DWORD)D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
            	sprintf(szStr,"%sCameraSpaceReflectionVector, ",szStr);
                break;
    }

    SetTextureStageState(0,D3DTSS_TEXTURETRANSFORMFLAGS,(DWORD)D3DTTFF_COUNT3);

    Matrix.Identity();											    // Load Identity matrix
	Matrix.RotateYaw(flAngle);									    // Rotate
	if (!SetTransform(D3DTRANSFORMSTATE_TEXTURE0,&Matrix.m_Matrix)) // Set texture matrix
	{
		WriteToLog("SetTransform(Texture0) failed with HResult = %X.\n",GetLastError());
		TestFinish(D3DTESTFINISH_ABORT);
        return false;
	}

    // Tell the log that we are starting
	sprintf(szStr,"%sYaw %.2f degrees",szStr,flAngle);

	BeginTestCase(szStr);

	return true;
}

bool CTexGen::TestTerminate()
{
    CCubemap::TestTerminate();

	// Cleanup image data
//    for (int i=0;i<6;i++)
//        RELEASE(pImage[i]);

	return true;
}
