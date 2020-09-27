#if !defined(AFX_PROGRESSDIALOG_H__78F41A92_4CED_11D2_9A8F_006097B01078__INCLUDED_)
#define AFX_PROGRESSDIALOG_H__78F41A92_4CED_11D2_9A8F_006097B01078__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ProgressDialog.h : header file
//
#include "Resource.h"


class CWave;
class CProgressUpdateThread : public CWinThread
{

public:
	CProgressUpdateThread();
	~CProgressUpdateThread();

public:
	CProgressCtrl* m_pProgressBar;

};


/////////////////////////////////////////////////////////////////////////////
// CProgressDialog dialog

class CProgressDialog : public CDialog
{
// Construction
public:
	CProgressDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgressDialog)
	enum { IDD = IDD_PROGRESS_DIALOG };
	CProgressCtrl	m_ProgressBar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDialog)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void SetRange(int nRange);


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int		m_nRange;
	UINT	m_uTimerID;



};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSDIALOG_H__78F41A92_4CED_11D2_9A8F_006097B01078__INCLUDED_)
