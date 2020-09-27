#ifndef _WKSELSATOOLSDEBUG_H
#define _WKSELSATOOLSDEBUG_H
//*****************************Module*Header******************************
//
// Module Name: wkselsatoolsdebug.h
//
// compatibility code only 
//
// FNicklisch 09/14/2000: New, derived from ELSA debug.h
//
// This part of code was taken from the code bought from ELSA. Parts of 
// it is used to get the workstation tools, application and features up
// and running. It is implemented on a temporary base and will be replaced
// by a NVIDIA propritary interface as soon as possible.
// Don't care about the code not necessariliy used.
//
// Copyright (c) 1999-2000, ELSA AG, Aachen (Germany)
// Copyright (c) 2000 NVidia Corporation. All Rights Reserved.
//

#ifdef USE_WKS_ELSA_TOOLS
#if DBG

#define DISPDBGCOND(cond,arg)                 if(cond)DebugPrint arg; else NULL
#define DBG_CURRENTPOSITION DISPDBG((0, "file:%s, line:%d",__FILE__,__LINE__))

#ifdef i386
#define DBG_HW_INDEP_BREAK { __asm {int 3} }
#else
#define DBG_HW_INDEP_BREAK EngDebugBreak()
#endif

#undef RIP
// FN more comfortable RIP
#define RIP(text)                                     \
  {                                                   \
    ENG_TIME_FIELDS localTime;                        \
    EngQueryLocalTime(&localTime);                    \
    DebugPrint(0, "RIP %s in %s, %d at %02d:%02d on %02d.%02d.%04d", \
      text?text:"",__FILE__,__LINE__,localTime.usHour,localTime.usMinute,localTime.usDay,localTime.usMonth,localTime.usYear);\
    DBG_HW_INDEP_BREAK;                                        \
    DebugPrint(0, "RIP %s in %s, %d at %02d:%02d on %02d.%02d.%04d", \
      text?text:"",__FILE__,__LINE__,localTime.usHour,localTime.usMinute,localTime.usDay,localTime.usMonth,localTime.usYear);\
  }

#ifdef _WIN64 
  // FNicklis IA-64: Rip that only works on IA64
  #define RIP_IA64(text)                                                           \
    DebugPrint(0, "RIP %s: FNicklis IA-64: Code possibly crashes on IA64",text?text:""); \
    RIP(text)
#else 
  #define RIP_IA64(text)
#endif

#define DBGBREAK()                                    \
  {                                                   \
    ENG_TIME_FIELDS localTime;                        \
    EngQueryLocalTime(&localTime);                    \
    DebugPrint(0, "BREAK in %s, %d at %02d:%02d on %02d.%02d.%04d", \
      __FILE__,__LINE__,localTime.usHour,localTime.usMinute,localTime.usDay,localTime.usMonth,localTime.usYear);\
    DBG_HW_INDEP_BREAK;                                        \
    DebugPrint(0, "BREAK in %s, %d at %02d:%02d on %02d.%02d.%04d", \
      __FILE__,__LINE__,localTime.usHour,localTime.usMinute,localTime.usDay,localTime.usMonth,localTime.usYear);\
  }

