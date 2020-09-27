//////////////////////////////////////////////////////////////////////
// Mipmaps.cpp: implementation of the CMipmaps class.
//////////////////////////////////////////////////////////////////////

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Cubemap.h"
#include "Matrix.h"
#define MIPMAP_LEVELS 5										// Mipmap levels to create

CMipmaps::CMipmaps()
{
	m_szTestName=TEXT("Cubic Mipmaps");
	m_szCommandKey=TEXT("Mipmaps");

	m_cTestMax=0;
	m_cLevelMax=0;
	m_flDegInc=45.0f;									// Set degree increase per test
}

/*
bool CMipmaps::AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize)
{
	// Test both texture loading methods
    if (IsConformance())
    {
        // Use CopyRects
        pMode->D3DMode.dwTexLoad = TEXLOAD_COPY_RECTS;
		CD3DTest::AddModeList(pMode,pData,dwDataSize);

        // Use UpdateTexture
        pMode->D3DMode.dwTexLoad = TEXLOAD_UPDATE_TEXTURE;
		CD3DTest::AddModeList(pMode,pData,dwDataSize);
    }
    else
    {
		// Let the framework handle stress mode
		CD3DTest::AddModeList(pMode,pData,dwDataSize);
	}

    return true;
}
*/

void CMipmaps::CommandLineHelp()
{
//    WriteCommandLineHelp("$y-MIPMAPDEGINC #\t$wSets the pitch degree increment (default=45.0)");
//    WriteCommandLineHelp("$y-MIPMAPLEVELS #\t$wSets the # of distances to test per mipmap level (default=1)");
}

UINT CMipmaps::TestInitialize(void)
{
//	CImageLoader Image;
	UINT Result;
	char szBuffer[256];
	int nLevels;
    bool bFormats = true;

//	if (ReadString("MIPMAPDEGINC",szBuffer,false))
//		m_flDegInc=atof(szBuffer);

	ReadInteger("MIPMAPLEVELS",1,&nLevels);
	if (nLevels<1)
		nLevels=1;

	// Request all cube formats except bumpmap
//	if (!RequestCubeFormats(PF_RGB | PF_FOURCC | PF_LUMINANCE))
//		bFormats = false;

    // Setup the Test range
	m_cLevelMax=nLevels * MIPMAP_LEVELS + 1;			// Total mipmap levels to test
	m_cTestMax=(int)(360.0f/m_flDegInc)*m_cLevelMax;	// Total tests
	SetTestRange(1, m_cTestMax * m_uCommonCubeFormats);

    // Check if cubemaps are available	
	Result = CCubemap::TestInitialize();					

	if (Result != D3DTESTINIT_RUN)			
		return Result;
 
    // On Dx8, check for MipCubeMap cap
    if (m_dwVersion >= 0x800)
    {
        if (!CheckMipCubeCap8())
    		return D3DTESTINIT_SKIPALL;
    }
    
    // Check if any cube formats were available
	if (!bFormats) 
    {
		WriteToLog("No cubemap formats found.\n");
		return D3DTESTINIT_SKIPALL;
    }

	// Setup the colors for the RedStripes array
//	D3DCOLOR		RedStripes[7];

	RedStripes[0] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[1] = RGBA_MAKE(255,255,255,255);	// White
	RedStripes[2] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[3] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[4] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[5] = RGBA_MAKE(255,255,255,255);	// White
	RedStripes[6] = RGBA_MAKE(255,0,0,255);		// Red

	// Setup the colors for the GreenStripes array
//	D3DCOLOR		GreenStripes[7];

	GreenStripes[0] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[1] = RGBA_MAKE(255,255,255,255);	// White
	GreenStripes[2] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[3] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[4] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[5] = RGBA_MAKE(255,255,255,255);	// White
	GreenStripes[6] = RGBA_MAKE(0,255,0,255);		// Green

	// Setup the colors for the BlueStripes array
//	D3DCOLOR		BlueStripes[7];

	BlueStripes[0] = RGBA_MAKE(0,0,255,255);	// Blue
	BlueStripes[1] = RGBA_MAKE(255,255,255,255);// White
	BlueStripes[2] = RGBA_MAKE(0,0,255,255);	// Blue
	BlueStripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	BlueStripes[4] = RGBA_MAKE(0,0,255,255);	// Blue
	BlueStripes[5] = RGBA_MAKE(255,255,255,255);// White
	BlueStripes[6] = RGBA_MAKE(0,0,255,255);	// Blue

	// Setup the colors for the YellowStripes array
//	D3DCOLOR		YellowStripes[7];

	YellowStripes[0] = RGBA_MAKE(255,255,0,255);  // Yellow
	YellowStripes[1] = RGBA_MAKE(255,255,255,255);// White
	YellowStripes[2] = RGBA_MAKE(255,255,0,255);  // Yellow
	YellowStripes[3] = RGBA_MAKE(255,255,0,255);  // Yellow
	YellowStripes[4] = RGBA_MAKE(255,255,0,255);  // Yellow
	YellowStripes[5] = RGBA_MAKE(255,255,255,255);// White
	YellowStripes[6] = RGBA_MAKE(255,255,0,255);  // Yellow

	// Setup the colors for the MagentaStripes array
//	D3DCOLOR		MagentaStripes[7];

	MagentaStripes[0] = RGBA_MAKE(255,0,255,255);  // Magenta
	MagentaStripes[1] = RGBA_MAKE(255,255,255,255);// White
	MagentaStripes[2] = RGBA_MAKE(255,0,255,255);  // Magenta
	MagentaStripes[3] = RGBA_MAKE(255,0,255,255);  // Magenta
	MagentaStripes[4] = RGBA_MAKE(255,0,255,255);  // Magenta
	MagentaStripes[5] = RGBA_MAKE(255,255,255,255);// White
	MagentaStripes[6] = RGBA_MAKE(255,0,255,255);  // Magenta

//    pImage[0]=Image.LoadStripes(m_dTextureWidthMax,m_dTextureHeightMax,7,RedStripes,false);
//	pImage[1]=Image.LoadStripes(m_dTextureWidthMax/2,m_dTextureHeightMax/2,7,GreenStripes,false);
//	pImage[2]=Image.LoadStripes(m_dTextureWidthMax/4,m_dTextureHeightMax/4,7,BlueStripes,false);
//	pImage[3]=Image.LoadStripes(m_dTextureWidthMax/8,m_dTextureHeightMax/8,7,YellowStripes,false);
//	pImage[4]=Image.LoadStripes(m_dTextureWidthMax/16,m_dTextureHeightMax/16,7,MagentaStripes,false);

	return Result;
}

