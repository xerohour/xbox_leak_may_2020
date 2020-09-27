// spytreed.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpyTreeDoc document

class CSpyTreeDoc : public CDocument
{
	DECLARE_DYNCREATE(CSpyTreeDoc);

protected:
	CSpyTreeDoc();		  // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
	virtual void InitializeData()
	{
		//
		// This must be overridden to be useful.
		//
		ASSERT(0);
	}

	virtual void SetSearchMenuText(CCmdUI *pCmdUI)
	{
		//
		// This must be overridden to be useful.
		//
		ASSERT(0);
	}

	virtual BOOL DoFind()
	{
		//
		// This must be overridden to be useful.
		//
		ASSERT(0);
		return(FALSE);
	}

	virtual BOOL DoFindNext()
	{
		//
		// This must be overridden to be useful.
		//
		ASSERT(0);
		return(FALSE);
	}

	virtual BOOL DoFindPrev()
	{
		//
		// This must be overridden to be useful.
		//
		ASSERT(0);
		return(FALSE);
	}

	virtual BOOL HasValidFind()
	{
		//
		// This must be overridden to be useful.
		//
		ASSERT(0);
		return(FALSE);
	}

	virtual BOOL ExpandFirstLine()
	{
		return FALSE;
	}

	virtual BOOL SortLines()
	{
		return TRUE;
	}

	virtual ~CSpyTreeDoc();
	virtual BOOL OnNewDocument();
	virtual void DeleteContents();

	CObList m_TopLevelNodeList;

	friend class CSpyTreeView;

	// Generated message map functions
protected:
	//{{AFX_MSG(CSpyTreeDoc)
	afx_msg void OnWindowRefresh();
	afx_msg void OnTreeExpandOneLevel();
	afx_msg void OnUpdateTreeExpandOneLevel(CCmdUI* pCmdUI);
	afx_msg void OnTreeExpandBranch();
	afx_msg void OnUpdateTreeExpandBranch(CCmdUI* pCmdUI);
	afx_msg void OnTreeExpandAll();
	afx_msg void OnUpdateTreeExpandAll(CCmdUI* pCmdUI);
	afx_msg void OnTreeCollapse();
	afx_msg void OnUpdateTreeCollapse(CCmdUI* pCmdUI);
	afx_msg void OnSearchFind();
	afx_msg void OnUpdateSearchFind(CCmdUI* pCmdUI);
	afx_msg void OnSearchFindNext();
	afx_msg void OnUpdateSearchFindNext(CCmdUI* pCmdUI);
	afx_msg void OnSearchFindPrev();
	afx_msg void OnUpdateSearchFindPrev(CCmdUI* pCmdUI);
	afx_msg void OnPrintTree();
	afx_msg void OnUpdatePrintTree(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
