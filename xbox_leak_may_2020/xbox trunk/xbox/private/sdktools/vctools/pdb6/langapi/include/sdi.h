//
// SQL Debug Information API
// Copyright (C) 1995, Microsoft Corp.	All Rights Reserved.
//

#include <stddef.h>

#ifndef __SDI_INCLUDED__
#define __SDI_INCLUDED__

#pragma pack(push, enter_SDI)
#pragma pack(2)

// Basic typedefs
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned long ULONG;
typedef unsigned short USHORT;

#ifndef __SDIDEF_INCLUDED__
#include "sdidef.h"
#endif // __SDIDEF_INCLUDED__


typedef SPID SDI_SPID;		// for VC

// Interface Version Number; they are shipped with VC so adopt the VC version suffix
enum {	SDIIntv42 = 951027, SDIIntv50 = 961025, SDIIntv60 = 970915 };

// Interesting values
enum {	
		cbMchNmMax   = 32,	// maximum m/c name length (sql has a limit of 30)
		cbSqlSrvrMax = 32	// maximum sql server name length
};

// dll/entrypoint names
#define SDIDLL_ENTRYPOINT   "SDIInit"

#ifndef EXTERN_C
#if __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif
#endif

// init API
typedef struct _SQLFNTBL SQLFNTBL;
typedef struct _DBGFNTBL DBGFNTBL;
typedef long SDI_EC;            // error code

BOOL _stdcall SDIInit (SQLFNTBL *psqlfntbl, INTV intvsql, DBGFNTBL **ppdbgfntbl, INTV *pintvdbg);

typedef BOOL (_stdcall *pfnSDIInit)(SQLFNTBL *psqlfntbl, INTV intvsql, DBGFNTBL **ppdbgfntbl, INTV *pintvdbg);

// execution control API
typedef BOOL (_stdcall *pfnSDINewSPID)(SDI_SPID spid, char szSqlServer[cbSqlSrvrMax], char szMachine[cbMchNmMax], void *pvDbgData,ULONG cbDbgData,PID pid, PID dbgpid);
typedef BOOL (_stdcall *pfnSDINewSP)  (SDI_SPID spid, char *DBName, USHORT DBLen, PRID prid, NLVL nlvl);
typedef BOOL (_stdcall *pfnSDIStep) (SDI_SPID spid, PRID prid, IDX idx, OFF off, NLVL nlvl);
typedef BOOL (_stdcall *pfnSDINewBatch)(SDI_SPID spid, char *CmdBuf, ULONG cbCmdBuf, NLVL nlvl);
typedef BOOL (_stdcall *pfnSDIPop)(SDI_SPID spid, NLVL nlvl);
typedef BOOL (_stdcall *pfnSDICloseConnection)(SDI_SPID spid, ULONG sqlerror, SDI_EC sdierror);
typedef BOOL (_stdcall *pfnSDISpidContext)(SDI_SPID spid, PID pid, THID tid);
//Provide a list of line numbers for action SQL statements
typedef BOOL (_stdcall *pfnSDIActionLines)(SDI_SPID spid, PRID prid, USHORT *rgusActionLines, USHORT cActionLines);

// data handling API

// 4.2
typedef BOOL (_stdcall *pfnSDISetSym) (SDI_SPID spid, SYMS syms, PSYMINFO psyminfo, NLVL nlvl);
typedef BOOL (_stdcall *pfnSDIGetSym) (SDI_SPID spid, SYMS syms, PSYMINFO *prgsyminfo, USHORT *pcsym, NLVL nlvl);
//6.0
typedef BOOL (_stdcall *pfnSDIGetSym60) (SDI_SPID spid, SYMS syms, PSYMINFO *prgsyminfo, USHORT *pcsym, NLVL nlvl, ULONG cbMax);

// version checking API

// error handling/shutdown API
typedef BOOL (_stdcall *pfnSDIDbgOff)(SDI_SPID);
typedef BOOL (_stdcall *pfnSDIError) (SDI_SPID spid, char *szDB, USHORT cchDB, PRID prid, IDX idx, OFF off, long numErr, char *szErr, ULONG cbErr);

enum SDIErrors {                // possible error codes
    SDI_OK,                     // looking good
    SDI_USAGE,                  // invalid paramter etc. should never happen
    SDI_VERSION,                // version mismatch; cannot proceed
    SDI_OUT_OF_MEMORY,          // out of memory
    SDI_SYM_NOTFOUND,           // invalid sym name
    SDI_INVALID_SYMTYPE,        // invalid sym type
    SDI_INVALID_SYMVAL,         // invalid sym value
    SDI_INVALID_SPID,           // invalid spid
    SDI_SHUTDOWN,               // code set during SDIDbgOff
    SDI_MAX                     // last code we know of
};

typedef SDI_EC (_stdcall *pfnSDIGetLastError) (void);    

// memory management routines
typedef void * (_stdcall *pfnSDIPvAlloc)  (size_t cb);
typedef void * (_stdcall *pfnSDIPvAllocZ) (size_t cb);
typedef void * (_stdcall *pfnSDIPvRealloc)(void *pv, size_t cb);
typedef void   (_stdcall *pfnSDIFreePv)   (void *pv);

