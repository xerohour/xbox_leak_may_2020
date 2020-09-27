// File: acapi.h
// This file defines the interfaces used by auto complete

#ifndef __ACAPI_H__
#define __ACAPI_H__

__interface IVsTextBuffer;
__interface IVsCompletionSet;
__interface IVsTipWindow;
__interface IVsMethodData;

//ignore this, it's just so things will compile
typedef enum _TextSelMode
{
    SM_STREAM,
    SM_BOX
} TextSelMode;

#define MAX_VISIBLE 10

////////////////////////////////////////////////////////////////////////////
// CompletionStatusFlags enum
////////////////////////////////////////////////////////////////////////////
typedef enum _CompletionStatusFlags
{
    // UCS_* flags are passed to IVsTextView::UpdateCompletionStatus
    UCS_NAMESCHANGED        = 0x0001,       // Names or name count has changed
    UCS_EXTENTCHANGED       = 0x0002,       // Extent changed
    UCS_FLAGSCHANGED        = 0x0004,       // Flags have changed...
	UCS_COMPLETEWORD		= 0x0008,		// Attempt word completion

    // CSF_* flags are returned from ICompletionSet::GetFlags
    CSF_CUSTOMMATCHING      = 0x0004,       // Use ICompletionSet::GetBestMatch() to manage selection
    CSF_CUSTOMCOMMIT        = 0x0008,       // Use ICompletionSet::OnCommit() to commit completions
    CSF_HAVEDESCRIPTIONS    = 0x0010,       // ICompletionSet::GetDescriptionText will provide text for at least some of the entries
    CSF_INITIALEXTENTKNOWN  = 0x0020,        // Use ICompletionSet::GetInitialExtent to determine initial word extent
	CSF_NOCASEMATCHING      = 0x0040,
	CSF_NOCASEUNIQUENESS    = 0x0080,

    // GBM_* flags are returned from IVsCompletionSet::GetBestMatch
    GBM_SELECT              = 0x0001,       // The returned index should be selected
    GBM_UNIQUE              = 0x0002        // The returned index is the only appropriate match

} UpdateCompletionFlags;

/////////////////////////////////////////////////////////////////////////
// TipWindowFlags enum
/////////////////////////////////////////////////////////////////////////
typedef enum _TipWindowFlags
{
    UTW_CONTEXTCHANGED      = 0x0001,       // Context stream is different
    UTW_CONTENTCHANGED      = 0x0002,       // Data displayed in window has changed (size will be re-queried)
    UTW_DISMISS             = 0x0004,       // Tip window should be dismissed
	UTW_ANGLEBRACKET		= 0x0008		// angle bracket
} TipWindowFlags;

////////////////////////////////////////////////////////////////////////////
// TipPosPreference enum
////////////////////////////////////////////////////////////////////////////
typedef enum _TipPosPreference
{
    TPP_ABOVE,
    TPP_BELOW,
    TPP_LEFT,
    TPP_RIGHT,
    TPP_DOCKED      // Current NYI -- defaults to TPP_ABOVE
} TipPosPreference;

////////////////////////////////////////////////////////////////////////////
// TIPSIZEDATA structure
////////////////////////////////////////////////////////////////////////////
typedef struct _tipsizedata
{
    SIZE                size;
    TipPosPreference    dwPosition;
} TIPSIZEDATA;


#undef INTERFACE
#define INTERFACE	IVsTextView

