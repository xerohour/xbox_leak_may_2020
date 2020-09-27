/**     eeapi.hxx - Public API to the Expression Evaluator
 *
 *      This file contains all types and APIs that are defined by
 *      the Expression Evaluator and are publicly accessible by
 *      other components.
 *
 *      Before including this file, you must include cvtypes.h and shapi.h
 */


/***    The master copy of this file resides in the CVINC project.
 *      All Microsoft projects are required to use the master copy without
 *      modification.  Modification of the master version or a copy
 *      without consultation with all parties concerned is extremely
 *      risky.
 *
 *      The projects known to use this version (1.00.00) are:
 *
 *          Codeview (uses version in CVINC project)
 *          Visual C++
 *          C/C++ expression evaluator (uses version in CVINC project)
 *          Cobol expression evaluator
 *          QC/Windows
 *          Pascal 2.0 expression evaluator
 */

#ifndef _VC_VER_INC
#include "..\include\vcver.h"
#endif

#ifndef EE_API
#define EE_API

#ifdef __cplusplus
#pragma warning(disable: 4200) // "non-standard extension: zero-sized array"
#endif

//  **********************************************************************
//  *                                                                    *
//  *   Initialization Structures                                        *
//  *                                                                    *
//  **********************************************************************

typedef struct {
    void *  (PASCAL *pMHlpvAlloc)( UINT );
    void    (PASCAL *pMHFreeLpv)(void *);
    HEXE    (PASCAL *pSHGetNextExe)(HEXE);
    HEXE    (PASCAL *pSHHexeFromHmod)(HMOD);
    HMOD    (PASCAL *pSHGetNextMod)(HEXE, HMOD);
    PCXT    (PASCAL *pSHGetCxtFromHmod)(HMOD, PCXT);
    PCXT    (PASCAL *pSHSetCxt)(LPADDR, PCXT);
    PCXT    (PASCAL *pSHSetCxtMod)(LPADDR, PCXT);
    HSYM    (PASCAL *pSHFindNameInGlobal)(HSYM, PCXT, HVOID, SHFLAG, PFNCMP, PCXT);
    HSYM    (PASCAL *pSHFindNameInContext)(HSYM, PCXT, HVOID, SHFLAG, PFNCMP, PCXT);
    HSYM    (PASCAL *pSHGoToParent)(PCXT, PCXT);
    HSYM    (PASCAL *pSHHsymFromPcxt)(PCXT);
    HSYM    (PASCAL *pSHNextHsym)(HMOD, HSYM);
    PCXF    (PASCAL *pSHGetFuncCxf)(LPADDR, PCXF);
    char *  (PASCAL *pSHGetModName)(HMOD);
    char *  (PASCAL *pSHGetExeName)(HEXE);
    HEXE    (PASCAL *pSHGethExeFromName)(char *);
    UOFF32  (PASCAL *pSHGetNearestHsym)(LPADDR, HMOD, int, PHSYM);
    SHFLAG  (PASCAL *pSHIsInProlog)(PCXT);
    SHFLAG  (PASCAL *pSHIsAddrInCxt)(PCXT, LPADDR);
    UINT    (PASCAL *pSHModelFromAddr)(PADDR,LPW,LPB,UOFFSET *);
    HSYM    (PASCAL *pSHFindSLink32)( PCXT );

    BOOL    (PASCAL *pSLLineFromAddr) ( LPADDR, unsigned short *, SHOFF *, SHOFF * );
    BOOL    (PASCAL *pSLFLineToAddr)  ( HSF, WORD, LPADDR, SHOFF *, WORD * );
    char *  (PASCAL *pSLNameFromHsf)  ( HSF );
    HMOD    (PASCAL *pSLHmodFromHsf)  ( HEXE, HSF );
    HSF     (PASCAL *pSLHsfFromPcxt)  ( PCXT );
    HSF     (PASCAL *pSLHsfFromFile)  ( HMOD, char * );

    UOFF32  (PASCAL *pPHGetNearestHsym)(LPADDR, HEXE, PHSYM);
    HSYM    (PASCAL *pPHFindNameInPublics)(HSYM, HEXE, HVOID, SHFLAG, PFNCMP);
    HTYPE   (PASCAL *pTHGetTypeFromIndex)(HMOD, THIDX);
    HTYPE   (PASCAL *pTHGetNextType)(HMOD, HTYPE);
    HDEP    (PASCAL *pMHMemAllocate)(UINT);
    HDEP    (PASCAL *pMHMemReAlloc)(HDEP, UINT);
    void    (PASCAL *pMHMemFree)(HDEP);
    HVOID   (PASCAL *pMHMemLock)(HDEP);
    void    (PASCAL *pMHMemUnLock)(HDEP);
    SHFLAG  (PASCAL *pMHIsMemLocked)(HDEP);
    HVOID   (PASCAL *pMHOmfLock)(HVOID);
    void    (PASCAL *pMHOmfUnLock)(HVOID);
    SHFLAG  (PASCAL *pDHExecProc)(LPADDR, SHCALL);
    UINT    (PASCAL *pDHGetDebuggeeBytes)(ADDR, UINT, void *);
    UINT    (PASCAL *pDHPutDebuggeeBytes)(ADDR, UINT, void *);
    PSHREG  (PASCAL *pDHGetReg)(PSHREG, PCXT);
    PSHREG  (PASCAL *pDHSetReg)(PSHREG, PCXT);
    HDEP    (PASCAL *pDHSaveReg)(PCXT);
    void    (PASCAL *pDHRestoreReg)(HDEP, PCXT);
    char   *pin386mode;
    char   *pis_assign;
    void    (PASCAL *pquit)(UINT);
    ushort *pArrayDefault;
    SHFLAG  (PASCAL *pSHCompareRE)(char *, char *);
    SHFLAG  (PASCAL *pSHFixupAddr)(LPADDR);
    SHFLAG  (PASCAL *pSHUnFixupAddr)(LPADDR);
    SHFLAG  (PASCAL *pCVfnCmp)(HVOID, HVOID, char *, SHFLAG);
    SHFLAG  (PASCAL *pCVtdCmp)(HVOID, HVOID, char *, SHFLAG);
    SHFLAG  (PASCAL *pCVcsCmp)(HVOID, HVOID, char *, SHFLAG);
    UINT    (PASCAL *pCVAssertOut)(char *, char *, UINT);
    SHFLAG  (PASCAL *pDHSetupExecute)(LPHIND);
    SHFLAG  (PASCAL *pDHCleanUpExecute)(HIND);
    SHFLAG  (PASCAL *pDHStartExecute)(HIND, LPADDR, BOOL, SHCALL);
    HSYM    (PASCAL *pSHFindNameInTypes)( PCXT, LPSSTR, SHFLAG , PFNCMP , PCXT );
    UINT    (PASCAL *pSYProcessor)(DWORD);
    BOOL    (PASCAL *pTHAreTypesEqual)(HMOD, CV_typ_t, CV_typ_t);
	MPT		(PASCAL *pGetTargetProcessor) (HPID);
	BOOL	(PASCAL *pGetUnicodeStrings) ();
} CVF;  // CodeView kernel Functions exported to the Expression Evaluator
typedef CVF * PCVF;

