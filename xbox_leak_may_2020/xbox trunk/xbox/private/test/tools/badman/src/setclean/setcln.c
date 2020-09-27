/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    setcln.c

Abstract:

    This file contains miscellaneous files for the use of the setup and
    clean up functions.

Author:

    John Miller (johnmil) 10-Feb-1992

Environment:

    XBox

Revision History:

    01-Apr-2000     schanbai

        Ported to XBox and made the code more readable

--*/


#include <setcln.h>
#include <stdarg.h>
#include "xlog.h"


int
SCPrintf(
    HANDLE hConOut,
    LPSTR Format,
    ...
    )

/*++

Routine Description:

    SCPrintf is a substitute for printf which will print to the debugger

Arguments:

Return Value:

--*/

{
    char    buff[512];
    va_list vaList;
    DWORD   i;

    UNREFERENCED_PARAMETER( hConOut );

    va_start( vaList, Format );
    i = vsprintf( buff, Format, vaList );
    va_end( vaList );
    OutputDebugStringA( buff );
    return i;
}


void
ErrorPrint(
    HANDLE hConOut,
    HANDLE hLog,
    LPSTR Module,
    int CaseNo,
    LPSTR Desc
    )

/*++

Routine Description:

    ErrorPrint is a standard format error printer which will print to the
    debugger and to the log file, if appropriate.

Arguments:

Return Value:

    None

--*/

{
    UNREFERENCED_PARAMETER( hConOut );
    
    DbgPrint( "SETCLN: %s [Case %d] -- %s\n", Module, CaseNo, Desc );
    
    if ( INVALID_HANDLE_VALUE != hLog ) {
        xLog( hLog, XLL_WARN, "ERROR: %s [Case %d] -- %s", Module, CaseNo, Desc );
    }
}

/***************************************************************************

    InfThread( LPVOID lpv)

    Code to implement a "responsible" infinite loop thread to be used
    with tasking variations.

    Added: 03-21-92 RickTu

 ***************************************************************************/


VOID
InfThread(
    VOID
    )
{
    for (;;) {
        SleepEx( 1000, TRUE );
    }
}


/****************************************************************************

   DLLEntry( HANDLE hInst, DWORD dwReason, LPVOID lpv )


   Get an Instance handle for use in MakeProcInstance calls (FARPROCSetup)

   MarkRi Mar 23, 1992

****************************************************************************/

HANDLE hInstance ;


BOOL WINAPI DllMain( HANDLE hInst, DWORD dwReason, LPVOID lpv )
{
    if ( dwReason == DLL_PROCESS_ATTACH ) {
        DisableThreadLibraryCalls( hInst );
    }

    return TRUE ;
    lpv; // -W3 warning elimination
}


/*DebugPrint(LPSTR a, ...)
{
    char    c[256];
    va_list vaList;

    va_start(vaList, a);
    vsprintf(c, a, vaList);
    va_end(vaList);

    OutputDebugStringA(c);
}
*/

/*
 * GetNativeBuffer
 *
 * Part of the support for invalid pointer testing.  Allocate a buffer of at
 * least NumberOfBytes bytes in size, and make sure that the tail end of the
 * buffer is invalid memory.  This depends on a side effect of
 * NtAllocateVirtualMemory:  Allocations are 64k aligned, but only enough
 * pages in that 64k region are mapped to satisfy the memory request.  This
 * routine is thus not likely to work if the request size is between 64k, and
 * 64-4k since 4k is the smallest mappable unit.  Beware.
 *
 * Returns a pointer to the first byte in the region offset such that
 * pointer+NumberOfBytes == invalid memory.
 */

DWORD           PageSize;

