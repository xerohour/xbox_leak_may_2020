#if !defined(AFX_REGIONKEYBOARDCTL_H__36CD3195_EE61_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_REGIONKEYBOARDCTL_H__36CD3195_EE61_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "RegionNote.h"
#include "AfxTempl.h"


#define NUMBER_OF_VISIBLE_LAYERS	4	// Number of layers visible by default


#define COLOR_ACTIVE_LAYER	PALETTERGB(244, 243, 152)
#define COLOR_LAYER_CANVAS	PALETTERGB(255, 255, 255)
#define COLOR_LAYER_SELECT	PALETTERGB(255, 0, 0)

#define RK_NULL		0
#define RK_START	(1 << 0)
#define RK_END		(1 << 1)
#define	RK_MOVE		(1 << 2)
#define	RK_COPY		(1 << 3)

#define	AUDITION_SOLO		0
#define AUDITION_MULTIPLE	1


	
class CRegion;

// RegionKeyboardCtl.h : Declaration of the CRegionKeyboardCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CRegionKeyboardCtrl : See RegionKeyboardCtl.cpp for implementation.

class CRegionKeyboardCtrl : public COleControl
{
	DECLARE_DYNCREATE(CRegionKeyboardCtrl)

// Constructor
public:
	CRegionKeyboardCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegionKeyboardCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CRegionKeyboardCtrl();

