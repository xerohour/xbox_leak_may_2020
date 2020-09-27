//
// SQL Debug Information Definitions
// Copyright (C) 1997, Microsoft Corp.	All Rights Reserved.
//

#ifndef __SDIDEF_INCLUDED__
#define __SDIDEF_INCLUDED__

#pragma pack(push, enter_SDIdef)
#pragma pack(2)

// Basic typedefs
typedef ULONG   INTV;       // interface version number
typedef ULONG   IMPV;       // implementation version number
typedef ULONG   SPID;       // connection id
typedef ULONG   PRID;       // stored procedure id
typedef USHORT  IDX;        // statement index
typedef USHORT  NLVL;       // nesting level
typedef USHORT  OFF;        // offset into stored proc/batch
typedef ULONG   PID;        // process id
typedef ULONG	THID;	     // thread id
typedef ULONG	SQLLEID;   // token issued by SQL LE for every connection

typedef enum {symGlobals, symLocals, symParams} SYMS;       // class of syms

typedef enum {                                              // sym types
    stInvalid,  
    stBIT,
    stTIMESTAMP,
    stTINYINT,
    stSMALLINT,
    stINT,
    stREAL,
    stFLOAT,
    stNUMERIC,
    stDECIMAL,
    stCHAR,
    stVARCHAR,
    stBINARY,
    stVARBINARY,
    stSMALLMONEY,
    stMONEY,
    stSMALLDATETIME,
    stDATETIME,
    stTEXT,
    stIMAGE,
 	stGUID,
	stLARGEINT,
	stNCHAR,
	stNVARCHAR,
	stNTEXT,
	stMONEY60,	//SQL Server 7.0 format; compatible with VC Ent 6.0
    stSMALLDATETIME60,//SQL Server 7.0 format; compatible with VC Ent 6.0
    stDATETIME60,//SQL Server 7.0 format; compatible with VC Ent 6.0
    stLast
} SYM_TYPE;                 


typedef struct _SYMINFO {
    SYM_TYPE    st;                 // symbol type
    void        *pv;                // ptr to symbol value
    USHORT      cb;                 // length 
    USHORT      cbName;             // length of name
    char        *Name;              // symbol name
    BYTE        cbPrec;             // precision info
    BYTE        cbScale;            // scale info
} SYMINFO, *PSYMINFO;

#pragma pack(pop, enter_SDIdef)

#endif // __SDIDEF_INCLUDED__
