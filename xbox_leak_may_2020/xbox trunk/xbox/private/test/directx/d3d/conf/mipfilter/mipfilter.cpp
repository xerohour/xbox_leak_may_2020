#define STRICT
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "MipFilter.h"

// Global Test and App instance
//CD3DWindowFramework		App;

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

    CMipFilterTest* pMipTest;
    BOOL            bQuit = FALSE, bRet = TRUE;
    UINT            i;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    for (i = 0; i < 18 && !bQuit && bRet; i++) {

        // Create the scene
        switch (i) {
            case 0:
                pMipTest = new CPoint_MinPointTest();
                break;
            case 1:
                pMipTest = new CPoint_MinLinearTest();
                break;
            case 2:
                pMipTest = new CPoint_NonSquareWTest();
                break;
            case 3:
                pMipTest = new CPoint_NonSquareHTest();
                break;
            case 4:
                pMipTest = new CPoint_FullSquareTest();
                break;
            case 5:
                pMipTest = new CPoint_FullNonSquareWTest();
                break;
            case 6:
                pMipTest = new CPoint_FullNonSquareHTest();
                break;
            case 7:
                pMipTest = new CPoint_MaxMipLevelTest();
                break;
            case 8:
                pMipTest = new CPoint_LODBiasTest();
                break;
            case 9:
                pMipTest = new CLinear_MinPointTest();
                break;
            case 10:
                pMipTest = new CLinear_MinLinearTest();
                break;
            case 11:
                pMipTest = new CLinear_NonSquareWTest();
                break;
            case 12:
                pMipTest = new CLinear_NonSquareHTest();
                break;
            case 13:
                pMipTest = new CLinear_FullSquareTest();
                break;
            case 14:
                pMipTest = new CLinear_FullNonSquareWTest();
                break;
            case 15:
                pMipTest = new CLinear_FullNonSquareHTest();
                break;
            case 16:
                pMipTest = new CLinear_MaxMipLevelTest();
                break;
            case 17:
                pMipTest = new CLinear_LODBiasTest();
                break;
        }

        if (!pMipTest) {
            return FALSE;
        }

        // Initialize the scene
        if (!pMipTest->Create(pDisplay)) {
            pMipTest->Release();
            return FALSE;
        }

        bRet = pMipTest->Exhibit(pnExitCode);

        bQuit = pMipTest->AbortedExit();

        // Clean up the scene
        pMipTest->Release();
    }

    return bRet;
}

CMipFilterTest::CMipFilterTest()
{
	// Initialize some variables
	bMax = false;
	bFull = false;
	bBias = false;
	bMipLinear = false;
	bMinLinear = false;
	bNonSquareW = false;
	bNonSquareH = false;

	// Default size array values
	SizeArray[0] = 248.0f;
	SizeArray[1] =  32.0f;
	SizeArray[2] =  48.0f;
	SizeArray[3] =  56.0f;
	SizeArray[4] =  96.0f;
	SizeArray[5] = 104.0f;
	SizeArray[6] = 120.0f;
	SizeArray[7] = 192.0f;
	SizeArray[8] = 216.0f;

    pTexture = NULL;
    memset(pMipLevel, 0, 9 * sizeof(CTexture8*));
    bLoadWithUpdate = FALSE;
    m_bExit = FALSE;
}

