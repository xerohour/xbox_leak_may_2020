// FILE:        func.cpp
// DESC:        stencil conformance tests
// AUTHOR:      Todd M. Frost
// COMMENTS:    tests for stencil funcs (D3DCMP)

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "flags.h"
#include "stencil.h"

const UINT NARGS = 5;

DWORD dwFailArgs[][NARGS] = {
    { D3DCMP_NEVER,        D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_LESS,         D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_LESS,         D3DSTENCILOP_INCR, 0, 1, 1 },
    { D3DCMP_EQUAL,        D3DSTENCILOP_ZERO, 1, 0, 0 },
    { D3DCMP_EQUAL,        D3DSTENCILOP_INCR, 0, 1, 1 },
    { D3DCMP_LESSEQUAL,    D3DSTENCILOP_INCR, 0, 1, 1 },
    { D3DCMP_GREATER,      D3DSTENCILOP_ZERO, 1, 0, 0 },
    { D3DCMP_GREATER,      D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_NOTEQUAL,     D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_GREATEREQUAL, D3DSTENCILOP_ZERO, 1, 0, 0 },
};

DWORD dwPassArgs[][NARGS] = {
    { D3DCMP_LESS,         D3DSTENCILOP_ZERO, 1, 0, 0 },
    { D3DCMP_EQUAL,        D3DSTENCILOP_INCR, 0, 0, 1 },
    { D3DCMP_EQUAL,        D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_LESSEQUAL,    D3DSTENCILOP_ZERO, 1, 0, 0 },
    { D3DCMP_LESSEQUAL,    D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_GREATER,      D3DSTENCILOP_INCR, 0, 1, 1 },
    { D3DCMP_NOTEQUAL,     D3DSTENCILOP_ZERO, 1, 0, 0 },
    { D3DCMP_NOTEQUAL,     D3DSTENCILOP_INCR, 0, 1, 1 },
    { D3DCMP_GREATEREQUAL, D3DSTENCILOP_ZERO, 1, 1, 0 },
    { D3DCMP_GREATEREQUAL, D3DSTENCILOP_INCR, 0, 1, 1 },
    { D3DCMP_ALWAYS,       D3DSTENCILOP_ZERO, 1, 1, 0 },
};

// NAME:        CStencilFunc()
// DESC:        stencil func class constructor
// INPUT:       none
// OUTPUT:      none

CStencilFunc::CStencilFunc()
{
    m_szTestName = TEXT("Stencil func");
    m_szCommandKey = TEXT("Func");

    m_nStipple = (UINT) 0;
}

// NAME:        ~CStencilFunc()
// DESC:        stencil func class destructor
// INPUT:       none
// OUTPUT:      none

CStencilFunc::~CStencilFunc()
{
}

// NAME:        CommandLineHelp()
// DESC:        command line help
// INPUT:       none
// OUTPUT:      none

void CStencilFunc::CommandLineHelp(void)
{
    CStencil::CommandLineHelp();
}

// NAME:        TestInitialize()
// DESC:        initialize test
// INPUT:       none
// OUTPUT:      uResult..result of test initialization

UINT CStencilFunc::TestInitialize(VOID)
{
    UINT uBase = NARGS*sizeof(DWORD);
    UINT uResult = CStencil::TestInitialize();

    m_nStipple = (UINT) (m_dwVersion > 0x0600) ? 1 : 2;
    SetTestRange((UINT) 1, (UINT) (NMASKS*m_nStipple*(sizeof(dwFailArgs)/uBase + 2*sizeof(dwPassArgs)/uBase)));

    if (D3DTESTINIT_RUN == uResult)
    {
        if (!(m_dwZCmpCaps & D3DPCMPCAPS_ALWAYS))
        {
            WriteToLog(_T("Device does NOT support D3DCMP_ALWAYS, blocked."));
            uResult = D3DTESTINIT_SKIPALL;
        }
        else if (!(m_dwZCmpCaps & D3DPCMPCAPS_NEVER))
        {
            WriteToLog(_T("Device does NOT support D3DCMP_NEVER, blocked."));
            uResult = D3DTESTINIT_SKIPALL;
        }
    }

    return uResult;
}

