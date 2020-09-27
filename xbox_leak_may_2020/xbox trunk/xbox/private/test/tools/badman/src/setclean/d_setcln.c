/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    d_setcln.c

Abstract:

    This module contains setup and cleanup functions for data types
    beginning with the letter 'D'. It also contains setup and cleanup
    funtions beginning with 'LPD'. For more information, please refer
    to BadMan.Doc.

Author:

    John Miller (johnmil) 02-Feb-1992

Environment:

    XBox

Revision History:

    01-Apr-2000     schanbai

        Ported to XBox and removed not needed cases

--*/


#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <setcln.h>
#include <filever.h>
// #include <d3d.h>
#include <D_Cases.bmh>
#include <S_Cases.bmh>
#include <memmgt.h>
// #include <ddeml.h>


int TranslateCase(int CaseNo);

/******************************
DDD   W   W  OOO  RRRR  DDD
D  D  W   W O   O R   R D  D
D   D W W W O   O RRRR  D   D
D   D WW WW O   O R  R  D   D
DDDD  W   W  OOO  R   R DDDD
******************************/

// defs for memmgt alloc flags:

#define GA_VALID  (DWORD)( GMEM_DISCARDABLE | GMEM_FIXED     | GMEM_LOWER     |   \
    GMEM_MOVEABLE      | GMEM_NOCOMPACT | GMEM_NODISCARD | \
    GMEM_NOT_BANKED  | GMEM_NOTIFY    | GMEM_SHARE     |   \
    GMEM_ZEROINIT    | GMEM_DDESHARE )

#define GR_VALID  (DWORD)( GMEM_DISCARDABLE | GMEM_MOVEABLE  | GMEM_MODIFY    |   \
    GMEM_NODISCARD   | GMEM_ZEROINIT )

#define LA_VALID  (DWORD)( LMEM_DISCARDABLE | LMEM_FIXED     | LMEM_MOVEABLE    | \
    LMEM_NOCOMPACT   | LMEM_NODISCARD | LMEM_ZEROINIT )

#define LR_VALID  (DWORD)( LMEM_DISCARDABLE | LMEM_MOVEABLE  | LMEM_MODIFY    |   \
    LMEM_ZEROINIT )

#define GA_INVALID   (DWORD)( ~ GA_VALID )
#define GR_INVALID   (DWORD)( ~ GR_VALID )
#define LA_INVALID   (DWORD)( ~ LA_VALID )
#define LR_INVALID   (DWORD)( ~ LR_VALID )



// extern HDDEDATA CALLBACK DdeCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1,       
//                                      HSZ hsz2, HDDEDATA hdata, DWORD dwData1, DWORD dwData2);