CMipFilterTest::~CMipFilterTest()
{
}
/*
bool CMipFilterTest::AddModeList(PMODELIST pMode, PVOID pData, DWORD dwDataSize)
{
	// For Dx8, test both texture loading methods for full cases
    if (IsConformance() && (m_dwVersion >= 0x0800) && bFull)
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
bool CMipFilterTest::SetDefaultRenderStates(void)
{
	// Turn off Texture Perspective for all tests
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);

	// Setup our default Texture Stage State
	SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);

	if (!bMinLinear)
		SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
	else
		SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);

	if (!bMipLinear)
		SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
	else
		SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_LINEAR);
	
	return true;
}

UINT CMipFilterTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Request all texture formats except bumpmap
//	if (!RequestTextureFormats(PF_RGB | PF_FOURCC | PF_LUMINANCE))
//	{
//		WriteToLog("No texture formats found.\n");
//		return D3DTESTINIT_ABORT;
//	}

	// Setup the Test range
	if (bFull)
		SetTestRange(1, MAX_TESTS * 2 * m_uCommonTextureFormats);
	else if (bBias && !bMipLinear)
		SetTestRange(1, (MAX_TESTS - 2) * m_uCommonTextureFormats);
	else
		SetTestRange(1, MAX_TESTS * m_uCommonTextureFormats);

	// Check the caps we are interested in
	DWORD dwTextureFilterCaps = m_d3dcaps.TextureFilterCaps;
	DWORD dwTextureCaps = m_d3dcaps.TextureCaps;
	DWORD dwRasterCaps = m_d3dcaps.RasterCaps;
	DWORD dwMaxTextureAspectRatio = m_d3dcaps.MaxTextureAspectRatio;
    if (dwMaxTextureAspectRatio == 0) {
        Log(LOG_FAIL, TEXT("D3DCAPS.MaxTextureAspectRatio is 0"));
        dwMaxTextureAspectRatio = 1;
    }

	// Check the appropriate Mip Filter cap
	if (!bMipLinear && !(dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))
	{
		WriteToLog("Device capability not supported: (Mip) Texture Filter Point.\n");
		return D3DTESTINIT_SKIPALL;
	}
	else if (bMipLinear && !(dwTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
	{
		WriteToLog("Device capability not supported: (Mip) Texture Filter Linear.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Check the appropriate Min Filter cap
	if (!bMinLinear && !(dwTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))
	{
		WriteToLog("Device capability not supported: (Min) Texture Filter Point.\n");
		return D3DTESTINIT_SKIPALL;
	}
	else if (bMinLinear && !(dwTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))
	{
		WriteToLog("Device capability not supported: (Min) Texture Filter Linear.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Check the appropriate Mag Filter cap
	if (!(dwTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))
	{
		WriteToLog("Device capability not supported: (Mag) Texture Filter Point.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Check for ability to do non-square textures
	if ((bNonSquareW || bNonSquareH) && (dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY))
	{
		WriteToLog("Device does not supported non-square textures.\n");
		return D3DTESTINIT_SKIPALL;
	}

	// Check for LODBias cap
	if (bBias && !(dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS))
	{
		WriteToLog("Device capability not supported: Raster MipMapLODBias.\n");
		return D3DTESTINIT_SKIPALL;
	}

/*
	// Since there is no cap for MaxMipLevel, we need to check with ValidateDevice
	if (bMax)
	{
		DWORD	dwPasses, dwMaxMipLevel;
		HRESULT	hResult;

		SetTextureStageState(0,D3DTSS_MAXMIPLEVEL,(DWORD)8);

		GetTextureStageState(0,D3DTSS_MAXMIPLEVEL,&dwMaxMipLevel,NULL,CD3D_SRC);

		if (!ValidateDevice(&hResult,&dwPasses))
		{
			WriteToLog("ValidateDevice(MaxMipLevel=%d) failed with Passes = %d and HResult = %s.\n",dwMaxMipLevel,dwPasses,GetHResultString(hResult).c_str());
			return D3DTESTINIT_SKIPALL;
		}
	}
*/
	// Initialize the size array based on test class
	InitSize();

	// Initialize the MipImage pointer based on test class
	if (!InitMipImage(dwMaxTextureAspectRatio)) {
        return D3DTESTINIT_SKIPALL;
    }

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return D3DTESTINIT_RUN;
}

bool CMipFilterTest::ClearFrame(void)
{
	return Clear(RGBA_MAKE(0,0,0,255));
}

