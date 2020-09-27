// proptab.h : header file
//


class CPropertyTab : public CDlgTab
{
// Construction
public:
	CPropertyTab(UINT nIDTemplate, UINT nIDCaption)
		: CDlgTab(nIDTemplate, nIDCaption)
	{
		SetRefreshFlag(TRUE);
	}

// Implementation
public:
	virtual ~CPropertyTab();
	virtual BOOL Activate(CTabbedDialog* pParentWnd, CPoint position);
	virtual void UpdateFields() {};

	void SetRefreshFlag(BOOL fRefreshData)
	{
		m_fRefreshData = fRefreshData;
	}
	BOOL GetRefreshFlag()
	{
		return m_fRefreshData;
	}

protected:
	virtual PINT GetControlIDTable()
	{
		return NULL;
	}

protected:
	BOOL m_fRefreshData;

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyTab)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnHotLink(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
