#if !defined(AFX_REMAPDLG_H__03599C07_22F2_11D2_850F_00A0C99F7E74__INCLUDED_)
#define AFX_REMAPDLG_H__03599C07_22F2_11D2_850F_00A0C99F7E74__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RemapDlg.h : header file
//

#include "resource.h"
#include <PChannelName.h>

/////////////////////////////////////////////////////////////////////////////
// CRemapDlg dialog

class CRemapDlg : public CDialog
{
// Construction
public:
	CRemapDlg(CWnd* pParent = NULL);   // standard constructor
	~CRemapDlg();		

	// these are to init the dialog
	IDMUSProdPChannelName*	m_pIPChannelName;
	DWORD	m_dwPChannel;
	long	m_lMeasure;
	long	m_lBeat;

	// the channel that the user chooses.
	DWORD	m_dwPChannelMap;

// Dialog Data
	//{{AFX_DATA(CRemapDlg)
	enum { IDD = IDD_REMAP };
	CEdit	m_ChannelNameEdit;
	CEdit	m_ChannelEdit;
	CSpinButtonCtrl	m_ChannelSpin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemapDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdatePChannelName();

	// Generated message map functions
	//{{AFX_MSG(CRemapDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDeltaposPchannelSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangePchannelEdit();
	afx_msg void OnKillfocusPchannelEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMAPDLG_H__03599C07_22F2_11D2_850F_00A0C99F7E74__INCLUDED_)
