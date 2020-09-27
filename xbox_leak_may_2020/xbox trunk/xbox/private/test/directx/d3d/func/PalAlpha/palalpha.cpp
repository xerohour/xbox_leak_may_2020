#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "PalAlpha.h"

//CD3DWindowFramework		theApp;
//CPalTest				TestOne;

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

    CPalTest* pPalTest;
    BOOL      bRet;

    // Verify that the display has been initialized
    if (!pDisplay) {
        return FALSE;
    }

    // Create the scene
    pPalTest = new CPalTest();
    if (!pPalTest) {
        return FALSE;
    }

    // Initialize the scene
    if (!pPalTest->Create(pDisplay)) {
        pPalTest->Release();
        return FALSE;
    }

    bRet = pPalTest->Exhibit(pnExitCode);

    // Clean up the scene
    pPalTest->Release();

    return bRet;
}


D3DTLVERTEX		CPalTest::vertices1[] = {
	{0.f,  0.f, 0.f, 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00), 0.01f, 0.99f},
	{10.f, 0.f, 0.f, 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00), 0.99f, 0.99f},
	{10.f,10.f, 0.f, 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00), 0.99f, 0.01f},
	{0.f, 10.f, 0.f, 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00), 0.01f, 0.01f},
};

D3DTLVERTEX		CPalTest::vertices2[] = {
	{0.f,  0.f, 0.f, 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00), -1.f,  2.f},
	{10.f, 0.f, 0.f, 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00),  2.f,  2.f},
	{10.f,10.f, 0.f, 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00),  2.f, -1.f},
	{0.f, 10.f, 0.f, 1.f, D3DCOLOR(0xffffffff), D3DCOLOR(0x00), -1.f, -1.f},
};

WORD			CPalTest::indices[]  = {0, 1, 2, 0, 2, 3};

CPalTest::CPalTest()
{
	bTest4BitPalette = true;
	bTest8BitPalette = true;
	bTestNonAlphaPalette = true;
	bTestAlphaPalette = true;
	dwCurrentTextureType = UNDEFINED_TEXTURE;
	bCreateNewTexture = false;
	lpszTextureType = NULL;
	
	bTestTextureCreate = true;
	bTestTextureLoad = true;
	bTestPaletteCreate = true;
	bTestSetEntries = true;
	bTestWrapMode = true;
	bTestMirrorMode = true;
	bTestClampMode = true;
	bTestBorderMode = true;
	bTestColorKey = true;
    bTestColorKeyBlend = true;
	lpszTestType = NULL;

	dwTextureTypesNumber = 4;
	dwTestTypesNumber = 10;
	dwSameFramesNumber = 64;
	dwTotalTestsNumber = dwTextureTypesNumber*dwTestTypesNumber*dwSameFramesNumber;

	dwPaletteType = 3;
	bAlphaPalette = true;
	bCreateTexture = false;
	bLoadTexture = false;
	bCreatePalette = false;
	bSetPaletteEntries = true;
	dwAddressMode = 0;
	bUseColorKey = false;
    bUseColorKeyBlend = false;
	dwFrameParam = 0;
	bLastTestFailed = false;

//	pPalette = NULL;
	dwNumFail = 0;

    pVertices = vertices1;
    dwVertexCount=4;
	pIndices = indices;
	dwIndexCount = 6;

	m_pTexture = 0;
	m_pTextureSource = 0;

	m_szTestName = TEXT("Palettized texture test");
	m_szCommandKey = TEXT("PalAlpha");
}

CPalTest::~CPalTest() {
    ReleaseTexture(m_pTexture);
	ReleaseTexture(m_pTextureSource);
//    RELEASE(m_pLight);
//    RELEASE(m_pMaterial);
}


