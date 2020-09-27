///////////////////////////////////////////////////////////////////////////////
//      BARCUST.H
//      Contains declarations for the major classes relavant to Sushi
//              toolbars CToolCustomizer, CASBar, CBorderBar, CToolbarDialog, and
//              CCustomDialog.
///////////////////////////////////////////////////////////////////////////////

#ifndef __BARCUST_H__
#define __BARCUST_H__

#include "transbut.h"
#include "custprxy.h"

//	class CWnd;
//		class CToolBar;
			class CCustomBar;
struct TRANSFER_TBBUTTON;
struct TBBUTTON_EXTRA;

class CDockManager;

/////////////////////////////////////////////////////////////////////////////
// CCustomBar window
// Toolbar button overview
// All toolbar buttons have a glyph, except separators and textual buttons. Hwnd buttons are
// only drawn as HWNDs when horizontal; when vertical, they pay attention to styles such as
// TEXTUAL, CUSTOMBITMAP, etc.

// The master (saved) copy of each custom bitmap is considered to be the one the user last 
// *edited*. This is independent of which one the user last displayed. When the user is 
// displaying the other button size (not the size of the edited bitmap), then the extra
// button info contains the master. Otherwise, the master is within the image well.

// martynl 15Apr96

// This struct contains the extra information that can be required by each button
// This class must contain no virtuality, and no base class

// Note that the array of these structures won't be allocated until the first one is required.
// This means that you should only use GetXXX() functions if you are sure the data will already
// be there. If the presence of data is in question (for example with a custom bitmap button
// where you have not yet established the presence of a second bitmap), then you should
// use CCustomBar::HasButtonExtra() first to check if the extra array is present.


extern void GetDroppedRect(HWND hwnd, LPRECT rect);

// These two styles extend the CBRS_ styles in MFC CControlBar; We should ensure 
// that the value of this constant is distinct from all of MFC's CBRS_ styles. 
// Currently, the highest CBRS_ style is 0x8000.

// This style is an augmentive style to CControlBar::GetBarStyle(). It inhibits the
// control bar's dragging functionality for moving or copying buttons. We need this
// for the custom button dialog.  martynl 27Mar96
#define CTBRS_NODRAG    0x10000

// This style is an augmentive style to CControlBar::GetBarStyle(). It tells a command
// bar that it should behave like, and draw like, a main menu. Menus on the main menu
// are drawn without dropdown arrows. Also, main menus cannot be deleted, and have the 
// primary keyboard semantics.
// martynl 23Apr96
#define CTBRS_MAINMENU  0x20000

/* The GetItemRect problem

One of the major problems with CCustomBar in 4.x is that it overrides CToolBar::GetItemRect without
modifying its parameter list. This means that GetItemRect doens't know, when it's called, what
orientation the bar is in [information which is required to correcly enumerate button rects].

The solution to GetItemRect's problems appears to be clear - just call GetOrient to determine the correct
orientation of the bar. However, this will not work.

The CCustomBar class incorporates a number of speculative positioning routines. This is code that tries
to decide what shape a bar would (or should) be in a future context. For example, this code is used to 
drag size the bar, and by the docking manager to draw a snapsize outline of the bar during bar dragging.

Because GetItemRect is called from within speculative positioning routines (Fit, GetSize, GetVSize, etc)
it can't call GetOrient, because GetOrient returns the current orientation, not the orientation that is
being speculatively calculated.

This problem was further exacerbated in 4.0 by our desire to keep the CToolBar class roughly as it was when 
we took it from MFC. So we couldn't change the parameters of GetItemRect without spoiling the virtual function
table.

So 4.x solved the problem with a nasty hack in GetItemRect. There was only one type of item in 4.x that was
sized and rendered differently depending on orientation: the window button. (TBBS_HWND). So 4.x measured 
the window for the button and, if the window was too large for the bar size, it would assume the bar was
vertical.

This worked ok in 4.0 (though it led to an odd situation if you resize a sizeable hwnd button to very small, 
and then vertically dock it). 

5.0 adds loads of buttons and toolbar areas which are different in different orientations. So I've solved
the problem by passing an orientation to GetItemRect, and promoting the definition of orientation to the oldbar
code.

To allow older users of the code to maintain the same GetItemRect interface to CToolBar, an inline function
in shlbar.h does a translation. However, this function is overridden privately in CCustomBar, since all
custombar derived callers must specify orientation depending on whether their code can be called speculatively
or not.

martynl 19/06/96

*/

