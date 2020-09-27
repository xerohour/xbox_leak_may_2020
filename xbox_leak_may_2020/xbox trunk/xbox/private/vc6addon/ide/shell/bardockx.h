///////////////////////////////////////////////////////////////////////////////
//      BARDOCKX.H
//      Declarations for the dockable toolbar class CASBar.
//
// This class adds the following functionality to CCustomBar; Dockability, savability,
// some menu bar functionality
///////////////////////////////////////////////////////////////////////////////

#ifndef __BARDOCKX_H__
#define __BARDOCKX_H__

#ifndef __BARCUST_H__
#include "barcust.h"
#endif

//      class CToolBar;
//              class CCustomBar;
					class CASBar;

class CDockManager;

/////////////////////////////////////////////////////////////////////////////
// CASBar window

class CASBar : public CCustomBar
{
public:
	// these steal messages for use by the menu bar
	static BOOL StealMenuMessage(MSG *pMsg);
	static BOOL EarlyStealMenuMessage(MSG *pMsg);

public:
	static CToolCustomizer* s_pCustomizer;
    static CObArray NEAR s_aUsage;
	static CASBar *s_pMenuBar;
	static CWnd *s_pOldFocus;
	
// Construction.        
public:
    CASBar();
    virtual ~CASBar();

	// If these create functions fail, they call delete this;
	BOOL Create(CDockManager* pManager, UINT nID, LPCRECT lpRect = NULL,
		LPCSTR lpszTitle = NULL);
    BOOL Create(CWnd* pParent, CDockManager* pManager, DWORD dwStyle,
				TOOLBARINIT FAR* lptbi, UINT FAR* lpIDArray, HGLOBAL hglobInit,
				BOOL bRegister = FALSE, DOCKINIT FAR* lpdi = NULL);

// Operations.
public:
	virtual CToolCustomizer* GetCustomizer();
	virtual ORIENTATION GetOrient() const;

	virtual CCustomBar* NewBar(UINT nID, const CPoint& pt);
	virtual void MoveBar(const CRect& rect);
	virtual void OnBarActivate();
	virtual void OnButtonSize();
	virtual BOOL KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual HWND GetControl(UINT nID);
	virtual void LoseFocus();
	static void ForceReleaseCapture();

	virtual void DeleteButton(int nIndex, BOOL bMoving = FALSE);
	virtual int InsertButton(int nIndex, TRANSFER_TBBUTTON& trans,
		BOOL bReplace, GAP gap);

	// Used for saving and loading data
    HGLOBAL GetData();
    BOOL SetData(HGLOBAL hglob);

    BOOL DynSetButtons(const UINT FAR* lpIDArray, int nIDCount);

	// used to specify whether to use large or small toolbar buttons
	// returns FALSE on error
	BOOL SetButtonSize(BOOL bLarge);

	// This class always remembers the menu bar pointer, so it can provide it as required
	static CASBar *GetMenuBar(void) {return s_pMenuBar;};
	static void UpdateMenuBar(CFrameWnd* pTarget) { if(s_pMenuBar) s_pMenuBar->OnUpdateCmdUI(pTarget, TRUE);}

	// menu calls this when it is completed, to stop the bar tracking
	virtual void MenuDone();

	// Returns TRUE if the item is expecting to regain the focus to the bar
	virtual int ExpandItem(int nIndex, BOOL bExpand, BOOL bKeyboard=FALSE);

	// Call from outside when a command table entry is going away. Iterates over all bars
	static void RemoveAllCommandReferences(UINT id);
	// used by the above
	BOOL RemoveCommandReferences(UINT id);

	virtual CSize GetSize(UINT nHitTest, CSize size);
	virtual CSize GetHSize(ORIENTATION or);

	// Sets the dirty state of the bar. 
	void SetDirty(BOOL bDirty) { m_bDirty=TRUE; };

	// reset the bar
	void Reset(void);
	
	int m_iKeySelect;                                       // the currently activated keyboarding button

// Attributes.
protected:
	UINT m_nIDWnd;  // the dock ID (MAKEDOCKID) of this toolbar - combination of the package id and the window id
	BOOL m_bDirty;  // Save to reg DB?
										// or -1 if not.
	BOOL m_bDropPending; // if TRUE, then the next time the keyboard navigates to a menu item
					   // it should drop down
    

	HANDLE CASBar::SaveSysColorBitmap(HBITMAP hbmSrc);
	HBITMAP CASBar::LoadSysColorBitmap(LPBITMAPINFOHEADER lpBitmap);
	HANDLE CASBar::CreateLogicalDib(HBITMAP hbm, WORD biBits,HPALETTE hpal);

// Message map functions.
protected:
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnButtonPressed(int iButton);
	virtual void LoseKeyFocus(void);
	virtual void GainKeyFocus(int iFocus=0);
	// This function will return TRUE if the button menu was displayed, and FALSE if not.
	// The button menu is the copy/paste/reset/edit/choose
	// toolbar button popup menu displayed only when a toolbar is being customised.
	// martynl 22Mar96
	BOOL DisplayButtonMenu(UINT nFlags, CPoint point);

    //{{AFX_MSG(CASBar)
	afx_msg void OnKillFocus(CWnd *);
	afx_msg void OnNcPaint();
    afx_msg UINT OnNcHitTest(CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nHitTest, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnSysColorChange();
	afx_msg void OnSysCommand(UINT nID, LONG lParam);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg LRESULT OnGetData(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGetMoveSize(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGetTrackSize(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDkNcLButtonDown(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDropMenu(WPARAM wParam, LPARAM lParam);
    
    DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CASBar)
};

extern UINT DSM_BARSIZECHANGING;
// This message will be sent whenever the toolbar button size has just changed
// WPARAM: 0: small bars
//         1: large bars
// LPARAM: result of _getpid() Only act on this message if your pid is the same as that
//                    passed in this message
// The LPARAM needs to contain a CWinApp pointer because floating toolbars/other are top-level windows,
// so we have to broadcast the DSM_BARSIZECHANGING message to all top-level windows. When the broadcast
// arrives, we validate the pid to avoid size changes emanating from other simultaneously]
// executing devstudio instances. martynl 03May96

#endif  // __BARDOCKX_H__