//This is the view interface used in the new shell.  By implementing a small portion of it
//in the old shell, we can allow the auto complete back end to use the same view interface
//in both the old and new shells, thus minimizing the work needed to move it forward into
//the new shell. 
DECLARE_INTERFACE_(IVsTextView, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

    //IVsTextView methods
    STDMETHOD       (Initialize)(IVsTextBuffer *pBuffer, HWND hwndParent, DWORD InitFlags) PURE;
    STDMETHOD       (CloseView)() PURE;
    STDMETHOD       (GetCaretPos)(long *piLine, long *piColumn) PURE;
    STDMETHOD       (SetCaretPos)(long iLine, long iColumn) PURE;
    STDMETHOD       (GetSelection)(long *piAnchorLine, long *piAnchorCol, long *piEndLine, long *piEndCol) PURE;
    STDMETHOD       (SetSelection)(long iAnchorLine, long iAnchorCol, long iEndLine, long iEndCol) PURE;
    STDMETHOD_(TextSelMode, GetSelectionMode)() PURE;
    STDMETHOD       (SetSelectionMode)(TextSelMode iSelMode) PURE;
    STDMETHOD       (ClearSelection)(BOOL fMoveToAnchor) PURE;
    STDMETHOD       (CenterLines)(long iLine, long iCount) PURE;
    STDMETHOD       (GetSelectedText)(BSTR *bstrText) PURE;
    STDMETHOD       (GetSelectionDataObject)(IDataObject **ppIDataObject) PURE;
    STDMETHOD       (GetTextStream)(long iTopLine, long iTopCol, long iBottomLine, long iBottomCol, BSTR *pbstrText) PURE;
    STDMETHOD       (GetBuffer)(IVsTextBuffer **ppBuffer) PURE;
    STDMETHOD       (SetBuffer)(IVsTextBuffer *pBuffer) PURE;
    STDMETHOD_(HWND, GetWindowHandle)()  PURE;
    STDMETHOD       (GetScrollInfo)(long iBar, long *piFirst, long *piLast, long *piVisible, long *piTop) PURE;
    STDMETHOD       (SetScrollPosition)(long iBar, long iPos) PURE;
    STDMETHOD       (AddCommandFilter)(IOleCommandTarget *pNewCmdTarg, IOleCommandTarget **ppNextCmdTarg) PURE;
    STDMETHOD       (RemoveCommandFilter)(IOleCommandTarget *pCmdTarg) PURE;
    STDMETHOD       (UpdateCompletionStatus)(IVsCompletionSet *pCompSet, DWORD dwFlags) PURE;
    STDMETHOD       (UpdateTipWindow)(IVsTipWindow *pTipWnd, DWORD dwFlags) PURE;
    STDMETHOD       (GetWordExtent)(long iLine, long iCol, DWORD dwFlags, long *piStartCol, long *piLength) PURE;
    STDMETHOD       (RestrictViewRange)(long iMinLine, long iMaxLine) PURE;
    STDMETHOD       (BeginCompoundEdit)(const WCHAR *pszDescription) PURE;
    STDMETHOD       (EndCompoundEdit)() PURE;
    STDMETHOD       (AbortCompoundEdit)() PURE;
    STDMETHOD       (ReplaceTextOnLine)(long iLine, long iStartCol, long iCharsToReplace, const WCHAR *pszNewText, long iNewLen) PURE;
    STDMETHOD       (GetLineAndColumn)(long iPos, long *piLine, long *piCol) PURE;
    STDMETHOD       (GetNearestPosition)(long iLine, long iCol, long *piPos, long *piVirtualSpaces) PURE;
};

#undef INTERFACE
#define INTERFACE	IVsCompletionSet