class CCustomBar : public CToolBar
{
	friend class CToolCustomizer;
	friend struct TRANSFER_TBBUTTON;
	friend class CCustomBarProxy;


public:
	// Bar and Index of the currently dropped menu item, or NULL and -1
	// Only one menu can be dropped from all bars at any give time, so this is static
	static CCustomBar *s_pDroppedBar;
	static int s_nDroppedItem;
	// A pointer to the current system menu, if there is one. 
	static CBMenuPopup *s_pSystemMenu;
	// the screen coordinates point where the last MouseMove was processed. We skip if we've processed a move 
	// very close to this before - stabilizes joint keyboard and mouse usage.
	static CPoint s_ptLastMove;
	// TRUE if this CCmdUI update has created a requirement for a recalclayout
	static BOOL s_bRecalcLayoutPending;

	// cancels any dropped menu on any bar
	static void CancelMenu(void);
	// cancels this menu, or any menu that contains it
	static void HideInvalidDestination(UINT nId);
	// implements specific menu cancelling for bars
	void HideInvalidBarDestination(UINT nId);
    
// Construction.        
    CCustomBar();
    virtual ~CCustomBar();
							   
    BOOL Create(DWORD dwStyle, const CRect& rect, CWnd* pParent, UINT nID);

// Operations.
public:
    BOOL IsInDialog();

#ifdef _DEBUG
	virtual void AssertValid() const;
#endif
	
	virtual CToolCustomizer* GetCustomizer() = 0;
	virtual ORIENTATION GetOrient() const;
	virtual BOOL IsStaticItem(int nIndex);

	virtual CCustomBar* NewBar(UINT nID, const CPoint& pt);
	virtual void MoveBar(const CRect& rect);
	virtual void OnSelChange(int nIndex);
	virtual void OnDeleteSelection();
	virtual void OnBarActivate();
	virtual void OnButtonSize();

	virtual void GetButtonTransferInfo(int nIndex, DWORD& dwData,
		TRANSDATA_CALLBACK* lplpDataCallback) const {;}
		// Override to add data to the TRANSFER_BUTTON struct.

	// This function gets the extra button data, but can return NULL in release versions, and 
	// ASSERT in debug versions. Use it to access data that you expect to already exist. 
	// If you don't know if the button extra info exists, use HasButtonExtra()
	// Get functions only.
	inline const TBBUTTON_EXTRA *GetButtonExtra(int iButton) const { ASSERT(m_pButtonExtra!=NULL); return &(m_pButtonExtra[iButton]); };
	// use to check if extra info is available
	inline BOOL HasButtonExtra(void) const { return (m_pButtonExtra!=NULL); };
	// This function gets the extra button data, and cannot return NULL, because it will
	// allocate the button data if necessary.
	TBBUTTON_EXTRA *EditButtonExtra(int iButton);

	virtual HWND GetControl(UINT nID);
	virtual void RemoveControl(HWND hwndRemove);
	virtual void LoseFocus() {;}    // Override for hwnd controls.

    virtual CSize GetHSize(ORIENTATION or);
    virtual CSize GetVSize();
	virtual int GetVWidth() const;
    virtual CSize GetSize(UINT nHitTest, CSize size);
    virtual void RecalcLayout(UINT nHTSize = HTRIGHT, ORIENTATION *pOr=NULL);
    virtual void RecalcSizes();

	// sets up default button sizes for small or large toolbars
	// does not initiate any redraw 
	void SetDefaultSizes(BOOL bLarge);
	// returns the size of a standard toolbar bitmap
	static CSize GetDefaultBitmapSize(BOOL bLarge);
	// returns the size of a standard toolbar button
	static CSize GetDefaultButtonSize(BOOL bLarge);

	// Warning: Non-virtual overrides.
	BOOL LoadBitmap(LPCSTR lpszResourceName, BOOL bUpdateTiles = TRUE);
	BOOL LoadBitmap(UINT nIDResource, BOOL bUpdateTiles = TRUE);
	void SetBitmap(HBITMAP hbmImageWell, BOOL bUpdateTiles = TRUE);
	// lpIDArray can be NULL to allocate empty buttons
	BOOL SetButtons(const UINT FAR* lpIDArray, int nIDCount);
    
	CSize Fit(int nWidth, ORIENTATION or);
	int IndexFromPoint(const CPoint& pt, CRect *pButtonRect=NULL);
	BOOL IsGap(int nIndex);
	BOOL IsValidCtrlSize(CWnd* pWnd);
	virtual void GetItemRect(int nIndex, LPRECT lpRect, ORIENTATION or) const;
	void EnumItemRects(int nIndex, CRect& rect, BOOL& bWrapped, ORIENTATION or,
		LPCRECT lpRect = NULL) const;

