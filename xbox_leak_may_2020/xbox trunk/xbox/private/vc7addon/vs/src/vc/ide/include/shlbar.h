/////////////////////////////////////////////////////////////////////////////
//      SHLBAR.H
//              All shell toolbar related classes.  For customizable docking
//              toolbars use the mechanism provided by the Dock API.

#ifndef __SHLBAR_H__
#define __SHLBAR_H__

//#undef AFX_DATA
//#define AFX_DATA AFX_EXT_DATA

/////////////////////////////////////////////////////////////////////////////
//      COldToolBar
//              Toolbar class from MFC 3.0 before they switched to system toolbars.
//              We continue to draw our own toolbars.

// Warning, these structures are currently still shared with the res package toolbars.
#ifndef _AFX_TBBUTTON_DEFINED
struct AFX_TBBUTTON
{
	UINT nID;        // Command ID that this button sends
	UINT nStyle;    // TBBS_ styles
	int iImage;     // index into mondo bitmap of this button's picture
						// or size of this spacer
};
#define _AFX_TBBUTTON_DEFINED
#endif

#ifndef _ORIENTATION_DEFINED
enum ORIENTATION {
	orNil,
	orHorz,
	orVert };
#define _ORIENTATION_DEFINED
#endif

// ORIENTATION has been promoted from CCustomBar to CToolbar, so that GetItemRect can take an orientation
// parameter. This allows GetItemRect to depend on orientation directly, rather than through hacks, as
// previously. For CToolBars, ORIENTATION should always be orHorz

class COldToolBar : public CControlBar
{
	DECLARE_DYNAMIC(COldToolBar)

private:
	using CControlBar::Create;

// Construction
public:
	COldToolBar();
	BOOL Create(CWnd* pParentWnd,
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
			UINT nID = AFX_IDW_TOOLBAR);

	void SetSizes(SIZE sizeButton, SIZE sizeImage);
				// button size should be bigger than image
	void SetHeight(int cyHeight);
				// call after SetSizes, height overrides bitmap size
	void SetDefaultSizes();
				// sets the sizes to their default UISG values
				// does not cause any redrawing
	static CSize GetDefaultBitmapSize();
				// returns the size of a standard toolbar bitmap
	static CSize GetDefaultButtonSize();
				// returns the size of a standard toolbar button
	BOOL LoadBitmap(LPCTSTR lpszResourceName);
	BOOL LoadBitmap(UINT nIDResource);
	BOOL SetButtons(const UINT* lpIDArray, int nIDCount);
				// lpIDArray can be NULL to allocate empty buttons

// Attributes
public: // standard control bar things
	int CommandToIndex(UINT nIDFind) const;
	UINT GetItemID(int nIndex) const;
	// this is provided for compatability with other clients. In subclasses it is overridden privately
	// and never implemented, to avoid problems
	inline void GetItemRect(int nIndex, LPRECT lpRect) const { GetItemRect(nIndex, lpRect, GetOrient()); };
	virtual void GetItemRect(int nIndex, LPRECT lpRect, ORIENTATION or) const;
	virtual ORIENTATION GetOrient() const;
	UINT GetButtonStyle(int nIndex) const;  //REVIEW: doc
	void SetButtonStyle(int nIndex, UINT nStyle);   //REVIEW: doc

public:
	// for changing button info
	void GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const;
	void SetButtonInfo(int nIndex, UINT nID, UINT nStyle, int iImage);

// Implementation
public:
	virtual ~COldToolBar();
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// update the text of a button
	virtual void UpdateText(int nIndex, LPCTSTR newText) {};

	// get the image well
	HBITMAP GetImageWell(void) { return m_hbmImageWell; };

	// cmdui needs to know this
	int GetCaptureIndex(void) { return m_iButtonCapture; };
	// kills extrusion, capture, etc
	void CancelMode(void) { OnCancelMode(); };

protected:
	AFX_TBBUTTON* _GetButtonPtr(int nIndex) const;
	void InvalidateButton(int nIndex);
	virtual void UpdateButton(int nIndex);
	void CreateMask(int iImage, CPoint offset, 
		BOOL bHilite, BOOL bHiliteShadow);