PVOID
GetNativeBuffer(
    DWORD           NumberOfBytes
)
{
    static BOOL         Init = FALSE;

    DWORD           RegionSize;
    PCHAR           PoolAddress = NULL;

    if (!Init) {
        //SYSTEM_INFO       SysInfo;

        Init = TRUE;

        //GetSystemInfo(&SysInfo);

        PageSize = 0x1000; //SysInfo.dwPageSize;
    }


// BUGBUG:  May need it a multiple of 8 on 64 bit aligned platforms.

    /*
     * We need to return a DWORD aligned address, round "NumberOfBytes" up
     * to a multiple of 4.
     */

#define ALIGNMENT   (sizeof (DWORD))

    NumberOfBytes += (ALIGNMENT - 1);
    NumberOfBytes &= ~(ALIGNMENT - 1);

    RegionSize = NumberOfBytes;

    PoolAddress = VirtualAlloc(NULL,
        RegionSize,
        MEM_COMMIT,
        PAGE_READWRITE);

    if (!PoolAddress) {
        printf("VirtualAlloc failed, LastError = %d\n",
            GetLastError());

        return NULL;
    } else {
        /*
         * RegionSize is rounded to the next page boundary
         */

        RegionSize += PageSize - 1;
        RegionSize &= ~(PageSize - 1);

        /*
         * This pretty much guarantees that we'll trap if we try to
         * write more than NumberOfBytes + 3 bytes.
         */

//      printf("0x%lx = GetNativeBuffer(0x%lx)\n",
//          PoolAddress,
//          NumberOfBytes);

//printf("(Pa = 0x%lx, Rs = 0x%lx, Nb = 0x$lx\n",
//  PoolAddress, RegionSize, NumberOfBytes);

        return PoolAddress + (RegionSize - NumberOfBytes);
    }
}


/* FreeNativeBuffer
 *
 * Free the buffer allocated by GetNativeBuffer.  You have to pass me the
 * pointer I passed you (Pointer), and the size of allocation you made
 * (NumberOfBytes) because I'm too lazy to store it for you, and besides if
 * you pass it to me again, you can't accidently trash the place where *I*
 * store it.  (Though you are free to trash the place *you* store it...
 */

VOID
FreeNativeBuffer(
    PVOID           Pointer,
    DWORD           NumberOfBytes
)
{
    UNREFERENCED_PARAMETER( NumberOfBytes );

    /*
     * Assuming that GetNativeBuffer will allocate the minimum number of
     * pages to satisfy a request, it is sufficient to simply round down
     * to the nearest page.  If the assumption doesn't hold, there's no
     * way to determine what will bring us to our original address because
     * the region-size is not returned by w32 (though it is from the
     * kernel).
     */

    Pointer = (PVOID)((ULONG_PTR)Pointer & ~(PageSize - 1));

    /*
     * and round "NumberOfBytes" up to a multiple of 4...
     */

//  printf("FreeNativeBuffer(0x%lx, 0x%lx)\n",
//      Pointer,
//      NumberOfBytes);

    (VOID)VirtualFree(Pointer,
        0,
        MEM_RELEASE);
}


