#if !defined(AFX_SEGMENTABOUTBOX_H__1B9DF6D3_020A_11D1_BAF1_00805F493F43__INCLUDED_)
#define AFX_SEGMENTABOUTBOX_H__1B9DF6D3_020A_11D1_BAF1_00805F493F43__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SegmentAboutBox.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CSegmentAboutBox dialog

class CSegmentAboutBox : public CDialog
{
// Construction
public:
	CSegmentAboutBox(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSegmentAboutBox)
	enum { IDD = IDD_ABOUTBOX_SEGMENT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSegmentAboutBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSegmentAboutBox)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEGMENTABOUTBOX_H__1B9DF6D3_020A_11D1_BAF1_00805F493F43__INCLUDED_)
