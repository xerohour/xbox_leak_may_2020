#if !defined(AFX_DLGMETRONOME_H__237BB477_063B_11D2_88D4_00C04FBF8D15__INCLUDED_)
#define AFX_DLGMETRONOME_H__237BB477_063B_11D2_88D4_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgMetronome.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DlgMetronome dialog

class DlgMetronome : public CDialog
{
// Construction
public:
	DlgMetronome(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DlgMetronome)
	enum { IDD = IDD_DIALOG_METRONOME };
	CSpinButtonCtrl	m_spinOtherVelocity;
	CEdit	m_editOtherVelocity;
	CComboBox	m_comboOtherValue;
	CSpinButtonCtrl	m_spinCountIn;
	CEdit	m_editCountIn;
	CSpinButtonCtrl	m_spinPChannel;
	CEdit	m_editPChannel;
	CEdit	m_editOneVelocity;
	CComboBox	m_comboOneValue;
	CSpinButtonCtrl	m_spinOneVelocity;
	//}}AFX_DATA

	BYTE	m_bOneVelocity;
	BYTE	m_bOneValue;
	BYTE	m_bOtherVelocity;
	BYTE	m_bOtherValue;
	DWORD	m_dwPChannel;

	long	m_lCountInBars;
	BOOL	m_fCountOnlyOnRecord;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgMetronome)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgMetronome)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillfocusEditOneVelocity();
	afx_msg void OnKillfocusEditPchannel();
	afx_msg void OnKillfocusEditOtherVelocity();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMETRONOME_H__237BB477_063B_11D2_88D4_00C04FBF8D15__INCLUDED_)
