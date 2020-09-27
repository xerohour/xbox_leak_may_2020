 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/
/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: OS.C                                                              *
*   This is the OS interface module.  All operating system transactions     *
*   pass through these routines.  No other operating system specific code   *
*   or data should exist in the source.                                     *
*                                                                           *
\***************************************************************************/
//
// Another great undocumented feature of windows.  You must including these
// defines to be able to include configmg.h
//
#define CAT_HELPER(x, y)    x##y
#define CAT(x, y)       CAT_HELPER(x, y)
#define MAKE_HEADER(RetType, DecType, Function, Parameters)

#include "basedef.h"
#include "vmm.h"
#include "vmmreg.h"
#include "vpicd.h"
#include "configmg.h"
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <dma.h>
#include <gr.h>
#include <mc.h>
#include <os.h>
#include <vga.h>
#include <tv.h>
#include <dac.h>
#include "nvrmwin.h"
#include "nvhw.h"
#include "vnvrmd.h"
#include "oswin.h"
#include "vdd.h"
#include "vmmtypes.h"
#include "vmm2.h"       // more vmm services
#include "nvwin32.h"
#include "nvos.h"
#include "vesadata.h"
#include "nvcm.h"
#include "edid.h"

#ifdef DEBUG
#include <vcomm.h>
#include "osdbg.h"
#include "serial.h"
#endif

#ifdef DEBUG
U032 thread_watcher;
#endif

//
// OS names.
//
char nameVideoSink[]        = "CON:";
char nameImageVideo[]       = "WND";
char nameLeft[]             = "\\LEFT";
char nameRight[]            = "\\RIGHT";
char nameSysMem[]           = "SYSMEM:";
char nameLVidMem[]          = "LOCALVIDMEM:";
char nameSVidMem[]          = "NONLOCALVIDMEM:";

//
// Define registry keys for timing parameters.
//
#ifdef OBSOLETE_FUNCTIONS
char strTimingParm0[]    = "TimingParm0";
char strTimingParm1[]    = "TimingParm1";
char strTimingChecksum[] = "TimingChecksum";
char strPBUSOverride[]   = "PBUSOverride";
char strUnderscanXOverride[]  = "UnderscanX";
char strUnderscanYOverride[]  = "UnderscanY";
#endif // OBSOLETE_FUNCTIONS
char strRefreshOverride[] = "RefreshOverride";
char strTilingOverride[] = "TilingOverride";
char strMemoryOverride[] = "MemoryOverride";
char strRTLOverride[]    = "RTLOverride";
char strFetchTriggerOverride[] = "FetchTrigger";
char strFetchSizeOverride[]    = "FetchSize";
char strFetchMaxReqsOverride[] = "FetchMaxReqs";
char strGraphicsDebug0[]        = "GRDEBUG0";
char strGraphicsDebug1[]        = "GRDEBUG1";
char strGraphicsDebug2[]        = "GRDEBUG2";
char strGraphicsDebug3[]        = "GRDEBUG3";

extern char strDevNodeRM[];
extern char strMaxRefreshRate[];
extern char strMaxOptimalRefreshRate[];
extern char strOptimalNotFromEDID[];
extern char strDMTOverride[];
extern char strMonitorTiming[];
extern char strCursorCacheOverride[];
extern char strDisplayType[];
extern char strDisplay2Type[];
extern char strTVtype[];
extern char strStartUp[];
extern char strTVOutType[];
extern char strNewModeSet[];
extern char strFpMode[];


#ifdef ON_NOW
extern U008 PowerOff;
#endif
//
// Service stack.
//
U032 *rmStack;
U032 *rmStackBase;
U032 vmmStack;
U032 tempStack;
//
// Hack
//
static U008 NopMask;


#ifdef DEBUG_TIMING

#define DEBUG_FRAMES 16

U032 dbgFrame ;
U032 dbgStart ;
U032 dbgStartInt[DEBUG_FRAMES][20];
U032 dbgEndInt[DEBUG_FRAMES][20];
U032 dbgCountInt[DEBUG_FRAMES];
#endif // DEBUG_TIMING

//
// Some quick and dirty library functions.
// This is an OS function because some operating systems supply their
// own version of this function that they require you to use instead
// of the C library function.  And some OS code developers may decide to
// use the actual C library function instead of this code.  In this case,
// just replace the code within osStringCopy with a call to the C library
// function strcpy.
//
char *osStringCopy
(
    char *dst,
    const char *src
)
{
    char *dstptr = dst;
    
    while (*dstptr++ = *src++);
    return (dst);
}

U032 osStringLength(const char * str)
{
    U032 i = 0;
    while (str[i++] != '\0');
    return i - 1;
}

S032 osStringCompare
(
    const char *s1,
    const char *s2
)
{
    while (*s1 && *s2)
    {
        if (*s1++ != *s2++)
            return (0);
    }
    return (!(*s1 || *s2));
}

/*****************************************************************************
 * fastish memcopy code - begin
 *
 *  osMemCopy 
 *   - optimal performance will have src and dst 4-byte aligned
 *   - the compiler does not even TRY to optimize this code... bad switch?
 */
void __declspec(naked) __stdcall __osMemCopy4
(
    U032 dst,
    U032 src,
    U032 len4   // u032 count
)
{
/*
    while (len4)
    {
        *(U032*)dst = *(U032*)src;
        src  += 4;
        dst  += 4;
        len4 -= 4;
    }
*/
    __asm
    {
        push esi
        push edi

        mov ecx,[esp + 20]
        mov esi,[esp + 16]
        shr ecx,2
        mov edi,[esp + 12]

        rep movsd

        pop edi
        pop esi
        ret 12
    }
}

void __declspec(naked) __stdcall  __osMemCopy1
(
    U032 dst,
    U032 src,
    U032 len    // u008 count
)
{
/*
    while (len)
    {
        *(U008*)dst = *(U008*)src;
        src ++;
        dst ++;
        len --;
    }
*/
    __asm
    {
        push esi
        push edi

        mov ecx,[esp + 20]
        mov esi,[esp + 16]
        mov edi,[esp + 12]

        rep movsb

        pop edi
        pop esi
        ret 12
    }
}

unsigned char * osMemCopy
(
          unsigned char *dst,
    const unsigned char *src,
          U032           len    // u008 count
)
{
    if (len >= 16)
    {
        __osMemCopy4 ((U032)dst,(U032)src,len & ~3);
        src += len & ~3;
        dst += len & ~3;
        __osMemCopy1 ((U032)dst,(U032)src,len &  3);
    }
    else
    {
        __osMemCopy1 ((U032)dst,(U032)src,len);
    }

    return dst;
}
/*
 * fastish memcopy code - end
 *****************************************************************************/

RM_STATUS osReleaseFifoSemaphore()
{
    return RM_OK;
}

RM_STATUS osRequestFifoSemaphore()
{
    return RM_OK;
}

//---------------------------------------------------------------------------
//
//  Operating System Memory functions.
//
//---------------------------------------------------------------------------

#ifdef OBSOLETE_FUNCTIONS
U032 osSetContext
(
    U032 ProcContext
)
{
    U032 CurrContext;

    CurrContext = (U032)vmmGetCurrentContext();
    if ((ProcContext) && (CurrContext != ProcContext))
    {
        DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "osSetContext: Current Context  = ",CurrContext);
        DBG_PRINT_STRING_VALUE (DEBUGLEVEL_TRACEINFO, "              Process Context  = ",ProcContext);
        return ((U032)vmmContextSwitch((DWORD) ProcContext));
    }
    //
    // No context switch required.
    //    
    return (0);
}
RM_STATUS osResetContext
(
    U032 OriginalContext
)
{
    if (OriginalContext)
        vmmContextSwitch((DWORD) OriginalContext);
    return (RM_OK);
}
#endif // OBSOLETE_FUNCTIONS

RM_STATUS  osCopyIn
(
	U008 *src, 
	U008 *target,
	U032 count
)
{
	//*target = src;

	osMemCopy(target, src, count);

	return(RM_OK);
}

RM_STATUS  osCopyOut
(
	U008 *src,
	U008 *target,
	U032 count
)
{
	osMemCopy(target, src, count);

	return(RM_OK);
}

RM_STATUS osAllocMem
(
    VOID **pAddress,
    U032   Size
)
{
    RM_STATUS status = RM_OK;

#ifdef DEBUG
    U008     *pMem;
#endif // DEBUG

#ifndef DEBUG
//    *pAddress = vmmHeapAllocate(Size + 4, 0);
    *pAddress = _HeapAllocate(Size + 4, 0);
    if (*pAddress != NULL)
    {
        **(U032 **)pAddress           = Size;
        *(U032 *)pAddress            += 4;
#else
    Size += 12;
//    *pAddress = vmmHeapAllocate(Size, 0);
    *pAddress = _HeapAllocate(Size + 4, 0);
    if (*pAddress != NULL)
    {
        **(U032 **)pAddress                     = Size;
        *(U032 *)(*(U032 *)pAddress + 4)        = NV_MARKER1;
        *(U032 *)(*(U032 *)pAddress + Size - 4) = NV_MARKER2;
        *(U032 *)pAddress += 8;
        pMem = (U008 *)*pAddress;
        Size -= 12;
        while (Size--)
            *pMem++ = 0x69;
#endif // DEBUG
    }
    else
    {
        status = RM_ERR_NO_FREE_MEM;
    }
    return (status);
}
RM_STATUS osFreeMem
(
    VOID *pAddress
)
{
    RM_STATUS status;
    U032      Size;
    
    if(!pAddress)
    {
#ifdef DEBUG
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Error: Null pointer called to osFreeMem.\n\r");
#endif // DEBUG    
        return (RM_OK);
    }    
    
#ifndef DEBUG
    pAddress = (VOID *)((U032)pAddress - 4);
    Size     = *(U032 *)pAddress;
#else
    pAddress = (VOID *)((U032)pAddress - 8);
    Size     = *(U032 *)pAddress;
    if (*(U032 *)((U032)pAddress + 4) != NV_MARKER1)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Invalid address to osFreeMem.\n\r");
        DBG_BREAKPOINT();
    }
    if (*(U032 *)((U032)pAddress + Size - 4) != NV_MARKER2)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Memory overrun in structure to osFreeMem.\n\r");
        DBG_BREAKPOINT();
        return (RM_ERR_MEM_NOT_FREED);
    }
    *(U032 *)((U032)pAddress + 4)        = 'DAED';
    *(U032 *)((U032)pAddress + Size - 4) = 'DEAD';
    Size -= 12;
#endif // DEBUG    
//    if (vmmHeapFree((BYTE *)pAddress, 0) == 0)
    if (_HeapFree((BYTE *)pAddress, 0) == 0)
      {
        status = RM_ERR_MEM_NOT_FREED;
    }
    else
    {
        status = RM_OK;
    }
    return (status);
}

#define CPUID   __asm _emit 0x0F __asm _emit 0xA2
#define RDMSR   __asm _emit 0x0F __asm _emit 0x32

#define AGP_APERTURE_SIZE               0x2000000       // 32 MB

//
// osUpdateAGPLimit - determine the write-combined AGP limit
//
// This routine is now called not only at boot time, but anytime a client
// does a ConfigGet for the AGP limit, in case it's changed.
//
RM_STATUS osUpdateAGPLimit
(
    PHWINFO pDev
)
{
	PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;

    U032 AGPBase, AGPLimit;
    U032 i, mtrrAddr, numRanges = 0;
    U032 BaseHi, BaseLo, MaskHi, MaskLo;
    RM_STATUS status;
        
    // If the aperture limit/size was established at boot time, just return
    if (pRmInfo->AGP.AGPLimit != 0)
        return RM_OK;

    CLI
    // Get the AGP base and limit from the chipset
    status = NvGetAGPBaseLimit(pDev, &AGPBase, &AGPLimit);
    STI

    if (status != RM_OK)
    {
        pRmInfo->AGP.AGPLimit = AGP_APERTURE_SIZE - 1;     // set to 32MB, by default
        return status;
    }

    // First, check if this processor supports the CPUID inst and MTRRs.
    __asm
    {
        ; save state
        push	eax
        push	ebx
        push	ecx
        push	edx

        pushfd
        pop     eax
        mov     ebx, eax
        xor     eax, 0x00200000
        push    eax
        popfd
        pushfd
        pop     eax
        cmp     eax, ebx
        je      Exit                // CPUID inst not supported

        mov     eax, 1
        CPUID
        and     edx, 0x1000         // check for MTRR support
        jz      Exit

        mov     ecx, 254            // read the MTRRcap MSR to determine number of ranges.
        RDMSR
        test    eax, 0x400          // check that WC as a type is supported (should be 1).
        jz      Exit

        and     eax, 0xFF           // save number of ranges
        mov     numRanges, eax
Exit:    
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }

    if (!numRanges)
    {
        pRmInfo->AGP.AGPLimit = AGPLimit;   // default to the aperture limit
        return RM_ERROR;
    }

    //
    // Now, determine if we find a Write Combined match in the MTRRs. We want
    // the AGP limit to only reflect the WC portion of the AGP aperture.
    //
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: Valid MTRRs:\n");
    for (i = 0, mtrrAddr = 512; i < numRanges; i++, mtrrAddr += 2)
    {
        __asm {

            push    eax
            push    ebx
            push    ecx
            push    edx

            mov     ecx, mtrrAddr
            RDMSR

            mov     BaseHi, edx
            mov     BaseLo, eax

            mov     ecx, mtrrAddr
            add     ecx, 1
            RDMSR

            mov     MaskHi, edx
            mov     MaskLo, eax

            pop     edx
            pop     ecx
            pop     ebx
            pop     eax
        }

        if (!(MaskLo & 0x800))
            continue;               // not a valid range

        MaskLo &= ~0xFFF;           // mask off the valid bit

        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Range ", i);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:      Base ", BaseLo);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM:    Length ", ~MaskLo + 1);

        // We only care about a Write-Combined range
        if ((BaseLo & 0xFF) != 0x1)
            continue;

        BaseLo &= ~0xFF;           // mask off the type bits

        if ((BaseLo >= AGPBase) && ((BaseLo + ~MaskLo) <= (AGPBase + AGPLimit)))
            pRmInfo->AGP.AGPLimit = ~MaskLo;  // the WC MTRR is within the aperture
    }

    return RM_OK;
}

//
// osAllocPages - Allocate a number of aligned memory pages
//  
//  **pAddress - handle to the linear address
//  PageCount  - number of pages to allocate
//  MemoryType - memory pool
//  Contiguous - contiguous pages?
//  Cache      - cacheing request
//  clientClass- is this a kernel/user allocation??
//  **pMemData - any per-allocation data (useful during a free)
//
RM_STATUS osAllocPages
(
    PHWINFO pDev,
    VOID    **pAddress,
    U032    PageCount,
    U032    MemoryType,
    U032    Contiguous,
    U032    Cache,
    U032    clientClass,
    VOID    **pMemData
)
{
	PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;

    RM_STATUS       status;
    DESCRIPTOR      desc;

    status = RM_OK;
    *pMemData = NULL;   // no associated per-alloc data
    
    switch (MemoryType)
    {
    
        case NV_MEMORY_TYPE_SYSTEM:
        
            //
            // Perform VMM page allocation.  System memory arena, system VM, no alignment
            // restrictions, no min/max phys restrictions, no need for the physical address
            // (no fixed request), and no need for contiguous or fixed flags
            //
            if (Contiguous == NV_MEMORY_NONCONTIGUOUS)
            {            
                vmmGetCritSectionStatus();      // complete delayed releases of critical section
                                                // is this really necessary in Win 95?
            
                vmmPageAllocate(PageCount, PG_SYS, 0, 0, 0, 0, 0, 0, &desc.Desc64);
                *pAddress = (VOID *)desc.Desc32[0];
                
                /* MTRR work
                if (Cache == NV_MEMORY_WRITECOMBINED)
                ...
                */
            }                
            else
            {
                *pAddress = NULL;
                status = RM_ERROR;
            }                
            
            break;
            
        case NV_MEMORY_TYPE_AGP:
            
            if ((pDev->Chip.Bus == BUS_AGP) && pRmInfo->AGP.AGPLimit && pRmInfo->AGP.AGPPhysStart &&
                 (pDev->Chip.ReqAGPRate != 0))
            {
                // Need to verify availability of VM services for AGP
                if (Get_VMM_Version() < 0x40A)
                {
                    *pAddress = NULL;
                    return RM_ERROR;
                }                    
                
                //
                // Perform VGARTD (VMM) AGP page reservation.  This device, no alignment
                // restrictions, custom cacheing
                //
                if (Cache == NV_MEMORY_UNCACHED)                    
                    *pAddress = (VOID *)vmmGARTReserve(pDev->Mapping.hDev, PageCount, 0, 0, PG_UNCACHED);
                else if (Cache == NV_MEMORY_WRITECOMBINED) {
                    *pAddress = (VOID *)vmmGARTReserve(pDev->Mapping.hDev, PageCount, 0, 0, PG_WRITECOMBINED);
                } else {
                    *pAddress = NULL;       // unknown Cache type
                    status = RM_ERROR;
                }

                if (*pAddress)
                {
                    // Perform VGARTD (VMM) AGP page commit.  No page offset, no init value
                    if (!vmmGARTCommit((U032)*pAddress, 0, PageCount, 0, 0))
                    {
                        // oops, that didn't work...
                        status = RM_ERROR;
                        vmmGARTFree((U032)*pAddress);
                    } else {
                        U032 i, *bufp = (U032 *)*pAddress;

                        // let's be considerate and zero the just allocated memory
                        for (i = 0; i < ((PageCount * RM_PAGE_SIZE) / sizeof(U032)); i++)
                            *bufp++ = 0x0;

                        // update our notion of how much persistent AGP has been alloced
                        if (Cache == NV_MEMORY_WRITECOMBINED)
                            pRmInfo->AGP.AGPHeapFree += (PageCount << RM_PAGE_SHIFT);
                    }
                }
                else
                {
                    // Out of memory. For persistent AGP allocs, this can happen when not enough was
                    // taken out of DDraw's pool. Just return error status (*pAddress is already NULL).
                    status = RM_ERR_NO_FREE_MEM;
                }
            }
            else
            {
                *pAddress = NULL;
                status = RM_ERROR;
            }
            
            break;
            
    }
    
    return (status);
}

