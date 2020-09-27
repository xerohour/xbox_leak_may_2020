//////////////////////////////////////////////////////////////////////
// Filter.cpp: implementation of the CCubeFilter class.
//////////////////////////////////////////////////////////////////////

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Cubemap.h"
#include "Matrix.h"

#define LEVELS      3										// Mipmap levels to create
#define NUM_TESTS   8

CCubeFilter::CCubeFilter()
{
	m_szTestName=TEXT("Cubic Filter");
	m_szCommandKey=TEXT("Filter");

	m_flDegInc=45.0f;									// Set degree increase per test
}

void CCubeFilter::CommandLineHelp()
{
//    WriteCommandLineHelp("$y-FILTERDEGINC #\t$wSets the pitch degree increment (default=45.0)");
}

UINT CCubeFilter::TestInitialize(void)
{
//	CImageLoader Image;
	UINT Result;
	char szBuffer[256];
    bool bFormats = true;

//	if (ReadString("FILTERDEGINC",szBuffer,false))
//		m_flDegInc=atof(szBuffer);

    // Setup the Test range
	SetTestRange(1, (int)((360.0f / m_flDegInc) * LEVELS * NUM_TESTS));

    // Check if cubemaps are available	
	Result = CCubemap::TestInitialize();					

	if (Result != D3DTESTINIT_RUN)			
		return Result;
 
    // Check if any cube formats were available
//	if (!bFormats) 
//    {
//		WriteToLog("No cubemap formats found.\n");
//		return D3DTESTINIT_SKIPALL;
//    }

    // Setup the colors for the RedStripes array
//	D3DCOLOR		RedStripes[8];

	RedStripes[0] = RGBA_MAKE(255,255,255,255);	// White
	RedStripes[1] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[2] = RGBA_MAKE(255,255,255,255);	// White
	RedStripes[3] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[4] = RGBA_MAKE(255,255,255,255);	// White
	RedStripes[5] = RGBA_MAKE(255,0,0,255);		// Red
	RedStripes[6] = RGBA_MAKE(255,255,255,255);	// White
	RedStripes[7] = RGBA_MAKE(255,0,0,255);		// Red

	// Setup the colors for the GreenStripes array
//	D3DCOLOR		GreenStripes[8];

	GreenStripes[0] = RGBA_MAKE(255,255,255,255);	// White
	GreenStripes[1] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[2] = RGBA_MAKE(255,255,255,255);	// White
	GreenStripes[3] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[4] = RGBA_MAKE(255,255,255,255);	// White
	GreenStripes[5] = RGBA_MAKE(0,255,0,255);		// Green
	GreenStripes[6] = RGBA_MAKE(255,255,255,255);	// White
	GreenStripes[7] = RGBA_MAKE(0,255,0,255);		// Green

	// Setup the colors for the BlueStripes array
//	D3DCOLOR		BlueStripes[8];

	BlueStripes[0] = RGBA_MAKE(255,255,255,255);// White
	BlueStripes[1] = RGBA_MAKE(0,0,255,255);	// Blue
	BlueStripes[2] = RGBA_MAKE(255,255,255,255);// White
	BlueStripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	BlueStripes[4] = RGBA_MAKE(255,255,255,255);// White
	BlueStripes[5] = RGBA_MAKE(0,0,255,255);	// Blue
	BlueStripes[6] = RGBA_MAKE(255,255,255,255);// White
	BlueStripes[7] = RGBA_MAKE(0,0,255,255);	// Blue

	// Setup the colors for the YellowStripes array
//	D3DCOLOR		YellowStripes[8];

	YellowStripes[0] = RGBA_MAKE(255,255,255,255);// White
	YellowStripes[1] = RGBA_MAKE(255,255,0,255);  // Yellow
	YellowStripes[2] = RGBA_MAKE(255,255,255,255);// White
	YellowStripes[3] = RGBA_MAKE(255,255,0,255);  // Yellow
	YellowStripes[4] = RGBA_MAKE(255,255,255,255);// White
	YellowStripes[5] = RGBA_MAKE(255,255,0,255);  // Yellow
	YellowStripes[6] = RGBA_MAKE(255,255,255,255);// White
	YellowStripes[7] = RGBA_MAKE(255,255,0,255);  // Yellow

	// Setup the colors for the MagentaStripes array
//	D3DCOLOR		MagentaStripes[8];

	MagentaStripes[0] = RGBA_MAKE(255,255,255,255);// White
	MagentaStripes[1] = RGBA_MAKE(255,0,255,255);  // Magenta
	MagentaStripes[2] = RGBA_MAKE(255,255,255,255);// White
	MagentaStripes[3] = RGBA_MAKE(255,0,255,255);  // Magenta
	MagentaStripes[4] = RGBA_MAKE(255,255,255,255);// White
	MagentaStripes[5] = RGBA_MAKE(255,0,255,255);  // Magenta
	MagentaStripes[6] = RGBA_MAKE(255,255,255,255);// White
	MagentaStripes[7] = RGBA_MAKE(255,0,255,255);  // Magenta

//    pMinImage = Image.LoadStripes(16,16,8,RedStripes,false);
//	pMagImage = Image.LoadStripes(m_dTextureWidthMax,m_dTextureHeightMax,8,GreenStripes,false);
//	pMipImage[0] = Image.LoadStripes(m_dTextureWidthMax/2,m_dTextureHeightMax/2,8,BlueStripes,false);
//	pMipImage[1] = Image.LoadStripes(m_dTextureWidthMax/4,m_dTextureHeightMax/4,8,YellowStripes,false);
//	pMipImage[2] = Image.LoadStripes(m_dTextureWidthMax/8,m_dTextureHeightMax/8,8,MagentaStripes,false);

	return Result;
}

