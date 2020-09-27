#if !defined(STDAPP_H_INCLUDED_)
#define STDAPP_H_INCLUDED_

/*============================================================================
Microsoft Access

Microsoft Confidential.
Copyright 1992-1993 Microsoft Corporation. All Rights Reserved.

File: stdapp.h

Data types and macros useful for application development
============================================================================*/

#ifndef _STDAPP_H
#define _STDAPP_H

/*===========================================================================
	General Purpose Defines												    
============================================================================*/

/* this macro does nothing, but used for indenting */
#define block

#ifdef __cplusplus
#define EX_C extern "C" 
#else
#define EX_C 
#endif

#if (defined(W95DEF) || defined(ICAP))
#define LOCAL	// so that statics show up in the map file or ICAP profile
#else
#define LOCAL static
#endif
#ifndef NO_C_BUGS   // Can't have static __export because of C8 bug.
#define LOCALEXP
#endif

#include <windowsx.h>

// Turn off warnings for seeing both DLLIMPORT and DLLEXPORT.  The compiler (vc2.0/2.1)
// does the right thing, the stupid warning is just saying that it saw the DLLIMPORT in
// the header file, and the DLLEXPORT in the .c file.  -mgrier 1/11/95
#pragma warning(disable:4273)

#define HUGE
#if !defined(DAVINCI_CUL_USED)
#define EXPORT
#endif // !defined(DAVINCI_CUL_USED)
#define ACCEXPORT WINAPI
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)
#define _export
#define __export
#define _far
#define far
#define __far
#define _near
#define huge
#define _huge
#define _segment HANDLE
#define _loadds
#define _pascal
#define GetWindowValPortable(hwnd,ib) GetWindowLong(hwnd,ib)
#define SetWindowValPortable(hwnd,ib,val) SetWindowLong(hwnd,ib,val)
#define API
#define MAKEINTLONG(i) (i)
#define GETINTLONG(l) (l)

#define INLINE			_inline
#define EXTERN			extern
#if !defined(DAVINCI_CUL_USED)
#define PUBLIC
#define PRIVATE
#endif // !defined(DAVINCI_CUL_USED)

#define	MakeLong(l, h)	MAKELONG(l, h)
#define LowWord(l)		LOWORD(l)
#define HighWord(l)		HIWORD(l)

#define CwFromCch(cch)	(((cch) + 1) / 2)
#define CwFromCb(cb)	(((cb) + 1) / 2)
#define cwof(x)			((sizeof(x) + 1) / 2)

/* size of an st string in bytes (1st byte is number of characters) */
#define CbSt(st)		(*(st)+1)

#define ibWndSeg 0		/* Offset of window word containing the heap selector
						   for the associated object */

#define cchFilenameMax	MAX_PATH
#define cchNameMax		65	/* now a real hungarian max! so names can have only 64 chars (excluding null) */
#define cchFormPrefixMax	31	/* Max length of "Form."/"Report." in US or international */
#define cchProcNameMax 	(cbProcNameMax+1)
#define cchModNameMax	(cchNameMax + cchFormPrefixMax)	/* big enough for the largest CBF module name */
#define cchDocNameMax	cchModNameMax
#define cchStringMax	2048	/* big enough for the largest international string */
#define cchLtoaDecMax	12	/* big enough for ltoa with arg3 == 10 : "-2147483648" */
#define cchExprLim		2048	/* maximum length of unparsed expression */
#define cchKeyMax		257	/* registry key maximum */

// US lcid
#define LCID_US MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), SORT_DEFAULT)

/* coordinate space limits */
#define GDI_MIN			SHRT_MIN
#define GDI_MAX			SHRT_MAX
#define FIsGdiCoord(l)	(l >= GDI_MIN && l <= GDI_MAX)	/* needs limits.h */

/* count of characters for a possibly unicode static string */
#define cchof(_buf) (sizeof(_buf)/sizeof(TCHAR))

// The Error-Reporting Options... 
#define bitNull			0x0000 // Nothing; as if calling straight to Windows
#define bitErrJump		0x0001 // ErrJump on error; flag will be set to bring
							   // up an alert the next time we hit the idle loop
							   // unless bitNoDispFailed or bitErrMsg also set