UINT CPalTest::TestInitialize(void) {
//    UINT n = m_pMode->nSrcDevice;

    if (m_dwVersion >= 0x0800)
    {
        bTestPaletteCreate = true;
    }
    else
    {
        if (KeySet("RECREATEPALETTE"))
            bTestPaletteCreate = true;
        else
            bTestPaletteCreate = false;
    }

    if (m_dwVersion >= 0x0800)
    {
        bTestTextureLoad = false;
        bTestColorKeyBlend = false;
        bTestColorKey = false;
        bTest4BitPalette = false;
    }

	// Check the device supports paletted textures
	if (bTest8BitPalette) {
		bTest8BitPalette = false;
//		for (int i = 0; i < m_uCommonTextureFormats; i++) {
//			if (m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
				bTest8BitPalette = true;
//		}
	}

	if (bTest4BitPalette) {
		bTest4BitPalette = false;
//		for (int i = 0; i < m_uCommonTextureFormats; i++) {
//			if (m_pCommonTextureFormats[i].ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
				bTest4BitPalette = true;
//		}
	}
	
	// Check the device supports alpha in texture palette

	if (bTestAlphaPalette && m_d3dcaps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE)
		bTestAlphaPalette = true;
	else
		bTestAlphaPalette = false;

	// Check supported addressing modes
	if (bTestWrapMode && m_d3dcaps.TextureAddressCaps & D3DPTADDRESSCAPS_WRAP)
		bTestWrapMode = true;
	else
		bTestWrapMode = false;
	
	if (bTestMirrorMode && m_d3dcaps.TextureAddressCaps & D3DPTADDRESSCAPS_MIRROR)
		bTestMirrorMode = true;
	else
		bTestMirrorMode = false;

	if (bTestClampMode && m_d3dcaps.TextureAddressCaps & D3DPTADDRESSCAPS_CLAMP)
		bTestClampMode = true;
	else
		bTestClampMode = false;
	
	if (bTestBorderMode && m_d3dcaps.TextureAddressCaps & D3DPTADDRESSCAPS_BORDER)
		bTestBorderMode = true;
	else
		bTestBorderMode = false;

	if (bTestColorKey && m_d3dcaps.TextureCaps & D3DPTEXTURECAPS_ALPHA)
		bTestColorKey = true;
	else
		bTestColorKey = false;

//	if (bTestColorKeyBlend && m_d3dcaps.TextureCaps & D3DPTEXTURECAPS_COLORKEYBLEND)
//		bTestColorKeyBlend = true;
//	else
		bTestColorKeyBlend = false;

	if (m_dwVersion >= 0x0800)
        SetTestRange(1,(dwTotalTestsNumber>>1));
    else
        SetTestRange(1,dwTotalTestsNumber);
		
	// Return error if device do not support paletted textures
	if (!bTest4BitPalette && !bTest8BitPalette) {
		WriteToLog("$yWarning: $wThe device does not support paletted textures.\n");
		return D3DTESTINIT_SKIPALL;
	}

	if (!bTestAlphaPalette && !bTestNonAlphaPalette) {
		WriteToLog("$yWarning: $wThe device does not support alpha in texture palette.\n");
		return D3DTESTINIT_SKIPALL;
	}
 
	vertices1[1].sx = vertices2[1].sx = float(m_pDisplay->GetWidth());
	vertices1[2].sx = vertices2[2].sx = float(m_pDisplay->GetWidth());
	vertices1[2].sy = vertices2[2].sy = float(m_pDisplay->GetHeight());
	vertices1[3].sy = vertices2[3].sy = float(m_pDisplay->GetHeight());

	return D3DTESTINIT_RUN;
}


