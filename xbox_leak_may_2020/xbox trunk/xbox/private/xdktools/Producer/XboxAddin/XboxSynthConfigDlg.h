#if !defined(AFX_XBOXSYNTHCONFIGDLG_H__B389E33A_2E8F_42F7_99D3_74BC2D153FFF__INCLUDED_)
#define AFX_XBOXSYNTHCONFIGDLG_H__B389E33A_2E8F_42F7_99D3_74BC2D153FFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XboxSynthConfigDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXboxSynthConfigDlg dialog

class CXboxSynthConfigDlg : public CDialog
{
// Construction
public:
	CXboxSynthConfigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CXboxSynthConfigDlg)
	enum { IDD = IDD_XBOX_SYNTH_CONFIG };
	BOOL	m_bPCSynth;
	BOOL	m_bXboxSynth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXboxSynthConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CXboxSynthConfigDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XBOXSYNTHCONFIGDLG_H__B389E33A_2E8F_42F7_99D3_74BC2D153FFF__INCLUDED_)
