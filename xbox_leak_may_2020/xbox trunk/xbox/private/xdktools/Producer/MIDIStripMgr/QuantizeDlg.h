#if !defined(AFX_QUANTIZEDLG_H__30880203_06B6_11D2_89B3_00C04FD912C8__INCLUDED_)
#define AFX_QUANTIZEDLG_H__30880203_06B6_11D2_89B3_00C04FD912C8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// QuantizeDlg.h : header file
//

#include "DMPPrivate.h"

#define MIN_GRIDS_PER_BEAT		1
#define MAX_GRIDS_PER_BEAT		24
#define QUANTIZE_TARGET_PATTERN QUANTIZE_TARGET_SEQUENCE

/////////////////////////////////////////////////////////////////////////////
// CQuantizeDlg dialog

class CQuantizeDlg : public CDialog
{
// Construction
public:
	CQuantizeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQuantizeDlg)
	enum { IDD = IDD_QUANTIZE };
	CSpinButtonCtrl	m_spinStrength;
	CComboBox	m_comboGrid;
	CEdit	m_editStrength;
	CButton	m_buttonStartTime;
	CButton	m_buttonDuration;
	//}}AFX_DATA

	BOOL				m_fEnableSelected;
	QUANTIZE_TARGET		m_qtTarget;
	long				m_lResolution;
	DWORD				m_dwFlags;
	long				m_lStrength;
	DirectMusicTimeSig	m_TimeSignature;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuantizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQuantizeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void OnOK();
	afx_msg void OnKillfocusEditStrength();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUANTIZEDLG_H__30880203_06B6_11D2_89B3_00C04FD912C8__INCLUDED_)
