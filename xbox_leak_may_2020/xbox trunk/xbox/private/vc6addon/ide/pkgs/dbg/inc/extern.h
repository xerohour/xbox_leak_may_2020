#ifndef EXTERN_H
#define EXTERN_H

extern char ExecutablePath[_MAX_PATH];

// temp variables
extern char szPath[_MAX_PATH];
extern char szDrive[_MAX_DRIVE];
extern char szDir[_MAX_DIR];
extern char szFName[_MAX_FNAME];
extern char szExt[_MAX_EXT];

// Currently highlighted trace line (F8/F10)
extern TRACEINFO TraceInfo;

//extern BOOL fIsLaunchingDebuggee;

//Handle to instance data
extern HINSTANCE	hInst;

//Temp variables for various modules
extern char szTmp[TMP_STRING_SIZE];

// from dbg.h - will be pulled in later
extern char is_assign;

//Autotest mode for test suites
extern BOOL AutoTest;

// Whether or not to ask the user when the EXE we're about to debug has no symbols.
extern BOOL g_fPromptNoSymbolInfo;

//Empty string
extern char const szNull[1];

extern UINT FAR CDECL LOADDS CVsprintf(LSZ, LSZ, ... );
extern int PASCAL BPResolveDlg( PHTM , int, BOOL );
extern EESTATUS PASCAL CVParseBind ( PHTM phtm, PCXT pcxt, int iRadix,
        char fCase, char fForce, char FAR * sz);
extern EESTATUS PASCAL CVParseBindCxtl ( PHTM phtm, PCXT pcxt, int iRadix,
        char fCase, char fForce, PHCXTL phcl, char FAR * sz);

extern void FAR * PASCAL LOADDS LDSFmalloc(size_t);
extern void PASCAL LOADDS LDSFfree(void FAR *);
extern void FAR * PASCAL LOADDS LDSFrealloc(void FAR *, size_t);

extern BOOL     fIgnoreAmbigBPs;
extern int      fBlocking;
extern BOOL     fMultiDebug;
extern char     is_assign;

//typedef unsigned long HPID;
//typedef unsigned long HTID;

extern	HPID	Hpid;
extern	HTID	Htid;


// Memory management functions
extern void FAR * PASCAL LOADDS LDScvw3_fmalloc(size_t) ;
extern void FAR * PASCAL LOADDS LDScvw3_frealloc(void FAR *,size_t) ;
extern void FAR * PASCAL LOADDS LDScvw3_ffree(void FAR *) ;


extern unsigned short LOADDS PASCAL LDSbUnlockMb(HMEM) ;
extern HLLE LOADDS PASCAL LDSLLHlleGetLast(HLLI) ;
extern void LOADDS PASCAL LDSLLHlleAddToHeadOfLI(HLLI,HLLE) ;
extern BOOL LOADDS PASCAL LDSLLFRemoveHlleFromLl(HLLI,HLLE) ;
extern void LOADDS PASCAL LDShfree(void _HUGE_ *) ;
extern UINT LOADDS PASCAL SYReadFar(UINT,LPB,UINT) ;
extern LONG LOADDS PASCAL SYSeek(UINT,LONG,UINT) ;

#if defined(OSDEBUG4)
extern UINT LOADDS PASCAL SYProcessor(DWORD) ;
#else
extern UINT LOADDS PASCAL SYProcessor(VOID) ;
#endif

extern VOID LOADDS PASCAL SYClose(UINT hfile) ;
extern UINT LOADDS PASCAL SYOpen(LSZ) ;
extern LONG LOADDS PASCAL SYTell(UINT hfile);
#if defined(OSDEBUG4)
extern UINT PASCAL SYFindExeFile(LSZ, LSZ, UINT, LPVOID, PFNVALIDATEEXE);
extern VOID PASCAL DLoadedSymbols(SHE, HPID, LSZ);
extern BOOL PASCAL SYGetDefaultShe(LSZ, SHE *);
#endif