#define bitErrMsg		0x0002 // Display an "Out of Memory" alert
#define bitErrJumpMsg	(bitErrJump | bitErrMsg)

EX_C PUBLIC BOOL FStkRoom();

/*****************************************************************

		Appearance codes

*****************************************************************/

#define appearNormal	0
#define appearRaised	1
#define appearSunken	2
#define appearEtched	3
#define appearShadow	4
#define appearChisel	5
#define appearMax		5


/*============================================================================
Constants for determining the current state of app execution

============================================================================*/
#define wAppBeginSession		0
#define wAppInLogonDialog		1
#define wAppRunning				2
#define wAppClosePending		3
#define wAppQueryExitApp		4
#define wAppQueryExitWindows	5
#define wAppExitApp 			6
#define wAppExitWindows			7

#define FInitting() (wAppState <= wAppInLogonDialog)
#define FQuitting()	(wAppState >= wAppExitApp)
/*
	Macros used to declared structures derived from each other.  In C++,
	uses derivation, but in C uses anonymous embedding.  Suppose an FI
	is the same as FOBJ, but with a based handle tacked on the end.

	H-file would read as follows:

	typedef struct FI
		Derived(FOBJ)
		CHAR BPP(segFx) pszName;
		EndDerived FI;

	Generated C++ code:

	typedef struct FI
		: public FOBJ
		{
		CHAR BPP(segFx) pszName;
		} FI;

	Generated C code:

	typedef struct FI
		{
		FOBJ;
		CHAR BPP(segFx) pszName;
		} FI;

*/
		
#ifdef __cplusplus