	DECLARE_OLECREATE_EX(CRegionKeyboardCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CRegionKeyboardCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CRegionKeyboardCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CRegionKeyboardCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CRegionKeyboardCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CRegionKeyboardCtrl)
	afx_msg void InitializeKeyBoard(LPUNKNOWN RegionMap);
	afx_msg void SetCurrentRegion(short nLayer, short nStartNote);
	afx_msg void MidiEvent(short nMidiNote, short nNoteFlag, short nVelocity);
	afx_msg void SetRootNote(short nNote);
	afx_msg short InsertNewRegion(short nStartNote, short nEndNote, short nStartVelocity, short nEndVelocity);
	afx_msg short InsertRegion(short nLayer, short nStartNote, short nEndNote, short nStartVelocity, short nEndVelocity, LPCTSTR pszWaveName);
	afx_msg BOOL SetRange(short nStartNote, short nEndNote, short nStartVelocity, short nEndVelocity);
	afx_msg void DeleteRegion(short nLayer, short nStartNote);
	afx_msg void DeleteAllRegions();
	afx_msg void ReleaseMouseCapture();
	afx_msg void TurnOffMidiNotes();
	afx_msg short SetCurrentLayer(short nLayer);
	afx_msg short SetFirstVisibleLayer(short nLayer);
	afx_msg short ScrollLayers(short nSBCode, short nCurPos);
	afx_msg short GetNumberOfLayers();
	afx_msg short GetFirstVisibleLayer();
	afx_msg short AddNewLayer();
	afx_msg short DeleteActiveLayer();
	afx_msg void SetAuditionMode(BOOL bAuditionMode);
	afx_msg void EnableRegion(short nLayer, short nStartNote, BOOL bEnable);
	afx_msg void SetNumberOfLayers(short nLayers);
	afx_msg void SetWaveName(short nLayer, short nStartNote, LPCTSTR pszWaveName);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CRegionKeyboardCtrl)
	void FireRegionSelectedChanged(short nLayer, short nStartNote)
		{FireEvent(eventidRegionSelectedChanged,EVENT_PARAM(VTS_I2  VTS_I2), nLayer, nStartNote);}
	void FireNewRegion(short nLayer, long lower, long upper)
		{FireEvent(eventidNewRegion,EVENT_PARAM(VTS_I2  VTS_I4  VTS_I4), nLayer, lower, upper);}
	void FireRangeChanged(short nLayer, long lower, long upper)
		{FireEvent(eventidRangeChanged,EVENT_PARAM(VTS_I2  VTS_I4  VTS_I4), nLayer, lower, upper);}
	void FireNotePlayed(long nNote, BOOL bType)
		{FireEvent(eventidNotePlayed,EVENT_PARAM(VTS_I4  VTS_BOOL), nNote, bType);}
	void FireRegionMoved(short nOldLayer, short nOldStartNote, short nMovedLayer, short nMovedStartNote, short nMovedEndNote)
		{FireEvent(eventidRegionMoved,EVENT_PARAM(VTS_I2  VTS_I2  VTS_I2  VTS_I2  VTS_I2), nOldLayer, nOldStartNote, nMovedLayer, nMovedStartNote, nMovedEndNote);}
	void FireRegionDeleted(short nLayer, short nStartNote)
		{FireEvent(eventidRegionDeleted,EVENT_PARAM(VTS_I2  VTS_I2), nLayer, nStartNote);}
	void FireActiveLayerChanged(short nLayer)
		{FireEvent(eventidActiveLayerChanged,EVENT_PARAM(VTS_I2), nLayer);}
	void FireCopyRegion(short nSourceLayer, short nSourceStartNote, short nCopyLayer, short nCopyStartNote)
		{FireEvent(eventidCopyRegion,EVENT_PARAM(VTS_I2  VTS_I2  VTS_I2  VTS_I2), nSourceLayer, nSourceStartNote, nCopyLayer, nCopyStartNote);}
	void FireScrollLayers(BOOL bUp)
		{FireEvent(eventidScrollLayers,EVENT_PARAM(VTS_BOOL), bUp);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CRegionKeyboardCtrl)
	dispidInitializeKeyBoard = 1L,
	dispidSetCurrentRegion = 2L,
	dispidMidiEvent = 3L,
	dispidSetRootNote = 4L,
	dispidInsertNewRegion = 5L,
	dispidInsertRegion = 6L,
	dispidSetRange = 7L,
	dispidDeleteRegion = 8L,
	dispidDeleteAllRegions = 9L,
	dispidReleaseMouseCapture = 10L,
	dispidTurnOffMidiNotes = 11L,
	dispidSetCurrentLayer = 12L,
	dispidSetFirstVisibleLayer = 13L,
	dispidScrollLayers = 14L,
	dispidGetNumberOfLayers = 15L,
	dispidGetFirstVisibleLayer = 16L,
	dispidAddNewLayer = 17L,
	dispidDeleteActiveLayer = 18L,
	dispidSetAuditionMode = 19L,
	dispidEnableRegion = 20L,
	dispidSetNumberOfLayers = 21L,
	dispidSetWaveName = 22L,
	eventidRegionSelectedChanged = 1L,
	eventidNewRegion = 2L,
	eventidRangeChanged = 3L,
	eventidNotePlayed = 4L,
	eventidRegionMoved = 5L,
	eventidRegionDeleted = 6L,
	eventidActiveLayerChanged = 7L,
	eventidCopyRegion = 8L,
	eventidScrollLayers = 9L,
	//}}AFX_DISP_ID
	};


public:
	int GetActiveLayer();

private:
	void DrawKeyboard(CDC* pDC, const CRect& rcBounds);
	void DrawLayersOnRegionBitmap(CDC* pDC, CRect& regionRect);
	void DrawRegionRects(CDC* pDC);
	void SetNormalCursor();		
	void SetNoDoCursor(bool bOn); //  A slashed circle
	void SetSizeCursor(bool bOn, bool bResizeStart);
	void SetMoveCursor(bool bOn);
	void SetCopyCursor(bool bOn);
    void DrawRootNote(CDC * pdc);
	void MarkPlayingRegions(short nNote, short nNoteFlag, short nVelocity, CPtrList* plstPlayingRegions);
	
	int			GetNotePlayed(CPoint point);
	void		PlayNote(int nNote, bool bType);
	int			GetKeyIndexFromPoint(CPoint point);
	CRegion*	IsMemberOfRegion(int nKey);				// Returns the region containing this note; else returns NULL
	int			GetGoodLayer(CPoint point);
	void		SelectAllRegions(int nNote);
	void		SelectRegion(CPoint point);
	void		SelectRegion(int nLayer, int nStartNote);
	CRegion*	GetRegionForPoint(CPoint point, bool bIgnoreY);
	int			GetLayerFromPoint(CPoint point);
	int			GetAbsoluteLayerFromPoint(CPoint point);
	int			GetRootNoteFromActiveRegion();
	void		SetActiveLayer(CPoint point);
	int			_InsertNewRegion(int nLayer, int nStartNote, int nEndNote, LPCTSTR pszWaveName);
	int			_InsertNewRegion(int nStartNote, int nEndNote);
	void		UpdateRegionRect();
	CRegion*	CheckForCollision(CRegion* pCheckedRegion, int nLayer, int nStartNote,int nEndNote, bool bCheckSelfCollision = false);
	void		CopySelectedRegionAtPoint(CPoint point);
	CRegion*	GetRegionBeforeStartNote(CRegion* pRegion);
	CRegion*	GetRegionAfterEndNote(CRegion* pRegion);

	void		DrawGhostRect(CRegion* pRegion, CPoint ptPeg, CPoint ptDrag);

