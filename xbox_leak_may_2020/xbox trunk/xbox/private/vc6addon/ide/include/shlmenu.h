/////////////////////////////////////////////////////////////////////////////
//      SHLMENU.H
//

#ifndef __SHLMENU_H__
#define __SHLMENU_H__

#ifndef NO_CMDBARS
#include <afxtempl.h>
#endif

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

struct MTM
{
    UINT id;
	UINT idString;
	struct POPDESC* ppop;
};

#pragma warning(disable:4200)
struct POPDESC
{
	UINT strID;
	UINT cmdID;
	MTM rgmtm [0];
};
#pragma warning(default:4200)

#define POP_NIL									0
#define POP_IDS_NIL								0
#define POP_SEPARATOR							UINT(-1)

#define MENU_CONTEXT_POPUP(name)				&pop##name

// We distinguish two types of menus. Command menus are the proper customisable ones that
// go on the bar. POPUP menus are the context ones
#define BEGIN_POPUP_MENU(name)					static POPDESC pop##name = { 0, 0,
#define BEGIN_POPUP_MENU_(name, id)				static POPDESC pop##name = { id, 0, 
#define BEGIN_COMMAND_MENU_(name, idm)			static POPDESC pop##name = { 0, idm, 
#define END_POPUP_MENU()						POP_NIL, POP_IDS_NIL, NULL };

#define MENU_GROUP(idg)							idg, POP_IDS_NIL, NULL,
#define MENU_POPUP(idg, name)					idg, POP_IDS_NIL, &pop##name,
#define MENU_ITEM(idm)							idm, POP_IDS_NIL, NULL,
#define MENU_ITEM_(idm, ids)					idm, ids, NULL,
#define MENU_SEPARATOR()						POP_SEPARATOR, POP_IDS_NIL, NULL,

// Use MenuFileText to correctly generate path text for use in menus.
// Ampersands in filenames must be doubled.

void MenuFileText(CString& str);

/////////////////////////////////////////////////////////////////////////////
//      CContextPopupMenu
//              Support for context popup menus

void ShowContextPopupMenu(POPDESC* ppop, CPoint pt);

struct CTE;

class CContextPopupMenu : public CMenu
{
public:
	static  CContextPopupMenu* CreateNewSubPopup(CContextPopupMenu* pParent);

public:
	CContextPopupMenu();
	~CContextPopupMenu();

	void Create(POPDESC* ppop = NULL);

	void AddItem(UINT id, LPCTSTR lpszText = NULL);
	void AddSubMenu(CContextPopupMenu*, LPCTSTR lpszText);
	BOOL TrackPopup(UINT flags, int x, int y, CWnd* pOwner);
	BOOL LoadMenu(UINT nID);

	void SetPos(int nPopupPos); // must have been loaded from resource

private:
	int m_idgPrev;
	CObList* m_pListSubMenus;
	BOOL m_bPopup;    // using menu resource
	int m_nPopupPos; // position of current popup submenu in the menu resource
};

#ifndef NO_CMDBARS
// REVIEW(DaleRo): Sure would be nice not to have to expose all this to
//                 package partners.  We should review why this is necessary
//                 and try to support it in some other way.

class CBMenuPopup ;