//
// osFreePages - Free a number of aligned memory pages
//  
//  **pAddress - handle to the linear address
//  MemoryType - memory pool
//
RM_STATUS osFreePages
(
    PHWINFO pDev,
    VOID    **pAddress,
    U032    MemoryType,
    U032    PageCount,
    VOID    *pMemData,
    BOOL    IsWriteCombined
)
{
	PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;
    RM_STATUS   status = RM_OK;
    
    switch (MemoryType)
    {
    
        case NV_MEMORY_TYPE_SYSTEM:
        
            //
            // Perform VMM page free.
            //
            vmmGetCritSectionStatus();
            //
            // KJK: Not really necessary to unlock before free
            //
            //PageCount = vmmPageGetSizeAddr(*pAddress, 0, &desc.Desc64);
            //vmmPageUnLock(*pAddress,
            //              desc.Desc32[0],
            //              0,
            //              0);
            if (!vmmPageFree((U032)*pAddress, 0))
                status = RM_ERROR;
            
            break;
            
        case NV_MEMORY_TYPE_AGP:

            if ((pDev->Chip.Bus == BUS_AGP) && (pDev->Chip.ReqAGPRate != 0))
            {
                //
                // Not sure if the Free below covers both the underlying pages and the
                // aperture addresses, so to be sure, we'll uncommit the pages and free.
                //
                vmmGARTUnCommit((U032)*pAddress, 0, PageCount);

                //
                // Perform VGARTD (VMM) AGP page free.
                //
                vmmGARTFree((U032)*pAddress);                   // for uncached AGP, free it all

                if (IsWriteCombined)
                {
                    // update our notion of how much persistent AGP has been alloced
                    pRmInfo->AGP.AGPHeapFree -= (PageCount << RM_PAGE_SHIFT);
                }
            }
            else
            {
                status = RM_ERROR;
            }
            
            break;
    }
    
    return (status);
}

// Translate a Virtual to Physical addr (used to provide the AGP aperture offset)
RM_STATUS osVirtualToPhysicalAddr
(
    VOID* virtaddr,
    VOID** physaddr
)
{
    U032 PageBase = (U032)virtaddr >> PAGESHIFT;
    U032 BaseAddr;

    // check that we've got a page (only need to succeed for one page translate the base)
    if (vmmPageCheckLinRange(PageBase, 1, 0) != 1)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Linear range not valid.\n\r");
        DBG_BREAKPOINT();
        return (RM_ERR_PAGE_TABLE_NOT_AVAIL);
    }

    // Get the first entry.
    if (!vmmCopyPageTable(PageBase, 1, (U032)&BaseAddr, 0))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Unable to copy page table.\n\r");
        return (RM_ERR_PAGE_TABLE_NOT_AVAIL);
    }

    (U032)*physaddr = (BaseAddr & ~RM_PAGE_MASK) | ((U032)virtaddr & RM_PAGE_MASK);
    return RM_OK;
}

RM_STATUS osCalculatePteAdjust
(
    PHWINFO pDev,
    U032    AddressSpace,
    VOID*   Address,
    U032 *  pPteAdjust
)
{
    *pPteAdjust = (U032)Address & RM_PAGE_MASK;

    return RM_OK;
}

// Get address information from the selector:offset address combination
RM_STATUS osGetAddressInfo
(
    PHWINFO pDev,
    U032  ChID,
    U032  Selector,
    VOID** pOffset,
    U032  Length,
    VOID** pAddress,
    NV_ADDRESS_SPACE *pAddressSpace
)
{
	PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;

    DESCRIPTOR  desc;
    U032        SelectorBase, SelectorLimit;
    U032        LinAddr, PageBase, PageArray;

#ifdef DEBUG
    U032        Page;
#endif // DEBUG

    // Default set the address type to system. 
    *pAddressSpace = ADDR_SYSMEM;

    vmmGetDescriptor(Selector, vmmGetSysVMHandle(), 0, &desc.Desc64);
    if ((desc.Desc32[0] == 0) && (desc.Desc32[1] == 0))       
    {
        return (RM_ERR_BAD_ADDRESS);
    }
    SelectorBase = ((U032)desc.Desc16[1]) |
                   ((U032)desc.Desc8[4] << 16) |
                   ((U032)desc.Desc8[7] << 24);
    SelectorLimit    = ((U032)desc.Desc16[0]) |
                       ((U032)(desc.Desc8[6] & 0x0F) << 16);
    if (desc.Desc8[6] & D_GRAN_PAGE)
    {
        //
        // Page granularity.
        //
        SelectorLimit = (SelectorLimit << 12) | 0x0FFF;
    }
    //
    // Gotta check all them bits for WIN95!!!
    //
    if (desc.Desc8[5] & D_ED)
    {
        //
        // Expand Down Segment.
        //
        // Check the "B" bit to determine maximum offset.
        //
        if (desc.Desc8[6] & D_DEF32)
            SelectorLimit = 0xFFFFFFFF;
        else
            SelectorLimit = 0x0000FFFF;
    }

    // Check to see if we have an address space conflict
    if ((*pAddressSpace != ADDR_UNKNOWN) && (*pAddressSpace != ADDR_SYSMEM) && (*pAddressSpace != ADDR_AGPMEM) && (*pAddressSpace != ADDR_FBMEM)) {
       return(RM_ERR_BAD_ADDRESS);
    }
    // Check to see if the transfer is in bounds
    if (((U032)*pOffset + Length) > SelectorLimit) {
       return(RM_ERR_BAD_ADDRESS);
    }

    // The address space is always system memory
    //KJK *pAddressSpace = ADDR_SYSMEM;
    // The returned address is the selector base + selector offset
    (U032)*pAddress = SelectorBase + (U032)*pOffset;

    //
    // We need to see if the address space matches any of our known ranges, such
    // as the framebuffer.  Override the memory type as necessary.  In most cases
    // this is done by the app when it names the context instance, but sometimes
    // he might not.
    //
    // For now, only check the framebuffer.  Need to add AGP GART aperture.
    //
    if (*pAddressSpace == ADDR_AGPMEM)
    {    
       return(RM_OK);
    }       
    
    //
    // Create the linear address of the buffer.
    //
    LinAddr = vmmSelectorMapFlat(vmmGetSysVMHandle(), Selector, 0) + (U032)*pOffset;
    //
    // Create the page base
    //
    PageBase = LinAddr >> PAGESHIFT;
    //
    // Linear Address is already in the shared or system memory arena.
    // Lock memory as was done under Win31...no need to change
    // memory contexts
    //
    if (vmmPageCheckLinRange(PageBase, 1, 0) != 1)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Linear page not valid.\n\r");
        //DBG_BREAKPOINT();
        return (RM_ERR_PAGE_TABLE_NOT_AVAIL);
    }
    //
    // Grab the first page and see if it falls within our known range
    //
    if (!vmmLinPageLock(PageBase, 1, 0))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Unable to lock detection page.\n\r");
        //return (RM_ERR_DMA_MEM_NOT_LOCKED);
        return (RM_OK);     // not fatal
    }
    if (!vmmCopyPageTable(PageBase, 1, (U032)&PageArray, 0))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Unable to copy detection page table.\n\r");
        //return (RM_ERR_PAGE_TABLE_NOT_AVAIL);
        return (RM_OK);     // not fatal
    }
    if (!(PageArray & P_PRES))
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Unable to lock page :LinAddr = ", (PageBase + Page) << 12);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "                          :PTE     = ", PageArray);
        //DBG_BREAKPOINT();
        //return (RM_ERR_PAGE_TABLE_NOT_AVAIL);
        return (RM_OK);     // not fatal
    }
    if (!vmmLinPageUnLock(PageBase, 1, 0))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Unable to unlock detection page.\n\r");
        //return (RM_ERR_DMA_MEM_NOT_LOCKED);
        return (RM_OK);     // not fatal
    }
    //
    // 4k page alignment
    //
    PageArray &= 0xFFFFF000;    
    //
    // If we're in FB space, we need to change the dma pte over to PNVM rather than PCI/AGP
    //
    if ((PageArray >= pDev->Mapping.PhysFbAddr) && (PageArray < (pDev->Mapping.PhysFbAddr + pDev->Framebuffer.HalInfo.RamSize)))
    {
        if ((PageArray+Length) >= (pDev->Mapping.PhysFbAddr + pDev->Framebuffer.HalInfo.RamSize))
        {
            //
            // We're straddling fb mem...bomb.
            //
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: osGetAddressInfo: buffer crosses fbmem boundary ", PageArray);
            DBG_BREAKPOINT();
            return (RM_ERR_MULTIPLE_MEMORY_TYPES);
        }

        //
        // Subtract off the start of the framebuffer
        //
        (U032)*pAddress = (U032)*pOffset = (PageArray - pDev->Mapping.PhysFbAddr) + 
                               (LinAddr  & RM_PAGE_MASK); // The offset within a page            
        //
        // Turn this into a PNVM dma structure
        //
        *pAddressSpace = ADDR_FBMEM;  
    }
    
    //
    // If we're in AGP space, we need to change the dma pte over to AGP rather than PCI
    //
    if ((pDev->Chip.Bus == BUS_AGP) && (pRmInfo->AGP.AGPLimit) && (pRmInfo->AGP.AGPPhysStart) &&
        (pDev->Chip.ReqAGPRate != 0))
    {
        if ((PageArray >= pRmInfo->AGP.AGPPhysStart) && (PageArray < (pRmInfo->AGP.AGPPhysStart + pRmInfo->AGP.AGPLimit)))
        {
            if ((PageArray+Length) > (pRmInfo->AGP.AGPPhysStart + pRmInfo->AGP.AGPLimit))
            {
                //
                // We're straddling agp mem...bomb.
                //
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: osGetAddressInfo: buffer crosses agp aperture boundary ", PageArray);
                DBG_BREAKPOINT();
                return (RM_ERR_MULTIPLE_MEMORY_TYPES);
            }

            //
            // Turn this into an AGP dma structure
            //
            *pAddressSpace = ADDR_AGPMEM;
        }
    }
    
    return (RM_OK);
}

RM_STATUS osLockUserMem
(
    PHWINFO          pDev, 
    U032             ChID,
    U032             Selector,
    VOID*            Offset,
    VOID*            DmaAddress,
    NV_ADDRESS_SPACE *DmaAddressSpace,
    U032             DmaLength,
    U032             PageCount,
    U032            *PageArray,
    VOID**           pLinAddr,
    VOID**           LockHandle
)
{
    U032 LinAddr;
    U032 PageBase;
    U032 Page;
    U032 temp;
    
    
    //
    // If we're asking to lock video memory, let's just build up the PTE's and assume
    // that the caller has real control over video memory (such as DirectDraw or the
    // display driver).
    //
    if (*DmaAddressSpace == ADDR_FBMEM)
    {
        
        LinAddr = (U032)*pLinAddr = (U032)*LockHandle = (U032)Offset;   // 0-based selector!!

        //
        // Should probably make sure the start+limit are within the available
        // video memory!!
        //
        
        //
        // This is hardcoded assuming zero = start of fb
        //
        PageBase = LinAddr >> PAGESHIFT;
        for (Page = 0; Page < PageCount; Page++)  {
            PageArray[Page] = ((PageBase + Page)<<12) | 3;
        }
        
        return (RM_OK);
    }

    //
    // Map the buffer.
    //
    LinAddr = (U032)*pLinAddr = vmmSelectorMapFlat(vmmGetSysVMHandle(), Selector, 0) + (U032)Offset;
    //
    // Validate the parameters.
    //
    PageBase = LinAddr >> PAGESHIFT;
    //
    // Use original linear address as the lock handle.
    //
    (U032)*LockHandle = (U032)*pLinAddr;

    if (*DmaAddressSpace == ADDR_AGPMEM)
    {
        U032 AGPBaseAddr;

        //
        // For AGP, just xlate the first page and derive the remaining pages knowing
        // this memory appears linear to the processor/bus master.
        //
        // Avoiding probing invalid GART entries fixes a problem seen on Compaq Aspen
        // systems (AMD 751/Via combo chipset). This occurs when OGL allocs a 16MB AGP
        // texture context DMA that's larger than what's been allocated so far.
        //

        // Get the first entry.
        if (!vmmCopyPageTable(PageBase, 1, (U032)&AGPBaseAddr, 0))
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Unable to copy page table.\n\r");
            return (RM_ERR_PAGE_TABLE_NOT_AVAIL);
        }

        // Fill in the rest of the entries (though instmem only needs one entry).
        for (Page = 0; Page < PageCount; Page++)
        {
            PageArray[Page] = (AGPBaseAddr + (Page << RM_PAGE_SHIFT)) | 3;
        }

        return (RM_OK);
    }

    if (((LinAddr >= MINSHAREDLADDR) && (LinAddr <= MAXSHAREDLADDR)) ||
        ((LinAddr >= MINSYSTEMLADDR) && (LinAddr <= MAXSYSTEMLADDR)))
    {   
        //
        // Linear Address is already in the shared or system memory arena.
        // Lock memory as was done under Win31...no need to change
        // memory contexts
        //
        if (vmmPageCheckLinRange(PageBase, PageCount, 0) != PageCount)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Linear range not valid.\n\r");
            DBG_BREAKPOINT();
            return (RM_ERR_PAGE_TABLE_NOT_AVAIL);
        }
        //
        // Scatter/gather lock the pages.
        //
        for (Page = 0; Page < PageCount; Page++)
        {
            //
            // Touch page to make sure it is in memory.  Win32s doesn't like
            // to play nicely with the VMMs memory manager.
            //
            NopMask |= *(volatile U008 *)LinAddr;
            LinAddr += PAGESIZE;
            //
            // If this is a standard AGP memory context (not an NV AGP context),
            // there is no need to relock the memory.  DirectDraw has already done
            // it for us, so we can just peer at the PTEs.
            //
            if (*DmaAddressSpace != ADDR_AGPMEM)
                if (!vmmLinPageLock(PageBase + Page, 1, 0))
                {
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Unable to lock pages.\n\r");
                    return (RM_ERR_DMA_MEM_NOT_LOCKED);
                }
            if (!vmmCopyPageTable(PageBase + Page, 1, (U032)&PageArray[Page], 0))
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Unable to copy page table.\n\r");
                return (RM_ERR_PAGE_TABLE_NOT_AVAIL);
            }
            if (!(PageArray[Page] & P_PRES))
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Unable to lock page :LinAddr = ", (PageBase + Page) << 12);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "                          :PTE     = ", PageArray[Page]);
                DBG_BREAKPOINT();
                return (RM_ERR_PAGE_TABLE_NOT_AVAIL);
            }
        }
    }
    else
    {
        //
        // Linear Address is in the private memory arena.
        // Lock memory and map linear address the the system memory arena.
        // Memory Context switch may be required.
        //
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "osLockUserMem: Original Linear Address = ",(U032)*pLinAddr);
        temp = vmmPageCheckLinRange(PageBase, PageCount, 0);
        if (temp != PageCount)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_USERERRORS, "NVRM:Linear range not valid.\n\r");
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "Page Base            = ", PageBase);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "Requested Page Count = ", PageCount);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_USERERRORS, "Actual Page Count    = ", temp);
            DBG_BREAKPOINT();
            return (RM_ERR_PAGE_TABLE_NOT_AVAIL);
        }
        for (Page = 0; Page < PageCount; Page++)
        {
            //
            // Touch page to make sure it is in memory.  Win32s doesn't like
            // to play nicely with the VMMs memory manager.
            //
            NopMask |= *(volatile U008 *)LinAddr;
            LinAddr += PAGESIZE;
        }
        //
        // Lock all pages at once and map to system memory arena.
        //
        if (*DmaAddressSpace != ADDR_AGPMEM)
            LinAddr = (U032)vmmLinPageLock(PageBase, PageCount, PAGEMAPGLOBAL);
        else    
            LinAddr = (U032)*pLinAddr;    // agp doesn't need a lock, so restore the original linaddr            
        if (!LinAddr)
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Unable to lock pages.\n\r");
            return (RM_ERR_DMA_MEM_NOT_LOCKED);
        }
        if (!vmmCopyPageTable(LinAddr >> PAGESHIFT, PageCount, (U032)&PageArray[0], 0))
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM:Unable to copy page table.\n\r");
            return (RM_ERR_PAGE_TABLE_NOT_AVAIL);
        }
        LinAddr += ((U032)*pLinAddr & 0xFFF);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "               New Linear Address      = ",LinAddr);
        (U032)*pLinAddr = LinAddr;
    }
    return (RM_OK);    
}
RM_STATUS osUnlockUserMem
(
    PHWINFO          pDev, 
    U032             ChID,
    VOID*            LinAddr,
    VOID*            DmaAddress,
    NV_ADDRESS_SPACE DmaAddressSpace,
    U032             DmaLength,
    U032             PageCount,
    VOID*            LockHandle,
    U032             DirtyFlag
)
{
    RM_STATUS status;
    U032      PageBase;
    U032      Page;
    U032      UnlockFlags;

    //
    // If we're asking to unlock video memory, we're assuming the caller (probably
    // direct draw)  has complete control of video memory, and therefore knows what
    // is locked/unlocked.  No need to do anything.
    //
    if (DmaAddressSpace == ADDR_FBMEM)
        return (RM_OK);    

    //
    // As a convenience to the drivers, we allow an AGP Context DMA to be setup that's
    // larger than the underlying alloced pages (setup to map the whole aperture).
    // Since we avoided a call to CheckLinRange in osLockUserMem, we can't do it here
    // either. And, besides, we don't lock/unlock AGP memory anyways.
    //
    if (DmaAddressSpace == ADDR_AGPMEM)
        return (RM_OK);    

    PageBase = (U032)LinAddr >> 12;
    UnlockFlags = 0;
    if ((U032)LockHandle != (U032)LinAddr)
    {
        UnlockFlags = PAGEMAPGLOBAL;
    }
    //
    // KJK Allow a twice-deleted dma context.  Windows will return an error to CheckLinRange
    // if the context has already been unlocked by someone else, so don't go try talking
    // to the memory.
    //
//#ifdef DEBUG
    if (vmmPageCheckLinRange((U032)LinAddr >> PAGESHIFT, PageCount, 0) != PageCount)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Unlocking invalid memory range!\n\r");
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "     Address: ", (U032)LinAddr);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "   PageCount: ", PageCount);
        DBG_BREAKPOINT();
        DirtyFlag = 0;
    }
