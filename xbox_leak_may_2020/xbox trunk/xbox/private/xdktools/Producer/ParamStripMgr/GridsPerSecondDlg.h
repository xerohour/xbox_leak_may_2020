#if !defined(AFX_GRIDSPERSECONDDLG_H__9EA8FD59_CA8B_44D5_A5A7_49D483F35093__INCLUDED_)
#define AFX_GRIDSPERSECONDDLG_H__9EA8FD59_CA8B_44D5_A5A7_49D483F35093__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// GridsPerSecondDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CGridsPerSecondDlg dialog

class CGridsPerSecondDlg : public CDialog
{
// Construction
public:
	CGridsPerSecondDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGridsPerSecondDlg)
	enum { IDD = IDD_GRIDS_PER_SECOND };
	CStatic	m_staticNbrMS;
	CButton	m_btnOK;
	CSpinButtonCtrl	m_spinNbrGrids;
	CEdit	m_editNbrGrids;
	CButton	m_radioActivePart;
	CButton	m_radioAllParts;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridsPerSecondDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetNbrMsPerGridText() ;

	// Generated message map functions
	//{{AFX_MSG(CGridsPerSecondDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillFocusEditNbrGrids();
	afx_msg void OnDeltaPosSpinNbrGrids(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	short   m_nSnapToRefTime_GridsPerSecond;
	BOOL	m_fAllParts;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDSPERSECONDDLG_H__9EA8FD59_CA8B_44D5_A5A7_49D483F35093__INCLUDED_)
