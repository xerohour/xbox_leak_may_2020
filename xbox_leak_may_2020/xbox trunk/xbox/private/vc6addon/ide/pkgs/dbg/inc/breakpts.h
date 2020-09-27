/**********************************************************************

	FILE:           BREAKPTS.H

	PURPOSE:        Defines and prototypes for the QCQP persistent
					breakpoint handler.

**********************************************************************/

#ifndef _BREAKPTS_H_
#define _BREAKPTS_H_

#include "cdebug.h"

#define INT int

#include "brkpt.h"

// Dependent upon od.h definitions!
#define	msgMaskMin		msgMaskWin
#define msgMaskMac		msgMaskDDE
#define msgSingleOnly	((UINT)(~msgMaskAll))
#define msgNone			0

// CV400/CANWIN context delimeters
#define OPENCXT         _T('{')
#define CLOSECXT        _T('}')

#define BP_LINELEADER '.'
#define BP_LINELEADER_OTHER '@'

// Mask optimization for dialog box
#define ENABLE_EDIT1    0x0001
#define ENABLE_EDIT2    0x0002
#define ENABLE_EDITL    0x0004
#define ENABLE_EDITW    0x0008
#define ENABLE_LABEL1   0x0010
#define ENABLE_LABEL2   0x0020
#define ENABLE_LABELL   0x0040
#define ENABLE_MSG      0x0080

#define ENABLE_BPLOC			(ENABLE_EDIT1 | ENABLE_LABEL1)
#define ENABLE_BPEXPRTRUE		(ENABLE_EDIT2 | ENABLE_LABEL2)
#define ENABLE_BPEXPRCHGD		(ENABLE_EDIT2 | ENABLE_LABEL2 | \
								 ENABLE_EDITL | ENABLE_LABELL)
#define ENABLE_BPLOCEXPRTRUE	(ENABLE_EDIT1 | ENABLE_LABEL1 | \
								 ENABLE_EDIT2 | ENABLE_LABEL2)
#define ENABLE_BPLOCEXPRCHGD	(ENABLE_EDIT1 | ENABLE_LABEL1 | \
								 ENABLE_EDIT2 | ENABLE_LABEL2 | \
								 ENABLE_EDITL | ENABLE_LABELL)
#define ENABLE_BPWNDPROCMSGRCVD	(ENABLE_EDIT1 | ENABLE_LABEL1 | \
								 ENABLE_EDIT2 | ENABLE_LABEL2)

// define for maximum line # we can set bp's on
#if defined (_WIN32)
#define cLineBPLim	0xffffL
#else
#define cLineBPLim	0x7fffL
#endif

typedef enum {
	ERR_NULL = 0,
	ERR_ACTION,
	ERR_LOC,
	ERR_PROC,
	ERR_EXPR,
	ERR_LEN,
	ERR_MSG,
	ERR_PASSCNT
} ERR_PARSE ;

#define AMB_STS_UNRESOLVED -1

typedef enum {
	BPNIL = -1,
	BPLOC = 0,
	BPLOCEXPRTRUE,
	BPLOCEXPRCHGD,
	BPEXPRTRUE,
	BPEXPRCHGD,
	BPWNDPROCMSGRCVD
}
BREAKPOINTTYPES;


typedef enum
{
	BPLOCFILELINE,		// Simple foo.c @ line
	BPLOCADDREXPR,		// Expression to parse (not line)
}
LOCATIONTYPES;


// The component parts of a CV400 {f,s,e} context

#define FUNC_NAME_LEN 150
typedef struct
{
	char Function[FUNC_NAME_LEN+1];	// or proc
	char Source[_MAX_PATH+2];		//    mod (+2 bytes for open/close quotes)
	char Exe[_MAX_PATH+2];			//    exe (+2 bytes for open/close quotes)
}
BPCONTEXT;

typedef BPCONTEXT _far *PBPCONTEXT;

// define the maximum length of bp commands
#define cbBpCmdMax	(sizeof(BPCONTEXT) + FUNC_NAME_LEN)

typedef struct
{
	int InitLine;	// Line number when BP first specified
	int CurLine;	// Current editor BP line number
	int Offset;		// Asm offset
}
BPLINEINFO;


typedef struct
{
	char AddrExpr[MAX_EXPRESS_SIZE];
}
BPADDREXPR;


typedef struct
{
	char WndProcName[FUNC_NAME_LEN+1];
}
BPWNDPROCEXPR;


typedef union
{
	BPLINEINFO 		l;
	BPADDREXPR 		ae;
	BPWNDPROCEXPR 	wpe;
}
BPLOCATION;


