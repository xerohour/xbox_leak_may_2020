/*
 * MODULE:  ParseDef.h
 * AUTHOR:  JohnMil
 * DATE:  1-14-92
 *
 * Copyright (c) 1992-6 Microsoft Corporation
 *
 * This module contains the defines used in the parser and Engine for BadMan
 */

#include <badman.h>
//#include <ntlog.h>


#define CALL_DELETE 1
#define CALL_MODIFY 2

/*
 * Command Line options are set as flags
 */

#define CLOPT_DOCUMENT          0x0001  // Generate a document of test coverage
#define CLOPT_TEST              0x0002  // Test the API (def)
#define CLOPT_GOOD              0x0004  // Go through Good Parm variations
#define CLOPT_BAD               0x0008  // Go through Bad Parm variations (def)
#define CLOPT_PARSEONLY         0x0010  // Parse the file, don't test or doc.
#define CLOPT_PERMUTE           0x0020  // Do permutations rather than linear
#define CLOPT_API               0x0040  // API to test specified
#define CLOPT_VARIATION         0x0080  // Variation to test specified
#define CLOPT_VALIDATE          0x0100  // Perform validation on return value
#define CLOPT_PERFORMANCE       0x0200  // Generate Performance data
#define CLOPT_NTLOG             0x0400  // Generate NTLog output
#define CLOPT_NOEXCEPT          0x0800  // Don't log exceptions
#define CLOPT_DBGBREAK          0x1000  // Break before API Calls
#define CLOPT_PRINT_ALL_CALLS   0x2000  // print successful calls too
#define CLOPT_DEFAULT           (CLOPT_TEST | CLOPT_BAD)


#define MAX_TIMING_ITERATIONS   2000
#define MIN_TIMING_ITERATIONS   3

/*
 * These are states for our state machine (script parser)
 */

typedef enum    _parser_state   {
    STATE_ERROR = 0,
    STATE_API_TYPE,
    STATE_API_NAME,
    STATE_API_NEXT_PARAM,
    STATE_API_PARAM_TYPE,
    STATE_API_PARAM_NAME,
    STATE_API_PARAM_DONE,
    STATE_API_RETURN_ASSIGN,
    STATE_API_RETURN_DONE,
    STATE_PARAM_IDENTIFIED,
    STATE_PARAM_GOOD_LIST,
    STATE_PARAM_BAD_LIST,
    STATE_PARAM_OMIT_LIST,
    STATE_PARAM_DEPEND,
    STATE_PARAM_ENDDEPEND,
    STATE_LOOP_THROUGH_API,
    STATE_BEGIN,
    STATE_API_NEXT_PARAM_WITH_COMMA,
    STATE_PARAM_BAD_READ,
    STATE_PARAM_LASTERROR,
    STATE_MODULE_ID,
    STATE_MODULE_NAME
} PARSER_STATE;


/*
 * This is the number of DLL's we dynamically link to to to get API's. Actual
 * names are declared in the Modules[] array in badman\src\engine\BadMan.c
 */

typedef enum    _BADMAN_MODULE_IDS  {
    BADMAN_MODULE_NUM_APICLN = 0,
    BADMAN_MODULE_NUM_SETCLN,
    BADMAN_MODULE_NUM_VALIDATE,
    BADMAN_MODULE_COUNT
} BADMAN_MODULE_IDS;


#define MAX_OTHER_MODULES       10


/*
 * Function Prototypes
 */

void
DestroyParmDep(
    ParmDepType             *PD
);


S_PARMS
*DestroyParm(
    S_PARMS                 *P
);


void
DumpDefines(
    DefineType              Hash[HASH_SIZE]
);


void
DumpFullParm(
    S_PARMS                 *P
);


void
DumpMacros(
    MacroType               MacroHash[]
);


void
DumpParms(
    S_PARMS                 *P
);


void
DumpStrings(
    StringType              Hash[],
    int                     HashSize
);


BOOL
ExistingParmName(
    char                    *Name,
    S_PARMS                 *PList
);


DefineType
*FindDefine(
    char                    *token,
    DefineType              *DList
);


MacroType
*FindMacro(
    char                    *token,
    MacroType               *MacroHash
);


StringType
*FindString(
    char                    *token,
    StringType              SList[],
    int                     HashSize
);


