#if !defined(AFX_SECONDARYDLG_H__3152BA5B_668B_4803_8C70_3B3BC055A112__INCLUDED_)
#define AFX_SECONDARYDLG_H__3152BA5B_668B_4803_8C70_3B3BC055A112__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SegmentDlg.h"

// SecondaryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSecondaryDlg dialog

class CSecondaryDlg : public CSegmentDlg
{
// Construction
public:
	CSecondaryDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSecondaryDlg)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecondaryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual HRESULT AddSegmentToList( CSegment *pSegment );

	// Generated message map functions
	//{{AFX_MSG(CSecondaryDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECONDARYDLG_H__3152BA5B_668B_4803_8C70_3B3BC055A112__INCLUDED_)
