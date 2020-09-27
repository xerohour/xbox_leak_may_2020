// FILE:        clear.cpp
// DESC:        stencil conformance tests (clear)
// AUTHOR:      Todd M. Frost

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "flags.h"
#include "stencil.h"

// NAME:        CStencilClear()
// DESC:        stencil clear class constructor
// INPUT:       none
// OUTPUT:      none

CStencilClear::CStencilClear()
{
    m_szTestName = TEXT("Stencil clear");
    m_szCommandKey = TEXT("Clear");

    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilClear()
// DESC:        stencil clear class destructor
// INPUT:       none
// OUTPUT:      none

CStencilClear::~CStencilClear()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CStencilClear::CommandLineHelp(void)
{
    CStencil::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilClear::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    m_nStipple = (UINT) (m_dwVersion > 0x0600) ? 1 : 2;
    SetTestRange((UINT) 1, (UINT) (NMASKS*m_nStipple*(m_dwStencilBitMask + 1)));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      bSuccess.....success flags

bool CStencilClear::ExecuteTest(UINT uTest)
{
    bool bSuccess = false;
    DWORD dwClear;
    DWORD dwMasks[2];
    UINT i, j, k;

    CStencil::ExecuteTest(uTest);
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = 0xffffffff;

    for (k = 1, i = 0; (i < m_nStipple) && !bSuccess; i++)
        for (j = 0; (j < NMASKS) && !bSuccess; j++)
            for (dwClear = 0; (dwClear <= m_dwStencilBitMask) && !bSuccess; dwClear++, k++)
                if (k == uTest)
                {
                    vTest(((i == 0) ? false : true), dwClear, dwMasks[j]);
                    bSuccess = true;
                }

//    return bSuccess;
    return false;
}

// NAME:        vTest()
// DESC:        perform stencil clear test variation
// INPUT:       bStipple.stipple flag
//              dwClear..stencil plane clear value
//              dwMask...stencil write mask
// OUTPUT:      none

VOID CStencilClear::vTest(bool bStipple, DWORD dwClear, DWORD dwMask)
{
    CHAR szStipple[2][13] = {
        "Non-Stippled",
        "    Stippled"
    };
    D3DVALUE dvDepth = 1.0f;
    DWORD dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
    UINT i = bStipple ? 1 : 0;

    sprintf(m_szBuffer, "Clear2():  0x%08X, 0x%08X, %s", dwClear, dwMask, szStipple[i]);
    BeginTestCase(m_szBuffer);

    vStipple(bStipple);
    SetRenderState(D3DRENDERSTATE_STENCILENABLE, (DWORD) TRUE);
    SetRenderState(D3DRENDERSTATE_STENCILWRITEMASK, dwMask);
    SetRenderState(D3DRENDERSTATE_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
    SetRenderState(D3DRENDERSTATE_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
    SetRenderState(D3DRENDERSTATE_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

    if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
    {
        SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD) D3DCMP_ALWAYS);
        SetRenderState(D3DRENDERSTATE_STENCILFUNC, (DWORD) D3DCMP_EQUAL);
        SetRenderState(D3DRENDERSTATE_STENCILREF, dwClear);
        SetRenderState(D3DRENDERSTATE_STENCILMASK, dwMask); 

        vFlood((D3DVALUE) 0.5, m_dwPassColor);

        if (EndScene())
        {
            if (bCompare())
                Pass();
            else
            {
                WriteToLog(_T("Non pass color detected."));
                Fail();
            }

            m_pDevice->Present(NULL, NULL, NULL, NULL);
        }
        else
        {
            WriteToLog(_T("Unable to end scene, blocked."));
            Fail();
        }
    }
    else
    {
        WriteToLog(_T("Unable to begin scene, blocked."));
        Fail();
    }
}