bool CPalTest::ExecuteTest(UINT iTestNum) {
	char	name_buf[80];
    bool    bColorKeyFill;

	dwCurrentTestNumber = iTestNum;
	ResolveTestNumber(iTestNum);
	sprintf(name_buf, "%s (%s)", lpszTestType, lpszTextureType);
	if (!ValidateTest(iTestNum))
		return false;

	BeginTestCase(name_buf);

    bColorKeyFill = (bUseColorKey || bUseColorKeyBlend) ? true : false;

	if (m_dwVersion >= 0x0800)
    {
        if (bCreateNewTexture) {
		    SetTexture(0,0);
		    ReleaseTexture(m_pTexture);
            m_pTexture = CreateTexture8(256, 256);
		    CreateTexturePalette8(0, bAlphaPalette, dwFrameParam);
            SetCurrentTexturePalette8(0);
		    SetTexture(0,m_pTexture);
		    bCreateNewTexture = false;
	    }

	    if (bCreateTexture) {
		    SetTexture(0,0);
		    ReleaseTexture(m_pTexture);
		    m_pTexture = CreateTexture8(256, 256);
		    SetTexture(0,m_pTexture);
	    }

	    if (bCreatePalette) {
		    CreateTexturePalette8(dwFrameParam, bAlphaPalette, dwFrameParam);
            SetCurrentTexturePalette8(dwFrameParam);
	    }

	    if (bSetPaletteEntries) {
		    CreateTexturePalette8(0, bAlphaPalette, dwFrameParam);
	    }
    }
/*
    else
    {
        if (bCreateNewTexture) {
		    SetTexture(0,0);
		    if (m_pTexture)	delete m_pTexture;
            m_pTexture = CreateTexture(256, 256, dwPaletteType, true, bColorKeyFill);
		    CreateTexturePalette(m_pTexture->m_pSrcSurface, dwPaletteType, bAlphaPalette, dwFrameParam, bColorKeyFill);
		    CreateTexturePalette(m_pTexture->m_pRefSurface, dwPaletteType, bAlphaPalette, dwFrameParam, bColorKeyFill);
		    SetTexture(0,m_pTexture);
		    bCreateNewTexture = false;
	    }

	    if (bCreateTexture) {
		    SetTexture(0,0);
		    if (m_pTexture)	delete m_pTexture;
		    m_pTexture = CreateTexture(256, 256, dwPaletteType, true, bColorKeyFill);
		    SetTexture(0,m_pTexture);
	    }

	    if (bCreatePalette) {
		    CreateTexturePalette(m_pTexture->m_pSrcSurface, dwPaletteType, bAlphaPalette, dwFrameParam, bColorKeyFill);
		    CreateTexturePalette(m_pTexture->m_pRefSurface, dwPaletteType, bAlphaPalette, dwFrameParam, bColorKeyFill);
		    SetTexture(0,m_pTexture);
	    }

	    if (bSetPaletteEntries) {
		    ChangeTexturePalette(m_pTexture->m_pSrcSurface, dwPaletteType, bAlphaPalette, dwFrameParam, bColorKeyFill);
		    ChangeTexturePalette(m_pTexture->m_pRefSurface, dwPaletteType, bAlphaPalette, dwFrameParam, bColorKeyFill);
		    SetTexture(0,m_pTexture);
	    }

	    if (bLoadTexture) {
		    POINT	pt = {0, 0};
		    RECT	rect = {0, 0, 256, 256};
		    SetTexture(0,0);
		    if (m_pTextureSource)	delete m_pTextureSource;
		    m_pTextureSource = CreateTexture(256, 256, dwPaletteType, false, bColorKeyFill);
		    CreateTexturePalette(m_pTextureSource->m_pSrcSurface, dwPaletteType, bAlphaPalette, 0, bColorKeyFill);
		    CreateTexturePalette(m_pTextureSource->m_pRefSurface, dwPaletteType, bAlphaPalette, 0, bColorKeyFill);
		    if (m_dwVersion > 0x0600)
		    {
			    SetLastError(m_pSrcDevice7->Load(m_pTextureSource->m_pSrcSurface->m_pSurface7, &pt, m_pTexture->m_pSrcSurface->m_pSurface7, &rect, 0));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Texture load failed\n");
                    Fail();
				    return false;
			    }
			    SetLastError(m_pRefDevice7->Load(m_pTextureSource->m_pRefSurface->m_pSurface7, &pt, m_pTexture->m_pRefSurface->m_pSurface7, &rect, 0));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Texture load failed\n");
                    Fail();
				    return false;
			    }
		    }
		    else
		    {
			    LPDIRECT3DTEXTURE2	pSrcTexture2, pDstTexture2;

			    SetLastError(m_pTexture->m_pSrcSurface->m_pSurface4->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pSrcTexture2));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Can't QI texture interface\n");
                    Fail();
				    return false;
			    }
			    SetLastError(m_pTextureSource->m_pSrcSurface->m_pSurface4->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pDstTexture2));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Can't QI texture interface\n");
                    Fail();
				    return false;
			    }
			    SetLastError(pDstTexture2->Load(pSrcTexture2));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Texture load failed\n");
                    Fail();
				    return false;
			    }
			    RELEASE(pSrcTexture2);
			    RELEASE(pDstTexture2);

			    SetLastError(m_pTexture->m_pRefSurface->m_pSurface4->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pSrcTexture2));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Can't QI texture interface\n");
                    Fail();
				    return false;
			    }
			    SetLastError(m_pTextureSource->m_pRefSurface->m_pSurface4->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pDstTexture2));
			    if (GetLastError() != D3D_OK)
			    {
				    WriteToLog("PalAlpha: Can't QI texture interface\n");
                    Fail();
				    return false;
			    }
			    SetLastError(pDstTexture2->Load(pSrcTexture2));
			    RELEASE(pSrcTexture2);
			    RELEASE(pDstTexture2);

		    }
		    SetTexture(0,m_pTextureSource);
	    }

	    if (bUseColorKey || bUseColorKeyBlend) {
		    DDCOLORKEY	key;

		    if (dwPaletteType == 3) {
			    key.dwColorSpaceLowValue = 120;
			    key.dwColorSpaceHighValue = 120;
		    }
		    else {
			    key.dwColorSpaceLowValue = 8;
			    key.dwColorSpaceHighValue = 8;
		    }

		    if (bUseColorKey)
                SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, (DWORD)TRUE);
            else
                SetRenderState(D3DRENDERSTATE_COLORKEYBLENDENABLE, (DWORD)TRUE);

		    SetLastError(m_pTexture->m_pSrcSurface->m_pSurface4->SetColorKey(DDCKEY_SRCBLT, &key));
            if (GetLastError() != D3D_OK)
		    {
			    WriteToLog("PalAlpha: SetColorKey failed\n");
                Fail();
			    return false;
		    }
		    SetLastError(m_pTexture->m_pRefSurface->m_pSurface4->SetColorKey(DDCKEY_SRCBLT, &key));
            if (GetLastError() != D3D_OK)
		    {
			    WriteToLog("PalAlpha: SetColorKey failed\n");
                Fail();
			    return false;
		    }
	    }
	    else {
		    SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, (DWORD)FALSE);
            SetRenderState(D3DRENDERSTATE_COLORKEYBLENDENABLE, (DWORD)FALSE);
	    }

    }
*/

	switch (dwAddressMode) {
	case 0:
		pVertices = vertices1;
		SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_WRAP);
		SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_WRAP);
		break;
	case D3DTADDRESS_WRAP:
		pVertices = vertices2;
		SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_WRAP);
		SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_WRAP);
		break;
	case D3DTADDRESS_MIRROR:
		pVertices = vertices2;
		SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_MIRROR);
		SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_MIRROR);
		break;
	case D3DTADDRESS_CLAMP:
		pVertices = vertices2;
		SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_CLAMP);
		SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_CLAMP);
		break;
	case D3DTADDRESS_BORDER:
		pVertices = vertices2;
		SetTextureStageState(0, D3DTSS_ADDRESSU, (DWORD)D3DTADDRESS_BORDER);
		SetTextureStageState(0, D3DTSS_ADDRESSV, (DWORD)D3DTADDRESS_BORDER);
		SetTextureStageState(0, D3DTSS_BORDERCOLOR, RGBA_MAKE(100, 100, 100, 100));
		break;
	}

	return true;
}

