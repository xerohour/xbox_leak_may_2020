#define STRICT
#define BURN_IN_HELL
//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Compress.h"

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

    CCompressTest*  pDXT;
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
                pDXT = new CDXT1Test();
                break;
            case 1:
                pDXT = new CDXT2Test();
                break;
            case 2:
                pDXT = new CDXT3Test();
                break;
            case 3:
                pDXT = new CDXT4Test();
                break;
            case 4:
                pDXT = new CDXT5Test();
                break;
        }

        if (!pDXT) {
            return FALSE;
        }

        // Initialize the scene
        if (!pDXT->Create(pDisplay)) {
            pDXT->Release();
            return FALSE;
        }

        bRet = pDXT->Exhibit(pnExitCode);

        bQuit = pDXT->AbortedExit();

        // Clean up the scene
        pDXT->Release();
    }

    return bRet;
}
 
CCompressTest::CCompressTest()
{
    D3DFORMAT fmt[] = {
#ifndef UNDER_XBOX
        D3DFMT_A8R8G8B8,
        D3DFMT_X8R8G8B8,
        D3DFMT_R5G6B5,
        D3DFMT_A1R5G5B5,
        D3DFMT_X1R5G5B5,
        D3DFMT_A4R4G4B4,
#else
        D3DFMT_LIN_A8R8G8B8,
        D3DFMT_LIN_X8R8G8B8,
        D3DFMT_LIN_R5G6B5,
        D3DFMT_LIN_A1R5G5B5,
        D3DFMT_LIN_X1R5G5B5,
        D3DFMT_LIN_A4R4G4B4,
#if 0
        D3DFMT_LIN_R6G5B5,
        D3DFMT_LIN_A8B8G8R8,
        D3DFMT_LIN_B8G8R8A8,
        D3DFMT_LIN_R4G4B4A4,
        D3DFMT_LIN_R5G5B5A1,
        D3DFMT_LIN_R8G8B8A8,
#endif
#endif
    };

    m_uCommonTextureFormats = countof(fmt);
    memcpy(m_fmtCommon, fmt, m_uCommonTextureFormats * sizeof(D3DFORMAT));

    pTexture = NULL;
	pDXTnTexture = NULL;
	pSysDXTnTexture = NULL;
    m_bExit = FALSE;
}

CCompressTest::~CCompressTest()
{
}

bool CCompressTest::SetDefaultRenderStates(void)
{
	// Turn off texture perspection
//	SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)FALSE);
	return true;
}

UINT CCompressTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Check for the appropriate compressed texture format
	bool bValidFormat = CheckDXTnFormat(fmtCompressed);

	// Request only the RGB texture formats
//	if (!RequestTextureFormats(PF_RGB))
//	{
//		WriteToLog("No common RGB texture formats found.\n");
//		return D3DTESTINIT_ABORT;
//	}

	// Setup the Test range
	SetTestRange(1, MAX_TESTS * m_uCommonTextureFormats);

	if (!bValidFormat)
	{
		WriteToLog("%s compressed texture format not supported.\n",m_szCommandKey);
		return D3DTESTINIT_SKIPALL;
	}

	// Check the caps we are interested in
	dwPerspectiveCap	= (m_d3dcaps.TextureCaps & D3DPTEXTURECAPS_PERSPECTIVE);
	dwSrcAlphaCap		= (m_d3dcaps.SrcBlendCaps & D3DPBLENDCAPS_SRCALPHA);
	dwOneCap			= (m_d3dcaps.SrcBlendCaps & D3DPBLENDCAPS_ONE);
	dwInvSrcAlphaCap	= (m_d3dcaps.DestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA);

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	// Setup the colors for the Stripes array
//	D3DCOLOR		Stripes[8];

	Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

	// Create the stripe image data
//    CImageLoader    Image;

//    pImage = Image.LoadStripes(256,256,8,Stripes,false); 

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
//    CImageLoader    AlphaImage;

//    pAlphaImage = AlphaImage.LoadStripes(256,256,8,AlphaStripes,false); 

	// Setup the colors for the AlphaStripes array
