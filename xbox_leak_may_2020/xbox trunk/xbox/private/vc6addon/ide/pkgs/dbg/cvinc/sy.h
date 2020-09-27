/**** SY.H - System Support Generic using osdebug                       ****
 *                                                                         *
 *                                                                         *
 *  Copyright <C> 1990, Microsoft Corp                                     *
 *                                                                         *
 *  Created: October 15, 1990 by David W. Gray                             *
 *                                                                         *
 *  Purpose:                                                               *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

typedef HMEM HPRC;
typedef HMEM HTHD;

extern HMEM hprcCurr;
extern HMEM hthdCurr;
extern HMEM hthdLast;
extern HMEM hthdPrompt;

typedef enum {
    stpNotStarted,
    stpRunning,
    stpDead,
    stpStopped,
    stpGone
} STP; // Process State

typedef enum {
    expStopped,		// debuggee is stopped
    expGo,			// we just did a go
    expStep,		// we just did a single-step
    expRange,		// we just did a range-step
    expAfterRet		// we just did a return-step
} EXP; // Process execution mode

typedef enum {
    tdfUnknown,
    tdfFrozen,
    tdfThawed,
    tdfDead,
} TDF; // Thread state

typedef enum {
	stoNone				= 0x00,
	stoOneThread		= 0x01,	// Execute a single thread
	stoInitialBP		= 0x02,	// Stop on initial breakpoint
	stoQueryStep		= 0x04,	// Query before stepping into
	stoPassEx			= 0x08,	// Pass exception on to debuggee
	stoTraceFunction	= 0x10,	// Stepping into specific function
} STO; // STep Options

typedef enum {
	edsNone,
	edsLaunching,		// we are currently launching the debuggee
	edsAsyncStop,		// we are currently attempting to async stop debuggee
	edsKilling,			// we are currently attempting to kill the debuggee
} DEBUGGEE_STATE_EX;

typedef enum {
	krNone			= 0x00,
	krQuiet			= 0x01,	// this is a quiet kill (no message box)
	krRestart		= 0x02,	// this is a kill for restart
	krFastRestart	= 0x04	// this is a kill for fast restart (note: the krRestart
							// flag is ignored in this case)
} KILL_REASON;


enum STEP_MODE
{
	STEP_MODE_FORCE_ASM = 0,	// force asm stepping
	STEP_MODE_FORCE_SOURCE,		// force source stepping
	STEP_MODE_FROM_WINDOW		// determine the stepping mode from the type
};								// of the top level window.  if the type is
								// source, we are in source mode, if the type
								// is asm, we are in disassembly mode

	
typedef struct _THD {
    HTID htid;
    unsigned long tid;
    TDF tdf;    // thread "deadness"
} THD;  // Thread info structure

typedef THD *PTHD;
typedef THD FAR *LPTHD;

typedef struct _PRC {
    BOOL fLoaded;
    BOOL fSymLoaded;
    BOOL fAmbName;

	// Flags to keep track of asynchronous call to Debugger.Go
	//  from inside the BreakpointHit event handler
	BOOL				m_bInsideEvent;			// we are currently executing a bp hit event handler.
	BOOL				m_bGoDeferred;			// event handler did a Debugger.Go, which thus was deferred
	BOOL				m_bStopDeferred;		// Debugger.Stop
	BOOL				m_bEncGoPending;		// Break/Go for edit and Continue while debuggee is running
	
    HPID hpid;
    HPDS hpds;
    HTHD hthdCurr;
    unsigned long pid;
    char *szTitle;
    char *szName;
    char *szArgs;
    HPRC hprcParent;
    STP  stp;
	EXP  exp;			// what type of action are we executing right now?
	BOOL fBpInitialCxt;	// do we currently have a BP at initial cxt (WinMain)?
	WORD wEM;
    BOOL fStepOver;
    STO  sto;
    ADDR addrStart;
    ADDR addrEnd;
    HTID htidExecuting;
    BOOL fFuncExec;
	UINT funcExecTimerID; 
    BOOL fHitBP;
	DBC  dbcLastStop;	// the most recently received dbc which caused
						// execution to stop, e.g. dbcBpt, dbcException,
						// dbcProcTerm, and so on
    HLLI llthd;
    BOOL fReallyNew;
    BOOL fPcodePresent;
    BOOL fHasThreads;
    BOOL fHasMessages;

	BOOL fHaveInitialContext;	// Is there a cached get_initial_context?
	int  iReturnInitialContext;	//    If so, what was the return value?
	CXT  cxtInitial;			//    Also, if it succeeded, what's the context?
	BOOL fSqlInproc;			// SQL inproc debugging?

	

	BOOL IsFirstChance()		{ return this ? fExceptionHappening : FALSE; }
	VOID SetFirstChance(BOOL f)	{ fExceptionHappening = !!f; }

		// [v-natpm] Flag set when a 1st chance exception happens
		// (upon a dbcException notification).
		// When this flag is set :
		// - Step commands are disabled (step over, step into, step out)
		// - The Go command brings up a "pass exception to debuggee ?
		//   YES/NO/CANCEL" dialogbox. The response of the user sets (YES),
		//   or resets (NO) the fPassException field of the EXOP struct
		//   used by OSDGo().  The flag is reset by Go/Restart/Stop
		//   debugging



	DEBUGGEE_STATE_EX
	GetDebuggeeStateEx(
		)
	{
		return m_DebuggeeStateEx;
	}

	void
	SetDebuggeeStateEx(
		DEBUGGEE_STATE_EX	State
		)
	{
		m_DebuggeeStateEx = State;
	}
	
	//	why was this debuggee killed. NB: the debugger must be in the process
	//	of being killed
	
	ULONG
	GetKillReason(
		)
	{
		ASSERT (m_DebuggeeStateEx == edsKilling);
		return m_KillReason;
	}

	void
	SetKillReason(
		ULONG	KillReason
		)
	{
		if (m_KillReason & krFastRestart)
			KillReason |= krFastRestart;
			
		m_KillReason = KillReason;
	}

	// have we seen dbcEntryPoint yet or not?
	
	BOOL
	IsBeyondEntryPoint(
		)
	{
		return m_fBeyondEntryPoint;
	}

	void
	SetBeyondEntryPoint(
		BOOL	fBeyondEntryPoint		
		)
	{
		m_fBeyondEntryPoint = fBeyondEntryPoint;
	}

	// were the BPs for this debuggee all successfully bound?
	
	BOOL
	IsBPsBound(
		)
	{
		return m_fBPsBound;
	}

	void
	SetBPsBound(
		BOOL	BPsBound
		)
	{
		m_fBPsBound = BPsBound;
	}

	BOOL
	IsOmfLoaded(
		)
	{
		return m_OmfLoaded;
	}


	void
	SetOmfLoaded(
		BOOL	OmfLoaded
		)
	{
		m_OmfLoaded = OmfLoaded;
	}


	//
	// NOTE: this returns SRCSTEPPING or ASMSTEPPING -- i.e., whether we are
	// currently source stepping or asm stepping, *NOT* the m_StepMode
	// variable.  The m_StepMode variable is returned in the optional
	// parameter.

	ULONG
	GetStepMode(
		STEP_MODE*	StepMode = NULL
		);

	void
	SetStepMode(
		STEP_MODE	StepMode
		)
	{
		m_StepMode = StepMode;
	}

private:

	// access through {Is,Set}FirstChance()

	BOOL				fExceptionHappening;

	ULONG				m_KillReason;			// see KILL_REASON above
	BOOL				m_fBeyondEntryPoint;	// have we reached the entry point?
	BOOL				m_fBPsBound;			// were all BPs successfully bound
	DEBUGGEE_STATE_EX	m_DebuggeeStateEx;
	BOOL				m_OmfLoaded;			// whether the OMF is loaded or not
	STEP_MODE			m_StepMode;				// stepping mode

} PRC;  // PRoCess info structure

typedef PRC *PPRC;
typedef PRC FAR *LPPRC;

extern LPPRC lpprcCurr;

#define hpidCurr   HpidFromHprc ( hprcCurr )
#define htidCurr   HtidFromHthd ( hthdCurr )
#define htidLast   HtidFromHthd ( hthdCurr )
#define htidPrompt HtidFromHthd ( hthdCurr )

#define SethtidCurr(htid)   hthdCurr   = HthdFromHtid ( hprcCurr, htid )
#define SethtidLast(htid)   hthdLast   = HthdFromHtid ( hprcCurr, htid )
#define SethtidPrompt(htid) hthdPrompt = HthdFromHtid ( hprcCurr, htid )

#define SetHpidCurr(hpid)   hprcCurr   = HprcFromHpid ( hpid );

BOOL   PASCAL SYInitProcessList ( void );
BOOL   PASCAL SYLoadDebugInfo ( void );
BOOL   PASCAL SYCreateHprc ( char *, char *, HPRC, HPID );
char * PASCAL SYGetDebuggeeArgs ( char * );
BOOL   PASCAL SYDisplayTLError();

// Wrapper for OSDCreateHpid, this reports errors when appropriate.
XOSD   PASCAL SYUtilCreateHpid(LPFNSVC, HEM, HTL, LPHPID);

BOOL SYSingleStep ( BOOL, STO, HPID, HTID );
BOOL SYRangeStep ( BOOL, STO, ADDR, ADDR, HPID, HTID );
BOOL SYAfterReturn ( STO, HPID, HTID );
BOOL SYGo ( const EXOP*, HPID, HTID );
XOSD SYNativeOnly ( BOOL );
void SYFlipScreen ( HPID, HTID, BOOL, BOOL );
BOOL SYProgramHasRun ( void );
void SYSetContext ( HPRC, HTHD, BOOL );

HPRC HprcFromHpid ( HPID );
HLLI LlthdFromHprc ( HPRC );

unsigned long TidFromHtid ( HPRC, HTID );
unsigned long TidFromHthd ( HTHD );
HTID HtidFromTid ( HPRC, unsigned long );
HTID HtidFromHthd ( HTHD );
HTHD HthdFromHtid ( HPRC, HTID );
HTHD HthdFromTid ( HPRC, unsigned long );

unsigned long PidFromHpid ( HPID );
unsigned long PidFromHprc ( HPRC );
HPID HpidFromPid ( unsigned long );
HPID HpidFromHprc ( HPRC );
HPRC HprcFromHpid ( HPID );
HPRC HprcFromPid ( unsigned long );

HTHD GetNthThread ( WORD );

void			SYSetFrame ( FRAME * );
int				SYCmpAddr ( LPADDR, LPADDR );
void			SYSetAddr ( HPID, HTID, ADR, LPADDR );
void            SYUnassemble ( HPID, HTID, LPSDI );
void            SYFormatSdi ( LPSDI, char *, BOOL );
XOSD            SYAssemble ( LPADDR );
BOOL            SYGetPrevInst ( HPID, HTID, PADDR );
void			SYSetEmi ( HPID, HTID, PADDR );
XOSD			SYGetAddr ( HPID, HTID, ADR, LPADDR );
void			SYSaveModels ( char *, WORD );
void			SYUnloadDlls ( void );
void            SYUnFixupOmf ( void );

#define write_registers()
#define get_registers()

TDF PASCAL FAR ThreadStatus ( HMEM, char *, BOOL );

BOOL LOADDS PASCAL SYFixupAddr ( PADDR );
BOOL LOADDS PASCAL SYUnFixupAddr ( PADDR );
UINT LOADDS PASCAL SYFIsOverlayLoaded ( PADDR );

// Enum Process Identifier Type
typedef enum { epiDefault, epiTitle, epiName, epiPID } EPI;

extern EPI epiDef;

int GetProcId ( char FAR *, HPRC, EPI, int );
int ProcStatus ( HPRC, char *, int );
