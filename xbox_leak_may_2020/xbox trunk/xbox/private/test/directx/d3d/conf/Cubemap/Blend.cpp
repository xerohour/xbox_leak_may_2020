//////////////////////////////////////////////////////////////////////
// Blend.cpp: implementation of the CCubeBlend class.
//////////////////////////////////////////////////////////////////////

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Cubemap.h"
#include "Matrix.h"

#define MAX_ANGLES      6
#define MAX_MATERIALS   3
#define BLEND_MODES     5
    
CCubeBlend::CCubeBlend()
{
	m_szTestName=TEXT("Cubic Blend");
	m_szCommandKey=TEXT("Blend");
}

bool CCubeBlend::SetDefaultRenderStates(void)
{
	SetRenderState(D3DRENDERSTATE_AMBIENT,(DWORD)0xFFFFFFFF);

	// Set the appropriate alpha blending renderstates
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

    return true;
}

UINT CCubeBlend::TestInitialize(void)
{
//	CImageLoader Image, AlphaImage;
	UINT Result;
    bool bFormats = true;

	// Request all cube formats except bumpmap
//	if (!RequestCubeFormats(PF_RGB | PF_FOURCC | PF_LUMINANCE))
//		bFormats = false;

    // Setup the Test range
	SetTestRange(1, MAX_ANGLES * m_uCommonCubeFormats * MAX_MATERIALS * BLEND_MODES);

    // Check if cubemaps are available	
	Result = CCubemap::TestInitialize();					

	if (Result != D3DTESTINIT_RUN)			
		return Result;
 
    // Check if any cube formats were available
	if (!bFormats) 
    {
		WriteToLog("No cubemap formats found.\n");
		return D3DTESTINIT_SKIPALL;
    }

	// Setup the colors for the Stripes array
//	D3DCOLOR		Stripes[8];

	Stripes[0] = RGBA_MAKE(255,255,255,255);	// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);		// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);		// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);		// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);		// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);		// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);		// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);	// Grey

	// Create the Stripe image data
//    pImage = Image.LoadStripes(64,64,8,Stripes,false); 

    // Setup the colors for the AlphaStripes array
//	D3DCOLOR		AlphaStripes[8];

	AlphaStripes[0] = RGBA_MAKE(255,255,255,0);		// White
	AlphaStripes[1] = RGBA_MAKE(255,0,0,32);		// Red
	AlphaStripes[2] = RGBA_MAKE(0,255,0,64);		// Green
	AlphaStripes[3] = RGBA_MAKE(0,0,255,127);		// Blue
	AlphaStripes[4] = RGBA_MAKE(255,255,0,128);		// Yellow
	AlphaStripes[5] = RGBA_MAKE(255,0,255,192);		// Magenta
	AlphaStripes[6] = RGBA_MAKE(0,255,255,224);		// Cyan
	AlphaStripes[7] = RGBA_MAKE(128,128,128,255);	// Grey

	// Create the AlphaStripe image data
//    pAlphaImage = AlphaImage.LoadStripes(64,64,8,AlphaStripes,false); 

    return Result;
}

