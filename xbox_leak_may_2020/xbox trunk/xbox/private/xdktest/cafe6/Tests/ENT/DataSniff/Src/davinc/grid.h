#if !defined(GRID_H_INCLUDED_)
#define GRID_H_INCLUDED_

/*============================================================================
Microsoft Access

Microsoft Confidential.
Copyright 1992 Microsoft Corporation. All Rights Reserved.

Component: Grid

File: Public include file for the grid component

#include "dspconst.h"	// for TXTINFO for grid.h
#include "collect.h"	// for EC for grid.h
============================================================================*/

#ifndef _GRID_H
#define _GRID_H

//#include "es.h"
#include "stdapp.h"
#include "dspconst.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef	DWORD	ICOL;
typedef	DWORD	IROW;
typedef	DWORD	ICLRW;		/* used for either columns or rows */

#define icolNil     ((ICOL) ~0)
#define icolBndNil	((ICOL) ~1)
#define irowNil     ((IROW) ~0)
#define iclrwNil    ((ICLRW) ~0)
#define wicolNil   	icolNil
#define wirowNil    irowNil

/* segGrid declaration */
extern	struct _gi far *pgi;
extern SEG segGrid;

typedef struct GRID 
	{ 
	DWORD	grfGa;			/* grid attributes (see below) */ 
	DWORD	grfGaEx;			/* extended grid attributes (see below) */ 
		/* pointer to client's callback routine (required): */
	DWORD	(FAR *lpfnlClientProc)(HWND hwnd, UINT wm, UINT wParam, DWORD lParam);
	HWND	hwndParent;			/* handle to parent window (required) */
	RECTD		rcd;				/* location in parent's client coordinates */
	BYTE	crowFixed, ccolFixed;	/* number of fixed rows/columns */ 
	INT		crow, ccol;			/* grid dimensions (incl. fixed rows/cols) */ 
	INT		dxtColDft;			/* default column width (in TWIPS) */
	INT		dytRowDft;			/* default row height (in TWIPS) */
	DWORD	dwClient;			/* client info to be passed back via GCL_INIT */
	UINT	cfCustom;			/* 432: custom CF to use in place of text & csv */
	} GRID;		/* GRID information */ 

typedef	GRID FAR *LPGRID;

typedef struct GV 
	{
	BRSHINFO	brshBk;					/* background brush info */ 
	HBR		hbrGlns, hbrGlnsFixed;		/* grid-line brushes */ 
	UINT appearLines;	// raised, sunken, normal
	} GV;			/* Grid Visuals */ 

typedef GV FAR *LPGV;

/* Space for a long (4 bytes) is allocated with the window, for client use. */
#define ibGridClient	(ibWndSeg+sizeof(SEG))

#ifdef __cplusplus
#pragma warning(disable:4200)       // zero-size array
#endif // __cplusplus

typedef struct _gca
	{
	UINT	grfGca;			/* grid cell attribute flags (see defs below) */ 
	TXTINFO	txtinfo;		/* color, font, alignment */
	BRSHINFO brshinfo;		/* brush, color, fPatterned, fTransparent */
	BDRINFO	bdrinfo;		/* rcpMarg, appearance */

	UINT	hkcl;			/* control type identifier */

		/* grid cell data: */
	UINT	cbKt;			/* amount of extra control data in rgbKt */ 
	BYTE	rgbKt[];		/* grid control definition (size varies) */
	} GCA;					/* Grid Cell Attributes */ 

typedef GCA FAR *LPGCA;
typedef GCA BP(segGrid)		PGCA;
typedef GCA BPP(segGrid)	PPGCA;
typedef PPGCA BP(segGrid)	PPPGCA;

#define PktOfPpgca(ppgca)			PktOfPgca(*(ppgca))
#define	PktOfPgca(pgca)				(&(pgca)->rgbKt)

/* NOTE: returned lpkt is only valid until the next allocation in seg */
#define	LpktOfPgktd(pgktd) \
		(PktOfPgca(*(GCA LBP)(pgktd)->ppgca))
#define	LpktOfPgcad(pgcad) \
		(PktOfPgca(*(GCA LBP)(pgcad)->ppgca))

#define	FreePpgcaInSeg(ppgca,seg)	FreePpb(seg,ppgca)

typedef struct _gc 
	{
	PPGCA	ppgca;			/* grid cell attributes */

		/* grid cell data: */
	UINT	cbData;			/* amount of extra cell data in rgbData */ 
	BYTE	rgbData[];		/* client definable data (variable size) */ 
	} GC;					/* Grid Cell */ 

typedef GC FAR *LPGC;
typedef GC BP(segGrid)		PGC;
typedef GC BPP(segGrid)		PPGC;
typedef PPGC BP(segGrid)	PPPGC;

#ifdef __cplusplus
#pragma warning(default:4200)       // zero-size array
#endif // __cplusplus

/* Grid Attributes */
#define fGaLargeValues	0x00000001	/* allow cells to hold > cchGridMax bytes
									   of data; implies !fGaHoldData */
#define fGaSelRc		0x00000002	/* allow rectangular selection of multiple cells (not supported in V1) */
#define fGaSelDisc		0x00000004	/* allow disontiguous selections */
#define fGaSelRow		0x00000008	/* allow row selections */
#define fGaInsRow		0x00000010	/* allow insertion of new rows */
#define fGaClipRow		0x00000020	/* allow cut/copy/paste of rows */
#define fGaDelRow		0x00000040	/* allow deletion of rows */
#define fGaDragRow		0x00000080	/* allow row dragging (=reorder=move=cut/paste) */
#define fGaSelCol		0x00000100	/* allow column selections */
#define fGaInsCol		0x00000200	/* allow insertion of new columns */
#define fGaClipCol		0x00000400	/* allow cut/copy/paste of columns */
#define fGaDelCol		0x00000800	/* allow deletion of columns */
#define fGaDragCol		0x00001000	/* allow column dragging (=reorder=move=cut/paste) */
#define fGaSizeCol		0x00002000	/* allow column width dragging */
#define fGaSizeRow		0x00004000	/* allow row height dragging */
#define fGaScrollVert	0x00008000	/* allow vertical scrolling */
#define fGaDispVert		0x00010000	/* draw vertical grid lines */
#define fGaScrollHorz	0x00020000	/* allow horizontal scrolling */
#define fGaDispHorz		0x00040000	/* draw horizontal grid lines */
#define	fGaBordered		0x00080000	/* give the grid window a border */
#define	fGaDiffWidths	0x00100000	/* columns can have different widths */
#define	fGaDiffHeights	0x00200000	/* rows can have different heights */
#define fGaHoldData		0x00400000	/* grid will store the textual data */
#define fGaDynamicRows	0x00800000	/* grid must query client for # of rows */

									// WARNING - If you use fGaNeverActive for any other type 
									// ( other then dropdown combos )
									// you need to add a new style to id dropdown combos for WGridEvnet WM_MOUSEMOVE
#define fGaNeverActive	0x01000000	/* grid can never get the Windows focus
									   (used for dropdown combos) */
									
#define fGaHookKeys		0x02000000	/* give client a shot at all key/char msgs */
#define fGaFixed3D		0x04000000	/* display fixed cells as 3D-raised */
#define	fGaListbox		0x08000000	/* look and act like a single-select listbox */
#define fGaColumnar		0x10000000	/* set for grids having a column for each
									   element (e.g. query design) rather than
									   a row; may affect keyboard interface */
#define	fGaStaticRows	0x20000000	/* maintain the same total number of rows
									   even when rows of data are deleted */
#define fGaPinLastRow	0x40000000	/* don't scroll bottom row to top - 
									   assumed if fGaListbox, only valid if
									   !fGaDiffHeights */
#define	fGaAllowUndo	0x80000000	/* have the grid store Undo state */

/* extended attributes follow */
#define fGaExDragRc		0x00000001	/* allow drag-drop of rectangular selection */
#define fGaExLboxMult	0x00000002	/* if fGaListbox and fGaSelDisc, then a 
										listbox uses LBS_EXTENDEDSEL selection 
										model, if this is also set, 
										LBS_MULTIPLESEL model is used. */
#define fGaExAnimateRows 0x00000004	/* animate row deletes/inserts */
#define fGaExDiffOutside 0x00000008 /* erase outside of grid with desk color */
#define fGaExThumbtips	 0x00000010	/* Draw thumbtips when vertical thumb dragged */
#define fGaExCustomCf	 0x00000020	/* client names the clipboard format to be used for text-based ops (see 432) */
#define fGaExNoLiveScroll	0x00000040	/* Don't scroll while dragging thumb */
#define fGaExSunken			0x00000080  /* Grid is to be drawn with WS_EX_CLIENTEDGE style */
/* Grid Cell Attributes */
#define fGcaHoldData	0x0002	/* grid will store the textual data */
#define fGcaRefillData	0x0004	/* refill to get real data after every
								   user-commit (to show formatting, etc.) */
#define fGcaNoClickThru	0x0008	/* when unselected cell clicked, select it but
							 	   don't pass click through to cell proc */
#define fGcaDisabled	0x0010	/* cell cannot receive the focus */	
#define fGcaLocked		0x0020	/* cell can get focus but cannot be changed */							 
#define fGcaNotGrayed	0x0040	/* cell text not grayed even though disabled */							 
#define fGcaForceGetData 0x0080	/* always do GCL_GETDATA even if after DataMac
									ie. do not optimize for after end of data
									(useful for selection columns) */
