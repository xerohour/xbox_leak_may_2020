/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    pal.h

Description:

    IDirect3DPalette classes.

*******************************************************************************/

#ifndef __PAL_H__
#define __PAL_H__

#ifdef UNDER_XBOX

//******************************************************************************
// CPalette
//******************************************************************************

//******************************************************************************
class CPalette : public CResource {

protected:

public:

                        CPalette();
                        ~CPalette();
};

//******************************************************************************
class CPalette8 : public CResource8 {

protected:

    LPDIRECT3DPALETTE8  m_pd3dp;

public:

                        CPalette8();
                        ~CPalette8();
    virtual BOOL        Create(CDevice8* pDevice, LPDIRECT3DPALETTE8 pd3dp);

    virtual LPDIRECT3DPALETTE8 GetIDirect3DPalette8();

    // Information

    virtual D3DPALETTESIZE GetSize();

    // Locking

    virtual HRESULT     Lock(D3DCOLOR** ppColors, DWORD dwFlags);
    virtual HRESULT     Unlock();
};

#endif // UNDER_XBOX

#endif // __PAL_H__
