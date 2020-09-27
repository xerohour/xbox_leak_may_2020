//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "srt.h"
//#include "cimageloader.h"

//************************************************************************
// Invalid Test functions

CSRTInvalid::CSRTInvalid()
{
	m_szTestName = TEXT("SRT Invalid");
	m_szCommandKey = TEXT("Invalid");

    // Tell parent that we are the Invalid case
    m_bInvalid = true;

    // Disable ZBuffers
    m_ModeOptions.fZBuffer = false;
}

CSRTInvalid::~CSRTInvalid()
{
}

UINT CSRTInvalid::TestInitialize(void)
{
    UINT Result;

    // Call parent's init 
    Result = CSRT::TestInitialize();

    if (Result != D3DTESTINIT_RUN)
        return Result;

    // Force the Invalid option to be given on command line
    if (!KeySet("Invalid"))
    {
	    WriteToLog("The Invalid test case must be explicitly requested (srt -invalid).\n");
		return D3DTESTINIT_SKIPALL;
    }

    m_pStripes = new DWORD[m_RTWidth];

    for(int i=0; i<m_RTWidth; i++)
        m_pStripes[i] = (i&1) ? 0xFF000000 : 0xFFFFFFFF;

//    CImageLoader Loader;

//    m_pImageData = Loader.LoadStripes(m_RTWidth, m_RTHeight, m_RTWidth, m_pStripes, false, false);

    return D3DTESTINIT_RUN;
}
