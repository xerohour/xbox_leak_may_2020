#if !defined(AFX_GRIDCTL_H__EAB0CD53_9459_11D0_8C10_00A0C92E1CAC__INCLUDED_)
#define AFX_GRIDCTL_H__EAB0CD53_9459_11D0_8C10_00A0C92E1CAC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// GridCtl.h : Declaration of the CGridCtrl ActiveX Control class.

#include "BandEditor.h"
#include "Channel.h"

/////////////////////////////////////////////////////////////////////////////
// CGridCtrl : See GridCtl.cpp for implementation.

class CGridCtrl : public COleControl
{
	DECLARE_DYNCREATE(CGridCtrl)

// Constructor
public:
	CGridCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CGridCtrl();

	DECLARE_OLECREATE_EX(CGridCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CGridCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CGridCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CGridCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CGridCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CGridCtrl)
	afx_msg void BringToFront(long nId);
	afx_msg void MoveButton(long nId, short nPan, short nVolume);
	afx_msg void RemoveButton(long nId);
	afx_msg void RemoveAllButtons();
	afx_msg void SelectButton(long nId, BOOL bSelection);
	afx_msg void SetEnabled(long nID, BOOL bEnable);
	afx_msg BOOL AddButton(LPUNKNOWN pInstrumentItem);
	afx_msg void UpdateButton(long nPChannel);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CGridCtrl)
	void FireCursorOver(long nId)
		{FireEvent(eventidCursorOver,EVENT_PARAM(VTS_I4), nId);}
	void FireMove(long nId, short nPan, short nVolume)
		{FireEvent(eventidMove,EVENT_PARAM(VTS_I4  VTS_I2  VTS_I2), nId, nPan, nVolume);}
	void FireUpdate(LPUNKNOWN pInstrument, BOOL bCTRLDown, BOOL bRefreshUI)
		{FireEvent(eventidUpdate,EVENT_PARAM(VTS_UNKNOWN  VTS_BOOL  VTS_BOOL), pInstrument, bCTRLDown, bRefreshUI);}
	void FireSelectPChannel(long nId, BOOL bSelection, BOOL bCtrlDown)
		{FireEvent(eventidSelectPChannel,EVENT_PARAM(VTS_I4  VTS_BOOL  VTS_BOOL), nId, bSelection, bCtrlDown);}
	void FireSaveUndoState()
		{FireEvent(eventidSaveUndoState,EVENT_PARAM(VTS_NONE));}
	void FireDisplayStatus(short nID)
		{FireEvent(eventidDisplayStatus,EVENT_PARAM(VTS_I2), nID);}
	void FireRightClick(short nX, short nY)
		{FireEvent(eventidRightClick,EVENT_PARAM(VTS_I2  VTS_I2), nX, nY);}
	void FireDeleteChannel()
		{FireEvent(eventidDeleteChannel,EVENT_PARAM(VTS_NONE));}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CGridCtrl)
	dispidBringToFront = 1L,
	dispidMoveButton = 2L,
	dispidRemoveButton = 3L,
	dispidSelectButton = 5L,
	dispidSetEnabled = 6L,
	dispidAddButton = 7L,
	dispidUpdateButton = 8L,
	eventidCursorOver = 1L,
	eventidMove = 2L,
	eventidUpdate = 3L,
	eventidSelectPChannel = 4L,
	eventidSaveUndoState = 5L,
	eventidDisplayStatus = 6L,
	eventidRightClick = 7L,
	eventidDeleteChannel = 8L,
	//}}AFX_DISP_ID
	};

	// UI function to draw a 3D border around the control
	void EmbossRect( HDC hdc, const CRect *pRect, char raised );

	// embossed border width
	const short m_EBW;

	CBitmap	m_bmBtnUp;
	CBitmap	m_bmBtnDn;
	CBitmap	m_bmDrums;
	CBitmap	m_bmMask;

private:
	CRectTracker* m_pTracker;

	CRect		m_rcExtent;
	CRect		m_rcAdjusted;
	CChannel*	m_pBtnLst;
	CChannel*	m_pDrag;
	CChannel*	m_pOver;
	CBitmap		m_bmGridTile;
	CPoint		m_ptTouch;
	BOOL		m_fMoved;
	BOOL		m_bSized; // Set TRUE on create time OnSize...
	bool		m_bMultiSelected;
	bool		m_bSendUpdate;


	bool		AreAnyChannelsSelected();
	void		UnselectAllButtons();
	void		SelectAllButtons();
	void		MarkButtonForMove(CChannel* pButton, CPoint point);
	CRect*		GetAdjustedArea( CChannel *pc );
	void		_BringToFront( CChannel *pc );
	void		SendToBack();
	CChannel*	GetChannelByID(long nID);

	void		StepVolume(bool bStepUp);
	void		StepPan(bool bStepRight);

	CChannel*	SwapAdjacentNodes( CChannel *pParent, CChannel *pChild );
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDCTL_H__EAB0CD53_9459_11D0_8C10_00A0C92E1CAC__INCLUDED)