//	D3DCOLOR		White[4];

	White[0] = RGBA_MAKE(255,255,255,0);	// Full Alpha
	White[1] = RGBA_MAKE(255,255,255,64);	// 25% Alpha
	White[2] = RGBA_MAKE(255,255,255,192);	// 75% Alpha
	White[3] = RGBA_MAKE(255,255,255,255);	// No Alpha

	// Create the White image data
//    CImageLoader    TinyImage;

//    pTinyImage = TinyImage.LoadStripes(4,4,4,White,false); 

#ifdef UNDER_XBOX
    if (m_pDisplay->GetConnectionStatus() == SERVER_CONNECTED) {
        m_pDisplay->EnableRMC(FALSE);
        m_pDevice->SetRenderState(D3DRS_DXT1NOISEENABLE, FALSE);
        m_pDisplay->EnableRMC(TRUE);
    }
#endif

	return D3DTESTINIT_RUN;
}

bool CCompressTest::ClearFrame(void)
{
	return Clear(RGBA_MAKE(0,0,0,255));
}

#if 1

bool CCompressTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	UINT uTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;
	int nTest = ((uTestNum-1) / m_uCommonTextureFormats) + 1;
    D3DSURFACE_DESC d3dsdSrc, d3dsdDst;
    D3DLOCKED_RECT d3dlrSrc, d3dlrDst;
    HRESULT hr;
    POINT Pt = {0,0};
 	dwVertexCount = 0;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Cleanup previous textures
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);
    pTexture = NULL;
	ReleaseTexture(pDXTnTexture);
    pDXTnTexture = NULL;

	// Let's build some triangles.
	switch (nTest)
	{
		case (1): // Use no Alpha Blending
			sprintf(msgString, "%sAlpha (Tex): Default",msgString);
			sprintf(szStatus, "%sAlpha (Tex): Default",szStatus);
			DrawBlendingGrid(255);

			// Create the texture
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, m_fmtCommon[uTextureFormat]);
            pDXTnTexture = (CTexture8*)CreateTexture(m_pDevice, 256, 256, fmtCompressed);
			break;
		case (2): // Use texture Alpha Blending
			sprintf(msgString, "%sAlpha (Tex): SrcAlpha/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Tex): SrcAlpha/InvSrcAlpha",szStatus);
			DrawBlendingGrid(255);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, m_fmtCommon[uTextureFormat]);
            pDXTnTexture = (CTexture8*)CreateTexture(m_pDevice, 256, 256, fmtCompressed);
			break;
		case (3): // Use mesh Alpha Blending
			sprintf(msgString, "%sAlpha (Mesh): SrcAlpha/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Mesh): SrcAlpha/InvSrcAlpha",szStatus);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, Stripes, false, false, m_fmtCommon[uTextureFormat]);
            pDXTnTexture = (CTexture8*)CreateTexture(m_pDevice, 256, 256, fmtCompressed);
			break;
		case (4): // Use mesh and texture Alpha Blending
			sprintf(msgString, "%sAlpha (Both): SrcAlpha/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Both): SrcAlpha/InvSrcAlpha",szStatus);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, m_fmtCommon[uTextureFormat]);
            pDXTnTexture = (CTexture8*)CreateTexture(m_pDevice, 256, 256, fmtCompressed);
			break;
		case (5): // Use texture Alpha Blending
			sprintf(msgString, "%sAlpha (Tex 4x4): SrcAlpha/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Tex 4x4): SrcAlpha/InvSrcAlpha",szStatus);
			DrawBlendingGrid(255);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
            pTexture = CreateStripedTexture(m_pDevice, 4, 4, 4, White, false, false, m_fmtCommon[uTextureFormat]);
            pDXTnTexture = (CTexture8*)CreateTexture(m_pDevice, 4, 4, fmtCompressed);
			break;
		case (6): // Use texture Alpha Blending
			sprintf(msgString, "%sAlpha (Tex): One/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Tex): One/InvSrcAlpha",szStatus);
			DrawBlendingGrid(255);

			// Make sure device supports the proper alpha blending modes
			if (!dwOneCap || !dwInvSrcAlphaCap)
			{
				if (!dwOneCap)
					WriteToLog("Device capability not supported: SrcBlend - One.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, m_fmtCommon[uTextureFormat]);
            pDXTnTexture = (CTexture8*)CreateTexture(m_pDevice, 256, 256, fmtCompressed);
			break;
		case (7): // Use mesh Alpha Blending
			sprintf(msgString, "%sAlpha (Mesh): One/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Mesh): One/InvSrcAlpha",szStatus);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwOneCap || !dwInvSrcAlphaCap)
			{
				if (!dwOneCap)
					WriteToLog("Device capability not supported: SrcBlend - One.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, Stripes, false, false, m_fmtCommon[uTextureFormat]);
            pDXTnTexture = (CTexture8*)CreateTexture(m_pDevice, 256, 256, fmtCompressed);
			break;
		case (8): // Use mesh and texture Alpha Blending
			sprintf(msgString, "%sAlpha (Both): One/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Both): One/InvSrcAlpha",szStatus);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwOneCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - One.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, m_fmtCommon[uTextureFormat]);
            pDXTnTexture = (CTexture8*)CreateTexture(m_pDevice, 256, 256, fmtCompressed);
			break;
		default:
			DrawBasicGrid(nTest-8,m_uCommonTextureFormats);

			// Set the default alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

			// Set the texture perspective renderstate
			if (nTest >= 15)
			{
				// Make sure device supports texture perspection
				if (!dwPerspectiveCap)
				{
					WriteToLog("Device capability not supported: Texture Perspective.\n");
					SkipTests(m_uCommonTextureFormats);
					return false;
				}

//				SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)TRUE);
			}

			// Create the texture
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, Stripes, false, false, m_fmtCommon[uTextureFormat]);
            pDXTnTexture = (CTexture8*)CreateTexture(m_pDevice, 256, 256, fmtCompressed);
			break;
	}

	// Make sure we got valid texture pointers
	if (NULL == pTexture)
	{
		WriteToLog("CreateTexture() failed with HResult = %X.\n",GetLastError());
//		TestFinish(D3DTESTFINISH_ABORT);
	    ReleaseTexture(pDXTnTexture);
        pDXTnTexture = NULL;
		return false;
	}

	if (NULL == pDXTnTexture)
	{
		WriteToLog("CreateCompressedTexture(Video) failed with HResult = %X.\n",GetLastError());
//		TestFinish(D3DTESTFINISH_ABORT);
        ReleaseTexture(pTexture);
        pTexture = NULL;
		return false;
	}