bool CMipFilterTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	int nTest = ((uTestNum-1) / m_uCommonTextureFormats) + 1;
	float fSize = SizeArray[((bFull) ? nTest % 18 : nTest % 9)];
	int nWidth = (int)fSize;
	int nHeight = (int)fSize;
	int nLevels = (bFull || bBias || bMax) ? -1 : 3;
	DWORD dwMipLevels;
    DWORD dwTType;
    int nTextureFormat;
	dwVertexCount = 0;
	msgString[0] = '\0';
 	szStatus[0]  = '\0';

	// Cleanup previous texture
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);
 
	// Let's build some triangles.
	DrawGrid(fSize, fSize);

	sprintf(msgString, "%sMesh:%dx%d, ",msgString,nWidth,nHeight);
	sprintf(szStatus, "%sMesh: %dx%d",szStatus,nWidth,nHeight);

	// Cycle the texture formats
	nTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;
 
	// Skip all YUV texture formats for the cases with small mip levels
	if (bFull || bBias || bMax)
	{
/*
		DWORD dwFourCC = m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat.dwFourCC;

		if ((dwFourCC == MAKEFOURCC('U','Y','V','Y')) ||
			(dwFourCC == MAKEFOURCC('Y','U','Y','2')))
		{
			char	szFourCC[5];

			szFourCC[0] = (char)(dwFourCC & 0xff);
			szFourCC[1] = (char)((dwFourCC >> 8) & 0xff);
			szFourCC[2] = (char)((dwFourCC >> 16) & 0xff);
			szFourCC[3] = (char)((dwFourCC >> 24) & 0xff);
			szFourCC[4] = '\0';

			WriteToLog("Ignoring this case with FourCC (%s) texture.\n",szFourCC);
			SkipTests(1);
			return false;
		}
*/
        switch (m_fmtCommon[nTextureFormat]) {
            case D3DFMT_UYVY:
            case D3DFMT_YUY2:
			    WriteToLog("Ignoring this case with YUV texture %d.\n",m_fmtCommon[nTextureFormat]);
			    SkipTests(1);
			    return false;
        }
	}

    // Get the top level Width & Height
//	nWidth = pMipImage[0]->GetWidth();
//	nHeight = pMipImage[0]->GetHeight();
    nWidth = nMWidth;
    nHeight = nMHeight;

    // Tell log about our Texture
//    if (m_dwVersion <= 0x0700)
//    {
//        sprintf(msgString, "%sTex: ",msgString);
//        GetTexFmtDesc(&m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat,msgString+strlen(msgString),80-strlen(msgString));
//    }
//    else
//    {
        if (!bLoadWithUpdate)
            sprintf(msgString, "%s(CopyRects) Tex: %s",msgString,D3DFmtToString(m_fmtCommon[nTextureFormat]));
        else
            sprintf(msgString, "%s(UpdateTex) Tex: %s",msgString,D3DFmtToString(m_fmtCommon[nTextureFormat]));
//    }

	sprintf(msgString, "%s, %dx%d",msgString,nWidth,nHeight);

    // Create the Mipmap texture