BOOL
MakeBadPointer(
    PVOID           *Pointer,
    SE_CLEANUP_INFO **CleanupInfo,
    POINTER_TYPE    PointerType,
    DWORD           PointerSize,
    HANDLE          hConOut,
    HANDLE          hLog
)
{
    BOOL  Result = FALSE;

    UNREFERENCED_PARAMETER( hConOut );
    UNREFERENCED_PARAMETER( hLog );

    switch (PointerType) {
    case NULL_POINTER:
        *Pointer = NULL;

        // should we do this or link into the list as necessary here?
        *CleanupInfo = NULL;

        Result = TRUE;

        break;

    case INVALID_POINTER:
        /*
         * hard-code for now, should fix someday...
         */

        *Pointer = (PVOID)0x80060004;

        // should we do this or link into the list as necessary here?
        *CleanupInfo = NULL;

        Result = TRUE;

        break;

    case VALIDRANGE_POINTER:
        *Pointer = GetNativeBuffer(PointerSize);

        Result = *Pointer ? TRUE : FALSE;

        *CleanupInfo = (SE_CLEANUP_INFO *)GlobalAlloc(GPTR,
            sizeof(SE_CLEANUP_INFO));

        if (!*CleanupInfo) {
            Result = FALSE;

            FreeNativeBuffer(*Pointer, PointerSize);

//          DebugPrint("MakeBadPointer, Type = 0x%x, "
//              "GlobalAlloc for cleanup failed VALIDRANGE\n",
//              PointerType);
        } else {
            if ((DWORD)(*CleanupInfo) & 3) {
//              DebugPrint("GlobalAlloc not aligned!!! VALID\n");
            }

            (*CleanupInfo) -> se_Next = NULL;
            (*CleanupInfo) -> se_AllocType = PointerType;
            (*CleanupInfo) -> se_AllocSize = PointerSize;
            (*CleanupInfo) -> se_Address = *Pointer;
        }

        break;

    case MALALIGNED_POINTER_1:
        *Pointer = GetNativeBuffer((DWORD)PointerSize + sizeof (DWORD));

        Result = *Pointer ? TRUE : FALSE;

        /*
         * Force the returned pointer to be aligned on odd-byte.
         */

        *Pointer = (PVOID *)((DWORD)*Pointer | 1);

        *CleanupInfo = (SE_CLEANUP_INFO *)GlobalAlloc(GPTR,
            sizeof(SE_CLEANUP_INFO));

        if (!*CleanupInfo) {
            Result =FALSE;

            FreeNativeBuffer(*Pointer, PointerSize + 1);

//          DebugPrint("MakeBadPointer GlobalAlloc for cleanup "
//              "failed MALALIGNED_1");
        } else {
            if ((DWORD)(*CleanupInfo) & 3) {
//              DebugPrint("GlobalAlloc not aligned!!! MAL_1\n");
            }

            (*CleanupInfo) -> se_Next = NULL;
            (*CleanupInfo) -> se_AllocType = PointerType;
            (*CleanupInfo) -> se_AllocSize = PointerSize + 1;
            (*CleanupInfo) -> se_Address = *Pointer;
        }

        break;

    case CLEANUP:
        while (*CleanupInfo) {
            FreeNativeBuffer((*CleanupInfo) -> se_Address,
                (*CleanupInfo) -> se_AllocSize);

            (*CleanupInfo) = (*CleanupInfo) -> se_Next;
        }

        break;
    default:
//      DebugPrint("MakeBadPointer:  Bogus case = 0x%lx\n",
//          PointerType);
        ;
    }

    return Result;
}


#ifdef  TIMF_DEBUG
/*
 * 'ware recursive soup if malloc/free changes to Global* calls...
 * MakeBadPointer uses them...
 */

DWORD           MallocCount = 0;
SE_CLEANUP_INFO     *MallocCleanupInfo = NULL;

void  * __cdecl
malloc(
    size_t      size
)
{
    DWORD           *Result = NULL;

    size |= 3;
    ++size;

    /*
     * size is now DWORD aligned, add a DWORD for a place to store the
     * size, and alloc it.
     */

    size += sizeof(DWORD);

    Result = (DWORD *)GetNativeBuffer(size);

    /*
     * store the size and return a pointer to the next DWORD.
     */

    *Result = size;

    return (void *)(&(Result[1]));
}


void    __cdecl
free(
    void            *a
)
{
    DWORD           addr = (DWORD)a,
                Size;

    /*
     * pick out the size of the allocation
     */

    Size = *(DWORD *)(addr - sizeof (DWORD));

    /*
     * round down to the nearest page, then free the region.
     */

    FreeNativeBuffer((PVOID)addr,
        Size);
}

#endif  /* TIMF_DEBUG */


VOID
NothingToCleanup(
    int CaseNo,
    LPVOID *SCInfo,
    HANDLE hLog,
    HANDLE hConOut
    )
{
    UNREFERENCED_PARAMETER( CaseNo );
    UNREFERENCED_PARAMETER( SCInfo );
    UNREFERENCED_PARAMETER( hLog );
    UNREFERENCED_PARAMETER( hConOut );
}