// DO NOT CHANGE THESE DEFINITIONS FROM __cdecl to CDECL
// Windows.h will define CDECL to an empty string if it has not been defined.
// We will run into bugs if windows.h is included before
// any other header files.


typedef struct {
    short   (__cdecl *pintLoadDS)();
    char  * (__cdecl *pultoa)(ulong, char  *, int);
    char  * (__cdecl *pitoa)(int, char  *, int);
    char  * (__cdecl *pltoa)(long, char  *, int);
    int     (__cdecl *peprintf)(const char *, char *, char *, int);
    int     (__cdecl *psprintf)(char  *, const char *, ...);
    FLOAT10 (__cdecl *p_strtold)( const char  *, char  *  *);
} CRF;  // C Runtime functions exported to the Expression Evaluator
typedef CRF * PCRF;

typedef struct CI {
    char    cbCI;
    char    Version;
    CVF *   pStructCVAPI;
    CRF *   pStructCRuntime;
} CI;
typedef CI * PCI;


typedef struct HDR_TYPE {
    ushort  offname;
    ushort  lenname;
    ushort  offtrail;
} HDR_TYPE;
typedef HDR_TYPE *PHDR_TYPE;

//  **********************************************************************
//  *                                                                    *
//  *   the expr evaluator stuff                                         *
//  *                                                                    *
//  **********************************************************************

typedef HDEP            HSYML;      //* An hsym list
typedef HSYML *         PHSYML;     //* A pointer to a hsym list
typedef uint            EERADIX;
typedef EERADIX *       PEERADIX;
typedef uchar *         PEEFORMAT;
typedef ushort          EESTATUS;
typedef HDEP            EEHSTR;
typedef EEHSTR *        PEEHSTR;
typedef HDEP            HTM;
typedef HTM *           PHTM;
typedef HDEP            HTI;
typedef HTI *           PHTI;
typedef HDEP            HBCIA;      // Base class index array.
typedef HBCIA *         PHBCIA;

// Error return values
#define EENOERROR       0
#define EENOMEMORY      1
#define EEGENERAL       2
#define EEBADADDR       3
#define EECATASTROPHIC  0XFF