#ifndef UNDER_XBOX
    hr = CopyTexture(pDXTnTexture, NULL, pTexture, NULL, D3DX_FILTER_POINT);
    if (FAILED(hr)) {
        Log(LOG_FAIL, TEXT("CopyTexture failed [0x%X]"), hr);
        ReleaseTexture(pTexture);
        pTexture = NULL;
        ReleaseTexture(pDXTnTexture);
        pDXTnTexture = NULL;
        return false;
    }
#else
    pTexture->GetLevelDesc(0, &d3dsdSrc);
    pDXTnTexture->GetLevelDesc(0, &d3dsdDst);

    pTexture->LockRect(0, &d3dlrSrc, NULL, 0);
    pDXTnTexture->LockRect(0, &d3dlrDst, NULL, 0);

    hr = XGCompressRect(d3dlrDst.pBits, d3dsdDst.Format, d3dlrDst.Pitch, d3dsdDst.Width, d3dsdDst.Height, 
                        d3dlrSrc.pBits, d3dsdSrc.Format, d3dlrSrc.Pitch, 0.5f, 
                        (d3dsdDst.Format == D3DFMT_DXT2 || d3dsdDst.Format == D3DFMT_DXT4) ? XGCOMPRESS_PREMULTIPLY : 0);

    pTexture->UnlockRect(0);
    pDXTnTexture->UnlockRect(0);

    if (FAILED(hr)) {
        Log(LOG_FAIL, TEXT("XGCompressRect failed [0x%X]"), hr);
        ReleaseTexture(pTexture);
        pTexture = NULL;
        ReleaseTexture(pDXTnTexture);
        pDXTnTexture = NULL;
        return false;
    }

    if (!SyncTexture(pDXTnTexture)) {
        ReleaseTexture(pTexture);
        pTexture = NULL;
        ReleaseTexture(pDXTnTexture);
        pDXTnTexture = NULL;
        return false;
    }