#undef ASSERTDD
// FN more comfortable ASSERTDD
#define ASSERTDD(condition, text)                                  \
  if (FALSE==(condition))                                          \
  {                                                                \
    DebugPrint(0, "ASSERTION \"%s\" failed: %s (%s,%d)",#condition, text,__FILE__,__LINE__); \
    DBG_TRACE_DUMP(0);                                             \
    DBGBREAK(); /* No more output (text) needed! */                 \
    DebugPrint(0, "ASSERTION \"%s\" failed: %s (%s,%d)",#condition, text,__FILE__,__LINE__); \
  }

#undef ASSERT // remove if defined!
#define ASSERT(condition) ASSERTDD(condition,"")
#define DBG_LVL_SWITCHFLAG      0 // ( BIT(31) ) // DON'T USE THIS, BECAUSE THIS IS THE FLAG TO SWITCH
                                            // BETWEEN FLAGED ANS UNFLAGED DEBUGLEVEL

#define DBG_LVL_0               0 // ( DBG_LVL_SWITCHFLAG | BIT(0)|BIT(1)|BIT(2)|BIT(3)) // bit 0-3 but not break!
#define DBG_LVL_ENTRY           5 // ( DBG_LVL_SWITCHFLAG | BIT(0) ) // for Entry to driver function
#define DBG_LVL_PARAM           7 // ( DBG_LVL_SWITCHFLAG | BIT(1) ) // info for functions parameter
#define DBG_LVL_OPENPARAM       8 // ( DBG_LVL_SWITCHFLAG | BIT(2) ) // db want's it for extended info for functions parameter
#define DBG_LVL_PUNT            4 // ( DBG_LVL_SWITCHFLAG | BIT(3) ) // when punting function to engine

#define DBG_LVL_1               0 // ( DBG_LVL_SWITCHFLAG | BIT(4)|BIT(5)|BIT(6)|BIT(7)) // bit 4-7
#define DBG_LVL_SUBENTRY        6 // ( DBG_LVL_SWITCHFLAG | BIT(4) ) // Output for sub entry/function
#define DBG_LVL_FLOW0           8 // ( DBG_LVL_SWITCHFLAG | BIT(5) ) // 
#define DBG_LVL_FLOW1           9 // ( DBG_LVL_SWITCHFLAG | BIT(6) )
#define DBG_LVL_FLOW2           10// ( DBG_LVL_SWITCHFLAG | BIT(7) )

#define DBG_LVL_2               0 // ( DBG_LVL_SWITCHFLAG | BIT(8)|BIT(9)|BIT(10)|BIT(11)) // bit 8-11
#define DBG_LVL_IA64            7 // ( DBG_LVL_SWITCHFLAG | BIT(8) )  // for output that is not for others interest
#define DBG_LVL_DB              1 // ( DBG_LVL_SWITCHFLAG | BIT(9) )
#define DBG_LVL_FN              1 // ( DBG_LVL_SWITCHFLAG | BIT(10) )
#define DBG_LVL_UNUSED11        10// ( DBG_LVL_SWITCHFLAG | BIT(11) )

#define DBG_LVL_3               0 // ( DBG_LVL_SWITCHFLAG | BIT(12)|BIT(13)|BIT(14)|BIT(15))
#define DBG_LVL_MS              1 // ( DBG_LVL_SWITCHFLAG | BIT(12) )
#define DBG_LVL_UNUSED13        10// ( DBG_LVL_SWITCHFLAG | BIT(13) ) // ***unused
#define DBG_LVL_UNUSED14        10// ( DBG_LVL_SWITCHFLAG | BIT(14) ) // ***unused
#define DBG_LVL_UNUSED15        10// ( DBG_LVL_SWITCHFLAG | BIT(15) ) // ***unused

#define DBG_LVL_4               0 // ( DBG_LVL_SWITCHFLAG | BIT(16)|BIT(17)|BIT(18)|BIT(19))
#define DBG_LVL_WARNING         2 // ( DBG_LVL_SWITCHFLAG | BIT(16) ) // for Warnings that aren't as severe as errors
#define DBG_LVL_ERROR           1 // ( DBG_LVL_SWITCHFLAG | BIT(17) ) // for output that isn't as severe as an failed assertion
#define DBG_LVL_UNUSED18        10// ( DBG_LVL_SWITCHFLAG | BIT(18) ) // ***unused
#define DBG_LVL_UNUSED19        10// ( DBG_LVL_SWITCHFLAG | BIT(19) ) // ***unused

#define DBG_LVL_5               0 // ( DBG_LVL_SWITCHFLAG | BIT(20)|BIT(21)|BIT(22)|BIT(23))
#define DBG_LVL_OUTS            10// ( DBG_LVL_SWITCHFLAG | BIT(20) ) // for data passed to HW
#define DBG_LVL_TEST            1 // ( DBG_LVL_SWITCHFLAG | BIT(21) ) // for temporay output, don't check in code to source safe with TEST set!
#define DBG_LVL_CONTEXT         10 // ( DBG_LVL_SWITCHFLAG | BIT(22) ) // for all output relating to contexts
#define DBG_LVL_UNUSED23        10 // ( DBG_LVL_SWITCHFLAG | BIT(23) ) // ***unused

#define DBG_LVL_6               0 // ( DBG_LVL_SWITCHFLAG | BIT(24)|BIT(25)|BIT(26)|BIT(27))
#define DBG_LVL_SWBC            8 // ( DBG_LVL_SWITCHFLAG | BIT(24) ) // for all output relating to sowftware broadcasting
#define DBG_LVL_RECT            10// ( DBG_LVL_SWITCHFLAG | BIT(25) ) // for all output relating to rectangles
#define DBG_LVL_HEAP            10// ( DBG_LVL_SWITCHFLAG | BIT(26) ) // for all output relating to heap
#define DBG_LVL_SYNC            10// ( DBG_LVL_SWITCHFLAG | BIT(27) ) // for all output relating to chip synchronization

// Signaling flags 
#define DBG_LVL_7               0 // ( BIT(28)|BIT(29)|BIT(30)|BIT(31) )
#define DBG_LVL_ENUMERATE       0 // ( BIT(28) ) // to enumerate lists, arrays etc instead of printing only pointer or one element
#define DBG_LVL_NOPREFIX        0 // ( BIT(29) ) // DebugPrint without prefix
#define DBG_LVL_NOLINEFEED      0 // ( BIT(30) ) // DebugPrint without linefeed
#define DBG_LVL_BIT31           0 // ( BIT(31) ) // DON'T USE THIS, BECAUSE THIS IS THE FLAG TO SWITCH
//                                          // BETWEEN FLAGGED AND UNFLAGGED DEBUGLEVEL

// Shorcuts to enable continuous lines (used by FN only?)
// Example:   DBG_TRACE_IN(DBG_LVL_SUBENTRY_NLF, functionX);           // continue line
//            DISPDBG((DBG_LVL_SUBENTRY_NPLF, "ppdev: 0x%x", ppdev));  // continued line to be continued again
//            DISPDBG((DBG_LVL_SUBENTRY_NP,   ", index: %d", index));  // continued line finished now
//            DBG_PRINTD(DBG_LVL_FLOW0, index);                        // new line (only to show result)
//          will print 
//            NV0:  functionX { ppdev:0x12345678, index: 4
//            NV0:    index: 4                            
#define DBG_LVL_ENTRY_NLF         ( DBG_LVL_ENTRY    | DBG_LVL_NOLINEFEED )
#define DBG_LVL_ENTRY_NPLF        ( DBG_LVL_ENTRY    | DBG_LVL_NOPREFIX   | DBG_LVL_NOLINEFEED)
#define DBG_LVL_ENTRY_NP          ( DBG_LVL_ENTRY    | DBG_LVL_NOPREFIX   )
#define DBG_LVL_SUBENTRY_NLF      ( DBG_LVL_SUBENTRY | DBG_LVL_NOLINEFEED )
#define DBG_LVL_SUBENTRY_NPLF     ( DBG_LVL_SUBENTRY | DBG_LVL_NOPREFIX   | DBG_LVL_NOLINEFEED)
#define DBG_LVL_SUBENTRY_NP       ( DBG_LVL_SUBENTRY | DBG_LVL_NOPREFIX   )
#define DBG_LVL_FLOW0_NLF         ( DBG_LVL_FLOW0    | DBG_LVL_NOLINEFEED )
#define DBG_LVL_FLOW0_NPLF        ( DBG_LVL_FLOW0    | DBG_LVL_NOPREFIX   | DBG_LVL_NOLINEFEED)
#define DBG_LVL_FLOW0_NP          ( DBG_LVL_FLOW0    | DBG_LVL_NOPREFIX   )
#define DBG_LVL_FLOW1_NLF         ( DBG_LVL_FLOW1    | DBG_LVL_NOLINEFEED )
#define DBG_LVL_FLOW1_NPLF        ( DBG_LVL_FLOW1    | DBG_LVL_NOPREFIX   | DBG_LVL_NOLINEFEED)
#define DBG_LVL_FLOW1_NP          ( DBG_LVL_FLOW1    | DBG_LVL_NOPREFIX   )
#define DBG_LVL_FLOW2_NLF         ( DBG_LVL_FLOW2    | DBG_LVL_NOLINEFEED )
#define DBG_LVL_FLOW2_NPLF        ( DBG_LVL_FLOW2    | DBG_LVL_NOPREFIX   | DBG_LVL_NOLINEFEED)
#define DBG_LVL_FLOW2_NP          ( DBG_LVL_FLOW2    | DBG_LVL_NOPREFIX   )
#define DBG_LVL_TEST_NLF          ( DBG_LVL_TEST     | DBG_LVL_NOLINEFEED )
#define DBG_LVL_TEST_NPLF         ( DBG_LVL_TEST     | DBG_LVL_NOPREFIX   | DBG_LVL_NOLINEFEED)
#define DBG_LVL_TEST_NP           ( DBG_LVL_TEST     | DBG_LVL_NOPREFIX   )

// Shortcuts (lower levels include higher - more important - levels)
#define DBG_LVL_2ERROR         (DBG_LVL_ERROR)
#define DBG_LVL_2WARNING       (DBG_LVL_2ERROR|DBG_LVL_WARNING)
#define DBG_LVL_2TEST          (DBG_LVL_2ERROR|DBG_LVL_TEST)
#define DBG_LVL_2ENTRY         (DBG_LVL_2WARNING|DBG_LVL_ENTRY)
#define DBG_LVL_2SUBENTRY      (DBG_LVL_2ENTRY|DBG_LVL_SUBENTRY)
#define DBG_LVL_2FLOW          (DBG_LVL_2SUBENTRY|DBG_LVL_FLOW0|DBG_LVL_FLOW1)
#define DBG_LVL_2DB            (DBG_LVL_2FLOW|DBG_LVL_DB|DBG_LVL_TEST)
#define DBG_LVL_2FN            (DBG_LVL_2FLOW|DBG_LVL_FN|DBG_LVL_TEST)
#define DBG_LVL_2SK            (DBG_LVL_2FLOW|DBG_LVL_SK|DBG_LVL_TEST)
#define DBG_LVL_2MS            (DBG_LVL_2FLOW|DBG_LVL_MS|DBG_LVL_TEST)
#define DBG_LVL_FULLOUTPUT     (DBG_LVL_0|DBG_LVL_1|DBG_LVL_4|DBG_LVL_5|DBG_LVL_6) // All but break, user depending and anoying output

#define DBG_LVL_FULLFN (DBG_LVL_FULLOUTPUT & ~(DBG_LVL_FLOW2|DBG_LVL_OUTS|DBG_LVL_HEAP))
#define DBG_LVL_FULLDB (DBG_LVL_FULLOUTPUT & ~DBG_LVL_OUTS)

#define DBG_LVL_CHECK(flDbgFlags,flFlag) ((flDbgFlags & ~DBG_LVL_7) & flFlag) // mask out signalling flags




extern VOID DebugGetFlagsAddress(void);
// In this array the global Debugflags are stored
#if 0 // FNicklisch 14.09.00: unused
extern FLONG flDbgFlags[DBG_MAX_GROUPS];
extern FLONG flDbgFlagsReset;
extern LONG  DebugLevel;
extern char szDbgStandardDebugPrefix[70];

// This variable conatines the stored flags local (!) to every module
static ULONG SavedFlags = 0; // **FN** 191196
#endif // FNicklisch 14.09.00: unused

#define DBGFLAGSSET(drventry,dbgflags) //                 { flDbgFlags[(drventry)]=(dbgflags); }
#define DBGFLAGSNEW(dbgflags)          //                 { SavedFlags=flDbgFlags[flDbgFlags[DBG_ACTUAL_FUNCTION]]; DBGFLAGSSET(flDbgFlags[DBG_ACTUAL_FUNCTION],dbgflags); }
#define DBGFLAGSOLD()                  //                 { flDbgFlags[flDbgFlags[DBG_ACTUAL_FUNCTION]]=SavedFlags; }

VOID vDbg_PrintPOINTL(FLONG flFlags, PCHAR pcString, const POINTL *pptl);
VOID vDbg_PrintRECTL(FLONG flFlags, PCHAR pcString, const RECTL *prcl);
VOID vDbg_PrintRECTLArray(FLONG flFlags, PCHAR pcString, const RECTL *prcl, LONG c);
VOID vDbg_PrintSIZEL(FLONG flFlags, PCHAR pcString, const SIZEL *psiz);

VOID vDbg_PrintCLIPOBJ(FLONG flFlags, PCHAR pcString, const CLIPOBJ *pco);
VOID vDbg_PrintCLIPOBJiDComplexity(FLONG flFlags, BYTE iDComplexity);
VOID vDbg_PrintCLIPOBJiFComplexity(FLONG flFlags, BYTE iFComplexity);

VOID vDbg_PrintXLATEOBJ(FLONG flFlags, PCHAR pcString, const XLATEOBJ *pxlo);
VOID vDbg_PrintXLATEOBJiType(FLONG flFlags, USHORT iType);
VOID vDbg_PrintXLATEOBJflXlate(FLONG flFlags, FLONG flXlate);

VOID vDbg_PrintSURFOBJ(FLONG flFlags, PCHAR pcString, const SURFOBJ *pso);
VOID vDbg_PrintSURFOBJiType(FLONG flFlags, USHORT  iType);
VOID vDbg_PrintSURFOBJiBitmapFormat(FLONG flFlags, ULONG iBitmapFormat);
char *szDbg_GetEscapeName(ULONG ulEsc);
char *szDbg_GetETSubEscName(LONG SubEscNr);

char *szDbg_TRACE_GetCurrentFn(VOID);
VOID vDbg_TRACE_IN(FLONG flDbgFlags, char *szFunction, PFN pfn);
VOID vDbg_TRACE_OUT(FLONG flDbgFlags, char *szRetVal, ULONG ulRetVal);
VOID vDbg_PrintTRACE_DUMP(FLONG flDbgLvl);

#define DBG_TRACE_IN(flDbgFlags,function) //vDbg_TRACE_IN((FLONG)flDbgFlags,#function,(PFN)function)
// FNicklis IA-64: Bad truncaton of a pointer to 32 bit!
#define DBG_TRACE_OUT(flDbgFlags,retVal)  //vDbg_TRACE_OUT((FLONG)flDbgFlags,#retVal,(ULONG)((ULONG_PTR)(retVal)))
#define DBG_TRACE_DUMP(flDbgLvl) //vDbg_PrintTRACE_DUMP(flDbgLvl)
#define DBG_ERROR(szText)   {static ULONG ulOutCount=0; if(ulOutCount++ <20) DISPDBG((DBG_LVL_ERROR, "ERROR in %s called by %s: %s",szDbg_TRACE_GetCurrentFn(),szGetApplicationName(), szText));}
#define DBG_WARNING(szText) {static ULONG ulOutCount=0; if(ulOutCount++ <20) DISPDBG((DBG_LVL_WARNING, "WARNING in %s called by %s: %s",szDbg_TRACE_GetCurrentFn(),szGetApplicationName(), szText));}
#define DBG_TEST(szText)    DISPDBG((DBG_LVL_TEST, "Testline in %s: %s (remove this output now:%s, line:%d!)",szDbg_TRACE_GetCurrentFn(),szText,__FILE__,__LINE__))
// FNicklis IA-64: Bad truncaton of a pointer to 32 bit!
#define DBG_PRINTX(flDbgFlags,variable) DISPDBG((flDbgFlags, "%s:0x%x",#variable,(ULONG)((ULONG_PTR)(variable))))
#define DBG_PRINTD(flDbgFlags,variable) DISPDBG((flDbgFlags, "%s:%d",#variable,variable))
#define DBG_PRINTS(flDbgFlags,variable) DISPDBG((flDbgFlags, "%s:%s",#variable,variable))
#define DBG_PRINTB(flDbgFlags,variable) DISPDBG((flDbgFlags, "%s:%s",#variable,variable?"TRUE":"FALSE"))

// FNicklis IA-64: Bad truncaton of a pointer to 32 bit!
#define DBG_TESTX(variable) DISPDBG((DBG_LVL_TEST, "%s: %s:0x%x",szDbg_TRACE_GetCurrentFn(),#variable,(ULONG)((ULONG_PTR)(variable))))
#define DBG_TESTD(variable) DISPDBG((DBG_LVL_TEST, "%s: %s:%d",szDbg_TRACE_GetCurrentFn(),#variable,variable))
#define DBG_TESTS(variable) DISPDBG((DBG_LVL_TEST, "%s: %s:%s",szDbg_TRACE_GetCurrentFn(),#variable,variable))
#define DBG_TESTB(variable) DISPDBG((DBG_LVL_TEST, "%s: %s:%s",szDbg_TRACE_GetCurrentFn(),#variable,variable?"TRUE":"FALSE"))

#define FL_DBG_CHECK_PTRONLY      0x00
#define FL_DBG_CHECK_NEXT         0x01
#define FL_DBG_CHECK_PREV         0x02
#define FL_DBG_CHECK_NEXTEMPTY    0x04
#define FL_DBG_CHECK_LIST         0x08
#define FL_DBG_CHECK_MEMORY       0x10

#define FL_DBG_CHECK_LINK         (FL_DBG_CHECK_NEXT |FL_DBG_CHECK_PREV)
#define FL_DBG_CHECK_LINKMEM      (FL_DBG_CHECK_NEXT |FL_DBG_CHECK_PREV|FL_DBG_CHECK_MEMORY)
#define FL_DBG_CHECK_LINKS        (FL_DBG_CHECK_NEXT |FL_DBG_CHECK_PREV|FL_DBG_CHECK_NEXTEMPTY)
#define FL_DBG_CHECK_LINKSMEM     (FL_DBG_CHECK_NEXT |FL_DBG_CHECK_PREV|FL_DBG_CHECK_NEXTEMPTY|FL_DBG_CHECK_MEMORY)
#define FL_DBG_CHECK_LINKLIST     (FL_DBG_CHECK_LINK |FL_DBG_CHECK_LIST)
#define FL_DBG_CHECK_LINKLISTMEM  (FL_DBG_CHECK_LINK |FL_DBG_CHECK_LIST|FL_DBG_CHECK_MEMORY)
#define FL_DBG_CHECK_LINKSLIST    (FL_DBG_CHECK_LINKS|FL_DBG_CHECK_LIST)
#define FL_DBG_CHECK_LINKSLISTMEM (FL_DBG_CHECK_LINKS|FL_DBG_CHECK_LIST|FL_DBG_CHECK_MEMORY)

#define FL_DBG_CHECK_ALL          (0xFF)

BOOL bDbg_CheckHeapNode(const struct _memory_t *pHeap,const struct _memory_t *pNode, FLONG flCheck);
VOID vDbg_CheckHeapIntegrity(FLONG flDbgLvl, char *szText, const struct _memory_t *pHeap);
VOID vDbg_Print_memory_t(FLONG flDbgLvl, char *szText, const struct _memory_t *pHeap);
char *szDbg_GetET_VALUE_IDName(DWORD dwID);


extern LONG Void;
#define DBGVOID Void

extern BOOL bDbg_CheckHeap;

VOID vDbg_Printfl2DCapabilities(
  FLONG flDbgFlags, 
  PCHAR pcString, 
  const PPDEV ppdev);

VOID vDbg_PrintET_VERSION(
  FLONG flDbgFlags, 
  PCHAR pcString,   
  const struct tagET_VERSION *pVersion);

VOID vDbg_PrintET_VERSION_INFO(
  FLONG flDbgFlags, 
  PCHAR pcString,   
  const struct tagET_VERSION_INFO *pVersionInfo);

char *szDbg_GetExceptionCode(DWORD dwID);

void vDbg_PrintBitBltParams(ULONG ulFlags,SURFOBJ  *psoTrg,    
                                          SURFOBJ  *psoSrc,
                                          SURFOBJ  *psoMask,
                                          CLIPOBJ  *pco,
                                          XLATEOBJ *pxlo,
                                          RECTL    *prclTrg,
                                          POINTL   *pptlSrc,
                                          POINTL   *pptlMask,
                                          BRUSHOBJ *pbo,
                                          POINTL   *pptlBrush,
                                          ROP4     rop4);
void vDbg_PrintCopyBitsParams(ULONG ulFlags,  
                              SURFOBJ*  psoTrg,
                              SURFOBJ*  psoSrc,
                              CLIPOBJ*  pco,
                              XLATEOBJ* pxlo,
                              RECTL*    prclTrg,
                              POINTL*   pptlSrc);

#define PBB_DBG(arg)      vDbg_PrintBitBltParams    arg
#define PCB_DBG(arg)      vDbg_PrintCopyBitsParams  arg

char *szDbg_GetET_SET_VALUE_RETdwFlags(DWORD dwFlags);
char *szDbg_GetESC_RETURN(LONG lRet);
VOID vDbg_PrintET_VALUE_RETdwFlags(
  FLONG flDbgLvl, 
  PCHAR pcString, 
  DWORD dwFlags);

VOID vDbg_PrintflCaps(
  FLONG flDbgFlags, 
  PCHAR pcString,   
  const PPDEV ppdev);

VOID vDbg_PrintBRUSHOBJ(FLONG flDbgFlags,PCHAR pcString,const BRUSHOBJ* pbo);
VOID vDbg_PrintExportedFunctions(FLONG flDbgLvl,PCHAR pcString, const DRVENABLEDATA *pded);
#if MULTI_BOARD
VOID vDbg_PrintFunctionTable(FLONG flDbgLvl, PCHAR pcString, const APFN* papfn);
VOID vDbg_PrintBlitDirectionTable( FLONG flDbgLvl, IN const PPDEV ppdev );
#endif
VOID vDbg_PrintPATHOBJ(
  FLONG    flDebugFlags, 
  PCHAR    pcString,   
  PATHOBJ *ppo);

VOID vDbg_PrintET_QUERY(
  IN       FLONG     flDebugFlags, 
  IN const PCHAR     pcString,   
  IN const struct tagET_QUERY *petQuery);
VOID vDbg_PrintET_WINMAN2_DATA_GET(
  IN       FLONG                flDebugFlags, 
  IN const PCHAR                pcString,   
  IN const struct tagET_WINMAN2_DATA_GET *pWINmanDataGet);
VOID vDbg_PrintET_WINMAN2_DATA_SET(
  IN       FLONG                flDebugFlags, 
  IN const PCHAR                pcString,   
  IN const struct tagET_WINMAN2_DATA_SET *pWINmanDataSet);
VOID vDbg_PrintET_WINMAN2_DATA_BRD_R(
  IN       FLONG                  flDebugFlags, 
  IN const PCHAR                  pcString,   
  IN const struct tagET_WINMAN2_DATA_BRD_R *pWINmanDataBrd);
VOID vDbg_PrintET_WINMAN2_DATA_BRD_RW(
  IN       FLONG                   flDebugFlags, 
  IN const PCHAR                   pcString,   
  IN const struct tagET_WINMAN2_DATA_BRD_RW *pWINmanDataBrdRW);
char *szDbg_GetWINMAN_SETName(ULONG fl);
char *szDbg_GetWINMAN_RESULTName(ULONG fl);
VOID vDbg_PrintflWINMAN_MOD(
  FLONG flDbgFlags, 
  PCHAR pcString,   
  DWORD dwDrvModFlags);
VOID vDbg_PrintVIDEO_MODE_INFORMATION(
  FLONG flDebugFlags, 
  PCHAR pcString,   
  struct _VIDEO_MODE_INFORMATION *pVideoModeInformation);
char *szDbg_GetREG_RETURN(LONG lRet);
VOID vDbg_PrintDEVMODEW(
  FLONG     flDebugFlags, 
  PCHAR     pcString,   
  struct _devicemodeW *pDevMode);
char *szDbg_GetDrvINDEX(ULONG ulIndex);

VOID vDbg_PrintET_VALUE_DWORD(
  FLONG           flDebugFlags, 
  PCHAR           pcString,   
  struct tagET_VALUE_DWORD *petDWord);

VOID vDbg_PrintET_VALUE_STRING(
  FLONG            flDebugFlags, 
  PCHAR            pcString,   
  struct tagET_VALUE_STRING *petString);

VOID vDbg_PrintSURFACE_flType(
  IN       FLONG flDbgLvl, 
  IN const PSTR  pjString, 
  IN       FLONG fl);

VOID vDbg_PrintSURFACE(
  IN       FLONG flDbgLvl, 
  IN const PSTR  pjString, 
  IN const struct _SURFACE *pSurface);

#if 0 // ELSA-FNicklis 25.04.00: unused
VOID vDbgFIFO(PPDEV ppdev, ULONG ulDbgLevel);
#define DBG_FIFO vDbgFIFO
#endif // ELSA-FNicklis 25.04.00: unused

// Not supported yet, disabled for debug and free!
#define vDbg_PrintGDIINFO(flDbgLvl, pgdiinfo)
#define vDbg_PrintDD_HALINFO(flDbgLvl,pcString,phalinfo)
#define vDbg_PrintSPS_FLAGS(flDbgLvl,ulSPS_FLAGS)
#define vDbg_PrintSPS_RETURNS(flDbgLvl,ulSPS_FLAGS)


typedef struct _DBG_NAMEINDEX
{
  ULONG ulIndex;
  char *szName;
} DBG_NAMEINDEX;

#define DBG_MAKE_NAMEINDEX(id) {(ULONG)id, #id}
char *szDbg_GetDBG_NAMEINDEX(IN const struct _DBG_NAMEINDEX *pNameIndex, IN LONG lEntries, IN ULONG ulIndex);
#define DBG_GETDBG_NAMEINDEX(array, ulIndex) szDbg_GetDBG_NAMEINDEX((array), sizeof(array)/sizeof((array)[0]), ulIndex)
VOID vDbg_PrintflCapabilities(
  FLONG flDbgFlags, 
  const PCHAR pcString, 
  char *(szFlCaps)(ULONG),  // function pointer to stringizer function (szDbg_GetFL_2DCAPSName)
  FLONG flPrint);

#define szBOOL(expression) ( (expression) ? "TRUE" : "FALSE" )

// If we are not in a debug environment, we want all of the debug
// information to be stripped out.

/* ********************************************************************** */
/* ********************************************************************** */
/* ********************************************************************** */

#else
/* ********************************************************************** */
/* ********************************************************************** */
/* ********************************************************************** */

#define DBGBREAK()
#define EngDebugBreak()
#define DISPDBG(arg)
#define DISPDBGCOND(cond,arg)
#define STATEDBG(level)
#define LOGDBG(arg)
#define RIP(x)
#define RIP_IA64(x)
#define ASSERTDD(condition,text)
#define ASSERT(condition)
#define DBG_CURRENTPOSITION 

#define DBGFLAGSSET(drventry,dbgflags)
#define DBGFLAGSNEW(dbgflags)
#define DBGFLAGSOLD()                 

#define pszDbg_GetFLOH(floh)

#define vDbg_PrintPOINTL(flFlags,pcString,pptl)
#define vDbg_PrintRECTL(flFlags,pcString,prcl)
#define vDbg_PrintRECTLArray(flFlags,pcString,prcl,c)
#define vDbg_PrintSIZEL(flFlags,pcString,psiz)

#define vDbg_PrintCLIPOBJ(flFlags,pcString,pco)
#define vDbg_PrintCLIPOBJiDComplexity(flFlags,iDComplexity)
#define vDbg_PrintCLIPOBJiFComplexity(flFlags,iFComplexity)

#define vDbg_PrintXLATEOBJ(flFlags,pcString,pxlo)
#define vDbg_PrintXLATEOBJiType(flFlags,iType)
#define vDbg_PrintXLATEOBJflXlate(flFlags,flXlate)

#define vDbg_PrintSURFOBJ(flFlags,pcString,pso)
#define vDbg_PrintSURFOBJiType(flFlags,iType)
#define vDbg_PrintSURFOBJiBitmapFormat(flFlags,iBitmapFormat)
#define szDbg_GetEscapeName(ulEsc)
#define szDbg_GetETSubEscName(SubEscNr)

#define DBG_TRACE_IN(flDbgFlags,function)
#define DBG_TRACE_OUT(flDbgFlags,retVal)
#define DBG_TRACE_DUMP(ulDbgLvl)
#define DBG_ERROR(szText)
#define DBG_WARNING(szText)
#define DBG_TEST(szText)
#define DBG_PRINTX(flDbgFlags,variable)
#define DBG_PRINTD(flDbgFlags,variable)
#define DBG_PRINTS(flDbgFlags,variable)
#define DBG_PRINTB(flDbgFlags,variable)

#define DBG_TESTX(variable)
#define DBG_TESTD(variable)
#define DBG_TESTS(variable)
#define DBG_TESTB(variable)

#define bDbg_CheckHeapNode(pHeap,pNode,flCheck)
#define vDbg_CheckHeapIntegrity(flDbgLvl, szText, pHeap)
#define vDbg_Print_memory_t(flDbgLvl, szText, pHeap)
#define szDbg_GetET_VALUE_IDName(dwID)

#define vDbg_Printfl2DCapabilities(flDbgLvl,szText,ppdev)
#define vDbg_PrintET_VERSION(flDbgLvl,szText,pVersion)
#define vDbg_PrintET_VERSION_INFO(flDbgLvl,szText,pVersionInfo)
#define szDbg_GetExceptionCode(dwID)
#define PBB_DBG(arg)
#define PCB_DBG(arg)
#define szDbg_GetET_SET_VALUE_RETdwFlags(dwFlags)
#define szDbg_GetESC_RETURN(lRet)
#define vDbg_PrintET_VALUE_RETdwFlags(flDbgLvl,pcString,dwFlags)
#define vDbg_PrintflCaps(flDbgFlags,pcString,ppdev)
#define vDbg_PrintBRUSHOBJ(flDbgFlags,pcString,pbo)
#define vDbg_PrintExportedFunctions(flDbgLvl,pcString,ded)
#define vDbg_PrintFunctionTable(flDbgLvl, pcString, papfn)
#define vDbg_PrintBlitDirectionTable(flDbgLvl, ppdev)
#define vDbg_PrintPATHOBJ(flDebugFlags,pcString,ppo)

#define vDbg_PrintET_QUERY(flDebugFlags,pcString,petQuery)
#define vDbg_PrintET_WINMAN2_DATA_GET(flDebugFlags,pcString,pWINmanDataGet)
#define vDbg_PrintET_WINMAN2_DATA_SET(flDebugFlags,pcString,pWINmanDataSet)
#define vDbg_PrintET_WINMAN2_DATA_BRD_R(flDebugFlags,pcString,pWINmanDataBrd)
#define vDbg_PrintET_WINMAN2_DATA_BRD_RW(flDebugFlags,pcString,pWINmanDataBrdRW)
#define szDbg_GetWINMAN_SETName(fl)
#define szDbg_GetWINMAN_RESULTName(fl)
#define vDbg_PrintflWINMAN_MOD(flDbgFlags,pcString,dwDrvModFlags)
#define vDbg_PrintVIDEO_MODE_INFORMATION(flDebugFlags,pcString,pVideoModeInformation)
#define szDbg_GetREG_RETURN(lRet)
#define vDbg_PrintDEVMODEW(flDebugFlags,pcString,pDevMode)
#define szDbg_GetDrvINDEX(ulIndex)
#define vDbg_PrintET_VALUE_DWORD(flDebugFlags,pcString,petDWord)
#define vDbg_PrintET_VALUE_STRING(flDebugFlags,pcString,petString)

#define vDbg_PrintSURFACE_flType(flDbgLvl, pjString, fl)
#define vDbg_PrintSURFACE(flDbgLvl, pjString, pSurface)


// Not supported yet, disabled for debug and free!
#define vDbg_PrintGDIINFO(flDbgLvl, pgdiinfo)
#define vDbg_PrintDD_HALINFO(flDbgLvl,pcString,phalinfo)
#define vDbg_PrintSPS_FLAGS(flDbgLvl,ulSPS_FLAGS)
#define vDbg_PrintSPS_RETURNS(flDbgLvl,ulSPS_FLAGS)
#define DBG_FIFO(ppdev, ulDbgLevel) 

#define vDbg_PrintFL_PUNT_Flags(flDbgLvl, pjString, flFlags)
#define bDbg_VerifySURFOBJ(pso)

#endif // DBG

#endif // USE_WKS_ELSA_TOOLS
#endif // _WKSELSATOOLSDEBUG_H