#define Derived(base)	: public base\
	{

#define Derived2(base1, base2)	: public base1, public base2\
	{

#else
#define Derived(base)	{\
	base;

#define Derived2(base1, base2)	{\
	base1;\
	base2;
	

#endif

#define EndDerived	}

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------
|  General Purpose Data types for Windows Apps							
--------------------------------------------------------------------------*/
typedef int 						ERR;
typedef unsigned char				BOOLB;
typedef _segment 					SEG;
typedef	char *						IB;
typedef char						DT[8];
typedef char						NUM[8];
typedef int							(*PFN)();
typedef long int					(*PFNL)();
typedef	int							CE;
typedef unsigned long 				ULONG;

/* BCS: CS based data */
// for the Mac, if ever #define BCS __declspec(allocate("_CODE"))
#define BCS const

/* BPS: based pointer self */
#define BPS *
#define PP BPS BPS

/* BP: based pointer (based on seg) */
#define BP(seg)	*
#define BPP(seg) BP(seg) BP(seg)
#define BPCS	BCS *
#define BPPCS	BPCS BPCS

#ifndef NO_C_BUGS   // Requiered due to C8 warning errors
#define BPPNULL ((VOID BPPV) NULL)
#endif

/* BP: based pointer (based on void) */
#define BPV *
#define BPPV **
#define BPIB void **

/* LBP: far pointer to a based pointer, based on the seg the far points to
   (i.e. perfect for values returned by PpbAlloc) */
#define LBP BPS FAR *

/* LBPs are now just pointers to pointers.  HeapOfPv returns the heap the 
	memory was allocated from (RegisterHeapPv MUST be called at allocation). */
#define SegOfLp(lp)	HeapOfPv(lp)
#define IbOfLp(lp)	((IB)lp)
#define FreeLbp(lbp)	FreePpb(HeapOfPv(lbp), lbp)
#define FreeLbpNil(lbp)	if (lbp!=NULL) FreeLbp(lbp)

/* primitive data types in upper case */
typedef int 			 INT;
typedef char 			 CHAR;
#ifdef LONG	// Replace the definition of LONG with a typedef
#undef LONG
#endif
typedef long 			 LONG;
typedef short 			 SHORT;
#define UNSIGNED		unsigned
#define CONST			const
#define REGISTER		register
#define DOUBLE			double
#define FLOAT			float

typedef union DPOINT
	{
	struct
		{
		int dx;
		int dy;
		};
	int rgdxy[2];
	} DPOINT;		/* Difference between PoinTs */

typedef union RECTD
	{
	struct
		{
		int left;
		int top;
		int dx;
		int dy;
		};
	struct
		{
//		struct
//			{
//			int x, y;
//			} topleft;
		POINT topleft;
		DPOINT dpt;
		};
	} RECTD;	/* ReCtangle holding dimension Difference */

#define	DxOfRc(prc)	((prc)->right - (prc)->left)
#define	DyOfRc(prc)	((prc)->bottom - (prc)->top)

/*------------------------------------------------------------------------
| API stub outs -- if there's one here, use the alternate
------------------------------------------------------------------------*/

#ifndef DAVINCI

#if !defined(CIRHDCMGR) && (!SOA)
/* Access: Use CreateDCC instead */
#undef CreateDC
#define CreateDC	dontusethisapi
/* Access: Use CreateICC instead */
#undef CreateIC
#define CreateIC	dontusethisapi
/* Access: Use CreateCompatibleDCC instead */
#define CreateCompatibleDC	dontusethisapi
/* Access: Use DeleteDCC instead */
#define DeleteDC	dontusethisapi
/* Access: Use SaveDCC instead */
#define SaveDC	dontusethisapi
/* Access: Use RestoreDCC instead */
#define RestoreDC	dontusethisapi
/* Access: Use SetMapModeC instead */
#define SetMapMode	dontusethisapi
/* Access: Use SelectObjectC instead */
#define SelectObject	dontusethisapi
/* Access: Use SelectRegionC instead */
#define SelectClipRgn	dontusethisapi
/* Access: Use SetTextColorC instead */
#define SetTextColor	dontusethisapi
/* Access: Use SetBkModeC instead */
#define SetBkMode	dontusethisapi
/* Access: Use CreateMetaFileC instead */
#undef CreateMetaFile
#define CreateMetaFile	dontusethisapi
/* Access: Use CloseMetaFileC instead */
#define CloseMetaFile	dontusethisapi
/* Access: Use EnumMetaFileC instead */
#define EnumMetaFile	dontusethisapi
/* Access: Use GetDCC instead */
#define GetDC	dontusethisapi
/* Access: Use ReleaseDCC instead */
#ifndef GRID_DLL
#define ReleaseDC	dontusethisapi
#endif	// GRID_DLL
/* Access: Use GetWindowDCC instead */
#define GetWindowDC	dontusethisapi
/* Access: Use BeginPaintC instead */
#define BeginPaint	dontusethisapi
/* Access: Use EndPaintC instead */
#define EndPaint	dontusethisapi
#endif

//#if !defined(CIRSETFOCUS) && (!SOA)
/* Access: Use SetKeyFocus instead */
//#define SetFocus	dontusethisapi
//#endif

#if !defined(CIRDEFMDICHILDPROC) && (!SOA)
/* Access: Use DefCirMDIChildProc instead */
#undef DefMDIChildProc
#define DefMDIChildProc	dontusethisapi
#endif

#if !defined(CIRDIALOG) && (!SOA)
/* Access: Use CeDlgBoxParam instead */
#undef DialogBox
#define DialogBox	dontusethisapi
#undef DialogBoxIndirect
#define DialogBoxIndirect	dontusethisapi
/* Access: Use CeDlgBoxParam instead */
#undef DialogBoxParam
#define DialogBoxParam dontusethisapi
#undef DialogBoxIndirectParam
#define DialogBoxIndirectParam dontusethisapi
#endif /* CIRDIALOG */

#endif	// DAVINCI

/*------------------------------------------------------------------------
| Win16 Compatibility types -- DO NOT USE FOR NEW CODE!
------------------------------------------------------------------------*/

#pragma pack(1)
typedef struct RECT16
	{
	SHORT left;
	SHORT top;
	SHORT right;
	SHORT bottom;
	} RECT16;
typedef struct POINT16
	{
	SHORT x;
	SHORT y;
	} POINT16;
#pragma pack()

#define POINT16TOPOINT(pt16,pt)	{ \
								(pt).x = (pt16).x; \
								(pt).y = (pt16).y; \
								}
								
#define POINTTOPOINT16(pt,pt16)	{ \
								(pt16).x = (SHORT)(pt).x; \
								(pt16).y = (SHORT)(pt).y; \
								}

#define RECT16TORECT(rc16,rc)	{ \
								(rc).left = (rc16).left; \
								(rc).right = (rc16).right; \
								(rc).top = (rc16).top; \
								(rc).bottom = (rc16).bottom; \
								}
