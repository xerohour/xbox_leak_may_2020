#ifndef _BLIST_H
#define _BLIST_H

//////////////////////////////////////////////////////////////////////////////
// Double Linked List
//
template<class T,class R,BOOL ts = FALSE> class BLinkedList
{
    //
    // private structures
    //
protected:
    struct ENTRY
    {
        ENTRY *m_pNext;
        ENTRY *m_pPrev;
        T      m_Data;
    };

    //
    // members
    //
protected:
    ENTRY *m_pHead;
    ENTRY *m_pTail;
protected:
    CRITICAL_SECTION m_csThread;

    //
    // methods
    //
public:
    inline void addHead (R data); // require operator=
    inline void addTail (R data); // require operator=
    inline void addHead (const BLinkedList<T,R,ts>& data); // require operator=
    inline void addTail (const BLinkedList<T,R,ts>& data); // require operator=

    inline void removeHead (void);
    inline void removeTail (void);
    inline void removeAll  (void);
    inline void removeAt   (BPosition pos);

    inline R         getHead             (void) const;
    inline R         getTail             (void) const;
    inline BPosition getHeadPosition     (void) const;
    inline BPosition getTailPosition     (void) const;
    inline R         getNextPosition     (BPosition& pos) const;
    inline R         getPreviousPosition (BPosition& pos) const;

    inline BPosition getIndexPosition (int index)     const; // order n since this is not an array
    inline R         getData          (BPosition pos) const;

    inline BPosition findForward  (R match,BPosition posStartFrom) const; // require operator==
    inline BPosition findBackward (R match,BPosition posStartFrom) const; // require operator==

    inline int isEmpty (void) const { return m_pHead == NULL; }

    inline const BLinkedList<T,R,ts>& operator= (const BLinkedList<T,R,ts>& c)
    {
        addTail (c);
        return *this;
    }

    //
    // construction
    //
public:
    inline  BLinkedList (void) 
    { 
        m_pHead = m_pTail = NULL; 
        if (ts) InitializeCriticalSection (&m_csThread);
    }
    inline ~BLinkedList (void) 
    { 
        removeAll(); 
        if (ts) DeleteCriticalSection (&m_csThread);
    }
};

//
// implementation
//
template<class T,class R,BOOL ts> void BLinkedList<T,R,ts>::addHead (R data)
{
    if (ts) EnterCriticalSection (&m_csThread);
    ENTRY *pEntry = new ENTRY;
    if (!pEntry) throw BException(BException::outOfMemory);
    pEntry->m_pNext = m_pHead;
    pEntry->m_pPrev = NULL;
    pEntry->m_Data  = data;
    if (m_pHead) m_pHead->m_pPrev = pEntry;
            else m_pTail          = pEntry;
    m_pHead = pEntry;
    if (ts) LeaveCriticalSection (&m_csThread);
}

template<class T,class R,BOOL ts> void BLinkedList<T,R,ts>::addTail (R data)
{
    if (ts) EnterCriticalSection (&m_csThread);
    ENTRY *pEntry = new ENTRY;
    if (!pEntry) throw BException(BException::outOfMemory);
    pEntry->m_pNext = NULL;
    pEntry->m_pPrev = m_pTail;
    pEntry->m_Data  = data;
    if (m_pTail) m_pTail->m_pNext = pEntry;
            else m_pHead          = pEntry;
    m_pTail = pEntry;
    if (ts) LeaveCriticalSection (&m_csThread);
}

template<class T,class R,BOOL ts> void BLinkedList<T,R,ts>::addHead (const BLinkedList<T,R,ts>& data)
{
    if (ts) EnterCriticalSection (&m_csThread);
    ENTRY *pEntry = (ENTRY*)data.m_pTail;
    while (pEntry)
    {
        addHead (pEntry->m_Data);
        pEntry = pEntry->m_pPrev;
    }
    if (ts) LeaveCriticalSection (&m_csThread);
}

template<class T,class R,BOOL ts> void BLinkedList<T,R,ts>::addTail (const BLinkedList<T,R,ts>& data)
{
    if (ts) EnterCriticalSection (&m_csThread);
    ENTRY *pEntry = (ENTRY*)data.m_pHead;
    while (pEntry)
    {
        addTail (pEntry->m_Data);
        pEntry = pEntry->m_pNext;
    }
    if (ts) LeaveCriticalSection (&m_csThread);
}
 