	// for custom drawing
	struct DrawState
	{
		HBITMAP hbmMono;
		HBITMAP hbmMonoOld;
		HBITMAP hbmOldGlyphs;
	};
	BOOL PrepareDrawButton(DrawState& ds);
	BOOL DrawButton(CDC* pDC, int x, int y, int iImage, UINT nStyle, BOOL bOutlineOnly=FALSE, CRect *pImageBounds=NULL);
#ifdef _MAC
	BOOL DrawMonoButton(CDC* pDC, int x, int y, int dx, int dy,
		int iImage, UINT nStyle);
#endif
	void EndDrawButton(DrawState& ds);

protected:
	CSize m_sizeButton;         // size of button
	CSize m_sizeImage;          // size of glyph
	int m_cxSharedBorder;       // shared x border between buttons
	int m_cySharedBorder;       // shared y border between buttons
	HBITMAP m_hbmImageWell;     // glyphs only
	int m_iButtonCapture;       // index of button with capture (-1 => none)
	HRSRC m_hRsrcImageWell;     // handle to loaded resource for image well
	HINSTANCE m_hInstImageWell; // instance handle to load image well from

#ifdef _MAC
	// Macintosh toolbars need per-toolbar DCs in order to
	// work correctly in multiple-monitor environments

	HDC m_hDCGlyphs;            // per-toolbar DC for glyph images
	HDC m_hDCMono;              // per-toolbar DC for mono glyph masks
#else
public:
	static HDC s_hDCGlyphs;     // shared DC for glyph images
	static HDC s_hDCMono;		// shared DC for mono glyph masks
protected:
#endif

	virtual void DoPaint(CDC* pDC);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual int HitTest(CPoint point, BOOL bCheckWindow=TRUE) const;
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	// called when the button has been pressed; the graphic will be drawn in the pressed
	// state. Return TRUE to un-press the button, or FALSE to leave it pressed
	virtual BOOL OnButtonPressed(int iButton);
	// called when the system has detected the potential to apply the extruded style to a button
	// buttons which don't support this style should return FALSE
	virtual BOOL ShouldExtrude(int iButton);

	//{{AFX_MSG(COldToolBar)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

// define CToolBar to COldToolBar for convenience
#define CToolBar COldToolBar

#ifndef _DEBUG
inline AFX_TBBUTTON* CToolBar::_GetButtonPtr(int nIndex) const
{       return ((AFX_TBBUTTON*)m_pData) + nIndex; }
#endif

class COldToolCmdUI : public CCmdUI        // class private to this file !
{
public: // re-implementations only
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetText(LPCTSTR lpszText);
};

#define CToolCmdUI COldToolCmdUI

/////////////////////////////////////////////////////////////////////////////
//      C3dBitmapButton
//              Bitmap button for use in dialog bars.  Correctly returns focus,
//              and uses fewer bitmaps than MFC bitmap buttons.

class C3dBitmapButton : public CBitmapButton
{
public:
	C3dBitmapButton();

	BOOL AutoLoad(UINT nID, CWnd* pParent,
		BOOL bToggle = FALSE, LPCRECT lpRect = NULL);

protected:
	BOOL m_bToggle;

protected:
	BOOL DrawBitmaps(BOOL bToggle, LPCRECT lpRect);
	virtual HBITMAP CreateButtonBitmap(UINT nStyle,  LPCRECT lpRect,
		CBitmap* pbmGlyph = NULL, LPCTSTR lpszText = NULL);

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint);
	afx_msg void OnSysColorChange();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//      C3dToggleBitmapButton

class CToggleBitmapButton: public C3dBitmapButton
{
    DECLARE_MESSAGE_MAP()

protected:
	BOOL    m_fDown;

public:
	CToggleBitmapButton();
	BOOL    IsButtonDown()  { return m_fDown; }
	void    SetChecked(BOOL);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint);
};

/////////////////////////////////////////////////////////////////////////////
//      C3dDialogBar
//              All Dev Studio dialog bars should derive from this class.
//              It can be used in MDI, modeless floating, as well as dockable
//              windows.

enum DOCKPOS;

class C3dDialogBar : public CDialogBar
{
private:
	using CDialogBar::Create;

public:
	C3dDialogBar();