	BOOL ShiftImage(int nIndex, int nTiles,
		TRANSFER_TBBUTTON* pTrans = NULL);
	BOOL BltCommandBitmap(HBITMAP, int nGlyph,
		HDC hdcDest, int nDest, HDC hdcSrc);
	virtual void DeleteButton(int nIndex, BOOL bMoving = FALSE);
	virtual int InsertButton(int nIndex, TRANSFER_TBBUTTON& trans,
		BOOL bReplace, GAP gap);
	// add this command id button to the end of the bar. Returns the insertion index
	virtual int AppendButton(UINT nId);

	// a button must always have at least one of a label or a glyph (unless it's a separator).
	// So to convert a button, first add the new properties, then remove the old if required

	// add or replace the label of a button
	void AddText(int nIndex); // default menu text
	void AddText(int nIndex, const CString &text, BOOL bCustom);
	// add or replace the bitmap of a button.
	void AddBitmap(int nIndex, BITMAPSOURCE bsSource=cBitmapSourcePackage, HBITMAP hbmCustom=NULL);
	// remove the label from a glyph+label button
	void RemoveText(int nIndex);
	// remove the glyph from a glyph+label button
	void RemoveBitmap(int nIndex);

	// returns TRUE if this bar could paste the current clipboard
	BOOL CanPasteImage(void);
	// returns TRUE if the selected button could be reset
	BOOL CanResetImage(void);
	// returns true if the choose button dialog could be applied to the selected button
	BOOL CanChooseAppearance(void);
	// call to invoke choose/edit/paste/reset button dialog on the selected button
	void ChooseButtonAppearance(void);
	void EditButtonImage(void);
	void PasteButtonImage(void);
	void ResetButtonImage(void);
	// called to update the enable status of a button
	virtual void UpdateButton(int nIndex);
	CToolBarProxy* GetProxy()
		{ return &m_Proxy; }
	// Returns TRUE if the item is expecting to regain the focus to the bar
	virtual int ExpandItem(int nIndex, BOOL bExpand, BOOL bKeyboard=FALSE);
	// menu calls this when it is completed, to stop the bar tracking
	virtual void MenuDone();
	virtual void AdjustMenuBar();
	virtual CBMenuPopup *GetItemMenu(int iButton);
	virtual void UpdateMDIIcon();
	// Does this bar contain a button with this command id
	BOOL FindButton(UINT nId) const;
	void EnsureMDIControls(void);
	int FindNextVisibleItem(int nIndex, BOOL bForward);
	// update the text of a button
	virtual void UpdateText(int nIndex, LPCTSTR newText);
	// Returns the HT code to use to resize the bar when expanding it. This function decides whether the bar
	// wants to be stretched vertically or horizontally, given the rect position that the expansion is occurring
	// If bUseOrient is TRUE, then orBar must point at a valid incoming orientation
	UINT GetExpansionConstraint(const CRect &expand, ORIENTATION *orBar=NULL, BOOL bUseOrient=FALSE);

protected:
	// removes any windows, custom bitmap data or strings from the bar extra data
	void RemoveExtra();
	// removes any windows
	void RemoveWindows();
	void Empty();
	// always use this function to access the font for drawing textual toolbar buttons
	// it will create the font as required. martynl 21Mar96
	CFont *GetTextFont(ORIENTATION or);
	// removes a string from the extra information
	void RemoveTextLabel(int nIndex);
	// causes button bitmaps to be reloaded from packages or custom bitmaps
	void RefreshButtonBitmaps();
	// causes a button bitmap to be reloaded from package or custom bitmap; copies
	// the bitmap for pTBB into hdcdest
	void RefreshButtonBitmap(HDC hdcDest, HDC hdcSrc, int nIndex);
	// gets the bitmaps for menu arrows
	HBITMAP GetMenuArrow(ORIENTATION or);
	HBITMAP GetDisabledMenuArrow(ORIENTATION or);
	// helper: measure the size of a string button if drawn into the bar
	void MeasureString(int nIndex, CSize &, ORIENTATION or);
	// Removes a separator before a button
	void RemoveSeparator(int iButton);
	// Inserts a separator before a button
	void InsertSeparator(int iButton);
	// call when a button has been modified, to update internal status, mark dirty, recalc
	void ButtonModified(int nIndex, BOOL bSized);
	// Over time we should move to using functions to interpret button flags, rather than testing
	// the flags directly
	// TRUE if the item can popup in keyboard focus
	BOOL CanFocus(int iButton);
	// TRUE if it's an hwnd button whose hwnd is visible
	BOOL IsVisibleControl(int iButton) const;
	// TRUE if this bar is the current host of the MDI gadgets
	BOOL IsMDIHost(void) const;
	// draws the small document icon in the top/left corner
	BOOL DrawMDIDocumentIcon(CDC* pDC, const CRect &rectIcon);
	// draws the MDI controls on a menu bar (doc icon, & min restore close)
	void DrawMDIControls(CDC* pDC);
	// tests and acts on 
	BOOL CheckMDIButtons(UINT nFlags, CPoint point);

