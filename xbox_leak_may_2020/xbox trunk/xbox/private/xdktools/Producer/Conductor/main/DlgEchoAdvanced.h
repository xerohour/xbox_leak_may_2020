#if !defined(AFX_DLGECHOADVANCED_H__07803D41_034C_47FB_A772_5162A1D2E0AA__INCLUDED_)
#define AFX_DLGECHOADVANCED_H__07803D41_034C_47FB_A772_5162A1D2E0AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEchoAdvanced.h : header file
//

#include "resource.h"
#include "OutputTool.h"

/////////////////////////////////////////////////////////////////////////////
// DlgEchoAdvanced dialog

class DlgMIDI;

class DlgEchoAdvanced : public CDialog
{
// Construction
public:
	DlgEchoAdvanced(CWnd* pParent = NULL);   // standard constructor

	DlgMIDI *m_pDlgMIDI;
// Dialog Data
	//{{AFX_DATA(DlgEchoAdvanced)
	enum { IDD = IDD_ECHO_ADVANCED };
	CComboBox	m_acomboEchoInput[ECHO_ADVANCED_PORTS];
	CSpinButtonCtrl	m_aspinEchoPChannels[ECHO_ADVANCED_PORTS];
	CEdit	m_aeditEchoPChannels[ECHO_ADVANCED_PORTS];
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DlgEchoAdvanced)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	int	m_anCurrentBlock[ECHO_ADVANCED_PORTS];
	void UpdatePChannelEdit( DWORD dwIndex, int nBlock );
	void DisplayPortCreationError( HRESULT hr, CComboBox &comboBox );

	// Generated message map functions
	//{{AFX_MSG(DlgEchoAdvanced)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDeltaposSpinEchoPchannels(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusEditEchoPchannels();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGECHOADVANCED_H__07803D41_034C_47FB_A772_5162A1D2E0AA__INCLUDED_)
