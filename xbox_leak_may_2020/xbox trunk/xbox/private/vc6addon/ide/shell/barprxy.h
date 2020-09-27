// barprxy.h
// The abstract base class for menu and toolbar proxies

#ifndef barprxy_h
#define barprxy_h

#define TBB_NIL         UINT(-1)

#define TBBS_HWND 0x04
#define TBBS_SIZABLE 0x08
#define TBBS_ENABLEWND 0x10

// When this flag is set, the button is drawn as a textual string. The string will be
// stored in the button extra data for that button. It will be drawn vertically
// if and only if the bar is in a docked, vertical situation. This follows the 
// conventions of current releases of Office 97. martynl 21Mar96
#define TBBS_TEXTUAL 0x20

// When this flag is set, the button is drawn using a custom bitmap. The bitmap will be 
// stored in the image well as usual. However, a second bitmap may be stored inside the
// TBBUTTON_EXTRA structure. In this case, the extra bitmap is the one that should be
// saved, and is in the other size with respect to the image well. If there is no bitmap
// inside the button extra structure, this implies that the bitmap in the well is the one to
// be saved and loaded.
// martynl 15Apr96
#define TBBS_CUSTOMBITMAP 0x40

// When this flag is set, the button is a menu. When pressed, it drops down the
// menu identified by the command id. Menus can have any kind of button. Mmenus will have a
// drop down arrow beside them, unless they are in the main menu bar.
// martynl 22Apr96
#define TBBS_MENU 0x80

// Paint this button "out," closer to the user.
#ifndef TBBS_EXTRUDED
#define TBBS_EXTRUDED 0x100
#endif

// When this flag is set, a button is drawn with a toolbar glyph. Most buttons will have this
// flag set, with the exception of buttons which are purely textual. TBBS_GLYPH is also set for hwnd
// buttons, unless their vertically docked view is text only. So TBBS_GLYPH can also be used to 
// indicate whether a space for the glyph has been reserved in the well. martynl 07Jun96
#define TBBS_GLYPH 0x200

// This flag causes a button to be invisible, unpressable, etc, etc, except in customisation mode
#define TBBS_INVISIBLE 0x400

// This flag is true when the text in a textual button is custom
#define TBBS_CUSTOMTEXT 0x800

enum GAP {
	gapNil,
	gapBefore,
	gapAfter,
	gapReplace };

enum BITMAPSOURCE
{
	cBitmapSourcePackage=0,    // revert to its default (package-supplied) bitmap
	cBitmapSourceCustom=1,     // use supplied custom glyph
	cBitmapSourceClipboard=2   // copy from clipboard
};

// this is used to describe what appearances a button can take
enum APPEARANCEOPTION
{
	cAppearanceCompulsory=0,
	cAppearanceOptional=1,
	cAppearanceForbidden=2
};

enum APPEARANCE {
	cAppearanceImageOnly=0, // these must stay in sync with the custom button dialog resource
	cAppearanceTextOnly=1,
	cAppearanceImageText=2
};

///////////////////////////////////////////////////////////////////////////////
// CToolBarProxy
// This class is the interface to a custom bar, either a CCustomBar, or a CBMenuPopup.
class CToolBarProxy
{
public:
	static CToolBarProxy* GetBarProxy(CObject* pBar);

// Proxy interface:
public:
	// Use these to work with the underlying "real" objects.
	virtual CWnd* GetWindow() = 0;
	void* GetRealObject()
		{ return this == NULL ? NULL : _GetRealObject(); }

	// "Bar" states.
    virtual BOOL IsInDialog() = 0;
	virtual UINT GetExpansionConstraint(const CRect &expand, ORIENTATION *orBar=NULL) = 0;
	virtual int GetCount() const = 0;
	virtual DWORD GetBarStyle() const = 0;
	virtual const CSize GetButtonSize() const = 0;
	virtual const CSize GetImageSize() const = 0;
	virtual HBITMAP GetImageWell(int nIndex) = 0;
	virtual int GetImageIndex(int nIndex) const = 0;
	// TRUE if this bar is using large buttons
	virtual BOOL HasLargeButtons() const=0;

