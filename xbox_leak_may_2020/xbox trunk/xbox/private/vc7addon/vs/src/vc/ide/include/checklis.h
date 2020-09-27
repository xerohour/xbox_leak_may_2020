/////////////////////////////////////////////////////////////////////////////
// Check listbox
//
// A CCheckList is a listbox with a checkbox to the left of each item.
// The user interface is exactly the same as a listbox with these
// exceptions:
//
//     * Left mouse button clicks (or double clicks) on the checkbox
//       part of an item toggle the check state.
//     * Pressing the spacebar when an item is selected will toggle
//       the check state of that item.
//     * For multi-sel listbox's, the spacebar will cycle through the
//       four state combinations (unselected-unchecked, selected-unchecked,
//       unselected-checked, selected-checked).
//
// These controls may be created explicitly with Create(), or existing
// listbox windows may be "subclassed" with SubclassDlgItem().
//
// When creating one of these (or specifying one in a dialog template)
// make sure the LBS_OWNERDRAWFIXED and LBS_HASSTRINGS styles are set.
//
// The class supports different sized fonts and will ensure that items
// are tall enough to contain both the box and the text.  The font may
// be changed dynamically.
//
// The default implementation uses the item data to store the state
// of the checkbox.  If the item data is needed for some other purpose,
// the SetCheck() and GetCheck() functions should be overrided to
// use an alternate storage location.

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __CHECKLIS_H__
#define __CHECKLIS_H__

#include "slob.h"
#ifndef SLOBAPI
#define SLOBAPI __declspec()
#endif

#define CLN_CHKCHANGE (20)
#define ON_CLN_CHKCHANGE(id, memberFxn) \
	{ WM_COMMAND, (WORD)CLN_CHKCHANGE, (WORD)id, (WORD)id, AfxSig_vv, \
		(AFX_PMSG)memberFxn },

class SLOBAPI CCheckList : public CListBox
{
// Construction
public:
	CCheckList();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
public:
	virtual BOOL IsTriState(int nItem) {return FALSE;}
	// nCheck -
	// 0 = unchecked
	// 1 = checked
	// 2 = undeterminate
	virtual void SetCheck(int nItem, int nCheck);
	virtual int GetCheck(int nItem);

// Operations
public:
	BOOL SubclassDlgItem(UINT nID, CWnd* pParent);

// Implementation
protected:
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void MeasureItem(LPMEASUREITEMSTRUCT lpmis);
	void InvalidateCheck(int nItem);
	void ResetItemHeight();

	virtual void ItemStringOut (CDC * pDC, const CString& strOut, const CRect& rect);
	
	int m_cyItem;	// Height of item including check box
	int m_cyText;	// Height of text only
	static CBitmap c_checkBitmap;
	static CSize c_checkSize;

	// Message map functions
protected:
	//{{AFX_MSG(CCheckList)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	virtual void GetDisplayText(int index, CDC *pDC, const CRect&, CString& strOut);
};

/////////////////////////////////////////////////////////////////////////////
// CPropCheckList window - a check-list that can show/set property values

class SLOBAPI CPropCheckList : public CCheckList
{
public:
	virtual void OnPropChanged(UINT nIDProp);

	// add an association between item text and property to the list
	int AddItem(LPCTSTR szId, UINT nIDProp);

	// which CSlob do the properties in this list belong to?
	inline void SetSlob(CSlob* pSlob) {m_pSlob = pSlob;}

protected:
	// provide property processing
	virtual BOOL IsTriState(int);
	virtual void SetCheck(int, int);
	virtual int GetCheck(int);

private:
	CSlob * m_pSlob;
};


/////////////////////////////////////////////////////////////////////////////

#endif