//#endif // DEBUG
    if (DirtyFlag)
    {
        NopMask  = 0xFF;
        for (Page = 0; Page < PageCount; Page++)
        {
            *(volatile U008 *)LinAddr &= NopMask;
            (U032)LinAddr += PAGESIZE;
        }
    }
    //
    // Unlock pages and restore the memory context if neccessary.
    //
    if (DmaAddressSpace != ADDR_AGPMEM)
        status = !vmmLinPageUnLock(PageBase, PageCount, UnlockFlags) ? RM_ERR_DMA_MEM_NOT_UNLOCKED : RM_OK;
    return (status);
}

RM_STATUS osMapFifo
(
    PHWINFO pDev, 
    U032 clientClass, 
    U032 ChannelID,
    VOID **ppAddress
)
{
    *ppAddress = (VOID *) ((U032)pDev->DBnvAddr + FIFO_USER_BASE(pDev) + (ChannelID << 16));
    return RM_OK;
}

RM_STATUS osMapFrameBuffer
(
    PHWINFO pDev, 
    U032 clientClass, 
    U032 fbOffset,
    U032 length,
    VOID **ppAddress
)
{
    *ppAddress = (VOID *) ((U032)pDev->DBfbAddr + fbOffset);
    return RM_OK;

}

RM_STATUS osMapInstanceMemory
(
    PHWINFO pDev, 
    U032 clientClass, 
    U032 instanceOffset,
    U032 length,
    VOID **ppAddress
)
{
    *ppAddress = (VOID *) ((U032)INSTANCE_MEMORY_BASE + instanceOffset);
    return RM_OK;
}

// This procedure undoes the work of osMapFifo, osMapFrameBuffer and osMapInstanceMemory
RM_STATUS osUnmapMemory
(
    PHWINFO pDev,
    U032    clientClass,
    VOID   *address,
    U032    length
)
{
    // There is nothing to unmap in win9x.
    return RM_OK;
}

RM_STATUS osSetFifoDmaParams
(
    PHWINFO pDev,
    U032   *pFetchTrigger,
    U032   *pFetchSize,
    U032   *pFetchRequests
)
{
    U032 data32;

    //
    // Default win9x DMA channel fetch parameters are optimized for
    // for the D3D driver.
    //

    //
    // The registry overrides are specified in values that can be
    // plugged directly into the hardware.  We need to convert them
    // to the appropriate units so they'll continue to work.
    //

    //
    // If there's a dma fifo fetch trigger override, use it now.
    //
    if (osReadRegistryDword(pDev,strDevNodeRM, strFetchTriggerOverride, &data32) == RM_OK)
        *pFetchTrigger = (data32+1) * 8;
    else
        *pFetchTrigger = 128;           // 128 bytes

    //
    // If there's a dma fifo fetch size override, use it now.
    //
    if (osReadRegistryDword(pDev,strDevNodeRM, strFetchSizeOverride, &data32) == RM_OK)
        *pFetchSize = (data32+1) * 32;
    else
        *pFetchSize = 32;               // 32 bytes

    //
    // If there's a dma fifo fetch max reqs override, use it now.
    //
    if (osReadRegistryDword(pDev,strDevNodeRM, strFetchMaxReqsOverride, &data32) == RM_OK)
        *pFetchRequests = data32;
    else
        *pFetchRequests = 15;           // 15 outstanding read requests

    return RM_OK;
}

//---------------------------------------------------------------------------
//
//  OS specific time of day.
//
//---------------------------------------------------------------------------

RM_STATUS osGetCurrentTime
(
    U032 *year,
    U032 *month,
    U032 *day,
    U032 *hour,
    U032 *min,
    U032 *sec,
    U032 *msec
)
{
    U032 bcd;
    CLIENT_STRUCT  crs;
    U032 i;
    
    //
    // Sorry, no msec accuracy.
    //
    *msec = 0;
    //
    // Read the real time clock chip. Convert BCD to integer.
    //
    /*
    outp(0x70, 0x00); bcd = inp(0x71);
    *sec = (bcd & 0x0F) + (bcd >> 4) * 10;
    outp(0x70, 0x02); bcd = inp(0x71);
    *min = (bcd & 0x0F) + (bcd >> 4) * 10;
    outp(0x70, 0x04); bcd = inp(0x71);
    *hour = (bcd & 0x0F) + (bcd >> 4) * 10;
    outp(0x70, 0x07); bcd = inp(0x71);
    *day = (bcd & 0x0F) + (bcd >> 4) * 10;
    outp(0x70, 0x08); bcd = inp(0x71);
    *month = (bcd & 0x0F) + (bcd >> 4) * 10;
    outp(0x70, 0x09); bcd = inp(0x71);
    *year = (bcd & 0x0F) + (bcd >> 4) * 10 + 1900;
    if (*year < 1995)
        *year += 100;
    */
    
    //
    // To make sure we get a valid time, ask the BIOS to recover the data
    // instead of going directly to the hardware.
    //
    // Wait for success, but don't wait too long
    //
    crs.CWRS.Client_Flags = BIT(CF_BIT);
    i = 0;
    while ((crs.CWRS.Client_Flags & BIT(CF_BIT)) && --i)
    {
        crs.CBRS.Client_AH  = 0x02;    // READ_CMOS_TIME
        TIMER(&crs);
    }
    
    //
    // Convert from BCD.  Also make sure the time is "somewhat" valid
    //
    bcd = crs.CBRS.Client_DH;
    *sec = (bcd & 0x0F) + (bcd >> 4) * 10;
    if (*sec > 60)
        *sec = 60;
    bcd = crs.CBRS.Client_CL;
    *min = (bcd & 0x0F) + (bcd >> 4) * 10;
    if (*min > 60)
        *min = 60;
    bcd = crs.CBRS.Client_CH;
    *hour = (bcd & 0x0F) + (bcd >> 4) * 10;
    if (*hour > 24)
        *month = 24;
    
    //
    // Now get the date.
    //
    // Wait for success, but don't wait too long
    //
    crs.CWRS.Client_Flags = BIT(CF_BIT);
    i = 0;
    while ((crs.CWRS.Client_Flags & BIT(CF_BIT)) && --i)
    {
        crs.CBRS.Client_AH  = 0x04;    // READ_CMOS_DATE
        TIMER(&crs);
    }
    
    //
    // Convert from BCD.  Also make sure the time is "somewhat" valid
    //
    bcd = crs.CBRS.Client_DL;
    *day = (bcd & 0x0F) + (bcd >> 4) * 10;
    if (*day > 31)
        *day = 31;
    bcd = crs.CBRS.Client_DH;
    *month = (bcd & 0x0F) + (bcd >> 4) * 10;
    if (*month > 12)
        *month = 12;
    bcd = crs.CBRS.Client_CL;
    *year = (bcd & 0x0F) + (bcd >> 4) * 10 + 1900;
    if (*year < 1995)
        *year += 100;
    
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  OS specific IPC, notifications and error callbacks.
//
//---------------------------------------------------------------------------
extern DWORD vwin32SetWin32Event( DWORD );
extern DWORD vwin32CloseVxdHandle( DWORD );

RM_STATUS osNotifyEvent
(
    PHWINFO   pDev,
    POBJECT   Object,
    U032      Notifier,    
    U032      Method,
    U032      Data,
    RM_STATUS Status,
    U032      Action
)
{
    PEVENTNOTIFICATION NotifyEvent;
    
    //
    // Check for the correct actions
    //
    if (Action == NV_OS_WRITE_THEN_AWAKEN)
    {
        //
        // Walk this object's event list and find any matches for this
        // specific notify
        //
        NotifyEvent = Object->NotifyEvent;
        while (NotifyEvent)
        {
            //
            // Check for a notify index match
            //
            if (NotifyEvent->NotifyIndex == Notifier)
            {
                // 
                // Match!!
                //
                switch (NotifyEvent->NotifyType)                
                {
                    case NV01_EVENT_WIN32_EVENT:
#ifndef IKOS
                        //
                        // Attempt to trigger the event
                        //
                        if (vwin32SetWin32Event((U032)(NotifyEvent->Data.low)) == 0)
                        {
                            //
                            // Failed, so lose this event so we don't try again
                            //
                            vwin32CloseVxdHandle((U032)(NotifyEvent->Data.low));
                            NotifyEvent->NotifyType = 0;
                        }
#endif
                        break;
                        
                    /*
                    case NV_OS_EVENT_VM_EVENT:
                        //
                        // Schedule event servicing.  Do this in the system VM.
                        //
                        vmmCallPriorityVMEvent(HIGH_PRI_DEVICE_BOOST,
                                               vmmGetSysVMHandle(),
                                               0,
                                               0,
                                               (U032)(NotifyEvent->Data.low),
                                               0);
                    
                        break;
                    */
                        
                    case NV01_EVENT_KERNEL_CALLBACK:
                        //
                        // Perform a direct callback.  This is assumed to be a Ring0->Ring0
                        // transfer.
                        //
                        {
                        CRS       SavedCRS;
                        U032      SysVM = vmmGetSysVMHandle();
                        
                        //
                        // Just in case the callback is ill behaved
                        //
                        vmmSaveClientState(&SavedCRS);
                        
                     
                        //
                        // Schedule RM servicing.
                        //
                        vmmCallPriorityVMEvent(TIME_CRITICAL_BOOST,
                                               SysVM,
                                               0,
                                               (U032)Status,
                                               (U032)(NotifyEvent->Data.low),
                                               0);
                        
                        //
                        // Restore our state.
                        //
                        vmmRestoreClientState(&SavedCRS);
                        
                        }
                        break;
                                                
                    default:
                        break;
                }                                                
            }
            
            // 
            // On to the next one
            //
            NotifyEvent = NotifyEvent->Next;
        }
    
    }
    //
    // The rest of these actions are legacy channel-based notifies
    //
    else
    {    
        return NVOS10_STATUS_ERROR_BAD_EVENT;
    }    
}

RM_STATUS osError
(
    POBJECT   Object,
    U032      Method,
    V032      Data,
    RM_STATUS Error
)
{
    PHWINFO pDev;
	PWIN9XHWINFO pOsHwInfo;
    POBJECT *ppObject;
    int i;

    for(i=0;i<MAX_INSTANCE;i++){
        pDev = NvDBPtr_Table[i];
        if(pDev != NULL){
			pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;

            // Search the fifo object tree for an object with the same name.
            fifoSearchObject(pDev,Object->Name,Object->ChID,ppObject);
            if(ppObject != NULL){
              if(*ppObject == Object){
                  break;
              }else{
                  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ERROR in osError (Object not found)\n\r");
                  return (RM_ERROR);
              }
            }
        }
    }

    //
    // Only call back on errors, not warnings.
    //
    if (Error & 0xFFFF)
    {
        if (pOsHwInfo->dllNotifySelector + pOsHwInfo->dllNotifyOffset == 0)    
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ERROR - Notify callback invalid!\n\r");
            return (RM_ERROR);
        }
        if (Object)
            return (osScheduleCallback(pDev,
                                       (U016)pOsHwInfo->dllNotifySelector,
                                       (U016)pOsHwInfo->dllNotifyOffset,
                                       6,
                                       Object->ChID,
                                       Object->Name, 
                                       Method,
                                       Data,
                                       Error,
                                       0));
        else       
            return (osScheduleCallback(pDev,
                                       (U016)pOsHwInfo->dllNotifySelector,
                                       (U016)pOsHwInfo->dllNotifyOffset,
                                       6,
                                       Data,
                                       0, 
                                       Method,
                                       0,
                                       Error,
                                       0));
    }
    return (RM_OK);
}

RM_STATUS osExit
(
    char *errstr
)
{
    vmmFatalErrorHandler((DWORD)errstr, 0);
    return (RM_OK);
}
//---------------------------------------------------------------------------
//
//  Misc services.
//
//---------------------------------------------------------------------------

RM_STATUS osDelay
(
    U032 MilliSeconds
)
{
    U032 Start;

    Start = vmmGetSystemTime();
    while ((vmmGetSystemTime() - Start) < MilliSeconds);
    return (RM_OK);
}

RM_STATUS osDelayUs
(
    U032 MicroSeconds
)
{
    U032 Start;

    //
    // TEMPORARY WORKAROUND
    //
    // The BIP language spec calls for a usec delay, yet win9x can only
    // handle msec delays.  So, floor us at 1ms and pre-convert all other 
    // timing requests
    //
    if (MicroSeconds < 1000)
        MicroSeconds = 1;
    else
        MicroSeconds = MicroSeconds / 1000;

    Start = vmmGetSystemTime();
    while ((vmmGetSystemTime() - Start) < MicroSeconds);
    return (RM_OK);
}
//---------------------------------------------------------------------------
//
//  PCI config space read/write routines.
//
//---------------------------------------------------------------------------
#define PCI_FUNCTION_ID         0xB1
#define READ_CONFIG_BYTE        0x08
#define READ_CONFIG_WORD        0x09
#define READ_CONFIG_DWORD       0x0A
#define WRITE_CONFIG_BYTE       0x0B
#define WRITE_CONFIG_WORD       0x0C
#define WRITE_CONFIG_DWORD      0x0D

U032
osPciReadDword(VOID *handle, U032 offset)
{
    CLIENT_STRUCT *pcrs = (CLIENT_STRUCT *)handle;

    pcrs->CBRS.Client_AH  = PCI_FUNCTION_ID;
    pcrs->CBRS.Client_AL  = READ_CONFIG_DWORD;
    pcrs->CWRS.Client_DI  = (U016) offset;
    PCI(pcrs);

    return pcrs->CRS.Client_ECX;
}

