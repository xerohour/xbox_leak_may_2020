/****************************************************************************
MODULE: MemMgt.c
AUTHOR: JohnMil
DATE:	1-19-91

  Copyright (c) 1992 Microsoft Corporation
  
    This module contains functions to make and destroy parameters for BadMan.
    Instead of repeating similar code in the case statements in SetCln.c,
    we hide the nasty details here.
****************************************************************************/

#include <setcln.h>
#include <d_cases.bmh>
#include <h_cases.bmh>
#include <p_cases.bmh>
#include <s_cases.bmh>
#include <v_cases.bmh>
#include <memmgt.h>



/*****************************************************************************
LMemHandleCreate will make a Local Memory handle for HANDLESetup
*****************************************************************************/

HANDLE LMemHandleCreate(HANDLE hLog, HANDLE hConOut, int CaseNo)
{
    HANDLE hRet,hMem;
    LPSTR lpMem;
    DWORD dwFlags = LPTR;
    
    // First, establish flags.
    switch (CaseNo) {
        
    case HANDLE_UNLOCKED_LMEM_BLOCK:
    case HANDLE_LOCKED_LMEM_BLOCK:
        //case HANDLE_DISCARDED_LMEM_BLOCK:
    case HANDLE_NONDISCARDABLE_LMEM_BLOCK:
    case HANDLE_FREED_LMEM_BLOCK:
        // REVIEW: LMEM_MOVEABLE, LMEM_DISCARDABLE is not implemented in xbox
        //dwFlags = LMEM_MOVEABLE | LMEM_DISCARDABLE;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"LMemHandleCreate",CaseNo,"Unknown Case");
    }
    
    // Next, allocate the basic block;
    hRet = LocalAlloc(dwFlags,0x400);
    if (hRet == NULL) {
        ErrorPrint(hConOut,hLog,"LMemHandleCreate",CaseNo,"Unable to allocate Local Block");
    }
    // Perform additional actions if required
    
    switch (CaseNo) {
        
        //  case HANDLE_DISCARDED_LMEM_BLOCK:
        //    hMem = LocalDiscard(hRet);
        //    if (hMem == NULL)
        //	      ErrorPrint(hConOut,hLog,"LMemHandleCreate",CaseNo,"Unable to discard Local Block");
        //    break;
        
    case HANDLE_LOCKED_LMEM_BLOCK:
        lpMem = LocalLock(hRet);
        if (lpMem == NULL) {
            ErrorPrint(hConOut,hLog,"LMemHandleCreate",CaseNo,"Unable to Lock Local Block");
        }
        break;
        
    case HANDLE_FREED_LMEM_BLOCK:
        hMem = LocalFree(hRet);
        if (hMem != NULL) {
            ErrorPrint(hConOut,hLog,"LMemHandleCreate",CaseNo,"Unable to Free Local Block");
        }
        break;
        
    default:
        break;
    }
    
    return hRet;
}




/*****************************************************************************
GMemHandleCreate will make a Global Memory handle for HANDLESetup
*****************************************************************************/

HANDLE GMemHandleCreate(HANDLE hLog, HANDLE hConOut, int CaseNo)
{
    HANDLE hRet,hMem;
    LPSTR lpMem;
    DWORD dwFlags = GPTR;
    
    // First, establish flags.
    switch (CaseNo) {
        
    case HANDLE_UNLOCKED_GMEM_BLOCK:
    case HANDLE_LOCKED_GMEM_BLOCK:
    case HANDLE_DISCARDED_GMEM_BLOCK:
    case HANDLE_FREED_GMEM_BLOCK:
        // REVIEW: GMEM_MOVEABLE is not implemented in xbox
        // dwFlags = GMEM_MOVEABLE | GMEM_DISCARDABLE;
        break;
        
    case HANDLE_NONDISCARDABLE_GMEM_BLOCK:
        // REVIEW: GMEM_MOVEABLE is not implemented in xbox
        // dwFlags = GMEM_MOVEABLE;
        break;
        
    default:
        ErrorPrint(hConOut,hLog,"GMemHandleCreate",CaseNo,"Unknown Case");
    }
    
    // Next, allocate the basic block;
    hRet = GlobalAlloc( dwFlags, 0x400 );
    
    if ( hRet == NULL ) {
        ErrorPrint( hConOut, hLog, "GMemHandleCreate", CaseNo, "Unable to allocate Global Block" );
    }
    
    // Perform additional actions if required
    switch (CaseNo) {
        
    case HANDLE_DISCARDED_GMEM_BLOCK:
        // REVIEW: GMEM_MOVEABLE and GMEM_DISCARDABLE is not implemented in xbox
        //hMem = GlobalDiscard( hRet );
        //if ( hMem == NULL ) {
        //ErrorPrint( hConOut, hLog, "GMemHandleCreate", CaseNo, "Unable to discard Global Block" );
        //}
        break;
        
    case HANDLE_LOCKED_GMEM_BLOCK:
        lpMem = GlobalLock( hRet );
        if ( lpMem == NULL ) {
            ErrorPrint( hConOut, hLog, "GMemHandleCreate", CaseNo, "Unable to Lock Global Block" );
        }
        break;
        
    case HANDLE_FREED_GMEM_BLOCK:
        hMem = GlobalFree( hRet );
        if ( hMem != NULL ) {
            ErrorPrint( hConOut, hLog, "GMemHandleCreate", CaseNo, "Unable to Free Global Block" );
        }
        break;
        
    default:
        break;
    }
    
    return hRet;
}