//    pTexture = CreateMipMapTexture(nWidth,nHeight,nLevels,CDDS_TEXTURE_VIDEO|CDDS_TEXTURE_MIPMAP,pMipImage);
    if (nLevels == -1) {
        dwTType = TTYPE_MIPMAP;
    }
    else {
        dwTType = TTYPE_MIPLEVELS(nLevels);
    }
    pTexture = (CTexture8*)CreateTexture(m_pDevice, nWidth, nHeight, m_fmtCommon[nTextureFormat], dwTType);

	// Get the real number of mip levels created
    if (NULL != pTexture)
	{
        CSurface8* pd3dsSrc, *pd3dsDst;
        UINT i;

//		dwMipLevels = pTexture->m_pSrcSurface->GetNumAttachedSurfaces();
        dwMipLevels = pTexture->GetLevelCount();

        for (i = 0; i < dwMipLevels; i++) {
            if (FAILED(pTexture->GetSurfaceLevel(i, &pd3dsDst))) {
                ReleaseTexture(pTexture);
                return false;
            }
            if (FAILED(pMipLevel[i]->GetSurfaceLevel(0, &pd3dsSrc))) {
                ReleaseTexture(pTexture);
                return false;
            }
            D3DXLoadSurfaceFromSurface(pd3dsDst->GetIDirect3DSurface8(), NULL, NULL, pd3dsSrc->GetIDirect3DSurface8(), NULL, NULL, D3DX_FILTER_POINT, 0);
            pd3dsDst->Release();
            pd3dsSrc->Release();
        }
        SyncTexture(pTexture);

		sprintf(msgString, "%s, Lvls:%d",msgString,dwMipLevels);
		sprintf(szStatus, "%s, Lvls: %d",szStatus,dwMipLevels);

	    // Setup the TSS for MipMapLODBias tests
	    if (bBias)
	    {
		    if (bMipLinear)
		    {
			    float fLodBias[] = {1.0f, 0.67f, 0.33f, 0.25f, 0.0f, -0.25f, -0.33f, -0.67f, -1.0f};

			    SetTextureStageState(0,D3DTSS_MIPMAPLODBIAS,F2DW(fLodBias[nTest-1]));
			    sprintf(msgString, "%s, Bias:%.2f",msgString,fLodBias[nTest-1]);
			    sprintf(szStatus, "%s, Bias: %.2f",szStatus,fLodBias[nTest-1]);
		    }
		    else
		    {
			    float fLodBias[] = {3.0f, 2.0f, 1.0f, 0.0f, -1.0f, -2.0f, -3.0f};

			    SetTextureStageState(0,D3DTSS_MIPMAPLODBIAS,F2DW(fLodBias[nTest-1]));
			    sprintf(msgString, "%s, Bias:%.2f",msgString,fLodBias[nTest-1]);
			    sprintf(szStatus, "%s, Bias: %.2f",szStatus,fLodBias[nTest-1]);
		    }		
	    }

        // Setup the TSS for MaxMipLevel tests
	    if (bMax)
	    {
		    SetTextureStageState(0,D3DTSS_MAXMIPLEVEL,(DWORD)(dwMipLevels - nTest));
		    sprintf(msgString, "%s, Max:%d",msgString,dwMipLevels - nTest);
		    sprintf(szStatus, "%s, Max: %d",szStatus,dwMipLevels - nTest);
	    }
    }

	// Tell the log that we are starting
	BeginTestCase(msgString);

    // Tell log about CreateTexture failure
	if (NULL == pTexture)
	{
/*
		if (m_dwVersion == 0x0600)
		{
			DWORD dwFourCC = m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat.dwFourCC;

			if ((dwFourCC == MAKEFOURCC('U','Y','V','Y')) ||
				(dwFourCC == MAKEFOURCC('Y','U','Y','2')))
			{
				char	szFourCC[5];

				szFourCC[0] = (char)(dwFourCC & 0xff);
				szFourCC[1] = (char)((dwFourCC >> 8) & 0xff);
				szFourCC[2] = (char)((dwFourCC >> 16) & 0xff);
				szFourCC[3] = (char)((dwFourCC >> 24) & 0xff);
				szFourCC[4] = '\0';

				WriteToLog("Can not create a FourCC (%s) texture on Dx6.\n",szFourCC);
				SkipTests(1);
				return false;
			}
		}
*/
		WriteToLog("CreateTexture() failed with HResult = %X.\n",GetLastError());

        // Tell the logfile how they did
        UpdateStatus(false);

		return false;
	}

    // Set the valid texture
	SetTexture(0,pTexture);

    return true;
}

void CMipFilterTest::SceneRefresh(void)
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

bool CMipFilterTest::ProcessFrame(void)
{
    bool	bResult;

     // Use the standard 15%
    bResult = GetCompareResult(0.15f, 0.78f, 0);

	// Tell the logfile how they did
    UpdateStatus(bResult);

    return bResult;
}

bool CMipFilterTest::TestTerminate(void)
{
	// Cleanup texture & image data
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);
    pTexture = NULL;

	for (int i=0; i < 9; i++) {
        ReleaseTexture(pMipLevel[i]);
        pMipLevel[i] = NULL;
//	    RELEASE(pMipImage[i]);
    }

	return true;
}

void CMipFilterTest::UpdateStatus(bool bResult)
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

	WriteStatus("$gTest Options$y",szStatus);
	WriteStatus("$gOverall Results",szBuffer);
}

void CMipFilterTest::InitSize(void)
{
	if (bFull)
	{
		// Init the Size array for all Full tests
		SizeArray[0] =  248.0f;
		SizeArray[1] =    1.0f;
		SizeArray[2] =    2.0f;
		SizeArray[3] =    3.0f;
		SizeArray[4] =    4.0f;
		SizeArray[5] =    6.0f; //  7.0f;
		SizeArray[6] =    7.0f; //  8.0f;
		SizeArray[7] =   12.0f; // 14.0f;
		SizeArray[8] =   14.0f; // 16.0f;
		SizeArray[9] =   24.0f; // 28.0f;
		SizeArray[10] =  30.0f; // 32.0f;
		SizeArray[11] =  48.0f;
		SizeArray[12] =  56.0f;
		SizeArray[13] =  96.0f;
		SizeArray[14] = 104.0f;
		SizeArray[15] = 120.0f;
		SizeArray[16] = 192.0f;
		SizeArray[17] = 216.0f;
	}
	else if (bMax)
	{
		// Init the Size array for MaxMipLevel tests
		for (int i=0; i < 9; i++)
			SizeArray[i] = 248.0f;
	}
	else if (bBias)
	{
		// Init the Size array for LODBias tests
		for (int i=0; i < 9; i++)
			SizeArray[i] = 30.0f; // 32.0f;
	}

	// else use default values
}