// NAME:        ExecuteTest()
// DESC:        execute test
// INPUT:       uTest....test number
// OUTPUT:      true.....if test executed successfully
//              false....otherwise
// COMMENTS:    dwFailArgs, dwPassArgs:
//                  0:  stencil compare function
//                  1:  stencil operation
//                  2:  stencil clear value (0 => 0, 1 => mask)
//                  3:  reference mask (pre)
//                  4:  reference mask (post)

bool CStencilFunc::ExecuteTest(UINT uTest)
{
    ARGS Args;
    CHAR szStipple[2][13] = {
        "Non-Stippled",
        "    Stippled"
    };
    DWORD dwMasks[2];
    UINT i, j, k, n, uTests;

    CStencil::ExecuteTest(uTest);
    dwMasks[0] = m_dwStencilBitMask;
    dwMasks[1] = 0xffffffff;

    for (k = 1, i = 0; i < m_nStipple; i++)
        for (j = 0; j < NMASKS; j++)
        {
            uTests = sizeof(dwFailArgs)/(NARGS*sizeof(DWORD));

            for (n = 0; n < uTests; n++, k++)
                if (k == uTest)
                {
                    sprintf(m_szBuffer, "%s, STENCILFAIL, %s", lpszGetFunc(dwFailArgs[i][0]), szStipple[i]);
                    BeginTestCase(m_szBuffer);

                    vStipple(((i == 0) ? false : true));

                    SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD) D3DCMP_ALWAYS);
                    SetRenderState(D3DRENDERSTATE_STENCILENABLE, (DWORD) TRUE);
                    SetRenderState(D3DRENDERSTATE_STENCILFUNC, dwFailArgs[i][0]);
                    SetRenderState(D3DRENDERSTATE_STENCILFAIL, dwFailArgs[i][1]);
                    SetRenderState(D3DRENDERSTATE_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRENDERSTATE_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRENDERSTATE_STENCILREF, (DWORD) 0x00000000);
                    SetRenderState(D3DRENDERSTATE_STENCILMASK, dwMasks[j]);
                    SetRenderState(D3DRENDERSTATE_STENCILWRITEMASK, dwMasks[j]);

                    Args.dwClearColor = m_dwFailColor;
                    Args.dwStencil = dwFailArgs[i][2];
                    Args.dwReference[0] = dwFailArgs[i][3];
                    Args.dwReference[1] = dwFailArgs[i][4];

                    vTest(&Args);
                    return true;
                }

            uTests = sizeof(dwPassArgs)/(NARGS*sizeof(DWORD));

            for (n = 0; n < uTests; n++, k++)
                if (k == uTest)
                {
                    sprintf(m_szBuffer, "%s, STENCILZFAIL, %s", lpszGetFunc(dwPassArgs[i][0]), szStipple[i]);
                    BeginTestCase(m_szBuffer);

                    vStipple(((i == 0) ? false : true));

                    SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD) D3DCMP_NEVER);
                    SetRenderState(D3DRENDERSTATE_STENCILENABLE, (DWORD) TRUE);
                    SetRenderState(D3DRENDERSTATE_STENCILFUNC, dwPassArgs[i][0]);
                    SetRenderState(D3DRENDERSTATE_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRENDERSTATE_STENCILZFAIL, dwPassArgs[i][1]);
                    SetRenderState(D3DRENDERSTATE_STENCILPASS, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRENDERSTATE_STENCILREF, (DWORD) 0x00000000);
                    SetRenderState(D3DRENDERSTATE_STENCILMASK, dwMasks[j]);
                    SetRenderState(D3DRENDERSTATE_STENCILWRITEMASK, dwMasks[j]);

                    Args.dwClearColor = m_dwFailColor;
                    Args.dwStencil = dwPassArgs[i][2];
                    Args.dwReference[0] = dwPassArgs[i][3];
                    Args.dwReference[1] = dwPassArgs[i][4];

                    vTest(&Args);
                    return true;
                }

            uTests = sizeof(dwPassArgs)/(NARGS*sizeof(DWORD));

            for (n = 0; n < uTests; n++, k++)
                if (k == uTest)
                {
                    sprintf(m_szBuffer, "%s, STENCILPASS, %s", lpszGetFunc(dwPassArgs[i][0]), szStipple[i]);
                    BeginTestCase(m_szBuffer);

                    vStipple(((i == 0) ? false : true));

                    SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD) D3DCMP_ALWAYS);
                    SetRenderState(D3DRENDERSTATE_STENCILENABLE, (DWORD) TRUE);
                    SetRenderState(D3DRENDERSTATE_STENCILFUNC, dwPassArgs[i][0]);
                    SetRenderState(D3DRENDERSTATE_STENCILFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRENDERSTATE_STENCILZFAIL, (DWORD) D3DSTENCILOP_KEEP);
                    SetRenderState(D3DRENDERSTATE_STENCILPASS, dwPassArgs[i][1]);
                    SetRenderState(D3DRENDERSTATE_STENCILREF, (DWORD) 0x00000000);
                    SetRenderState(D3DRENDERSTATE_STENCILMASK, dwMasks[j]);
                    SetRenderState(D3DRENDERSTATE_STENCILWRITEMASK, dwMasks[j]);

                    Args.dwClearColor = m_dwFailColor;
                    Args.dwStencil = dwPassArgs[i][2];
                    Args.dwReference[0] = dwPassArgs[i][3];
                    Args.dwReference[1] = dwPassArgs[i][4];

                    vTest(&Args);
                    return true;
                }
        }

    return false;
}

