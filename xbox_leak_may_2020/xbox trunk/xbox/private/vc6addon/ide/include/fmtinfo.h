//	REVIEW(DavidGa):  This needs to be drastically simplified and moved
//                    into shlsrvc.h.

//								FMTINFO.H
//
//	Classes defined:
//
//		CFormatInfo
//		CFormatInfoArray
//
//	Structures defined:
//
//		FMT_ELEMENT
//		FMT_WINDOW
//		FMT_WINGROUP
//		LOGFONTSEARCH
//		AUTO_COLOR
//
///////////////////////////
//
//	This provides font and color support for packages.  The FMT_* structures can be used by
//	the packages to statically define the default values.  CFormatInfo manages a single
//	FMT_WINGROUP structure and all it points to.  CFormatInfoArray is used by the dialog to manage
//	the CFormatInfo instances supplied by the packages.
//
/////

class CFormatInfo;
class CFmtIterator;
class CFmtGroupIterator;
class CFmtWindowIterator;
class CFmtElementIterator;
class CFCDialogState;
class CFontColorDlg;	// Define in fcdialog.h

class CPackage;

#ifndef __FMTINFO_H__
#define __FMTINFO_H__

_TCHAR * NewString (const _TCHAR * sz);

#define RGB_BLACK		RGB(0x00, 0x00, 0x00)
#define RGB_WHITE		RGB(0xFF, 0xFF, 0xFF)
#define RGB_RED			RGB(0xFF, 0x00, 0x00)
#define RGB_GREEN		RGB(0x00, 0xFF, 0x00)
#define RGB_BLUE		RGB(0x00, 0x00, 0xFF)
#define RGB_YELLOW		RGB(0xFF, 0xFF, 0x00)
#define RGB_MAGENTA		RGB(0xFF, 0x00, 0xFF)
#define RGB_CYAN		RGB(0x00, 0xFF, 0xFF)
#define RGB_LIGHTGRAY	RGB(0xC0, 0xC0, 0xC0)
#define RGB_GRAY		RGB(0x80, 0x80, 0x80)
#define RGB_DARKRED		RGB(0x80, 0x00, 0x00)
#define RGB_DARKGREEN	RGB(0x00, 0x80, 0x00)
#define RGB_DARKBLUE	RGB(0x00, 0x00, 0x80)
#define RGB_LIGHTBROWN	RGB(0x80, 0x80, 0x00)
#define RGB_DARKMAGENTA	RGB(0x80, 0x00, 0x80)
#define RGB_DARKCYAN	RGB(0x00, 0x80, 0x80)

// IMPORTANT: These macros depend heavily on the order of things in colors.cpp.
//    1) The order of colors in window must be Source Text, Text Selection, Text Highlight.
#define AUTO_TEXT			{ TRUE, TRUE, FALSE, FALSE, FALSE, COLOR_WINDOWTEXT },	{ TRUE, TRUE, FALSE, FALSE, FALSE, COLOR_WINDOW }
#define AUTO_SELECTION		{ TRUE, FALSE, FALSE, TRUE, FALSE, 0 },					{ TRUE, FALSE, FALSE, TRUE, FALSE, 0 }
#define AUTO_HIGHLIGHT 		{ TRUE, TRUE, FALSE, FALSE, FALSE, COLOR_HIGHLIGHTTEXT },	{ TRUE, TRUE, FALSE, FALSE, FALSE, COLOR_HIGHLIGHT }

#define AUTO_REF(n) 		{ TRUE, FALSE, FALSE, FALSE, FALSE, n }, 	{ TRUE, FALSE, FALSE, FALSE, FALSE, n }
#define AUTO_REF_SRC(n) 	{ TRUE, FALSE, TRUE, FALSE, FALSE, n }, 	{ TRUE, FALSE, TRUE, FALSE, FALSE, n }