	enum MDIBUTTON {
		cMDINotDrawn=-3,	// MDI buttons aren't even drawn
		cMDINoButton=-2,	// MDI buttons are drawn but inactive
		cMDIDragging=-1,	// Anything higher than this means a drag is happening
		cMDIDragPending=-1, // this is used when a drag is in progress, but the mouse isn't over a button
		cMDIIcon=0,
		cMDIRectFirst=0,
		cMDIMinimize=1,
		cMDIRestore=2,
		cMDIClose=3,
		cMDIRectLast=3,
		cMDIRectCount=4
	};
	
	// gets the rects of the mdi buttons
	void GetMDIButtonRects(CRect rects[cMDIRectCount]);
	// checks if an MDI button was hit
	MDIBUTTON HitTestMDI(CPoint point);

	int CountTiles();

	enum GLYPHSTATUS {
		cGlyphsUnknown=0,
		cGlyphsAbsent=1,
		cGlyphsPresent=2
	};

	// returns whether the bar contains any glyphs at all, for drawing purposes (only the main menu bar might say no)
	BOOL HasGlyphs(void) const;
	// sets the glyphs status when it is known
	void SetGlyphStatus(GLYPHSTATUS gs) { m_glyphStatus=gs; };
	// get the size of buttons in the bar
	CSize GetButtonSize(ORIENTATION or) const;
	
	virtual void DoPaint(CDC* pDC);
	virtual int HitTest(CPoint point, BOOL bCheckWindow=TRUE) const;
	int SizeTest(CPoint point);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual BOOL OnButtonPressed(int iButton);
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual BOOL ShouldExtrude(int iButton);

    //{{AFX_MSG(CASBar)
	afx_msg void OnKillFocus(CWnd *);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
    afx_msg LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
    
	DECLARE_DYNAMIC(CCustomBar);
    DECLARE_MESSAGE_MAP()

	int GetLeftBorder(ORIENTATION or) const;
	int GetTopBorder(ORIENTATION or) const;
	int GetRightBorder(ORIENTATION or) const;
	int GetBottomBorder(ORIENTATION or) const;
	int GetLeftMDIArea(ORIENTATION or) const;
	int GetTopMDIArea(ORIENTATION or) const;
	int GetRightMDIArea(ORIENTATION or) const;
	int GetBottomMDIArea(ORIENTATION or) const;
	void DrawGrabber(CDC* pDC);



// Attributes.
protected:
	int m_cxRightBorder;
	int m_cyDefaultGap;
	CSize m_sizeMaxLayout;
	CSize m_sizeFixedLayout;
	int m_nTiles;
	TBBUTTON_EXTRA *m_pButtonExtra;     // may be NULL
	CCustomBarProxy m_Proxy;
    static HCURSOR s_hcurVSize;
	// it's an enum so that we don't allocate storage
	enum {
		cExtraTextBorder=3, // extra width added around string contained in
							// a textual button, for cosmetic purposes
		cGrabberWidth=8,    // when on left
		cGrabberHeight=8    // when at top
	};

	// stuff used by main menu bar; could go in a subclass
	HICON m_hIconMDI;
	// the mdi drawing/tracking status of the bar
	int m_nMDITrack;
	// knows whether the bar has glyphs at the moment, or whether we need to recalc the fact
	mutable GLYPHSTATUS m_glyphStatus; 

private:
	// Do not use this. See comment at top of class.
	using CToolBar::GetItemRect;
};

extern UINT DSM_BARSELECTCHANGING;
// This message will be sent whenever the selected toolbar button has changed during
// toolbar customisation
// WPARAM: nIndex - index of newly selected button
// LPARAM: pBar - pointer to bar containing newly selected button

extern UINT DSM_DROPMENU;
// WPARAM: index of item to drop (prevalidated)
// LPARAM: keyboard flag

inline BOOL CCustomBar::IsInDialog()
{       ASSERT(GetParent() != NULL);
	return GetParent()->IsKindOf(RUNTIME_CLASS(CDialog)); }

inline BOOL CCustomBar::LoadBitmap(UINT nIDResource, BOOL bUpdateTiles /*=TRUE*/)
	{ return LoadBitmap(MAKEINTRESOURCE(nIDResource), bUpdateTiles); }

#endif  // __BARCUST_H__
