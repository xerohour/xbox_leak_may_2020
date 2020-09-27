///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CD3DXImage.h
//  Content:    Image file class
//
///////////////////////////////////////////////////////////////////////////

#ifndef __CD3DXImage_H__
#define __CD3DXImage_H__


///////////////////////////////////////////////////////////////////////////
// CD3DXImage /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class CD3DXImage
{
public:
    D3DFORMAT       m_Format;
    LPVOID          m_pvData;
    DWORD           m_cbPitch;
    RECT            m_Rect;
    PALETTEENTRY*   m_pPalette;

    BOOL            m_bDeleteData;
    BOOL            m_bDeletePalette;

    CD3DXImage*     m_pMip;
    CD3DXImage*     m_pFace;


public:
    CD3DXImage();
    ~CD3DXImage();

    HRESULT Load(LPCVOID pvData, DWORD cbData, D3DXIMAGE_INFO *pInfo);

private:
    HRESULT LoadBMP(LPCVOID pvData, DWORD cbData);
    HRESULT LoadDIB(LPCVOID pvData, DWORD cbData);
    HRESULT LoadJPG(LPCVOID pvData, DWORD cbData);
    HRESULT LoadTGA(LPCVOID pvData, DWORD cbData);
    HRESULT LoadPPM(LPCVOID pvData, DWORD cbData);
    HRESULT LoadDDS(LPCVOID pvData, DWORD cbData);
    HRESULT LoadPNG(LPCVOID pvData, DWORD cbData);
};


#endif