#if !defined(AFX_KEYBOARD_H__E41D1398_A999_11D1_9858_00805FA67D16__INCLUDED_)
#define AFX_KEYBOARD_H__E41D1398_A999_11D1_9858_00805FA67D16__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Keyboard.h : header file
//
// Keyboard.h : header file
//
class KeyInfo
{
public:
				KeyInfo();
	void		SetState(short nState);
	CRect		m_crDrawRect;
	CString		m_csName;
	short		m_nState;
	BOOL		m_fBlackKey;
	BOOL		m_fRootKey;
	COLORREF	m_Color;
};


inline int HighestBit(DWORD pattern)
{
	int highestbit = -1;
	for(int i = 0; i < sizeof(DWORD) * 8; i++, pattern = pattern >> 1)
	{
		if(pattern & 1)
		{
			highestbit = i;
		}
	}
	return highestbit;
}

class CKeyboard;

typedef void (*CKeyboardCallback)(CKeyboard* caller, void* hint, short nKey);
typedef void (*CMouseCallback)(CKeyboard* caller, void* hint, short nKey, CPoint& ptMouse);

/////////////////////////////////////////////////////////////////////////////
// CKeyboard window

class CKeyboard : public CButton
{
// Construction
public:
	CKeyboard();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyboard)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
private:
	// use to get mouse down/up calls
	CKeyboardCallback m_pfnLButtonDown;
	CKeyboardCallback m_pfnLButtonUp;
	CMouseCallback m_pfnRButtonDown;
	void*	m_pHintLButtonDown;
	void*	m_pHintLButtonUp;
	void*	m_pHintRButtonDown;
public:
	enum {Size = 48};
	virtual ~CKeyboard();
	void SetLowerBound( int where)
	{
		ASSERT(0 <= where && where < 24);
		m_lowerbound  = static_cast<short>(where);
		Invalidate(FALSE);
	}
	void ShiftKeys(short howmuch)
	{
		m_lowerbound = static_cast<short>(m_lowerbound + howmuch);
		if(m_lowerbound < 0)
		{
			m_lowerbound = 0;
		}
		else if(m_lowerbound > m_visible)
		{
			m_lowerbound = m_visible;
		}
		Invalidate(FALSE);
	}
	int Transpose() const
	{
		return m_transpose;
	}
	void Transpose(int newval, bool bShift=false)
	{
		int howmuch = newval - m_transpose;
		m_transpose = static_cast<short>(newval);
		if(m_transpose < 0)
		{
			m_transpose = 0;
		}
		/* this code is faulty and needs to be rethought
		else if(m_transpose > Visible + m_lowerbound)
		{
			// need to shift keyboard to make chord visible
			bShift = true;
//			m_transpose = Visible;
//			bShift = true;	// must shift keyboard
//			m_lowerbound = howmuch;
		}
		*/
		if(bShift)
		{
			ShiftKeys(static_cast<short>(howmuch));
		}
	}
	void SetLButtonDownCallback(CKeyboardCallback pfn, void* hint)
	{
		m_pfnLButtonDown = pfn;
		m_pHintLButtonDown = hint;
	}
	void SetLButtonUpCallback(CKeyboardCallback pfn, void* hint)
	{
		m_pfnLButtonUp = pfn;
		m_pHintLButtonUp = hint;
	}
	void SetRButtonDownCallback(CMouseCallback pfn, void* hint)
	{
		m_pfnRButtonDown = pfn;
		m_pHintRButtonDown = hint;
	}
	short LowerBound()
	{
		return m_lowerbound;
	}
	short Extent()
	{
		return m_visible;
	}
	void SetExtent(short s)
	{
		m_visible = s;
	}
	short GetRootKeys()
	{
		return m_nRootKeys;
	}
	void SetRootKeys(short n);
	void		DrawKeyboard(CDC* pdc);
	int FindKey(CPoint point);
	short GetNoteState(short nKey)
	{
		return m_Keys[nKey].m_nState;
	}
	void SetNoteState(short nKey, LPCTSTR pszName, short nState);

	void SetNewSize(const CRect& crNewRect);

	void ClearKeys();

private:
	short		  m_lowerbound;
	short		  m_transpose;
	short		  m_visible;
	short		  m_nRootKeys;
	KeyInfo		m_Keys[Size];
	CRect		m_crBoundRect;


	// Generated message map functions
protected:
	//{{AFX_MSG(CKeyboard)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYBOARD_H__E41D1398_A999_11D1_9858_00805FA67D16__INCLUDED_)
