// tree.h

#pragma once

/*
 *
 * CTreeItem handles a node in a tree list. 
 * The node maintains pointers to its parent node, 
 * to its adjacent siblings, and to its first 
 * and last children.
 *
 */
class CTreeItem
{
    CTreeItem* m_pPrev;         // points to previous sibling
    CTreeItem* m_pNext;         // points to next sibling
    CTreeItem* m_pParent;       // points to parent
    CTreeItem* m_pFirst;        // points to first child
    CTreeItem* m_pLast;         // points to last child

//    LPTSTR m_name;              // item text
	CAutoDelete<_TCHAR> m_name;

public:     // constructor and destructor
    CTreeItem (LPCTSTR s = NULL);
    virtual ~CTreeItem();

public:     // class functions
    void AddChild (CTreeItem* pChild, BOOL bFirst = FALSE);
    void AddChildAfter (CTreeItem* pChild,CTreeItem* pPrev);
    void AddChildBefore (CTreeItem* pChild,CTreeItem* pNext);
    CTreeItem* CreateChild (LPCTSTR s = NULL);
    CTreeItem* CreateSibing (LPCTSTR s = NULL);
    void DeleteChildren ();
	CTreeItem *FindChild (LPCTSTR name) const;
	int GetChildCount () const;
    CTreeItem* GetFirstChild() const
        {
        return m_pFirst;
        }
    CTreeItem* GetLastChild() const
        {
        return m_pLast;
        }
//    GetLevel() const;
//    GetNChildren() const;
    CTreeItem* GetNextSibling() const
        {
        return m_pNext;
        }
    CTreeItem* GetParent() const
        {
        return m_pParent;
        }
    CTreeItem* GetPrevSib() const
        {
        return m_pPrev;
        }
    CTreeItem* GetRoot();
    BOOL HasChildren() const
        {
        return(m_pFirst? TRUE:FALSE);
        }
    LPCTSTR GetName ()
		{
			return m_name.get();
		}
    BOOL HasParent() const
        {
        return(m_pParent ? TRUE : FALSE);
        }
    BOOL IsAncestor (const CTreeItem* pTest) const;
    BOOL IsDescendant (const CTreeItem* pTest) const;
    BOOL IsLinked () const
        {
        return((m_pNext || m_pPrev)? TRUE:FALSE);
        }
    BOOL IsParent (const CTreeItem* pTest) const
        {
        return((pTest->m_pParent == this) ? TRUE : FALSE);
        }
    void LinkAfter (CTreeItem* pPrev);
    void LinkBefore (CTreeItem* pNext);
	LPTSTR NewTSTR (LPCTSTR str);
    void RemoveChild (CTreeItem* pChild);
    void SetName (LPCTSTR s);
    void Unlink ();
};    
