#if !defined(AFX_EDITVALUE_H__25634E66_BE46_47F0_B33E_07BAD7B8C5CF__INCLUDED_)
#define AFX_EDITVALUE_H__25634E66_BE46_47F0_B33E_07BAD7B8C5CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditValue.h : header file
//


class CScriptDlg;

/////////////////////////////////////////////////////////////////////////////
// CEditValue window

class CEditValue : public CEdit
{
friend class CScriptDlg;

// Construction
public:
	CEditValue();

// Attributes
public:
	CScriptDlg*		m_pScriptDlg;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditValue)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditValue();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditValue)
	afx_msg void OnKillFocus();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITVALUE_H__25634E66_BE46_47F0_B33E_07BAD7B8C5CF__INCLUDED_)