U016
osPciReadWord(VOID *handle, U032 offset)
{
    CLIENT_STRUCT *pcrs = (CLIENT_STRUCT *)handle;

    pcrs->CBRS.Client_AH  = PCI_FUNCTION_ID;
    pcrs->CBRS.Client_AL  = READ_CONFIG_WORD;
    pcrs->CWRS.Client_DI  = (U016) offset;
    PCI(pcrs);

    return pcrs->CBRS.Client_CL;
}

U008
osPciReadByte(VOID *handle, U032 offset)
{
    CLIENT_STRUCT *pcrs = (CLIENT_STRUCT *)handle;

    pcrs->CBRS.Client_AH  = PCI_FUNCTION_ID;
    pcrs->CBRS.Client_AL  = READ_CONFIG_BYTE;
    pcrs->CWRS.Client_DI  = (U016) offset;
    PCI(pcrs);

    return pcrs->CBRS.Client_CL;
}

VOID
osPciWriteDword(VOID *handle, U032 offset, U032 value)
{
    CLIENT_STRUCT *pcrs = (CLIENT_STRUCT *)handle;

    pcrs->CBRS.Client_AH  = PCI_FUNCTION_ID;
    pcrs->CBRS.Client_AL  = WRITE_CONFIG_DWORD;
    pcrs->CWRS.Client_DI  = (U016) offset;
    pcrs->CRS.Client_ECX  = value;
    PCI(pcrs);
}

VOID
osPciWriteWord(VOID *handle, U032 offset, U016 value)
{
    CLIENT_STRUCT *pcrs = (CLIENT_STRUCT *)handle;

    pcrs->CBRS.Client_AH  = PCI_FUNCTION_ID;
    pcrs->CBRS.Client_AL  = WRITE_CONFIG_WORD;
    pcrs->CWRS.Client_DI  = (U016) offset;
    pcrs->CBRS.Client_CL  = (U008) value;
    PCI(pcrs);
}

VOID
osPciWriteByte(VOID *handle, U032 offset, U008 value)
{
    CLIENT_STRUCT *pcrs = (CLIENT_STRUCT *)handle;

    pcrs->CBRS.Client_AH  = PCI_FUNCTION_ID;
    pcrs->CBRS.Client_AL  = WRITE_CONFIG_BYTE;
    pcrs->CWRS.Client_DI  = (U016) offset;
    pcrs->CBRS.Client_CL  = value;
    PCI(pcrs);
}

VOID *
osPciInitHandle(U008 bus, U008 device, U008 func, U016 *pVendorId, U016 *pDeviceId)
{
    U016 venID, devID;
    static CLIENT_STRUCT crs;

    // get vendor/device id at offset 0
    _asm {
        push    eax
        push	edx

        // build up bus/device/func config addr
        movzx   eax, bus
        shl     eax, 10h
        movzx   edx, device
        shl     edx, 0Bh
        or      dh,  func

        or      eax, edx
        or      eax, 80000000h

        mov     edx, 0cf8h
        out     dx,eax
        mov     edx, 0cfch
        in      eax, dx

        // vendor/device from config data
        mov     venID, ax
        shr     eax, 16
        mov     devID, ax

        pop     edx
        pop     eax
    }

    // return these to the caller
    if (pVendorId)
        *pVendorId = venID;
    if (pDeviceId)
        *pDeviceId = devID;

    // initialize the CLIENT_STRUCT state
    crs.CBRS.Client_AH  = PCI_FUNCTION_ID;  // PCI_FUNCTION_ID
    crs.CBRS.Client_AL  = 0x02;             // FIND_PCI_DEVICE
    crs.CWRS.Client_CX  = devID;            // Device ID
    crs.CWRS.Client_DX  = venID;            // Vendor ID
    crs.CWRS.Client_SI  = 0;                // Index
    PCI(&crs);

    return &crs;
}


#ifdef OBSOLETE_FUNCTIONS
RM_STATUS osRedrawScreen
(
    VOID
)
{
    osIPC(NVRM_WM_REDRAW_SCREEN, 0, 0);
    return (RM_OK);
}
RM_STATUS osRedrawCanvas
(
    U032 hwnd
)
{
    if (hwnd != 0)
        osIPC(NVRM_WM_REDRAW_HWND, (U016)hwnd, 0);
    return (RM_OK);
}
RM_STATUS osBeginWatchCanvas
(
    U032 hwnd
)
{
    if (hwnd != 0)
        osIPC(NVRM_WM_OPEN_HWND, (U016)hwnd, 0);
    return (RM_OK);
}
RM_STATUS osEndWatchCanvas
(
    U032 hwnd
)
{
    if (hwnd != 0)
        osIPC(NVRM_WM_CLOSE_HWND, (U016)hwnd, 0);
    return (RM_OK);
}
#endif // OBSOLETE_FUNCTIONS

// New semaphore functions for Multi-Mon support.
RM_STATUS mmWaitSemaphore
(
    VOID
)
{
//    vmmWaitSemaphore(mmSemaphore, 0);
    return (RM_OK);
}
RM_STATUS mmSignalSemaphore
(
    VOID
)
{
//    vmmSignalSemaphore(mmSemaphore);
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  OS name validation.
//
//---------------------------------------------------------------------------
#ifdef OBSOLETE_FUNCTIONS
RM_STATUS osValidateVideoSink
(
    char *OsName
)
{
    U032 i;
    
    for (i = 0; i < 4; i++)
        if (OsName[i] != nameVideoSink[i])
            return (RM_ERR_INVALID_OS_NAME);
    return (RM_OK);
}
RM_STATUS osValidateImageVideo
(
    char *OsName,
    U032 *CanvasID
)
{
    U032 i;
    
    *CanvasID = 0;
    for (i = 0; i < 3; i++)
        if (*OsName++ != nameImageVideo[i])
            return (RM_ERR_INVALID_OS_NAME);
    while (*OsName >= '0' && *OsName <= '9')
        *CanvasID = *CanvasID * 10 + *OsName++ - '0';
    return (*OsName == ':' ? RM_OK : RM_ERROR);
}


// Validate the DMA address space name from the app
// A DMA object with no name is treated as System Memory space
// The DMA direction does not matter for system memory space
RM_STATUS osValidateDMAObject(
    char *OsName,
    U032 DmaObjectName,
    NV_ADDRESS_SPACE *AddressSpace
)
{
    U032 i;
    RM_STATUS status;

    if (OsName[0] == '\0') {
        *AddressSpace = ADDR_SYSMEM;
        return(RM_OK);
    }

    if (osStringCompare(OsName, nameSysMem))
    {
       *AddressSpace = ADDR_SYSMEM;
       return(RM_OK);
    }

    if (osStringCompare(OsName, nameLVidMem))
    {
       *AddressSpace = ADDR_FBMEM;
       return(RM_OK);
    }
    
    if (osStringCompare(OsName, nameSVidMem))
    {
        //
        // Only allow true AGP on AGP systems!!  
        //
        if (pDev->Chip.Bus == BUS_AGP)
            //
            // When we attempt to lock this context we need to verify it falls within
            // the AGP space.  If not, it is really a PCI memory block and needs to be
            // treated as such.
            //
            *AddressSpace = ADDR_AGPMEM;
        else    
            *AddressSpace = ADDR_SYSMEM;
        return(RM_OK);
    }
    
    *AddressSpace = ADDR_SYSMEM;
    
    return(RM_ERR_INVALID_OS_NAME);
    
}
#endif // OBSOLETE_FUNCTIONS

//---------------------------------------------------------------------------
//
//  Interrupt time stuff.
//
//---------------------------------------------------------------------------

CARRY osInterrupt
(
    U032 hIRQ,
    U032 hVM
)
{
    PHWINFO   pDev;
    U032 status;

    //
    // Need to do this within a routine with no local variables to make
    // sure we can return the carry flag correctly without the compiler
    // stomping on it.
    //
    //int  saved_DeviceInstance; 
    // Save the DeviceInstance of the current executing RM thread.
    //saved_DeviceInstance = DeviceInstance; 

    status = osInterruptProcess(&pDev,hIRQ, hVM);
    
    //_asm push eax
    //DeviceInstance = saved_DeviceInstance; 
    //_asm pop  eax
        
    //
    // Of course, the Microsoft compiler handles things differently from
    // the Watcom compiler.  We need to exit this routine with the
    // carry flag denoting if we handled this interrupt ourselves.
    // But the return value from vmmCallBackExit() is in EAX and will
    // destroy any carry flag.  So, let's set the carry flag according
    // to EAX.
    //
    // CY=1 if ok to share since we didn't process
    // CY=0 if we processed our own interrupt (w/ EOI)
    //        
    _asm {   
        clc        
        cmp eax, 0
        je notus
        stc
        notus:
    }

}

BOOL CDECL osServiceHandler
(
    VOID
)
{
    PHWINFO   pDev;
    U032      hDevice;
	PWIN9XHWINFO pOsHwInfo;

    _asm mov hDevice, edx
    pDev = (PHWINFO) hDevice;
	pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;

    if (pDev->DeviceDisabled)
    {
        U032 we_are_mapped;
        U032 pci_address;
    
        // Check if the device has become unmapped. If so stop servicing.
        // Read the 1st base register.
        pci_address = pDev->Mapping.nvBusDeviceFunc;
        _asm mov  dx, 0xcf8
        _asm in   eax, dx
        _asm push eax
        _asm mov  eax, pci_address  
        _asm or   eax, 0x80000010 
        _asm out  dx, eax
        _asm mov  dx, 0xcfc
        _asm in   eax,dx
        _asm mov  we_are_mapped,eax
        _asm pop  eax
        _asm mov  dx, 0xcf8
        _asm out  dx, eax
    
        if(!we_are_mapped)
        {
            // The device is disabled. Just ignore this interrupt.
            return(TRUE);
        }  
    }

    //See if we are still lineraly mapped
    if (nvAddr->Reg032[0] == 0xFFFFFFFF)
    {
		  return(1);
	 }
 
    if (pOsHwInfo->osScheduled)
    {    
        pOsHwInfo->osScheduled--;
        osEnsureDeviceEnabled(pDev);

        {
            BOOL x;
            U032 t1,t2;

            t1=(U032) pOsHwInfo->NvIsrStackFrame;
            osSwapStack(t1, t2);
            pOsHwInfo->NvSaveVmmStack = (U032 *) t2;

            x=osService(pDev);

            t1=(U032) pOsHwInfo->NvSaveVmmStack;
            osSwapStack(t1, t2);
            pOsHwInfo->NvIsrStackFrame = (U032 *) t2;

            return(x);        
        }
    }
}

U032 osInterruptProcess
(
    PHWINFO *ppDev,
    U032 hIRQ,
    U032 hVM
)
{
    PHWINFO pDev;
	PWIN9XHWINFO pOsHwInfo;

    V032 pmc;
    V032 pmce;
#ifdef RM_STATS
    U032 EnterTimeLo;
    U032 EnterTimeHi;
    U032 ExitTimeLo;
    U032 ExitTimeHi;
    U032 IntTimeLo;
    U032 IntTimeHi;
#endif
//  int  saved_DeviceInstance; 
#ifdef TIMER_OVERRIDE
    V032 ptimer;
#endif
    VOID VBlank(PHWINFO);

    CLI

//
  // Let's just walk the devices and find the one that is
  // associated with this hIRQ.  The IRQ handle was saved away
  // when each particular device hooked its interrupt routine
  // with the VPICD.
  //
  // We should get called for every handle, independent of
  // actual IRQ sharing in the system.
  //
  // For all possible devices in the system
  //
  {
    U032 i;
    
    for (i=0;i<MAX_INSTANCE;i++)
    {
        // Check if this device exists.
        if (NvDBPtr_Table[i] != NULL)
        {
            pDev = NvDBPtr_Table[i];
			pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;
            //DeviceInstance = i;
            
            if (pDev->Mapping.hpicIRQ != NULL && pDev->Mapping.hpicIRQ == hIRQ)
            {
                // Make sure this device has not been disabled by Windows98.
                // If it has been disabled then re-enable it.
                osEnsureDeviceEnabled(pDev);

                // This may or may not be our interrupt, but we have established our
                // pDev (osInterrupt needs it setup, so we can chain the interrupt).
                *ppDev = pDev;

                // This code is in place for MultiMonitor support during a shutdown.
                // If the system is shutting down Win98 will zero out the pci base registers.
                // If this happens ... just return as if it was not meant for us.
                if (pDev->DeviceDisabled)
                {
                    U032 we_are_mapped;
                    U032 pci_address;
                  
                    // Check if the device has become unmapped. If so stop servicing.
                    // Read the 1st base register.
                    pci_address = pDev->Mapping.nvBusDeviceFunc;
                    _asm mov  eax, pci_address  
                    _asm or   eax, 0x80000010 
                    _asm mov  dx, 0xcf8
                    _asm out  dx, eax
                    _asm mov  dx, 0xcfc
                    _asm in   eax,dx
                    _asm mov  we_are_mapped,eax
                  
                    if(!we_are_mapped)
                        return(1);
                }

                //See if we are still linearaly mapped
                if (nvAddr->Reg032[0] == 0xFFFFFFFF)
                {
                    return(1);
                }
                
                //
                // Windows thought this IRQ was meant for us...
                //
                // Check if this device's interrupt has been asserted.
                //
                pmc  = REG_RD32(NV_PMC_INTR_0);
                pmce = REG_RD32(NV_PMC_INTR_EN_0);
                if (pmc && pmce)
                {
                    // 
                    // Hey, it was us!  Break out.
                    //
                    i = MAX_INSTANCE;
                }     
                else
                {
                    //
                    // It was our IRQ, but it wasn't our device
                    // Restore the DeviceInstance of the current executing RM thread.
                    // Let it chain
                    //
                    // JIM !!!! DON'T RESTORE THIS YET !!!!!
                    //DeviceInstance = saved_DeviceInstance; 
                    return(1);
                }                    
            }                    
        } 
        else
        {
            //
            // No more devices to check !
            // Let it chain to other ISR's
            // Restore the DeviceInstance of the current executing RM thread.
            //
            return(1);
        }
    } 
  }

    if (pmc && pmce)
    {
        //
        // Turn off interrupts here to avoid glitching the PIC on cheap
        // PCI chipsets.
        //
        REG_WR32(NV_PMC_INTR_EN_0, 0x00000000);
#ifdef RM_STATS
        //
        // Profile the interrupt execution time.
        //
        tmrGetCurrentTime(pDev, &EnterTimeHi, &EnterTimeLo);
        //
        // 12/20/95 - msk - get time right away to do accurate timings for
        // new emulation method
        //
        pDev->Chip.osStartInterrupt = EnterTimeLo;
#endif
        //
        // EOI the PIC.
        //
        VPICD_Phys_EOI(pDev->Mapping.hpicIRQ);

#ifdef DEBUG_TIMING
        if (dbgCountInt[dbgFrame] < 20)
           dbgStartInt[dbgFrame][dbgCountInt[dbgFrame]] = REG_RD32(NV_PTIMER_TIME_0) - dbgStart ;
        dbgCountInt[dbgFrame]++;
#endif // DEBUG_TIMING

        //
        // Call any high priority service routines and schedule low priority 
        // service routines if service still required.
        //
        if (pmc & (DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING) |
                   DRF_DEF(_PMC, _INTR_0, _PCRTC2, _PENDING)))
        {
            if (!pDev->Vga.Enabled)   // no need for vblank service in NV3
                                        // DOS prgms which poll 3DA for vblank status
                                        // may not see it because of latency
            {
            //
            // Do any VBlank servicing.
            //    
                VBlank(pDev);
            }
        }

        //
        // check if need to service any high priority Mediaport tasks
        //
        //mpService(pDev);

#ifdef NV_VGA
#ifdef TIMER_OVERRIDE // Note: TIMER_OVERRIDE should usually not be defined!
        //
        // Usually stop the timer interrupt from being serviced
        //
        if (pDev->Vga.Enabled)
        {
           if (pmc & DRF_DEF(_PMC, _INTR_0, _PTIMER, _PENDING))
           {
               REG_WR_DRF_DEF(_PTIMER, _INTR_0, _ALARM, _RESET);
               ptimer = REG_RD32(NV_PTIMER_INTR_0);
               vgaTimer();
           }
        }
#endif // TIMER_OVERRIDE
#endif // NV_VGA
#ifdef RM_STATS
        //
        // Add time spent in service routines to accumulated time.
        //
        tmrGetCurrentTime(pDev, &ExitTimeHi, &ExitTimeLo);
        IntTimeLo = ExitTimeLo - EnterTimeLo;
        IntTimeHi = ExitTimeHi - EnterTimeHi;
        if (IntTimeLo > ExitTimeLo)
            IntTimeHi--;
        pDev->Chip.ExceptionTimeLo += IntTimeLo;
        if (pDev->Chip.ExceptionTimeLo < IntTimeLo)
            IntTimeHi++;
        pDev->Chip.ExceptionTimeHi += IntTimeHi;    
#endif
        // BUG_BM_OFF();
        pmc = REG_RD32(NV_PMC_INTR_0);
        // BUG_BM_ON();
        if (pmc)
        {
        
            if (!pOsHwInfo->osScheduled)
            {   
            
                U032    VMh = vmmGetSysVMHandle();
                     
                //
                // Schedule RM servicing.
                //
                pOsHwInfo->osScheduled = 1;
                vmmCallPriorityVMEvent(TIME_CRITICAL_BOOST,
                                       VMh,
                                       0,
                                       (DWORD)pDev,
                                       (U032)osServiceHandler,
                                       0);
            }                                       
        }
        else
        {
            //
            // Reenable chip interrupts.
            // Only if the device was NOT DISABLED.
            if (!pDev->DeviceDisabled){
                REG_WR32(NV_PMC_INTR_EN_0, pDev->Chip.IntrEn0);
            }
        }
#ifdef DEBUG_TIMING
        if (dbgCountInt[dbgFrame] - 1 < 20)
            dbgEndInt[dbgFrame][dbgCountInt[dbgFrame]-1] = REG_RD32(NV_PTIMER_TIME_0) - dbgStart ;
#endif // DEBUG_TIMING
        //vmmCallBackExit(0);
        // Restore the DeviceInstance of the current executing RM thread.
        //DeviceInstance = saved_DeviceInstance; 
        return(0);
    }
    else
    {
        //
        // This interrupt is not for us, chain to next handler.
        //
        // Restore the DeviceInstance of the current executing RM thread.
        //DeviceInstance = saved_DeviceInstance; 
        return(1);
    }
}


