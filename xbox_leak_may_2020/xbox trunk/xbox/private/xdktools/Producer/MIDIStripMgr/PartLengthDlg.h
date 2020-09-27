#if !defined(AFX_PARTLENGTHDLG_H__B2B7FA22_73ED_11D1_89AE_00A0C9054129__INCLUDED_)
#define AFX_PARTLENGTHDLG_H__B2B7FA22_73ED_11D1_89AE_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PartLengthDlg.h : header file
//


#define MIN_PART_LENGTH		1
#define MAX_PART_LENGTH		999

#define MIN_EXTRA_BARS		0
#define MAX_EXTRA_BARS		32767


/////////////////////////////////////////////////////////////////////////////
// CPartLengthDlg dialog

class CPartLengthDlg : public CDialog
{
// Construction
public:
	CPartLengthDlg( CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPartLengthDlg)
	enum { IDD = IDD_PART_LENGTH };
	CButton	m_checkPickup;
	CEdit	m_editExtLength;
	CSpinButtonCtrl	m_spinExtLength;
	CEdit	m_editLength;
	CButton	m_btnOK;
	CSpinButtonCtrl	m_spinLength;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartLengthDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	WORD	m_wNbrMeasures;
	DWORD	m_dwNbrExtraBars;
	BOOL	m_fPickupBar;

protected:
	// Generated message map functions
	//{{AFX_MSG(CPartLengthDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillfocusLength();
	afx_msg void OnKillfocusExtLength();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTLENGTHDLG_H__B2B7FA22_73ED_11D1_89AE_00A0C9054129__INCLUDED_)