#define BACKAUTO_TEXT			{ FALSE, TRUE, FALSE, FALSE, FALSE, COLOR_WINDOWTEXT },	{ TRUE, TRUE, FALSE, FALSE, FALSE, COLOR_WINDOW }
#define BACKAUTO_SELECTION		{ FALSE, FALSE, FALSE, TRUE, FALSE, 0 },					{ TRUE, FALSE, FALSE, TRUE, FALSE, 0 }
#define BACKAUTO_HIGHLIGHT 		{ FALSE, TRUE, FALSE, FALSE, FALSE, COLOR_HIGHLIGHTTEXT },	{ TRUE, TRUE, FALSE, FALSE, FALSE, COLOR_HIGHLIGHT }

#define BACKAUTO_REF(n) 		{ FALSE, FALSE, FALSE, FALSE, FALSE, n }, 	{ TRUE, FALSE, FALSE, FALSE, FALSE, n }
#define BACKAUTO_REF_SRC(n) 	{ FALSE, FALSE, TRUE, FALSE, FALSE, n }, 	{ TRUE, FALSE, TRUE, FALSE, FALSE, n }

#define NOTAUTO_TEXT		{ FALSE, TRUE, FALSE, FALSE, FALSE, COLOR_WINDOWTEXT },	{ FALSE, TRUE, FALSE, FALSE, FALSE, COLOR_WINDOW }
#define NOTAUTO_SELECTION	{ FALSE, FALSE, FALSE, TRUE, FALSE, 0 },	{ FALSE, FALSE, FALSE, TRUE, FALSE, 0 }
#define NOTAUTO_HIGHLIGHT	{ FALSE, TRUE, FALSE, FALSE, FALSE, COLOR_HIGHLIGHTTEXT },	{ FALSE, TRUE, FALSE, FALSE, FALSE, COLOR_HIGHLIGHT }

#define NOTAUTO_REF(n) 		{ FALSE, FALSE, FALSE, FALSE, FALSE, n }, 		{ FALSE, FALSE, FALSE, FALSE, FALSE, n }
#define NOTAUTO_REF_SRC(n) 	{ FALSE, FALSE, TRUE, FALSE, FALSE, n }, 		{ FALSE, FALSE, TRUE, FALSE, FALSE, n }


struct AUTO_COLOR
{
	WORD	bOn:1;		// Is auto color being used now?
	WORD	bSys:1;		// Get the color from the system(1) or from a window(0)?
	WORD	bSrc:1;		// If bSys == 0, use this window(0) or the Source Window(1)?
	WORD	bRev:1;		// If from this window, reverse fore/background(1)?
	WORD	bUpd:1;		// Used by UpdateAutoColors().
	WORD	index:5;	// Index into element list(bSys==0) or COLOR_* value (bSys==1).
};

// This structure is used to translate between a Windows LOGFONT
// structure and name/size/flags combination.
//
struct LOGFONTSEARCH
{
	LOGFONT		LogFont;
	TCHAR *		szFontFace;
	BYTE		nFontSize;
	BYTE		nCharSet;
	int			nPixPerInchY;
	BOOL		bMonospace:1;
	BOOL		bTrueType:1;
};


struct FMT_ELEMENT
{
	TCHAR *		szElement;
	COLORREF	rgbText;
	COLORREF	rgbBackground;
	AUTO_COLOR	autoFore;
	AUTO_COLOR	autoBack;
};

struct FMT_WINDOW
{
	TCHAR *			szWindow;
	TCHAR *			szRegEntry;
	BOOL			bChanged:1;
	BOOL			bMonospace:1;
	TCHAR *			szFontFace;
	UINT			nFontSize;
	PLOGFONT		pLogFont;
	int				nElements;
	FMT_ELEMENT *	rgElements;
};

struct FMT_WINGROUP
{
	int				nWindows;
	BOOL			bSingleGroup:1;	// If this is a singleton, do we show the group name?
	_TCHAR *		szName;		// Name of Windows group
	FMT_WINDOW *	rgWindows;
};