bool CCubeBlend::ExecuteTest(UINT uTestNum)
{
 	// Initialize some variables
	CMatrix Matrix;
//	CImageData *pTempImage;
	int nTest = (uTestNum-1) / m_uCommonCubeFormats;
    int nAngle = nTest % MAX_ANGLES;
	int nMaterial = nTest / MAX_ANGLES;
	int nBlendMode = nMaterial / MAX_MATERIALS;
    D3DCOLOR* pcStripes;
    CTexture8* pd3dtTemp;
	float flAngle = 135.0f * (float)nAngle;				// Get angle for current test
	while (flAngle>=360.0f) { flAngle-=360.0f; }
	szStr[0] = '\0';

	// Cleanup previous texture
	SetTexture(0, NULL);
	ReleaseTexture(m_paTexture);
    m_paTexture = NULL;
 	ReleaseTexture(m_paSysTexture);

	// Cycle the common cube formats
	nCubeFormat = (uTestNum-1) % m_uCommonCubeFormats;

    // Determine which image data to use
    if ((nMaterial % MAX_MATERIALS) == 0)
//        pTempImage = pAlphaImage;
        pcStripes = AlphaStripes;
    else
//        pTempImage = pImage;
        pcStripes = Stripes;

	// Create the cubic environment mapping texture
/*
    if (bVideo)
    	m_paTexture=CreateCubicTexture(pTempImage->GetWidth(),pTempImage->GetHeight(),1,CDDS_TEXTURE_VIDEO|CDDS_TEXTURE_ALLFACES);

    if (bManage)
    	m_paTexture=CreateCubicTexture(pTempImage->GetWidth(),pTempImage->GetHeight(),1,CDDS_TEXTURE_MANAGE|CDDS_TEXTURE_ALLFACES);

    if (bD3DManage)
    	m_paTexture=CreateCubicTexture(pTempImage->GetWidth(),pTempImage->GetHeight(),1,CDDS_TEXTURE_D3DMANAGE|CDDS_TEXTURE_ALLFACES);
*/
    pd3dtTemp = CreateStripedTexture(m_pDevice, 64, 64, 8, pcStripes, false, false, m_fmtCommonCube[nCubeFormat]);
    if (!pd3dtTemp) {
        return false;
    }

    m_paTexture = (CCubeTexture8*)CreateTexture(m_pDevice, 64, 64, m_fmtCommonCube[nCubeFormat], TTYPE_CUBEMAP);

	if (m_paTexture==NULL)
	{
		WriteToLog("CreateCubicTexture() failed with HResult = %X.\n",GetLastError());
  		TestFinish(D3DTESTFINISH_ABORT);
        ReleaseTexture(pd3dtTemp);
        return false;
	}

    // Load the image data onto the texture
    if (FAILED(CopyTexture(m_paTexture, NULL, pd3dtTemp, NULL, D3DX_FILTER_POINT))) {
        ReleaseTexture(m_paTexture);
        m_paTexture = NULL;
        ReleaseTexture(pd3dtTemp);
        return false;
    }

    ReleaseTexture(pd3dtTemp);
/*
    if (m_dwVersion <= 0x0700)
    {
        if (bLock)
        {
            // Lock/Paint directly to vidmem method
	        for (int i=0;i<6;i++) 
            {
		        m_paTexture->m_pSrcSurface->GetCubicSurface(i)->SetImageData(pTempImage);

       	        if (NULL != m_pRefTarget)
	    	        m_paTexture->m_pRefSurface->GetCubicSurface(i)->SetImageData(pTempImage);
	        }
        }

        if (bBlt)
        {
    	    m_paSysTexture=CreateCubicTexture(pTempImage->GetWidth(),pTempImage->GetHeight(),1,CDDS_TEXTURE_SYSTEM|CDDS_TEXTURE_ALLFACES);

	        if (m_paSysTexture==NULL)
	        {
		        WriteToLog("Sys CreateCubicTexture() failed with HResult = %X.\n",GetLastError());
    		    TestFinish(D3DTESTFINISH_ABORT);
                return false;
	        }

	        for (int i=0;i<6;i++) 
            {
		        m_paSysTexture->m_pSrcSurface->GetCubicSurface(i)->SetImageData(pTempImage);

       	        if (NULL != m_pRefTarget)
	    	        m_paSysTexture->m_pRefSurface->GetCubicSurface(i)->SetImageData(pTempImage);
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
    	m_paSysTexture=CreateCubicTexture(pTempImage->GetWidth(),pTempImage->GetHeight(),1,CDDS_TEXTURE_SYSTEM|CDDS_TEXTURE_ALLFACES);

	    if (m_paSysTexture==NULL)
	    {
		    WriteToLog("Sys CreateCubicTexture() failed with HResult = %X.\n",GetLastError());
    		TestFinish(D3DTESTFINISH_ABORT);
            return false;
	    }

	    for (int i=0;i<6;i++) 
        {
		    m_paSysTexture->m_pSrcSurface->GetCubicSurface(i)->SetImageData(pTempImage);

       	    if (NULL != m_pRefTarget)
	    	    m_paSysTexture->m_pRefSurface->GetCubicSurface(i)->SetImageData(pTempImage);
	    }

        // Load cubemap from system surface to original surface
        if (!LoadCubicTexture8())
        {
    		TestFinish(D3DTESTFINISH_ABORT);
            return false;
        }
    }	
*/
	SetTexture(0,m_paTexture);

    if (!SetupBlendMode(nBlendMode))
    {
		SkipTests(MAX_ANGLES * m_uCommonCubeFormats * MAX_MATERIALS);
        return false;
    }

    if (!ApplyMaterial((nMaterial % MAX_MATERIALS), (nBlendMode == 4)))
    {
  		TestFinish(D3DTESTFINISH_ABORT);
        return false;
    }

    Matrix.Identity();											// Load Identity matrix
	Matrix.Translate(cD3DVECTOR(0.0f,0.0f,m_flZDepthMin));		// Translate sphere to min depth
	Matrix.RotatePitch(flAngle);									// Rotate
	if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix.m_Matrix))// Set world matrix
	{
		WriteToLog("SetTransform(World) failed with HResult = %X.\n",GetLastError());
		TestFinish(D3DTESTFINISH_ABORT);
        return false;
	}

    // Tell the log that we are starting
    if (m_dwVersion <= 0x0700)
    {
	    sprintf(szStr,"%s%.2f degrees, Tex: ",szStr,flAngle);
//	    GetTexFmtDesc(&m_pCommonCubeFormats[m_pMode->nCubeFormat].ddpfPixelFormat,szStr+strlen(szStr),80-strlen(szStr));
    }
    else
#ifndef UNICODE
	    sprintf(szStr,"%s%.2f degrees, Tex: %s",szStr,flAngle,D3DFmtToString(m_fmtCommonCube[nCubeFormat]));