extern BOOL FAR PASCAL SYFindDebugInfoFile (PSEARCHDEBUGINFO);

// Path string manipulations
extern void FAR LOADDS PASCAL CVSplitPath(LSZ,LSZ,LSZ,LSZ,LSZ) ;
extern void FAR LOADDS PASCAL CVSearchEnv(LSZ,LSZ,LSZ) ;
extern LSZ  FAR LOADDS PASCAL CVFullPath(LSZ,LSZ,UINT) ;
extern void FAR LOADDS PASCAL CVMakePath(LSZ,LSZ,LSZ,LSZ,LSZ) ;
extern UINT FAR LOADDS PASCAL CVStat(LSZ,LPCH) ;
extern UINT FAR LOADDS CDECL  CVsprintf(LSZ,LSZ,...) ;
extern void _HUGE_ * LOADDS PASCAL LDShalloc(long,size_t) ;

extern void FAR * PASCAL LOADDS LDSFmalloc(size_t);
extern void FAR * PASCAL LOADDS LDSFrealloc(void FAR *,size_t);
extern void PASCAL LOADDS LDSFfree(void FAR *);
extern void ModLoad(LSZ,BOOL) ;
extern BOOL LOADDS PASCAL SYFHasSource(HPID,LPADDR) ;
extern "C" BOOL cvw3_minit(void);
extern "C" void cvw3_mcleanup(void);
extern BOOL PASCAL SHFInit(void);
extern int fBlocking ;
extern unsigned long ulChildFlags ;
extern fEnvir fEnvirGbl ;
extern CVF FAR * lpcvf;

// CVEXTRAS items that need C linkage for access by vcnt.lib
extern char is_assign ;

// ERRORWIN.C
extern VOID PASCAL DBGOutputDebugString(LPSTR,BOOL) ;

// CL.C prototypes
extern PCXF LOADDS PASCAL CLGetFuncCXF(PADDR paddr,PCXF pcxf) ;

// CVEXTRAS.C externs and prototypes
extern ushort ArrayDefault ;
extern ADDR asmaddr ;

// DAMFILE.C
extern void DAMSaveDebugFile( LPCSTR, HSF );

// From BREAKPTS.C
extern int PASCAL ClearCV400Breakpoints(void) ;

//extern BOOL FAR fIsLaunchingDebuggee;

extern CXF cxfIp ;                     // Global context
extern char FAR DebugMessage[256];

// SYSTEM.C prototypes and variables
extern void FAR CDECL LOADDS CVDisconnect( void ) ;
extern void _HUGE_ * PASCAL LOADDS LDShalloc(long,size_t) ;
extern SHF FAR * lpshf;

extern void PASCAL LOADDS LDShfree(void _HUGE_ *) ;
extern void SYSetContext(HPRC,HTHD,BOOL) ;
extern void UpdateIn386Mode(void) ;
extern EPI epiDef ;
extern void SYContinue(HPRC) ;
extern void SYStop(HPRC);
extern void SYPause(HPRC);
extern void SYInstallVirtual(HPID,LSZ,BOOL) ;
extern BOOL PASCAL BPMsgBpt ( BOOL, LPMSGI );

extern BOOL fCanKillChild ;
extern HEM	hemCurr ;		// handle on current execution model
extern HEM	hnmCurr ;		// handle on current non-native execution model
extern HTL	htlCurr ;		// handle on current transport layer
extern HLLI llprc ;         // handle on processes linked list
extern LPDBF lpdbf ;        // Pointer to DBF entry structure


extern BOOL OmfLoaded;
extern BOOL fInFindLocalDlg;
extern CRITICAL_SECTION csDbgTimerSet; // guards fDbgTimerSet
extern BOOL fBPsBoundSuccessfully;
extern HMEM 	hprcCurr ;	// handle on current process
extern LPPRC	lpprcCurr ;	// pointer to current process in list
extern HMEM 	hthdCurr ;	// handle on current thread in list

