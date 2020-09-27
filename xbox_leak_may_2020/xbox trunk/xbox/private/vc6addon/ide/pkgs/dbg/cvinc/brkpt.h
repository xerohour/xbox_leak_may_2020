/*** brkpt.h - header file for breakpoint api
*
*   Copyright <C> 1989, Microsoft Corporation
*
*   Purpose:
*
*   Revision History:
*
*
*************************************************************************/
#ifndef _BRKPT_H	// WHOLE FILE {
#define _BRKPT_H

// Just to hide things
#define HBPI        HLLE
#define hbpiNull    hlleNull


#define chBeginCxtOp    _T('{')
#define chEndCxtOp      _T('}')
#define chQuote         _T('\"')


// Types of breakpoints.  These
// should be sorted by priority
// so when walking the entire
// bp list, the last entry is
// the one returned as the current
// status. (deadchild, abort, and
// hardcodedbp are not regular
// conditions, so they do not need
// to be in any specific order
typedef enum {
	BPDEADCHILD,
	BPABORT,
	BPHARDCODEDBP,
	BPCONTINUE,
	BPTMP,
	BPBREAKPT,
	BPTMPENC	// temp BP for edit & continue
} BPC;

typedef enum {
    BPUNKNOWN,
    BPSYMBOL,
    BPLINE,
    BPADDR
} EBPT;

#if defined (_X86_)
typedef BYTE BP_UNIT;
#define BRKPTOPCODE ((BYTE)0xCC)
#elif defined(_MIPS_)
typedef DWORD BP_UNIT;
#define BRKPTOPCODE 0x0016000D
BOOL MassageBPAddr(LPADDR, LPUOFFSET); // see brkpt0.c
#elif defined(_ALPHA_)
typedef DWORD BP_UNIT;
#define BRKPTOPCODE 0x80L
BOOL MassageBPAddr(LPADDR, LPUOFFSET); // see brkpt0.c
#else
#error "Need arch specific breakpoint"
#endif
#define DPLBITS ((unsigned int) 0x3)

#define SKIPBPUIFLAGS    3
#define SKIPOS2FLAGS     4

typedef union _bpf {
    unsigned        flags;
    struct {
        unsigned    fBpCode     : 1;    // Is there a Code bp
        unsigned    fBpData     : 1;    // Is there a watch/trace point
        unsigned    fActive     : 1;    // Breakpoint is active
        unsigned    fBPLoaded   : 1;    // is BRKPTOPCODE in the code
        unsigned    fVirtual    : 1;    // This is a virtual bp
        unsigned    fAmbig      : 1;    // This BP came from an ambiguous expression
        unsigned    fPass       : 1;    // if a passcount was specified
        unsigned    fUser       : 1;    // for use WITHIN a proc, up to you how to use
        unsigned    fDlgActive  : 1;    // Dialog hold are for orignal fActive state
        unsigned    fDlgMarkDel : 1;    // This breakpoint has been marked for deletion
        unsigned    fDlgMarkAdd : 1;    // This breakpoint has been marked for addition
        unsigned    BpType      : 3;    // The type of breakpoint
        unsigned    fMessage    : 1;
        unsigned    fNoStop     : 1;
        unsigned    fParamGiven : 1;
    } f;
} BPF;
typedef BPF *   PBPF;

typedef union _dpf {
    unsigned        flags;
    struct {
        unsigned    fEmulate    : 1;    // If emulation is required
        unsigned    fFuncBpSet  : 1;    // if a function breakpoint is set
        unsigned    fTpFuncHit  : 1;    // When we hit a breakpoint set by tracepoint
        unsigned    fFuncLoad   : 1;    // We must load the breakpoint value on function entry
        unsigned    fEvalExpr   : 1;    // Is the expr not an lvalue
        unsigned    fDataRange  : 1;    // if a datarange was specifed
        unsigned    fBpRel      : 1;    // if a code range was specified
        unsigned    fContext    : 1;    // if context checking is required
        unsigned    fReg        : 1;    // if it is in a register
        unsigned    fUser       : 1;    // for use WITHIN a proc, up to you how to use
        unsigned    fIsTrue     : 1;    // Have we already told the user
                                        //  that this wp has fired?
    } f;
} DPF;
typedef DPF FAR *   LPDPF;

/*
 * This defines a function callback mechanism which can be used to
 * generalize the breakpoint facility.
 * The messages and responses are defined earlier in this file.
 */
typedef struct _bcm {
    unsigned    wMsg;        // A message giving the reason for the call
    unsigned    wInstance;   // BPBrkExec call count
    char FAR *  lpch;        // A buffer for return info
    HBPI        hbpi;        // Points to the parent bpi
} BCM;  // Breakpoint Callback Method
typedef BCM FAR *   LPBCM;

