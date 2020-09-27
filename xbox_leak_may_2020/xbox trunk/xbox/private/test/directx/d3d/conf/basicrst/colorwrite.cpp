//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "BasicRst.h"

//************************************************************************
// ColorWrite Test functions

CColorWriteTest::CColorWriteTest()
{
	m_szTestName = TEXT("BasicRst ColorWrite");
	m_szCommandKey = TEXT("ColorWrite");

    pTempSurface = NULL;
    pPrevSurface = NULL;
}

CColorWriteTest::~CColorWriteTest()
{
}

UINT CColorWriteTest::TestInitialize(void)
{
//    UINT n = m_pMode->nSrcDevice;

	// Setup the Test range
	SetTestRange(1,COLORWRITE_TESTS * 3);

	// Make sure we are on at lease DX8
	if (m_dwVersion <= 0x0700)
	{
		WriteToLog("This test requires at least Dx8.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Check the caps we are interested in
	DWORD dwMiscCaps = m_d3dcaps.PrimitiveMiscCaps;

	// Check the ColorWriteEnable cap
	if (!(dwMiscCaps & D3DPMISCCAPS_COLORWRITEENABLE))
	{
		WriteToLog("Device capability not supported: Misc ColorWriteEnable.\n");
		return D3DTESTINIT_SKIPALL;
	}

     // Build up a temporary buffer with alpha channel
    if (CreateDestBuffer())
		bDestBuffer = true;
    else
        bDestBuffer = false;

    // Tell RenderPrimitive disable API cycling
    // m_dwPrimitives = 1; should be changed to something like 
    // SetAPI(RP_DP);

	return D3DTESTINIT_RUN;
}

bool CColorWriteTest::ClearFrame(void)
{
    return true;
}

bool CColorWriteTest::ExecuteTest(UINT uTestNum)
{
    int     nTest, nGroup;

	// Initialize some variables
    nTest = ((uTestNum-1) % COLORWRITE_TESTS) + 1;
    nGroup = (uTestNum-1) / COLORWRITE_TESTS;
 	dwVertexCount = 0;
	szClear[0] = '\0';
	szColor[0] = '\0';
	msgString[0] = '\0';

	// Set the render target to render to our temporary surface.
    if (bDestBuffer)
    {
        if (FAILED(m_pDevice->SetRenderTarget(pTempSurface, NULL)))
//	    if (!SetRenderTarget(pTempSurface, false, m_pSrcZBuffer))
	    {
		    WriteToLog("SetRenderTarget() to TempSurface failed\n");
		    RELEASE(pTempSurface);
//		    TestFinish(D3DTESTFINISH_ABORT);
		    return false;
	    }
    }

	// Let's build some triangles.
	DrawColorWriteTests();

    // Update clear color for logfile and status window
    if (nGroup == 0)
    {
        Clear(RGBA_MAKE(0,0,0,0));
        sprintf(szClear,"RGBA(0,0,0,0)");
    	sprintf(msgString, "%sClear: RGBA(0,0,0,0)", msgString);
    }
    else if (nGroup == 1)
    {
        Clear(RGBA_MAKE(128,128,128,128));
        sprintf(szClear,"RGBA(128,128,128,128)");
    	sprintf(msgString, "%sClear: RGBA(128,128,128,128)", msgString);
    }
    else // nGroup == 2
    {
        Clear(RGBA_MAKE(255,255,255,255));
        sprintf(szClear,"RGBA(255,255,255,255)");
    	sprintf(msgString, "%sClear: RGBA(255,255,255,255)", msgString);
    }

    // By default, don't check alpha values
    bAlphaMask = false;

	// Set the appropriate renderstate
	switch (nTest)
	{
		case (1):
			sprintf(msgString, "%s, ColorWrite: Default", msgString);
            sprintf(szColor,"Default");
            bAlphaMask = true;
			break;
		case (2):
			sprintf(msgString, "%s, ColorWrite: Red", msgString);
            sprintf(szColor,"Red");
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED);
			break;
		case (3):
			sprintf(msgString, "%s, ColorWrite: Green", msgString);
            sprintf(szColor,"Green");
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_GREEN);
			break;
		case (4):
			sprintf(msgString, "%s, ColorWrite: Blue", msgString);
            sprintf(szColor,"Blue");
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_BLUE);
			break;
		case (5):
			sprintf(msgString, "%s, ColorWrite: Alpha", msgString);
            sprintf(szColor,"Alpha");
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (6):
			sprintf(msgString, "%s, ColorWrite: Red|Green", msgString);
            sprintf(szColor,"Red|Green");
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN);
			break;
		case (7):
			sprintf(msgString, "%s, ColorWrite: Red|Blue", msgString);
            sprintf(szColor,"Red|Blue");
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_BLUE);
			break;
		case (8):
			sprintf(msgString, "%s, ColorWrite: Red|Alpha", msgString);
            sprintf(szColor,"Red|Alpha");
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (9):
			sprintf(msgString, "%s, ColorWrite: Green|Blue", msgString);
            sprintf(szColor,"Green|Blue");
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);
			break;
		case (10):
			sprintf(msgString, "%s, ColorWrite: Green|Alpha", msgString);
            sprintf(szColor,"Green|Alpha");
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (11):
			sprintf(msgString, "%s, ColorWrite: Blue|Alpha", msgString);
            sprintf(szColor,"Blue|Alpha");
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (12):
			sprintf(msgString, "%s, ColorWrite: Red|Green|Blue", msgString);
            sprintf(szColor,"Red|Green|Blue");
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE);
			break;
		case (13):
			sprintf(msgString, "%s, ColorWrite: Red|Green|Alpha", msgString);
            sprintf(szColor,"Red|Green|Alpha");
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (14):
			sprintf(msgString, "%s, ColorWrite: Red|Blue|Alpha", msgString);
            sprintf(szColor,"Red|Blue|Alpha");
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (15):
			sprintf(msgString, "%s, ColorWrite: Green|Blue|Alpha", msgString);
            sprintf(szColor,"Green|Blue|Alpha");
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
			break;
		case (16):
			sprintf(msgString, "%s, ColorWrite: Red|Green|Blue|Alpha", msgString);
            sprintf(szColor,"Red|Green|Blue|Alpha");
            bAlphaMask = true;
			SetRenderState(D3DRS_COLORWRITEENABLE, (DWORD)D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
			break;
	}

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CColorWriteTest::SceneRefresh(void)
{
	if (BeginScene())
	{
        HRESULT hr;

        // Render the data.
		RenderPrimitive(D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,VertexList,dwVertexCount,NULL,0,0);

		EndScene();

        if (bDestBuffer)
        {
            D3DLOCKED_RECT d3dlr;

            // Load the image back to the original back buffer
            hr = D3DXLoadSurfaceFromSurface(
		             pPrevSurface->GetIDirect3DSurface8(),
		             NULL, 
                     NULL,					    
			         pTempSurface->GetIDirect3DSurface8(), 
			         NULL,					    
			         NULL,					   
			         D3DX_FILTER_POINT, 	  
                     0);

            if (FAILED(hr))
            {
                WriteToLog("Source D3DXLoadSurfaceFromSurface() failed with HResult = %X.\n",hr);
            }

            pPrevSurface->LockRect(&d3dlr, NULL, 0);
            pPrevSurface->UnlockRect();

	        // Set the render target to our original surface
            if (FAILED(m_pDevice->SetRenderTarget(pPrevSurface, NULL)))
//	        if (!SetRenderTarget(pPrevSurface, false, m_pSrcZBuffer))
	        {
		        WriteToLog("SetRenderTarget() back to PrevSurface failed\n");
	        }
        }
	}	
}