/////////////////////////////////////////////////////////////////////////////
/*
   Each menu item consists of four fields.
      Button   |  Name  |  Accel    |  Arrow
      (fixed)     (var)    (var)       (fixed)
			  2 Pixels      1 char          2 Pixels 
	 c_iButtonTextGap               e_FieldGap.
			  
   The menuitem and the menu communcate the widths and rects for
   these fields using arrays of values index by these enums.

   e_FieldAll is the index for the rectangle that covers the whole menu item.
   e_FieldGap is the width of the gap in between these fields.

  30 May 96 - The word command bars do not put the accelerators at the end of
  the longest menu item. Like windows. Instead they find the size of the longest
  menu item and then right justify the accelerator onto that.

  To support this style of menu, I added the e_FieldNameAccel field. This field is
  used to determine maximum width of a menu item treating the Name and Accel as
  one unit.

  This field is only used inside of CBMenuPopup. CBMenuItem does not know or care about
  this field.

  To go back to the old way #define _FIELD_ALIGN_ACCEL_.

  _FIELD_ALIGN_ACCEL is NOT defined currently! If you define it change this comment
  or else!.
  
	I added the e_FieldText to hold the 

*/
   const int c_iFieldWidthUnknown = -1 ;
   const int c_iNumFields = 7 ;
   enum FieldTypes { e_FieldAll = 0 ,
		     e_FieldButton = 1,
		     e_FieldName = 2,
		     e_FieldAccel = 3,
		     e_FieldArrow = 4,
		     e_FieldGap = 5, // The Gap is used for returns from GetWidths.
					 e_FieldNameAccel = 6 
		     }  ;


/////////////////////////////////////////////////////////////////////////////
//      CBMenuItem

class CBMenuItem 
{
	friend class CMenuPopupProxy;
public:
	enum MenuItemType 
	{
		MIT_Unknown		= 0,
		MIT_Command		= 1,
		MIT_Separator	= 2,
		MIT_Popup		= 3
	};



	// Contruction
	CBMenuItem() ;
	virtual ~CBMenuItem() ;

	// Creation
	BOOL Create(CBMenuPopup* pMenuWnd, UINT nCmdID, CString strName) ;
	// Creation
	BOOL CreatePopup(CBMenuPopup* pMenuWnd, CBMenuPopup* pMenuPopup, CString strName) ;
	// Creation for a menu item whose menu is in the global menu table
	BOOL CreatePopup(CBMenuPopup* pMenuWnd, UINT nCmdID, CString strName);
	// Creation for a menu item whose type is determined by the global command table
	BOOL CreateDynamic(CBMenuPopup* pMenuWnd, UINT nCmdID, CString strName,	BOOL bShowGlyph, HBITMAP hbmCustom);
	BOOL CreateSeparator() ;

	// Attributes
	void Check(BOOL bChecked); 
	BOOL IsChecked() {return m_bChecked;}
	BOOL IsVisible() {return m_bVisible;};
	void SetVisible(BOOL bVisible) { m_bNeedToCalculateSizes=TRUE; m_bVisible=bVisible; };

	UINT Enable(BOOL bEnable) ;
	BOOL Show(BOOL bShow) ;
	// Updates the text (item + accel, separated by tab), and recalcs menu
	void SetText(LPCTSTR lpszText) ;
	// updates accelerator, and optionally recalcs menu     
	void SetAccelerator(LPCTSTR lpszAccel, BOOL bRecalc=TRUE) ;

   // Operation

   //
   // This is the old IMenuItem Interface
   //
   int GetItemType() {return m_iItemType;}
   CBMenuPopup* Expand(BOOL bExpand) ;
   void Select(BOOL bSelect) ;
   void Draw(CDC* pDC, CRect rectFields[c_iNumFields]); 
   void DrawSeparator(CDC* pdc, const CRect* pRect);
   // determines a possible hit, given a scroll offset
   BOOL HitTest(CPoint pt, int cyOffset);
   UINT Click() ;

   UINT GetCmdID() {return m_nCmdID;}
   CString GetName() {return m_strName;}
   CBMenuPopup* GetPopup() {return m_pMenuPopup ;}

   void Release() ;

   BOOL ModifyItem(	UINT nNewCmdID,
					LPCTSTR lpszNewItem, 
					CBMenuPopup* pNewPopup, 
					int iNewItemType);

   void Sync() ;

   void GetWidths(CDC* pdc, /*out*/ int** pRect) ;
   int GetHeight(CDC* pdc) ;
   
   BOOL GetDefault(void) { return m_bDefault; };
   void SetDefault(BOOL bDefault);