#define fGcaForceStatic	0x0100	/* Pass fDsStatic to DispSzPb, even though
									it's not really a static item */							 
#define fGcaAuxButton	0x0200	/* It's an aux button -- don't paint if its not
								   enabled, don't let the user get here using
								   movement keys. */
#define fGcaCantTab		0x0400	/* cell can receive the focus via mouse click, 
									but not from key strokes */	

/* Grid Column/Row Attributes (also stored in grfGca) */
#define	fGcaNoSizeCol	0x1000	/* allow click & drag column resizing */
#define	fGcaNoSizeRow	0x2000	/* allow click & drag row resizing */
#define	fGcaNoSelCol	0x4000	/* allow column selection */
#define	fGcaNoSelRow	0x8000	/* allow row selection */

/* The following structures (GCID, GCAD, GKTD, GCLS, GROWS, GSIZ, GMID,
   GFUNC, GPOS, GSCR, GRPR) are used only as parameter blocks, never
   allocated or stored */

typedef struct GCID
	{
	ICOL	icol;
	IROW	irow;
	} GCID;		/* Grid Cell IDentifier */

typedef GCID FAR *LPGCID;

struct GCMD
	Derived(GCID)

	UINT wHt;		// hit region
	EndDerived;

typedef struct GCMD GCMD;
typedef GCMD FAR *LPGCMD;

/* return flags for WGridHitTest: */
#define wHtNone		0x0000
#define wHtTopGcol	0x0100
#define wHtSelGcol	0x0200
#define wHtSizeGcol	0x0300
#define wHtTopGrow	0x0400
#define wHtSelGrow	0x0500
#define wHtSizeGrow	0x0600
#define wHtSelGc	0x0800
#define wHtInGc		0x1000
#define wHtOutside	0x2000
#define wHtDisabled	0x4000
#define wHtDragSel	0x8000

struct GCAD
	Derived(GCID)

		/* this field is only used for G_SETCELLATTRS */
	UINT	grfSet;			/* mask of bits describing which attributes should
							   be set */

		/* this field is only used for G_GETCELLATTRS */
	SEG seg;				/* Grid's seg which PPGCA refers to */

		/* these fields may be used for either GET or SET */
	PPGCA	ppgca;		  	/* holds GCA allocated in grid's seg */

	GCA		gca;			/* at the end as its variable-length */
	EndDerived;	/* Grid Cell Attribute Descriptor */
	
typedef struct GCAD GCAD;
typedef GCAD FAR *LPGCAD;

typedef struct _gktd
	{
	SEG		seg;			/* returned grid's seg in which to allocate PPB
							   members of the desired KT */
	PPGCA	ppgca;			/* returned GCA containing a default control
							   instance of type hkcl */
	} GKTD;		/* Grid Kontrol Type Descriptor */

typedef GKTD FAR *LPGKTD;


struct GCLS
	Derived(GCID)
	union
		{
		DWORD	cb;		   /* length of data on a SETDATA */
		DWORD	cbBuf;	   /* size of buffer on a GETDATA */
		};
	union
		{
		CHAR	FAR *lsz;	/* hope to get rid of null termination */
		BYTE	FAR *lpb;
		};

	DWORD	cbActual;		/* client need only fill this on a GCL_GETDATA */
	union
		{					/* client should ensure these are initially 0: */
		LONG	ibStart;	/* starting point when getting data
							 * (G_GETDATA / GCL_GETDATA)*/
		BOOL	fNotDone;	/* flag used in Saving large values (GCL_SETDATA) */
		};

	/* these 2 fields are used on a GCL_GETDATA msg to get text formatting
	   info from clients that support it; all others can simply ignore them */
	struct TFIB *ptfib;	/* pointer to Text Formatting Info Block */
	INT dx;
	BOOL fUseFmt;		/* retrieved formatted data? */
	EndDerived;		/* Grid CLient String */
	
typedef struct GCLS GCLS;
typedef GCLS FAR *LPGCLS;

struct GCPRIV
	Derived(GCID)
	UINT	wId;			/* information identifier */

	VOID	FAR *lpv;		/* buffer pointer: use is wId-dependent */	

	EndDerived;		/* grid cell private info request */

typedef struct GCPRIV GCPRIV;
typedef GCPRIV FAR *LPGCPRIV;

typedef struct _grows
	{
	BOOL	fMoreRows;	/* client sets to FALSE if return value is final */
	UINT	wNum;		/* either wNumMin or wNumFinal (see below) */
	IROW	irowMin;	/* if wNumMin, return value must be either > irowMin
						   or final */
	UINT	crowScrollBelow;	/* if fMoreRows, scrollbar range max will be
								   set this far beyond the number of data rows
								   returned  */
	} GROWS;	/* used only for GCL_GETNUMROWS */

typedef GROWS FAR *LPGROWS;

/* possible values of GROWS.wNum: */
#define	wNumMin		1
#define	wNumFinal	2
#define	wNumReset	4

typedef struct _gsiz
	{
	union
		{
		ICOL	icol;
		IROW	irow;
		ICLRW	iclrw;
		};
	union
		{
		UINT	ccol;
		UINT	crow;
		UINT	cclrw;
		};
	union
		{
		UINT	dxtNew;
		UINT	dytNew;
		UINT	dtNew;
		};
	union
		{	/* when resizing multiple rows/cols, this will hold the old size
			   of the last one */
		UINT	dxtOld;
		UINT	dytOld;
		UINT	dtOld;
		};
	} GSIZ;	/* used only for G_SHOWCOLS/ROWS, GCN_SETCOLWIDTH/ROWHEIGHT */
	
typedef GSIZ FAR *LPGSIZ;

typedef struct _gmid
	{
	union
		{
		ICOL	icolSrc;
		IROW	irowSrc;
		ICLRW	iclrwSrc;
		};
	union
		{
		ICOL	icolDst;
		IROW	irowDst;
		ICLRW	iclrwDst;
		};
	union
		{
		LONG	ccol;
		LONG	crow;
		LONG	cclrw;
		};
	} GMID;	/* used for G/GCL_QRYMOV/INSERT/DELETECOLS/ROWS */
	
typedef GMID FAR *LPGMID;

struct _gfunc
	Derived(GCID)	/* cell identifier, use if wParam != 0 */
	UINT	(*pfn)(struct KPB *, LPVOID);
	LPVOID	lpvArg;
	EndDerived;		/* used only for G_CALLFUNC */

typedef struct _gfunc GFUNC;
typedef GFUNC FAR *LPGFUNC;

struct _gpos
	Derived(GCID)	/* cell (icol, irow) containing ptc, returned by grid */
	POINT		ptc;	/* point in client coordinates of the grid */
	EndDerived;		/* used only for G_GCIDOFPOS */

typedef struct _gpos GPOS;
typedef GPOS FAR *LPGPOS;

typedef	struct RCG
	{	/* like RECT, this rectangle doesn't include icolRight & irowBot */
	ICOL	icolLeft;
	IROW	irowTop;
	ICOL	icolRight;		
	IROW	irowBot;
	} RCG;	/* ReCtangle of Grid coordinates */

typedef RCG FAR		*LPRCG;

struct RCGSEL
	Derived(RCG)
	BYTE	grfSelTyp;
	BYTE	grfSelAnchor;
	EndDerived;			/* ReCtangular chunk of Grid SELection */

typedef struct RCGSEL RCGSEL;
typedef RCGSEL FAR	*LPRCGSEL;

typedef struct _gscr
	{
	INT		iFirst;
	INT		iLast;
	INT		iMult;	/* Actual Range = indicated range * iMult */
	BOOL	fHideScrollbar;
	} GSCR;		/* Grid SCRoll range */

typedef GSCR FAR	*LPGSCR;

typedef struct _gkpb
	{
	UINT (*pfnClient)(struct KPB *, LPVOID);
	struct KPB *pkpb;
	LPVOID lpv;
	UINT *pwRet;
	} GKPB;		/* used only by GCL_KPBCALL */

typedef GKPB FAR	*LPGKPB;

typedef struct _gnil
	{
	ICOL icol;
	IROW irow;
	CHAR FAR *lpch;
	UINT cch;
	} GNIL;		/* used only by GCN_NOTINLISTEVENT */

// Return values for GCN_NOTINLISTEVENT -- duplicated in ktcombo.h
//#define DATA_ERRCONTINUE	0
//#define DATA_ERRDISPLAY		1
//#define DATA_ERRADDED		2

typedef GNIL FAR	*LPGNIL;

typedef struct _gwpb
	{
	UINT (*pfnWpbHandler)(struct _wpb *);
	struct _wpb *pwpb;
	UINT *pwRet;
	} GWPB;		/* used only by GCL_WPBCALL */

typedef GWPB FAR	*LPGWPB;

struct GRPR
	Derived(RCG)				/* contains cell range to be printed */
	BOOLB	fPrintFixedCols;	/* if TRUE, fixed columns will be printed */
	BOOLB	fPrintFixedRows;	/* if TRUE, fixed rows will be printed */
	EndDerived;			/* GRid Print Range; used only by GCL_PRINT */

typedef struct GRPR GRPR;
typedef GRPR FAR	*LPGRPR;

/* types of grid selection (note that components of the grid selection
   (RCGSELs) must have grfSelTyp == fSelGcol or fSelGrow (fSelGc & fSelAll
   won't be supported in V1)): */
#define	bSelNil		0x00
#define	fSelGc		0x01
#define	fSelGcol	0x02
#define	fSelGrow	0x04
#define fSelAll		0x08

