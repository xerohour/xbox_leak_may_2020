//-----------------------------------------------------------------------
//
// File:	 lex.h
//
// Contains: Smart indenting code for C / C++ files.  A number of defines
//			 and structures from various places in the old shell tree have
//			 been added to this file in order to get things to work.
//
// REVIEW:   This code is just a port of the old lexer code and is a 
//			 candidate for a rewrite since it has to be hacked in order
//			 to work with the new shell editor.  I've tried to isolate
//			 this code as much as possible so that a rewrite would be 
//			 easier in the event that we decide to do one.		
//
//-----------------------------------------------------------------------

#pragma once

#include "memref.h"
#include <cppsvc.h>
#include "cvr.h"
#include "assert_.h"
#include "helper.h"
#include "array_t.h"

#define P_IN(x)     const x &
#define P_OUT(x)    x &
#define P_IO(x)     x &
#define PURE        = 0

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

typedef Array <TXTB> RGTXTB;

typedef UINT	TOKENTYPE;

typedef unsigned short HINT;

typedef struct ReservedWord
{
	WCHAR *		psz;		// pointer to reserved word string 
	int			ich;		// length of reserved word
	TOKENTYPE	tTokenType;	// token type
} ReservedWord;


class CVCStringList;

///////////////////////////////////////////////////////
// Class CLex
// 	specific lexer should derive from this class
///////////////////////////////////////////////////////
class CLex
{
protected :

	ReservedWord *		m_pUserDefKW;
    int					m_iSizeUserDef;

	CLex(ReservedWord * pUserDefKW = NULL) : m_pUserDefKW(pUserDefKW), m_iSizeUserDef(0) { }
	virtual ~CLex(void) {};

	static void LoadNCBExtensionsFromRegistry(LPCTSTR szRegistryValueName, LPCTSTR * rgszDefaultExtensions,
		CVCStringList & listExtensions);

	static BOOL FIsValidFile(LPCTSTR szFileName, const CVCStringList & listExtensions);

public:
    static void LoadCPPODLExtensionsFromRegistry();

public :

	// lex the line, giving us back ton'o'info,
	// returns count of TCHAR that were tokenized in ptchLine.
	//
	
	virtual UINT CbLexLine (
					LPCWSTR			ptchLine,
					UINT			cbLine,
					PLXS			plxs,	  // in: BOL state, out: eol state
					P_OUT (RGTXTB)	rgtxtb,
					P_IO(DWORD)		dwReserved
					) PURE;

	// slim version of lex line since only interested in the plxs.
	//

	virtual UINT CbLexLine (
					LPCWSTR			ptchLine,
					UINT			cbLine,
					PLXS			plxs,	  // in: BOL state, out: eol state
					P_IO(DWORD)		dwReserved
					) PURE;
};

typedef CLex *	PLEX;

// Maximum length (not including \0 at end) of name returned by CLex::SzLanguage()
#define MAX_LANGNAMELEN (50)

