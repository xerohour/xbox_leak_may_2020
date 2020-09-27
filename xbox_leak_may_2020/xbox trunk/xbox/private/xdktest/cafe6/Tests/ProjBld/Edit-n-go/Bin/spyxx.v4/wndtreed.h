// wndtreed.h : header file
//

#define SRCHFLAG_WINDOW_USEHANDLE	0x01
#define SRCHFLAG_WINDOW_USECAPTION	0x02
#define SRCHFLAG_WINDOW_USECLASS	0x04

/////////////////////////////////////////////////////////////////////////////
// CWndTreeDoc document

class CWndTreeDoc : public CSpyTreeDoc
{
	DECLARE_DYNCREATE(CWndTreeDoc);

protected:
	CWndTreeDoc();		  // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
	// data variables use in searching
	WORD 	m_wSearchFlags;
	HWND	m_hwndWnd;
	CString	m_strCaption;
	CString m_strClass;
	BOOL	m_fSearchUp;	// TRUE if searching up, FALSE if searching down (from dialog)

	virtual void InitializeData();
	virtual void SetSearchMenuText(CCmdUI *pCmdUI);
	virtual BOOL DoFind();
	virtual BOOL DoFindNext();
	virtual BOOL DoFindPrev();
	virtual BOOL HasValidFind();
	virtual BOOL FindFirstMatchingNode();
	virtual BOOL FindNextMatchingNode();
	virtual BOOL FindPreviousMatchingNode();
	virtual BOOL FindLastMatchingNode();
	virtual BOOL FindNodeRecurse(CWndTreeNode *pcwtn, CObList &listExpand, BOOL fSearchingUp = FALSE);
	virtual BOOL CompareNode(CWndTreeNode *pcwtn);
	virtual BOOL ExpandFirstLine()
	{
		//
		// Yes, we want to expand the first line in the new view.
		// It is not very useful to just see one line (the desktop
		// window) in the initial view.
		//
		return TRUE;
	}

	virtual BOOL SortLines()
	{
		//
		// Do NOT sort lines as they are added.  Windows are
		// enumerated in a specific order that must be preserved
		// for the tree to be useful.
		//
		return FALSE;
	}

	virtual ~CWndTreeDoc();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndTreeDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