#define FMT_ELEMENTS(x)		(sizeof(x) / sizeof(FMT_ELEMENT))
#define FMT_WINDOWS(x)		(sizeof(x) / sizeof(FMT_WINDOW))

enum DialogState { PreDialog, SelAll, SelGroup, SelWindow, PostDialog };

//////////////////////////////////////////////////////////////////////////////
//																			//
//							class CFormatInfo								//
//																			//
//	This class handles one instance of FMT_WINGROUP data.					//
//																			//
//////////////////////////////////////////////////////////////////////////////


class CFormatInfo : public CObject
{
// Iterators are friends.
friend CFmtIterator;
friend CFmtGroupIterator;
friend CFmtWindowIterator;
friend CFmtElementIterator;
friend CFCDialogState;

DECLARE_DYNAMIC (CFormatInfo);

private:
	CPackage * 		m_pPackage;		// Package info belongs to
	FMT_WINGROUP *	m_pWinGroup;
	int				m_nWinGroups;
	CFormatInfo *	m_pFormatInfoFromPackage;

	// These are used by the Registry code
	//
	static _TCHAR	m_szRegKey[];
	static _TCHAR	m_szFontFace[];
	static _TCHAR	m_szFontSize[];
	static _TCHAR	m_szCharSet[];

	// This is useful to know
	//
	static int		m_nPixPerInchY;

	// These are useful tools

	void SetForeColor ( UINT iFmtInfo, UINT iGroup, UINT iElement, COLORREF rgb );
	void SetBackColor ( UINT iFmtInfo, UINT iGroup, UINT iElement, COLORREF rgb );

public:
	CFormatInfo ( CPackage * pPackage = NULL);
	~CFormatInfo ();

	void Clear ();	// Remove and free all data

	BOOL IsEmpty () const { return m_nWinGroups == 0; }

	void SaveToRegistry () const;
	void UpdateFromRegistry ();

	BOOL GetFormatInfo ( CPackage * pPackage = NULL);

	const CFormatInfo& operator= (const FMT_WINGROUP&);
	const CFormatInfo& operator= (const CFormatInfo&);
	const CFormatInfo& operator+= (const FMT_WINGROUP&);

	FMT_ELEMENT * GetElementList ( const _TCHAR * szWindow ) const;
	FMT_WINDOW *  GetWindow ( const _TCHAR * szWindow ) const;

	int GetWindowCount () const;

	void UpdateAutoColors (FMT_WINDOW * pWindow);
	void UpdateAllAutoColors ();

	void Update (const CFormatInfo& fmtInfo, int iWinGroup = -1, int iWindow = -1);

	void UpdateAllLogFonts ();

	static int GetLogFont (LOGFONTSEARCH * pSearchLogFont);
	static void UpdateLogFont (FMT_WINDOW * pWindow);

	// This is used as an EnumLogFont callack to get the LOGFONT struct for
	// a given set of name/size/flags.  LPARAM is a pointer to a
	// LOGFONTSEARCH.
	static int CALLBACK SearchCallBack(CONST ENUMLOGFONT *, CONST NEWTEXTMETRIC *, int, LPARAM);


	//
	// Support for Fonts and Colors dialog
	//
	BOOL Commit () const;
	BOOL IsDirty () const;

	void Clean ();
};


//////////////////////////////////////////////////////////////////////////////
//						class CFmtIterator									//
//						class CFmtGroupIterator								//
//						class CFmtWindowIterator							//
//						class CFmtElementIterator							//
//																			//
//	Iteration classes for CFormatInfo.										//
//																			//
//////////////////////////////////////////////////////////////////////////////

class CFmtIterator
{
protected:

	const CObArray&	m_rgFmtInfo;
	int		m_iFmtInfo;

public:
	CFmtIterator ( const CObArray& rgFmtInfo);

	CFormatInfo * Get ();
	CFormatInfo * Peek () const;

	inline void Inc ();

	inline UINT	GetIndex () const;
	inline void	Set (int iFmtInfo);
};

