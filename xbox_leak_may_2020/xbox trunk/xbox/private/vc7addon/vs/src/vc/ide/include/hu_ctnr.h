//---------------------------------------------------------------------------
// Microsoft Visual InterDev
//
// Microsoft Confidential
// Copyright 1994 - 1997 Microsoft Corporation. All Rights Reserved.
//
// File: hu_ctnr.h
// Area: Namespace utilities
// Contents: CHierContainer class
//	Description: Derived from CHierNode class, it is a container
// 	(ie. non-leaf) element in a heirarchy.
//---------------------------------------------------------------------------

#ifndef __HU_CTNR_H__
#define __HU_CTNR_H__

#ifndef  __HU_NODE_H__
#include "hu_node.h"
#endif

#include "hu_archy.h"

//-----------------------------------------------------------------------------
// Name: CHierContainer
//
// Description:
//	Class for every object in a hierarchy that has children. Implements the
//	idea of a node that has children, relies on CHierNode to take care of
//	parent/sibling info.
//
//-----------------------------------------------------------------------------
class CHierContainer : 
	public CHierImpl<CHierContainer, CHierNode, Type_CHierContainer>
{
protected:
			 CHierContainer(void);


public: // CHierNode overrides
	virtual ~CHierContainer(void);
	virtual VSITEMID GetFirstChild(void);

public:
	HRESULT SetProperty(VSHPROPID propid, const VARIANT& var);
	// CHierNode virtual methods
	virtual BOOL		IsContainer(void) const;	// returns TRUE
	virtual BOOL		IsTypeOf(int nodeType) const;

	// CHierContainer virtual methods
	virtual HRESULT		Refresh(CVsHierarchy *pCVsHierarchy);
	virtual HRESULT		EnumerateChildren(void) = 0;
	virtual	UINT		GetKindOf(void) const;
	virtual HRESULT DisplayContextMenu(VARIANT *pvaIn);

	// Property functions
	virtual HRESULT		GetProperty(VSHPROPID propid, VARIANT* pvar);

	virtual CHierNode*	GetHeadEx(void);

	CHierNode* GetHead(void) const
	{
		return m_pHeadNode;
	}
	CHierNode* GetTail(void) const
	{
		return m_pTailNode;
	}
	long GetCount(void) const;	// return number of children

	CHierNode* GetPrevChildOf(const CHierNode* pCurrent) const;

	// Override to get custom add behavior suchas keeping the list sorted.
	// Default calls AddTail(). 
	virtual void Add(CHierNode *const pNode);

	void 	AddAfter(CHierNode* const pCurrNode, CHierNode* const pNewNode);
	void	AddHead(CHierNode *const pNode);
	void	AddTail(CHierNode *const pNode);
	virtual HRESULT Remove(CHierNode* pNode);
	virtual HRESULT Delete(CHierNode* pNode, CVsHierarchy *pCVsHierarchy);
	virtual void	DeleteAll(CVsHierarchy *pCVsHierarchy);

	// returns the node from child list who's GetCaption() == pszCaption
	CHierNode *GetNodeByCaption(LPCTSTR pszCaption);
	CHierNode *GetNodeByIndex(DWORD dwIndex);
	CHierNode *GetNodeByVariant(const VARIANT *pvar);

	virtual const CString& GetCaption(void) = 0;
	virtual const CString& GetFullPath(void) = 0;
	virtual void SetCaption(LPCSTR pszNewCaption) = 0;
	virtual void SetCaption(LPCWSTR pszNewCaption) = 0;
	virtual void SetCaption(const CString& strNewCaption) = 0;
	virtual	HRESULT			CleanUpBeforeDelete() = 0;


	void SetRevaluateChildren() { m_grfStateFlags &= ~ST_ChildrenEnumerated; }

	BOOL HaveChildrenBeenEvaluated(void) const
	{
		return (m_grfStateFlags & ST_ChildrenEnumerated);
	}
	BOOL IsExpanded(void) const
	{
		return (m_grfStateFlags & ST_IsExpanded);
	}

protected:
	void NotifyHierarchyOfAdd(CHierNode* pNodePrev, CHierNode* pNodeAdded)
	{
		if (HaveChildrenBeenEvaluated())
		{
			GetCVsHierarchy()->OnItemAdded(this, pNodePrev, pNodeAdded);
		}
	}

	BOOL m_fAutoExpand;
	DWORD		m_grfStateFlags;		// ChildrenEnumerated, IsExpanded, etc
	enum	// m_grfStateFlags
	{
		ST_ChildrenEnumerated = (1<<0),
		ST_IsExpanded = (1<<1),
		ST_FirstUserFlag = (1<<16)	// Derived classes are free to use these upper bits
	};

	CHierNode*	m_pHeadNode;
	CHierNode*	m_pTailNode;
};

#endif // __HU_CTNR_H__

