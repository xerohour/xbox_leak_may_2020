//
// DBGAPI.H
//    Debug package interface declarations.

#ifndef __DBGAPI_H__
#define __DBGAPI_H__

#ifndef OSDEBUG4
typedef FD FAR *  LPFD;
typedef RD FAR *  LPRD;
#endif // OSDEBUG4

interface IDebugCmd;
interface IOSDebug;
interface IExprEval;
interface ISymbolHandler;
interface IMiscDebug;

typedef IDebugCmd *     LPDEBUGCMD;
typedef IOSDebug  *     LPOSDEBUG;
typedef IExprEval *     LPEXPREVAL;
typedef ISymbolHandler* LPSYMBOLHANDLER;
typedef IMiscDebug *    LPMISCDEBUG;

// BPChange Action Items
#define BP_DISABLE   0
#define BP_ENABLE    1
#define BP_CLEAR     2

//
// IDebugCmd interface
//
#undef  INTERFACE
#define INTERFACE IDebugCmd

DECLARE_INTERFACE_(IDebugCmd, IUnknown)
{
   // IUnknown methods
   STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
   STDMETHOD_(ULONG,AddRef)(THIS) PURE;
   STDMETHOD_(ULONG,Release)(THIS) PURE;

   // IDebugCmd methods

//   Attach - not sure how to do it

//   Kill - have to wait; not sure its purpose

   // This routine changes the current Frame to the nFrameNumber one.
   // If successful, it returns S_OK.
   // Windbg equivalent: % <num>
   STDMETHOD(FrameChange)(
      INT nFrameNumber,
      PCXF pcxf = NULL,
      BOOL fUpdateDisplay = TRUE
   ) PURE;

   // This routine changes the main window title of the debugger.
   // If successful, it returns TRUE.
   // Windbg equivalent: .title <title>
   STDMETHOD(SetTitle)(
      LPSTR lszTitle
   ) PURE;

   // This routine changes the case sensitivity.
   // Used in windbg command options dialog.
   STDMETHOD(SetCaseSensitive)(
      BOOL fcSensitive
   ) PURE;

   // This routine changes the radix settings between hexadecimal and decimal
   // Windbg equivalent: N <8|16>
   STDMETHOD(SetRadix)(
      BOOL fdecimal
   ) PURE;

   // This routine returns the current radix settings.  It can be decimal
   // or hexadecimal.
   // Windbg equivalent: N
   STDMETHOD(IsRadixDecimal)(
      void
   ) PURE;

   // This routine returns TRUE and the next register structure in the
   // list.  The first register strucutres is returned if phwri is hwriNull.
   // Windbg equivalent: fr <reg>=<value>
   STDMETHOD(InitRWI)(
      RD    rd,
      LPRWI lprwi
   ) PURE;

   // This routine returns TRUE if the lprui register was successfully
   // updated with the data in lpur.
   // Windbg equivalent: fr <reg>=<value>
   STDMETHOD(WriteRegister)(
      LPRUI    lprui,
      LPUR     lpur
   ) PURE;

   STDMETHOD(LockRegister)(
      HRWI hrwi1,
      LPRWI *hrwi2
   ) PURE;

   STDMETHOD(UnlockRegister)(
      HRWI hrwi
   ) PURE;

   // Given a thread object, this routine updates it with info
   // from the next thread object in the list.
   // If fFirstThread is TRUE, this routine updates the given
   // thread object with the first thread object info.
   // Windbg equivalent: ~, F, Z
   STDMETHOD(GetThreadInfo)(
      BOOL           fFirstThread,
      THREAD_OBJECT  *pObject
   ) PURE;

   // This routine either suspends or resumes execution of the given thread
   // Windbg equivalent: F, Z
   STDMETHOD(SetThreadState)(
      THREAD_OBJECT  *pObject,
      BOOL           fSuspend
   ) PURE;

   // This routine moves cb bytes starting from addr0 to addr1
   // If successful, TRUE is returned.  It handles overlapped
   // moves correctly.  On failure, the content of the memory
   // is not restored.
   // Windbg equivalent: M
   STDMETHOD(MovMemory)(
      ADDR        addr0,
      size_t      cb,
      ADDR        addr1
   ) PURE;

   // This routine writes bBuff containing cb bytes into
   // memory starting at addr location
   // Windbg equivalent: E, FI
   STDMETHOD(EnterMemory)(
      ADDR        addr,
      size_t      cb,
      void FAR    *bBuff,
      DWORD       dwRepeatCount = 1,
      ADDR        *paddrFail = NULL
   ) PURE;

   // This routine performs a Go and if there is a first
   // chance exception, it will uses the given argument
   // to decide whether the exception should be passed to
   // the debuggee or being ignored.
   // Windbg equivalent: GH, GN
   STDMETHOD(GoException)(
      BOOL fHandled
   ) PURE;

   // This routine takes a bp and allows one of the three
   // operations on it, namely disable, enable, and remove.
   // Windbg equivalent: BC, BE, BD
   STDMETHOD(BPChange)(
      PBREAKPOINTNODE pbpn,
      INT             iAction,
      BOOL            fUpdateUI
   ) PURE;

   // This routine takes a bp declarations
   // and add it into the list.  Currently it
   // takes many arguments but will be refined
   // in the future.
   // Windbg equivalent: BP, BA
   STDMETHOD(BPSet)(
      BREAKPOINTTYPES bpType,
      LPTSTR lszLocation,
      LPTSTR lszExpression,
      LPTSTR lszLength,
      LPTSTR lszPassCnt,
      LPTSTR lszMessage,
      INT    *iRet
   ) PURE;

   // This routine takes the name of a debuggee,
   // its arguments,and start the debugging
   // Windbg equivalent: .start
   STDMETHOD(Start)(
      LPTSTR lszLocalExe,
      char * szDebuggeeArgs
   ) PURE;
   // This routine stops the debugging process - currently
   // not supported by windbg cmd window
   // Windbg equivalent: N/A
   STDMETHOD(Stop)(void) PURE;

   // This routine performs a halt to the debugging process
   // Windbg equivalent: ^C, .break
   STDMETHOD(Break)(void) PURE;

   // The Go routine should not pop up dialog since it is used in a command window
   // Windbg equivalent: g
   STDMETHOD(Go)(void) PURE;

   // This routine restarts the debuggee for debugging
   // Windbg equivalent: l
   STDMETHOD(Restart)(void) PURE;

   // The StepOver routine should take a repeat count arg
   // Windbg equivalent: P
   STDMETHOD(StepOver)(void) PURE;

   // The StepOut routine stops on returning from a routine - currently
   // not supported by windbg cmd window
   // Windbg equivalent: N/A
   STDMETHOD(StepOut)(void) PURE;

   // The TraceInto routine should take a repeat count arg
   // Windbg equivalent: T
   STDMETHOD(TraceInto)(void) PURE;

   // Misc low level routines
   // Windbg equivalent: BA, BP, BC, BD, BE, BL (helper)
   STDMETHOD(BHFirstBPNode)(PBREAKPOINTNODE *hBreakPointNode) PURE;
};