class CFmtGroupIterator : public CFmtIterator
{
protected:

	int	m_iGroup;

public:
	CFmtGroupIterator ( const CObArray& rgFmtInfo);

	FMT_WINGROUP * Get ();
	FMT_WINGROUP * Peek () const;

	inline void Inc ();

	void	SetLinear (UINT iGroupLinear);
	void	Set (FMT_WINGROUP * pWindow);
	inline void	Set (int iFmtInfo, int iWinGroup);
	FMT_WINGROUP *  GetLinear ();
	FMT_WINGROUP * PeekLinear ();
	
	inline UINT	GetIndex () const;
};

class CFmtWindowIterator : public CFmtGroupIterator
{
protected:

	int		m_iWindow;


public:
	CFmtWindowIterator ( const CObArray& rgFmtInfo);

	FMT_WINDOW * Get ();
	FMT_WINDOW * Peek () const;

	inline void Inc ();

	void	SetLinear (UINT iWinLinear);
	void	Set (FMT_WINDOW * pWindow);
	FMT_WINDOW * GetLinear ();
	FMT_WINDOW * PeekLinear ();

	inline UINT	GetIndex () const;
};

class CFmtElementIterator : public CFmtWindowIterator
{
protected:

	int		m_iElement;

public:
	CFmtElementIterator ( const CObArray& rgFmtInfo);

	FMT_ELEMENT * Get ();
	FMT_ELEMENT * Peek () const;

	inline void Inc ();
	inline void Dec ();	// HACK!

	void	SetLinear (UINT iElementLinear);
	FMT_ELEMENT * GetLinear ();
	FMT_ELEMENT * PeekLinear ();

	inline UINT	GetIndex () const;

};

inline UINT	CFmtIterator::GetIndex () const { return m_iFmtInfo; }
inline UINT	CFmtGroupIterator::GetIndex () const { return m_iGroup; }
inline UINT	CFmtWindowIterator::GetIndex () const { return m_iWindow; }
inline UINT	CFmtElementIterator::GetIndex () const { return m_iElement; }

inline void CFmtIterator::Inc () { m_iFmtInfo++; }
inline void CFmtGroupIterator::Inc () { m_iGroup++; }
inline void CFmtWindowIterator::Inc () { m_iWindow++; }
inline void CFmtElementIterator::Inc () { m_iElement++; }

inline void CFmtElementIterator::Dec () { m_iElement--; }

inline void	CFmtGroupIterator::Set (int iFmtInfo, int iWinGroup)
{
	m_iGroup = iWinGroup;
	CFmtIterator::Set (iFmtInfo);
}

inline void	CFmtIterator::Set (int iFmtInfo)
{
	m_iFmtInfo = iFmtInfo;
}

//////////////////////////////////////////////////////////////////////////////
//							class CElList									//
//																			//
// A list of unique element names.  Uniqueness is enforced.  Will also		//
// iterate a given name - i.e. will sequentially find all elements with		//
// the given name.															//
//																			//
//////////////////////////////////////////////////////////////////////////////

class CElList
{
	UINT		m_inc;	// Allocation increment
	CPtrArray	m_rgStrings;

	CString		m_strSearch;
public:
	CElList (UINT inc = 4);
	CElList::~CElList ();

	BOOL AddString (const CString& str);	// FALSE -> String was already there

	FMT_ELEMENT * GetFirst (_TCHAR * szName, CFmtElementIterator& it, int iFormat = -1, int iGroup = -1);
	FMT_ELEMENT * GetFirst (UINT iEl, CFmtElementIterator& it, int iFormat = -1, int iGroup = -1);
	FMT_ELEMENT * GetNext (CFmtElementIterator& it, int iFormat = -1, int iGroup = -1);

	void Fill (CListBox& lbox) const;
	void Clear ();

