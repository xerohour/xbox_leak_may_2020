/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vertbuf8.cpp

Description:

    IDirect3DVertexBuffer8 interface BVTs.

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
// IDirect3DVertexBuffer8 inteface tests
//******************************************************************************

//******************************************************************************
TESTPROCAPI TVRB8_GetDesc(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVERTEXBUFFER8 pd3dr;
    D3DVERTEXBUFFER_DESC    d3drd;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    pd3dr = GetVertexBuffer8();
    if (!pd3dr) {
        return TR_ABORT;
    }

    hr = pd3dr->GetDesc(&d3drd);
    if (ResultFailed(hr, TEXT("IDirect3DVertexBuffer8::GetDesc"))) {
        return TR_FAIL;
    }

#ifndef UNDER_XBOX

    if (d3drd.FVF != VERTBUFFFORMAT) {
        Log(LOG_FAIL, TEXT("IDirect3DVertexBuffer8::GetDesc returned an incorrect buffer FVF"));
        tr = TR_FAIL;
    }

    if (d3drd.Size != VERTBUFFSIZE) {
        Log(LOG_FAIL, TEXT("IDirect3DVertexBuffer8::GetDesc returned an incorrect buffer size"));
        tr = TR_FAIL;
    }

    if (d3drd.Pool != D3DPOOL_MANAGED) {
        Log(LOG_FAIL, TEXT("IDirect3DVertexBuffer8::GetDesc returned an incorrect buffer pool"));
        tr = TR_FAIL;
    }

    if (d3drd.Usage != 0) {
        Log(LOG_FAIL, TEXT("IDirect3DVertexBuffer8::GetDesc returned an incorrect buffer usage"));
        tr = TR_FAIL;
    }

#endif

    if (d3drd.Type != D3DRTYPE_VERTEXBUFFER) {
        Log(LOG_FAIL, TEXT("IDirect3DVertexBuffer8::GetDesc returned an incorrect buffer type"));
        tr = TR_FAIL;
    }

    if (d3drd.Format != D3DFMT_VERTEXDATA) {
        Log(LOG_FAIL, TEXT("IDirect3DVertexBuffer8::GetDesc returned an incorrect buffer format"));
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TVRB8_Lock(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVERTEXBUFFER8 pd3dr;

    pd3dr = GetVertexBuffer8();
    if (!pd3dr) {
        return TR_ABORT;
    }

    return TSHR8_Lock(ptte, (LPVOID)pd3dr);
}

//******************************************************************************
TESTPROCAPI TVRB8_Unlock(PTESTTABLEENTRY ptte) {

    LPDIRECT3DVERTEXBUFFER8 pd3dr;

    pd3dr = GetVertexBuffer8();
    if (!pd3dr) {
        return TR_ABORT;
    }

    return TSHR8_Unlock(ptte, (LPVOID)pd3dr);
}
