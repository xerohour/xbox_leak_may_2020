// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation, 1996 - 2000.  All rights reserved.
//
// File Name:
//
//  Template classes for singly linked list of specified data type
//
// Abstract:
//
//  Implementation for class
//
// @@BEGIN_MSINTERNAL
//      mitchr      08/08/1998  - created
// @@END_MSINTERNAL
// -------------------------------------------------------------------------------

#ifndef _LIST_H_
#define _LIST_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define CList CKSList
#define CNode CKSNode

// forward 
template<class type> class CList;

// -----------------------------------------------------------------------------
template<class type> class CNode
{
private:
    CNode<type>* pPrev;
    
    CNode<type>* pNext;

public:
    type*        pData;

    CNode(type* pNewData)
    {
        pPrev = pNext = NULL;
        pData = pNewData;
    }

friend CList<type>;
};

// -----------------------------------------------------------------------------
template<class type> class CList
{
private:
    CNode<type>* m_pHead;
    CNode<type>* m_pTail;
    UINT         m_cMembers;

public:
    CList(void);
    
    ~CList(void);

    CNode<type>* GetHead(void)
    {
        return m_pHead;
    }
    CNode<type>* GetTail(void)
    {
        return m_pTail;
    }
    CNode<type>* GetNext(CNode<type>* pNode)
    {
        return pNode->pNext;
    }

    CNode<type>* AddTail(type*);
    
    CNode<type>* AddHead(type*);
    
    CNode<type>* Find(type*);
    
    void         Remove(CNode<type>*);
    
    void         Empty(void);
    
    UINT         GetCount(void)  { return m_cMembers; }
    
    BOOL         IsEmpty(void)  { return (m_cMembers == 0); }
};

// ---------------------
template<class type> 
CList<type>::CList(void)
{
    m_pHead = NULL;
    m_pTail = NULL;
    m_cMembers = 0;
}

// ---------------------
template<class type> 
CList<type>::~CList(void)
{
    Empty();
}

// ---------------------
template<class type> 
CNode<type>*
CList<type>::AddHead(type* pNewData)
{
    CNode<type>* pNewNode = new CNode<type>(pNewData);

    if(pNewNode)
    {
        if(m_pHead)
        {
            m_pHead->pPrev = pNewNode;
            pNewNode->pPrev = NULL;
            pNewNode->pNext = m_pHead;
        }

        m_pHead = pNewNode;
        if(!m_pTail)
            m_pTail = pNewNode;

        m_cMembers++;
    }

    return pNewNode;
}

// ---------------------
template<class type> 
CNode<type>*
CList<type>::AddTail(type* pNewData)
{
    CNode<type>* pNewNode = new CNode<type>(pNewData);

    if(pNewNode)
    {
        if(m_pTail)
        {
            m_pTail->pNext = pNewNode;
            pNewNode->pNext = NULL;
            pNewNode->pPrev = m_pTail;
        }
        m_pTail = pNewNode;
        if(!m_pHead)
            m_pHead = pNewNode;

        m_cMembers++;
    }

    return pNewNode;
}

// ---------------------
template<class type> 
CNode<type>*
CList<type>::Find(type* pDataFind)
{
    CNode<type>* pNode = m_pHead;

    while(pNode)
    {
        if(pNode->pData == pDataFind)
            return pNode;

        pNode = pNode->pNext;
    }

    return NULL;
}

// ---------------------
template<class type> 
void
CList<type>::Remove(CNode<type>* pDeadNode)
{
    if(pDeadNode)
    {
        if(pDeadNode->pPrev)
            pDeadNode->pPrev->pNext = pDeadNode->pNext;
        else
            m_pHead = pDeadNode->pNext;

        if(pDeadNode->pNext)
            pDeadNode->pNext->pPrev = pDeadNode->pPrev;
        else
            m_pTail = pDeadNode->pPrev;

        delete pDeadNode;
        m_cMembers--;
    }
}

// ---------------------
template<class type> 
void 
CList<type>::Empty(void)
{
    CNode<type>* pNext;

    while(m_pHead)
    {
        pNext = m_pHead->pNext;
        delete m_pHead;
        m_pHead = pNext;
    }

    m_pHead = m_pTail = NULL;
    m_cMembers = 0;
}

#endif // #ifndef _LIST_