#else
	    sprintf(szStr,"%s%.2f degrees, Tex: %S",szStr,flAngle,D3DFmtToString(m_fmtCommonCube[nCubeFormat]));
#endif // UNICODE

	BeginTestCase(szStr);

	return true;
}

bool CCubeBlend::TestTerminate()
{
    CCubemap::TestTerminate();

	// Cleanup image data
//    RELEASE(pImage);
//    RELEASE(pAlphaImage);

	return true;
}

bool CCubeBlend::ApplyMaterial(int nMaterial, bool bBlack)
{
	D3DMATERIAL8 Material;
    HRESULT hr;

    // Release previous material
//	RELEASE(m_paMaterial);

//	m_paMaterial=CreateMaterial();
//	if (m_paMaterial==NULL)
//	{
//		WriteToLog("CreateMaterial() failed with HResult = %X.\n",GetLastError());
//        return false;
//	}

	ZeroMemory(&Material,sizeof(D3DMATERIAL8));
//	Material.dwSize=sizeof(D3DMATERIAL);

    switch (nMaterial)
    {
        case 0:
            {   // Black with alpha
                if (bBlack)
                {
                    Material.Diffuse.a=0.5f;
	                Material.Ambient.r=0.0f;
	                Material.Ambient.g=0.0f;
	                Material.Ambient.b=0.0f;
            	    sprintf(szStr,"%sBlack (alpha=0.5), ",szStr);
                }
                else // White with alpha
                {
                    Material.Diffuse.a=0.5f;
	                Material.Ambient.r=1.0f;
	                Material.Ambient.g=1.0f;
	                Material.Ambient.b=1.0f;
            	    sprintf(szStr,"%sWhite (alpha=0.5), ",szStr);
                }
            }
            break;
        case 1:
            {   // Black with no alpha
                if (bBlack)
                {
                    Material.Diffuse.a=1.0f;
	                Material.Ambient.r=0.0f;
	                Material.Ambient.g=0.0f;
	                Material.Ambient.b=0.0f;
            	    sprintf(szStr,"%sBlack (alpha=1.0), ",szStr);
                }
                else // White with no alpha
                {
                    Material.Diffuse.a=1.0f;
	                Material.Ambient.r=1.0f;
	                Material.Ambient.g=1.0f;
	                Material.Ambient.b=1.0f;
            	    sprintf(szStr,"%sWhite (alpha=1.0), ",szStr);
                }
            }
            break;
        case 2:
            {   // Red with no alpha
                Material.Diffuse.a=1.0f;
	            Material.Ambient.r=1.0f;
	            Material.Ambient.g=0.0f;
	            Material.Ambient.b=0.0f;
            	sprintf(szStr,"%sRed (alpha=1.0), ",szStr);
            }
            break;
    }

/*
	if (!m_paMaterial->SetMaterial(&Material))
	{
		WriteToLog("SetMaterial() failed with HResult = %X.\n",GetLastError());
        return false;
	}

	if (!SetMaterial(m_paMaterial))
	{
		WriteToLog("SetMaterial() failed with HResult = %X.\n",GetLastError());
        return false;
	}

    return true;
*/
    hr = m_pDevice->SetMaterial(&Material);
    return (hr == D3D_OK);
}

bool CCubeBlend::SetupBlendMode(int nMode)
{
    bool bResult = true;
    ARGBPIXELDESC pixd;

    GetARGBPixelDesc(m_fmtCommonCube[nCubeFormat], &pixd);

    switch (nMode)
    {
        case 0:
           	sprintf(szStr,"%sDecal, ",szStr);
            bResult = SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)D3DTBLEND_DECAL);

            if (!bResult)
    		    WriteToLog("\nValidateDevice() failed: TextureOp SelectArg1.");

            break;
        case 1:
           	sprintf(szStr,"%sModulate, ",szStr);
            if (pixd.dwAMask) {
                bResult = SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)D3DTBLEND_MODULATE_ALPHATEXTURE);
            }
            else {
                bResult = SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)D3DTBLEND_MODULATE);
            }

            if (!bResult)
    		    WriteToLog("\nValidateDevice() failed: TextureOp Modulate.");

            break;
        case 2:
           	sprintf(szStr,"%sDecalAlpha, ",szStr);
            bResult = SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)D3DTBLEND_DECALALPHA);

            if (!bResult)
    		    WriteToLog("\nValidateDevice() failed: TextureOp BlendTextureAlpha.");

            break;
        case 3:
           	sprintf(szStr,"%sModulateAlpha, ",szStr);
            bResult = SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)D3DTBLEND_MODULATEALPHA);

            if (!bResult)
    		    WriteToLog("\nValidateDevice() failed: TextureOp Modulate.");

            break;
        case 4:
           	sprintf(szStr,"%sAdd, ",szStr);
            bResult = SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, (DWORD)D3DTBLEND_ADD);

            if (!bResult)
    		    WriteToLog("\nValidateDevice() failed: TextureOp Add.");

            break;
    }

    return bResult;
}