#define RECTTORECT16(rc,rc16)	{ \
								(rc16).left = (SHORT)(rc).left; \
								(rc16).right = (SHORT)(rc).right; \
								(rc16).top = (SHORT)(rc).top; \
								(rc16).bottom = (SHORT)(rc).bottom; \
								}

/*------------------------------------------------------------------------
| other common hungarianized Window's types
|
------------------------------------------------------------------------*/
#define HBR HBRUSH
#define HBMP HBITMAP
#define HCRS HCURSOR

/*------------------------------------------------------------------------
| Null values for special types
------------------------------------------------------------------------*/
#define segNil		((SEG) 0)
#define hdcNil		(HDC)0
#define hfontNil	(HFONT)0
#define hwndNil		(HWND)0
#define	hbrNil		(HBR)0
#define hmenuNil	(HMENU)0
#define hNil		(HANDLE)0
#define lpNil		(VOID FAR *)0
#define hpenNil		(HPEN)0
#define hrgnNil		(HRGN)0
#define hiconNil	(HICON)0
#define hcrsNil		(HCURSOR)0
/*------------------------------------------------------------------------
| CsConst and Errjump defines
------------------------------------------------------------------------*/
#define CsConst(type) BCS type

/* Our dialogs go in cs space */
#define DLG_CONST BCS

/* alias for standard C setjmp function */
#define SetJmp(arg)	setjmp((_JBTYPE*)arg)
#define ENV jmp_buf
#define PENV ENV *

#define FValidSeg(seg)	(seg != segNil)
#define FImplies(f1,f2) (!(f1)||(f2))
#define FOREVER for(;;)

#include <setjmp.h>

EXTERN  ENV *penvError;

	/* macro to declare the environment vars for you.  requires ending ; */
#define DeclareEjd	\
	ENV	envSave;	\
	ENV	*  penvSave

/* segCur==segDefault in preparation for SetJmp call */
#define SetEjd(ejd)		{		\
		Assert(GetCurrentThreadId() == dwMainThreadId);	\
		PushEjd((EJD)(VOID FAR *)ejd);	\
		penvSave = penvError;	\
		penvError = &envSave;	\
		}

#define ResetEjd(ejd)	{			\
		Assert(EjdPop() == (EJD)(VOID FAR *)(ejd));	\
		penvError = penvSave;		\
		}

// The following macros are for convenient and structured handling
// of exceptions. They can be used as follows:
//
//  ErrBegin
//      ... main body of code containing possible ErrJumps
//      MUST NOT "goto" or "return" out of here - use ErrGoto or ErrReturn!
//		don't use "break" or "continue" to get out of here either.
//		control now transfers to line after "ErrEnd"
//  ErrRecover
//      error recovery code - OK to use goto/return. Error code is available
//      in "errEJ" variable.
//		ErrJump(errEJ);	// Propagate exception (optional)
//      return errEJ;   // Return error code (optional)
//		(if no ErrJump/return, code would continue executing after "ErrEnd")
//  ErrEnd
//
//	The ErrRecover section and the code after it are optional, but if used,
//  it must come AFTER the main body of code. The ErrEnd is required.
//  If there is no ErrRecover section, when an exception occurs, the error
//  is trapped and execution resumes after the ErrEnd.
//
// To surround code that is not supposed to contain any ErrJumps, use
//  SafeBegin
//      ... code supposedly not ErrJumping
//      MUST NOT "goto" or "return" out of here - use SafeGoto or SafeReturn!
//		don't use "break" or "continue" to get out of here either.
//  SafeEnd
// This will cause an Assert if the code does indeed ErrJump.
//
//	DON'T EVER JUMP INTO OR OUT OF ONE OF THESE BLOCKS BY USING goto, return,
//	break, continue, or something else I haven't thought of. That will
//	cause bugs galore!
//
//	Also, DON'T PUT ANY FUNCTION CALLS THAT MAY ErrJump INTO THE
//	ErrGoto/ErrReturn MACROS: The errors won't be trapped!
//
//
//	Example code (illustrates use of ErrReturn):
//
//	// Allocate memory; return NULL if out of memory; propagate error
//	//	if any other error is detected (not a very useful routine, but...)
//	PV PvAllocExample
//	(
//	unsigned cb;	// Count of bytes to allocate
//	)
//	{
//		PV pv;
//
//		ErrBegin
//			pv = PvAlloc(cb);
//			ErrReturn(pv);
//		ErrRecover
//			if (errEJ == idaNoMem)
//				ErrReturn(NULL);
//			else
//				ErrJump(errEJ);	// Propagate unexpected error
//		ErrEnd
//	}
//
// or, alternatively:
//
//	// Allocate memory; return NULL if out of memory; propagate error
//	//	if any other error is detected (not a very useful routine, but...)
//	PV PvAllocExample
//	(
//	unsigned cb;	// Count of bytes to allocate
//	)
//	{
//		PV pv;
//
//		ErrBegin
//			pv = PvAlloc(cb);
//		ErrRecover
//			if (errEJ == errOutOfMemory)
//				pv = NULL;
//			else
//				ErrJump(errEJ);	// Propagate unexpected error
//		ErrEnd
//
//		return (pv);
//	}

