/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    d3dperf.cpp

Abstract:

    D3D related perf BVT tests

--*/

#include "precomp.h"

DECLARE_TEST_FUNCTION(TriCTest)
{
    static const struct {
        FLOAT x,y,z,w;
        DWORD color;
    } vertices[] = {
        {320.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, },
        {620.0f, 430.0f, 0.5f, 1.0f, 0xff00ff00, },
        { 20.0f, 430.0f, 0.5f, 1.0f, 0xff0000ff, },
    };

    UINT count = testparam ? atoi(testparam) : 2000;
    ULONG time = ReadTimestamp();
    HRESULT hr = S_OK;

    while (count--) {
        hr |= d3ddev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00404040, 1.0, 0);
        hr |= d3ddev->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
        hr |= d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        hr |= d3ddev->DrawVerticesUP(D3DPT_TRIANGLELIST, 3, vertices, sizeof(vertices[0]));
        hr |= d3ddev->Present(NULL, NULL, NULL, NULL);
    }

    ASSERT(SUCCEEDED(hr));
    return ReadTimestamp() - time;
}
