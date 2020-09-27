#if !defined(AFX_SEQUENCESCROLLBAR_H__BAA6C243_24E4_11D2_850F_00A0C99F7E74__INCLUDED_)
#define AFX_SEQUENCESCROLLBAR_H__BAA6C243_24E4_11D2_850F_00A0C99F7E74__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SequenceScrollBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSequenceScrollBar window

class CSequenceScrollBar : public CScrollBar
{
public:
	CSequenceScrollBar();
	virtual ~CSequenceScrollBar();

	void SetSequenceStrip(class CSequenceStrip* pSequenceStrip);

private:
	class CSequenceStrip* m_pSequenceStrip;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSequenceScrollBar)
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	//{{AFX_MSG(CSequenceScrollBar)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	LRESULT OnApp( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEQUENCESCROLLBAR_H__BAA6C243_24E4_11D2_850F_00A0C99F7E74__INCLUDED_)
