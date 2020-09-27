#if !defined(AFX_WAVECTL_H__BC964EAE_96F7_11D0_89AA_00A0C9054129__INCLUDED_)
#define AFX_WAVECTL_H__BC964EAE_96F7_11D0_89AA_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ================================================================
// constants used by wavectl
// ================================================================
const int PAGE_SIZE_IN_PIXELS = 20;
const int MAX_ZOOM_OUT = 16;
const int START= 1;
typedef enum {
	_NONE = 0,
	_START = 1,
	_END = 2
} DRAGDIR;

typedef enum {
	dcArrow,
	dcSizeSelection,
	dcSizeLoopStart,
	dcSizeLoopEnd,
} DRAGCURSOR;

class CWave;
class CWaveNode;
class CUndoMan;

// ================================================================
//
// WaveCtl.h : Declaration of the CWaveCtrl ActiveX Control class.
//
// ================================================================

/////////////////////////////////////////////////////////////////////////////
// CWaveCtrl : See WaveCtl.cpp for implementation.

class CWaveCtrl : public COleControl
{
friend class CDLSComponent;
	DECLARE_DYNCREATE(CWaveCtrl)

// Constructor
public:
	CWaveCtrl();

	CWave* GetWave() {return(m_pWave);}

	void	PreparePlaybackTimer();
	void	KillPlaybackTimer();
	void	SetSelection(int nSelectionStart, int nSelectionEnd, BOOL bDoSnap = TRUE);		// Sets the selection
	void	GetSelection(int& nSelectionStart, int& nSelectionEnd);	// Gets the selection
	
	int		GetScrolledSamples();	// Returns m_nxSampleOffset
	int		GetSamplesPerPixel();	// Returns the number of samples that map per pixel (changes with the zoom ratio)
	int		GetZoomFactor();		// Returns the zoom factor

	// Returns the sample that draws at this pixel. Counts from start if bCountFromDecompressedStart is false. Sample can be negative.
    int		PixelToSample(int x, bool bCountFromDecompressedStart);

	// Returns the pixel the sample should draw at. Counts from start if bCountFromDecompressedStart is false. Sample can be negative.
	int		SampleToPixel(int n, bool bCountFromDecompressedStart);

// Attributes
private:
	void	UpdateRefRegions();
	void	UpdateLoopInRegions();
	void	TurnOffMidiNotes();
	void	CutSelectionToClipboard();
	void	SetSelection(CPoint ptSelection);
	void	SetDraggedLoopPoint(CPoint ptDraggedPoint);
	HRESULT RemoveSelection();
	void	UpdateSelectionCmdUI(CCmdUI* pCmdUI);
	void	PasteFromClipboard();
	HRESULT	CopySelectionToClipboard();
	void	UpdatePlaybackPos(int nNewPos, CRect* pRectInvalid = NULL);
	

	HMENU			m_hMenuInPlace;
	HACCEL			m_hAcceleratorTable;
	HANDLE			m_hKeyStatusBar;
	CToolBar*		m_pToolBar;
	CWaveNode*		m_pWaveNode;
	CWave*			m_pWave;
    CScrollBar      m_HScrollBar;
    int             m_nxSampleOffset;				// The number of scrolled samples.
    short           m_nZoomFactor;
    CBitmapButton   m_BtnZoomIn;
    CBitmapButton   m_BtnZoomOut;
    int             m_nSamplesPerPixel;				// Reflects amount of zoom. In case of zoom < 0, this is the number of *pixels* per sample
	int				m_nSamplesPerPixelBeforeZoom;	// Used only to decide wheree to zoom in !! This is a HACK !!

    int				m_nStartSel;					// In samples, from decompressed start
    int				m_nEndSel;						// In samples, from decompressed start
    BOOL			m_bInSelMode;					// If we're currently dragging out a selection.
	bool			m_bDraggingLoop;				// Set is we're dragging a loop point 			
    DRAGDIR			m_nDragDir;						// What's being dragged: selection, loop start, or loop end

	HCURSOR			m_hSIZEWECursor;
	HCURSOR			m_hPrevCursor;
	HCURSOR			m_hSizeLoopStartCursor;
	HCURSOR			m_hSizeLoopEndCursor;
	
	bool			m_bSizeCursorSet;
    BOOL			m_bSnapToZero;
    int				m_nSnapStart;					// Snapped-to-zero value for start of selection
    int				m_nSnapEnd;
    DWORD			m_dwMaxScrollPos;
	int				m_nCurPos;						// This was in a static variable local to OnHScroll()
    CHAR			m_szDefault[50];

    CInstrument*	m_pDummyInstrument;

    CMenu *			m_pRMenu;
    CString			m_cstr;        

    DWORD			m_dwCookie;

