#if !defined(AFX_SCRIPTEDIT_H__5DBD135B_D610_449C_90E0_76C7932AEA1C__INCLUDED_)
#define AFX_SCRIPTEDIT_H__5DBD135B_D610_449C_90E0_76C7932AEA1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptEdit.h : header file
//

class CConditionEditor;

/////////////////////////////////////////////////////////////////////////////
// CScriptEdit window

class CScriptEdit : public CEdit
{
// Construction
public:
	CScriptEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScriptEdit();
	void SetEditor(CConditionEditor* pEditor);

private:
	CConditionEditor* m_pConditionEditor;

	// Generated message map functions
protected:
	//{{AFX_MSG(CScriptEdit)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTEDIT_H__5DBD135B_D610_449C_90E0_76C7932AEA1C__INCLUDED_)
