#if !defined(AFX_KEYSOCXCTL_H__CDD09F93_E73C_11D0_89AB_00A0C9054129__INCLUDED_)
#define AFX_KEYSOCXCTL_H__CDD09F93_E73C_11D0_89AB_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// KeysOCXCtl.h : Declaration of the CKeysOCXCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CKeysOCXCtrl : See KeysOCXCtl.cpp for implementation.

class KeyInfo
{
public:
				KeyInfo();
	void		SetState(short nState);
	CRect		m_crDrawRect;
	CString		m_csName;
	short		m_nState;
	BOOL		m_fBlackKey;
	COLORREF	m_Color;
};

class CKeysOCXCtrl : public COleControl
{
	DECLARE_DYNCREATE(CKeysOCXCtrl)

// Constructor
public:
	CKeysOCXCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeysOCXCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CKeysOCXCtrl();

	DECLARE_OLECREATE_EX(CKeysOCXCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CKeysOCXCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CKeysOCXCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CKeysOCXCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CKeysOCXCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CKeysOCXCtrl)
	afx_msg void SetNoteState(short nKey, LPCTSTR pszName, short nState);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CKeysOCXCtrl)
	void FireNoteDown(short nKey)
		{FireEvent(eventidNoteDown,EVENT_PARAM(VTS_I2), nKey);}
	void FireNoteUp(short nKey)
		{FireEvent(eventidNoteUp,EVENT_PARAM(VTS_I2), nKey);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CKeysOCXCtrl)
	dispidSetNoteState = 1L,
	eventidNoteDown = 1L,
	eventidNoteUp = 2L,
	//}}AFX_DISP_ID
	};
	void		SetNewSize(const CRect& crNewRect);
	int			FindKey(CPoint point);
	void		DrawKeyboard(CDC* pdc);
	KeyInfo		m_Keys[24];
	CRect		m_crBoundRect;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYSOCXCTL_H__CDD09F93_E73C_11D0_89AB_00A0C9054129__INCLUDED)
