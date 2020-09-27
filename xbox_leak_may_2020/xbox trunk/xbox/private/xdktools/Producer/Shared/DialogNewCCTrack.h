#if !defined(AFX_DIALOGNEWCCTRACK_H__66590545_D86D_11D1_89B1_00C04FD912C8__INCLUDED_)
#define AFX_DIALOGNEWCCTRACK_H__66590545_D86D_11D1_89B1_00C04FD912C8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogNewCCTrack.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogNewCCTrack dialog

typedef BOOL (*FuncPtr)(void *, BYTE, WORD);

class CDialogNewCCTrack : public CDialog
{
// Construction
public:
	CDialogNewCCTrack(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogNewCCTrack)
	enum { IDD = IDD_NEW_CC_TRACK };
	CComboBox	m_comboCC;
	CStatic	m_staticRPNDesc;
	CStatic	m_staticRPNFriendlyName;
	CStatic m_staticRPNExists;
	CSpinButtonCtrl	m_spinRPNType;
	CEdit	m_editRPNType;
	CButton		m_btnOK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogNewCCTrack)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	FuncPtr			 m_pfCurveStripExists;
	BYTE			 m_bCCType;
	WORD			 m_wRPNType; // RPN or NRPN type
	void			*m_pVoid;

protected:
	void FillComboCC();

	// Generated message map functions
	//{{AFX_MSG(CDialogNewCCTrack)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeEditRPNType();
	afx_msg void OnSelChangeComboCC();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGNEWCCTRACK_H__66590545_D86D_11D1_89B1_00C04FD912C8__INCLUDED_)