//
// IOSDebug interface
//
#undef  INTERFACE
#define INTERFACE IOSDebug

DECLARE_INTERFACE_(IOSDebug, IUnknown)
{
   // IUnknown methods
   STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
   STDMETHOD_(ULONG,AddRef)(THIS) PURE;
   STDMETHOD_(ULONG,Release)(THIS) PURE;

   // IOSDebug methods

   //
   // Target Load/Unload Operations
   //

   // Support Windbg command: .kill
   STDMETHOD(COSDProgramFree)(
      HPID hpid,
      XOSD *xosd
   ) PURE;

#ifdef OSDEBUG4
   // Support Windbg command: AttachDebuggee
   STDMETHOD(COSDSetPath)(
      HPID hpid,
      DWORD fSet,
      LPTSTR lszPath,
      XOSD  *xosd
   ) PURE;
#endif // OSDEBUG4

/*
   STDMETHOD(COSDProgramLoad)(
      HPID   hpid,
      LPTSTR lszRemoteExe,
      LPTSTR lszArgs,
      LPTSTR lszWorkingDir,
      LPTSTR lszDebugger,
      DWORD  dwFlags,
      XOSD   *xosd
   ) PURE;
*/

   STDMETHOD(COSDDebugActive)(
      HPID hpid,
      LPVOID lpvData,
      DWORD cbData,
      XOSD  *xosd
   ) PURE;

   //
   // Exception Operations
   //

#ifdef OSDEBUG4
   // Support Windbg command: SXD, SXE
   STDMETHOD(COSDSetExceptionState)(
      HPID hpid,
      HTID htid,
      LPEXCEPTION_DESCRIPTION lpExd,
      XOSD  *xosd
   ) PURE;
#endif // OSDEBUG4

#ifdef OSDEBUG4
   // Support Windbg command: SX
   STDMETHOD(COSDGetExceptionState)(
      HPID hpid,
      HTID htid,
      LPEXCEPTION_DESCRIPTION lpExd,
      EXCEPTION_CONTROL exf,
      XOSD  *xosd
   ) PURE;
#endif // OSDEBUG4

   //
   // Assemble Operations
   //

   // Support Windbg command: .list, DC, U
   STDMETHOD(COSDUnassemble)(
      HPID hpid,
      HTID htid,
      LPSDI lpsdi,
      XOSD  *xosd
   ) PURE;

#ifdef OSDEBUG4
   STDMETHOD(COSDGetPrevInst)(
      HPID hpid,
      HTID htid,
      LPADDR lpaddr,
      LPUOFFSET lpuoffset,
      XOSD  *xosd
   ) PURE;
#endif // OSDEBUG4

   //
   // Stack Operations
   //

   STDMETHOD(COSDGetFrame)(
       HPID hpid,
       HTID htid,
       DWORD cFrame,
       LPHTID lphtid,
       XOSD *xosd
   ) PURE;

   //
   // Address Operations
   //

   // Support Windbg command: C, FI
   STDMETHOD(COSDSetAddr)(
      HPID hpid,
      HTID htid,
      ADR adr,
      LPADDR lpaddr,
      XOSD  *xosd
   ) PURE;

   // Support Windbg command: D
   STDMETHOD(COSDGetAddr)(
      HPID hpid,
      HTID htid,
      ADR adr,
      LPADDR lpaddr,
      XOSD  *xosd
   ) PURE;

   // Support Windbg command: K
   STDMETHOD(COSDUnFixupAddr)(
      HPID hpid,
      HTID htid,
      LPADDR lpaddr,
      XOSD  *xosd
   ) PURE;

   // Subsitute for OSDPtrace
   STDMETHOD(COSDFixupAddr)(
      HPID hpid,
      HTID htid,
      LPADDR lpaddr,
      XOSD  *xosd
   ) PURE;

   // Support Windbg command: K, LN
   STDMETHOD(COSDSetEmi)(
      HPID hpid,
      HTID htid,
      LPADDR lpaddr,
      XOSD  *xosd
   ) PURE;

   STDMETHOD(COSDRegisterEmi)(
      HPID hpid,
      HEMI hemi,
      LPTSTR lsz,
      XOSD  *xosd
   ) PURE;

#ifdef OSDEBUG4
   STDMETHOD(COSDUnRegisterEmi)(
      HPID hpid,
      HEMI hemi,
      XOSD  *xosd
   ) PURE;
#endif

   STDMETHOD(COSDCompareAddrs)(
       HPID hpid,
       LPADDR lpaddr1,
       LPADDR lpaddr2,
       LPDWORD lpResult,
       XOSD *xosd
   ) PURE;

#ifdef OSDEBUG4
   STDMETHOD(COSDGetMemInfo)(
       HPID hpid,
       HTID htid,
       LPMEMINFO lpMemInfo,
       XOSD *xosd
   ) PURE;
#endif // OSDEBUG4

   //
   // Registers Operations
   //

   // Support Windbg command: FR
   STDMETHOD(COSDGetRegDesc)(
      HPID hpid,
      HTID htid,
      DWORD ird,
      LPRD lprd,
      XOSD  *xosd
   ) PURE;

   // Support Windbg command: FR
   STDMETHOD(COSDGetFlagDesc)(
      HPID hpid,
      HTID htid,
      DWORD ifd,
      LPFD lpfd,
      XOSD  *xosd
   ) PURE;

   // Support Windbg command: FR
   STDMETHOD(COSDReadRegister)(
      HPID hpid,
      HTID htid,
      DWORD dwid,
      LPVOID lpValue,
      XOSD  *xosd
   ) PURE;

   // Support Windbg command: FR
   STDMETHOD(COSDReadFlag)(
      HPID hpid,
      HTID htid,
      DWORD dwId,
      LPVOID lpValue,
      XOSD  *xosd
   ) PURE;

   //
   // Target Information
   //

   // Support Windbg command: FR, K
   STDMETHOD(COSDGetDebugMetric)(
      HPID hpid,
      HTID htid,
      MTRC mtrc,
      LPVOID lpv,
      XOSD  *xosd
   ) PURE;

   //
   // Module Operations
   //

#ifdef OSDEBUG4
   // Support Windbg command: LM, LN
   STDMETHOD(COSDGetModuleList)(
      HPID hpid,
      HTID htid,
      LPTSTR lszModuleName,
      LPMODULE_LIST FAR * lplpModuleList,
      XOSD  *xosd
   ) PURE;
#endif // OSDEBUG4

   //
   // TL Operations
   //

   // Support Windbg command: DisconnectDebugger
   STDMETHOD(COSDDeleteTL)(
      HTL htl,
      XOSD  *xosd
   ) PURE;

   //
   // EM Operations
   //

   // Support Windbg command: DisconnectDebugger
   STDMETHOD(COSDDeleteEM)(
      HEM hem,
      XOSD  *xosd
   ) PURE;

   //
   // Process and Thread Operations
   //

   // Support Windbg command: DisconnectDebugger
   STDMETHOD(COSDDestroyPID)(   // same as OSDDestroyPID
      HPID hpid,
      XOSD  *xosd
   ) PURE;

#ifdef OSDEBUG4
   STDMETHOD(COSDDestroyTID)(
      HPID hpid,
      HTID htid,
      XOSD  *xosd
   ) PURE;
#endif // OSDEBUG4

   //
   // Memory Operations
   //

   // Subsitute for OSDPtrace
   STDMETHOD(COSDReadMemory)(
      HPID hpid,
      HTID htid,
      LPADDR lpaddr,
#ifdef OSDEBUG4 // fix should be in od.h
      LPVOID lpBuffer,
#else
      LPB    lpBuffer,
#endif
      DWORD cbBuffer,
      LPDWORD lpcbRead,
      XOSD  *xosd
   ) PURE;

/*
   STDMETHOD(COSDWriteMemory)(
      HPID hpid,
      HTID htid,
      LPADDR lpaddr,
      LPBYTE lpbBuffer,
      DWORD cbBuffer,
      LPDWORD lpcbWritten,
      XOSD  *xosd
   ) PURE;
*/

   //
   // Process and Thread Status
   //

   STDMETHOD(COSDGetObjectLength)(
      HPID hpid,
      HTID htid,
      LPADDR lpaddr,
      LPUOFFSET lpuoffStart,
      LPUOFFSET lpuoffLength,
      XOSD  *xosd
   ) PURE;

   STDMETHOD(COSDGetThreadStatus)(
       HPID hpid,
       HTID htid,
       LPTST lptst,
       XOSD *xosd
   ) PURE;

   STDMETHOD(COSDGetProcessStatus)(
       HPID hpid,
       LPPST lppst,
       XOSD *xosd
   ) PURE;

   //
   // Message Handling
   //

   STDMETHOD(COSDGetMsgMap)(
      HPID            hpid,
      HTID            htid,
      LPMESSAGEMAP*   lplpMessageMap,
      XOSD  *xosd
   ) PURE;

   STDMETHOD(COSDGetMessageMaskMap)(
      HPID hpid,
      HTID htid,
      LPMASKMAP FAR * lplpMaskMap,
      XOSD  *xosd
   ) PURE;

   //
   // OS Specific Services
   //

#ifdef OSDEBUG4
   STDMETHOD(COSDGetTaskList)(
      HPID hpid,
      LPTASKLIST FAR * lplpTaskList,
      XOSD  *xosd
   ) PURE;
#endif // OSDEBUG4

   STDMETHOD(COSDSystemService)(
       HPID hpid,
       HTID htid,
       SSVC ssvc,
       LPVOID  lpvData,
       DWORD cbData,
       LPDWORD lpcbReturn,
       XOSD *xosd
   ) PURE;
};