typedef enum {
    EEHORIZONTAL,
    EEVERTICAL,
    EEBPADDRESS
} EEDSP;                    // Display format specifier

typedef enum {
    ETIPRIMITIVE,
    ETIARRAY,
    ETIPOINTER,
    ETICLASS,
    ETIFUNCTION
} ETI;
typedef ETI *PETI;

typedef enum {
    EENOTEXP,
    EEAGGREGATE,
    EETYPE,
    EEPOINTER,
    EETYPENOTEXP,
    EETYPEPTR
} EEPDTYP;
typedef EEPDTYP *PEEPDTYP;

typedef struct TML {
    unsigned    cTMListMax;
    unsigned    cTMListAct;
    unsigned    iTMError;
    HDEP        hTMList;
} TML;
typedef TML *PTML;

typedef struct RTMI {
    ushort  fSegType    :1;
    ushort  fAddr       :1;
    ushort  fValue      :1;
    ushort  fSzBits     :1;
    ushort  fSzBytes    :1;
    ushort  fLvalue     :1;
    ushort  fSynthChild :1;
    ushort  fLabel      :1;
    CV_typ_t Type;
} RTMI;
typedef RTMI *    PRI;

typedef struct TMI {
    RTMI        fResponse;
    struct  {
        ushort  SegType    :4;
        ushort  fLvalue    :1;
        ushort  fAddrInReg :1;
        ushort  fBPRel     :1;
        ushort  fFunction  :1;
        ushort  fLData     :1;      // True if expression references local data
        ushort  fGData     :1;      // True if expression references global data
        ushort  fSynthChild:1;
        ushort  fLabel:1;
    };
    union   {
        ADDR    AI;
        ushort  hReg;               // This is really a CV_HREG_e
    };
    ulong       cbValue;
    char        Value[0];
} TMI;
typedef TMI *   PTI;

typedef struct {
    HSYM    hSym;
    CXT     CXT;
} HCS;

typedef struct {
    CXT     CXT;
    ushort  cHCS;
    HCS     rgHCS[0];
} CXTL;

typedef HDEP        HCXTL;
typedef HCXTL * PHCXTL;
typedef CXTL  * PCXTL;

//  Structures for Get/Free HSYMList

//  Search request / response flags for Get/Free HSYMList

#define  HSYMR_lexical  0x0001  // lexical out to function scope
#define  HSYMR_function 0x0002  // function scope
#define  HSYMR_class    0x0004  // class scope
#define  HSYMR_module   0x0008  // module scope
#define  HSYMR_global   0x0010  // global symbol table scope
#define  HSYMR_exe      0x0020  // all other module scope
#define  HSYMR_public   0x0040  // public symbols
#define  HSYMR_nocase   0x8000  // case insensitive
#define  HSYMR_allscopes   \
               (HSYMR_lexical    |\
                HSYMR_function   |\
                HSYMR_class      |\
                HSYMR_module     |\
                HSYMR_global     |\
                HSYMR_exe        |\
                HSYMR_public     |\
                HSYMR_nocase)


//  structure describing HSYM list for a context

typedef struct HSL_LIST {
    ushort      request;        // context that this block statisfies
    struct  {
        ushort  isused      :1; // block contains data if true
        ushort  hascxt      :1; // context packet has been stored
        ushort  complete    :1; // block is complete if true
        ushort  isclass     :1; // context is class if true
    } status;
    HSYM        hThis;          // handle of this pointer if class scope
    ushort      symbolcnt;      // number of symbol handles in this block
    CXT         Cxt;            // context for this block of symbols
    HSYM        hSym[];         // list of symbol handles
} HSL_LIST;
typedef HSL_LIST *PHSL_LIST;


typedef struct HSL_HEAD {
    ushort      size;           // number of bytes in buffer
    ushort      remaining;      // remaining space in buffer
    PHSL_LIST   pHSLList;       // pointer to current context list (EE internal)
    struct  {
        ushort  endsearch   :1; // end of search reached if true
        ushort  fatal       :1; // fatal error if true
    } status;
    ushort      blockcnt;       // number of CXT blocks in buffer
    ushort      symbolcnt;      // number of symbol handles in buffer
    HDEP        restart;        // handle of search restart information
} HSL_HEAD;
typedef HSL_HEAD *PHSL_HEAD;

typedef struct HINDEX_ARRAY {
    ushort      count;              // number of indices in this buffer.
    long        rgIndex[0];
}   HINDEX_ARRAY;
typedef HINDEX_ARRAY    *PHINDEX_ARRAY;