   //
   //
   //
   //Members
   //
   //
   //
   int m_iItemType ;
   CString m_strName ;
   CString m_strAccel ;

   UINT m_nCmdID ;
   CBMenuPopup* m_pMenuWnd;						// Window of the menu that contains this item

   unsigned int m_bSelected : 1;
   unsigned int m_bChecked : 1;
   unsigned int m_bGrayed : 1;
   unsigned int m_bVisible : 1;
   unsigned int m_bCustomGlyph : 1;                         // true if the HBITMAP inside this item is not the default and should be saved
   unsigned int m_bCustomText : 1;							// true if the text inside this item is not the default and should be saved
   unsigned int m_bTransient : 1;                                     // true if the item has been temporarily added
   unsigned int m_bDefault : 1;								// true if the item is drawn in bold (default)
   unsigned int m_bExpanded : 1;
   unsigned int m_bNeedToCalculateSizes : 1;
   unsigned int m_bUpdated : 1;
   unsigned int m_bTextModified : 1;						// TRUE if the text of this item has been modified since it came from the command table

	static CBitmap s_bmpCheckMark;
	static CSize s_sizeCheckMark ;
	static CSize s_sizeCmdGlyph;

	// Popup
	static CBitmap s_bmpArrow ;
	static CSize s_sizeArrow ;

	// removed cached sizes
	static void FlushSizeCache();
	// and recalculate them
	static void EnsureSizeCache();


   // Glyph
   HBITMAP m_hbmCmdGlyph ;
   HBITMAP m_hbmOriginalGlyph;      // contains the original command glyph if the glyph had to be scaled to fit 
								   // the current button size

   // Popup Members
   CBMenuPopup* m_pMenuPopup ;
   CTE *m_pCTE;                                         // we are caching a copy of this here for efficiency reasons

   // Fields
   int m_iHeight ;
   int m_iFieldWidths[c_iNumFields] ; // Caches the widths desired for each field.
   CRect m_rectFields[c_iNumFields] ; // Holds the rects for each field from the last Draw.

   // OnCommandUpdate Menu handlers can do all sorts of nasty things like
   // delete menu items. We need to be able to track such changes to keep the menu
   // code in sink.
   void Updated(BOOL bUpdated)
	{ m_bUpdated = bUpdated; }

   BOOL Updated()
	{ return m_bUpdated ; }

	void UpdateGlyph() ;

	BOOL IsTextModified(void) { return m_bTextModified; };								// TRUE if text was modified since creation/command table update
	void SetTextModified(BOOL bTextModified) { m_bTextModified=bTextModified; };

protected:
	// helper function during creation
	// bCustom is TRUE if the glyph is custom (and a candidate for saving and resetting)
	void SetGlyph(HBITMAP hbmSource, int nIndex, BOOL bMustCopy, BOOL bCustom);

	// Other Helpers
	void CalcSizes(CDC* pdc) ;
	void ParseOutAccelerator(CString strWhole) ;

};


/////////////////////////////////////////////////////////////////////////////
// CBMenuPopup window

class CForwardMouseMessage ;

class CBMenuPopup : public CWnd
{
	friend CBMenuItem ;
	friend class CMenuPopupProxy;
// Construction
public:
	CBMenuPopup();

// Attributes
public:
   static BOOL IsCBMenuCmdUI(CCmdUI* pCmdUI) ;

   enum {
      HT_MISSED = -1,
      HT_HITSELECTEDITEM = -2
   };
// Operations
public:
	// xOffset and yOffset are the extra ammounts to move the menu by if it is to overflow
	// the screen from its current position. This allows menus that are dropping from 
	// command bar buttons to not be placed over their command bar button martynl 05May96
   // Der - TrackDropDownMenu replaces TrackPopupMenu for drop down menus.

	static CTypedPtrList<CObList, CBMenuPopup *> s_listMenus;