/*****************************************************************************
HeapHandleCreate makes handles to Heaps.
*****************************************************************************/

HANDLE HeapHandleCreate(HANDLE hLog,HANDLE hConOut,int CaseNo)
{
    HANDLE hRet;
    DWORD dwInitSize;
    DWORD dwGrowSize;
    
    // Grow size and initial size are 32k by default.
    dwInitSize = dwGrowSize = 32*1024;
    if (CaseNo == HANDLE_GROWABLE_HEAP) {
        dwGrowSize = 0;
    }
    // Allocate the heap
    hRet = HeapCreate(HEAP_NO_SERIALIZE,dwInitSize,dwGrowSize);
    if (hRet == NULL) {
        ErrorPrint(hConOut,hLog,"HeapHandleCreate",CaseNo,"Unable to create heap");
        return(hRet);
    }
    // Fill the heap if necessary
    if (CaseNo == HANDLE_UNGROWABLE_FILLED_HEAP) {
        while(HeapAlloc(hRet, 0, 1) != NULL) {
            ; // allocate all space in heap
        }
    }
    
    if (CaseNo == HANDLE_DESTROYED_32K_HEAP) {
        if( !HeapDestroy(hRet) ) {
            ErrorPrint( hConOut, hLog, "HeapHandleCreate", CaseNo, "Unable to destroy heap ()" );
        }
    }
    
    return(hRet);
}



/*****************************************************************************
LMemPointerCreate makes an LPSTR for memory management blocks for
LPSTRSetup.
*****************************************************************************/

LPSTR LMemPointerCreate(HANDLE hLog, HANDLE hConOut,int CaseNo, LPVOID *SCInfo)
{
    LPSTR lpRet;
    HANDLE hMem = NULL;
    
    // First, we need to allocate a block and
    // get a pointer to its base.
    switch(CaseNo) {
    case LPSTR_BASE_VALID_LMEM_BLOCK:
    case LPSTR_INSIDE_VALID_LMEM_BLOCK:
    case LPSTR_INSIDE_FREED_LMEM_BLOCK:
        //  case LPSTR_INSIDE_DISCARDED_LMEM_BLOCK:
        hMem = LocalAlloc(LMEM_FIXED,0x400);
        *SCInfo = (LPVOID) hMem;
        lpRet = LocalLock(hMem);
        if (lpRet == NULL) {
            ErrorPrint(hConOut,hLog,"LMemPointerCreate",CaseNo,"Unable to create memory block");
        }
        else {
            if (CaseNo != LPSTR_BASE_VALID_LMEM_BLOCK) {
                lpRet += 0x100;
            }
        }
        break;
    default:
        ErrorPrint(hConOut,hLog,"LMemPointerCreate",CaseNo,"Unknown Case");
        lpRet = NULL;
        break;
    }
    // Now, free the block or whatever.
    switch(CaseNo) {
    case LPSTR_INSIDE_FREED_LMEM_BLOCK:
        hMem = LocalFree(hMem);
        if (hMem != NULL) {
            ErrorPrint(hConOut,hLog,"LMemPointerCreate",CaseNo,"Unable to Free memory block");
        };
        break;
        //  case LPSTR_INSIDE_DISCARDED_LMEM_BLOCK:
        //    hMem = LocalDiscard(hMem);
        //    if (hMem == NULL)
        //		  ErrorPrint(hConOut,hLog,"LMemPointerCreate",CaseNo,"Unable to Discard memory block");
        //    break;
    default:
        break;
    }
    
    return(lpRet);
}




/*****************************************************************************
GMemPointerCreate makes an LPSTR for memory management blocks for
LPSTRSetup.
*****************************************************************************/