	BOOL Create(CWnd* pParentWnd, LPCTSTR lpszTemplateName,
			UINT nStyle, UINT nID);
	BOOL Create(CWnd* pParentWnd, UINT nIDTemplate,
			UINT nStyle, UINT nID);
			
	BOOL CreateIndirect(const void *lpDialogTemplate, CWnd* pParentWnd,
			UINT nStyle, UINT nID);

	virtual BOOL PreTranslateSysKey(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    //{{AFX_MSG(C3dDialogBar)
    afx_msg UINT OnNcHitTest(CPoint pt);
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(C3dDialogBar)

public:
	void SetDockableID(UINT nID);
	void RecalcBarLayout(DOCKPOS dp, BOOL bForceCenter = FALSE);

	void VertCenterControls(int cySize = 0);

protected:
	UINT m_nDockableID;
};

inline BOOL C3dDialogBar::Create(CWnd* pParentWnd, UINT nIDTemplate,
	UINT nStyle, UINT nID)
{       return Create(pParentWnd, MAKEINTRESOURCE(nIDTemplate), nStyle, nID); }
inline void C3dDialogBar::SetDockableID(UINT nID)
{       m_nDockableID = nID; }

/////////////////////////////////////////////////////////////////////////////
//      Toolbar controls for use in customizable dockable toolbars
//              Controls correctly call DkPreHandleMessage.

// Class declarations
class CDockEdit ;       // Dockable Edit control.
class CDockCombo ;      // Dockable Combo box.
class CToolbarCombo ;   // Combo box used as part of the toolbar.
class CDockComboEdit ;          // Used internally only.


class CDockEdit : public CEdit
{
private:
	using CEdit::Create;

public:
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

class CDockComboEdit : public CWnd
{
	// Construction
public:
	CDockComboEdit(BOOL fDockable = TRUE);

protected:
	virtual WNDPROC* GetSuperWndProcAddr();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	BOOL m_fDockable; // set false to use in a dialog
};

class CDockCombo : public CComboBox
{
private:
	using CComboBox::Create;

// Construction
public:
	CDockCombo(BOOL fDockable = TRUE) {} 

    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) {return FALSE;}

	virtual CWnd* SubclassEdit() {return NULL;}
    
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) {return S_FALSE;}
	virtual void PostNcDestroy() {}
	
protected:
	CWnd* m_pChildEdit;
	BOOL m_fDockable;       // set false to use in a dialog
};

class CToolbarCombo : public CDockCombo 
{
// Generated message map functions
protected:
	//{{AFX_MSG( CToolbarCombo )
	afx_msg UINT OnNcHitTest(CPoint point);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

#include "shlbar.h"

class CToolbarEditCombo;

HWND CreateToolbarEditCombo(CToolbarEditCombo *pCombo, DWORD dwStyle, RECT* lpRect, HWND hwndParent, UINT nID);

class CToolbarEditCombo : public CToolbarCombo
{
private:
	using CToolbarCombo::Create;

// Construction.
public:
	CToolbarEditCombo();
	~CToolbarEditCombo();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pWnd, UINT nID);

	virtual CWnd* SubclassEdit();
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	DECLARE_DYNAMIC(CToolbarEditCombo);

public:
	virtual void UpdateView()=0;
	virtual void OnEditSetFocus(void)=0;
	virtual void OnEditKillFocus(void)=0;
	virtual BOOL OnEditHelp(void)=0;

    DECLARE_MESSAGE_MAP()

// Attributes.
protected:
	UINT m_nId; // Help ID
};

class CToolbarEditComboEdit : public CWnd
{
protected:
	virtual WNDPROC* GetSuperWndProcAddr();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	DECLARE_DYNAMIC(CToolbarEditComboEdit);
};

BOOL DrawButtonGlyph(CDC* pDC,
					 int x, int y,
					 CPoint ptOffset,
					 int iImage,
					 UINT nStyle,
					 const SIZE& sizeButton,
					 const SIZE& sizeImage,
					 HDC hdcGlyphs,
					 HDC hdcMono);

//#undef AFX_DATA
//#define AFX_DATA NEAR

#endif  // !__SHLBAR_H__