VOID CDECL rmService
(
    VOID
)
{
    PHWINFO pDev=NULL;
	PWIN9XHWINFO pOsHwInfo;
    V032 pmc;
    V032 pmce;
#ifdef RM_STATS
    U032 EnterTimeLo;
    U032 EnterTimeHi;
    U032 ExitTimeLo;
    U032 ExitTimeHi;
    U032 IntTimeLo;
    U032 IntTimeHi;
#endif
    CRS  SavedCRS;
    VOID VBlank(PHWINFO);

    //
    // Turn off interrupts to keep real interrupt routine from interfering.
    //
    CLI
    // BUG_BM_OFF();
    pmc  = REG_RD32(NV_PMC_INTR_0);
    pmce = REG_RD32(NV_PMC_INTR_EN_0);
    // BUG_BM_ON();
    //
    // Turn off chip interrupt here to avoid re-entrancy issues.
    //
    REG_WR32(NV_PMC_INTR_EN_0, 0x00000000);
    if (pmc && pmce)
    {
#ifdef RM_STATS
        //
        // Profile the interrupt execution time.
        //
        tmrGetCurrentTime(&EnterTimeHi, &EnterTimeLo);
#endif
        //
        // EOI the PIC.
        //
        VPICD_Phys_EOI(pDev->Mapping.hpicIRQ);
        //
        // Call all service routines requiring attention.
        //
        if (pmc & DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING))
        {
            //
            // Do any VBlank servicing.
            //    
            VBlank(pDev);
        }
#ifdef RM_STATS
        //
        // Add time spent in service routines to accumulated time.
        //
        tmrGetCurrentTime(&ExitTimeHi, &ExitTimeLo);
        IntTimeLo = ExitTimeLo - EnterTimeLo;
        IntTimeHi = ExitTimeHi - EnterTimeHi;
        if (IntTimeLo > ExitTimeLo)
            IntTimeHi--;
        pDev->Chip.ExceptionTimeLo += IntTimeLo;
        if (pDev->Chip.ExceptionTimeLo < IntTimeLo)
            IntTimeHi++;
        pDev->Chip.ExceptionTimeHi += IntTimeHi;    
#endif
        // BUG_BM_OFF();
        pmc = REG_RD32(NV_PMC_INTR_0);
        // BUG_BM_ON();
    }
    if (pmc)
    {
        //
        // Service chip right now.
        //
        //
        // The only sure way to make sure that we don't deadlock if we get a second
        // flush fifo call from DirectAudio or DirectInput while we are currently
        // servicing a previous flush fifo is to protect the calls with a real
        // blocking semaphore.
        //
        vmmWaitSemaphore(osSemaphore, 0);
        if (!osService(pDev))
        {
            //
            // The RM is currently in service.  To allow the InService event
            // to complete, a dummy call to the 16 bit NVSYS.DLL will be made
            // to force the VMM to reschedule its events.  Maybe next time
            // through we will actually be able to call the RM.
            //
			pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;

            vmmSaveClientState(&SavedCRS);
            vmmBeginNestExecution();
            vmmSimulatePush(0);
            vmmSimulatePush(0);
            vmmSimulatePush(0);
            vmmSimulatePush(0);
            vmmSimulatePush(0);
            vmmSimulatePush(0);
            vmmSimulatePush(0);
            vmmSimulatePush(0);
            vmmSimulatePush(0);
            vmmSimulatePush(0);
            vmmSimulatePush(0xFFFF);
            vmmSimulatePush(0xFFFF);
            vmmSimulateFarCall(pOsHwInfo->dllNotifySelector, pOsHwInfo->dllNotifyOffset);
            vmmResumeExec();
            vmmEndNestExecution();
            vmmRestoreClientState(&SavedCRS);
        }
        vmmSignalSemaphore(osSemaphore);
    }
    else
    {
        //
        // Reenable chip interrupts.
        //
        REG_WR32(NV_PMC_INTR_EN_0, pmce);
    }
    //
    // Re-enable interrupts.
    //
    STI
}
BOOL CDECL osService
(
    PHWINFO pDev
)
{
    //
    // Make sure interrupts are enabled.
    //
    STI
    //
    // Service all chip functions.
    //
    mcService(pDev);
    //
    // Try scheduling callbacks here.
    //
    if (osCallbackList && !rmInCallback)
    {
        rmInCallback++;
        vmmCallPriorityVMEvent(HIGH_PRI_DEVICE_BOOST,
                               vmmGetSysVMHandle(),
                               (PEF_WAIT_FOR_STI | PEF_ALWAYS_SCHED),
                               (DWORD)pDev,
                               (DWORD)osCallback,
                               0);
    }
    //
    // Reenable chip interrupts.
    // Only if the device was NOT DISABLED.
    if (!pDev->DeviceDisabled){
        REG_WR32(NV_PMC_INTR_EN_0, pDev->Chip.IntrEn0);
    }

    return (TRUE);
}

VOID osNopInt
(
    U032 hIRQ,
    U032 hVM
)
{
    return;
}

RM_STATUS CDECL osScheduleCallback
(
    PHWINFO pDev,
    U016    dllSelector,
    U016    dllOffset,
    U032    Count,
    U032    Params,
    ...
)
{
    U032      i;
    U032      *ParamList;
    PCALLBACK CallbackInfo;
    PCALLBACK SearchCallback;
    
    if (dllSelector == 0 && dllOffset == 0)    
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: ERROR - schedule callback invalid!\n\r");
        return (RM_ERROR);
    }
    //
    // Schedule the callback at a nice time.
    //
    CLI
    CallbackInfo = osCallbackFreeList;
    if (CallbackInfo == NULL)
    {
        STI
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Unable to allocate callback structure!\n\r");
        return (RM_ERR_NO_FREE_MEM);
    }
    osCallbackFreeList     = osCallbackFreeList->Next;
    CallbackInfo->Next     = NULL;
    CallbackInfo->Selector = dllSelector;
    CallbackInfo->Offset   = dllOffset;
    CallbackInfo->Count    = Count;
    ParamList              = &Params;
    for (i = 0; i < Count; i++)
        CallbackInfo->Params[i] = *ParamList++;
    if (osCallbackList)
    {
        //
        // Insert into linked list of callbacks.
        //
        SearchCallback = osCallbackList;
        while (SearchCallback->Next)
            SearchCallback = SearchCallback->Next;
        SearchCallback->Next = CallbackInfo;
    }
    else
    {
        //
        // Schedule a callback.
        //
        osCallbackList = CallbackInfo;
    }
    STI
    return (RM_OK);
}

//
// Some customers are still using these legacy callback mechanisms.  Do not remove
//
extern CallBackDef mpCallback, mpImageCallback;
extern DWORD rmmpInCallback;
VOID osmpVbiCallback( U032 );
VOID osmpImageCallback( U032 );

VOID osmpCheckCallbacks(
    PHWINFO pDev,
    U032 dwCallbackSelect
)
{
    if (!(pDev->Video.HalInfo.CustomizationCode & CUSTOMER_CANOPUS))
        return;
// select VBI callback
if (dwCallbackSelect==0)
    {
    if (((mpCallback.Selector+mpCallback.Offset)!=0) && !rmmpInCallback)
    {
       rmmpInCallback++;
       vmmCallPriorityVMEvent(HIGH_PRI_DEVICE_BOOST,
                                   vmmGetSysVMHandle(),
                                   (PEF_WAIT_FOR_STI | PEF_ALWAYS_SCHED),
                                   (DWORD)pDev,
                                   (DWORD)osmpVbiCallback,
                                   0);
    }
    }                                      

// select image callback
if (dwCallbackSelect==1)
    {
    if (((mpImageCallback.Selector+mpImageCallback.Offset)!=0) && !rmmpInCallback)
    {
       rmmpInCallback++;
       vmmCallPriorityVMEvent(HIGH_PRI_DEVICE_BOOST,
                                   vmmGetSysVMHandle(),
                                   (PEF_WAIT_FOR_STI | PEF_ALWAYS_SCHED),
                                   (DWORD)pDev,
                                   (DWORD)osmpImageCallback,
                                   0);
    }


    }

}
extern DWORD mpImageInstalledCallback;
extern DWORD mpInstalledCallback;

VOID osmpVbiCallback
(
    // The parameter passing must be done via registers as seen below.
    U032 hVM
)
{
    CRS       SavedCRS;
    PHWINFO   pDev;

    // Load the device instance for this callback.
    _asm mov pDev, edx

    if (mpInstalledCallback)
        {
        vmmCallBackEntry();
        CLI
        vmmSaveClientState(&SavedCRS);
        vmmBeginNestExecution();
        STI
        // why do these have to be 16 bit pushes ?
        //vmmSimulatePush(dwVbiBufferNumber >> 16 );
        //vmmSimulatePush(dwVbiBufferNumber & 0xFFFF);
        vmmSimulatePush(0 >> 16 );
        vmmSimulatePush(0 & 0xFFFF);
        vmmSimulateFarCall(mpCallback.Selector, mpCallback.Offset & 0xFFFF);
        vmmResumeExec();
        CLI

        vmmEndNestExecution();
        vmmRestoreClientState(&SavedCRS);
        rmmpInCallback--;
        STI
        vmmCallBackExit(TRUE);
        }
}

VOID osmpImageCallback
(
    // The parameter passing must be done via registers as seen below.
    U032 hVM
)
{
    CRS       SavedCRS;
    PHWINFO   pDev;
    U032      DeviceInstance;

    // Load the device instance for this callback.
    _asm mov pDev, edx
    // Grab the device instance #
    DeviceInstance = pDev->DBdevInstance;

    if (mpImageInstalledCallback)
        {
        vmmCallBackEntry();
        CLI
        vmmSaveClientState(&SavedCRS);
        vmmBeginNestExecution();
        STI
        
        vmmSimulatePush(DeviceInstance >> 16);      // keyed for Canopus client
        vmmSimulatePush(DeviceInstance & 0xFFFF);
        
        // why do these have to be 16 bit pushes ?
        vmmSimulatePush(pDev->MediaPort.ImageBufferNumber >> 16 );
        vmmSimulatePush(pDev->MediaPort.ImageBufferNumber & 0xFFFF);

        vmmSimulateFarCall((U032) mpImageCallback.Selector, 
                            (U032) (mpImageCallback.Offset & 0xFFFF));
        vmmResumeExec();
        CLI

        vmmEndNestExecution();
        vmmRestoreClientState(&SavedCRS);
        rmmpInCallback--;
        STI
        vmmCallBackExit(TRUE);
        }
}


#define rdtsc __asm _emit 0x0f __asm _emit 0x31 
extern U032 vmmGetSystemTime();

// Return frequency value in MHz
U032 osGetCpuFrequency()
{
    U032 t1, t2;
    U032 tsc1, tsc2;

    // wait for timer to roll the first time
    t2 = vmmGetSystemTime();
    do {
        t1 = vmmGetSystemTime();
    } while (t1 == t2);

    // read the tsc
    _asm {
        push    eax
        push    edx
        rdtsc
        mov     DWORD PTR [tsc1], eax
        pop     edx
        pop     eax
    }

    t1 += 100;              // add 1/10 second
    while(t2 < t1) {        // wait 1/10 second
        t2 = vmmGetSystemTime();
    }

    // read the tsc again
    _asm {
        push    eax
        push    edx
        rdtsc
        mov     DWORD PTR [tsc2], eax
        pop     edx
        pop     eax
    }

    // multiply returned value by 10 since we timed 1/10th second
    // and divide the result to convert from Hz to MHz
    return (U032) ((tsc2-tsc1) * 10) / (1000000);
}

VOID osCallback
(
    // The parameter passing must be done via registers as seen below.
    U032 hVM
)
{
    U032      i;
    CRS       SavedCRS;
    PCALLBACK OldCallback;
    U032      hDevice;
    PHWINFO   pDev;

    // Load the device instance for this callback.
    _asm mov hDevice, edx

    pDev = NvDBPtr_Table[hDevice];
    //DeviceInstance = hDevice;

    vmmCallBackEntry();    
    CLI
//    rmInCallback = TRUE;
#if DEBUG
    if (osCallbackList == NULL)
    {
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: NULL Callback Callback List!\n\r");
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      Add more structures.\n\r");
    }
#endif    
    vmmSaveClientState(&SavedCRS);
    vmmBeginNestExecution();
    while (osCallbackList)
    {
        U032 Selector = osCallbackList->Selector;
        U032 Offset = osCallbackList->Offset;
    
        STI
        for (i = 0; i < osCallbackList->Count; i++)
        {
            vmmSimulatePush(osCallbackList->Params[i] >> 16);
            vmmSimulatePush(osCallbackList->Params[i] & 0xFFFF);
        }
//        vmmSimulateFarCall(osCallbackList->Selector, osCallbackList->Offset);
        vmmSimulateFarCall(Selector, Offset);
        vmmResumeExec();
        CLI
        //
        // Move to next callback in the pending list.
        //
        OldCallback    = osCallbackList;
        osCallbackList = osCallbackList->Next;
        //
        // Stick the old callback structure back on the free list.
        //
        OldCallback->Next  = osCallbackFreeList;
        osCallbackFreeList = OldCallback;
    }
    vmmEndNestExecution();
    vmmRestoreClientState(&SavedCRS);
    rmInCallback--;
    STI
    vmmCallBackExit(TRUE);
}

// 
//
VOID osDisplayModeHook
(
    PHWINFO pDev,
    U032    msg
)
{
    //
    // If the Win9X display driver has asked for state change
    // notification, call him now
    //
    if (pDev->DBdisplayModeHook)
    {
        // This needed to be modified due to the incomplete 
        // _asm support in MS tools.
        //(*pDev->DBdisplayModeHook)(msg);
        VOID *x;
        x = pDev->DBdisplayModeHook;
        __asm {
            push    edi
            mov     eax, msg
            mov     edi, x
            call    edi
            pop     edi
        } 
    }
} // end of osDisplayModeHook

//
// Perform any os-specific DAC state updates that need to occur
// when the DAC/CRTC has been updated.
//
VOID osStateDacEx
(
    PHWINFO pDev,
    U032    msg
)
{
#if 0   // this should be in osPostModeSet
	PWIN9XHWINFO pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;

    //
    // Notify the Win9X minivdd that a mode change has probably occurred.  This allows
    // it to save any new register states
    //
    if (pOsHwInfo->miniVDDPostModeChange)  // if primary display, we got callback at init
        (*pOsHwInfo->miniVDDPostModeChange)();
#endif
}

