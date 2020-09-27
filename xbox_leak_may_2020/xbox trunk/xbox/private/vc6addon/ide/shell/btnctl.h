// btnctl.h : List box that allows dragging of toolbar buttons from it
/////////////////////////////////////////////////////////////////////////////

#ifndef __BTNCTL_H__
#define __BTNCTL_H__

class CToolCustomizer;

/////////////////////////////////////////////////////////////////////////////
// CButtonDragListBox control

class CButtonDragListBox : public CListBox
{
// Construction
public:
	CButtonDragListBox();

// Implementation
public:
	virtual ~CButtonDragListBox();

	enum SPECIALITEM
	{
		cNextFreeMenu=-1
	};

	BOOL	Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CButtonDragListBox)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CButtonDragListBox)
};

#endif // __BTNCTL_H__