#endif

	// Tell the framework what texture to use
    SetTexture(0,pDXTnTexture);

    if (m_dwVersion <= 0x0700)
    {
    	sprintf(msgString, "%sTex:",msgString);
//        GetTexFmtDesc(&m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat,msgString+strlen(msgString),80-strlen(msgString));
    }
    else
#ifndef UNICODE
    	sprintf(msgString, "%sTex:%s",msgString,D3DFmtToString(m_fmtCommon[uTextureFormat]));
#else
    	sprintf(msgString, "%sTex:%S",msgString,D3DFmtToString(m_fmtCommon[uTextureFormat]));
#endif // UNICODE

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

#else

bool CCompressTest::ExecuteTest(UINT uTestNum)
{
	// Initialize some variables
	UINT uTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;
	int nTest = ((uTestNum-1) / m_uCommonTextureFormats) + 1;
    POINT Pt = {0,0};
 	dwVertexCount = 0;
	msgString[0] = '\0';
	szStatus[0] = '\0';

	// Cleanup previous textures
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);
    pTexture = NULL;
	ReleaseTexture(pDXTnTexture);
    pDXTnTexture = NULL;
	ReleaseTexture(pSysDXTnTexture);
    pSysDXTnTexture = NULL;

	// Cycle the texture formats
//	m_pMode->nTextureFormat = (uTestNum-1) % m_uCommonTextureFormats;

	// Let's build some triangles.
	switch (nTest)
	{
		case (1): // Use no Alpha Blending
			sprintf(msgString, "%sAlpha (Tex): Default",msgString);
			sprintf(szStatus, "%sAlpha (Tex): Default",szStatus);
			DrawBlendingGrid(255);

			// Create the texture
//		    pTexture = CreateTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),CDDS_TEXTURE_SYSTEM,pAlphaImage);
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, m_fmtCommon[uTextureFormat]);
//		    pDXTnTexture = CreateCompressedTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_VIDEO,NULL);
            pDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, fmtCompressed);

            if (0x0800 <= m_dwVersion)
//    		    pSysDXTnTexture = CreateCompressedTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_SYSTEM,NULL);
                pSysDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, fmtCompressed);
                
			break;
		case (2): // Use texture Alpha Blending
			sprintf(msgString, "%sAlpha (Tex): SrcAlpha/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Tex): SrcAlpha/InvSrcAlpha",szStatus);
			DrawBlendingGrid(255);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
//		    pTexture = CreateTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),CDDS_TEXTURE_SYSTEM,pAlphaImage);
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, m_fmtCommon[uTextureFormat]);
//		    pDXTnTexture = CreateCompressedTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_VIDEO,NULL);
            pDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, fmtCompressed);

            if (0x0800 <= m_dwVersion)
//    		    pSysDXTnTexture = CreateCompressedTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_SYSTEM,NULL);
                pSysDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, fmtCompressed);
                
			break;
		case (3): // Use mesh Alpha Blending
			sprintf(msgString, "%sAlpha (Mesh): SrcAlpha/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Mesh): SrcAlpha/InvSrcAlpha",szStatus);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
//		    pTexture = CreateTexture(pImage->GetWidth(),pImage->GetHeight(),CDDS_TEXTURE_SYSTEM,pImage);
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, Stripes, false, false, m_fmtCommon[uTextureFormat]);
//		    pDXTnTexture = CreateCompressedTexture(pImage->GetWidth(),pImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_VIDEO,NULL);
            pDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, Stripes, false, false, fmtCompressed);

            if (0x0800 <= m_dwVersion)
//    		    pSysDXTnTexture = CreateCompressedTexture(pImage->GetWidth(),pImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_SYSTEM,NULL);
                pSysDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, Stripes, false, false, fmtCompressed);
                
			break;
		case (4): // Use mesh and texture Alpha Blending
			sprintf(msgString, "%sAlpha (Both): SrcAlpha/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Both): SrcAlpha/InvSrcAlpha",szStatus);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
