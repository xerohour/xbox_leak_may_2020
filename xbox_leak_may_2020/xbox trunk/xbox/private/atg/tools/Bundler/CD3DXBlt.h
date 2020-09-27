//-----------------------------------------------------------------------------
// File: CD3DXBlt.h
//
// Desc: Contains general purpose blitting routines
//
// Hist: 01.29.02 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef __CXD3DXBlt_H__
#define __CXD3DXBlt_H__

#include "CD3DXCodec.h"

///////////////////////////////////////////////////////////////////////////
// CXD3DXBlt ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class CXD3DXBlt
{
public:
    CXD3DXBlt();
   ~CXD3DXBlt();

    HRESULT Blt(D3DX_BLT* pDestBlt, D3DX_BLT* pSrcBlt, DWORD dwFilter);

protected:
    // Generic filters
    HRESULT BltSame();
    HRESULT BltCopy();
    HRESULT BltNone();
    HRESULT BltPoint();
    HRESULT BltBox2D();
    HRESULT BltBox3D();
    HRESULT BltLinear2D();
    HRESULT BltLinear3D();
    HRESULT BltTriangle2D();
    HRESULT BltTriangle3D();

    // Optimized filters
    HRESULT BltSame_DXTn();
    HRESULT BltBox2D_R8G8B8();  
    HRESULT BltBox2D_A8R8G8B8();
    HRESULT BltBox2D_X8R8G8B8();
    HRESULT BltBox2D_R5G6B5();  
    HRESULT BltBox2D_X1R5G5B5();
    HRESULT BltBox2D_A1R5G5B5();
    HRESULT BltBox2D_A4R4G4B4();
    HRESULT BltBox2D_R3G3B2();  
    HRESULT BltBox2D_A8();      
    HRESULT BltBox2D_A8R3G3B2();
    HRESULT BltBox2D_X4R4G4B4();
    HRESULT BltBox2D_A8P8();    
    HRESULT BltBox2D_P8();      
    HRESULT BltBox2D_A8L8();    
    HRESULT BltBox2D_A4L4();    

    // Codecs and Filter Type
    CXD3DXCodec *m_pSrc;
    CXD3DXCodec *m_pDest;

    DWORD m_dwFilter;
};


#endif