DECLARE_INTERFACE_(IVsCompletionSet, IUnknown){
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

    // ICompletionSet methods
    // Flags indicating specific behaviors of this completion set (CSF_* above)
    STDMETHOD_(DWORD,GetFlags)() PURE;

    // Get the number of available completions (assert > 1?)
    STDMETHOD_(long, GetCount)() PURE;
    
    // Get the text of a completion, as it is to be displayed in the list.
    // The pointer returned should be maintained by the completion set
    // object, and remain valid until final release, or until updated
    // through IVsTextView::UpdateCompletionStatus
	STDMETHOD       (GetDisplayText)(long iIndex, const WCHAR** ppszText, long *piGlyph) PURE;
    
    // Get the list of images (glyphs) supported by this completion set.
    // UNDONE:  Define HIMAGELIST so we don't have to cast...
	STDMETHOD       (GetImageList)(HANDLE *phImageList) PURE;
    
    // Return text describing the indicated item
	STDMETHOD       (GetDescriptionText)(long iIndex, BSTR *pbstrDescription) PURE;
    
	// Get the initial extent of the word to be completed.  *piLen can be
	// set to zero, but an anchor coordinate (line, col) must be provided.
	STDMETHOD       (GetInitialExtent)(long *piLine, long *piCol, long *piLen) PURE;
    
    // Determine the index of the closest matching completion, given
    // what has been typed so far.  Note that this is only called if
    // CSF_CUSTOMMATCHING is set in this completion set's flags.  If
    // *pdwFlags is set to contain one of the GBM_* flags
    // The default matching in the view uses case sensitive comparison.
	STDMETHOD       (GetBestMatch)(const WCHAR *pszSoFar, long iLength, long *piIndex, DWORD *pdwFlags) PURE;
    
    // Commit a completion.  S_FALSE return causes 'ding', and commit
    // character is NOT inserted.  S_OK return with *pbstrCompleteWord == pszSoFar
    // will match the default behavior if fSelected is FALSE.  If fSelected
    // is TRUE, the default behavior is to replace pszSoFar in the editor
    // with the text returned by GetDisplayText.  This is only called
    // if this completion set's flags include CSF_CUSTOMCOMMIT.
	STDMETHOD       (OnCommit)(const WCHAR *pszSoFar, long iIndex, BOOL fSelected, WCHAR cCommit, BSTR *pbstrCompleteWord)  PURE;
    
    // Called by the view when the completion set is no longer needed.
    // Note that if a language filter wants to force a dismiss of a completion
    // set, it calls IVsTextView::UpdateCompletionStatus with pCompSet == NULL;
    // if the view has a completion set pointer cached, it calls Dismiss()
    // on it and then releases it (and also removes the UI, etc).
	STDMETHOD_(void, Dismiss)() PURE;

};

#undef INTERFACE
#define INTERFACE	IVsTipWindow

DECLARE_INTERFACE_(IVsTipWindow, IUnknown){
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IVsTipWindow
    // Return the context stream (the position and length of text in the
    // buffer that should not be obscured by the tip window, but to which
    // the content of the tip is related).
    STDMETHOD   (GetContextStream)(long *piPos, long *piLength) PURE;

    // Get the size preferences for this tip window.  Note that implementations
    // should be written to handle cases where the size preference they
    // return here are not what they are given for real estate in which to
    // display content.  (i.e., it may not be possible to provide the
    // tip window in the requested size).  prcCtxBounds represents the bounding
    // rectangle of the tip's context should the implementation be interested.
    // (This rectangle is provided in screen coordinates).
    STDMETHOD   (GetSizePreferences)(BOOL fAngleBracket, const RECT *prcCtxBounds, TIPSIZEDATA *pSizeData) PURE;

    // Paint the tip window.
    STDMETHOD   (Paint)(HDC hdc, const RECT *prcPaint) PURE;

	// The window is being dismissed by the view.
    STDMETHOD_(void, Dismiss)() PURE;

    // Event handler for the tip window
    STDMETHOD_(LRESULT, WndProc) (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) PURE;
};

/////////////////////////////////////////////////////////////////////////
// MethodTextType enum
/////////////////////////////////////////////////////////////////////////
typedef enum _MethodTextType
{
    // Note that GetMethodText can return NULL for any of these types except NAME.
    // Any that it does not will be displayed.  Format is as follows:
    //  [TYPEPREFIX] [NAME] ( <parms> ) [TYPEPOSTFIX]
    //       [DESCRIPTION]
    MTT_TYPEPREFIX,             // return type if prefixed, ala Java/C++ ("long foo()")
    MTT_TYPEPOSTFIX,            // return type if postfixed, ala Basic ("foo() as long") (text must include "as" keyword)
    MTT_NAME,                   // Method name
    MTT_DESCRIPTION             // Description of method (gets its own line)
} MethodTextType;