	// Interact with items states.
	virtual UINT GetButtonStyle(int nIndex) const = 0;
	virtual BOOL IsStaticItem(int nIndex) = 0;
	virtual BOOL IsGapItem(int nIndex) = 0;
	virtual const CString& GetButtonText(int nIndex) const = 0;
	virtual HWND GetButtonControl(int nIndex) const = 0;
	virtual HBITMAP GetButtonImage(int nIndex) const = 0;
	virtual int GetButtonCmdID(int nIndex) = 0;
	virtual void SetButtonControl(int nIndex, HWND hControl) = 0;
	virtual void GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const = 0;
	virtual void GetButtonInfoEx(int nIndex, HWND& hControl, CString& rText, HBITMAP& hImage) const = 0;
	virtual BOOL HasExInfo(int nIndex) const = 0;
	// call this when the button has been modified to mark the bar dirty and possibly redraw (if bSized==TRUE)
	virtual void ButtonModified(int nIndex, BOOL bSized)=0;
	// Removes a separator before a button
	virtual void RemoveSeparator(int iButton)=0;
	// Inserts a separator before a button
	virtual void InsertSeparator(int iButton)=0;
	// returns true if the item is a control, and is currently visible
	virtual BOOL IsVisibleControl(int iButton)=0;
	// returns if the specified button can be shown with text
	virtual APPEARANCEOPTION CanText(int iButton)=0;
	// returns if the specified button can be shown with a glyph
	virtual APPEARANCEOPTION CanGlyph(int iButton)=0;
	// returns any limitations on the appearance of a given button style 
	virtual APPEARANCEOPTION CanText(UINT nStyle)=0;
	// returns any limitations on the appearance of a given button style
	virtual APPEARANCEOPTION CanGlyph(UINT nStyle)=0;
	// returns the appearance of a button
	virtual APPEARANCE GetAppearance(int iButton)=0;
	// returns the default appearance of a button
	virtual APPEARANCE GetAppearance()=0;
	// returns the appearance of a button of a given type when placed in this kind of container. If these display components
	// are not available, the custom button dialog will appear
	virtual APPEARANCE GetInitialAppearance(UINT nStyle)=0;
	// allocates or returns an individual glyph bitmap for a given button
	virtual HBITMAP CreateSingleBitmap(int iButton)=0;
	// always call this function if the one above returns non-null. Do not delete hbmSingle yourself, since it
	// may still be in use elsewhere
	virtual void DestroySingleBitmap(int iButton, HBITMAP hbmSingle)=0;
	
	// Operations
	virtual int InsertButton(int nIndex, TRANSFER_TBBUTTON& trans, BOOL bReplace, GAP gap) = 0;
	virtual void DeleteButton(int nIndex, BOOL bMoving = FALSE) = 0;
    virtual void RecalcLayout(UINT nHTSize = HTRIGHT) = 0;
	virtual void GetItemRect(int nIndex, LPRECT lpRect) const = 0;
	// For a given point, returns the insert-after button for drag purposes.
	virtual int IndexFromPoint(const CPoint& pt, CRect *pButtonRect=NULL) const = 0;
	// Returns the index of the button hit, or -1 if none was hit. Does not return the HT_ codes used in cbmenu
	virtual int HitTest(CPoint point) const=0;
	virtual CToolBarProxy* NewBar(UINT nID, const CPoint& pt) = 0;
	virtual void GetButtonTransferInfo(int nIndex, DWORD& dwData, TRANSDATA_CALLBACK* lplpDataCallback) const = 0;
	virtual BOOL CanChooseAppearance() = 0;
	virtual BOOL CanPasteImage() = 0;
	virtual BOOL CanResetImage() = 0;
	virtual void PasteButtonImage() = 0;
	virtual void ResetButtonImage() = 0;
	virtual ORIENTATION GetOrient() const = 0;
	// adds text to the button
	virtual void AddText(int nIndex)=0; // default menu text
	virtual void AddText(int nIndex, const CString &text, BOOL bCustom)=0;
	// add or replace the bitmap of a button.
	virtual void AddBitmap(int nIndex, BITMAPSOURCE bsSource=cBitmapSourcePackage, HBITMAP hbmCustom=NULL)=0;
	// remove the label from a glyph+label button
	virtual void RemoveText(int nIndex)=0;
	// remove the glyph from a glyph+label button
	virtual void RemoveBitmap(int nIndex)=0;
	// remove all items from the bar
	virtual void Empty(void)=0;
	// expand a bar item if the being dragged command item is capable of being inserted into it
	virtual BOOL ExpandDestination(int nCmdIDSrc, int nIndexDest)=0;
	// returns the next visible item after the specified one. nIndex==-1 gets the first visible item
	virtual int FindNextVisibleItem(int nIndex, BOOL bForward)=0;

	// Batching operations for use during load and similar broad scope additive operations
	virtual void BatchBegin(int nCount=0)=0;
	virtual void BatchAdd(int nIndex, int nID, UINT nStyle, int nExtra, HBITMAP hGlyph, const CString &name)=0;
	virtual void BatchEnd()=0;

	// Notifications
	virtual void OnSelChange(int nIndex) = 0;
	virtual void OnDeleteSelection() = 0;
	virtual void OnBarActivate() = 0;
	virtual void OnButtonSize() = 0;
	virtual void OnButtonMove(int nIndex) = 0;

	// functions for reading and writing menus and toolbars
	BOOL SetData(HGLOBAL hglob);
	HGLOBAL GetData();
protected:
	virtual void* _GetRealObject() = 0;
};

#endif
