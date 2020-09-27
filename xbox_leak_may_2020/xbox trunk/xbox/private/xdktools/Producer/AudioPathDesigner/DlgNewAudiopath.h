#if !defined(AFX_DLGNEWAUDIOPATH_H__7775BF7C_BE75_4F4F_8F75_9634EB1BA20D__INCLUDED_)
#define AFX_DLGNEWAUDIOPATH_H__7775BF7C_BE75_4F4F_8F75_9634EB1BA20D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgNewAudiopath.h : header file
//

#include "resource.h"

class CAudioPathComponent;
class CDirectMusicAudioPath;

/////////////////////////////////////////////////////////////////////////////
// CDlgNewAudiopath dialog

class CDlgNewAudiopath : public CDialog
{
// Construction
public:
	CDlgNewAudiopath(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNewAudiopath)
	enum { IDD = IDD_NEW_AUDIOPATH };
	CListBox	m_listAudiopath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewAudiopath)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CAudioPathComponent		*m_pAudioPathComponent;
	bool					m_fBaseOnExistingAudiopath;
	CDirectMusicAudioPath	*m_pExistingAudiopath;
	DWORD					m_dwDefaultAudiopathID;

protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgNewAudiopath)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnNewAudiopath();
	afx_msg void OnExistingAudiopath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWAUDIOPATH_H__7775BF7C_BE75_4F4F_8F75_9634EB1BA20D__INCLUDED_)