bool CCubeFilter::ExecuteTest(UINT uTestNum)
{
	CMatrix Matrix;
	int iLevel;
	float flAngle,flZDist;
	int nTest = (uTestNum - 1) % (LEVELS * NUM_TESTS);
	int nGroup = (uTestNum - 1) / (LEVELS * NUM_TESTS);
    CTexture8* pd3dtTemp;
	szStr[0] = '\0';

	// Get the Level
	iLevel = (int)((float)nTest / (360.0f / m_flDegInc));

	// Get the Z distance of the current level
	flZDist = ((float)iLevel * LEVELS) + m_flZDepthMin;

	// Get the angle of the current test
	flAngle=m_flDegInc*(float)nTest;
	while (flAngle>=360.0f) { flAngle-=360.0f; }

	nCubeFormat = (uTestNum-1) % m_uCommonCubeFormats;

    // Cleanup previous texture
	SetTexture(0, NULL);
	ReleaseTexture(m_paTexture);
 	ReleaseTexture(m_paSysTexture);

    // If Dx8, check MipCube cap for mipmap cases
    if ((nGroup >= 4) && (m_dwVersion >= 0x800))
    {
        if (!CheckMipCubeCap8())
        {
            TestFinish(D3DTESTFINISH_SKIPREMAINDER);
	    	return false;
        }
    }

    // Setup the filter mode
    if (!SetupFilterMode(nGroup))
    {
        SkipTests((UINT)(360.0f / m_flDegInc) * LEVELS);
	    return false;
    }

	// Create the cubic environment mapping texture
    if (nGroup <= 3)
    {
        DWORD dwWidth, dwHeight;

        // Using Min image data
        if (nGroup <= 1)
        {
            dwWidth = 16;//pMinImage->GetWidth();
            dwHeight = 16;//pMinImage->GetHeight();
        }
        else    // Using Mag image data
        {
            dwWidth = m_dTextureWidthMax;//pMagImage->GetWidth();
            dwHeight = m_dTextureWidthMax;//pMagImage->GetHeight();
        }

/*
        if (bVideo)
    	    m_paTexture=CreateCubicTexture(dwWidth,dwHeight,1,CDDS_TEXTURE_VIDEO|CDDS_TEXTURE_ALLFACES);

        if (bManage)
    	    m_paTexture=CreateCubicTexture(dwWidth,dwHeight,1,CDDS_TEXTURE_MANAGE|CDDS_TEXTURE_ALLFACES);

        if (bD3DManage)
    	    m_paTexture=CreateCubicTexture(dwWidth,dwHeight,1,CDDS_TEXTURE_D3DMANAGE|CDDS_TEXTURE_ALLFACES);
*/

        m_paTexture = (CCubeTexture8*)CreateTexture(m_pDevice, dwWidth, dwHeight, m_fmtCommonCube[nCubeFormat], TTYPE_CUBEMAP);

    	sprintf(szStr,"%sTexSize: %dx%d, ",szStr,dwWidth,dwHeight);
    }
    else
    {
        // Using Mip image data
/*
        if (bVideo)
    	    m_paTexture=CreateCubicTexture(pMipImage[0]->GetWidth(),pMipImage[0]->GetHeight(),LEVELS,CDDS_TEXTURE_VIDEO|CDDS_TEXTURE_ALLFACES|CDDS_TEXTURE_MIPMAP);

        if (bManage)
    	    m_paTexture=CreateCubicTexture(pMipImage[0]->GetWidth(),pMipImage[0]->GetHeight(),LEVELS,CDDS_TEXTURE_MANAGE|CDDS_TEXTURE_ALLFACES|CDDS_TEXTURE_MIPMAP);

        if (bD3DManage)
    	    m_paTexture=CreateCubicTexture(pMipImage[0]->GetWidth(),pMipImage[0]->GetHeight(),LEVELS,CDDS_TEXTURE_D3DMANAGE|CDDS_TEXTURE_ALLFACES|CDDS_TEXTURE_MIPMAP);
*/
        m_paTexture = (CCubeTexture8*)CreateTexture(m_pDevice, m_dTextureWidthMax/2, m_dTextureHeightMax/2, m_fmtCommonCube[nCubeFormat], TTYPE_CUBEMAP | TTYPE_MIPLEVELS(LEVELS));

        sprintf(szStr,"%sMipTexSize: %dx%d, ",szStr,(m_dTextureWidthMax/2)>>iLevel,(m_dTextureHeightMax/2)>>iLevel);
    }

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

                    if (nGroup <= 1)
    			        pSurface->SetImageData(pMinImage);
                    else if (nGroup <= 3)
			            pSurface->SetImageData(pMagImage);
                    else
			            pSurface->SetImageData(pMipImage[j]);

                    RELEASE(pSurface);

           	        if (NULL != m_pRefTarget)
                    {
	    		        CDirectDrawSurface * pSurface = m_paTexture->m_pRefSurface->GetCubicSurface(i)->GetAttachedSurface(j);

                        if (nGroup <= 1)
    			            pSurface->SetImageData(pMinImage);
                        else if (nGroup <= 3)
			                pSurface->SetImageData(pMagImage);
                        else
			                pSurface->SetImageData(pMipImage[j]);

                        RELEASE(pSurface);
                    }
		        }
	        }
        }

        if (bBlt)
        {
            // Need a system copy of the texture in order to lock
            if (nGroup <= 1)
            {
                // Using Min image data
    	        m_paSysTexture=CreateCubicTexture(pMinImage->GetWidth(),pMinImage->GetHeight(),1,CDDS_TEXTURE_SYSTEM|CDDS_TEXTURE_ALLFACES);
            }
            else if (nGroup <= 3)
            {
                // Using Mag image data
    	        m_paSysTexture=CreateCubicTexture(pMagImage->GetWidth(),pMagImage->GetHeight(),1,CDDS_TEXTURE_SYSTEM|CDDS_TEXTURE_ALLFACES);
            }
            else
            {
                // Using Mip image data
	            m_paSysTexture=CreateCubicTexture(pMipImage[0]->GetWidth(),pMipImage[0]->GetHeight(),LEVELS,CDDS_TEXTURE_SYSTEM|CDDS_TEXTURE_ALLFACES|CDDS_TEXTURE_MIPMAP);
            }

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

                    if (nGroup <= 1)
    			        pSurface->SetImageData(pMinImage);
                    else if (nGroup <= 3)
			            pSurface->SetImageData(pMagImage);
                    else
			            pSurface->SetImageData(pMipImage[j]);

                    RELEASE(pSurface);

           	        if (NULL != m_pRefTarget)
                    {
	    		        CDirectDrawSurface * pSurface = m_paSysTexture->m_pRefSurface->GetCubicSurface(i)->GetAttachedSurface(j);

                        if (nGroup <= 1)
    			            pSurface->SetImageData(pMinImage);
                        else if (nGroup <= 3)
			                pSurface->SetImageData(pMagImage);
                        else
			                pSurface->SetImageData(pMipImage[j]);

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
        if (nGroup <= 1)
        {
            // Using Min image data
    	    m_paSysTexture=CreateCubicTexture(pMinImage->GetWidth(),pMinImage->GetHeight(),1,CDDS_TEXTURE_SYSTEM|CDDS_TEXTURE_ALLFACES);
        }
        else if (nGroup <= 3)
        {
            // Using Mag image data
    	    m_paSysTexture=CreateCubicTexture(pMagImage->GetWidth(),pMagImage->GetHeight(),1,CDDS_TEXTURE_SYSTEM|CDDS_TEXTURE_ALLFACES);
        }
        else
        {
            // Using Mip image data
	        m_paSysTexture=CreateCubicTexture(pMipImage[0]->GetWidth(),pMipImage[0]->GetHeight(),LEVELS,CDDS_TEXTURE_SYSTEM|CDDS_TEXTURE_ALLFACES);
        }

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

                if (nGroup <= 1)
    			    pSurface->SetImageData(pMinImage);
                else if (nGroup <= 3)
			        pSurface->SetImageData(pMagImage);
                else
			        pSurface->SetImageData(pMipImage[j]);

                RELEASE(pSurface);

           	    if (NULL != m_pRefTarget)
                {
	    		    CDirectDrawSurface * pSurface = m_paSysTexture->m_pRefSurface->GetCubicSurface(i)->GetAttachedSurface(j);

                    if (nGroup <= 1)
    			        pSurface->SetImageData(pMinImage);
                    else if (nGroup <= 3)
			            pSurface->SetImageData(pMagImage);
                    else
			            pSurface->SetImageData(pMipImage[j]);

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

    if (nGroup <= 3) {
        if (nGroup <= 1) {
            pd3dtTemp = CreateStripedTexture(m_pDevice, 16, 16, 8, RedStripes, false, false, m_fmtCommonCube[nCubeFormat]);
        }
        else {
            pd3dtTemp = CreateStripedTexture(m_pDevice, m_dTextureWidthMax, m_dTextureHeightMax, 8, GreenStripes, false, false, m_fmtCommonCube[nCubeFormat]);
        }
        if (!pd3dtTemp) {
            return false;
        }
        if (FAILED(CopyTexture(m_paTexture, NULL, pd3dtTemp, NULL, D3DX_FILTER_POINT))) {
            ReleaseTexture(pd3dtTemp);
            return false;
        }
        ReleaseTexture(pd3dtTemp);
    }
    else {

        CSurface8* pd3dsSrc, *pd3dsDst;
        UINT i, j;
        HRESULT hr;

        for (i = 0; i < 3; i++) {
            switch (i) {
                case 0:
                    pd3dtTemp = CreateStripedTexture(m_pDevice, m_dTextureWidthMax/2,m_dTextureHeightMax/2, 8, BlueStripes, false, false, m_fmtCommonCube[nCubeFormat]);
                    break;
                case 1:
                    pd3dtTemp = CreateStripedTexture(m_pDevice, m_dTextureWidthMax/4,m_dTextureHeightMax/4, 8, YellowStripes, false, false, m_fmtCommonCube[nCubeFormat]);
                    break;
                case 2:
                    pd3dtTemp = CreateStripedTexture(m_pDevice, m_dTextureWidthMax/8,m_dTextureHeightMax/8, 8, MagentaStripes, false, false, m_fmtCommonCube[nCubeFormat]);
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
    }

	SetTexture(0,m_paTexture);

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
	sprintf(szStr,"%sLevel %d, Pitch %.2f degrees",szStr,iLevel,flAngle);

	BeginTestCase(szStr);

	return true;
}

bool CCubeFilter::TestTerminate()
{
    CCubemap::TestTerminate();

    // Cleanup image data
//    RELEASE(pMinImage);
//    RELEASE(pMagImage);

//    for (int i=0;i<LEVELS;i++)
//        RELEASE(pMipImage[i]);

	return true;
}

bool CCubeFilter::SetupFilterMode(int nMode)
{
    switch (nMode)
    {
        case 0:
        case 2:
            if (!CheckCubeFilterCaps8(D3DPTFILTERCAPS_MINFLINEAR, 0, 0))       
            {
                WriteToLog("\nDevice capability not supported: %sTexture Filter MinFLinear.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            if (!CheckCubeFilterCaps8(0, D3DPTFILTERCAPS_MAGFPOINT, 0))       
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MagFPoint.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
       	    sprintf(szStr,"%sFilter: LPN, ",szStr);
            break;
        case 1:
        case 3:
            if (!CheckCubeFilterCaps8(D3DPTFILTERCAPS_MINFPOINT, 0, 0))       
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MinFPoint.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            if (!CheckCubeFilterCaps8(0, D3DPTFILTERCAPS_MAGFLINEAR, 0))       
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MagFLinear.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
        	sprintf(szStr,"%sFilter: PLN, ",szStr);
            break;
        case 4:
            if (!CheckCubeFilterCaps8(D3DPTFILTERCAPS_MINFPOINT, 0, 0)) 
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MinFPoint.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            if (!CheckCubeFilterCaps8(0, D3DPTFILTERCAPS_MAGFPOINT, 0))       
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MagFPoint.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            if (!CheckCubeFilterCaps8(0, 0, D3DPTFILTERCAPS_MIPFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MipFPoint.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
        	sprintf(szStr,"%sFilter: PPP, ",szStr);
            break;
        case 5:
            if (!CheckCubeFilterCaps8(D3DPTFILTERCAPS_MINFLINEAR, 0, 0)) 
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MinFLinear.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            if (!CheckCubeFilterCaps8(0, D3DPTFILTERCAPS_MAGFLINEAR, 0))       
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MagFLinear.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            if (!CheckCubeFilterCaps8(0, 0, D3DPTFILTERCAPS_MIPFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MipFPoint.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
        	sprintf(szStr,"%sFilter: LLP, ",szStr);
            break;
        case 6:
            if (!CheckCubeFilterCaps8(D3DPTFILTERCAPS_MINFPOINT, 0, 0)) 
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MinFPoint.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            if (!CheckCubeFilterCaps8(0, D3DPTFILTERCAPS_MAGFPOINT, 0))       
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MagFPoint.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            if (!CheckCubeFilterCaps8(0, 0, D3DPTFILTERCAPS_MIPFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MipFLinear.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);
        	sprintf(szStr,"%sFilter: PPL, ",szStr);
            break;
        case 7:
            if (!CheckCubeFilterCaps8(D3DPTFILTERCAPS_MINFLINEAR, 0, 0)) 
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MinFLinear.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            if (!CheckCubeFilterCaps8(0, D3DPTFILTERCAPS_MAGFLINEAR, 0))       
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MagFLinear.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            if (!CheckCubeFilterCaps8(0, 0, D3DPTFILTERCAPS_MIPFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: %sTexture Filter MipFLinear.",(m_dwVersion >= 0x0800) ? "Cube " : "");
                return false;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);
        	sprintf(szStr,"%sFilter: LLL, ",szStr);
            break;
    }

    return true;
}