	// the screen coordinates point where the last MouseMove was processed. We skip if we've processed a move 
	// very close to this before - stabilizes joint keyboard and mouse usage.
	static CPoint s_ptLastMove;

	static CFont *GetMenuFont(BOOL bDefault);

	UINT TrackDropDownMenu(int x, int y, 
							CWnd* pWnd, 
							LPCRECT lpRect = 0 , 
							CRect rectAvoid=CRect(0,0,0,0),
							CWnd* pWndMenuBar = NULL,			// Pointer to window to a window to forward the mouse/keys.
							BOOL bSelectFirstItem =FALSE,		// Select the first menu item. Required for menu keyboard ui.
							BOOL *pbBarFocus=NULL,				// OUT parameter, explains whether the calling bar should be re-focussed after the menu
							BOOL bReturnCmd=FALSE				// IN param. If true, menu should return command without posting
							);
	
   UINT TrackPopupMenu( UINT nFlags, int x, int y, 
			CWnd* pWnd, LPCRECT lpRect = NULL);
   UINT TrackPopupMenuEx( UINT nFlags, int x, int y, 
			CWnd* pWnd, CRect rectAvoid, LPCRECT lpRect = NULL);
   
   // Internal Function...
   UINT TrackMenu( UINT nFlags, int x, int y, 
		   CWnd* pWnd, LPCRECT lpRect = 0 , CRect rectAvoid=CRect(0,0,0,0), BOOL bSelectFirstItem=FALSE);

   // possibly dubious way to handle the parenting issue with submenus. Temporary solution, hopefully.
   inline void SetParent(CWnd *pParent) { m_pParentWnd=pParent; };
   inline void SetAutoEnable(BOOL bAuto) { m_bAutoEnable=bAuto; };
   inline CWnd *GetParent() { return m_pParentWnd; };

   // DER! Add Reference Counting!
   void Release() {delete this;}

   // Array Access
   CBMenuItem* GetMenuItem(int index) ;
   void AddMenuItem(int index, CBMenuItem* pMenuItem) ;

   //
   // CMenu Simulation Functions
   //
   BOOL CreateMenu() ;
   BOOL CreatePopupMenu(CWnd* pParentWnd) ;
   BOOL DeleteMenu(UINT nPosition, UINT nFlags) ;
   void DeleteAll();
   BOOL AppendMenu(UINT nFlags, UINT nIDNewItem = 0, 
		   LPCTSTR lpszNewItem = NULL,
		   CBMenuPopup* pSubPopup = NULL);       
   int GetMenuString(UINT nIDItem, CString& rString, 
		     UINT nFlags) const;
   CBMenuPopup* GetSubMenu(int nPos) const;
   BOOL InsertMenu(UINT nPosition, UINT nFlgs, 
		   UINT nIDNewItem = 0, 
		   LPCTSTR lpszNewItem = NULL,
		   CBMenuPopup* pSubPopup = NULL, 
		   BOOL bTransient=FALSE);       
   // Note: MergeMenu empties pSection of items
   BOOL MergeMenu(UINT nPosition, UINT nFlags, CBMenuPopup* pSection);
   UINT EnableMenuItem(UINT nIDEnableItem, UINT nEnable) ;
   UINT ShowMenuItem(UINT nIDEnableItem, UINT nShow);

   BOOL ModifyMenu(UINT nPosition, UINT nFlags, 
		   UINT nIDNewItem = 0, LPCTSTR lpszNewItem = NULL,
		   CBMenuPopup* pSubPopup = NULL);       
   void RemoveCommandReferences(UINT id);

   HGLOBAL GetData();
   BOOL SetData(HGLOBAL hglob);

   UINT GetMenuItemCount( ) const { return m_nMaxIndex ; } // Used by CTypedRecentFileList::UpdateMenu

	UINT GetMenuItemID(int nPos) ;
	BOOL ContainsMenu(int nCmdID);