DWORD DWORDSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    DWORD dwRet = 0;
    
    switch (CaseNo) {
    case DWORD_VALID_LOCALALLOC_FLAGS:
        dwRet = LMEM_MOVEABLE | LMEM_DISCARDABLE;
        break;
    case DWORD_VALID_AND_SPURIOUS_LOCALALLOC_FLAGS:
        dwRet = LA_INVALID | LMEM_MOVEABLE;
        break;
    case DWORD_SPURIOUS_LOCALALLOC_FLAGS:
        dwRet = LA_INVALID;
        break;
    case DWORD_VALID_GLOBALALLOC_FLAGS:
        dwRet = GMEM_MOVEABLE;
        break;
    case DWORD_VALID_AND_SPURIOUS_GLOBALALLOC_FLAGS:
        dwRet = GA_INVALID | GMEM_MOVEABLE | GMEM_DISCARDABLE;
        break;
    case DWORD_SPURIOUS_GLOBALALLOC_FLAGS:
        dwRet = GA_INVALID;
        break;
    case DWORD_VALID_LOCALREALLOC_FLAGS:
        dwRet = LMEM_MOVEABLE;
        break;
    case DWORD_INVALID_LOCALREALLOC_FLAGS:
        dwRet = LMEM_DISCARDABLE;
        break;
    case DWORD_VALID_AND_SPURIOUS_LOCALREALLOC_FLAGS:
        dwRet = LR_INVALID | LMEM_MODIFY | LMEM_DISCARDABLE;
        break;          
    case DWORD_SPURIOUS_LOCALREALLOC_FLAGS:
        dwRet = LR_INVALID;
        break;
    case DWORD_VALID_GLOBALREALLOC_FLAGS:
        dwRet = GMEM_MOVEABLE;
        break;
    case DWORD_INVALID_GLOBALREALLOC_FLAGS:
        dwRet = GMEM_DISCARDABLE;
        break;
    case DWORD_VALID_AND_SPURIOUS_GLOBALREALLOC_FLAGS:
        dwRet = GR_INVALID | GMEM_MODIFY;
        break;
    case DWORD_SPURIOUS_GLOBALREALLOC_FLAGS:
        dwRet = GR_INVALID;
        break;
    case DWORD_ONE:
        dwRet = 1;
        break;
    case DWORD_ZERO:
        dwRet = 0;
        break;
    case DWORD_ONE_K:
        dwRet = 1024;
        break;
    case DWORD_ONE_MEG:
        dwRet = 1024*1024;
        break;
    case DWORD_MINUS_ONE:
        dwRet = (DWORD)-1;
        break;
    case DWORD_32MB:
        dwRet = 32*1024*1024;
        break;
    case DWORD_THREE_GIG:
        dwRet = 1024U*1024U*1024U*3U;
        break;
    case DWORD_ACCESSMASK_GENALL:
        dwRet = GENERIC_ALL;
        break;
    case DWORD_ACCESSMASK_GENREAD:
        dwRet = GENERIC_READ;
        break;
    case DWORD_ACCESSMASK_GRANTED:
        dwRet = STANDARD_RIGHTS_READ | TOKEN_QUERY | TOKEN_ADJUST_GROUPS;
        break;
    case DWORD_ACCESSMASK_DESIRED:
        dwRet = STANDARD_RIGHTS_READ | TOKEN_QUERY;
        break;
    case DWORD_VMEM_COMMIT_FLAG:
        dwRet = MEM_COMMIT;
        break;
    case DWORD_VMEM_RESERVE_FLAG:
        dwRet = MEM_RESERVE;
        break;
    case DWORD_VMEM_READWRITE_FLAG:
        dwRet = PAGE_READWRITE;
        break;
    case DWORD_INVALID_VMEM_PROT:
        dwRet = (DWORD)~(PAGE_NOACCESS|PAGE_READONLY|PAGE_READWRITE);
        break;
    case DWORD_VMEM_DECOMMIT_FLAG:
        dwRet = MEM_DECOMMIT;
        break;
    case DWORD_VMEM_RELEASE_FLAG:
        dwRet = MEM_RELEASE;
        break;
    case DWORD_VMEM_INVAL_FREETYPE:
        dwRet = (DWORD)~(MEM_DECOMMIT|MEM_RELEASE);
        break;
    case DWORD_VMEM_INFOSIZE:
        dwRet = sizeof(MEMORY_BASIC_INFORMATION);
        break;
    case DWORD_HEAP_SERIALIZE_FLAG:
        dwRet = HEAP_NO_SERIALIZE;
        break;
    case DWORD_HEAP_BADSERFLAG:
        dwRet = (DWORD)~HEAP_NO_SERIALIZE;
        break;
    case DWORD_VMEM_BADALLOCFLAG:
        dwRet = (DWORD)~(MEM_COMMIT | MEM_RESERVE);
        break;
        
    case DWORD_PROCESS_ALL_ACCESS:
        dwRet = PROCESS_ALL_ACCESS;
        break;
    case DWORD_PROCESS_DUP_HANDLE:
        dwRet = PROCESS_DUP_HANDLE;
        break;
    case DWORD_PROCESS_QUERY_INFORMATION:
        dwRet = PROCESS_QUERY_INFORMATION;
        break;
    case DWORD_PROCESS_SET_INFORMATION:
        dwRet = PROCESS_SET_INFORMATION;
        break;
    case DWORD_PROCESS_SYNCHRONIZE:
    case DWORD_THREAD_SYNCHRONIZE:
        dwRet = SYNCHRONIZE;
        break;
    case DWORD_PROCESS_VM_READ:
        dwRet = PROCESS_VM_READ;
        break;
    case DWORD_PROCESS_VM_WRITE:
        dwRet = PROCESS_VM_WRITE;
        break;
    case DWORD_PROCESS_BAD_ACCESS:
        dwRet = ~(PROCESS_ALL_ACCESS|SYNCHRONIZE|ACCESS_SYSTEM_SECURITY|
            GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE|GENERIC_ALL|
            MAXIMUM_ALLOWED);
        break;
    case DWORD_THREAD_ALL_ACCESS:
        dwRet = THREAD_ALL_ACCESS;
        break;
    case DWORD_THREAD_GET_CONTEXT:
        dwRet = THREAD_GET_CONTEXT;
        break;
    case DWORD_THREAD_QUERY_INFORMATION:
        dwRet = THREAD_QUERY_INFORMATION;
        break;
    case DWORD_THREAD_SET_INFORMATION:
        dwRet = THREAD_SET_INFORMATION;
        break;
    case DWORD_THREAD_SET_CONTEXT:
        dwRet = THREAD_SET_CONTEXT;
        break;
    case DWORD_THREAD_SUSPEND_RESUME:
        dwRet = THREAD_SUSPEND_RESUME;
        break;
    case DWORD_THREAD_BAD_ACCESS:
        dwRet = ~(THREAD_ALL_ACCESS|SYNCHRONIZE|ACCESS_SYSTEM_SECURITY|
            GENERIC_READ|GENERIC_WRITE|GENERIC_EXECUTE|GENERIC_ALL|
            MAXIMUM_ALLOWED);
        break;
    case DWORD_TOKEN_PRIV_LENGTH:
        dwRet = sizeof(TOKEN_PRIVILEGES);
        break;
    case DWORD_TOKEN_GROUPS_LENGTH:
        dwRet = sizeof(TOKEN_GROUPS);
        break;
    case DWORD_GMEM_MODIFY_FLAG:
        dwRet = GMEM_MODIFY;
        break;
    case DWORD_LMEM_MODIFY_FLAG:
        dwRet = LMEM_MODIFY;
        break;
    case DWORD_NORMAL_PRIORITY_CLASS:
        dwRet = 0x20;
        break;
    case DWORD_IDLE_PRIORITY_CLASS:
        dwRet = 0x40;
        break;
    case DWORD_HIGH_PRIORITY_CLASS:
        dwRet = 0x80;
        break;
    case DWORD_CREATE_SUSPENDED:
        dwRet = CREATE_SUSPENDED;
        break;
    case DWORD_CREATE_NEW_CONSOLE:
        dwRet = 0x10;
        break;
    case DWORD_CREATE_DETACHED_PROCESS:
        dwRet = 0x8;
        break;
    case DWORD_CREATE_DETACHED_AND_NEW_CONSOLE:
        dwRet = 0x18;
        break;
    case DWORD_CREATE_MULTIPLE_PRIORITY_CLASSES:
        dwRet = 0xe0;
        break;
    case DWORD_CURRENT_PROCESS_ID:
        dwRet = 8;
        break;
    case DWORD_NORMAL_THREAD_FLAGS:
        dwRet = 0;
        break;
    case DWORD_THREAD_CREATE_SUSPENDED:
        dwRet = CREATE_SUSPENDED;
        break;
    case DWORD_VALID_TLS_SLOT:
        dwRet = TlsAlloc();
        if (dwRet==(-1L))
            ErrorPrint(hConOut,hLog,"DWORDSetup",CaseNo,"TlsAlloc failed" );
        *SCInfo = (LPVOID) dwRet;
        break;
    case DWORD_FREED_TLS_SLOT:
        dwRet = TlsAlloc();
        if (dwRet==(-1L))
            ErrorPrint(hConOut,hLog,"DWORDSetup",CaseNo,"TlsAlloc failed" );
        if (!TlsFree(dwRet))
            ErrorPrint(hConOut,hLog,"DWORDSetup",CaseNo,"TlsFree failed" );
        break;
    case DWORD_GENERIC_READ_AND_WRITE:
    case DWORD_GENERIC_INVALID_VALUE:
    case DWORD_FILE_SHARE_READ_AND_WRITE:
    case DWORD_FILE_SHARE_READ_WRITE_DELETE:
    case DWORD_FILE_SHARE_INVALID_VALUE:
    case DWORD_DISPOSITION_CREATE_FILE_ALWAYS:
    case DWORD_DISPOSITION_INVALID_VALUE:
    case DWORD_FILE_FLAGS_NORMAL:
    case DWORD_FILE_FLAGS_INVALID_VALUE:
    case DWORD_READWRITE_VALID_BUFFER_SIZE:
    case DWORD_READWRITE_INVALID_BUFFER_SIZE:
    case DWORD_VOLUME_NAME_BUFFER_LENGTH:
    case DWORD_FILE_SYSTEM_NAME_BUFFER_LENGTH:
    case DWORD_NORMAL_FILE_ATTRIBUTES:
    case DWORD_INVALID_FILE_ATTRIBUTES:
    case DWORD_VER_INVALID_FIND_FLAGS:
    case DWORD_VER_INVALID_INST_FLAGS:
    case DWORD_VER_VALID_INFO_LENGTH:
    case DWORD_VER_LANG_US_ENGLISH:
    case DWORD_VER_LANG_UNDEFINED:
    case DWORD_VER_LANGUAGE_BUFFER_SIZE:
        dwRet = FIODwordSetup(CaseNo, SCInfo, hLog, hConOut);
        break;
    case DWORD_VALID_PATH_BUFFER_LENGTH:
        dwRet = MAX_PATH;
        break;
    case DWORD_MOVE_METHOD_FILE_BEGIN:
        dwRet = FILE_BEGIN;
        break;
    case DWORD_MOVE_METHOD_INVALID_VALUE:
        dwRet = (DWORD)~( FILE_BEGIN | FILE_CURRENT | FILE_END );
        break;
    case DWORD_LOGICAL_DRIVE_STRING_BUFFER_LENGTH:
        dwRet = FIO_LOGICAL_DRIVE_STRING_BUFFER_LENGTH;
        break;
    case DWORD_MAX:
        dwRet = 0xffffffff;
        break;
    case DWORD_VALID_CHANGE_NOTIFY_FILTER:
        dwRet = FILE_NOTIFY_CHANGE_FILE_NAME;
        break;
    case DWORD_INVALID_CHANGE_NOTIFY_FILTER:
        dwRet = (DWORD)~ ( FILE_NOTIFY_CHANGE_FILE_NAME  |
            FILE_NOTIFY_CHANGE_DIR_NAME   |
            FILE_NOTIFY_CHANGE_ATTRIBUTES |
            FILE_NOTIFY_CHANGE_SIZE       |
            FILE_NOTIFY_CHANGE_LAST_WRITE |
            FILE_NOTIFY_CHANGE_SECURITY );
        break;
        
    case DWORD_12:
        dwRet = 12;
        break;
    case DWORD_13:
        dwRet = 13;
        break;
    case DWORD_16:
        dwRet = 16;
        break;
    case DWORD_50:
        dwRet = 50;
        break;
    case DWORD_64K:
        dwRet = 65535;
        break;
    case DWORD_65537:
        dwRet = 65537;
        break;
    case DWORD_EIGHT:
        dwRet = 8;
        break;
    case DWORD_FOUR:
        dwRet = 4;
        break;
    case DWORD_GENERIC_ALL:
        dwRet = GENERIC_ALL;
        break;
    case DWORD_GENERIC_EXECUTE:
        dwRet = GENERIC_EXECUTE;
        break;
    case DWORD_GENERIC_READ:
        dwRet = GENERIC_READ;
        break;
    case DWORD_GENERIC_WRITE:
        dwRet = GENERIC_WRITE;
        break;
    case DWORD_MIN_SD_LENGTH:
        dwRet = SECURITY_DESCRIPTOR_MIN_LENGTH;
        break;
    case DWORD_RANDOM:
        //
        // rand_max is 0x7FFF or 32k, so add two of them together and shift
        // them high, then add two more to cover the low end.  This should
        // provide a range of nearly every possible 32 bit number (DWORD).
        //
        dwRet = ((rand() + rand()) << 16) | (rand() + rand());
        break;
    case DWORD_READ_CONTROL:
        dwRet = READ_CONTROL;
        break;
    case DWORD_SYNCHRONIZE:
        dwRet = SYNCHRONIZE;
        break;
    case DWORD_TWO:
        dwRet = 2;
        break;

#if 0
    case DWORD_D3DEXECUTE_CLIPPED:
        dwRet = D3DEXECUTE_CLIPPED;
        break;
    case DWORD_D3DEXECUTE_UNCLIPPED:
        dwRet = D3DEXECUTE_UNCLIPPED;
        break;
    case DWORD_D3DEXECUTE_BOTH:
        dwRet = (D3DEXECUTE_CLIPPED | D3DEXECUTE_UNCLIPPED);
        break;
#endif 0

    case DWORD_D3DEXECUTE_NONE:
        dwRet = 0;
        break;
        
    case DWORD_THREADID:
        dwRet = GetCurrentThreadId();
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"DWORDSetup",CaseNo,"Unknown Case");
        dwRet = 0;
        break;
    }
  
    return dwRet;
}

void DWORDCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case DWORD_VALID_TLS_SLOT:
        if (!TlsFree((DWORD)(*SCInfo)))
            ErrorPrint(hConOut,hLog,"DWORDCleanup",CaseNo,"TlsFree failed" );
        break;
    default:
        break;
    }
}



/******************************
L     PPPP  DDD    CCC  BBBB
L     P   P D  D  C   C B   B
L     PPPP  D   D C     BBBB
L     P     D   D C   C B   B
LLLLL P     DDDD   CCC  BBBB
******************************/

PVOID LPDCBSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    LPDCB Ret;

    UNREFERENCED_PARAMETER( SCInfo );

    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"LPDCBSetup",CaseNo,"Unknown Case");
        Ret = (LPDCB) NULL;
        break;
    }
    
    return(Ret);
#endif
    return 0;
}


void LPDCBCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}



/******************************************************************************
L     PPPP  DDD   EEEEE BBBB  U   U  GGGG       EEEEE V   V EEEEE N   N TTTTT
L     P   P D  D  E     B   B U   U G           E     V   V E     NN  N   T
L     PPPP  D   D EEEE  BBBB  U   U G  GG       EEEE  V   V EEEE  N N N   T
L     P     D   D E     B   B U   U G   G       E      V V  E     N  NN   T
LLLLL P     DDDD  EEEEE BBBB   UUU   GGG  _____ EEEEE   V   EEEEE N   N   T
******************************************************************************/

