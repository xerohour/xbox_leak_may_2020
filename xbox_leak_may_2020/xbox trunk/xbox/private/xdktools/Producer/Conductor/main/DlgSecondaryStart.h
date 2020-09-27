#if !defined(AFX_DLGSECONDARYSTART_H__6CB4201E_259F_11D2_88F7_00C04FBF8D15__INCLUDED_)
#define AFX_DLGSECONDARYSTART_H__6CB4201E_259F_11D2_88F7_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgSecondaryStart.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// DlgSecondaryStart dialog

class DlgSecondaryStart : public CDialog
{
// Construction
public:
	DlgSecondaryStart(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DlgSecondaryStart)
	enum { IDD = IDD_SECONDARY };
	CButton	m_checkBoundary;
	CButton	m_radioSwitchAnyTime;
	CButton	m_radioSwitchAnyGrid;
	CButton	m_radioSwitchAnyBeat;
	CButton	m_radioSwitchAnyBar;
	CButton	m_radioSwitchOnBoundary;
	CStatic	m_staticNoMarkers;
	CStatic	m_staticAlignPrompt;
	CComboBox	m_comboBoundary;
	//}}AFX_DATA

	DWORD	m_dwSegmentFlags;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgSecondaryStart)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DlgSecondaryStart)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckBoundary();
	afx_msg void OnSelchangeComboBoundary();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSECONDARYSTART_H__6CB4201E_259F_11D2_88F7_00C04FBF8D15__INCLUDED_)