#define ErrBegin \
    { \
        ENV env;       \
        PENV penvSave;  \
        ERR errEJ;                \
        BOOL fErrRecover = FALSE;	\
		PushEjd((EJD)(VOID FAR *)&env);	\
        penvSave = penvError;   \
        penvError = &env;       \
        if ((errEJ = SetJmp(&env)) == 0)  \
        {

#define ErrGoto(lbl) \
            { \
	        Assert(penvError == &env || penvError == penvSave);  \
			Assert(EjdPop() == (EJD)(VOID FAR *)(&env));	\
			penvError = penvSave; \
			goto lbl; \
			}

#define ErrReturn(val) \
            { \
	        Assert(penvError == &env || penvError == penvSave);  \
			Assert(EjdPop() == (EJD)(VOID FAR *)(&env));	\
			penvError = penvSave; \
			return(val); \
			}

#define ErrRecover \
        }                       \
        else                    \
        {                       \
            Assert(errEJ != 0);  \
			Assert(EjdPop() == (EJD)(VOID FAR *)(&env));	\
			fErrRecover = TRUE;		\
            penvError = penvSave;   \

#define ErrEnd \
        }                       \
        Assert(penvError == &env || penvError == penvSave);  \
		if (!fErrRecover)	\
			Assert(EjdPop() == (EJD)(VOID FAR *)(&env));	\
        penvError = penvSave;   \
    }

#ifdef DEBUG

#define SafeBegin   \
        ErrBegin

#define SafeGoto(lbl) \
            { \
			Assert(EjdPop() == (EJD)(VOID FAR *)(&env));	\
            penvError = penvSave; \
            goto lbl; }

#define SafeReturn(val) \
            { \
			Assert(EjdPop() == (EJD)(VOID FAR *)(&env));	\
            penvError = penvSave; \
            return (val); }

#define SafeExit \
	{ \
	Assert(EjdPop() == (EJD)(VOID FAR *)(&env));	\
	penvError = penvSave; \
	return; }

#define SafeEnd \
        ErrRecover  \
            Assert(FALSE); \
        ErrEnd

#else   // not DEBUG

#define SafeBegin   {

#define SafeGoto(lbl) \
            { \
            goto lbl; }

#define SafeReturn(val) \
            { \
            return val; }

#define SafeExit \
		{ \
		return; }

#define SafeEnd }

#endif  // not DEBUG


/*============================================================================

TrapEjd will execute the code specified by fn and set fejd to TRUE
if an errjump occured during the execution of fn otherwise fejd will
contain false.  Use this in functions where you need to call code that
errjumps on failure but you need a simple TRUE/FALSE success indicator
============================================================================*/

#define TrapEjd(ejd, fejd, fn)	{	\
		SetEjd(ejd); fejd = TRUE;	\
		if (SetJmp(penvError) == 0) \
			{fn; fejd = FALSE;}		\
		ResetEjd(ejd);		\
		}

#define TrapEjdCpp(fejd, fn)	{	\
		penvSave = penvError;	\
		penvError = &envSave;	\
		fejd = TRUE;	\
		if (SetJmp(penvError) == 0) \
			{fn; fejd = FALSE;}		\
		penvError = penvSave;		\
		}
	
/*------------------------------------------------------------------------
| Blt macros
------------------------------------------------------------------------*/
#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

