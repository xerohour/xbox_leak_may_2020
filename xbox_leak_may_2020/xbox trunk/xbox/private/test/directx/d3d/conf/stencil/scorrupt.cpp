// FILE:        scorrupt.cpp
// DESC:        stencil conformance tests
// AUTHOR:      Todd M. Frost
// COMMENTS:    tests for stencil corruption due to z clear/write operations

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "flags.h"
#include "stencil.h"

const UINT CLEAR2_ENABLED  = 0;
const UINT ZWRITE_ENABLED  = 1;
const UINT CLEAR2_DISABLED = 2;
const UINT ZWRITE_DISABLED = 3;

const UINT NCASES          = 4;

// NAME:        CStencilSCorrupt()
// DESC:        stencil s corrupt class constructor
// INPUT:       none
// OUTPUT:      none

CStencilSCorrupt::CStencilSCorrupt()
{
    m_szTestName = TEXT("Stencil corruption");
    m_szCommandKey = TEXT("SCorrupt");

    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilSCorrupt()
// DESC:        stencil s corrupt class destructor
// INPUT:       none
// OUTPUT:      none

CStencilSCorrupt::~CStencilSCorrupt()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CStencilSCorrupt::CommandLineHelp(void)
{
    CStencil::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilSCorrupt::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    m_nStipple = (UINT) (m_dwVersion > 0x0600) ? 1 : 2;
    SetTestRange((UINT) 1, (UINT) (NCASES*NMASKS*m_nStipple*(m_dwStencilBitMask + 1)));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test executed successfully
//              false....otherwise

bool CStencilSCorrupt::ExecuteTest(UINT uTest)
{
    bool bSuccess = false;
    CHAR szLabel[NCASES][19] = {
        "Clear2(), enabled",
        "Z write, enabled",
        "Clear2(), disabled",
        "Z write, disabled"
    };
    CHAR szStipple[2][13] = {
        "Non-Stippled",
        "    Stippled"
    };
    D3DVALUE dvDepth = 1.0f;
    DWORD dwCase, dwClear, dwFlags;
    DWORD dwEnable[NCASES] = {
        (DWORD) TRUE,
        (DWORD) TRUE,
        (DWORD) FALSE,
        (DWORD) FALSE
    };
    DWORD dwMasks[2];
    UINT i, j, k;

    CStencil::ExecuteTest(uTest);
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = 0xffffffff;

    for (k = 1, i = 0; i < m_nStipple; i++)
        for (j = 0; j < NMASKS; j++)
            for (dwClear = (DWORD) 0; dwClear <= m_dwStencilBitMask; dwClear++)
                for (dwCase = (DWORD) 0; dwCase < NCASES; dwCase++, k++)
                    if (k == uTest)
                    {
                        sprintf(m_szBuffer, "%s:  0x%08X, 0x%08X, %s", szLabel[dwCase], dwClear, dwMasks[j], szStipple[i]);
                        BeginTestCase(m_szBuffer);
                        vStipple(((i == 0) ? false : true));
                        SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD) D3DCMP_ALWAYS);
                        SetRenderState(D3DRENDERSTATE_STENCILENABLE, (DWORD) TRUE);
                        SetRenderState(D3DRENDERSTATE_STENCILWRITEMASK, dwMasks[j]);
                        SetRenderState(D3DRENDERSTATE_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                        SetRenderState(D3DRENDERSTATE_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                        SetRenderState(D3DRENDERSTATE_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);
                        dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

                        if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                        {
                            if (EndScene())
                            {
                                dwFlags = D3DCLEAR_TARGET;

                                if ((CLEAR2_ENABLED == dwCase) || (CLEAR2_DISABLED == dwCase))
                                    dwFlags |= D3DCLEAR_ZBUFFER;
                        
                                SetRenderState(D3DRENDERSTATE_STENCILENABLE, dwEnable[dwCase]);

                                if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                                {
                                    if ((ZWRITE_ENABLED == dwCase) || (ZWRITE_DISABLED == dwCase))
                                        vFlood((D3DVALUE) 0.0, m_dwFailColor);

                                    SetRenderState(D3DRENDERSTATE_STENCILENABLE, (DWORD) TRUE);
                                    SetRenderState(D3DRENDERSTATE_STENCILFUNC, (DWORD) D3DCMP_EQUAL);
                                    SetRenderState(D3DRENDERSTATE_STENCILREF, dwClear);
                                    SetRenderState(D3DRENDERSTATE_STENCILMASK, dwMasks[j]);
                                    vFlood((D3DVALUE) 0.0, m_dwPassColor);

                                    if (EndScene())
                                    {
                                        if (bCompare())
                                            Pass();
                                        else
                                        {
                                            WriteToLog(_T("Invalid pixel detected."));
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

                        bSuccess = true;
                    }

//    return bSuccess;
    return false;
}