// function tables
// Important!!  The caller of these SQL Server APIs should always have the same SPID as
// the SPID argument they pass, since fatal errors in these routines will cause an
// MSSqlSDICleanup, which cleans up everything and switch off SQL Debugging.
//
typedef struct _SQLFNTBL {                      // function table filled in by sql server
    pfnSDIGetLastError          SDIGetLastError;//4.2
    pfnSDIGetSym                SDIGetSym;		//4.2
    pfnSDISetSym                SDISetSym;		//4.2
    pfnSDIDbgOff                SDIDbgOff;		//4.2
    pfnSDIGetSym60              SDIGetSym60;	//6.0
} SQLFNTBL, *PSQLFNTBL;

typedef struct _DBGFNTBL {                      // function table filled in by debug dll
    pfnSDINewSPID				SDINewSPID;		//4.2
    pfnSDINewSP                 SDINewSP;		//4.2
    pfnSDIStep                  SDIStep;		//4.2
    pfnSDINewBatch              SDINewBatch;	//4.2
    pfnSDIPvAlloc               SDIPvAlloc;		//4.2
    pfnSDIPvAllocZ              SDIPvAllocZ;	//4.2
    pfnSDIPvRealloc             SDIPvRealloc;	//4.2
    pfnSDIFreePv                SDIFreePv;		//4.2
    pfnSDIPop                   SDIPop;			//4.2
    pfnSDICloseConnection       SDICloseConnection;//4.2
    pfnSDISpidContext           SDISpidContext;	//4.2
	pfnSDIError					SDIError;		//5.0
	pfnSDIActionLines			SDIActionLines;	//6.0
} DBGFNTBL, *PDBGFNTBL;

// macros for ease of use
#define SDINEWSPID(pfntbl,spid,szSqlSrvr,szMachine,pvDbgData,cbDbgData,pid,dbgpid) ((*((pfntbl)->SDINewSPID))((spid),(szSqlSrvr),(szMachine),(pvDbgData),(cbDbgData),(pid),(dbgpid)))
#define SDINEWBATCH(pfntbl,spid,CmdBuf,cbCmdBuf,nlvl) ((*((pfntbl)->SDINewBatch))((spid),(CmdBuf),(cbCmdBuf),(nlvl)))
#define SDINEWSP(pfntbl, spid, DBName, DBLen, prid, nlvl) ((*((pfntbl)->SDINewSP))((spid),(DBName),(DBLen),(prid),(nlvl)))
#define SDISTEP(pfntbl, spid, prid, idx, off, nlvl) ((*((pfntbl)->SDIStep))((spid),(prid),(idx),(off),(nlvl)))
#define SDIPOP(pfntbl, spid, nlvl) ((*((pfntbl)->SDIPop))((spid),(nlvl)))
#define SDICLOSECONNECTION(pfntbl,spid,sqlerror,sdierror) ((*((pfntbl)->SDICloseConnection))((spid),(sqlerror),(sdierror)))
#define	SDISPIDCONTEXT(pfntbl,spid,pid,tid) ((*((pfntbl)->SDISpidContext))((spid),(pid),(tid)))
#define SDIACTIONLINES(pfntbl,spid,prid,rgusActionLines, cActionLines) ((*((pfntbl)->SDIActionLines))((spid),(prid),(rgusActionLines),(cActionLines)))
#define SDIGETSYM(pfntbl, spid, syms, rgsyminfo, cnt, nlvl)((*((pfntbl)->SDIGetSym))((spid),(syms),(rgsyminfo),(cnt),(nlvl)))
#define SDISETSYM(pfntbl, spid, syms, psyminfo, nlvl)((*((pfntbl)->SDISetSym))((spid),(syms),(psyminfo),(nlvl)))
#define SDIGETSYM60(pfntbl, spid, syms, rgsyminfo, cnt, nlvl, cbMax)((*((pfntbl)->SDIGetSym60))((spid),(syms),(rgsyminfo),(cnt),(nlvl), (cbMax)))

#define SDIDBGOFF(pfntbl, spid)((*((pfntbl)->SDIDbgOff))((spid)))

#define SDIGETLASTERROR(pfntbl)((*((pfntbl)->SDIGetLastError))())

#define SDIPVALLOC(pfntbl, cb)((*((pfntbl)->SDIPvAlloc))((cb)))
#define SDIPVALLOCZ(pfntbl, cb)((*((pfntbl)->SDIPvAllocZ))((cb)))
#define SDIPVREALLOC(pfntbl, pv, cb)((*((pfntbl)->SDIPvReAlloc))((pv), (cb)))
#define SDIFREEPV(pfntbl, pv)((*((pfntbl)->SDIPvReAlloc))((pv)))
#define SDIERROR(pfntbl, spid, szDB, cchDB, prid, idx, off, numErr, szErr, cbErr) ((*((pfntbl)->SDIError))((spid),(szDB), (cchDB), (prid), (idx), (off), (numErr),(szErr),(cbErr)))

#pragma pack(pop, enter_SDI)

#endif // __SDI_INCLUDED__