private:
	
	enum 
	{
		MIN_POS = 0, REGION_LAYER_HEIGHT = 10, 
		REGION_LAYER_AREA_HEIGHT = NUMBER_OF_VISIBLE_LAYERS * REGION_LAYER_HEIGHT, 
		PAGE_SIZE = 12, PAGE_SIZE_PIXELS = 56
	};

    int m_nMaxScrollPos;   
	//TODO: page_size is pagesize in notes. 
	static const int m_nKeyPos[12];
	static const int m_nKeySize[12];
    static const int m_nKeyCenter[12];  // center of key at whoch the circle is drawn
    static const int m_nRadius[12]; // radius of the circle that shows key is a root note.
	static const int m_nRegionKeyboardSize;

	int			m_nLastMidiNoteOn[128];

	bool		m_bNoteIsOn;
	CPoint		m_ptLastNote;

	RegionNote m_nRegionMap[128];

	CRect		m_RegionsRect;
	int			m_nNumberOfLayers;
	int			m_nFirstVisibleLayer;				// keeps track of the first layer to handle scrolling
	int			m_nActiveLayer;						// The active layer
	int			m_nDraggedKey;
	CRegion*	m_pDraggedRegion;					// Used to keep track of the dragged region
	CRegion*	m_pCurrentRegion;
	CTypedPtrList<CPtrList, CRegion*> m_lstRegions; // List of regions
	CFont		m_fnRegion;						// cached font for region labels
	bool		m_bRegionFontCreated;			// whether m_fnRegion has been created

	int m_nCurKeyPos;

	static	int			m_nKeyboards;				// Keeps track of instances 
	static	HANDLE		m_hKeyBoard128;			
	static	CBitmap		m_bmKeyBoard128;
	static	HANDLE		m_hBlank128;
	static	CBitmap		m_bmBlank128;

	int			m_nKeyboardOffset;
	int			m_nCurRegion;
	int			m_nRegionMapIndex;
	
	bool		m_bHasCapture;
	CPoint		m_ptLButtonDown;
	static	HCURSOR		m_hArrowCursor;
	static	HCURSOR		m_hStartEditCursor;
	static	HCURSOR		m_hEndEditCursor;
	static	HCURSOR		m_hSIZEWECursor;
	static	HCURSOR		m_hMOVECursor;
	static	HCURSOR		m_hCOPYCursor;
	static	HCURSOR		m_hNODOCursor;
	bool		m_bStartCursorSet;
	bool		m_bEndCursorSet;
	bool		m_bMoveCursorSet;
	bool		m_bCopyCursorSet;
	bool		m_bNoDoCursorSet;
	
	int 		m_nStartEnd;
	int			m_nMaxEndPixel;
	int			m_nMinEndPixel;

	BOOL		m_bAuditionMode;	// MULTIPLE_LAYER == 1 ; SOLO_LAYER == 0
	CRect		m_lastGhostRect;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONKEYBOARDCTL_H__36CD3195_EE61_11D0_876A_00AA00C08146__INCLUDED)
