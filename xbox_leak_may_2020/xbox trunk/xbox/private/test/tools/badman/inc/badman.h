/*
 * MODULE:  BadMan.h
 * AUTHOR:  JohnMil
 * DATE:    2-5-92
 *
 * Copyright (c) 1992-5 Microsoft Corporation
 *
 * This module contains Typedefs and macros uses throughout BadMan. No
 * function prototypes should be put into this file.
 */


#ifndef _BADMANH
#define _BADMANH


#include    <windows.h>
#include    <string.h>
#include    <stdlib.h>
#include    <stdio.h>


/*
 * An absolute value function
 */

#define ABS(x)          ((x) < 0 ? -(x) : (x))


/*
 * Maximum # of parameters in an API call. If this number is changed, the
 * number of parms used in the call in GenParm() must be changed by hand.
 */

#define MAXNOPARMS      20


/*
 * Hashing tables are used for storing types of data for which there are a
 * large number.
 */

/*
 * # of buckets in the Definitions Hash Table
 */
#define HASH_SIZE       199


/*
 * # of buckets in the Macros Hash Table
 */

#define MACRO_HASH_SIZE     19


/*
 * # of buckets in the Data Type Hash Table
 */

#define DATA_HASH_SIZE      19


/*
 * An arbitrary number that represents the value of a define which was not
 * found. This number MUST NOT MATCH any of the defines.
 */

#define BAD_DEFINE      9194716


/*
 * Different types of return values specified in a script entry for use in
 * ReturnType.Type.
 */

#define RET_ISEQUAL     0x0
#define RET_VALUE       0x1
#define RET_PARM        0x2
#define RET_NONE        0x4
#define RET_NOTEQUAL        0x8


/*
 * CL = Command Line options. info passed in by the user and defaults
 *
 * Flags                CLOPT_xxx flags are set in this int
 * MinPerfIterations    Minimum performance iterations
 * MaxPerfIterations    maximum performance iterations
 * RunFile[256]         Name of the script file
 * LogFile[256]         LogFile Name
 * APIName[256]         Name of a selected API, if any
 * VarNum               Variation number to run, if picked
 */

typedef struct  _s_CLType   {
    int         Flags;  
    int         MinPerfIterations,
                MaxPerfIterations;
    char        RunFile[256],
                //LogFile[256],
                APIName[256];
    int         VarNum;
} CLType;


/*
 * SeenType is used to help with knowing what parameter sets to keep or
 * destroy.
 * S            True/false for if a parm name has been seen yet
 * next         pointer to the next in the list
 */

typedef struct  _s_SeenType {
    BOOL            S[MAXNOPARMS];
    struct _s_SeenType  *next;
} SeenType;

        // Data structure for hashed defines.
typedef struct _sDefin      {
    char            *Symbol,    // Name of the define
                *Comment;   // Comment for the define
    int         Case;       // Actual value for the define
    struct _sDefin      *next;
}  DefineType;


/*
 * StringTypes are linked lists of strings. This is used primarily for Data
 * Types.
 */

typedef struct _s_String    {
    char            *Name;
    struct _s_String    *next;
} StringType;


/*
 * This is for a linked list of Good Cases for a Parameter.
 */

typedef struct _s_GoodVal {
        // V is a direct pointer to the define for a given case
    DefineType      *V;
    struct _s_GoodVal   *next;
} GoodValType;


        // BadVal is a linked list of bad cases for a parm.
typedef struct _s_BadVal {
    DefineType      *V, // Pointer to the bad variation define
                *L; // Pointer to a definition for GetLastError, if any
    int         LastError;  // BAD_DEFINE, else the value expected from GetLastError
    struct _s_BadVal    *next;
} BadValType;


/*
 * Macro's can be of either good or bad cases. Since Good are a subset of bad,
 * it's natural to use bad.
 */

typedef BadValType MacroValType;


/*
 * Macro of multiple cases
 */

typedef struct _s_Macro {
    char            *Name;  // Name of the macro
    MacroValType        M;  // Head of linked list of cases
    struct _s_Macro     *next;
} MacroType;


/*
 * Data structure for a parameter and its current set of Good and Bad cases.
 */

typedef struct _s_Parms {
    StringType      *Type;  // Pointer directly to the datatype
    char            *Name;  // Name of the parameter
    BOOL            bIsEndCase; // Whether this Parm signals the end of a case.
    BOOL            NoPush; // Don't put this param on stack
    GoodValType     Good;   // head of good cases linked list
    BadValType      Bad;    // head of bad cases linked list
    struct _s_Parms     *Depend, // Dependency lists are more parameter entries.
                *next;  // Depend->next is the first entry.
} S_PARMS;


/*
 * ParmDep is a linked list of lists of parms.  it's used for processing
 * dependency lists.
 */

typedef struct _s_ParmDep {
    S_PARMS         *P; // Parameter list for this ParmDep
    struct _s_ParmDep   *next;
} ParmDepType;


/*
 * Type of return argument to compare against API return
 */

typedef struct _s_ReturnType {
    DWORD           Type;
    int         Value;  // Int if return is a value, else parm # if parm.
} ReturnType;


/*
 * APIType contains an entire entry for an API.
 */

typedef struct _s_APIType {
    StringType      *Type;  // Direct pointer to a datatype
    char            *Name;  // Name of the API
    S_PARMS         Parms;  // ParmList head
    ReturnType      RValue; // Return value to compare
} APIType;


/*
 * Information for dynamically loaded libraries.
 */

typedef struct _s_ModType {
    char            *Name;  // Name of the library
    HANDLE          h;  // Handle to it.
} ModType;


typedef DWORD (FAR *FARFNSETUP)(int CaseNo, LPVOID *SCInfo, HANDLE hLog,
    HANDLE hConOut);

typedef void (FAR *FARFNCLEANUP)(int CaseNo, LPVOID *SCInfo, HANDLE hLog,
    HANDLE hConOut);

typedef void (FAR *FARFNVALIDATE)(DefineType VarInfo[], DWORD dwParms[],
    int ParmGoodBad[], DWORD dwRetVal,
    HANDLE hLog, HANDLE hConOut);

typedef VOID (FAR *FARFNAPICLN)(DWORD dwRet, DWORD *dwParms,
    HANDLE hConOut, HANDLE hLog);

typedef DWORD (FAR WINAPI *FARDPROC)();



#define malloc(x)   calloc(1, x)


#endif // _BADMANH
