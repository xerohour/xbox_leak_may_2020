/***************************************************************************
 *
 *  Copyright (C) 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       refcount.h
 *  Content:    Basic reference-counting class.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  08/06/01    dereks  Created.
 *
 ****************************************************************************/

#ifndef __REFCOUNT_H__
#define __REFCOUNT_H__

#ifdef __cplusplus

class CRefCountTest
{
protected:
    DWORD                   m_dwRefCount;

public:
    CRefCountTest(DWORD dwInitialRefCount = 1);
    virtual ~CRefCountTest(void);

public:
    virtual DWORD STDMETHODCALLTYPE AddRef(void);
    virtual DWORD STDMETHODCALLTYPE Release(void);
};

__inline CRefCountTest::CRefCountTest(DWORD dwInitialRefCount)
    : m_dwRefCount(dwInitialRefCount)
{
}

__inline CRefCountTest::~CRefCountTest(void)
{
    ASSERT(!m_dwRefCount);
}

__inline DWORD CRefCountTest::AddRef(void)
{
    ASSERT(m_dwRefCount < ~0UL);
    return ++m_dwRefCount;
}

__inline DWORD CRefCountTest::Release(void)
{
    ASSERT(m_dwRefCount);

    if(m_dwRefCount > 0)
    {
        if(!--m_dwRefCount)
        {
            delete this;
            return 0;
        }
    }

    return m_dwRefCount;
}

template <class type> type *__AddRef(type *p)
{
    if(p)
    {
        p->AddRef();
    }

    return p;
}

#define ADDREF(p) \
    __AddRef(p)

#define RELEASE(p) \
    { \
        if(p) \
        { \
            (p)->Release(); \
            (p) = NULL; \
        } \
    }

#endif // __cplusplus

#endif // __REFCOUNT_H__