bool CPalTest::ClearFrame()
{
//	if (m_pSrcTarget && m_pSrcTarget->m_pSurface4 && !ColorSurface(m_pSrcTarget->m_pSurface4))
//        return false;
//	if (m_pRefTarget && m_pRefTarget->m_pSurface4 && !ColorSurface(m_pRefTarget->m_pSurface4))
//        return false;
    return true;
}

void CPalTest::SceneRefresh() {
	D3DMATRIX	Matrix;

	Matrix = IdentityMatrix();
  
	if (BeginScene())  {
        // TODO: Render by using RenderPrimitive()
		SetTransform(D3DTRANSFORMSTATE_WORLD,&Matrix);
		if (!RenderPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, pVertices, 4, pIndices, 6, 0))
			WriteToLog("Error in primitive rendering\n");

		EndScene();
    }

    return;
}

bool CPalTest::ProcessFrame(void) {
    BOOL    bResult;

    // This range (0.00002) basically allows 1 pixel to be different
//    bResult = GetCompareResult(0.0071f, 0.78f, 0);
    bResult = GetCompareResult(0.05f, 0.78f, 0);

    if (!bResult) {
        WriteToLog("Test %d failed (%s; %s).\n", dwCurrentTestNumber, lpszTextureType, lpszTestType);
		bLastTestFailed = true;
		Fail();
		dwNumFail++;
    }
	else {
		Pass();
	}
	EndTestCase();

    return true;
}

