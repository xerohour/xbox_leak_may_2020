#ifndef __UTILAPI_H__
#define __UTILAPI_H__

interface ISrcUtil;

typedef ISrcUtil*   PSRCUTIL;

/////////////////////////////////////////////////////////////////////////////
//  ISrcUtil interface
//      Allow the new debugger package access to the general utility routines

#undef INTERFACE
#define INTERFACE ISrcUtil

// Source environment settings.
#define ENV_HSCROLL                     1
#define ENV_VSCROLL                     2
#define ENV_ENABLE_MARGIN_GLYPHS        3
#define ENV_CX_DEFAULT_SELECT_MARGIN    4
#define ENV_PROMPT_BEFORE_SAVE          5
#define ENV_ENABLE_AUTO_COMPLETE		6
#define ENV_ENABLE_AUTO_COMMENTS		7
#define ENV_ENABLE_QUICK_INFO			8
#define ENV_ENABLE_PARAMETER_HELP		9
//#define ENV_AUTO_COMMENTS_TAGS		8

// Source color settings
#define CLR_TEXT                        0
#define CLR_CURRENTLINE                 1
#define CLR_COMMENT                     2
#define CLR_BREAKPT                     3

// CTextDoc Callback values
#define DCB_CLOSE                       1

class CIDEView;

DECLARE_INTERFACE_(ISrcUtil, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	/*
	** ISrcUtil
	**
	*/

    STDMETHOD_(BOOL, RECompare)(LPSTR sz, PSTR szRE) PURE;
    
	STDMETHOD_(BOOL, StatusText)(UINT uID, UINT uType, BOOL fForceUpdate) PURE;

    STDMETHOD_(BOOL, HandleBuildStateChange)(void) PURE;
    STDMETHOD_(BOOL, HandleProjectStateChange)(void) PURE;
    STDMETHOD_(BOOL, DebuggeeStateModified)(void) PURE;
    STDMETHOD_(BOOL, ProjectStateModified)(void) PURE;
    STDMETHOD_(BOOL, ClearAllDocStatus)(UINT state) PURE;
    STDMETHOD_(BOOL, ResetProjectState)(void) PURE;
	STDMETHOD_(BOOL, InitProjectState)(void) PURE;

    STDMETHOD_(FMT_ELEMENT *, GetSourceColor)(UINT) PURE;

    STDMETHOD_(DWORD, GetSrcEnvironParam)(UINT) PURE;

    STDMETHOD_(BOOL, SetLineStatus)(CDocument *, LONG, UINT, UINT, BOOL, BOOL) PURE; 	// Non-standard COM.
    STDMETHOD_(BOOL, FindLineStatus)(CDocument *, UINT, BOOL, LONG *) PURE; 	// Non-standard COM.
    STDMETHOD_(USHORT, GetLStat)(CDocument *, LONG) PURE; 	// Non-standard COM.

    STDMETHOD_(BOOL, GetCurrentText)(CView *, BOOL *, LPSTR, ULONG, ULONG *, ULONG *) PURE; 	// Non-standard COM.

    STDMETHOD_(BOOL, LoadDocument)(LPCSTR, FARPROC) PURE;
    STDMETHOD_(BOOL, FindDocument)(LPCSTR, CDocument **, BOOL) PURE; 	// Non-standard COM.
    STDMETHOD_(CView *, GetCurTextView)(void) PURE; 	// Non-standard COM.
    STDMETHOD_(CView *, FirstView)(CDocument *) PURE; 	// Non-standard COM.
    STDMETHOD_(CView *, FindTextDocActiveView)(CDocument *) PURE; 	// Non-standard COM.
    STDMETHOD_(CDocument *, GetDocFromHsf)(void *) PURE; 	// Non-standard COM.
    STDMETHOD_(BOOL, Select)(CView *, INT, INT, BOOL) PURE; 	// Non-standard COM.
    
	STDMETHOD_(BOOL, ResetDocMod)(void) PURE;
    STDMETHOD_(void *, GetHSF)(CDocument *) PURE; 	// Non-standard COM.
    STDMETHOD_(BOOL, SetHSF)(CDocument *, void *) PURE; 	// Non-standard COM.
    
	STDMETHOD_(BOOL, ViewBelongsToSrc)(CView *) PURE; 	// Non-standard COM.
    STDMETHOD_(INT, GetCurrentLine)(CView *) PURE; 	// Non-standard COM.
    STDMETHOD_(CDocument *, GetDocFromFileName)(PCSTR) PURE; 	// Non-standard COM.
    STDMETHOD_(BOOL, ThingyForBFHFindDoc)(CDocument **, CPath &) PURE; 	// Non-standard COM.
    STDMETHOD_(BOOL, ActivateTextDoc)(LPCSTR, INT) PURE;
	STDMETHOD(CreateSourceEditForHSF)(THIS_ void * hsf, ISourceEdit **ppISourceEdit) PURE;
	STDMETHOD(SetCallback)(THIS_ CDocument *, FARPROC) PURE;

	// This is used by the debugger to find non-disk based files
	// through the ISourceEdit::GetPseudoPath method
	STDMETHOD(CreateSourceEditFromDoc)(THIS_ CDocument* pDoc, ISourceEdit **ppISourceEdit, BOOL fWriteable) PURE;
	STDMETHOD(LoadNonDiskDocument)(LPCTSTR szName, CDocument** ppDoc) PURE;
#ifdef AUTO_COMPLETE
	// TODO: this method should be replaced by one that is more general. [t-mashra]
    STDMETHOD(GetCurFontDimensions)(UINT &, UINT &) PURE; 	// Non-standard COM.
#endif // AUTO_COMPLETE

    // Edit-n-Continue support
    STDMETHOD_(BOOL, ENCStateModified)(void) PURE;
    STDMETHOD_(BOOL, GetSrcPathsForENC)(CStringList *plstPath) PURE; // Non-standard COM.

	// Identifier info for AutoCompletion/NCB
	STDMETHOD (CreateQuickInfoForIdentifier) (LPCTSTR lpstrIdent) PURE;
};

#endif // __UTILAPI_H__
