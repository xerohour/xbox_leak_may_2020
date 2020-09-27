#if !defined(AFX_DLGALTTEMPO_H__A89CD69A_3092_11D2_8900_00C04FBF8D15__INCLUDED_)
#define AFX_DLGALTTEMPO_H__A89CD69A_3092_11D2_8900_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "resource.h"

// DlgAltTempo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DlgAltTempo dialog

class DlgAltTempo : public CDialog
{
// Construction
public:
	DlgAltTempo(CWnd* pParent = NULL);   // standard constructor

	int m_nRatio;

// Dialog Data
	//{{AFX_DATA(DlgAltTempo)
	enum { IDD = IDD_ALTERNATE_TEMPO };
	CSpinButtonCtrl	m_spinRatio;
	CEdit	m_editRatio;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgAltTempo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgAltTempo)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusRatioEdit();
	afx_msg void OnDeltaposSpinRatio(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGALTTEMPO_H__A89CD69A_3092_11D2_8900_00C04FBF8D15__INCLUDED_)
