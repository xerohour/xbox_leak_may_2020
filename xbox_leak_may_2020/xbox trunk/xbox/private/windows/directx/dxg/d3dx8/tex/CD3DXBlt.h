///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CD3DXBlt.h
//  Content:    Blitter
//
///////////////////////////////////////////////////////////////////////////

#ifndef __CD3DXBlt_H__
#define __CD3DXBlt_H__


///////////////////////////////////////////////////////////////////////////
// CD3DXBlt ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class CD3DXBlt
{
public:
    CD3DXBlt();
   ~CD3DXBlt();

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
    CD3DXCodec *m_pSrc;
    CD3DXCodec *m_pDest;

    DWORD m_dwFilter;
};


#endif