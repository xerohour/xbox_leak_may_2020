// spytreec.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CSpyTreeCtl window

class CSpyTreeCtl : public CTreeCtl
{
// Construction
public:
	CSpyTreeCtl();

// Attributes
public:

// Operations
public:
	virtual BOOL InsertLevel(CNode* pNode, int nIndex, BOOL bExpandAll = FALSE);
	virtual void DrawNodeGlyph(CDC* pDC, CNode* pNode, BOOL bHighlight, CPoint pt);
	virtual BOOL NodeIsExpandable(CNode* pNode);
	virtual void ShowSelItemProperties();

	virtual void OnSelect(int nIndex);	  // called on selection change
	virtual void OnRightButtonDown(UINT nFlags, CPoint pt);

	void SetSorting(BOOL fSorted)
	{
		m_bSorted = fSorted;
	}

// Implementation
public:
	virtual ~CSpyTreeCtl();

	CBitmap m_bmFolders;
	CBitmap m_bmExpandNodes;

	// Generated message map functions
protected:
	static CImageWell c_imageSpyWell;

	//{{AFX_MSG(CSpyTreeCtl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLbDblClk();
	afx_msg void OnLbSetFocus();
	afx_msg void OnLbKillFocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// CSpyTreeNode object

class CSpyTreeNode : public CObject
{
	DECLARE_DYNCREATE(CSpyTreeNode);

public:
	CSpyTreeNode() {};
	virtual ~CSpyTreeNode();

// Attributes
public:

// Operations
public:
	virtual void GetDescription(CString& str)
	{
		// must be overridden to be useful...
		// would be pure virtual but that breaks DECLARE_DYNCREATE() above.
		ASSERT(FALSE);
	}
	virtual void ShowProperties()
	{
		// must be overridden to be useful...
		// would be pure virtual but that breaks DECLARE_DYNCREATE() above.
		ASSERT(FALSE);
	}
	virtual int GetBitmapIndex()
	{
		// must be overridden to be useful...
		// would be pure virtual but that breaks DECLARE_DYNCREATE() above.
		ASSERT(FALSE);
		return(0);
	}
	virtual DWORD GetObjectHandle()
	{
		// must be overridden to be useful...
		// would be pure virtual but that breaks DECLARE_DYNCREATE() above.
		ASSERT(FALSE);
		return(0);
	}
	virtual int GetObjectType()
	{
		// must be overridden to be useful...
		// would be pure virtual but that breaks DECLARE_DYNCREATE() above.
		ASSERT(FALSE);
		return(0);
	}

	virtual POSITION AddChild(CSpyTreeNode* pTreeNode)
	{
		return m_ChildList.AddTail(pTreeNode);
	}
	virtual POSITION GetFirstChildPosition()
	{
		return m_ChildList.GetHeadPosition();
	}
	virtual CSpyTreeNode*& GetNextChild(POSITION& pos)
	{
		return (CSpyTreeNode*&)m_ChildList.GetNext(pos);
	}

protected:
//	static PSYSTEM_PROCESS_INFORMATION m_pProcessBuffer;
	static int m_cUsage;

// Implementation
protected:
	CObList m_ChildList;
};

/////////////////////////////////////////////////////////////////////////////