	// Determine if the menu is a dropdown menu or not. This has been used by packages.
	BOOL IsDropDown() {return m_bDropDown;};

   // Used to for cbverb.cpp
   BOOL IsSubPopup() {return m_bSubPopup;} // Returns true if menu is a subpopup.

	// Proxy handlers
	// When it is visible, a menu has a proxy. Or when the proxy is manually allocated
	void AllocateProxy();
	void FreeProxy();
	CMenuPopupProxy* GetProxy();

   inline int GetCount() { return m_nMaxIndex; };

   // this is the 'full height' of the menu
   int GetHeight();
   // this is the height of the window containing the menu
   int GetWindowHeight(void);
   int GetWidth();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBMenuPopup)
	public:
	//}}AFX_VIRTUAL
protected:
	virtual BOOL Create(CWnd* pParentWnd, POINT pt, BOOL bSubPopup = FALSE, CRect* prectAvoid =NULL);
   //virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler) ;
	virtual void PostNcDestroy() ;

   DECLARE_DYNAMIC(CBMenuPopup)

//
// Implementation
//
public:
	void Initialize();
	virtual ~CBMenuPopup();

	//
	// Keybord UI
	//

	// Show Expanded
	// these act immediately
	void HideSubPopup() ;
	void ShowSubPopup(int nItem);
	void ScrollMenu(BOOL bForward, BOOL bEnd);
	// down one item
	void ScrollMenuDown();
	// up one item
	void ScrollMenuUp();
	// to first item at top
	void ScrollMenuTop();
	// to last item at bottom
	void ScrollMenuBottom();
	// make selected menu item scroll into view
	void EnsureSelectionScrolled();

	// these act after a timer fires
	void DeferHideSubPopup() ;
	void DeferShowSubPopup(int nItem);
	void DeferScrollMenuDown(BOOL bInitial);
	void DeferScrollMenuUp(BOOL bInitial);
	void EndDefer(void);
	void EndScrollDefer(void);
	// hittests the scrolling area and defers a scroll if appropriate. If bInitial is true, the delay is longer
	void DoScroll(CPoint pt, BOOL bInitial);

	// This synchronises the selection with the currently expanded item, and recurses upwards
	void SelectExpanded(void);

	// If TRUE is returned, the key should be passed to the menu bar
	BOOL ForwardRightKey() ;
	void ForwardUpKey() ;
	void ForwardDownKey() ;
	void ForwardHomeKey() ;
	void ForwardEndKey() ;
	UINT ForwardReturnKey() ;
	UINT ForwardCharKeyDown(TCHAR keychar, BOOL *bUseCmd);
	void KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

   //
   // Subpopup Navigation
   //

   CBMenuPopup* FindNextToLastChild();
   CBMenuPopup* FindLastChild();

	// This is returned from DoMouseMove. These constants can be added.
	enum {
		cMoveUnhandled=0,               // Message needs to be handled by parent
		cMoveHandled=1,                 // Message Does not need to be handled elsewhere
		cMoveSelectExpanded=2   // Message requires parent to select the expanded item
	};

	int DoMouseMove(UINT nFlags, CPoint pt); // returns a combination of the above flags
	UINT DoLButtonUp(UINT nFlags, CPoint point) ;
	BOOL DoLButtonDown(UINT nFlags, CPoint point) ;

	void InitMenuPopup() ;
	void MainFrameInitMenuPopup() ; // Copied from MainFrame.

	// Modal Support...
	BOOL ContinueModal();
	void DoModal(int x, int y) ;
	void Done(UINT nCmdID = NULL) ;
	void MessageLoop();

	// Find the menu item corresponding to the point.
	int HitTest(CPoint pt) ;
	// Determine if the point is in any menu or the menu bar.
	BOOL PtInMenus(CPoint pt) ;

	// Helpers
	void CalcMenuSize() ;
	void VerifyMenuSize(BOOL bAlways) ; // Checks if toolbar glyphs have changed. 
	int GetMenuIndexForCommand(UINT nCmdId) const;
	void InvalidateItem(int nIndex);
	CRect GetItemRect(int nIndex);
	void GetItemsRect(CRect *pRect);

	// finds the next visible item after the one specified
	int FindNextVisibleItem(int nIndex, BOOL bForward);

	void AdjustMenuSize(CBMenuItem* pItem) ;

	void SelectFirstItem();      // TrackDropDownMenu uses this function to
							// select the first menu item when moving with the keyboard.
	inline BOOL IsDirty() { return m_bDirty; };
	inline BOOL IsPainted() { return m_bPainted; };

	// Number of visible non-separators, or -1 if unknown, -2 during calculation
	int m_nVisibleItems;

   // The menu can be modified in OnCommandUpdate handlers. We need some extra
   // information to determine if we have been changed or not.
   void ChangedByOnUpdate(BOOL bChanged)	// The menu has changed.
	{m_bChanged = bChanged ;}
   
   BOOL ChangedByOnUpdate()						// Has the menu changed?
	{return m_bChanged ; }
   
   BOOL m_bChanged ;							// Variable.

	// returns true if this menu is participating in customization
	BOOL IsCustomizing(void);

	// Use this to hide any submenu which has this id, or indirectly contains this id
	void HideInvalidMenuDestination(UINT nId);

	// returns TRUE if this menu, or one of its submenu, contains the specified command id
	BOOL ContainsMenu(UINT nId);

	// Flushes all cached menu size information
	void FlushSizeCache();

	// Sets whether the menu wants to see its cancellation
	void SetPassBackCancel(BOOL bPassBack) { m_bPassBackCancel=bPassBack; };

	// Call when a click could not be a system double click part 1
	static void ClearSystemClick(void);
	// call to tell a menu about the first part of a system click
	static void SetSystemClick(int nTime, CPoint ptScreen);

	// called when colours have been changed
	static void SysColorChange(void);

