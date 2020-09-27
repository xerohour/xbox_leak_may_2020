#ifndef __LEX__
#define __LEX__

#include <tchar.h>
#include <afx.h>
#include <afxtempl.h>
#include <shlsrvc.h>
#include "memref.h"

//#include "srcapi.h"
//#include "objmodel/textdefs.h"
//#include "objmodel/textauto.h"

#define P_IN(x)     const x &
#define P_OUT(x)    x &
#define P_IO(x)     x &
#define PURE        = 0

// POPDESC is used for popup menus
// defined in shlmenu.h
struct POPDESC;

class CLex;
// Lex state, kept at the beginning of every line (lxsBOL) from
// previous line's state at its end (lxsEOL). Must fit all bits
// necessary to restart lexing on a line by line basis.

const int       cbLexState = sizeof (DWORD);
typedef DWORD   LXS;
typedef LXS *   PLXS;
const LXS       lxsMaskFortranFixed =   0x80000000;
const LXS       lxsMaskFortranTab6 =    0x40000000;
const LXS       lxsMaskFortran =        lxsMaskFortranFixed | lxsMaskFortranTab6;
const LXS       lxsMaskValid =          0x20000000; // Set internally, lexers should not use

inline LXS  LxsFortranBits ( LXS lxs ) { return lxs & lxsMaskFortran; }
inline BOOL FFortranFixed ( LXS lxs ) { return !!(lxs & lxsMaskFortranFixed); }
inline BOOL FFortranTab6 ( LXS lxs ) { return (lxs & lxsMaskFortran) == lxsMaskFortran; }

//
// smart indent support
//
enum INDENT_TYPE {	// Indent Type
	itNone,		// always move caret to home
	itStd,		// move caret to previous lines indent
	itSmart		// smart, context based indenting
	};

// CMemoryBlock TBD, but supports 2 sizes (allocated/used)
// as well as pointer casting operators.
typedef CRefMem * PMEMBL;

typedef DWORD _TS;  // Tabs and Spaces

struct TAB_METRIC;  // forward declaration
typedef TAB_METRIC *PTABMETRIC;
typedef const TAB_METRIC *PCTABMETRIC;

struct GET_LINE_STATE
{
	UINT        lineCur;
	LXS         lxsBolCur;  // necessary for some impls.
	PCTABMETRIC pTabMetric;
	VOID *      pvReserved; // for holding a CTextDoc*
};
typedef GET_LINE_STATE *PGLST;

// The smart indent functions below require a PMEMBL for the editor
// to copy lines into when PfnFGetLine is called from the lexer.
//
// _TS * is a pointer to an array of cLines entries.  Each entry
// describes the number of spaces that need to appear at the beginning
// of the corresponding line.  (The editor is then responsible for
// converting cSpaces into the correct number of tabs and spaces.)

typedef BOOL (__stdcall *PfnFGetLine)(PGLST, INT dLine, PMEMBL);
typedef BOOL (__stdcall *PfnFGetLxs)(PGLST, INT dLine, PLXS & plxs);
typedef void (__stdcall *PfnSmartIndent)
	(CLex *, PGLST, _TS *, UINT cLines, PfnFGetLine, PfnFGetLxs, PMEMBL );

// A tab expansion block is used to determine how to expand tabs
// in the cases where the lexer does special tab expansion as
// well as for variable indent levels.  Given both the tab
// expansion and an array of TABEXBs describing indents,
// the lexer (or anyone) can handle all tabbing/indent behaviors.
typedef INT CTAB;
const CTAB  ctabEnd = 0;
const CTAB  ctabInfinite = -1;  // also implies end of list

struct TABEXB // Tab Expansion Block
{
	CTAB ctabRun;  // run of tabs this width expand to
	union
	{
		UINT ctchIndent; // count of spaces to expand indent
		UINT ctchTab;    // count of spaces to expand tab
	};
};
typedef TABEXB * PTABEXB;
typedef const TABEXB * PCTABEXB;

struct TAB_METRIC
{
	UINT            iTabHead;           // back pointer to extra info
	BOOL            fTrackDefault;      // track the default settings
	BOOL            fInsertSpaces;
	INDENT_TYPE     itCur;
	INDENT_TYPE     itSupported;
	BOOL            fIndentOpen;
	BOOL            fIndentClose;
	UINT            cLinesLookBack;
	PfnSmartIndent  pfnSmartIndent;
	UINT            ctchTab;    // in spaces
	UINT            ctabexb;
	TABEXB          rgtabexb[1];
};