bool CColorWriteTest::ProcessFrame(void)
{
	char	szBuffer[80];
    DWORD   dwAlphaValue;
    bool	bCompareResult;
    bool    bAlphaResult = true;
	static	int nPass = 0;
	static  int nFail = 0;

     // Use the standard 15%
    bCompareResult = GetCompareResult(0.15f, 0.78f, 0);

    if (bDestBuffer)
    {
	    RECT	rSurf;
	    DWORD	dwColor;
	    void	*pBuf, *pColor;
        D3DLOCKED_RECT d3dlr;

	    // Build the subrect we want
	    rSurf.top = 10;
	    rSurf.left = 10;
	    rSurf.bottom = 270;
	    rSurf.right = 310;

	    // Get the Surface data pointer for our subrect
        if (FAILED(pTempSurface->LockRect(&d3dlr, &rSurf, 0))) {
            pBuf = NULL;
        }
        else {
            pBuf = d3dlr.pBits;
        }
//        pBuf = pTempSurface->Lock(&rSurf);

        if (pBuf == NULL)
	    {
		    WriteToLog("ProcessFrame: Lock failed on back buffer.");
            return false;
	    }

	    // Look for Alpha pixel matches
	    for (int j=0; j < 260; j++)		// Height loop
	    {
		    pColor = (char*)pBuf + (d3dlr.Pitch * j);

		    for (int i=0; i < 300; i++)	// Width loop
		    {
//                DWORD dwFilter = (pTempSurface->m_dwAlphaMask << pTempSurface->m_nAlphaShift) | pTempSurface->m_dwFilter;
			    DWORD dwAlpha, dwClearAlpha;

//			    dwColor = pTempSurface->ReadColor(&pColor);
                dwColor = *(LPDWORD)pColor;

			    dwAlpha = RGBA_GETALPHA(dwColor);
			    dwClearAlpha = RGBA_GETALPHA(m_dwClearColor);

                if (bAlphaMask)
                {
                    if (dwAlpha != 240)
                    {
                        bAlphaResult = false;
                        dwAlphaValue = dwAlpha;
                    }
                }
                else
                {
                    if (dwAlpha != dwClearAlpha)
                    {
                        bAlphaResult = false;
                        dwAlphaValue = dwAlpha;
                    }
                }
		    }
	    }

        if (FAILED(pTempSurface->UnlockRect()))
//        if (!pTempSurface->Unlock())
 	    {
		    WriteToLog("ProcessFrame: Unlock failed on back buffer.");
            return false;
	    }
    }

	// Tell the logfile how the compare went
    if (bCompareResult && bAlphaResult)
    {
		(m_fIncrementForward) ? nPass++ : nPass--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Pass();
    }
	else if (!bAlphaResult)
    {
        if (bAlphaMask)
		    WriteToLog("Found invalid Alpha value (%d, expected 240) on destination surface.\n",dwAlphaValue);
        else
		    WriteToLog("Found invalid Alpha value (%d, expected %d) on destination surface.\n",dwAlphaValue,RGBA_GETALPHA(m_dwClearColor));

		(m_fIncrementForward) ? nFail++ : nFail--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Fail();
    }
    else
	{
		(m_fIncrementForward) ? nFail++ : nFail--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
        Fail();
	}

    // Update the Status window
	WriteStatus("$gColorWriteEnable$y",szColor);
	WriteStatus("$gClear Color$y",szClear);
	WriteStatus("$gOverall Results",szBuffer);

    return (bCompareResult && bAlphaResult);
}

