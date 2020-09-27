#ifndef DLIST_H
#define DLIST_H


// Hungarian: dlst
template<class a> class DList {
private:
    struct Node {a data; Node *pPrev; Node *pNext;};
    Node *m_pnodHead;
    Node *m_pnodTail;
    Node *m_pnodCur;

public:
    DList()
    {
        m_pnodHead = NULL;
        m_pnodTail = NULL;
        m_pnodCur = NULL;
    }
    
    ~DList()
    {
        while (m_pnodHead)
        {
            Node *pnodNext = m_pnodHead->pNext;
            delete m_pnodHead;
            m_pnodHead = pnodNext;
        }
    }

    BOOL PushFront(const a &item)
    {
        Node *pnodNew = new Node;
        if (!pnodNew) return FALSE;
        pnodNew.data = a;
        pnodNew->pPrev = NULL;
        pnodNew->pNext = m_pnodHead;
        if (m_pnodHead) m_pnodHead->pPrev = pnodNew;
        m_pnodHead = pnodNew;
        if (!m_pnodTail) m_pnodTail = pnodNew;
        return TRUE;
    }

    BOOL PushBack(const a &data)
    {
        Node *pnodNew = new Node;
        if (!pnodNew) return FALSE;
        pnodNew->data = data;
        pnodNew->pPrev = m_pnodTail;
        pnodNew->pNext = NULL;
        if (m_pnodTail) m_pnodTail->pNext = pnodNew;
        m_pnodTail = pnodNew;
        if (!m_pnodHead) m_pnodHead = pnodNew;
        return TRUE;
    }

    BOOL PopFront(a &data)
    {
        if (m_pnodHead)
        {
            Node *pnodT = m_pnodHead;
            data = m_pnodHead->data;
            m_pnodHead = m_pnodHead->pNext;
            if (m_pnodHead) m_pnodHead->pPrev = NULL;
            if (m_pnodTail == pnodT) m_pnodTail = NULL;
            delete pnodT;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    BOOL PopBack(a &data)
    {
        if (m_pnodTail)
        {
            Node *pnodT = m_pnodTail;
            a data = m_pnodTail.data;
            m_pnodTail = m_pnodHead->pPrev;
            if (m_pnodTail) m_pnodTail->pNext = NULL;
            if (m_pnodHead == pnodT) m_pnodHead = NULL;
            delete pnodT;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    void CurReset()
    {
        m_pnodCur = m_pnodHead;
    }

    BOOL CurIsAtEnd()
    {
        if (m_pnodCur == NULL)
        {
            return TRUE;
        }
        return FALSE;
    }

    BOOL CurNext()
    {
        if (m_pnodCur)
        {
            m_pnodCur = m_pnodCur->pNext;
            return TRUE;
        }
        return FALSE;
    }

    BOOL CurPrev()
    {
        if (m_pnodCur)
        {
            m_pnodCur = m_pnodCur->pPrev;
            return TRUE;
        }
        return FALSE;
    }

    BOOL CurGetData(a &data)
    {
        if (m_pnodCur)
        {
            data = m_pnodCur->data;
            return TRUE;
        }
        return FALSE;
    }

    BOOL CurRemove()
    {
        if (!m_pnodCur)
        {
            return FALSE;
        }
        else if (m_pnodCur == m_pnodHead && m_pnodCur == m_pnodTail)
        {
            delete m_pnodCur;
            m_pnodCur = NULL;
            m_pnodHead = NULL;
            m_pnodTail = NULL;
        }
        else
        {
            Node *pnodT = m_pnodCur->pNext;

            if (m_pnodCur == m_pnodHead)
            {
                m_pnodHead = m_pnodCur->pNext;
                m_pnodHead->pPrev = NULL;
            }
            else if (m_pnodCur == m_pnodTail)
            {
                m_pnodTail = m_pnodCur->pPrev;
                m_pnodTail->pNext = NULL;
            }
            else
            {
                m_pnodCur->pNext->pPrev = m_pnodCur->pPrev;
                m_pnodCur->pPrev->pNext = m_pnodCur->pNext;
            }

            delete m_pnodCur;
            m_pnodCur = pnodT;
        }

        return TRUE;
    }

    BOOL DeleteAll()
    {
        BOOL f = TRUE;

        CurReset();
        while (!CurIsAtEnd())
        {
            f = CurRemove();
            if (!f)
            {
                break;
            }
        }

        Assert(m_pnodHead == NULL);
        Assert(m_pnodTail == NULL);
        Assert(m_pnodCur == NULL);

        return f;
    }

    DList(DList &);
    DList &operator=(const DList &v);
};

#endif