/* the overall selection (pgi->bSeltyp) can also contain these flags */
#define fSelComplex	0x10	/* contains mix of Gc, Gcol & Grow */
#define fSelDiscon	0x20	/* multiple components of Gc, Gcol or Grow */

/* anchor points: */
#define fSelLeft	0x01
#define fSelRight	0x02
#define fSelTop		0x04
#define fSelBot		0x08

/* flags for GridPaint, DrawGcs: */
#define	fDgPrint	0x0001
#define	fDgWipeData	0x0002
#define	fDgTryClip	0x0004
#define fDgNoGridLn	0x0008

typedef	HANDLE	HGDATA;

#define	cchGridMax	1024	/* maximum string length in a HoldData grid */	
#define	crowGridDft	 200	/* default no. of rows if client specifies -1 */
#define	crowScrollDft	100	/* for grids with dynamic no. of rows assume 
							   this many past irowDataMac unless client
							   overrides via GCL_GETNUMROWS msg */

/* structure for the G_GRIDLINES message */
typedef struct _glns
	{
	UINT appearLines;
	UINT grfLines;
	COLORREF crLines;
	UINT grfSet;
	} GLNS, *LPGLNS;

/* flags for grfLines */
#define fGlnNone	0
#define fGlnHoriz	1
#define fGlnVert	2
#define fGlnBoth	3
#define fGlnV2		0xFF	// take setting from v2.0

/* flags for grfSet */
#define fSetAppear	0x0001
#define fSetLines	0x0002
#define fSetColor	0x0003
#define fSetAll		(fSetAppear | fSetLines | fSetColor)

/* bits for GCL_KEYMSG */
#define	bitfFromKt		0x0001
#define bitfNoAccel		0x0002
#define bitfOnlyAccel	0x0004

