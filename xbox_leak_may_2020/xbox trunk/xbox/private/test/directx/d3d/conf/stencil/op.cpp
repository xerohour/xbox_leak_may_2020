// FILE:        op.cpp
// DESC:        stencil plane op conformance tests
// AUTHOR:      Todd M. Frost
// COMMENTS:    tests for stencil ops (D3DSTENCILOP)

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "flags.h"
#include "stencil.h"

const UINT NSTENCILSTATES = 3;//(D3DRENDERSTATE_STENCILPASS - D3DRENDERSTATE_STENCILFAIL + 1);
const UINT NSTENCILOPS    = 8;//(D3DSTENCILOP_DECR - D3DSTENCILOP_KEEP + 1);

// NAME:        CStencilOp()
// DESC:        stencil op class constructor
// INPUT:       none
// OUTPUT:      none

CStencilOp::CStencilOp()
{
    m_szTestName = TEXT("Stencil op");
    m_szCommandKey = TEXT("Op");

    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilOp()
// DESC:        stencil op class destructor
// INPUT:       none
// OUTPUT:      none

CStencilOp::~CStencilOp()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CStencilOp::CommandLineHelp(void)
{
    CStencil::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilOp::TestInitialize(VOID)
{
    UINT uResult = CStencil::TestInitialize();

    m_nStipple = (UINT) (m_dwVersion > 0x0600) ? 1 : 2;
    SetTestRange((UINT) 1, (UINT) (NMASKS*m_nStipple*NSTENCILSTATES*NSTENCILOPS));
    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test executed successfully
//              false....otherwise

bool CStencilOp::ExecuteTest(UINT uTest)
{
    bool bFail = false, bSuccess = false, bWrap;
    CHAR szStipple[2][13] = {
        "Non-Stippled",
        "    Stippled"
    };
    D3DVALUE dvDepth = 1.0f;
    DWORD dwOp, dwState;
    DWORD dwMasks[2];
    UINT i, j, k, n, o, p;
    DWORD dwClear, dwIterations;
    DWORD dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
    DWORD dwReference[2], dwStates[5];
    DWORD dwStencilOp[] = {
            D3DSTENCILOP_KEEP,
            D3DSTENCILOP_ZERO,
            D3DSTENCILOP_REPLACE,
            D3DSTENCILOP_INCRSAT,
            D3DSTENCILOP_DECRSAT,
            D3DSTENCILOP_INVERT,
            D3DSTENCILOP_INCR,
            D3DSTENCILOP_DECR,
    };
    DWORD dwStencilState[] = {
            D3DRS_STENCILFAIL,
            D3DRS_STENCILZFAIL,
            D3DRS_STENCILPASS,
    };

    CStencil::ExecuteTest(uTest);
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = 0xffffffff;

    for (k = 1, i = 0; (i < m_nStipple) && !bSuccess; i++) {
        for (j = 0; (j < NMASKS) && !bSuccess; j++) {
//            for (dwState = D3DRENDERSTATE_STENCILFAIL; dwState <= D3DRENDERSTATE_STENCILPASS; dwState++)
            for (p = 0; p <= 2; p++) {
                dwState = dwStencilState[p];
//                for (dwOp = D3DSTENCILOP_KEEP; dwOp <= D3DSTENCILOP_DECR; dwOp++, k++)
                for (o = 0; o <= 7; o++) {
                    dwOp = dwStencilOp[o];
                    if (k == uTest)
                    {
                        vStipple(((i == 0) ? false : true));

                        switch (dwState)
                        {
                            case D3DRENDERSTATE_STENCILFAIL:
                                dwStates[0] = D3DCMP_ALWAYS;
                                dwStates[1] = D3DCMP_NEVER;
                                dwStates[2] = dwOp;
                                dwStates[3] = D3DSTENCILOP_KEEP;
                                dwStates[4] = D3DSTENCILOP_KEEP;
                            break;

                            case D3DRENDERSTATE_STENCILZFAIL:
                                dwStates[0] = D3DCMP_NEVER;
                                dwStates[1] = D3DCMP_ALWAYS;
                                dwStates[2] = D3DSTENCILOP_KEEP;
                                dwStates[3] = dwOp;
                                dwStates[4] = D3DSTENCILOP_KEEP;
                            break;

                            case D3DRENDERSTATE_STENCILPASS:
                                dwStates[0] = D3DCMP_ALWAYS;
                                dwStates[1] = D3DCMP_ALWAYS;
                                dwStates[2] = D3DSTENCILOP_KEEP;
                                dwStates[3] = D3DSTENCILOP_KEEP;
                                dwStates[4] = dwOp;
                            break;

                            default:
                                WriteToLog(_T("Unknown render state, blocked."));
                                Fail();
                                return false;
                            break;
                        }

                        switch (dwOp)
                        {
                            case D3DSTENCILOP_KEEP:
                                dwClear = m_dwStencilBitMask & 0x55555555;
                                dwIterations = (DWORD) 1;
                                bWrap = false;
                                dwReference[0] = dwClear;
                                dwReference[1] = (DWORD) 0;
                            break;

                            case D3DSTENCILOP_ZERO:
                                dwClear = m_dwStencilBitMask;
                                dwIterations = (DWORD) 1;
                                bWrap = false;
                                dwReference[0] = (DWORD) 0;
                                dwReference[1] = (DWORD) 0;
                            break;

                            case D3DSTENCILOP_REPLACE:
                                dwClear = m_dwStencilBitMask;
                                dwIterations = (DWORD) 1;
                                bWrap = false;
                                dwReference[0] = (DWORD) 0;
                                dwReference[1] = (DWORD) 0;
                            break;

                            case D3DSTENCILOP_INCRSAT:
                                dwClear = (DWORD) 0;
                                dwIterations = m_dwStencilBitMask;
                                bWrap = true;
                                dwReference[0] = m_dwStencilBitMask;
                                dwReference[1] = m_dwStencilBitMask;
                            break;

                            case D3DSTENCILOP_DECRSAT:
                                dwClear = m_dwStencilBitMask;
                                dwIterations = m_dwStencilBitMask;
                                bWrap = true;
                                dwReference[0] = (DWORD) 0;
                                dwReference[1] = (DWORD) 0;
                            break;

                            case D3DSTENCILOP_INVERT:
                                dwClear = m_dwStencilBitMask & 0x55555555;
                                dwIterations = (DWORD) 1;
                                bWrap = false;
                                dwReference[0] = m_dwStencilBitMask & 0xaaaaaaaa;
                                dwReference[1] = (DWORD) 0;
                            break;

                            case D3DSTENCILOP_INCR:
                                dwClear = (DWORD) 0;
                                dwIterations = m_dwStencilBitMask;
                                bWrap = true;
                                dwReference[0] = m_dwStencilBitMask;
                                dwReference[1] = (DWORD) 0;
                            break;

                            case D3DSTENCILOP_DECR:
                                dwClear = m_dwStencilBitMask;
                                dwIterations = m_dwStencilBitMask;
                                bWrap = true;
                                dwReference[0] = (DWORD) 0;
                                dwReference[1] = m_dwStencilBitMask;
                            break;

                            default:
                                WriteToLog(_T("Unknown stencil op, blocked."));
                                Fail();
                                return false;
                            break;         
                        }

                        sprintf(m_szBuffer, "%s, %s, %s", lpszGetOp(dwOp), lpszGetState(dwState), szStipple[i]);
                        BeginTestCase(m_szBuffer);
                        SetRenderState(D3DRENDERSTATE_ZFUNC, dwStates[0]);
                        SetRenderState(D3DRENDERSTATE_STENCILENABLE, (DWORD) TRUE);
                        SetRenderState(D3DRENDERSTATE_STENCILFUNC, dwStates[1]);
                        SetRenderState(D3DRENDERSTATE_STENCILFAIL, dwStates[2]);
                        SetRenderState(D3DRENDERSTATE_STENCILZFAIL, dwStates[3]);
                        SetRenderState(D3DRENDERSTATE_STENCILPASS, dwStates[4]);
                        SetRenderState(D3DRENDERSTATE_STENCILREF, dwReference[0]);
                        SetRenderState(D3DRENDERSTATE_STENCILMASK, dwMasks[j]);
                        SetRenderState(D3DRENDERSTATE_STENCILWRITEMASK, dwMasks[j]);

                        for (n = 0; n < dwIterations; n++)
                        {
                            if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                            {
                                vFlood((D3DVALUE) 0.5, m_dwFailColor);

                                if (!EndScene())
                                {
                                    WriteToLog(_T("Unable to end scene, blocked."));
                                    Fail();
                                    bFail = true;
                                    break;
                                }
                            }
                            else
                            {
                                WriteToLog(_T("Unable to begin scene, blocked."));
                                Fail();
                                bFail = true;
                                break;
                            }

                            dwFlags = (DWORD) 0;
                        }

                        if (!bFail)
                        {
                            SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD) D3DCMP_ALWAYS);
                            SetRenderState(D3DRENDERSTATE_STENCILFUNC, (DWORD) D3DCMP_EQUAL);
                            SetRenderState(D3DRENDERSTATE_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                            SetRenderState(D3DRENDERSTATE_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                            SetRenderState(D3DRENDERSTATE_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);

                            if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                            {
                                vFlood((D3DVALUE) 0.5, m_dwPassColor);
                    
                                if (EndScene())
                                {
                                    m_fPassPercentage = 1.0;

                                    if (!bSupportedOperation(dwOp) || bCompare())
                                        Pass();
                                    else
                                    {
                                        WriteToLog(_T("Invalid pixel detected."));
                                        Fail();
                                        bFail = true;
                                    }
                                }
                                else
                                {
                                    WriteToLog(_T("Unable to end scene, blocked."));
                                    Fail();
                                    bFail = true;
                                }
                            }
                            else
                            {
                                WriteToLog(_T("Unable to begin scene, blocked."));
                                Fail();
                                bFail = true;
                            }
                        }

                        if (bWrap && !bFail)
                        {
                            if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                            {
                                vFlood((D3DVALUE) 0.5, m_dwFailColor);

                                if (EndScene())
                                {
                                    SetRenderState(D3DRENDERSTATE_ZFUNC, dwStates[0]);
                                    SetRenderState(D3DRENDERSTATE_STENCILFUNC, dwStates[1]);
                                    SetRenderState(D3DRENDERSTATE_STENCILFAIL, dwStates[2]);
                                    SetRenderState(D3DRENDERSTATE_STENCILZFAIL, dwStates[3]);
                                    SetRenderState(D3DRENDERSTATE_STENCILPASS, dwStates[4]);

                                    if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                                    {
                                        vFlood((D3DVALUE) 0.5, m_dwFailColor);

                                        if (EndScene())
                                        {
                                            SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD) D3DCMP_ALWAYS);
                                            SetRenderState(D3DRENDERSTATE_STENCILFUNC, (DWORD) D3DCMP_EQUAL);
                                            SetRenderState(D3DRENDERSTATE_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                                            SetRenderState(D3DRENDERSTATE_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                                            SetRenderState(D3DRENDERSTATE_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);
                                            SetRenderState(D3DRENDERSTATE_STENCILREF, dwReference[1]);

                                            if (BeginScene(dvDepth, dwClear, m_dwFailColor, dwFlags))
                                            {
                                                vFlood((D3DVALUE) 0.5, m_dwPassColor);

                                                if (EndScene())
                                                {
                                                    m_fPassPercentage = 1.0;

                                                    if (!bSupportedOperation(dwOp) || bCompare())
                                                        Pass();
                                                    else
                                                    {
                                                        WriteToLog(_T("Invalid pixel detected."));
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

                        bSuccess = true;
                    }
                }
            }
        }
    }

//    return bSuccess;
    return false;
}

const UINT NOPS = 8;

// NAME:        lpszGetOp()
// DESC:        get operation string
// INPUT:       dwOp.....operation index
// OUTPUT:      szOps[dwOp]
// COMMENTS:    dwOp clamped to NOPS

LPSTR CStencilOp::lpszGetOp(DWORD dwOp)
{
    static PSTR szOps[] = {
        "D3DSTENCILOP_KEEP",
        "D3DSTENCILOP_ZERO",
        "D3DSTENCILOP_REPLACE",
        "D3DSTENCILOP_INCRSAT",
        "D3DSTENCILOP_DECRSAT",
        "D3DSTENCILOP_INVERT",
        "D3DSTENCILOP_INCR",
        "D3DSTENCILOP_DECR",
        "invalid op"
    };
    static DWORD dwOps[] = {
        D3DSTENCILOP_KEEP,
        D3DSTENCILOP_ZERO,
        D3DSTENCILOP_REPLACE,
        D3DSTENCILOP_INCRSAT,
        D3DSTENCILOP_DECRSAT,
        D3DSTENCILOP_INVERT,
        D3DSTENCILOP_INCR,
        D3DSTENCILOP_DECR,
    };
    UINT i;

    for (i = 0; i < NOPS; i++) {
        if (dwOp == dwOps[i]) {
            return (LPSTR)szOps[i];
        }
    }

    return (LPSTR)szOps[NOPS];
//    dwOp -= D3DSTENCILOP_KEEP;
//    dwOp = (DWORD) ((dwOp < NOPS) ? dwOp : NOPS);
//    return((LPSTR) szOps[dwOp]);
}

const UINT NSTATES = 3;

// NAME:        lpszGetState()
// DESC:        get state string
// INPUT:       dwState..state index
// OUTPUT:      szStates[dwState]
// COMMENTS:    dwState clamped to NSTATES

LPSTR CStencilOp::lpszGetState(DWORD dwState)
{
    static PSTR szStates[] = {
        "D3DRENDERSTATE_STENCILFAIL",
        "D3DRENDERSTATE_STENCILZFAIL",
        "D3DRENDERSTATE_STENCILPASS",
        "invalid state"
    };
    static DWORD dwStates[] = {
        D3DRENDERSTATE_STENCILFAIL,
        D3DRENDERSTATE_STENCILZFAIL,
        D3DRENDERSTATE_STENCILPASS,
    };
    UINT i;

    for (i = 0; i < NSTATES; i++) {
        if (dwState == dwStates[i]) {
            return (LPSTR)szStates[i];
        }
    }

    return (LPSTR)szStates[NSTATES];

//    dwState -= D3DRENDERSTATE_STENCILFAIL;
//    dwState = (DWORD) ((dwState < NSTATES) ? dwState : NSTATES);
//    return((LPSTR) szStates[dwState]);
}
