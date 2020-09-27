///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CCapsDatabase.cpp
//  Content:    D3DX capabilities database
//
///////////////////////////////////////////////////////////////////////////

#include "pchcore.h"


//-------------------------------------------------------------------------
// D3DXCreateSprite
//-------------------------------------------------------------------------

HRESULT WINAPI
    D3DXCreateSprite(
        LPDIRECT3DDEVICE8   pDevice,
        LPD3DXSPRITE*       ppSprite)
{
    HRESULT hr;
    CD3DXSprite *pSprite = NULL;

    if (!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if (!ppSprite)
    {
        DPF(0, "ppSprite pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!(pSprite = new CD3DXSprite))
        return E_OUTOFMEMORY;

    if(FAILED(hr = pSprite->Initialize(pDevice)))
    {
        delete pSprite;
        return hr;
    }

    *ppSprite = (LPD3DXSPRITE) pSprite;
    return S_OK;
}



//-------------------------------------------------------------------------
// D3DXCreateRenderToSurface
//-------------------------------------------------------------------------

HRESULT WINAPI
    D3DXCreateRenderToSurface(
        LPDIRECT3DDEVICE8       pDevice,
        UINT                    Width,
        UINT                    Height,
        D3DFORMAT               Format,
        BOOL                    DepthStencil,
        D3DFORMAT               DepthStencilFormat,
        LPD3DXRENDERTOSURFACE*  ppRenderToSurface)
{
    HRESULT hr;
    CD3DXRenderToSurface *pRTS;

    if(!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if (!ppRenderToSurface)
    {
        DPF(0, "ppRenderToSurface pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(!(pRTS = new CD3DXRenderToSurface()))
        return E_OUTOFMEMORY;

    if(FAILED(hr = pRTS->Init(pDevice, Width, Height, Format, 
        DepthStencil, DepthStencilFormat)))
    {
        delete pRTS;
        return hr;
    }

    *ppRenderToSurface = (ID3DXRenderToSurface *) pRTS;
    return S_OK;
}


//-------------------------------------------------------------------------
// D3DXCreateRenderToEnvMap
//-------------------------------------------------------------------------

HRESULT WINAPI
    D3DXCreateRenderToEnvMap(
        LPDIRECT3DDEVICE8       pDevice,
        UINT                    Size,
        D3DFORMAT               Format,
        BOOL                    DepthStencil,
        D3DFORMAT               DepthStencilFormat,
        LPD3DXRenderToEnvMap*   ppRenderToEnvMap)
{
    HRESULT hr;
    CD3DXRenderToEnvMap *pRTE;

    if(!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if (!ppRenderToEnvMap)
    {
        DPF(0, "ppRenderToEnvMap pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(!(pRTE = new CD3DXRenderToEnvMap()))
        return E_OUTOFMEMORY;

    if(FAILED(hr = pRTE->Init(pDevice, Size, Format, 
        DepthStencil, DepthStencilFormat)))
    {
        delete pRTE;
        return hr;
    }

    *ppRenderToEnvMap = (ID3DXRenderToEnvMap *) pRTE;
    return S_OK;
}


//-------------------------------------------------------------------------
// D3DXGetFVFVertexSize:
//-------------------------------------------------------------------------

UINT WINAPI
    D3DXGetFVFVertexSize(DWORD dwFVF)
{
    UINT uSize = 0;

    switch (dwFVF & D3DFVF_POSITION_MASK)
    {
    case D3DFVF_XYZ:    uSize = 3 * sizeof(float); break;
    case D3DFVF_XYZRHW: uSize = 4 * sizeof(float); break;
    case D3DFVF_XYZB1:  uSize = 4 * sizeof(float); break;
    case D3DFVF_XYZB2:  uSize = 5 * sizeof(float); break;
    case D3DFVF_XYZB3:  uSize = 6 * sizeof(float); break;
    case D3DFVF_XYZB4:  uSize = 7 * sizeof(float); break;
    }

    if(dwFVF & D3DFVF_NORMAL)
        uSize += 3 * sizeof(float);

    if(dwFVF & D3DFVF_DIFFUSE)
        uSize += sizeof(DWORD);

    if(dwFVF & D3DFVF_SPECULAR)
        uSize += sizeof(DWORD);


    // Texture coordinates
    UINT uNumTexCoords = (((dwFVF) & D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);
    UINT uTextureFormats = dwFVF >> 16;

    if(uTextureFormats)
    {
        for (UINT i=0; i < uNumTexCoords; i++)
        {
            switch(uTextureFormats & 3)
            {
            case 0: uSize += 2 * sizeof(float); break;
            case 1: uSize += 3 * sizeof(float); break;
            case 2: uSize += 4 * sizeof(float); break;
            case 3: uSize += 1 * sizeof(float); break;
            }

            uTextureFormats >>= 2;
        }
    }
    else
    {
        uSize += uNumTexCoords * (2 * sizeof(float));
    }

    return uSize;
}



//-------------------------------------------------------------------------
// D3DXGetErrorString
//-------------------------------------------------------------------------

HRESULT WINAPI
    D3DXGetErrorStringA(
        HRESULT             hr,
        LPSTR               pBuffer,
        UINT                cchBuffer)
{
    if(!pBuffer || !cchBuffer)
        return S_OK;


    // Lookup error
    LPSTR pErrStr = NULL;

    switch (hr)
    {
    #define CASE_ERROR( err ) \
        case err: pErrStr = #err; break

    // Generic errors
    CASE_ERROR( S_OK );
    CASE_ERROR( E_FAIL );
    CASE_ERROR( E_OUTOFMEMORY );
    CASE_ERROR( E_NOTIMPL );

    // D3D errors
    CASE_ERROR( D3DERR_WRONGTEXTUREFORMAT );
    CASE_ERROR( D3DERR_UNSUPPORTEDCOLOROPERATION );
    CASE_ERROR( D3DERR_UNSUPPORTEDCOLORARG );
    CASE_ERROR( D3DERR_UNSUPPORTEDALPHAOPERATION );
    CASE_ERROR( D3DERR_UNSUPPORTEDALPHAARG );
    CASE_ERROR( D3DERR_TOOMANYOPERATIONS );
    CASE_ERROR( D3DERR_CONFLICTINGTEXTUREFILTER );
    CASE_ERROR( D3DERR_UNSUPPORTEDFACTORVALUE );
    CASE_ERROR( D3DERR_CONFLICTINGRENDERSTATE );
    CASE_ERROR( D3DERR_UNSUPPORTEDTEXTUREFILTER );
    CASE_ERROR( D3DERR_CONFLICTINGTEXTUREPALETTE );
    CASE_ERROR( D3DERR_DRIVERINTERNALERROR );
    CASE_ERROR( D3DERR_NOTFOUND );
    CASE_ERROR( D3DERR_MOREDATA );
    CASE_ERROR( D3DERR_DEVICELOST );
    CASE_ERROR( D3DERR_DEVICENOTRESET );
    CASE_ERROR( D3DERR_NOTAVAILABLE );
    CASE_ERROR( D3DERR_OUTOFVIDEOMEMORY );
    CASE_ERROR( D3DERR_INVALIDDEVICE );
    CASE_ERROR( D3DERR_INVALIDCALL );

    // D3DX errors
    CASE_ERROR( D3DXERR_CANNOTMODIFYINDEXBUFFER );
    CASE_ERROR( D3DXERR_INVALIDMESH );
    CASE_ERROR( D3DXERR_CANNOTATTRSORT );
    CASE_ERROR( D3DXERR_SKINNINGNOTSUPPORTED );
    CASE_ERROR( D3DXERR_TOOMANYINFLUENCES );
    CASE_ERROR( D3DXERR_INVALIDDATA );

    #undef CASE_ERROR
    }


    // Copy error into supplied buffer
    int cch;

    if( pErrStr )
        cch = _snprintf(pBuffer, cchBuffer, "%s", pErrStr );
    else
        cch = _snprintf(pBuffer, cchBuffer, "Unrecognized error: 0x%0.8x", hr );

    if(cch < 0 && cchBuffer > 0)
        pBuffer[cchBuffer - 1] = '\0';

    return S_OK;
}


HRESULT WINAPI
    D3DXGetErrorStringW(
        HRESULT             hr,
        LPWSTR              pBuffer,
        UINT                cchBuffer)
{
    int err;
    if(!pBuffer || !cchBuffer)
        return S_OK;

    char *pszA = (char *) _alloca(cchBuffer);

    if(FAILED(hr = D3DXGetErrorStringA(hr, pszA, cchBuffer)))
        return hr;

    if(!(err = MultiByteToWideChar(CP_ACP, 0, pszA, strlen(pszA), pBuffer, cchBuffer)))
    {
        DPF(0, "Could not generate wide char string");
        return HRESULT_FROM_WIN32(err);
    }
    return S_OK;
}