bool CColorWriteTest::TestTerminate()
{
	// Cleanup buffers that we created
	RELEASE(pTempSurface);
	RELEASE(pPrevSurface);

	return true;
}

bool CColorWriteTest::CreateDestBuffer(void)
{
    HRESULT hr;
    D3DSURFACE_DESC Desc;

    // Get the render target
    hr = m_pDevice->GetRenderTarget(&pPrevSurface);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetRenderTarget"))) {
        return false;
    }

    pPrevSurface->GetDesc(&Desc);

    // Create a temp surface for rendering
//    pTempSurface = new CDirectDrawSurface(this);

//    if (NULL == pTempSurface)
//	{
//		WriteToLog("Initialize failed\n");
//		RELEASE(pTempSurface);
//		return false;
//	}

//    m_pSrcTarget->GetSurfaceDescription(&Desc);

    hr = m_pSrcDevice8->CreateRenderTarget(Desc.Width, 
                                           Desc.Height,
                                           D3DFMT_LIN_A8R8G8B8,
                                           D3DMULTISAMPLE_NONE,
                                           TRUE,
                                           &pTempSurface); 
    if (FAILED(hr))
    {
	    WriteToLog("CreateRenderTarget() failed with HResult = %0x%X.\n",hr);
		RELEASE(pPrevSurface);
		return false;
	}


    // Get and process surface description 
    if (FAILED(pTempSurface->GetDesc(&Desc)))