BOOL CMipFilterTest::InitMipImage(DWORD dwAspectRatio)
{
	int	nMinSize, nMipWidth, nMipHeight, i;
//	CImageLoader Image;

	if (bFull || bBias || bMax)
	{
		// Setup the color arrays
		D3DCOLOR White[1]	= {RGBA_MAKE(255,255,255,255)};
		D3DCOLOR Yellow[1]	= {RGBA_MAKE(255,255,0,255)};
		D3DCOLOR Magenta[1] = {RGBA_MAKE(255,0,255,255)};
		D3DCOLOR Red[1]		= {RGBA_MAKE(255,0,0,255)};
		D3DCOLOR Cyan[1]	= {RGBA_MAKE(0,255,255,255)};
		D3DCOLOR Green[1]	= {RGBA_MAKE(0,255,0,255)};
		D3DCOLOR Blue[1]	= {RGBA_MAKE(0,0,255,255)};
		D3DCOLOR Grey[1]	= {RGBA_MAKE(128,128,128,255)};
		D3DCOLOR Brown[1]	= {RGBA_MAKE(128,128,0,255)};

		// Determine Width and Height
		if (dwAspectRatio && ((256 / dwAspectRatio) > 16))
			nMinSize = 256 / dwAspectRatio;
		else
			nMinSize = 16;

		nMipWidth = (bNonSquareW) ? nMinSize : 256;
		nMipHeight = (bNonSquareH) ? nMinSize : 256;

        nMWidth = nMipWidth;
        nMHeight = nMipHeight;
		// Load images for the full level cases
/*
		pMipImage[0] = Image.LoadStripes(nMipWidth,nMipHeight,1,White,false); 
		pMipImage[1] = Image.LoadStripes(nMipWidth/2,nMipHeight/2,1,Yellow,false); 
		pMipImage[2] = Image.LoadStripes(nMipWidth/4,nMipHeight/4,1,Magenta,false); 
		pMipImage[3] = Image.LoadStripes(nMipWidth/8,nMipHeight/8,1,Red,false); 
		pMipImage[4] = Image.LoadStripes(nMipWidth/16,nMipHeight/16,1,Cyan,false); 
		pMipImage[5] = Image.LoadStripes(nMipWidth/32,nMipHeight/32,1,Green,false); 
		pMipImage[6] = Image.LoadStripes(nMipWidth/64,nMipHeight/64,1,Blue,false); 
		pMipImage[7] = Image.LoadStripes(nMipWidth/128,nMipHeight/128,1,Grey,false); 
		pMipImage[8] = Image.LoadStripes(nMipWidth/256,nMipHeight/256,1,Brown,false);
*/
		pMipLevel[0] = (CTexture8*)CreateStripedTexture(m_pDevice, nMipWidth,nMipHeight,1,White,false); 
		pMipLevel[1] = (CTexture8*)CreateStripedTexture(m_pDevice, nMipWidth/2,nMipHeight/2,1,Yellow,false); 
		pMipLevel[2] = (CTexture8*)CreateStripedTexture(m_pDevice, nMipWidth/4,nMipHeight/4,1,Magenta,false); 
		pMipLevel[3] = (CTexture8*)CreateStripedTexture(m_pDevice, nMipWidth/8,nMipHeight/8,1,Red,false); 
		pMipLevel[4] = (CTexture8*)CreateStripedTexture(m_pDevice, nMipWidth/16,nMipHeight/16,1,Cyan,false); 
		pMipLevel[5] = (CTexture8*)CreateStripedTexture(m_pDevice, nMipWidth/32,nMipHeight/32,1,Green,false); 
		pMipLevel[6] = (CTexture8*)CreateStripedTexture(m_pDevice, nMipWidth/64,nMipHeight/64,1,Blue,false); 
		pMipLevel[7] = (CTexture8*)CreateStripedTexture(m_pDevice, nMipWidth/128,nMipHeight/128,1,Grey,false); 
		pMipLevel[8] = (CTexture8*)CreateStripedTexture(m_pDevice, nMipWidth/256,nMipHeight/256,1,Brown,false);
        for (i = 0; i < 9; i++) {
            if (!pMipLevel[i]) {
                for (i = 0; i < 9; i++) {
                    if (pMipLevel[i]) {
                        ReleaseTexture(pMipLevel[i]);
                        pMipLevel[i] = NULL;
                    }
                }
                return FALSE;
            }
        }
	}
	else
	{
		// Setup the colors for the RedStripes array
		D3DCOLOR		RedStripes[8];

		RedStripes[0] = RGBA_MAKE(255,255,255,255);	// White
		RedStripes[1] = RGBA_MAKE(255,0,0,255);		// Red
		RedStripes[2] = RGBA_MAKE(255,255,255,255);	// White
		RedStripes[3] = RGBA_MAKE(255,0,0,255);		// Red
		RedStripes[4] = RGBA_MAKE(255,255,255,255);	// White
		RedStripes[5] = RGBA_MAKE(255,0,0,255);		// Red
		RedStripes[6] = RGBA_MAKE(255,255,255,255);	// White
		RedStripes[7] = RGBA_MAKE(255,0,0,255);		// Red

		// Setup the colors for the GreenStripes array
		D3DCOLOR		GreenStripes[8];

		GreenStripes[0] = RGBA_MAKE(255,255,255,255);	// White
		GreenStripes[1] = RGBA_MAKE(0,255,0,255);		// Green
		GreenStripes[2] = RGBA_MAKE(255,255,255,255);	// White
		GreenStripes[3] = RGBA_MAKE(0,255,0,255);		// Green
		GreenStripes[4] = RGBA_MAKE(255,255,255,255);	// White
		GreenStripes[5] = RGBA_MAKE(0,255,0,255);		// Green
		GreenStripes[6] = RGBA_MAKE(255,255,255,255);	// White
		GreenStripes[7] = RGBA_MAKE(0,255,0,255);		// Green

		// Setup the colors for the BlueStripes array
		D3DCOLOR		BlueStripes[8];

		BlueStripes[0] = RGBA_MAKE(255,255,255,255);// White
		BlueStripes[1] = RGBA_MAKE(0,0,255,255);	// Blue
		BlueStripes[2] = RGBA_MAKE(255,255,255,255);// White
		BlueStripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
		BlueStripes[4] = RGBA_MAKE(255,255,255,255);// White
		BlueStripes[5] = RGBA_MAKE(0,0,255,255);	// Blue
		BlueStripes[6] = RGBA_MAKE(255,255,255,255);// White
		BlueStripes[7] = RGBA_MAKE(0,0,255,255);	// Blue

		// Determine Width and Height
		if (dwAspectRatio && ((256 / dwAspectRatio) > 64))
			nMinSize = 256 / dwAspectRatio;
		else
			nMinSize = 64;

		nMipWidth = (bNonSquareW) ? nMinSize : 256;
		nMipHeight = (bNonSquareH) ? nMinSize : 256;

        nMWidth = nMipWidth;
        nMHeight = nMipHeight;

		// Load images for the three level cases
		pMipLevel[0] = (CTexture8*)CreateStripedTexture(m_pDevice, nMipWidth,nMipHeight,8,RedStripes,false); 
		pMipLevel[1] = (CTexture8*)CreateStripedTexture(m_pDevice, nMipWidth/2,nMipHeight/2,8,GreenStripes,false); 
		pMipLevel[2] = (CTexture8*)CreateStripedTexture(m_pDevice, nMipWidth/4,nMipHeight/4,8,BlueStripes,false); 
        for (i = 0; i < 3; i++) {
            if (!pMipLevel[i]) {
                for (i = 0; i < 3; i++) {
                    if (pMipLevel[i]) {
                        ReleaseTexture(pMipLevel[i]);
                        pMipLevel[i] = NULL;
                    }
                }
                return FALSE;
            }
        }
	}

    return TRUE;
}

//******************************************************************************
BOOL CMipFilterTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CMipFilterTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CMipFilterTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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
