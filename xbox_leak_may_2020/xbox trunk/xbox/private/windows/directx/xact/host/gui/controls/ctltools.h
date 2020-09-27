/***************************************************************************
 *
 *  Copyright (C) 1/30/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ctltools.h
 *  Content:    Control tools.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  1/30/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __CTLTOOLS_H__
#define __CTLTOOLS_H__

#ifdef __cplusplus

//
// CTempSelectObject helper -- saves and restores the previous GDI object
//

class CTempSelectObject
{
protected:
    HGDIOBJ                 m_hgdiSave;
    HDC                     m_hdc;

public:
    CTempSelectObject(void);
    ~CTempSelectObject(void);

public:
    BOOL SelectObject(HDC hdc, HGDIOBJ hgdi);
    void RestoreObject(void);
};

__inline CTempSelectObject::CTempSelectObject(void)
{
    m_hgdiSave = NULL;
    m_hdc = NULL;
}

__inline CTempSelectObject::~CTempSelectObject(void)
{
    RestoreObject();
}

__inline BOOL CTempSelectObject::SelectObject(HDC hdc, HGDIOBJ hgdi)
{
    if(hgdi = ::SelectObject(hdc, hgdi))
    {
        if(HGDI_ERROR == hgdi)
        {
            return FALSE;
        }
    }

    if(m_hgdiSave)
    {
        ASSERT(hdc == m_hdc);
    }
    else
    {
        m_hgdiSave = hgdi;
        m_hdc = hdc;
    }

    return TRUE;
}

__inline void CTempSelectObject::RestoreObject(void)
{
    if(m_hdc && m_hgdiSave)
    {
        ::SelectObject(m_hdc, m_hgdiSave);
    }

    m_hgdiSave = NULL;
    m_hdc = NULL;
}

#endif // __cplusplus

#endif // __CTLTOOLS_H__
