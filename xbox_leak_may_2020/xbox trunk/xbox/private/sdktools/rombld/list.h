//  LIST.H
//
//		Doubly-linked lists
//
//	Created 15-Mar-2000 [JonT]


#ifndef __LIST_H__
#define __LIST_H__

// To use this very simple list package, create a CListHead object then add CNode objects to it

class CListHead;

class CNode
{
private:
	CNode* m_pNext;
	CNode* m_pPrev;

public:
	CNode() 
    { 
        m_pNext = m_pPrev = NULL;
    }
	virtual ~CNode();
	
    CNode* Next() { return m_pNext; }
	CNode* Prev() { return m_pPrev; }

    void Link(CListHead* pList, CNode* pBefore);
	void Unlink(CListHead* pList);

    virtual void Dump();
};

class CListHead
{
private:
	CNode* m_pHead;
	CNode* m_pTail;

public:
	CListHead() { m_pHead = m_pTail = NULL; }
	~CListHead();
	CNode* GetHead(void) { return m_pHead; }
	CNode* GetTail(void) { return m_pTail; }

    void Dump(LPCSTR pszTitle, int nLevel);

    DWORD Count();

friend CNode;
};


#endif // #ifndef __LIST_H__