/* Grid CLient messages:

Sent to the Grid CLient callback by the grid; the client is responsible for
displaying alerts before returning error values to the grid.

GCL_INIT: Guaranteed to be the first message sent to the client proc, sent
	right after the window has been initialized
 Criteria:
 wParam: HWND of new grid window
 lParam: DWORD client passed to HwndGridCreate in lpgrid->dwClient
 Return: 0L if successful, error != 0 if unsuccessful (will cause
	HwndGridCreate to fail/ErrJump)

GCL_GETDATA: Gets the cell string from the client
 Criteria: !fGaHoldData, !fGcaHoldData
 wParam: 
 lParam: LP to GCLS in DS pointing to icol, irow, pb & cbMax of buffer to
 	copy string into, and data offset ibStart; user should return the actual
 	size of its data (which might be > than cbMax) in cbActual
 Return: 0L if successful, error != 0 if unsuccessful
 	NOTE: If buffer not large enough, client should return error with
 		  required size in gcls.cb.

GCL_SETDATA: Tries to commit the modified cell string to the client
 Criteria: 
 wParam:
 lParam: LPGCLS pointing to icol, irow, buffer containing string, size
 Return: 0L if successful, != 0 if unsuccessful or disallowed by client
	NOTE: If the client saves the data that the grid was about to save, then
	  they should return idaInternalWarnSaveData to the grid to let it know
	  that the save succeeded (and thus no error should be reported) but that
	  the grid doesn't need to do any more work.

GCL_QUERYFOCUS: Request to set the focus to the specified cell. Sent before
	any action is taken. Don't send a G_SETFOCUS msg here to have the
	currency moved elsewhere; instead just update LPGCID->icol/irow.
	NOTE: this msg is sent when a disabled cell is clicked on to allow the
	client to set the focus elsewhere as a result. It is also sent when the
	cell becoming current already is (though GCL_SETFOCUS won't be)
 Criteria: 
 wParam: UINT *pgrfGcSet; if wGcSetCur, only changes currency; if wGcSetCaret
 		 also sets caret; if wGcSetFocus also tries to move input focus.
 		 Client may modify this.
 lParam: LPGCID specifying icol, irow; client may modify this to cause a
 	different cell to get the focus.
 Return: 0L if allowed; != 0 to disallow (client should display error)

GCL_SETFOCUS: Attempt to set the focus to the specified cell.  Sent after
	contents of current cell have been committed.
 Criteria: 
 wParam: if wGcSetCur, only changes currency; if wGcSetCaret also sets caret;
		 if wGcSetFocus also tries to move input focus
 lParam: LPGCID specifying icol, irow
 Return: 0L if successful; != 0 if unsuccessful (client should display error)

GCL_MOUSEACTIVATE: Notify client that a grid window has been clicked on,
	giving it the opportunity to preempt the BUTTONDOWN
 Criteria: 
 wParam: mouse msg # (from WM_MOUSEACTIVATE)
 lParam: LPGCID specifying icol, irow where the click occurred
 Return: 0L by default; != 0 to eat the ensuing WM_?BUTTONDOWN msg

GCL_BEGINEDIT: Pre-emptable notification that a cell's value has changed
 Criteria: 
 wParam: 
 lParam: LPGCID specifying icol, irow
 Return: 0L if allowed; != 0 to disallow (client should display error)

GCL_QUERYDROPOBJECT: ask client whether it is willing to accept a drop
 Criteria: 
 wParam:
 lParam: LPGRDROP containing DROP, ICOL, IROW
 Return: 0 to decline object drop, != 0 to accept

GCL_DROPOBJECT: inform client that an object is being dropped on it
 Criteria: 
 wParam:
 lParam: LPGRDROP containing DROP, ICOL, IROW
 Return: 0 if object drop fails, != 0 if successful

GCL_GETNUMROWS: ask client for current, minimum or final # of rows
 Criteria: fGaDynamicRows
 wParam: 
 lParam: GROWS FAR *: client sets fMoreRows to FALSE if # returned is final;
 	wNum is wNumFinal if final # required, wNumMin if minimum # required
 	(if # returned is less than the irowMin it must be final
 	 i.e. fMoreRows must be FALSE).
 	Client can also set lpgrows->crowScrollBelow to the desired number
 	of rows between the returned row count and the scrollbar range maximum.
 Return: DWORD containing the # of rows

GCL_KEYMSG: give client a chance to intercept a key msg (gets called for
	every WM_CHAR or WM_KEYDOWN message that will otherwise go to the
	grid's current control.
	CLIENTS: how are you using this? what would you like to see in lParam?
 Criteria: 
 wParam: WPB * describing the message
 lParam: Information bits, bitfFromKt if key msg is being handled by a KT's 
	 wndproc, bitfNoAccel if client should look for accelerators, bitfOnlyAccel 
	 if client should ONLY look for accelerators
 Return: 0 to let message go to control, != 0 to intercept

GCL_MOUSEMSG: give client a chance to intercept a mouse message
 Criteria: 
 wParam: WPB * describing the message
 lParam: LPGCMD containing icol, irow clicked on
 Return: 0 to let message go to control, != 0 to intercept

GCL_LBOXSEL: The selection in a listbox has changed.
 Criteria: fGaListbox
 wParam: TRUE if the focus is in the listbox (focus rect displayed around
 	the selection), FALSE otherwise
 lParam: IROW describing the newly selected row
 Return: 0L if allowed; != 0 to disallow

(CONSIDER) GCL_LBOXTRACK: The selection in a listbox is tracking (mouse button still
	down) over a row

GCL_STARTTRACK: The user has initiated a mouse-tracking operation (e.g. row,
	column, cell selection, or row/column resizing).
 Criteria: 
 wParam: the msop code of the mouse operation being initiated
 lParam: LPGCID describing the column, row or cell being selected or sized;
		 if only a row or col is being used, the other value in the gcid will
		 be set to icolNil or irowNil, respectively.
 Return: 0L if allowed; != 0 to disallow


NOTE: the following 6 messages allow the grid client to modify the parameters
	pointed to by ((LPGMID) lParam); if this is desired, the client should
	indicate success by returning 0, as a non-zero return value preempts the
	operation completely.

GCL_QRYINSCOLS: User is inserting grid columns
 Criteria: 
 wParam: TRUE if initiated via a G_INSERTCOLS, FALSE if from UI
 lParam: LPGMID containing icolDst, ccol to insert; see NOTE above
 Return: 0L to allow, != 0 to disallow

GCL_QRYINSROWS: User is inserting grid rows
 Criteria: 
 wParam: TRUE if initiated via a G_INSERTROWS, FALSE if from UI
 lParam: LPGMID containing irowDst, crow to insert; see NOTE above
 Return: 0L to allow, != 0 to disallow

GCL_QRYDELCOLS: User is deleting grid columns
 Criteria: 
 wParam: TRUE if initiated via a G_DELETECOLS, FALSE if from UI
 lParam: LPGMID containing icolSrc, ccol to delete; see NOTE above
 Return: 0L to allow, != 0 to disallow

GCL_QRYDELROWS: User is deleting grid rows
 Criteria: 
 wParam: TRUE if initiated via a G_DELETEROWS, FALSE if from UI
 lParam: LPGMID containing irowSrc, crow to delete; see NOTE above
 Return: 0L to allow, != 0 to disallow

GCL_QRYMOVCOLS: User is moving grid columns
 Criteria: 
 wParam: TRUE if initiated via a G_MOVECOLS, FALSE if from UI
 lParam: LPGMID containing icolSrc, ccol & icolDst to move between;
 	see NOTE above
 Return: 0L to allow, != 0 to disallow

GCL_QRYMOVROWS: User is moving grid rows
 Criteria: 
 wParam: TRUE if initiated via a G_MOVEROWS, FALSE if from UI
 lParam: LPGMID containing irowSrc, crow & irowDst to move between;
 	see NOTE above
 Return: 0L to allow, != 0 to disallow

GCL_SCROLLRANGE: notify client that a scroll bar's range is being set;
	client can alter the scroll range
 wParam: LoByte: scroll bar to range (SB_HORZ or SB_VERT)
 		 HiBYte: TRUE to redraw scrollbar
 lParam: LPGSCR to iMin, iMac defining range
 Return: 0 to allow grid to do it, != 0 if operation performed or preempted
 	by the client

GCL_SCROLLPOS: notify client that a scroll bar's thumb postion is being set;
	client can alter this position.
 wParam: LoByte: scroll bar to range (SB_HORZ or SB_VERT)
 		 HiBYte: TRUE to redraw scrollbar
 lParam: (UINT FAR *) pointing to ithumb position being set to
 Return: 0 to allow grid to do it, != 0 if operation performed or preempted
 	by the client

GCL_BEGINUNDO: let client preempt grid taking ownership of the Undo buffer
 wParam: undo code (see undo.h)
 lParam: The ids for the undo menu item.
 Return: 0 to let grid own the Undo buffer, > 0 if client already called
 	BeginUndoRecord for this undoable action.
(CONSIDER) letting client return < 0 to avoid Undo recording completely

GCL_RECORDUNDO: let client preempt grid recording an undo atom.
 wParam: undo code: one of ungrOpEditCell, ungrOpRowInsert, ungrOpRowDelete,
 			ungrOpRowMove, ungrOpCut, ungrOpResize
 lParam: 
 Return: 0 to let grid record an undo atom, != 0 grid will not record atom

GCL_EMPTYUNDO: let client preempt grid emptying the Undo buffer; in this case
	the client should call EmptyUndo to free this undoable action with the
	same HWND it passed to BeginUndoRecord.
 wParam: 
 lParam: 
 Return: 0 to let grid own the Undo buffer, > 0 if client calls	EmptyUndo itself

GCL_SELCHANGE: The current column or row selection has changed
 Criteria: 
 wParam: grfSelTyp (see bSel* #define's above)
 lParam: LPRCGSEL containing newly added selection rectangle if
 	wParam != bSelNil, 0L if wParam == bSelNil
 Return: 0 to allow, != 0 to preempt. 
 	NOTE: the client can't preempt collapsing the selection

GCL_PRINT: Let the client change the area to be printed
 Criteria: 
 wParam: 
 lParam: LPGRPR containing the grid range that will be printed; the client can
 	modify this
 Return: 0 to allow, != 0 to preempt printing altogether

GCL_INSERTROWS: Insert grid rows, Sent even when client instigated.
 Criteria: 
 wParam: 
 lParam: LPGMID containing irowDst, crow to insert
 Return: 0L to allow, < 0 to disallow

GCL_DELETEROWS: Delete grid rows. Sent even when client instigated.
 Criteria: 
 wParam: 
 lParam: LPGMID containing irowSrc, crow to delete
 Return: 0L to allow, < 0 to disallow

GCL_MOVEROWS: Move grid rows. Sent even when client instigated.
 Criteria: 
 wParam: 
 lParam: LPGMID containing irowSrc, crow & irowDst to move between
 Return: 0L to allow, < 0 to disallow

 **Note on GCL MoveRows/Cols:
	When the GCL Move message is sent, irowSrc is the source row/col
	to start moving from, and irowDst is the destination row/col to
	move to.  However, when the GCL notification message is sent
	this information is historical.  That is to say, if a row
	is moved down, from row 2 to row 6, at GCL_QRYMOVROWS time
	irowSrc is 2 and irowDst is 6.  But when the GCL_MOVEROWS message
	is sent, the move has already occurred, so row 6 is now row 5, because
	row 2 is gone. 	Nonetheless, the value of irowDst is still 6.
	Note too that if the client is calling AdjCci at the time that
	the GCL is sent, this will work correctly, since AdjCci corrects
	for this behavior.  Also, there is no such problem if a row/col
	is moved up.

GCL_GETROB:
 Criteria: 
 wParam:
 lParam: LPGCID pointing to icol
 Return: != 0L if successful, != 0 if unsuccessful

GCL_GETIVRCONTEXT:
 Criteria: 
 wParam:
 lParam:
 Return: >= 0L if successful, ivrNil (-1) if unsuccessful

GCL_GETCCHTEXTMOST: get limit on number of chars in given cell
 Criteria: 
 wParam:
 lParam: LPGCID specifying icol, irow
 Return: 0xfffffff for unlimited, 0L for use grid default, max number of
	characters acceptable by client for this control

GCL_GETPRIVINFO: control request for private info from client
 Criteria: 
 wParam:
 lParam: LPGCPRIV specifying icol, irow, info request, buffer pointer
 Return: != 0L if succesful, 0 if unsuccessful 

GCL_KPBCALL: helper to set up the form state, call the specific client
	routine, and reset the form state back before return.
 Criteria: 
 wParam:
 lParam: LPGKPB specifying the client routine, pkpb, lpv, and pwRet.
 Return: != 0L if client handles, 0 if grid should call the routine itself 
	If client handles, *pwRet is filled in with routine's return value. 

GCL_WPBCALL: helper to set up the form state, call the specific WPB handler,
	and reset the form state back before return.
 Criteria: 
 wParam:
 lParam: LPGWPB specifying the client routine, pwpb,and pwRet.
 Return: != 0L if client handles, 0 if grid should call the routine itself.
	If client handles, *pwRet is filled in with routine's return value. 

GCL_PREPAINT: Sent just before the grid handles a WM_PAINT message
	This gives the client an opportunity to do something before the grid
	paints or to preempt the paint and simply erase.
 wParam:
 lParam:
 Return: 0 to continue with paint, != 0 to erase the background but avoid
 	the paint (so a GCN_POSTPAINT won't be sent)

GCL_REMOTEDATA: Sent to ask if the specified column's data comes from a
	remote source
 wParam: 
 lparam: iCol to check
 Return: TRUE if data is remote, FALSE otherwise

GCL_EXPANDCELL: Sent to ask if expand cell capability is on
 wParam: 
 lparam: iCol to check
 Return: 0 if expand cell capability is off, != 0 if expand cell capability
	is off

GCL_GETCOLTYP: Sent to ask the coltyp of the specified column
 wParam: 
 lparam: iCol to check
 Return: coltyp (unsigned long)

GCL_GETORSCONTEXT:
 wParam:
 lParam:
 Return: orsContxt of the current form

GCL_GETHINST:
 wParam:
 lParam:
 Return: hinst of the current form

GCL_GRIDCURSOR:
 wParam: msop indicating potential grid cursor
 lParam: 0
 Return: 0 to allow cursor, != 0 to disallow

GCL_LOCKBYTES: Get an ILockBytes interface from the grid container
 wParam: VOID ** to store the interface in
 lParam: GCPRIV *, holding icol/irow and wId as BOOL for fWrite
 Return: idaSuccess, or error code

GCL_AUTOCORRECT: Get whether a column can autocorrect
 wParam:
 lParam: icol of column to get info on
 Return: TRUE to allow, FALSE to disallow
 
The following messages are notifications sent from the grid to the client:

GCN_INSERTCOLS: Insert grid columns. Sent even when client instigated.
 Criteria: 
 wParam: 
 lParam: LPGMID containing icolDst, ccol to insert
 Return: ignored (can't be disallowed/rolled back)

GCN_DELETECOLS: Delete grid columns, Sent even when client instigated.
 Criteria: 
 wParam: 
 lParam: LPGMID containing icolSrc, ccol to delete
 Return: ignored (can't be disallowed/rolled back)

GCN_MOVECOLS: Move grid columns. Sent even when client instigated.
 Criteria: 
 wParam: 
 lParam: LPGMID containing icolSrc, ccol & icolDst to move between
 Return: ignored (can't be disallowed/rolled back)

GCN_SETCOLWIDTH: The user has changed the specified column's width.
	If the new value is 0, grid will treat as hidden rather than resized.
 Criteria: fGaSizeCol
 wParam:
 lParam: LPGSIZ containing icol, ccol, dxtNew, dxtOld of last col sized
 Return: 

GCN_SETROWHEIGHT: The user has changed the specified row's height.
	If the new value is 0, grid will treat as hidden rather than resized.
 Criteria: fGaSizeRow
 wParam: 
 lParam: LPGSIZ containing irow, crow, dytNew, dytOld of last row sized
 Return: 

GCN_ACTIVATE: Notify the client of a grid's (de)activation via a WM_SETFOCUS
	or WM_KILLFOCUS message.
 Criteria: 
 wParam: FALSE if being deactivated, TRUE if being activated
 lParam: 
 Return: 


(CONSIDER) GCN_ENTERQUERYDROP: sent when object first dragged over client

(CONSIDER) GCN_EXITQUERYDROP: sent when object stops being dragged over client

GCN_SCROLLBAR: Sent when the grid adds or removes scroll bars
	The client may want to adjust its size, since the addition or removal
	of scroll bars causes the grid to become smaller or larger.
 Criteria: 
 wParam: Scroll bar added (SB_HORZ or SB_VERT)
 lParam: 0 = removed; 1 = added
 Return: Ignored

GCN_FREEZE: Sent when rows or columns are frozen or unfrozen.
 Criteria: 
 wParam: TRUE: rows; FALSE: columns
 lParam: irowMac - the last row that is now frozen
 Return: Ignored

GCN_POSTPAINT: Sent just after the grid handles a WM_PAINT message; will not
	be sent if the client preempted the paint via GCL_PREPAINT.
	This gives the client an opportunity to do something after the grid
	paints.
 wParam:
 lParam:
 Return: Ignored

GCN_FOCUSSET: Sent after the focus has been set to the specified cell,
	only if the focus has actually moved to a different cell.
 wParam:
 lParam: LPGCID specifying icol, irow
 Return: Ignored

GCN_UNDOSTATCHANGE: Notification from a control that its Undo status has changed
 Criteria: 
 wParam:
 lParam: 
 Return: Ignored

GCN_RECURSIVESAVE: Notification to client that grid is about to fail a
 recursive attempt to save a value.  Client can use this notification to
 put up appropriate error mesage.
 wParam:
 lParam:
 Return: Ignored

GCN_SELCHANGED: Notification to client that the grid has finished changing the
  selection and that it has updated all internal structures (so that G_GETSEL
  will return the current selection)
 wParam: grfSelTyp (see bSel* #define's above)
 lParam: LPRCGSEL containing newly added selection rectangle if
 	wParam != bSelNil, 0L if wParam == bSelNil
 Return: Ignored

GCN_NOTINLISTEVENT: Notification to client that a combo match has failed.
 wParam: 0
 lParam: LPGNIL containing information about failed match
 Return: DATA_ERRCONTINUE, DATA_ERRDISPLAY, DATA_ERRADDED

GCN_THUMBTRACK: Sent to client while tracking thumb
 wParam: TRUE if vertical scrollbar, FALSE if horizontal
 lParam: Row that would be scrolled to if thumb released
 Return: Ignored


*/