//		    pTexture = CreateTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),CDDS_TEXTURE_SYSTEM,pAlphaImage);
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, m_fmtCommon[uTextureFormat]);
//		    pDXTnTexture = CreateCompressedTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_VIDEO,NULL);
            pDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, fmtCompressed);

            if (0x0800 <= m_dwVersion)
//    		    pSysDXTnTexture = CreateCompressedTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_SYSTEM,NULL);
                pSysDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, fmtCompressed);
                
			break;
		case (5): // Use texture Alpha Blending
			sprintf(msgString, "%sAlpha (Tex 4x4): SrcAlpha/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Tex 4x4): SrcAlpha/InvSrcAlpha",szStatus);
			DrawBlendingGrid(255);

			// Make sure device supports the proper alpha blending modes
			if (!dwSrcAlphaCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - SrcAlpha.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
//		    pTexture = CreateTexture(pTinyImage->GetWidth(),pTinyImage->GetHeight(),CDDS_TEXTURE_SYSTEM,pTinyImage);
            pTexture = CreateStripedTexture(m_pDevice, 4, 4, 4, White, false, false, m_fmtCommon[uTextureFormat]);
//		    pDXTnTexture = CreateCompressedTexture(pTinyImage->GetWidth(),pTinyImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_VIDEO,NULL);
            pDXTnTexture = CreateStripedTexture(m_pDevice, 4, 4, 4, White, false, false, fmtCompressed);

            if (0x0800 <= m_dwVersion)
//    		    pSysDXTnTexture = CreateCompressedTexture(pTinyImage->GetWidth(),pTinyImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_SYSTEM,NULL);
                pSysDXTnTexture = CreateStripedTexture(m_pDevice, 4, 4, 4, White, false, false, fmtCompressed);
                
			break;
		case (6): // Use texture Alpha Blending
			sprintf(msgString, "%sAlpha (Tex): One/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Tex): One/InvSrcAlpha",szStatus);
			DrawBlendingGrid(255);

			// Make sure device supports the proper alpha blending modes
			if (!dwOneCap || !dwInvSrcAlphaCap)
			{
				if (!dwOneCap)
					WriteToLog("Device capability not supported: SrcBlend - One.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
//		    pTexture = CreateTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),CDDS_TEXTURE_SYSTEM,pAlphaImage);
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, m_fmtCommon[uTextureFormat]);
//		    pDXTnTexture = CreateCompressedTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_VIDEO,NULL);
            pDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, fmtCompressed);

            if (0x0800 <= m_dwVersion)
//    		    pSysDXTnTexture = CreateCompressedTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_SYSTEM,NULL);
                pSysDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, fmtCompressed);
                
			break;
		case (7): // Use mesh Alpha Blending
			sprintf(msgString, "%sAlpha (Mesh): One/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Mesh): One/InvSrcAlpha",szStatus);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwOneCap || !dwInvSrcAlphaCap)
			{
				if (!dwOneCap)
					WriteToLog("Device capability not supported: SrcBlend - One.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
//		    pTexture = CreateTexture(pImage->GetWidth(),pImage->GetHeight(),CDDS_TEXTURE_SYSTEM,pImage);
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, Stripes, false, false, m_fmtCommon[uTextureFormat]);
//		    pDXTnTexture = CreateCompressedTexture(pImage->GetWidth(),pImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_VIDEO,NULL);
            pDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, Stripes, false, false, fmtCompressed);

            if (0x0800 <= m_dwVersion)
