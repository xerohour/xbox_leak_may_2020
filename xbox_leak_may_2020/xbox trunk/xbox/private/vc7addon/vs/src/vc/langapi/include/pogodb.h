// pogodb.h
//
// Pogo Data Base reading/writing interfaces

#if __cplusplus
extern "C" {
#endif

#include <pdb.h>


#pragma warning(push)

#pragma warning(disable: 4200)     // allow 0 sized array in struct/union
#pragma warning(disable: 4201)     // Allow nameless struct/union


#include <stddef.h>
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned __int64 QWORD;


// REVIEW: May want to typedef DWORD PGDSYMID;

#if !defined(POGODB_DLL_BUILD)

#define POGODLL __declspec(dllimport)

#else

#define POGODLL __declspec(dllexport)

#endif

#define POGODLLAPI(type) POGODLL type __stdcall


#if defined(_M_ALPHA)

#define POGODB_MACHINE IMAGE_FILE_MACHINE_ALPHA

#elif defined(_M_IX86)

#define POGODB_MACHINE IMAGE_FILE_MACHINE_I386

#elif defined(_M_IA64)

#define POGODB_MACHINE IMAGE_FILE_MACHINE_IA64

#else

#error Unsupported host platform

#endif


// Database sig/ver defines.

#define POGODB_HEADER_SIGNATURE 0x4F474F50
#define POGODB_HEADER_VERSION   0x00000020


// Pogo database operating modes.  These control access to various APIs and
// such.

typedef enum {
    POGODB_TOOL_READ,     // for Pogo profile data tools
    POGODB_TOOL_WRITE,
    POGODB_COMPILER_READ, // VC++ compiler
    POGODB_COMPILER_WRITE,
    POGODB_OVERRIDE_READ, // for non-Pogo override info tools
    POGODB_OVERRIDE_WRITE,
    POGODB_LINKER_READ,   // VC++ linker
    POGODB_LINKER_WRITE
} POGODB_MODE;


// Enumeration of profile streams (must be in sync with names in pogodb.cpp)
// Don't forget to add per-module streams to PogoDbOpenStream and
// PogoDbSetModule.

typedef enum {
    PGDS_HEADER = 0,
    PGDS_HISTORY,
    PGDS_OBJECT_INFO,
    PGDS_LINK_INFO,
    PGDS_SYMBOL_TABLE,
    PGDS_PROBE_ID,
    PGDS_ENTRY,
    PGDS_MODULE,
    PGDS_FUNCTIONS,
    PGDS_PROBE_INFO,
    PGDS_PROBE_DATA,
    PGDS_VALUE_DATA,
    PGDS_MDS_DATA,
    PGDS_FLOW_GRAPH,
    PGDS_CALL_GRAPH,
    PGDS_BRANCH_INFO,
    PGDS_OVERRIDE_INFO,
    PGDS_FUNC_HASH,
    PGDS_TOB_DATA,
    PGDS_VCALL_TABLE,
    PGDS_MAX
} POGODB_STREAM;
extern POGODLL const char * const PogoDbStreamNames[];

// Enumeration of error codes.

typedef enum {
    PGDE_OK = 0,
    PGDE_ERROR, // generic error
    PGDE_NOT_FOUND, // file not found
    PGDE_SHARING_VIOLATION, // sharing violation (opened for write elsewhere)
    PGDE_PDB_ERROR, // PDB error not explicitly handled
    PGDE_SIGNATURE_MISMATCH,
    PGDE_VERSION_MISMATCH,
    PGDE_READ_ONLY,
    PGDE_LTCG_MISMATCH,   // PGD is LTCG and compile isn't or vice-versa
    PGDE_OUT_OF_MEMORY,
    PGDE_BAD_PARAMETER,
    PGDE_NO_DATA, // no data of the requested type
    PGDE_PGD_NOT_OPEN,
    PGDE_MODULE_NOT_OPEN,
    PGDE_FUNCTION_NOT_OPEN,
    PGDE_MISSING_STREAM,
    PGDE_INVALID_STREAM = PGDE_MISSING_STREAM + PGDS_MAX,
    PGDE_REPLACE_ERROR = PGDE_INVALID_STREAM + PGDS_MAX,
    PGDE_WRITE_ERROR = PGDE_REPLACE_ERROR + PGDS_MAX,
    PGDE_APPEND_ERROR = PGDE_WRITE_ERROR + PGDS_MAX,
    PGDE_STREAM_NOT_OPEN = PGDE_APPEND_ERROR + PGDS_MAX,
    PGDE_MAX = PGDE_STREAM_NOT_OPEN + PGDS_MAX
} POGODB_ERROR;


typedef enum {
    PGDH_CLEAR=0, PGDH_ADD, PGDH_SUB, PGDH_COMP, PGDH_OPT,
    PGDH_MAX
} POGODB_HISTORY_TYPE;
extern POGODLL const char * const PogoDbHistoryType[];


struct tagPGDHEADER                 // POGO Database Header
{
    DWORD Signature;              // Magic number
    DWORD Version;                // Database version

    // !!! Do not add any members above this comment !!!
    
    DWORD Id;                     // "Unique" identified for this database
    DWORD Machine;                // Machine that generated the PGD
    DWORD HeaderFlags;            // flags
    LONG  numEntryProbe;          // Count of function entry probe handles
    LONG  numSimpleProbe;         // Count of simple probe handles
    LONG  numValueProbe;          // Count of special value probe handles
    DWORD fBigEndian;             // true if created on big-endian machine
    LONG  OffsetNextSymItem;      // Offset to next Symbol table record
    LONG  OffsetEntryProbeList;   // Offset to list of avail entry probe ids
    LONG  OffsetSimpleProbeList;  // Offset to list of avail simple probe ids
    LONG  OffsetValueProbeList;   // Offset to list of avail value probe ids
    LONG  OffsetFreeProbeList;    // Offset to list of unused probe blocks
    LONG  numModules;             // Count of number modules (files) compiled
    LONG  numFunctions;           // Count of functions in all modules
    QWORD ModTime;                // Time of last update
    QWORD HotRegionThreshold;     // Dyn instr count for a region to be hot
    DWORD dwTOB;                  // Last valid TOB DWORD
};
typedef struct tagPGDHEADER PGDHEADER, *PPGDHEADER;

// Pogo DB Header flags
#define POGODB_HF_LTCG               0x80000000
#define POGODB_HF_OVERRIDE_ONLY      0x40000000

struct tagPGDHISTORYRECORD {
    QWORD  ModTime;                // Time of last update
    QWORD  MergeFileTime;          // Time stamp from idf file
    POGODB_HISTORY_TYPE  HistoryType;
    char PathName[0];
};
typedef struct tagPGDHISTORYRECORD PGDHISTORYRECORD, *PPGDHISTORYRECORD;


struct tagPGDOBJECTRECORD {
    union {
        const char *szObjName; // for clients
        NI nameIndex;    // persisted
    };
    QWORD ObjectTime;
    QWORD FileTime;
};
typedef struct tagPGDOBJECTRECORD PGDOBJECTRECORD, *PPGDOBJECTRECORD;


struct tagPGDSYMFLAGS {
    DWORD ModuleId:20;      // module where symbol is defined
    DWORD unused:9;         //
    DWORD fCalleeSaved:1;   // 1 => force all registers to be callee saved
    DWORD SymbolType:2;     // see below for definition
};
typedef struct tagPGDSYMFLAGS PGDSYMFLAGS;

// Pogo Symtab Flags (PSF)
#define PSF_MODULEID         0x000FFFFF
#define PSF_unused           0x1FF00000
#define PSF_CALLEESAVED      0x20000000
#define PSF_SYMBOLTYPE       0xC0000000
#define PSF_GLOBALINLINE     0x40000000
#define PSF_LOCAL            0x80000000
#define PSF_MODULE           0xC0000000

// WARNING: there is code that assumes OffsetNextHash is the first field
struct tagPGDSYMTABRECORD      {
    LONG OffsetNextHash;  // offset to next item with same hash value
    LONG OffsetNextSym;   // offset to next item with same symbol value
    NI   NameIndex;       // symbol name index
    union {
        DWORD SymbolFlagsWord;
        PGDSYMFLAGS SymbolFlags;
    };
};
typedef struct tagPGDSYMTABRECORD PGDSYMTABRECORD, *PPGDSYMTABRECORD;


// forward pointer definitions needed for MODULERECORD
typedef struct tagPGDVALUECOUNTRECORD *PPGDVALUECOUNT;
typedef struct tagPGDVALUEDATARECORD  *PPGDVALUEPROBEDATA;
typedef struct tagPGDCALLGRAPHNODE    *PPGDCALLGRAPHNODE;


struct tagPGDMODULERECORD {
    DWORD SymbolId;          // Offset to Symtab record
    WORD  numFunctions;      // Count of functions in module
    WORD  numBaseDataSets;   // Count of data sets in module(before pogoopt)
    LONG  numSimpleProbe;    // Count of simple probe handles
    LONG  numValueProbe;     // Count of special value probe handles
    LONG  OffsetCallGraph;   // Offset to call graph in graph stream
    DWORD Checksum;          // Call graph checksum for source edit checking
};
typedef struct tagPGDMODULERECORD PGDMODULERECORD, *PPGDMODULERECORD;


struct tagPGDOPTFLAGS {
    WORD fOptOt:1;               // 1 => -Ot, 0 => -Os
    WORD fInline:1;              // 1 => inline into caller (should be just 1)
    WORD unused:14;              // available for expansion
};
typedef struct tagPGDOPTFLAGS PGDOPTFLAGS;


// WARNING: there is code that assumes OffsetNextHash is the first field
struct tagPGDFUNCHASHRECORD   {
    LONG OffsetNextHash;   // offset to next item with same hash value
    LONG SymbolId;         // function symbol ID
    LONG OffsetFuncData;   // offset to function data in func data stream
};
typedef struct tagPGDFUNCHASHRECORD PGDFUNCHASHRECORD, *PPGDFUNCHASHRECORD;


struct tagPGDFUNCRECORD {
    DWORD LineNumberBase;         // Starting line number for this function
    DWORD EntryProbeId;           // Entry probe id for this func

    WORD  numSimpleProbe;         // Number of logical simple probe handles
    WORD  MaxSimpleProbe;         // Max number of simple probe handles

    WORD  numValueProbe;          // Count of value probe handles
    WORD  MaxValueProbe;          // Max number of value probe handles

    WORD  numBranchRecords;       //
    WORD  numDataSetRecords;      // 0 if !MDS, also 0 if none seen

    struct {
        WORD fMDS    : 1;       // MDS function
        WORD _unused : 15;
    };
    union {
        WORD optFlagsWord;
        PGDOPTFLAGS optFlags;
    };

    WORD  LinkOrderIndex;         // Opt: link order

    LONG  OffsetOverride;         // Offset into Override stream
    LONG  OffsetBranch;           // Offset into Branch stream
    LONG  OffsetProbeInfo;        // Offset into ProbeInfo stream
    LONG  OffsetProbeData;        // Offset into ProbeData stream
    LONG  OffsetTOBData;          // Offset into TOB stream

    LONG  DataSetIndex;           // Index into DataSet stream (0 if !MDS)

    LONG  SymbolId;               // Offset to Symtab record

    DWORD Checksum;               // Checksum for source edit determination

    WORD  numPhysSimpleProbe;     // Number of physical simple probes
                                  // (doesn't count duplicates)
};
typedef struct tagPGDFUNCRECORD PGDFUNCRECORD, *PPGDFUNCRECORD;

struct tagPGDFUNCOVERRIDE {
    DWORD fOptOt:1;               // 1 => -Ot, 0 => -Os
    DWORD fUnused:31;
    DWORD mask; // valid fields
};
typedef struct tagPGDFUNCOVERRIDE PGDFUNCOVERRIDE, *PPGDFUNCOVERRIDE;

#define PFO_OPTOT 0x00000001


// Pogo Probe Flags (PPF)

// Common to all types

#define PPF_VALUE                0x80000000

// Simple flags

#define PPF_DUPLICATE            0x40000000
#define PPF_CALL                 0x20000000

// Value flags

#define PPF_TEMPLATED            0x40000000


// !!!WARNING!!!
// there is code that relies on sizeof(SIMPLEPROBE) == sizeof(VALUEPROBE)
// UNDONE: we need a compile time assert to that fact (markro)

struct tagPGDSIMPLEPROBERECORD {
    WORD LineNumber;     // line number of probe relative to current function
    WORD ProbeId;        // probe index relative to current function
    union {
        DWORD  ProbeField;
        // NOTE: Experiments with Excel showed a probe count of only 19-20
        //       total bits.  Hence, it should be extremely safe to use the
        //       highest 3 bits for flags.
        struct {
            DWORD ProbeOffset:29; // probe index relative to entire database
            DWORD fCall:1;        // 1 => probe is for a CALL
            DWORD fDup:1;         // 1 => probe has a duplicate ProbeOffset
            DWORD fValueProbe:1;  // 0 => simple probe
        };
    };
};
typedef struct tagPGDSIMPLEPROBERECORD PGDSIMPLEPROBE, *PPGDSIMPLEPROBE;

// Value probe type enumeration.

typedef enum {
    POGODB_VPT_NONE = 0, POGODB_VPT_LINEAR_SWITCH, POGODB_VPT_JUMP_SWITCH,
    POGODB_VPT_BINARY_SWITCH, POGODB_VPT_DEAD,
    POGODB_VPT_DIV, POGODB_VPT_REM, POGODB_VPT_DIVREM, POGODB_VPT_MUL,
    POGODB_VPT_CALL,
    POGODB_VPT_MAX
} POGODB_VPT_ENUM;
extern POGODLL const char * const PogoDbValueProbeTypeNames[];

struct tagPGDVALUEPROBERECORD {
    WORD LineNumber;     // line number of probe relative to current function
    WORD ProbeId;        // probe index relative to current function
    union {
        DWORD  ProbeField;
        // NOTE: Experiments with Excel showed a probe count of only 19-20
        //       total bits.  Hence, it should be extremely safe to use the
        //       highest 6 bits for other fields.
        // NOTE2: Currently probe offsets come from single pool so the
        // valid range is actually min(simple,value).  If we were to allocate
        // value probes first, we could get away with differing offset field
        // sizes, but that doesn't seem important given the above NOTE was
        // based on all plain probes, not the more common MDS scenario.
        struct {
            DWORD ProbeOffset:26; // probe index relative to entire database
            DWORD Type:4;         // switch/value probe type
            DWORD fTemplated:1;   // 1 => templated value probe
            DWORD fValueProbe:1;  // 1 => value probe
        };
    };
};
typedef struct tagPGDVALUEPROBERECORD PGDVALUEPROBE, *PPGDVALUEPROBE;

// !!!WARNING!!!
// there is code that relies on sizeof(SIMPLECOUNT) == sizeof(VALUECOUNT)
// UNDONE: we need a compile time assert to that fact (markro)

struct tagPGDSIMPLECOUNTRECORD {
    QWORD  ProbeCount;      // execution count collected when program runs
};
typedef struct tagPGDSIMPLECOUNTRECORD PGDSIMPLECOUNT, *PPGDSIMPLECOUNT;

struct tagPGDVALUECOUNTRECORD {
    union {
        LONG  NumDataValues;  // count of selection records
        struct {
            DWORD NDVDummy:31;  //
            DWORD fTmpPointer:1;// whether ValueDataIndex is offset or pointer
        };
    };
    LONG ValueDataIndex;  // index into module value data stream
};
typedef struct tagPGDVALUECOUNTRECORD PGDVALUECOUNT, *PPGDVALUECOUNT;

struct tagPGDVALUEDATARECORD  {
    DWORD SelectionValue;  // case value for this item
    DWORD Unused;          // (because of QWORD alignment requirement)
// ISSUE: if space becomes a problem, we could make this a DWORD? (markro)
    QWORD ProbeCount;      // execution count collected when program runs
};
typedef struct tagPGDVALUEDATARECORD PGDVALUEPROBEDATA, *PPGDVALUEPROBEDATA;

struct tagPGDENTRYRECORD {
    WORD  numSimpleProbe;  // Count of simple probe handles
    WORD  numValueProbe;   // Count of special value probe handles
    DWORD pEntryData;      // This field is used by POGO runtime
};
typedef struct tagPGDENTRYRECORD PGDENTRYRECORD, *PPGDENTRYRECORD;

struct tagPGDPROBERANGERECORD  {
    LONG LowValue;        // low end of free probe values
    LONG HighValue;       // high end of free probe values
    LONG OffsetNext;      // offset to next probe range record
                            // -1 => end of list
};
typedef struct tagPGDPROBERANGERECORD PGDPROBERANGERECORD, *PPGDPROBERANGERECORD;

struct tagPGDDATASETRECORD {
    union {
        DWORD DataSetField;   //
        struct {
            DWORD pathVal:30;     // indentifies source of data (0 if !MDS)
            DWORD fInline:1;      // should inline at call site?
            DWORD fSynthesized:1; // created by pogoopt?
        };
    };
    LONG   OffsetProbeData; // offset into probe data stream
    LONG   OffsetTOBData;   // offset into TOB stream
};
typedef struct tagPGDDATASETRECORD PGDDATASET, *PPGDDATASET;

struct tagPGDBRANCHRECORD {
    WORD BranchId;               // branch index relative to current function
    WORD Type : 4;               // branch type
    WORD SubNumber : 4;          // branch number relative to statement
    WORD Status : 4;             // branch status
    WORD LoopTest : 1;           // branch is loop test
    WORD unused : 3;             //
};
typedef struct tagPGDBRANCHRECORD PGDBRANCHRECORD, *PPGDBRANCHRECORD;


typedef enum {
    POGODB_FG_COND = 0, POGODB_FG_UNCOND, POGODB_FG_SWITCH, POGODB_FG_USWITCH,
    POGODB_FG_RETURN, POGODB_FG_FALL_THROUGH, POGODB_FG_DUMMY,
    POGODB_FG_MAX
} POGODB_FG_ENUM;
extern POGODLL const char * const PogoDbFGNodeType[];


// Note: these structures do not appear in the compiler (although, one could
// argue they should to hide the DB encoding).  They are currently only
// populated by pogodb.c for use by the dumpers.  All these DWORD are really
// WORD on disk, but no point in memory.

struct tagPGDFGSWITCHDATA {
    DWORD val;
    DWORD target;
};
typedef struct tagPGDFGSWITCHDATA PGDFGSWITCHDATA, *PPGDFGSWITCHDATA;

struct tagPGDFGCALLDATA {
    DWORD num;
    DWORD index;
    DWORD id;
    DWORD indirValueId;
};
typedef struct tagPGDFGCALLDATA PGDFGCALLDATA, *PPGDFGCALLDATA;

struct tagPGDFLOWGRAPHNODE {
    DWORD blockNum;
    DWORD startLine, endLine;
    DWORD numInstrs, numCalls, numIndirectCalls;
    DWORD fFoldableCmp : 1;
    DWORD fUnused : 31;
    POGODB_FG_ENUM type;
    DWORD blockTarget;

    union {
        struct {
            DWORD condNum;
            DWORD condSubNum;
            DWORD condType;
            DWORD condStatus;
        };
        struct {
            DWORD switchNum;
            DWORD switchCount;
            PPGDFGSWITCHDATA pSwitchData;
        };
    };
    PPGDFGCALLDATA pCallData;
};
typedef struct tagPGDFLOWGRAPHNODE PGDFLOWGRAPHNODE, *PPGDFLOWGRAPHNODE;


struct tagPGDFLOWGRAPHINFO {
    DWORD prologInstrs;
    DWORD epilogInstrs;
};
typedef struct tagPGDFLOWGRAPHINFO PGDFLOWGRAPHINFO, *PPGDFLOWGRAPHINFO;

struct tagPGDFOLDEDCMP {
    DWORD num;
    DWORD subNum; // -1 == switch
};
typedef struct tagPGDFOLDEDCMP PGDFOLDEDCMP, *PPGDFOLDEDCMP;

#define PGDFC_SWITCH_SUBNUM -1

struct tagPGDINLINESAVINGS {
    DWORD foldedSize;
    DWORD numFoldedCmps;
    PGDFOLDEDCMP foldedCmps[]; // array of size numFoldedCmps
};
typedef struct tagPGDINLINESAVINGS PGDINLINESAVINGS, *PPGDINLINESAVINGS;

struct tagPGDCALLGRAPHCALLEE {
    DWORD num;
    DWORD callSiteNum;
    DWORD pathInc;
    PPGDINLINESAVINGS pInlSavings;
};
typedef struct tagPGDCALLGRAPHCALLEE PGDCALLGRAPHCALLEE, *PPGDCALLGRAPHCALLEE;

struct tagPGDCALLGRAPHNODE {
    DWORD id;
    DWORD flags;
    DWORD numCallees;
    DWORD startPathValOpt;
    DWORD startPathValInst;
    DWORD overheadSize;
    PPGDCALLGRAPHCALLEE callees;
};
typedef struct tagPGDCALLGRAPHNODE PGDCALLGRAPHNODE, *PPGDCALLGRAPHNODE;

#define PGDCGNF_PATHPROFILE 0x01
#define PGDCGNF_REDIRECTOR  0x02


// This record is a copy of MLE from merge.h in the bbt tree.  There are a
// couple of unused fields for POGO.

struct tagPGDMERGERECORD {
   QWORD    qwStartTime;           // Instrumentation start time
   DWORD    dwWeight;              // Weight (relative importance)
   DWORD    iSequenceEndBoot;      // Sequence number of the last boot time code or resource
   DWORD    iIntervalEndBoot;      // Last time interval of boot
   BOOL     fSubtract;             // True if subtracting this merge
};
typedef struct tagPGDMERGERECORD PGDMERGERECORD, *PPGDMERGERECORD;


// This is not a database record, but is shared by the compiler and runtime
// so this is the best place to define it.

struct tagPOGOVALUETEMPLATE {
    DWORD   probeOffset;           // probe offset (must be first!)
                                   // this is a runtime convenience to avoid
                                   // passing both the template pointer and
                                   // probe offset to the probe thunks
    WORD    numCases;              // number of cases in template
    struct {
        WORD fSixtyFourBits:1;     // Size of values: 0 = 32, 1 = 64
        WORD fSigned:1;            // Signed or unsigned
        WORD fUnused:14;
    };
    void *data;                    // numCases * 32/64 ...
};
typedef struct tagPOGOVALUETEMPLATE POGOVALUETEMPLATE, *PPOGOVALUETEMPLATE;


// External representation.

struct tagPGDTOBCHUNK {
    DWORD    firstInterval;          // time interval of bit 0
    DWORD    bits;                   // bits for intervals firstInterval..+31
};
typedef struct tagPGDTOBCHUNK PGDTOBCHUNK, *PPGDTOBCHUNK;

struct tagPGDTOBVECTOR {
    unsigned len;                    // number of chunks in bit vector
    PPGDTOBCHUNK pChunks;           // array of chunks
};
typedef struct tagPGDTOBVECTOR PGDTOBVECTOR, *PPGDTOBVECTOR;

// Internal representation.

struct tagPGDTOBDATA {
    PGDTOBCHUNK chunk;
    LONG offsetNext;            // offset to next TOB
};
typedef struct tagPGDTOBDATA PGDTOBDATA, *PPGDTOBDATA;


extern POGODLLAPI(POGODB_ERROR) PogoDbGetLastError(void);
extern POGODLLAPI(const char *) PogoDbGetErrorText(POGODB_ERROR error);
extern POGODLLAPI(BOOL) PogoDbFormatTime(const QWORD *time, char *buffer);
extern POGODLLAPI(const char *) PogoDbGetMachineName(DWORD idMachine);
extern POGODLLAPI(DWORD) PogoDbGetMachineId(const char *szName);
extern POGODLLAPI(BOOL) PogoDbGenerateModuleName(const char *PGDName, const char *OBJName, char *moduleName, size_t bufLen);

extern POGODLLAPI(BOOL) PogoDbOpen(const char *name, POGODB_MODE mode, BOOL fLTCG);
extern POGODLLAPI(BOOL) PogoDbClose(void);

extern POGODLLAPI(void *) PogoDbReadStream(POGODB_STREAM stream, LONG *cb);
extern POGODLLAPI(BOOL) PogoDbStreamQueryCb(POGODB_STREAM stream, LONG *cb);
extern POGODLLAPI(BOOL) PogoDbStreamTruncate(POGODB_STREAM stream, LONG cb);
extern POGODLLAPI(BOOL) PogoDbStreamReplace(POGODB_STREAM stream, void *pData, LONG cb);
extern POGODLLAPI(BOOL) PogoDbStreamWrite(POGODB_STREAM stream, DWORD offset, void *pData, LONG cb);
extern POGODLLAPI(BOOL) PogoDbStreamAppend(POGODB_STREAM stream, void *pData, LONG cb);
extern POGODLLAPI(PPGDHEADER) PogoDbReadHeader(void);
extern POGODLLAPI(PPGDHISTORYRECORD) PogoDbReadHistory(PPGDHISTORYRECORD pHistory);
extern POGODLLAPI(PPGDMODULERECORD) PogoDbReadModule(PPGDMODULERECORD pMod);

extern POGODLLAPI(PPGDMODULERECORD) PogoDbGetLogicalModule(void);
extern POGODLLAPI(PPGDSYMTABRECORD) PogoDbGetSymtabRecord(LONG offset);
extern POGODLLAPI(const char *) PogoDbGetSymbol(LONG id);
extern POGODLLAPI(DWORD) PogoDbLookupSymbol(const char *pSym);
extern POGODLLAPI(BOOL) PogoDbModifiedSymbolTable(void);
extern POGODLLAPI(DWORD) PogoDbLookupNextId(DWORD id, DWORD FlagsMask, DWORD FlagsValue);
extern POGODLLAPI(DWORD) PogoDbLookupId(const char *pSym, DWORD FlagsMask, DWORD FlagsValue);

extern POGODLLAPI(BOOL) PogoDbWriteObjectInfoStart(void);
extern POGODLLAPI(BOOL) PogoDbWriteObjectInfo(const char *szObjectName, QWORD ObjectTime, QWORD FileTime);
extern POGODLLAPI(BOOL) PogoDbWriteObjectInfoEnd(void);
extern POGODLLAPI(PPGDOBJECTRECORD) PogoDbReadObjectInfo(PPGDOBJECTRECORD pObj);

extern POGODLLAPI(BOOL) PogoDbWriteLinkInfo(struct LinkInfo *pLinkInfo);
extern POGODLLAPI(BOOL) PogoDbReadLinkInfo(struct LinkInfo **ppLinkInfo);

extern POGODLLAPI(BOOL) PogoDbOpenModule(PPGDMODULERECORD pMod);
extern POGODLLAPI(BOOL) PogoDbWriteModuleChecksum(DWORD checksum);
extern POGODLLAPI(DWORD) PogoDbReadModuleChecksum(void);
extern POGODLLAPI(const char *) PogoDbGetModuleName(void);
extern POGODLLAPI(void) PogoDbCloseModule(void);

extern POGODLLAPI(BOOL) PogoDbLocateFunctionRecord(DWORD id, PPGDFUNCRECORD *pFunc);
extern POGODLLAPI(BOOL) PogoDbSetFunction(const char *funcName, DWORD symFlags);
extern POGODLLAPI(PPGDFUNCRECORD) PogoDbReadFunction(PPGDFUNCRECORD pFunc);
extern POGODLLAPI(BOOL) PogoDbOpenFunction(PPGDFUNCRECORD pFunc);
extern POGODLLAPI(const char *) PogoDbGetFunctionName(void);
extern POGODLLAPI(PPGDENTRYRECORD) PogoDbReadFunctionEntry(void);
extern POGODLLAPI(DWORD) PogoDbReadFunctionChecksum(PPGDFUNCRECORD pFunc);
extern POGODLLAPI(BOOL) PogoDbReadFunctionOverride(PPGDFUNCOVERRIDE pOverride);
extern POGODLLAPI(BOOL) PogoDbWriteFunctionOverride(PPGDFUNCOVERRIDE pOverride);
extern POGODLLAPI(void) PogoDbCloseFunction(void);

extern POGODLLAPI(BOOL) PogoDbModifiedFunctionData(void);
extern POGODLLAPI(BOOL) PogoDbModifiedDataSets(void);

extern POGODLLAPI(PPGDSIMPLEPROBE) PogoDbReadSimpleProbe(PPGDSIMPLEPROBE pSimple, PPGDSIMPLECOUNT *ppSimpleCount, DWORD offset);
extern POGODLLAPI(PPGDSIMPLEPROBE) PogoDbReadSimpleProbeEx(PPGDSIMPLEPROBE pSimple, PPGDSIMPLECOUNT *ppSimpleCount, PPGDFUNCRECORD pFunc, DWORD offset);
extern POGODLLAPI(PPGDVALUEPROBE) PogoDbReadValueProbe(PPGDVALUEPROBE pValue, PPGDVALUECOUNT *ppValueCount, DWORD offset);
extern POGODLLAPI(PPGDVALUEPROBE) PogoDbReadValueProbeEx(PPGDVALUEPROBE pValue, PPGDVALUECOUNT *ppValueCount, PPGDFUNCRECORD pFunc, DWORD offset);
extern POGODLLAPI(PPGDVALUEPROBEDATA) PogoDbReadValueProbeData(PPGDVALUEPROBEDATA pValueData, PPGDVALUECOUNT pValueCount);
extern POGODLLAPI(PPGDTOBVECTOR) PogoDbReadTOBVector(LONG offset);

extern POGODLLAPI(PPGDDATASET) PogoDbReadDataSet(PPGDDATASET pMDS);
extern POGODLLAPI(PPGDDATASET) PogoDbReadDataSetEx(PPGDDATASET pMDS, PPGDFUNCRECORD pFunc);
extern POGODLLAPI(BOOL) PogoDbLocateDataSet(DWORD CallPathId);
extern POGODLLAPI(PPGDBRANCHRECORD) PogoDbReadBranchData(PPGDBRANCHRECORD pBranch);
extern POGODLLAPI(PPGDBRANCHRECORD) PogoDbReadBranchDataEx(PPGDBRANCHRECORD pBranch, PPGDFUNCRECORD pFunc);
extern POGODLLAPI(PPGDFLOWGRAPHNODE) PogoDbReadFlowGraph(PPGDFLOWGRAPHNODE pFG);
extern POGODLLAPI(PPGDFLOWGRAPHINFO) PogoDbReadFlowGraphInfo(void);

extern POGODLLAPI(BOOL) PogoDbOpenCallGraph(DWORD *numNodes);
extern POGODLLAPI(PPGDCALLGRAPHNODE) PogoDbReadCallGraphNode(PPGDCALLGRAPHNODE pLast);
extern POGODLLAPI(DWORD) PogoDbGetCallGraphNodeSym(DWORD iNode);
extern POGODLLAPI(PPGDCALLGRAPHCALLEE) PogoDbReadCallGraphCallee(PPGDCALLGRAPHCALLEE pLast, PPGDCALLGRAPHNODE pNode);
extern POGODLLAPI(void) PogoDbCloseCallGraph(void);

extern POGODLLAPI(BOOL) PogoDbUpdateHistory(POGODB_HISTORY_TYPE type, QWORD MergeFileTime, const char *name);

extern POGODLLAPI(BOOL) PogoDbStartNewFunction(void);
extern POGODLLAPI(BOOL) PogoDbWriteFunctionChecksum(DWORD checksum);

extern POGODLLAPI(BOOL) PogoDbWriteFlowGraphStart(void);
extern POGODLLAPI(BOOL) PogoDbWriteFlowGraphNode(DWORD blkNum, DWORD startLineOffset, DWORD endLineOffset, DWORD instrs, DWORD calls, DWORD indirectCalls);
extern POGODLLAPI(BOOL) PogoDbWriteFlowGraphSwitch(DWORD id, BOOL fSigned, DWORD defaultTarget, DWORD cases, BOOL fFoldable);
extern POGODLLAPI(BOOL) PogoDbWriteFlowGraphSwitchData(DWORD val, DWORD target);
extern POGODLLAPI(BOOL) PogoDbWriteFlowGraphCond(DWORD num, DWORD subNum, DWORD type, DWORD status, DWORD target, BOOL fFoldable);
extern POGODLLAPI(BOOL) PogoDbWriteFlowGraphUncond(DWORD target);
extern POGODLLAPI(BOOL) PogoDbWriteFlowGraphReturn(void);
extern POGODLLAPI(BOOL) PogoDbWriteFlowGraphFallThrough(void);
extern POGODLLAPI(BOOL) PogoDbWriteFlowGraphDummy(void);
extern POGODLLAPI(BOOL) PogoDbWriteFlowGraphCall(DWORD num, DWORD index, DWORD id, DWORD indirValueId);
extern POGODLLAPI(BOOL) PogoDbWriteFlowGraphEnd(DWORD prologInstrs, DWORD epilogInstrs);

extern POGODLLAPI(BOOL) PogoDbWriteBranchInfoStart(void);
extern POGODLLAPI(BOOL) PogoDbWriteBranchInfo(DWORD Id, DWORD Type, DWORD subnumber, DWORD Status, BOOL LoopTest);
extern POGODLLAPI(BOOL) PogoDbWriteBranchInfoEnd(void);

extern POGODLLAPI(void) PogoDbInitOffsetAllocation(BOOL fMDS);
extern POGODLLAPI(DWORD) PogoDbValueProbeOffset(void);
extern POGODLLAPI(BOOL) PogoDbWriteProbeInfoStart(void);
extern POGODLLAPI(BOOL) PogoDbWriteSimpleProbeInfo(DWORD lineOffset, DWORD id, DWORD offset, DWORD flags);
extern POGODLLAPI(BOOL) PogoDbWriteValueProbeInfo(DWORD lineOffset, DWORD id, DWORD offset, DWORD type, DWORD flags);
extern POGODLLAPI(BOOL) PogoDbWriteProbeInfoEnd(void);

extern POGODLLAPI(BOOL) PogoDbWriteCallGraphStart(DWORD nodes);
extern POGODLLAPI(BOOL) PogoDbWriteCallGraphNode(DWORD id, DWORD flags, DWORD startPathValOpt, DWORD startPathValInst, DWORD callees, DWORD overheadSize);
extern POGODLLAPI(BOOL) PogoDbWriteCallGraphCallee(DWORD num, DWORD index, DWORD pathInc);
extern POGODLLAPI(BOOL) PogoDbWriteCallGraphCalleeInlineSavings(DWORD foldedSize, DWORD numFoldedCmps);
extern POGODLLAPI(BOOL) PogoDbWriteCallGraphCalleeFoldedCmp(DWORD id, DWORD subNum);
extern POGODLLAPI(BOOL) PogoDbWriteCallGraphEnd(void);

extern POGODLLAPI(DWORD) PogoDbAllocEntryProbeId(void);
extern POGODLLAPI(DWORD) PogoDbAllocSimpleProbeId(void);
extern POGODLLAPI(DWORD) PogoDbAllocValueProbeId(void);

extern POGODLLAPI(DWORD) PogoDbLookupFunctionRef(const char *szFuncName, BOOL fLocalRef, DWORD SymFlags);
extern POGODLLAPI(BOOL) PogoDbSetModule(const char *moduleName);
extern POGODLLAPI(BOOL) PogoDbSetLogicalModule(PPGDMODULERECORD pModule);
extern POGODLLAPI(BOOL) PogoDbSetLogicalModuleFromName(const char *moduleName);
extern POGODLLAPI(PPGDHEADER) PogoDbCreateGlobalStreams(const char *pdbName);
extern POGODLLAPI(BOOL) PogoDbReadGlobalStreams(void);
extern POGODLLAPI(BOOL) PogoDbWriteFunctionInfo(DWORD LineNumberBase, DWORD MaxSimpleProbe, DWORD MaxValueProbe, DWORD EntryProbeId, const char *szFuncName, DWORD SymFlags, BOOL fMDS);
extern POGODLLAPI(BOOL) PogoDbFinishFunction(void);

extern POGODLLAPI(DWORD) PogoDbGetSymbolHashPrime(void);
extern POGODLLAPI(DWORD) PogoDbGetFuncHashPrime(void);

extern POGODLLAPI(BOOL) PogoDbWriteVCallTarget(DWORD id);
extern POGODLLAPI(BOOL) PogoDbReadVCallTargets(DWORD **pTable, DWORD *num);


#pragma warning(pop)

#if __cplusplus
} // extern "C"
#endif
