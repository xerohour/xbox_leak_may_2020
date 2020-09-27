#include "d3dlocus.h"
#include "cd3dtest.h"
#include "Volume.h"
#include "Matrix.h"
//#include "cimageloader.h"

#define MAX_ANGLES      6
#define MAX_MATERIALS   3
#define BLEND_MODES     5
    
CVolBlend::CVolBlend()
{
	m_szTestName=TEXT("Volume Blend");
	m_szCommandKey=TEXT("Blend");
}

CVolBlend::~CVolBlend()
{
}

bool CVolBlend::SetDefaultRenderStates(void)
{
	SetRenderState(D3DRS_AMBIENT,(DWORD)0xFFFFFFFF);

	// Set the appropriate alpha blending renderstates
	SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
	SetRenderState(D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
	SetRenderState(D3DRS_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

    return true;
}

UINT CVolBlend::TestInitialize(void)
{
    int i;
    bool bFormats = true;
    
	// Request all volume formats except bumpmap
//	if (!RequestVolumeFormats(PF_RGB))// | PF_FOURCC | PF_LUMINANCE))
//		bFormats = false;

    // Setup the Test range
	SetTestRange(1, MAX_ANGLES * m_uCommonVolumeFormats * MAX_MATERIALS * BLEND_MODES);

 	// Make sure we are on at lease DX8
	if (m_dwVersion <= 0x0700)
	{
		WriteToLog("This test requires at least Dx8.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Check if Volumes are available	
	DWORD dwTextureCaps = m_d3dcaps.TextureCaps;

	if (!(dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP))
	{
		WriteToLog("Device capability not supported: Texture VolumeMap.\n");
		return D3DTESTINIT_SKIPALL;
	}
 
    // Check if any volume formats were available
	if (!bFormats) 
    {
		WriteToLog("No volume formats found.\n");
		return D3DTESTINIT_SKIPALL;
    }

    // Load sphere vertices
	if (!LoadSphere()) {
        return D3DTESTINIT_ABORT;
    }

    // Setup the colors for the Stripes array
//	D3DCOLOR		Stripes[8];

	Stripes[0] = 0xffffffff;    // White
	Stripes[1] = 0xffff0000;	// Red
	Stripes[2] = 0xff00ff00;	// Green
	Stripes[3] = 0xff0000ff;	// Blue
	Stripes[4] = 0xffffff00;	// Yellow
	Stripes[5] = 0xffff00ff;	// Magenta
	Stripes[6] = 0xff00ffff;	// Cyan
	Stripes[7] = 0xff808080;    // Grey

    // Setup volume depth
//    VolImage.SetDepth(8);

    // Create the stripe image data
//    CImageLoader    Image;

//    for (i=0; i<VolImage.GetDepth(); i++)
//    {
//        VolImage.GetData()[i] = Image.LoadStripes(64,64,1,&Stripes[i],false); 
//    }

    // Setup the colors for the AlphaStripes array
//	D3DCOLOR		AlphaStripes[8];

	AlphaStripes[0] = 0x00ffffff;    // White
	AlphaStripes[1] = 0x20ff0000;	// Red
	AlphaStripes[2] = 0x4000ff00;	// Green
	AlphaStripes[3] = 0x7f0000ff;	// Blue
	AlphaStripes[4] = 0x80ffff00;	// Yellow
	AlphaStripes[5] = 0xc0ff00ff;	// Magenta
	AlphaStripes[6] = 0xe000ffff;	// Cyan
	AlphaStripes[7] = 0xff808080;    // Grey

    // Setup volume depth
//    AlphaVolImage.SetDepth(8);

    // Create the AlphaStripe image data
//    CImageLoader    AlphaImage;

//    for (i=0; i<AlphaVolImage.GetDepth(); i++)
//    {
//        AlphaVolImage.GetData()[i] = AlphaImage.LoadStripes(64,64,1,&AlphaStripes[i],false); 
//    }

    return D3DTESTINIT_RUN;
}

bool CVolBlend::ExecuteTest(UINT uTestNum)
{
 	// Initialize some variables
	CMatrix Matrix;
//    CVolumeImage *pTempVolImage;
    D3DCOLOR* pcStripes;
    DWORD dwWidth, dwHeight, dwDepth;
    D3DVECTOR MinZ = {0.0f,0.0f,3.0f};
	int nTest = (uTestNum-1) / m_uCommonVolumeFormats;
    int nAngle = nTest % MAX_ANGLES;
	int nMaterial = nTest / MAX_ANGLES;
	int nBlendMode = nMaterial / MAX_MATERIALS;
	float flAngle = 135.0f * (float)nAngle;				// Get angle for current test
	while (flAngle>=360.0f) { flAngle-=360.0f; }
	szStr[0] = '\0';

	// Cleanup previous texture
	SetTexture(0, NULL);
	ReleaseTexture(pVolTexture);
    pVolTexture = NULL;

	// Cycle the common volume formats
	nVolumeFormat = (uTestNum-1) % m_uCommonVolumeFormats;

    if (!SetupBlendMode(nBlendMode))
    {
		SkipTests(MAX_ANGLES * m_uCommonVolumeFormats * MAX_MATERIALS);
        return false;
    }

    if (!ApplyMaterial((nMaterial % MAX_MATERIALS), (nBlendMode == 4)))
    {
  		TestFinish(D3DTESTFINISH_ABORT);
        return false;
    }

    sprintf(szStr,"%s%.2f degrees, Tex: %s",szStr,flAngle,D3DFmtToString(m_fmtvCommon[nVolumeFormat]));

    // Tell the log that we are starting
    BeginTestCase(szStr);

    // Determine which image data to use
    if ((nMaterial % MAX_MATERIALS) == 0)
        pcStripes = AlphaStripes;
//        pTempVolImage = &AlphaVolImage;
    else
        pcStripes = Stripes;
//        pTempVolImage = &VolImage;

//    dwWidth = pTempVolImage->GetSlice(0)->GetWidth();
//    dwHeight = pTempVolImage->GetSlice(0)->GetHeight();
//    dwDepth = pTempVolImage->GetDepth();
    
    // Create the volume texture
//	pVolTexture = CreateVolumeTexture8(dwWidth,dwHeight,dwDepth,1,CDDS_TEXTURE_VIDEO,pTempVolImage);
    pVolTexture = CreateStripedVolumeTexture(m_pDevice, 64, 64, 8, 1, pcStripes, 1, false, false, m_fmtvCommon[nVolumeFormat]);

	// Tell log about CreateVolumeTexture failure
	if (NULL == pVolTexture) 
	{
		WriteToLog("CreateVolumeTexture() failed with HResult = %X.\n",GetLastError());
        UpdateStatus(false);
		return false;
	}

    // Set the valid texture
	SetTexture(0, pVolTexture);

    // Setup the world matrix for the rotating sphere
    Matrix.Identity();										
	Matrix.Translate(MinZ);		
	Matrix.RotatePitch(flAngle);								
	if (!SetTransform(D3DTS_WORLD,&Matrix.m_Matrix))
	{
        // Tell the logfile about the SetTransform failure
		WriteToLog("SetTransform(World) failed with HResult = %X.\n",GetLastError());
        UpdateStatus(false);
        return false;
	}

	return true;
}

bool CVolBlend::TestTerminate()
{
    int i;

    CVolumeTest::TestTerminate();

	// Cleanup image data
//    for (i=0; i<VolImage.GetDepth(); i++)
//    {
//        RELEASE(VolImage.GetData()[i]); 
//    }

	// Cleanup alpha image data
//    for (i=0; i<AlphaVolImage.GetDepth(); i++)
//    {
//        RELEASE(AlphaVolImage.GetData()[i]); 
//    }

	return true;
}

bool CVolBlend::ApplyMaterial(int nMaterial, bool bBlack)
{
	D3DMATERIAL8 Material;
    HRESULT      hr;

    // Release previous material
//	RELEASE(pMaterial);

//	pMaterial = CreateMaterial();
//	if (pMaterial == NULL)
//	{
//		WriteToLog("CreateMaterial() failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
//        return false;
//	}

	ZeroMemory(&Material,sizeof(D3DMATERIAL8));

    switch (nMaterial)
    {
        case 0:
            {   // Black with alpha
                if (bBlack)
                {
                    Material.Diffuse.a = 0.5f;
	                Material.Ambient.r = 0.0f;
	                Material.Ambient.g = 0.0f;
	                Material.Ambient.b = 0.0f;
            	    sprintf(szStr,"%sBlack (alpha=0.5), ",szStr);
                }
                else // White with alpha
                {
                    Material.Diffuse.a = 0.5f;
	                Material.Ambient.r = 1.0f;
	                Material.Ambient.g = 1.0f;
	                Material.Ambient.b = 1.0f;
            	    sprintf(szStr,"%sWhite (alpha=0.5), ",szStr);
                }
            }
            break;
        case 1:
            {   // Black with no alpha
                if (bBlack)
                {
                    Material.Diffuse.a = 1.0f;
	                Material.Ambient.r = 0.0f;
	                Material.Ambient.g = 0.0f;
	                Material.Ambient.b = 0.0f;
            	    sprintf(szStr,"%sBlack (alpha=1.0), ",szStr);
                }
                else // White with no alpha
                {
                    Material.Diffuse.a = 1.0f;
	                Material.Ambient.r = 1.0f;
	                Material.Ambient.g = 1.0f;
	                Material.Ambient.b = 1.0f;
            	    sprintf(szStr,"%sWhite (alpha=1.0), ",szStr);
                }
            }
            break;
        case 2:
            {   // Red with no alpha
                Material.Diffuse.a = 1.0f;
	            Material.Ambient.r = 1.0f;
	            Material.Ambient.g = 0.0f;
	            Material.Ambient.b = 0.0f;
            	sprintf(szStr,"%sRed (alpha=1.0), ",szStr);
            }
            break;
    }

//	if (!pMaterial->SetMaterial(&Material))
//	{
//		WriteToLog("SetMaterial() failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
  //      return false;
//	}

//	if (!SetMaterial(pMaterial))
//	{
//		WriteToLog("SetMaterial() failed with HResult = %s.\n",GetHResultString(GetLastError()).c_str());
//        return false;
//	}

    hr = m_pDevice->SetMaterial(&Material);
    return (hr == D3D_OK);

//    return true;
}

bool CVolBlend::SetupBlendMode(int nMode)
{
	DWORD	dwPasses;
	HRESULT	hResult;

    switch (nMode)
    {
        case 0:
           	sprintf(szStr,"%sDecal, ",szStr);
            SetTextureStageState(0,D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
            SetTextureStageState(0,D3DTSS_ALPHAARG1, (DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);

#ifndef UNDER_XBOX
		    if (!ValidateDevice(&hResult,&dwPasses))
		    {
    		    WriteToLog("\nValidateDevice() failed: TextureOp SelectArg1, HResult = %X.",hResult);
			    return false;
		    }
#endif // !UNDER_XBOX

            break;
        case 1:
           	sprintf(szStr,"%sModulate, ",szStr);
            SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_COLOROP,(DWORD)D3DTOP_MODULATE);
            SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_ALPHAOP,(DWORD)D3DTOP_SELECTARG1);

#ifndef UNDER_XBOX
		    if (!ValidateDevice(&hResult,&dwPasses))
		    {
    		    WriteToLog("\nValidateDevice() failed: TextureOp Modulate, HResult = %X.",hResult);
			    return false;
		    }
#endif // !UNDER_XBOX

            break;
        case 2:
           	sprintf(szStr,"%sDecalAlpha, ",szStr);
            SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_BLENDTEXTUREALPHA);
            SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_ALPHAOP,  (DWORD)D3DTOP_SELECTARG2);

#ifndef UNDER_XBOX
		    if (!ValidateDevice(&hResult,&dwPasses))
		    {
    		    WriteToLog("\nValidateDevice() failed: TextureOp BlendTextureAlpha, HResult = %X.",hResult);
			    return false;
		    }
#endif // !UNDER_XBOX

            break;
        case 3:
           	sprintf(szStr,"%sModulateAlpha, ",szStr);
            SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_MODULATE);
            SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_ALPHAOP,  (DWORD)D3DTOP_MODULATE);

#ifndef UNDER_XBOX
		    if (!ValidateDevice(&hResult,&dwPasses))
		    {
    		    WriteToLog("\nValidateDevice() failed: TextureOp Modulate, HResult = %X.",hResult);
			    return false;
		    }
#endif // !UNDER_XBOX

            break;
        case 4:
           	sprintf(szStr,"%sAdd, ",szStr);
            SetTextureStageState(0,D3DTSS_COLORARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_COLORARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_COLOROP,  (DWORD)D3DTOP_ADD);
            SetTextureStageState(0,D3DTSS_ALPHAARG1,(DWORD)D3DTA_TEXTURE);
            SetTextureStageState(0,D3DTSS_ALPHAARG2,(DWORD)D3DTA_CURRENT);
            SetTextureStageState(0,D3DTSS_ALPHAOP,  (DWORD)D3DTOP_SELECTARG2);

#ifndef UNDER_XBOX
		    if (!ValidateDevice(&hResult,&dwPasses))
		    {
    		    WriteToLog("\nValidateDevice() failed: TextureOp Add, HResult = %X.",hResult);
			    return false;
		    }
#endif // !UNDER_XBOX

            break;
    }

    return true;
}