#define GCL_GETDATA			(WM_GRIDSTART + 0)
#define GCL_SETDATA			(WM_GRIDSTART + 1)
#define GCL_SETFOCUS		(WM_GRIDSTART + 2)
#define	GCL_MOUSEACTIVATE	(WM_GRIDSTART + 3)
#define	GCL_BEGINEDIT		(WM_GRIDSTART + 4)
#define GCL_QUERYFOCUS		(WM_GRIDSTART + 5)
#define GCL_INIT			(WM_GRIDSTART + 6)
#define GCL_QUERYDROPOBJECT	(WM_GRIDSTART + 7)
#define GCL_DROPOBJECT		(WM_GRIDSTART + 8)
#define GCL_GETNUMROWS		(WM_GRIDSTART + 9)
#define GCL_KEYMSG			(WM_GRIDSTART + 10)
#define GCL_QRYINSCOLS		(WM_GRIDSTART + 11)
#define GCL_QRYINSROWS		(WM_GRIDSTART + 12)
#define GCL_QRYDELCOLS		(WM_GRIDSTART + 13)
#define GCL_QRYDELROWS		(WM_GRIDSTART + 14)
#define GCL_QRYMOVCOLS		(WM_GRIDSTART + 15)
#define GCL_QRYMOVROWS		(WM_GRIDSTART + 16)
#define GCL_LBOXSEL			(WM_GRIDSTART + 17)
#define GCL_STARTTRACK		(WM_GRIDSTART + 18)
#define GCL_SELCHANGE		(WM_GRIDSTART + 19)
#define GCL_MOUSEMSG		(WM_GRIDSTART + 20)
#define GCL_SCROLLRANGE		(WM_GRIDSTART + 21)
#define GCL_SCROLLPOS		(WM_GRIDSTART + 22)
#define GCL_BEGINUNDO		(WM_GRIDSTART + 23)
#define GCL_EMPTYUNDO		(WM_GRIDSTART + 24)
#define	GCL_GETPCONTROL		(WM_GRIDSTART + 25)
#define GCL_PRINT			(WM_GRIDSTART + 26)
#define	GCL_GETIVRCONTEXT	(WM_GRIDSTART + 27)
#define GCL_GETCCHTEXTMOST	(WM_GRIDSTART + 28)
#define GCL_GETPRIVINFO		(WM_GRIDSTART + 29)

#define GCN_SETCOLWIDTH		(WM_GRIDSTART + 30)
#define GCN_SETROWHEIGHT	(WM_GRIDSTART + 31)
#define GCN_UNDOSTATCHANGE	(WM_GRIDSTART + 32)
#define GCN_INSERTCOLS		(WM_GRIDSTART + 33)
#define GCL_INSERTROWS		(WM_GRIDSTART + 34)
#define GCN_DELETECOLS		(WM_GRIDSTART + 35)
#define GCL_DELETEROWS		(WM_GRIDSTART + 36)
#define GCN_MOVECOLS		(WM_GRIDSTART + 37)
#define GCL_MOVEROWS		(WM_GRIDSTART + 38)
#define GCN_ENDEDIT			(WM_GRIDSTART + 39)
#define GCN_SCROLLBAR		(WM_GRIDSTART + 40)
#define	GCN_ACTIVATE		(WM_GRIDSTART + 41)
#define	GCN_FREEZE			(WM_GRIDSTART + 42)
#define GCL_PREPAINT		(WM_GRIDSTART + 43)
#define GCN_FOCUSSET		(WM_GRIDSTART + 44)
#define GCN_POSTPAINT		(WM_GRIDSTART + 45)

#define	GCL_KPBCALL			(WM_GRIDSTART + 46)

#define GCL_RECORDUNDO		(WM_GRIDSTART + 47)

#define GCL_WPBCALL			(WM_GRIDSTART + 48)

#define GCL_REMOTEDATA		(WM_GRIDSTART + 49)

#define GCL_EXPANDCELL		(WM_GRIDSTART + 50)

/* GCN_RECURSIVESAVE and GCN_SELCHANGED is defined below, because we're
of acceptable values here. */