//  Set the FilterEnable value for TV flicker filtering
//  This checks the registry for an override 
//
VOID osStateFbEx
(
    PHWINFO pDev
)
{
U032    data32;
// To Do:
// we probably want to return a value (0,1, or 2) representing the amount of filtering.
// for now we just check for the string.
// we should be able to override on as well as off. This means don't check monitor.type, just check range
// on data32 and copy 0,1, or 2 to FilterEnable.
    if (osReadRegistryDword(pDev,strDevNodeRM, "FilterOverride", &data32) == RM_OK)  // TV filter?
    {
        if (GETMONITORTYPE(pDev, 0) != MONITOR_TYPE_VGA)
            pDev->Framebuffer.FilterEnable = 2;
    } 
    else 
    {
        //
        // Otherwise, make sure we get a default value in during initialization
        //
        if (GETMONITORTYPE(pDev, 0) == MONITOR_TYPE_VGA)
            pDev->Framebuffer.FilterEnable = 0;
        else
            pDev->Framebuffer.FilterEnable = 2;
    }

    // Tiling is automatically done in the RM for most modes. This key allows us
    // to disable the auto tiling feature. Once enabled or disabled, it stays that way
    // till this registry key is changed. There is also a programmatic way of 
    // enabling and disabling auto tiling (ConfigSetEx). Right now, these two methods
    // can be freely mixed. i.e, the operation performed by the programmatic interface
    // can be undone by the registry flag and vice versa.
    if (osReadRegistryDword(pDev,strDevNodeRM, "AutoTilingOverride", &data32) == RM_OK) 
    {
        if (data32 == 0)
        {
            // Disable auto tiling.
            nvHalFbControl(pDev, FB_CONTROL_TILING_DISABLE);
        }
        else
        {
            // Enable tiling.
            nvHalFbControl(pDev, FB_CONTROL_TILING_ENABLE);
        }
    } // Auto tiling override
}

// Prepare the pDev for the new style mode set.
// This should be called by the legacy mode set flow only.
//
VOID osSetupForNewStyleModeSet(PHWINFO pDev)
{
#if 0 // this is an archaic, non-architectural function
	PWIN9XHWINFO pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;
    RM_STATUS status;
    DISPLAYINFO tmpDisplayInfo;
    BOOL    optimalRefreshSelected;
    U032    monitorTimingMode;
    U032    UseDMTFlag;
    U032    tmpDMTFlag;
    BOOL    monitorIsGTF;
    U032    data32;
    MODE_TIMING_VALUES timingValues;
    unsigned int vesaStatus;
    BOOL arbitrationValidated;
    U032 numRefreshRates;
    U032 nextLower;
    int i;

    REG_WR08(NV_PRMCIO_CRX__COLOR, 0x571F & 0xFF); 
    REG_WR08(NV_PRMCIO_CRX__COLOR+1, (0x571F >> 8) & 0xFF);
    ////OEMEnableExtensions();          // Unlock the registers before DDC
    EDIDRead(pDev, 0, GETDISPLAYTYPE(pDev, 0));
    REG_WR08(NV_PRMCIO_CRX__COLOR, 0x991F & 0xFF); 
    REG_WR08(NV_PRMCIO_CRX__COLOR+1, (0x991F >> 8) & 0xFF);
    ////OEMDisableExtensions();         // Lock the registers

    // Figure out what monitor timing mode we want. It is controlled by a registry flag "MonitorTiming".
    monitorTimingMode = 0; // By default, read the EDID of the monitor.
    if (osReadRegistryDword(pDev,pDev->Registry.DBstrDevNodeDisplay, strMonitorTiming, &data32) == RM_OK)
    {
        monitorTimingMode = data32;
    }
    switch (monitorTimingMode)
    {
        case 1:
            // Use DMT.
            UseDMTFlag = TRUE;
            break;
        case 2:
            // Use GTF.
            UseDMTFlag = FALSE;
            break;
        case 3:
            // Legacy mode.
            // Read the legacy DMT Override flag to see if we should use DMT. Otherwise use GTF.
            UseDMTFlag = FALSE;
            if (osReadRegistryDword(pDev,pDev->Registry.DBstrDevNodeDisplay, strDMTOverride, &data32) == RM_OK)
            {
                if (data32 == 1)
                    UseDMTFlag = TRUE;
            }
            break;
        default:
            // Automatically set it by reading the EDID.
            UseDMTFlag = TRUE; // default to DMT
            if (EDIDIsMonitorGTF(pDev, &monitorIsGTF) == RM_OK)
            {
                UseDMTFlag = (monitorIsGTF) ? FALSE : TRUE;
            }
            break;
            // 
    }

    //
    // If there's an override setting for the refresh rate, use it instead
    //
    if (osReadRegistryDword(pDev,strDevNodeRM, strRefreshOverride, &data32) == RM_OK)
        pDev->Framebuffer.HalInfo.RefreshRate = data32;

    //
    // The GTF timings for 640x400 at 70Hz and 72Hz are drastically different than
    // the original VGA timings (which officially only list 85Hz as a support rate
    // at this resolution).  Therefore, when we attempt to set the GTF rates the
    // monitors just aren't happy.
    //
    // We have two choices.  Tweak our 70Hz and 72Hz GTF numbers to mimic VGA, or
    // just use IBM VGA (70Hz) for these two rates.  Since we're doing it right, and
    // the monitors just aren't happy, I vote for the latter.
    //
    // Remember that IBM VGA 640x400 starts at 70Hz, not 60Hz.
    //
    if ((pDev->Framebuffer.HalInfo.HorizDisplayWidth == 640) && (pDev->Framebuffer.HalInfo.VertDisplayWidth == 400)
        && (pDev->Framebuffer.HalInfo.RefreshRate < 75))
            pDev->Framebuffer.HalInfo.RefreshRate = 60; // this will kick in BIOS default values (70Hz)

    //
    // NV4 limits for hires modes at 32bpp.  Windows will still send down optimal values larger
    // than the limits we placed in our INF.  Don't let them through.
    //
    if (!IsNV5orBetter(pDev))
    {    
        if ((pDev->Framebuffer.HalInfo.HorizDisplayWidth == 1280) && (pDev->Framebuffer.HalInfo.VertDisplayWidth == 1024)
            && (pDev->Dac.HalInfo.Depth >= 24)
            && (pDev->Framebuffer.HalInfo.RefreshRate > 100))
                pDev->Framebuffer.HalInfo.RefreshRate = 100; 
        
        if ((pDev->Framebuffer.HalInfo.HorizDisplayWidth == 1600) && (pDev->Framebuffer.HalInfo.VertDisplayWidth == 1200)
            && (pDev->Dac.HalInfo.Depth >= 24)
            && (pDev->Framebuffer.HalInfo.RefreshRate > 75))
                pDev->Framebuffer.HalInfo.RefreshRate = 75; 
        
        if ((pDev->Framebuffer.HalInfo.HorizDisplayWidth == 1920) && (pDev->Framebuffer.HalInfo.VertDisplayWidth == 1080)
            && (pDev->Dac.HalInfo.Depth >= 24)
            && (pDev->Framebuffer.HalInfo.RefreshRate > 72))
                pDev->Framebuffer.HalInfo.RefreshRate = 72; 
        
        if ((pDev->Framebuffer.HalInfo.HorizDisplayWidth == 1920) && (pDev->Framebuffer.HalInfo.VertDisplayWidth == 1200)
            && (pDev->Dac.HalInfo.Depth >= 24)
            && (pDev->Framebuffer.HalInfo.RefreshRate > 60))
                pDev->Framebuffer.HalInfo.RefreshRate = 60; 
    }                
            
    //
    // There is a hardware bug in NV4's arbitration hardware having to do with memory refresh and
    // page misses.  This shows its ugly head during 64bit operation (8meg SDRAM).  If we see
    // a pixelclock setting above ~160Hz in 32bpp, drop it down to 60Hz.
    //
    // The user may have selected a different refresh rate (since the INF allows it), but they're
    // going to get 60Hz.  Sorry.
    //            
    // The highest resolution in 8meg is 1600x1200x32bpp, so just watch for that
    //
    if ((pDev->Chip.HalInfo.MaskRevision == NV_PMC_BOOT_0_MASK_REVISION_A)
        && (pDev->Dac.HalInfo.InputWidth == 64)
        && (pDev->Framebuffer.HalInfo.RamSizeMb == 8)
        && (pDev->Framebuffer.HalInfo.HorizDisplayWidth == 1600) 
        && (pDev->Framebuffer.HalInfo.VertDisplayWidth == 1200)
        && (pDev->Dac.HalInfo.Depth >= 24)
        && (pDev->Framebuffer.HalInfo.RefreshRate > 60))
            pDev->Framebuffer.HalInfo.RefreshRate = 60; 

    //
    // Check if "optimal" refresh rate was chosen. 
    // When "optimal" is chosen, both min and max refresh rates in DISPLAYINFO will have non-zero values. 
    // When a specific refresh rate is chosen, max will have that value and the min will have zero. 
    // When "default" is chosen, both min and max will be zero.
    //    
    tmpDisplayInfo.HdrSize = sizeof(DISPLAYINFO);
    vddGetDisplayInfo(pOsHwInfo->osDeviceHandle, &tmpDisplayInfo, sizeof(DISPLAYINFO));
    pDev->Framebuffer.RefreshRateType = NV_CFG_REFRESH_FLAG_SPECIFIC; // assume specific 
    if ((tmpDisplayInfo.RefreshRateMax != 0) && (tmpDisplayInfo.RefreshRateMin != 0))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "OPTIMAL REFRESH!!\n\r");

        // Save the fact that its optimal so we can inform the control panel
        pDev->Framebuffer.RefreshRateType = NV_CFG_REFRESH_FLAG_OPTIMAL; 
        // Try to get the max frequency for this mode from the EDID. 
        // If we get a refresh rate that is less than what we have now,
        // we will use the lower value.
        if (osReadRegistryDword(pDev,pDev->Registry.DBstrDevNodeDisplay, strOptimalNotFromEDID, &data32) != RM_OK)
        {
            // No registry entry that prohibits us from looking in the EDID.
            status = EDIDGetMaxRefreshRate(pDev, pDev->Framebuffer.HalInfo.HorizDisplayWidth, 
                                           pDev->Framebuffer.HalInfo.VertDisplayWidth,
                                           &data32);
            if (status == RM_OK     && 
                data32 != 0 &&
                data32 < pDev->Framebuffer.HalInfo.RefreshRate)
            {
                pDev->Framebuffer.HalInfo.RefreshRate = data32;
            }
        } // Get optimal from EDID

        // Read the registry to see if there is a limit for the "optimal" refresh rate.
        if (osReadRegistryDword(pDev,pDev->Registry.DBstrDevNodeDisplay, strMaxOptimalRefreshRate, &data32) == RM_OK)
        {
            // Ignore the value read if it is less than 60Hz.
            if (data32 >= 60 &&
                data32 < pDev->Framebuffer.HalInfo.RefreshRate)
            {
                pDev->Framebuffer.HalInfo.RefreshRate = data32;
            }
        }
        
    } // else check if "default" refresh rate chosen
    else if ((tmpDisplayInfo.RefreshRateMax == 0) && (tmpDisplayInfo.RefreshRateMin == 0))
    {
        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "ADAPTER DEFAULT REFRESH!!\n\r");
        
        // Save the fact that its default so we can inform the control panel
        pDev->Framebuffer.RefreshRateType = NV_CFG_REFRESH_FLAG_ADAPTER_DEFAULT; 
        pDev->Framebuffer.HalInfo.RefreshRate = 60;
    }        

    //
    // Attempt to workaround a Win95 bug with OPTIMAL and some Plug-n-Play monitors.
    //
    // When the user selects OPTIMAL from the control panel, the call to the VDD returns values to
    // the driver that look similar to a specific rate request (max != 0, min == 0) rather than 
    // looking like an OPTIMAL selection (max != 0, min != 0).
    //
    // Because of this misidentification of OPTIMAL, the MaxOptimalRefresh registry flag is ignored
    // and the rate goes higher than we want.   
    //
    // First check if we're on Win95 (CONFIG_MGR version 0x400)
    //
    if (CONFIGMG_Get_Version() < 0x40A)
    {
        U032 foundRate = 0;
        U032 optimalflag = 0;
        char config_settings[] = "CONFIG\\0000\\Display\\Settings";
        char config_key[] = "RefreshRate";
        U032 i;
        
        //
        // For safety, only check for this condition if there is a MaxOptimalRefresh flag present
        //
        if (osReadRegistryDword(pDev,pDev->Registry.DBstrDevNodeDisplay, strMaxOptimalRefreshRate, &data32) == RM_OK)
        {
            //
            // This is ugly, but look for the RefreshRate key in the HKLM\Config\000X\Display\...
            //
            // Remember, since this is Win95, we don't have to worry about multimonitor
            //
            for (i = 0; (i <= 9) && !foundRate; i++)
            {
                config_settings[10] = '0' + i;
            
                if (osReadRegistryDword(pDev,config_settings, config_key, &optimalflag) == RM_OK)
                    foundRate = 1;
            }                    
            
            //
            // We found a RefreshRate key in the display config.  Check if -1 (OPTIMAL)
            //
            if ((foundRate) && (((optimalflag & 0xFF) == '-') && (((optimalflag >> 8) & 0xFF) == '1')))
            {
                pDev->Framebuffer.RefreshRateType = NV_CFG_REFRESH_FLAG_OPTIMAL; 
                // Ignore the value read if it is less than 60Hz.
                if (data32 >= 60 &&
                    data32 < pDev->Framebuffer.HalInfo.RefreshRate)
                {
                    pDev->Framebuffer.HalInfo.RefreshRate = data32;
                }
            }
            
            //
            // VX700 workaround.  The Win95 PlugNPlay monitor INF incorrectly believes this particular
            // monitor can support up to 85Hz at 16x12.  Therefore the user is given the choice of
            // all possible rates up to 85.  The problem is that the monitor only supports up to
            // 65Hz.  Attempt to manually detect this specific monitor (by name, by detailed 16x12 timing)
            // and limit 16x12 to 60Hz in all cases. 
            //
            {
                U032 offset;
                
                offset = EDID_V1_DETAILED_TIMINGS_START_INDEX + (18 * 2); // DETAILED BLOCK3
                
                // Look for VX700 descriptor
                if ((pDev->EDIDBuffer[offset+5] == 'V') &&
                    (pDev->EDIDBuffer[offset+6] == 'X') &&
                    (pDev->EDIDBuffer[offset+7] == '7') &&
                    (pDev->EDIDBuffer[offset+8] == '0') &&
                    (pDev->EDIDBuffer[offset+9] == '0'))
                {
             
                    offset = EDID_V1_DETAILED_TIMINGS_START_INDEX; // DETAILED BLOCK1
                
                    // Look for 173.0 MHz timing at 1600x1200
                    if ((pDev->EDIDBuffer[offset+0] == 0x94) &&   // here's the MHz
                        (pDev->EDIDBuffer[offset+1] == 0x43) &&
                        (pDev->EDIDBuffer[offset+2] == 0x40) &&   // here's the active horiz
                        (pDev->EDIDBuffer[offset+4] == 0x62))
                    {
                    
                        // Look for 1600x1200 requested resolution
                        if ((pDev->Framebuffer.HalInfo.HorizDisplayWidth == 1600) && 
                            (pDev->Framebuffer.HalInfo.VertDisplayWidth == 1200))
                            pDev->Framebuffer.HalInfo.RefreshRate = 60; 
                    
                    }
                }
            }
             
            //
            // VX900 workaround.  The Win95 PlugNPlay monitor INF incorrectly believes this particular
            // monitor can support up to 85Hz at 16x12.  Therefore the user is given the choice of
            // all possible rates up to 85.  The problem is that the monitor only supports up to
            // 75Hz.  Attempt to manually detect this specific monitor (by name, by detailed 16x12 timing)
            // and limit 16x12 to 75Hz in all cases. 
            //
            {
                U032 offset;
                
                offset = EDID_V1_DETAILED_TIMINGS_START_INDEX + (18 * 2); // DETAILED BLOCK3
                
                // Look for VX900 descriptor
                if ((pDev->EDIDBuffer[offset+13] == 'V') &&
                    (pDev->EDIDBuffer[offset+14] == 'X') &&
                    (pDev->EDIDBuffer[offset+15] == '9') &&
                    (pDev->EDIDBuffer[offset+16] == '0') &&
                    (pDev->EDIDBuffer[offset+17] == '0'))
                {
             
                    offset = EDID_V1_DETAILED_TIMINGS_START_INDEX; // DETAILED BLOCK1
                
                    // Look for 202.50 MHz timing at 1600x1200
                    if ((pDev->EDIDBuffer[offset+0] == 0x1A) &&   // here's the MHz
                        (pDev->EDIDBuffer[offset+1] == 0x4F) &&
                        (pDev->EDIDBuffer[offset+2] == 0x40) &&   // here's the active horiz
                        (pDev->EDIDBuffer[offset+4] == 0x62))
                    {
                    
                        // Look for 1600x1200 requested resolution
                        if ((pDev->Framebuffer.HalInfo.HorizDisplayWidth == 1600) && 
                            (pDev->Framebuffer.HalInfo.VertDisplayWidth == 1200) &&
                            (pDev->Framebuffer.HalInfo.RefreshRate > 75))
                            pDev->Framebuffer.HalInfo.RefreshRate = 75; 
                    
                    }
                }
            }
             
            //
            // EV500/EV700 workaround.  The Win95 PlugNPlay monitor INF incorrectly believes this particular
            // monitor can support up to 100Hz at 10x7.  Therefore the user is given the choice of
            // all possible rates up to 100.  The problem is that the monitor only supports up to
            // 85Hz.  Attempt to manually detect this specific monitor (by name, by detailed 10x7 timing)
            // and limit 10x7 to 85Hz in all cases. 
            //
            {
                U032 offset;
                U032 block;
                
                offset = 0;
                
                // Look for the Gateway mfg descriptor
                if ((pDev->EDIDBuffer[offset+8] == 0x1E) &&
                    (pDev->EDIDBuffer[offset+9] == 0xF9))
                {
             
                    for (block = 0; block < 4; block++)
                    {
                        offset = EDID_V1_DETAILED_TIMINGS_START_INDEX + (18 * block); // DETAILED BLOCK N
                
                        // Look for 94.50 MHz timing at 1024x768; this translates to 85Hz
                        if ((pDev->EDIDBuffer[offset+0] == 0xEA) &&   // here's the MHz
                            (pDev->EDIDBuffer[offset+1] == 0x24) &&
                            (pDev->EDIDBuffer[offset+2] == 0x00) &&   // here's the active horiz
                            (pDev->EDIDBuffer[offset+4] == 0x41))
                        {
                        
                            // Look for 1024x768 requested resolution, cap to 85Hz
                            if ((pDev->Framebuffer.HalInfo.HorizDisplayWidth == 1024) && 
                                (pDev->Framebuffer.HalInfo.VertDisplayWidth == 768) &&
                                (pDev->Framebuffer.HalInfo.RefreshRate > 85))
                                pDev->Framebuffer.HalInfo.RefreshRate = 85; 
                        
                        }
                    }                        
                }
            }
            
        }
    }        
    
    //
    // Safety: make sure we aren't working off a bogus refresh rate.  Cap us to some silly limit.
    //
    if (pDev->Framebuffer.HalInfo.RefreshRate > 1000)
        pDev->Framebuffer.HalInfo.RefreshRate = 60;
    
    //
    // If there's max refresh rate, make sure we don't exceed that.
    //
    if (osReadRegistryDword(pDev,pDev->Registry.DBstrDevNodeDisplay, strMaxRefreshRate, &data32) == RM_OK)
    {
        // Ignore the value read if it is less than 60Hz.
        if (data32 >= 60 &&
            pDev->Framebuffer.HalInfo.RefreshRate > data32)
        {
            pDev->Framebuffer.HalInfo.RefreshRate = data32;
        }
    }

    //
    // Most monitors can support the vert range of 800x600x150Hz but cannot support the
    // horiz range.  We removed 150Hz from the INF, but in case Windows tries it anyway,
    // override with one lower rate.
    //
    if ((pDev->Framebuffer.HalInfo.HorizDisplayWidth == 800) && (pDev->Framebuffer.HalInfo.VertDisplayWidth == 600) 
        && (pDev->Framebuffer.HalInfo.RefreshRate > 144) && (pDev->Framebuffer.HalInfo.RefreshRate < 170))
    {
        pDev->Framebuffer.HalInfo.RefreshRate = 144;
    }                


    //
    // The documented method for Windows to select refresh rates for a display/monitor
    // combination is to take the intersection of the monitor INF entries (EDID) and
    // the display device's INF values.  This will then dictate the rates that are
    // valid for both devices.
    //
    // Unfortunately, it seems Microsoft didn't follow their own documentation in all
    // cases.  If a user selects OPTIMAL from the refresh rate menu, Windows will ask
    // us to set a refresh rate that matches the monitor information.  It doesn't matter
    // if the rate is beyond the display device's INF entries.
    //
    // Due to clocking differences between all of our shipping chips, we need to
    // validate all attempted refresh rates.  If a rate is beyond our device's limits
    // based on memory and clock variables, we need to attempt one refresh rate lower.
    // Luckily, the user has selected OPTIMAL and doesn't know exactly what rate she
    // is choosing.  If we drop it by one or two values, it doesn't really matter.
    //

    if (pDev->Framebuffer.HalInfo.RefreshRate == 0) // we'll be here an awfully long time if 
        pDev->Framebuffer.HalInfo.RefreshRate = 60; // refresh is set to 0

    arbitrationValidated = FALSE;
    while (!arbitrationValidated)
    {
        tmpDMTFlag = UseDMTFlag;
        if ((pDev->Framebuffer.HalInfo.HorizDisplayWidth == 640) && 
            (pDev->Framebuffer.HalInfo.VertDisplayWidth == 480)  &&
            (pDev->Framebuffer.HalInfo.RefreshRate == 60))
        {
            // force 640x480 @60Hz into DMT mode.
            tmpDMTFlag = UseDMTFlag = TRUE;
        }

        if (tmpDMTFlag)
        {
            // Get DMT timings for this mode. 
            vesaStatus = vesaGetDMTTimings(pDev->Framebuffer.HalInfo.HorizDisplayWidth, 
                                           pDev->Framebuffer.HalInfo.VertDisplayWidth, 
                                           pDev->Framebuffer.HalInfo.RefreshRate,
                                           &timingValues);
            // If we failed to get DMT timings for this mode, force GTF
            if (vesaStatus != 0)
            {
                tmpDMTFlag = FALSE;
            }
        }

        if (!tmpDMTFlag)
        {
            // Get the GTF timings
            vesaStatus = vesaGetGTFTimings(pDev->Framebuffer.HalInfo.HorizDisplayWidth, 
                                           pDev->Framebuffer.HalInfo.VertDisplayWidth, 
                                           pDev->Framebuffer.HalInfo.RefreshRate,
                                           &timingValues);
        }

        if (!vesaStatus)        
        {
            //
            // We found a rate match, so check if we can set valid arbitration for a clean screen
            //
            nvHalDacValidateArbSettings(pDev, 
                                        pDev->Dac.HalInfo.Depth, 
                                        0, 0, 
                                        (U032)(timingValues.PixelClock * 10000),
                                        0, &arbitrationValidated);
        }
        
        //
        // If both DMT and GTF lookups failed, we don't have a rate that matches the request.  Drop the
        // rate by one and try again.  Don't go below some really low number (say 45).
        //
        // Same thing goes for a rate that doesn't pass arbitration.  Drop on down.
        //
        if (!arbitrationValidated || vesaStatus)
        {
            //
            // Drop it, but don't go too low
            //
            if (--pDev->Framebuffer.HalInfo.RefreshRate < 45)
            {
                pDev->Framebuffer.HalInfo.RefreshRate = 60;     // just assume 60Hz on a total failure
                pDev->Framebuffer.HalInfo.HorizDisplayWidth = 640;
                pDev->Framebuffer.HalInfo.VertDisplayWidth = 480;
                
                vesaStatus = vesaGetDMTTimings(pDev->Framebuffer.HalInfo.HorizDisplayWidth, 
                                               pDev->Framebuffer.HalInfo.VertDisplayWidth, 
                                               pDev->Framebuffer.HalInfo.RefreshRate,
                                               &timingValues);
                                               
                arbitrationValidated = TRUE;            //  and get out of here
            }
        
        
        }

    } // while arbitration is not validated.

    // Save off the timing values in the pDev.
    pDev->Dac.HorizontalVisible       = timingValues.HorizontalVisible;
    pDev->Dac.HorizontalBlankStart    = timingValues.HorizontalBlankStart;
    pDev->Dac.HorizontalRetraceStart  = timingValues.HorizontalRetraceStart;
    pDev->Dac.HorizontalRetraceEnd    = timingValues.HorizontalRetraceEnd;
    pDev->Dac.HorizontalBlankEnd      = timingValues.HorizontalBlankEnd;
    pDev->Dac.HorizontalTotal         = timingValues.HorizontalTotal;
    pDev->Dac.VerticalVisible         = timingValues.VerticalVisible;
    pDev->Dac.VerticalBlankStart      = timingValues.VerticalBlankStart;
    pDev->Dac.VerticalRetraceStart    = timingValues.VerticalRetraceStart;
    pDev->Dac.VerticalRetraceEnd      = timingValues.VerticalRetraceEnd;
    pDev->Dac.VerticalBlankEnd        = timingValues.VerticalBlankEnd;
    pDev->Dac.VerticalTotal           = timingValues.VerticalTotal;
    pDev->Dac.PixelDepth              = pDev->Dac.HalInfo.Depth;
    pDev->Dac.HalInfo.PixelClock      = timingValues.PixelClock;
    pDev->Dac.HorizontalSyncPolarity  = timingValues.HSyncpolarity;
    pDev->Dac.VerticalSyncPolarity    = timingValues.VSyncpolarity;
    pDev->Dac.DoubleScannedMode       = (pDev->Framebuffer.HalInfo.VertDisplayWidth < 400) ? TRUE : FALSE;