typedef struct {
    void     (PASCAL *pEEFreeStr)(EEHSTR);
    EESTATUS (PASCAL *pEEGetError)(PHTM, EESTATUS, PEEHSTR);
    EESTATUS (PASCAL *pEEParse)(const char *, EERADIX, SHFLAG, PHTM, ushort *);
    EESTATUS (PASCAL *pEEBindTM)(PHTM, PCXT, SHFLAG, SHFLAG);
    EESTATUS (PASCAL *pEEvaluateTM)(PHTM, PFRAME, EEDSP);
    EESTATUS (PASCAL *pEEGetExprFromTM)(PHTM, PEERADIX, PEEHSTR, ushort *);
    EESTATUS (PASCAL *pEEGetValueFromTM)(PHTM, EERADIX, PEEFORMAT, PEEHSTR);
    EESTATUS (PASCAL *pEEGetNameFromTM)(PHTM, PEEHSTR);
    EESTATUS (PASCAL *pEEGetTypeFromTM)(PHTM, EEHSTR, PEEHSTR, ulong);
    EESTATUS (PASCAL *pEEFormatCXTFromPCXT)(PCXT, PEEHSTR);
    void     (PASCAL *pEEFreeTM)(PHTM);
    EESTATUS (PASCAL *pEEParseBP)(char *, EERADIX, SHFLAG, PCXF, PTML, ulong, ushort *, SHFLAG);
    void     (PASCAL *pEEFreeTML)(PTML);
    EESTATUS (PASCAL *pEEInfoFromTM)(PHTM, PRI, PHTI);
    void     (PASCAL *pEEFreeTI)(PHTI);
    EESTATUS (PASCAL *pEEGetCXTLFromTM)(PHTM, PHCXTL);
    void     (PASCAL *pEEFreeCXTL)(PHCXTL);
    EESTATUS (PASCAL *pEEAssignTMToTM)(PHTM, PHTM);
    EEPDTYP  (PASCAL *pEEIsExpandable)(PHTM);
    SHFLAG   (PASCAL *pEEAreTypesEqual)(PHTM, PHTM);
    EESTATUS (PASCAL *pEEcChildrenTM)(PHTM, long *, PSHFLAG);
    EESTATUS (PASCAL *pEEGetChildTM)(PHTM, long, PHTM, ushort *, EERADIX, SHFLAG);
    EESTATUS (PASCAL *pEEDereferenceTM)(PHTM, PHTM, ushort *, SHFLAG);
    EESTATUS (PASCAL *pEEcParamTM)(PHTM, ushort *, PSHFLAG);
    EESTATUS (PASCAL *pEEGetParmTM)(PHTM, ushort, PHTM, ushort *, SHFLAG);
    EESTATUS (PASCAL *pEEGetTMFromHSYM)(HSYM, PCXT, PHTM, ushort *, SHFLAG, SHFLAG);
    void     (PASCAL *pEEFormatAddress)(PADDR, char *);
    EESTATUS (PASCAL *pEEGetHSYMList)(PHSYML, PCXT, ushort, uchar *, SHFLAG);
    void     (PASCAL *pEEFreeHSYMList)(PHSYML);
    EESTATUS (PASCAL *pEEGetExtendedTypeInfo)(PHTM, PETI);
    EESTATUS (PASCAL *pEEGetAccessFromTM)(PHTM, PEEHSTR, ulong);
    BOOL     (PASCAL *pEEEnableAutoClassCast)(BOOL);
    void     (PASCAL *pEEInvalidateCache)(void);
    EESTATUS (PASCAL *pEEcSynthChildTM)(PHTM, long *);
    EESTATUS (PASCAL *pEEGetBCIA)(PHTM, PHBCIA);
    void     (PASCAL *pEEFreeBCIA)(PHBCIA);
    SHFLAG   (PASCAL *pfnCmp)(HVOID, HVOID, char *, SHFLAG);
    SHFLAG   (PASCAL *ptdCmp)(HVOID, HVOID, char *, SHFLAG);
    SHFLAG   (PASCAL *pcsCmp)(HVOID, HVOID, char *, SHFLAG);
	MPT		 (PASCAL *pEESetTarget)(MPT);
	void (PASCAL *pEEUnload)();
} EXF;
typedef EXF * PEXF;

typedef struct EI {
    char    cbEI;
    char    Version;
    PEXF    pStructExprAPI;
    char    Language;
    char   *IdCharacters;
    char   *EETitle;
    char   *EESuffixes;
    char   *Assign;             // length prefixed assignment operator
} EI;
typedef EI * PEI;

// FNEEINIT is the prototype for the EEInitializeExpr function
typedef VOID EXPCALL FNEEINIT(CI *, EI *);
typedef FNEEINIT *  PFNEEINIT;
typedef FNEEINIT *  LPFNEEINIT;

// This is the only EE function that's actually exported from the DLL
FNEEINIT EEInitializeExpr;

#endif // EE_API
