/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    indxbuf8.cpp

Description:

    IDirect3DIndexBuffer8 interface BVTs.

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
// IDirect3DIndexBuffer8 inteface tests
//******************************************************************************

//******************************************************************************
TESTPROCAPI TIXB8_GetDesc(PTESTTABLEENTRY ptte) {

    LPDIRECT3DINDEXBUFFER8  pd3di;
    D3DINDEXBUFFER_DESC     d3did;
    HRESULT                 hr;
    TRESULT                 tr = TR_PASS;

    pd3di = GetIndexBuffer8();
    if (!pd3di) {
        return TR_ABORT;
    }

    hr = pd3di->GetDesc(&d3did);
    if (ResultFailed(hr, TEXT("IDirect3DIndexBuffer8::GetDesc"))) {
        return TR_FAIL;
    }

#ifndef UNDER_XBOX

    if (d3did.Size != INDXBUFFSIZE) {
        Log(LOG_FAIL, TEXT("IDirect3DIndexBuffer8::GetDesc returned an incorrect buffer size"));
        tr = TR_FAIL;
    }

    if (d3did.Pool != D3DPOOL_MANAGED) {
        Log(LOG_FAIL, TEXT("IDirect3DIndexBuffer8::GetDesc returned an incorrect buffer pool"));
        tr = TR_FAIL;
    }

    if (d3did.Usage != 0) {
        Log(LOG_FAIL, TEXT("IDirect3DIndexBuffer8::GetDesc returned an incorrect buffer usage"));
        tr = TR_FAIL;
    }

#endif

    if (d3did.Type != D3DRTYPE_INDEXBUFFER) {
        Log(LOG_FAIL, TEXT("IDirect3DIndexBuffer8::GetDesc returned an incorrect buffer type"));
        tr = TR_FAIL;
    }

    if (d3did.Format != D3DFMT_INDEX16) {
        Log(LOG_FAIL, TEXT("IDirect3DIndexBuffer8::GetDesc returned an incorrect buffer format"));
        tr = TR_FAIL;
    }

    return tr;
}

//******************************************************************************
TESTPROCAPI TIXB8_Lock(PTESTTABLEENTRY ptte) {

    LPDIRECT3DINDEXBUFFER8  pd3di;

    pd3di = GetIndexBuffer8();
    if (!pd3di) {
        return TR_ABORT;
    }

    return TSHR8_Lock(ptte, (LPVOID)pd3di);
}

//******************************************************************************
TESTPROCAPI TIXB8_Unlock(PTESTTABLEENTRY ptte) {

    LPDIRECT3DINDEXBUFFER8  pd3di;

    pd3di = GetIndexBuffer8();
    if (!pd3di) {
        return TR_ABORT;
    }

    return TSHR8_Unlock(ptte, (LPVOID)pd3di);
}