enum SMTSTY  // Smart Styles, dealing with smart indent
{
	smtstyNone = 0,
	smtstySmartIndent = 0x1,
	smtstyIndentSpacesOnly = 0x2,   // only use spaces for indent
	smtstyIndentTabsOnly = 0x4,     // only use tabs (implies tab==indent)
	smtstyIndentBlockTokens = 0x8,  // supports indenting block tokens
	smtstyIndentLookBack = 0x10,    // supports the lookback var
};

// this info comes from the lexer and supplies us with
// the smart indent styles it supports as well as the
// smart indent function pointer and a phrase to use for
// "block token".  in C/C++, that phrase will be "Brace".
const unsigned		ctchBlockTokenPhrase = 31;
struct SMART_STYLES 
{
	DWORD          dwStyles;
	TCHAR          szBlockTokenPhrase[ ctchBlockTokenPhrase + 1 ];
	LPCSTR *       rgExtensions;			// same info as in LANGMETRICS
	PfnSmartIndent pfnSmartIndent;
};


#if 0 // unused
//
// utility functions/defines
//
inline UINT CTabFromTs( _TS ts )	{ return ts >> 16; }
inline UINT CSpaceFromTs( _TS ts )	{ return ts & 0xffff; }
inline _TS TsFromTabSpace( UINT cTab, UINT cSpace ) { return _TS((cTab << 16) + cSpace); }
#endif

// Lexer and language Metrics
const unsigned ctchUserTokenPhrase = 100;
struct USERTOKENS
{
	INT			token;		// preassigned in the user range
	TCHAR		szToken[ctchUserTokenPhrase+1];	// token class name exposed to user
	COLORREF	RGBText;
	COLORREF	RGBBackground;
	AUTO_COLOR	autocolorFore;
	AUTO_COLOR	autocolorBack;
};

typedef USERTOKENS *		PUSERTOKENS;
typedef const USERTOKENS *	PCUSERTOKENS;

struct LANGMETRICS
{
	DWORD			cbMetrics;			// sizeof (LANGMETRICS)
	DWORD			verMetrics;			// version support
	DWORD			cbLXS;				// currently hardwired to 4
	DWORD			fSimpleLxsCmp;		// currently hardwired to TRUE
	LPCSTR * 		rgExtensionsLexed;	// list of file extensions this lexer handles
	PCUSERTOKENS	rgusertok;			// user token list for editor to map
										// new tokens to. NULL szToken denotes
										// end of array.
};

typedef LANGMETRICS * PLANGMETRICS;
typedef const LANGMETRICS * PCLANGMETRICS;

typedef int 	TOKEN;

enum TOKCLS { // token classes
	tokclsError = 0,

	// all standard language keywords
	tokclsKeyWordMin = 1,

	// for block start/end that are keywords instead of operators...like
	// Pascal or BASIC for instance.
	tokclsKeyWordOpenBlock = 0xfe,
	tokclsKeyWordCloseBlock = 0xff,

	tokclsKeyWordMax = 0x100,

	// all language operators
	tokclsOpMin = 0x100,
	tokclsOpSpecOpenBlock = 0x1fe,
	tokclsOpSpecCloseBlock = 0x1ff,
	tokclsOpMax = 0x200,

	// special, hard coded operators that editor keys off of
	tokclsOpSpecMin = 0x200,
	tokclsOpSpecEOL = 0x200,
	tokclsOpSpecLineCmt = 0x201, // automatic skip to eol on this one
	tokclsOpSpecEOS = 0x202,
	tokclsOpSpecMax = 0x210,

	// all identifiers, give ~500 possibilities
	tokclsIdentMin = 0x210,
	tokclsIdentUser = 0x211,	// special idents (user keywords)
	tokclsIdentMax = 0x400,

	// all constants (numeric and string)
	tokclsConstMin = 0x400,
	tokclsConstInteger = 0x400,
	tokclsConstReal = 0x401,
	tokclsConstString = 0x402,
	tokclsStringPart = 0x402,		// partial string ("....)
	tokclsConstMax = 0x410,

	// comments
	tokclsCommentMin = 0x500,
	tokclsCommentPart = 0x500,	// partial comment (/* ...)
	tokclsCommentMax = 0x510,