typedef struct
{
	BPCONTEXT 		Cxt;
	BPLOCATION 		Loc;
	LOCATIONTYPES 	LocType;
}
BPLOCATIONNODE;


// NB Contexts for expressions are treated as part of
// the expressions and left in the Expr string

typedef struct
{
	char 		Expr[MAX_EXPRESS_SIZE];
	int 		Len;				// used for expression changed
	BPCONTEXT 	Cxt;				// Cxt of expression
	BOOL 		UseCxt;				// TRUE if Cxt used in setting the bp
}
NORMALEXPRESSION;


typedef struct
{
	UINT MessageClass;
	UINT MessageNum;
}
WNDPROCEXPRESSION;


typedef union
{
	NORMALEXPRESSION ne;
	WNDPROCEXPRESSION we;
}
BPEXPRESSIONNODE;


typedef union
{
	UINT AllFlags;          // Flags bit fields
	struct
	{
		// Flags used only for storing breakpoint dialog info
		UINT DlgMarkAdd     : 1 ; // This breakpoint has been marked for addition
		UINT DlgMarkDel     : 1 ; // This breakpoint has been marked for deletion
		UINT DlgMarkEnable  : 1 ; // This breakpoint has been marked for enabling
		UINT DlgMarkDisable : 1 ; // This breakpoint has been marked for disabling

		// Miscellaneous other flags
		UINT fEnabled      : 1 ; // This breakpoint is enabled
		UINT fAmbig        : 1 ; // Set this flag for ambiguous
		UINT fExtInfo      : 1 ; // This BP needs extended info on display
		UINT fSymbolized   : 1 ; // Have we already tried to convert this
								 // BP node to a symbolic address?
		UINT fWarnedUnsupp : 1 ; // Have we already warned the user if this
								 // BP's type is unsupported on this platform?
		UINT fSqlBp        : 1 ; // SQL Specific breakpoints
	} f;
}
BREAKPOINTFLAGS;

#ifdef DEBUGGER_AUTOMATION
class CAutoBreakpoint;
#endif

typedef struct BREAKPOINTNODEtag
{
	BREAKPOINTTYPES 				Type;
	BPLOCATIONNODE  				bpl;
	BPEXPRESSIONNODE 				bpe;
	struct BREAKPOINTNODEtag _far *	Next;
	INT TMindex;
	BREAKPOINTFLAGS 				Flags;
	USHORT							cPass;

	// Breakpoint type (for BPLOC and BPLOCEXPRxxx aka LocatonNode)
	EBPT							ebpt;

	// Error during initialization, RESERVED FOR FUTURE USE (markbro)
	// Someday we will decide to queue up non-bound BPs at startup
	// and only display one dialog/message box instead of a seperate
	// one for each BP which didn't bind.  The error will be cached
	// here and cleared once the BP error is displayed
	UINT							idInitError;

	// This is a handle to the corresponding
	// CV400 breakpoint structure.  It is
	// only, (possibly), non-NULL while the
	// debuggee exists.
#ifdef _WIN32
	HBPI 							hbpi;
#else
	WORD 							hbpi;
#endif
	INT								iGroup;
#ifdef DEBUGGER_AUTOMATION
	CAutoBreakpoint *				pAutoBP;
#endif
}
BREAKPOINTNODE;

typedef BREAKPOINTNODE _far *PBREAKPOINTNODE;



// BREAKPOINTNODE access macros:

#define bpnNext(bpn)					((bpn).Next)
#define pbpnNext(pbpn)					((pbpn)->Next)

// BP type:
#define bpnType(bpn)					((bpn).Type)
#define pbpnType(pbpn)					((pbpn)->Type)

// BP location type:
#define bpnLocType(bpn)					((bpn).bpl.LocType)
#define pbpnLocType(pbpn)				((pbpn)->bpl.LocType)

