#if !defined(AFX_DIALOGVELOCITY_H__8F946A98_5182_4206_BE6E_57678957D44F__INCLUDED_)
#define AFX_DIALOGVELOCITY_H__8F946A98_5182_4206_BE6E_57678957D44F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogVelocity.h : header file
//

#include "resource.h"

class CVelocityEdit :
	public CEdit
{
	DECLARE_DYNCREATE(CVelocityEdit)
public:
	CVelocityEdit();

	// for processing Windows messages
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

typedef enum
{
	VELOCITY_TARGET_SELECTED,
	VELOCITY_TARGET_PART,
	VELOCITY_TARGET_PATTERN
} VELOCITY_TARGET;

/////////////////////////////////////////////////////////////////////////////
// CDialogVelocity dialog

class CDialogVelocity : public CDialog
{
// Construction
public:
	CDialogVelocity(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogVelocity)
	enum { IDD = IDD_VELOCITY };
	CSpinButtonCtrl	m_spinStrengthEnd;
	CVelocityEdit	m_editStrengthEnd;
	CSpinButtonCtrl	m_spinStrength;
	CSpinButtonCtrl	m_spinMin;
	CSpinButtonCtrl	m_spinMax;
	CVelocityEdit	m_editStrength;
	CEdit	m_editMin;
	CEdit	m_editMax;
	//}}AFX_DATA

	bool				m_fEnableSelected;
	bool				m_fEndTracksStart;

	VELOCITY_TARGET		m_vtTarget;
	bool				m_fAbsolute;
	bool				m_fPercent;
	long				m_lAbsoluteChangeStart;
	long				m_lAbsoluteChangeEnd;
	BYTE				m_bCompressMin;
	BYTE				m_bCompressMax;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogVelocity)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableDlgItem( int nItemID, BOOL fEnable );

	// Generated message map functions
	//{{AFX_MSG(CDialogVelocity)
	afx_msg void OnRadioAbsolute();
	afx_msg void OnRadioCompress();
	afx_msg void OnRadioLinear();
	afx_msg void OnRadioPercent();
	afx_msg void OnKillfocusEditMax();
	afx_msg void OnKillfocusEditMin();
	afx_msg void OnKillfocusEditStrength();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillfocusEditStrengthEnd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGVELOCITY_H__8F946A98_5182_4206_BE6E_57678957D44F__INCLUDED_)