#endif
} // end of osSetupForNewStyleModeSet

// Os Specifix pre mode set actions.
VOID osPreModeSet(PHWINFO pDev, U032 Head)
{
    U032 DisplayType, data32;
    U032 PrimaryHead;
    PVIDEO_LUT_CURSOR_DAC_OBJECT pVidLutCurDac;

    // Check for registry override for display type (TV/MONITOR/FLAT_PANEL)
    // Then check that the device is actually connected. If not, find one that is.

    // Get current display type for this head.
    DisplayType = GETDISPLAYTYPE(pDev, Head);

    // Determine which Head is the PrimaryDevice
    for (PrimaryHead = 0; PrimaryHead < pDev->Dac.HalInfo.NumCrtcs; PrimaryHead++)
    {
        if (pDev->Dac.CrtcInfo[PrimaryHead].PrimaryDevice == TRUE)
        {
            break;
        }
    }

    //
    // Canopus-specific code
    //
    if (pDev->Video.HalInfo.CustomizationCode & CUSTOMER_CANOPUS)
    {
        if (DisplayType==DISPLAY_TYPE_MONITOR) {
            //if(!dacMonitorConnectStatus(pDev)&&dacFlatPanelConnectStatus(pDev))
            //  DisplayType = DISPLAY_TYPE_FLAT_PANEL;
        }
        else {
            if (dacMonitorConnectStatus(pDev, Head)&&!dacFlatPanelConnectStatus(pDev, Head)) {
                DisplayType = DISPLAY_TYPE_MONITOR;
                SETDISPLAYTYPE(pDev, Head, DisplayType);
            }
        }
    }            

    //
    // Reset TV output format value from registry.
    //
    if (dacReadTVOutFromRegistry(pDev, Head, &data32) == RM_OK)
        pDev->Dac.TVoutput = data32;

    // Set the DisplayChanged flag so we can signal the miniVDD that we are no longer on the same display.
    if (Head == PrimaryHead)    // only need to signal for primary
    {                           // if the display type in the object is different from the current display type (in HalInfo)
        pVidLutCurDac = (PVIDEO_LUT_CURSOR_DAC_OBJECT) pDev->Dac.CrtcInfo[Head].pVidLutCurDac;
        if (GETDISPLAYTYPE(pDev, Head) != pVidLutCurDac->DisplayType)
            pDev->Dac.DisplayChanged = 1;
        pDev->Dac.CurrentDisplay = GETDISPLAYTYPE(pDev, Head);
    }
}

// Os Specifix post mode set actions.
VOID osPostModeSet(PHWINFO pDev, U032 Head)
{
    PWIN9XHWINFO pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;
    RM_STATUS status;
    U032      data32;
    U032      PrimaryHead;

    // Determine which Head is the PrimaryDevice
    for (PrimaryHead = 0; PrimaryHead < pDev->Dac.HalInfo.NumCrtcs; PrimaryHead++)
    {
        if (pDev->Dac.CrtcInfo[PrimaryHead].PrimaryDevice == TRUE)
        {
            break;
        }
    }

    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV)
        dacSetBiosDefaultTVType(pDev, pDev->Dac.TVStandard);

    //
    // Notify the Win9X minivdd that a mode change has probably occurred. This allows
    // it to save any new register states. This is only done for the PrimaryDevice.
    //
    if (Head == PrimaryHead)
    {
        if (pOsHwInfo->miniVDDPostModeChange)  // if primary display, we got callback at init
            if (pDev->Dac.DisplayChanged)
                (*pOsHwInfo->miniVDDPostModeChange)(1);    // signal display change
            else
                (*pOsHwInfo->miniVDDPostModeChange)(0);
        pDev->Dac.DisplayChanged = 0;

        //
        // At the completetion of this modeset, check if there's been a modeset
        // on the other head, if not, call dacDisableDac on that head.
        //
        if (pDev->Dac.CrtcInfo[Head^1].pVidLutCurDac == (VOID_PTR)NULL)
            dacDisableDac(pDev, Head^1);
    }

    // See if there are any registry overrides for the desktop positioning.
    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_MONITOR)
    {
        U032 cbLen;
        NV_CFGEX_SET_DESKTOP_POSITION_MONITOR_PARAMS position;

        cbLen = 4 * sizeof(U032);
        status = dacReadDesktopPositionFromRegistry(pDev, Head,
                                                    TRUE,  // isMonitor
                                                    (U008 *) &(position.HRetraceStart),
                                                    &cbLen);
        if (status == RM_OK)
        {
            // Program the new values.
            dacSetMonitorPosition(pDev, Head, 
                                  position.HRetraceStart, position.HRetraceEnd,
                                  position.VRetraceStart, position.VRetraceEnd);
        }
    }
    // See if there are any registry overrides for the monitor timing.
    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_MONITOR)
    {
        NV_CFGEX_CRTC_TIMING_PARAMS timings;

        //
        // First check if there are timings available in the registry for this mode
        //
        timings.Head = Head;
        timings.Reg = NV_CFGEX_CRTC_TIMING_REGISTRY;
        status = dacGetCRTCTiming(pDev, Head, &timings);

        if (status == RM_OK)
        {
            //
            // Program the new values
            //
            timings.Reg = NV_CFGEX_CRTC_TIMING_REGISTER;
            status = dacSetCRTCTiming(pDev, Head, &timings);
        }

    }
    // See if there are any registry overrides for the TV desktop positioning.
    if (GETDISPLAYTYPE(pDev, Head) == DISPLAY_TYPE_TV) 
    {
        U032 cbLen;
        NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS Params;

        // Read the registry to get any updates to the default setting.
        // This should be done after dacAdjustCRTCForTV because it involves changes to CRTC.
        cbLen = sizeof(NV_CFGEX_DESKTOP_POSITION_TV_MULTI_PARAMS);
        status = dacReadTVDesktopPositionFromRegistry(pDev, Head, (U008 *) &Params, &cbLen);

        // Make sure data is valid for the current encoder.
        if ((status == RM_OK) && (Params.Encoder_ID == pDev->Dac.EncoderType))
        {
            // Write the saved settings
            dacSetTVPosition(pDev, Head, (U032 *) &Params);
        }
        if (!pDev->Power.MobileOperation)
        {
            dacWriteTVStandardToRegistry(pDev, pDev->Dac.TVStandard);   // save TV standard in registry
        }

    }
    
    // Check/Set any PFB overrides
    if (osReadRegistryDword(pDev,strDevNodeRM, strMemoryOverride, &data32) == RM_OK)
    {
        nvHalFbLoadOverride(pDev, FB_LOAD_OVERRIDE_MEMORY, data32);
    }
    if (osReadRegistryDword(pDev,strDevNodeRM, strRTLOverride, &data32) == RM_OK)
    {
        nvHalFbLoadOverride(pDev, FB_LOAD_OVERRIDE_RTL, data32);
    }
    else
    {
        nvHalFbLoadOverride(pDev, FB_LOAD_OVERRIDE_RTL, 0);
    }

    //
    // If there are graphics debug overrides, use them now
    //
    if (osReadRegistryDword(pDev,strDevNodeRM, strGraphicsDebug0, &data32) == RM_OK)
    {
        nvHalGrLoadOverride(pDev, GR_LOAD_OVERRIDE_DEBUG0, data32);
    }
    if (osReadRegistryDword(pDev,strDevNodeRM, strGraphicsDebug1, &data32) == RM_OK)
    {
        nvHalGrLoadOverride(pDev, GR_LOAD_OVERRIDE_DEBUG1, data32);
    }
    if (osReadRegistryDword(pDev,strDevNodeRM, strGraphicsDebug2, &data32) == RM_OK)
    {
        nvHalGrLoadOverride(pDev, GR_LOAD_OVERRIDE_DEBUG2, data32);
    }
    if (osReadRegistryDword(pDev,strDevNodeRM, strGraphicsDebug3, &data32) == RM_OK)
    {
        nvHalGrLoadOverride(pDev, GR_LOAD_OVERRIDE_DEBUG3, data32);
    }
    
    //
    // Use the display cursor cache override if available
    //
    if (osReadRegistryDword(pDev,pDev->Registry.DBstrDevNodeDisplay, strCursorCacheOverride, &data32) == RM_OK)
    {
        if (data32 == 0)
            pDev->Dac.CursorCacheEnable = 0;
        else            
            pDev->Dac.CursorCacheEnable = 1;
    }
} // end of osPostModeSet

