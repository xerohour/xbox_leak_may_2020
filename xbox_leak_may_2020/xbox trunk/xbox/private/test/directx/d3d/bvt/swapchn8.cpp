/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    swapchn8.cpp

Description:

    IDirect3DSwapChain8 interface BVTs.

*******************************************************************************/

#ifndef UNDER_XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif // UNDER_XBOX
#include <tchar.h>
#include <d3dx8.h>
#include "d3dbvt.h"
#include "log.h"

using namespace D3DBVT;

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

//******************************************************************************
// IDirect3DSwapChain8 inteface tests
//******************************************************************************

#ifndef UNDER_XBOX

//******************************************************************************
TESTPROCAPI TSWC8_Present(PTESTTABLEENTRY ptte) {

    LPDIRECT3DSWAPCHAIN8    pd3dsw;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    pd3dsw = GetSwapChain8();
    if (!pd3dsw) {
        return TR_ABORT;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TSWC8_GetBackBuffer(PTESTTABLEENTRY ptte) {

    LPDIRECT3DSWAPCHAIN8    pd3dsw;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    pd3dsw = GetSwapChain8();
    if (!pd3dsw) {
        return TR_ABORT;
    }

    return tr;
}

#endif // !UNDER_XBOX
