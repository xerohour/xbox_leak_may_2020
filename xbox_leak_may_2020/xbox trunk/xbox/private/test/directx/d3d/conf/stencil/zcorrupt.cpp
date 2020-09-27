// FILE:        zcorrupt.cpp
// DESC:        stencil conformance tests
// AUTHOR:      Todd M. Frost
// COMMENTS:    tests for z corruption due to stencil clear operations

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "flags.h"
#include "stencil.h"

// NAME:        CStencilZCorrupt()
// DESC:        stencil z corrupt class constructor
// INPUT:       none
// OUTPUT:      none

CStencilZCorrupt::CStencilZCorrupt()
{
    m_szTestName = TEXT("Z corruption");
    m_szCommandKey = TEXT("ZCorrupt");

    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilZCorrupt()
// DESC:        stencil z corrupt class destructor
// INPUT:       none
// OUTPUT:      none

CStencilZCorrupt::~CStencilZCorrupt()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CStencilZCorrupt::CommandLineHelp(void)
{
    CStencil::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilZCorrupt::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    m_nStipple = (UINT) (m_dwVersion > 0x0600) ? 1 : 2;
    SetTestRange((UINT) 1, (UINT) (NMASKS*m_nStipple*(m_dwStencilBitMask + 1)));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test executed successfully
//              false....otherwise

bool CStencilZCorrupt::ExecuteTest(UINT uTest)
{
    bool bSuccess = false;
    CHAR szStipple[2][13] = {
        "Non-Stippled",
        "    Stippled"
    };
    D3DVALUE dvDepth = 0.0f;
    DWORD dwClear, dwFlags, dwInvert;
    DWORD dwMasks[2];
    UINT i, j, k;

    CStencil::ExecuteTest(uTest);
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = 0xffffffff;

    for (k = 1, i = 0; (i < m_nStipple) && !bSuccess; i++)
        for (j = 0; (j < NMASKS) && !bSuccess; j++)
            for (dwClear = 0; dwClear <= m_dwStencilBitMask && !bSuccess; dwClear++, k++)
                if (k == uTest)
                {
                    sprintf(m_szBuffer, "Clear2() corruption:  0x%08X, 0x%08X, %s", dwClear, dwMasks[j], szStipple[i]);
                	BeginTestCase(m_szBuffer);
                    vStipple(((i == 0) ? false : true));
                    SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD) D3DCMP_EQUAL);
                    SetRenderState(D3DRENDERSTATE_STENCILFUNC, (DWORD) D3DCMP_ALWAYS);
                    SetRenderState(D3DRENDERSTATE_STENCILENABLE, (DWORD) TRUE);
                    SetRenderState(D3DRENDERSTATE_STENCILWRITEMASK, dwMasks[j]);
                    SetRenderState(D3DRENDERSTATE_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRENDERSTATE_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRENDERSTATE_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);
                    dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

                    if (BeginScene(dvDepth, dwClear, m_dwPassColor, dwFlags))
                    {
                        if (EndScene())
                        {
                            dwFlags = D3DCLEAR_TARGET | D3DCLEAR_STENCIL;
                            dwInvert = m_dwStencilBitMask & ~dwClear;

                            if (BeginScene(dvDepth, dwInvert, m_dwPassColor, dwFlags))
                            {
                                SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD) D3DCMP_NOTEQUAL);
                                SetRenderState(D3DRENDERSTATE_STENCILFUNC, (DWORD) D3DCMP_ALWAYS);
                                vFlood(dvDepth, m_dwFailColor);
        
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
