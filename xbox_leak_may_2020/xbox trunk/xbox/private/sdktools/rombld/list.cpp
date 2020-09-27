//  LIST.CPP
//
//		Doubly-linked lists
//
//	Created 15-Mar-2000 [JonT]

#include "rombld.h"

//---------------------------------------------------------------------
//  CListHead class

CListHead::~CListHead()
{
	CNode* pNode;

	// Nuke all remaining nodes in the list
	while (m_pHead)
	{
		pNode = m_pHead;
		m_pHead = m_pHead->Next();
		delete pNode;
	}
}


void
CListHead::Dump(
    LPCSTR pszTitle, 
    int nLevel                
    )
{

    if (nLevel > g_ib.m_nTraceLevel)
    {
        return;
    }

//    g_ib.m_dwDumpVar = 0;

    if (pszTitle != NULL && pszTitle[0] != '\0')
    {
        TRACE_OUT(nLevel, "\n%s:", pszTitle);
    }

    for (CNode* pNode = m_pHead; pNode != NULL; pNode = pNode->Next())
    {
        pNode->Dump();
    }
}


DWORD
CListHead::Count()
{
    DWORD dwCnt = 0;

    for (CNode* pNode = m_pHead; pNode != NULL; pNode = pNode->Next())
    {
        dwCnt++;
    }
    return dwCnt;
}


//---------------------------------------------------------------------
//  CNode class
//		Using this class is very simple: Start by doing a 'new CNode.' or a derived class
//		Once you have a new CNode, link it into a list by passing to CNode::Link
//		pointing to the list head already created and to the node to insert after.
//		Pass in NULL to link to the end of the list.
//		Don't delete a CNode before Unlink'ing it!

CNode::~CNode()
{
    // derived class should clean up in its own destructor
}


//  CNode::Link
//		Links in an element to the list before the node passed in.
//		Pass in NULL to link at end of list.

void
CNode::Link(
	CListHead* pList,
	CNode* pBefore
	)
{
    if (!pList->m_pHead)
    {
        m_pPrev = m_pNext = NULL;
        pList->m_pHead = pList->m_pTail = this;
    }
    else if (pBefore == NULL)
    {
        m_pPrev = pList->m_pTail;
        m_pNext = NULL;
        pList->m_pTail->m_pNext = this;
        pList->m_pTail = this;
    }
    else
    {
        m_pPrev = pBefore->m_pPrev;
        m_pNext = pBefore;
        if (pBefore->m_pPrev == NULL)
            pList->m_pHead = this;
        else
            pBefore->m_pPrev->m_pNext = this;
        pBefore->m_pPrev = this;
    }
}


//  CNode::Unlink
//		Removed the current element from the list. The memory is not reclaimed
//		until the node is delete'ed.

void
CNode::Unlink(
	CListHead* pList
	)
{
    // Check the previous element
    if (!m_pPrev)
        pList->m_pHead = m_pNext;
    else
        m_pPrev->m_pNext = m_pNext;

    // Check the next element
    if (!m_pNext)
        pList->m_pTail = m_pPrev;
    else
        m_pNext->m_pPrev = m_pPrev;
}


void
CNode::Dump()
{
}