//-----------------------------------------------------------------------------
// File: autorun.cpp
//
// Desc: header file
//
// Hist: 10.27.00 Emily Wang Created For XBOX
//       
// Copyright (c) 1998-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#if !defined(AFX_AUTORUNDLG_H__BBCC11C6_4698_4E91_A0D7_7AB6D85084F3__INCLUDED_)
#define AFX_AUTORUNDLG_H__BBCC11C6_4698_4E91_A0D7_7AB6D85084F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAutorunDlg dialog
#include "bitmap.h"

class CAutorunDlg : public CDialog
{
// Construction
public:
	CAutorunDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAutorunDlg)
	enum { IDD = IDD_AUTORUN_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutorunDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON	m_hIcon;
	HCURSOR m_hCursor;
    HCURSOR m_hOldCursor;

	// Generated message map functions
	//{{AFX_MSG(CAutorunDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	SIZE CalculateTextSize(CDC* pDC, LPCTSTR pszString);
    void DrawLink(CDC* pDC, LPCTSTR pszText, COLORREF crText, RECT rcRect);

    void OnOK(void);

private:
    // The font of links
    CFont            m_font;

    // The index of link which is hited by mouse. If m_nIndexHitLink = -1, no link has been hitted
    INT              m_nIndexHitLink;

    // The image of the dialog background
    CBitmapFileImage m_cImage;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#define IDC_STATIC_AUTORUN_LINK  0x0101

#endif // !defined(AFX_AUTORUNDLG_H__BBCC11C6_4698_4E91_A0D7_7AB6D85084F3__INCLUDED_)
