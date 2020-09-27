#if !defined(AFX_DIALOGLINKEXISTING_H__B135C0F6_2637_11D2_88F7_00C04FBF8D15__INCLUDED_)
#define AFX_DIALOGLINKEXISTING_H__B135C0F6_2637_11D2_88F7_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogLinkExisting.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogLinkExisting dialog

class CDialogLinkExisting : public CDialog
{
// Construction
public:
	CDialogLinkExisting(CWnd* pParent = NULL);   // standard constructor

	class CDirectMusicPartRef *m_pDMPartRef;
	const class CDirectMusicStyle* m_pDMStyle;

// Dialog Data
	//{{AFX_DATA(CDialogLinkExisting)
	enum { IDD = IDD_PART_LINK };
	CListBox	m_listPart;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogLinkExisting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogLinkExisting)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGLINKEXISTING_H__B135C0F6_2637_11D2_88F7_00C04FBF8D15__INCLUDED_)