typedef struct _dpi {
    union {
        struct {
            ADDR        DataAddr;     // Data address to watch
        } d;
        struct {
            ADDR        BlkAddr;      // the start address of the block
            UOFFSET     oEnd;         // The end offset of the function
            long        ldbBp;        // ofset from the bp
        } bp;
    } u;
    CXF                 cxf;          // the frame
    char *              pValue;       // pointer to the initial value
    short               iReg;         // if in register, the reg index
    ushort              cData;        // Number of data items to watch
    ushort              cbData;       // Number of bytes in data item
    HTM                 hTM;          // a TM handle for the data breakpoint
    LPBPS               lpbps;        // The OSD bps associated w/ this
} DPI;
typedef DPI *   PDPI;
typedef DPI FAR *   LPDPI;

typedef struct _bpi {
    BPF             bpf;        // flags
    DPF             dpf;        // the data bp flags
    int             fLang;      // the language type
    unsigned char   OpCode;     // The orignal opcode
    ADDR            CodeAddr;   // Code address of breakpoint
    SEGMENT         segFixed;   // Fixedup segment for MOVE BPFixSeg (unload)
    LPDPI           lpDpi;      // pointer to the DataBreakpoint
/*
 * This defines a function callback mechanism which can be used to
 * generalize the breakpoint facility.
 * The messages and responses are defined earlier in this file.
 */
    DWORD  dwMask;
    DWORD lMessage;
    ushort   cPass;      // Initial Pass count
    ushort   cPassCur;   // Current Pass count
    char FAR *lszCmd;    // Offset of the command to Execute
    HPRC     hprc;       // The process this bp is associated with
    HTHD     hthd;       // The tread to break at
                         //  NULL breaks on all threads
} BPI;
typedef BPI *   PBPI;
typedef BPI FAR *   LPBPI;

typedef enum ABPT {
    abptNotAmbig = 0,   // Not ambiguous
    abptRestoredAmbig,  // Used in restoring ambiguous BP's
    abptAmbigExpr,      // The expression evaluator tells us that it is ambig
    abptAmbigUser       // User gave a parameter list, it may have be ambiguous
} ABPT;

typedef struct _pbp {
    HTHD        hthd;
    ushort      BPType;
    PCXF        pCXF;
    ushort      BPSegType;
    char FAR *  lszCmd;
    ABPT        abpt;
    union {
        ADDR    Addr;
        struct {
            unsigned int    cBPMax;
            TML             tml;
        };
    };
    LPDPI       lpDpi;
    DPF         DPF;
    ushort      cPass;
    char FAR *  lszOptCmd;
    int         iErr;
    BOOL        fNoStop;
    BOOL        fMessage;
    DWORD       lMessage;
    DWORD       dwMask;
    BOOL        fInsert;
} PBP;
typedef PBP FAR *LPPBP;

typedef struct _sfi {
    int       fLang;
    ushort    fEnable;
    HTHD      hthd;
} SFI;
typedef SFI *   PSFI;

// user global variable  YUK! this should not be visable to the world!
// extern PBPI pbpiFirst;
extern int   G_BPfEmulate;

/***
 *
 *    Breakpoint Handler API
 *
 */
extern int  PASCAL      BPUIAdd( char *, HMEM);
extern int  PASCAL      BPUIEnable(char *);
extern int  PASCAL      BPUIDisable(char *);
extern int  PASCAL      BPUIList(char *);
extern int  PASCAL      BPUIClear(char *);
extern char FAR * PASCAL    BPUIFormat(HBPI, ushort, ushort, char FAR *, ushort);

extern void PASCAL     BPFreeType( ushort );
extern EESTATUS PASCAL BPParse( LPPBP );
extern HBPI PASCAL     BPCommitBP ( LPPBP );
extern void PASCAL     BPFixSeg( ushort );
extern HBPI PASCAL     BPIsAddr( PADDR, UOFFSET, HTHD, int, BOOL fCheckDisabled = FALSE, UINT *pWidgetFlags = 0, BPC bpcType = BPBREAKPT );
extern void PASCAL     BPDelete( HBPI );
extern int  PASCAL     BPGetNbr(HBPI);
extern int  PASCAL     BPCheckBpt(PADDR, LPMSGI, HTID);
extern void PASCAL     BPResetAllPassCnt( void );
extern int  PASCAL     BPBrkExec(PADDR, HTID, BPI **ppBPI = NULL);
extern void PASCAL     BPListDlg( void );
extern void PASCAL     BPModifyDlg( void );
extern void PASCAL     BPUISetOp( char * );
extern HBPI PASCAL     BPFirstBPI( void );
extern EESTATUS PASCAL BPParseAddress(LPPBP, BOOL);
extern EESTATUS PASCAL BPADDRFromTM (PHTM, unsigned short FAR *, PADDR);
extern LSZ  PASCAL     BPShortenSzCxt ( LSZ, LSZ );
extern VOID PASCAL	   BPInstallVirtual(HPID, HEXE, BOOL);
extern XOSD PASCAL     SetWatch ( LPBPI, BOOL );

extern HLLI    llbpi;

#endif	// _BRKPT_H }

