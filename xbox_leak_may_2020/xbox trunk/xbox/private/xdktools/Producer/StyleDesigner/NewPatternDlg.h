#if !defined(AFX_NEWPATTERNDLG_H__B98F9683_BF6C_11D2_8545_00105A2796DE__INCLUDED_)
#define AFX_NEWPATTERNDLG_H__B98F9683_BF6C_11D2_8545_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewPatternDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewPatternDlg dialog

class CNewPatternDlg : public CDialog
{
friend class CStyleMotifs;
friend class CStylePatterns;

// Construction
public:
	CNewPatternDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewPatternDlg)
	enum { IDD = IDD_NEW_PATTERN };
	CListBox	m_lstbxPattern;
	CButton	m_radioNewPattern;
	CButton	m_radioLinkPattern;
	//}}AFX_DATA

protected:
	WORD					m_wEmbellishment;	// Used to distinguish Pattern from Motif
	CDirectMusicPattern*	m_pPattern;			// Pointer to linked Pattern
	CDirectMusicStyle*		m_pStyle;			// Pointer to parent Style of the new Pattern/Motif 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewPatternDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewPatternDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNewPattern();
	afx_msg void OnDoubleClickedNewPattern();
	afx_msg void OnLinkPattern();
	afx_msg void OnDoubleClickedLinkPattern();
	afx_msg void OnSelChangePatternList();
	virtual void OnOK();
	afx_msg int OnCompareItem(int nIDCtl, LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWPATTERNDLG_H__B98F9683_BF6C_11D2_8545_00105A2796DE__INCLUDED_)