	BOOL IsForeColorTheSame (UINT iEl, CFmtElementIterator& it, int iFormat, int iGroup, COLORREF& rgb);
	BOOL IsBackColorTheSame (UINT iEl, CFmtElementIterator& it, int iFormat, int iGroup, COLORREF& rgb);

	inline int	GetSize () const;
};

inline int	CElList::GetSize () const
{
	return m_rgStrings.GetSize ();
}
	
//////////////////////////////////////////////////////////////////////////////
//							class CWinList									//
//																			//
//	A list of FMT_WINDOW and FMT_WINGROUP.  Used for the list of windows	//
//	under "Category" in the Fonts dialogs.									//
//																			//
//////////////////////////////////////////////////////////////////////////////


class CWinList
{
	CPtrArray	m_rgGroups;
	CPtrArray	m_rgWindows;

public:
	CWinList (UINT inc = 4);
	~CWinList ();

	void Add (FMT_WINGROUP * pGroup);

	void Fill (CListBox& lbox) const;

	BOOL GetName (int index, CString& strName) const;
	BOOL GetPtr (int index, void * &p) const;

	int FirstWindowIndex () const;
};

//////////////////////////////////////////////////////////////////////////////
//							class FCDialogState								//
//																			//
//	This class acts as an intermediary between a CFontColorDlg and the		//
//	CFormatInfo objects underlying it.  It locates and manages the data		//
//	from the packages, it keeps track of the state of the dialog and		//
//  propogates changes to the underlying data.  It also propogates state	//
//	changes in one part of the dialog to the rest of the dialog.			//
//																			//
//////////////////////////////////////////////////////////////////////////////

class CFCDialogState
{
	DialogState		m_state;

	// As an intermediary, an instance of this class must have access
	// to the CFontColorDlg object that it is working with
	CFontColorDlg *	m_pDialog;

	CObArray		m_rgFmtInfo;

	// In the Group and All states, a list of unique element names from
	// the group(s) in question is maintained:
	CElList			m_elList;

	// This holds the list of windows and window groups that is shown
	// in the dialog
	CWinList		m_winList;
	UINT			m_iCurWin;

	// These describe the current Window/Element selection in terms of
	// CFormatInfo array element / Windows Group / Window / Element.
	// 
	UINT			m_nFmtInfoCur;
	UINT			m_nWinGroupCur;
	UINT			m_nWindowCur;
	UINT			m_nElementCur;

	// Enabled flags. Which parts of the dialog are enabled?

	BOOL	m_benFontName:1;
	BOOL	m_benFontSize:1;
	BOOL	m_benElements:1;
	BOOL	m_benForeColor:1;
	BOOL	m_benBackColor:1;

public:

	CFCDialogState ( CFontColorDlg * pDialog = NULL );
	~CFCDialogState ();

	// State retrievers
	inline operator DialogState () const;

	FMT_WINDOW * GetWinCur (BOOL bReturnNull = TRUE, BOOL bMustHaveFont = FALSE) const;

	COLORREF GetForeColor ();
	COLORREF GetBackColor ();
	PLOGFONT GetCurrentLogFont () const;

	// Talk a walk over 
	enum WW_ACTION { FontEquality, FontPitch };
	enum WW_RESULT { SameFont, FontsDiffer, NoFont, PitchFixed, PitchProportional, PitchMixed, NoWindows };
	WW_RESULT WindowWalk (WW_ACTION act) const;

	// State modifiers.
	void InitDialog ();
	void SetCurrentWindow (int iWindow);
	void SetCurrentElement  (int iElement);
	void SetCurrentFont (const CString& strFontName, int size, BOOL bTrueType);
	void SetCurrentFontSize (int nFontSize);

	void SetForeColor ( COLORREF rgb );
	void SetBackColor ( COLORREF rgb );

	void RestoreDefaults ();

	void UpdateAllAutoColors ();

	// Other stuff
	void Commit();

};

inline CFCDialogState::operator DialogState () const
{
	return m_state;
}



#endif  // __FMTINFO_H__