// BP locations:
#define bpnLocCxt(bpn)					((bpn).bpl.Cxt)
#define pbpnLocCxt(pbpn)				((pbpn)->bpl.Cxt)
#define bpnLocCxtFunction(bpn)			((bpn).bpl.Cxt.Function)
#define pbpnLocCxtFunction(pbpn)		((pbpn)->bpl.Cxt.Function)
#define bpnLocCxtSource(bpn)			((bpn).bpl.Cxt.Source)
#define pbpnLocCxtSource(pbpn)			((pbpn)->bpl.Cxt.Source)
#define bpnLocCxtExe(bpn)				((bpn).bpl.Cxt.Exe)
#define pbpnLocCxtExe(pbpn)				((pbpn)->bpl.Cxt.Exe)
#define bpnFileName(bpn)				bpnLocCxtSource(bpn)
#define pbpnFileName(pbpn)				pbpnLocCxtSource(pbpn)
#define bpnInitLine(bpn)				((bpn).bpl.Loc.l.InitLine)
#define pbpnInitLine(pbpn)				((pbpn)->bpl.Loc.l.InitLine)
#define bpnCurLine(bpn)					((bpn).bpl.Loc.l.CurLine)
#define pbpnCurLine(pbpn)				((pbpn)->bpl.Loc.l.CurLine)
#define bpnOffset(bpn)					((bpn).bpl.Loc.l.Offset)
#define pbpnOffset(pbpn)				((pbpn)->bpl.Loc.l.Offset)
#define bpnAddrExpr(bpn)				((bpn).bpl.Loc.ae.AddrExpr)
#define pbpnAddrExpr(pbpn)				((pbpn)->bpl.Loc.ae.AddrExpr)
#define bpnWndProc(bpn)					((bpn).bpl.Loc.wpe.WndProcName)
#define pbpnWndProc(pbpn)				((pbpn)->bpl.Loc.wpe.WndProcName)

// BP expressions:
#define bpnExpression(bpn)				((bpn).bpe.ne.Expr)
#define pbpnExpression(pbpn)			((pbpn)->bpe.ne.Expr)
#define bpnExprLen(bpn)					((bpn).bpe.ne.Len)
#define pbpnExprLen(pbpn)				((pbpn)->bpe.ne.Len)
#define bpnExprCxt(bpn)					((bpn).bpe.ne.Cxt)
#define pbpnExprCxt(pbpn)				((pbpn)->bpe.ne.Cxt)
#define bpnExprCxtFunction(bpn)			((bpn).bpe.ne.Cxt.Function)
#define pbpnExprCxtFunction(pbpn)		((pbpn)->bpe.ne.Cxt.Function)
#define bpnExprCxtSource(bpn)			((bpn).bpe.ne.Cxt.Source)
#define pbpnExprCxtSource(pbpn)			((pbpn)->bpe.ne.Cxt.Source)
#define bpnExprCxtExe(bpn)				((bpn).bpe.ne.Cxt.Exe)
#define pbpnExprCxtExe(pbpn)			((pbpn)->bpe.ne.Cxt.Exe)
#define bpnMessageClass(bpn)			((bpn).bpe.we.MessageClass)
#define pbpnMessageClass(pbpn)			((pbpn)->bpe.we.MessageClass)
#define bpnMessageNum(bpn)				((bpn).bpe.we.MessageNum)
#define pbpnMessageNum(pbpn)			((pbpn)->bpe.we.MessageNum)
#define bpnExprUseCxt(bpn)				((bpn).bpe.ne.UseCxt)
#define pbpnExprUseCxt(pbpn)			((pbpn)->bpe.ne.UseCxt)

// BP flags
#define bpnMarkAdd(bpn)					((bpn).Flags.f.DlgMarkAdd)
#define pbpnMarkAdd(pbpn)				((pbpn)->Flags.f.DlgMarkAdd)
#define bpnMarkDel(bpn)					((bpn).Flags.f.DlgMarkDel)
#define pbpnMarkDel(pbpn)				((pbpn)->Flags.f.DlgMarkDel)
#define bpnMarkEnable(bpn)				((bpn).Flags.f.DlgMarkEnable)
#define pbpnMarkEnable(pbpn)			((pbpn)->Flags.f.DlgMarkEnable)
#define bpnMarkDisable(bpn)				((bpn).Flags.f.DlgMarkDisable)
#define pbpnMarkDisable(pbpn)			((pbpn)->Flags.f.DlgMarkDisable)
#define bpnAmbigBP(bpn)					((bpn).Flags.f.fAmbig)
#define pbpnAmbigBP(pbpn)				((pbpn)->Flags.f.fAmbig)
#define bpnOldStatus(bpn)				((bpn).Flags.f.DlgOldStatus)
#define pbpnOldStatus(pbpn)				((pbpn)->Flags.f.DlgOldStatus)
#define bpnBPTMindex(bpn)				((bpn).TMindex)
#define pbpnBPTMindex(pbpn)				((pbpn)->TMindex)
#define bpnEnabled(bpn)					((bpn).Flags.f.fEnabled)
#define pbpnEnabled(pbpn)				((pbpn)->Flags.f.fEnabled)
#define bpnBPExtInfo(bpn)				((bpn).Flags.f.fExtInfo)
#define pbpnBPExtInfo(pbpn)				((pbpn)->Flags.f.fExtInfo)
#define bpnSymbolIzed(bpn)				((bpn).Flags.f.fSymbolized)
#define pbpnSymbolized(pbpn)			((pbpn)->Flags.f.fSymbolized)
#define bpnWarnedUnsupported(bpn)		((bpn).Flags.f.fWarnedUnsupp)
#define pbpnWarnedUnsupported(pbpn)		((pbpn)->Flags.f.fWarnedUnsupp)
#define bpnSqlBp(pbpn)					((pbpn).Flags.f.fSqlBp)
#define pbpnSqlBp(pbpn)					((pbpn)->Flags.f.fSqlBp)