LPSTR GMemPointerCreate(HANDLE hLog, HANDLE hConOut,int CaseNo, LPVOID *SCInfo)
{
    LPSTR lpRet;
    HANDLE hMem = NULL;
    
    // First, we need to allocate a block and
    // get a pointer to its base.
    switch(CaseNo) {
    case LPSTR_BASE_VALID_GMEM_BLOCK:
    case LPSTR_INSIDE_VALID_GMEM_BLOCK:
    case LPSTR_INSIDE_FREED_GMEM_BLOCK:
    case LPSTR_INSIDE_DISCARDED_GMEM_BLOCK:
        hMem = GlobalAlloc(GMEM_FIXED,0x400);
        *SCInfo = (LPVOID) hMem;
        lpRet = GlobalLock(hMem);
        if (lpRet == NULL) {
            ErrorPrint(hConOut,hLog,"GMemPointerCreate",CaseNo,"Unable to create memory block");
        }
        else {
            if (CaseNo != LPSTR_BASE_VALID_GMEM_BLOCK) {
                lpRet += 0x100;
            }
        }
        break;
    default:
        ErrorPrint(hConOut,hLog,"GMemPointerCreate",CaseNo,"Unknown Case");
        lpRet = NULL;
        break;
    }
    // Now, free the block or whatever.
    switch(CaseNo) {
    case LPSTR_INSIDE_FREED_GMEM_BLOCK:
        GlobalUnlock(hMem);
        hMem = GlobalFree(hMem);
        if (hMem != NULL) {
            ErrorPrint(hConOut,hLog,"GMemPointerCreate",CaseNo,"Unable to Free memory block");
        };
        break;
    case LPSTR_INSIDE_DISCARDED_GMEM_BLOCK:
        GlobalUnlock(hMem);
        //      hMem = GlobalDiscard(hMem);
        if (hMem == NULL) {
            ErrorPrint(hConOut,hLog,"GMemPointerCreate",CaseNo,"Unable to Discard memory block");
        };
        break;
    default:
        break;
    }
    
    return(lpRet);
}


/***************************************************************************
HeapPointerCreate will make Pointers to memory locations inside of a heap
***************************************************************************/

LPSTR HeapPointerCreate(HANDLE hLog, HANDLE hConOut,int CaseNo, LPVOID *SCInfo)
{
    LPSTR lpRet;
    HANDLE hHeap;
    
    // Allocate the Heap
    
    hHeap = HeapHandleCreate(hLog,hConOut,HANDLE_GROWABLE_HEAP);
    *SCInfo = (LPVOID) hHeap;
    
    // Set up the pointer.
    
    lpRet = HeapAlloc(hHeap, 0, 100);
    if (lpRet == NULL) {
        ErrorPrint(hConOut,hLog,"HeapPointerCreate",CaseNo,
            "Unable to Allocate from Heap");
    }
    
    switch(CaseNo) {
    case LPSTR_DESTROYED_HEAP_BLOCK:
        if(!HeapDestroy(hHeap)) {
            ErrorPrint(hConOut,hLog,"HeapPointerCreate",CaseNo,
                "Unable to destroy heap");
        }
        break;
    case LPSTR_FREED_HEAP_BLOCK:
        if (! HeapFree(hHeap, 0, lpRet)) {
            ErrorPrint(hConOut,hLog,"HeapPointerCreate",CaseNo,
                "Unable to free memory block");
        }
        break;
    }
    
    return(lpRet);
}



/*****************************************************************************
VirtAddrCreate returns the address to a certain kind of virtual memory.
*****************************************************************************/

LPVOID VirtAddrCreate(HANDLE hLog, HANDLE hConOut,int CaseNo, LPVOID *SCInfo)
{
    LPVOID lpRet = NULL;
    DWORD dwOffset,dwAllocType,dwProtect;
    
    dwOffset = 0;
    
    if (CaseNo == LPVOID_RES_UNCOMMIT_V_ADDR) {
        dwAllocType = MEM_RESERVE;
        dwProtect = PAGE_NOACCESS;
    }
    else {
        dwAllocType = MEM_RESERVE | MEM_COMMIT;
        dwProtect = PAGE_READWRITE;
    }
    
    switch(CaseNo) {
    case LPVOID_NONALIGNED_V_ADDR:
    case LPVOID_LOCKED_V_ADDR:
    case LPVOID_UNLOCKED_V_ADDR:
        dwOffset = 3100;
    case LPVOID_RES_UNCOMMIT_V_ADDR:
    case LPVOID_ALIGNED_V_ADDR:
    case LPVOID_REGION_BEG_V_ADDR:
        *SCInfo = VirtualAlloc(NULL,64*1024,dwAllocType,dwProtect);
        lpRet = (LPVOID) ((DWORD) *SCInfo + dwOffset);
    }
    
    if (*SCInfo == NULL) {
        ErrorPrint(hConOut,hLog,"VirtAddrCreate",CaseNo,
            "Unable to allocate virtual memory");
    }
    
    return(lpRet);
}
