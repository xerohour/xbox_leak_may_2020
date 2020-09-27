#if !defined(AFX_MUTESCROLLBAR_H__BAA6C243_24E4_11D2_850F_00A0C99F7E74__INCLUDED_)
#define AFX_MUTESCROLLBAR_H__BAA6C243_24E4_11D2_850F_00A0C99F7E74__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MuteScrollBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMuteScrollBar window

class CMuteScrollBar : public CScrollBar
{
public:
	CMuteScrollBar();
	virtual ~CMuteScrollBar();

	void SetMuteStrip(class CMuteStrip* pMuteStrip);

private:
	class CMuteStrip* m_pMuteStrip;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMuteScrollBar)
	//}}AFX_VIRTUAL


	// Generated message map functions
protected:
	//{{AFX_MSG(CMuteScrollBar)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MUTESCROLLBAR_H__BAA6C243_24E4_11D2_850F_00A0C99F7E74__INCLUDED_)