template<class T,class R,BOOL ts> void BLinkedList<T,R,ts>::removeHead (void)
{
    if (ts) EnterCriticalSection (&m_csThread);
    if (m_pHead)
    {
        removeAt (BPosition(m_pHead));
    }
    if (ts) LeaveCriticalSection (&m_csThread);
}

template<class T,class R,BOOL ts> void BLinkedList<T,R,ts>::removeTail (void)
{
    if (ts) EnterCriticalSection (&m_csThread);
    if (m_pTail)
    {
        removeAt (BPosition(m_pTail));
    }
    if (ts) LeaveCriticalSection (&m_csThread);
}

template<class T,class R,BOOL ts> void BLinkedList<T,R,ts>::removeAll (void)
{
    if (ts) EnterCriticalSection (&m_csThread);
    while (m_pHead) removeHead();
    if (ts) LeaveCriticalSection (&m_csThread);
}

template<class T,class R,BOOL ts> void BLinkedList<T,R,ts>::removeAt (BPosition pos)
{
    if (ts) EnterCriticalSection (&m_csThread);
    assert (pos);
    ENTRY *pEntry = (ENTRY*)pos;
    if (pEntry->m_pNext) pEntry->m_pNext->m_pPrev = pEntry->m_pPrev;
                    else m_pTail                  = pEntry->m_pPrev;
    if (pEntry->m_pPrev) pEntry->m_pPrev->m_pNext = pEntry->m_pNext;
                    else m_pHead                  = pEntry->m_pNext;
    delete pEntry;
    if (ts) LeaveCriticalSection (&m_csThread);
}

template<class T,class R,BOOL ts> R BLinkedList<T,R,ts>::getHead (void) const
{
    assert (m_pHead);
    return m_pHead->m_Data;
}

template<class T,class R,BOOL ts> R BLinkedList<T,R,ts>::getTail (void) const
{
    assert (m_pTail);
    return m_pTail->m_Data;
}

template<class T,class R,BOOL ts> BPosition BLinkedList<T,R,ts>::getHeadPosition (void) const
{
    return BPosition(m_pHead);
}

template<class T,class R,BOOL ts> BPosition BLinkedList<T,R,ts>::getTailPosition (void) const
{
    return BPosition(m_pTail);
}

template<class T,class R,BOOL ts> R BLinkedList<T,R,ts>::getNextPosition (BPosition& pos) const
{
    assert (pos);
    ENTRY *pEntry = (ENTRY*)pos;
    pos = BPosition(pEntry->m_pNext);
    return pEntry->m_Data;
}

template<class T,class R,BOOL ts> R BLinkedList<T,R,ts>::getPreviousPosition (BPosition& pos) const
{
    assert (pos);
    ENTRY *pEntry = (ENTRY*)pos;
    pos = BPosition(pEntry->m_pPrev);
    return pEntry->m_Data;
}

template<class T,class R,BOOL ts> BPosition BLinkedList<T,R,ts>::getIndexPosition (int index) const // order n since this is not an array
{
    ENTRY *pEntry = m_pHead;
    while (pEntry && index)
    {
        index  --;
        pEntry = pEntry->m_pNext;
    }
    // done
    return BPosition(pEntry);
}

template<class T,class R,BOOL ts> R BLinkedList<T,R,ts>::getData (BPosition pos) const
{
    assert (pos);
    return ((ENTRY*)pos)->m_Data;
}

template<class T,class R,BOOL ts> BPosition BLinkedList<T,R,ts>::findForward (R match,BPosition posStartFrom) const
{
    ENTRY *pEntry = (ENTRY*)posStartFrom;
    while (pEntry)
    {
        if (pEntry->m_Data == match) return BPosition(pEntry);
        pEntry = pEntry->m_pNext;
    }
    // not found
    return BPosition(NULL);
}

template<class T,class R,BOOL ts> BPosition BLinkedList<T,R,ts>::findBackward (R match,BPosition posStartFrom) const
{
    ENTRY *pEntry = (ENTRY*)posStartFrom;
    while (pEntry)
    {
        if (pEntry->m_Data == match) return BPosition(pEntry);
        pEntry = pEntry->m_pPrev;
    }
    // not found
    return BPosition(NULL);
}

#endif