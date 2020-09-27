//-----------------------------------------------------------------------------
// File: LoadImage.h
//
// Desc: Loads image files
//
// Hist: 04.13.01 - New for May XDK release
//       01.29.02 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef LOADIMAGE_H
#define LOADIMAGE_H

#include "bundler.h"

#define DPF(x,y)
#define D3DXASSERT(x)
#define D3DXERR_INVALIDDATA -1




//-----------------------------------------------------------------------------
// Name: class CImage
// Desc: Class to load different image formats
//-----------------------------------------------------------------------------
class CImage
{
public:
    CImage();
    CImage( DWORD dwWidth, DWORD dwHeight, D3DFORMAT format );
    ~CImage();

    VOID*           m_pData;
    D3DFORMAT       m_Format;
    DWORD           m_Width;
    DWORD           m_Height;
    DWORD           m_Pitch;
    PALETTEENTRY*   m_pPalette;

    RECT            m_Rect;

    BOOL            m_bDeleteData;
    BOOL            m_bDeletePalette;

    HRESULT LoadBMP( const VOID* pData, DWORD dwSize );
    HRESULT LoadDIB( const VOID* pData, DWORD dwSize );
    HRESULT LoadTGA( const VOID* pData, DWORD dwSize );
    HRESULT LoadPPM( const VOID* pData, DWORD dwSize );

public:
    HRESULT Load( const VOID* pData, DWORD dwSize );
    HRESULT Load( CHAR* strFilename );

    HRESULT Depalettize();
};

class CVolume
{
public:
    CVolume();
    CVolume( DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, D3DFORMAT format );
    ~CVolume();

    VOID*           m_pData;
    D3DFORMAT       m_Format;
    DWORD           m_Width;
    DWORD           m_Height;
    DWORD           m_Depth;
    DWORD           m_RowPitch;
    DWORD           m_SlicePitch;

    D3DBOX          m_Box;
};


HRESULT LoadImageFromImage( CImage* pDestImage, CImage* pSrcImage, DWORD dwFilter );
HRESULT LoadVolumeFromVolume( CVolume* pDestVolume, CVolume* pSrcVolume, DWORD dwFilter );

#endif