//
// IExprEval Interface
//
#undef  INTERFACE
#define INTERFACE IExprEval

DECLARE_INTERFACE_(IExprEval, IUnknown)
{
   // IUnknown methods
   STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
   STDMETHOD_(ULONG,AddRef)(THIS) PURE;
   STDMETHOD_(ULONG,Release)(THIS) PURE;

   // IExprEval methods
   // Required
   STDMETHOD(CEEFreeStr)(
      EEHSTR hszStr
   ) PURE;

   STDMETHOD(CEEParse)(
      char FAR *szExpr,
      uint radix,
      SHFLAG fCase,
      PHTM phTM,
      ushort FAR *pEnd,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEBindTM)(
      PHTM phTM,
      PCXT pcxt,
      SHFLAG fForceBind,
      SHFLAG fEnableProlog,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEGetExprFromTM)(
      PHTM phTM,
      PEERADIX pRadix,
      PEEHSTR phStr,
      ushort FAR *pEnd,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEFormatCXTFromPCXT)(
      PCXT pCXT,
      PEEHSTR phStr,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEFreeTM)(
      PHTM phTM
   ) PURE;

   STDMETHOD(CEEGetCXTLFromTM)(
      PHTM phTM,
      PHCXTL phCXTL,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEGetTMFromHSYM)(
      HSYM hSym,
      PCXT pcxt,
      PHTM phTM,
      ushort FAR *pEnd,
      SHFLAG fForceBind,
      SHFLAG fEnableProlog,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEFormatAddress)(
       PADDR paddr,
       char FAR *szAddr
   ) PURE;

   STDMETHOD(CEEGetHSYMList)(
      HDEP FAR *phSYML,
      PCXT pCxt,
      ushort mask,
      uchar FAR * pRE,
      SHFLAG fEnableProlog,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEFreeHSYMList)(
      HDEP FAR *phSYML
   ) PURE;

   // Optional
   STDMETHOD(CEEGetError)(
      PHTM phTM,
      EESTATUS Status,
      PEEHSTR phError,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEvaluateTM)(
      PHTM phTM,
      PFRAME pFrame,
      EEDSP style,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEGetValueFromTM)(
      PHTM phTM,
      uint Radix,
      PEEFORMAT pFormat,
      PEEHSTR phszValue,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEGetNameFromTM)(
      PHTM phTM,
      PEEHSTR phszName,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEGetTypeFromTM)(
      PHTM phTM,
      EEHSTR hszName,
      PEEHSTR phszType,
      ulong select,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEParseBP)(
      char FAR *pExpr,
      uint radix,
      SHFLAG fCase,
      PCXF pCxf,
      PTML pTML,
      ulong select,
      ushort FAR *pEnd,
      SHFLAG fEnableProlog,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEFreeTML)(
      PTML pTML
   ) PURE;

   STDMETHOD(CEEInfoFromTM)(
      PHTM phTM,
      PRI pReqInfo,
      PHTI phTMInfo,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEFreeTI)(
      PHTI phTI
   ) PURE;

   STDMETHOD(CEEFreeCXTL)(
      PHCXTL phCXTL
   ) PURE;

   STDMETHOD(CEEAssignTMToTM)(
      PHTM     phTMLeft,
      PHTM     phTMRight,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEIsExpandable)(
      PHTM     phTM,
      EEPDTYP  *eepdtyp
   ) PURE;

   STDMETHOD(CEEAreTypesEqual)(
      PHTM     phTMLeft,
      PHTM     phTMRight,
      SHFLAG   *shflag
   ) PURE;

   STDMETHOD(CEEcChildrenTM)(
      PHTM phTM,
      DWORD *pcChildren,
      PSHFLAG pVar,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEGetChildTM)(
      PHTM phTMIn,
      long iChild,
      PHTM phTMOut,
      ushort FAR *pEnd,
      EERADIX eeRadix,
      SHFLAG fCase,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEDereferenceTM)(
      PHTM phTMIn,
      PHTM phTMOut,
      ushort FAR *pEnd,
      SHFLAG fCase,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEcParamTM)(
      PHTM phTM,
      ushort FAR *pcParam,
      PSHFLAG pVarArg,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEGetParmTM)(
      PHTM phTMIn,
      ushort iParam,
      PHTM phTMOut,
      ushort FAR *pEnd,
      SHFLAG fCase,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEGetExtendedTypeInfo)(
      PHTM phTM,
      PETI pETI,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEGetAccessFromTM)(
      PHTM phTM,
      PEEHSTR phszAccess,
      ulong format,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEEnableAutoClassCast)(
      BOOL fNew
   ) PURE;

   STDMETHOD(CEEInvalidateCache)(
      void
   ) PURE;

   STDMETHOD(CEEcSynthChildTM)(
      PHTM phTM,
      long FAR *pcChildren,
      EESTATUS *eestatus
   ) PURE;

   STDMETHOD(CEEGetBCIA)(
       HTM * pHTM,
       PHBCIA pHBCIA,
       EESTATUS   *eestatus
   ) PURE;

   STDMETHOD(CEEFreeBCIA)(
       PHBCIA pHBCIA
   ) PURE;

   STDMETHOD(Evaluate)(
      LSZ    lpsz,
      PCXF   pcxf,
      PHTM   phTm,
      ushort *pEnd,
      EESTATUS *eestatus
   ) PURE;
};