/* Grid messages (start at WM_GRIDSTART + 50):

Sent to the Grid window proc by the grid client:

G_GETDATA: Retrieve the text from the grid
			(side effect: if the grid cell is dirty, force a commit.)
 Criteria: fGaHoldData
 wParam: 
	wGetDataNormal		- Standard GETDATA behavior
	wGetDataNoCommit	- Dont commit even if the cell is dirty
 lParam: LPGCLS pointing to icol, irow, buffer length, buffer to
 	copy string into. NOTE: Make sure to set ibstart to be the
	correct value. Random values (esp. with the grid holding data)
	can get nasty!
 Return: 0L if successful, errVal otherwise (e.g. OOM)

G_SETDATA: Initialize the text in the grid
 Criteria: fGaHoldData
 wParam:
	wSetDataNormal		- Standard SETDATA behavior
	wSetDataUpdScroll	- Update scrollbars immediately as necessary
	wSetDataTransient	- Data is "transient". Dont update DataMac
 		 NOTE: optimize by passing wSetDataNormal and sending a
 		 	G_UPDATE(wUpdScroll) msg after all G_SETDATA msgs are sent.
 lParam: LPGCLS pointing to icol, irow, string length (cb), and buffer (lpb/lsz)
 		 to copy string from. NOTE: client can pass cb=0 & lsz != lpNil and
 		 the grid will set gcls.cb=CchLsz(lsz)
 Return: 0L if successful, errVal otherwise (e.g. OOM)

G_CLEARDATA: Free up grid-owned data and invalidate its window
 Criteria: fGaHoldData
 wParam: one of		fGClearAll	- clear all cells
 					fGClearScr	- clear all scrollable cells, not headers
 lParam: unused
 Return: 

G_GETFOCUS: Returns the current cell position (one with the focus)
 Criteria: 
 wParam: if fLboxFocus, then give back real value if in the middle of setting
		 the listbox focus. NOTE: The listbox code is the ONLY code that should
		 use this. It's a mega-hack. See FSetLboxFocus as to why you'd use it.
 lParam: LPGCID containing icolCur, irowCur
 Return: 

G_SETFOCUS: Sets the current cell position (possibly giving it the input focus)
 Criteria: !fGcaDisabled
 wParam: if wGcSetCur, only changes currency; if wGcSetCaret also sets caret;
		 if wGcSetFocus also tries to move input focus if grid already has it
		 (use to wGcForceFocus to ensure the grid gets the input focus),
		 if fGcForceCur is set, force current cell to change even if
		 		commit fails. 
 lParam: LPGCID specifying icol & irow
 Return: 0 if successful

G_GETCOLWIDTH: Get the specified column's width
 Criteria: 
 wParam: != 0 to get pixels, 0 for twips
 lParam: icol 
 Return: MakeLong(column width, 0)

G_GETROWHEIGHT: Get the specified row's height
 Criteria: 
 wParam: != 0 to get pixels, 0 for twips
 lParam: irow
 Return: MakeLong(row height, 0)

G_SETCOLWIDTH: Set the specified column's width
 Criteria: fGaDiffWidths (unless icol == icolNil)
 wParam: column width (in twips), -1 for best fit
 lParam: icol (if == icolNil, changes width of all columns)
 Return: 0 if resized, > 0 if no change, < 0 on error

G_SETROWHEIGHT: Set the specified row's height
 Criteria: fGaDiffHeight unless irow == irowNil
 wParam: row height (in twips)
 lParam: irow (if == irowNil, changes height of all rows)
 Return: 0 if resized, > 0 if no change, < 0 on error

G_SHOWCOLS: Show or hide the specified columns; showing a hidden column will
	set it to the default width.
 Criteria: fGaDiffWidths (unless icol == icolNil)
 wParam: TRUE to show, FALSE to hide
 lParam: LPGSIZ containing icolSrc, ccol defining column range to Show/Hide
 Return: 0 if resized, > 0 if no change, < 0 on error

G_SHOWROWS: Show or hide the specified rows; showing a hidden row will
	set it to the default height.
 Criteria: fGaDiffHeights (unless irow == irowNil)
 wParam: TRUE to show, FALSE to hide
 lParam: LPGSIZ containing irowSrc, crow defining row range to Show/Hide
 Return: 0 if resized, > 0 if no change, < 0 on error

G_FREEZECOLS: Either freeze the specified columns (first moving them to the
	left of all unfrozen cols) or unfreeze all frozen columns
 Criteria: 
 wParam: TRUE to freeze, FALSE to unfreeze
 lParam: LPGMID containing icolSrc, ccol defining column range to freeze if
 	wParam TRUE; unused if wParam FALSE
 Return: 0 if successful, != 0 otherwise

G_FREEZEROWS: Either freeze the specified rows (first moving them above all
	unfrozen rows) or unfreeze all frozen rows
 Criteria: 
 wParam: TRUE to freeze, FALSE to unfreeze
 lParam: LPGMID containing irowSrc, crow defining row range to freeze if
 	wParam TRUE (if NULL, use selected/current rows); unused if wParam FALSE
 Return: 0 if successful, != 0 otherwise

G_GETCELLATTRS: Get the current cell/col/row attributes
 Criteria: 
 wParam: 
 lParam: LPGCAD pointing to icol & irow identifying cell, GCA to hold current
 		 attribute values; if icol == icolNil get row defaults,
 		 if irow == irowNil get col defaults,
 		 if both == Nil get grid defaults, if neither get cell attributes
 Return: 0L always (can't fail)

G_SETCELLATTRS: Set the default column, default row or cell attributes
 Criteria:
 wParam: wNoInval (0) for no invalidation; otherwise may combine:
 	fInvalCellBits to simply repaint,
 	fInvalData to cause control to refill with current data,
 	fInvalAttrs to cause visible controls to reset with current attributes.
 lParam: LPGCAD pointing to icol & irow identifying cell, GCA holding new
 		 attribute values; if icol == icolNil set row defaults,
 		 if irow == irowNil set col defaults,
 		 if both == Nil set grid defaults, if neither set cell attributes

 		 if lpgcad->grfSet != fSetAll then only the attributes described by
 		 this bitmask (see values specified below) will be changed; this
 		 obviates the need for most G_GETCELLATTRS calls
 Return: 0L if successful, errVal otherwise (e.g. OOM)
 	NOTE: if an error value is returned it's the client's responsibility
 		to free any PPGCA previously allocated via a G_CREATEKT call

G_CREATEKT: Allocate and return a PPGCA containing a default KT of the
	specified type. Client should then set all specific KT info (via
	LpktOfPgktd macro) before calling G_SETCELLATTRS.
 Criteria: 
 wParam: hkcl (type of kt) to create
 lParam: LPGKTD returning the allocated PPGCA and SEG it was allocated out of.
 Return: 0 if succesful, != 0 otherwise
	NOTE: if there are errors between this call's successful return and
		a successful return of G_SETCELLATTRS the client must clean up
		via FreePpgcaInSeg (gktd.ppgca, gktd.seg)
	NOTE: The returned PPGCA is owned by the grid, and the grid may
		deallocate it anytime after it has been used in a G_SETCELLATRS call.
		Specifically, G_SETCELLATTRS with fSetPpgca deallocates the
		PPGCA that was previously set.  Therefore, the client should not
		expect to be able to hold on to the PPGCA that this call provides.

G_ACTNOTIFY: Notify the grid of its "active" state (corresponds to Windows
	activation).  Note that this does not itself transfer focus.  When
	client wants grid (or a control within it) to get focus, then client
	should SetKeyFocus to grid.
 Criteria: 
 wParam: != 0 on activation, == 0 on deactivation
 lParam: 
 Return: 

G_INVALCELL: Invalidate the specified cell, row or column
 Criteria: 
 wParam: may combine:
 	fInvalCellBits to simply repaint cell data,
 	fInvalAllBits to erase and repaint,
 	fInvalData to cause current control to refill with current data,
	fInvalDataClean to cause current control to refill with current data only
		if not dirty,
 	fInvalAttrs to cause current control to reset with current attributes.
	fInvalSource to cause current control's data source to get reinitialized
	fInvalSourceCols to cause specified columns' data sources to get reinitialized
	fInvalSourceRows to cause specified rows' data sources to get reinitialized
	
 lParam: LPGCID pointing to icol & irow identifying cell/row/col to invalidate
 		 if icol == icolNil invalidate irow'th row,
 		 if irow == irowNil invalidate icol'th col,
 		 if both == Nil invalidate entire grid
 Return: 

G_UPDATE: Call this to initialize scrollbars after all G_SETDATA calls
 Criteria: fGaHoldData
 wParam: if wLimitRows, set the number of visible rows (irowRegMac) equal to
 		 the number that currently have data (irowDataMac); if wExactRows,
 		 set the number of rows to lParam; if wUnlimitRows, reset the
 		 number of rows to crowsGridDft; if wDynamicRows, reset grid to ask
 		 client for rows as necessary (as for fGaDynamicRows); if wUpdScroll,
 		 simply update the scrollbar.
 lParam: the number of visible rows if wParam == wExactRows
 Return: 

G_INSERTCOLS: Insert grid columns
 Criteria: 
 wParam: UINT # of cols to insert
 lParam: ICOL to insert in front of (if icolNil, uses icolCur)
 Return: 0L if successful, > 0 if canceled, < 0 on error (e.g. OOM)

G_INSERTROWS: Insert grid rows
 Criteria: 
 wParam: UINT # of rows to insert
 lParam: IROW to insert in front of (if irowNil, uses irowCur)
 Return: 0L if successful, > 0 if canceled, < 0 on error (e.g. OOM)

G_DELETECOLS: Delete grid columns
 Criteria: 
 wParam: UINT # of cols to delete
 lParam: ICOL to start deleting at (if icolNil, uses icolCur & dels selection)
 Return: 0L if successful, > 0 if canceled, < 0 on error (e.g. OOM)

G_DELETEROWS: Delete grid rows
 Criteria: 
 wParam: UINT # of rows to delete
 lParam: IROW to start deleting at (if irowNil, uses irowCur & dels selection)
 Return: 0L if successful, > 0 if canceled, < 0 on error (e.g. OOM)

G_MOVECOLS: Move grid columns
 Criteria: 
 wParam: 
 lParam: LPGMID containing icolSrc, ccol & icolDst to move between
 Return: 0L if successful, > 0 if canceled, < 0 on error (e.g. OOM)

G_MOVEROWS: Move grid rows
 Criteria: 
 wParam: 
 lParam: LPGMID containing irowSrc, crow & irowDst to move between
 Return: 0L if successful, > 0 if canceled, < 0 on error (e.g. OOM)

G_GETDATAMAC: Return the number of columns and rows containing data.
 Criteria: 
 wParam: 
 lParam: LPGCID holding icolDataMac, irowDataMac
 Return: 

G_COMMITCELL: Commit the current cell if dirty
	Note: cell will refill with (potentially reformatted) data after commit
 Criteria: 
 wParam: if fGcForceCur, reset dirty flag even if commit fails.
			Note: will still report failure if this flag is set.
		 if fGcForceCommit, will mark cell dirty before trying commit.
 lParam: 
 Return: 0 if successful, != 0 otherwise

G_GRIDLINES: Set grid line options and redraw
 Criteria: 
 wParam: Nothing
 lParam: LPGLNS of options to set
 Return: 

G_DFTROWHEIGHT: get the default row height based on the font of the specified 
				cell/row/column/grid
 Criteria: 
 wParam: != 0 to get pixels, 0 for twips
 lParam: LPGCID pointing to icol & irow identifying cell;
 		 if icol == icolNil get row defaults,
 		 if irow == irowNil get col defaults,
 		 if both == Nil get grid defaults, if neither get cell attributes
 Return: row height (returns grid default if OOM occurs)

G_GETSEL: get the current grid selection (see bSel* #define's above)
 Criteria: 
 wParam: 0 to get first component of selection, 1 to get second (if selection
 	was discontiguous), 2 to get third ...
 lParam: LPRCGSEL returning the desired selection info; if there's no selection
 	 lpgsel->grfSelTyp will be bSelNil and lpgsel->rcg.topleft will hold
 	 (icolCur,irowCur)
 Return: number of components in selection: 0 if Nil, 1 if simple, > 1 if
 	discontiguous ...

G_GETORG: Get the current scroll origin in the grid (i.e. the cell at the
	top left scrollable position)
 Criteria: 
 wParam: 
 lParam: LPGCID returning the icol, irow of the cell at the origin
 Return: 

G_SETORG: Scroll the grid such that the specified cell is in the top left
	scrollable position
 Criteria: 
 wParam: TRUE to invalidate rather than force synchronous update
 lParam: LPGCID specifying the icol, irow to scroll to
 Return: 

G_SETUNDO: Set an explicit rollback point for Undo; Row/Column Delete/Cut/
	Paste/Insert/Move are the implicitly supported Undoable operations
 Criteria: 
 wParam: one of
 	wUndoNormal - save current grid state until next implicit or
 		explicit (G_SETUNDO) Undoable operation (UNUSED?)
	wUndoCapture - save current grid state until next wUndoCapture/Release
		(i.e. don't allow implicit rollback points to override)
	wUndoRelease - release Undo to allow rollback of next implicitly Undoable
		operation that follows
 lParam:
 Return: 0L if successful, != 0L otherwise

G_SETSEL: set the grid selection (see bSel* #define's above)
 Criteria: 
 wParam: 0 to set selection to *lpgsel, 1 to add *lpgsel as a discontiguous
 	component to the current selection
 lParam: LPRCGSEL setting the desired selection info; if there's no selection
 	 lpgsel->grfSelTyp should be bSelNil 
 Return: 0L if successful, > 0 if cancelled, < 0 on error

G_CALLFUNC: Call a specified function with a KPB based on the current cell
 Criteria: 
 wParam: 0 to use current cell, != 0 to use that specified in lParam->icol/irow
 lParam: LPGFUNC specifying the function to call and the arguments to pass it
 Return: the UINT returned from the function (cast to a DWORD)

G_GCIDOFPT: Return the cell containing the specified client coordinate
 Criteria: 
 wParam: 
 lParam: LPGPOS specifying the point (in client coordinates of the grid);
 	the grid coordinates will be returned in the GCID.
 Return: 0 if point is outside the grid (i.e. icol == icolNil || irow == irowNil)
 	!= 0 if within the grid. (CONSIDER: could return hit-code if needed)

G_GETCELRANGE: Return an rcg of the currently visible cells
 wParam:
 lParam: LPRCG used to hold return value
 Return:

G_DESTROY: Initiate the termination of the grid control
 wParam:
 lParam:
 Return:

G_BEGINEDIT: Tell grid to consider current cell to be dirty
 wParam:
 lParam:
 Return: 0L if successful, < 0 otherwise

G_ENDEDIT: Tell grid to consider current cell to be clean
 wParam:
 lParam:
 Return:

G_SELTEXT: Get or set selection range for current cell
 Criteria: Must be text-oriented control -- not a checkbox...
 wParam: 0 if getting, 1 if setting
 lParam: MAKELONG(ichSelMic, ichSelMac) if setting
 Return: MAKELONG(ichSelMic, ichSelMac) if getting. If errors on getting, it
	will return MAKELONG(-1, -1)

G_MAKEVIS: Make a given row/column visible
 wParam: grfVis (see #define's below)
 lParam: LPGCID to cell
 Return: 0L if successful, < 0 otherwise
 
G_SETCOLORS: 
 wParam: not used
 lParam: LPGV defining new grid brushes
 Return: Always returns 0L (successful). Cant fail.
 
G_GETDISPTEXT: Get any part of currently displayed text from one field
 wParam: 
 lParam: LPGCLS pointing to icol, irow, buffer length, buffer to
		 copy string into
 Return: 0L if successful, != 0 othrerwise

G_SETDISPTEXT: Set the given text into the current cell's control
 wParam: 
 lParam: CHAR FAR * lszText. (must be null terminated)
 Return: 0L if successful, != 0 othrerwise

G_FDIRTY: Find out whether the grid is currently dirty or not.
 wParam: not used
 lParam: not used
 Return: 0 if not dirty, != 0 if dirty.

G_PAINTCELLS: Paints a square of cells.
 wParam: not used
 lParam: LPRCG of cells to paint.
 Return: Always returns 0L (successful). Can't fail.

G_ICOLOFID: Returns a unique ID for a column (unique only in the life of the column)
 wParam: not used
 lParam: icol of column to get ID for
 Return: wId for column, 0 if no such column.

G_IDOFICOL: Returns the current icol of the unique ID
 wParam: not used
 lParam: ID to get icol for
 Return: icol for column, icolNil if no such column

G_GRIDTOCLIENT
 wParam: not used
 lParam: LPPOINT in client coordinates
 Return: Always returns 0L (successful). Point in lparam is now in client
		 coordinates

G_PTOFGCID: Return the client coordinates of the specified cell
 Criteria: 
 wParam: 
 lParam: LPGPOS specifying the cell, the grid coordinates will be returned
		 in the POINT.
 Return: 0 if point couldn't fit in the POINT (still will be very large), != 0
		 otherwise

G_SETLBOXSELTYPE: Changes the selection model of a listbox
 wParam: wSingleSel, wExtendedSel, or wMultipleSel
 lParam: not used
 returns: 0
 
G_TOGGLEROW: Turns selection for a specified row on/off
 wParam:
 lParam: irow to change
 returns: 0 if successful, non-zero if not

*/

