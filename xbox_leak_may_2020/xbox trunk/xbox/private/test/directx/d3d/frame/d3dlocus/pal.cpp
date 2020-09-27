/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    pal.cpp

Author:

    Matt Bronder

Description:

    CPalette methods.

*******************************************************************************/

#include "d3dlocus.h"

#ifdef UNDER_XBOX

//******************************************************************************
// CIndexBuffer
//******************************************************************************

//******************************************************************************
CPalette::CPalette() {
}

//******************************************************************************
CPalette::~CPalette() {
}

//******************************************************************************
// CIndexBuffer8
//******************************************************************************

//******************************************************************************
CPalette8::CPalette8() {

    m_pd3dp = NULL;
}

//******************************************************************************
CPalette8::~CPalette8() {
}

//******************************************************************************
BOOL CPalette8::Create(CDevice8* pDevice, LPDIRECT3DPALETTE8 pd3dp) {

    if (m_pd3dp) {
        return FALSE;
    }

    if (pDevice && pd3dp) {

        if (CResource8::Create(pDevice, (LPDIRECT3DRESOURCE8)pd3dp)) {
            m_pd3dp = pd3dp;
        }
    }

    return (BOOL)m_pd3dp;
}

//******************************************************************************
LPDIRECT3DPALETTE8 CPalette8::GetIDirect3DPalette8() {

    return m_pd3dp;
}

//******************************************************************************
D3DPALETTESIZE CPalette8::GetSize() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dp) {
        return D3DPALETTE_256;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dp->GetSize();
}

//******************************************************************************
HRESULT CPalette8::Lock(D3DCOLOR** ppColors, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dp) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dp->Lock(ppColors, dwFlags);
}

//******************************************************************************
HRESULT CPalette8::Unlock() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dp) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dp->Unlock();
}

#endif // UNDER_XBOX
