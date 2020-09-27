// thdptabs.h : header file
//



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CThreadGeneralPropTab window

class CThreadGeneralPropTab : public CPropertyTab
{
// Construction
public:
	CThreadGeneralPropTab(UINT nIDTemplate, UINT nIDCaption)
		: CPropertyTab(nIDTemplate, nIDCaption) {}

// Dialog Data
	//{{AFX_DATA(CThreadGeneralPropTab)
	enum { IDD = IDD_THREADGENERALTAB };
	//}}AFX_DATA

// Implementation
public:
	virtual ~CThreadGeneralPropTab();
	virtual void UpdateFields();

protected:
	virtual PINT GetControlIDTable()
	{
		return m_aControlIDs;
	}

protected:
	CHotLinkCtrlPrc m_HotLinkProcessID;

protected:
	static INT m_aControlIDs[];
	static VALUETABLE m_astThreadState[];
	static VALUETABLE m_astWaitReason[];

	// Generated message map functions
protected:
	//{{AFX_MSG(CThreadGeneralPropTab)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CThreadTabbedDialog;
};

/////////////////////////////////////////////////////////////////////////////



