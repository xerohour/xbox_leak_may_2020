//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "ZBuffer.h"

//************************************************************************
// Print Test functions

CZPrintTest::CZPrintTest()
{
	m_szTestName = TEXT("Print Z Buffer Formats");
	m_szCommandKey = TEXT("Print");

    // Tell Parent that we are the Print class
    bPrint = true;
}

CZPrintTest::~CZPrintTest()
{
}

UINT CZPrintTest::TestInitialize(void)
{
	// Reset the Test range 
	SetTestRange(1, m_uCommonZBufferFormats);

	// Print out all source Z buffer formats
	for (int i=0; i < (int)m_uCommonZBufferFormats; i++)
	{
//        if (m_dwVersion <= 0x0700)
//    		PrintFormat(&m_pAdapter->Devices[m_pMode->nSrcDevice].ZBufferFormat[i].ddpfPixelFormat, i+1);
//        else
            WriteToLog("Z Buffer Format %02d: %s\n",i+1,D3DFmtToString(m_fmtdCommon[i]));
	}

	return D3DTESTINIT_SKIPALL;
}
/*
void CZPrintTest::PrintFormat(const PIXELFORMAT *pDDPF, int nFormat)
{
    DWORD dwFlags			= pDDPF->dwFlags;
    DWORD dwTotalBitDepth	= pDDPF->dwZBufferBitDepth;
    DWORD dwStencilBitDepth	= pDDPF->dwStencilBitDepth;
    DWORD dwZBitMask		= pDDPF->dwZBitMask;
    DWORD dwStencilBitMask	= pDDPF->dwStencilBitMask;
	DWORD dwZBitCount		= 0;
	DWORD dwStencilBitCount	= 0;
	TCHAR szBuffer[80];
    DWORD mask;

    // Count number of bits for each component
    for (mask = pDDPF->dwZBitMask; mask; mask>>=1)
        dwZBitCount += (mask & 0x1);
    for (mask = pDDPF->dwStencilBitMask; mask; mask>>=1)
        dwStencilBitCount += (mask & 0x1);

	// Report the format
	sprintf(szBuffer,"Z Buffer Format %02d, Z: %d-bit (0x%x)",nFormat,dwZBitCount,dwZBitMask);

    if (dwFlags & PF_STENCILBUFFER)
	{
		sprintf(szBuffer,"%s, Stencil: %d-bit (0x%x)",szBuffer,dwStencilBitCount,dwStencilBitMask);
	}

	sprintf(szBuffer, "%s.\n", szBuffer);
	WriteToLog(szBuffer);

    // Check for invalid fields
    if (dwZBitCount != (dwTotalBitDepth - dwStencilBitDepth))
	{
		WriteToLog("Error: ZBitMask inconsistant with bit depth value!\n");
	}
    if (dwStencilBitCount != dwStencilBitDepth)
	{
		WriteToLog("Error: StencilBitMask inconsistant with bit depth value!\n");
	}
    if (!(dwFlags & PF_ZBUFFER))
	{
		WriteToLog("Error: DDPF_ZBUFFER not set on enumerated Z buffer!\n");
	}

	// Print the pixel description
	WriteToLog("PixelFormat:\n");
	WriteToLog("\tdwSize: %d.\n",pDDPF->dwSize);
	WriteToLog("\tdwFlags: 0x%x.\n",pDDPF->dwFlags);
	WriteToLog("\tdwZBufferBitDepth: %d.\n",pDDPF->dwZBufferBitDepth);
	WriteToLog("\tdwStencilBitDepth: %d.\n",pDDPF->dwStencilBitDepth);
	WriteToLog("\tdwZBitMask: %#010x.\n",pDDPF->dwZBitMask);
	WriteToLog("\tdwStencilBitMask: %#010x.\n\n",pDDPF->dwStencilBitMask);
}
*/
bool CZPrintTest::ExecuteTest(UINT uTestNum)
{
    return true;
}