// NAME:        vTest()
// DESC:        perform stencil function test (D3DRENDERSTATE_STENCILFUNC)
// INPUT:       pArgs........address of ARGS data
// OUTPUT:      none

VOID CStencilFunc::vTest(PARGS pArgs)
{
    D3DVALUE dvDepth;
    DWORD dwFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;

    dvDepth = 1.0f;
    BeginScene(dvDepth, pArgs->dwStencil, pArgs->dwClearColor, dwFlags);
    dvDepth = 0.5f;
    SetRenderState(D3DRENDERSTATE_STENCILREF, pArgs->dwReference[0]);
    vFlood(dvDepth, m_dwFailColor);
    SetRenderState(D3DRENDERSTATE_ZFUNC, (DWORD) D3DCMP_ALWAYS);
    SetRenderState(D3DRENDERSTATE_STENCILFUNC, (DWORD) D3DCMP_EQUAL);
    SetRenderState(D3DRENDERSTATE_STENCILREF, pArgs->dwReference[1]);
    vFlood(dvDepth, m_dwPassColor);
    EndScene();

    if (bCompare())
        Pass();
    else
    {
        WriteToLog(_T("Invalid pixel detected."));
        Fail();
    }

    m_pDevice->Present(NULL, NULL, NULL, NULL);
}

const UINT NFUNCS = 8;

// NAME:        lpszGetFunc()
// DESC:        get function string
// INPUT:       dwFunc...function index
// OUTPUT:      szFuncs[dwFunc]
// COMMENTS:    dwFunc clamped to NFUNCS

LPSTR CStencilFunc::lpszGetFunc(DWORD dwFunc)
{
    static PSTR szFuncs[] = {
        "D3DCMP_NEVER",
        "D3DCMP_LESS",
        "D3DCMP_EQUAL",
        "D3DCMP_LESSEQUAL",
        "D3DCMP_GREATER",
        "D3DCMP_NOTEQUAL",
        "D3DCMP_GREATEREQUAL",
        "D3DCMP_ALWAYS",
        "invalid func"
    };
    static DWORD dwFuncs[] = {
        D3DCMP_NEVER,
        D3DCMP_LESS,
        D3DCMP_EQUAL,
        D3DCMP_LESSEQUAL,
        D3DCMP_GREATER,
        D3DCMP_NOTEQUAL,
        D3DCMP_GREATEREQUAL,
        D3DCMP_ALWAYS,
    };
    UINT i;

    for (i = 0; i < NFUNCS; i++) {
        if (dwFunc == dwFuncs[i]) {
            return (LPSTR)szFuncs[i];
        }
    }

    return (LPSTR)szFuncs[NFUNCS];

//    dwFunc -= D3DCMP_NEVER;
//    dwFunc = (DWORD) ((dwFunc < NFUNCS) ? dwFunc : NFUNCS);
//    return((LPSTR) szFuncs[dwFunc]);
}