//
// ISymbolHandler interface
//
#undef  INTERFACE
#define INTERFACE ISymbolHandler

DECLARE_INTERFACE_(ISymbolHandler, IUnknown)
{
   // IUnknown methods
   STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
   STDMETHOD_(ULONG,AddRef)(THIS) PURE;
   STDMETHOD_(ULONG,Release)(THIS) PURE;

   // ISymbolHandler methods
   // Required
   STDMETHOD(CSHCreateProcess)(
      HPDS  *hpdsOut
   ) PURE;

   STDMETHOD(CSHSetHpid)(
      HPID hpid
   ) PURE;

   STDMETHOD(CSHDeleteProcess)(
      HPDS hpds
   ) PURE;

   STDMETHOD(CSHChangeProcess)(
      HPDS hpds
   ) PURE;

   STDMETHOD(CSHGetSymName)(
      HSYM hsym,
      LSZ lszIn,
      LSZ *lszOut
   ) PURE;

   STDMETHOD(CSHAddrFromHsym)(
      LPADDR paddr,
      HSYM hsym
   ) PURE;

   STDMETHOD(CSHGetNextMod)(
      HEXE hexe,
      HMOD hmodIn,
      HMOD *hmodOut
   ) PURE;

   STDMETHOD(CSHSetCxt)(
      LPADDR paddr,
      PCXT pcxtIn,
      PCXT *pcxtOut
   ) PURE;

   STDMETHOD(CSHGethExeFromName)(
      LPTSTR ltszPath,
      HEXE *hexeOut
   ) PURE;

   STDMETHOD(CPHGetNearestHsym)(
      LPADDR   lpaddr,
      HEXE     hexe,
      PHSYM    phsym,
      CV_uoff32_t *cvoffOut
   ) PURE;

   STDMETHOD(CSLLineFromAddr)(
       LPADDR  lpaddr,
       LPW     lpwLine,
       SHOFF * lpcb,
       SHOFF * lpdb
   ) PURE;

   STDMETHOD(CSHWantSymbols)(
      HEXE  hexe
   ) PURE;

   STDMETHOD(CSHGetSymFName)(
      HEXE hexe,
      LSZ  *lszOut
   ) PURE;

   STDMETHOD(CSHGetDebugData)(
      HEXE hexe,
      LPDEBUGDATA *lpDebugDataOut
   ) PURE;

   STDMETHOD(CSHGetModNameFromHexe)(
      HEXE hexe,
      LSZ  *lszOut
   ) PURE;

   STDMETHOD(CSHLszGetErrorText)(
      SHE she,
      LSZ *lszOut
   ) PURE;

   STDMETHOD(CSHGetModule)(
      LPADDR paddrOp,
      LSZ    rgbName,
      LSZ    *lszOut
   ) PURE;

   STDMETHOD(CSHUnloadSymbolHandler)(
      BOOL fResetLists
   ) PURE;

   STDMETHOD(CSHFree)(
      LPV lpv
   ) PURE;

   STDMETHOD(CSHGetDebugStart)(
      HSYM hsym,
      UOFFSET *offOut
   ) PURE;

   STDMETHOD(CSHHModGetNextGlobal)(
      HEXE FAR *phexe,
      HMOD hmodIn,
      HMOD *hmodOut
   ) PURE;

   STDMETHOD(CSHGetSymbol)(
       LPADDR paddrOp,
       LPADDR paddrLoc,
       SOP    sop,
       LPODR  lpodr,
       LSZ    *lszOut
   ) PURE;

   STDMETHOD(CSHGetPublicAddr)(
      LPADDR paddr,
      LSZ    lszName
   ) PURE;

   STDMETHOD(CSHAddDll)(
      LSZ   lsz,
      BOOL  fDll,
      SHE   *sheOut
   ) PURE;

   STDMETHOD(CSHIsLabel)(
      HSYM hsym
   ) PURE;

   STDMETHOD(CSHSetDebuggeeDir)(
      LSZ lszDir
   ) PURE;

   STDMETHOD(CSHUnloadDll)(
      HEXE hexe
   ) PURE;

   STDMETHOD(CSHLoadDll)(
      LSZ  lszName,
      BOOL fLoading,
      SHE  *sheOut
   ) PURE;

   STDMETHOD(CSHAddDllsToProcess)(
      SHE *sheOut
   ) PURE;

   STDMETHOD(CSHHexeFromHmod)(
      HMOD hmod,
      HEXE *hexeOut
   ) PURE;

   STDMETHOD(CSHGetNextExe)(
      HEXE hexeIn,
      HEXE *hexeOut
   ) PURE;

   STDMETHOD(CSHGetCxtFromHmod)(
      HMOD hmod,
      PCXT pcxtIn,
      PCXT *pcxtOut
   ) PURE;

   STDMETHOD(CSHSetCxtMod)(
      LPADDR paddr,
      PCXT  pcxtIn,
      PCXT  *pcxtOut
   ) PURE;

   STDMETHOD(CSHFindNameInGlobal)(
      HSYM   hsym,
      PCXT   pcxt,
      LPSSTR lpsstr,
      SHFLAG fCase,
      PFNCMP pfnCmp,
      PCXT   pcxtOut,
      HSYM   *hSymOut
   ) PURE;

   STDMETHOD(CSHFindNameInContext)(
      HSYM    hSym,
      PCXT    pcxt,
      LPSSTR  lpsstr,
      SHFLAG  fCase,
      PFNCMP  pfnCmp,
      PCXT    pcxtOut,
      HSYM    *hSymOut
   ) PURE;

   STDMETHOD(CSHGoToParent)(
      PCXT pcxt,
      PCXT pcxtOut,
      HSYM *hSymOut
   ) PURE;

   STDMETHOD(CSHHsymFromPcxt)(
      PCXT pcxt,
      HSYM *hSymOut
   ) PURE;

   STDMETHOD(CSHNextHsym)(
      HMOD hmod,
      HSYM hSymIn,
      HSYM *hSymOut
   ) PURE;

   STDMETHOD(CSHCompareRE)(
      LPCH psym,
      LPCH pRe,
      SHFLAG *shflagOut
   ) PURE;

   STDMETHOD(CSHGetModName)(
      HMOD  hmod,
      LSZ *lszOut
   ) PURE;

   STDMETHOD(CSHGetExeName)(
      HEXE hexe,
      LSZ *lszOut
   ) PURE;

   STDMETHOD(CSHCanDisplay)(
      HSYM hsym
   ) PURE;

   STDMETHOD(CSHGetNearestHsym)(
      LPADDR paddr,
      HMOD hmod,
      int mDataCode,
      PHSYM phSym,
      UOFF32 *offOut
   ) PURE;

   STDMETHOD(CSHFindSymInExe)(
      HEXE hexe,
      LPSSTR lpsstr,
      BOOL fCaseSensitive,
      HSYM *hSymOut
   ) PURE;

   STDMETHOD(CSHModelFromAddr)(
      LPADDR   paddr,
      LPW      lpwModel,
      LPB      lpbModel,
      CV_uoff32_t FAR *pobMax,
      int      *Out
   ) PURE;

   STDMETHOD(CSHPublicNameToAddr)(
      PADDR loc,
      PADDR pMpAddr,
      LSZ   lszName,
      int   *Out
   ) PURE;

   STDMETHOD(CSHAddrToLabel)(
      LPADDR paddr,
      LSZ lsz
   ) PURE;

   STDMETHOD(CSHGetSymLoc)(
      HSYM  hsym,
      LSZ   lsz,
      UINT  cbMax,
      PCXT  pcxt,
      int   *Out
   ) PURE;

   STDMETHOD(CSHFIsAddrNonVirtual)(
      LPADDR paddr
   ) PURE;

   STDMETHOD(CSHIsFarProc)(
      HSYM hsym
   ) PURE;

   STDMETHOD(CSHGetFuncCXF)(
      PADDR    paddr,
      PCXF     pcxfIn,
      PCXF     *pcxfOut
   ) PURE;

   STDMETHOD(CSHIsInProlog)(
      PCXT     pcxt,
      SHFLAG *shflagOut
   ) PURE;

   STDMETHOD(CSHIsAddrInCxt)(
      PCXT     pcxt,
      LPADDR   paddr,
      SHFLAG *shflagOut
   ) PURE;

   STDMETHOD(CPHFindNameInPublics)(
      HSYM    hsym,
      HEXE    hexe,
      LPSSTR  lpsstr,
      SHFLAG  fCase,
      PFNCMP  pfnCmp,
      HSYM *hSymOut
   ) PURE;

   STDMETHOD(CTHGetTypeFromIndex)(
      HMOD hmod,
      THIDX index,
      HTYPE *htypeOut
   ) PURE;

   STDMETHOD(CTHGetNextType)(
      HMOD hmod,
      HTYPE hType,
      HTYPE *htypeOut
   ) PURE;

   STDMETHOD(CSHLpGSNGetTable)(
      HEXE hexe,
      LPV *Out
   ) PURE;

   STDMETHOD(CSHFindSymbol)(
      LSZ   lsz,
      PADDR lpaddr,
      LPASR lpasr
   ) PURE;

   STDMETHOD(CSLFLineToAddr)(
      HSF         hsf,
      WORD        line,
      LPADDR      lpaddr,
      SHOFF FAR   *lpcbLn,
      WORD FAR    *rgwNearestLines
   ) PURE;

   STDMETHOD(CSLNameFromHsf)(
      HSF hsf,
      LPCH *lpchOut
   ) PURE;

   STDMETHOD(CSLNameFromHmod)(
      HMOD hmod,
      WORD iFile,
      LPCH *lpchOut
   ) PURE;

   STDMETHOD(CSLFQueryModSrc)(
      HMOD hmod
   ) PURE;

   STDMETHOD(CSLHmodFromHsf)(
      HEXE hexe,
      HSF hsf,
      HMOD *hModOut
   ) PURE;

   STDMETHOD(CSLHsfFromPcxt)(
      PCXT pcxt,
      HSF *hsfOut
   ) PURE;

   STDMETHOD(CSLHsfFromFile)(
      HMOD hmod,
      LSZ lszFile,
      HSF *hsfOut
   ) PURE;

   STDMETHOD(CSLCAddrFromLine)(
      HEXE hexeStart,
      HMOD hmodStart,
      LSZ  lszFileT,
      WORD line,
      LPSLP FAR * lplpslp,
      int *Out
   ) PURE;

   STDMETHOD(CSHPdbNameFromExe)(
      LSZ lszExe,
      LSZ lszPdbName,
      UINT cbMax
   ) PURE;

   STDMETHOD(CSHGetExeTimeStamp)(
      LSZ   szExeName,
      ULONG *lplTimeStamp,
      SHE *sheOut
   ) PURE;

   STDMETHOD(CSHIsThunk)(
      HSYM hsym
   ) PURE;

   STDMETHOD(CSHIsDllLoaded)(
      HEXE hexe
   ) PURE;
};