bool CMipmaps::ExecuteTest(UINT uTestNum)
{
	char szStr[100];
	CMatrix Matrix;
	int iLevel;
	float flAngle,flZDist;
	int nTest = (uTestNum-1) / m_uCommonCubeFormats;
    int nCubeFormat;
    CTexture8* pd3dtTemp;
    CSurface8* pd3dsSrc, *pd3dsDst;
    UINT i, j;
    HRESULT hr;
	szStr[0] = '\0';

	// Get the Level
	iLevel=(int)((float)nTest/(float)m_cTestMax*(float)m_cLevelMax);

	// Get the Z distance of the current level
	flZDist=((float)iLevel*(m_flZDepthMax-m_flZDepthMin)/(float)(m_cLevelMax-1))+m_flZDepthMin;

	// Get the angle of the current test
	flAngle=m_flDegInc*(float)nTest;
	while (flAngle>=360.0f) { flAngle-=360.0f; }

    // Cleanup previous texture
	SetTexture(0, NULL);
	ReleaseTexture(m_paTexture);
 	ReleaseTexture(m_paSysTexture);

	// Cycle the common cube formats
	nCubeFormat = (uTestNum-1) % m_uCommonCubeFormats;

	// Create the cubic environment mapping texture
/*
    if (bVideo)
    	m_paTexture=CreateCubicTexture(m_dTextureWidthMax,m_dTextureHeightMax,MIPMAP_LEVELS,CDDS_TEXTURE_VIDEO|CDDS_TEXTURE_ALLFACES|CDDS_TEXTURE_MIPMAP);
    
    if (bManage)
    	m_paTexture=CreateCubicTexture(m_dTextureWidthMax,m_dTextureHeightMax,MIPMAP_LEVELS,CDDS_TEXTURE_MANAGE|CDDS_TEXTURE_ALLFACES|CDDS_TEXTURE_MIPMAP);
    
    if (bD3DManage)
    	m_paTexture=CreateCubicTexture(m_dTextureWidthMax,m_dTextureHeightMax,MIPMAP_LEVELS,CDDS_TEXTURE_D3DMANAGE|CDDS_TEXTURE_ALLFACES|CDDS_TEXTURE_MIPMAP);
*/

    m_paTexture = (CCubeTexture8*)CreateTexture(m_pDevice, m_dTextureWidthMax,m_dTextureHeightMax, m_fmtCommonCube[nCubeFormat], TTYPE_CUBEMAP | TTYPE_MIPLEVELS(MIPMAP_LEVELS));

    if (m_paTexture==NULL) 
	{
		WriteToLog("CreateCubicTexture() failed with HResult = %X.\n",GetLastError());
  		TestFinish(D3DTESTFINISH_ABORT);
        return false;
	}
/*
    if (m_dwVersion <= 0x0700)
    {
        if (bLock)
        {
            // Lock/Paint directly to vidmem method
	        for (int i=0;i<6;i++) 
            {
		        for (int j=0;j<m_paTexture->m_pSrcSurface->GetNumAttachedSurfaces();j++) 
                {
                    CDirectDrawSurface * pSurface = m_paTexture->m_pSrcSurface->GetCubicSurface(i)->GetAttachedSurface(j);
			        pSurface->SetImageData(pImage[j]);
                    RELEASE(pSurface);

           	        if (NULL != m_pRefTarget)
                    {
	    		        CDirectDrawSurface * pSurface = m_paTexture->m_pRefSurface->GetCubicSurface(i)->GetAttachedSurface(j);
                        pSurface->SetImageData(pImage[j]);
                        RELEASE(pSurface);
                    }
		        }
	        }
        }

        if (bBlt)
        {
    	    m_paSysTexture=CreateCubicTexture(m_dTextureWidthMax,m_dTextureHeightMax,MIPMAP_LEVELS,CDDS_TEXTURE_SYSTEM|CDDS_TEXTURE_ALLFACES|CDDS_TEXTURE_MIPMAP);

	        if (m_paSysTexture==NULL)
	        {
		        WriteToLog("Sys CreateCubicTexture() failed with HResult = %X.\n",GetLastError());
    		    TestFinish(D3DTESTFINISH_ABORT);
                return false;
	        }

	        for (int i=0;i<6;i++) 
            {
		        for (int j=0;j<m_paSysTexture->m_pSrcSurface->GetNumAttachedSurfaces();j++) 
                {
                    CDirectDrawSurface * pSurface = m_paSysTexture->m_pSrcSurface->GetCubicSurface(i)->GetAttachedSurface(j);
			        pSurface->SetImageData(pImage[j]);
                    RELEASE(pSurface);

           	        if (NULL != m_pRefTarget)
                    {
	    		        CDirectDrawSurface * pSurface = m_paSysTexture->m_pRefSurface->GetCubicSurface(i)->GetAttachedSurface(j);
                        pSurface->SetImageData(pImage[j]);
                        RELEASE(pSurface);
                    }
		        }
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
    	m_paSysTexture=CreateCubicTexture(m_dTextureWidthMax,m_dTextureHeightMax,MIPMAP_LEVELS,CDDS_TEXTURE_SYSTEM|CDDS_TEXTURE_ALLFACES);

	    if (m_paSysTexture==NULL)
	    {
		    WriteToLog("Sys CreateCubicTexture() failed with HResult = %X.\n",GetLastError());
    		TestFinish(D3DTESTFINISH_ABORT);
            return false;
	    }

        // Load the image data onto the system copy of the cubemap
	    for (int i=0;i<6;i++) 
        {
		    for (int j=0;j<m_paSysTexture->m_pSrcSurface->GetNumAttachedSurfaces();j++) 
            {
                CDirectDrawSurface * pSurface = m_paSysTexture->m_pSrcSurface->GetCubicSurface(i)->GetAttachedSurface(j);
			    pSurface->SetImageData(pImage[j]);
                RELEASE(pSurface);

           	    if (NULL != m_pRefTarget)
                {
	    		    CDirectDrawSurface * pSurface = m_paSysTexture->m_pRefSurface->GetCubicSurface(i)->GetAttachedSurface(j);
                    pSurface->SetImageData(pImage[j]);
                    RELEASE(pSurface);
                }
		    }
	    }

        // Load cubemap from system surface to original surface
        if (!LoadCubicTexture8())
        {
    		TestFinish(D3DTESTFINISH_ABORT);
            return false;
        }
    }	
*/

    for (i = 0; i < MIPMAP_LEVELS; i++) {

        switch (i) {
            case 0:
                pd3dtTemp = CreateStripedTexture(m_pDevice, m_dTextureWidthMax,m_dTextureHeightMax, 7, RedStripes, false, false, m_fmtCommonCube[nCubeFormat]);
                break;
            case 1:
                pd3dtTemp = CreateStripedTexture(m_pDevice, m_dTextureWidthMax/2,m_dTextureHeightMax/2,7,GreenStripes, false, false, m_fmtCommonCube[nCubeFormat]);
                break;
            case 2:
                pd3dtTemp = CreateStripedTexture(m_pDevice, m_dTextureWidthMax/4,m_dTextureHeightMax/4,7,BlueStripes, false, false, m_fmtCommonCube[nCubeFormat]);
                break;
            case 3:
                pd3dtTemp = CreateStripedTexture(m_pDevice, m_dTextureWidthMax/8,m_dTextureHeightMax/8,7,YellowStripes, false, false, m_fmtCommonCube[nCubeFormat]);
                break;
            case 4:
                pd3dtTemp = CreateStripedTexture(m_pDevice, m_dTextureWidthMax/16,m_dTextureHeightMax/16,7,MagentaStripes, false, false, m_fmtCommonCube[nCubeFormat]);
                break;
        }
        
        if (!pd3dtTemp) {
            return false;
        }

        hr = pd3dtTemp->GetSurfaceLevel(0, &pd3dsSrc);
        if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetSurfaceLevel"))) {
            ReleaseTexture(pd3dtTemp);
            return false;
        }

        for (j = 0; j < 6; j++) {

            hr = m_paTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)j, i, &pd3dsDst);
            if (ResultFailed(hr, TEXT("IDirect3DCubeTexture8::GetCubeMapSurface"))) {
                pd3dsSrc->Release();
                ReleaseTexture(pd3dtTemp);
                return false;
            }

            hr = D3DXLoadSurfaceFromSurface(pd3dsDst->GetIDirect3DSurface8(), NULL, NULL, pd3dsSrc->GetIDirect3DSurface8(), NULL, NULL, D3DX_FILTER_POINT, 0);

            pd3dsDst->Release();

            if (ResultFailed(hr, TEXT("D3DXLoadSurfaceFromSurface"))) {
                pd3dsSrc->Release();
                ReleaseTexture(pd3dtTemp);
                return false;
            }
        }

        pd3dsSrc->Release();
        ReleaseTexture(pd3dtTemp);

        if (!SyncTexture(m_paTexture)) {
            return false;
        }
    }

	SetTexture(0,m_paTexture);
    SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);

    Matrix.Identity();											// Load Identity matrix
	Matrix.Translate(cD3DVECTOR(0.0f,0.0f,flZDist));				// Translate sphere to min depth
	Matrix.RotatePitch(flAngle);								// Rotate
	if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix.m_Matrix))// Set world matrix
	{
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %X.\n",GetLastError());
		TestFinish(D3DTESTFINISH_ABORT);
        return false;
	}

	// Tell the log that we are starting
    if (m_dwVersion <= 0x0700)
    {
    	sprintf(szStr,"%sLevel %d, Pitch %.2f degrees, Tex: ",szStr,iLevel,flAngle);
//	    GetTexFmtDesc(&m_pCommonCubeFormats[m_pMode->nCubeFormat].ddpfPixelFormat,szStr+strlen(szStr),80-strlen(szStr));
    }
    else
#ifndef UNICODE
	    sprintf(szStr,"%sLevel %d, Pitch %.2f degrees, Tex: %s",szStr,iLevel,flAngle,D3DFmtToString(m_fmtCommonCube[nCubeFormat]));
#else
	    sprintf(szStr,"%sLevel %d, Pitch %.2f degrees, Tex: %S",szStr,iLevel,flAngle,D3DFmtToString(m_fmtCommonCube[nCubeFormat]));
#endif // UNICODE

	BeginTestCase(szStr);

	return true;
}

bool CMipmaps::TestTerminate()
{
    CCubemap::TestTerminate();

    // Cleanup image data
//    for (int i=0;i<MIPMAP_LEVELS;i++)
//        RELEASE(pImage[i]);

	return true;
}
