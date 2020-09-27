#ifndef btnextra_h
#define btnextra_h

struct TBBUTTON_EXTRA
{
	TBBUTTON_EXTRA() : hwndControl(NULL), hbmGlyph(NULL)
		{ }
#ifdef _DEBUG
	void AssertValid() const;
#else
	inline void AssertValid() const { };
#endif

	inline const HWND GetControl(void) const { AssertValid(); return hwndControl; };
	inline const CString& GetLabel(void) const { AssertValid(); return strLabel; };
	inline const HBITMAP GetGlyph(void) const { AssertValid(); return hbmGlyph; };
	inline BOOL HasLabel() { return !strLabel.IsEmpty(); }
	inline CSize GetLabelSize(void) const { AssertValid(); return nLabelSize; };

	inline HWND GetControl(void) { AssertValid(); return hwndControl; };
	inline CString& GetLabel(void) { AssertValid(); return strLabel; };
	inline HBITMAP GetGlyph(void) { AssertValid(); return hbmGlyph; };
	inline CSize GetLabelSize(void) { AssertValid(); return nLabelSize; };

	inline BOOL IsUsed(void) const { AssertValid(); return hwndControl!=NULL || !strLabel.IsEmpty() || hbmGlyph!=NULL; };

	inline void SetControl(HWND theControl) { AssertValid(); hwndControl=theControl; };
	inline void SetLabel(const CString& theLabel) { AssertValid(); strLabel = theLabel; nLabelSize=0; };
	inline void SetGlyph(HBITMAP theGlyph) { AssertValid(); hbmGlyph=theGlyph; };
	inline void SetLabelSize(CSize theLabelSize) { AssertValid(); nLabelSize = theLabelSize; };

	// we don't use a constructor in this class, as we expect to be able to manipulate it like
	// an ordinary C data structure (so it can be copied using memcpy, for example). And we are
	// going to use a standard memory array for efficiency purposes, rather than a CObArray, so 
	// all manipulation functions must be simple
	void Create(HWND theControl=NULL, CString* theLabel=NULL, HBITMAP theCustom=NULL);
	void Copy(TBBUTTON_EXTRA *src);
	// This deletes any items contained within the extra data. Only use if you're happy
	// for the extra data to completely go away.
	void Destroy();

private:
	HWND hwndControl; // the control used for window buttons (TBBS_HWND)
	CString strLabel; // the text used for menu and textual buttons
	HBITMAP hbmGlyph; // If this glyph is present, then it is the glyph that should be saved
					  // to disk, and is of the other size than the glyph currently in the well
	CSize nLabelSize; // this contains the pixel size of the label. It is updated by internal CCustomBar functions
					  // when they measure the string for the first time. If this value is positive, it is the 
					  // size of a label within a horizontal button. If it is negative, it is the negation of the 
					  // size within a vertical button
};

#endif