// CV400 breakpoint structure
// This is a handle to the linked list manager which
// when locked gives an LPBPI
#define bpnBPhbpi(bpn)					(bpn).hbpi
#define pbpnBPhbpi(pbpn)				(pbpn)->hbpi

// This macro checks for a virtual BP
// It's an existing BP that has not been bound at Startup...
#define bpnBPVirtual(bpn)				(bpnBPhbpi(bpn) == (HBPI)0)
#define pbpnBPVirtual(pbpn)				(pbpnBPhbpi(pbpn) == (HBPI)0)

// This macro checks if an ambiguous BP has been resolved
#define bpnBPResolved(bpn)				((bpn).TMindex > AMB_STS_UNRESOLVED)
#define pbpnBPResolved(pbpn)			((pbpn)->TMindex > AMB_STS_UNRESOLVED)

// This macro gets/sets an associated group number with a breakpoint
#define bpnBPGroup(bpn)					((bpn).iGroup)
#define pbpnBPGroup(pbpn)				((pbpn)->iGroup)

// This macro gets/sets pass count
#define bpnPassCount(bpn)				((bpn).cPass)
#define pbpnPassCount(pbpn)				((pbpn)->cPass)

// BP type if pbpnLocationNode
#define bpnEbpt(bpn)					((bpn).ebpt)
#define pbpnEbpt(pbpn)					((pbpn)->ebpt)

// Initialization error
#define bpnInitError(bpn)				((bpn).idInitError)
#define pbpnInitError(pbpn)				((pbpn)->idInitError)

// Utitlity macros
#define bpnFileLineNode(bpn) \
	(((bpnType(bpn) == BPLOC) ||\
	  (bpnType(bpn) == BPLOCEXPRTRUE) ||\
	  (bpnType(bpn) == BPLOCEXPRCHGD)) &&\
	 (bpnLocType(bpn) == BPLOCFILELINE))

#define pbpnFileLineNode(pbpn)			(bpnFileLineNode(*(pbpn)))

#define bpnLocationNode(bpn) \
	(((bpnType(bpn) == BPLOC) ||\
	  (bpnType(bpn) == BPLOCEXPRTRUE) ||\
	  (bpnType(bpn) == BPLOCEXPRCHGD)))

#define pbpnLocationNode(pbpn)			(bpnLocationNode(*(pbpn)))

#define bpnExpressionNode(bpn) \
	(((bpnType(bpn) == BPLOCEXPRTRUE) ||\
	  (bpnType(bpn) == BPLOCEXPRCHGD) ||\
	  (bpnType(bpn) == BPEXPRTRUE) ||\
	  (bpnType(bpn) == BPEXPRCHGD)))

#define pbpnExpressionNode(pbpn)		(bpnExpressionNode(*(pbpn)))


// Prototypes
BOOL PASCAL ParseCV400Location(
	LPSTR Location,
	PBREAKPOINTNODE Target);

BOOL PASCAL ParseQC25Location(
	LPSTR Location,
	PBREAKPOINTNODE Target);

LPSTR FindNonQuotedChar(LPSTR sz, UINT ch);

BOOL FFilenameNeedsQuotes(LPCSTR szFile);

BOOL PASCAL ExtractCV400Context(
	LPCTSTR StartContext,
	PBPCONTEXT Context);

void PASCAL FnNameFromHsym(
	HSYM	hsym,
	PCXT	pcxt,
	LPSTR	lpstrName);

BOOL PASCAL ParseExpression(
	LPCTSTR Expression,
	PBREAKPOINTNODE Target);

BOOL PASCAL ParseWndProc(
	LPSTR WndProc,
	PBREAKPOINTNODE Target);

// Breakpoint node procs:

PBREAKPOINTNODE PASCAL AddBreakpointNode(
	PBREAKPOINTNODE NewNodeData,
	BOOL fReset,
	BOOL fQuiet,
	BOOL fRestoring,
	LPINT lpiErr);

BOOL PASCAL DeleteBreakpointNode(int BreakpointNodeIndex);

BOOL PASCAL DeleteBreakpointNode(PBREAKPOINTNODE pbpn);

