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

namespace DirectSound
{
    class CRefCount
    {
    protected:
        DWORD                   m_dwRefCount;

    public:
        CRefCount(DWORD dwInitialRefCount = 1);
        virtual ~CRefCount(void);

    public:
        virtual DWORD STDMETHODCALLTYPE AddRef(void);
        virtual DWORD STDMETHODCALLTYPE Release(void);
    };

    __inline CRefCount::CRefCount(DWORD dwInitialRefCount)
        : m_dwRefCount(dwInitialRefCount)
    {
    }

    __inline CRefCount::~CRefCount(void)
    {
        ASSERT(!m_dwRefCount);
    }

    __inline DWORD CRefCount::AddRef(void)
    {
        ASSERT(m_dwRefCount < ~0UL);
        return ++m_dwRefCount;
    }

    __inline DWORD CRefCount::Release(void)
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
}

#define ADDREF(p) \
    DirectSound::__AddRef(p)

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
