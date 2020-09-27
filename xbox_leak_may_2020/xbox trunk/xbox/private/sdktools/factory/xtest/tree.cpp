// tree.cpp


#include "stdafx.h"


/*
 * CTreeItem
 */

/*
 * This class handles a node in a tree. Each node is linked to
 * its adjacent siblings, its parent, and its first and last
 * child.
 */
CTreeItem::CTreeItem (LPCTSTR s)
{
	m_pPrev = m_pNext = 0;
	m_pFirst = m_pLast = 0;
	m_pParent = 0;
	if (s) 
		SetName (s);
}

/*
 * The destructor unlinks the node from its sibling list and its parent if 
 * those operations have not already been done.  It also deletes all child
 * nodes.
 */ 
CTreeItem::~CTreeItem()
{
	Unlink();
	DeleteChildren();
//	if (m_name) 
//		delete m_name;
}

/*
 * Create a new string
 */
LPTSTR CTreeItem::NewTSTR (LPCTSTR str)
{
	if (str == NULL)
		return NULL;

	LPTSTR copy = new _TCHAR [_tcslen (str) + 1];
	_tcscpy (copy, str);
	return copy;
}

void CTreeItem::SetName (LPCTSTR s)
{
	m_name.set (NewTSTR (s));
}

/*
 * This function unlinks the node from its sibling list and its parent.  It 
 * does not disturb the children.
 */
void CTreeItem::Unlink()
{    
	if(m_pParent)
	{
		if (m_pParent->m_pFirst == this) 
			m_pParent->m_pFirst = m_pNext;
		if (m_pParent->m_pLast == this) 
			m_pParent->m_pLast = m_pPrev;
	}

	if(m_pPrev) 
		m_pPrev->m_pNext = m_pNext;
	if(m_pNext) 
		m_pNext->m_pPrev = m_pPrev;

	m_pPrev = m_pNext = m_pParent = 0;
}

/*
 * These functions link a node to a previous or next sibling.
 */
void CTreeItem::LinkAfter(CTreeItem* pPrev)
{
	ASSERT(!IsLinked());
	ASSERT(pPrev != NULL);
	m_pNext = pPrev->m_pNext;
	m_pPrev = pPrev;
	pPrev->m_pNext = this;
	if (m_pNext) 
		m_pNext->m_pPrev = this;
	m_pParent = pPrev->GetParent();
	if (m_pParent && (m_pNext == 0)) 
		m_pParent->m_pLast = this;
}

void CTreeItem::LinkBefore(CTreeItem* pNext)
{
	ASSERT(!IsLinked());
	ASSERT(pNext != NULL);
	m_pPrev = pNext->m_pPrev;
	m_pNext = pNext;
	pNext->m_pPrev = this;
	if (m_pPrev) 
		m_pPrev->m_pNext = this;
	m_pParent = pNext->GetParent();
	if (m_pParent && (m_pPrev == 0)) 
		m_pParent->m_pFirst = this;
}

/*
 * These functions add a child.
 */

void CTreeItem::AddChild (CTreeItem* pChild, BOOL bFirst)
{
	ASSERT(pChild != NULL);
	ASSERT(!pChild->IsLinked());

	pChild->m_pParent = this;
	if (bFirst)
	{
		pChild->m_pNext = m_pFirst;
		pChild->m_pPrev = 0;
		if (m_pFirst) 
			m_pFirst->m_pPrev = pChild;
		m_pFirst = pChild;
		if(m_pLast == 0) 
			m_pLast = pChild; 
	}
	else
	{
		pChild->m_pPrev = m_pLast;
		pChild->m_pNext = 0;
		if (m_pLast) 
			m_pLast->m_pNext = pChild;
		m_pLast = pChild;
		if (m_pFirst == 0) 
			m_pFirst = pChild;
	}
}        

void CTreeItem::AddChildAfter (CTreeItem* pChild, CTreeItem* pPrev)
{
	ASSERT(pChild != NULL);
	ASSERT(!pChild->IsLinked());
	if (pPrev) 
		ASSERT(IsParent(pPrev));

	pChild->m_pParent = this;
	if (pPrev == NULL) 
		AddChild (pChild, FALSE);
	pChild->LinkAfter(pPrev);
}

void CTreeItem::AddChildBefore (CTreeItem* pChild, CTreeItem* pNext)
{
	ASSERT(pChild != NULL);
	ASSERT(!pChild->IsLinked());
	if (pNext) 
		ASSERT(IsParent(pNext));

	pChild->m_pParent = this;
	if (pNext == NULL) 
		AddChild(pChild,TRUE);
	pChild->LinkBefore(pNext);
}

CTreeItem* CTreeItem::CreateChild (LPCTSTR s)
{
	CTreeItem* pItem = new CTreeItem(s);
	if (pItem) 
		AddChild (pItem);
	return pItem;
}

/*
 * Remove a node from the child list;
 */
void CTreeItem::RemoveChild(CTreeItem* pChild)
{
	if (IsParent (pChild)) 
		pChild->Unlink();
}

/*
 * This function deletes all children.
 */
void CTreeItem::DeleteChildren()
{
	while (m_pFirst) 
		delete m_pFirst;
}

/*
 * Test if this object is a descendant of a specified object.
 */ 
BOOL CTreeItem::IsDescendant (const CTreeItem* pTest) const
{
	CTreeItem* p;
	BOOL ret = FALSE;
	ASSERT(pTest);

	for (p = m_pParent; p; p = p->m_pParent) 
		if (p == pTest)
		{
			ret = TRUE;
			break; 
		}

	return(ret);
}

/*
 * Test if this object is an ancestor of a specified object.
 */
BOOL CTreeItem::IsAncestor (const CTreeItem* pTest) const
{
	ASSERT(pTest);
	return pTest->IsDescendant (this);
}

/*
 * Find the root of the tree.
 */
CTreeItem* CTreeItem::GetRoot()
{
	CTreeItem* pItem = this;
	for (pItem = this; pItem->GetParent(); pItem = pItem->GetParent())
		{}
	return pItem;
}    

/*
 * Count the number of children in this branch.
 */
int CTreeItem::GetChildCount () const
{
	CTreeItem* p;
	int count = 0;
	for (p = GetFirstChild(); p; p = p->GetNextSibling())
		count += 1 + p->GetChildCount();

	return count;
}

/*
 * Find a child with a name that matches the arguement.
 */
CTreeItem *CTreeItem::FindChild (LPCTSTR name) const
{
	if (name != NULL)
	{
		CTreeItem* p;
		for (p = GetFirstChild(); p; p = p->GetNextSibling())
		{
			if (_tcsicmp (p->GetName(), name) == 0)
				return p;
		}
	}

	return NULL;
}