//    if (pTempSurface->GetSurfaceDescription(&Desc))
//        pTempSurface->ProcessSurfaceDescription(&Desc);
//    else
    {
	    WriteToLog("Failed to get texture surface description.\n");
		RELEASE(pPrevSurface);
		RELEASE(pTempSurface);
		return false;
    }

	// Save a pointer to the previous surface
//	pPrevSurface = m_pSrcTarget;
//    pPrevSurface->AddRef();

	// Set the render target to render to our temporary surface.
    if (FAILED(m_pDevice->SetRenderTarget(pTempSurface, NULL)))
//	if (!SetRenderTarget(pTempSurface, false, m_pSrcZBuffer))
	{
		WriteToLog("SetRenderTarget() to TempSurface failed\n");
		RELEASE(pTempSurface);
		return false;
	}

    return true;
}

//************************************************************************
// Internal API:    DrawColorWriteTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CColorWriteTest::DrawColorWriteTests(void)
{
    D3DVECTOR v0 = { 10.0f, 10.0f, 0.5f};
    D3DVECTOR v1 = {160.0f, 10.0f, 0.5f};
    D3DVECTOR v2 = { 10.0f,270.0f, 0.5f};
    D3DVECTOR v3 = { 10.0f,270.0f, 0.5f};
    D3DVECTOR v4 = {160.0f, 10.0f, 0.5f};
    D3DVECTOR v5 = {160.0f,270.0f, 0.5f};
    D3DVECTOR v6 = {160.0f,270.0f, 0.5f};
    D3DVECTOR v7 = {160.0f, 10.0f, 0.5f};
    D3DVECTOR v8 = {310.0f,270.0f, 0.5f};
    D3DVECTOR v9 = {160.0f, 10.0f, 0.5f};
    D3DVECTOR v10 = {310.0f, 10.0f, 0.5f};
    D3DVECTOR v11 = {310.0f,270.0f, 0.5f};

    // RGB Triangle
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v0, (1.0f / 0.5f), RGBA_MAKE(0,255,0,240), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v1, (1.0f / 0.5f), RGBA_MAKE(0,0,255,240), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v2, (1.0f / 0.5f), RGBA_MAKE(255,0,0,240), 0, 0.0f,1.0f);

    // White Triangle
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v3, (1.0f / 0.5f), RGBA_MAKE(255,255,255,240), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v4, (1.0f / 0.5f), RGBA_MAKE(255,255,255,240), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v5, (1.0f / 0.5f), RGBA_MAKE(255,255,255,240), 0, 0.0f,1.0f);

    // Black Triangle
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v6, (1.0f / 0.5f), RGBA_MAKE(0,0,0,240), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v7, (1.0f / 0.5f), RGBA_MAKE(0,0,0,240), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v8, (1.0f / 0.5f), RGBA_MAKE(0,0,0,240), 0, 0.0f,1.0f);

    // CMY Triangle
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v9,  (1.0f / 0.5f), RGBA_MAKE(0,255,255,240), 0, 0.5f,0.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v10, (1.0f / 0.5f), RGBA_MAKE(255,0,255,240), 0, 1.0f,1.0f);
	VertexList[dwVertexCount++] = cD3DTLVERTEX(v11, (1.0f / 0.5f), RGBA_MAKE(255,255,0,240), 0, 0.0f,1.0f);
}
