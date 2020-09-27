// thdtreed.h : header file
//

#define SRCHFLAG_THREAD_USETHREAD	0x01
#define SRCHFLAG_THREAD_USEMODULE	0x02

/////////////////////////////////////////////////////////////////////////////
// CThdTreeDoc document

class CThdTreeDoc : public CSpyTreeDoc
{
	DECLARE_DYNCREATE(CThdTreeDoc);

protected:
	CThdTreeDoc();		  // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
	// data variables use in searching
	WORD	m_wSearchFlags;
	DWORD	m_dwThread;
	CString	m_strModule;
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
	virtual ~CThdTreeDoc();

	// Generated message map functions
protected:
	//{{AFX_MSG(CThdTreeDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
