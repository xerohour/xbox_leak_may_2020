//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xlist.h
//
//  Description:    list related classes header file.
//
//  History:
//      01/08/98    CongpaY     Created
//
//**************************************************************************

#ifndef _XLIST_H_
#define _XLIST_H_

template<class T, DWORD cChunk>
class XArray
{
    DWORD m_cUsed;
    DWORD m_cTotal;
    DWORD m_cChunk;

protected:
    T *m_pData;

public:
    XArray(DWORD count = 0, T *data = NULL)
      : m_cUsed(count),
        m_cTotal(count),
        m_cChunk(cChunk),
        m_pData(data) {}

    ~XArray() { XFree(m_pData); }

    const T &operator[](int i) const { return m_pData[i]; }

    const DWORD count() { return m_cUsed; }
    const T *list() { return m_pData; }

    int Add(T & tSrc)
    {
        if (m_cUsed == m_cTotal) {
            m_cTotal += m_cChunk;
            if (FAILED(XRealloc((void **)&m_pData, m_cTotal * sizeof(T))))
                return 0;
        }

        m_pData[m_cUsed] = tSrc;
        return ++m_cUsed;
    }
};

template<class T, DWORD cChunk>
class XPtrArray : public XArray<T *, cChunk>
{
public:
    ~XPtrArray()
    {
        if (m_pData) {
            for (DWORD i = 0; i < count(); i++)
                delete m_pData[i];
        }
    }

};

template<class T, DWORD cChunk>
class XObjPtrArray : public XArray<T *, cChunk>
{
public:
    ~XObjPtrArray()
    {
        if (m_pData) {
            for (DWORD i = 0; i < count(); i++)
                m_pData[i]->Release();
        }
    }
};

#endif // _XLIST_H_
