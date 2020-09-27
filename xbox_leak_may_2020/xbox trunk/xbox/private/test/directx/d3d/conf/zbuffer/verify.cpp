//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "ZBuffer.h"

//************************************************************************
// Verify Test functions

CZVerifyTest::CZVerifyTest()
{
	m_szTestName = TEXT("Verify Z Buffer Formats");
	m_szCommandKey = TEXT("Verify");
}

CZVerifyTest::~CZVerifyTest()
{
}

bool CZVerifyTest::ExecuteTest(UINT uTestNum)
{
    UINT uTest = (uTestNum - 1) % m_uMaxTests + 1;

	// Initialize some variables
	errString[0] = '\0';
	msgString[0] = '\0';

    if (uTest == 1) {
        CSurface8* pd3ds;
        CSurface8* pd3dsRT;
        D3DSURFACE_DESC d3dsd;
        int nZBufferFormat = (uTestNum - 1) / m_uMaxTests;
        if (FAILED(m_pDevice->GetRenderTarget(&pd3dsRT))) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetRenderTarget failed"));
            SkipTests(m_uMaxTests);
            return false;
        }
        pd3dsRT->GetDesc(&d3dsd);
        pd3dsRT->Release();
        if (FAILED(m_pDevice->CreateDepthStencilSurface(d3dsd.Width, d3dsd.Height, m_fmtdCommon[nZBufferFormat], D3DMULTISAMPLE_NONE, &pd3ds))) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::CreateDepthStencilSurface failed on format %s"), D3DFmtToString(m_fmtdCommon[nZBufferFormat]));
            SkipTests(m_uMaxTests);
            return false;
        }
    	msgString[0] = '\0';
	    sprintf(msgString, "%sZ Buffer Format: %s.\n",msgString,D3DFmtToString(m_fmtdCommon[nZBufferFormat]));
        WriteToLog(msgString);
        if (FAILED(m_pDevice->SetRenderTarget(NULL, pd3ds))) {
            Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetRenderTarget failed on depth format %s"), D3DFmtToString(m_fmtdCommon[nZBufferFormat]));
            SkipTests(m_uMaxTests);
            pd3ds->Release();
            return false;
        }
        pd3ds->Release();
    	msgString[0] = '\0';
    }

//    if (m_dwVersion <= 0x0700)
//    {
	    // Perform some Z buffer validation
//	    bValidFormat = VerifyFormat();
//    }
//    else
//    {
	    // Perform Dx8 Z buffer validation
	    bValidFormat = VerifyFormat8(); 
//    }

	// Tell the log that we are starting
	BeginTestCase(msgString);

    return true;
}

void CZVerifyTest::SceneRefresh(void)
{
}

bool CZVerifyTest::ProcessFrame(void)
{
	char	szBuffer[80];
	static	int nPass = 0;
	static  int nFail = 0;

	// Tell the logfile how they did
	if (bValidFormat)
	{
		(m_fIncrementForward) ? nPass++ : nPass--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
		m_fPassPercentage = 1.0;
		Pass();
	}
	else
	{
		(m_fIncrementForward) ? nFail++ : nFail--;
		sprintf(szBuffer,"$yPass: %d, Fail: %d", nPass, nFail);
		m_fPassPercentage = 0.0;
		Fail();

		// Print any errors to the log
		if (errString[0] != '\0')
			WriteToLog(errString);
	}

	WriteStatus("$gTest Options$y",msgString);
	WriteStatus("$gOverall Results",szBuffer);

    return bValidFormat;
}
/*
bool CZVerifyTest::VerifyFormat(void)
{
    DDSURFACEDESC2	ddsd2;
	m_pSrcZBuffer->GetSurfaceDescription(&ddsd2);

	DDPIXELFORMAT *pDDPF;
	pDDPF = &(ddsd2.ddpfPixelFormat);

    bool bError				= false;
    DWORD dwFlags			= pDDPF->dwFlags;
    DWORD dwTotalBitDepth	= pDDPF->dwZBufferBitDepth;
    DWORD dwStencilBitDepth	= pDDPF->dwStencilBitDepth;
    DWORD dwZBitMask		= pDDPF->dwZBitMask;
    DWORD dwStencilBitMask	= pDDPF->dwStencilBitMask;
	DWORD dwZBitCount		= 0;
	DWORD dwStencilBitCount	= 0;
    DWORD mask;

    // Count number of bits for each component
    for (mask = pDDPF->dwZBitMask; mask; mask>>=1)
        dwZBitCount += (mask & 0x1);
    for (mask = pDDPF->dwStencilBitMask; mask; mask>>=1)
        dwStencilBitCount += (mask & 0x1);

   	// Report the format
	sprintf(msgString,"%s%d-bit Z Buffer, Z: %d-bit (0x%x)",msgString,dwTotalBitDepth,dwZBitCount,dwZBitMask);

    if (dwFlags & PF_STENCILBUFFER)
	{
		sprintf(msgString,"%s, Stencil: %d-bit (0x%x)",msgString,dwStencilBitCount,dwStencilBitMask);
	}

	// Check for invalid formats
    if (dwZBitCount != (dwTotalBitDepth - dwStencilBitDepth))
	{
		sprintf(errString,"%sError: ZBitMask inconsistant with bit depth value!\n",errString);
		bError = true;
	}
    if (dwStencilBitCount != dwStencilBitDepth)
	{
		sprintf(errString,"%sError: StencilBitMask inconsistant with bit depth value!\n",errString);
		bError = true;
	}
    if (!(dwFlags & PF_ZBUFFER))
	{
		sprintf(errString,"%sError: DDPF_ZBUFFER not set on enumerated Z buffer!\n",errString);
		bError = true;
	}

	return (!bError);
}
*/