//Members
protected:
	static BOOL s_bCreateTransients;		// When true, all menu items created get their m_bTransient flag set

   typedef CTypedPtrArray <CPtrArray, CBMenuItem*> CMenuItems;
   CMenuItems m_MenuItems ;
   int m_nExpandedMenuItem; // index of the item whose subpopup is expanded. Subpopups only
							// contract when the mouse moves over another subpopup item, or when the
							// menu is closed
   int m_nExpandDeferredItem; // item to which deferred expand timer refers
   int m_nSelectedMenuItem; // Index of selected item.
   int m_nMaxIndex ;            // The index of the first empty slot.
   BOOL m_bPainted;				// TRUE if the menu has been painted. Set false by create
   BOOL m_bBarFocus;            // True if focus should be returned to the bar
   BOOL m_bAutoEnable;			// TRUE if the menu item should be enabled even if no command handler can be found
   CSize m_sizeDesired;         // The height of all the items and the width of the widest
   BOOL m_bSelectFirstItem;		// TRUE if the first item of the menu needs to be selected

   CWnd* m_pWndMenuBar;         // The window which acts like a menubar to the popup menu.
							// The popup forwards mouse and keyboard messages to this
							// window. This window is in charge of closing the menu.
   BOOL m_bSubPopup ;
   CWnd* m_pParentWnd ;
   CBMenuPopup* m_pChildPopup ;
   CRect m_rectAvoid;		// Screen coords rect to avoid when placing menu

   // Modal
   BOOL m_bContinueModal ;

   // This is the ID of the currently active timer, or 0 if there isn't one. Used for
   // delaying appearance of subpopups
   int m_nCurrentTimer;

   UINT m_nPostCmdID ; // Command ID to send to m_pParentWnd;
   BOOL m_bReturnCmdID; // If true, TrackXXX returns the command id, rather than posting the message. TPM_RETURNCMD

   BOOL m_bControl;

   // Large Toolbars
   BOOL m_bLargeToolbar ;

   BOOL m_bDirty;				// true if this has been modified since it was saved.

   // Field Widths
   int m_iHeight;
   int m_iFieldWidths[c_iNumFields] ;

   class CMenuPopupProxy* m_pProxy;

	// Drop Down Support
	BOOL m_bDropDown ; // True if TrackDropDownMenu Called False otherwise.

	// The following pointer is used to forward mouse messages on when the user
	// clicks outside of the window.
	CForwardMouseMessage* m_pForwardMsg;
	// TRUE if cancellation in the menu bar should still be forwarded
	BOOL m_bPassBackCancel;

	// Menus now support scrolling. 
	BOOL m_bScrolling;							// TRUE if this menu is required to scroll
	BOOL m_bHasTopScroll;						// TRUE if the top scroll area is present
	BOOL m_bHasBottomScroll;					// TRUE if the bottom scrolling area is present
	int m_nTopIndex;							// index of the top visible menu item, after the scroll area
	int m_cyTopOffset;							// Coordinate offset of items.

	static BOOL s_bLastWasKey;					// TRUE if the last proper input the menu processed was keyboard
	static BOOL s_bSuppressedMouseMove;			// TRUE when a single mouse move is suppressed just after dropping the menu
	static CBitmap s_bmpScrollDown;				// syscolour loaded bitmap of the scroll down arrow
	static CBitmap s_bmpScrollUp;				// syscolour loaded bitmap of the scroll up arrow
	static CSize s_sizeScroll;					// size of the scroll arrows (assumed to be same)

	// System click handling
	// This is the last time the MDI system icon was clicked
	static int s_nLastSystemClick;
	static CPoint s_ptLastSystemClick;

	// Generated message map functions