/* the following Blt* macros (memcpy) DON'T handle src/dest overlap
	- use Blt*O (memmove) macros below if overlap is possible */
#define BltB(pFrom, pTo, cb)	memcpy(pTo, pFrom, cb)
#define BltBF(lpFrom, lpTo, cb)	memcpy((char FAR *)(lpTo), (char FAR *)(lpFrom), (cb))
#define BltW(pFrom, pTo, cw)	memcpy(pTo, pFrom, (cw)*sizeof(UINT))
#define BltWF(lpFrom, lpTo, cw)	memcpy(lpTo, lpFrom, (cw)*sizeof(UINT))

// PUBLIC DWORD BltBH(CONST VOID _huge *, VOID _huge *, DWORD);
#define BltBH(pFrom, pTo, cb) memcpy(pTo, pFrom, cb)

/* the following Blt*O macros (memcpy) DO handle src/dest overlap
   but are slower than the Blt* macros above */
#define BltBO(pFrom, pTo, cb)	memmove(pTo, pFrom, cb)
#define BltBFO(lpFrom, lpTo, cb)	memmove((char FAR *)(lpTo), (char FAR *)(lpFrom), (cb))
#define BltWO(pFrom, pTo, cw)	memmove(pTo, pFrom, (cw)*sizeof(UINT))
#define BltWFO(lpFrom, lpTo, cw)	memmove(lpTo, lpFrom, (cw)*sizeof(UINT))


#define FillB(b, pb, cb)		memset((char *)(pb), (b), (cb))
#define FillBF(b, lpb, cb)		memset((char FAR *)(lpb), (b), (cb))

/* the following string macros are Hungarian-ized and list the source
   string before the destination string (the opposite of C-runtime routines) */
#define CpySz(szFrom, szTo)				lstrcpy(szTo, szFrom)
#define CpyLsz(lszFrom, lszTo)			lstrcpy(lszTo, lszFrom)
#define CpySzCch(szFrom, szTo, cch)		lstrcpyn(szTo, szFrom, (size_t)cch)
#define CpyLszCch(lszFrom, lszTo, cch)	lstrcpyn(lszTo, lszFrom, (size_t)cch)

#define CatSz(szFrom, szTo)		lstrcat(szTo, szFrom)
#define CatLsz(lszFrom, lszTo)	lstrcat(lszTo, lszFrom)

#define CpySt(stFrom, stTo)		memcpy(stTo, stFrom, *(stFrom)+1)
#define CpyLst(lstFrom, lstTo)	memcpy(lstTo, lstFrom, *(lstFrom)+1)

#ifndef DAVINCI
#define CchSz(sz)		((UINT)lstrlen (sz))
#endif	// DAVINCI

#define CchLsz(lsz)		((UINT)lstrlen(lsz))

/*------------------------------------------------------------------------
| Internationalization and Misc string handling macros
------------------------------------------------------------------------*/
/* This function can be made into a function later */
/* CONSIDER: Do we want these as macros? */
#define lMin(l1, l2) ((l1 < l2) ? l1 : l2)
#define lMax(l1, l2) ((l1 < l2) ? l2 : l1)
#define FInRange(w, wLo, wHi)	((unsigned)((w) - (wLo)) <= (unsigned)((wHi) - (wLo)))

/* Character functions. */
#define FIsDigitCh(ch) (ch >= '0' && ch <= '9')
#define ToLowerCh(ch) ((char)LOWORD((LONG)AnsiLower((LPSTR)MAKELONG(ch,0))))
#define ToUpperCh(ch) ((char)LOWORD((LONG)AnsiUpper((LPSTR)MAKELONG(ch,0))))
#define ToLowerLpsz(lpsz) AnsiLower((LPSTR)(lpsz))
#define ToUpperLpsz(lpsz) AnsiUpper((LPSTR)(lpsz))

#define FIsAlphaCh(ch) IsCharAlpha(ch)
#define FIsAlNumCh(ch) IsCharAlphaNumeric(ch)
#define FIsLowerCh(ch) IsCharLower(ch)
#define FIsUpperCh(ch) IsCharUpper(ch)
#define ToLowerLrgch(lrgch, cch) AnsiLowerBuff(lrgch, cch);
#define ToUpperLrgch(lrgch, cch) AnsiUpperBuff(lrgch, cch);

