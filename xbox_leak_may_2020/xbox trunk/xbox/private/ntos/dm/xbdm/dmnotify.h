/*
 *
 * dmnotify.h
 *
 * Random stuff for notifications
 *
 */

/* Debug exception handler parameter block */
typedef struct _EXCEPTION_TRAP {
    PKTRAP_FRAME TrapFrame;
    PKEXCEPTION_FRAME ExceptionFrame;
    PEXCEPTION_RECORD ExceptionRecord;
    PCONTEXT ContextRecord;
    BOOLEAN SecondChance;
} EXCEPTION_TRAP, *PEXCEPTION_TRAP;

/* Extended notification handler */
typedef struct _ENH {
	char szExtName[64];
	PDM_EXT_NOTIFY_FUNCTION pfnHandler;
} ENH;

#define MAX_ENH 16

/* Function call setup */
typedef struct _FCS {
	ULONG ReportedEsp;
	ULONG ParameterData[1022];
	ULONG SavedEip;
} FCS, *PFCS;

#define HF_BREAK 1
#define HF_DEFERRED 2
#define HF_CLEANUP 4

/* Deferred Notification */
typedef struct _DFN {
	struct _DFN *pdfnNext;
	/* For now, we just do debug prints */
	DMN_DEBUGSTR dmds;
} DFN, *PDFN;