//    		    pSysDXTnTexture = CreateCompressedTexture(pImage->GetWidth(),pImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_SYSTEM,NULL);
                pSysDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, Stripes, false, false, fmtCompressed);
                
			break;
		case (8): // Use mesh and texture Alpha Blending
			sprintf(msgString, "%sAlpha (Both): One/InvSrcAlpha",msgString);
			sprintf(szStatus, "%sAlpha (Both): One/InvSrcAlpha",szStatus);
			DrawBlendingGrid(0);

			// Make sure device supports the proper alpha blending modes
			if (!dwOneCap || !dwInvSrcAlphaCap)
			{
				if (!dwSrcAlphaCap)
					WriteToLog("Device capability not supported: SrcBlend - One.\n");

				if (!dwInvSrcAlphaCap)
					WriteToLog("Device capability not supported: DestBlend - InvSrcAlpha.\n");

				SkipTests(m_uCommonTextureFormats);
				return false;
			}

			// Set the appropriate alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)TRUE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_INVSRCALPHA);

			// Create the texture
//		    pTexture = CreateTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),CDDS_TEXTURE_SYSTEM,pAlphaImage);
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, m_fmtCommon[uTextureFormat]);
//		    pDXTnTexture = CreateCompressedTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_VIDEO,NULL);
            pDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, fmtCompressed);

            if (0x0800 <= m_dwVersion)
//    		    pSysDXTnTexture = CreateCompressedTexture(pAlphaImage->GetWidth(),pAlphaImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_SYSTEM,NULL);
                pSysDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, AlphaStripes, false, false, fmtCompressed);
                
			break;
		default:
			DrawBasicGrid(nTest-8,m_uCommonTextureFormats);

			// Set the default alpha blending renderstates
			SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD)FALSE);
			SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD)D3DBLEND_ONE);
			SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD)D3DBLEND_ZERO);

			// Set the texture perspective renderstate
			if (nTest >= 15)
			{
				// Make sure device supports texture perspection
				if (!dwPerspectiveCap)
				{
					WriteToLog("Device capability not supported: Texture Perspective.\n");
					SkipTests(m_uCommonTextureFormats);
					return false;
				}

//				SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)TRUE);
			}

			// Create the texture
//		    pTexture = CreateTexture(pImage->GetWidth(),pImage->GetHeight(),CDDS_TEXTURE_SYSTEM,pImage);
            pTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, Stripes, false, false, m_fmtCommon[uTextureFormat]);
//		    pDXTnTexture = CreateCompressedTexture(pImage->GetWidth(),pImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_VIDEO,NULL);
            pDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, Stripes, false, false, fmtCompressed);

            if (0x0800 <= m_dwVersion)
//    		    pSysDXTnTexture = CreateCompressedTexture(pImage->GetWidth(),pImage->GetHeight(),dwFourCC,1,CDDS_TEXTURE_SYSTEM,NULL);
                pSysDXTnTexture = CreateStripedTexture(m_pDevice, 256, 256, 8, Stripes, false, false, fmtCompressed);
                
			break;
	}

	// Make sure we got valid texture pointers
	if (NULL == pTexture)
	{
		WriteToLog("CreateTexture() failed with HResult = %X.\n",GetLastError());
//		TestFinish(D3DTESTFINISH_ABORT);
	    ReleaseTexture(pDXTnTexture);
        pDXTnTexture = NULL;
	    ReleaseTexture(pSysDXTnTexture);
        pSysDXTnTexture = NULL;
		return false;
	}

	if (NULL == pDXTnTexture)
	{
		WriteToLog("CreateCompressedTexture(Video) failed with HResult = %X.\n",GetLastError());
//		TestFinish(D3DTESTFINISH_ABORT);
        ReleaseTexture(pTexture);
        pTexture = NULL;
	    ReleaseTexture(pSysDXTnTexture);
        pSysDXTnTexture = NULL;
		return false;
	}

    if ((0x0800 <= m_dwVersion) && (NULL == pSysDXTnTexture))
	{
		WriteToLog("CreateCompressedTexture(System) failed with HResult = %X.\n",GetLastError());
//		TestFinish(D3DTESTFINISH_ABORT);
        ReleaseTexture(pTexture);
        pTexture = NULL;
	    ReleaseTexture(pDXTnTexture);
        pDXTnTexture = NULL;
		return false;
	}