int
GenParm(
    S_PARMS                 ParmList[MAXNOPARMS],
    int                     ParmNo,
    int                     NumParms,
    FARFNSETUP              pfnSetup[MAXNOPARMS],
    FARFNCLEANUP            pfnCleanup[MAXNOPARMS],
    FARDPROC                pfnAPI,
    FARFNVALIDATE           pfnValidate,
    FARFNAPICLN             pfnAPICln,
    int                     GoodBad,
    int                     *Variation,
    CLType                  *CLOptions,
    DefineType              VarInfo[MAXNOPARMS],
    APIType                 *API,
    int                     MadeParm,
    int                     ParmGoodBad[MAXNOPARMS],
    HANDLE                  hLog,
    HANDLE                  hConOut
);


int
HashVal(
    char                    *ToHash,
    int                     HashSize
);


BOOL
ModifyParms(
    int                     CallType,
    S_PARMS                 *ParmList,
    ParmDepType             *PD
);


BadValType
*NewBadVal(
    DefineType              *Def,
    DefineType              *LDef,
    int                     LastError,
    BadValType              *next
);


char
*NewCharString(
    char                    *Text
);


DefineType
*NewDefine(
    char                    *Name,
    char                    *Comment,
    int                     Val,
    DefineType              *Next
);


GoodValType
*NewGoodVal(
    DefineType              *Def,
    GoodValType             *next
);


MacroType
*NewMacro(
    char                    *Name,
    MacroValType            *M,
    MacroType               *next
);


MacroValType
*NewMacroVal(
    DefineType              *Def,
    DefineType              *LDef,
    int                     LastError,
    MacroValType            *next
);


S_PARMS                 *
NewParm(
    StringType              *Type,
    char                    *Name,
    BOOL                    bIsEndCase,
    BOOL                    NoPush,
    GoodValType             *Good,
    BadValType              *Bad,
    S_PARMS                 *Depend,
    S_PARMS                 *next
);


ParmDepType
*NewParmDep(
    S_PARMS                 *P,
    ParmDepType             *next
);


SeenType
*NewSeen(
    SeenType                *next
);


StringType
*NewString(
    char                    *token,
    StringType              *next
);



int
Parse(
    DefineType              Hash[HASH_SIZE],
    MacroType               MacroHash[MACRO_HASH_SIZE],
    StringType              DataTypes[DATA_HASH_SIZE],
    CLType                  *CLOptions,
    ModType                 *BadmanModules,
    ModType                 *OtherModules,
    HANDLE                  hLog
);


void
PrintAPICall(
    HANDLE                  hLog,
    APIType                 *API,
    S_PARMS                 *ParmList,
    DefineType              *VarInfo,
    DWORD                   *dwParms,
    DWORD                   dwRetVal,
    int                     Variation,
    int                     NumParms,
    int                     *GoodBadParm,
    BOOL                    GLPass,
    BOOL                    VarPass,
    BOOL                    VarExcept,
    DWORD                   LastError
);


int
ProcessAPI(
    APIType                 *API,
    DefineType              *Hash,
    StringType              *DataTypes,
    CLType                  *CLOptions,
    ModType                 *BadmanModules,
    ModType                 *OtherModules,
    HANDLE                  hLog,
    HANDLE                  hConOut
);


BOOL
ProcessCLILogFlags(
    CLType                  *CLOptions,
    char                    *Flags
);


BOOL
ReadCommandLine(
    int                     argc,
    char                    **argv,
    CLType                  *CLOptions
);


void
ReadDefines(
    char                    *FileName,
    DefineType              Hash[HASH_SIZE],
    MacroType               MacroHash[MACRO_HASH_SIZE]
);


void
ReadStringHash(
    char                    *FileName,
    StringType              Hash[],
    int                     HashSize
);


int
TranslateDefine(
    char                    *DefineName,
    DefineType              Hash[HASH_SIZE]
);


BOOL
ValidParmName(
    char                    *Name
);


HANDLE
LMemHandleCreate(
    int                     CaseNo
);


HANDLE
GMemHandleCreate(
    int                     CaseNo
);


LPSTR
LMemPointerCreate(
    int                     CaseNo,
    LPVOID                  *SCInfo
);


LPSTR
GMemPointerCreate(
    int                     CaseNo,
    LPVOID                  *SCInfo
);


VOID
FreeDefineType(
    DefineType * ptr
    );


VOID
FreeStringType(
    StringType * ptr
    );


VOID
FreeValType(
    MacroValType * ptr
    );


VOID
FreeMacroType(
    MacroType * ptr
    );