void CPalTest::TestCompleted() {
    WriteToLog("$g%Palette recreation test flush summary:\n");
    WriteToLog("$cErrors:\t\t$y%d\n",dwNumFail);
}

bool CPalTest::TestTerminate()
{
    ReleaseTexture(m_pTexture);
	ReleaseTexture(m_pTextureSource);
//    RELEASE(m_pLight);
//    RELEASE(m_pMaterial);
    return true;
}

void CPalTest::ResolveTestNumber(int iTestNum) {
	DWORD	num = iTestNum - 1;
	
	dwFrameParam = num % dwSameFramesNumber;

	num /= dwSameFramesNumber;
	dwCurrentTestType = num%10;
	switch(dwCurrentTestType) {
	case 0:
		// Texture create test
		bCreateTexture = true;
		bLoadTexture = false;
		bCreatePalette = true;
		bSetPaletteEntries = false;
		dwAddressMode = 0;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = TEXT("Create texture");
		break;
	case 1:
		// Texture load test
		bCreateTexture = false;
		bLoadTexture = true;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = 0;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = TEXT("Load texture");
		break;
	case 2:
		// Palette create test
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = true;
		bSetPaletteEntries = false;
		dwAddressMode = 0;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = TEXT("Create palette");
		break;
	case 3:
		// SetEntries test
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = 0;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = TEXT("Set entries");
		break;
	case 4:
		// WrapMode test
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = D3DTADDRESS_WRAP;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = TEXT("Wrap addressing mode");
		break;
	case 5:
		// MirrorMode test
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = D3DTADDRESS_MIRROR;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = TEXT("Mirror addressing mode");
		break;
	case 6:
		// ClampMode test
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = D3DTADDRESS_CLAMP;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = TEXT("Clamp addressing mode");
		break;
	case 7:
		// BorderMode test
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = D3DTADDRESS_BORDER;
		bUseColorKey = false;
        bUseColorKeyBlend = false;
		lpszTestType = TEXT("Border addressing mode");
		break;
	case 8:
		// ColorKey test
		bCreateNewTexture = true;
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = 0;
		bUseColorKey = true;
        bUseColorKeyBlend = false;
		lpszTestType = TEXT("color key");
		break;
    case 9:
        // ColorKeyBlend test
		bCreateNewTexture = true;
		bCreateTexture = false;
		bLoadTexture = false;
		bCreatePalette = false;
		bSetPaletteEntries = true;
		dwAddressMode = 0;
		bUseColorKey = false;
        bUseColorKeyBlend = true;
		lpszTestType = TEXT("color key blend");
		break;
	} 

	num /= 10;
	switch (num) {
	case 0:
		dwPaletteType = 3;
		bAlphaPalette = true;
		lpszTextureType = TEXT("8 bit alpha");
		if (dwCurrentTextureType != PAL8ALPHA) {
			dwCurrentTextureType = PAL8ALPHA;
			bCreateNewTexture = true;
			bCreateTexture = false;
			bCreatePalette = false;
		}
		break;
	case 1:
		dwPaletteType = 3;
		bAlphaPalette = false;
		lpszTextureType = TEXT("8 bit");
		if (dwCurrentTextureType != PAL8) {
			dwCurrentTextureType = PAL8;
			bCreateNewTexture = true;
//			bCreateTexture = false;
//			bCreatePalette = false;
		}
		break;
	case 2:
		dwPaletteType = 2;
		bAlphaPalette = true;
		lpszTextureType = TEXT("4 bit alpha");
		if (dwCurrentTextureType != PAL4ALPHA) {
			dwCurrentTextureType = PAL4ALPHA;
			bCreateNewTexture = true;
			bCreateTexture = false;
			bCreatePalette = false;
		}
		break;
	case 3:
		dwPaletteType = 2;
		bAlphaPalette = false;
		lpszTextureType = TEXT("4 bit");
		if (dwCurrentTextureType != PAL4) {
			dwCurrentTextureType = PAL4;
			bCreateNewTexture = true;
			bCreateTexture = false;
			bCreatePalette = false;
		}
		break;
	}
	WriteStatus(TEXT("$wTexture type$g"), lpszTextureType);
	WriteStatus(TEXT("$wTest type$g"), lpszTestType);
}

