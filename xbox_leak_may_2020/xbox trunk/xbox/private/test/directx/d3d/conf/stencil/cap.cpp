// FILE:        cap.cpp
// DESC:        stencil conformance tests
// AUTHOR:      Todd M. Frost
// COMMENTS:    tests for stencil caps

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "flags.h"
#include "stencil.h"

// NAME:        CStencilCap()
// DESC:        stencil cap class constructor
// INPUT:       none
// OUTPUT:      none

CStencilCap::CStencilCap()
{
    m_szTestName = TEXT("Stencil cap");
    m_szCommandKey = TEXT("Cap");
}

// NAME:        ~CStencilCap()
// DESC:        stencil cap class destructor
// INPUT:       none
// OUTPUT:      none

CStencilCap::~CStencilCap()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CStencilCap::CommandLineHelp(void)
{
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilCap::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();
    SetTestRange((UINT) 1, (UINT) 9);
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest
// OUTPUT:      true.....if test executes successfully
//              false....otherwise

bool CStencilCap::ExecuteTest(UINT uTest)
{
    bool bSuccess = true;

    CStencil::ExecuteTest(uTest);

    switch (uTest)
    {
        case 1:
            BeginTestCase("D3DSTENCILCAPS_KEEP");

            if (D3DSTENCILCAPS_KEEP & m_dwStencilCaps)
                Pass();
            else
            {
                WriteToLog(_T("D3DSTENCILOP_KEEP not supported, required stencil cap."));
                Fail();
            }
        break;

        case 2:
            BeginTestCase("D3DSTENCILCAPS_ZERO");

            if (D3DSTENCILCAPS_ZERO & m_dwStencilCaps)
                Pass();
            else
            {
                WriteToLog(_T("D3DSTENCILOP_ZERO not supported, required stencil cap."));
                Fail();
            }
        break;

        case 3:
            BeginTestCase("D3DSTENCILCAPS_REPLACE");

            if (D3DSTENCILCAPS_REPLACE & m_dwStencilCaps)
                Pass();
            else
            {
                WriteToLog(_T("Failed, D3DSTENCILOP_REPLACE not supported."));
                Fail();
            }
        break;

        case 4:
            BeginTestCase("D3DSTENCILCAPS_INCRSAT");

            if (D3DSTENCILCAPS_INCRSAT & m_dwStencilCaps)
                Pass();
            else
            {
                WriteToLog(_T("D3DSTENCILOP_INCRSAT not supported, required stencil cap."));
                Fail();
            }
        break;

        case 5:
            BeginTestCase("D3DSTENCILCAPS_DECRSAT");

            if (D3DSTENCILCAPS_DECRSAT & m_dwStencilCaps)
                Pass();
            else
            {
                WriteToLog(_T("D3DSTENCILOP_DECRSAT not supported, required stencil cap."));
                Fail();
            }
        break;

        case 6:
            BeginTestCase("D3DSTENCILCAPS_INVERT");

            if (D3DSTENCILCAPS_INVERT & m_dwStencilCaps)
                Pass();
            else
            {
                WriteToLog(_T("D3DSTENCILOP_INVERT not supported, required stencil cap."));
                Fail();
            }
        break;

        case 7:
            BeginTestCase("D3DSTENCILCAPS_INCR");
            if (D3DSTENCILCAPS_INCR & m_dwStencilCaps)
                Pass();
            else
            {
                WriteToLog(_T("D3DSTENCILOP_INCR not supported, but not required."));
                Pass();
            }
        break;

        case 8:
            BeginTestCase("D3DSTENCILCAPS_DECR");

            if (D3DSTENCILCAPS_DECR & m_dwStencilCaps)
                Pass();
            else
            {
                WriteToLog(_T("D3DSTENCILOP_DECR not supported, but not required."));
                Pass();
            }
        break;

        case 9:
            BeginTestCase("Bit depth verification");

            if (m_dwStencilBitDepth == m_dwStencilBitDepthReported)
                Pass();
            else
            {
                WriteToLog(_T("Bit depth verification failed:  %d reported, %d computed."),
                           m_dwStencilBitDepthReported, m_dwStencilBitDepth);
                Fail();
            }
        break;

        default:
            WriteToLog(_T("Invalid test number."));
            Fail();
            bSuccess = false;
        break;
    }

    m_fPassPercentage = (bSuccess) ? 1.0f : 0.0f;
    return bSuccess;
}
