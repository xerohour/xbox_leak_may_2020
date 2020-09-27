#if !defined(AFX_DLGSETLENGTH_H__B2B7FA22_73ED_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_DLGSETLENGTH_H__B2B7FA22_73ED_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgSetLength.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetLength dialog

class CDlgSetLength : public CDialog
{
// Construction
public:
	CDlgSetLength(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetLength)
	enum { IDD = IDD_SET_LENGTH };
	CEdit	m_editLength;
	CButton	m_btnOK;
	CSpinButtonCtrl	m_spinLength;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetLength)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgSetLength)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	DWORD	m_dwNbrMeasures;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETLENGTH_H__B2B7FA22_73ED_11D1_89AE_00A0C9054129__INCLUDED_)