/*
    if (m_dwVersion <= 0x0700)
    {
	    // Blt from RGB source surface to DXTn source surface
	    if (!pDXTnTexture->m_pSrcSurface->Blt(pTexture->m_pSrcSurface,Pt))
	    {
		    WriteToLog("Source Blt() failed with HResult = %s.",GetHResultString(GetLastError()).c_str());
		    SkipTests(1);
		    return false;
	    }

	    if (NULL != m_pRefTarget)
	    {
    	    // Blt from RGB ref surface to DXTn ref surface
		    if (!pDXTnTexture->m_pRefSurface->Blt(pTexture->m_pRefSurface,Pt))
		    {
			    WriteToLog("Reference Blt() failed with HResult = %s.",GetHResultString(GetLastError()).c_str());
			    SkipTests(1);
			    return false;
		    }
	    }
    }
    else
    {
*/
	    // Load from RGB surface to DXTn surface
        if (!DXTnLoad8())
        {
//    		TestFinish(D3DTESTFINISH_ABORT);
            ReleaseTexture(pTexture);
            pTexture = NULL;
	        ReleaseTexture(pDXTnTexture);
            pDXTnTexture = NULL;
	        ReleaseTexture(pSysDXTnTexture);
            pSysDXTnTexture = NULL;
		    return false;
        }
//    }

	// Tell the framework what texture to use
    SetTexture(0,pDXTnTexture);

    if (m_dwVersion <= 0x0700)
    {
    	sprintf(msgString, "%sTex:",msgString);
//        GetTexFmtDesc(&m_pCommonTextureFormats[m_pMode->nTextureFormat].ddpfPixelFormat,msgString+strlen(msgString),80-strlen(msgString));
    }
    else
#ifndef UNICODE
    	sprintf(msgString, "%sTex:%s",msgString,D3DFmtToString(m_fmtCommon[uTextureFormat]));
#else
    	sprintf(msgString, "%sTex:%S",msgString,D3DFmtToString(m_fmtCommon[uTextureFormat]));
#endif // UNICODE

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

#endif

void CCompressTest::SceneRefresh()
{
	if (BeginScene())
	{
		// Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();
	}	
}

bool CCompressTest::ProcessFrame(void)
{
	char	szBuffer[80];
	static	int nPass = 0;
	static  int nFail = 0;
    bool	bResult;

     // Use the standard 15%
    bResult = GetCompareResult(0.15f, 0.78f, 0);

	// Tell the logfile how the compare went
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

    return bResult;
}

bool CCompressTest::TestTerminate()
{
	// Cleanup texture & image data
	SetTexture(0, NULL);
    ReleaseTexture(pTexture);
    pTexture = NULL;
	ReleaseTexture(pDXTnTexture);
    pDXTnTexture = NULL;
	ReleaseTexture(pSysDXTnTexture);
    pSysDXTnTexture = NULL;
//    RELEASE(pImage);
//	RELEASE(pAlphaImage);
//	RELEASE(pTinyImage);

	return true;
}

bool CCompressTest::CheckDXTnFormat(D3DFORMAT fmtDXTn)
{
/*
	bool	bFoundDXTn = false;

	// Request only the FourCC texture formats
	if (!RequestTextureFormats(PF_FOURCC))
		return false;

	for (int i=0; !bFoundDXTn && i < m_uCommonTextureFormats; i++)
	{
        // TODO: need to handle DX8 formats (m_pCommonTextureFormats[m_pMode->nTextureFormat].d3dfFormat)
		if (m_pCommonTextureFormats[i].ddpfPixelFormat.dwFourCC == dwDXTn)
			bFoundDXTn = true;
	}

	// Reset the texture format common list
	ResetTextureFormats();

	return bFoundDXTn;
*/
    return SUCCEEDED(CheckCurrentDeviceFormat(m_pDevice, 0, D3DRTYPE_TEXTURE, fmtDXTn));
}

//******************************************************************************
BOOL CCompressTest::AbortedExit(void) {

    return m_bExit;
}

//******************************************************************************
void CCompressTest::ProcessInput() {

    CD3DTest::ProcessInput();

    if (BUTTON_PRESSED(m_jsJoys, JOYBUTTON_BACK)) {
        m_bExit = TRUE;
    }
}

//******************************************************************************
BOOL CCompressTest::WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

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