extern PEXF pEXF ;

extern HLLI	qCallBack;				// Our internal notifications linked list
extern HLLI qCallBackReturns;		// Return codes from processed notifications

extern DWORD dwFuncTimeOut;
extern char FAR AlternatePath[255] ;

extern BOOL PASCAL LOADDS RECompare(char FAR *,char FAR *) ;
extern int  PASCAL TDCompare(LPTHD lpthd,unsigned long FAR *lpl,LONG l) ;
extern void AddThread(HPID,HTID) ;
extern void set_addrs(void) ;

extern BOOL FSetTLName(LPSTR szNewDLLName);
extern LPSTR SzGetTLName(void);
extern void QCTLSetup(HWND hwndOwner);
extern void ActivateRemoteDebugger(void);

extern HLLI hlliFME;
extern int fBPCCInExe;
extern char fCaseSensitive;
extern fEnvir fEnvirGbl ;

extern void fetch_error(ushort, char FAR *);
extern char FAR *fetch_signal(int);

extern int PASCAL BPNbrAtAddr(LPADDR,UOFFSET);

extern BOOL fLoadingJitProject;

//RunDebug Params
extern RUNDEBUGPARAMS runDebugParams;

extern ADDR dump_start_address;
extern char flip_or_swap;
extern fEnvir fEnvirGbl;

extern VOID PASCAL ControlExecDebuggee(WORD, WORD);

extern BOOL fExcepListInitialized;

extern void ActivateRemoteDebugger(void);
extern void PrereadFile();

extern UINT WU_QUERYJIT;
extern UINT WU_BEGINJIT;

// Last document of type DOC_WIN that had the focus
extern CIDEDoc  *pLastDocWin;

extern char		fCaseSensitive;

extern VOID PASCAL UpdateAllDebugWindows(PCXF) ;

interface IDBGProj;

extern PSRCUTIL         gpISrc;
extern LPBUILDSYSTEM    gpIBldSys;
extern LPBUILDPLATFORMS gpIBldPlatforms;
extern LPSOURCEQUERY    gpISrcQuery;
extern LPOUTPUTWINDOW	gpIOutputWindow;
extern IProjectWorkspace *gpIProjectWorkspace;
extern IPkgProject		 *gpActiveIPkgProject;
extern IDBGProj			 *gpActiveIDBGProj;

// Use this to get the interfaces, doing a QI if necessary
extern IPkgProject		*GetActiveIPkgProject(void);
extern IDBGProj			*GetActiveIDBGProj(void);

extern CDebug   *pDebugCurr;
extern BOOL     fVisTestPresent; // Is the Visual Test package present.
extern BOOL     fVisTestDebugActive; // Is a Visual Test debug session active currently.
extern void     UpdateMultiEditScroll( CMultiEdit * );

// Full Screen Data
extern FULLSCREENDATA FullScreenData;

//Debugger window view #
extern CIDEView *	pViewCpu;
extern CIDEView *	pViewMemory;
extern CIDEView *	pViewCalls;
extern CIDEView *	pViewDisassy;
extern CIDEView *	pViewNewWatch;
extern CIDEView *	pViewNewLocals;

//CrLf
extern char const CrLf[3];

extern HWND g_hWndShell;

//Handle to edit control
extern HWND hwndActiveEdit;

// Handle to currently activated child
extern HWND hwndActive;

//Main window frame
extern HWND hwndFrame;

// If you use these, you have to have a local copy.
extern UINT WM_VIEWACTIVATE;

extern DWORD g_dwDragDelay;
extern DWORD g_dwDragScrollInset;
extern DWORD g_dwDragScrollDelay;
extern DWORD g_dwDragScrollInterval;
extern DWORD g_dwDragMinRadius;

// clipboard formats
extern UINT g_cfTMText;
extern UINT g_cfColSelect;

#endif	// EXTERN_H