//
// IMiscDebug
//
#undef  INTERFACE
#define INTERFACE IMiscDebug

DECLARE_INTERFACE_(IMiscDebug, IUnknown)
{
   // IUnknown methods
   STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
   STDMETHOD_(ULONG,AddRef)(THIS) PURE;
   STDMETHOD_(ULONG,Release)(THIS) PURE;

   // IMiscDebug style methods
   STDMETHOD(SYFixupAddr)(
      PADDR
   ) PURE;

   STDMETHOD(SYUnFixupAddr)(
      PADDR
   ) PURE;

   STDMETHOD(DebuggeeAlive)(
      void
   ) PURE;

   STDMETHOD(DebuggeeActive)(
      void
   ) PURE;

   STDMETHOD(DebuggeeRunning)(
      void
   ) PURE;

   STDMETHOD(UpdateDebuggerState)(
      WORD UpdateFlags
   ) PURE;

   // Support Windbg command: .list, K
   STDMETHOD(GetSourceFromAddr)(
      LPADDR   pAddr,
      LPSTR    SrcName,
      WORD     SrcLen,
      LONG FAR *pSrcLine,
      UOFFSET FAR *pDelta,
      HSF FAR  *lphsf
   ) PURE;

   // Support Windbg command: .list
   STDMETHOD(FindDoc)(
      LPCSTR fileName,
      PTDOC *ppTextDoc,
      BOOL  docOnly
   ) PURE;

   STDMETHOD(theRadix)(
      INT   **Out
   ) PURE;

   STDMETHOD(CurrHPRC)(
      HMEM  **Out
   ) PURE;

   STDMETHOD(CurrLPPRC)(
      LPPRC **Out
   ) PURE;

   STDMETHOD(CurrHTHD)(
      HMEM  **Out
   ) PURE;

   STDMETHOD(CurrLPTHD)(
      LPTHD **Out
   ) PURE;

   STDMETHOD(CxfIp)(
      CXF   **Out
   ) PURE;

   STDMETHOD(BMLock)(
      HMEM  hmem,
      LPV   *Out
   ) PURE;

   STDMETHOD(BMUnlock)(
      HMEM  hmem
   ) PURE;

   STDMETHOD(MHAlloc)(
      size_t   size,
      void FAR *  *Out
   ) PURE;

   STDMETHOD(MHRealloc)(
      void FAR *buffer,
      size_t   size,
      void FAR *  *Out
   ) PURE;

   STDMETHOD(MHFree)(
      void FAR *buffer
   ) PURE;

   STDMETHOD(fCaseSensitive)(
      char **Out
   ) PURE;

#undef AuxPrintf
   STDMETHOD(AuxPrintf)(
      LPSTR text,
      ...
   ) PURE;

   STDMETHOD(LLHlleFindNext)(
      HLLI    hlli,
      HLLE    hlle,
      HLLE    *Out
   ) PURE;

   STDMETHOD(LLLpvFromHlle)(
      HLLE  hlle,
      LPV   *Out
   ) PURE;

   STDMETHOD(LlthdFromHprc)(
      HPRC  hprc,
      HLLI  *Out
   ) PURE;

   STDMETHOD(ValidhprcFromPid)(
      unsigned long  pid,
      HPRC           *Out
   ) PURE;

   STDMETHOD(HthdFromTid)(
      HPRC           hprc,
      unsigned long  tid,
      HTHD           *Out
   ) PURE;

   STDMETHOD(GetHprcHead)(
      HPRC *Out
   ) PURE;

   STDMETHOD(GetLLPRC)(
      HLLI *Out
   ) PURE;

#if defined(_X86_)
   STDMETHOD(SzFromLd)(
      char     *pchBuf,
      size_t   cb,
      FLOAT10  f10,
      char *   *Out
   ) PURE;

   STDMETHOD(LdFromSz)(
      char    * szFloat,
      char    ** ppchEnd,
      FLOAT10 *Out
   ) PURE;
#endif // _X86_

   STDMETHOD(SYGetAddr)(
      HPID  hpid,
      HTID  htid,
      ADR   adr,
      LPADDR   lpaddr,
      XOSD  *Out
   ) PURE;

   // Support Windbg command??: .list
   STDMETHOD(FGetLine)(
      PTDOC doc,
      PCSTR &pcstr,
      ILINE iline
   ) PURE;
};

#endif   // __DBGAPI_H__