bool CPalTest::ValidateTest(int iTestNum) {
	int	test = iTestNum - 1;

	if (bLastTestFailed) {	
        bLastTestFailed = false;
		bCreateNewTexture = true;
//		SkipTests(dwSameFramesNumber - test%dwSameFramesNumber);
//		return false;
	}

	// Validate texture type
	if ( (dwPaletteType == 3 && !bTest8BitPalette) || (dwPaletteType == 2 && !bTest4BitPalette) ) {
		int num = dwSameFramesNumber*dwTestTypesNumber*2;
		SkipTests(num - test%num);
		return false;
	}
	
	if ( (bAlphaPalette && !bTestAlphaPalette) || (!bAlphaPalette && !bTestNonAlphaPalette) ) {
		int num = dwSameFramesNumber*dwTestTypesNumber;
		SkipTests(num - test%num);
		return false;
	}

	// Validate test type
	if ((dwCurrentTestType == 0 && !bTestTextureCreate)	||
		(dwCurrentTestType == 1 && !bTestTextureLoad)	||
		(dwCurrentTestType == 2 && !bTestPaletteCreate)	||
		(dwCurrentTestType == 3 && !bTestSetEntries)	||
		(dwCurrentTestType == 4 && !bTestWrapMode)		||
		(dwCurrentTestType == 5 && !bTestMirrorMode)	||
		(dwCurrentTestType == 6 && !bTestClampMode)		||
		(dwCurrentTestType == 7 && !bTestBorderMode)	||
		(dwCurrentTestType == 8 && !bTestColorKey)      ||
		(dwCurrentTestType == 9 && !bTestColorKeyBlend)	) 
	{
		SkipTests(dwSameFramesNumber - test%dwSameFramesNumber);
		return false;
	}

    // Don't run color key test on texture with alpha information on RGB device
//    if (dwCurrentTestType == 8 && bAlphaPalette && m_pAdapter->Devices[m_pMode->nSrcDevice].Guid == IID_IDirect3DRGBDevice)
//    {
//		SkipTests(dwSameFramesNumber - test%dwSameFramesNumber);
//		return false;
//    }

	return true;

	DECLARE(iTestNum);
}


bool CPalTest::SetDefaultMatrices(void) {
    D3DMATRIX           M;
    // We need to be far from scane to receive correct specular highlights
	D3DVECTOR           from = cD3DVECTOR(0.0f, 0.0f, 1.0f); 
    D3DVECTOR	        at = cD3DVECTOR(0.0f, 0.0f, 0.0f);
    D3DVECTOR	        up = cD3DVECTOR(0.0f, 1.0f, 0.0f);
    float               fZn=0.5f;
	float               fZf=1.5f;
	DWORD				dwWidth = m_pDisplay->GetWidth();
	DWORD				dwHeight = m_pDisplay->GetHeight();
    
	// Setup an orthogonal projection

	// Projection matrix for source
    InitMatrix(&M, 
	    2.f / dwWidth,	0.f,			0.f,		0.f,
	    0.f,			2.f / dwHeight,	0.f,		0.f,
	    0.f,			0.f,			1.f / 20.f,	0.f,
	    0.f,			0.f,			10.f/ 20.f,	1.f
    );
	if (!SetTransform(D3DTRANSFORMSTATE_PROJECTION,&M))
		return false;


    M = IdentityMatrix();
    if (!SetTransform(D3DTRANSFORMSTATE_WORLD,&M))
        return false;
    if (!SetTransform(D3DTRANSFORMSTATE_VIEW,&M))
        return false;

    return true;
}