	WORD			m_nMIDINoteOns[128];			// From instrument.h
	UINT			m_uiTimer;						// Id of timer object used for keeping track of playback
	int				m_nPlaybackPos;
	bool			m_bPlayedLooped;
	int				m_nPlayLoopStart;
	int				m_nPlayLoopLength;
	short			m_sPlayFineTune;
	CPoint			m_ptDraggedLoop;


// Methods
private:
    void			ResetScrollBar();
	void			SetZoomFactor(int nNewZoomFactor);
	void			SetSizeCursor(bool bOn, DRAGCURSOR dc = dcArrow);
    void			SnapToZero(void);
    void			UpdateStatusBar(void);
    void			DrawLoopPoints(CDC * pdc);
    void			SetupDefaultStatusText(void);
	void			SwapPoints(void);
	void			UpdateSelectionInPropertyPage(bool bBroadcastUpdate = true);
	void			UpdateLoopInPropertyPage();
	void			DeleteSelection();
	void			SetSelectionBounds();
	HRESULT			Fade(bool bFadeDirection = true);	// bFadeDirection == true -> Fade In; false -> Out

	/* ensures the given sample count is within selection boundaries */
	void			SnapToSelectionBoundaries(int& nSel);

	/* draws the given loop cursor using the given pen style */
	void			DrawLoopCursor(CDC &dc, int nPenStyle, int x, DRAGDIR dd);

protected:
	CDllJazzDataObject*		m_pClipboardDataObject;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaveCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual HMENU OnGetInPlaceMenu();
	virtual void OnHideToolBars();
	virtual void OnShowToolBars();
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CWaveCtrl();

	DECLARE_OLECREATE_EX(CWaveCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CWaveCtrl)      // GetTypeInfo
	//DECLARE_PROPPAGEIDS(CWaveCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CWaveCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CWaveCtrl)
	afx_msg void AboutBox();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSnapToZero();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetLoopFromSelection();
	afx_msg void OnProperties();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnAppAbout();
	afx_msg void OnUpdateSetLoopFromSelection(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnCopy();
	afx_msg void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnCut();
	afx_msg void OnUpdateCut(CCmdUI* pCmdUI);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHelpFinder();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnCopyLoop();
	afx_msg void OnRegionFadeIn();
	afx_msg void OnRegionFadeOut();
	afx_msg void OnUpdateRegionFadeIn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRegionFadeOut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopyLoop(CCmdUI* pCmdUI);
	afx_msg void OnSelectAll();
	afx_msg void OnUpdateSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnFindBestLoop();
	afx_msg void OnUpdateFindBestLoop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDecompressStart(CCmdUI* pCmdUI);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDecompressStart();
	afx_msg void OnInsertSilence();
	afx_msg void OnUpdateInsertSilence(CCmdUI* pCmdUI);
	afx_msg void OnResample();
	afx_msg void OnUpdateResample(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CWaveCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()


// Event maps
	//{{AFX_EVENT(CWaveCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	HRESULT SaveStateForUndo(UINT uUndoTextID);
	
	// IOleInPlaceActiveObject
	BEGIN_INTERFACE_PART(MyOleInPlaceActiveObject, IOleInPlaceActiveObject)
		INIT_INTERFACE_PART(CWaveCtrl, MyOleInPlaceActiveObject)
		STDMETHOD(GetWindow)(HWND*);
		STDMETHOD(ContextSensitiveHelp)(BOOL);
		STDMETHOD(TranslateAccelerator)(LPMSG);
		STDMETHOD(OnFrameWindowActivate)(BOOL);
		STDMETHOD(OnDocWindowActivate)(BOOL);
		STDMETHOD(ResizeBorder)(LPCRECT, LPOLEINPLACEUIWINDOW, BOOL);
		STDMETHOD(EnableModeless)(BOOL);
	END_INTERFACE_PART(MyOleInPlaceActiveObject)

    // IDMUSProdEditor functions
	BEGIN_INTERFACE_PART(Editor, IDMUSProdEditor)
		STDMETHOD(AttachObjects)(IDMUSProdNode*);
		STDMETHOD(OnInitMenuFilePrint)(HMENU, UINT);
		STDMETHOD(OnFilePrint)();
		STDMETHOD(OnInitMenuFilePrintPreview)(HMENU, UINT);
		STDMETHOD(OnFilePrintPreview)();
		STDMETHOD(OnViewProperties)();
		STDMETHOD(OnF1Help)();
	END_INTERFACE_PART(Editor)
	
	DECLARE_INTERFACE_MAP()
	

	enum {
	//{{AFX_DISP_ID(CWaveCtrl)
		// NOTE: ClassWizard will add and remove enumeration elements here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAVECTL_H__BC964EAE_96F7_11D0_89AA_00A0C9054129__INCLUDED)
