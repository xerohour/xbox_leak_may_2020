#if !defined(AFX_EDITSOURCE_H__0242CEE0_98A1_43AB_81B4_293E60BD6ACA__INCLUDED_)
#define AFX_EDITSOURCE_H__0242CEE0_98A1_43AB_81B4_293E60BD6ACA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditSource.h : header file
//


class CScriptDlg;

/////////////////////////////////////////////////////////////////////////////
// CEditSource window

class CEditSource : public CEdit
{
friend class CScriptDlg;

// Construction
public:
	CEditSource();

// Attributes
public:
	CScriptDlg*		m_pScriptDlg;

protected:
	bool			m_fSourceChanged;
	bool			m_fNeedUndoEntry;
	bool			m_fInOnLButtonDown;
	DWORD			m_dwLastSelection;
	CString			m_strSourceForUndo;

// Operations
protected:
	void SaveUndoState();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditSource)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditSource();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditSource)
	afx_msg void OnChange();
	afx_msg void OnKillFocus();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITSOURCE_H__0242CEE0_98A1_43AB_81B4_293E60BD6ACA__INCLUDED_)