PVOID LPDEBUG_EVENTSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
#if 0
    LPDEBUG_EVENT Ret;
    
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );

    switch(CaseNo) {
        
    default:
        ErrorPrint(hConOut,hLog,"LPDEBUG_EVENTSetup",CaseNo,"Unknown Case");
        Ret = (LPDEBUG_EVENT) NULL;
        break;
    }
    
    return(Ret);
#endif
    return 0;
}


void LPDEBUG_EVENTCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}

/******************************************
L     PPPP  DDD   W   W  OOO  RRRR  DDD
L     P   P D  D  W   W O   O R   R D  D
L     PPPP  D   D W W W O   O RRRR  D   D
L     P     D   D WW WW O   O R  R  D   D
LLLLL P     DDDD  W   W  OOO  R   R DDDD
******************************************/

LPDWORD LPDWORDSetup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    LPDWORD Ret, lpPtr;
    DWORD dwBlank;
    
    switch(CaseNo) {
    case LPDWORD_BUFFER:
        Ret = (LPDWORD) LocalAlloc(LPTR, sizeof(DWORD));
        if(!Ret) {
            ErrorPrint(hConOut,hLog,"LPDWORDSetup",CaseNo,
                "insufficient memory");
            break;
        }
        *SCInfo = Ret;
        break;
        
    case LPDWORD_NULL:
        Ret = (LPDWORD)NULL;
        break;
    case LPDWORD_MINUS_ONE:
        Ret = (LPDWORD)(-1L);
        break;
    case LPDWORD_RANDOM_POINTER:
        Ret = (LPDWORD)rand();
        break;
    case LPDWORD_DWORD_SIZED_BLOCK:
        Ret = &dwBlank;
        break;
    case LPDWORD_VALID_ADDRESS:
        if ( (Ret = malloc(sizeof(DWORD))) == NULL )
            ErrorPrint(hConOut,hLog,"LPDWORDSetup",CaseNo,
            "Error allocating memory");
        *SCInfo = (LPVOID) Ret;
        break;
        
    case LPDWORD_454:
        Ret = (LPDWORD) LocalAlloc(LPTR, 3*sizeof(DWORD));
        if(!Ret) {
            ErrorPrint(hConOut,hLog,"LPDWORDSetup",CaseNo,
                "LocalAlloc Failed");
            break;
        } else {
            lpPtr = Ret;
            *lpPtr++ = 4;
            *lpPtr++ = 5;
            *lpPtr++ = 4;
        }
        *SCInfo = Ret;
        break;
        
    case LPDWORD_64K:
    /*
    * points to 65536...almost
    *
    * I just arbitrarly decided this should be 64k; but because of a bug
    * reduced it to 64k-2
        */
        
        if (NULL != (Ret = (LPDWORD)GlobalAlloc(GPTR, sizeof(DWORD)))) {
            ErrorPrint(hConOut, hLog, "LPDWORDSetup", CaseNo,
                "GlobalAlloc failed");
        } else {
            *Ret = 65534;
            *SCInfo = Ret;
        }
        
        break;
        
    case LPDWORD_TWO:
    /*
    * points to some value > 0, and < min(number of butes needed to hold
    * the shortest privilege name, shortest ASCII privilege display name)
        */
        
        if (NULL != (Ret = (LPDWORD)GlobalAlloc(GPTR, sizeof(DWORD)))) {
            ErrorPrint(hConOut, hLog, "LPDWORDSetup", CaseNo,
                "GlobalAlloc failed");
        } else {
            *Ret = 2;
            *SCInfo = Ret;
        }
        
        break;
        
    case LPDWORD_ZERO:
    /*
    * points to a 0
        */
        
        if (NULL != (Ret = (LPDWORD)GlobalAlloc(GPTR, sizeof(DWORD)))) {
            ErrorPrint(hConOut, hLog, "LPDWORDSetup", CaseNo,
                "GlobalAlloc failed");
        } else {
            *Ret = 0;
            *SCInfo = Ret;
        }
        
        break;
        
        /*
        * Several of these cases allocate memory only in this pass, and then in
        * second pass set the value at the memory location...  Watch
        * carefully...
        */
        
    case LPDWORD_INVALID: {
    /*
    * an invalid pointer
        */
        
        SE_CLEANUP_INFO      *CleanupInfo = NULL;
        
        if (!MakeBadPointer((PVOID *)&Ret,
            &CleanupInfo,
            INVALID_POINTER,
            0,
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPDWORDSetup", CaseNo,
                "MakeBadPointer failed");
        }
        
        *SCInfo = CleanupInfo;
        
        break;
                          }
        
    case LPDWORD_10:
    case LPDWORD_50:
    case LPDWORD_65537:
    case LPDWORD_FOUR: {
        SE_CLEANUP_INFO      *CleanupInfo = NULL;
        
        if (!MakeBadPointer((PVOID *)&Ret,
            &CleanupInfo,
            VALIDRANGE_POINTER,
            sizeof (DWORD),
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPDWORDSetup", CaseNo,
                "MakeBadPointer failed");
        }
        
        *SCInfo = CleanupInfo;
        
        break;
                                          }
        
    case LPDWORD_MALALIGNED: {
        SE_CLEANUP_INFO      *CleanupInfo = NULL;
        
        if (!MakeBadPointer((PVOID *)&Ret,
            &CleanupInfo,
            MALALIGNED_POINTER_1,
            sizeof(DWORD),
            hConOut,
            hLog)) {
            ErrorPrint(hConOut, hLog, "LPDWORDSetup", CaseNo,
                "MakeBadPointer failed");
        }
        
        *SCInfo = CleanupInfo;
        
        break;
                             }
        
    default:
        ErrorPrint(hConOut,hLog,"LPDWORDSetup",CaseNo,"Unknown Case");
        Ret = (LPDWORD)NULL;
        break;
    }
    
    /*
    * some of these take a second part:
    */
    
    try {
        switch (CaseNo) {
        case LPDWORD_10:
            
            *Ret = 10;
            
            break;
            
        case LPDWORD_50:
            
            *Ret = 50;
            
            break;
            
        case LPDWORD_65537:
            
            *Ret = 65537;
            
            break;
            
        case LPDWORD_FOUR:
            
            *Ret = 4;
            
            break;
            
        default:
            
        /*
        * Nothing happens here, whatever may reach here was completely set up
        * in the first part.
            */
            
            break;
        }
    } except (EXCEPTION_EXECUTE_HANDLER) {
    /*
    * just go silently along
        */
    }
    
    return(Ret);
}


void LPDWORDCleanup(int CaseNo,LPVOID *SCInfo,HANDLE hLog, HANDLE hConOut)
{
    switch(CaseNo) {
    case LPDWORD_64K:
    case LPDWORD_TWO:
    case LPDWORD_ZERO:
        if(*SCInfo) {
            LocalFree(*SCInfo);
        }
        break;
        
    case LPDWORD_VALID_ADDRESS:
        if ( *SCInfo != NULL )
            free(*SCInfo);
        break;
        
    case LPDWORD_INVALID:
    case LPDWORD_10:
    case LPDWORD_50:
    case LPDWORD_65537:
    case LPDWORD_FOUR:
    case LPDWORD_MALALIGNED:
        if (*SCInfo) {
        /*
        * Make the cleanup call
            */
            
            MakeBadPointer(NULL,
                (SE_CLEANUP_INFO **)SCInfo,
                CLEANUP,
                0,
                hConOut,
                hLog);
            
            if (*SCInfo) {
                ErrorPrint(hConOut, hLog, "LPDWORDCleanup", CaseNo,
                    "Cleanup failed.");
            }
        }
        
        break;
        
    default:
        break;
    }
}