	// language dependent token class(es) start at 0x800 to 0xfff
	tokclsUserMin = 0x800,
	tokclsUserLast = 0xfff,
	tokclsUserMax = 0x1000,

	// mask to ignore all the bits in a token[class] that the lexer can use
	// for private status.	they will be masked off and ignored by clients
	// of the lexer.  A good use of this feature is to encode the real token
	// type in the lexer private portion (area is ~tokclsMask) when including
	// meta-token types (such as MFC/Wizard user token types) so that other
	// clients of the lexer can keep that information.
	tokclsUserMask = ~(tokclsUserMin - 1),
	tokclsMask = tokclsUserLast,

};

// Alternate way of looking at a token, editor will only look at tokUser.
// Other clients of the lexer (like the parser or the EE) may want to look
// at the actual token in tokAct.  If any of tokAct is set, then it is expected
// that the actual token is different than the meta token it passed back.
// The status bits are only used by the lexer for whatever it wants.

union TOK_ALT  {
	TOKEN 	tok;
	struct {
        unsigned        tokUser : 12;
        unsigned        tokUserStatus : 4;
		unsigned		tokAct : 12;
		unsigned		tokActStatus : 4;
    };
};

// A text token block indicates the token, and its starting and ending 
// indexes in the line of source just lexed.
// Note that for any N > 0, rgtxtb[N].ibTokMin >= rgtxtb[N-1].ibTokMac.
// if it is such that rgtxtb[N].ibTokMin > rgtxtb[N-1].ibTokMac, then 
// the intervening unclassified characters are treated as white space tokens.
struct TXTB { // Text token class block
	TOKEN 	tok;
	UINT	ibTokMin;		// token length given by ibTokMac - ibTokMin
	UINT	ibTokMac;		// given in bytes
};

typedef CArray <TXTB, TXTB> RGTXTB;

// A SUBLANG structure was originally used for identifying different 
// dialects of the same language (like fortran fixed and fortran free) 
// that use the same lexer, can be treated as two languages in the editor, 
// and share all the same color/font info in the format dialog.
//
// We've extended it to be a general descriptor for a type of text file.
//
struct SUBLANG
{
	LPCSTR  szSubLang;
	LXS     lxsInitial;
	UINT    nIdTemplate; // Icon and MFC doc template string resource id
	CLSID   clsidTemplate;
};
typedef SUBLANG * PSUBLANG;
typedef const SUBLANG *	PCSUBLANG;

// Editor Interface Request flags
#define EIR_IEdit   0x0001
#define EIR_IDoc    0x0002
#define EIR_ISel    0x0004

// forward decls
interface ISourceEdit;
interface ITextDocument;
interface ITextSelection;
class CBContextPopupMenu;

///////////////////////////////////////////////////////
// Class CLex
// 	specific lexer should derive from this class
///////////////////////////////////////////////////////
class CLex
{
protected:
	// ctor and dtor are protected and can only be called 
	// internally by the creator in the owning package.
	CLex() {};

	virtual ~CLex() {};

public:
	// give a pointer to the language specific metrics.
	// Lexer can expect it to be used at any time, so leave
	// it allocated and unchanged. It can ( and probably
	// should) be in r/o memory.
	virtual void QueryLangMetrics ( P_IO(LANGMETRICS) ) PURE;

	// return a string indicating the language this lexer
	// understands, eg: "C/C++", "Fortran", or "Basic"

// REVIEW [paulde]: see if we can just get this from the sublang,
// since we're pretty much requiring a PSUBLANG override now.
	virtual LPCSTR SzLanguage() PURE;

// [paulde]	To be removed: use sublangs instead.
	// return a default file extension string id for this language
	virtual UINT GetDefaultFileExtension( LPCSTR szSubLanguage ) PURE;

	// see if this lexer will by default handle this type of file
	virtual BOOL FCanLexFile ( LPCSTR szFilename ) PURE;

	// compare two lexer states for equality or not.
// REVIEW [paulde]: see if we can remove this.
	virtual BOOL FCmpLxsLxs ( PLXS, PLXS ) PURE;

	// lex the line, giving us back ton'o'info,
	// returns count of TCHAR that were tokenized in
	// ptchLine.

