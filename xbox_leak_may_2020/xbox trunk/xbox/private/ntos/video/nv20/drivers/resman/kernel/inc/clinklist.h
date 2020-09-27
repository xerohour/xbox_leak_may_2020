#if !defined _CLINKLISTH_
#define _CLINKLISTH_

// C++ class that can be used as a base class by objects that
// are links in a linked list

class CLinkList
{
public:
    CLinkList() { m_pNext = m_pPrev = NULL; }

public:
    virtual VOID Add(CLinkList **ppHead)
    {
        m_pPrev = NULL;
        m_pNext = *ppHead;

        if (*ppHead)
            (*ppHead)->SetPrev(this);

        *ppHead = this;
    }

    virtual VOID Remove(CLinkList **ppHead)
    {
        if (m_pPrev)
            m_pPrev->SetNext(m_pNext);

        if (m_pNext)
            m_pNext->SetPrev(m_pPrev);
    
        if (*ppHead == this)
            *ppHead = m_pNext;
    }
    
    virtual BOOL Exists(CLinkList *pElem)
    {
        CLinkList *pTemp = this;

        while (pTemp)
        {
            if (pTemp == pElem)
                return TRUE;

            pTemp = pTemp->GetNext();
        }
        
        return FALSE;
    }

    CLinkList *GetNext() { return m_pNext; }
    CLinkList *GetPrev() { return m_pPrev; }

    virtual VOID SetNext(CLinkList *pNext) { m_pNext = pNext; }
    virtual VOID SetPrev(CLinkList *pPrev) { m_pPrev = pPrev; }

private:
    CLinkList *m_pNext;
    CLinkList *m_pPrev;
};

#endif