#define	G_SETDATA		(WM_GRIDSTART + 50)
#define	G_CLEARDATA		(WM_GRIDSTART + 51)
#define	G_GETFOCUS		(WM_GRIDSTART + 52)
#define	G_SETFOCUS		(WM_GRIDSTART + 53)
#define	G_GETCOLWIDTH	(WM_GRIDSTART + 54)
#define	G_GETROWHEIGHT	(WM_GRIDSTART + 55)
#define	G_SETCOLWIDTH	(WM_GRIDSTART + 56)
#define	G_SETROWHEIGHT	(WM_GRIDSTART + 57)
#define	G_GETCELLATTRS	(WM_GRIDSTART + 58)
#define	G_SETCELLATTRS	(WM_GRIDSTART + 59)
#define	G_SHOWCOLS		(WM_GRIDSTART + 60)
#define	G_SHOWROWS		(WM_GRIDSTART + 61)
#define	G_ACTNOTIFY		(WM_GRIDSTART + 62)
#define	G_GETDATA		(WM_GRIDSTART + 63)
#define G_GETORG		(WM_GRIDSTART + 64)
#define G_INVALCELL		(WM_GRIDSTART + 65)
#define G_GETDATAMAC	(WM_GRIDSTART + 66)
#define G_UPDATE		(WM_GRIDSTART + 67)
#define G_INSERTCOLS	(WM_GRIDSTART + 68)
#define G_INSERTROWS	(WM_GRIDSTART + 69)
#define G_DELETECOLS	(WM_GRIDSTART + 70)
#define G_DELETEROWS	(WM_GRIDSTART + 71)
#define G_MOVECOLS		(WM_GRIDSTART + 72)
#define G_MOVEROWS		(WM_GRIDSTART + 73)
#define G_COMMITCELL	(WM_GRIDSTART + 74)
#define G_GRIDLINES		(WM_GRIDSTART + 75)
#define G_DFTROWHEIGHT	(WM_GRIDSTART + 76)
#define G_GETSEL		(WM_GRIDSTART + 77)
#define G_CREATEKT		(WM_GRIDSTART + 78)
#define G_SETORG		(WM_GRIDSTART + 79)
#define G_SETUNDO		(WM_GRIDSTART + 80)
#define G_SETSEL		(WM_GRIDSTART + 81)
#define G_CALLFUNC		(WM_GRIDSTART + 82)
#define G_GCIDOFPT		(WM_GRIDSTART + 83)
#define G_FREEZECOLS	(WM_GRIDSTART + 84)
#define G_FREEZEROWS	(WM_GRIDSTART + 85)
#define G_GETCELRANGE	(WM_GRIDSTART + 86)
#define G_DESTROY		(WM_GRIDSTART + 87)
#define G_ENDEDIT		(WM_GRIDSTART + 88)
#define G_SELTEXT		(WM_GRIDSTART + 89)
#define G_GETDISPTEXT	(WM_GRIDSTART + 90)
#define G_MAKEVIS		(WM_GRIDSTART + 91)
#define G_SETCOLORS		(WM_GRIDSTART + 92)
#define G_SETDISPTEXT	(WM_GRIDSTART + 93)
#define G_BEGINEDIT		(WM_GRIDSTART + 94)
#define G_CROWSHIGH		(WM_GRIDSTART + 95)
#define G_GETCOLORS     (WM_GRIDSTART + 96)
#define G_FDIRTY		(WM_GRIDSTART + 97)
#define G_PAINTCELLS	(WM_GRIDSTART + 98)

/* WM_GRIDEND is WM_GRIDSTART + 99 */

/* We ran out of availiable values for grid messages, so we allocated another
block of 100 values for grid messages */
#define GCN_RECURSIVESAVE	(WM_GRIDXSTART + 0)
#define GCN_SELCHANGED		(WM_GRIDXSTART + 1)
#define GCN_SETFOCUS        (WM_GRIDXSTART + 2)
#define GCN_MOUSEEVENT      (WM_GRIDXSTART + 3)
#define GCN_CHANGE          (WM_GRIDXSTART + 4)
#define GCN_KEYEVENT      	(WM_GRIDXSTART + 5)
#define GCN_NOTINLISTEVENT	(WM_GRIDXSTART + 6)

#define G_ICOLOFID			(WM_GRIDXSTART + 10)
#define G_IDOFICOL			(WM_GRIDXSTART + 11)
#define G_GRIDTOCLIENT		(WM_GRIDXSTART + 12)
#define G_PTOFGCID			(WM_GRIDXSTART + 13)

#define GCL_GETCOLTYP		(WM_GRIDXSTART + 20)
#define GCL_GETHBASMOD		(WM_GRIDXSTART + 21)
#define GCL_GETPUNK			(WM_GRIDXSTART + 22)

#define GCL_GRIDCURSOR		(WM_GRIDXSTART + 23)