int PASCAL ClearCV400Breakpoints(void);

int PASCAL ClearBreakpointNodeList(void);


/***********************************************************************

    BREAKPOINT HANDLER MSVC API

***********************************************************************/
BOOL            PASCAL BHBindBPList(VOID);
VOID            PASCAL BHUnBindBPList(BOOL fPerformingENC = FALSE) ;
VOID			PASCAL BHRememberBPDLLs(VOID) ;
PBREAKPOINTNODE PASCAL BHFirstBPNode(VOID) ;
PBREAKPOINTNODE PASCAL BHLastBPNode(VOID) ;
PBREAKPOINTNODE PASCAL BHLastBPCreated(VOID) ;
BOOL            PASCAL BHCreateBPHandle(PBREAKPOINTNODE,BOOL,BOOL,LPINT) ;
VOID 			PASCAL BHSetLastBPNode(PBREAKPOINTNODE) ;
VOID			PASCAL BHGotoBP(PBREAKPOINTNODE) ;
BOOL			PASCAL BHCanGotoBP(PBREAKPOINTNODE) ;


/***********************************************************************

    BREAKPOINT FILE HANDLER MSVC API

***********************************************************************/
BOOL            PASCAL BFHShowCodeBP(PBREAKPOINTNODE) ;
BOOL            PASCAL BFHShowBPNode(PBREAKPOINTNODE) ;
VOID            PASCAL BFHShowAllBPNode(VOID) ;
VOID            PASCAL BFHResetShowAllBPNode(VOID) ;
INT             PASCAL BFHGetLineBPIndex(LPSTR,INT,LPINT,BOOL fCheckDisabled = FALSE, UINT *pWidgetFlag = 0) ;
BOOL 			PASCAL BFHFindDoc(LPCSTR, CDocument **);

void PASCAL AdjustBreakpointLines(
	const CString &strDocFileName, int StartLine, int NumberLines,
	BOOL fAdded, BOOL fLineBreak);

void PASCAL DeleteBreakpointLinesInDoc(
	const CString &strDocFileName);

void PASCAL CementBreakpointLinesInDoc(
	const CString &strDocFileName);

void PASCAL BuildCV400Location(
	PBREAKPOINTNODE pBreakpoint,
	PSTR LocSpec, UINT Len,
	BOOL FullPaths, BOOL ToLower, BOOL ToOem,
	BOOL Quote);

void PASCAL BuildCV400Expression(
	PBREAKPOINTNODE pBreakpoint,
	PSTR ExprSpec, UINT Len,
	BOOL FullPaths, BOOL ToLower, BOOL ToOem);

void PASCAL BuildCV400LocationMessage(
	PBREAKPOINTNODE pBreakpoint,
	PSTR LocSpec, UINT Len,
	BOOL FullPaths, BOOL ToLower, BOOL ToOem);

void PASCAL FormatDisplayBP(
	PBREAKPOINTNODE pBreakpoint,
	PSTR DisplayBP, UINT DisplayLen);

BOOL PASCAL ToggleLocBP(
	void);

enum BPSetOpt { bplset, bplclear, bpltoggle, bplfind};

BOOL PASCAL SetEditorLocBP(BPSetOpt, PBREAKPOINTNODE * ppbpn = NULL, int line = -1);

UINT GetBPLineState(
    VOID);

VOID PASCAL EnableDisableLocBP(
	BOOL fEnable);

VOID PASCAL DisableAllBreakpoints(
	VOID);

void PASCAL HighlightAllBPNodes(
	BOOL Set);

void PASCAL MakeCV400BPCmd(
	PBREAKPOINTNODE pBreakpoint,
	PSTR CV400BPCmd, UINT CmdLen);

void SaveStatusFile ( char * pszMakefile );

void LoadStatusFile ( char * pszMakefile );

BOOL PASCAL SetCV400Breakpoint(
	PBREAKPOINTNODE bpNode,
	BOOL SetAtStartUp,
	BOOL fQuiet,
	LPINT lpiErr);

BOOL PASCAL HighlightBPNode ( PBREAKPOINTNODE pBpNode, BOOL Set );

struct _pbp;

BOOL PASCAL LoopExeGetCV400BPPacket(
	PBREAKPOINTNODE bpNode,
	_pbp *pBPPacket);

BOOL BHFIsValidWindowForBP(void);
void UpdateBPInViews(void);
void CheckBPsAtRestart(void);

BOOL BHFTypeSupported( WORD wBPType );
BOOL BHFTypeEnabled( WORD wBPType );

BOOL LoadBPReferencedClasses(void);

#endif	// _BREAKPTS_H_
