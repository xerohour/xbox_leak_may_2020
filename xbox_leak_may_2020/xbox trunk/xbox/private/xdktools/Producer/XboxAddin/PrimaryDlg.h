#if !defined(AFX_PRIMARYDLG_H__DEBBFE12_5D20_423D_9587_0A65D02038D3__INCLUDED_)
#define AFX_PRIMARYDLG_H__DEBBFE12_5D20_423D_9587_0A65D02038D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SegmentDlg.h"

// PrimaryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrimaryDlg dialog

class CPrimaryDlg : public CSegmentDlg
{
// Construction
public:
	CPrimaryDlg(CWnd* pParent = NULL);   // standard constructor

	void HandleSegmentNotification( CSegment *pSegment, DWORD dwNotification );

// Dialog Data
	//{{AFX_DATA(CPrimaryDlg)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrimaryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrimaryDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRIMARYDLG_H__DEBBFE12_5D20_423D_9587_0A65D02038D3__INCLUDED_)