#define G_SETLBOXSELTYPE	(WM_GRIDXSTART + 24)
#define GCN_BEGINSELDRAG	(WM_GRIDXSTART + 25)	/* drag-drop interior sel */
#define GCL_LOCKBYTES		(WM_GRIDXSTART + 26)

#define G_TOGGLEROW			(WM_GRIDXSTART + 27)
#define GCL_AUTOCORRECT		(WM_GRIDXSTART + 28)
#define GCN_THUMBTRACK		(WM_GRIDXSTART + 29)
#define G_SETCELLHIGHLIGHT	(WM_GRIDXSTART + 30)
#define G_NOCELLHIGHLIGHT (WM_GRIDXSTART + 31)

/* WM_GRIDXEND is WM_GRIDXSTART + 49 */

/* generic return value for cancelled actions: */
#define	errClientAbort	(1)
#define	errSuccess		(0)
#define errFailure		(-1)

/* Flags for G_CLEARDATA: */
#define	fGClearAll		0x0000	/* default: clear all cell data */
#define	fGClearScr		0x0001	/* only clear scrollable cells' data */
// CONSIDER #define	fGClearCols		0x0002	/* only clear cells in given column range */
// CONSIDER #define	fGClearRows		0x0004	/* only clear cells in given row range */

/* this bit can be OR'ed along with any of those above: */
#define	fGClearAttrs	0x0008	/* also clear all row/col/cell attributes */


#define	fGcSetCur		0x0001	/* sets the current cell */
#define	fGcSetCaret		0x0002	/* gives specified cell the caret */
#define	fGcSetFocus		0x0004	/* gives specified cell the focus ONLY IF the
								   grid or one of its children already has it */
#define	fGcForceFocus	0x0008	/* gives specified cell the focus EVEN IF
								   neither the grid nor any of its children
								   already has it */
#define fGcForceCur 	0x0010	/* force current cell to change, even if commit
								   fails (suppresses error messages) */
#define fGcForceCommit	0x0020	/* force cell to commit even if not dirty */
#define	fGcDelayAct		0x0040	/* delay activation of some controls (e.g. textboxes) */
#define fGcShow			0x0080	/* scroll new current cell into view */
#define fGcByClient		0x0100	/* focus change was by client, not user */
#define	fGcResetCur		0x0200	/* SetCur(FALSE) then SetCur(TRUE) even if
								   currency isn't changing */
#define fGcImmediate	0x0400	/* Set focus immediately */

/* Values for G_SETFOCUS wParam: */

/* to set the current cell: */
#define	wGcSetCur	(fGcSetCur)

/* to set the current cell & give it the caret: */
#define	wGcSetCaret	(fGcSetCur|fGcSetCaret)

/* to set the current cell, scroll it into view, give it the caret & the
   Windows focus (only if the grid or another control already has it): */
#define	wGcSetFocus	(fGcSetCur|fGcSetCaret|fGcSetFocus|fGcShow)

/* to set the current cell, scroll it into view, give it the caret & the
   Windows focus (even if neither the grid nor another control already has it): */
#define	wGcForceFocus	(fGcSetCur|fGcSetCaret|fGcForceFocus|fGcShow)

/* Values for G_UPDATE wParam: */
#define	wUpdScroll		0	/* update both scrollbars based on current data */
#define	wLimitRows		1	/* sets # of rows to those having data */
#define	wUnlimitRows	2	/* resets # of rows to crowGridDft */
#define	wExactRows		3	/* sets # of rows to lParam */
#define	wDynamicRows	4	/* prompts grid to start asking client for # of rows */
#define	wDataRows		5	/* sets # of rows having data (irowDataMac) */
#define	wUpdHScroll		6	/* update horizontal scrollbar based on current data */
#define	wUpdVScroll		7	/* update vertical scrollbar based on current data */
#define wResetDynamic	8	/* for grid w. dynamic rows, reset irowDataMac to irowRegMin */
#define wExactRowsInval	9	/* wExactRows, but don't update immediately */

/* Values for G_GETFOCUS wParam: */
#define fLboxFocus		1	/* see description for G_GETFOCUS */

/* Values for G_INVALCELL wParam: */
#define	fInvalAllBits		0x01	/* invalidates all cells, grid lines and margins */
#define	fInvalCellBits		0x02	/* invalidates all cells */
#define	fInvalData			0x04	/* causes cell data to be refilled (only makes
									   sense where !fGcaHoldData) */
#define	fInvalAttrs			0x08	/* causes cell controls to request new display
									   attributes from the grid */
#define fInvalSource		0x10	/* re-initialize sources for combos */
#define fInvalDataIfClean	0x20	/* cause cell data to be refilled if cell not dirty */
#define fInvalSourceCols	0x40	/* cause specified grid cols to re-init sources */
#define fInvalSourceRows	0x80	/* cause specified grid rows to re-init sources */
#define fInvalDataIfNotCheckDirty 0x100 /* invalidates all cells, except for the case of
											a cell which wants to be treated as dirty,
											but hasn't done a successful FBeginEditKpb.
											Hack to avoid losing work in OLE servers */

#define	wNoInval	0
#define	wInvalAllBits	(fInvalAllBits)
#define	wInvalData		(fInvalData | fInvalCellBits)	
#define	wInvalAttrs		(fInvalAttrs | fInvalCellBits)

/* Values for G_SETDATA */
#define wSetDataNormal		0		/* Standard SETDATA behavior */
#define wSetDataUpdScroll	1		/* Update scrollbars */
#define wSetDataTransient	2		/* Data is "transient". Dont update DataMac */
#define wRecursiveSetData	4		/* Client calling G_SETDATA within
									   GCL_SETDATA notification */

/* Values for G_GETDATA */
#define wGetDataNormal		0		/* Standard GETDATA behavior */
#define wGetDataNoCommit	1		/* Do not commit even if the cell is dirty */

/* Values for G_SETCELLATTRS */
#define	fSetFlags		0x01		/* will |= lpgcad->gca.grfGca */
#define	fUnsetFlags		0x02		/* will &= ~(lpgcad->grfGcaUnset) */
#define	fExactFlags		0x200		/* will = lpgcad->grfGcaUnset */
#define fSetAln			0x04		/* set aln without changing rest of TXTINFO */
#define	fSetTxtinfo		0x08		// CONSIDER: (0x08 | fSetAln)
#define	fSetBrshinfo	0x10
#define fSetBdrinfo		0x20
#define	fSetAttrs		0x40		/* updates everything but control type */
#define	fSetPpgca		0x100		/* will set control type using specified
									   PPGCA */
#define	fSetBkBrsh		0x400		/* grid window's background brush */

/* Values for G_SETUNDO wParam: */
#define	wUndoNormal		0
#define	wUndoCapture	1
#define	wUndoRelease	2

/* Values for G_SETLBOXSELTYPE wParam */
#define	wSingleSel		0
#define	wMultipleSel	1
#define	wExtendedSel	2


/* Values for G_RECORDUNDO wParam */
#define ungrOpEditCell	0
#define ungrOpRowInsert	1
#define ungrOpRowDelete	2
#define ungrOpRowMove	3
#define ungrOpCut		4
#define ungrOpResize	5

/* Values for G_MAKEVIS wParam: */
#define fVisBotRight	0x0001		/* ensure that the cell's bottom right
									   corner is made visible if possible */
#define fVisMakeOrg		0x0002		/* if a scroll is needed, scroll the
									   specified cell to the origin */
#define fVisInval		0x0004		/* avoid a synchronous paint - simply
									   invalidate if scroll is needed */

/* Standard width for a grid's row selector button column */
#define dxtSelectColumn	284

typedef struct _grpi BPP(segPrint)	PPGRPI;

/* Public prototypes: */

PUBLIC VOID InitGrids(VOID);
PUBLIC VOID CleanupGrids(VOID);
PUBLIC DLLIMPORT HWND HwndGridCreate (LPGRID lpgrid, LPGV lpgv);
PUBLIC DLLIMPORT VOID GridInitialize(VOID);
PUBLIC DLLIMPORT VOID GridPaint(HWND hwnd, HDC hdc, RECT rcUpdate, UINT grfDg);
PRIVATE VOID PrtGridJ(HWND hwndGrid,struct _eop BPP(segPrint) ppeop,struct _cip BPP(segPrint) ppcip,UINT	pfmsg);
PRIVATE PPGRPI PpgrpiInit(HWND hwndGrid,INT dxtPage,INT dytPage,
						  BOOL fPrintSelection,BOOL fCheckSizes);
PRIVATE BOOL FLastPrintPpgrpi(PPGRPI ppgrpi,HDC hdc,BOOL fDrawing,BOOL fDataOnly,
							  UINT wPageDesired, LONG ldyAbove,INT xOrgOffset,
							  INT yOrgOffset, struct _mip *pmip, RECT rcClip, INT *pdyt, INT *pdytBorder);
PRIVATE VOID TermPpgrpi(PPGRPI ppgrpi);
PRIVATE HWND HwndOfPpgrpi(PPGRPI ppgrpi);
PRIVATE VOID ResetPpgrpi(PPGRPI ppgrpi);
PRIVATE BOOL FPrintSelection(HWND hwndGrid);
PRIVATE VOID FreePpgrpiNil(PPGRPI ppgrpi);
PRIVATE PPGRPI PpgrpiDupJ(PPGRPI ppgrpi);
#ifdef __cplusplus
}
#endif

#endif _GRID_H

#endif	// !defined(GRID_H_INCLUDED_)
