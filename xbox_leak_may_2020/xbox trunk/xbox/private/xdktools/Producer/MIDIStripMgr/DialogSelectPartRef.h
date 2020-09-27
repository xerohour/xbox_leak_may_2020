#if !defined(AFX_DIALOGSELECTPARTREF_H__B135C0F7_2637_11D2_88F7_00C04FBF8D15__INCLUDED_)
#define AFX_DIALOGSELECTPARTREF_H__B135C0F7_2637_11D2_88F7_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogSelectPartRef.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogSelectPartRef dialog

class CDialogSelectPartRef : public CDialog
{
// Construction
public:
	CDialogSelectPartRef(CWnd* pParent = NULL);   // standard constructor

	class CDirectMusicPartRef *m_pDMPartRef;
	const class CDirectMusicPattern* m_pDMPattern;

	DWORD	m_dwPChannel;

// Dialog Data
	//{{AFX_DATA(CDialogSelectPartRef)
	enum { IDD = IDD_SELECT_PARTREF };
	CListBox	m_listPartRef;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogSelectPartRef)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogSelectPartRef)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGSELECTPARTREF_H__B135C0F7_2637_11D2_88F7_00C04FBF8D15__INCLUDED_)
