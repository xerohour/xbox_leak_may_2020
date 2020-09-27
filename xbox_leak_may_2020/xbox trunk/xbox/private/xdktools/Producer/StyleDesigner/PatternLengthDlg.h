#if !defined(AFX_PATTERNLENGTHDLG_H__B2B7FA22_73ED_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_PATTERNLENGTHDLG_H__B2B7FA22_73ED_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PatternLengthDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPatternLengthDlg dialog

class CPatternLengthDlg : public CDialog
{
// Construction
public:
	CPatternLengthDlg( CDirectMusicPattern* pPattern, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPatternLengthDlg)
	enum { IDD = IDD_PATTERN_LENGTH };
	CEdit	m_editLength;
	CButton	m_btnOK;
	CSpinButtonCtrl	m_spinLength;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatternLengthDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDirectMusicPattern* m_pPattern;

	// Generated message map functions
	//{{AFX_MSG(CPatternLengthDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATTERNLENGTHDLG_H__B2B7FA22_73ED_11D1_89AE_00A0C9054129__INCLUDED_)
