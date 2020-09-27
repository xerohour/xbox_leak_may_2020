///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       d3dx8effect.cpp
//  Content:    Compile D3DX Effect files
//
///////////////////////////////////////////////////////////////////////////

#include "pcheffect.h"
#include "CD3DXFile.h"


//----------------------------------------------------------------------------
// D3DXCompileEffect
//----------------------------------------------------------------------------

HRESULT WINAPI
    D3DXCompileEffectFromFileA(
        LPCSTR            pSrcFile,
        LPD3DXBUFFER*     ppCompiledEffect,
        LPD3DXBUFFER*     ppCompilationErrors)
{
    HRESULT hr;
    CD3DXFile fm;
    CD3DXEffectCompiler compiler;

    if(ppCompiledEffect)
        *ppCompiledEffect = NULL;

    if(ppCompilationErrors)
        *ppCompilationErrors = NULL;

    if(FAILED(hr = fm.Open(pSrcFile, FALSE)))
        return hr;

    if(FAILED(hr = compiler.Compile(fm.m_pvData, fm.m_cbData, pSrcFile, ppCompiledEffect, ppCompilationErrors)))
        return hr;

    return S_OK;
}


HRESULT WINAPI
    D3DXCompileEffectFromFileW(
        LPCWSTR           pSrcFile,
        LPD3DXBUFFER*     ppCompiledEffect,
        LPD3DXBUFFER*     ppCompilationErrors)
{
    HRESULT hr;
    CD3DXFile fm;
    CD3DXEffectCompiler compiler;

    // Convert file name to ascii
    int cch = WideCharToMultiByte(CP_ACP, 0, pSrcFile, -1, NULL, 0, NULL, NULL);
    char *pch = (char *) _alloca(cch);
    WideCharToMultiByte(CP_ACP, 0, pSrcFile, -1, pch, cch, NULL, NULL);


    if(ppCompiledEffect)
        *ppCompiledEffect = NULL;

    if(ppCompilationErrors)
        *ppCompilationErrors = NULL;

    if(FAILED(hr = fm.Open(pSrcFile, TRUE)))
        return hr;

    if(FAILED(hr = compiler.Compile(fm.m_pvData, fm.m_cbData, pch, ppCompiledEffect, ppCompilationErrors)))
        return hr;

    return S_OK;
}


HRESULT WINAPI
    D3DXCompileEffect(
        LPCVOID           pSrcData,
        UINT              SrcDataSize,
        LPD3DXBUFFER*     ppCompiledEffect,
        LPD3DXBUFFER*     ppCompilationErrors)
{
    HRESULT hr;
    CD3DXEffectCompiler compiler;

    if(ppCompiledEffect)
        *ppCompiledEffect = NULL;

    if(ppCompilationErrors)
        *ppCompilationErrors = NULL;

    if(FAILED(hr = compiler.Compile(pSrcData, SrcDataSize, NULL, ppCompiledEffect, ppCompilationErrors)))
        return hr;

    return S_OK;
}



//----------------------------------------------------------------------------
// D3DXCreateEffect
//----------------------------------------------------------------------------

HRESULT WINAPI
    D3DXCreateEffect(
        LPDIRECT3DDEVICE8 pDevice,
        LPCVOID           pCompiledEffect,
        UINT              CompiledEffectSize,
        DWORD             Usage,
        LPD3DXEFFECT*     ppEffect)
{
    HRESULT hr;
    CD3DXEffect *pEffect;

    if(ppEffect)
        *ppEffect = NULL;

    if(!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if (!pCompiledEffect)
    {
        DPF(0, "pCompiledEffect pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if (!ppEffect)
    {
        DPF(0, "ppEffect pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if (CompiledEffectSize == 0)
    {
        DPF(0, "CompiledEffectSize should be > 0");
        return D3DERR_INVALIDCALL;
    }

    if(!(pEffect = new CD3DXEffect))
        return E_OUTOFMEMORY;

    if(FAILED(hr = pEffect->Initialize(pDevice, pCompiledEffect, CompiledEffectSize, Usage)))
    {
        if (hr == D3DXERR_INVALIDDATA || hr == E_FAIL)
        {
            DPF(0, "Could not parse compiled effect");
            hr = D3DXERR_INVALIDDATA;
        }
        delete pEffect;
        return hr;
    }

    *ppEffect = (LPD3DXEFFECT) pEffect;
    return S_OK;
}


