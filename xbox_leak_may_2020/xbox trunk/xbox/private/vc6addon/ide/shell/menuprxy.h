#ifndef _menuprxy_h
#define _menuprxy_h

///////////////////////////////////////////////////////////////////////////////
// CMenuPopupProxy
// This class implements the bar proxy on top of a CBMenuPopup.
class CMenuPopupProxy : public CToolBarProxy
{
public:
	// Attach the proxy to a CBMenuPopup.
	void AttachMenu(class CBMenuPopup* pPopup);

// CToolBarProxy methods.

	// Use these to work with the underlying "real" objects.
	virtual CWnd* GetWindow();

	// "Bar" states.
    virtual BOOL IsInDialog();
	virtual UINT GetExpansionConstraint(const CRect &expand, ORIENTATION *orBar=NULL);
	virtual int GetCount() const;
	virtual DWORD GetBarStyle() const;
	virtual BOOL HasButtonExtra() const;
	virtual const CSize GetButtonSize() const;
	virtual const CSize GetImageSize() const;
	virtual HBITMAP GetImageWell(int nIndex);
	virtual int GetImageIndex(int nIndex) const;
	// TRUE if this bar is using large buttons
	virtual BOOL HasLargeButtons() const;

	// Interact with items states.
	virtual UINT GetButtonStyle(int nIndex) const;
	virtual BOOL IsStaticItem(int nIndex);
	virtual BOOL IsGapItem(int nIndex);
	virtual const CString& GetButtonText(int nIndex) const;
	virtual HWND GetButtonControl(int nIndex) const;
	virtual HBITMAP GetButtonImage(int nIndex) const;
	virtual void SetButtonControl(int nIndex, HWND hControl);
	virtual void GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const;
	virtual void GetButtonInfoEx(int nIndex, HWND& hControl, CString& rText, HBITMAP& hImage) const;
	virtual int GetButtonCmdID(int nIndex);
	virtual BOOL HasExInfo(int nIndex) const;
	// call this when the button has been modified to mark the bar dirty and possibly redraw (if bSized==TRUE)
	virtual void ButtonModified(int nIndex, BOOL bSized);
	// Removes a separator before a button
	virtual void RemoveSeparator(int iButton);
	// Inserts a separator before a button
	virtual void InsertSeparator(int iButton);
	// returns true if the item is a control, and is currently visible
	virtual BOOL IsVisibleControl(int iButton) { return FALSE; };
	// returns if the specified button can be shown with text
	virtual APPEARANCEOPTION CanText(int iButton);
	// returns if the specified button can be shown with a glyph
	virtual APPEARANCEOPTION CanGlyph(int iButton);
	// returns any limitations on the appearance of a given button style 
	virtual APPEARANCEOPTION CanText(UINT nStyle);
	// returns any limitations on the appearance of a given button style
	virtual APPEARANCEOPTION CanGlyph(UINT nStyle);
	// returns the appearance of a button
	virtual APPEARANCE GetAppearance(int iButton);
	// returns the default appearance of a button
	virtual APPEARANCE GetAppearance();
	// returns the appearance of a button of a given type when placed in this kind of container. If these display components
	// are not available, the custom button dialog will appear
	virtual APPEARANCE GetInitialAppearance(UINT nStyle);
	// allocates or returns an individual glyph bitmap for a given button
	virtual HBITMAP CreateSingleBitmap(int iButton);
	// always call this function if the one above returns non-null. Do not delete hbmSingle yourself, since it
	// may still be in use elsewhere
	virtual void DestroySingleBitmap(int iButton, HBITMAP hbmSingle);

	// Operations
	virtual int InsertButton(int nIndex, TRANSFER_TBBUTTON& trans, BOOL bReplace, GAP gap);
	virtual void DeleteButton(int nIndex, BOOL bMoving = FALSE);
    virtual void RecalcLayout(UINT nHTSize = HTRIGHT);
	virtual void GetItemRect(int nIndex, LPRECT lpRect) const;
	virtual int IndexFromPoint(const CPoint& pt, CRect *pButtonRect) const;
	virtual int HitTest(CPoint point) const;
	virtual CToolBarProxy* NewBar(UINT nID, const CPoint& rect);
	virtual void GetButtonTransferInfo(int nIndex, DWORD& dwData, TRANSDATA_CALLBACK* lplpDataCallback) const;
	virtual BOOL CanChooseAppearance();
	virtual BOOL CanPasteImage();
	virtual BOOL CanResetImage();
	virtual void PasteButtonImage();
	virtual void ResetButtonImage();
	virtual ORIENTATION GetOrient() const;
	// adds text to the button
	void AddText(int nIndex); // default menu text
	void AddText(int nIndex, const CString &text, BOOL bCustom);
	// add or replace the bitmap of a button.
	void AddBitmap(int nIndex, BITMAPSOURCE bsSource=cBitmapSourcePackage, HBITMAP hbmCustom=NULL);
	// remove the label from a glyph+label button
	void RemoveText(int nIndex) { ASSERT(FALSE); };
	// remove the glyph from a glyph+label button
	void RemoveBitmap(int nIndex);
	// remove all items from the bar
	virtual void Empty(void);
	virtual BOOL ExpandDestination(int nCmdIDSrc, int nIndexDest);
	// returns the next visible item after the specified one. nIndex==-1 gets the first visible item
	virtual int FindNextVisibleItem(int nIndex, BOOL bForward);

	virtual void BatchBegin(int nCount=0);
	virtual void BatchAdd(int nIndex, int nID, UINT nStyle, int nExtra, HBITMAP hGlyph, const CString &name);
	virtual void BatchEnd();

	// Notifications
	virtual void OnSelChange(int nIndex);
	virtual void OnDeleteSelection();
	virtual void OnBarActivate();
	virtual void OnButtonSize();
	virtual void OnButtonMove(int nIndex);

protected:
	virtual void* _GetRealObject();

private:
	class CBMenuPopup* m_pPopup;
	CMapPtrToPtr m_mapBtnExtra;
};

#endif