/////////////////////////////////////////////////////////////////////////
// ParameterTextType enum
/////////////////////////////////////////////////////////////////////////
typedef enum _ParameterTextType
{
    // Note that GetParameterText can return NULL for any except DECLARATION, and NAME if DESCRIPTION is non-NULL.
    PTT_DECLARATION,            // full decl as it appears in signature (i.e. "long x", or "x as long")
    PTT_NAME,                   // parameter name only (i.e. "x")
    PTT_DESCRIPTION             // description (gets its own line, and is prefixed with NAME: (bolded))
} ParameterTextType;

#undef INTERFACE
#define INTERFACE	IVsMethodTipWindow

DECLARE_INTERFACE_(IVsMethodTipWindow, IVsTipWindow){
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IVsTipWindow methods
    STDMETHOD   (GetContextStream)(long *piPos, long *piLength) PURE;
    STDMETHOD   (GetSizePreferences)(BOOL fAngleBracket, const RECT *prcCtxBounds, TIPSIZEDATA *pSizeData) PURE;
    STDMETHOD   (Paint)(HDC hdc, const RECT *prcPaint) PURE;
    STDMETHOD_(void, Dismiss)() PURE;
    STDMETHOD_(LRESULT, WndProc) (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) PURE;

    // IVsMethodTipWindow methods
    STDMETHOD   (SetMethodData)(IVsMethodData *pData) PURE;
};

#undef INTERFACE
#define INTERFACE	IVsMethodData

DECLARE_INTERFACE_(IVsMethodData, IUnknown){
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

    //IVsMethodData methods
    STDMETHOD_(long,    GetOverloadCount)() PURE;
    STDMETHOD_(PCWSTR,  GetMethodText)(long iMethod, MethodTextType type) PURE;
    STDMETHOD_(long,    GetParameterCount)(long iMethod) PURE;
    STDMETHOD_(PCWSTR,  GetParameterText)(long iMethod, long iParm, ParameterTextType type) PURE;
    STDMETHOD_(long,    GetCurrentParameter)(long iMethod) PURE;
    STDMETHOD (			GetContextStream)(long *piPos, long *piLength) PURE;
    STDMETHOD_(void,    OnDismiss)() PURE;
	STDMETHOD_(long,    GetCurMethod) (void) PURE;
    STDMETHOD_(void,    UpdateView) (void) PURE;
    STDMETHOD_(long,    NextMethod) (void) PURE;
    STDMETHOD_(long,    PrevMethod) (void) PURE;
};

#undef INTERFACE
#define INTERFACE	IVsTextViewFilter

DECLARE_INTERFACE_(IVsTextViewFilter, IUnknown){
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

    //IVsTextViewFilter methods
    
    // Word extent calculation
    STDMETHOD (GetWordExtent) (long iPos, DWORD dwFlags, long *piStart, long *piLength) PURE;
    // Data-tip support.  piPos and piLength are in/out -- they indicate the
    // selected characters (*piLength will be one if no selection) on the way
    // in, and indicate the bounding context of the tip on the way out.
    STDMETHOD (GetDataTipText) (long *piPos, long *piLength, BSTR *bstrText) PURE;

    //this method is only in the old shell, to take the place of the command routing
    //through the language filters that happens in the new shell.  This gets called after
    //EVERY command in the shell, so you'd better find a quick way of knowing whether
    //anything that you care about has changed.  I'm passing in the WndMsg params so
    //you can figure out what happened if you need to know that
    STDMETHOD (OnChangeEditorStatus)(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) PURE;
};

#endif //ifndef __ACAPI_H__