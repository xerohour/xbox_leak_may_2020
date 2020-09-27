#if !defined(AFX_DLGCHANGELENGTH_H__9F9541FC_4402_11D1_981C_00805FA67D16__INCLUDED_)
#define AFX_DLGCHANGELENGTH_H__9F9541FC_4402_11D1_981C_00805FA67D16__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgChangeLength.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgChangeLength dialog

class CDlgChangeLength : public CDialog
{
// Construction
public:
	CDlgChangeLength(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgChangeLength)
	enum { IDD = IDD_ChangeLength };
	CSpinButtonCtrl	m_spinLength;
	CString	m_strLength;
	//}}AFX_DATA

	// use this only to set initial value
	void SetChordMapLength(short n)
	{
		m_nChordMapLength = n;
		m_strLength.Format("%d", n);
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChangeLength)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
// Implementation
protected:
	short m_nChordMapLength;
	BOOL  bValidate;
	// Generated message map functions
	//{{AFX_MSG(CDlgChangeLength)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditlength();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHANGELENGTH_H__9F9541FC_4402_11D1_981C_00805FA67D16__INCLUDED_)
