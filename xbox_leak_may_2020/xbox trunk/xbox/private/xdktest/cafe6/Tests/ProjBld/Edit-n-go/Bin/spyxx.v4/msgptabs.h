// msgptabs.h : header file
//


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMessageGeneralPropTab window

class CMessageGeneralPropTab : public CPropertyTab
{
// Construction
public:
	CMessageGeneralPropTab(UINT nIDTemplate, UINT nIDCaption)
		: CPropertyTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CMessageGeneralPropTab)
	enum { IDD = IDD_MESSAGEGENERALTAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CMessageGeneralPropTab();
	virtual void UpdateFields();

protected:
	virtual PINT GetControlIDTable()
	{
		return m_aControlIDs;
	}

	CHotLinkCtrlWnd m_HotLinkWindowHandle;

	static INT m_aControlIDs[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CMessageGeneralPropTab)
	virtual BOOL OnInitDialog();
	afx_msg void OnMessageHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