protected:
	//{{AFX_MSG(CBMenuPopup)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnDestroy( );
    afx_msg LRESULT OnTestMenu(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillFocus(CWnd *);
	//}}AFX_MSG
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

#ifdef _DEBUG
public:
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
//      CBContextPopupMenu
//              Command bar version of CContextPopupMenu

class CBContextPopupMenu : public CBMenuPopup
{
public:
			CBContextPopupMenu();
			~CBContextPopupMenu();

	void    Create(POPDESC* ppop = NULL);

	BOOL AddItem(UINT id, LPCTSTR lpszText = NULL);
	void    AddSubMenu(CBContextPopupMenu*, LPCTSTR lpszText);
	UINT    TrackPopup(UINT flags, int x, int y, CWnd* pOwner );
	UINT    TrackPopup(UINT flags, int x, int y, CWnd* pOwner, CRect rectAvoid );
	//BOOL  LoadMenu(UINT nID);
	//void  SetPos( int nPopupPos ); // must have been loaded from resource
	static  CBContextPopupMenu* CreateNewSubPopup(CBContextPopupMenu* pParent);

private:
	int             m_idgPrev;
	CObList*        m_pListSubMenus;
	BOOL    m_bPopup;    // using menu resource
	int             m_nPopupPos; // position of current popup submenu in the menu resource
};

extern void CBShowContextPopupMenu(POPDESC* ppop, CPoint pt, CWnd* pParent = NULL);

////////////////////////////////////////////////////////////////////////////////////
//
//OLD_MENU Backward compatibility menu mode support
// Grep for OLD_MENU.
// Menu is a namespace.
namespace Menu
{
void UseHMENU(BOOL bUseOldWindowsMenu) ;
BOOL IsInCmdBarMode() ;
BOOL IsShowingCmdBars() ;
void UpdateMode(BOOL bUpdateNow);
void UseOLE(BOOL bStart);
};

#endif // !NO_CMDBARS

#undef AFX_DATA
#define AFX_DATA NEAR

#endif // !__SHLMENU_H__