// Registry access functions.
RM_STATUS osReadRegistryBinary
(
    PHWINFO pDev,
    char *regDevNode,
    char *regParmStr,
    U008 *Data,
    U032 *cbLen
)
{
    U032 cbType;
    U032 hkSW;
    U032 ret;

    //
    // Get access to specified Device Node in the registry.
    //
    if (VMM_RegOpenKey(HKEY_LOCAL_MACHINE, regDevNode, (PVMMHKEY)&hkSW) != 0)
        return (RM_ERROR);

    if (hkSW)
    {
        cbType = REG_BINARY;

        // Is this a request to read Global Data ?
        if(regDevNode == strDevNodeRM){
            // Then just read from the global location.
            ret = VMM_RegQueryValueEx(hkSW, regParmStr, NULL, &cbType, (PBYTE)Data, cbLen);
            if (ret != 0L){
                VMM_RegCloseKey(hkSW);
                return (RM_ERROR);
            }
        }else{
            // First attempt to read the data value from the Global area.
            // Then go to the device specific location.
            ret = VMM_RegQueryValueEx(hkSW, strDevNodeRM, NULL, &cbType, (PBYTE)Data, cbLen);
            if (ret != 0L){
                ret = VMM_RegQueryValueEx(hkSW, regParmStr, NULL, &cbType, (PBYTE)Data, cbLen);
                if (ret != 0L){
                    VMM_RegCloseKey(hkSW);
                    return (RM_ERROR);
                }
            }

        }

        VMM_RegCloseKey(hkSW);
    }
    return (RM_OK);
} // end of osReadRegistryBinary

RM_STATUS osWriteRegistryBinary
(
    PHWINFO pDev,
    char *regDevNode,
    char *regParmStr,
    U008 *Data,
    U032 cbLen
)
{
    U032 cbType;
    U032 hkSW;
    U032 ret;

    //
    // Get access to specified Device Node in the registry.
    //
    if (VMM_RegCreateKey(HKEY_LOCAL_MACHINE, regDevNode, (PVMMHKEY)&hkSW) != 0)
        return (RM_ERROR);
    if (hkSW)
    {
        //
        // Write the specified parameter to the registry. (REG_DWORD = 4)
        //
        cbType = REG_BINARY;
        ret = VMM_RegSetValueEx(hkSW, regParmStr, NULL, cbType, Data, cbLen);
        VMM_RegCloseKey(hkSW);
        if (ret != 0L)
            return (RM_ERROR);
    }
    return (RM_OK);
} // end of osWriteRegistryBinary

RM_STATUS osReadRegistryDword
(
    PHWINFO pDev,
    char *regDevNode,
    char *regParmStr,
    U032 *Data
)
{
    U032 cbLen;

    cbLen = 4;
    return osReadRegistryBinary(pDev,regDevNode, regParmStr, (U008 *)Data, &cbLen);
} // end of osReadRegistryDword

RM_STATUS osWriteRegistryDword
(
    PHWINFO pDev,
    char *regDevNode,
    char *regParmStr,
    U032 Data
)
{
    U032 cbLen = 4;
    return osWriteRegistryBinary(pDev,regDevNode, regParmStr, (U008 *)&Data, cbLen);
}

RM_STATUS osDeleteRegistryValue
(
    char *regDevNode,
    char *regParmStr
)
{
    U032 hkSW;
    RM_STATUS status;

    if (VMM_RegOpenKey(HKEY_LOCAL_MACHINE, regDevNode, (PVMMHKEY)&hkSW) != 0)
        return (RM_ERROR);

    if (hkSW) {
        status = VMM_RegDeleteValue(hkSW, regParmStr);
        VMM_RegCloseKey(hkSW);
        return status;
    }
}
    
// We have found that during shutdown (and other instances) Win98 disables the memory mapping
// of the device. This procedure is a workaround for this problem. 
// It is assumed that the global DeviceInstance is set to indicate the desired device.
VOID osEnsureDeviceEnabled(PHWINFO pDev)
{
    U032 bar0_address, pci_address, pci_reg, i;

    //Check if we're in powerdown otherwise we would hang.
    if (pDev->Power.State == MC_POWER_LEVEL_2)
    {
        DBG_BREAKPOINT();
        return;
    }

    bar0_address = pDev->Chip.HalInfo.PciBars[0];
    pci_address = pDev->Mapping.nvBusDeviceFunc;

    if(pDev != NULL && nvAddr->Reg032[0]==0xffffffff)
    {   // The device is disabled.
            
        _asm mov  dx, 0xcf8
        _asm in   eax, dx
        _asm push eax
        _asm mov  eax, pci_address  
        _asm or   eax, 0x80000004 
        _asm out  dx, eax
        _asm mov  dx, 0xcfc
        _asm in   al,dx
        // Enable the memory decode and BusmasterEnable of the PCIO device.
        _asm or   al,6
        _asm out  dx,al
        _asm pop  eax
        _asm mov  dx, 0xcf8
        _asm out  dx, eax
    }

    //Make sure our PCI BAR addresses is what we expect it to be.
    //In some cases we may be moved (which we should not be)
    //or Windows may fail to restore our address. 

    if ((pDev != NULL) && (bar0_address != REG_RD32(NV_PBUS_PCI_NV_4))) 
    {
        for (i = 0, pci_reg = 0x80000010; i < pDev->Chip.HalInfo.TotalPciBars; i++, pci_reg += 4)
        {
            U032 barN_address = pDev->Chip.HalInfo.PciBars[i];

            _asm mov  dx, 0xcf8
            _asm in   eax, dx
            _asm push eax
            _asm mov  eax, pci_address  
            _asm or   eax, pci_reg
            _asm out  dx, eax
            _asm mov  dx, 0xcfc
            _asm mov  eax, barN_address
            _asm out  dx, eax 
            _asm pop  eax
            _asm mov  dx, 0xcf8
            _asm out  dx, eax
        }
    }
}

RM_STATUS osDeviceNameToDeviceClass
(
    char * deviceName,
    U032 * deviceClass
)
{
    RM_STATUS rmStatus = RM_OK;
    U032      deviceIndex;
    U032      deviceNameLen;
    
    switch (*deviceClass) 
    {
        case NV01_DEVICE_0:
        case NV01_DEVICE_1:
        case NV01_DEVICE_2:
        case NV01_DEVICE_3:
        case NV01_DEVICE_4:
        case NV01_DEVICE_5:
        case NV01_DEVICE_6:
        case NV01_DEVICE_7:
            // Nothing to do.
            return RM_OK;
            
        case NV03_DEVICE_XX:
            // Search all the nvinfo structures for a match on device name.
            // All secondary displays will have a non null string in pDev.
            deviceNameLen = osStringLength(deviceName);
            if ((deviceName[deviceNameLen - 1] == '1')  ||
                (deviceName[deviceNameLen - 1] == 'y')  ||
                (deviceName[deviceNameLen - 1] == 'Y'))
            {
                // This must be the Primary display.
                *deviceClass = NV01_DEVICE_0;
                return RM_OK;
            }
            
            for (deviceIndex = 0; deviceIndex < MAX_INSTANCE; deviceIndex++)
            {
                if (NvDBPtr_Table[deviceIndex] != 0)
                {
					PWIN9XHWINFO pOsHwInfo = (PWIN9XHWINFO) &NvDBPtr_Table[deviceIndex];

                    // This device is allocated. 
                    if ((pOsHwInfo->osDeviceName != 0)  &&
                        (osStringCompare(pOsHwInfo->osDeviceName, deviceName)))
                    {
                        // found a match.
                        *deviceClass = deviceIndex + NV01_DEVICE_0;
                        return RM_OK;   
                    }
                } // Device is allocated
            }
            
            // Could not find a match
            return RM_ERROR;
                
        default:
            // invalid device class.
            return RM_ERROR;
    }
}

//
// Read a data word from Priveledged IO
//
// Entry: U032 addr                // Address of controller register
//        U008 reg                 // CR/GR/SR register to read
//
// Exit:  index/data pair
//
U016 ReadIndexedpDev(PHWINFO pDev, U032 addr, U008 reg)
{      
    U016 datum;
              
    //
    // Return the data in the upper byte, index in the lower
    //
    REG_WR08(addr, reg);
    datum = (REG_RD08(addr+1) << 8) & 0xFF00;
    datum |= reg;
    
    return(datum);
}                          


//
// Function call entry to reach the video (VGA) BIOS for various needs.  In general you
// should not rely on, or attempt to call, the BIOS for any support.  Many systems do
// not even carry a copy of the video BIOS in memory.
//
// Be very very sure you need to make such calls, even if they are possible.
//
RM_STATUS osCallVideoBIOS
(
    PHWINFO pDev,
    U032    *pEAX,
    U032    *pEBX,
    U032    *pECX,
    U032    *pEDX,
    VOID    *pBuffer
)
{

    CLIENT_STRUCT     crs;

    // 
    // If no buffer data to pass, use standard Int10h interface
    //
    if (pBuffer == NULL)
    {
        // only care about the lower 16bits
        crs.CWRS.Client_AX = (U016)*pEAX;
        crs.CWRS.Client_BX = (U016)*pEBX;
        crs.CWRS.Client_CX = (U016)*pECX;
        crs.CWRS.Client_DX = (U016)*pEDX;
        VBE(&crs);
        *pEAX = (U032)crs.CWRS.Client_AX;
        *pEBX = (U032)crs.CWRS.Client_BX;
        *pECX = (U032)crs.CWRS.Client_CX;
        *pEDX = (U032)crs.CWRS.Client_DX;
    } 
    else
    {
        //
        // Otherwise construct data-passing interface
        //
        crs.CWRS.Client_AX = (U016)*pEAX;
        crs.CWRS.Client_BX = (U016)*pEBX;
        crs.CWRS.Client_CX = (U016)*pECX;
        crs.CWRS.Client_DX = (U016)*pEDX;
        crs.CRS.Client_ES = 0x0000;         // flat for now
        crs.CRS.Client_EDI = (U032)pBuffer;
        readToshibaEDID(&crs);              // special case call for EDID; really should
                                            // convert to common call!!!
        *pEAX = (U032)crs.CWRS.Client_AX;
        *pEBX = (U032)crs.CWRS.Client_BX;
        *pECX = (U032)crs.CWRS.Client_CX;
        *pEDX = (U032)crs.CWRS.Client_DX;
    }
                            

    if (crs.CWRS.Client_AX == 0x004F)
        return RM_OK;
    else
        return (RM_STATUS)(crs.CWRS.Client_AX);

}

/*

// KEEP AROUND FOR JUST A BIT LONGER

U008 convToToshibaDispMask(PHWINFO pDev, U032 disptype)
{
    if (disptype == DISPLAY_TYPE_FLAT_PANEL) return 1;
    else if (disptype == DISPLAY_TYPE_MONITOR) return 2;
    else if (disptype == DISPLAY_TYPE_TV) return 4;

    return 0;
}


// Function: osGenerateInt10h
//
// Generalized mechanism to pass parameters to VGA BIOS with an int10h call.
// Added to allow communication with the System BIOS (using VGA BIOS as a
// pass-through) in (Toshiba) mobile systems.
// 
// The creation of the display masks loaded into Dl and DH for the DEV_CTRL
// events has not yet been tested, though the other parameters for those calls
// have been.
//
// This function in currently only using the EDID read.  Note that this is
// a Win9x only mechanism -- the most likely final implementation will be
// a custom int10h that causes our BIOS to put the EDID somewhere in our
// frame buffer memory.
//
// Parameters:  pDev
//              event: an event code (defined in os.h)
//              Head: head (0 or 1) -- ignored for EDID read
//              param1: event-specific parameter
//                  (for TOSHIBA_DEV_CTRL_BEGIN, the display this head is switching to
//                   -- 0 if it is being disabled)
//              pEDID: pointer to a 128 byte buffer to copy the EDID into
//
RM_STATUS  osGenerateInt10h(PHWINFO pDev, U032 event, U032 Head, U032 param1, VOID* pEDID)
{
    CLIENT_STRUCT     crs;

    if (!pDev->Power.MobileOperation) return RM_OK;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: making int10h call for event:", event);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: making int10h call for head:", Head);

    crs.CRS.Client_ES = 0x0000;
    crs.CWRS.Client_CX = 0x0000;
    crs.CWRS.Client_DX = 0x0000;

    switch(event) {
    case TOSHIBA_READ_EDID:
        crs.CWRS.Client_AX = 0x4F15;
        crs.CWRS.Client_BX = 0x0001;
        crs.CWRS.Client_CX = 0;  //(U016)Head;  //'controller number' == head, or is primary always 0?
        crs.CWRS.Client_DX = 0x0000;    //EDID block number?
        crs.CRS.Client_EDI = (U032)pEDID;
//        VBE(&crs);
        readToshibaEDID(&crs);
        break;
    case TOSHIBA_DEV_CTRL_BEGIN:
        crs.CWRS.Client_AX = 0x4F14;
        crs.CWRS.Client_BX = 0x028F;
//        crs.CWRS.Client_DX = 0x0202;    //temp
        crs.CBRS.Client_DL = convToToshibaDispMask(pDev, GETDISPLAYTYPE(pDev, Head))
            | convToToshibaDispMask(pDev, GETDISPLAYTYPE(pDev, Head^1));
        crs.CBRS.Client_DH = convToToshibaDispMask(pDev, param1)
            | convToToshibaDispMask(pDev, GETDISPLAYTYPE(pDev, Head^1));
        VBE(&crs);
        break;
    case TOSHIBA_DEV_CTRL_END:
        crs.CWRS.Client_AX = 0x4F14;
        crs.CWRS.Client_BX = 0x038F;
        crs.CBRS.Client_DL = convToToshibaDispMask(pDev, GETDISPLAYTYPE(pDev, Head))
            | convToToshibaDispMask(pDev, GETDISPLAYTYPE(pDev, Head^1));
        crs.CBRS.Client_DH = 0;
        VBE(&crs);
        break;
    }

    if (crs.CWRS.Client_AX == 0x004F)
        return RM_OK;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: int10h call returned error:", crs.CWRS.Client_AX);

    return RM_ERROR;
}
*/


#if DEBUG
//---------------------------------------------------------------------------
//
//  Debugging support.
//
//---------------------------------------------------------------------------

// The current debug display level 
//
// Best to leave it above setup and trace messages, but below warnings and error messages
//
int cur_debuglevel = DBG_LEVEL_DEFAULTS;    // all modules set to warning level

int dbgObject_inuse = DBG_OBJECT_OS; 
   
DBGOBJECT DbgObject[] = { 
    {dbgConfigNull, dbgInitNull, dbgOpenNull, vmmOutDebugString, dbgCloseNull},     // Standard Debug
    {dbgConfigNull, dbgInitSerial, dbgOpenNull, dbgDisplaySerial, dbgCloseNull},    // Serial
    {dbgConfigNull, dbgInitNull, dbgOpenNull, vmmOutDebugString, dbgCloseNull},     // Parallel
    {dbgConfigNull, dbgInitNull, dbgOpenNull, vmmOutDebugString, dbgCloseNull},     // Infrared
    {dbgConfigNull, dbgInitNull, dbgOpenNull, vmmOutDebugString, dbgCloseNull},     // USB
    {dbgConfigNull, dbgInitNull, dbgOpenNull, vmmOutDebugString, dbgCloseNull},     // I2C
};

VOID osDbgBreakPoint
(
    void
)
{
    dbgBreakPoint();
}

VOID osDbgPrintf_va(
    int     module,
    int     debuglevel,
    const char *printf_format,
    va_list arglist)
{
    int     moduleValue = cur_debuglevel;
    char    nv_error_string[MAX_ERROR_STRING];
    char    *p = nv_error_string;
    int     chars_written;

    // get the module
    // put the module we want in the first 2 bits
    moduleValue = moduleValue >> (module * 2);

    // get the value of those two bits
    // mask off the last 30 bits
    moduleValue = moduleValue & 0x00000003;

   if (debuglevel >= moduleValue)
   {
        chars_written = nvDbgSprintf(p, printf_format, arglist);

        DbgObject[dbgObject_inuse].pDisplay(p);
   }
}

#endif // DEBUG
