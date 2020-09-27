#if !defined(AFX_PIANOROLLSCROLLBAR_H__2D72BA9D_7BDD_11D1_885E_00C04FBF8D15__INCLUDED_)
#define AFX_PIANOROLLSCROLLBAR_H__2D72BA9D_7BDD_11D1_885E_00C04FBF8D15__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PianoRollScrollBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPianoRollScrollBar window

class CPianoRollScrollBar : public CScrollBar
{
// Construction
public:
	CPianoRollScrollBar();

// Attributes
public:
private:
	class CPianoRollStrip* m_pPRS;

// Operations
public:
	void SetPianoRollStrip( class CPianoRollStrip* pPRS );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPianoRollScrollBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPianoRollScrollBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPianoRollScrollBar)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	LRESULT OnApp( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIANOROLLSCROLLBAR_H__2D72BA9D_7BDD_11D1_885E_00C04FBF8D15__INCLUDED_)