/* CONSIDER: making this a function? */
#define FIsShort(l)	(l >= SHRT_MIN && l <= SHRT_MAX)	/* needs limits.h */
#define FIsWord(dw)	(dw <= USHRT_MAX)					/* needs limits.h */

#ifdef DEBUG			/* DEBUG build macros follow ... */
/*------------------------------------------------------------------------
| Assertion failure support
| Define the module name once.  Include at the top of
| any module which has an assertion check
------------------------------------------------------------------------*/

#ifndef DAVINCI
#define SZASSERT static char szAssert[] = __FILE__
#define SZASSERTH static char szAssertH[] = __FILE__
PUBLIC BOOL AssertFail(LPCSTR lszMsg, LPCSTR lszFile, UINT wLine);
#else
#define szAssert "Grid DLL"
#define szAssertH "Grid DLL"
#endif	// DAVINCI

#ifndef DAVINCI
#ifdef Assert
#undef Assert
#endif
#endif	// DAVINCI

#ifndef DAVINCI
#define Assert(exp) ( (exp) ? 0 : AssertFail(#exp, szAssert, __LINE__) )
#define AssertH(exp) ( (exp) ? 0 : AssertFail(#exp, szAssertH, __LINE__) )

#define AssertMsg(exp, szMsg) ( (exp) ? 0 : \
							AssertFail(szMsg, szAssert, __LINE__) )
#else
#define AssertH(exp)
#define AssertMsg(exp, szMsg)
#endif	// DAVINCI

#define SideAssert(f) Assert(f)
#if !defined(DAVINCI_CUL_USED)
#define Ensure(exp, exp2) {if ((exp) != (exp2)) Assert(FALSE);}
#define EnsureNE(exp, exp2) {if ((exp) == (exp2)) Assert(FALSE);}
#define Implies(f1, f2)         Assert((f1) ? (f2) : fTrue)
#endif // !defined(DAVINCI_CUL_USED)

#else
#define SZASSERT
#define SZASSERTH

#ifndef DAVINCI
#define Assert(exp) (0)
#endif	// DAVINCI

#define AssertH(exp) (0)
#define AssertMsg(exp, szMsg) (0)
#define ErrMsg(sz)
#define SideAssert(f)		(f)

#ifndef DAVINCI
#define Ensure(exp, exp2)	exp;
#define EnsureNE(exp, exp2)	exp;
#endif	// DAVINCI

#define Implies(f1, f2)

#endif /* !DEBUG */

#ifndef RETAIL		/* needed for DEBUG & TESTOPT */

#define DisarmRFS()	DisarmAllRfs()
#define RestoreRFS() RestoreAllRfs()

#else	/* RETAIL */

#define DisarmRFS()
#define RestoreRFS()
#define	SkipFail(id)

#endif /* RETAIL */

/*------------------------------------------------------------------------
|
| Common useful prototypes
|
------------------------------------------------------------------------*/

PUBLIC  VOID ErrJump(INT);

/*------------------------------------------------------------------------
|
| Common useful external declarations
|
------------------------------------------------------------------------*/
extern DLLIMPORT HINSTANCE hInstApp;
extern HDC hdcApp;
extern HWND hwndApp;
extern UINT wAppState;
extern CHAR szNull[];
extern UINT wWinVer;
extern DWORD lPlatform;
extern HINSTANCE hInstIntlDLL;
extern BOOLB fInIdle;
extern DWORD dwMainThreadId;
extern CHAR szAppName[];
extern CHAR *szVisAppName;	// WARNING: Compiles wrong if this is declared [] instead of * !
extern HKEY hKeyProfile;
extern const CHAR szSettingKey[];

#define FIsChicago() (lPlatform == VER_PLATFORM_WIN32_WINDOWS)

// FAdvancedShell() was commented out--I've re-included it (tomva)
#define FAdvancedShell() (LOBYTE(LOWORD(wWinVer)) >= 0x04)

/*============================================================================
Common useful typedefs
============================================================================*/
typedef INT HSBAR;

/*============================================================================

	Component Message Range Definitions

============================================================================*/
	/* include them rather than define them here so we can make
	 * an include file of them very easily.  (can't make an include of stdapp).
	 */
#include "cmpmsg.h"
#ifdef __cplusplus
}
#endif

#endif // _STDAPP_H

#endif
