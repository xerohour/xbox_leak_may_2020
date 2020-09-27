#if !defined(AFX_DLGEXTRALENGTH_H__EA279F1D_B15C_4603_A3A2_1423E365C61A__INCLUDED_)
#define AFX_DLGEXTRALENGTH_H__EA279F1D_B15C_4603_A3A2_1423E365C61A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExtraLength.h : header file
//

#include "resource.h"

#define MIN_EXTRA_BARS		0
#define MAX_EXTRA_BARS		32767

/////////////////////////////////////////////////////////////////////////////
// CDlgExtraLength dialog

class CDlgExtraLength : public CDialog
{
// Construction
public:
	CDlgExtraLength(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgExtraLength)
	enum { IDD = IDD_EXT_LENGTH };
	CButton	m_checkPickup;
	CSpinButtonCtrl	m_spinLength;
	CEdit	m_editLength;
	//}}AFX_DATA
	DWORD	m_dwNbrExtraBars;
	BOOL	m_fPickupBar;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExtraLength)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgExtraLength)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusExtLength();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXTRALENGTH_H__EA279F1D_B15C_4603_A3A2_1423E365C61A__INCLUDED_)
