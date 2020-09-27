#if !defined(AFX_DIALOGEDITSEED_H__4C00C9F8_1546_11D3_A71A_00105A26620B__INCLUDED_)
#define AFX_DIALOGEDITSEED_H__4C00C9F8_1546_11D3_A71A_00105A26620B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogEditSeed.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogEditSeed dialog

class CDialogEditSeed : public CDialog
{
// Construction
public:
	CDialogEditSeed(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogEditSeed)
	enum { IDD = IDD_EDIT_VARIATION_SEED };
	CEdit	m_editSeed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogEditSeed)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	DWORD		m_dwVariationSeed;

protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogEditSeed)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGEDITSEED_H__4C00C9F8_1546_11D3_A71A_00105A26620B__INCLUDED_)
