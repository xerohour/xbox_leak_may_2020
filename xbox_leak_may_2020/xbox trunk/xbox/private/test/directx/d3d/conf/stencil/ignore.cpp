// FILE:        ignore.cpp
// DESC:        stencil conformance tests (ignore)
// AUTHOR:      Todd M. Frost
// COMMENTS:    verifies stencil tests aren't passing because stencil states are being ignored

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "flags.h"
#include "stencil.h"

// NAME:        CStencilIgnore()
// DESC:        stencil ignore class constructor
// INPUT:       none
// OUTPUT:      none

CStencilIgnore::CStencilIgnore()
{
    m_szTestName = TEXT("Stencil ignore");
    m_szCommandKey = TEXT("Ignore");

    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilIgnore()
// DESC:        stencil ignore class destructor
// INPUT:       none
// OUTPUT:      none

CStencilIgnore::~CStencilIgnore()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CStencilIgnore::CommandLineHelp(void)
{
    CStencil::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilIgnore::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    m_nStipple = (UINT) (m_dwVersion > 0x0600) ? 1 : 2;
    SetTestRange((UINT) 1, (UINT) (NMASKS*m_nStipple*2));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest........test number
// OUTPUT:      bSuccess.....success flags

bool CStencilIgnore::ExecuteTest(UINT uTest)
{
    bool bSuccess = false;
    DWORD dwClear[2];
    DWORD dwMasks[2];
    UINT i, j, k, uTemp;

    CStencil::ExecuteTest(uTest);
    dwClear[0] = (DWORD) 0;
    dwClear[1] = m_dwStencilBitMask;
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = (DWORD) 0xffffffff;

    for (uTemp = 1, i = 0; (i < m_nStipple) && !bSuccess; i++)
        for (j = 0; (j < NMASKS) && !bSuccess; j++)
            for (k = 0; (k < 2) && !bSuccess; k++, uTemp++)
                if (uTemp == uTest)
                {
                    vTest(((i == 0) ? false : true), dwClear[k], dwMasks[j]);
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

VOID CStencilIgnore::vTest(bool bStipple, DWORD dwClear, DWORD dwMask)
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

    if (BeginScene(dvDepth, dwClear, m_dwPassColor, dwFlags))
    {
        SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD) D3DCMP_ALWAYS);
        SetRenderState(D3DRENDERSTATE_STENCILFUNC, (DWORD) D3DCMP_NOTEQUAL);
        SetRenderState(D3DRENDERSTATE_STENCILREF, dwClear);
        SetRenderState(D3DRENDERSTATE_STENCILMASK, dwMask); 

        vFlood((D3DVALUE) 0.5, m_dwFailColor);

        if (EndScene())
        {
            if (bCompare())
                Pass();
            else
            {
                WriteToLog(_T("Non pass color detected."));
                Fail();
            }

//            DisplayFrame();

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