bool CPalTest::SetDefaultMaterials(void) {
    D3DMATERIAL8         Material;
    HRESULT              hr;

//    m_pMaterial = CreateMaterial();

//    if (NULL == m_pMaterial)
//        return false;

    ZeroMemory(&Material,sizeof(D3DMATERIAL8));
    Material.Diffuse.r  = 1.0f;
    Material.Diffuse.g  = 1.0f;
    Material.Diffuse.b  = 1.0f;
    Material.Diffuse.a  = 1.0f;
    Material.Ambient.r  = 1.0f;
    Material.Ambient.g  = 1.0f;
    Material.Ambient.b  = 1.0f;
    Material.Ambient.a  = 1.0f;
    Material.Specular.r = 0.1f;
    Material.Specular.g = 0.1f;
    Material.Specular.b = 0.1f;
    Material.Specular.a  = 1.0f;
    Material.Power      = 20.0f;

//    if (!m_pMaterial->SetMaterial(&Material))
//        return false;

    // Now that the material data has been set,
    // re-set the material so the changes get picked up.

//    if (!SetMaterial(m_pMaterial))
//        return false;
    
    hr = m_pDevice->SetMaterial(&Material);
    return (hr == D3D_OK);
}

bool CPalTest::SetDefaultLightStates(void) {
//    SetLightState(D3DLIGHTSTATE_AMBIENT,RGBA_MAKE(255,255,255,255));
#if(DIRECT3D_VERSION >= 0x0700)
    if (m_dwVersion >= 0x0700) {
		SetRenderState(D3DRENDERSTATE_AMBIENT,RGB_MAKE(25,25,25));
		SetRenderState(D3DRENDERSTATE_LOCALVIEWER,(DWORD)FALSE);
	}
#endif
    return true;
}

bool CPalTest::SetDefaultRenderStates(void) {
//    SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, (DWORD)TRUE);
//    SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (DWORD)D3DFILTER_LINEAR);
//    SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (DWORD)D3DFILTER_LINEAR);
    SetTextureStageState(0, D3DTSS_MINFILTER, (DWORD)D3DTEXF_LINEAR);
    SetTextureStageState(0, D3DTSS_MAGFILTER, (DWORD)D3DTEXF_LINEAR);
#if(DIRECT3D_VERSION >= 0x0700)
    if (m_dwVersion >= 0x0700) {
		SetRenderState(D3DRENDERSTATE_LIGHTING, (DWORD)FALSE);
//		SetRenderState(D3DRENDERSTATE_LIGHTENABLE,(DWORD)0);
	}
#endif
	SetRenderState(D3DRENDERSTATE_LASTPIXEL, (DWORD)TRUE);
  	SetRenderState(D3DRENDERSTATE_SRCBLEND, (DWORD) D3DBLEND_SRCALPHA);
	SetRenderState(D3DRENDERSTATE_DESTBLEND, (DWORD) D3DBLEND_INVSRCALPHA);
	SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, (DWORD) TRUE);

    SetTextureStageState(0, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE);
    SetTextureStageState(0, D3DTSS_ALPHAOP, (DWORD)D3DTOP_SELECTARG1);
    
    return true;
}

bool CPalTest::SetDefaultLights(void) {
    D3DLIGHT8   Light;
    HRESULT     hr;

//    m_pLight = CreateLight();

//    if (NULL == m_pLight)
//        return false;

    ZeroMemory(&Light,sizeof(D3DLIGHT8));
    Light.Type           = D3DLIGHT_POINT;
    Light.Diffuse.r        = 0.4f;
    Light.Diffuse.g        = 1.0f;
    Light.Diffuse.b        = 1.0f;
    Light.Diffuse.a        = 1.0f;
    Light.Position.x      = -500.0f;
    Light.Position.y      = 200.0f;
    Light.Position.z      = 200.0f;
    Light.Range           = D3DLIGHT_RANGE_MAX;
    Light.Attenuation0    = 0.9f;
    Light.Attenuation1    = 0.5f;
    Light.Attenuation2    = 0.1f;

//    m_pLight->SetLight(&Light);

//    return SetLight(0,m_pLight);

    hr = m_pDevice->SetLight(0, &Light);
    if (FAILED(hr)) {
        return false;
    }

    hr = m_pDevice->LightEnable(0, TRUE);
	
	return (hr == D3D_OK);
}