	virtual UINT CbLexLine (
					LPCSTR			ptchLine,
					UINT			cbLine,
					PLXS			plxs,	  // in: BOL state, out: eol state
					P_OUT (RGTXTB)	rgtxtb,
					P_IO(DWORD)		dwReserved
					) PURE;

	// slim version of lex line
	// since only interested in the plxs.
	virtual UINT CbLexLine (
					LPCSTR			ptchLine,
					UINT			cbLine,
					PLXS			plxs,	  // in: BOL state, out: eol state
					P_IO(DWORD)		dwReserved
					) PURE;

	// give out the smart indent info
	virtual void QuerySmartStyles ( P_IO(SMART_STYLES) ) PURE;

	// get the initial LXS for this file, so one lexer can handle multiple
	//	languages or variations of the same language.
	virtual LXS LxsInitial ( LPCSTR szFilename ) { return LXS(0); }

	// inform the lexer that the default set of extensions to be lexed has
	//	been changed and the new list should be used instead.
	virtual void SetExtensionsLexed ( P_IN(CStringList) ) { }

	// gives back a pointer to an array of SUBLANGs, ending with a szSubLang == NULL
	virtual PCSUBLANG PSubLang() { return NULL; }

// REVIEW [paulde]: see if we can remove the 'special state' methods
	// returns TRUE if LXS is in the special state (in C++, in AFX_WIZARD state for instance)
	virtual BOOL IsInSpecialState (LXS lxs) { return FALSE;}

	// reset the state if it is in the special state
	virtual void UnsetSpecialState (P_IO(LXS) lxs) { return; }

	// get the size of the first tab (used only for fortran fixed form)
	virtual UINT CbFirstTab(
					LPCSTR			ptchLine,
					UINT			cbLine,
					LXS				lxs	  // BOL state
					) { return 0; };

	// return a popdesc array with the menu to display
	// OBSOLETE: override FCanDoContextMenu and and DoContextMenu instead.
    virtual POPDESC* GetContextMenu() {return NULL;}

	// Can lexer display a popup menu?
	// If so, return TRUE with dwFlags set with the EIR_... values to indicate 
	// which interfaces you want passed to you by DoContextMenu().
	virtual BOOL FCanDoContextMenu( P_OUT(ULONG) dwFlags ) { dwFlags = 0; return FALSE; }

	// Display the context menu.
	// Recommended lexer implementation:
    // void CMyLexer::DoContextMenu( ... )
    // {
    //    CBContextPopupMenu menuLexer; // lexer's context menu
    //    // Create lexer menu, probably using menuLexer.Create(PPOPDESC);
    //    ...
    //    // Merge editor menu with lexer menu
    //    // Note that the lexer menu is empty after merging.
    //    menu.MergeMenu( nMergePos, nMergeFlags, & menuLexer );
    //
    //   // Show menu
    //    menu.TrackPopup(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, AfxGetMainWnd());
    //
    //   // release interfaces
    //   piEdit->Release();
    //   ...
    // }
	virtual void DoContextMenu(
		CPoint pt,                 // Mouse position
		CBContextPopupMenu & menu, // Editor's menu - should merge lexer's items with editor's
		UINT nMergePos,
		UINT nMergeFlags,
		// Requested interfaces - can be NULL
		ISourceEdit * piEdit,
		ITextDocument * piDoc,
		ITextSelection * piSel
		)
	{ ASSERT(0); } // must provide an implementation if FCanDoContextMenu returns TRUE

	// Can lexer render a file template (called at FileNew)?
	// If so, return TRUE with dwFlags set with the EIR_... values to indicate 
	// which interfaces you want passed to you by RenderTemplate().
	virtual BOOL FCanRenderTemplate( P_OUT(ULONG) dwFlags ) { return FALSE; }

	virtual void RenderTemplate(
		ISourceEdit* piEdit, 
		ITextDocument* piDoc, 
		ITextSelection* piSel )
	{ ASSERT(0); } // must provide an implementation if FCanRenderTemplate returns TRUE
};

typedef CLex *	PLEX;

// Maximum length (not including \0 at end) of name returned by CLex::SzLanguage()
#define MAX_LANGNAMELEN (50)

// C/C++ tokens included here for historical/migration reasons
#ifdef USE_CPLUSPLUS_TOKENS
#include "tokcpp.h"
#endif	// USE_CPLUSPLUS_TOKENS

#endif
